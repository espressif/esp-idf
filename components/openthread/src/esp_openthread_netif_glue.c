/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_netif_glue.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_openthread.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue_priv.h"
#include "esp_openthread_platform.h"
#include "esp_openthread_state.h"
#include "esp_openthread_types.h"
#include "esp_vfs_eventfd.h"
#include "sdkconfig.h"
#include "common/code_utils.hpp"
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
static esp_netif_t *s_openthread_netif;
static const char *netif_glue_workflow = "netif_glue";
const esp_netif_inherent_config_t g_esp_netif_inherent_openthread_config = ESP_NETIF_INHERENT_DEFAULT_OPENTHREAD();

#define NETIF_OUTPUT_SIGNAL 1

static bool is_link_local_addr(const otIp6Address *address)
{
    return address->mFields.m32[0] == 0xfe800000 && address->mFields.m32[1] == 0;
}

static bool is_mesh_local_addr(const otIp6Address *address)
{
    return memcmp(address->mFields.m8, otThreadGetMeshLocalPrefix(esp_openthread_get_instance())->m8,
                  sizeof(otThreadGetMeshLocalPrefix(esp_openthread_get_instance())->m8)) == 0;
}

static bool is_addr_locator(const otIp6Address *address)
{
    return address->mFields.m32[2] == 0xff000000 && address->mFields.m16[6] == 0xfe;
}

bool is_openthread_internal_mesh_local_addr(const otIp6Address *address)
{
    return is_addr_locator(address) && is_mesh_local_addr(address);
}

static esp_err_t notify_packets_pending(void)
{
    uint64_t signal = NETIF_OUTPUT_SIGNAL;
    ssize_t ret = write(s_openthread_netif_glue.event_fd, &signal, sizeof(signal));
    if (ret != sizeof(signal)) {
        ESP_LOGW(OT_PLAT_LOG_TAG, "Thread netif failed to notify eventfd");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void process_thread_address(const otIp6AddressInfo *address_info, bool is_added, void *context)
{
    bool is_multicast = address_info->mAddress->mFields.m8[0] == 0xff;
    esp_ip6_addr_t addr;

    if (is_openthread_internal_mesh_local_addr(address_info->mAddress)) {
        return;
    }

    memcpy(addr.addr, address_info->mAddress->mFields.m8, sizeof(addr.addr));
    if (is_added) {
        if (is_multicast) {
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_MULTICAST_GROUP_JOIN, &addr, sizeof(addr), 0) !=
                ESP_OK) {
                ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to post OpenThread join multicast group event");
            }
        } else {
            ip_event_add_ip6_t add_addr;
            add_addr.addr = addr;
            // if an address is not mesh local or link local, we set preferred for this address.
            add_addr.preferred =
                is_mesh_local_addr(address_info->mAddress) || is_link_local_addr(address_info->mAddress) ? 0 : 1;
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_GOT_IP6, &add_addr, sizeof(add_addr), 0) != ESP_OK) {
                ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to post OpenThread got ip6 address event");
            }
        }
    } else {
        if (is_multicast) {
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_MULTICAST_GROUP_LEAVE, &addr, sizeof(addr), 0) !=
                ESP_OK) {
                ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to post OpenThread leave multicast group event");
            }
        } else {
            if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_LOST_IP6, &addr, sizeof(addr), 0) != ESP_OK) {
                ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to post OpenThread lost ip6 address event");
            }
        }
    }
}

static void process_thread_receive(otMessage *message, void *context)
{
    esp_err_t error;
    uint16_t length = otMessageGetLength(message);

    error = esp_netif_receive(s_openthread_netif_glue.base.netif, message, length, NULL);

    if (error != ESP_OK) {
        ESP_LOGW(OT_PLAT_LOG_TAG, "process_thread_receive failed: %s", esp_err_to_name(error));
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
                ESP_LOGW(OT_PLAT_LOG_TAG, "ThreadNetif Failed to send OpenThread IP6 message: %s",
                         otThreadErrorToString(ot_error));
            }
            if (ot_error == OT_ERROR_DROP) {
                // OpenThread will intentionally drop some multicast and ICMPv6 packets
                // which are not required for the Thread network.
                ESP_LOGD(OT_PLAT_LOG_TAG, "OpenThread stack filtered netif packet");
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

static esp_err_t openthread_netif_transmit(void *handle, void *buffer, size_t len)
{
    esp_err_t error = ESP_OK;
    otError ot_error = OT_ERROR_NONE;

    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
    otMessage *message = otIp6NewMessage(esp_openthread_get_instance(), NULL);
    if (message == NULL) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to allocate OpenThread message");
        ExitNow(error = ESP_ERR_NO_MEM);
    }

    ot_error = otMessageAppend(message, buffer, len);
    if (ot_error != OT_ERROR_NONE) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to copy to OpenThread message: %s", otThreadErrorToString(ot_error));
        ExitNow(error = ESP_ERR_NO_MEM);
    }

    if (xQueueSend(s_packet_queue, &message, 0) != pdTRUE) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to send to Thread netif: packet queue full");
        ExitNow(error = ESP_ERR_NO_MEM, ot_error = OT_ERROR_NO_BUFS);
    }
    VerifyOrExit(notify_packets_pending() == ESP_OK, error = ESP_FAIL);

