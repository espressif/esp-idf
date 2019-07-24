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
#include <errno.h>

static const char *TAG = "esp-tls";
static mbedtls_x509_crt *global_cacert = NULL;

#ifdef ESP_PLATFORM
#include <esp_log.h>
#else
#define ESP_LOGD(TAG, ...) //printf(__VA_ARGS__);
#define ESP_LOGE(TAG, ...) printf(__VA_ARGS__);
#endif

static struct addrinfo *resolve_host_name(const char *host, size_t hostlen)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char *use_host = strndup(host, hostlen);
    if (!use_host) {
        return NULL;
    }

    ESP_LOGD(TAG, "host:%s: strlen %lu", use_host, (unsigned long)hostlen);
    struct addrinfo *res;
    if (getaddrinfo(use_host, NULL, &hints, &res)) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s:", use_host);
        free(use_host);
        return NULL;
    }
    free(use_host);
    return res;
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

static int esp_tcp_connect(const char *host, int hostlen, int port, int *sockfd, const esp_tls_cfg_t *cfg)
{
    int ret = -1;
    struct addrinfo *res = resolve_host_name(host, hostlen);
    if (!res) {
        return ret;
    }

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0) {
        ESP_LOGE(TAG, "Failed to create socket (family %d socktype %d protocol %d)", res->ai_family, res->ai_socktype, res->ai_protocol);
        goto err_freeaddr;
    }
    *sockfd = fd;

    void *addr_ptr;
    if (res->ai_family == AF_INET) {
        struct sockaddr_in *p = (struct sockaddr_in *)res->ai_addr;
        p->sin_port = htons(port);
        addr_ptr = p;
    } else if (res->ai_family == AF_INET6) {
        struct sockaddr_in6 *p = (struct sockaddr_in6 *)res->ai_addr;
        p->sin6_port = htons(port);
        p->sin6_family = AF_INET6;
        addr_ptr = p;
    } else {
        ESP_LOGE(TAG, "Unsupported protocol family %d", res->ai_family);
        goto err_freesocket;
    }

    if (cfg) {
        if (cfg->timeout_ms >= 0) {
            struct timeval tv;
            ms_to_timeval(cfg->timeout_ms, &tv);
            setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
        }
        if (cfg->non_block) {
            int flags = fcntl(fd, F_GETFL, 0);
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }
    }

    ret = connect(fd, addr_ptr, res->ai_addrlen);
    if (ret < 0 && !(errno == EINPROGRESS && cfg->non_block)) {

        ESP_LOGE(TAG, "Failed to connnect to host (errno %d)", errno);
        goto err_freesocket;
    }

    freeaddrinfo(res);
    return 0;

err_freesocket:
    close(fd);
err_freeaddr:
    freeaddrinfo(res);
    return ret;
}

esp_err_t esp_tls_init_global_ca_store()
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
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        mbedtls_x509_crt_free(global_cacert);
        global_cacert = NULL;
        return ESP_FAIL;
    } else if (ret > 0) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse was partly successful. No. of failed certificates: %d", ret);
    }
    return ESP_OK;
}

mbedtls_x509_crt *esp_tls_get_global_ca_store()
{
    return global_cacert;
}

void esp_tls_free_global_ca_store()
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
    mbedtls_x509_crt_free(&tls->cacert);
    mbedtls_x509_crt_free(&tls->clientcert);
    mbedtls_pk_free(&tls->clientkey);
    mbedtls_entropy_free(&tls->entropy);
    mbedtls_ssl_config_free(&tls->conf);
    mbedtls_ctr_drbg_free(&tls->ctr_drbg);
    mbedtls_ssl_free(&tls->ssl);
    mbedtls_net_free(&tls->server_fd);
}

