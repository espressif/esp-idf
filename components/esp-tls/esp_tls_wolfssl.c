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

int esp_create_wolfssl_handle(const char *hostname, size_t hostlen, const void *cfg1, esp_tls_t *tls)
{
#ifdef CONFIG_ESP_DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif
    const esp_tls_cfg_t *cfg = cfg1;
    assert(cfg != NULL);
    assert(tls != NULL);

    int ret;
    ret = wolfSSL_Init();
    if (ret != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "Init wolfSSL failed: %d", ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
        goto exit;
    }

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
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, SSL_VERIFY_PEER, NULL);
    } else if (cfg->cacert_pem_buf != NULL) {
        wolfSSL_CTX_load_verify_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cfg->cacert_pem_buf, cfg->cacert_pem_bytes, WOLFSSL_FILETYPE_PEM);
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, SSL_VERIFY_PEER, NULL);
    } else if (cfg->psk_hint_key) {
        ESP_LOGE(TAG,"psk_hint_key not supported in wolfssl");
        goto exit;
    } else {
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, SSL_VERIFY_NONE, NULL);
    }

    if (cfg->clientcert_pem_buf != NULL && cfg->clientkey_pem_buf != NULL) {
        wolfSSL_CTX_use_certificate_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cfg->clientcert_pem_buf, cfg->clientcert_pem_bytes, WOLFSSL_FILETYPE_PEM);
        wolfSSL_CTX_use_PrivateKey_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cfg->clientkey_pem_buf, cfg->clientkey_pem_bytes, WOLFSSL_FILETYPE_PEM);
    } else if (cfg->clientcert_pem_buf != NULL || cfg->clientkey_pem_buf != NULL) {
        ESP_LOGE(TAG, "You have to provide both clientcert_pem_buf and clientkey_pem_buf for mutual authentication\n\n");
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
    return 0;
exit:
    esp_wolfssl_cleanup(tls);
    return ret;
}

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

            if (cfg->cacert_pem_buf != NULL || cfg->use_global_ca_store == true) {
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
