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
#include <stdint.h>
#include <xtensa/config/core-isa.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define EPCx_REGISTER_COUNT XCHAL_NUM_INTLEVELS

/**
 * @brief Xtensa architecture specific extra information
 */
typedef struct {
    uint32_t exc_cause;                 /*!< Cause of exception */
    uint32_t exc_vaddr;                 /*!< Virtual address of exception */
    uint32_t exc_a[16];                 /*!< a register set when the exception caused */
    uint32_t epcx[EPCx_REGISTER_COUNT]; /*!< PC register address at exception level(1 to 7) */
    uint8_t epcx_reg_bits;              /*!< Bit mask of available EPCx registers */
} esp_core_dump_summary_extra_info_t;

#ifdef __cplusplus
}
#endif
