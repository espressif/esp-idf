// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

// Utilities for esp-idf unit tests

#include <stdint.h>
#include <esp_partition.h>

/* Return the 'flash_test' custom data partition (type 0x55)
   defined in the custom partition table.
*/
const esp_partition_t *get_test_data_partition();

/**
 * @brief Initialize reference clock
 *
 * Reference clock provides timestamps at constant 1 MHz frequency, even when
 * the APB frequency is changing.
 */
void ref_clock_init();

/**
 * @brief Deinitialize reference clock
 */
void ref_clock_deinit();

/**
 * @brief Get reference clock timestamp
 * @return number of microseconds since the reference clock was initialized
 */
uint64_t ref_clock_get();
