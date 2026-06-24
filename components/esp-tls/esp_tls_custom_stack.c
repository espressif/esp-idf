/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_ESP_TLS_CUSTOM_STACK

#include "esp_tls_custom_stack.h"
#include "esp_tls_private.h"
#include "esp_log.h"
#include <string.h>
#include <inttypes.h>

static const char *TAG = "esp-tls-custom-stack";

static const esp_tls_stack_ops_t *s_esp_tls_custom_stack = NULL;
static void *s_esp_tls_custom_stack_user_ctx = NULL;

#define CHECK_STACK_REGISTERED(ret_val) \
    do { \
        if (s_esp_tls_custom_stack == NULL) { \
            ESP_LOGE(TAG, "No TLS stack registered. Call esp_tls_register_stack() first."); \
            return ret_val; \
        } \
    } while (0)

#define CHECK_STACK_REGISTERED_VOID() \
    do { \
        if (s_esp_tls_custom_stack == NULL) { \
            ESP_LOGE(TAG, "No TLS stack registered. Call esp_tls_register_stack() first."); \
            return; \
        } \
    } while (0)

esp_err_t esp_tls_register_stack(const esp_tls_stack_ops_t *ops, void *user_ctx)
{
    if (!ops) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Validate version */
    if (ops->version != ESP_TLS_STACK_OPS_VERSION) {
        ESP_LOGE(TAG, "Invalid TLS stack version: expected %d, got %" PRIu32,
                 ESP_TLS_STACK_OPS_VERSION, ops->version);
        return ESP_ERR_INVALID_VERSION;
    }

    /* Validate required function pointers */
    if (!ops->create_ssl_handle || !ops->handshake || !ops->read || !ops->write ||
        !ops->conn_delete || !ops->net_init || !ops->get_ssl_context ||
        !ops->get_bytes_avail || !ops->init_global_ca_store ||
        !ops->set_global_ca_store || !ops->get_global_ca_store ||
        !ops->free_global_ca_store || !ops->get_ciphersuites_list) {
        ESP_LOGE(TAG, "Invalid TLS stack operations: required function pointers are NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (s_esp_tls_custom_stack != NULL) {
        ESP_LOGE(TAG, "TLS stack already registered");
        return ESP_ERR_INVALID_STATE;
    }

    s_esp_tls_custom_stack = ops;
    s_esp_tls_custom_stack_user_ctx = user_ctx;
    ESP_LOGI(TAG, "Custom TLS stack registered successfully");
    return ESP_OK;
}

const esp_tls_stack_ops_t *esp_tls_get_registered_stack(void)
{
    return s_esp_tls_custom_stack;
}

esp_err_t esp_tls_unregister_stack(void)
{
    if (s_esp_tls_custom_stack == NULL) {
        ESP_LOGW(TAG, "No TLS stack registered");
        return ESP_ERR_INVALID_STATE;
    }

    s_esp_tls_custom_stack = NULL;
    s_esp_tls_custom_stack_user_ctx = NULL;
    ESP_LOGI(TAG, "Custom TLS stack unregistered successfully");
    return ESP_OK;
}

esp_err_t esp_tls_custom_stack_create_ssl_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls, void *server_params)
{
    CHECK_STACK_REGISTERED(ESP_ERR_INVALID_STATE);
    return s_esp_tls_custom_stack->create_ssl_handle(s_esp_tls_custom_stack_user_ctx, hostname, hostlen, cfg, tls, server_params);
}

int esp_tls_custom_stack_handshake(esp_tls_t *tls, const esp_tls_cfg_t *cfg)
{
    CHECK_STACK_REGISTERED(-1);
    return s_esp_tls_custom_stack->handshake(s_esp_tls_custom_stack_user_ctx, tls, cfg);
}

ssize_t esp_tls_custom_stack_read(esp_tls_t *tls, char *data, size_t datalen)
{
    CHECK_STACK_REGISTERED(-1);
    return s_esp_tls_custom_stack->read(s_esp_tls_custom_stack_user_ctx, tls, data, datalen);
}

