#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "dm9051_test";

#define ETH_START_BIT BIT(0)
#define ETH_STOP_BIT BIT(1)
#define ETH_CONNECT_BIT BIT(2)
#define ETH_GOT_IP_BIT BIT(3)

#define ETH_START_TIMEOUT_MS (10000)
#define ETH_CONNECT_TIMEOUT_MS (40000)
#define ETH_STOP_TIMEOUT_MS (10000)
#define ETH_GET_IP_TIMEOUT_MS (60000)

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)arg;
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        xEventGroupSetBits(eth_event_group, ETH_CONNECT_BIT);
        ESP_LOGI(TAG, "Ethernet Link Up");
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        xEventGroupSetBits(eth_event_group, ETH_START_BIT);
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        xEventGroupSetBits(eth_event_group, ETH_STOP_BIT);
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
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

#if CONFIG_ETH_USE_SPI_ETHERNET
TEST_CASE("dm9051 io test", "[ethernet][dm9051][ignore]")
{
    spi_device_handle_t spi_handle = NULL;
    spi_bus_config_t buscfg = {
        .miso_io_num = 25,
        .mosi_io_num = 23,
        .sclk_io_num = 19,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    TEST_ESP_OK(spi_bus_initialize(HSPI_HOST, &buscfg, 1));
    spi_device_interface_config_t devcfg = {
        .command_bits = 1,
        .address_bits = 7,
        .mode = 0,
        .clock_speed_hz = 20 * 1000 * 1000,
        .spics_io_num = 22,
        .queue_size = 20
    };
    TEST_ESP_OK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi_handle));
    gpio_install_isr_service(0);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    // create TCP/IP netif
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&cfg);
    // set default handlers to do layer 3 (and up) stuffs
    TEST_ESP_OK(esp_eth_set_default_handlers(eth_netif));
    // register user defined event handers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_dm9051_config_t dm9051_config = ETH_DM9051_DEFAULT_CONFIG(spi_handle);
    esp_eth_mac_t *mac = esp_eth_mac_new_dm9051(&dm9051_config, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_dm9051(&phy_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&config, &eth_handle));
    // combine driver with netif
    void *glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));
    // start Ethernet driver
    TEST_ESP_OK(esp_eth_start(eth_handle));
    /* wait for IP lease */
    vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    TEST_ESP_OK(esp_eth_del_netif_glue(glue));
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_eth_clear_default_handlers(eth_netif));
    esp_netif_destroy(eth_netif);
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(spi_bus_remove_device(spi_handle));
    TEST_ESP_OK(spi_bus_free(HSPI_HOST));
}
#endif
