/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/soc_caps.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_lp_timer_shared.h"
#include "ulp_lp_core_memory_shared.h"

extern void main();

/* Initialize lp core related system functions before calling user's main*/
void lp_core_startup()
{
    ulp_lp_core_update_wakeup_cause();

    main();

    ulp_lp_core_memory_shared_cfg_t* shared_mem = ulp_lp_core_memory_shared_cfg_get();

#if SOC_LP_TIMER_SUPPORTED
    uint64_t sleep_duration_ticks = shared_mem->sleep_duration_ticks;

    if (sleep_duration_ticks) {
        ulp_lp_core_lp_timer_set_wakeup_ticks(sleep_duration_ticks);
    }
#endif //SOC_LP_TIMER_SUPPORTED

    ulp_lp_core_halt();
}
