/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdatomic.h>
#include <sys/param.h>
#include "unity.h"
#include "unity_test_utils_cache.h"
#include "test_utils.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_clk_tree.h"
#include "freertos/FreeRTOS.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "soc/twai_periph.h"
#include "esp_private/gpio.h"
#include "driver/uart.h" // for baudrate detection

#define TEST_TX_GPIO                GPIO_NUM_4
#define TEST_RX_GPIO                GPIO_NUM_5
#define TEST_TWAI_QUEUE_DEPTH       5
#define TEST_TRANS_LEN              100
#define TEST_FRAME_LEN              7
#define TEST_FRAME_NUM              howmany(TEST_TRANS_LEN, TEST_FRAME_LEN)

static IRAM_ATTR bool test_driver_install_rx_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    twai_frame_t rx_frame = {};

    if (ESP_OK == twai_node_receive_from_isr(handle, &rx_frame)) {
        ESP_EARLY_LOGI("Recv ", "id 0x%lx rtr %d", rx_frame.header.id, rx_frame.header.rtr);
    }
    if (rx_frame.header.id != 0x100) {
        TEST_FAIL();    //callback is unregistered, should not run here
    }
    return false;
}

TEST_CASE("twai install uninstall (loopback)", "[twai]")
{
    esp_err_t ret;
    twai_node_handle_t node_hdl[SOC_TWAI_CONTROLLER_NUM + 1];
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.tx = TEST_TX_GPIO;
    node_config.io_cfg.rx = TEST_TX_GPIO; // Using same pin for test without transceiver
    node_config.bit_timing.bitrate = 1000000;
    node_config.tx_queue_depth = TEST_TWAI_QUEUE_DEPTH;
    node_config.flags.enable_self_test = true;
    node_config.flags.enable_loopback = true;

    // loop 10 times to check memory leak
    for (uint8_t loop = 0; loop < 10; loop ++) {
        for (uint8_t i = 0; i < SOC_TWAI_CONTROLLER_NUM + 1; i++) {
            ret = twai_new_node_onchip(&node_config, &node_hdl[i]);
            printf("Install TWAI%d return %s\n", i, esp_err_to_name(ret));
            TEST_ASSERT(ret == ((i < SOC_TWAI_CONTROLLER_NUM) ? ESP_OK : ESP_ERR_NOT_FOUND));
        }
        // can't disable before enable
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, twai_node_disable(node_hdl[0]));

        twai_event_callbacks_t user_cbs = {};
        user_cbs.on_rx_done = test_driver_install_rx_cb;
        TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl[0], &user_cbs, NULL));

        printf("Test unregister callback\n");
        user_cbs.on_rx_done = NULL;
        TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl[0], &user_cbs, NULL));

        twai_frame_t tx_frame = {};
        tx_frame.header.id = 0x82;
        tx_frame.header.rtr = true;
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
        TEST_ESP_OK(twai_node_transmit_wait_all_done(node_hdl[SOC_TWAI_CONTROLLER_NUM], 0));    // test wait before transmit
        TEST_ESP_OK(twai_node_transmit(node_hdl[SOC_TWAI_CONTROLLER_NUM], &tx_frame, 0));
        TEST_ESP_ERR(ESP_ERR_TIMEOUT, twai_node_transmit_wait_all_done(node_hdl[SOC_TWAI_CONTROLLER_NUM], 0));  // test return before finish
        twai_frame_t rx_frame = {};
        printf("Test receive from task\n");
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, twai_node_receive_from_isr(node_hdl[SOC_TWAI_CONTROLLER_NUM], &rx_frame));

        TEST_ESP_OK(twai_node_disable(node_hdl[SOC_TWAI_CONTROLLER_NUM]));
        for (uint8_t i = 1; i <= SOC_TWAI_CONTROLLER_NUM; i++) {
            printf("Uninstall TWAI%d\n", i - 1);
            TEST_ESP_OK(twai_node_delete(node_hdl[i]));
        }
    }
}

