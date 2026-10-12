/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "aes/esp_aes.h"
#include "soc/soc_caps.h"
#include "hal/aes_hal.h"
#include "esp_crypto_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

bool valid_key_length(const esp_aes_context *ctx);

#ifdef CONFIG_MBEDTLS_AES_USE_PSEUDO_ROUND_FUNC
/**
 * @brief Default pseudo rounds configs of the AES accelerator
 */
typedef enum {
    ESP_AES_PSEUDO_ROUNDS_DISABLE = 0,
    ESP_AES_PSEUDO_ROUNDS_LOW,
    ESP_AES_PSEUDO_ROUNDS_MEDIUM,
    ESP_AES_PSEUDO_ROUNDS_HIGH,
} esp_aes_psuedo_rounds_state_t;

void esp_aes_enable_pseudo_rounds(esp_aes_psuedo_rounds_state_t state);
#endif /* CONFIG_MBEDTLS_AES_USE_PSEUDO_ROUND_FUNC */

#if SOC_AES_SUPPORT_DMA
/**
 * @brief           Run a AES operation using DMA
 *
 * @param ctx           Aes context
 * @param input         Pointer to input data
 * @param output        Pointer to output data
 * @param len           Length of the input data
 * @param stream_out    The saved stream-block for resuming next operation.
 * @return int          -1 on error
 */
int esp_aes_process_dma(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, uint8_t *stream_out);
#endif

#if SOC_AES_SUPPORT_GCM
/**
 * @brief           Run a AES-GCM conversion using DMA
 *
 * @param ctx       Aes context
 * @param input     Pointer to input data
 * @param output    Pointer to output data
 * @param len       Length of the input data
 * @param aad_desc  GCM additional data DMA descriptor
 * @param aad_len   GCM additional data length
 * @return int      -1 on error
 */
int esp_aes_process_dma_gcm(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, const unsigned char *aad_desc, size_t aad_len);

#if CONFIG_MBEDTLS_HARDWARE_GCM
/**
 * @brief           Check whether esp_aes_process_dma_gcm() can operate on a given buffer
 *
 * @note            AES-GCM runs as a single DMA transaction, so a buffer that the DMA cannot
 *                  reach, or that this port cannot align, has no chunked fallback. Such buffers
 *                  are rejected here and the caller uses the software GHASH path instead.
 *
 * @param p         Buffer to check
 * @param len       Length of the buffer
 * @param is_output true for the buffer the DMA writes into, which may carry extra alignment rules
 * @return bool     true if the hardware GCM path can use this buffer
 */
bool esp_aes_gcm_buffer_dma_capable(const void *p, size_t len, bool is_output);
#endif
#endif

#ifdef __cplusplus
}
#endif
