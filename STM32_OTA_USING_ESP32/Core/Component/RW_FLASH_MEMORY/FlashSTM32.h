#ifndef __FLASH_STM32_H
#define __FLASH_STM32_H
#include "stdint.h"
#include "string.h"
#include "stm32f1xx.h"
#include "response_cli.h"

#pragma pack (1)
/**
 * @brief Struct to store Wi-Fi information.
 * 
 * This structure stores Wi-Fi information including SSID and password.
 */
typedef struct {
    uint8_t no;            ///< Unique number or identifier
    uint8_t ssid[30];      ///< Wi-Fi SSID (name)
    uint8_t password[30];  ///< Wi-Fi password
} wifi_info_t;
#pragma pack ()

/**
 * @brief Erase specific flash memory pages.
 * 
 * This function erases the flash memory starting from the given address and for the 
 * specified number of pages.
 * 
 * @param address Starting address of the flash memory to be erased.
 * @param number_page_erase Number of pages to erase.
 */
void Flash_erase(uint32_t address, int number_page_erase);

/**
 * @brief Write an integer value to flash memory.
 * 
 * This function writes a 16-bit integer value to a specific address in flash memory.
 * 
 * @param address Address where the integer will be written.
 * @param value Integer value to write.
 */
void Flash_write_int(uint32_t address, int value);

/**
 * @brief Write a float value to flash memory.
 * 
 * This function writes a 32-bit float value to a specific address in flash memory.
 * 
 * @param address Address where the float will be written.
 * @param value Float value to write.
 */
void Flash_write_float(uint32_t address, float value);

/**
 * @brief Write an array of data to flash memory.
 * 
 * This function writes an array of data (bytes) to the specified address in flash memory.
 * 
 * @param address Starting address where the array will be written.
 * @param arr Pointer to the array of data to write.
 * @param len Length of the array.
 */
void Flash_write_array(uint32_t address, uint8_t *arr, uint16_t len);

/**
 * @brief Write a struct to flash memory.
 * 
 * This function writes a `wifi_info_t` structure to a specific address in flash memory.
 * 
 * @param address Address where the structure will be written.
 * @param data Structure to be written to flash memory.
 */
void Flash_write_Struct(uint32_t address, wifi_info_t data);

/**
 * @brief Read an integer value from flash memory.
 * 
 * This function reads a 16-bit integer value from a specific address in flash memory.
 * 
 * @param address Address from where the integer will be read.
 * @return Integer value read from flash memory.
 */
int Flash_read_int(uint32_t address);

/**
 * @brief Read a float value from flash memory.
 * 
 * This function reads a 32-bit float value from a specific address in flash memory.
 * 
 * @param address Address from where the float will be read.
 * @return Float value read from flash memory.
 */
float Flash_read_float(uint32_t address);

/**
 * @brief Read an array of data from flash memory.
 * 
 * This function reads an array of data (bytes) from a specific address in flash memory.
 * 
 * @param address Starting address to read data from.
 * @param arr Pointer to the buffer where the read data will be stored.
 * @param len Length of the array to read.
 */
void Flash_read_array(uint32_t address, uint8_t *arr, uint16_t len);

/**
 * @brief Read a struct from flash memory.
 * 
 * This function reads a `wifi_info_t` structure from a specific address in flash memory.
 * 
 * @param address Address from where the structure will be read.
 * @param data Pointer to the structure where the read data will be stored.
 */
void Flash_read_struct(uint32_t address, wifi_info_t *data);

#endif
