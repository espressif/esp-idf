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
#if SOC_I2S_SUPPORTS_PDM_RX
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
    config.clk_cfg.flags.clk_src_fixed = !adjustable;
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

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    // expected error < 0.5%
    i2s_test_common_sample_rate(rx_handle, &std_cfg.clk_cfg, 200);
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

static void test_apll_uses_preconfigured_frequency(void)
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

static void test_apll_released_after_calc_failure(void)
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

static void test_apll_kept_when_planning_fails(void)
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
    TEST_ASSERT_EQUAL_UINT32(initial_frequency_hz, retained_frequency_hz);

    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
    TEST_ASSERT_EQUAL_UINT32(retained_frequency_hz, get_apll_frequency());
    TEST_ESP_OK(i2s_del_channel(tx_handle));
}

static void test_apll_released_when_switching_source(void)
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t apll_cfg = create_apll_std_config(true);
    i2s_std_clk_config_t pll_clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(TEST_APLL_SAMPLE_RATE_1_HZ);

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &apll_cfg));
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &pll_clk_cfg));
    TEST_ESP_OK(i2s_del_channel(tx_handle));

    /* Reacquiring the same direction on the same controller detects stale
     * controller ownership left by the APLL-to-PLL transition. */
    tx_handle = NULL;
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &apll_cfg));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
}

static void test_apll_fixed(void)
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = create_apll_std_config(false);
    std_cfg.gpio_cfg = (i2s_std_gpio_config_t)I2S_TEST_MASTER_DEFAULT_PIN;

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

static void test_apll_adjustable(void)
{
    i2s_std_clk_config_t default_clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(TEST_APLL_SAMPLE_RATE_1_HZ);
    i2s_std_clk_config_t zero_clk_cfg = {};
    TEST_ASSERT_FALSE(default_clk_cfg.flags.clk_src_fixed);
    TEST_ASSERT_FALSE(zero_clk_cfg.flags.clk_src_fixed);

    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = create_apll_std_config(true);

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
#if SOC_I2S_HW_VERSION_1
    /* HW v1 rejects two non-duplex channels on one controller before reaching the APLL ownership check. */
    const esp_err_t incompatible_peer_err = ESP_ERR_INVALID_ARG;
#else
    const esp_err_t incompatible_peer_err = ESP_ERR_INVALID_STATE;
#endif
    TEST_ESP_ERR(incompatible_peer_err, i2s_channel_init_std_mode(rx_handle, &std_cfg));

    i2s_std_clk_config_t fixed_clk_cfg = std_cfg.clk_cfg;
    fixed_clk_cfg.flags.clk_src_fixed = true;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2s_channel_reconfig_std_clock(tx_handle, &fixed_clk_cfg));
    TEST_ESP_ERR(incompatible_peer_err, i2s_channel_init_std_mode(rx_handle, &(i2s_std_config_t) {
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

#if SOC_I2S_SUPPORTS_PDM_RX
static void test_apll_pdm_rate_tuning(void)
{
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_pdm_rx_config_t pdm_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(1024000),
        .slot_cfg = I2S_PDM_RX_SLOT_RAW_FMT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = I2S_GPIO_UNUSED,
            .din = I2S_GPIO_UNUSED,
        },
    };
    pdm_cfg.clk_cfg.clk_src = I2S_CLK_SRC_APLL;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_pdm_rx_mode(rx_handle, &pdm_cfg));
    uint32_t frequency_before_tuning_hz = get_apll_frequency();

    i2s_tuning_config_t tune_cfg = {
        .tune_mode = I2S_TUNING_MODE_ADDSUB,
        .tune_mclk_val = TEST_APLL_TUNE_MCLK_HZ,
        .max_delta_mclk = 1000,
        .min_delta_mclk = -1000,
    };
    TEST_ESP_OK(i2s_channel_tune_rate(rx_handle, &tune_cfg, NULL));
    TEST_ASSERT_NOT_EQUAL(frequency_before_tuning_hz, get_apll_frequency());

    TEST_ESP_OK(i2s_del_channel(rx_handle));
}
#endif

TEST_CASE("I2S APLL management rules", "[i2s][apll]")
{
    test_apll_uses_preconfigured_frequency();
    test_apll_released_after_calc_failure();
    test_apll_kept_when_planning_fails();
    test_apll_released_when_switching_source();
    test_apll_fixed();
    test_apll_adjustable();
#if SOC_I2S_SUPPORTS_PDM_RX
    test_apll_pdm_rate_tuning();
#endif
}

