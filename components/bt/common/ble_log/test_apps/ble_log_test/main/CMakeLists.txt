# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

idf_component_register(
    SRCS "test_ble_log_main.c" "test_ble_log_rt.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES unity bt esp_hw_support esp_timer
    WHOLE_ARCHIVE
)

idf_component_get_property(bt_dir bt COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE
    "${bt_dir}/common/ble_log/include"
    "${bt_dir}/common/ble_log/src/internal_include"
    "${bt_dir}/common/ble_log/src/internal_include/prph"
)
