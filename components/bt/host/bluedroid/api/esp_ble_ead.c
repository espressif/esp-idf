/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>
#include "esp_ble_ead.h"
#include "esp_random.h"
#include "esp_log.h"
#include "sdkconfig.h"

#if (CONFIG_BT_BLE_FEAT_ENC_ADV_DATA)

#define TAG "BLE_EAD"

#if defined(CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT)
#include "tinycrypt/aes.h"
#include "tinycrypt/ccm_mode.h"
#include "tinycrypt/constants.h"
#elif defined(CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS)
#include "psa/crypto.h"
#else
#error "Please select either CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT or CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS"
#endif

/* Additional Authenticated Data for EAD - EA (Encrypted Advertising) */
static const uint8_t ble_ead_aad[ESP_BLE_EAD_AAD_SIZE] = { 0xEA };

/**
 * @brief Generate randomizer with direction bit set
 *
 * Per Bluetooth Core Spec Supplement v11, Part A 1.23.3:
 * The MSB of the Randomizer shall be set to indicate direction
 */
static int ble_ead_generate_randomizer(uint8_t randomizer[ESP_BLE_EAD_RANDOMIZER_SIZE])
{
    esp_fill_random(randomizer, ESP_BLE_EAD_RANDOMIZER_SIZE);
    randomizer[ESP_BLE_EAD_RANDOMIZER_SIZE - 1] |= (1 << ESP_BLE_EAD_RANDOMIZER_DIRECTION_BIT);
    return 0;
}

/**
 * @brief Generate nonce from IV and randomizer
 *
 * Nonce = Randomizer (5 bytes) || IV (8 bytes) = 13 bytes
 */
static int ble_ead_generate_nonce(const uint8_t iv[ESP_BLE_EAD_IV_SIZE],
                                  const uint8_t randomizer[ESP_BLE_EAD_RANDOMIZER_SIZE],
                                  uint8_t nonce[ESP_BLE_EAD_NONCE_SIZE])
{
    if (iv == NULL || nonce == NULL) {
        return -1;
    }

    if (randomizer != NULL) {
        memcpy(nonce, randomizer, ESP_BLE_EAD_RANDOMIZER_SIZE);
    } else {
        ble_ead_generate_randomizer(nonce);
    }

    memcpy(nonce + ESP_BLE_EAD_RANDOMIZER_SIZE, iv, ESP_BLE_EAD_IV_SIZE);
    return 0;
}

static int ble_aes_ccm_encrypt(const uint8_t *key, const uint8_t *nonce,
                               const uint8_t *plaintext, size_t plaintext_len,
                               const uint8_t *aad, size_t aad_len,
                               uint8_t *ciphertext, size_t tag_len)
{
#if defined(CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT)
    struct tc_aes_key_sched_struct sched;
    struct tc_ccm_mode_struct ccm_state;
    int ret;

    if (key == NULL || nonce == NULL || ciphertext == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return -1;
    }

    ret = tc_aes128_set_encrypt_key(&sched, key);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_aes128_set_encrypt_key failed");
        memset(&sched, 0, sizeof(sched));
        return -1;
    }

    ret = tc_ccm_config(&ccm_state, &sched, (uint8_t *)nonce, ESP_BLE_EAD_NONCE_SIZE, tag_len);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_ccm_config failed");
        memset(&sched, 0, sizeof(sched));
        memset(&ccm_state, 0, sizeof(ccm_state));
        return -1;
    }

    ret = tc_ccm_generation_encryption(ciphertext, plaintext_len + tag_len,
                                       aad, aad_len,
                                       plaintext, plaintext_len,
                                       &ccm_state);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_ccm_generation_encryption failed");
        memset(&sched, 0, sizeof(sched));
        memset(&ccm_state, 0, sizeof(ccm_state));
        return -1;
    }

    memset(&sched, 0, sizeof(sched));
    memset(&ccm_state, 0, sizeof(ccm_state));
    return 0;

