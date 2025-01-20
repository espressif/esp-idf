/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <inttypes.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"
#include "hal/clk_tree_ll.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_private/esp_clk_utils.h"

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
void esp_clk_utils_mspi_speed_mode_sync_before_cpu_freq_switching(uint32_t target_cpu_src_freq, uint32_t target_cpu_freq)
{
#if CONFIG_IDF_TARGET_ESP32S3
    (void) target_cpu_freq;
    if (target_cpu_src_freq <= clk_ll_xtal_load_freq_mhz()) {
        mspi_timing_change_speed_mode_cache_safe(true);
    }
#elif CONFIG_IDF_TARGET_ESP32P4
    (void) target_cpu_src_freq;
    /**
     * Workaround for ESP32P4,
     * f_cpu >= f_mspi
     */
    if (((target_cpu_freq) < CONFIG_ESPTOOLPY_FLASHFREQ_VAL)
#if CONFIG_SPIRAM
        || ((target_cpu_freq) < CONFIG_SPIRAM_SPEED)
#endif
    ) {
        mspi_timing_change_speed_mode_cache_safe(true);
    }
#else
    (void) target_cpu_src_freq;
    (void) target_cpu_freq;
#endif
}

void esp_clk_utils_mspi_speed_mode_sync_after_cpu_freq_switching(uint32_t target_cpu_src_freq, uint32_t target_cpu_freq)
{
#if CONFIG_IDF_TARGET_ESP32S3
    (void) target_cpu_freq;
    if (target_cpu_src_freq > clk_ll_xtal_load_freq_mhz()) {
        mspi_timing_change_speed_mode_cache_safe(false);
    }
#elif CONFIG_IDF_TARGET_ESP32P4
    (void) target_cpu_src_freq;
    /**
     * Workaround for ESP32P4,
     * f_cpu >= f_mspi
     */
    if (((target_cpu_freq) >= CONFIG_ESPTOOLPY_FLASHFREQ_VAL)
#if CONFIG_SPIRAM
        && ((target_cpu_freq) >= CONFIG_SPIRAM_SPEED)
#endif
    ) {
        mspi_timing_change_speed_mode_cache_safe(false);
    }
#else
    (void) target_cpu_src_freq;
    (void) target_cpu_freq;
#endif
}
#endif
