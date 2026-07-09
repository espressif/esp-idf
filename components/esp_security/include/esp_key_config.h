/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Key format */
typedef enum {
    ESP_KEY_FORMAT_AUTO = 0,            /*!< Auto-detect */
    ESP_KEY_FORMAT_PEM,                 /*!< PEM (base64) */
    ESP_KEY_FORMAT_DER,                 /*!< DER (binary) */
    ESP_KEY_FORMAT_RAW,                 /*!< Raw key bytes */
} esp_key_format_t;

/**
 * Key source types
 *
 * Hardware-backed key sources (DS peripheral, ECDSA peripheral,
 * secure element, key manager) as well as software PSA imported keys
 * are accessed via PSA Crypto drivers. Use ESP_KEY_SOURCE_PSA with
 * the PSA key ID obtained from psa_import_key() or the corresponding
 * hardware setup helper API.
 */
typedef enum {
    ESP_KEY_SOURCE_NONE = 0,            /*!< No private key configured */
    ESP_KEY_SOURCE_BUFFER,              /*!< Key in memory buffer (PEM/DER/RAW) */
    ESP_KEY_SOURCE_PSA,                 /*!< PSA Crypto key (opaque or transparent) */
} esp_key_source_t;

/**
 * Unified private key configuration
 *
 * For hardware-backed keys (DS peripheral, ECDSA peripheral, ATECC608,
 * key manager), use ESP_KEY_SOURCE_PSA with the key ID returned by
 * the respective setup helper (e.g., esp_secure_element_psa_setup()).
 *
 * @note Must remain valid for the entire TLS session lifetime
 */
typedef struct esp_key_config {
    esp_key_source_t source;            /*!< Key source type */

    union {
        struct {
            const void *data;           /*!< Key data (PEM/DER/RAW) */
            size_t len;                 /*!< Length (0 for null-terminated PEM) */
            const char *password;       /*!< Decryption password */
            size_t password_len;        /*!< Password length */
            esp_key_format_t format;    /*!< Key format */
        } buffer;

        struct {
            uint32_t key_id;            /*!< PSA key identifier */
        } psa;
    };

} esp_key_config_t;

#ifdef __cplusplus
}
#endif
