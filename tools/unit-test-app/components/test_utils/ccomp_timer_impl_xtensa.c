/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "ccomp_timer_impl.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "eri.h"
#include "freertos/FreeRTOS.h"
#include "esp_freertos_hooks.h"
#include "perfmon.h"
#include "xtensa/core-macros.h"
#include "xtensa/xt_perf_consts.h"
#include "xtensa-debug-module.h"
#include "esp_private/esp_clk.h"

#define D_STALL_COUNTER_ID 0
#define I_STALL_COUNTER_ID 1

typedef enum
{
    PERF_TIMER_UNINIT = 0,              // timer has not been initialized yet
    PERF_TIMER_IDLE,                    // timer has been initialized but is not tracking elapsed time
    PERF_TIMER_ACTIVE                   // timer is tracking elapsed time
} ccomp_timer_state_t;

typedef struct
{
    int i_ovfl;                         // number of times instruction stall counter has overflowed
    int d_ovfl;                         // number of times data stall counter has overflowed
    uint32_t last_ccount;               // last CCOUNT value, updated every os tick
    ccomp_timer_state_t state;          // state of the timer
    intr_handle_t intr_handle;          // handle to allocated handler for perfmon counter overflows, so that it can be freed during deinit
    int64_t ccount;                     // accumulated processors cycles during the time when timer is active
} ccomp_timer_status_t;

// Each core has its independent timer
ccomp_timer_status_t s_status[] = {
    (ccomp_timer_status_t){
        .i_ovfl = 0,
        .d_ovfl = 0,
        .ccount = 0,
        .last_ccount = 0,
        .state = PERF_TIMER_UNINIT,
        .intr_handle = NULL,
    },
    (ccomp_timer_status_t){
        .i_ovfl = 0,
        .d_ovfl = 0,
        .ccount = 0,
        .last_ccount = 0,
        .state = PERF_TIMER_UNINIT,
        .intr_handle = NULL
    }
};

static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR update_ccount(void)
{
    if (s_status[xPortGetCoreID()].state == PERF_TIMER_ACTIVE) {
        int64_t new_ccount = xthal_get_ccount();
        if (new_ccount > s_status[xPortGetCoreID()].last_ccount) {
            s_status[xPortGetCoreID()].ccount += new_ccount - s_status[xPortGetCoreID()].last_ccount;
        } else {
            // CCOUNT has wrapped around
            s_status[xPortGetCoreID()].ccount += new_ccount + (UINT32_MAX - s_status[xPortGetCoreID()].last_ccount);
        }
        s_status[xPortGetCoreID()].last_ccount = new_ccount;
    }
}

static void inline update_overflow(int id, int *cnt)
{
    uint32_t pmstat = eri_read(ERI_PERFMON_PMSTAT0 + id * sizeof(int32_t));
    if (pmstat & PMSTAT_OVFL) {
        *cnt += 1;
        // Clear overflow and PerfMonInt asserted bits. The only valid bits in PMSTAT is the ones we're trying to clear. So it should be
        // ok to just modify the whole register.
        eri_write(ERI_PERFMON_PMSTAT0 + id, ~0x0);
    }
}

static void IRAM_ATTR perf_counter_overflow_handler(void *args)
{
    update_overflow(D_STALL_COUNTER_ID, &s_status[xPortGetCoreID()].d_ovfl);
    update_overflow(I_STALL_COUNTER_ID, &s_status[xPortGetCoreID()].i_ovfl);
}

static void set_perfmon_interrupt(bool enable)
{
    uint32_t d_pmctrl = eri_read(ERI_PERFMON_PMCTRL0 + D_STALL_COUNTER_ID * sizeof(int32_t));
    uint32_t i_pmctrl = eri_read(ERI_PERFMON_PMCTRL0 + I_STALL_COUNTER_ID * sizeof(int32_t));

    if (enable) {
        d_pmctrl |= PMCTRL_INTEN;
        i_pmctrl |= PMCTRL_INTEN;
    }
    else {
        d_pmctrl &= ~PMCTRL_INTEN;
        i_pmctrl &= ~PMCTRL_INTEN;
    }

    eri_write(ERI_PERFMON_PMCTRL0 + D_STALL_COUNTER_ID * sizeof(int32_t), d_pmctrl);
    eri_write(ERI_PERFMON_PMCTRL0 + I_STALL_COUNTER_ID * sizeof(int32_t), i_pmctrl);
}


