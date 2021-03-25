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

#include <string.h>
#include "sys/queue.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_rom_gpio.h"
#include "hal/usbh_hal.h"
#include "soc/gpio_pins.h"
#include "soc/gpio_sig_map.h"
#include "driver/periph_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "hcd.h"

// ----------------------------------------------------- Macros --------------------------------------------------------

// --------------------- Constants -------------------------

/**
 * @brief Number of transfer descriptors per transfer for various transfer types
 *
 * Control: Requires 3 transfer descriptors for a single transfer
 *          corresponding to each stage of a control transfer
 * Bulk: Requires 1 transfer descriptor for each transfer
 */
#define NUM_DESC_PER_XFER_CTRL      3
#define NUM_DESC_PER_XFER_BULK      1
#define XFER_LIST_LEN_CTRL          1
#define XFER_LIST_LEN_BULK          1

#define INIT_DELAY_MS               30      //A delay of at least 25ms to enter Host mode. Make it 30ms to be safe
#define DEBOUNCE_DELAY_MS           250     //A debounce delay of 250ms
#define RESET_HOLD_MS               30      //Spec requires at least 10ms. Make it 30ms to be safe
#define RESET_RECOVERY_MS           30      //Reset recovery delay of 10ms (make it 30 ms to be safe) to allow for connected device to recover (and for port enabled interrupt to occur)
#define RESUME_HOLD_MS              30      //Spec requires at least 20ms, Make it 30ms to be safe
#define RESUME_RECOVERY_MS          20      //Resume recovery of at least 10ms. Make it 20 ms to be safe. This will include the 3 LS bit times of the EOP

#define CTRL_EP_MAX_MPS_LS          8       //Largest Maximum Packet Size for Low Speed control endpoints
#define CTRL_EP_MAX_MPS_FS          64      //Largest Maximum Packet Size for Full Speed control endpoints

#define NUM_PORTS                   1       //The controller only has one port.

typedef enum {
    XFER_REQ_STATE_IDLE,        //The transfer request is not enqueued
    XFER_REQ_STATE_PENDING,     //The transfer request is enqueued and pending execution
    XFER_REQ_STATE_INFLIGHT,    //The transfer request is currently being executed
    XFER_REQ_STATE_DONE,        //The transfer request has completed executed or is retired, and is waiting to be dequeued
} xfer_req_state_t;

// -------------------- Convenience ------------------------

#define HCD_ENTER_CRITICAL_ISR()    portENTER_CRITICAL_ISR(&hcd_lock)
#define HCD_EXIT_CRITICAL_ISR()     portEXIT_CRITICAL_ISR(&hcd_lock)
#define HCD_ENTER_CRITICAL()        portENTER_CRITICAL(&hcd_lock)
#define HCD_EXIT_CRITICAL()         portEXIT_CRITICAL(&hcd_lock)

#define HCD_CHECK(cond, ret_val) ({                                         \
            if (!(cond)) {                                                  \
                return (ret_val);                                           \
            }                                                               \
})
#define HCD_CHECK_FROM_CRIT(cond, ret_val) ({                               \
            if (!(cond)) {                                                  \
                HCD_EXIT_CRITICAL();                                        \
                return ret_val;                                             \
            }                                                               \
})

// ------------------------------------------------------ Types --------------------------------------------------------

typedef struct xfer_req_obj xfer_req_t;
typedef struct pipe_obj pipe_t;
typedef struct port_obj port_t;

/**
 * @brief Object representing an HCD transfer request
 */
struct xfer_req_obj {
    TAILQ_ENTRY(xfer_req_obj) tailq_entry;  //TailQ entry for pending or done tailq in pipe object
    pipe_t *pipe;   //Target pipe of transfer request
    usb_irp_t *irp; //Target IRP
    void *context;  //Context variable of transfer request
    xfer_req_state_t state; //Current state of the transfer request
};

/**
 * @brief Object representing a pipe in the HCD layer
 */
