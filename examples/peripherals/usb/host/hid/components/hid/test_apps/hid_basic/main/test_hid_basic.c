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
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "usb/usb_host.h"

#include "hid_host.h"
#include "hid_usage_keyboard.h"
#include "hid_usage_mouse.h"

#include "test_hid_basic.h"

typedef enum {
    HOST_NO_CLIENT = 0x1,
    HOST_ALL_FREE = 0x2,
    DEVICE_CONNECTED = 0x4,
    DEVICE_DISCONNECTED = 0x8,
    DEVICE_ADDRESS_MASK = 0xFF0,
} test_app_event_t;

static EventGroupHandle_t test_usb_flags;
hid_host_device_handle_t test_hid_device = NULL;
TaskHandle_t test_usb_event_task_handle;
SemaphoreHandle_t test_semaphore;

// ----------------------- Private -------------------------
/**
 * @brief USB HID Host event callback. Handle such event as device connection and removing
 *
 * @param[in] event  HID Host device event
 * @param[in] arg    Pointer to arguments, does not used
 *
 */
static void test_hid_host_event_callback(const hid_host_event_t *event, void *arg)
{
    if (event->event == HID_DEVICE_CONNECTED) {
        // Obtained USB device address is placed after application events
        xEventGroupSetBits(test_usb_flags, DEVICE_CONNECTED | (event->device.address << 4));
    } else if (event->event == HID_DEVICE_DISCONNECTED) {
        xEventGroupSetBits(test_usb_flags, DEVICE_DISCONNECTED);
    }
}

/**
 * @brief USB HID Host interface callback
 *
 * @param[in] event  HID Host interface event
 * @param[in] arg    Pointer to arguments, does not used
 */
static void test_hid_host_interface_event_callback(const hid_host_interface_event_t *event, void *arg)
{
    switch (event->event) {
    case HID_DEVICE_INTERFACE_INIT:
        printf("Found Interface number %d, protocol %s\n",
               event->interface.num,
               (event->interface.proto == HID_PROTOCOL_KEYBOARD)
               ? "Keyboard"
               : "Mouse");
        break;
    case HID_DEVICE_INTERFACE_TRANSFER_ERROR:
    case HID_DEVICE_INTERFACE_CLAIM:
    case HID_DEVICE_INTERFACE_RELEASE:
    default:
        // ... do nothing here for now
        break;
    }
}

/**
 * @brief Handle common USB host library events
 *
 * @param[in] args  Pointer to arguments, does not used
 */
static void test_handle_usb_events(void *args)
{
    while (1) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);

        // Release devices once all clients has deregistered
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            usb_host_device_free_all();
            xEventGroupSetBits(test_usb_flags, HOST_NO_CLIENT);
        }
        // Give ready_to_uninstall_usb semaphore to indicate that USB Host library
        // can be deinitialized, and terminate this task.
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            xEventGroupSetBits(test_usb_flags, HOST_ALL_FREE);
        }
    }

    vTaskDelete(NULL);
}

/**
 * @brief Create global semaphore
 */
static void test_create_semaphore(void)
{
    test_semaphore = xSemaphoreCreateBinary();
}

/**
 * @brief Deletes global semaphore
 */
static void test_delete_semaphore(void)
{
    vSemaphoreDelete(test_semaphore);
}

/**
 * @brief HID Keyboard report callback
 *
 * Keyboard report length == size of keyboard boot report
 *
 */
static void test_hid_host_keyboard_report_callback(const uint8_t *const data, const int length)
{
    printf("Keyboard report length %d\n", length);
    TEST_ASSERT_EQUAL(sizeof(hid_keyboard_input_report_boot_t), length);
    xSemaphoreGive(test_semaphore);
}

/**
 * @brief HID Mouse report callback
 *
 * Mouse report length >= size of mouse boot report
 *
 */
