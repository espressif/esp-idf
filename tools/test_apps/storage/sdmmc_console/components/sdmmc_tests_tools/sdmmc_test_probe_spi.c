/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "unity.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end.h"

static void do_one_sdspi_probe(int slot, int freq_khz)
{
    sdmmc_card_t card;
    sdmmc_test_spi_skip_if_board_incompatible(slot, freq_khz);
    sdmmc_test_spi_begin(slot, freq_khz, &card);
    sdmmc_card_print_info(stdout, &card);
    uint8_t* buffer = heap_caps_calloc(512, 1, MALLOC_CAP_DMA);
    TEST_ESP_OK(sdmmc_read_sectors(&card, buffer, 0, 1));
    sdmmc_test_spi_end(slot, &card);
}

TEST_CASE("sdspi probe, slot 0", "[sdspi]")
{
    do_one_sdspi_probe(SLOT_0, SDMMC_FREQ_PROBING);
    do_one_sdspi_probe(SLOT_0, SDMMC_FREQ_DEFAULT);
}

TEST_CASE("sdspi probe, slot 0, HS", "[sdspi]")
{
    do_one_sdspi_probe(SLOT_0, SDMMC_FREQ_HIGHSPEED);
}

TEST_CASE("sdspi probe, slot 1", "[sdspi]")
{
    do_one_sdspi_probe(SLOT_1, SDMMC_FREQ_PROBING);
    do_one_sdspi_probe(SLOT_1, SDMMC_FREQ_DEFAULT);
}

TEST_CASE("sdspi probe, slot 1, HS", "[sdspi]")
{
    do_one_sdspi_probe(SLOT_1, SDMMC_FREQ_HIGHSPEED);
}
