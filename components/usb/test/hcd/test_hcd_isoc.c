// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"
#include "test_hcd_common.h"

//We talk to a non-existent device. Since ISOC out requires no ACK, there should be no errors.
#define MOCK_ISOC_EP_NUM        2
#define MOCK_ISOC_EP_MPS        512

#define NUM_IRPS                3
#define NUM_PACKETS_PER_IRP     3
#define ISOC_PACKET_SIZE        MOCK_ISOC_EP_MPS
#define IRP_DATA_BUFF_SIZE      (NUM_PACKETS_PER_IRP * ISOC_PACKET_SIZE)

static const usb_desc_ep_t isoc_out_ep_desc = {
    .bLength = sizeof(usb_desc_ep_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = MOCK_ISOC_EP_NUM,
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_ISOC,
    .wMaxPacketSize = MOCK_ISOC_EP_MPS,     //MPS of 512 bytes
    .bInterval = 1,     //Isoc interval is (2 ^ (bInterval - 1)) which means an interval of 1ms
};

/*
Test HCD ISOC pipe IRPs

Purpose:
    - Test that an isochronous pipe can be created
    - IRPs can be created and enqueued to the isoc pipe pipe
    - isoc pipe returns HCD_PIPE_EVENT_IRP_DONE for completed IRPs
    - Test utilizes ISOC OUT transfers and do not require ACKs. So the isoc pipe will target a non existing endpoint

Procedure:
    - Setup HCD and wait for connection
    - Allocate default pipe and enumerate the device
    - Allocate an isochronous pipe and multiple IRPs. Each IRP should contain multiple packets to test HCD's ability to
      schedule an IRP across multiple intervals.
    - Enqueue those IRPs
    - Expect HCD_PIPE_EVENT_IRP_DONE for each IRP. Verify that data is correct using logic analyzer
    - Deallocate IRPs
    - Teardown
*/

TEST_CASE("Test HCD isochronous pipe IRPs", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    //The MPS of the ISOC OUT pipe is quite large, so we need to bias the FIFO sizing
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_set_fifo_bias(port_hdl, HCD_PORT_FIFO_BIAS_PTX));
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Enumerate and reset device
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, 0, port_speed); //Create a default pipe (using a NULL EP descriptor)
    uint8_t dev_addr = test_hcd_enum_devc(default_pipe);

    //Create ISOC OUT pipe to non-existent device
    hcd_pipe_handle_t isoc_out_pipe = test_hcd_pipe_alloc(port_hdl, &isoc_out_ep_desc, dev_addr + 1, port_speed);
    //Create IRPs
    usb_irp_t *irp_list[NUM_IRPS];
    //Initialize IRPs
    for (int irp_idx = 0; irp_idx < NUM_IRPS; irp_idx++) {
        irp_list[irp_idx] = test_hcd_alloc_irp(NUM_PACKETS_PER_IRP, IRP_DATA_BUFF_SIZE);
        irp_list[irp_idx]->num_bytes = 0;  //num_bytes is not used for ISOC
        irp_list[irp_idx]->context = IRP_CONTEXT_VAL;
        for (int pkt_idx = 0; pkt_idx < NUM_PACKETS_PER_IRP; pkt_idx++) {
            irp_list[irp_idx]->iso_packet_desc[pkt_idx].length = ISOC_PACKET_SIZE;
            //Each packet will consist of the same byte, but each subsequent packet's byte will increment (i.e., packet 0 transmits all 0x0, packet 1 transmits all 0x1)
            memset(&irp_list[irp_idx]->data_buffer[pkt_idx * ISOC_PACKET_SIZE], (irp_idx * NUM_IRPS) + pkt_idx, ISOC_PACKET_SIZE);
        }
    }
    //Enqueue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(isoc_out_pipe, irp_list[i]));
    }
    //Wait for each done event from each IRP
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_expect_pipe_event(isoc_out_pipe, HCD_PIPE_EVENT_IRP_DONE);
    }
    //Dequeue IRPs
    for (int irp_idx = 0; irp_idx < NUM_IRPS; irp_idx++) {
        usb_irp_t *irp = hcd_irp_dequeue(isoc_out_pipe);
        TEST_ASSERT_EQUAL(irp_list[irp_idx], irp);
        TEST_ASSERT_EQUAL(IRP_CONTEXT_VAL, irp->context);
        for (int pkt_idx = 0; pkt_idx < NUM_PACKETS_PER_IRP; pkt_idx++) {
            TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, irp->iso_packet_desc[pkt_idx].status);
        }
    }
    //Free IRP list and pipe
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_free_irp(irp_list[i]);
    }
    test_hcd_pipe_free(isoc_out_pipe);
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}
