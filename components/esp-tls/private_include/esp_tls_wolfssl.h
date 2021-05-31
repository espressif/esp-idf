/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_tls.h"

/**
 * Internal Callback for creating ssl handle for wolfssl
 */
int esp_create_wolfssl_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls);

/**
 * Internal Callback for wolfssl_handshake
 */
int esp_wolfssl_handshake(esp_tls_t *tls, const esp_tls_cfg_t *cfg);

/**
 * Internal Callback API for wolfssl_ssl_read
 */
ssize_t esp_wolfssl_read(esp_tls_t *tls, char *data, size_t datalen);

/**
 * Internal callback API for wolfssl_ssl_write
 */
ssize_t esp_wolfssl_write(esp_tls_t *tls, const char *data, size_t datalen);

/**
 * Internal Callback for wolfssl_cleanup , frees up all the memory used by wolfssl
 */
void esp_wolfssl_cleanup(esp_tls_t *tls);

/**
 * Internal Callback for Certificate verification for wolfssl
 */
void esp_wolfssl_verify_certificate(esp_tls_t *tls);

/**
 * Internal Callback for deleting the wolfssl connection
 */
void esp_wolfssl_conn_delete(esp_tls_t *tls);

/**
 * Internal Callback for wolfssl_get_bytes_avail
 */
ssize_t esp_wolfssl_get_bytes_avail(esp_tls_t *tls);

/**
 * Callback function for setting global CA store data for TLS/SSL using wolfssl
 */
esp_err_t esp_wolfssl_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes);

/**
 * Callback function for freeing global ca store for TLS/SSL using wolfssl
 */
void esp_wolfssl_free_global_ca_store(void);

/**
 *
 * Callback function for Initializing the global ca store for TLS?SSL using wolfssl
 */
esp_err_t esp_wolfssl_init_global_ca_store(void);

/**
 * wolfSSL function for Initializing socket wrappers (no-operation for wolfSSL)
 */
static inline void esp_wolfssl_net_init(esp_tls_t *tls)
{
}

#ifdef CONFIG_ESP_TLS_SERVER

/**
 * Function to Create ESP-TLS Server session with wolfssl Stack
 */
int esp_wolfssl_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls);

/*
 * Delete Server Session
 */
void esp_wolfssl_server_session_delete(esp_tls_t *tls);

#endif
