/*
 * SPDX-FileCopyrightText: 2016-2023 Espressif Systems (Shanghai) CO LTD
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

#if SOC_LP_TIMER_SUPPORTED
#include "hal/lp_timer_hal.h"
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
#else
#define APB_CYCLE_WAIT_NUM (16)
#endif

#if CONFIG_IDF_TARGET_ESP32P4
#include "esp_log.h"
static const char *TAG = "hw_random";

uint32_t IRAM_ATTR esp_random(void)
{
    // TODO: IDF-6522
    ESP_EARLY_LOGW(TAG, "esp_random() has not been implemented yet");
    return 0xDEADBEEF;
}
#else // !CONFIG_IDF_TARGET_ESP32P4

uint32_t IRAM_ATTR esp_random(void)
{
    /* The PRNG which implements WDEV_RANDOM register gets 2 bits
     * of extra entropy from a hardware randomness source every APB clock cycle
     * (provided WiFi or BT are enabled). To make sure entropy is not drained
     * faster than it is added, this function needs to wait for at least 16 APB
     * clock cycles after reading previous word. This implementation may actually
     * wait a bit longer due to extra time spent in arithmetic and branch statements.
     *
     * As a (probably unncessary) precaution to avoid returning the
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
#if SOC_LP_TIMER_SUPPORTED
    for (size_t i = 0; i < sizeof(result); i++) {
        do {
            ccount = esp_cpu_get_cycle_count();
            result ^= REG_READ(WDEV_RND_REG);
        } while (ccount - last_ccount < cpu_to_apb_freq_ratio * APB_CYCLE_WAIT_NUM);
        uint32_t current_rtc_timer_counter = (lp_timer_hal_get_cycle_count() & 0xFF);
        result ^= ((result ^ current_rtc_timer_counter) & 0xFF) << (i * 8);
    }
#else
    do {
        ccount = esp_cpu_get_cycle_count();
        result ^= REG_READ(WDEV_RND_REG);
    } while (ccount - last_ccount < cpu_to_apb_freq_ratio * APB_CYCLE_WAIT_NUM);
#endif
    last_ccount = ccount;
    return result ^ REG_READ(WDEV_RND_REG);
}
#endif //!CONFIG_IDF_TARGET_ESP32P4

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
