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
#include "hal/twai_types.h"
#include "hal/twai_ll.h"

/* ------------------------- Defines and Typedefs --------------------------- */

//Error active interrupt related
#define TWAI_HAL_EVENT_BUS_OFF               (1 << 0)
#define TWAI_HAL_EVENT_BUS_RECOV_CPLT        (1 << 1)
#define TWAI_HAL_EVENT_BUS_RECOV_PROGRESS    (1 << 2)
#define TWAI_HAL_EVENT_ABOVE_EWL             (1 << 3)
#define TWAI_HAL_EVENT_BELOW_EWL             (1 << 4)
#define TWAI_HAL_EVENT_ERROR_PASSIVE         (1 << 5)
#define TWAI_HAL_EVENT_ERROR_ACTIVE          (1 << 6)
#define TWAI_HAL_EVENT_BUS_ERR               (1 << 7)
#define TWAI_HAL_EVENT_ARB_LOST              (1 << 8)
#define TWAI_HAL_EVENT_RX_BUFF_FRAME         (1 << 9)
#define TWAI_HAL_EVENT_TX_BUFF_FREE          (1 << 10)

typedef struct {
    twai_dev_t *dev;
} twai_hal_context_t;

typedef twai_ll_frame_buffer_t twai_hal_frame_t;

/* ---------------------------- Init and Config ----------------------------- */

/**
 * @brief Initialize TWAI peripheral and HAL context
 *
 * Sets HAL context, puts TWAI peripheral into reset mode, then sets some
 * registers with default values.
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successfully initialized, false otherwise.
 */
bool twai_hal_init(twai_hal_context_t *hal_ctx);

/**
 * @brief Deinitialize the TWAI peripheral and HAL context
 *
 * Clears any unhandled interrupts and unsets HAL context
 *
 * @param hal_ctx Context of the HAL layer
 */
void twai_hal_deinit(twai_hal_context_t *hal_ctx);

/**
 * @brief Configure the TWAI peripheral
 *
 * @param hal_ctx Context of the HAL layer
 * @param t_config Pointer to timing configuration structure
 * @param f_config Pointer to filter configuration structure
 * @param intr_mask Mask of interrupts to enable
 * @param clkout_divider Clock divider value for CLKOUT. Set to -1 to disable CLKOUT
 */
void twai_hal_configure(twai_hal_context_t *hal_ctx, const twai_timing_config_t *t_config, const twai_filter_config_t *f_config, uint32_t intr_mask, uint32_t clkout_divider);

/* -------------------------------- Actions --------------------------------- */

/**
 * @brief Start the TWAI peripheral
 *
 * Start the TWAI peripheral by configuring its operating mode, then exiting
 * reset mode so that the TWAI peripheral can participate in bus activities.
 *
 * @param hal_ctx Context of the HAL layer
 * @param mode Operating mode
 * @return True if successfully started, false otherwise.
 */
bool twai_hal_start(twai_hal_context_t *hal_ctx, twai_mode_t mode);

/**
 * @brief Stop the TWAI peripheral
 *
 * Stop the TWAI peripheral by entering reset mode to stop any bus activity, then
 * setting the operating mode to Listen Only so that REC is frozen.
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successfully stopped, false otherwise.
 */
bool twai_hal_stop(twai_hal_context_t *hal_ctx);

/**
 * @brief Start bus recovery
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successfully started bus recovery, false otherwise.
 */
static inline bool twai_hal_start_bus_recovery(twai_hal_context_t *hal_ctx)
{
    return twai_ll_exit_reset_mode(hal_ctx->dev);
}

/**
 * @brief Get the value of the TX Error Counter
 *
 * @param hal_ctx Context of the HAL layer
 * @return TX Error Counter Value
 */
static inline uint32_t twai_hal_get_tec(twai_hal_context_t *hal_ctx)
{
    return twai_ll_get_tec((hal_ctx)->dev);
}

/**
 * @brief Get the value of the RX Error Counter
 *
 * @param hal_ctx Context of the HAL layer
 * @return RX Error Counter Value
 */
static inline uint32_t twai_hal_get_rec(twai_hal_context_t *hal_ctx)
{
    return twai_ll_get_rec((hal_ctx)->dev);
}

/**
 * @brief Get the RX message count register
 *
 * @param hal_ctx Context of the HAL layer
 * @return RX message count
 */
static inline uint32_t twai_hal_get_rx_msg_count(twai_hal_context_t *hal_ctx)
{
    return twai_ll_get_rx_msg_count((hal_ctx)->dev);
}

