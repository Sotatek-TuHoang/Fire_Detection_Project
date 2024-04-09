#include <stdio.h>
#include <esp_log.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "module_sim/A7680C.h"
#include "bme680/bme680.h"

#define BME680_I2C_ADDR 0x77
#define PORT 0
#define CONFIG_EXAMPLE_I2C_MASTER_SDA 21
#define CONFIG_EXAMPLE_I2C_MASTER_SCL 22

bme680_t sensor;
bme680_values_float_t values;
uint32_t duration;

void init_bme680(void)
{
  memset(&sensor, 0, sizeof(bme680_t));

  ESP_ERROR_CHECK(bme680_init_desc(&sensor, BME680_I2C_ADDR, PORT, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));

  // init the sensor
  ESP_ERROR_CHECK(bme680_init_sensor(&sensor));

  // Changes the oversampling rates to 4x oversampling for temperature
  // and 2x oversampling for humidity. Pressure measurement is skipped.
  bme680_set_oversampling_rates(&sensor, BME680_OSR_4X, BME680_OSR_2X, BME680_OSR_2X);

  // Change the IIR filter size for temperature and pressure to 7.
  bme680_set_filter_size(&sensor, BME680_IIR_SIZE_7);

  // Change the heater profile 0 to 200 degree Celsius for 100 ms.
  bme680_set_heater_profile(&sensor, 0, 200, 100);
  bme680_use_heater_profile(&sensor, 0);

  // Set ambient temperature to 10 degree Celsius
  bme680_set_ambient_temperature(&sensor, 10);

  // as long as sensor configuration isn't changed, duration is constant
  bme680_get_measurement_duration(&sensor, &duration);
}
void get_bme680_readings(void)
{

  // trigger the sensor to start one TPHG measurement cycle
  if (bme680_force_measurement(&sensor) == ESP_OK)
  {
    // passive waiting until measurement results are available
    vTaskDelay(duration);

    // get the results and do something with them
    if (bme680_get_results_float(&sensor, &values) == ESP_OK)
      printf("BME680 Sensor: %.2f °C, %.2f %%, %.2f hPa, %.2f KOhm\n",
             values.temperature, values.humidity, values.pressure, values.gas_resistance);
  }
}

void app_main()
{
  ESP_ERROR_CHECK(i2cdev_init());
  init_bme680();

  for (;;)
  {
    get_bme680_readings();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}