#elif defined(CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS)
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, tag_len);
    size_t output_length = 0;

    if (key == NULL || nonce == NULL || ciphertext == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return -1;
    }

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_crypto_init failed: %d", (int)status);
        return -1;
    }

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, ESP_BLE_EAD_KEY_SIZE * 8);

    status = psa_import_key(&attributes, key, ESP_BLE_EAD_KEY_SIZE, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_import_key failed: %d", (int)status);
        psa_reset_key_attributes(&attributes);
        return -1;
    }
    psa_reset_key_attributes(&attributes);

    status = psa_aead_encrypt(key_id, alg,
                              nonce, ESP_BLE_EAD_NONCE_SIZE,
                              aad, aad_len,
                              plaintext, plaintext_len,
                              ciphertext, plaintext_len + tag_len,
                              &output_length);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_aead_encrypt failed: %d", (int)status);
        psa_destroy_key(key_id);
        return -1;
    }

    if (output_length != plaintext_len + tag_len) {
        ESP_LOGE(TAG, "psa_aead_encrypt output length mismatch: expected %zu, got %zu",
                 plaintext_len + tag_len, output_length);
        psa_destroy_key(key_id);
        return -1;
    }

    psa_destroy_key(key_id);
    return 0;
#endif
}

static int ble_aes_ccm_decrypt(const uint8_t *key, const uint8_t *nonce,
                               const uint8_t *ciphertext, size_t ciphertext_len,
                               const uint8_t *aad, size_t aad_len,
                               uint8_t *plaintext, size_t tag_len,
                               size_t plaintext_capacity)
{
#if defined(CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT)
    struct tc_aes_key_sched_struct sched;
    struct tc_ccm_mode_struct ccm_state;
    int ret;
    size_t plaintext_len;

    if (key == NULL || nonce == NULL || ciphertext == NULL || plaintext == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return -1;
    }

    if (ciphertext_len < tag_len) {
        ESP_LOGE(TAG, "ciphertext_len (%zu) < tag_len (%zu)", ciphertext_len, tag_len);
        return -1;
    }

    plaintext_len = ciphertext_len - tag_len;
    if (plaintext_len > plaintext_capacity) {
        ESP_LOGE(TAG, "plaintext_len (%zu) > plaintext_capacity (%zu)", plaintext_len, plaintext_capacity);
        return -1;
    }

    ret = tc_aes128_set_encrypt_key(&sched, key);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_aes128_set_encrypt_key failed");
        memset(&sched, 0, sizeof(sched));
        return -1;
    }

    ret = tc_ccm_config(&ccm_state, &sched, (uint8_t *)nonce, ESP_BLE_EAD_NONCE_SIZE, tag_len);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_ccm_config failed");
        memset(&sched, 0, sizeof(sched));
        memset(&ccm_state, 0, sizeof(ccm_state));
        return -1;
    }

    ret = tc_ccm_decryption_verification(plaintext, plaintext_len,
                                         aad, aad_len,
                                         (uint8_t *)ciphertext, ciphertext_len,
                                         &ccm_state);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_ccm_decryption_verification failed");
        memset(&sched, 0, sizeof(sched));
        memset(&ccm_state, 0, sizeof(ccm_state));
        return -1;
    }

    memset(&sched, 0, sizeof(sched));
    memset(&ccm_state, 0, sizeof(ccm_state));
    return 0;

#elif defined(CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS)
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, tag_len);
    size_t output_length = 0;
    size_t plaintext_len;

    if (key == NULL || nonce == NULL || ciphertext == NULL || plaintext == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return -1;
    }

    if (ciphertext_len < tag_len) {
        ESP_LOGE(TAG, "ciphertext_len (%zu) < tag_len (%zu)", ciphertext_len, tag_len);
        return -1;
    }

    plaintext_len = ciphertext_len - tag_len;
    if (plaintext_len > plaintext_capacity) {
        ESP_LOGE(TAG, "plaintext_len (%zu) > plaintext_capacity (%zu)", plaintext_len, plaintext_capacity);
        return -1;
    }

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_crypto_init failed: %d", (int)status);
        return -1;
    }

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, ESP_BLE_EAD_KEY_SIZE * 8);

    status = psa_import_key(&attributes, key, ESP_BLE_EAD_KEY_SIZE, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_import_key failed: %d", (int)status);
        psa_reset_key_attributes(&attributes);
        return -1;
    }
    psa_reset_key_attributes(&attributes);

    status = psa_aead_decrypt(key_id, alg,
                              nonce, ESP_BLE_EAD_NONCE_SIZE,
                              aad, aad_len,
                              ciphertext, ciphertext_len,
                              plaintext, plaintext_len,
                              &output_length);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_aead_decrypt failed: %d", (int)status);
        psa_destroy_key(key_id);
        return -1;
    }

    if (output_length != plaintext_len) {
        ESP_LOGE(TAG, "psa_aead_decrypt output length mismatch: expected %zu, got %zu",
                 plaintext_len, output_length);
        psa_destroy_key(key_id);
        return -1;
    }

    psa_destroy_key(key_id);
    return 0;
