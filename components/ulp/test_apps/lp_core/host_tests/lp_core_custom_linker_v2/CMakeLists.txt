# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.22)

# v2-native parent: the ULP program is built as a full CMake v2 subproject
# (ulp_project_default), so the custom LINKER layout is exercised through the
# buildv2 linker-script path rather than the legacy CMake v1 one.
include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

project(lp_core_custom_linker_v2 C CXX ASM)

idf_project_default()
