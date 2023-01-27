/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_tls.h"
#include "esp_tls_error_capture_internal.h"

#if CONFIG_IDF_TARGET_LINUX
#include "esp_linux_helper.h"
#endif

typedef struct esp_tls_error_storage {
    struct esp_tls_last_error parent;   /*!< standard esp-tls last error container */
    int    sock_errno;                  /*!< last socket error captured in esp-tls */
} esp_tls_error_storage_t;

void esp_tls_internal_event_tracker_capture(esp_tls_error_handle_t h, uint32_t type, int code)
{
    if (h) {
        esp_tls_error_storage_t * storage = __containerof(h, esp_tls_error_storage_t, parent);

        if (type == ESP_TLS_ERR_TYPE_ESP) {
            storage->parent.last_error = code;
        } else if (type == ESP_TLS_ERR_TYPE_MBEDTLS ||
                   type == ESP_TLS_ERR_TYPE_WOLFSSL) {
            storage->parent.esp_tls_error_code = code;
        } else if (type == ESP_TLS_ERR_TYPE_MBEDTLS_CERT_FLAGS ||
                   type == ESP_TLS_ERR_TYPE_WOLFSSL_CERT_FLAGS) {
            storage->parent.esp_tls_flags = code;
        } else if (type == ESP_TLS_ERR_TYPE_SYSTEM) {
            storage->sock_errno = code;
        }
    }
}

esp_tls_error_handle_t esp_tls_internal_event_tracker_create(void)
{
    // Allocating internal error storage which extends the parent type
    // `esp_tls_last_error` defined at interface level
    struct esp_tls_error_storage* storage =
            calloc(1, sizeof(struct esp_tls_error_storage));
    return &storage->parent;
}

void esp_tls_internal_event_tracker_destroy(esp_tls_error_handle_t h)
{
    esp_tls_error_storage_t * storage = __containerof(h, esp_tls_error_storage_t, parent);
    free(storage);
}

esp_err_t esp_tls_get_and_clear_error_type(esp_tls_error_handle_t h, esp_tls_error_type_t type, int *code)
{
    if (h && type < ESP_TLS_ERR_TYPE_MAX && code) {
        esp_tls_error_storage_t * storage = __containerof(h, esp_tls_error_storage_t, parent);
        if (type == ESP_TLS_ERR_TYPE_ESP) {
            *code = storage->parent.last_error;
            storage->parent.last_error = 0;
        } else if (type == ESP_TLS_ERR_TYPE_MBEDTLS ||
                   type == ESP_TLS_ERR_TYPE_WOLFSSL) {
            *code = storage->parent.esp_tls_error_code;
            storage->parent.esp_tls_error_code = 0;
        } else if (type == ESP_TLS_ERR_TYPE_MBEDTLS_CERT_FLAGS ||
                   type == ESP_TLS_ERR_TYPE_WOLFSSL_CERT_FLAGS) {
            *code = storage->parent.esp_tls_flags;
            storage->parent.esp_tls_flags = 0;
        } else if (type == ESP_TLS_ERR_TYPE_SYSTEM) {
            *code = storage->sock_errno;
            storage->sock_errno = 0;
        } else {
            return ESP_ERR_INVALID_ARG;
        }
        return ESP_OK;
    }
    return ESP_ERR_INVALID_ARG;
}