#endif
}

esp_err_t esp_ble_ead_encrypt(const uint8_t session_key[ESP_BLE_EAD_KEY_SIZE],
                              const uint8_t iv[ESP_BLE_EAD_IV_SIZE],
                              const uint8_t *payload, size_t payload_size,
                              uint8_t *encrypted_payload)
{
    int ret;
    uint8_t nonce[ESP_BLE_EAD_NONCE_SIZE];

    if (session_key == NULL) {
        ESP_LOGE(TAG, "session_key is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (iv == NULL) {
        ESP_LOGE(TAG, "iv is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (payload == NULL && payload_size > 0) {
        ESP_LOGE(TAG, "payload is NULL but payload_size > 0");
        return ESP_ERR_INVALID_ARG;
    }

    if (encrypted_payload == NULL) {
        ESP_LOGE(TAG, "encrypted_payload is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* CSS Part A 1.23.2: plaintext shall consist of one or more AD structures */
    if (payload_size == 0) {
        ESP_LOGE(TAG, "payload_size is 0");
        return ESP_ERR_INVALID_ARG;
    }

    if (payload_size > SIZE_MAX - (ESP_BLE_EAD_RANDOMIZER_SIZE + ESP_BLE_EAD_MIC_SIZE)) {
        ESP_LOGE(TAG, "payload_size too large");
        return ESP_ERR_INVALID_ARG;
    }

    ret = ble_ead_generate_nonce(iv, NULL, nonce);
    if (ret != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(encrypted_payload, nonce, ESP_BLE_EAD_RANDOMIZER_SIZE);

    ret = ble_aes_ccm_encrypt(session_key, nonce,
                              payload, payload_size,
                              ble_ead_aad, ESP_BLE_EAD_AAD_SIZE,
                              &encrypted_payload[ESP_BLE_EAD_RANDOMIZER_SIZE],
                              ESP_BLE_EAD_MIC_SIZE);
    if (ret != 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_ead_decrypt(const uint8_t session_key[ESP_BLE_EAD_KEY_SIZE],
                              const uint8_t iv[ESP_BLE_EAD_IV_SIZE],
                              const uint8_t *encrypted_payload, size_t encrypted_payload_size,
                              uint8_t *payload, size_t payload_capacity)
{
    int ret;
    uint8_t nonce[ESP_BLE_EAD_NONCE_SIZE];
    const uint8_t *randomizer;
    const uint8_t *ciphertext;
    size_t ciphertext_len;
    size_t expected_plaintext_len;

    if (session_key == NULL) {
        ESP_LOGE(TAG, "session_key is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (iv == NULL) {
        ESP_LOGE(TAG, "iv is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (encrypted_payload == NULL) {
        ESP_LOGE(TAG, "encrypted_payload is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (payload == NULL) {
        ESP_LOGE(TAG, "payload is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    /* Randomizer + MIC + at least 1 byte of AD structure */
    if (encrypted_payload_size < ESP_BLE_EAD_RANDOMIZER_SIZE + ESP_BLE_EAD_MIC_SIZE + 1) {
        ESP_LOGE(TAG, "encrypted_payload_size too small");
        return ESP_ERR_INVALID_ARG;
    }

    expected_plaintext_len = ESP_BLE_EAD_DECRYPTED_PAYLOAD_SIZE(encrypted_payload_size);
    if (expected_plaintext_len > payload_capacity) {
        ESP_LOGE(TAG, "EAD plaintext length %zu exceeds payload buffer %zu",
                 expected_plaintext_len, payload_capacity);
        return ESP_ERR_INVALID_ARG;
    }

    randomizer = encrypted_payload;
    ciphertext = &encrypted_payload[ESP_BLE_EAD_RANDOMIZER_SIZE];
    ciphertext_len = encrypted_payload_size - ESP_BLE_EAD_RANDOMIZER_SIZE;

    ret = ble_ead_generate_nonce(iv, randomizer, nonce);
    if (ret != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = ble_aes_ccm_decrypt(session_key, nonce,
                              ciphertext, ciphertext_len,
                              ble_ead_aad, ESP_BLE_EAD_AAD_SIZE,
                              payload, ESP_BLE_EAD_MIC_SIZE,
                              payload_capacity);
    if (ret != 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#endif /* CONFIG_BT_BLE_FEAT_ENC_ADV_DATA */
