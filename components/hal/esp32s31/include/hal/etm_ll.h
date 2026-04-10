/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#include <stdbool.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/etm_types.h"
#include "soc/soc_etm_struct.h"
#include "soc/hp_sys_clkrst_struct.h"

#define ETM_LL_GET(_attr) ETM_LL_ ## _attr
#define ETM_LL_SUPPORT(_feat) ETM_LL_SUPPORT_ ## _feat

// Number of ETM instances
#define ETM_LL_INST_NUM 1

// Number of channels in each ETM instance
#define ETM_LL_CHANS_PER_INST 50

// Support to get and clear the status of the ETM event and task
#define ETM_LL_SUPPORT_STATUS_REG  1

// Support to set the clock source for ETM
#define ETM_LL_SUPPORT_CLOCK_SRC   1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the clock for ETM register
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void etm_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.etm_ctrl0.reg_etm_apb_clk_en = enable;
}

/**
 * @brief Enable the clock for ETM function
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void etm_ll_enable_function_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.etm_ctrl0.reg_soc_etm_clk_en = enable;
}

/**
 * @brief Set the clock source for ETM
 *
 * @param group_id Group ID
 * @param clk_src Clock source
 */
static inline void etm_ll_set_clock_source(int group_id, etm_clock_source_t clk_src)
{
    (void)group_id;
    switch (clk_src) {
    case ETM_CLK_SRC_XTAL:
        HP_SYS_CLKRST.etm_ctrl0.reg_soc_etm_clk_sel = 0;
        break;
    case ETM_CLK_SRC_RC_FAST:
        HP_SYS_CLKRST.etm_ctrl0.reg_soc_etm_clk_sel = 1;
        break;
    case ETM_CLK_SRC_PLL_F80M:
        HP_SYS_CLKRST.etm_ctrl0.reg_soc_etm_clk_sel = 2;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Reset the ETM register
 *
 * @param group_id Group ID
 */
static inline void etm_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.etm_ctrl0.reg_etm_rst_en = 1;
    HP_SYS_CLKRST.etm_ctrl0.reg_etm_rst_en = 0;
}

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
    hw->channel[chan].eid.chn_evt_id = event;
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
    hw->channel[chan].tid.chn_task_id = task;
}

#ifdef __cplusplus
}
#endif
