/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_pm.h"
#include "soc/soc_caps.h"
#include "hal/mcpwm_hal.h"
#include "hal/mcpwm_types.h"
#include "driver/mcpwm_types.h"
#include "esp_private/sleep_retention.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_MCPWM_ISR_IRAM_SAFE || CONFIG_MCPWM_CTRL_FUNC_IN_IRAM
#define MCPWM_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define MCPWM_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

#if CONFIG_MCPWM_ISR_IRAM_SAFE
#define MCPWM_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM)
#else
#define MCPWM_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_INTRDISABLED)
#endif

// Use retention link only when the target supports sleep retention is enabled
#define MCPWM_USE_RETENTION_LINK  (SOC_MCPWM_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

#define MCPWM_ALLOW_INTR_PRIORITY_MASK ESP_INTR_FLAG_LOWMED

#define MCPWM_GROUP_CLOCK_DEFAULT_PRESCALE 2
#define MCPWM_PM_LOCK_NAME_LEN_MAX 16

typedef struct mcpwm_group_t mcpwm_group_t;
typedef struct mcpwm_timer_t mcpwm_timer_t;
typedef struct mcpwm_cap_timer_t mcpwm_cap_timer_t;
typedef struct mcpwm_oper_t mcpwm_oper_t;
typedef struct mcpwm_cmpr_t mcpwm_cmpr_t;
typedef struct mcpwm_oper_cmpr_t mcpwm_oper_cmpr_t;
typedef struct mcpwm_evt_cmpr_t mcpwm_evt_cmpr_t;
typedef struct mcpwm_gen_t mcpwm_gen_t;
typedef struct mcpwm_fault_t mcpwm_fault_t;
typedef struct mcpwm_gpio_fault_t mcpwm_gpio_fault_t;
typedef struct mcpwm_soft_fault_t mcpwm_soft_fault_t;
typedef struct mcpwm_sync_t mcpwm_sync_t;
typedef struct mcpwm_gpio_sync_src_t mcpwm_gpio_sync_src_t;
typedef struct mcpwm_timer_sync_src_t mcpwm_timer_sync_src_t;
typedef struct mcpwm_soft_sync_src_t mcpwm_soft_sync_src_t;
typedef struct mcpwm_cap_channel_t mcpwm_cap_channel_t;

struct mcpwm_group_t {
    int group_id;            // group ID, index from 0
    int intr_priority;       // MCPWM interrupt priority
    mcpwm_hal_context_t hal; // HAL instance is at group level
    portMUX_TYPE spinlock;   // group level spinlock
    uint32_t prescale;       // group prescale
    uint32_t resolution_hz;  // MCPWM group clock resolution: clock_src_hz / clock_prescale = resolution_hz
    esp_pm_lock_handle_t pm_lock; // power management lock
    soc_module_clk_t clk_src; // peripheral source clock
    mcpwm_cap_timer_t *cap_timer; // mcpwm capture timers
    mcpwm_timer_t *timers[SOC_MCPWM_TIMERS_PER_GROUP]; // mcpwm timer array
    mcpwm_oper_t *operators[SOC_MCPWM_OPERATORS_PER_GROUP]; // mcpwm operator array
    mcpwm_gpio_fault_t *gpio_faults[SOC_MCPWM_GPIO_FAULTS_PER_GROUP]; // mcpwm fault detectors array
    mcpwm_gpio_sync_src_t *gpio_sync_srcs[SOC_MCPWM_GPIO_SYNCHROS_PER_GROUP];  // mcpwm gpio sync array
#if CONFIG_PM_ENABLE
    char pm_lock_name[MCPWM_PM_LOCK_NAME_LEN_MAX]; // pm lock name
#endif
};

typedef enum {
    MCPWM_TIMER_FSM_INIT,
    MCPWM_TIMER_FSM_ENABLE,
} mcpwm_timer_fsm_t;

struct mcpwm_timer_t {
    int timer_id;           // timer ID, index from 0
    mcpwm_group_t *group;   // which group the timer belongs to
    mcpwm_timer_fsm_t fsm;  // driver FSM
    portMUX_TYPE spinlock;  // spin lock
    intr_handle_t intr;     // interrupt handle
    uint32_t resolution_hz; // resolution of the timer
    uint32_t peak_ticks;    // peak ticks that the timer could reach to
    mcpwm_timer_sync_src_t *sync_src;      // timer sync_src
    mcpwm_timer_count_mode_t count_mode; // count mode
    mcpwm_timer_event_cb_t on_full;      // callback function when MCPWM timer counts to peak value
    mcpwm_timer_event_cb_t on_empty;     // callback function when MCPWM timer counts to zero
    mcpwm_timer_event_cb_t on_stop;      // callback function when MCPWM timer stops
    void *user_data;                     // user data which would be passed to the timer callbacks
};

