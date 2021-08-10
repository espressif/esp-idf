// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/mem.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "esp_log.h"
#include "ping/ping_sock.h"

const static char *TAG = "ping_sock";

#define PING_CHECK(a, str, goto_tag, ret_value, ...)                              \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

#define PING_TIME_DIFF_MS(_end, _start) ((uint32_t)(((_end).tv_sec - (_start).tv_sec) * 1000 + \
                                                    ((_end).tv_usec - (_start).tv_usec) / 1000))

#define PING_CHECK_START_TIMEOUT_MS (1000)

#define PING_FLAGS_INIT (1 << 0)
#define PING_FLAGS_START (1 << 1)

typedef struct {
    int sock;
    struct sockaddr_storage target_addr;
    TaskHandle_t ping_task_hdl;
    struct icmp_echo_hdr *packet_hdr;
    ip_addr_t recv_addr;
    uint32_t recv_len;
    uint32_t icmp_pkt_size;
    uint32_t count;
    uint32_t transmitted;
    uint32_t received;
    uint32_t interval_ms;
    uint32_t elapsed_time_ms;
    uint32_t total_time_ms;
    uint8_t ttl;
    uint32_t flags;
    void (*on_ping_success)(esp_ping_handle_t hdl, void *args);
    void (*on_ping_timeout)(esp_ping_handle_t hdl, void *args);
    void (*on_ping_end)(esp_ping_handle_t hdl, void *args);
    void *cb_args;
} esp_ping_t;

static esp_err_t esp_ping_send(esp_ping_t *ep)
{
    esp_err_t ret = ESP_OK;
    ep->packet_hdr->seqno++;
    /* generate checksum since "seqno" has changed */
    ep->packet_hdr->chksum = 0;
    if (ep->packet_hdr->type == ICMP_ECHO) {
        ep->packet_hdr->chksum = inet_chksum(ep->packet_hdr, ep->icmp_pkt_size);
    }
    
    int sent = sendto(ep->sock, ep->packet_hdr, ep->icmp_pkt_size, 0,
                      (struct sockaddr *)&ep->target_addr, sizeof(ep->target_addr));

    if (sent != ep->icmp_pkt_size) {
        int opt_val;
        socklen_t opt_len = sizeof(opt_val);
        getsockopt(ep->sock, SOL_SOCKET, SO_ERROR, &opt_val, &opt_len);
        ESP_LOGE(TAG, "send error=%d", opt_val);
        ret = ESP_FAIL;
    } else {
        ep->transmitted++;
    }
    return ret;
}

static int esp_ping_receive(esp_ping_t *ep)
{
    char buf[64]; // 64 bytes are enough to cover IP header and ICMP header
    int len = 0;
    struct sockaddr_storage from;
    int fromlen = sizeof(from);
    uint16_t data_head = 0;

    while ((len = recvfrom(ep->sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, (socklen_t *)&fromlen)) > 0) {
        if (from.ss_family == AF_INET) {
            // IPv4
            struct sockaddr_in *from4 = (struct sockaddr_in *)&from;
            inet_addr_to_ip4addr(ip_2_ip4(&ep->recv_addr), &from4->sin_addr);
            IP_SET_TYPE_VAL(ep->recv_addr, IPADDR_TYPE_V4);
            data_head = (uint16_t)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr));
        } else {
            // IPv6
            struct sockaddr_in6 *from6 = (struct sockaddr_in6 *)&from;
            inet6_addr_to_ip6addr(ip_2_ip6(&ep->recv_addr), &from6->sin6_addr);
            IP_SET_TYPE_VAL(ep->recv_addr, IPADDR_TYPE_V6);
            data_head = (uint16_t)(sizeof(struct ip6_hdr) + sizeof(struct icmp6_echo_hdr));
        }
        if (len >= data_head) {
            if (IP_IS_V4_VAL(ep->recv_addr)) {              // Currently we process IPv4
                struct ip_hdr *iphdr = (struct ip_hdr *)buf;
                struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
                if ((iecho->id == ep->packet_hdr->id) && (iecho->seqno == ep->packet_hdr->seqno)) {
                    ep->received++;
                    ep->ttl = iphdr->_ttl;
                    ep->recv_len = lwip_ntohs(IPH_LEN(iphdr)) - data_head;  // The data portion of ICMP
                    return len;
                }
            } else if (IP_IS_V6_VAL(ep->recv_addr)) {      // Currently we process IPv6
                struct ip6_hdr *iphdr = (struct ip6_hdr *)buf;
                struct icmp6_echo_hdr *iecho6 = (struct icmp6_echo_hdr *)(buf + sizeof(struct ip6_hdr)); // IPv6 head length is 40
                if ((iecho6->id == ep->packet_hdr->id) && (iecho6->seqno == ep->packet_hdr->seqno)) {
                    ep->received++;
                    ep->recv_len = IP6H_PLEN(iphdr) - sizeof(struct icmp6_echo_hdr); //The data portion of ICMPv6
                    return len;
                }
            }
        }
        fromlen = sizeof(from);
    }
    // if timeout, len will be -1
    return len;
}

