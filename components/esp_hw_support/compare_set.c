/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/compare_set.h"
#include "soc/spinlock.h"
#include "soc/soc_caps.h"

#if __XTENSA__ && SOC_SPIRAM_SUPPORTED

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
#else // __XTENSA__ && SOC_SPIRAM_SUPPORTED

void compare_and_set_extram(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
    compare_and_set_native(addr, compare, set);
}
#endif // endif
