#include "OTAHost_esp32.h"

// Declare local flags for tracking status
static bool flag_uart = false;  // Tracks whether UART communication is ongoing
static bool flag_response_of_STM32 = false;  // Tracks if STM32 has acknowledged the command
uint8_t **array_save_data_convert_file_hex;  // Holds the converted hex data
int position_path_data = 0;  // Tracks the position of the data during OTA transfer

// Declare the `min_context` structure for MIN protocol communication
struct min_context *minctx;

// MIN protocol communication functions (sending data via UART)
void min_tx_start(uint8_t port) { }
void min_tx_finished(uint8_t port) { }
uint16_t min_tx_space(uint8_t port) { 
    return 512;  // Return the available space for data transmission
}

// Send a byte of data via UART
void min_tx_byte(uint8_t port, uint8_t byte) {
    uart_write_bytes(UART2_PORT_NUM, (const void*)&byte, 1);  // Transmit byte via UART
}

// Returns the current time in milliseconds
uint32_t min_time_ms(void) { 
    return esp_timer_get_time(); 
}

// Handle responses from STM32 via the MIN protocol
void min_application_handler(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload, uint8_t port) {
    // Check the type of response received from STM32
    if (min_payload[0] == OTA_COMMAND_TYPE_RESPONSE && min_payload[1] == 0x01) {  // If the response has a valid length
        switch (min_payload[2]) {
        case OTA_ACKNOWLEDGE:  // Command successfully received by STM32
            printf("STM32 receives the command successfully\n");
            flag_response_of_STM32 = true;  // Set flag indicating success
            break;
        case OTA_NOT_ACKNOWLEDGE:  // Command failed to be received by STM32
            printf("STM32 failed to receive the command\n");
            flag_response_of_STM32 = false;  // Set flag indicating failure
            break;
        default:
            printf("Invalid command response\n");
            flag_response_of_STM32 = false;
            break;
        }
    } else {
        printf("Invalid Command ID\n");
        flag_response_of_STM32 = false;
    }
    flag_uart = false;  // Reset flag to prevent repeated polling
    uart_flush_input(UART2_PORT_NUM);  // Clear the UART buffer to prepare for new data
}

// Initialize the MIN protocol for communication with STM32
void initMin(struct min_context *_minctx) {
    minctx = _minctx;
    min_init_context(minctx, MIN_PORT);  // Initialize MIN context for the specified port
}

// Send data via the MIN protocol
static void booLoader_send_data(void* data, uint8_t len) {
    min_send_frame(minctx, MIN_ID, (uint8_t*)data, len);  // Send a data frame via MIN
}

// Send OTA data with a specific command type
void ota_send_data(OTA_Data_Type data_type, uint8_t *_data_array, uint8_t length) {
    // Allocate memory for the OTA command structure, accounting for the size of the data
    OTA_Command *cmd = (OTA_Command *)malloc(sizeof(OTA_Command) + length - 1);

    cmd->command_id = data_type;  // Set the command ID to the specified data type
    cmd->length = length;  // Set the length of the data
    memcpy(cmd->data_array, _data_array, length);  // Copy the data into the command structure

    booLoader_send_data(cmd, sizeof(cmd->command_id) + sizeof(cmd->length) + length);  // Send the command

    free(cmd);  // Free the allocated memory after sending
}

// Send either the START or END command for OTA
void ota_send_start_end(OTA_Data_Type start_or_end_command_type, uint8_t length_one_command) {
    OTA_Command ota_open_command;
    ota_open_command.command_id = start_or_end_command_type;  // Set command type to START or END
    ota_open_command.length = 1;
    memcpy(ota_open_command.data_array, &length_one_command, 1);  // Copy the length of the command
    booLoader_send_data(&ota_open_command, sizeof(ota_open_command));  // Send the command
}

// Handle response from STM32 via UART
void response_command_of_STM32_handle() {
    uint8_t data[BUF_UART_SIZE];
    int len = uart_read_bytes(UART2_PORT_NUM, data, BUF_UART_SIZE, 2000 / portTICK_PERIOD_MS);  // Read data from UART buffer
    if (len > 0 && !flag_uart) {
        flag_uart = true;  // Set flag indicating data has been received
        min_poll(minctx, data, len);  // Poll the data using MIN protocol
    }
}

