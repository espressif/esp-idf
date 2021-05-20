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
 * If Wi-Fi or Bluetooth are enabled, this function returns true random numbers. In other
 * situations, if true random numbers are required then consult the ESP-IDF Programming
 * Guide "Random Number Generation" section for necessary prerequisites.
 *
 * This function automatically busy-waits to ensure enough external entropy has been
 * introduced into the hardware RNG state, before returning a new random number. This delay
 * is very short (always less than 100 CPU cycles).
 *
 * @return Random value between 0 and UINT32_MAX
 */
uint32_t esp_random(void);

/**
 * @brief Fill a buffer with random bytes from hardware RNG
 *
 * @note This function is implemented via calls to esp_random(), so the same
 * constraints apply.
 *
 * @param buf Pointer to buffer to fill with random numbers.
 * @param len Length of buffer in bytes
 */
void esp_fill_random(void *buf, size_t len);

#ifdef __cplusplus
}
#endif
