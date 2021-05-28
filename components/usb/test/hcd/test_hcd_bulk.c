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
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"
#include "test_hcd_common.h"

// ------------------------------------------------- Mock MSC SCSI -----------------------------------------------------

/*
Note: The following test requires that USB flash drive be connected. The flash drive should...

- Be implement the Mass Storage class supporting BULK only transfers using SCSI commands
- It's configuration 1 should have the following endpoints

Endpoint Descriptor:
    bLength             7
    bDescriptorType     5
    bEndpointAddress    0x01  EP 1 OUT
    bmAttributes        2
        Transfer Type   Bulk
        Synch Type      None
        Usage Type      Data
    wMaxPacketSize      0x0040  1x 64 bytes
    bInterval           1
Endpoint Descriptor:
    bLength             7
    bDescriptorType     5
    bEndpointAddress    0x82  EP 2 IN
    bmAttributes        2
        Transfer Type   Bulk
        Synch Type      None
        Usage Type      Data
    wMaxPacketSize      0x0040  1x 64 bytes
    bInterval           1

If you're using a flash driver with different endpoints, modify the endpoint descriptors below.
*/

static const usb_desc_ep_t bulk_out_ep_desc = {
    .bLength = sizeof(usb_desc_ep_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x01,       //EP 1 OUT
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = 64,           //MPS of 64 bytes
    .bInterval = 1,
};

static const usb_desc_ep_t bulk_in_ep_desc = {
    .bLength = sizeof(usb_desc_ep_t),
    .bDescriptorType = USB_B_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 0x82,       //EP 2 IN
    .bmAttributes = USB_BM_ATTRIBUTES_XFER_BULK,
    .wMaxPacketSize = 64,           //MPS of 64 bytes
    .bInterval = 1,
};

#define MOCK_MSC_SCSI_SECTOR_SIZE       512
#define MOCK_MSC_SCSI_LUN               0
#define MSC_SCSI_INTR_NUMBER            0

#define MOCK_MSC_SCSI_REQ_INIT_RESET(ctrl_req_ptr, intf_num) ({  \
    (ctrl_req_ptr)->bRequestType = USB_B_REQUEST_TYPE_DIR_OUT | USB_B_REQUEST_TYPE_TYPE_CLASS | USB_B_REQUEST_TYPE_RECIP_INTERFACE; \
    (ctrl_req_ptr)->bRequest = 0xFF;    \
    (ctrl_req_ptr)->wValue = 0; \
    (ctrl_req_ptr)->wIndex = (intf_num);    \
    (ctrl_req_ptr)->wLength = 0;    \
})

typedef struct __attribute__((packed)) {
    uint8_t opcode; //0x28 = read(10), 0x2A=write(10)
    uint8_t flags;
    uint8_t lba_3;
    uint8_t lba_2;
    uint8_t lba_1;
    uint8_t lba_0;
    uint8_t group;
    uint8_t len_1;
    uint8_t len_0;
    uint8_t control;
} mock_scsi_cmd10_t;

typedef struct __attribute__((packed)) {
    uint32_t dCBWSignature;
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    mock_scsi_cmd10_t CBWCB;
    uint8_t padding[6];
} mock_msc_bulk_cbw_t;

// USB Bulk Transfer Command Status Wrapper data
typedef struct __attribute__((packed)) {
    uint32_t dCSWSignature;
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
} mock_msc_bulk_csw_t;

static void mock_msc_reset_req(hcd_pipe_handle_t default_pipe)
{
    //Create IRP
    usb_irp_t *irp = heap_caps_calloc(1, sizeof(usb_irp_t), MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_EQUAL(NULL, irp);
    irp->data_buffer = heap_caps_malloc(sizeof(usb_ctrl_req_t), MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_EQUAL(NULL, irp->data_buffer);
    usb_ctrl_req_t *ctrl_req = (usb_ctrl_req_t *)irp->data_buffer;
    MOCK_MSC_SCSI_REQ_INIT_RESET(ctrl_req, MSC_SCSI_INTR_NUMBER);
    irp->num_bytes = 0;
    //Enqueue, wait, dequeue, and check IRP
    TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp));
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_IRP_DONE);
    TEST_ASSERT_EQUAL(irp, hcd_irp_dequeue(default_pipe));
    TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, irp->status);
    //Free IRP
    heap_caps_free(irp->data_buffer);
    heap_caps_free(irp);
}