static void test_twai_baudrate_correctness(twai_clock_source_t clk_src, uint32_t test_bitrate)
{
    twai_node_handle_t twai_node = NULL;
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.tx = TEST_TX_GPIO;
    node_config.io_cfg.rx = TEST_TX_GPIO;
    node_config.clk_src = clk_src;
    node_config.bit_timing.bitrate = test_bitrate;
    node_config.tx_queue_depth = 1;
    node_config.flags.enable_self_test = true;
    node_config.flags.enable_loopback = true;
    TEST_ESP_OK(twai_new_node_onchip(&node_config, &twai_node));
    TEST_ESP_OK(twai_node_enable(twai_node));
    printf("TWAI driver installed @ %ld Hz\n", test_bitrate);

    // We use the UART baudrate detection submodule to measure the TWAI baudrate
    uart_bitrate_detect_config_t detect_config = {};
    detect_config.rx_io_num = TEST_TX_GPIO;
    TEST_ESP_OK(uart_detect_bitrate_start(UART_NUM_1, &detect_config));

    twai_frame_t tx_frame = {};
    tx_frame.header.id = 0x55555;
    tx_frame.header.dlc = 8;
    tx_frame.header.ide = true;
    tx_frame.buffer = (uint8_t []) {
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
    };
    tx_frame.buffer_len = 8;

    TEST_ESP_OK(twai_node_transmit(twai_node, &tx_frame, 500));
    vTaskDelay(100);

    // analyze the measurement result
    uart_bitrate_res_t measure_result;
    TEST_ESP_OK(uart_detect_bitrate_stop(UART_NUM_1, true, &measure_result));
    uint32_t bitrate_measured = measure_result.clk_freq_hz * 4 / (measure_result.pos_period + measure_result.neg_period);
    printf("TWAI bitrate measured: %" PRIu32 "\r\n", bitrate_measured);
    TEST_ASSERT_INT_WITHIN(1000, test_bitrate, bitrate_measured); // 1k tolerance

    TEST_ESP_OK(twai_node_disable(twai_node));
    TEST_ESP_OK(twai_node_delete(twai_node));
}

TEST_CASE("twai baudrate measurement", "[twai]")
{
    twai_clock_source_t twai_available_clk_srcs[] = SOC_TWAI_CLKS;
    uint32_t source_freq = 0;
    for (size_t i = 0; i < sizeof(twai_available_clk_srcs) / sizeof(twai_available_clk_srcs[0]); i++) {
        TEST_ESP_OK(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)twai_available_clk_srcs[i], ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &source_freq));
        printf("Test clock source %d frequency: %ld Hz\n", twai_available_clk_srcs[i], source_freq);
        test_twai_baudrate_correctness(twai_available_clk_srcs[i], 200000);

        test_twai_baudrate_correctness(twai_available_clk_srcs[i], 1000000);
    }
}

static IRAM_ATTR bool test_enable_disable_rx_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    twai_frame_t *rx_frame = (twai_frame_t *)user_ctx;
    if (ESP_OK == twai_node_receive_from_isr(handle, rx_frame)) {
        ESP_EARLY_LOGI("Recv", "RX id 0x%x len %d ext %d brs %d esi %d", rx_frame->header.id, twaifd_dlc2len(rx_frame->header.dlc), rx_frame->header.ide, rx_frame->header.brs, rx_frame->header.esi);
        rx_frame->buffer += rx_frame->buffer_len;
    }
    return false;
}

