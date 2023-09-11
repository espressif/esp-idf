/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @brief      ESP-TLS Connection Handle
 */

#include <stdbool.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "esp_err.h"
#include "esp_tls_errors.h"
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#ifdef CONFIG_ESP_TLS_SERVER_SESSION_TICKETS
#include "mbedtls/ssl_ticket.h"
#endif
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#include "psa/crypto.h"
#endif
#elif CONFIG_ESP_TLS_USING_WOLFSSL
#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/ssl.h"
#endif

struct esp_tls {
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
    mbedtls_ssl_context ssl;                                                    /*!< TLS/SSL context */

    mbedtls_entropy_context entropy;                                            /*!< mbedTLS entropy context structure */

    mbedtls_ctr_drbg_context ctr_drbg;                                          /*!< mbedTLS ctr drbg context structure.
                                                                                     CTR_DRBG is deterministic random
                                                                                     bit generation based on AES-256 */

    mbedtls_ssl_config conf;                                                    /*!< TLS/SSL configuration to be shared
                                                                                     between mbedtls_ssl_context
                                                                                     structures */

    mbedtls_net_context server_fd;                                              /*!< mbedTLS wrapper type for sockets */

    mbedtls_x509_crt cacert;                                                    /*!< Container for the X.509 CA certificate */

    mbedtls_x509_crt *cacert_ptr;                                               /*!< Pointer to the cacert being used. */

    mbedtls_x509_crt clientcert;                                                /*!< Container for the X.509 client certificate */

    mbedtls_pk_context clientkey;                                               /*!< Container for the private key of the client
                                                                                     certificate */
#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
    bool use_ecdsa_peripheral;                                                  /*!< Use the ECDSA peripheral for the private key operations. */
    uint8_t ecdsa_efuse_blk;                                                    /*!< The efuse block number where the ECDSA key is stored. */
#endif
#ifdef CONFIG_ESP_TLS_SERVER
    mbedtls_x509_crt servercert;                                                /*!< Container for the X.509 server certificate */

    mbedtls_pk_context serverkey;                                               /*!< Container for the private key of the server
                                                                                   certificate */
#endif
#elif CONFIG_ESP_TLS_USING_WOLFSSL
    void *priv_ctx;
    void *priv_ssl;
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

};
