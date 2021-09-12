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

/* Prototypes for the static functions */
static esp_err_t set_client_config(const char *hostname, size_t hostlen, esp_tls_cfg_t *cfg, esp_tls_t *tls);

#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
#include "freertos/semphr.h"
static SemaphoreHandle_t tls_conn_lock;
static inline unsigned int esp_wolfssl_psk_client_cb(WOLFSSL* ssl, const char* hint, char* identity,
        unsigned int id_max_len, unsigned char* key,unsigned int key_max_len);
static esp_err_t esp_wolfssl_set_cipher_list(WOLFSSL_CTX *ctx);
#ifdef WOLFSSL_TLS13
#define PSK_MAX_ID_LEN 128
#else
#define PSK_MAX_ID_LEN 64
#endif
#define PSK_MAX_KEY_LEN 64

static char psk_id_str[PSK_MAX_ID_LEN];
static uint8_t psk_key_array[PSK_MAX_KEY_LEN];
static uint8_t psk_key_max_len = 0;
#endif /* CONFIG_ESP_TLS_PSK_VERIFICATION */

#ifdef CONFIG_ESP_TLS_SERVER
static esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls);
#endif /* CONFIG_ESP_TLS_SERVER */

typedef enum x509_file_type {
    FILE_TYPE_CA_CERT = 0, /* CA certificate to authenticate entity at other end */
    FILE_TYPE_SELF_CERT, /* Self certificate of the entity */
    FILE_TYPE_SELF_KEY, /* Private key in the self cert-key pair */
} x509_file_type_t;

/* Checks whether the certificate provided is in pem format or not */
static esp_err_t esp_load_wolfssl_verify_buffer(esp_tls_t *tls, const unsigned char *cert_buf, unsigned int cert_len, x509_file_type_t type, int *err_ret)
{
    int wolf_fileformat = WOLFSSL_FILETYPE_DEFAULT;
    if (type == FILE_TYPE_SELF_KEY) {
        if (cert_buf[cert_len - 1] == '\0' && strstr( (const char *) cert_buf, "-----BEGIN " )) {
            wolf_fileformat = WOLFSSL_FILETYPE_PEM;
        } else {
            wolf_fileformat = WOLFSSL_FILETYPE_ASN1;
        }
        if ((*err_ret = wolfSSL_CTX_use_PrivateKey_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cert_buf, cert_len, wolf_fileformat)) == WOLFSSL_SUCCESS) {
            return ESP_OK;
        }
        return ESP_FAIL;
    } else {
        if (cert_buf[cert_len - 1] == '\0' && strstr( (const char *) cert_buf, "-----BEGIN CERTIFICATE-----" )) {
            wolf_fileformat = WOLFSSL_FILETYPE_PEM;
        } else {
            wolf_fileformat = WOLFSSL_FILETYPE_ASN1;
        }
        if (type == FILE_TYPE_SELF_CERT) {
            if ((*err_ret = wolfSSL_CTX_use_certificate_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cert_buf, cert_len, wolf_fileformat)) == WOLFSSL_SUCCESS) {
                return ESP_OK;
            }
            return ESP_FAIL;
        } else if (type == FILE_TYPE_CA_CERT) {
            if ((*err_ret = wolfSSL_CTX_load_verify_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cert_buf, cert_len, wolf_fileformat)) == WOLFSSL_SUCCESS) {
                return ESP_OK;
            }
            return ESP_FAIL;
        } else {
            /* Wrong file type provided */
            return ESP_FAIL;
        }
    }
}

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
        ESP_LOGE(TAG, "ESP_TLS_SERVER Not enabled in menuconfig");
        goto exit;
#endif
    }
    else {
        ESP_LOGE(TAG, "tls->role is not valid");
        goto exit;
    }

    return ESP_OK;
exit:
    esp_wolfssl_cleanup(tls);
    return esp_ret;
}