TEST_CASE("twai transmit stop resume (loopback)", "[twai]")
{
    // prepare test memory
    uint8_t *send_pkg_ptr = (uint8_t*)heap_caps_malloc(TEST_TRANS_LEN, MALLOC_CAP_8BIT);
    uint8_t *recv_pkg_ptr = (uint8_t*)heap_caps_malloc(TEST_TRANS_LEN, MALLOC_CAP_8BIT);
    TEST_ASSERT(send_pkg_ptr && recv_pkg_ptr);
    printf("Transmit %d bytes package in %d frames\n", TEST_TRANS_LEN, TEST_FRAME_NUM);

    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.tx = TEST_TX_GPIO;
    node_config.io_cfg.rx = TEST_TX_GPIO; // Using same pin for test without transceiver
    node_config.bit_timing.bitrate = 200000;
    node_config.tx_queue_depth = TEST_TWAI_QUEUE_DEPTH;
    node_config.flags.enable_self_test = true;
    node_config.flags.enable_loopback = true;

    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));

    twai_frame_t rx_frame = {};
    rx_frame.buffer = recv_pkg_ptr;
    rx_frame.buffer_len = TEST_FRAME_LEN;

    twai_event_callbacks_t user_cbs = {};
    user_cbs.on_rx_done = test_enable_disable_rx_cb;
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, &rx_frame));
    TEST_ESP_OK(twai_node_enable(node_hdl));

    //create and enqueue all transfers
    twai_frame_t *tx_msgs = (twai_frame_t*)heap_caps_calloc(TEST_FRAME_NUM, sizeof(twai_frame_t), MALLOC_CAP_8BIT);
    TEST_ASSERT(tx_msgs);
    for (uint32_t tx_cnt = 0; tx_cnt < TEST_FRAME_NUM; tx_cnt++) {
        tx_msgs[tx_cnt].header.id = tx_cnt | 0x400;
        tx_msgs[tx_cnt].header.ide = !!(tx_cnt % 3);
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
    TEST_ESP_OK(twai_node_transmit_wait_all_done(node_hdl, -1));
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
    uint8_t send_pkg_ptr[TWAI_FRAME_MAX_LEN];
    twai_frame_t tx_msg = {};
    tx_msg.buffer = send_pkg_ptr;

    printf("Sending %ld random trans ...\n", trans_num);
    for (uint32_t tx_cnt = 0; tx_cnt < trans_num; tx_cnt++) {
        tx_msg.header.id = tx_cnt | 0xf000;
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

TEST_CASE("twai mask filter (loopback)", "[twai]")
{
    uint8_t test_ctrl[2];
    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.tx = TEST_TX_GPIO;
    node_config.io_cfg.rx = TEST_TX_GPIO; // Using same pin for test without transceiver
    node_config.bit_timing.bitrate = 1000000;
    node_config.tx_queue_depth = TEST_TWAI_QUEUE_DEPTH;
    node_config.flags.enable_self_test = true;
    node_config.flags.enable_loopback = true;

    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));

    twai_event_callbacks_t user_cbs = {};
    user_cbs.on_rx_done = test_filter_rx_done_cb;
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, test_ctrl));

    twai_mask_filter_config_t mfilter_cfg = {};
    for (uint8_t i = 0; i < SOC_TWAI_MASK_FILTER_NUM; i++) {
        printf("\n--------------------------------------\n");
        test_ctrl[0] = 0;
        test_ctrl[1] = 0;
        mfilter_cfg.id = 0x10,
        mfilter_cfg.mask = 0xf0,
        mfilter_cfg.is_ext = false,
        printf("Testing mask filter %d: id 0x%lx mask 0x%lx is_ext %d\n", i, mfilter_cfg.id, mfilter_cfg.mask, mfilter_cfg.is_ext);
        TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, i, &mfilter_cfg));
        TEST_ESP_OK(twai_node_enable(node_hdl));
        test_random_trans_generator(node_hdl, 30);
        TEST_ASSERT_EQUAL(7, test_ctrl[1]);     // must receive 7 of 30 frames under filter config

        test_ctrl[0] = 1;
        test_ctrl[1] = 0;
        mfilter_cfg.id = 0;
        mfilter_cfg.mask = 0;
        printf("Change filter %d to receive ALL: id 0x%lx mask 0x%lx\n", i, mfilter_cfg.id, mfilter_cfg.mask);
        TEST_ESP_OK(twai_node_disable(node_hdl));
        TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, i, &mfilter_cfg));
        TEST_ESP_OK(twai_node_enable(node_hdl));
        test_random_trans_generator(node_hdl, 20);
        TEST_ASSERT_EQUAL(20, test_ctrl[1]);

        test_ctrl[0] = 2;
        test_ctrl[1] = 0;
        mfilter_cfg.id = 0xFFFFFFFF;
        mfilter_cfg.mask = 0xFFFFFFFF;
        printf("Disable filter %d: id 0x%lx mask 0x%lx\n", i, mfilter_cfg.id, mfilter_cfg.mask);
        TEST_ESP_OK(twai_node_disable(node_hdl));
        TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, i, &mfilter_cfg));
        TEST_ESP_OK(twai_node_enable(node_hdl));
        test_random_trans_generator(node_hdl, 40);
        TEST_ASSERT_EQUAL(0, test_ctrl[1]);
        TEST_ESP_OK(twai_node_disable(node_hdl));
    }
    TEST_ESP_OK(twai_node_delete(node_hdl));
}

