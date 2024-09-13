#ifndef OTA_GUESS_STM32_H
#define OTA_GUESS_STM32_H


#include "min.h"
#include "stdio.h"
#include "stm32f1xx_hal.h"
#include "response_cli.h"
#include "FlashSTM32.h"
#include "processing_data_of_file_hex.h"
#include "BootLoader.h"
#include <stdlib.h>



#define MIN_PORT 0
#define MIN_ID 0

#define BUF_UART_SIZE       1024 // Buffer size for UART

#define POSITION_HEADER 1
#define POSITION_DATA_RECEIVE_ESP32 (2)
#define POSITION_DATA 5
#define LENGTH_HEADER_COMMAND 4
#define LENGTH_DATA_PUSHED_FLASH 16
#define LENGTH_COMPLETE_COMMAND 21



			

// Enumeration for different types of OTA data
typedef enum {
    OTA_COMMAND_TYPE_START,
    OTA_COMMAND_TYPE_HEADER,
    OTA_COMMAND_TYPE_DATA,
    OTA_COMMAND_TYPE_END,
    OTA_COMMAND_TYPE_RESPONSE
} OTA_Data_Type;

typedef enum {
	RT_DATA_PUSHED_FLASH,
	RT_END_FILE,
	RT_EXTENDED_ADDR,
	null,
	RT_BASE_ADDR,
	RT_START_ADDR
	
} Reconrd_Type_t;


typedef enum {
	
	Flag_Memory_Program_1,
	Flag_Memory_Program_2

}flag_Memory_Flash;



// Enumeration for OTA responses
typedef enum {
    OTA_ACKNOWLEDGE,
    OTA_NOT_ACKNOWLEDGE
} OTA_Response_Type;

// Struct for OTA command data
typedef struct {      
    uint8_t command_id;      // Identifier for the command
    uint8_t length;          // Length of the operation code data
    uint8_t data_array[1];  // To hold the operation code (dynamic size)
} __attribute__((packed)) OTA_Command;

// Struct for OTA response data
typedef struct {      
    uint8_t command_id;  // Identifier for the command
    uint8_t length;      // Length of the response data
    uint8_t response_code; // OTA response code

} __attribute__((packed)) OTA_Response;


typedef struct 
{
    uint8_t Header_hex[4]; // Length(1byte) + Address(2byte) + Record type(1byte)
    uint8_t *Data_hex; // Data(16byte) + Checksum(1byte)
} __attribute__((packed)) storeDataOfHex;


/**
 * @brief Initializes the MIN protocol for communication.
 * 
 * This function initializes the communication protocol for sending and receiving data 
 * using the MIN (Microcontroller Interconnect Network) protocol.
 * 
 * @param min_ctx The MIN context structure used for protocol initialization.
 */
void initMin(struct min_context *min_ctx);

/**
 * @brief Sends an OTA response to the ESP32.
 * 
 * This function sends an acknowledgment (ACK) or non-acknowledgment (NACK) response 
 * back to the ESP32 after receiving a command.
 * 
 * @param response_type Type of response (OTA_ACKNOWLEDGE or OTA_NOT_ACKNOWLEDGE).
 */
void ota_send_response(OTA_Response_Type response_type);

/**
 * @brief Handles incoming commands from ESP32 via the MIN protocol.
 * 
 * This function processes the commands received from the ESP32 through the MIN protocol. 
 * It checks for complete commands and processes them accordingly.
 */
void response_command_of_ESP32_handle(void);

/**
 * @brief Receives data from the ESP32 via UART.
 * 
 * This function handles byte-by-byte data reception from the ESP32 via UART. 
 * It stores the received bytes in a buffer and flags when a full command has been received.
 * 
 * @param data The byte of data received from ESP32.
 */
void receiveData_From_ESP32(uint8_t data);




#endif
