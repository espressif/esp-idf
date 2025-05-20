/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "esp_tls.h"
#include "esp_log.h"

#include "esp_transport.h"
#include "esp_transport_ssl.h"
#include "esp_transport_internal.h"

#define INVALID_SOCKET (-1)

#define GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t)         \
    transport_esp_tls_t *ssl = ssl_get_context_data(t);  \
    if (!ssl) { return; }

static const char *TAG = "transport_base";

typedef enum {
    TRANS_SSL_INIT = 0,
    TRANS_SSL_CONNECTING,
} transport_ssl_conn_state_t;

/**
 *  mbedtls specific transport data
 */
typedef struct transport_esp_tls {
    esp_tls_t                *tls;
    esp_tls_cfg_t            cfg;
    bool                     ssl_initialized;
    transport_ssl_conn_state_t conn_state;
    int                      sockfd;
#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    esp_tls_client_session_t *session_ticket;
#endif
} transport_esp_tls_t;

/**
 * @brief      Destroys esp-tls transport used in the foundation transport
 *
 * @param[in]  transport esp-tls handle
 */
void esp_transport_esp_tls_destroy(struct transport_esp_tls* transport_esp_tls);

static inline transport_esp_tls_t *ssl_get_context_data(esp_transport_handle_t t)
{
    if (!t) {
        return NULL;
    }
    return (transport_esp_tls_t *)t->data;
}

static int esp_tls_connect_async(esp_transport_handle_t t, const char *host, int port, int timeout_ms, bool is_plain_tcp)
{
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);

    if (ssl->conn_state == TRANS_SSL_INIT) {
        ssl->cfg.timeout_ms = timeout_ms;
        ssl->cfg.is_plain_tcp = is_plain_tcp;
        ssl->cfg.non_block = true;
        ssl->ssl_initialized = true;
        ssl->tls = esp_tls_init();
        if (!ssl->tls) {
            return -1;
        }
        ssl->conn_state = TRANS_SSL_CONNECTING;
        ssl->sockfd = INVALID_SOCKET;
    }
    if (ssl->conn_state == TRANS_SSL_CONNECTING) {
        int progress = esp_tls_conn_new_async(host, strlen(host), port, &ssl->cfg, ssl->tls);
        if (progress >= 0) {
            if (esp_tls_get_conn_sockfd(ssl->tls, &ssl->sockfd) != ESP_OK) {
                ESP_LOGE(TAG, "Error in obtaining socket fd for the session");
                esp_tls_conn_destroy(ssl->tls);
                return -1;
            }
        }
        return progress;

    }
    return 0;
}

static inline int ssl_connect_async(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    return esp_tls_connect_async(t, host, port, timeout_ms, false);
}

static inline int tcp_connect_async(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    return esp_tls_connect_async(t, host, port, timeout_ms, true);
}

static int ssl_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);

    ssl->cfg.timeout_ms = timeout_ms;

    ssl->ssl_initialized = true;
    ssl->tls = esp_tls_init();
    if (ssl->tls == NULL) {
        ESP_LOGE(TAG, "Failed to initialize new connection object");
        capture_tcp_transport_error(t, ERR_TCP_TRANSPORT_NO_MEM);
        return -1;
    }
    if (esp_tls_conn_new_sync(host, strlen(host), port, &ssl->cfg, ssl->tls) <= 0) {
        ESP_LOGE(TAG, "Failed to open a new connection");
        esp_tls_error_handle_t esp_tls_error_handle;
        if (esp_tls_get_error_handle(ssl->tls, &esp_tls_error_handle) == ESP_OK) {
            esp_transport_set_errors(t, esp_tls_error_handle);
        } else {
            ESP_LOGE(TAG, "Error in obtaining the error handle");
        }
        goto exit_failure;
    }

    if (esp_tls_get_conn_sockfd(ssl->tls, &ssl->sockfd) != ESP_OK) {
        ESP_LOGE(TAG, "Error in obtaining socket fd for the session");
        goto exit_failure;
    }
    return 0;

exit_failure:
        esp_tls_conn_destroy(ssl->tls);
        ssl->tls = NULL;
        ssl->sockfd = INVALID_SOCKET;
        return -1;
}

