#ifndef SENSOR_FACTORY_HPP
#define SENSOR_FACTORY_HPP

#include "Sensor.hpp"

#include <memory>
#include <string>

namespace autflr {
    class SensorFactory {
    public:
        SensorFactory(const SensorFactory&) = delete;
        SensorFactory& operator=(const SensorFactory&) = delete;

        static SensorFactory& getInstance() {
            static SensorFactory instance;
            return instance;
        }

        std::unique_ptr<Sensor> createSensorOneShot(
            const std::string& tag,
            adc_unit_t adcUnit,
            adc_channel_t channel
        ) {
            if (adcUnit == ADC_UNIT_1) {
                if (mHandlerOneShot1.get() == nullptr) {
                    setHandler(ADC_UNIT_1);
                }
                if (mCaliHandler1.get() == nullptr) {
                    setCalibrationScheme(ADC_UNIT_1);
                }
                ESP_ERROR_CHECK(
                    adc_oneshot_config_channel(mHandlerOneShot1.get(), channel, &mChannelCfgOneShot)
                );

                return std::make_unique<Sensor>(
                    tag,
                    mHandlerOneShot1.get(),
                    mCaliHandler1.get(),
                    channel
                );
            } else {
                if (mHandlerOneShot2.get() == nullptr) {
                    setHandler(ADC_UNIT_2);
                }
                ESP_ERROR_CHECK(
                    adc_oneshot_config_channel(mHandlerOneShot2.get(), channel, &mChannelCfgOneShot)
                );

                return std::make_unique<Sensor>(
                    tag,
                    mHandlerOneShot2.get(),
                    mCaliHandler2.get(),
                    channel
                );
            }
        }

    private:
        SensorFactory() : mChannelCfgOneShot{.atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_10},
                          mTag{"SensorFactoryTag"}
        {

        }

        void setHandler(adc_unit_t adcUnit) {
            if (adcUnit == ADC_UNIT_1) {
                adc_oneshot_unit_init_cfg_t cfg1{
                    .unit_id = ADC_UNIT_1,
                    .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
                    .ulp_mode = ADC_ULP_MODE_DISABLE,
                };
                adc_oneshot_unit_handle_t handler1 = nullptr;

                ESP_ERROR_CHECK(adc_oneshot_new_unit(&cfg1, &handler1));
                mHandlerOneShot1.reset(handler1);
            } else {
                adc_oneshot_unit_init_cfg_t cfg2{
                    .unit_id = ADC_UNIT_2,
                    .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
                    .ulp_mode = ADC_ULP_MODE_DISABLE,
                };
                adc_oneshot_unit_handle_t handler2 = nullptr;

                ESP_ERROR_CHECK(adc_oneshot_new_unit(&cfg2, &handler2));
                mHandlerOneShot2.reset(handler2);
            }
        }

        void setCalibrationScheme(adc_unit_t adcUnit) {
            adc_cali_scheme_ver_t schemeMask;
            adc_cali_check_scheme(&schemeMask);

            if (schemeMask & ADC_CALI_SCHEME_VER_LINE_FITTING) {
                adc_cali_line_fitting_config_t caliConfig = {
                    .unit_id = adcUnit,
                    .atten = ADC_ATTEN_DB_12,
                    .bitwidth = ADC_BITWIDTH_10,
                };
                adc_cali_handle_t handler;

                ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&caliConfig, &handler));
                if (adcUnit == ADC_UNIT_1) {
                    mCaliHandler1.reset(handler);
                } else {
                    mCaliHandler2.reset(handler);
                }
            } else {
                ESP_LOGE(mTag.c_str(), "Calibration scheme not found");
                return;
            }
        }

    private:
        struct AdcHandlerDeleter {
            void operator()(adc_oneshot_unit_ctx_t* pHandler) const {
                if (pHandler) {
                    ESP_ERROR_CHECK(adc_oneshot_del_unit(pHandler));
                }
            }
        };
        struct CaliHandlerDeleter {
            void operator()(adc_cali_scheme_t* pHandler) {
                if (pHandler) {
                    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(pHandler));
                }
            }
        };
        std::unique_ptr<adc_oneshot_unit_ctx_t, AdcHandlerDeleter> mHandlerOneShot1{nullptr};
        adc_oneshot_chan_cfg_t mChannelCfgOneShot;
        std::unique_ptr<adc_cali_scheme_t, CaliHandlerDeleter> mCaliHandler1{nullptr};
        std::unique_ptr<adc_oneshot_unit_ctx_t, AdcHandlerDeleter> mHandlerOneShot2{nullptr};
        std::unique_ptr<adc_cali_scheme_t, CaliHandlerDeleter> mCaliHandler2{nullptr};
        const std::string mTag;
    };
}

#endif