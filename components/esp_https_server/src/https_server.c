// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "esp_https_server.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_tls.h"

const static char *TAG = "esp_https_server";

/**
 * SSL socket close handler
 *
 * @param[in] ctx - session transport context (SSL context we stored there)
 */
static void httpd_ssl_close(void *ctx)
{
    assert(ctx != NULL);
    esp_tls_server_session_delete(ctx);
    ESP_LOGD(TAG, "Secure socket closed");
}

/**
 * SSL socket pending-check function
 *
 * @param server
 * @param sockfd
 * @return number of pending bytes, negative on error
 */
static int httpd_ssl_pending(httpd_handle_t server, int sockfd)
{
    esp_tls_t *tls = httpd_sess_get_transport_ctx(server, sockfd);
    assert(tls != NULL);
    return esp_tls_get_bytes_avail(tls);
}

/**
 * Receive from a SSL socket
 *
 * @param server
 * @param sockfd
 * @param buf
 * @param buf_len
 * @param flags
 * @return bytes read, negative on error
 */
static int httpd_ssl_recv(httpd_handle_t server, int sockfd, char *buf, size_t buf_len, int flags)
{
    esp_tls_t *tls = httpd_sess_get_transport_ctx(server, sockfd);
    assert(tls != NULL);
    return esp_tls_conn_read(tls, buf, buf_len);
}

/**
 * Send to a SSL socket
 *
 * @param server
 * @param sockfd
 * @param buf
 * @param buf_len
 * @param flags
 * @return bytes sent, negative on error
 */
static int httpd_ssl_send(httpd_handle_t server, int sockfd, const char *buf, size_t buf_len, int flags)
{
    esp_tls_t *tls = httpd_sess_get_transport_ctx(server, sockfd);
    assert(tls != NULL);
    return esp_tls_conn_write(tls, buf, buf_len);
}

/**
 * Open a SSL socket for the server.
 * The fd is already open and ready to read / write raw data.
 *
 * @param server
 * @param sockfd - raw socket fd
 * @return success
 */
static esp_err_t httpd_ssl_open(httpd_handle_t server, int sockfd)
{
    assert(server != NULL);

    // Retrieve the SSL context from the global context field (set in config)
    esp_tls_cfg_server_t *global_ctx = httpd_get_global_transport_ctx(server);
    assert(global_ctx != NULL);

    esp_tls_t *tls = (esp_tls_t *)calloc(1, sizeof(esp_tls_t));
    if (!tls) {
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "performing session handshake");
    int ret = esp_tls_server_session_create(global_ctx, sockfd, tls);
    if (ret != 0) {
        ESP_LOGE(TAG, "esp_tls_create_server_session failed");
        goto fail;
    }

    // Store the SSL session into the context field of the HTTPD session object
    httpd_sess_set_transport_ctx(server, sockfd, tls, httpd_ssl_close);

    // Set rx/tx/pending override functions
    httpd_sess_set_send_override(server, sockfd, httpd_ssl_send);
    httpd_sess_set_recv_override(server, sockfd, httpd_ssl_recv);
    httpd_sess_set_pending_override(server, sockfd, httpd_ssl_pending);

    // all access should now go through SSL

    ESP_LOGD(TAG, "Secure socket open");

    return ESP_OK;
fail:
    esp_tls_server_session_delete(tls);
    return ESP_FAIL;
}

/**
 * Tear down the HTTPD global transport context
 *
 * @param ctx
 */
static void free_secure_context(void *ctx)
{
    assert(ctx != NULL);
    esp_tls_cfg_server_t *cfg = (esp_tls_cfg_server_t *)ctx;
    ESP_LOGI(TAG, "Server shuts down, releasing SSL context");
    if (cfg->servercert_pem_buf) {
        free((void *)cfg->servercert_pem_buf);
    }
    if (cfg->serverkey_pem_buf) {
        free((void *)cfg->serverkey_pem_buf);
    }
    free(cfg);
}

static esp_tls_cfg_server_t *create_secure_context(const struct httpd_ssl_config *config)
{
    esp_tls_cfg_server_t *cfg = (esp_tls_cfg_server_t *)calloc(1, sizeof(esp_tls_cfg_server_t));
    if (!cfg) {
        return NULL;
    }
    cfg->servercert_pem_buf = (unsigned char *)malloc(config->cacert_len);
    if (!cfg->servercert_pem_buf) {
        free(cfg);
        return NULL;
    }
    memcpy((char *)cfg->servercert_pem_buf, config->cacert_pem, config->cacert_len);
    cfg->servercert_pem_bytes = config->cacert_len;

    cfg->serverkey_pem_buf = (unsigned char *)malloc(config->prvtkey_len);
    if (!cfg->serverkey_pem_buf) {
        free((void *)cfg->servercert_pem_buf);
        free(cfg);
        return NULL;
    }
    memcpy((char *)cfg->serverkey_pem_buf, config->prvtkey_pem, config->prvtkey_len);
    cfg->serverkey_pem_bytes = config->prvtkey_len;

    return cfg;
}

/** Start the server */
esp_err_t httpd_ssl_start(httpd_handle_t *pHandle, struct httpd_ssl_config *config)
{
    assert(config != NULL);
    assert(pHandle != NULL);

    ESP_LOGI(TAG, "Starting server");

    if (HTTPD_SSL_TRANSPORT_SECURE == config->transport_mode) {

        esp_tls_cfg_server_t *esp_tls_cfg = create_secure_context(config);
        if (!esp_tls_cfg) {
            return -1;
        }

        ESP_LOGD(TAG, "SSL context ready");

        // set SSL specific config
        config->httpd.global_transport_ctx = esp_tls_cfg;
        config->httpd.global_transport_ctx_free_fn = free_secure_context;
        config->httpd.open_fn = httpd_ssl_open; // the open function configures the created SSL sessions

        config->httpd.server_port = config->port_secure;
    } else {
        ESP_LOGD(TAG, "SSL disabled, using plain HTTP");
        config->httpd.server_port = config->port_insecure;
    }

    httpd_handle_t handle = NULL;

    esp_err_t ret = httpd_start(&handle, &config->httpd);
    if (ret != ESP_OK) return ret;

    *pHandle = handle;

    ESP_LOGI(TAG, "Server listening on port %d", config->httpd.server_port);
    return ESP_OK;
}

/** Stop the server */
void httpd_ssl_stop(httpd_handle_t handle)
{
    httpd_stop(handle);
}
