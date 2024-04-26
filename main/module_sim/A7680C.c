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

#include "A7680C.h"
#include "uartdev.h"

extern QueueHandle_t uart_msg_queue;
TaskHandle_t mqtt_vPublish_task_handle = NULL;

uint8_t trans_code;

static const char *TAG = "Module sim";

void create_client_id(char *client_id)
{
  uint8_t mac_address[6];
  esp_efuse_mac_get_default(mac_address);
  char str_mac[13];
  sprintf(str_mac, "%02X%02X%02X%02X%02X%02X", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);
  char str_rd[13];
  const char char_pool[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  srand(time(NULL));
  for (int i = 0; i < 12; ++i)
  {
    int idx = rand() % (sizeof(char_pool) - 1);
    str_rd[i] = char_pool[idx];
  }
  str_rd[12] = '\0';
  snprintf(client_id, 27, "%s%s", str_mac, str_rd);
}

void connect_mqtt_broker()
{
  char cmd[512];
  char resp[512];
  char client_id[27];
  create_client_id(client_id);
  printf("client_id: %s", client_id);

  snprintf(cmd, sizeof(cmd), "AT+CMQTTSTART\r\n");
  AT_tx(cmd, 100);

  snprintf(cmd, sizeof(cmd), "AT+CMQTTACCQ=0,\"%s\",0\r\n", client_id);
  AT_tx(cmd, 100);

  snprintf(cmd, sizeof(cmd), "AT+CMQTTCONNECT=0,\"tcp://%s:%s\",60,1\r\n", BROKER_ADDR, BROKER_PORT);
  AT_tx(cmd, 100);

  // snprintf(cmd, sizeof(cmd), "AT+CMQTTTOPIC=0,12\r\n");
  // AT_tx(cmd, 500);

  // snprintf(cmd, sizeof(cmd), "mymqtt/test1\r\n");
  // AT_tx(cmd, 100);

  // snprintf(cmd, sizeof(cmd), "AT+CMQTTPAYLOAD=0,6\r\n");
  // AT_tx(cmd, 500);

  // snprintf(cmd, sizeof(cmd), "HELLO2\r\n");
  // AT_tx(cmd, 100);

  // snprintf(cmd, sizeof(cmd), "AT+CMQTTPUB=0,1,60,0\r\n");
  // AT_tx(cmd, 100);
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/