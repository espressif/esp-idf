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

#include "esp_openthread_netif_glue.h"

#include <stdio.h>
#include <string.h>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_openthread.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_lock.h"
#include "esp_vfs_eventfd.h"
#include "sdkconfig.h"
#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "config/link_quality.h"
#include "freertos/FreeRTOS.h"
#include "openthread/error.h"
#include "openthread/icmp6.h"
#include "openthread/instance.h"
#include "openthread/ip6.h"
#include "openthread/link.h"
#include "openthread/message.h"
#include "openthread/thread.h"

typedef struct {
    esp_netif_driver_base_t base;
    int event_fd;
} esp_openthread_netif_glue_t;

static esp_openthread_netif_glue_t s_openthread_netif_glue = {
    .event_fd = -1,
};

ESP_EVENT_DEFINE_BASE(OPENTHREAD_EVENT);

static QueueHandle_t s_packet_queue;

#define NETIF_OUTPUT_SIGNAL 1

static esp_err_t notify_packets_pending(void)
{
    uint64_t signal = NETIF_OUTPUT_SIGNAL;
    ssize_t ret = write(s_openthread_netif_glue.event_fd, &signal, sizeof(signal));
    if (ret != sizeof(signal)) {
        otLogWarnPlat("Thread netif failed to notify eventfd");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void process_thread_address(const otIp6AddressInfo *address_info, bool is_added, void *context)
{
    bool is_multicast = address_info->mAddress->mFields.m8[0] == 0xff;
    esp_ip6_addr_t addr;

    memcpy(addr.addr, address_info->mAddress->mFields.m8, sizeof(addr.addr));
    if (is_added) {
        if (is_multicast) {
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_MULTICAST_GROUP_JOIN, &addr, sizeof(addr), 0) !=
                    ESP_OK) {
                otLogCritPlat("Failed to post OpenThread join multicast group event");
            }
        } else {
            ip_event_add_ip6_t add_addr;
            add_addr.addr = addr;
            add_addr.preferred = address_info->mPreferred;
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_GOT_IP6, &add_addr, sizeof(add_addr), 0) != ESP_OK) {
                otLogCritPlat("Failed to post OpenThread got ip6 address event");
            }
        }
    } else {
        if (is_multicast) {
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_MULTICAST_GROUP_LEAVE, &addr, sizeof(addr), 0) !=
                    ESP_OK) {
                otLogCritPlat("Failed to post OpenThread leave multicast group event");
            }
        } else {
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_LOST_IP6, &addr, sizeof(addr), 0) != ESP_OK) {
                otLogCritPlat("Failed to post OpenThread lost ip6 address event");
            }
        }
    }
}

static void process_thread_receive(otMessage *message, void *context)
{
    esp_err_t error;
    uint16_t length = otMessageGetLength(message);
    uint8_t *buffer = malloc(length);

    VerifyOrExit(buffer != NULL, error = OT_ERROR_NO_BUFS);
    uint16_t read_length = otMessageRead(message, 0, buffer, length);
    assert(read_length == length);

    error = esp_netif_receive(s_openthread_netif_glue.base.netif, buffer, length, NULL);
    buffer = NULL;

exit:
    if (error != ESP_OK) {
        otLogWarnPlat("process_thread_receive failed: %s", esp_err_to_name(error));
    }

    otMessageFree(message);
}

static esp_err_t process_thread_transmit(otInstance *instance)
{
    otMessage *msg = NULL;
    esp_err_t error = ESP_OK;
    uint64_t event;

    int ret = read(s_openthread_netif_glue.event_fd, &event, sizeof(event));
    assert(ret == sizeof(event));
    while (xQueueReceive(s_packet_queue, &msg, 0) == pdTRUE) {
        if (msg) {
            otError ot_error = otIp6Send(esp_openthread_get_instance(), msg);
            if (ot_error != OT_ERROR_NONE && ot_error != OT_ERROR_DROP) {
                otLogWarnPlat("ThreadNetif Failed to send OpenThread IP6 message: %s", otThreadErrorToString(ot_error));
            }
            if (ot_error == OT_ERROR_DROP) {
                // OpenThread will intentionally drop some multicast and ICMPv6 packets
                // which are not required for the Thread network.
                otLogDebgPlat("OpenThread stack filtered netif packet");
            }
            if (ot_error != OT_ERROR_NONE) {
                break;
            }
        }
    }

    if (uxQueueMessagesWaiting(s_packet_queue) > 0) {
        error = notify_packets_pending();
    }

    return error;
}

