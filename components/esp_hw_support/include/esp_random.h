// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Get one random 32-bit word from hardware RNG
 *
 * The hardware RNG is fully functional whenever an RF subsystem is running (ie Bluetooth or WiFi is enabled). For
 * random values, call this function after WiFi or Bluetooth are started.
 *
 * If the RF subsystem is not used by the program, the function bootloader_random_enable() can be called to enable an
 * entropy source. bootloader_random_disable() must be called before RF subsystem or I2S peripheral are used. See these functions'
 * documentation for more details.
 *
 * Any time the app is running without an RF subsystem (or bootloader_random) enabled, RNG hardware should be
 * considered a PRNG. A very small amount of entropy is available due to pre-seeding while the IDF
 * bootloader is running, but this should not be relied upon for any use.
 *
 * @return Random value between 0 and UINT32_MAX
 */
uint32_t esp_random(void);

/**
 * @brief Fill a buffer with random bytes from hardware RNG
 *
 * @note This function has the same restrictions regarding available entropy as esp_random()
 *
 * @param buf Pointer to buffer to fill with random numbers.
 * @param len Length of buffer in bytes
 */
void esp_fill_random(void *buf, size_t len);

#ifdef __cplusplus
}
#endif
