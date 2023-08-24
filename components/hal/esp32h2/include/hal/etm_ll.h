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
#include "soc/pcr_struct.h"

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
    PCR.etm_conf.etm_clk_en = enable;
}

/**
 * @brief Reset the ETM register
 *
 * @param group_id Group ID
 */
static inline void etm_ll_reset_register(int group_id)
{
    (void)group_id;
    PCR.etm_conf.etm_rst_en = 1;
    PCR.etm_conf.etm_rst_en = 0;
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

#ifdef __cplusplus
}
#endif
