/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
    uint64_t sleep_duration = shared_mem->sleep_duration_us;

    if (sleep_duration) {
        ulp_lp_core_lp_timer_set_wakeup_time(sleep_duration);
    }

    ulp_lp_core_halt();
}
