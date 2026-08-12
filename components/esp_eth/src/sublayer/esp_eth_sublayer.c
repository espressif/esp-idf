/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/queue.h>
#include "esp_err.h"
#include "esp_eth_driver.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "esp_eth_sublayer_core.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_spec.h"
#include "hal/eth_types.h"
#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
#include "esp_private/esp_eth_sublayer_iodriver.h"
#endif
#if CONFIG_ESP_NETIF_L2_TAP
#include "esp_vfs_l2tap.h"
#endif
#if CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
/**
 * @brief Transmit timeout when multiple tasks may transmit through the same sublayer concurrently
 */
#define ESP_ETH_SUBLAYER_TX_TIMEOUT_MS   250
#endif // CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX

typedef struct esp_eth_sublayer_s esp_eth_sublayer_t;

static const char *TAG = "esp_eth.sublayer";

struct esp_eth_sublayer_s {
#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
    esp_eth_iodriver_provider_base_t base; /*!< IO driver provider base; must stay first for provider handle downcast */
#endif
    SLIST_HEAD(, esp_eth_sublayer_vlan_s) vlan_child;
#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
    esp_eth_sublayer_switch_t *switch_child;
#endif

    esp_eth_handle_t eth_driver;
    eth_speed_t link_speed;
    bool connect_confirmed;
    esp_event_handler_instance_t start_ctx_handler;
    esp_event_handler_instance_t stop_ctx_handler;
    esp_event_handler_instance_t connect_ctx_handler;
    esp_event_handler_instance_t connect_confirm_ctx_handler;
    esp_event_handler_instance_t disconnect_ctx_handler;
    esp_event_handler_instance_t disconnect_trigger_ctx_handler;
    esp_event_handler_instance_t get_ip_ctx_handler;

    esp_eth_sublayer_event_t connect_confirm;
    esp_eth_sublayer_event_t disconnect_trigger;

    esp_eth_sublayer_tx_hook_t tx_hook;
    esp_eth_sublayer_post_tx_hook_t post_tx_hook;
    esp_eth_sublayer_rx_hook_t rx_hook;
    void *hook_ctx;

#if CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX
    SemaphoreHandle_t transmit_mutex;
    TickType_t transmit_mutex_timeout_ticks;
#endif // CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX
};

void eth_sublayer_buf_free(esp_eth_sublayer_handle_t sub, void *buffer)
{
    (void)sub;
    free(buffer);
}

esp_err_t eth_sublayer_set_mac_filter(esp_eth_sublayer_handle_t sub, const uint8_t *eth_mac, size_t mac_len, bool add)
{
    esp_eth_sublayer_t *sub_impl = (esp_eth_sublayer_t *)sub;

    ESP_RETURN_ON_FALSE(mac_len == ETH_ADDR_LEN, ESP_ERR_INVALID_ARG, TAG, "invalid MAC length");
    ESP_LOGD(TAG, "%s filter MAC: %02x:%02x:%02x:%02x:%02x:%02x", add ? "Add" : "Del", eth_mac[0], eth_mac[1],
                 eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);
    if (add) {
        ESP_RETURN_ON_ERROR(esp_eth_ioctl(sub_impl->eth_driver, ETH_CMD_ADD_MAC_FILTER, (void *)eth_mac), TAG,
                            "failed to add mac filter");
    } else {
        ESP_RETURN_ON_ERROR(esp_eth_ioctl(sub_impl->eth_driver, ETH_CMD_DEL_MAC_FILTER, (void *)eth_mac), TAG,
                            "failed to delete mac filter");
    }
    return ESP_OK;
}

