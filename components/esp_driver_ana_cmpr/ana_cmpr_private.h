/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdatomic.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#if CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_pm.h"
#include "driver/ana_cmpr_types.h"
#include "soc/soc_caps.h"
#include "hal/ana_cmpr_ll.h"
#include "hal/ana_cmpr_periph.h"

#define TAG "ana_cmpr"

#if CONFIG_ANA_CMPR_OBJ_CACHE_SAFE
#define ANA_CMPR_MEM_ALLOC_CAPS         (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define ANA_CMPR_MEM_ALLOC_CAPS         MALLOC_CAP_DEFAULT
#endif

#if CONFIG_ANA_CMPR_ISR_CACHE_SAFE
// the interrupt source of Analog Comparator is marked as shared because
// it may share with other GPIO interrupt events
// it may share with different analog comparator units
#define ANA_CMPR_INTR_FLAG              (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED)
#else
#define ANA_CMPR_INTR_FLAG              (ESP_INTR_FLAG_SHARED)
#endif

#define ANA_CMPR_ALLOW_INTR_PRIORITY_MASK ESP_INTR_FLAG_LOWMED

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This driver uses two state machines at different layers:
 * 1) ana_cmpr_fsm_t (per handle): functional state of a published comparator object.
 *    It protects transitions among init/config/enable operations after users get a valid handle.
 * 2) ana_cmpr_unit_slot_state_t (per unit slot): lifecycle ownership of the global unit slot.
 *    It protects create/delete in-flight windows before publish and after unpublish.
 *
 * Keeping these state machines separated avoids overloading a single field for both
 * object runtime behavior and global slot allocation lifecycle.
 */
typedef enum {
    ANA_CMPR_FSM_INIT,        // Comparator is in the initialization stage, not enabled yet
    ANA_CMPR_FSM_ENABLE,      // Comparator is enabled
    ANA_CMPR_FSM_WAIT,        // Comparator is in the middle of state change, so busy, other operations should wait
} ana_cmpr_fsm_t;

typedef enum {
    ANA_CMPR_UNIT_SLOT_FREE = 0,      // no handle published, slot can be allocated
    ANA_CMPR_UNIT_SLOT_ALLOCATING,    // unit create is in progress
    ANA_CMPR_UNIT_SLOT_READY,         // handle published and available
    ANA_CMPR_UNIT_SLOT_DELETING,      // unit delete is in progress
} ana_cmpr_unit_slot_state_t;

typedef struct ana_cmpr_t ana_cmpr_t;

typedef struct ana_cmpr_ref_chan_t {
    ana_cmpr_ref_source_t       ref_src;  // the reference source type of this reference channel, internal or external
    // for internal reference channel
    ana_cmpr_ref_voltage_t      ref_volt;
    ana_cmpr_ref_hys_t          ref_hys_level;
    // for external reference channel
    int                         gpio_num;
    int                         pad_id;   // the pad id corresponding to the gpio_num
} ana_cmpr_ref_chan_t;

typedef struct ana_cmpr_src_chan_t {
    uint8_t                     chan_id;  // there're multiple source channels in the analog comparator unit
    int                         pad_id;   // the pad id corresponding to the gpio_num
    ana_cmpr_cross_type_t       cross_type;
    int                         gpio_num;
    bool                        configured;
} ana_cmpr_src_chan_t;

struct ana_cmpr_t {
    int                         unit_id;            // Analog comparator unit id
    analog_cmpr_dev_t           *dev;               // Analog comparator unit device address
    _Atomic ana_cmpr_fsm_t      fsm;                // The state machine of the Analog Comparator unit
    ana_cmpr_event_callbacks_t  cbs;                // The callback group that set by user
    void                        *user_data;         // User data that passed to the callbacks
    intr_handle_t               intr_handle;        // Interrupt handle
    uint32_t                    intr_mask;          // Interrupt mask
    int                         intr_priority;      // Interrupt priority
    uint32_t                    src_clk_freq_hz;    // Source clock frequency of the Analog Comparator unit
    ana_cmpr_src_chan_t         src_chans[ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM)]; // The source channel objects in the unit
    ana_cmpr_ref_chan_t         ref_chan;           // The reference channel object in the unit
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t        pm_lock;            // The Power Management lock that used to avoid unexpected power down of the clock domain
#endif
};

__attribute__((always_inline))
static inline bool _ana_cmpr_is_src_chan_id_valid(int src_chan_id)
{
    return (src_chan_id >= 0) && (src_chan_id < ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM));
}

extern portMUX_TYPE s_ana_cmpr_spinlock;

__attribute__((always_inline))
static inline void _ana_cmpr_enter_critical(void)
{
    portENTER_CRITICAL_SAFE(&s_ana_cmpr_spinlock);
}

__attribute__((always_inline))
static inline void _ana_cmpr_exit_critical(void)
{
    portEXIT_CRITICAL_SAFE(&s_ana_cmpr_spinlock);
}

__attribute__((always_inline))
static inline bool _ana_cmpr_try_acquire_state(ana_cmpr_handle_t cmpr, ana_cmpr_fsm_t expected_state)
{
    return atomic_compare_exchange_strong(&cmpr->fsm, &expected_state, ANA_CMPR_FSM_WAIT);
}

