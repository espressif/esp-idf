/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "unity.h"
#include "sdmmc_test_begin_end_sd.h"
#if SOC_SDMMC_IO_POWER_EXTERNAL
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif

#if SOC_SDMMC_HOST_SUPPORTED
TEST_CASE("sdmmc_card_init with SDIO configuration test", "[sdmmc]")
{
    sdmmc_card_t card;

    // Test with slot 1, 1-bit width, default frequency
    sdmmc_test_sd_skip_if_board_incompatible(SLOT_1, 1, SDMMC_FREQ_DEFAULT, NO_DDR, NO_EMMC);

    // Print current SDIO configuration for verification
#ifdef CONFIG_SD_ENABLE_SDIO_SUPPORT
    printf("\n[CONFIG] SDIO support is ENABLED\n");
#else
    printf("\n[CONFIG] SDIO support is DISABLED\n");
#endif

    // Initialize using the test framework
    sdmmc_test_sd_begin(SLOT_1, 1, SDMMC_FREQ_DEFAULT, NO_DDR, &card);

    // Print card information to verify card was initialized correctly
    sdmmc_card_print_info(stdout, &card);

    // Verify basic card properties are initialized
    TEST_ASSERT_NOT_EQUAL(0, card.csd.capacity);
    TEST_ASSERT_NOT_EQUAL(0, card.csd.sector_size);

    // Clean up
    sdmmc_test_sd_end(&card);
}
#endif
