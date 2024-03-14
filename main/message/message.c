#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <esp_log.h>

#include "message.h"

QueueHandle_t module_sim_queue;
QueueHandle_t sensor_sht3x_queue;
QueueHandle_t sensor_smoke_queue;

static const char *TAG = "MsqQue";

int msg_que_init()
{
  module_sim_queue = xQueueCreate(QUEUE_SIZE, sizeof(Message));
  if (module_sim_queue == NULL)
  {
    ESP_LOGI(TAG, "Error creating module_sim_queue");
    return -1;
  }

  sensor_sht3x_queue = xQueueCreate(QUEUE_SIZE, sizeof(Message));
  if (sensor_sht3x_queue == NULL)
  {
    ESP_LOGI(TAG, "Error creating module_sim_queue");
    return -1;
  }

  sensor_smoke_queue = xQueueCreate(QUEUE_SIZE, sizeof(Message));
  if (sensor_smoke_queue == NULL)
  {
    ESP_LOGI(TAG, "Error creating module_sim_queue");
    return -1;
  }

  return 0;
}

Error msg_que_send(MsgQue dst, Sender sender, void *data)
{
  Message message;
  message.sender = sender;
  message.p_msg = data;

  BaseType_t ret = -1;

  switch (dst)
  {
  case MSG_MODULE_SIM:
    ret = xQueueSend(module_sim_queue, &message, 0);
    break;
  case MSG_SENSOR_SHT3X:
    ret = xQueueSend(sensor_sht3x_queue, &message, 0);
    break;
  case MSG_SENSOR_SMOKE:
    ret = xQueueSend(sensor_smoke_queue, &message, 0);
    break;
  default:
    // Invalid taskID
    break;
  }

  if (ret != pdTRUE)
  {
    ESP_LOGI(TAG, "Faild to send message to queue %d", dst);
    // TODO: Retry?
    return E_ENQUEUE_FAILED;
  }

  return E_SUCCESS;
}