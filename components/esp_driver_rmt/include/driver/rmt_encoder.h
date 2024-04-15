/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "hal/rmt_types.h"
#include "driver/rmt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond */
typedef struct rmt_encoder_t rmt_encoder_t;
/** @endcond */

/**
 * @brief RMT encoding state
 */
typedef enum {
    RMT_ENCODING_RESET = 0,           /*!< The encoding session is in reset state */
    RMT_ENCODING_COMPLETE = (1 << 0), /*!< The encoding session is finished, the caller can continue with subsequent encoding */
    RMT_ENCODING_MEM_FULL = (1 << 1), /*!< The encoding artifact memory is full, the caller should return from current encoding session */
} rmt_encode_state_t;

/**
 * @brief Interface of RMT encoder
 */
struct rmt_encoder_t {
    /**
     * @brief Encode the user data into RMT symbols and write into RMT memory
     *
     * @note The encoding function will also be called from an ISR context, thus the function must not call any blocking API.
     * @note It's recommended to put this function implementation in the IRAM, to achieve a high performance and less interrupt latency.
     *
     * @param[in] encoder Encoder handle
     * @param[in] tx_channel RMT TX channel handle, returned from `rmt_new_tx_channel()`
     * @param[in] primary_data App data to be encoded into RMT symbols
     * @param[in] data_size Size of primary_data, in bytes
     * @param[out] ret_state Returned current encoder's state
     * @return Number of RMT symbols that the primary data has been encoded into
     */
    size_t (*encode)(rmt_encoder_t *encoder, rmt_channel_handle_t tx_channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state);

    /**
     * @brief Reset encoding state
     *
     * @param[in] encoder Encoder handle
     * @return
     *      - ESP_OK: reset encoder successfully
     *      - ESP_FAIL: reset encoder failed
     */
    esp_err_t (*reset)(rmt_encoder_t *encoder);

    /**
     * @brief Delete encoder object
     *
     * @param[in] encoder Encoder handle
     * @return
     *      - ESP_OK: delete encoder successfully
     *      - ESP_FAIL: delete encoder failed
     */
    esp_err_t (*del)(rmt_encoder_t *encoder);
};

/**
 * @brief Callback for simple callback encoder
 *
 * This will get called to encode the data stream of given length (as passed to
 * rmt_transmit by the user) into symbols to be sent by the hardware.
 *
 * The callback will be initially called with symbol_pos=0. If the callback
 * encodes N symbols and finishes, the next callback will always be with
 * symbols_written=N. If the callback then encodes M symbols, the next callback
 * will always be with symbol_pos=N+M, etc. The only exception is when the
 * encoder is reset (e.g. to begin a new transaction) in which case
 * symbol_pos will always restart at 0.
 *
 * If the amount of free space in the symbol buffer (as indicated by
 * symbols_free) is too low, the function can return 0 as result and
 * the RMT will call the function again once there is more space available.
 * Note that the callback should eventually return non-0 if called with
 * free space of rmt_simple_encoder_config_t::min_chunk_size or more. It
 * is acceptable to return 0 for a given free space N, then on the next
 * call (possibly with a larger free buffer space) return less or more
 * than N symbols.
 *
 * When the transaction is done (all data_size data is encoded), the callback
 * can indicate this by setting *done to true. This can either happen on the
 * last callback call that returns an amount of symbols encoded, or on a
 * callback that returns zero. In either case, the callback will not be
 * called again for this transaction.
 *
 * @param[in] data Data pointer, as passed to rmt_transmit()
 * @param[in] data_size Size of the data, as passed to rmt_transmit()
 * @param[in] symbols_written Current position in encoded stream, in symbols
 * @param[in] symbols_free The maximum amount of symbols that can be written into the `symbols` buffer
 * @param[out] symbols Symbols to be sent to the RMT hardware
 * @param[out] done Setting this to true marks this transaction as finished
 * @param arg Opaque argument
 * @return Amount of symbols encoded in this callback round. 0 if more space is needed.
 */
typedef size_t (*rmt_encode_simple_cb_t)(const void *data, size_t data_size,
                                         size_t symbols_written, size_t symbols_free,
                                         rmt_symbol_word_t *symbols, bool *done, void *arg);

/**
 * @brief Bytes encoder configuration
 */
typedef struct {
    rmt_symbol_word_t bit0; /*!< How to represent BIT0 in RMT symbol */
    rmt_symbol_word_t bit1; /*!< How to represent BIT1 in RMT symbol */
    struct {
        uint32_t msb_first: 1; /*!< Whether to encode MSB bit first */
    } flags;                   /*!< Encoder config flag */
} rmt_bytes_encoder_config_t;

