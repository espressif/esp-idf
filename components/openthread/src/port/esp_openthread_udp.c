/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_openthread.h"
#include "esp_openthread_border_router.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_netif_glue_priv.h"
#include "esp_openthread_task_queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/ip_addr.h"
#include "lwip/mld6.h"
#include "lwip/pbuf.h"
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#include "openthread/error.h"
#include "openthread/platform/udp.h"

typedef struct {
    otUdpSocket *socket;
    struct pbuf *recv_buf;
    ip_addr_t addr;
    uint16_t port;
    uint8_t hop_limit;
    bool is_host_interface;
} udp_recv_task_t;

typedef struct {
    otUdpSocket *socket;
    struct udp_pcb *pcb_ret;
} udp_new_task_t;

typedef struct {
    struct udp_pcb *pcb;
    ip_addr_t addr;
    uint16_t port;
} udp_bind_connect_task_t;

typedef struct {
    struct udp_pcb *pcb;
    uint8_t netif_index;
} udp_bind_netif_task_t;

typedef struct {
    struct udp_pcb *pcb;
    otMessage *message;
    ip_addr_t source_addr;
    uint16_t source_port;
    ip_addr_t peer_addr;
    uint16_t peer_port;
    bool multicast_loop;
    uint8_t hop_limit;
    uint8_t netif_index;
} udp_send_task_t;

typedef struct {
    bool is_join;
    uint8_t netif_index;
    ip6_addr_t addr;
} udp_multicast_join_leave_task_t;

static ip_addr_t map_openthread_addr_to_lwip_addr(const otIp6Address *address)
{
    ip_addr_t addr;

    memcpy(ip_2_ip6(&addr)->addr, address->mFields.m8, sizeof(ip_2_ip6(&addr)->addr));
#if CONFIG_LWIP_IPV4
    if (ip6_addr_isipv4mappedipv6(ip_2_ip6(&addr))) {
        unmap_ipv4_mapped_ipv6(ip_2_ip4(&addr), ip_2_ip6(&addr));
        addr.type = IPADDR_TYPE_V4;
    } else {
        addr.type = IPADDR_TYPE_V6;
#if LWIP_IPV6_SCOPES
        addr.u_addr.ip6.zone = IP6_NO_ZONE;
#endif // LWIP_IPV6_SCOPES
    }
#else
#if LWIP_IPV6_SCOPES
    addr.zone = IP6_NO_ZONE;
#endif // LWIP_IPV6_SCOPES
#endif // CONFIG_LWIP_IPV4
    return addr;
}

