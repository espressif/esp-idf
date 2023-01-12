/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "unity.h"
#include "unity_test_utils.h"

#include "hid_host.h"
#include "test_hid_basic.h"

// ----------------------- Private -------------------------
/**
 * @brief Test HID Uninstall USB driver with device inserted
 *
 * - Wait events: DEVICE_DISCONNECTED
 * - On DEVICE_DISCONNECTED proceed
 */
void test_hid_usb_uninstall(void)
{
    printf("HID device remain inserted, uninstall device ... ");
    test_hid_uninstall_hid_device(test_hid_device);
}

// ----------------------- Public --------------------------


// ------------------------- USB Test ------------------------------------------
static void test_setup_hid_usb_driver(void)
{
    test_hid_setup();
}

static void test_teardown_hid_usb_driver(void)
{
    test_hid_teardown();
    //Short delay to allow task to be cleaned up
    vTaskDelay(pdMS_TO_TICKS(10));
    test_hid_device = NULL;
}

TEST_CASE("(HID Host) USB uninstall (dev present)", "[hid_host]")
{
    test_setup_hid_usb_driver();

    test_hid_device = test_hid_wait_connection_and_install();
    test_hid_usb_uninstall();
    test_teardown_hid_usb_driver();
    vTaskDelay(20);
}
