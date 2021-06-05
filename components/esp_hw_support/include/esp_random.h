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
 * The hardware RNG produces true random numbers under any of the following conditions:
 *
 * - An RF subsystem is running (i.e. Bluetooth or WiFi is enabled)
 * - An internal entropy source has been enabled by calling bootloader_random_enable()
 *   and not yet disabled by calling bootloader_random_disable()
 * - While the ESP-IDF bootloader is running (due to the internal entropy source being enabled
 *   for the duration of bootloader execution).
 *
 * If none of the above conditions are true, the hardware RNG will produce pseudo-random numbers only.
 *
 * When the hardware RNG is producing true random numbers, external entropy (noise samples) are
 * continuously mixed into the internal hardware RNG state. Consult the SoC Technical Reference Manual
 * for more details.
 *
 * This function automatically busy-waits to ensure enough external entropy has been
 * introduced into the hardware RNG state, before returning a new random number.
 *
 * If generating random numbers from an app which has not yet enabled Bluetooth or Wi-Fi, call the
 * API function bootloader_random_enable() before generating random numbers and then call
 * bootloader_random_disable() before using any APIs for Bluetooth, Wi-Fi, ADC, or I2S. Consult the
 * bootloader_random.h header for more details.
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
