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
#include "psa_crypto_driver_esp_opaque_common.h"
#include "include/psa_crypto_driver_esp_rsa_ds_utilities.h"

#include "esp_log.h"
#include "esp_efuse.h"
#include "esp_assert.h"
#include "soc/soc_caps.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include "esp_key_mgr.h"
#endif /* SOC_KEY_MANAGER_SUPPORTED */

#define ESP_RSA_DS_TIMEOUT_BUFFER_MS 1000

static const char *TAG = "PSA_RSA_DS";

static SemaphoreHandle_t s_ds_lock = NULL;
static int s_timeout_ms = 0;

/*
 * Per-source storage structs — internal to the driver.
 * These are what actually get persisted to NVS, not the user-facing esp_rsa_ds_opaque_key_t.
 *
 * All storage structs share a common prefix: [version][key_source]
 * so that the driver can identify the key source at operation time.
 */
typedef enum {
    ESP_RSA_DS_KEY_STORAGE_VERSION_INVALID = 0,
    ESP_RSA_DS_KEY_STORAGE_VERSION_V1 = 1,
    ESP_RSA_DS_KEY_STORAGE_VERSION_MAX = 2,
} esp_rsa_ds_key_storage_version_t;

typedef enum {
    ESP_RSA_DS_KEY_SOURCE_EFUSE = 0,
    ESP_RSA_DS_KEY_SOURCE_KEY_MGR = 1,
} esp_rsa_ds_key_source_t;

/* Storage structs use uint8_t for key_source instead of enum
 * to ensure stable serialized size across compilers. */

typedef struct __attribute__((packed)) {
    uint8_t version;
    uint8_t key_source;             /* esp_rsa_ds_key_source_t */
} esp_rsa_ds_common_key_storage_metadata_t;

ESP_STATIC_ASSERT(sizeof(esp_rsa_ds_common_key_storage_metadata_t) == 2 * sizeof(uint8_t),
                  "esp_rsa_ds_common_key_storage_metadata_t must be exactly 2 bytes");

/* esp_ds_data_t is serialised verbatim into NVS as part of the persistent storage structs,
 * and its first field is an enum (esp_digital_signature_length_t). esp_ds.h documents that
 * "in IDF, the enum type length is the same as of type unsigned" — assert it here so a
 * future toolchain change that shrinks enums fails the build instead of silently making
 * existing stored DS keys unreadable. */
ESP_STATIC_ASSERT(sizeof(esp_digital_signature_length_t) == sizeof(unsigned),
                  "esp_digital_signature_length_t must be sized as unsigned for stable NVS layout of esp_ds_data_t");

/**
 * Pointer-based storage for volatile keys.
 * The caller must keep all referenced data valid until psa_destroy_key().
 * This avoids deep-copying esp_ds_data_t (~1600 bytes), preserving the
 * heap savings of mmap'd flash data from esp_secure_cert_mgr.
 */
typedef struct {
    esp_rsa_ds_common_key_storage_metadata_t metadata;
    esp_ds_data_ctx_t *ds_data_ctx;
#if SOC_KEY_MANAGER_SUPPORTED
    esp_key_mgr_key_recovery_info_t *key_recovery_info;
#endif /* SOC_KEY_MANAGER_SUPPORTED */
} esp_rsa_ds_volatile_key_storage_t;

/**
 * Inline storage for persistent eFuse DS keys.
 * All data is deep-copied — no external references.
 */
typedef struct {
    esp_rsa_ds_common_key_storage_metadata_t metadata;
    uint8_t efuse_key_id;
    uint8_t reserved;               /* explicit padding */
    uint16_t rsa_length_bits;
    uint8_t reserved2[2];           /* explicit padding for ds_data 4-byte alignment */
    esp_ds_data_t ds_data;
} esp_rsa_ds_efuse_key_storage_t;

ESP_STATIC_ASSERT(offsetof(esp_rsa_ds_efuse_key_storage_t, ds_data) % 4 == 0,
                  "ds_data must be 4-byte aligned in esp_rsa_ds_efuse_key_storage_t");

