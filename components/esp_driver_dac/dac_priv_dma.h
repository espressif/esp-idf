/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "soc/soc_caps.h"
#include "esp_bit_defs.h"
#include "esp_intr_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

// one node in the descriptor chain is finished
#define DAC_DMA_DONE_INTR       BIT(0)
// all nodes in the descriptor chain are finished
#define DAC_DMA_TEOF_INTR       BIT(1)

/**
 * @brief Initialize DAC DMA peripheral
 *
 * @param[in]   freq_hz         DAC data frequency per channel
 * @param[in]   is_alternate    Transmit data alternate between two channels or simultaneously
 * @param[in]   is_apll         Whether use APLL as DAC digital controller clock source
 * @return
 *      - ESP_ERR_NOT_FOUND     The DMA peripheral has been occupied
 *      - ESP_ERR_NO_MEM        No memory for the DMA peripheral struct
 *      - ESP_ERR_INVALID_ARG   The frequency is out of range
 *      - ESP_OK                Initialize DAC DMA peripheral success
 */
esp_err_t dac_dma_periph_init(uint32_t freq_hz, bool is_alternate, bool is_apll);

/**
 * @brief Deinitialize DAC DMA peripheral
 *
 * @return
 *      - ESP_ERR_INVALID_STATE The DAC DMA has been de-initialized already
 *                              or the interrupt has not been de-registered
 *      - ESP_OK                Deinitialize DAC DMA peripheral success
 */
esp_err_t dac_dma_periph_deinit(void);

/**
 * @brief Get the DMA interrupt signal id
 *
 * @return
 *      - int       DMA interrupt signal id
 */
int dac_dma_periph_get_intr_signal(void);

/**
 * @brief Enable the DMA and interrupt of the DAC DMA peripheral
 *
 */
void dac_dma_periph_enable(void);

/**
 * @brief Disable the DMA and interrupt of the DAC DMA peripheral
 *
 */
void dac_dma_periph_disable(void);

/**
 * @brief Get the mask of the triggered interrupt
 *
 * @return
 *      - uint32_t  Mask of the triggered interrupt: DAC_DMA_DONE_INTR, DAC_DMA_TEOF_INTR
 */
uint32_t dac_dma_periph_intr_get_mask(void);

/**
 * @brief Start a DMA transaction
 * @note  DMA transaction will stop when reaches the tail of the descriptor link
 *
 * @param[in]   desc_addr   Descriptor address
 */
void dac_dma_periph_trans_start(uintptr_t desc_addr);

/**
 * @brief Stop the current DMA transaction immediately
 */
void dac_dma_periph_trans_stop(void);

#if !SOC_IS(ESP32)
/**
 * @brief Append the newly linked DMA descriptors to the current transaction
 * @note  The caller should link new descriptors to the current tail before calling this function.
 */
void dac_dma_periph_trans_append(void);
#endif

#ifdef __cplusplus
}
#endif
