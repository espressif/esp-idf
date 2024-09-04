/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "unity.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_spi.h"
#include "sdmmc_test_rw_common.h"

/* ========== Read/write performance tests, SPI ========== */

static void do_one_sdspi_perf_test(int slot, int freq_khz)
{
    sdmmc_card_t card;
    sdmmc_test_spi_skip_if_board_incompatible(slot, freq_khz);
    sdmmc_test_spi_begin(slot, freq_khz, &card);
    sdmmc_card_print_info(stdout, &card);
    sdmmc_test_rw_performance(&card, NULL);
    sdmmc_test_spi_end(slot, &card);
}

TEST_CASE("sdspi read/write performance, slot 0", "[sdspi]")
{
    do_one_sdspi_perf_test(SLOT_0, SDMMC_FREQ_HIGHSPEED);
}

TEST_CASE("sdspi read/write performance, slot 1", "[sdspi]")
{
    //TODO: IDF-8749
    //here freq should be changed to SDMMC_FREQ_HIGHSPEED after fixing IDF-8749
    do_one_sdspi_perf_test(SLOT_1, SDMMC_FREQ_DEFAULT);
}

/* ========== Read/write tests with offset, SPI ========== */

static void do_one_sdspi_rw_test_with_offset(int slot, int freq_khz)
{
    sdmmc_card_t card;
    sdmmc_test_spi_skip_if_board_incompatible(slot, freq_khz);
    sdmmc_test_spi_begin(slot, freq_khz, &card);
    sdmmc_card_print_info(stdout, &card);
    sdmmc_test_rw_with_offset(&card);
    sdmmc_test_spi_end(slot, &card);
}

TEST_CASE("sdspi read/write performance with offset, slot 0", "[sdspi]")
{
    do_one_sdspi_rw_test_with_offset(SLOT_0, SDMMC_FREQ_HIGHSPEED);
}

TEST_CASE("sdspi read/write performance with offset, slot 1", "[sdspi]")
{
    //TODO: IDF-8749
    //here freq should be changed to SDMMC_FREQ_HIGHSPEED after fixing IDF-8749
    do_one_sdspi_rw_test_with_offset(SLOT_1, SDMMC_FREQ_DEFAULT);
}

/* ========== Read/write tests with unaligned source/destination buffer, SPI ========== */

static void do_one_sdspi_rw_test_unaligned_buffer(int slot, int freq_khz)
{
    sdmmc_card_t card;
    sdmmc_test_spi_skip_if_board_incompatible(slot, freq_khz);
    sdmmc_test_spi_begin(slot, freq_khz, &card);
    sdmmc_card_print_info(stdout, &card);
    sdmmc_test_rw_unaligned_buffer(&card);
    sdmmc_test_spi_end(slot, &card);
}

TEST_CASE("sdspi read/write using unaligned buffer, slot 0", "[sdspi]")
{
    do_one_sdspi_rw_test_unaligned_buffer(SLOT_0, SDMMC_FREQ_DEFAULT);
}

TEST_CASE("sdspi read/write using unaligned buffer, slot 1", "[sdspi]")
{
    do_one_sdspi_rw_test_unaligned_buffer(SLOT_1, SDMMC_FREQ_DEFAULT);
}
