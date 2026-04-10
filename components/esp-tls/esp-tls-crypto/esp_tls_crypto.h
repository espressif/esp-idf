/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_TLS_CRYPTO_H
#define _ESP_TLS_CRYPTO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculate sha1 sum
 * esp-tls abstraction for crypto sha1 API, calculates the sha1 sum(digest) of
 * the data provided in input which is of ilen size and returns
 * a 20 char sha1 sum
 * @param[in]   input   Input array
 * @param[in]   ilen    Length of Input array
 * @param[out]  output  calculated sha1 sum
 *
 * @return
 *              -  0   if successful.
 *              - -1   if error.
 *              - MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED if SHA1 is not supported.
 */
int esp_crypto_sha1(const unsigned char *input,
                    size_t ilen,
                    unsigned char output[20]);

/**
 * @brief Do Base64 encode of the src data
 *
 * @param[in]   dst   destination buffer
 * @param[in]   dlen  length of destination buffer
 * @param[out]  olen  number of bytes written
 * @param[in]   src   src buffer to be encoded
 * @param[in]   slen  src buffer len
 *
 * @return
 *               - MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL  if buffer is of insufficient size.
 *               -  0   if successful.
 */
int esp_crypto_base64_encode(unsigned char *dst, size_t dlen,
                             size_t *olen, const unsigned char *src,
                             size_t slen);

#ifdef __cplusplus
}
#endif
#endif /* _ESP_TLS_CRYPTO_H */
