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
#define NUM_URBS                    3
#define TRANSFER_MAX_BYTES          256
#define URB_DATA_BUFF_SIZE          (sizeof(usb_ctrl_req_t) + TRANSFER_MAX_BYTES)   //256 is worst case size for configuration descriptors

/*
Test HCD control pipe URBs (normal completion and early abort)

Purpose:
    - Test that a control pipe can be created
    - URBs can be created and enqueued to the control pipe
    - Control pipe returns HCD_PIPE_EVENT_URB_DONE
    - Test that URBs can be aborted when enqueued

Procedure:
    - Setup HCD and wait for connection
    - Setup default pipe and allocate URBs
    - Enqueue URBs
    - Expect HCD_PIPE_EVENT_URB_DONE
    - Requeue URBs, but abort them immediately
    - Expect URB to be USB_TRANSFER_STATUS_CANCELED or USB_TRANSFER_STATUS_COMPLETED
    - Teardown
*/
TEST_CASE("Test HCD control pipe URBs", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate some URBs and initialize their data buffers with control transfers
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, TEST_DEV_ADDR, port_speed); //Create a default pipe (using a NULL EP descriptor)
    urb_t *urb_list[NUM_URBS];
    for (int i = 0; i < NUM_URBS; i++) {
        urb_list[i] = test_hcd_alloc_urb(0, URB_DATA_BUFF_SIZE);
        //Initialize with a "Get Config Descriptor request"
        urb_list[i]->transfer.num_bytes = TRANSFER_MAX_BYTES;
        USB_CTRL_REQ_INIT_GET_CONFIG_DESC((usb_ctrl_req_t *)urb_list[i]->transfer.data_buffer, 0, TRANSFER_MAX_BYTES);
        urb_list[i]->transfer.context = URB_CONTEXT_VAL;
    }

    //Enqueue URBs but immediately suspend the port
    printf("Enqueuing URBs\n");
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb_list[i]));
    }
    //Wait for each done event of each URB
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_URB_DONE);
    }
    //Dequeue URBs
    for (int i = 0; i < NUM_URBS; i++) {
        urb_t *urb = hcd_urb_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(urb_list[i], urb);
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status);
        TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
    }

    //Print config desc
    for (int i = 0; i < urb_list[0]->transfer.actual_num_bytes; i++) {
        printf("%d\t0x%x\n", i, urb_list[0]->transfer.data_buffer[sizeof(usb_ctrl_req_t) + i]);
    }


    //Enqueue URBs again but abort them short after
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb_list[i]));
    }
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_abort(urb_list[i]));
    }
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for any inflight transfers to complete

    //Wait for the URBs to complete and dequeue them, then check results
    //Dequeue URBs
    for (int i = 0; i < NUM_URBS; i++) {
        urb_t *urb = hcd_urb_dequeue(default_pipe);
        //No need to check for URB pointer address as they may be out of order
        TEST_ASSERT(urb->transfer.status == USB_TRANSFER_STATUS_COMPLETED || urb->transfer.status == USB_TRANSFER_STATUS_CANCELED);
        if (urb->transfer.status == USB_TRANSFER_STATUS_COMPLETED) {
            TEST_ASSERT_GREATER_THAN(0, urb->transfer.actual_num_bytes);
        } else {
            TEST_ASSERT_EQUAL(0, urb->transfer.actual_num_bytes);
        }
        TEST_ASSERT_EQUAL(urb->transfer.context, URB_CONTEXT_VAL);
    }

    //Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
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
    - The HCD_PIPE_CMD_ABORT can retire all URBs
    - Pipe clear command can return the pipe to being active

Procedure:
    - Setup HCD and wait for connection
    - Setup default pipe and allocate URBs
    - Corrupt the first URB so that it will trigger a STALL, then enqueue all the URBs
    - Check that a HCD_PIPE_EVENT_ERROR_STALL event is triggered
    - Check that all URBs can be retired using HCD_PIPE_CMD_ABORT
    - Check that the STALL can be cleared by using HCD_PIPE_CMD_CLEAR
    - Fix the corrupt first URB and retry the URBs
    - Dequeue URBs
    - Teardown
