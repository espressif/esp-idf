/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/parlio_tx.h"
#include "driver/gpio.h"
#include "hal/parlio_ll.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "test_board.h"

static void IRAM_ATTR test_delay_post_cache_disable(void *args)
{
    esp_rom_delay_us(100000);
}

static void test_parlio_tx_cache_safe(void)
{
    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 1,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = TEST_VALID_GPIO, // generate the valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
        },
        .output_clk_freq_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 65535,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };

    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    const size_t buffer_size = 160 * 1000;
    const size_t chunk_size = buffer_size / 4; // 40KB per trunk
    uint8_t *buffer = heap_caps_malloc(buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(buffer);
    for (int i = 0; i < buffer_size; i++) {
        buffer[i] = i;
    }

    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };

    for (int i = 0; i < 20; i++) {
        TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, buffer + (i % 4) * chunk_size, chunk_size * 8, &transmit_config));
    }
    // during the cache disabled period, the parlio tx unit can be scheduled as well
    unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
    free(buffer);

}

TEST_CASE("parlio tx works with cache disabled", "[parlio]")
{
    test_parlio_tx_cache_safe();
}
