#ifndef INT_EXTENSION_HPP
#define INT_EXTENSION_HPP

#include <algorithm>
#include <cstdint>

namespace autflr {
    constexpr float ONE_HUNDRED_PERC = 100.0f;

    constexpr float mapToPercentage(
        uint16_t raw,
        uint16_t min,
        uint16_t max,
        bool inverted = false
    ) {
        if (min == max) {
            return 0.0f;
        }

        raw = std::clamp(raw, min, max);
        uint16_t numerator = 0;
        uint16_t denominator = max - min;

        if (inverted) {
            numerator = max - raw;
        } else {
            numerator = raw - min;
        }

        return static_cast<float>(numerator) / denominator * ONE_HUNDRED_PERC;
    }
}

#endif