/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

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
 *              - ESP_OK  In case of success
 *              - ESP_ERR_NO_MEM In case internal context could not be allocated.
 *              - ESP_ERR_INVALID_ARG in case input parametrers are NULL
 *
 */
esp_err_t esp_ds_init_data_ctx(esp_ds_data_ctx_t *ds_data);

/**
 * @brief       Deinitializes internal DS data context
 *
 * This function deinitializes internal ds data context which is used for Digital Signature operation.
 *
 * @return esp_err_t
 *             - ESP_OK  In case of success
 *             - ESP_ERR_INVALID_STATE In case internal context is not initialized
 */
esp_err_t esp_ds_deinit_data_ctx(void);

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
                     mbedtls_md_type_t md_alg, unsigned int hashlen,
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

/**
 * @brief Alternate implementation for mbedtls_rsa_decrypt, Internally makes use
 *              of DS module to perform hardware accelerated RSA decrypt operation
 *
 * @param ctx   Context for the RSA operation. It should be a pointer to an mbedtls_rsa_context structure.
 *              The RSA context should have the correct padding type set (either MBEDTLS_RSA_PKCS_V15 or MBEDTLS_RSA_PKCS_V21).
 *              If ctx is NULL, the default padding type (MBEDTLS_RSA_PKCS_V15) will be used.
 * @param olen  Pointer to the output length variable, which will be set to the length of the decrypted data.
 * @param input     The input data to be decrypted. It should be a pointer to an array of unsigned char.
 * @param output    The buffer to hold the decrypted output. It should be a pointer to an array of unsigned char.
 * @param output_max_len    The maximum length of the output buffer. It should be greater than or equal to the expected length of the decrypted data.
 * @return int
 *       - 0 on success
 *      - -1 on failure
 */
int esp_ds_rsa_decrypt(void *ctx, size_t *olen,
    const unsigned char *input, unsigned char *output,
    size_t output_max_len);

#ifdef __cplusplus
}
#endif
