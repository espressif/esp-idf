/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sd_protocol_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines for readability */
#define SLOT_0  0
#define SLOT_1  1
#define NO_DDR 0
#define WITH_DDR 1

/* Helper functions to initialize/deinintalize the host (SDMMC/SDSPI) inside the test */

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