static void test_duplex_apll_reconfig_and_tuning(void)
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = create_apll_std_config(true);
    std_cfg.gpio_cfg = (i2s_std_gpio_config_t)I2S_TEST_MASTER_DEFAULT_PIN;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));

    i2s_chan_info_t tx_info = {};
    i2s_chan_info_t rx_info = {};
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_PTR(rx_handle, tx_info.pair_chan);
    TEST_ASSERT_EQUAL_PTR(tx_handle, rx_info.pair_chan);
    TEST_ASSERT_EQUAL(I2S_ROLE_MASTER, tx_info.role);
    TEST_ASSERT_EQUAL(I2S_ROLE_SLAVE, rx_info.role);

    uint32_t initial_frequency_hz = get_apll_frequency();
    i2s_std_clk_config_t new_clk_cfg = std_cfg.clk_cfg;
    new_clk_cfg.sample_rate_hz = TEST_APLL_SAMPLE_RATE_2_HZ;
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &new_clk_cfg));
    uint32_t master_first_frequency_hz = get_apll_frequency();
    TEST_ASSERT_NOT_EQUAL(initial_frequency_hz, master_first_frequency_hz);
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_UINT32(master_first_frequency_hz, tx_info.sclk_hz);
    TEST_ASSERT_EQUAL_UINT32(master_first_frequency_hz, rx_info.sclk_hz);
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &new_clk_cfg));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));

    /* Repeat in the opposite order. The current APLL can still provide the slave MCLK,
     * so only the master reconfiguration retargets APLL and refreshes the slave divider. */
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &std_cfg.clk_cfg));
    uint32_t slave_first_frequency_hz = get_apll_frequency();
    TEST_ASSERT_EQUAL_UINT32(master_first_frequency_hz, slave_first_frequency_hz);
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_UINT32(slave_first_frequency_hz, tx_info.sclk_hz);
    TEST_ASSERT_EQUAL_UINT32(slave_first_frequency_hz, rx_info.sclk_hz);
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));

    /* Reconfigure through the valid 48 kHz / 96 kHz intermediate state.
     * The master MCLK becomes 24.576 MHz, so the lowest APLL frequency with an
     * integer master divider is 49.152 MHz, which also provides the 12.288 MHz slave MCLK. */
    i2s_std_clk_config_t high_rate_clk_cfg = std_cfg.clk_cfg;
    high_rate_clk_cfg.sample_rate_hz = 96000;
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &high_rate_clk_cfg));
    uint32_t mixed_rate_frequency_hz = get_apll_frequency();
    TEST_ASSERT_UINT32_WITHIN(100, 49152000, mixed_rate_frequency_hz);
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_UINT32(mixed_rate_frequency_hz, tx_info.sclk_hz);
    TEST_ASSERT_EQUAL_UINT32(mixed_rate_frequency_hz, rx_info.sclk_hz);

    TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &high_rate_clk_cfg));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));

    /* Restore both sides to 48 kHz for the tuning checks below. */
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &std_cfg.clk_cfg));

    i2s_tuning_config_t tune_cfg = {
        .tune_mode = I2S_TUNING_MODE_ADDSUB,
        .tune_mclk_val = TEST_APLL_TUNE_MCLK_HZ,
        .max_delta_mclk = 1000,
        .min_delta_mclk = -1000,
    };
    uint32_t frequency_before_master_tune_hz = get_apll_frequency();
    TEST_ESP_OK(i2s_channel_tune_rate(tx_handle, &tune_cfg, NULL));
    uint32_t frequency_after_master_tune_hz = get_apll_frequency();
    TEST_ASSERT_NOT_EQUAL(frequency_before_master_tune_hz, frequency_after_master_tune_hz);
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_UINT32(frequency_after_master_tune_hz, tx_info.sclk_hz);
    TEST_ASSERT_EQUAL_UINT32(frequency_after_master_tune_hz, rx_info.sclk_hz);
#if SOC_I2S_HW_VERSION_1
    TEST_ASSERT_EQUAL_UINT32(tx_info.mclk_hz, rx_info.mclk_hz);
