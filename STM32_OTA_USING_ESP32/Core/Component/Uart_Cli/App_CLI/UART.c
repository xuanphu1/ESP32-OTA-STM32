#include "UART.h"


char uart_buff[MAX_DATA_UART] ;
uint8_t uart_len;
uint8_t uart_flag = 0;
void receiveData(uint8_t data){

	if(data == '\n'){
		
		uart_buff[uart_len++] = '\0';
		uart_flag = 1;
	}
	else {
	
		uart_buff[uart_len++] = data;
	}
}

void uart_init(void){

	
}

void uart_handle(void){

if(uart_flag){
		
		cli_command_excute(uart_buff,uart_len);
	
		
		uart_flag = 0 ;
		uart_len = 0 ;
	
	
	}
}