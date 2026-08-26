# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on esp_psram
# as we set CONFIG_SPIRAM_... options.
set(COMPONENTS main esp_psram)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mspi_test)

idf_build_get_property(elf EXECUTABLE)
if(CONFIG_COMPILER_DUMP_RTL_FILES)
    # Collect RTL directories in a variable for readability. Join them
    # with commas so they are passed as a single --rtl-dirs argument to the script.
    set(MSPI_RTL_DIRS
        # ${CMAKE_BINARY_DIR}/esp-idf/spi_flash # IDF-14271
        ${CMAKE_BINARY_DIR}/esp-idf/hal
        ${CMAKE_BINARY_DIR}/esp-idf/esp_hal_mspi
    )
    string(JOIN "," MSPI_RTL_DIRS_JOINED ${MSPI_RTL_DIRS})
    add_custom_target(check_test_app_sections ALL
                      COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                      --rtl-dirs ${MSPI_RTL_DIRS_JOINED}
                      --elf-file ${CMAKE_BINARY_DIR}/mspi_test.elf
                      find-refs
                      --from-sections=.iram0.text
                      --to-sections=.flash.text,.flash.rodata
                      --exit-code
                      DEPENDS ${elf}
                      )
endif()
