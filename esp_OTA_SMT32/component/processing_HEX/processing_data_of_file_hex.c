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

void copy_array(uint8_t *source, uint8_t *destination, size_t length, size_t start_pos, uint8_t length_source) {
    // Kiểm tra nếu vị trí bắt đầu và độ dài nằm ngoài phạm vi mảng nguồn
    if (start_pos + length <= length_source) {
        memcpy(destination, source + start_pos, length);
        for (int i = 0; i < length; i++) {
                printf("%02X ", destination[i]);  // In từng byte dưới dạng hex
        }
        printf("\n");
    } else {
        printf("Error: Out of bounds.\n");
    }
}

void save_data_convert_file_hex(char* file_hex, uint8_t **array2D_Save_Data) {
    char *file_hex_msk = malloc(strlen(file_hex) + 1);  // Cấp phát bộ nhớ
    strcpy(file_hex_msk, file_hex);
    char* token = strtok(file_hex_msk, "\n");
    int row = 0;
    while (token != NULL) {
        convert_string_to_byte(token, array2D_Save_Data[row]);
        if (check_sum(array2D_Save_Data[row], array2D_Save_Data[row][0] + 5) != STATUS_OK) {
            printf("Checksum error\n");
            break;
        }
        if (array2D_Save_Data[row][3] == 0x00) {
            swap_4_byte(&array2D_Save_Data[row][4], array2D_Save_Data[row][0]);
        }
        row++;
        token = strtok(NULL, "\n");
    }
    free(file_hex_msk);  // Giải phóng bộ nhớ sau khi sử dụng
}

void getData_length_one_line_command_length_command(char* file_hex, uint8_t *length_one_line_command, uint8_t *length_command) {
    char *file_hex_msk = malloc(strlen(file_hex) + 1);  // Cấp phát bộ nhớ
    strcpy(file_hex_msk, file_hex);
    char* token = strtok(file_hex_msk, "\n");
    int i = 0;
    while (token != NULL) {
        uint8_t index = 0;
        if (*token == ':') {
            token++;
            while (*token != '\0') {
                token += 2; // Di chuyển qua hai ký tự (tương ứng với một byte hex)
                index++;
            }
            length_one_line_command[i] = index; // Lưu chiều dài của từng dòng
            i++;
        }
        token = strtok(NULL, "\n");
    }
    *length_command = i; // Tổng số lệnh
    free(file_hex_msk);  // Giải phóng bộ nhớ sau khi sử dụng
}
