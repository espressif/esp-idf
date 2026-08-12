/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include "esp_eth_sublayer_switch.h"
#include "esp_eth_sublayer_core.h"
#include "esp_log.h"
#include "esp_check.h"
#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
#include "esp_private/esp_eth_sublayer_iodriver.h"
#endif

typedef struct esp_eth_sublayer_ic_switch_port_s {
    esp_eth_sublayer_switch_t *sw_parent;
    int32_t port_num;
    esp_eth_handle_t eth_handle;
} esp_eth_sublayer_ic_switch_port_t;

/* Integrated switch (Tail Tag) child. */
struct esp_eth_sublayer_switch_s {
    esp_eth_sublayer_handle_t sub_parent;
    esp_eth_sublayer_switch_demux_t demux;                   /*!< Per RX frame tag strip / ingress port resolve */
    esp_eth_sublayer_switch_mux_t mux;                       /*!< Per TX frame tag append */
    esp_eth_sublayer_switch_tag_deinit_t tag_process_deinit; /*!< Releases @c ctx on delete */
    void *ctx;                                               /*!< Driver switch context from tag_process_init */
    uint32_t ports_count;                                    /*!< Number of per-port Ethernet driver handles */
    esp_eth_sublayer_ic_switch_port_t port_io_handles[];     /*!< Per-port driver handles */
};

static const char *TAG = "esp_eth.sublayer.ic_switch";

esp_err_t eth_switch_demux(esp_eth_sublayer_switch_t *sw, uint8_t **buffer, uint32_t *length, int32_t *src_port)
{
    ESP_RETURN_ON_FALSE(sw, ESP_ERR_INVALID_ARG, TAG, "switch can't be null");

    // No demux callback means nothing to strip and no ingress port to report, so the frame is left
    // unchanged and the caller must keep using the sublayer level handles
    if (sw->demux == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return sw->demux(sw->ctx, buffer, length, src_port);
}

esp_err_t eth_switch_resolve_ingress_port(esp_eth_sublayer_switch_t *sw, int32_t port_num,
                                          esp_eth_handle_t *eth_handle, void **io_handle)
{
    ESP_RETURN_ON_FALSE(sw, ESP_ERR_INVALID_ARG, TAG, "switch can't be null");
    ESP_RETURN_ON_FALSE(port_num >= 1 && port_num <= (int32_t)sw->ports_count, ESP_ERR_INVALID_ARG, TAG, "invalid port");

    if (eth_handle) {
        *eth_handle = sw->port_io_handles[port_num - 1].eth_handle;
    }
    if (io_handle) {
        *io_handle = &sw->port_io_handles[port_num - 1];
    }
    return ESP_OK;
}

esp_err_t eth_switch_mux(esp_eth_sublayer_switch_t *sw, esp_eth_sublayer_tx_bufs_t *tx_bufs, int32_t port)
{
    ESP_RETURN_ON_FALSE(sw, ESP_ERR_INVALID_ARG, TAG, "switch can't be null");
    ESP_RETURN_ON_FALSE(tx_bufs && tx_bufs->bufs && tx_bufs->buf_count, ESP_ERR_INVALID_ARG, TAG, "invalid buffer");
    ESP_RETURN_ON_FALSE(port >= -1 && port <= (int32_t)sw->ports_count, ESP_ERR_INVALID_ARG, TAG, "invalid port");

    if (sw->mux == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return sw->mux(sw->ctx, tx_bufs, port);
}

esp_err_t esp_eth_sublayer_switch_add(esp_eth_sublayer_handle_t sublayer,
                                      const esp_eth_sublayer_switch_config_t *config,
                                      esp_eth_sublayer_switch_handle_t *sw)
{
    ESP_RETURN_ON_FALSE(sublayer && config && sw, ESP_ERR_INVALID_ARG, TAG, "invalid arg");
    ESP_RETURN_ON_FALSE(config->port_eth_handles && config->ports_count > 0, ESP_ERR_INVALID_ARG, TAG, "invalid port config");

    // A sublayer can hold at most one switch instance
    if (eth_sublayer_get_switch(sublayer) != NULL) {
        ESP_LOGE(TAG, "switch already added to sublayer");
        return ESP_ERR_INVALID_STATE;
    }

    esp_eth_sublayer_switch_t *sw_entry = calloc(1, sizeof(esp_eth_sublayer_switch_t)
                                                   + config->ports_count * sizeof(esp_eth_sublayer_ic_switch_port_t));
    ESP_RETURN_ON_FALSE(sw_entry, ESP_ERR_NO_MEM, TAG, "alloc switch entry failed");
    for (uint32_t i = 0; i < config->ports_count; i++) {
        sw_entry->port_io_handles[i].eth_handle = config->port_eth_handles[i];
        sw_entry->port_io_handles[i].port_num = i + 1;
        sw_entry->port_io_handles[i].sw_parent = sw_entry;
    }
    sw_entry->ports_count = config->ports_count;
    sw_entry->sub_parent = sublayer;
    sw_entry->demux = config->demux;
    sw_entry->mux = config->mux;
    sw_entry->tag_process_deinit = config->tag_process_deinit;

    // Let the driver allocate its switch context (e.g. enable Tail Tagging, build the port table)
    if (config->tag_process_init) {
        esp_err_t ret = config->tag_process_init(config->host_eth_handle, config->port_eth_handles,
                                                 config->ports_count, &sw_entry->ctx);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "switch tag process init failed (0x%x)", ret);
            free(sw_entry);
            return ret;
        }
    }

    esp_err_t ret = eth_sublayer_set_switch(sublayer, sw_entry);
    if (ret != ESP_OK) {
        if (sw_entry->tag_process_deinit) {
            sw_entry->tag_process_deinit(sw_entry->ctx);
        }
        free(sw_entry);
        return ret;
    }
    *sw = sw_entry;
    return ESP_OK;
}

