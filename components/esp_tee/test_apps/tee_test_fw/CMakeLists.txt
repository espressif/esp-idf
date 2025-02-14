#This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# For registering the test-specific and attestation secure services
include(${CMAKE_CURRENT_LIST_DIR}/components/test_sec_srv/test_tee_project.cmake)

project(esp_tee_test)
