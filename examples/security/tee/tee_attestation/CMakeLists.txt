# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# (Not part of the boilerplate)
# This example uses extra components for the following -
# 1. Printing TEE attestation info
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/components/esp_tee/subproject/components/tee_attestation)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Including the attestation service calls
include($ENV{IDF_PATH}/components/esp_tee/subproject/components/tee_attestation/esp_tee_att.cmake)

project(tee_attestation)
