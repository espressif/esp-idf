/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_sleep.h"
#include "driver/uart.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "esp_bit_defs.h"
#include "esp_eth_test_emac_utils.h"
#include "eth_phy_802_3_regs.h"
#include "hal/emac_hal.h"

#include "lwip/inet.h"
#include "ping/ping_sock.h"

#define ETH_TEST_DEBUG_EN (0) // set to 1 to enable test debug

#define ETH_SLEEP_DURATION_US (1'000'000) // 1 second
// Host sends the magic packet during this window. The timer only stops a hang if the packet never arrives.
#define ETH_WOL_MAGIC_SLEEP_US (2'000'000)
// No magic packet is sent. The timer is the expected wakeup.
#define ETH_WOL_TIMER_SLEEP_US (1'000'000)

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

/**================================================================================================
 * Helper functions
 *================================================================================================*/
static void eth_test_start_sleep(esp_eth_handle_t eth_handle, bool pd_top_down, bool pd_cnnt_down,
                                 uint64_t sleep_duration_us)
{
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);

    // Configure sleep
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    if (pd_top_down) {
        ESP_LOGI(TAG, "Enable CPU power down");
        TEST_ESP_OK(sleep_cpu_configure(true));
#if SOC_PM_SUPPORT_CNNT_PD
        if (!pd_cnnt_down) {
            ESP_LOGI(TAG, "Keep CNNT power domain up");
            TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_CNNT, ESP_PD_OPTION_ON));
        }
#endif
    }
#endif

#if ETH_TEST_DEBUG_EN
    esp_eth_ioctl(eth_handle, ETH_MAC_ESP_CMD_DUMP_REGS, NULL);
#endif
    ESP_LOGI(TAG, "Entering light sleep for %" PRIu64 " us...", sleep_duration_us);
    printf("\n           ( -.-)Zzz\n\n");
    uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(sleep_duration_us));
    TEST_ESP_OK(esp_light_sleep_start());
    printf("\n           ( o_o)!\n\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    if (pd_top_down) {
        TEST_ESP_OK(sleep_cpu_configure(false));
#if SOC_PM_SUPPORT_CNNT_PD
        if (!pd_cnnt_down) {
            TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_CNNT, ESP_PD_OPTION_OFF));
        }
#endif
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
    TEST_ASSERT_EQUAL_HEX32(pd_top_down ? PMU_SLEEP_PD_TOP : 0, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);

#if SOC_PM_SUPPORT_CNNT_PD
#if CONFIG_EMAC_TEST_SLEEP_MODE_NO_SLEEP || CONFIG_EMAC_TEST_SLEEP_MODE_MAC_WAKE_ON_LAN
    ESP_LOGI(TAG, "Verify CNNT power domain stayed powered");
    ESP_LOGI(TAG, "In NO_SLEEP and Wake-on-LAN modes, CNNT stays up even when the test asks for it down");
    TEST_ASSERT_EQUAL_HEX32(0, sleep_ctx.sleep_flags & PMU_SLEEP_PD_CNNT);
#else
    if (pd_cnnt_down) {
        ESP_LOGI(TAG, "Verify CNNT power domain was powered down when EMAC is in sleep mode and CNNT is powered down");
        TEST_ASSERT_EQUAL_HEX32(PMU_SLEEP_PD_CNNT, sleep_ctx.sleep_flags & PMU_SLEEP_PD_CNNT);
    } else {
        ESP_LOGI(TAG, "Verify CNNT power domain was not powered down when EMAC is in sleep mode");
        TEST_ASSERT_EQUAL_HEX32(0, sleep_ctx.sleep_flags & PMU_SLEEP_PD_CNNT);
    }
#endif
#endif

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

static void verify_gateway_ping(SemaphoreHandle_t sem)
{
    esp_netif_ip_info_t ip_info = {0};
    esp_netif_t *eth_netif = eth_test_get_netif();
    TEST_ASSERT_NOT_NULL(eth_netif);
    esp_netif_get_ip_info(eth_netif, &ip_info);
    ping_start(&ip_info.gw, sem);
    TEST_ASSERT_EQUAL(pdPASS, xSemaphoreTake(sem, pdMS_TO_TICKS(ETH_PING_SEM_TIMEOUT_MS)));
}

/**================================================================================================
 * MAC sleep retention tests under different power modes
 *================================================================================================*/
