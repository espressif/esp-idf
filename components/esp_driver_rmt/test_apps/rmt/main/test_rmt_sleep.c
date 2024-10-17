/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "test_util_rmt_encoders.h"
#include "test_board.h"

typedef struct {
    TaskHandle_t task_to_notify;
    size_t received_symbol_num;
} test_rx_user_data_t;

static bool test_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    test_rx_user_data_t *test_user_data = (test_rx_user_data_t *)user_data;
    rmt_symbol_word_t *remote_codes = edata->received_symbols;
    esp_rom_printf("%u symbols decoded:\r\n", edata->num_symbols);
    for (size_t i = 0; i < edata->num_symbols; i++) {
        esp_rom_printf("{%d:%d},{%d:%d}\r\n", remote_codes[i].level0, remote_codes[i].duration0, remote_codes[i].level1, remote_codes[i].duration1);
    }
    test_user_data->received_symbol_num = edata->num_symbols;
    vTaskNotifyGiveFromISR(test_user_data->task_to_notify, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

/**
 * @brief Test the RMT driver can still work after light sleep
 *
 * @param allow_pd Whether to allow power down the peripheral in light sleep
 */
static void test_rmt_tx_rx_sleep_retention(bool allow_pd)
{
    uint32_t const test_rx_buffer_symbols = 128;
    rmt_symbol_word_t *remote_codes = heap_caps_aligned_calloc(64, test_rx_buffer_symbols, sizeof(rmt_symbol_word_t),
                                                               MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(remote_codes);

    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .gpio_num = TEST_RMT_GPIO_NUM_A,
        .flags.allow_pd = allow_pd,
    };
    printf("install rx channel\r\n");
    rmt_channel_handle_t rx_channel = NULL;
    TEST_ESP_OK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));
    printf("register rx event callbacks\r\n");
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = test_rmt_rx_done_callback,
    };
    test_rx_user_data_t test_user_data = {
        .task_to_notify = xTaskGetCurrentTaskHandle(),
    };
    TEST_ESP_OK(rmt_rx_register_event_callbacks(rx_channel, &cbs, &test_user_data));

    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .mem_block_symbols = SOC_RMT_MEM_WORDS_PER_CHANNEL,
        .trans_queue_depth = 4,
        .gpio_num = TEST_RMT_GPIO_NUM_A,
        .flags.allow_pd = allow_pd,
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

    // Note: don't enable the RMT channel before going to sleep, ensure no power management lock is acquired by RMT

    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 2 seconds\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(2 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up! Let's see if RMT driver can still work...\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_RMT_SUPPORT_SLEEP_RETENTION
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(allow_pd ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);

    // enable both channels, pm lock will be acquired, so no light sleep will even happen during the transaction
    TEST_ESP_OK(rmt_enable(tx_channel));
    TEST_ESP_OK(rmt_enable(rx_channel));

    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 12000000,
    };

    // ready to receive
    TEST_ESP_OK(rmt_receive(rx_channel, remote_codes, test_rx_buffer_symbols * sizeof(rmt_symbol_word_t), &receive_config));
    printf("send NEC frame\r\n");
    TEST_ESP_OK(rmt_transmit(tx_channel, nec_encoder, (uint16_t[]) {
        0x0440, 0x3003 // address, command
    }, 4, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(34, test_user_data.received_symbol_num);

    printf("uninstall the RMT driver\r\n");
    TEST_ESP_OK(rmt_disable(tx_channel));
    TEST_ESP_OK(rmt_disable(rx_channel));
    TEST_ESP_OK(rmt_del_channel(rx_channel));
    TEST_ESP_OK(rmt_del_channel(tx_channel));
    TEST_ESP_OK(rmt_del_encoder(nec_encoder));
    free(remote_codes);
}

TEST_CASE("rmt tx+rx after light sleep", "[rmt]")
{
    test_rmt_tx_rx_sleep_retention(false);
#if SOC_RMT_SUPPORT_SLEEP_RETENTION
    test_rmt_tx_rx_sleep_retention(true);
#endif
}