struct pipe_obj {
    //Transfer requests related
    TAILQ_HEAD(tailhead_xfer_req_pend, xfer_req_obj) pend_xfer_req_tailq;
    TAILQ_HEAD(tailhead_xfer_req_done, xfer_req_obj) done_xfer_req_tailq;
    int num_xfer_req_pending;
    int num_xfer_req_done;
    xfer_req_t *inflight_xfer_req;  //Pointer to the current transfer request being executed by the pipe. NULL if none.
    //Port related
    port_t *port;                       //The port to which this pipe is routed through
    TAILQ_ENTRY(pipe_obj) tailq_entry;  //TailQ entry for port's list of pipes
    //HAl channel related
    void *xfer_desc_list;
    usbh_hal_chan_t *chan_obj;
    usbh_hal_ep_char_t ep_char;
    //Pipe status, state, and events
    hcd_pipe_state_t state;
    hcd_pipe_event_t last_event;
    TaskHandle_t task_waiting_pipe_notif;   //Task handle used for internal pipe events
    union {
        struct {
            uint32_t waiting_xfer_done: 1;
            uint32_t paused: 1;
            uint32_t pipe_cmd_processing: 1;
            //Flags only used by control transfers
            uint32_t ctrl_data_stg_in: 1;
            uint32_t ctrl_data_stg_skip: 1;
            uint32_t reserved3: 3;
            uint32_t xfer_desc_list_len: 8;
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } flags;
    //Pipe callback and context
    hcd_pipe_isr_callback_t callback;
    void *callback_arg;
    void *context;
};

/**
 * @brief Object representing a port in the HCD layer
 */
struct port_obj {
    usbh_hal_context_t *hal;
    //Pipes routed through this port
    TAILQ_HEAD(tailhead_pipes_idle, pipe_obj) pipes_idle_tailq;
    TAILQ_HEAD(tailhead_pipes_queued, pipe_obj) pipes_queued_tailq;
    int num_pipes_idle;
    int num_pipes_queued;
    //Port status, state, and events
    hcd_port_state_t state;
    usb_speed_t speed;
    hcd_port_event_t last_event;
    TaskHandle_t task_waiting_port_notif;   //Task handle used for internal port events
    union {
        struct {
            uint32_t event_pending: 1;      //The port has an event that needs to be handled
            uint32_t event_processing: 1;   //The port is current processing (handling) an event
            uint32_t cmd_processing: 1;     //Used to indicate command handling is ongoing
            uint32_t waiting_all_pipes_pause: 1;    //Waiting for all pipes routed through this port to be paused
            uint32_t disable_requested: 1;
            uint32_t conn_devc_ena: 1;      //Used to indicate the port is connected to a device that has been reset
            uint32_t reserved10: 10;
            uint32_t num_pipes_waiting_pause: 16;
        };
        uint32_t val;
    } flags;
    bool initialized;
    //Port callback and context
    hcd_port_isr_callback_t callback;
    void *callback_arg;
    SemaphoreHandle_t port_mux;
    void *context;
};

/**
 * @brief Object representing the HCD
 */
typedef struct {
    //Ports (Hardware only has one)
    port_t *port_obj;
    intr_handle_t isr_hdl;
} hcd_obj_t;

static portMUX_TYPE hcd_lock = portMUX_INITIALIZER_UNLOCKED;
static hcd_obj_t *s_hcd_obj = NULL;     //Note: "s_" is for the static pointer

// ------------------------------------------------- Forward Declare ---------------------------------------------------

// ----------------------- Events --------------------------

/**
 * @brief Wait for an internal event from a port
 *
 * @note For each port, there can only be one thread/task waiting for an internal port event
 * @note This function is blocking (will exit and re-enter the critical section to do so)
 *
 * @param port Port object
 */
static void _internal_port_event_wait(port_t *port);

/**
 * @brief Notify (from an ISR context) the thread/task waiting for the internal port event
 *
 * @param port Port object
 * @return true A yield is required
 * @return false Whether a yield is required or not
 */
static bool _internal_port_event_notify_from_isr(port_t *port);

/**
 * @brief Wait for an internal event from a particular pipe
 *
 * @note For each pipe, there can only be one thread/task waiting for an internal port event
 * @note This function is blocking (will exit and re-enter the critical section to do so)
 *
 * @param pipe Pipe object
 */
static void _internal_pipe_event_wait(pipe_t *pipe);

/**
 * @brief Notify (from an ISR context) the thread/task waiting for an internal pipe event
 *
 * @param pipe Pipe object
 * @param from_isr Whether this is called from an ISR or not
 * @return true A yield is required
 * @return false Whether a yield is required or not. Always false when from_isr is also false
 */
static bool _internal_pipe_event_notify(pipe_t *pipe, bool from_isr);

// ------------------------ Port ---------------------------

/**
 * @brief Invalidates all the pipes routed through a port
 *
 * This should be called when port or its connected device is no longer valid (e.g., the port is suddenly reset/disabled
 * or the device suddenly disconnects)
 *
 * @note This function may run one or more callbacks, and will exit and enter the critical section to do so
 *
 * Entry:
 *  - The port or its connected device is no longer valid. This guarantees that none of the pipes will be transferring
 * Exit:
 *  - Each pipe will have any pending transfer request moved to their respective done tailq
 *  - Each pipe will be put into the invalid state
 *  - Generate a HCD_PIPE_EVENT_INVALID event on each pipe and run their respective callbacks
 *
 * @param port Port object
 */
static void _port_invalidate_all_pipes(port_t *port);

/**
 * @brief Pause all pipes routed through a port
 *
 * Call this before attempting to reset or suspend a port
 *
 * Entry:
 *  - The port is in the HCD_PORT_STATE_ENABLED state (i.e., there is a connected device which has been reset)
 * Exit:
 *  - All pipes of the port have either paused, or are waiting to complete their inflight transfer request to pause
 *  - If waiting for one or more pipes, _internal_port_event_wait() must be called after this function returns
 *
 * @param port Port object
 * @return true All pipes have been paused
 * @return false Need to wait for one or more pipes to pause. Call _internal_port_event_wait() afterwards
 */
static bool _port_pause_all_pipes(port_t *port);

/**
 * @brief Un-pause all pipes routed through a port
 *
 * Call this before after coming out of a port reset or resume.
 *
 * Entry:
 *  - The port is in the HCD_PORT_STATE_ENABLED state
 *  - All pipes are paused
 * Exit:
 *  - All pipes un-paused. If those pipes have pending transfer requests, they will be started.
 *
 * @param port Port object
 */
static void _port_unpause_all_pipes(port_t *port);

/**
 * @brief Send a reset condition on a port's bus
 *
 * Entry:
 *  - The port must be in the HCD_PORT_STATE_ENABLED or HCD_PORT_STATE_DISABLED state
 * Exit:
 * - Reset condition sent on the port's bus
 *
 * @note This function is blocking (will exit and re-enter the critical section to do so)
 *
 * @param port Port object
 * @return true Reset condition successfully sent
 * @return false Failed to send reset condition due to unexpected port state
 */
static bool _port_bus_reset(port_t *port);

/**
 * @brief Send a suspend condition on a port's bus
 *
 * This function will first pause pipes routed through a port, and then send a suspend condition.
 *
 * Entry:
 *  - The port must be in the HCD_PORT_STATE_ENABLED state
 * Exit:
 *  - All pipes paused and the port is put into the suspended state
 *
 * @note This function is blocking (will exit and re-enter the critical section to do so)
 *
 * @param port Port object
 * @return true Suspend condition successfully sent. Port is now in the HCD_PORT_STATE_SUSPENDED state
 * @return false Failed to send a suspend condition due to unexpected port state
 */
static bool _port_bus_suspend(port_t *port);

/**
 * @brief Send a resume condition on a port's bus
 *
 * This function will send a resume condition, and then un-pause all the pipes routed through a port
 *
 * Entry:
 *  - The port must be in the HCD_PORT_STATE_SUSPENDED state
 * Exit:
 *  - The port is put into the enabled state and all pipes un-paused
 *
 * @note This function is blocking (will exit and re-enter the critical section to do so)
 *
 * @param port Port object
 * @return true Resume condition successfully sent. Port is now in the HCD_PORT_STATE_ENABLED state
 * @return false Failed to send a resume condition due to unexpected port state.
 */
static bool _port_bus_resume(port_t *port);

/**
 * @brief Disable a port
 *
 * Entry:
 *  - The port must be in the HCD_PORT_STATE_ENABLED or HCD_PORT_STATE_SUSPENDED state
 * Exit:
 *  - All pipes paused (should already be paused if port was suspended), and the port is put into the disabled state.
 *
 * @note This function is blocking (will exit and re-enter the critical section to do so)
 *
 * @param port Port object
 * @return true Port successfully disabled
 * @return false Port to disable port due to unexpected port state
 */
static bool _port_disable(port_t *port);

/**
 * @brief Debounce port after a connection or disconnection event
 *
 * This function should be called after a port connection or disconnect event. This function will execute a debounce
 * delay then check the actual connection/disconnections state.
 *
 * @param port Port object
 * @return true A device is connected
 * @return false No device connected
 */
static bool _port_debounce(port_t *port);

// ------------------------ Pipe ---------------------------

/**
 * @brief Get the next pending transfer request from the pending tailq
 *
 * Entry:
 * - The inflight transfer request must be set to NULL (indicating the pipe currently has no inflight transfer request)
 * Exit:
 * - If (num_xfer_req_pending > 0), the first transfer request is removed from pend_xfer_req_tailq and and
 *   inflight_xfer_req is set to that transfer request.
 * - If there are no more queued transfer requests, inflight_xfer_req is left as NULL
 *
 * @param pipe Pipe object
 * @return true A pending transfer request is now set as the inflight transfer request
 * @return false No more pending transfer requests
 */
static bool _pipe_get_next_xfer_req(pipe_t *pipe);

/**
 * @brief Return the inflight transfer request to the done tailq
 *
 * Entry:
 *  - The inflight transfer request must already have been parsed (i.e., results have been checked)
 * Exit:
 * - The inflight transfer request is returned to the done tailq and inflight_xfer_req is set to NULL
 *
 * @param pipe Pipe object
 */
static void _pipe_ret_cur_xfer_req(pipe_t *pipe);

/**
 * @brief Wait until a pipe's inflight transfer request is done
 *
 * If the pipe has an inflight transfer request, this function will block until it is done (via a internal pipe event).
 * If the pipe has no inflight transfer request, this function do nothing and return immediately.
 * If the pipe's state changes unexpectedely, this function will return false.
 *
 * @note This function is blocking (will exit and re-enter the critical section to do so)
 *
 * @param pipe Pipe object
 * @return true Pipes inflight transfer request is done
 * @return false Pipes state unexpectedly changed
 */
static bool _pipe_wait_done(pipe_t *pipe);

/**
 * @brief Retires all transfer requests (those that were previously inflight or pending)
 *
 * Retiring all transfer requests will result in any pending transfer request being moved to the done tailq. This
 * function will update the IPR status of each transfer request.
 *  - If the retiring is self-initiated (i.e., due to a pipe command), the IRP status will be set to USB_TRANSFER_STATUS_CANCELLED.
 *  - If the retiring is NOT self-initiated (i.e., the pipe is no longer valid), the IRP status will be set to USB_TRANSFER_STATUS_NO_DEVICE
 *
 * Entry:
 * - There can be no inflight transfer request (must already be parsed and returned to done queue)
 * Exit:
 * - If there was an inflight transfer request, it is parsed and returned to the done queue
 * - If there are any pending transfer requests:
 *      - They are moved to the done tailq
 *
 * @param pipe Pipe object
 * @param cancelled Are we actively Pipe retire is initialized by the user due to a command, thus transfer request are actively
 *                  cancelled
 */
static void _pipe_retire(pipe_t *pipe, bool self_initiated);

/**
 * @brief Decode a HAL channel error to the corresponding pipe event
 *
 * @param chan_error The HAL channel error
 * @return hcd_pipe_event_t The corresponding pipe error event
 */
static inline hcd_pipe_event_t pipe_decode_error_event(usbh_hal_chan_error_t chan_error);

// ------------------ Transfer Requests --------------------

/**
 * @brief Fill a transfer request into the pipe's transfer descriptor list
 *
 * Entry:
 *  - The pipe's inflight_xfer_req must be set to the next transfer request
 * Exit:
 *  - inflight_xfer_req filled into the pipe's transfer descriptor list
 *  - Starting PIDs and directions set
 *  - Channel slot acquired. Will need to call usbh_hal_chan_activate() to actually start execution
 *
 * @param pipe Pipe where inflight_xfer_req is already set to the next transfer request
 */
static void _xfer_req_fill(pipe_t *pipe);

/**
 * @brief Continue a transfer request
 *
 * @note This is currently only used for control transfers
 *
 * @param pipe Pipe where inflight_xfer_req contains the transfer request to continue
 */
static void _xfer_req_continue(pipe_t *pipe);

/**
 * @brief Parse the results of a pipe's transfer descriptor list into a transfer request
 *
 * Entry:
 *  - The pipe must have stop transferring either due a channel event or a port disconnection.
 *  - The pipe's state and last_event must be updated before parsing the transfer request as
 *    they will used to determine the resuult of the transfer request
 * Exit:
 *  - The pipe's inflight_xfer_req is filled with result of the transfer request (i.e., the underlying IRP has its status set)
 *
 * @param pipe Pipe where inflight_xfer_req contains the completed transfer request
 * @param error_occurred Are we parsing after the pipe had an error (or has become invalid)
 */
static void _xfer_req_parse(pipe_t *pipe, bool error_occurred);

// ----------------------------------------------- Interrupt Handling --------------------------------------------------

// ------------------- Internal Event ----------------------

static void _internal_port_event_wait(port_t *port)
{
    //There must NOT be another thread/task already waiting for an internal event
    assert(port->task_waiting_port_notif == NULL);
    port->task_waiting_port_notif = xTaskGetCurrentTaskHandle();
    HCD_EXIT_CRITICAL();
    //Wait to be notified from ISR
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    HCD_ENTER_CRITICAL();
    port->task_waiting_port_notif = NULL;
}

static bool _internal_port_event_notify_from_isr(port_t *port)
{
    //There must be a thread/task waiting for an internal event
    assert(port->task_waiting_port_notif != NULL);
    BaseType_t xTaskWoken = pdFALSE;
    //Unblock the thread/task waiting for the notification
    HCD_EXIT_CRITICAL_ISR();
    vTaskNotifyGiveFromISR(port->task_waiting_port_notif, &xTaskWoken);
    HCD_ENTER_CRITICAL_ISR();
    return (xTaskWoken == pdTRUE);
}

static void _internal_pipe_event_wait(pipe_t *pipe)
{
    //There must NOT be another thread/task already waiting for an internal event
    assert(pipe->task_waiting_pipe_notif == NULL);
    pipe->task_waiting_pipe_notif = xTaskGetCurrentTaskHandle();
    HCD_EXIT_CRITICAL();
    //Wait to be notified from ISR
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    HCD_ENTER_CRITICAL();
    pipe->task_waiting_pipe_notif = NULL;
}

static bool _internal_pipe_event_notify(pipe_t *pipe, bool from_isr)
{
    //There must be a thread/task waiting for an internal event
    assert(pipe->task_waiting_pipe_notif != NULL);
    bool ret;
    if (from_isr) {
        BaseType_t xTaskWoken = pdFALSE;
        HCD_EXIT_CRITICAL_ISR();
        //Unblock the thread/task waiting for the pipe notification
        vTaskNotifyGiveFromISR(pipe->task_waiting_pipe_notif, &xTaskWoken);
        HCD_ENTER_CRITICAL_ISR();
        ret = (xTaskWoken == pdTRUE);
    } else {
        HCD_EXIT_CRITICAL();
        xTaskNotifyGive(pipe->task_waiting_pipe_notif);
        HCD_ENTER_CRITICAL();
        ret = false;
    }
    return ret;
}

// ----------------- Interrupt Handlers --------------------

/**
 * @brief Handle a HAL port interrupt and obtain the corresponding port event
 *
 * @param[in] port Port object
 * @param[in] hal_port_event The HAL port event
 * @param[out] yield Set to true if a yield is required as a result of handling the interrupt
 * @return hcd_port_event_t  Returns a port event, or HCD_PORT_EVENT_NONE if no port event occurred
 */
static hcd_port_event_t _intr_hdlr_hprt(port_t *port, usbh_hal_port_event_t hal_port_event, bool *yield)
{
    hcd_port_event_t port_event = HCD_PORT_EVENT_NONE;
    switch (hal_port_event) {
        case USBH_HAL_PORT_EVENT_CONN: {
            //Don't update state immediately, we still need to debounce.
            port_event = HCD_PORT_EVENT_CONNECTION;
            break;
        }
        case USBH_HAL_PORT_EVENT_DISCONN: {
            if (port->flags.conn_devc_ena) {
                //The port was previously enabled, so this is a sudden disconenction
                port->state = HCD_PORT_STATE_RECOVERY;
                port_event = HCD_PORT_EVENT_SUDDEN_DISCONN;
            } else {
                //For normal disconnections, don't update state immediately as we still need to debounce.
                port_event = HCD_PORT_EVENT_DISCONNECTION;
            }
            port->flags.conn_devc_ena = 0;
            break;
        }
        case USBH_HAL_PORT_EVENT_ENABLED: {
            usbh_hal_port_enable(port->hal);  //Initialize remaining host port registers
            port->speed = usbh_hal_port_get_conn_speed(port->hal);
            port->state = HCD_PORT_STATE_ENABLED;
            port->flags.conn_devc_ena = 1;
            //This was triggered by a command, so no event needs to be propagated.
            break;
        }
        case USBH_HAL_PORT_EVENT_DISABLED: {
            port->flags.conn_devc_ena = 0;
            //Disabled could be due to a disable request or reset request, or due to a port error
            if (port->state != HCD_PORT_STATE_RESETTING) {  //Ignore the disable event if it's due to a reset request
                port->state = HCD_PORT_STATE_DISABLED;
                if (port->flags.disable_requested) {
                    //Disabled by request (i.e. by port command). Generate an internal event
                    port->flags.disable_requested = 0;
                    *yield |= _internal_port_event_notify_from_isr(port);
                } else {
                    //Disabled due to a port error
                    port_event = HCD_PORT_EVENT_ERROR;
                }
            }
            break;
        }
        case USBH_HAL_PORT_EVENT_OVRCUR:
        case USBH_HAL_PORT_EVENT_OVRCUR_CLR: {  //Could occur if a quick overcurrent then clear happens
            if (port->state != HCD_PORT_STATE_NOT_POWERED) {
                //We need to power OFF the port to protect it
                usbh_hal_port_toggle_power(port->hal, false);
                port->state = HCD_PORT_STATE_NOT_POWERED;
                port_event = HCD_PORT_EVENT_OVERCURRENT;
            }
            port->flags.conn_devc_ena = 0;
            break;
        }
        default: {
            abort();
            break;
        }
    }
    return port_event;
}

/**
 * @brief Handles a HAL channel interrupt
 *
 * This function should be called on a HAL channel when it has an interrupt. Most HAL channel events will correspond to
 * to a pipe event, but not always. This function will store the pipe event and return a pipe object pointer if a pipe
 * event occurred, or return NULL otherwise.
 *
 * @param[in] chan_obj Pointer to HAL channel object with interrupt
 * @param[out] yield Set to true if a yield is required as a result of handling the interrupt
 * @return hcd_pipe_event_t The pipe event
 */
static hcd_pipe_event_t _intr_hdlr_chan(pipe_t *pipe, usbh_hal_chan_t *chan_obj, bool *yield)
{
    usbh_hal_chan_event_t chan_event = usbh_hal_chan_decode_intr(chan_obj);
    hcd_pipe_event_t event = HCD_PIPE_EVENT_NONE;
    //Check the the pipe's port still has a connected and enabled device before processing the interrupt
    if (!pipe->port->flags.conn_devc_ena) {
        return event;   //Treat as a no event.
    }

    switch (chan_event) {
        case USBH_HAL_CHAN_EVENT_SLOT_DONE: {
            //An entire transfer descriptor list has completed execution
            pipe->last_event = HCD_PIPE_EVENT_XFER_REQ_DONE;
            event = HCD_PIPE_EVENT_XFER_REQ_DONE;
            _xfer_req_parse(pipe, false);    //Parse results of transfer request
            _pipe_ret_cur_xfer_req(pipe);    //Return the transfer request to the pipe's done tailq
            if (pipe->flags.waiting_xfer_done) {
                //A port/pipe command is waiting for this pipe to complete its transfer. So don't load the next transfer
                pipe->flags.waiting_xfer_done = 0;
                if (pipe->port->flags.waiting_all_pipes_pause) {
                    //Port command is waiting for all pipes to be paused
                    pipe->flags.paused = 1;
                    pipe->port->flags.num_pipes_waiting_pause--;
                    if (pipe->port->flags.num_pipes_waiting_pause == 0) {
                        //All pipes have finished pausing, Notify the blocked port command
                        pipe->port->flags.waiting_all_pipes_pause = 0;
                        *yield |= _internal_port_event_notify_from_isr(pipe->port);
                    }
                } else {
                    //Pipe command is waiting for transfer to complete
                    *yield |= _internal_pipe_event_notify(pipe, true);
                }
            } else if (_pipe_get_next_xfer_req(pipe)) {
                //Fill the descriptor list with the transfer request and start the transfer
                _xfer_req_fill(pipe);
                usbh_hal_chan_activate(chan_obj, 0);  //Start with the first descriptor
            }
            break;
        }
        case USBH_HAL_CHAN_EVENT_SLOT_HALT: {
            //A transfer descriptor list has partially completed. This currently only happens on control pipes
            assert(pipe->ep_char.type == USB_XFER_TYPE_CTRL);
            _xfer_req_continue(pipe);    //Continue the transfer request.
            //We are continuing a transfer, so no event has occurred
            break;
        }
        case USBH_HAL_CHAN_EVENT_ERROR: {
            //Get and store the pipe error event
            usbh_hal_chan_error_t chan_error = usbh_hal_chan_get_error(chan_obj);
            usbh_hal_chan_clear_error(chan_obj);
            pipe->last_event = pipe_decode_error_event(chan_error);
            event = pipe->last_event;
            pipe->state = HCD_PIPE_STATE_HALTED;
            //Parse the failed transfer request and update it's IRP status
            _xfer_req_parse(pipe, true);
            _pipe_ret_cur_xfer_req(pipe);    //Return the transfer request to the pipe's done tailq
            break;
        }
        case USBH_HAL_CHAN_EVENT_HALT_REQ:  //We currently don't halt request so this event should never occur
        default:
            abort();
            break;
    }

    return event;
}

/**
 * @brief Main interrupt handler
 *
 * - Handle all HPRT (Host Port) related interrupts first as they may change the
 *   state of the driver (e.g., a disconnect event)
 * - If any channels (pipes) have pending interrupts, handle them one by one
 * - The HCD has not blocking functions, so the user's ISR callback is run to
 *   allow the users to send whatever OS primitives they need.
 * @param arg
 */
static void intr_hdlr_main(void *arg)
{
    port_t *port = (port_t *)arg;
    bool yield = false;

    HCD_ENTER_CRITICAL_ISR();
    usbh_hal_port_event_t hal_port_evt = usbh_hal_decode_intr(port->hal);
    if (hal_port_evt == USBH_HAL_PORT_EVENT_CHAN) {
        //Channel event. Cycle through each pending channel
        usbh_hal_chan_t *chan_obj = usbh_hal_get_chan_pending_intr(port->hal);
        while (chan_obj != NULL) {
            pipe_t *pipe = (pipe_t *)usbh_hal_chan_get_context(chan_obj);
            hcd_pipe_event_t event = _intr_hdlr_chan(pipe, chan_obj, &yield);
            //Run callback if a pipe event has occurred and the pipe also has a callback
            if (event != HCD_PIPE_EVENT_NONE && pipe->callback != NULL) {
                HCD_EXIT_CRITICAL_ISR();
                yield |= pipe->callback((hcd_pipe_handle_t)pipe, event, pipe->callback_arg, true);
                HCD_ENTER_CRITICAL_ISR();
            }
            //Check for more channels with pending interrupts. Returns NULL if there are no more
            chan_obj = usbh_hal_get_chan_pending_intr(port->hal);
        }
    } else if (hal_port_evt != USBH_HAL_PORT_EVENT_NONE) {  //Port event
        hcd_port_event_t port_event = _intr_hdlr_hprt(port, hal_port_evt, &yield);
        if (port_event != HCD_PORT_EVENT_NONE) {
            port->last_event = port_event;
            port->flags.event_pending = 1;
            if (port->callback != NULL) {
                HCD_EXIT_CRITICAL_ISR();
                yield |= port->callback((hcd_port_handle_t)port, port_event, port->callback_arg, true);
                HCD_ENTER_CRITICAL_ISR();
            }
        }
    }
    HCD_EXIT_CRITICAL_ISR();

    if (yield) {
        portYIELD_FROM_ISR();
    }
}

// --------------------------------------------- Host Controller Driver ------------------------------------------------

static port_t *port_obj_alloc(void)
{
    port_t *port = calloc(1, sizeof(port_t));
    usbh_hal_context_t *hal = malloc(sizeof(usbh_hal_context_t));
    SemaphoreHandle_t port_mux = xSemaphoreCreateMutex();
    if (port == NULL || hal == NULL || port_mux == NULL) {
        free(port);
        free(hal);
        if (port_mux != NULL) {
            vSemaphoreDelete(port_mux);
        }
        return NULL;
    }
    port->hal = hal;
    port->port_mux = port_mux;
    return port;
}

static void port_obj_free(port_t *port)
{
    if (port == NULL) {
        return;
    }
    vSemaphoreDelete(port->port_mux);
    free(port->hal);
    free(port);
}

// ----------------------- Public --------------------------

esp_err_t hcd_install(const hcd_config_t *config)
{
    HCD_ENTER_CRITICAL();
    HCD_CHECK_FROM_CRIT(s_hcd_obj == NULL, ESP_ERR_INVALID_STATE);
    HCD_EXIT_CRITICAL();

    esp_err_t err_ret;
    //Allocate memory and resources for driver object and all port objects
    hcd_obj_t *p_hcd_obj_dmy = calloc(1, sizeof(hcd_obj_t));
    if (p_hcd_obj_dmy == NULL) {
        return ESP_ERR_NO_MEM;
    }

    //Allocate resources for each port (there's only one)
    p_hcd_obj_dmy->port_obj = port_obj_alloc();
    esp_err_t intr_alloc_ret = esp_intr_alloc(ETS_USB_INTR_SOURCE,
                                              config->intr_flags | ESP_INTR_FLAG_INTRDISABLED,  //The interruupt must be disabled until the port is initialized
                                              intr_hdlr_main,
                                              (void *)p_hcd_obj_dmy->port_obj,
                                              &p_hcd_obj_dmy->isr_hdl);
    if (p_hcd_obj_dmy->port_obj == NULL) {
        err_ret = ESP_ERR_NO_MEM;
    }
    if (intr_alloc_ret != ESP_OK) {
        err_ret = intr_alloc_ret;
        goto err;
    }

    HCD_ENTER_CRITICAL();
    if (s_hcd_obj != NULL) {
        HCD_EXIT_CRITICAL();
        err_ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    s_hcd_obj = p_hcd_obj_dmy;
    //Set HW prereqs for each port (there's only one)
    periph_module_enable(PERIPH_USB_MODULE);
    periph_module_reset(PERIPH_USB_MODULE);
    /*
    Configure GPIOS for Host mode operation using internal PHY
        - Forces ID to GND for A side
        - Forces B Valid to GND as we are A side host
        - Forces VBUS Valid to HIGH
        - Froces A Valid to HIGH
    */
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_OTG_IDDIG_IN_IDX, false);
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_SRP_BVALID_IN_IDX, false);
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_VBUSVALID_IN_IDX, false);
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_AVALID_IN_IDX, false);
    HCD_EXIT_CRITICAL();
    return ESP_OK;
