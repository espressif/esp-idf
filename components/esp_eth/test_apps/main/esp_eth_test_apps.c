/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
#include "esp_eth_test_utils.h"
#include "unity.h"

#define LOOPBACK_TEST_PACKET_SIZE 256

static const char *TAG = "esp32_eth_test";

extern const char dl_espressif_com_root_cert_pem_start[] asm("_binary_dl_espressif_com_root_cert_pem_start");
extern const char dl_espressif_com_root_cert_pem_end[]   asm("_binary_dl_espressif_com_root_cert_pem_end");

// compute md5 of download file
static md5_context_t md5_context;
static uint8_t digest[16];

// Basic test to verify that the Ethernet driver can be initialized and deinitialized
TEST_CASE("ethernet init/deinit test", "[ethernet],[skip_setup_teardown]")
{
    esp_eth_handle_t eth_handle = NULL;

    TEST_ESP_OK(esp_eth_test_eth_init(&eth_handle));
    TEST_ESP_OK(esp_eth_test_eth_deinit(eth_handle));
}

TEST_CASE("ethernet io test", "[ethernet]")
{
    // get handles from common module initialized by setUp()
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();

    /* get default MAC address */
    uint8_t mac_addr[ETH_ADDR_LEN];
    memset(mac_addr, 0, sizeof(mac_addr));
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr));
    ESP_LOGI(TAG, "Ethernet MAC Address: %02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    TEST_ASSERT(mac_addr[0] != 0);
// *** SPI Ethernet modules deviation ***
// Rationale: The SPI Ethernet modules don't have a burned default factory MAC address hence local MAC is used
#if CONFIG_ETH_TEST_MAC_ADDR_UI
    TEST_ASSERT_BITS(0b00000011, 0b00, mac_addr[0]);    // Check UL&IG, should be UI
#else
    TEST_ASSERT_BITS(0b00000011, 0b10, mac_addr[0]);    // Check UL&IG, should be U
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
#if CONFIG_ETH_TEST_PHY_ADDRESS_DISABLED
    /* get PHY address */
    int phy_addr = -1;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_PHY_ADDR, &phy_addr));
    ESP_LOGI(TAG, "Ethernet PHY Address: %d", phy_addr);
    TEST_ASSERT(phy_addr >= 0 && phy_addr <= 31);
#endif
}

// This test expects autonegotiation to be enabled on the other node.
TEST_CASE("ethernet io speed/duplex/autonegotiation", "[ethernet]")
{
    // get handles from common module initialized by setUp()
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();

    EventBits_t bits = 0;
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
#if CONFIG_ETH_TEST_LAN8720_ERRATA_ENABLED
    TEST_ESP_OK(eth_test_set_phy_reg_bits(eth_handle, 27, 0x8000, 3));
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
    TEST_ESP_OK(eth_test_clear_phy_reg_bits(eth_handle, 27, 0x8000, 3));
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
}

// use static semaphore to avoid dynamic allocation and so need for de-allocation in case of test failure
static SemaphoreHandle_t loopback_test_case_data_received;
static StaticSemaphore_t loopback_test_case_data_received_buffer;
static esp_err_t loopback_test_case_incoming_handler(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv)
{
    TEST_ASSERT(memcmp(priv, buffer, LOOPBACK_TEST_PACKET_SIZE) == 0);
    xSemaphoreGive(loopback_test_case_data_received);
    free(buffer);
    return ESP_OK;
}

TEST_CASE("ethernet io loopback", "[ethernet]")
{
    // get handles from common module initialized by setUp()
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();

    loopback_test_case_data_received = xSemaphoreCreateBinaryStatic(&loopback_test_case_data_received_buffer);
    EventBits_t bits = 0;
    // Disable autonegotiation to manually set speed and duplex mode
    bool auto_nego_en = false;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    bool loopback_en = true;
// *** PHY loopback not supported deviation ***
// Rationale: Some PHYs do not support loopback at all
#if CONFIG_ETH_TEST_LOOPBACK_DISABLED
    TEST_ASSERT(esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en) == ESP_ERR_NOT_SUPPORTED);
    return;
