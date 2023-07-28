/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "driver/sdmmc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines for readability */
#define SLOT_0  0
#define SLOT_1  1
#define NO_DDR 0
#define WITH_DDR 1

/* Helper functions to initialize/deinintalize the host (SDMMC/SDSPI) inside the test */


#if SOC_SDMMC_HOST_SUPPORTED
/**
 * @brief Skip the test if the board is incompatible with the given slot, width, frequency and DDR mode
 * This compares the requested values with the information from the board config, and skips the test
 * (basically, exits it using TEST_IGNORE/longjmp) if the board is incompatible.
 * @param slot  Slot index (SLOT_0 or SLOT_1)
 * @param width  Slot width (1, 4 or 8)
 * @param freq_khz  Bus frequency in kHz
 * @param ddr  Whether to use DDR mode (NO_DDR or WITH_DDR)
 */
void sdmmc_test_sd_skip_if_board_incompatible(int slot, int width, int freq_khz, int ddr);

/**
 * @brief Helper function to initialize the SDMMC host and slot for the test using the given settings
 * @param slot  Slot index (SLOT_0 or SLOT_1)
 * @param width  Slot width (1, 4 or 8)
 * @param freq_khz  Bus frequency in kHz
 * @param ddr  Whether to use DDR mode (NO_DDR or WITH_DDR)
 * @param[out] out_card  Output, pointer to the card structure to be filled by this function
 */
void sdmmc_test_sd_begin(int slot, int width, int freq_khz, int ddr, sdmmc_card_t *out_card);

/**
 * @brief Helper function to deinitialize the SDMMC host and slot after the test
 * @param card  Pointer to the card structure to be deinitialized
 */
void sdmmc_test_sd_end(sdmmc_card_t *card);
#endif

/**
 * @brief Skip the test if the board is incompatible with the given slot and frequency, for SPI mode.
 * @see sdmmc_test_sd_skip_if_board_incompatible
 */
void sdmmc_test_spi_skip_if_board_incompatible(int slot, int freq_khz);

/**
 * @brief Helper function to initialize the SDMMC host and slot for the test using the given settings, for SPI mode
 * @see sdmmc_test_sd_begin
 */
void sdmmc_test_spi_begin(int slot, int freq_khz, sdmmc_card_t *out_card);

/**
 * @brief Helper function to deinitialize the SDMMC host and slot after the test, for SPI mode
 * @see sdmmc_test_sd_end
 */
void sdmmc_test_spi_end(int slot, sdmmc_card_t *card);


#ifdef __cplusplus
};
#endif
