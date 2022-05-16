#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "test_utils.h"
#include "unity.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "ping/ping_sock.h"

#define ETH_PING_END_BIT BIT(1)
#define ETH_PING_DURATION_MS (5000)
#define ETH_PING_END_TIMEOUT_MS (ETH_PING_DURATION_MS * 2)
#define TEST_ICMP_DESTINATION_DOMAIN_NAME "127.0.0.1"

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

TEST_CASE("localhost ping test", "[lwip]")
{
    EventBits_t bits;
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT(eth_event_group != NULL);
    test_case_uses_tcpip();

    // Parse IP address: Destination is a localhost address, so we don't need any interface (esp-netif/driver)
    ip_addr_t target_addr;
    struct addrinfo hint;
    struct addrinfo *res = NULL;
    memset(&hint, 0, sizeof(hint));
    memset(&target_addr, 0, sizeof(target_addr));
    /* convert URL to IP */
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

    vEventGroupDelete(eth_event_group);
}
