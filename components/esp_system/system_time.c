// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_system.h"
#include "esp_attr.h"

#include "soc/spinlock.h"
#include "soc/rtc.h"

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rtc.h"
#endif

#include "esp_private/startup_internal.h"

// A component in the build should provide strong implementations that make use of
// and actual hardware timer to provide timekeeping functions.
int64_t IRAM_ATTR __attribute__((weak)) esp_system_get_time(void)
{
    int64_t t = 0;
    static spinlock_t s_time_lock = SPINLOCK_INITIALIZER;
    spinlock_acquire(&s_time_lock, SPINLOCK_WAIT_FOREVER);
    t = (esp_rtc_get_time_us() - g_startup_time); 
    spinlock_release(&s_time_lock);
    return t;
}

uint32_t IRAM_ATTR __attribute__((weak)) esp_system_get_time_resolution(void)
{
    return 1000000L / rtc_clk_slow_freq_get_hz();
}