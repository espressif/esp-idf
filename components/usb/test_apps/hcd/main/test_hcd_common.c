/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "soc/usb_wrap_struct.h"
#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_rom_gpio.h"
#include "hcd.h"
#include "usb_private.h"
#include "usb/usb_types_ch9.h"
#include "test_hcd_common.h"
#include "test_usb_common.h"
#include "unity.h"

#define PORT_NUM                1
#define EVENT_QUEUE_LEN         5
#define ENUM_ADDR               1   //Device address to use for tests that enumerate the device
#define ENUM_CONFIG             1   //Device configuration number to use for tests that enumerate the device

typedef struct {
    hcd_port_handle_t port_hdl;
    hcd_port_event_t port_event;
} port_event_msg_t;

typedef struct {
    hcd_pipe_handle_t pipe_hdl;
    hcd_pipe_event_t pipe_event;
} pipe_event_msg_t;

hcd_port_handle_t port_hdl = NULL;

// ---------------------------------------------------- Private --------------------------------------------------------

/**
 * @brief HCD port callback. Registered when initializing an HCD port
 *
 * @param port_hdl Port handle
 * @param port_event Port event that triggered the callback
 * @param user_arg User argument
 * @param in_isr Whether callback was called in an ISR context
 * @return true ISR should yield after this callback returns
 * @return false No yield required (non-ISR context calls should always return false)
 */
static bool port_callback(hcd_port_handle_t port_hdl, hcd_port_event_t port_event, void *user_arg, bool in_isr)
{
    //We store the port's queue handle in the port's context variable
    void *port_ctx = hcd_port_get_context(port_hdl);
    QueueHandle_t port_evt_queue = (QueueHandle_t)port_ctx;
    TEST_ASSERT_TRUE(in_isr);    //Current HCD implementation should never call a port callback in a task context
    port_event_msg_t msg = {
        .port_hdl = port_hdl,
        .port_event = port_event,
    };
    BaseType_t xTaskWoken = pdFALSE;
    xQueueSendFromISR(port_evt_queue, &msg, &xTaskWoken);
    return (xTaskWoken == pdTRUE);
}

/**
 * @brief HCD pipe callback. Registered when allocating a HCD pipe
 *
 * @param pipe_hdl Pipe handle
 * @param pipe_event Pipe event that triggered the callback
 * @param user_arg User argument
 * @param in_isr Whether the callback was called in an ISR context
 * @return true ISR should yield after this callback returns
 * @return false No yield required (non-ISR context calls should always return false)
 */
static bool pipe_callback(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr)
{
    QueueHandle_t pipe_evt_queue = (QueueHandle_t)user_arg;
    pipe_event_msg_t msg = {
        .pipe_hdl = pipe_hdl,
        .pipe_event = pipe_event,
    };
    if (in_isr) {
        BaseType_t xTaskWoken = pdFALSE;
        xQueueSendFromISR(pipe_evt_queue, &msg, &xTaskWoken);
        return (xTaskWoken == pdTRUE);
    } else {
        xQueueSend(pipe_evt_queue, &msg, portMAX_DELAY);
        return false;
    }
}

// ------------------------------------------------- HCD Event Test ----------------------------------------------------

void test_hcd_expect_port_event(hcd_port_handle_t port_hdl, hcd_port_event_t expected_event)
{
    //Get the port event queue from the port's context variable
    QueueHandle_t port_evt_queue = (QueueHandle_t)hcd_port_get_context(port_hdl);
    TEST_ASSERT_NOT_NULL(port_evt_queue);
    //Wait for port callback to send an event message
    port_event_msg_t msg;
    BaseType_t ret = xQueueReceive(port_evt_queue, &msg, pdMS_TO_TICKS(5000));
    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, ret, "Port event not generated on time");
    //Check the contents of that event message
    TEST_ASSERT_EQUAL(port_hdl, msg.port_hdl);
    TEST_ASSERT_EQUAL_MESSAGE(expected_event, msg.port_event, "Unexpected event");
    printf("\t-> Port event\n");
}

void test_hcd_expect_pipe_event(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t expected_event)
{
    //Get the pipe's event queue from the pipe's context variable
    QueueHandle_t pipe_evt_queue = (QueueHandle_t)hcd_pipe_get_context(pipe_hdl);
    TEST_ASSERT_NOT_NULL(pipe_evt_queue);
    //Wait for pipe callback to send an event message
    pipe_event_msg_t msg;
    BaseType_t ret =  xQueueReceive(pipe_evt_queue, &msg, pdMS_TO_TICKS(5000));
    TEST_ASSERT_EQUAL_MESSAGE(pdPASS, ret, "Pipe event not generated on time");
    //Check the contents of that event message
    TEST_ASSERT_EQUAL(pipe_hdl, msg.pipe_hdl);
    TEST_ASSERT_EQUAL_MESSAGE(expected_event, msg.pipe_event, "Unexpected event");
}

