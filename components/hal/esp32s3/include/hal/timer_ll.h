/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Attention: Timer Group has 3 independent functions: General Purpose Timer, Watchdog Timer and Clock calibration.
//            This Low Level driver only serve the General Purpose Timer function.

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/timer_types.h"
#include "soc/timer_group_struct.h"
#include "soc/system_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get timer group register base address with giving group number
#define TIMER_LL_GET_HW(group_id) ((group_id == 0) ? (&TIMERG0) : (&TIMERG1))
#define TIMER_LL_EVENT_ALARM(timer_id) (1 << (timer_id))

/**
 * @brief Enable the bus clock for timer group module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void _timer_ll_enable_bus_clock(int group_id, bool enable)
{
    if (group_id == 0) {
        SYSTEM.perip_clk_en0.timergroup_clk_en = enable;
    } else {
        SYSTEM.perip_clk_en0.timergroup1_clk_en = enable;
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define timer_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _timer_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the timer group module
 *
 * @note  After reset the register, the "flash boot protection" will be enabled again.
 *        FLash boot protection is not used anymore after system boot up.
 *        This function will disable it by default in order to prevent the system from being reset unexpectedly.
 *
 * @param group_id Group ID
 */
static inline void _timer_ll_reset_register(int group_id)
{
    if (group_id == 0) {
        SYSTEM.perip_rst_en0.timergroup_rst = 1;
        SYSTEM.perip_rst_en0.timergroup_rst = 0;
        TIMERG0.wdtconfig0.wdt_flashboot_mod_en = 0;
    } else {
        SYSTEM.perip_rst_en0.timergroup1_rst = 1;
        SYSTEM.perip_rst_en0.timergroup1_rst = 0;
        TIMERG1.wdtconfig0.wdt_flashboot_mod_en = 0;
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define timer_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _timer_ll_reset_register(__VA_ARGS__); \
    } while(0)

/**
 * @brief Set clock source for timer
 *
 * @param group_id Group ID
 * @param timer_num Timer number in the group
 * @param clk_src Clock source
 */
static inline void timer_ll_set_clock_source(int group_id, uint32_t timer_num, gptimer_clock_source_t clk_src)
{
    timg_dev_t *hw = TIMER_LL_GET_HW(group_id);
    switch (clk_src) {
    case GPTIMER_CLK_SRC_APB:
        hw->hw_timer[timer_num].config.tn_use_xtal = 0;
        break;
    case GPTIMER_CLK_SRC_XTAL:
        hw->hw_timer[timer_num].config.tn_use_xtal = 1;
        break;
    default:
        HAL_ASSERT(false && "unsupported clock source");
        break;
    }
}

/**
 * @brief Enable Timer Group (GPTimer) module clock
 *
 * @note This function is not optional, created for backward compatible.
 *
 * @param group_id Group ID
 * @param timer_num Timer index in the group
 * @param en true to enable, false to disable
 */
static inline void timer_ll_enable_clock(int group_id, uint32_t timer_num, bool en)
{
    (void)group_id;
    (void)timer_num;
    (void)en;
}

/**
 * @brief Enable alarm event
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 * @param en True: enable alarm
 *           False: disable alarm
 */
__attribute__((always_inline))
static inline void timer_ll_enable_alarm(timg_dev_t *hw, uint32_t timer_num, bool en)
{
    hw->hw_timer[timer_num].config.tn_alarm_en = en;
}

/**
 * @brief Set clock prescale for timer
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 * @param divider Prescale value (0 and 1 are not valid)
 */
static inline void timer_ll_set_clock_prescale(timg_dev_t *hw, uint32_t timer_num, uint32_t divider)
{
    HAL_ASSERT(divider >= 2 && divider <= 65536);
    if (divider >= 65536) {
        divider = 0;
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hw_timer[timer_num].config, tn_divider, divider);
    hw->hw_timer[timer_num].config.tn_divcnt_rst = 1;
}

/**
 * @brief Enable auto-reload mode
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 * @param en True: enable auto reload mode
 *           False: disable auto reload mode
 */
__attribute__((always_inline))
static inline void timer_ll_enable_auto_reload(timg_dev_t *hw, uint32_t timer_num, bool en)
{
    hw->hw_timer[timer_num].config.tn_autoreload = en;
}

/**
 * @brief Set count direction
 *
 * @param hw Timer peripheral register base address
 * @param timer_num Timer number in the group
 * @param direction Count direction
 */
static inline void timer_ll_set_count_direction(timg_dev_t *hw, uint32_t timer_num, gptimer_count_direction_t direction)
{
    hw->hw_timer[timer_num].config.tn_increase = (direction == GPTIMER_COUNT_UP);
}

/**
 * @brief Enable timer, start counting
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 * @param en True: enable the counter
 *           False: disable the counter
 */
__attribute__((always_inline))
static inline void timer_ll_enable_counter(timg_dev_t *hw, uint32_t timer_num, bool en)
{
    hw->hw_timer[timer_num].config.tn_en = en;
}

/**
 * @brief Trigger software capture event
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 */
__attribute__((always_inline))
static inline void timer_ll_trigger_soft_capture(timg_dev_t *hw, uint32_t timer_num)
{
    hw->hw_timer[timer_num].update.tn_update = 1;
    // Timer register is in a different clock domain from Timer hardware logic
    // We need to wait for the update to take effect before fetching the count value
    while (hw->hw_timer[timer_num].update.tn_update) {
    }
}

/**
 * @brief Get counter value
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 *
 * @return counter value
 */
__attribute__((always_inline))
static inline uint64_t timer_ll_get_counter_value(timg_dev_t *hw, uint32_t timer_num)
{
    return ((uint64_t)hw->hw_timer[timer_num].hi.tn_hi << 32) | (hw->hw_timer[timer_num].lo.tn_lo);
}

/**
 * @brief Set alarm value
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 * @param alarm_value When counter reaches alarm value, alarm event will be triggered
 */
__attribute__((always_inline))
static inline void timer_ll_set_alarm_value(timg_dev_t *hw, uint32_t timer_num, uint64_t alarm_value)
{
    hw->hw_timer[timer_num].alarmhi.tn_alarm_hi = (uint32_t)(alarm_value >> 32);
    hw->hw_timer[timer_num].alarmlo.tn_alarm_lo = (uint32_t)alarm_value;
}

/**
 * @brief Set reload value
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 * @param reload_val Reload counter value
 */
__attribute__((always_inline))
static inline void timer_ll_set_reload_value(timg_dev_t *hw, uint32_t timer_num, uint64_t reload_val)
{
    hw->hw_timer[timer_num].loadhi.tn_load_hi = (uint32_t)(reload_val >> 32);
    hw->hw_timer[timer_num].loadlo.tn_load_lo = (uint32_t)reload_val;
}

/**
 * @brief Get reload value
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 * @return reload count value
 */
__attribute__((always_inline))
static inline uint64_t timer_ll_get_reload_value(timg_dev_t *hw, uint32_t timer_num)
{
    return ((uint64_t)hw->hw_timer[timer_num].loadhi.tn_load_hi << 32) | (hw->hw_timer[timer_num].loadlo.tn_load_lo);
}

/**
 * @brief Trigger software reload, value set by `timer_ll_set_reload_value()` will be reflected into counter immediately
 *
 * @param hw Timer Group register base address
 * @param timer_num Timer number in the group
 */
__attribute__((always_inline))
static inline void timer_ll_trigger_soft_reload(timg_dev_t *hw, uint32_t timer_num)
{
    hw->hw_timer[timer_num].load.tn_load = 1;
}

/**
 * @brief Enable timer interrupt by mask
 *
 * @param hw Timer Group register base address
 * @param mask Mask of interrupt events
 * @param en True: enable interrupt
 *           False: disable interrupt
 */
__attribute__((always_inline))
static inline void timer_ll_enable_intr(timg_dev_t *hw, uint32_t mask, bool en)
{
    if (en) {
        hw->int_ena_timers.val |= mask;
    } else {
        hw->int_ena_timers.val &= ~mask;
    }
}

/**
 * @brief Get interrupt status
 *
 * @param hw Timer Group register base address
 *
 * @return Interrupt status
 */
__attribute__((always_inline))
static inline uint32_t timer_ll_get_intr_status(timg_dev_t *hw)
{
    return hw->int_st_timers.val & 0x03;
}

/**
 * @brief Clear interrupt status by mask
 *
 * @param hw Timer Group register base address
 * @param mask Interrupt events mask
 */
__attribute__((always_inline))
static inline void timer_ll_clear_intr_status(timg_dev_t *hw, uint32_t mask)
{
    hw->int_clr_timers.val = mask;
}

/**
 * @brief Enable the register clock forever
 *
 * @param hw Timer Group register base address
 * @param en True: Enable the register clock forever
 *           False: Register clock is enabled only when register operation happens
 */
static inline void timer_ll_enable_register_clock_always_on(timg_dev_t *hw, bool en)
{
    hw->regclk.clk_en = en;
}

/**
 * @brief Get interrupt status register address
 *
 * @param hw Timer Group register base address
 *
 * @return Interrupt status register address
 */
static inline volatile void *timer_ll_get_intr_status_reg(timg_dev_t *hw)
{
    return &hw->int_st_timers;
}

#ifdef __cplusplus
}
#endif
