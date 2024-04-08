#ifndef MODULE_SIM_H
#define MODULE_SIM_H
#include "driver/gpio.h"

#define UART_TX GPIO_NUM_22
#define UART_RX GPIO_NUM_23

#define BUF_SIZE 1024

void send_sms(char *phone_num, char *msg);

int ModuleSim_uart_init();

int ModuleSim_Task_Init();

int AT_tx(char *cmd);

int AT_rx(char *buf);

#endif // MODULE_SIM_UTIL_H