esp_err_t eth_sublayer_transmit(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_tx_bufs_t *tx_bufs, void *eb, int32_t port)
{
    esp_err_t ret = ESP_OK;
#if !CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
    (void)port;
#endif

#if CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX
    // Serializes the complete TX path below (TX hook, integrated switch mux, driver transmit and post-TX
    // hook) since different tasks may call this concurrently (e.g. multiple L2TAP sockets on different
    // switch/VLAN ports), and TX hooks / integrated switch drivers may keep mutable state that is only
    // safe to touch from one task at a time.
    if (xSemaphoreTake(sub->transmit_mutex, sub->transmit_mutex_timeout_ticks) == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
#endif // CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX

    if (sub->tx_hook) {
        ret = sub->tx_hook(sub->eth_driver, tx_bufs, sub->hook_ctx);
        if (ret != ESP_OK) {
            goto out_no_post_tx;
        }
    }
    // No buffers to transmit, skip the rest of the function
    if (*tx_bufs->buf_count == 0) {
        goto out_no_post_tx;
    }
    assert(*tx_bufs->buf_count <= tx_bufs->buf_capacity && "buffer count exceeds capacity");

#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
    if (sub->switch_child) {
        ret = eth_switch_mux(sub->switch_child, tx_bufs, port);
        if (ret != ESP_OK && ret != ESP_ERR_NOT_SUPPORTED) {
            goto out;
        }
    }
#endif

    ret = esp_eth_transmit_ctrl_bufs(sub->eth_driver, eb, tx_bufs->bufs, *tx_bufs->buf_count);
#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
out:
#endif
    if (sub->post_tx_hook) {
        sub->post_tx_hook(sub->eth_driver, tx_bufs, sub->hook_ctx);
    }
out_no_post_tx:
#if CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX
    xSemaphoreGive(sub->transmit_mutex);
#endif // CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX
    return ret;
}

static esp_err_t eth_sublayer_input(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv, void *info)
{
    esp_eth_sublayer_t *sub = (esp_eth_sublayer_t *)priv;
    esp_err_t ret = ESP_OK;
    (void)eth_handle;

    uint8_t *alloc_base = buffer;
    uint8_t *frame = buffer;

    esp_eth_handle_t src_eth_handle = sub->eth_driver;
#if CONFIG_ESP_NETIF_L2_TAP
    void *l2tap_io_handle = (void *)sub;
#endif
#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
    // Integrated switch demux: strip the switch tag and resolve the ingress port before
    // any further processing, so RX hook / L2 TAP / VLAN demux all operate on a clean frame.
    if (sub->switch_child) {
        int32_t src_port = -1;
        ret = eth_switch_demux(sub->switch_child, &frame, &length, &src_port);
        if (ret == ESP_OK) {
#if CONFIG_ESP_NETIF_L2_TAP
            ret = eth_switch_resolve_ingress_port(sub->switch_child, src_port, &src_eth_handle, &l2tap_io_handle);
#else
            ret = eth_switch_resolve_ingress_port(sub->switch_child, src_port, &src_eth_handle, NULL);
#endif
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "eth_switch_resolve_ingress_port failed: %d", ret);
                eth_sublayer_buf_free(sub, alloc_base);
                return ret;
            }
        } else if (ret != ESP_ERR_NOT_SUPPORTED) {
            ESP_LOGE(TAG, "eth_switch_demux failed: %d", ret);
            eth_sublayer_buf_free(sub, alloc_base);
            return ret;
        }
    }
#endif

    esp_eth_sublayer_rx_info_t rx_info = {
        .l2_buffer = alloc_base,
        .driver_info = info,
    };
    if (sub->rx_hook) {
        ret = sub->rx_hook(src_eth_handle, &frame, &length, &rx_info, sub->hook_ctx);
        if (ret == ESP_OK) {
            if (length == 0) {
                return ret;
            }
        } else {
            eth_sublayer_buf_free(sub, alloc_base);
            return ret;
        }
    }

    // low level L2TAP
#if CONFIG_ESP_NETIF_L2_TAP
    l2tap_eth_filter_info_t l2tap_info = {
        .l2_buffer = alloc_base,
        .hw_ts = (l2tap_timestamp_t *)info, // Memory layout matches the Ethernet MAC driver type (eth_mac_time_t)
    };
    ret = esp_vfs_l2tap_eth_filter_frame(l2tap_io_handle, frame, (size_t *)&length, &l2tap_info);
    if (ret == ESP_OK) {
        if (length == 0) {
            return ret;
        }
    } else {
        eth_sublayer_buf_free(sub, alloc_base);
        return ret;
    }
#endif

    uint16_t vlan_id = eth_vlan_get_ether_type(frame);
    esp_eth_sublayer_vlan_t *vlan_netif_driver = eth_sublayer_find_vlan_by_vid(sub, vlan_id);
    if (vlan_netif_driver == NULL) {
        ESP_LOGD(TAG, "vlan child not found for VLAN ID %" PRIu16, vlan_id);
        eth_sublayer_buf_free(sub, alloc_base);
        return ESP_ERR_NOT_FOUND;
    }
    return eth_vlan_input(vlan_netif_driver, frame, length, alloc_base, info);
}

