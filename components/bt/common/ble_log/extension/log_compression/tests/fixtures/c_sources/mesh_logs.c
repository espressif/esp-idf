/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Test fixture: BLE Mesh module log patterns */

void mesh_error_func(int err) {
    BT_ERR("Mesh error %d", err);
}

void mesh_warn_func(int code) {
    BT_WARN("Mesh warning %d", code);
}

void mesh_info_func(void) {
    BT_INFO("Mesh info");
}

void mesh_debug_func(int val) {
    BT_DBG("Mesh debug %d", val);
}
