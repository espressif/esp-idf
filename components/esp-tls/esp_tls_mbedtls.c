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
#include "esp_tls_mbedtls.h"
#include "esp_tls_error_capture_internal.h"
#include <errno.h>
#include "esp_log.h"

#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT

#define ATECC608A_TNG_SLAVE_ADDR        0x6A
#define ATECC608A_TFLEX_SLAVE_ADDR      0x6C
#define ATECC608A_TCUSTOM_SLAVE_ADDR    0xC0
/* cryptoauthlib includes */
#include "mbedtls/atca_mbedtls_wrap.h"
#include "tng_atca.h"
#include "cryptoauthlib.h"
static const atcacert_def_t* cert_def = NULL;
/* Prototypes for functions */
static esp_err_t esp_set_atecc608a_pki_context(esp_tls_t *tls, esp_tls_cfg_t *cfg);
#endif /* CONFIG_ESP_TLS_USE_SECURE_ELEMENT */

static const char *TAG = "esp-tls-mbedtls";
static mbedtls_x509_crt *global_cacert = NULL;

typedef struct esp_tls_pki_t {
    mbedtls_x509_crt *public_cert;
    mbedtls_pk_context *pk_key;
    const unsigned char *publiccert_pem_buf;
    unsigned int publiccert_pem_bytes;
    const unsigned char *privkey_pem_buf;
    unsigned int privkey_pem_bytes;
    const unsigned char *privkey_password;
    unsigned int privkey_password_len;
} esp_tls_pki_t;

esp_err_t esp_create_mbedtls_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls)
{
    assert(cfg != NULL);
    assert(tls != NULL);
    int ret;
    esp_err_t esp_ret = ESP_FAIL;
    tls->server_fd.fd = tls->sockfd;
    mbedtls_ssl_init(&tls->ssl);
    mbedtls_ctr_drbg_init(&tls->ctr_drbg);
    mbedtls_ssl_config_init(&tls->conf);
    mbedtls_entropy_init(&tls->entropy);

    if (tls->role == ESP_TLS_CLIENT) {
        esp_ret = set_client_config(hostname, hostlen, (esp_tls_cfg_t *)cfg, tls);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set client configurations");
            goto exit;
        }
    } else if (tls->role == ESP_TLS_SERVER) {
#ifdef CONFIG_ESP_TLS_SERVER
        esp_ret = set_server_config((esp_tls_cfg_server_t *) cfg, tls);
        if (esp_ret != 0) {
            ESP_LOGE(TAG, "Failed to set server configurations");
            goto exit;
        }
#else
            ESP_LOGE(TAG, "ESP_TLS_SERVER Not enabled in Kconfig");
            goto exit;
#endif
    }

    if ((ret = mbedtls_ctr_drbg_seed(&tls->ctr_drbg,
                                     mbedtls_entropy_func, &tls->entropy, NULL, 0)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned -0x%x", -ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
        esp_ret = ESP_ERR_MBEDTLS_CTR_DRBG_SEED_FAILED;
        goto exit;
    }

    mbedtls_ssl_conf_rng(&tls->conf, mbedtls_ctr_drbg_random, &tls->ctr_drbg);

#ifdef CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log(&tls->conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif

    if ((ret = mbedtls_ssl_setup(&tls->ssl, &tls->conf)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x", -ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
        esp_ret = ESP_ERR_MBEDTLS_SSL_SETUP_FAILED;
        goto exit;
    }
    mbedtls_ssl_set_bio(&tls->ssl, &tls->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    return ESP_OK;

exit:
    esp_mbedtls_cleanup(tls);
    return esp_ret;

}

int esp_mbedtls_handshake(esp_tls_t *tls, const esp_tls_cfg_t *cfg)
{
    int ret;
    ret = mbedtls_ssl_handshake(&tls->ssl);
    if (ret == 0) {
        tls->conn_state = ESP_TLS_DONE;
        return 1;
    } else {
        if (ret != ESP_TLS_ERR_SSL_WANT_READ && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED);
            if (cfg->cacert_buf != NULL || cfg->use_global_ca_store == true) {
                /* This is to check whether handshake failed due to invalid certificate*/
                esp_mbedtls_verify_certificate(tls);
            }
            tls->conn_state = ESP_TLS_FAIL;
            return -1;
        }
        /* Irrespective of blocking or non-blocking I/O, we return on getting ESP_TLS_ERR_SSL_WANT_READ
        or ESP_TLS_ERR_SSL_WANT_WRITE during handshake */
        return 0;
    }
}

ssize_t esp_mbedtls_read(esp_tls_t *tls, char *data, size_t datalen)
{

    ssize_t ret = mbedtls_ssl_read(&tls->ssl, (unsigned char *)data, datalen);
    if (ret < 0) {
        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            return 0;
        }
        if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            ESP_LOGE(TAG, "read error :%d:", ret);
        }
    }
    return ret;
}

