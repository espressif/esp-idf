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
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_sd.h"
#include "sdmmc_test_erase_common_sd.h"

static void do_one_sdmmc_erase_test(int slot, int width, int freq_khz, int ddr)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, ddr);
    sdmmc_test_sd_begin(slot, width, freq_khz, ddr, &card);
    sdmmc_card_print_info(stdout, &card);
    sdmmc_test_sd_erase_blocks(&card);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("sdmmc erase, slot 1, 1-bit", "[sdmmc]")
{
    do_one_sdmmc_erase_test(SLOT_1, 1, SDMMC_FREQ_PROBING, NO_DDR);
    do_one_sdmmc_erase_test(SLOT_1, 1, SDMMC_FREQ_DEFAULT, NO_DDR);
    do_one_sdmmc_erase_test(SLOT_1, 1, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("sdmmc erase, slot 1, 4-bit", "[sdmmc]")
{
    do_one_sdmmc_erase_test(SLOT_1, 4, SDMMC_FREQ_PROBING, NO_DDR);
    do_one_sdmmc_erase_test(SLOT_1, 4, SDMMC_FREQ_DEFAULT, NO_DDR);
    do_one_sdmmc_erase_test(SLOT_1, 4, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}