static esp_err_t set_client_config(const char *hostname, size_t hostlen, esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    int ret = WOLFSSL_FAILURE;
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    if (!tls->priv_ctx) {
        ESP_LOGE(TAG, "Set wolfSSL ctx failed");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
        return ESP_ERR_WOLFSSL_CTX_SETUP_FAILED;
    }

    if (cfg->crt_bundle_attach != NULL) {
        ESP_LOGE(TAG,"use_crt_bundle not supported in wolfssl");
        return ESP_FAIL;
    }

    if (cfg->use_global_ca_store == true) {
        if ((esp_load_wolfssl_verify_buffer(tls, global_cacert, global_cacert_pem_bytes, FILE_TYPE_CA_CERT, &ret)) != ESP_OK) {
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d", ret);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER, NULL);
    } else if (cfg->cacert_buf != NULL) {
        if ((esp_load_wolfssl_verify_buffer(tls, cfg->cacert_buf, cfg->cacert_bytes, FILE_TYPE_CA_CERT, &ret)) != ESP_OK) {
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d", ret);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER, NULL);
    } else if (cfg->psk_hint_key) {
#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
        /*** PSK encryption mode is configured only if no certificate supplied and psk pointer not null ***/
        if(cfg->psk_hint_key->key == NULL || cfg->psk_hint_key->hint == NULL || cfg->psk_hint_key->key_size <= 0) {
            ESP_LOGE(TAG, "Please provide appropriate key, keysize and hint to use PSK");
            return ESP_FAIL;
        }
        /* mutex is given back when call back function executes or in case of failure (at cleanup) */
        if ((xSemaphoreTake(tls_conn_lock, 1000/portTICK_PERIOD_MS) != pdTRUE)) {
            ESP_LOGE(TAG, "tls_conn_lock could not be obtained in specified time");
            return -1;
        }
        ESP_LOGI(TAG, "setting psk configurations");
        if((cfg->psk_hint_key->key_size > PSK_MAX_KEY_LEN) || (strlen(cfg->psk_hint_key->hint) > PSK_MAX_ID_LEN)) {
            ESP_LOGE(TAG, "psk key length should be <= %d and identity hint length should be <= %d", PSK_MAX_KEY_LEN, PSK_MAX_ID_LEN);
            return ESP_ERR_INVALID_ARG;
        }
        psk_key_max_len = cfg->psk_hint_key->key_size;
        memset(psk_key_array, 0, sizeof(psk_key_array));
        memset(psk_id_str, 0, sizeof(psk_id_str));
        memcpy(psk_key_array, cfg->psk_hint_key->key, psk_key_max_len);
        memcpy(psk_id_str, cfg->psk_hint_key->hint, strlen(cfg->psk_hint_key->hint));
        wolfSSL_CTX_set_psk_client_callback( (WOLFSSL_CTX *)tls->priv_ctx, esp_wolfssl_psk_client_cb);
        if(esp_wolfssl_set_cipher_list( (WOLFSSL_CTX *)tls->priv_ctx) != ESP_OK) {
            ESP_LOGE(TAG, "error in setting cipher-list");
            return ESP_FAIL;
        }
#else
        ESP_LOGE(TAG, "psk_hint_key configured but not enabled in menuconfig: Please enable ESP_TLS_PSK_VERIFICATION option");
        return ESP_ERR_INVALID_STATE;
#endif
    } else {
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_NONE, NULL);
    }

    if (cfg->clientcert_buf != NULL && cfg->clientkey_buf != NULL) {
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->clientcert_buf, cfg->clientcert_bytes, FILE_TYPE_SELF_CERT, &ret)) != ESP_OK) {
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d", ret);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->clientkey_buf, cfg->clientkey_bytes, FILE_TYPE_SELF_KEY, &ret)) != ESP_OK) {
            ESP_LOGE(TAG, "Error in loading private key verify buffer, returned %d", ret);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
    } else if (cfg->clientcert_buf != NULL || cfg->clientkey_buf != NULL) {
        ESP_LOGE(TAG, "You have to provide both clientcert_buf and clientkey_buf for mutual authentication\n\n");
        return ESP_FAIL;
    }

    tls->priv_ssl =(void *)wolfSSL_new( (WOLFSSL_CTX *)tls->priv_ctx);
    if (!tls->priv_ssl) {
        ESP_LOGE(TAG, "Create wolfSSL failed");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
        return ESP_ERR_WOLFSSL_SSL_SETUP_FAILED;
    }

    if (!cfg->skip_common_name) {
        char *use_host = NULL;
        if (cfg->common_name != NULL) {
            use_host = strdup(cfg->common_name);
        } else {
            use_host = strndup(hostname, hostlen);
        }
        if (use_host == NULL) {
            return ESP_ERR_NO_MEM;
        }
        /* Hostname set here should match CN in server certificate */
        if ((ret = (wolfSSL_check_domain_name( (WOLFSSL *)tls->priv_ssl, use_host))) != WOLFSSL_SUCCESS) {
            ESP_LOGE(TAG, "wolfSSL_check_domain_name returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
            free(use_host);
            return ESP_ERR_WOLFSSL_SSL_SET_HOSTNAME_FAILED;
        }
        free(use_host);
    }

    if (cfg->alpn_protos) {
#ifdef CONFIG_WOLFSSL_HAVE_ALPN
        char **alpn_list = (char **)cfg->alpn_protos;
        for (; *alpn_list != NULL; alpn_list ++) {
            ESP_LOGD(TAG, "alpn protocol is %s", *alpn_list);
            if ((ret = wolfSSL_UseALPN( (WOLFSSL *)tls->priv_ssl, *alpn_list, strlen(*alpn_list), WOLFSSL_ALPN_FAILED_ON_MISMATCH)) != WOLFSSL_SUCCESS) {
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
                ESP_LOGE(TAG, "wolfSSL UseALPN failed, returned %d", ret);
                return ESP_ERR_WOLFSSL_SSL_CONF_ALPN_PROTOCOLS_FAILED;
            }
        }
#else
    ESP_LOGE(TAG, "CONFIG_WOLFSSL_HAVE_ALPN not enabled in menuconfig");
    return ESP_FAIL;
#endif /* CONFIG_WOLFSSL_HAVE_ALPN */
    }

    wolfSSL_set_fd((WOLFSSL *)tls->priv_ssl, tls->sockfd);
    return ESP_OK;
}