static void process_thread_state(otChangedFlags changed_flags, void *context)
{
    otInstance *instance = (otInstance *)context;
    esp_err_t err = ESP_OK;

    if (OT_CHANGED_THREAD_NETIF_STATE & changed_flags) {
        if (otLinkIsEnabled(instance)) {
            otLogInfoPlat("netif up");
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_IF_UP, NULL, 0, 0) != ESP_OK) {
                otLogCritPlat("Failed to post OpenThread if up event");
            }
        } else {
            otLogInfoPlat("netif down");
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_IF_DOWN, NULL, 0, 0) != ESP_OK) {
                otLogCritPlat("Failed to post OpenThread if down event");
            }
        }
    }

    if (err != ESP_OK) {
        otLogCritPlat("Failed to configure netif state");
    }
}

static esp_err_t openthread_netif_transmit(void *handle, void *buffer, size_t len)
{
    esp_err_t error = ESP_OK;
    otError ot_error = OT_ERROR_NONE;

    esp_openthread_lock_acquire(portMAX_DELAY);
    otMessage *message = otIp6NewMessage(esp_openthread_get_instance(), NULL);
    if (message == NULL) {
        otLogCritPlat("Failed to allocate OpenThread message");
        ExitNow(error = ESP_ERR_NO_MEM);
    }

    ot_error = otMessageAppend(message, buffer, len);
    if (ot_error != OT_ERROR_NONE) {
        otLogCritPlat("Failed to copy to OpenThread message: %s", otThreadErrorToString(ot_error));
        ExitNow(error = ESP_ERR_NO_MEM);
    }

    if (xQueueSend(s_packet_queue, &message, 0) != pdTRUE) {
        otLogCritPlat("Failed to send to Thread netif: packet queue full");
        ExitNow(error = ESP_ERR_NO_MEM);
    }
    VerifyOrExit(notify_packets_pending() == ESP_OK, error = ESP_FAIL);

exit:
    if (ot_error != OT_ERROR_NONE && message != NULL) {
        otMessageFree(message);
    }
    esp_openthread_lock_release();
    return error;
}

static esp_err_t register_openthread_event_handlers(esp_netif_t *esp_netif)
{
    ESP_RETURN_ON_ERROR(
        esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_START, esp_netif_action_start, esp_netif),
        OT_PLAT_LOG_TAG, "OpenThread start event register failed");
    ESP_RETURN_ON_ERROR(
        esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_STOP, esp_netif_action_stop, esp_netif),
        OT_PLAT_LOG_TAG, "OpenThread stop event register failed");
    ESP_RETURN_ON_ERROR(
        esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_IF_UP, esp_netif_action_connected, esp_netif),
        OT_PLAT_LOG_TAG, "OpenThread interface up event register failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_IF_DOWN,
                        esp_netif_action_disconnected, esp_netif),
                        OT_PLAT_LOG_TAG, "OpenThread interface down event register failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_GOT_IP6,
                        esp_netif_action_add_ip6_address, esp_netif),
                        OT_PLAT_LOG_TAG, "OpenThread interface got ip6 event register failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_LOST_IP6,
                        esp_netif_action_remove_ip6_address, esp_netif),
                        OT_PLAT_LOG_TAG, "OpenThread interface remove ip6 event register failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_MULTICAST_GROUP_JOIN,
                        esp_netif_action_join_ip6_multicast_group, esp_netif),
                        OT_PLAT_LOG_TAG, "OpenThread interface join ip6 multicast group event register failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_MULTICAST_GROUP_LEAVE,
                        esp_netif_action_leave_ip6_multicast_group, esp_netif),
                        OT_PLAT_LOG_TAG, "OpenThread interface leave ip6 multicast group event register failed");
    return ESP_OK;
}

