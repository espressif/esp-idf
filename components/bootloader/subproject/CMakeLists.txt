cmake_minimum_required(VERSION 3.5)

if(NOT SDKCONFIG)
    message(FATAL_ERROR "Bootloader subproject expects the SDKCONFIG variable to be passed "
        "in by the parent build process.")
endif()

if(NOT IDF_PATH)
    message(FATAL_ERROR "Bootloader subproject expects the IDF_PATH variable to be passed "
        "in by the parent build process.")
endif()

set(COMPONENTS bootloader esptool_py esp32 partition_table soc bootloader_support log spi_flash micro-ecc soc main)
set(BOOTLOADER_BUILD 1)
add_definitions(-DBOOTLOADER_BUILD=1)

set(COMPONENT_REQUIRES_COMMON log esp32 soc)

include("${IDF_PATH}/tools/cmake/project.cmake")
project(bootloader)

target_linker_script(bootloader.elf
    "main/esp32.bootloader.ld"
    "main/esp32.bootloader.rom.ld")
# Imported from esp32 component
target_linker_script(bootloader.elf ${ESP32_BOOTLOADER_LINKER_SCRIPTS})

target_link_libraries(bootloader.elf gcc)
