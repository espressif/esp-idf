/*
 * SPDX-FileCopyrightText: 2016-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>
#include "esp_attr.h"
#include "esp_cpu.h"
#include "soc/wdev_reg.h"
#include "esp_private/esp_clk.h"
#include "soc/soc_caps.h"
#include "esp_log.h"

#if !ESP_TEE_BUILD
#include "esp_private/startup_internal.h"
#endif

#include "hal/rtc_timer_hal.h"

#if SOC_RNG_CLOCK_IS_INDEPENDENT
#include "hal/lp_clkrst_ll.h"
#if SOC_RNG_BUF_CHAIN_ENTROPY_SOURCE || SOC_RNG_RTC_TIMER_ENTROPY_SOURCE
#include "hal/rng_ll.h"
#endif
#endif

#if defined CONFIG_IDF_TARGET_ESP32S3
#define APB_CYCLE_WAIT_NUM (1778) /* If APB clock is 80 MHz, the maximum sampling frequency is around 45 KHz*/
                                  /* 45 KHz reading frequency is the maximum we have tested so far on S3 */
#elif defined CONFIG_IDF_TARGET_ESP32C6
#define APB_CYCLE_WAIT_NUM (160 * 16) /* On ESP32C6, we only read one byte at a time, then XOR the value with
                                      * an asynchronous timer (see code below).
                                      * The current value translates to a sampling frequency of around 62.5 KHz
                                      * for reading 8 bit samples, which is the rate at which the RNG was tested,
                                      * plus additional overhead for the calculation, making it slower. */
#elif defined CONFIG_IDF_TARGET_ESP32H2
#define APB_CYCLE_WAIT_NUM (96 * 16) /* Same reasoning as for ESP32C6, but the CPU frequency on ESP32H2 is
                                      * 96MHz instead of 160 MHz */
#elif defined CONFIG_IDF_TARGET_ESP32P4
/* On ESP32P4, the RNG has been tested with around 75 KHz bytes reading frequency */
#define APB_CYCLE_WAIT_NUM (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 14)
#else
#define APB_CYCLE_WAIT_NUM (16)
#endif

#if CONFIG_ESP_BRINGUP_BYPASS_RANDOM_SETTING
static bool s_random_warning_printed = false;
#endif

uint32_t IRAM_ATTR esp_random(void)
{
#if CONFIG_ESP_BRINGUP_BYPASS_RANDOM_SETTING
    if (!s_random_warning_printed) {
        ESP_LOGW("esp_random", "esp_random is not yet supported and will not give proper random values");
        s_random_warning_printed = true;
    }
    // Return a fixed pattern for bringup purposes
    return 0x5A5A5A5A;
#else
    /* The PRNG which implements WDEV_RANDOM register gets 2 bits
     * of extra entropy from a hardware randomness source every APB clock cycle
     * (provided WiFi or BT are enabled). To make sure entropy is not drained
     * faster than it is added, this function needs to wait for at least 16 APB
     * clock cycles after reading previous word. This implementation may actually
     * wait a bit longer due to extra time spent in arithmetic and branch statements.
     *
     * As a (probably unnecessary) precaution to avoid returning the
     * RNG state as-is, the result is XORed with additional
     * WDEV_RND_REG reads while waiting.
     */

    /* This code does not run in a critical section, so CPU frequency switch may
     * happens while this code runs (this will not happen in the current
     * implementation, but possible in the future). However if that happens,
     * the number of cycles spent on frequency switching will certainly be more
     * than the number of cycles we need to wait here.
     */
    uint32_t cpu_to_apb_freq_ratio = esp_clk_cpu_freq() / esp_clk_apb_freq();

    static uint32_t last_ccount = 0;
    uint32_t ccount;
    uint32_t result = 0;
    for (size_t i = 0; i < sizeof(result); i++) {
        do {
            ccount = esp_cpu_get_cycle_count();
            result ^= REG_READ(WDEV_RND_REG);
        } while (ccount - last_ccount < cpu_to_apb_freq_ratio * APB_CYCLE_WAIT_NUM);
#if SOC_RTC_TIMER_SUPPORTED
        uint32_t current_rtc_timer_counter = (rtc_timer_hal_get_cycle_count(0) & 0xFF);
        result ^= (current_rtc_timer_counter << (i * 8));
#endif
    }
    last_ccount = ccount;
    return result ^ REG_READ(WDEV_RND_REG);
#endif // CONFIG_ESP_BRINGUP_BYPASS_RANDOM_SETTING
}

void esp_fill_random(void *buf, size_t len)
{
    assert(buf != NULL);
    uint8_t *buf_bytes = (uint8_t *)buf;
    while (len > 0) {
        uint32_t word = esp_random();
        uint32_t to_copy = MIN(sizeof(word), len);
        memcpy(buf_bytes, &word, to_copy);
        buf_bytes += to_copy;
        len -= to_copy;
    }
}

#if SOC_RNG_CLOCK_IS_INDEPENDENT && !ESP_TEE_BUILD
ESP_SYSTEM_INIT_FN(init_rng, SECONDARY, BIT(0), 102)
{
#if SOC_RNG_BUF_CHAIN_ENTROPY_SOURCE || SOC_RNG_RTC_TIMER_ENTROPY_SOURCE
    rng_ll_enable();
#else
    _lp_clkrst_ll_enable_rng_clock(true);
#endif
    return ESP_OK;
}
#endif
