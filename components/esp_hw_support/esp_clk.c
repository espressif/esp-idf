/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"
#include "esp_rom_sys.h"
#include "esp_private/esp_clk.h"
#include "hal/clk_tree_ll.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/rtc.h"
#include "esp32/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#include "esp32s2/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#include "esp32s3/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#include "esp32c3/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/rtc.h"
#include "esp32c2/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/rtc.h"
#include "esp32c6/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C61    //TODO: IDF-9526, refactor this
#include "esp32c61/rom/rtc.h"
#include "esp32c61/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/rtc.h"
#include "esp32h2/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/rtc.h"
#include "esp32p4/rtc.h"
#endif

#define MHZ (1000000)

// g_ticks_us defined in ROMs for PRO and APP CPU
extern uint32_t g_ticks_per_us_pro;

static portMUX_TYPE s_esp_rtc_time_lock = portMUX_INITIALIZER_UNLOCKED;

#if SOC_RTC_MEM_SUPPORTED
typedef struct {
    uint64_t rtc_time_us;
    uint64_t rtc_last_ticks;
    uint32_t reserve;
    uint32_t checksum;
} retain_mem_t;
_Static_assert(sizeof(retain_mem_t) == 24, "retain_mem_t must be 24 bytes");
_Static_assert(offsetof(retain_mem_t, checksum) == sizeof(retain_mem_t) - sizeof(uint32_t), "Wrong offset for checksum field in retain_mem_t structure");

static __attribute__((section(".rtc_timer_data_in_rtc_mem"))) retain_mem_t s_rtc_timer_retain_mem;

static uint32_t calc_checksum(void)
{
    uint32_t checksum = 0;
    uint32_t *data = (uint32_t*) &s_rtc_timer_retain_mem;

    for (uint32_t i = 0; i < (sizeof(retain_mem_t) - sizeof(s_rtc_timer_retain_mem.checksum)) / 4; i++) {
        checksum = ((checksum << 5) - checksum) ^ data[i];
    }
    return checksum;
}
#define IS_RETAIN_MEM_VALID() (s_rtc_timer_retain_mem.checksum == calc_checksum())
#endif // SOC_RTC_MEM_SUPPORTED

inline static int IRAM_ATTR s_get_cpu_freq_mhz(void)
{
#if ESP_ROM_GET_CLK_FREQ
    return esp_rom_get_cpu_ticks_per_us();
#else
    return g_ticks_per_us_pro;
#endif
}

int IRAM_ATTR esp_clk_cpu_freq(void)
{
    return s_get_cpu_freq_mhz() * MHZ;
}

int IRAM_ATTR esp_clk_apb_freq(void)
{
    // TODO: IDF-5173 Require cleanup, implementation should be unified
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32P4 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61
    return rtc_clk_apb_freq_get();
#else
    return MIN(s_get_cpu_freq_mhz() * MHZ, APB_CLK_FREQ);
#endif
}

int IRAM_ATTR esp_clk_xtal_freq(void)
{
    return rtc_clk_xtal_freq_get() * MHZ;
}

