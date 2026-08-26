# This example demonstrates conditional component inclusion with Build System v2

cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

project(conditional_component C CXX ASM)

# Initialize the project
idf_project_init()

# Build the executable.
# main component provides the main function and conditionally
# includes logging_util and math_util. esptool_py is required
# for binary generation.
idf_build_executable(${CMAKE_PROJECT_NAME}.elf
                     COMPONENTS main esptool_py)

# Generate binary from executable
idf_build_binary(${CMAKE_PROJECT_NAME}.elf
                 OUTPUT_FILE "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin"
                 TARGET ${CMAKE_PROJECT_NAME}_binary)

# Create flash target
idf_flash_binary(${CMAKE_PROJECT_NAME}_binary
                 TARGET app-flash
                 NAME "app"
                 FLASH)

# Check binary size
idf_check_binary_size(${CMAKE_PROJECT_NAME}_binary)

# Create app target
add_custom_target(app ALL DEPENDS ${CMAKE_PROJECT_NAME}_binary)

# Generate flasher_args.json and metadata
idf_build_generate_flasher_args()
idf_build_generate_metadata(BINARY ${CMAKE_PROJECT_NAME}_binary)

# Create utility targets for configuration
idf_create_menuconfig(${CMAKE_PROJECT_NAME}.elf TARGET menuconfig)
idf_create_confserver(${CMAKE_PROJECT_NAME}.elf TARGET confserver)
