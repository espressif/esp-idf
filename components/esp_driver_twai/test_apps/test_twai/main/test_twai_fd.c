/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "test_utils.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"

#define TEST_TX_GPIO                4
#define TEST_RX_GPIO                5
#define TEST_TWAI_QUEUE_DEPTH       5

static void test_random_trans_generator(twai_node_handle_t node_hdl, uint32_t trans_num)
{
    uint8_t send_pkg_ptr[TWAIFD_FRAME_MAX_LEN];
    twai_frame_t tx_msg = {
        .buffer = send_pkg_ptr,
    };
    printf("Sending %ld random trans ...\n", trans_num);
    for (uint32_t tx_cnt = 0; tx_cnt < trans_num; tx_cnt++) {
        tx_msg.header.id = tx_cnt | 0xf000;
        tx_msg.header.ide = !!(tx_cnt % 2);
        tx_msg.header.rtr = !!(tx_cnt % 3);
        tx_msg.header.fdf = !!(tx_cnt % 5);
        tx_msg.buffer_len = tx_msg.header.fdf ? (tx_cnt % TWAIFD_FRAME_MAX_LEN) : (tx_cnt % TWAI_FRAME_MAX_LEN);
        TEST_ESP_OK(twai_node_transmit(node_hdl, &tx_msg, 0));
        vTaskDelay(8);  //as async transaction, waiting trans done
    }
}

static IRAM_ATTR bool test_range_filter_rx_done_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    uint8_t *test_ctrl = user_ctx;
    uint8_t recv_pkg_ptr[TWAIFD_FRAME_MAX_LEN];
    twai_frame_t rx_frame = {
        .buffer = recv_pkg_ptr,
        .buffer_len = TWAIFD_FRAME_MAX_LEN,
    };
    if (ESP_OK == twai_node_receive_from_isr(handle, &rx_frame)) {
        ESP_EARLY_LOGI("Recv", "RX id 0x%4x len %2d ext %d rmt %d fd %d", rx_frame.header.id, twaifd_dlc2len(rx_frame.header.dlc), rx_frame.header.ide, rx_frame.header.rtr, rx_frame.header.fdf);
        switch (test_ctrl[0]) {
        case 0: // enable range filter
            TEST_ASSERT(!rx_frame.header.ide);
            TEST_ASSERT((rx_frame.header.id >= 0x0a) && (rx_frame.header.id <= 0x15));
            break;
        case 1: break; // disable range filter
        default: TEST_ASSERT(false);
        }
        test_ctrl[1] ++;
    }
    return false;
}

TEST_CASE("twai range filter (loopback)", "[twai]")
{
    uint8_t test_ctrl[2];
    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = TEST_TX_GPIO,
        .io_cfg.rx = TEST_TX_GPIO,  // Using same pin for test without transceiver
        .bit_timing.bitrate = 1000000,
        .tx_queue_depth = TEST_TWAI_QUEUE_DEPTH,
        .flags.enable_loopback = true,
        .flags.enable_self_test = true,
    };
    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));

    twai_event_callbacks_t user_cbs = {
        .on_rx_done = test_range_filter_rx_done_cb,
    };
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, test_ctrl));

    // disable mask filter 0 which enabled by default
    twai_mask_filter_config_t mfilter_cfg = {
        .id = 0xFFFFFFFF,
        .mask = 0xFFFFFFFF,
    };
    TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, 0, &mfilter_cfg));

    test_ctrl[0] = 0;
    test_ctrl[1] = 0;
    twai_range_filter_config_t rfilter_cfg = {
        .range_low = 0x0a,
        .range_high = 0x15,
        .is_ext = false,
    };
    printf("Config range filter 0: 0x%lx - 0x%lx\n", rfilter_cfg.range_low, rfilter_cfg.range_high);
    TEST_ESP_OK(twai_node_config_range_filter(node_hdl, 0, &rfilter_cfg));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 30);
    TEST_ASSERT_EQUAL(6, test_ctrl[1]);     // must receive 6 of 30 frames under filter config

    test_ctrl[0] = 1;
    test_ctrl[1] = 0;
    rfilter_cfg.range_low = 1;
    rfilter_cfg.range_high = 0;    // config invalid range to disable range filter
    printf("Disable range filter 0: 0x%lx - 0x%lx\n", rfilter_cfg.range_low, rfilter_cfg.range_high);
    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_config_range_filter(node_hdl, 0, &rfilter_cfg));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 30);
    TEST_ASSERT_EQUAL(0, test_ctrl[1]);

    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}

