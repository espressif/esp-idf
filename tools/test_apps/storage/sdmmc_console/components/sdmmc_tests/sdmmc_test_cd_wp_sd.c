/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "unity.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_test_cd_wp_common.h"
#include "sdmmc_test_board.h"

TEST_CASE("CD input works in SD mode", "[sdmmc]")
{
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    sdmmc_test_board_get_config_sdmmc(SDMMC_HOST_SLOT_1, &config, &slot_config);
    const int test_gpio = sdmmc_test_board_get_slot_info(SDMMC_HOST_SLOT_1)->unused_pin;
    slot_config.gpio_cd = test_gpio;
    sdmmc_test_board_card_power_set(true);
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));

    sdmmc_test_cd_input(test_gpio, &config);

    TEST_ESP_OK(sdmmc_host_deinit());
    sdmmc_test_board_card_power_set(false);
}

TEST_CASE("WP input works in SD mode", "[sdmmc]")
{
    sdmmc_test_board_card_power_set(true);
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    sdmmc_test_board_get_config_sdmmc(SDMMC_HOST_SLOT_1, &config, &slot_config);
    const int test_gpio = sdmmc_test_board_get_slot_info(SDMMC_HOST_SLOT_1)->unused_pin;
    slot_config.gpio_wp = test_gpio;
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));

    sdmmc_test_wp_input(test_gpio, &config);

    TEST_ESP_OK(sdmmc_host_deinit());
    sdmmc_test_board_card_power_set(false);
}
