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

#include "esp_timer.h"

#include "esp_private/esp_timer_private.h"
#include "esp_private/system_internal.h"

#include "sdkconfig.h"

#if defined( CONFIG_ESP32_TIME_SYSCALL_USE_FRC1 ) || \
    defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 ) || \
    defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_FRC1 ) || \
    defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_RTC_FRC1 )
#define WITH_FRC 1
#endif

#if WITH_FRC
void esp_timer_timekeeping_impl_init(void)
{
    // esp_system_get_time here calls the previous system time provider.
    // This should add the time elapsed from g_startup_time up to esp_timer_init,
    // therefore keeping it as the point of reference (g_startup_time, that is).
    esp_timer_private_advance(esp_system_get_time());

    // esp_timer provides microsecond-resolution timers to the system
    esp_system_set_time_provider(esp_timer_get_time, 1);
}
#else
void esp_timer_timekeeping_impl_init(void)
{
    // Do not override default system time provider
}
#endif