static void test_hid_host_mouse_report_callback(const uint8_t *const data, const int length)
{
    printf("Mouse report length %d\n", length);
    TEST_ASSERT_GREATER_OR_EQUAL(sizeof(hid_mouse_input_report_boot_t), length);
    xSemaphoreGive(test_semaphore);
}

/**
 * @brief Waits global semaphore for timeout
 */
static bool test_wait_semaphore_timeout(unsigned int ms)
{
    return ( xSemaphoreTake(test_semaphore, pdMS_TO_TICKS(ms)) )
           ? true
           : false;
}

/**
 * @brief HID Keyboard report length test
 *
 * - Claim Keyboard interface
 * - Wait Keyboard interface report callback for 5 sec
 * - Release Keyboard interface
 */
static void test_hid_keyboard_report_length(void)
{
    hid_host_interface_handle_t keyboard_handle;
    hid_host_interface_config_t keyboard_iface_config = {
        .proto = HID_PROTOCOL_KEYBOARD,
        .callback = test_hid_host_keyboard_report_callback,
    };

    // claim keyboard interface
    TEST_ASSERT_EQUAL(ESP_OK, hid_host_claim_interface(&keyboard_iface_config, &keyboard_handle) );
    // wait report
    printf("Please, press any keyboard key ...\n");
    fflush(stdout);
    test_wait_semaphore_timeout(5000);
    // release keyboard interface
    TEST_ASSERT_EQUAL(ESP_OK, hid_host_release_interface(keyboard_handle) );
}

/**
 * @brief HID Mouse report length test
 *
 * - Claim Mouse interface
 * - Wait Mouse interface report callback for 5 sec
 * - Release Mouse interface
 */
static void test_hid_mouse_report_length(void)
{
    hid_host_interface_handle_t mouse_handle;
    hid_host_interface_config_t mouse_iface_config = {
        .proto = HID_PROTOCOL_MOUSE,
        .callback = test_hid_host_mouse_report_callback,
    };
    // claim mouse interface
    TEST_ASSERT_EQUAL(ESP_OK, hid_host_claim_interface(&mouse_iface_config, &mouse_handle) );
    // wait report
    printf("Please, move mouse or press any button ...\n");
    fflush(stdout);
    test_wait_semaphore_timeout(5000);
    // release mouse interface
    TEST_ASSERT_EQUAL(ESP_OK, hid_host_release_interface(mouse_handle) );
}

// ----------------------- Public -------------------------

/**
 * @brief Setups HID
 *
 * - Create events handle
 * - Create usb_events task
 * - Install USB Host driver
 * - Install HID Host driver
 */
void test_hid_setup(void)
{
    test_usb_flags = xEventGroupCreate();
    const usb_host_config_t host_config = { .intr_flags = ESP_INTR_FLAG_LEVEL1 };
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_install(&host_config) );

    xTaskCreate(test_handle_usb_events, "usb_events", 4096, NULL, 2, &test_usb_event_task_handle);

    // hid host driver config
    const hid_host_driver_config_t hid_host_config = {
        .create_background_task = true,
        .task_priority = 5,
        .stack_size = 4096,
        .callback = test_hid_host_event_callback,
    };

    TEST_ASSERT_EQUAL(ESP_OK, hid_host_install(&hid_host_config) );
}

/**
 * @brief Waits HID connection and install
 *
 * - Wait events: DEVICE_CONNECTED, DEVICE_ADDRESS_MASK
 * - On DEVICE_ADDRESS_MASK install HID Host device driver and return handle
 *
 * @return hid_host_device_handle_t HID Device handle
 */
