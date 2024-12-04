#include "Lcd.hpp"

#include <vector>

namespace autflr {
    Lcd::Lcd(idf::I2CMaster* pMaster, uint8_t address) : mMasterPtr{pMaster}, mAddress{idf::I2CAddress(address)} {
        if (!mMasterPtr) {
            ESP_LOGE(TAG, "I2CMaster instance is null");
            throw std::invalid_argument("I2CMaster instance cannot be null");
        }
        initialize();
    }

    void Lcd::initialize() const {
        constexpr std::chrono::milliseconds INIT_DELAY_MS(150);
        constexpr std::chrono::milliseconds CMD_DELAY_MS(6);
        constexpr std::chrono::microseconds CMD_DELAY_US(60);
        constexpr std::chrono::milliseconds CMD_DELAY_FINISHED_MS(4);

        std::this_thread::sleep_for(INIT_DELAY_MS);
        sendCmd(0x30);
        std::this_thread::sleep_for(CMD_DELAY_MS);
        sendCmd(0x30);
        std::this_thread::sleep_for(INIT_DELAY_MS);
        sendCmd(0x30);
        std::this_thread::sleep_for(INIT_DELAY_MS);
        sendCmd(0x20);
        std::this_thread::sleep_for(INIT_DELAY_MS);

        sendCmd(0x28);
        std::this_thread::sleep_for(CMD_DELAY_US);
        sendCmd(0x08);
        std::this_thread::sleep_for(CMD_DELAY_US);
        sendCmd(0x06);
        std::this_thread::sleep_for(CMD_DELAY_US);
        sendCmd(0x0C);
        std::this_thread::sleep_for(CMD_DELAY_US);
        sendCmd(0x01);
        std::this_thread::sleep_for(CMD_DELAY_FINISHED_MS);
        sendCmd(0x02);
        std::this_thread::sleep_for(CMD_DELAY_FINISHED_MS);

        clear();

        ESP_LOGI(TAG, "Initialization is completed!");
    }

    void Lcd::putCursor(uint16_t row, uint16_t col) const {
        constexpr uint16_t MAX_ROW = 1;
        constexpr uint16_t MAX_COLUMN = 15;
        row = std::min(row, MAX_ROW);
        col = std::min(col, MAX_COLUMN);

        sendCmd((row == 0 ? ROW_0_OFFSET : ROW_1_OFFSET) | col);
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

    void Lcd::sendCmd(uint8_t cmd) const {
        uint8_t highOrderBit = cmd & 0xF0;
        uint8_t lowOrderBit = cmd << 4;
        std::vector<uint8_t> bits = {
            static_cast<uint8_t>(highOrderBit | ENABLE_BIT),
            static_cast<uint8_t>(highOrderBit | DISABLE_BIT),
            static_cast<uint8_t>(lowOrderBit | ENABLE_BIT),
            static_cast<uint8_t>(lowOrderBit | DISABLE_BIT)
        };

        mMasterPtr->sync_write(mAddress, bits);
    }

    void Lcd::sendData(uint8_t data) const {
        uint8_t highOrderBit = data & 0xF0;
        uint8_t lowOrderBit = data << 4;
        std::vector<uint8_t> bits = {
            static_cast<uint8_t>(highOrderBit | ENABLE_DATA),
            static_cast<uint8_t>(highOrderBit | DISABLE_DATA),
            static_cast<uint8_t>(lowOrderBit | ENABLE_DATA),
            static_cast<uint8_t>(lowOrderBit | DISABLE_DATA)
        };

        mMasterPtr->sync_write(mAddress, bits);
    }

}