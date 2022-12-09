// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
#include "soc/compare_set.h"
#include "soc/spinlock.h"

static spinlock_t global_extram_lock = SPINLOCK_INITIALIZER;

void compare_and_set_extram(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
    uint32_t intlevel, old_value;
    __asm__ __volatile__ ("rsil %0, " XTSTR(XCHAL_EXCM_LEVEL) "\n"
                          : "=r"(intlevel));

	spinlock_acquire(&global_extram_lock, SPINLOCK_WAIT_FOREVER);

    old_value = *addr;
    if (old_value == compare) {
        *addr = *set;
    }

	spinlock_release(&global_extram_lock);

	__asm__ __volatile__ ("memw \n"
						"wsr %0, ps\n"
						:: "r"(intlevel));

    *set = old_value;
}

