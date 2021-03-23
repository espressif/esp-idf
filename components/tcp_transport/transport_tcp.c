// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#include <stdlib.h>
#include <string.h>

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "esp_transport_utils.h"
#include "esp_transport.h"

static const char *TAG = "TRANS_TCP";

typedef struct {
    int sock;
} transport_tcp_t;

static int resolve_dns(const char *host, struct sockaddr_in *ip) 
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;

    int err = getaddrinfo(host, NULL, &hints, &res);
    if(err != 0 || res == NULL) {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        return ESP_FAIL;
    }
    ip->sin_family = AF_INET;
    memcpy(&ip->sin_addr, &((struct sockaddr_in *)(res->ai_addr))->sin_addr, sizeof(ip->sin_addr));
    freeaddrinfo(res);
    return ESP_OK;
}

static int tcp_enable_keep_alive(int fd, esp_transport_keep_alive_t *keep_alive_cfg)
{
    int keep_alive_enable = 1;
    int keep_alive_idle = keep_alive_cfg->keep_alive_idle;
    int keep_alive_interval = keep_alive_cfg->keep_alive_interval;
    int keep_alive_count = keep_alive_cfg->keep_alive_count;

    ESP_LOGD(TAG, "Enable TCP keep alive. idle: %d, interval: %d, count: %d", keep_alive_idle, keep_alive_interval, keep_alive_count);
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive_enable, sizeof(keep_alive_enable)) != 0) {
        ESP_LOGE(TAG, "Fail to setsockopt SO_KEEPALIVE");
        return -1;
    }
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keep_alive_idle, sizeof(keep_alive_idle)) != 0) {
        ESP_LOGE(TAG, "Fail to setsockopt TCP_KEEPIDLE");
        return -1;
    }
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keep_alive_interval, sizeof(keep_alive_interval)) != 0) {
        ESP_LOGE(TAG, "Fail to setsockopt TCP_KEEPINTVL");
        return -1;
    }
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keep_alive_count, sizeof(keep_alive_count)) != 0) {
        ESP_LOGE(TAG, "Fail to setsockopt TCP_KEEPCNT");
        return -1;
    }

    return 0;
}

static int tcp_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    struct sockaddr_in remote_ip;
    struct timeval tv = { 0 };
    transport_tcp_t *tcp = esp_transport_get_context_data(t);
    esp_transport_keep_alive_t *keep_alive_cfg = esp_transport_get_keep_alive(t);

    bzero(&remote_ip, sizeof(struct sockaddr_in));

    //if stream_host is not ip address, resolve it AF_INET,servername,&serveraddr.sin_addr
    if (inet_pton(AF_INET, host, &remote_ip.sin_addr) != 1) {
        if (resolve_dns(host, &remote_ip) < 0) {
            return -1;
        }
    }

    tcp->sock = socket(PF_INET, SOCK_STREAM, 0);

    if (tcp->sock < 0) {
        ESP_LOGE(TAG, "Error create socket");
        return -1;
    }

    remote_ip.sin_family = AF_INET;
    remote_ip.sin_port = htons(port);

    esp_transport_utils_ms_to_timeval(timeout_ms, &tv); // if timeout=-1, tv is unchanged, 0, i.e. waits forever

    setsockopt(tcp->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(tcp->sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    // Set socket keep-alive option
    if (keep_alive_cfg && keep_alive_cfg->keep_alive_enable) {
        if (tcp_enable_keep_alive(tcp->sock, keep_alive_cfg) < 0) {
            ESP_LOGE(TAG, "Error to set tcp [socket=%d] keep-alive", tcp->sock);
            close(tcp->sock);
            tcp->sock = -1;
            return -1;
        }
    }
    // Set socket to non-blocking
    int flags;
    if ((flags = fcntl(tcp->sock, F_GETFL, NULL)) < 0) {
        ESP_LOGE(TAG, "[sock=%d] get file flags error: %s", tcp->sock, strerror(errno));
        goto error;
    }
    if (fcntl(tcp->sock, F_SETFL, flags |= O_NONBLOCK) < 0) {
        ESP_LOGE(TAG, "[sock=%d] set nonblocking error: %s", tcp->sock, strerror(errno));
        goto error;
    }

    ESP_LOGD(TAG, "[sock=%d] Connecting to server. IP: %s, Port: %d",
            tcp->sock, ipaddr_ntoa((const ip_addr_t*)&remote_ip.sin_addr.s_addr), port);

    if (connect(tcp->sock, (struct sockaddr *)(&remote_ip), sizeof(struct sockaddr)) < 0) {
        if (errno == EINPROGRESS) {
            fd_set fdset;

            esp_transport_utils_ms_to_timeval(timeout_ms, &tv);
            FD_ZERO(&fdset);
            FD_SET(tcp->sock, &fdset);

            int res = select(tcp->sock+1, NULL, &fdset, NULL, &tv);
            if (res < 0) {
                ESP_LOGE(TAG, "[sock=%d] select() error: %s", tcp->sock, strerror(errno));
                goto error;
            }
            else if (res == 0) {
                ESP_LOGE(TAG, "[sock=%d] select() timeout", tcp->sock);
                goto error;
            } else {
                int sockerr;
                socklen_t len = (socklen_t)sizeof(int);

                if (getsockopt(tcp->sock, SOL_SOCKET, SO_ERROR, (void*)(&sockerr), &len) < 0) {
                    ESP_LOGE(TAG, "[sock=%d] getsockopt() error: %s", tcp->sock, strerror(errno));
                    goto error;
                }
                else if (sockerr) {
                    ESP_LOGE(TAG, "[sock=%d] delayed connect error: %s", tcp->sock, strerror(sockerr));
                    goto error;
                }
            }
        } else {
            ESP_LOGE(TAG, "[sock=%d] connect() error: %s", tcp->sock, strerror(errno));
            goto error;
        }
    }
    // Reset socket to blocking
    if ((flags = fcntl(tcp->sock, F_GETFL, NULL)) < 0) {
        ESP_LOGE(TAG, "[sock=%d] get file flags error: %s", tcp->sock, strerror(errno));
        goto error;
    }
    if (fcntl(tcp->sock, F_SETFL, flags & ~O_NONBLOCK) < 0) {
        ESP_LOGE(TAG, "[sock=%d] reset blocking error: %s", tcp->sock, strerror(errno));
        goto error;
    }
    return tcp->sock;
error:
    close(tcp->sock);
    tcp->sock = -1;
    return -1;
}

static int tcp_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    int poll;
    transport_tcp_t *tcp = esp_transport_get_context_data(t);
    if ((poll = esp_transport_poll_write(t, timeout_ms)) <= 0) {
        return poll;
    }
    return write(tcp->sock, buffer, len);
}