uint64_t esp_rtc_get_time_us(void)
{
    portENTER_CRITICAL_SAFE(&s_esp_rtc_time_lock);
    const uint32_t cal = esp_clk_slowclk_cal_get();
#if SOC_RTC_MEM_SUPPORTED
    static bool first_call = true;
    if (cal == 0 || (first_call && !IS_RETAIN_MEM_VALID())) {
        /*
            If cal is 0, then this is the first power-up. Cal is keeping valid
            after reboot and deepsleep. If s_rtc_timer_retain_mem is invalid, it means
            that something unexpected happened (the structure was moved around
            after OTA update). To keep the system time valid even after OTA we
            reset the s_rtc_timer_retain_mem. But the resetting can also lead to some
            drift of the system time, because only the last current calibration
            value will be applied to all rtc ticks. To mitigate this effect you
            might need updating of the system time (via SNTP).
        */
        memset(&s_rtc_timer_retain_mem, 0, sizeof(retain_mem_t));
    }
    first_call = false;
    const uint64_t rtc_this_ticks = rtc_time_get();
    const uint64_t ticks = rtc_this_ticks - s_rtc_timer_retain_mem.rtc_last_ticks;
#else
    const uint64_t ticks = rtc_time_get();
#endif
    /* RTC counter result is up to 2^48, calibration factor is up to 2^24,
     * for a 32kHz clock. We need to calculate (assuming no overflow):
     *   (ticks * cal) >> RTC_CLK_CAL_FRACT
     *
     * An overflow in the (ticks * cal) multiplication would cause time to
     * wrap around after approximately 13 days, which is probably not enough
     * for some applications.
     * Therefore multiplication is split into two terms, for the lower 32-bit
     * and the upper 16-bit parts of "ticks", i.e.:
     *   ((ticks_low + 2^32 * ticks_high) * cal) >> RTC_CLK_CAL_FRACT
     */
    const uint64_t ticks_low = ticks & UINT32_MAX;
    const uint64_t ticks_high = ticks >> 32;
    const uint64_t delta_time_us = ((ticks_low * cal) >> RTC_CLK_CAL_FRACT) +
                                   ((ticks_high * cal) << (32 - RTC_CLK_CAL_FRACT));
#if SOC_RTC_MEM_SUPPORTED
    s_rtc_timer_retain_mem.rtc_time_us += delta_time_us;
    s_rtc_timer_retain_mem.rtc_last_ticks = rtc_this_ticks;
    s_rtc_timer_retain_mem.checksum = calc_checksum();
    uint64_t esp_rtc_time_us = s_rtc_timer_retain_mem.rtc_time_us;
    portEXIT_CRITICAL_SAFE(&s_esp_rtc_time_lock);
    return esp_rtc_time_us;
#else
    uint64_t esp_rtc_time_us = delta_time_us + clk_ll_rtc_slow_load_rtc_fix_us();
    portEXIT_CRITICAL_SAFE(&s_esp_rtc_time_lock);
    return esp_rtc_time_us;
#endif
}

void esp_clk_slowclk_cal_set(uint32_t new_cal)
{
#if defined(CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER)
    /* To force monotonic time values even when clock calibration value changes,
     * we adjust esp_rtc_time
     */
#if SOC_RTC_MEM_SUPPORTED
    esp_rtc_get_time_us();
#else
    portENTER_CRITICAL_SAFE(&s_esp_rtc_time_lock);
    uint32_t old_cal = clk_ll_rtc_slow_load_cal();
    if (old_cal != 0) {
        /**
         * The logic of time correction is:
         * old_rtc_us = ticks * old_cal >> RTC_CLK_CAL_FRACT + old_fix_us
         * new_rtc_us = ticks * new_cal >> RTC_CLK_CAL_FRACT + new_fix_us
         *
         * Keep "old_rtc_us == new_rtc_us" to make time monotonically increasing,
         * then we can get new_fix_us:
         * new_fix_us = (ticks * old_cal >> RTC_CLK_CAL_FRACT + old_fix_us) - (ticks * new_cal >> RTC_CLK_CAL_FRACT)
         */
        uint64_t ticks = rtc_time_get();
        const uint64_t ticks_low = ticks & UINT32_MAX;
        const uint64_t ticks_high = ticks >> 32;
        uint64_t old_fix_us = clk_ll_rtc_slow_load_rtc_fix_us();
        uint64_t new_fix_us;

        old_fix_us += ((ticks_low * old_cal) >> RTC_CLK_CAL_FRACT) + ((ticks_high * old_cal) << (32 - RTC_CLK_CAL_FRACT));
        new_fix_us = ((ticks_low * new_cal) >> RTC_CLK_CAL_FRACT) + ((ticks_high * new_cal) << (32 - RTC_CLK_CAL_FRACT));
        new_fix_us = old_fix_us - new_fix_us;
        clk_ll_rtc_slow_store_rtc_fix_us(new_fix_us);
    }
    portEXIT_CRITICAL_SAFE(&s_esp_rtc_time_lock);
#endif // SOC_RTC_MEM_SUPPORTED
#endif // CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
    clk_ll_rtc_slow_store_cal(new_cal);
}

uint32_t esp_clk_slowclk_cal_get(void)
{
    return clk_ll_rtc_slow_load_cal();
}

uint64_t esp_clk_rtc_time(void)
{
#ifdef CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
    return esp_rtc_get_time_us();
#else
    return 0;
#endif
}

void esp_clk_private_lock(void)
{
    portENTER_CRITICAL(&s_esp_rtc_time_lock);
}

void esp_clk_private_unlock(void)
{
    portEXIT_CRITICAL(&s_esp_rtc_time_lock);
}
