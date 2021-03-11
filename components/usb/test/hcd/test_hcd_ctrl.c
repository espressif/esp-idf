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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "test_utils.h"
#include "test_hcd_common.h"

#define TEST_DEV_ADDR               0
#define NUM_IRPS                    3
#define TRANSFER_MAX_BYTES          256
#define IRP_DATA_BUFF_SIZE          (sizeof(usb_ctrl_req_t) + TRANSFER_MAX_BYTES)   //256 is worst case size for configuration descriptors

/*
Test HCD control pipe IRPs (normal completion and early abort)

Purpose:
    - Test that a control pipe can be created
    - IRPs can be created and enqueued to the control pipe
    - Control pipe returns HCD_PIPE_EVENT_IRP_DONE
    - Test that IRPs can be aborted when enqueued

Procedure:
    - Setup HCD and wait for connection
    - Setup default pipe and allocate IRPs
    - Enqueue IRPs
    - Expect HCD_PIPE_EVENT_IRP_DONE
    - Requeue IRPs, but abort them immediately
    - Expect IRP to be USB_TRANSFER_STATUS_CANCELED or USB_TRANSFER_STATUS_COMPLETED
    - Teardown
*/
TEST_CASE("Test HCD control pipe IRPs", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate some IRPs and initialize their data buffers with control transfers
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, TEST_DEV_ADDR, port_speed); //Create a default pipe (using a NULL EP descriptor)
    usb_irp_t *irp_list[NUM_IRPS];
    for (int i = 0; i < NUM_IRPS; i++) {
        irp_list[i] = test_hcd_alloc_irp(0, IRP_DATA_BUFF_SIZE);
        //Initialize with a "Get Config Descriptor request"
        irp_list[i]->num_bytes = TRANSFER_MAX_BYTES;
        USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *)irp_list[i]->data_buffer, 0, TRANSFER_MAX_BYTES);
        irp_list[i]->context = IRP_CONTEXT_VAL;
    }

    //Enqueue IRPs but immediately suspend the port
    printf("Enqueuing IRPs\n");
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    //Wait for each done event of each IRP
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_IRP_DONE);
    }
    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(irp_list[i], irp);
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, irp->status);
        TEST_ASSERT_EQUAL(IRP_CONTEXT_VAL, irp->context);
    }

    //Enqueue IRPs again but abort them short after
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_abort(irp_list[i]));
    }
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for any inflight transfers to complete

    //Wait for the IRPs to complete and dequeue them, then check results
    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        //No need to check for IRP pointer address as they may be out of order
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED || irp->status == USB_TRANSFER_STATUS_CANCELED);
        if (irp->status == USB_TRANSFER_STATUS_COMPLETED) {
            TEST_ASSERT_GREATER_THAN(0, irp->actual_num_bytes);
        } else {
            TEST_ASSERT_EQUAL(0, irp->actual_num_bytes);
        }
        TEST_ASSERT_EQUAL(irp->context, IRP_CONTEXT_VAL);
    }

    //Free IRP list and pipe
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_free_irp(irp_list[i]);
    }
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}

