/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_attr.h"
#include "soc/intpri_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Clear the crosscore interrupt that just occurred on the current core
 */
FORCE_INLINE_ATTR void crosscore_int_ll_clear_interrupt(int core_id)
{
    if (core_id == 0) {
        WRITE_PERI_REG(INTPRI_CPU_INTR_FROM_CPU_0_REG, 0);
    } else {
        WRITE_PERI_REG(INTPRI_CPU_INTR_FROM_CPU_1_REG, 0);
    }
}


/**
 * @brief Trigger a crosscore interrupt on the given core
 *
 * @param core_id Core to trigger an interrupt on. Ignored on single core targets.
 */
FORCE_INLINE_ATTR void crosscore_int_ll_trigger_interrupt(int core_id)
{
    if (core_id == 0) {
        WRITE_PERI_REG(INTPRI_CPU_INTR_FROM_CPU_0_REG, INTPRI_CPU_INTR_FROM_CPU_0);
    } else {
        WRITE_PERI_REG(INTPRI_CPU_INTR_FROM_CPU_1_REG, INTPRI_CPU_INTR_FROM_CPU_1);

    }
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
    uint32_t reg = 0;

    if (core_id == 0) {
        reg = REG_READ(INTPRI_CPU_INTR_FROM_CPU_0_REG);
    } else {
        reg = REG_READ(INTPRI_CPU_INTR_FROM_CPU_1_REG);
    }

    return reg;
}


#ifdef __cplusplus
}
#endif
