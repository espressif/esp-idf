cmake_minimum_required(VERSION 3.16)

if((IDF_TARGET STREQUAL "esp32s2") OR (IDF_TARGET STREQUAL "esp32c3") OR (IDF_TARGET STREQUAL "esp32s3"))
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    set(COMPONENTS main)
    project(test_memprot)

    target_link_libraries(${project_elf} PRIVATE
            "-Wl,--wrap=esp_panic_handler"
            "-Wl,--wrap=esp_panic_handler_reconfigure_wdts"
            "-Wl,--wrap=esp_cpu_stall")
endif()
