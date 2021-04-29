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
#include "soc/gpio_pins.h"
#include "soc/gpio_sig_map.h"
#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_rom_gpio.h"
#include "hal/usbh_ll.h"
#include "usb.h"
#include "hcd.h"

// -------------------------------------------------- PHY Control ------------------------------------------------------

static void phy_force_conn_state(bool connected, TickType_t delay_ticks)
{
    vTaskDelay(delay_ticks);
    usb_wrap_dev_t *wrap = &USB_WRAP;
    if (connected) {
        //Swap back to internal PHY that is connected to a device
        wrap->otg_conf.phy_sel = 0;
    } else {
        //Set external PHY input signals to fixed voltage levels mimicking a disconnected state
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_EXTPHY_VP_IDX, false);
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_EXTPHY_VM_IDX, false);
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, USB_EXTPHY_RCV_IDX, false);
        //Swap to the external PHY
        wrap->otg_conf.phy_sel = 1;
    }
}

// ------------------------------------------------ Helper Functions ---------------------------------------------------

#define EVENT_QUEUE_LEN         5
#define NUM_IRPS                3
#define TRANSFER_DATA_MAX_BYTES 256     //Just assume that will only IN/OUT 256 bytes for now
#define PORT_NUM                1
#define IRP_CONTEXT_VAL         ((void *)0xDEADBEEF)    //Conext value for created IRPs

typedef struct {
    hcd_port_handle_t port_hdl;
    hcd_port_event_t port_event;
} port_event_msg_t;

typedef struct {
    hcd_pipe_handle_t pipe_hdl;
    hcd_pipe_event_t pipe_event;
} pipe_event_msg_t;

static bool port_callback(hcd_port_handle_t port_hdl, hcd_port_event_t port_event, void *user_arg, bool in_isr)
{
    QueueHandle_t port_evt_queue = (QueueHandle_t)user_arg;
    TEST_ASSERT(in_isr);    //Current HCD implementation should never call a port callback in a task context
    port_event_msg_t msg = {
        .port_hdl = port_hdl,
        .port_event = port_event,
    };
    BaseType_t xTaskWoken = pdFALSE;
    xQueueSendFromISR(port_evt_queue, &msg, &xTaskWoken);
    return (xTaskWoken == pdTRUE);
}

static bool pipe_callback(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr)
{
    QueueHandle_t pipe_evt_queue = (QueueHandle_t)user_arg;
    pipe_event_msg_t msg = {
        .pipe_hdl = pipe_hdl,
        .pipe_event = pipe_event,
    };
    if (in_isr) {
        BaseType_t xTaskWoken = pdFALSE;
        xQueueSendFromISR(pipe_evt_queue, &msg, &xTaskWoken);
        return (xTaskWoken == pdTRUE);
    } else {
        xQueueSend(pipe_evt_queue, &msg, portMAX_DELAY);
        return false;
    }
}

static void expect_port_event(QueueHandle_t port_evt_queue, hcd_port_handle_t expected_hdl, hcd_port_event_t expected_event)
{
    port_event_msg_t msg;
    xQueueReceive(port_evt_queue, &msg, portMAX_DELAY);
    TEST_ASSERT_EQUAL(expected_hdl, msg.port_hdl);
    TEST_ASSERT_EQUAL(expected_event, msg.port_event);
    printf("\t-> Port event\n");
}

static void expect_pipe_event(QueueHandle_t pipe_evt_queue, hcd_pipe_handle_t expected_hdl, hcd_pipe_event_t expected_event)
{
    pipe_event_msg_t msg;
    xQueueReceive(pipe_evt_queue, &msg, portMAX_DELAY);
    TEST_ASSERT_EQUAL(expected_hdl, msg.pipe_hdl);
    TEST_ASSERT_EQUAL(expected_event, msg.pipe_event);
}

/**
 * @brief Creates port and pipe event queues. Sets up the HCD, and initializes a port.
 *
 * @param[out] port_evt_queue Port event queue
 * @param[out] pipe_evt_queue Pipe event queue
 * @param[out] port_hdl Port handle
 */
