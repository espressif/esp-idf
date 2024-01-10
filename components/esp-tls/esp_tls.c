/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <http_parser.h>
#include "sdkconfig.h"
#include "esp_tls.h"
#include "esp_tls_private.h"
#include "esp_tls_error_capture_internal.h"
#include <fcntl.h>
#include <errno.h>

#if CONFIG_IDF_TARGET_LINUX && !ESP_TLS_WITH_LWIP
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>

#ifdef __linux__
#include <linux/if.h>
#endif

#ifdef __APPLE__
#include <net/if.h>
#endif

typedef struct in_addr ip_addr_t;
typedef struct in6_addr ip6_addr_t;
#define ipaddr_ntoa(ipaddr)     inet_ntoa(*ipaddr)

static inline char *ip6addr_ntoa(const ip6_addr_t *addr)
{
  static char str[40];
  return (char *)inet_ntop(AF_INET6, addr->s6_addr, str, 40);
}

#endif  // CONFIG_IDF_TARGET_LINUX && !ESP_TLS_WITH_LWIP

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
#define _esp_tls_net_init                   esp_mbedtls_net_init
#define _esp_tls_get_client_session         esp_mbedtls_get_client_session
#define _esp_tls_free_client_session        esp_mbedtls_free_client_session
#define _esp_tls_get_ssl_context            esp_mbedtls_get_ssl_context
#define _esp_tls_server_session_create      esp_mbedtls_server_session_create
#define _esp_tls_server_session_delete      esp_mbedtls_server_session_delete
#define _esp_tls_server_session_ticket_ctx_init    esp_mbedtls_server_session_ticket_ctx_init
#define _esp_tls_server_session_ticket_ctx_free    esp_mbedtls_server_session_ticket_ctx_free
#define _esp_tls_get_bytes_avail            esp_mbedtls_get_bytes_avail
#define _esp_tls_init_global_ca_store       esp_mbedtls_init_global_ca_store
#define _esp_tls_set_global_ca_store        esp_mbedtls_set_global_ca_store                 /*!< Callback function for setting global CA store data for TLS/SSL */
#define _esp_tls_get_global_ca_store        esp_mbedtls_get_global_ca_store
#define _esp_tls_free_global_ca_store       esp_mbedtls_free_global_ca_store                /*!< Callback function for freeing global ca store for TLS/SSL */
#define _esp_tls_get_ciphersuites_list      esp_mbedtls_get_ciphersuites_list
#elif CONFIG_ESP_TLS_USING_WOLFSSL /* CONFIG_ESP_TLS_USING_MBEDTLS */
#define _esp_create_ssl_handle              esp_create_wolfssl_handle
#define _esp_tls_handshake                  esp_wolfssl_handshake
#define _esp_tls_read                       esp_wolfssl_read
#define _esp_tls_write                      esp_wolfssl_write
#define _esp_tls_conn_delete                esp_wolfssl_conn_delete
#define _esp_tls_net_init                   esp_wolfssl_net_init
#define _esp_tls_server_session_create      esp_wolfssl_server_session_create
#define _esp_tls_server_session_delete      esp_wolfssl_server_session_delete
#define _esp_tls_get_bytes_avail            esp_wolfssl_get_bytes_avail
#define _esp_tls_init_global_ca_store       esp_wolfssl_init_global_ca_store
#define _esp_tls_set_global_ca_store        esp_wolfssl_set_global_ca_store                 /*!< Callback function for setting global CA store data for TLS/SSL */
#define _esp_tls_free_global_ca_store       esp_wolfssl_free_global_ca_store                /*!< Callback function for freeing global ca store for TLS/SSL */
#define _esp_tls_get_ssl_context            esp_wolfssl_get_ssl_context
#else   /* ESP_TLS_USING_WOLFSSL */
#error "No TLS stack configured"
#endif

#if CONFIG_IDF_TARGET_LINUX
#define IPV4_ENABLED    1
#define IPV6_ENABLED    1
#else   // CONFIG_IDF_TARGET_LINUX
#define IPV4_ENABLED    CONFIG_LWIP_IPV4
#define IPV6_ENABLED    CONFIG_LWIP_IPV6
#endif  // !CONFIG_IDF_TARGET_LINUX

