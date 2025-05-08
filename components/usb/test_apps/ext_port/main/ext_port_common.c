/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
// USB Host Lib
#include "ext_hub.h" // For port driver API, remove when IDF-12562 is closed
#include "ext_port.h"
#include "usb_private.h"
#include "usb/usb_types_ch9.h"
#include "usb/usb_types_ch11.h"
#include "unity.h"
#include "hub_common.h"
#include "ext_port_common.h"

#define TEST_EXT_PORT_QUEUE_LEN             3
#define EXT_PORT_PROC_CB_TIMEOUT_MS         50
#define EXT_PORT_HUB_REQUEST_TIMEOUT_MS     100
#define EXT_PORT_EVENT_TIMEOUT_MS           100

#define USB_B_REQUEST_ANY                   0xFF

const char *const hub_request_string[] = {
    "Get Port Status",
    "Clear Port Feature",
    "Get Hub State",
    "Set Feature",
    "RESERVED",
    "RESERVED",
    "Get Hub Descriptor",
    "Set Hub Descriptor",
    "Clear TT Buffer",
    "Reset TT",
    "Get TT State",
    "Stop TT",
};

const char *const hub_feature_string[] = {
    "Port Connection",
    "Port Enable",
    "Port Suspend",
    "Port Over Current",
    "Port Reset",
    "Feature N/A",
    "Feature N/A",
    "Feature N/A",
    "Port Power",
    "Port Low Speed",
    "Feature N/A",
    "Feature N/A",
    "Feature N/A",
    "Feature N/A",
    "Feature N/A",
    "Feature N/A",
    "C Port Connection",
    "C Port Enable",
    "C Port Suspend",
    "C Port Over Current",
    "C Port Reset",
    "Port Test",
    "Port Indicator",
};

const char *const ext_port_event_string[] = {
    "N/A",
    "<- Connected",
    "<> Reset Completed",
    "-> Disconnected",
};

typedef struct {
    ext_port_hdl_t port_hdl;
    ext_port_parent_request_data_t data;
} ext_port_hub_request_msg_t;

static const ext_port_driver_api_t* port_api;
SemaphoreHandle_t _process_cd_req = NULL;
QueueHandle_t _ext_port_hub_req_queue = NULL;
QueueHandle_t _ext_port_event_queue = NULL;

static void test_ext_port_callback(void *user_arg)
{
    // We can call ext_port_process() right here, but use semaphore instead to have more linear logic in the test
    SemaphoreHandle_t process_req_cb = (SemaphoreHandle_t)user_arg;
    xSemaphoreGive(process_req_cb);
}

static void test_ext_port_event_callback(ext_port_hdl_t port_hdl, ext_port_event_data_t *event_data, void *arg)
{
    QueueHandle_t ext_port_event_queue = (QueueHandle_t)arg;
    ext_port_event_data_t msg = *event_data;
    xQueueSend(ext_port_event_queue, &msg, portMAX_DELAY);
}

static esp_err_t test_ext_port_hub_request(ext_port_hdl_t port_hdl, ext_port_parent_request_data_t *data, void *user_arg)
{
    QueueHandle_t hub_req_queue = (QueueHandle_t)user_arg;
    ext_port_hub_request_msg_t msg = {
        .port_hdl = port_hdl,
        .data = {
            .type = data->type,
            .control = {
                .req = data->control.req,
                .feature = data->control.feature,
            },
        },
    };
    xQueueSend(hub_req_queue, &msg, portMAX_DELAY);
    return ESP_OK;
}

static void test_wait_ext_port_process_request(void)
{
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(_process_cd_req, pdMS_TO_TICKS(EXT_PORT_PROC_CB_TIMEOUT_MS)));
}

