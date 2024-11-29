#ifndef I2C_SLAVE_DEVICE_HPP
#define I2C_SLAVE_DEVICE_HPP

#include "driver/i2c_master.h"

#include <memory>

namespace autflr {
    class I2cSlaveDevice {
    public:
        I2cSlaveDevice(i2c_master_dev_t* pHandler) : mHandler{std::move(pHandler)} {}
        virtual ~I2cSlaveDevice() = default;

    protected:
        virtual void initialize() const = 0;

        struct DeviceHandlerDeleter {
            void operator()(i2c_master_dev_t* pHandler) {
                if (pHandler) {
                    ESP_ERROR_CHECK(i2c_master_bus_rm_device(pHandler));
                }
            }
        };
        std::unique_ptr<i2c_master_dev_t, DeviceHandlerDeleter> mHandler{nullptr};
    };
}

#endif