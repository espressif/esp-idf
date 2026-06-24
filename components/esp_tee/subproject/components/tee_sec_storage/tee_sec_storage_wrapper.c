/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "esp_err.h"

#include "esp_tee.h"
#include "esp_tee_sec_storage.h"
#include "secure_service_num.h"

esp_err_t esp_tee_sec_storage_clear_key(const char *key_id)
{
    return esp_tee_service_call_with_noniram_intr_disabled(2, SS_ESP_TEE_SEC_STORAGE_CLEAR_KEY, key_id);
}

esp_err_t esp_tee_sec_storage_gen_key(const esp_tee_sec_storage_key_cfg_t *cfg)
{
    return esp_tee_service_call_with_noniram_intr_disabled(2, SS_ESP_TEE_SEC_STORAGE_GEN_KEY, cfg);
}

esp_err_t esp_tee_sec_storage_ecdsa_sign(const esp_tee_sec_storage_key_cfg_t *cfg, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign)
{
    return esp_tee_service_call_with_noniram_intr_disabled(5, SS_ESP_TEE_SEC_STORAGE_ECDSA_SIGN, cfg, hash, hlen, out_sign);
}

esp_err_t esp_tee_sec_storage_ecdsa_get_pubkey(const esp_tee_sec_storage_key_cfg_t *cfg, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey)
{
    return esp_tee_service_call_with_noniram_intr_disabled(3, SS_ESP_TEE_SEC_STORAGE_ECDSA_GET_PUBKEY, cfg, out_pubkey);
}

esp_err_t esp_tee_sec_storage_aead_encrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, uint8_t *iv, size_t iv_len, uint8_t *tag, size_t tag_len, uint8_t *output)
{
    return esp_tee_service_call_with_noniram_intr_disabled(7, SS_ESP_TEE_SEC_STORAGE_AEAD_ENCRYPT, ctx, iv, iv_len, tag, tag_len, output);
}

esp_err_t esp_tee_sec_storage_aead_decrypt(const esp_tee_sec_storage_aead_ctx_t *ctx, const uint8_t *iv, size_t iv_len, const uint8_t *tag, size_t tag_len, uint8_t *output)
{
    return esp_tee_service_call_with_noniram_intr_disabled(7, SS_ESP_TEE_SEC_STORAGE_AEAD_DECRYPT, ctx, iv, iv_len, tag, tag_len, output);
}

#if SOC_HMAC_SUPPORTED
esp_err_t esp_tee_sec_storage_ecdsa_sign_pbkdf2(const esp_tee_sec_storage_pbkdf2_ctx_t *ctx, const uint8_t *hash, size_t hlen, esp_tee_sec_storage_ecdsa_sign_t *out_sign, esp_tee_sec_storage_ecdsa_pubkey_t *out_pubkey)
{
    return esp_tee_service_call(6, SS_ESP_TEE_SEC_STORAGE_ECDSA_SIGN_PBKDF2, ctx, hash, hlen, out_sign, out_pubkey);
}
#endif
