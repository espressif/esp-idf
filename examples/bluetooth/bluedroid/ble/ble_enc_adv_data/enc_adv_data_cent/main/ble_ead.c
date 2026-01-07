/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
#include "mbedtls/ccm.h"
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
    mbedtls_ccm_context ctx = {0};
    int ret;

    /* Validate inputs */
    if (key == NULL || nonce == NULL || ciphertext == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return -1;
    }

    mbedtls_ccm_init(&ctx);

    /* Set encryption key */
    ret = mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, BLE_EAD_KEY_SIZE * 8);
    if (ret != 0) {
        ESP_LOGE(TAG, "mbedtls_ccm_setkey failed: %d", ret);
        mbedtls_ccm_free(&ctx);
        return -1;
    }

    /* Encrypt and authenticate */
    /* mbedtls_ccm_encrypt_and_tag outputs: ciphertext || tag */
    ret = mbedtls_ccm_encrypt_and_tag(&ctx, plaintext_len,
                                      nonce, BLE_EAD_NONCE_SIZE,
                                      aad, aad_len,
                                      plaintext, ciphertext,
                                      ciphertext + plaintext_len, tag_len);
    if (ret != 0) {
        ESP_LOGE(TAG, "mbedtls_ccm_encrypt_and_tag failed: %d", ret);
        mbedtls_ccm_free(&ctx);
        return -1;
    }

    mbedtls_ccm_free(&ctx);
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
    mbedtls_ccm_context ctx = {0};
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

    mbedtls_ccm_init(&ctx);

    /* Set decryption key */
    ret = mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, BLE_EAD_KEY_SIZE * 8);
    if (ret != 0) {
        ESP_LOGE(TAG, "mbedtls_ccm_setkey failed: %d", ret);
        mbedtls_ccm_free(&ctx);
        return -1;
    }

    /* Decrypt and verify */
    /* mbedtls_ccm_auth_decrypt expects: ciphertext || tag */
    /* ciphertext_len here already includes tag length */
    ret = mbedtls_ccm_auth_decrypt(&ctx, plaintext_len,
                                   nonce, BLE_EAD_NONCE_SIZE,
                                   aad, aad_len,
                                   ciphertext, plaintext,
                                   ciphertext + plaintext_len, tag_len);
    if (ret != 0) {
        ESP_LOGE(TAG, "mbedtls_ccm_auth_decrypt failed: %d", ret);
        mbedtls_ccm_free(&ctx);
        return -1;
    }

    mbedtls_ccm_free(&ctx);
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
    /* ciphertext_len includes both ciphertext and MIC (tag) for mbedTLS API */
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