static void setup(QueueHandle_t *port_evt_queue, QueueHandle_t *pipe_evt_queue, hcd_port_handle_t *port_hdl)
{
    *port_evt_queue = xQueueCreate(EVENT_QUEUE_LEN, sizeof(port_event_msg_t));
    *pipe_evt_queue = xQueueCreate(EVENT_QUEUE_LEN, sizeof(pipe_event_msg_t));
    TEST_ASSERT_NOT_EQUAL(NULL, *port_evt_queue);
    TEST_ASSERT_NOT_EQUAL(NULL, *pipe_evt_queue);

    //Install HCD
    hcd_config_t config = {
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    TEST_ASSERT_EQUAL(ESP_OK, hcd_install(&config));
    //Initialize a port
    hcd_port_config_t port_config = {
        .callback = port_callback,
        .callback_arg = (void *)*port_evt_queue,
        .context = NULL,
    };
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_init(PORT_NUM, &port_config, port_hdl));
    TEST_ASSERT_NOT_EQUAL(NULL, *port_hdl);
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_NOT_POWERED, hcd_port_get_state(*port_hdl));
    phy_force_conn_state(false, 0);    //Force disconnected state on PHY
}

/**
 * @brief Deinitializes the port, uninstalls HCD, and frees port and pipe event queues
 *
 * @param[in] port_evt_queue Port event queue
 * @param[in] pipe_evt_queue Pipe event semaphore
 * @param[in] port_hdl Port handle
 */
static void teardown(QueueHandle_t port_evt_queue, QueueHandle_t pipe_evt_queue, hcd_port_handle_t port_hdl)
{
    //Deinitialize a port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_deinit(port_hdl));
    //Uninstall the HCD
    TEST_ASSERT_EQUAL(ESP_OK, hcd_uninstall());
    vQueueDelete(port_evt_queue);
    vQueueDelete(pipe_evt_queue);
}

/**
 * @brief Powers ON a port and waits for a connection, then resets the connected device
 *
 * @param port_hdl Port handle
 * @param port_evt_queue Port event queue
 */
static void wait_for_connection(hcd_port_handle_t port_hdl, QueueHandle_t port_evt_queue)
{
    //Power ON the port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_POWER_ON));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISCONNECTED, hcd_port_get_state(port_hdl));
    //Wait for connection event
    printf("Waiting for conenction\n");
    phy_force_conn_state(true, pdMS_TO_TICKS(100));     //Allow for connected state on PHY
    expect_port_event(port_evt_queue, port_hdl, HCD_PORT_EVENT_CONNECTION);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_CONNECTION, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISABLED, hcd_port_get_state(port_hdl));
    //Reset newly connected device
    printf("Resetting\n");
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_RESET));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_ENABLED, hcd_port_get_state(port_hdl));
    //Get speed of conencted
    usb_speed_t port_speed;
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_get_speed(port_hdl, &port_speed));
    if (port_speed == USB_SPEED_FULL) {
        printf("Full speed enabled\n");
    } else {
        printf("Low speed enabled\n");
    }
}

/**
 * @brief Disables the port, waits for a disconnection, then powers OFF the port
 *
 * @param port_hdl Port handle
 * @param port_evt_queue Port event queue
 * @param already_disabled If the port is already disabled, it will skip disabling the port
 */
static void wait_for_disconnection(hcd_port_handle_t port_hdl, QueueHandle_t port_evt_queue, bool already_disabled)
{
    if (!already_disabled) {
        //Disable the device
        printf("Disabling\n");
        TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_DISABLE));
        TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISABLED, hcd_port_get_state(port_hdl));
    }
    //Wait for a safe disconnect
    printf("Waiting for disconnection\n");
    phy_force_conn_state(false, pdMS_TO_TICKS(100));    //Force disconnected state on PHY
    expect_port_event(port_evt_queue, port_hdl, HCD_PORT_EVENT_DISCONNECTION);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_DISCONNECTION, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISCONNECTED, hcd_port_get_state(port_hdl));
    //Power down the port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_POWER_OFF));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_NOT_POWERED, hcd_port_get_state(port_hdl));
}