// Main function to start OTA process using hex file data
void otaStart(char *file_hex, bool *status_kick_off) {
    if (*status_kick_off) {  // Check if the OTA process has been triggered
        storeDataOfHex data_of_hex;
        uint8_t length_one_line_command;
        uint8_t length_command;

        // Calculate the total number of commands/lines in the hex file
        getData_length_command(file_hex, &length_command);
        printf("Length of commands in hex file: %d\n", length_command);

        // Create a copy of the hex file data to avoid modifying the original string
        char* file_hex_copy = strdup(file_hex);
        printf("Full hex file data:\n%s\n", file_hex_copy);

        // Extract the first line from the hex data
        char* data_line = strtok(file_hex_copy, "\n");

        // Process each command/line in the hex file
        for (int row = 0; row < length_command && data_line != NULL; row++) {
            uint8_t data_line_copy[22];  // Buffer to store byte representation of hex line

            // Convert the current hex line to a byte array
            save_data_convert_single_line_to_array1D(data_line, data_line_copy);

            // Get the length of the current command/line
            getData_length_one_line_command(data_line, &length_one_line_command);
            printf("\n");
            ESP_LOGI("ROW DATA FRAME", "Processing row %d\n", row);
            printf("Length of data in row %d: %d\n", row, length_one_line_command);

            // Dynamically allocate memory for the current data row
            data_of_hex.Data_hex = (uint8_t*)malloc((length_one_line_command - LENGTH_HEADER_COMMAND) * sizeof(uint8_t));
            printf("Header Frame: ");

            // Copy the header data from the current command
            copy_array(data_line_copy, data_of_hex.Header_hex, LENGTH_HEADER_COMMAND, POSITION_HEADER, 0, length_one_line_command, LENGTH_HEADER_COMMAND);
            printf("Data Frame: ");

            // Copy the main data from the current command
            copy_array(data_line_copy, data_of_hex.Data_hex, length_one_line_command - LENGTH_HEADER_COMMAND, POSITION_DATA, 0, length_one_line_command, length_one_line_command - LENGTH_HEADER_COMMAND);

            // Send OTA commands (START, HEADER, DATA, END) to STM32
            for (OTA_Data_Type Command = OTA_COMMAND_TYPE_START; Command < sizeof(OTA_Data_Type); Command++) {
                switch (Command) {
                    case OTA_COMMAND_TYPE_START:  // Send START command
                        while (!flag_response_of_STM32) {
                            ota_send_start_end(OTA_COMMAND_TYPE_START, length_one_line_command);  // Send the START command
                            ESP_LOGI("OTA_HOST", "ESP32 successfully sent the START command");
                            response_command_of_STM32_handle();  // Handle response from STM32
                            vTaskDelay(500 / portTICK_PERIOD_MS);
                        }
                        flag_response_of_STM32 = false;
                        break;

                    case OTA_COMMAND_TYPE_HEADER:  // Send HEADER command
                        printf("Header Data: ");
                        for (int i = 0; i < LENGTH_HEADER_COMMAND; i++) {
                            printf("%02X ", data_of_hex.Header_hex[i]);
                        }
                        printf("\n");

                        while (!flag_response_of_STM32) {
                            ota_send_data(OTA_COMMAND_TYPE_HEADER, data_of_hex.Header_hex, LENGTH_HEADER_COMMAND);  // Send HEADER command
                            ESP_LOGI("OTA_HOST", "ESP32 successfully sent the HEADER command");
                            response_command_of_STM32_handle();
                            vTaskDelay(500 / portTICK_PERIOD_MS);
                        }
                        flag_response_of_STM32 = false;
                        break;

                    case OTA_COMMAND_TYPE_DATA:  // Send DATA command
                        printf("Main Data: ");
                        for (int i = 0; i < length_one_line_command - LENGTH_HEADER_COMMAND; i++) {
                            printf("%02X ", data_of_hex.Data_hex[i]);
                        }
                        printf("\n");

                        while (!flag_response_of_STM32) {
                            ota_send_data(OTA_COMMAND_TYPE_DATA, data_of_hex.Data_hex, length_one_line_command - LENGTH_HEADER_COMMAND);  // Send DATA command
                            ESP_LOGI("OTA_HOST", "ESP32 successfully sent the DATA command");
                            response_command_of_STM32_handle();
                            vTaskDelay(500 / portTICK_PERIOD_MS);
                        }
                        flag_response_of_STM32 = false;
                        break;

                    case OTA_COMMAND_TYPE_END:  // Send END command
                        while (!flag_response_of_STM32) {
                            ota_send_start_end(OTA_COMMAND_TYPE_END, 0);  // Send END command
                            ESP_LOGI("OTA_HOST", "ESP32 successfully sent the END command");
                            response_command_of_STM32_handle();
                            vTaskDelay(500 / portTICK_PERIOD_MS);
                        }
                        flag_response_of_STM32 = false;
                        break;

                    default:
                        ESP_LOGE("OTA_HOST", "ESP32 encountered an invalid command structure");
                        break;
                }
            }

            // Free the allocated memory for the data row
            free(data_of_hex.Data_hex);

            // Retrieve the next line from the hex file
            data_line = strtok(NULL, "\n");
        }

        // Free the copied hex file data
        free(file_hex_copy);
        *status_kick_off = false;
        ESP_LOGI("END OTA", "OTA process completed successfully!");
        // Pause to avoid watchdog errors
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
