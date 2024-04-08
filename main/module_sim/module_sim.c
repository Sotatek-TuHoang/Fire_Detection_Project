#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_system.h"
#include "esp_mac.h"
#include <string.h>
#include "esp_log.h"
#include "cJSON.h"
#include <time.h>
#include <stdio.h>

#include "module_sim.h"

extern QueueHandle_t uart_msg_queue;
TaskHandle_t mqtt_vPublish_task_handle = NULL;

uint8_t trans_code;

void send_sms(char *phone_num, char *msg)
{
  AT_tx("AT\r\n");
}

static const char *TAG = "Module sim";

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/