static void alloc_pipe_and_irp_list(hcd_port_handle_t port_hdl,
                                    QueueHandle_t pipe_evt_queue,
                                    int num_irps,
                                    hcd_pipe_handle_t *pipe_hdl,
                                    usb_irp_t ***irp_list)
{
    //We don't support hubs yet. Just get the speed of the port to determine the speed of the device
    usb_speed_t port_speed;
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_get_speed(port_hdl, &port_speed));

    //Create default pipe
    printf("Creating default pipe\n");
    hcd_pipe_config_t config = {
        .callback = pipe_callback,
        .callback_arg = (void *)pipe_evt_queue,
        .context = NULL,
        .ep_desc = NULL,    //NULL EP descriptor to create a default pipe
        .dev_addr = 0,
        .dev_speed = port_speed,
    };
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_alloc(port_hdl, &config, pipe_hdl));
    TEST_ASSERT_NOT_EQUAL(NULL, *pipe_hdl);
    //Create IRPs and their data buffers
    printf("Creating IRPs and IRP list\n");
    *irp_list = heap_caps_malloc(sizeof(usb_irp_t *) * num_irps, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_EQUAL(NULL, *irp_list);
    for (int i = 0; i < num_irps; i++) {
        //Allocate IRP
        usb_irp_t *irp = heap_caps_calloc(1, sizeof(usb_irp_t), MALLOC_CAP_DEFAULT);
        TEST_ASSERT_NOT_EQUAL(NULL, irp);
        //Allocate data buffer
        uint8_t *data_buffer = heap_caps_malloc(sizeof(usb_ctrl_req_t) + TRANSFER_DATA_MAX_BYTES, MALLOC_CAP_DMA);
        TEST_ASSERT_NOT_EQUAL(NULL, data_buffer);
        //Initialize IRP and IRP list
        irp->data_buffer = data_buffer;
        irp->num_iso_packets = 0;
        (*irp_list)[i] = irp;
    }
}

static void free_pipe_and_irp_list(hcd_pipe_handle_t pipe_hdl,
                                   int num_irps,
                                   usb_irp_t **irp_list)
{
    printf("Freeing IRPs and IRP list\n");
    for (int i = 0; i < num_irps; i++) {
        usb_irp_t *irp = irp_list[i] ;
        //Free data buffer
        heap_caps_free(irp->data_buffer);
        heap_caps_free(irp);
    }
    heap_caps_free(irp_list);
    printf("Freeing default pipe\n");
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_free(pipe_hdl));
}

// ------------------------------------------------ Host Port Tests ----------------------------------------------------

/*
Test a port sudden disconnect and port recovery

Purpose: Test that when sudden disconnection happens on an HCD port, the port will
    - Generate the HCD_PORT_EVENT_SUDDEN_DISCONN and be put into the HCD_PORT_STATE_RECOVERY state
    - Ongoing IRPs and pipes are handled correctly

Procedure:
    - Setup HCD, a default pipe, and multiple IRPs
    - Start transfers but immediately trigger a disconnect
    - Check that HCD_PORT_EVENT_SUDDEN_DISCONN event is generated
    - Check that default pipe is invalid and IRPs can be dequeued
    - Recover the port and try to connect then disconnect again (to make sure the port works port recovery)
    - Teardown HCD
*/
TEST_CASE("Test HCD port sudden disconnect", "[hcd][ignore]")
{
    QueueHandle_t port_evt_queue;
    QueueHandle_t pipe_evt_queue;
    hcd_port_handle_t port_hdl;
    setup(&port_evt_queue, &pipe_evt_queue, &port_hdl);
    wait_for_connection(port_hdl, port_evt_queue);
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate default pipe and IRPs
    hcd_pipe_handle_t default_pipe;
    usb_irp_t **irp_list;
    alloc_pipe_and_irp_list(port_hdl, pipe_evt_queue, NUM_IRPS, &default_pipe, &irp_list);

    //Initialize IRPs to send a "Get Device Descriptor" request
    for (int i = 0; i < NUM_IRPS; i++) {
        irp_list[i]->num_bytes = 64;    //1 worst case MPS
        USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *) irp_list[i]->data_buffer, 0, TRANSFER_DATA_MAX_BYTES);
        irp_list[i]->context = IRP_CONTEXT_VAL;
    }

    //Enqueue those IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    phy_force_conn_state(false, 0);    //Force disconnected state on PHY

    expect_port_event(port_evt_queue, port_hdl, HCD_PORT_EVENT_SUDDEN_DISCONN);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_SUDDEN_DISCONN, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_RECOVERY, hcd_port_get_state(port_hdl));
    printf("Sudden disconnect\n");

    //Handling the disconnection event should have invalidated all pipes.
    //Pipe should have received (zero or more HCD_PIPE_EVENT_IRP_DONE) followed by a HCD_PIPE_EVENT_INVALID (MUST OCCUR)
    int num_pipe_events = EVENT_QUEUE_LEN - uxQueueSpacesAvailable(pipe_evt_queue);
    for (int i = 0; i < num_pipe_events - 1; i++) {
        expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_IRP_DONE);
    }
    expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_INVALID);
    TEST_ASSERT_EQUAL(hcd_pipe_get_state(default_pipe), HCD_PIPE_STATE_INVALID);

    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_NOT_EQUAL(NULL, irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED || irp->status == USB_TRANSFER_STATUS_NO_DEVICE);
        TEST_ASSERT(irp->context == IRP_CONTEXT_VAL);
    }
    //Free IRPs
    free_pipe_and_irp_list(default_pipe, NUM_IRPS, irp_list);

    //Recover the port should return to the to NOT POWERED state
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_recover(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_NOT_POWERED, hcd_port_get_state(port_hdl));

    //Recovered port should be able to connect and disconnect again
    wait_for_connection(port_hdl, port_evt_queue);
    wait_for_disconnection(port_hdl, port_evt_queue, false);
    teardown(port_evt_queue, pipe_evt_queue, port_hdl);
}

