# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES bt
)

# The lifecycle check sizes its sealing write with BLE_LOG_MAX_PAYLOAD_LEN,
# so it needs the component's internal headers (same as ble_log_rt_test).
idf_component_get_property(bt_dir bt COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE
    "${bt_dir}/common/ble_log/src/internal_include"
    "${bt_dir}/common/ble_log/src/internal_include/prph"
)
