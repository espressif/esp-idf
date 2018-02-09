cmake_minimum_required(VERSION 3.5)
set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/toolchain.cmake)

if(NOT SDKCONFIG)
  message(FATAL_ERROR "Bootloader subproject expects the SDKCONFIG variable to be passed in by the parent build process.")
endif()

project(bootloader ASM C CXX)

set(COMPONENTS bootloader esptool_py esp32 soc bootloader_support log spi_flash micro-ecc soc)
set(BOOTLOADER_BUILD 1)
set(IS_BOOTLOADER_BUILD 1)  # deprecated, use BOOTLOADER_BUILD
add_definitions(-DBOOTLOADER_BUILD=1)

set(MAIN_SRCS main/bootloader_start.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

target_link_libraries(bootloader.elf "-L ${CMAKE_CURRENT_SOURCE_DIR}/main")
target_link_libraries(bootloader.elf "-T esp32.bootloader.ld")
target_link_libraries(bootloader.elf "-T esp32.bootloader.rom.ld")
target_link_libraries(bootloader.elf "${BOOTLOADER_LINKER_ARGS}")
target_link_libraries(bootloader.elf gcc)

