#This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.16)

# This example uses extra components for the following -
# 1. Test framework related.
# 2. Managing TEE OTA updates
# 3. TEE Secure Storage
# 4. TEE Entity Attestation
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/tools/unit-test-app/components
                                 $ENV{IDF_PATH}/components/esp_tee/subproject/components/tee_ota_ops
                                 $ENV{IDF_PATH}/components/esp_tee/subproject/components/tee_sec_storage
                                 $ENV{IDF_PATH}/components/esp_tee/subproject/components/tee_attestation)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/components/test_sec_srv/test_tee_project.cmake)
include($ENV{IDF_PATH}/components/esp_tee/subproject/components/tee_attestation/esp_tee_att.cmake)

project(esp_tee_test)
