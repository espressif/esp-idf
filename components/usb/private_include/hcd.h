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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <sys/queue.h>
#include "esp_err.h"
#include "usb.h"

// ------------------------------------------------- Macros & Types ----------------------------------------------------

// ----------------------- States --------------------------

/**
 * @brief States of the HCD port
 *
 * @note The port can be thought of as an abstraction of the Root Hub that contains
 *       a single port.
 * @note These states roughly match the port states outlined in 11.5.1 of the
 *       USB2.0 specification.
 */
typedef enum {
    HCD_PORT_STATE_NOT_POWERED,     /**< The port is not powered */
    HCD_PORT_STATE_DISCONNECTED,    /**< The port is powered but no device is conencted */
    HCD_PORT_STATE_DISABLED,        /**< A device has connected to the port but has not been reset. SOF/keep alive are not being sent */
    HCD_PORT_STATE_RESETTING,       /**< The port is issuing a reset condition */
    HCD_PORT_STATE_SUSPENDED,       /**< The port has been suspended. */
    HCD_PORT_STATE_RESUMING,        /**< The port is issuing a resume condition */
    HCD_PORT_STATE_ENABLED,         /**< The port has been enabled. SOF/keep alive are being sent */
    HCD_PORT_STATE_RECOVERY,        /**< Port needs to be recovered from a fatal error (port error, overcurrent, or sudden disconnection) */
} hcd_port_state_t;

/**
 * @brief States of an HCD pipe
 *
 * Active:
 *  - Pipe is able to transmit data. IRPs can be enqueued.
 *  - Event if pipe has no IRPs enqueued, it can still be in the active state.
 * Halted:
 *  - An error has occurred on the pipe. IRPs will no longer be executed.
 *  - Halt should be cleared using the clear command
 * Invalid:
 *  - The underlying device that the pipe connects is not longer valid, thus making the pipe invalid.
 *  - Pending IRPs should be dequeued and the pipe should be freed.
 */
typedef enum {
    HCD_PIPE_STATE_ACTIVE,          /**< The pipe is active */
    HCD_PIPE_STATE_HALTED,          /**< The pipe is halted */
    HCD_PIPE_STATE_INVALID,         /**< The pipe no longer exists and should be freed */
} hcd_pipe_state_t;

// ----------------------- Events --------------------------

/**
 * @brief HCD port events
 *
 * On receiving a port event, hcd_port_handle_event() should be called to handle that event
 */
typedef enum {
    HCD_PORT_EVENT_NONE,            /**< No event has ocurred. Or the previous event is no longer valid */
    HCD_PORT_EVENT_CONNECTION,      /**< A device has been connected to the port */
    HCD_PORT_EVENT_DISCONNECTION,   /**< A device disconnection has been detected */
    HCD_PORT_EVENT_ERROR,           /**< A port error has been detected. Port is now HCD_PORT_STATE_RECOVERY  */
    HCD_PORT_EVENT_OVERCURRENT,     /**< Overcurrent detected on the port. Port is now HCD_PORT_STATE_RECOVERY */
    HCD_PORT_EVENT_SUDDEN_DISCONN,  /**< The port has suddenly disconencted (i.e., there was an enabled device connected
                                         to the port when the disconnection occurred. Port is now HCD_PORT_STATE_RECOVERY. */
} hcd_port_event_t;

/**
 * @brief HCD pipe events
 *
 * @note Pipe error events will put the pipe into the HCD_PIPE_STATE_HALTED state
 * @note The HCD_PIPE_EVENT_INVALID will put the pipe in the HCD_PIPE_STATE_INVALID state
 */
