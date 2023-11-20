/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/lldesc.h"
#include "soc/soc_caps.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start the DMA engine
 *
 * @param input AES input descriptor (outlink)
 * @param output AES output descriptor (inlink)
 * @return
 *      - ESP_OK: Successfully started the DMA
 *      - ESP_ERR_INVALID_STATE: No DMA channel available
 */
esp_err_t esp_aes_dma_start(const lldesc_t *input, const lldesc_t *output);

/**
 * @brief Check if the DMA engine is finished reading the result
 *
 * @param output AES output descriptor (inlink)
 * @return
 *      - true: DMA finished
 *      - false: DMA not yet finished
 */
bool esp_aes_dma_done(const lldesc_t *output);

/**
 * @brief Allocate AES peripheral interrupt handler
 */
void esp_aes_intr_alloc(void);

#ifdef __cplusplus
}
#endif
