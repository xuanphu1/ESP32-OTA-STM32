#ifndef UART_H
#define UART_H
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "getData.h"
#include "setData.h"
#include "cli_command.h"

#define MAX_DATA_UART 100

void receiveData(uint8_t data);
void uart_init(void);
void uart_handle(void);
#endif

