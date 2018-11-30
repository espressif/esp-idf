# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(mqtt_websocket_secure)

target_add_binary_data(mqtt_websocket_secure.elf "main/iot_eclipse_org.pem" TEXT)
