#include "I2cDeviceFactory.hpp"
#include "Lcd.hpp"
#include "SensorFactory.hpp"
#include "WiFiManager.hpp"
#include "IntExtension.hpp"

#include "esp_cpu.h"
#include "esp_event.h"
#include "esp_exception.hpp"
#include "esp_pthread.h"
#include "esp_netif_sntp.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "gpio_cxx.hpp"

#include <chrono>
#include <ctime>
#include <thread>

#define CORE0 0x00
#define CORE1 0x01
#define LCD_ADDRESS 0x27
#define PUMP_POWER_PIN 33
#define SENSOR_POWER_PIN 32
#define WARNING_LED_PIN 25

namespace autflr {
    constexpr const char* APP_TAG = "[IRRIGATION]";
    constexpr const char* NTP_TAG = "[NTP]";
    constexpr const char* SENSOR_TAG_MOISTURE = "[MOISTURE SENSOR]";
    constexpr const char* SENSOR_TAG_WATER = "[WATER SENSOR]";

    constexpr uint32_t STACK_SIZE = 4 * 1024;

    // Constants that require customisation by the user
    constexpr uint16_t MIN_MAP_MOISTURE = 400; // Floor for mapping "raw" moisture sensor's value
    constexpr uint16_t MAX_MAP_MOISTURE = 820; // Ceil for mapping "raw" moisture sensor's value
    constexpr uint16_t MIN_MAP_WATER = 100; // Floor for mapping "raw" water sensor's value
    constexpr uint16_t MAX_MAP_WATER = 495; // Ceil for mapping "raw" water sensor's value
    constexpr uint16_t MIN_LEVEL_WATER = 300;
    constexpr uint16_t MIN_LEVEL_MOISTURE = 715;

    constexpr const char* WARNING_MESSAGE = "Low water level!";

    constexpr uint16_t SENSOR_WARM_UP_TIME = 10; // "Warm-up" time for stabilization of sensors.
    constexpr uint16_t TARGET_HOUR = 18; // Hour to start the irrigation.
    constexpr uint16_t TARGET_MINUTES = 00; // Minutes to start the irrigation.
    constexpr uint32_t NEXT_DAY = 24 * 60 * 60; // Next day for start the irrigation.
    constexpr uint16_t PUMPING_TIME = 20;

    /* FreeRTOS event group to signal when we are connected*/
    static EventGroupHandle_t wifiEventGroup;

    auto& sensorFactory = autflr::SensorFactory::getInstance();
    auto& i2cDeviceFactory = autflr::I2cDeviceFactory::getInstance();
    auto& wifiManager = autflr::WiFiManager::getInstance();

    esp_pthread_cfg_t create_cfg(
        const char* name,
        uint16_t coreId,
        size_t stack,
        size_t priority
    ) {
        auto cfg = esp_pthread_get_default_config();
        cfg.thread_name = name;
        cfg.pin_to_core = coreId;
        cfg.stack_size = stack;
        cfg.prio = priority;

        return cfg;
    }

    /** Starts the irrigation process. */
    void irrigate() {
        auto lcdDevice = i2cDeviceFactory.createDevice<autflr::Lcd>(I2C_ADDR_BIT_LEN_7, LCD_ADDRESS);

        if (!lcdDevice) {
            ESP_LOGE(APP_TAG, "Failed to initialize LCD device.");
            return;
        }

        auto moistureSensor = sensorFactory.createSensorOneShot(SENSOR_TAG_MOISTURE, ADC_UNIT_1, ADC_CHANNEL_6);
        auto waterSensor = sensorFactory.createSensorOneShot(SENSOR_TAG_WATER, ADC_UNIT_1, ADC_CHANNEL_7);

        if (!moistureSensor || !waterSensor) {
            ESP_LOGE(APP_TAG, "Failed to initialize sensors.");
            return;
        }

        auto sensorPower = std::make_unique<idf::GPIO_Output>(idf::GPIONum(SENSOR_POWER_PIN));
        auto pumpPower = std::make_unique<idf::GPIO_Output>(idf::GPIONum(PUMP_POWER_PIN));
        auto warningLed = std::make_unique<idf::GPIO_Output>(idf::GPIONum(WARNING_LED_PIN));

        warningLed->set_low();
        lcdDevice->clear();
        sensorPower->set_high();
        std::this_thread::sleep_for(std::chrono::seconds(SENSOR_WARM_UP_TIME)); // Sensor stabilisation.

        auto moisture = moistureSensor->getValueRaw();
        auto moistureConverted = mapToPercentage(moisture, MIN_MAP_MOISTURE, MAX_MAP_MOISTURE, true);
        auto waterLevel = waterSensor->getValueRaw();
        auto waterLevelConverted = mapToPercentage(waterLevel, MIN_MAP_WATER, MAX_MAP_WATER);

        lcdDevice->print(std::format("{}{:.1f}%", "Moisture:", moistureConverted), 0, 0);
        lcdDevice->print(std::format("{}{:.1f}%", "Water:", waterLevelConverted), 1, 0);
        ESP_LOGI(
            APP_TAG,
            "Moisture:%.1f%%(%d), Water level: %.1f%%(%d)",
            moistureConverted,
            moisture,
            waterLevelConverted,
            waterLevel
        );

        if (moisture >= MIN_LEVEL_MOISTURE) {
            if (waterLevel <= MIN_LEVEL_WATER) {
                ESP_LOGW(APP_TAG, "%s", WARNING_MESSAGE);
                lcdDevice->clear();
                lcdDevice->print(WARNING_MESSAGE, 0, 0);
                warningLed->set_high();
            } else {
                pumpPower->set_high();
                std::this_thread::sleep_for(std::chrono::seconds(PUMPING_TIME)); // Pump operating time.
                pumpPower->set_low();

                // TODO REFACTORING!
                moisture = moistureSensor->getValueRaw();
                moistureConverted = mapToPercentage(moisture, MIN_MAP_MOISTURE, MAX_MAP_MOISTURE, true);
                waterLevel = waterSensor->getValueRaw();
                waterLevelConverted = mapToPercentage(waterLevel, MIN_MAP_WATER, MAX_MAP_WATER);

                lcdDevice->print(std::format("{}{:.1f}%", "Moisture:", moistureConverted), 0, 0);
                lcdDevice->print(std::format("{}{:.1f}%", "Water:", waterLevelConverted), 1, 0);

                ESP_LOGI(APP_TAG, "Irrigation process completed.");
            }
        } else {
            ESP_LOGI(APP_TAG, "No irrigation needed.");
        }

        sensorPower->set_low();
    }

