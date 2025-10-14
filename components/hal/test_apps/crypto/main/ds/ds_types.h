/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include "soc/soc_caps.h"

#define ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL           (0x1) /*!< HMAC peripheral problem */
#define ESP_ERR_HW_CRYPTO_DS_INVALID_KEY         (0x2) /*!< given HMAC key isn't correct, HMAC peripheral problem */
#define ESP_ERR_HW_CRYPTO_DS_INVALID_DIGEST      (0x4) /*!< message digest check failed, result is invalid */
#define ESP_ERR_HW_CRYPTO_DS_INVALID_PADDING     (0x5) /*!< padding check failed, but result is produced anyway and can be read*/

#define ESP_DS_IV_BIT_LEN 128
#define ESP_DS_IV_LEN (ESP_DS_IV_BIT_LEN / 8)
#define ESP_DS_SIGNATURE_MAX_BIT_LEN SOC_RSA_MAX_BIT_LEN
#define ESP_DS_SIGNATURE_MD_BIT_LEN 256
#define ESP_DS_SIGNATURE_M_PRIME_BIT_LEN 32
#define ESP_DS_SIGNATURE_L_BIT_LEN 32
#define ESP_DS_SIGNATURE_PADDING_BIT_LEN 64

#define ESP_DS_C_LEN (((ESP_DS_SIGNATURE_MAX_BIT_LEN * 3 \
        + ESP_DS_SIGNATURE_MD_BIT_LEN   \
        + ESP_DS_SIGNATURE_M_PRIME_BIT_LEN   \
        + ESP_DS_SIGNATURE_L_BIT_LEN   \
        + ESP_DS_SIGNATURE_PADDING_BIT_LEN) / 8))

typedef enum {
    ESP_DS_RSA_1024 = (1024 / 32) - 1,
    ESP_DS_RSA_2048 = (2048 / 32) - 1,
    ESP_DS_RSA_3072 = (3072 / 32) - 1,
    ESP_DS_RSA_4096 = (4096 / 32) - 1
} esp_digital_signature_length_t;

typedef struct esp_digital_signature_data {
    esp_digital_signature_length_t rsa_length;
    uint32_t iv[ESP_DS_IV_BIT_LEN / 32];
    uint8_t c[ESP_DS_C_LEN];
} esp_ds_data_t;

typedef struct {
    uint32_t  Y[ESP_DS_SIGNATURE_MAX_BIT_LEN / 32];
    uint32_t  M[ESP_DS_SIGNATURE_MAX_BIT_LEN / 32];
    uint32_t Rb[ESP_DS_SIGNATURE_MAX_BIT_LEN / 32];
    uint32_t M_prime;
    uint32_t length;
} esp_ds_p_data_t;
