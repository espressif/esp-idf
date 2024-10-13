/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "multiconf_client.h"
#include "mock_msc.h"
#include "dev_msc.h"
#include "usb/usb_host.h"
#include "unity.h"

/*
Implementation of a multi-configuration client used for USB Host Tests.

- The multi-configuration client will:
    - Register itself as a client
    - Receive USB_HOST_CLIENT_EVENT_NEW_DEV event message, and open the device
    - Get active configuration descriptor
    - Start handling client events
    - Wait for a request from main task to read a configuration descriptor
    - Compare the obtained configuration descriptor with the active configuration descriptor
    - Free the memory used for storing the configuration descriptor
    - Close the device
    - Deregister control client
*/

const char *MULTICONF_CLIENT_TAG = "Multi config Client";

#define CLIENT_NUM_EVENT_MSG        5

typedef enum {
    TEST_STAGE_WAIT_CONN,
    TEST_STAGE_DEV_OPEN,
    TEST_STAGE_WAIT,
    TEST_STAGE_CHECK_CONFIG_DESC,
    TEST_STAGE_DEV_CLOSE,
} test_stage_t;

typedef struct {
    // Test parameters
    multiconf_client_test_param_t test_param;
    // device info
    uint8_t dev_addr;
    usb_speed_t dev_speed;
    // Client variables
    usb_host_client_handle_t client_hdl;
    usb_device_handle_t dev_hdl;
    // Test state
    test_stage_t cur_stage;
    test_stage_t next_stage;
    const usb_config_desc_t *config_desc_cached;
} multiconf_client_obj_t;

static multiconf_client_obj_t *s_multiconf_obj;

static void multiconf_client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    multiconf_client_obj_t *multiconf_obj = (multiconf_client_obj_t *)arg;
    switch (event_msg->event) {
    case USB_HOST_CLIENT_EVENT_NEW_DEV:
        TEST_ASSERT_EQUAL(TEST_STAGE_WAIT_CONN, multiconf_obj->cur_stage);
        multiconf_obj->next_stage = TEST_STAGE_DEV_OPEN;
        multiconf_obj->dev_addr = event_msg->new_dev.address;
        break;
    default:
        abort();    // Should never occur in this test
        break;
    }
}

void multiconf_client_async_task(void *arg)
{
    multiconf_client_obj_t multiconf_obj;
    // Initialize test params
    memcpy(&multiconf_obj.test_param, arg, sizeof(multiconf_client_test_param_t));
    // Initialize client variables
    multiconf_obj.client_hdl = NULL;
    multiconf_obj.dev_hdl = NULL;
    // Initialize test stage
    multiconf_obj.cur_stage = TEST_STAGE_WAIT_CONN;
    multiconf_obj.next_stage = TEST_STAGE_WAIT_CONN;
    multiconf_obj.dev_addr = 0;

    // Register client
    usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = CLIENT_NUM_EVENT_MSG,
        .async = {
            .client_event_callback = multiconf_client_event_cb,
            .callback_arg = (void *) &multiconf_obj,
        },
    };
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_register(&client_config, &multiconf_obj.client_hdl));
    s_multiconf_obj = &multiconf_obj;

    // Wait to be started by main thread
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    ESP_LOGD(MULTICONF_CLIENT_TAG, "Starting");

    bool exit_loop = false;
    bool skip_event_handling = false;
    while (!exit_loop) {
        if (!skip_event_handling) {
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_handle_events(multiconf_obj.client_hdl, portMAX_DELAY));
        }
        skip_event_handling = false;
        if (multiconf_obj.cur_stage == multiconf_obj.next_stage) {
            continue;
        }
        multiconf_obj.cur_stage = multiconf_obj.next_stage;

        switch (multiconf_obj.next_stage) {
        case TEST_STAGE_DEV_OPEN: {
            ESP_LOGD(MULTICONF_CLIENT_TAG, "Open");
            // Open the device
            TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, usb_host_device_open(multiconf_obj.client_hdl, multiconf_obj.dev_addr, &multiconf_obj.dev_hdl), "Failed to open the device");

            // Get device info to get it's speed
            usb_device_info_t dev_info;
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_info(multiconf_obj.dev_hdl, &dev_info));
            multiconf_obj.dev_speed = dev_info.speed;

            multiconf_obj.next_stage = TEST_STAGE_WAIT;
            skip_event_handling = true;
            break;
        }
        case TEST_STAGE_WAIT: {
            // Give semaphore signalizing that the device has been opened
            xSemaphoreGive(multiconf_obj.test_param.dev_open_smp);
            break;
        }
        case TEST_STAGE_CHECK_CONFIG_DESC: {
            ESP_LOGD(MULTICONF_CLIENT_TAG, "Check config descriptors");
            // Get mocked config descriptor
            const usb_config_desc_t *config_desc_ref = dev_msc_get_config_desc(multiconf_obj.dev_speed);
            TEST_ASSERT_EQUAL_MESSAGE(multiconf_obj.config_desc_cached->wTotalLength, config_desc_ref->wTotalLength, "Incorrect length of CFG descriptor");
            TEST_ASSERT_EQUAL_MEMORY_MESSAGE(config_desc_ref, multiconf_obj.config_desc_cached, sizeof(usb_config_desc_t), "Configuration descriptors do not match");

            // Free the memory used to store the config descriptor
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_free_config_desc(multiconf_obj.config_desc_cached));
            multiconf_obj.next_stage = TEST_STAGE_DEV_CLOSE;
            skip_event_handling = true;
            break;
        }
        case TEST_STAGE_DEV_CLOSE: {
            ESP_LOGD(MULTICONF_CLIENT_TAG, "Close");
            vTaskDelay(10); // Give USB Host Lib some time to process all transfers
            TEST_ASSERT_EQUAL(ESP_OK, usb_host_device_close(multiconf_obj.client_hdl, multiconf_obj.dev_hdl));
            exit_loop = true;
            break;
        }
        default:
            abort();
            break;
        }
    }
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_client_deregister(multiconf_obj.client_hdl));
    ESP_LOGD(MULTICONF_CLIENT_TAG, "Done");
    vTaskDelete(NULL);
}

void multiconf_client_get_conf_desc(void)
{
    // Get configuration descriptor, ctrl transfer is sent to the device to get the config descriptor
    TEST_ASSERT_EQUAL(ESP_OK, usb_host_get_config_desc(s_multiconf_obj->client_hdl, s_multiconf_obj->dev_hdl, s_multiconf_obj->test_param.bConfigurationValue, &s_multiconf_obj->config_desc_cached));

    // Go to next stage
    s_multiconf_obj->next_stage = TEST_STAGE_CHECK_CONFIG_DESC;
    ESP_ERROR_CHECK(usb_host_client_unblock(s_multiconf_obj->client_hdl));
}
