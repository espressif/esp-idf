# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
project(test_build)

add_custom_target(check_bootloader_sections ALL
                  COMMAND ${PYTHON} $ENV{IDF_PATH}/tools/ci/check_callgraph.py
                  --rtl-dirs ${CMAKE_BINARY_DIR}/bootloader
                  --elf-file ${CMAKE_BINARY_DIR}/bootloader/bootloader.elf
                  find-refs
                  --from-sections=.iram_loader.text
                  --to-sections=.iram.text
                  --exit-code
                  DEPENDS bootloader
                  )
