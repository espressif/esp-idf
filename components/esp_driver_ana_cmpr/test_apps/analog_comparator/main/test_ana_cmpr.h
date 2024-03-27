/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "soc/soc_caps.h"
#include "driver/ana_cmpr.h"

/**
 * @brief Test default on cross callback
 *
 * @param cmpr      Analog Comparator handle
 * @param edata     Event data
 * @param user_ctx  User context, need to input a unint32_t counter
 * @return
 *      - true      Need to yield
 *      - false     Don't need yield
 */
bool test_ana_cmpr_on_cross_callback(ana_cmpr_handle_t cmpr, const ana_cmpr_cross_event_data_t *edata, void *user_ctx);

/**
 * @brief Initialize Analog Comparator source channel GPIO
 *
 * @return
 *      - int   Source channel GPIO number
 */
int test_init_src_chan_gpio(void);

/**
 * @brief Simulate source channel signal
 *
 * @param src_chan  Source channel GPIO number
 * @param val       0 to set low, others to set high
 */
void test_simulate_src_signal(int src_chan, uint32_t val);
