/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32 MCPWM register operations

#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/mcpwm_struct.h"
#include "soc/dport_reg.h"
#include "hal/mcpwm_types.h"
#include "hal/misc.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get MCPWM group register base address
#define MCPWM_LL_GET_HW(ID)                  (((ID) == 0) ? &MCPWM0 : &MCPWM1)

// MCPWM interrupt event mask
#define MCPWM_LL_EVENT_TIMER_STOP(timer)     (1 << (timer))
#define MCPWM_LL_EVENT_TIMER_EMPTY(timer)    (1 << ((timer) + 3))
#define MCPWM_LL_EVENT_TIMER_FULL(timer)     (1 << ((timer) + 6))
#define MCPWM_LL_EVENT_TIMER_MASK(timer)     (MCPWM_LL_EVENT_TIMER_STOP(timer) | MCPWM_LL_EVENT_TIMER_EMPTY(timer) | MCPWM_LL_EVENT_TIMER_FULL(timer))
#define MCPWM_LL_EVENT_FAULT_ENTER(fault)    (1 << ((fault) + 9))
#define MCPWM_LL_EVENT_FAULT_EXIT(fault)     (1 << ((fault) + 12))
#define MCPWM_LL_EVENT_FAULT_MASK(fault)     (MCPWM_LL_EVENT_FAULT_ENTER(fault) | MCPWM_LL_EVENT_FAULT_EXIT(fault))
#define MCPWM_LL_EVENT_CMP_EQUAL(oper, cmp)  (1 << ((oper) + (cmp) * 3 + 15))
#define MCPWM_LL_EVENT_OPER_BRAKE_CBC(oper)  (1 << ((oper) + 21))
#define MCPWM_LL_EVENT_OPER_BRAKE_OST(oper)  (1 << ((oper) + 24))
#define MCPWM_LL_EVENT_OPER_MASK(oper)       (MCPWM_LL_EVENT_OPER_BRAKE_CBC(oper) | MCPWM_LL_EVENT_OPER_BRAKE_OST(oper))
#define MCPWM_LL_EVENT_CAPTURE(cap)          (1 << ((cap) + 27))

// Maximum values due to limited register bit width
#define MCPWM_LL_MAX_GROUP_PRESCALE          256
#define MCPWM_LL_MAX_TIMER_PRESCALE          256
#define MCPWM_LL_MAX_CARRIER_PRESCALE        16
#define MCPWM_LL_MAX_CARRIER_ONESHOT         16
#define MCPWM_LL_MAX_CAPTURE_PRESCALE        256
#define MCPWM_LL_MAX_CAPTURE_TIMER_PRESCALE  1
#define MCPWM_LL_MAX_DEAD_DELAY              65536
#define MCPWM_LL_MAX_COUNT_VALUE             65536

// translate the HAL types into register values
#define MCPWM_LL_TIMER_EVENT_TO_REG_VAL(event) ((uint8_t[]) {0, 1}[(event)])
#define MCPWM_LL_GEN_ACTION_TO_REG_CAL(action) ((uint8_t[]) {0, 1, 2, 3}[(action)])
#define MCPWM_LL_BRAKE_MODE_TO_REG_VAL(mode)  ((uint8_t[]) {0, 1}[(mode)])

/**
 * @brief The dead time module's clock source
 */
typedef enum {
    MCPWM_LL_DEADTIME_CLK_SRC_GROUP,
    MCPWM_LL_DEADTIME_CLK_SRC_TIMER,
} mcpwm_ll_deadtime_clock_src_t;

////////////////////////////////////////MCPWM Group Specific////////////////////////////////////////////////////////////

/**
 * @brief Enable the bus clock for MCPWM module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void mcpwm_ll_enable_bus_clock(int group_id, bool enable)
{
    uint32_t reg_val = DPORT_READ_PERI_REG(DPORT_PERIP_CLK_EN_REG);
    if (group_id == 0) {
        reg_val &= ~DPORT_PWM0_CLK_EN;
        reg_val |= enable << 17;
    } else {
        reg_val &= ~DPORT_PWM1_CLK_EN;
        reg_val |= enable << 20;
    }
    DPORT_WRITE_PERI_REG(DPORT_PERIP_CLK_EN_REG, reg_val);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mcpwm_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        mcpwm_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the MCPWM module
 *
 * @param group_id Group ID
 */
static inline void mcpwm_ll_reset_register(int group_id)
{
    if (group_id == 0) {
        DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, DPORT_PWM0_RST);
        DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, 0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, DPORT_PWM1_RST);
        DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, 0);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mcpwm_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        mcpwm_ll_reset_register(__VA_ARGS__); \
    } while(0)

/**
 * @brief Enable MCPWM function clock
 *
 * @note Not support to enable/disable the peripheral clock
 *
 * @param group_id Group ID
 * @param en true to enable, false to disable
 */
static inline void mcpwm_ll_group_enable_clock(int group_id, bool en)
{
    (void)group_id;
    (void)en;
}

/**
 * @brief Set the clock source for MCPWM
 *
 * @param group_id Group ID
 * @param clk_src Clock source for the MCPWM peripheral
 */
static inline void mcpwm_ll_group_set_clock_source(int group_id, mcpwm_timer_clock_source_t clk_src)
{
    (void)group_id;
    (void)clk_src;
}

/**
 * @brief Set the MCPWM group clock prescale
 *
 * @param group_id Group ID
 * @param pre_scale Prescale value
 */
static inline void mcpwm_ll_group_set_clock_prescale(int group_id, int prescale)
{
    // group clock: PWM_clk = CLK_160M / (prescale)
    HAL_ASSERT(prescale <= 256 && prescale > 0);
    mcpwm_dev_t *mcpwm = MCPWM_LL_GET_HW(group_id);
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->clk_cfg, clk_prescale, prescale - 1);
}

/**
 * @brief Enable update MCPWM active registers from shadow registers
 *
 * @param mcpwm Peripheral instance address
 */
static inline void mcpwm_ll_group_enable_shadow_mode(mcpwm_dev_t *mcpwm)
{
    mcpwm->update_cfg.global_up_en = 1;
    mcpwm->update_cfg.op0_up_en = 1;
    mcpwm->update_cfg.op1_up_en = 1;
    mcpwm->update_cfg.op2_up_en = 1;
}