#ifdef CONFIG_ESP_TLS_SERVER
static esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls)
{
    int ret = WOLFSSL_FAILURE;
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfTLSv1_2_server_method());
    if (!tls->priv_ctx) {
        ESP_LOGE(TAG, "Set wolfSSL ctx failed");
        return ESP_ERR_WOLFSSL_CTX_SETUP_FAILED;
    }

    if (cfg->cacert_buf != NULL) {
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->cacert_buf, cfg->cacert_bytes, FILE_TYPE_CA_CERT, &ret)) != ESP_OK) {
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d", ret);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER | WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
        ESP_LOGD(TAG," Verify Client for Mutual Auth");
    } else {
        ESP_LOGD(TAG," Not verifying Client ");
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_NONE, NULL);
    }

    if (cfg->servercert_buf != NULL && cfg->serverkey_buf != NULL) {
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->servercert_buf, cfg->servercert_bytes, FILE_TYPE_SELF_CERT, &ret)) != ESP_OK) {
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d", ret);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->serverkey_buf, cfg->serverkey_bytes, FILE_TYPE_SELF_KEY, &ret)) != ESP_OK) {
            ESP_LOGE(TAG, "Error in loading private key verify buffer, returned %d", ret);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
    } else {
        ESP_LOGE(TAG, "You have to provide both servercert_buf and serverkey_buf for https_server\n\n");
        return ESP_FAIL;
    }

    tls->priv_ssl =(void *)wolfSSL_new( (WOLFSSL_CTX *)tls->priv_ctx);
    if (!tls->priv_ssl) {
        ESP_LOGE(TAG, "Create wolfSSL failed");
        return ESP_ERR_WOLFSSL_SSL_SETUP_FAILED;
    }

    wolfSSL_set_fd((WOLFSSL *)tls->priv_ssl, tls->sockfd);
    return ESP_OK;
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
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, ESP_ERR_WOLFSSL_SSL_HANDSHAKE_FAILED);
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
    if (ret <= 0) {
        ret = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, ESP_ERR_WOLFSSL_SSL_WRITE_FAILED);
            ESP_LOGE(TAG, "write error :%d:", ret);

        }
    }
    return ret;
}

