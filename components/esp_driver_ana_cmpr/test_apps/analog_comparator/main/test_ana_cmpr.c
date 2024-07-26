/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_ana_cmpr.h"

TEST_CASE("ana_cmpr_unit_install_uninstall", "[ana_cmpr]")
{
    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = SOC_ANA_CMPR_NUM,   // Set a wrong unit
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
    };
    /* Allocate a wrong unit */
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ana_cmpr_new_unit(&config, &cmpr));
    /* Allocate a correct unit  */
    config.unit = 0;
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));
    /* Try to allocate a existed unit */
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, ana_cmpr_new_unit(&config, &cmpr));
    /* Set the internal reference before enable */
    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    TEST_ESP_OK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));
    /* Enable the unit */
    TEST_ESP_OK(ana_cmpr_enable(cmpr));
    /* Set the internal reference after enable */
    ref_cfg.ref_volt = ANA_CMPR_REF_VOLT_30_PCT_VDD;
    TEST_ESP_OK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));
    /* Try tp delete unit after enable */
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, ana_cmpr_del_unit(cmpr));
    /* Disable the unit */
    TEST_ESP_OK(ana_cmpr_disable(cmpr));
    /* Try to delete the unit with a wrong handle */
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ana_cmpr_del_unit((void *)&cmpr));
    /* Delete the unit */
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));

    /* Try to set internal reference for a external unit */
    config.ref_src = ANA_CMPR_REF_SRC_EXTERNAL;
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, ana_cmpr_set_internal_reference(cmpr, &ref_cfg));
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
}

TEST_CASE("ana_cmpr_internal_reference", "[ana_cmpr]")
{
    int src_chan = test_init_src_chan_gpio(TEST_ANA_CMPR_UNIT_ID);

    uint32_t cnt = 0;
    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = TEST_ANA_CMPR_UNIT_ID,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
        .flags.io_loop_back = 1,
    };
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));
    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    TEST_ESP_OK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));
    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10.0,
    };
    TEST_ESP_OK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));
    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = test_ana_cmpr_on_cross_callback,
    };

    TEST_ESP_OK(ana_cmpr_register_event_callbacks(cmpr, &cbs, &cnt));
    TEST_ESP_OK(ana_cmpr_enable(cmpr));
    cnt = 0;
    for (uint32_t i = 1; i <= 10; i++) {
        test_simulate_src_signal(src_chan, i % 2);
        esp_rom_delay_us(100);
        TEST_ASSERT_EQUAL_UINT32(i, cnt);
    }
    TEST_ESP_OK(ana_cmpr_disable(cmpr));
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
}