esp_eth_sublayer_vlan_t *eth_sublayer_find_vlan_by_vid(esp_eth_sublayer_handle_t sub, uint16_t vlan_id)
{
    esp_eth_sublayer_vlan_t *netif_iodriver;
    esp_eth_sublayer_t *sub_impl = (esp_eth_sublayer_t *)sub;
    SLIST_FOREACH(netif_iodriver, &sub_impl->vlan_child, next) {
        if (netif_iodriver->vlan_id == vlan_id) {
            return netif_iodriver;
        }
    }
    return NULL;
}

esp_err_t eth_sublayer_insert_vlan(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_vlan_t *vlan)
{
    ESP_RETURN_ON_FALSE(sub && vlan, ESP_ERR_INVALID_ARG, TAG, "invalid arg");

    esp_eth_sublayer_t *sub_impl = (esp_eth_sublayer_t *)sub;
    SLIST_INSERT_HEAD(&sub_impl->vlan_child, vlan, next);
    return ESP_OK;
}

esp_err_t eth_sublayer_remove_vlan(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_vlan_t *vlan)
{
    ESP_RETURN_ON_FALSE(sub && vlan, ESP_ERR_INVALID_ARG, TAG, "invalid arg");

    esp_eth_sublayer_t *sub_impl = (esp_eth_sublayer_t *)sub;
    esp_eth_sublayer_vlan_t *v;
    SLIST_FOREACH(v, &sub_impl->vlan_child, next) {
        if (v == vlan) {
            SLIST_REMOVE(&sub_impl->vlan_child, vlan, esp_eth_sublayer_vlan_s, next);
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
esp_err_t eth_sublayer_set_switch(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_switch_t *sw)
{
    ESP_RETURN_ON_FALSE(sub && sw, ESP_ERR_INVALID_ARG, TAG, "invalid arg");
    ESP_RETURN_ON_FALSE(sub->switch_child == NULL, ESP_ERR_INVALID_STATE, TAG, "switch already attached");
    sub->switch_child = sw;
    return ESP_OK;
}

esp_eth_sublayer_switch_t *eth_sublayer_get_switch(esp_eth_sublayer_handle_t sub)
{
    if (sub == NULL) {
        return NULL;
    }
    return sub->switch_child;
}

esp_err_t eth_sublayer_remove_switch(esp_eth_sublayer_handle_t sub, esp_eth_sublayer_switch_t *sw)
{
    ESP_RETURN_ON_FALSE(sub && sw, ESP_ERR_INVALID_ARG, TAG, "invalid arg");
    ESP_RETURN_ON_FALSE(sub->switch_child == sw, ESP_ERR_NOT_FOUND, TAG, "switch is not attached to this sublayer");
    sub->switch_child = NULL;
    return ESP_OK;
}
#endif // CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT

/** One handler per ETH_EVENT: dispatch to base netif and every VLAN netif bound to this sublayer. */
static void eth_action_start(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_sublayer_t *sub = handler_args;
    if (sub->eth_driver != eth_handle) {
        return;
    }
    esp_eth_sublayer_vlan_t *netif_iodriver;
    SLIST_FOREACH(netif_iodriver, &sub->vlan_child, next) {
        if (netif_iodriver->base.netif) {
            esp_netif_action_start(netif_iodriver->base.netif, base, event_id, event_data);
        }
    }
}

static void eth_action_stop(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_sublayer_t *sub = handler_args;
    if (sub->eth_driver != eth_handle) {
        return;
    }
    sub->link_speed = ETH_SPEED_MAX; // set link speed to invalid value to indicate that the physical link is not up
    sub->connect_confirmed = false; // a new confirmation is required once the interface is started again
    esp_eth_sublayer_vlan_t *netif_iodriver;
    SLIST_FOREACH(netif_iodriver, &sub->vlan_child, next) {
        if (netif_iodriver->base.netif) {
            esp_netif_action_stop(netif_iodriver->base.netif, base, event_id, event_data);
        }
    }
}

static void eth_action_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_sublayer_t *sub = handler_args;

    if (base == ETH_EVENT) {
        esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
        if (sub->eth_driver != eth_handle) {
            return;
        }
        if (event_id == ETHERNET_EVENT_CONNECTED) {
            eth_speed_t speed;
            esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &speed);
            sub->link_speed = speed;
        }
    }

    bool set_connected = false;
    // if no confirm connect handler is registered, immediately set the link, otherwise wait for confirmation event
    if (!sub->connect_confirm_ctx_handler) {
        set_connected = true;
    } else if (base == sub->connect_confirm.base && event_id == sub->connect_confirm.event_id) {
        // the confirmation is remembered so the order in which the confirmation and the physical link up
        // are reported does not matter (the confirmation source does not need to be driven by ETH_EVENT)
        sub->connect_confirmed = true;
        if (sub->link_speed < ETH_SPEED_MAX) {
            set_connected = true;
        } else {
            ESP_LOGD(TAG, "physical link is not up, skipping confirm connected event");
        }
    } else if (sub->connect_confirmed && sub->link_speed < ETH_SPEED_MAX) {
        // the confirmation was already received before the physical link came up
        set_connected = true;
    }

    if (set_connected) {
        uint32_t link_speed = sub->link_speed == ETH_SPEED_1000M ? 1000'000'000 : sub->link_speed == ETH_SPEED_100M ? 100'000'000 : 10'000'000;

        esp_eth_sublayer_vlan_t *netif_iodriver;
        SLIST_FOREACH(netif_iodriver, &sub->vlan_child, next) {
            if (netif_iodriver->base.netif) {
                esp_netif_set_link_speed(netif_iodriver->base.netif, link_speed);
                esp_netif_action_connected(netif_iodriver->base.netif, base, event_id, event_data);
            }
        }
    }
}

static void eth_action_disconnected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_sublayer_t *sub = handler_args;
    if (base == ETH_EVENT) {
        esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
        if (sub->eth_driver != eth_handle) {
            return;
        }
        sub->link_speed = ETH_SPEED_MAX; // set link speed to invalid value to indicate that the physical link is not up
    }
    sub->connect_confirmed = false; // a new confirmation is required to bring the child netifs up again

    // note: disconnect event always results in all child netifs being disconnected (no matter of event id)
    esp_eth_sublayer_vlan_t *netif_iodriver;
    SLIST_FOREACH(netif_iodriver, &sub->vlan_child, next) {
        if (netif_iodriver->base.netif) {
            esp_netif_action_disconnected(netif_iodriver->base.netif, base, event_id, event_data);
        }
    }
}

