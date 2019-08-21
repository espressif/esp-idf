// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_tls.h"
#include "esp_tls_error_capture_internal.h"
#include <errno.h>

static const char *TAG = "esp-tls";
static mbedtls_x509_crt *global_cacert = NULL;

#ifdef ESP_PLATFORM
#include <esp_log.h>
#else
#define ESP_LOGD(TAG, ...) //printf(__VA_ARGS__);
#define ESP_LOGE(TAG, ...) printf(__VA_ARGS__);
#endif

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

static esp_err_t resolve_host_name(const char *host, size_t hostlen, struct addrinfo **address_info)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char *use_host = strndup(host, hostlen);
    if (!use_host) {
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGD(TAG, "host:%s: strlen %lu", use_host, (unsigned long)hostlen);
    if (getaddrinfo(use_host, NULL, &hints, address_info)) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s:", use_host);
        free(use_host);
        return ESP_ERR_ESP_TLS_CANNOT_RESOLVE_HOSTNAME;
    }
    free(use_host);
    return ESP_OK;
}

static ssize_t tcp_read(esp_tls_t *tls, char *data, size_t datalen)
{
    return recv(tls->sockfd, data, datalen, 0);
}

static ssize_t tls_read(esp_tls_t *tls, char *data, size_t datalen)
{
    ssize_t ret = mbedtls_ssl_read(&tls->ssl, (unsigned char *)data, datalen);   
    if (ret < 0) {
        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            return 0;
        }
        if (ret != MBEDTLS_ERR_SSL_WANT_READ  && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            ESP_LOGE(TAG, "read error :%d:", ret);
        }
    }
    return ret;
}

static void ms_to_timeval(int timeout_ms, struct timeval *tv)
{
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms % 1000) * 1000;
}

static esp_err_t esp_tcp_connect(const char *host, int hostlen, int port, int *sockfd, const esp_tls_t *tls, const esp_tls_cfg_t *cfg)
{
    esp_err_t ret;
    struct addrinfo *addrinfo;
    if ((ret = resolve_host_name(host, hostlen, &addrinfo)) != ESP_OK) {
        return ret;
    }

    int fd = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
    if (fd < 0) {
        ESP_LOGE(TAG, "Failed to create socket (family %d socktype %d protocol %d)", addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_SYSTEM, errno);
        ret = ESP_ERR_ESP_TLS_CANNOT_CREATE_SOCKET;
        goto err_freeaddr;
    }

    void *addr_ptr;
    if (addrinfo->ai_family == AF_INET) {
        struct sockaddr_in *p = (struct sockaddr_in *)addrinfo->ai_addr;
        p->sin_port = htons(port);
        addr_ptr = p;
    } else if (addrinfo->ai_family == AF_INET6) {
        struct sockaddr_in6 *p = (struct sockaddr_in6 *)addrinfo->ai_addr;
        p->sin6_port = htons(port);
        p->sin6_family = AF_INET6;
        addr_ptr = p;
    } else {
        ESP_LOGE(TAG, "Unsupported protocol family %d", addrinfo->ai_family);
        ret = ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY;
        goto err_freesocket;
    }

    if (cfg) {
        if (cfg->timeout_ms >= 0) {
            struct timeval tv;
            ms_to_timeval(cfg->timeout_ms, &tv);
            setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        }
        if (cfg->non_block) {
            int flags = fcntl(fd, F_GETFL, 0);
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }
    }

    ret = connect(fd, addr_ptr, addrinfo->ai_addrlen);
    if (ret < 0 && !(errno == EINPROGRESS && cfg && cfg->non_block)) {

        ESP_LOGE(TAG, "Failed to connnect to host (errno %d)", errno);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_SYSTEM, errno);
        ret = ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST;
        goto err_freesocket;
    }

    *sockfd = fd;
    freeaddrinfo(addrinfo);
    return ESP_OK;

err_freesocket:
    close(fd);
err_freeaddr:
    freeaddrinfo(addrinfo);
    return ret;
}

esp_err_t esp_tls_init_global_ca_store(void)
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

