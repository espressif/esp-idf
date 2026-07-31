/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "esp_clk_tree.h"
#include "driver/i2s_common.h"
#include "driver/i2s_std.h"
#if SOC_I2S_SUPPORTS_TDM
#include "driver/i2s_tdm.h"
#endif
#if SOC_I2S_SUPPORTS_PDM
#include "driver/i2s_pdm.h"
#endif

#include "../../test_inc/test_i2s.h"

#define TEST_APLL_SAMPLE_RATE_1_HZ  48000
#define TEST_APLL_SAMPLE_RATE_2_HZ  44100
#define TEST_APLL_INVALID_RATE_HZ   1
#define TEST_APLL_TUNE_MCLK_HZ      400
#define TEST_APLL_PRECONFIG_HZ      100000000

static uint32_t get_apll_frequency(void)
{
    uint32_t frequency_hz = 0;
    TEST_ESP_OK(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_APLL, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,
                                             &frequency_hz));
    TEST_ASSERT_NOT_EQUAL(0, frequency_hz);
    return frequency_hz;
}

static i2s_std_config_t create_apll_std_config(bool adjustable)
{
    i2s_std_config_t config = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(TEST_APLL_SAMPLE_RATE_1_HZ),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_GPIO_UNUSED,
            .ws = I2S_GPIO_UNUSED,
            .dout = I2S_GPIO_UNUSED,
            .din = I2S_GPIO_UNUSED,
        },
    };
    config.clk_cfg.clk_src = I2S_CLK_SRC_APLL;
    config.clk_cfg.flags.clk_src_adjustable = adjustable;
    return config;
}

TEST_CASE("I2S APLL maintains accurate sample rates", "[i2s][apll]")
{
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };
    std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_APLL;
    std_cfg.clk_cfg.flags.clk_src_adjustable = true;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    // expected error < 0.2%
    i2s_test_common_sample_rate(rx_handle, &std_cfg.clk_cfg, 500);
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

TEST_CASE("I2S uses a preconfigured APLL frequency", "[i2s][apll]")
{
    const bool adjustable_modes[] = {false, true};
    for (size_t i = 0; i < sizeof(adjustable_modes) / sizeof(adjustable_modes[0]); i++) {
        uint32_t configured_frequency_hz = 0;
        TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_PRECONFIG_HZ,
                                                 &configured_frequency_hz));

        i2s_chan_handle_t tx_handle = NULL;
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
        i2s_std_config_t std_cfg = create_apll_std_config(adjustable_modes[i]);
        TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
        TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
        TEST_ASSERT_EQUAL_UINT32(configured_frequency_hz, get_apll_frequency());
        TEST_ESP_OK(i2s_del_channel(tx_handle));
    }
}

TEST_CASE("I2S releases a newly acquired APLL after clock calculation failure", "[i2s][apll]")
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = create_apll_std_config(true);
    std_cfg.clk_cfg.sample_rate_hz = TEST_APLL_INVALID_RATE_HZ;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_del_channel(tx_handle));

    tx_handle = NULL;
    std_cfg = create_apll_std_config(true);
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
}

TEST_CASE("I2S retains an adjusted APLL frequency after clock calculation failure", "[i2s][apll]")
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = create_apll_std_config(true);

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    uint32_t initial_frequency_hz = get_apll_frequency();

    i2s_std_clk_config_t invalid_clk_cfg = std_cfg.clk_cfg;
    invalid_clk_cfg.sample_rate_hz = TEST_APLL_INVALID_RATE_HZ;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, i2s_channel_reconfig_std_clock(tx_handle, &invalid_clk_cfg));
    uint32_t retained_frequency_hz = get_apll_frequency();
    TEST_ASSERT_NOT_EQUAL(initial_frequency_hz, retained_frequency_hz);

    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
    TEST_ASSERT_NOT_EQUAL(retained_frequency_hz, get_apll_frequency());
    TEST_ESP_OK(i2s_del_channel(tx_handle));
}

TEST_CASE("I2S fixed APLL test", "[i2s][apll]")
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = create_apll_std_config(false);

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    uint32_t fixed_frequency_hz = get_apll_frequency();

    std_cfg.clk_cfg.sample_rate_hz = TEST_APLL_SAMPLE_RATE_2_HZ;
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
    TEST_ASSERT_EQUAL_UINT32(fixed_frequency_hz, get_apll_frequency());

    i2s_tuning_config_t tune_cfg = {
        .tune_mode = I2S_TUNING_MODE_ADDSUB,
        .tune_mclk_val = TEST_APLL_TUNE_MCLK_HZ,
        .max_delta_mclk = 1000,
        .min_delta_mclk = -1000,
    };
    TEST_ESP_OK(i2s_channel_tune_rate(tx_handle, &tune_cfg, NULL));
    TEST_ASSERT_EQUAL_UINT32(fixed_frequency_hz, get_apll_frequency());

    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

TEST_CASE("I2S adjustable APLL test", "[i2s][apll]")
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = create_apll_std_config(true);

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2s_channel_init_std_mode(rx_handle, &std_cfg));

    i2s_std_clk_config_t fixed_clk_cfg = std_cfg.clk_cfg;
    fixed_clk_cfg.flags.clk_src_adjustable = false;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2s_channel_reconfig_std_clock(tx_handle, &fixed_clk_cfg));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2s_channel_init_std_mode(rx_handle, &(i2s_std_config_t) {
        .clk_cfg = fixed_clk_cfg,
        .slot_cfg = std_cfg.slot_cfg,
        .gpio_cfg = std_cfg.gpio_cfg,
    }));

    uint32_t initial_frequency_hz = get_apll_frequency();
    std_cfg.clk_cfg.sample_rate_hz = TEST_APLL_SAMPLE_RATE_2_HZ;
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
    TEST_ASSERT_NOT_EQUAL(initial_frequency_hz, get_apll_frequency());

    i2s_tuning_config_t tune_cfg = {
        .tune_mode = I2S_TUNING_MODE_ADDSUB,
        .tune_mclk_val = TEST_APLL_TUNE_MCLK_HZ,
        .max_delta_mclk = 1000,
        .min_delta_mclk = -1000,
    };
    i2s_tuning_info_t tune_info = {};
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_tune_rate(tx_handle, NULL, &tune_info));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2s_channel_tune_rate(tx_handle, &tune_cfg, NULL));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));

    uint32_t frequency_before_tuning_hz = get_apll_frequency();
    TEST_ESP_OK(i2s_channel_tune_rate(tx_handle, &tune_cfg, &tune_info));
    TEST_ASSERT_NOT_EQUAL(frequency_before_tuning_hz, get_apll_frequency());

    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}
