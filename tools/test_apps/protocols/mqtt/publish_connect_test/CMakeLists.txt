# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)

project(mqtt_publish_connect_test)

target_add_binary_data(mqtt_publish_connect_test.elf "main/mqtt_eclipseprojects_io.pem" TEXT)
target_add_binary_data(mqtt_publish_connect_test.elf "ca.crt" TEXT)
target_add_binary_data(mqtt_publish_connect_test.elf "ca.der" TEXT)
target_add_binary_data(mqtt_publish_connect_test.elf "client_pwd.key" TEXT)
target_add_binary_data(mqtt_publish_connect_test.elf "client_pwd.crt" TEXT)
target_add_binary_data(mqtt_publish_connect_test.elf "client_no_pwd.key" TEXT)
target_add_binary_data(mqtt_publish_connect_test.elf "client_inv.crt" TEXT)
