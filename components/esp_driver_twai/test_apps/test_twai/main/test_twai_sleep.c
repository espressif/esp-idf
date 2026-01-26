/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/gpio.h"
#include "hal/twai_periph.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"

#if CONFIG_IDF_TARGET_ESP32H4
#define TEST_TX_GPIO                GPIO_NUM_2
#define TEST_RX_GPIO                GPIO_NUM_3
#else
#define TEST_TX_GPIO                GPIO_NUM_4
#define TEST_RX_GPIO                GPIO_NUM_5
#endif
#define TEST_TWAI_QUEUE_DEPTH       5
#define TEST_TRANS_LEN              100
#define TEST_FRAME_LEN              7
#define TEST_FRAME_NUM              howmany(TEST_TRANS_LEN, TEST_FRAME_LEN)

static IRAM_ATTR bool test_sleep_rx_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    twai_frame_t *rx_frame = (twai_frame_t *)user_ctx;
    if (ESP_OK == twai_node_receive_from_isr(handle, rx_frame)) {
        ESP_EARLY_LOGI("RX", "timestamp %llu frame %x [%d] %s", rx_frame->header.timestamp, rx_frame->header.id, rx_frame->header.dlc, rx_frame->buffer);
    }
    return false;
}

TEST_CASE("twai sleep retention", "[twai]")
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    esp_sleep_context_t sleep_ctx = {};
    esp_sleep_set_sleep_context(&sleep_ctx);

    twai_node_handle_t node_hdl[SOC_TWAI_CONTROLLER_NUM];
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.quanta_clk_out = GPIO_NUM_NC;
    node_config.io_cfg.bus_off_indicator = GPIO_NUM_NC;
    node_config.bit_timing.bitrate = 800000;
    node_config.timestamp_resolution_hz = 1000000;
    node_config.tx_queue_depth = TEST_FRAME_NUM;
    node_config.flags.enable_loopback = true;
    node_config.flags.enable_self_test = true;

    twai_frame_t tx_frame = {};
    tx_frame.buffer = (uint8_t *)"sleep";
    tx_frame.buffer_len = strlen((const char *)tx_frame.buffer);

    uint8_t rx_buffer[TWAI_FRAME_MAX_LEN] = {0};
    twai_frame_t rx_frame = {};
    rx_frame.buffer = rx_buffer;
    rx_frame.buffer_len = sizeof(rx_buffer);

    twai_event_callbacks_t user_cbs = {};
    user_cbs.on_rx_done = test_sleep_rx_cb;
    for (int power = 0; power < 2; power++) {
        node_config.flags.sleep_allow_pd = power;
#if !SOC_TWAI_SUPPORT_SLEEP_RETENTION
        node_config.flags.sleep_allow_pd = false;
#endif
        printf("\n===> Testing sleep power %s\n", node_config.flags.sleep_allow_pd ? "DOWN" : "HOLD");
        for (int node = 0; node < SOC_TWAI_CONTROLLER_NUM; node++) {
            node_config.io_cfg.tx = TEST_TX_GPIO;
            node_config.io_cfg.rx = TEST_TX_GPIO;  // Using same pin for test without transceiver
            TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl[node]));
            TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl[node], &user_cbs, &rx_frame));
        }

        for (int node = 0; node < SOC_TWAI_CONTROLLER_NUM; node++) {
            printf("\nTWAI%d\n", node);
            // fix signal connections after all nodes are created
            gpio_matrix_input(TEST_TX_GPIO, twai_periph_signals[node].rx_sig, false);
            gpio_matrix_output(TEST_TX_GPIO, twai_periph_signals[node].tx_sig, false, false);

            // test with 2 transactions
            for (int i = 1; i < 3; i++) {
                printf("Go to sleep ...\n");
                uint64_t time_before = esp_timer_get_time();
                TEST_ESP_OK(esp_light_sleep_start());
#if SOC_TWAI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
                printf("Waked up! sleep %d power down %ld\n", !sleep_ctx.sleep_request_result, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
                // check if the peripheral is powered down as expected
                TEST_ASSERT_EQUAL(node_config.flags.sleep_allow_pd ? PMU_SLEEP_PD_TOP : 0, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
#endif
                TEST_ESP_OK(twai_node_enable(node_hdl[node]));

                tx_frame.header.id = i + node * 0x10;
                memset(&rx_frame.header, 0, sizeof(rx_frame.header));
                memset(rx_buffer, 0, sizeof(rx_buffer));
                // Waiting 1 second after node_enable to check timestamp correctness after wakeup
                esp_rom_delay_us(1000 * 1000);
                TEST_ESP_OK(twai_node_transmit(node_hdl[node], &tx_frame, 100));
                TEST_ESP_OK(twai_node_transmit_wait_all_done(node_hdl[node], -1));
                TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_frame.buffer, rx_buffer, tx_frame.buffer_len);
                TEST_ASSERT_EQUAL_UINT32(tx_frame.header.id, rx_frame.header.id);
                // sleep 1s, wait 1s, so diff should be 2000000us
                printf("Time diff: %llu, expect 2000000\n", rx_frame.header.timestamp - time_before);
                TEST_ASSERT_INT32_WITHIN(10000, 2000000, rx_frame.header.timestamp - time_before);
                time_before = rx_frame.header.timestamp;
                TEST_ESP_OK(twai_node_disable(node_hdl[node]));
            }
        }
        for (int node = 0; node < SOC_TWAI_CONTROLLER_NUM; node++) {
            TEST_ESP_OK(twai_node_delete(node_hdl[node]));
        }
    }

    esp_sleep_set_sleep_context(NULL);
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
}