err:
    if (intr_alloc_ret == ESP_OK) {
        esp_intr_free(p_hcd_obj_dmy->isr_hdl);
    }
    port_obj_free(p_hcd_obj_dmy->port_obj);
    free(p_hcd_obj_dmy);
    return err_ret;
}

esp_err_t hcd_uninstall(void)
{
    HCD_ENTER_CRITICAL();
    //Check that all ports have been disabled (theres only one)
    if (s_hcd_obj == NULL || s_hcd_obj->port_obj->initialized) {
        HCD_EXIT_CRITICAL();
        return ESP_ERR_INVALID_STATE;
    }
    periph_module_disable(PERIPH_USB_MODULE);
    hcd_obj_t *p_hcd_obj_dmy = s_hcd_obj;
    s_hcd_obj = NULL;
    HCD_EXIT_CRITICAL();

    //Free resources
    port_obj_free(p_hcd_obj_dmy->port_obj);
    esp_intr_free(p_hcd_obj_dmy->isr_hdl);
    free(p_hcd_obj_dmy);
    return ESP_OK;
}

// ------------------------------------------------------ Port ---------------------------------------------------------

// ----------------------- Private -------------------------

static void _port_invalidate_all_pipes(port_t *port)
{
    //This function should only be called when the port is invalid
    assert(!port->flags.conn_devc_ena);
    pipe_t *pipe;
    //Process all pipes that have queued transfer requests
    TAILQ_FOREACH(pipe, &port->pipes_queued_tailq, tailq_entry) {
        //Mark the pipe as invalid and set an invalid event
        pipe->state = HCD_PIPE_STATE_INVALID;
        pipe->last_event = HCD_PIPE_EVENT_INVALID;
        //If the pipe had an inflight transfer, parse and return it
        if (pipe->inflight_xfer_req != NULL) {
            _xfer_req_parse(pipe, true);
            _pipe_ret_cur_xfer_req(pipe);
        }
        //Retire any remaining transfer requests
        _pipe_retire(pipe, false);
        if (pipe->task_waiting_pipe_notif != NULL) {
            //Unblock the thread/task waiting for a notification from the pipe as the pipe is no longer valid.
            _internal_pipe_event_notify(pipe, false);
        }
        if (pipe->callback != NULL) {
            HCD_EXIT_CRITICAL();
            (void) pipe->callback((hcd_pipe_handle_t)pipe, HCD_PIPE_EVENT_INVALID, pipe->callback_arg, false);
            HCD_ENTER_CRITICAL();
        }
    }
    //Process all idle pipes
    TAILQ_FOREACH(pipe, &port->pipes_idle_tailq, tailq_entry) {
        //Mark pipe as invalid and call its callback
        pipe->state = HCD_PIPE_STATE_INVALID;
        pipe->last_event = HCD_PIPE_EVENT_INVALID;
        if (pipe->callback != NULL) {
            HCD_EXIT_CRITICAL();
            (void) pipe->callback((hcd_pipe_handle_t)pipe, HCD_PIPE_EVENT_INVALID, pipe->callback_arg, false);
            HCD_ENTER_CRITICAL();
        }
    }
}

