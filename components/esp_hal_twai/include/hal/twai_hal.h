/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/twai_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------- Defines and Typedefs --------------------------- */

typedef struct twai_hal_errata_ctx_t twai_hal_errata_ctx_t;

#if SOC_HAS(TWAI_FD)
typedef struct twaifd_dev_t *twai_soc_handle_t;
typedef union twaifd_frame_buffer_t twai_hal_frame_t;
#else
typedef struct twai_dev_t *twai_soc_handle_t;
typedef union twai_ll_frame_buffer_t twai_hal_frame_t;
#endif // SOC_HAS(TWAI_FD)

#define TWAI_HAL_SET_BITS(var, flag)            ((var) |= (flag))
#define TWAI_HAL_CLEAR_BITS(var, flag)          ((var) &= ~(flag))

//HAL state flags
#define TWAI_HAL_STATE_FLAG_RUNNING             (1 << 0)    //Controller is active (not in reset mode)
#define TWAI_HAL_STATE_FLAG_RECOVERING          (1 << 1)    //Bus is undergoing bus recovery
#define TWAI_HAL_STATE_FLAG_ERR_WARN            (1 << 2)    //TEC or REC is >= error warning limit
#define TWAI_HAL_STATE_FLAG_ERR_PASSIVE         (1 << 3)    //TEC or REC is >= 128
#define TWAI_HAL_STATE_FLAG_BUS_OFF             (1 << 4)    //Bus-off due to TEC >= 256
#define TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED    (1 << 5)    //Transmit buffer is occupied
#define TWAI_HAL_STATE_FLAG_TX_NEED_RETRY       (1 << 7)    //TX needs to be restarted due to errata workarounds

//Interrupt Events
#define TWAI_HAL_EVENT_BUS_OFF                  (1 << 0)
#define TWAI_HAL_EVENT_BUS_RECOV_CPLT           (1 << 1)
#define TWAI_HAL_EVENT_BUS_RECOV_PROGRESS       (1 << 2)
#define TWAI_HAL_EVENT_ERROR_WARNING            (1 << 3)
#define TWAI_HAL_EVENT_BELOW_EWL                (1 << 4)
#define TWAI_HAL_EVENT_ERROR_PASSIVE            (1 << 5)
#define TWAI_HAL_EVENT_ERROR_ACTIVE             (1 << 6)
#define TWAI_HAL_EVENT_BUS_ERR                  (1 << 7)
#define TWAI_HAL_EVENT_ARB_LOST                 (1 << 8)
#define TWAI_HAL_EVENT_RX_BUFF_FRAME            (1 << 9)
#define TWAI_HAL_EVENT_TX_BUFF_FREE             (1 << 10)
#define TWAI_HAL_EVENT_NEED_PERIPH_RESET        (1 << 11)
#define TWAI_HAL_EVENT_TX_SUCCESS               (1 << 12)

typedef struct {
    twai_soc_handle_t dev; // TWAI SOC layer handle (i.e. register base address)
    uint32_t state_flags;
    uint32_t clock_source_hz;
    uint32_t timer_overflow_cnt;
    twai_error_flags_t errors;
    uint8_t sja1000_filter_id_type;    // hardware don't check id type, check in software, 0:no_filter, 1: std_id_only, 2: ext_id_only
    int8_t retry_cnt;
    bool enable_self_test;
    bool enable_loopback;
    bool enable_listen_only;
    twai_hal_errata_ctx_t *errata_ctx;
} twai_hal_context_t;

/* ---------------------------- Init and Config ----------------------------- */

typedef struct {
    int controller_id;
    uint32_t clock_source_hz;
    uint32_t timer_freq;
    uint32_t intr_mask;
    int8_t retry_cnt;
    bool no_receive_rtr;
    bool enable_self_test;
    bool enable_loopback;
    bool enable_listen_only;
} twai_hal_config_t;

/**
 * @brief Get the memory requirement of hal_ctx and errata workaround context
 *
 * @return size_t
 */
size_t twai_hal_get_mem_requirment(void);

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
 * @brief Configure the TWAI peripheral for legacy driver (deprecated)
 *
 * @param hal_ctx Context of the HAL layer
 * @param t_config Pointer to timing configuration structure
 * @param f_config Pointer to filter configuration structure
 * @param clkout_divider Clock divider value for CLKOUT. Set to -1 to disable CLKOUT
 */
