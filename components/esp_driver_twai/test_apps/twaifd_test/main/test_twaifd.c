/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "test_utils.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"

#define TEST_TX_GPIO                4
#define TEST_RX_GPIO                5
#define TEST_TWAI_QUEUE_DEPTH       5
#define TEST_TRANS_LEN              100
#define TEST_FRAME_LEN              7
#define TEST_FRAME_NUM              howmany(TEST_TRANS_LEN, TEST_FRAME_LEN)

static bool test_driver_install_rx_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    twai_frame_header_t rx_header;
    if (ESP_OK == twai_node_receive_from_isr(handle, &rx_header, NULL, 0, NULL)) {
        ESP_EARLY_LOGI("Recv ", "id 0x%lx rtr %d", rx_header.id, rx_header.rtr);
    }
    if (rx_header.id != 0x100) {
        TEST_FAIL();    //callback is unregistered, should not run here
    }
    return false;
}

TEST_CASE("twai install uninstall (loopback)", "[TWAI]")
{
    esp_err_t ret;
    twai_node_handle_t node_hdl[SOC_TWAI_CONTROLLER_NUM + 1];
    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = TEST_TX_GPIO,
        .io_cfg.rx = TEST_TX_GPIO,  // Using same pin for test without transceiver
        .bit_timing.bitrate = 1000000,
        .data_timing.bitrate = 1000000,
        .tx_queue_depth = TEST_TWAI_QUEUE_DEPTH,
        .flags.enable_loopback = true,
        .flags.enable_self_test = true,
    };

    // loop 10 times to check memory leak
    for (uint8_t loop = 0; loop < 10; loop ++) {
        for (uint8_t i = 0; i < SOC_TWAI_CONTROLLER_NUM + 1; i++) {
            ret = twai_new_node_onchip(&node_config, &node_hdl[i]);
            printf("Install TWAI%d return %s\n", i, esp_err_to_name(ret));
            TEST_ASSERT(ret == ((i < SOC_TWAI_CONTROLLER_NUM) ? ESP_OK : ESP_ERR_NOT_FOUND));
        }
        // can't disable before enable
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, twai_node_disable(node_hdl[0]));
        twai_event_callbacks_t user_cbs = {
            .on_rx_done = test_driver_install_rx_cb,
        };
        TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl[0], &user_cbs, NULL));

        printf("Test unregister callback\n");
        user_cbs.on_rx_done = NULL;
        TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl[0], &user_cbs, NULL));

        twai_frame_t tx_frame = {
            .header.id = 0x82,
            .header.rtr = true,
        };
        printf("Test transmit before enable\n");
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, twai_node_transmit(node_hdl[0], &tx_frame, 0));
        TEST_ESP_OK(twai_node_enable(node_hdl[0]));
        TEST_ESP_OK(twai_node_disable(node_hdl[0]));
        TEST_ESP_OK(twai_node_enable(node_hdl[0]));
        TEST_ESP_OK(twai_node_transmit(node_hdl[0], &tx_frame, 0));

        TEST_ESP_OK(twai_node_disable(node_hdl[0]));
        TEST_ESP_OK(twai_node_delete(node_hdl[0]));

        printf("Test install after delete\n");
        TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl[SOC_TWAI_CONTROLLER_NUM]));
        user_cbs.on_rx_done = test_driver_install_rx_cb,
        TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl[SOC_TWAI_CONTROLLER_NUM], &user_cbs, NULL));
        TEST_ESP_OK(twai_node_enable(node_hdl[SOC_TWAI_CONTROLLER_NUM]));
        tx_frame.header.id = 0x100;
        TEST_ESP_OK(twai_node_transmit(node_hdl[SOC_TWAI_CONTROLLER_NUM], &tx_frame, 0));
        twai_frame_t rx_frame;
        printf("Test receive from task\n");
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, twai_node_receive_from_isr(node_hdl[SOC_TWAI_CONTROLLER_NUM], &rx_frame.header, rx_frame.buffer, rx_frame.buffer_len, NULL));

        TEST_ESP_OK(twai_node_disable(node_hdl[SOC_TWAI_CONTROLLER_NUM]));
        for (uint8_t i = 1; i <= SOC_TWAI_CONTROLLER_NUM; i++) {
            printf("Uninstall TWAI%d\n", i - 1);
            TEST_ESP_OK(twai_node_delete(node_hdl[i]));
        }
    }
}

