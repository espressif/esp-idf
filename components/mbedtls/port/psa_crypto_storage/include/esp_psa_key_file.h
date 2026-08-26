/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Custom PSA ITS backends that synthesize blobs on read, or that strip the
 * header on write to save space, use these helpers to convert between PSA
 * attributes + raw key material and the documented blob format without
 * depending on tf-psa-crypto internal functions.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Total blob size produced by esp_psa_key_file_pack() for a given key
 *        material length (i.e. fixed header size + @p material_len).
 */
size_t esp_psa_key_file_size(const size_t material_len);

/**
 * @brief Pack key attributes and material into a key file blob.
 *
 * @param[in]  attrs         Source attributes (lifetime, type, bits, policy).
 * @param[in]  material      Key material. For transparent keys, the
 *                           psa_export_key() output. For opaque keys, the
 *                           driver-specific opaque blob.
 * @param[in]  material_len  Length of @p material in bytes.
 * @param[out] out_buf       Destination buffer.
 * @param[in]  out_buf_size  Size of @p out_buf. Must be at least
 *                           esp_psa_key_file_size(material_len).
 * @param[out] out_len       Set to the number of bytes written.
 *
 * @retval PSA_SUCCESS
 * @retval PSA_ERROR_INVALID_ARGUMENT  Null pointer.
 * @retval PSA_ERROR_BUFFER_TOO_SMALL  @p out_buf_size is too small.
 */
psa_status_t esp_psa_key_file_pack(const psa_key_attributes_t *attrs,
                                   const uint8_t *material,
                                   const size_t material_len,
                                   uint8_t *out_buf,
                                   const size_t out_buf_size,
                                   size_t *out_len);

/**
 * @brief Parse a key file blob into key attributes and material.
 *
 * The returned @p material pointer points into @p blob and is valid only as
 * long as @p blob itself is. No allocation occurs.
 *
 * Attribute fields described in the blob (lifetime, type, bits, usage,
 * algorithm, enrollment algorithm) are set on @p attrs on success. Other
 * fields (notably the key id) are left untouched, so the caller can set the
 * id beforehand if needed.
 *
 * @param[in]  blob          Blob to parse.
 * @param[in]  blob_len      Length of @p blob in bytes.
 * @param[out] attrs         Filled with parsed attributes on success.
 * @param[out] material      Set to point to the material section within @p blob,
 *                           or NULL if the blob declares zero-length material.
 * @param[out] material_len  Length of the material section in bytes.
 *
 * @retval PSA_SUCCESS
 * @retval PSA_ERROR_INVALID_ARGUMENT  Null pointer, or @p blob_len smaller than
 *                                     ESP_PSA_KEY_FILE_HEADER_SIZE.
 * @retval PSA_ERROR_DATA_INVALID      Bad magic, unsupported version, or the
 *                                     declared material length does not match
 *                                     the blob size (the spec rejects trailing
 *                                     data on load).
 */
psa_status_t esp_psa_key_file_unpack(const uint8_t *blob,
                                     const size_t blob_len,
                                     psa_key_attributes_t *attrs,
                                     const uint8_t **material,
                                     size_t *material_len);

#ifdef __cplusplus
}
#endif
