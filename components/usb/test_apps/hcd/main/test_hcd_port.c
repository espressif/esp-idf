/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "esp_rom_sys.h"
#include "test_usb_common.h"
#include "test_hcd_common.h"

#define TEST_DEV_ADDR               0
#define NUM_URBS                    3
#define TRANSFER_MAX_BYTES          256
#define URB_DATA_BUFF_SIZE          (sizeof(usb_setup_packet_t) + TRANSFER_MAX_BYTES)   //256 is worst case size for configuration descriptors
#define POST_ENQUEUE_DELAY_US       10

/*
Test a port sudden disconnect and port recovery

Purpose: Test that when sudden disconnection happens on an HCD port, the port will
    - Generate the HCD_PORT_EVENT_SUDDEN_DISCONN and be put into the HCD_PORT_STATE_RECOVERY state
    - Pipes can be halted and flushed after a port error

Procedure:
    - Setup the HCD and a port
    - Trigger a port connection
    - Create a default pipe
    - Start transfers but trigger a disconnect after a short delay
    - Check that HCD_PORT_EVENT_SUDDEN_DISCONN event is generated. Handle that port event.
    - Check that the default pipe remains in the HCD_PIPE_STATE_ACTIVE after the port error.
    - Check that the default pipe can be halted.
    - Check that the default pipe can be flushed, a HCD_PIPE_EVENT_URB_DONE event should be generated
    - Check that all URBs can be dequeued.
    - Free default pipe
    - Recover the port
    - Trigger connection and disconnection again (to make sure the port works post recovery)
    - Teardown port and HCD
*/

TEST_CASE("Test HCD port sudden disconnect", "[port][low_speed][full_speed]")
{
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate some URBs and initialize their data buffers with control transfers
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, TEST_DEV_ADDR, port_speed); //Create a default pipe (using a NULL EP descriptor)
    urb_t *urb_list[NUM_URBS];
    for (int i = 0; i < NUM_URBS; i++) {
        urb_list[i] = test_hcd_alloc_urb(0, URB_DATA_BUFF_SIZE);
        //Initialize with a "Get Config Descriptor request"
        urb_list[i]->transfer.num_bytes = sizeof(usb_setup_packet_t) + TRANSFER_MAX_BYTES;
        USB_SETUP_PACKET_INIT_GET_CONFIG_DESC((usb_setup_packet_t *)urb_list[i]->transfer.data_buffer, 0, TRANSFER_MAX_BYTES);
        urb_list[i]->transfer.context = (void *)0xDEADBEEF;
    }

    //Enqueue URBs but immediately trigger a disconnect
    printf("Enqueuing URBs\n");
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb_list[i]));
    }
    //Add a short delay to let the transfers run for a bit
    esp_rom_delay_us(POST_ENQUEUE_DELAY_US);
    test_usb_set_phy_state(false, 0);
    //Disconnect event should have occurred. Handle the port event
    test_hcd_expect_port_event(port_hdl, HCD_PORT_EVENT_DISCONNECTION);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_DISCONNECTION, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_RECOVERY, hcd_port_get_state(port_hdl));
    printf("Sudden disconnect\n");

    //We should be able to halt then flush the pipe
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_HALT));
    printf("Pipe halted\n");
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_FLUSH));
    test_hcd_expect_pipe_event(default_pipe, HCD_PIPE_EVENT_URB_DONE);
    printf("Pipe flushed\n");

    //Dequeue URBs
    for (int i = 0; i < NUM_URBS; i++) {
        urb_t *urb = hcd_urb_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(urb_list[i], urb);
        TEST_ASSERT(urb->transfer.status == USB_TRANSFER_STATUS_COMPLETED || urb->transfer.status == USB_TRANSFER_STATUS_NO_DEVICE);
        if (urb->transfer.status == USB_TRANSFER_STATUS_COMPLETED) {
            //We must have transmitted at least the setup packet, but device may return less than bytes requested
            TEST_ASSERT_GREATER_OR_EQUAL(sizeof(usb_setup_packet_t), urb->transfer.actual_num_bytes);
            TEST_ASSERT_LESS_OR_EQUAL(urb->transfer.num_bytes, urb->transfer.actual_num_bytes);
        } else {
            //A failed transfer should 0 actual number of bytes transmitted
            TEST_ASSERT_EQUAL(0, urb->transfer.actual_num_bytes);
        }
        TEST_ASSERT_EQUAL(0xDEADBEEF, urb->transfer.context);
    }
    //Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
    }
    test_hcd_pipe_free(default_pipe);

    //Recover the port should return to the to NOT POWERED state
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_recover(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_NOT_POWERED, hcd_port_get_state(port_hdl));

    //Recovered port should be able to connect and disconnect again
    test_hcd_wait_for_conn(port_hdl);
    test_hcd_wait_for_disconn(port_hdl, false);
}

