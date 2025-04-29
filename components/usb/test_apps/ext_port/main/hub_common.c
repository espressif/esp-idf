/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "string.h"
// Test common
#include "hcd_common.h"
#include "hub_common.h"
// USB Host Lib
#include "usb_private.h"
#include "usb/usb_types_ch9.h"
#include "usb/usb_types_ch11.h"
#include "unity.h"

typedef struct {
    hcd_port_handle_t root_port_hdl;
    hcd_pipe_handle_t ctrl_pipe_hdl;
    urb_t *urb;
    usb_speed_t speed;
    uint8_t addr;
    usb_hub_descriptor_t desc;
} hub_dev_t;

static hub_dev_t hub;

// -------------------------------------------------------------------------------------------------
// --------------------------------------- Internal logic  -----------------------------------------
// -------------------------------------------------------------------------------------------------
/**
 * @brief Get Port status
 *
 * @param[in] port1 Port number
 * @param[out] status Port status
 */
static void hub_class_request_get_port_status(uint8_t port1, usb_port_status_t *status)
{
    usb_port_status_t *port_status = (usb_port_status_t *)(hub.urb->transfer.data_buffer + sizeof(usb_setup_packet_t));
    usb_setup_packet_t *setup_pkt = (usb_setup_packet_t *)hub.urb->transfer.data_buffer;
    USB_SETUP_PACKET_INIT_GET_PORT_STATUS(setup_pkt, port1);
    hub.urb->transfer.num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_port_status_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(hub.ctrl_pipe_hdl, hub.urb));
    test_hcd_expect_pipe_event(hub.ctrl_pipe_hdl, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(hub.urb, hcd_urb_dequeue(hub.ctrl_pipe_hdl));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, hub.urb->transfer.status, "Get Port Status: Transfer NOT completed");
    if (status) {
        memcpy(status, port_status, sizeof(usb_port_status_t));
    }
}

/**
 * @brief Set Port Feature
 *
 * @param[in] port1 Port number
 * @param[in] feature Feature to set
 */
static void hub_class_request_set_port_feature(uint8_t port1, usb_hub_port_feature_t feature)
{
    usb_setup_packet_t *setup_pkt = (usb_setup_packet_t *)hub.urb->transfer.data_buffer;
    USB_SETUP_PACKET_INIT_SET_PORT_FEATURE(setup_pkt, port1, feature);
    hub.urb->transfer.num_bytes = sizeof(usb_setup_packet_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(hub.ctrl_pipe_hdl, hub.urb));
    test_hcd_expect_pipe_event(hub.ctrl_pipe_hdl, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(hub.urb, hcd_urb_dequeue(hub.ctrl_pipe_hdl));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, hub.urb->transfer.status, "Set Port Feature: Transfer NOT completed");
}

/**
 * @brief Clear Port Feature
 *
 * @param[in] port1 Port number
 * @param[in] feature Feature to clear
 */
static void hub_class_request_clear_port_feature(uint8_t port1, usb_hub_port_feature_t feature)
{
    usb_setup_packet_t *setup_pkt = (usb_setup_packet_t *)hub.urb->transfer.data_buffer;
    USB_SETUP_PACKET_INIT_CLEAR_PORT_FEATURE(setup_pkt, port1, feature);
    hub.urb->transfer.num_bytes = sizeof(usb_setup_packet_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(hub.ctrl_pipe_hdl, hub.urb));
    test_hcd_expect_pipe_event(hub.ctrl_pipe_hdl, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(hub.urb, hcd_urb_dequeue(hub.ctrl_pipe_hdl));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, hub.urb->transfer.status, "Clear Port Feature: Transfer NOT completed");
}

/**
 * @brief Get Hub Descriptor
 *
 * Get Hub Descriptor and store it in the hub structure
 */
