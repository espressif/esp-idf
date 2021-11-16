/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "test_usb_mock_classes.h"
#include "msc_client.h"
#include "ctrl_client.h"
#include "usb/usb_host.h"
#include "unity.h"
#include "test_utils.h"

#define TEST_MSC_NUM_SECTORS_TOTAL          10
#define TEST_MSC_NUM_SECTORS_PER_XFER       2
#define TEST_MSC_SCSI_TAG                   0xDEADBEEF
#define TEST_CTRL_NUM_TRANSFERS             30

// --------------------------------------------------- Test Cases ------------------------------------------------------

/*
Test USB Host Asynchronous API single client

Requires: This test requires an MSC SCSI device to be attached (see the MSC mock class)

Purpose:
    - Test that USB Host Asynchronous API works correctly with a single client
    - Test that a client can be created
    - Test that client can operate concurrently in a separate thread
    - Test that the main thread is able to detect library events (such as USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS)

Procedure:
    - Install USB Host Library
    - Create a task to run an MSC client
    - Start the MSC client task. It will execute a bunch of MSC SCSI sector reads
    - Wait for the host library event handler to report a USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS event
    - Free all devices
    - Uninstall USB Host Library
*/

TEST_CASE("Test USB Host async (single client)", "[usb_host][ignore]")
{
    //Install USB Host
    usb_host_config_t host_config = {
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));
    printf("Installed\n");

    //Create task to run client that communicates with MSC SCSI interface
    msc_client_test_param_t params = {
        .num_sectors_to_read = TEST_MSC_NUM_SECTORS_TOTAL,
        .num_sectors_per_xfer = TEST_MSC_NUM_SECTORS_PER_XFER,
        .msc_scsi_xfer_tag = TEST_MSC_SCSI_TAG,
        .idVendor = MOCK_MSC_SCSI_DEV_ID_VENDOR,
        .idProduct = MOCK_MSC_SCSI_DEV_ID_PRODUCT,
    };
    TaskHandle_t task_hdl;
    xTaskCreatePinnedToCore(msc_client_async_seq_task, "async", 4096, (void *)&params, 2, &task_hdl, 0);
    //Start the task
    xTaskNotifyGive(task_hdl);

    while (1) {
        //Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            printf("No more clients\n");
            TEST_ASSERT_EQUAL(ESP_ERR_NOT_FINISHED, usb_host_device_free_all());
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            break;
        }
    }

    //Short delay to allow task to be cleaned up
    vTaskDelay(10);
    //Clean up USB Host
    ESP_ERROR_CHECK(usb_host_uninstall());
}

/*
Test USB Host Asynchronous API with multiple clients

Requires: This test requires an MSC SCSI device to be attached (see the MSC mock class)

Purpose:
    - Test the USB Host Asynchronous API works correctly with multiple clients
    - Test that multiple clients can be created
    - Test that multiple clients can operate concurrently in separate threads
    - Test that the main thread is able to detect library events (such as USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS)

Procedure:
    - Install USB Host Library
    - Create separate tasks to run an MSC client and Ctrl Client
        - MSC Client will execute a bunch of MSC SCSI sector reads
        - Ctrl Client will execute a bunch of control transfers
    - Wait for the host library event handler to report a USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS event
    - Free all devices
    - Uninstall USB Host Library
*/
TEST_CASE("Test USB Host async (multi client)", "[usb_host][ignore]")
{
    //Install USB Host
    usb_host_config_t host_config = {
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));
    printf("Installed\n");

    //Create task to run the MSC client
    msc_client_test_param_t msc_params = {
        .num_sectors_to_read = TEST_MSC_NUM_SECTORS_TOTAL,
        .num_sectors_per_xfer = TEST_MSC_NUM_SECTORS_PER_XFER,
        .msc_scsi_xfer_tag = TEST_MSC_SCSI_TAG,
        .idVendor = MOCK_MSC_SCSI_DEV_ID_VENDOR,
        .idProduct = MOCK_MSC_SCSI_DEV_ID_PRODUCT,
    };
    TaskHandle_t msc_task_hdl;
    xTaskCreatePinnedToCore(msc_client_async_seq_task, "msc", 4096, (void *)&msc_params, 2, &msc_task_hdl, 0);

    //Create task a control transfer client
    ctrl_client_test_param_t ctrl_params = {
        .num_ctrl_xfer_to_send = TEST_CTRL_NUM_TRANSFERS,
        .idVendor = MOCK_MSC_SCSI_DEV_ID_VENDOR,
        .idProduct = MOCK_MSC_SCSI_DEV_ID_PRODUCT,
    };
    TaskHandle_t ctrl_task_hdl;
    xTaskCreatePinnedToCore(ctrl_client_async_seq_task, "ctrl", 4096, (void *)&ctrl_params, 2, &ctrl_task_hdl, 0);

    //Start both tasks
    xTaskNotifyGive(msc_task_hdl);
    xTaskNotifyGive(ctrl_task_hdl);

    while (1) {
        //Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            printf("No more clients\n");
            TEST_ASSERT_EQUAL(ESP_ERR_NOT_FINISHED, usb_host_device_free_all());
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            break;
        }
    }

    //Short delay to allow task to be cleaned up
    vTaskDelay(10);
    //Clean up USB Host
    ESP_ERROR_CHECK(usb_host_uninstall());
}