exit:
    if (ot_error != OT_ERROR_NONE && message != NULL) {
        otMessageFree(message);
    }
    esp_openthread_task_switching_lock_release();
    return error;
}

static esp_event_handler_t meshcop_e_publish_handler = NULL;
static void esp_openthread_meshcop_e_publish_handler(void *args, esp_event_base_t base, int32_t event_id, void *data)
{
    if (meshcop_e_publish_handler) {
        meshcop_e_publish_handler(args, base, event_id, data);
    }
}

static esp_event_handler_t meshcop_e_remove_handler = NULL;
static void esp_openthread_meshcop_e_remove_handler(void *args, esp_event_base_t base, int32_t event_id, void *data)
{
    if (meshcop_e_remove_handler) {
        meshcop_e_remove_handler(args, base, event_id, data);
    }
}

void esp_openthread_register_meshcop_e_handler(esp_event_handler_t handler, bool for_publish)
{
    if (for_publish) {
        meshcop_e_publish_handler = handler;
    } else if (!for_publish) {
        meshcop_e_remove_handler = handler;
    } else {
        ESP_ERROR_CHECK(ESP_FAIL);
    }
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
    ESP_RETURN_ON_ERROR(esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_PUBLISH_MESHCOP_E,
                                                   esp_openthread_meshcop_e_publish_handler, NULL),
                        OT_PLAT_LOG_TAG, "OpenThread publish meshcop-e service event register failed");
    ESP_RETURN_ON_ERROR(esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_REMOVE_MESHCOP_E,
                                                   esp_openthread_meshcop_e_remove_handler, NULL),
                        OT_PLAT_LOG_TAG, "OpenThread remove meshcop-e service event register failed");
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
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_PUBLISH_MESHCOP_E, esp_openthread_meshcop_e_publish_handler);
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_REMOVE_MESHCOP_E, esp_openthread_meshcop_e_remove_handler);
}

static esp_err_t openthread_netif_post_attach(esp_netif_t *esp_netif, void *args)
{
    esp_netif_driver_base_t *base = (esp_netif_driver_base_t *)args;
    base->netif = esp_netif;

    // set driver related config to esp-netif
    esp_netif_driver_ifconfig_t driver_ifconfig = {
        .handle = &s_openthread_netif_glue, .transmit = openthread_netif_transmit, .driver_free_rx_buffer = NULL};

    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));

    ESP_LOGI(OT_PLAT_LOG_TAG, "OpenThread attached to netif");
    esp_err_t error = register_openthread_event_handlers(esp_netif);
    s_openthread_netif = esp_netif;
    if (error == ESP_OK) {
        error = esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_START, NULL, 0, 0);
    }

    return error;
}

void *esp_openthread_netif_glue_init(const esp_openthread_platform_config_t *config)
{
    otInstance *instance = esp_openthread_get_instance();
    esp_err_t error = ESP_OK;

    if (instance == NULL || s_packet_queue || s_openthread_netif_glue.event_fd >= 0) {
        return NULL;
    }

    s_packet_queue = xQueueCreate(config->port_config.netif_queue_size, sizeof(otMessage *));
    if (s_packet_queue == NULL) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to allocate Thread netif packet queue");
        ExitNow(error = ESP_ERR_NO_MEM);
    }

    otIp6SetAddressCallback(instance, process_thread_address, instance);
    otIp6SetReceiveCallback(instance, process_thread_receive, instance);
    otIp6SetReceiveFilterEnabled(instance, true);
    otIcmp6SetEchoMode(instance, OT_ICMP6_ECHO_HANDLER_DISABLED);

    s_openthread_netif_glue.event_fd = eventfd(0, 0);
    if (s_openthread_netif_glue.event_fd < 0) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to create event fd for Thread netif");
        ExitNow(error = ESP_FAIL);
    }
    s_openthread_netif_glue.base.post_attach = openthread_netif_post_attach;
    error = esp_openthread_platform_workflow_register(&esp_openthread_netif_glue_update,
                                                      &esp_openthread_netif_glue_process, netif_glue_workflow);
exit:
    if (error != ESP_OK) {
        return NULL;
    }

    return &s_openthread_netif_glue.base;
}

void esp_openthread_netif_glue_deinit(void)
{
    otInstance *instance = esp_openthread_get_instance();
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
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to stop OpenThread netif");
    }
    s_openthread_netif = NULL;
    unregister_openthread_event_handlers();
    esp_openthread_platform_workflow_unregister(netif_glue_workflow);
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

esp_netif_t *esp_openthread_get_netif(void)
{
    return s_openthread_netif;
}