#endif

    /* While the other channel is running, the shared APLL is kept and only the master divider changes */
    i2s_tuning_config_t reset_tune_cfg = {
        .tune_mode = I2S_TUNING_MODE_RESET,
    };
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    TEST_ESP_OK(i2s_channel_tune_rate(tx_handle, &reset_tune_cfg, NULL));
    TEST_ASSERT_EQUAL_UINT32(frequency_after_master_tune_hz, get_apll_frequency());
    /* The 11.2896 MHz master MCLK uses fractional division of the current APLL */
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &new_clk_cfg));
    TEST_ASSERT_EQUAL_UINT32(frequency_after_master_tune_hz, get_apll_frequency());
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ASSERT_EQUAL_UINT32(frequency_after_master_tune_hz, tx_info.sclk_hz);
#if SOC_I2S_HW_VERSION_2
    TEST_ASSERT_UINT32_WITHIN(500, 11289600, tx_info.mclk_hz);
#else
    /* HW v1 uses a 6-bit fractional divider denominator, so the quantized MCLK has wider error. */
    TEST_ASSERT_UINT32_WITHIN(5000, 11289600, tx_info.mclk_hz);
#endif
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
    TEST_ASSERT_EQUAL_UINT32(frequency_after_master_tune_hz, get_apll_frequency());
    TEST_ESP_OK(i2s_channel_disable(rx_handle));

    TEST_ESP_OK(i2s_channel_tune_rate(rx_handle, &tune_cfg, NULL));
    TEST_ASSERT_EQUAL_UINT32(frequency_after_master_tune_hz, get_apll_frequency());
#if SOC_I2S_HW_VERSION_1
    /* Tuning the slave also changes the master because HW v1 shares the MCLK divider */
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_UINT32(tx_info.mclk_hz, rx_info.mclk_hz);
#endif

    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ASSERT_EQUAL_UINT32(frequency_after_master_tune_hz, get_apll_frequency());
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_NULL(rx_info.pair_chan);
    TEST_ASSERT_EQUAL(I2S_ROLE_SLAVE, rx_info.role);
    TEST_ESP_OK(i2s_del_channel(rx_handle));

    /* Verify that deleting the slave preserves APLL and HW v1's shared clock source for the surviving RX master. */
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    uint32_t survivor_apll_hz = get_apll_frequency();
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ASSERT_EQUAL_UINT32(survivor_apll_hz, get_apll_frequency());
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_NULL(rx_info.pair_chan);
    TEST_ASSERT_EQUAL(I2S_ROLE_MASTER, rx_info.role);
    i2s_test_check_sample_rate(rx_handle, TEST_APLL_SAMPLE_RATE_1_HZ, 200);
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

#if SOC_I2S_HW_VERSION_2
static void test_duplex_apll_join_rollback(void)
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t rx_cfg = create_apll_std_config(true);
    rx_cfg.gpio_cfg = (i2s_std_gpio_config_t)I2S_TEST_MASTER_DEFAULT_PIN;
    rx_cfg.gpio_cfg.mclk = I2S_GPIO_UNUSED;
    i2s_std_config_t tx_cfg = rx_cfg;
    tx_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;
    tx_cfg.gpio_cfg.mclk = SOC_GPIO_PIN_COUNT;  // invalid GPIO config

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &rx_cfg));
    uint32_t initial_frequency_hz = get_apll_frequency();

    /* The 18.432 MHz TX slave MCLK doesn't fit the 24.576 MHz APLL, so APLL is raised to
     * 3 * 12.288 MHz before the invalid MCLK GPIO fails. */
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, i2s_channel_init_std_mode(tx_handle, &tx_cfg));
    uint32_t frequency_after_failed_join_hz = get_apll_frequency();
    TEST_ASSERT_NOT_EQUAL(initial_frequency_hz, frequency_after_failed_join_hz);
    TEST_ASSERT_UINT32_WITHIN(100, 36864000, frequency_after_failed_join_hz);

    i2s_chan_info_t tx_info = {};
    i2s_chan_info_t rx_info = {};
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_NULL(tx_info.pair_chan);
    TEST_ASSERT_NULL(rx_info.pair_chan);
    TEST_ASSERT_EQUAL(I2S_COMM_MODE_NONE, tx_info.mode);
    TEST_ASSERT_EQUAL(I2S_ROLE_MASTER, tx_info.role);
    TEST_ASSERT_EQUAL_UINT32(frequency_after_failed_join_hz, rx_info.sclk_hz);
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));

    /* Reinitialization proves that the failed TX logical user was rolled back. */
    tx_cfg.gpio_cfg.mclk = I2S_GPIO_UNUSED;
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &tx_cfg));
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_PTR(rx_handle, tx_info.pair_chan);
    TEST_ASSERT_EQUAL_PTR(tx_handle, rx_info.pair_chan);
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));

    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

