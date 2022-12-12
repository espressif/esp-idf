/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_intr_alloc.h"


#ifdef __cplusplus
extern "C" {
#endif

#define DAC_DMA_EOF_INTR        0x01
#define DAC_DMA_TEOF_INTR       0x02

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
 * @brief Whether the TX_EOF interrupt is triggered
 *
 * @return
 *      - uint32_t  Mask of the triggered interrupt: DAC_DMA_EOF_INTR, DAC_DMA_EOF_INTR
 */
uint32_t dac_dma_periph_intr_is_triggered(void);

/**
 * @brief Get the descriptor that just finished sending data
 *
 * @return
 *      - uint32_t  The address of the EOF descriptor
 */
uint32_t dac_dma_periph_intr_get_eof_desc(void);

/**
 * @brief Start a DMA transaction
 * @note  DMA transaction will stop when reaches the tail of the descriptor link
 *
 * @param[in]   desc_addr   Descriptor address
 */
void dac_dma_periph_dma_trans_start(uint32_t desc_addr);

#ifdef __cplusplus
}
#endif
