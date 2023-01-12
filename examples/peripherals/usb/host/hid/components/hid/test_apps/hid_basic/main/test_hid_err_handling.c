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

#define TEST_HID_ERR_HANDLING_CASES         (3)

static void test_install_hid_driver_without_config(void);
static void test_install_hid_driver_with_wrong_config(void);
static void test_claim_interface_without_driver(void);

void (*test_hid_err_handling_case[TEST_HID_ERR_HANDLING_CASES])(void) = {
    test_install_hid_driver_without_config,
    test_install_hid_driver_with_wrong_config,
    test_claim_interface_without_driver
};
// ----------------------- Private -------------------------
/**
 * @brief USB HID Host event callback. Handle such event as device connection and removing
 *
 * @param[in] event  HID Host device event
 * @param[in] arg    Pointer to arguments, does not used
 *
 */
static void test_hid_host_event_callback_stub(const hid_host_event_t *event, void *arg)
{
    if (event->event == HID_DEVICE_CONNECTED) {
        // Device connected
    } else if (event->event == HID_DEVICE_DISCONNECTED) {
        // Device disconnected
    }
}

// Install HID driver without USB Host and without configuration
static void test_install_hid_driver_without_config(void)
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, hid_host_install(NULL));
}

// Install HID driver without USB Host and with configuration
static void test_install_hid_driver_with_wrong_config(void)
{
    const hid_host_driver_config_t hid_host_config_callback_null = {
        .create_background_task = true,
        .task_priority = 5,
        .stack_size = 4096,
        .core_id = 0,
        .callback = NULL, /* error expected */
        .callback_arg = NULL
    };

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, hid_host_install(&hid_host_config_callback_null));

    const hid_host_driver_config_t hid_host_config_stack_size_null = {
        .create_background_task = true,
        .task_priority = 5,
        .stack_size = 0, /* error expected */
        .core_id = 0,
        .callback = NULL,
        .callback_arg = NULL
    };

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, hid_host_install(&hid_host_config_stack_size_null));

    const hid_host_driver_config_t hid_host_config_task_priority_null = {
        .create_background_task = true,
        .task_priority = 0,/* error expected */
        .stack_size = 4096,
        .core_id = 0,
        .callback = NULL,
        .callback_arg = NULL
    };

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, hid_host_install(&hid_host_config_task_priority_null));

    const hid_host_driver_config_t hid_host_config_correct = {
        .create_background_task = true,
        .task_priority = 5,
        .stack_size = 4096,
        .core_id = 0,
        .callback = test_hid_host_event_callback_stub,
        .callback_arg = NULL
    };

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, hid_host_install(&hid_host_config_correct));
}

// Open device without installed driver
static void test_claim_interface_without_driver(void)
{
    hid_host_interface_handle_t keyboard_handle;


    hid_host_interface_config_t keyboard_iface_config_wrong_proto1 = {
        .proto = HID_PROTOCOL_NONE,
        .callback = NULL,
    };

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      hid_host_claim_interface(&keyboard_iface_config_wrong_proto1, &keyboard_handle));

    hid_host_interface_config_t keyboard_iface_config_wrong_proto2 = {
        .proto = HID_PROTOCOL_MAX,
        .callback = NULL,
    };

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      hid_host_claim_interface(&keyboard_iface_config_wrong_proto2, &keyboard_handle));

    /*
    hid_host_interface_config_t keyboard_iface_config = {
        .proto = HID_PROTOCOL_KEYBOARD,
        .callback = NULL,
    };

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE,
            hid_host_claim_interface(&keyboard_iface_config, &keyboard_handle));
    */
}

// ----------------------- Public --------------------------

/**
 * @brief HID
 *
 * There are multiple erroneous scenarios checked in this test:
 *
 * -# Install HID driver without USB Host
 * -# Open device without installed driver
 * -# Uninstall driver before installing it
 * -# Open non-existent device
 * -# Open the same device twice
 * -# Uninstall driver with open devices
 */
TEST_CASE("(HID Host) Error handling", "[auto][hid_host]")
{
    for (int i = 0; i < TEST_HID_ERR_HANDLING_CASES; i++) {
        (*test_hid_err_handling_case[i])();
    }
}