static void test_duplex_apll_lazy_join(void)
{
    /* The running TX master uses 256 * 48 kHz on a 24.576 MHz APLL. The joining RX slave
     * fits that frequency with 256 or 192, while 384 requires APLL retargeting. */
    const i2s_mclk_multiple_t joining_multiples[] = {
        I2S_MCLK_MULTIPLE_256,
        I2S_MCLK_MULTIPLE_192,
        I2S_MCLK_MULTIPLE_384,
    };

    for (size_t i = 0; i < sizeof(joining_multiples) / sizeof(joining_multiples[0]); i++) {
        i2s_chan_handle_t tx_handle = NULL;
        i2s_chan_handle_t rx_handle = NULL;
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
        i2s_std_config_t tx_cfg = create_apll_std_config(true);
        tx_cfg.gpio_cfg = (i2s_std_gpio_config_t)I2S_TEST_MASTER_DEFAULT_PIN;
        i2s_std_config_t rx_cfg = tx_cfg;
        rx_cfg.clk_cfg.mclk_multiple = joining_multiples[i];

        TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
        TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &tx_cfg));
        TEST_ESP_OK(i2s_channel_enable(tx_handle));
        uint32_t running_frequency_hz = get_apll_frequency();

        TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
        if (joining_multiples[i] <= tx_cfg.clk_cfg.mclk_multiple) {
            TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &rx_cfg));
            TEST_ASSERT_EQUAL_UINT32(running_frequency_hz, get_apll_frequency());
            TEST_ESP_OK(i2s_channel_enable(rx_handle));
            TEST_ESP_OK(i2s_channel_disable(rx_handle));
        } else {
            TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2s_channel_init_std_mode(rx_handle, &rx_cfg));
            i2s_chan_info_t tx_info = {};
            i2s_chan_info_t rx_info = {};
            TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
            TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
            TEST_ASSERT_NULL(tx_info.pair_chan);
            TEST_ASSERT_NULL(rx_info.pair_chan);
            TEST_ASSERT_TRUE(tx_info.is_enabled);
            TEST_ASSERT_EQUAL(I2S_ROLE_MASTER, rx_info.role);
            TEST_ASSERT_EQUAL_UINT32(running_frequency_hz, get_apll_frequency());
        }

        TEST_ESP_OK(i2s_channel_disable(tx_handle));
        TEST_ESP_OK(i2s_del_channel(rx_handle));
        TEST_ESP_OK(i2s_del_channel(tx_handle));
    }

    /* A peer can initialize at a default 16 kHz rate with shared BCLK/WS pins while TX runs at
     * 48 kHz on adjustable APLL (dividing cleanly from the 24.576 MHz APLL), then reconfigure
     * to 48 kHz before enabling. */
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t default_cfg = create_apll_std_config(true);
    default_cfg.gpio_cfg = (i2s_std_gpio_config_t)I2S_TEST_MASTER_DEFAULT_PIN;
    default_cfg.clk_cfg.sample_rate_hz = 16000;
    i2s_std_clk_config_t target_clk_cfg = default_cfg.clk_cfg;
    target_clk_cfg.sample_rate_hz = TEST_APLL_SAMPLE_RATE_1_HZ;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &default_cfg));
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &target_clk_cfg));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    uint32_t running_frequency_hz = get_apll_frequency();

    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &default_cfg));
    TEST_ASSERT_EQUAL_UINT32(running_frequency_hz, get_apll_frequency());
    i2s_chan_info_t rx_info = {};
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_PTR(tx_handle, rx_info.pair_chan);
    TEST_ASSERT_EQUAL(I2S_ROLE_SLAVE, rx_info.role);
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &target_clk_cfg));
    TEST_ASSERT_EQUAL_UINT32(running_frequency_hz, get_apll_frequency());
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
}

