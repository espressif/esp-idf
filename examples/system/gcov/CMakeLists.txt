# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

file(TO_NATIVE_PATH "$ENV{IDF_PATH}/tools/cmake/project.cmake" _project_path)

include(${_project_path})
project(gcov_example)

file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/coverage_report" _coverage_path)

idf_create_coverage_report(${_coverage_path})
idf_clean_coverage_report(${_coverage_path})