#if SOC_TWAI_SUPPORT_SLEEP_RETENTION
static void test_random_trans_generator(twai_node_handle_t node_hdl, uint32_t trans_num)
{
    uint8_t send_pkg_ptr[TWAI_FRAME_MAX_LEN];
    twai_frame_t tx_msg = {};
    tx_msg.buffer = send_pkg_ptr;

    printf("Sending %ld random trans ...\n", trans_num);
    for (uint32_t tx_cnt = 0; tx_cnt < trans_num; tx_cnt++) {
        tx_msg.header.id = tx_cnt | 0xf000 | (tx_cnt << 16);
        tx_msg.header.ide = !!(tx_cnt % 2);
        tx_msg.header.rtr = !!(tx_cnt % 3);
        tx_msg.buffer_len = tx_cnt % TWAI_FRAME_MAX_LEN;
        TEST_ESP_OK(twai_node_transmit(node_hdl, &tx_msg, 0));
        TEST_ESP_OK(twai_node_transmit_wait_all_done(node_hdl, 8));
    }
}

static IRAM_ATTR bool test_filter_rx_done_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    uint8_t *test_ctrl = (uint8_t *)user_ctx;
    uint8_t recv_pkg_ptr[TWAI_FRAME_MAX_LEN];
    twai_frame_t rx_frame = {};
    rx_frame.buffer = recv_pkg_ptr;
    rx_frame.buffer_len = TWAI_FRAME_MAX_LEN;

    if (ESP_OK == twai_node_receive_from_isr(handle, &rx_frame)) {
        ESP_EARLY_LOGI("Recv", "RX id 0x%4x len %2d ext %d rmt %d", rx_frame.header.id, twaifd_dlc2len(rx_frame.header.dlc), rx_frame.header.ide, rx_frame.header.rtr);
        switch (test_ctrl[0]) {
        case 0:         // receive something
            TEST_ASSERT(rx_frame.header.id >= 0x10);
            TEST_ASSERT(!rx_frame.header.ide);
            break;
        case 1:         // receive all
        case 2: break;  // receive none
        default: TEST_ASSERT(false);
        }
        test_ctrl[1] ++;
    }
    return false;
}

TEST_CASE("twai mask filter sleep retention", "[twai]")
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    esp_sleep_context_t sleep_ctx = {};
    esp_sleep_set_sleep_context(&sleep_ctx);

    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.tx = TEST_TX_GPIO;
    node_config.io_cfg.rx = TEST_TX_GPIO; // Using same pin for test without transceiver
    node_config.io_cfg.quanta_clk_out = GPIO_NUM_NC;
    node_config.io_cfg.bus_off_indicator = GPIO_NUM_NC;
    node_config.bit_timing.bitrate = 1000000;
    node_config.tx_queue_depth = TEST_TWAI_QUEUE_DEPTH;
    node_config.flags.enable_self_test = true;
    node_config.flags.enable_loopback = true;
    node_config.flags.sleep_allow_pd = true;
    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));

    uint8_t test_ctrl[2] = {0, 0};
    twai_event_callbacks_t user_cbs = {};
    user_cbs.on_rx_done = test_filter_rx_done_cb;
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, test_ctrl));

    twai_mask_filter_config_t mfilter_cfg = {};
    mfilter_cfg.id = 0x10,
    mfilter_cfg.mask = 0xf0,
    mfilter_cfg.is_ext = false,
    printf("Testing mask filter: id 0x%lx mask 0x%lx is_ext %d\n", mfilter_cfg.id, mfilter_cfg.mask, mfilter_cfg.is_ext);
    TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, 0, &mfilter_cfg));

    printf("Go to sleep ...\n");
    TEST_ESP_OK(esp_light_sleep_start());
    printf("Waked up! sleep %d power down %ld\n", !sleep_ctx.sleep_request_result, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
#if SOC_TWAI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    TEST_ASSERT_EQUAL(PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
#endif
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 30);
    TEST_ASSERT_EQUAL(7, test_ctrl[1]);     // must receive 7 of 30 frames under filter config

    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
    esp_sleep_set_sleep_context(NULL);
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
}
#endif  //SOC_TWAI_SUPPORT_SLEEP_RETENTION
