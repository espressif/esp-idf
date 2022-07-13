#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "ping/ping_sock.h"
#include "esp32/rom/md5_hash.h"
#include "soc/soc_caps.h"

#if SOC_EMAC_SUPPORTED

static const char *TAG = "esp32_eth_test";

#define ETH_START_BIT BIT(0)
#define ETH_STOP_BIT BIT(1)
#define ETH_CONNECT_BIT BIT(2)
#define ETH_GOT_IP_BIT BIT(3)
#define ETH_PING_END_BIT BIT(4)
#define ETH_DOWNLOAD_END_BIT BIT(5)

#define ETH_START_TIMEOUT_MS (10000)
#define ETH_CONNECT_TIMEOUT_MS (40000)
#define ETH_STOP_TIMEOUT_MS (10000)
#define ETH_GET_IP_TIMEOUT_MS (60000)
#define ETH_DOWNLOAD_END_TIMEOUT_MS (240000)
#define ETH_PING_DURATION_MS (5000)
#define ETH_PING_END_TIMEOUT_MS (ETH_PING_DURATION_MS * 2)

/** To make the test stable in our CI environment we ping only internal localhost endpoint,
 * so the packet doesn't really leave the chip boundary, let alone via ethernet interface.
 * The ICMP test doesn't logically belong to esp-eth component, but is historically kept here.
 * Note the test was moved to lwip component in the further release branches
 */
#define TEST_ICMP_DESTINATION_DOMAIN_NAME "127.0.0.1"

// compute md5 of download file
static struct MD5Context md5_context;
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
    mac_config.flags = ETH_MAC_FLAG_PIN_TO_CORE; // pin to core
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
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
    // set default handlers to do layer 3 (and up) stuffs
    TEST_ESP_OK(esp_eth_set_default_handlers(eth_netif));
    // register user defined event handers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    // combine driver with netif
    void *glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));
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
    TEST_ESP_OK(esp_eth_clear_default_handlers(eth_netif));
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
    // set default handlers to do layer 3 (and up) stuffs
    TEST_ESP_OK(esp_eth_set_default_handlers(eth_netif));
    // register user defined event handers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    // combine driver with netif
    void *glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));

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
    TEST_ESP_OK(esp_eth_clear_default_handlers(eth_netif));
    esp_netif_destroy(eth_netif);
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
}

TEST_CASE("esp32 ethernet icmp test", "[ethernet][test_env=UT_T2_Ethernet]")
{
    EventBits_t bits = 0;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    // create TCP/IP netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);
    // set default handlers to do layer 3 (and up) stuffs
    TEST_ESP_OK(esp_eth_set_default_handlers(eth_netif));
    // register user defined event handers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    // combine driver with netif
    void *glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));
    // start Ethernet driver
    TEST_ESP_OK(esp_eth_start(eth_handle));
    /* wait for IP lease */
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);

    // Parse IP address
    ip_addr_t target_addr;
    struct addrinfo hint;
    struct addrinfo *res = NULL;
    memset(&hint, 0, sizeof(hint));
    memset(&target_addr, 0, sizeof(target_addr));
    /* convert ICMP destination string to lwip's representation of an IP address */
    TEST_ASSERT(getaddrinfo(TEST_ICMP_DESTINATION_DOMAIN_NAME, NULL, &hint, &res) == 0);
    struct in_addr addr4 = ((struct sockaddr_in *)(res->ai_addr))->sin_addr;
    inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
    freeaddrinfo(res);

    esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
    ping_config.timeout_ms = 2000;
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
    TEST_ESP_OK(esp_eth_clear_default_handlers(eth_netif));
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
        MD5Update(&md5_context, evt->data, evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

static void eth_download_task(void *param)
{
    EventGroupHandle_t eth_event_group = (EventGroupHandle_t)param;
    MD5Init(&md5_context);
    esp_http_client_config_t config = {
        .url = "https://dl.espressif.com/dl/misc/2MB.bin",
        .event_handler = http_event_handle,
        .buffer_size = 5120
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    TEST_ASSERT_NOT_NULL(client);
    TEST_ESP_OK(esp_http_client_perform(client));
    TEST_ESP_OK(esp_http_client_cleanup(client));
    MD5Final(digest, &md5_context);
    xEventGroupSetBits(eth_event_group, ETH_DOWNLOAD_END_BIT);
    vTaskDelete(NULL);
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
    // set default handlers to do layer 3 (and up) stuffs
    TEST_ESP_OK(esp_eth_set_default_handlers(eth_netif));
    // register user defined event handers
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    // install Ethernet driver
    TEST_ESP_OK(esp_eth_driver_install(&eth_config, &eth_handle));
    // combine driver with netif
    void *glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));
    // start Ethernet driver
    TEST_ESP_OK(esp_eth_start(eth_handle));
    /* wait for IP lease */
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);

    xTaskCreate(eth_download_task, "eth_dl", 4096, eth_event_group, tskIDLE_PRIORITY + 2, NULL);
    /* wait for download end */
    bits = xEventGroupWaitBits(eth_event_group, ETH_DOWNLOAD_END_BIT, true, true, pdMS_TO_TICKS(ETH_DOWNLOAD_END_TIMEOUT_MS));
    TEST_ASSERT_EQUAL(ETH_DOWNLOAD_END_BIT, bits & ETH_DOWNLOAD_END_BIT);
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
    TEST_ESP_OK(esp_eth_clear_default_handlers(eth_netif));
    esp_netif_destroy(eth_netif);
    TEST_ESP_OK(esp_event_loop_delete_default());
    vEventGroupDelete(eth_event_group);
}

#endif // SOC_EMAC_SUPPORTED