/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/common/bt_str.h>

#define USE_MBEDTLS     0

#if USE_MBEDTLS
#include "mbedtls/aes.h"
#include "mbedtls/cipher.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/cmac.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"
#else /* USE_MBEDTLS */
#include <tinycrypt/aes.h>
#include <tinycrypt/cmac_mode.h>
#include <tinycrypt/constants.h>
#endif /* USE_MBEDTLS */

#include "esp_random.h"

int bt_rand(void *buf, size_t len)
{
    if (buf == NULL || len == 0) {
        return -EINVAL;
    }

    esp_fill_random(buf, len);

    return 0;
}

int bt_encrypt_le(const uint8_t key[16],
                  const uint8_t plaintext[16],
                  uint8_t enc_data[16])
{
    uint8_t tmp[16];

    CHECKIF(key == NULL || plaintext == NULL || enc_data == NULL) {
        return -EINVAL;
    }

    LOG_DBG("EncKey[%s]", bt_hex(key, 16));
    LOG_DBG("EncPlainText[%s]", bt_hex(plaintext, 16));

#if USE_MBEDTLS
    mbedtls_aes_context ctx = {0};
    int ret = 0;

    mbedtls_aes_init(&ctx);

    sys_memcpy_swap(tmp, key, 16);

    if ((ret = mbedtls_aes_setkey_enc(&ctx, tmp, 128)) != 0) {
        goto exit;
    }

    sys_memcpy_swap(tmp, plaintext, 16);

    if ((ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT,
                                     tmp, enc_data)) != 0) {
        goto exit;
    }

    sys_mem_swap(enc_data, 16);

    LOG_DBG("EncData[%s]", bt_hex(enc_data, 16));

exit:
    mbedtls_aes_free(&ctx);
    return ret;
#else /* USE_MBEDTLS */
    struct tc_aes_key_sched_struct s;

    sys_memcpy_swap(tmp, key, 16);

    if (tc_aes128_set_encrypt_key(&s, tmp) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    sys_memcpy_swap(tmp, plaintext, 16);

    if (tc_aes_encrypt(enc_data, tmp, &s) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    sys_mem_swap(enc_data, 16);

    LOG_DBG("EncData[%s]", bt_hex(enc_data, 16));

    return 0;
#endif /* USE_MBEDTLS */
}

int bt_crypto_aes_cmac(const uint8_t *key,
                       const uint8_t *in,
                       size_t len,
                       uint8_t *out)
{
    CHECKIF(key == NULL || in == NULL || len == 0 || out == NULL) {
        return -EINVAL;
    }

    LOG_DBG("CmacKey[%s]", bt_hex(key, 16));
    LOG_DBG("CmacInput[%s]", bt_hex(in, len));

#if USE_MBEDTLS
    const mbedtls_cipher_info_t *cipher_info;
    mbedtls_cipher_context_t ctx;
    int ret = 0;

    mbedtls_cipher_init(&ctx);

    cipher_info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
    if (cipher_info == NULL) {
        ret = -1;
        goto exit;
    }

    if ((ret = mbedtls_cipher_setup(&ctx, cipher_info)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_starts(&ctx, key, 128)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_update(&ctx, in, len)) != 0) {
        goto exit;
    }

    if ((ret = mbedtls_cipher_cmac_finish(&ctx, out)) != 0) {
        goto exit;
    }

    ret = 0;

exit:
    mbedtls_cipher_free(&ctx);
    return ret;
#else /* USE_MBEDTLS */
    struct tc_aes_key_sched_struct sched;
    struct tc_cmac_struct state;

    if (tc_cmac_setup(&state, key, &sched) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    if (tc_cmac_update(&state, in, len) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    if (tc_cmac_final(out, &state) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    return 0;
#endif /* USE_MBEDTLS */
}
