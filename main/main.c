#include <stdio.h>
#include <esp_log.h>

#include "task/task.h"
#include "message/message.h"

int app_main(void)
{
  ESP_LOGI("APP_MAIN", "App start");

  msg_que_init();

  init_task_module_sim();
  init_task_sensor_sht3x();
  init_task_sensor_smoke();
  return 0;
}