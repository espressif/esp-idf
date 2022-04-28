/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_rom_md5.h"
#include "soc/soc_caps.h"

#if SOC_EMAC_SUPPORTED

static const char *TAG = "esp32_eth_test";

#define ETH_START_BIT BIT(0)
#define ETH_STOP_BIT BIT(1)
#define ETH_CONNECT_BIT BIT(2)
#define ETH_GOT_IP_BIT BIT(3)

#define ETH_START_TIMEOUT_MS (10000)
#define ETH_CONNECT_TIMEOUT_MS (40000)
#define ETH_STOP_TIMEOUT_MS (10000)
#define ETH_GET_IP_TIMEOUT_MS (60000)
#define ETH_DOWNLOAD_END_TIMEOUT_MS (240000)

extern const char dl_espressif_com_root_cert_pem_start[] asm("_binary_dl_espressif_com_root_cert_pem_start");
extern const char dl_espressif_com_root_cert_pem_end[]   asm("_binary_dl_espressif_com_root_cert_pem_end");

// compute md5 of download file
static md5_context_t md5_context;
static uint8_t digest[16];

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
    if (i < ms_to_wait / 100) {
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

TEST_CASE("esp32 ethernet io test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    mac_config.flags = ETH_MAC_FLAG_PIN_TO_CORE; // pin to core
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    // auto detect PHY address
    phy_config.phy_addr = ESP_ETH_PHY_ADDR_AUTO;
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    /* get MAC address */
    uint8_t mac_addr[6];
    memset(mac_addr, 0, sizeof(mac_addr));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr));
    ESP_LOGI(TAG, "Ethernet MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    TEST_ASSERT(mac_addr[0] != 0);
    /* get PHY address */
    int phy_addr = -1;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_PHY_ADDR, &phy_addr));
    ESP_LOGI(TAG, "Ethernet PHY Address: %d", phy_addr);
    TEST_ASSERT(phy_addr >= 0 && phy_addr <= 31);
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
}

TEST_CASE("esp32 ethernet speed/duplex/autonegotiation", "[ethernet][test_env=UT_T2_Ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    mac_config.flags = ETH_MAC_FLAG_PIN_TO_CORE; // pin to core
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    // auto detect PHY address
    phy_config.phy_addr = ESP_ETH_PHY_ADDR_AUTO;
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));

    // Set PHY to loopback mode so we do not have to take care about link configuration of the other node.
    // The reason behind is improbable, however, if the other node was configured to e.g. 100 Mbps and we
    // tried to change the speed at ESP node to 10 Mbps, we could get into trouble to establish a link.
    bool loopback_en = true;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en);

    // this test only test layer2, so don't need to register input callback (i.e. esp_eth_update_input_path)
    TEST_ESP_OK(esp_eth_start(eth_handle));
    // wait for connection start
    bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, true, true, pdMS_TO_TICKS(ETH_START_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);
    // wait for connection establish
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);

    eth_duplex_t exp_duplex;
    esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex);

    eth_speed_t exp_speed;
    esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed);
    // verify autonegotiation result (expecting the best link configuration)
    TEST_ASSERT_EQUAL(ETH_DUPLEX_FULL, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_100M, exp_speed);

    bool exp_autoneg_en;
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_AUTONEGO, &exp_autoneg_en));
    TEST_ASSERT_EQUAL(true, exp_autoneg_en);

    ESP_LOGI(TAG, "try to change autonegotiation when driver is started...");
    bool auto_nego_en = false;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_AUTONEGO, &exp_autoneg_en));
    TEST_ASSERT_EQUAL(true, exp_autoneg_en);

    ESP_LOGI(TAG, "stop the Ethernet driver and...");
    esp_eth_stop(eth_handle);

    ESP_LOGI(TAG, "try to change speed/duplex prior disabling the autonegotiation...");
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));

    eth_speed_t speed = ETH_SPEED_10M;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

    // Disable autonegotiation and change speed to 10 Mbps and duplex to half
    ESP_LOGI(TAG, "disable the autonegotiation and change the speed/duplex...");
    auto_nego_en = false;
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_AUTONEGO, &exp_autoneg_en));
    TEST_ASSERT_EQUAL(false, exp_autoneg_en);

    // set new duplex mode
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));

    // set new speed
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

    // start the driver and wait for connection establish
    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

    TEST_ASSERT_EQUAL(ETH_DUPLEX_HALF, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_10M, exp_speed);

    // Change speed back to 100 Mbps
    esp_eth_stop(eth_handle);
    ESP_LOGI(TAG, "change speed again...");
    speed = ETH_SPEED_100M;
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

    // start the driver and wait for connection establish
    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ASSERT_EQUAL(ETH_DUPLEX_HALF, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_100M, exp_speed);

    // Change duplex back to full
    esp_eth_stop(eth_handle);
    ESP_LOGI(TAG, "change duplex again...");
    duplex = ETH_DUPLEX_FULL;
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));

    // start the driver and wait for connection establish
    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

    TEST_ASSERT_EQUAL(ETH_DUPLEX_FULL, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_100M, exp_speed);

    ESP_LOGI(TAG, "try to change speed/duplex when driver is started and autonegotiation disabled...");
    speed = ETH_SPEED_10M;
    duplex = ETH_DUPLEX_HALF;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

    TEST_ASSERT_EQUAL(ETH_DUPLEX_FULL, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_100M, exp_speed);

    ESP_LOGI(TAG, "change the speed/duplex to 10 Mbps/half and then enable autonegotiation...");
    esp_eth_stop(eth_handle);
    speed = ETH_SPEED_10M;
    duplex = ETH_DUPLEX_HALF;

    // set new duplex mode
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));

    // set new speed
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

    // start the driver and wait for connection establish
    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

    TEST_ASSERT_EQUAL(ETH_DUPLEX_HALF, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_10M, exp_speed);

    esp_eth_stop(eth_handle);
    auto_nego_en = true;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en);
    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);

    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_AUTONEGO, &exp_autoneg_en));
    TEST_ASSERT_EQUAL(true, exp_autoneg_en);

    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

    // verify autonegotiation result (expecting the best link configuration)
    TEST_ASSERT_EQUAL(ETH_DUPLEX_FULL, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_100M, exp_speed);

    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
}

