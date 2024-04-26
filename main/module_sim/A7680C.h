#ifndef __A7680C_H__
#define __A7680C_H__

#include "driver/gpio.h"

#define BROKER_ADDR "broker.hivemq.com"
#define BROKER_PORT "1883"

int ModuleSim_Task_Init();

void connect_mqtt_broker();

#endif // __A7680C_H__