static void verify_mcast_loopback(esp_eth_handle_t eth_handle, SemaphoreHandle_t sem,
                                  const uint8_t *multicast_addr_base, uint32_t mcast_addr_count)
{
    // Create multicast frame and send it
    emac_frame_t *eth_frame = malloc(60);
    TEST_ASSERT_NOT_NULL(eth_frame);
    memcpy(eth_frame->dest, multicast_addr_base, ETH_ADDR_LEN);
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, eth_frame->src);
    eth_frame->proto = htons(ETH_TEST_ETHERTYPE);

    // Drain any stale RX notifications from previous traffic
    while (xSemaphoreTake(sem, 0) == pdPASS) {
    }

    uint32_t mcast_recv_cnt = 0;
    for (uint32_t i = 0; i < mcast_addr_count; i++) {
        TEST_ESP_OK(esp_eth_transmit(eth_handle, eth_frame, 60));
        eth_frame->dest[ETH_ADDR_LEN - 1]++;
        if (xSemaphoreTake(sem, pdMS_TO_TICKS(ETH_RECV_TIMEOUT_MS)) == pdFAIL) {
            break;
        }
        mcast_recv_cnt++;
    }
    free(eth_frame);
    TEST_ASSERT_EQUAL(mcast_addr_count, mcast_recv_cnt);
}

static void test_emac_sleep_retention(bool pd_top_down, bool pd_cnnt_down)
{
    // get handles from common module initialized by setUp()
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();

    uint8_t mac_addr[ETH_ADDR_LEN];
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr));

    // Force 100M full, autoneg off for test simplicity — no PHY-specific config required.
    // Generic 802.3 SW reset leaves YT8531 with AN off / 10H (YT8531 chip issue),
    // and 1G needs PHY RGMII TX/RX delays that the generic driver does not program.
    bool auto_nego_en = false;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    eth_speed_t speed = ETH_SPEED_100M;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));
    eth_duplex_t duplex = ETH_DUPLEX_FULL;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));

    // -------- Verify retention when Ethernet is stopped --------
    eth_test_start_sleep(eth_handle, pd_top_down, pd_cnnt_down, ETH_SLEEP_DURATION_US);
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
    eth_test_start_sleep(eth_handle, pd_top_down, pd_cnnt_down, ETH_SLEEP_DURATION_US);

    static StaticSemaphore_t sem_buffer;
    SemaphoreHandle_t sem = xSemaphoreCreateBinaryStatic(&sem_buffer);
    TEST_ASSERT_NOT_NULL(sem);

    // Verify functionality of MAC and the full IP stack
    verify_gateway_ping(sem);

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
            multicast_addr[ETH_ADDR_LEN - 1]++; // increment last field
        }
    } while (ret == ESP_OK);
    multicast_addr[ETH_ADDR_LEN - 1] = 0x01;
    ESP_LOGI(TAG, "Added %" PRIu32 " multicast addresses", mcast_addr_count);

    // Verify loopback + MAC filter work before sleep
    ESP_LOGI(TAG, "Verify multicast loopback before sleep");
    verify_mcast_loopback(eth_handle, sem, multicast_addr, mcast_addr_count);

    // Let's sleep
    eth_test_start_sleep(eth_handle, pd_top_down, pd_cnnt_down, ETH_SLEEP_DURATION_US);

    // Verify MAC filter retention via the same loopback path
    ESP_LOGI(TAG, "Verify multicast loopback after sleep");
    verify_mcast_loopback(eth_handle, sem, multicast_addr, mcast_addr_count);

    // stop Ethernet driver
    TEST_ESP_OK(esp_eth_stop(eth_handle));
    /* wait for stop stop */
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);

    // -------- Verify retention when Ethernet is stopped (when link is up) --------
    eth_test_start_sleep(eth_handle, pd_top_down, pd_cnnt_down, ETH_SLEEP_DURATION_US);
    memset(mac_addr_post_sleep, 0, ETH_ADDR_LEN);
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr_post_sleep));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(mac_addr, mac_addr_post_sleep, ETH_ADDR_LEN);
    // verify that EMAC is not started
    TEST_ASSERT_EQUAL(0, dma_regs->dmaoperation_mode.start_stop_rx);
    TEST_ASSERT_EQUAL(0, dma_regs->dmaoperation_mode.start_stop_transmission_command);
    TEST_ASSERT_EQUAL(0, mac_regs->gmacconfig.rx);
    TEST_ASSERT_EQUAL(0, mac_regs->gmacconfig.tx);
}

TEST_CASE("internal emac sleep retention PD_TOP up", "[sleep_retention][esp-netif]")
{
    ESP_LOGI(TAG, "Testing with PD_TOP powered up");
    test_emac_sleep_retention(false, false);
}

#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
TEST_CASE("internal emac sleep retention - PD_TOP/CNNT down", "[sleep_retention][esp-netif]")
{
    ESP_LOGI(TAG, "Testing with PD_TOP/CNNT powered down");
    test_emac_sleep_retention(true, true);
}

