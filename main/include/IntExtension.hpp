#ifndef INT_EXTENSION_HPP
#define INT_EXTENSION_HPP

#include <algorithm>

namespace autflr {
    constexpr float ONE_HUNDRED_PERC = 100.0f;

    constexpr float mapToPercentage(int rawValue, int min, int max) {
        if (min == max) {
            return 0.0f;
        }
        rawValue = std::clamp(rawValue, min, max);
        return (static_cast<float>(rawValue - min) / (max - min)) * ONE_HUNDRED_PERC;
    }
}

#endif