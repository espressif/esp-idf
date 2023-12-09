# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Set extra component directories for
#   - test_utils component
#   - the different test types (e.g., kernel, port, performance, ...) that are organized as components
set(EXTRA_COMPONENT_DIRS
    "$ENV{IDF_PATH}/tools/unit-test-app/components"
    "./kernel"
    "./misc"
    "./performance"
    "./port"
)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on esp_psram
# as we enable CONFIG_SPIRAM_... options.
set(COMPONENTS main esp_psram)

project(freertos_test)