static void mock_msc_scsi_init_cbw(mock_msc_bulk_cbw_t *cbw, bool is_read, int offset, int num_sectors, uint32_t tag)
{
    cbw->dCBWSignature = 0x43425355;    //Fixed value
    cbw->dCBWTag = tag; //Random value that is echoed back
    cbw->dCBWDataTransferLength = num_sectors * MOCK_MSC_SCSI_SECTOR_SIZE;
    cbw->bmCBWFlags = (is_read) ? (1 << 7) : 0; //If this is a read, set the direction flag
    cbw->bCBWLUN = MOCK_MSC_SCSI_LUN;
    cbw->bCBWCBLength = 10;     //The length of the SCSI command
    //Initialize SCSI CMD as READ10 or WRITE 10
    cbw->CBWCB.opcode = (is_read) ? 0x28 : 0x2A;  //SCSI CMD READ10 or WRITE10
    cbw->CBWCB.flags = 0;
    cbw->CBWCB.lba_3 = (offset >> 24);
    cbw->CBWCB.lba_2 = (offset >> 16);
    cbw->CBWCB.lba_1 = (offset >> 8);
    cbw->CBWCB.lba_0 = (offset >> 0);
    cbw->CBWCB.group = 0;
    cbw->CBWCB.len_1 = (num_sectors >> 8);
    cbw->CBWCB.len_0 = (num_sectors >> 0);
    cbw->CBWCB.control = 0;
}

static bool mock_msc_scsi_check_csw(mock_msc_bulk_csw_t *csw, uint32_t tag_expect)
{
    bool no_issues = true;
    if (csw->dCSWSignature != 0x53425355) {
        no_issues = false;
        printf("Warning: csw signature corrupt (0x%X)\n", csw->dCSWSignature);
    }
    if (csw->dCSWTag != tag_expect) {
        no_issues = false;
        printf("Warning: csw tag unexpected! Expected %d got %d\n", tag_expect, csw->dCSWTag);
    }
    if (csw->dCSWDataResidue) {
        no_issues = false;
        printf("Warning: csw indicates data residue of %d bytes!\n", csw->dCSWDataResidue);
    }
    if (csw->bCSWStatus) {
        no_issues = false;
        printf("Warning: csw indicates non-good status %d!\n", csw->bCSWStatus);
    }
    return no_issues;
}

// --------------------------------------------------- Test Cases ------------------------------------------------------

/*
Test HCD bulk pipe IRPs

Purpose:
    - Test that a bulk pipe can be created
    - IRPs can be created and enqueued to the bulk pipe pipe
    - Bulk pipe returns HCD_PIPE_EVENT_IRP_DONE for completed IRPs
    - Test utilizes a bare bones (i.e., mock) MSC class using SCSI commands

Procedure:
    - Setup HCD and wait for connection
    - Allocate default pipe and enumerate the device
    - Allocate separate IRPS for CBW, Data, and CSW transfers of the MSC class
    - Read TEST_NUM_SECTORS number of sectors for the mass storage device
    - Expect HCD_PIPE_EVENT_IRP_DONE for each IRP
    - Deallocate IRPs
    - Teardown
*/

#define TEST_NUM_SECTORS                10
#define TEST_NUM_SECTORS_PER_ITER       2

