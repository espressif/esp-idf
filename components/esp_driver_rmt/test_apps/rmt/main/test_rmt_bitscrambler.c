/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
#include "driver/gpio.h"
#include "driver/bitscrambler.h"
#include "test_board.h"

BITSCRAMBLER_PROGRAM(bitscrambler_program_test_tx, "test_tx");

IRAM_ATTR static bool test_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    TaskHandle_t task_to_notify = (TaskHandle_t)user_data;

    const rmt_symbol_word_t expected_symbols[] = {
        { .level0 = 1, .duration0 = 273, .level1 = 0, .duration1 = 1 },
        { .level0 = 1, .duration0 = 546, .level1 = 0, .duration1 = 2 },
        { .level0 = 1, .duration0 = 819, .level1 = 0, .duration1 = 3 },
        { .level0 = 1, .duration0 = 1092, .level1 = 0, .duration1 = 4 },
        { .level0 = 1, .duration0 = 1, .level1 = 0, .duration1 = 0 },
    };
    rmt_symbol_word_t *remote_codes = edata->received_symbols;
    esp_rom_printf("%u symbols received:\r\n", edata->num_symbols);
    for (int i = 0; i < edata->num_symbols; i++) {
        esp_rom_printf("{%d:%d},{%d:%d}\r\n", remote_codes[i].level0, remote_codes[i].duration0, remote_codes[i].level1, remote_codes[i].duration1);
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(expected_symbols, remote_codes, sizeof(expected_symbols) / sizeof(rmt_symbol_word_t));

    vTaskNotifyGiveFromISR(task_to_notify, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("rmt TX with bitscrambler", "[rmt]")
{
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = 48,  // no need a large DMA memory to save the primary data
        .gpio_num = TEST_RMT_GPIO_NUM_B,
        .trans_queue_depth = 4,
        .flags.with_dma = true,   // bitscrambler has to work with DMA
    };
    printf("install tx channel\r\n");
    rmt_channel_handle_t tx_channel = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));

    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .gpio_num = TEST_RMT_GPIO_NUM_B,
    };
    printf("install rx channel to the same GPIO\r\n");
    rmt_channel_handle_t rx_channel = NULL;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    printf("register rx event callbacks\r\n");
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = test_rmt_rx_done_callback,
    };
    TEST_ESP_OK(rmt_rx_register_event_callbacks(rx_channel, &cbs, xTaskGetCurrentTaskHandle()));

    printf("install bitscrambler encoder\r\n");
    rmt_encoder_handle_t bs_encoder = NULL;
    rmt_bs_encoder_config_t bs_encoder_config = {
        .program_bin = bitscrambler_program_test_tx,
    };
    TEST_ESP_OK(rmt_new_bitscrambler_encoder(&bs_encoder_config, &bs_encoder));

    printf("enable tx+rx channel\r\n");
    TEST_ESP_OK(rmt_enable(tx_channel));
    TEST_ESP_OK(rmt_enable(rx_channel));

    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 500,
        .signal_range_max_ns = 2000000,
    };
    rmt_symbol_word_t symbols[8];
    TEST_ESP_OK(rmt_receive(rx_channel, symbols, sizeof(symbols), &receive_config));

    printf("transmit!\r\n");
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
    };
    TEST_ESP_OK(rmt_transmit(tx_channel, bs_encoder, (uint8_t[]) {
        0x12, 0x34, 0x56, 0x78, 0x9a, // dummy test values, will be further processed by bitscrambler program
    }, 5, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));

    TEST_ESP_OK(rmt_receive(rx_channel, symbols, sizeof(symbols), &receive_config));
    printf("transmit again!\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, bs_encoder, (uint8_t[]) {
        0x12, 0x34, 0x56, 0x78, 0x9a, // dummy test values, will be further processed by bitscrambler program
    }, 5, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));

    printf("disable tx+rx channel\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel));
    TEST_ESP_OK(rmt_disable(rx_channel));
    printf("remove tx+rx channel and bs encoder\r\n");
    TEST_ESP_OK(rmt_del_channel(tx_channel));
    TEST_ESP_OK(rmt_del_channel(rx_channel));
    TEST_ESP_OK(rmt_del_encoder(bs_encoder));
}
