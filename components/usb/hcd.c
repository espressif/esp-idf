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

#include <stdint.h>
#include <string.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_rom_gpio.h"
#include "hal/usbh_hal.h"
#include "hal/usb_types_private.h"
#include "soc/gpio_pins.h"
#include "soc/gpio_sig_map.h"
#include "driver/periph_ctrl.h"
#include "usb.h"
#include "hcd.h"

// ----------------------------------------------------- Macros --------------------------------------------------------

// --------------------- Constants -------------------------

#define INIT_DELAY_MS                           30  //A delay of at least 25ms to enter Host mode. Make it 30ms to be safe
#define DEBOUNCE_DELAY_MS                       250 //A debounce delay of 250ms
#define RESET_HOLD_MS                           30  //Spec requires at least 10ms. Make it 30ms to be safe
#define RESET_RECOVERY_MS                       30  //Reset recovery delay of 10ms (make it 30 ms to be safe) to allow for connected device to recover (and for port enabled interrupt to occur)
#define RESUME_HOLD_MS                          30  //Spec requires at least 20ms, Make it 30ms to be safe
#define RESUME_RECOVERY_MS                      20  //Resume recovery of at least 10ms. Make it 20 ms to be safe. This will include the 3 LS bit times of the EOP

#define CTRL_EP_MAX_MPS_LS                      8   //Largest Maximum Packet Size for Low Speed control endpoints
#define CTRL_EP_MAX_MPS_FS                      64  //Largest Maximum Packet Size for Full Speed control endpoints

#define NUM_PORTS                               1   //The controller only has one port.

// ----------------------- Configs -------------------------

typedef struct {
    int in_mps;
    int non_periodic_out_mps;
    int periodic_out_mps;
} fifo_mps_limits_t;

/**
 * @brief Default FIFO sizes (see 2.1.2.4 for programming guide)
 *
 * RXFIFO
 * - Recommended: ((LPS/4) * 2) + 2
 * - Actual: Whatever leftover size: USBH_HAL_FIFO_TOTAL_USABLE_LINES(200) - 48 - 48 = 104
 * - Worst case can accommodate two packets of 204 bytes, or one packet of 408
 * NPTXFIFO
 * - Recommended: (LPS/4) * 2
 * - Actual: Assume LPS is 64, and 3 packets: (64/4) * 3 = 48
 * - Worst case can accommodate three packets of 64 bytes or one packet of 192
 * PTXFIFO
 * - Recommended: (LPS/4) * 2
 * - Actual: Assume LPS is 64, and 3 packets: (64/4) * 3 = 48
 * - Worst case can accommodate three packets of 64 bytes or one packet of 192
 */
const usbh_hal_fifo_config_t fifo_config_default = {
    .rx_fifo_lines = 104,
    .nptx_fifo_lines = 48,
    .ptx_fifo_lines = 48,
};

const fifo_mps_limits_t mps_limits_default = {
    .in_mps = 408,
    .non_periodic_out_mps = 192,
    .periodic_out_mps = 192,
};

/**
 * @brief FIFO sizes that bias to giving RX FIFO more capacity
 *
 * RXFIFO
 * - Recommended: ((LPS/4) * 2) + 2
 * - Actual: Whatever leftover size: USBH_HAL_FIFO_TOTAL_USABLE_LINES(200) - 32 - 16 = 152
 * - Worst case can accommodate two packets of 300 bytes or one packet of 600 bytes
 * NPTXFIFO
 * - Recommended: (LPS/4) * 2
 * - Actual: Assume LPS is 64, and 1 packets: (64/4) * 1 = 16
 * - Worst case can accommodate one packet of 64 bytes
 * PTXFIFO
 * - Recommended: (LPS/4) * 2
 * - Actual: Assume LPS is 64, and 3 packets: (64/4) * 2 = 32
 * - Worst case can accommodate two packets of 64 bytes or one packet of 128
 */
const usbh_hal_fifo_config_t fifo_config_bias_rx = {
    .rx_fifo_lines = 152,
    .nptx_fifo_lines = 16,
    .ptx_fifo_lines = 32,
};

const fifo_mps_limits_t mps_limits_bias_rx = {
    .in_mps = 600,
    .non_periodic_out_mps = 64,
    .periodic_out_mps = 128,
};

/**
 * @brief FIFO sizes that bias to giving Periodic TX FIFO more capacity (i.e., ISOC OUT)
 *
 * RXFIFO
 * - Recommended: ((LPS/4) * 2) + 2
 * - Actual: Assume LPS is 64, and 2 packets: ((64/4) * 2) + 2 = 34
 * - Worst case can accommodate two packets of 64 bytes or one packet of 128
 * NPTXFIFO
 * - Recommended: (LPS/4) * 2
 * - Actual: Assume LPS is 64, and 1 packets: (64/4) * 1 = 16
 * - Worst case can accommodate one packet of 64 bytes
 * PTXFIFO
 * - Recommended: (LPS/4) * 2
 * - Actual: Whatever leftover size: USBH_HAL_FIFO_TOTAL_USABLE_LINES(200) - 34 - 16 = 150
 * - Worst case can accommodate two packets of 300 bytes or one packet of 600 bytes
 */
const usbh_hal_fifo_config_t fifo_config_bias_ptx = {
    .rx_fifo_lines = 34,
    .nptx_fifo_lines = 16,
    .ptx_fifo_lines = 150,
};

const fifo_mps_limits_t mps_limits_bias_ptx = {
    .in_mps = 128,
    .non_periodic_out_mps = 64,
    .periodic_out_mps = 600,
};

#define FRAME_LIST_LEN                          USB_HAL_FRAME_LIST_LEN_32
#define NUM_BUFFERS                             2

#define XFER_LIST_LEN_CTRL                      3   //One descriptor for each stage
#define XFER_LIST_LEN_BULK                      2   //One descriptor for transfer, one to support an extra zero length packet
#define XFER_LIST_LEN_INTR                      32
#define XFER_LIST_LEN_ISOC                      FRAME_LIST_LEN  //Same length as the frame list makes it easier to schedule. Must be power of 2

// ------------------------ Flags --------------------------

/**
 * @brief Bit masks for the HCD to use in the IRPs reserved_flags field
 *
 * The IRP object has a reserved_flags member for host stack's internal use. The following flags will be set in
 * reserved_flags in order to keep track of state of an IRP within the HCD.
 */
#define IRP_STATE_IDLE                          0x0 //The IRP is not enqueued in an HCD pipe
#define IRP_STATE_PENDING                       0x1 //The IRP is enqueued and pending execution
#define IRP_STATE_INFLIGHT                      0x2 //The IRP is currently in flight
#define IRP_STATE_DONE                          0x3 //The IRP has completed execution or is retired, and is waiting to be dequeued
#define IRP_STATE_MASK                          0x3 //Bit mask of all the IRP state flags
#define IRP_STATE_SET(reserved_flags, state)    (reserved_flags = (reserved_flags & ~IRP_STATE_MASK) | state)
#define IRP_STATE_GET(reserved_flags)           (reserved_flags & IRP_STATE_MASK)

// -------------------- Convenience ------------------------

#define HCD_ENTER_CRITICAL_ISR()                portENTER_CRITICAL_ISR(&hcd_lock)
#define HCD_EXIT_CRITICAL_ISR()                 portEXIT_CRITICAL_ISR(&hcd_lock)
#define HCD_ENTER_CRITICAL()                    portENTER_CRITICAL(&hcd_lock)
#define HCD_EXIT_CRITICAL()                     portEXIT_CRITICAL(&hcd_lock)

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

typedef struct pipe_obj pipe_t;
typedef struct port_obj port_t;

/**
 * @brief Object representing a single buffer of a pipe's multi buffer implementation
 */
typedef struct {
    void *xfer_desc_list;
    usb_irp_t *irp;
    union {
        struct {
            uint32_t data_stg_in: 1;        //Data stage of the control transfer is IN
            uint32_t data_stg_skip: 1;      //Control transfer has no data stage
            uint32_t cur_stg: 2;            //Index of the current stage (e.g., 0 is setup stage, 2 is status stage)
            uint32_t reserved28: 28;
        } ctrl;                             //Control transfer related
        struct {
            uint32_t zero_len_packet: 1;    //Bulk transfer should add a zero length packet at the end regardless
            uint32_t reserved31: 31;
        } bulk;                             //Bulk transfer related
        struct {
            uint32_t num_qtds: 8;           //Number of transfer descriptors filled
            uint32_t reserved24: 24;
        } intr;                             //Interrupt transfer related
        struct {
            uint32_t num_qtds: 8;           //Number of transfer descriptors filled (including NULL descriptors)
            uint32_t interval: 8;           //Interval (in number of SOF i.e., ms)
            uint32_t irp_start_idx: 8;      //Index of the first transfer descriptor in the list
            uint32_t next_irp_start_idx: 8; //Index for the first descriptor of the next buffer
        } isoc;
        uint32_t val;
    } flags;
    union {
        struct {
            uint32_t stop_idx: 8;           //The descriptor index when the channel was halted
            uint32_t executing: 1;          //The buffer is currently executing
            uint32_t error_occurred: 1;     //An error occurred
            uint32_t cancelled: 1;          //The buffer was actively cancelled
            uint32_t reserved5: 5;
            hcd_pipe_state_t pipe_state: 8; //The pipe's state when the error occurred
            hcd_pipe_event_t pipe_event: 8; //The pipe event when the error occurred
        };
        uint32_t val;
    } status_flags;                         //Status flags for the buffer
} dma_buffer_block_t;

/**
 * @brief Object representing a pipe in the HCD layer
 */
