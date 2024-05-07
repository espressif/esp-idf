/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "dev_hid.h"
#include "test_hcd_common.h"

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
    // Clearnup
    test_hcd_wait_for_disconn(port_hdl, false);
}
