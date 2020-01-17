// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <http_parser.h>
#include "esp_tls_wolfssl.h"
#include "esp_tls_error_capture_internal.h"
#include <errno.h>
#include "esp_log.h"

static unsigned char *global_cacert = NULL;
static unsigned int global_cacert_pem_bytes = 0;
static const char *TAG = "esp-tls-wolfssl";

esp_err_t esp_create_wolfssl_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls)
{
#ifdef CONFIG_ESP_DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif

    assert(cfg != NULL);
    assert(tls != NULL);

    esp_err_t esp_ret = ESP_FAIL;
    int ret;

    ret = wolfSSL_Init();
    if (ret != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "Init wolfSSL failed: %d", ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
        goto exit;
    }

    if (tls->role == ESP_TLS_CLIENT) {
        esp_ret = set_client_config(hostname, hostlen, (esp_tls_cfg_t *)cfg, tls);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set client configurations");
            goto exit;
        }
    } else if (tls->role == ESP_TLS_SERVER) {
#ifdef CONFIG_ESP_TLS_SERVER
        esp_ret = set_server_config((esp_tls_cfg_server_t *) cfg, tls);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set server configurations");
            goto exit;
        }
#else
        ESP_LOGE(TAG, "ESP_TLS_SERVER Not enabled in Kconfig");
        goto exit;
#endif
    }

    return ESP_OK;
exit:
    esp_wolfssl_cleanup(tls);
    return esp_ret;
}

esp_err_t set_client_config(const char *hostname, size_t hostlen, esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (!tls->priv_ctx) {
        ESP_LOGE(TAG, "Set wolfSSL ctx failed");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
        goto exit;
    }

#ifdef HAVE_ALPN
    if (cfg->alpn_protos) {
        char **alpn_list = (char **)cfg->alpn_protos;
        for (; *alpn_list != NULL; alpn_list ++) {
            if (wolfSSL_UseALPN( (WOLFSSL *)tls->priv_ssl, *alpn_list, strlen(*alpn_list), WOLFSSL_ALPN_FAILED_ON_MISMATCH) != WOLFSSL_SUCCESS) {
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
                ESP_LOGE(TAG, "Use wolfSSL ALPN failed");
                goto exit;
            }
        }
    }
#endif

    if ( cfg->use_global_ca_store == true) {
        wolfSSL_CTX_load_verify_buffer( (WOLFSSL_CTX *)tls->priv_ctx, global_cacert, global_cacert_pem_bytes, WOLFSSL_FILETYPE_PEM);
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER, NULL);
    } else if (cfg->cacert_buf != NULL) {
        wolfSSL_CTX_load_verify_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cfg->cacert_buf, cfg->cacert_bytes, WOLFSSL_FILETYPE_PEM);
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER, NULL);
    } else if (cfg->psk_hint_key) {
        ESP_LOGE(TAG,"psk_hint_key not supported in wolfssl");
        goto exit;
    } else {
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_NONE, NULL);
    }

    if (cfg->clientcert_buf != NULL && cfg->clientkey_buf != NULL) {
        wolfSSL_CTX_use_certificate_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cfg->clientcert_buf, cfg->clientcert_pem_bytes, WOLFSSL_FILETYPE_PEM);
        wolfSSL_CTX_use_PrivateKey_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cfg->clientkey_buf, cfg->clientkey_bytes, WOLFSSL_FILETYPE_PEM);
    } else if (cfg->clientcert_buf != NULL || cfg->clientkey_buf != NULL) {
        ESP_LOGE(TAG, "You have to provide both clientcert_buf and clientkey_buf for mutual authentication\n\n");
        goto exit;
    }

    tls->priv_ssl =(void *)wolfSSL_new( (WOLFSSL_CTX *)tls->priv_ctx);
    if (!tls->priv_ssl) {
        ESP_LOGE(TAG, "Create wolfSSL failed");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
        goto exit;
    }

