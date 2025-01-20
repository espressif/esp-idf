/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_err.h"

/**
 * @brief Perform AES-256-CBC encryption/decryption operation in TEE
 *
 * @param mode      ESP_AES_ENCRYPT (1) for encryption, ESP_AES_DECRYPT (0) for decryption
 * @param length    Length of input data in bytes
 * @param iv        Initialization vector (16 bytes)
 * @param input     Input buffer containing plaintext (for encryption) or ciphertext (for decryption)
 * @param output    Output buffer for ciphertext (for encryption) or plaintext (for decryption)
 *
 * @return esp_err_t ESP_OK on success, appropriate error code on failure
 */
esp_err_t example_sec_serv_aes_op(int mode, size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output);
