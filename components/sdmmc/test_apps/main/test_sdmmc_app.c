/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_fixture.h"
#include "sd_protocol_defs.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_sd.h"
#include "sdmmc_test_rw_common.h"

TEST_GROUP(sdmmc);

TEST_SETUP(sdmmc)
{
}

TEST_TEAR_DOWN(sdmmc)
{
}

TEST(sdmmc, test_multiblock_unaligned_rw)
{
    sdmmc_card_t card;
    int slot = 1;
    int width = 1;
    int freq_khz = SDMMC_FREQ_DEFAULT;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, 0, 0);
    sdmmc_test_sd_begin(slot, width, freq_khz, 0, &card);
    sdmmc_test_rw_unaligned_buffer_multiblock(&card, 4);
    sdmmc_test_sd_end(&card);
}

TEST_GROUP_RUNNER(sdmmc)
{
    RUN_TEST_CASE(sdmmc, test_multiblock_unaligned_rw)
}

void app_main(void)
{
    UNITY_MAIN(sdmmc);
}
