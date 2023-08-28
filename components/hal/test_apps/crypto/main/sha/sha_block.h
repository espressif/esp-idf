/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
//  * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if SOC_SHA_SUPPORTED

#include "soc/periph_defs.h"
#include "esp_private/periph_ctrl.h"
#include "hal/sha_hal.h"
#include "test_params.h"

#if defined(SOC_SHA_SUPPORT_SHA1)

void sha1_block(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output);

#endif /* defined(SOC_SHA_SUPPORT_SHA1) */

#if defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256)

void sha256_block(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output);

#endif /* defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256) */

#if defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512)

#if SOC_SHA_SUPPORT_SHA512_T

int sha_512_t_init_hash_block(uint16_t t);

#endif //SOC_SHA_SUPPORT_SHA512_T

void sha512_block(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output);

#endif /* defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512) */

#if SOC_SHA_SUPPORT_SHA512_T

void sha512t_block(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output, uint32_t t_val);

#endif /*SOC_SHA_SUPPORT_SHA512_T*/

#endif /*SOC_SHA_SUPPORTED*/