void twai_hal_configure(twai_hal_context_t *hal_ctx, const twai_timing_config_t *t_config, const twai_filter_config_t *f_config, uint32_t clkout_divider);

/**
 * @brief Check if the brp value valid for hardware register
 *
 * @param hal_ctx Context of the HAL layer
 * @param brp Bit rate prescaler value
 * @return true or False
 */
bool twai_hal_check_brp_validation(twai_hal_context_t *hal_ctx, uint32_t brp);

/**
 * @brief Configure the TWAI timing
 *
 * @param hal_ctx Context of the HAL layer
 * @param t_config Pointer to timing configuration structure
 */
void twai_hal_configure_timing(twai_hal_context_t *hal_ctx, const twai_timing_advanced_config_t *t_config);

/**
 * @brief Configure the TWAI timing of FD data
 *
 * @param hal_ctx Context of the HAL layer
 * @param t_config_fd Pointer to fd timing configuration structure
 */
void twai_hal_configure_timing_fd(twai_hal_context_t *hal_ctx, const twai_timing_advanced_config_t *t_config_fd);

/**
 * @brief Configure the TWAI mask filter
 *
 * @param hal_ctx Context of the HAL layer
 * @param filter_id Which filter to be configured
 * @param f_config Pointer to filter configuration structure
 */
void twai_hal_configure_mask_filter(twai_hal_context_t *hal_ctx, uint8_t filter_id, const twai_mask_filter_config_t *f_config);

/**
 * @brief Configure the TWAI range filter
 *
 * @param hal_ctx Context of the HAL layer
 * @param filter_id Which filter to be configured
 * @param f_config Pointer to filter configuration structure
 */
void twai_hal_configure_range_filter(twai_hal_context_t *hal_ctx, uint8_t filter_id, const twai_range_filter_config_t *f_config);

/**
 * @brief Check if the message in rx_frame match the filter config which save in hal_ctx
 *
 * @param hal_ctx Context of the HAL layer
 * @param rx_frame Where the message will be checked saved in
 * @return true: Pass the configured filter, false: don't pass
 */
bool twai_hal_soft_filter_check_msg(twai_hal_context_t *hal_ctx, twai_hal_frame_t *rx_frame);
/* -------------------------------- Actions --------------------------------- */

/**
 * @brief Start the TWAI peripheral
 *
 * Start the TWAI peripheral by configuring its operating mode, then exiting
 * reset mode so that the TWAI peripheral can participate in bus activities.
 *
 * @param hal_ctx Context of the HAL layer
 */
void twai_hal_start(twai_hal_context_t *hal_ctx);

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
 * @brief Start the TWAI timer with a start value
 *
 * @param hal_ctx Context of the HAL layer
 * @param preload_value Preload value for the timer
 */
void twai_hal_timer_start_with(twai_hal_context_t *hal_ctx, uint64_t preload_value);

/**
 * @brief Stop the TWAI timer
 */
void twai_hal_timer_stop(twai_hal_context_t *hal_ctx);

/**
 * @brief Start bus recovery
 *
 * @param hal_ctx Context of the HAL layer
 */
void twai_hal_start_bus_recovery(twai_hal_context_t *hal_ctx);

/**
 * @brief Get the value of the TX Error Counter
 *
 * @param hal_ctx Context of the HAL layer
 * @return TX Error Counter Value
 */
uint16_t twai_hal_get_tec(twai_hal_context_t *hal_ctx);

/**
 * @brief Get the value of the RX Error Counter
 *
 * @param hal_ctx Context of the HAL layer
 * @return RX Error Counter Value
 */
uint16_t twai_hal_get_rec(twai_hal_context_t *hal_ctx);

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
bool twai_hal_check_state_flags(twai_hal_context_t *hal_ctx, uint32_t check_flags);

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

/**
 * @brief Get the hardware error fsm state (active/passive/bus_off)
 *
 * @param hal_ctx Context of the HAL layer
 * @return twai_error_state
 */
twai_error_state_t twai_hal_get_err_state(twai_hal_context_t *hal_ctx);

/**
 * @brief Get hardware transmit error flags
 *
 * @param hal_ctx Context of the HAL layer
 * @return twai_error_flags_t
 */
__attribute__((always_inline))
static inline twai_error_flags_t twai_hal_get_err_flags(twai_hal_context_t *hal_ctx)
{
    return hal_ctx->errors;
}

