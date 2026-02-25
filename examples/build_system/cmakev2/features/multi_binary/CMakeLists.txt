cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

project(multi_binary C CXX ASM)

# Manual initialization for fine-grained control over component inclusion
idf_project_init()

# Build the first executable: app1
# Links: app1_main, component1, component2
idf_build_executable(app1.elf
                     COMPONENTS app1_main component1 component2)

# Build the second executable: app2
# Links: app2_main, component1, component2, component3
idf_build_executable(app2.elf
                     COMPONENTS app2_main component1 component2 component3)

# Generate binaries and flash targets

# App1 binary
idf_build_binary(app1.elf
                 OUTPUT_FILE "${CMAKE_BINARY_DIR}/app1.bin"
                 TARGET app1_binary)

idf_flash_binary(app1_binary
                 TARGET app1-flash
                 NAME "app1"
                 FLASH)

# Generate metadata only for the primary app1 binary
idf_build_generate_metadata(BINARY app1_binary)

# Create menuconfig and confserver targets for app1 binary
idf_create_menuconfig(app1.elf TARGET app1-menuconfig)
idf_create_confserver(app1.elf TARGET app1-confserver)

# App2 binary
idf_build_binary(app2.elf
                 OUTPUT_FILE "${CMAKE_BINARY_DIR}/app2.bin"
                 TARGET app2_binary)

idf_flash_binary(app2_binary
                 TARGET app2-flash
                 NAME "app2"
                 FLASH)

# Create menuconfig and confserver targets for app2 binary
idf_create_menuconfig(app2.elf TARGET app2-menuconfig)
idf_create_confserver(app2.elf TARGET app2-confserver)

# Make both binaries part of the default "app" target
add_custom_target(app ALL DEPENDS app1.bin app2.bin)
