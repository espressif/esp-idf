cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
# Freertos is included via common components. However, CATCH isn't compatible with the FreeRTOS component yet, hence
# using the FreeRTOS mock component.
# target.
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/mocks/freertos/")

project(wear_levelling_host_test)
