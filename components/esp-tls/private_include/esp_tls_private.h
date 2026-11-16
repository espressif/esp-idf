/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <netdb.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "esp_err.h"
#include "esp_tls_errors.h"
#include "esp_tls.h"
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#ifdef CONFIG_ESP_TLS_SERVER_SESSION_TICKETS
#include "mbedtls/ssl_ticket.h"
#endif /* CONFIG_ESP_TLS_SERVER_SESSION_TICKETS */
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#include "psa/crypto.h"
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */
#endif /* CONFIG_ESP_TLS_USING_MBEDTLS */

/**
 * @brief      ESP-TLS Connection Handle
 */
struct esp_tls {
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
    mbedtls_ssl_context ssl;                                                    /*!< TLS/SSL context */

    mbedtls_ssl_config conf;                                                    /*!< TLS/SSL configuration to be shared
                                                                                     between mbedtls_ssl_context
                                                                                     structures */

    mbedtls_net_context server_fd;                                              /*!< mbedTLS wrapper type for sockets */

    mbedtls_x509_crt cacert;                                                    /*!< Container for the X.509 CA certificate */

    mbedtls_x509_crt *cacert_ptr;                                               /*!< Pointer to the cacert being used. */
    union {
    mbedtls_x509_crt clientcert;                                                /*!< Container for the X.509 client certificate */
    mbedtls_x509_crt servercert;                                                /*!< Container for the X.509 server certificate */
    };

    union {
    mbedtls_pk_context clientkey;                                               /*!< Container for the private key of the client
                                                                                     certificate */
    mbedtls_pk_context serverkey;                                               /*!< Container for the private key of the server
                                                                                     certificate */
    };

    bool opaque_key_is_external;                                                /*!< True when the opaque PSA client/server key was supplied
                                                                                     by the caller (ESP_KEY_SOURCE_PSA). Such keys are owned
                                                                                     externally and must never be destroyed on cleanup. */
#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
    bool use_ecdsa_peripheral;                                                  /*!< Use the ECDSA peripheral for the private key operations. */
    uint8_t ecdsa_efuse_blk;                                                    /*!< The efuse block number where the ECDSA key is stored. */
    esp_tls_ecdsa_curve_t ecdsa_curve;                                          /*!< ECDSA curve to use (SECP256R1 or SECP384R1) */
#endif
#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    unsigned char *client_session;                                              /*!< Pointer for the serialized client session ticket context. */
    size_t client_session_len;                                                  /*!< Length of the serialized client session ticket context. */
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS */
#elif CONFIG_ESP_TLS_CUSTOM_STACK
    void *priv_ctx;                                                             /*!< Private context for custom TLS stack (e.g., SSL_CTX*) */
    void *priv_ssl;                                                             /*!< Private SSL handle for custom TLS stack (e.g., SSL*) */
#endif
    int sockfd;                                                                 /*!< Underlying socket file descriptor. */

    ssize_t (*read)(esp_tls_t  *tls, char *data, size_t datalen);          /*!< Callback function for reading data from TLS/SSL
                                                                                     connection. */

    ssize_t (*write)(esp_tls_t *tls, const char *data, size_t datalen);    /*!< Callback function for writing data to TLS/SSL
                                                                                     connection. */

    esp_tls_conn_state_t  conn_state;                                           /*!< ESP-TLS Connection state */

    fd_set rset;                                                                /*!< read file descriptors */

    fd_set wset;                                                                /*!< write file descriptors */

    bool is_tls;                                                                /*!< indicates connection type (TLS or NON-TLS) */

    esp_tls_role_t role;                                                        /*!< esp-tls role
                                                                                     - ESP_TLS_CLIENT
                                                                                     - ESP_TLS_SERVER */

    esp_tls_error_handle_t error_handle;                                        /*!< handle to error descriptor */

    struct addrinfo *addrinfo;                                                  /*!< Resolver results owned while connecting asynchronously */
    struct addrinfo *next_addrinfo;                                             /*!< Next address candidate for an async connect */
    bool connect_pending;                                                       /*!< Whether the current async TCP connect is pending */
    uint64_t connect_start_time_us;                                             /*!< Start time for the complete address iteration */
    uint64_t connect_attempt_start_time_us;                                     /*!< Start time for the current async address */
    int connect_attempt_timeout_ms;                                             /*!< Timeout budget for the current address */
    int connect_errno;                                                          /*!< Last address-specific connect error */
    esp_err_t connect_error;                                                    /*!< Last address-specific ESP-TLS error */

#if CONFIG_MBEDTLS_DYNAMIC_BUFFER
    esp_tls_dyn_buf_strategy_t esp_tls_dyn_buf_strategy;                        /*!< ESP-TLS dynamic buffer strategy */
#endif

};

/* Internal helper used by the ESP-TLS tests to exercise address fallback. */
esp_err_t esp_tls_tcp_connect_addrinfo(const struct addrinfo *address_info, int port,
                                       const esp_tls_cfg_t *cfg,
                                       esp_tls_error_handle_t error_handle, int *sockfd);
esp_err_t esp_tls_connect_next_address(esp_tls_t *tls, int port, const esp_tls_cfg_t *cfg);
int esp_tls_poll_connect_address(esp_tls_t *tls, int port, const esp_tls_cfg_t *cfg);

// Function pointer for the server configuration API
typedef esp_err_t (*set_server_config_func_ptr) (esp_tls_cfg_server_t *cfg, esp_tls_t *tls);

// This struct contains any data that is only specific to the server session and not required by the client.
typedef struct esp_tls_server_params {
    set_server_config_func_ptr set_server_cfg;
} esp_tls_server_params_t;

#define ESP_TLS_DEFAULT_SERVER_HANDSHAKE_TIMEOUT_MS  (10000) /*!< Default handshake timeout in milliseconds */
