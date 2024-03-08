/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "aes/esp_aes.h"
#include "soc/soc_caps.h"
#include "esp_crypto_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

bool valid_key_length(const esp_aes_context *ctx);

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
#endif

#ifdef __cplusplus
}
#endif
