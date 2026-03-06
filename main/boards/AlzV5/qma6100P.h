/*В проекте Xiaozhi‑ESP32 плата AlzV5 использует акселерометр QMA6100P,
 подключённый по I2C с пинами:

SDA → GPIO40

SCL → GPIO41

INT1 → GPIO19 (прерывание активация камеры и микрофона)

INT2 → GPIO20 (двойной щелчек, вызов Аи агента)

Задача: настроить детектор движения (any‑motion) так, 
чтобы при встряхивании или перемещении устройства генерировалось прерывание,
 которое выведет ESP32‑S3 из глубокого сна.
*/


#ifndef QMA6100P_H
#define QMA6100P_H

#include "driver/i2c.h"

// I2C адрес (по умолчанию 0x12, если ADDR = GND)
#define QMA6100P_I2C_ADDR        0x12

// Регистры QMA6100P (выдержка из даташита)
#define QMA6100P_REG_CHIP_ID     0x00   // Chip ID = 0xE5
#define QMA6100P_REG_POWER_MODE  0x11   // Mode register
#define QMA6100P_REG_RANGE       0x0F   // Range select
#define QMA6100P_REG_BW          0x10   // Bandwidth / ODR
#define QMA6100P_REG_INT_EN_1    0x16   // Interrupt enable 1
#define QMA6100P_REG_INT_MAP_1   0x19   // Interrupt mapping 1
#define QMA6100P_REG_INT_MAP_2   0x1A   // Interrupt mapping 2
#define QMA6100P_REG_MOTION_CFG  0x09   // Motion configuration
#define QMA6100P_REG_MOTION_TH   0x0A   // Motion threshold
#define QMA6100P_REG_INT_STATUS_0 0x0B  // Interrupt status
#define QMA6100P_REG_ACCEL_X_L    0x01
#define QMA6100P_REG_ACCEL_X_H    0x02
#define QMA6100P_REG_ACCEL_Y_L    0x03
#define QMA6100P_REG_ACCEL_Y_H    0x04
#define QMA6100P_REG_ACCEL_Z_L    0x05
#define QMA6100P_REG_ACCEL_Z_H    0x06

// Значения для настройки
#define QMA6100P_MODE_NORMAL      0x80   // Normal mode (bit7=1)
#define QMA6100P_RANGE_2G         0x01   // ±2g
#define QMA6100P_RANGE_4G         0x02
#define QMA6100P_RANGE_8G         0x03
#define QMA6100P_RANGE_16G        0x04
#define QMA6100P_BW_250HZ         0x09   // 250 Hz
#define QMA6100P_INT_ANY_MOTION_EN (1 << 2) // бит 2 в INT_EN_1
#define QMA6100P_INT1_ANY_MOTION   (1 << 2) // бит 2 в INT_MAP_1

class QMA6100P {
public:
    QMA6100P(i2c_port_t i2c_port, gpio_num_t int1_pin, uint8_t addr = QMA6100P_I2C_ADDR);
    esp_err_t init();
    esp_err_t configureAnyMotion(uint8_t threshold = 20, uint8_t duration = 5);
    esp_err_t enableInterrupt(bool enable);
    esp_err_t readAccel(float &x, float &y, float &z);
    bool isMotionTriggered();

private:
    i2c_port_t _port;
    gpio_num_t _int1_pin;
    uint8_t _addr;
    uint8_t _range;   // текущий диапазон для пересчёта сырых значений

    esp_err_t writeReg(uint8_t reg, uint8_t value);
    esp_err_t readReg(uint8_t reg, uint8_t *value);
    esp_err_t readRegs(uint8_t reg, uint8_t *buf, size_t len);
};

#endif