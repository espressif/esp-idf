/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/dport_reg.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Clear the crosscore interrupt that just occurred on the current core
 */
static inline void crosscore_int_ll_clear_interrupt(int core_id)
{
    DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, 0);
}


/**
 * @brief Trigger a crosscore interrupt on the given core
 *
 * @param core_id Core to trigger an interrupt on. Ignored on single core targets.
 */
static inline void crosscore_int_ll_trigger_interrupt(int core_id)
{
    DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, DPORT_CPU_INTR_FROM_CPU_0);
}


#ifdef __cplusplus
}
#endif
