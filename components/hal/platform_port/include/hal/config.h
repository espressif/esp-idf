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
 * @brief Macro to access HAL configuration options.
 *
 * This macro is used to access various HAL configuration options defined in config.h
 * It should be used instead of directly accessing the HAL_CONFIG_ prefixed options.
 *
 * @param x The configuration option to access, without the HAL_CONFIG_ prefix.
 */
#define HAL_CONFIG(x) HAL_CONFIG_##x

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

/**
 * @brief Enable this to use constant-time ECDSA signature generation.
 *        This provides enhanced security against timing attacks.
 */
#define HAL_CONFIG_ECDSA_GEN_SIG_CM CONFIG_HAL_ECDSA_GEN_SIG_CM

/**
 * @brief The minimum supported chip revision.
 */
#define HAL_CONFIG_CHIP_SUPPORT_MIN_REV CONFIG_ESP_REV_MIN_FULL

#ifdef __cplusplus
}
#endif