/**
 * @brief Flush shadow registers to active registers
 *
 * @param mcpwm Peripheral instance address
 */
static inline void mcpwm_ll_group_flush_shadow(mcpwm_dev_t *mcpwm)
{
    // a toggle can trigger a forced update of all active registers in MCPWM, i.e. shadow->active
    mcpwm->update_cfg.global_force_up = 1;
    mcpwm->update_cfg.global_force_up = 0;
}

//////////////////////////////////////////Interrupt Specific////////////////////////////////////////////////////////////

/**
 * @brief Get interrupt status register address
 *
 * @param mcpwm Peripheral instance address
 * @return Register address
 */
static inline volatile void *mcpwm_ll_intr_get_status_reg(mcpwm_dev_t *mcpwm)
{
    return &mcpwm->int_st;
}

/**
 * @brief Enable MCPWM interrupt for specific event mask
 *
 * @param mcpwm Peripheral instance address
 * @param mask Event mask
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_intr_enable(mcpwm_dev_t *mcpwm, uint32_t mask, bool enable)
{
    if (enable) {
        mcpwm->int_ena.val |= mask;
    } else {
        mcpwm->int_ena.val &= ~mask;
    }
}

/**
 * @brief Get MCPWM interrupt status
 *
 * @param mcpwm Peripheral instance address
 * @return Interrupt status
 */
__attribute__((always_inline))
static inline uint32_t mcpwm_ll_intr_get_status(mcpwm_dev_t *mcpwm)
{
    return mcpwm->int_st.val;
}

/**
 * @brief Clear MCPWM interrupt status by mask
 *
 * @param mcpwm Peripheral instance address
 * @param mask Interrupt status mask
 */
__attribute__((always_inline))
static inline void mcpwm_ll_intr_clear_status(mcpwm_dev_t *mcpwm, uint32_t mask)
{
    mcpwm->int_clr.val = mask;
}

////////////////////////////////////////MCPWM Timer Specific////////////////////////////////////////////////////////////

/**
 * @brief Set MCPWM timer prescale
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @param prescale Prescale value
 */
static inline void mcpwm_ll_timer_set_clock_prescale(mcpwm_dev_t *mcpwm, int timer_id, uint32_t prescale)
{
    HAL_ASSERT(prescale <= 256 && prescale > 0);
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_prescale, prescale - 1);
}

/**
 * @brief Set peak value for MCPWM timer
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @param peak Peak value
 * @param symmetric True to set symmetric peak value, False to set asymmetric peak value
 */
__attribute__((always_inline))
static inline void mcpwm_ll_timer_set_peak(mcpwm_dev_t *mcpwm, int timer_id, uint32_t peak, bool symmetric)
{
    if (!symmetric) { // in asymmetric mode, period = [0,peak-1]
        HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_period, peak - 1);
    } else { // in symmetric mode, period = [0,peak-1] + [peak,1]
        HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_period, peak);
    }
}

/**
 * @brief Update MCPWM period immediately
 * @note When period value is updated in the shadow register, it will be flushed to active register immediately.
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 */
static inline void mcpwm_ll_timer_update_period_at_once(mcpwm_dev_t *mcpwm, int timer_id)
{
    mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod = 0;
}

/**
 * @brief Enable to update MCPWM period upon TEZ event
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_timer_enable_update_period_on_tez(mcpwm_dev_t *mcpwm, int timer_id, bool enable)
{
    if (enable) {
        mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod |= 0x01;
    } else {
        mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod &= ~0x01;
    }
}

/**
 * @brief Enable to update MCPWM period upon sync event
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_timer_enable_update_period_on_sync(mcpwm_dev_t *mcpwm, int timer_id, bool enable)
{
    if (enable) {
        mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod |= 0x02;
    } else {
        mcpwm->timer[timer_id].timer_cfg0.timer_period_upmethod &= ~0x02;
    }
}

/**
 * @brief Set MCPWM timer count mode
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @param mode Timer count mode
 */
