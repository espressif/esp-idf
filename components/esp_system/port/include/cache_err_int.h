// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

/**
 * @brief initialize cache invalid access interrupt
 *
 * This function enables cache invalid access interrupt source and connects it
 * to interrupt input number. It is called from the startup code.
 *
 * On ESP32, the interrupt input number is ETS_MEMACCESS_ERR_INUM. On other targets
 * it is ETS_CACHEERR_INUM. See soc/soc.h for more information.
 */
void esp_cache_err_int_init(void);


/**
 * @brief get the CPU which caused cache invalid access interrupt. Helper function in
 * panic handling.
 * @return
 *  - PRO_CPU_NUM, if PRO_CPU has caused cache IA interrupt
 *  - APP_CPU_NUM, if APP_CPU has caused cache IA interrupt
 *  - (-1) otherwise
 */
int esp_cache_err_get_cpuid(void);

#ifdef __cplusplus
}
#endif
