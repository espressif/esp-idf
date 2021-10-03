/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"
#include "test_usb_mock_classes.h"
#include "test_hcd_common.h"

#define NUM_URBS                3
#define NUM_PACKETS_PER_URB     3
#define ISOC_PACKET_SIZE        MOCK_ISOC_EP_MPS
#define URB_DATA_BUFF_SIZE      (NUM_PACKETS_PER_URB * ISOC_PACKET_SIZE)

/*
Test HCD ISOC pipe URBs

Purpose:
    - Test that an isochronous pipe can be created
    - URBs can be created and enqueued to the isoc pipe pipe
    - isoc pipe returns HCD_PIPE_EVENT_URB_DONE for completed URBs
    - Test utilizes ISOC OUT transfers and do not require ACKs. So the isoc pipe will target a non existing endpoint

Procedure:
    - Setup HCD and wait for connection
    - Allocate default pipe and enumerate the device
    - Allocate an isochronous pipe and multiple URBs. Each URB should contain multiple packets to test HCD's ability to
      schedule an URB across multiple intervals.
    - Enqueue those URBs
    - Expect HCD_PIPE_EVENT_URB_DONE for each URB. Verify that data is correct using logic analyzer
    - Deallocate URBs
    - Teardown
*/

TEST_CASE("Test HCD isochronous pipe URBs", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    //The MPS of the ISOC OUT pipe is quite large, so we need to bias the FIFO sizing
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_set_fifo_bias(port_hdl, HCD_PORT_FIFO_BIAS_PTX));
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Enumerate and reset device
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, 0, port_speed); //Create a default pipe (using a NULL EP descriptor)
    uint8_t dev_addr = test_hcd_enum_device(default_pipe);

    //Create ISOC OUT pipe to non-existent device
    hcd_pipe_handle_t isoc_out_pipe = test_hcd_pipe_alloc(port_hdl, &mock_isoc_out_ep_desc, dev_addr + 1, port_speed);
    //Create URBs
    urb_t *urb_list[NUM_URBS];
    //Initialize URBs
    for (int urb_idx = 0; urb_idx < NUM_URBS; urb_idx++) {
        urb_list[urb_idx] = test_hcd_alloc_urb(NUM_PACKETS_PER_URB, URB_DATA_BUFF_SIZE);
        urb_list[urb_idx]->transfer.num_bytes = URB_DATA_BUFF_SIZE;
        urb_list[urb_idx]->transfer.context = URB_CONTEXT_VAL;
        for (int pkt_idx = 0; pkt_idx < NUM_PACKETS_PER_URB; pkt_idx++) {
            urb_list[urb_idx]->transfer.isoc_packet_desc[pkt_idx].num_bytes = ISOC_PACKET_SIZE;
            //Each packet will consist of the same byte, but each subsequent packet's byte will increment (i.e., packet 0 transmits all 0x0, packet 1 transmits all 0x1)
            memset(&urb_list[urb_idx]->transfer.data_buffer[pkt_idx * ISOC_PACKET_SIZE], (urb_idx * NUM_URBS) + pkt_idx, ISOC_PACKET_SIZE);
        }
    }
    //Enqueue URBs
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(isoc_out_pipe, urb_list[i]));
    }
    //Wait for each done event from each URB
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_expect_pipe_event(isoc_out_pipe, HCD_PIPE_EVENT_URB_DONE);
    }
    //Dequeue URBs
    for (int urb_idx = 0; urb_idx < NUM_URBS; urb_idx++) {
        urb_t *urb = hcd_urb_dequeue(isoc_out_pipe);
        TEST_ASSERT_EQUAL(urb_list[urb_idx], urb);
        TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
        for (int pkt_idx = 0; pkt_idx < NUM_PACKETS_PER_URB; pkt_idx++) {
            TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.isoc_packet_desc[pkt_idx].status);
        }
    }
    //Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
    }
    test_hcd_pipe_free(isoc_out_pipe);
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}
