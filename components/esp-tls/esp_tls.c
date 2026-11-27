/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
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
#include "esp_tls_platform_port.h"
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
#elif CONFIG_ESP_TLS_CUSTOM_STACK
#include "esp_tls_custom_stack.h"
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
#define _esp_tls_server_session_init        esp_mbedtls_server_session_init
#define _esp_tls_server_session_continue_async     esp_mbedtls_server_session_continue_async
#define _esp_tls_server_session_delete      esp_mbedtls_server_session_delete
#define _esp_tls_server_session_ticket_ctx_init    esp_mbedtls_server_session_ticket_ctx_init
#define _esp_tls_server_session_ticket_ctx_free    esp_mbedtls_server_session_ticket_ctx_free
#define _esp_tls_get_bytes_avail            esp_mbedtls_get_bytes_avail
#define _esp_tls_init_global_ca_store       esp_mbedtls_init_global_ca_store
#define _esp_tls_set_global_ca_store        esp_mbedtls_set_global_ca_store
#define _esp_tls_get_global_ca_store        esp_mbedtls_get_global_ca_store
#define _esp_tls_free_global_ca_store       esp_mbedtls_free_global_ca_store
#define _esp_tls_get_ciphersuites_list      esp_mbedtls_get_ciphersuites_list
#define _esp_tls_get_supported_groups_list  esp_mbedtls_get_supported_groups_list
#elif CONFIG_ESP_TLS_CUSTOM_STACK
#define _esp_create_ssl_handle              esp_tls_custom_stack_create_ssl_handle
#define _esp_tls_handshake                  esp_tls_custom_stack_handshake
#define _esp_tls_read                       esp_tls_custom_stack_read
#define _esp_tls_write                      esp_tls_custom_stack_write
#define _esp_tls_conn_delete                esp_tls_custom_stack_conn_delete
#define _esp_tls_net_init                   esp_tls_custom_stack_net_init
#define _esp_tls_get_client_session         esp_tls_custom_stack_get_client_session
#define _esp_tls_free_client_session        esp_tls_custom_stack_free_client_session
#define _esp_tls_get_ssl_context            esp_tls_custom_stack_get_ssl_context
#define _esp_tls_server_session_create      esp_tls_custom_stack_server_session_create
#define _esp_tls_server_session_init        esp_tls_custom_stack_server_session_init
#define _esp_tls_server_session_continue_async     esp_tls_custom_stack_server_session_continue_async
#define _esp_tls_server_session_delete      esp_tls_custom_stack_server_session_delete
#define _esp_tls_server_session_ticket_ctx_init    esp_tls_custom_stack_server_session_ticket_ctx_init
#define _esp_tls_server_session_ticket_ctx_free    esp_tls_custom_stack_server_session_ticket_ctx_free
#define _esp_tls_get_bytes_avail            esp_tls_custom_stack_get_bytes_avail
#define _esp_tls_init_global_ca_store       esp_tls_custom_stack_init_global_ca_store
#define _esp_tls_set_global_ca_store        esp_tls_custom_stack_set_global_ca_store
#define _esp_tls_get_global_ca_store        esp_tls_custom_stack_get_global_ca_store
#define _esp_tls_free_global_ca_store       esp_tls_custom_stack_free_global_ca_store
#define _esp_tls_get_ciphersuites_list      esp_tls_custom_stack_get_ciphersuites_list
#define _esp_tls_get_supported_groups_list  esp_tls_custom_stack_get_supported_groups_list
#else
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
    if (!tls) {
        return -1;
    }
    if (!tls->read) {
        return -1;
    }
#if CONFIG_MBEDTLS_DYNAMIC_BUFFER
    if (tls->is_tls && tls->conn_state != ESP_TLS_DONE) {
        ESP_LOGE(TAG, "TLS handshake has not completed, read operation not permitted");
        return -1;
    }
#endif
    return tls->read(tls, (char *)data, datalen);
}