static void test_duplex_apll_asymmetric_mclk(void)
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t rx_cfg = create_apll_std_config(true);
    rx_cfg.slot_cfg =
        (i2s_std_slot_config_t)I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT,
                                                                   I2S_SLOT_MODE_STEREO);
    rx_cfg.gpio_cfg = (i2s_std_gpio_config_t)I2S_TEST_MASTER_DEFAULT_PIN;
    i2s_std_config_t tx_cfg = rx_cfg;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &rx_cfg));

    i2s_chan_info_t tx_info = {};
    i2s_chan_info_t rx_info = {};

    /* Raising only the TX slave to 384 gives a ratio of 6. Its 18.432 MHz MCLK doesn't fit
     * the 24.576 MHz APLL, so APLL is raised to the lowest multiple of the 12.288 MHz
     * master MCLK that provides it: 36.864 MHz. */
    tx_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &tx_cfg));
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_PTR(rx_handle, tx_info.pair_chan);
    TEST_ASSERT_EQUAL_PTR(tx_handle, rx_info.pair_chan);
    TEST_ASSERT_EQUAL(I2S_ROLE_SLAVE, tx_info.role);
    TEST_ASSERT_EQUAL(I2S_ROLE_MASTER, rx_info.role);
    TEST_ASSERT_EQUAL_UINT32(tx_info.sclk_hz, rx_info.sclk_hz);
    /* Check different mclk_multiple conditions*/
    TEST_ASSERT_UINT32_WITHIN(100, 36864000, tx_info.sclk_hz);
    TEST_ASSERT_UINT32_WITHIN(100, 18432000, tx_info.mclk_hz);
    TEST_ASSERT_UINT32_WITHIN(100, 12288000, rx_info.mclk_hz);

    /* Tuning the master keeps its divider of 3. The TX slave still fits the scaled APLL. */
    i2s_tuning_config_t tune_cfg = {
        .tune_mode = I2S_TUNING_MODE_ADDSUB,
        .tune_mclk_val = -100,
        .max_delta_mclk = 1000,
        .min_delta_mclk = -1000,
    };
    TEST_ESP_OK(i2s_channel_tune_rate(rx_handle, &tune_cfg, NULL));
    uint32_t tuned_frequency_hz = get_apll_frequency();
    TEST_ASSERT_UINT32_WITHIN(200, 3 * (12288000 - 100), tuned_frequency_hz);
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_UINT32(tuned_frequency_hz, tx_info.sclk_hz);
    TEST_ASSERT_EQUAL_UINT32(tuned_frequency_hz, rx_info.sclk_hz);

    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

