/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "esp_crypto_dma.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start the DMA engine
 *
 * @param input SHA input descriptor (outlink)
 * @return
 *      - ESP_OK: Successfully started the DMA
 *      - ESP_ERR_INVALID_STATE: No DMA channel available
 */
esp_err_t esp_sha_dma_start(const crypto_dma_desc_t *input);


#ifdef __cplusplus
}
#endif