/**
 * @brief Check if the last transmitted frame was successful
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successful
 */
static inline bool twai_hal_check_last_tx_successful(twai_hal_context_t *hal_ctx)
{
    return twai_ll_is_last_tx_successful((hal_ctx)->dev);
}

/* ----------------------------- Event Handling ----------------------------- */

/**
 * @brief Decode current events that triggered an interrupt
 *
 * This function should be called on every TWAI interrupt. It will read (and
 * thereby clear) the interrupt register, then determine what events have
 * occurred to trigger the interrupt.
 *
 * @param hal_ctx Context of the HAL layer
 * @param bus_recovering Whether the TWAI peripheral was previous undergoing bus recovery
 * @return Bit mask of events that have occurred
 */
uint32_t twai_hal_decode_interrupt_events(twai_hal_context_t *hal_ctx, bool bus_recovering);

/**
 * @brief Handle bus recovery complete
 *
 * This function should be called on an bus recovery complete event. It simply
 * enters reset mode to stop bus activity.
 *
 * @param hal_ctx Context of the HAL layer
 * @return True if successfully handled bus recovery completion, false otherwise.
 */
static inline bool twai_hal_handle_bus_recov_cplt(twai_hal_context_t *hal_ctx)
{
    return twai_ll_enter_reset_mode((hal_ctx)->dev);
}

/**
 * @brief Handle arbitration lost
 *
 * This function should be called on an arbitration lost event. It simply clears
 * the clears the ALC register.
 *
 * @param hal_ctx Context of the HAL layer
 */
static inline void twai_hal_handle_arb_lost(twai_hal_context_t *hal_ctx)
{
    twai_ll_clear_arb_lost_cap((hal_ctx)->dev);
}

/**
 * @brief Handle bus error
 *
 * This function should be called on an bus error event. It simply clears
 * the clears the ECC register.
 *
 * @param hal_ctx Context of the HAL layer
 */
static inline void twai_hal_handle_bus_error(twai_hal_context_t *hal_ctx)
{
    twai_ll_clear_err_code_cap((hal_ctx)->dev);
}

/**
 * @brief Handle BUS OFF
 *
 * This function should be called on a BUS OFF event. It simply changes the
 * mode to LOM to freeze REC
 *
 * @param hal_ctx Context of the HAL layer
 */
static inline void twai_hal_handle_bus_off(twai_hal_context_t *hal_ctx)
{
    twai_ll_set_mode((hal_ctx)->dev, TWAI_MODE_LISTEN_ONLY);
}

/* ------------------------------- TX and RX -------------------------------- */

/**
 * @brief Format a TWAI Frame
 *
 * This function takes a TWAI message structure (containing ID, DLC, data, and
 * flags) and formats it to match the layout of the TX frame buffer.
 *
 * @param message Pointer to TWAI message
 * @param frame Pointer to empty frame structure
 */
static inline void twai_hal_format_frame(const twai_message_t *message, twai_hal_frame_t *frame)
{
    //Direct call to ll function
    twai_ll_format_frame_buffer(message->identifier, message->data_length_code, message->data,
                               message->flags, frame);
}

/**
 * @brief Parse a TWAI Frame
 *
 * This function takes a TWAI frame (in the format of the RX frame buffer) and
 * parses it to a TWAI message (containing ID, DLC, data and flags).
 *
 * @param frame Pointer to frame structure
 * @param message Pointer to empty message structure
 */
static inline void twai_hal_parse_frame(twai_hal_frame_t *frame, twai_message_t *message)
{
    //Direct call to ll function
    twai_ll_prase_frame_buffer(frame, &message->identifier, &message->data_length_code,
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
void twai_hal_set_tx_buffer_and_transmit(twai_hal_context_t *hal_ctx, twai_hal_frame_t *tx_frame);

/**
 * @brief Copy a frame from the RX buffer and release
 *
 * This function copies a frame from the RX buffer, then release the buffer (so
 * that it loads the next frame in the RX FIFO).
 *
 * @param hal_ctx Context of the HAL layer
 * @param rx_frame Pointer to structure to store RX frame
 */
static inline void twai_hal_read_rx_buffer_and_clear(twai_hal_context_t *hal_ctx, twai_hal_frame_t *rx_frame)
{
    twai_ll_get_rx_buffer(hal_ctx->dev, rx_frame);
    twai_ll_set_cmd_release_rx_buffer(hal_ctx->dev);
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
