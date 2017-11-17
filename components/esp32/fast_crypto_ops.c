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

#include "crypto/common.h"
#include "crypto/aes_wrap.h"
#include "crypto/sha256.h"
#include "crypto/crypto.h"
#include "esp_wifi_crypto_types.h"

/* 
 * The parameters is used to set the cyrpto callback function for station connect when in security mode,
 * every callback function can register as fast_xxx or normal one, i.e, fast_aes_wrap or aes_wrap, the 
 * difference between them is the normal API is calculate by software, the fast one use the hardware 
 * crypto in it, can be faster than the normal one, so the callback function register in default is which
 * we recommend, so as the API in WPS default and WPA2 default.
 */
const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs = {
    .aes_wrap = (esp_aes_wrap_t)fast_aes_wrap,
    .aes_unwrap = (esp_aes_unwrap_t)fast_aes_unwrap,
    .hmac_sha256_vector = (esp_hmac_sha256_vector_t)fast_hmac_sha256_vector,
    .sha256_prf = (esp_sha256_prf_t)fast_sha256_prf
};

const wps_crypto_funcs_t g_wifi_default_wps_crypto_funcs = {
    .aes_128_encrypt = (esp_aes_128_encrypt_t)fast_aes_128_cbc_encrypt,
    .aes_128_decrypt = (esp_aes_128_decrypt_t)fast_aes_128_cbc_decrypt,
    .crypto_mod_exp = (esp_crypto_mod_exp_t)fast_crypto_mod_exp,
    .hmac_sha256 = (esp_hmac_sha256_t)fast_hmac_sha256,
    .hmac_sha256_vector = (esp_hmac_sha256_vector_t)fast_hmac_sha256_vector,
    .sha256_vector = (esp_sha256_vector_t)fast_sha256_vector
};

/*
 * What should notice is that the cyrpto hash type function and crypto cipher type function can not register
 * as different, i.e, if you use fast_crypto_hash_init, you should use fast_crypto_hash_update and 
 * fast_crypto_hash_finish for finish hash calculate, rather than call crypto_hash_update and 
 * crypto_hash_finish, so do crypto_cipher.
 */
const wpa2_crypto_funcs_t g_wifi_default_wpa2_crypto_funcs = {
    .crypto_hash_init = (esp_crypto_hash_init_t)fast_crypto_hash_init,
    .crypto_hash_update = (esp_crypto_hash_update_t)fast_crypto_hash_update,
    .crypto_hash_finish = (esp_crypto_hash_finish_t)fast_crypto_hash_finish,
    .crypto_cipher_init = (esp_crypto_cipher_init_t)fast_crypto_cipher_init,
    .crypto_cipher_encrypt = (esp_crypto_cipher_encrypt_t)fast_crypto_cipher_encrypt,
    .crypto_cipher_decrypt = (esp_crypto_cipher_decrypt_t)fast_crypto_cipher_decrypt,
    .crypto_cipher_deinit = (esp_crypto_cipher_deinit_t)fast_crypto_cipher_deinit,
    .sha256_vector = (esp_sha256_vector_t)fast_sha256_vector,
    .crypto_mod_exp = (esp_crypto_mod_exp_t)crypto_mod_exp
};

