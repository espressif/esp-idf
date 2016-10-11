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
  * @brief     Set the chip to deep-sleep mode.
  *
  *            The device will automatically wake up after the deep-sleep time set
  *            by the users. Upon waking up, the device boots up from user_init.
  *
  * @attention  The parameter time_in_us to be "uint64" is for further development.
  *                  Only the low 32 bits of parameter time_in_us are avalable now.
  *
  * @param     uint64 time_in_us : deep-sleep time, only the low 32bits are avalable now. unit: microsecond
  *
  * @return    null
  */
void system_deep_sleep(uint64_t time_in_us);

/**
 * @brief Default stub to run on wake from deep sleep.
 *
 * Allows for executing code immediately on wake from sleep, before
 * the software bootloader or esp-idf app has started up.
 *
 * This function is weak-linked, so you can implement your own version
 * to run code immediately when the chip wakes from
 * sleep.
 *
 * For example:
 * @code
 * void RTC_IRAM_ATTR esp_wake_deep_sleep(void) {
 *    esp_default_wake_deep_sleep();
 *    // Add additional functionality here
 * }
 *
 * (Implementing this function is not required for normal operation,
 * in the usual case your app will start normally when waking from
 * deep sleep.)
 *
 * esp_wake_deep_sleep() functionality is limited:
 *
 * - Runs immediately on wake, so most of the SoC is freshly reset -
 *   flash is unmapped and hardware is otherwise uninitialised.
 *
 * - Can only call functions implemented in ROM, or marked RTC_IRAM_ATTR.
 *
 * - Static variables marked RTC_DATA_ATTR will have initial values on
 *   cold boot, and maintain these values between sleep/wake cycles.
 *
 * - Read-only data should be marked RTC_RODATA_ATTR. Strings must be
 *   declared as variables also using RTC_RODATA_ATTR, like this:
 *   RTC_RODATA_ATTR const char message[] = "Hello from very early boot!\n";
 *
 * - Any other static memory will not be initialised (either to zero,
 *   or to any predefined value).
 *
 *
 * - If you implement your own stub, the first call the stub makes
     should be to esp_default_wake_deep_sleep().
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

   If you replace esp_wake_deep_sleep() in your program, or use
   esp_set_deep_sleep_wake_stub(), then it is recommended you call
   esp_default_wake_deep_sleep() as the first function in your stub.
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
