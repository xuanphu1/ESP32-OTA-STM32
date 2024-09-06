#include "OTAHost_esp32.h"



static bool flag_uart = false ;
static bool flag_response_of_STM32 = false ;

uint8_t **array_save_data_convert_file_hex;



struct min_context *minctx;

void min_tx_start(uint8_t port){

}
void min_tx_finished(uint8_t port){

}
uint16_t min_tx_space(uint8_t port){
	return 512;
}
void min_tx_byte(uint8_t port, uint8_t byte){
    uart_write_bytes(UART2_PORT_NUM, (const void*)&byte, 1);  // Truyền địa chỉ của byte
}

uint32_t min_time_ms(void){
	return esp_timer_get_time();
}


void min_application_handler(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload, uint8_t port){
    
    if (min_payload[0] == OTA_COMMAND_TYPE_RESPONSE && min_payload[1] == 0x01) // 0x01 is length of command response
    {
        switch (min_payload[2])
        {
        case OTA_ACKNOWLEDGE:
            printf("STM32 receives the command successfully\n");
            flag_response_of_STM32 = true ;
            break;
        
        case OTA_NOT_ACKNOWLEDGE:
            printf("STM32 receives the command failed\n");
            flag_response_of_STM32 = false ;
            break;

        default:
            printf("Command response wrong\n");
            flag_response_of_STM32 = false ;
            break;
        }
    } else {
        printf("Command ID wrong\n");
        flag_response_of_STM32 = false ;
    }
    flag_uart = false;  // Đặt lại cờ để ngăn việc gọi lại min_poll
    uart_flush_input(UART2_PORT_NUM);  // Xóa bộ đệm UART để chuẩn bị cho dữ liệu mới
}


void initMin(struct min_context *_minctx){
    minctx  = _minctx;
    min_init_context(minctx,MIN_PORT);
}

static void booLoader_send_data(void* data,uint8_t len){
    min_send_frame(minctx,MIN_ID,(uint8_t*)data,len);

}

void ota_send_data(OTA_Data_Type data_type, uint8_t *_data_array, uint8_t length) {
    // Tạo một cấu trúc OTA_Command với kích thước phù hợp
    OTA_Command *cmd = (OTA_Command *)malloc(sizeof(OTA_Command) + length - 1); // -1 vì chúng ta đã có 1 byte cho operation_code

    cmd->command_id = data_type;
    cmd->length = length;

    // Sao chép dữ liệu vào trong cấu trúc
    memcpy(cmd->data_array, _data_array, length);

    // Gửi dữ liệu
    booLoader_send_data(cmd, sizeof(cmd->command_id) + sizeof(cmd->length) + length);

    // Giải phóng bộ nhớ đã cấp phát
    free(cmd);
    
}

void ota_send_start_end(OTA_Data_Type start_or_end_command_type){
    OTA_Command ota_open_command;
    ota_open_command.command_id = start_or_end_command_type;
    ota_open_command.length = 1;
    memcpy(ota_open_command.data_array,(uint8_t*)&start_or_end_command_type,1);
    booLoader_send_data(&ota_open_command,sizeof(ota_open_command));


}

void ota_send_response(OTA_Response_Type response_type){
    OTA_Response response;
    response.command_id = OTA_COMMAND_TYPE_RESPONSE;
    response.length = 1;
    response.response_code = response_type;
    booLoader_send_data(&response,sizeof(response));
}

void response_command_of_STM32_handle(){
    uint8_t data[BUF_UART_SIZE];
    int len = uart_read_bytes(UART2_PORT_NUM, data, BUF_UART_SIZE, 100 / portTICK_PERIOD_MS);
    if (len > 0 && !flag_uart) {
        // Đánh dấu rằng đã nhận được dữ liệu và chỉ xử lý một lần
        flag_uart = true;
        min_poll(minctx, data, len);  // Gọi min_poll với độ dài thực tế của dữ liệu nhận được
    }

}


