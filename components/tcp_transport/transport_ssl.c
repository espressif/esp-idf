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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_tls.h"
#include "esp_log.h"
#include "esp_system.h"

#include "esp_transport.h"
#include "esp_transport_ssl.h"
#include "esp_transport_utils.h"
#include "esp_transport_ssl_internal.h"

static const char *TAG = "TRANS_SSL";

typedef enum {
    TRANS_SSL_INIT = 0,
    TRANS_SSL_CONNECTING,
} transport_ssl_conn_state_t;

/**
 *  mbedtls specific transport data
 */
typedef struct {
    esp_tls_t                *tls;
    esp_tls_cfg_t            cfg;
    bool                     ssl_initialized;
    transport_ssl_conn_state_t conn_state;
} transport_ssl_t;

static int ssl_close(esp_transport_handle_t t);

static int ssl_connect_async(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (ssl->conn_state == TRANS_SSL_INIT) {
        ssl->cfg.timeout_ms = timeout_ms;
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

static int ssl_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);

    ssl->cfg.timeout_ms = timeout_ms;
    ssl->ssl_initialized = true;
    ssl->tls = esp_tls_init();
    if (esp_tls_conn_new_sync(host, strlen(host), port, &ssl->cfg, ssl->tls) < 0) {
        ESP_LOGE(TAG, "Failed to open a new connection");
        esp_transport_set_errors(t, ssl->tls->error_handle);
        esp_tls_conn_delete(ssl->tls);
        ssl->tls = NULL;
        return -1;
    }

    return 0;
}

static int ssl_poll_read(esp_transport_handle_t t, int timeout_ms)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    int ret = -1;
    fd_set readset;
    fd_set errset;
    FD_ZERO(&readset);
    FD_ZERO(&errset);
    FD_SET(ssl->tls->sockfd, &readset);
    FD_SET(ssl->tls->sockfd, &errset);
    struct timeval timeout;
    esp_transport_utils_ms_to_timeval(timeout_ms, &timeout);

    ret = select(ssl->tls->sockfd + 1, &readset, NULL, &errset, &timeout);
    if (ret > 0 && FD_ISSET(ssl->tls->sockfd, &errset)) {
        int sock_errno = 0;
        uint32_t optlen = sizeof(sock_errno);
        getsockopt(ssl->tls->sockfd, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
        ESP_LOGE(TAG, "ssl_poll_read select error %d, errno = %s, fd = %d", sock_errno, strerror(sock_errno), ssl->tls->sockfd);
        ret = -1;
    }
    return ret;
}

static int ssl_poll_write(esp_transport_handle_t t, int timeout_ms)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    int ret = -1;
    fd_set writeset;
    fd_set errset;
    FD_ZERO(&writeset);
    FD_ZERO(&errset);
    FD_SET(ssl->tls->sockfd, &writeset);
    FD_SET(ssl->tls->sockfd, &errset);
    struct timeval timeout;
    esp_transport_utils_ms_to_timeval(timeout_ms, &timeout);
    ret = select(ssl->tls->sockfd + 1, NULL, &writeset, &errset, &timeout);
    if (ret > 0 && FD_ISSET(ssl->tls->sockfd, &errset)) {
        int sock_errno = 0;
        uint32_t optlen = sizeof(sock_errno);
        getsockopt(ssl->tls->sockfd, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
        ESP_LOGE(TAG, "ssl_poll_write select error %d, errno = %s, fd = %d", sock_errno, strerror(sock_errno), ssl->tls->sockfd);
        ret = -1;
    }
    return ret;
}

static int ssl_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    int poll, ret;
    transport_ssl_t *ssl = esp_transport_get_context_data(t);

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
    transport_ssl_t *ssl = esp_transport_get_context_data(t);

    if (esp_tls_get_bytes_avail(ssl->tls) <= 0) {
        if ((poll = esp_transport_poll_read(t, timeout_ms)) <= 0) {
            return poll;
        }
    }
    ret = esp_tls_conn_read(ssl->tls, (unsigned char *)buffer, len);
    if (ret < 0) {
        ESP_LOGE(TAG, "esp_tls_conn_read error, errno=%s", strerror(errno));
        esp_transport_set_errors(t, ssl->tls->error_handle);
    }
    if (ret == 0) {
        ret = -1;
    }
    return ret;
}

static int ssl_close(esp_transport_handle_t t)
{
    int ret = -1;
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (ssl->ssl_initialized) {
        esp_tls_conn_delete(ssl->tls);
        ssl->ssl_initialized = false;
    }
    return ret;
}

static int ssl_destroy(esp_transport_handle_t t)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    esp_transport_close(t);
    free(ssl);
    return 0;
}

void esp_transport_ssl_enable_global_ca_store(esp_transport_handle_t t)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (t && ssl) {
        ssl->cfg.use_global_ca_store = true;
    }
}

void esp_transport_ssl_set_psk_key_hint(esp_transport_handle_t t, const psk_hint_key_t* psk_hint_key)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (t && ssl) {
        ssl->cfg.psk_hint_key =  psk_hint_key;
    }
}

void esp_transport_ssl_set_cert_data(esp_transport_handle_t t, const char *data, int len)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (t && ssl) {
        ssl->cfg.cacert_pem_buf = (void *)data;
        ssl->cfg.cacert_pem_bytes = len + 1;
    }
}

void esp_transport_ssl_set_cert_data_der(esp_transport_handle_t t, const char *data, int len)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (t && ssl) {
        ssl->cfg.cacert_buf = (void *)data;
        ssl->cfg.cacert_bytes = len;
    }
}

void esp_transport_ssl_set_client_cert_data(esp_transport_handle_t t, const char *data, int len)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (t && ssl) {
        ssl->cfg.clientcert_pem_buf = (void *)data;
        ssl->cfg.clientcert_pem_bytes = len + 1;
    }
}

void esp_transport_ssl_set_client_cert_data_der(esp_transport_handle_t t, const char *data, int len)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (t && ssl) {
        ssl->cfg.clientcert_buf = (void *)data;
        ssl->cfg.clientcert_bytes = len;
    }
}

void esp_transport_ssl_set_client_key_data(esp_transport_handle_t t, const char *data, int len)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (t && ssl) {
        ssl->cfg.clientkey_pem_buf = (void *)data;
        ssl->cfg.clientkey_pem_bytes = len + 1;
    }
}

void esp_transport_ssl_set_client_key_data_der(esp_transport_handle_t t, const char *data, int len)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (t && ssl) {
        ssl->cfg.clientkey_buf = (void *)data;
        ssl->cfg.clientkey_bytes = len;
    }
}

void esp_transport_ssl_skip_common_name_check(esp_transport_handle_t t)
{
    transport_ssl_t *ssl = esp_transport_get_context_data(t);
    if (t && ssl) {
        ssl->cfg.skip_common_name = true;
    }
}

esp_transport_handle_t esp_transport_ssl_init(void)
{
    esp_transport_handle_t t = esp_transport_init();
    transport_ssl_t *ssl = calloc(1, sizeof(transport_ssl_t));
    ESP_TRANSPORT_MEM_CHECK(TAG, ssl, return NULL);
    esp_transport_set_context_data(t, ssl);
    esp_transport_set_func(t, ssl_connect, ssl_read, ssl_write, ssl_close, ssl_poll_read, ssl_poll_write, ssl_destroy);
    esp_transport_set_async_connect_func(t, ssl_connect_async);
    return t;
}