typedef enum {
    MCPWM_TRIGGER_NO_ASSIGN,    //default trigger source
    MCPWM_TRIGGER_GPIO_FAULT,   //trigger assigned to gpio fault
    MCPWM_TRIGGER_SYNC_EVENT,   //trigger assigned to sync event
} mcpwm_trigger_source_t;

struct mcpwm_oper_t {
    int oper_id;           // operator ID, index from 0
    mcpwm_group_t *group;  // which group the timer belongs to
    mcpwm_timer_t *timer;  // which timer is connected to this operator
    portMUX_TYPE spinlock; // spin lock
    intr_handle_t intr;    // interrupt handle
    mcpwm_gen_t *generators[SOC_MCPWM_GENERATORS_PER_OPERATOR];    // mcpwm generator array
    mcpwm_oper_cmpr_t *comparators[SOC_MCPWM_COMPARATORS_PER_OPERATOR]; // mcpwm operator comparator array
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    mcpwm_evt_cmpr_t *event_comparators[SOC_MCPWM_EVENT_COMPARATORS_PER_OPERATOR]; // mcpwm event comparator array
#endif
    mcpwm_trigger_source_t triggers[SOC_MCPWM_TRIGGERS_PER_OPERATOR];                 // mcpwm trigger array, can be either a fault or a sync
    mcpwm_soft_fault_t *soft_fault;                                // mcpwm software fault
    mcpwm_operator_brake_mode_t brake_mode_on_soft_fault;          // brake mode on software triggered fault
    mcpwm_operator_brake_mode_t brake_mode_on_gpio_fault[SOC_MCPWM_GPIO_FAULTS_PER_GROUP]; // brake mode on GPIO triggered faults
    uint32_t deadtime_resolution_hz;     // resolution of deadtime submodule
    mcpwm_gen_t *posedge_delay_owner;    // which generator owns the positive edge delay
    mcpwm_gen_t *negedge_delay_owner;    // which generator owns the negative edge delay
    mcpwm_brake_event_cb_t on_brake_cbc; // callback function which would be invoked when mcpwm operator goes into trip zone
    mcpwm_brake_event_cb_t on_brake_ost; // callback function which would be invoked when mcpwm operator goes into trip zone
    void *user_data;                     // user data which would be passed to the trip zone callback
};

typedef enum {
    MCPWM_OPERATOR_COMPARATOR, // operator comparator, can affect generator's behaviour
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    MCPWM_EVENT_COMPARATOR,    // event comparator, can only generate ETM event
#endif
} mcpwm_comparator_type_t;

struct mcpwm_cmpr_t {
    int cmpr_id;                       // comparator ID, index from 0
    mcpwm_oper_t *oper;                // which operator that the comparator resides in
    portMUX_TYPE spinlock;             // spin lock
    uint32_t compare_ticks;            // compare value of this comparator
    mcpwm_comparator_type_t type;      // comparator type
};

struct mcpwm_oper_cmpr_t {
    mcpwm_cmpr_t base;                 // base class
    intr_handle_t intr;                // interrupt handle
    mcpwm_compare_event_cb_t on_reach; // ISR callback function  which would be invoked on timer counter reaches compare value
    void *user_data;                   // user data which would be passed to the comparator callbacks
};

struct mcpwm_evt_cmpr_t {
    mcpwm_cmpr_t base; // base class
};

struct mcpwm_gen_t {
    int gen_id;             // generator ID, index from 0
    mcpwm_oper_t *oper;     // which operator that the generator resides in
    int gen_gpio_num;       // GPIO number used by the generator
    portMUX_TYPE spinlock;  // spin lock
};

typedef enum {
    MCPWM_FAULT_TYPE_GPIO, // external GPIO fault
    MCPWM_FAULT_TYPE_SOFT, // software fault
} mcpwm_fault_type_t;

struct mcpwm_fault_t {
    mcpwm_group_t *group;    // which group the fault belongs to
    mcpwm_fault_type_t type; // fault type
    esp_err_t (*del)(mcpwm_fault_t *fault);
};

