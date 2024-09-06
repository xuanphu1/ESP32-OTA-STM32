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


// Enumeration for different types of OTA data
typedef enum {
    OTA_COMMAND_TYPE_START,
    OTA_COMMAND_TYPE_HEADER,
    OTA_COMMAND_TYPE_DATA,
    OTA_COMMAND_TYPE_END,
    OTA_COMMAND_TYPE_RESPONSE
} OTA_Data_Type;

// Enumeration for OTA operation codes
// typedef enum {
//     OTA_START_CODE,
//     OTA_STOP_CODE,
//     OTA_REQUEST_CODE
// } OTA_Operation_Code;

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
void initMin(struct min_context *min_ctx);
void ota_send_data(OTA_Data_Type data_type, uint8_t *operation_code, uint8_t length);
void ota_send_start_end(OTA_Data_Type open_command_type);
void ota_send_response(OTA_Response_Type response_type);
void response_command_of_STM32_handle();
void otaStart(char *file_hex,bool *status_kick_off);

#endif // HOST_BOOTLOADER_H