void otaStart(char *file_hex,bool *status_kick_off){

    if (*status_kick_off)
    {   
        
        storeDataOfHex data_of_hex;
        uint8_t length_one_line_command[21];
        uint8_t length_command;
      

        getData_length_one_line_command_length_command(file_hex,length_one_line_command,&length_command);
        // Cấp phát bộ nhớ động cho array_save_data_convert_file_hex
        array_save_data_convert_file_hex = (uint8_t **)malloc(length_command * sizeof(uint8_t *));
        for (int i = 0; i < length_command; i++) {
            array_save_data_convert_file_hex[i] = (uint8_t *)malloc(length_one_line_command[i] * sizeof(uint8_t));
        }
 
        

        save_data_convert_file_hex(file_hex, array_save_data_convert_file_hex);
        for (int j = 0 ; j < length_command ; j++){
            for (int i = 0; i < length_one_line_command[j]; i++) {
                printf("%02X ", array_save_data_convert_file_hex[j][i]);  // In từng byte dưới dạng hex
            }
            printf("\n");

        }
        
        printf("length command of file hex: %d\n",length_command);
        
        
        for (int row = 0 ; row < length_command ; row ++){
            printf("\n");
            ESP_LOGI("ROW DATA FRAME","ROW %d\n",row);

            printf("length line in row %d: %d\n",row,length_one_line_command[row]);
            

            //Cấp phát động bộ nhớ theo dữ liệu từng hàng để tránh việc mảng data bị thừa dữ liệu ==> tránh việc sai dữ liệu
            data_of_hex.Data_hex = (uint8_t*)malloc((length_one_line_command[row]-LENGTH_HEADER_COMMAND)*sizeof(uint8_t));
            printf("HEADER Frame: ");
            copy_array(array_save_data_convert_file_hex[row],data_of_hex.Header_hex,LENGTH_HEADER_COMMAND,POSITION_HEADER,length_one_line_command[row]); // 4 byte ==> 1byte length + 2byte address + 1byte record type
            printf("DATA Frame: ");
            copy_array(array_save_data_convert_file_hex[row],data_of_hex.Data_hex,length_one_line_command[row] - LENGTH_HEADER_COMMAND,POSITION_DATA,length_one_line_command[row]); // Position = 4 ==> Position Data after position Header 4 byte
            
            for (OTA_Data_Type Command = OTA_COMMAND_TYPE_START ; Command < sizeof(OTA_Data_Type); Command++)
            {
                switch (Command)
                {
                case OTA_COMMAND_TYPE_START: // Command Start
                    ota_send_start_end(OTA_COMMAND_TYPE_START);
                    ESP_LOGI("OTA_HOST","ESP32 send the command to START successfully");
                    while (!flag_response_of_STM32)
                    {
                        response_command_of_STM32_handle();
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }
                    flag_response_of_STM32 = false ;
                break;
                
                case OTA_COMMAND_TYPE_HEADER: // Command HEADER
                    ota_send_data(OTA_COMMAND_TYPE_HEADER,data_of_hex.Header_hex,LENGTH_HEADER_COMMAND);
                    printf("Data frame HEADER: ");
                    for (int i = 0; i < LENGTH_HEADER_COMMAND; i++) {
                        printf("%02X ", data_of_hex.Header_hex[i]);  // In từng byte dưới dạng hex
                    }
                    printf("\n");
                    ESP_LOGI("OTA_HOST","ESP32 send the command HEADER successfully");
                    while (!flag_response_of_STM32)
                    {
                        response_command_of_STM32_handle();
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }
                    flag_response_of_STM32 = false ; 
                break;

                case OTA_COMMAND_TYPE_DATA: // Command DATA

                    ota_send_data(OTA_COMMAND_TYPE_DATA,data_of_hex.Data_hex,length_one_line_command[row]-LENGTH_HEADER_COMMAND);
                    printf("Data frame DATA: ");
                    for (int i = 0; i < length_one_line_command[row] - LENGTH_HEADER_COMMAND; i++) {
                        printf("%02X ", data_of_hex.Data_hex[i]);  // In từng byte dưới dạng hex
                    }
                    printf("\n");
                    ESP_LOGI("OTA_HOST","ESP32 send the command DATA successfully");
                    while (!flag_response_of_STM32)
                    {
                        response_command_of_STM32_handle();
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }
                    flag_response_of_STM32 = false ; 
                break;

                case OTA_COMMAND_TYPE_END: // Command End
                    ota_send_start_end(OTA_COMMAND_TYPE_END);
                    ESP_LOGI("OTA_HOST","ESP32 send the command to END successfully");
                    while (!flag_response_of_STM32)
                    {
                        response_command_of_STM32_handle();
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }
                    flag_response_of_STM32 = false ; 
                break;

                default:
                    ESP_LOGE("OTA_HOST","ESP32 SEND WRONG STRUCTURE COMMAND");
                    break;
                }
            }
            //Giải phóng bộ nhớ đã cấp phát cho mảng lưu dữ liệu về frame data
            free(data_of_hex.Data_hex);
        }
        for (int i = 0; i < length_command; i++) {
                free(array_save_data_convert_file_hex[i]);
            }
        free(array_save_data_convert_file_hex);
        *status_kick_off = false ;
    }
}