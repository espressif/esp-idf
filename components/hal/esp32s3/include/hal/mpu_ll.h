// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>

#include "soc/soc_caps.h"

#include "xt_instr_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t mpu_ll_id_to_addr(int id)
{
    // vpn - id
    // 0x00000000 = 0
    // 0x20000000 = 1
    // 0x40000000 = 2
    // 0x60000000 = 3
    // 0x80000000 = 4
    // 0xa0000000 = 5
    // 0xc0000000 = 6
    // 0xe0000000 = 7
    return id * SOC_MPU_MIN_REGION_SIZE;
}

static inline void mpu_ll_set_region_rw(uint32_t addr)
{
    WDTLB(0x0, addr); // cached, no allocate
}

static inline void mpu_ll_set_region_rwx(uint32_t addr)
{
    WDTLB(0x2, addr); // bypass cache
}

static inline void mpu_ll_set_region_x(uint32_t addr)
{
    WITLB(0x3, addr); // cached
}

static inline void mpu_ll_set_region_illegal(uint32_t addr)
{
    WITLB(0xF, addr);
    WDTLB(0xF, addr);
}

#ifdef __cplusplus
}
#endif
