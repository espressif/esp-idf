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
#include "esp_tls.h"
#include "esp_tls_error_capture_internal.h"
#include <errno.h>
static const char *TAG = "esp-tls";

#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#include "esp_tls_mbedtls.h"
#elif CONFIG_ESP_TLS_USING_WOLFSSL
#include "esp_tls_wolfssl.h"
#endif

#ifdef ESP_PLATFORM
#include <esp_log.h>
#else
#define ESP_LOGD(TAG, ...) //printf(__VA_ARGS__);
#define ESP_LOGE(TAG, ...) printf(__VA_ARGS__);
#endif

#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#define _esp_create_ssl_handle              esp_create_mbedtls_handle
#define _esp_tls_handshake                  esp_mbedtls_handshake
#define _esp_tls_read                       esp_mbedtls_read
#define _esp_tls_write                      esp_mbedtls_write
#define _esp_tls_conn_delete                esp_mbedtls_conn_delete
#ifdef CONFIG_ESP_TLS_SERVER
#define _esp_tls_server_session_create      esp_mbedtls_server_session_create
#define _esp_tls_server_session_delete      esp_mbedtls_server_session_delete
#endif  /* CONFIG_ESP_TLS_SERVER */
#define _esp_tls_get_bytes_avail            esp_mbedtls_get_bytes_avail
#define _esp_tls_init_global_ca_store       esp_mbedtls_init_global_ca_store
#define _esp_tls_set_global_ca_store        esp_mbedtls_set_global_ca_store                 /*!< Callback function for setting global CA store data for TLS/SSL */
#define _esp_tls_get_global_ca_store        esp_mbedtls_get_global_ca_store
#define _esp_tls_free_global_ca_store       esp_mbedtls_free_global_ca_store                /*!< Callback function for freeing global ca store for TLS/SSL */
#elif CONFIG_ESP_TLS_USING_WOLFSSL /* CONFIG_ESP_TLS_USING_MBEDTLS */
#define _esp_create_ssl_handle              esp_create_wolfssl_handle
#define _esp_tls_handshake                  esp_wolfssl_handshake
#define _esp_tls_read                       esp_wolfssl_read
#define _esp_tls_write                      esp_wolfssl_write
#define _esp_tls_conn_delete                esp_wolfssl_conn_delete
#define _esp_tls_get_bytes_avail            esp_wolfssl_get_bytes_avail
#define _esp_tls_init_global_ca_store       esp_wolfssl_init_global_ca_store
#define _esp_tls_set_global_ca_store        esp_wolfssl_set_global_ca_store                 /*!< Callback function for setting global CA store data for TLS/SSL */
#define _esp_tls_free_global_ca_store       esp_wolfssl_free_global_ca_store                /*!< Callback function for freeing global ca store for TLS/SSL */
#else   /* ESP_TLS_USING_WOLFSSL */
#error "No TLS stack configured"
#endif

static esp_err_t create_ssl_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls)
{
    return _esp_create_ssl_handle(hostname, hostlen, cfg, tls);
}

static esp_err_t esp_tls_handshake(esp_tls_t *tls, const esp_tls_cfg_t *cfg)
{
    return _esp_tls_handshake(tls, cfg);
}

static ssize_t tcp_read(esp_tls_t *tls, char *data, size_t datalen)
{
    return recv(tls->sockfd, data, datalen, 0);
}

static ssize_t tcp_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    return send(tls->sockfd, data, datalen, 0);
}

/**
 * @brief      Close the TLS connection and free any allocated resources.
 */
void esp_tls_conn_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        _esp_tls_conn_delete(tls);
        if (tls->sockfd >= 0) {
            close(tls->sockfd);
        }
    free(tls->error_handle);
    free(tls);
    }
}

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
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
    tls->server_fd.fd = tls->sockfd = -1;
#endif
    return tls;
}

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
            setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
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

static int esp_tls_low_level_conn(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    if (!tls) {
        ESP_LOGE(TAG, "empty esp_tls parameter");
        return -1;
    }
    esp_err_t esp_ret;
    /* These states are used to keep a tab on connection progress in case of non-blocking connect,
    and in case of blocking connect these cases will get executed one after the other */
    switch (tls->conn_state) {
    case ESP_TLS_INIT:
        tls->sockfd = -1;
        if (cfg != NULL) {
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
            mbedtls_net_init(&tls->server_fd);
#endif
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
        tls->read = _esp_tls_read;
        tls->write = _esp_tls_write;
        tls->conn_state = ESP_TLS_HANDSHAKE;
    /* falls through */
    case ESP_TLS_HANDSHAKE:
        ESP_LOGD(TAG, "handshake in progress...");
        return esp_tls_handshake(tls, cfg);
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
int esp_tls_conn_new_async(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
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
    if (!tls) {
        return NULL;
    }
    /* Connect to host */
    if (esp_tls_conn_new_sync(&url[u.field_data[UF_HOST].off], u.field_data[UF_HOST].len,
                              get_port(url, &u), cfg, tls) == 1) {
        return tls;
    }
    return NULL;
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

#ifdef CONFIG_ESP_TLS_USING_MBEDTLS

mbedtls_x509_crt *esp_tls_get_global_ca_store(void)
{
    return _esp_tls_get_global_ca_store();
}

#ifdef CONFIG_ESP_TLS_SERVER
/**
 * @brief      Create a server side TLS/SSL connection
 */
int esp_tls_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls)
{
    return _esp_tls_server_session_create(cfg, sockfd, tls);
}
/**
 * @brief      Close the server side TLS/SSL connection and free any allocated resources.
 */
void esp_tls_server_session_delete(esp_tls_t *tls)
{
    return _esp_tls_server_session_delete(tls);
}
#endif /* CONFIG_ESP_TLS_SERVER */
#endif /* CONFIG_ESP_TLS_USING_MBEDTLS */

ssize_t esp_tls_get_bytes_avail(esp_tls_t *tls)
{
    return _esp_tls_get_bytes_avail(tls);
}

esp_err_t esp_tls_get_and_clear_last_error(esp_tls_error_handle_t h, int *esp_tls_code, int *esp_tls_flags)
{
    if (!h) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t last_err = h->last_error;
    if (esp_tls_code) {
        *esp_tls_code = h->esp_tls_error_code;
    }
    if (esp_tls_flags) {
        *esp_tls_flags = h->esp_tls_flags;
    }
    memset(h, 0, sizeof(esp_tls_last_error_t));
    return last_err;
}

esp_err_t esp_tls_init_global_ca_store(void)
{
    return _esp_tls_init_global_ca_store();
}

esp_err_t esp_tls_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes)
{
    return _esp_tls_set_global_ca_store(cacert_pem_buf, cacert_pem_bytes);
}

void esp_tls_free_global_ca_store(void)
{
    return _esp_tls_free_global_ca_store();
}
