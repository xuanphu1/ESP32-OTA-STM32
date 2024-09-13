#include "OTAGuess_STM32.h"

// Global variables to track state and data
uint8_t uart_flag_success_receive_data = 0;  // Flag to indicate successful UART data reception
uint8_t length_data_one_command_receive = 0;  // Length of the received command
uint8_t buffer_receive_command[40];  // Buffer to hold received UART data
uint8_t buff_mask_for_flashMemory[21];  // Buffer to store data for flash memory
uint8_t buff_data_push_flash[30];  // Buffer to push data to flash
uint8_t length_one_line_command;  // Length of a single line command
uint8_t num_segments = 0;  // Number of data segments
uint8_t path_of_data_frame = 0;  // Track the number of data frames received
Reconrd_Type_t record_type;  // Record type (used for different OTA records)

// Flag to select between memory program slots
flag_Memory_Flash flag_select_memory;

// Address pointer to write in memory
int poiter_addr_memory = 0;

struct min_context *minctx;  // Context for MIN protocol communication

// UART transmission-related functions for MIN protocol
void min_tx_start(uint8_t port) {
    // Function called when UART transmission starts (currently empty)
}
void min_tx_finished(uint8_t port) {
    // Function called when UART transmission finishes (currently empty)
}
uint16_t min_tx_space(uint8_t port) {
    // Return available space for UART transmission (512 bytes)
    return 512;
}
void min_tx_byte(uint8_t port, uint8_t byte) {
    // Transmit a byte via UART
    USART1->DR = byte;
    while (!(USART1->SR & USART_SR_TC));  // Wait until the transmission is complete
}
uint32_t min_time_ms(void) {
    // Get the current system tick (in milliseconds)
    return HAL_GetTick();
}

