#include "cli_command_table.h"
#include "cli_types.h"
#include "getData.h"
#include "setData.h"

cli_command_info_t command_infor_get_data={

    getData,
    "Function get data"

};

cli_command_info_t command_infor_set_data={

    setData,
    "Funtion set parameter data"

};

cli_command_entry_t command_entry_table[] = {

    {"getData",&command_infor_get_data},
    {"setData",&command_infor_set_data},
    {NULL,NULL}

};

