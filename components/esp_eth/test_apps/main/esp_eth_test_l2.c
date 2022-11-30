/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_eth_test_common.h"

#define ETH_BROADCAST_RECV_BIT BIT(0)
#define ETH_MULTICAST_RECV_BIT BIT(1)
#define ETH_UNICAST_RECV_BIT BIT(2)

#define POKE_REQ 0xFA
#define POKE_RESP 0xFB
#define DUMMY_TRAFFIC 0xFF

TEST_CASE("ethernet broadcast transmit", "[ethernet_l2]")
{
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);

    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ESP_OK(esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);
    extra_eth_config(eth_handle);
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_eth_start(eth_handle)); // start Ethernet driver state machine

    EventBits_t bits = 0;
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    // if DUT is connected in network with switch: even if link is indicated up, it may take some time the switch
    // starts switching the associated port (e.g. it runs RSTP at first)
    vTaskDelay(pdMS_TO_TICKS(1000));

    emac_frame_t *pkt = malloc(1024);
    pkt->proto = 0x2222;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, pkt->src));
    memset(pkt->dest, 0xff, ETH_ADDR_LEN);     // broadcast addr
    for (int i = 0; i < (1024 - ETH_HEADER_LEN); ++i){
        pkt->data[i] = i & 0xff;
    }

    TEST_ESP_OK(esp_eth_transmit(eth_handle, pkt, 1024));
    vTaskDelay(pdMS_TO_TICKS(100));
    free(pkt);

    TEST_ESP_OK(esp_eth_stop(eth_handle));
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
}

static uint8_t local_mac_addr[ETH_ADDR_LEN] = {};

esp_err_t l2_packet_txrx_test_cb(esp_eth_handle_t hdl, uint8_t *buffer, uint32_t length, void *priv) {
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)priv;
    emac_frame_t *pkt = (emac_frame_t *) buffer;
    // check header
    if (pkt->proto == 0x2222 && length == 1024) {
        // check content
        for (int i = 0; i < (length - ETH_HEADER_LEN); ++i) {
            if (pkt->data[i] != (i & 0xff)) {
                printf("payload mismatch\n");
                return ESP_OK;
            }
        }
        if (memcmp(pkt->dest, "\xff\xff\xff\xff\xff\xff", ETH_ADDR_LEN) == 0) {
            printf("broadcast received...\n");
            xEventGroupSetBits(eth_event_group, ETH_BROADCAST_RECV_BIT);
        } else if (pkt->dest[0] & 0x1) {
            printf("multicast received...\n");
            xEventGroupSetBits(eth_event_group, ETH_MULTICAST_RECV_BIT);
        } else if (memcmp(pkt->dest, local_mac_addr, ETH_ADDR_LEN) == 0) {
            printf("unicast received...\n");
            xEventGroupSetBits(eth_event_group, ETH_UNICAST_RECV_BIT);
        }
    } else {
        printf("unexpected frame (protocol: 0x%x, length: %" PRIu32 ")\n", pkt->proto, length);
    }
    return ESP_OK;
};

TEST_CASE("ethernet recv_pkt", "[ethernet_l2]")
{
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);

    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ESP_OK(esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);
    extra_eth_config(eth_handle);
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_eth_start(eth_handle)); // start Ethernet driver state machine

    TEST_ESP_OK(mac->get_addr(mac, local_mac_addr));
    // test app will parse the DUT MAC from this line of log output
    printf("DUT MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", local_mac_addr[0], local_mac_addr[1], local_mac_addr[2],
           local_mac_addr[3], local_mac_addr[4], local_mac_addr[5]);

    TEST_ESP_OK(esp_eth_update_input_path(eth_handle, l2_packet_txrx_test_cb, eth_event_group));

    EventBits_t bits = 0;
    bits = xEventGroupWaitBits(eth_event_group, ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT,
                               true, true, pdMS_TO_TICKS(5000));
    TEST_ASSERT((bits & (ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT)) ==
                                 (ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT));

    TEST_ESP_OK(esp_eth_stop(eth_handle));
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
}

typedef struct
{
    SemaphoreHandle_t mutex;
    int rx_pkt_cnt;
} recv_info_t;

static esp_err_t eth_recv_cb(esp_eth_handle_t hdl, uint8_t *buffer, uint32_t length, void *priv)
{
    emac_frame_t *pkt = (emac_frame_t *)buffer;
    recv_info_t *recv_info = (recv_info_t *)priv;

    if (pkt->proto == 0x2222) {
        switch (pkt->data[0])
        {
        case POKE_RESP:
            xSemaphoreGive(recv_info->mutex);
            break;

        case DUMMY_TRAFFIC:
            (recv_info->rx_pkt_cnt)++;
            break;
        default:
            break;
        }
    }
    free(buffer);
    return ESP_OK;
}

