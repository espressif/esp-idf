/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/hp_system_struct.h"
#include "soc/lp_system_struct.h"
#include "hal/debug_probe_types.h"

/**
 * @brief Define how many channels are there in one debug probe unit
 */
#define DEBUG_PROBE_LL_CHANNELS_PER_UNIT 2

#ifdef __cplusplus
extern "C" {
#endif

// translate the HAL types into register values
#define DEBUG_PROBE_LL_PART_TO_REG_VAL(part) ((uint8_t[]) {0, 1}[(part)])

/**
 * @brief Enable the debug probe module
 *
 * @param unit_id unit identifier, see debug_probe_unit_id_t
 * @param en true: enable, false: disable
 */
static inline void debug_probe_ll_enable_unit(debug_probe_unit_id_t unit_id, bool en)
{
    if (unit_id == DEBUG_PROBE_UNIT_HP) {
        HP_SYSTEM.probea_ctrl.reg_probe_global_en = en;
    } else {
        LP_SYS.lp_probea_ctrl.probe_global_en = en;
    }
}

/**
 * @brief Enable a specific channel in the debug probe unit
 *
 * @param unit_id unit identifier, see debug_probe_unit_id_t
 * @param channel channel number (only support 0 and 1)
 * @param en true: enable, false: disable
 */
static inline void debug_probe_ll_enable_channel(debug_probe_unit_id_t unit_id, int channel, bool en)
{
    // channel 0 is always enabled
    if (channel == 1) {
        if (unit_id == DEBUG_PROBE_UNIT_HP) {
            HP_SYSTEM.probeb_ctrl.reg_probe_b_en = en;
        } else {
            LP_SYS.lp_probeb_ctrl.probe_b_en = en;
        }
    }
}

/**
 * @brief Set the target module for a probe channel
 *
 * @param unit_id unit identifier, see debug_probe_unit_id_t
 * @param channel channel number (only support 0 and 1)
 * @param target_module  HP: soc_debug_probe_target_t, LP: soc_debug_probe_target_lp_t
 */
static inline void debug_probe_ll_channel_set_target_module(debug_probe_unit_id_t unit_id, uint8_t channel, uint8_t target_module)
{
    if (channel == 0) {
        if (unit_id == DEBUG_PROBE_UNIT_HP) {
            HP_SYSTEM.probea_ctrl.reg_probe_a_top_sel = target_module;
        } else {
            LP_SYS.lp_probea_ctrl.probe_a_top_sel = target_module;
        }
    } else {
        if (unit_id == DEBUG_PROBE_UNIT_HP) {
            HP_SYSTEM.probeb_ctrl.reg_probe_b_top_sel = target_module;
        } else {
            LP_SYS.lp_probeb_ctrl.probe_b_top_sel = target_module;
        }
    }
}

/**
 * @brief Add signals from a specific group to the probe channel
 *
 * @note One sig_group contains 4 bytes of signals. The sig_byte_idx is used to select the byte in the sig_group.
 *
 * @param channel channel number (only support 0 and 1)
 * @param sig_byte_idx signal byte index (0-3)
 * @param sig_group signal group (0-15)
 */
static inline void debug_probe_ll_channel_add_signal_group(debug_probe_unit_id_t unit_id, uint8_t channel, uint8_t sig_byte_idx, uint8_t sig_group)
{
    if (unit_id == DEBUG_PROBE_UNIT_HP) {
        if (channel == 0) {
            HP_SYSTEM.probea_ctrl.reg_probe_a_mod_sel &= ~(0xf << (sig_byte_idx * 4));
            HP_SYSTEM.probea_ctrl.reg_probe_a_mod_sel |= (sig_group << (sig_byte_idx * 4));
        } else {
            HP_SYSTEM.probeb_ctrl.reg_probe_b_mod_sel &= ~(0xf << (sig_byte_idx * 4));
            HP_SYSTEM.probeb_ctrl.reg_probe_b_mod_sel |= (sig_group << (sig_byte_idx * 4));
        }
    } else {
        if (channel == 0) {
            LP_SYS.lp_probea_ctrl.probe_a_mod_sel &= ~(0xf << (sig_byte_idx * 4));
            LP_SYS.lp_probea_ctrl.probe_a_mod_sel |= (sig_group << (sig_byte_idx * 4));
        } else {
            LP_SYS.lp_probeb_ctrl.probe_b_mod_sel &= ~(0xf << (sig_byte_idx * 4));
            LP_SYS.lp_probeb_ctrl.probe_b_mod_sel |= (sig_group << (sig_byte_idx * 4));
        }
    }
}

/**
 * @brief Set the lower 16 bits of the probe output
 *
 * @param part where does the probe_top_out[15:0] come from
 */
static inline void debug_probe_ll_set_lower16_output(debug_probe_unit_id_t unit_id, int channel, debug_probe_split_u16_t part)
{
    uint8_t val = (uint8_t)(channel * 2 + DEBUG_PROBE_LL_PART_TO_REG_VAL(part));
    if (unit_id == DEBUG_PROBE_UNIT_HP) {
        HP_SYSTEM.probea_ctrl.reg_probe_l_sel = val;
    } else {
        LP_SYS.lp_probea_ctrl.probe_l_sel = val;
    }
}

/**
 * @brief Set the upper 16 bits of the probe output
 *
 * @param part where does the probe_top_out[31:16] come from
 * @note No-op for LP unit: only 16 bits are routed to LP GPIO, so upper 16 selection has no effect.
 */
static inline void debug_probe_ll_set_upper16_output(debug_probe_unit_id_t unit_id, int channel, debug_probe_split_u16_t part)
{
    if (unit_id != DEBUG_PROBE_UNIT_HP) {
        return; /* LP has only 16 bits to GPIO; probe_top_out[31:16] is not routed */
    }
    HP_SYSTEM.probea_ctrl.reg_probe_h_sel = channel * 2 + DEBUG_PROBE_LL_PART_TO_REG_VAL(part);
}

/**
 * @brief Read the value that currently being probed
 *
 * @note ESP32P4 can only route the LSB probe_top_out[15:0] to the GPIO pad. But the register still saves 32 signal bits.
 *
 * @return the value that currently being probed
 */
static inline uint32_t debug_probe_ll_read_output(debug_probe_unit_id_t unit_id)
{
    if (unit_id == DEBUG_PROBE_UNIT_HP) {
        return HP_SYSTEM.probe_out.reg_probe_top_out;
    } else {
        return LP_SYS.lp_probe_out.probe_top_out;
    }
}

#ifdef __cplusplus
}
#endif
