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
#include "sdmmc_test_begin_end_spi.h"
#include "sdmmc_test_erase_common_spi.h"

static void do_one_sdspi_erase(int slot, int freq_khz)
{
    sdmmc_card_t card;
    sdmmc_test_spi_skip_if_board_incompatible(slot, freq_khz);
    sdmmc_test_spi_begin(slot, freq_khz, &card);
    sdmmc_card_print_info(stdout, &card);
    sdmmc_test_sd_erase_blocks(&card);
    sdmmc_test_spi_end(slot, &card);
}

TEST_CASE("sdspi erase, slot 1", "[sdspi]")
{
    do_one_sdspi_erase(SLOT_1, SDMMC_FREQ_PROBING);
    do_one_sdspi_erase(SLOT_1, SDMMC_FREQ_DEFAULT);
}
