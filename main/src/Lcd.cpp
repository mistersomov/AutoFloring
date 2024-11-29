#include "Lcd.hpp"

namespace autflr {
    Lcd::Lcd(i2c_master_dev_t* pHandler) : I2cSlaveDevice(pHandler) {
        initialize();
        clear();
    }

    void Lcd::initialize() const {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        sendCmd(0x30);
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
        sendCmd(0x30);
        std::this_thread::sleep_for(std::chrono::microseconds(150));
        sendCmd(0x30);
        std::this_thread::sleep_for(std::chrono::microseconds(150));
        sendCmd(0x20);
        std::this_thread::sleep_for(std::chrono::microseconds(150));

        sendCmd(0x28);
        std::this_thread::sleep_for(std::chrono::microseconds(60));
        sendCmd(0x08);
        std::this_thread::sleep_for(std::chrono::microseconds(60));
        sendCmd(0x06);
        std::this_thread::sleep_for(std::chrono::microseconds(60));
        sendCmd(0x0C);
        std::this_thread::sleep_for(std::chrono::microseconds(60));
        sendCmd(0x01);
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
        sendCmd(0x02);
        std::this_thread::sleep_for(std::chrono::milliseconds(4));

        ESP_LOGI(TAG, "Initialization is completed!");
    }

    void Lcd::putCursor(uint8_t row, uint32_t col) const {
        switch(row) {
            case 0:
                col |= ROW_0_OFFSET;
                break;
            case 1:
                col |= ROW_1_OFFSET;
                break;
        }
        sendCmd(col);
        std::this_thread::sleep_for(std::chrono::microseconds(150));
    }

    void Lcd::print(
        const std::string& message,
        uint8_t row,
        uint32_t col
    ) const {
        putCursor(row, col);
        for (auto itt = message.begin(); itt != message.end(); ++itt) {
            sendData(*itt);
        }
    }

    void Lcd::sendCmd(char cmd) const {
        char highOrderBit = cmd & 0xF0;
        char lowOrderBit = cmd << 4;
        std::vector<uint8_t> bits = {
            static_cast<uint8_t>(highOrderBit | ENABLE_BIT),
            static_cast<uint8_t>(highOrderBit | DISABLE_BIT),
            static_cast<uint8_t>(lowOrderBit | ENABLE_BIT),
            static_cast<uint8_t>(lowOrderBit | DISABLE_BIT)
        };

        esp_err_t err = i2c_master_transmit(mHandler.get(), bits.data(), sizeof(uint8_t) * bits.size(), 1000);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send command: %d", err);
        }
    }

    void Lcd::sendData(char data) const {
        char highOrderBit = data & 0xF0;
        char lowOrderBit = data << 4;
        std::vector<uint8_t> bits = {
            static_cast<uint8_t>(highOrderBit | ENABLE_DATA),
            static_cast<uint8_t>(highOrderBit | DISABLE_DATA),
            static_cast<uint8_t>(lowOrderBit | ENABLE_DATA),
            static_cast<uint8_t>(lowOrderBit | DISABLE_DATA)
        };

        esp_err_t err = i2c_master_transmit(mHandler.get(), bits.data(), sizeof(uint8_t) * bits.size(), 1000);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send data: %d", err);
        }
    }

}