static int create_ssl_handle(esp_tls_t *tls, const char *hostname, size_t hostlen, const esp_tls_cfg_t *cfg)
{
    int ret;
    
    mbedtls_net_init(&tls->server_fd);
    tls->server_fd.fd = tls->sockfd;
    mbedtls_ssl_init(&tls->ssl);
    mbedtls_ctr_drbg_init(&tls->ctr_drbg);
    mbedtls_ssl_config_init(&tls->conf);
    mbedtls_entropy_init(&tls->entropy);
    
    if ((ret = mbedtls_ctr_drbg_seed(&tls->ctr_drbg, 
                    mbedtls_entropy_func, &tls->entropy, NULL, 0)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        goto exit;        
    }
    
    if (!cfg->skip_common_name) {
        char *use_host = NULL;
        if (cfg->common_name != NULL) {
            use_host = strndup(cfg->common_name, strlen(cfg->common_name));
        } else {
            use_host = strndup(hostname, hostlen);
        }

        if (use_host == NULL) {
            goto exit;
        }

        /* Hostname set here should match CN in server certificate */
        if ((ret = mbedtls_ssl_set_hostname(&tls->ssl, use_host)) != 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
            free(use_host);
            goto exit;
        }
        free(use_host);
    }

    if ((ret = mbedtls_ssl_config_defaults(&tls->conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

#ifdef CONFIG_MBEDTLS_SSL_ALPN
    if (cfg->alpn_protos) {
        mbedtls_ssl_conf_alpn_protocols(&tls->conf, cfg->alpn_protos);
    }
#endif

    if (cfg->use_global_ca_store == true) {
        if (global_cacert == NULL) {
            ESP_LOGE(TAG, "global_cacert is NULL");
            goto exit;
        }
        tls->cacert_ptr = global_cacert;
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
    } else if (cfg->cacert_pem_buf != NULL) {
        tls->cacert_ptr = &tls->cacert;
        mbedtls_x509_crt_init(tls->cacert_ptr);
        ret = mbedtls_x509_crt_parse(tls->cacert_ptr, cfg->cacert_pem_buf, cfg->cacert_pem_bytes);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
            goto exit;
        }
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
    } else {
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
    }

    if (cfg->clientcert_pem_buf != NULL && cfg->clientkey_pem_buf != NULL) {
        mbedtls_x509_crt_init(&tls->clientcert);
        mbedtls_pk_init(&tls->clientkey);

        ret = mbedtls_x509_crt_parse(&tls->clientcert, cfg->clientcert_pem_buf, cfg->clientcert_pem_bytes);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
            goto exit;
        }

        ret = mbedtls_pk_parse_key(&tls->clientkey, cfg->clientkey_pem_buf, cfg->clientkey_pem_bytes,
                  cfg->clientkey_password, cfg->clientkey_password_len);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_pk_parse_keyfile returned -0x%x\n\n", -ret);
            goto exit;
        }

        ret = mbedtls_ssl_conf_own_cert(&tls->conf, &tls->clientcert, &tls->clientkey);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_conf_own_cert returned -0x%x\n\n", -ret);
            goto exit;
        }
    } else if (cfg->clientcert_pem_buf != NULL || cfg->clientkey_pem_buf != NULL) {
        ESP_LOGE(TAG, "You have to provide both clientcert_pem_buf and clientkey_pem_buf for mutual authentication\n\n");
        goto exit;
    }

    mbedtls_ssl_conf_rng(&tls->conf, mbedtls_ctr_drbg_random, &tls->ctr_drbg);

#ifdef CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log(&tls->conf, 4);
#endif

    if ((ret = mbedtls_ssl_setup(&tls->ssl, &tls->conf)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x\n\n", -ret);
        goto exit;
    }
    mbedtls_ssl_set_bio(&tls->ssl, &tls->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    return 0;
exit:
    mbedtls_cleanup(tls);
    return -1;
}

/**
 * @brief      Close the TLS connection and free any allocated resources.
 */
void esp_tls_conn_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        mbedtls_cleanup(tls);
        if (tls->sockfd) {
            close(tls->sockfd);
        }
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
    /* These states are used to keep a tab on connection progress in case of non-blocking connect,
    and in case of blocking connect these cases will get executed one after the other */
    switch (tls->conn_state) {
        case ESP_TLS_INIT:
            ;
            int sockfd;
            int ret = esp_tcp_connect(hostname, hostlen, port, &sockfd, cfg);
            if (ret < 0) {
                return -1;
            }
            tls->sockfd = sockfd;
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
                        tls->conn_state = ESP_TLS_FAIL;
                        return -1;
                    }
                }
            }
            /* By now, the connection has been established */
            ret = create_ssl_handle(tls, hostname, hostlen, cfg);
            if (ret != 0) {
                ESP_LOGD(TAG, "create_ssl_handshake failed");
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
                    if (cfg->cacert_pem_buf != NULL || cfg->use_global_ca_store == true) {
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
    size_t start = xTaskGetTickCount();
    while (1) {
        int ret = esp_tls_low_level_conn(hostname, hostlen, port, cfg, tls);
        if (ret == 1) {
            return tls;
        } else if (ret == -1) {
            esp_tls_conn_delete(tls);
            ESP_LOGE(TAG, "Failed to open new connection");
            return NULL;
        } else if (ret == 0 && cfg->timeout_ms >= 0) {
            size_t timeout_ticks = pdMS_TO_TICKS(cfg->timeout_ms);
            uint32_t expired = xTaskGetTickCount() - start;
            if (expired >= timeout_ticks) {
                esp_tls_conn_delete(tls);
                ESP_LOGE(TAG, "Failed to open new connection in specified timeout");
                return NULL;
            }
        }
    }
    return NULL;
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

    /* Connect to host */
    return esp_tls_conn_new(&url[u.field_data[UF_HOST].off], u.field_data[UF_HOST].len,
			    get_port(url, &u), cfg);
}

size_t esp_tls_get_bytes_avail(esp_tls_t *tls)
{
    if (!tls) {
        ESP_LOGE(TAG, "empty arg passed to esp_tls_get_bytes_avail()");
        return ESP_FAIL;
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
