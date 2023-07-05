/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_eth_test_common.h"
#include "arpa/inet.h" // for ntohs, etc.
#include "esp_log.h"

#define TEST_ETH_TYPE 0x3300
#define TEST_CTRL_ETH_TYPE (TEST_ETH_TYPE + 1)

#define WAIT_AFTER_CONN_MS 2500
#define WAIT_AFTER_CONN_TMO_MS 20000

#define ETH_BROADCAST_RECV_BIT BIT(0)
#define ETH_MULTICAST_RECV_BIT BIT(1)
#define ETH_UNICAST_RECV_BIT BIT(2)
#define ETH_POKE_RESP_RECV_BIT BIT(3)

#define POKE_REQ 0xFA
#define POKE_RESP 0xFB
#define DUMMY_TRAFFIC 0xFF

typedef struct
{
    EventGroupHandle_t eth_event_group;
    uint8_t dst_mac_addr[ETH_ADDR_LEN];
    int unicast_rx_cnt;
    int multicast_rx_cnt;
    int brdcast_rx_cnt;

    bool check_rx_data;
} recv_info_t;


esp_err_t l2_packet_txrx_test_cb(esp_eth_handle_t hdl, uint8_t *buffer, uint32_t length, void *priv) {
    recv_info_t *recv_info = (recv_info_t*)priv;
    EventGroupHandle_t eth_event_group = recv_info->eth_event_group;
    emac_frame_t *pkt = (emac_frame_t *)buffer;
    // check header
    if (pkt->proto == ntohs(TEST_ETH_TYPE)) { // data packet
        uint8_t local_mac_addr[ETH_ADDR_LEN];
        esp_eth_ioctl(hdl, ETH_CMD_G_MAC_ADDR, local_mac_addr);
        // check data content
        if (recv_info->check_rx_data) {
            if (length == 1024) {
                for (int i = 0; i < (length - ETH_HEADER_LEN); ++i) {
                    if (pkt->data[i] != (i & 0xff)) {
                        printf("payload mismatch\n");
                        free(buffer);
                        return ESP_OK;
                    }
                }
            }
        }

        if (memcmp(pkt->dest, "\xff\xff\xff\xff\xff\xff", ETH_ADDR_LEN) == 0) {
            recv_info->brdcast_rx_cnt++;
            xEventGroupSetBits(eth_event_group, ETH_BROADCAST_RECV_BIT);
        } else if (pkt->dest[0] & 0x1) {
            recv_info->multicast_rx_cnt++;
            xEventGroupSetBits(eth_event_group, ETH_MULTICAST_RECV_BIT);
        } else if (memcmp(pkt->dest, local_mac_addr, ETH_ADDR_LEN) == 0) {
            recv_info->unicast_rx_cnt++;
            xEventGroupSetBits(eth_event_group, ETH_UNICAST_RECV_BIT);
        }
    } else if (ntohs(pkt->proto) == TEST_CTRL_ETH_TYPE) { // control packet
        if (pkt->data[0] == POKE_RESP) {
            memcpy(recv_info->dst_mac_addr, pkt->dest, ETH_ADDR_LEN);
            printf("Poke response received\n");
            xEventGroupSetBits(eth_event_group, ETH_POKE_RESP_RECV_BIT);
        }
    }
    free(buffer);
    return ESP_OK;
}

/**
 * @brief The function sends a "POKE" request message over the Ethernet and waits until the test script sends a reply.
 *        Multiple "POKE" attempts are issued when timeout for the reply expires.
 *        This function is used to drive the test flow and to ensure that data path between the test points
 *        has been established. I.e. if DUT is connected in network with a switch, even if link is indicated up,
 *        it may take some time the switch starts forwarding the associated port (e.g. it runs RSTP at first).
 */
void poke_and_wait(esp_eth_handle_t eth_handle, void *data, uint16_t size, EventGroupHandle_t eth_event_group)
{
    // create a control frame to control test flow between the UT and the Python test script
    emac_frame_t *ctrl_pkt = calloc(1, 60);
    ctrl_pkt->proto = htons(TEST_CTRL_ETH_TYPE);
    memset(ctrl_pkt->dest, 0xff, ETH_ADDR_LEN); // broadcast addr
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, ctrl_pkt->src);

    ctrl_pkt->data[0] = POKE_REQ;
    if (data != NULL && size > 0) {
        memcpy(&ctrl_pkt->data[1], data, size);
    }

    uint32_t tmo;
    uint32_t i;
    for(tmo = 0, i = 1; tmo < WAIT_AFTER_CONN_TMO_MS; tmo += WAIT_AFTER_CONN_MS, i++) {
        printf("Poke attempt #%" PRIu32 "\n", i);
        TEST_ESP_OK(esp_eth_transmit(eth_handle, ctrl_pkt, 60));
        EventBits_t bits = xEventGroupWaitBits(eth_event_group, ETH_POKE_RESP_RECV_BIT,
                               true, true, pdMS_TO_TICKS(WAIT_AFTER_CONN_MS));
        if ((bits & ETH_POKE_RESP_RECV_BIT) == ETH_POKE_RESP_RECV_BIT) {
            break;
        }
    }
    TEST_ASSERT(tmo < WAIT_AFTER_CONN_TMO_MS);
    free(ctrl_pkt);
}