static int tcp_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    transport_tcp_t *tcp = esp_transport_get_context_data(t);
    int poll = -1;
    if ((poll = esp_transport_poll_read(t, timeout_ms)) <= 0) {
        return poll;
    }
    int read_len = read(tcp->sock, buffer, len);
    if (read_len == 0) {
        return -1;
    }
    return read_len;
}

static int tcp_poll_read(esp_transport_handle_t t, int timeout_ms)
{
    transport_tcp_t *tcp = esp_transport_get_context_data(t);
    int ret = -1;
    struct timeval timeout;
    fd_set readset;
    fd_set errset;
    FD_ZERO(&readset);
    FD_ZERO(&errset);
    FD_SET(tcp->sock, &readset);
    FD_SET(tcp->sock, &errset);

    ret = select(tcp->sock + 1, &readset, NULL, &errset, esp_transport_utils_ms_to_timeval(timeout_ms, &timeout));
    if (ret > 0 && FD_ISSET(tcp->sock, &errset)) {
        int sock_errno = 0;
        uint32_t optlen = sizeof(sock_errno);
        getsockopt(tcp->sock, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
        ESP_LOGE(TAG, "tcp_poll_read select error %d, errno = %s, fd = %d", sock_errno, strerror(sock_errno), tcp->sock);
        ret = -1;
    }
    return ret;
}

static int tcp_poll_write(esp_transport_handle_t t, int timeout_ms)
{
    transport_tcp_t *tcp = esp_transport_get_context_data(t);
    int ret = -1;
    struct timeval timeout;
    fd_set writeset;
    fd_set errset;
    FD_ZERO(&writeset);
    FD_ZERO(&errset);
    FD_SET(tcp->sock, &writeset);
    FD_SET(tcp->sock, &errset);

    ret = select(tcp->sock + 1, NULL, &writeset, &errset, esp_transport_utils_ms_to_timeval(timeout_ms, &timeout));
    if (ret > 0 && FD_ISSET(tcp->sock, &errset)) {
        int sock_errno = 0;
        uint32_t optlen = sizeof(sock_errno);
        getsockopt(tcp->sock, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);
        ESP_LOGE(TAG, "tcp_poll_write select error %d, errno = %s, fd = %d", sock_errno, strerror(sock_errno), tcp->sock);
        ret = -1;
    }
    return ret;
}

static int tcp_close(esp_transport_handle_t t)
{
    transport_tcp_t *tcp = esp_transport_get_context_data(t);
    int ret = -1;
    if (tcp->sock >= 0) {
        ret = close(tcp->sock);
        tcp->sock = -1;
    }
    return ret;
}

static esp_err_t tcp_destroy(esp_transport_handle_t t)
{
    transport_tcp_t *tcp = esp_transport_get_context_data(t);
    esp_transport_close(t);
    free(tcp);
    return 0;
}

void esp_transport_tcp_set_keep_alive(esp_transport_handle_t t, esp_transport_keep_alive_t *keep_alive_cfg)
{
    esp_transport_set_keep_alive(t, keep_alive_cfg);
}

esp_transport_handle_t esp_transport_tcp_init(void)
{
    esp_transport_handle_t t = esp_transport_init();
    transport_tcp_t *tcp = calloc(1, sizeof(transport_tcp_t));
    ESP_TRANSPORT_MEM_CHECK(TAG, tcp, {
        esp_transport_destroy(t);
	return NULL;
    });
    tcp->sock = -1;
    esp_transport_set_func(t, tcp_connect, tcp_read, tcp_write, tcp_close, tcp_poll_read, tcp_poll_write, tcp_destroy);
    esp_transport_set_context_data(t, tcp);

    return t;
}
