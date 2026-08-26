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

# Generate flasher_args.json and metadata for the primary app1 binary.
# app1 is the only app registered in the global 'flash' target (app2 omits FLASH),
# so flasher_args.json correctly contains app1 as the sole app binary.
idf_build_generate_flasher_args()
idf_build_generate_metadata(BINARY app1_binary)

# Create menuconfig and confserver targets for app1 binary
idf_create_menuconfig(app1.elf TARGET app1-menuconfig)
idf_create_confserver(app1.elf TARGET app1-confserver)

# App2 binary
idf_build_binary(app2.elf
                 OUTPUT_FILE "${CMAKE_BINARY_DIR}/app2.bin"
                 TARGET app2_binary)

# Do NOT pass FLASH here: both apps share the same 0x10000 offset, which would
# create a duplicate key in the global 'flash' target and break flasher_args.json.
# app1 (with FLASH) is used as the default app in flasher_args.json; the pytest
# test swaps in app2.bin when needed.
idf_flash_binary(app2_binary
                 TARGET app2-flash
                 NAME "app2")

# Create menuconfig and confserver targets for app2 binary
idf_create_menuconfig(app2.elf TARGET app2-menuconfig)
idf_create_confserver(app2.elf TARGET app2-confserver)

# Make both binaries part of the default "app" target
add_custom_target(app ALL DEPENDS app1.bin app2.bin)
