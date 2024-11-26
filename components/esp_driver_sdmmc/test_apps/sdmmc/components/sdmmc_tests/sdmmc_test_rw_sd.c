/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stddef.h>
#include "unity.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_sd.h"
#include "sdmmc_test_rw_common.h"

/* ========== Read/write performance tests, SD ========== */

static void do_one_sdmmc_perf_test(int slot, int width, int freq_khz, int ddr, FILE* perf_log)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, ddr, NO_EMMC);
    sdmmc_test_sd_begin(slot, width, freq_khz, ddr, &card);
    sdmmc_card_print_info(stdout, &card);
    sdmmc_test_rw_performance(&card, perf_log);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("sdmmc read/write performance, slot 0, 4-bit", "[sdmmc]")
{
    do_one_sdmmc_perf_test(SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, NO_DDR, NULL);
}

TEST_CASE("sdmmc read/write performance, slot 0, 4-bit DDR", "[sdmmc]")
{
    do_one_sdmmc_perf_test(SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, WITH_DDR, NULL);
}

TEST_CASE("sdmmc read/write performance, slot 0, 8-bit", "[sdmmc]")
{
    do_one_sdmmc_perf_test(SLOT_0, 8, SDMMC_FREQ_HIGHSPEED, NO_DDR, NULL);
}

TEST_CASE("sdmmc read/write performance, slot 1, 1-bit", "[sdmmc]")
{
    do_one_sdmmc_perf_test(SLOT_1, 1, SDMMC_FREQ_HIGHSPEED, NO_DDR, NULL);
}

TEST_CASE("sdmmc read/write performance, slot 1, 4-bit", "[sdmmc]")
{
    /* Set up in-memory file for collecting performance logs */
    char *perf_log_buf = NULL;
    size_t perf_log_size = 0;
    FILE* perf_log = open_memstream(&perf_log_buf, &perf_log_size);

    /* Run the actual tests */
    do_one_sdmmc_perf_test(SLOT_1, 4, SDMMC_FREQ_HIGHSPEED, NO_DDR, perf_log);

    /* Dump the contents of the performance log to stdout */
    fclose(perf_log);
    fwrite(perf_log_buf, perf_log_size, 1, stdout);
    free(perf_log_buf);
}

TEST_CASE("sdmmc read/write performance, slot 1, 4-bit DDR", "[sdmmc]")
{
    do_one_sdmmc_perf_test(SLOT_1, 4, SDMMC_FREQ_HIGHSPEED, WITH_DDR, NULL);
}

/* ========== Read/write tests with offset, SD ========== */

static void do_one_sdmmc_rw_test_with_offset(int slot, int width, int freq_khz, int ddr)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, ddr, NO_EMMC);
    sdmmc_test_sd_begin(slot, width, freq_khz, ddr, &card);
    sdmmc_card_print_info(stdout, &card);
    sdmmc_test_rw_with_offset(&card);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("sdmmc read/write performance with offset, slot 0, 4-bit", "[sdmmc]")
{
    do_one_sdmmc_rw_test_with_offset(SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, 0);
}

TEST_CASE("sdmmc read/write performance with offset, slot 1, 4-bit", "[sdmmc]")
{
    do_one_sdmmc_rw_test_with_offset(SLOT_1, 4, SDMMC_FREQ_HIGHSPEED, 0);
}

/* ========== Read/write tests with unaligned source/destination buffer, SD ========== */

static void do_one_sdmmc_rw_test_unaligned_buffer(int slot, int width, int freq_khz, int ddr)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, ddr, NO_EMMC);
    sdmmc_test_sd_begin(slot, width, freq_khz, ddr, &card);
    sdmmc_card_print_info(stdout, &card);
    sdmmc_test_rw_unaligned_buffer(&card);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("sdmmc read/write using unaligned buffer, slot 0, 4-bit", "[sdmmc]")
{
    do_one_sdmmc_rw_test_unaligned_buffer(SLOT_0, 4, SDMMC_FREQ_DEFAULT, 0);
}

TEST_CASE("sdmmc read/write using unaligned buffer, slot 1, 4-bit", "[sdmmc]")
{
    do_one_sdmmc_rw_test_unaligned_buffer(SLOT_1, 4, SDMMC_FREQ_DEFAULT, 0);
}