static void udp_recv_task(void *ctx)
{
    udp_recv_task_t *task = (udp_recv_task_t *)ctx;

    otMessageInfo message_info;
    otMessage *message = NULL;
    otMessageSettings msg_settings = { .mLinkSecurityEnabled = false, .mPriority = OT_MESSAGE_PRIORITY_NORMAL };
    struct pbuf *recv_buf = task->recv_buf;
    uint8_t *data_buf = (uint8_t *)recv_buf->payload;
    uint8_t *data_buf_to_free = NULL;

    message_info.mSockPort = 0;
    memset(&message_info.mSockAddr, 0, sizeof(message_info.mSockAddr));
    message_info.mHopLimit = task->hop_limit;
    message_info.mPeerPort = task->port;
#if CONFIG_LWIP_IPV4
    if (task->addr.type == IPADDR_TYPE_V4) {
        ip4_2_ipv4_mapped_ipv6(ip_2_ip6(&task->addr), ip_2_ip4(&task->addr));
    }
#endif
    memcpy(&message_info.mPeerAddr, ip_2_ip6(&task->addr)->addr, sizeof(message_info.mPeerAddr));

    if (recv_buf->next != NULL) {
        data_buf = (uint8_t *)malloc(recv_buf->tot_len);
        if (data_buf != NULL) {
            data_buf_to_free = data_buf;
            pbuf_copy_partial(recv_buf, data_buf, recv_buf->tot_len, 0);
        }
    }
    VerifyOrExit(data_buf != NULL,
                 ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to allocate data buf when receiving OpenThread plat UDP"));
    message = otUdpNewMessage(esp_openthread_get_instance(), &msg_settings);
    VerifyOrExit(message != NULL,
                 ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to allocate OpenThread message when receiving OpenThread plat UDP"));
    VerifyOrExit(otMessageAppend(message, data_buf, recv_buf->tot_len) == OT_ERROR_NONE,
                 ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to copy OpenThread message when receiving OpenThread plat UDP"));
    task->socket->mHandler(task->socket->mContext, message, &message_info);
    otMessageFree(message);

exit:
    free(task);
    if (data_buf_to_free) {
        free(data_buf_to_free);
    }
    pbuf_free(recv_buf);
    return;
}

static void handle_udp_recv(void *ctx, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
    udp_recv_task_t *task = (udp_recv_task_t *)malloc(sizeof(udp_recv_task_t));
    const struct ip6_hdr *ip6_hdr = ip6_current_header();
#if CONFIG_LWIP_IPV4
    const struct ip_hdr *ip4_hdr = ip4_current_header();
#endif
    struct netif *source_netif = ip_current_netif();

    if (task == NULL) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to allocate recv task when receiving OpenThread plat UDP");
    }
    task->socket = (otUdpSocket *)ctx;
    task->recv_buf = p;
    task->addr = *addr;
    task->port = port;
#if CONFIG_LWIP_IPV4
    task->hop_limit = (addr->type == IPADDR_TYPE_V6) ? IP6H_HOPLIM(ip6_hdr) : IPH_TTL(ip4_hdr);
#else
    task->hop_limit = IP6H_HOPLIM(ip6_hdr);
#endif
    task->is_host_interface =
        (netif_get_index(source_netif) == esp_netif_get_netif_impl_index(esp_openthread_get_backbone_netif()));

    if (esp_openthread_task_queue_post(udp_recv_task, task) != ESP_OK) {
        free(task);
    }
}

static esp_err_t udp_new_task(void *ctx)
{
    udp_new_task_t *task = (udp_new_task_t *)ctx;
    task->pcb_ret = udp_new();
    ESP_RETURN_ON_FALSE(task->pcb_ret != NULL, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG, "Failed to create a new UDP pcb");
    udp_recv(task->pcb_ret, handle_udp_recv, task->socket);
    return ESP_OK;
}

otError otPlatUdpSocket(otUdpSocket *udp_socket)
{
    otError error = OT_ERROR_NONE;
    esp_err_t err = ESP_OK;

    udp_new_task_t task = {.socket = udp_socket };
    esp_openthread_task_switching_lock_release();
    err = esp_netif_tcpip_exec(udp_new_task, &task);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
    VerifyOrExit(err == ESP_OK, error = OT_ERROR_FAILED);
    udp_socket->mHandle = task.pcb_ret;

exit:
    return error;
}

static void udp_close_task(void *ctx)
{
    struct udp_pcb *pcb = (struct udp_pcb *)ctx;

    udp_remove(pcb);
}

otError otPlatUdpClose(otUdpSocket *udp_socket)
{
    struct udp_pcb *pcb = (struct udp_pcb *)udp_socket->mHandle;

    if (pcb) {
        esp_openthread_task_switching_lock_release();
        tcpip_callback(udp_close_task, pcb);
        esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
    }

    return OT_ERROR_NONE;
}

static esp_err_t udp_bind_task(void *ctx)
{
    udp_bind_connect_task_t *task = (udp_bind_connect_task_t *)ctx;
    err_t ret = udp_bind(task->pcb, &task->addr, task->port);
    return (ret == ERR_OK) ? ESP_OK : ESP_FAIL;
}

otError otPlatUdpBind(otUdpSocket *udp_socket)
{
    esp_err_t err = ESP_OK;
    udp_bind_connect_task_t task = {
        .pcb = (struct udp_pcb *)udp_socket->mHandle,
        .port = udp_socket->mSockName.mPort,
    };
    ESP_LOGI(OT_PLAT_LOG_TAG, "Platform UDP bound to port %d", udp_socket->mSockName.mPort);

#if CONFIG_LWIP_IPV4
    task.addr.type = IPADDR_TYPE_ANY;
#endif
    memcpy(ip_2_ip6(&task.addr)->addr, udp_socket->mSockName.mAddress.mFields.m8, sizeof(ip_2_ip6(&task.addr)->addr));
    esp_openthread_task_switching_lock_release();
    err = esp_netif_tcpip_exec(udp_bind_task, &task);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);

    return err == ESP_OK ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

static esp_err_t udp_bind_netif_task(void *ctx)
{
    udp_bind_netif_task_t *task = (udp_bind_netif_task_t *)ctx;
    udp_bind_netif(task->pcb, netif_get_by_index(task->netif_index));
    return ESP_OK;
}

static uint8_t get_netif_index(otNetifIdentifier netif_identifier)
{
    switch (netif_identifier) {
    case OT_NETIF_UNSPECIFIED:
        return NETIF_NO_INDEX;
    case OT_NETIF_THREAD:
        return esp_netif_get_netif_impl_index(esp_openthread_get_netif());
    case OT_NETIF_BACKBONE:
        return esp_netif_get_netif_impl_index(esp_openthread_get_backbone_netif());
    default:
        return NETIF_NO_INDEX;
    }
}

otError otPlatUdpBindToNetif(otUdpSocket *udp_socket, otNetifIdentifier netif_identifier)
{
    esp_err_t err = ESP_OK;
    udp_bind_netif_task_t task = {
        .pcb = (struct udp_pcb *)udp_socket->mHandle,
        .netif_index = get_netif_index(netif_identifier),
    };

    esp_openthread_task_switching_lock_release();
    err = esp_netif_tcpip_exec(udp_bind_netif_task, &task);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);

    return err == ESP_OK ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

static esp_err_t udp_connect_task(void *ctx)
{
    udp_bind_connect_task_t *task = (udp_bind_connect_task_t *)ctx;
    err_t ret = udp_connect(task->pcb, &task->addr, task->port);
    return (ret == ERR_OK) ? ESP_OK : ESP_FAIL;
}

otError otPlatUdpConnect(otUdpSocket *udp_socket)
{
    esp_err_t err = ESP_OK;
    udp_bind_connect_task_t task = {
        .pcb = (struct udp_pcb *)udp_socket->mHandle,
        .port = udp_socket->mPeerName.mPort,
    };

    task.addr = map_openthread_addr_to_lwip_addr(&udp_socket->mPeerName.mAddress);
    if (ip_addr_isany_val(task.addr) && task.port == 0) {
        return OT_ERROR_NONE;
    }
    esp_openthread_task_switching_lock_release();
    err = esp_netif_tcpip_exec(udp_connect_task, &task);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);

    return err == ESP_OK ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

static bool is_link_local(const otIp6Address *address)
{
    return address->mFields.m8[0] == 0xfe && address->mFields.m8[1] == 0x80;
}

static bool is_multicast(const otIp6Address *address)
{
    return address->mFields.m8[0] == 0xff;
}

static void udp_send_task(void *ctx)
{
    err_t err = ERR_OK;
    udp_send_task_t *task = (udp_send_task_t *)ctx;
    struct pbuf *send_buf = NULL;
    uint16_t len = otMessageGetLength(task->message);

    task->pcb->ttl = task->hop_limit;
    task->pcb->netif_idx = task->netif_index;
#if LWIP_IPV6_SCOPES
#if CONFIG_LWIP_IPV4
    if (task->peer_addr.type == IPADDR_TYPE_V6)
#endif
    {
        ip_2_ip6(&task->peer_addr)->zone = task->netif_index;
    }
#if CONFIG_LWIP_IPV4
    if (task->source_addr.type == IPADDR_TYPE_V6)
#endif
    {
        ip_2_ip6(&task->source_addr)->zone = task->netif_index;
    }
#endif
    task->pcb->flags = (task->pcb->flags & (~UDP_FLAGS_MULTICAST_LOOP));
    task->pcb->local_ip = task->source_addr;
    task->pcb->local_port = task->source_port;
    if (task->multicast_loop) {
        task->pcb->flags |= UDP_FLAGS_MULTICAST_LOOP;
    }
    send_buf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    VerifyOrExit(send_buf != NULL);
    otMessageRead(task->message, 0, send_buf->payload, len);

    if (task->netif_index == get_netif_index(OT_NETIF_THREAD)) {
        // If the input arguments indicated the netif is OT, directly send the message.
        err = udp_sendto_if_src(task->pcb, send_buf, &task->peer_addr, task->peer_port, netif_get_by_index(task->netif_index), &task->source_addr);
    } else {
        // Otherwise, let Lwip to determine which netif will be used.
        err = udp_sendto(task->pcb, send_buf, &task->peer_addr, task->peer_port);
    }

exit:
    if (send_buf) {
        pbuf_free(send_buf);
    }
    if (err != ERR_OK) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to Send UDP message, err: %d", err);
    }
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
    otMessageFree(task->message);
    esp_openthread_task_switching_lock_release();
    free(task);
}

static inline bool is_addr_ip6_any(const ip_addr_t *addr)
{
    return ip_2_ip6(addr)->addr[0] == 0 && ip_2_ip6(addr)->addr[1] == 0 && ip_2_ip6(addr)->addr[2] == 0 && ip_2_ip6(addr)->addr[3] == 0
#if CONFIG_LWIP_IPV4
        && addr->type == IPADDR_TYPE_V6
#endif
        ;
}

otError otPlatUdpSend(otUdpSocket *udp_socket, otMessage *message, const otMessageInfo *message_info)
{
    udp_send_task_t *task = (udp_send_task_t *)malloc(sizeof(udp_send_task_t));
    otError error = OT_ERROR_NONE;
    VerifyOrExit(task != NULL, error = OT_ERROR_NO_BUFS);
    task->pcb = (struct udp_pcb *)udp_socket->mHandle;
    task->message = message;
    task->source_port = message_info->mSockPort;
    task->peer_port = message_info->mPeerPort;
    task->multicast_loop = message_info->mMulticastLoop;
    task->hop_limit = message_info->mHopLimit ? message_info->mHopLimit : UDP_TTL;
    task->netif_index = NETIF_NO_INDEX;
    task->source_addr = map_openthread_addr_to_lwip_addr(&message_info->mSockAddr);
    task->peer_addr = map_openthread_addr_to_lwip_addr(&message_info->mPeerAddr);
#if CONFIG_LWIP_IPV4
    if (task->peer_addr.type == IPADDR_TYPE_V4 && is_addr_ip6_any(&task->source_addr)) {
        task->source_addr.type = IPADDR_TYPE_ANY;
    }
#endif

    if (is_link_local(&message_info->mPeerAddr) || is_multicast(&message_info->mPeerAddr)) {
        task->netif_index = get_netif_index(message_info->mIsHostInterface ? OT_NETIF_BACKBONE : OT_NETIF_THREAD);
    }

    if (is_openthread_internal_mesh_local_addr(&message_info->mPeerAddr)) {
        // If the destination address is a openthread mesh local address, set the netif OT.
        task->netif_index = get_netif_index(OT_NETIF_THREAD);
    }
    esp_openthread_task_switching_lock_release();
    tcpip_callback(udp_send_task, task);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);

exit:
    return error;
}

