/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "hal/twai_types.h"
#include "hal/twai_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------- Defines and Typedefs --------------------------- */

#define TWAI_HAL_SET_BITS(var, flag)            ((var) |= (flag))
#define TWAI_HAL_CLEAR_BITS(var, flag)          ((var) &= ~(flag))

//HAL state flags
#define TWAI_HAL_STATE_FLAG_RUNNING             (1 << 0)    //Controller is active (not in reset mode)
#define TWAI_HAL_STATE_FLAG_RECOVERING          (1 << 1)    //Bus is undergoing bus recovery
#define TWAI_HAL_STATE_FLAG_ERR_WARN            (1 << 2)    //TEC or REC is >= error warning limit
#define TWAI_HAL_STATE_FLAG_ERR_PASSIVE         (1 << 3)    //TEC or REC is >= 128
#define TWAI_HAL_STATE_FLAG_BUS_OFF             (1 << 4)    //Bus-off due to TEC >= 256
#define TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED    (1 << 5)    //Transmit buffer is occupied
#if defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)
#define TWAI_HAL_STATE_FLAG_TX_NEED_RETRY       (1 << 7)    //TX needs to be restarted due to errata workarounds
#endif

//Interrupt Events
#define TWAI_HAL_EVENT_BUS_OFF                  (1 << 0)
#define TWAI_HAL_EVENT_BUS_RECOV_CPLT           (1 << 1)
#define TWAI_HAL_EVENT_BUS_RECOV_PROGRESS       (1 << 2)
#define TWAI_HAL_EVENT_ABOVE_EWL                (1 << 3)
#define TWAI_HAL_EVENT_BELOW_EWL                (1 << 4)
#define TWAI_HAL_EVENT_ERROR_PASSIVE            (1 << 5)
#define TWAI_HAL_EVENT_ERROR_ACTIVE             (1 << 6)
#define TWAI_HAL_EVENT_BUS_ERR                  (1 << 7)
#define TWAI_HAL_EVENT_ARB_LOST                 (1 << 8)
#define TWAI_HAL_EVENT_RX_BUFF_FRAME            (1 << 9)
#define TWAI_HAL_EVENT_TX_BUFF_FREE             (1 << 10)
#if defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)
#define TWAI_HAL_EVENT_NEED_PERIPH_RESET        (1 << 11)
#endif

typedef twai_ll_frame_buffer_t twai_hal_frame_t;

typedef struct {
    twai_dev_t *dev;
    uint32_t state_flags;
    uint32_t clock_source_hz;
#if defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)
    twai_hal_frame_t tx_frame_save;
    twai_ll_reg_save_t reg_save;
    uint8_t rx_msg_cnt_save;
#endif
} twai_hal_context_t;

/* ---------------------------- Init and Config ----------------------------- */

typedef struct {
    int controller_id;
    uint32_t clock_source_hz;
} twai_hal_config_t;

/**
 * @brief Initialize TWAI peripheral and HAL context
 *
 * Sets HAL context, puts TWAI peripheral into reset mode, then sets some
 * registers with default values.
 *
 * @param hal_ctx Context of the HAL layer
 * @param config HAL driver configuration
 * @return True if successfully initialized, false otherwise.
 */
bool twai_hal_init(twai_hal_context_t *hal_ctx, const twai_hal_config_t *config);

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
 */
void twai_hal_start(twai_hal_context_t *hal_ctx, twai_mode_t mode);

/**
 * @brief Stop the TWAI peripheral
 *
 * Stop the TWAI peripheral by entering reset mode to stop any bus activity, then
 * setting the operating mode to Listen Only so that REC is frozen.
 *
 * @param hal_ctx Context of the HAL layer
 */
void twai_hal_stop(twai_hal_context_t *hal_ctx);

/**
 * @brief Start bus recovery
 *
 * @param hal_ctx Context of the HAL layer
 */
static inline void twai_hal_start_bus_recovery(twai_hal_context_t *hal_ctx)
{
    TWAI_HAL_CLEAR_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED);
    TWAI_HAL_SET_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_RECOVERING);
    twai_ll_exit_reset_mode(hal_ctx->dev);
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
__attribute__((always_inline))
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
__attribute__((always_inline))
static inline bool twai_hal_check_last_tx_successful(twai_hal_context_t *hal_ctx)
{
    return twai_ll_is_last_tx_successful((hal_ctx)->dev);
}

/**
 * @brief Check if certain HAL state flags are set
 *
 * The HAL will maintain a record of the controller's state via a set of flags.
 * These flags are automatically maintained (i.e., set and reset) inside various
 * HAL function calls. This function checks if certain flags are currently set.
 *
 * @param hal_ctx Context of the HAL layer
 * @param check_flags Bit mask of flags to check
 * @return True if one or more of the flags in check_flags are set
 */
__attribute__((always_inline))
static inline bool twai_hal_check_state_flags(twai_hal_context_t *hal_ctx, uint32_t check_flags)
{
    return hal_ctx->state_flags & check_flags;
}

/* ----------------------------- Event Handling ----------------------------- */