static int tcp_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    esp_tls_last_error_t *err_handle = esp_transport_get_error_handle(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);

    ssl->cfg.timeout_ms = timeout_ms;
    esp_err_t err = esp_tls_plain_tcp_connect(host, strlen(host), port, &ssl->cfg, err_handle, &ssl->sockfd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open a new connection: %d", err);
        err_handle->last_error = err;
        ssl->sockfd = INVALID_SOCKET;
        return -1;
    }
    return 0;
}

static int base_poll_read(esp_transport_handle_t t, int timeout_ms)
{
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);
    int ret = -1;
    int remain = 0;
    struct timeval timeout;
    fd_set readset;
    fd_set errset;
    FD_ZERO(&readset);
    FD_ZERO(&errset);
    FD_SET(ssl->sockfd, &readset);
    FD_SET(ssl->sockfd, &errset);

    if (ssl->tls && (remain = esp_tls_get_bytes_avail(ssl->tls)) > 0) {
        ESP_LOGD(TAG, "remain data in cache, need to read again");
        return remain;
    }
    ret = select(ssl->sockfd + 1, &readset, NULL, &errset, esp_transport_utils_ms_to_timeval(timeout_ms, &timeout));
    // The select() function monitors the socket for readiness to read or write, and checks for errors.
    // If both an error (errset) and readiness (readset/writeset) are detected simultaneously,
    // this code ensures that the pending read data must be handled before we start processing error.
    if (ret == 1 && FD_ISSET(ssl->sockfd, &errset)) {
        int sock_errno = 0;
        uint32_t optlen = sizeof(sock_errno);
        getsockopt(ssl->sockfd, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
        esp_transport_capture_errno(t, sock_errno);
        ESP_LOGE(TAG, "poll_read select error %d, errno = %s, fd = %d", sock_errno, strerror(sock_errno), ssl->sockfd);
        ret = -1;
    } else if (ret == 0) {
        ESP_LOGV(TAG, "poll_read: select - Timeout before any socket was ready!");
    }
    return ret;
}

static int base_poll_write(esp_transport_handle_t t, int timeout_ms)
{
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);
    int ret = -1;
    struct timeval timeout;
    fd_set writeset;
    fd_set errset;
    FD_ZERO(&writeset);
    FD_ZERO(&errset);
    FD_SET(ssl->sockfd, &writeset);
    FD_SET(ssl->sockfd, &errset);
    ret = select(ssl->sockfd + 1, NULL, &writeset, &errset, esp_transport_utils_ms_to_timeval(timeout_ms, &timeout));
    if (ret > 0 && FD_ISSET(ssl->sockfd, &errset)) {
        int sock_errno = 0;
        uint32_t optlen = sizeof(sock_errno);
        getsockopt(ssl->sockfd, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
        esp_transport_capture_errno(t, sock_errno);
        ESP_LOGE(TAG, "poll_write select error %d, errno = %s, fd = %d", sock_errno, strerror(sock_errno), ssl->sockfd);
        ret = -1;
    } else if (ret == 0) {
        ESP_LOGD(TAG, "poll_write: select - Timeout before any socket was ready!");
    }
    return ret;
}

static int ssl_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    int poll;
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);

    if ((poll = esp_transport_poll_write(t, timeout_ms)) <= 0) {
        ESP_LOGW(TAG, "Poll timeout or error, errno=%s, fd=%d, timeout_ms=%d", strerror(errno), ssl->sockfd, timeout_ms);
        return poll;
    }
    int ret = esp_tls_conn_write(ssl->tls, (const unsigned char *) buffer, len);
    if (ret < 0) {
        ESP_LOGE(TAG, "esp_tls_conn_write error, errno=%s", strerror(errno));
        esp_tls_error_handle_t esp_tls_error_handle;
        if (esp_tls_get_error_handle(ssl->tls, &esp_tls_error_handle) == ESP_OK) {
            esp_transport_set_errors(t, esp_tls_error_handle);
        } else {
            ESP_LOGE(TAG, "Error in obtaining the error handle");
        }
    }
    return ret;
}

