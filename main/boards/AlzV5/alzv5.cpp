#include "alzv5_esp32.h"
#include "config.h"
#include <driver/i2c.h>
#include <driver/i2s.h>
#include <sdmmc_cmd.h>

void alz_board_init() {
    // 1. Настройка пинов питания
    gpio_set_direction(POWER_GOOD_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(CAM_ON_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(AUDIO_ON_PIN, GPIO_MODE_OUTPUT);

    // 2. Включение питания камеры и аудио
    gpio_set_level(CAM_ON_PIN, 0);  // активен низкий уровень
    gpio_set_level(AUDIO_ON_PIN, 1); // активен высокий уровень

    // 3. Инициализация I2C для акселерометра
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = QMA_SDA_PIN,
        .scl_io_num = QMA_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000
    };
    i2c_param_config(I2C_NUM_0, &i2c_cfg);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    // 4. Инициализация SD-карты
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_cfg = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_cfg.gpio_cmd = SD_CMD_PIN;
    slot_cfg.gpio_clk = SD_CLK_PIN;
    slot_cfg.gpio_d0 = SD_DATA0_PIN;
    // ... остальная конфигурация

    // 5. Инициализация I2S для микрофона и аудиоусилителя
    i2s_config_t i2s_cfg = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };

    i2s_pin_config_t pin_cfg = {
        .bck_io_num = I2S_BCLK_PIN,
        .ws_io_num = I2S_LRCLK_PIN,
        .data_out_num = I2S_DIN_PIN,
        .data_in_num = I2S_DOUT_PIN
    };

    i2s_driver_install(I2S_NUM_0, &i2s_cfg, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_cfg);

    // 6. Инициализация камеры (если используется)
    // ... код для OV3660

    // 7. Настройка UART для отладки
    uart_config_t uart_cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_cfg);
    uart_set_pin(UART_NUM_0, TXD0_PIN, RXD0_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
}

// Функция проверки батареи
int alz_read_battery() {
    return adc1_get_raw(ADC1_CHANNEL_0); // BATTERY_CHECK_PIN -> ADC1_CH0
}

// Функция проверки зарядки
bool alz_is_charging() {
    return gpio_get_level(CHG_PIN) == 0; // активный низкий уровень
}