/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 //This file implements esp_flash_freq_limit_cb and esp_flash_freq_unlimit_cb callbacks to limit and unlimit CPU
 //frequency when encrypted flash writes are performed.

#include "sdkconfig.h"

#include "esp_log.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "soc/chip_revision.h"
#include "soc/rtc.h"
#include "hal/efuse_hal.h"

#include "esp_private/startup_internal.h"
#include "esp_private/pm_impl_freq_limit.h"
#include "esp_private/spi_flash_freq_limit_cbs.h"
#include "esp_private/esp_clk_utils.h"

/**
 * @brief Get the frequency limit for flash encryption lock based on chip revision
 *
 * @return uint32_t Frequency limit (MHz)
 *
 * Logic:
 * - v1.2+: limit to 160MHz
 * - v1.0: limit to 80MHz
 */
static uint32_t IRAM_ATTR get_encrypt_lock_freq_limit(void)
{
    unsigned int chip_revision = efuse_hal_chip_revision();
    if (ESP_CHIP_REV_ABOVE(chip_revision, 102)) {
        return 160;
    } else {
        return 80;
    }
}

#if !CONFIG_PM_ENABLE
/* Saved original frequency for !PM_ENABLE case (0 means no change was made) */
static uint32_t s_saved_freq_mhz = 0;

// Switch CPU frequency with all necessary synchronization (for !PM_ENABLE case)
// Similar to do_switch() but without PM-specific state management
static void IRAM_ATTR esp_pm_impl_switch_cpu_freq(const rtc_cpu_freq_config_t *old_config, const rtc_cpu_freq_config_t *new_config)
{
    if (new_config->freq_mhz != old_config->freq_mhz) {
        //No need to run on_freq_update for ccount, since that's not supported on C5.
#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
        /* Synchronize MSPI speed mode before CPU frequency switching */
        esp_clk_utils_mspi_speed_mode_sync_before_cpu_freq_switching(new_config->source_freq_mhz, new_config->freq_mhz);
#endif

        //Not taking s_time_update_lock since C5 have fixed divider for systimer
        rtc_clk_cpu_freq_set_config_fast(new_config);

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
        /* Synchronize MSPI speed mode after CPU frequency switching */
        esp_clk_utils_mspi_speed_mode_sync_after_cpu_freq_switching(new_config->source_freq_mhz, new_config->freq_mhz);
#endif
    }
}

/**
 * @brief Limit CPU frequency to target frequency (for !PM_ENABLE case only)
 *
 * This function automatically reads the current CPU frequency, saves it,
 * and switches to the target frequency if current frequency is higher.
 * The original frequency can be restored by calling unlimit_cpu_freq().
 *
 * @param target_freq_mhz Target frequency limit in MHz
 * @note limit->unlimit cannot be nested, and this function must not be called concurrently
 * @note This is a private function, only for use by spi_flash component.
 * @note This function is only available when CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED is enabled and CONFIG_PM_ENABLE is not set.
 */
static void IRAM_ATTR limit_cpu_freq(uint32_t target_freq_mhz)
{
    /* PM not enabled, directly switch frequency and save original frequency */
    rtc_cpu_freq_config_t old_config, new_config;
    rtc_clk_cpu_freq_get_config(&old_config);

    if (old_config.freq_mhz > target_freq_mhz) {
        s_saved_freq_mhz = old_config.freq_mhz;
        if (rtc_clk_cpu_freq_mhz_to_config(target_freq_mhz, &new_config)) {
            /* Use PM implementation function to switch frequency with all necessary synchronization */
            esp_pm_impl_switch_cpu_freq(&old_config, &new_config);
            const unsigned wait_clock_stable_us = 10;
            esp_rom_delay_us(wait_clock_stable_us);
        }
    } else {
        s_saved_freq_mhz = 0;  /* No change needed */
    }
}

/**
 * @brief Unlimit CPU frequency, restoring to original frequency (for !PM_ENABLE case only)
 *
 * This function restores the CPU frequency to the value saved by
 * limit_cpu_freq(). If no frequency change was made, this function
 * does nothing.
 *
 * @note limit->unlimit cannot be nested, and this function must not be called concurrently
 * @note This is a private function, only for use by spi_flash component.
 * @note This function is only available when CONFIG_PM_WORKAROUND_FREQ_LIMIT_ENABLED is enabled and CONFIG_PM_ENABLE is not set.
 */
static void IRAM_ATTR unlimit_cpu_freq(void)
{
    /* PM not enabled, restore to original frequency */
    if (s_saved_freq_mhz > 0) {
        rtc_cpu_freq_config_t old_config, new_config;
        rtc_clk_cpu_freq_get_config(&old_config);
        if (rtc_clk_cpu_freq_mhz_to_config(s_saved_freq_mhz, &new_config)) {
            /* Use PM implementation function to switch frequency with all necessary synchronization */
            esp_pm_impl_switch_cpu_freq(&old_config, &new_config);
            const unsigned wait_clock_stable_us = 10;
            esp_rom_delay_us(wait_clock_stable_us);
        }
    }
    /* If s_saved_freq_mhz == 0, no change was made, so no need to restore */
    s_saved_freq_mhz = 0;
}
#endif // !CONFIG_PM_ENABLE

ESP_SYSTEM_INIT_FN(init_pm_flash_freq_limit, SECONDARY, BIT(0), 202)
{
    uint32_t limit_freq_mhz = get_encrypt_lock_freq_limit();
    ESP_EARLY_LOGW("spi_flash", "CPU frequency is set to 240MHz. esp_flash_write_encrypted() will automatically limit CPU frequency to %dMHz during execution.", limit_freq_mhz);
#ifdef CONFIG_PM_ENABLE
    /* Pre-calculate and store forced frequency configuration during initialization.
     * This is done here to avoid runtime calculation overhead in lock/unlock functions.
     */
    esp_pm_impl_cpu_max_freq_force_init(limit_freq_mhz);
#endif
    return ESP_OK;
}

void pm_c5_flash_freq_limit_init_include_func(void)
{
    // Linker hook function, exists to make the linker examine this file
}

void IRAM_ATTR esp_flash_freq_limit_cb(void)
{
    /* Limit the frequency */
#ifdef CONFIG_PM_ENABLE
    /* PM enabled, use force mechanism */
    esp_pm_impl_cpu_max_freq_force();
#else
    /* PM not enabled, use limit mechanism */
    uint32_t limit_freq_mhz = get_encrypt_lock_freq_limit();
    /* Call PM implementation function to limit CPU frequency.
     * This function automatically reads current frequency, saves it, and switches to target frequency.
     */
    limit_cpu_freq(limit_freq_mhz);
#endif
}

void IRAM_ATTR esp_flash_freq_unlimit_cb(void)
{
    /* Restore the frequency */
#ifdef CONFIG_PM_ENABLE
    /* PM enabled, use unforce mechanism */
    esp_pm_impl_cpu_max_freq_unforce();
#else
    /* PM not enabled, use unlimit mechanism */
    /* Call PM implementation function to unlimit CPU frequency.
     * This function restores the original frequency saved by limit_cpu_freq().
     */
    unlimit_cpu_freq();
#endif
}
