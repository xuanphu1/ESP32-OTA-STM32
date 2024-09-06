#ifndef PROCESSING_DATA_OF_FILE_HEX_H
#define PROCESSING_DATA_OF_FILE_HEX_H
#include "stdio.h"
#include "stdint.h"
#include "string.h"


#define BUFF_HEX_CMD_ONE_LINE_SIZE  21

typedef enum {
    STATUS_OK,
    STATUS_ERROR
} status_convert;

void copy_array(uint8_t *source, uint8_t *destination, size_t length, size_t start_pos, uint8_t length_source);
void save_data_convert_file_hex(char* file_hex, uint8_t **array2D_Save_Data);
void getData_length_one_line_command_length_command(char* file_hex, uint8_t *length_one_line_command, uint8_t *length_command);
#endif