    /**
     * Calculates the time until the next start in microseconds.
     * @param targetHour Hour of launch.
     * @param targetMinute Minutes of launch.
     * @return Time to start in microseconds.
     */
    uint64_t calculateTimeToNextRun(uint16_t targetHour, uint16_t targetMinute) {
        std::time_t now;
        std::tm timeInfo;

        std::time(&now);
        setenv("TZ", "GMT-3", 1);
        tzset();
        localtime_r(&now, &timeInfo);

        timeInfo.tm_sec = 0;
        timeInfo.tm_min = targetMinute;
        timeInfo.tm_hour = targetHour;

        std::time_t targetTime = std::mktime(&timeInfo);

        if (targetTime <= now) {
            targetTime += NEXT_DAY;
        }

        ESP_LOGI(APP_TAG, "Current time: %s", std::asctime(std::localtime(&now)));
        ESP_LOGI(APP_TAG, "Target time: %s", std::asctime(std::localtime(&targetTime)));

        return static_cast<uint64_t>((targetTime - now) * 1000000ULL); // us.
    }

    /** Sets the current time by initialize a particular SNTP server and also starting the SNTP service.*/
    void syncTime() {
        ESP_LOGI(NTP_TAG, "Initializing SNTP...");

        esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

        esp_netif_sntp_init(&config);
        if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) != ESP_OK) {
            ESP_LOGE(NTP_TAG, "Failed to update system time within 10s timeout");
        }
    }

    static void eventHandler(
        void* arg,
        esp_event_base_t base,
        int32_t event_id,
        void* event_data
    ) {
        if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
            wifiManager.connect();
        } else if (base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
            if (wifiManager.getRetryNum() < WIFI_MAXIMUM_RETRY) {
                wifiManager.connect();
            } else {
                xEventGroupSetBits(wifiEventGroup, WIFI_FAIL_BIT);
            }
            ESP_LOGI(APP_TAG,"connect to the AP fail");
        } else if (base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(APP_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            wifiManager.resetRetry();
            xEventGroupSetBits(wifiEventGroup, WIFI_CONNECTED_BIT);
        }
    }

    void initializeWifi() {
        wifiEventGroup = xEventGroupCreate();
        
        wifiManager.configure("MERCUSYS_37B4", "ssau_2015");

        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        ESP_ERROR_CHECK(
            esp_event_handler_instance_register(
                WIFI_EVENT,                                           
                ESP_EVENT_ANY_ID,                              
                &eventHandler,
                nullptr,
                &instance_any_id
            )
        );
        ESP_ERROR_CHECK(
            esp_event_handler_instance_register(
                IP_EVENT,
                IP_EVENT_STA_GOT_IP,
                &eventHandler,
                nullptr,
                &instance_got_ip
            )
        );

        wifiManager.start();
    }

    void launch() {
        initializeWifi();
        syncTime();

        auto timeToNextRun = calculateTimeToNextRun(TARGET_HOUR, TARGET_MINUTES);

        irrigate();

        ESP_LOGI(APP_TAG, "Scheduling next run in %llu seconds.", timeToNextRun / 1000000ULL);
        esp_deep_sleep(timeToNextRun);
    }
}

extern "C" void app_main(void) {
    ESP_LOGI(autflr::APP_TAG, "Application started.");
    autflr::launch();
}
