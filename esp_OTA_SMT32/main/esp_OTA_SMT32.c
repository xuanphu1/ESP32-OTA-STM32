#include "string.h"
#include "../component/Min/min.h"
#include "../component/HostOTA_ESP32/OTAHost_esp32.h"
#include "esp_timer.h"

char file_hex[1024 * 20] =
R"(
:020000040800F2
:100000007006002089010008D70A0008610A00086C
:10001000D50A00088F020008410B0008000000000C
:10002000000000000000000000000000DB0A0008E3
:100030009102000800000000D90A0008DD0A00084B
:10004000A3010008A3010008A3010008A301000800
:10005000A3010008A3010008A3010008A3010008F0
:10006000A3010008A3010008A3010008A3010008E0
)";



struct min_context min_ctx;


void init_uart() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART2_PORT_NUM, &uart_config);
    uart_set_pin(UART2_PORT_NUM, UART2_TX_PIN, UART2_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART2_PORT_NUM, BUF_UART_SIZE * 2, 0, 0, NULL, 0);


}


bool flag_test = true;


void app_main(void) {
    init_uart();  // Khởi tạo UART
    initMin(&min_ctx);
    
    while (1) {
        
        //ota_send_start_end(OTA_COMMAND_TYPE_END,7);
        //uint8_t test[] = {0x01,0x02};
        //min_send_frame(&min_ctx,0,test,2);
        //ota_send_response(OTA_NOT_ACKNOWLEDGE);
        otaStart(file_hex,&flag_test);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
    
        }
    }