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

static void test_discard_blocks(sdmmc_card_t* card, int slot)
{
    /* MMC discard applies to write blocks */
    sdmmc_erase_arg_t arg = SDMMC_DISCARD_ARG;
    if (slot == SLOT_0) {
        uint32_t prev_ext_csd = card->ext_csd.rev;
        // overwrite discard_support as not-supported for -ve test
        card->ext_csd.rev = 0;
        TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, sdmmc_erase_sectors(card, 0, 32, arg));
        // restore discard_support
        card->ext_csd.rev = prev_ext_csd;
    } else {
        uint32_t prev_discard_support = card->ssr.discard_support;
        // overwrite discard_support as not-supported for -ve test
        card->ssr.discard_support = 0;
        TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, sdmmc_erase_sectors(card, 0, 32, arg));
        // restore discard_support
        card->ssr.discard_support = prev_discard_support;
    }

    if (sdmmc_can_discard(card) != ESP_OK) {
        printf("Card/device do not support discard\n");
        return;
    }
    sdmmc_test_sd_erase_blocks(card);
}

static void do_one_mmc_discard_test(int slot, int width, int freq_khz, int ddr)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, ddr, IS_EMMC);
    sdmmc_test_sd_begin(slot, width, freq_khz, ddr, &card);
    sdmmc_card_print_info(stdout, &card);
    test_discard_blocks(&card, slot);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("sdmmc discard, slot 0, 4-bit", "[sdmmc]")
{
    do_one_mmc_discard_test(SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("sdmmc discard, slot 0, 8-bit", "[sdmmc]")
{
    do_one_mmc_discard_test(SLOT_0, 8, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("sdmmc discard, slot 1, 4-bit", "[sdmmc]")
{
    do_one_mmc_discard_test(SLOT_1, 4, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}