int test_hcd_get_num_port_events(hcd_port_handle_t port_hdl)
{
    //Get the port event queue from the port's context variable
    QueueHandle_t port_evt_queue = (QueueHandle_t)hcd_port_get_context(port_hdl);
    TEST_ASSERT_NOT_NULL(port_evt_queue);
    return EVENT_QUEUE_LEN - uxQueueSpacesAvailable(port_evt_queue);
}

int test_hcd_get_num_pipe_events(hcd_pipe_handle_t pipe_hdl)
{
    //Get the pipe's event queue from the pipe's context variable
    QueueHandle_t pipe_evt_queue = (QueueHandle_t)hcd_pipe_get_context(pipe_hdl);
    TEST_ASSERT_NOT_NULL(pipe_evt_queue);
    return EVENT_QUEUE_LEN - uxQueueSpacesAvailable(pipe_evt_queue);
}

// ----------------------------------------------- Driver/Port Related -------------------------------------------------

hcd_port_handle_t test_hcd_setup(void)
{
    test_usb_init_phy();    //Initialize the internal USB PHY and USB Controller for testing
    //Create a queue for port callback to queue up port events
    QueueHandle_t port_evt_queue = xQueueCreate(EVENT_QUEUE_LEN, sizeof(port_event_msg_t));
    TEST_ASSERT_NOT_NULL(port_evt_queue);
    //Install HCD
    hcd_config_t hcd_config = {
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    TEST_ASSERT_EQUAL(ESP_OK, hcd_install(&hcd_config));
    //Initialize a port
    hcd_port_config_t port_config = {
        .fifo_bias = HCD_PORT_FIFO_BIAS_BALANCED,
        .callback = port_callback,
        .callback_arg = (void *)port_evt_queue,
        .context = (void *)port_evt_queue,
    };
    hcd_port_handle_t port_hdl;
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_init(PORT_NUM, &port_config, &port_hdl));
    TEST_ASSERT_NOT_NULL(port_hdl);
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_NOT_POWERED, hcd_port_get_state(port_hdl));
    test_usb_set_phy_state(false, 0);    //Force disconnected state on PHY
    return port_hdl;
}

void test_hcd_teardown(hcd_port_handle_t port_hdl)
{
    if (!port_hdl) {
        return; // In case of setup stage failure, don't run tear-down stage
    }
    //Get the queue handle from the port's context variable
    QueueHandle_t port_evt_queue = (QueueHandle_t)hcd_port_get_context(port_hdl);
    TEST_ASSERT_NOT_NULL(port_evt_queue);
    //Deinitialize a port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_deinit(port_hdl));
    //Uninstall the HCD
    TEST_ASSERT_EQUAL(ESP_OK, hcd_uninstall());
    vQueueDelete(port_evt_queue);
    test_usb_deinit_phy();  //Deinitialize the internal USB PHY after testing
}

usb_speed_t test_hcd_wait_for_conn(hcd_port_handle_t port_hdl)
{
    //Power ON the port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_POWER_ON));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISCONNECTED, hcd_port_get_state(port_hdl));
    //Wait for connection event
    printf("Waiting for connection\n");
    test_usb_set_phy_state(true, pdMS_TO_TICKS(100));     //Allow for connected state on PHY
    test_hcd_expect_port_event(port_hdl, HCD_PORT_EVENT_CONNECTION);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_CONNECTION, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISABLED, hcd_port_get_state(port_hdl));
    //Reset newly connected device
    printf("Resetting\n");
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_RESET));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_ENABLED, hcd_port_get_state(port_hdl));
    //Get speed of connected
    usb_speed_t port_speed;
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_get_speed(port_hdl, &port_speed));
    if (port_speed == USB_SPEED_FULL) {
        printf("Full speed enabled\n");
    } else {
        printf("Low speed enabled\n");
    }
    return port_speed;
}

void test_hcd_wait_for_disconn(hcd_port_handle_t port_hdl, bool already_disabled)
{
    if (!already_disabled) {
        //Disable the device
        printf("Disabling\n");
        TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_DISABLE));
        TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISABLED, hcd_port_get_state(port_hdl));
    }
    //Wait for a safe disconnect
    printf("Waiting for disconnection\n");
    test_usb_set_phy_state(false, pdMS_TO_TICKS(100));    //Force disconnected state on PHY
    test_hcd_expect_port_event(port_hdl, HCD_PORT_EVENT_DISCONNECTION);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_DISCONNECTION, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_RECOVERY, hcd_port_get_state(port_hdl));
    //Power down the port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_POWER_OFF));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_NOT_POWERED, hcd_port_get_state(port_hdl));
}

