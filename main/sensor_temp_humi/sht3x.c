/***************************************************************************
 * @file         sht3x.c
 * @author       tuha
 * @date         14 August 2023
 * @brief        SHT3x sensor driver implementation.
 *               This file contains the implementation of functions to
 *               interact with the Sensirion SHT3x series temperature and
 *               humidity sensor using the I2C communication protocol.
 *               It provides functions for initializing the sensor, starting
 *               measurements, reading raw data, computing temperature and
 *               humidity values, and handling errors. The driver supports
 *               both single-shot and periodic measurement modes.
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <esp_log.h>
#include <sys/time.h>
#include <math.h>

#include "sht3x.h"
#include "sht3x_util.h"

static const char *SHT3X_TAG = "SHT3X";

typedef struct measurements
{
  sht3x_sensor_value_t temperature;
  sht3x_sensor_value_t humidity;
} measurements_t;

// Single Shot Data Acquisition
uint8_t clock_stretching_enabled_repeatability_high[] = {0x2C, 0x06};
uint8_t clock_stretching_enabled_repeatability_medium[] = {0x2C, 0x0D};
uint8_t clock_stretching_enabled_repeatability_low[] = {0x2C, 0x10};
uint8_t clock_stretching_disabled_repeatability_high[] = {0x24, 0x00};
uint8_t clock_stretching_disabled_repeatability_medium[] = {0x24, 0x0B};
uint8_t clock_stretching_disabled_repeatability_low[] = {0x24, 0x16};

// Periodic Data Acquisition Mode
uint8_t mps_0_5_repeatability_high[] = {0x20, 0x32};
uint8_t mps_0_5_repeatability_medium[] = {0x20, 0x24};
uint8_t mps_0_5_repeatability_low[] = {0x20, 0x2F};
uint8_t mps_1_repeatability_high[] = {0x21, 0x30};
uint8_t mps_1_repeatability_medium[] = {0x21, 0x26};
uint8_t mps_1_repeatability_low[] = {0x21, 0x2D};
uint8_t mps_2_repeatability_high[] = {0x22, 0x36};
uint8_t mps_2_repeatability_medium[] = {0x22, 0x20};
uint8_t mps_2_repeatability_low[] = {0x22, 0x2B};
uint8_t mps_4_repeatability_high[] = {0x23, 0x34};
uint8_t mps_4_repeatability_medium[] = {0x23, 0x22};
uint8_t mps_4_repeatability_low[] = {0x23, 0x29};
uint8_t mps_10_repeatability_high[] = {0x27, 0x37};
uint8_t mps_10_repeatability_medium[] = {0x27, 0x21};
uint8_t mps_10_repeatability_low[] = {0x27, 0x2A};

// Commands
uint8_t read_measurement[] = {0xE0, 0x00};
uint8_t periodic_measurement_with_art[] = {0x2B, 0x32};
uint8_t stop_periodic_measurement[] = {0x30, 0x93};
uint8_t soft_reset[] = {0x30, 0xA2};
uint8_t general_call_reset[] = {0x00, 0x06};
uint8_t heater_enable[] = {0x30, 0x6D};
uint8_t heater_disable[] = {0x30, 0x66};
uint8_t read_status_register[] = {0xF3, 0x2D};
uint8_t clear_status_register[] = {0x30, 0x41};

/**
 * @brief Calculate the 8-bit CRC checksum for data.
 *
 * This function calculates the 8-bit CRC checksum for a given data buffer.
 * The CRC covers the contents of the data bytes in the buffer.
 *
 * @param[in] data Pointer to the data buffer.
 * @param[in] data_len Length of the data buffer.
 *
 * @return The calculated CRC checksum.
 */
static uint8_t calculate_crc(const uint8_t *data, uint8_t data_len)
{
  uint16_t current_byte;
  uint8_t crc = 0xFF;
  uint8_t crc_bit;

  for (current_byte = 0; current_byte < data_len; ++current_byte)
  {
    crc ^= (data[current_byte]);
    for (crc_bit = 8; crc_bit > 0; --crc_bit)
    {
      if (crc & 0x80)
      {
        crc = (crc << 1) ^ CRC8_POLYNOMIAL;
      }
      else
      {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}

esp_err_t sht3x_start_periodic_measurement(uint8_t *periodic_command)
{
  return sht3x_send_command(periodic_command);
}

esp_err_t sht3x_start_periodic_measurement_with_art()
{
  return sht3x_send_command(periodic_measurement_with_art);
}

esp_err_t sht3x_stop_periodic_measurement()
{
  return sht3x_send_command(stop_periodic_measurement);
}

esp_err_t sht3x_read_measurement(sht3x_sensors_values_t *sensors_values)
{
  measurements_t measurements =
      {
          .temperature = {{0x00, 0x00}, 0x00},
          .humidity = {{0x00, 0x00}, 0x00}};

  esp_err_t err = sht3x_read(read_measurement, (uint8_t *)&measurements, sizeof(measurements));

  if (calculate_crc((uint8_t *)&measurements.temperature, sizeof(measurements.temperature.value)) != measurements.temperature.crc ||
      calculate_crc((uint8_t *)&measurements.humidity, sizeof(measurements.humidity.value)) != measurements.humidity.crc)
  {
    return ESP_ERR_INVALID_CRC;
  }

  sensors_values->temperature = (175.0 * (((measurements.temperature.value.msb << 8) + measurements.temperature.value.lsb) / 65535.0)) - 45.0;
  sensors_values->humidity = 100.0 * ((measurements.humidity.value.msb << 8) + measurements.humidity.value.lsb) / 65535.0;

  return err;
}

esp_err_t sht3x_soft_reset()
{
  return sht3x_send_command(soft_reset);
}

esp_err_t sht3x_general_call_reset()
{
  return sht3x_send_command(general_call_reset);
}

esp_err_t sht3x_enable_heater()
{
  return sht3x_send_command(heater_enable);
}

esp_err_t sht3x_disable_heater()
{
  return sht3x_send_command(heater_disable);
}

esp_err_t sht3x_read_status_register(sht3x_sensor_value_t *sensors_value)
{
  sht3x_sensor_value_t status_register =
      {
          .value = {0x00, 0x00},
          .crc = 0x00};

  esp_err_t err = sht3x_read(read_status_register, (uint8_t *)&status_register, sizeof(status_register));

  if (err != ESP_OK)
  {
    ESP_LOGE(SHT3X_TAG, "read_status_register failed with status code: %s", esp_err_to_name(err));
    return SHT3X_READ_ERROR;
  }
  return err;
}

esp_err_t sht3x_clear_status_register()
{
  return sht3x_send_command(clear_status_register);
}

esp_err_t sht3x_read_singleshot(sht3x_sensors_values_t *sensors_values)
{
  esp_err_t err = sht3x_send_command(clock_stretching_enabled_repeatability_high);
  if (err != ESP_OK)
  {
    ESP_LOGE(SHT3X_TAG, "sht3x_send_command failed with status code: %s", esp_err_to_name(err));
    return err;
  }
  vTaskDelay(pdMS_TO_TICKS(40));
  return sht3x_read_measurement(sensors_values);
}

void sht3x_init(void)
{
  sht3x_i2c_init();
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/