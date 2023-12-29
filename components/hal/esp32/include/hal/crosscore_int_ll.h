/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_attr.h"
#include "soc/dport_reg.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Clear the crosscore interrupt that just occurred on the current core
 */
FORCE_INLINE_ATTR void crosscore_int_ll_clear_interrupt(int core_id)
{
    if (core_id == 0) {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, 0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, 0);
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
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, DPORT_CPU_INTR_FROM_CPU_0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, DPORT_CPU_INTR_FROM_CPU_1);
    }
}



#ifdef __cplusplus
}
#endif
