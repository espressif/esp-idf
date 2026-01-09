/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "soc/emac_mac_struct.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_sleep.h"
#include "driver/uart.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "esp_eth_test_common.h"
#include "hal/emac_hal.h"

#include "lwip/inet.h"
#include "ping/ping_sock.h"

#define ETH_TEST_DEBUG_EN (0) // set to 1 to enable test debug

#define ETH_SLEEP_DURATION_US (1000000) // 1 second

// send more pings than Number of DMA transmit buffers to ensure that descriptors are correctly returned back to its owners
// and overflow is handled correctly
#define ETH_PING_CNT (CONFIG_ETH_DMA_TX_BUFFER_NUM + 5)
#define ETH_PING_INTERVAL_MS (100)
#define ETH_PING_TIMEOUT_MS (1000)
#define ETH_PING_SEM_TIMEOUT_MS (ETH_PING_TIMEOUT_MS * ETH_PING_CNT * 2)

#define ETH_TEST_ETHERTYPE (0x0666)
#define ETH_RECV_TIMEOUT_MS (50)

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_EMAC_SUPPORT_SLEEP_RETENTION

static const char *TAG = "emac_sleep_test";

static void eth_test_start_sleep(esp_eth_handle_t eth_handle, bool pd_top_down)
{
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);

    // Configure sleep
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    if (pd_top_down) {
        printf("Enable CPU power down\n");
        TEST_ESP_OK(sleep_cpu_configure(true));
    }
#endif

#if ETH_TEST_DEBUG_EN
    esp_eth_ioctl(eth_handle, ETH_MAC_ESP_CMD_DUMP_REGS, NULL);
#endif

    uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(ETH_SLEEP_DURATION_US));
    ESP_LOGI(TAG, "Entering light sleep for %d us...", ETH_SLEEP_DURATION_US);
    printf("\n           ( -.-)Zzz\n\n");
    TEST_ESP_OK(esp_light_sleep_start());
    printf("\n           ( o_o)!\n\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    if (pd_top_down) {
        TEST_ESP_OK(sleep_cpu_configure(false));
    }
#endif
    ESP_LOGI(TAG, "Woke up from light sleep");

    // Verify sleep happened as expected
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);

#if ETH_TEST_DEBUG_EN
    esp_eth_ioctl(eth_handle, ETH_MAC_ESP_CMD_DUMP_REGS, NULL);
#endif

    printf("sleep_ctx.sleep_flags: 0x%" PRIx32 "\n", sleep_ctx.sleep_flags);
    // Check if the power domain was powered down
    TEST_ASSERT_EQUAL(pd_top_down ? PMU_SLEEP_PD_TOP : 0, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);

    esp_sleep_set_sleep_context(NULL);
}

static esp_err_t eth_recv_esp_emac_check_cb(esp_eth_handle_t hdl, uint8_t *buffer, uint32_t length, void *priv, void *info)
{
    emac_frame_t *frame = (emac_frame_t *)buffer;

    if (frame->proto == ntohs(ETH_TEST_ETHERTYPE)) {
        if (frame->dest[0] == 0x01 && frame->dest[1] == 0x00 && frame->dest[2] == 0x5E) {
            SemaphoreHandle_t sem = (SemaphoreHandle_t)priv;
            xSemaphoreGive(sem);
        }
    }
    return ESP_OK;
}

static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args)
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
    printf("%" PRIu32 " bytes from %s icmp_seq=%" PRIu16 " ttl=%" PRIu16 " time=%" PRIu32 " ms\n",
           recv_len, ipaddr_ntoa((ip_addr_t *)&target_addr), seqno, ttl, elapsed_time);
}

static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    printf("From %s icmp_seq=%" PRIu16 " timeout\n", ipaddr_ntoa((ip_addr_t *)&target_addr), seqno);
}

