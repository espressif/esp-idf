/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_intr_alloc.h"
#include "hal/dma_types.h"
#include "hal/adc_hal.h"
#include "../adc_continuous_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize DMA interrupt event
 *
 * @param[in] adc_ctx ADC continuous mode driver handle
 *
 * @return
 *      - ESP_OK:                On success
 */
esp_err_t adc_dma_intr_event_init(adc_continuous_ctx_t *adc_ctx);

/**
 * @brief Initialize DMA on ADC continuous mode
 *
 * @param[in] adc_dma ADC DMA context
 *
 * @return
 *      - ESP_OK:                On success
 */
esp_err_t adc_dma_init(adc_dma_t *adc_dma);

/**
 * @brief Deinitialize DMA on ADC continuous mode
 *
 * @param[in] adc_dma ADC DMA context
 *
 * @return
 *      - ESP_OK:                On success
 */
esp_err_t adc_dma_deinit(adc_dma_t adc_dma);

/**
 * @brief Start DMA on ADC continuous mode
 *
 * @param[in] adc_dma ADC DMA context
 * @param[in] addr    ADC DMA descriptor
 *
 * @return
 *      - ESP_OK:                On success
 */
esp_err_t adc_dma_start(adc_dma_t adc_dma, dma_descriptor_t *addr);

/**
 * @brief Stop DMA on ADC continuous mode
 *
 * @param[in] adc_dma ADC DMA context
 *
 * @return
 *      - ESP_OK:                On success
 */
esp_err_t adc_dma_stop(adc_dma_t adc_dma);

/**
 * @brief Reset DMA on ADC continuous mode
 *
 * @param[in] adc_dma ADC DMA context
 *
 * @return
 *      - ESP_OK:                On success
 */
esp_err_t adc_dma_reset(adc_dma_t adc_dma);

#ifdef __cplusplus
}
#endif