ssize_t esp_tls_custom_stack_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    CHECK_STACK_REGISTERED(-1);
    return s_esp_tls_custom_stack->write(s_esp_tls_custom_stack_user_ctx, tls, data, datalen);
}

void esp_tls_custom_stack_conn_delete(esp_tls_t *tls)
{
    CHECK_STACK_REGISTERED_VOID();
    s_esp_tls_custom_stack->conn_delete(s_esp_tls_custom_stack_user_ctx, tls);
}

void esp_tls_custom_stack_net_init(esp_tls_t *tls)
{
    CHECK_STACK_REGISTERED_VOID();
    s_esp_tls_custom_stack->net_init(s_esp_tls_custom_stack_user_ctx, tls);
}

void *esp_tls_custom_stack_get_ssl_context(esp_tls_t *tls)
{
    CHECK_STACK_REGISTERED(NULL);
    return s_esp_tls_custom_stack->get_ssl_context(s_esp_tls_custom_stack_user_ctx, tls);
}

ssize_t esp_tls_custom_stack_get_bytes_avail(esp_tls_t *tls)
{
    CHECK_STACK_REGISTERED(-1);
    return s_esp_tls_custom_stack->get_bytes_avail(s_esp_tls_custom_stack_user_ctx, tls);
}

esp_err_t esp_tls_custom_stack_init_global_ca_store(void)
{
    CHECK_STACK_REGISTERED(ESP_ERR_INVALID_STATE);
    return s_esp_tls_custom_stack->init_global_ca_store(s_esp_tls_custom_stack_user_ctx);
}

esp_err_t esp_tls_custom_stack_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes)
{
    CHECK_STACK_REGISTERED(ESP_ERR_INVALID_STATE);
    return s_esp_tls_custom_stack->set_global_ca_store(s_esp_tls_custom_stack_user_ctx, cacert_pem_buf, cacert_pem_bytes);
}

void *esp_tls_custom_stack_get_global_ca_store(void)
{
    CHECK_STACK_REGISTERED(NULL);
    return s_esp_tls_custom_stack->get_global_ca_store(s_esp_tls_custom_stack_user_ctx);
}

void esp_tls_custom_stack_free_global_ca_store(void)
{
    CHECK_STACK_REGISTERED_VOID();
    s_esp_tls_custom_stack->free_global_ca_store(s_esp_tls_custom_stack_user_ctx);
}

const int *esp_tls_custom_stack_get_ciphersuites_list(void)
{
    CHECK_STACK_REGISTERED(NULL);
    return s_esp_tls_custom_stack->get_ciphersuites_list(s_esp_tls_custom_stack_user_ctx);
}

void *esp_tls_custom_stack_get_client_session(esp_tls_t *tls)
{
    if (s_esp_tls_custom_stack == NULL || !s_esp_tls_custom_stack->get_client_session) {
        ESP_LOGE(TAG, "No TLS stack registered or client session not supported.");
        return NULL;
    }
    return s_esp_tls_custom_stack->get_client_session(s_esp_tls_custom_stack_user_ctx, tls);
}

void esp_tls_custom_stack_free_client_session(void *client_session)
{
    if (s_esp_tls_custom_stack == NULL || !s_esp_tls_custom_stack->free_client_session) {
        ESP_LOGE(TAG, "No TLS stack registered or client session not supported.");
        return;
    }
    s_esp_tls_custom_stack->free_client_session(s_esp_tls_custom_stack_user_ctx, client_session);
}

esp_err_t esp_tls_custom_stack_server_session_ticket_ctx_init(void *cfg)
{
    if (s_esp_tls_custom_stack == NULL || !s_esp_tls_custom_stack->server_session_ticket_ctx_init) {
        ESP_LOGE(TAG, "No TLS stack registered or server session tickets not supported.");
        return ESP_ERR_NOT_SUPPORTED;
    }
    return s_esp_tls_custom_stack->server_session_ticket_ctx_init(s_esp_tls_custom_stack_user_ctx, cfg);
}