esp_err_t ccomp_timer_impl_init(void)
{
    // Keep track of how many times each counter has overflowed.
    esp_err_t err = esp_intr_alloc(ETS_INTERNAL_PROFILING_INTR_SOURCE, 0,
                         perf_counter_overflow_handler, NULL, &s_status[xPortGetCoreID()].intr_handle);

    if (err != ESP_OK) {
        return err;
    }

    xtensa_perfmon_init(D_STALL_COUNTER_ID,
                        XTPERF_CNT_D_STALL,
                        XTPERF_MASK_D_STALL_BUSY, 0, -1);
    xtensa_perfmon_init(I_STALL_COUNTER_ID,
                        XTPERF_CNT_I_STALL,
                        XTPERF_MASK_I_STALL_BUSY, 0, -1);

    set_perfmon_interrupt(true);
    s_status[xPortGetCoreID()].state = PERF_TIMER_IDLE;
    return ESP_OK;
}

esp_err_t ccomp_timer_impl_deinit(void)
{
    set_perfmon_interrupt(false);

    esp_err_t err = esp_intr_free(s_status[xPortGetCoreID()].intr_handle);

    if (err != ESP_OK) {
        return err;
    }

    s_status[xPortGetCoreID()].intr_handle = NULL;
    s_status[xPortGetCoreID()].state = PERF_TIMER_UNINIT;
    return ESP_OK;
}

esp_err_t ccomp_timer_impl_start(void)
{
    s_status[xPortGetCoreID()].state = PERF_TIMER_ACTIVE;
    s_status[xPortGetCoreID()].last_ccount = xthal_get_ccount();
    // Update elapsed cycles every OS tick
    esp_register_freertos_tick_hook_for_cpu(update_ccount, xPortGetCoreID());
    xtensa_perfmon_start();
    return ESP_OK;
}

esp_err_t IRAM_ATTR ccomp_timer_impl_stop(void)
{
    xtensa_perfmon_stop();
    esp_deregister_freertos_tick_hook_for_cpu(update_ccount, xPortGetCoreID());
    update_ccount();
    s_status[xPortGetCoreID()].state = PERF_TIMER_IDLE;
    return ESP_OK;
}

int64_t IRAM_ATTR ccomp_timer_impl_get_time(void)
{
    update_ccount();
    int64_t d_stalls = xtensa_perfmon_value(D_STALL_COUNTER_ID) +
                       s_status[xPortGetCoreID()].d_ovfl * (1 << sizeof(int32_t));
    int64_t i_stalls = xtensa_perfmon_value(I_STALL_COUNTER_ID) +
                       s_status[xPortGetCoreID()].i_ovfl * (1 << sizeof(int32_t));
    int64_t stalls = d_stalls + i_stalls;
    int64_t cycles = s_status[xPortGetCoreID()].ccount;
    return ((cycles - stalls) * 1000000) / esp_clk_cpu_freq();
}

esp_err_t ccomp_timer_impl_reset(void)
{
    xtensa_perfmon_reset(D_STALL_COUNTER_ID);
    xtensa_perfmon_reset(I_STALL_COUNTER_ID);
    s_status[xPortGetCoreID()].d_ovfl = 0;
    s_status[xPortGetCoreID()].i_ovfl = 0;
    s_status[xPortGetCoreID()].ccount = 0;
    s_status[xPortGetCoreID()].last_ccount = 0;
    return ESP_OK;
}

bool ccomp_timer_impl_is_init(void)
{
    return s_status[xPortGetCoreID()].state != PERF_TIMER_UNINIT;
}

bool IRAM_ATTR ccomp_timer_impl_is_active(void)
{
    return s_status[xPortGetCoreID()].state == PERF_TIMER_ACTIVE;
}

void IRAM_ATTR ccomp_timer_impl_lock(void)
{
    portENTER_CRITICAL(&s_lock);
}

void IRAM_ATTR ccomp_timer_impl_unlock(void)
{
    portEXIT_CRITICAL(&s_lock);
}
