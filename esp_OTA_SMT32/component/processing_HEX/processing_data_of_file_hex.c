#include "processing_data_of_file_hex.h"

// Convert a single hexadecimal character into a byte (4-bit value)
static uint8_t char_to_byte(char c) {
    if (c >= '0' && c <= '9') return (c - '0');
    else if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    else if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    return 0;
}

// Validate checksum for a given buffer (checks if the sum of bytes is valid)
static uint8_t check_sum(uint8_t* buff, uint8_t len) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < len - 1; i++) {
        sum += buff[i];  // Sum all bytes except the last one
    }
    sum = ~sum;  // Invert the sum
    sum += 1;
    if (sum == buff[len - 1]) return STATUS_OK;  // Check if the sum matches the last byte (checksum)
    return STATUS_ERROR;
}

// Swap two bytes of data
static void swap(uint8_t* a, uint8_t* b) {
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

// Swap 4 bytes of data (used for converting endianness or byte order)
static void swap_4_byte(uint8_t* data, uint8_t len) {
    for (int i = 0; i < len; i += 4) {
        swap(&data[i], &data[i + 3]);
        swap(&data[i + 1], &data[i + 2]);
    }
}

// Convert a string of hexadecimal characters into bytes and store them in an output array
static void convert_string_to_byte(char* input, uint8_t* output) {
    uint8_t index = 0;
    if (*input == ':') {  // Ignore the starting colon (':') in Intel HEX format
        input++;
        while (*input != '\0') {
            uint8_t hex_value = char_to_byte(*input++) << 4;  // Convert two hex chars to a byte
            hex_value |= char_to_byte(*input++);
            output[index++] = hex_value;  // Store byte in output array
        }
    }
}

// Copy a subset of an array from source to destination with bounds checking
void copy_array(const uint8_t *source, uint8_t *destination, size_t length, size_t start_pos_src, size_t start_pos_dst, uint8_t length_source, uint8_t length_destination) {
    if (start_pos_src + length <= length_source && start_pos_dst + length <= length_destination) {
        memcpy(destination + start_pos_dst, source + start_pos_src, length);  // Copy the specified section of the array
        for (int i = 0; i < length; i++) {
            printf("%02X ", destination[i]);  // Print each byte in hexadecimal format
        }
        printf("\n");
    } else {
        printf("Error: Out of bounds.\n");
    }
}

// Convert the contents of a hex file to a 2D array while checking each line's checksum
void save_data_convert_file_hex_to_Array2D(char* file_hex, uint8_t **array2D_Save_Data) {
    char *file_hex_msk = malloc(strlen(file_hex) + 1);  // Allocate memory for hex data copy
    strcpy(file_hex_msk, file_hex);
    char* token = strtok(file_hex_msk, "\n");  // Tokenize the file by lines
    int row = 0;
    while (token != NULL) {
        convert_string_to_byte(token, array2D_Save_Data[row]);  // Convert each line to bytes
        if (check_sum(array2D_Save_Data[row], array2D_Save_Data[row][0] + 5) != STATUS_OK) {  // Verify checksum
            printf("Checksum error\n");
            break;
        }
        // if (array2D_Save_Data[row][3] == 0x00) {
        //     swap_4_byte(&array2D_Save_Data[row][4], array2D_Save_Data[row][0]);  // Perform byte swapping if necessary
        // }
        row++;
        token = strtok(NULL, "\n");  // Get the next line
    }
    free(file_hex_msk);  // Free allocated memory after use
}

// Convert a single line of hex data into a byte array and validate the checksum
void save_data_convert_single_line_to_array1D(char* line, uint8_t* array1D_Save_Data) {
    convert_string_to_byte(line, array1D_Save_Data);  // Convert the string into a byte array

    // Validate checksum for the converted line
    if (check_sum(array1D_Save_Data, array1D_Save_Data[0] + 5) != STATUS_OK) {
        printf("Checksum error in line: %s\n", line);  // Print error if checksum is invalid
    } else {
        // Perform byte swapping if the data type is 0x00
        // if (array1D_Save_Data[3] == 0x00) {
        //     swap_4_byte(&array1D_Save_Data[4], array1D_Save_Data[0]);
        // }
    }
}

// Calculate the length of a single hex command line by counting its characters (excluding ':' and line terminators)
void getData_length_one_line_command(char* dataLine, uint8_t *length_one_line_command) {
    char* temp_ptr = dataLine;  // Use a temporary pointer to avoid modifying the original string

    if (*temp_ptr == ':') {
        temp_ptr++;  // Skip the colon character at the start of the line
        *length_one_line_command = 0;
        while (*temp_ptr != '\0' && *temp_ptr != '\n' && *temp_ptr != '\r') {  // Loop until end of line or end of string
            temp_ptr += 2;  // Each byte is represented by two hex characters
            (*length_one_line_command)++;
        }
    }
}

// Calculate the total number of commands (lines) in the hex file by counting lines that start with ':'
void getData_length_command(char* file_hex, uint8_t *length_command) {
    char *file_hex_msk = malloc(strlen(file_hex) + 1);  // Allocate memory for hex data copy
    strcpy(file_hex_msk, file_hex);
    char* token = strtok(file_hex_msk, "\n");  // Tokenize the hex file by lines
    int i = 0;
    while (token != NULL) {
        if (*token == ':') {
            i++;  // Increment the count for each line starting with ':'
        }
        token = strtok(NULL, "\n");  // Get the next line
    }
    *length_command = i;  // Set the total number of commands
    free(file_hex_msk);  // Free allocated memory after use
}