void esp_wolfssl_verify_certificate(esp_tls_t *tls)
{
    int flags;
    if ((flags = wolfSSL_get_verify_result( (WOLFSSL *)tls->priv_ssl)) != X509_V_OK) {
        ESP_LOGE(TAG, "Failed to verify peer certificate , returned %d!", flags);
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
#ifdef CONFIG_ESP_TLS_PSK_VERIFICATION
    xSemaphoreGive(tls_conn_lock);
#endif /* CONFIG_ESP_TLS_PSK_VERIFICATION */
    wolfSSL_shutdown( (WOLFSSL *)tls->priv_ssl);
    wolfSSL_free( (WOLFSSL *)tls->priv_ssl);
    tls->priv_ssl = NULL;
    wolfSSL_CTX_free( (WOLFSSL_CTX *)tls->priv_ctx);
    tls->priv_ctx = NULL;
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
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_WOLFSSL, -ret);
            ESP_LOGE(TAG, "wolfSSL_handshake_server returned %d", ret);
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
}
#endif /* CONFIG_ESP_TLS_SERVER */

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

#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
static esp_err_t esp_wolfssl_set_cipher_list(WOLFSSL_CTX *ctx)
{
    const char *defaultCipherList;
    int ret;
#if defined(HAVE_AESGCM) && !defined(NO_DH)
#ifdef WOLFSSL_TLS13
    defaultCipherList = "DHE-PSK-AES128-GCM-SHA256:"
                                    "TLS13-AES128-GCM-SHA256";
#else
    defaultCipherList = "DHE-PSK-AES128-GCM-SHA256";
#endif
#elif defined(HAVE_NULL_CIPHER)
    defaultCipherList = "PSK-NULL-SHA256";
#else
    defaultCipherList = "PSK-AES128-CBC-SHA256";
#endif
    ESP_LOGD(TAG, "cipher list is %s", defaultCipherList);
    if ((ret = wolfSSL_CTX_set_cipher_list(ctx,defaultCipherList)) != WOLFSSL_SUCCESS) {
        wolfSSL_CTX_free(ctx);
        ESP_LOGE(TAG, "can't set cipher list, returned %02x", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

/* initialize the mutex before app_main() when using PSK */
static void __attribute__((constructor))
espt_tls_wolfssl_init_conn_lock (void)
{
    if ((tls_conn_lock = xSemaphoreCreateMutex()) == NULL) {
        ESP_EARLY_LOGE(TAG, "mutex for tls psk connection could not be created");
    }
}

/* Some callback functions required by PSK */
static inline unsigned int esp_wolfssl_psk_client_cb(WOLFSSL* ssl, const char* hint,
        char* identity, unsigned int id_max_len, unsigned char* key,
        unsigned int key_max_len)
{
    (void)key_max_len;

    /* see internal.h MAX_PSK_ID_LEN for PSK identity limit */
    memcpy(identity, psk_id_str, id_max_len);
    for(int count = 0; count < psk_key_max_len; count ++) {
         key[count] = psk_key_array[count];
    }
    xSemaphoreGive(tls_conn_lock);
    return psk_key_max_len;
    /* return length of key in octets or 0 or for error */
}
#endif /* CONFIG_ESP_TLS_PSK_VERIFICATION */
