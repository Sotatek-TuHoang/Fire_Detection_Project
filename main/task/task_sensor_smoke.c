#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

#include "message/message.h"
#include "task/task.h"

static TaskHandle_t TaskHandle;
extern QueueHandle_t sensor_smoke_queue;

static const char *TAG = "Sensor_Smoke_Queue";

void sensor_smoke_task(void *parameter)
{
  QueueHandle_t my_que = sensor_smoke_queue;
  for (;;)
  {
    if (uxQueueMessagesWaiting(my_que) > 0)
    {
      // Allocate memory for the message structure
      Message *msg = (Message *)malloc(sizeof(Message));
      if (msg != NULL)
      {
        // Receive the message from the my_que queue
        if (xQueueReceive(my_que, msg, 0) == pdTRUE)
        {
          // Process the received file message
          ESP_LOGI(TAG, "Processing message");
          if (msg->sender == TASK_MODULE_SIM)
          {
            // TODO
          }
        }
        // Free the memory after processing the message
        free(msg);
      }
    }
    // Delay
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

int init_task_sensor_smoke()
{
  BaseType_t taskCreated = xTaskCreate(sensor_smoke_task, "sensor_smoke_task", 4096, NULL, 10, &TaskHandle);

  if (taskCreated != pdTRUE)
  {
    ESP_LOGI(TAG, "Failed to create task");
    return taskCreated;
  }
  return taskCreated;
}