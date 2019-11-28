#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "tcpip_adapter.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "esp_eth_test";

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
    const tcpip_adapter_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    xEventGroupSetBits(eth_event_group, ETH_GOT_IP_BIT);
}


static esp_err_t test_uninstall_driver(esp_eth_handle_t eth_hdl, uint32_t ms_to_wait)
{
    int i = 0;
    ms_to_wait += 100;
    for (i = 0; i < ms_to_wait / 100; i++) {
        vTaskDelay(pdMS_TO_TICKS(100));
        if (esp_eth_driver_uninstall(eth_hdl) == ESP_OK) {
            break;
        }
    }
    if (i < ms_to_wait / 10) {
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

TEST_CASE("esp32 ethernet io test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    /* get MAC address */
    uint8_t mac_addr[6];
    memset(mac_addr, 0, sizeof(mac_addr));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr));
    ESP_LOGI(TAG, "Ethernet MAC Address: %d:%d:%d:%d:%d:%d",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    TEST_ASSERT(mac_addr[0] != 0);
    /* get PHY address */
    int phy_addr = -1;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_PHY_ADDR, &phy_addr));
    ESP_LOGI(TAG, "Ethernet PHY Address: %d", phy_addr);
    TEST_ASSERT(phy_addr >= 0 && phy_addr <= 31);
    /* driver should be uninstalled within 2 seconds */
    TEST_ESP_OK(test_uninstall_driver(eth_handle, 2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
}

TEST_CASE("esp32 ethernet event test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    TEST_ESP_OK(esp_eth_start(eth_handle));
    /* wait for connection start */
    bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, true, true, pdMS_TO_TICKS(ETH_START_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);
    /* wait for connection establish */
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    /* driver should be uninstalled within 2 seconds */
    TEST_ESP_OK(test_uninstall_driver(eth_handle, 2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
}

TEST_CASE("esp32 ethernet dhcp test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(tcpip_adapter_set_default_eth_handlers());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    TEST_ESP_OK(esp_eth_start(eth_handle));
    /* wait for IP lease */
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);
    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    /* driver should be uninstalled within 2 seconds */
    TEST_ESP_OK(test_uninstall_driver(eth_handle, 2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(tcpip_adapter_clear_default_eth_handlers());
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
}

TEST_CASE("esp32 ethernet start/stop stress test", "[ethernet][test_env=UT_T2_Ethernet][timeout=240]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(tcpip_adapter_set_default_eth_handlers());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));

    for (int i = 0; i < 10; i++) {
        TEST_ESP_OK(esp_eth_start(eth_handle));
        /* wait for IP lease */
        bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
        TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);
        // stop Ethernet driver
        TEST_ESP_OK(esp_eth_stop(eth_handle));
        /* wait for connection stop */
        bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
        TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    }

    /* driver should be uninstalled within 2 seconds */
    TEST_ESP_OK(test_uninstall_driver(eth_handle, 2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(tcpip_adapter_clear_default_eth_handlers());
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
}

#if CONFIG_ETH_USE_SPI_ETHERNET
TEST_CASE("dm9051 io test", "[ethernet][ignore]")
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
    TEST_ESP_OK(tcpip_adapter_set_default_eth_handlers());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_dm9051_config_t dm9051_config = ETH_DM9051_DEFAULT_CONFIG(spi_handle);
    esp_eth_mac_t *mac = esp_eth_mac_new_dm9051(&dm9051_config, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_dm9051(&phy_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&config, &eth_handle));
    TEST_ESP_OK(esp_eth_start(eth_handle));
    vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* driver should be uninstalled within 2 seconds */
    TEST_ESP_OK(test_uninstall_driver(eth_handle, 2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(spi_bus_remove_device(spi_handle));
    TEST_ESP_OK(spi_bus_free(HSPI_HOST));
}
#endif
