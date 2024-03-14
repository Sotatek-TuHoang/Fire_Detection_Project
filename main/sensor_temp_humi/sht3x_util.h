#ifndef SHT3X_UTIL_H
#define SHT3X_UTIL_H

#include "sht3x.h"

#define SHT3X_I2C_NUM 0
#define SHT3X_I2C_IMEOUT_MS

#define SHT3X_I2C_SDA 16
#define SHT3X_I2C_SCL 17
#define SHT3X_I2C_RX_BUF_DISABLE 0
#define SHT3X_I2C_TX_BUF_DISABLE 0
#define SHT3X_I2C_FREQ_HZ 100000
#define SHT3X_I2C_TIMEOUT_MS 1000
#define SHT3X_I2C_ACK_CHECK_DIS 0x00
#define SHT3X_I2C_ACK_CHECK_EN 0x01
#define SHT3X_I2C_ACK_VAL 0x00
#define SHT3X_I2C_NACK_VAL 0x01

void sht3x_i2c_init(void);

esp_err_t sht3x_send_command(uint8_t *command);
esp_err_t sht3x_read(uint8_t *hex_code, uint8_t *measurements, uint8_t size);

#endif // SHT3X_UTIL_H