static bool _port_pause_all_pipes(port_t *port)
{
    assert(port->state == HCD_PORT_STATE_ENABLED);
    pipe_t *pipe;
    int num_pipes_waiting_done = 0;
    //Process all pipes that have queued transfer requests
    TAILQ_FOREACH(pipe, &port->pipes_queued_tailq, tailq_entry) {
        if (pipe->inflight_xfer_req != NULL) {
            //Pipe has an inflight transfer. Indicate to the pipe we are waiting the transfer to complete
            pipe->flags.waiting_xfer_done = 1;
            num_pipes_waiting_done++;
        } else {
            //No inflight transfer so no need to wait
            pipe->flags.paused = 1;
        }
    }
    //Process all idle pipes. They don't have queue transfer so just mark them as paused
    TAILQ_FOREACH(pipe, &port->pipes_idle_tailq, tailq_entry) {
        pipe->flags.paused = 1;
    }
    if (num_pipes_waiting_done > 0) {
        //Indicate we need to wait for one or more pipes to complete their transfers
        port->flags.num_pipes_waiting_pause = num_pipes_waiting_done;
        port->flags.waiting_all_pipes_pause = 1;
        return false;
    }
    return true;
}

static void _port_unpause_all_pipes(port_t *port)
{
    assert(port->state == HCD_PORT_STATE_ENABLED);
    pipe_t *pipe;
    //Process all idle pipes. They don't have queue transfer so just mark them as un-paused
    TAILQ_FOREACH(pipe, &port->pipes_idle_tailq, tailq_entry) {
        pipe->flags.paused = 0;
    }
    //Process all pipes that have queued transfer requests
    TAILQ_FOREACH(pipe, &port->pipes_queued_tailq, tailq_entry) {
        pipe->flags.paused = 0;
        //If the pipe has more pending transfer request, start them.
        if (_pipe_get_next_xfer_req(pipe)) {
            _xfer_req_fill(pipe);
            usbh_hal_chan_activate(pipe->chan_obj, 0);
        }
    }
}

static bool _port_bus_reset(port_t *port)
{
    assert(port->state == HCD_PORT_STATE_ENABLED || port->state == HCD_PORT_STATE_DISABLED);
    //Put and hold the bus in the reset state. If the port was previously enabled, a disabled event will occur after this
    port->state = HCD_PORT_STATE_RESETTING;
    usbh_hal_port_toggle_reset(port->hal, true);
    HCD_EXIT_CRITICAL();
    vTaskDelay(pdMS_TO_TICKS(RESET_HOLD_MS));
    HCD_ENTER_CRITICAL();
    if (port->state != HCD_PORT_STATE_RESETTING) {
        //The port state has unexpectedly changed
        goto bailout;
    }
    //Return the bus to the idle state and hold it for the required reset recovery time. Port enabled event should occur
    usbh_hal_port_toggle_reset(port->hal, false);
    HCD_EXIT_CRITICAL();
    vTaskDelay(pdMS_TO_TICKS(RESET_RECOVERY_MS));
    HCD_ENTER_CRITICAL();
    if (port->state != HCD_PORT_STATE_ENABLED || !port->flags.conn_devc_ena) {
        //The port state has unexpectedly changed
        goto bailout;
    }
    return true;
bailout:
    return false;
}

static bool _port_bus_suspend(port_t *port)
{
    assert(port->state == HCD_PORT_STATE_ENABLED);
    //Pause all pipes before suspending the bus
    if (!_port_pause_all_pipes(port)) {
        //Need to wait for some pipes to pause. Wait for notification from ISR
        _internal_port_event_wait(port);
        if (port->state != HCD_PORT_STATE_ENABLED || !port->flags.conn_devc_ena) {
            //Port state unexpectedley changed
            goto bailout;
        }
    }
    //All pipes are guaranteed paused at this point. Proceed to suspend the port
    usbh_hal_port_suspend(port->hal);
    port->state = HCD_PORT_STATE_SUSPENDED;
    return true;
bailout:
    return false;
}

static bool _port_bus_resume(port_t *port)
{
    assert(port->state == HCD_PORT_STATE_SUSPENDED);
    //Put and hold the bus in the K state.
    usbh_hal_port_toggle_resume(port->hal, true);
    port->state = HCD_PORT_STATE_RESUMING;
    HCD_EXIT_CRITICAL();
    vTaskDelay(pdMS_TO_TICKS(RESUME_HOLD_MS));
    HCD_ENTER_CRITICAL();
    //Return and hold the bus to the J state (as port of the LS EOP)
    usbh_hal_port_toggle_resume(port->hal, false);
    if (port->state != HCD_PORT_STATE_RESUMING || !port->flags.conn_devc_ena) {
        //Port state unexpectedley changed
        goto bailout;
    }
    HCD_EXIT_CRITICAL();
    vTaskDelay(pdMS_TO_TICKS(RESUME_RECOVERY_MS));
    HCD_ENTER_CRITICAL();
    if (port->state != HCD_PORT_STATE_RESUMING || !port->flags.conn_devc_ena) {
        //Port state unexpectedley changed
        goto bailout;
    }
    port->state = HCD_PORT_STATE_ENABLED;
    _port_unpause_all_pipes(port);
    return true;
bailout:
    return false;
}