static void esp_ping_thread(void *args)
{
    esp_ping_t *ep = (esp_ping_t *)(args);
    TickType_t last_wake;
    struct timeval start_time, end_time;
    int recv_ret;

    while (1) {
        /* wait for ping start signal */
        if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(PING_CHECK_START_TIMEOUT_MS))) {
            /* initialize runtime statistics */
            ep->packet_hdr->seqno = 0;
            ep->transmitted = 0;
            ep->received = 0;
            ep->total_time_ms = 0;

            last_wake = xTaskGetTickCount();
            while ((ep->flags & PING_FLAGS_START) && ((ep->count == 0) || (ep->packet_hdr->seqno < ep->count))) {
                esp_ping_send(ep);
                gettimeofday(&start_time, NULL);
                recv_ret = esp_ping_receive(ep);
                gettimeofday(&end_time, NULL);
                ep->elapsed_time_ms = PING_TIME_DIFF_MS(end_time, start_time);
                ep->total_time_ms += ep->elapsed_time_ms;
                if (recv_ret >= 0) {
                    if (ep->on_ping_success) {
                        ep->on_ping_success((esp_ping_handle_t)ep, ep->cb_args);
                    }
                } else {
                    if (ep->on_ping_timeout) {
                        ep->on_ping_timeout((esp_ping_handle_t)ep, ep->cb_args);
                    }
                }
                if (pdMS_TO_TICKS(ep->interval_ms)) {
                    vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(ep->interval_ms)); // to get a more accurate delay
                }
            }
            /* batch of ping operations finished */
            if (ep->on_ping_end) {
                ep->on_ping_end((esp_ping_handle_t)ep, ep->cb_args);
            }
        } else {
            // check if ping has been de-initialized
            if (!(ep->flags & PING_FLAGS_INIT)) {
                break;
            }
        }
    }
    /* before exit task, free all resources */
    if (ep->packet_hdr) {
        free(ep->packet_hdr);
    }
    if (ep->sock > 0) {
        close(ep->sock);
    }
    free(ep);
    vTaskDelete(NULL);
}

