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
#include "test_usb_mock_msc.h"
#include "test_usb_common.h"
#include "msc_client.h"
#include "usb/usb_host.h"
#include "unity.h"

/*
Implementation of an asynchronous MSC client used for USB Host enumeration test.

- The MSC client will:
    - Register itself as a client
    - Receive USB_HOST_CLIENT_EVENT_NEW_DEV event message, and open the device
    - Check the device and configuration descriptor of the device
    - Check the device's information
    - Close device
    - Repeat for multiple iterations from waiting connection by forcing a disconnection
    - Deregister MSC client
*/

#define TEST_ENUM_ITERATIONS   3

typedef enum {
    TEST_STAGE_WAIT_CONN,
    TEST_STAGE_DEV_OPEN,
    TEST_STAGE_CHECK_DEV_DESC,
    TEST_STAGE_CHECK_CONFIG_DESC,
    TEST_STAGE_CHECK_STR_DESC,
    TEST_STAGE_DEV_CLOSE,
} test_stage_t;

typedef struct {
    test_stage_t cur_stage;
    test_stage_t next_stage;
    uint8_t dev_addr_to_open;
    usb_host_client_handle_t client_hdl;
    usb_device_handle_t dev_hdl;
    usb_speed_t dev_speed;
} msc_client_obj_t;

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

static void mock_msc_scsi_init_reference_ep_descriptors(const msc_client_obj_t *msc_obj)
{
    uint8_t *dest_ptr = mock_msc_scsi_config_desc;
    dest_ptr += USB_CONFIG_DESC_SIZE;
    dest_ptr += USB_INTF_DESC_SIZE;

    const usb_ep_desc_t en_desc_in = (msc_obj->dev_speed == USB_SPEED_HIGH)
                                     ? mock_msc_scsi_bulk_in_ep_desc_hs
                                     : mock_msc_scsi_bulk_in_ep_desc_fs;
    const usb_ep_desc_t en_desc_out = (msc_obj->dev_speed == USB_SPEED_HIGH)
                                      ? mock_msc_scsi_bulk_out_ep_desc_hs
                                      : mock_msc_scsi_bulk_out_ep_desc_fs;

    memcpy(dest_ptr, (void*)&en_desc_in, sizeof(en_desc_in));
    dest_ptr += USB_EP_DESC_SIZE;
    memcpy(dest_ptr, (void*)&en_desc_out, sizeof(en_desc_out));
}