#define ESP_TLS_DEFAULT_CONN_TIMEOUT  (10)  /*!< Default connection timeout in seconds */

static esp_err_t create_ssl_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls)
{
    return _esp_create_ssl_handle(hostname, hostlen, cfg, tls, NULL);
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

ssize_t esp_tls_conn_read(esp_tls_t *tls, void  *data, size_t datalen)
{
    if (!tls || !data) {
        return -1;
    }
    return tls->read(tls, (char *)data, datalen);
}

ssize_t esp_tls_conn_write(esp_tls_t *tls, const void  *data, size_t datalen)
{
    if (!tls || !data) {
        return -1;
    }
    return tls->write(tls, (char *)data, datalen);
}

/**
 * @brief      Close the TLS connection and free any allocated resources.
 */
int esp_tls_conn_destroy(esp_tls_t *tls)
{
    if (tls != NULL) {
        int ret = 0;
        _esp_tls_conn_delete(tls);
        if (tls->sockfd >= 0) {
            ret = close(tls->sockfd);
        }
        esp_tls_internal_event_tracker_destroy(tls->error_handle);
        free(tls);
        tls = NULL;
        return ret;
    }
    return -1; // invalid argument
}

esp_tls_t *esp_tls_init(void)
{
    esp_tls_t *tls = (esp_tls_t *)calloc(1, sizeof(esp_tls_t));
    if (!tls) {
        return NULL;
    }
    tls->error_handle = esp_tls_internal_event_tracker_create();
    if (!tls->error_handle) {
        free(tls);
        return NULL;
    }
    _esp_tls_net_init(tls);
    tls->sockfd = -1;
    return tls;
}

static esp_err_t esp_tls_hostname_to_fd(const char *host, size_t hostlen, int port, esp_tls_addr_family_t addr_family, struct sockaddr_storage *address, int* fd)
{
    struct addrinfo *address_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    switch (addr_family) {
        case ESP_TLS_AF_INET:
            hints.ai_family = AF_INET;
            break;
        case ESP_TLS_AF_INET6:
            hints.ai_family = AF_INET6;
            break;
        default:
            hints.ai_family = AF_UNSPEC;
            break;
    }

    hints.ai_socktype = SOCK_STREAM;

    char *use_host = strndup(host, hostlen);
    if (!use_host) {
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGD(TAG, "host:%s: strlen %lu", use_host, (unsigned long)hostlen);
    int res = getaddrinfo(use_host, NULL, &hints, &address_info);
    if (res != 0 || address_info == NULL) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s: "
                      "getaddrinfo() returns %d, addrinfo=%p", use_host, res, address_info);
        free(use_host);
        return ESP_ERR_ESP_TLS_CANNOT_RESOLVE_HOSTNAME;
    }
    free(use_host);
    *fd = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
    if (*fd < 0) {
        ESP_LOGE(TAG, "Failed to create socket (family %d socktype %d protocol %d)", address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
        freeaddrinfo(address_info);
        return ESP_ERR_ESP_TLS_CANNOT_CREATE_SOCKET;
    }

#if IPV4_ENABLED
    if (address_info->ai_family == AF_INET) {
        struct sockaddr_in *p = (struct sockaddr_in *)address_info->ai_addr;
        p->sin_port = htons(port);
        ESP_LOGD(TAG, "[sock=%d] Resolved IPv4 address: %s", *fd, ipaddr_ntoa((const ip_addr_t*)&p->sin_addr.s_addr));
        memcpy(address, p, sizeof(struct sockaddr ));
    }
#endif

#if IPV4_ENABLED && IPV6_ENABLED
    else
#endif

#if IPV6_ENABLED
    if (address_info->ai_family == AF_INET6) {
        struct sockaddr_in6 *p = (struct sockaddr_in6 *)address_info->ai_addr;
        p->sin6_port = htons(port);
        p->sin6_family = AF_INET6;
        ESP_LOGD(TAG, "[sock=%d] Resolved IPv6 address: %s", *fd, ip6addr_ntoa((const ip6_addr_t*)&p->sin6_addr));
        memcpy(address, p, sizeof(struct sockaddr_in6 ));
    }
#endif
    else {
        ESP_LOGE(TAG, "Unsupported protocol family %d", address_info->ai_family);
        close(*fd);
        freeaddrinfo(address_info);
        return ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY;
    }

    freeaddrinfo(address_info);
    return ESP_OK;
}

static void ms_to_timeval(int timeout_ms, struct timeval *tv)
{
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms % 1000) * 1000;
}

