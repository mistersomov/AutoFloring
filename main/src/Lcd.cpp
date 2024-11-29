#include "Lcd.hpp"

#include "esp_log.h"

#include <thread>
#include <chrono>
#include <vector>

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

        ESP_LOGI(TAG.c_str(), "Initialization is completed!");
    }

    void Lcd::setBacklight(bool enable) const {
        sendCmd(0x00);
    }

    void Lcd::putCursor(uint8_t row, uint32_t col) const {
        switch(row) {
            case 0:
                col |= 0x80;
                break;
            case 1:
                col |= 0xC0;
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
        for (auto i = 0; i != message.length(); ++i) {
            sendData(message[i]);
        }
    }

    void Lcd::clear() const {
        sendCmd(0x01);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    void Lcd::sendCmd(char cmd) const {
        char highOrderBit = cmd & 0xF0;
        char lowOrderBit = cmd << 4;
        std::vector<uint8_t> bits = {
            static_cast<uint8_t>(highOrderBit | 0x0C),
            static_cast<uint8_t>(highOrderBit | 0x08),
            static_cast<uint8_t>(lowOrderBit | 0x0C),
            static_cast<uint8_t>(lowOrderBit | 0x08)
        };

        esp_err_t err = i2c_master_transmit(mHandler.get(), bits.data(), sizeof(uint8_t) * bits.size(), 1000);
        if (err != ESP_OK) {
            ESP_LOGE(TAG.c_str(), "Failed to send command: %d", err);
        }
    }

    void Lcd::sendData(char data) const {
        char highOrderBit = data & 0xF0;
        char lowOrderBit = data << 4;
        std::vector<uint8_t> bits = {
            static_cast<uint8_t>(highOrderBit | 0x0D),
            static_cast<uint8_t>(highOrderBit | 0x09),
            static_cast<uint8_t>(lowOrderBit | 0x0D),
            static_cast<uint8_t>(lowOrderBit | 0x09)
        };

        esp_err_t err = i2c_master_transmit(mHandler.get(), bits.data(), sizeof(uint8_t) * bits.size(), 1000);
        if (err != ESP_OK) {
            ESP_LOGE(TAG.c_str(), "Failed to send data: %d", err);
        }
    }
}