#ifdef HAVE_SNI
    /* Hostname set here should match CN in server certificate */
    char *use_host = strndup(hostname, hostlen);
    if (!use_host) {
        goto exit;
    }
    wolfSSL_set_tlsext_host_name( (WOLFSSL *)tls->priv_ssl, use_host);
    free(use_host);
#endif
    wolfSSL_set_fd((WOLFSSL *)tls->priv_ssl, tls->sockfd);
    return ESP_OK;
exit:
    return ESP_FAIL;
}

#ifdef CONFIG_ESP_TLS_SERVER
esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls)
{
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfTLSv1_2_server_method());
    if (!tls->priv_ctx) {
        ESP_LOGE(TAG, "Set wolfSSL ctx failed");
        goto exit;
    }

#ifdef HAVE_ALPN
    if (cfg->alpn_protos) {
        char **alpn_list = (char **)cfg->alpn_protos;
        for (; *alpn_list != NULL; alpn_list ++) {
            if (wolfSSL_UseALPN( (WOLFSSL *)tls->priv_ssl, *alpn_list, strlen(*alpn_list), WOLFSSL_ALPN_FAILED_ON_MISMATCH) != WOLFSSL_SUCCESS) {
                ESP_LOGE(TAG, "Use wolfSSL ALPN failed");
                goto exit;
            }
        }
    }
#endif
    if (cfg->cacert_buf != NULL) {
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER | WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
        if (wolfSSL_CTX_load_verify_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cfg->cacert_buf, cfg->cacert_bytes, WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
            goto exit;
        }
        ESP_LOGD(TAG," Verify Client for Mutual Auth");
    } else {
            ESP_LOGD(TAG," Not verifying Client ");
            wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_NONE, NULL);
    }

    if (cfg->servercert_buf != NULL && cfg->serverkey_buf != NULL) {
        wolfSSL_CTX_use_certificate_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cfg->servercert_buf, cfg->servercert_bytes, WOLFSSL_FILETYPE_PEM);
        wolfSSL_CTX_use_PrivateKey_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cfg->serverkey_buf, cfg->serverkey_bytes, WOLFSSL_FILETYPE_PEM);
    } else {
        ESP_LOGE(TAG, "You have to provide both servercert_buf and serverkey_buf for https_server\n\n");
        goto exit;
    }

    tls->priv_ssl =(void *)wolfSSL_new( (WOLFSSL_CTX *)tls->priv_ctx);
    if (!tls->priv_ssl) {
        ESP_LOGE(TAG, "Create wolfSSL failed");
        goto exit;
    }

    wolfSSL_set_fd((WOLFSSL *)tls->priv_ssl, tls->sockfd);
    return ESP_OK;
exit:
    return ESP_FAIL;
}
#endif

int esp_wolfssl_handshake(esp_tls_t *tls, const esp_tls_cfg_t *cfg)
{
    int ret;
    ret = wolfSSL_connect( (WOLFSSL *)tls->priv_ssl);
    if (ret == WOLFSSL_SUCCESS) {
        tls->conn_state = ESP_TLS_DONE;
        return 1;
    } else {
        int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        if (err != ESP_TLS_ERR_SSL_WANT_READ && err != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "wolfSSL_connect returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);

            if (cfg->cacert_buf != NULL || cfg->use_global_ca_store == true) {
                /* This is to check whether handshake failed due to invalid certificate*/
                esp_wolfssl_verify_certificate(tls);
            }
            tls->conn_state = ESP_TLS_FAIL;
            return -1;
        }
        /* Irrespective of blocking or non-blocking I/O, we return on getting wolfSSL_want_read
        or wolfSSL_want_write during handshake */
        return 0;
    }
}

ssize_t esp_wolfssl_read(esp_tls_t *tls, char *data, size_t datalen)
{
    ssize_t ret = wolfSSL_read( (WOLFSSL *)tls->priv_ssl, (unsigned char *)data, datalen);
    if (ret < 0) {
        ret = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        /* peer sent close notify */
        if (ret == WOLFSSL_ERROR_ZERO_RETURN) {
            return 0;
        }

        if (ret != ESP_TLS_ERR_SSL_WANT_READ && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
            ESP_LOGE(TAG, "read error :%d:", ret);
        }
    }
    return ret;
}