void msc_client_async_enum_task(void *arg)
{
    msc_client_obj_t msc_obj;
    msc_obj.cur_stage = TEST_STAGE_WAIT_CONN;
    msc_obj.next_stage = TEST_STAGE_WAIT_CONN;
    msc_obj.client_hdl = NULL;
    msc_obj.dev_addr_to_open = 0;
    msc_obj.dev_hdl = NULL;

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

    //Wait to be started by main thread
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ESP_LOGD(MSC_CLIENT_TAG, "Starting");

    bool exit_loop = false;
    bool skip_event_handling = false;
    int enum_iter = 0;
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
            //Wait for connection, nothing to do
            break;
        }
        case TEST_STAGE_DEV_OPEN: {
            ESP_LOGD(MSC_CLIENT_TAG, "Open");
            //Open the device
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_open(msc_obj.client_hdl, msc_obj.dev_addr_to_open, &msc_obj.dev_hdl));
            msc_obj.next_stage = TEST_STAGE_CHECK_DEV_DESC;
            //Get device info to get device speed
            usb_device_info_t dev_info;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_info(msc_obj.dev_hdl, &dev_info));
            msc_obj.dev_speed = dev_info.speed;
            mock_msc_scsi_init_reference_ep_descriptors(&msc_obj);
            skip_event_handling = true; //Need to execute TEST_STAGE_CHECK_DEV_DESC
            break;
        }
        case TEST_STAGE_CHECK_DEV_DESC: {
            //Check the device descriptor
            const usb_device_desc_t *device_desc;
            const usb_device_desc_t *device_desc_ref = &mock_msc_scsi_dev_desc;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_get_device_descriptor(msc_obj.dev_hdl, &device_desc));
            TEST_ASSERT_EQUAL(device_desc_ref->bLength, device_desc->bLength);
            TEST_ASSERT_EQUAL_MEMORY_MESSAGE(device_desc_ref, device_desc, device_desc_ref->bLength, "Device descriptors do not match.");
            msc_obj.next_stage = TEST_STAGE_CHECK_CONFIG_DESC;
            skip_event_handling = true; //Need to execute TEST_STAGE_CHECK_CONFIG_DESC
            break;
        }

        case TEST_STAGE_CHECK_CONFIG_DESC: {
            //Check the configuration descriptor
            const usb_config_desc_t *config_desc;
            const usb_config_desc_t *config_desc_ref = (const usb_config_desc_t *)mock_msc_scsi_config_desc;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_get_active_config_descriptor(msc_obj.dev_hdl, &config_desc));
            TEST_ASSERT_EQUAL_MESSAGE(config_desc_ref->wTotalLength, config_desc->wTotalLength, "Incorrent length of CFG descriptor");
            TEST_ASSERT_EQUAL_MEMORY_MESSAGE(config_desc_ref, config_desc, config_desc_ref->wTotalLength, "Configuration descriptors do not match");
            msc_obj.next_stage = TEST_STAGE_CHECK_STR_DESC;
            skip_event_handling = true; //Need to execute TEST_STAGE_CHECK_STR_DESC
            break;
        }
        case TEST_STAGE_CHECK_STR_DESC: {
            usb_device_info_t dev_info;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_info(msc_obj.dev_hdl, &dev_info));
            //Check manufacturer string descriptors
            const usb_str_desc_t *manu_str_desc_ref = (const usb_str_desc_t *)mock_msc_scsi_str_desc_manu;
            const usb_str_desc_t *product_str_desc_ref = (const usb_str_desc_t *)mock_msc_scsi_str_desc_prod;
            const usb_str_desc_t *ser_num_str_desc_ref = (const usb_str_desc_t *)mock_msc_scsi_str_desc_ser_num;
            TEST_ASSERT_EQUAL(manu_str_desc_ref->bLength, dev_info.str_desc_manufacturer->bLength);
            TEST_ASSERT_EQUAL(product_str_desc_ref->bLength, dev_info.str_desc_product->bLength);
            TEST_ASSERT_EQUAL(ser_num_str_desc_ref->bLength, dev_info.str_desc_serial_num->bLength);
            TEST_ASSERT_EQUAL_MEMORY_MESSAGE(manu_str_desc_ref, dev_info.str_desc_manufacturer, manu_str_desc_ref->bLength, "Manufacturer string descriptors do not match.");
            TEST_ASSERT_EQUAL_MEMORY_MESSAGE(product_str_desc_ref, dev_info.str_desc_product, manu_str_desc_ref->bLength, "Product string descriptors do not match.");
            //TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ser_num_str_desc_ref, dev_info.str_desc_serial_num , manu_str_desc_ref->bLength, "Serial number string descriptors do not match.");
            //Get dev info and compare
            msc_obj.next_stage = TEST_STAGE_DEV_CLOSE;
            skip_event_handling = true; //Need to execute TEST_STAGE_DEV_CLOSE
            break;
        }

        case TEST_STAGE_DEV_CLOSE: {
            ESP_LOGD(MSC_CLIENT_TAG, "Close");
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_close(msc_obj.client_hdl, msc_obj.dev_hdl));
            enum_iter++;
            if (enum_iter < TEST_ENUM_ITERATIONS) {
                //Start the next test iteration by disconnecting the device, then going back to TEST_STAGE_WAIT_CONN stage
                test_usb_set_phy_state(false, 0);
                test_usb_set_phy_state(true, 0);
                msc_obj.next_stage = TEST_STAGE_WAIT_CONN;
                skip_event_handling = true; //Need to execute TEST_STAGE_WAIT_CONN
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
    //Free transfers and deregister the client
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_deregister(msc_obj.client_hdl));
    ESP_LOGD(MSC_CLIENT_TAG, "Done");
    vTaskDelete(NULL);
}
