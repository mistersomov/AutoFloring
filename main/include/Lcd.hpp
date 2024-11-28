#ifndef LCD_HPP
#define LCD_HPP

#include "Device.hpp"

#include <string>

namespace autflr {
    class Lcd : Device {
    public:
        Lcd(i2c_master_dev_t* pHandler);

        void setBacklight(bool enable) const;
        void putCursor(uint8_t row, uint32_t col) const;
        void print(
            const std::string& message,
            uint8_t row,
            uint32_t col
        ) const;
        void clear() const;

    protected:
        void initialize() const override;

    private:
        void sendCmd(char cmd) const;
        void sendData(char data) const;

        constexpr static const std::string TAG{"[LCD]"};
    };
}

#endif