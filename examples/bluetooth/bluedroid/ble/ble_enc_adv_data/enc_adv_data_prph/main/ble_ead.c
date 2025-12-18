/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "ble_ead.h"
#include "esp_random.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define TAG "BLE_EAD"

/* Select crypto library based on configuration */
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
static const uint8_t ble_ead_aad[BLE_EAD_AAD_SIZE] = { 0xEA };

/**
 * @brief Generate randomizer with direction bit set
 *
 * Per Bluetooth Core Spec Supplement v11, Part A 1.23.3:
 * The MSB of the Randomizer shall be set to indicate direction
 */
static int ble_ead_generate_randomizer(uint8_t randomizer[BLE_EAD_RANDOMIZER_SIZE])
{
    /* Generate random bytes */
    esp_fill_random(randomizer, BLE_EAD_RANDOMIZER_SIZE);

    /* Set direction bit (MSB of last byte) - required by spec */
    randomizer[BLE_EAD_RANDOMIZER_SIZE - 1] |= (1 << BLE_EAD_RANDOMIZER_DIRECTION_BIT);

    return 0;
}

/**
 * @brief Generate nonce from IV and randomizer
 *
 * Nonce = Randomizer (5 bytes) || IV (8 bytes) = 13 bytes
 */
static int ble_ead_generate_nonce(const uint8_t iv[BLE_EAD_IV_SIZE],
                                   const uint8_t randomizer[BLE_EAD_RANDOMIZER_SIZE],
                                   uint8_t nonce[BLE_EAD_NONCE_SIZE])
{
    if (iv == NULL || nonce == NULL) {
        return -1;
    }

    /* Randomizer in first 5 bytes */
    if (randomizer != NULL) {
        memcpy(nonce, randomizer, BLE_EAD_RANDOMIZER_SIZE);
    } else {
        /* Generate new randomizer with direction bit */
        ble_ead_generate_randomizer(nonce);
    }

    /* IV in last 8 bytes */
    memcpy(nonce + BLE_EAD_RANDOMIZER_SIZE, iv, BLE_EAD_IV_SIZE);

    return 0;
}

/**
 * @brief AES-CCM encryption using selected crypto library
 */
static int ble_aes_ccm_encrypt(const uint8_t *key, const uint8_t *nonce,
                                const uint8_t *plaintext, size_t plaintext_len,
                                const uint8_t *aad, size_t aad_len,
                                uint8_t *ciphertext, size_t tag_len)
{
#if defined(CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT)
    struct tc_aes_key_sched_struct sched;
    struct tc_ccm_mode_struct ccm_state;
    int ret;

    /* Validate inputs */
    if (key == NULL || nonce == NULL || ciphertext == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return -1;
    }

    /* Set AES encryption key */
    ret = tc_aes128_set_encrypt_key(&sched, key);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_aes128_set_encrypt_key failed");
        memset(&sched, 0, sizeof(sched));
        return -1;
    }

    /* Configure CCM mode */
    ccm_state.sched = &sched;
    ccm_state.nonce = (uint8_t *)nonce;
    ccm_state.mlen = tag_len;

    ret = tc_ccm_config(&ccm_state, &sched, (uint8_t *)nonce, BLE_EAD_NONCE_SIZE, tag_len);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_ccm_config failed");
        memset(&sched, 0, sizeof(sched));
        memset(&ccm_state, 0, sizeof(ccm_state));
        return -1;
    }

    /* Encrypt and generate tag */
    /* TinyCrypt outputs: ciphertext || tag */
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

    /* Clear sensitive data from key schedule */
    memset(&sched, 0, sizeof(sched));
    memset(&ccm_state, 0, sizeof(ccm_state));
    return 0;

