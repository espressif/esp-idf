/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils/common.h"
#include "aes_wrap.h"
#include "sha256.h"
#include "crypto.h"
#include "md5.h"
#include "sha1.h"
#include "aes.h"
#include "esp_wpa.h"
#include "ccmp.h"
#include "esp_rom_crc.h"

#define DEFAULT_KEK_LEN 16

static int esp_aes_wrap(const u8 *kek, int n, const u8 *plain, u8 *cipher)
{
	return aes_wrap(kek, DEFAULT_KEK_LEN, n, plain, cipher);
}

static int esp_aes_unwrap(const u8 *kek, int n, const u8 *cipher, u8 *plain)
{
	return aes_unwrap(kek, DEFAULT_KEK_LEN, n, cipher, plain);
}

static void esp_aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
	aes_encrypt(ctx, plain, crypt);
}

static void esp_aes_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
	aes_decrypt(ctx, crypt, plain);
}

static int esp_aes_gmac(const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,
			const u8 *aad, size_t aad_len, u8 *tag)
{
#if CONFIG_GMAC
	return aes_gmac(key, key_len, iv, iv_len, aad, aad_len, tag);
#else
	return -1;
#endif
}

static uint32_t esp_supp_crc32(uint32_t crc, uint8_t const *buf, uint32_t len)
{
    return esp_rom_crc32_le(crc, buf, len);
}

/*
 * This structure is used to set the cyrpto callback function for station to connect when in security mode.
 * These functions either call MbedTLS API's if CONFIG_CRYPTO_MBEDTLS flag is set through Kconfig, or native
 * API's otherwise. We recommend setting the flag since MbedTLS API's utilize hardware acceleration while
 * native API's are use software implementations.
 */
const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs = {
    .size = sizeof(wpa_crypto_funcs_t),
    .version = ESP_WIFI_CRYPTO_VERSION,
    .aes_wrap = (esp_aes_wrap_t)esp_aes_wrap,
    .aes_unwrap = (esp_aes_unwrap_t)esp_aes_unwrap,
    .hmac_sha256_vector = (esp_hmac_sha256_vector_t)hmac_sha256_vector,
    .sha256_prf = (esp_sha256_prf_t)sha256_prf,
    .hmac_md5 = (esp_hmac_md5_t)hmac_md5,
    .hamc_md5_vector = (esp_hmac_md5_vector_t)hmac_md5_vector,
    .hmac_sha1 = (esp_hmac_sha1_t)hmac_sha1,
    .hmac_sha1_vector = (esp_hmac_sha1_vector_t)hmac_sha1_vector,
    .sha1_prf = (esp_sha1_prf_t)sha1_prf,
    .sha1_vector = (esp_sha1_vector_t)sha1_vector,
    .pbkdf2_sha1 = (esp_pbkdf2_sha1_t)pbkdf2_sha1,
    .rc4_skip = (esp_rc4_skip_t)rc4_skip,
    .md5_vector = (esp_md5_vector_t)md5_vector,
    .aes_encrypt = (esp_aes_encrypt_t)esp_aes_encrypt,
    .aes_encrypt_init = (esp_aes_encrypt_init_t)aes_encrypt_init,
    .aes_encrypt_deinit = (esp_aes_encrypt_deinit_t)aes_encrypt_deinit,
    .aes_decrypt = (esp_aes_decrypt_t)esp_aes_decrypt,
    .aes_decrypt_init = (esp_aes_decrypt_init_t)aes_decrypt_init,
    .aes_decrypt_deinit = (esp_aes_decrypt_deinit_t)aes_decrypt_deinit,
    .aes_128_encrypt = (esp_aes_128_encrypt_t)aes_128_cbc_encrypt,
    .aes_128_decrypt = (esp_aes_128_decrypt_t)aes_128_cbc_decrypt,
    .omac1_aes_128 = (esp_omac1_aes_128_t)omac1_aes_128,
    .ccmp_decrypt = (esp_ccmp_decrypt_t)ccmp_decrypt,
    .ccmp_encrypt = (esp_ccmp_encrypt_t)ccmp_encrypt,
    .aes_gmac = (esp_aes_gmac_t)esp_aes_gmac,
    .sha256_vector = (esp_sha256_vector_t)sha256_vector,
    .crc32 = (esp_crc32_le_t)esp_supp_crc32,
};

const mesh_crypto_funcs_t g_wifi_default_mesh_crypto_funcs = {
    .aes_128_encrypt = (esp_aes_128_encrypt_t)aes_128_cbc_encrypt,
    .aes_128_decrypt = (esp_aes_128_decrypt_t)aes_128_cbc_decrypt,
};
