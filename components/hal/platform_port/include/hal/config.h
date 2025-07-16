/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable this to reuse ROM APIs for GPIO operations.
 *        It will save some code size.
 */
#define HAL_CONFIG_GPIO_USE_ROM_API CONFIG_HAL_GPIO_USE_ROM_IMPL

/**
 * @brief Enable this to indicate the target is an FPGA.
 */
#define HAL_CONFIG_EFUSE_ENV_FPGA   CONFIG_IDF_ENV_FPGA

/**
 * @brief When the hardware fails in measuring the XTAL frequency, use this value as a hint.
 */
#define HAL_CONFIG_XTAL_HINT_FREQ_MHZ CONFIG_XTAL_FREQ

/**
 * @brief Enable this to use ROM APIs for SPI Flash operations.
 *        It will save some code size.
 */
#define HAL_CONFIG_SPI_FLASH_USE_ROM_API CONFIG_SPI_FLASH_ROM_IMPL

#ifdef __cplusplus
}
#endif
