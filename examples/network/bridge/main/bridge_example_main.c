/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_netif_br_glue.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#if CONFIG_ETH_USE_SPI_ETHERNET
#include "driver/spi_master.h"
#endif // CONFIG_ETH_USE_SPI_ETHERNET

static const char *TAG = "eth_bridge_example";

#if CONFIG_EXAMPLE_SPI_ETHERNETS_NUM
#define SPI_ETHERNETS_NUM           CONFIG_EXAMPLE_SPI_ETHERNETS_NUM
#else
#define SPI_ETHERNETS_NUM           0
#endif

#if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET
#define INTERNAL_ETHERNETS_NUM      1
#else
#define INTERNAL_ETHERNETS_NUM      0
#endif

#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
#define INIT_SPI_ETH_MODULE_CONFIG(eth_module_config, num)                                      \
    do {                                                                                        \
        eth_module_config[num].spi_cs_gpio = CONFIG_EXAMPLE_ETH_SPI_CS ##num## _GPIO;           \
        eth_module_config[num].int_gpio = CONFIG_EXAMPLE_ETH_SPI_INT ##num## _GPIO;             \
        eth_module_config[num].phy_reset_gpio = CONFIG_EXAMPLE_ETH_SPI_PHY_RST ##num## _GPIO;   \
        eth_module_config[num].phy_addr = CONFIG_EXAMPLE_ETH_SPI_PHY_ADDR ##num;                \
    } while(0)

typedef struct {
    uint8_t spi_cs_gpio;
    uint8_t int_gpio;
    int8_t phy_reset_gpio;
    uint8_t phy_addr;
}spi_eth_module_config_t;
#endif

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet (%p) Link Up", eth_handle);
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet (%p) Link Down", eth_handle);
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet (%p) Started", eth_handle);
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet (%p) Stopped", eth_handle);
        break;
    default:
        break;
    }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
}

#if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET
/** Internal EMAC initialization */
esp_eth_handle_t eth_init_internal(void)
{
    esp_eth_handle_t eth_handle;

    // Init MAC and PHY configs to default
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO;
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp32_emac_config.smi_mdc_gpio_num = CONFIG_EXAMPLE_ETH_MDC_GPIO;
    esp32_emac_config.smi_mdio_gpio_num = CONFIG_EXAMPLE_ETH_MDIO_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
#if CONFIG_EXAMPLE_ETH_PHY_IP101
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
#elif CONFIG_EXAMPLE_ETH_PHY_RTL8201
    esp_eth_phy_t *phy = esp_eth_phy_new_rtl8201(&phy_config);
#elif CONFIG_EXAMPLE_ETH_PHY_LAN87XX
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
#elif CONFIG_EXAMPLE_ETH_PHY_DP83848
    esp_eth_phy_t *phy = esp_eth_phy_new_dp83848(&phy_config);
#elif CONFIG_EXAMPLE_ETH_PHY_KSZ80XX
    esp_eth_phy_t *phy = esp_eth_phy_new_ksz80xx(&phy_config);
#endif
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));

    return eth_handle;
}
#endif // CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET

#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
/** Ethernet SPI modules initialization */
esp_eth_handle_t eth_init_spi(spi_eth_module_config_t *spi_eth_module_config, uint8_t *mac_addr)
{
    esp_eth_handle_t eth_handle;

    // Init MAC and PHY configs to default
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    // Set module specific PHY config
    phy_config.phy_addr = spi_eth_module_config->phy_addr;
    phy_config.reset_gpio_num = spi_eth_module_config->phy_reset_gpio;

    // Configure SPI interface and Ethernet driver for specific SPI module
    esp_eth_mac_t *mac;
    esp_eth_phy_t *phy;

    spi_device_interface_config_t spi_devcfg = {
        .mode = 0,
        .clock_speed_hz = CONFIG_EXAMPLE_ETH_SPI_CLOCK_MHZ * 1000 * 1000,
        .queue_size = 20,
        .spics_io_num = spi_eth_module_config->spi_cs_gpio
    };
#if CONFIG_EXAMPLE_USE_KSZ8851SNL
    eth_ksz8851snl_config_t ksz8851snl_config = ETH_KSZ8851SNL_DEFAULT_CONFIG(CONFIG_EXAMPLE_ETH_SPI_HOST, &spi_devcfg);
    ksz8851snl_config.int_gpio_num = spi_eth_module_config->int_gpio;

    mac = esp_eth_mac_new_ksz8851snl(&ksz8851snl_config, &mac_config);
    phy = esp_eth_phy_new_ksz8851snl(&phy_config);
#elif CONFIG_EXAMPLE_USE_DM9051
    eth_dm9051_config_t dm9051_config = ETH_DM9051_DEFAULT_CONFIG(CONFIG_EXAMPLE_ETH_SPI_HOST, &spi_devcfg);
    dm9051_config.int_gpio_num = spi_eth_module_config->int_gpio;

    mac = esp_eth_mac_new_dm9051(&dm9051_config, &mac_config);
    phy = esp_eth_phy_new_dm9051(&phy_config);
#elif CONFIG_EXAMPLE_USE_W5500
    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(CONFIG_EXAMPLE_ETH_SPI_HOST, &spi_devcfg);
    w5500_config.int_gpio_num = spi_eth_module_config->int_gpio;

    mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
    phy = esp_eth_phy_new_w5500(&phy_config);

#endif //CONFIG_EXAMPLE_USE_W5500
    esp_eth_config_t eth_config_spi = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config_spi, &eth_handle));

    // The SPI Ethernet module might not have a burned factory MAC address, we cat to set it manually.
    ESP_ERROR_CHECK(esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, mac_addr));

    return eth_handle;
}
#endif // CONFIG_EXAMPLE_USE_SPI_ETHERNET