static void udp_multicast_join_leave_task(void *ctx)
{
    udp_multicast_join_leave_task_t *task = (udp_multicast_join_leave_task_t *)ctx;
    struct netif *target = netif_get_by_index(task->netif_index);

    if (target == NULL) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to %s multicast group, index%d netif is not ready",
                task->is_join ? "join" : "leave", task->netif_index);
    } else {
        if (task->is_join) {
            if (mld6_joingroup_netif(target, &task->addr) != ERR_OK) {
                ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to join multicast group");
            }
        } else {
            if (mld6_leavegroup_netif(target, &task->addr) != ERR_OK) {
                ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to leave multicast group");
            }
        }
    }
    free(task);
}

otError otPlatUdpJoinMulticastGroup(otUdpSocket *socket, otNetifIdentifier netif_id, const otIp6Address *addr)
{
    udp_multicast_join_leave_task_t *task =
        (udp_multicast_join_leave_task_t *)malloc(sizeof(udp_multicast_join_leave_task_t));
    otError error = OT_ERROR_NONE;

    VerifyOrExit(task != NULL, error = OT_ERROR_NO_BUFS);
    task->is_join = true;
    task->netif_index = get_netif_index(netif_id);
    task->addr.zone = task->netif_index;
    memcpy(task->addr.addr, addr->mFields.m8, sizeof(task->addr.addr));
    esp_openthread_task_switching_lock_release();
    tcpip_callback(udp_multicast_join_leave_task, task);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);

exit:
    return error;
}

otError otPlatUdpLeaveMulticastGroup(otUdpSocket *socket, otNetifIdentifier netif_id, const otIp6Address *addr)
{
    udp_multicast_join_leave_task_t *task =
        (udp_multicast_join_leave_task_t *)malloc(sizeof(udp_multicast_join_leave_task_t));
    otError error = OT_ERROR_NONE;

    VerifyOrExit(task != NULL, error = OT_ERROR_NO_BUFS);
    task->is_join = false;
    task->netif_index = get_netif_index(netif_id);
    task->addr.zone = task->netif_index;
    memcpy(task->addr.addr, addr->mFields.m8, sizeof(task->addr.addr));
    esp_openthread_task_switching_lock_release();
    tcpip_callback(udp_multicast_join_leave_task, task);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);

exit:
    return error;
}