ssize_t esp_tls_conn_write(esp_tls_t *tls, const void  *data, size_t datalen)
{
    if (!tls || !data) {
        return -1;
    }
    if (!tls->write) {
        return -1;
    }
#if CONFIG_MBEDTLS_DYNAMIC_BUFFER
    if (tls->is_tls && tls->conn_state != ESP_TLS_DONE) {
        ESP_LOGE(TAG, "TLS handshake has not completed, write operation not permitted");
        return -1;
    }
#endif
    return tls->write(tls, (char *)data, datalen);
}

/**
 * @brief      Close the TLS connection and free any allocated resources.
 */
int esp_tls_conn_destroy(esp_tls_t *tls)
{
    if (tls != NULL) {
        int ret = 0;
        if (tls->addrinfo != NULL) {
            freeaddrinfo(tls->addrinfo);
            tls->addrinfo = NULL;
            tls->next_addrinfo = NULL;
        }
        _esp_tls_conn_delete(tls);
        if (tls->sockfd >= 0) {
            ret = close(tls->sockfd);
        }
        esp_tls_internal_event_tracker_destroy(tls->error_handle);
#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
        if (tls->client_session) {
            mbedtls_platform_zeroize(tls->client_session, tls->client_session_len);
            free(tls->client_session);
            tls->client_session = NULL;
            tls->client_session_len = 0;
        }
#endif // CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
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
#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    tls->client_session = NULL;
    tls->client_session_len = 0;
#endif // CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    return tls;
}

static esp_err_t esp_tls_resolve_hostname(const char *host, size_t hostlen,
                                          esp_tls_addr_family_t addr_family,
                                          struct addrinfo **address_info)
{
    if (host == NULL || address_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *address_info = NULL;
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
    int res = getaddrinfo(use_host, NULL, &hints, address_info);
    if (res != 0 || *address_info == NULL) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s: "
                      "getaddrinfo() returns %d, addrinfo=%p", use_host, res, *address_info);
        if (*address_info != NULL) {
            freeaddrinfo(*address_info);
            *address_info = NULL;
        }
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

#if CONFIG_ESP_TLS_ENABLE_TCP_NODELAY
        /* Disable Nagle's algorithm. Small control-plane writes (e.g. MQTT PUBLISH/PUBACK/PINGREQ)
         * otherwise interact with the peer's delayed-ACK and stall ~40-200 ms until its timer fires.
         * Non-fatal: a latency hint, not required for a correct connection (unlike the timeouts). */
        int nodelay = 1;
        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) != 0) {
            ESP_LOGW(TAG, "Fail to setsockopt TCP_NODELAY (non-fatal)");
        }
#endif

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

static esp_err_t esp_tls_make_sockaddr(const struct addrinfo *candidate, int port,
                                       struct sockaddr_storage *address, socklen_t *addr_len)
{
    if (candidate->ai_addr == NULL || candidate->ai_addrlen > sizeof(*address)) {
        return ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY;
    }

    memset(address, 0, sizeof(*address));
    memcpy(address, candidate->ai_addr, candidate->ai_addrlen);

#if IPV4_ENABLED
    if (candidate->ai_family == AF_INET && candidate->ai_addrlen >= sizeof(struct sockaddr_in)) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)address;
        ipv4->sin_port = htons(port);
        *addr_len = sizeof(*ipv4);
        ESP_LOGD(TAG, "Resolved IPv4 address: %s", ipaddr_ntoa((const ip_addr_t *)&ipv4->sin_addr.s_addr));
        return ESP_OK;
    }
#endif

#if IPV6_ENABLED
    if (candidate->ai_family == AF_INET6 && candidate->ai_addrlen >= sizeof(struct sockaddr_in6)) {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)address;
        ipv6->sin6_port = htons(port);
        *addr_len = sizeof(*ipv6);
        ESP_LOGD(TAG, "Resolved IPv6 address: %s", ip6addr_ntoa((const ip6_addr_t *)&ipv6->sin6_addr));
        return ESP_OK;
    }
#endif

    return ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY;
}