typedef enum {
    HCD_PIPE_EVENT_NONE,                    /**< The pipe has no events (used to indicate no events when polling) */
    HCD_PIPE_EVENT_IRP_DONE,                /**< The pipe has completed an IRP. The IRP can be dequeued */
    HCD_PIPE_EVENT_INVALID,                 /**< The pipe is invalid because  */
    HCD_PIPE_EVENT_ERROR_XFER,              /**< Excessive (three consecutive) transaction errors (e.g., no ACK, bad CRC etc) */
    HCD_PIPE_EVENT_ERROR_IRP_NOT_AVAIL,     /**< IRP was not available */
    HCD_PIPE_EVENT_ERROR_OVERFLOW,          /**< Received more data than requested. Usually a Packet babble error
                                                 (i.e., an IN packet has exceeded the endpoint's MPS) */
    HCD_PIPE_EVENT_ERROR_STALL,             /**< Pipe received a STALL response received */
} hcd_pipe_event_t;

// ---------------------- Commands -------------------------

/**
 * @brief HCD port commands
 */
typedef enum {
    HCD_PORT_CMD_POWER_ON,          /**< Power ON the port */
    HCD_PORT_CMD_POWER_OFF,         /**< Power OFF the port */
    HCD_PORT_CMD_RESET,             /**< Issue a reset on the port */
    HCD_PORT_CMD_SUSPEND,           /**< Suspend the port */
    HCD_PORT_CMD_RESUME,            /**< Resume the port */
    HCD_PORT_CMD_DISABLE,           /**< Disable the port (stops the SOFs or keep alive) */
} hcd_port_cmd_t;

/**
 * @brief HCD pipe commands
 *
 * The pipe commands represent the list of pipe manipulations outlined in 10.5.2.2. of USB2.0 specification.
 */
typedef enum {
    HCD_PIPE_CMD_ABORT,             /**< Retire all scheduled IRPs. Pipe's state remains unchanged */
    HCD_PIPE_CMD_RESET,             /**< Retire all scheduled IRPs. Pipe's state moves to active */
    HCD_PIPE_CMD_CLEAR,             /**< Pipe's state moves from halted to active */
    HCD_PIPE_CMD_HALT               /**< Pipe's state moves to halted */
} hcd_pipe_cmd_t;

// -------------------- Object Types -----------------------

/**
 * @brief Port handle type
 */
typedef void * hcd_port_handle_t;

/**
 * @brief Pipe handle type
 */
typedef void * hcd_pipe_handle_t;

/**
 * @brief Port event callback type
 *
 * This callback is run when a port event occurs
 */
typedef bool (*hcd_port_isr_callback_t)(hcd_port_handle_t port_hdl, hcd_port_event_t port_event, void *user_arg, bool in_isr);

/**
 * @brief Pipe event callback
 *
 * This callback is run when a pipe event occurs
 */
typedef bool (*hcd_pipe_isr_callback_t)(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr);

/**
 * @brief HCD configuration structure
 */
typedef struct {
    int intr_flags;                         /**< Interrupt flags for HCD interrupt */
} hcd_config_t;

/**
 * @brief Port configuration structure
 */
typedef struct {
    hcd_port_isr_callback_t callback;       /**< HCD port event callback */
    void *callback_arg;                     /**< User argument for HCD port callback */
    void *context;
} hcd_port_config_t;

/**
 * @brief Pipe configuration structure
 *
 * @note The callback can be set to NULL if no callback is required (e.g., using HCD in a polling manner).
 */
typedef struct {
    hcd_pipe_isr_callback_t callback;       /**< HCD pipe event ISR callback */
    void *callback_arg;                     /**< User argument for HCD pipe callback */
    void *context;                          /**< Context variable used to associate the pipe with upper layer object */
    usb_desc_ep_t *ep_desc;                 /**< Pointer to endpoint descriptor of the pipe */
    uint8_t dev_addr;                       /**< Device address of the pipe */
    usb_speed_t dev_speed;                  /**< Speed of the device */
} hcd_pipe_config_t;

// --------------------------------------------- Host Controller Driver ------------------------------------------------

/**
 * @brief Installs the Host Controller Driver
 *
 * - Allocates memory and interrupt resources for the HCD and underlying ports
 * - Setups up HCD to use internal PHY
 *
 * @note This function must be called before any other HCD function is called
 *
 * @param config HCD configuration
 * @retval ESP_OK: HCD successfully installed
 * @retval ESP_ERR_NO_MEM: Insufficient memory
 * @retval ESP_ERR_INVALID_STATE: HCD is already installed
 * @retval ESP_ERR_NOT_FOUND: HCD could not allocate interrupt
 * @retval ESP_ERR_INVALID_ARG: Arguments are invalid
 */