static bool test_enable_disable_rx_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    twai_frame_t *rx_frame = user_ctx;
    size_t ret_len;
    if (ESP_OK == twai_node_receive_from_isr(handle, &rx_frame->header, rx_frame->buffer, rx_frame->buffer_len, &ret_len)) {
        ESP_EARLY_LOGI("twai", "RX id 0x%x len %d ext %d fd %d brs %d esi %d", rx_frame->header.id, ret_len, rx_frame->header.ide, rx_frame->header.fdf, rx_frame->header.brs, rx_frame->header.esi);
        rx_frame->buffer += rx_frame->buffer_len;
    }
    return false;
}

TEST_CASE("twai transmit stop resume (loopback)", "[TWAI]")
{
    // prepare test memory
    uint8_t *send_pkg_ptr = heap_caps_malloc(TEST_TRANS_LEN, MALLOC_CAP_8BIT);
    uint8_t *recv_pkg_ptr = heap_caps_malloc(TEST_TRANS_LEN, MALLOC_CAP_8BIT);
    TEST_ASSERT(send_pkg_ptr && recv_pkg_ptr);
    printf("Transmit %d bytes package in %d frames\n", TEST_TRANS_LEN, TEST_FRAME_NUM);

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

    // reconfig fd timing to 48M/(12+5+6+1)=2MHz, ssp=20/(12+5+6+1)=83%
    twai_timing_advanced_config_t timing_fd = {
        .brp = 1,
        .prop_seg = 12,
        .tseg_1 = 5,
        .tseg_2 = 6,
        .sjw = 3,
        .ssp_offset = 20,
    };
    TEST_ESP_OK(twai_node_reconfig_timing(node_hdl, NULL, &timing_fd));

    twai_frame_t rx_frame = {
        .buffer = recv_pkg_ptr,
        .buffer_len = TEST_FRAME_LEN,
    };
    twai_event_callbacks_t user_cbs = {
        .on_rx_done = test_enable_disable_rx_cb,
    };
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, &rx_frame));
    TEST_ESP_OK(twai_node_enable(node_hdl));

    //create and enqueue all transfers
    twai_frame_t *tx_msgs = heap_caps_calloc(TEST_FRAME_NUM, sizeof(twai_frame_t), MALLOC_CAP_8BIT);
    TEST_ASSERT(tx_msgs);
    for (uint32_t tx_cnt = 0; tx_cnt < TEST_FRAME_NUM; tx_cnt++) {
        tx_msgs[tx_cnt].header.id = tx_cnt | 0x400;
        tx_msgs[tx_cnt].header.ide = !!(tx_cnt % 3);
        tx_msgs[tx_cnt].header.fdf = !!(tx_cnt % 4);
        tx_msgs[tx_cnt].header.brs = !!(tx_cnt % 2);
        tx_msgs[tx_cnt].buffer = send_pkg_ptr + tx_cnt * TEST_FRAME_LEN;
        tx_msgs[tx_cnt].buffer_len = ((tx_cnt + 1) == TEST_FRAME_NUM) ? (TEST_TRANS_LEN - tx_cnt * TEST_FRAME_LEN) : TEST_FRAME_LEN;
        TEST_ESP_OK(twai_node_transmit(node_hdl, &tx_msgs[tx_cnt], 500));
    }

    TEST_ESP_OK(twai_node_disable(node_hdl));
    for (uint8_t i = 3; i > 0; i--) {
        printf("interrupted, %d sec\n", i);
        vTaskDelay(1000);
    }
    printf("continuing ...\n");
    TEST_ESP_OK(twai_node_enable(node_hdl));

    //waiting pkg receive finish
    while (rx_frame.buffer < recv_pkg_ptr + TEST_TRANS_LEN);
    free(tx_msgs);

    // check if pkg receive correct
    printf("pkg check %s!!\n", memcmp(recv_pkg_ptr, send_pkg_ptr, TEST_TRANS_LEN) ? "failed" : "ok");
    TEST_ASSERT_EQUAL_HEX8_ARRAY(send_pkg_ptr, recv_pkg_ptr, TEST_TRANS_LEN);

    free(send_pkg_ptr);
    free(recv_pkg_ptr);
    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}

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

