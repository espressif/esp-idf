#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

project(azure_external_unit_tests)

# Set up the C SDK options to build as little as possible
option(skip_samples "set skip_samples to ON to skip building samples (default is OFF)[if possible, they are always build]" ON)
option(compileOption_C "passes a string to the command line of the C compiler" OFF)
option(compileOption_CXX "passes a string to the command line of the C++ compiler" OFF)
option(no_logging "disable logging" OFF)
option(run_e2e_tests "set run_e2e_tests to ON to run e2e tests (default is OFF)" OFF)
option(run_unittests "set run_unittests to ON to run unittests (default is OFF)" OFF)
option(use_cppunittest "set use_cppunittest to ON to build CppUnitTest tests on Windows (default is ON)" ON)

# include the sdk for its header files and libs
set(run_unittests OFF)
include("sdk/c-utility/configs/azure_iot_external_pal_unit_test_setup.cmake")

# Configure this external repo
# Currently this CMakeLists.txt file is only used for unit tests
# EXTERNAL_PAL_REPO_DIR contains this top-level CMakeLists.txt
set(EXTERNAL_PAL_REPO_DIR ${CMAKE_CURRENT_LIST_DIR})
include_directories(${SHARED_UTIL_PAL_INC_FOLDER})


set(run_unittests ON)
add_subdirectory(sdk_tests)