static esp_err_t esp_tls_set_socket_options(int fd, const esp_tls_cfg_t *cfg)
{
    if (cfg) {
        struct timeval tv = {};
        if (cfg->timeout_ms > 0) {
            ms_to_timeval(cfg->timeout_ms, &tv);
        } else {
            tv.tv_sec = ESP_TLS_DEFAULT_CONN_TIMEOUT;
            tv.tv_usec = 0;
        }
        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0) {
            ESP_LOGE(TAG, "Fail to setsockopt SO_RCVTIMEO");
            return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
        }
        if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) != 0) {
            ESP_LOGE(TAG, "Fail to setsockopt SO_SNDTIMEO");
            return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
        }

        if (cfg->keep_alive_cfg && cfg->keep_alive_cfg->keep_alive_enable) {
            int keep_alive_enable = 1;
            int keep_alive_idle = cfg->keep_alive_cfg->keep_alive_idle;
            int keep_alive_interval = cfg->keep_alive_cfg->keep_alive_interval;
            int keep_alive_count = cfg->keep_alive_cfg->keep_alive_count;

            ESP_LOGD(TAG, "Enable TCP keep alive. idle: %d, interval: %d, count: %d", keep_alive_idle, keep_alive_interval, keep_alive_count);
            if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive_enable, sizeof(keep_alive_enable)) != 0) {
                ESP_LOGE(TAG, "Fail to setsockopt SO_KEEPALIVE");
                return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
            }
#ifndef __APPLE__
            if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keep_alive_idle, sizeof(keep_alive_idle)) != 0) {
                ESP_LOGE(TAG, "Fail to setsockopt TCP_KEEPIDLE");
                return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
            }
            if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keep_alive_interval, sizeof(keep_alive_interval)) != 0) {
                ESP_LOGE(TAG, "Fail to setsockopt TCP_KEEPINTVL");
                return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
            }
            if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keep_alive_count, sizeof(keep_alive_count)) != 0) {
                ESP_LOGE(TAG, "Fail to setsockopt TCP_KEEPCNT");
                return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
            }
#else // __APPLE__
            if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &keep_alive_idle, sizeof(keep_alive_idle)) != 0) {
                ESP_LOGE(TAG, "Fail to setsockopt TCP_KEEPALIVE");
                return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
            }
#endif // __APPLE__
        }
        if (cfg->if_name) {
            if (cfg->if_name->ifr_name[0] != 0) {
                ESP_LOGD(TAG, "Bind [sock=%d] to interface %s", fd, cfg->if_name->ifr_name);
#ifndef __APPLE__
                if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE,  cfg->if_name, sizeof(struct ifreq)) != 0) {
#else
                int idx = if_nametoindex(cfg->if_name->ifr_name);
                if (setsockopt(fd, IPPROTO_IP, IP_BOUND_IF, &idx, sizeof(idx)) != 0) {
#endif
                    ESP_LOGE(TAG, "Bind [sock=%d] to interface %s fail", fd, cfg->if_name->ifr_name);
                    return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
                }
            }
        }
    }
    return ESP_OK;
}

