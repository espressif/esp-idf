/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <stdint.h>
#include <string.h>
#include "psa/crypto.h"
#include "mbedtls/constant_time.h"
#include "psa_crypto_driver_esp_hmac_opaque.h"
#include "psa_crypto_driver_esp_opaque_common.h"
#include "esp_efuse.h"
#include "esp_efuse_chip.h"
#include "esp_assert.h"
#include "hal/hmac_types.h"
#include "esp_hmac.h"
#include "esp_log.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include "esp_key_mgr.h"
#endif /* SOC_KEY_MANAGER_SUPPORTED */

/*
 * Per-source storage structs — internal to the driver.
 * All storage structs share a common prefix: [version][key_source]
 * so that the driver can identify the key source at operation time.
 */
typedef enum {
    ESP_HMAC_KEY_STORAGE_VERSION_INVALID = 0,
    ESP_HMAC_KEY_STORAGE_VERSION_V1 = 1,
    ESP_HMAC_KEY_STORAGE_VERSION_MAX = 2,
} esp_hmac_key_storage_version_t;

typedef enum {
    ESP_HMAC_KEY_SOURCE_EFUSE = 0,
    ESP_HMAC_KEY_SOURCE_KEY_MGR = 1,
} esp_hmac_key_source_t;

typedef struct __attribute__((packed)) {
    uint8_t version;
    uint8_t key_source;             /* esp_hmac_key_source_t */
} esp_hmac_common_key_storage_metadata_t;

ESP_STATIC_ASSERT(sizeof(esp_hmac_common_key_storage_metadata_t) == 2 * sizeof(uint8_t), "esp_hmac_common_key_storage_metadata_t structure must be exactly 2 bytes");

typedef struct __attribute__((packed)) {
    esp_hmac_common_key_storage_metadata_t metadata;
    uint8_t efuse_key_id;
} esp_hmac_efuse_key_storage_t;

#if SOC_KEY_MANAGER_SUPPORTED
typedef struct {
    esp_hmac_common_key_storage_metadata_t metadata;
    uint8_t reserved[2];            /* explicit padding for alignment */
    esp_key_mgr_key_recovery_info_t key_recovery_info;
} esp_hmac_km_key_storage_t;

ESP_STATIC_ASSERT(offsetof(esp_hmac_km_key_storage_t, key_recovery_info) % 4 == 0,
                  "key_recovery_info must be 4-byte aligned in esp_hmac_km_key_storage_t");
#endif /* SOC_KEY_MANAGER_SUPPORTED */

/**
 * Pointer-based storage for volatile keys.
 * Embeds the user-facing opaque key struct (which contains pointers).
 * The caller must keep all referenced data valid until psa_destroy_key().
 */
typedef struct {
    esp_hmac_common_key_storage_metadata_t metadata;
    esp_hmac_opaque_key_t opaque_key;
} esp_hmac_volatile_key_storage_t;

static inline esp_hmac_key_source_t hmac_storage_get_key_source(const uint8_t *key_buffer)
{
    return (esp_hmac_key_source_t)key_buffer[1];
}

