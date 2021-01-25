// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include <stdlib.h>

#include "esp_tls.h"
#include "esp_log.h"

#include "esp_transport.h"
#include "esp_transport_ssl.h"
#include "esp_transport_utils.h"
#include "esp_transport_internal.h"

static const char *TAG = "TRANSPORT_BASE";

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
} transport_esp_tls_t;

static int ssl_close(esp_transport_handle_t t);

static int esp_tls_connect_async(esp_transport_handle_t t, const char *host, int port, int timeout_ms, bool is_plain_tcp)
{
    transport_esp_tls_t *ssl = t->base->transport_esp_tls;
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
    }
    if (ssl->conn_state == TRANS_SSL_CONNECTING) {
        return esp_tls_conn_new_async(host, strlen(host), port, &ssl->cfg, ssl->tls);
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

static int esp_tls_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms, bool is_plain_tcp)
{
    transport_esp_tls_t *ssl = t->base->transport_esp_tls;

    ssl->cfg.timeout_ms = timeout_ms;
    ssl->cfg.is_plain_tcp = is_plain_tcp;

    ssl->ssl_initialized = true;
    ssl->tls = esp_tls_init();
    if (esp_tls_conn_new_sync(host, strlen(host), port, &ssl->cfg, ssl->tls) <= 0) {
        ESP_LOGE(TAG, "Failed to open a new connection");
        esp_transport_set_errors(t, ssl->tls->error_handle);
        esp_tls_conn_destroy(ssl->tls);
        ssl->tls = NULL;
        return -1;
    }
    return 0;
}

static inline int ssl_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    return esp_tls_connect(t, host, port, timeout_ms, false);
}

static inline int tcp_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    return esp_tls_connect(t, host, port, timeout_ms, true);
}

static int ssl_poll_read(esp_transport_handle_t t, int timeout_ms)
{
    transport_esp_tls_t *ssl = t->base->transport_esp_tls;
    int ret = -1;
    int remain = 0;
    struct timeval timeout;
    fd_set readset;
    fd_set errset;
    FD_ZERO(&readset);
    FD_ZERO(&errset);
    FD_SET(ssl->tls->sockfd, &readset);
    FD_SET(ssl->tls->sockfd, &errset);

    if ((remain = esp_tls_get_bytes_avail(ssl->tls)) > 0) {
        ESP_LOGD(TAG, "remain data in cache, need to read again");
        return remain;
    }
    ret = select(ssl->tls->sockfd + 1, &readset, NULL, &errset, esp_transport_utils_ms_to_timeval(timeout_ms, &timeout));
    if (ret > 0 && FD_ISSET(ssl->tls->sockfd, &errset)) {
        int sock_errno = 0;
        uint32_t optlen = sizeof(sock_errno);
        getsockopt(ssl->tls->sockfd, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
        esp_transport_capture_errno(t, sock_errno);
        ESP_LOGE(TAG, "ssl_poll_read select error %d, errno = %s, fd = %d", sock_errno, strerror(sock_errno), ssl->tls->sockfd);
        ret = -1;
    }
    return ret;
}

static int ssl_poll_write(esp_transport_handle_t t, int timeout_ms)
{
    transport_esp_tls_t *ssl = t->base->transport_esp_tls;
    int ret = -1;
    struct timeval timeout;
    fd_set writeset;
    fd_set errset;
    FD_ZERO(&writeset);
    FD_ZERO(&errset);
    FD_SET(ssl->tls->sockfd, &writeset);
    FD_SET(ssl->tls->sockfd, &errset);
    ret = select(ssl->tls->sockfd + 1, NULL, &writeset, &errset, esp_transport_utils_ms_to_timeval(timeout_ms, &timeout));
    if (ret > 0 && FD_ISSET(ssl->tls->sockfd, &errset)) {
        int sock_errno = 0;
        uint32_t optlen = sizeof(sock_errno);
        getsockopt(ssl->tls->sockfd, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
        esp_transport_capture_errno(t, sock_errno);
        ESP_LOGE(TAG, "ssl_poll_write select error %d, errno = %s, fd = %d", sock_errno, strerror(sock_errno), ssl->tls->sockfd);
        ret = -1;
    }
    return ret;
}

static int ssl_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    int poll, ret;
    transport_esp_tls_t *ssl = t->base->transport_esp_tls;

    if ((poll = esp_transport_poll_write(t, timeout_ms)) <= 0) {
        ESP_LOGW(TAG, "Poll timeout or error, errno=%s, fd=%d, timeout_ms=%d", strerror(errno), ssl->tls->sockfd, timeout_ms);
        return poll;
    }
    ret = esp_tls_conn_write(ssl->tls, (const unsigned char *) buffer, len);
    if (ret < 0) {
        ESP_LOGE(TAG, "esp_tls_conn_write error, errno=%s", strerror(errno));
        esp_transport_set_errors(t, ssl->tls->error_handle);
    }
    return ret;
}

static int ssl_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    int poll, ret;
    transport_esp_tls_t *ssl = t->base->transport_esp_tls;

    if ((poll = esp_transport_poll_read(t, timeout_ms)) <= 0) {
        return poll;
    }
    ret = esp_tls_conn_read(ssl->tls, (unsigned char *)buffer, len);
    if (ret < 0) {
        ESP_LOGE(TAG, "esp_tls_conn_read error, errno=%s", strerror(errno));
        esp_transport_set_errors(t, ssl->tls->error_handle);
    }
    if (ret == 0) {
        if (poll > 0) {
            // no error, socket reads 0 while previously detected as readable -> connection has been closed cleanly
            capture_tcp_transport_error(t, ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN);
        }
        ret = -1;
    }
    return ret;
}

