/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_ana_cmpr.h"
#include "unity_test_utils.h"
#include "unity_test_utils_cache.h"

typedef struct {
    ana_cmpr_handle_t handle;
    uint32_t count;
    int src_chan;
} test_ana_cmpr_data_t;

/**
 * @brief Global debounce configuration
 * @note  Why it is global?
 *        If we declare a local variable in 'float' type when cache disabled,
 *        it may be allocated in the flash, which can lead crash when trying to access it.
 */
static ana_cmpr_debounce_config_t IRAM_ATTR s_dbc_cfg = {
    .wait_us = 1.0,
};

static void IRAM_ATTR test_ana_cmpr_iram_safety(void *args)
{
    test_ana_cmpr_data_t *data = (test_ana_cmpr_data_t *)args;
    ana_cmpr_intl_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    ana_cmpr_set_intl_reference(data->handle, &ref_cfg);
    ana_cmpr_set_debounce(data->handle, &s_dbc_cfg);
    data->count = 0;
    for (int i = 1; i <= 10; i++) {
        test_simulate_src_signal(data->src_chan, i % 2);
        esp_rom_delay_us(100);
    }
}

TEST_CASE("ana_cmpr_internal_reference_iram_safe", "[ana_cmpr]")
{
    test_ana_cmpr_data_t test_data = {
        .handle = NULL,
        .count = 0,
        .src_chan = -1,
    };
    test_data.src_chan = test_init_src_chan_gpio();

    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = ANA_CMPR_UNIT_0,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .intr_type = ANA_CMPR_INTR_ANY_CROSS,
    };
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));
    test_data.handle = cmpr;
    ana_cmpr_intl_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    TEST_ESP_OK(ana_cmpr_set_intl_reference(cmpr, &ref_cfg));
    TEST_ESP_OK(ana_cmpr_set_debounce(cmpr, &s_dbc_cfg));
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
