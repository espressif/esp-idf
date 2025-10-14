/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdio.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "soc/soc_caps.h"
#include "driver/ana_cmpr.h"
#include "driver/ana_cmpr_etm.h"
#include "driver/gpio.h"

#if CONFIG_IDF_TARGET_ESP32P4
// The pin of unit 0 is not exposed on some ESP32-P4 runner, so test unit 1 by default
#define TEST_ANA_CMPR_UNIT_ID           1
#else
#define TEST_ANA_CMPR_UNIT_ID           0
#endif

#ifdef __cplusplus
extern "C" {
#endif

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
 * @param unit_id   Analog Comparator unit ID
 * @param init_level  Initial level of the GPIO
 *
 * @return
 *      - int   Source channel GPIO number
 */
int test_init_src_chan_gpio(int unit_id, int init_level);

#ifdef __cplusplus
}
#endif
