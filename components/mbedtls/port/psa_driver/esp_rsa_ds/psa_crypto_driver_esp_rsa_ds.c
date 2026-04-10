/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "psa_crypto_driver_esp_rsa_ds.h"
#include "psa_crypto_driver_esp_rsa_ds_contexts.h"
#include "include/psa_crypto_driver_esp_rsa_ds_utilities.h"

#include "esp_log.h"
#include "esp_efuse.h"
#include "soc/soc_caps.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include "esp_key_mgr.h"
#endif /* SOC_KEY_MANAGER_SUPPORTED */

#define ESP_RSA_DS_TIMEOUT_BUFFER_MS 1000

static const char *TAG = "PSA_RSA_DS";

static SemaphoreHandle_t s_ds_lock = NULL;
static int s_timeout_ms = 0;

void esp_rsa_ds_release_ds_lock(void);

static int esp_rsa_ds_pad(esp_rsa_ds_padding_t padding, psa_algorithm_t hash_alg, unsigned int hashlen,
                                            const unsigned char *hash,
                                            int saltlen,
                                            unsigned char *sig, size_t dst_len)
{
    if (padding == ESP_RSA_DS_PADDING_PKCS_V15) {
        (void)saltlen;
        return esp_rsa_ds_pad_v15_encode(hash_alg, hashlen, hash, dst_len, sig);
    }
#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    else if (padding == ESP_RSA_DS_PADDING_PSS) {
        return esp_rsa_ds_pad_v21_encode(hash_alg, hashlen, hash, saltlen, sig, dst_len);
    }
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */
    else {
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

/* Lock for the DS session, other TLS connections trying to use the DS peripheral will be blocked
 * till this DS session is completed (i.e. TLS handshake for this connection is completed) */
static void __attribute__((constructor)) esp_rsa_ds_conn_lock(void)
{
    if ((s_ds_lock = xSemaphoreCreateMutex()) == NULL) {
        ESP_EARLY_LOGE(TAG, "Failed to create DS lock");
    }
}

void esp_rsa_ds_release_ds_lock(void)
{
    if (s_ds_lock == NULL) {
        return;
    }
    if (xSemaphoreGetMutexHolder(s_ds_lock) == xTaskGetCurrentTaskHandle()) {
        /* Give back the semaphore (DS lock) */
        xSemaphoreGive(s_ds_lock);
    }
}

static int esp_rsa_ds_validate_opaque_key(const esp_rsa_ds_opaque_key_t *opaque_key)
{
    if (opaque_key == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (opaque_key->ds_data_ctx == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (opaque_key->ds_data_ctx->esp_ds_data == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (opaque_key->ds_data_ctx->rsa_length_bits % 32 != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (opaque_key->ds_data_ctx->rsa_length_bits < 1024 || opaque_key->ds_data_ctx->rsa_length_bits > SOC_DS_SIGNATURE_MAX_BIT_LEN) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* DS data rsa_length must match rsa_length_bits so we can use the key's data directly in sign operations */
    if (opaque_key->ds_data_ctx->esp_ds_data->rsa_length != (opaque_key->ds_data_ctx->rsa_length_bits / 32) - 1) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

#if SOC_KEY_MANAGER_SUPPORTED
    if (!opaque_key->key_recovery_info) {
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        if ((opaque_key->ds_data_ctx->efuse_key_id + EFUSE_BLK_KEY0) >= EFUSE_BLK_KEY_MAX) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        esp_efuse_purpose_t purpose = esp_efuse_get_key_purpose(EFUSE_BLK_KEY0 + opaque_key->ds_data_ctx->efuse_key_id);
        if (purpose != ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE) {
            return PSA_ERROR_NOT_PERMITTED;
        }
#if SOC_KEY_MANAGER_SUPPORTED
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    return PSA_SUCCESS;
}

psa_status_t esp_rsa_ds_opaque_sign_hash_start(
    esp_rsa_ds_opaque_sign_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length)
{
    esp_err_t err = ESP_FAIL;

    if (!attributes || !key_buffer || !hash || !operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < sizeof(esp_rsa_ds_opaque_key_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) && !PSA_ALG_IS_RSA_PSS(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    operation->alg = alg;

    const esp_rsa_ds_opaque_key_t *opaque_key = (const esp_rsa_ds_opaque_key_t *)key_buffer;
    operation->esp_rsa_ds_opaque_key = opaque_key;

    if (esp_rsa_ds_validate_opaque_key(opaque_key) != PSA_SUCCESS) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((xSemaphoreTake(s_ds_lock, s_timeout_ms / portTICK_PERIOD_MS) != pdTRUE)) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    esp_rsa_ds_padding_t padding = ESP_RSA_DS_PADDING_INVALID;
    if (PSA_ALG_IS_RSA_PSS(operation->alg)) {
        padding = ESP_RSA_DS_PADDING_PSS;
    } else if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(operation->alg)) {
        padding = ESP_RSA_DS_PADDING_PKCS_V15;
    }

    psa_algorithm_t hash_alg = PSA_ALG_SIGN_GET_HASH(operation->alg);

    const size_t words_len = (opaque_key->ds_data_ctx->rsa_length_bits / 32);
    const size_t rsa_len_bytes = words_len * 4;
    operation->sig_buffer_size = rsa_len_bytes;
    operation->sig_buffer = NULL;

    unsigned char *em = heap_caps_malloc_prefer(rsa_len_bytes, 1, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    if (em == NULL) {
        esp_rsa_ds_release_ds_lock();
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    psa_status_t status = esp_rsa_ds_pad(
        padding, hash_alg, hash_length, hash, -1, em, rsa_len_bytes);
    if (status != PSA_SUCCESS) {
        goto error;
    }

    operation->sig_buffer = heap_caps_malloc_prefer(rsa_len_bytes, 2, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    if (operation->sig_buffer == NULL) {
        status = PSA_ERROR_INSUFFICIENT_MEMORY;
        goto error;
    }

    uint32_t *sig_words = (uint32_t *)operation->sig_buffer;
    const uint32_t *em_words = (const uint32_t *)em;
    for (unsigned int i = 0; i < words_len; i++) {
        sig_words[i] = SWAP_INT32(em_words[words_len - (i + 1)]);
    }

    hmac_key_id_t hmac_key_id = opaque_key->ds_data_ctx->efuse_key_id;

#if SOC_KEY_MANAGER_SUPPORTED
    esp_key_mgr_key_recovery_info_t *km_key_recovery_info = operation->esp_rsa_ds_opaque_key->key_recovery_info;
    if (km_key_recovery_info) {
        err = esp_key_mgr_activate_key(km_key_recovery_info);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to activate key: 0x%x", err);
            status = PSA_ERROR_INVALID_HANDLE;
            goto error;
        }
        hmac_key_id = HMAC_KEY_KM;
        operation->is_km_key_active = true;
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    err = esp_ds_start_sign((const void *)operation->sig_buffer,
                            opaque_key->ds_data_ctx->esp_ds_data,
                            hmac_key_id,
                            &operation->esp_rsa_ds_ctx);
    if (err != ESP_OK) {
        status = PSA_ERROR_GENERIC_ERROR;
        goto error;
    }

error:
    if (em) {
        heap_caps_free(em);
        em = NULL;
    }

    return status;
}

psa_status_t esp_rsa_ds_opaque_sign_hash_complete(
    esp_rsa_ds_opaque_sign_hash_operation_t *operation,
    uint8_t *signature, size_t signature_size,
    size_t *signature_length)
{
    if (!operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!signature || !signature_length || !operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((operation->esp_rsa_ds_ctx == NULL) || (operation->sig_buffer == NULL)) {
        return PSA_ERROR_BAD_STATE;
    }

    int expected_signature_size = operation->esp_rsa_ds_opaque_key->ds_data_ctx->rsa_length_bits / 8;
    if (signature_size < expected_signature_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    esp_err_t err = esp_ds_finish_sign((void *)operation->sig_buffer, operation->esp_rsa_ds_ctx);
    operation->esp_rsa_ds_ctx = NULL;
    if (err != ESP_OK) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    unsigned int words_len = expected_signature_size / 4;
    uint32_t *out_words = (uint32_t *)signature;
    const uint32_t *ds_words = (const uint32_t *)operation->sig_buffer;
    for (unsigned int i = 0; i < words_len; i++) {
        out_words[i] = SWAP_INT32(ds_words[words_len - (i + 1)]);
    }

    *signature_length = expected_signature_size;

    esp_rsa_ds_release_ds_lock();

    if (operation->sig_buffer) {
        memset(operation->sig_buffer, 0, operation->sig_buffer_size);
        heap_caps_free(operation->sig_buffer);
        operation->sig_buffer = NULL;
    }

    return PSA_SUCCESS;
}

psa_status_t esp_rsa_ds_opaque_sign_hash_abort(
    esp_rsa_ds_opaque_sign_hash_operation_t *operation)
{
    if (!operation) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (operation->esp_rsa_ds_opaque_key) {
#if SOC_KEY_MANAGER_SUPPORTED
        esp_key_mgr_key_recovery_info_t *km_key_recovery_info = operation->esp_rsa_ds_opaque_key->key_recovery_info;
        if (km_key_recovery_info && operation->is_km_key_active) {
            esp_key_mgr_deactivate_key(km_key_recovery_info->key_type);
            operation->is_km_key_active = false;
        }
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        operation->esp_rsa_ds_opaque_key = NULL;
    }

    if (operation->esp_rsa_ds_ctx) {
#if !ESP_TEE_BUILD
        free(operation->esp_rsa_ds_ctx);
#endif
        operation->esp_rsa_ds_ctx = NULL;
    }

    // Free allocated memory if exists
    if (operation->sig_buffer) {
        memset(operation->sig_buffer, 0, operation->sig_buffer_size);
        heap_caps_free(operation->sig_buffer);
        operation->sig_buffer = NULL;
    }

    // Release the DS lock if held
    esp_rsa_ds_release_ds_lock();

    // Clear the operation structure
    memset(operation, 0, sizeof(esp_rsa_ds_opaque_sign_hash_operation_t));

    return PSA_SUCCESS;
}

psa_status_t esp_rsa_ds_opaque_signature_sign_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    uint8_t *signature,
    size_t signature_size,
    size_t *signature_length
)
{
    esp_rsa_ds_opaque_sign_hash_operation_t operation = {0};
    psa_status_t status = esp_rsa_ds_opaque_sign_hash_start(
        &operation,
        attributes,
        key_buffer,
        key_buffer_size,
        alg,
        hash,
        hash_length
    );
    if (status != PSA_SUCCESS) {
        esp_rsa_ds_opaque_sign_hash_abort(&operation);
        return status;
    }

    status = esp_rsa_ds_opaque_sign_hash_complete(
        &operation,
        signature,
        signature_size,
        signature_length
    );

    esp_rsa_ds_opaque_sign_hash_abort(&operation);
    return status;
}

psa_status_t esp_rsa_ds_opaque_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits)
{
    if (!attributes || !data || data_length < sizeof(esp_rsa_ds_opaque_key_t) || !key_buffer || !key_buffer_length || !bits) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < sizeof(esp_rsa_ds_opaque_key_t)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    const esp_rsa_ds_opaque_key_t *opaque_key = (const esp_rsa_ds_opaque_key_t *)data;
    int ret = esp_rsa_ds_validate_opaque_key(opaque_key);
    if (ret != PSA_SUCCESS) {
        return ret;
    }

    /* Shallow copy: key buffer holds the context; esp_ds_data points to the caller's data.
     * The key material (esp_rsa_ds_opaque_key_t and the esp_ds_data_t it points to) must remain
     * valid until psa_destroy_key() is called on this key. */
    memcpy(key_buffer, opaque_key, sizeof(esp_rsa_ds_opaque_key_t));
    *key_buffer_length = sizeof(esp_rsa_ds_opaque_key_t);
    *bits = opaque_key->ds_data_ctx->rsa_length_bits;
    return PSA_SUCCESS;
}

size_t esp_rsa_ds_opaque_size_function(
    psa_key_type_t key_type,
    size_t key_bits)
{
    (void)key_type;
    (void)key_bits;

    return sizeof(esp_rsa_ds_opaque_key_t);
}

void esp_rsa_ds_opaque_set_session_timeout(int timeout_ms)
{
    /* add additional offset of ESP_RSA_DS_TIMEOUT_BUFFER_MS ms to have enough time for deleting the TLS connection
     * and free the previous ds context after exceeding timeout value
     * (this offset also helps when timeout is set to 0)
     */
    if (timeout_ms < 0) {
        return;
    }
    if (timeout_ms > s_timeout_ms) {
        s_timeout_ms = timeout_ms + ESP_RSA_DS_TIMEOUT_BUFFER_MS;
    }
}

psa_status_t esp_rsa_ds_opaque_asymmetric_decrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key,
    size_t key_length, psa_algorithm_t alg, const uint8_t *input,
    size_t input_length, const uint8_t *salt, size_t salt_length,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    (void)salt;
    (void)salt_length;

    esp_err_t err = ESP_FAIL;

    if (!attributes || !key || key_length < sizeof(esp_rsa_ds_opaque_key_t) ||
        !input || input_length < 1 || !output || !output_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_RSA_OAEP(alg) && alg != PSA_ALG_RSA_PKCS1V15_CRYPT) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    const esp_rsa_ds_opaque_key_t *opaque_key = (const esp_rsa_ds_opaque_key_t *)key;

    if (esp_rsa_ds_validate_opaque_key(opaque_key) != PSA_SUCCESS) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t key_bits = opaque_key->ds_data_ctx->rsa_length_bits;
    if (input_length != (key_bits / 8)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    esp_rsa_ds_padding_t padding = ESP_RSA_DS_PADDING_INVALID;
    if (PSA_ALG_IS_RSA_OAEP(alg)) {
        padding = ESP_RSA_DS_PADDING_OAEP;
    } else if (alg == PSA_ALG_RSA_PKCS1V15_CRYPT) {
        padding = ESP_RSA_DS_PADDING_PKCS_V15;
    }

    if (xSemaphoreTake(s_ds_lock, s_timeout_ms / portTICK_PERIOD_MS) != pdTRUE) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    size_t ilen = key_bits / 8;
    size_t data_len = ilen / 4;
    uint32_t *em_words = heap_caps_malloc_prefer(sizeof(uint32_t) * data_len, 1, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    if (em_words == NULL) {
        esp_rsa_ds_release_ds_lock();
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    for (unsigned int i = 0; i < (data_len); i++) {
        em_words[i] = SWAP_INT32(((uint32_t *)input)[(data_len) - (i + 1)]);
    }

    esp_rsa_ds_opaque_sign_hash_operation_t operation = {0};
    operation.alg = alg;
    operation.esp_rsa_ds_opaque_key = opaque_key;
    operation.sig_buffer = em_words;

    hmac_key_id_t hmac_key_id = opaque_key->ds_data_ctx->efuse_key_id;
#if SOC_KEY_MANAGER_SUPPORTED
    esp_key_mgr_key_recovery_info_t *km_key_recovery_info = opaque_key->key_recovery_info;
    if (km_key_recovery_info) {
        err = esp_key_mgr_activate_key(km_key_recovery_info);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to activate key: 0x%x", err);
            heap_caps_free(em_words);
            esp_rsa_ds_release_ds_lock();
            return PSA_ERROR_INVALID_HANDLE;
        }
        hmac_key_id = HMAC_KEY_KM;
        operation.is_km_key_active = true;
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    err = esp_ds_start_sign((const void *)em_words,
                            opaque_key->ds_data_ctx->esp_ds_data,
                            hmac_key_id,
                            &operation.esp_rsa_ds_ctx);
    if (err != ESP_OK) {
        heap_caps_free(em_words);
#if SOC_KEY_MANAGER_SUPPORTED
    if (km_key_recovery_info && operation.is_km_key_active) {
        esp_key_mgr_deactivate_key(km_key_recovery_info->key_type);
        operation.is_km_key_active = false;
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        esp_rsa_ds_release_ds_lock();
        return PSA_ERROR_GENERIC_ERROR;
    }

    err = esp_ds_finish_sign((void *)em_words, operation.esp_rsa_ds_ctx);

#if SOC_KEY_MANAGER_SUPPORTED
    if (km_key_recovery_info && operation.is_km_key_active) {
        esp_key_mgr_deactivate_key(km_key_recovery_info->key_type);
        operation.is_km_key_active = false;
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    if (err != ESP_OK) {
        heap_caps_free(em_words);
        esp_rsa_ds_release_ds_lock();
        return PSA_ERROR_GENERIC_ERROR;
    }

    esp_rsa_ds_release_ds_lock();

    // Remove padding
    uint32_t *out_tmp = heap_caps_malloc_prefer(sizeof(uint32_t) * data_len, 1, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    if (out_tmp == NULL) {
        memset(em_words, 0, sizeof(uint32_t) * data_len);
        heap_caps_free(em_words);
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    for (unsigned int i = 0; i < (data_len); i++) {
        ((uint32_t *)out_tmp)[i] = SWAP_INT32(em_words[(data_len) - (i + 1)]);
    }

    size_t unpadded_len = 0;
    psa_status_t ret = PSA_ERROR_NOT_SUPPORTED;
    if (padding == ESP_RSA_DS_PADDING_PKCS_V15) {
        ret = esp_rsa_ds_pad_v15_unpad((unsigned char *)out_tmp, ilen, (unsigned char *)out_tmp, ilen, &unpadded_len);
    }
#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    else if (padding == ESP_RSA_DS_PADDING_OAEP) {
        ret = esp_rsa_ds_pad_oaep_unpad((unsigned char *)out_tmp, ilen, (unsigned char *)out_tmp, ilen, &unpadded_len, PSA_ALG_RSA_OAEP_GET_HASH(alg));
    }
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */
    else {
        ret = PSA_ERROR_NOT_SUPPORTED;
        goto exit;
    }

    if (ret != PSA_SUCCESS) {
        ret = PSA_ERROR_INVALID_PADDING;
        goto exit;
    }

    if (output_size < unpadded_len) {
        ret = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }

    memcpy(output, out_tmp, unpadded_len);
    *output_length = unpadded_len;
    ret = PSA_SUCCESS;

exit:
    memset(em_words, 0, sizeof(uint32_t) * data_len);
    memset(out_tmp, 0, sizeof(uint32_t) * data_len);
    heap_caps_free(em_words);
    heap_caps_free(out_tmp);
    return ret;
}
