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

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief RISC-V architecture specific extra information
 */
typedef struct {
    uint32_t mstatus;     /* Machine Status */
    uint32_t mtvec;       /* Machine Trap-Vector Base Address */
    uint32_t mcause;      /* Machine Trap Cause */
    uint32_t mtval;       /* Machine Trap Value */
    uint32_t exc_a[8];    /*!< a register set when the exception caused */
} esp_core_dump_summary_extra_info_t;

#ifdef __cplusplus
}
#endif