*/
TEST_CASE("Test HCD control pipe STALL", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate some URBs and initialize their data buffers with control transfers
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, TEST_DEV_ADDR, port_speed); //Create a default pipe (using a NULL EP descriptor)
    urb_t *urb_list[NUM_URBS];
    for (int i = 0; i < NUM_URBS; i++) {
        urb_list[i] = test_hcd_alloc_urb(0, URB_DATA_BUFF_SIZE);
        //Initialize with a "Get Config Descriptor request"
        urb_list[i]->transfer.num_bytes = TRANSFER_MAX_BYTES;
        USB_CTRL_REQ_INIT_GET_CONFIG_DESC((usb_ctrl_req_t *)urb_list[i]->transfer.data_buffer, 0, TRANSFER_MAX_BYTES);
        urb_list[i]->transfer.context = URB_CONTEXT_VAL;
    }
    //Corrupt the first URB so that it triggers a STALL
    ((usb_ctrl_req_t *)urb_list[0]->transfer.data_buffer)->bRequest = 0xAA;

    //Enqueue URBs. A STALL should occur
    int num_enqueued = 0;
    for (int i = 0; i < NUM_URBS; i++) {
        if (hcd_urb_enqueue(default_pipe, urb_list[i]) != ESP_OK)  {
            //STALL may occur before we are done enqueing
            break;
        }
        num_enqueued++;
    }
    TEST_ASSERT_GREATER_THAN(0, num_enqueued);
    printf("Expecting STALL\n");
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_ERROR_STALL);
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));

    //Call the pipe abort command to retire all URBs then dequeue them all
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_ABORT));
    for (int i = 0; i < num_enqueued; i++) {
        urb_t *urb = hcd_urb_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(urb_list[i], urb);
        TEST_ASSERT(urb->transfer.status == USB_TRANSFER_STATUS_STALL || urb->transfer.status == USB_TRANSFER_STATUS_CANCELED);
        if (urb->transfer.status == USB_TRANSFER_STATUS_COMPLETED) {
            TEST_ASSERT_GREATER_THAN(0, urb->transfer.actual_num_bytes);
        } else {
            TEST_ASSERT_EQUAL(0, urb->transfer.actual_num_bytes);
        }
        TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
    }

    //Call the clear command to un-stall the pipe
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_CLEAR));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));

    printf("Retrying\n");
    //Correct first URB then requeue
    USB_CTRL_REQ_INIT_GET_CONFIG_DESC((usb_ctrl_req_t *)urb_list[0]->transfer.data_buffer, 0, TRANSFER_MAX_BYTES);
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb_list[i]));
    }

    //Wait for each URB to be done, deequeue, and check results
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_URB_DONE);
        //expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_URB_DONE);
        urb_t *urb = hcd_urb_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(urb_list[i], urb);
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status);
        TEST_ASSERT_GREATER_THAN(0, urb->transfer.actual_num_bytes);
        TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
    }

    //Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
    }
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}

/*
Test control pipe run-time halt and clear

Purpose:
    - Test that a control pipe can be halted with HCD_PIPE_CMD_HALT whilst there are ongoing URBs
    - Test that a control pipe can be un-halted with a HCD_PIPE_CMD_CLEAR
    - Test that enqueued URBs are resumed when pipe is un-halted

Procedure:
    - Setup HCD and wait for connection
    - Setup default pipe and allocate URBs
    - Enqqueue URBs but execute a HCD_PIPE_CMD_HALT command immediately after. Halt command should let on
      the current going URB finish before actually halting the pipe.
    - Un-halt the pipe a HCD_PIPE_CMD_HALT command. Enqueued URBs will be resumed
    - Check that all URBs have completed successfully
    - Dequeue URBs and teardown
*/
TEST_CASE("Test HCD control pipe runtime halt and clear", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate some URBs and initialize their data buffers with control transfers
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, TEST_DEV_ADDR, port_speed); //Create a default pipe (using a NULL EP descriptor)
    urb_t *urb_list[NUM_URBS];
    for (int i = 0; i < NUM_URBS; i++) {
        urb_list[i] = test_hcd_alloc_urb(0, URB_DATA_BUFF_SIZE);
        //Initialize with a "Get Config Descriptor request"
        urb_list[i]->transfer.num_bytes = TRANSFER_MAX_BYTES;
        USB_CTRL_REQ_INIT_GET_CONFIG_DESC((usb_ctrl_req_t *)urb_list[i]->transfer.data_buffer, 0, TRANSFER_MAX_BYTES);
        urb_list[i]->transfer.context = URB_CONTEXT_VAL;
    }

    //Enqueue URBs but immediately halt the pipe
    printf("Enqueuing URBs\n");
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb_list[i]));
    }
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_HALT));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));
    printf("Pipe halted\n");

    //Un-halt the pipe
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_CLEAR));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));
    printf("Pipe cleared\n");
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time pending for transfers to restart and complete

    //Wait for each URB to be done, dequeue, and check results
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_URB_DONE);
        urb_t *urb = hcd_urb_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(urb_list[i], urb);
        TEST_ASSERT_EQUAL(USB_TRANSFER_STATUS_COMPLETED, urb->transfer.status);
        TEST_ASSERT_GREATER_THAN(0, urb->transfer.actual_num_bytes);
        TEST_ASSERT_EQUAL(URB_CONTEXT_VAL, urb->transfer.context);
    }

    //Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
    }
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}
