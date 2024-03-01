/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "test_usb_common.h"
#include "ctrl_client.h"
#include "usb/usb_host.h"
#include "unity.h"

/*
Implementation of a control transfer client used for USB Host Tests.

- Implemented using sequential call patterns, meaning:
    - The entire client is contained within a single task
    - All API calls and callbacks are run sequentially
    - No critical sections required since everything is sequential
- The control transfer client will:
    - Register itself as a client
    - Receive USB_HOST_CLIENT_EVENT_NEW_DEV event message, and open the device
    - Allocate multiple transfer objects
    - Submit a number of control transfers (get configuration descriptor requests)
    - Free transfer objects
    - Close the device
    - Deregister control client
*/

#define CTRL_CLIENT_MAX_EVENT_MSGS      5
#define NUM_TRANSFER_OBJ                3
#define MAX_TRANSFER_BYTES              256

const char *CTRL_CLIENT_TAG = "Ctrl Client";

typedef enum {
    TEST_STAGE_WAIT_CONN,
    TEST_STAGE_DEV_OPEN,
    TEST_STAGE_CTRL_XFER,
    TEST_STAGE_CTRL_XFER_WAIT,
    TEST_STAGE_DEV_CLOSE,
} test_stage_t;

typedef struct {
    ctrl_client_test_param_t test_param;
    test_stage_t cur_stage;
    test_stage_t next_stage;
    uint8_t num_xfer_done;
    uint8_t num_xfer_sent;
    uint8_t dev_addr_to_open;
    usb_host_client_handle_t client_hdl;
    usb_device_handle_t dev_hdl;
    const usb_config_desc_t *config_desc_cached;
} ctrl_client_obj_t;

static void ctrl_transfer_cb(usb_transfer_t *transfer)
{
    ctrl_client_obj_t *ctrl_obj = (ctrl_client_obj_t *)transfer->context;
    //Check the completed control transfer
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, transfer->status, "Transfer NOT completed");
    TEST_ASSERT_EQUAL(ctrl_obj->config_desc_cached->wTotalLength, transfer->actual_num_bytes - sizeof(usb_setup_packet_t));
    ctrl_obj->num_xfer_done++;
    if (ctrl_obj->num_xfer_sent < ctrl_obj->test_param.num_ctrl_xfer_to_send) {
        ctrl_obj->next_stage = TEST_STAGE_CTRL_XFER;
    } else if (ctrl_obj->num_xfer_done == ctrl_obj->test_param.num_ctrl_xfer_to_send) {
        ctrl_obj->next_stage = TEST_STAGE_DEV_CLOSE;
    }
}

static void ctrl_client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    ctrl_client_obj_t *ctrl_obj = (ctrl_client_obj_t *)arg;
    switch (event_msg->event) {
    case USB_HOST_CLIENT_EVENT_NEW_DEV:
        TEST_ASSERT_EQUAL(TEST_STAGE_WAIT_CONN, ctrl_obj->cur_stage);
        ctrl_obj->next_stage = TEST_STAGE_DEV_OPEN;
        ctrl_obj->dev_addr_to_open = event_msg->new_dev.address;
        break;
    default:
        abort();    //Should never occur in this test
        break;
    }
}