static int tcp_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    int poll;
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);

    if ((poll = esp_transport_poll_write(t, timeout_ms)) <= 0) {
        ESP_LOGW(TAG, "Poll timeout or error, errno=%s, fd=%d, timeout_ms=%d", strerror(errno), ssl->sockfd, timeout_ms);
        return poll;
    }
    int ret = send(ssl->sockfd, (const unsigned char *) buffer, len, 0);
    if (ret < 0) {
        ESP_LOGE(TAG, "tcp_write error, errno=%s", strerror(errno));
        esp_transport_capture_errno(t, errno);
    }
    return ret;
}

static int ssl_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);

    int poll = esp_transport_poll_read(t, timeout_ms);
    if (poll == -1) {
        return ERR_TCP_TRANSPORT_CONNECTION_FAILED;
    }
    if (poll == 0) {
        return ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT;
    }

    int ret = esp_tls_conn_read(ssl->tls, (unsigned char *)buffer, len);
    if (ret < 0) {
        if (ret == ESP_TLS_ERR_SSL_WANT_READ || ret == ESP_TLS_ERR_SSL_TIMEOUT) {
            ret = ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT;
            ESP_LOGD(TAG, "esp_tls_conn_read error, errno=%s", strerror(errno));
        }
        else {
            ESP_LOGE(TAG, "esp_tls_conn_read error, errno=%s", strerror(errno));
        }

        esp_tls_error_handle_t esp_tls_error_handle;
        if (esp_tls_get_error_handle(ssl->tls, &esp_tls_error_handle) == ESP_OK) {
            esp_transport_set_errors(t, esp_tls_error_handle);
        } else {
            ESP_LOGE(TAG, "Error in obtaining the error handle");
        }
    } else if (ret == 0) {
        if (poll > 0) {
            // no error, socket reads 0 while previously detected as readable -> connection has been closed cleanly
            capture_tcp_transport_error(t, ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN);
        }
        ret = ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN;
    }
    return ret;
}

static int tcp_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);

    int poll = esp_transport_poll_read(t, timeout_ms);
    if (poll == -1) {
        return ERR_TCP_TRANSPORT_CONNECTION_FAILED;
    }
    if (poll == 0) {
        return ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT;
    }

    int ret = recv(ssl->sockfd, (unsigned char *)buffer, len, 0);
    if (ret < 0) {
        ESP_LOGE(TAG, "tcp_read error, errno=%s", strerror(errno));
        esp_transport_capture_errno(t, errno);
        if (errno == EAGAIN) {
            ret = ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT;
        } else {
            ret = ERR_TCP_TRANSPORT_CONNECTION_FAILED;
        }
    } else if (ret == 0) {
        if (poll > 0) {
            // no error, socket reads 0 while previously detected as readable -> connection has been closed cleanly
            capture_tcp_transport_error(t, ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN);
        }
        ret = ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN;
    }
    return ret;
}

static int base_close(esp_transport_handle_t t)
{
    int ret = -1;
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    ESP_STATIC_ANALYZER_CHECK(ssl == NULL, -1);

    if (ssl && ssl->ssl_initialized) {
        ret = esp_tls_conn_destroy(ssl->tls);
        ssl->tls = NULL;
        ssl->conn_state = TRANS_SSL_INIT;
        ssl->ssl_initialized = false;
        ssl->sockfd = INVALID_SOCKET;
    } else if (ssl && ssl->sockfd >= 0) {
        ret = close(ssl->sockfd);
        ssl->sockfd = INVALID_SOCKET;
    }
    return ret;
}

static int base_destroy(esp_transport_handle_t transport)
{
    transport_esp_tls_t *ssl = ssl_get_context_data(transport);
    if (ssl) {
        esp_transport_close(transport);
        esp_transport_destroy_foundation_transport(transport->foundation);

        esp_transport_esp_tls_destroy(transport->data); // okay to pass NULL
    }
    return 0;
}

void esp_transport_ssl_enable_global_ca_store(esp_transport_handle_t t)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.use_global_ca_store = true;
}

