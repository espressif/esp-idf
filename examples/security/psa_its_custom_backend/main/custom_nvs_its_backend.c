/*
 * Custom NVS storage implementation for PSA ITS blobs
 *
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * Demonstrates the most stripped-down on-disk format possible: only the raw
 * key material is persisted in a separate NVS namespace ("psa_its_ext").
 * Nothing else — no PSA wrapper, no attribute trailer, no metadata.
 */

#include <string.h>
#include <stdlib.h>

#include "custom_nvs_its_backend.h"
#include "psa/crypto.h"
#include "esp_psa_key_file.h"
#include "nvs.h"

#define EXT_NVS_NAMESPACE "psa_its_ext"
#define EXT_NVS_KEY_LEN   14

/* Hardcoded attributes for keys served by this backend. They must match
 * what the application uses when calling psa_generate_key() / psa_import_key()
 * for keys in the custom UID range — PSA Crypto validates the attributes
 * we report against the operation being requested. */
#define BACKEND_KEY_LIFETIME  PSA_KEY_LIFETIME_PERSISTENT
#define BACKEND_KEY_TYPE      PSA_KEY_TYPE_AES
#define BACKEND_KEY_USAGE     (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT)
#define BACKEND_KEY_ALG       PSA_ALG_CBC_NO_PADDING

static void uid_to_nvs_key(psa_storage_uid_t uid, char *key)
{
    static const char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    for (int i = 0; i < 13; i++) {
        key[12 - i] = base32[uid & 0x1F];
        uid >>= 5;
    }
    key[13] = '\0';
}

/* Fill psa_key_attributes_t for a stored key whose material is N bytes long.
 * Bits is derived from N; everything else is the backend's fixed contract. */
static void fill_hardcoded_attributes(psa_key_attributes_t *attr, size_t key_data_len)
{
    psa_set_key_lifetime(attr, BACKEND_KEY_LIFETIME);
    psa_set_key_type(attr, BACKEND_KEY_TYPE);
    psa_set_key_bits(attr, key_data_len * 8);
    psa_set_key_usage_flags(attr, BACKEND_KEY_USAGE);
    psa_set_key_algorithm(attr, BACKEND_KEY_ALG);
}

/* ---- Public storage operations ---- */

psa_status_t custom_nvs_its_set(void *ctx, psa_storage_uid_t uid, uint32_t data_length,
                                const void *p_data, psa_storage_create_flags_t create_flags)
{
    (void)ctx;
    (void)create_flags;  /* WRITE_ONCE not tracked in this minimal example */

    /* Unpack the incoming PSA blob — we keep only the inner key material
     * and drop the attributes, because we already know what kind of key
     * this backend stores. */
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    const uint8_t *key_data = NULL;
    size_t key_data_len = 0;
    psa_status_t status = esp_psa_key_file_unpack(p_data, data_length,
                                                  &attr, &key_data, &key_data_len);
    psa_reset_key_attributes(&attr);
    if (status != PSA_SUCCESS) {
        return status;
    }

    nvs_handle_t handle;
    char nvs_key[EXT_NVS_KEY_LEN];
    uid_to_nvs_key(uid, nvs_key);

    esp_err_t err = nvs_open(EXT_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    err = nvs_set_blob(handle, nvs_key, key_data, key_data_len);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);

    return (err == ESP_OK) ? PSA_SUCCESS : PSA_ERROR_STORAGE_FAILURE;
}

psa_status_t custom_nvs_its_get(void *ctx, psa_storage_uid_t uid, uint32_t data_offset,
                                uint32_t data_length, void *p_data, size_t *p_data_length)
{
    (void)ctx;

    nvs_handle_t handle;
    char nvs_key[EXT_NVS_KEY_LEN];
    uid_to_nvs_key(uid, nvs_key);

    esp_err_t err = nvs_open(EXT_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    if (err != ESP_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    size_t key_data_len = 0;
    err = nvs_get_blob(handle, nvs_key, NULL, &key_data_len);
    if (err != ESP_OK) {
        nvs_close(handle);
        return (err == ESP_ERR_NVS_NOT_FOUND) ? PSA_ERROR_DOES_NOT_EXIST
                                              : PSA_ERROR_STORAGE_FAILURE;
    }

    uint8_t *key_data = calloc(1, key_data_len);
    if (key_data == NULL) {
        nvs_close(handle);
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    err = nvs_get_blob(handle, nvs_key, key_data, &key_data_len);
    nvs_close(handle);
    if (err != ESP_OK) {
        free(key_data);
        return PSA_ERROR_STORAGE_FAILURE;
    }

    /* Reconstruct the PSA blob: hardcoded attributes + stored key bytes. */
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    fill_hardcoded_attributes(&attr, key_data_len);

    size_t blob_size = esp_psa_key_file_size(key_data_len);
    if (data_offset + data_length < data_offset ||
        data_offset + data_length > blob_size) {
        free(key_data);
        psa_reset_key_attributes(&attr);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t *blob = calloc(1, blob_size);
    if (blob == NULL) {
        free(key_data);
        psa_reset_key_attributes(&attr);
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    size_t written = 0;
    psa_status_t status = esp_psa_key_file_pack(&attr, key_data, key_data_len,
                                                blob, blob_size, &written);
    if (status != PSA_SUCCESS) {
        free(blob);
        free(key_data);
        psa_reset_key_attributes(&attr);
        return status;
    }

    if (data_length > 0 && p_data != NULL) {
        memcpy(p_data, blob + data_offset, data_length);
    }
    if (p_data_length != NULL) {
        *p_data_length = data_length;
    }

    free(blob);
    free(key_data);
    psa_reset_key_attributes(&attr);
    return PSA_SUCCESS;
}

psa_status_t custom_nvs_its_get_info(void *ctx, psa_storage_uid_t uid,
                                     struct psa_storage_info_t *p_info)
{
    (void)ctx;

    nvs_handle_t handle;
    char nvs_key[EXT_NVS_KEY_LEN];
    uid_to_nvs_key(uid, nvs_key);

    esp_err_t err = nvs_open(EXT_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    if (err != ESP_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    size_t key_data_len = 0;
    err = nvs_get_blob(handle, nvs_key, NULL, &key_data_len);
    nvs_close(handle);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    if (err != ESP_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    /* PSA expects the size of the synthesized PSA blob, not the on-disk size. */
    p_info->size  = (uint32_t)esp_psa_key_file_size(key_data_len);
    p_info->flags = 0;  /* WRITE_ONCE not tracked in this minimal example */
    return PSA_SUCCESS;
}

psa_status_t custom_nvs_its_remove(void *ctx, psa_storage_uid_t uid)
{
    (void)ctx;

    nvs_handle_t handle;
    char nvs_key[EXT_NVS_KEY_LEN];
    uid_to_nvs_key(uid, nvs_key);

    esp_err_t err = nvs_open(EXT_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    if (err != ESP_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    err = nvs_erase_key(handle, nvs_key);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    return (err == ESP_OK) ? PSA_SUCCESS : PSA_ERROR_STORAGE_FAILURE;
}
