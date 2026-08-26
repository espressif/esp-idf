/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * PSA ITS custom storage backend API.
 *
 * When CONFIG_MBEDTLS_PSA_ITS_CUSTOM_STORAGE_BACKEND is enabled, users can register
 * a custom storage backend for a reserved range of PSA key IDs. UIDs within
 * the configured range are routed to the registered backend; all other UIDs
 * continue using the default NVS backend.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "psa/internal_trusted_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Custom storage backend operations for PSA ITS.
 *
 * Implement this structure to provide a custom storage backend for PSA ITS
 * keys in the configured UID range. Callback signatures mirror the PSA ITS
 * API with an additional user-provided context pointer.
 */
typedef struct {
    /**
     * @brief Store data for the given UID.
     *
     * @param ctx           User-provided context pointer
     * @param uid           Storage UID (key ID)
     * @param data_length   Length of the data to store
     * @param p_data        Pointer to the data buffer
     * @param create_flags  Storage flags (e.g., PSA_STORAGE_FLAG_WRITE_ONCE)
     * @return PSA status code
     */
    psa_status_t (*set)(void *ctx,
                        const psa_storage_uid_t uid,
                        const uint32_t data_length,
                        const void *p_data,
                        const psa_storage_create_flags_t create_flags);

    /**
     * @brief Retrieve data for the given UID.
     *
     * @param ctx           User-provided context pointer
     * @param uid           Storage UID (key ID)
     * @param data_offset   Byte offset within the stored data
     * @param data_length   Number of bytes to retrieve
     * @param p_data        Output buffer for the data
     * @param p_data_length On success, set to the number of bytes written
     * @return PSA status code
     */
    psa_status_t (*get)(void *ctx,
                        const psa_storage_uid_t uid,
                        const uint32_t data_offset,
                        const uint32_t data_length,
                        void *p_data,
                        size_t *p_data_length);

    /**
     * @brief Retrieve metadata for the given UID.
     *
     * @param ctx    User-provided context pointer
     * @param uid    Storage UID (key ID)
     * @param p_info Output structure for size and flags
     * @return PSA status code
     */
    psa_status_t (*get_info)(void *ctx,
                             const psa_storage_uid_t uid,
                             struct psa_storage_info_t *p_info);

    /**
     * @brief Remove data for the given UID.
     *
     * @param ctx  User-provided context pointer
     * @param uid  Storage UID (key ID)
     * @return PSA status code
     */
    psa_status_t (*remove)(void *ctx,
                           const psa_storage_uid_t uid);

    /** User-provided context pointer, passed as the first argument to all callbacks. */
    void *ctx;
} esp_psa_its_custom_ops_t;

/**
 * @brief Register a custom storage backend for the configured UID range.
 *
 * Only one custom backend may be registered at a time. The UID range is
 * determined by CONFIG_MBEDTLS_PSA_ITS_CUSTOM_BACKEND_UID_MIN (inclusive) and
 * CONFIG_MBEDTLS_PSA_ITS_CUSTOM_BACKEND_UID_MAX (inclusive).
 *
 * @param ops Pointer to the operations structure. Must remain valid for
 *            the lifetime of the registration. All four function pointers
 *            (set, get, get_info, remove) must be non-NULL.
 *
 * @return PSA_SUCCESS on success
 * @return PSA_ERROR_INVALID_ARGUMENT if ops or any callback is NULL
 * @return PSA_ERROR_NOT_PERMITTED if a backend is already registered
 */
psa_status_t esp_psa_its_register_custom_backend(const esp_psa_its_custom_ops_t *ops);

/**
 * @brief Unregister the currently registered custom storage backend.
 *
 * After this call, UIDs in the custom range will return
 * PSA_ERROR_STORAGE_FAILURE until a new backend is registered.
 *
 * @return PSA_SUCCESS on success
 * @return PSA_ERROR_DOES_NOT_EXIST if no backend is registered
 */
psa_status_t esp_psa_its_unregister_custom_backend(void);

#ifdef __cplusplus
}
#endif
