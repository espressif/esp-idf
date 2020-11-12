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

#include "hal/cpu_hal.h"
#include "hal/mpu_hal.h"
#include "hal/mpu_types.h"
#include "soc/soc_caps.h"
#include "bootloader_mem.h"
#include "soc/cpu.h"

void bootloader_init_mem(void)
{
    cpu_hal_init_hwloop();

    // protect memory region
    esp_cpu_configure_region_protection();
}
