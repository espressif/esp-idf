# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS
    main/bt_app_av.c
    main/bt_app_core.c
    main/main.c
    )

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(a2dp_sink)