ssize_t esp_mbedtls_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    size_t written = 0;
    size_t write_len = datalen;
    while (written < datalen) {
        if (write_len > MBEDTLS_SSL_OUT_CONTENT_LEN) {
            write_len = MBEDTLS_SSL_OUT_CONTENT_LEN;
        }
        if (datalen > MBEDTLS_SSL_OUT_CONTENT_LEN) {
            ESP_LOGD(TAG, "Fragmenting data of excessive size :%d, offset: %d, size %d", datalen, written, write_len);
        }
        ssize_t ret = mbedtls_ssl_write(&tls->ssl, (unsigned char*) data + written, write_len);
        if (ret <= 0) {
            if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE && ret != 0) {
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, ESP_ERR_MBEDTLS_SSL_WRITE_FAILED);
                ESP_LOGE(TAG, "write error :%d:", ret);
                return ret;
            } else {
                // Exiting the tls-write process as less than desired datalen are writable
                ESP_LOGD(TAG, "mbedtls_ssl_write() returned %d, already written %d, exitting...", ret, written);
                return (written > 0) ? written : ret;
            }
        }
        written += ret;
        write_len = datalen - written;
    }
    return written;
}

void esp_mbedtls_conn_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        esp_mbedtls_cleanup(tls);
        if (tls->is_tls) {
            mbedtls_net_free(&tls->server_fd);
            tls->sockfd = tls->server_fd.fd;
        }
    }
}

void esp_mbedtls_verify_certificate(esp_tls_t *tls)
{
    int flags;
    char buf[100];
    if ((flags = mbedtls_ssl_get_verify_result(&tls->ssl)) != 0) {
        ESP_LOGI(TAG, "Failed to verify peer certificate!");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS_CERT_FLAGS, flags);
        bzero(buf, sizeof(buf));
        mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", flags);
        ESP_LOGI(TAG, "verification info: %s", buf);
    } else {
        ESP_LOGI(TAG, "Certificate verified.");
    }
}

ssize_t esp_mbedtls_get_bytes_avail(esp_tls_t *tls)
{
    if (!tls) {
        ESP_LOGE(TAG, "empty arg passed to esp_tls_get_bytes_avail()");
        return ESP_FAIL;
    }
    return mbedtls_ssl_get_bytes_avail(&tls->ssl);
}

void esp_mbedtls_cleanup(esp_tls_t *tls)
{
    if (!tls) {
        return;
    }
    if (tls->cacert_ptr != global_cacert) {
        mbedtls_x509_crt_free(tls->cacert_ptr);
    }
    tls->cacert_ptr = NULL;
#ifdef CONFIG_ESP_TLS_SERVER
    mbedtls_x509_crt_free(&tls->servercert);
    mbedtls_pk_free(&tls->serverkey);
#endif
    mbedtls_x509_crt_free(&tls->cacert);
    mbedtls_x509_crt_free(&tls->clientcert);
    mbedtls_pk_free(&tls->clientkey);
    mbedtls_entropy_free(&tls->entropy);
    mbedtls_ssl_config_free(&tls->conf);
    mbedtls_ctr_drbg_free(&tls->ctr_drbg);
    mbedtls_ssl_free(&tls->ssl);
#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
    atcab_release();
#endif
}

static esp_err_t set_ca_cert(esp_tls_t *tls, const unsigned char *cacert, size_t cacert_len)
{
    assert(tls);
    tls->cacert_ptr = &tls->cacert;
    mbedtls_x509_crt_init(tls->cacert_ptr);
    int ret = mbedtls_x509_crt_parse(tls->cacert_ptr, cacert, cacert_len);
    if (ret < 0) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x", -ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
        return ESP_ERR_MBEDTLS_X509_CRT_PARSE_FAILED;
    }
    if (ret > 0) {
        /* This will happen if the CA chain contains one or more invalid certs, going ahead as the hadshake
         * may still succeed if the other certificates in the CA chain are enough for the authentication */
        ESP_LOGW(TAG, "mbedtls_x509_crt_parse was partly successful. No. of failed certificates: %d", ret);
    }
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
    return ESP_OK;
}