static void eth_action_got_ip(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *ip_event = (ip_event_got_ip_t *)event_data;
    esp_eth_sublayer_t *sub = handler_args;

    esp_eth_sublayer_vlan_t *netif_iodriver;
    SLIST_FOREACH(netif_iodriver, &sub->vlan_child, next) {
        if (netif_iodriver->base.netif == ip_event->esp_netif) {
            esp_netif_action_got_ip(ip_event->esp_netif, base, event_id, event_data);
            return;
        }
    }
}

static esp_err_t sub_clear_handlers(esp_eth_sublayer_handle_t sub)
{
    ESP_RETURN_ON_FALSE(sub, ESP_ERR_INVALID_ARG, TAG, "sublayer handle can't be null");

    if (sub->start_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_START, sub->start_ctx_handler);
        sub->start_ctx_handler = NULL;
    }
    if (sub->stop_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_STOP, sub->stop_ctx_handler);
        sub->stop_ctx_handler = NULL;
    }
    if (sub->connect_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, sub->connect_ctx_handler);
        sub->connect_ctx_handler = NULL;
    }
    if (sub->connect_confirm_ctx_handler) {
        esp_event_handler_instance_unregister(sub->connect_confirm.base, sub->connect_confirm.event_id,
                                              sub->connect_confirm_ctx_handler);
        sub->connect_confirm_ctx_handler = NULL;
    }
    if (sub->disconnect_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, sub->disconnect_ctx_handler);
        sub->disconnect_ctx_handler = NULL;
    }
    if (sub->disconnect_trigger_ctx_handler) {
        esp_event_handler_instance_unregister(sub->disconnect_trigger.base, sub->disconnect_trigger.event_id,
                                              sub->disconnect_trigger_ctx_handler);
        sub->disconnect_trigger_ctx_handler = NULL;
    }
    if (sub->get_ip_ctx_handler) {
        esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, sub->get_ip_ctx_handler);
        sub->get_ip_ctx_handler = NULL;
    }
    return ESP_OK;
}

