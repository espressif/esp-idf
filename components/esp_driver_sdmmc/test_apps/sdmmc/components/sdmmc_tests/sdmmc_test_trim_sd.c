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

static void test_mmc_trim_blocks(sdmmc_card_t* card)
{
    /* MMC trim applies to write blocks */
    sdmmc_erase_arg_t arg = SDMMC_ERASE_ARG;
    uint8_t prev_sec_feature = card->ext_csd.sec_feature;
    // overwrite sec_feature
    card->ext_csd.sec_feature &=  ~(EXT_CSD_SEC_GB_CL_EN);
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, sdmmc_erase_sectors(card, 0, 32, arg));
    // restore sec_feature
    card->ext_csd.sec_feature = prev_sec_feature;
    if (sdmmc_can_trim(card) != ESP_OK) {
        printf("Card/device do not support trim\n");
        return;
    }
    sdmmc_test_sd_erase_blocks(card);
}

static void do_one_mmc_trim_test(int slot, int width, int freq_khz, int ddr)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, ddr, IS_EMMC);
    sdmmc_test_sd_begin(slot, width, freq_khz, ddr, &card);
    sdmmc_card_print_info(stdout, &card);
    test_mmc_trim_blocks(&card);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("sdmmc trim, slot 0, 4-bit", "[sdmmc]")
{
    do_one_mmc_trim_test(SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("sdmmc trim, slot 0, 8-bit", "[sdmmc]")
{
    do_one_mmc_trim_test(SLOT_0, 8, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}