static int esp_tls_connect_timeout_ms(const esp_tls_cfg_t *cfg)
{
    return cfg != NULL && cfg->timeout_ms > 0 ? cfg->timeout_ms : ESP_TLS_DEFAULT_CONN_TIMEOUT * 1000;
}

static size_t esp_tls_addrinfo_count(const struct addrinfo *address_info)
{
    size_t count = 0;
    for (const struct addrinfo *candidate = address_info; candidate != NULL; candidate = candidate->ai_next) {
        count++;
    }
    return count;
}

static int esp_tls_candidate_timeout_ms(uint64_t start_time_us, int total_timeout_ms,
                                        size_t candidates_left)
{
    uint64_t total_timeout_us = (uint64_t)total_timeout_ms * 1000;
    uint64_t elapsed_us = esp_tls_get_platform_time() - start_time_us;
    if (elapsed_us >= total_timeout_us || candidates_left == 0) {
        return 0;
    }

    uint64_t remaining_us = total_timeout_us - elapsed_us;
    uint64_t timeout_us = remaining_us / candidates_left;
    if (timeout_us == 0) {
        return 1;
    }
    return (int)((timeout_us + 999) / 1000);
}

static esp_err_t esp_tls_connect_one_address(const struct addrinfo *candidate, int port,
                                             const esp_tls_cfg_t *cfg, bool wait_for_connection,
                                             int timeout_ms,
                                             int *sockfd, bool *pending, int *connect_errno)
{
    if (candidate == NULL || sockfd == NULL || pending == NULL || connect_errno == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *sockfd = -1;
    *pending = false;
    *connect_errno = 0;

    struct sockaddr_storage address;
    socklen_t addr_len = 0;
    esp_err_t ret = esp_tls_make_sockaddr(candidate, port, &address, &addr_len);
    if (ret != ESP_OK) {
        return ret;
    }

    int fd = socket(candidate->ai_family, candidate->ai_socktype, candidate->ai_protocol);
    if (fd < 0) {
        *connect_errno = errno;
        return ESP_ERR_ESP_TLS_CANNOT_CREATE_SOCKET;
    }

    ret = esp_tls_set_socket_options(fd, cfg);
    if (ret != ESP_OK) {
        *connect_errno = errno;
        close(fd);
        return ret;
    }

    ret = esp_tls_set_socket_non_blocking(fd, true);
    if (ret != ESP_OK) {
        *connect_errno = errno;
        close(fd);
        return ret;
    }

    ESP_LOGD(TAG, "[sock=%d] Connecting to address family %d, Port: %d", fd, candidate->ai_family, port);
    if (connect(fd, (struct sockaddr *)&address, addr_len) < 0) {
        int err = errno;
        if (err != EINPROGRESS && err != EWOULDBLOCK && err != EAGAIN) {
            ESP_LOGD(TAG, "[sock=%d] connect() error: %s", fd, strerror(err));
            *connect_errno = err;
            close(fd);
            return ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST;
        }

        if (!wait_for_connection) {
            *sockfd = fd;
            *pending = true;
            return ESP_OK;
        }

        fd_set write_set;
        FD_ZERO(&write_set);
        FD_SET(fd, &write_set);
        struct timeval tv;
        ms_to_timeval(timeout_ms, &tv);
        int res = select(fd + 1, NULL, &write_set, NULL, &tv);
        if (res == 0) {
            *connect_errno = ETIMEDOUT;
            close(fd);
            return ESP_ERR_ESP_TLS_CONNECTION_TIMEOUT;
        }
        if (res < 0) {
            *connect_errno = errno;
            close(fd);
            return ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST;
        }

        int sockerr = 0;
        socklen_t len = sizeof(sockerr);
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &sockerr, &len) < 0) {
            *connect_errno = errno;
            close(fd);
            return ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
        }
        if (sockerr != 0) {
            *connect_errno = sockerr;
            ESP_LOGD(TAG, "[sock=%d] delayed connect error: %s", fd, strerror(sockerr));
            close(fd);
            return ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST;
        }
    }

    if (cfg == NULL || !cfg->non_block) {
        ret = esp_tls_set_socket_non_blocking(fd, false);
        if (ret != ESP_OK) {
            *connect_errno = errno;
            close(fd);
            return ret;
        }
    }

    *sockfd = fd;
    *pending = false;
    return ESP_OK;
}

