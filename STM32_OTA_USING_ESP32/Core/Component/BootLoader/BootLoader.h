#ifndef BOOTLOADER_H
#define BOOTLOADER_H
#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "stdint.h"



#define ADDR_MEMORY_PROGRAM_1 0x08005000
#define ADDR_MEMORY_PROGRAM_2 0x08011800
#define ADDR_FLAG_SELECT_PROGRAM 0x0801E000



void bootLoader_handler(uint32_t addr_app_program);

#endif


