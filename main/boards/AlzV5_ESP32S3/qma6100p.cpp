#include "qma6100p.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static const char *TAG = "QMA6100P";

QMA6100P::QMA6100P(i2c_port_t port, gpio_num_t int1_pin, uint8_t addr)
    : _port(port), _int1_pin(int1_pin), _addr(addr), _range(QMA6100P_RANGE_2G) {}

esp_err_t QMA6100P::writeReg(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C write error: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t QMA6100P::readReg(uint8_t reg, uint8_t *value) {
    return readRegs(reg, value, 1);
}

esp_err_t QMA6100P::readRegs(uint8_t reg, uint8_t *buf, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, buf, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C read error: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t QMA6100P::init() {
    uint8_t chip_id = 0;
    if (readReg(QMA6100P_REG_CHIP_ID, &chip_id) != ESP_OK) {
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Chip ID: 0x%02X", chip_id);
    if (chip_id != 0xE5) { // ожидаемое значение
        ESP_LOGE(TAG, "Invalid chip ID");
        return ESP_ERR_NOT_FOUND;
    }

    // Выход из sleep (если был) – запись в регистр режима
    writeReg(QMA6100P_REG_POWER_MODE, QMA6100P_MODE_NORMAL);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Установка диапазона и частоты
    writeReg(QMA6100P_REG_RANGE, _range);
    writeReg(QMA6100P_REG_BW, QMA6100P_BW_250HZ);
    vTaskDelay(pdMS_TO_TICKS(10));

    ESP_LOGI(TAG, "QMA6100P initialized");
    return ESP_OK;
}

esp_err_t QMA6100P::configureAnyMotion(uint8_t threshold, uint8_t duration) {
    // threshold – значение для порога (зависит от диапазона)
    // duration – количество последовательных сэмплов для подтверждения движения (0..3)
    // Регистр Motion_Config (0x09): биты [7:6] – режим (any-motion), [5:4] – duration (0..3)
    uint8_t motion_cfg = (0x01 << 6) | ((duration & 0x03) << 4); // any-motion mode
    writeReg(QMA6100P_REG_MOTION_CFG, motion_cfg);

    // Регистр Motion_Threshold (0x0A)
    writeReg(QMA6100P_REG_MOTION_TH, threshold);

    // Включение прерывания any-motion в INT_EN_1 (0x16)
    uint8_t int_en1;
    readReg(QMA6100P_REG_INT_EN_1, &int_en1);
    int_en1 |= QMA6100P_INT_ANY_MOTION_EN;
    writeReg(QMA6100P_REG_INT_EN_1, int_en1);

    // Маппинг прерывания на пин INT1 через INT_MAP_1 (0x19)
    uint8_t int_map1;
    readReg(QMA6100P_REG_INT_MAP_1, &int_map1);
    int_map1 |= QMA6100P_INT1_ANY_MOTION;
    writeReg(QMA6100P_REG_INT_MAP_1, int_map1);

    ESP_LOGI(TAG, "Any-motion configured: threshold=%d, duration=%d", threshold, duration);
    return ESP_OK;
}

esp_err_t QMA6100P::enableInterrupt(bool enable) {
    // Можно дополнительно управлять глобальным включением прерываний, но достаточно настроек выше.
    return ESP_OK;
}

esp_err_t QMA6100P::readAccel(float &x, float &y, float &z) {
    uint8_t data[6];
    if (readRegs(QMA6100P_REG_ACCEL_X_L, data, 6) != ESP_OK) {
        return ESP_FAIL;
    }
    int16_t raw_x = (int16_t)(data[1] << 8 | data[0]);
    int16_t raw_y = (int16_t)(data[3] << 8 | data[2]);
    int16_t raw_z = (int16_t)(data[5] << 8 | data[4]);

    // Преобразование в g в зависимости от диапазона
    float scale;
    switch (_range) {
        case QMA6100P_RANGE_2G: scale = 16384.0f; break;
        case QMA6100P_RANGE_4G: scale = 8192.0f; break;
        case QMA6100P_RANGE_8G: scale = 4096.0f; break;
        case QMA6100P_RANGE_16G: scale = 2048.0f; break;
        default: scale = 16384.0f;
    }
    x = raw_x / scale;
    y = raw_y / scale;
    z = raw_z / scale;
    return ESP_OK;
}

bool QMA6100P::isMotionTriggered() {
    // Можно прочитать статус прерывания по регистру INT_STATUS_0 (0x0B)
    uint8_t status = 0;
    readReg(QMA6100P_REG_INT_STATUS_0, &status);
    return (status & QMA6100P_INT_ANY_MOTION_EN) != 0;
}