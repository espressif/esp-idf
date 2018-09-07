# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS
    main/spi_master_example_main.c
    main/pretty_effect.c
    main/decode_image.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(spi_master)

# Embed the image into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
target_add_binary_data(spi_master.elf
    "main/image.jpg" BINARY)
