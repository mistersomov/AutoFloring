#ifndef I2C_DEVICE_FACTORY_HPP
#define I2C_DEVICE_FACTORY_HPP

#include "i2c_cxx.hpp"
#include "esp_log.h"

#include <cstdint>

constexpr idf::I2CNumber I2C_MASTER_NUM(idf::I2CNumber::I2C0());

namespace autflr {
    class I2cDeviceFactory {
    public:
        I2cDeviceFactory(const I2cDeviceFactory&) = delete;
        I2cDeviceFactory& operator=(const I2cDeviceFactory&) = delete;

        static I2cDeviceFactory& getInstance() {
            static I2cDeviceFactory instance;
            return instance;
        }

        /**
         * @brief Creates an I2C device of the specified type.
         * @tparam DeviceType The type of the I2C device to create.
         * @param address The I2C address of the device.
         * @param args Additional arguments required for the device's constructor.
         * @return A unique pointer to the created device.
         */
        template<typename DeviceType, typename... Args> 
        std::unique_ptr<DeviceType> createDevice(
            uint8_t address,
            Args&&... args
        ) {
            if (mI2CMaster.get() == nullptr) {
                setupMaster();
            }

            return std::make_unique<DeviceType>(mI2CMaster.get(), address, std::forward<Args>(args)...);
        }

    private:
        I2cDeviceFactory() {}

        void setupMaster() {
            try {
                mI2CMaster = std::make_shared<idf::I2CMaster>(
                    I2C_MASTER_NUM,
                    idf::SCL_GPIO(I2C_MASTER_SCL_IO),
                    idf::SDA_GPIO(I2C_MASTER_SDA_IO),
                    idf::Frequency(FREQUENCY)
                );

                ESP_LOGI(TAG, "I2C initialized successfully");
            } catch (const idf::I2CException& e) {
                ESP_LOGE(TAG, "I2C Exception with error: %s (0x%X)", e.what(), e.error);
            }
        }

    private:
        std::shared_ptr<idf::I2CMaster> mI2CMaster{nullptr};
        static constexpr uint16_t I2C_MASTER_SDA_IO = 21;
        static constexpr uint16_t I2C_MASTER_SCL_IO = 22;
        static constexpr uint32_t FREQUENCY = 400000;
        constexpr static const char* TAG{"[I2cDeviceFactory]"};
    };
}

#endif