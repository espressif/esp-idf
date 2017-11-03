// Copyright 2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_attr.h"
#include "esp_clk.h"
#include "soc/wdev_reg.h"
#include "freertos/FreeRTOSConfig.h"
#include "xtensa/core-macros.h"

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
        ccount = XTHAL_GET_CCOUNT();
        result ^= REG_READ(WDEV_RND_REG);
    } while (ccount - last_ccount < cpu_to_apb_freq_ratio * 16);
    last_ccount = ccount;
    return result ^ REG_READ(WDEV_RND_REG);
}
