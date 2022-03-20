#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "unity.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "sdkconfig.h"
#include "lwip/sockets.h"

#define ETH_START_BIT BIT(0)
#define ETH_STOP_BIT BIT(1)
#define ETH_CONNECT_BIT BIT(2)

#define ETH_BROADCAST_RECV_BIT BIT(0)
#define ETH_MULTICAST_RECV_BIT BIT(1)
#define ETH_UNICAST_RECV_BIT BIT(2)

#define POKE_REQ 0xFA
#define POKE_RESP 0xFB
#define DUMMY_TRAFFIC 0xFF

typedef struct {
    uint8_t dest[6];
    uint8_t src[6];
    uint16_t proto;
    uint8_t data[];
} __attribute__((__packed__)) emac_frame_t;

static void eth_event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data){
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)arg;
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        xEventGroupSetBits(eth_event_group, ETH_CONNECT_BIT);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        break;
    case ETHERNET_EVENT_START:
        xEventGroupSetBits(eth_event_group, ETH_START_BIT);
    break;
    case ETHERNET_EVENT_STOP:
        xEventGroupSetBits(eth_event_group, ETH_STOP_BIT);
        break;
    default:
        break;
    }
}

TEST_CASE("start_and_stop", "[esp_eth]")
{
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();  // apply default MAC configuration
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // create MAC instance
    TEST_ASSERT_NOT_NULL(mac);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG(); // apply default PHY configuration
#if defined(CONFIG_TARGET_ETH_PHY_DEVICE_IP101)
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config); // create PHY instance
#elif defined(CONFIG_TARGET_ETH_PHY_DEVICE_LAN87XX)
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
#endif
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_start(eth_handle)); // start Ethernet driver state machine

    EventBits_t bits = 0;
    bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, true, true, pdMS_TO_TICKS(3000));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);

    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_stop(eth_handle));

    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(3000));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);

    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete_default());
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    vEventGroupDelete(eth_event_group);
}

TEST_CASE("get_set_mac", "[esp_eth]")
{
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();  // apply default MAC configuration
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // create MAC instance
    TEST_ASSERT_NOT_NULL(mac);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG(); // apply default PHY configuration
#if defined(CONFIG_TARGET_ETH_PHY_DEVICE_IP101)
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config); // create PHY instance
#elif defined(CONFIG_TARGET_ETH_PHY_DEVICE_LAN87XX)
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
#endif
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_start(eth_handle)); // start Ethernet driver state machine

    EventBits_t bits = 0;
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);

    uint8_t mac_addr[6] = {};
    TEST_ASSERT_EQUAL(ESP_OK, mac->get_addr(mac, mac_addr));
    TEST_ASSERT_BITS(0b00000011, 0b00, mac_addr[0]);    // Check UL&IG, should be UI
    mac_addr[5] ^= mac_addr[4];
    TEST_ASSERT_EQUAL(ESP_OK, mac->set_addr(mac, mac_addr));
    uint8_t new_mac_addr[6] = {};
    TEST_ASSERT_EQUAL(ESP_OK, mac->get_addr(mac, new_mac_addr));
    TEST_ASSERT_EQUAL(0, memcmp(mac_addr, new_mac_addr, 6));

    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_stop(eth_handle));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete_default());
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    vEventGroupDelete(eth_event_group);
}

TEST_CASE("ethernet_broadcast_transmit", "[esp_eth]")
{
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();  // apply default MAC configuration
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // create MAC instance
    TEST_ASSERT_NOT_NULL(mac);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG(); // apply default PHY configuration
#if defined(CONFIG_TARGET_ETH_PHY_DEVICE_IP101)
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config); // create PHY instance
#elif defined(CONFIG_TARGET_ETH_PHY_DEVICE_LAN87XX)
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
#endif
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_start(eth_handle)); // start Ethernet driver state machine

    EventBits_t bits = 0;
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    // even if PHY (IP101) indicates autonegotiation done and link up, it sometimes may miss few packets after atonego reset, hence wait a bit
    vTaskDelay(pdMS_TO_TICKS(100));

    emac_frame_t *pkt = malloc(1024);
    pkt->proto = 0x2222;
    memset(pkt->dest, 0xff, 6);     // broadcast addr
    for (int i = 0; i < (1024 - ETH_HEADER_LEN); ++i){
        pkt->data[i] = i & 0xff;
    }

    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_transmit(eth_handle, pkt, 1024));
    vTaskDelay(pdMS_TO_TICKS(100));
    free(pkt);

    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_stop(eth_handle));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete_default());
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    vEventGroupDelete(eth_event_group);
}

static uint8_t local_mac_addr[6] = {};

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
        if (memcmp(pkt->dest, "\xff\xff\xff\xff\xff\xff", 6) == 0) {
            printf("broadcast received...\n");
            xEventGroupSetBits(eth_event_group, ETH_BROADCAST_RECV_BIT);
        } else if (pkt->dest[0] & 0x1) {
            printf("multicast received...\n");
            xEventGroupSetBits(eth_event_group, ETH_MULTICAST_RECV_BIT);
        } else if (memcmp(pkt->dest, local_mac_addr, 6) == 0) {
            printf("unicast received...\n");
            xEventGroupSetBits(eth_event_group, ETH_UNICAST_RECV_BIT);
        }
    } else {
        printf("unexpected frame (protocol: 0x%x, length: %u)\n", pkt->proto, length);
    }
    return ESP_OK;
};

