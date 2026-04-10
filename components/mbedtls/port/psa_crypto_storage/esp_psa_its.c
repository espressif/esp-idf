/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * PSA ITS (Internal Trusted Storage) implementation using ESP-IDF NVS.
 *
 * This file provides the PSA Internal Trusted Storage API using ESP-IDF's
 * NVS (Non-Volatile Storage) flash storage instead of a filesystem.
 * This is suitable for embedded systems without filesystem support.
 */

#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "psa_crypto_its.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

/* ESP-IDF specific includes */
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "esp_psa_its";

/* NVS namespace for PSA ITS */
#define PSA_ITS_NVS_NAMESPACE "psa_its"

/* The maximum value of psa_storage_info_t.size */
#define PSA_ITS_MAX_SIZE 0xffffffff

/* Magic number for entry validation: 'PSAI' */
#define PSA_ITS_MAGIC 0x50534149

/* NVS key length limit is 15 characters + null terminator */
#define PSA_ITS_NVS_KEY_LEN 14  /* 13 chars + null */

/**
 * Storage entry format stored in NVS blob:
 * - magic: 4 bytes (0x50534149 'PSAI')
 * - flags: 4 bytes (PSA storage flags)
 * - size:  4 bytes (data size)
 * - data:  variable length
 */
typedef struct {
    uint32_t magic;
    uint32_t flags;
    uint32_t size;
    uint8_t data[];
} __attribute__((packed)) psa_its_entry_t;

/**
 * Convert 64-bit UID to 13-character base32 NVS key.
 * Uses RFC 4648 base32 alphabet: A-Z, 2-7 (32 characters).
 * 64 bits / 5 bits per char = 12.8, needs 13 chars + null terminator.
 */
static void uid_to_nvs_key(psa_storage_uid_t uid, char *key)
{
    static const char base32_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

    /* Encode 64-bit UID as 13 base32 characters */
    for (int i = 0; i < 13; i++) {
        key[12 - i] = base32_alphabet[uid & 0x1F];
        uid >>= 5;
    }
    key[13] = '\0';
}

/**
 * Map ESP error codes to PSA status codes.
 */
static psa_status_t esp_err_to_psa_status(esp_err_t err)
{
    switch (err) {
        case ESP_OK:
            return PSA_SUCCESS;
        case ESP_ERR_NVS_NOT_FOUND:
            return PSA_ERROR_DOES_NOT_EXIST;
        case ESP_ERR_NVS_NOT_ENOUGH_SPACE:
        case ESP_ERR_NVS_NO_FREE_PAGES:
            return PSA_ERROR_INSUFFICIENT_STORAGE;
        case ESP_ERR_NVS_INVALID_LENGTH:
        case ESP_ERR_NVS_INVALID_NAME:
            return PSA_ERROR_INVALID_ARGUMENT;
        default:
            return PSA_ERROR_STORAGE_FAILURE;
    }
}

/**
 * Get storage information for a UID.
 */
