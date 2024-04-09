#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

#include "A7680C.h"

static TaskHandle_t ModuleSim_TaskHandle;
QueueHandle_t ModuleSim_queue;

static const char *TAG = "Module_SIM_Queue";

void ModuleSim_task(void *parameter)
{
  uint8_t msg[100];
  ModuleSim_queue = xQueueCreate(5, sizeof(msg));
  if (ModuleSim_queue == 0)
  {
    ESP_LOGE(TAG, "Failed to create queue= %p\n", ModuleSim_queue);
  }

  for (;;)
  {
    if (uxQueueMessagesWaiting(ModuleSim_queue) > 0)
    {
      if (xQueueReceive(ModuleSim_queue, msg, pdMS_TO_TICKS(100)) == pdTRUE)
      {
        // TODO
      }
    }
    // Delay
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

int ModuleSim_Task_Init()
{
  BaseType_t taskCreated = xTaskCreate(ModuleSim_task, "ModuleSim_Task", 4096, NULL, 10, &ModuleSim_TaskHandle);

  if (taskCreated != pdTRUE)
  {
    ESP_LOGE(TAG, "Failed to create task");
    return taskCreated;
  }
  return taskCreated;
}