TEST_CASE("ethernet broadcast transmit", "[ethernet_l2]")
{
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
    EventGroupHandle_t eth_event_state_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_state_group != NULL);
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_state_group));
    EventGroupHandle_t eth_event_rx_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_rx_group != NULL);
    recv_info_t recv_info = {
        .eth_event_group = eth_event_rx_group,
        .check_rx_data = false,
        .unicast_rx_cnt = 0,
        .multicast_rx_cnt = 0,
        .brdcast_rx_cnt = 0
    };

    uint8_t local_mac_addr[ETH_ADDR_LEN] = {};
    TEST_ESP_OK(mac->get_addr(mac, local_mac_addr));
    // test app will parse the DUT MAC from this line of log output
    printf("DUT MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", local_mac_addr[0], local_mac_addr[1], local_mac_addr[2],
           local_mac_addr[3], local_mac_addr[4], local_mac_addr[5]);

    TEST_ESP_OK(esp_eth_update_input_path(eth_handle, l2_packet_txrx_test_cb, &recv_info));
    TEST_ESP_OK(esp_eth_start(eth_handle)); // start Ethernet driver state machine

    EventBits_t bits = 0;
    bits = xEventGroupWaitBits(eth_event_state_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    // if DUT is connected in network with switch: even if link is indicated up, it may take some time the switch
    // starts switching the associated port (e.g. it runs RSTP at first)
    poke_and_wait(eth_handle, NULL, 0, eth_event_rx_group);

    emac_frame_t *pkt = malloc(1024);
    pkt->proto = htons(TEST_ETH_TYPE);
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, pkt->src));
    memset(pkt->dest, 0xff, ETH_ADDR_LEN);     // broadcast addr
    for (int i = 0; i < (1024 - ETH_HEADER_LEN); ++i){
        pkt->data[i] = i & 0xff;
    }

    TEST_ESP_OK(esp_eth_transmit(eth_handle, pkt, 1024));
    // give it some time to complete transmit
    vTaskDelay(pdMS_TO_TICKS(500));
    free(pkt);

    TEST_ESP_OK(esp_eth_stop(eth_handle));
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    extra_cleanup();
    vEventGroupDelete(eth_event_rx_group);
    vEventGroupDelete(eth_event_state_group);
}

TEST_CASE("ethernet recv_pkt", "[ethernet_l2]")
{
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
    EventGroupHandle_t eth_event_state_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_state_group != NULL);
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_state_group));
    EventGroupHandle_t eth_event_rx_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_rx_group != NULL);
    recv_info_t recv_info = {
        .eth_event_group = eth_event_rx_group,
        .check_rx_data = true,
        .unicast_rx_cnt = 0,
        .multicast_rx_cnt = 0,
        .brdcast_rx_cnt = 0
    };

    uint8_t local_mac_addr[ETH_ADDR_LEN] = {};
    TEST_ESP_OK(mac->get_addr(mac, local_mac_addr));
    // test app will parse the DUT MAC from this line of log output
    printf("DUT MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", local_mac_addr[0], local_mac_addr[1], local_mac_addr[2],
           local_mac_addr[3], local_mac_addr[4], local_mac_addr[5]);

    TEST_ESP_OK(esp_eth_update_input_path(eth_handle, l2_packet_txrx_test_cb, &recv_info));
    TEST_ESP_OK(esp_eth_start(eth_handle)); // start Ethernet driver state machine

    EventBits_t bits = 0;
    bits = xEventGroupWaitBits(eth_event_state_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    // if DUT is connected in network with switch: even if link is indicated up, it may take some time the switch
    // starts switching the associated port (e.g. it runs RSTP at first)
    poke_and_wait(eth_handle, NULL, 0, eth_event_rx_group);

    bits = 0;
    bits = xEventGroupWaitBits(eth_event_rx_group, ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT,
                               true, true, pdMS_TO_TICKS(5000));
    printf("bits = 0x%" PRIu32 "\n", (uint32_t)bits & (ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT));
    TEST_ASSERT((bits & (ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT)) ==
                                 (ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT));

    TEST_ESP_OK(esp_eth_stop(eth_handle));
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    extra_cleanup();
    vEventGroupDelete(eth_event_state_group);
    vEventGroupDelete(eth_event_rx_group);
}