/*
Test port suspend and resume with active pipes

Purpose:
    - Test port suspend and resume procedure
    - When suspending, the pipes should be halted before suspending the port
    - When resuming, the pipes should remain in the halted state

Procedure:
    - Setup the HCD and a port
    - Trigger a port connection
    - Create a default pipe
    - Test that port can't be suspended with an active pipe
    - Halt the default pipe after a short delay
    - Suspend the port
    - Resume the port
    - Check that all the pipe is still halted
    - Cleanup default pipe
    - Trigger disconnection and teardown
*/
TEST_CASE("Test HCD port suspend and resume", "[port][low_speed][full_speed]")
{
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate some URBs and initialize their data buffers with control transfers
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, TEST_DEV_ADDR, port_speed); //Create a default pipe (using a NULL EP descriptor)

    //Test that suspending the port now fails as there is an active pipe
    TEST_ASSERT_NOT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_SUSPEND));

    //Halt the default pipe before suspending
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_HALT));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));

    //Suspend the port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_SUSPEND));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_SUSPENDED, hcd_port_get_state(port_hdl));
    printf("Suspended\n");
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for bus to remain suspended

    //Resume the port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_RESUME));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_ENABLED, hcd_port_get_state(port_hdl));
    printf("Resumed\n");

    //Clear the default pipe's halt
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_CLEAR));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for resumed URBs to complete

    test_hcd_pipe_free(default_pipe);
    //Cleanup
    test_hcd_wait_for_disconn(port_hdl, false);
}

