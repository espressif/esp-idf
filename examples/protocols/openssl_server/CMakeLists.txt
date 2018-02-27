# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS main/openssl_server_example_main.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(openssl_server)

# Embed the certificate & key data directly in the built binary
target_add_binary_data(openssl_server.elf main/cacert.pem TEXT)
target_add_binary_data(openssl_server.elf main/prvtkey.pem TEXT)