esp_err_t hcd_install(const hcd_config_t *config);

/**
 * @brief Uninstalls the HCD
 *
 * Before uninstalling the HCD, the following conditions should be met:
 * - All ports must be uninitialized, all pipes freed
 *
 * @retval ESP_OK: HCD successfully uninstalled
 * @retval ESP_ERR_INVALID_STATE: HCD is not in the right condition to be uninstalled
 */
esp_err_t hcd_uninstall(void);

// ---------------------------------------------------- HCD Port -------------------------------------------------------

/**
 * @brief Initialize a particular port of the HCD
 *
 * After a port is initialized, it will be put into the HCD_PORT_STATE_NOT_POWERED state.
 *
 * @note The host controller only has one port, thus the only valid port_number is 1
 *
 * @param[in] port_number Port number
 * @param[in] port_config Port configuration
 * @param[out] port_hdl Port handle
 * @retval ESP_OK: Port enabled
 * @retval ESP_ERR_NO_MEM: Insufficient memory
 * @retval ESP_ERR_INVALID_STATE: The port is already enabled
 * @retval ESP_ERR_NOT_FOUND: Port number not found
 * @retval ESP_ERR_INVALID_ARG: Arguments are invalid
 */
esp_err_t hcd_port_init(int port_number, hcd_port_config_t *port_config, hcd_port_handle_t *port_hdl);

/**
 * @brief Deinitialize a particular port
 *
 * The port must be placed in the HCD_PORT_STATE_NOT_POWERED or HCD_PORT_STATE_RECOVERY state before it can be
 * deinitialized.
 *
 * @param port_hdl Port handle
 * @retval ESP_OK: Port disabled
 * @retval ESP_ERR_INVALID_STATE: The port is not in a condition to be disabled (not unpowered)
 */
esp_err_t hcd_port_deinit(hcd_port_handle_t port_hdl);

/**
 * @brief Execute a port command
 *
 * Call this function to manipulate a port (e.g., powering it ON, sending a reset etc). The following conditions
 * must be met when calling this function:
 * - The port is in the correct state for the command (e.g., port must be suspend in order to use the resume command)
 * - The port does not have any pending events
 *
 * @note This function is internally protected by a mutex. If multiple threads call this function, this function will
 *       can block.
 * @note For some of the commands that involve a blocking delay (e.g., reset and resume), if the port's state changes
 *       unexpectedly (e.g., a disconnect during a resume), this function will return ESP_ERR_INVALID_RESPONSE.
 *
 * @param port_hdl Port handle
 * @param command Command for the HCD port
 * @retval ESP_OK: Command executed successfully
 * @retval ESP_ERR_INVALID_STATE: Conditions have not been met to call this function
 * @retval ESP_ERR_INVALID_RESPONSE: The command is no longer valid due to a change in the port's state
 */
esp_err_t hcd_port_command(hcd_port_handle_t port_hdl, hcd_port_cmd_t command);

/**
 * @brief Get the port's current state
 *
 * @param port_hdl Port handle
 * @return hcd_port_state_t Current port state
 */
hcd_port_state_t hcd_port_get_state(hcd_port_handle_t port_hdl);

/**
 * @brief Get the speed of the port
 *
 * The speed of the port is determined by the speed of the device connected to it.
 *
 * @note This function is only valid after a device directly to the port and has been reset
 *
 * @param[in port_hdl Port handle
 * @param[out] speed Speed of the port
 * @retval ESP_OK Device speed obtained
 * @retval ESP_ERR_INVALID_STATE: No valid device connected to the port
 * @retval ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t hcd_port_get_speed(hcd_port_handle_t port_hdl, usb_speed_t *speed);

/**
 * @brief Handle a ports event
 *
 * When an port event occurs (as indicated by a callback), this function should be called the handle this event. A
 * port's event should always be handled before attempting to execute a port command. Note that is actually handled
 * may be different than the event reflected in the callback.
 *
 * If the port has no events, this function will return HCD_PORT_EVENT_NONE.
 *
 * @note If callbacks are not used, this function can also be used in a polling manner to repeatedely check for and
 *       handle a port's events.
 * @note This function is internally protected by a mutex. If multiple threads call this function, this function will
 *       can block.
 *
 * @param port_hdl Port handle
 * @return hcd_port_event_t The port event that was handled
 */