hid_host_device_handle_t test_hid_wait_connection_and_install(void)
{
    TEST_ASSERT_NULL(test_hid_device);

    printf("Please, insert HID device ...\n");
    fflush(stdout);

    EventBits_t event = xEventGroupWaitBits(test_usb_flags,
                                            DEVICE_CONNECTED | DEVICE_ADDRESS_MASK,
                                            pdTRUE,
                                            pdFALSE,
                                            portMAX_DELAY);

    if (event & DEVICE_CONNECTED) {
        xEventGroupClearBits(test_usb_flags, DEVICE_CONNECTED);
    }

    if (event & DEVICE_ADDRESS_MASK) {
        xEventGroupClearBits(test_usb_flags, DEVICE_ADDRESS_MASK);

        const hid_host_device_config_t hid_host_device_config = {
            .dev_addr = (event & DEVICE_ADDRESS_MASK) >> 4,
                    .iface_event_cb = test_hid_host_interface_event_callback,
                    .iface_event_arg = NULL,
        };

        TEST_ASSERT_EQUAL(ESP_OK, hid_host_install_device(&hid_host_device_config, &test_hid_device) );
    }

    return test_hid_device;
}

/**
 * @brief Test HID Wait device removal
 *
 * - Wait events: DEVICE_DISCONNECTED
 * - On DEVICE_DISCONNECTED proceed
 */
void test_hid_wait_for_removal(void)
{
    printf("Please, remove HID device ...\n");
    fflush(stdout);

    EventBits_t event = xEventGroupWaitBits(test_usb_flags,
                                            DEVICE_DISCONNECTED,
                                            pdTRUE,
                                            pdFALSE,
                                            portMAX_DELAY);

    if (event & DEVICE_DISCONNECTED) {
        xEventGroupClearBits(test_usb_flags, DEVICE_DISCONNECTED);
    }
}

/**
 * @brief Uninstalls HID device by handle obtained from test_hid_wait_connection_and_install()
 *
 * - Wait events: DEVICE_DISCONNECTED
 * - On DEVICE_DISCONNECTED proceed
 */
void test_hid_uninstall_hid_device(hid_host_device_handle_t hid_device_handle)
{
    TEST_ASSERT_EQUAL(ESP_OK, hid_host_uninstall_device(test_hid_device) );
}

/**
 * @brief Teardowns HID
 *
 * - Uninstall HID Host driver
 * - Uninstall USB Host driver
 * - Delete usb_events task
 * - Delete events handle
 */
void test_hid_teardown(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, hid_host_uninstall() );
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_uninstall() );

    vTaskDelete(test_usb_event_task_handle);
    vEventGroupDelete(test_usb_flags);
}

// ------------------------- HID Test ------------------------------------------

static void test_setup_hid_basic(void)
{
    test_hid_setup();
    test_create_semaphore();
}

static void test_teardown_hid_basic(void)
{
    test_delete_semaphore();
    test_hid_teardown();
    //Short delay to allow task to be cleaned up
    vTaskDelay(pdMS_TO_TICKS(10));
    test_hid_device = NULL;
}

TEST_CASE("(HID Host) Memory leakage basic", "[auto][hid_host]")
{
    test_setup_hid_basic();
    test_teardown_hid_basic();
    vTaskDelay(20);
}

TEST_CASE("(HID Host) Memory leakage with HID device", "[hid_host]")
{
    test_setup_hid_basic();
    test_hid_device = test_hid_wait_connection_and_install();
    test_hid_wait_for_removal();
    test_hid_uninstall_hid_device(test_hid_device);
    test_teardown_hid_basic();
    vTaskDelay(20);
}

TEST_CASE("(HID Host) HID Keyboard report length", "[hid_host]")
{
    test_setup_hid_basic();
    test_hid_device = test_hid_wait_connection_and_install();
    test_hid_keyboard_report_length();
    test_hid_wait_for_removal();
    test_hid_uninstall_hid_device(test_hid_device);
    test_teardown_hid_basic();
    vTaskDelay(20);
}

TEST_CASE("(HID Host) HID Mouse report length", "[hid_host]")
{
    test_setup_hid_basic();
    test_hid_device = test_hid_wait_connection_and_install();
    test_hid_mouse_report_length();
    test_hid_wait_for_removal();
    test_hid_uninstall_hid_device(test_hid_device);
    test_teardown_hid_basic();
    vTaskDelay(20);
}