#elif defined(CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS)
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, tag_len);
    size_t output_length = 0;

    /* Set key attributes */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, BLE_EAD_KEY_SIZE * 8);

    /* Import key */
    status = psa_import_key(&attributes, key, BLE_EAD_KEY_SIZE, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_import_key failed: %d", status);
        psa_reset_key_attributes(&attributes);
        return -1;
    }
    psa_reset_key_attributes(&attributes);

    /* Encrypt and authenticate */
    /* PSA AEAD encrypt outputs: ciphertext || tag */
    status = psa_aead_encrypt(key_id, alg,
                              nonce, BLE_EAD_NONCE_SIZE,
                              aad, aad_len,
                              plaintext, plaintext_len,
                              ciphertext, plaintext_len + tag_len,
                              &output_length);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_aead_encrypt failed: %d", status);
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
#else
    #error "No crypto library selected"
#endif
}

/**
 * @brief AES-CCM decryption with authentication using selected crypto library
 */
static int ble_aes_ccm_decrypt(const uint8_t *key, const uint8_t *nonce,
                                const uint8_t *ciphertext, size_t ciphertext_len,
                                const uint8_t *aad, size_t aad_len,
                                uint8_t *plaintext, size_t tag_len)
{
#if defined(CONFIG_BT_SMP_CRYPTO_STACK_TINYCRYPT)
    struct tc_aes_key_sched_struct sched;
    struct tc_ccm_mode_struct ccm_state;
    int ret;
    /* ciphertext_len here includes both ciphertext and tag */
    size_t plaintext_len;

    /* Validate inputs */
    if (key == NULL || nonce == NULL || ciphertext == NULL || plaintext == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return -1;
    }

    /* Check for integer underflow */
    if (ciphertext_len < tag_len) {
        ESP_LOGE(TAG, "ciphertext_len (%zu) < tag_len (%zu)", ciphertext_len, tag_len);
        return -1;
    }

    plaintext_len = ciphertext_len - tag_len;

    /* Set AES encryption key */
    ret = tc_aes128_set_encrypt_key(&sched, key);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_aes128_set_encrypt_key failed");
        memset(&sched, 0, sizeof(sched));
        return -1;
    }

    /* Configure CCM mode */
    ccm_state.sched = &sched;
    ccm_state.nonce = (uint8_t *)nonce;
    ccm_state.mlen = tag_len;

    ret = tc_ccm_config(&ccm_state, &sched, (uint8_t *)nonce, BLE_EAD_NONCE_SIZE, tag_len);
    if (ret != TC_CRYPTO_SUCCESS) {
        ESP_LOGE(TAG, "tc_ccm_config failed");
        memset(&sched, 0, sizeof(sched));
        memset(&ccm_state, 0, sizeof(ccm_state));
        return -1;
    }

    /* Decrypt and verify tag */
    /* TinyCrypt expects: ciphertext || tag */
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

    /* Clear sensitive data from key schedule */
    memset(&sched, 0, sizeof(sched));
    memset(&ccm_state, 0, sizeof(ccm_state));
    return 0;

#elif defined(CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS)
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, tag_len);
    size_t output_length = 0;
    /* ciphertext_len here includes both ciphertext and tag */
    size_t plaintext_len;

    /* Validate inputs */
    if (key == NULL || nonce == NULL || ciphertext == NULL || plaintext == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return -1;
    }

    /* Check for integer underflow */
    if (ciphertext_len < tag_len) {
        ESP_LOGE(TAG, "ciphertext_len (%zu) < tag_len (%zu)", ciphertext_len, tag_len);
        return -1;
    }

    plaintext_len = ciphertext_len - tag_len;

    /* Set key attributes */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, BLE_EAD_KEY_SIZE * 8);

    /* Import key */
    status = psa_import_key(&attributes, key, BLE_EAD_KEY_SIZE, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_import_key failed: %d", status);
        psa_reset_key_attributes(&attributes);
        return -1;
    }
    psa_reset_key_attributes(&attributes);

    /* Decrypt and verify */
    /* PSA AEAD decrypt expects: ciphertext || tag */
    status = psa_aead_decrypt(key_id, alg,
                              nonce, BLE_EAD_NONCE_SIZE,
                              aad, aad_len,
                              ciphertext, ciphertext_len,
                              plaintext, plaintext_len,
                              &output_length);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "psa_aead_decrypt failed: %d", status);
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
#else
    #error "No crypto library selected"
