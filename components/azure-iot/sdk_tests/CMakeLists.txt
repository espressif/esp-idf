#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for the folder tests of C shared utility
set(SHARED_UTIL_REAL_TEST_FOLDER ${CMAKE_CURRENT_LIST_DIR}/real_test_files CACHE INTERNAL "this is what needs to be included when doing test sources" FORCE)
message(STATUS "including external unit test directory " ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED MACOSX)
    add_subdirectory(tlsio_openssl_compact_ut)
endif()
