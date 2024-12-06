/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "unity.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_sd.h"

#define SDMMC_FREQ_CUSTOM_10M     10000

static void do_one_sdmmc_probe_test(int slot, int width, int freq_khz, int ddr)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, ddr, NO_EMMC);
    sdmmc_test_sd_begin(slot, width, freq_khz, ddr, &card);
    sdmmc_card_print_info(stdout, &card);
    uint8_t* buffer = heap_caps_calloc(512, 1, MALLOC_CAP_DMA);
    TEST_ESP_OK(sdmmc_read_sectors(&card, buffer, 0, 1));
    free(buffer);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("sdmmc probe, slot 0, 1-bit", "[sdmmc]")
{
    do_one_sdmmc_probe_test(SLOT_0, 1, SDMMC_FREQ_PROBING, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_0, 1, SDMMC_FREQ_DEFAULT, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_0, 1, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("sdmmc probe, slot 0, 4-bit", "[sdmmc]")
{
    do_one_sdmmc_probe_test(SLOT_0, 4, SDMMC_FREQ_PROBING, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_0, 4, SDMMC_FREQ_DEFAULT, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("sdmmc probe, slot 0, 4-bit DDR", "[sdmmc]")
{
    do_one_sdmmc_probe_test(SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, WITH_DDR);
}

TEST_CASE("sdmmc probe, slot 0, 8-bit", "[sdmmc]")
{
    do_one_sdmmc_probe_test(SLOT_0, 8, SDMMC_FREQ_PROBING, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_0, 8, SDMMC_FREQ_DEFAULT, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_0, 8, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("sdmmc probe, slot 1, 1-bit", "[sdmmc]")
{
    do_one_sdmmc_probe_test(SLOT_1, 1, SDMMC_FREQ_PROBING, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_1, 1, SDMMC_FREQ_DEFAULT, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_1, 1, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("sdmmc probe, slot 1, 4-bit", "[sdmmc]")
{
    do_one_sdmmc_probe_test(SLOT_1, 4, SDMMC_FREQ_PROBING, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_1, 4, SDMMC_FREQ_DEFAULT, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_1, 4, SDMMC_FREQ_HIGHSPEED, NO_DDR);
    do_one_sdmmc_probe_test(SLOT_1, 4, SDMMC_FREQ_CUSTOM_10M, NO_DDR);
}

TEST_CASE("sdmmc probe, slot 1, 4-bit DDR", "[sdmmc]")
{
    do_one_sdmmc_probe_test(SLOT_1, 4, SDMMC_FREQ_HIGHSPEED, WITH_DDR);
}