void esp_transport_ssl_set_tls_version(esp_transport_handle_t t, esp_tls_proto_ver_t tls_version)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.tls_version = tls_version;
}

#ifdef CONFIG_ESP_TLS_PSK_VERIFICATION
void esp_transport_ssl_set_psk_key_hint(esp_transport_handle_t t, const psk_hint_key_t *psk_hint_key)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.psk_hint_key =  psk_hint_key;
}
#endif

void esp_transport_ssl_set_cert_data(esp_transport_handle_t t, const char *data, int len)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.cacert_pem_buf = (void *)data;
    ssl->cfg.cacert_pem_bytes = len + 1;
}

void esp_transport_ssl_set_cert_data_der(esp_transport_handle_t t, const char *data, int len)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.cacert_buf = (void *)data;
    ssl->cfg.cacert_bytes = len;
}

void esp_transport_ssl_set_client_cert_data(esp_transport_handle_t t, const char *data, int len)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.clientcert_pem_buf = (void *)data;
    ssl->cfg.clientcert_pem_bytes = len + 1;
}

void esp_transport_ssl_set_addr_family(esp_transport_handle_t t, esp_tls_addr_family_t addr_family)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.addr_family = addr_family;
}

#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
void esp_transport_ssl_set_client_key_ecdsa_peripheral(esp_transport_handle_t t, uint8_t ecdsa_efuse_blk)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.use_ecdsa_peripheral = true;
    ssl->cfg.ecdsa_key_efuse_blk = ecdsa_efuse_blk;
}
#endif

void esp_transport_ssl_set_client_cert_data_der(esp_transport_handle_t t, const char *data, int len)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.clientcert_buf = (void *)data;
    ssl->cfg.clientcert_bytes = len;
}

void esp_transport_ssl_set_client_key_data(esp_transport_handle_t t, const char *data, int len)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.clientkey_pem_buf = (void *)data;
    ssl->cfg.clientkey_pem_bytes = len + 1;
}

void esp_transport_ssl_set_client_key_password(esp_transport_handle_t t, const char *password, int password_len)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.clientkey_password = (void *)password;
    ssl->cfg.clientkey_password_len = password_len;
}

void esp_transport_ssl_set_client_key_data_der(esp_transport_handle_t t, const char *data, int len)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.clientkey_buf = (void *)data;
    ssl->cfg.clientkey_bytes = len;
}

#if defined(CONFIG_MBEDTLS_SSL_ALPN) || defined(CONFIG_WOLFSSL_HAVE_ALPN)
void esp_transport_ssl_set_alpn_protocol(esp_transport_handle_t t, const char **alpn_protos)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.alpn_protos = alpn_protos;
}
#endif

void esp_transport_ssl_skip_common_name_check(esp_transport_handle_t t)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.skip_common_name = true;
}

void esp_transport_ssl_set_common_name(esp_transport_handle_t t, const char *common_name)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.common_name = common_name;
}

void esp_transport_ssl_set_ciphersuites_list(esp_transport_handle_t t, const int *ciphersuites_list)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.ciphersuites_list = ciphersuites_list;
}

#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
void esp_transport_ssl_use_secure_element(esp_transport_handle_t t)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.use_secure_element = true;
}
#endif

#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
void esp_transport_ssl_crt_bundle_attach(esp_transport_handle_t t, esp_err_t ((*crt_bundle_attach)(void *conf)))
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.crt_bundle_attach = crt_bundle_attach;
}
#endif

static int base_get_socket(esp_transport_handle_t t)
{
    transport_esp_tls_t *ctx = ssl_get_context_data(t);
    if (ctx) {
        return ctx->sockfd;
    }
    return INVALID_SOCKET;
}

#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
void esp_transport_ssl_set_ds_data(esp_transport_handle_t t, void *ds_data)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.ds_data = ds_data;
}
#endif

void esp_transport_ssl_set_keep_alive(esp_transport_handle_t t, esp_transport_keep_alive_t *keep_alive_cfg)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.keep_alive_cfg = (tls_keep_alive_cfg_t *) keep_alive_cfg;
}