// ---------------------------------------------- Pipe Setup/Tear-down -------------------------------------------------

hcd_pipe_handle_t test_hcd_pipe_alloc(hcd_port_handle_t port_hdl, const usb_ep_desc_t *ep_desc, uint8_t dev_addr, usb_speed_t dev_speed)
{
    //Create a queue for pipe callback to queue up pipe events
    QueueHandle_t pipe_evt_queue = xQueueCreate(EVENT_QUEUE_LEN, sizeof(pipe_event_msg_t));
    TEST_ASSERT_NOT_NULL(pipe_evt_queue);
    printf("Creating pipe\n");
    hcd_pipe_config_t pipe_config = {
        .callback = pipe_callback,
        .callback_arg = (void *)pipe_evt_queue,
        .context = (void *)pipe_evt_queue,
        .ep_desc = ep_desc,
        .dev_addr = dev_addr,
        .dev_speed = dev_speed,
    };
    hcd_pipe_handle_t pipe_hdl;
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_alloc(port_hdl, &pipe_config, &pipe_hdl));
    TEST_ASSERT_NOT_NULL(pipe_hdl);
    return pipe_hdl;
}

void test_hcd_pipe_free(hcd_pipe_handle_t pipe_hdl)
{
    //Get the pipe's event queue from its context variable
    QueueHandle_t pipe_evt_queue = (QueueHandle_t)hcd_pipe_get_context(pipe_hdl);
    TEST_ASSERT_NOT_NULL(pipe_evt_queue);
    //Free the pipe and queue
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_free(pipe_hdl));
    vQueueDelete(pipe_evt_queue);
}

urb_t *test_hcd_alloc_urb(int num_isoc_packets, size_t data_buffer_size)
{
    //Allocate a URB and data buffer
    urb_t *urb = heap_caps_calloc(1, sizeof(urb_t) + (num_isoc_packets * sizeof(usb_isoc_packet_desc_t)), MALLOC_CAP_DEFAULT);
    uint8_t *data_buffer = heap_caps_malloc(data_buffer_size, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(urb);
    TEST_ASSERT_NOT_NULL(data_buffer);
    //Initialize URB and underlying transfer structure. Need to cast to dummy due to const fields
    usb_transfer_dummy_t *transfer_dummy = (usb_transfer_dummy_t *)&urb->transfer;
    transfer_dummy->data_buffer = data_buffer;
    transfer_dummy->num_isoc_packets = num_isoc_packets;
    return urb;
}

void test_hcd_free_urb(urb_t *urb)
{
    //Free data buffer of the transfer
    heap_caps_free(urb->transfer.data_buffer);
    //Free the URB
    heap_caps_free(urb);
}

uint8_t test_hcd_enum_device(hcd_pipe_handle_t default_pipe)
{
    //We need to create a URB for the enumeration control transfers
    urb_t *urb = test_hcd_alloc_urb(0, sizeof(usb_setup_packet_t) + 256);
    usb_setup_packet_t *setup_pkt = (usb_setup_packet_t *)urb->transfer.data_buffer;

    //Get the device descriptor (note that device might only return 8 bytes)
    USB_SETUP_PACKET_INIT_GET_DEVICE_DESC(setup_pkt);
    urb->transfer.num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_device_desc_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb));
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(urb, hcd_urb_dequeue(default_pipe));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status, "Transfer NOT completed");

    //Update the MPS of the default pipe
    usb_device_desc_t *device_desc = (usb_device_desc_t *)(urb->transfer.data_buffer + sizeof(usb_setup_packet_t));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_update_mps(default_pipe, device_desc->bMaxPacketSize0));

    //Send a set address request
    USB_SETUP_PACKET_INIT_SET_ADDR(setup_pkt, ENUM_ADDR);    //We only support one device for now so use address 1
    urb->transfer.num_bytes = sizeof(usb_setup_packet_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb));
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(urb, hcd_urb_dequeue(default_pipe));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status, "Transfer NOT completed");

    //Update address of default pipe
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_update_dev_addr(default_pipe, ENUM_ADDR));

    //Send a set configuration request
    USB_SETUP_PACKET_INIT_SET_CONFIG(setup_pkt, ENUM_CONFIG);
    urb->transfer.num_bytes = sizeof(usb_setup_packet_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb));
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(urb, hcd_urb_dequeue(default_pipe));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status, "Transfer NOT completed");

    //Free URB
    test_hcd_free_urb(urb);
    return ENUM_ADDR;
}