/* ------------------------------- TX and RX -------------------------------- */

/**
 * @brief Get the RX message count register
 *
 * @param hal_ctx Context of the HAL layer
 * @return RX message count
 */
uint32_t twai_hal_get_rx_msg_count(twai_hal_context_t *hal_ctx);

/**
 * @brief TWAI hal transaction description type
 */
typedef struct {
    struct {
        twai_frame_header_t *header;    // message attribute/metadata, exclude data buffer
        uint8_t *buffer;                // buffer address for tx and rx message data
        size_t buffer_len;              // buffer length of provided data buffer pointer, in bytes.
    } frame;
    struct {
        int8_t retry_cnt;               // Number of retransmission attempts (-1 for unlimited)
        bool loopback;                  // loopback this message
    } config;                           // Hardware config for TX transaction
} twai_hal_trans_desc_t;

/**
 * @brief Format a TWAI Frame
 *
 * This function takes a TWAI message structure (containing ID, DLC, data, and
 * flags) and formats it to match the layout of the TX frame buffer.
 *
 * @param trans_desc Pointer to TWAI hal trans config
 * @param frame Pointer to empty frame structure
 */
void twai_hal_format_frame(const twai_hal_trans_desc_t *trans_desc, twai_hal_frame_t *frame);

/**
 * @brief Parse a TWAI Frame
 *
 * This function takes a TWAI frame (in the format of the RX frame buffer) and
 * parses it to a TWAI message (containing ID, DLC, data and flags).
 *
 * @param hal_ctx Context of the HAL layer
 * @param frame Pointer to frame structure
 * @param message Pointer to empty message structure
 */
void twai_hal_parse_frame(twai_hal_context_t *hal_ctx, twai_hal_frame_t *frame, twai_frame_header_t *header, uint8_t *buffer, uint8_t buffer_len);

/**
 * @brief Copy a frame into the TX buffer and transmit
 *
 * This function copies a formatted TX frame into the TX buffer, and the
 * transmit by setting the correct transmit command (e.g. normal, single shot,
 * self RX) in the command register.
 *
 * @param hal_ctx Context of the HAL layer
 * @param tx_frame Pointer to structure containing formatted TX frame
 * @param buffer_idx Hardware message buffer id to use
 */
void twai_hal_set_tx_buffer_and_transmit(twai_hal_context_t *hal_ctx, twai_hal_frame_t *tx_frame, uint8_t buffer_idx);

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
bool twai_hal_read_rx_fifo(twai_hal_context_t *hal_ctx, twai_hal_frame_t *rx_frame);

/**
 * @brief Clear the RX FIFO of overrun frames
 *
 * This function will clear the RX FIFO of overrun frames. The RX message count
 * will return to 0 after calling this function.
 *
 * @param hal_ctx Context of the HAL layer
 * @return Number of overrun messages cleared from RX FIFO
 */
uint32_t twai_hal_clear_rx_fifo_overrun(twai_hal_context_t *hal_ctx);

/* --------------------------- Errata Workarounds --------------------------- */

/**
 * @brief Check if hardware is busy (transmitting / receiving)
 *
 * @param hal_ctx Context of the HAL layer
 */
bool twai_hal_is_hw_busy(twai_hal_context_t *hal_ctx);

/**
 * @brief Backup the hardware configs, to prepare for hardware reset
 *
 * @param hal_ctx Context of the HAL layer
 */
void twai_hal_backup_config(twai_hal_context_t *hal_ctx);

/**
 * @brief Restore the hardware configs which backup before
 *
 * @param hal_ctx Context of the HAL layer
 */
void twai_hal_restore_config(twai_hal_context_t *hal_ctx);

/**
 * @brief Prepare the peripheral for a HW reset
 *
 * Some HW erratas will require the peripheral be reset. This function should be
 * called if twai_hal_get_events() returns the TWAI_HAL_EVENT_NEED_PERIPH_RESET event.
 * Preparing for a reset involves the following:
 * - Checking if a reset will cancel a TX. If so, mark that we need to retry that message after the reset
 * - Save how many RX messages were lost due to this reset
 * - Enter reset mode to stop any the peripheral from receiving any bus activity
 * - Store the register state of the peripheral
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
uint32_t twai_hal_get_reset_lost_rx_cnt(twai_hal_context_t *hal_ctx);

#ifdef __cplusplus
}
#endif
