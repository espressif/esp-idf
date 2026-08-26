/*
 * SPDX-FileCopyrightText: 2016-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED

/**
 * @brief Initialize and pre-calculate forced CPU_MAX frequency configuration (private function for spi_flash)
 *
 * This function pre-calculates and stores the forced CPU_MAX frequency configuration
 * based on the given frequency limit. The configuration is computed once during
 * initialization and reused during runtime for better performance.
 *
 * @param limit_freq_mhz Frequency limit in MHz
 * @note This is a private function, only for use by spi_flash component.
 * @note Must be called during initialization before esp_pm_impl_cpu_max_freq_force().
 * @note This function is only available when CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED is enabled and CONFIG_PM_ENABLE is enabled.
 */
void esp_pm_impl_cpu_max_freq_force_init(uint32_t limit_freq_mhz);

/**
 * @brief Force CPU_MAX frequency to pre-configured limit (private function for spi_flash)
 *
 * This function activates the pre-configured forced CPU_MAX frequency limit.
 * When forced, all reads of CPU_MAX frequency will use the pre-configured value
 * instead of the configured value.
 *
 * @note This is a private function, only for use by spi_flash component.
 * @note The forced frequency configuration must be pre-calculated during initialization.
 * @note This function is only available when CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED is enabled and CONFIG_PM_ENABLE is enabled.
 */
void esp_pm_impl_cpu_max_freq_force(void);

/**
 * @brief Unforce CPU_MAX frequency (private function for spi_flash)
 *
 * This function removes the forced CPU_MAX frequency, allowing the configured
 * value to be used again.
 * @note This is a private function, only for use by spi_flash component.
 * @note This function is only available when CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED is enabled and CONFIG_PM_ENABLE is enabled.
 */
void esp_pm_impl_cpu_max_freq_unforce(void);

#endif // CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED

#ifdef __cplusplus
}
#endif