TEST_CASE("ethernet start/stop stress test under heavy traffic", "[ethernet_l2]")
{
    recv_info_t recv_info;
    recv_info.mutex = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(recv_info.mutex);
    recv_info.rx_pkt_cnt = 0;

    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ESP_OK(esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);
    extra_eth_config(eth_handle);

    TEST_ESP_OK(mac->get_addr(mac, local_mac_addr));
    // test app will parse the DUT MAC from this line of log output
    printf("DUT MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", local_mac_addr[0], local_mac_addr[1], local_mac_addr[2],
           local_mac_addr[3], local_mac_addr[4], local_mac_addr[5]);

    TEST_ESP_OK(esp_eth_update_input_path(eth_handle, eth_recv_cb, &recv_info));

    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));

    // create a control frame to control test flow between the UT and the Python test script
    emac_frame_t *ctrl_pkt = calloc(1, 60);
    ctrl_pkt->proto = 0x2222;
    memset(ctrl_pkt->dest, 0xff, ETH_ADDR_LEN); // broadcast addr
    memcpy(ctrl_pkt->src, local_mac_addr, ETH_ADDR_LEN);

    // create dummy data packet used for traffic generation
    emac_frame_t *pkt = calloc(1, 1500);
    pkt->proto = 0x2222;
    // we don't care about dest MAC address much, however it is better to not be broadcast or multifcast to not flood
    // other network nodes
    memset(pkt->dest, 0xBA, ETH_ADDR_LEN);
    memcpy(pkt->src, local_mac_addr, ETH_ADDR_LEN);

    printf("EMAC start/stop stress test under heavy Tx traffic\n");
    for (int tx_i = 0; tx_i < 10; tx_i++) {
        TEST_ESP_OK(esp_eth_start(eth_handle)); // start Ethernet driver state machine
        bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
        // if DUT is connected in network with switch: even if link is indicated up, it may take some time the switch
        // starts switching the associated port (e.g. it runs RSTP at first)
        vTaskDelay(pdMS_TO_TICKS(1000));

        // at first, check that Tx/Rx path works as expected by poking the test script
        // this also serves as main PASS/FAIL criteria
        ctrl_pkt->data[0] = POKE_REQ;
        ctrl_pkt->data[1] = tx_i;
        TEST_ESP_OK(esp_eth_transmit(eth_handle, ctrl_pkt, 60));
        TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(3000)));
        printf("Tx Test iteration %d\n", tx_i);

        // generate heavy Tx traffic
        printf("Note: transmit errors are expected...\n");
        for (int j = 0; j < 150; j++) {
            // return value is not checked on purpose since it is expected that it may fail time to time because
            // we may try to queue more packets than hardware is able to handle
            pkt->data[0] = j & 0xFF; // sequence number
            esp_eth_transmit(eth_handle, pkt, 1500);
        }
        TEST_ESP_OK(esp_eth_stop(eth_handle));
        bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
        printf("Ethernet stopped\n");
    }

    printf("EMAC start/stop stress test under heavy Rx traffic\n");
    for (int rx_i = 0; rx_i < 10; rx_i++) {
        recv_info.rx_pkt_cnt = 0;
        TEST_ESP_OK(esp_eth_start(eth_handle)); // start Ethernet driver state machine
        bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
        // if DUT is connected in network with switch: even if link is indicated up, it may take some time the switch
        // starts switching the associated port (e.g. it runs RSTP at first)
        vTaskDelay(pdMS_TO_TICKS(1000));

        ctrl_pkt->data[0] = POKE_REQ;
        ctrl_pkt->data[1] = rx_i;
        TEST_ESP_OK(esp_eth_transmit(eth_handle, ctrl_pkt, 60));
        TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(3000)));
        printf("Rx Test iteration %d\n", rx_i);

        vTaskDelay(pdMS_TO_TICKS(500));

        TEST_ESP_OK(esp_eth_stop(eth_handle));
        bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
        printf("Recv packets: %d\n", recv_info.rx_pkt_cnt);
        TEST_ASSERT_GREATER_THAN_INT32(0, recv_info.rx_pkt_cnt);
        printf("Ethernet stopped\n");
    }

    free(ctrl_pkt);
    free(pkt);

    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
    vSemaphoreDelete(recv_info.mutex);
}
