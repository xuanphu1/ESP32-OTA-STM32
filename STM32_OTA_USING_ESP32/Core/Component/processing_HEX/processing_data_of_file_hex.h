#ifndef PROCESSING_DATA_OF_FILE_HEX_H
#define PROCESSING_DATA_OF_FILE_HEX_H
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "response_cli.h"

#define BUFF_HEX_CMD_ONE_LINE_SIZE  21

typedef enum {
    STATUS_OK,
    STATUS_ERROR
} status_convert;

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



#endif


