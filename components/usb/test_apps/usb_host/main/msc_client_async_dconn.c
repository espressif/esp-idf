/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "mock_msc.h"
#include "dev_msc.h"
#include "msc_client.h"
#include "usb/usb_types_ch9.h"
#include "usb/usb_host.h"
#include "unity.h"

/*
Implementation of an asynchronous MSC client used for USB Host disconnection test.

- The MSC client will:
    - Register itself as a client
    - Receive USB_HOST_CLIENT_EVENT_NEW_DEV event message, and open the device
    - Allocate IN and OUT transfer objects for MSC SCSI transfers
    - Trigger a single MSC SCSI transfer
        - Split the data stage into multiple transfers (so that the endpoint multiple queued up transfers)
        - Cause a disconnection mid-way through the data stage
    - All of the transfers should be automatically dequeued
    - Then a USB_HOST_CLIENT_EVENT_DEV_GONE event should occur afterwards
    - Free transfer objects
    - Close device
    - Deregister MSC client
*/

#define TEST_DCONN_ITERATIONS   3

typedef enum {
    TEST_STAGE_WAIT_CONN,
    TEST_STAGE_DEV_OPEN,
    TEST_STAGE_MSC_RESET,
    TEST_STAGE_MSC_CBW,
    TEST_STAGE_MSC_DATA_DCONN,
    TEST_STAGE_DEV_CLOSE,
} test_stage_t;

typedef struct {
    // Test parameters
    msc_client_test_param_t test_param;
    // MSC device info
    const dev_msc_info_t *dev_info;
    usb_speed_t dev_speed;
    uint8_t dev_addr;
    // Client variables
    usb_host_client_handle_t client_hdl;
    usb_device_handle_t dev_hdl;
    // Test state
    test_stage_t cur_stage;
    test_stage_t next_stage;
    int num_data_transfers;
    int event_count;
} msc_client_obj_t;

static void msc_reset_cbw_transfer_cb(usb_transfer_t *transfer)
{
    msc_client_obj_t *msc_obj = (msc_client_obj_t *)transfer->context;
    // We expect the reset and CBW transfers to complete with no issues
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, transfer->status, "Transfer NOT completed");
    TEST_ASSERT_EQUAL(transfer->num_bytes, transfer->actual_num_bytes);
    switch (msc_obj->cur_stage) {
    case TEST_STAGE_MSC_RESET:
        msc_obj->next_stage = TEST_STAGE_MSC_CBW;
        break;
    case TEST_STAGE_MSC_CBW:
        msc_obj->next_stage = TEST_STAGE_MSC_DATA_DCONN;
        break;
    default:
        abort();
        break;
    }
}

static void msc_data_transfer_cb(usb_transfer_t *transfer)
{
    // The data stage should have either completed, or failed due to the disconnection.
    TEST_ASSERT(transfer->status == USB_TRANSFER_STATUS_COMPLETED || transfer->status == USB_TRANSFER_STATUS_NO_DEVICE);
    if (transfer->status == USB_TRANSFER_STATUS_COMPLETED) {
        printf("Data transfer completed\n");
        TEST_ASSERT_EQUAL(transfer->num_bytes, transfer->actual_num_bytes);
    } else {
        printf("Data transfer NOT completed: No device\n");
        TEST_ASSERT_EQUAL(0, transfer->actual_num_bytes);
    }
    msc_client_obj_t *msc_obj = (msc_client_obj_t *)transfer->context;
    msc_obj->event_count++;
    // If all transfers dequeued and device gone event occurred. Go to next stage
    if (msc_obj->event_count >= msc_obj->num_data_transfers + 1) {
        msc_obj->next_stage = TEST_STAGE_DEV_CLOSE;
    }
}

static void msc_client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    msc_client_obj_t *msc_obj = (msc_client_obj_t *)arg;
    switch (event_msg->event) {
    case USB_HOST_CLIENT_EVENT_NEW_DEV:
        TEST_ASSERT_EQUAL(TEST_STAGE_WAIT_CONN, msc_obj->cur_stage);
        msc_obj->next_stage = TEST_STAGE_DEV_OPEN;
        msc_obj->dev_addr = event_msg->new_dev.address;
        break;
    case USB_HOST_CLIENT_EVENT_DEV_GONE:
        msc_obj->event_count++;
        // If all transfers dequeued and device gone event occurred. Go to next stage
        if (msc_obj->event_count >= msc_obj->num_data_transfers + 1) {
            msc_obj->next_stage = TEST_STAGE_DEV_CLOSE;
        }
        break;
    default:
        abort();    // Should never occur in this test
        break;
    }
}

