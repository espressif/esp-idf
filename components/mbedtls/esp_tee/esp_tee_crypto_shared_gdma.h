/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "hal/gdma_types.h"
#include "esp_crypto_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start a GDMA transfer on the shared crypto DMA channel for ESP-TEE
 *
 * @note This is a low-level (LL), non-OS version of esp_crypto_shared_gdma_start()
 *
 * @param input     Input linked list descriptor (crypto_dma_desc_t *)
 * @param output    Output linked list descriptor (crypto_dma_desc_t *)
 * @param periph    Crypto peripheral to connect the DMA to, either CRYPTO_DMA_USER_AES or CRYPTO_DMA_USER_SHA
 * @return esp_err_t ESP_OK on success, ESP_ERR_INVALID_ARG if invalid peripheral specified
 */
esp_err_t esp_tee_crypto_shared_gdma_start(const crypto_dma_desc_t *input, const crypto_dma_desc_t *output, crypto_dma_user_t periph);

/**
 * @brief Frees the TEE-specific shared crypto DMA channel.
 *
 * @note This is a low-level (LL), non-OS version of esp_crypto_shared_gdma_free()
 */
void esp_tee_crypto_shared_gdma_free(void);

#ifdef __cplusplus
}
#endif