esp_err_t esp_tls_tcp_connect_addrinfo(const struct addrinfo *address_info, int port,
                                       const esp_tls_cfg_t *cfg,
                                       esp_tls_error_handle_t error_handle, int *sockfd)
{
    if (address_info == NULL || error_handle == NULL || sockfd == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *sockfd = -1;

    esp_err_t last_error = ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY;
    int last_errno = 0;
    bool found_candidate = false;
    int total_timeout_ms = esp_tls_connect_timeout_ms(cfg);
    uint64_t start_time_us = esp_tls_get_platform_time();
    for (const struct addrinfo *candidate = address_info; candidate != NULL; candidate = candidate->ai_next) {
        int timeout_ms = esp_tls_candidate_timeout_ms(start_time_us, total_timeout_ms,
                                                      esp_tls_addrinfo_count(candidate));
        if (timeout_ms == 0) {
            found_candidate = true;
            last_error = ESP_ERR_ESP_TLS_CONNECTION_TIMEOUT;
            last_errno = ETIMEDOUT;
            break;
        }
        int fd = -1;
        bool pending = false;
        int connect_errno = 0;
        esp_err_t ret = esp_tls_connect_one_address(candidate, port, cfg, true, timeout_ms,
                                                     &fd, &pending,
                                                     &connect_errno);
        if (ret == ESP_OK) {
            *sockfd = fd;
            return ESP_OK;
        }
        if (ret != ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY) {
            found_candidate = true;
            last_error = ret;
            last_errno = connect_errno;
        }
    }

    if (!found_candidate) {
        last_error = ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY;
    }
    if (last_errno != 0) {
        ESP_INT_EVENT_TRACKER_CAPTURE(error_handle, ESP_TLS_ERR_TYPE_SYSTEM, last_errno);
    }
    ESP_INT_EVENT_TRACKER_CAPTURE(error_handle, ESP_TLS_ERR_TYPE_ESP, last_error);
    return last_error;
}

static esp_err_t tcp_connect(const char *host, int hostlen, int port, const esp_tls_cfg_t *cfg,
                             esp_tls_error_handle_t error_handle, int *sockfd)
{
    struct addrinfo *address_info = NULL;
    esp_tls_addr_family_t addr_family = cfg != NULL ? cfg->addr_family : ESP_TLS_AF_UNSPEC;
    esp_err_t ret = esp_tls_resolve_hostname(host, hostlen, addr_family, &address_info);
    if (ret != ESP_OK) {
        ESP_INT_EVENT_TRACKER_CAPTURE(error_handle, ESP_TLS_ERR_TYPE_ESP, ret);
        return ret;
    }

    ret = esp_tls_tcp_connect_addrinfo(address_info, port, cfg, error_handle, sockfd);
    freeaddrinfo(address_info);
    return ret;
}

static void esp_tls_release_addrinfo(esp_tls_t *tls)
{
    if (tls->addrinfo != NULL) {
        freeaddrinfo(tls->addrinfo);
        tls->addrinfo = NULL;
    }
    tls->next_addrinfo = NULL;
}

esp_err_t esp_tls_connect_next_address(esp_tls_t *tls, int port, const esp_tls_cfg_t *cfg)
{
    while (tls->next_addrinfo != NULL) {
        const struct addrinfo *candidate = tls->next_addrinfo;
        tls->next_addrinfo = candidate->ai_next;
        int timeout_ms = esp_tls_candidate_timeout_ms(tls->connect_start_time_us,
                                                      esp_tls_connect_timeout_ms(cfg),
                                                      esp_tls_addrinfo_count(candidate));
        if (timeout_ms == 0) {
            tls->connect_error = ESP_ERR_ESP_TLS_CONNECTION_TIMEOUT;
            tls->connect_errno = ETIMEDOUT;
            break;
        }
        int fd = -1;
        bool pending = false;
        int connect_errno = 0;
        uint64_t attempt_start_time_us = esp_tls_get_platform_time();
        esp_err_t ret = esp_tls_connect_one_address(candidate, port, cfg, false, timeout_ms,
                                                     &fd,
                                                     &pending, &connect_errno);
        if (ret == ESP_OK) {
            tls->sockfd = fd;
            tls->connect_pending = pending;
            tls->connect_attempt_start_time_us = attempt_start_time_us;
            tls->connect_attempt_timeout_ms = timeout_ms;
            if (!pending) {
                esp_tls_release_addrinfo(tls);
            }
            return ESP_OK;
        }
        if (ret != ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY) {
            tls->connect_error = ret;
            tls->connect_errno = connect_errno;
        }
    }

    esp_err_t last_error = tls->connect_error;
    if (tls->connect_errno != 0) {
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_SYSTEM, tls->connect_errno);
    }
    ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, last_error);
    tls->sockfd = -1;
    tls->connect_pending = false;
    tls->conn_state = ESP_TLS_FAIL;
    esp_tls_release_addrinfo(tls);
    return last_error;
}