/*
Test port suspend and resume with active pipes

Purpose:
    - Test port suspend and resume commands work correctly whilst there are active pipes with ongoing transfers
    - When suspending, the pipes should be allowed to finish their current ongoing transfer before the bus is suspended.
    - When resuming, pipes with pending transfer should be started after the bus is resumed.

Procedure:
    - Setup HCD, a port, a default pipe, and multiple IRPS
    - Start transfers but immediately suspend the port
    - Resume the port
    - Check all IRPs have also be resumed and completed on port resume
    - Teardown
*/
TEST_CASE("Test HCD port suspend and resume", "[hcd][ignore]")
{
    QueueHandle_t port_evt_queue;
    QueueHandle_t pipe_evt_queue;
    hcd_port_handle_t port_hdl;
    setup(&port_evt_queue, &pipe_evt_queue, &port_hdl);
    wait_for_connection(port_hdl, port_evt_queue);
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate default pipe and IRPs
    hcd_pipe_handle_t default_pipe;
    usb_irp_t **irp_list;
    alloc_pipe_and_irp_list(port_hdl, pipe_evt_queue, NUM_IRPS, &default_pipe, &irp_list);

    //Initialize IRPs to send a "Get Device Descriptor" request
    for (int i = 0; i < NUM_IRPS; i++) {
        irp_list[i]->num_bytes = 64;    //1 worst case MPS
        USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *)irp_list[i]->data_buffer, 0, TRANSFER_DATA_MAX_BYTES);
        irp_list[i]->context = IRP_CONTEXT_VAL;
    }

    //Enqueue those IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }

    //Immediately suspend the bus whilst pies are active
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_SUSPEND));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_SUSPENDED, hcd_port_get_state(port_hdl));
    printf("Suspended\n");
    vTaskDelay(pdMS_TO_TICKS(100));

    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_RESUME));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_ENABLED, hcd_port_get_state(port_hdl));
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for resumed transfers to complete

    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_IRP_DONE);
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_NOT_EQUAL(NULL, irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED);
        TEST_ASSERT(irp->context == IRP_CONTEXT_VAL);
    }
    //Free IRPs
    free_pipe_and_irp_list(default_pipe, NUM_IRPS, irp_list);

    //Cleanup
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)
    wait_for_disconnection(port_hdl, port_evt_queue, false);
    teardown(port_evt_queue, pipe_evt_queue, port_hdl);
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
    QueueHandle_t port_evt_queue;
    QueueHandle_t pipe_evt_queue;
    hcd_port_handle_t port_hdl;
    setup(&port_evt_queue, &pipe_evt_queue, &port_hdl);
    wait_for_connection(port_hdl, port_evt_queue);
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate default pipe and IRPs
    hcd_pipe_handle_t default_pipe;
    usb_irp_t **irp_list;
    alloc_pipe_and_irp_list(port_hdl, pipe_evt_queue, NUM_IRPS, &default_pipe, &irp_list);

    //Initialize IRPs to send a "Get Device Descriptor" request
    for (int i = 0; i < NUM_IRPS; i++) {
        irp_list[i]->num_bytes = 64;    //1 worst case MPS
        USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *) irp_list[i]->data_buffer, 0, TRANSFER_DATA_MAX_BYTES);
        irp_list[i]->context = IRP_CONTEXT_VAL;
    }

    //Enqueue those IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    //Immediately disable port
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_DISABLE));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_DISABLED, hcd_port_get_state(port_hdl));
    printf("Disabled\n");

    //Pipe should have received (zero or more HCD_PIPE_EVENT_IRP_DONE) followed by a HCD_PIPE_EVENT_INVALID (MUST OCCUR)
    int num_pipe_events = EVENT_QUEUE_LEN - uxQueueSpacesAvailable(pipe_evt_queue);
    for (int i = 0; i < num_pipe_events - 1; i++) {
        expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_IRP_DONE);
    }
    expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_INVALID);

    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_NOT_EQUAL(NULL, irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED || irp->status == USB_TRANSFER_STATUS_NO_DEVICE);
        TEST_ASSERT(irp->context == IRP_CONTEXT_VAL);
    }
    //Free IRPs
    free_pipe_and_irp_list(default_pipe, NUM_IRPS, irp_list);

    //Already disabled. Disconnect and teardown
    wait_for_disconnection(port_hdl, port_evt_queue, true);
    teardown(port_evt_queue, pipe_evt_queue, port_hdl);
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
    //Forcibly a disconnection
    phy_force_conn_state(false, 0);
    vTaskDelay(portMAX_DELAY);  //Block forever and wait to be deleted
}