void ctrl_client_async_seq_task(void *arg)
{
    ctrl_client_obj_t ctrl_obj = {0};
    memcpy(&ctrl_obj.test_param, arg, sizeof(ctrl_client_test_param_t));
    ctrl_obj.cur_stage = TEST_STAGE_WAIT_CONN;
    ctrl_obj.next_stage = TEST_STAGE_WAIT_CONN;

    //Register client
    usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = CTRL_CLIENT_MAX_EVENT_MSGS,
        .async = {
            .client_event_callback = ctrl_client_event_cb,
            .callback_arg = (void *) &ctrl_obj,
        },
    };
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_register(&client_config, &ctrl_obj.client_hdl));

    //Allocate transfers
    usb_transfer_t *ctrl_xfer[NUM_TRANSFER_OBJ] = {NULL};
    for (int i = 0; i < NUM_TRANSFER_OBJ; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_alloc(sizeof(usb_setup_packet_t) + MAX_TRANSFER_BYTES, 0, &ctrl_xfer[i]));
        ctrl_xfer[i]->callback = ctrl_transfer_cb;
        ctrl_xfer[i]->context = (void *)&ctrl_obj;
    }

    //Wait to be started by main thread
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ESP_LOGD(CTRL_CLIENT_TAG, "Starting");

    bool exit_loop = false;
    bool skip_event_handling = false;
    while (!exit_loop) {
        if (!skip_event_handling) {
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_handle_events(ctrl_obj.client_hdl, portMAX_DELAY));
        }
        skip_event_handling = false;
        if (ctrl_obj.cur_stage == ctrl_obj.next_stage) {
            continue;
        }
        ctrl_obj.cur_stage = ctrl_obj.next_stage;

        switch (ctrl_obj.next_stage) {
        case TEST_STAGE_DEV_OPEN: {
            ESP_LOGD(CTRL_CLIENT_TAG, "Open");
            //Open the device
            TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, usb_host_device_open(ctrl_obj.client_hdl, ctrl_obj.dev_addr_to_open, &ctrl_obj.dev_hdl), "Failed to open the device");
            //Target our transfers to the device
            for (int i = 0; i < NUM_TRANSFER_OBJ; i++) {
                ctrl_xfer[i]->device_handle = ctrl_obj.dev_hdl;
            }
            //Check the VID/PID of the opened device
            const usb_device_desc_t *device_desc;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_get_device_descriptor(ctrl_obj.dev_hdl, &device_desc));
            TEST_ASSERT_EQUAL(ctrl_obj.test_param.idVendor, device_desc->idVendor);
            TEST_ASSERT_EQUAL(ctrl_obj.test_param.idProduct, device_desc->idProduct);
            //Cache the active configuration descriptor for later comparison
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_get_active_config_descriptor(ctrl_obj.dev_hdl, &ctrl_obj.config_desc_cached));
            ctrl_obj.next_stage = TEST_STAGE_CTRL_XFER;
            skip_event_handling = true;
            break;
        }
        case TEST_STAGE_CTRL_XFER: {
            ESP_LOGD(CTRL_CLIENT_TAG, "Transfer");
            //Send a control transfer to get the device's configuration descriptor
            usb_transfer_t *transfer = ctrl_xfer[ctrl_obj.num_xfer_sent % NUM_TRANSFER_OBJ];
            USB_SETUP_PACKET_INIT_GET_CONFIG_DESC((usb_setup_packet_t *)transfer->data_buffer, 0, MAX_TRANSFER_BYTES);
            transfer->num_bytes = sizeof(usb_setup_packet_t) + MAX_TRANSFER_BYTES;
            transfer->bEndpointAddress = 0x80;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_submit_control(ctrl_obj.client_hdl, transfer));
            ctrl_obj.num_xfer_sent++;
            ctrl_obj.next_stage = TEST_STAGE_CTRL_XFER_WAIT;
            skip_event_handling = true;
            break;
        }
        case TEST_STAGE_CTRL_XFER_WAIT: {
            //Nothing to do but wait
            break;
        }
        case TEST_STAGE_DEV_CLOSE: {
            ESP_LOGD(CTRL_CLIENT_TAG, "Close");
            vTaskDelay(10); // Give USB Host Lib some time to process all trnsfers
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_close(ctrl_obj.client_hdl, ctrl_obj.dev_hdl));
            exit_loop = true;
            break;
        }
        default:
            abort();
            break;
        }
    }
    //Free transfers and deregister client
    for (int i = 0; i < NUM_TRANSFER_OBJ; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, usb_host_transfer_free(ctrl_xfer[i]));
    }
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_deregister(ctrl_obj.client_hdl));
    ESP_LOGD(CTRL_CLIENT_TAG, "Done");
    vTaskDelete(NULL);
}