psa_status_t psa_its_get_info(psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info)
{
    nvs_handle_t handle;
    esp_err_t err;
    char nvs_key[PSA_ITS_NVS_KEY_LEN];
    size_t required_size = 0;
    psa_its_entry_t *entry = NULL;
    psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

    if (p_info == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Convert UID to NVS key */
    uid_to_nvs_key(uid, nvs_key);

    /* Open NVS handle */
    err = nvs_open(PSA_ITS_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        /* Namespace doesn't exist yet, which means key doesn't exist */
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return esp_err_to_psa_status(err);
    }

    /* Get the blob size first */
    err = nvs_get_blob(handle, nvs_key, NULL, &required_size);
    if (err != ESP_OK) {
        status = esp_err_to_psa_status(err);
        goto exit;
    }

    /* Check minimum size for header */
    if (required_size < sizeof(psa_its_entry_t)) {
        ESP_LOGE(TAG, "Corrupted entry: size too small");
        status = PSA_ERROR_DATA_CORRUPT;
        goto exit;
    }

    /* Allocate and read entry header */
    entry = mbedtls_calloc(1, required_size);
    if (entry == NULL) {
        status = PSA_ERROR_INSUFFICIENT_MEMORY;
        goto exit;
    }

    err = nvs_get_blob(handle, nvs_key, entry, &required_size);
    if (err != ESP_OK) {
        status = esp_err_to_psa_status(err);
        goto exit;
    }

    /* Validate magic number */
    if (entry->magic != PSA_ITS_MAGIC) {
        ESP_LOGE(TAG, "Invalid magic number: 0x%08lx", (unsigned long)entry->magic);
        status = PSA_ERROR_DATA_CORRUPT;
        goto exit;
    }

    /* Return info */
    p_info->size = entry->size;
    p_info->flags = entry->flags;
    status = PSA_SUCCESS;

exit:
    if (entry != NULL) {
        mbedtls_platform_zeroize(entry, required_size);
        mbedtls_free(entry);
    }
    nvs_close(handle);
    return status;
}

/**
 * Retrieve data from storage.
 */
psa_status_t psa_its_get(psa_storage_uid_t uid,
                         uint32_t data_offset,
                         uint32_t data_length,
                         void *p_data,
                         size_t *p_data_length)
{
    nvs_handle_t handle;
    esp_err_t err;
    char nvs_key[PSA_ITS_NVS_KEY_LEN];
    size_t required_size = 0;
    psa_its_entry_t *entry = NULL;
    psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

    if (p_data == NULL && data_length != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Convert UID to NVS key */
    uid_to_nvs_key(uid, nvs_key);

    /* Open NVS handle */
    err = nvs_open(PSA_ITS_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return esp_err_to_psa_status(err);
    }

    /* Get the blob size */
    err = nvs_get_blob(handle, nvs_key, NULL, &required_size);
    if (err != ESP_OK) {
        status = esp_err_to_psa_status(err);
        goto exit;
    }

    /* Check minimum size */
    if (required_size < sizeof(psa_its_entry_t)) {
        ESP_LOGE(TAG, "Corrupted entry: size too small");
        status = PSA_ERROR_DATA_CORRUPT;
        goto exit;
    }

    /* Allocate and read full entry */
    entry = mbedtls_calloc(1, required_size);
    if (entry == NULL) {
        status = PSA_ERROR_INSUFFICIENT_MEMORY;
        goto exit;
    }

    err = nvs_get_blob(handle, nvs_key, entry, &required_size);
    if (err != ESP_OK) {
        status = esp_err_to_psa_status(err);
        goto exit;
    }

    /* Validate magic number */
    if (entry->magic != PSA_ITS_MAGIC) {
        ESP_LOGE(TAG, "Invalid magic number: 0x%08lx", (unsigned long)entry->magic);
        status = PSA_ERROR_DATA_CORRUPT;
        goto exit;
    }

    /* Validate offset and length */
    if (data_offset + data_length < data_offset) {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }
#if SIZE_MAX < 0xffffffff
    if (data_offset + data_length > SIZE_MAX) {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }
#endif
    if (data_offset + data_length > entry->size) {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    /* Copy requested data portion */
    if (data_length > 0) {
        memcpy(p_data, entry->data + data_offset, data_length);
    }

    if (p_data_length != NULL) {
        *p_data_length = data_length;
    }

    status = PSA_SUCCESS;

exit:
    if (entry != NULL) {
        mbedtls_platform_zeroize(entry, required_size);
        mbedtls_free(entry);
    }
    nvs_close(handle);
    return status;
}

/**
 * Store data in NVS.
 */
psa_status_t psa_its_set(psa_storage_uid_t uid,
                         uint32_t data_length,
                         const void *p_data,
                         psa_storage_create_flags_t create_flags)
{
    nvs_handle_t handle;
    esp_err_t err;
    char nvs_key[PSA_ITS_NVS_KEY_LEN];
    psa_its_entry_t *entry = NULL;
    size_t entry_size;
    size_t existing_size = 0;
    psa_its_entry_t *existing_entry = NULL;
    psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

    /* UID 0 is invalid per PSA spec */
    if (uid == 0) {
        return PSA_ERROR_INVALID_HANDLE;
    }

    if (p_data == NULL && data_length != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Convert UID to NVS key */
    uid_to_nvs_key(uid, nvs_key);

    /* Open NVS handle with read-write access */
    err = nvs_open(PSA_ITS_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return esp_err_to_psa_status(err);
    }

    /* Check if entry already exists and has WRITE_ONCE flag */
    err = nvs_get_blob(handle, nvs_key, NULL, &existing_size);
    if (err == ESP_OK && existing_size >= sizeof(psa_its_entry_t)) {
        /* Entry exists, check WRITE_ONCE flag */
        existing_entry = mbedtls_calloc(1, existing_size);
        if (existing_entry != NULL) {
            err = nvs_get_blob(handle, nvs_key, existing_entry, &existing_size);
            if (err == ESP_OK &&
                existing_entry->magic == PSA_ITS_MAGIC &&
                (existing_entry->flags & PSA_STORAGE_FLAG_WRITE_ONCE)) {
                ESP_LOGW(TAG, "Cannot modify WRITE_ONCE entry");
                status = PSA_ERROR_NOT_PERMITTED;
                mbedtls_platform_zeroize(existing_entry, existing_size);
                mbedtls_free(existing_entry);
                goto exit;
            }
            mbedtls_platform_zeroize(existing_entry, existing_size);
            mbedtls_free(existing_entry);
        }
    }

    /* Allocate entry with header + data */
    entry_size = sizeof(psa_its_entry_t) + data_length;
    entry = mbedtls_calloc(1, entry_size);
    if (entry == NULL) {
        status = PSA_ERROR_INSUFFICIENT_MEMORY;
        goto exit;
    }

    /* Fill entry */
    entry->magic = PSA_ITS_MAGIC;
    entry->flags = create_flags;
    entry->size = data_length;
    if (data_length > 0) {
        memcpy(entry->data, p_data, data_length);
    }

    /* Write to NVS */
    err = nvs_set_blob(handle, nvs_key, entry, entry_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write blob: %s", esp_err_to_name(err));
        status = esp_err_to_psa_status(err);
        goto exit;
    }

    /* Commit to ensure atomicity */
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit: %s", esp_err_to_name(err));
        status = esp_err_to_psa_status(err);
        goto exit;
    }

    status = PSA_SUCCESS;

exit:
    if (entry != NULL) {
        mbedtls_platform_zeroize(entry, entry_size);
        mbedtls_free(entry);
    }
    nvs_close(handle);
    return status;
}

/**
 * Remove data from storage.
 */
psa_status_t psa_its_remove(psa_storage_uid_t uid)
{
    nvs_handle_t handle;
    esp_err_t err;
    char nvs_key[PSA_ITS_NVS_KEY_LEN];
    size_t existing_size = 0;
    psa_its_entry_t *existing_entry = NULL;
    psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

    /* Convert UID to NVS key */
    uid_to_nvs_key(uid, nvs_key);

    /* Open NVS handle */
    err = nvs_open(PSA_ITS_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return esp_err_to_psa_status(err);
    }

    /* Check if entry exists and has WRITE_ONCE flag */
    err = nvs_get_blob(handle, nvs_key, NULL, &existing_size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        status = PSA_ERROR_DOES_NOT_EXIST;
        goto exit;
    }
    if (err != ESP_OK) {
        status = esp_err_to_psa_status(err);
        goto exit;
    }

    if (existing_size >= sizeof(psa_its_entry_t)) {
        /* Read entry to check WRITE_ONCE flag */
        existing_entry = mbedtls_calloc(1, existing_size);
        if (existing_entry == NULL) {
            status = PSA_ERROR_INSUFFICIENT_MEMORY;
            goto exit;
        }

        err = nvs_get_blob(handle, nvs_key, existing_entry, &existing_size);
        if (err == ESP_OK &&
            existing_entry->magic == PSA_ITS_MAGIC &&
            (existing_entry->flags & PSA_STORAGE_FLAG_WRITE_ONCE)) {
            ESP_LOGW(TAG, "Cannot remove WRITE_ONCE entry");
            status = PSA_ERROR_NOT_PERMITTED;
            goto exit;
        }
    }

    /* Erase the key */
    err = nvs_erase_key(handle, nvs_key);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to erase key: %s", esp_err_to_name(err));
        status = esp_err_to_psa_status(err);
        goto exit;
    }

    /* Commit to ensure atomicity */
    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit: %s", esp_err_to_name(err));
        status = esp_err_to_psa_status(err);
        goto exit;
    }

    status = PSA_SUCCESS;

exit:
    if (existing_entry != NULL) {
        mbedtls_platform_zeroize(existing_entry, existing_size);
        mbedtls_free(existing_entry);
    }
    nvs_close(handle);
    return status;
}
