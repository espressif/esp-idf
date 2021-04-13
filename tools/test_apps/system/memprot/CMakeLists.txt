cmake_minimum_required(VERSION 3.5)

if((IDF_TARGET STREQUAL "esp32s2") OR (IDF_TARGET STREQUAL "esp32c3"))
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(test_memprot)

    target_link_libraries(${project_elf} "-Wl,--wrap=esp_panic_handler")

endif()