hcd_port_event_t hcd_port_handle_event(hcd_port_handle_t port_hdl);

/**
 * @brief Recover a port after a fatal error has occurred on it
 *
 * The port must be in the HCD_PORT_STATE_RECOVERY state to be called. Recovering the port will involve issuing a soft
 * reset on the underlying USB controller. The port will be returned to the HCD_PORT_STATE_NOT_POWERED state.
 *
 * @param port_hdl Port handle
 * @retval ESP_OK Port recovered successfully
 * @retval ESP_ERR_INVALID_STATE Port is not in the HCD_PORT_STATE_RECOVERY state
 */
esp_err_t hcd_port_recover(hcd_port_handle_t port_hdl);

/**
 * @brief Get the context variable of a port
 *
 * @param port_hdl Port handle
 * @return void* Context variable
 */
void *hcd_port_get_ctx(hcd_port_handle_t port_hdl);

// --------------------------------------------------- HCD Pipes -------------------------------------------------------

/**
 * @brief Allocate a pipe
 *
 * When allocating a pipe, the HCD will assess whether there are sufficient resources (i.e., bus time, and controller
 * channels). If sufficient, the pipe will be allocated.
 *
 * @note Currently, Interrupt and Isochronous pipes are not supported yet
 * @note The host port must be in the enabled state before a pipe can be allcoated
 *
 * @param[in] port_hdl Handle of the port this pipe will be routed through
 * @param[in] pipe_config Pipe configuration
 * @param[out] pipe_hdl Pipe handle
 *
 * @retval ESP_OK: Pipe successfully allocated
 * @retval ESP_ERR_NO_MEM: Insufficient memory
 * @retval ESP_ERR_INVALID_ARG: Arguments are invalid
 * @retval ESP_ERR_INVALID_STATE: Host port is not in the correct state to allocate a pipe
 * @retval ESP_ERR_NOT_SUPPORTED: The pipe cannot be supported
 */
esp_err_t hcd_pipe_alloc(hcd_port_handle_t port_hdl, const hcd_pipe_config_t *pipe_config, hcd_pipe_handle_t *pipe_hdl);

/**
 * @brief Free a pipe
 *
 * Frees the resources used by an HCD pipe. The pipe's handle should be discarded after calling this function. The pipe
 * must be in following condition before it can be freed:
 * - All IRPs have been dequeued
 *
 * @param pipe_hdl Pipe handle
 *
 * @retval ESP_OK: Pipe successfully freed
 * @retval ESP_ERR_INVALID_STATE: Pipe is not in a condition to be freed
 */
esp_err_t hcd_pipe_free(hcd_pipe_handle_t pipe_hdl);

/**
 * @brief Update a pipe's device address and maximum packet size
 *
 * This function is intended to be called on default pipes during enumeration in order to update the pipe's device
 * address and maximum packet size. This function can only be called on a pipe that has met the following conditions:
 * - Pipe is still valid (i.e., not in the HCD_PIPE_STATE_INVALID state)
 * - Pipe is not currently processing a command
 * - All IRPs have been dequeued from the pipe
 *
 * @param pipe_hdl Pipe handle
 * @param dev_addr New device address
 * @param mps New Maximum Packet Size
 *
 * @retval ESP_OK: Pipe successfully updated
 * @retval ESP_ERR_INVALID_STATE: Pipe is no in a condition to be updated
 */
esp_err_t hcd_pipe_update(hcd_pipe_handle_t pipe_hdl, uint8_t dev_addr, int mps);

