/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_err.h"

typedef struct {
    const uint8_t *aad;                      /*!< Additional authenticated data */
    size_t aad_len;                          /*!< Length of additional authenticated data */
    const uint8_t *input;                    /*!< Input data buffer */
    size_t input_len;                        /*!< Length of input data */
} example_aes_gcm_ctx_t;

esp_err_t example_sec_serv_aes_gcm_encrypt(example_aes_gcm_ctx_t *ctx, uint8_t *tag, size_t tag_len, uint8_t *output);

esp_err_t example_sec_serv_aes_gcm_decrypt(example_aes_gcm_ctx_t *ctx, const uint8_t *tag, size_t tag_len, uint8_t *output);
