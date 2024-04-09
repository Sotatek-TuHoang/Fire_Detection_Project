#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"
#include "driver/uart.h"
#include <string.h>
#include <stdio.h>

#include "uartdev.h"

QueueHandle_t uart_msg_queue;

int ModuleSim_uart_init()
{
  int ret;
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

int AT_tx(char *cmd)
{
  int ret;
  ret = uart_write_bytes(UART_NUM_1, cmd, strlen(cmd) + 1);
  return ret;
}

int AT_rx(char *buf)
{
  uart_event_t uart_event;
  if (xQueueReceive(uart_msg_queue, (void *)&uart_event, (TickType_t)50))
    return uart_read_bytes(UART_NUM_1, buf, uart_event.size * 2, 100);
  else
    return -1;
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/