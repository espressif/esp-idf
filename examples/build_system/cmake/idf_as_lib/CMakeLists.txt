cmake_minimum_required(VERSION 3.5)

project(idf_as_lib C)

set(SUPPORTED_TARGETS esp32)
if("${TARGET}" STREQUAL "esp32")
    # Include for ESP-IDF build system functions
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Create idf::esp32 and idf::freertos static libraries
    idf_build_process(esp32
                    # try and trim the build; additional components
                    # will be included as needed based on dependency tree
                    #
                    # although esptool_py does not generate static library,
                    # processing the component is needed for flashing related
                    # targets and file generation
                    COMPONENTS esp32 freertos esptool_py
                    SDKCONFIG ${CMAKE_BINARY_DIR}/sdkconfig
                    BUILD_DIR ${CMAKE_BINARY_DIR})
else()
    # Create stubs for esp32 and freertos, stub::esp32 and stub::freertos
    add_subdirectory(stubs/esp32)
    add_subdirectory(stubs/freertos)
    add_subdirectory(stubs/spi_flash)
endif()

set(elf_file ${CMAKE_PROJECT_NAME}.elf)
add_executable(${elf_file} main.c)

# Link the static libraries to the executable
if("${TARGET}" STREQUAL "esp32")
    target_link_libraries(${elf_file} idf::esp32 idf::freertos idf::spi_flash)
    # Attach additional targets to the executable file for flashing,
    # linker script generation, partition_table generation, etc.
    idf_build_executable(${elf_file})
else()
    target_link_libraries(${elf_file} stub::esp32 stub::freertos stub::spi_flash)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)