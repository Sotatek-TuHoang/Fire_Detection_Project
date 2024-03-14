#include <driver/i2c.h>
#include <esp_err.h>

#include "sht3x_util.h"

/*
 * For the send command sequences, after writing the address and/or data to the sensor
 * and sending the ACK bit, the sensor needs the execution time to respond to the I2C read header with an ACK bit.
 * Hence, it is required to wait the command execution time before issuing the read header.
 * Commands must not be sent while a previous command is being processed.
 */
esp_err_t sht3x_send_command(uint8_t *command)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_start(cmd));
  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, (SHT3X_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, SHT3X_I2C_ACK_CHECK_EN));

  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write(cmd, command, sizeof(command), SHT3X_I2C_ACK_CHECK_EN));

  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_stop(cmd));
  esp_err_t err = i2c_master_cmd_begin(SHT3X_I2C_NUM, cmd, SHT3X_I2C_TIMEOUT_MS / portTICK_PERIOD_MS);

  i2c_cmd_link_delete(cmd);
  return err;
}

/*
 * Data sent to and received from the sensor consists of a sequence of 16-bit commands and/or 16-bit words
 * (each to be interpreted as unsigned integer, most significant byte transmitted first). Each data word is
 * immediately succeeded by an 8-bit CRC. In write direction it is mandatory to transmit the checksum.
 * In read direction it is up to the master to decide if it wants to process the checksum.
 */
esp_err_t sht3x_read(uint8_t *hex_code, uint8_t *measurements, uint8_t size)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_start(cmd));
  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, (SHT3X_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, SHT3X_I2C_ACK_CHECK_EN));
  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write(cmd, hex_code, SHT3X_HEX_CODE_SIZE, SHT3X_I2C_ACK_CHECK_EN));

  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_start(cmd));
  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, (SHT3X_SENSOR_ADDR << 1) | I2C_MASTER_READ, SHT3X_I2C_ACK_CHECK_EN));
  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_read(cmd, measurements, size, I2C_MASTER_LAST_NACK));

  ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_stop(cmd));
  esp_err_t err = i2c_master_cmd_begin(SHT3X_I2C_NUM, cmd, SHT3X_I2C_TIMEOUT_MS / portTICK_PERIOD_MS);

  i2c_cmd_link_delete(cmd);
  return err;
}

void sht3x_i2c_init(void)
{
  i2c_config_t i2c_config = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = SHT3X_I2C_SDA,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = SHT3X_I2C_SCL,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = SHT3X_I2C_FREQ_HZ};

  ESP_ERROR_CHECK(i2c_param_config(SHT3X_I2C_NUM, &i2c_config));
  ESP_ERROR_CHECK(i2c_driver_install(SHT3X_I2C_NUM, i2c_config.mode,
                                     SHT3X_I2C_RX_BUF_DISABLE, SHT3X_I2C_RX_BUF_DISABLE, 0));
}