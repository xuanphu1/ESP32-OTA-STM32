set(app_src processing_data_of_file_hex.c)
set(pre_req )
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pre_req})