static esp_err_t sub_set_handlers(esp_eth_sublayer_handle_t sub)
{
    ESP_RETURN_ON_FALSE(sub, ESP_ERR_INVALID_ARG, TAG, "sublayer handle can't be null");

    esp_err_t ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_START, eth_action_start, sub,
                                                        &sub->start_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }
    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_STOP, eth_action_stop, sub, &sub->stop_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }
    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, eth_action_connected, sub,
                                              &sub->connect_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }
    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, eth_action_disconnected, sub,
                                              &sub->disconnect_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }
    ret = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, eth_action_got_ip, sub, &sub->get_ip_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    if (sub->connect_confirm.base) {
        ret = esp_event_handler_instance_register(sub->connect_confirm.base, sub->connect_confirm.event_id,
                                                  eth_action_connected, sub,
                                                  &sub->connect_confirm_ctx_handler);
        if (ret != ESP_OK) {
            goto fail;
        }
    }
    if (sub->disconnect_trigger.base) {
        ret = esp_event_handler_instance_register(sub->disconnect_trigger.base, sub->disconnect_trigger.event_id,
                                                  eth_action_disconnected, sub,
                                                  &sub->disconnect_trigger_ctx_handler);
        if (ret != ESP_OK) {
            goto fail;
        }
    }
    return ESP_OK;
fail:
    sub_clear_handlers(sub);
    return ret;
}

#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
static esp_err_t eth_sublayer_ll_transmit_wrap(void *h, void *buf, size_t len, void *eb)
{
    esp_eth_sublayer_t *sub = (esp_eth_sublayer_t *)h;
    esp_eth_buf_desc_t bufs[ESP_ETH_SUBLAYER_TX_BUF_DESC_CAPACITY];
    bufs[0].buf = buf;
    bufs[0].len = len;
    size_t buf_count = 1;
    esp_eth_sublayer_tx_bufs_t tx_bufs = {
        .bufs = bufs,
        .buf_count = &buf_count,
        .buf_capacity = ESP_ETH_SUBLAYER_TX_BUF_DESC_CAPACITY,
    };
    // port is not defined for direct lower layer transmit (integrated switch internal lookup is used when needed)
    return eth_sublayer_transmit(sub, &tx_bufs, eb, -1);
}

static esp_err_t eth_sublayer_ll_transmit(void *h, void *buf, size_t len)
{
    return eth_sublayer_ll_transmit_wrap(h, buf, len, NULL);
}

static esp_err_t eth_sublayer_get_ll_driver(void *h, void **ll_driver)
{
    esp_eth_sublayer_t *sub = (esp_eth_sublayer_t *)h;
    *ll_driver = sub->eth_driver;
    return ESP_OK;
}

static void eth_sublayer_ll_free(void *h, void *buffer)
{
    esp_eth_sublayer_t *sub = (esp_eth_sublayer_t *)h;
    eth_sublayer_buf_free(sub, buffer);
}

