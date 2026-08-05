# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
idf_component_register(SRCS "test_main.c"
                       INCLUDE_DIRS "."
                       REQUIRES ulp)

# Pytest overrides CUSTOM_ULP_LAYOUT per test case via -DCUSTOM_ULP_LAYOUT=...
if(NOT DEFINED CUSTOM_ULP_LAYOUT)
    set(CUSTOM_ULP_LAYOUT "${CMAKE_CURRENT_LIST_DIR}/ulp/layouts/good.ld")
endif()

set(ulp_app_name ulp_${COMPONENT_NAME})
ulp_embed_binary(${ulp_app_name} "ulp/main.c" "test_main.c"
                 LINKER_LAYOUT "${CUSTOM_ULP_LAYOUT}")
