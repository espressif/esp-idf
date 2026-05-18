/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_ana_cmpr.h"
#include "unity_test_utils_cache.h"

static inline gpio_num_t test_pad_gpio_num(int pad_gpio)
{
    return static_cast<gpio_num_t>(pad_gpio);
}

typedef struct {
    ana_cmpr_handle_t handle;
    uint32_t count;
    gpio_num_t src_chan_io;
} test_ana_cmpr_data_t;

static void IRAM_ATTR test_ana_cmpr_iram_safety(void *args)
{
    test_ana_cmpr_data_t *data = (test_ana_cmpr_data_t *)args;
    ana_cmpr_internal_ref_config_t ref_cfg = {};
    ref_cfg.ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD;
    ref_cfg.ref_hys_level = ANA_CMPR_REF_HYS_LEVEL0;
    ana_cmpr_set_internal_reference(data->handle, &ref_cfg);
    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 1,
    };
    ana_cmpr_set_debounce(data->handle, &dbc_cfg);
    data->count = 0;

    // generate 10 cross events by toggling the GPIO
    for (int i = 1; i <= 10; i++) {
        gpio_set_level(data->src_chan_io, i % 2);
        esp_rom_delay_us(100);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
        ana_cmpr_trigger_scan(data->handle);
        esp_rom_delay_us(100);
#endif
    }
    ana_cmpr_set_src_chan_cross_type(data->handle, 0, ANA_CMPR_CROSS_POS);
}

TEST_CASE("ana_cmpr works with cache disabled", "[ana_cmpr]")
{
    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {};
    config.unit = TEST_ANA_CMPR_UNIT_ID;
    config.clk_src = ANA_CMPR_CLK_SRC_DEFAULT;
    config.ref_src = ANA_CMPR_REF_SRC_INTERNAL;
    config.cross_type = ANA_CMPR_CROSS_ANY;
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    config.src_chan0_gpio = test_pad_gpio_num(ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[0]);
    config.resample_limit = 3;
#else
    config.src_chan0_gpio = GPIO_NUM_NC;
    config.resample_limit = 0;
#endif
    config.ext_ref_gpio = GPIO_NUM_NC;
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));

    ana_cmpr_internal_ref_config_t ref_cfg = {};
    ref_cfg.ref_volt = ANA_CMPR_REF_VOLT_20_PCT_VDD;
    ref_cfg.ref_hys_level = ANA_CMPR_REF_HYS_LEVEL0;
    TEST_ESP_OK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));
    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10,
    };
    TEST_ESP_OK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    ana_cmpr_scan_config_t scan_cfg = {
        .scan_mode = ANA_CMPR_SCAN_MODE_FULL,
        .poll_period_us = 2,
    };
    TEST_ESP_OK(ana_cmpr_set_scan_config(cmpr, &scan_cfg));
#endif

    test_ana_cmpr_data_t test_data = {
        .handle = cmpr,
        .count = 0,
        .src_chan_io = test_init_src_chan_gpio(cmpr, 0, 0),
    };

    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = test_ana_cmpr_on_cross_callback,
    };
    TEST_ESP_OK(ana_cmpr_register_event_callbacks(cmpr, &cbs, &test_data.count));
    TEST_ESP_OK(ana_cmpr_enable(cmpr));

    unity_utils_run_cache_disable_stub(test_ana_cmpr_iram_safety, &test_data);
    TEST_ASSERT_EQUAL_INT(test_data.count, 10);

    TEST_ESP_OK(ana_cmpr_disable(cmpr));
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
}