TEST_CASE("Test HCD bulk pipe IRPs", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Enumerate and reset MSC SCSI device
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, 0, port_speed); //Create a default pipe (using a NULL EP descriptor)
    uint8_t dev_addr = test_hcd_enum_devc(default_pipe);
    mock_msc_reset_req(default_pipe);

    //Create BULK IN and BULK OUT pipes for SCSI
    hcd_pipe_handle_t bulk_out_pipe = test_hcd_pipe_alloc(port_hdl, &bulk_out_ep_desc, dev_addr, port_speed);
    hcd_pipe_handle_t bulk_in_pipe = test_hcd_pipe_alloc(port_hdl, &bulk_in_ep_desc, dev_addr, port_speed);
    //Create IRPs for CBW, Data, and CSW transport. IN Buffer sizes are rounded up to nearest MPS
    usb_irp_t *irp_cbw = test_hcd_alloc_irp(0, sizeof(mock_msc_bulk_cbw_t));
    usb_irp_t *irp_data = test_hcd_alloc_irp(0, TEST_NUM_SECTORS_PER_ITER * MOCK_MSC_SCSI_SECTOR_SIZE);
    usb_irp_t *irp_csw = test_hcd_alloc_irp(0, sizeof(mock_msc_bulk_csw_t) + (bulk_in_ep_desc.wMaxPacketSize - (sizeof(mock_msc_bulk_csw_t) % bulk_in_ep_desc.wMaxPacketSize)));
    irp_cbw->num_bytes = sizeof(mock_msc_bulk_cbw_t);
    irp_data->num_bytes = TEST_NUM_SECTORS_PER_ITER * MOCK_MSC_SCSI_SECTOR_SIZE;
    irp_csw->num_bytes = sizeof(mock_msc_bulk_csw_t) + (bulk_in_ep_desc.wMaxPacketSize - (sizeof(mock_msc_bulk_csw_t) % bulk_in_ep_desc.wMaxPacketSize));

    for (int block_num = 0; block_num < TEST_NUM_SECTORS; block_num += TEST_NUM_SECTORS_PER_ITER) {
        //Initialize CBW IRP, then send it on the BULK OUT pipe
        mock_msc_scsi_init_cbw((mock_msc_bulk_cbw_t *)irp_cbw->data_buffer, true, block_num, TEST_NUM_SECTORS_PER_ITER, 0xAAAAAAAA);
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(bulk_out_pipe, irp_cbw));
        test_hcd_expect_pipe_event(bulk_out_pipe, HCD_PIPE_EVENT_IRP_DONE);
        TEST_ASSERT_EQUAL(irp_cbw, hcd_irp_dequeue(bulk_out_pipe));
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, irp_cbw->status);
        //Read data through BULK IN pipe
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(bulk_in_pipe, irp_data));
        test_hcd_expect_pipe_event(bulk_in_pipe, HCD_PIPE_EVENT_IRP_DONE);
        TEST_ASSERT_EQUAL(irp_data, hcd_irp_dequeue(bulk_in_pipe));
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, irp_data->status);
        //Read the CSW through BULK IN pipe
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(bulk_in_pipe, irp_csw));
        test_hcd_expect_pipe_event(bulk_in_pipe, HCD_PIPE_EVENT_IRP_DONE);
        TEST_ASSERT_EQUAL(irp_csw, hcd_irp_dequeue(bulk_in_pipe));
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, irp_data->status);
        TEST_ASSERT_EQUAL(sizeof(mock_msc_bulk_csw_t), irp_csw->actual_num_bytes);
        TEST_ASSERT_EQUAL(true, mock_msc_scsi_check_csw((mock_msc_bulk_csw_t *)irp_csw->data_buffer, 0xAAAAAAAA));
        //Print the read data
        printf("Block %d to %d:\n", block_num, block_num + TEST_NUM_SECTORS_PER_ITER);
        for (int i = 0; i < irp_data->actual_num_bytes; i++) {
            printf("0x%02x,", ((char *)irp_data->data_buffer)[i]);
        }
        printf("\n\n");
    }

    test_hcd_free_irp(irp_cbw);
    test_hcd_free_irp(irp_data);
    test_hcd_free_irp(irp_csw);
    test_hcd_pipe_free(bulk_out_pipe);
    test_hcd_pipe_free(bulk_in_pipe);
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}
