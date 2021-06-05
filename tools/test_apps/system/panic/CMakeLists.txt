# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(COMPONENTS esptool_py main)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(test_panic)

# Enable UBSAN checks
#
# shift-base sanitizer is disabled due to the following pattern found in register header files:
#   #define SOME_FIELD  0xFFFF
#   #define SOME_FIELD_M  ((SOME_FIELD_V)<<(SOME_FIELD_S))
#   #define SOME_FIELD_V  0xFFFF
#   #define SOME_FIELD_S  16
# here SOME_FIELD_V doesn't have an unsigned (U) prefix, so the compiler flags
# SOME_FIELD_M expansion (0xFFFF << 16) as generating integer overflow.
#
set(ubsan_options "-fsanitize=undefined" "-fno-sanitize=shift-base")

# Only enable UBSAN for a few components related to the panic test,
# due to RAM size limitations.
foreach(component main espcoredump esp_system spi_flash
                  esp_common esp_hw_support soc hal freertos)
    idf_component_get_property(lib ${component} COMPONENT_LIB)
    target_compile_options(${lib} PRIVATE ${ubsan_options})
endforeach()