TEST_CASE("ethernet start/stop stress test under heavy traffic", "[ethernet_l2]")
{
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
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_state_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_state_group != NULL);
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_state_group));
    EventGroupHandle_t eth_event_rx_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_rx_group != NULL);
    recv_info_t recv_info = {
        .eth_event_group = eth_event_rx_group,
        .check_rx_data = false,
        .unicast_rx_cnt = 0,
        .multicast_rx_cnt = 0,
        .brdcast_rx_cnt = 0
    };

    uint8_t local_mac_addr[ETH_ADDR_LEN] = {};
    TEST_ESP_OK(mac->get_addr(mac, local_mac_addr));
    // test app will parse the DUT MAC from this line of log output
    printf("DUT MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", local_mac_addr[0], local_mac_addr[1], local_mac_addr[2],
           local_mac_addr[3], local_mac_addr[4], local_mac_addr[5]);

    TEST_ESP_OK(esp_eth_update_input_path(eth_handle, l2_packet_txrx_test_cb, &recv_info));

    // create dummy data packet used for traffic generation
    emac_frame_t *pkt = calloc(1, 1500);
    pkt->proto = htons(TEST_ETH_TYPE);
    memcpy(pkt->dest, recv_info.dst_mac_addr, ETH_ADDR_LEN);
    memcpy(pkt->src, local_mac_addr, ETH_ADDR_LEN);

    printf("EMAC start/stop stress test under heavy Tx traffic\n");
    for (int tx_i = 0; tx_i < 10; tx_i++) {
        printf("Tx Test iteration %d\n", tx_i);
        TEST_ESP_OK(esp_eth_start(eth_handle)); // start Ethernet driver state machine
        bits = xEventGroupWaitBits(eth_event_state_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
        // at first, check that Tx/Rx path works as expected by poking the test script
        // this also serves as main PASS/FAIL criteria
        poke_and_wait(eth_handle, &tx_i, sizeof(tx_i), eth_event_rx_group);

        // generate heavy Tx traffic
        printf("Note: transmit errors are expected...\n");
        for (int j = 0; j < 150; j++) {
            // return value is not checked on purpose since it is expected that it may fail time to time because
            // we may try to queue more packets than hardware is able to handle
            pkt->data[2] = j & 0xFF; // sequence number
            esp_eth_transmit(eth_handle, pkt, 1500);
        }
        TEST_ESP_OK(esp_eth_stop(eth_handle));
        bits = xEventGroupWaitBits(eth_event_state_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
        printf("Ethernet stopped\n");
    }

    printf("EMAC start/stop stress test under heavy Rx traffic\n");
    for (int rx_i = 0; rx_i < 10; rx_i++) {
        printf("Rx Test iteration %d\n", rx_i);
        TEST_ESP_OK(esp_eth_start(eth_handle)); // start Ethernet driver state machine
        bits = xEventGroupWaitBits(eth_event_state_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
        poke_and_wait(eth_handle, &rx_i, sizeof(rx_i), eth_event_rx_group);

        // wait for dummy traffic
        xEventGroupClearBits(eth_event_rx_group, ETH_UNICAST_RECV_BIT);
        recv_info.unicast_rx_cnt = 0;
        bits = xEventGroupWaitBits(eth_event_rx_group, ETH_UNICAST_RECV_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_UNICAST_RECV_BIT) == ETH_UNICAST_RECV_BIT);

        vTaskDelay(pdMS_TO_TICKS(500));

        TEST_ESP_OK(esp_eth_stop(eth_handle));
        bits = xEventGroupWaitBits(eth_event_state_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
        printf("Recv packets: %d\n", recv_info.unicast_rx_cnt);
        TEST_ASSERT_GREATER_THAN_INT32(0, recv_info.unicast_rx_cnt);
        printf("Ethernet stopped\n");
    }

    free(pkt);

    // Add an extra delay to be sure that there is no traffic generated by the test script during the driver un-installation.
    // It was observed unintended behavior of the switch used in test environment when link is set down under heavy load.
    vTaskDelay(pdMS_TO_TICKS(500));

    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    extra_cleanup();
    vEventGroupDelete(eth_event_rx_group);
    vEventGroupDelete(eth_event_state_group);
}