static esp_err_t set_pki_context(esp_tls_t *tls, const esp_tls_pki_t *pki)
{
    assert(tls);
    assert(pki);
    int ret;

    if (pki->publiccert_pem_buf != NULL &&
        pki->privkey_pem_buf != NULL &&
        pki->public_cert != NULL &&
        pki->pk_key != NULL) {

        mbedtls_x509_crt_init(pki->public_cert);
        mbedtls_pk_init(pki->pk_key);

        ret = mbedtls_x509_crt_parse(pki->public_cert, pki->publiccert_pem_buf, pki->publiccert_pem_bytes);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_X509_CRT_PARSE_FAILED;
        }

        ret = mbedtls_pk_parse_key(pki->pk_key, pki->privkey_pem_buf, pki->privkey_pem_bytes,
                                   pki->privkey_password, pki->privkey_password_len);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_pk_parse_keyfile returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_PK_PARSE_KEY_FAILED;
        }

        ret = mbedtls_ssl_conf_own_cert(&tls->conf, pki->public_cert, pki->pk_key);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_conf_own_cert returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_SSL_CONF_OWN_CERT_FAILED;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static esp_err_t set_global_ca_store(esp_tls_t *tls)
{
    assert(tls);
    if (global_cacert == NULL) {
        ESP_LOGE(TAG, "global_cacert is NULL");
        return ESP_ERR_INVALID_STATE;
    }
    tls->cacert_ptr = global_cacert;
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
    return ESP_OK;
}