static void test_wait_ext_port_event(ext_port_event_t event)
{
    // Get the port event queue from the port's context variable
    QueueHandle_t ext_port_evt_queue = _ext_port_event_queue;
    TEST_ASSERT_NOT_NULL(ext_port_evt_queue);
    // Wait for port callback to send an event message
    ext_port_event_data_t msg;
    BaseType_t ret = xQueueReceive(ext_port_evt_queue, &msg, pdMS_TO_TICKS(EXT_PORT_EVENT_TIMEOUT_MS));
    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, ret, "External Hub port event not generated on time");
    // Check the contents of that event message
    printf("\tHub port event: %s\n", ext_port_event_string[msg.event]);
    TEST_ASSERT_EQUAL_MESSAGE(event, msg.event, "Unexpected External Hub port event");
}

static esp_err_t test_wait_ext_port_hub_request(ext_port_hdl_t port_hdl, ext_port_parent_request_type_t type, usb_hub_class_request_t request)
{
    // Get the hub request queue
    TEST_ASSERT_NOT_NULL(_ext_port_hub_req_queue);
    // Wait for port callback to send an event message
    ext_port_hub_request_msg_t msg;
    BaseType_t ret = xQueueReceive(_ext_port_hub_req_queue, &msg, pdMS_TO_TICKS(EXT_PORT_HUB_REQUEST_TIMEOUT_MS));
    if (ret != pdPASS) {
        printf("Hub request callback not triggered on time \n");
        return ESP_ERR_TIMEOUT;
    }
    // Check the contents of that event message
    // TEST_ASSERT_EQUAL(port_hdl, msg.port_hdl); // TODO: Enable verification after closing IDF-10023
    TEST_ASSERT_EQUAL(type, msg.data.type);
    switch (type) {
    case EXT_PORT_PARENT_REQ_CONTROL:
        printf("\tCallback for class request: %s %s\n", hub_request_string[msg.data.control.req],
               (msg.data.control.req == USB_B_REQUEST_HUB_SET_PORT_FEATURE ||
                msg.data.control.req == USB_B_REQUEST_HUB_CLEAR_FEATURE)
               ? hub_feature_string[msg.data.control.feature]
               : " ");
        // Verify the request if it is not USB_B_REQUEST_ANY
        if (msg.data.control.req != USB_B_REQUEST_ANY) {
            TEST_ASSERT_EQUAL_MESSAGE(request, msg.data.control.req, "Unexpected request");
        }
        break;

    default:
        break;

    }
    return ESP_OK;
}

void test_ext_port_setup(void)
{
    // Create a semaphore to wait for the process request
    _process_cd_req = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(_process_cd_req);
    // Create a queue for ext port hub request
    _ext_port_hub_req_queue = xQueueCreate(TEST_EXT_PORT_QUEUE_LEN, sizeof(ext_port_hub_request_msg_t));
    TEST_ASSERT_NOT_NULL(_ext_port_hub_req_queue);
    // Create a queue for ext port event
    _ext_port_event_queue = xQueueCreate(TEST_EXT_PORT_QUEUE_LEN, sizeof(ext_port_event_data_t));
    TEST_ASSERT_NOT_NULL(_ext_port_event_queue);
    // Install External Port driver
    ext_port_driver_config_t ext_port_config = {
        .proc_req_cb = test_ext_port_callback,
        .proc_req_cb_arg = (void*)_process_cd_req,
        .event_cb = test_ext_port_event_callback,
        .event_cb_arg = (void*)_ext_port_event_queue,
        .hub_request_cb = test_ext_port_hub_request,
        .hub_request_cb_arg = (void*)_ext_port_hub_req_queue,
    };
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_install(&ext_port_config));
    port_api = ext_port_get_driver();
    TEST_ASSERT_NOT_NULL(port_api);
}

void test_ext_port_teardown(void)
{
    // Uninstall External Port driver
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_uninstall());
    vSemaphoreDelete(_process_cd_req);
    vQueueDelete(_ext_port_hub_req_queue);
    vQueueDelete(_ext_port_event_queue);
}

ext_port_hdl_t test_ext_port_new(ext_port_config_t *config)
{
    ext_port_hdl_t port = NULL;
    TEST_ASSERT_EQUAL(ESP_OK, port_api->new (config, (void**) &port));
    TEST_ASSERT_NOT_NULL(port);
    // Adding the port should trigger the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Newly added port should trigger the Get Port Status request on the parent hub
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_GET_PORT_STATUS));    // Process the port
    return port;
}

