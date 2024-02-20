/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_attr.h"
#include "soc/system_reg.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Clear the crosscore interrupt that just occurred on the current core
 */
FORCE_INLINE_ATTR void crosscore_int_ll_clear_interrupt(int core_id)
{
    WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, 0);
}


/**
 * @brief Trigger a crosscore interrupt on the given core
 *
 * @param core_id Core to trigger an interrupt on. Ignored on single core targets.
 */
FORCE_INLINE_ATTR void crosscore_int_ll_trigger_interrupt(int core_id)
{
    WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, SYSTEM_CPU_INTR_FROM_CPU_0);
}


/**
 * @brief Get the state of the crosscore interrupt register for the given core
 *
 * @param core_id Core to get the crosscore interrupt state of. Ignored on single core targets.
 *
 * @return Non zero value if a software interrupt is pending on the given core,
 *         0 if no software interrupt is pending.
 */
FORCE_INLINE_ATTR uint32_t crosscore_int_ll_get_state(int core_id)
{
    return REG_READ(SYSTEM_CPU_INTR_FROM_CPU_0_REG);
}


#ifdef __cplusplus
}
#endif
