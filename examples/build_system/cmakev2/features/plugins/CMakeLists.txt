cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
project(plugins C CXX ASM)

# Use project-level initialization instead of idf_project_default
idf_project_init()

# Create executable with explicit component specification
# This approach includes plugin components in the executable
idf_build_executable(${CMAKE_PROJECT_NAME}.elf
                     COMPONENTS main plugins plugin_hello plugin_nihao)

# Generate binary
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

# Create other utility targets
idf_create_menuconfig(${CMAKE_PROJECT_NAME}.elf
                      TARGET menuconfig)
