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

#include "transport.h"
#include "transport_ssl.h"
#include "http_utils.h"

static const char *TAG = "TRANS_SSL";
/**
 *  mbedtls specific transport data
 */
typedef struct {
    esp_tls_t                *tls;
    void                     *cert_pem_data;
    int                      cert_pem_len;
    bool                     ssl_initialized;
    bool                     verify_server;
} transport_ssl_t;

static int ssl_close(transport_handle_t t);

static int ssl_connect(transport_handle_t t, const char *host, int port, int timeout_ms)
{
    transport_ssl_t *ssl = transport_get_context_data(t);
    esp_tls_cfg_t cfg = { 0 };
    if (ssl->cert_pem_data) {
        ssl->verify_server = true;
        cfg.cacert_pem_buf = ssl->cert_pem_data;
        cfg.cacert_pem_bytes = ssl->cert_pem_len + 1;
    }
    cfg.timeout_ms = timeout_ms;
    ssl->ssl_initialized = true;
    ssl->tls = esp_tls_conn_new(host, strlen(host), port, &cfg);
    if (!ssl->tls) {
        ESP_LOGE(TAG, "Failed to open a new connection");
        return -1;
    }
    return 0;
}

static int ssl_poll_read(transport_handle_t t, int timeout_ms)
{
    transport_ssl_t *ssl = transport_get_context_data(t);
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(ssl->tls->sockfd, &readset);
    struct timeval timeout;
    http_utils_ms_to_timeval(timeout_ms, &timeout);

    return select(ssl->tls->sockfd + 1, &readset, NULL, NULL, &timeout);
}

static int ssl_poll_write(transport_handle_t t, int timeout_ms)
{
    transport_ssl_t *ssl = transport_get_context_data(t);
    fd_set writeset;
    FD_ZERO(&writeset);
    FD_SET(ssl->tls->sockfd, &writeset);
    struct timeval timeout;
    http_utils_ms_to_timeval(timeout_ms, &timeout);
    return select(ssl->tls->sockfd + 1, NULL, &writeset, NULL, &timeout);
}

static int ssl_write(transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    int poll, ret;
    transport_ssl_t *ssl = transport_get_context_data(t);

    if ((poll = transport_poll_write(t, timeout_ms)) <= 0) {
        ESP_LOGW(TAG, "Poll timeout or error, errno=%s, fd=%d, timeout_ms=%d", strerror(errno), ssl->tls->sockfd, timeout_ms);
        return poll;
    }
    ret = esp_tls_conn_write(ssl->tls, (const unsigned char *) buffer, len);
    if (ret <= 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_write error, errno=%s", strerror(errno));
    }
    return ret;
}

static int ssl_read(transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    int poll, ret;
    transport_ssl_t *ssl = transport_get_context_data(t);

    if (esp_tls_get_bytes_avail(ssl->tls) <= 0) {
        if ((poll = transport_poll_read(t, timeout_ms)) <= 0) {
            return poll;
        }
    }
    ret = esp_tls_conn_read(ssl->tls, (unsigned char *)buffer, len);
    if (ret <= 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_read error, errno=%s", strerror(errno));
    }
    return ret;
}

static int ssl_close(transport_handle_t t)
{
    int ret = -1;
    transport_ssl_t *ssl = transport_get_context_data(t);
    if (ssl->ssl_initialized) {
        esp_tls_conn_delete(ssl->tls);
        ssl->ssl_initialized = false;
        ssl->verify_server = false;
    }
    return ret;
}

static int ssl_destroy(transport_handle_t t)
{
    transport_ssl_t *ssl = transport_get_context_data(t);
    transport_close(t);
    free(ssl);
    return 0;
}

void transport_ssl_set_cert_data(transport_handle_t t, const char *data, int len)
{
    transport_ssl_t *ssl = transport_get_context_data(t);
    if (t && ssl) {
        ssl->cert_pem_data = (void *)data;
        ssl->cert_pem_len = len;
    }
}

transport_handle_t transport_ssl_init()
{
    transport_handle_t t = transport_init();
    transport_ssl_t *ssl = calloc(1, sizeof(transport_ssl_t));
    HTTP_MEM_CHECK(TAG, ssl, return NULL);
    transport_set_context_data(t, ssl);
    transport_set_func(t, ssl_connect, ssl_read, ssl_write, ssl_close, ssl_poll_read, ssl_poll_write, ssl_destroy);
    return t;
}