esp_err_t esp_tls_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes)
{
    int ret;
    if (cacert_pem_buf == NULL) {
        ESP_LOGE(TAG, "cacert_pem_buf is null");
        return ESP_ERR_INVALID_ARG;
    }
    if (global_cacert == NULL) {
        ret = esp_tls_init_global_ca_store();
        if (ret != ESP_OK) {
            return ret;
        }
    }
    ret = mbedtls_x509_crt_parse(global_cacert, cacert_pem_buf, cacert_pem_bytes);
    if (ret < 0) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x", -ret);
        mbedtls_x509_crt_free(global_cacert);
        global_cacert = NULL;
        return ESP_FAIL;
    } else if (ret > 0) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse was partly successful. No. of failed certificates: %d", ret);
        return ESP_ERR_MBEDTLS_CERT_PARTLY_OK;
    }
    return ESP_OK;
}

mbedtls_x509_crt *esp_tls_get_global_ca_store(void)
{
    return global_cacert;
}

void esp_tls_free_global_ca_store(void)
{
    if (global_cacert) {
        mbedtls_x509_crt_free(global_cacert);
        global_cacert = NULL;
    }
}

static void verify_certificate(esp_tls_t *tls)
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

static void mbedtls_cleanup(esp_tls_t *tls) 
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
                  NULL, 0);
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

#ifdef CONFIG_ESP_TLS_SERVER
static esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls)
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

static esp_err_t set_client_config(const char *hostname, size_t hostlen, esp_tls_cfg_t *cfg, esp_tls_t *tls)
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

#ifdef CONFIG_MBEDTLS_SSL_ALPN
    if (cfg->alpn_protos) {
        if ((ret =mbedtls_ssl_conf_alpn_protocols(&tls->conf, cfg->alpn_protos) != 0)) {
            ESP_LOGE(TAG, "mbedtls_ssl_conf_alpn_protocols returned -0x%x", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_SSL_CONF_ALPN_PROTOCOLS_FAILED;
        }
    }
#endif
    if (cfg->use_global_ca_store == true) {
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

    if (cfg->clientcert_buf != NULL && cfg->clientkey_buf != NULL) {
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

static esp_err_t create_ssl_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls)
{
    assert(cfg != NULL);
    assert(tls != NULL);
    int ret;
    esp_err_t esp_ret;
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
#ifdef CONFIG_ESP_TLS_SERVER
    } else if (tls->role == ESP_TLS_SERVER) {
        esp_ret = set_server_config((esp_tls_cfg_server_t *) cfg, tls);
        if (esp_ret != 0) {
            ESP_LOGE(TAG, "Failed to set server configurations");
            goto exit;
        }
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
    mbedtls_cleanup(tls);
    return esp_ret;
}

/**
 * @brief      Close the TLS connection and free any allocated resources.
 */
void esp_tls_conn_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        mbedtls_cleanup(tls);
        if (tls->is_tls) {
            mbedtls_net_free(&tls->server_fd);
        } else if (tls->sockfd >= 0) {
            close(tls->sockfd);
        }
        free(tls->error_handle);
        free(tls);
    }
};

static ssize_t tcp_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    return send(tls->sockfd, data, datalen, 0);
}

static ssize_t tls_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    ssize_t ret = mbedtls_ssl_write(&tls->ssl, (unsigned char*) data, datalen);
    if (ret < 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ  && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, ESP_ERR_MBEDTLS_SSL_WRITE_FAILED);
            ESP_LOGE(TAG, "write error :%d:", ret);
        }
    }
    return ret;
}

