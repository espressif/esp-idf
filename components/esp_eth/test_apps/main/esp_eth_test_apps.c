/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_rom_md5.h"
#include "esp_eth_test_common.h"

#define LOOPBACK_TEST_PACKET_SIZE 256

static const char *TAG = "esp32_eth_test";

extern const char dl_espressif_com_root_cert_pem_start[] asm("_binary_dl_espressif_com_root_cert_pem_start");
extern const char dl_espressif_com_root_cert_pem_end[]   asm("_binary_dl_espressif_com_root_cert_pem_end");

// compute md5 of download file
static md5_context_t md5_context;
static uint8_t digest[16];

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

TEST_CASE("ethernet io test", "[ethernet]")
{
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    mac_config.flags = ETH_MAC_FLAG_PIN_TO_CORE; // pin to core
    esp_eth_mac_t *mac = mac_init(NULL, &mac_config);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    extra_eth_config(eth_handle);

    /* get default MAC address */
    uint8_t mac_addr[ETH_ADDR_LEN];
    memset(mac_addr, 0, sizeof(mac_addr));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr));
    ESP_LOGI(TAG, "Ethernet MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    TEST_ASSERT(mac_addr[0] != 0);
// *** SPI Ethernet modules deviation ***
// Rationale: The SPI Ethernet modules don't have a burned default factory MAC address hence local MAC is used
#if !CONFIG_TARGET_USE_SPI_ETHERNET
    TEST_ASSERT_BITS(0b00000011, 0b00, mac_addr[0]);    // Check UL&IG, should be UI
#endif

    /* set different MAC address */
    mac_addr[5] ^= mac_addr[4];
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, mac_addr));
    /* get new MAC address */
    uint8_t mac_addr_new[ETH_ADDR_LEN] = { 0 };
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr_new));
    ESP_LOGI(TAG, "Ethernet MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr_new[0], mac_addr_new[1], mac_addr_new[2], mac_addr_new[3], mac_addr_new[4], mac_addr_new[5]);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(mac_addr_new, mac_addr, ETH_ADDR_LEN);

// *** SPI Ethernet modules deviation ***
// Rationale: SPI Ethernet modules PHYs and MACs are statically configured at one die, hence there is no need for PHY address
// from user's point of view
#if !CONFIG_TARGET_USE_SPI_ETHERNET
    /* get PHY address */
    int phy_addr = -1;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_PHY_ADDR, &phy_addr));
    ESP_LOGI(TAG, "Ethernet PHY Address: %d", phy_addr);
    TEST_ASSERT(phy_addr >= 0 && phy_addr <= 31);
#endif
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    extra_cleanup();
}

// This test expects autonegotiation to be enabled on the other node.
TEST_CASE("ethernet io speed/duplex/autonegotiation", "[ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    mac_config.flags = ETH_MAC_FLAG_PIN_TO_CORE; // pin to core
    esp_eth_mac_t *mac = mac_init(NULL, &mac_config);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    extra_eth_config(eth_handle);
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));

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
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_AUTONEGO, &exp_autoneg_en));
    TEST_ASSERT_EQUAL(true, exp_autoneg_en);

    ESP_LOGI(TAG, "try to change autonegotiation when driver is started...");
    bool auto_nego_en = false;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_AUTONEGO, &exp_autoneg_en));
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
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_AUTONEGO, &exp_autoneg_en));
    TEST_ASSERT_EQUAL(false, exp_autoneg_en);

    // set new duplex mode
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));

    // set new speed
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

// *** LAN8720 deviation ***
// Rationale: When the device is in manual 100BASE-TX or 10BASE-T modes with Auto-MDIX enabled, the PHY does not link to a
//            link partner that is configured for auto-negotiation. See LAN8720 errata for more details.
#ifdef CONFIG_TARGET_ETH_PHY_DEVICE_LAN8720
    esp_eth_phy_reg_rw_data_t reg;
    uint32_t reg_val;
    reg.reg_addr = 27;
    reg.reg_value_p = &reg_val;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg));
    reg_val |= 0x8000;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_WRITE_PHY_REG, &reg));
    uint32_t reg_val_act;
    reg.reg_value_p = &reg_val_act;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg));
    TEST_ASSERT_EQUAL(reg_val, reg_val_act);