//------------------ Dual Filter Test -------------------//
#if !SOC_TWAI_SUPPORT_FD
static IRAM_ATTR bool test_dual_filter_rx_done_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
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
            TEST_ASSERT(!rx_frame.header.ide);
            TEST_ASSERT((rx_frame.header.id >= 0x10) && (rx_frame.header.id <= 0x2f));
            break;
        case 1: break;  // receive none
        default: TEST_ASSERT(false);
        }
        test_ctrl[1] ++;
    }
    return false;
}

TEST_CASE("twai dual 16bit mask filter (loopback)", "[twai]")
{
    uint8_t test_ctrl[2];
    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.tx = TEST_TX_GPIO;
    node_config.io_cfg.rx = TEST_TX_GPIO; // Using same pin for test without transceiver
    node_config.bit_timing.bitrate = 1000000;
    node_config.tx_queue_depth = TEST_TWAI_QUEUE_DEPTH;
    node_config.flags.enable_self_test = true;
    node_config.flags.enable_loopback = true;

    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));

    twai_event_callbacks_t user_cbs = {};
    user_cbs.on_rx_done = test_dual_filter_rx_done_cb;
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, test_ctrl));

    printf("Testing dual filter: id1 0x%x mask1 0x%x, id2 0x%x mask2 0x%x\n", 0x020, 0x7f0, 0x013, 0x7f8);
    test_ctrl[0] = 0;
    test_ctrl[1] = 0;
    // filter 1 receive only std id 0x02x
    // filter 2 receive only std id 0x010~0x017
    twai_mask_filter_config_t dual_config = twai_make_dual_filter(0x020, 0x7f0, 0x013, 0x7f8, false);
    TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, 0, &dual_config));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 50);
    TEST_ASSERT_EQUAL(12, test_ctrl[1]);    // must receive 12 of 50 frames under filter config

    printf("Disable filter\n");
    test_ctrl[0] = 1;
    test_ctrl[1] = 0;
    dual_config.id = 0xFFFFFFFF;
    dual_config.mask = 0xFFFFFFFF;
    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_config_mask_filter(node_hdl, 0, &dual_config));
    TEST_ESP_OK(twai_node_enable(node_hdl));
    test_random_trans_generator(node_hdl, 40);
    TEST_ASSERT_EQUAL(0, test_ctrl[1]);

    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}
#endif

#if CONFIG_TWAI_ISR_CACHE_SAFE
static IRAM_ATTR bool test_iram_safe_rx_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    twai_frame_t *rx_frame = (twai_frame_t *)user_ctx;
    if (ESP_OK == twai_node_receive_from_isr(handle, rx_frame)) {
        esp_rom_printf(DRAM_STR("RX id 0x%x len %d ext %d brs %d esi %d\n"), rx_frame->header.id, twaifd_dlc2len(rx_frame->header.dlc), rx_frame->header.ide, rx_frame->header.brs, rx_frame->header.esi);
        rx_frame->buffer += rx_frame->buffer_len;
    }
    return false;
}

