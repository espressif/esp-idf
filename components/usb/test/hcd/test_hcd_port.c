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
Test a port sudden disconnect and port recovery

Purpose: Test that when sudden disconnection happens on an HCD port, the port will
    - Generate the HCD_PORT_EVENT_SUDDEN_DISCONN and be put into the HCD_PORT_STATE_RECOVERY state
    - Ongoing IRPs and pipes are handled correctly

Procedure:
    - Setup the HCD and a port
    - Trigger a port connection
    - Create a default pipe
    - Start transfers but immediately trigger a disconnect
    - Check that HCD_PORT_EVENT_SUDDEN_DISCONN event is generated. Handle the event.
    - Check that default pipe received a HCD_PIPE_EVENT_INVALID event. Pipe state should be invalid. Dequeue IRPs
    - Free default pipe
    - Recover the port
    - Trigger connection and disconnection again (to make sure the port works post recovery)
    - Teardown port and HCD
*/

TEST_CASE("Test HCD port sudden disconnect", "[hcd][ignore]")
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
        irp_list[i]->context = (void *)0xDEADBEEF;
    }

    //Enqueue IRPs but immediately trigger a disconnect
    printf("Enqueuing IRPs\n");
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    test_hcd_force_conn_state(false, 0);
    //Disconnect event should have occurred. Handle the event
    test_hcd_expect_port_event(port_hdl, HCD_PORT_EVENT_SUDDEN_DISCONN);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_SUDDEN_DISCONN, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_RECOVERY, hcd_port_get_state(port_hdl));
    printf("Sudden disconnect\n");

    //Pipe should have received (zero or more HCD_PIPE_EVENT_IRP_DONE) followed by a HCD_PIPE_EVENT_INVALID (MUST OCCUR)
    int num_pipe_events = test_hcd_get_num_pipe_events(default_pipe);
    for (int i = 0; i < num_pipe_events - 1; i++) {
        test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_IRP_DONE);
    }
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_INVALID);
    TEST_ASSERT_EQUAL(hcd_pipe_get_state(default_pipe), HCD_PIPE_STATE_INVALID);

    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(irp_list[i], irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED || irp->status == USB_TRANSFER_STATUS_NO_DEVICE);
        if (irp->status == USB_TRANSFER_STATUS_COMPLETED) {
            TEST_ASSERT_GREATER_THAN(0, irp->actual_num_bytes);
        } else {
            TEST_ASSERT_EQUAL(0, irp->actual_num_bytes);
        }
        TEST_ASSERT_EQUAL(0xDEADBEEF, irp->context);
    }
    //Free IRP list and pipe
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_free_irp(irp_list[i]);
    }
    test_hcd_pipe_free(default_pipe);

    //Recover the port should return to the to NOT POWERED state
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_recover(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_NOT_POWERED, hcd_port_get_state(port_hdl));

    //Recovered port should be able to connect and disconnect again
    test_hcd_wait_for_conn(port_hdl);
    test_hcd_wait_for_disconn(port_hdl, false);
    test_hcd_teardown(port_hdl);
}

/*
Test port suspend and resume with active pipes

Purpose:
    - Test port suspend and resume commands work correctly whilst there are active pipes with ongoing transfers
    - When suspending, the pipes should be allowed to finish their current ongoing transfer before the bus is suspended.
    - When resuming, pipes with pending transfer should be started after the bus is resumed.

Procedure:
    - Setup the HCD and a port
    - Trigger a port connection
    - Create a default pipe
    - Start transfers but suspend the port immediately
    - Resume the port
    - Check that all the IRPs have completed successfully
    - Cleanup IRPs and default pipe
    - Trigger disconnection and teardown
*/
TEST_CASE("Test HCD port suspend and resume", "[hcd][ignore]")
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
        irp_list[i]->context = (void *)0xDEADBEEF;
    }

    //Enqueue IRPs but immediately suspend the port
    printf("Enqueuing IRPs\n");
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_SUSPEND));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_SUSPENDED, hcd_port_get_state(port_hdl));
    printf("Suspended\n");
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for bus to remain suspended

    //Resume the port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_RESUME));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_ENABLED, hcd_port_get_state(port_hdl));
    printf("Resumed\n");
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for resumed IRPs to complete
    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(irp_list[i], irp);
        TEST_ASSERT_EQUAL(irp->status, USB_TRANSFER_STATUS_COMPLETED);
        TEST_ASSERT_GREATER_THAN(0, irp->actual_num_bytes);
        TEST_ASSERT_EQUAL(0xDEADBEEF, irp->context);
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
Test HCD port disable with active pipes