void test_ext_port_power_on(uint8_t port1, ext_port_hdl_t port_hdl)
{
    usb_port_status_t port_status;
    hub_get_port_status(port1, &port_status);
    // Verify actual port status
    TEST_ASSERT_EQUAL(0, port_status.wPortStatus.val);          // Port should be not powered
    TEST_ASSERT_EQUAL(0, port_status.wPortChange.val);          // No port Change

    // Set port status to the External Port Driver
    TEST_ASSERT_EQUAL(ESP_OK, port_api->set_status(port_hdl, &port_status));
    // Setting the port status should trigger the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Wait Set Feature - Port Power
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_SET_PORT_FEATURE));
    // Power on the port
    hub_port_power_on(port1);
    vTaskDelay(pdMS_TO_TICKS(30)); // TODO: change to power power on delay
    // After powering the port, trigger the port processing
    TEST_ASSERT_EQUAL(ESP_OK, port_api->req_process(port_hdl));
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Wait Get Port Status
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_GET_PORT_STATUS));
    // Get Port Status
    hub_get_port_status(port1, &port_status);
    // Verify the actual port status
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_POWER); // Port should be powered
}

void test_ext_port_disconnected(uint8_t port1, ext_port_hdl_t port_hdl)
{
    usb_port_status_t port_status;

    // Get Actual Port Status
    hub_get_port_status(port1, &port_status);
    // Port should be powered, has no connection and no port change
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_POWER);
    TEST_ASSERT_EQUAL(0, port_status.wPortStatus.PORT_CONNECTION);
    TEST_ASSERT_EQUAL(0, port_status.wPortChange.val);

    // Set port status to the External Port Driver
    TEST_ASSERT_EQUAL(ESP_OK, port_api->set_status(port_hdl, &port_status));
    // Setting the port status should trigger the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Disconnected port doesn't require any processing, so there should be no process request
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl, EXT_PORT_PARENT_REQ_PROC_COMPLETED, 0));
    // Port is idle
}

