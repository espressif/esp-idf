/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "soc/soc_caps.h"
#include "test_util_rmt_encoders.h"

#if CONFIG_RMT_ISR_IRAM_SAFE
#define TEST_RMT_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_RMT_CALLBACK_ATTR
#endif

typedef struct {
    TaskHandle_t task_to_notify;
    size_t received_symbol_num;
} test_nec_rx_user_data_t;

TEST_RMT_CALLBACK_ATTR
static bool test_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    test_nec_rx_user_data_t *test_user_data = (test_nec_rx_user_data_t *)user_data;
    rmt_symbol_word_t *remote_codes = edata->received_symbols;
    esp_rom_printf("%u symbols decoded:\r\n", edata->num_symbols);
    for (size_t i = 0; i < edata->num_symbols; i++) {
        esp_rom_printf("{%d:%d},{%d:%d}\r\n", remote_codes[i].level0, remote_codes[i].duration0, remote_codes[i].level1, remote_codes[i].duration1);
    }
    test_user_data->received_symbol_num = edata->num_symbols;
    vTaskNotifyGiveFromISR(test_user_data->task_to_notify, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void test_rmt_rx_nec_carrier(size_t mem_block_symbols, bool with_dma, rmt_clock_source_t clk_src)
{
    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = clk_src,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = mem_block_symbols,
        .gpio_num = 0,
        .flags.with_dma = with_dma,
        .flags.io_loop_back = true, // the GPIO will act like a loopback
    };
    printf("install rx channel\r\n");
    rmt_channel_handle_t rx_channel = NULL;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));
    printf("register rx event callbacks\r\n");
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = test_rmt_rx_done_callback,
    };
    test_nec_rx_user_data_t test_user_data = {
        .task_to_notify = xTaskGetCurrentTaskHandle(),
    };
    TEST_ESP_OK(rmt_rx_register_event_callbacks(rx_channel, &cbs, &test_user_data));

    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = clk_src,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .trans_queue_depth = 4,
        .gpio_num = 0,
        .flags.io_loop_back = true, // TX channel and RX channel will bounded to the same GPIO
    };
    printf("install tx channel\r\n");
    rmt_channel_handle_t tx_channel = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));

    printf("install nec protocol encoder\r\n");
    rmt_encoder_handle_t nec_encoder = NULL;
    TEST_ESP_OK(test_rmt_new_nec_protocol_encoder(&nec_encoder));
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
    };

    printf("enable tx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel));
    printf("enable rx channel\r\n");
    TEST_ESP_OK(rmt_enable(rx_channel));

    rmt_symbol_word_t remote_codes[128];

    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 12000000,
    };

    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, sizeof(remote_codes), &receive_config));
    printf("send NEC frame without carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(34, test_user_data.received_symbol_num);

    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, sizeof(remote_codes), &receive_config));
    printf("send NEC frame without carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(34, test_user_data.received_symbol_num);

#if SOC_RMT_SUPPORT_RX_PINGPONG
    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, sizeof(remote_codes), &receive_config));
    printf("send customized NEC frame without carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0xFF00, 0xFF00, 0xFF00, 0xFF00
    }, 8, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(66, test_user_data.received_symbol_num);
#else
    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, sizeof(remote_codes), &receive_config));
    printf("send customized NEC frame without carrier\r\n");
    // the maximum symbols can receive is its memory block capacity
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0xFF00, 0xFF00, 0xFF00, 0xFF00, 0xFF00
    }, 10, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(test_user_data.received_symbol_num, mem_block_symbols);
#endif // SOC_RMT_SUPPORT_RX_PINGPONG

#if SOC_RMT_SUPPORT_RX_DEMODULATION
    rmt_carrier_config_t carrier_cfg = {
        .duty_cycle = 0.33,
        .frequency_hz = 38000,
    };
    printf("enable modulation for tx channel\r\n");
    TEST_ESP_OK(rmt_apply_carrier(tx_channel, &carrier_cfg));
    printf("enable demodulation for rx channel\r\n");
    // need to leave a tolerance for the carrier demodulation, can't set the carrier frequency exactly to 38KHz
    // should reduce frequency to some extend
    carrier_cfg.frequency_hz = 25000;
    TEST_ESP_OK(rmt_apply_carrier(rx_channel, &carrier_cfg));

    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, sizeof(remote_codes), &receive_config));
    printf("send NEC frame with carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(34, test_user_data.received_symbol_num);

#if SOC_RMT_SUPPORT_RX_PINGPONG
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, sizeof(remote_codes), &receive_config));
    printf("send customized frame with carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0xFF00, 0xFF00, 0xFF00, 0xFF00
    }, 8, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(66, test_user_data.received_symbol_num);
#endif // SOC_RMT_SUPPORT_RX_PINGPONG

    printf("disable modulation and demodulation for tx and rx channels\r\n");
    TEST_ESP_OK(rmt_apply_carrier(tx_channel, NULL));
    TEST_ESP_OK(rmt_apply_carrier(rx_channel, NULL));
#endif // SOC_RMT_SUPPORT_RX_DEMODULATION

    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, sizeof(remote_codes), &receive_config));
    printf("send NEC frame without carrier\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(34, test_user_data.received_symbol_num);

    TEST_ESP_OK(rmt_tx_wait_all_done(tx_channel, -1));
    printf("disable tx and rx channels\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel));
    TEST_ESP_OK(rmt_disable(rx_channel));
    printf("delete channels and encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(rx_channel));
    TEST_ESP_OK(rmt_del_channel(tx_channel));
    TEST_ESP_OK(rmt_del_encoder(nec_encoder));
}

TEST_CASE("rmt_rx_nec_carrier_no_dma", "[rmt]")
{
    // test width different clock sources
    rmt_clock_source_t clk_srcs[] = SOC_RMT_CLKS;
    for (size_t i = 0; i < sizeof(clk_srcs) / sizeof(clk_srcs[0]); i++) {
        test_rmt_rx_nec_carrier(SOC_RMT_MEM_WORDS_PER_CHANNEL, false, clk_srcs[i]);
    }
}

#if SOC_RMT_SUPPORT_DMA
TEST_CASE("rmt_rx_nec_carrier_with_dma", "[rmt]")
{
    test_rmt_rx_nec_carrier(128, true, RMT_CLK_SRC_DEFAULT);
}
#endif