static size_t esp_hmac_get_storage_size(const esp_hmac_opaque_key_t *key, bool persistent)
{
    if (!persistent) {
        (void)key;
        return sizeof(esp_hmac_volatile_key_storage_t);
    }

    /* Persistent: per-source dispatch */
#if SOC_KEY_MANAGER_SUPPORTED
    if (key->key_recovery_info) {
        return sizeof(esp_hmac_km_key_storage_t);
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    return sizeof(esp_hmac_efuse_key_storage_t);
}

static psa_status_t esp_hmac_get_expected_storage_size(const uint8_t *key_buffer, bool persistent, size_t *expected_storage_size)
{
    if (key_buffer[0] <= ESP_HMAC_KEY_STORAGE_VERSION_INVALID || key_buffer[0] >= ESP_HMAC_KEY_STORAGE_VERSION_MAX) {
        return PSA_ERROR_DATA_INVALID;
    }

    *expected_storage_size = 0;

    if (!persistent) {
        *expected_storage_size = sizeof(esp_hmac_volatile_key_storage_t);
        return PSA_SUCCESS;
    }

    /* Persistent: per-source dispatch */
    esp_hmac_key_source_t key_source = hmac_storage_get_key_source(key_buffer);

    switch (key_source) {
#if SOC_KEY_MANAGER_SUPPORTED
        case ESP_HMAC_KEY_SOURCE_KEY_MGR:
            *expected_storage_size = sizeof(esp_hmac_km_key_storage_t);
            return PSA_SUCCESS;
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        case ESP_HMAC_KEY_SOURCE_EFUSE:
            *expected_storage_size = sizeof(esp_hmac_efuse_key_storage_t);
            return PSA_SUCCESS;
        default:
            return PSA_ERROR_DATA_INVALID;
    }
}

static bool validate_hmac_opaque_key_attributes(const esp_hmac_opaque_key_t *opaque_key)
{
#if SOC_KEY_MANAGER_SUPPORTED
    if (opaque_key->key_recovery_info) {
        return true;
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    // efuse_key_id is uint8_t, so opaque_key->efuse_key_id + EFUSE_BLK_KEY0 >= EFUSE_BLK_KEY0
    if (((opaque_key->efuse_key_id + EFUSE_BLK_KEY0) < EFUSE_BLK_KEY_MAX)
        && (esp_efuse_get_key_purpose(EFUSE_BLK_KEY0 + opaque_key->efuse_key_id) == ESP_EFUSE_KEY_PURPOSE_HMAC_UP)) {
        return true;
    }
    return false;
}

psa_status_t esp_hmac_import_key_opaque(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits)
{
    if (!attributes || !data || data_length < sizeof(esp_hmac_opaque_key_t) || !key_buffer || !key_buffer_length || !bits) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(psa_get_key_algorithm(attributes));
    if (hash_alg != PSA_ALG_SHA_256) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (psa_get_key_bits(attributes) != 256) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    const esp_hmac_opaque_key_t *opaque_key = (const esp_hmac_opaque_key_t *) data;
    bool valid = validate_hmac_opaque_key_attributes(opaque_key);
    if (!valid) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Convert user-facing struct to per-source storage struct */
    bool is_persistent = esp_opaque_key_is_persistent(attributes);

    if (!is_persistent) {
        /* Volatile: store pointers — caller keeps data alive */
        if (key_buffer_size < sizeof(esp_hmac_volatile_key_storage_t)) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        esp_hmac_volatile_key_storage_t *storage = (esp_hmac_volatile_key_storage_t *)key_buffer;
        storage->metadata.version = ESP_HMAC_KEY_STORAGE_VERSION_V1;
        esp_hmac_key_source_t key_source = ESP_HMAC_KEY_SOURCE_EFUSE;
#if SOC_KEY_MANAGER_SUPPORTED
        if (opaque_key->key_recovery_info) {
            key_source = ESP_HMAC_KEY_SOURCE_KEY_MGR;
        }
#endif
        storage->metadata.key_source = key_source;
        memcpy(&storage->opaque_key, opaque_key, sizeof(esp_hmac_opaque_key_t));
        *key_buffer_length = sizeof(esp_hmac_volatile_key_storage_t);
    } else {
        /* Persistent: per-source inline storage */
#if SOC_KEY_MANAGER_SUPPORTED
        if (opaque_key->key_recovery_info) {
            if (key_buffer_size < sizeof(esp_hmac_km_key_storage_t)) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }
            esp_hmac_km_key_storage_t *storage = (esp_hmac_km_key_storage_t *)key_buffer;
            storage->metadata.version = ESP_HMAC_KEY_STORAGE_VERSION_V1;
            storage->metadata.key_source = ESP_HMAC_KEY_SOURCE_KEY_MGR;
            memset(storage->reserved, 0, sizeof(storage->reserved));
            memcpy(&storage->key_recovery_info, opaque_key->key_recovery_info,
                   sizeof(esp_key_mgr_key_recovery_info_t));
            *key_buffer_length = sizeof(esp_hmac_km_key_storage_t);
        } else
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        {
            if (key_buffer_size < sizeof(esp_hmac_efuse_key_storage_t)) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            esp_hmac_efuse_key_storage_t *storage = (esp_hmac_efuse_key_storage_t *)key_buffer;
            storage->metadata.version = ESP_HMAC_KEY_STORAGE_VERSION_V1;
            storage->metadata.key_source = ESP_HMAC_KEY_SOURCE_EFUSE;
            storage->efuse_key_id = opaque_key->efuse_key_id;
            *key_buffer_length = sizeof(esp_hmac_efuse_key_storage_t);
        }
    }

    *bits = psa_get_key_bits(attributes);
    return PSA_SUCCESS;
}

psa_status_t esp_hmac_abort_opaque(esp_hmac_opaque_operation_t *esp_hmac_ctx)
{
    if (esp_hmac_ctx != NULL) {
        mbedtls_platform_zeroize(esp_hmac_ctx, sizeof(esp_hmac_opaque_operation_t));
    }
    return PSA_SUCCESS;
}

psa_status_t esp_hmac_setup_opaque(
    esp_hmac_opaque_operation_t *esp_hmac_ctx,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    if (!esp_hmac_ctx || !attributes || !key_buffer || !key_buffer_size || !alg) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (esp_hmac_ctx->alg != 0) {
        return PSA_ERROR_BAD_STATE;
    }

    if (key_buffer_size < sizeof(esp_hmac_efuse_key_storage_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    bool is_persistent = esp_opaque_key_is_persistent(attributes);

    size_t expected_storage_size = 0;
    psa_status_t status = esp_hmac_get_expected_storage_size(key_buffer, is_persistent, &expected_storage_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (key_buffer_size < expected_storage_size) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_HMAC(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_algorithm_t hash_alg = PSA_ALG_GET_HASH(alg);

    if (hash_alg != PSA_ALG_SHA_256) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    memset(esp_hmac_ctx, 0, sizeof(esp_hmac_opaque_operation_t));

    esp_hmac_ctx->alg = alg;
    esp_hmac_ctx->key_buffer = key_buffer;
    esp_hmac_ctx->is_persistent = is_persistent;

    return PSA_SUCCESS;
}

#if SOC_KEY_MANAGER_SUPPORTED && !ESP_TEE_BUILD
static psa_status_t esp_hmac_opaque_resolve_key(const esp_hmac_opaque_operation_t *ctx,
                                                hmac_key_id_t *hmac_key_id,
                                                const esp_key_mgr_key_recovery_info_t **kri)
{
    const uint8_t *key_buffer = ctx->key_buffer;
    *kri = NULL;

    if (hmac_storage_get_key_source(key_buffer) == ESP_HMAC_KEY_SOURCE_KEY_MGR) {
        *kri = ctx->is_persistent
             ? &((const esp_hmac_km_key_storage_t *)key_buffer)->key_recovery_info
             : ((const esp_hmac_volatile_key_storage_t *)key_buffer)->opaque_key.key_recovery_info;
        if (esp_key_mgr_activate_key((esp_key_mgr_key_recovery_info_t *)*kri) != ESP_OK) {
            *kri = NULL;
            ESP_LOGE("ESP_HMAC_OPAQUE", "Failed to activate key");
            return PSA_ERROR_INVALID_HANDLE;
        }
        *hmac_key_id = HMAC_KEY_KM;
        return PSA_SUCCESS;
    }

    *hmac_key_id = ctx->is_persistent
                 ? ((const esp_hmac_efuse_key_storage_t *)key_buffer)->efuse_key_id
                 : ((const esp_hmac_volatile_key_storage_t *)key_buffer)->opaque_key.efuse_key_id;
    return PSA_SUCCESS;
}
#else
static psa_status_t esp_hmac_opaque_resolve_key(const esp_hmac_opaque_operation_t *ctx,
                                                hmac_key_id_t *hmac_key_id)
{
    const uint8_t *key_buffer = ctx->key_buffer;
    *hmac_key_id = ctx->is_persistent
                 ? ((const esp_hmac_efuse_key_storage_t *)key_buffer)->efuse_key_id
                 : ((const esp_hmac_volatile_key_storage_t *)key_buffer)->opaque_key.efuse_key_id;
    return PSA_SUCCESS;
}
#endif /* SOC_KEY_MANAGER_SUPPORTED && !ESP_TEE_BUILD */

static psa_status_t esp_hmac_opaque_calculate(esp_hmac_opaque_operation_t *ctx,
                                              const uint8_t *data, size_t data_length)
{
    hmac_key_id_t hmac_key_id = 0;
    psa_status_t status;

#if SOC_KEY_MANAGER_SUPPORTED && !ESP_TEE_BUILD
    const esp_key_mgr_key_recovery_info_t *kri = NULL;
    status = esp_hmac_opaque_resolve_key(ctx, &hmac_key_id, &kri);
#else
    status = esp_hmac_opaque_resolve_key(ctx, &hmac_key_id);
#endif /* SOC_KEY_MANAGER_SUPPORTED && !ESP_TEE_BUILD */
    if (status != PSA_SUCCESS) {
        return status;
    }

    esp_err_t hmac_ret = esp_hmac_calculate(hmac_key_id, data, data_length, ctx->hmac);

#if SOC_KEY_MANAGER_SUPPORTED && !ESP_TEE_BUILD
    if (kri) {
        esp_key_mgr_deactivate_key(kri->key_type);
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED && !ESP_TEE_BUILD */

    if (hmac_ret == ESP_OK) {
        ctx->computed = true;
        return PSA_SUCCESS;
    }

    mbedtls_platform_zeroize(ctx->hmac, sizeof(ctx->hmac));

    if (hmac_ret == ESP_ERR_INVALID_ARG) {
        return PSA_ERROR_INVALID_ARGUMENT;
    } else if (hmac_ret == ESP_FAIL) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    return PSA_ERROR_CORRUPTION_DETECTED;
}

psa_status_t esp_hmac_update_opaque(esp_hmac_opaque_operation_t *esp_hmac_ctx, const uint8_t *data, size_t data_length)
{
    if (!esp_hmac_ctx) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (data == NULL && data_length != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (esp_hmac_ctx->alg == 0) {
        return PSA_ERROR_BAD_STATE;
    }

    if (esp_hmac_ctx->computed) {
        return PSA_ERROR_BAD_STATE;
    }

    /*
     * A zero-length update is a valid no-op. The empty-message MAC is produced
     * lazily in finish: the one-shot HW HMAC cannot be fed incrementally, so we
     * must not commit a MAC until we know no further (non-empty) data follows.
     */
    if (data_length == 0) {
        return PSA_SUCCESS;
    }

    return esp_hmac_opaque_calculate(esp_hmac_ctx, data, data_length);
}

psa_status_t esp_hmac_finish_opaque(
    esp_hmac_opaque_operation_t *esp_hmac_ctx,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    if (!esp_hmac_ctx || !mac || !mac_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (esp_hmac_ctx->alg == 0) {
        return PSA_ERROR_BAD_STATE;
    }

    if (mac_size == 0 || mac_size > ESP_HMAC_RESULT_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /*
     * No update committed a MAC: the message is empty (e.g. psa_mac_compute()
     * over "" or setup->finish with no update). Compute HMAC(key, "") now.
     * esp_hmac_calculate() rejects a NULL pointer, so pass a valid zero-length
     * buffer rather than NULL.
     */
    if (!esp_hmac_ctx->computed) {
        const uint8_t empty = 0;
        psa_status_t status = esp_hmac_opaque_calculate(esp_hmac_ctx, &empty, 0);
        if (status != PSA_SUCCESS) {
            return status;
        }
    }

    /* PSA core passes the truncated length as mac_size; copy exactly that
     * many bytes to honour PSA_ALG_TRUNCATED_MAC. */
    memcpy(mac, esp_hmac_ctx->hmac, mac_size);
    *mac_length = mac_size;

    mbedtls_platform_zeroize(esp_hmac_ctx->hmac, sizeof(esp_hmac_ctx->hmac));

    return PSA_SUCCESS;
}

psa_status_t esp_hmac_compute_opaque(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    esp_hmac_opaque_operation_t esp_hmac_ctx = {0};

    status = esp_hmac_setup_opaque(&esp_hmac_ctx, attributes, key_buffer, key_buffer_size, alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    status = esp_hmac_update_opaque(&esp_hmac_ctx, input, input_length);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    size_t actual_mac_length = 0;
    status = esp_hmac_finish_opaque(&esp_hmac_ctx, mac, mac_size, &actual_mac_length);
    if (status == PSA_SUCCESS) {
        *mac_length = actual_mac_length;
    }

exit:
    esp_hmac_abort_opaque(&esp_hmac_ctx);
    return status;
}

psa_status_t esp_hmac_verify_finish_opaque(
    esp_hmac_opaque_operation_t *esp_hmac_ctx,
    const uint8_t *mac,
    size_t mac_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    uint8_t actual_mac[ESP_HMAC_RESULT_SIZE];
    size_t actual_mac_length = 0;

    if (esp_hmac_ctx == NULL || mac == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (mac_length == 0 || mac_length > sizeof(actual_mac)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (esp_hmac_ctx->alg == 0) {
        return PSA_ERROR_BAD_STATE;
    }

    status = esp_hmac_finish_opaque(esp_hmac_ctx, actual_mac, mac_length, &actual_mac_length);
    if (status == PSA_SUCCESS) {
        if (mbedtls_ct_memcmp(mac, actual_mac, mac_length) != 0) {
            status = PSA_ERROR_INVALID_SIGNATURE;
        }
    }

    mbedtls_platform_zeroize(actual_mac, sizeof(actual_mac));
    return status;
}

size_t esp_hmac_opaque_size_function(
    const psa_key_attributes_t *attributes,
    psa_key_type_t key_type,
    const uint8_t *data,
    size_t data_length)
{
    (void)key_type;
    bool is_persistent = esp_opaque_key_is_persistent(attributes);

    if (!data || data_length < sizeof(esp_hmac_opaque_key_t)) {
        return is_persistent ? sizeof(esp_hmac_efuse_key_storage_t)
                             : sizeof(esp_hmac_volatile_key_storage_t);
    }

    return esp_hmac_get_storage_size((const esp_hmac_opaque_key_t *)data, is_persistent);
}
