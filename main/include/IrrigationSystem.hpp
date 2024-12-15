#ifndef IRRIGATION_SYSTEM_HPP
#define IRRIGATION_SYSTEM_HPP

#include "WiFiManager.hpp"

#include "esp_event_cxx.hpp"
#include "sdkconfig.h"

#include <memory>
#include <string_view>

#if CONFIG_ENABLE_LCD
#include "Lcd.hpp"
#define LCD_ADDRESS 0x27
#endif

namespace autflr {
    class IrrigationSystem {
    public:
        IrrigationSystem(const IrrigationSystem&) = delete;
        IrrigationSystem& operator=(const IrrigationSystem&) = delete;

        static IrrigationSystem& getInstance() {
            static IrrigationSystem instance;
            return instance;
        }

        void launch();
        void openSettings();

    private:
        IrrigationSystem();

        /** @brief Starts the irrigation process.*/
        void irrigate() const;
        void launchWiFi() const;
        /** @brief Sets the current time by initialize a particular SNTP server and also starting the SNTP service.*/
        void syncTime() const;

    private:
        idf::event::ESPEventLoop mLoop;
        WiFiManager& mWiFiManager;
        static constexpr std::string_view TAG = "[IRRIGATION]";
        static constexpr std::string_view SENSOR_TAG_MOISTURE = "[MOISTURE SENSOR]";
        static constexpr std::string_view SENSOR_TAG_WATER = "[WATER SENSOR]";
        static constexpr std::string_view NTP_TAG = "[NTP]";
    };
}

#endif