struct pipe_obj {
    //IRP queueing related
    TAILQ_HEAD(tailhead_irp_pending, usb_irp_obj) pending_irp_tailq;
    TAILQ_HEAD(tailhead_irp_done, usb_irp_obj) done_irp_tailq;
    int num_irp_pending;
    int num_irp_done;
    //Multi-buffer control
    dma_buffer_block_t *buffers[NUM_BUFFERS];  //Double buffering scheme
    union {
        struct {
            uint32_t buffer_num_to_fill: 2; //Number of buffers that can be filled
            uint32_t buffer_num_to_exec: 2; //Number of buffers that are filled and need to be executed
            uint32_t buffer_num_to_parse: 2;//Number of buffers completed execution and waiting to be parsed
            uint32_t reserved2: 2;
            uint32_t wr_idx: 1;             //Index of the next buffer to fill. Bit width must allow NUM_BUFFERS to wrap automatically
            uint32_t rd_idx: 1;             //Index of the current buffer in-flight. Bit width must allow NUM_BUFFERS to wrap automatically
            uint32_t fr_idx: 1;             //Index of the next buffer to parse. Bit width must allow NUM_BUFFERS to wrap automatically
            uint32_t buffer_is_executing: 1;//One of the buffers is in flight
            uint32_t reserved20: 20;
        };
        uint32_t val;
    } multi_buffer_control;
    //HAL related
    usbh_hal_chan_t *chan_obj;
    usbh_hal_ep_char_t ep_char;
    //Port related
    port_t *port;                           //The port to which this pipe is routed through
    TAILQ_ENTRY(pipe_obj) tailq_entry;      //TailQ entry for port's list of pipes
    //Pipe status/state/events related
    hcd_pipe_state_t state;
    hcd_pipe_event_t last_event;
    TaskHandle_t task_waiting_pipe_notif;   //Task handle used for internal pipe events
    union {
        struct {
            uint32_t waiting_xfer_done: 1;
            uint32_t paused: 1;
            uint32_t pipe_cmd_processing: 1;
            uint32_t is_active: 1;
            uint32_t reserved28: 28;
        };
        uint32_t val;
    } cs_flags;
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
    void *frame_list;
    //Pipes routed through this port
    TAILQ_HEAD(tailhead_pipes_idle, pipe_obj) pipes_idle_tailq;
    TAILQ_HEAD(tailhead_pipes_queued, pipe_obj) pipes_active_tailq;
    int num_pipes_idle;
    int num_pipes_queued;
    //Port status, state, and events
    hcd_port_state_t state;
    usb_speed_t speed;
    hcd_port_event_t last_event;
    TaskHandle_t task_waiting_port_notif;           //Task handle used for internal port events
    union {
        struct {
            uint32_t event_pending: 1;              //The port has an event that needs to be handled
            uint32_t event_processing: 1;           //The port is current processing (handling) an event
            uint32_t cmd_processing: 1;             //Used to indicate command handling is ongoing
            uint32_t waiting_all_pipes_pause: 1;    //Waiting for all pipes routed through this port to be paused
            uint32_t disable_requested: 1;
            uint32_t conn_devc_ena: 1;              //Used to indicate the port is connected to a device that has been reset
            uint32_t periodic_scheduling_enabled: 1;
            uint32_t reserved9: 9;
            uint32_t num_pipes_waiting_pause: 16;
        };
        uint32_t val;
    } flags;
    bool initialized;
    hcd_port_fifo_bias_t fifo_bias;
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

// ------------------- Buffer Control ----------------------

/**
 * @brief Check if an inactive buffer can be filled with a pending IRP
 *
 * @param pipe Pipe object
 * @return true There are one or more pending IRPs, and the inactive buffer is yet to be filled
 * @return false Otherwise
 */
static inline bool _buffer_can_fill(pipe_t *pipe)
{
    //We can only fill if there are pending IRPs and at least one unfilled buffer
    if (pipe->num_irp_pending > 0 && pipe->multi_buffer_control.buffer_num_to_fill > 0) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Fill an empty buffer with
 *
 * This function will:
 * - Remove an IRP from the pending tailq
 * - Fill that IRP into the inactive buffer
 *
 * @note _buffer_can_fill() must return true before calling this function
 *
 * @param pipe Pipe object
 */
static void _buffer_fill(pipe_t *pipe);

/**
 * @brief Check if there are more filled buffers than can be executed
 *
 * @param pipe Pipe object
 * @return true There are more filled buffers to be executed
 * @return false No more buffers to execute
 */
static inline bool _buffer_can_exec(pipe_t *pipe)
{
    //We can only execute if there is not already a buffer executing and if there are filled buffers awaiting execution
    if (!pipe->multi_buffer_control.buffer_is_executing && pipe->multi_buffer_control.buffer_num_to_exec > 0) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Execute the next filled buffer
 *
 * - Must have called _buffer_can_exec() before calling this function
 * - Will start the execution of the buffer
 *
 * @param pipe Pipe object
 */
static void _buffer_exec(pipe_t *pipe);

/**
 * @brief Check if a buffer as completed execution
 *
 * This should only be called after receiving a USBH_HAL_CHAN_EVENT_CPLT event to check if a buffer is actually
 * done. Buffers that aren't complete (such as Control transfers) will be continued automatically.
 *
 * @param pipe Pipe object
 * @return true Buffer complete
 * @return false Buffer not complete
 */
static bool _buffer_check_done(pipe_t *pipe);

/**
 * @brief Marks the last executed buffer as complete
 *
 * This should be called on a pipe that has confirmed that a buffer is completed via _buffer_check_done()
 *
 * @param pipe Pipe object
 * @param stop_idx Descriptor index when the buffer stopped execution
 */
static inline void _buffer_done(pipe_t *pipe, int stop_idx)
{
    //Store the stop_idx for later parsing
    dma_buffer_block_t *buffer_done = pipe->buffers[pipe->multi_buffer_control.rd_idx];
    buffer_done->status_flags.executing = 0;
    buffer_done->status_flags.error_occurred = 0;
    buffer_done->status_flags.stop_idx = stop_idx;
    pipe->multi_buffer_control.rd_idx++;
    pipe->multi_buffer_control.buffer_num_to_exec--;
    pipe->multi_buffer_control.buffer_num_to_parse++;
    pipe->multi_buffer_control.buffer_is_executing = 0;
}

/**
 * @brief Marks the last executed buffer as complete due to an error
 *
 * This should be called on a pipe that has received a USBH_HAL_CHAN_EVENT_ERROR event
 *
 * @param pipe Pipe object
 * @param stop_idx Descriptor index when the buffer stopped execution
 * @param pipe_state State of the pipe after the error
 * @param pipe_event Error event
 * @param cancelled Whether the pipe stopped due to cancellation
 */
static inline void _buffer_done_error(pipe_t *pipe, int stop_idx, hcd_pipe_state_t pipe_state, hcd_pipe_event_t pipe_event, bool cancelled)
{
    //Mark the buffer as erroneous for later parsing
    dma_buffer_block_t *buffer_done = pipe->buffers[pipe->multi_buffer_control.rd_idx];
    buffer_done->status_flags.executing = 0;
    buffer_done->status_flags.error_occurred = 1;
    buffer_done->status_flags.cancelled = cancelled;
    buffer_done->status_flags.stop_idx = stop_idx;
    buffer_done->status_flags.pipe_state = pipe_state;
    buffer_done->status_flags.pipe_event = pipe_event;
    pipe->multi_buffer_control.rd_idx++;
    pipe->multi_buffer_control.buffer_num_to_exec--;
    pipe->multi_buffer_control.buffer_num_to_parse++;
    pipe->multi_buffer_control.buffer_is_executing = 0;
}

/**
 * @brief Checks if a pipe has one or more completed buffers to parse
 *
 * @param pipe Pipe object
 * @return true There are one or more buffers to parse
 * @return false There are no more buffers to parse
 */
static inline bool _buffer_can_parse(pipe_t *pipe)
{
    if (pipe->multi_buffer_control.buffer_num_to_parse > 0) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Parse a completed buffer
 *
 * This function will:
 * - Parse the results of an IRP from a completed buffer
 * - Put the IRP into the done tailq
 *
 * @note This function should only be called on the completion of a buffer
 *
 * @param pipe Pipe object
 * @param stop_idx (For INTR pipes only) The index of the descriptor that follows the last descriptor of the IRP. Set to 0 otherwise
 */
static void _buffer_parse(pipe_t *pipe);

/**
 * @brief Marks all buffers pending execution as completed, then parses those buffers
 *
 * @note This should only be called on pipes do not have any currently executing buffers.
 *
 * @param pipe Pipe object
 * @param cancelled Whether this flush is due to cancellation
 */
static void _buffer_flush_all(pipe_t *pipe, bool cancelled);

// ------------------------ Pipe ---------------------------

/**
 * @brief Wait until a pipe's in-flight IRP is done
 *
 * If the pipe has an in-flight IRP, this function will block until it is done (via a internal pipe event).
 * If the pipe has no in-flight IRP, this function do nothing and return immediately.
 * If the pipe's state changes unexpectedly, this function will return false.
 *
 * Also parses all buffers on exit
 *
 * @note This function is blocking (will exit and re-enter the critical section to do so)
 *
 * @param pipe Pipe object
 * @return true Pipes in-flight IRP is done
 * @return false Pipes state unexpectedly changed
 */
static bool _pipe_wait_done(pipe_t *pipe);

/**
 * @brief Retires all IRPs (those that were previously in-flight or pending)
 *
 * Retiring all IRPs will result in any pending IRP being moved to the done tailq. This function will update the IPR
 * status of each IRP.
 *  - If the retiring is self-initiated (i.e., due to a pipe command), the IRP status will be set to USB_TRANSFER_STATUS_CANCELED.
 *  - If the retiring is NOT self-initiated (i.e., the pipe is no longer valid), the IRP status will be set to USB_TRANSFER_STATUS_NO_DEVICE
 *
 * Entry:
 * - There can be no in-flight IRP (must already be parsed and returned to done queue)
 * - All buffers must be parsed
 * Exit:
 * - If there was an in-flight IRP, it is parsed and returned to the done queue
 * - If there are any pending IRPs:
 *      - They are moved to the done tailq
 *
 * @param pipe Pipe object
 * @param cancelled Are we actively Pipe retire is initialized by the user due to a command, thus IRP are
 *                  actively cancelled.
 */
static void _pipe_retire(pipe_t *pipe, bool self_initiated);

/**
 * @brief Decode a HAL channel error to the corresponding pipe event
 *
 * @param chan_error The HAL channel error
 * @return hcd_pipe_event_t The corresponding pipe error event
 */
static inline hcd_pipe_event_t pipe_decode_error_event(usbh_hal_chan_error_t chan_error);

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
 *  - Each pipe will have any pending IRPs moved to their respective done tailq
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
 *  - All pipes routed through the port have either paused, or are waiting to complete their in-flight IRPs before pausing
 *  - If waiting for one or more pipes to pause, _internal_port_event_wait() must be called after this function returns
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
 *  - All pipes un-paused. If those pipes have pending IRPs, they will be started.
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
                //The port was previously enabled, so this is a sudden disconnection
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
            port->speed = (usbh_hal_port_get_conn_speed(port->hal) == USB_PRIV_SPEED_FULL) ? USB_SPEED_FULL : USB_SPEED_LOW;
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
    bool handle_waiting_xfer_done = false;
    switch (chan_event) {
        case USBH_HAL_CHAN_EVENT_CPLT: {
            if (!_buffer_check_done(pipe)) {
                break;
            }
            pipe->last_event = HCD_PIPE_EVENT_IRP_DONE;
            event = pipe->last_event;
            //Mark the buffer as done
            int stop_idx = usbh_hal_chan_get_qtd_idx(chan_obj);
            _buffer_done(pipe, stop_idx);
            //First check if there is another buffer we can execute
            if (_buffer_can_exec(pipe) && !pipe->cs_flags.waiting_xfer_done) {
                //If the next buffer is filled and ready to execute, execute it
                _buffer_exec(pipe);
            }
            //Handle the previously done buffer
            _buffer_parse(pipe);
            if (pipe->cs_flags.waiting_xfer_done) {
                handle_waiting_xfer_done = true;
            } else if (_buffer_can_fill(pipe)) {
                //Now that we've parsed a buffer, see if another IRP can be filled in its place
                _buffer_fill(pipe);
            }
            break;
        }
        case USBH_HAL_CHAN_EVENT_ERROR: {
            //Get and store the pipe error event
            usbh_hal_chan_error_t chan_error = usbh_hal_chan_get_error(chan_obj);
            usbh_hal_chan_clear_error(chan_obj);
            pipe->last_event = pipe_decode_error_event(chan_error);
            event = pipe->last_event;
            pipe->state = HCD_PIPE_STATE_HALTED;
            //Mark the buffer as done with an error
            int stop_idx = usbh_hal_chan_get_qtd_idx(chan_obj);
            _buffer_done_error(pipe, stop_idx, pipe->state, pipe->last_event, false);
            //Parse the buffer
            _buffer_parse(pipe);
            if (pipe->cs_flags.waiting_xfer_done) {
                handle_waiting_xfer_done = true;
            }
            break;
        }
        case USBH_HAL_CHAN_EVENT_NONE: {
            break;  //Nothing to do
        }
        case USBH_HAL_CHAN_EVENT_HALT_REQ:  //We currently don't halt request so this event should never occur
        default:
            abort();
            break;
    }
    if (handle_waiting_xfer_done) {
        //A port/pipe command is waiting for this pipe to complete its transfer. So don't load the next transfer
        pipe->cs_flags.waiting_xfer_done = 0;
        if (pipe->port->flags.waiting_all_pipes_pause) {
            //Port command is waiting for all pipes to be paused
            pipe->cs_flags.paused = 1;
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
 *
 * @param arg Interrupt handler argument
 */
static void intr_hdlr_main(void *arg)
{
    port_t *port = (port_t *) arg;
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
    void *frame_list = heap_caps_aligned_calloc(USBH_HAL_FRAME_LIST_MEM_ALIGN, FRAME_LIST_LEN,sizeof(uint32_t), MALLOC_CAP_DMA);
    SemaphoreHandle_t port_mux = xSemaphoreCreateMutex();
    if (port == NULL || hal == NULL || frame_list == NULL || port_mux == NULL) {
        free(port);
        free(hal);
        free(frame_list);
        if (port_mux != NULL) {
            vSemaphoreDelete(port_mux);
        }
        return NULL;
    }
    port->hal = hal;
    port->frame_list = frame_list;
    port->port_mux = port_mux;
    return port;
}

static void port_obj_free(port_t *port)
{
    if (port == NULL) {
        return;
    }
    vSemaphoreDelete(port->port_mux);
    free(port->frame_list);
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
                                              config->intr_flags | ESP_INTR_FLAG_INTRDISABLED,  //The interrupt must be disabled until the port is initialized
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
    //Set HW prerequisites for each port (there's only one)
    periph_module_enable(PERIPH_USB_MODULE);
    periph_module_reset(PERIPH_USB_MODULE);
    /*
    Configure GPIOS for Host mode operation using internal PHY
        - Forces ID to GND for A side
        - Forces B Valid to GND as we are A side host
        - Forces VBUS Valid to HIGH
        - Forces A Valid to HIGH
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
    //Check that all ports have been disabled (there's only one port)
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
    //Process all pipes that have queued IRPs
    TAILQ_FOREACH(pipe, &port->pipes_active_tailq, tailq_entry) {
        //Mark the pipe as invalid and set an invalid event
        pipe->state = HCD_PIPE_STATE_INVALID;
        pipe->last_event = HCD_PIPE_EVENT_INVALID;
        //Flush all buffers that are still awaiting exec
        _buffer_flush_all(pipe, false);
        //Retire any remaining IRPs in the pending tailq
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
    //Process all pipes that have queued IRPs
    TAILQ_FOREACH(pipe, &port->pipes_active_tailq, tailq_entry) {
        //Check if pipe is currently executing
        if (pipe->multi_buffer_control.buffer_is_executing) {
            //Pipe is executing a buffer. Indicate to the pipe we are waiting the buffer's transfer to complete
            pipe->cs_flags.waiting_xfer_done = 1;
            num_pipes_waiting_done++;
        } else {
            //No buffer is being executed so need to wait
            pipe->cs_flags.paused = 1;
        }
    }
    //Process all idle pipes. They don't have queue transfer so just mark them as paused
    TAILQ_FOREACH(pipe, &port->pipes_idle_tailq, tailq_entry) {
        pipe->cs_flags.paused = 1;
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
        pipe->cs_flags.paused = 0;
    }
    //Process all pipes that have queued IRPs
    TAILQ_FOREACH(pipe, &port->pipes_active_tailq, tailq_entry) {
        pipe->cs_flags.paused = 0;
        if (_buffer_can_fill(pipe)) {
            _buffer_fill(pipe);
        }
        if (_buffer_can_exec(pipe)) {
            _buffer_exec(pipe);
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
            //Port state unexpectedly changed
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
        //Port state unexpectedly changed
        goto bailout;
    }
    HCD_EXIT_CRITICAL();
    vTaskDelay(pdMS_TO_TICKS(RESUME_RECOVERY_MS));
    HCD_ENTER_CRITICAL();
    if (port->state != HCD_PORT_STATE_RESUMING || !port->flags.conn_devc_ena) {
        //Port state unexpectedly changed
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
                //Port state unexpectedly changed
                goto bailout;
            }
        }
    }
    //All pipes are guaranteed paused at this point. Proceed to suspend the port. This should trigger an internal event
    port->flags.disable_requested = 1;
    usbh_hal_port_disable(port->hal);
    _internal_port_event_wait(port);
    if (port->state != HCD_PORT_STATE_DISABLED) {
        //Port state unexpectedly changed
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
    //Port object memory and resources (such as the mutex) already be allocated. Just need to initialize necessary fields only
    port_t *port_obj = s_hcd_obj->port_obj;
    TAILQ_INIT(&port_obj->pipes_idle_tailq);
    TAILQ_INIT(&port_obj->pipes_active_tailq);
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
                    usbh_hal_port_init(port->hal);
                    usbh_hal_port_toggle_power(port->hal, true);
                    ret = ESP_OK;
                }
                break;
            }
            case HCD_PORT_CMD_POWER_OFF: {
                //Port can only be unpowered if already powered
                if (port->state != HCD_PORT_STATE_NOT_POWERED) {
                    port->state = HCD_PORT_STATE_NOT_POWERED;
                    usbh_hal_port_deinit(port->hal);
                    usbh_hal_port_toggle_power(port->hal, false);
                    //If a device is currently connected, this should trigger a disconnect event
                    ret = ESP_OK;
                }
                break;
            }
            case HCD_PORT_CMD_RESET: {
                //Port can only a reset when it is in the enabled or disabled states (in case of new connection)
                if (port->state == HCD_PORT_STATE_ENABLED || port->state == HCD_PORT_STATE_DISABLED) {
                    if (_port_bus_reset(port)) {
                        //Set FIFO sizes to default
                        usbh_hal_set_fifo_size(port->hal, &fifo_config_default);
                        port->fifo_bias = HCD_PORT_FIFO_BIAS_BALANCED;
                        //Reset frame list and enable periodic scheduling
                        memset(port->frame_list, 0, FRAME_LIST_LEN * sizeof(uint32_t));
                        usbh_hal_port_set_frame_list(port->hal, port->frame_list, FRAME_LIST_LEN);
                        usbh_hal_port_periodic_enable(port->hal);
                        ret = ESP_OK;
                    } else {
                        ret = ESP_ERR_INVALID_RESPONSE;
                    }
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
    //Device speed is only valid if there is device connected to the port that has been reset
    HCD_CHECK_FROM_CRIT(port->flags.conn_devc_ena, ESP_ERR_INVALID_STATE);
    usb_priv_speed_t hal_speed = usbh_hal_port_get_conn_speed(port->hal);
    if (hal_speed == USB_PRIV_SPEED_FULL) {
        *speed = USB_SPEED_FULL;
    } else {
        *speed = USB_SPEED_LOW;
    }
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
                    //No device connected after debounce delay. This is an actual disconnection
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

void *hcd_port_get_context(hcd_port_handle_t port_hdl)
{
    port_t *port = (port_t *)port_hdl;
    void *ret;
    HCD_ENTER_CRITICAL();
    ret = port->context;
    HCD_EXIT_CRITICAL();
    return ret;
}

esp_err_t hcd_port_set_fifo_bias(hcd_port_handle_t port_hdl, hcd_port_fifo_bias_t bias)
{
    esp_err_t ret;
    port_t *port = (port_t *)port_hdl;
    xSemaphoreTake(port->port_mux, portMAX_DELAY);
    HCD_ENTER_CRITICAL();
    //Check that port is in the correct state to update FIFO sizes
    if (port->initialized && !port->flags.event_pending && port->num_pipes_idle == 0 && port->num_pipes_queued == 0) {
        const usbh_hal_fifo_config_t *fifo_config;
        switch (bias) {
            case HCD_PORT_FIFO_BIAS_BALANCED:
                fifo_config = &fifo_config_default;
                break;
            case HCD_PORT_FIFO_BIAS_RX:
                fifo_config = &fifo_config_bias_rx;
                break;
            case HCD_PORT_FIFO_BIAS_PTX:
                fifo_config = &fifo_config_bias_ptx;
                break;
            default:
                fifo_config = NULL;
                abort();
        }
        usbh_hal_set_fifo_size(port->hal, fifo_config);
        port->fifo_bias = bias;
        ret = ESP_OK;
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    HCD_EXIT_CRITICAL();
    xSemaphoreGive(port->port_mux);
    return ret;
}

// --------------------------------------------------- HCD Pipes -------------------------------------------------------

// ----------------------- Private -------------------------

static bool _pipe_wait_done(pipe_t *pipe)
{
    //Check if the pipe has a currently executing buffer
    if (pipe->multi_buffer_control.buffer_is_executing) {
        //Wait for pipe to complete its transfer
        pipe->cs_flags.waiting_xfer_done = 1;
        _internal_pipe_event_wait(pipe);
        if (pipe->state == HCD_PIPE_STATE_INVALID) {
            //The pipe become invalid whilst waiting for its internal event
            pipe->cs_flags.waiting_xfer_done = 0;  //Need to manually reset this bit in this case
            return false;
        }
        bool chan_halted = usbh_hal_chan_request_halt(pipe->chan_obj);
        assert(chan_halted);
        (void) chan_halted;
    }
    return true;
}

static void _pipe_retire(pipe_t *pipe, bool self_initiated)
{
    //Cannot have a currently executing buffer
    assert(!pipe->multi_buffer_control.buffer_is_executing);
    if (pipe->num_irp_pending > 0) {
        //Process all remaining pending IRPs
        usb_irp_t *irp;
        TAILQ_FOREACH(irp, &pipe->pending_irp_tailq, tailq_entry) {
            //Update the IRP's current state
            IRP_STATE_SET(irp->reserved_flags, IRP_STATE_DONE);
            //If we are initiating the retire, mark the IRP as canceled
            irp->status = (self_initiated) ? USB_TRANSFER_STATUS_CANCELED : USB_TRANSFER_STATUS_NO_DEVICE;
        }
        //Concatenated pending tailq to the done tailq
        TAILQ_CONCAT(&pipe->done_irp_tailq, &pipe->pending_irp_tailq, tailq_entry);
        pipe->num_irp_done += pipe->num_irp_pending;
        pipe->num_irp_pending = 0;
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
            event = HCD_PIPE_EVENT_ERROR_IRP_NOT_AVAIL;
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

static dma_buffer_block_t *buffer_block_alloc(usb_transfer_type_t type)
{
    int desc_list_len;
    switch (type) {
    case USB_TRANSFER_TYPE_CTRL:
        desc_list_len = XFER_LIST_LEN_CTRL;
        break;
    case USB_TRANSFER_TYPE_ISOCHRONOUS:
        desc_list_len = XFER_LIST_LEN_ISOC;
        break;
    case USB_TRANSFER_TYPE_BULK:
        desc_list_len = XFER_LIST_LEN_BULK;
        break;
    default:    //USB_TRANSFER_TYPE_INTR:
        desc_list_len = XFER_LIST_LEN_INTR;
        break;
    }
    dma_buffer_block_t *buffer = calloc(1, sizeof(dma_buffer_block_t));
    void *xfer_desc_list = heap_caps_aligned_calloc(USBH_HAL_DMA_MEM_ALIGN, desc_list_len, sizeof(usbh_ll_dma_qtd_t), MALLOC_CAP_DMA);
    if (buffer == NULL || xfer_desc_list == NULL) {
        free(buffer);
        heap_caps_free(xfer_desc_list);
        return NULL;
    }
    buffer->xfer_desc_list = xfer_desc_list;
    return buffer;
}

static void buffer_block_free(dma_buffer_block_t *buffer)
{
    if (buffer == NULL) {
        return;
    }
    heap_caps_free(buffer->xfer_desc_list);
    free(buffer);
}

static bool pipe_alloc_check_args(const hcd_pipe_config_t *pipe_config, usb_speed_t port_speed, hcd_port_fifo_bias_t fifo_bias, usb_transfer_type_t type, bool is_default_pipe)
{
    //Check if pipe can be supported
    if (port_speed == USB_SPEED_LOW && pipe_config->dev_speed == USB_SPEED_FULL) {
        //Low speed port does not supported full speed pipe
        return false;
    }
    if (pipe_config->dev_speed == USB_SPEED_LOW && (type == USB_TRANSFER_TYPE_BULK || type == USB_TRANSFER_TYPE_ISOCHRONOUS)) {
        //Low speed does not support Bulk or Isochronous pipes
        return false;
    }
    //Check interval of pipe
    if (type == USB_TRANSFER_TYPE_INTR &&
        (pipe_config->ep_desc->bInterval > 0 && pipe_config->ep_desc->bInterval > 32)) {
        //Interval not supported for interrupt pipe
        return false;
    }
    if (type == USB_TRANSFER_TYPE_ISOCHRONOUS &&
        (pipe_config->ep_desc->bInterval > 0 && pipe_config->ep_desc->bInterval > 6)) {
        //Interval not supported for isochronous pipe (where 0 < 2^(bInterval - 1) <= 32)
        return false;
    }

    if (is_default_pipe) {
        return true;
    }
    //Check if MPS is within FIFO limits
    const fifo_mps_limits_t *mps_limits;
    switch (fifo_bias) {
        case HCD_PORT_FIFO_BIAS_BALANCED:
            mps_limits = &mps_limits_default;
            break;
        case HCD_PORT_FIFO_BIAS_RX:
            mps_limits = &mps_limits_bias_rx;
            break;
        default:    //HCD_PORT_FIFO_BIAS_PTX
            mps_limits = &mps_limits_bias_ptx;
            break;
    }
    int limit;
    if (USB_DESC_EP_GET_EP_DIR(pipe_config->ep_desc)) { //IN
        limit = mps_limits->in_mps;
    } else {    //OUT
        if (type == USB_TRANSFER_TYPE_CTRL || type == USB_TRANSFER_TYPE_BULK) {
            limit = mps_limits->non_periodic_out_mps;
        } else {
            limit = mps_limits->periodic_out_mps;
        }
    }
    return (pipe_config->ep_desc->wMaxPacketSize <= limit);
}

static void pipe_set_ep_char(const hcd_pipe_config_t *pipe_config, usb_transfer_type_t type, bool is_default_pipe, int pipe_idx, usb_speed_t port_speed, usbh_hal_ep_char_t *ep_char)
{
    //Initialize EP characteristics
    usb_priv_xfer_type_t hal_xfer_type;
    switch (type) {
        case USB_TRANSFER_TYPE_CTRL:
            hal_xfer_type = USB_PRIV_XFER_TYPE_CTRL;
            break;
        case USB_TRANSFER_TYPE_ISOCHRONOUS:
            hal_xfer_type = USB_PRIV_XFER_TYPE_ISOCHRONOUS;
            break;
        case USB_TRANSFER_TYPE_BULK:
            hal_xfer_type = USB_PRIV_XFER_TYPE_BULK;
            break;
        default:    //USB_TRANSFER_TYPE_INTR
            hal_xfer_type = USB_PRIV_XFER_TYPE_INTR;
            break;
    }
    ep_char->type = hal_xfer_type;
    if (is_default_pipe) {
        ep_char->bEndpointAddress = 0;
        //Set the default pipe's MPS to the worst case MPS for the device's speed
        ep_char->mps = (pipe_config->dev_speed == USB_SPEED_FULL) ? CTRL_EP_MAX_MPS_FS : CTRL_EP_MAX_MPS_LS;
    } else {
        ep_char->bEndpointAddress = pipe_config->ep_desc->bEndpointAddress;
        ep_char->mps = pipe_config->ep_desc->wMaxPacketSize;
    }
    ep_char->dev_addr = pipe_config->dev_addr;
    ep_char->ls_via_fs_hub = (port_speed == USB_SPEED_FULL && pipe_config->dev_speed == USB_SPEED_LOW);
    //Calculate the pipe's interval in terms of USB frames
    if (type == USB_TRANSFER_TYPE_INTR || type == USB_TRANSFER_TYPE_ISOCHRONOUS) {
        int interval_frames;
        if (type == USB_TRANSFER_TYPE_INTR) {
            interval_frames = pipe_config->ep_desc->bInterval;
        } else {
            interval_frames = (1 << (pipe_config->ep_desc->bInterval - 1));
        }
        //Round down interval to nearest power of 2
        if (interval_frames >= 32) {
            interval_frames = 32;
        } else if (interval_frames >= 16) {
            interval_frames = 16;
        } else if (interval_frames >= 8) {
            interval_frames = 8;
        } else if (interval_frames >= 4) {
            interval_frames = 4;
        } else if (interval_frames >= 2) {
            interval_frames = 2;
        } else if (interval_frames >= 1) {
            interval_frames = 1;
        }
        ep_char->periodic.interval = interval_frames;
        //We are the Nth pipe to be allocated. Use N as a phase offset
        ep_char->periodic.phase_offset_frames = pipe_idx & (XFER_LIST_LEN_ISOC - 1);
    }else {
        ep_char->periodic.interval = 0;
        ep_char->periodic.phase_offset_frames = 0;
    }
}

// ----------------------- Public --------------------------

esp_err_t hcd_pipe_alloc(hcd_port_handle_t port_hdl, const hcd_pipe_config_t *pipe_config, hcd_pipe_handle_t *pipe_hdl)
{
    HCD_CHECK(port_hdl != NULL && pipe_config != NULL && pipe_hdl != NULL, ESP_ERR_INVALID_ARG);
    port_t *port = (port_t *)port_hdl;
    HCD_ENTER_CRITICAL();
    //Can only allocate a pipe if the target port is initialized and connected to an enabled device
    HCD_CHECK_FROM_CRIT(port->initialized && port->flags.conn_devc_ena, ESP_ERR_INVALID_STATE);
    usb_speed_t port_speed = port->speed;
    hcd_port_fifo_bias_t port_fifo_bias = port->fifo_bias;
    int pipe_idx = port->num_pipes_idle + port->num_pipes_queued;
    HCD_EXIT_CRITICAL();

    usb_transfer_type_t type;
    bool is_default;
    if (pipe_config->ep_desc == NULL) {
        type = USB_TRANSFER_TYPE_CTRL;
        is_default = true;
    } else {
        type = USB_DESC_EP_GET_XFERTYPE(pipe_config->ep_desc);
        is_default = false;
    }
    //Check if pipe configuration can be supported
    if (!pipe_alloc_check_args(pipe_config, port_speed, port_fifo_bias, type, is_default)) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_err_t ret;
    //Allocate the pipe resources
    pipe_t *pipe = calloc(1, sizeof(pipe_t));
    usbh_hal_chan_t *chan_obj = calloc(1, sizeof(usbh_hal_chan_t));
    dma_buffer_block_t *buffers[NUM_BUFFERS] = {0};
    if (pipe == NULL|| chan_obj == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    for (int i = 0; i < NUM_BUFFERS; i++) {
        buffers[i] = buffer_block_alloc(type);
        if (buffers[i] == NULL) {
            ret = ESP_ERR_NO_MEM;
            goto err;
        }
    }

    //Initialize pipe object
    TAILQ_INIT(&pipe->pending_irp_tailq);
    TAILQ_INIT(&pipe->done_irp_tailq);
    for (int i = 0; i < NUM_BUFFERS; i++) {
        pipe->buffers[i] = buffers[i];
    }
    pipe->multi_buffer_control.buffer_num_to_fill = NUM_BUFFERS;
    pipe->port = port;
    pipe->chan_obj = chan_obj;
    usbh_hal_ep_char_t ep_char;
    pipe_set_ep_char(pipe_config, type, is_default, pipe_idx, port_speed, &ep_char);
    memcpy(&pipe->ep_char, &ep_char, sizeof(usbh_hal_ep_char_t));
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
    usbh_hal_chan_set_ep_char(port->hal, pipe->chan_obj, &pipe->ep_char);
    //Add the pipe to the list of idle pipes in the port object
    TAILQ_INSERT_TAIL(&port->pipes_idle_tailq, pipe, tailq_entry);
    port->num_pipes_idle++;
    HCD_EXIT_CRITICAL();

    *pipe_hdl = (hcd_pipe_handle_t)pipe;
    return ESP_OK;

err:
    for (int i = 0; i < NUM_BUFFERS; i++) {
        buffer_block_free(buffers[i]);
    }
    free(chan_obj);
    free(pipe);
    return ret;
}

esp_err_t hcd_pipe_free(hcd_pipe_handle_t pipe_hdl)
{
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    HCD_ENTER_CRITICAL();
    //Check that all IRPs have been removed and pipe has no pending events
    HCD_CHECK_FROM_CRIT(!pipe->multi_buffer_control.buffer_is_executing
                        && pipe->multi_buffer_control.buffer_num_to_parse == 0
                        && pipe->multi_buffer_control.buffer_num_to_exec == 0
                        && pipe->num_irp_pending == 0
                        && pipe->num_irp_done == 0,
                        ESP_ERR_INVALID_STATE);
    //Remove pipe from the list of idle pipes (it must be in the idle list because it should have no queued IRPs)
    TAILQ_REMOVE(&pipe->port->pipes_idle_tailq, pipe, tailq_entry);
    pipe->port->num_pipes_idle--;
    usbh_hal_chan_free(pipe->port->hal, pipe->chan_obj);
    HCD_EXIT_CRITICAL();

    //Free pipe resources
    for (int i = 0; i < NUM_BUFFERS; i++) {
        buffer_block_free(pipe->buffers[i]);
    }
    free(pipe->chan_obj);
    free(pipe);
    return ESP_OK;
}

esp_err_t hcd_pipe_update_mps(hcd_pipe_handle_t pipe_hdl, int mps)
{
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    HCD_ENTER_CRITICAL();
    //Check if pipe is in the correct state to be updated
    HCD_CHECK_FROM_CRIT(pipe->state != HCD_PIPE_STATE_INVALID
                        && !pipe->cs_flags.pipe_cmd_processing
                        && pipe->num_irp_pending == 0
                        && pipe->num_irp_done == 0,
                        ESP_ERR_INVALID_STATE);
    pipe->ep_char.mps = mps;
    //Update the underlying channel's registers
    usbh_hal_chan_set_ep_char(pipe->port->hal, pipe->chan_obj, &pipe->ep_char);
    HCD_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t hcd_pipe_update_dev_addr(hcd_pipe_handle_t pipe_hdl, uint8_t dev_addr)
{
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    HCD_ENTER_CRITICAL();
    //Check if pipe is in the correct state to be updated
    HCD_CHECK_FROM_CRIT(pipe->state != HCD_PIPE_STATE_INVALID
                        && !pipe->cs_flags.pipe_cmd_processing
                        && pipe->num_irp_pending == 0
                        && pipe->num_irp_done == 0,
                        ESP_ERR_INVALID_STATE);
    pipe->ep_char.dev_addr = dev_addr;
    //Update the underlying channel's registers
    usbh_hal_chan_set_ep_char(pipe->port->hal, pipe->chan_obj, &pipe->ep_char);
    HCD_EXIT_CRITICAL();
    return ESP_OK;
}

void *hcd_pipe_get_context(hcd_pipe_handle_t pipe_hdl)
{
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    void *ret;
    HCD_ENTER_CRITICAL();
    ret = pipe->context;
    HCD_EXIT_CRITICAL();
    return ret;
}

hcd_pipe_state_t hcd_pipe_get_state(hcd_pipe_handle_t pipe_hdl)
{
    hcd_pipe_state_t ret;
    pipe_t *pipe = (pipe_t *)pipe_hdl;
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
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    bool ret = ESP_OK;

    HCD_ENTER_CRITICAL();
    //Cannot execute pipe commands the pipe is already executing a command, or if the pipe or its port are no longer valid
    if (pipe->cs_flags.pipe_cmd_processing || !pipe->port->flags.conn_devc_ena || pipe->state == HCD_PIPE_STATE_INVALID) {
        ret = ESP_ERR_INVALID_STATE;
    } else {
        pipe->cs_flags.pipe_cmd_processing = 1;
        switch (command) {
            case HCD_PIPE_CMD_ABORT: {
                //Retire all scheduled IRPs. Pipe's state remains unchanged
                if (!_pipe_wait_done(pipe)) {   //Stop any on going transfers
                    ret = ESP_ERR_INVALID_RESPONSE;
                }
                _buffer_flush_all(pipe, true);  //Some buffers might still be filled. Flush them
                _pipe_retire(pipe, true);  //Retire any pending transfers
                break;
            }
            case HCD_PIPE_CMD_RESET: {
                //Retire all scheduled IRPs. Pipe's state moves to active
                if (!_pipe_wait_done(pipe)) {   //Stop any on going transfers
                    ret = ESP_ERR_INVALID_RESPONSE;
                    break;
                }
                _buffer_flush_all(pipe, true);  //Some buffers might still be filled. Flush them
                _pipe_retire(pipe, true);  //Retire any pending transfers
                pipe->state = HCD_PIPE_STATE_ACTIVE;
                break;
            }
            case HCD_PIPE_CMD_CLEAR: {  //Can only do this if port is still active
                //Pipe's state moves from halted to active
                if (pipe->state == HCD_PIPE_STATE_HALTED) {
                    pipe->state = HCD_PIPE_STATE_ACTIVE;
                    //Start the next pending transfer if it exists
                    if (_buffer_can_fill(pipe)) {
                        _buffer_fill(pipe);
                    }
                    if (_buffer_can_exec(pipe)) {
                        _buffer_exec(pipe);
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
        pipe->cs_flags.pipe_cmd_processing = 0;
    }
    HCD_EXIT_CRITICAL();
    return ret;
}

hcd_pipe_event_t hcd_pipe_get_event(hcd_pipe_handle_t pipe_hdl)
{
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    hcd_pipe_event_t ret;
    HCD_ENTER_CRITICAL();
    ret = pipe->last_event;
    pipe->last_event = HCD_PIPE_EVENT_NONE;
    HCD_EXIT_CRITICAL();
    return ret;
}

// ------------------------------------------------- Buffer Control ----------------------------------------------------

static inline void _buffer_fill_ctrl(dma_buffer_block_t *buffer, usb_irp_t *irp)
{
    //Get information about the control transfer by analyzing the setup packet (the first 8 bytes of the IRP's data)
    usb_ctrl_req_t *ctrl_req = (usb_ctrl_req_t *)irp->data_buffer;
    bool data_stg_in = (ctrl_req->bRequestType & USB_B_REQUEST_TYPE_DIR_IN);
    bool data_stg_skip = (irp->num_bytes == 0);
    //Fill setup stage
    usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, 0, irp->data_buffer, sizeof(usb_ctrl_req_t),
                            USBH_HAL_XFER_DESC_FLAG_SETUP | USBH_HAL_XFER_DESC_FLAG_HOC);
    //Fill data stage
    if (data_stg_skip) {
        //Not data stage. Fill with an empty descriptor
        usbh_hal_xfer_desc_clear(buffer->xfer_desc_list, 1);
    } else {
        //Fill data stage
        usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, 1, irp->data_buffer + sizeof(usb_ctrl_req_t), irp->num_bytes,
                                ((data_stg_in) ? USBH_HAL_XFER_DESC_FLAG_IN : 0) | USBH_HAL_XFER_DESC_FLAG_HOC);
    }
    //Fill status stage (i.e., a zero length packet). If data stage is skipped, the status stage is always IN.
    usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, 2, NULL, 0,
                            ((data_stg_in && !data_stg_skip) ? 0 : USBH_HAL_XFER_DESC_FLAG_IN) | USBH_HAL_XFER_DESC_FLAG_HOC);
    //Update buffer flags
    buffer->flags.ctrl.data_stg_in = data_stg_in;
    buffer->flags.ctrl.data_stg_skip = data_stg_skip;
    buffer->flags.ctrl.cur_stg = 0;
}

static inline void _buffer_fill_bulk(dma_buffer_block_t *buffer, usb_irp_t *irp, bool is_in)
{
    if (is_in) {
        usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, 0, irp->data_buffer, irp->num_bytes,
                                USBH_HAL_XFER_DESC_FLAG_IN | USBH_HAL_XFER_DESC_FLAG_HOC);
    } else if (irp->flags & USB_IRP_FLAG_ZERO_PACK) {
        //We need to add an extra zero length packet, so two descriptors are used
        usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, 0, irp->data_buffer, irp->num_bytes, 0);
        usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, 1, NULL, 0, USBH_HAL_XFER_DESC_FLAG_HOC);
    } else {
        usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, 0, irp->data_buffer, irp->num_bytes, USBH_HAL_XFER_DESC_FLAG_HOC);
    }
    //Update buffer flags
    buffer->flags.bulk.zero_len_packet = (is_in && (irp->flags & USB_IRP_FLAG_ZERO_PACK)) ? 1 : 0;
}

static inline void _buffer_fill_intr(dma_buffer_block_t *buffer, usb_irp_t *irp, bool is_in, int mps)
{
    int num_qtds;
    if (is_in) {
        assert(irp->num_bytes % mps == 0);  //IN transfers MUST be integer multiple of MPS
        num_qtds = irp->num_bytes / mps;
    } else {
        num_qtds = irp->num_bytes / mps;    //Floor division for number of MPS packets
        if (irp->num_bytes % irp->num_bytes > 0) {
            num_qtds++; //For the last shot packet
        }
    }
    assert(num_qtds <= XFER_LIST_LEN_INTR);
    //Fill all but last descriptor
    int bytes_filled = 0;
    for (int i = 0; i < num_qtds - 1; i++) {
        usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, i, &irp->data_buffer[bytes_filled], mps, (is_in) ? USBH_HAL_XFER_DESC_FLAG_IN : 0);
        bytes_filled += mps;
    }
    //Fill in the last descriptor with HOC flag
    usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, num_qtds - 1, &irp->data_buffer[bytes_filled], irp->num_bytes - bytes_filled,
                            ((is_in) ? USBH_HAL_XFER_DESC_FLAG_IN : 0) | USBH_HAL_XFER_DESC_FLAG_HOC);
    //Update buffer members and flags
    buffer->flags.intr.num_qtds = num_qtds;
}

static inline void _buffer_fill_isoc(dma_buffer_block_t *buffer, usb_irp_t *irp, bool is_in, int mps, int interval, int start_idx)
{
    assert(interval > 0);
    int total_num_desc = irp->num_iso_packets * interval;
    assert(total_num_desc <= XFER_LIST_LEN_ISOC);
    int desc_idx = start_idx;
    int bytes_filled = 0;
    //For each packet, fill in a descriptor and a interval-1 blank descriptor after it
    for (int pkt_idx = 0; pkt_idx < irp->num_iso_packets; pkt_idx++) {
        int xfer_len = irp->iso_packet_desc[pkt_idx].length;
        uint32_t flags = (is_in) ? USBH_HAL_XFER_DESC_FLAG_IN : 0;
        if (pkt_idx == irp->num_iso_packets - 1) {
            //Last packet, set the the HOC flag
            flags |= USBH_HAL_XFER_DESC_FLAG_HOC;
        }
        usbh_hal_xfer_desc_fill(buffer->xfer_desc_list, desc_idx, &irp->data_buffer[bytes_filled], xfer_len, flags);
        bytes_filled += xfer_len;
        if (++desc_idx >= XFER_LIST_LEN_ISOC) {
            desc_idx = 0;
        }
        //Clear descriptors for unscheduled frames
        for (int i = 0; i < interval - 1; i++) {
            usbh_hal_xfer_desc_clear(buffer->xfer_desc_list, desc_idx);
            if (++desc_idx >= XFER_LIST_LEN_ISOC) {
                desc_idx = 0;
            }
        }
    }
    //Update buffer members and flags
    buffer->flags.isoc.num_qtds = total_num_desc;
    buffer->flags.isoc.interval = interval;
    buffer->flags.isoc.irp_start_idx = start_idx;
    buffer->flags.isoc.next_irp_start_idx = desc_idx;
}

static void _buffer_fill(pipe_t *pipe)
{
    //Get an IRP from the pending tailq
    usb_irp_t *irp = TAILQ_FIRST(&pipe->pending_irp_tailq);
    assert(pipe->num_irp_pending > 0 && irp != NULL);
    TAILQ_REMOVE(&pipe->pending_irp_tailq, irp, tailq_entry);
    pipe->num_irp_pending--;

    //Select the inactive buffer
    assert(pipe->multi_buffer_control.buffer_num_to_exec <= NUM_BUFFERS);
    dma_buffer_block_t *buffer_to_fill = pipe->buffers[pipe->multi_buffer_control.wr_idx];
    assert(buffer_to_fill->irp == NULL);
    bool is_in = pipe->ep_char.bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK;
    int mps = pipe->ep_char.mps;
    switch (pipe->ep_char.type) {
        case USB_PRIV_XFER_TYPE_CTRL: {
            _buffer_fill_ctrl(buffer_to_fill, irp);
            break;
        }
        case USB_PRIV_XFER_TYPE_ISOCHRONOUS: {
            uint32_t start_idx;
            if (pipe->multi_buffer_control.buffer_num_to_exec == 0) {
                //There are no more previously filled buffers to execute. We need to calculate a new start index based on HFNUM and the pipe's schedule
                uint32_t cur_frame_num = usbh_hal_port_get_cur_frame_num(pipe->port->hal);
                uint32_t cur_mod_idx_no_offset = (cur_frame_num - pipe->ep_char.periodic.phase_offset_frames) & (XFER_LIST_LEN_ISOC - 1);    //Get the modulated index (i.e., the Nth desc in the descriptor list)
                //This is the non-offset modulated QTD index of the last scheduled interval
                uint32_t last_interval_mod_idx_no_offset = (cur_mod_idx_no_offset / pipe->ep_char.periodic.interval) * pipe->ep_char.periodic.interval; //Floor divide and the multiply again
                uint32_t next_interval_idx_no_offset = (last_interval_mod_idx_no_offset + pipe->ep_char.periodic.interval);
                //We want at least a half interval or 2 frames of buffer space
                if (next_interval_idx_no_offset - cur_mod_idx_no_offset > (pipe->ep_char.periodic.interval / 2)
                    && next_interval_idx_no_offset - cur_mod_idx_no_offset >= 2) {
                        start_idx = (next_interval_idx_no_offset + pipe->ep_char.periodic.phase_offset_frames) & (XFER_LIST_LEN_ISOC - 1);
                } else {
                    //Not enough time until the next schedule, add another interval to it.
                        start_idx =  (next_interval_idx_no_offset + pipe->ep_char.periodic.interval + pipe->ep_char.periodic.phase_offset_frames) & (XFER_LIST_LEN_ISOC - 1);
                }
            } else {
                //Start index is based on previously filled buffer
                uint32_t prev_buffer_idx = (pipe->multi_buffer_control.wr_idx - 1) & (NUM_BUFFERS - 1);
                dma_buffer_block_t *prev_filled_buffer = pipe->buffers[prev_buffer_idx];
                start_idx = prev_filled_buffer->flags.isoc.next_irp_start_idx;
            }
            _buffer_fill_isoc(buffer_to_fill, irp, is_in, mps, (int)pipe->ep_char.periodic.interval, start_idx);
            break;
        }
        case USB_PRIV_XFER_TYPE_BULK: {
            _buffer_fill_bulk(buffer_to_fill, irp, is_in);
            break;
        }
        case USB_PRIV_XFER_TYPE_INTR: {
            _buffer_fill_intr(buffer_to_fill, irp, is_in, mps);
            break;
        }
        default: {
            abort();
            break;
        }
    }
    buffer_to_fill->irp = irp;
    IRP_STATE_SET(irp->reserved_flags, IRP_STATE_INFLIGHT);
    //Update multi buffer flags
    pipe->multi_buffer_control.wr_idx++;
    pipe->multi_buffer_control.buffer_num_to_fill--;
    pipe->multi_buffer_control.buffer_num_to_exec++;
}

static void _buffer_exec(pipe_t *pipe)
{
    assert(pipe->multi_buffer_control.rd_idx != pipe->multi_buffer_control.wr_idx || pipe->multi_buffer_control.buffer_num_to_exec > 0);
    dma_buffer_block_t *buffer_to_exec = pipe->buffers[pipe->multi_buffer_control.rd_idx];
    assert(buffer_to_exec->irp != NULL);

    uint32_t start_idx;
    int desc_list_len;
    switch (pipe->ep_char.type) {
        case USB_PRIV_XFER_TYPE_CTRL: {
            start_idx = 0;
            desc_list_len = XFER_LIST_LEN_CTRL;
            //Set the channel's direction to OUT and PID to 0 respectively for the the setup stage
            usbh_hal_chan_set_dir(pipe->chan_obj, false);   //Setup stage is always OUT
            usbh_hal_chan_set_pid(pipe->chan_obj, 0);   //Setup stage always has a PID of DATA0
            break;
        }
        case USB_PRIV_XFER_TYPE_ISOCHRONOUS: {
            start_idx = buffer_to_exec->flags.isoc.irp_start_idx;
            desc_list_len = XFER_LIST_LEN_ISOC;
            break;
        }
        case USB_PRIV_XFER_TYPE_BULK: {
            start_idx = 0;
            desc_list_len = (buffer_to_exec->flags.bulk.zero_len_packet) ? XFER_LIST_LEN_BULK : 1;
            break;
        }
        case USB_PRIV_XFER_TYPE_INTR: {
            start_idx = 0;
            desc_list_len = buffer_to_exec->flags.intr.num_qtds;
            break;
        }
        default: {
            start_idx = 0;
            desc_list_len = 0;
            abort();
            break;
        }
    }
    //Update buffer and multi buffer flags
    buffer_to_exec->status_flags.executing = 1;
    pipe->multi_buffer_control.buffer_is_executing = 1;
    usbh_hal_chan_activate(pipe->chan_obj, buffer_to_exec->xfer_desc_list, desc_list_len, start_idx);
}

static bool _buffer_check_done(pipe_t *pipe)
{
    if (pipe->ep_char.type != USB_PRIV_XFER_TYPE_CTRL) {
        return true;
    }
    //Only control transfers need to be continued
    dma_buffer_block_t *buffer_inflight = pipe->buffers[pipe->multi_buffer_control.rd_idx];
    bool next_dir_is_in;
    int next_pid;
    if (buffer_inflight->flags.ctrl.cur_stg == 0) { //Just finished control stage
        if (buffer_inflight->flags.ctrl.data_stg_skip) {
            //Skipping data stage. Go straight to status stage
            next_dir_is_in = true;     //With no data stage, status stage must be IN
            next_pid = 1;       //Status stage always has a PID of DATA1
            buffer_inflight->flags.ctrl.cur_stg = 2;    //Skip over the null descriptor representing the skipped data stage
        } else {
            //Go to data stage
            next_dir_is_in = buffer_inflight->flags.ctrl.data_stg_in;
            next_pid = 1;   //Data stage always starts with a PID of DATA1
            buffer_inflight->flags.ctrl.cur_stg = 1;
        }
    } else if (buffer_inflight->flags.ctrl.cur_stg == 1) {  //Just finished data stage. Go to status stage
        next_dir_is_in = !buffer_inflight->flags.ctrl.data_stg_in;  //Status stage is always the opposite direction of data stage
        next_pid = 1;   //Status stage always has a PID of DATA1
        buffer_inflight->flags.ctrl.cur_stg = 2;
    } else {    //Just finished status stage. Transfer is complete
        return true;
    }
    //Continue the control transfer
    usbh_hal_chan_set_dir(pipe->chan_obj, next_dir_is_in);
    usbh_hal_chan_set_pid(pipe->chan_obj, next_pid);
    usbh_hal_chan_activate(pipe->chan_obj, buffer_inflight->xfer_desc_list, XFER_LIST_LEN_CTRL, buffer_inflight->flags.ctrl.cur_stg);
    return false;
}

static inline void _buffer_parse_ctrl(dma_buffer_block_t *buffer)
{
    usb_irp_t *irp = buffer->irp;
    //Update IRP's actual number of bytes
    if (buffer->flags.ctrl.data_stg_skip)     {
        //There was no data stage. Just set the actual length to zero
        irp->actual_num_bytes = 0;
    } else {
        //Parse the data stage for the remaining length
        int rem_len;
        int desc_status;
        usbh_hal_xfer_desc_parse(buffer->xfer_desc_list, 1, &rem_len, &desc_status);
        assert(desc_status == USBH_HAL_XFER_DESC_STS_SUCCESS);
        assert(rem_len <= irp->num_bytes);
        irp->actual_num_bytes = irp->num_bytes - rem_len;
    }
    //Update IRP status
    irp->status = USB_TRANSFER_STATUS_COMPLETED;
    //Clear the descriptor list
    memset(buffer->xfer_desc_list, XFER_LIST_LEN_CTRL, sizeof(usbh_ll_dma_qtd_t));
}

static inline void _buffer_parse_bulk(dma_buffer_block_t *buffer)
{
    usb_irp_t *irp = buffer->irp;
    //Update IRP's actual number of bytes
    int rem_len;
    int desc_status;
    usbh_hal_xfer_desc_parse(buffer->xfer_desc_list, 0, &rem_len, &desc_status);
    assert(desc_status == USBH_HAL_XFER_DESC_STS_SUCCESS);
    assert(rem_len <= irp->num_bytes);
    irp->actual_num_bytes = irp->num_bytes - rem_len;
    //Update IRP's status
    irp->status = USB_TRANSFER_STATUS_COMPLETED;
    //Clear the descriptor list
    memset(buffer->xfer_desc_list, XFER_LIST_LEN_BULK, sizeof(usbh_ll_dma_qtd_t));
}

static inline void _buffer_parse_intr(dma_buffer_block_t *buffer, bool is_in, int mps)
{
    usb_irp_t *irp = buffer->irp;
    int intr_stop_idx = buffer->status_flags.stop_idx;
    if (is_in) {
        if (intr_stop_idx > 0) { //This is an early stop (short packet)
            assert(intr_stop_idx <= buffer->flags.intr.num_qtds);
            int rem_len;
            int desc_status;
            for (int i = 0; i < intr_stop_idx - 1; i++) {    //Check all packets before the short
                usbh_hal_xfer_desc_parse(buffer->xfer_desc_list, i, &rem_len, &desc_status);
                assert(rem_len == 0 && desc_status == USBH_HAL_XFER_DESC_STS_SUCCESS);
            }
            //Check the short packet
            usbh_hal_xfer_desc_parse(buffer->xfer_desc_list, intr_stop_idx - 1, &rem_len, &desc_status);
            assert(rem_len > 0 && desc_status == USBH_HAL_XFER_DESC_STS_SUCCESS);
            //Update actual bytes
            irp->actual_num_bytes = (mps * intr_stop_idx - 2) + (mps - rem_len);
        } else {
            //Check that all but the last packet transmitted MPS
            for (int i = 0; i < buffer->flags.intr.num_qtds - 1; i++) {
                int rem_len;
                int desc_status;
                usbh_hal_xfer_desc_parse(buffer->xfer_desc_list, i, &rem_len, &desc_status);
                assert(rem_len == 0 && desc_status == USBH_HAL_XFER_DESC_STS_SUCCESS);
            }
            //Check the last packet
            int last_packet_rem_len;
            int last_packet_desc_status;
            usbh_hal_xfer_desc_parse(buffer->xfer_desc_list, buffer->flags.intr.num_qtds - 1, &last_packet_rem_len, &last_packet_desc_status);
            assert(last_packet_desc_status == USBH_HAL_XFER_DESC_STS_SUCCESS);
            //All packets except last MUST be MPS. So just deduct the remaining length of the last packet to get actual number of bytes
            irp->actual_num_bytes = irp->num_bytes - last_packet_rem_len;
        }
    } else {
        //OUT INTR transfers can only complete successfully if all MPS packets have been transmitted. Double check
        for (int i = 0 ; i < buffer->flags.intr.num_qtds; i++) {
            int rem_len;
            int desc_status;
            usbh_hal_xfer_desc_parse(buffer->xfer_desc_list, i, &rem_len, &desc_status);
            assert(rem_len == 0 && desc_status == USBH_HAL_XFER_DESC_STS_SUCCESS);
        }
        irp->actual_num_bytes = irp->num_bytes;
    }
    //Update IRP's status
    irp->status = USB_TRANSFER_STATUS_COMPLETED;
    //Clear the descriptor list
    memset(buffer->xfer_desc_list, XFER_LIST_LEN_INTR, sizeof(usbh_ll_dma_qtd_t));
}

static inline void _buffer_parse_isoc(dma_buffer_block_t *buffer, bool is_in)
{
    usb_irp_t *irp = buffer->irp;
    int desc_idx = buffer->flags.isoc.irp_start_idx;    //Descriptor index tracks which descriptor in the QTD list
    for (int pkt_idx = 0; pkt_idx < irp->num_iso_packets; pkt_idx++) {
        //Clear the filled descriptor
        int rem_len;
        int desc_status;
        usbh_hal_xfer_desc_parse(buffer->xfer_desc_list, desc_idx, &rem_len, &desc_status);
        usbh_hal_xfer_desc_clear(buffer->xfer_desc_list, desc_idx);
        assert(rem_len == 0 || is_in);
        assert(desc_status == USBH_HAL_XFER_DESC_STS_SUCCESS || USBH_HAL_XFER_DESC_STS_NOT_EXECUTED);
        assert(rem_len <= irp->iso_packet_desc[pkt_idx].length);    //Check for DMA errata
        //Update ISO packet actual length and status
        irp->iso_packet_desc[pkt_idx].actual_length = irp->iso_packet_desc[pkt_idx].length - rem_len;
        irp->iso_packet_desc[pkt_idx].status = (desc_status == USBH_HAL_XFER_DESC_STS_NOT_EXECUTED) ? USB_TRANSFER_STATUS_SKIPPED : USB_TRANSFER_STATUS_COMPLETED;
        //A descriptor is also allocated for unscheduled frames. We need to skip over them
        desc_idx += buffer->flags.isoc.interval;
        if (desc_idx >= XFER_LIST_LEN_INTR) {
            desc_idx -= XFER_LIST_LEN_INTR;
        }
    }
}

static inline void _buffer_parse_error(dma_buffer_block_t *buffer)
{
    //The IRP had an error, so we consider that NO bytes were transferred
    usb_irp_t *irp = buffer->irp;
    irp->actual_num_bytes = 0;
    for (int i = 0; i < irp->num_iso_packets; i++) {
        irp->iso_packet_desc[i].actual_length = 0;
    }
    //Update status of IRP
    if (buffer->status_flags.cancelled) {
        irp->status = USB_TRANSFER_STATUS_CANCELED;
    } else if (buffer->status_flags.pipe_state == HCD_PIPE_STATE_INVALID) {
        irp->status = USB_TRANSFER_STATUS_NO_DEVICE;
    } else {
        switch (buffer->status_flags.pipe_event) {
            case HCD_PIPE_EVENT_ERROR_XFER: //Excessive transaction error
                irp->status = USB_TRANSFER_STATUS_ERROR;
                break;
            case HCD_PIPE_EVENT_ERROR_OVERFLOW:
                irp->status = USB_TRANSFER_STATUS_OVERFLOW;
                break;
            case HCD_PIPE_EVENT_ERROR_STALL:
                irp->status = USB_TRANSFER_STATUS_STALL;
                break;
            case HCD_PIPE_EVENT_IRP_DONE:   //Special case where we are cancelling an IRP due to pipe_retire
                irp->status = USB_TRANSFER_STATUS_CANCELED;
                break;
            default:
                //HCD_PIPE_EVENT_ERROR_IRP_NOT_AVAIL should never occur
                abort();
                break;
        }
    }
    //Clear error flags
    buffer->status_flags.val = 0;
}

static void _buffer_parse(pipe_t *pipe)
{
    assert(pipe->multi_buffer_control.buffer_num_to_parse > 0);
    dma_buffer_block_t *buffer_to_parse = pipe->buffers[pipe->multi_buffer_control.fr_idx];
    assert(buffer_to_parse->irp != NULL);
    bool is_in = pipe->ep_char.bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK;
    int mps = pipe->ep_char.mps;

    //Parsing the buffer will update the buffer's corresponding IRP
    if (buffer_to_parse->status_flags.error_occurred) {
        _buffer_parse_error(buffer_to_parse);
    } else {
        switch (pipe->ep_char.type) {
            case USB_PRIV_XFER_TYPE_CTRL: {
                _buffer_parse_ctrl(buffer_to_parse);
                break;
            }
            case USB_PRIV_XFER_TYPE_ISOCHRONOUS: {
                _buffer_parse_isoc(buffer_to_parse, is_in);
                break;
            }
            case USB_PRIV_XFER_TYPE_BULK: {
                _buffer_parse_bulk(buffer_to_parse);
                break;
            }
            case USB_PRIV_XFER_TYPE_INTR: {
                _buffer_parse_intr(buffer_to_parse, is_in, mps);
                break;
            }
            default: {
                abort();
                break;
            }
        }
    }
    usb_irp_t *irp = buffer_to_parse->irp;
    IRP_STATE_SET(irp->reserved_flags, IRP_STATE_DONE);
    buffer_to_parse->irp = NULL;
    buffer_to_parse->flags.val = 0; //Clear flags
    //Move the IRP to the done tailq
    TAILQ_INSERT_TAIL(&pipe->done_irp_tailq, irp, tailq_entry);
    pipe->num_irp_done++;
    //Update multi buffer flags
    pipe->multi_buffer_control.fr_idx++;
    pipe->multi_buffer_control.buffer_num_to_parse--;
    pipe->multi_buffer_control.buffer_num_to_fill++;
}

static void _buffer_flush_all(pipe_t *pipe, bool cancelled)
{
    int cur_num_to_mark_done =  pipe->multi_buffer_control.buffer_num_to_exec;
    for (int i = 0; i < cur_num_to_mark_done; i++) {
        //Mark any filled buffers as done
        _buffer_done_error(pipe, 0, pipe->state, pipe->last_event, cancelled);
    }
    int cur_num_to_parse = pipe->multi_buffer_control.buffer_num_to_parse;
    for (int i = 0; i < cur_num_to_parse; i++) {
        _buffer_parse(pipe);
    }
    //At this point, there should be no more filled buffers. Only IRPs in the pending or done tailq
}

// ---------------------------------------------- HCD Transfer Descriptors ---------------------------------------------

// ----------------------- Public --------------------------

esp_err_t hcd_irp_enqueue(hcd_pipe_handle_t pipe_hdl, usb_irp_t *irp)
{
    //Check that IRP has not already been enqueued
    HCD_CHECK(irp->reserved_ptr == NULL
              && IRP_STATE_GET(irp->reserved_flags) == IRP_STATE_IDLE,
              ESP_ERR_INVALID_STATE);
    pipe_t *pipe = (pipe_t *)pipe_hdl;

    HCD_ENTER_CRITICAL();
    //Check that pipe and port are in the correct state to receive IRPs
    HCD_CHECK_FROM_CRIT(pipe->port->state == HCD_PORT_STATE_ENABLED         //The pipe's port must be in the correct state
                        && pipe->state == HCD_PIPE_STATE_ACTIVE             //The pipe must be in the correct state
                        && !pipe->cs_flags.pipe_cmd_processing,            //Pipe cannot currently be processing a pipe command
                        ESP_ERR_INVALID_STATE);
    //Use the IRP's reserved_ptr to store the pipe's
    irp->reserved_ptr = (void *)pipe;
    //Add the IRP to the pipe's pending tailq
    IRP_STATE_SET(irp->reserved_flags, IRP_STATE_PENDING);
    TAILQ_INSERT_TAIL(&pipe->pending_irp_tailq, irp, tailq_entry);
    pipe->num_irp_pending++;
    //use the IRP's reserved_flags to store the IRP's current state
    if (_buffer_can_fill(pipe)) {
        _buffer_fill(pipe);
    }
    if (_buffer_can_exec(pipe)) {
        _buffer_exec(pipe);
    }
    if (!pipe->cs_flags.is_active) {
        //This is the first IRP to be enqueued into the pipe. Move the pipe to the list of active pipes
        TAILQ_REMOVE(&pipe->port->pipes_idle_tailq, pipe, tailq_entry);
        TAILQ_INSERT_TAIL(&pipe->port->pipes_active_tailq, pipe, tailq_entry);
        pipe->port->num_pipes_idle--;
        pipe->port->num_pipes_queued++;
        pipe->cs_flags.is_active = 1;
    }
    HCD_EXIT_CRITICAL();
    return ESP_OK;
}

usb_irp_t *hcd_irp_dequeue(hcd_pipe_handle_t pipe_hdl)
{
    pipe_t *pipe = (pipe_t *)pipe_hdl;
    usb_irp_t *irp;

    HCD_ENTER_CRITICAL();
    if (pipe->num_irp_done > 0) {
        irp = TAILQ_FIRST(&pipe->done_irp_tailq);
        TAILQ_REMOVE(&pipe->done_irp_tailq, irp, tailq_entry);
        pipe->num_irp_done--;
        //Check the IRP's reserved fields then reset them
        assert(irp->reserved_ptr == (void *)pipe && IRP_STATE_GET(irp->reserved_flags) == IRP_STATE_DONE);  //The IRP's reserved field should have been set to this pipe
        irp->reserved_ptr = NULL;
        IRP_STATE_SET(irp->reserved_flags, IRP_STATE_IDLE);
        if (pipe->cs_flags.is_active
            && pipe->num_irp_pending == 0 && pipe->num_irp_done == 0
            && pipe->multi_buffer_control.buffer_num_to_exec == 0 && pipe->multi_buffer_control.buffer_num_to_parse == 0) {
            //This pipe has no more enqueued IRPs. Move the pipe to the list of idle pipes
            TAILQ_REMOVE(&pipe->port->pipes_active_tailq, pipe, tailq_entry);
            TAILQ_INSERT_TAIL(&pipe->port->pipes_idle_tailq, pipe, tailq_entry);
            pipe->port->num_pipes_idle++;
            pipe->port->num_pipes_queued--;
            pipe->cs_flags.is_active = 0;
        }
    } else {
        //No more IRPs to dequeue from this pipe
        irp = NULL;
    }
    HCD_EXIT_CRITICAL();
    return irp;
}

esp_err_t hcd_irp_abort(usb_irp_t *irp)
{
    HCD_ENTER_CRITICAL();
    //Check that the IRP was enqueued to begin with
    HCD_CHECK_FROM_CRIT(irp->reserved_ptr != NULL
                        && IRP_STATE_GET(irp->reserved_flags) != IRP_STATE_IDLE,
                        ESP_ERR_INVALID_STATE);
    if (IRP_STATE_GET(irp->reserved_flags) == IRP_STATE_PENDING) {
        //IRP has not been executed so it can be aborted
        pipe_t *pipe = (pipe_t *)irp->reserved_ptr;
        //Remove it form the pending queue
        TAILQ_REMOVE(&pipe->pending_irp_tailq, irp, tailq_entry);
        pipe->num_irp_pending--;
        //Add it to the done queue
        TAILQ_INSERT_TAIL(&pipe->done_irp_tailq, irp, tailq_entry);
        pipe->num_irp_done++;
        //Update the IRP's current state, status, and actual length
        IRP_STATE_SET(irp->reserved_flags, IRP_STATE_DONE);
        irp->actual_num_bytes = 0;
        irp->status = USB_TRANSFER_STATUS_CANCELED;
        //If this is an ISOC IRP, update the ISO packet descriptors as well
        for (int i = 0; i < irp->num_iso_packets; i++) {
            irp->iso_packet_desc[i].actual_length = 0;
            irp->iso_packet_desc[i].status = USB_TRANSFER_STATUS_CANCELED;
        }
    }// Otherwise, the IRP is in-flight or already done thus cannot be aborted
    HCD_EXIT_CRITICAL();
    return ESP_OK;
}
