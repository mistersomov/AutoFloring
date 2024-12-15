#ifndef IRRIGATION_SYSTEM_HPP
#define IRRIGATION_SYSTEM_HPP

#include "I2cDeviceFactory.hpp"
#include "SensorFactory.hpp"
#include "WiFiManager.hpp"

#include "sdkconfig.h"

#include <memory>
#include <string_view>
#include <functional>

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

    private:
        IrrigationSystem();

        void launchWiFi() const;
        void registerEventHandlers();
        static void handleEvent(
            void* arg,
            esp_event_base_t event_base,
            int32_t event_id,
            void* event_data
        );
        void syncTime() const;
        void irrigate() const;
        void scheduleNextLaunch() const;
        /**
         * Calculates the time until the next start in microseconds.
         * @param targetHour Hour of launch.
         * @param targetMinute Minutes of launch.
         * @return Time to start in microseconds.
         */
        uint64_t calculateNextTime(uint16_t targetHour, uint16_t targetMinute) const;
        void openSettings() const;

    private:
        idf::event::ESPEventLoop mLoop;

        I2cDeviceFactory& mI2cDeviceFactory;
        SensorFactory& mSensorFactory;
        WiFiManager& mWiFiManager;

        static constexpr std::string_view TAG = "[IRRIGATION]";
        static constexpr std::string_view SENSOR_TAG_MOISTURE = "[MOISTURE SENSOR]";
        static constexpr std::string_view SENSOR_TAG_WATER = "[WATER SENSOR]";
        static constexpr std::string_view NTP_TAG = "[NTP]";
    };
}

#endif