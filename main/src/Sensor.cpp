#include "Sensor.hpp"

namespace autflr {
    Sensor::Sensor(
        const std::string& tag,
        adc_oneshot_unit_ctx_t* pHandler,
        adc_cali_scheme_t* pCaliHandler,
        adc_channel_t channel
    ) : mTag{tag}, mAdcHandler{pHandler}, mCaliHandler{pCaliHandler}, mChannel{channel} {}

    uint16_t Sensor::getValueRaw() const {
        int value = 0;
        
        if (adc_oneshot_read(mAdcHandler, mChannel, &value) != ESP_OK) {
            ESP_LOGE(mTag.c_str(), "Failed to read from ADC");
        } else {
            mValue = static_cast<uint16_t>(value);
        }

        return mValue;
    }

    uint16_t Sensor::getValueCalibrated() const {
        int value = 0;

        if (adc_oneshot_get_calibrated_result(mAdcHandler, mCaliHandler, mChannel, &value) != ESP_OK) {
            ESP_LOGE(mTag.c_str(), "Failed to read calibrated value from ADC");
        } else {
            mValue = static_cast<uint16_t>(value);
        }

        return mValue;
    }

}