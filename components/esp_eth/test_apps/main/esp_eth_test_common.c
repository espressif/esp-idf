/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#if CONFIG_TARGET_USE_SPI_ETHERNET
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_mac.h"
#endif // CONFIG_TARGET_USE_SPI_ETHERNET
#include "sdkconfig.h"
#include "esp_eth_test_common.h"

#if CONFIG_TARGET_USE_SPI_ETHERNET
#define DEFAULT_TARGET_SPI_HOST 1
#define DEFAULT_TARGET_SPI_MISO_GPIO 12
#define DEFAULT_TARGET_SPI_MOSI_GPIO 13
#define DEFAULT_TARGET_SPI_SCLK_GPIO 14
#define DEFAULT_TARGET_SPI_CS_GPIO 15
#endif // CONFIG_TARGET_USE_SPI_ETHERNET

static const char *TAG = "esp32_eth_test_common";

esp_eth_mac_t *mac_init(void *vendor_emac_config, eth_mac_config_t *mac_config)
{
    esp_eth_mac_t *mac = NULL;

    eth_mac_config_t mac_config_default = ETH_MAC_DEFAULT_CONFIG();
    if (mac_config == NULL) {
        mac_config = &mac_config_default;
    }
#if CONFIG_TARGET_USE_INTERNAL_ETHERNET
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
#if !CONFIG_TARGET_USE_DEFAULT_EMAC_CONFIG
    esp32_emac_config.smi_gpio.mdc_num = CONFIG_TARGET_IO_MDC;
    esp32_emac_config.smi_gpio.mdio_num = CONFIG_TARGET_IO_MDIO;
#endif // CONFIG_TARGET_USE_DEFAULT_EMAC_CONFIG
    if (vendor_emac_config == NULL) {
        vendor_emac_config = &esp32_emac_config;
    }
    mac = esp_eth_mac_new_esp32(vendor_emac_config, mac_config);
#elif CONFIG_TARGET_USE_SPI_ETHERNET
    // Install GPIO ISR handler to be able to service SPI Eth modules interrupts
    gpio_install_isr_service(0);

    spi_bus_config_t buscfg = {
        .miso_io_num = DEFAULT_TARGET_SPI_MISO_GPIO,
        .mosi_io_num = DEFAULT_TARGET_SPI_MOSI_GPIO,
        .sclk_io_num = DEFAULT_TARGET_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    TEST_ESP_OK(spi_bus_initialize(DEFAULT_TARGET_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    spi_device_interface_config_t devcfg = {
        .mode = 0,
        .spics_io_num = DEFAULT_TARGET_SPI_CS_GPIO,
        .clock_speed_hz = CONFIG_TARGET_SPI_CLOCK_MHZ * 1000 * 1000,
        .queue_size = 20
    };
#if CONFIG_TARGET_ETH_PHY_DEVICE_W5500
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(DEFAULT_TARGET_SPI_HOST, &devcfg);
#if CONFIG_TARGET_ETH_SPI_POLL_MS > 0
    w5500_config.int_gpio_num = -1;
    w5500_config.poll_period_ms = CONFIG_TARGET_ETH_SPI_POLL_MS;
#endif
    if (vendor_emac_config == NULL) {
        vendor_emac_config = &w5500_config;
    }
    mac = esp_eth_mac_new_w5500(vendor_emac_config, mac_config);
#elif CONFIG_TARGET_ETH_PHY_DEVICE_KSZ8851SNL
    eth_ksz8851snl_config_t ksz8851snl_config = ETH_KSZ8851SNL_DEFAULT_CONFIG(DEFAULT_TARGET_SPI_HOST, &devcfg);
#if CONFIG_TARGET_ETH_SPI_POLL_MS > 0
    ksz8851snl_config.int_gpio_num = -1;
    ksz8851snl_config.poll_period_ms = CONFIG_TARGET_ETH_SPI_POLL_MS;
#endif
    if (vendor_emac_config == NULL) {
        vendor_emac_config = &ksz8851snl_config;
    }
    mac = esp_eth_mac_new_ksz8851snl(vendor_emac_config, mac_config);
#elif CONFIG_TARGET_ETH_PHY_DEVICE_DM9051
    eth_dm9051_config_t dm9051_config = ETH_DM9051_DEFAULT_CONFIG(DEFAULT_TARGET_SPI_HOST, &devcfg);
#if CONFIG_TARGET_ETH_SPI_POLL_MS > 0
    dm9051_config.int_gpio_num = -1;
    dm9051_config.poll_period_ms = CONFIG_TARGET_ETH_SPI_POLL_MS;
#endif
    if (vendor_emac_config == NULL) {
        vendor_emac_config = &dm9051_config ;
    }
    mac = esp_eth_mac_new_dm9051(vendor_emac_config, mac_config);
#endif // CONFIG_TARGET_ETH_PHY_DEVICE_W5500
#endif // CONFIG_TARGET_USE_INTERNAL_ETHERNET
    return mac;
}

esp_eth_phy_t *phy_init(eth_phy_config_t *phy_config)
{
    esp_eth_phy_t *phy = NULL;
    eth_phy_config_t phy_config_default = ETH_PHY_DEFAULT_CONFIG();
    if (phy_config == NULL) {
        phy_config = &phy_config_default;
    }
#if CONFIG_TARGET_USE_INTERNAL_ETHERNET
    phy_config->phy_addr = ESP_ETH_PHY_ADDR_AUTO;
#if CONFIG_TARGET_ETH_PHY_DEVICE_IP101
    phy = esp_eth_phy_new_ip101(phy_config);
    ESP_LOGI(TAG, "DUT PHY: IP101");
#elif CONFIG_TARGET_ETH_PHY_DEVICE_LAN8720
    phy = esp_eth_phy_new_lan87xx(phy_config);
    ESP_LOGI(TAG, "DUT PHY: LAN8720");
#elif CONFIG_TARGET_ETH_PHY_DEVICE_KSZ8041
    phy = esp_eth_phy_new_ksz80xx(phy_config);
    ESP_LOGI(TAG, "DUT PHY: KSZ8041");
#elif CONFIG_TARGET_ETH_PHY_DEVICE_RTL8201
    phy = esp_eth_phy_new_rtl8201(phy_config);
    ESP_LOGI(TAG, "DUT PHY: RTL8201");
#elif CONFIG_TARGET_ETH_PHY_DEVICE_DP83848
    phy = esp_eth_phy_new_dp83848(phy_config);
    ESP_LOGI(TAG, "DUT PHY: DP83848");
#endif // CONFIG_TARGET_ETH_PHY_DEVICE_IP101
#elif CONFIG_TARGET_USE_SPI_ETHERNET
    phy_config->reset_gpio_num = -1;
#if CONFIG_TARGET_ETH_PHY_DEVICE_W5500
    phy = esp_eth_phy_new_w5500(phy_config);
    ESP_LOGI(TAG, "DUT PHY: W5500");
#elif CONFIG_TARGET_ETH_PHY_DEVICE_KSZ8851SNL
    phy = esp_eth_phy_new_ksz8851snl(phy_config);
    ESP_LOGI(TAG, "DUT PHY: KSZ8851SNL");
#elif CONFIG_TARGET_ETH_PHY_DEVICE_DM9051
    phy = esp_eth_phy_new_dm9051(phy_config);
    ESP_LOGI(TAG, "DUT PHY: DM9051");
#endif // CONFIG_TARGET_ETH_PHY_DEVICE_W5500
#endif // CONFIG_TARGET_USE_INTERNAL_ETHERNET
    return phy;
}

void extra_eth_config(esp_eth_handle_t eth_handle)
{
// *** SPI Ethernet modules deviation ***
// Rationale: The SPI Ethernet modules don't have a burned default factory MAC address
#if CONFIG_TARGET_USE_SPI_ETHERNET
    uint8_t base_mac_addr[ETH_ADDR_LEN];
    TEST_ESP_OK(esp_efuse_mac_get_default(base_mac_addr));
    uint8_t local_mac[ETH_ADDR_LEN];
    TEST_ESP_OK(esp_derive_local_mac(local_mac, base_mac_addr));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, local_mac));
#endif
}

void extra_cleanup(void)
{
#if CONFIG_TARGET_USE_SPI_ETHERNET
    TEST_ESP_OK(spi_bus_free(DEFAULT_TARGET_SPI_HOST));
#endif
}

/** Event handler for Ethernet events */
void eth_event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
{
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)arg;
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Up");
        xEventGroupSetBits(eth_event_group, ETH_CONNECT_BIT);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        xEventGroupSetBits(eth_event_group, ETH_START_BIT);
    break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        xEventGroupSetBits(eth_event_group, ETH_STOP_BIT);
        break;
    default:
        break;
    }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)arg;
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;
    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    xEventGroupSetBits(eth_event_group, ETH_GOT_IP_BIT);
}