/**
 * @brief Get the context variable of a pipe from its handle
 *
 * @param pipe_hdl Pipe handle
 * @return void* Context variable
 */
void *hcd_pipe_get_ctx(hcd_pipe_handle_t pipe_hdl);

/**
 * @brief Get the current sate of the pipe
 *
 * @param pipe_hdl Pipe handle
 * @return hcd_pipe_state_t Current state of the pipe
 */
hcd_pipe_state_t hcd_pipe_get_state(hcd_pipe_handle_t pipe_hdl);

/**
 * @brief Execute a command on a particular pipe
 *
 * Pipe commands allow a pipe to be manipulated (such as clearing a halt, retiring all IRPs etc). The following
 * conditions must for a pipe command to be issued:
 * - Pipe is still valid (i.e., not in the HCD_PIPE_STATE_INVALID)
 * - No other thread/task processing a command on the pipe concurrently (will return)
 *
 * @note Some pipe commands will block until the pipe's current inflight IRP is complete. If the pipe's state
 *       changes unexpectedley, this function will return ESP_ERR_INVALID_RESPONSE
 *
 * @param pipe_hdl Pipe handle
 * @param command Pipe command
 * @retval ESP_OK: Command executed successfully
 * @retval ESP_ERR_INVALID_STATE: The pipe is not in the correct state/condition too execute the command
 * @retval ESP_ERR_INVALID_RESPONSE: The pipe's state changed unexpectedley
 */
esp_err_t hcd_pipe_command(hcd_pipe_handle_t pipe_hdl, hcd_pipe_cmd_t command);

/**
 * @brief Get the last event that occurred on a pipe
 *
 * This function allows a pipe to be polled for events (i.e., when callbacks are not used). Once an event has been
 * obtained, this function reset the last event of the pipe to HCD_PIPE_EVENT_NONE.
 *
 * @param pipe_hdl Pipe handle
 * @return hcd_pipe_event_t Last pipe event to occur
 */
hcd_pipe_event_t hcd_pipe_get_event(hcd_pipe_handle_t pipe_hdl);

// ---------------------------------------------------- HCD IRPs -------------------------------------------------------

/**
 * @brief Enqueue an IRP to a particular pipe
 *
 * The following conditions must be met before an IRP can be enqueued:
 * - The IRP is properly initialized (data buffer and transfer length are set)
 * - The IRP must not already be enqueued
 * - The pipe must be in the HCD_PIPE_STATE_ACTIVE state
 *
 * @param pipe_hdl Pipe handle
 * @param irp I/O Request Packet to enqueue
 * @retval ESP_OK: IRP enqueued successfully
 * @retval ESP_ERR_INVALID_STATE: Conditions not met to enqueue IRP
 */
esp_err_t hcd_irp_enqueue(hcd_pipe_handle_t pipe_hdl, usb_irp_t *irp);

/**
 * @brief Dequeue an IRP from a particular pipe
 *
 * This function should be called on a pipe after a pipe receives a HCD_PIPE_EVENT_IRP_DONE event. If a pipe has
 * multiple IRPs that can be dequeued, this function should be called repeatedely until all IRPs are dequeued. If a pipe
 * has no more IRPs to dequeue, this function will return NULL.
 *
 * @param pipe_hdl Pipe handle
 * @return usb_irp_t* Dequeued I/O Request Packet, or NULL if no more IRPs to dequeue
 */
usb_irp_t *hcd_irp_dequeue(hcd_pipe_handle_t pipe_hdl);

/**
 * @brief Abort an enqueued IRP
 *
 * This function will attempt to abort an IRP that is already enqueued. If the IRP has yet to be executed, it will be
 * "cancelled" and can then be dequeued. If the IRP is currenty inflight or has already completed, the IRP will not be
 * affected by this function.
 *
 * @param irp I/O Request Packet to abort
 * @retval ESP_OK: IRP successfully aborted, or was not affected by this function
 * @retval ESP_ERR_INVALID_STATE: IRP was never enqueued
 */
esp_err_t hcd_irp_abort(usb_irp_t *irp);

#ifdef __cplusplus
}
#endif