Purpose:
    - Test that the port disable command works correctly with active pipes
    - Pipes should be to finish their current ongoing transfer before port is disabled
    - After disabling the port, all pipes should become invalid.

Procedure:
    - Setup HCD, a default pipe, and multiple IRPs
    - Start transfers but immediately disable the port
    - Check pipe received invalid event
    - Check that transfer are either done or not executed
    - Teardown
*/
TEST_CASE("Test HCD port disable", "[hcd][ignore]")
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
        irp_list[i]->context = (void *)0xDEADBEEF;
    }

    //Enqueue IRPs but immediately disable the port
    printf("Enqueuing IRPs\n");
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_DISABLE));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISABLED, hcd_port_get_state(port_hdl));
    printf("Disabled\n");

    //Pipe should have received (zero or more HCD_PIPE_EVENT_IRP_DONE) followed by a HCD_PIPE_EVENT_INVALID (MUST OCCUR)
    int num_pipe_events = test_hcd_get_num_pipe_events(default_pipe);
    for (int i = 0; i < num_pipe_events - 1; i++) {
        test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_IRP_DONE);
    }
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_INVALID);

    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(irp_list[i], irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED || irp->status == USB_TRANSFER_STATUS_NO_DEVICE);
        if (irp->status == USB_TRANSFER_STATUS_COMPLETED) {
            TEST_ASSERT_GREATER_THAN(0, irp->actual_num_bytes);
        } else {
            TEST_ASSERT_EQUAL(0, irp->actual_num_bytes);
        }
        TEST_ASSERT_EQUAL(0xDEADBEEF, irp->context);
    }

    //Free IRP list and pipe
    for (int i = 0; i < NUM_IRPS; i++) {
        test_hcd_free_irp(irp_list[i]);
    }
    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, true);
    test_hcd_teardown(port_hdl);
}

/*
Test HCD port command bailout

Purpose:
    - Test that if the a port's state changes whilst a command is being executed, the port command should return
       ESP_ERR_INVALID_RESPONSE

Procedure:
    - Setup HCD and wait for connection
    - Suspend the port
    - Resume the port but trigger a disconnect from another thread during the resume command
    - Check that port command returns ESP_ERR_INVALID_RESPONSE
*/

static void concurrent_task(void *arg)
{
    SemaphoreHandle_t sync_sem = (SemaphoreHandle_t) arg;
    xSemaphoreTake(sync_sem, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(10));  //Give a short delay let reset command start in main thread
    //Force a disconnection
    test_hcd_force_conn_state(false, 0);
    vTaskDelay(portMAX_DELAY);  //Block forever and wait to be deleted
}

TEST_CASE("Test HCD port command bailout", "[hcd][ignore]")
{
    hcd_port_handle_t port_hdl = test_hcd_setup();  //Setup the HCD and port
    test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Create task to run port commands concurrently
    SemaphoreHandle_t sync_sem = xSemaphoreCreateBinary();
    TaskHandle_t task_handle;
    TEST_ASSERT_NOT_EQUAL(NULL, sync_sem);
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(concurrent_task, "tsk", 4096, (void *) sync_sem, UNITY_FREERTOS_PRIORITY + 1, &task_handle, 0));

    //Suspend the device
    printf("Suspending\n");
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_SUSPEND));
    vTaskDelay(pdMS_TO_TICKS(20)); //Short delay for device to enter suspend state

    //Attempt to resume the port. But the concurrent task should override this with a disconnection event
    printf("Attempting to resume\n");
    xSemaphoreGive(sync_sem);   //Trigger concurrent task
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_RESPONSE, hcd_port_command(port_hdl, HCD_PORT_CMD_RESUME));

    //Check that concurrent task triggered a sudden disconnection
    test_hcd_expect_port_event(port_hdl, HCD_PORT_EVENT_SUDDEN_DISCONN);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_SUDDEN_DISCONN, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_RECOVERY, hcd_port_get_state(port_hdl));

    //Cleanup task and semaphore
    vTaskDelay(pdMS_TO_TICKS(10)); //Short delay for concurrent task finish running
    vTaskDelete(task_handle);
    vSemaphoreDelete(sync_sem);

    test_hcd_teardown(port_hdl);
}
