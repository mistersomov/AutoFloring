#ifndef LCD_HPP
#define LCD_HPP

#include "i2c_cxx.hpp"
#include "esp_log.h"

#include <chrono>
#include <string>
#include <thread>

namespace autflr {
    class Lcd {
    public:
        Lcd(idf::I2CMaster* pMaster, uint8_t address);

        void putCursor(uint16_t row, uint16_t col) const;
        void print(
            const std::string& message,
            uint8_t row,
            uint32_t col
        ) const;
        inline void clear() const {
            sendCmd(0x01);
        }

    private:
        void initialize() const;
        void sendCmd(uint8_t cmd) const;
        void sendData(uint8_t data) const;

    private:
        idf::I2CMaster* mMasterPtr{nullptr};
        idf::I2CAddress mAddress{0};
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