void esp_transport_ssl_set_interface_name(esp_transport_handle_t t, struct ifreq *if_name)
{
    GET_SSL_FROM_TRANSPORT_OR_RETURN(ssl, t);
    ssl->cfg.if_name = if_name;
}

static transport_esp_tls_t *esp_transport_esp_tls_create(void)
{
    transport_esp_tls_t *transport_esp_tls = calloc(1, sizeof(transport_esp_tls_t));
    if (transport_esp_tls == NULL) {
        return NULL;
    }
    transport_esp_tls->sockfd = INVALID_SOCKET;
    return transport_esp_tls;
}

static esp_transport_handle_t esp_transport_base_init(void)
{
    esp_transport_handle_t transport = esp_transport_init();
    if (transport == NULL) {
        return NULL;
    }
    transport->foundation = esp_transport_init_foundation_transport();
    ESP_TRANSPORT_MEM_CHECK(TAG, transport->foundation,
                            free(transport);
                            return NULL);

    transport->data = esp_transport_esp_tls_create();
    ESP_TRANSPORT_MEM_CHECK(TAG, transport->data,
                            free(transport->foundation);
                            free(transport);
                            return NULL)
    return transport;
}

esp_transport_handle_t esp_transport_ssl_init(void)
{
    esp_transport_handle_t ssl_transport = esp_transport_base_init();
    if (ssl_transport == NULL) {
        return NULL;
    }
    ((transport_esp_tls_t *)ssl_transport->data)->cfg.is_plain_tcp = false;
    esp_transport_set_func(ssl_transport, ssl_connect, ssl_read, ssl_write, base_close, base_poll_read, base_poll_write, base_destroy);
    esp_transport_set_async_connect_func(ssl_transport, ssl_connect_async);
    ssl_transport->_get_socket = base_get_socket;
    return ssl_transport;
}


void esp_transport_esp_tls_destroy(struct transport_esp_tls *transport_esp_tls)
{
#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    esp_tls_free_client_session(transport_esp_tls->session_ticket);
#endif
    free(transport_esp_tls);
}

esp_transport_handle_t esp_transport_tcp_init(void)
{
    esp_transport_handle_t tcp_transport = esp_transport_base_init();
    if (tcp_transport == NULL) {
        return NULL;
    }
    ((transport_esp_tls_t *)tcp_transport->data)->cfg.is_plain_tcp = true;
    esp_transport_set_func(tcp_transport, tcp_connect, tcp_read, tcp_write, base_close, base_poll_read, base_poll_write, base_destroy);
    esp_transport_set_async_connect_func(tcp_transport, tcp_connect_async);
    tcp_transport->_get_socket = base_get_socket;
    return tcp_transport;
}

void esp_transport_tcp_set_keep_alive(esp_transport_handle_t t, esp_transport_keep_alive_t *keep_alive_cfg)
{
    return esp_transport_ssl_set_keep_alive(t, keep_alive_cfg);
}

void esp_transport_tcp_set_interface_name(esp_transport_handle_t t, struct ifreq *if_name)
{
    return esp_transport_ssl_set_interface_name(t, if_name);
}

#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
esp_err_t esp_transport_ssl_session_ticket_operation(esp_transport_handle_t t, esp_transport_session_ticket_operation_t operation)
{
    transport_esp_tls_t *ssl = ssl_get_context_data(t);
    if (!ssl) {
        return ESP_FAIL;
    }
    switch (operation) {
        case ESP_TRANSPORT_SESSION_TICKET_INIT:
            break;
        case ESP_TRANSPORT_SESSION_TICKET_SAVE:
            esp_tls_free_client_session(ssl->session_ticket);
            ssl->session_ticket = esp_tls_get_client_session(ssl->tls);
            break;
        case ESP_TRANSPORT_SESSION_TICKET_USE:
            if (ssl->session_ticket == NULL) {
                return ESP_ERR_INVALID_STATE;
            }
            ssl->cfg.client_session = ssl->session_ticket;
            break;
        case ESP_TRANSPORT_SESSION_TICKET_FREE:
            esp_tls_free_client_session(ssl->session_ticket);
            ssl->session_ticket = NULL;
            break;
    }
    return ESP_OK;
}
#endif // CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
