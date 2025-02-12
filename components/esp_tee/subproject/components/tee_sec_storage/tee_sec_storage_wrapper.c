/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "secure_service_num.h"
#include "esp_tee.h"
#include "esp_err.h"
#include "esp_tee_sec_storage.h"

esp_err_t esp_tee_sec_storage_init(void)
{
    return esp_tee_service_call_with_noniram_intr_disabled(1, SS_ESP_TEE_SEC_STORAGE_INIT);
}

esp_err_t esp_tee_sec_storage_gen_key(uint16_t slot_id, esp_tee_sec_storage_type_t key_type)
{
    return esp_tee_service_call_with_noniram_intr_disabled(3, SS_ESP_TEE_SEC_STORAGE_GEN_KEY, slot_id, key_type);
}

esp_err_t esp_tee_sec_storage_get_signature(uint16_t slot_id, esp_tee_sec_storage_type_t key_type, uint8_t *hash, size_t hlen, esp_tee_sec_storage_sign_t *sign)
{
    return esp_tee_service_call_with_noniram_intr_disabled(6, SS_ESP_TEE_SEC_STORAGE_GET_SIGNATURE, slot_id, key_type, hash, hlen, sign);
}

esp_err_t esp_tee_sec_storage_get_pubkey(uint16_t slot_id, esp_tee_sec_storage_type_t key_type, esp_tee_sec_storage_pubkey_t *pubkey)
{
    return esp_tee_service_call_with_noniram_intr_disabled(4, SS_ESP_TEE_SEC_STORAGE_GET_PUBKEY, slot_id, key_type, pubkey);
}

bool esp_tee_sec_storage_is_slot_empty(uint16_t slot_id)
{
    return esp_tee_service_call_with_noniram_intr_disabled(2, SS_ESP_TEE_SEC_STORAGE_IS_SLOT_EMPTY, slot_id);
}

esp_err_t esp_tee_sec_storage_clear_slot(uint16_t slot_id)
{
    return esp_tee_service_call_with_noniram_intr_disabled(2, SS_ESP_TEE_SEC_STORAGE_CLEAR_SLOT, slot_id);
}

esp_err_t esp_tee_sec_storage_encrypt(uint16_t slot_id, uint8_t *input, uint8_t len, uint8_t *aad,
                                      uint16_t aad_len, uint8_t *tag, uint16_t tag_len, uint8_t *output)
{
    return esp_tee_service_call_with_noniram_intr_disabled(9, SS_ESP_TEE_SEC_STORAGE_ENCRYPT, slot_id,
                                                           input, len, aad, aad_len, tag, tag_len, output);
}

esp_err_t esp_tee_sec_storage_decrypt(uint16_t slot_id, uint8_t *input, uint8_t len, uint8_t *aad,
                                      uint16_t aad_len, uint8_t *tag, uint16_t tag_len, uint8_t *output)
{
    return esp_tee_service_call_with_noniram_intr_disabled(9, SS_ESP_TEE_SEC_STORAGE_DECRYPT, slot_id,
                                                           input, len, aad, aad_len, tag, tag_len, output);
}