static void IRAM_ATTR test_wait_trans_done_cache_disable(void *args)
{
    twai_frame_t *rx_frame = ((twai_frame_t **)args)[0];
    uint8_t *orig_buff = ((uint8_t **)args)[1];

    esp_rom_printf(DRAM_STR("Cache disabled now !!!\n"));
    //waiting pkg receive finish
    while (rx_frame->buffer < orig_buff + TEST_TRANS_LEN) {
        esp_rom_delay_us(1000);
    }
}

TEST_CASE("twai driver cache safe (loopback)", "[twai]")
{
    // prepare test memory
    uint8_t *send_pkg_ptr = (uint8_t *)heap_caps_malloc(TEST_TRANS_LEN, MALLOC_CAP_8BIT);
    uint8_t *recv_pkg_ptr = (uint8_t *)heap_caps_malloc(TEST_TRANS_LEN, MALLOC_CAP_8BIT);
    TEST_ASSERT(send_pkg_ptr && recv_pkg_ptr);
    printf("Transmit %d bytes package in %d frames\n", TEST_TRANS_LEN, TEST_FRAME_NUM);

    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.tx = TEST_TX_GPIO;
    node_config.io_cfg.rx = TEST_TX_GPIO; // Using same pin for test without transceiver
    node_config.bit_timing.bitrate = 50000; //slow bitrate to ensure cache disabled before tx_queue finish
    node_config.tx_queue_depth = TEST_FRAME_NUM;
    node_config.flags.enable_loopback = true;
    node_config.flags.enable_self_test = true;

    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));

    twai_frame_t rx_frame = {};
    rx_frame.buffer = recv_pkg_ptr;
    rx_frame.buffer_len = TEST_FRAME_LEN;

    twai_event_callbacks_t user_cbs = {};
    user_cbs.on_rx_done = test_iram_safe_rx_cb;
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, &rx_frame));
    TEST_ESP_OK(twai_node_enable(node_hdl));

    //create and enqueue all transfers
    twai_frame_t *tx_msgs = (twai_frame_t *)heap_caps_calloc(TEST_FRAME_NUM, sizeof(twai_frame_t), MALLOC_CAP_8BIT);
    TEST_ASSERT(tx_msgs);
    for (uint32_t tx_cnt = 0; tx_cnt < TEST_FRAME_NUM; tx_cnt++) {
        tx_msgs[tx_cnt].header.id = tx_cnt | 0x400;
        tx_msgs[tx_cnt].header.ide = !!(tx_cnt % 3);
        tx_msgs[tx_cnt].buffer = send_pkg_ptr + tx_cnt * TEST_FRAME_LEN;
        tx_msgs[tx_cnt].buffer_len = ((tx_cnt + 1) == TEST_FRAME_NUM) ? (TEST_TRANS_LEN - tx_cnt * TEST_FRAME_LEN) : TEST_FRAME_LEN;
        TEST_ESP_OK(twai_node_transmit(node_hdl, &tx_msgs[tx_cnt], 0));
    }

    //disable cache immediately before tx_queue finish
    void *user_data[2] = {&rx_frame, recv_pkg_ptr};
    unity_utils_run_cache_disable_stub(test_wait_trans_done_cache_disable, user_data);

    //if it is able to waiting finish, means pass the test
    free(tx_msgs);

    // check if pkg receive correct
    printf("Transaction finish, pkg check %s!!\n", memcmp(recv_pkg_ptr, send_pkg_ptr, TEST_TRANS_LEN) ? "failed" : "ok");
    TEST_ASSERT_EQUAL_HEX8_ARRAY(send_pkg_ptr, recv_pkg_ptr, TEST_TRANS_LEN);

    free(send_pkg_ptr);
    free(recv_pkg_ptr);
    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}
#endif //CONFIG_TWAI_ISR_CACHE_SAFE

