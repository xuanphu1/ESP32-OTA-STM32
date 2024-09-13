#include "processing_data_of_file_hex.h"


static uint8_t char_to_byte(char c) {
    if (c >= '0' && c <= '9') return (c - '0');
    else if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    else if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    return 0;
}

static uint8_t check_sum(uint8_t* buff, uint8_t len) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < len - 1; i++) {
        sum += buff[i];
    }
    sum = ~sum;
    sum += 1;
    if (sum == buff[len - 1]) return STATUS_OK;
    return STATUS_ERROR;
}



static void swap(uint8_t* a, uint8_t* b) {
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

static void swap_4_byte(uint8_t* data, uint8_t len) {
    for (int i = 0; i < len; i += 4) {
        swap(&data[i], &data[i + 3]);
        swap(&data[i + 1], &data[i + 2]);
    }
}





static void convert_string_to_byte(char* input, uint8_t* output) {
    uint8_t index = 0;
    if (*input == ':') {
        input++;
        while (*input != '\0') {
            uint8_t hex_value = char_to_byte(*input++) << 4; // bit cao
            hex_value |= char_to_byte(*input++);
            output[index++] = hex_value;
        }
    }
}

void copy_array(const uint8_t *source, uint8_t *destination, size_t length, size_t start_pos_src, size_t start_pos_dst, uint8_t length_source, uint8_t length_destination) {
    // Kiểm tra nếu vị trí bắt đầu và độ dài nằm ngoài phạm vi mảng nguồn
    if (start_pos_src + length <= length_source && start_pos_dst + length <= length_destination + 1) {
        memcpy(destination + start_pos_dst, source + start_pos_src, length);
    } else {
        responsePrint("Error: Out of bounds.\n");
    }
}