static void hub_get_descriptor(void)
{
    usb_setup_packet_t *setup_pkt = (usb_setup_packet_t *)hub.urb->transfer.data_buffer;
    USB_SETUP_PACKET_INIT_GET_HUB_DESCRIPTOR(setup_pkt);
    hub.urb->transfer.num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_hub_descriptor_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(hub.ctrl_pipe_hdl, hub.urb));
    test_hcd_expect_pipe_event(hub.ctrl_pipe_hdl, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(hub.urb, hcd_urb_dequeue(hub.ctrl_pipe_hdl));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, hub.urb->transfer.status, "Get Hub Descriptor: Transfer NOT completed");

    // Extract Hub capabilities
    const usb_hub_descriptor_t *hub_desc = (const usb_hub_descriptor_t *)(hub.urb->transfer.data_buffer + sizeof(usb_setup_packet_t));
    memcpy(&hub.desc, hub_desc, sizeof(usb_hub_descriptor_t));
}

// -------------------------------------------------------------------------------------------------
// ----------------------------------------- Hub Functions -----------------------------------------
// -------------------------------------------------------------------------------------------------

void hub_set_root_port(hcd_port_handle_t root_port_hdl)
{
    hub.root_port_hdl = root_port_hdl;
}

void hub_attach(void)
{
    // Attach the root port
    hub.speed = test_hcd_wait_for_conn(hub.root_port_hdl);
    // Short delay send of SOF (for FS) or EOPs (for LS)
    vTaskDelay(pdMS_TO_TICKS(100));
    // Create Control Pipe
    hub.ctrl_pipe_hdl = test_hcd_pipe_alloc(hub.root_port_hdl, NULL, 0, hub.speed);
    TEST_ASSERT_NOT_NULL(hub.ctrl_pipe_hdl);
    // Enumerate Hub
    hub.addr = test_hcd_enum_device(hub.ctrl_pipe_hdl);
    // Hub should have an address
    TEST_ASSERT_NOT_EQUAL(0, hub.addr);
    // Create urb for CTRL transfers
    urb_t *urb = test_hcd_alloc_urb(0, sizeof(usb_setup_packet_t) + 256);
    TEST_ASSERT_NOT_NULL(urb);
    hub.urb = urb;
    // Get Hub Descriptor
    hub_get_descriptor();
    printf("\tHub has %d ports\n", hub.desc.bNbrPorts);
}

hcd_pipe_handle_t hub_child_create_pipe(usb_speed_t speed)
{
    // Downstream pipe has the same root port handle
    hcd_pipe_handle_t ctrl_pipe = test_hcd_pipe_alloc(hub.root_port_hdl, NULL, 0, speed);
    TEST_ASSERT_NOT_NULL(ctrl_pipe);
    return ctrl_pipe;
}

void hub_child_pipe_free(hcd_pipe_handle_t pipe_hdl)
{
    TEST_ASSERT_NOT_NULL(pipe_hdl);
    test_hcd_pipe_free(pipe_hdl);
}

