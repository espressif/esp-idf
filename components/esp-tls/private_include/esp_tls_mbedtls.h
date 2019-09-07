// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include "esp_tls.h"

/**
 * Internal Callback API for mbedtls_ssl_read
 */
ssize_t esp_mbedtls_read(esp_tls_t *tls, char *data, size_t datalen);

/**
 * Internal callback API for mbedtls_ssl_write
 */
ssize_t esp_mbedtls_write(esp_tls_t *tls, const char *data, size_t datalen);

/**
 * Internal Callback for mbedtls_handshake
 */
int esp_mbedtls_handshake(esp_tls_t *tls, const esp_tls_cfg_t *cfg);

/**
 * Internal Callback for mbedtls_cleanup , frees up all the memory used by mbedtls
 */
void esp_mbedtls_cleanup(esp_tls_t *tls);

/**
 * Internal Callback for Certificate verification for mbedtls
 */
void esp_mbedtls_verify_certificate(esp_tls_t *tls);

/**
 * Internal Callback for deleting the mbedtls connection
 */
void esp_mbedtls_conn_delete(esp_tls_t *tls);

/**
 * Internal Callback for mbedtls_get_bytes_avail
 */
ssize_t esp_mbedtls_get_bytes_avail(esp_tls_t *tls);

/**
 * Internal Callback for creating ssl handle for mbedtls
 */
esp_err_t esp_create_mbedtls_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls);

#ifdef CONFIG_ESP_TLS_SERVER
/**
 * Internal Callback for set_server_config
 *
 * /note :- can only be used with mbedtls ssl library
 */
esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls);

/**
 * Internal Callback for mbedtls_server_session_create
 *
 * /note :- The function can only be used with mbedtls ssl library
 */
int esp_mbedtls_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls);

/**
 * Internal Callback for mbedtls_server_session_delete
 *
 * /note :- The function can only be used with mbedtls ssl library
 */
void esp_mbedtls_server_session_delete(esp_tls_t *tls);
#endif

/**
 * Internal Callback for set_client_config_function
 */
esp_err_t set_client_config(const char *hostname, size_t hostlen, esp_tls_cfg_t *cfg, esp_tls_t *tls);

/**
 * Internal Callback for mbedtls_init_global_ca_store
 */
esp_err_t esp_mbedtls_init_global_ca_store(void);

/**
 * Callback function for setting global CA store data for TLS/SSL using mbedtls
 */
esp_err_t esp_mbedtls_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes);

/**
 * Internal Callback for esp_tls_global_ca_store
 */
mbedtls_x509_crt *esp_mbedtls_get_global_ca_store(void);

/**
 * Callback function for freeing global ca store for TLS/SSL using mbedtls
 */
void esp_mbedtls_free_global_ca_store(void);