static int ssl_close(esp_transport_handle_t t)
{
    int ret = -1;
    if (t && t->base && t->base->transport_esp_tls && t->base->transport_esp_tls->ssl_initialized) {
        transport_esp_tls_t *ssl = t->base->transport_esp_tls;
        ret = esp_tls_conn_destroy(ssl->tls);
        ssl->conn_state = TRANS_SSL_INIT;
        ssl->ssl_initialized = false;
    }
    return ret;
}

static int ssl_destroy(esp_transport_handle_t t)
{
    esp_transport_close(t);
    return 0;
}

void esp_transport_ssl_enable_global_ca_store(esp_transport_handle_t t)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.use_global_ca_store = true;
    }
}

void esp_transport_ssl_set_psk_key_hint(esp_transport_handle_t t, const psk_hint_key_t* psk_hint_key)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.psk_hint_key =  psk_hint_key;
    }
}

void esp_transport_ssl_set_cert_data(esp_transport_handle_t t, const char *data, int len)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.cacert_pem_buf = (void *)data;
        t->base->transport_esp_tls->cfg.cacert_pem_bytes = len + 1;
    }
}

void esp_transport_ssl_set_cert_data_der(esp_transport_handle_t t, const char *data, int len)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.cacert_buf = (void *)data;
        t->base->transport_esp_tls->cfg.cacert_bytes = len;
    }
}

void esp_transport_ssl_set_client_cert_data(esp_transport_handle_t t, const char *data, int len)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.clientcert_pem_buf = (void *)data;
        t->base->transport_esp_tls->cfg.clientcert_pem_bytes = len + 1;
    }
}

void esp_transport_ssl_set_client_cert_data_der(esp_transport_handle_t t, const char *data, int len)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.clientcert_buf = (void *)data;
        t->base->transport_esp_tls->cfg.clientcert_bytes = len;
    }
}

void esp_transport_ssl_set_client_key_data(esp_transport_handle_t t, const char *data, int len)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.clientkey_pem_buf = (void *)data;
        t->base->transport_esp_tls->cfg.clientkey_pem_bytes = len + 1;
    }
}

void esp_transport_ssl_set_client_key_password(esp_transport_handle_t t, const char *password, int password_len)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.clientkey_password = (void *)password;
        t->base->transport_esp_tls->cfg.clientkey_password_len = password_len;
    }
}

void esp_transport_ssl_set_client_key_data_der(esp_transport_handle_t t, const char *data, int len)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.clientkey_buf = (void *)data;
        t->base->transport_esp_tls->cfg.clientkey_bytes = len;
    }
}

void esp_transport_ssl_set_alpn_protocol(esp_transport_handle_t t, const char **alpn_protos)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.alpn_protos = alpn_protos;
    }
}

void esp_transport_ssl_skip_common_name_check(esp_transport_handle_t t)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.skip_common_name = true;
    }
}

void esp_transport_ssl_use_secure_element(esp_transport_handle_t t)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.use_secure_element = true;
    }
}

static int ssl_get_socket(esp_transport_handle_t t)
{
    if (t && t->base && t->base->transport_esp_tls && t->base->transport_esp_tls->tls) {
        return t->base->transport_esp_tls->tls->sockfd;
    }
    return -1;
}

void esp_transport_ssl_set_ds_data(esp_transport_handle_t t, void *ds_data)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.ds_data = ds_data;
    }
}

void esp_transport_ssl_set_keep_alive(esp_transport_handle_t t, esp_transport_keep_alive_t *keep_alive_cfg)
{
    if (t && t->base && t->base->transport_esp_tls) {
        t->base->transport_esp_tls->cfg.keep_alive_cfg = (tls_keep_alive_cfg_t *) keep_alive_cfg;
    }
}

esp_transport_handle_t esp_transport_ssl_init(void)
{
    esp_transport_handle_t t = esp_transport_init();
    esp_transport_set_context_data(t, NULL);
    esp_transport_set_func(t, ssl_connect, ssl_read, ssl_write, ssl_close, ssl_poll_read, ssl_poll_write, ssl_destroy);
    esp_transport_set_async_connect_func(t, ssl_connect_async);
    t->_get_socket = ssl_get_socket;
    return t;
}

struct transport_esp_tls* esp_transport_esp_tls_create(void)
{
    transport_esp_tls_t *transport_esp_tls = calloc(1, sizeof(transport_esp_tls_t));
    return transport_esp_tls;
}

void esp_transport_esp_tls_destroy(struct transport_esp_tls* transport_esp_tls)
{
    free(transport_esp_tls);
}

esp_transport_handle_t esp_transport_tcp_init(void)
{
    esp_transport_handle_t t = esp_transport_init();
    esp_transport_set_context_data(t, NULL);
    esp_transport_set_func(t, tcp_connect, ssl_read, ssl_write, ssl_close, ssl_poll_read, ssl_poll_write, ssl_destroy);
    esp_transport_set_async_connect_func(t, tcp_connect_async);
    t->_get_socket = ssl_get_socket;
    return t;
}

void esp_transport_tcp_set_keep_alive(esp_transport_handle_t t, esp_transport_keep_alive_t *keep_alive_cfg)
{
    return esp_transport_ssl_set_keep_alive(t, keep_alive_cfg);
}
