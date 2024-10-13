/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_ana_cmpr.h"
#include "hal/gpio_ll.h"
#include "driver/gpio.h"
#include "esp_attr.h"

bool IRAM_ATTR test_ana_cmpr_on_cross_callback(ana_cmpr_handle_t cmpr, const ana_cmpr_cross_event_data_t *edata, void *user_ctx)
{
    uint32_t *count = (uint32_t *)user_ctx;
    (*count)++;
    return false;
}

int test_init_src_chan_gpio(int unit_id)
{
    int src_chan_num = -1;
    TEST_ESP_OK(ana_cmpr_get_gpio(unit_id, ANA_CMPR_SOURCE_CHAN, &src_chan_num));
    TEST_ASSERT(src_chan_num > 0);
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << src_chan_num),
        .pull_down_en = false,
        .pull_up_en = false,
    };
    TEST_ESP_OK(gpio_config(&io_conf));
    TEST_ESP_OK(gpio_set_level(src_chan_num, 0));
    return src_chan_num;
}

void IRAM_ATTR test_simulate_src_signal(int src_chan, uint32_t val)
{
    gpio_set_level(src_chan, val);
}
