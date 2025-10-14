/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_ana_cmpr.h"
#include "esp_private/gpio.h"
#include "esp_attr.h"

bool IRAM_ATTR test_ana_cmpr_on_cross_callback(ana_cmpr_handle_t cmpr, const ana_cmpr_cross_event_data_t *edata, void *user_ctx)
{
    uint32_t *count = (uint32_t *)user_ctx;
    (*count)++;
    return false;
}

int test_init_src_chan_gpio(int unit_id, int init_level)
{
    int src_chan_num = -1;
    TEST_ESP_OK(ana_cmpr_get_gpio(unit_id, ANA_CMPR_SOURCE_CHAN, &src_chan_num));
    TEST_ASSERT(src_chan_num >= 0);
    TEST_ESP_OK(gpio_set_level(src_chan_num, init_level));
    TEST_ESP_OK(gpio_output_enable(src_chan_num));
    return src_chan_num;
}