static bool _port_disable(port_t *port)
{
    assert(port->state == HCD_PORT_STATE_ENABLED || port->state == HCD_PORT_STATE_SUSPENDED);
    if (port->state == HCD_PORT_STATE_ENABLED) {
        //There may be pipes that are still transferring, so pause them.
        if (!_port_pause_all_pipes(port)) {
            //Need to wait for some pipes to pause. Wait for notification from ISR
            _internal_port_event_wait(port);
            if (port->state != HCD_PORT_STATE_ENABLED || !port->flags.conn_devc_ena) {
                //Port state unexpectedley changed
                goto bailout;
            }
        }
    }
    //All pipes are guaranteed paused at this point. Proceed to suspend the port. This should trigger an internal event
    port->flags.disable_requested = 1;
    usbh_hal_port_disable(port->hal);
    _internal_port_event_wait(port);
    if (port->state != HCD_PORT_STATE_DISABLED) {
        //Port state unexpectedley changed
        goto bailout;
    }
    _port_invalidate_all_pipes(port);
    return true;
bailout:
    return false;
}

static bool _port_debounce(port_t *port)
{
    if (port->state == HCD_PORT_STATE_NOT_POWERED) {
        //Disconnect event due to power off, no need to debounce or update port state.
        return false;
    }
    HCD_EXIT_CRITICAL();
    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY_MS));
    HCD_ENTER_CRITICAL();
    //Check the post-debounce state of the bus (i.e., whether it's actually connected/disconnected)
    bool is_connected = usbh_hal_port_check_if_connected(port->hal);
    if (is_connected) {
        port->state = HCD_PORT_STATE_DISABLED;
    } else {
        port->state = HCD_PORT_STATE_DISCONNECTED;
    }
    //Disable debounce lock
    usbh_hal_disable_debounce_lock(port->hal);
    return is_connected;
}

// ----------------------- Public --------------------------

esp_err_t hcd_port_init(int port_number, hcd_port_config_t *port_config, hcd_port_handle_t *port_hdl)
{
    HCD_CHECK(port_number > 0 && port_config != NULL && port_hdl != NULL, ESP_ERR_INVALID_ARG);
    HCD_CHECK(port_number <= NUM_PORTS, ESP_ERR_NOT_FOUND);

    HCD_ENTER_CRITICAL();
    HCD_CHECK_FROM_CRIT(s_hcd_obj != NULL && !s_hcd_obj->port_obj->initialized, ESP_ERR_INVALID_STATE);
    //Port object memory and resources (such as mutex) already be allocated. Just need to initialize necessary fields only
    port_t *port_obj = s_hcd_obj->port_obj;
    TAILQ_INIT(&port_obj->pipes_idle_tailq);
    TAILQ_INIT(&port_obj->pipes_queued_tailq);
    port_obj->state = HCD_PORT_STATE_NOT_POWERED;
    port_obj->last_event = HCD_PORT_EVENT_NONE;
    port_obj->callback = port_config->callback;
    port_obj->callback_arg = port_config->callback_arg;
    port_obj->context = port_config->context;
    usbh_hal_init(port_obj->hal);
    port_obj->initialized = true;
    esp_intr_enable(s_hcd_obj->isr_hdl);
    *port_hdl = (hcd_port_handle_t)port_obj;
    HCD_EXIT_CRITICAL();

    vTaskDelay(pdMS_TO_TICKS(INIT_DELAY_MS));    //Need a short delay before host mode takes effect
    return ESP_OK;
}

