/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "esp_eth_sublayer_frame.h"
#include "esp_eth_sublayer_vlan.h"
#include "esp_eth_sublayer_core.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_spec.h"
#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
#include "esp_private/esp_eth_sublayer_iodriver.h"
#endif
#if CONFIG_ESP_NETIF_L2_TAP
#include "esp_vfs_l2tap.h"
#endif

static const char *TAG = "esp_eth.sublayer.vlan_child";


uint16_t eth_vlan_get_ether_type(uint8_t *buffer)
{
    const eth_hdr_t *hdr = (const eth_hdr_t *)buffer;
    if (__builtin_bswap16(hdr->ether_type) != ETH_T_8021Q) {
        return ESP_ETH_SUBLAYER_UNTAGGED_VID;
    }
    const eth_vlan_hdr_t *vlan_hdr = (const eth_vlan_hdr_t *)buffer;
    uint16_t vid = __builtin_bswap16(vlan_hdr->vlan_tag.tci) & 0x0FFF;

    return vid;
}

esp_err_t eth_vlan_input(esp_eth_sublayer_vlan_t *vlan_netif_driver, uint8_t *frame, uint32_t length,
                         void *alloc_base, void *info)
{
#if CONFIG_ESP_NETIF_L2_TAP
    esp_err_t ret = ESP_OK;
#endif

    // frame is not VLAN tagged
    if (vlan_netif_driver->vlan_id == ESP_ETH_SUBLAYER_UNTAGGED_VID) {
        if (unlikely(vlan_netif_driver->base.netif == NULL)) {
            ESP_LOGD(TAG, "untagged netif not found");
            eth_sublayer_buf_free(vlan_netif_driver->parent, alloc_base);
            return ESP_ERR_NOT_FOUND;
        }
#if CONFIG_ESP_NETIF_L2_TAP
        l2tap_eth_filter_info_t l2tap_info = {
            .l2_buffer = alloc_base,
            .hw_ts = (l2tap_timestamp_t *)info, // Memory layout matches the Ethernet MAC driver type (eth_mac_time_t)
        };
        ret = esp_vfs_l2tap_eth_filter_frame(vlan_netif_driver, frame, (size_t *)&length, &l2tap_info);
        if (ret == ESP_OK) {
            if (length == 0) {
                return ret;
            }
        } else {
            eth_sublayer_buf_free(vlan_netif_driver->parent, alloc_base);
            return ret;
        }
#endif
        return esp_netif_receive(vlan_netif_driver->base.netif, frame, length, alloc_base);
    }

#if !CONFIG_ETH_SUBLAYER_VLAN_SUPPORT
    // esp_eth_sublayer_vlan_add() only allows the untagged VID when VLAN support is disabled, so no
    // tagged VLAN child can exist here - this path is unreachable, kept only to satisfy the compiler.
    eth_sublayer_buf_free(vlan_netif_driver->parent, alloc_base);
    return ESP_ERR_NOT_SUPPORTED;
#else
    // check if VLAN netif is attached
    if (vlan_netif_driver->base.netif == NULL) {
        ESP_LOGD(TAG, "netif not found for VLAN ID %" PRIu16, vlan_netif_driver->vlan_id);
        eth_sublayer_buf_free(vlan_netif_driver->parent, alloc_base);
        return ESP_ERR_NOT_FOUND;
    }

    // remove VLAN tag - just shift start of the frame over VLAN tag to avoid long memcpy of payload
    memmove(frame + ETH_VLAN_TAG_LEN, frame, ETH_HEADER_LEN - 2);
    length = length - ETH_VLAN_TAG_LEN;
#if CONFIG_ESP_NETIF_L2_TAP
    // Process VLAN tagged frames through L2 TAP
    l2tap_eth_filter_info_t l2tap_info = {
        .l2_buffer = alloc_base,
        .hw_ts = (l2tap_timestamp_t *)info, // Memory layout matches the Ethernet MAC driver type (eth_mac_time_t)
    };
    ret = esp_vfs_l2tap_eth_filter_frame(vlan_netif_driver, frame + ETH_VLAN_TAG_LEN, (size_t *)&length, &l2tap_info);
    if (ret == ESP_OK) {
        if (length == 0) {
            return ret;
        }
    } else {
        eth_sublayer_buf_free(vlan_netif_driver->parent, alloc_base);
        return ret;
    }
#endif
    return esp_netif_receive(vlan_netif_driver->base.netif, frame + ETH_VLAN_TAG_LEN, length, alloc_base);
#endif // !CONFIG_ETH_SUBLAYER_VLAN_SUPPORT
}

