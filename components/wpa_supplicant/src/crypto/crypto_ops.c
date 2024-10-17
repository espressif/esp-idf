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

static int esp_aes_gmac(const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,
			const u8 *aad, size_t aad_len, u8 *tag)
{
#if CONFIG_GMAC
	return aes_gmac(key, key_len, iv, iv_len, aad, aad_len, tag);
#else
	return -1;
#endif
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
    .hmac_sha256_vector = (esp_hmac_sha256_vector_t)hmac_sha256_vector,
    .pbkdf2_sha1 = (esp_pbkdf2_sha1_t)pbkdf2_sha1,
    .aes_128_encrypt = (esp_aes_128_encrypt_t)aes_128_cbc_encrypt,
    .aes_128_decrypt = (esp_aes_128_decrypt_t)aes_128_cbc_decrypt,
    .omac1_aes_128 = (esp_omac1_aes_128_t)omac1_aes_128,
    .ccmp_decrypt = (esp_ccmp_decrypt_t)ccmp_decrypt,
    .ccmp_encrypt = (esp_ccmp_encrypt_t)ccmp_encrypt,
    .aes_gmac = (esp_aes_gmac_t)esp_aes_gmac,
    .sha256_vector = (esp_sha256_vector_t)sha256_vector,
};

const mesh_crypto_funcs_t g_wifi_default_mesh_crypto_funcs = {
    .aes_128_encrypt = (esp_aes_128_encrypt_t)aes_128_cbc_encrypt,
    .aes_128_decrypt = (esp_aes_128_decrypt_t)aes_128_cbc_decrypt,
};
