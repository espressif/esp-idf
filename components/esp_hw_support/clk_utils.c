/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_log.h"
#include "soc/rtc.h"
#include "hal/mspi_ll.h"
#include "hal/clk_tree_ll.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_private/esp_clk_utils.h"

// Not directly divide to avoid truncation issue
// DIG-498
#if CONFIG_IDF_TARGET_ESP32P4
#define BELOW_FREQ_THRESHOLD(freq) ((freq) < CONFIG_SPIRAM_SPEED)
#elif CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61 || CONFIG_IDF_TARGET_ESP32H4
#define BELOW_FREQ_THRESHOLD(freq) ((freq) * 8 < CONFIG_SPIRAM_SPEED)
#elif CONFIG_IDF_TARGET_ESP32S31
#define BELOW_FREQ_THRESHOLD(freq) ((freq) * 4 < CONFIG_SPIRAM_SPEED * 3)
#endif

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
void esp_clk_utils_mspi_speed_mode_sync_before_cpu_freq_switching(uint32_t target_cpu_src_freq, uint32_t target_cpu_freq)
{
#if MSPI_TIMING_LL_FLASH_CPU_CLK_SRC_BINDED
    (void) target_cpu_freq;
    /* For ESP32S3, the clock source of MSPI is same as the CPU. When CPU use XTAL as clock source, we need to sync the
     * MSPI speed mode. */
    if (target_cpu_src_freq <= clk_ll_xtal_load_freq_mhz()) {
        mspi_timing_change_speed_mode_cache_safe(true);
    }
#elif MSPI_TIMING_LL_PSRAM_FREQ_AXI_CONSTRAINED && CONFIG_SPIRAM
    /* On chips with AXI bus, currently there is a restriction that AXI frequency (usually equals to a portion of CPU
     * frequency) needs to be greater than or equal to MSPI PSRAM frequency to avoid writing MSPI FIFO overflow.
     */
    if (BELOW_FREQ_THRESHOLD(target_cpu_freq)) {
        // Before switching to low speed mode, verify CPU frequency meets the constraint
        assert(target_cpu_freq >= mspi_timing_get_psram_low_speed_freq_mhz());
        mspi_timing_change_speed_mode_cache_safe(true);
    }
#else
    (void) target_cpu_src_freq;
    (void) target_cpu_freq;
#endif
}

void esp_clk_utils_mspi_speed_mode_sync_after_cpu_freq_switching(uint32_t target_cpu_src_freq, uint32_t target_cpu_freq)
{
#if MSPI_TIMING_LL_FLASH_CPU_CLK_SRC_BINDED
    (void) target_cpu_freq;
    if (target_cpu_src_freq > clk_ll_xtal_load_freq_mhz()) {
        mspi_timing_change_speed_mode_cache_safe(false);
    }
#elif MSPI_TIMING_LL_PSRAM_FREQ_AXI_CONSTRAINED && CONFIG_SPIRAM
    /* On chips with AXI bus, currently there is a restriction that AXI frequency (usually equals to a portion of CPU
     * frequency) needs to be greater than or equal to MSPI PSRAM frequency to avoid writing MSPI FIFO overflow.
     */
    if (!BELOW_FREQ_THRESHOLD(target_cpu_freq)) {
        mspi_timing_change_speed_mode_cache_safe(false);
    }
#else
    (void) target_cpu_src_freq;
    (void) target_cpu_freq;
#endif
}
#endif