#if SOC_KEY_MANAGER_SUPPORTED
/**
 * Inline storage for persistent Key Manager DS keys.
 * All data is deep-copied — no external references.
 */
typedef struct {
    esp_rsa_ds_common_key_storage_metadata_t metadata;
    uint16_t rsa_length_bits;
    esp_ds_data_t ds_data;
    esp_key_mgr_key_recovery_info_t key_recovery_info;
} esp_rsa_ds_km_key_storage_t;

ESP_STATIC_ASSERT(offsetof(esp_rsa_ds_km_key_storage_t, ds_data) % 4 == 0,
                  "ds_data must be 4-byte aligned in esp_rsa_ds_km_key_storage_t");
ESP_STATIC_ASSERT(offsetof(esp_rsa_ds_km_key_storage_t, key_recovery_info) % 4 == 0,
                  "key_recovery_info must be 4-byte aligned in esp_rsa_ds_km_key_storage_t");
#endif /* SOC_KEY_MANAGER_SUPPORTED */

/**
 * @brief Read the key_source tag from any storage struct.
 *
 * All storage structs share the layout: [version(1)][key_source(1)][...]
 */
static inline esp_rsa_ds_key_source_t rsa_ds_storage_get_key_source(const uint8_t *key_buffer)
{
    return (esp_rsa_ds_key_source_t)key_buffer[1];
}

/**
 * @brief Calculate the storage buffer size for an import operation.
 *
 * For volatile keys: returns pointer-based storage size (small).
 * For persistent keys: inspects the user struct to determine key source
 * and returns the corresponding inline storage struct size (large).
 */
