#ifndef OTA_HOST_ESP32_H
#define OTA_HOST_ESP32_H

#include "driver/uart.h"
#include "../Min/min.h"
#include "esp_timer.h"
#include "string.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "processing_data_of_file_hex.h"

#define MIN_PORT 0
#define MIN_ID 0

#define UART2_PORT_NUM      2
#define UART2_BAUD_RATE     115200
#define UART2_TX_PIN        17   // TX pin for UART2
#define UART2_RX_PIN        16   // RX pin for UART2
#define BUF_UART_SIZE       1024 // Buffer size for UART

#define POSITION_HEADER 0
#define POSITION_DATA 4
#define LENGTH_HEADER_COMMAND 4 
#define LENGTH_PATH_OF_FRAME_DATA 4 


// Enumeration for different types of OTA data
typedef enum {
    OTA_COMMAND_TYPE_START,
    OTA_COMMAND_TYPE_HEADER,
    OTA_COMMAND_TYPE_DATA,
    OTA_COMMAND_TYPE_END,
    OTA_COMMAND_TYPE_RESPONSE
} OTA_Data_Type;

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


// Function prototypes
/**
 * @brief Initializes the MIN protocol.
 * 
 * This function initializes the MIN protocol for ESP32, allowing communication between ESP32 and STM32.
 * 
 * @param min_ctx Pointer to the `min_context` structure used to manage the MIN communication.
 */
void initMin(struct min_context *min_ctx);

/**
 * @brief Sends OTA data with a specified type.
 * 
 * This function sends a data frame containing an operation code with a specified command type (data_type)
 * to STM32.
 * 
 * @param data_type Type of OTA data to send (e.g., START, HEADER, DATA, END).
 * @param operation_code The operation code or data to be sent within the frame.
 * @param length The length of the data in the `operation_code`.
 */
void ota_send_data(OTA_Data_Type data_type, uint8_t *operation_code, uint8_t length);

/**
 * @brief Sends START or END command for the OTA process.
 * 
 * This function sends the start or end command for the OTA process. The START command is sent before
 * transmitting data, and the END command is sent after all data has been transmitted.
 * 
 * @param start_or_end_command_type The type of command (START or END) to be sent.
 * @param length_one_command The length of the command to be sent, typically the number of bytes in a command frame.
 */
void ota_send_start_end(OTA_Data_Type start_or_end_command_type, uint8_t length_one_command);

/**
 * @brief Handles the response from STM32.
 * 
 * This function reads and processes the response from STM32 over UART. It checks the type of response
 * received (ACK, NACK, etc.) to determine whether the data transmission was successful.
 */
void response_command_of_STM32_handle();

/**
 * @brief Starts the OTA process using hex file data.
 * 
 * This function initiates the OTA process by parsing and sending data from a hex file. The hex file
 * is divided into smaller data lines, each of which is sent sequentially to STM32. The process includes
 * sending START, HEADER, DATA, and END commands.
 * 
 * @param file_hex String containing the hex file data.
 * @param status_kick_off Pointer to a flag to track whether the OTA process has started.
 */
void otaStart(char *file_hex, bool *status_kick_off);


#endif // HOST_BOOTLOADER_H