static inline void mcpwm_ll_timer_set_count_mode(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_count_mode_t mode)
{
    switch (mode) {
    case MCPWM_TIMER_COUNT_MODE_PAUSE:
        mcpwm->timer[timer_id].timer_cfg1.timer_mod = 0;
        break;
    case MCPWM_TIMER_COUNT_MODE_UP:
        mcpwm->timer[timer_id].timer_cfg1.timer_mod = 1;
        break;
    case MCPWM_TIMER_COUNT_MODE_DOWN:
        mcpwm->timer[timer_id].timer_cfg1.timer_mod = 2;
        break;
    case MCPWM_TIMER_COUNT_MODE_UP_DOWN:
        mcpwm->timer[timer_id].timer_cfg1.timer_mod = 3;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Execute MCPWM timer start/stop command
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id  Timer ID, index from 0 to 2
 * @param cmd Timer start/stop command
 */
static inline void mcpwm_ll_timer_set_start_stop_command(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_start_stop_cmd_t cmd)
{
    switch (cmd) {
    case MCPWM_TIMER_STOP_EMPTY:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 0;
        break;
    case MCPWM_TIMER_STOP_FULL:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 1;
        break;
    case MCPWM_TIMER_START_NO_STOP:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 2;
        break;
    case MCPWM_TIMER_START_STOP_EMPTY:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 3;
        break;
    case MCPWM_TIMER_START_STOP_FULL:
        mcpwm->timer[timer_id].timer_cfg1.timer_start = 4;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Get timer count value
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @return Timer count value
 */
__attribute__((always_inline))
static inline uint32_t mcpwm_ll_timer_get_count_value(mcpwm_dev_t *mcpwm, int timer_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->timer[timer_id].timer_status, timer_value);
}

/**
 * @brief Get timer count direction
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @return Timer count direction
 */
__attribute__((always_inline))
static inline mcpwm_timer_direction_t mcpwm_ll_timer_get_count_direction(mcpwm_dev_t *mcpwm, int timer_id)
{
    return mcpwm->timer[timer_id].timer_status.timer_direction ? MCPWM_TIMER_DIRECTION_DOWN : MCPWM_TIMER_DIRECTION_UP;
}

/**
 * @brief Enable sync input for timer
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_timer_enable_sync_input(mcpwm_dev_t *mcpwm, int timer_id, bool enable)
{
    mcpwm->timer[timer_id].timer_sync.timer_synci_en = enable;
}

/**
 * @brief Use the input sync signal as the output sync signal (i.e. propagate the input sync signal)
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 */
static inline void mcpwm_ll_timer_propagate_input_sync(mcpwm_dev_t *mcpwm, int timer_id)
{
    // sync_out is selected to sync_in
    mcpwm->timer[timer_id].timer_sync.timer_synco_sel = 0;
}

/**
 * @brief Set the sync output signal to one of the timer event
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @param event Timer event
 */
static inline void mcpwm_ll_timer_sync_out_on_timer_event(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_event_t event)
{
    switch (event) {
    case MCPWM_TIMER_EVENT_EMPTY:
        mcpwm->timer[timer_id].timer_sync.timer_synco_sel = 1;
        break;
    case MCPWM_TIMER_EVENT_FULL:
        mcpwm->timer[timer_id].timer_sync.timer_synco_sel = 2;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Disable sync output for MCPWM timer
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 */
static inline void mcpwm_ll_timer_disable_sync_out(mcpwm_dev_t *mcpwm, int timer_id)
{
    // sync_out will always be zero
    mcpwm->timer[timer_id].timer_sync.timer_synco_sel = 3;
}

/**
 * @brief Trigger MCPWM timer software sync event
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 */
static inline void mcpwm_ll_timer_trigger_soft_sync(mcpwm_dev_t *mcpwm, int timer_id)
{
    mcpwm->timer[timer_id].timer_sync.timer_sync_sw = ~mcpwm->timer[timer_id].timer_sync.timer_sync_sw;
}

/**
 * @brief Set sync count value for MCPWM timer
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @param phase_value Sync phase value
 */
static inline void mcpwm_ll_timer_set_sync_phase_value(mcpwm_dev_t *mcpwm, int timer_id, uint32_t phase_value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->timer[timer_id].timer_sync, timer_phase, phase_value);
}

/**
 * @brief Set sync phase direction for MCPWM timer
 *
 * @param mcpwm Peripheral instance address
 * @param timer_id Timer ID, index from 0 to 2
 * @param direction Sync phase direction
 */
static inline void mcpwm_ll_timer_set_sync_phase_direction(mcpwm_dev_t *mcpwm, int timer_id, mcpwm_timer_direction_t direction)
{
    mcpwm->timer[timer_id].timer_sync.timer_phase_direction = direction;
}

/**
 * @brief Select which GPIO sync input to use
 *
 * @param mcpwm Peripheral instance address
 * @param timer Timer ID, index from 0 to 2
 * @param gpio_sync_id GPIO sync ID, index from 0 to 2
 */
static inline void mcpwm_ll_timer_set_gpio_sync_input(mcpwm_dev_t *mcpwm, int timer, int gpio_sync_id)
{
    mcpwm->timer_synci_cfg.val &= ~(0x07 << (timer * 3));
    mcpwm->timer_synci_cfg.val |= (gpio_sync_id + 4) << (timer * 3);
}

/**
 * @brief Select which timer sync input to use
 *
 * @param mcpwm Peripheral instance address
 * @param timer Timer ID, index from 0 to 2
 * @param timer_sync_id Timer sync ID, index from 0 to 2
 */
static inline void mcpwm_ll_timer_set_timer_sync_input(mcpwm_dev_t *mcpwm, int timer, int timer_sync_id)
{
    mcpwm->timer_synci_cfg.val &= ~(0x07 << (timer * 3));
    mcpwm->timer_synci_cfg.val |= (timer_sync_id + 1) << (timer * 3);
}

/**
 * @brief Clear timer sync input selection
 *
 * @param mcpwm Peripheral instance address
 * @param timer Timer ID, index from 0 to 2
 */
static inline void mcpwm_ll_timer_clear_sync_input(mcpwm_dev_t *mcpwm, int timer)
{
    // no sync input is selected, but software sync can still work
    mcpwm->timer_synci_cfg.val &= ~(0x07 << (timer * 3));
}

/**
 * @brief Invert the GPIO sync input signal
 *
 * @param mcpwm Peripheral instance address
 * @param sync_id GPIO sync ID, index from 0 to 2
 * @param invert True to invert, False to not invert
 */
static inline void mcpwm_ll_invert_gpio_sync_input(mcpwm_dev_t *mcpwm, int sync_id, bool invert)
{
    if (invert) {
        mcpwm->timer_synci_cfg.val |= 1 << (sync_id + 9);
    } else {
        mcpwm->timer_synci_cfg.val &= ~(1 << (sync_id + 9));
    }
}

////////////////////////////////////////MCPWM Operator Specific/////////////////////////////////////////////////////////

/**
 * @brief Flush operator shadow registers to active registers
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 */
static inline void mcpwm_ll_operator_flush_shadow(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->update_cfg.val ^= (1 << (2 * operator_id + 3));
}

/**
 * @brief Connect operator and timer by ID
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param timer_id Timer ID, index from 0 to 2
 */
static inline void mcpwm_ll_operator_connect_timer(mcpwm_dev_t *mcpwm, int operator_id, int timer_id)
{
    mcpwm->operator_timersel.val &= ~(0x03 << (2 * operator_id));
    mcpwm->operator_timersel.val |= (timer_id << (2 * operator_id));
}

/**
 * @brief Update the compare value immediately
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param compare_id Compare ID, index from 0 to 1
 */
static inline void mcpwm_ll_operator_update_compare_at_once(mcpwm_dev_t *mcpwm, int operator_id, int compare_id)
{
    mcpwm->operators[operator_id].gen_stmp_cfg.val &= ~(0x0F << (4 * compare_id));
}

/**
 * @brief Enable to update the compare value upon TEZ event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param compare_id Compare ID, index from 0 to 1
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_operator_enable_update_compare_on_tez(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_stmp_cfg.val |= (1 << 0) << (4 * compare_id);
    } else {
        mcpwm->operators[operator_id].gen_stmp_cfg.val &= ~((1 << 0) << (4 * compare_id));
    }
}

/**
 * @brief Enable to update the compare value upon TEP event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param compare_id Compare ID, index from 0 to 1
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_operator_enable_update_compare_on_tep(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_stmp_cfg.val |= (1 << 1) << (4 * compare_id);
    } else {
        mcpwm->operators[operator_id].gen_stmp_cfg.val &= ~((1 << 1) << (4 * compare_id));
    }
}

/**
 * @brief Enable to update the compare value upon sync event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param compare_id Compare ID, index from 0 to 1
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_operator_enable_update_compare_on_sync(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_stmp_cfg.val |= (1 << 2) << (4 * compare_id);
    } else {
        mcpwm->operators[operator_id].gen_stmp_cfg.val &= ~((1 << 2) << (4 * compare_id));
    }
}

/**
 * @brief Stop updating the compare value
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param compare_id Compare ID, index from 0 to 1
 * @param stop_or_not True to stop, False to not stop
 */
static inline void mcpwm_ll_operator_stop_update_compare(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, bool stop_or_not)
{
    if (stop_or_not) {
        mcpwm->operators[operator_id].gen_stmp_cfg.val |= (1 << 3) << (4 * compare_id);
    } else {
        mcpwm->operators[operator_id].gen_stmp_cfg.val &= ~((1 << 3) << (4 * compare_id));
    }
}

/**
 * @brief Set compare value for comparator
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param compare_id Compare ID, index from 0 to 1
 * @param compare_value Compare value
 */
__attribute__((always_inline))
static inline void mcpwm_ll_operator_set_compare_value(mcpwm_dev_t *mcpwm, int operator_id, int compare_id, uint32_t compare_value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->operators[operator_id].timestamp[compare_id], gen, compare_value);
}

/**
 * @brief Update operator actions immediately
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 */
static inline void mcpwm_ll_operator_update_action_at_once(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod = 0;
}

/**
 * @brief Enable update actions on TEZ event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_operator_enable_update_action_on_tez(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod |= 1 << 0;
    } else {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod &= ~(1 << 0);
    }
}

/**
 * @brief Enable update actions on TEP event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_operator_enable_update_action_on_tep(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod |= 1 << 1;
    } else {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod &= ~(1 << 1);
    }
}

/**
 * @brief Enable update actions on sync event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_operator_enable_update_action_on_sync(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod |= 1 << 2;
    } else {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod &= ~(1 << 2);
    }
}

/**
 * @brief Stop updating actions
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param stop_or_not True to stop, False to not stop
 */
static inline void mcpwm_ll_operator_stop_update_action(mcpwm_dev_t *mcpwm, int operator_id, bool stop_or_not)
{
    if (stop_or_not) {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod |= 1 << 3;
    } else {
        mcpwm->operators[operator_id].gen_cfg0.gen_cfg_upmethod &= ~(1 << 3);
    }
}

/**
 * @brief Set trigger from GPIO (reuse the fault GPIO)
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param trig_id Trigger ID, index from 0 to 1
 * @param fault_gpio_id Fault GPIO ID, index from 0 to 3
 */
static inline void mcpwm_ll_operator_set_trigger_from_gpio_fault(mcpwm_dev_t *mcpwm, int operator_id, int trig_id, int fault_gpio_id)
{
    mcpwm->operators[operator_id].gen_cfg0.val &= ~(0x07 << (4 + 3 * trig_id));
    mcpwm->operators[operator_id].gen_cfg0.val |= (fault_gpio_id << (4 + 3 * trig_id));
}

/**
 * @brief Set trigger from sync event (when the timer/gpio/soft taken the sync signal)
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param trig_id Trigger ID, index from 0 to 1
 */
static inline void mcpwm_ll_operator_set_trigger_from_sync(mcpwm_dev_t *mcpwm, int operator_id, int trig_id)
{
    // the timer here is not selectable, must be the one connected with the operator
    mcpwm->operators[operator_id].gen_cfg0.val &= ~(0x07 << (4 + 3 * trig_id));
    mcpwm->operators[operator_id].gen_cfg0.val |= (3 << (4 + 3 * trig_id));
}

////////////////////////////////////////MCPWM Generator Specific////////////////////////////////////////////////////////

/**
 * @brief Reset actions for the generator
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 */
static inline void mcpwm_ll_generator_reset_actions(mcpwm_dev_t *mcpwm, int operator_id, int generator_id)
{
    mcpwm->operators[operator_id].generator[generator_id].val = 0;
}

/**
 * @brief Set generator action on timer event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 * @param direction Timer direction
 * @param event Timer event
 * @param action Action to set
 */
static inline void mcpwm_ll_generator_set_action_on_timer_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
                                                                mcpwm_timer_direction_t direction, mcpwm_timer_event_t event, mcpwm_generator_action_t action)
{
    // empty: 0, full: 1
    if (direction == MCPWM_TIMER_DIRECTION_UP) { // utez, utep
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (MCPWM_LL_TIMER_EVENT_TO_REG_VAL(event) * 2));
        mcpwm->operators[operator_id].generator[generator_id].val |= MCPWM_LL_GEN_ACTION_TO_REG_CAL(action) << (MCPWM_LL_TIMER_EVENT_TO_REG_VAL(event) * 2);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) { // dtez, dtep
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (MCPWM_LL_TIMER_EVENT_TO_REG_VAL(event) * 2 + 12));
        mcpwm->operators[operator_id].generator[generator_id].val |= MCPWM_LL_GEN_ACTION_TO_REG_CAL(action) << (MCPWM_LL_TIMER_EVENT_TO_REG_VAL(event) * 2 + 12);
    }
}