esp_err_t hcd_port_deinit(hcd_port_handle_t port_hdl)
{
    port_t *port = (port_t *)port_hdl;

    HCD_ENTER_CRITICAL();
    HCD_CHECK_FROM_CRIT(s_hcd_obj != NULL && port->initialized
                        && port->num_pipes_idle == 0 && port->num_pipes_queued == 0
                        && (port->state == HCD_PORT_STATE_NOT_POWERED || port->state == HCD_PORT_STATE_RECOVERY)
                        && port->flags.val == 0 && port->task_waiting_port_notif == NULL,
                        ESP_ERR_INVALID_STATE);
    port->initialized = false;
    esp_intr_disable(s_hcd_obj->isr_hdl);
    usbh_hal_deinit(port->hal);
    HCD_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t hcd_port_command(hcd_port_handle_t port_hdl, hcd_port_cmd_t command)
{
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    port_t *port = (port_t *)port_hdl;
    xSemaphoreTake(port->port_mux, portMAX_DELAY);
    HCD_ENTER_CRITICAL();
    if (port->initialized && !port->flags.event_pending) { //Port events need to be handled first before issuing a command
        port->flags.cmd_processing = 1;
        switch (command) {
            case HCD_PORT_CMD_POWER_ON: {
                //Port can only be powered on if currently unpowered
                if (port->state == HCD_PORT_STATE_NOT_POWERED) {
                    port->state = HCD_PORT_STATE_DISCONNECTED;
                    usbh_hal_port_start(port->hal);
                    usbh_hal_port_toggle_power(port->hal, true);
                    ret = ESP_OK;
                }
                break;
            }
            case HCD_PORT_CMD_POWER_OFF: {
                //Port can only be unpowered if already powered
                if (port->state != HCD_PORT_STATE_NOT_POWERED) {
                    port->state = HCD_PORT_STATE_NOT_POWERED;
                    usbh_hal_port_stop(port->hal);
                    usbh_hal_port_toggle_power(port->hal, false);
                    //If a device is currently connected, this should trigger a disconnect event
                    ret = ESP_OK;
                }
                break;
            }
            case HCD_PORT_CMD_RESET: {
                //Port can only a reset when it is in the enabled or disabled states (in case of new connection)
                if (port->state == HCD_PORT_STATE_ENABLED || port->state == HCD_PORT_STATE_DISABLED) {
                    ret = (_port_bus_reset(port)) ? ESP_OK : ESP_ERR_INVALID_RESPONSE;
                }
                break;
            }
            case HCD_PORT_CMD_SUSPEND: {
                //Port can only be suspended if already in the enabled state
                if (port->state == HCD_PORT_STATE_ENABLED) {
                    ret = (_port_bus_suspend(port)) ? ESP_OK : ESP_ERR_INVALID_RESPONSE;
                }
                break;
            }
            case HCD_PORT_CMD_RESUME: {
                //Port can only be resumed if already suspended
                if (port->state == HCD_PORT_STATE_SUSPENDED) {
                    ret = (_port_bus_resume(port)) ? ESP_OK : ESP_ERR_INVALID_RESPONSE;
                }
                break;
            }
            case HCD_PORT_CMD_DISABLE: {
                //Can only disable the port when already enabled or suspended
                if (port->state == HCD_PORT_STATE_ENABLED || port->state == HCD_PORT_STATE_SUSPENDED) {
                    ret = (_port_disable(port)) ? ESP_OK : ESP_ERR_INVALID_RESPONSE;
                }
                break;
            }
        }
        port->flags.cmd_processing = 0;
    }
    HCD_EXIT_CRITICAL();
    xSemaphoreGive(port->port_mux);
    return ret;
}

hcd_port_state_t hcd_port_get_state(hcd_port_handle_t port_hdl)
{
    port_t *port = (port_t *)port_hdl;
    hcd_port_state_t ret;
    HCD_ENTER_CRITICAL();
    ret = port->state;
    HCD_EXIT_CRITICAL();
    return ret;
}

esp_err_t hcd_port_get_speed(hcd_port_handle_t port_hdl, usb_speed_t *speed)
{
    port_t *port = (port_t *)port_hdl;
    HCD_CHECK(speed != NULL, ESP_ERR_INVALID_ARG);
    HCD_ENTER_CRITICAL();
    //Device speed is only valid if there is a resetted device connected to the port
    HCD_CHECK_FROM_CRIT(port->flags.conn_devc_ena, ESP_ERR_INVALID_STATE);
    *speed = usbh_hal_port_get_conn_speed(port->hal);
    HCD_EXIT_CRITICAL();
    return ESP_OK;
}

hcd_port_event_t hcd_port_handle_event(hcd_port_handle_t port_hdl)
{
    port_t *port = (port_t *)port_hdl;
    hcd_port_event_t ret = HCD_PORT_EVENT_NONE;
    xSemaphoreTake(port->port_mux, portMAX_DELAY);
    HCD_ENTER_CRITICAL();
    if (port->initialized && port->flags.event_pending) {
        port->flags.event_pending = 0;
        port->flags.event_processing = 1;
        ret = port->last_event;
        switch (ret) {
            case HCD_PORT_EVENT_CONNECTION: {
                if (_port_debounce(port)) {
                    ret = HCD_PORT_EVENT_CONNECTION;
                }
                break;
            }
            case HCD_PORT_EVENT_DISCONNECTION:
                if (_port_debounce(port)) {
                    //A device is still connected, so it was just a debounce
                    port->state = HCD_PORT_STATE_DISABLED;
                    ret = HCD_PORT_EVENT_NONE;
                } else {
                    //No device conencted after debounce delay. This is an actual disconenction
                    port->state = HCD_PORT_STATE_DISCONNECTED;
                    ret = HCD_PORT_EVENT_DISCONNECTION;
                }
                break;
            case HCD_PORT_EVENT_ERROR:
            case HCD_PORT_EVENT_OVERCURRENT:
            case HCD_PORT_EVENT_SUDDEN_DISCONN: {
                _port_invalidate_all_pipes(port);
                break;
            }
            default: {
                break;
            }
        }
        port->flags.event_processing = 0;
    } else {
        ret = HCD_PORT_EVENT_NONE;
    }
    HCD_EXIT_CRITICAL();
    xSemaphoreGive(port->port_mux);
    return ret;
}

esp_err_t hcd_port_recover(hcd_port_handle_t port_hdl)
{
    port_t *port = (port_t *)port_hdl;
    HCD_ENTER_CRITICAL();
    HCD_CHECK_FROM_CRIT(s_hcd_obj != NULL && port->initialized && port->state == HCD_PORT_STATE_RECOVERY
                        && port->num_pipes_idle == 0 && port->num_pipes_queued == 0
                        && port->flags.val == 0 && port->task_waiting_port_notif == NULL,
                        ESP_ERR_INVALID_STATE);
    //We are about to do a soft reset on the peripheral. Disable the peripheral throughout
    esp_intr_disable(s_hcd_obj->isr_hdl);
    usbh_hal_core_soft_reset(port->hal);
    port->state = HCD_PORT_STATE_NOT_POWERED;
    port->last_event = HCD_PORT_EVENT_NONE;
    port->flags.val = 0;
    esp_intr_enable(s_hcd_obj->isr_hdl);
    HCD_EXIT_CRITICAL();
    return ESP_OK;
}

void *hcd_port_get_ctx(hcd_port_handle_t port_hdl)
{
    port_t *port = (port_t *)port_hdl;
    void *ret;
    HCD_ENTER_CRITICAL();
    ret = port->context;
    HCD_EXIT_CRITICAL();
    return ret;
}

// --------------------------------------------------- HCD Pipes -------------------------------------------------------

// ----------------------- Private -------------------------

static bool _pipe_get_next_xfer_req(pipe_t *pipe)
{
    assert(pipe->inflight_xfer_req == NULL);
    bool ret;
    //This function assigns the next pending transfer request to the inflight_xfer_req
    if (pipe->num_xfer_req_pending > 0) {
        //Set inflight_xfer_req to the next pending transfer request
        pipe->inflight_xfer_req = TAILQ_FIRST(&pipe->pend_xfer_req_tailq);
        TAILQ_REMOVE(&pipe->pend_xfer_req_tailq, pipe->inflight_xfer_req, tailq_entry);
        pipe->inflight_xfer_req->state = XFER_REQ_STATE_INFLIGHT;
        pipe->num_xfer_req_pending--;
        ret =  true;
    } else {
        ret = false;
    }
    return ret;
}

static void _pipe_ret_cur_xfer_req(pipe_t *pipe)
{
    assert(pipe->inflight_xfer_req != NULL);
    //Add the transfer request to the pipe's done tailq
    TAILQ_INSERT_TAIL(&pipe->done_xfer_req_tailq, pipe->inflight_xfer_req, tailq_entry);
    pipe->inflight_xfer_req->state = XFER_REQ_STATE_DONE;
    pipe->inflight_xfer_req = NULL;
    pipe->num_xfer_req_done++;
}

static bool _pipe_wait_done(pipe_t *pipe)
{
    //Check if there is a currently inflight transfer request
    if (pipe->inflight_xfer_req != NULL) {
        //Wait for pipe to complete its transfer
        pipe->flags.waiting_xfer_done = 1;
        _internal_pipe_event_wait(pipe);
        if (pipe->state == HCD_PIPE_STATE_INVALID) {
            //The pipe become invalid whilst waiting for its internal event
            pipe->flags.waiting_xfer_done = 0;  //Need to manually reset this bit in this case
            return false;
        }
        bool chan_halted = usbh_hal_chan_slot_request_halt(pipe->chan_obj);
        assert(chan_halted);
        (void) chan_halted;
    }
    return true;
}

static void _pipe_retire(pipe_t *pipe, bool self_initiated)
{
    //Cannot have any inflight transfer request
    assert(pipe->inflight_xfer_req == NULL);
    if (pipe->num_xfer_req_pending > 0) {
        //Process all remaining pending transfer requests
        xfer_req_t *xfer_req;
        TAILQ_FOREACH(xfer_req, &pipe->pend_xfer_req_tailq, tailq_entry) {
            xfer_req->state = XFER_REQ_STATE_DONE;
            //If we are initiating the retire, mark the transfer request as cancelled
            xfer_req->irp->status = (self_initiated) ? USB_TRANSFER_STATUS_CANCELLED : USB_TRANSFER_STATUS_NO_DEVICE;
        }
        //Concatenated pending tailq to the done tailq
        TAILQ_CONCAT(&pipe->done_xfer_req_tailq, &pipe->pend_xfer_req_tailq, tailq_entry);
        pipe->num_xfer_req_done += pipe->num_xfer_req_pending;
        pipe->num_xfer_req_pending = 0;
    }
}

static inline hcd_pipe_event_t pipe_decode_error_event(usbh_hal_chan_error_t chan_error)
{
    hcd_pipe_event_t event = HCD_PIPE_EVENT_NONE;
    switch (chan_error) {
        case USBH_HAL_CHAN_ERROR_XCS_XACT:
            event = HCD_PIPE_EVENT_ERROR_XFER;
            break;
        case USBH_HAL_CHAN_ERROR_BNA:
            event = HCD_PIPE_EVENT_ERROR_XFER_NOT_AVAIL;
            break;
        case USBH_HAL_CHAN_ERROR_PKT_BBL:
            event = HCD_PIPE_EVENT_ERROR_OVERFLOW;
            break;
        case USBH_HAL_CHAN_ERROR_STALL:
            event = HCD_PIPE_EVENT_ERROR_STALL;
            break;
    }
    return event;
}

// ----------------------- Public --------------------------

esp_err_t hcd_pipe_alloc(hcd_port_handle_t port_hdl, const hcd_pipe_config_t *pipe_config, hcd_pipe_handle_t *pipe_hdl)
{
    HCD_CHECK(port_hdl != NULL && pipe_config != NULL && pipe_hdl != NULL, ESP_ERR_INVALID_ARG);
    port_t *port = (port_t *)port_hdl;
    HCD_ENTER_CRITICAL();
    //Can only allocate a pipe if the targetted port is initialized and conencted to an enabled device
    HCD_CHECK_FROM_CRIT(port->initialized && port->flags.conn_devc_ena, ESP_ERR_INVALID_STATE);
    usb_speed_t port_speed = port->speed;
    HCD_EXIT_CRITICAL();
    //Cannot connect to a FS device if the port is LS
    HCD_CHECK(port_speed == USB_SPEED_FULL || (port_speed == USB_SPEED_LOW && pipe_config->dev_speed == USB_SPEED_LOW), ESP_ERR_NOT_SUPPORTED);

    esp_err_t ret = ESP_OK;
    //Get the type of pipe to allocate
    usb_xfer_type_t type;
    bool is_default_pipe;
    if (pipe_config->ep_desc == NULL) {  //A NULL ep_desc indicates we are allocating a default pipe
        type = USB_XFER_TYPE_CTRL;
        is_default_pipe = true;
    } else {
        type = USB_DESC_EP_GET_XFERTYPE(pipe_config->ep_desc);
        is_default_pipe = false;
    }
    size_t num_xfer_desc = 0;
    switch (type) {
        case USB_XFER_TYPE_CTRL: {
            num_xfer_desc = XFER_LIST_LEN_CTRL * NUM_DESC_PER_XFER_CTRL;
            break;
        }
        case USB_XFER_TYPE_BULK: {
            if (pipe_config->dev_speed == USB_SPEED_LOW) {
                return ESP_ERR_NOT_SUPPORTED;   //Low speed devices do not support bulk transfers
            }
            num_xfer_desc = XFER_LIST_LEN_BULK * NUM_DESC_PER_XFER_BULK;
            break;
        }
        default: {
            //Isochronous and Interrupt pipes currently not supported
            return ESP_ERR_NOT_SUPPORTED;
        }
    }

    //Allocate the pipe resources
    pipe_t *pipe = calloc(1, sizeof(pipe_t));
    usbh_hal_chan_t *chan_obj = malloc(sizeof(usbh_hal_chan_t));
    void *xfer_desc_list = heap_caps_aligned_calloc(USBH_HAL_DMA_MEM_ALIGN, num_xfer_desc, USBH_HAL_XFER_DESC_SIZE, MALLOC_CAP_DMA);
    if (pipe == NULL|| chan_obj == NULL || xfer_desc_list == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    //Initialize pipe object
    TAILQ_INIT(&pipe->pend_xfer_req_tailq);
    TAILQ_INIT(&pipe->done_xfer_req_tailq);
    pipe->port = port;
    pipe->xfer_desc_list = xfer_desc_list;
    pipe->flags.xfer_desc_list_len = num_xfer_desc;
    pipe->chan_obj = chan_obj;
    pipe->ep_char.type = type;
    if (is_default_pipe) {
        pipe->ep_char.bEndpointAddress = 0;
        //Set the default pipe's MPS to the worst case MPS for the device's speed
        pipe->ep_char.mps = (pipe_config->dev_speed == USB_SPEED_FULL) ? CTRL_EP_MAX_MPS_FS : CTRL_EP_MAX_MPS_LS;
    } else {
        pipe->ep_char.bEndpointAddress = pipe_config->ep_desc->bEndpointAddress;
        pipe->ep_char.mps = pipe_config->ep_desc->wMaxPacketSize;
    }
    pipe->ep_char.dev_addr = pipe_config->dev_addr;
    pipe->ep_char.ls_via_fs_hub = (port_speed == USB_SPEED_FULL && pipe_config->dev_speed == USB_SPEED_LOW);
    pipe->state = HCD_PIPE_STATE_ACTIVE;
    pipe->callback = pipe_config->callback;
    pipe->callback_arg = pipe_config->callback_arg;
    pipe->context = pipe_config->context;

    //Allocate channel
    HCD_ENTER_CRITICAL();
    if (!port->initialized || !port->flags.conn_devc_ena) {
        HCD_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    bool chan_allocated = usbh_hal_chan_alloc(port->hal, pipe->chan_obj, (void *) pipe);
    if (!chan_allocated) {
        HCD_EXIT_CRITICAL();
        ret = ESP_ERR_NOT_SUPPORTED;
        goto err;
    }
    usbh_hal_chan_set_ep_char(pipe->chan_obj, &pipe->ep_char);

    //Add the pipe to the list of idle pipes in the port object
    TAILQ_INSERT_TAIL(&port->pipes_idle_tailq, pipe, tailq_entry);
    port->num_pipes_idle++;
    HCD_EXIT_CRITICAL();
    *pipe_hdl = (hcd_pipe_handle_t)pipe;
    return ret;

err:
    free(xfer_desc_list);
    free(chan_obj);
    free(pipe);
    return ret;
}

esp_err_t hcd_pipe_free(hcd_pipe_handle_t pipe_hdl)
{
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    HCD_ENTER_CRITICAL();
    //Check that all transfer requests have been removed and pipe has no pending events
    HCD_CHECK_FROM_CRIT(pipe->inflight_xfer_req == NULL
                        && pipe->num_xfer_req_pending == 0
                        && pipe->num_xfer_req_done == 0,
                        ESP_ERR_INVALID_STATE);
    //Remove pipe from the list of idle pipes (it must be in the idle list because it should have no queued transfer requests)
    TAILQ_REMOVE(&pipe->port->pipes_idle_tailq, pipe, tailq_entry);
    pipe->port->num_pipes_idle--;
    usbh_hal_chan_free(pipe->port->hal, pipe->chan_obj);
    HCD_EXIT_CRITICAL();

    //Free pipe resources
    free(pipe->xfer_desc_list);
    free(pipe->chan_obj);
    free(pipe);
    return ESP_OK;
}

esp_err_t hcd_pipe_update(hcd_pipe_handle_t pipe_hdl, uint8_t dev_addr, int mps)
{
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    HCD_ENTER_CRITICAL();
    //Check if pipe is in the correct state to be updated
    HCD_CHECK_FROM_CRIT(pipe->state != HCD_PIPE_STATE_INVALID
                        && !pipe->flags.pipe_cmd_processing
                        && pipe->num_xfer_req_pending == 0
                        && pipe->num_xfer_req_done == 0,
                        ESP_ERR_INVALID_STATE);
    //Check that all transfer requests have been removed and pipe has no pending events
    pipe->ep_char.dev_addr = dev_addr;
    pipe->ep_char.mps = mps;
    usbh_hal_chan_set_ep_char(pipe->chan_obj, &pipe->ep_char);
    HCD_EXIT_CRITICAL();
    return ESP_OK;
}

void *hcd_pipe_get_ctx(hcd_pipe_handle_t pipe_hdl)
{
    pipe_t *pipe = (pipe_t *) pipe_hdl;
    void *ret;
    HCD_ENTER_CRITICAL();
    ret = pipe->context;
    HCD_EXIT_CRITICAL();
    return ret;
}

hcd_pipe_state_t hcd_pipe_get_state(hcd_pipe_handle_t pipe_hdl)
{
    hcd_pipe_state_t ret;
    pipe_t *pipe = (pipe_t *) pipe_hdl;
    HCD_ENTER_CRITICAL();
    //If there is no enabled device, all existing pipes are invalid.
    if (pipe->port->state != HCD_PORT_STATE_ENABLED
        && pipe->port->state != HCD_PORT_STATE_SUSPENDED
        && pipe->port->state != HCD_PORT_STATE_RESUMING) {
            ret = HCD_PIPE_STATE_INVALID;
    } else {
        ret = pipe->state;
    }
    HCD_EXIT_CRITICAL();
    return ret;
}

esp_err_t hcd_pipe_command(hcd_pipe_handle_t pipe_hdl, hcd_pipe_cmd_t command)
{
    pipe_t *pipe = (pipe_t *) pipe_hdl;
    bool ret = ESP_OK;

    HCD_ENTER_CRITICAL();
    //Cannot execute pipe commands the pipe is already executing a command, or if the pipe or its port are no longer valid
    if (pipe->flags.pipe_cmd_processing || !pipe->port->flags.conn_devc_ena || pipe->state == HCD_PIPE_STATE_INVALID) {
        ret = ESP_ERR_INVALID_STATE;
    } else {
        pipe->flags.pipe_cmd_processing = 1;
        switch (command) {
            case HCD_PIPE_CMD_ABORT: {
                //Retire all scheduled transfer requests. Pipe's state remains unchanged
                if (!_pipe_wait_done(pipe)) {   //Stop any on going transfers
                    ret = ESP_ERR_INVALID_RESPONSE;
                    break;
                }
                _pipe_retire(pipe, true);  //Retire any pending transfers
                break;
            }
            case HCD_PIPE_CMD_RESET: {
                //Retire all scheduled transfer requests. Pipe's state moves to active
                if (!_pipe_wait_done(pipe)) {   //Stop any on going transfers
                    ret = ESP_ERR_INVALID_RESPONSE;
                    break;
                }
                _pipe_retire(pipe, true);  //Retire any pending transfers
                pipe->state = HCD_PIPE_STATE_ACTIVE;
                break;
            }
            case HCD_PIPE_CMD_CLEAR: {  //Can only do this if port is still active
                //Pipe's state moves from halted to active
                if (pipe->state == HCD_PIPE_STATE_HALTED) {
                    pipe->state = HCD_PIPE_STATE_ACTIVE;
                    //Start the next pending transfer if it exists
                    if (_pipe_get_next_xfer_req(pipe)) {
                        //Fill the descriptor list with the transfer request and start the transfer
                        _xfer_req_fill(pipe);
                        usbh_hal_chan_activate(pipe->chan_obj, 0);  //Start with the first descriptor
                    }
                }
                break;
            }
            case HCD_PIPE_CMD_HALT: {
                //Pipe's state moves to halted
                if (!_pipe_wait_done(pipe)) {   //Stop any on going transfers
                    ret = ESP_ERR_INVALID_RESPONSE;
                    break;
                }
                pipe->state = HCD_PIPE_STATE_HALTED;
                break;
            }
        }
        pipe->flags.pipe_cmd_processing = 0;
    }
    HCD_EXIT_CRITICAL();
    return ret;
}

hcd_pipe_event_t hcd_pipe_get_event(hcd_pipe_handle_t pipe_hdl)
{
    pipe_t *pipe = (pipe_t *) pipe_hdl;
    hcd_pipe_event_t ret;
    HCD_ENTER_CRITICAL();
    ret = pipe->last_event;
    pipe->last_event = HCD_PIPE_EVENT_NONE;
    HCD_EXIT_CRITICAL();
    return ret;
}

// ----------------------------------------------- HCD Transfer Requests -----------------------------------------------

// ----------------------- Private -------------------------

static void _xfer_req_fill(pipe_t *pipe)
{
    //inflight_xfer_req of the pipe must already set to the target transfer request
    assert(pipe->inflight_xfer_req != NULL);
    //Fill transfer descriptor list with a single transfer request
    usb_irp_t *usb_irp = pipe->inflight_xfer_req->irp;
    switch (pipe->ep_char.type) {
        case USB_XFER_TYPE_CTRL: {
            //Get information about the contorl transfer by analyzing the setup packet (the first 8 bytes)
            usb_ctrl_req_t *ctrl_req = (usb_ctrl_req_t *)usb_irp->data_buffer;
            pipe->flags.ctrl_data_stg_in = ((ctrl_req->bRequestType & USB_B_REQUEST_TYPE_DIR_IN) != 0);
            pipe->flags.ctrl_data_stg_skip = (usb_irp->num_bytes == 0);

            //Fill setup stage
            usbh_hal_xfer_desc_fill(pipe->xfer_desc_list, 0, usb_irp->data_buffer, sizeof(usb_ctrl_req_t),
                                    USBH_HAL_XFER_DESC_FLAG_SETUP | USBH_HAL_XFER_DESC_FLAG_HALT);
            if (pipe->flags.ctrl_data_stg_skip) {
                //Fill a NULL packet if there is no data stage
                usbh_hal_xfer_desc_fill(pipe->xfer_desc_list, 1, NULL, 0, USBH_HAL_XFER_DESC_FLAG_NULL);
            } else {
                //Fill data stage
                usbh_hal_xfer_desc_fill(pipe->xfer_desc_list, 1, usb_irp->data_buffer + sizeof(usb_ctrl_req_t), usb_irp->num_bytes,
                                        ((pipe->flags.ctrl_data_stg_in) ? USBH_HAL_XFER_DESC_FLAG_IN : 0) | USBH_HAL_XFER_DESC_FLAG_HALT);
            }
            //Fill status stage (i.e., a zero length packet). If data stage is skipped, the status stage is always IN.
            usbh_hal_xfer_desc_fill(pipe->xfer_desc_list, 2, NULL, 0,
                                    ((pipe->flags.ctrl_data_stg_in && !pipe->flags.ctrl_data_stg_skip) ? 0 : USBH_HAL_XFER_DESC_FLAG_IN) | USBH_HAL_XFER_DESC_FLAG_HALT);
            //Set the channel's direction to OUT and PID to 0 respectively for the the setup stage
            usbh_hal_chan_set_dir(pipe->chan_obj, false);   //Setup stage is always OUT
            usbh_hal_chan_set_pid(pipe->chan_obj, 0);   //Setup stage always has a PID of DATA0
            break;
        }
        case USB_XFER_TYPE_BULK: {
            bool is_in = pipe->ep_char.bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK;
            usbh_hal_xfer_desc_fill(pipe->xfer_desc_list, 0, usb_irp->data_buffer, usb_irp->num_bytes,
                                    ((is_in) ? USBH_HAL_XFER_DESC_FLAG_IN : 0) | USBH_HAL_XFER_DESC_FLAG_HALT);
            break;
        }
        default: {
            break;  //Isoc and Interrupt transfers not supported yet
        }
    }
    //Claim slot
    usbh_hal_chan_slot_acquire(pipe->chan_obj, pipe->xfer_desc_list, pipe->flags.xfer_desc_list_len, (void *)pipe);
}

static void _xfer_req_continue(pipe_t *pipe)
{
    int next_idx = usbh_hal_chan_get_next_desc_index(pipe->chan_obj);
    bool next_dir_is_in;    //Next descriptor direction is IN
    int next_pid;           //Next PID (DATA0 or DATA 1)
    int num_to_skip;        //Number of descriptors to skip
    if (next_idx == 1) {
        //Just finished setup stage
        if (pipe->flags.ctrl_data_stg_skip) {
            //Skipping data stage. Go straight to status stage
            next_dir_is_in = true;     //With no data stage, status stage must be IN
            next_pid = 1;       //Status stage always has a PID of DATA1
            num_to_skip = 1;    //Skip over the null descriptor representing the skipped data stage
        } else {
            //Go to data stage
            next_dir_is_in = pipe->flags.ctrl_data_stg_in;
            next_pid = 1;   //Data stage always starts with a PID of DATA1
            num_to_skip = 0;
        }
    } else {    //next_idx == 2
        //Going to status stage from data stage
        next_dir_is_in = !pipe->flags.ctrl_data_stg_in;     //Status stage is opposite direction of data stage
        next_pid = 1;   //Status stage always has a PID of DATA1
        num_to_skip = 0;
    }

    usbh_hal_chan_set_dir(pipe->chan_obj, next_dir_is_in);
    usbh_hal_chan_set_pid(pipe->chan_obj, next_pid);
    usbh_hal_chan_activate(pipe->chan_obj, num_to_skip);    //Start the next stage
}

static void _xfer_req_parse(pipe_t *pipe, bool error_occurred)
{
    assert(pipe->inflight_xfer_req != NULL);
    //Release the slot
    void *xfer_desc_list;
    int xfer_desc_len;
    usbh_hal_chan_slot_release(pipe->chan_obj, &xfer_desc_list, &xfer_desc_len);
    assert(xfer_desc_list == pipe->xfer_desc_list);
    (void) xfer_desc_len;

    //Parse the transfer descriptor list for the result of the transfer
    usb_irp_t *usb_irp = pipe->inflight_xfer_req->irp;
    usb_transfer_status_t xfer_status;
    int xfer_rem_len;
    if (error_occurred) {
        //Either a pipe error has occurred or the pipe is no longer valid
        if (pipe->state == HCD_PIPE_STATE_INVALID) {
            xfer_status = USB_TRANSFER_STATUS_NO_DEVICE;
        } else {
            //Must have been a pipe error event
            switch (pipe->last_event) {
                case HCD_PIPE_EVENT_ERROR_XFER: //Excessive transaction error
                    xfer_status = USB_TRANSFER_STATUS_ERROR;
                    break;
                case HCD_PIPE_EVENT_ERROR_OVERFLOW:
                    xfer_status = USB_TRANSFER_STATUS_OVERFLOW;
                    break;
                case HCD_PIPE_EVENT_ERROR_STALL:
                    xfer_status = USB_TRANSFER_STATUS_STALL;
                    break;
                default:
                    //HCD_PIPE_EVENT_ERROR_XFER_NOT_AVAIL should never occur
                    abort();
                    break;
            }
        }
        //We assume no bytes transmitted because of an error.
        xfer_rem_len = usb_irp->num_bytes;
    } else {
        int desc_status;
        switch (pipe->ep_char.type) {
            case USB_XFER_TYPE_CTRL: {
                if (pipe->flags.ctrl_data_stg_skip) {
                    //There was no data stage. Just set it as successful
                    desc_status = USBH_HAL_XFER_DESC_STS_SUCCESS;
                    xfer_rem_len = 0;
                } else {
                    //Check the data stage (index 1)
                    usbh_hal_xfer_desc_parse(pipe->xfer_desc_list, 1, &xfer_rem_len, &desc_status);
                }
                break;
            }
            case USB_XFER_TYPE_BULK: {
                usbh_hal_xfer_desc_parse(pipe->xfer_desc_list, 0, &xfer_rem_len, &desc_status);
                break;
            }
            default: {
                //We don't supportISOC and INTR pipes yet
                desc_status = USBH_HAL_XFER_DESC_STS_NOT_EXECUTED;
                xfer_rem_len = 0;
                xfer_status = USB_TRANSFER_STATUS_ERROR;
                abort();
                break;
            }
        }
        xfer_status = USB_TRANSFER_STATUS_COMPLETED;
        assert(desc_status == USBH_HAL_XFER_DESC_STS_SUCCESS);
    }
    //Write back results to IRP
    usb_irp->actual_num_bytes = usb_irp->num_bytes - xfer_rem_len;
    usb_irp->status = xfer_status;
}

// ----------------------- Public --------------------------

hcd_xfer_req_handle_t hcd_xfer_req_alloc()
{
    xfer_req_t *xfer_req = calloc(1, sizeof(xfer_req_t));
    xfer_req->state = XFER_REQ_STATE_IDLE;
    return (hcd_xfer_req_handle_t) xfer_req;
}

void hcd_xfer_req_free(hcd_xfer_req_handle_t req_hdl)
{
    if (req_hdl == NULL) {
        return;
    }
    xfer_req_t *xfer_req = (xfer_req_t *) req_hdl;
    //Cannot free a transfer request that is still being used
    assert(xfer_req->state == XFER_REQ_STATE_IDLE);
    free(xfer_req);
}

void hcd_xfer_req_set_target(hcd_xfer_req_handle_t req_hdl, hcd_pipe_handle_t pipe_hdl, usb_irp_t *irp, void *context)
{
    xfer_req_t *xfer_req = (xfer_req_t *) req_hdl;
    //Can only set an transfer request's target when the transfer request is idl
    assert(xfer_req->state == XFER_REQ_STATE_IDLE);
    xfer_req->pipe = (pipe_t *) pipe_hdl;
    xfer_req->irp = irp;
    xfer_req->context = context;
}

void hcd_xfer_req_get_target(hcd_xfer_req_handle_t req_hdl, hcd_pipe_handle_t *pipe_hdl, usb_irp_t **irp, void **context)
{
    xfer_req_t *xfer_req = (xfer_req_t *) req_hdl;
    *pipe_hdl = (hcd_pipe_handle_t) xfer_req->pipe;
    *irp = xfer_req->irp;
    *context = xfer_req->context;
}

esp_err_t hcd_xfer_req_enqueue(hcd_xfer_req_handle_t req_hdl)
{
    xfer_req_t *xfer_req = (xfer_req_t *) req_hdl;
    HCD_CHECK(xfer_req->pipe != NULL && xfer_req->irp != NULL       //The transfer request's target must be set
              && xfer_req->state == XFER_REQ_STATE_IDLE,    //The transfer request cannot be already enqueued
              ESP_ERR_INVALID_STATE);
    pipe_t *pipe = xfer_req->pipe;
    HCD_ENTER_CRITICAL();
    HCD_CHECK_FROM_CRIT(pipe->port->state == HCD_PORT_STATE_ENABLED     //The pipe's port must be in the correct state
                        && pipe->state == HCD_PIPE_STATE_ACTIVE         //The pipe must be in the correct state
                        && !pipe->flags.pipe_cmd_processing,            //Pipe cannot currently be processing a pipe command
                        ESP_ERR_INVALID_STATE);
    //Check if we can start execution on the pipe immediately
    if (!pipe->flags.paused && pipe->num_xfer_req_pending == 0 && pipe->inflight_xfer_req == NULL) {
        //Pipe isn't executing any transfers. Start immediately
        pipe->inflight_xfer_req = xfer_req;
        _xfer_req_fill(pipe);
        usbh_hal_chan_activate(pipe->chan_obj, 0);  //Start with the first descriptor
        xfer_req->state = XFER_REQ_STATE_INFLIGHT;
        if (pipe->num_xfer_req_done == 0) {
            //This is the first transfer request to be enqueued into the pipe. Move the pipe to the list of queued pipes
            TAILQ_REMOVE(&pipe->port->pipes_idle_tailq, pipe, tailq_entry);
            TAILQ_INSERT_TAIL(&pipe->port->pipes_queued_tailq, pipe, tailq_entry);
            pipe->port->num_pipes_idle--;
            pipe->port->num_pipes_queued++;
        }
    } else {
        //Add the transfer request to the pipe's pending tailq
        TAILQ_INSERT_TAIL(&pipe->pend_xfer_req_tailq, xfer_req, tailq_entry);
        pipe->num_xfer_req_pending++;
        xfer_req->state = XFER_REQ_STATE_PENDING;
    }
    HCD_EXIT_CRITICAL();
    return ESP_OK;
}

hcd_xfer_req_handle_t hcd_xfer_req_dequeue(hcd_pipe_handle_t pipe_hdl)
{
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    hcd_xfer_req_handle_t ret;

    HCD_ENTER_CRITICAL();
    if (pipe->num_xfer_req_done > 0) {
        xfer_req_t *xfer_req = TAILQ_FIRST(&pipe->done_xfer_req_tailq);
        TAILQ_REMOVE(&pipe->done_xfer_req_tailq, xfer_req, tailq_entry);
        pipe->num_xfer_req_done--;
        assert(xfer_req->state == XFER_REQ_STATE_DONE);
        xfer_req->state = XFER_REQ_STATE_IDLE;
        ret = (hcd_xfer_req_handle_t) xfer_req;
        if (pipe->num_xfer_req_done == 0 && pipe->num_xfer_req_pending == 0) {
            //This pipe has no more enqueued transfers. Move the pipe to the list of idle pipes
            TAILQ_REMOVE(&pipe->port->pipes_queued_tailq, pipe, tailq_entry);
            TAILQ_INSERT_TAIL(&pipe->port->pipes_idle_tailq, pipe, tailq_entry);
            pipe->port->num_pipes_idle++;
            pipe->port->num_pipes_queued--;
        }
    } else {
        ret = NULL;
    }
    HCD_EXIT_CRITICAL();
    return ret;
}

esp_err_t hcd_xfer_req_abort(hcd_xfer_req_handle_t req_hdl)
{
    xfer_req_t *xfer_req = (xfer_req_t *) req_hdl;
    esp_err_t ret;

    HCD_ENTER_CRITICAL();
    switch (xfer_req->state) {
        case XFER_REQ_STATE_PENDING: {
            //Transfer request has not been executed so it can be aborted
            pipe_t *pipe = xfer_req->pipe;
            //Remove it form the pending queue
            TAILQ_REMOVE(&pipe->pend_xfer_req_tailq, xfer_req, tailq_entry);
            pipe->num_xfer_req_pending--;
            //Add it to the done queue
            TAILQ_INSERT_TAIL(&pipe->done_xfer_req_tailq, xfer_req, tailq_entry);
            pipe->num_xfer_req_done++;
            //Update the transfer request and associated IRP's status
            xfer_req->state = XFER_REQ_STATE_DONE;
            xfer_req->irp->status = USB_TRANSFER_STATUS_CANCELLED;
            ret = ESP_OK;
            break;
        }
        case XFER_REQ_STATE_IDLE: {
            //Cannot abort a transfer request that was never enqueued
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
        default :{
            //Transfer request is currently or has already been executed. Nothing to do.
            ret = ESP_OK;
            break;
        }
    }
    HCD_EXIT_CRITICAL();
    return ret;
}