static void test_duplex_apll_reconfig_with_running_peer(void)
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = create_apll_std_config(true);
    std_cfg.gpio_cfg = (i2s_std_gpio_config_t)I2S_TEST_MASTER_DEFAULT_PIN;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    uint32_t running_frequency_hz = get_apll_frequency();

    /* The 24.576 MHz APLL can provide a 9.216 MHz slave MCLK, so the master keeps running. */
    i2s_std_clk_config_t slave_clk_cfg = std_cfg.clk_cfg;
    slave_clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_192;
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &slave_clk_cfg));
    TEST_ASSERT_EQUAL_UINT32(running_frequency_hz, get_apll_frequency());
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));

    /* A 18.432 MHz slave MCLK requires a higher APLL frequency, which is rejected while the master runs. */
    slave_clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2s_channel_reconfig_std_clock(rx_handle, &slave_clk_cfg));
    TEST_ASSERT_EQUAL_UINT32(running_frequency_hz, get_apll_frequency());

    /* With the master disabled, APLL is raised to 3 * 12.288 MHz and the master divider is refreshed. */
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &slave_clk_cfg));
    TEST_ASSERT_UINT32_WITHIN(100, 36864000, get_apll_frequency());
    i2s_chan_info_t tx_info = {};
    i2s_chan_info_t rx_info = {};
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_UINT32(tx_info.sclk_hz, rx_info.sclk_hz);
    TEST_ASSERT_UINT32_WITHIN(100, 12288000, tx_info.mclk_hz);
    TEST_ASSERT_UINT32_WITHIN(100, 18432000, rx_info.mclk_hz);
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));

    /* The slave returns to 256 and keeps the 36.864 MHz APLL. While the slave is running, the master
     * reconfiguration keeps this APLL because it is still an integer multiple of the master MCLK. */
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &std_cfg.clk_cfg));
    TEST_ASSERT_UINT32_WITHIN(100, 36864000, get_apll_frequency());
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
    TEST_ASSERT_UINT32_WITHIN(100, 36864000, get_apll_frequency());
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ASSERT_UINT32_WITHIN(100, 12288000, tx_info.mclk_hz);
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));

    TEST_ESP_OK(i2s_del_channel(rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
}

static void test_duplex_apll_min_divider(void)
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = create_apll_std_config(true);
    std_cfg.gpio_cfg = (i2s_std_gpio_config_t)I2S_TEST_MASTER_DEFAULT_PIN;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    TEST_ASSERT_UINT32_WITHIN(100, 24576000, get_apll_frequency());

    /* Keeping the master divider of 2 would give 24.516 MHz, where the 12.288 MHz slave MCLK ratio
     * of 1.995 results in the integer divider 1. APLL is raised to 3 * 12.258 MHz instead. */
    i2s_tuning_config_t tune_cfg = {
        .tune_mode = I2S_TUNING_MODE_ADDSUB,
        .tune_mclk_val = -30000,
        .max_delta_mclk = 0,
        .min_delta_mclk = -30000,
    };
    TEST_ESP_OK(i2s_channel_tune_rate(tx_handle, &tune_cfg, NULL));
    TEST_ASSERT_UINT32_WITHIN(300, 3 * (12288000 - 30000), get_apll_frequency());
    i2s_chan_info_t rx_info = {};
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_UINT32_WITHIN(200, 12288000, rx_info.mclk_hz);

    TEST_ESP_OK(i2s_del_channel(rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
}

static void test_duplex_apll_master_joins_slave(void)
{
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t master_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_chan_config_t slave_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_SLAVE);
    i2s_std_config_t std_cfg = create_apll_std_config(true);
    std_cfg.gpio_cfg = (i2s_std_gpio_config_t)I2S_TEST_MASTER_DEFAULT_PIN;

    /* The explicit RX slave uses BCLK * 8 = 12.288 MHz and selects 2 * 12.288 MHz alone. */
    TEST_ESP_OK(i2s_new_channel(&slave_chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    uint32_t slave_frequency_hz = get_apll_frequency();
    TEST_ASSERT_UINT32_WITHIN(100, 24576000, slave_frequency_hz);

    /* The current APLL is an integer multiple of the 12.288 MHz master MCLK, so it is kept. */
    TEST_ESP_OK(i2s_new_channel(&master_chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    i2s_chan_info_t tx_info = {};
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ASSERT_EQUAL_PTR(rx_handle, tx_info.pair_chan);
    TEST_ASSERT_EQUAL(I2S_ROLE_MASTER, tx_info.role);
    TEST_ASSERT_EQUAL_UINT32(slave_frequency_hz, get_apll_frequency());
    TEST_ESP_OK(i2s_del_channel(tx_handle));

    /* A 18.432 MHz master MCLK can't divide 24.576 MHz by an integer, so the master selects 2 * 18.432 MHz. */
    i2s_std_config_t master_cfg = std_cfg;
    master_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;
    TEST_ESP_OK(i2s_new_channel(&master_chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &master_cfg));
    uint32_t master_frequency_hz = get_apll_frequency();
    TEST_ASSERT_UINT32_WITHIN(100, 36864000, master_frequency_hz);
    i2s_chan_info_t rx_info = {};
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &tx_info));
    TEST_ESP_OK(i2s_channel_get_info(rx_handle, &rx_info));
    TEST_ASSERT_EQUAL_PTR(rx_handle, tx_info.pair_chan);
    TEST_ASSERT_EQUAL_UINT32(master_frequency_hz, tx_info.sclk_hz);
    TEST_ASSERT_EQUAL_UINT32(master_frequency_hz, rx_info.sclk_hz);

    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}
#endif

TEST_CASE("I2S duplex APLL management rules", "[i2s][apll]")
{
    test_duplex_apll_reconfig_and_tuning();
#if SOC_I2S_HW_VERSION_2
    test_duplex_apll_join_rollback();
    test_duplex_apll_lazy_join();
    test_duplex_apll_asymmetric_mclk();
    test_duplex_apll_reconfig_with_running_peer();
    test_duplex_apll_min_divider();
    test_duplex_apll_master_joins_slave();
#endif
}