static bool test_filter_rx_done_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    uint8_t *test_ctrl = user_ctx;
    size_t ret_len;
    uint8_t recv_pkg_ptr[TWAIFD_FRAME_MAX_LEN];
    twai_frame_t rx_frame = {
        .buffer = recv_pkg_ptr,
        .buffer_len = TWAIFD_FRAME_MAX_LEN,
    };
    if (ESP_OK == twai_node_receive_from_isr(handle, &rx_frame.header, rx_frame.buffer, rx_frame.buffer_len, &ret_len)) {
        ESP_EARLY_LOGI("twai", "RX id 0x%4x len %2d ext %d rmt %d fd %d", rx_frame.header.id, ret_len, rx_frame.header.ide, rx_frame.header.rtr, rx_frame.header.fdf);
        switch (test_ctrl[0]) {
        case 0: //filter 0
            TEST_ASSERT(rx_frame.header.id >= 0x10);
            TEST_ASSERT(!rx_frame.header.ide);
            break;
        case 1:
        case 2: break;
        case 3: //filter 1
            TEST_ASSERT((rx_frame.header.id & 0xfff0) == 0xf000);
            TEST_ASSERT(rx_frame.header.ide && !rx_frame.header.fdf);
            break;
        case 4: //range filter 0
            TEST_ASSERT(!rx_frame.header.ide && rx_frame.header.fdf);
            break;
        default: TEST_ASSERT(false);
        }
        test_ctrl[1] ++;
    }
    return false;
}

TEST_CASE("twai filter (loopback)", "[TWAI]")
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
        .on_rx_done = test_filter_rx_done_cb,
    };
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, test_ctrl));

    //------------------ Filter 0 -------------------//
    test_ctrl[0] = 0;
    test_ctrl[1] = 0;
    printf("Testing mask filter 0\n");
    twai_mask_filter_config_t mfilter_cfg0 = {
        .id = 0x10,
        .mask = 0xf0,
        .is_ext = false,
    };
    TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, 0, &mfilter_cfg0));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 30);
    TEST_ASSERT_UINT8_WITHIN(30 / 2 - 1, 30 / 2, test_ctrl[1]);    //after filter, rx frame should less than tx num

    printf("Change to receive ALL\n");
    test_ctrl[0] = 1;
    test_ctrl[1] = 0;
    // set to receive ALL
    mfilter_cfg0.id = 0;
    mfilter_cfg0.mask = 0;
    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, 0, &mfilter_cfg0));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 30);
    TEST_ASSERT_EQUAL(30, test_ctrl[1]);

    printf("Change to receive NONE\n");
    test_ctrl[0] = 2;
    test_ctrl[1] = 0;
    // set to receive NONE
    mfilter_cfg0.id = 0xFFFFFFFF;
    mfilter_cfg0.mask = 0xFFFFFFFF;
    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, 0, &mfilter_cfg0));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 30);
    TEST_ASSERT_EQUAL(0, test_ctrl[1]);

    //------------------ Filter 1 -------------------//
    test_ctrl[0] = 3;
    test_ctrl[1] = 0;
    printf("Testing mask filter 1\n");
    twai_mask_filter_config_t mfilter_cfg1 = {
        .id = 0xf000,
        .mask = 0xfff0,
        .is_ext = true,
        .no_fd = true,
    };
    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, 1, &mfilter_cfg1));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 30);
    TEST_ASSERT_UINT8_WITHIN(30 / 2 - 1, 30 / 2, test_ctrl[1]);
    // set to receive NONE
    mfilter_cfg1.id = 0xFFFFFFFF;
    mfilter_cfg1.mask = 0xFFFFFFFF;
    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, 1, &mfilter_cfg1));

    //------------------ Range Filter 0 -------------------//
    test_ctrl[0] = 4;
    test_ctrl[1] = 0;
    printf("Testing range filter 0\n");
    twai_range_filter_config_t rfilter_cfg0 = {
        .range_low = 0,
        .range_high = 0xFFFFFFFF,
        .is_ext = false,
        .no_classic = true,
    };
    TEST_ESP_OK(twai_node_config_range_filter(node_hdl, 0, &rfilter_cfg0));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 30);
    TEST_ASSERT_UINT8_WITHIN(30 / 2 - 1, 30 / 2, test_ctrl[1]);

    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}