static int esp_tls_low_level_conn(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    if (!tls) {
        ESP_LOGE(TAG, "empty esp_tls parameter");
        return -1;
    }
    esp_err_t esp_ret;
    int ret;

    /* These states are used to keep a tab on connection progress in case of non-blocking connect,
    and in case of blocking connect these cases will get executed one after the other */
    switch (tls->conn_state) {
        case ESP_TLS_INIT:
            tls->sockfd = -1;
            if (cfg != NULL) {
                mbedtls_net_init(&tls->server_fd);
                tls->is_tls = true;
            }
            if ((esp_ret = esp_tcp_connect(hostname, hostlen, port, &tls->sockfd, tls, cfg)) != ESP_OK) {
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, esp_ret);
                return -1;
            }
            if (!cfg) {
                tls->read = tcp_read;
                tls->write = tcp_write;
                ESP_LOGD(TAG, "non-tls connection established");
                return 1;
            }
            if (cfg->non_block) {
                FD_ZERO(&tls->rset);
                FD_SET(tls->sockfd, &tls->rset);
                tls->wset = tls->rset;
            }
            tls->conn_state = ESP_TLS_CONNECTING;
            /* falls through */
        case ESP_TLS_CONNECTING:
            if (cfg->non_block) {
                ESP_LOGD(TAG, "connecting...");
                struct timeval tv;
                ms_to_timeval(cfg->timeout_ms, &tv);

                /* In case of non-blocking I/O, we use the select() API to check whether
                   connection has been estbalished or not*/
                if (select(tls->sockfd + 1, &tls->rset, &tls->wset, NULL,
                    cfg->timeout_ms ? &tv : NULL) == 0) {
                    ESP_LOGD(TAG, "select() timed out");
                    return 0;
                }
                if (FD_ISSET(tls->sockfd, &tls->rset) || FD_ISSET(tls->sockfd, &tls->wset)) {
                    int error;
                    unsigned int len = sizeof(error);
                    /* pending error check */
                    if (getsockopt(tls->sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                        ESP_LOGD(TAG, "Non blocking connect failed");
                        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_SYSTEM, errno);
                        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED);
                        tls->conn_state = ESP_TLS_FAIL;
                        return -1;
                    }
                }
            }
            /* By now, the connection has been established */
            esp_ret = create_ssl_handle(hostname, hostlen, cfg, tls);
            if (esp_ret != ESP_OK) {
                ESP_LOGE(TAG, "create_ssl_handle failed");
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, esp_ret);
                tls->conn_state = ESP_TLS_FAIL;
                return -1;
            }
            tls->read = tls_read;
            tls->write = tls_write;
            tls->conn_state = ESP_TLS_HANDSHAKE;
            /* falls through */
        case ESP_TLS_HANDSHAKE:
            ESP_LOGD(TAG, "handshake in progress...");
            ret = mbedtls_ssl_handshake(&tls->ssl);
            if (ret == 0) {
                tls->conn_state = ESP_TLS_DONE;
                return 1;
            } else {
                if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                    ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
                    ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_MBEDTLS, -ret);
                    ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED);
                    if (cfg->cacert_buf != NULL || cfg->use_global_ca_store == true) {
                        /* This is to check whether handshake failed due to invalid certificate*/
                        verify_certificate(tls);
                    }
                    tls->conn_state = ESP_TLS_FAIL;
                    return -1;
                }
                /* Irrespective of blocking or non-blocking I/O, we return on getting MBEDTLS_ERR_SSL_WANT_READ
                   or MBEDTLS_ERR_SSL_WANT_WRITE during handshake */
                return 0;
            }
            break;
        case ESP_TLS_FAIL:
            ESP_LOGE(TAG, "failed to open a new connection");;
            break;
        default:
            ESP_LOGE(TAG, "invalid esp-tls state");
            break;
    }
    return -1;
}

/**
 * @brief      Create a new TLS/SSL connection
 */
esp_tls_t *esp_tls_conn_new(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg)
{
    esp_tls_t *tls = (esp_tls_t *)calloc(1, sizeof(esp_tls_t));
    if (!tls) {
        return NULL;
    }
    /* esp_tls_conn_new() API establishes connection in a blocking manner thus this loop ensures that esp_tls_conn_new()
       API returns only after connection is established unless there is an error*/
    while (1) {
        int ret = esp_tls_low_level_conn(hostname, hostlen, port, cfg, tls);
        if (ret == 1) {
            return tls;
        } else if (ret == -1) {
            esp_tls_conn_delete(tls);
            ESP_LOGE(TAG, "Failed to open new connection");
            return NULL;
        }
    }
    return NULL;
}

int esp_tls_conn_new_sync(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    /* esp_tls_conn_new_sync() is a sync alternative to esp_tls_conn_new_async() with symetric function prototype
    it is an alternative to esp_tls_conn_new() which is left for compatibility reasons */
    while (1) {
        int ret = esp_tls_low_level_conn(hostname, hostlen, port, cfg, tls);
        if (ret == 1) {
            return ret;
        } else if (ret == -1) {
            ESP_LOGE(TAG, "Failed to open new connection");
            return -1;
        }
    }
    return 0;
}

