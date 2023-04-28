/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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

/**
 * @brief Type of RMT encoder
 */
typedef struct rmt_encoder_t rmt_encoder_t;

/**
 * @brief RMT encoding state
 */
typedef enum {
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

#ifdef __cplusplus
}
#endif
