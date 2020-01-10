// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include "hal/can_types.h"
#include "hal/can_ll.h"

/* ------------------------- Defines and Typedefs --------------------------- */

//Error active interrupt related
#define CAN_HAL_EVENT_BUS_OFF               (1 << 0)
#define CAN_HAL_EVENT_BUS_RECOV_CPLT        (1 << 1)
#define CAN_HAL_EVENT_BUS_RECOV_PROGRESS    (1 << 2)
#define CAN_HAL_EVENT_ABOVE_EWL             (1 << 3)
#define CAN_HAL_EVENT_BELOW_EWL             (1 << 4)
#define CAN_HAL_EVENT_ERROR_PASSIVE         (1 << 5)
#define CAN_HAL_EVENT_ERROR_ACTIVE          (1 << 6)
#define CAN_HAL_EVENT_BUS_ERR               (1 << 7)
#define CAN_HAL_EVENT_ARB_LOST              (1 << 8)
#define CAN_HAL_EVENT_RX_BUFF_FRAME         (1 << 9)
#define CAN_HAL_EVENT_TX_BUFF_FREE          (1 << 10)

typedef struct {
    can_dev_t *dev;
} can_hal_context_t;

typedef can_ll_frame_buffer_t can_hal_frame_t;

/* ---------------------------- Init and Config ----------------------------- */

/**
 * @brief Initialize CAN peripheral and HAL context
 *
 * Sets HAL context, puts CAN peripheral into reset mode, then sets some
 * registers with default values.
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successfully initialized, false otherwise.
 */
bool can_hal_init(can_hal_context_t *hal_ctx);

/**
 * @brief Deinitialize the CAN peripheral and HAL context
 *
 * Clears any unhandled interrupts and unsets HAL context
 *
 * @param hal_ctx Context of the HAL layer
 */
void can_hal_deinit(can_hal_context_t *hal_ctx);

/**
 * @brief Configure the CAN peripheral
 *
 * @param hal_ctx Context of the HAL layer
 * @param t_config Pointer to timing configuration structure
 * @param f_config Pointer to filter configuration structure
 * @param intr_mask Mask of interrupts to enable
 * @param clkout_divider Clock divider value for CLKOUT. Set to -1 to disable CLKOUT
 */
void can_hal_configure(can_hal_context_t *hal_ctx, const can_timing_config_t *t_config, const can_filter_config_t *f_config, uint32_t intr_mask, uint32_t clkout_divider);

/* -------------------------------- Actions --------------------------------- */

/**
 * @brief Start the CAN peripheral
 *
 * Start the CAN peripheral by configuring its operating mode, then exiting
 * reset mode so that the CAN peripheral can participate in bus activities.
 *
 * @param hal_ctx Context of the HAL layer
 * @param mode Operating mode
 * @return True if successfully started, false otherwise.
 */
bool can_hal_start(can_hal_context_t *hal_ctx, can_mode_t mode);

/**
 * @brief Stop the CAN peripheral
 *
 * Stop the CAN peripheral by entering reset mode to stop any bus activity, then
 * setting the operating mode to Listen Only so that REC is frozen.
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successfully stopped, false otherwise.
 */
bool can_hal_stop(can_hal_context_t *hal_ctx);

/**
 * @brief Start bus recovery
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successfully started bus recovery, false otherwise.
 */
static inline bool can_hal_start_bus_recovery(can_hal_context_t *hal_ctx)
{
    return can_ll_exit_reset_mode(hal_ctx->dev);
}

/**
 * @brief Get the value of the TX Error Counter
 *
 * @param hal_ctx Context of the HAL layer
 * @return TX Error Counter Value
 */
static inline uint32_t can_hal_get_tec(can_hal_context_t *hal_ctx)
{
    return can_ll_get_tec((hal_ctx)->dev);
}

/**
 * @brief Get the value of the RX Error Counter
 *
 * @param hal_ctx Context of the HAL layer
 * @return RX Error Counter Value
 */
static inline uint32_t can_hal_get_rec(can_hal_context_t *hal_ctx)
{
    return can_ll_get_rec((hal_ctx)->dev);
}

/**
 * @brief Get the RX message count register
 *
 * @param hal_ctx Context of the HAL layer
 * @return RX message count
 */
static inline uint32_t can_hal_get_rx_msg_count(can_hal_context_t *hal_ctx)
{
    return can_ll_get_rx_msg_count((hal_ctx)->dev);
}

/**
 * @brief Check if the last transmitted frame was successful
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successful
 */
static inline bool can_hal_check_last_tx_successful(can_hal_context_t *hal_ctx)
{
    return can_ll_is_last_tx_successful((hal_ctx)->dev);
}