#if SOC_PM_SUPPORT_CNNT_PD
TEST_CASE("internal emac sleep retention - CNNT up, PD_TOP down", "[sleep_retention][esp-netif]")
{
    ESP_LOGI(TAG, "Testing with CNNT powered up and PD_TOP powered down");
    test_emac_sleep_retention(true, false);
}
#endif // SOC_PM_SUPPORT_CNNT_PD
#endif

/**================================================================================================
 * PHY config retention tests
 *================================================================================================*/
/**
 * Forced 10/100 Mbps / autoneg-off is a canary for a PHY hardware reset during sleep.
 * GOT_IP with autoneg on both ends can still pass after a hidden RST pulse.
 * ETH_CMD_G_AUTONEGO / ETH_CMD_G_SPEED are driver cache and would not catch it.
 * Do not gpio_hold_en() the reset pin here — that would hide the failure.
 */
static uint32_t eth_test_read_phy_reg(esp_eth_handle_t eth_handle, uint32_t reg_addr)
{
    uint32_t value = 0;
    esp_eth_phy_reg_rw_data_t rw = {
        .reg_addr = reg_addr,
        .reg_value_p = &value,
    };
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_READ_PHY_REG, &rw));
    return value;
}

static void test_sleep_phy_not_reset(bool pd_top_down, bool pd_cnnt_down)
{
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    const eth_speed_t speeds[] = { ETH_SPEED_10M, ETH_SPEED_100M };

    for (size_t i = 0; i < sizeof(speeds) / sizeof(speeds[0]); i++) {
        eth_speed_t speed = speeds[i];
        const int expect_speed_select = (speed == ETH_SPEED_100M) ? 1 : 0;
        const char *speed_str = (speed == ETH_SPEED_100M) ? "100 Mbps" : "10 Mbps";

        bool auto_nego_en = false;
        TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
        TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));

        bmcr_reg_t bmcr_pre = { .val = eth_test_read_phy_reg(eth_handle, ETH_PHY_BMCR_REG_ADDR) };
        ESP_LOGI(TAG, "BMCR before sleep (%s): 0x%04" PRIx32, speed_str, bmcr_pre.val);
        TEST_ASSERT_NOT_EQUAL_HEX32(0xFFFF, bmcr_pre.val & 0xFFFF);
        TEST_ASSERT_EQUAL(0, bmcr_pre.en_auto_nego);
        TEST_ASSERT_EQUAL(expect_speed_select, bmcr_pre.speed_select);
        TEST_ASSERT_EQUAL(0, bmcr_pre.speed_1000);
        TEST_ASSERT_EQUAL(0, bmcr_pre.reset);

        eth_test_start_sleep(eth_handle, pd_top_down, pd_cnnt_down, ETH_SLEEP_DURATION_US);

        bmcr_reg_t bmcr_post = { .val = eth_test_read_phy_reg(eth_handle, ETH_PHY_BMCR_REG_ADDR) };
        ESP_LOGI(TAG, "BMCR after sleep (%s):  0x%04" PRIx32, speed_str, bmcr_post.val);
        TEST_ASSERT_NOT_EQUAL_HEX32(0xFFFF, bmcr_post.val & 0xFFFF);
        TEST_ASSERT_EQUAL_MESSAGE(0, bmcr_post.en_auto_nego,
                                  "PHY autoneg re-enabled after sleep (PHY was likely reset)");
        TEST_ASSERT_EQUAL_MESSAGE(expect_speed_select, bmcr_post.speed_select,
                                  "PHY speed bits changed after sleep (PHY was likely reset)");
        TEST_ASSERT_EQUAL_MESSAGE(0, bmcr_post.speed_1000,
                                  "PHY 1000 Mbps select set after sleep (PHY was likely reset)");
    }
}

TEST_CASE("internal emac PHY config retention PD_TOP up", "[sleep_retention]")
{
    ESP_LOGI(TAG, "PHY BMCR canary with PD_TOP powered up");
    test_sleep_phy_not_reset(false, false);
}

#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
TEST_CASE("internal emac PHY config retention - PD_TOP/CNNT down", "[sleep_retention]")
{
    ESP_LOGI(TAG, "PHY BMCR canary with PD_TOP/CNNT powered down");
    test_sleep_phy_not_reset(true, true);
}

#if SOC_PM_SUPPORT_CNNT_PD
TEST_CASE("internal emac PHY config retention - CNNT up, PD_TOP down", "[sleep_retention]")
{
    ESP_LOGI(TAG, "PHY BMCR canary with CNNT up and PD_TOP down");
    test_sleep_phy_not_reset(true, false);
}
#endif // SOC_PM_SUPPORT_CNNT_PD
#endif

/**================================================================================================
 * Wake on LAN tests
 *================================================================================================*/
