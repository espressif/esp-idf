/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "mock_msc.h"
#include "dev_msc.h"
#include "msc_client.h"
#include "ctrl_client.h"
#include "usb/usb_host.h"
#include "unity.h"

// --------------------------------------------------- Test Cases ------------------------------------------------------

/*
Test USB Host Library Sudden Disconnection Handling (no clients)
Purpose:
- Test that sudden disconnections are handled properly when there are no clients
- Test that devices can reconnect after a sudden disconnection has been handled by the USB Host Library

Procedure:
- Install USB Host Library
- Wait for connection (and enumeration) to occur
- Force a disconnection, then wait for disconnection to be handled (USB_HOST_LIB_EVENT_FLAGS_ALL_FREE)
- Allow connections again, and repeat test for multiple iterations
*/

#define TEST_DCONN_NO_CLIENT_ITERATIONS     3

TEST_CASE("Test USB Host sudden disconnection (no client)", "[usb_host][low_speed][full_speed][high_speed]")
{
    bool connected = false;
    int dconn_iter = 0;
    while (1) {
        // Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (!connected) {
            usb_host_lib_info_t lib_info;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_lib_info(&lib_info));
            if (lib_info.num_devices == 1) {
                // We've just connected. Trigger a disconnect
                connected = true;
                printf("Forcing Sudden Disconnect\n");
                // Trigger a disconnect by powering OFF the root port
                usb_host_lib_set_root_port_power(false);
            }
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            // The device has disconnected and it's disconnection has been handled
            printf("Dconn iter %d done\n", dconn_iter);
            if (++dconn_iter < TEST_DCONN_NO_CLIENT_ITERATIONS) {
                // Start next iteration
                connected = false;
                // Allow connections again by powering ON the root port
                usb_host_lib_set_root_port_power(true);
            } else {
                break;
            }
        }
    }
}

/*
Test USB Host Library Sudden Disconnection Handling (with client)
Purpose:
- Test that sudden disconnections are handled properly when there are registered clients
- Test that devices can reconnect after a sudden disconnection has been handled by the USB Host Library

Procedure:
    - Install USB Host Library
    - Create a task to run an MSC client
    - Start the MSC disconnect client task. It will open the device then force a disconnect for multiple iterations
    - Wait for USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS and USB_HOST_LIB_EVENT_FLAGS_ALL_FREE before uninstalling
*/

#define TEST_FORCE_DCONN_NUM_TRANSFERS      3
#define TEST_MSC_SCSI_TAG                   0xDEADBEEF

TEST_CASE("Test USB Host sudden disconnection (single client)", "[usb_host][full_speed][high_speed]")
{
    // Create task to run client that communicates with MSC SCSI interface
    const dev_msc_info_t *dev_info = dev_msc_get_info();
    msc_client_test_param_t params = {
        .num_sectors_to_read = 1,   // Unused by disconnect MSC client
        .num_sectors_per_xfer = TEST_FORCE_DCONN_NUM_TRANSFERS * dev_info->scsi_sector_size,
        .msc_scsi_xfer_tag = TEST_MSC_SCSI_TAG,
    };
    TaskHandle_t task_hdl;
    xTaskCreatePinnedToCore(msc_client_async_dconn_task, "async", 4096, (void *)&params, 2, &task_hdl, 0);
    // Start the task
    xTaskNotifyGive(task_hdl);

    bool all_clients_gone = false;
    bool all_dev_free = false;
    while (!all_clients_gone || !all_dev_free) {
        // Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            printf("No more clients\n");
            all_clients_gone = true;
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            printf("All device's freed\n");
            all_dev_free = true;
        }
    }
}

/*
Test USB Host Library Enumeration
Purpose:
- Test that the USB Host Library enumerates device correctly

Procedure:
- Install USB Host Library
- Create a task to run an MSC client
- Start the MSC enumeration client task. It will:
    - Wait for device connection
    - Open the device
    - Check details of the device's enumeration
    - Disconnect the device, and repeat the steps above for multiple iterations.
- Wait for USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS and USB_HOST_LIB_EVENT_FLAGS_ALL_FREE before uninstalling
*/

#define TEST_ENUM_ITERATIONS    3

TEST_CASE("Test USB Host enumeration", "[usb_host][full_speed][high_speed]")
{
    // Create task to run client that checks the enumeration of the device
    TaskHandle_t task_hdl;
    xTaskCreatePinnedToCore(msc_client_async_enum_task, "async", 6144, NULL, 2, &task_hdl, 0);
    // Start the task
    xTaskNotifyGive(task_hdl);

    bool all_clients_gone = false;
    bool all_dev_free = false;
    while (!all_clients_gone || !all_dev_free) {
        // Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            printf("No more clients\n");
            TEST_ASSERT_EQUAL(ESP_ERR_NOT_FINISHED, usb_host_device_free_all());
            all_clients_gone = true;
        }
        if (all_clients_gone && event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            all_dev_free = true;
        }
    }
}
