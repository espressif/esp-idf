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
#include "openssl/ssl.h"
#include "esp_log.h"
#include "sdkconfig.h"

const static char *TAG = "esp_https_server";

/**
 * SSL socket close handler
 *
 * @param[in] ctx - session transport context (SSL context we stored there)
 */
static void httpd_ssl_close(void *ctx)
{
    assert(ctx != NULL);
    SSL_shutdown(ctx);
    SSL_free(ctx);
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
    SSL *ssl = httpd_sess_get_transport_ctx(server, sockfd);
    assert(ssl != NULL);
    return SSL_pending(ssl);
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
    SSL *ssl = httpd_sess_get_transport_ctx(server, sockfd);
    assert(ssl != NULL);
    return SSL_read(ssl, buf, buf_len);
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
    SSL *ssl = httpd_sess_get_transport_ctx(server, sockfd);
    assert(ssl != NULL);
    return SSL_write(ssl, buf, buf_len);
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
    SSL_CTX *global_ctx = httpd_get_global_transport_ctx(server);
    assert(global_ctx != NULL);

    SSL *ssl = SSL_new(global_ctx);
    if (NULL == ssl) {
        ESP_LOGE(TAG, "SSL_new ret NULL (out of memory)");
        return ESP_ERR_NO_MEM;
    }

    if (1 != SSL_set_fd(ssl, sockfd)) {
        ESP_LOGE(TAG, "fail to set SSL fd");
        goto teardown;
    }

    ESP_LOGD(TAG, "SSL accept");
    if (1 != SSL_accept(ssl)) {
        ESP_LOGW(TAG, "fail to SSL_accept - handshake error");
        goto teardown;
    }

    // Store the SSL session into the context field of the HTTPD session object
    httpd_sess_set_transport_ctx(server, sockfd, ssl, httpd_ssl_close);

    // Set rx/tx/pending override functions
    httpd_sess_set_send_override(server, sockfd, httpd_ssl_send);
    httpd_sess_set_recv_override(server, sockfd, httpd_ssl_recv);
    httpd_sess_set_pending_override(server, sockfd, httpd_ssl_pending);

    // all access should now go through SSL

    ESP_LOGD(TAG, "Secure socket open");

    return ESP_OK;

teardown:
    SSL_free(ssl);
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

    ESP_LOGI(TAG, "Server shuts down, releasing SSL context");
    SSL_CTX_free(ctx);
}

/**
* Create and perform basic init of a SSL_CTX, or return NULL on failure
*
* @return ctx or null
*/
static SSL_CTX *create_secure_context(const struct httpd_ssl_config *config)
{
    SSL_CTX *ctx = NULL;

    ESP_LOGD(TAG, "SSL server context create");
    ctx = SSL_CTX_new(TLS_server_method());
    if (NULL != ctx) {
        //region SSL ctx alloc'd
        ESP_LOGD(TAG, "SSL ctx set own cert");
        if (SSL_CTX_use_certificate_ASN1(ctx, config->cacert_len, config->cacert_pem)
            && SSL_CTX_use_PrivateKey_ASN1(0, ctx, config->prvtkey_pem, (long) config->prvtkey_len)) {
            return ctx;
        }
        else {
            ESP_LOGE(TAG, "Failed to set certificate");
            SSL_CTX_free(ctx);
            ctx = NULL;
        }
    } else {
        ESP_LOGE(TAG, "Failed to create SSL context");
    }
    return NULL;
}

/** Start the server */
esp_err_t httpd_ssl_start(httpd_handle_t *pHandle, struct httpd_ssl_config *config)
{
    assert(config != NULL);
    assert(pHandle != NULL);

    ESP_LOGI(TAG, "Starting server");

    if (HTTPD_SSL_TRANSPORT_SECURE == config->transport_mode) {
        SSL_CTX *ctx = create_secure_context(config);
        if (!ctx) {
            return ESP_FAIL;
        }

        ESP_LOGD(TAG, "SSL context ready");

        // set SSL specific config
        config->httpd.global_transport_ctx = ctx;
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
