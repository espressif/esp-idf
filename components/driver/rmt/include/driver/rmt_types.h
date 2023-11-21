/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal/rmt_types.h"
#include "hal/gpio_types.h" // for gpio_num_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of RMT channel handle
 */
typedef struct rmt_channel_t *rmt_channel_handle_t;

/**
 * @brief Type of RMT synchronization manager handle
 */
typedef struct rmt_sync_manager_t *rmt_sync_manager_handle_t;

/**
 * @brief Type of RMT encoder handle
 */
typedef struct rmt_encoder_t *rmt_encoder_handle_t;

/**
 * @brief Type of RMT TX done event data
 */
typedef struct {
    size_t num_symbols; /*!< The number of transmitted RMT symbols, including one EOF symbol, which is appended by the driver to mark the end of a transmission.
                             For a loop transmission, this value only counts for one round. */
} rmt_tx_done_event_data_t;

/**
 * @brief Prototype of RMT event callback
 * @param[in] tx_chan RMT channel handle, created from `rmt_new_tx_channel()`
 * @param[in] edata Point to RMT event data. The lifecycle of this pointer memory is inside this function,
 *                  user should copy it into static memory if used outside this function.
 * @param[in] user_ctx User registered context, passed from `rmt_tx_register_event_callbacks()`
 *
 * @return Whether a high priority task has been waken up by this callback function
 */
typedef bool (*rmt_tx_done_callback_t)(rmt_channel_handle_t tx_chan, const rmt_tx_done_event_data_t *edata, void *user_ctx);

/**
 * @brief Type of RMT RX done event data
 */
typedef struct {
    rmt_symbol_word_t *received_symbols; /*!< Point to the received RMT symbols */
    size_t num_symbols;                  /*!< The number of received RMT symbols */
    struct {
        uint32_t is_last: 1; /*!< Indicating if the current received data are the last part of the transaction */
    } flags;                 /*!< Extra flags */
} rmt_rx_done_event_data_t;

/**
 * @brief Prototype of RMT event callback
 *
 * @param[in] rx_chan RMT channel handle, created from `rmt_new_rx_channel()`
 * @param[in] edata Point to RMT event data. The lifecycle of this pointer memory is inside this function,
 *                  user should copy it into static memory if used outside this function.
 * @param[in] user_ctx User registered context, passed from `rmt_rx_register_event_callbacks()`
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*rmt_rx_done_callback_t)(rmt_channel_handle_t rx_chan, const rmt_rx_done_event_data_t *edata, void *user_ctx);

#ifdef __cplusplus
}
#endif
