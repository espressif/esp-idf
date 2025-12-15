/*
 * SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "psa/crypto.h"

#include "hal/sha_hal.h"
#include "hal/sha_types.h"
#include "psa/crypto_sizes.h"
#include "soc/soc_caps.h"
#include "esp_log.h"

#if SOC_SHA_SUPPORT_PARALLEL_ENG
#include "sha/sha_parallel_engine.h"
#else
#include "sha/sha_core.h"
#endif

static const char *TAG = "esp_sha";

void esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{
    int ret __attribute__((unused));
    assert(input != NULL && output != NULL);

    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_NONE;

#if SOC_SHA_SUPPORT_SHA1
    if (sha_type == SHA1) {
        alg = PSA_ALG_SHA_1;
    }
#endif //SOC_SHA_SUPPORT_SHA1

#if SOC_SHA_SUPPORT_SHA224
    if (sha_type == SHA2_224) {
        alg = PSA_ALG_SHA_224;
    }
#endif //SOC_SHA_SUPPORT_SHA224

#if SOC_SHA_SUPPORT_SHA256
    if (sha_type == SHA2_256) {
        alg = PSA_ALG_SHA_256;
    }
#endif //SOC_SHA_SUPPORT_SHA256

#if SOC_SHA_SUPPORT_SHA384
    if (sha_type == SHA2_384) {
        alg = PSA_ALG_SHA_384;
    }
#endif //SOC_SHA_SUPPORT_SHA384

#if SOC_SHA_SUPPORT_SHA512
    if (sha_type == SHA2_512) {
        alg = PSA_ALG_SHA_512;
    }
#endif //SOC_SHA_SUPPORT_SHA512

    if (alg == PSA_ALG_NONE) {
        ESP_LOGE(TAG, "SHA type %d not supported", (int)sha_type);
        abort();
    }
    size_t olen;
    size_t output_len = PSA_HASH_LENGTH(alg);
    status = psa_hash_compute(alg, input, ilen, output, output_len, &olen);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "SHA computation failed, status %d", status);
        abort();
    }
    if (olen != output_len) {
        ESP_LOGE(TAG, "SHA output length mismatch, expected %u, got %u", output_len, olen);
        abort();
    }
}


#if SOC_SHA_SUPPORT_SHA512_T

/* The initial hash value for SHA512/t is generated according to the
   algorithm described in the TRM, chapter SHA-Accelerator
*/
int esp_sha_512_t_init_hash(uint16_t t)
{
    uint32_t t_string = 0;
    uint8_t t0, t1, t2, t_len;

    if (t == 384) {
        ESP_LOGE(TAG, "Invalid t for SHA512/t, t = %u,cannot be 384", t);
        return -1;
    }

    if (t <= 9) {
        t_string = (uint32_t)((1 << 23) | ((0x30 + t) << 24));
        t_len = 0x48;
    } else if (t <= 99) {
        t0 = t % 10;
        t1 = (t / 10) % 10;
        t_string = (uint32_t)((1 << 15) | ((0x30 + t0) << 16) |
                              (((0x30 + t1) << 24)));
        t_len = 0x50;
    } else if (t <= 512) {
        t0 = t % 10;
        t1 = (t / 10) % 10;
        t2 = t / 100;
        t_string = (uint32_t)((1 << 7) | ((0x30 + t0) << 8) |
                              (((0x30 + t1) << 16) + ((0x30 + t2) << 24)));
        t_len = 0x58;
    } else {
        ESP_LOGE(TAG, "Invalid t for SHA512/t, t = %u, must equal or less than 512", t);
        return -1;
    }

    sha_hal_sha512_init_hash(t_string, t_len);

    return 0;
}

#endif //SOC_SHA_SUPPORT_SHA512_T
