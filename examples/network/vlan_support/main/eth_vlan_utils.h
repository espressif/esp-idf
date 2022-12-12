/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */


// Maximum number of interface that can be added
#define MAX_ETH_NETIF_COUNT     (10)

typedef struct {
    esp_netif_t *esp_netif[MAX_ETH_NETIF_COUNT];
    uint8_t netif_count;
} esp_vlan_netifs;


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
esp_err_t eth_input_to_netif(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv);


/**
 * @brief This function creates configuration for esp-netif Ethernet
 *
 * @param vlan_id handle of Ethernet driver, used to name the interface key.
 * @param vlan_eth_cfg_o output parameter returning the esp-netif ethernet configuration.
 *
 * @return ESP_OK or ESP_FAIL
 */
esp_err_t get_vlan_netif_config(uint16_t vlan_id, esp_netif_config_t *vlan_eth_cfg_o);


/**
 * @brief This function frees the memory allocated for configuration for esp-netif Ethernet
 *
 * @param vlan_eth_cfg configuration for esp-netif Ethernet
 */
void free_vlan_config(esp_netif_config_t *vlan_eth_cfg);
