#include <stdio.h>
#include <esp_log.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "module_sim/A7680C.h"
#include "module_sim/uartdev.h"
#include "bme680/bme680.h"

void app_main()
{
  ModuleSim_uart_init();
  connect_mqtt_broker();
}
