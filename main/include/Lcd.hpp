#ifndef LCD_HPP
#define LCD_HPP

#include "I2cSlaveDevice.hpp"

#include "esp_log.h"

#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace autflr {
    class Lcd : public I2cSlaveDevice {
    public:
        Lcd(i2c_master_dev_t* pHandler);

        void putCursor(uint8_t row, uint32_t col) const;
        void print(
            const std::string& message,
            uint8_t row,
            uint32_t col
        ) const;

        inline void clear() const {
            sendCmd(0x01);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        inline void setBacklight(bool enable) const {

        }

    protected:
        void initialize() const override;

    private:
        void sendCmd(char cmd) const;
        void sendData(char data) const;

        static constexpr uint8_t ENABLE_BIT = 0x0C;
        static constexpr uint8_t DISABLE_BIT = 0x08;
        static constexpr uint8_t ENABLE_DATA = 0x0D;
        static constexpr uint8_t DISABLE_DATA = 0x09;
        static constexpr uint8_t ROW_0_OFFSET = 0x80;
        static constexpr uint8_t ROW_1_OFFSET = 0xC0;
        constexpr static const char* TAG{"[LCD]"};
    };
}

#endif