/**
 * @brief Set generator action on compare event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 * @param direction Timer direction
 * @param comp_id Compare ID, index from 0 to 1
 * @param action Action to set
 */
static inline void mcpwm_ll_generator_set_action_on_compare_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
                                                                  mcpwm_timer_direction_t direction, int cmp_id, int action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) { // utea, uteb
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (cmp_id * 2 + 4));
        mcpwm->operators[operator_id].generator[generator_id].val |= MCPWM_LL_GEN_ACTION_TO_REG_CAL(action) << (cmp_id * 2 + 4);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) { // dtea, dteb
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (cmp_id * 2 + 16));
        mcpwm->operators[operator_id].generator[generator_id].val |= MCPWM_LL_GEN_ACTION_TO_REG_CAL(action) << (cmp_id * 2 + 16);
    }
}

/**
 * @brief Set generator action on trigger event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 * @param direction Timer direction
 * @param trig_id Trigger ID, index from 0 to 1
 * @param action Action to set
 */
static inline void mcpwm_ll_generator_set_action_on_trigger_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
                                                                  mcpwm_timer_direction_t direction, int trig_id, int action)
{
    if (direction == MCPWM_TIMER_DIRECTION_UP) { // ut0, ut1
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (trig_id * 2 + 8));
        mcpwm->operators[operator_id].generator[generator_id].val |= MCPWM_LL_GEN_ACTION_TO_REG_CAL(action) << (trig_id * 2 + 8);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) { // dt0, dt1
        mcpwm->operators[operator_id].generator[generator_id].val &= ~(0x03 << (trig_id * 2 + 20));
        mcpwm->operators[operator_id].generator[generator_id].val |= MCPWM_LL_GEN_ACTION_TO_REG_CAL(action) << (trig_id * 2 + 20);
    }
}

