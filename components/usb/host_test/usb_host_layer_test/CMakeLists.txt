cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)

list(APPEND EXTRA_COMPONENT_DIRS
     "$ENV{IDF_PATH}/tools/mocks/usb/usb_host_layer_mock/usb"

     # The following line would be needed to include the freertos mock component if this test used mocked FreeRTOS.
     #"$ENV{IDF_PATH}/tools/mocks/freertos/"
    )

project(host_test_usb_host_layer)
