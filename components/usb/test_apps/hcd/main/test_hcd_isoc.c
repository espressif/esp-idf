/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "hal/usb_dwc_ll.h" // For USB-DWC configuration
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "dev_isoc.h"
#include "usb/usb_types_ch9.h"
#include "test_hcd_common.h"

#define NUM_URBS                3
#define NUM_PACKETS_PER_URB     3
#define POST_ENQUEUE_DELAY_US   20
#define ENQUEUE_DELAY (usb_dwc_ll_ghwcfg_get_hsphy_type(USB_DWC_LL_GET_HW(0)) ? 100 : 500) // With this delay we want to enqueue the URBs at different times

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

TEST_CASE("Test HCD isochronous pipe URBs", "[isoc][full_speed][high_speed]")
{
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  // Trigger a connection
    // The MPS of the ISOC OUT pipe is quite large, so we need to bias the FIFO sizing
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_set_fifo_bias(port_hdl, HCD_PORT_FIFO_BIAS_PTX));
    vTaskDelay(pdMS_TO_TICKS(100)); // Short delay send of SOF (for FS) or EOPs (for LS)

    // Enumerate and reset device
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, 0, port_speed); // Create a default pipe (using a NULL EP descriptor)
    uint8_t dev_addr = test_hcd_enum_device(default_pipe);

    // Create ISOC OUT pipe to non-existent device
    const usb_ep_desc_t *out_ep_desc = dev_isoc_get_out_ep_desc(port_speed);
    const int isoc_packet_size = USB_EP_DESC_GET_MPS(out_ep_desc);
    hcd_pipe_handle_t isoc_out_pipe = test_hcd_pipe_alloc(port_hdl, out_ep_desc, dev_addr + 1, port_speed);
    // Create URBs
    urb_t *urb_list[NUM_URBS];
    // Initialize URBs
    for (int urb_idx = 0; urb_idx < NUM_URBS; urb_idx++) {
        urb_list[urb_idx] = test_hcd_alloc_urb(NUM_PACKETS_PER_URB, NUM_PACKETS_PER_URB * isoc_packet_size);
        urb_list[urb_idx]->transfer.num_bytes = NUM_PACKETS_PER_URB * isoc_packet_size;
        urb_list[urb_idx]->transfer.context = URB_CONTEXT_VAL;
        for (int pkt_idx = 0; pkt_idx < NUM_PACKETS_PER_URB; pkt_idx++) {
            urb_list[urb_idx]->transfer.isoc_packet_desc[pkt_idx].num_bytes = isoc_packet_size;
            // Each packet will consist of the same byte, but each subsequent packet's byte will increment (i.e., packet 0 transmits all 0x0, packet 1 transmits all 0x1)
            memset(&urb_list[urb_idx]->transfer.data_buffer[pkt_idx * isoc_packet_size], (urb_idx * NUM_URBS) + pkt_idx, isoc_packet_size);
        }
    }
    // Enqueue URBs
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(isoc_out_pipe, urb_list[i]));
    }
    // Wait for each done event from each URB
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_expect_pipe_event(isoc_out_pipe, HCD_PIPE_EVENT_URB_DONE);
    }
    // Dequeue URBs
    for (int urb_idx = 0; urb_idx < NUM_URBS; urb_idx++) {
        urb_t *urb = hcd_urb_dequeue(isoc_out_pipe);
        TEST_ASSERT_EQUAL(urb_list[urb_idx], urb);
        TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
        // Overall URB status and overall number of bytes
        TEST_ASSERT_EQUAL(NUM_PACKETS_PER_URB * isoc_packet_size, urb->transfer.actual_num_bytes);
        TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status, "Transfer NOT completed");
        for (int pkt_idx = 0; pkt_idx < NUM_PACKETS_PER_URB; pkt_idx++) {
            TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.isoc_packet_desc[pkt_idx].status, "Transfer NOT completed");
        }
    }
    // Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
    }
    test_hcd_pipe_free(isoc_out_pipe);
    test_hcd_pipe_free(default_pipe);
    // Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
}

