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
#include "test_usb_common.h"
#include "test_usb_mock_msc.h"
#include "msc_client.h"
#include "usb/usb_host.h"
#include "unity.h"

/*
Implementation of an MSC client used for USB Host Tests

- Implemented using sequential call patterns, meaning:
    - The entire client is contained within a single task
    - All API calls and callbacks are run sequentially
    - No critical sections required since everything is sequential
- The MSC client will:
    - Register itself as a client
    - Receive USB_HOST_CLIENT_EVENT_NEW_DEV event message, and open the device
    - Allocate IN and OUT transfer objects for MSC SCSI transfers
    - Iterate through multiple MSC SCSI block reads
    - Free transfer objects
    - Close device
    - Deregister MSC client
*/

typedef enum {
    TEST_STAGE_WAIT_CONN,
    TEST_STAGE_DEV_OPEN,
    TEST_STAGE_MSC_RESET,
    TEST_STAGE_MSC_CBW,
    TEST_STAGE_MSC_DATA,
    TEST_STAGE_MSC_CSW,
    TEST_STAGE_DEV_CLOSE,
} test_stage_t;

typedef struct {
    msc_client_test_param_t test_param;
    test_stage_t cur_stage;
    test_stage_t next_stage;
    uint8_t dev_addr_to_open;
    usb_host_client_handle_t client_hdl;
    usb_device_handle_t dev_hdl;
    int num_sectors_read;
    usb_speed_t dev_speed;
} msc_client_obj_t;

static void msc_transfer_cb(usb_transfer_t *transfer)
{
    msc_client_obj_t *msc_obj = (msc_client_obj_t *)transfer->context;
    switch (msc_obj->cur_stage) {
    case TEST_STAGE_MSC_RESET: {
        //Check MSC SCSI interface reset
        TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, transfer->status, "Transfer NOT completed");
        TEST_ASSERT_EQUAL(transfer->num_bytes, transfer->actual_num_bytes);
        msc_obj->next_stage = TEST_STAGE_MSC_CBW;
        break;
    }
    case TEST_STAGE_MSC_CBW: {
        //Check MSC SCSI CBW transfer
        TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, transfer->status, "Transfer NOT completed");
        TEST_ASSERT_EQUAL(sizeof(mock_msc_bulk_cbw_t), transfer->actual_num_bytes);
        msc_obj->next_stage = TEST_STAGE_MSC_DATA;
        break;
    }
    case TEST_STAGE_MSC_DATA: {
        //Check MSC SCSI data IN transfer
        TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, transfer->status, "Transfer NOT completed");
        TEST_ASSERT_EQUAL(MOCK_MSC_SCSI_SECTOR_SIZE * msc_obj->test_param.num_sectors_per_xfer, transfer->actual_num_bytes);
        msc_obj->next_stage = TEST_STAGE_MSC_CSW;
        break;
    }
    case TEST_STAGE_MSC_CSW: {
        //Check MSC SCSI CSW transfer
        TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, transfer->status, "Transfer NOT completed");
        TEST_ASSERT_TRUE(mock_msc_scsi_check_csw((mock_msc_bulk_csw_t *)transfer->data_buffer, msc_obj->test_param.msc_scsi_xfer_tag));
        msc_obj->num_sectors_read += msc_obj->test_param.num_sectors_per_xfer;
        if (msc_obj->num_sectors_read < msc_obj->test_param.num_sectors_to_read) {
            msc_obj->next_stage = TEST_STAGE_MSC_CBW;
        } else {
            msc_obj->next_stage = TEST_STAGE_DEV_CLOSE;
        }
        break;
    }
    default: {
        abort();
        break;
    }
    }
}

static void msc_client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    msc_client_obj_t *msc_obj = (msc_client_obj_t *)arg;
    switch (event_msg->event) {
    case USB_HOST_CLIENT_EVENT_NEW_DEV:
        TEST_ASSERT_EQUAL(TEST_STAGE_WAIT_CONN, msc_obj->cur_stage);
        msc_obj->next_stage = TEST_STAGE_DEV_OPEN;
        msc_obj->dev_addr_to_open = event_msg->new_dev.address;
        break;
    default:
        abort();    //Should never occur in this test
        break;

    }
}