esp_err_t esp_ping_new_session(const esp_ping_config_t *config, const esp_ping_callbacks_t *cbs, esp_ping_handle_t *hdl_out)
{
    esp_err_t ret = ESP_FAIL;
    esp_ping_t *ep = NULL;
    PING_CHECK(config, "ping config can't be null", err, ESP_ERR_INVALID_ARG);
    PING_CHECK(hdl_out, "ping handle can't be null", err, ESP_ERR_INVALID_ARG);

    ep = mem_calloc(1, sizeof(esp_ping_t));
    PING_CHECK(ep, "no memory for esp_ping object", err, ESP_ERR_NO_MEM);

    /* set INIT flag, so that ping task won't exit (must set before create ping task) */
    ep->flags |= PING_FLAGS_INIT;

    /* create ping thread */
    BaseType_t xReturned = xTaskCreate(esp_ping_thread, "ping", config->task_stack_size, ep,
                                       config->task_prio, &ep->ping_task_hdl);
    PING_CHECK(xReturned == pdTRUE, "create ping task failed", err, ESP_ERR_NO_MEM);

    /* callback functions */
    if (cbs) {
        ep->cb_args = cbs->cb_args;
        ep->on_ping_end = cbs->on_ping_end;
        ep->on_ping_timeout = cbs->on_ping_timeout;
        ep->on_ping_success = cbs->on_ping_success;
    }
    /* set parameters for ping */
    ep->recv_addr = config->target_addr;
    ep->count = config->count;
    ep->interval_ms = config->interval_ms;
    ep->icmp_pkt_size = sizeof(struct icmp_echo_hdr) + config->data_size;
    ep->packet_hdr = mem_calloc(1, ep->icmp_pkt_size);
    PING_CHECK(ep->packet_hdr, "no memory for echo packet", err, ESP_ERR_NO_MEM);
    /* set ICMP type and code field */
    ep->packet_hdr->code = 0;
    /* ping id should be unique, treat task handle as ping ID */
    ep->packet_hdr->id = ((uint32_t)ep->ping_task_hdl) & 0xFFFF;
    /* fill the additional data buffer with some data */
    char *d = (char *)(ep->packet_hdr) + sizeof(struct icmp_echo_hdr);
    for (uint32_t i = 0; i < config->data_size; i++) {
        d[i] = 'A' + i;
    }

    /* create socket */
    if (IP_IS_V4(&config->target_addr) || ip6_addr_isipv4mappedipv6(ip_2_ip6(&config->target_addr))) {
        ep->sock = socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
    } else {
        ep->sock = socket(AF_INET6, SOCK_RAW, IP6_NEXTH_ICMP6);
    }
    PING_CHECK(ep->sock > 0, "create socket failed: %d", err, ESP_FAIL, ep->sock);
    /* set if index */
    if(config->interface) {
        struct ifreq iface;
        if(netif_index_to_name(config->interface, iface.ifr_name) == NULL) {
            ESP_LOGE(TAG, "fail to find interface name with netif index %d", config->interface);
            goto err;
        }
        if(setsockopt(ep->sock, SOL_SOCKET, SO_BINDTODEVICE, &iface, sizeof(iface)) != 0) {
            ESP_LOGE(TAG, "fail to setsockopt SO_BINDTODEVICE");
            goto err;
        }
    }
    struct timeval timeout;
    timeout.tv_sec = config->timeout_ms / 1000;
    timeout.tv_usec = (config->timeout_ms % 1000) * 1000;
    /* set receive timeout */
    setsockopt(ep->sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    /* set tos */
    setsockopt(ep->sock, IPPROTO_IP, IP_TOS, &config->tos, sizeof(config->tos));

    /* set socket address */
    if (IP_IS_V4(&config->target_addr)) {
        struct sockaddr_in *to4 = (struct sockaddr_in *)&ep->target_addr;
        to4->sin_family = AF_INET;
        inet_addr_from_ip4addr(&to4->sin_addr, ip_2_ip4(&config->target_addr));
        ep->packet_hdr->type = ICMP_ECHO;
    }
    if (IP_IS_V6(&config->target_addr)) {
        struct sockaddr_in6 *to6 = (struct sockaddr_in6 *)&ep->target_addr;
        to6->sin6_family = AF_INET6;
        inet6_addr_from_ip6addr(&to6->sin6_addr, ip_2_ip6(&config->target_addr));
        ep->packet_hdr->type = ICMP6_TYPE_EREQ;
    }
    /* return ping handle to user */
    *hdl_out = (esp_ping_handle_t)ep;
    return ESP_OK;
err:
    if (ep) {
        if (ep->sock > 0) {
            close(ep->sock);
        }
        if (ep->packet_hdr) {
            free(ep->packet_hdr);
        }
        if (ep->ping_task_hdl) {
            vTaskDelete(ep->ping_task_hdl);
        }
        free(ep);
    }
    return ret;
}

esp_err_t esp_ping_delete_session(esp_ping_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_ping_t *ep = (esp_ping_t *)hdl;
    PING_CHECK(ep, "ping handle can't be null", err, ESP_ERR_INVALID_ARG);
    /* reset init flags, then ping task will exit */
    ep->flags &= ~PING_FLAGS_INIT;
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_ping_start(esp_ping_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_ping_t *ep = (esp_ping_t *)hdl;
    PING_CHECK(ep, "ping handle can't be null", err, ESP_ERR_INVALID_ARG);
    ep->flags |= PING_FLAGS_START;
    xTaskNotifyGive(ep->ping_task_hdl);
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_ping_stop(esp_ping_handle_t hdl)
{
    esp_err_t ret = ESP_OK;
    esp_ping_t *ep = (esp_ping_t *)hdl;
    PING_CHECK(ep, "ping handle can't be null", err, ESP_ERR_INVALID_ARG);
    ep->flags &= ~PING_FLAGS_START;
    return ESP_OK;
err:
    return ret;
}

esp_err_t esp_ping_get_profile(esp_ping_handle_t hdl, esp_ping_profile_t profile, void *data, uint32_t size)
{
    esp_err_t ret = ESP_OK;
    esp_ping_t *ep = (esp_ping_t *)hdl;
    const void *from = NULL;
    uint32_t copy_size = 0;
    PING_CHECK(ep, "ping handle can't be null", err, ESP_ERR_INVALID_ARG);
    PING_CHECK(data, "profile data can't be null", err, ESP_ERR_INVALID_ARG);
    switch (profile) {
    case ESP_PING_PROF_SEQNO:
        from = &ep->packet_hdr->seqno;
        copy_size = sizeof(ep->packet_hdr->seqno);
        break;
    case ESP_PING_PROF_TTL:
        from = &ep->ttl;
        copy_size = sizeof(ep->ttl);
        break;
    case ESP_PING_PROF_REQUEST:
        from = &ep->transmitted;
        copy_size = sizeof(ep->transmitted);
        break;
    case ESP_PING_PROF_REPLY:
        from = &ep->received;
        copy_size = sizeof(ep->received);
        break;
    case ESP_PING_PROF_IPADDR:
        from = &ep->recv_addr;
        copy_size = sizeof(ep->recv_addr);
        break;
    case ESP_PING_PROF_SIZE:
        from = &ep->recv_len;
        copy_size = sizeof(ep->recv_len);
        break;
    case ESP_PING_PROF_TIMEGAP:
        from = &ep->elapsed_time_ms;
        copy_size = sizeof(ep->elapsed_time_ms);
        break;
    case ESP_PING_PROF_DURATION:
        from = &ep->total_time_ms;
        copy_size = sizeof(ep->total_time_ms);
        break;
    default:
        PING_CHECK(false, "unknow profile: %d", err, ESP_ERR_INVALID_ARG, profile);
        break;
    }
    PING_CHECK(size >= copy_size, "unmatched data size for profile %d", err, ESP_ERR_INVALID_SIZE, profile);
    memcpy(data, from, copy_size);
    return ESP_OK;
err:
    return ret;
}
