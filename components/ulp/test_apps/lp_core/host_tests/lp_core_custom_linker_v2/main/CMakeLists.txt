# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
idf_component_register(SRCS "test_main.c"
                       REQUIRES ulp)

# Pytest overrides CUSTOM_ULP_LAYOUT per test case via -DCUSTOM_ULP_LAYOUT=...
if(NOT DEFINED CUSTOM_ULP_LAYOUT)
    set(CUSTOM_ULP_LAYOUT "${CMAKE_CURRENT_LIST_DIR}/ulp/layouts/good.ld")
endif()

# Build the ULP program as a full CMake v2 subproject (see ulp/CMakeLists.txt),
# supplying the custom memory layout through the LINKER_LAYOUT option.
ulp_add_project("ulp_main" "${CMAKE_CURRENT_LIST_DIR}/ulp/"
                TYPE lp_core
                LINKER_LAYOUT "${CUSTOM_ULP_LAYOUT}")