// Note: The consumer may call this from a critical section, hence no logging, allocation nor blocking is
// allowed here. See esp_eth_iodriver_provider_base_t::get_io_fns.
static esp_err_t eth_sublayer_get_io_fns(esp_eth_iodriver_provider_handle sublayer, void *io_handle, esp_eth_iodriver_io_fns_t *io_fns)
{
    if (sublayer == NULL || io_handle == NULL || io_fns == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_eth_sublayer_t *sub = (esp_eth_sublayer_t *)sublayer;

    // Based on the io_handle, get the io_fns - it can be either higher layer transmit functions with additional
    // processing like VLAN tagged, or lower layer sublayer transmit functions (closer to the underlying Ethernet driver).
    // At first check if the io_handle is a VLAN tagged netif iodriver
    esp_eth_sublayer_vlan_t *netif_iodriver;
    SLIST_FOREACH(netif_iodriver, &sub->vlan_child, next) {
        if (netif_iodriver == io_handle) {
            return eth_vlan_get_iodriver_io_fns(netif_iodriver, io_fns);
        }
    }
    // If no VLAN tagged netif iodriver is found, check if switch port is requested
    // Note: It's not allowed to request direct host Ethernet driver access when in switch mode.
#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
    if (sub->switch_child) {
        return eth_switch_get_iodriver_io_fns(sub->switch_child, io_handle, io_fns);
    }
#endif
    // If no switch, check if direct lower layer is requested
    if (io_handle == sub->eth_driver) {
        io_fns->io_handle = sub;
        io_fns->iodriver_transmit = eth_sublayer_ll_transmit;
        io_fns->iodriver_transmit_wrap = eth_sublayer_ll_transmit_wrap;
        io_fns->iodriver_free_rx_buffer = eth_sublayer_ll_free;
        io_fns->iodriver_get_ll_driver = eth_sublayer_get_ll_driver;
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}
#endif

esp_err_t esp_eth_sublayer_del(esp_eth_sublayer_handle_t sublayer)
{
    ESP_RETURN_ON_FALSE(sublayer, ESP_ERR_INVALID_ARG, TAG, "sublayer can't be null");

    while (!SLIST_EMPTY(&sublayer->vlan_child)) {
        esp_eth_sublayer_vlan_del(sublayer, SLIST_FIRST(&sublayer->vlan_child));
    }

#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER && CONFIG_ESP_NETIF_L2_TAP
    esp_vfs_l2tap_iodriver_provider_unregister(sublayer);
#endif // CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER && CONFIG_ESP_NETIF_L2_TAP

#if CONFIG_ETH_SUBLAYER_SWITCH_SUPPORT
    if (sublayer->switch_child) {
        esp_eth_sublayer_switch_del(sublayer->switch_child);
    }
#endif

    sub_clear_handlers(sublayer);
    esp_eth_decrease_reference(sublayer->eth_driver);
#if CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX
    if (sublayer->transmit_mutex) {
        vSemaphoreDelete(sublayer->transmit_mutex);
    }
#endif // CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX
    free(sublayer);
    return ESP_OK;
}

esp_err_t esp_eth_sublayer_new(const esp_eth_sublayer_config_t *config, esp_eth_sublayer_handle_t *sublayer)
{
    ESP_RETURN_ON_FALSE(config && sublayer, ESP_ERR_INVALID_ARG, TAG, "invalid arg");
    ESP_RETURN_ON_ERROR(esp_eth_increase_reference(config->eth_handle), TAG, "failed to increase reference");

    esp_err_t ret = ESP_OK;
    esp_eth_sublayer_t *sub = calloc(1, sizeof(esp_eth_sublayer_t));
    ESP_RETURN_ON_FALSE(sub, ESP_ERR_NO_MEM, TAG, "create netif sublayer failed");
    sub->eth_driver = config->eth_handle;
    sub->link_speed = ETH_SPEED_MAX; // invalid speed indicates that the speed is not yet known (link is not up)

    sub->connect_confirm = config->connect_confirm_event;
    sub->disconnect_trigger = config->disconnect_trigger_event;

    sub->tx_hook = config->tx_hook;
    sub->post_tx_hook = config->post_tx_hook;
    sub->rx_hook = config->rx_hook;
    sub->hook_ctx = config->hook_ctx;

#if CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX
    sub->transmit_mutex = xSemaphoreCreateMutex();
    ESP_GOTO_ON_FALSE(sub->transmit_mutex, ESP_ERR_NO_MEM, err, TAG, "failed to create sublayer transmit mutex");
    uint32_t transmit_mutex_timeout_ms = config->transmit_mutex_timeout_ms ? config->transmit_mutex_timeout_ms
                                                                           : ESP_ETH_SUBLAYER_TX_TIMEOUT_MS;
    sub->transmit_mutex_timeout_ticks = pdMS_TO_TICKS(transmit_mutex_timeout_ms);
#endif // CONFIG_ETH_SUBLAYER_TRANSMIT_MUTEX

    SLIST_INIT(&sub->vlan_child);

#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
    sub->base.get_io_fns = eth_sublayer_get_io_fns;
#if CONFIG_ESP_NETIF_L2_TAP
    ESP_GOTO_ON_ERROR(esp_vfs_l2tap_iodriver_provider_register(sub), err, TAG, "failed to register sublayer with L2 TAP");
#endif // CONFIG_ESP_NETIF_L2_TAP
#endif // CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
    ESP_GOTO_ON_ERROR(esp_eth_update_input_path_info(config->eth_handle, eth_sublayer_input, sub), err, TAG, "failed to update input path info");
    ESP_GOTO_ON_ERROR(sub_set_handlers(sub), err, TAG, "failed to set handlers");

    *sublayer = sub;
    return ESP_OK;
err:
    esp_eth_sublayer_del(sub);
    return ret;
}

esp_err_t eth_sublayer_get_mac_addr(esp_eth_sublayer_handle_t sublayer, uint8_t *mac_addr)
{
    ESP_RETURN_ON_FALSE(sublayer && mac_addr, ESP_ERR_INVALID_ARG, TAG, "invalid arg");
    return esp_eth_ioctl(sublayer->eth_driver, ETH_CMD_G_MAC_ADDR, mac_addr);
}

esp_eth_handle_t esp_eth_sublayer_get_eth_handle(esp_eth_sublayer_handle_t sublayer)
{
    ESP_RETURN_ON_FALSE(sublayer, NULL, TAG, "sublayer can't be null");
    return sublayer->eth_driver;
}
