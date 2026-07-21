/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t sdmmc_blockdev_calculate_sectors(size_t sector_size, uint64_t addr, size_t data_len,
                                           size_t *out_start_sector_num, size_t *out_num_of_sectors);

#ifdef __cplusplus
}
#endif
