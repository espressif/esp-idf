/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>
#include "esp_attr.h"
#include "hal/cpu_hal.h"
#include "soc/wdev_reg.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/clk.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/clk.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/clk.h"
#elif CONFIG_IDF_TARGET_ESP8684
#include "esp_private/esp_clk.h"
#endif

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
    do {
        ccount = cpu_hal_get_cycle_count();
        result ^= REG_READ(WDEV_RND_REG);
    } while (ccount - last_ccount < cpu_to_apb_freq_ratio * 16);
    last_ccount = ccount;
    return result ^ REG_READ(WDEV_RND_REG);
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
