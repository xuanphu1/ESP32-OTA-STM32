#include "cli_command.h"


extern const cli_command_entry_t command_entry_table[] ;

const cli_command_entry_t* find_command_entry(char *cmd){

	const cli_command_entry_t* command_entry = command_entry_table;
	
	while (command_entry->command_infor != NULL){
		
		if (strcmp(command_entry->name,cmd)==0){
			return command_entry;
		}
		command_entry++;
	}
	return NULL ;
}
void cli_command_excute(char *uart_buff,uint8_t len){
    char *argv[10];
    char *pch;
    uint8_t argv_num = 0 ;
    pch = strtok(uart_buff," ");
    while (pch != NULL){
    
        argv[argv_num++] = pch;
        pch = strtok(NULL," ");
    }
    
    const cli_command_entry_t *command_entry = find_command_entry(argv[0]);
		if (command_entry != NULL){
			command_entry->command_infor->function(argv,argv_num);
		} else {
				responsePrint("File not found command\n");
		}
}


