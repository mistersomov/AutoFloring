#ifndef WIFI_MANAGER_HPP
#define WIFI_MANAGER_HPP

#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_handle.hpp"

#include <cstdint>
#include <string>
#include <cstring>
#include <memory>

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP.
 * - we failed to connect after the maximum amount of retries.
 */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

namespace autflr {
    constexpr uint16_t WIFI_MAXIMUM_RETRY = 5;
    const std::string WIFI_SSID = "WIFI_SSID";
    const std::string WIFI_BSSID = "WIFI_BSSID";
    constexpr size_t MAX_SSID_LENGTH = 32;
    constexpr size_t MAX_BSSID_LENGTH = 64;

    struct WiFiCredentials {
        bool isLoaded;
        std::string ssid;
        std::string bssid;
    };

    class WiFiManager {
    public:
        WiFiManager(const WiFiManager&) = delete;
        WiFiManager& operator=(const WiFiManager&) = delete;
        ~WiFiManager() {
            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_stop());
            ESP_ERROR_CHECK(esp_wifi_deinit());
        }

        static WiFiManager& getInstance() {
            static WiFiManager instance;
            return instance;
        }

        inline void configure(const std::string& ssid, const std::string& bssid) {
            esp_err_t ret = nvs_flash_init();

            if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                ESP_ERROR_CHECK(nvs_flash_erase());
                ret = nvs_flash_init();
            }

            ESP_ERROR_CHECK(ret);

            mCreds = loadCredentials();

            if (mCreds->isLoaded) {
                if (mCreds->ssid != ssid || mCreds->bssid != bssid) {
                    saveCredentials(ssid, bssid);
                }

                ESP_LOGI(TAG.c_str(), "Configuring Wi-Fi with SSID: %s", ssid.c_str());
                ESP_ERROR_CHECK(esp_netif_init());
                ESP_ERROR_CHECK(esp_event_loop_create_default());
                esp_netif_create_default_wifi_sta();

                wifi_init_config_t initCfg = WIFI_INIT_CONFIG_DEFAULT();
                ESP_ERROR_CHECK(esp_wifi_init(&initCfg));
            }
        }

        inline void start() const {
            wifi_config_t wifiConfig = {};
            strncpy(reinterpret_cast<char*>(wifiConfig.sta.ssid), mCreds->ssid.c_str(), MAX_SSID_LENGTH);
            strncpy(reinterpret_cast<char*>(wifiConfig.sta.password), mCreds->bssid.c_str(), MAX_BSSID_LENGTH);
            wifiConfig.sta.ssid[MAX_SSID_LENGTH - 1] = '\0';
            wifiConfig.sta.password[MAX_BSSID_LENGTH - 1] = '\0';

            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifiConfig));
            ESP_ERROR_CHECK(esp_wifi_start());
        }

        inline void connect() const {
            esp_wifi_connect();
        }

        inline void reconnect() {
            if (mRetryNum < WIFI_MAXIMUM_RETRY) {
                connect();
                mRetryNum++;
                ESP_LOGI(TAG.c_str(), "retry to connect to the AP");
            }
        }

        inline uint16_t getRetryNum() const {
            return mRetryNum;
        }

        inline void resetRetry() {
            mRetryNum = 0;
        }

    private:
        WiFiManager() {}

        inline std::unique_ptr<WiFiCredentials> loadCredentials() {
            esp_err_t ret = ESP_OK;
            std::unique_ptr<nvs::NVSHandle> handler = nvs::open_nvs_handle("storage", NVS_READWRITE, &ret);

            if (ret != ESP_OK) {
                ESP_LOGE(TAG.c_str(), "Failed to open NVS: %s", esp_err_to_name(ret));
                return std::make_unique<WiFiCredentials>();
            } else {
                char ssid[MAX_SSID_LENGTH] = {0};
                char bssid[MAX_BSSID_LENGTH] = {0};
                size_t ssidLen = sizeof(ssid);
                size_t bssidLen = sizeof(bssid);

                if ((ret = handler->get_string(WIFI_SSID.c_str(), ssid, ssidLen)) != ESP_OK
                    || (ret = handler->get_string(WIFI_BSSID.c_str(), bssid, bssidLen)) != ESP_OK
                ) {
                    ESP_LOGW(TAG.c_str(), "No Wi-Fi credentials found: %s", esp_err_to_name(ret));
                    return std::make_unique<WiFiCredentials>(true, "", "");
                } else {
                    ESP_LOGI(TAG.c_str(), "Wi-Fi credentials found.");
                    return std::make_unique<WiFiCredentials>(true, std::string(ssid, ssidLen), std::string(bssid, bssidLen));
                }
            }
        }

        inline void saveCredentials(const std::string& ssid, const std::string& bssid) {
            esp_err_t ret = ESP_OK;
            std::unique_ptr<nvs::NVSHandle> handler = nvs::open_nvs_handle("storage", NVS_READWRITE, &ret);

            if (ret != ESP_OK) {
                ESP_LOGE(TAG.c_str(), "Failed to open NVS handle: %s", esp_err_to_name(ret));
                return;
            }
            if ((ret = handler->set_string(WIFI_SSID.c_str(), ssid.c_str())) != ESP_OK) {
                ESP_LOGE(TAG.c_str(), "Failed to save SSID: %s", esp_err_to_name(ret));
            }
            if ((ret = handler->set_string(WIFI_BSSID.c_str(), bssid.c_str())) != ESP_OK) {
                ESP_LOGE(TAG.c_str(), "Failed to save BSSID: %s", esp_err_to_name(ret));
            }
            if ((ret = handler->commit()) != ESP_OK) {
                ESP_LOGE(TAG.c_str(), "Failed to commit changes: %s", esp_err_to_name(ret));
            }
        }

    private:
        uint16_t mRetryNum{0};
        std::unique_ptr<WiFiCredentials> mCreds{nullptr};
        constexpr static const std::string TAG{"[WIFI]"};
    };
}

#endif