static esp_err_t esp_tls_set_socket_non_blocking(int fd, bool non_blocking)
{
    int flags;
    if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
        ESP_LOGE(TAG, "[sock=%d] get file flags error: %s", fd, strerror(errno));
        return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
    }

    if (non_blocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    if (fcntl(fd, F_SETFL, flags) < 0) {
        ESP_LOGE(TAG, "[sock=%d] set blocking/nonblocking error: %s", fd, strerror(errno));
        return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
    }
    return ESP_OK;
}

static inline esp_err_t tcp_connect(const char *host, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_error_handle_t error_handle, int *sockfd)
{
    struct sockaddr_storage address;
    int fd;

    esp_tls_addr_family_t addr_family = (cfg != NULL) ? cfg->addr_family : ESP_TLS_AF_UNSPEC;
    esp_err_t ret = esp_tls_hostname_to_fd(host, hostlen, port, addr_family, &address, &fd);
    if (ret != ESP_OK) {
        ESP_INT_EVENT_TRACKER_CAPTURE(error_handle, ESP_TLS_ERR_TYPE_SYSTEM, errno);
        return ret;
    }

    // Set timeout options, keep-alive options and bind device options if configured
    ret = esp_tls_set_socket_options(fd, cfg);
    if (ret != ESP_OK) {
        goto err;
    }

    // Set to non block before connecting to better control connection timeout
    ret = esp_tls_set_socket_non_blocking(fd, true);
    if (ret != ESP_OK) {
        goto err;
    }

    ret = ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST;
    ESP_LOGD(TAG, "[sock=%d] Connecting to server. HOST: %s, Port: %d", fd, host, port);
    if (connect(fd, (struct sockaddr *)&address, sizeof(struct sockaddr)) < 0) {
        if (errno == EINPROGRESS) {
            fd_set fdset;
            struct timeval tv = { .tv_usec = 0, .tv_sec = ESP_TLS_DEFAULT_CONN_TIMEOUT }; // Default connection timeout is 10 s

            if (cfg && cfg->non_block) {
                // Non-blocking mode -> just return successfully at this stage
                *sockfd = fd;
                return ESP_OK;
            }

            if ( cfg && cfg->timeout_ms > 0 ) {
                ms_to_timeval(cfg->timeout_ms, &tv);
            }
            FD_ZERO(&fdset);
            FD_SET(fd, &fdset);

            int res = select(fd+1, NULL, &fdset, NULL, &tv);
            if (res < 0) {
                ESP_LOGE(TAG, "[sock=%d] select() error: %s", fd, strerror(errno));
                ESP_INT_EVENT_TRACKER_CAPTURE(error_handle, ESP_TLS_ERR_TYPE_SYSTEM, errno);
                goto err;
            }
            else if (res == 0) {
                ESP_LOGE(TAG, "[sock=%d] select() timeout", fd);
                ret = ESP_ERR_ESP_TLS_CONNECTION_TIMEOUT;
                goto err;
            } else {
                int sockerr;
                socklen_t len = (socklen_t)sizeof(int);

                if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)(&sockerr), &len) < 0) {
                    ESP_LOGE(TAG, "[sock=%d] getsockopt() error: %s", fd, strerror(errno));
                    ret = ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
                    goto err;
                }
                else if (sockerr) {
                    ESP_INT_EVENT_TRACKER_CAPTURE(error_handle, ESP_TLS_ERR_TYPE_SYSTEM, sockerr);
                    ESP_LOGE(TAG, "[sock=%d] delayed connect error: %s", fd, strerror(sockerr));
                    goto err;
                }
            }
        } else {
            ESP_LOGE(TAG, "[sock=%d] connect() error: %s", fd, strerror(errno));
            goto err;
        }
    }

    if (cfg && cfg->non_block == false) {
        // reset back to blocking mode (unless non_block configured)
        ret = esp_tls_set_socket_non_blocking(fd, false);
        if (ret != ESP_OK) {
            goto err;
        }
    }

    *sockfd = fd;
    return ESP_OK;

err:
    close(fd);
    return ret;
}