int esp_tls_poll_connect_address(esp_tls_t *tls, int port, const esp_tls_cfg_t *cfg)
{
    if (tls == NULL || cfg == NULL || !cfg->non_block || tls->sockfd < 0 || !tls->connect_pending) {
        return -1;
    }

    struct timeval tv;
    uint64_t timeout_us = (uint64_t)tls->connect_attempt_timeout_ms * 1000;
    uint64_t elapsed_us = esp_tls_get_platform_time() - tls->connect_attempt_start_time_us;
    int remaining_timeout_ms = elapsed_us < timeout_us ?
                               (int)((timeout_us - elapsed_us + 999) / 1000) : 0;
    ms_to_timeval(remaining_timeout_ms, &tv);

    fd_set read_set;
    fd_set write_set;
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    FD_SET(tls->sockfd, &read_set);
    FD_SET(tls->sockfd, &write_set);

    int select_ret = select(tls->sockfd + 1, &read_set, &write_set, NULL, &tv);
    int connect_error = 0;
    if (select_ret == 0) {
        ESP_LOGD(TAG, "connect attempt timed out; trying the next address");
        connect_error = ETIMEDOUT;
        tls->connect_error = ESP_ERR_ESP_TLS_CONNECTION_TIMEOUT;
    } else if (select_ret < 0) {
        if (errno == EINTR) {
            return 0;
        }
        connect_error = errno;
        tls->connect_error = ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST;
    } else if (FD_ISSET(tls->sockfd, &read_set) || FD_ISSET(tls->sockfd, &write_set)) {
        socklen_t len = sizeof(connect_error);
        if (getsockopt(tls->sockfd, SOL_SOCKET, SO_ERROR, &connect_error, &len) < 0) {
            connect_error = errno;
            tls->connect_error = ESP_ERR_ESP_TLS_SOCKET_SETOPT_FAILED;
        } else if (connect_error != 0) {
            tls->connect_error = ESP_ERR_ESP_TLS_FAILED_CONNECT_TO_HOST;
        } else {
            tls->connect_pending = false;
            esp_tls_release_addrinfo(tls);
            return 1;
        }
    } else {
        return 0;
    }

    tls->connect_errno = connect_error;
    close(tls->sockfd);
    tls->sockfd = -1;
    esp_err_t err = esp_tls_connect_next_address(tls, port, cfg);
    if (err != ESP_OK) {
        return -1;
    }
    return tls->connect_pending ? 0 : 1;
}

