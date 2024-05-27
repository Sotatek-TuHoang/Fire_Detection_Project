#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"
#include "driver/uart.h"
#include <string.h>
#include <stdio.h>

#include "uartdev.h"

QueueHandle_t uart_msg_queue;

esp_err_t module_sim_uart_init()
{
  esp_err_t ret;
  uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };

  // Install UART driver, and get the queue.
  ret = uart_driver_install(UART_NUM_1, 1024, 1024, 30, &uart_msg_queue, 0);
  ret = uart_param_config(UART_NUM_1, &uart_config);
  // Set UART pins (using UART0 default pins ie no changes.)
  ret = uart_set_pin(UART_NUM_1, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  return ret;
}

esp_err_t AT_tx(char *cmd, uint64_t delay_ms)
{
  esp_err_t ret;
  ret = uart_write_bytes(UART_NUM_1, cmd, strlen(cmd));
  vTaskDelay(pdMS_TO_TICKS(delay_ms));
  return ret;
}

int AT_rx(char *buf, size_t buf_size)
{
  uart_event_t uart_event;
  int total_bytes = 0;
  while (xQueueReceive(uart_msg_queue, (void *)&uart_event, (TickType_t)50))
  {
    int bytes = uart_read_bytes(UART_NUM_1, buf + total_bytes, uart_event.size, 100);
    if (bytes > 0)
    {
      total_bytes += bytes;
      if (total_bytes >= buf_size - 1)
      {
        break;
      }
    }
    else
    {
      break;
    }
  }
  buf[total_bytes] = '\0'; // Null-terminate the received string
  return total_bytes;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/