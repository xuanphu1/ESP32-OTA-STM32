#include "FlashSTM32.h"




void Flash_erase(uint32_t address,int number_page_erase){

	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.Banks = 1;
	EraseInitStruct.NbPages = number_page_erase;
	EraseInitStruct.PageAddress = address;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	uint32_t pageerr; 

	HAL_FLASHEx_Erase(&EraseInitStruct,&pageerr);
	HAL_FLASH_Lock();
	
}
void Flash_write_int(uint32_t address, int value){
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,address,value);
	HAL_FLASH_Lock();
}
void Flash_write_float (uint32_t address, float value){

	uint8_t data[4];
	*(float*)data = value ;
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,address,*(uint32_t*)data);
	HAL_FLASH_Lock();
	
}
void Flash_write_array (uint32_t address, uint8_t *arr, uint16_t len){

	responsePrint("Command: ");
	for(int i = 0;i < len ;i++){
		responsePrint("%02X ",arr[i]);
	}
	HAL_FLASH_Unlock();

	
	uint16_t *pt = (uint16_t*)arr ;
	for(uint16_t i = 0 ; i < (len+1)/2; i++){
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,address + 2*i,*pt);
	pt++;
	}
	HAL_FLASH_Lock();
	
}
void Flash_write_Struct(uint32_t address, wifi_info_t data){

	Flash_write_array(address,(uint8_t*)&data,sizeof(data));
	
}

int Flash_read_int(uint32_t address){
	return *(__IO uint16_t *)(address);
}
float Flash_read_float(uint32_t address){
	
	uint32_t data = *(__IO uint32_t*)address;
	return *(float*)(&data);
	
}
void Flash_read_array(uint32_t address, uint8_t *arr,uint16_t len){

	uint16_t *result = (uint16_t*)arr ;
	for(uint16_t i = 0 ; i < (len+1)/2; i++){
		
		*result = *(__IO uint16_t*)(address +2*i);
		result++;
	}
}
void Flash_read_struct(uint32_t address,wifi_info_t *data){

	Flash_read_array(address,(uint8_t*)data,sizeof(wifi_info_t));
	
	
}
