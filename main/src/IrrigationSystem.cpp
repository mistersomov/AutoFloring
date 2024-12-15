#include "IrrigationSystem.hpp"
#include "I2cDeviceFactory.hpp"
#include "IntExtension.hpp"
#include "MeasureConstants.hpp"
#include "SensorFactory.hpp"

#include "driver/rtc_io.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include <chrono>
#include <ctime>

#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_BSSID CONFIG_WIFI_PASSWORD
#define SENSOR_POWER_PIN CONFIG_SENSOR_POWER_PIN
#define PUMP_POWER_PIN CONFIG_PUMP_PIN
#define WARNING_LED_PIN CONFIG_WARNING_LED_PIN

namespace autflr {
    IrrigationSystem::IrrigationSystem() : mLoop{}, mWiFiManager{WiFiManager::getInstance()} {

    }

    void IrrigationSystem::launch() {
        ESP_LOGI(TAG.data(), "Launching Irrigation System...");

        launchWiFi();
        irrigate();
    }

    void IrrigationSystem::irrigate() const {
        auto& sensorFactory = autflr::SensorFactory::getInstance();
        auto& i2cDeviceFactory = autflr::I2cDeviceFactory::getInstance();
        auto moistureSensor = sensorFactory.createSensorOneShot(SENSOR_TAG_MOISTURE.data(), ADC_UNIT_1, ADC_CHANNEL_6);

        #if CONFIG_ENABLE_WATER_SENSOR
            auto waterSensor = sensorFactory.createSensorOneShot(SENSOR_TAG_WATER.data(), ADC_UNIT_1, ADC_CHANNEL_7);
        #endif
        #if CONFIG_ENABLE_LCD
            auto lcdDevice = i2cDeviceFactory.createDevice<autflr::Lcd>(LCD_ADDRESS);

            if (!lcdDevice) {
                ESP_LOGE(TAG.data(), "Failed to initialize LCD device.");
                return;
            }

            lcdDevice->clear();
            lcdDevice->print("Measuring...", 0, 0);
        #endif

        if (!moistureSensor) {
            ESP_LOGE(TAG.data(), "Failed to initialize moisture sensor.");
            return;
        }
        #if CONFIG_ENABLE_WATER_SENSOR
            if (!waterSensor) {
                ESP_LOGE(TAG.data(), "Failed to initialize water sensor.");
                return;
            }
        #endif

        auto sensorPower = std::make_unique<idf::GPIO_Output>(idf::GPIONum(SENSOR_POWER_PIN));
        auto pumpPower = std::make_unique<idf::GPIO_Output>(idf::GPIONum(PUMP_POWER_PIN));
        auto warningLed = std::make_unique<idf::GPIO_Output>(idf::GPIONum(WARNING_LED_PIN));

        warningLed->set_low();
        sensorPower->set_high();
        std::this_thread::sleep_for(std::chrono::seconds(SENSOR_WARM_UP_TIME)); // Sensor stabilisation.

        auto moisture = moistureSensor->getValueRaw();
        auto moistureConverted = mapToPercentage(moisture, MIN_MAP_MOISTURE, MAX_MAP_MOISTURE, true);
        #if CONFIG_ENABLE_WATER_SENSOR
            auto waterLevel = waterSensor->getValueRaw();
            auto waterLevelConverted = mapToPercentage(waterLevel, MIN_MAP_WATER, MAX_MAP_WATER);
        #endif

        #if CONFIG_ENABLE_LCD
            lcdDevice->print(std::format("{}{:.1f}%", "Moisture:", moistureConverted), 0, 0);
            #if CONFIG_ENABLE_WATER_SENSOR
                lcdDevice->print(std::format("{}{:.1f}%", "Water:", waterLevelConverted), 1, 0);
            #endif
        #endif

        ESP_LOGI(
            TAG.data(),
            "Moisture:%.1f%%(%d)",
            moistureConverted,
            moisture
        );
        #if CONFIG_ENABLE_WATER_SENSOR
            ESP_LOGI(
                TAG.data(),
                "Water level: %.1f%%(%d)",
                waterLevelConverted,
                waterLevel
            );
        #endif

        if (moisture >= MIN_LEVEL_MOISTURE) {
            #if CONFIG_ENABLE_WATER_SENSOR
                if (waterLevel <= MIN_LEVEL_WATER) {
                    ESP_LOGW(TAG.data(), "%s", WARNING_MESSAGE.data());
                    #if CONFIG_ENABLE_LCD
                        lcdDevice->clear();
                        lcdDevice->print(WARNING_MESSAGE.data(), 0, 0);
                    #endif
                    warningLed->set_high();
            } else {
            #endif
                pumpPower->set_high();
                std::this_thread::sleep_for(std::chrono::seconds(PUMPING_TIME)); // Pump operating time.
                pumpPower->set_low();

                // TODO REFACTORING!
                moisture = moistureSensor->getValueRaw();
                moistureConverted = mapToPercentage(moisture, MIN_MAP_MOISTURE, MAX_MAP_MOISTURE, true);
                #if CONFIG_ENABLE_WATER_SENSOR
                    waterLevel = waterSensor->getValueRaw();
                    waterLevelConverted = mapToPercentage(waterLevel, MIN_MAP_WATER, MAX_MAP_WATER);
                #endif
                #if CONFIG_ENABLE_LCD
                    lcdDevice->print(std::format("{}{:.1f}%", "Moisture:", moistureConverted), 0, 0);
                    lcdDevice->print(std::format("{}{:.1f}%", "Water:", waterLevelConverted), 1, 0);
                #endif
                ESP_LOGI(TAG.data(), "Irrigation process completed.");
            #if CONFIG_ENABLE_WATER_SENSOR
            }
            #endif
        } else {
            ESP_LOGI(TAG.data(), "No irrigation needed.");
        }

        sensorPower->set_low();
    }

    void IrrigationSystem::launchWiFi() const {
        mWiFiManager.configure(WIFI_SSID, WIFI_BSSID);
        mWiFiManager.start();
    }

    void IrrigationSystem::openSettings() {

    }

    void IrrigationSystem::syncTime() const {
        ESP_LOGI(NTP_TAG.data(), "Initializing SNTP...");

        esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

        esp_netif_sntp_init(&config);
        if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(SNTP_TIMEOUT)) != ESP_OK) {
            ESP_LOGE(NTP_TAG.data(), "Failed to update system time within 10s timeout");
        }
    }

}