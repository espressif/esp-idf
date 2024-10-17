/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "test_board.h"

TEST_CASE("rmt channel install & uninstall", "[rmt]")
{
    rmt_tx_channel_config_t tx_channel_cfg = {
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .gpio_num = TEST_RMT_GPIO_NUM_A,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .trans_queue_depth = 1,
    };
    rmt_channel_handle_t tx_channels[SOC_RMT_TX_CANDIDATES_PER_GROUP] = {};
    rmt_rx_channel_config_t rx_channel_cfg = {
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .gpio_num = TEST_RMT_GPIO_NUM_B,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
    };
    rmt_channel_handle_t rx_channels[SOC_RMT_RX_CANDIDATES_PER_GROUP] = {};

    printf("install tx/rx channels, each channel takes one memory block\r\n");
    for (int i = 0; i < SOC_RMT_TX_CANDIDATES_PER_GROUP; i++) {
        TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channels[i]));
    }
    // alloc more when tx channels are exhausted should report error
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, rmt_new_tx_channel(&tx_channel_cfg, &tx_channels[0]));
    for (int i = 0; i < SOC_RMT_TX_CANDIDATES_PER_GROUP; i++) {
        TEST_ESP_OK(rmt_del_channel(tx_channels[i]));
    }
    for (int i = 0; i < SOC_RMT_RX_CANDIDATES_PER_GROUP; i++) {
        TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channels[i]));
    }
    // alloc more when rx channels are exhausted should report error
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, rmt_new_rx_channel(&rx_channel_cfg, &rx_channels[0]));
    for (int i = 0; i < SOC_RMT_RX_CANDIDATES_PER_GROUP; i++) {
        TEST_ESP_OK(rmt_del_channel(rx_channels[i]));
    }

    printf("install tx/rx channels, each channel takes two memory blocks\r\n");
    tx_channel_cfg.mem_block_symbols = 2 * SOC_RMT_MEM_WORDS_PER_CHANNEL;
    rx_channel_cfg.mem_block_symbols = 2 * SOC_RMT_MEM_WORDS_PER_CHANNEL;
    for (int i = 0; i < SOC_RMT_TX_CANDIDATES_PER_GROUP / 2; i++) {
        TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channels[i]));
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, rmt_new_tx_channel(&tx_channel_cfg, &tx_channels[0]));
    for (int i = 0; i < SOC_RMT_TX_CANDIDATES_PER_GROUP / 2; i++) {
        TEST_ESP_OK(rmt_del_channel(tx_channels[i]));
    }
    for (int i = 0; i < SOC_RMT_RX_CANDIDATES_PER_GROUP / 2; i++) {
        TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channels[i]));
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, rmt_new_rx_channel(&rx_channel_cfg, &rx_channels[0]));
    for (int i = 0; i < SOC_RMT_RX_CANDIDATES_PER_GROUP / 2; i++) {
        TEST_ESP_OK(rmt_del_channel(rx_channels[i]));
    }

    printf("install tx+rx channels, memory blocks exhaustive\r\n");
    tx_channel_cfg.mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channels[0]));
    tx_channel_cfg.mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL * (SOC_RMT_CHANNELS_PER_GROUP - 2);
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channels[1]));
    rx_channel_cfg.mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channels[0]));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, rmt_new_rx_channel(&rx_channel_cfg, &rx_channels[1]));
    TEST_ESP_OK(rmt_del_channel(tx_channels[0]));
    TEST_ESP_OK(rmt_del_channel(tx_channels[1]));
    TEST_ESP_OK(rmt_del_channel(rx_channels[0]));

#if SOC_RMT_SUPPORT_DMA
    printf("install DMA channel + normal channel\r\n");
    tx_channel_cfg.mem_block_symbols = 1024; // DMA is aimed for transfer large amount of buffers
    tx_channel_cfg.flags.with_dma = true;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channels[0]));
    rx_channel_cfg.flags.with_dma = true;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channels[0]));
    tx_channel_cfg.mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL;
    tx_channel_cfg.flags.with_dma = false;
    rx_channel_cfg.mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL;
    rx_channel_cfg.flags.with_dma = false;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channels[1]));
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channels[1]));
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(rmt_del_channel(tx_channels[i]));
        TEST_ESP_OK(rmt_del_channel(rx_channels[i]));
    }
#endif // SOC_RMT_SUPPORT_DMA
}

TEST_CASE("RMT interrupt priority", "[rmt]")
{
    rmt_tx_channel_config_t tx_channel_cfg = {
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .trans_queue_depth = 4,
        .gpio_num = 0,
        .intr_priority = 3
    };
    // --- Check if specifying interrupt priority works
    printf("install tx channel\r\n");
    rmt_channel_handle_t tx_channel = NULL;
    TEST_ESP_OK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));

    rmt_channel_handle_t another_tx_channel = NULL;
    rmt_tx_channel_config_t another_tx_channel_cfg = tx_channel_cfg;
    // --- Check if rmt interrupt priority valid check works
    another_tx_channel_cfg.intr_priority = 4;
    TEST_ESP_ERR(rmt_new_tx_channel(&another_tx_channel_cfg, &another_tx_channel), ESP_ERR_INVALID_ARG);
    // --- Check if rmt interrupt priority conflict check works
    another_tx_channel_cfg.intr_priority = 1;   ///< Specifying a conflict intr_priority
    TEST_ESP_ERR(rmt_new_tx_channel(&another_tx_channel_cfg, &another_tx_channel), ESP_ERR_INVALID_ARG);
    another_tx_channel_cfg.intr_priority = 0;   ///< Do not specify an intr_priority, should not conflict
    TEST_ESP_OK(rmt_new_tx_channel(&another_tx_channel_cfg, &another_tx_channel));

    rmt_rx_channel_config_t rx_channel_cfg = {
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
        .gpio_num = 0,
        .flags.with_dma = false,    // Interrupt will only be allocated when dma disabled
        .intr_priority = 3,
    };
    // --- Check if specifying interrupt priority works
    printf("install rx channel\r\n");
    rmt_channel_handle_t rx_channel = NULL;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    rmt_channel_handle_t another_rx_channel = NULL;
    rmt_rx_channel_config_t another_rx_channel_cfg = rx_channel_cfg;
    // --- Check if rmt interrupt priority valid check works
    another_rx_channel_cfg.intr_priority = 4;   ///< Specifying a invalid intr_priority
    TEST_ESP_ERR(rmt_new_rx_channel(&another_rx_channel_cfg, &another_rx_channel), ESP_ERR_INVALID_ARG);
    // --- Check if rmt interrupt priority conflict check works
    another_rx_channel_cfg.intr_priority = 1;   ///< Specifying a conflict intr_priority
    TEST_ESP_ERR(rmt_new_rx_channel(&another_rx_channel_cfg, &another_rx_channel), ESP_ERR_INVALID_ARG);
    another_rx_channel_cfg.intr_priority = 0;   ///< Do not specify an intr_priority, should not conflict
    TEST_ESP_OK(rmt_new_rx_channel(&another_rx_channel_cfg, &another_rx_channel));

    TEST_ESP_OK(rmt_del_channel(tx_channel));
    TEST_ESP_OK(rmt_del_channel(another_tx_channel));
    TEST_ESP_OK(rmt_del_channel(rx_channel));
    TEST_ESP_OK(rmt_del_channel(another_rx_channel));
}