void hub_child_quick_enum(hcd_pipe_handle_t ctrl_pipe, uint8_t dev_addr, uint8_t config_num)
{
    // TODO: test_hcd_enum_device() could be used instead, but it requires refactoring: assign the configurable dev address
    usb_setup_packet_t *setup_pkt = (usb_setup_packet_t *)hub.urb->transfer.data_buffer;
    // Get the device descriptor (note that device might only return 8 bytes)
    USB_SETUP_PACKET_INIT_GET_DEVICE_DESC(setup_pkt);
    hub.urb->transfer.num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_device_desc_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(ctrl_pipe, hub.urb));
    test_hcd_expect_pipe_event(ctrl_pipe, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(hub.urb, hcd_urb_dequeue(ctrl_pipe));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, hub.urb->transfer.status, "Transfer NOT completed");

    // Update the MPS of the default pipe
    usb_device_desc_t *device_desc = (usb_device_desc_t *)(hub.urb->transfer.data_buffer + sizeof(usb_setup_packet_t));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_update_mps(ctrl_pipe, device_desc->bMaxPacketSize0));
    //
    printf("\t\tEnumerated device: %04x:%04x, mps: %d\n",
           device_desc->idVendor,
           device_desc->idProduct,
           device_desc->bMaxPacketSize0);

    // Send a set address request
    USB_SETUP_PACKET_INIT_SET_ADDR(setup_pkt, dev_addr);
    hub.urb->transfer.num_bytes = sizeof(usb_setup_packet_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(ctrl_pipe, hub.urb));
    test_hcd_expect_pipe_event(ctrl_pipe, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(hub.urb, hcd_urb_dequeue(ctrl_pipe));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, hub.urb->transfer.status, "Transfer NOT completed");

    // Update address of default pipe
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_update_dev_addr(ctrl_pipe, dev_addr));

    // Send a set configuration request
    USB_SETUP_PACKET_INIT_SET_CONFIG(setup_pkt, config_num);
    hub.urb->transfer.num_bytes = sizeof(usb_setup_packet_t);
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(ctrl_pipe, hub.urb));
    test_hcd_expect_pipe_event(ctrl_pipe, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(hub.urb, hcd_urb_dequeue(ctrl_pipe));
    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, hub.urb->transfer.status, "Transfer NOT completed");
}

void hub_detach(void)
{
    // Free urb
    test_hcd_free_urb(hub.urb);
    // Free Control Pipe
    test_hcd_pipe_free(hub.ctrl_pipe_hdl);
    // Detach the root port
    test_hcd_wait_for_disconn(hub.root_port_hdl, true);
}

void* hub_get_context(void)
{
    return (void*) &hub;
}

uint8_t hub_get_port_num(void)
{
    return hub.desc.bNbrPorts;
}

uint16_t hub_get_port_poweron_delay_ms(void)
{
    return hub.desc.bPwrOn2PwrGood * 2;
}

void hub_get_port_status(uint8_t port1, usb_port_status_t *status)
{
    TEST_ASSERT_NOT_NULL(status);
    hub_class_request_get_port_status(port1, status);
    printf("\t\tport%d %04X.%04X\n", port1,
           status->wPortStatus.val,
           status->wPortChange.val);
}

bool hub_get_port_connection(uint8_t port1)
{
    usb_port_status_t port_status;
    hub_class_request_get_port_status(port1, &port_status);
    return port_status.wPortStatus.PORT_CONNECTION;
}

void hub_port_power_off(uint8_t port1)
{
    hub_class_request_clear_port_feature(port1, USB_FEATURE_PORT_POWER);
}

void hub_port_power_on(uint8_t port1)
{
    hub_class_request_set_port_feature(port1, USB_FEATURE_PORT_POWER);
    // Wait while port powered, crucial for High-speed ports
    vTaskDelay(pdMS_TO_TICKS(hub.desc.bPwrOn2PwrGood));
}

void hub_port_reset(uint8_t port1)
{
    hub_class_request_set_port_feature(port1, USB_FEATURE_PORT_RESET);
}

void hub_port_suspend(uint8_t port1)
{
    hub_class_request_set_port_feature(port1, USB_FEATURE_PORT_SUSPEND);
}

void hub_port_resume(uint8_t port1)
{
    hub_class_request_clear_port_feature(port1, USB_FEATURE_C_PORT_SUSPEND);
}

void hub_port_disable(uint8_t port1)
{
    hub_class_request_clear_port_feature(port1, USB_FEATURE_PORT_ENABLE);
}

void hub_port_clear_connection(uint8_t port1)
{
    hub_class_request_clear_port_feature(port1, USB_FEATURE_C_PORT_CONNECTION);
}

void hub_port_clear_reset(uint8_t port1)
{
    hub_class_request_clear_port_feature(port1, USB_FEATURE_C_PORT_RESET);
}