#define TEST_TRANS_TIME_BUF_LEN              20000
static IRAM_ATTR bool test_fd_trans_time_rx_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    twai_frame_t *rx_frame = user_ctx;
    uint32_t data_len;
    if (ESP_OK == twai_node_receive_from_isr(handle, rx_frame)) {
        data_len = MIN(twaifd_dlc2len(rx_frame->header.dlc), rx_frame->buffer_len);
        ESP_EARLY_LOGD("Recv", "RX id 0x%x len %d brs %d", rx_frame->header.id, data_len, rx_frame->header.brs);
        rx_frame->buffer += data_len;
        // calc the `rx_frame->buffer_len` for last frame receive
        if ((rx_frame->header.id - 0x400 + 1) * rx_frame->buffer_len > (TEST_TRANS_TIME_BUF_LEN - rx_frame->buffer_len)) {
            rx_frame->buffer_len = TEST_TRANS_TIME_BUF_LEN % rx_frame->buffer_len;
        }
    }
    return false;
}

TEST_CASE("twai fd transmit time (loopback)", "[twai]")
{
    // prepare test memory
    uint8_t *send_pkg_ptr = heap_caps_malloc(TEST_TRANS_TIME_BUF_LEN, MALLOC_CAP_8BIT);
    uint8_t *recv_pkg_ptr = heap_caps_malloc(TEST_TRANS_TIME_BUF_LEN, MALLOC_CAP_8BIT);
    TEST_ASSERT(send_pkg_ptr && recv_pkg_ptr);

    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = TEST_TX_GPIO,
        .io_cfg.rx = TEST_TX_GPIO,  // Using same pin for test without transceiver
        .bit_timing.bitrate = 1000000,
        .data_timing.bitrate = 4000000,
        .data_timing.ssp_permill = 700, // ssp 70.0%
        .tx_queue_depth = TEST_TWAI_QUEUE_DEPTH,
        .flags.enable_loopback = true,
        .flags.enable_self_test = true,
    };
    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));

    twai_frame_t rx_frame = {.buffer_len = 64};
    twai_event_callbacks_t user_cbs = {
        .on_rx_done = test_fd_trans_time_rx_cb,
    };
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, &rx_frame));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    printf("%-12s %-14s %-14s %-7s %-15s %s\n", "pkg_len", "frame_len", "frame_num", "brs", "trans_time/ms", "result");
    printf("-------------------------------------------------------------------------\n");

    uint64_t time1, time2;
    for (uint8_t test_mode = 0; test_mode < 3; test_mode ++) {
        //create and enqueue all transfers
        uint8_t frame_len = test_mode >= 1 ? 64 : 8;
        uint16_t trans_num = howmany(TEST_TRANS_TIME_BUF_LEN, frame_len);
        rx_frame.buffer = recv_pkg_ptr;
        rx_frame.buffer_len = frame_len;
        memset(recv_pkg_ptr, 0xff, TEST_TRANS_TIME_BUF_LEN);
        twai_frame_t *tx_msgs = heap_caps_calloc(trans_num, sizeof(twai_frame_t), MALLOC_CAP_8BIT);
        TEST_ASSERT(tx_msgs);
        time1 = esp_timer_get_time();
        for (uint32_t tx_cnt = 0; tx_cnt < trans_num; tx_cnt++) {
            tx_msgs[tx_cnt].header.id = tx_cnt | 0x400;
            tx_msgs[tx_cnt].header.fdf = frame_len == 64;
            tx_msgs[tx_cnt].header.brs = test_mode == 2;
            tx_msgs[tx_cnt].buffer = send_pkg_ptr + tx_cnt * frame_len;
            tx_msgs[tx_cnt].buffer_len = ((tx_cnt + 1) == trans_num) ? (TEST_TRANS_TIME_BUF_LEN - tx_cnt * frame_len) : frame_len;
            TEST_ESP_OK(twai_node_transmit(node_hdl, &tx_msgs[tx_cnt], 1000));
        }

        //waiting pkg receive finish
        while (rx_frame.buffer < recv_pkg_ptr + TEST_TRANS_TIME_BUF_LEN) {
            vTaskDelay(1);
        }
        time2 = esp_timer_get_time();
        free(tx_msgs);

        // check if pkg receive correct
        printf("%-12d %-14d %-14d %-7d %-15.2f %-s\n",
               TEST_TRANS_TIME_BUF_LEN,
               frame_len,
               trans_num,
               (test_mode == 2),
               (time2 - time1) / 1000.f,
               memcmp(recv_pkg_ptr, send_pkg_ptr, TEST_TRANS_TIME_BUF_LEN) ? "failed" : "ok");
        TEST_ASSERT_EQUAL_HEX8_ARRAY(send_pkg_ptr, recv_pkg_ptr, TEST_TRANS_TIME_BUF_LEN);
    }
    printf("-------------------------------------------------------------------------\n");

    free(send_pkg_ptr);
    free(recv_pkg_ptr);
    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}
