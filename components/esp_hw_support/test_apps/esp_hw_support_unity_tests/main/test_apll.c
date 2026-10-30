/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "esp_clk_tree.h"
#include "esp_private/esp_clk_tree_common.h"

#define TEST_APLL_FREQ_1_HZ  (80000000)
#define TEST_APLL_FREQ_2_HZ  (100000000)
#define TEST_APLL_FREQ_3_HZ  (120000000)

static void assert_apll_frequency(uint32_t expected_freq_hz)
{
    uint32_t hardware_freq_hz = 0;
    TEST_ESP_OK(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_APLL, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,
                                             &hardware_freq_hz));
    TEST_ASSERT_UINT32_WITHIN(1, expected_freq_hz, hardware_freq_hz);
}

TEST_CASE("APLL fixed resources share one stable frequency", "[APLL]")
{
    uint32_t configured_freq_hz = 0;
    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_1_HZ,
                                             &configured_freq_hz));

    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));
    assert_apll_frequency(configured_freq_hz);
    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));

    uint32_t real_freq_hz = 0;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE,
                 esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_2_HZ, &real_freq_hz));
    TEST_ASSERT_EQUAL_UINT32(configured_freq_hz, real_freq_hz);

    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
}

TEST_CASE("APLL adjustable resource is exclusive", "[APLL]")
{
    esp_clk_tree_src_config_t adjustable_config = {
        .flags.adjustable = true,
    };
    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, &adjustable_config));

    uint32_t real_freq_hz = 0;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));

    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_1_HZ, &real_freq_hz));
    assert_apll_frequency(real_freq_hz);
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, &adjustable_config));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));

    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_2_HZ, &real_freq_hz));
    TEST_ASSERT_NOT_EQUAL(0, real_freq_hz);
    assert_apll_frequency(real_freq_hz);
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
}

TEST_CASE("APLL fixed resources use the first successful configuration", "[APLL]")
{
    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));
    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));

    uint32_t fixed_freq_hz = 0;
    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_1_HZ, &fixed_freq_hz));
    assert_apll_frequency(fixed_freq_hz);

    uint32_t real_freq_hz = 0;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE,
                 esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_2_HZ, &real_freq_hz));
    TEST_ASSERT_EQUAL_UINT32(fixed_freq_hz, real_freq_hz);
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
}

TEST_CASE("APLL idle preconfiguration takes precedence on acquire", "[APLL]")
{
    uint32_t preconfigured_freq_hz = 0;
    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_3_HZ,
                                             &preconfigured_freq_hz));

    esp_clk_tree_apll_info_t apll_info = {};
    TEST_ESP_OK(esp_clk_tree_apll_get_info(&apll_info));
    TEST_ASSERT_TRUE(apll_info.config_valid);
    TEST_ASSERT_EQUAL_UINT32(preconfigured_freq_hz, apll_info.frequency_hz);

    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));
    assert_apll_frequency(preconfigured_freq_hz);
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));

    TEST_ESP_OK(esp_clk_tree_apll_get_info(&apll_info));
    TEST_ASSERT_FALSE(apll_info.config_valid);
    TEST_ASSERT_EQUAL_UINT32(0, apll_info.frequency_hz);
}

TEST_CASE("APLL adjustable request cannot fall back to fixed sharing", "[APLL]")
{
    uint32_t fixed_freq_hz = 0;
    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_1_HZ, &fixed_freq_hz));

    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));
    esp_clk_tree_src_config_t adjustable_config = {
        .flags.adjustable = true,
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE,
                 esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, &adjustable_config));
    assert_apll_frequency(fixed_freq_hz);

    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
}

TEST_CASE("generic APLL acquisition uses fixed mode", "[APLL]")
{
    uint32_t initial_freq_hz = 0;
    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));
    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_1_HZ,
                                             &initial_freq_hz));
    assert_apll_frequency(initial_freq_hz);

    uint32_t real_freq_hz = 0;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE,
                 esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_2_HZ, &real_freq_hz));
    TEST_ASSERT_EQUAL_UINT32(initial_freq_hz, real_freq_hz);

    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE,
                 esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_3_HZ, &real_freq_hz));
    TEST_ASSERT_EQUAL_UINT32(initial_freq_hz, real_freq_hz);

    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
}

TEST_CASE("APLL fixed frequency setting is idempotent", "[APLL]")
{
    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));

    uint32_t configured_freq_hz = 0;
    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_1_HZ,
                                             &configured_freq_hz));

    uint32_t repeated_freq_hz = 0;
    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, TEST_APLL_FREQ_1_HZ,
                                             &repeated_freq_hz));
    TEST_ASSERT_EQUAL_UINT32(configured_freq_hz, repeated_freq_hz);
    assert_apll_frequency(configured_freq_hz);

    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
}

TEST_CASE("APLL ownership mode resets after final release", "[APLL]")
{
    esp_clk_tree_src_config_t adjustable_config = {
        .flags.adjustable = true,
    };
    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, &adjustable_config));
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));

    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));

    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, &adjustable_config));
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
}

TEST_CASE("failed APLL acquisition does not retain a reference", "[APLL]")
{
    esp_clk_tree_src_config_t adjustable_config = {
        .flags.adjustable = true,
    };
    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, &adjustable_config));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, &adjustable_config));

    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
    TEST_ASSERT_FALSE(esp_clk_tree_is_power_on(SOC_ROOT_CIRCUIT_CLK_APLL));

    TEST_ESP_OK(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, NULL));
    TEST_ESP_OK(esp_clk_tree_release_src(SOC_MOD_CLK_APLL));
}
