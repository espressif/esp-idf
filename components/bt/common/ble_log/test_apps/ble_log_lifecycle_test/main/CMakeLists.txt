# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
idf_component_register(SRCS "test_lifecycle.c" INCLUDE_DIRS "."
    PRIV_REQUIRES bt esp_timer nvs_flash)
idf_component_get_property(bt_dir bt COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE
    "${bt_dir}/common/ble_log/include"
    "${bt_dir}/common/ble_log/src/internal_include"
    "${bt_dir}/common/ble_log/src/internal_include/prph")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=ble_log_write_hex_ll")
if(LOG_STOP_NEGATIVE_CONTROL)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE LOG_STOP_NEGATIVE_CONTROL=1)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=ble_log_pause")
endif()

if(LOG_STOP_RECYCLE_CONTROL)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE LOG_STOP_RECYCLE_CONTROL=1)
endif()