#endif
}

int ble_ead_encrypt(const uint8_t session_key[BLE_EAD_KEY_SIZE],
                    const uint8_t iv[BLE_EAD_IV_SIZE],
                    const uint8_t *payload, size_t payload_size,
                    uint8_t *encrypted_payload)
{
    int ret;
    uint8_t nonce[BLE_EAD_NONCE_SIZE];

    if (session_key == NULL) {
        ESP_LOGE(TAG, "session_key is NULL");
        return -1;
    }

    if (iv == NULL) {
        ESP_LOGE(TAG, "iv is NULL");
        return -1;
    }

    if (payload == NULL && payload_size > 0) {
        ESP_LOGE(TAG, "payload is NULL but payload_size > 0");
        return -1;
    }

    if (encrypted_payload == NULL) {
        ESP_LOGE(TAG, "encrypted_payload is NULL");
        return -1;
    }

    /* Generate nonce with random randomizer */
    ret = ble_ead_generate_nonce(iv, NULL, nonce);
    if (ret != 0) {
        return ret;
    }

    /* Copy randomizer to the start of encrypted payload */
    memcpy(encrypted_payload, nonce, BLE_EAD_RANDOMIZER_SIZE);

    /* Encrypt: output = ciphertext + MIC */
    ret = ble_aes_ccm_encrypt(session_key, nonce,
                               payload, payload_size,
                               ble_ead_aad, BLE_EAD_AAD_SIZE,
                               &encrypted_payload[BLE_EAD_RANDOMIZER_SIZE],
                               BLE_EAD_MIC_SIZE);

    return ret;
}

int ble_ead_decrypt(const uint8_t session_key[BLE_EAD_KEY_SIZE],
                    const uint8_t iv[BLE_EAD_IV_SIZE],
                    const uint8_t *encrypted_payload, size_t encrypted_payload_size,
                    uint8_t *payload)
{
    int ret;
    uint8_t nonce[BLE_EAD_NONCE_SIZE];
    const uint8_t *randomizer;
    const uint8_t *ciphertext;
    size_t ciphertext_len;

    if (session_key == NULL) {
        ESP_LOGE(TAG, "session_key is NULL");
        return -1;
    }

    if (iv == NULL) {
        ESP_LOGE(TAG, "iv is NULL");
        return -1;
    }

    if (encrypted_payload == NULL) {
        ESP_LOGE(TAG, "encrypted_payload is NULL");
        return -1;
    }

    if (payload == NULL) {
        ESP_LOGE(TAG, "payload is NULL");
        return -1;
    }

    if (encrypted_payload_size < BLE_EAD_RANDOMIZER_SIZE + BLE_EAD_MIC_SIZE) {
        ESP_LOGE(TAG, "encrypted_payload_size too small");
        return -1;
    }

    /* Extract randomizer from the start of encrypted payload */
    randomizer = encrypted_payload;

    /* Ciphertext + MIC follows the randomizer */
    ciphertext = &encrypted_payload[BLE_EAD_RANDOMIZER_SIZE];
    /* ciphertext_len includes both ciphertext and MIC (tag) for PSA API */
    ciphertext_len = encrypted_payload_size - BLE_EAD_RANDOMIZER_SIZE;

    /* Generate nonce from randomizer and IV */
    ret = ble_ead_generate_nonce(iv, randomizer, nonce);
    if (ret != 0) {
        return ret;
    }

    /* Decrypt and verify */
    ret = ble_aes_ccm_decrypt(session_key, nonce,
                               ciphertext, ciphertext_len,
                               ble_ead_aad, BLE_EAD_AAD_SIZE,
                               payload, BLE_EAD_MIC_SIZE);

    return ret;
}