#ifdef CONFIG_ESP_TLS_SERVER
esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls)
{
    assert(cfg != NULL);
    assert(tls != NULL);
    int ret;
    esp_err_t esp_ret;
    if ((ret = mbedtls_ssl_config_defaults(&tls->conf,
                    MBEDTLS_SSL_IS_SERVER,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
        return ESP_ERR_MBEDTLS_SSL_CONFIG_DEFAULTS_FAILED;
    }

#ifdef CONFIG_MBEDTLS_SSL_ALPN
    if (cfg->alpn_protos) {
        mbedtls_ssl_conf_alpn_protocols(&tls->conf, cfg->alpn_protos);
    }
#endif

    if (cfg->cacert_buf != NULL) {
        esp_ret = set_ca_cert(tls, cfg->cacert_buf, cfg->cacert_bytes);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
    } else {
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
    }

    if (cfg->servercert_buf != NULL && cfg->serverkey_buf != NULL) {
        esp_tls_pki_t pki = {
            .public_cert = &tls->servercert,
            .pk_key = &tls->serverkey,
            .publiccert_pem_buf = cfg->servercert_buf,
            .publiccert_pem_bytes = cfg->servercert_bytes,
            .privkey_pem_buf = cfg->serverkey_buf,
            .privkey_pem_bytes = cfg->serverkey_bytes,
            .privkey_password = cfg->serverkey_password,
            .privkey_password_len = cfg->serverkey_password_len,
        };
        esp_ret = set_pki_context(tls, &pki);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set server pki context");
            return esp_ret;
        }
    } else {
        ESP_LOGE(TAG, "Missing server certificate and/or key");
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}
#endif /* ! CONFIG_ESP_TLS_SERVER */

esp_err_t set_client_config(const char *hostname, size_t hostlen, esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    assert(cfg != NULL);
    assert(tls != NULL);
    int ret;
    if (!cfg->skip_common_name) {
        char *use_host = NULL;
        if (cfg->common_name != NULL) {
            use_host = strndup(cfg->common_name, strlen(cfg->common_name));
        } else {
            use_host = strndup(hostname, hostlen);
        }

        if (use_host == NULL) {
            return ESP_ERR_NO_MEM;
        }
        /* Hostname set here should match CN in server certificate */
        if ((ret = mbedtls_ssl_set_hostname(&tls->ssl, use_host)) != 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            free(use_host);
            return ESP_ERR_MBEDTLS_SSL_SET_HOSTNAME_FAILED;
        }
        free(use_host);
    }

    if ((ret = mbedtls_ssl_config_defaults(&tls->conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned -0x%x", -ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
        return ESP_ERR_MBEDTLS_SSL_CONFIG_DEFAULTS_FAILED;
    }

#ifdef CONFIG_MBEDTLS_SSL_RENEGOTIATION
    mbedtls_ssl_conf_renegotiation(&tls->conf, MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#endif

    if (cfg->alpn_protos) {
#ifdef CONFIG_MBEDTLS_SSL_ALPN
        if ((ret = mbedtls_ssl_conf_alpn_protocols(&tls->conf, cfg->alpn_protos)) != 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_conf_alpn_protocols returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_SSL_CONF_ALPN_PROTOCOLS_FAILED;
        }
#else
        ESP_LOGE(TAG, "alpn_protos configured but not enabled in menuconfig: Please enable MBEDTLS_SSL_ALPN option");
        return ESP_ERR_INVALID_STATE;
#endif
    }

    if (cfg->crt_bundle_attach != NULL) {
#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        ESP_LOGD(TAG, "Use certificate bundle");
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        cfg->crt_bundle_attach(&tls->conf);
#else //CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        ESP_LOGE(TAG, "use_crt_bundle configured but not enabled in menuconfig: Please enable MBEDTLS_CERTIFICATE_BUNDLE option");
        return ESP_ERR_INVALID_STATE;
#endif
    } else if (cfg->use_global_ca_store == true) {
        esp_err_t esp_ret = set_global_ca_store(tls);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
    } else if (cfg->cacert_buf != NULL) {
        esp_err_t esp_ret = set_ca_cert(tls, cfg->cacert_buf, cfg->cacert_bytes);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
        mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
    } else if (cfg->psk_hint_key) {
#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
        //
        // PSK encryption mode is configured only if no certificate supplied and psk pointer not null
        ESP_LOGD(TAG, "ssl psk authentication");
        ret = mbedtls_ssl_conf_psk(&tls->conf, cfg->psk_hint_key->key, cfg->psk_hint_key->key_size,
                                   (const unsigned char *)cfg->psk_hint_key->hint, strlen(cfg->psk_hint_key->hint));
        if (ret != 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_conf_psk returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_SSL_CONF_PSK_FAILED;
        }
#else
        ESP_LOGE(TAG, "psk_hint_key configured but not enabled in menuconfig: Please enable ESP_TLS_PSK_VERIFICATION option");
        return ESP_ERR_INVALID_STATE;
#endif
    } else {
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
    }

    if (cfg->use_secure_element) {
#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
        ret = esp_set_atecc608a_pki_context(tls, (esp_tls_cfg_t *)cfg);
        if (ret != ESP_OK) {
            return ret;
        }
#else
        ESP_LOGE(TAG, "Please enable secure element support for ESP-TLS in menuconfig");
        return ESP_FAIL;
#endif /* CONFIG_ESP_TLS_USE_SECURE_ELEMENT */
    } else if (cfg->clientcert_pem_buf != NULL && cfg->clientkey_pem_buf != NULL) {
        esp_tls_pki_t pki = {
            .public_cert = &tls->clientcert,
            .pk_key = &tls->clientkey,
            .publiccert_pem_buf = cfg->clientcert_buf,
            .publiccert_pem_bytes = cfg->clientcert_bytes,
            .privkey_pem_buf = cfg->clientkey_buf,
            .privkey_pem_bytes = cfg->clientkey_bytes,
            .privkey_password = cfg->clientkey_password,
            .privkey_password_len = cfg->clientkey_password_len,
        };
        esp_err_t esp_ret = set_pki_context(tls, &pki);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set client pki context");
            return esp_ret;
        }
    } else if (cfg->clientcert_buf != NULL || cfg->clientkey_buf != NULL) {
        ESP_LOGE(TAG, "You have to provide both clientcert_buf and clientkey_buf for mutual authentication");
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

#ifdef CONFIG_ESP_TLS_SERVER
/**
 * @brief      Create TLS/SSL server session
 */
int esp_mbedtls_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls)
{
    if (tls == NULL || cfg == NULL) {
        return -1;
    }
    tls->role = ESP_TLS_SERVER;
    tls->sockfd = sockfd;
    esp_err_t esp_ret = esp_create_mbedtls_handle(NULL, 0, cfg, tls);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "create_ssl_handle failed");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, esp_ret);
        tls->conn_state = ESP_TLS_FAIL;
        return -1;
    }
    tls->read = esp_mbedtls_read;
    tls->write = esp_mbedtls_write;
    int ret;
    while ((ret = mbedtls_ssl_handshake(&tls->ssl)) != 0) {
        if (ret != ESP_TLS_ERR_SSL_WANT_READ && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "mbedtls_ssl_handshake returned %d", ret);
            tls->conn_state = ESP_TLS_FAIL;
            return ret;
        }
    }
    return 0;
}
/**
 * @brief      Close the server side TLS/SSL connection and free any allocated resources.
 */
void esp_mbedtls_server_session_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        esp_mbedtls_cleanup(tls);
        free(tls);
    }
};
#endif /* ! CONFIG_ESP_TLS_SERVER */