TEST_CASE("Test HCD port command bailout", "[hcd][ignore]")
{
    QueueHandle_t port_evt_queue;
    QueueHandle_t pipe_evt_queue;
    hcd_port_handle_t port_hdl;
    setup(&port_evt_queue, &pipe_evt_queue, &port_hdl);
    wait_for_connection(port_hdl, port_evt_queue);
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Create task to run commands concurrently
    SemaphoreHandle_t sync_sem = xSemaphoreCreateBinary();
    TaskHandle_t task_handle;
    TEST_ASSERT_NOT_EQUAL(NULL, sync_sem);
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(concurrent_task, "tsk", 4096, (void *) sync_sem, UNITY_FREERTOS_PRIORITY + 1, &task_handle, 0));

    //Suspend the device
    printf("Suspending\n");
    TEST_ASSERT_EQUAL(ESP_OK, hcd_port_command(port_hdl, HCD_PORT_CMD_SUSPEND));
    vTaskDelay(pdMS_TO_TICKS(20)); //Short delay for device to enter suspend state

    printf("Attempting to resume\n");
    xSemaphoreGive(sync_sem);   //Trigger concurrent task
    //Attempt to resume the port. But the concurrent task should override this with a disconnection event
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_RESPONSE, hcd_port_command(port_hdl, HCD_PORT_CMD_RESUME));

    //Check that concurrent task triggered a sudden disconnection
    expect_port_event(port_evt_queue, port_hdl, HCD_PORT_EVENT_SUDDEN_DISCONN);
    TEST_ASSERT_EQUAL(HCD_PORT_EVENT_SUDDEN_DISCONN, hcd_port_handle_event(port_hdl));
    TEST_ASSERT_EQUAL(HCD_PORT_STATE_RECOVERY, hcd_port_get_state(port_hdl));

    vTaskDelay(pdMS_TO_TICKS(10)); //Short delay for concurrent task finish running
    vTaskDelete(task_handle);
    vSemaphoreDelete(sync_sem);
    //Directly teardown the port without recovery
    teardown(port_evt_queue, pipe_evt_queue, port_hdl);
}

// --------------------------------------------------- Pipe Tests ------------------------------------------------------

