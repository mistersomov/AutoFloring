#ifndef I2C_DEVICE_FACTORY_HPP
#define I2C_DEVICE_FACTORY_HPP

#include "Device.hpp"

#include "driver/gpio.h"

#include <cstdint>

namespace autflr {
    const gpio_num_t I2C_MASTER_SDA_IO = GPIO_NUM_21;
    const gpio_num_t I2C_MASTER_SCL_IO = GPIO_NUM_22;
    constexpr int32_t I2C_PORT_DEFAULT = I2C_NUM_0;
    constexpr uint32_t SCL_SPEED_HZ_DEFAULT = 100000;
    constexpr uint8_t GLITCH_IGNORE_COUNT_DEFAULT = 7;

    class I2cDeviceFactory {
    public:
        I2cDeviceFactory(const I2cDeviceFactory&) = delete;
        I2cDeviceFactory& operator=(const I2cDeviceFactory&) = delete;

        static I2cDeviceFactory& getInstance() {
            static I2cDeviceFactory instance;
            return instance;
        }

        template<typename DeviceType, typename... Args> 
        std::unique_ptr<DeviceType> createDevice(
            i2c_addr_bit_len_t addressLength,
            uint16_t address,
            Args&&... args
        ) {
            if (mBusHandler.get() == nullptr) {
                setHandler();
            }
            i2c_device_config_t cfg = {
                .dev_addr_length = addressLength,
                .device_address = address,
                .scl_speed_hz = SCL_SPEED_HZ_DEFAULT,
            };
            i2c_master_dev_handle_t handler;

            ESP_ERROR_CHECK(i2c_master_probe(mBusHandler.get(), address, 10000));
            ESP_ERROR_CHECK(i2c_master_bus_add_device(mBusHandler.get(), &cfg, &handler));

            return std::make_unique<DeviceType>(handler, std::forward<Args>(args)...);
        }

    private:
        I2cDeviceFactory() {}

        void setHandler() {
            i2c_master_bus_config_t cfg = {
                .i2c_port = I2C_PORT_DEFAULT,
                .sda_io_num = I2C_MASTER_SDA_IO,
                .scl_io_num = I2C_MASTER_SCL_IO,
                .clk_source = I2C_CLK_SRC_DEFAULT,
                .glitch_ignore_cnt = GLITCH_IGNORE_COUNT_DEFAULT,
            };
            i2c_master_bus_handle_t handler;

            ESP_ERROR_CHECK(i2c_new_master_bus(&cfg, &handler));
            mBusHandler.reset(handler);
        }


    private:
        struct BusHandlerDeleter {
            void operator()(i2c_master_bus_t* pBus) {
                if (pBus) {
                    ESP_ERROR_CHECK(i2c_del_master_bus(pBus));
                }
            }
        };
        std::unique_ptr<i2c_master_bus_t, BusHandlerDeleter> mBusHandler{nullptr};
    };
}

#endif