TEST_CASE("twai bus off recovery (loopback)", "[twai]")
{
    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.tx = TEST_TX_GPIO;
    node_config.io_cfg.rx = TEST_TX_GPIO;   // Using same pin for test without transceiver
    node_config.bit_timing.bitrate = 50000; //slow bitrate to ensure soft error trigger
    node_config.tx_queue_depth = 1;
    node_config.flags.enable_self_test = true;

    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));
    TEST_ESP_OK(twai_node_enable(node_hdl));

    twai_node_status_t node_status;
    twai_frame_t tx_frame = {};
    tx_frame.buffer = (uint8_t *)"hello\n";
    tx_frame.buffer_len = 6;

    // send frames and trigger error, must become bus off before 50 frames
    while ((node_status.state != TWAI_ERROR_BUS_OFF) && (tx_frame.header.id < 50)) {
        printf("sending frame %ld\n", tx_frame.header.id ++);
        TEST_ESP_OK(twai_node_transmit(node_hdl, &tx_frame, 500));
        if (tx_frame.header.id > 3) {    // trigger error after 3 frames
            printf("trigger bit_error now!\n");
            esp_rom_delay_us(30 * (1000000 / node_config.bit_timing.bitrate)); // trigger error at 30 bits after frame start
            gpio_matrix_output(TEST_TX_GPIO, twai_periph_signals[0].tx_sig, true, false);
            esp_rom_delay_us(2 * (1000000 / node_config.bit_timing.bitrate)); // trigger error for 2 bits
            gpio_matrix_output(TEST_TX_GPIO, twai_periph_signals[0].tx_sig, false, false);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // some time for hardware report errors
        twai_node_get_info(node_hdl, &node_status, NULL);
    }

    // recover node
    TEST_ASSERT_EQUAL(TWAI_ERROR_BUS_OFF, node_status.state);
    printf("node offline, start recover ...\n");
    TEST_ESP_OK(twai_node_recover(node_hdl));

    // wait node recovered
    while (node_status.state != TWAI_ERROR_ACTIVE) {
        printf("waiting ...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        twai_node_get_info(node_hdl, &node_status, NULL);
    }
    printf("node recovered! continue\n");
    TEST_ESP_OK(twai_node_transmit(node_hdl, &tx_frame, 500));

    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}

static void test_send_wait_task(void *args)
{
    const char *task_name = pcTaskGetName(NULL);
    twai_node_handle_t node_hdl = ((twai_node_handle_t *)args)[0];
    atomic_int *tasks_finished = ((atomic_int **)args)[1];
    twai_frame_t tx_frame = {};
    tx_frame.header.rtr = true;

    for (uint8_t i = 0; i < 10; i++) {
        tx_frame.header.id = 0x100 + i;
        printf("%s send %d\n", task_name, i);
        TEST_ESP_OK(twai_node_transmit(node_hdl, &tx_frame, -1));
        printf("%s wait %d\n", task_name, i);
        TEST_ESP_OK(twai_node_transmit_wait_all_done(node_hdl, -1));
    }
    printf("%s finished\n", task_name);
    atomic_fetch_add(tasks_finished, 1);
    vTaskDelete(NULL);
}

TEST_CASE("twai tx_wait_all_done thread safe", "[twai]")
{
    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {};
    node_config.io_cfg.tx = TEST_TX_GPIO;
    node_config.io_cfg.rx = TEST_TX_GPIO;    //Using same pin for test without transceiver
    node_config.bit_timing.bitrate = 100000; //slow bitrate to ensure transmite finish during wait_all_done
    node_config.tx_queue_depth = TEST_FRAME_NUM;
    node_config.flags.enable_self_test = true;

    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));
    TEST_ESP_OK(twai_node_enable(node_hdl));

    atomic_int tasks_finished = 0;
    void *args[2] = {node_hdl, &tasks_finished};
    xTaskCreate(test_send_wait_task, "task_high", 4096, args, 1, NULL);
    xTaskCreate(test_send_wait_task, "task_low ", 4096, args, 0, NULL);

    vTaskDelay(500);   //wait for tasks finished
    printf("tasks_finished: %d\n", atomic_load(&tasks_finished));
    TEST_ASSERT_EQUAL(2, atomic_load(&tasks_finished));

    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}
