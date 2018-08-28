# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS main/ble_hidd_demo_main.c
    main/hid_dev.c
    main/hid_device_le_prf.c
    main/esp_hidd_prf_api.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(hidd_demos)
