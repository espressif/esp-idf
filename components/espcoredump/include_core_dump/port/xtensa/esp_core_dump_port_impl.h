// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
#ifndef ESP_CORE_DUMP_PORT_IMPL_H_
#define ESP_CORE_DUMP_PORT_IMPL_H_

/**
 * @file
 * @brief Core dump port interface implementation for Xtensa boards.
 */
#include "esp_core_dump_types.h"
#include "esp_app_format.h"
/**
 * Included for SET_STACK macro
 */
#include <xtensa/xtruntime.h>
#include <xt_instr_macros.h>

/**
 * @brief Chip ID associated to this implementation.
 */
#if CONFIG_IDF_TARGET_ESP32
    #define COREDUMP_VERSION_CHIP ESP_CHIP_ID_ESP32
#elif CONFIG_IDF_TARGET_ESP32S2
    #define COREDUMP_VERSION_CHIP ESP_CHIP_ID_ESP32S2
#elif CONFIG_IDF_TARGET_ESP32S3
    #define COREDUMP_VERSION_CHIP ESP_CHIP_ID_ESP32S3
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the stack pointer to the address passed as a parameter.
 * @note This function must be inlined.
 *
 * @param new_sp New stack pointer to set in sp register.
 *
 * @return Former stack pointer address (sp register value).
 */
FORCE_INLINE_ATTR void* esp_core_dump_replace_sp(void* new_sp)
{
    void* current_sp = NULL;
    asm volatile ("mov %0, sp \n\t\
                  "
                   : "=&r"(current_sp)
                   :);
    SET_STACK(new_sp);
    return current_sp;
}

#ifdef __cplusplus
}
#endif

#endif
