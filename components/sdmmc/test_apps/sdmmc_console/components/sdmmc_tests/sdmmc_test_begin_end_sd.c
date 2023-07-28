/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "unity.h"
#include "sdmmc_test_board.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end.h"
#include "hal/gpio_hal.h"

void sdmmc_test_sd_skip_if_board_incompatible(int slot, int width, int freq_khz, int ddr)
{
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    if (!sdmmc_test_board_has_slot(slot)) {
        TEST_IGNORE_MESSAGE("Board doesn't have the required slot");
    }
    sdmmc_test_board_get_config_sdmmc(slot, &config, &slot_config);
    int board_max_freq_khz = sdmmc_test_board_get_slot_info(slot)->max_freq_khz;
    if (board_max_freq_khz > 0 && board_max_freq_khz < freq_khz) {
        TEST_IGNORE_MESSAGE("Board doesn't support required max_freq_khz");
    }
    if (slot_config.width < width) {
        TEST_IGNORE_MESSAGE("Board doesn't support required bus width");
    }
}
void sdmmc_test_sd_begin(int slot, int width, int freq_khz, int ddr, sdmmc_card_t *out_card) {
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    /* Similar to the checks in sdmmc_test_sd_skip_if_board_incompatible, but
     * we fail the test if we somehow got to this point with an incompatible board.
     */
    if (!sdmmc_test_board_has_slot(slot)) {
        TEST_FAIL_MESSAGE("Board doesn't have the required slot");
    }
    sdmmc_test_board_get_config_sdmmc(slot, &config, &slot_config);
    int board_max_freq_khz = sdmmc_test_board_get_slot_info(slot)->max_freq_khz;
    if (board_max_freq_khz > 0 && board_max_freq_khz < freq_khz) {
        TEST_FAIL_MESSAGE("Board doesn't support required max_freq_khz");
    }
    if (slot_config.width < width) {
        TEST_FAIL_MESSAGE("Board doesn't support required bus width");
    }

    config.max_freq_khz = freq_khz;

    if (width == 1) {
        config.flags = SDMMC_HOST_FLAG_1BIT;
        slot_config.width = 1;
    } else if (width == 4) {
        config.flags = SDMMC_HOST_FLAG_4BIT;
        slot_config.width = 4;
    } else {
        config.flags = SDMMC_HOST_FLAG_8BIT;
        slot_config.width = 8;
        assert(!ddr && "host driver does not support 8-line DDR mode yet");
    }

    /* Note, not checking sdmmc_test_board_slot_is_emmc here,
     * to let us test DDR mode with eMMC breakout boards in SD slots.
     */
    if (ddr) {
        config.flags |= SDMMC_HOST_FLAG_DDR;
    }

    sdmmc_test_board_card_power_set(true);
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(slot, &slot_config));
    TEST_ESP_OK(sdmmc_card_init(&config, out_card));
}

void sdmmc_test_sd_end(sdmmc_card_t *card)
{
    TEST_ESP_OK(sdmmc_host_deinit());

    // Reset all GPIOs to their default states
    int slot = card->host.slot;
    const sdmmc_test_board_slot_info_t *slot_info = sdmmc_test_board_get_slot_info(slot);
    const int pins[] = {
            slot_info->clk,
            slot_info->cmd_mosi,
            slot_info->d0_miso,
            slot_info->d1,
            slot_info->d2,
            slot_info->d3_cs,
            slot_info->d4,
            slot_info->d5,
            slot_info->d6,
            slot_info->d7,
            slot_info->cd,
            slot_info->wp,
    };
    const int num_pins = sizeof(pins) / sizeof(pins[0]);
    // Silence logging in gpio_reset_pin, which logs at INFO level
    esp_log_level_t old_level = esp_log_level_get("gpio");
    esp_log_level_set("gpio", ESP_LOG_WARN);
    for (int i = 0; i < num_pins; i++) {
        if (pins[i] >= 0) {
            gpio_reset_pin(pins[i]);
            gpio_pullup_dis(pins[i]);
        }
    }
    esp_log_level_set("gpio", old_level);

    //Need to reset GPIO first, otherrwise cannot discharge VDD of card completely.
    sdmmc_test_board_card_power_set(false);
}
