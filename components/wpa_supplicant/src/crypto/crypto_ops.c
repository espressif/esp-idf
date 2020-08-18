// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "utils/common.h"
#include "aes_wrap.h"
#include "sha256.h"
#include "crypto.h"
#include "md5.h"
#include "sha1.h"
#include "aes.h"
#include "esp_wpa.h"
#include "ccmp.h"

/*
 * This structure is used to set the cyrpto callback function for station to connect when in security mode.
 * These functions either call MbedTLS API's if USE_MBEDTLS_CRYPTO flag is set through Kconfig, or native
 * API's otherwise. We recommend setting the flag since MbedTLS API's utilize hardware acceleration while
 * native API's are use software implementations.
 */
const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs = {
    .size = sizeof(wpa_crypto_funcs_t),
    .version = ESP_WIFI_CRYPTO_VERSION,
    .aes_wrap = (esp_aes_wrap_t)aes_wrap,
    .aes_unwrap = (esp_aes_unwrap_t)aes_unwrap,
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
    .aes_encrypt = (esp_aes_encrypt_t)aes_encrypt,
    .aes_encrypt_init = (esp_aes_encrypt_init_t)aes_encrypt_init,
    .aes_encrypt_deinit = (esp_aes_encrypt_deinit_t)aes_encrypt_deinit,
    .aes_decrypt = (esp_aes_decrypt_t)aes_decrypt,
    .aes_decrypt_init = (esp_aes_decrypt_init_t)aes_decrypt_init,
    .aes_decrypt_deinit = (esp_aes_decrypt_deinit_t)aes_decrypt_deinit,
    .aes_128_encrypt = (esp_aes_128_encrypt_t)aes_128_cbc_encrypt,
    .aes_128_decrypt = (esp_aes_128_decrypt_t)aes_128_cbc_decrypt,
    .omac1_aes_128 = (esp_omac1_aes_128_t)omac1_aes_128,
    .ccmp_decrypt = (esp_ccmp_decrypt_t)ccmp_decrypt,
    .ccmp_encrypt = (esp_ccmp_encrypt_t)ccmp_encrypt
};

const mesh_crypto_funcs_t g_wifi_default_mesh_crypto_funcs = {
    .aes_128_encrypt = (esp_aes_128_encrypt_t)aes_128_cbc_encrypt,
    .aes_128_decrypt = (esp_aes_128_decrypt_t)aes_128_cbc_decrypt,
};