/**
 * @brief Set generator action on brake event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 * @param direction Timer direction
 * @param brake_mode Brake mode
 * @param action Action to set
 */
static inline void mcpwm_ll_generator_set_action_on_brake_event(mcpwm_dev_t *mcpwm, int operator_id, int generator_id,
                                                                mcpwm_timer_direction_t direction, mcpwm_operator_brake_mode_t brake_mode, int action)
{
    // the following bit operation is highly depend on the register bit layout.
    // the priority comes: generator ID > brake mode > direction
    if (direction == MCPWM_TIMER_DIRECTION_UP) {
        mcpwm->operators[operator_id].fh_cfg0.val &= ~(0x03 << (8 + 8 * generator_id + 4 * MCPWM_LL_BRAKE_MODE_TO_REG_VAL(brake_mode) + 2));
        mcpwm->operators[operator_id].fh_cfg0.val |= MCPWM_LL_GEN_ACTION_TO_REG_CAL(action) << (8 + 8 * generator_id + 4 * MCPWM_LL_BRAKE_MODE_TO_REG_VAL(brake_mode) + 2);
    } else if (direction == MCPWM_TIMER_DIRECTION_DOWN) {
        mcpwm->operators[operator_id].fh_cfg0.val &= ~(0x03 << (8 + 8 * generator_id + 4 * MCPWM_LL_BRAKE_MODE_TO_REG_VAL(brake_mode)));
        mcpwm->operators[operator_id].fh_cfg0.val |= MCPWM_LL_GEN_ACTION_TO_REG_CAL(action) << (8 + 8 * generator_id + 4 * MCPWM_LL_BRAKE_MODE_TO_REG_VAL(brake_mode));
    }
}

/**
 * @brief Trigger non-continue forced action for generator
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 */
static inline void mcpwm_ll_gen_trigger_noncontinue_force_action(mcpwm_dev_t *mcpwm, int operator_id, int generator_id)
{
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_nciforce = ~mcpwm->operators[operator_id].gen_force.gen_a_nciforce;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_nciforce = ~mcpwm->operators[operator_id].gen_force.gen_b_nciforce;
    }
}

/**
 * @brief Disable continue forced action for generator
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 */
static inline void mcpwm_ll_gen_disable_continue_force_action(mcpwm_dev_t *mcpwm, int operator_id, int generator_id)
{
    mcpwm->operators[operator_id].gen_force.gen_cntuforce_upmethod = 0; // update force method immediately
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_cntuforce_mode = 0;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_cntuforce_mode = 0;
    }
}

/**
 * @brief Disable non-continue forced action for generator
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 */
static inline void mcpwm_ll_gen_disable_noncontinue_force_action(mcpwm_dev_t *mcpwm, int operator_id, int generator_id)
{
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_nciforce_mode = 0;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_nciforce_mode = 0;
    }
}

/**
 * @brief Set continue force level for generator
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 * @param level Force level to set
 */
static inline void mcpwm_ll_gen_set_continue_force_level(mcpwm_dev_t *mcpwm, int operator_id, int generator_id, int level)
{
    mcpwm->operators[operator_id].gen_force.gen_cntuforce_upmethod = 0; // update force method immediately
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_cntuforce_mode = level + 1;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_cntuforce_mode = level + 1;
    }
}

/**
 * @brief Set non-continue force level for generator
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator_id Generator ID, index from 0 to 1
 * @param level Force level to set
 */
static inline void mcpwm_ll_gen_set_noncontinue_force_level(mcpwm_dev_t *mcpwm, int operator_id, int generator_id, int level)
{
    if (generator_id == 0) {
        mcpwm->operators[operator_id].gen_force.gen_a_nciforce_mode = level + 1;
    } else {
        mcpwm->operators[operator_id].gen_force.gen_b_nciforce_mode = level + 1;
    }
}

////////////////////////////////////////MCPWM Dead Time Specific////////////////////////////////////////////////////////

/**
 * @brief Set clock source for dead time submodule
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param src Clock source for dead time submodule
 */