/*
Test HCD control pipe STALL condition, abort, and clear

Purpose:
    - Test that a control pipe can react to a STALL (i.e., a HCD_PIPE_EVENT_HALTED event)
    - The HCD_PIPE_CMD_ABORT can retire all IRPs
    - Pipe clear command can return the pipe to being active

Procedure:
    - Setup HCD and wait for connection
    - Setup default pipe and allocate IRPs
    - Corrupt the first IRP so that it will trigger a STALL, then enqueue all the IRPs
    - Check that a HCD_PIPE_EVENT_ERROR_STALL event is triggered
    - Check that all IRPs can be retired using HCD_PIPE_CMD_ABORT
    - Check that the STALL can be cleared by using HCD_PIPE_CMD_CLEAR
    - Fix the corrupt first IRP and retry the IRPs
    - Dequeue IRPs
    - Teardown
*/
TEST_CASE("Test HCD control pipe STALL", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate some IRPs and initialize their data buffers with control transfers
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, TEST_DEV_ADDR, port_speed); //Create a default pipe (using a NULL EP descriptor)
    usb_irp_t *irp_list[NUM_IRPS];
    for (int i = 0; i < NUM_IRPS; i++) {
        irp_list[i] = test_hcd_alloc_irp(0, IRP_DATA_BUFF_SIZE);
        //Initialize with a "Get Config Descriptor request"
        irp_list[i]->num_bytes = TRANSFER_MAX_BYTES;
        USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *)irp_list[i]->data_buffer, 0, TRANSFER_MAX_BYTES);
        irp_list[i]->context = IRP_CONTEXT_VAL;
    }
    //Corrupt the first IRP so that it triggers a STALL
    ((usb_ctrl_req_t *)irp_list[0]->data_buffer)->bRequest = 0xAA;

    //Enqueue IRPs. A STALL should occur
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    printf("Expecting STALL\n");
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_ERROR_STALL);
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));

    //Call the pipe abort command to retire all IRPs then dequeue them all
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_ABORT));
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(irp_list[i], irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_STALL || irp->status == USB_TRANSFER_STATUS_CANCELED);
        if (irp->status == USB_TRANSFER_STATUS_COMPLETED) {
            TEST_ASSERT_GREATER_THAN(0, irp->actual_num_bytes);
        } else {
            TEST_ASSERT_EQUAL(0, irp->actual_num_bytes);
        }
        TEST_ASSERT_EQUAL(IRP_CONTEXT_VAL, irp->context);
    }

    //Call the clear command to un-stall the pipe
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_CLEAR));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));

    printf("Retrying\n");
    //Correct first IRP then requeue
    USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *)irp_list[0]->data_buffer, 0, TRANSFER_MAX_BYTES);
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }

    //Wait for each IRP to be done, deequeue, and check results
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_IRP_DONE);
        //expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_IRP_DONE);
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(irp_list[i], irp);
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, irp->status);
        TEST_ASSERT_GREATER_THAN(0, irp->actual_num_bytes);
        TEST_ASSERT_EQUAL(IRP_CONTEXT_VAL, irp->context);
    }

    //Free IRP list and pipe
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_free_irp(irp_list[i]);
    }
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}

/*
Test control pipe run-time halt and clear

Purpose:
    - Test that a control pipe can be halted with HCD_PIPE_CMD_HALT whilst there are ongoing IRPs
    - Test that a control pipe can be un-halted with a HCD_PIPE_CMD_CLEAR
    - Test that enqueued IRPs are resumed when pipe is un-halted

Procedure:
    - Setup HCD and wait for connection
    - Setup default pipe and allocate IRPs
    - Enqqueue IRPs but execute a HCD_PIPE_CMD_HALT command immediately after. Halt command should let on
      the current going IRP finish before actually halting the pipe.
    - Un-halt the pipe a HCD_PIPE_CMD_HALT command. Enqueued IRPs will be resumed
    - Check that all IRPs have completed successfully
    - Dequeue IRPs and teardown
*/
TEST_CASE("Test HCD control pipe runtime halt and clear", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate some IRPs and initialize their data buffers with control transfers
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, TEST_DEV_ADDR, port_speed); //Create a default pipe (using a NULL EP descriptor)
    usb_irp_t *irp_list[NUM_IRPS];
    for (int i = 0; i < NUM_IRPS; i++) {
        irp_list[i] = test_hcd_alloc_irp(0, IRP_DATA_BUFF_SIZE);
        //Initialize with a "Get Config Descriptor request"
        irp_list[i]->num_bytes = TRANSFER_MAX_BYTES;
        USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *)irp_list[i]->data_buffer, 0, TRANSFER_MAX_BYTES);
        irp_list[i]->context = IRP_CONTEXT_VAL;
    }

    //Enqueue IRPs but immediately halt the pipe
    printf("Enqueuing IRPs\n");
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_HALT));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));
    printf("Pipe halted\n");

    //Un-halt the pipe
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_CLEAR));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));
    printf("Pipe cleared\n");
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time pending for transfers to restart and complete

    //Wait for each IRP to be done, dequeue, and check results
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_IRP_DONE);
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(irp_list[i], irp);
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, irp->status);
        TEST_ASSERT_GREATER_THAN(0, irp->actual_num_bytes);
        TEST_ASSERT_EQUAL(IRP_CONTEXT_VAL, irp->context);
    }

    //Free IRP list and pipe
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_free_irp(irp_list[i]);
    }
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}
