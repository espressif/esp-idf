/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils_cache.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "test_utils.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"

#define TEST_TX_GPIO                4
#define TEST_RX_GPIO                5

static bool IRAM_ATTR test_listen_only_rx_cb(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    twai_frame_t *rx_frame = ((twai_frame_t **)user_ctx)[1];
    uint8_t *rx_cnt = ((uint8_t **)user_ctx)[0];
    if (ESP_OK == twai_node_receive_from_isr(handle, rx_frame)) {
        *rx_cnt += 1;
    }
    return false;
}

TEST_CASE("twai_listen_only", "[twai_net]")
{
    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = TEST_TX_GPIO,
        .io_cfg.rx = TEST_RX_GPIO,
        .bit_timing.bitrate = 250000,
        .tx_queue_depth = 3,
        .flags.enable_listen_only = true,
    };
    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));
    ESP_LOGI("Test", "driver installed");

    uint8_t rx_buffer[8] = {0};
    twai_frame_t rx_frame = {
        .buffer = rx_buffer,
        .buffer_len = sizeof(rx_buffer),
    };
    uint8_t rx_msg_cnt = 0;
    void *user_data[2] = {&rx_msg_cnt, &rx_frame};

    twai_event_callbacks_t user_cbs = {
        .on_rx_done = test_listen_only_rx_cb,
    };
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, user_data));
    TEST_ESP_OK(twai_node_enable(node_hdl));

    ESP_LOGI("Test", "Listening ...");
    while (!rx_msg_cnt) {
        vTaskDelay(1);
    }
    ESP_LOGI("Test", "receive with id 0x%lx", rx_frame.header.id);
    ESP_LOG_BUFFER_HEX("Data", rx_frame.buffer, twaifd_dlc2len(rx_frame.header.dlc));
    TEST_ASSERT_EQUAL_HEX(0x6688, rx_frame.header.id);
    uint8_t expected_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_frame.buffer, twaifd_dlc2len(rx_frame.header.dlc));

    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}

TEST_CASE("twai_remote_request", "[twai_net]")
{
    twai_node_handle_t node_hdl;
    twai_onchip_node_config_t node_config = {
        .io_cfg.tx = TEST_TX_GPIO,
        .io_cfg.rx = TEST_RX_GPIO,
        .bit_timing.bitrate = 250000,
        .fail_retry_cnt = -1,   // retry forever if send remote frame failed
        .tx_queue_depth = 3,
    };
    TEST_ESP_OK(twai_new_node_onchip(&node_config, &node_hdl));
    ESP_LOGI("Test", "driver installed");

    uint8_t rx_buffer[8] = {0};
    twai_frame_t rx_frame = {
        .buffer = rx_buffer,
        .buffer_len = sizeof(rx_buffer),
    };
    uint8_t rx_msg_cnt = 0;
    void *user_data[2] = {&rx_msg_cnt, &rx_frame};

    twai_event_callbacks_t user_cbs = {
        .on_rx_done = test_listen_only_rx_cb,
    };
    TEST_ESP_OK(twai_node_register_event_callbacks(node_hdl, &user_cbs, user_data));
    TEST_ESP_OK(twai_node_enable(node_hdl));

    twai_frame_t tx_frame = {
        .header.id = 0x123,
        .header.dlc = 8,
        .header.rtr = true,
        .header.ide = true,
    };
    TEST_ESP_OK(twai_node_transmit(node_hdl, &tx_frame, 1000));
    ESP_LOGI("Test", "send remote frame");

    uint8_t expected_data[8] = {0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10};
    //waiting pkg receive finish
    while (!rx_msg_cnt) {
        vTaskDelay(1);
    }
    ESP_LOGI("Test", "receive with id 0x%lx", rx_frame.header.id);
    ESP_LOG_BUFFER_HEX("Data", rx_frame.buffer, twaifd_dlc2len(rx_frame.header.dlc));
    TEST_ASSERT_EQUAL_HEX(0x123, rx_frame.header.id);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_frame.buffer, twaifd_dlc2len(rx_frame.header.dlc));

    TEST_ESP_OK(twai_node_disable(node_hdl));
    TEST_ESP_OK(twai_node_delete(node_hdl));
}
