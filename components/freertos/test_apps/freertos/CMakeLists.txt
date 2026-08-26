# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Set extra component directories for
#   - test_utils component
#   - the different test types (e.g., kernel, port, performance, ...) that are organized as components
set(EXTRA_COMPONENT_DIRS
    "$ENV{IDF_PATH}/tools/test_apps/components"
    "./kernel"
    "./misc"
    "./performance"
    "./port"
)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on. We also depend on esp_psram
# as we enable CONFIG_SPIRAM_... options.
set(COMPONENTS main esp_psram)

project(freertos_test)

# ============================================================================
# Code Coverage Instrumentation
# ============================================================================
# When CONFIG_FREERTOS_ENABLE_COVERAGE_TESTS is enabled, instrument FreeRTOS
# kernel, port, and ESP additions source files for coverage analysis.

if(CONFIG_FREERTOS_ENABLE_COVERAGE_TESTS)
    # Get the FreeRTOS component library and directory
    idf_component_get_property(freertos_lib freertos COMPONENT_LIB)
    idf_component_get_property(freertos_dir freertos COMPONENT_DIR)

    # Add coverage flags to the entire FreeRTOS component
    target_compile_options(${freertos_lib} PRIVATE "--coverage")
    target_link_options(${freertos_lib} PRIVATE "--coverage")

    # Create coverage report generation targets
    file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/coverage_report" _coverage_report_path)

    idf_create_coverage_report(
        ${_coverage_report_path}
        SOURCE_DIR ${freertos_dir}
        GCOV_OPTIONS "--gcov-ignore-parse-errors=negative_hits.warn"
    )
    idf_clean_coverage_report(${_coverage_report_path})

    message(STATUS "================================================")
    message(STATUS "FreeRTOS code coverage instrumentation enabled for test app")
    message(STATUS "Coverage report will be generated in: ${_coverage_report_path}")
    message(STATUS "Coverage root directory: ${freertos_dir}")
    message(STATUS "Use 'idf.py gcovr-report' to generate coverage report after running tests")
    message(STATUS "Use 'idf.py cov-data-clean' to clean coverage data")
endif()