static void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args)
{
    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
    uint32_t loss;
    SemaphoreHandle_t sem = (SemaphoreHandle_t)args;

    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));

    if (transmitted > 0) {
        loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
    } else {
        loss = 0;
    }
    if (IP_IS_V4(&target_addr)) {
        printf("\n--- %s ping statistics ---\n", inet_ntoa(*ip_2_ip4(&target_addr)));
    } else {
        printf("\n--- %s ping statistics ---\n", inet6_ntoa(*ip_2_ip6(&target_addr)));
    }
    printf("%" PRIu32 " packets transmitted, %" PRIu32 " received, %" PRIu32 "%% packet loss, time %" PRIu32 "ms\n",
           transmitted, received, loss, total_time_ms);

    esp_ping_delete_session(hdl);
    TEST_ASSERT_EQUAL(0, loss);
    TEST_ASSERT_EQUAL(ETH_PING_CNT, transmitted);
    TEST_ASSERT_EQUAL(ETH_PING_CNT, received);
    xSemaphoreGive(sem);
}

static void ping_start(const esp_ip4_addr_t *ip, SemaphoreHandle_t sem)
{
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    config.count = ETH_PING_CNT;
    config.timeout_ms = ETH_PING_TIMEOUT_MS;
    config.interval_ms = ETH_PING_INTERVAL_MS;

    ip4_addr_set_u32(ip_2_ip4(&config.target_addr), ip->addr);
    config.target_addr.type = IPADDR_TYPE_V4;

    /* set callback functions */
    esp_ping_callbacks_t cbs = {
        .cb_args = sem,
        .on_ping_success = cmd_ping_on_ping_success,
        .on_ping_timeout = cmd_ping_on_ping_timeout,
        .on_ping_end = cmd_ping_on_ping_end
    };
    esp_ping_handle_t ping;
    esp_ping_new_session(&config, &cbs, &ping);
    esp_ping_start(ping);
}