void esp_tls_custom_stack_server_session_ticket_ctx_free(void *cfg)
{
    if (s_esp_tls_custom_stack == NULL || !s_esp_tls_custom_stack->server_session_ticket_ctx_free) {
        ESP_LOGE(TAG, "No TLS stack registered or server session tickets not supported.");
        return;
    }
    s_esp_tls_custom_stack->server_session_ticket_ctx_free(s_esp_tls_custom_stack_user_ctx, cfg);
}

int esp_tls_custom_stack_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls)
{
    if (s_esp_tls_custom_stack == NULL || !s_esp_tls_custom_stack->server_session_create) {
        ESP_LOGE(TAG, "No TLS stack registered or server session not supported.");
        return -1;
    }
    return s_esp_tls_custom_stack->server_session_create(s_esp_tls_custom_stack_user_ctx, cfg, sockfd, tls);
}

esp_err_t esp_tls_custom_stack_server_session_init(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls)
{
    if (s_esp_tls_custom_stack == NULL || !s_esp_tls_custom_stack->server_session_init) {
        ESP_LOGE(TAG, "No TLS stack registered or server session not supported.");
        return ESP_ERR_INVALID_STATE;
    }
    return s_esp_tls_custom_stack->server_session_init(s_esp_tls_custom_stack_user_ctx, cfg, sockfd, tls);
}

int esp_tls_custom_stack_server_session_continue_async(esp_tls_t *tls)
{
    if (s_esp_tls_custom_stack == NULL || !s_esp_tls_custom_stack->server_session_continue_async) {
        ESP_LOGE(TAG, "No TLS stack registered or server session not supported.");
        return -1;
    }
    return s_esp_tls_custom_stack->server_session_continue_async(s_esp_tls_custom_stack_user_ctx, tls);
}

void esp_tls_custom_stack_server_session_delete(esp_tls_t *tls)
{
    CHECK_STACK_REGISTERED_VOID();
    if (s_esp_tls_custom_stack->server_session_delete) {
        s_esp_tls_custom_stack->server_session_delete(s_esp_tls_custom_stack_user_ctx, tls);
    } else {
        /* Fall back to conn_delete as documented */
        s_esp_tls_custom_stack->conn_delete(s_esp_tls_custom_stack_user_ctx, tls);
    }
}

int esp_tls_custom_stack_crypto_sha1(const unsigned char *input, size_t ilen, unsigned char output[20])
{
    if (s_esp_tls_custom_stack == NULL || !s_esp_tls_custom_stack->crypto_sha1) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return s_esp_tls_custom_stack->crypto_sha1(s_esp_tls_custom_stack_user_ctx, input, ilen, output);
}

int esp_tls_custom_stack_crypto_base64_encode(unsigned char *dst, size_t dlen, size_t *olen,
                                              const unsigned char *src, size_t slen)
{
    if (s_esp_tls_custom_stack == NULL || !s_esp_tls_custom_stack->crypto_base64_encode) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return s_esp_tls_custom_stack->crypto_base64_encode(s_esp_tls_custom_stack_user_ctx, dst, dlen, olen, src, slen);
}

#else /* CONFIG_ESP_TLS_CUSTOM_STACK */

#include "esp_err.h"
#include "esp_tls_custom_stack.h"

/* Stub implementations when custom stack is disabled */
esp_err_t esp_tls_register_stack(const esp_tls_stack_ops_t *ops, void *user_ctx)
{
    (void)ops;
    (void)user_ctx;
    return ESP_ERR_NOT_SUPPORTED;
}

const esp_tls_stack_ops_t *esp_tls_get_registered_stack(void)
{
    return NULL;
}

esp_err_t esp_tls_unregister_stack(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

#endif /* CONFIG_ESP_TLS_CUSTOM_STACK */