void msc_client_async_seq_task(void *arg)
{
    msc_client_obj_t msc_obj;
    memcpy(&msc_obj.test_param, arg, sizeof(msc_client_test_param_t));
    msc_obj.cur_stage = TEST_STAGE_WAIT_CONN;
    msc_obj.next_stage = TEST_STAGE_WAIT_CONN;
    msc_obj.client_hdl = NULL;
    msc_obj.dev_addr_to_open = 0;
    msc_obj.dev_hdl = NULL;
    msc_obj.num_sectors_read = 0;

    //Register client
    usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = MSC_ASYNC_CLIENT_MAX_EVENT_MSGS,
        .async = {
            .client_event_callback = msc_client_event_cb,
            .callback_arg = (void *) &msc_obj,
        },
    };
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_register(&client_config, &msc_obj.client_hdl));

    //Allocate transfers
    usb_transfer_t *xfer_out = NULL;    //Must be large enough to contain CBW and MSC reset control transfer
    usb_transfer_t *xfer_in = NULL;     //Must be large enough to contain CSW and Data
    size_t out_worst_case_size = MAX(sizeof(mock_msc_bulk_cbw_t), sizeof(usb_setup_packet_t));
    size_t in_worst_case_size = usb_round_up_to_mps(MAX(MOCK_MSC_SCSI_SECTOR_SIZE * msc_obj.test_param.num_sectors_per_xfer, sizeof(mock_msc_bulk_csw_t)), MOCK_MSC_SCSI_BULK_EP_MPS_HS);
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_alloc(out_worst_case_size, 0, &xfer_out));
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_alloc(in_worst_case_size, 0, &xfer_in));
    xfer_out->callback = msc_transfer_cb;
    xfer_in->callback = msc_transfer_cb;
    xfer_out->context = (void *)&msc_obj;
    xfer_in->context = (void *)&msc_obj;

    //Wait to be started by main thread
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ESP_LOGD(MSC_CLIENT_TAG, "Starting");

    bool exit_loop = false;
    bool skip_event_handling = false;
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
        case TEST_STAGE_DEV_OPEN: {
            ESP_LOGD(MSC_CLIENT_TAG, "Open");
            //Open the device
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_open(msc_obj.client_hdl, msc_obj.dev_addr_to_open, &msc_obj.dev_hdl));
            //Target our transfers to the device
            xfer_out->device_handle = msc_obj.dev_hdl;
            xfer_in->device_handle = msc_obj.dev_hdl;
            //Check the VID/PID of the opened device
            const usb_device_desc_t *device_desc;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_get_device_descriptor(msc_obj.dev_hdl, &device_desc));
            TEST_ASSERT_EQUAL(msc_obj.test_param.idVendor, device_desc->idVendor);
            TEST_ASSERT_EQUAL(msc_obj.test_param.idProduct, device_desc->idProduct);
            //Get device info to get device speed
            usb_device_info_t dev_info;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_info(msc_obj.dev_hdl, &dev_info));
            msc_obj.dev_speed = dev_info.speed;
            //Claim the MSC interface
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_interface_claim(msc_obj.client_hdl, msc_obj.dev_hdl, MOCK_MSC_SCSI_INTF_NUMBER, MOCK_MSC_SCSI_INTF_ALT_SETTING));
            msc_obj.next_stage = TEST_STAGE_MSC_RESET;
            skip_event_handling = true; //Need to execute TEST_STAGE_MSC_RESET
            break;
        }
        case TEST_STAGE_MSC_RESET: {
            ESP_LOGD(MSC_CLIENT_TAG, "MSC Reset");
            //Send an MSC SCSI interface reset
            MOCK_MSC_SCSI_REQ_INIT_RESET((usb_setup_packet_t *)xfer_out->data_buffer, MOCK_MSC_SCSI_INTF_NUMBER);
            xfer_out->num_bytes = sizeof(usb_setup_packet_t);
            xfer_out->bEndpointAddress = 0;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_submit_control(msc_obj.client_hdl, xfer_out));
            //Test that an inflight control transfer cannot be resubmitted
            TEST_ASSERT_EQUAL(ESP_ERR_NOT_FINISHED, usb_host_transfer_submit_control(msc_obj.client_hdl, xfer_out));
            //Next stage set from transfer callback
            break;
        }
        case TEST_STAGE_MSC_CBW: {
            ESP_LOGD(MSC_CLIENT_TAG, "CBW");
            mock_msc_scsi_init_cbw((mock_msc_bulk_cbw_t *)xfer_out->data_buffer, true, msc_obj.next_stage, msc_obj.test_param.num_sectors_per_xfer, msc_obj.test_param.msc_scsi_xfer_tag);
            xfer_out->num_bytes = sizeof(mock_msc_bulk_cbw_t);
            xfer_out->bEndpointAddress = MOCK_MSC_SCSI_BULK_OUT_EP_ADDR;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_submit(xfer_out));
            //Test that an inflight transfer cannot be resubmitted
            TEST_ASSERT_EQUAL(ESP_ERR_NOT_FINISHED, usb_host_transfer_submit(xfer_out));
            //Next stage set from transfer callback
            break;
        }
        case TEST_STAGE_MSC_DATA: {
            ESP_LOGD(MSC_CLIENT_TAG, "Data");
            const int bulk_ep_mps = (msc_obj.dev_speed == USB_SPEED_HIGH)
                                    ? MOCK_MSC_SCSI_BULK_EP_MPS_HS
                                    : MOCK_MSC_SCSI_BULK_EP_MPS_FS;
            xfer_in->num_bytes = usb_round_up_to_mps(MOCK_MSC_SCSI_SECTOR_SIZE * msc_obj.test_param.num_sectors_per_xfer, bulk_ep_mps);
            xfer_in->bEndpointAddress = MOCK_MSC_SCSI_BULK_IN_EP_ADDR;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_submit(xfer_in));
            //Test that an inflight transfer cannot be resubmitted
            TEST_ASSERT_EQUAL(ESP_ERR_NOT_FINISHED, usb_host_transfer_submit(xfer_in));
            //Next stage set from transfer callback
            break;
        }
        case TEST_STAGE_MSC_CSW: {
            ESP_LOGD(MSC_CLIENT_TAG, "CSW");
            const int bulk_ep_mps = (msc_obj.dev_speed == USB_SPEED_HIGH)
                                    ? MOCK_MSC_SCSI_BULK_EP_MPS_HS
                                    : MOCK_MSC_SCSI_BULK_EP_MPS_FS;
            xfer_in->num_bytes = usb_round_up_to_mps(sizeof(mock_msc_bulk_csw_t), bulk_ep_mps);
            xfer_in->bEndpointAddress = MOCK_MSC_SCSI_BULK_IN_EP_ADDR;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_submit(xfer_in));
            //Test that an inflight transfer cannot be resubmitted
            TEST_ASSERT_EQUAL(ESP_ERR_NOT_FINISHED, usb_host_transfer_submit(xfer_in));
            //Next stage set from transfer callback
            break;
        }
        case TEST_STAGE_DEV_CLOSE: {
            ESP_LOGD(MSC_CLIENT_TAG, "Close");
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_interface_release(msc_obj.client_hdl, msc_obj.dev_hdl, MOCK_MSC_SCSI_INTF_NUMBER));
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_close(msc_obj.client_hdl, msc_obj.dev_hdl));
            exit_loop = true;
            break;
        }
        default:
            abort();
            break;
        }
    }
    //Free transfers and deregister the client
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_free(xfer_out));
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_free(xfer_in));
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_deregister(msc_obj.client_hdl));
    ESP_LOGD(MSC_CLIENT_TAG, "Done");
    vTaskDelete(NULL);
}
