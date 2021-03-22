// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the License

#include "esp_openthread_netif.h"

#include <stdio.h>
#include <string.h>

#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "esp_err.h"
#include "esp_openthread.h"
#include "esp_vfs_eventfd.h"
#include "freertos/FreeRTOS.h"
#include "lwip/err.h"
#include "lwip/ip.h"
#include "lwip/ip6.h"
#include "lwip/mld6.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/tcpip.h"
#include "openthread/error.h"
#include "openthread/icmp6.h"
#include "openthread/instance.h"
#include "openthread/ip6.h"
#include "openthread/link.h"
#include "openthread/message.h"
#include "openthread/thread.h"

typedef struct {
    otIp6AddressInfo address_info;
    bool is_added;
} thread_netif_address_task_t;

static int s_netif_event_fd = -1;
static struct netif s_thread_netif;
QueueHandle_t s_packet_queue;

#define NETIF_OUTPUT_SIGNAL 1

#ifndef OPENTHREAD_NETIF_QUEUE_SIZE
#define OPENTHREAD_NETIF_QUEUE_SIZE CONFIG_OPENTHREAD_NETIF_QUEUE_SIZE
#endif

static esp_err_t notify_packets_pending(void)
{
    uint64_t signal = NETIF_OUTPUT_SIGNAL;
    ssize_t ret = write(s_netif_event_fd, &signal, sizeof(signal));
    if (ret != sizeof(signal)) {
        otLogWarnPlat("Thread netif failed to notify eventfd");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static err_t netif_output_ip6(struct netif *interface, struct pbuf *buffer, const struct ip6_addr *peer_addr)
{
    err_t error = ERR_OK;
    struct pbuf *copy_buf = pbuf_alloc(PBUF_LINK, buffer->tot_len, PBUF_POOL);

    otLogDebgPlat("ThreadNetif send to %s", ip6addr_ntoa(peer_addr));
    VerifyOrExit(copy_buf != NULL, error = ERR_MEM);
    SuccessOrExit(error = pbuf_copy(copy_buf, buffer));
    if (xQueueSend(s_packet_queue, &copy_buf, 0) != pdTRUE) {
        otLogCritPlat("Failed to send to Thread netif: packet queue full");
        ExitNow(error = ERR_MEM);
    }
    copy_buf = NULL;
    VerifyOrExit(notify_packets_pending() == ESP_OK, error = ERR_IF);

exit:
    if (copy_buf != NULL) {
        pbuf_free(copy_buf);
    }
    return error;
}

static err_t thread_netif_init(struct netif *interface)
{
    interface->name[0]    = 'o';
    interface->name[1]    = 't';
    interface->hwaddr_len = sizeof(otExtAddress);
    memset(interface->hwaddr, 0, sizeof(interface->hwaddr));
    interface->mtu   = OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH;
    interface->flags = NETIF_FLAG_BROADCAST;
    interface->output = NULL;
    interface->output_ip6 = netif_output_ip6;
    interface->num = 0;

    return ERR_OK;
}

static bool is_link_local(const ip6_addr_t *address)
{
    const uint8_t *val = (const uint8_t *)(&address);
    return val[0] == 0xfe && val[1] == 0x80;
}

static otError add_multicast_address(const ip6_addr_t *addres)
{
    otError error = OT_ERROR_NONE;

    if (mld6_joingroup_netif(&s_thread_netif, addres) != ERR_OK) {
        error = OT_ERROR_FAILED;
    }

    return error;
}

static otError add_address(const ip6_addr_t *address)
{
    otError error = OT_ERROR_NONE;
    err_t   err   = ERR_OK;


    if (is_link_local(address)) {
        netif_ip6_addr_set(&s_thread_netif, 0, address);
        netif_ip6_addr_set_state(&s_thread_netif, 0, IP6_ADDR_PREFERRED);
    } else {
        int8_t                   index  = -1;
        const otMeshLocalPrefix *prefix = otThreadGetMeshLocalPrefix(esp_openthread_get_instance());

        err = netif_add_ip6_address(&s_thread_netif, address, &index);
        VerifyOrExit(err == ERR_OK && index != -1, error = OT_ERROR_FAILED);
        if (memcmp(address, prefix, sizeof(prefix->m8)) != 0) {
            netif_ip6_addr_set_state(&s_thread_netif, index, IP6_ADDR_PREFERRED);
        } else {
            netif_ip6_addr_set_state(&s_thread_netif, index, IP6_ADDR_VALID);
        }
    }

exit:
    return error;
}

static otError remove_multicast_address(const ip6_addr_t *address)
{
    otError error = OT_ERROR_NONE;

    if (mld6_leavegroup_netif(&s_thread_netif, address) != ERR_OK) {
        error = OT_ERROR_FAILED;
    }

    return error;
}

static otError remove_address(const ip6_addr_t *address)
{
    int8_t index;
    otError error = OT_ERROR_NONE;

    index = netif_get_ip6_addr_match(&s_thread_netif, address);
    VerifyOrExit(index != -1, error = OT_ERROR_NOT_FOUND);
    netif_ip6_addr_set_state(&s_thread_netif, index, IP6_ADDR_INVALID);

exit:
    return error;
}


static void process_thread_address_task(void *ctx)
{
    // All multicast addresses have prefix ff00::/8
    thread_netif_address_task_t *task = (thread_netif_address_task_t *)ctx;
    bool is_multicast = (task->address_info.mAddress->mFields.m8[0] == 0xff);
    ip6_addr_t address;
    otError error = OT_ERROR_NONE;

    memset(&address, 0, sizeof(address));
    memcpy(&address.addr, &(task->address_info.mAddress->mFields.m8), sizeof(address.addr));
    otLogInfoPlat("address %s: %s", task->is_added ? "added" : "removed", ip6addr_ntoa(&address));

    if (task->is_added) {
        if (is_multicast) {
            error = add_multicast_address(&address);
        } else {
            error = add_address(&address);
        }
    } else {
        if (is_multicast) {
            error = remove_multicast_address(&address);
        } else {
            error = remove_address(&address);
        }
    }

    if (error != OT_ERROR_NONE) {
        otLogCritPlat("Failed to process address: %s", otThreadErrorToString(error));
    }
    free(task);
}

void process_thread_address(const otIp6AddressInfo *address_info, bool is_added, void *context)
{
    thread_netif_address_task_t *task = malloc(sizeof(thread_netif_address_task_t));

    task->address_info = *address_info;
    task->is_added = is_added;

    if (task == NULL) {
        otLogCritPlat("Cannot allocate address task");
    }
    if (tcpip_callback(process_thread_address_task, task) != ERR_OK) {
        free(task);
    }
}

static void netif_input_task(void *ctx)
{
    struct pbuf *buffer = (struct pbuf *)ctx;
    if (s_thread_netif.input(buffer, &s_thread_netif) != ERR_OK) {
        otLogCritPlat("Failed to send OpenThread message to lwIP network interface");
        pbuf_free(buffer);
    }
}

static void process_thread_receive(otMessage *message, void *context)
{
    otError      error      = OT_ERROR_NONE;
    err_t        err        = ERR_OK;
    const size_t kBlockSize = 128;
    uint16_t     length     = otMessageGetLength(message);
    struct pbuf *buffer     = NULL;
    otInstance *instance  = (otInstance *)context;

    assert(s_thread_netif.state == instance);

    buffer = pbuf_alloc(PBUF_LINK, length, PBUF_POOL);

    VerifyOrExit(buffer != NULL, error = OT_ERROR_NO_BUFS);

    for (uint16_t i = 0; i < length; i += kBlockSize) {
        uint8_t block[kBlockSize];
        int     count = otMessageRead(message, i, block, sizeof(block));

        assert(count > 0);
        err = pbuf_take_at(buffer, block, count, i);
        VerifyOrExit(err == ERR_OK, error = OT_ERROR_FAILED);
    }

    err = tcpip_callback(netif_input_task, buffer);
    VerifyOrExit(err == ERR_OK, error = OT_ERROR_FAILED);
    buffer = NULL;

exit:
    if (error != OT_ERROR_NONE) {
        if (buffer != NULL) {
            pbuf_free(buffer);
        }

        if (error == OT_ERROR_FAILED) {
            otLogWarnPlat("%s failed for lwip error %d", __func__, err);
        }

        otLogWarnPlat("%s failed: %s", __func__, otThreadErrorToString(error));
    }

    otMessageFree(message);
}

static esp_err_t transmit_pbuf(struct pbuf *buffer)
{
    otMessage *message = otIp6NewMessage(esp_openthread_get_instance(), NULL);
    esp_err_t error = ESP_OK;
    otError ot_error = OT_ERROR_NONE;

    if (message == NULL) {
        otLogCritPlat("Failed to allocate OpenThread message");
        ExitNow(error = ESP_ERR_NO_MEM);
    }
    for (struct pbuf *p = buffer; p != NULL; p = p->next) {
        ot_error = otMessageAppend(message, p->payload, p->len);
        if (ot_error != OT_ERROR_NONE) {
            otLogCritPlat("Failed to copy to OpenThread message: %s", otThreadErrorToString(ot_error));
            error = ESP_ERR_NO_MEM;
            ExitNow();
        }

    }
    ot_error = otIp6Send(esp_openthread_get_instance(), message);
    message = NULL;
    if (ot_error != OT_ERROR_NONE && ot_error != OT_ERROR_DROP) {
        otLogWarnPlat("ThreadNetif Failed to send OpenThread IP6 message: %s", otThreadErrorToString(ot_error));
        if (ot_error == OT_ERROR_NO_BUFS) {
            otLogInfoPlat("Try to retransmit message %p", buffer);
            if (xQueueSendToFront(s_packet_queue, &buffer, 0) != pdTRUE) {
                otLogWarnPlat("ThreadNetif: No buf, drop packet");
                error = ESP_ERR_NO_MEM;
                buffer = NULL;
            }
        } else {
            error = ESP_FAIL;
        }
    }
    if (ot_error == OT_ERROR_DROP) {
        // OpenThread will intentionally drop some multicast and ICMPv6 packets
        // which are not required for the Thread network.
        otLogDebgPlat("OpenThread stack filtered netif packet");
    }

exit:
    if (buffer != NULL) {
        pbuf_free(buffer);
    }
    if (message != NULL) {
        otMessageFree(message);
    }
    return error;
}

static esp_err_t process_thread_transmit(otInstance *instance)
{
    struct pbuf *buffer = NULL;
    esp_err_t error = OT_ERROR_NONE;
    uint64_t event;

    read(s_netif_event_fd, &event, sizeof(event));
    while (xQueueReceive(s_packet_queue, &buffer, 0) == pdTRUE) {
        if (buffer) {
            error = transmit_pbuf(buffer);
        }
        if (error != OT_ERROR_NONE) {
            break;
        }
    }

    if (uxQueueMessagesWaiting(s_packet_queue) > 0) {
        error = notify_packets_pending();
    }

    return error;
}

static void netif_up_task(void *ctx)
{
    netif_set_up(&s_thread_netif);
}

static void netif_down_task(void *ctx)
{
    netif_set_down(&s_thread_netif);
}

static void process_thread_state(otChangedFlags changed_flags, void *context)
{
    otInstance *instance = (otInstance *)context;
    err_t err = ERR_OK;

    if (OT_CHANGED_THREAD_NETIF_STATE & changed_flags) {
        if (otLinkIsEnabled(instance)) {
            otLogInfoPlat("netif up");
            err = tcpip_callback(netif_up_task, NULL);
        } else {
            otLogInfoPlat("netif down");
            err = tcpip_callback(netif_down_task, NULL);
        }
    }

    if (err != ERR_OK) {
        otLogCritPlat("Faile to configure netif state");
    }
}

esp_err_t esp_openthread_netif_init(void)
{
    otInstance *instance = esp_openthread_get_instance();
    otError ot_err;
    esp_err_t error = ESP_OK;

    if (instance == NULL || s_packet_queue || s_thread_netif.state || s_netif_event_fd >= 0) {
        return ESP_ERR_INVALID_STATE;
    }

    memset(&s_thread_netif, 0, sizeof(s_thread_netif));
    if (netif_add(&s_thread_netif, NULL, NULL, NULL, instance, thread_netif_init, tcpip_input) == NULL) {
        otLogCritPlat("Failed to add Thread netif");
        ExitNow(error = ESP_FAIL);
    }
    netif_set_link_up(&s_thread_netif);
    s_packet_queue = xQueueCreate(OPENTHREAD_NETIF_QUEUE_SIZE, sizeof(void *));
    if (s_packet_queue == NULL) {
        otLogCritPlat("Failed to allocate Thread netif packet queue");
        ExitNow(error = ESP_ERR_NO_MEM);
    }

    otIp6SetAddressCallback(instance, process_thread_address, instance);
    otIp6SetReceiveCallback(instance, process_thread_receive, instance);
    ot_err = otSetStateChangedCallback(instance, process_thread_state, instance);
    if (ot_err != OT_ERROR_NONE) {
        otLogCritPlat("Failed to register callback for OpenThread lwip interface: %s", otThreadErrorToString(ot_err));
        ExitNow(error = ESP_FAIL);
    }
    otIp6SetReceiveFilterEnabled(instance, true);
    otIcmp6SetEchoMode(instance, OT_ICMP6_ECHO_HANDLER_DISABLED);

    s_netif_event_fd = eventfd(0, 0);
    if (s_netif_event_fd < 0) {
        otLogCritPlat("Failed to create event fd for Thread netif");
        ExitNow(error = ESP_FAIL);
    }

exit:
    if (error != ESP_OK) {
        esp_openthread_netif_deinit();
    }

    return error;
}

void esp_openthread_netif_deinit(void)
{
    otInstance *instance = esp_openthread_get_instance();
    otRemoveStateChangeCallback(instance, process_thread_state, instance);
    if (s_packet_queue) {
        vQueueDelete(s_packet_queue);
        s_packet_queue = NULL;
    }
    if (s_thread_netif.state) {
        netif_remove(&s_thread_netif);
    }
    if (s_netif_event_fd >= 0) {
        close(s_netif_event_fd);
    }
}

void esp_openthread_netif_update(esp_openthread_mainloop_context_t *mainloop)
{
    FD_SET(s_netif_event_fd, &mainloop->read_fds);
    if (s_netif_event_fd > mainloop->max_fd) {
        mainloop->max_fd = s_netif_event_fd;
    }
}

esp_err_t esp_openthread_netif_process(otInstance *instance, const esp_openthread_mainloop_context_t *context)
{
    if (FD_ISSET(s_netif_event_fd, &context->read_fds)) {
        return process_thread_transmit(instance);
    } else {
        return ESP_OK;
    }
}
