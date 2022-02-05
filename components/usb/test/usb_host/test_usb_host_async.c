/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "test_usb_common.h"
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

TEST_CASE("Test USB Host async client (single client)", "[usb_host][ignore]")
{
    test_usb_init_phy();    //Initialize the internal USB PHY and USB Controller for testing
    //Install USB Host
    usb_host_config_t host_config = {
        .skip_phy_setup = true,     //test_usb_init_phy() will already have setup the internal USB PHY for us
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
    test_usb_deinit_phy();  //Deinitialize the internal USB PHY after testing
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
TEST_CASE("Test USB Host async client (multi client)", "[usb_host][ignore]")
{
    test_usb_init_phy();    //Initialize the internal USB PHY and USB Controller for testing
    //Install USB Host
    usb_host_config_t host_config = {
        .skip_phy_setup = true,     //test_usb_init_phy() will already have setup the internal USB PHY for us
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
    test_usb_deinit_phy();  //Deinitialize the internal USB PHY after testing
}

/*
Test USB Host Asynchronous API Usage

Requires: This test requires an MSC SCSI device to be attached (see the MSC mock class)

Purpose:
    - Test that incorrect usage of USB Host Asynchronous API will returns errors

Procedure:
    - Install USB Host Library
    - Register two clients and all event handler functions from the same loop
    - Wait for each client to detect device connection
    - Check that both clients can open the same device
    - Check that a client cannot open a non-existent device
    - Check that only one client can claim a particular interface
    - Check that a client cannot release an already released interface
    - Wait for device disconnection
    - Cleanup
*/

static uint8_t dev_addr = 0;

typedef enum {
    CLIENT_TEST_STAGE_NONE,
    CLIENT_TEST_STAGE_CONN,
    CLIENT_TEST_STAGE_DCONN,
} client_test_stage_t;

static void test_async_client_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    client_test_stage_t *stage = (client_test_stage_t *)arg;

    switch (event_msg->event) {
        case USB_HOST_CLIENT_EVENT_NEW_DEV:
            if (dev_addr == 0) {
                dev_addr = event_msg->new_dev.address;
            } else {
                TEST_ASSERT_EQUAL(dev_addr, event_msg->new_dev.address);
            }
            *stage = CLIENT_TEST_STAGE_CONN;
            break;
        case USB_HOST_CLIENT_EVENT_DEV_GONE:
            *stage = CLIENT_TEST_STAGE_DCONN;
            break;
        default:
            abort();
            break;
    }
}

TEST_CASE("Test USB Host async API", "[usb_host][ignore]")
{
    test_usb_init_phy();    //Initialize the internal USB PHY and USB Controller for testing

    //Install USB Host
    usb_host_config_t host_config = {
        .skip_phy_setup = true,     //test_usb_init_phy() will already have setup the internal USB PHY for us
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));
    printf("Installed\n");

    //Register two clients
    client_test_stage_t client0_stage = CLIENT_TEST_STAGE_NONE;
    client_test_stage_t client1_stage = CLIENT_TEST_STAGE_NONE;

    usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = 5,
        .async = {
            .client_event_callback = test_async_client_cb,
            .callback_arg = (void *)&client0_stage,
        },
    };
    usb_host_client_handle_t client0_hdl;
    usb_host_client_handle_t client1_hdl;
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_register(&client_config, &client0_hdl));
    client_config.async.callback_arg = (void *)&client1_stage;
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_register(&client_config, &client1_hdl));

    //Wait until the device connects and the clients receive the event
    while (!(client0_stage == CLIENT_TEST_STAGE_CONN && client1_stage == CLIENT_TEST_STAGE_CONN)) {
        usb_host_lib_handle_events(0, NULL);
        usb_host_client_handle_events(client0_hdl, 0);
        usb_host_client_handle_events(client1_hdl, 0);
        vTaskDelay(10);
    }

    //Check that both clients can open the device
    TEST_ASSERT_NOT_EQUAL(0, dev_addr);
    usb_device_handle_t client0_dev_hdl;
    usb_device_handle_t client1_dev_hdl;
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_open(client0_hdl, dev_addr, &client0_dev_hdl));
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_open(client1_hdl, dev_addr, &client1_dev_hdl));
    TEST_ASSERT_EQUAL(client0_dev_hdl, client1_dev_hdl);    //Check that its the same device
    //Check that a client cannot open a non-existent device
    TEST_ASSERT_NOT_EQUAL(ESP_OK, usb_host_device_open(client0_hdl, 0, &client0_dev_hdl));

    //Check that the device cannot be opened again by the same client
    usb_device_handle_t dummy_dev_hdl;
    TEST_ASSERT_NOT_EQUAL(ESP_OK, usb_host_device_open(client0_hdl, dev_addr, &dummy_dev_hdl));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, usb_host_device_open(client1_hdl, dev_addr, &dummy_dev_hdl));
    //Check that both clients cannot claim the same interface
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_interface_claim(client0_hdl, client0_dev_hdl, MOCK_MSC_SCSI_INTF_NUMBER, MOCK_MSC_SCSI_INTF_ALT_SETTING));
    TEST_ASSERT_NOT_EQUAL(ESP_OK, usb_host_interface_claim(client1_hdl, client1_dev_hdl, MOCK_MSC_SCSI_INTF_NUMBER, MOCK_MSC_SCSI_INTF_ALT_SETTING));
    //Check that client0 cannot claim the same interface multiple times
    TEST_ASSERT_NOT_EQUAL(ESP_OK, usb_host_interface_claim(client0_hdl, client0_dev_hdl, MOCK_MSC_SCSI_INTF_NUMBER, MOCK_MSC_SCSI_INTF_ALT_SETTING));

    //Check that client0 can release the interface
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_interface_release(client0_hdl, client0_dev_hdl, MOCK_MSC_SCSI_INTF_NUMBER));
    //Check that client0 cannot release interface it has not claimed
    TEST_ASSERT_NOT_EQUAL(ESP_OK, usb_host_interface_release(client0_hdl, client0_dev_hdl, MOCK_MSC_SCSI_INTF_NUMBER));

    //Wait until the device disconnects and the clients receive the event
    test_usb_set_phy_state(false, 0);
    while (!(client0_stage == CLIENT_TEST_STAGE_DCONN && client1_stage == CLIENT_TEST_STAGE_DCONN)) {
        usb_host_lib_handle_events(0, NULL);
        usb_host_client_handle_events(client0_hdl, 0);
        usb_host_client_handle_events(client1_hdl, 0);
        vTaskDelay(10);
    }
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_close(client0_hdl, client0_dev_hdl));
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_close(client1_hdl, client1_dev_hdl));

    //Deregister the clients
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_deregister(client0_hdl));
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_deregister(client1_hdl));

    while (1) {
        uint32_t event_flags;
        usb_host_lib_handle_events(0, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            break;
        }
        vTaskDelay(10);
    }

    //Cleanup
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_uninstall());
    test_usb_deinit_phy();
}
