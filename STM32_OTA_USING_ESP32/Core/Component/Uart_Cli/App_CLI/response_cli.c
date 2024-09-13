#include "response_cli.h"

UART_HandleTypeDef *huart_print;

void responsePrint(const char*str,...){

    char stringArray[100];

    va_list args;
    va_start(args,str);
    vsprintf(stringArray,str,args);
    va_end(args);

    HAL_UART_Transmit(huart_print,(uint8_t *)stringArray,strlen(stringArray),100);

}

void responseInit(UART_HandleTypeDef *_huart_select){

	huart_print = _huart_select ;
}


