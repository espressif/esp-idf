/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
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

typedef struct httpd_ssl_transport_ctx {
    esp_tls_t *tls;
    httpd_ssl_ctx_t *global_ctx;
} httpd_ssl_transport_ctx_t;

ESP_EVENT_DEFINE_BASE(ESP_HTTPS_SERVER_EVENT);

#if CONFIG_ESP_HTTPS_SERVER_EVENT_POST_TIMEOUT == -1
#define ESP_HTTPS_SERVER_EVENT_POST_TIMEOUT portMAX_DELAY
#else
#define ESP_HTTPS_SERVER_EVENT_POST_TIMEOUT pdMS_TO_TICKS(CONFIG_ESP_HTTPS_SERVER_EVENT_POST_TIMEOUT)
#endif


static void http_dispatch_event_to_event_loop(int32_t event_id, const void* event_data, size_t event_data_size)
{
    esp_err_t err = esp_event_post(ESP_HTTPS_SERVER_EVENT, event_id, event_data, event_data_size, ESP_HTTPS_SERVER_EVENT_POST_TIMEOUT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to post http_client event: %"PRId32", error: %s", event_id, esp_err_to_name(err));
    }
}

/**
 * SSL socket close handler
 *
 * @param[in] ctx - session transport context (SSL context we stored there)
 */
static void httpd_ssl_close(void *ctx)
{
    assert(ctx != NULL);

    httpd_ssl_transport_ctx_t *transport_ctx = (httpd_ssl_transport_ctx_t *)ctx;
    httpd_ssl_ctx_t *global_ctx = transport_ctx->global_ctx;
    esp_tls_t *tls = transport_ctx->tls;

    if (global_ctx->user_cb) {
        esp_https_server_user_cb_arg_t user_cb_data = {0};
        user_cb_data.user_cb_state = HTTPD_SSL_USER_CB_SESS_CLOSE;
        user_cb_data.tls = tls;
        (global_ctx->user_cb)((void *)&user_cb_data);
    }

    esp_tls_server_session_delete(tls);
    free(ctx);
    ESP_LOGD(TAG, "Secure socket closed");
    http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_DISCONNECTED, NULL, 0);
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
    httpd_ssl_transport_ctx_t *transport_ctx = httpd_sess_get_transport_ctx(server, sockfd);
    assert(transport_ctx != NULL);
    esp_tls_t *tls = transport_ctx->tls;
    assert(tls != NULL);
    int ret = esp_tls_get_bytes_avail(tls);
    if (ret < 0) {
        esp_tls_error_handle_t error_handle;
        if (esp_tls_get_error_handle(tls, &error_handle) == ESP_OK) {
            esp_https_server_last_error_t last_error = {0};
            last_error.last_error = esp_tls_get_and_clear_last_error(error_handle, &last_error.esp_tls_error_code, &last_error.esp_tls_flags);
            http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_ERROR, &last_error, sizeof(last_error));
        }
    }
    return ret;
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
    httpd_ssl_transport_ctx_t *transport_ctx = httpd_sess_get_transport_ctx(server, sockfd);
    assert(transport_ctx != NULL);
    esp_tls_t *tls = transport_ctx->tls;
    assert(tls != NULL);
    int ret = esp_tls_conn_read(tls, buf, buf_len);
    if (ret < 0) {
        esp_tls_error_handle_t error_handle;
        if (esp_tls_get_error_handle(tls, &error_handle) == ESP_OK) {
            esp_https_server_last_error_t last_error = {0};
            last_error.last_error = esp_tls_get_and_clear_last_error(error_handle, &last_error.esp_tls_error_code, &last_error.esp_tls_flags);
            http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_ERROR, &last_error, sizeof(last_error));
        }
    } else {
        http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_ON_DATA, &ret, sizeof(int));
    }
    return ret;
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
    httpd_ssl_transport_ctx_t *transport_ctx = httpd_sess_get_transport_ctx(server, sockfd);
    assert(transport_ctx != NULL);
    esp_tls_t *tls = transport_ctx->tls;
    assert(tls != NULL);
    int ret = esp_tls_conn_write(tls, buf, buf_len);
    if (ret < 0) {
        esp_tls_error_handle_t error_handle;
        if (esp_tls_get_error_handle(tls, &error_handle) == ESP_OK) {
            esp_https_server_last_error_t last_error = {0};
            last_error.last_error = esp_tls_get_and_clear_last_error(error_handle, &last_error.esp_tls_error_code, &last_error.esp_tls_flags);
            http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_ERROR, &last_error, sizeof(last_error));
        }
    } else {
        http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_SENT_DATA, NULL, 0);
    }
    return ret;
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

    esp_tls_t *tls = esp_tls_init();
    if (!tls) {
        esp_https_server_last_error_t last_error = {0};
        last_error.last_error = ESP_ERR_NO_MEM;
        http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_ERROR, &last_error, sizeof(last_error));
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "performing session handshake");
    int ret = esp_tls_server_session_create(global_ctx->tls_cfg, sockfd, tls);
    if (ret != 0) {
        ESP_LOGE(TAG, "esp_tls_create_server_session failed, 0x%04x", -ret);
        goto fail;
    }

    // Pass a new structure containing the global context and the tls pointer to httpd_ssl_close
    // Store it in the context field of the HTTPD session object
    // NOTE: allocated memory will be freed by httpd_ssl_close
    httpd_ssl_transport_ctx_t *transport_ctx = (httpd_ssl_transport_ctx_t *)calloc(1, sizeof(httpd_ssl_transport_ctx_t));
    if (!transport_ctx) {
        esp_https_server_last_error_t last_error = {0};
        last_error.last_error = ESP_ERR_NO_MEM;
        http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_ERROR, &last_error, sizeof(last_error));
        return ESP_ERR_NO_MEM;
    }
    transport_ctx->tls = tls;
    transport_ctx->global_ctx = global_ctx;

    // Store the SSL session into the context field of the HTTPD session object
    httpd_sess_set_transport_ctx(server, sockfd, transport_ctx, httpd_ssl_close);

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
        user_cb_data.user_cb_state = HTTPD_SSL_USER_CB_SESS_CREATE;
        user_cb_data.tls = tls;
        (global_ctx->user_cb)((void *)&user_cb_data);
    }
    http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_ON_CONNECTED, NULL, 0);
    return ESP_OK;
