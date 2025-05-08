/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "dev_hid.h"
#include "hcd_common.h"

// --------------------------------------------------- Test Cases ------------------------------------------------------

/*
Test HCD interrupt pipe URBs
Purpose:
    - Test that an interrupt pipe can be created
    - URBs can be created and enqueued to the interrupt pipe
    - Interrupt pipe returns HCD_PIPE_EVENT_URB_DONE
    - Test that URBs can be aborted when enqueued

Procedure:
    - Setup HCD and wait for connection
    - Allocate default pipe and enumerate the device
    - Setup interrupt pipe and allocate URBs
    - Enqueue URBs, expect HCD_PIPE_EVENT_URB_DONE, and requeue
    - Stop after fixed number of iterations
    - Deallocate URBs
    - Teardown

Note: Some mice will NAK until it is moved, so try moving the mouse around if this test case gets stuck.
*/

#define TEST_HID_DEV_SPEED                  USB_SPEED_LOW
#define NUM_URBS                            3
#define NUM_URB_ITERS                       (NUM_URBS * 100)

TEST_CASE("Test HCD interrupt pipe URBs", "[intr][low_speed]")
{
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  // Trigger a connection
    TEST_ASSERT_EQUAL_MESSAGE(TEST_HID_DEV_SPEED, port_speed, "Connected device is not Low Speed!");
    vTaskDelay(pdMS_TO_TICKS(100)); // Short delay send of SOF (for FS) or EOPs (for LS)

    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, 0, port_speed); // Create a default pipe (using a NULL EP descriptor)
    uint8_t dev_addr = test_hcd_enum_device(default_pipe);

    // Allocate interrupt pipe and URBS
    const usb_ep_desc_t *in_ep_desc = dev_hid_get_in_ep_desc(port_speed);
    const int data_buff_size = USB_EP_DESC_GET_MPS(in_ep_desc);
    hcd_pipe_handle_t intr_pipe = test_hcd_pipe_alloc(port_hdl, in_ep_desc, dev_addr, port_speed);
    urb_t *urb_list[NUM_URBS];
    for (int i = 0; i < NUM_URBS; i++) {
        urb_list[i] = test_hcd_alloc_urb(0, data_buff_size);
        urb_list[i]->transfer.num_bytes = data_buff_size;
        urb_list[i]->transfer.context = URB_CONTEXT_VAL;
    }

    // Enqueue URBs
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(intr_pipe, urb_list[i]));
    }
    int iter_count = NUM_URB_ITERS;
    for (iter_count = NUM_URB_ITERS; iter_count > 0; iter_count--) {
        // Wait for an URB to be done
        test_hcd_expect_pipe_event(intr_pipe, HCD_PIPE_EVENT_URB_DONE);
        // Dequeue the URB and check results
        urb_t *urb = hcd_urb_dequeue(intr_pipe);
        TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status, "Transfer NOT completed");
        TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
        // Byte 1 and 2 contains x and y movement respectively
        printf("X mov %d, Y mov %d\n", urb->transfer.data_buffer[1], urb->transfer.data_buffer[2]);
        // Requeue URB
        if (iter_count > NUM_URBS) {
            TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(intr_pipe, urb));
        }
    }

    // Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
    }
    test_hcd_pipe_free(intr_pipe);
    test_hcd_pipe_free(default_pipe);
    // Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
}