/**
 * @brief Get a bit mask of the events that triggered that triggered an interrupt
 *
 * This function should be called at the beginning of an interrupt. This function will do the following:
 * - Read and clear interrupt register
 * - Calculate what events have triggered the interrupt
 * - Respond to low latency interrupt events
 *      - Bus off: Change to LOM to freeze TEC/REC. Errata 1 Fix
 *      - Recovery complete: Enter reset mode
 *      - Clear ECC and ALC so that their interrupts are re-armed
 * - Update HAL state flags based on interrupts that have occurred.
 * - For the ESP32, check for errata conditions. If a HW reset is required, this function
 *   will set the TWAI_HAL_EVENT_NEED_PERIPH_RESET event.
 *
 * @param hal_ctx Context of the HAL layer
 * @return Bit mask of events that have occurred
 */
uint32_t twai_hal_get_events(twai_hal_context_t *hal_ctx);

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
    twai_ll_parse_frame_buffer(frame, &message->identifier, &message->data_length_code,
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
 * that it loads the next frame in the RX FIFO). False is returned under the
 * following conditions:
 * - On the ESP32S2, false is returned if the RX buffer points to an overrun frame
 * - On the ESP32, false is returned if the RX buffer points to the first overrun
 * frame in the RX FIFO
 *
 * @param hal_ctx Context of the HAL layer
 * @param rx_frame Pointer to structure to store RX frame
 * @return True if a valid frame was copied and released. False if overrun.
 */
__attribute__((always_inline))
static inline bool twai_hal_read_rx_buffer_and_clear(twai_hal_context_t *hal_ctx, twai_hal_frame_t *rx_frame)
{
#ifdef SOC_TWAI_SUPPORTS_RX_STATUS
    if (twai_ll_get_status(hal_ctx->dev) & TWAI_LL_STATUS_MS) {
        //Release the buffer for this particular overrun frame
        twai_ll_set_cmd_release_rx_buffer(hal_ctx->dev);
        return false;
    }
#else
    if (twai_ll_get_status(hal_ctx->dev) & TWAI_LL_STATUS_DOS) {
        //No need to release RX buffer as we'll be releaseing all RX frames in continuously later
        return false;
    }
#endif
    twai_ll_get_rx_buffer(hal_ctx->dev, rx_frame);
    twai_ll_set_cmd_release_rx_buffer(hal_ctx->dev);
    return true;
}

#ifndef SOC_TWAI_SUPPORTS_RX_STATUS
/**
 * @brief Clear the RX FIFO of overrun frames
 *
 * This function will clear the RX FIFO of overrun frames. The RX message count
 * will return to 0 after calling this function.
 *
 * @param hal_ctx Context of the HAL layer
 * @return Number of overrun messages cleared from RX FIFO
 */
__attribute__((always_inline))
static inline uint32_t twai_hal_clear_rx_fifo_overrun(twai_hal_context_t *hal_ctx)
{
    uint32_t msg_cnt = 0;
    //Note: Need to keep polling th rx message counter incase another message arrives whilst clearing
    while (twai_ll_get_rx_msg_count(hal_ctx->dev) > 0) {
        twai_ll_set_cmd_release_rx_buffer(hal_ctx->dev);
        msg_cnt++;
    }
    //Set a clear data overrun command to clear the data overrun status bit
    twai_ll_set_cmd_clear_data_overrun(hal_ctx->dev);

    return msg_cnt;
}
#endif  //SOC_TWAI_SUPPORTS_RX_STATUS

/* --------------------------- Errata Workarounds --------------------------- */

#if defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)
/**
 * @brief Prepare the peripheral for a HW reset
 *
 * Some HW erratas will require the peripheral be reset. This function should be
 * called if twai_hal_get_events() returns the TWAI_HAL_EVENT_NEED_PERIPH_RESET event.
 * Preparing for a reset involves the following:
 * - Checking if a reset will cancel a TX. If so, mark that we need to retry that message after the reset
 * - Save how many RX messages were lost due to this reset
 * - Enter reset mode to stop any the peripheral from receiving any bus activity
 * - Store the regsiter state of the peripheral
 *
 * @param hal_ctx Context of the HAL layer
 */
void twai_hal_prepare_for_reset(twai_hal_context_t *hal_ctx);

/**
 * @brief Recover the peripheral after a HW reset
 *
 * This should be called after calling twai_hal_prepare_for_reset() and then
 * executing the HW reset.
 * Recovering the peripheral from a HW reset involves the following:
 * - Restoring the previously saved register state
 * - Exiting reset mode to allow receiving of bus activity
 * - Retrying any TX message that was cancelled by the HW reset
 *
 * @param hal_ctx Context of the HAL layer
 */
void twai_hal_recover_from_reset(twai_hal_context_t *hal_ctx);

/**
 * @brief Get how many RX messages were lost due to HW reset
 *
 * @note The number of lost RX messages are saved during twai_hal_prepare_for_reset()
 *
 * @param hal_ctx Context of the HAL layer
 * @return uint32_t Number of RX messages lost due to HW reset
 */
__attribute__((always_inline))
static inline uint32_t twai_hal_get_reset_lost_rx_cnt(twai_hal_context_t *hal_ctx)
{
    return hal_ctx->rx_msg_cnt_save;
}
#endif  //defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)

#ifdef __cplusplus
}
#endif