#else
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en));
#endif

    eth_duplex_t duplex_modes[] = {ETH_DUPLEX_HALF, ETH_DUPLEX_FULL};
    eth_speed_t speeds[] = {ETH_SPEED_100M, ETH_SPEED_10M};
    emac_frame_t* test_packet = (emac_frame_t*)eth_test_alloc(LOOPBACK_TEST_PACKET_SIZE);
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, test_packet->src);
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, test_packet->dest);
    for(size_t i = 0; i < LOOPBACK_TEST_PACKET_SIZE-ETH_HEADER_LEN; i++){
        test_packet->data[i] = rand() & 0xff;
    }
    TEST_ESP_OK(esp_eth_update_input_path(eth_handle, loopback_test_case_incoming_handler, test_packet));

    for (int i = 0; i < sizeof(speeds) / sizeof(eth_speed_t); i++) {
        eth_speed_t expected_speed = speeds[i];
        for (int j = 0; j < sizeof(duplex_modes) / sizeof(eth_duplex_t); j++) {
            eth_duplex_t expected_duplex = duplex_modes[j];
            ESP_LOGI(TAG, "Test with %s Mbps %s duplex.", expected_speed == ETH_SPEED_10M ? "10" : "100", expected_duplex == ETH_DUPLEX_HALF ? "half" : "full");
// *** 10 Mbps loopback disabled deviation ***
// Rationale: Some PHYs do not support loopback at 10 Mbps
#if CONFIG_ETH_TEST_10MB_LOOPBACK_DISABLED
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
            /* 10Mbps RMII loopback may have timing issues due to clock division architecture. RMII CLK stays 50MHz while the data signal must be held still for 10 cycles to
            achieve the speed reduction. Everything, including control signals must be perfectly synchronized. This may be a challenge for some PHYs or PCB layouts.*/
            if (expected_speed == ETH_SPEED_10M) {
                int i;
                for (i = 0; i < 3; i++) {
                    if(xSemaphoreTake(loopback_test_case_data_received, pdMS_TO_TICKS(1000)) != pdTRUE) {
                        ESP_LOGW(TAG, "Timeout waiting for data received for 10 Mbps mode, trying again...");
                        TEST_ESP_OK(esp_eth_stop(eth_handle));
                        bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
                        TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
                        TEST_ESP_OK(esp_eth_start(eth_handle));
                        bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
                        TEST_ESP_OK(esp_eth_transmit(eth_handle, test_packet, LOOPBACK_TEST_PACKET_SIZE));
                    } else {
                        break;
                    }
                }
// *** 10 Mbps loopback ignore failures deviation ***
// Rationale: 10 Mbps loopback may be supported by PHY but the test is not reliable.
#if !CONFIG_ETH_TEST_10MB_LOOPBACK_IGNORE_FAILURES
                TEST_ASSERT_LESS_THAN(3, i);
#endif
            } else {
                TEST_ASSERT(xSemaphoreTake(loopback_test_case_data_received, pdMS_TO_TICKS(1000)) == pdTRUE);
            }
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
// *** Loopback with autonegotiation deviation ***
// Rationale: Some PHYs do not support autonegotiation with loopback enabled.
#if CONFIG_ETH_TEST_LOOPBACK_WITH_AUTONEGOTIATION_DISABLED
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    // Test passes - these devices correctly report autonegotiation is not supported with loopback
    return;
#endif
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    TEST_ESP_OK(esp_eth_start(eth_handle));
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));

    TEST_ESP_OK(esp_eth_transmit(eth_handle, test_packet, LOOPBACK_TEST_PACKET_SIZE));
    TEST_ASSERT(xSemaphoreTake(loopback_test_case_data_received, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS)) == pdTRUE);

    loopback_en = false;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en));
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
}

TEST_CASE("ethernet event test", "[ethernet]")
{
    // get handles from common module initialized by setUp()
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();

    EventBits_t bits = 0;
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
}

TEST_CASE("ethernet dhcp test", "[ethernet][esp-netif]")
{
    // get handles from common module initialized by setUp()
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();

    EventBits_t bits = 0;
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

TEST_CASE("ethernet start/stop stress test with IP stack", "[ethernet][esp-netif]")
{
    // get handles from common module initialized by setUp()
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();

    EventBits_t bits = 0;

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
#if CONFIG_ETH_TEST_LAN8720_ERRATA_ENABLED
            TEST_ESP_OK(eth_test_set_phy_reg_bits(eth_handle, 27, 0x8000, 3));
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
    case HTTP_EVENT_ON_HEADERS_COMPLETE:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADERS_COMPLETE");
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
    default:
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

TEST_CASE("ethernet download test", "[ethernet][esp-netif]")
{
    // get handles from common module initialized by setUp()
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();

    EventBits_t bits = 0;
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
}
