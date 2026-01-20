/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_private/dma2d.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the 2D-DMA module to perform memcopy operations
 */
esp_err_t dma2d_m2m_init(void);

/**
 * @brief De-initialize the 2D-DMA module
 */
esp_err_t dma2d_m2m_deinit(void);

/**
 * @brief Callback function when a memcopy operation is done
 *
 * @param user_data User registered data from `dma2d_m2m_trans_config_t`
 *
 * @return Whether a task switch is needed after the callback function returns,
 *         this is usually due to the callback wakes up some high priority task.
 */
typedef bool (*dma2d_m2m_trans_eof_callback_t)(void *user_data);

/**
 * @brief A collection of configuration items for perferming a memcopy operation with 2D-DMA
 */
typedef struct {
    intptr_t tx_desc_base_addr;                             /*!< 2D-DMA TX descriptor address */
    intptr_t rx_desc_base_addr;                             /*!< 2D-DMA RX descriptor address */
    dma2d_m2m_trans_eof_callback_t trans_eof_cb;            /*!< Callback function to be called when the memcopy operation is done */
    void *user_data;                                        /*!< User registered data to be passed into `trans_eof_cb` callback */
    dma2d_transfer_ability_t *transfer_ability_config;      /*!< Pointer to a collection of 2D-DMA transfer ability configuration */
    dma2d_strategy_config_t *tx_strategy_config;            /*!< Pointer to a collection of 2D-DMA TX strategy configuration */
    dma2d_strategy_config_t *rx_strategy_config;            /*!< Pointer to a collection of 2D-DMA RX strategy configuration */
    dma2d_csc_config_t *tx_csc_config;                      /*!< Pointer to a collection of 2D-DMA TX color space conversion configuration */
    dma2d_csc_config_t *rx_csc_config;                      /*!< Pointer to a collection of 2D-DMA RX color space conversion configuration */
} dma2d_m2m_trans_config_t;

/**
 * @brief Do a memcopy operation with 2D-DMA module
 *
 * @param trans_config Pointer to a collection of configurations for the memcopy operation
 * @return
 *      - ESP_OK: Enqueue the transaction to 2D-DMA pool successfully
 *      - ESP_ERR_INVALID_ARG: Enqueue the transaction to 2D-DMA pool failed because of invalid argument
 */
esp_err_t dma2d_m2m(const dma2d_m2m_trans_config_t *trans_config);

#ifdef __cplusplus
}
#endif
