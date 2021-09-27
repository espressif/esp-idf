/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"
#include "test_usb_mock_classes.h"
#include "test_hcd_common.h"

// --------------------------------------------------- Test Cases ------------------------------------------------------

static void mock_msc_reset_req(hcd_pipe_handle_t default_pipe)
{
    //Create URB
    urb_t *urb = test_hcd_alloc_urb(0, sizeof(usb_setup_packet_t));
    usb_setup_packet_t *setup_pkt = (usb_setup_packet_t *)urb->transfer.data_buffer;
    MOCK_MSC_SCSI_REQ_INIT_RESET(setup_pkt, MOCK_MSC_SCSI_INTF_NUMBER);
    urb->transfer.num_bytes = sizeof(usb_setup_packet_t);
    //Enqueue, wait, dequeue, and check URB
    TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb));
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_URB_DONE);
    TEST_ASSERT_EQUAL(urb, hcd_urb_dequeue(default_pipe));
    TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status);
    //Free URB
    test_hcd_free_urb(urb);
}

/*
Test HCD bulk pipe URBs

Purpose:
    - Test that a bulk pipe can be created
    - URBs can be created and enqueued to the bulk pipe pipe
    - Bulk pipe returns HCD_PIPE_EVENT_URB_DONE for completed URBs
    - Test utilizes a bare bones (i.e., mock) MSC class using SCSI commands

Procedure:
    - Setup HCD and wait for connection
    - Allocate default pipe and enumerate the device
    - Allocate separate URBS for CBW, Data, and CSW transfers of the MSC class
    - Read TEST_NUM_SECTORS_TOTAL number of sectors for the mass storage device
    - Expect HCD_PIPE_EVENT_URB_DONE for each URB
    - Deallocate URBs
    - Teardown
*/

#define TEST_NUM_SECTORS_TOTAL          10
#define TEST_NUM_SECTORS_PER_XFER       2

TEST_CASE("Test HCD bulk pipe URBs", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Enumerate and reset MSC SCSI device
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, 0, port_speed); //Create a default pipe (using a NULL EP descriptor)
    uint8_t dev_addr = test_hcd_enum_device(default_pipe);
    mock_msc_reset_req(default_pipe);

    //Create BULK IN and BULK OUT pipes for SCSI
    hcd_pipe_handle_t bulk_out_pipe = test_hcd_pipe_alloc(port_hdl, &mock_msc_scsi_bulk_out_ep_desc, dev_addr, port_speed);
    hcd_pipe_handle_t bulk_in_pipe = test_hcd_pipe_alloc(port_hdl, &mock_msc_scsi_bulk_in_ep_desc, dev_addr, port_speed);
    //Create URBs for CBW, Data, and CSW transport. IN Buffer sizes are rounded up to nearest MPS
    urb_t *urb_cbw = test_hcd_alloc_urb(0, sizeof(mock_msc_bulk_cbw_t));
    urb_t *urb_data = test_hcd_alloc_urb(0, TEST_NUM_SECTORS_PER_XFER * MOCK_MSC_SCSI_SECTOR_SIZE);
    urb_t *urb_csw = test_hcd_alloc_urb(0, sizeof(mock_msc_bulk_csw_t) + (mock_msc_scsi_bulk_in_ep_desc.wMaxPacketSize - (sizeof(mock_msc_bulk_csw_t) % mock_msc_scsi_bulk_in_ep_desc.wMaxPacketSize)));
    urb_cbw->transfer.num_bytes = sizeof(mock_msc_bulk_cbw_t);
    urb_data->transfer.num_bytes = TEST_NUM_SECTORS_PER_XFER * MOCK_MSC_SCSI_SECTOR_SIZE;
    urb_csw->transfer.num_bytes = sizeof(mock_msc_bulk_csw_t) + (mock_msc_scsi_bulk_in_ep_desc.wMaxPacketSize - (sizeof(mock_msc_bulk_csw_t) % mock_msc_scsi_bulk_in_ep_desc.wMaxPacketSize));

    for (int block_num = 0; block_num < TEST_NUM_SECTORS_TOTAL; block_num += TEST_NUM_SECTORS_PER_XFER) {
        //Initialize CBW URB, then send it on the BULK OUT pipe
        mock_msc_scsi_init_cbw((mock_msc_bulk_cbw_t *)urb_cbw->transfer.data_buffer, true, block_num, TEST_NUM_SECTORS_PER_XFER, 0xAAAAAAAA);
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(bulk_out_pipe, urb_cbw));
        test_hcd_expect_pipe_event(bulk_out_pipe, HCD_PIPE_EVENT_URB_DONE);
        TEST_ASSERT_EQUAL(urb_cbw, hcd_urb_dequeue(bulk_out_pipe));
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, urb_cbw->transfer.status);
        //Read data through BULK IN pipe
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(bulk_in_pipe, urb_data));
        test_hcd_expect_pipe_event(bulk_in_pipe, HCD_PIPE_EVENT_URB_DONE);
        TEST_ASSERT_EQUAL(urb_data, hcd_urb_dequeue(bulk_in_pipe));
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, urb_data->transfer.status);
        //Read the CSW through BULK IN pipe
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(bulk_in_pipe, urb_csw));
        test_hcd_expect_pipe_event(bulk_in_pipe, HCD_PIPE_EVENT_URB_DONE);
        TEST_ASSERT_EQUAL(urb_csw, hcd_urb_dequeue(bulk_in_pipe));
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, urb_data->transfer.status);
        TEST_ASSERT_EQUAL(sizeof(mock_msc_bulk_csw_t), urb_csw->transfer.actual_num_bytes);
        TEST_ASSERT_EQUAL(true, mock_msc_scsi_check_csw((mock_msc_bulk_csw_t *)urb_csw->transfer.data_buffer, 0xAAAAAAAA));
        //Print the read data
        printf("Block %d to %d:\n", block_num, block_num + TEST_NUM_SECTORS_PER_XFER);
        for (int i = 0; i < urb_data->transfer.actual_num_bytes; i++) {
            printf("0x%02x,", ((char *)urb_data->transfer.data_buffer)[i]);
        }
        printf("\n\n");
    }

    test_hcd_free_urb(urb_cbw);
    test_hcd_free_urb(urb_data);
    test_hcd_free_urb(urb_csw);
    test_hcd_pipe_free(bulk_out_pipe);
    test_hcd_pipe_free(bulk_in_pipe);
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}
