#ifndef ANALOG_SENSOR_HPP
#define ANALOG_SENSOR_HPP

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

#include <cstdint>
#include <string>

namespace autflr {
    class Sensor {
    public:
        Sensor(
            const std::string& tag,
            adc_oneshot_unit_ctx_t* pHandler,
            adc_cali_scheme_t* pCaliHandler,
            adc_channel_t channel
        );

        virtual uint16_t getValueRaw() const;
        virtual uint16_t getValueCalibrated() const;

    private:
        std::string mTag;
        adc_oneshot_unit_ctx_t* mAdcHandler{nullptr};
        adc_cali_scheme_t* mCaliHandler{nullptr};
        adc_channel_t mChannel;
        mutable uint16_t mValue{0};
    };
}

#endif