static int esp_tls_low_level_conn(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{

    esp_err_t esp_ret;
    /* These states are used to keep a tab on connection progress in case of non-blocking connect,
    and in case of blocking connect these cases will get executed one after the other */
    switch (tls->conn_state) {
    case ESP_TLS_INIT:
        tls->sockfd = -1;
        if (cfg != NULL && cfg->is_plain_tcp == false) {
            _esp_tls_net_init(tls);
            tls->is_tls = true;
        }
        if ((esp_ret = tcp_connect(hostname, hostlen, port, cfg, tls->error_handle, &tls->sockfd)) != ESP_OK) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, esp_ret);
            return -1;
        }
        if (tls->is_tls == false) {
            tls->read = tcp_read;
            tls->write = tcp_write;
            ESP_LOGD(TAG, "non-tls connection established");
            return 1;
        }
        if (cfg && cfg->non_block) {
            FD_ZERO(&tls->rset);
            FD_SET(tls->sockfd, &tls->rset);
            tls->wset = tls->rset;
        }
        tls->conn_state = ESP_TLS_CONNECTING;
    /* falls through */
    case ESP_TLS_CONNECTING:
        if (cfg && cfg->non_block) {
            ESP_LOGD(TAG, "connecting...");
            struct timeval tv;
            ms_to_timeval(cfg->timeout_ms, &tv);

            /* In case of non-blocking I/O, we use the select() API to check whether
               connection has been established or not*/
            if (select(tls->sockfd + 1, &tls->rset, &tls->wset, NULL,
                       cfg->timeout_ms>0 ? &tv : NULL) == 0) {
                ESP_LOGD(TAG, "select() timed out");
                return 0;
            }
            if (FD_ISSET(tls->sockfd, &tls->rset) || FD_ISSET(tls->sockfd, &tls->wset)) {
                int error;
                socklen_t len = sizeof(error);
                /* pending error check */
                if (getsockopt(tls->sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                    ESP_LOGD(TAG, "Non blocking connect failed");
                    ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_SYSTEM, errno);
                    ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED);
                    tls->conn_state = ESP_TLS_FAIL;
                    return -1;
                }
            }
        }
        /* By now, the connection has been established */
        esp_ret = create_ssl_handle(hostname, hostlen, cfg, tls);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "create_ssl_handle failed");
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, esp_ret);
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
 * @brief Create a new plain TCP connection
 */
esp_err_t esp_tls_plain_tcp_connect(const char *host, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_error_handle_t error_handle, int *sockfd)
{
    if (sockfd == NULL || error_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return tcp_connect(host, hostlen, port, cfg, error_handle, sockfd);
}

int esp_tls_conn_new_sync(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    if (!cfg || !tls || !hostname || hostlen < 0) {
        return -1;
    }
    struct timeval time = {};
    gettimeofday(&time, NULL);
    uint32_t start_time_ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    while (1) {
        int ret = esp_tls_low_level_conn(hostname, hostlen, port, cfg, tls);
        if (ret == 1) {
            return ret;
        } else if (ret == -1) {
            ESP_LOGE(TAG, "Failed to open new connection");
            return -1;
        } else if (ret == 0 && cfg->timeout_ms >= 0) {
            gettimeofday(&time, NULL);
            uint32_t current_time_ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
            uint32_t elapsed_time_ms = current_time_ms - start_time_ms;
            if (elapsed_time_ms >= cfg->timeout_ms) {
                ESP_LOGW(TAG, "Failed to open new connection in specified timeout");
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, ESP_ERR_ESP_TLS_CONNECTION_TIMEOUT);
                return 0;
            }
        }
    }
    return 0;
}

/*
 * @brief      Create a new TLS/SSL non-blocking connection
 */
