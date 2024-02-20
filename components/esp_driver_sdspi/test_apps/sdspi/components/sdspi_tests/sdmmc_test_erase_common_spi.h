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
 * @brief Test erase blocks performance of the card
 *
 * This function writes a buffer to the card, then erase all the buffers.
 * The time taken for each operation is measured, and the throughput is calculated.
 * The process is repeated for different buffer ranges.
 * In this test, data is always written and then erase from the card
 *
 * This test function works both with SDMMC and SDSPI hosts.
 *
 * @param card Pointer to the card object, must be initialized before calling this function.
 */
void sdmmc_test_sd_erase_blocks(sdmmc_card_t* card);

#ifdef __cplusplus
};
#endif
