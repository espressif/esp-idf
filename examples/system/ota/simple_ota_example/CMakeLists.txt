# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS main/simple_ota_example.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(simple_ota)

# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
target_add_binary_data(simple_ota.elf
    "../server_certs/ca_cert.pem" TEXT)