fail:
    {
        esp_tls_error_handle_t error_handle;
        if (esp_tls_get_error_handle(tls, &error_handle) == ESP_OK) {
            esp_https_server_last_error_t last_error = {0};
            last_error.last_error = esp_tls_get_and_clear_last_error(error_handle, &last_error.esp_tls_error_code, &last_error.esp_tls_flags);
            http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_ERROR, &last_error, sizeof(last_error));
        }
        esp_tls_server_session_delete(tls);
    }
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

static esp_err_t create_secure_context(const struct httpd_ssl_config *config, httpd_ssl_ctx_t **ssl_ctx)
{
    if (!ssl_ctx || !*ssl_ctx) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    esp_tls_cfg_server_t *cfg = (esp_tls_cfg_server_t *)calloc(1, sizeof(esp_tls_cfg_server_t));
    if (!cfg) {
        ret = ESP_ERR_NO_MEM;
        goto exit;
    }

    if (config->session_tickets) {
        ret = esp_tls_cfg_server_session_tickets_init(cfg);
        if ( ret != ESP_OK ) {
            ESP_LOGE(TAG, "Failed to init session ticket support. error: %s", esp_err_to_name(ret));
            goto exit;
        }
    }

    cfg->userdata = config->ssl_userdata;
    cfg->alpn_protos = config->alpn_protos;

#if defined(CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK)
    cfg->cert_select_cb = config->cert_select_cb;
#endif

    (*ssl_ctx)->tls_cfg = cfg;
    (*ssl_ctx)->user_cb = config->user_cb;

/* cacert = CA which signs client cert, or client cert itself */
    if (config->cacert_pem != NULL && config->cacert_len > 0) {
        cfg->cacert_buf = (unsigned char *)malloc(config->cacert_len);

        if (cfg->cacert_buf) {
            memcpy((char *) cfg->cacert_buf, config->cacert_pem, config->cacert_len);
            cfg->cacert_bytes = config->cacert_len;
        } else {
            ESP_LOGE(TAG, "Could not allocate memory for client certificate authority");
            ret = ESP_ERR_NO_MEM;
            goto exit;
        }
    }

/* servercert = cert of server itself */
    if (config->servercert != NULL && config->servercert_len > 0) {
        cfg->servercert_buf = (unsigned char *)malloc(config->servercert_len);

        if (cfg->servercert_buf) {
            memcpy((char *) cfg->servercert_buf, config->servercert, config->servercert_len);
            cfg->servercert_bytes = config->servercert_len;
        } else {
            ESP_LOGE(TAG, "Could not allocate memory for server certificate");
            ret = ESP_ERR_NO_MEM;
            goto exit;
        }
    } else {
#if defined(CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK)
        if (config->cert_select_cb == NULL) {
#endif
        ESP_LOGE(TAG, "No Server certificate supplied");
        ret = ESP_ERR_INVALID_ARG;
        goto exit;
#if defined(CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK)
        } else {
            ESP_LOGW(TAG, "Server certificate not supplied, make sure to supply it in the certificate selection hook!");
        }
#endif
    }

    /* Pass on secure element boolean */
    cfg->use_secure_element = config->use_secure_element;
    if (!cfg->use_secure_element) {
        if (config->use_ecdsa_peripheral) {
#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
            (*ssl_ctx)->tls_cfg->use_ecdsa_peripheral = config->use_ecdsa_peripheral;
            (*ssl_ctx)->tls_cfg->ecdsa_key_efuse_blk = config->ecdsa_key_efuse_blk;
#else
            ESP_LOGE(TAG, "Please enable the support for signing using ECDSA peripheral in menuconfig.");
            ret = ESP_ERR_NOT_SUPPORTED;
            goto exit;
#endif
        } else if (config->prvtkey_pem != NULL && config->prvtkey_len > 0) {
            cfg->serverkey_buf = malloc(config->prvtkey_len);

            if (cfg->serverkey_buf) {
                memcpy((char *) cfg->serverkey_buf, config->prvtkey_pem, config->prvtkey_len);
                cfg->serverkey_bytes = config->prvtkey_len;
            } else {
                ESP_LOGE(TAG, "Could not allocate memory for server key");
                ret = ESP_ERR_NO_MEM;
                goto exit;
            }
        } else {
#if defined(CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK)
            if (config->cert_select_cb == NULL) {
                ESP_LOGE(TAG, "No Server key supplied and no certificate selection hook is present");
                ret = ESP_ERR_INVALID_ARG;
                goto exit;
            } else {
                ESP_LOGW(TAG, "Server key not supplied, make sure to supply it in the certificate selection hook");
            }
#else
            ESP_LOGE(TAG, "No Server key supplied");
            ret = ESP_ERR_INVALID_ARG;
            goto exit;
#endif
        }
    }

    return ret;

exit:
    if (cfg) {
        free((void *) cfg->servercert_buf);
        free((void *) cfg->cacert_buf);
    }
    free(cfg);
    return ret;
}