/*
Test HCD IRPs (normal completion and early abort)

Purpose:
    - Test that pipes can be created
    - IRPs can be created and enqueued
    - Pipe returns HCD_PIPE_EVENT_IRP_DONE
    - Test that IRPs can be aborted when enqueued

Procedure:
    - Setup
    - Allocate IRPs. Initialize as Get Device Descriptor request
    - Enqueue IRPs
    - Expect HCD_PIPE_EVENT_IRP_DONE. Deallocate IRPs
    - Requeue IRPs, but abort them immediately
    - Teardown
*/
TEST_CASE("Test HCD IRP enqueue", "[hcd][ignore]")
{
    QueueHandle_t port_evt_queue;
    QueueHandle_t pipe_evt_queue;
    hcd_port_handle_t port_hdl;
    setup(&port_evt_queue, &pipe_evt_queue, &port_hdl);
    wait_for_connection(port_hdl, port_evt_queue);
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate default pipe and IRPs
    hcd_pipe_handle_t default_pipe;
    usb_irp_t **irp_list;
    alloc_pipe_and_irp_list(port_hdl, pipe_evt_queue, NUM_IRPS, &default_pipe, &irp_list);

    //Initialize IRPs to send a "Get Config Descriptor 0" request
    for (int i = 0; i < NUM_IRPS; i++) {
        irp_list[i]->num_bytes = 64;    //1 worst case MPS
        USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *) irp_list[i]->data_buffer, 0, TRANSFER_DATA_MAX_BYTES);
        irp_list[i]->context = IRP_CONTEXT_VAL;
    }

    //Enqueue those IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }

    //Wait for each done event of each IRP
    for (int i = 0; i < NUM_IRPS; i++) {
        expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_IRP_DONE);
    }

    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_NOT_EQUAL(NULL, irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED);
        TEST_ASSERT(irp->context == IRP_CONTEXT_VAL);
    }

    //Enqueue them again but abort them short after
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_abort(irp_list[i]));
    }
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for any in-flight transfers to complete

    expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_IRP_DONE);
    //Wait for the IRPs to complete and dequeue them, then check results
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_NOT_EQUAL(NULL, irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED || irp->status == USB_TRANSFER_STATUS_CANCELLED);
        TEST_ASSERT(irp->context == IRP_CONTEXT_VAL);
    }

    //Free IRPs and default pipe
    free_pipe_and_irp_list(default_pipe, NUM_IRPS, irp_list);

    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)
    wait_for_disconnection(port_hdl, port_evt_queue, false);
    teardown(port_evt_queue, pipe_evt_queue, port_hdl);
}

/*
Test HCD pipe STALL condition, abort, and clear

Purpose:
    - Test that a pipe can react to a STALL (i.e., a HCD_PIPE_EVENT_HALTED event)
    - The HCD_PIPE_CMD_ABORT can retire all IRPs
    - Pipe clear command can return the pipe to being active

Procedure:
    - Setup HCD and a port, a default pipe, and multiple IRPs
    - Corrupt the first IRP, then enqueue all of them.
    - The corrupted IRP should trigger a STALL response from the endpoint
    - Check that the correct pipe event, error, and state is returned from the pipe
    - Check that the other transfers can be retired using the abort command
    - Check that the halt can be cleared by using the clear command
    - Requeue correct transfers to check that pipe still works after being cleared
    - Teardown
*/
TEST_CASE("Test HCD pipe STALL", "[hcd][ignore]")
{
    QueueHandle_t port_evt_queue;
    QueueHandle_t pipe_evt_queue;
    hcd_port_handle_t port_hdl;
    setup(&port_evt_queue, &pipe_evt_queue, &port_hdl);
    wait_for_connection(port_hdl, port_evt_queue);
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate default pipe and IRPs
    hcd_pipe_handle_t default_pipe;
    usb_irp_t **irp_list;
    alloc_pipe_and_irp_list(port_hdl, pipe_evt_queue, NUM_IRPS, &default_pipe, &irp_list);

    //Initialize IRPs to send a "Get Device Descriptor" request
    for (int i = 0; i < NUM_IRPS; i++) {
        irp_list[i]->num_bytes = 64;    //1 worst case MPS
        USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *) irp_list[i]->data_buffer, 0, TRANSFER_DATA_MAX_BYTES);
        irp_list[i]->context = IRP_CONTEXT_VAL;
    }
    //Corrupt first transfer so that it triggers a STALL
    ((usb_ctrl_req_t *)irp_list[0]->data_buffer)->bRequest = 0xAA;

    //Enqueue those IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for transfers to complete

    //Check that pipe has been stalled
    printf("Expecting STALL\n");
    expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_ERROR_STALL);
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));

    //Call the pipe abort command to retire all transfers then dequeue all transfers
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_ABORT));
    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_NOT_EQUAL(NULL, irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_STALL || irp->status == USB_TRANSFER_STATUS_CANCELLED);
        TEST_ASSERT(irp->context == IRP_CONTEXT_VAL);
    }

    //Call the clear command to un-stall the pipe
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_CLEAR));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));

    //Correct first transfer then requeue
    USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *) irp_list[0]->data_buffer, 0, TRANSFER_DATA_MAX_BYTES);
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for transfers to complete

    //Wait for the IRPs to complete and dequeue them, then check results
    for (int i = 0; i < NUM_IRPS; i++) {
        expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_IRP_DONE);
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_NOT_EQUAL(NULL, irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED);
        TEST_ASSERT(irp->context == IRP_CONTEXT_VAL);
    }
    //Free IRPs
    free_pipe_and_irp_list(default_pipe, NUM_IRPS, irp_list);

    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)
    wait_for_disconnection(port_hdl, port_evt_queue, false);
    teardown(port_evt_queue, pipe_evt_queue, port_hdl);
}

