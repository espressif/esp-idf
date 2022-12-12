/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "freertos/portmacro.h"
#include "esp_freertos_hooks.h"
#include "soc/soc_caps.h"
#include "esp_rom_sys.h"
#include "esp_cpu.h"
#include "esp_private/esp_clk.h"

typedef enum {
    PERF_TIMER_UNINIT = 0,  // timer has not been initialized yet
    PERF_TIMER_IDLE,        // timer has been initialized but is not tracking elapsed time
    PERF_TIMER_ACTIVE       // timer is tracking elapsed time
} ccomp_timer_state_t;

typedef struct {
    uint32_t last_ccount;      // last CCOUNT value, updated every os tick
    ccomp_timer_state_t state; // state of the timer
    int64_t ccount;            // accumulated processors cycles during the time when timer is active
} ccomp_timer_status_t;

// Each core has its independent timer
ccomp_timer_status_t s_status[SOC_CPU_CORES_NUM];

static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR update_ccount(void)
{
    if (s_status[esp_cpu_get_core_id()].state == PERF_TIMER_ACTIVE) {
        int64_t new_ccount = esp_cpu_get_cycle_count();
        if (new_ccount > s_status[esp_cpu_get_core_id()].last_ccount) {
            s_status[esp_cpu_get_core_id()].ccount += new_ccount - s_status[esp_cpu_get_core_id()].last_ccount;
        } else {
            // CCOUNT has wrapped around
            s_status[esp_cpu_get_core_id()].ccount += new_ccount + (UINT32_MAX - s_status[esp_cpu_get_core_id()].last_ccount);
        }
        s_status[esp_cpu_get_core_id()].last_ccount = new_ccount;
    }
}

esp_err_t ccomp_timer_impl_init(void)
{
    s_status[esp_cpu_get_core_id()].state = PERF_TIMER_IDLE;
    return ESP_OK;
}

esp_err_t ccomp_timer_impl_deinit(void)
{
    s_status[esp_cpu_get_core_id()].state = PERF_TIMER_UNINIT;
    return ESP_OK;
}

esp_err_t ccomp_timer_impl_start(void)
{
    s_status[esp_cpu_get_core_id()].state = PERF_TIMER_ACTIVE;
    s_status[esp_cpu_get_core_id()].last_ccount = esp_cpu_get_cycle_count();
    // Update elapsed cycles every OS tick
    esp_register_freertos_tick_hook_for_cpu(update_ccount, esp_cpu_get_core_id());
    return ESP_OK;
}

esp_err_t IRAM_ATTR ccomp_timer_impl_stop(void)
{
    esp_deregister_freertos_tick_hook_for_cpu(update_ccount, esp_cpu_get_core_id());
    update_ccount();
    s_status[esp_cpu_get_core_id()].state = PERF_TIMER_IDLE;
    return ESP_OK;
}

int64_t IRAM_ATTR ccomp_timer_impl_get_time(void)
{
    update_ccount();
    int64_t cycles = s_status[esp_cpu_get_core_id()].ccount;
    return (cycles * 1000000) / esp_clk_cpu_freq();
}

esp_err_t ccomp_timer_impl_reset(void)
{
    s_status[esp_cpu_get_core_id()].ccount = 0;
    s_status[esp_cpu_get_core_id()].last_ccount = 0;
    return ESP_OK;
}

bool ccomp_timer_impl_is_init(void)
{
    return s_status[esp_cpu_get_core_id()].state != PERF_TIMER_UNINIT;
}

bool IRAM_ATTR ccomp_timer_impl_is_active(void)
{
    return s_status[esp_cpu_get_core_id()].state == PERF_TIMER_ACTIVE;
}

void IRAM_ATTR ccomp_timer_impl_lock(void)
{
    portENTER_CRITICAL(&s_lock);
}

void IRAM_ATTR ccomp_timer_impl_unlock(void)
{
    portEXIT_CRITICAL(&s_lock);
}