/*
 * @brief      Create a new TLS/SSL non-blocking connection
 */
int esp_tls_conn_new_async(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg , esp_tls_t *tls)
{
    return esp_tls_low_level_conn(hostname, hostlen, port, cfg, tls);
}

static int get_port(const char *url, struct http_parser_url *u)
{
    if (u->field_data[UF_PORT].len) {
        return strtol(&url[u->field_data[UF_PORT].off], NULL, 10);
    } else {
        if (strncasecmp(&url[u->field_data[UF_SCHEMA].off], "http", u->field_data[UF_SCHEMA].len) == 0) {
            return 80;
        } else if (strncasecmp(&url[u->field_data[UF_SCHEMA].off], "https", u->field_data[UF_SCHEMA].len) == 0) {
            return 443;
        }
    }
    return 0;
}

/**
 * @brief      Create a new TLS/SSL connection with a given "HTTP" url
 */
esp_tls_t *esp_tls_conn_http_new(const char *url, const esp_tls_cfg_t *cfg)
{
    /* Parse URI */
    struct http_parser_url u;
    http_parser_url_init(&u);
    http_parser_parse_url(url, strlen(url), 0, &u);
    esp_tls_t *tls = esp_tls_init();
    if (!tls) return NULL;
    /* Connect to host */
    if (esp_tls_conn_new_sync(&url[u.field_data[UF_HOST].off], u.field_data[UF_HOST].len,
                     get_port(url, &u), cfg, tls) == 1) {
        return tls;
    }
    return NULL;
}

ssize_t esp_tls_get_bytes_avail(esp_tls_t *tls)
{
    if (!tls) {
        ESP_LOGE(TAG, "empty arg passed to esp_tls_get_bytes_avail()");
        return -1;
    }
    return mbedtls_ssl_get_bytes_avail(&tls->ssl);
}

/**
 * @brief      Create a new non-blocking TLS/SSL connection with a given "HTTP" url
 */
int esp_tls_conn_http_new_async(const char *url, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    /* Parse URI */
    struct http_parser_url u;
    http_parser_url_init(&u);
    http_parser_parse_url(url, strlen(url), 0, &u);

    /* Connect to host */
    return esp_tls_conn_new_async(&url[u.field_data[UF_HOST].off], u.field_data[UF_HOST].len,
			    get_port(url, &u), cfg, tls);
}

#ifdef CONFIG_ESP_TLS_SERVER
/**
 * @brief      Create TLS/SSL server session
 */
int esp_tls_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls)
{
    if (tls == NULL || cfg == NULL) {
        return -1;
    }
    tls->role = ESP_TLS_SERVER;
    tls->sockfd = sockfd;
    esp_err_t esp_ret = create_ssl_handle(NULL, 0, cfg, tls);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "create_ssl_handle failed");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ERR_TYPE_ESP, esp_ret);
        tls->conn_state = ESP_TLS_FAIL;
        return -1;
    }
    tls->read = tls_read;
    tls->write = tls_write;
    int ret;
    while ((ret = mbedtls_ssl_handshake(&tls->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
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
void esp_tls_server_session_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        mbedtls_cleanup(tls);
        free(tls);
    }
};
#endif /* ! CONFIG_ESP_TLS_SERVER */

esp_tls_t *esp_tls_init(void)
{
    esp_tls_t *tls = (esp_tls_t *)calloc(1, sizeof(esp_tls_t));
    if (!tls) {
        return NULL;
    }
    tls->error_handle = calloc(1, sizeof(esp_tls_last_error_t));
    if (!tls->error_handle) {
        free(tls);
        return NULL;
    }
    tls->server_fd.fd = tls->sockfd = -1;
    return tls;
}

esp_err_t esp_tls_get_and_clear_last_error(esp_tls_error_handle_t h, int *mbedtls_code, int *mbedtls_flags)
{
    if (!h) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t last_err = h->last_error;
    if (mbedtls_code) {
        *mbedtls_code = h->mbedtls_error_code;
    }
    if (mbedtls_flags) {
        *mbedtls_flags = h->mbedtls_flags;
    }
    memset(h, 0, sizeof(esp_tls_last_error_t));
    return last_err;
}
