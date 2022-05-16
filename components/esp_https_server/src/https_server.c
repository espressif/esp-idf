/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_https_server.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_tls.h"

const static char *TAG = "esp_https_server";

typedef struct httpd_ssl_ctx {
    esp_tls_cfg_server_t *tls_cfg;
    httpd_open_func_t open_fn;
    esp_https_server_user_cb *user_cb;
} httpd_ssl_ctx_t;

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
    httpd_ssl_ctx_t *global_ctx = httpd_get_global_transport_ctx(server);
    assert(global_ctx != NULL);

    esp_tls_t *tls = (esp_tls_t *)calloc(1, sizeof(esp_tls_t));
    if (!tls) {
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "performing session handshake");
    int ret = esp_tls_server_session_create(global_ctx->tls_cfg, sockfd, tls);
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

    if (global_ctx->open_fn) {
        (global_ctx->open_fn)(server, sockfd);
    }

    if (global_ctx->user_cb) {
        esp_https_server_user_cb_arg_t user_cb_data = {0};
        user_cb_data.tls = tls;
        (global_ctx->user_cb)((void *)&user_cb_data);
    }

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
    httpd_ssl_ctx_t *ssl_ctx = ctx;
    esp_tls_cfg_server_t *cfg = ssl_ctx->tls_cfg;
    ESP_LOGI(TAG, "Server shuts down, releasing SSL context");
    if (cfg->cacert_buf) {
        free((void *)cfg->cacert_buf);
    }
    if (cfg->servercert_buf) {
        free((void *)cfg->servercert_buf);
    }
    if (cfg->serverkey_buf) {
        free((void *)cfg->serverkey_buf);
    }
    esp_tls_cfg_server_session_tickets_free(cfg);
    free(cfg);
    free(ssl_ctx);
}

static httpd_ssl_ctx_t *create_secure_context(const struct httpd_ssl_config *config)
{
    httpd_ssl_ctx_t *ssl_ctx = calloc(1, sizeof(httpd_ssl_ctx_t));
    if (!ssl_ctx) {
        return NULL;
    }
    esp_tls_cfg_server_t *cfg = (esp_tls_cfg_server_t *)calloc(1, sizeof(esp_tls_cfg_server_t));
    if (!cfg) {
        free(ssl_ctx);
        return NULL;
    }

    if (config->session_tickets) {
        if ( esp_tls_cfg_server_session_tickets_init(cfg) != ESP_OK ) {
            ESP_LOGE(TAG, "Failed to init session ticket support");
            free(ssl_ctx);
            free(cfg);
            return NULL;
        }
    }

    ssl_ctx->tls_cfg = cfg;
    ssl_ctx->user_cb = config->user_cb;
/* cacert = CA which signs client cert, or client cert itself , which is mapped to client_verify_cert_pem */
    if(config->client_verify_cert_pem != NULL) {
        cfg->cacert_buf = (unsigned char *)malloc(config->client_verify_cert_len);
        if (!cfg->cacert_buf) {
            ESP_LOGE(TAG, "Could not allocate memory");
            free(cfg);
            free(ssl_ctx);
            return NULL;
        }
        memcpy((char *)cfg->cacert_buf, config->client_verify_cert_pem, config->client_verify_cert_len);
        cfg->cacert_bytes = config->client_verify_cert_len;
    }
/* servercert = cert of server itself ( in our case it is mapped to cacert in https_server example) */
    cfg->servercert_buf = (unsigned char *)malloc(config->cacert_len);
    if (!cfg->servercert_buf) {
        ESP_LOGE(TAG, "Could not allocate memory");
        free((void *)cfg->cacert_buf);
        free(cfg);
        free(ssl_ctx);
        return NULL;
    }
    memcpy((char *)cfg->servercert_buf, config->cacert_pem, config->cacert_len);
    cfg->servercert_bytes = config->cacert_len;

    cfg->serverkey_buf = (unsigned char *)malloc(config->prvtkey_len);
    if (!cfg->serverkey_buf) {
        ESP_LOGE(TAG, "Could not allocate memory");
        free((void *)cfg->servercert_buf);
        free((void *)cfg->cacert_buf);
        free(cfg);
        free(ssl_ctx);
        return NULL;
    }
    memcpy((char *)cfg->serverkey_buf, config->prvtkey_pem, config->prvtkey_len);
    cfg->serverkey_bytes = config->prvtkey_len;

    return ssl_ctx;
}

/** Start the server */
esp_err_t httpd_ssl_start(httpd_handle_t *pHandle, struct httpd_ssl_config *config)
{
    assert(config != NULL);
    assert(pHandle != NULL);

    ESP_LOGI(TAG, "Starting server");

    if (HTTPD_SSL_TRANSPORT_SECURE == config->transport_mode) {

        httpd_ssl_ctx_t *ssl_ctx = create_secure_context(config);
        if (!ssl_ctx) {
            return -1;
        }

        ESP_LOGD(TAG, "SSL context ready");

        // set SSL specific config
        config->httpd.global_transport_ctx = ssl_ctx;
        config->httpd.global_transport_ctx_free_fn = free_secure_context;
        if (config->httpd.open_fn) {
            // since the httpd's open_fn is used for opening the SSL session, we save the configured
            // user pointer and call it upon opening the ssl socket
            ssl_ctx->open_fn = config->httpd.open_fn;
        }
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