/* ----------------------------- Event Handling ----------------------------- */

/**
 * @brief Decode current events that triggered an interrupt
 *
 * This function should be called on every CAN interrupt. It will read (and
 * thereby clear) the interrupt register, then determine what events have
 * occurred to trigger the interrupt.
 *
 * @param hal_ctx Context of the HAL layer
 * @param bus_recovering Whether the CAN peripheral was previous undergoing bus recovery
 * @return Bit mask of events that have occurred
 */
uint32_t can_hal_decode_interrupt_events(can_hal_context_t *hal_ctx, bool bus_recovering);

/**
 * @brief Handle bus recovery complete
 *
 * This function should be called on an bus recovery complete event. It simply
 * enters reset mode to stop bus activity.
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successfully handled bus recovery completion, false otherwise.
 */
static inline bool can_hal_handle_bus_recov_cplt(can_hal_context_t *hal_ctx)
{
    return can_ll_enter_reset_mode((hal_ctx)->dev);
}

/**
 * @brief Handle arbitration lost
 *
 * This function should be called on an arbitration lost event. It simply clears
 * the clears the ALC register.
 *
 * @param hal_ctx Context of the HAL layer
 */
static inline void can_hal_handle_arb_lost(can_hal_context_t *hal_ctx)
{
    can_ll_clear_arb_lost_cap((hal_ctx)->dev);
}

/**
 * @brief Handle bus error
 *
 * This function should be called on an bus error event. It simply clears
 * the clears the ECC register.
 *
 * @param hal_ctx Context of the HAL layer
 */
static inline void can_hal_handle_bus_error(can_hal_context_t *hal_ctx)
{
    can_ll_clear_err_code_cap((hal_ctx)->dev);
}

/**
 * @brief Handle BUS OFF
 *
 * This function should be called on a BUS OFF event. It simply changes the
 * mode to LOM to freeze REC
 *
 * @param hal_ctx Context of the HAL layer
 */
static inline void can_hal_handle_bus_off(can_hal_context_t *hal_ctx)
{
    can_ll_set_mode((hal_ctx)->dev, CAN_MODE_LISTEN_ONLY);
}

/* ------------------------------- TX and RX -------------------------------- */

/**
 * @brief Format a CAN Frame
 *
 * This function takes a CAN message structure (containing ID, DLC, data, and
 * flags) and formats it to match the layout of the TX frame buffer.
 *
 * @param message Pointer to CAN message
 * @param frame Pointer to empty frame structure
 */
static inline void can_hal_format_frame(const can_message_t *message, can_hal_frame_t *frame)
{
    //Direct call to ll function
    can_ll_format_frame_buffer(message->identifier, message->data_length_code, message->data,
                               message->flags, frame);
}

/**
 * @brief Parse a CAN Frame
 *
 * This function takes a CAN frame (in the format of the RX frame buffer) and
 * parses it to a CAN message (containing ID, DLC, data and flags).
 *
 * @param frame Pointer to frame structure
 * @param message Pointer to empty message structure
 */
static inline void can_hal_parse_frame(can_hal_frame_t *frame, can_message_t *message)
{
    //Direct call to ll function
    can_ll_prase_frame_buffer(frame, &message->identifier, &message->data_length_code,
                              message->data, &message->flags);
}

/**
 * @brief Copy a frame into the TX buffer and transmit
 *
 * This function copies a formatted TX frame into the TX buffer, and the
 * transmit by setting the correct transmit command (e.g. normal, single shot,
 * self RX) in the command register.
 *
 * @param hal_ctx Context of the HAL layer
 * @param tx_frame Pointer to structure containing formatted TX frame
 */
void can_hal_set_tx_buffer_and_transmit(can_hal_context_t *hal_ctx, can_hal_frame_t *tx_frame);

/**
 * @brief Copy a frame from the RX buffer and release
 *
 * This function copies a frame from the RX buffer, then release the buffer (so
 * that it loads the next frame in the RX FIFO).
 *
 * @param hal_ctx Context of the HAL layer
 * @param rx_frame Pointer to structure to store RX frame
 */
static inline void can_hal_read_rx_buffer_and_clear(can_hal_context_t *hal_ctx, can_hal_frame_t *rx_frame)
{
    can_ll_get_rx_buffer(hal_ctx->dev, rx_frame);
    can_ll_set_cmd_release_rx_buffer(hal_ctx->dev);
    /*
     * Todo: Support overrun handling by:
     * - Check overrun status bit. Return false if overrun
     */
}


//Todo: Decode ALC register
//Todo: Decode error code capture

#ifdef __cplusplus
}
#endif