static inline void mcpwm_ll_operator_set_deadtime_clock_src(mcpwm_dev_t *mcpwm, int operator_id, mcpwm_ll_deadtime_clock_src_t src)
{
    switch (src) {
    case MCPWM_LL_DEADTIME_CLK_SRC_GROUP:
        mcpwm->operators[operator_id].dt_cfg.dt_clk_sel = 0;
        break;
    case MCPWM_LL_DEADTIME_CLK_SRC_TIMER:
        mcpwm->operators[operator_id].dt_cfg.dt_clk_sel = 1;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Select the generator for RED block
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator Generator ID, index from 0 to 1
 */
static inline void mcpwm_ll_deadtime_red_select_generator(mcpwm_dev_t *mcpwm, int operator_id, int generator)
{
    mcpwm->operators[operator_id].dt_cfg.dt_red_insel = generator;
}

/**
 * @brief Select the generator for FED block
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param generator Generator ID, index from 0 to 1
 */
static inline void mcpwm_ll_deadtime_fed_select_generator(mcpwm_dev_t *mcpwm, int operator_id, int generator)
{
    mcpwm->operators[operator_id].dt_cfg.dt_fed_insel = generator;
}

/**
 * @brief Set which path to bypass in the deadtime submodule
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param path Path to bypass, index from 0 to 1
 * @param bypass True to bypass, False to not bypass
 */
static inline void mcpwm_ll_deadtime_bypass_path(mcpwm_dev_t *mcpwm, int operator_id, int path, bool bypass)
{
    if (bypass) {
        mcpwm->operators[operator_id].dt_cfg.val |= 1 << (path + 15);
    } else {
        mcpwm->operators[operator_id].dt_cfg.val &= ~(1 << (path + 15));
    }
}

/**
 * @brief Invert the output path
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param path Path to invert, index from 0 to 1
 * @param invert True to invert, False to not invert
 */
static inline void mcpwm_ll_deadtime_invert_outpath(mcpwm_dev_t *mcpwm, int operator_id, int path, bool invert)
{
    if (invert) {
        mcpwm->operators[operator_id].dt_cfg.val |= 1 << (path + 13);
    } else {
        mcpwm->operators[operator_id].dt_cfg.val &= ~(1 << (path + 13));
    }
}

/**
 * @brief Swap the output path
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param path Path to swap, index from 0 to 1
 * @param swap True to swap, False to not swap
 */
static inline void mcpwm_ll_deadtime_swap_out_path(mcpwm_dev_t *mcpwm, int operator_id, int path, bool swap)
{
    if (swap) {
        mcpwm->operators[operator_id].dt_cfg.val |= 1 << (path + 9);
    } else {
        mcpwm->operators[operator_id].dt_cfg.val &= ~(1 << (path + 9));
    }
}

/**
 * @brief Enable the DEB block
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_deadtime_enable_deb(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->operators[operator_id].dt_cfg.dt_deb_mode = enable;
}

/**
 * @brief Get the deadtime switch topology
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @return Dead time submodule's switch topology, each bit represents one switch on/off status
 */
static inline uint32_t mcpwm_ll_deadtime_get_switch_topology(mcpwm_dev_t *mcpwm, int operator_id)
{
    return (mcpwm->operators[operator_id].dt_cfg.dt_deb_mode << 8) | (mcpwm->operators[operator_id].dt_cfg.dt_b_outswap << 7) |
           (mcpwm->operators[operator_id].dt_cfg.dt_a_outswap << 6) | (mcpwm->operators[operator_id].dt_cfg.dt_fed_insel << 5) |
           (mcpwm->operators[operator_id].dt_cfg.dt_red_insel << 4) | (mcpwm->operators[operator_id].dt_cfg.dt_fed_outinvert << 3) |
           (mcpwm->operators[operator_id].dt_cfg.dt_red_outinvert << 2) | (mcpwm->operators[operator_id].dt_cfg.dt_a_outbypass << 1) |
           (mcpwm->operators[operator_id].dt_cfg.dt_b_outbypass << 0);
}

/**
 * @brief Set falling edge delay duration
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param fed Delay duration, in deadtime submodule's clock cycles
 */
static inline void mcpwm_ll_deadtime_set_falling_delay(mcpwm_dev_t *mcpwm, int operator_id, uint32_t fed)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->operators[operator_id].dt_fed_cfg, dt_fed, fed - 1);
}

/**
 * @brief Set rising edge delay duration
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param red Delay duration, in deadtime submodule's clock cycles
 */
static inline void mcpwm_ll_deadtime_set_rising_delay(mcpwm_dev_t *mcpwm, int operator_id, uint32_t red)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->operators[operator_id].dt_red_cfg, dt_red, red - 1);
}

/**
 * @brief Update deadtime immediately
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 */
static inline void mcpwm_ll_deadtime_update_delay_at_once(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod = 0;
    mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod = 0;
}

/**
 * @brief Enable to update deadtime on TEZ event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_deadtime_enable_update_delay_on_tez(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod |= 1 << 0;
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod |= 1 << 0;
    } else {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod &= ~(1 << 0);
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod &= ~(1 << 0);
    }
}

/**
 * @brief Enable to update deadtime on TEP event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_deadtime_enable_update_delay_on_tep(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod |= 1 << 1;
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod |= 1 << 1;
    } else {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod &= ~(1 << 1);
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod &= ~(1 << 1);
    }
}

/**
 * @brief Enable to update deadtime on sync event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_deadtime_enable_update_delay_on_sync(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod |= 1 << 2;
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod |= 1 << 2;
    } else {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod &= ~(1 << 2);
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod &= ~(1 << 2);
    }
}

/**
 * @brief Stop updating deadtime
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param stop_or_not True to stop, False to continue
 */
static inline void mcpwm_ll_deadtime_stop_update_delay(mcpwm_dev_t *mcpwm, int operator_id, bool stop_or_not)
{
    if (stop_or_not) {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod |= 1 << 3;
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod |= 1 << 3;
    } else {
        mcpwm->operators[operator_id].dt_cfg.dt_fed_upmethod &= ~(1 << 3);
        mcpwm->operators[operator_id].dt_cfg.dt_red_upmethod &= ~(1 << 3);
    }
}

////////////////////////////////////////MCPWM Carrier Specific//////////////////////////////////////////////////////////

/**
 * @brief Enable carrier for MCPWM operator
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_carrier_enable(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_en = enable;
}

/**
 * @brief Set prescale for MCPWM carrier source clock
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param prescale Prescale value
 */
static inline void mcpwm_ll_carrier_set_prescale(mcpwm_dev_t *mcpwm, int operator_id, uint8_t prescale)
{
    HAL_ASSERT(prescale > 0 && prescale <= 16);
    mcpwm->operators[operator_id].carrier_cfg.carrier_prescale = prescale - 1;
}

/**
 * @brief Set duty cycle of MCPWM carrier
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param carrier_duty Duty cycle value
 */
static inline void mcpwm_ll_carrier_set_duty(mcpwm_dev_t *mcpwm, int operator_id, uint8_t carrier_duty)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_duty = carrier_duty;
}

/**
 * @brief Invert the signal after the carrier is applied
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param invert True to invert, False to not invert
 */
static inline void mcpwm_ll_carrier_out_invert(mcpwm_dev_t *mcpwm, int operator_id, bool invert)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_out_invert = invert;
}

/**
 * @brief Invert the signal before applying the carrier
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param invert True to invert, False to not invert
 */
static inline void mcpwm_ll_carrier_in_invert(mcpwm_dev_t *mcpwm, int operator_id, bool invert)
{
    mcpwm->operators[operator_id].carrier_cfg.carrier_in_invert = invert;
}