esp_err_t esp_mbedtls_init_global_ca_store(void)
{
    if (global_cacert == NULL) {
        global_cacert = (mbedtls_x509_crt *)calloc(1, sizeof(mbedtls_x509_crt));
        if (global_cacert == NULL) {
            ESP_LOGE(TAG, "global_cacert not allocated");
            return ESP_ERR_NO_MEM;
        }
        mbedtls_x509_crt_init(global_cacert);
    }
    return ESP_OK;
}

esp_err_t esp_mbedtls_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes)
{
#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT
    ESP_LOGE(TAG, "Please disable dynamic freeing of ca cert in mbedtls (CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT)\n in order to use the global ca_store");
    return ESP_FAIL;
#endif
    if (cacert_pem_buf == NULL) {
        ESP_LOGE(TAG, "cacert_pem_buf is null");
        return ESP_ERR_INVALID_ARG;
    }
    int ret;
    if (global_cacert == NULL) {
        ret = esp_mbedtls_init_global_ca_store();
        if (ret != ESP_OK) {
            return ret;
        }
    }
    ret = mbedtls_x509_crt_parse(global_cacert, cacert_pem_buf, cacert_pem_bytes);
    if (ret < 0) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x", -ret);
        mbedtls_x509_crt_free(global_cacert);
        free(global_cacert);
        global_cacert = NULL;
        return ESP_FAIL;
    } else if (ret > 0) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse was partly successful. No. of failed certificates: %d", ret);
        return ESP_ERR_MBEDTLS_CERT_PARTLY_OK;
    }
    return ESP_OK;
}

mbedtls_x509_crt *esp_mbedtls_get_global_ca_store(void)
{
    return global_cacert;
}

void esp_mbedtls_free_global_ca_store(void)
{
    if (global_cacert) {
        mbedtls_x509_crt_free(global_cacert);
        free(global_cacert);
        global_cacert = NULL;
    }
}

#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
static esp_err_t esp_init_atecc608a(uint8_t slave_addr)
{
    cfg_ateccx08a_i2c_default.atcai2c.slave_address = slave_addr;
    int ret = atcab_init(&cfg_ateccx08a_i2c_default);
    if(ret != 0) {
        ESP_LOGE(TAG, "Failed\n !atcab_init returned %02x", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t esp_set_atecc608a_pki_context(esp_tls_t *tls, esp_tls_cfg_t *cfg)
{
    int ret = 0;
    int esp_ret = ESP_FAIL;
    ESP_LOGI(TAG, "Initialize the ATECC interface...");
#if defined(CONFIG_ATECC608A_TNG) || defined(CONFIG_ATECC608A_TFLEX)
#ifdef CONFIG_ATECC608A_TNG
    esp_ret = esp_init_atecc608a(ATECC608A_TNG_SLAVE_ADDR);
    if (ret != ESP_OK) {
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }
#elif CONFIG_ATECC608A_TFLEX /* CONFIG_ATECC608A_TNG */
    esp_ret = esp_init_atecc608a(ATECC608A_TFLEX_SLAVE_ADDR);
    if (ret != ESP_OK) {
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }
#endif /* CONFIG_ATECC608A_TFLEX */
    mbedtls_x509_crt_init(&tls->clientcert);
    ret = tng_get_device_cert_def(&cert_def);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to get device cert def");
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }

    /* Extract the device certificate and convert to mbedtls cert */
    ret = atca_mbedtls_cert_add(&tls->clientcert, cert_def);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to parse cert from device, return %02x", ret);
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }
#elif CONFIG_ATECC608A_TCUSTOM
    esp_ret = esp_init_atecc608a(ATECC608A_TCUSTOM_SLAVE_ADDR);
    if (ret != ESP_OK) {
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }
    mbedtls_x509_crt_init(&tls->clientcert);

    if(cfg->clientcert_buf != NULL) {
        ret = mbedtls_x509_crt_parse(&tls->clientcert, (const unsigned char*)cfg->clientcert_buf, cfg->clientcert_bytes);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_X509_CRT_PARSE_FAILED;
        }
    } else {
        ESP_LOGE(TAG, "Device certificate must be provided for TrustCustom Certs");
        return ESP_FAIL;
    }
#endif /* CONFIG_ATECC608A_TCUSTOM */
    ret = atca_mbedtls_pk_init(&tls->clientkey, 0);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to parse key from device");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }

    ret = mbedtls_ssl_conf_own_cert(&tls->conf, &tls->clientcert, &tls->clientkey);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed\n ! mbedtls_ssl_conf_own_cert returned -0x%x", ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }
    return ESP_OK;
}
#endif /* CONFIG_ESP_TLS_USE_SECURE_ELEMENT */
