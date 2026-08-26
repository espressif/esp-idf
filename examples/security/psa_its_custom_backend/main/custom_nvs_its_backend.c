/*
 * Custom NVS storage implementation for PSA ITS blobs
 *
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * Stores PSA blobs verbatim in a separate NVS namespace ("psa_its_ext").
 */

#include <string.h>
#include <stdlib.h>

#include "custom_nvs_its_backend.h"
#include "psa/crypto.h"
#include "nvs.h"

#define EXT_NVS_NAMESPACE "psa_its_ext"
#define EXT_NVS_KEY_LEN   14

static void uid_to_nvs_key(psa_storage_uid_t uid, char *key)
{
    static const char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    for (int i = 0; i < 13; i++) {
        key[12 - i] = base32[uid & 0x1F];
        uid >>= 5;
    }
    key[13] = '\0';
}

/* ---- Public storage operations ---- */

psa_status_t custom_nvs_its_set(void *ctx, const psa_storage_uid_t uid,
                                const uint32_t data_length, const void *p_data,
                                const psa_storage_create_flags_t create_flags)
{
    (void)ctx;
    (void)create_flags;  /* WRITE_ONCE not tracked in this minimal example */

    nvs_handle_t handle;
    char nvs_key[EXT_NVS_KEY_LEN];
    uid_to_nvs_key(uid, nvs_key);

    esp_err_t err = nvs_open(EXT_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    err = nvs_set_blob(handle, nvs_key, p_data, data_length);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);

    return (err == ESP_OK) ? PSA_SUCCESS : PSA_ERROR_STORAGE_FAILURE;
}

psa_status_t custom_nvs_its_get(void *ctx, const psa_storage_uid_t uid,
                                const uint32_t data_offset, const uint32_t data_length,
                                void *p_data, size_t *p_data_length)
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

    size_t blob_size = 0;
    err = nvs_get_blob(handle, nvs_key, NULL, &blob_size);
    if (err != ESP_OK) {
        nvs_close(handle);
        return (err == ESP_ERR_NVS_NOT_FOUND) ? PSA_ERROR_DOES_NOT_EXIST
                                              : PSA_ERROR_STORAGE_FAILURE;
    }

    if (data_offset + data_length < data_offset ||
        data_offset + data_length > blob_size) {
        nvs_close(handle);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t *blob = calloc(1, blob_size);
    if (blob == NULL) {
        nvs_close(handle);
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    err = nvs_get_blob(handle, nvs_key, blob, &blob_size);
    nvs_close(handle);
    if (err != ESP_OK) {
        free(blob);
        return PSA_ERROR_STORAGE_FAILURE;
    }

    if (data_length > 0 && p_data != NULL) {
        memcpy(p_data, blob + data_offset, data_length);
    }
    if (p_data_length != NULL) {
        *p_data_length = data_length;
    }

    free(blob);
    return PSA_SUCCESS;
}

psa_status_t custom_nvs_its_get_info(void *ctx, const psa_storage_uid_t uid,
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

    size_t blob_size = 0;
    err = nvs_get_blob(handle, nvs_key, NULL, &blob_size);
    nvs_close(handle);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    if (err != ESP_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    p_info->size  = (uint32_t)blob_size;
    p_info->flags = 0;  /* WRITE_ONCE not tracked in this minimal example */
    return PSA_SUCCESS;
}

psa_status_t custom_nvs_its_remove(void *ctx, const psa_storage_uid_t uid)
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