#endif

    // start the driver and wait for connection establish
    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

    TEST_ASSERT_EQUAL(ETH_DUPLEX_HALF, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_10M, exp_speed);

    // Change speed back to 100 Mbps
    esp_eth_stop(eth_handle);
    ESP_LOGI(TAG, "change speed again...");
    speed = ETH_SPEED_100M;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

    // start the driver and wait for connection establish
    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ASSERT_EQUAL(ETH_DUPLEX_HALF, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_100M, exp_speed);

    // Change duplex back to full
    esp_eth_stop(eth_handle);
    ESP_LOGI(TAG, "change duplex again...");
    duplex = ETH_DUPLEX_FULL;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));

    // start the driver and wait for connection establish
    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

    TEST_ASSERT_EQUAL(ETH_DUPLEX_FULL, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_100M, exp_speed);

    ESP_LOGI(TAG, "try to change speed/duplex when driver is started and autonegotiation disabled...");
    speed = ETH_SPEED_10M;
    duplex = ETH_DUPLEX_HALF;

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

    TEST_ASSERT_EQUAL(ETH_DUPLEX_FULL, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_100M, exp_speed);

    ESP_LOGI(TAG, "change the speed/duplex to 10 Mbps/half and then enable autonegotiation...");
    esp_eth_stop(eth_handle);
    speed = ETH_SPEED_10M;
    duplex = ETH_DUPLEX_HALF;

    // set new duplex mode
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));

    // set new speed
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

    // start the driver and wait for connection establish
    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

    TEST_ASSERT_EQUAL(ETH_DUPLEX_HALF, exp_duplex);
    TEST_ASSERT_EQUAL(ETH_SPEED_10M, exp_speed);

    esp_eth_stop(eth_handle);
    auto_nego_en = true;
    esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en);

// *** LAN8720 deviation ***
// Rationale: See above
#ifdef CONFIG_TARGET_ETH_PHY_DEVICE_LAN8720
    reg.reg_value_p = &reg_val;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg));
    reg_val &= ~0x8000;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_WRITE_PHY_REG, &reg));
    reg.reg_value_p = &reg_val_act;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg));
    TEST_ASSERT_EQUAL(reg_val, reg_val_act);
#endif

    esp_eth_start(eth_handle);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);

    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_AUTONEGO, &exp_autoneg_en));
    TEST_ASSERT_EQUAL(true, exp_autoneg_en);

    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &exp_duplex));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &exp_speed));

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
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
}

static SemaphoreHandle_t loopback_test_case_data_received;
static esp_err_t loopback_test_case_incoming_handler(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv)
{
    TEST_ASSERT(memcmp(priv, buffer, LOOPBACK_TEST_PACKET_SIZE) == 0);
    xSemaphoreGive(loopback_test_case_data_received);
    free(buffer);
    return ESP_OK;
}

TEST_CASE("ethernet io loopback", "[ethernet]")
{
    loopback_test_case_data_received = xSemaphoreCreateBinary();
    // init everything else
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    mac_config.flags = ETH_MAC_FLAG_PIN_TO_CORE; // pin to core
    esp_eth_mac_t *mac = mac_init(NULL, &mac_config);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    extra_eth_config(eth_handle);
    // Disable autonegotiation to manually set speed and duplex mode
    bool auto_nego_en = false;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    bool loopback_en = true;
// *** W5500 deviation ***
// Rationale: does not support loopback
#ifdef CONFIG_TARGET_ETH_PHY_DEVICE_W5500
    TEST_ASSERT(esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en) == ESP_ERR_NOT_SUPPORTED);
    goto cleanup;
#else
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en));
#endif

    eth_duplex_t duplex_modes[] = {ETH_DUPLEX_HALF, ETH_DUPLEX_FULL};
    eth_speed_t speeds[] = {ETH_SPEED_10M, ETH_SPEED_100M};
    emac_frame_t* test_packet = malloc(LOOPBACK_TEST_PACKET_SIZE);
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, test_packet->src);
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, test_packet->dest);
    for(size_t i = 0; i < LOOPBACK_TEST_PACKET_SIZE-ETH_HEADER_LEN; i++){
        test_packet->data[i] = rand() & 0xff;
    }
    TEST_ESP_OK(esp_eth_update_input_path(eth_handle, loopback_test_case_incoming_handler, test_packet));
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));

    for (int i = 0; i < sizeof(speeds) / sizeof(eth_speed_t); i++) {
        eth_speed_t expected_speed = speeds[i];
        for (int j = 0; j < sizeof(duplex_modes) / sizeof(eth_duplex_t); j++) {
            eth_duplex_t expected_duplex = duplex_modes[j];
            ESP_LOGI(TAG, "Test with %s Mbps %s duplex.", expected_speed == ETH_SPEED_10M ? "10" : "100", expected_duplex == ETH_DUPLEX_HALF ? "half" : "full");
// *** KSZ80XX, KSZ8851SNL and DM9051 deviation ***
// Rationale: do not support loopback at 10 Mbps
#if defined(CONFIG_TARGET_ETH_PHY_DEVICE_KSZ8041) || defined(CONFIG_TARGET_ETH_PHY_DEVICE_DM9051)
            if ((expected_speed == ETH_SPEED_10M)) {
                TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &expected_speed));
                continue;
            } else if (expected_speed == ETH_SPEED_100M) {
                TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &expected_speed));
            }
#else
            TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &expected_speed));
