// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdbool.h>

#include "xtensa/config/core.h"
#include "hal/cpu_hal.h"
#include "hal/mpu_hal.h"
#include "hal/mpu_types.h"
#include "soc/mpu_caps.h"
#include "bootloader_mem.h"
#include "xt_instr_macros.h"
#include "xtensa/config/specreg.h"

static inline void cpu_configure_region_protection(void)
{
    /* Currently, the only supported chips esp32 and esp32s2
     * have the same configuration. Move this to the port layer once
     * more chips with different configurations are supported.
     * 
     * Both chips have the address space divided into 8 regions, 512MB each.
     */
    const int illegal_regions[] = {0, 4, 5, 6, 7}; // 0x00000000, 0x80000000, 0xa0000000, 0xc0000000, 0xe0000000
    for (int i = 0; i < sizeof(illegal_regions) / sizeof(illegal_regions[0]); ++i) {
        mpu_hal_set_region_access(illegal_regions[i], MPU_REGION_ILLEGAL);
    }

    mpu_hal_set_region_access(1, MPU_REGION_RW); // 0x20000000
}

void bootloader_init_mem(void)
{
    cpu_hal_init_hwloop();

    // protect memory region
    cpu_configure_region_protection();
}