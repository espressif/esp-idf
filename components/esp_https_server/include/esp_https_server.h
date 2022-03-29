/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_HTTPS_SERVER_H_
#define _ESP_HTTPS_SERVER_H_

#include <stdbool.h>
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_tls.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HTTPD_SSL_TRANSPORT_SECURE,      // SSL Enabled
    HTTPD_SSL_TRANSPORT_INSECURE     // SSL disabled
} httpd_ssl_transport_mode_t;

/**
 * @brief Indicates the state at which the user callback is executed,
 *        i.e at session creation or session close
 */
typedef enum {
    HTTPD_SSL_USER_CB_SESS_CREATE,
    HTTPD_SSL_USER_CB_SESS_CLOSE
} httpd_ssl_user_cb_state_t;

/**
 * @brief Callback data struct, contains the ESP-TLS connection handle
 * and the connection state at which the callback is executed
 */
typedef struct esp_https_server_user_cb_arg {
    httpd_ssl_user_cb_state_t user_cb_state;
    const esp_tls_t *tls;
} esp_https_server_user_cb_arg_t;

/**
 * @brief Callback function prototype
 * Can be used to get connection or client information (SSL context)
 * E.g. Client certificate, Socket FD, Connection state, etc.
 *
 * @param user_cb Callback data struct
 */
typedef void esp_https_server_user_cb(esp_https_server_user_cb_arg_t *user_cb);

/**
 * HTTPS server config struct
 *
 * Please use HTTPD_SSL_CONFIG_DEFAULT() to initialize it.
 */
struct httpd_ssl_config {
    /**
     * Underlying HTTPD server config
     *
     * Parameters like task stack size and priority can be adjusted here.
     */
    httpd_config_t httpd;

    /** Server certificate */
    const uint8_t *servercert;

    /** Server certificate byte length */
    size_t servercert_len;

    /** CA certificate ((CA used to sign clients, or client cert itself) */
    const uint8_t *cacert_pem;

    /** CA certificate byte length */
    size_t cacert_len;

    /** Private key */
    const uint8_t *prvtkey_pem;

    /** Private key byte length */
    size_t prvtkey_len;

    /** Transport Mode (default secure) */
    httpd_ssl_transport_mode_t transport_mode;

    /** Port used when transport mode is secure (default 443) */
    uint16_t port_secure;

    /** Port used when transport mode is insecure (default 80) */
    uint16_t port_insecure;

    /** Enable tls session tickets */
    bool session_tickets;

    /** Enable secure element for server session */
    bool use_secure_element;

    /** User callback for esp_https_server */
    esp_https_server_user_cb *user_cb;
};

typedef struct httpd_ssl_config httpd_ssl_config_t;

/**
 * Default config struct init
 *
 * (http_server default config had to be copied for customization)
 *
 * Notes:
 * - port is set when starting the server, according to 'transport_mode'
 * - one socket uses ~ 40kB RAM with SSL, we reduce the default socket count to 4
 * - SSL sockets are usually long-lived, closing LRU prevents pool exhaustion DOS
 * - Stack size may need adjustments depending on the user application
 */
#define HTTPD_SSL_CONFIG_DEFAULT() {              \
    .httpd = {                                    \
        .task_priority      = tskIDLE_PRIORITY+5, \
        .stack_size         = 10240,              \
        .core_id            = tskNO_AFFINITY,     \
        .server_port        = 0,                  \
        .ctrl_port          = 32768,              \
        .max_open_sockets   = 4,                  \
        .max_uri_handlers   = 8,                  \
        .max_resp_headers   = 8,                  \
        .backlog_conn       = 5,                  \
        .lru_purge_enable   = true,               \
        .recv_wait_timeout  = 5,                  \
        .send_wait_timeout  = 5,                  \
        .global_user_ctx = NULL,                  \
        .global_user_ctx_free_fn = NULL,          \
        .global_transport_ctx = NULL,             \
        .global_transport_ctx_free_fn = NULL,     \
        .open_fn = NULL,                          \
        .close_fn = NULL,                         \
        .uri_match_fn = NULL                      \
    },                                            \
    .servercert = NULL,                           \
    .servercert_len = 0,                          \
    .cacert_pem = NULL,                           \
    .cacert_len = 0,                              \
    .prvtkey_pem = NULL,                          \
    .prvtkey_len = 0,                             \
    .transport_mode = HTTPD_SSL_TRANSPORT_SECURE, \
    .port_secure = 443,                           \
    .port_insecure = 80,                          \
    .session_tickets = false,                     \
    .user_cb = NULL,                              \
}

/**
 * Create a SSL capable HTTP server (secure mode may be disabled in config)
 *
 * @param[in,out] config - server config, must not be const. Does not have to stay valid after
 *                         calling this function.
 * @param[out] handle - storage for the server handle, must be a valid pointer
 * @return success
 */
esp_err_t httpd_ssl_start(httpd_handle_t *handle, httpd_ssl_config_t *config);

/**
 * Stop the server. Blocks until the server is shut down.
 *
 * @param[in] handle
 */
void httpd_ssl_stop(httpd_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif // _ESP_HTTPS_SERVER_H_
