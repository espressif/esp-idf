/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_ana_cmpr.h"

TEST_CASE("ana_cmpr unit install/uninstall", "[ana_cmpr]")
{
    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 100,   // Set a wrong unit
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
        .src_chan0_gpio = ana_cmpr_periph[0].pad_gpios[0],
#endif
    };
    /* Allocate a wrong unit */
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ana_cmpr_new_unit(&config, &cmpr));
    /* Reject negative interrupt priority */
    config.unit = 0;
    config.intr_priority = -1;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ana_cmpr_new_unit(&config, &cmpr));
    config.intr_priority = 0;
    /* Allocate a correct unit  */
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
    /* Delete the unit */
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));

    /* Try to set internal reference for a external unit */
    config.ref_src = ANA_CMPR_REF_SRC_EXTERNAL;
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    config.ext_ref_gpio = ana_cmpr_periph[0].pad_gpios[1];
#endif
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));
    TEST_ESP_ERR(ESP_ERR_NOT_ALLOWED, ana_cmpr_set_internal_reference(cmpr, &ref_cfg));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ana_cmpr_del_unit(NULL));
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
}

TEST_CASE("ana_cmpr event callback", "[ana_cmpr]")
{
    uint32_t cnt = 0;
    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = TEST_ANA_CMPR_UNIT_ID,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
        .src_chan0_gpio = ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[0],
        .resample_limit = 3,
#endif
    };
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));

    gpio_num_t src_chan_io = test_init_src_chan_gpio(cmpr, 0, 0);
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

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    ana_cmpr_scan_config_t scan_cfg = {
        .scan_mode = ANA_CMPR_SCAN_MODE_FULL,
        .poll_period_us = 2,
    };
    TEST_ESP_OK(ana_cmpr_set_scan_config(cmpr, &scan_cfg));
#endif

    printf("register ana_cmpr event callbacks\r\n");
    TEST_ESP_OK(ana_cmpr_register_event_callbacks(cmpr, &cbs, &cnt));
    TEST_ESP_OK(ana_cmpr_enable(cmpr));
    cnt = 0;
    for (uint32_t i = 1; i <= 10; i++) {
        gpio_set_level(src_chan_io, i % 2);
        esp_rom_delay_us(100); // must be larger than the debounce time
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
        TEST_ESP_OK(ana_cmpr_trigger_scan(cmpr));
        esp_rom_delay_us(100);
#endif
        // we assume the cross event was triggered already, and the value of cnt should be updated
        TEST_ASSERT_EQUAL_UINT32(i, cnt);
    }
    TEST_ESP_OK(ana_cmpr_disable(cmpr));
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
}

TEST_CASE("ana_cmpr source channel management api", "[ana_cmpr]")
{
    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = TEST_ANA_CMPR_UNIT_ID,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
        .resample_limit = 3,
        .src_chan0_gpio = ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[0],
    };
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    ana_cmpr_src_chan_config_t src_cfg = {
        .gpio_num = ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[1],
        .cross_type = ANA_CMPR_CROSS_POS,
    };
    TEST_ESP_OK(ana_cmpr_add_src_chan(cmpr, 1, &src_cfg));
    TEST_ESP_OK(ana_cmpr_remove_src_chan(cmpr, 1));
    TEST_ESP_OK(ana_cmpr_remove_src_chan(cmpr, 1));
#endif

    ana_cmpr_scan_config_t scan_cfg = {
        .scan_mode = ANA_CMPR_SCAN_MODE_FULL,
        .poll_period_us = 2,
    };
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    TEST_ESP_OK(ana_cmpr_set_scan_config(cmpr, &scan_cfg));
#else
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, ana_cmpr_set_scan_config(cmpr, &scan_cfg));
#endif

    // remove channel 0 is allowed, but enabling without any source channel should fail
    TEST_ESP_OK(ana_cmpr_remove_src_chan(cmpr, 0));
    TEST_ESP_OK(ana_cmpr_remove_src_chan(cmpr, 0));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, ana_cmpr_enable(cmpr));

    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
}

TEST_CASE("ana_cmpr trigger scan and get output level", "[ana_cmpr]")
{
#if ANALOG_CMPR_LL_GET(IP_VERSION) <= 1
    TEST_IGNORE_MESSAGE("not supported on old IP version");
#else
    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = TEST_ANA_CMPR_UNIT_ID,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
        .resample_limit = 3,
        .src_chan0_gpio = ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[0],
    };
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));

    ana_cmpr_src_chan_config_t src_cfg = {
        .cross_type = ANA_CMPR_CROSS_ANY,
    };
    // for test purpose, source channel N uses pad N.
    for (int i = 1; i < ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM); i++) {
        src_cfg.gpio_num = ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[i];
        TEST_ESP_OK(ana_cmpr_add_src_chan(cmpr, i, &src_cfg));
    }

    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    TEST_ESP_OK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

    ana_cmpr_scan_config_t scan_cfg = {
        .scan_mode = ANA_CMPR_SCAN_MODE_FULL,
        .poll_period_us = 2,
    };
    TEST_ESP_OK(ana_cmpr_set_scan_config(cmpr, &scan_cfg));

    TEST_ESP_OK(ana_cmpr_enable(cmpr));

    for (int i = 0; i < ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM); i++) {
        bool out_level = false;
        gpio_output_enable(ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[i]);
        // Set input to low (0V), which is lower than 50% VDD reference
        gpio_set_level(ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[i], 0);
        // Trigger a scan to update the comparison result
        TEST_ESP_OK(ana_cmpr_trigger_scan(cmpr));
        vTaskDelay(pdMS_TO_TICKS(10));
        TEST_ESP_OK(ana_cmpr_get_output_level(cmpr, i, &out_level));
        TEST_ASSERT_EQUAL(false, out_level);

        // Set input to high (3.3V), which is higher than 50% VDD reference
        gpio_set_level(ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[i], 1);
        // Trigger a scan to update the comparison result
        TEST_ESP_OK(ana_cmpr_trigger_scan(cmpr));
        vTaskDelay(pdMS_TO_TICKS(10));
        TEST_ESP_OK(ana_cmpr_get_output_level(cmpr, i, &out_level));
        TEST_ASSERT_EQUAL(true, out_level);
    }

    TEST_ESP_OK(ana_cmpr_disable(cmpr));
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
#endif
}
