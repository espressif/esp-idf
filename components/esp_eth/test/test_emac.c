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
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "ping/ping_sock.h"

#define GOT_IP_BIT BIT(0)
#define ETHERNET_GET_IP_TIMEOUT_MS (20000)

static const char *TAG = "esp_eth_test";

#define ETH_START_BIT BIT(0)
#define ETH_STOP_BIT BIT(1)
#define ETH_CONNECT_BIT BIT(2)
#define ETH_GOT_IP_BIT BIT(3)
#define ETH_PING_END_BIT BIT(4)

#define ETH_START_TIMEOUT_MS (10000)
#define ETH_CONNECT_TIMEOUT_MS (40000)
#define ETH_STOP_TIMEOUT_MS (10000)
#define ETH_GET_IP_TIMEOUT_MS (60000)
#define ETH_PING_DURATION_MS (5000)
#define ETH_PING_END_TIMEOUT_MS (ETH_PING_DURATION_MS * 2)

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

static void test_on_ping_success(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    printf("%d bytes from %s icmp_seq=%d ttl=%d time=%d ms\n",
           recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
}

static void test_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    printf("From %s icmp_seq=%d timeout\n", inet_ntoa(target_addr.u_addr.ip4), seqno);
}

static void test_on_ping_end(esp_ping_handle_t hdl, void *args)
{
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)args;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;

    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
    printf("%d packets transmitted, %d received, time %dms\n", transmitted, received, total_time_ms);
    if (transmitted == received) {
        xEventGroupSetBits(eth_event_group, ETH_PING_END_BIT);
    }
}

TEST_CASE("esp32 ethernet io test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    TEST_ESP_OK(esp_event_loop_create_default());
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    vTaskDelay(pdMS_TO_TICKS(1000));
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
    TEST_ESP_OK(esp_event_loop_delete_default());
}

TEST_CASE("esp32 ethernet event test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    TEST_ESP_OK(esp_event_loop_create_default());
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t* eth_netif = esp_netif_new(&cfg);
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    TEST_ESP_OK(esp_netif_attach(eth_netif, eth_handle));
    /* wait for connection start */
    bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, true, true, pdMS_TO_TICKS(ETH_START_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);
    /* wait for connection establish */
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, true, true, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    // "check link timer callback" might owned the reference of phy object, make sure it has release it
    vTaskDelay(pdMS_TO_TICKS(2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
    esp_netif_destroy(eth_netif);
}

TEST_CASE("esp32 ethernet dhcp test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t* eth_netif = esp_netif_new(&cfg);
    TEST_ESP_OK(esp_eth_set_default_handlers(eth_netif));

    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    TEST_ESP_OK(esp_netif_attach(eth_netif, eth_handle));

    /* wait for IP lease */
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    // "check link timer callback" might owned the reference of phy object, make sure it has release it
    vTaskDelay(pdMS_TO_TICKS(2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    TEST_ESP_OK(esp_eth_clear_default_handlers(eth_netif));
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
    esp_netif_destroy(eth_netif);
}

TEST_CASE("esp32 ethernet icmp test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t* eth_netif = esp_netif_new(&cfg);
    TEST_ESP_OK(esp_eth_set_default_handlers(eth_netif));

    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    TEST_ESP_OK(esp_netif_attach(eth_netif, eth_handle));
    /* wait for IP lease */
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);

    // Parse IP address
    ip_addr_t target_addr;
    struct addrinfo hint;
    struct addrinfo *res = NULL;
    memset(&hint, 0, sizeof(hint));
    memset(&target_addr, 0, sizeof(target_addr));
    /* convert URL to IP */
    TEST_ASSERT(getaddrinfo("www.baidu.com", NULL, &hint, &res) == 0);
    struct in_addr addr4 = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
    inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
    freeaddrinfo(res);

    esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
    ping_config.target_addr = target_addr;
    ping_config.count = 0; // ping in infinite mode
    /* set callback functions */
    esp_ping_callbacks_t cbs;
    cbs.on_ping_success = test_on_ping_success;
    cbs.on_ping_timeout = test_on_ping_timeout;
    cbs.on_ping_end = test_on_ping_end;
    cbs.cb_args = eth_event_group;

    esp_ping_handle_t ping;
    TEST_ESP_OK(esp_ping_new_session(&ping_config, &cbs, &ping));
    /* start ping */
    TEST_ESP_OK(esp_ping_start(ping));
    /* ping for a while */
    vTaskDelay(pdMS_TO_TICKS(ETH_PING_DURATION_MS));
    /* stop ping */
    TEST_ESP_OK(esp_ping_stop(ping));
    /* wait for end of ping */
    bits = xEventGroupWaitBits(eth_event_group, ETH_PING_END_BIT, true, true, pdMS_TO_TICKS(ETH_PING_END_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_PING_END_BIT) == ETH_PING_END_BIT);
    /* restart ping */
    TEST_ESP_OK(esp_ping_start(ping));
    vTaskDelay(pdMS_TO_TICKS(ETH_PING_DURATION_MS));
    TEST_ESP_OK(esp_ping_stop(ping));
    bits = xEventGroupWaitBits(eth_event_group, ETH_PING_END_BIT, true, true, pdMS_TO_TICKS(ETH_PING_END_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_PING_END_BIT) == ETH_PING_END_BIT);
    /* de-initialize ping process */
    TEST_ESP_OK(esp_ping_delete_session(ping));

    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    /* wait for connection stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
    // "check link timer callback" might owned the reference of phy object, make sure it has release it
    vTaskDelay(pdMS_TO_TICKS(2000));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_eth_clear_default_handlers(eth_netif));
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
    esp_netif_destroy(eth_netif);
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
    esp_netif_init();
    TEST_ESP_OK(esp_event_loop_create_default());
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t* eth_netif = esp_netif_new(&cfg);
    TEST_ESP_OK(esp_eth_set_default_handlers(eth_netif));
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
    TEST_ESP_OK(esp_netif_attach(eth_netif, eth_handle));
    vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_loop_delete_default());
    TEST_ESP_OK(spi_bus_remove_device(spi_handle));
    TEST_ESP_OK(spi_bus_free(HSPI_HOST));
}
#endif
