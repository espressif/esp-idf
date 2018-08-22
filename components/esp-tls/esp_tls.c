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

static const char *TAG = "esp-tls";

#ifdef ESP_PLATFORM
#include <esp_log.h>
#else
#define ESP_LOGD(TAG, ...) //printf(__VA_ARGS__);
#define ESP_LOGE(TAG, ...) printf(__VA_ARGS__);
#endif

#define DEFAULT_TIMEOUT_MS -1

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

static int esp_tcp_connect(const char *host, int hostlen, int port, int timeout_ms)
{
    struct addrinfo *res = resolve_host_name(host, hostlen);
    if (!res) {
        return -1;
    }

    int ret = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (ret < 0) {
        ESP_LOGE(TAG, "Failed to create socket (family %d socktype %d protocol %d)", res->ai_family, res->ai_socktype, res->ai_protocol);
        goto err_freeaddr;
    }
    int fd = ret;

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

    if (timeout_ms >= 0) {
        struct timeval tv;
        ms_to_timeval(timeout_ms, &tv);
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }

    ret = connect(fd, addr_ptr, res->ai_addrlen);
    if (ret < 0) {
        ESP_LOGE(TAG, "Failed to connnect to host (errno %d)", errno);
        goto err_freesocket;
    }

    freeaddrinfo(res);
    return fd;

err_freesocket:
    close(fd);
err_freeaddr:
    freeaddrinfo(res);
    return -1;
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
    mbedtls_x509_crt_free(&tls->cacert);
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
    
    /* Hostname set here should match CN in server certificate */    
    char *use_host = strndup(hostname, hostlen);
    if (!use_host) {
        goto exit;
    }

    if ((ret = mbedtls_ssl_set_hostname(&tls->ssl, use_host)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
        free(use_host);
        goto exit;
    }
    free(use_host);

    if ((ret = mbedtls_ssl_config_defaults(&tls->conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    if (cfg->alpn_protos) {
        mbedtls_ssl_conf_alpn_protocols(&tls->conf, cfg->alpn_protos);
    }

    if (cfg->cacert_pem_buf != NULL) {
        mbedtls_x509_crt_init(&tls->cacert);
        ret = mbedtls_x509_crt_parse(&tls->cacert, cfg->cacert_pem_buf, cfg->cacert_pem_bytes);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
            goto exit;
        }
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(&tls->conf, &tls->cacert, NULL);
    } else {
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
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

    while ((ret = mbedtls_ssl_handshake(&tls->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
            if (cfg->cacert_pem_buf != NULL) {
                /* This is to check whether handshake failed due to invalid certificate*/
                verify_certificate(tls);
            }   
            goto exit;
        }
    }
    
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

/**
 * @brief      Create a new TLS/SSL connection
 */
esp_tls_t *esp_tls_conn_new(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg)
{
    int sockfd;
    if (cfg) {
        sockfd = esp_tcp_connect(hostname, hostlen, port, cfg->timeout_ms);
    } else {
        sockfd = esp_tcp_connect(hostname, hostlen, port, DEFAULT_TIMEOUT_MS);
    }

    if (sockfd < 0) {
        return NULL;
    }

    esp_tls_t *tls = (esp_tls_t *)calloc(1, sizeof(esp_tls_t));
    if (!tls) {
        close(sockfd);
        return NULL;
    }
    tls->sockfd = sockfd;
    tls->read = tcp_read;
    tls->write = tcp_write;

    if (cfg) {
        if (create_ssl_handle(tls, hostname, hostlen, cfg) != 0) {
            esp_tls_conn_delete(tls);
            return NULL;
        }
    	tls->read = tls_read;
    	tls->write = tls_write;
        if (cfg->non_block == true) {
            int flags = fcntl(tls->sockfd, F_GETFL, 0);
            fcntl(tls->sockfd, F_SETFL, flags | O_NONBLOCK);
        }
    }

    return tls;
}

static int get_port(const char *url, struct http_parser_url *u)
{
    if (u->field_data[UF_PORT].len) {
        return strtol(&url[u->field_data[UF_PORT].off], NULL, 10);
    } else {
        if (strncmp(&url[u->field_data[UF_SCHEMA].off], "http", u->field_data[UF_SCHEMA].len) == 0) {
            return 80;
        } else if (strncmp(&url[u->field_data[UF_SCHEMA].off], "https", u->field_data[UF_SCHEMA].len) == 0) {
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