static void test_emac_sleep_retention(bool pd_top_down)
{
    test_case_uses_tcpip();
    TEST_ESP_OK(esp_event_loop_create_default());
    // create TCP/IP netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);

    // Initialize Ethernet
    esp_eth_mac_t *mac = mac_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(mac);
    esp_eth_phy_t *phy = phy_init(NULL);
    TEST_ASSERT_NOT_NULL(phy);

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    TEST_ESP_OK(esp_eth_driver_install(&config, &eth_handle));
    extra_eth_config(eth_handle);
    // combine driver with netif
    esp_eth_netif_glue_handle_t glue = esp_eth_new_netif_glue(eth_handle);
    TEST_ESP_OK(esp_netif_attach(eth_netif, glue));

    // Register event handlers
    EventGroupHandle_t eth_event_group = xEventGroupCreate();
    TEST_ASSERT_NOT_NULL(eth_event_group);
    TEST_ESP_OK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, eth_event_group));
    TEST_ESP_OK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, eth_event_group));

    uint8_t mac_addr[ETH_ADDR_LEN];
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr));

    // -------- Verify retention when Ethernet is stopped --------
    eth_test_start_sleep(eth_handle, pd_top_down);
    uint8_t mac_addr_post_sleep[ETH_ADDR_LEN];
    // Verify that EMAC configuration was retained by checking MAC address
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr_post_sleep));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(mac_addr, mac_addr_post_sleep, ETH_ADDR_LEN);
    // verify that EMAC is not started
    emac_dma_soc_regs_t dma_regs = &EMAC_DMA;
    emac_mac_soc_regs_t mac_regs = &EMAC_MAC;
    TEST_ASSERT_EQUAL(0, dma_regs->dmaoperation_mode.start_stop_rx);
    TEST_ASSERT_EQUAL(0, dma_regs->dmaoperation_mode.start_stop_transmission_command);
    TEST_ASSERT_EQUAL(0, mac_regs->gmacconfig.rx);
    TEST_ASSERT_EQUAL(0, mac_regs->gmacconfig.tx);

    // Start Ethernet
    TEST_ESP_OK(esp_eth_start(eth_handle));
    EventBits_t bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(ETH_START_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);

    // Wait for Ethernet connection
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);

    // wait for IP lease
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);

    // ----- Verify EMAC and full IP stack is still working after sleep -----
    eth_test_start_sleep(eth_handle, pd_top_down);

    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(sem);
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(eth_netif, &ip_info);

    // Verify functionality of MAC and the full IP stack
    ping_start(&ip_info.gw, sem);

    // wait for ping to finish
    TEST_ASSERT_EQUAL(pdPASS, xSemaphoreTake(sem, pdMS_TO_TICKS(ETH_PING_SEM_TIMEOUT_MS)));

    // ----- Verify MAC filter retention -----
    // Stop Ethernet to be able to set loopback
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);

    // Set loopback to simplify test
    bool loopback_en = true;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_PHY_LOOPBACK, &loopback_en));
    TEST_ESP_OK(esp_eth_update_input_path_info(eth_handle, eth_recv_esp_emac_check_cb, sem));

    // Start Ethernet
    TEST_ESP_OK(esp_eth_start(eth_handle));
    bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(ETH_START_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);

    // Wait for Ethernet connection
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);

    ESP_LOGW(TAG, "Add multicast addresses to filter until it's full (add cmd fails)");
    uint8_t multicast_addr[ETH_ADDR_LEN] = {0x01, 0x00, 0x5E, 0x00, 0x00, 0x01};
    esp_err_t ret;
    uint32_t mcast_addr_count = 0;
    do {
        ret = esp_eth_ioctl(eth_handle, ETH_CMD_ADD_MAC_FILTER, multicast_addr);
        if (ret == ESP_OK) {
            mcast_addr_count++;
            multicast_addr[ETH_ADDR_LEN-1]++; // increment last field
        }
    } while (ret == ESP_OK);
    multicast_addr[ETH_ADDR_LEN-1] = 0x01;
    ESP_LOGI(TAG, "Added %" PRIu32 " multicast addresses", mcast_addr_count);

    // Let's sleep
    eth_test_start_sleep(eth_handle, pd_top_down);

    // Create multicast frame and send it
    emac_frame_t *eth_frame = malloc(60);
    TEST_ASSERT_NOT_NULL(eth_frame);
    memcpy(eth_frame->dest, multicast_addr, ETH_ADDR_LEN);
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, eth_frame->src);
    eth_frame->proto = htons(ETH_TEST_ETHERTYPE);

    uint32_t mcast_recv_cnt = 0;
    for (uint32_t i = 0; i < mcast_addr_count; i++) {
        esp_eth_transmit(eth_handle, eth_frame, 60);
        eth_frame->dest[ETH_ADDR_LEN-1]++;
        if (xSemaphoreTake(sem, pdMS_TO_TICKS(ETH_RECV_TIMEOUT_MS)) == pdFAIL) {
            break;
        }
        mcast_recv_cnt++;
    }
    free(eth_frame);
    TEST_ASSERT_EQUAL(mcast_addr_count, mcast_recv_cnt);

    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* wait for stop stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);

    // -------- Verify retention when Ethernet is stopped (when link is up) --------
    eth_test_start_sleep(eth_handle, pd_top_down);
    memset(mac_addr_post_sleep, 0, ETH_ADDR_LEN);
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr_post_sleep));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(mac_addr, mac_addr_post_sleep, ETH_ADDR_LEN);
    // verify that EMAC is not started
    TEST_ASSERT_EQUAL(0, dma_regs->dmaoperation_mode.start_stop_rx);
    TEST_ASSERT_EQUAL(0, dma_regs->dmaoperation_mode.start_stop_transmission_command);
    TEST_ASSERT_EQUAL(0, mac_regs->gmacconfig.rx);
    TEST_ASSERT_EQUAL(0, mac_regs->gmacconfig.tx);

    TEST_ESP_OK(esp_eth_del_netif_glue(glue));
    /* driver should be uninstalled within 2 seconds */
    TEST_ESP_OK(esp_eth_driver_uninstall(eth_handle));
    TEST_ESP_OK(phy->del(phy));
    TEST_ESP_OK(mac->del(mac));
    TEST_ESP_OK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, got_ip_event_handler));
    TEST_ESP_OK(esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, eth_event_handler));
    esp_netif_destroy(eth_netif);
    TEST_ESP_OK(esp_event_loop_delete_default());
    extra_cleanup();
    vEventGroupDelete(eth_event_group);
    vSemaphoreDelete(sem);
}

TEST_CASE("internal emac sleep retention", "[sleep_retention]")
{
    ESP_LOGI(TAG, "Testing with PD_TOP powered up");
    test_emac_sleep_retention(false);

#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    ESP_LOGI(TAG, "Testing with PD_TOP powered down");
    test_emac_sleep_retention(true);
#endif
}

#endif // CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_EMAC_SUPPORT_SLEEP_RETENTION
