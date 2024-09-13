# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v5.3/components/bootloader/subproject"
  "D:/ESP32_OTA_STM32/ESP32-OTA-STM32/esp_OTA_SMT32/build/bootloader"
  "D:/ESP32_OTA_STM32/ESP32-OTA-STM32/esp_OTA_SMT32/build/bootloader-prefix"
  "D:/ESP32_OTA_STM32/ESP32-OTA-STM32/esp_OTA_SMT32/build/bootloader-prefix/tmp"
  "D:/ESP32_OTA_STM32/ESP32-OTA-STM32/esp_OTA_SMT32/build/bootloader-prefix/src/bootloader-stamp"
  "D:/ESP32_OTA_STM32/ESP32-OTA-STM32/esp_OTA_SMT32/build/bootloader-prefix/src"
  "D:/ESP32_OTA_STM32/ESP32-OTA-STM32/esp_OTA_SMT32/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/ESP32_OTA_STM32/ESP32-OTA-STM32/esp_OTA_SMT32/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/ESP32_OTA_STM32/ESP32-OTA-STM32/esp_OTA_SMT32/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
