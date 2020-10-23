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
 * mbedtls stack:-
 *              - MBEDTLS_ERR_SHA1_BAD_INPUT_DATA   on BAD INPUT.
 *              -  0 on success.
 * wolfssl stack:-
 *              - -1    on failure.
 *              -  0    on success.
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
 * mbedtls stack:-
 *               - MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL  if buffer is of insufficient size.
 *               -  0   if successful.
 * wolfssl stack:-
 *               - <0   on failure.
 *               -  0   if succcessful.
 */
int esp_crypto_base64_encode(unsigned char *dst, size_t dlen,
                             size_t *olen, const unsigned char *src,
                             size_t slen);

#ifdef __cplusplus
}
#endif
#endif /* _ESP_TLS_CRYPTO_H */
