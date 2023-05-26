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

/**
 * @brief Test card detect functionality
 *
 * Common part of CD test, works with both SDMMC and SDSPI hosts.
 * Called from sdmmc_test_cd_wp_sd.c and sdmmc_test_cd_wp_spi.c.
 *
 * @param gpio_cd_num GPIO number used for the test. Must not be connected to anything on the board.
 * @param config Pointer to the host configuration structure.
 */
void sdmmc_test_cd_input(int gpio_cd_num, const sdmmc_host_t* config);

/**
 * @brief Test write protect functionality
 *
 * Common part of WP test, works with both SDMMC and SDSPI hosts.
 * Called from sdmmc_test_cd_wp_sd.c and sdmmc_test_cd_wp_spi.c.
 *
 * @param gpio_wp_num GPIO number used for the test. Must not be connected to anything on the board.
 * @param config Pointer to the host configuration structure.
 */
void sdmmc_test_wp_input(int gpio_wp_num, const sdmmc_host_t* config);

#ifdef __cplusplus
};
#endif
