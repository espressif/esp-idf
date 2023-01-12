/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hid_host.h"

#ifdef __cplusplus
extern "C" {
#endif

extern hid_host_device_handle_t test_hid_device;

// ------------------------ HID Test -------------------------------------------

void test_hid_setup(void);

hid_host_device_handle_t test_hid_wait_connection_and_install(void);

void test_hid_wait_for_removal(void);

void test_hid_uninstall_hid_device(hid_host_device_handle_t hid_device_handle);

void test_hid_teardown(void);

#ifdef __cplusplus
}
#endif //__cplusplus