TEST_CASE("recv_pkt", "[esp_eth]")
{
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();  // apply default MAC configuration
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // create MAC instance
    TEST_ASSERT_NOT_NULL(mac);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG(); // apply default PHY configuration
#if defined(CONFIG_TARGET_ETH_PHY_DEVICE_IP101)
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config); // create PHY instance
#elif defined(CONFIG_TARGET_ETH_PHY_DEVICE_LAN87XX)
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
#endif
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_start(eth_handle)); // start Ethernet driver state machine

    TEST_ASSERT_EQUAL(ESP_OK, mac->get_addr(mac, local_mac_addr));
    // test app will parse the DUT MAC from this line of log output
    printf("DUT MAC: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", local_mac_addr[0], local_mac_addr[1], local_mac_addr[2],
           local_mac_addr[3], local_mac_addr[4], local_mac_addr[5]);

    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_update_input_path(eth_handle, l2_packet_txrx_test_cb, eth_event_group));

    EventBits_t bits = 0;
    bits = xEventGroupWaitBits(eth_event_group, ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT,
                               true, true, pdMS_TO_TICKS(5000));
    TEST_ASSERT((bits & (ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT)) ==
                                 (ETH_BROADCAST_RECV_BIT | ETH_MULTICAST_RECV_BIT | ETH_UNICAST_RECV_BIT));

    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_stop(eth_handle));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete_default());
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
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

TEST_CASE("start_stop_stress_test", "[esp_eth]")
{
    recv_info_t recv_info;
    recv_info.mutex = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(recv_info.mutex);
    recv_info.rx_pkt_cnt = 0;

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();  // apply default MAC configuration
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config); // create MAC instance
    TEST_ASSERT_NOT_NULL(mac);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG(); // apply default PHY configuration
#if defined(CONFIG_TARGET_ETH_PHY_DEVICE_IP101)
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config); // create PHY instance
#elif defined(CONFIG_TARGET_ETH_PHY_DEVICE_LAN87XX)
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
#endif
    TEST_ASSERT_NOT_NULL(phy);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy); // apply default driver configuration
    esp_eth_handle_t eth_handle = NULL; // after driver installed, we will get the handle of the driver
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_install(&config, &eth_handle)); // install driver
    TEST_ASSERT_NOT_NULL(eth_handle);

    TEST_ASSERT_EQUAL(ESP_OK, mac->get_addr(mac, local_mac_addr));
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
    memset(ctrl_pkt->dest, 0xff, 6); // broadcast addr
    memcpy(ctrl_pkt->src, local_mac_addr, 6);

    // create dummy data packet used for traffic generation
    emac_frame_t *pkt = calloc(1, 1500);
    pkt->proto = 0x2222;
    // we don't care about dest MAC address much, however it is better to not be broadcast or multifcast to not flood
    // other network nodes
    memset(pkt->dest, 0xBA, 6);
    memcpy(pkt->src, local_mac_addr, 6);

    printf("EMAC start/stop stress test under heavy Tx traffic\n");
    for (int tx_i = 0; tx_i < 10; tx_i++) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_eth_start(eth_handle)); // start Ethernet driver state machine
        bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
        // even if PHY (IP101) indicates autonegotiation done and link up, it sometimes may miss few packets after atonego reset, hence wait a bit
        vTaskDelay(pdMS_TO_TICKS(100));

        // at first, check that Tx/Rx path works as expected by poking the test script
        // this also serves as main PASS/FAIL criteria
        ctrl_pkt->data[0] = POKE_REQ;
        ctrl_pkt->data[1] = tx_i;
        TEST_ASSERT_EQUAL(ESP_OK, esp_eth_transmit(eth_handle, ctrl_pkt, 60));
        TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(3000)));
        printf("Tx Test iteration %d\n", tx_i);

        // generate heavy Tx traffic
        printf("Note: transmit errors are expected...\n");
        for (int j = 0; j < 150; j++) {
            // return value is not checked on purpose since it is expected that it may fail time to time because
            // we may try to queue more packets than hardware is able to handle
            pkt->data[0] = j & 0xFF;
            esp_eth_transmit(eth_handle, pkt, 1500);
        }
        TEST_ASSERT_EQUAL(ESP_OK, esp_eth_stop(eth_handle));
        bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
        printf("Ethernet stopped\n");
    }

    printf("EMAC start/stop stress test under heavy Rx traffic\n");
    for (int rx_i = 0; rx_i < 10; rx_i++) {
        recv_info.rx_pkt_cnt = 0;
        TEST_ASSERT_EQUAL(ESP_OK, esp_eth_start(eth_handle)); // start Ethernet driver state machine
        bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
        // even if PHY (IP101) indicates autonegotiation done and link up, it sometimes may miss few packets after atonego reset, hence wait a bit
        vTaskDelay(pdMS_TO_TICKS(100));

        ctrl_pkt->data[0] = POKE_REQ;
        ctrl_pkt->data[1] = rx_i;
        TEST_ASSERT_EQUAL(ESP_OK, esp_eth_transmit(eth_handle, ctrl_pkt, 60));
        TEST_ASSERT(xSemaphoreTake(recv_info.mutex, pdMS_TO_TICKS(3000)));
        printf("Rx Test iteration %d\n", rx_i);

        vTaskDelay(pdMS_TO_TICKS(500));

        TEST_ASSERT_EQUAL(ESP_OK, esp_eth_stop(eth_handle));
        bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(3000));
        TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
        printf("Recv packets: %d\n", recv_info.rx_pkt_cnt);
        TEST_ASSERT_GREATER_THAN_INT32(0, recv_info.rx_pkt_cnt);
        printf("Ethernet stopped\n");
    }

    free(ctrl_pkt);
    free(pkt);

    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_delete_default());
    TEST_ASSERT_EQUAL(ESP_OK, esp_eth_driver_uninstall(eth_handle));
    phy->del(phy);
    mac->del(mac);
    vEventGroupDelete(eth_event_group);
    vSemaphoreDelete(recv_info.mutex);
}

void app_main(void)
{
    unity_run_menu();
}
