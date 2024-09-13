#ifndef RESPONSE_CLI_H
#define RESPONSE_CLI_H
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdarg.h"


void responsePrint(const char*str,...);
void responseInit(UART_HandleTypeDef *_huart_select);

#endif