/**
 * @brief Copy encoder configuration
 */
typedef struct {
} rmt_copy_encoder_config_t;

/**
 * @brief Simple callback encoder configuration
 */
typedef struct {
    rmt_encode_simple_cb_t callback;  /*!< Callback to call for encoding data into RMT items */
    void *arg;                        /*!< Opaque user-supplied argument for callback */
    size_t min_chunk_size;            /*!< Minimum amount of free space, in RMT symbols, the
                                           encoder needs in order to guarantee it always
                                           returns non-zero. Defaults
                                           to 64 if zero / not given. */
} rmt_simple_encoder_config_t;

/**
 * @brief Create RMT bytes encoder, which can encode byte stream into RMT symbols
 *
 * @param[in] config Bytes encoder configuration
 * @param[out] ret_encoder Returned encoder handle
 * @return
 *      - ESP_OK: Create RMT bytes encoder successfully
 *      - ESP_ERR_INVALID_ARG: Create RMT bytes encoder failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create RMT bytes encoder failed because out of memory
 *      - ESP_FAIL: Create RMT bytes encoder failed because of other error
 */
esp_err_t rmt_new_bytes_encoder(const rmt_bytes_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);

/**
 * @brief Update the configuration of the bytes encoder
 *
 * @note The configurations of the bytes encoder is also set up by `rmt_new_bytes_encoder()`.
 *       This function is used to update the configuration of the bytes encoder at runtime.
 *
 * @param[in] bytes_encoder Bytes encoder handle, created by e.g `rmt_new_bytes_encoder()`
 * @param[in] config Bytes encoder configuration
 * @return
 *      - ESP_OK: Update RMT bytes encoder successfully
 *      - ESP_ERR_INVALID_ARG: Update RMT bytes encoder failed because of invalid argument
 *      - ESP_FAIL: Update RMT bytes encoder failed because of other error
 */
esp_err_t rmt_bytes_encoder_update_config(rmt_encoder_handle_t bytes_encoder, const rmt_bytes_encoder_config_t *config);

/**
 * @brief Create RMT copy encoder, which copies the given RMT symbols into RMT memory
 *
 * @param[in] config Copy encoder configuration
 * @param[out] ret_encoder Returned encoder handle
 * @return
 *      - ESP_OK: Create RMT copy encoder successfully
 *      - ESP_ERR_INVALID_ARG: Create RMT copy encoder failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create RMT copy encoder failed because out of memory
 *      - ESP_FAIL: Create RMT copy encoder failed because of other error
 */
esp_err_t rmt_new_copy_encoder(const rmt_copy_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);

/**
 * @brief Create RMT simple callback encoder, which uses a callback to convert incoming
 *        data into RMT symbols.
 *
 * @param[in] config Simple callback encoder configuration
 * @param[out] ret_encoder Returned encoder handle
 * @return
 *      - ESP_OK: Create RMT simple callback encoder successfully
 *      - ESP_ERR_INVALID_ARG: Create RMT simple callback encoder failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create RMT simple callback encoder failed because out of memory
 *      - ESP_FAIL: Create RMT simple callback encoder failed because of other error
 */
esp_err_t rmt_new_simple_encoder(const rmt_simple_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);

/**
 * @brief Delete RMT encoder
 *
 * @param[in] encoder RMT encoder handle, created by e.g `rmt_new_bytes_encoder()`
 * @return
 *      - ESP_OK: Delete RMT encoder successfully
 *      - ESP_ERR_INVALID_ARG: Delete RMT encoder failed because of invalid argument
 *      - ESP_FAIL: Delete RMT encoder failed because of other error
 */
esp_err_t rmt_del_encoder(rmt_encoder_handle_t encoder);

/**
 * @brief Reset RMT encoder
 *
 * @param[in] encoder RMT encoder handle, created by e.g `rmt_new_bytes_encoder()`
 * @return
 *      - ESP_OK: Reset RMT encoder successfully
 *      - ESP_ERR_INVALID_ARG: Reset RMT encoder failed because of invalid argument
 *      - ESP_FAIL: Reset RMT encoder failed because of other error
 */
esp_err_t rmt_encoder_reset(rmt_encoder_handle_t encoder);

/**
 * @brief A helper function to allocate a proper memory for RMT encoder
 *
 * @param size Size of memory to be allocated
 * @return Pointer to the allocated memory if the allocation is successful, NULL otherwise
 */
void* rmt_alloc_encoder_mem(size_t size);

#ifdef __cplusplus
}
#endif
