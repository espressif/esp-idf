/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdatomic.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_pm.h"
#include "soc/soc_caps.h"
#include "hal/timer_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

// If ISR handler is allowed to run whilst cache is disabled,
// Make sure all the code and related variables used by the handler are in the SRAM
#if CONFIG_GPTIMER_ISR_IRAM_SAFE || CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM
#define GPTIMER_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define GPTIMER_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

#if CONFIG_GPTIMER_ISR_IRAM_SAFE
#define GPTIMER_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED)
#else
#define GPTIMER_INTR_ALLOC_FLAGS    ESP_INTR_FLAG_INTRDISABLED
#endif

#define GPTIMER_PM_LOCK_NAME_LEN_MAX 16

typedef struct gptimer_t gptimer_t;

typedef struct gptimer_group_t {
    int group_id;
    portMUX_TYPE spinlock; // to protect per-group register level concurrent access
    gptimer_t *timers[SOC_TIMER_GROUP_TIMERS_PER_GROUP];
} gptimer_group_t;

typedef enum {
    GPTIMER_FSM_INIT,        // Timer is initialized, but not enabled
    GPTIMER_FSM_ENABLE,      // Timer is enabled, but is not running
    GPTIMER_FSM_ENABLE_WAIT, // Timer is in the middle of the enable process (Intermediate state)
    GPTIMER_FSM_RUN,         // Timer is in running
    GPTIMER_FSM_RUN_WAIT,    // Timer is in the middle of the run process (Intermediate state)
} gptimer_fsm_t;

struct gptimer_t {
    gptimer_group_t *group;
    int timer_id;
    uint32_t resolution_hz;
    uint64_t reload_count;
    uint64_t alarm_count;
    gptimer_count_direction_t direction;
    timer_hal_context_t hal;
    _Atomic gptimer_fsm_t fsm;
    intr_handle_t intr;
    portMUX_TYPE spinlock; // to protect per-timer resources concurrent accessed by task and ISR handler
    gptimer_alarm_cb_t on_alarm;
    void *user_ctx;
    gptimer_clock_source_t clk_src;
    esp_pm_lock_handle_t pm_lock; // power management lock
#if CONFIG_PM_ENABLE
    char pm_lock_name[GPTIMER_PM_LOCK_NAME_LEN_MAX]; // pm lock name
#endif
    struct {
        uint32_t intr_shared: 1;
        uint32_t auto_reload_on_alarm: 1;
        uint32_t alarm_en: 1;
    } flags;
};

#ifdef __cplusplus
}
#endif