/** Start the server */
esp_err_t httpd_ssl_start(httpd_handle_t *pHandle, struct httpd_ssl_config *config)
{
    assert(config != NULL);
    assert(pHandle != NULL);

    ESP_LOGI(TAG, "Starting server");

    esp_err_t ret = ESP_OK;
    httpd_ssl_ctx_t *ssl_ctx = NULL;

    if (HTTPD_SSL_TRANSPORT_SECURE == config->transport_mode) {
        ssl_ctx = calloc(1, sizeof(httpd_ssl_ctx_t));
        if (!ssl_ctx) {
            return ESP_ERR_NO_MEM;
        }

        ret = create_secure_context(config, &ssl_ctx);
        if (ret != ESP_OK) {
            free(ssl_ctx);
            return ret;
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

    ret = httpd_start(&handle, &config->httpd);
    if (ret != ESP_OK) {
        free(ssl_ctx);
        ssl_ctx = NULL;
        return ret;
    }

    *pHandle = handle;

    ESP_LOGI(TAG, "Server listening on port %d", config->httpd.server_port);
    http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_START, NULL, 0);
    return ESP_OK;
}

/** Stop the server */
esp_err_t httpd_ssl_stop(httpd_handle_t handle)
{
    esp_err_t ret = httpd_stop(handle);
    if (ret == ESP_OK) {
        http_dispatch_event_to_event_loop(HTTPS_SERVER_EVENT_STOP, NULL, 0);
    }
    return ret;
}