__attribute__((always_inline))
static inline void _ana_cmpr_release_state(ana_cmpr_handle_t cmpr, ana_cmpr_fsm_t target_state)
{
    atomic_store(&cmpr->fsm, target_state);
}

/*
 * Centralized FSM transition rules.
 *
 * We only model externally visible states here (INIT/ENABLE).
 * WAIT is an internal transient state managed by the transition macro.
 * It is never a stable/public state, so it doesn't appear in allowed table.
 */
__attribute__((always_inline))
static inline bool _ana_cmpr_is_transition_allowed(ana_cmpr_fsm_t expected_state, ana_cmpr_fsm_t target_state)
{
    switch (expected_state) {
    case ANA_CMPR_FSM_INIT:
        return (target_state == ANA_CMPR_FSM_INIT || target_state == ANA_CMPR_FSM_ENABLE);
    case ANA_CMPR_FSM_ENABLE:
        return (target_state == ANA_CMPR_FSM_ENABLE || target_state == ANA_CMPR_FSM_INIT);
    default:
        return false;
    }
}

/*
 * Finalize a scoped transition with centralized rule checking.
 * If caller provides an invalid target, fail-safe to expected_state.
 */
__attribute__((always_inline))
static inline void _ana_cmpr_complete_transition(ana_cmpr_handle_t cmpr, ana_cmpr_fsm_t expected_state, ana_cmpr_fsm_t target_state)
{
    // only do the validation in debug build, in release build we trust the caller to provide the correct target_state to avoid extra overhead
    assert(_ana_cmpr_is_transition_allowed(expected_state, target_state) && "invalid ana_cmpr fsm transition target");
    _ana_cmpr_release_state(cmpr, target_state);
}

/*
 * Token concatenation helpers used by scoped macros below.
 *
 * We append __LINE__ to guard variable names so each macro expansion gets a
 * unique local variable and can be used multiple times in one function.
 */
#define _ANA_CMPR_CONCAT_IMPL(x, y) x##y
#define _ANA_CMPR_CONCAT(x, y) _ANA_CMPR_CONCAT_IMPL(x, y)

/*
 * Scoped critical section helper (RAII-like in C).
 *
 * Usage:
 *   ANA_CMPR_CRITICAL_SECTION() {
 *       // protected code
 *   }
 *
 * Expansion model:
 * - Enter critical section in for-loop initializer.
 * - Run loop body exactly once while guard == true.
 * - Exit critical section in for-loop increment expression.
 *
 * Important:
 * - Do NOT use early return/goto/break/continue in this block, otherwise
 *   _ana_cmpr_exit_critical() may be skipped.
 */
#define ANA_CMPR_CRITICAL_SECTION() \
    for (bool _ANA_CMPR_CONCAT(_ana_cmpr_critical_guard_, __LINE__) = (_ana_cmpr_enter_critical(), true); \
         _ANA_CMPR_CONCAT(_ana_cmpr_critical_guard_, __LINE__); \
         _ana_cmpr_exit_critical(), _ANA_CMPR_CONCAT(_ana_cmpr_critical_guard_, __LINE__) = false)

/*
 * Scoped FSM transition helper.
 *
 * Semantics:
 * - Write whether acquisition succeeds to acquired_var.
 * - If acquired, run body once and complete transition using target_state_expr.
 * - target_state_expr can be either a value or a mutable variable.
 *
 * Typical usage:
 *   bool acquired = false;
 *   ana_cmpr_fsm_t final_state = ANA_CMPR_FSM_INIT;
 *   ANA_CMPR_WITH_STATE_TRANSITION(cmpr, ANA_CMPR_FSM_INIT, final_state, acquired) {
 *       // configure HW before making FSM visible as ENABLE
 *       final_state = ANA_CMPR_FSM_ENABLE;
 *   }
 *   if (!acquired) { return ESP_ERR_INVALID_STATE; }
 *
 * Important:
 * - Do NOT use early return/goto/break/continue in this block, otherwise
 *   restore/transition step may be skipped.
 */
#define ANA_CMPR_WITH_STATE_TRANSITION(cmpr, expected_state, target_state_var, acquired_var) \
    for (bool _ANA_CMPR_CONCAT(_ana_cmpr_temp_state_guard_, __LINE__) = (((acquired_var) = _ana_cmpr_try_acquire_state((cmpr), (expected_state))), \
                                                                          (acquired_var)); \
         _ANA_CMPR_CONCAT(_ana_cmpr_temp_state_guard_, __LINE__); \
         _ana_cmpr_complete_transition((cmpr), (expected_state), (target_state_var)), _ANA_CMPR_CONCAT(_ana_cmpr_temp_state_guard_, __LINE__) = false)

/*
 * Backward-compatible shorthand:
 * - expected_state -> WAIT -> expected_state
 * - writes acquisition result to acquired_var
 * - implemented on top of ANA_CMPR_WITH_STATE_TRANSITION
 */
#define ANA_CMPR_WITH_TEMP_STATE(cmpr, expected_state, acquired_var) \
    ANA_CMPR_WITH_STATE_TRANSITION((cmpr), (expected_state), (expected_state), (acquired_var))

#ifdef __cplusplus
}
#endif