TEST_CASE("esp32 ethernet event test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    // this test only test layer2 event, so don't need to register input callback (i.e. esp_eth_update_input_path)
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
    // create TCP/IP netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    // combine driver with netif
    esp_eth_netif_glue_handle_t glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));
    // register user defined event handers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    // start Ethernet driver
    TEST_ESP_OK(esp_eth_start(eth_handle));
    /* wait for IP lease */
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);
    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    TEST_ESP_OK(esp_eth_del_netif_glue(glue));
    /* driver should be uninstalled within 2 seconds */
    TEST_ESP_OK(test_uninstall_driver(eth_handle, 2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    esp_netif_destroy(eth_netif);
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
    // create TCP/IP netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    // combine driver with netif
    esp_eth_netif_glue_handle_t glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));
    // register user defined event handers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));

    for (int i = 0; i < 10; i++) {
        // start Ethernet driver
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

    TEST_ESP_OK(esp_eth_del_netif_glue(glue));
    /* driver should be uninstalled within 2 seconds */
    TEST_ESP_OK(test_uninstall_driver(eth_handle, 2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    esp_netif_destroy(eth_netif);
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
}

esp_err_t http_event_handle(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
        break;
    case HTTP_EVENT_ON_DATA:
        esp_rom_md5_update(&md5_context, evt->data, evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

static void eth_start_download(void)
{
    esp_rom_md5_init(&md5_context);
    esp_http_client_config_t config = {
        .url = "https://dl.espressif.com/dl/misc/2MB.bin",
        .cert_pem = dl_espressif_com_root_cert_pem_start,
        .event_handler = http_event_handle,
        .buffer_size = 5120
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);
    TEST_ESP_OK(esp_http_client_perform(client));
    TEST_ESP_OK(esp_http_client_cleanup(client));
    esp_rom_md5_final(digest, &md5_context);
}

TEST_CASE("esp32 ethernet download test", "[ethernet][test_env=UT_T2_Ethernet][timeout=240]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    // create TCP/IP netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    // combine driver with netif
    esp_eth_netif_glue_handle_t glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));
    // register user defined event handers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    // start Ethernet driver
    TEST_ESP_OK(esp_eth_start(eth_handle));
    /* wait for IP lease */
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);

    eth_start_download();
    // check MD5 digest
    // MD5: df61db8564d145bbe67112aa8ecdccd8
    uint8_t expect_digest[16] = {223, 97, 219, 133, 100, 209, 69, 187, 230, 113, 18, 170, 142, 205, 204, 216};
    printf("MD5 Digest: ");
    for (int i = 0; i < 16; i++) {
        printf("%d ", digest[i]);
    }
    printf("\r\n");
    TEST_ASSERT(memcmp(expect_digest, digest, sizeof(digest)) == 0);

    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    TEST_ESP_OK(esp_eth_del_netif_glue(glue));
    /* driver should be uninstalled within 2 seconds */
    TEST_ESP_OK(test_uninstall_driver(eth_handle, 2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    esp_netif_destroy(eth_netif);
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
}

#endif // SOC_EMAC_SUPPORTED
