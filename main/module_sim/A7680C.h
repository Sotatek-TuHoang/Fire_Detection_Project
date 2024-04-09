#ifndef __A7680C_H__
#define __A7680C_H__

#include "driver/gpio.h"

int ModuleSim_Task_Init();

void send_sms(char *phone_num, char *msg);

#endif // __A7680C_H__