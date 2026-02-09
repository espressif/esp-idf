/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/timer_types.h"
#include "hal/timg_ll.h"
#include "soc/timer_group_struct.h"
#include "soc/pcr_struct.h"
#include "soc/soc_etm_source.h"

// Total number of general purpose timers
#define TIMER_LL_GPTIMERS_TOTAL     (TIMG_LL_INST_NUM * TIMG_LL_GPTIMERS_PER_INST)

// Get timer group register base address with giving group number
#define TIMER_LL_GET_HW(group_id) ((group_id == 0) ? (&TIMERG0) : (&TIMERG1))

// Bit width of GPTIMER counter
#define TIMER_LL_COUNTER_BIT_WIDTH   54

// Get alarm interrupt mask with the given timer ID
#define TIMER_LL_EVENT_ALARM(timer_id) (1 << (timer_id))

// Support RC_FAST as function clock
#define TIMER_LL_FUNC_CLOCK_SUPPORT_RC_FAST 1

#ifdef __cplusplus
extern "C" {
#endif

#define TIMER_LL_ETM_TASK_TABLE(group, timer, task)                                     \
    (uint32_t [2][1][GPTIMER_ETM_TASK_MAX]){{{                                          \
                            [GPTIMER_ETM_TASK_START_COUNT] = TG0_TASK_CNT_START_TIMER0, \
                            [GPTIMER_ETM_TASK_STOP_COUNT] = TG0_TASK_CNT_STOP_TIMER0,   \
                            [GPTIMER_ETM_TASK_EN_ALARM] = TG0_TASK_ALARM_START_TIMER0,  \
                            [GPTIMER_ETM_TASK_RELOAD] = TG0_TASK_CNT_RELOAD_TIMER0,     \
                            [GPTIMER_ETM_TASK_CAPTURE] = TG0_TASK_CNT_CAP_TIMER0,       \
                        }},                                                             \
                        {{                                                              \
                            [GPTIMER_ETM_TASK_START_COUNT] = TG1_TASK_CNT_START_TIMER0, \
                            [GPTIMER_ETM_TASK_STOP_COUNT] = TG1_TASK_CNT_STOP_TIMER0,   \
                            [GPTIMER_ETM_TASK_EN_ALARM] = TG1_TASK_ALARM_START_TIMER0,  \
                            [GPTIMER_ETM_TASK_RELOAD] = TG1_TASK_CNT_RELOAD_TIMER0,     \
                            [GPTIMER_ETM_TASK_CAPTURE] = TG1_TASK_CNT_CAP_TIMER0,       \
                        }},                                                             \
    }[group][timer][task]

#define TIMER_LL_ETM_EVENT_TABLE(group, timer, event)                                   \
    (uint32_t [2][1][GPTIMER_ETM_EVENT_MAX]){{{                                         \
                            [GPTIMER_ETM_EVENT_ALARM_MATCH] = TG0_EVT_CNT_CMP_TIMER0,   \
                        }},                                                             \
                        {{                                                              \
                            [GPTIMER_ETM_EVENT_ALARM_MATCH] = TG1_EVT_CNT_CMP_TIMER0,   \
                        }},                                                             \
    }[group][timer][event]

/**
 * @brief Set clock source for timer
 *
 * @param group_id Group ID
 * @param timer_num Timer number in the group
 * @param clk_src Clock source
 */
static inline void timer_ll_set_clock_source(int group_id, uint32_t timer_num, gptimer_clock_source_t clk_src)
{
    (void)timer_num; // only one timer in each group
    uint8_t clk_id = 0;
    switch (clk_src) {
    case GPTIMER_CLK_SRC_XTAL:
        clk_id = 0;
        break;
    case GPTIMER_CLK_SRC_RC_FAST:
        clk_id = 1;
        break;
    case GPTIMER_CLK_SRC_PLL_F48M:
        clk_id = 2;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
    if (group_id == 0) {
        PCR.timergroup0_timer_clk_conf.tg0_timer_clk_sel = clk_id;
    } else {
        PCR.timergroup1_timer_clk_conf.tg1_timer_clk_sel = clk_id;
    }
}

/**
 * @brief Enable Timer Group (GPTimer) module clock
 *
 * @param group_id Group ID
 * @param timer_num Timer index in the group
 * @param en true to enable, false to disable
 */
static inline void timer_ll_enable_clock(int group_id, uint32_t timer_num, bool en)
{
    (void)timer_num; // only one timer in each group
    if (group_id == 0) {
        PCR.timergroup0_timer_clk_conf.tg0_timer_clk_en = en;
    } else {
        PCR.timergroup1_timer_clk_conf.tg1_timer_clk_en = en;
    }
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
    hw->hw_timer[timer_num].config.tx_alarm_en = en;
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hw_timer[timer_num].config, tx_divider, divider);
    hw->hw_timer[timer_num].config.tx_divcnt_rst = 1;
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
    hw->hw_timer[timer_num].config.tx_autoreload = en;
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
    hw->hw_timer[timer_num].config.tx_increase = (direction == GPTIMER_COUNT_UP);
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
    hw->hw_timer[timer_num].config.tx_en = en;
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
    hw->hw_timer[timer_num].update.tx_update = 1;
    // Timer register is in a different clock domain from Timer hardware logic
    // We need to wait for the update to take effect before fetching the count value
    while (hw->hw_timer[timer_num].update.tx_update) {
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
    return ((uint64_t)hw->hw_timer[timer_num].hi.tx_hi << 32) | (hw->hw_timer[timer_num].lo.tx_lo);
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
    hw->hw_timer[timer_num].alarmhi.tx_alarm_hi = (uint32_t)(alarm_value >> 32);
    hw->hw_timer[timer_num].alarmlo.tx_alarm_lo = (uint32_t)alarm_value;
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
    hw->hw_timer[timer_num].loadhi.tx_load_hi = (uint32_t)(reload_val >> 32);
    hw->hw_timer[timer_num].loadlo.tx_load_lo = (uint32_t)reload_val;
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
    return ((uint64_t)hw->hw_timer[timer_num].loadhi.tx_load_hi << 32) | (hw->hw_timer[timer_num].loadlo.tx_load_lo);
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
    hw->hw_timer[timer_num].load.tx_load = 1;
}

/**
 * @brief Enable ETM module
 *
 * @param hw Timer Group register base address
 * @param en True: enable ETM module, False: disable ETM module
 */
static inline void timer_ll_enable_etm(timg_dev_t *hw, bool en)
{
    hw->regclk.etm_en = en;
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
    return hw->int_st_timers.val & 0x01;
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
