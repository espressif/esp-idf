# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mqtt_ssl)

target_add_binary_data(mqtt_ssl.elf "main/mqtt_eclipseprojects_io.pem" TEXT)
