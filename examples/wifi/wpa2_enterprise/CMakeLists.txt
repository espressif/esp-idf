
# (Automatically converted from project Makefile by convert_to_cmake.py.)

# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS main/wpa2_enterprise_main.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(wpa2-enterprise)

# Embed CA, certificate & key directly into binary
target_add_binary_data(wpa2-enterprise.elf main/wpa2_ca.pem TEXT)
target_add_binary_data(wpa2-enterprise.elf main/wpa2_client.crt TEXT)
target_add_binary_data(wpa2-enterprise.elf main/wpa2_client.key TEXT)

