
// xiaozhi-esp32/boards/alzv5_esp32/config.h
#ifndef __BOARD_ALZV5_CONFIG_H_
#define __BOARD_ALZV5_CONFIG_H_

// Board identification
#define BOARD_NAME "AlzV5"

#include <driver/gpio.h>
// ========================
// Power Management
// ========================
#define POWER_GOOD_PIN        GPIO_NUM_35   // LD39100PU33RY PG (internal pull-up)
#define CAM_POWER_EN_PIN      GPIO_NUM_42   // CAM_3V3 enable (active LOW)
#define AUDIO_POWER_EN_PIN    GPIO_NUM_38   // MAX98357A power enable (active HIGH)
#define BATTERY_ADC_PIN       GPIO_NUM_1    // ADC1_CH0 for battery voltage
#define CHARGE_STATUS_PIN     GPIO_NUM_45   // Wireless charger status (pull-up)

// ========================
// Camera OV3660 (DVP + I2C)
// ========================
#define CAM_XMCLK_PIN         GPIO_NUM_15
#define CAM_SDA_PIN           GPIO_NUM_2
#define CAM_SCL_PIN           GPIO_NUM_3
#define CAM_D0_PIN            GPIO_NUM_4
#define CAM_D1_PIN            GPIO_NUM_5
#define CAM_D2_PIN            GPIO_NUM_6
#define CAM_D3_PIN            GPIO_NUM_7
#define CAM_D4_PIN            GPIO_NUM_8
#define CAM_D5_PIN            GPIO_NUM_9
#define CAM_D6_PIN            GPIO_NUM_10
#define CAM_D7_PIN            GPIO_NUM_11
#define CAM_VSYNC_PIN         GPIO_NUM_12
#define CAM_HREF_PIN          GPIO_NUM_13
#define CAM_PCLK_PIN          GPIO_NUM_14
#define CAM_RESET_PIN         GPIO_NUM_36   // active LOW

// ========================
// SD Card (SPI mode)
// ========================
#define SD_CMD_PIN            GPIO_NUM_16
#define SD_CLK_PIN            GPIO_NUM_17
#define SD_DATA0_PIN          GPIO_NUM_18
#define SD_CS_PIN             GPIO_NUM_37

/

// ── Audio ───────────────────────────────────────────────
#define AUDIO_INPUT_SAMPLE_RATE             24000
#define AUDIO_OUTPUT_SAMPLE_RATE            24000
#define AUDIO_I2S_METHOD_DUPLEX

/ ========================
// I2S Audio
// ========================
// Shared clock lines (mic & speaker)

#define I2S_LRCLK_PIN         GPIO_NUM_47
#define I2S_BCLK_PIN          GPIO_NUM_48

// Microphone (ICS-43434) - I2S input
#define I2S_MIC_DATA_PIN      GPIO_NUM_39   // DOUT from mic

// Speaker (MAX98357A) - I2S output
#define I2S_SPK_DATA_PIN      GPIO_NUM_21   // DIN to amp

// ========================
// Accelerometer QMA6100P (I2C)
// ========================
#define QMA_SDA_PIN           GPIO_NUM_40
#define QMA_SCL_PIN           GPIO_NUM_41
#define QMA_INT1_PIN          GPIO_NUM_19
#define QMA_INT2_PIN          GPIO_NUM_20

// ========================
// Status LED
// ========================
#define STATUS_LED_PIN        GPIO_NUM_46   // active HIGH (with pull-down)

// ========================
// System Control
// ========================
#define BOOT_BUTTON_PIN       GPIO_NUM_0    // pulled up, active LOW
#define UART_TX_PIN           GPIO_NUM_43
#define UART_RX_PIN           GPIO_NUM_44

// ========================
// Optional: I2C Bus Definitions
// ========================
#define CAMERA_I2C_PORT       I2C_NUM_0
#define CAMERA_I2C_SDA        CAM_SDA_PIN
#define CAMERA_I2C_SCL        CAM_SCL_PIN

#define QMA_I2C_PORT          I2C_NUM_1
#define QMA_I2C_SDA           QMA_SDA_PIN
#define QMA_I2C_SCL           QMA_SCL_PIN

// ========================
// ADC Channel Mapping
// ========================
#define BATTERY_ADC_CHANNEL   ADC_CHANNEL_0  // ADC1_CH0 → GPIO1

#define XCLK_FREQ_HZ 20000000

#endif // __BOARD_ALZV5_CONFIG_H_