void msc_client_async_dconn_task(void *arg)
{
    msc_client_obj_t msc_obj;
    // Initialize test params
    memcpy(&msc_obj.test_param, arg, sizeof(msc_client_test_param_t));
    // Initialize MSC device info
    msc_obj.dev_info = dev_msc_get_info();
    // Initialize client variables
    msc_obj.client_hdl = NULL;
    msc_obj.dev_hdl = NULL;
    // Initialize test state
    msc_obj.cur_stage = TEST_STAGE_WAIT_CONN;
    msc_obj.next_stage = TEST_STAGE_WAIT_CONN;
    msc_obj.num_data_transfers = msc_obj.test_param.num_sectors_per_xfer / msc_obj.dev_info->scsi_sector_size;
    msc_obj.event_count = 0;

    // Register client
    usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = MSC_ASYNC_CLIENT_MAX_EVENT_MSGS,
        .async = {
            .client_event_callback = msc_client_event_cb,
            .callback_arg = (void *) &msc_obj,
        },
    };
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_register(&client_config, &msc_obj.client_hdl));

    // Allocate transfers
    usb_transfer_t *xfer_out;   // Must be large enough to contain CBW and MSC reset control transfer
    usb_transfer_t *xfer_in[msc_obj.num_data_transfers];   // We manually split the data stage into multiple transfers
    size_t xfer_out_size = MAX(sizeof(mock_msc_bulk_cbw_t), sizeof(usb_setup_packet_t));
    size_t xfer_in_size = msc_obj.dev_info->scsi_sector_size;
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_alloc(xfer_out_size, 0, &xfer_out));
    xfer_out->context = (void *)&msc_obj;
    for (int i = 0; i < msc_obj.num_data_transfers; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_alloc(xfer_in_size, 0, &xfer_in[i]));
        xfer_in[i]->context = (void *)&msc_obj;
    }

    // Wait to be started by main thread
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ESP_LOGD(MSC_CLIENT_TAG, "Starting");

    bool exit_loop = false;
    bool skip_event_handling = false;
    int dconn_iter = 0;
    while (!exit_loop) {
        if (!skip_event_handling) {
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_handle_events(msc_obj.client_hdl, portMAX_DELAY));
        }
        skip_event_handling = false;
        if (msc_obj.cur_stage == msc_obj.next_stage) {
            continue;
        }
        msc_obj.cur_stage = msc_obj.next_stage;

        switch (msc_obj.cur_stage) {
        case TEST_STAGE_WAIT_CONN: {
            // Nothing to do while waiting for connection
            break;
        }
        case TEST_STAGE_DEV_OPEN: {
            ESP_LOGD(MSC_CLIENT_TAG, "Open");
            // Open the device
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_open(msc_obj.client_hdl, msc_obj.dev_addr, &msc_obj.dev_hdl));
            // Target our transfers to the device
            xfer_out->device_handle = msc_obj.dev_hdl;
            xfer_out->callback = msc_reset_cbw_transfer_cb;
            for (int i = 0; i < msc_obj.num_data_transfers; i++) {
                xfer_in[i]->device_handle = msc_obj.dev_hdl;
                xfer_in[i]->callback = msc_data_transfer_cb;
            }
            // Get device info to get device speed
            usb_device_info_t dev_info;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_info(msc_obj.dev_hdl, &dev_info));
            msc_obj.dev_speed = dev_info.speed;
            // Check the device descriptor of the opened device
            const usb_device_desc_t *device_desc;
            const usb_device_desc_t *device_desc_ref = dev_msc_get_dev_desc(msc_obj.dev_speed);
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_get_device_descriptor(msc_obj.dev_hdl, &device_desc));
            TEST_ASSERT_EQUAL(device_desc_ref->bLength, device_desc->bLength);
            TEST_ASSERT_EQUAL_MEMORY_MESSAGE(device_desc_ref, device_desc, device_desc_ref->bLength, "Device descriptors do not match.");
            // Claim the MSC interface
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_interface_claim(msc_obj.client_hdl,
                                                               msc_obj.dev_hdl,
                                                               msc_obj.dev_info->bInterfaceNumber,
                                                               msc_obj.dev_info->bAlternateSetting));
            msc_obj.next_stage = TEST_STAGE_MSC_RESET;
            skip_event_handling = true; // Need to execute TEST_STAGE_MSC_RESET
            break;
        }
        case TEST_STAGE_MSC_RESET: {
            ESP_LOGD(MSC_CLIENT_TAG, "MSC Reset");
            // Send an MSC SCSI interface reset
            MOCK_MSC_SCSI_REQ_INIT_RESET((usb_setup_packet_t *)xfer_out->data_buffer, msc_obj.dev_info->bInterfaceNumber);
            xfer_out->num_bytes = sizeof(usb_setup_packet_t);
            xfer_out->bEndpointAddress = 0;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_submit_control(msc_obj.client_hdl, xfer_out));
            // Next stage set from transfer callback
            break;
        }
        case TEST_STAGE_MSC_CBW: {
            ESP_LOGD(MSC_CLIENT_TAG, "CBW");
            mock_msc_scsi_init_cbw((mock_msc_bulk_cbw_t *)xfer_out->data_buffer,
                                   true,
                                   0,
                                   msc_obj.test_param.num_sectors_per_xfer,
                                   msc_obj.dev_info->scsi_sector_size,
                                   msc_obj.test_param.msc_scsi_xfer_tag);
            xfer_out->num_bytes = sizeof(mock_msc_bulk_cbw_t);
            xfer_out->bEndpointAddress = msc_obj.dev_info->out_up_addr;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_submit(xfer_out));
            // Next stage set from transfer callback
            break;
        }
        case TEST_STAGE_MSC_DATA_DCONN: {
            ESP_LOGD(MSC_CLIENT_TAG, "Data (%d transfers) and disconnect", msc_obj.num_data_transfers);
            // Setup the Data IN transfers
            const usb_ep_desc_t *in_ep_desc = dev_msc_get_in_ep_desc(msc_obj.dev_speed);
            const int bulk_ep_mps = USB_EP_DESC_GET_MPS(in_ep_desc);
            for (int i = 0; i < msc_obj.num_data_transfers; i++) {
                xfer_in[i]->num_bytes = usb_round_up_to_mps(msc_obj.dev_info->scsi_sector_size, bulk_ep_mps);
                xfer_in[i]->bEndpointAddress = msc_obj.dev_info->in_ep_addr;
            }
            // Submit those transfers
            for (int i = 0; i < msc_obj.num_data_transfers; i++) {
                TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_submit(xfer_in[i]));
            }
            // Trigger a disconnect by powering OFF the root port
            usb_host_lib_set_root_port_power(false);
            // Next stage set from transfer callback
            break;
        }
        case TEST_STAGE_DEV_CLOSE: {
            ESP_LOGD(MSC_CLIENT_TAG, "Close");
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_interface_release(msc_obj.client_hdl, msc_obj.dev_hdl, msc_obj.dev_info->bInterfaceNumber));
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_close(msc_obj.client_hdl, msc_obj.dev_hdl));
            vTaskDelay(10); // Yield to USB Host task so it can handle the disconnection

            // The device has disconnected and it's disconnection has been handled
            printf("Dconn iter %d done\n", dconn_iter);
            if (++dconn_iter < TEST_DCONN_ITERATIONS) {
                // Start the next test iteration by going back to TEST_STAGE_WAIT_CONN and reenabling connections
                msc_obj.next_stage = TEST_STAGE_WAIT_CONN;
                skip_event_handling = true; // Need to execute TEST_STAGE_WAIT_CONN
                // Allow connections again by powering ON the root port
                usb_host_lib_set_root_port_power(true);
            } else {
                exit_loop = true;
            }
            break;
        }
        default:
            abort();
            break;
        }
    }
    // Free transfers
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_free(xfer_out));
    for (int i = 0; i < msc_obj.num_data_transfers; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_free(xfer_in[i]));
    }
    // Deregister the client
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_deregister(msc_obj.client_hdl));
    ESP_LOGD(MSC_CLIENT_TAG, "Done");
    vTaskDelete(NULL);
}
