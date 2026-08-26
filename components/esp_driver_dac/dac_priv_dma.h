/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DAC DMA event callback
 * @note  Invoked from the DMA ISR
 *
 * @return Whether a high-priority task has been woken up by this callback
 */
typedef bool (*dac_dma_event_callback_t)(void *ctx);

/**
 * @brief Group of DAC DMA event callbacks
 * @note  The callbacks run in ISR context
 * @note  When CONFIG_DAC_ISR_IRAM_SAFE is enabled, the callbacks and the functions they call
 *        must be placed in IRAM, and the variables they use must be in internal RAM
 */
typedef struct {
    dac_dma_event_callback_t on_done;   /*!< Invoked when one DMA descriptor is finished */
    dac_dma_event_callback_t on_teof;   /*!< Invoked when the DMA descriptor chain reaches total EOF */
} dac_dma_event_callbacks_t;

/**
 * @brief Initialize DAC DMA peripheral
 *
 * @param[in]   clk_src         DAC digital controller clock source
 * @param[in]   freq_hz         Requested DAC data frequency per channel
 * @param[in]   is_alternate    Transmit data alternate between two channels or simultaneously
 * @param[in]   cbs             Group of event callback functions, must not be NULL
 * @param[in]   ctx             Driver context passed to the callback functions
 * @return
 *      - ESP_OK                Initialize DAC DMA peripheral success
 *      - ESP_ERR_INVALID_ARG   Invalid clock source, frequency, or `cbs` is NULL
 *      - ESP_ERR_NOT_FOUND     The DMA peripheral has been occupied
 *      - ESP_ERR_NO_MEM        No memory for the DMA peripheral struct
 */
esp_err_t dac_priv_dma_init(soc_periph_dac_digi_clk_src_t clk_src, uint32_t freq_hz, bool is_alternate,
                            const dac_dma_event_callbacks_t *cbs, void *ctx);

/**
 * @brief Deinitialize DAC DMA peripheral
 *
 * @return
 *      - ESP_OK                Deinitialize DAC DMA peripheral success
 *      - Others                Failed to release interrupt, clock, or DMA peripheral
 */
esp_err_t dac_priv_dma_deinit(void);

/**
 * @brief Enable the DMA and interrupt of the DAC DMA peripheral
 *
 */
void dac_priv_dma_enable(void);

/**
 * @brief Disable the DMA and interrupt of the DAC DMA peripheral
 *
 */
void dac_priv_dma_disable(void);

/**
 * @brief Start a DMA transaction
 * @note  DMA transaction will stop when reaches the tail of the descriptor link
 *
 * @param[in]   desc_addr   Descriptor address
 */
void dac_priv_dma_trans_start(uintptr_t desc_addr);

/**
 * @brief Stop the current DMA transaction immediately
 */
void dac_priv_dma_trans_stop(void);

#if !SOC_IS(ESP32)
/**
 * @brief Append the newly linked DMA descriptors to the current transaction
 * @note  The caller should link new descriptors to the current tail before calling this function.
 */
void dac_priv_dma_trans_append(void);
#endif

#ifdef __cplusplus
}
#endif