static void unregister_openthread_event_handlers(void)
{
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_START, esp_netif_action_start);
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_STOP, esp_netif_action_stop);
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_IF_UP, esp_netif_action_connected);
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_IF_DOWN, esp_netif_action_disconnected);
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_GOT_IP6, esp_netif_action_add_ip6_address);
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_LOST_IP6, esp_netif_action_remove_ip6_address);
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_MULTICAST_GROUP_JOIN,
                                 esp_netif_action_join_ip6_multicast_group);
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_MULTICAST_GROUP_LEAVE,
                                 esp_netif_action_leave_ip6_multicast_group);
}

static esp_err_t openthread_netif_post_attach(esp_netif_t *esp_netif, void *args)
{
    esp_netif_driver_base_t *base = (esp_netif_driver_base_t *)args;
    base->netif = esp_netif;

    // set driver related config to esp-netif
    esp_netif_driver_ifconfig_t driver_ifconfig = {
        .handle = &s_openthread_netif_glue, .transmit = openthread_netif_transmit, .driver_free_rx_buffer = NULL
    };

    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));

    otLogInfoPlat("OpenThread attached to netif");
    esp_err_t error = register_openthread_event_handlers(esp_netif);
    if (error == ESP_OK) {
        error = esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_START, NULL, 0, 0);
    }

    return error;
}

void *esp_openthread_netif_glue_init(void)
{
    otInstance *instance = esp_openthread_get_instance();
    otError ot_err;
    esp_err_t error = ESP_OK;

    if (instance == NULL || s_packet_queue || s_openthread_netif_glue.event_fd >= 0) {
        return NULL;
    }

    s_packet_queue = xQueueCreate(CONFIG_OPENTHREAD_NETIF_QUEUE_SIZE, sizeof(otMessage *));
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

    s_openthread_netif_glue.event_fd = eventfd(0, 0);
    if (s_openthread_netif_glue.event_fd < 0) {
        otLogCritPlat("Failed to create event fd for Thread netif");
        ExitNow(error = ESP_FAIL);
    }
    s_openthread_netif_glue.base.post_attach = openthread_netif_post_attach;

exit:
    if (error != ESP_OK) {
        return NULL;
    }

    return &s_openthread_netif_glue.base;
}

void esp_openthread_netif_glue_deinit(void)
{
    otInstance *instance = esp_openthread_get_instance();
    otRemoveStateChangeCallback(instance, process_thread_state, instance);
    otIp6SetAddressCallback(instance, NULL, NULL);
    otIp6SetReceiveCallback(instance, NULL, NULL);
    if (s_packet_queue) {
        vQueueDelete(s_packet_queue);
        s_packet_queue = NULL;
    }
    if (s_openthread_netif_glue.event_fd >= 0) {
        close(s_openthread_netif_glue.event_fd);
        s_openthread_netif_glue.event_fd = -1;
    }
    if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_STOP, NULL, 0, 0) != ESP_OK) {
        otLogCritPlat("Failed to stop OpenThread netif");
    }
    unregister_openthread_event_handlers();
}

void esp_openthread_netif_glue_update(esp_openthread_mainloop_context_t *mainloop)
{
    if (s_openthread_netif_glue.event_fd >= 0) {
        FD_SET(s_openthread_netif_glue.event_fd, &mainloop->read_fds);
        if (s_openthread_netif_glue.event_fd > mainloop->max_fd) {
            mainloop->max_fd = s_openthread_netif_glue.event_fd;
        }
    }
}

esp_err_t esp_openthread_netif_glue_process(otInstance *instance, const esp_openthread_mainloop_context_t *context)
{
    if (s_openthread_netif_glue.event_fd >= 0 && FD_ISSET(s_openthread_netif_glue.event_fd, &context->read_fds)) {
        return process_thread_transmit(instance);
    }
    return ESP_OK;
}