/*
Test HCD interrupt pipe allocation, when bInterval = 0
Purpose:
    - Test that an interrupt pipe cause no panic

Procedure:
    - Setup HCD and wait for connection
    - Allocate interrupt pipe with bInterval = 0
    - Expect ESP_ERR_NOT_SUPPORTED
    - Cleanup
*Note: for all speeds (LS, FS, HS)
*/
TEST_CASE("Test HCD interrupt pipe alloc: bInterval=0", "[intr][low_speed][full_speed][high_speed]")
{
    // Trigger a connection
    test_hcd_wait_for_conn(port_hdl);
    // The device speed not needed, we need just the connection to unlock pipe allocation
    vTaskDelay(pdMS_TO_TICKS(100)); // Short delay send of SOF (for FS) or EOPs (for LS)

    usb_ep_desc_t test_ep_intr_desc = {
        .bLength = USB_EP_DESC_SIZE,
        .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = 0x81, // IN endpoint
        .bmAttributes = USB_BM_ATTRIBUTES_XFER_INT,
        .wMaxPacketSize = 8,
        .bInterval = 0, // bInterval = 0
    };

    hcd_pipe_config_t pipe_config = {
        .callback = NULL,               // No callback
        .callback_arg = NULL,           // No callback argument
        .context = NULL,                // No context
        .ep_desc = &test_ep_intr_desc,  // IN endpoint descriptor
        .dev_addr = 1,                  // Any device address
    };

    for (usb_speed_t test_speed = USB_SPEED_LOW; test_speed <= USB_SPEED_HIGH; test_speed++) {
        // Create an interrupt pipe (using a EP descriptor)
        hcd_pipe_handle_t pipe_hdl = NULL;
        pipe_config.dev_speed = test_speed;
        TEST_ASSERT_EQUAL_MESSAGE(ESP_ERR_NOT_SUPPORTED, hcd_pipe_alloc(port_hdl, &pipe_config, &pipe_hdl), "Create pipe with bInterval=0 should fail");
        TEST_ASSERT_NULL_MESSAGE(pipe_hdl, "Pipe handle should be NULL");
    }
    // Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
}

/*
Test HCD interrupt pipe allocation, when bInterval in the interval [0x01..0xFF]
Purpose:
    - Test that an interrupt pipe can be created with bInterval in the interval [0x01..0xFF]
    - Test that an interrupt pipe creation cause no panic with any bInterval

Procedure:
    - Setup HCD and wait for connection
    - Allocate interrupt pipe with bInterval in the interval [0x01..0xFF]
    - Expect ESP_OK
    - Cleanup
*Note: for all speeds (LS, FS, HS)
*/
TEST_CASE("Test HCD interrupt pipe alloc: bInterval [0x1..0xff]", "[intr][low_speed][full_speed][high_speed]")
{
    // Trigger a connection
    test_hcd_wait_for_conn(port_hdl);
    // The device speed not needed, we need just the connection to unlock pipe allocation
    vTaskDelay(pdMS_TO_TICKS(100)); // Short delay send of SOF (for FS) or EOPs (for LS)

    for (int test_bInterval = 1; test_bInterval <= 0xFF; test_bInterval++) {
        usb_ep_desc_t test_ep_intr_desc = {
            .bLength = USB_EP_DESC_SIZE,
            .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
            .bEndpointAddress = 0x81, // IN endpoint
            .bmAttributes = USB_BM_ATTRIBUTES_XFER_INT,
            .wMaxPacketSize = 8,
            .bInterval = test_bInterval,
        };

        hcd_pipe_config_t pipe_config = {
            .callback = NULL,               // No callback
            .callback_arg = NULL,           // No callback argument
            .context = NULL,                // No context
            .ep_desc = &test_ep_intr_desc,  // IN endpoint descriptor
            .dev_addr = 1,                  // Any device address
        };

        for (usb_speed_t test_speed = USB_SPEED_LOW; test_speed <= USB_SPEED_HIGH; test_speed++) {
            // Create an interrupt pipe (using a EP descriptor)
            hcd_pipe_handle_t pipe_hdl = NULL;
            pipe_config.dev_speed = test_speed;
            TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, hcd_pipe_alloc(port_hdl, &pipe_config, &pipe_hdl), "Create pipe should not fail");
            TEST_ASSERT_NOT_NULL_MESSAGE(pipe_hdl, "Pipe handle should not be NULL");
            TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_free(pipe_hdl));
        }
    }
    // Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
}
