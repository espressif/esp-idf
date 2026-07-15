/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_ana_cmpr.h"

TEST_CASE("ana_cmpr unit install/uninstall", "[ana_cmpr]")
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
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ana_cmpr_del_unit(NULL));
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
}

TEST_CASE("ana_cmpr internal reference", "[ana_cmpr]")
{
    int src_chan = test_init_src_chan_gpio();

    uint32_t cnt = 0;
    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 0,
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
        .wait_us = 10,
    };
    TEST_ESP_OK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));
    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = test_ana_cmpr_on_cross_callback,
    };

    TEST_ESP_OK(ana_cmpr_register_event_callbacks(cmpr, &cbs, &cnt));
    TEST_ESP_OK(ana_cmpr_enable(cmpr));
    cnt = 0;
    for (int i = 1; i <= 10; i++) {
        test_simulate_src_signal(src_chan, i % 2);
        esp_rom_delay_us(100);
        TEST_ASSERT(cnt == i);
    }
    TEST_ESP_OK(ana_cmpr_disable(cmpr));
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
}

TEST_CASE("ana_cmpr edge-specific interrupt reports correct cross direction", "[ana_cmpr]")
{
#if !SOC_ANA_CMPR_CAN_DISTINGUISH_EDGE
    TEST_IGNORE_MESSAGE("target cannot distinguish cross direction");
#else
    test_ana_cmpr_edge_cnt_t cnt = {};
    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = test_ana_cmpr_edge_cnt_callback,
    };
    ana_cmpr_internal_ref_config_t ref_cfg = {};
    ref_cfg.ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD;
    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10,
    };

    /* Arm only the rising (POS) interrupt: a real rising transition must be
     * reported as POS, and a following falling transition must not fire at
     * all, since NEG was never armed. */
    {
        ana_cmpr_handle_t cmpr = NULL;
        ana_cmpr_config_t config = {};
        config.unit = 0;
        config.clk_src = ANA_CMPR_CLK_SRC_DEFAULT;
        config.ref_src = ANA_CMPR_REF_SRC_INTERNAL;
        config.cross_type = ANA_CMPR_CROSS_POS;
        TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));

        int src_chan_io = test_init_src_chan_gpio();
        TEST_ESP_OK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));
        TEST_ESP_OK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));
        TEST_ESP_OK(ana_cmpr_register_event_callbacks(cmpr, &cbs, &cnt));
        TEST_ESP_OK(ana_cmpr_enable(cmpr));
        esp_rom_delay_us(1000); // allow the comparator analog block to settle after power-up

        gpio_set_level(src_chan_io, 1); // rising: armed as POS, must fire as POS
        esp_rom_delay_us(1000);
        TEST_ASSERT_EQUAL_UINT32(1, cnt.pos_cnt);
        TEST_ASSERT_EQUAL_UINT32(0, cnt.neg_cnt);

        gpio_set_level(src_chan_io, 0); // falling: NEG never armed, must not fire
        esp_rom_delay_us(1000);
        TEST_ASSERT_EQUAL_UINT32(1, cnt.pos_cnt);
        TEST_ASSERT_EQUAL_UINT32(0, cnt.neg_cnt);

        TEST_ESP_OK(ana_cmpr_disable(cmpr));
        TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
    }

    /* Same as above, mirrored: arm only the falling (NEG) interrupt. */
    {
        cnt.pos_cnt = 0;
        cnt.neg_cnt = 0;
        ana_cmpr_handle_t cmpr = NULL;
        ana_cmpr_config_t config = {};
        config.unit = 0;
        config.clk_src = ANA_CMPR_CLK_SRC_DEFAULT;
        config.ref_src = ANA_CMPR_REF_SRC_INTERNAL;
        config.cross_type = ANA_CMPR_CROSS_NEG;
        TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));

        int src_chan_io = test_init_src_chan_gpio();
        TEST_ESP_OK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));
        TEST_ESP_OK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));
        TEST_ESP_OK(ana_cmpr_register_event_callbacks(cmpr, &cbs, &cnt));
        TEST_ESP_OK(ana_cmpr_enable(cmpr));
        esp_rom_delay_us(1000); // allow the comparator analog block to settle after power-up

        gpio_set_level(src_chan_io, 0); // falling: armed as NEG, must fire as NEG
        esp_rom_delay_us(1000);
        TEST_ASSERT_EQUAL_UINT32(0, cnt.pos_cnt);
        TEST_ASSERT_EQUAL_UINT32(1, cnt.neg_cnt);

        gpio_set_level(src_chan_io, 1); // rising: POS never armed, must not fire
        esp_rom_delay_us(1000);
        TEST_ASSERT_EQUAL_UINT32(0, cnt.pos_cnt);
        TEST_ASSERT_EQUAL_UINT32(1, cnt.neg_cnt);

        TEST_ESP_OK(ana_cmpr_disable(cmpr));
        TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
    }
#endif
}
