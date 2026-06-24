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
#include "driver/sdspi_host.h"
#include "unity.h"
#include "sdmmc_test_begin_end_spi.h"

#if CONFIG_SOC_GPSPI_SUPPORTED
TEST_CASE("sdspi_card_init with SDIO configuration test", "[sdspi]")
{
    sdmmc_card_t card;

    // Test with slot 1, default frequency for SPI mode
    sdmmc_test_spi_skip_if_board_incompatible(SLOT_1, SDMMC_FREQ_DEFAULT);

    // Print current SDIO configuration for verification
#ifdef CONFIG_SD_ENABLE_SDIO_SUPPORT
    printf("\n[CONFIG] SDIO support is ENABLED\n");
#else
    printf("\n[CONFIG] SDIO support is DISABLED\n");
#endif

    // Initialize using the SPI test framework
    sdmmc_test_spi_begin(SLOT_1, SDMMC_FREQ_DEFAULT, &card, NULL, NULL, NULL);

    // Print card information to verify card was initialized correctly
    sdmmc_card_print_info(stdout, &card);

    // Verify basic card properties are initialized
    TEST_ASSERT_NOT_EQUAL(0, card.csd.capacity);
    TEST_ASSERT_NOT_EQUAL(0, card.csd.sector_size);

    // Clean up
    sdmmc_test_spi_end(SLOT_1, &card);
}
#endif