static size_t esp_rsa_ds_get_storage_size(const esp_rsa_ds_opaque_key_t *key, bool persistent)
{
    if (!persistent) {
        return sizeof(esp_rsa_ds_volatile_key_storage_t);
    }

#if SOC_KEY_MANAGER_SUPPORTED
    if (key->key_recovery_info) {
        return sizeof(esp_rsa_ds_km_key_storage_t);
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    (void)key;
    return sizeof(esp_rsa_ds_efuse_key_storage_t);
}

/**
 * @brief Calculate the expected storage size from an already-serialized storage buffer.
 *
 * @param key_buffer        The storage buffer.
 * @param key_buffer_size   Size of @p key_buffer in bytes.
 * @param persistent        true if the key is persistent (inline data), false if volatile.
 * @param[out] expected_storage_size  The expected minimum buffer size.
 */
static psa_status_t esp_rsa_ds_get_expected_storage_size(const uint8_t *key_buffer,
                                                         size_t key_buffer_size,
                                                         bool persistent,
                                                         size_t *expected_storage_size)
{
    if (key_buffer_size < sizeof(esp_rsa_ds_common_key_storage_metadata_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer[0] == ESP_RSA_DS_KEY_STORAGE_VERSION_INVALID || key_buffer[0] >= ESP_RSA_DS_KEY_STORAGE_VERSION_MAX) {
        return PSA_ERROR_DATA_INVALID;
    }

    *expected_storage_size = 0;

    if (!persistent) {
        *expected_storage_size = sizeof(esp_rsa_ds_volatile_key_storage_t);
        return PSA_SUCCESS;
    }

    esp_rsa_ds_key_source_t key_source = rsa_ds_storage_get_key_source(key_buffer);

    switch (key_source) {
#if SOC_KEY_MANAGER_SUPPORTED
        case ESP_RSA_DS_KEY_SOURCE_KEY_MGR:
            *expected_storage_size = sizeof(esp_rsa_ds_km_key_storage_t);
            return PSA_SUCCESS;
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        case ESP_RSA_DS_KEY_SOURCE_EFUSE:
            *expected_storage_size = sizeof(esp_rsa_ds_efuse_key_storage_t);
            return PSA_SUCCESS;
        default:
            return PSA_ERROR_DATA_INVALID;
    }
}

/**
 * @brief Validate a storage buffer and extract the per-source DS parameters.
 *
 * Centralises the metadata + size validation and the per-source pointer/value extraction
 * shared by the sign-hash and asymmetric-decrypt entry points. On success, all out-params
 * are filled. On failure, out-params are left untouched.
 *
 * @param key_buffer        The PSA storage buffer.
 * @param key_buffer_size   Size of @p key_buffer in bytes.
 * @param is_persistent     Persistence flag derived from the PSA key attributes.
 * @param[out] key_source   Storage key source tag (eFuse / Key Manager).
 * @param[out] rsa_length_bits  RSA key length in bits, taken from the storage struct.
 * @param[out] ds_data      Pointer to the in-storage @c esp_ds_data_t (volatile: caller's
 *                          mmap'd flash buffer; persistent: the inline copy).
 * @param[out] hmac_key_id  HMAC key id for the DS peripheral.
 * @param[out] km_ri        Pointer to the @c esp_key_mgr_key_recovery_info_t to pass to
 *                          @c esp_key_mgr_activate_key, or NULL when the key source is
 *                          not Key Manager. Only present on KM-capable SoCs.
 *
 * @return PSA_SUCCESS, or a PSA error if validation fails.
 */
static psa_status_t esp_rsa_ds_extract_storage(
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    bool is_persistent,
    esp_rsa_ds_key_source_t *key_source,
    uint16_t *rsa_length_bits,
    const esp_ds_data_t **ds_data,
    hmac_key_id_t *hmac_key_id
#if SOC_KEY_MANAGER_SUPPORTED
    , esp_key_mgr_key_recovery_info_t **km_ri
#endif /* SOC_KEY_MANAGER_SUPPORTED */
)
{
    size_t expected_storage_size = 0;
    psa_status_t status = esp_rsa_ds_get_expected_storage_size(key_buffer, key_buffer_size,
                                                               is_persistent, &expected_storage_size);
    if (status != PSA_SUCCESS) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_buffer_size < expected_storage_size) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    esp_rsa_ds_key_source_t src = rsa_ds_storage_get_key_source(key_buffer);
    uint16_t bits = 0;
    const esp_ds_data_t *data = NULL;
    hmac_key_id_t hmac_id = 0;
#if SOC_KEY_MANAGER_SUPPORTED
    esp_key_mgr_key_recovery_info_t *ri = NULL;
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    if (!is_persistent) {
        const esp_rsa_ds_volatile_key_storage_t *ptr_st =
            (const esp_rsa_ds_volatile_key_storage_t *)key_buffer;
        bits = ptr_st->ds_data_ctx->rsa_length_bits;
        data = ptr_st->ds_data_ctx->esp_ds_data;
        hmac_id = ptr_st->ds_data_ctx->efuse_key_id;
#if SOC_KEY_MANAGER_SUPPORTED
        if (src == ESP_RSA_DS_KEY_SOURCE_KEY_MGR) {
            hmac_id = HMAC_KEY_KM;
            ri = ptr_st->key_recovery_info;
        }
#endif /* SOC_KEY_MANAGER_SUPPORTED */
    } else {
        switch (src) {
            case ESP_RSA_DS_KEY_SOURCE_EFUSE: {
                const esp_rsa_ds_efuse_key_storage_t *efuse_st =
                    (const esp_rsa_ds_efuse_key_storage_t *)key_buffer;
                bits = efuse_st->rsa_length_bits;
                data = &efuse_st->ds_data;
                hmac_id = efuse_st->efuse_key_id;
                break;
            }
#if SOC_KEY_MANAGER_SUPPORTED
            case ESP_RSA_DS_KEY_SOURCE_KEY_MGR: {
                const esp_rsa_ds_km_key_storage_t *km_st =
                    (const esp_rsa_ds_km_key_storage_t *)key_buffer;
                bits = km_st->rsa_length_bits;
                data = &km_st->ds_data;
                hmac_id = HMAC_KEY_KM;
                /* esp_key_mgr_activate_key() takes a non-const pointer for API compatibility
                 * but does not modify the recovery info. The cast away const is therefore safe;
                 * if that contract ever changes, copy the recovery info to a local first. */
                ri = (esp_key_mgr_key_recovery_info_t *)&km_st->key_recovery_info;
                break;
            }
#endif /* SOC_KEY_MANAGER_SUPPORTED */
            default:
                return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    if (bits % 32 != 0 || bits < 1024 || bits > SOC_DS_SIGNATURE_MAX_BIT_LEN) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (data->rsa_length != (bits / 32) - 1) {
        return PSA_ERROR_DATA_INVALID;
    }

    *key_source = src;
    *rsa_length_bits = bits;
    *ds_data = data;
    *hmac_key_id = hmac_id;
#if SOC_KEY_MANAGER_SUPPORTED
    *km_ri = ri;
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    return PSA_SUCCESS;
}

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

    bool is_persistent = esp_opaque_key_is_persistent(attributes);

    if (!PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) && !PSA_ALG_IS_RSA_PSS(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    esp_rsa_ds_key_source_t key_source = ESP_RSA_DS_KEY_SOURCE_EFUSE;
    uint16_t rsa_length_bits = 0;
    const esp_ds_data_t *ds_data = NULL;
    hmac_key_id_t hmac_key_id = 0;
#if SOC_KEY_MANAGER_SUPPORTED
    esp_key_mgr_key_recovery_info_t *km_ri = NULL;
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    psa_status_t status = esp_rsa_ds_extract_storage(
        key_buffer, key_buffer_size, is_persistent,
        &key_source, &rsa_length_bits, &ds_data, &hmac_key_id
#if SOC_KEY_MANAGER_SUPPORTED
        , &km_ri
#endif /* SOC_KEY_MANAGER_SUPPORTED */
    );
    if (status != PSA_SUCCESS) {
        return status;
    }

    operation->alg = alg;
    operation->key_buffer = key_buffer;

    if ((xSemaphoreTake(s_ds_lock, s_timeout_ms / portTICK_PERIOD_MS) != pdTRUE)) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    esp_rsa_ds_padding_t padding = ESP_RSA_DS_PADDING_INVALID;
    if (PSA_ALG_IS_RSA_PSS(alg)) {
        padding = ESP_RSA_DS_PADDING_PSS;
    } else if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg)) {
        padding = ESP_RSA_DS_PADDING_PKCS_V15;
    }

    psa_algorithm_t hash_alg = PSA_ALG_SIGN_GET_HASH(alg);

    const size_t words_len = rsa_length_bits / 32;
    const size_t rsa_len_bytes = words_len * 4;
    operation->sig_buffer_size = rsa_len_bytes;
    operation->sig_buffer = NULL;

    unsigned char *em = heap_caps_malloc_prefer(rsa_len_bytes, 1, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    if (em == NULL) {
        esp_rsa_ds_release_ds_lock();
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    status = esp_rsa_ds_pad(
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

#if SOC_KEY_MANAGER_SUPPORTED
    if (key_source == ESP_RSA_DS_KEY_SOURCE_KEY_MGR) {
        err = esp_key_mgr_activate_key(km_ri);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to activate key: 0x%x", err);
            status = PSA_ERROR_INVALID_HANDLE;
            goto error;
        }
        operation->is_km_key_active = true;
        operation->km_ri = km_ri;
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    err = esp_ds_start_sign((const void *)operation->sig_buffer,
                            ds_data,
                            hmac_key_id,
                            &operation->esp_rsa_ds_ctx);
    if (err != ESP_OK) {
        status = PSA_ERROR_GENERIC_ERROR;
        goto error;
    }

error:
    if (em) {
        memset(em, 0, rsa_len_bytes);
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

    size_t expected_signature_size = operation->sig_buffer_size;
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

#if SOC_KEY_MANAGER_SUPPORTED
    if (operation->is_km_key_active) {
        esp_key_mgr_deactivate_key(operation->km_ri->key_type);
        operation->is_km_key_active = false;
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    if (operation->key_buffer) {
        operation->key_buffer = NULL;
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

    const esp_rsa_ds_opaque_key_t *opaque_key = (const esp_rsa_ds_opaque_key_t *)data;
    int ret = esp_rsa_ds_validate_opaque_key(opaque_key);
    if (ret != PSA_SUCCESS) {
        return ret;
    }

    bool is_persistent = esp_opaque_key_is_persistent(attributes);

    esp_rsa_ds_key_source_t key_source = ESP_RSA_DS_KEY_SOURCE_EFUSE;
#if SOC_KEY_MANAGER_SUPPORTED
    if (opaque_key->key_recovery_info) {
        key_source = ESP_RSA_DS_KEY_SOURCE_KEY_MGR;
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    if (!is_persistent) {
        /* Volatile: store pointers only — caller keeps data alive.
         * This preserves the heap savings of mmap'd flash data from esp_secure_cert_mgr. */
        if (key_buffer_size < sizeof(esp_rsa_ds_volatile_key_storage_t)) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        esp_rsa_ds_volatile_key_storage_t *storage = (esp_rsa_ds_volatile_key_storage_t *)key_buffer;
        storage->metadata.version = ESP_RSA_DS_KEY_STORAGE_VERSION_V1;
        storage->metadata.key_source = key_source;
        storage->ds_data_ctx = opaque_key->ds_data_ctx;
#if SOC_KEY_MANAGER_SUPPORTED
        storage->key_recovery_info = opaque_key->key_recovery_info;
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        *key_buffer_length = sizeof(esp_rsa_ds_volatile_key_storage_t);
    } else {
        /* Persistent: deep-copy all data into self-contained storage struct */
#if SOC_KEY_MANAGER_SUPPORTED
        if (key_source == ESP_RSA_DS_KEY_SOURCE_KEY_MGR) {
            if (key_buffer_size < sizeof(esp_rsa_ds_km_key_storage_t)) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            esp_rsa_ds_km_key_storage_t *storage = (esp_rsa_ds_km_key_storage_t *)key_buffer;
            storage->metadata.version = ESP_RSA_DS_KEY_STORAGE_VERSION_V1;
            storage->metadata.key_source = ESP_RSA_DS_KEY_SOURCE_KEY_MGR;
            storage->rsa_length_bits = opaque_key->ds_data_ctx->rsa_length_bits;
            memcpy(&storage->ds_data, opaque_key->ds_data_ctx->esp_ds_data, sizeof(esp_ds_data_t));
            memcpy(&storage->key_recovery_info, opaque_key->key_recovery_info,
                   sizeof(esp_key_mgr_key_recovery_info_t));
            *key_buffer_length = sizeof(esp_rsa_ds_km_key_storage_t);
        } else
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        {
            if (key_buffer_size < sizeof(esp_rsa_ds_efuse_key_storage_t)) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            esp_rsa_ds_efuse_key_storage_t *storage = (esp_rsa_ds_efuse_key_storage_t *)key_buffer;
            storage->metadata.version = ESP_RSA_DS_KEY_STORAGE_VERSION_V1;
            storage->metadata.key_source = ESP_RSA_DS_KEY_SOURCE_EFUSE;
            storage->efuse_key_id = opaque_key->ds_data_ctx->efuse_key_id;
            storage->reserved = 0;
            storage->rsa_length_bits = opaque_key->ds_data_ctx->rsa_length_bits;
            memset(storage->reserved2, 0, sizeof(storage->reserved2));
            memcpy(&storage->ds_data, opaque_key->ds_data_ctx->esp_ds_data, sizeof(esp_ds_data_t));
            *key_buffer_length = sizeof(esp_rsa_ds_efuse_key_storage_t);
        }
    }

    *bits = opaque_key->ds_data_ctx->rsa_length_bits;
    return PSA_SUCCESS;
}

size_t esp_rsa_ds_opaque_size_function(
    const psa_key_attributes_t *attributes,
    psa_key_type_t key_type,
    const uint8_t *data,
    size_t data_length)
{
    (void)key_type;

    bool is_persistent = esp_opaque_key_is_persistent(attributes);

    if (!data || data_length < sizeof(esp_rsa_ds_opaque_key_t)) {
        /* Data too short to inspect the user struct — return the largest possible
         * size for the persistence flavor so import has enough room to write whichever
         * variant ends up being needed. import_key() does the real validation. */
        if (!is_persistent) {
            return sizeof(esp_rsa_ds_volatile_key_storage_t);
        }
#if SOC_KEY_MANAGER_SUPPORTED
        return sizeof(esp_rsa_ds_km_key_storage_t);
#else
        return sizeof(esp_rsa_ds_efuse_key_storage_t);
#endif /* SOC_KEY_MANAGER_SUPPORTED */
    }

    return esp_rsa_ds_get_storage_size((const esp_rsa_ds_opaque_key_t *)data, is_persistent);
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

    if (!attributes || !key || !input || input_length < 1 || !output || !output_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_RSA_OAEP(alg) && alg != PSA_ALG_RSA_PKCS1V15_CRYPT) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    bool is_persistent = esp_opaque_key_is_persistent(attributes);

    esp_rsa_ds_key_source_t key_source = ESP_RSA_DS_KEY_SOURCE_EFUSE;
    uint16_t rsa_length_bits = 0;
    const esp_ds_data_t *ds_data = NULL;
    hmac_key_id_t hmac_key_id = 0;
#if SOC_KEY_MANAGER_SUPPORTED
    esp_key_mgr_key_recovery_info_t *km_ri = NULL;
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    psa_status_t status = esp_rsa_ds_extract_storage(
        key, key_length, is_persistent,
        &key_source, &rsa_length_bits, &ds_data, &hmac_key_id
#if SOC_KEY_MANAGER_SUPPORTED
        , &km_ri
#endif /* SOC_KEY_MANAGER_SUPPORTED */
    );
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (input_length != (rsa_length_bits / 8)) {
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

    size_t ilen = rsa_length_bits / 8;
    size_t data_len = ilen / 4;
    uint32_t *em_words = heap_caps_malloc_prefer(sizeof(uint32_t) * data_len, 1, MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    if (em_words == NULL) {
        esp_rsa_ds_release_ds_lock();
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    for (unsigned int i = 0; i < (data_len); i++) {
        em_words[i] = SWAP_INT32(((uint32_t *)input)[(data_len) - (i + 1)]);
    }

#if SOC_KEY_MANAGER_SUPPORTED
    bool is_km_key_active = false;
    if (key_source == ESP_RSA_DS_KEY_SOURCE_KEY_MGR) {
        err = esp_key_mgr_activate_key(km_ri);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to activate key: 0x%x", err);
            memset(em_words, 0, sizeof(uint32_t) * data_len);
            heap_caps_free(em_words);
            esp_rsa_ds_release_ds_lock();
            return PSA_ERROR_INVALID_HANDLE;
        }
        is_km_key_active = true;
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    esp_ds_context_t *ds_ctx = NULL;
    err = esp_ds_start_sign((const void *)em_words,
                            ds_data,
                            hmac_key_id,
                            &ds_ctx);
    if (err != ESP_OK) {
        memset(em_words, 0, sizeof(uint32_t) * data_len);
        heap_caps_free(em_words);
#if SOC_KEY_MANAGER_SUPPORTED
        if (is_km_key_active) {
            esp_key_mgr_deactivate_key(km_ri->key_type);
        }
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        esp_rsa_ds_release_ds_lock();
        return PSA_ERROR_GENERIC_ERROR;
    }

    err = esp_ds_finish_sign((void *)em_words, ds_ctx);

#if SOC_KEY_MANAGER_SUPPORTED
    if (is_km_key_active) {
        esp_key_mgr_deactivate_key(km_ri->key_type);
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    if (err != ESP_OK) {
        memset(em_words, 0, sizeof(uint32_t) * data_len);
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
