/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Returns chip version
 *
 * @return Chip version in format: Major * 100 + Minor
 */
uint32_t efuse_hal_chip_revision(void);

/**
 * @brief Returns major chip version
 */
uint32_t efuse_hal_get_major_chip_version(void);

/**
 * @brief Returns minor chip version
 */
uint32_t efuse_hal_get_minor_chip_version(void);

#ifdef __cplusplus
}
#endif