#if SOC_PM_SUPPORT_EMAC_WAKEUP && CONFIG_EMAC_TEST_SLEEP_MODE_MAC_WAKE_ON_LAN
static void verify_wol_wakeup(bool expect_emac)
{
    uint32_t causes = esp_sleep_get_wakeup_causes();
    ESP_LOGI(TAG, "Wakeup causes 0x%" PRIx32, causes);
    if (expect_emac) {
        // REGDMA restore performs a masked write (read-modify-write) to PMT CSR, which clears
        // MGKPRCVD and deasserts pmt_intr_o before the CPU reads PMU_SLP_WAKEUP_STATUS0_REG.
        // Because the PMU EMAC wakeup bit is not sticky, esp_sleep_get_wakeup_causes() reports UNDEFINED.
        // IDF-16490
        TEST_ASSERT_MESSAGE(causes & BIT(ESP_SLEEP_WAKEUP_UNDEFINED), "expected EMAC wakeup from magic packet");
    } else {
        TEST_ASSERT_EQUAL_MESSAGE(0, causes & BIT(ESP_SLEEP_WAKEUP_EMAC),
                                  "EMAC woke the CPU without a magic packet");
        TEST_ASSERT_MESSAGE(causes & BIT(ESP_SLEEP_WAKEUP_TIMER), "expected timer wakeup");
    }
}

TEST_CASE("internal emac sleep retention - wake on lan", "[sleep_wol][esp-netif]")
{
#if !SOC_PM_SUPPORT_EMAC_WAKEUP
    TEST_IGNORE_MESSAGE("EMAC wakeup not supported");
#else
    esp_eth_handle_t eth_handle = eth_test_get_eth_handle();
    EventGroupHandle_t eth_event_group = eth_test_get_default_event_group();

    bool auto_nego_en = false;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_AUTONEGO, &auto_nego_en));
    eth_speed_t speed = ETH_SPEED_100M;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_SPEED, &speed));
    eth_duplex_t duplex = ETH_DUPLEX_FULL;
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));

    TEST_ESP_OK(esp_eth_start(eth_handle));
    EventBits_t bits = xEventGroupWaitBits(eth_event_group, ETH_START_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(ETH_START_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_START_BIT) == ETH_START_BIT);
    bits = xEventGroupWaitBits(eth_event_group, ETH_CONNECT_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_CONNECT_BIT) == ETH_CONNECT_BIT);
    bits = xEventGroupWaitBits(eth_event_group, ETH_GOT_IP_BIT, true, true, pdMS_TO_TICKS(ETH_GET_IP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_GOT_IP_BIT) == ETH_GOT_IP_BIT);

    uint8_t mac_addr[ETH_ADDR_LEN];
    TEST_ESP_OK(esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr));
    // Printed after GOT_IP so the host can pair it with the esp_netif_handlers address log.
    ESP_LOGI(TAG, "WoL MAC %02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    static StaticSemaphore_t sem_buffer;
    SemaphoreHandle_t sem = xSemaphoreCreateBinaryStatic(&sem_buffer);
    TEST_ASSERT_NOT_NULL(sem);

    ESP_LOGI(TAG, "Verify ping before sleep");
    verify_gateway_ping(sem);

    // WoL keeps the connectivity domain powered. TOP follows the CPU power-down config.
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    const bool pd_top_down = true;
#else
    const bool pd_top_down = false;
#endif

    ESP_LOGI(TAG, "Sleep until a magic packet");
    eth_test_start_sleep(eth_handle, pd_top_down, false, ETH_WOL_MAGIC_SLEEP_US);
    verify_wol_wakeup(true);

    ESP_LOGI(TAG, "Verify ping after magic-packet wakeup");
    verify_gateway_ping(sem);

    ESP_LOGI(TAG, "Sleep with no magic packet");
    eth_test_start_sleep(eth_handle, pd_top_down, false, ETH_WOL_TIMER_SLEEP_US);
    verify_wol_wakeup(false);

    ESP_LOGI(TAG, "Verify ping after timer wakeup");
    verify_gateway_ping(sem);

    TEST_ESP_OK(esp_eth_stop(eth_handle));
    bits = xEventGroupWaitBits(eth_event_group, ETH_STOP_BIT, true, true, pdMS_TO_TICKS(ETH_STOP_TIMEOUT_MS));
    TEST_ASSERT((bits & ETH_STOP_BIT) == ETH_STOP_BIT);
#endif // !SOC_PM_SUPPORT_EMAC_WAKEUP
}
#endif // SOC_PM_SUPPORT_EMAC_WAKEUP && CONFIG_EMAC_TEST_SLEEP_MODE_MAC_WAKE_ON_LAN

#endif // CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_EMAC_SUPPORT_SLEEP_RETENTION
