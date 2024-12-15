#ifndef MEASURE_CONSTANTS_HPP
#define MEASURE_CONSTANTS_HPP

#include <cstdint>
#include <string_view>

namespace autflr {
    // Messages
    constexpr std::string_view WARNING_MESSAGE = "Low water level!";

    // Sensor's constants
    constexpr uint16_t MIN_MAP_MOISTURE = 400; // Floor for mapping "raw" moisture sensor's value
    constexpr uint16_t MAX_MAP_MOISTURE = 820; // Ceil for mapping "raw" moisture sensor's value
    constexpr uint16_t MIN_MAP_WATER = 100; // Floor for mapping "raw" water sensor's value
    constexpr uint16_t MAX_MAP_WATER = 495; // Ceil for mapping "raw" water sensor's value
    constexpr uint16_t MIN_LEVEL_WATER = 300;
    constexpr uint16_t MIN_LEVEL_MOISTURE = 715;

    // Time constants
    constexpr uint16_t SENSOR_WARM_UP_TIME = 10; // Time in seconds for sensor stabilization.
    constexpr uint16_t TARGET_HOUR = 18; // Hour to start the irrigation.
    constexpr uint16_t TARGET_MINUTES = 00; // Minutes to start the irrigation.
    constexpr uint32_t NEXT_DAY = 24 * 60 * 60; // Next day for start the irrigation.
    constexpr uint16_t PUMPING_TIME = 20;
    constexpr uint16_t SNTP_TIMEOUT = 10000;

}

#endif