/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
#define BELOW_FREQ_THRESHOLD(cpu_freq, psram_freq) ((cpu_freq) < (psram_freq))
#elif CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61 || CONFIG_IDF_TARGET_ESP32H4
#define BELOW_FREQ_THRESHOLD(cpu_freq, psram_freq) ((cpu_freq) * 8 < (psram_freq))
#elif CONFIG_IDF_TARGET_ESP32S31
/**
 * PSRAM clock must stay below 4 * AXI, otherwise MSPI write FIFO can overflow.
 * CPU:AXI is not constant across DFS (see rtc_clk_cpu_freq_to_cpll_mhz()):
 * CPU <= 40MHz:   CPU:AXI = 1:1 -> cpu >= spiram / 4
 * CPU 53MHz:      CPU:AXI = 1:1 -> cpu >= spiram / 4
 * CPU 80MHz:      CPU:AXI = 1:1 -> cpu >= spiram / 4
 * CPU 160MHz:     CPU:AXI = 1:2 -> cpu >= spiram * 2 / 4
 * CPU 240/320MHz: CPU:AXI = 1:3 -> cpu >= spiram * 3 / 4
 */
static inline bool BELOW_FREQ_THRESHOLD(uint32_t cpu_freq, uint32_t psram_freq)
{
    // if (cpu_freq >= 240) {
    //     return cpu_freq * 4 < psram_freq * 3; // a.k.a. psram > 320 MHz which is impossible for the psram speed on S31, so this condition is always false
    // }
    // if (cpu_freq >= 160) {
    //     return cpu_freq * 2 < psram_freq; // a.k.a. psram > 320 MHz which is impossible for the psram speed on S31, so this condition is always false
    // }
    // for cpu <= 80 MHz:
    return cpu_freq * 4 < psram_freq; // for the two cases above, this condition is even stronger, so always false
}
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
    if (BELOW_FREQ_THRESHOLD(target_cpu_freq, CONFIG_SPIRAM_SPEED)) {
        // Before switching to low speed mode, verify AXI still meets the constraint at PSRAM low speed
        assert(!BELOW_FREQ_THRESHOLD(target_cpu_freq, mspi_timing_get_psram_low_speed_freq_mhz()));
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
    if (!BELOW_FREQ_THRESHOLD(target_cpu_freq, CONFIG_SPIRAM_SPEED)) {
        mspi_timing_change_speed_mode_cache_safe(false);
    }
#else
    (void) target_cpu_src_freq;
    (void) target_cpu_freq;
#endif
}
#endif