#endif
            if ((expected_duplex == ETH_DUPLEX_HALF)) {
                TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &expected_duplex));
                continue;
            } else if (expected_duplex == ETH_DUPLEX_FULL) {
                TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &expected_duplex));
            }

            TEST_ESP_OK(esp_eth_start(eth_handle));
            bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));

            eth_speed_t actual_speed = -1;
            TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &actual_speed));
            TEST_ASSERT_EQUAL(expected_speed, actual_speed);

            eth_duplex_t actual_duplex = -1;
            TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_DUPLEX_MODE, &actual_duplex));
            TEST_ASSERT_EQUAL(expected_duplex, actual_duplex);

            TEST_ESP_OK(esp_eth_transmit(eth_handle, test_packet, LOOPBACK_TEST_PACKET_SIZE));
            TEST_ASSERT(xSemaphoreTake(loopback_test_case_data_received, pdMS_TO_TICKS(10000)) == pdTRUE);
            TEST_ESP_OK(esp_eth_stop(eth_handle));
        }
    }

    // Test enabling autonegotiation when loopback is disabled
    ESP_LOGI(TAG, "Test enabling autonegotiation without loopback.");
    loopback_en = false;
    auto_nego_en = true;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    auto_nego_en = false;
    loopback_en = true;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en));
    // Test with enabled autonegotiaton
    ESP_LOGI(TAG, "Test with enabled autonegotiation.");
    auto_nego_en = true;
// *** RTL8201, DP83848 and LAN87xx deviation ***
// Rationale: do not support autonegotiation with loopback enabled.
#if defined(CONFIG_TARGET_ETH_PHY_DEVICE_RTL8201) || defined(CONFIG_TARGET_ETH_PHY_DEVICE_DP83848) || \
    defined(CONFIG_TARGET_ETH_PHY_DEVICE_LAN8720)
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    goto cleanup;
#endif
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    TEST_ESP_OK(esp_eth_start(eth_handle));
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));

    TEST_ESP_OK(esp_eth_transmit(eth_handle, test_packet, LOOPBACK_TEST_PACKET_SIZE));
    TEST_ASSERT(xSemaphoreTake(loopback_test_case_data_received, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS)) == pdTRUE);

    free(test_packet);
    loopback_en = false;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en));
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
// *** W5500, LAN87xx, RTL8201 and DP83848 deviation ***
// Rationale: in those cases 'goto cleanup' is used to skip part of the test code. Incasing in #if block is done to prevent unused label error
#if defined(CONFIG_TARGET_ETH_PHY_DEVICE_W5500) || defined(CONFIG_TARGET_ETH_PHY_DEVICE_LAN8720) || \
    defined(CONFIG_TARGET_ETH_PHY_DEVICE_RTL8201) || defined(CONFIG_TARGET_ETH_PHY_DEVICE_DP83848)
cleanup:
#endif
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_event_loop_delete_default());
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
}

TEST_CASE("ethernet event test", "[ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    extra_eth_config(eth_handle);
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
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
}

TEST_CASE("ethernet dhcp test", "[ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    // create TCP/IP netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);
    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    extra_eth_config(eth_handle);
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
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
}

TEST_CASE("ethernet start/stop stress test with IP stack", "[ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    // create TCP/IP netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);
    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    extra_eth_config(eth_handle);
    // combine driver with netif
    esp_eth_netif_glue_handle_t glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));
    // register user defined event handers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));

    for(int j = 0; j < 2; j++) {
        // run the start/stop test with disabled auto-negotiation
        if (j > 0) {
            ESP_LOGI(TAG, "Run with auto-negotiation disabled...");
            bool auto_nego_en = false;
            bool exp_autoneg_en;
            TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
            TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_AUTONEGO, &exp_autoneg_en));
            TEST_ASSERT_EQUAL(false, exp_autoneg_en);
// *** LAN8720 deviation ***
// Rationale: When the device is in manual 100BASE-TX or 10BASE-T modes with Auto-MDIX enabled, the PHY does not link to a
//            link partner that is configured for auto-negotiation. See LAN8720 errata for more details.
#ifdef CONFIG_TARGET_ETH_PHY_DEVICE_LAN8720
            esp_eth_phy_reg_rw_data_t reg;
            uint32_t reg_val;
            reg.reg_addr = 27;
            reg.reg_value_p = &reg_val;
            TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg));
            reg_val |= 0x8000;
            TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_WRITE_PHY_REG, &reg));
            uint32_t reg_val_act;
            reg.reg_value_p = &reg_val_act;
            TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &reg));
            TEST_ASSERT_EQUAL(reg_val, reg_val_act);
#endif
        }
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
    extra_cleanup();
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

TEST_CASE("ethernet download test", "[ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    // create TCP/IP netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);
    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    extra_eth_config(eth_handle);
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
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expect_digest, digest, sizeof(digest));

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
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
}