int esp_tls_conn_new_async(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    if (!cfg || !tls || !hostname || hostlen < 0) {
        return -1;
    }
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

esp_tls_t *esp_tls_conn_http_new(const char *url, const esp_tls_cfg_t *cfg)
{
    if (!url || !cfg) {
        return NULL;
    }

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
    esp_tls_conn_destroy(tls);
    return NULL;
}

/**
 * @brief      Create a new TLS/SSL connection with a given "HTTP" url
 */
int esp_tls_conn_http_new_sync(const char *url, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    if (!url || !cfg || !tls) {
        return -1;
    }

    /* Parse URI */
    struct http_parser_url u;
    http_parser_url_init(&u);
    http_parser_parse_url(url, strlen(url), 0, &u);

    /* Connect to host */
    return esp_tls_conn_new_sync(&url[u.field_data[UF_HOST].off], u.field_data[UF_HOST].len,
                                  get_port(url, &u), cfg, tls);
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

const int *esp_tls_get_ciphersuites_list(void)
{
    return _esp_tls_get_ciphersuites_list();
}
#endif /* CONFIG_ESP_TLS_USING_MBEDTLS */

#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
esp_tls_client_session_t *esp_tls_get_client_session(esp_tls_t *tls)
{
    return _esp_tls_get_client_session(tls);
}

void esp_tls_free_client_session(esp_tls_client_session_t *client_session)
{
    _esp_tls_free_client_session(client_session);
}
#endif /* CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS */


esp_err_t esp_tls_cfg_server_session_tickets_init(esp_tls_cfg_server_t *cfg)
{
#if defined(CONFIG_ESP_TLS_SERVER_SESSION_TICKETS)
    if (!cfg || cfg->ticket_ctx) {
        return ESP_ERR_INVALID_ARG;
    }
    cfg->ticket_ctx = calloc(1, sizeof(esp_tls_server_session_ticket_ctx_t));
    if (!cfg->ticket_ctx) {
        return ESP_ERR_NO_MEM;
    }
    esp_err_t ret =  _esp_tls_server_session_ticket_ctx_init(cfg->ticket_ctx);
    if (ret != ESP_OK) {
        free(cfg->ticket_ctx);
    }
    return ret;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

void esp_tls_cfg_server_session_tickets_free(esp_tls_cfg_server_t *cfg)
{
#if defined(CONFIG_ESP_TLS_SERVER_SESSION_TICKETS)
    if (cfg && cfg->ticket_ctx) {
        _esp_tls_server_session_ticket_ctx_free(cfg->ticket_ctx);
    }
#endif
}

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

ssize_t esp_tls_get_bytes_avail(esp_tls_t *tls)
{
    return _esp_tls_get_bytes_avail(tls);
}

void *esp_tls_get_ssl_context(esp_tls_t *tls)
{
    return _esp_tls_get_ssl_context(tls);
}

esp_err_t esp_tls_get_conn_sockfd(esp_tls_t *tls, int *sockfd)
{
    if (!tls || !sockfd) {
        ESP_LOGE(TAG, "Invalid arguments passed");
        return ESP_ERR_INVALID_ARG;
    }
    *sockfd = tls->sockfd;
    return ESP_OK;
}

esp_err_t esp_tls_set_conn_sockfd(esp_tls_t *tls, int sockfd)
{
    if (!tls || sockfd < 0) {
        ESP_LOGE(TAG, "Invalid arguments passed");
        return ESP_ERR_INVALID_ARG;
    }
    tls->sockfd = sockfd;
    return ESP_OK;
}

esp_err_t esp_tls_get_conn_state(esp_tls_t *tls, esp_tls_conn_state_t *conn_state)
{
    if (!tls || !conn_state) {
        ESP_LOGE(TAG, "Invalid arguments passed");
        return ESP_ERR_INVALID_ARG;
    }
    *conn_state = tls->conn_state;
    return ESP_OK;
}

esp_err_t esp_tls_set_conn_state(esp_tls_t *tls, esp_tls_conn_state_t conn_state)
{
    if (!tls || conn_state < ESP_TLS_INIT || conn_state > ESP_TLS_DONE) {
        ESP_LOGE(TAG, "Invalid arguments passed");
        return ESP_ERR_INVALID_ARG;
    }
    tls->conn_state = conn_state;
    return ESP_OK;
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

esp_err_t esp_tls_get_error_handle(esp_tls_t *tls, esp_tls_error_handle_t *error_handle)
{
    if (!tls || !error_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    *error_handle = tls->error_handle;
    return ESP_OK;
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
