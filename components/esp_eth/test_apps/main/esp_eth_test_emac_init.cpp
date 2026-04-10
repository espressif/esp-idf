/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

// Initialize the Ethernet stack compiling as C++ to catch possible C++ specific build issues.
#include "sdkconfig.h"
#include "esp_eth_mac.h"
#include "esp_eth_mac_esp.h"
#include "esp_eth_phy.h"
#include "esp_eth_driver.h"

#if CONFIG_ETH_USE_ESP32_EMAC
static void cpp_eth_init_internal_emac(void) __attribute__((unused));
static void cpp_eth_init_internal_emac(void)
{
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();

    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_config, &mac_config);
    if (mac == nullptr) {
        return;
    }
    esp_eth_phy_t *phy = esp_eth_phy_new_generic(&phy_config);
    if (phy == nullptr) {
        (void)mac->del(mac);
        return;
    }
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t handle = nullptr;
    if (esp_eth_driver_install(&eth_config, &handle) != ESP_OK) {
        (void)phy->del(phy);
        (void)mac->del(mac);
        return;
    }
    (void)esp_eth_driver_uninstall(handle);
    (void)phy->del(phy);
    (void)mac->del(mac);
}

#endif // CONFIG_ETH_USE_ESP32_EMAC

#if CONFIG_ETH_USE_SPI_ETHERNET
#include "ethernet_init.h"

static void cpp_eth_spi_custom_driver_config(void) __attribute__((unused));
static void cpp_eth_spi_custom_driver_config(void)
{
    volatile eth_spi_custom_driver_config_t cfg = ETH_DEFAULT_SPI;
    (void)cfg;
}

static void cpp_eth_init_spi(void) __attribute__((unused));
static void cpp_eth_init_spi(void)
{
    esp_eth_handle_t *handles = nullptr;
    uint8_t cnt = 0;
    (void)ethernet_init_all(&handles, &cnt);
    if (handles != nullptr && cnt > 0) {
        (void)ethernet_deinit_all(handles);
    }
}
#endif // CONFIG_ETH_USE_SPI_ETHERNET
