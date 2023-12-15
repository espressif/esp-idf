/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "unity.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_sd.h"
#include "sdmmc_test_erase_common_sd.h"

static bool do_sanitize_flag;

bool get_sanitize_flag(void)
{
    return do_sanitize_flag;
}

static void test_mmc_sanitize_blocks(sdmmc_card_t* card)
{
    /* MMC trim applies to write blocks */
    if (sdmmc_mmc_can_sanitize(card) != ESP_OK) {
        printf("Card/device do not support sanitize\n");
        return;
    }
    do_sanitize_flag = true;
    sdmmc_test_sd_erase_blocks(card);
    do_sanitize_flag = false;
}

static void do_one_mmc_sanitize_test(int slot, int width, int freq_khz, int ddr)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, ddr);
    sdmmc_test_sd_begin(slot, width, freq_khz, ddr, &card);
    sdmmc_card_print_info(stdout, &card);
    test_mmc_sanitize_blocks(&card);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("sdmmc sanitize, slot 0, 4-bit", "[sdmmc]")
{
    do_one_mmc_sanitize_test(SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("sdmmc sanitize, slot 0, 8-bit", "[sdmmc]")
{
    do_one_mmc_sanitize_test(SLOT_0, 8, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}
