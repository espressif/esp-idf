/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/soc_etm_struct.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock for ETM module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void etm_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl3.reg_etm_apb_clk_en = enable;
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_etm_sys_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define etm_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; etm_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the ETM module
 *
 * @param group_id Group ID
 */
static inline void etm_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_etm = 1;
    HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_etm = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define etm_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; etm_ll_reset_register(__VA_ARGS__)

/**
 * @brief Enable ETM channel
 *
 * @param hw ETM register base address
 * @param chan Channel ID
 */
static inline void etm_ll_enable_channel(soc_etm_dev_t *hw, uint32_t chan)
{
    if (chan < 32) {
        hw->ch_ena_ad0_set.val = 1 << chan;
    } else {
        hw->ch_ena_ad1_set.val = 1 << (chan - 32);
    }
}

/**
 * @brief Disable ETM channel
 *
 * @param hw ETM register base address
 * @param chan Channel ID
 */
static inline void etm_ll_disable_channel(soc_etm_dev_t *hw, uint32_t chan)
{
    if (chan < 32) {
        hw->ch_ena_ad0_clr.val = 1 << chan;
    } else {
        hw->ch_ena_ad1_clr.val = 1 << (chan - 32);
    }
}

/**
 * @brief Check whether the ETM channel is enabled or not
 *
 * @param hw ETM register base address
 * @param chan Channel ID
 * @return true if the channel is enabled, false otherwise
 */
static inline bool etm_ll_is_channel_enabled(soc_etm_dev_t *hw, uint32_t chan)
{
    if (chan < 32) {
        return hw->ch_ena_ad0.val & (1 << chan);
    } else {
        return hw->ch_ena_ad1.val & (1 << (chan - 32));
    }
}

/**
 * @brief Set the input event for the ETM channel
 *
 * @param hw ETM register base address
 * @param chan Channel ID
 * @param event Event ID
 */
static inline void etm_ll_channel_set_event(soc_etm_dev_t *hw, uint32_t chan, uint32_t event)
{
    hw->channel[chan].evt_id.evt_id = event;
}

/**
 * @brief Set the output task for the ETM channel
 *
 * @param hw ETM register base address
 * @param chan Channel ID
 * @param task Task ID
 */
static inline void etm_ll_channel_set_task(soc_etm_dev_t *hw, uint32_t chan, uint32_t task)
{
    hw->channel[chan].task_id.task_id = task;
}

/**
 * @brief Get the flag that marks whether LP CPU is awakened by ETM
 *
 * @return Return true if lpcore is woken up by soc etm flag
 */
static inline bool etm_ll_is_lpcore_wakeup_triggered(void)
{
    return SOC_ETM.task_st5.ulp_task_wakeup_cpu_st;
}

/**
 * @brief Clear the flag that marks whether LP CPU is awakened by ETM
 */
static inline void etm_ll_clear_lpcore_wakeup_status(void)
{
    SOC_ETM.task_st5_clr.ulp_task_wakeup_cpu_st_clr = 1;
}

#ifdef __cplusplus
}
#endif