/**
 * @brief Set the first pulse width of the carrier
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param pulse_width Pulse width
 */
static inline void mcpwm_ll_carrier_set_first_pulse_width(mcpwm_dev_t *mcpwm, int operator_id, uint8_t pulse_width)
{
    HAL_ASSERT(pulse_width >= 1);
    mcpwm->operators[operator_id].carrier_cfg.carrier_oshtwth = pulse_width - 1;
}

////////////////////////////////////////MCPWM Fault Specific////////////////////////////////////////////////////////////

/**
 * @brief Enable GPIO fault detection
 *
 * @param mcpwm Peripheral instance address
 * @param fault_sig GPIO fault ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_fault_enable_detection(mcpwm_dev_t *mcpwm, int fault_sig, bool enable)
{
    if (enable) {
        mcpwm->fault_detect.val |= 1 << fault_sig;
    } else {
        mcpwm->fault_detect.val &= ~(1 << fault_sig);
    }
}

/**
 * @brief Set fault polarity (i.e. which level is treated as an active fault)
 *
 * @param mcpwm Peripheral instance address
 * @param fault_sig GPIO fault ID, index from 0 to 2
 * @param level Active level, 0 for low, 1 for high
 */
static inline void mcpwm_ll_fault_set_active_level(mcpwm_dev_t *mcpwm, int fault_sig, bool level)
{
    if (level) {
        mcpwm->fault_detect.val |= 1 << (fault_sig + 3);
    } else {
        mcpwm->fault_detect.val &= ~(1 << (fault_sig + 3));
    }
}

/**
 * @brief Clear the OST brake
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 */
static inline void mcpwm_ll_brake_clear_ost(mcpwm_dev_t *mcpwm, int operator_id)
{
    // a posedge can clear the ost fault status
    mcpwm->operators[operator_id].fh_cfg1.fh_clr_ost = 0;
    mcpwm->operators[operator_id].fh_cfg1.fh_clr_ost = 1;
}

/**
 * @brief Enable the OST brake
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param fault_sig GPIO fault ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_brake_enable_oneshot_mode(mcpwm_dev_t *mcpwm, int operator_id, int fault_sig, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].fh_cfg0.val |= (1 << (7 - fault_sig));
    } else {
        mcpwm->operators[operator_id].fh_cfg0.val &= ~(1 << (7 - fault_sig));
    }
}

/**
 * @brief Enable the CBC brake
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param fault_sig GPIO fault ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_brake_enable_cbc_mode(mcpwm_dev_t *mcpwm, int operator_id, int fault_sig, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].fh_cfg0.val |= (enable << (3 - fault_sig));
    } else {
        mcpwm->operators[operator_id].fh_cfg0.val &= ~(1 << (3 - fault_sig));
    }
}

/**
 * @brief Enable refresh the CBC brake on TEZ event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_brake_enable_cbc_refresh_on_tez(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].fh_cfg1.val |= 1 << 1;
    } else {
        mcpwm->operators[operator_id].fh_cfg1.val &= ~(1 << 1);
    }
}

/**
 * @brief Enable refresh the CBC brake on TEP event
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_fault_enable_cbc_refresh_on_tep(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    if (enable) {
        mcpwm->operators[operator_id].fh_cfg1.val |= 1 << 2;
    } else {
        mcpwm->operators[operator_id].fh_cfg1.val &= ~(1 << 2);
    }
}

/**
 * @brief Enable software CBC brake
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_brake_enable_soft_cbc(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->operators[operator_id].fh_cfg0.fh_sw_cbc = enable;
}

/**
 * @brief Enable software OST brake
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_brake_enable_soft_ost(mcpwm_dev_t *mcpwm, int operator_id, bool enable)
{
    mcpwm->operators[operator_id].fh_cfg0.fh_sw_ost = enable;
}

/**
 * @brief Trigger software CBC brake for once
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 */
static inline void mcpwm_ll_brake_trigger_soft_cbc(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->operators[operator_id].fh_cfg1.fh_force_cbc = ~mcpwm->operators[operator_id].fh_cfg1.fh_force_cbc;
}

/**
 * @brief Trigger software OST brake for once
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 */
static inline void mcpwm_ll_brake_trigger_soft_ost(mcpwm_dev_t *mcpwm, int operator_id)
{
    mcpwm->operators[operator_id].fh_cfg1.fh_force_ost = ~mcpwm->operators[operator_id].fh_cfg1.fh_force_ost;
}

/**
 * @brief Whether the OST brake is still active
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @return True if active, False if not
 */
static inline bool mcpwm_ll_ost_brake_active(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->operators[operator_id].fh_status.fh_ost_on;
}

/**
 * @brief Whether the CBC brake is still active
 *
 * @param mcpwm Peripheral instance address
 * @param operator_id Operator ID, index from 0 to 2
 * @return True if active, False if not
 */
static inline bool mcpwm_ll_cbc_brake_active(mcpwm_dev_t *mcpwm, int operator_id)
{
    return mcpwm->operators[operator_id].fh_status.fh_cbc_on;
}

////////////////////////////////////////MCPWM Capture Specific//////////////////////////////////////////////////////////

/**
 * @brief Enable capture timer
 *
 * @param mcpwm Peripheral instance address
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_capture_enable_timer(mcpwm_dev_t *mcpwm, bool enable)
{
    mcpwm->cap_timer_cfg.cap_timer_en = enable;
}

/**
 * @brief Enable capture channel
 *
 * @param mcpwm Peripheral instance address
 * @param channel Channel ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_capture_enable_channel(mcpwm_dev_t *mcpwm, int channel, bool enable)
{
    mcpwm->cap_chn_cfg[channel].capn_en = enable;
}

/**
 * @brief Set sync phase for capture timer
 *
 * @param mcpwm Peripheral instance address
 * @param phase_value Phase value
 */
static inline void mcpwm_ll_capture_set_sync_phase_value(mcpwm_dev_t *mcpwm, uint32_t phase_value)
{
    mcpwm->cap_timer_phase.cap_timer_phase = phase_value;
}

