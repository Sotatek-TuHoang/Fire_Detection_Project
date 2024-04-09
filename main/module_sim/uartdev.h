#ifndef __UARTDEV_H__
#define __UARTDEV_H__

#define UART_TX 22
#define UART_RX 23

#define BUF_SIZE 1024

int ModuleSim_uart_init();

int AT_tx(char *cmd);

int AT_rx(char *buf);

#endif //__UARTDEV_H__