static esp_err_t eth_vlan_transmit_wrap(void *h, void *buf, size_t len, void *eb)
{
    esp_eth_sublayer_vlan_t *vlan_netif_driver = (esp_eth_sublayer_vlan_t *)h;
    esp_eth_sublayer_handle_t sub = vlan_netif_driver->parent;
    esp_err_t ret = ESP_OK;
    esp_eth_buf_desc_t bufs[ESP_ETH_SUBLAYER_TX_BUF_DESC_CAPACITY];
    size_t buf_count;

#if CONFIG_ETH_SUBLAYER_VLAN_SUPPORT
    eth_vlan_hdr_t *eth_hdr_buf = NULL;
    if (vlan_netif_driver->vlan_id != ESP_ETH_SUBLAYER_UNTAGGED_VID) {
        eth_hdr_buf = (eth_vlan_hdr_t *)malloc(sizeof(eth_vlan_hdr_t));
        if (eth_hdr_buf == NULL) {
            return ESP_ERR_NO_MEM;
        }

        // Copy Ethernet header to special buffer and add VLAN tag, manipulate only with header to avoid memcpy of payload
        memcpy(eth_hdr_buf->da, ((eth_hdr_t *)buf)->da, sizeof(eth_hdr_buf->da) + sizeof(eth_hdr_buf->sa));
        eth_hdr_buf->vlan_tag.tpid = __builtin_bswap16(ETH_T_8021Q);
        eth_hdr_buf->vlan_tag.tci = vlan_netif_driver->tci_be;
        eth_hdr_buf->ether_type = ((eth_hdr_t *)buf)->ether_type;
        bufs[0] = (esp_eth_buf_desc_t){ .buf = (uint8_t *)eth_hdr_buf, .len = ETH_HEADER_LEN + ETH_VLAN_TAG_LEN };

        // Set offset of payload to skip Ethernet header in the second buffer
        bufs[1] = (esp_eth_buf_desc_t){ .buf = (uint8_t *)buf + ETH_HEADER_LEN, .len = len - ETH_HEADER_LEN };
        buf_count = 2;
    } else
#endif // CONFIG_ETH_SUBLAYER_VLAN_SUPPORT
    {
        // vlan_id is always ESP_ETH_SUBLAYER_UNTAGGED_VID when CONFIG_ETH_SUBLAYER_VLAN_SUPPORT is disabled,
        // esp_eth_sublayer_vlan_add() rejects any other VID in that case.
        bufs[0] = (esp_eth_buf_desc_t){ .buf = (uint8_t *)buf, .len = len };
        buf_count = 1;
    }
    esp_eth_sublayer_tx_bufs_t tx_bufs = {
        .bufs = bufs,
        .buf_count = &buf_count,
        .buf_capacity = ESP_ETH_SUBLAYER_TX_BUF_DESC_CAPACITY,
    };
    // eb holds additional metadata like time stamp,
    // specific port is not defined for VLAN (use integrated switch internal lookup)
    // Note, this keeps door open to force port netif instance if needed in the future
    ret = eth_sublayer_transmit(sub, &tx_bufs, eb, -1);
#if CONFIG_ETH_SUBLAYER_VLAN_SUPPORT
    free(eth_hdr_buf);
#endif // CONFIG_ETH_SUBLAYER_VLAN_SUPPORT
    return ret;
}

static esp_err_t eth_vlan_transmit(void *h, void *buf, size_t len)
{
    return eth_vlan_transmit_wrap(h, buf, len, NULL);
}

static esp_err_t eth_vlan_set_mac_filter(void *h, const uint8_t *eth_mac, size_t mac_len, bool add)
{
    esp_eth_sublayer_vlan_t *vlan_netif_driver = (esp_eth_sublayer_vlan_t *)h;
    return eth_sublayer_set_mac_filter(vlan_netif_driver->parent, eth_mac, mac_len, add);
}

static void eth_vlan_free_rx_buffer(void *h, void *buffer)
{
    esp_eth_sublayer_vlan_t *vlan_netif_driver = (esp_eth_sublayer_vlan_t *)h;
    eth_sublayer_buf_free(vlan_netif_driver->parent, buffer);
}

