cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
option(TEST_BUILD "" ON)
project(host_mqtt_client_test)
