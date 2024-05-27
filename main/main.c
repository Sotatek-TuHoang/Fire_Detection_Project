#include <stdio.h>
#include <esp_log.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "module_sim/A7680C.h"
#include "module_sim/uartdev.h"
#include "bme680/bme680.h"
#include "wifi_mqtt/button.h"
#include "wifi_mqtt/mqtt.h"
#include "wifi_mqtt/wifi.h"
#include "wifi_mqtt/nvs.h"

extern int bme680_task_init();
void app_main()
{
  module_sim_uart_init();
  // connect_mqtt_broker();
  button_init(18);
  nvs_flash_func_init();
  wifi_func_init();
  wifi_mqtt_func_init();
  bme680_task_init();
}