struct mcpwm_gpio_fault_t {
    mcpwm_fault_t base;  // base class
    int fault_id;        // fault detector ID, index from 0
    int gpio_num;        // GPIO number of fault detector
    intr_handle_t intr;  // interrupt handle
    mcpwm_fault_event_cb_t on_fault_enter; // ISR callback function that would be invoked when fault signal got triggered
    mcpwm_fault_event_cb_t on_fault_exit;  // ISR callback function that would be invoked when fault signal got clear
    void *user_data;      // user data which would be passed to the isr_cb
};

struct mcpwm_soft_fault_t {
    mcpwm_fault_t base;     // base class
    mcpwm_oper_t *oper;     // the operator where the soft fault allocated from
};

typedef enum {
    MCPWM_SYNC_TYPE_TIMER, // sync event generated by MCPWM timer count event
    MCPWM_SYNC_TYPE_GPIO,  // sync event generated by GPIO
    MCPWM_SYNC_TYPE_SOFT,  // sync event generated by software
} mcpwm_sync_src_type_t;

struct mcpwm_sync_t {
    mcpwm_group_t *group;      // which group the sync_src belongs to
    mcpwm_sync_src_type_t type; // sync_src type
    esp_err_t (*del)(mcpwm_sync_t *sync_src);
};

struct mcpwm_gpio_sync_src_t {
    mcpwm_sync_t base; // base class
    int sync_id;       // sync signal ID
    int gpio_num; // GPIO number
};

struct mcpwm_timer_sync_src_t {
    mcpwm_sync_t base;    // base class
    mcpwm_timer_t *timer; // timer handle, where this sync_src allocated from
};

typedef enum {
    MCPWM_SOFT_SYNC_FROM_NONE,  // the software sync event generator has not been assigned
    MCPWM_SOFT_SYNC_FROM_TIMER, // the software sync event is generated by MCPWM timer
    MCPWM_SOFT_SYNC_FROM_CAP,   // the software sync event is generated by MCPWM capture timer
} mcpwm_soft_sync_source_t;

struct mcpwm_soft_sync_src_t {
    mcpwm_sync_t base;                       // base class
    mcpwm_soft_sync_source_t soft_sync_from; // where the software sync event is generated by
    union {
        mcpwm_timer_t *timer;                // soft sync is generated by which MCPWM timer
        mcpwm_cap_timer_t *cap_timer;        // soft sync is generated by which MCPWM capture timer
    };
};

typedef enum {
    MCPWM_CAP_TIMER_FSM_INIT,
    MCPWM_CAP_TIMER_FSM_ENABLE,
} mcpwm_cap_timer_fsm_t;

typedef enum {
    MCPWM_CAP_CHAN_FSM_INIT,
    MCPWM_CAP_CHAN_FSM_ENABLE,
} mcpwm_cap_channel_fsm_t;

struct mcpwm_cap_timer_t {
    mcpwm_group_t *group;   // which group the capture timer belongs to
    portMUX_TYPE spinlock;  // spin lock, to prevent concurrently accessing capture timer level resources, including registers
    uint32_t resolution_hz; // resolution of capture timer
    mcpwm_cap_timer_fsm_t fsm;    // driver FSM
    esp_pm_lock_handle_t pm_lock; // power management lock
    mcpwm_cap_channel_t *cap_channels[SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER]; // capture channel array
};

struct mcpwm_cap_channel_t {
    int cap_chan_id;                  // capture channel ID, index from 0
    mcpwm_cap_timer_t *cap_timer;     // which capture timer that the channel resides in
    uint32_t prescale;                // prescale of capture signal
    int gpio_num;                     // GPIO number used by the channel
    mcpwm_cap_channel_fsm_t fsm;      // driver FSM
    intr_handle_t intr;               // Interrupt handle
    mcpwm_capture_event_cb_t on_cap;  // Callback function which would be invoked in capture interrupt routine
    void *user_data;                  // user data which would be passed to the capture callback
};

mcpwm_group_t *mcpwm_acquire_group_handle(int group_id);
void mcpwm_release_group_handle(mcpwm_group_t *group);
esp_err_t mcpwm_check_intr_priority(mcpwm_group_t *group, int intr_priority);
int mcpwm_get_intr_priority_flag(mcpwm_group_t *group);
esp_err_t mcpwm_select_periph_clock(mcpwm_group_t *group, soc_module_clk_t clk_src);
esp_err_t mcpwm_set_prescale(mcpwm_group_t *group, uint32_t expect_module_resolution_hz, uint32_t module_prescale_max, uint32_t *ret_module_prescale);

#if MCPWM_USE_RETENTION_LINK
void mcpwm_create_retention_module(mcpwm_group_t *group);
#endif

#ifdef __cplusplus
}
#endif