static int esp_tls_low_level_conn(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{

    esp_err_t esp_ret;
    /* These states are used to keep a tab on connection progress in case of non-blocking connect,
    and in case of blocking connect these cases will get executed one after the other */
    switch (tls->conn_state) {
    case ESP_TLS_INIT:
        tls->sockfd = -1;
        esp_tls_release_addrinfo(tls);
        tls->connect_pending = false;
        if (cfg != NULL && cfg->is_plain_tcp == false) {
            _esp_tls_net_init(tls);
            tls->is_tls = true;
        }
        if (cfg != NULL && cfg->non_block) {
            tls->connect_error = ESP_ERR_ESP_TLS_UNSUPPORTED_PROTOCOL_FAMILY;
            tls->connect_errno = 0;
            esp_ret = esp_tls_resolve_hostname(hostname, hostlen,
                                               cfg->addr_family, &tls->addrinfo);
            if (esp_ret != ESP_OK) {
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, esp_ret);
                tls->conn_state = ESP_TLS_FAIL;
                return -1;
            }
            tls->connect_start_time_us = esp_tls_get_platform_time();
            tls->next_addrinfo = tls->addrinfo;
            esp_ret = esp_tls_connect_next_address(tls, port, cfg);
            if (esp_ret != ESP_OK) {
                return -1;
            }
        } else if ((esp_ret = tcp_connect(hostname, hostlen, port, cfg,
                                          tls->error_handle, &tls->sockfd)) != ESP_OK) {
            tls->conn_state = ESP_TLS_FAIL;
            return -1;
        }
        if (tls->is_tls == false && !(cfg != NULL && cfg->non_block && tls->connect_pending)) {
            tls->read = tcp_read;
            tls->write = tcp_write;
            tls->conn_state = ESP_TLS_DONE;
            ESP_LOGD(TAG, "non-tls connection established");
            return 1;
        }
        tls->conn_state = ESP_TLS_CONNECTING;
    /* falls through */
    case ESP_TLS_CONNECTING:
        if (cfg && cfg->non_block && tls->connect_pending) {
            ESP_LOGD(TAG, "connecting...");
            int connect_ret = esp_tls_poll_connect_address(tls, port, cfg);
            if (connect_ret <= 0) {
                return connect_ret;
            }
        }
        if (tls->is_tls == false) {
            tls->read = tcp_read;
            tls->write = tcp_write;
            tls->conn_state = ESP_TLS_DONE;
            ESP_LOGD(TAG, "non-tls connection established");
            return 1;
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
    if (sockfd == NULL || error_handle == NULL || host == NULL || hostlen < 0) {
        return ESP_ERR_INVALID_ARG;
    }
    return tcp_connect(host, hostlen, port, cfg, error_handle, sockfd);
}

int esp_tls_conn_new_sync(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    if (!cfg || !tls || !hostname || hostlen < 0) {
        return -1;
    }
    uint64_t start_time_us;
    start_time_us = esp_tls_get_platform_time();
    while (1) {
        int ret = esp_tls_low_level_conn(hostname, hostlen, port, cfg, tls);
        if (ret == 1) {
            return ret;
        } else if (ret == -1) {
            ESP_LOGE(TAG, "Failed to open new connection");
            return -1;
        } else if (ret == 0 && cfg->timeout_ms > 0) {
            uint64_t elapsed_time_us = esp_tls_get_platform_time() - start_time_us;
            if ((elapsed_time_us / 1000) >= cfg->timeout_ms) {
                ESP_LOGW(TAG, "Failed to open new connection in specified timeout");
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, ESP_ERR_ESP_TLS_CONNECTION_TIMEOUT);
                return -1;
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
    if (!url || !cfg || !tls) {
        return -1;
    }

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

const uint16_t *esp_tls_get_supported_groups_list(void)
{
    return _esp_tls_get_supported_groups_list();
}

esp_err_t esp_tls_server_session_init(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls)
{
    return _esp_tls_server_session_init(cfg, sockfd, tls);
}

int esp_tls_server_session_continue_async(esp_tls_t *tls)
{
    return _esp_tls_server_session_continue_async(tls);
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
