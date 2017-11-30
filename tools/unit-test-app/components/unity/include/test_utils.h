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

/**
 * @brief wait for signals.
 *
 * for multiple devices test cases, DUT might need to wait for other DUTs before continue testing.
 * As all DUTs are independent, need user (or test script) interaction to make test synchronized.
 *
 * Here we provide signal functions for this.
 * For example, we're testing GPIO, DUT1 has one pin connect to with DUT2.
 * DUT2 will output high level and then DUT1 will read input.
 * DUT1 should call `unity_wait_for_signal("output high level");` before it reads input.
 * DUT2 should call `unity_send_signal("output high level");` after it finished setting output high level.
 * According to the console logs:
 *
 * DUT1 console:
 *
 * ```
 *     Waiting for signal: [output high level]!
 *     Please press "Enter" key to once any board send this signal.
 * ```
 *
 * DUT2 console:
 *
 * ```
 *     Send signal: [output high level]!
 * ```
 *
 * Then we press Enter key on DUT1's console, DUT1 starts to read input and then test success.
 *
 * @param signal_name signal name which DUT expected to wait before proceed testing
 */
void unity_wait_for_signal(const char* signal_name);

/**
 * @brief DUT send signal.
 *
 * @param signal_name signal name which DUT send once it finished preparing.
 */
void unity_send_signal(const char* signal_name);