/*
Test HCD ISOC pipe URBs with all channels and intervals combinations

Purpose:
    - Test that the ISOC scheduler correctly schedules all channels and intervals

Procedure:
    - Setup HCD and wait for connection
    - Allocate default pipe and enumerate the device
    - Allocate an isochronous pipe and multiple URBs. Each URB should contain multiple packets to test HCD's ability to
      schedule an URB across multiple intervals.
    - Repeat for all channels and intervals
    - Enqueue those URBs
    - Expect HCD_PIPE_EVENT_URB_DONE for each URB. Verify that data is correct using logic analyzer
    - Deallocate URBs
    - Teardown
*/
TEST_CASE("Test HCD isochronous pipe URBs all", "[isoc][full_speed][high_speed]")
{
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  // Trigger a connection
    // The MPS of the ISOC OUT pipe is quite large, so we need to bias the FIFO sizing
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_set_fifo_bias(port_hdl, HCD_PORT_FIFO_BIAS_PTX));
    vTaskDelay(pdMS_TO_TICKS(100)); // Short delay send of SOF (for FS) or EOPs (for LS)

    // Enumerate and reset device
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, 0, port_speed); // Create a default pipe (using a NULL EP descriptor)
    uint8_t dev_addr = test_hcd_enum_device(default_pipe);

    urb_t *urb_list[NUM_URBS];
    hcd_pipe_handle_t unused_pipes[16];
    const usb_ep_desc_t *out_ep_desc = dev_isoc_get_out_ep_desc(port_speed);
    const int isoc_packet_size = USB_EP_DESC_GET_MPS(out_ep_desc);

    // For all channels (except channel allocated for EP0)
    for (int channel = 0; channel < usb_dwc_ll_ghwcfg_get_channel_num(USB_DWC_LL_GET_HW(0)) - 1; channel++) {
        // Allocate unused pipes, so the active isoc_out_pipe uses different channel index
        for (int ch = 0; ch < channel; ch++) {
            unused_pipes[ch] = test_hcd_pipe_alloc(port_hdl, out_ep_desc, dev_addr + 1, port_speed);
        }

        // For all intervals
        for (int interval = 1; interval <= 6; interval++) {

            vTaskDelay(5);
            unsigned num_packets_per_urb = 32; // This is maximum number of packets if interval = 1. This is limited by FRAME_LIST_LEN
            num_packets_per_urb >>= (interval - 1);
            // Create ISOC OUT pipe
            usb_ep_desc_t isoc_out_ep;
            memcpy(&isoc_out_ep, out_ep_desc, sizeof(usb_ep_desc_t));
            isoc_out_ep.bInterval = interval;
            isoc_out_ep.bEndpointAddress = interval; // So you can see the bInterval value in trace
            hcd_pipe_handle_t isoc_out_pipe = test_hcd_pipe_alloc(port_hdl, &isoc_out_ep, channel + 1, port_speed); // Channel number represented in dev_num, so you can see it in trace

            // Initialize URBs
            for (int urb_idx = 0; urb_idx < NUM_URBS; urb_idx++) {
                urb_list[urb_idx] = test_hcd_alloc_urb(num_packets_per_urb, num_packets_per_urb * isoc_packet_size);
                urb_list[urb_idx]->transfer.num_bytes = num_packets_per_urb * isoc_packet_size;
                urb_list[urb_idx]->transfer.context = URB_CONTEXT_VAL;
                for (int pkt_idx = 0; pkt_idx < num_packets_per_urb; pkt_idx++) {
                    urb_list[urb_idx]->transfer.isoc_packet_desc[pkt_idx].num_bytes = isoc_packet_size;
                    // Each packet will consist of the same byte, but each subsequent packet's byte will increment (i.e., packet 0 transmits all 0x0, packet 1 transmits all 0x1)
                    memset(&urb_list[urb_idx]->transfer.data_buffer[pkt_idx * isoc_packet_size], (urb_idx * num_packets_per_urb) + pkt_idx, isoc_packet_size);
                }
            }

            // Add a delay so we start scheduling the transactions at different time in USB frame
            esp_rom_delay_us(ENQUEUE_DELAY * (interval - 1) + ENQUEUE_DELAY * channel);

            // Enqueue URBs
            for (int i = 0; i < NUM_URBS; i++) {
                TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(isoc_out_pipe, urb_list[i]));
            }
            // Wait for each done event from each URB
            for (int i = 0; i < NUM_URBS; i++) {
                test_hcd_expect_pipe_event(isoc_out_pipe, HCD_PIPE_EVENT_URB_DONE);
            }
            // Dequeue URBs
            for (int urb_idx = 0; urb_idx < NUM_URBS; urb_idx++) {
                urb_t *urb = hcd_urb_dequeue(isoc_out_pipe);
                TEST_ASSERT_EQUAL(urb_list[urb_idx], urb);
                TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
                // Overall URB status and overall number of bytes
                TEST_ASSERT_EQUAL(num_packets_per_urb * isoc_packet_size, urb->transfer.actual_num_bytes);
                TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status, "Transfer NOT completed");
                for (int pkt_idx = 0; pkt_idx < num_packets_per_urb; pkt_idx++) {
                    TEST_ASSERT_EQUAL_MESSAGE(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.isoc_packet_desc[pkt_idx].status, "Transfer NOT completed");
                }
            }
            // Free URB list and pipe
            for (int i = 0; i < NUM_URBS; i++) {
                test_hcd_free_urb(urb_list[i]);
            }
            test_hcd_pipe_free(isoc_out_pipe);
        }

        // Free unused pipes
        for (int ch = 0; ch < channel; ch++) {
            test_hcd_pipe_free(unused_pipes[ch]);
        }
    }
    test_hcd_pipe_free(default_pipe);
    // Cleanup
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
TEST_CASE("Test HCD isochronous pipe sudden disconnect", "[isoc][full_speed][high_speed]")
{
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  // Trigger a connection
    // The MPS of the ISOC OUT pipe is quite large, so we need to bias the FIFO sizing
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_set_fifo_bias(port_hdl, HCD_PORT_FIFO_BIAS_PTX));
    vTaskDelay(pdMS_TO_TICKS(100)); // Short delay send of SOF (for FS) or EOPs (for LS)

    // Enumerate and reset device
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, 0, port_speed); // Create a default pipe (using a NULL EP descriptor)
    uint8_t dev_addr = test_hcd_enum_device(default_pipe);

    // Create ISOC OUT pipe to non-existent device
    const usb_ep_desc_t *out_ep_desc = dev_isoc_get_out_ep_desc(port_speed);
    const int isoc_packet_size = USB_EP_DESC_GET_MPS(out_ep_desc);
    hcd_pipe_handle_t isoc_out_pipe = test_hcd_pipe_alloc(port_hdl, out_ep_desc, dev_addr + 1, port_speed);
    // Create URBs
    urb_t *urb_list[NUM_URBS];
    // Initialize URBs
    for (int urb_idx = 0; urb_idx < NUM_URBS; urb_idx++) {
        urb_list[urb_idx] = test_hcd_alloc_urb(NUM_PACKETS_PER_URB, NUM_PACKETS_PER_URB * isoc_packet_size);
        urb_list[urb_idx]->transfer.num_bytes = NUM_PACKETS_PER_URB * isoc_packet_size;
        urb_list[urb_idx]->transfer.context = URB_CONTEXT_VAL;
        for (int pkt_idx = 0; pkt_idx < NUM_PACKETS_PER_URB; pkt_idx++) {
            urb_list[urb_idx]->transfer.isoc_packet_desc[pkt_idx].num_bytes = isoc_packet_size;
            // Each packet will consist of the same byte, but each subsequent packet's byte will increment (i.e., packet 0 transmits all 0x0, packet 1 transmits all 0x1)
            memset(&urb_list[urb_idx]->transfer.data_buffer[pkt_idx * isoc_packet_size], (urb_idx * NUM_URBS) + pkt_idx, isoc_packet_size);
        }
    }
    // Enqueue URBs
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(isoc_out_pipe, urb_list[i]));
    }
    // Add a short delay to let the transfers run for a bit
    esp_rom_delay_us(POST_ENQUEUE_DELAY_US);
    // Power-off the port to trigger a disconnection
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_POWER_OFF));
    // Disconnect event should have occurred. Handle the port event
    test_hcd_expect_port_event(port_hdl, HCD_PORT_EVENT_DISCONNECTION);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_DISCONNECTION, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_RECOVERY, hcd_port_get_state(port_hdl));
    printf("Sudden disconnect\n");

    // Both pipes should still be active
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(isoc_out_pipe));
    // Halt both pipes
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_HALT));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(isoc_out_pipe, HCD_PIPE_CMD_HALT));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(isoc_out_pipe));
    // Flush both pipes. ISOC pipe should return completed URBs
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_FLUSH));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(isoc_out_pipe, HCD_PIPE_CMD_FLUSH));

    // Dequeue ISOC URBs
    for (int urb_idx = 0; urb_idx < NUM_URBS; urb_idx++) {
        urb_t *urb = hcd_urb_dequeue(isoc_out_pipe);
        TEST_ASSERT_EQUAL(urb_list[urb_idx], urb);
        TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
        // The URB has either completed entirely or is marked as no_device
        TEST_ASSERT(urb->transfer.status == USB_TRANSFER_STATUS_COMPLETED || urb->transfer.status == USB_TRANSFER_STATUS_NO_DEVICE);
    }

    // Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
    }
    test_hcd_pipe_free(isoc_out_pipe);
    test_hcd_pipe_free(default_pipe);
}
