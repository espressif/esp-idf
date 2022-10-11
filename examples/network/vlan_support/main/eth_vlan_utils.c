/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_netif.h"
#include "esp_eth_netif_glue.h"
#include "esp_netif_net_stack.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"
#include "lwip/esp_netif_net_stack.h"
#include "sdkconfig.h"
#include "lwip/prot/ethernet.h"
#include "lwip/prot/ieee.h"
#include "eth_vlan_utils.h"
#if CONFIG_ESP_NETIF_L2_TAP
#include "esp_vfs_l2tap.h"
#endif


/**
 * @brief This function gets invoked when Ethernet receive a new packets.
 *
 * @note This function is to be registered as a callback function which get invoked when Ethernet receive a new packets.
 *
 * @param eth_handle handle of Ethernet driver
 * @param buffer buffer of the received packet
 * @param length length of the received packet
 * @param priv private pointer
 *
 * @return
 *      - ESP_OK: input frame buffer to upper stack successfully
 *      - ESP_FAIL: error occurred when inputting buffer to upper stack
 */
esp_err_t eth_input_to_netif(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv)
{
    struct netif *lwip_netif;
    u16_t netif_vlan_id;
    struct eth_hdr *ethhdr = (struct eth_hdr *)buffer;
    struct eth_vlan_hdr *vlan;
    esp_vlan_netifs *vlan_netifs = (esp_vlan_netifs *)priv;

    if (PP_HTONS(ETHTYPE_VLAN) != ethhdr->type) {
        // L2 tap after VLAN is not supported.
#if CONFIG_ESP_NETIF_L2_TAP
        esp_err_t ret = ESP_OK;
        ret = esp_vfs_l2tap_eth_filter(eth_handle, buffer, (size_t *)&length);
        if (length == 0) {
            return ret;
        }
#endif
        return esp_netif_receive(vlan_netifs->esp_netif[0], buffer, length, NULL);
    } else {
        for (int i = 1; i < vlan_netifs->netif_count; i++) {
            lwip_netif = esp_netif_get_netif_impl(vlan_netifs->esp_netif[i]);
            netif_vlan_id =  *((uint16_t *)netif_get_client_data(lwip_netif, LWIP_NETIF_CLIENT_DATA_INDEX_MAX + 1));

            vlan = (struct eth_vlan_hdr *)(((char *)ethhdr) + SIZEOF_ETH_HDR);

            if (VLAN_ID(vlan) == netif_vlan_id) {
                return esp_netif_receive(vlan_netifs->esp_netif[i], buffer, length, NULL);
            }
        }

        // If the vlan id of the incoming frame doesn't match with any of the interface send it to the default interface.
        return esp_netif_receive(vlan_netifs->esp_netif[0], buffer, length, NULL);
    }
}


/**
 * @brief This function creates configuration for esp-netif Ethernet
 *
 * @param vlan_id handle of Ethernet driver, used to name the interface key.
 * @param vlan_eth_cfg_o output parameter returning the esp-netif ethernet configuration.
 *
 * @return ESP_OK or ESP_FAIL
 */
esp_err_t get_vlan_netif_config(uint16_t vlan_id, esp_netif_config_t *vlan_eth_cfg_o)
{
    // Create new default instance of VLAN esp-netif for Ethernet
    char *if_key;
    if (asprintf(&if_key, "ETH_VLAN%d", vlan_id) == -1) {
        return ESP_FAIL;
    }

    esp_netif_inherent_config_t *esp_eth_vlan_base_config = malloc(sizeof(esp_netif_inherent_config_t));
    if (NULL == esp_eth_vlan_base_config) {
        return ESP_FAIL;
    }
    *esp_eth_vlan_base_config = (esp_netif_inherent_config_t)ESP_NETIF_INHERENT_DEFAULT_ETH();
    esp_eth_vlan_base_config->if_key = if_key;

    vlan_eth_cfg_o->base = esp_eth_vlan_base_config;
    vlan_eth_cfg_o->driver = NULL;
    vlan_eth_cfg_o->stack = ESP_NETIF_NETSTACK_DEFAULT_ETH;

    return ESP_OK;
}

/**
 * @brief This function frees the memory allocated for configuration for esp-netif Ethernet
 *
 * @param vlan_eth_cfg configuration for esp-netif Ethernet
 */
void free_vlan_config(esp_netif_config_t *vlan_eth_cfg)
{
    if ((NULL != vlan_eth_cfg) && (NULL != vlan_eth_cfg->base)) {
        free((void *)(vlan_eth_cfg->base->if_key));
        free((void *)(vlan_eth_cfg->base));
    }
}
