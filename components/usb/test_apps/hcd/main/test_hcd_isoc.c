/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_usb_mock_msc.h"
#include "test_usb_common.h"
#include "test_hcd_common.h"

#define NUM_URBS                3
#define NUM_PACKETS_PER_URB     3
#define ISOC_PACKET_SIZE        MOCK_ISOC_EP_MPS
#define URB_DATA_BUFF_SIZE      (NUM_PACKETS_PER_URB * ISOC_PACKET_SIZE)
#define POST_ENQUEUE_DELAY_US   20

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

TEST_CASE("Test HCD isochronous pipe URBs", "[isoc][full_speed]")
{
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
        //Overall URB status and overall number of bytes
        TEST_ASSERT_EQUAL(URB_DATA_BUFF_SIZE, urb->transfer.actual_num_bytes);
        TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status, "Transfer NOT completed");
        for (int pkt_idx = 0; pkt_idx < NUM_PACKETS_PER_URB; pkt_idx++) {
            TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.isoc_packet_desc[pkt_idx].status, "Transfer NOT completed");
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
}

/*
Test a port sudden disconnect with an active ISOC pipe

Purpose: Test that when sudden disconnection happens on an HCD port, the ISOC pipe will
    - Remain active after the HCD_PORT_EVENT_SUDDEN_DISCONN port event
    - ISOC pipe can be halted
    - ISOC pipe can be flushed (and transfers status are updated accordingly)

Procedure:
    - Setup HCD and wait for connection
    - Allocate default pipe and enumerate the device
    - Allocate an isochronous pipe and multiple URBs. Each URB should contain multiple packets to test HCD's ability to
      schedule an URB across multiple intervals.
    - Enqueue those URBs
    - Trigger a disconnect after a short delay
    - Check that HCD_PORT_EVENT_SUDDEN_DISCONN event is generated. Handle that port event.
    - Check that both pipes remain in the HCD_PIPE_STATE_ACTIVE after the port error.
    - Check that both pipes pipe can be halted.
    - Check that the default pipe can be flushed. A HCD_PIPE_EVENT_URB_DONE event should be generated for the ISOC pipe
      because it had enqueued URBs.
    - Check that all URBs can be dequeued and their status is updated
    - Free both pipes
    - Teardown
*/
TEST_CASE("Test HCD isochronous pipe sudden disconnect", "[isoc][full_speed]")
{
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
    //Add a short delay to let the transfers run for a bit
    esp_rom_delay_us(POST_ENQUEUE_DELAY_US);
    test_usb_set_phy_state(false, 0);
    //Disconnect event should have occurred. Handle the port event
    test_hcd_expect_port_event(port_hdl, HCD_PORT_EVENT_DISCONNECTION);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_DISCONNECTION, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_RECOVERY, hcd_port_get_state(port_hdl));
    printf("Sudden disconnect\n");

    //Both pipes should still be active
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(isoc_out_pipe));
    //Halt both pipes
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_HALT));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(isoc_out_pipe, HCD_PIPE_CMD_HALT));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(isoc_out_pipe));
    //Flush both pipes. ISOC pipe should return completed URBs
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_FLUSH));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(isoc_out_pipe, HCD_PIPE_CMD_FLUSH));

    //Dequeue ISOC URBs
    for (int urb_idx = 0; urb_idx < NUM_URBS; urb_idx++) {
        urb_t *urb = hcd_urb_dequeue(isoc_out_pipe);
        TEST_ASSERT_EQUAL(urb_list[urb_idx], urb);
        TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
        //The URB has either completed entirely or is marked as no_device
        TEST_ASSERT(urb->transfer.status == USB_TRANSFER_STATUS_COMPLETED || urb->transfer.status == USB_TRANSFER_STATUS_NO_DEVICE);
    }

    //Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
    }
    test_hcd_pipe_free(isoc_out_pipe);
    test_hcd_pipe_free(default_pipe);
}
