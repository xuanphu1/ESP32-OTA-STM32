#include "BootLoader.h"



typedef void(*run_bootloader_handler) (void);
run_bootloader_handler run_BL_handler;

void bootLoader_handler(uint32_t addr_app_program){

    /*Turn off Peripheral, Clear Interrupt Flag*/
    HAL_RCC_DeInit();
    /*Clear Pending Interrupt Request, turn off System Tick*/
    HAL_DeInit(); // Turn off fault harder
    SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk| 
                    SCB_SHCSR_BUSFAULTENA_Msk|
                    SCB_SHCSR_MEMFAULTENA_Msk);

    // Set Main Stack Pointer
    __set_MSP(*((volatile uint32_t*)addr_app_program));
    run_BL_handler = (run_bootloader_handler)*((volatile uint32_t*)(addr_app_program + 4));
    run_BL_handler();
}