esp_err_t netif_post_attach(esp_netif_t *esp_netif, void *args)
{
    uint8_t eth_mac[ETH_ADDR_LEN];
    esp_eth_sublayer_vlan_t *vlan_netif_driver = (esp_eth_sublayer_vlan_t *)args;
    esp_eth_sublayer_handle_t parent_sub = vlan_netif_driver->parent;

    esp_netif_driver_ifconfig_t driver_ifconfig = {
        .handle = vlan_netif_driver,
        .transmit = eth_vlan_transmit,
        .transmit_wrap = eth_vlan_transmit_wrap,
        .driver_free_rx_buffer = eth_vlan_free_rx_buffer,
        .driver_set_mac_filter = eth_vlan_set_mac_filter,
    };

    ESP_RETURN_ON_ERROR(esp_netif_set_driver_config(esp_netif, &driver_ifconfig), TAG, "failed to set driver config");
    ESP_RETURN_ON_ERROR(eth_sublayer_get_mac_addr(parent_sub, eth_mac), TAG, "failed to get MAC addr");
    ESP_RETURN_ON_ERROR(esp_netif_set_mac(esp_netif, eth_mac), TAG, "failed to set MAC");
    // assigned only once the netif is fully configured, the Rx path uses it to decide whether the frame
    // can be passed to the netif
    vlan_netif_driver->base.netif = esp_netif;

    if (vlan_netif_driver->vlan_id == ESP_ETH_SUBLAYER_UNTAGGED_VID) {
        ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x", eth_mac[0], eth_mac[1],
                 eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);
        ESP_LOGI(TAG, "untagged Ethernet attached to netif");
    } else {
        ESP_LOGI(TAG, "VLAN %" PRIu16 " attached to netif", vlan_netif_driver->vlan_id);
    }
    return ESP_OK;
}

esp_eth_handle_t esp_eth_sublayer_vlan_get_eth_handle(esp_eth_sublayer_vlan_handle_t vlan)
{
    ESP_RETURN_ON_FALSE(vlan, NULL, TAG, "vlan can't be null");
    return esp_eth_sublayer_get_eth_handle(vlan->parent);
}

esp_err_t esp_eth_sublayer_vlan_add(esp_eth_sublayer_handle_t sublayer, uint16_t vlan_id,
                                    esp_eth_sublayer_vlan_handle_t *vlan)
{
    ESP_RETURN_ON_FALSE(sublayer && vlan, ESP_ERR_INVALID_ARG, TAG, "invalid arg");

#if !CONFIG_ETH_SUBLAYER_VLAN_SUPPORT
    ESP_RETURN_ON_FALSE(vlan_id == ESP_ETH_SUBLAYER_UNTAGGED_VID, ESP_ERR_NOT_SUPPORTED, TAG,
                        "tagged VLAN children require CONFIG_ETH_SUBLAYER_VLAN_SUPPORT");
#endif // !CONFIG_ETH_SUBLAYER_VLAN_SUPPORT

    if (eth_sublayer_find_vlan_by_vid(sublayer, vlan_id) != NULL) {
        ESP_LOGE(TAG, "duplicate VLAN id %" PRIu16, vlan_id);
        return ESP_ERR_INVALID_STATE;
    }

    esp_eth_sublayer_vlan_t *vlan_entry = calloc(1, sizeof(esp_eth_sublayer_vlan_t));
    ESP_RETURN_ON_FALSE(vlan_entry, ESP_ERR_NO_MEM, TAG, "alloc vlan entry failed");
    vlan_entry->vlan_id = vlan_id;
    vlan_entry->tci_be = __builtin_bswap16((uint16_t)(vlan_id & 0x0FFF));
    vlan_entry->parent = sublayer;
    vlan_entry->base.post_attach = netif_post_attach;

    eth_sublayer_insert_vlan(sublayer, vlan_entry);
    *vlan = vlan_entry;
    return ESP_OK;
}

esp_err_t esp_eth_sublayer_vlan_del(esp_eth_sublayer_handle_t sublayer, esp_eth_sublayer_vlan_handle_t vlan)
{
    ESP_RETURN_ON_FALSE(sublayer && vlan, ESP_ERR_INVALID_ARG, TAG, "invalid arg");

    esp_err_t ret = eth_sublayer_remove_vlan(sublayer, vlan);
    if (ret == ESP_OK) {
        free(vlan);
    }
    return ret;
}

#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
static esp_err_t eth_vlan_get_ll_driver(void *h, void **ll_driver)
{
    esp_eth_sublayer_vlan_t *vlan_netif_driver = (esp_eth_sublayer_vlan_t *)h;
    *ll_driver = esp_eth_sublayer_get_eth_handle(vlan_netif_driver->parent);
    return ESP_OK;
}

esp_err_t eth_vlan_get_iodriver_io_fns(esp_eth_sublayer_vlan_t *vlan_netif_driver, esp_eth_iodriver_io_fns_t *io_fns)
{
    // Note: May be called from a critical section, hence no logging, allocation nor blocking is allowed here.
    if (vlan_netif_driver == NULL || io_fns == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    io_fns->io_handle = vlan_netif_driver;
    io_fns->iodriver_transmit = eth_vlan_transmit;
    io_fns->iodriver_transmit_wrap = eth_vlan_transmit_wrap;
    io_fns->iodriver_free_rx_buffer = eth_vlan_free_rx_buffer;
    io_fns->iodriver_get_ll_driver = eth_vlan_get_ll_driver;
    return ESP_OK;
}
#endif