/**
 * @brief Enable sync for capture timer
 *
 * @param mcpwm Peripheral instance address
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_capture_enable_timer_sync(mcpwm_dev_t *mcpwm, bool enable)
{
    mcpwm->cap_timer_cfg.cap_synci_en = enable;
}

/**
 * @brief Set the timer sync source for MCPWM capture timer
 *
 * @param mcpwm Peripheral instance address
 * @param sync_out_timer MCPWM Timer ID, index from 0 to 2
 */
static inline void mcpwm_ll_capture_set_timer_sync(mcpwm_dev_t *mcpwm, int sync_out_timer)
{
    mcpwm->cap_timer_cfg.cap_synci_sel = sync_out_timer + 1;
}

/**
 * @brief Set the GPIO sync source for MCPWM capture timer
 *
 * @param mcpwm Peripheral instance address
 * @param gpio_sync GPIO sync ID, index from 0 to 2
 */
static inline void mcpwm_ll_capture_set_gpio_sync(mcpwm_dev_t *mcpwm, int gpio_sync)
{
    mcpwm->cap_timer_cfg.cap_synci_sel = gpio_sync + 4;
}

/**
 * @brief Trigger a software sync for capture timer
 *
 * @param mcpwm Peripheral instance address
 */
static inline void mcpwm_ll_capture_trigger_sw_sync(mcpwm_dev_t *mcpwm)
{
    mcpwm->cap_timer_cfg.cap_sync_sw = 1; // auto clear
}

/**
 * @brief Enable capture on positive edge
 *
 * @param mcpwm Peripheral instance address
 * @param channel Channel ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_capture_enable_posedge(mcpwm_dev_t *mcpwm, int channel, bool enable)
{
    if (enable) {
        mcpwm->cap_chn_cfg[channel].val |= 1 << 2;
    } else {
        mcpwm->cap_chn_cfg[channel].val &= ~(1 << 2);
    }
}

/**
 * @brief Enable capture on negative edge
 *
 * @param mcpwm Peripheral instance address
 * @param channel Channel ID, index from 0 to 2
 * @param enable True to enable, False to disable
 */
static inline void mcpwm_ll_capture_enable_negedge(mcpwm_dev_t *mcpwm, int channel, bool enable)
{
    if (enable) {
        mcpwm->cap_chn_cfg[channel].val |= 1 << 1;
    } else {
        mcpwm->cap_chn_cfg[channel].val &= ~(1 << 1);
    }
}

/**
 * @brief Invert the capture input signal
 *
 * @param mcpwm Peripheral instance address
 * @param channel Channel ID, index from 0 to 2
 * @param invert True to invert, False to not invert
 */
static inline void mcpwm_ll_invert_input(mcpwm_dev_t *mcpwm, int channel, bool invert)
{
    mcpwm->cap_chn_cfg[channel].capn_in_invert = invert;
}

/**
 * @brief Trigger the software capture for once
 *
 * @param mcpwm Peripheral instance address
 * @param channel Channel ID, index from 0 to 2
 */
static inline void mcpwm_ll_trigger_soft_capture(mcpwm_dev_t *mcpwm, int channel)
{
    mcpwm->cap_chn_cfg[channel].capn_sw = 1; // auto clear
}

/**
 * @brief Get the captured value
 *
 * @param mcpwm Peripheral instance address
 * @param channel Channel ID, index from 0 to 2
 * @return Captured value
 */
__attribute__((always_inline))
static inline uint32_t mcpwm_ll_capture_get_value(mcpwm_dev_t *mcpwm, int channel)
{
    return mcpwm->cap_chn[channel].capn_value;
}

/**
 * @brief Get the captured edge
 *
 * @param mcpwm Peripheral instance address
 * @param channel Channel ID, index from 0 to 2
 * @return Captured edge
 */
__attribute__((always_inline))
static inline mcpwm_capture_edge_t mcpwm_ll_capture_get_edge(mcpwm_dev_t *mcpwm, int channel)
{
    return mcpwm->cap_status.val & (1 << channel) ? MCPWM_CAP_EDGE_NEG : MCPWM_CAP_EDGE_POS;
}

/**
 * @brief Set the prescale of the input capture signal
 *
 * @param mcpwm Peripheral instance address
 * @param channel Channel ID, index from 0 to 2
 * @param prescale Prescale value
 */
static inline void mcpwm_ll_capture_set_prescale(mcpwm_dev_t *mcpwm, int channel, uint32_t prescale)
{
    HAL_ASSERT(prescale > 0);
    HAL_FORCE_MODIFY_U32_REG_FIELD(mcpwm->cap_chn_cfg[channel], capn_prescale, prescale - 1);
}

//////////////////////////////////////////Deprecated Functions//////////////////////////////////////////////////////////
/////////////////////////////The following functions are only used by the legacy driver/////////////////////////////////
/////////////////////////////They might be removed in the next major release (ESP-IDF 6.0)//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static inline uint32_t mcpwm_ll_timer_get_peak(mcpwm_dev_t *mcpwm, int timer_id, bool symmetric)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->timer[timer_id].timer_cfg0, timer_period) + (symmetric ? 0 : 1);
}

static inline mcpwm_timer_count_mode_t mcpwm_ll_timer_get_count_mode(mcpwm_dev_t *mcpwm, int timer_id)
{
    switch (mcpwm->timer[timer_id].timer_cfg1.timer_mod) {
    case 1:
        return MCPWM_TIMER_COUNT_MODE_UP;
    case 2:
        return MCPWM_TIMER_COUNT_MODE_DOWN;
    case 3:
        return MCPWM_TIMER_COUNT_MODE_UP_DOWN;
    case 0:
    default:
        return MCPWM_TIMER_COUNT_MODE_PAUSE;
    }
}

static inline uint32_t mcpwm_ll_operator_get_compare_value(mcpwm_dev_t *mcpwm, int operator_id, int compare_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(mcpwm->operators[operator_id].timestamp[compare_id], gen);
}

__attribute__((always_inline))
static inline uint32_t mcpwm_ll_intr_get_capture_status(mcpwm_dev_t *mcpwm)
{
    return (mcpwm->int_st.val >> 27) & 0x07;
}

__attribute__((always_inline))
static inline void mcpwm_ll_intr_clear_capture_status(mcpwm_dev_t *mcpwm, uint32_t capture_mask)
{
    mcpwm->int_clr.val = (capture_mask & 0x07) << 27;
}

#ifdef __cplusplus
}
#endif