void app_main(void)
{
    // number of Ethernet ports to be used in the bridge
    uint8_t port_cnt = 0;
    // the same MAC address will be used for all Ethernet ports since the bridge acts as one device
    uint8_t common_mac_addr[ETH_ADDR_LEN];

    esp_eth_handle_t eth_handles[SPI_ETHERNETS_NUM + INTERNAL_ETHERNETS_NUM] = { NULL };
    esp_netif_t *eth_netifs[SPI_ETHERNETS_NUM + INTERNAL_ETHERNETS_NUM] = { NULL };

    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());

#if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET
    eth_handles[port_cnt++] = eth_init_internal();
    // use burned ESP32 MAC address as commom address for all Ethernet interfaces
    ESP_ERROR_CHECK(esp_eth_ioctl(eth_handles[0], ETH_CMD_G_MAC_ADDR, common_mac_addr));
#elif CONFIG_EXAMPLE_USE_SPI_ETHERNET
    // if ESP32 internal Ethernet is not used, use manually configured MAC address
    // 02:00:00 is a Locally Administered OUI range so should not be used except when testing on a LAN under your control.
    memcpy(common_mac_addr, (uint8_t[]) {0x02, 0x00, 0x00, 0x12, 0x34, 0x56}, ETH_ADDR_LEN);
#endif //CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET

#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
    // Install GPIO ISR handler to be able to service SPI Eth modlues interrupts
    gpio_install_isr_service(0);

    // Init SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num = CONFIG_EXAMPLE_ETH_SPI_MISO_GPIO,
        .mosi_io_num = CONFIG_EXAMPLE_ETH_SPI_MOSI_GPIO,
        .sclk_io_num = CONFIG_EXAMPLE_ETH_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(CONFIG_EXAMPLE_ETH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // Init specific SPI Ethernet module configuration from Kconfig (CS GPIO, Interrupt GPIO, etc.)
    spi_eth_module_config_t spi_eth_module_config[CONFIG_EXAMPLE_SPI_ETHERNETS_NUM];
    INIT_SPI_ETH_MODULE_CONFIG(spi_eth_module_config, 0);
#if CONFIG_EXAMPLE_SPI_ETHERNETS_NUM > 1
    INIT_SPI_ETH_MODULE_CONFIG(spi_eth_module_config, 1);
#endif
    for (int i = 0; i < CONFIG_EXAMPLE_SPI_ETHERNETS_NUM; i++) {
        eth_handles[port_cnt++] = eth_init_spi(&spi_eth_module_config[i], common_mac_addr);
    }
#endif // CONFIG_ETH_USE_SPI_ETHERNET
    // Create instances of esp-netif for Ethernet ports
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    esp_netif_config_t netif_cfg = {
        .base = &esp_netif_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };
    char if_key_str[10];
    char if_desc_str[10];
    char num_str[3];
    for (int i = 0; i < port_cnt; i++) {
        itoa(i, num_str, 10);
        strcat(strcpy(if_key_str, "ETH_"), num_str);
        strcat(strcpy(if_desc_str, "eth"), num_str);
        esp_netif_config.if_key = if_key_str;
        esp_netif_config.if_desc = if_desc_str;
        esp_netif_config.route_prio = 50 - i;
        esp_netif_config.flags = 0; // ESP-NETIF flags need to be zero when port's to be bridged

        eth_netifs[i] = esp_netif_new(&netif_cfg);

        // attach Ethernet driver to TCP/IP stack
        ESP_ERROR_CHECK(esp_netif_attach(eth_netifs[i], esp_eth_new_netif_glue(eth_handles[i])));
    }

    // Create instance of esp-netif for bridge interface
    esp_netif_inherent_config_t esp_netif_br_config = ESP_NETIF_INHERENT_DEFAULT_BR();
    esp_netif_config_t netif_br_cfg = {
        .base = &esp_netif_br_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_BR,
    };
    // Bridge configuration
    bridgeif_config_t bridgeif_config = {
        .max_fdb_dyn_entries = 10, // maximum number of address entries in dynamic forwarding database
        .max_fdb_sta_entries = 2,  // maximum number of address entries in static forwarding database
        .max_ports = port_cnt      // maximum number of ports the bridge can consist of
    };
    esp_netif_br_config.bridge_info = &bridgeif_config;
    // Set MAC address of bridge interface the same as the Ethernet interface
    memcpy(esp_netif_br_config.mac, common_mac_addr, ETH_ADDR_LEN);
    esp_netif_t *br_netif = esp_netif_new(&netif_br_cfg);

    // Create new esp netif bridge glue instance
    esp_netif_br_glue_handle_t netif_br_glue = esp_netif_br_glue_new();
    // Add Ethernet port interfaces to that esp netif bridge glue instance
    for (int i = 0; i < port_cnt; i++) {
        ESP_ERROR_CHECK(esp_netif_br_glue_add_port(netif_br_glue, eth_netifs[i]));
    }
    // Attach esp netif bridge glue instance with added ports to bridge netif
    ESP_ERROR_CHECK(esp_netif_attach(br_netif, netif_br_glue));

    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    for (int i = 0; i < port_cnt; i++) {
        // Since the MAC forwarding is performed in lwIP bridge, we need to pass all addresses through the Ethernet MACs
        bool promiscuous = true;
        esp_eth_ioctl(eth_handles[i], ETH_CMD_S_PROMISCUOUS, &promiscuous);
        // Start Ethernet driver state machine
        ESP_ERROR_CHECK(esp_eth_start(eth_handles[i]));
    }
}