ssize_t esp_wolfssl_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    ssize_t ret = wolfSSL_write( (WOLFSSL *)tls->priv_ssl, (unsigned char *) data, datalen);
    if (ret < 0) {
        ret = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
            ESP_LOGE(TAG, "write error :%d:", ret);
        }
    }
    return ret;
}

void esp_wolfssl_verify_certificate(esp_tls_t *tls)
{
    int flags;
    if ((flags = wolfSSL_get_verify_result( (WOLFSSL *)tls->priv_ssl)) != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "Failed to verify peer certificate %d!", flags);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL_CERT_FLAGS, flags);
    } else {
        ESP_LOGI(TAG, "Certificate verified.");
    }
}

ssize_t esp_wolfssl_get_bytes_avail(esp_tls_t *tls)
{
    if (!tls) {
        ESP_LOGE(TAG, "empty arg passed to esp_tls_get_bytes_avail()");
        return ESP_FAIL;
    }
    return wolfSSL_pending( (WOLFSSL *)tls->priv_ssl);
}

void esp_wolfssl_conn_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        esp_wolfssl_cleanup(tls);
    }
}

void esp_wolfssl_cleanup(esp_tls_t *tls)
{
    if (!tls) {
        return;
    }
    wolfSSL_shutdown( (WOLFSSL *)tls->priv_ssl);
    wolfSSL_free( (WOLFSSL *)tls->priv_ssl);
    wolfSSL_CTX_free( (WOLFSSL_CTX *)tls->priv_ctx);
    wolfSSL_Cleanup();
}

#ifdef CONFIG_ESP_TLS_SERVER
/**
 * @brief       Create TLS/SSL server session
 */
int esp_wolfssl_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls)
{
    if (tls == NULL || cfg == NULL) {
        return -1;
    }
    tls->role = ESP_TLS_SERVER;
    tls->sockfd = sockfd;
    esp_err_t esp_ret = esp_create_wolfssl_handle(NULL, 0, cfg, tls);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "create_ssl_handle failed");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, esp_ret);
        tls->conn_state = ESP_TLS_FAIL;
        return -1;
    }
    tls->read = esp_wolfssl_read;
    tls->write = esp_wolfssl_write;
    
    int ret;
    while ((ret = wolfSSL_accept((WOLFSSL *)tls->priv_ssl)) != WOLFSSL_SUCCESS) {
        if (ret != ESP_TLS_ERR_SSL_WANT_READ && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "wolfSSL_handshake returned %d", ret);
            tls->conn_state = ESP_TLS_FAIL;
            return ret;
        }
    }
    return 0;
}

/**
 * @brief       Close the server side TLS/SSL connection and free any allocated resources.
 */
void esp_wolfssl_server_session_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        esp_wolfssl_cleanup(tls);
        free(tls);
    }
};
#endif /* ! CONFIG_ESP_TLS_SERVER */

esp_err_t esp_wolfssl_init_global_ca_store(void)
{
    /* This function is just to provide consistancy between function calls of esp_tls.h and wolfssl */
    return ESP_OK;
}

esp_err_t esp_wolfssl_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes)
{
    if (cacert_pem_buf == NULL) {
        ESP_LOGE(TAG, "cacert_pem_buf is null");
        return ESP_ERR_INVALID_ARG;
    }
    if (global_cacert != NULL) {
        esp_wolfssl_free_global_ca_store();
    }

    global_cacert = (unsigned char *)strndup((const char *)cacert_pem_buf, cacert_pem_bytes);
    if (!global_cacert) {
        return ESP_FAIL;
    }

    global_cacert_pem_bytes  = cacert_pem_bytes;

    return ESP_OK;
}

void esp_wolfssl_free_global_ca_store(void)
{
    if (global_cacert) {
        free(global_cacert);
        global_cacert = NULL;
        global_cacert_pem_bytes = 0;
    }
}
