set(app_src OTAHost_esp32.c)
set(pre_req esp_driver_uart esp_timer Min processing_HEX)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pre_req})