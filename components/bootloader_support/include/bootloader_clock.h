// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Configure clocks for early boot
 *
 * Called by bootloader, or by the app if the bootloader version is old (pre v2.1).
 */
void bootloader_clock_configure(void);

/** @brief Return the rated maximum frequency of this chip
 */
int bootloader_clock_get_rated_freq_mhz(void);

#ifdef __cplusplus
}
#endif
