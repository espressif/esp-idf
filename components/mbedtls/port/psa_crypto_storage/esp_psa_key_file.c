/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_psa_key_file.h"

#include "psa/crypto.h"
#include "psa_crypto_storage.h"

size_t esp_psa_key_file_size(const size_t material_len)
{
    return psa_persistent_key_storage_blob_size(material_len);
}

psa_status_t esp_psa_key_file_pack(const psa_key_attributes_t *attrs,
                                   const uint8_t *material,
                                   const size_t material_len,
                                   uint8_t *out_buf,
                                   const size_t out_buf_size,
                                   size_t *out_len)
{
    if (attrs == NULL || out_buf == NULL || out_len == NULL ||
        (material == NULL && material_len != 0)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    const size_t total = psa_persistent_key_storage_blob_size(material_len);
    if (out_buf_size < total) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_format_key_data_for_storage(material, material_len, attrs, out_buf);

    *out_len = total;
    return PSA_SUCCESS;
}

psa_status_t esp_psa_key_file_unpack(const uint8_t *blob,
                                     const size_t blob_len,
                                     psa_key_attributes_t *attrs,
                                     const uint8_t **material,
                                     size_t *material_len)
{
    if (blob == NULL || attrs == NULL || material == NULL || material_len == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t *upstream_material = NULL;
    size_t upstream_material_len = 0;
    psa_status_t status = psa_parse_key_data_from_storage(blob, blob_len,
                                                          &upstream_material,
                                                          &upstream_material_len,
                                                          attrs);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Upstream allocates and copies the material into a fresh buffer.
     * Discard the copy and return a pointer into the caller's blob — the
     * material section starts immediately after the fixed-size header, so
     * the bytes are identical. Preserves the zero-copy public contract. */
    *material = (upstream_material_len == 0) ? NULL
                                              : blob + (blob_len - upstream_material_len);
    *material_len = upstream_material_len;

    psa_free_persistent_key_data(upstream_material, upstream_material_len);
    return PSA_SUCCESS;
}