/*
Test Pipe runtime halt and clear

Purpose:
    - Test that a pipe can be halted with a command whilst there are ongoing IRPs
    - Test that a pipe can be un-halted with a HCD_PIPE_CMD_CLEAR
    - Test that enqueued IRPs are resumed when pipe is cleared

Procedure:
    - Setup HCD, a default pipe, and multiple IRPs
    - Enqueue IRPs but execute a HCD_PIPE_CMD_HALT command immediately after. Halt command should let on
      the current going IRP finish before actually halting the pipe.
    - Clear the pipe halt using a HCD_PIPE_CMD_HALT command. Enqueued IRPs will be resumed
    - Check that all IRPs have completed successfully.
    - Teardown
*/
TEST_CASE("Test HCD pipe runtime halt and clear", "[hcd][ignore]")
{
    QueueHandle_t port_evt_queue;
    QueueHandle_t pipe_evt_queue;
    hcd_port_handle_t port_hdl;
    setup(&port_evt_queue, &pipe_evt_queue, &port_hdl);
    wait_for_connection(port_hdl, port_evt_queue);
    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)

    //Allocate default pipe and IRPs
    hcd_pipe_handle_t default_pipe;
    usb_irp_t **irp_list;
    alloc_pipe_and_irp_list(port_hdl, pipe_evt_queue, NUM_IRPS, &default_pipe, &irp_list);

    //Initialize IRPs to send a "Get Device Descriptor" request
    for (int i = 0; i < NUM_IRPS; i++) {
        irp_list[i]->num_bytes = 64;    //1 worst case MPS
        USB_CTRL_REQ_INIT_GET_CFG_DESC((usb_ctrl_req_t *)irp_list[i]->data_buffer, 0, TRANSFER_DATA_MAX_BYTES);
        irp_list[i]->context = IRP_CONTEXT_VAL;
    }

    printf("Enqueuing IRPs\n");
    //Enqueue those IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        TEST_ASSERT_EQUAL(ESP_OK, hcd_irp_enqueue(default_pipe, irp_list[i]));
    }

    //Halt the pipe immediately
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_HALT));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_HALTED, hcd_pipe_get_state(default_pipe));
    printf("Pipe halted\n");
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time for current in-flight transfer to complete

    //Clear command to un-halt the pipe
    TEST_ASSERT_EQUAL(ESP_OK, hcd_pipe_command(default_pipe, HCD_PIPE_CMD_CLEAR));
    TEST_ASSERT_EQUAL(HCD_PIPE_STATE_ACTIVE, hcd_pipe_get_state(default_pipe));
    printf("Pipe cleared\n");
    vTaskDelay(pdMS_TO_TICKS(100)); //Give some time pending for transfers to restart and complete

    //Dequeue IRPs
    for (int i = 0; i < NUM_IRPS; i++) {
        expect_pipe_event(pipe_evt_queue, default_pipe, HCD_PIPE_EVENT_IRP_DONE);
        usb_irp_t *irp = hcd_irp_dequeue(default_pipe);
        TEST_ASSERT_NOT_EQUAL(NULL, irp);
        TEST_ASSERT(irp->status == USB_TRANSFER_STATUS_COMPLETED);
        TEST_ASSERT(irp->context == IRP_CONTEXT_VAL);
    }
    //Free IRPs
    free_pipe_and_irp_list(default_pipe, NUM_IRPS, irp_list);

    vTaskDelay(pdMS_TO_TICKS(100)); //Short delay send of SOF (for FS) or EOPs (for LS)
    wait_for_disconnection(port_hdl, port_evt_queue, false);
    teardown(port_evt_queue, pipe_evt_queue, port_hdl);
}
