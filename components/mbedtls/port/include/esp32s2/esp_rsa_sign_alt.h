// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifndef _ESP_RSA_SIGN_ALT_H_
#define _ESP_RSA_SIGN_ALT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_ds.h"
#include "mbedtls/md.h"

/**
 * @brief      ESP-DS data context
 *
 * @note       This structure includes encrypted private key parameters such as ciphertext_c, initialization vector, efuse_key_id, RSA key length, which are obtained when DS peripheral is configured.
 */

/* Context for encrypted private key data required for DS */
typedef struct esp_ds_data_ctx {
    esp_ds_data_t *esp_ds_data;
    uint8_t efuse_key_id; /* efuse block id in which DS_KEY is stored e.g. 0,1*/
    uint16_t rsa_length_bits; /* length of RSA private key in bits e.g. 2048 */
} esp_ds_data_ctx_t;

/**
 * @brief       Initializes internal DS data context
 *
 * This function allocates and initializes internal ds data context which is used for Digital Signature operation.
 *
 * @in          ds_data     ds_data context containing encrypted private key parameters
 * @return
 *              - ESP_OK  In case of succees
 *              - ESP_ERR_NO_MEM In case internal context could not be allocated.
 *              - ESP_ERR_INVALID_ARG in case input parametrers are NULL
 *
 */
esp_err_t esp_ds_init_data_ctx(esp_ds_data_ctx_t *ds_data);

/**
 *
 * @brief      Release the ds lock acquired for the DS operation (then the DS peripheral can be used for other TLS connection)
 *
 */
void esp_ds_release_ds_lock(void);

/**
 *
 * @brief       Alternate implementation for mbedtls_rsa_rsassa_pkcs1_v15_sign, Internally makes use
 *              of DS module to perform hardware accelerated RSA sign operation
 */
int esp_ds_rsa_sign( void *ctx,
                     int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                     int mode, mbedtls_md_type_t md_alg, unsigned int hashlen,
                     const unsigned char *hash, unsigned char *sig );

/*
 * @brief       Get RSA key length in bytes from internal DS context
 *
 * @return      RSA key length in bytes
 */
size_t esp_ds_get_keylen(void *ctx);

/*
 * @brief       Set timeout (equal to TLS session timeout), so that DS module usage can be synchronized in case of multiple TLS connections using DS module,
 */
void esp_ds_set_session_timeout(int timeout);
#ifdef __cplusplus
}
#endif

#endif /* _ESP_RSA_SIGN_ALT_H_ */