/*
Test HCD port disable and disconnection

Purpose:
    - Test that the port disable command works correctly
    - Check that port can only be disabled when pipes have been halted
    - Check that a disconnection after port disable still triggers a HCD_PORT_EVENT_DISCONNECTION event

Procedure:
    - Setup HCD, a default pipe, and multiple URBs
    - Start transfers
    - Halt the default pipe after a short delay
    - Check that port can be disabled
    - Flush the default pipe and cleanup the default pipe
    - Check that a disconnection still works after disable
    - Teardown
*/
TEST_CASE("Test HCD port disable", "[port][low_speed][full_speed]")
{
    usb_speed_t port_speed = test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate some URBs and initialize their data buffers with control transfers
    hcd_pipe_handle_t default_pipe = test_hcd_pipe_alloc(port_hdl, NULL, TEST_DEV_ADDR, port_speed); //Create a default pipe (using a NULL EP descriptor)
    urb_t *urb_list[NUM_URBS];
    for (int i = 0; i < NUM_URBS; i++) {
        urb_list[i] = test_hcd_alloc_urb(0, URB_DATA_BUFF_SIZE);
        //Initialize with a "Get Config Descriptor request"
        urb_list[i]->transfer.num_bytes = sizeof(usb_setup_packet_t) + TRANSFER_MAX_BYTES;
        USB_SETUP_PACKET_INIT_GET_CONFIG_DESC((usb_setup_packet_t *)urb_list[i]->transfer.data_buffer, 0, TRANSFER_MAX_BYTES);
        urb_list[i]->transfer.context = (void *)0xDEADBEEF;
    }

    //Enqueue URBs but immediately disable the port
    printf("Enqueuing URBs\n");
    for (int i = 0; i < NUM_URBS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_urb_enqueue(default_pipe, urb_list[i]));
        //Add a short delay to let the transfers run for a bit
        esp_rom_delay_us(POST_ENQUEUE_DELAY_US);
    }

    //Halt the default pipe before suspending
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_HALT));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));

    //Check that port can be disabled
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_DISABLE));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISABLED, hcd_port_get_state(port_hdl));
    printf("Disabled\n");

    //Flush pipe
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_FLUSH));
    //Dequeue URBs
    for (int i = 0; i < NUM_URBS; i++) {
        urb_t *urb = hcd_urb_dequeue(default_pipe);
        TEST_ASSERT_EQUAL(urb_list[i], urb);
        TEST_ASSERT(urb->transfer.status == USB_TRANSFER_STATUS_COMPLETED ||    //The transfer completed before the pipe halted
                    urb->transfer.status == USB_TRANSFER_STATUS_CANCELED ||     //The transfer was stopped mid-way by the halt
                    urb->transfer.status == USB_TRANSFER_STATUS_NO_DEVICE);     //The transfer was never started
        if (urb->transfer.status == USB_TRANSFER_STATUS_COMPLETED) {
            //We must have transmitted at least the setup packet, but device may return less than bytes requested
            TEST_ASSERT_GREATER_OR_EQUAL(sizeof(usb_setup_packet_t), urb->transfer.actual_num_bytes);
            TEST_ASSERT_LESS_OR_EQUAL(urb->transfer.num_bytes, urb->transfer.actual_num_bytes);
        } else {
            //A failed transfer should 0 actual number of bytes transmitted
            TEST_ASSERT_EQUAL(0, urb->transfer.actual_num_bytes);
        }
        TEST_ASSERT_EQUAL(0xDEADBEEF, urb->transfer.context);
    }

    //Free URB list and pipe
    for (int i = 0; i < NUM_URBS; i++) {
        test_hcd_free_urb(urb_list[i]);
    }
    test_hcd_pipe_free(default_pipe);

    //Trigger a disconnection and cleanup
    test_hcd_wait_for_disconn(port_hdl, true);
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
    test_usb_set_phy_state(false, 0);
    vTaskDelay(portMAX_DELAY);  //Block forever and wait to be deleted
}

TEST_CASE("Test HCD port command bailout", "[port][low_speed][full_speed]")
{
    test_hcd_wait_for_conn(port_hdl);  //Trigger a connection
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Create task to run port commands concurrently
    SemaphoreHandle_t sync_sem = xSemaphoreCreateBinary();
    TaskHandle_t task_handle;
    TEST_ASSERT_NOT_NULL(sync_sem);
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(concurrent_task, "tsk", 4096, (void *) sync_sem, uxTaskPriorityGet(NULL) + 1, &task_handle, 0));

    //Suspend the device
    printf("Suspending\n");
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_SUSPEND));
    vTaskDelay(pdMS_TO_TICKS(20)); //Short delay for device to enter suspend state

    //Attempt to resume the port. But the concurrent task should override this with a disconnection event
    printf("Attempting to resume\n");
    xSemaphoreGive(sync_sem);   //Trigger concurrent task
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_RESPONSE, hcd_port_command(port_hdl, HCD_PORT_CMD_RESUME));

    //Check that concurrent task triggered a sudden disconnection
    test_hcd_expect_port_event(port_hdl, HCD_PORT_EVENT_DISCONNECTION);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_DISCONNECTION, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_RECOVERY, hcd_port_get_state(port_hdl));

    //Cleanup task and semaphore
    vTaskDelay(pdMS_TO_TICKS(10)); //Short delay for concurrent task finish running
    vTaskDelete(task_handle);
    vSemaphoreDelete(sync_sem);
}
