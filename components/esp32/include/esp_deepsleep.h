// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_DEEPSLEEP_H__
#define __ESP_DEEPSLEEP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Deep_Sleep_API Deep Sleep API
  * @brief API for putting device into deep sleep
  */

/** @addtogroup Deep_Sleep_API
  * @{
  */

/**
 * @brief Enter deep-sleep mode
 *
 * The device will automatically wake up after the deep-sleep time
 * Upon waking up, the device calls deep sleep wake stub, and then proceeds
 * to load application.
 *
 * This function does not return.
 *
 * @param time_in_us  deep-sleep time, unit: microsecond
 */
void esp_deep_sleep(uint64_t time_in_us) __attribute__((noreturn));


/**
 * @brief Enter deep-sleep mode
 *
 * Function has been renamed to esp_deep_sleep.
 * This name is deprecated and will be removed in a future version.
 *
 * @param time_in_us  deep-sleep time, unit: microsecond
 */
void system_deep_sleep(uint64_t time_in_us) __attribute__((noreturn, deprecated));

/**
 * @brief Default stub to run on wake from deep sleep.
 *
 * Allows for executing code immediately on wake from sleep, before
 * the software bootloader or ESP-IDF app has started up.
 *
 * This function is weak-linked, so you can implement your own version
 * to run code immediately when the chip wakes from
 * sleep.
 *
 * See docs/deep-sleep-stub.rst for details.
 */
void esp_wake_deep_sleep(void);

/**
 * @brief Function type for stub to run on wake from sleep.
 *
 */
typedef void (*esp_deep_sleep_wake_stub_fn_t)(void);

/**
 * @brief Install a new stub at runtime to run on wake from deep sleep
 *
 * If implementing esp_wake_deep_sleep() then it is not necessary to
 * call this function.
 *
 * However, it is possible to call this function to substitute a
 * different deep sleep stub. Any function used as a deep sleep stub
 * must be marked RTC_IRAM_ATTR, and must obey the same rules given
 * for esp_wake_deep_sleep().
 */
void esp_set_deep_sleep_wake_stub(esp_deep_sleep_wake_stub_fn_t new_stub);

/**
 * @brief Return current wake from deep sleep stub, or NULL if
 * no stub is installed.
 */
esp_deep_sleep_wake_stub_fn_t esp_get_deep_sleep_wake_stub(void);

/* The default esp-idf-provided esp_wake_deep_sleep() stub.

   See docs/deep-sleep-stub.rst for details.
*/
void esp_default_wake_deep_sleep(void);

/**
  * @}
  */


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SYSTEM_H__ */
