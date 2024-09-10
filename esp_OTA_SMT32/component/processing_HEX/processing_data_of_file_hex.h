#ifndef PROCESSING_DATA_OF_FILE_HEX_H
#define PROCESSING_DATA_OF_FILE_HEX_H

#include "stdio.h"
#include "stdint.h"
#include "string.h"

// Buffer size for one line of hex command
#define BUFF_HEX_CMD_ONE_LINE_SIZE  21

// Enum to represent status of conversions (OK or Error)
typedef enum {
    STATUS_OK,       // Operation succeeded
    STATUS_ERROR     // Operation failed
} status_convert;

// Function declarations

/**
 * @brief Copies a portion of the source array to the destination array, with bounds checking.
 * 
 * @param source The source array from which to copy.
 * @param destination The destination array to which the data will be copied.
 * @param length The number of bytes to copy.
 * @param start_pos_src The starting position in the source array.
 * @param start_pos_dst The starting position in the destination array.
 * @param length_source The length of the source array.
 * @param length_destination The length of the destination array.
 */
void copy_array(const uint8_t *source, uint8_t *destination, size_t length, size_t start_pos_src, size_t start_pos_dst, uint8_t length_source, uint8_t length_destination);

/**
 * @brief Converts the contents of a hex file into a 2D array, performing checksum validation.
 * 
 * @param file_hex The string representing the contents of the hex file.
 * @param array2D_Save_Data The 2D array where the converted data will be stored.
 */
void save_data_convert_file_hex_to_Array2D(char* file_hex, uint8_t **array2D_Save_Data);

/**
 * @brief Converts a single line of hex data into a 1D byte array, with checksum validation.
 * 
 * @param line The string representing a single line in the hex file.
 * @param array1D_Save_Data The byte array where the converted data will be stored.
 */
void save_data_convert_single_line_to_array1D(char* line, uint8_t* array1D_Save_Data);

/**
 * @brief Determines the length (in bytes) of a single line command in the hex file.
 * 
 * @param dataLine The string representing the single command line.
 * @param length_one_line_command A pointer to store the length of the command line.
 */
void getData_length_one_line_command(char* dataLine, uint8_t *length_one_line_command);

/**
 * @brief Calculates the total number of command lines in the hex file.
 * 
 * @param file_hex The string representing the contents of the hex file.
 * @param length_command A pointer to store the total number of command lines.
 */
void getData_length_command(char* file_hex, uint8_t *length_command);

#endif  // PROCESSING_DATA_OF_FILE_HEX_H
