# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

idf_component_register(SRCS "test_app_main.c" "test_sv32.c"
                       INCLUDE_DIRS "."
                       REQUIRES unity riscv freertos
                       WHOLE_ARCHIVE)