// MIN protocol handler to process received data and OTA commands
void min_application_handler(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload, uint8_t port) {
    // Handle different OTA command types based on the first byte in the payload
    switch (min_payload[0]) {
        case OTA_COMMAND_TYPE_START:
            // Handle the start of the OTA process
            length_one_line_command = min_payload[2];  // Get the length of the command

            // Calculate the number of data segments (divide the data into chunks of 4 bytes)
            if ((length_one_line_command - LENGTH_HEADER_COMMAND - 1) % 4 == 0) 
                num_segments = (length_one_line_command - LENGTH_HEADER_COMMAND - 1) / 4;
            else 
                num_segments = ((length_one_line_command - LENGTH_HEADER_COMMAND - 1) / 4) + 1;

            responsePrint("num_segments: %d,", num_segments);  // Print the number of segments
            ota_send_response(OTA_ACKNOWLEDGE);  // Acknowledge the start command
            break;

        case OTA_COMMAND_TYPE_HEADER:
            // Handle the header of the OTA process
            record_type = (Reconrd_Type_t)min_payload[5];  // Get the record type from the header
            ota_send_response(OTA_ACKNOWLEDGE);  // Acknowledge the header command
            break;

        case OTA_COMMAND_TYPE_DATA:
            // Handle the data segment of the OTA process
            if (path_of_data_frame < num_segments) {
                // Copy data to the buffer for flash memory
                memcpy(buff_data_push_flash + path_of_data_frame * 4, min_payload + POSITION_DATA_RECEIVE_ESP32, LENGTH_DATA_PUSHED_FLASH);
                path_of_data_frame++;  // Increment the data frame counter
            }
            if (path_of_data_frame == num_segments) {
                // All segments have been received, proceed with writing to flash
                responsePrint("check");

                if (record_type == RT_DATA_PUSHED_FLASH) {
                    // Read the current program memory flag
                    flag_select_memory = (flag_Memory_Flash)Flash_read_int(ADDR_FLAG_SELECT_PROGRAM);

                    // Write data to the appropriate memory location
                    switch (flag_select_memory) {
                        case Flag_Memory_Program_1:
                            Flash_write_array(ADDR_MEMORY_PROGRAM_1 + poiter_addr_memory * 16, buff_data_push_flash, length_one_line_command - LENGTH_HEADER_COMMAND - 1);
                            break;
                        case Flag_Memory_Program_2:
                            Flash_write_array(ADDR_MEMORY_PROGRAM_2 + poiter_addr_memory * 16, buff_data_push_flash, length_one_line_command - LENGTH_HEADER_COMMAND - 1);
                            break;
                    }
                }
                // Reset the data frame counter after processing the complete data segment
                path_of_data_frame = 0;
            }

            ota_send_response(OTA_ACKNOWLEDGE);  // Acknowledge the data command
            break;

        case OTA_COMMAND_TYPE_END:
            // Handle the end of the OTA process
            poiter_addr_memory++;  // Increment memory pointer

            if (record_type == RT_END_FILE) {
                // If the end of the file is reached, perform memory management
                switch (flag_select_memory) {
                    case Flag_Memory_Program_1:
                        Flash_erase(ADDR_FLAG_SELECT_PROGRAM, 1);
                        Flash_write_int(ADDR_FLAG_SELECT_PROGRAM, Flag_Memory_Program_2);
                        Flash_erase(ADDR_MEMORY_PROGRAM_2, 50);
                        bootLoader_handler(ADDR_MEMORY_PROGRAM_1);  // Boot from program 1
                        break;

                    case Flag_Memory_Program_2:
                        Flash_erase(ADDR_FLAG_SELECT_PROGRAM, 1);
                        Flash_write_int(ADDR_FLAG_SELECT_PROGRAM, Flag_Memory_Program_1);
                        Flash_erase(ADDR_MEMORY_PROGRAM_1, 50);
                        bootLoader_handler(ADDR_MEMORY_PROGRAM_2);  // Boot from program 2
                        break;
                }
                // Reset the memory pointer
                poiter_addr_memory = 0;
                ota_send_response(OTA_ACKNOWLEDGE);  // Acknowledge the end command
                // Optionally, trigger a soft reset if needed
                NVIC_SystemReset();
            }

            ota_send_response(OTA_ACKNOWLEDGE);  // Acknowledge the end command
            break;

        default:
            // Handle any invalid commands
            ota_send_response(OTA_NOT_ACKNOWLEDGE);  // Send negative acknowledgment
            memset(buffer_receive_command, 0, sizeof(buffer_receive_command));  // Clear the receive buffer
            length_data_one_command_receive = 0;  // Reset the length of received command
            break;
    }

    // Reset the receive buffer and command length after processing
    memset(buffer_receive_command, 0, sizeof(buffer_receive_command));
    length_data_one_command_receive = 0;
}

// Initialize the MIN protocol for communication
void initMin(struct min_context *_minctx) {
    minctx = _minctx;
    min_init_context(minctx, MIN_PORT);  // Initialize the MIN context for the specified port
}

// Send data via the MIN protocol
static void booLoader_send_data(void* data, uint8_t len) {
    min_send_frame(minctx, MIN_ID, (uint8_t*)data, len);  // Send a data frame via MIN
}

// Send a response to STM32 via the MIN protocol
void ota_send_response(OTA_Response_Type response_type) {
    OTA_Response response;
    response.command_id = OTA_COMMAND_TYPE_RESPONSE;  // Set the response command ID
    response.length = 1;  // Set the response length
    response.response_code = response_type;  // Set the response code (ACK/NACK)
    booLoader_send_data(&response, sizeof(response));  // Send the response
}

// Handle data received from ESP32 via UART
void receiveData_From_ESP32(uint8_t data) {
    if (data == '\n') {
        // End of data, mark the command as complete
        buffer_receive_command[length_data_one_command_receive++] = '\0';  // Null-terminate the buffer
        uart_flag_success_receive_data = 1;  // Set flag indicating successful reception
    } else {
        // Store received byte in the buffer
        buffer_receive_command[length_data_one_command_receive++] = data;
    }
}

// Handle the response command from ESP32 using MIN protocol
void response_command_of_ESP32_handle() {
    min_poll(minctx, buffer_receive_command, LENGTH_COMPLETE_COMMAND);  // Poll and process the received command
}
