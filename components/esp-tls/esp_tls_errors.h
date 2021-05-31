/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_TLS_ERRORS_H_
#define _ESP_TLS_ERRORS_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_ESP_TLS_BASE           0x8000             /*!< Starting number of ESP-TLS error codes */
#define ESP_ERR_ESP_TLS_CANNOT_RESOLVE_HOSTNAME           (ESP_ERR_ESP_TLS_BASE + 0x01)  /*!< Error if hostname couldn't be resolved upon tls connection */
#define ESP_ERR_ESP_TLS_CANNOT_CREATE_SOCKET              (ESP_ERR_ESP_TLS_BASE + 0x02)  /*!< Failed to create socket */
#define ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY       (ESP_ERR_ESP_TLS_BASE + 0x03)  /*!< Unsupported protocol family */
#define ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST            (ESP_ERR_ESP_TLS_BASE + 0x04)  /*!< Failed to connect to host */
#define ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED              (ESP_ERR_ESP_TLS_BASE + 0x05)  /*!< failed to set/get socket option */
#define ESP_ERR_MBEDTLS_CERT_PARTLY_OK                    (ESP_ERR_ESP_TLS_BASE + 0x06)  /*!< mbedtls parse certificates was partly successful */
#define ESP_ERR_MBEDTLS_CTR_DRBG_SEED_FAILED              (ESP_ERR_ESP_TLS_BASE + 0x07)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_SET_HOSTNAME_FAILED           (ESP_ERR_ESP_TLS_BASE + 0x08)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_CONFIG_DEFAULTS_FAILED        (ESP_ERR_ESP_TLS_BASE + 0x09)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_CONF_ALPN_PROTOCOLS_FAILED    (ESP_ERR_ESP_TLS_BASE + 0x0A)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_X509_CRT_PARSE_FAILED             (ESP_ERR_ESP_TLS_BASE + 0x0B)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_CONF_OWN_CERT_FAILED          (ESP_ERR_ESP_TLS_BASE + 0x0C)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_SETUP_FAILED                  (ESP_ERR_ESP_TLS_BASE + 0x0D)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_SSL_WRITE_FAILED                  (ESP_ERR_ESP_TLS_BASE + 0x0E)  /*!< mbedtls api returned error */
#define ESP_ERR_MBEDTLS_PK_PARSE_KEY_FAILED               (ESP_ERR_ESP_TLS_BASE + 0x0F)  /*!< mbedtls api returned failed  */
#define ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED              (ESP_ERR_ESP_TLS_BASE + 0x10)  /*!< mbedtls api returned failed  */
#define ESP_ERR_MBEDTLS_SSL_CONF_PSK_FAILED               (ESP_ERR_ESP_TLS_BASE + 0x11)  /*!< mbedtls api returned failed  */
#define ESP_ERR_ESP_TLS_CONNECTION_TIMEOUT                (ESP_ERR_ESP_TLS_BASE + 0x12)  /*!< new connection in esp_tls_low_level_conn connection timeouted */
#define ESP_ERR_WOLFSSL_SSL_SET_HOSTNAME_FAILED           (ESP_ERR_ESP_TLS_BASE + 0x13)  /*!< wolfSSL api returned error */
#define ESP_ERR_WOLFSSL_SSL_CONF_ALPN_PROTOCOLS_FAILED    (ESP_ERR_ESP_TLS_BASE + 0x14)  /*!< wolfSSL api returned error */
#define ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED          (ESP_ERR_ESP_TLS_BASE + 0x15)  /*!< wolfSSL api returned error */
#define ESP_ERR_WOLFSSL_KEY_VERIFY_SETUP_FAILED           (ESP_ERR_ESP_TLS_BASE + 0x16)  /*!< wolfSSL api returned error */
#define ESP_ERR_WOLFSSL_SSL_HANDSHAKE_FAILED              (ESP_ERR_ESP_TLS_BASE + 0x17)  /*!< wolfSSL api returned failed  */
#define ESP_ERR_WOLFSSL_CTX_SETUP_FAILED                  (ESP_ERR_ESP_TLS_BASE + 0x18)  /*!< wolfSSL api returned failed */
#define ESP_ERR_WOLFSSL_SSL_SETUP_FAILED                  (ESP_ERR_ESP_TLS_BASE + 0x19)  /*!< wolfSSL api returned failed */
#define ESP_ERR_WOLFSSL_SSL_WRITE_FAILED                  (ESP_ERR_ESP_TLS_BASE + 0x1A)  /*!< wolfSSL api returned failed */

#define ESP_ERR_ESP_TLS_SE_FAILED                         (ESP_ERR_ESP_TLS_BASE + 0x1B)  /*< esp-tls use Secure Element returned failed */
#define ESP_ERR_ESP_TLS_TCP_CLOSED_FIN                    (ESP_ERR_ESP_TLS_BASE + 0x1C)  /*< esp-tls's TPC transport connection has benn closed (in a clean way) */

/**
* Definition of errors reported from IO API (potentially non-blocking) in case of error:
* - esp_tls_conn_read()
* - esp_tls_conn_write()
*/
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#define ESP_TLS_ERR_SSL_WANT_READ                          MBEDTLS_ERR_SSL_WANT_READ
#define ESP_TLS_ERR_SSL_WANT_WRITE                         MBEDTLS_ERR_SSL_WANT_WRITE
#define ESP_TLS_ERR_SSL_TIMEOUT                            MBEDTLS_ERR_SSL_TIMEOUT
#elif CONFIG_ESP_TLS_USING_WOLFSSL /* CONFIG_ESP_TLS_USING_MBEDTLS */
#define ESP_TLS_ERR_SSL_WANT_READ                          -0x6900
#define ESP_TLS_ERR_SSL_WANT_WRITE                         -0x6880
#define ESP_TLS_ERR_SSL_TIMEOUT                            WOLFSSL_CBIO_ERR_TIMEOUT
#endif /*CONFIG_ESP_TLS_USING_WOLFSSL */

/**
* Definition of different types/sources of error codes reported
* from different components
*/
typedef enum {
    ESP_TLS_ERR_TYPE_UNKNOWN = 0,
    ESP_TLS_ERR_TYPE_SYSTEM,                /*!< System error -- errno */
    ESP_TLS_ERR_TYPE_MBEDTLS,               /*!< Error code from mbedTLS library */
    ESP_TLS_ERR_TYPE_MBEDTLS_CERT_FLAGS,    /*!< Certificate flags defined in mbedTLS */
    ESP_TLS_ERR_TYPE_ESP,                   /*!< ESP-IDF error type -- esp_err_t  */
    ESP_TLS_ERR_TYPE_WOLFSSL,               /*!< Error code from wolfSSL library */
    ESP_TLS_ERR_TYPE_WOLFSSL_CERT_FLAGS,    /*!< Certificate flags defined in wolfSSL */
    ESP_TLS_ERR_TYPE_MAX,                   /*!< Last err type -- invalid entry */
} esp_tls_error_type_t;

typedef struct esp_tls_last_error* esp_tls_error_handle_t;

/**
*  @brief Error structure containing relevant errors in case tls error occurred
*/
typedef struct esp_tls_last_error {
    esp_err_t last_error;               /*!< error code (based on ESP_ERR_ESP_TLS_BASE) of the last occurred error */
    int       esp_tls_error_code;       /*!< esp_tls error code from last esp_tls failed api */
    int       esp_tls_flags;            /*!< last certification verification flags */
} esp_tls_last_error_t;

#ifdef __cplusplus
}
#endif

#endif //_ESP_TLS_ERRORS_H_