usb_speed_t test_ext_port_connected(uint8_t port1, ext_port_hdl_t port_hdl)
{
    usb_port_status_t port_status;
    // Get the port status
    hub_get_port_status(port1, &port_status);
    // Port should be powered, has a connection and a port change
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_POWER);
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_CONNECTION);
    TEST_ASSERT_EQUAL(1, port_status.wPortChange.C_PORT_CONNECTION);

    // Set port status to the External Port Driver
    TEST_ASSERT_EQUAL(ESP_OK, port_api->set_status(port_hdl, &port_status));
    // Setting the port status should trigger the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Setting the status of the port with C_CONNECTION should trigger the Clear Port Connection
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_CLEAR_FEATURE));

    // Clear Port Connection
    hub_port_clear_connection(port1);
    // After clearing connection, trigger the port processing
    TEST_ASSERT_EQUAL(ESP_OK, port_api->req_process(port_hdl));
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Powered port with connection should trigger the status update
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_GET_PORT_STATUS));
    // Get Port Status
    hub_get_port_status(port1, &port_status);
    // Port should be powered, has a connection and no port change
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_POWER);          // Powered
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_CONNECTION);     // Connection detected
    TEST_ASSERT_EQUAL(0, port_status.wPortChange.C_PORT_CONNECTION);   // Port does not need to clear connection
    // Set port status to the External Port Driver
    TEST_ASSERT_EQUAL(ESP_OK, port_api->set_status(port_hdl, &port_status));
    // Wait for the process request callback
    test_wait_ext_port_process_request();
    // Setting the port status should trigger the process request callback
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Powered port with connection should trigger the Port Reset request on the parent hub
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_SET_PORT_FEATURE));
    // Port Reset
    hub_port_reset(port1);
    // Give port some time to process the reset
    vTaskDelay(pdMS_TO_TICKS(30)); // TODO: Port Reset recovery
    // After resetting the port, trigger the port processing
    TEST_ASSERT_EQUAL(ESP_OK, port_api->req_process(port_hdl));
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Powered port with connection should trigger the Get Port Status request on the parent hub
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_GET_PORT_STATUS));
    // Get Port Status
    hub_get_port_status(port1, &port_status);
    // Port should be powered, has a connection and requires clear a port reset
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_POWER);          // Powered
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_CONNECTION);     // Connection detected
    TEST_ASSERT_EQUAL(0, port_status.wPortChange.C_PORT_CONNECTION);   // Port does not need to clear connection
    TEST_ASSERT_EQUAL(1, port_status.wPortChange.C_PORT_RESET);        // Port does need to clear reset
    // Set port status
    TEST_ASSERT_EQUAL(ESP_OK, port_api->set_status(port_hdl, &port_status));
    // Wait for the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Powered port with connection after reset should trigger the Clear Port Reset request on the parent hub
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_CLEAR_FEATURE));
    // Clear Port reset request
    hub_port_clear_reset(port1);
    // After clearing reset, trigger the port processing
    TEST_ASSERT_EQUAL(ESP_OK, port_api->req_process(port_hdl));
    // Wait for the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Powered port with connection after reset should trigger the Get Port Status request on the parent hub
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_GET_PORT_STATUS));

    // Get Port Status
    hub_get_port_status(port1, &port_status);

    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_POWER);          // Powered
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_CONNECTION);     // Connection detected
    TEST_ASSERT_EQUAL(0, port_status.wPortChange.C_PORT_CONNECTION);   // Port does not need to clear connection
    TEST_ASSERT_EQUAL(0, port_status.wPortChange.C_PORT_RESET);        // Port does not need to clear reset
    // Set port status to the External Port Driver
    TEST_ASSERT_EQUAL(ESP_OK, port_api->set_status(port_hdl, &port_status));
    // Wait for the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Processing the port after clear reset should generate the connection event
    test_wait_ext_port_event(EXT_PORT_CONNECTED);
    // Get device speed
    usb_speed_t dev_speed;
    TEST_ASSERT_EQUAL(ESP_OK, port_api->get_speed(port_hdl, &dev_speed));
    return dev_speed;
}

void test_ext_port_imitate_disconnection(uint8_t port1, ext_port_hdl_t port_hdl)
{
    usb_port_status_t port_status;
    // Get Port Status
    hub_get_port_status(port1, &port_status);
    // Imitate disconnection possible only on the port, that has device and enabled
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_POWER);       // Port powered
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_ENABLE);      // Port enabled
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_CONNECTION);  // Port has connection
    TEST_ASSERT_EQUAL(0, port_status.wPortChange.val);              // No change to handle

    //
    // EMULATE DISCONNECTION VIA PORT STATUS
    //
    // Change the port status: reset enable, reset the connection and set the clear connection bit
    port_status.wPortStatus.PORT_ENABLE = 0;
    port_status.wPortStatus.PORT_CONNECTION = 0;
    port_status.wPortChange.C_PORT_CONNECTION = 1;

    // Set new port status
    printf("set status %04X.%04X*\n", port_status.wPortStatus.val, port_status.wPortChange.val);
    TEST_ASSERT_EQUAL(ESP_OK, port_api->set_status(port_hdl, &port_status));
    // Setting the port status should trigger the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Setting the status of the port with connection should trigger the Clear Port Connection
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_CLEAR_FEATURE));
    // Assume that clear connection done successfully, but we do not need actually change the port status
    // Clear Port Feature: C_CONNECTION - assume we done it, ESP_OK
    port_status.wPortChange.C_PORT_CONNECTION = 0;
    // Request process
    TEST_ASSERT_EQUAL(ESP_OK, port_api->req_process(port_hdl));
    // Wait for the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Cleared port connection should trigger the Get Port Status request on the parent hub
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_GET_PORT_STATUS));

    // We don't need to request actual port status, proceed with cached status

    // Set new port status
    printf("set status %04X.%04X*\n", port_status.wPortStatus.val, port_status.wPortChange.val);
    TEST_ASSERT_EQUAL(ESP_OK, port_api->set_status(port_hdl, &port_status));
    // Setting the port status should trigger the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // The External Port driver should indicate that device has been disconnected and port disconnected
    test_wait_ext_port_event(EXT_PORT_DISCONNECTED);
}

