#ifndef CLI_COMMAND_H
#define CLI_COMMAND_H
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "response_cli.h"
#include "cli_types.h"
#include "getData.h"

void cli_command_excute(char *uart_buff,uint8_t len);


#endif