esp_err_t esp_eth_sublayer_switch_del(esp_eth_sublayer_switch_handle_t sw)
{
    ESP_RETURN_ON_FALSE(sw, ESP_ERR_INVALID_ARG, TAG, "switch handle can't be null");

    esp_err_t ret = eth_sublayer_remove_switch(sw->sub_parent, sw);
    if (ret == ESP_OK) {
        if (sw->tag_process_deinit) {
            sw->tag_process_deinit(sw->ctx);
        }
        free(sw);
    }
    return ret;
}

#if CONFIG_ETH_SUBLAYER_IODRIVER_PROVIDER
static esp_err_t eth_switch_transmit_wrap(void *h, void *buf, size_t len, void *eb)
{
    esp_eth_sublayer_ic_switch_port_t *port = (esp_eth_sublayer_ic_switch_port_t *)h;
    esp_eth_sublayer_handle_t sub = port->sw_parent->sub_parent;
    esp_eth_buf_desc_t bufs[ESP_ETH_SUBLAYER_TX_BUF_DESC_CAPACITY];
    size_t buf_count = 1;

    bufs[0] = (esp_eth_buf_desc_t){ .buf = (uint8_t *)buf, .len = len };
    esp_eth_sublayer_tx_bufs_t tx_bufs = {
        .bufs = bufs,
        .buf_count = &buf_count,
        .buf_capacity = ESP_ETH_SUBLAYER_TX_BUF_DESC_CAPACITY,
    };
    return eth_sublayer_transmit(sub, &tx_bufs, eb, port->port_num);
}

static esp_err_t eth_switch_transmit(void *h, void *buf, size_t len)
{
    return eth_switch_transmit_wrap(h, buf, len, NULL);
}

static void eth_switch_free_rx_buffer(void *h, void *buffer)
{
    esp_eth_sublayer_ic_switch_port_t *port = (esp_eth_sublayer_ic_switch_port_t *)h;
    eth_sublayer_buf_free(port->sw_parent->sub_parent, buffer);
}

static esp_err_t eth_switch_get_ll_driver(void *h, void **ll_driver)
{
    esp_eth_sublayer_ic_switch_port_t *port = (esp_eth_sublayer_ic_switch_port_t *)h;
    *ll_driver = port->eth_handle;
    return ESP_OK;
}

esp_err_t eth_switch_get_iodriver_io_fns(esp_eth_sublayer_switch_t *sw, void *io_handle, esp_eth_iodriver_io_fns_t *io_fns)
{
    for(int i = 0; i < sw->ports_count; i++) {
        if (sw->port_io_handles[i].eth_handle == io_handle) {
            io_fns->io_handle = &sw->port_io_handles[i];
            io_fns->iodriver_transmit = eth_switch_transmit;
            io_fns->iodriver_transmit_wrap = eth_switch_transmit_wrap;
            io_fns->iodriver_free_rx_buffer = eth_switch_free_rx_buffer;
            io_fns->iodriver_get_ll_driver = eth_switch_get_ll_driver;
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}
#endif