void test_ext_port_disable(uint8_t port1, ext_port_hdl_t port_hdl)
{
    usb_port_status_t port_status;
    hub_get_port_status(port1, &port_status);

    // Disable port is possible only when it is enabled, powered and has a connection
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_POWER);       // Port powered
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_ENABLE);      // Port enabled
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_CONNECTION);  // Port has connection
    TEST_ASSERT_EQUAL(0, port_status.wPortChange.val);              // No change to handle

    TEST_ASSERT_EQUAL(ESP_OK, port_api->disable(port_hdl));
    // Wait for the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_CLEAR_FEATURE));
    // When port is not gone, it should create a DISCONNECTION event
    test_wait_ext_port_event(EXT_PORT_DISCONNECTED);
    // Disable the port
    hub_port_disable(port1);
    // After completion, trigger the port processing
    TEST_ASSERT_EQUAL(ESP_OK, port_api->req_process(port_hdl));
    // Wait for the process request callback
    test_wait_ext_port_process_request();
    // Process the port
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Cleared port enable should trigger the Get Port Status request on the parent hub
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl,
                                                             EXT_PORT_PARENT_REQ_CONTROL,
                                                             USB_B_REQUEST_HUB_GET_PORT_STATUS));
    // Get Port Status
    hub_get_port_status(port1, &port_status);
    // Port should be powered, disabled and have a connection
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_POWER);          // Powered
    TEST_ASSERT_EQUAL(0, port_status.wPortStatus.PORT_ENABLE);         // Disabled
    TEST_ASSERT_EQUAL(1, port_status.wPortStatus.PORT_CONNECTION);     // Connection detected
    // Set port status
    TEST_ASSERT_EQUAL(ESP_OK, port_api->set_status(port_hdl, &port_status));
    // Wait for the process request callback
    test_wait_ext_port_process_request();
    // Port doesn't require any processing, so there should debug message "No more ports to handle"
    TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
    // Disabled port doesn't require any processing, handling should be completed
    TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl, EXT_PORT_PARENT_REQ_PROC_COMPLETED, 0));
    // Port is disabled and wait device to be detached.
    // Terminal state
}

void test_ext_port_gone(ext_port_hdl_t port_hdl, test_gone_flag_t flag)
{
    if (flag == GONE_DEVICE_PRESENT) {
        // Port is enabled, port gone returns ESP_ERR_NOT_FINISHED
        TEST_ASSERT_EQUAL(ESP_ERR_NOT_FINISHED, port_api->gone(port_hdl));
        // Mark the connected port as gone should trigger disconnect event if the port has a device
        test_wait_ext_port_event(EXT_PORT_DISCONNECTED);
        return;
    }
    // Port doesn't have a device, port gone returns ESP_OK
    TEST_ASSERT_EQUAL(ESP_OK, port_api->gone(port_hdl));
}

void test_ext_port_delete(ext_port_hdl_t port_hdl)
{
    TEST_ASSERT_EQUAL(ESP_OK, port_api->del(port_hdl));
}

void test_ext_port_recycle(ext_port_hdl_t port_hdl, test_recycle_flag_t flag)
{
    switch (flag) {
    case RECYCLE_PORT_IS_GONE:
        // Recycling in not available for the gone port
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, port_api->recycle(port_hdl));
        break;
    case RECYCLE_PORT_PRESENT:
        TEST_ASSERT_EQUAL(ESP_OK, port_api->recycle(port_hdl));
        // Recycling the port for the process request callback
        test_wait_ext_port_process_request();
        // Process the port
        TEST_ASSERT_EQUAL(ESP_OK, ext_port_process());
        TEST_ASSERT_EQUAL(ESP_OK, test_wait_ext_port_hub_request(port_hdl, EXT_PORT_PARENT_REQ_PROC_COMPLETED, 0));
        break;
    default:
        TEST_ASSERT(0);     // Invalid flag
        break;
    }
}
