#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

#include "message/message.h"
#include "task/task.h"

#include "sensor_temp_humi/sht3x.h"

static TaskHandle_t TaskHandle;
extern QueueHandle_t sensor_sht3x_queue;

static const char *TAG = "SHT3X_Queue";
static sht3x_sensors_values_t sht3x_values;

void handle_sht3x()
{
  sht3x_start_periodic_measurement_with_art();

  vTaskDelay(100 / portTICK_PERIOD_MS);

  if (sht3x_read_measurement(&sht3x_values) != ESP_OK)
  {
    ESP_LOGE(TAG, "Sensors read measurement error!");
  }
  vTaskDelay(100 / portTICK_PERIOD_MS);

  float temperature = sht3x_values.temperature;
  float humidity = sht3x_values.humidity;

  ESP_LOG_BUFFER_HEX_LEVEL(TAG, &sht3x_values, sizeof(sht3x_values), ESP_LOG_DEBUG);

  sht3x_stop_periodic_measurement();

  ESP_LOGI(TAG, "Temperature %2.1f °C - Humidity %2.1f%%", temperature, humidity);
}

void sensor_sht3x_task(void *parameter)
{
  QueueHandle_t my_que = sensor_sht3x_queue;
  for (;;)
  {
    handle_sht3x();
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

int init_task_sensor_sht3x()
{
  sht3x_init();
  BaseType_t taskCreated = xTaskCreate(sensor_sht3x_task, "sensor_sht3x_task", 4096, NULL, 10, &TaskHandle);

  if (taskCreated != pdTRUE)
  {
    ESP_LOGI(TAG, "Failed to create task");
    return taskCreated;
  }
  return taskCreated;
}