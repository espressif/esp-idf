/*
 * NVS storage implementation for PSA ITS blobs (separate namespace)
 *
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * Signatures match esp_psa_its_custom_ops_t so these functions can be
 * assigned directly to the ops struct in app_main.c.
 */

#pragma once

#include "psa/internal_trusted_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Persist the PSA blob verbatim under `uid` in the custom NVS namespace.
 * create_flags is ignored — this example does not track
 * PSA_STORAGE_FLAG_WRITE_ONCE. */
psa_status_t custom_nvs_its_set(void *ctx, const psa_storage_uid_t uid,
                                const uint32_t data_length, const void *p_data,
                                const psa_storage_create_flags_t create_flags);

/* Copy `data_length` bytes from `data_offset` of the stored PSA blob for
 * `uid` into `p_data`. */
psa_status_t custom_nvs_its_get(void *ctx, const psa_storage_uid_t uid,
                                const uint32_t data_offset, const uint32_t data_length,
                                void *p_data, size_t *p_data_length);

/* Report the size of the stored PSA blob for `uid`. flags is always
 * reported as 0 since create_flags is not tracked. */
psa_status_t custom_nvs_its_get_info(void *ctx, const psa_storage_uid_t uid,
                                     struct psa_storage_info_t *p_info);

/* Remove the blob stored under `uid` from the custom NVS namespace. */
psa_status_t custom_nvs_its_remove(void *ctx, const psa_storage_uid_t uid);

#ifdef __cplusplus
}
#endif
