/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C61 PMU register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "esp_attr.h"
#include "hal/assert.h"
#include "soc/pmu_struct.h"
#include "hal/pmu_types.h"
#include "hal/misc.h"

// TODO: [ESP32C61] IDF-9250
#pragma message "pmu_ll.h has not been fully updated on ESP32C61 (IDF-9250). Use with care!"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the power domain that needs to be powered down in the digital power
 *
 * @param hw Beginning address of the peripheral registers.
 * @param mode The pmu mode
 * @param flag Digital power domain flag
 *
 * @return None
 */
FORCE_INLINE_ATTR void pmu_ll_hp_set_dig_power(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t flag)
{
    hw->hp_sys[mode].dig_power.val = flag;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_icg_func(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t icg_func)
{
    hw->hp_sys[mode].icg_func = icg_func;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_icg_apb(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t bitmap)
{
    hw->hp_sys[mode].icg_apb = bitmap;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_icg_modem(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t code)
{
    hw->hp_sys[mode].icg_modem.code = code;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_uart_wakeup_enable(pmu_dev_t *hw, pmu_hp_mode_t mode, bool wakeup_en)
{
    hw->hp_sys[mode].syscntl.uart_wakeup_en = wakeup_en;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_hold_all_lp_pad(pmu_dev_t *hw, pmu_hp_mode_t mode, bool hold_all)
{
    hw->hp_sys[mode].syscntl.lp_pad_hold_all = hold_all;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_hold_all_hp_pad(pmu_dev_t *hw, pmu_hp_mode_t mode, bool hold_all)
{
    hw->hp_sys[mode].syscntl.hp_pad_hold_all = hold_all;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_dig_pad_slp_sel(pmu_dev_t *hw, pmu_hp_mode_t mode, bool slp_sel)
{
    hw->hp_sys[mode].syscntl.dig_pad_slp_sel = slp_sel;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_pause_watchdog(pmu_dev_t *hw, pmu_hp_mode_t mode, bool pause_wdt)
{
    hw->hp_sys[mode].syscntl.dig_pause_wdt = pause_wdt;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_cpu_stall(pmu_dev_t *hw, pmu_hp_mode_t mode, bool cpu_stall)
{
    hw->hp_sys[mode].syscntl.dig_cpu_stall = cpu_stall;
}

/**
 * @brief Set the power domain that needs to be powered down in the clock power
 *
 * @param hw Beginning address of the peripheral registers.
 * @param mode The pmu mode
 * @param flag Clock power domain flag
 *
 * @return None
 */
FORCE_INLINE_ATTR void pmu_ll_hp_set_clk_power(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t xpd_flag)
{
    hw->hp_sys[mode].clk_power.val = xpd_flag;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_xtal_xpd(pmu_dev_t *hw, pmu_hp_mode_t mode, bool xpd_xtal)
{
    hw->hp_sys[mode].xtal.xpd_xtal = xpd_xtal;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_bias_xpd(pmu_dev_t *hw, pmu_hp_mode_t mode, bool xpd_bias)
{
    hw->hp_sys[mode].bias.xpd_bias = xpd_bias;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_dbg_atten(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t value)
{
    hw->hp_sys[mode].bias.dbg_atten = value;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_current_power_off(pmu_dev_t *hw, pmu_hp_mode_t mode, bool off)
{
    hw->hp_sys[mode].bias.pd_cur = off;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_bias_sleep_enable(pmu_dev_t *hw, pmu_hp_mode_t mode, bool en)
{
    hw->hp_sys[mode].bias.bias_sleep = en;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_retention_param(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t param)
{
    hw->hp_sys[mode].backup.val = param;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_sleep_to_active_backup_enable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_ACTIVE].backup.hp_sleep2active_backup_en = 1;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_sleep_to_active_backup_disable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_ACTIVE].backup.hp_sleep2active_backup_en = 0;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_modem_to_active_backup_enable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_ACTIVE].backup.hp_modem2active_backup_en = 1;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_modem_to_active_backup_disable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_ACTIVE].backup.hp_modem2active_backup_en = 0;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_sleep_to_modem_backup_enable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_MODEM].backup.hp_sleep2modem_backup_en = 1;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_sleep_to_modem_backup_disable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_MODEM].backup.hp_sleep2modem_backup_en = 0;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_active_to_sleep_backup_enable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_SLEEP].backup.hp_active2sleep_backup_en = 1;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_active_to_sleep_backup_disable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_SLEEP].backup.hp_active2sleep_backup_en = 0;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_modem_to_sleep_backup_enable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_SLEEP].backup.hp_modem2sleep_backup_en = 1;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_modem_to_sleep_backup_disable(pmu_dev_t *hw)
{
    hw->hp_sys[PMU_MODE_HP_SLEEP].backup.hp_modem2sleep_backup_en = 0;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_backup_icg_func(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t icg_func)
{
    hw->hp_sys[mode].backup_clk = icg_func;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_sysclk_nodiv(pmu_dev_t *hw, pmu_hp_mode_t mode, bool sysclk_nodiv)
{
    hw->hp_sys[mode].sysclk.dig_sysclk_nodiv = sysclk_nodiv;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_icg_sysclk_enable(pmu_dev_t *hw, pmu_hp_mode_t mode, bool icg_sysclk_en)
{
    hw->hp_sys[mode].sysclk.icg_sysclk_en = icg_sysclk_en;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_sysclk_slp_sel(pmu_dev_t *hw, pmu_hp_mode_t mode, bool slp_sel)
{
    hw->hp_sys[mode].sysclk.sysclk_slp_sel = slp_sel;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_icg_sysclk_slp_sel(pmu_dev_t *hw, pmu_hp_mode_t mode, bool slp_sel)
{
    hw->hp_sys[mode].sysclk.icg_slp_sel = slp_sel;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_dig_sysclk(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t sysclk_sel)
{
    hw->hp_sys[mode].sysclk.dig_sysclk_sel = sysclk_sel;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_regulator_sleep_logic_xpd(pmu_dev_t *hw, pmu_hp_mode_t mode, bool slp_xpd)
{
    hw->hp_sys[mode].regulator0.slp_logic_xpd = slp_xpd;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_regulator_sleep_memory_xpd(pmu_dev_t *hw, pmu_hp_mode_t mode, bool slp_xpd)
{
    hw->hp_sys[mode].regulator0.slp_mem_xpd = slp_xpd;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_regulator_xpd(pmu_dev_t *hw, pmu_hp_mode_t mode, bool xpd)
{
    hw->hp_sys[mode].regulator0.xpd = xpd;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_regulator_sleep_logic_dbias(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t slp_dbias)
{
    hw->hp_sys[mode].regulator0.slp_logic_dbias = slp_dbias;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_regulator_sleep_memory_dbias(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t slp_dbias)
{
    hw->hp_sys[mode].regulator0.slp_mem_dbias = slp_dbias;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_regulator_dbias(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t dbias)
{
    hw->hp_sys[mode].regulator0.dbias = dbias;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_regulator_driver_bar(pmu_dev_t *hw, pmu_hp_mode_t mode, uint32_t drv_b)
{
    hw->hp_sys[mode].regulator1.drv_b = drv_b;
}


FORCE_INLINE_ATTR void pmu_ll_lp_set_regulator_slp_xpd(pmu_dev_t *hw, pmu_lp_mode_t mode, bool slp_xpd)
{
    hw->lp_sys[mode].regulator0.slp_xpd = slp_xpd;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_regulator_xpd(pmu_dev_t *hw, pmu_lp_mode_t mode, bool xpd)
{
    hw->lp_sys[mode].regulator0.xpd = xpd;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_regulator_sleep_dbias(pmu_dev_t *hw, pmu_lp_mode_t mode, uint32_t slp_dbias)
{
    hw->lp_sys[mode].regulator0.slp_dbias = slp_dbias;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_regulator_dbias(pmu_dev_t *hw, pmu_lp_mode_t mode, uint32_t dbias)
{
    hw->lp_sys[mode].regulator0.dbias = dbias;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_regulator_driver_bar(pmu_dev_t *hw, pmu_lp_mode_t mode, uint32_t drv_b)
{
    hw->lp_sys[mode].regulator1.drv_b = drv_b;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_xtal_xpd(pmu_dev_t *hw, pmu_lp_mode_t mode, bool xpd_xtal)
{
    HAL_ASSERT(mode == PMU_MODE_LP_SLEEP);
    hw->lp_sys[mode].xtal.xpd_xtal = xpd_xtal;
}


FORCE_INLINE_ATTR void pmu_ll_lp_set_dig_power(pmu_dev_t *hw, pmu_lp_mode_t mode, uint32_t flag)
{
    hw->lp_sys[mode].dig_power.val = flag;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_clk_power(pmu_dev_t *hw, pmu_lp_mode_t mode, uint32_t xpd_flag)
{
    hw->lp_sys[mode].clk_power.val = xpd_flag;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_lp_get_clk_power(pmu_dev_t *hw, pmu_lp_mode_t mode)
{
    return hw->lp_sys[mode].clk_power.val;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_bias_xpd(pmu_dev_t *hw, pmu_lp_mode_t mode, bool xpd_bias)
{
    HAL_ASSERT(mode == PMU_MODE_LP_SLEEP);
    hw->lp_sys[mode].bias.xpd_bias = xpd_bias;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_dbg_atten(pmu_dev_t *hw, pmu_lp_mode_t mode, uint32_t value)
{
    HAL_ASSERT(mode == PMU_MODE_LP_SLEEP);
    hw->lp_sys[mode].bias.dbg_atten = value;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_current_power_off(pmu_dev_t *hw, pmu_lp_mode_t mode, bool off)
{
    HAL_ASSERT(mode == PMU_MODE_LP_SLEEP);
    hw->lp_sys[mode].bias.pd_cur = off;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_bias_sleep_enable(pmu_dev_t *hw, pmu_lp_mode_t mode, bool en)
{
    HAL_ASSERT(mode == PMU_MODE_LP_SLEEP);
    hw->lp_sys[mode].bias.bias_sleep = en;
}


/****/
FORCE_INLINE_ATTR void pmu_ll_imm_set_clk_power(pmu_dev_t *hw, uint32_t flag)
{
    hw->imm.clk_power.val = flag;
}

FORCE_INLINE_ATTR void pmu_ll_imm_set_icg_slp_sel(pmu_dev_t *hw, bool slp_sel)
{
    if (slp_sel) {
        hw->imm.sleep_sysclk.tie_high_icg_slp_sel = 1;
    } else {
        hw->imm.sleep_sysclk.tie_low_icg_slp_sel = 1;
    }
}

FORCE_INLINE_ATTR void pmu_ll_imm_update_dig_sysclk_sel(pmu_dev_t *hw, bool update)
{
    hw->imm.sleep_sysclk.update_dig_sysclk_sel = update;
}

FORCE_INLINE_ATTR void pmu_ll_imm_update_dig_icg_switch(pmu_dev_t *hw, bool update)
{
    hw->imm.sleep_sysclk.update_dig_icg_switch = update;
}

FORCE_INLINE_ATTR void pmu_ll_imm_update_dig_icg_func(pmu_dev_t *hw, bool icg_func_update)
{
    hw->imm.hp_func_icg.update_dig_icg_func_en = icg_func_update;
}

FORCE_INLINE_ATTR void pmu_ll_imm_update_dig_icg_apb(pmu_dev_t *hw, bool icg_apb_update)
{
    hw->imm.hp_apb_icg.update_dig_icg_apb_en = icg_apb_update;
}

FORCE_INLINE_ATTR void pmu_ll_imm_update_dig_icg_modem_code(pmu_dev_t *hw, bool icg_modem_update)
{
    hw->imm.modem_icg.update_dig_icg_modem_en = icg_modem_update;
}

FORCE_INLINE_ATTR void pmu_ll_imm_set_lp_rootclk_sel(pmu_dev_t *hw, bool rootclk_sel)
{
    if (rootclk_sel) {
        hw->imm.lp_icg.tie_high_lp_rootclk_sel = 1;
    } else {
        hw->imm.lp_icg.tie_low_lp_rootclk_sel = 1;
    }
}

FORCE_INLINE_ATTR void pmu_ll_imm_set_hp_pad_hold_all(pmu_dev_t *hw, bool hold_all)
{
    if (hold_all) {
        hw->imm.pad_hold_all.tie_high_hp_pad_hold_all = 1;
    } else {
        hw->imm.pad_hold_all.tie_low_hp_pad_hold_all = 1;
    }
}

FORCE_INLINE_ATTR void pmu_ll_imm_set_lp_pad_hold_all(pmu_dev_t *hw, bool hold_all)
{
    if (hold_all) {
        hw->imm.pad_hold_all.tie_high_lp_pad_hold_all = 1;
    } else {
        hw->imm.pad_hold_all.tie_low_lp_pad_hold_all = 1;
    }
}

/*** */
FORCE_INLINE_ATTR void pmu_ll_hp_set_power_force_reset(pmu_dev_t *hw, pmu_hp_power_domain_t domain, bool rst)
{
    hw->power.hp_pd[domain].force_reset = rst;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_power_force_isolate(pmu_dev_t *hw, pmu_hp_power_domain_t domain, bool iso)
{
    hw->power.hp_pd[domain].force_iso = iso;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_power_force_power_up(pmu_dev_t *hw, pmu_hp_power_domain_t domain, bool fpu)
{
    hw->power.hp_pd[domain].force_pu = fpu;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_power_force_no_reset(pmu_dev_t *hw, pmu_hp_power_domain_t domain, bool no_rst)
{
    hw->power.hp_pd[domain].force_no_reset = no_rst;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_power_force_no_isolate(pmu_dev_t *hw, pmu_hp_power_domain_t domain, bool no_iso)
{
    hw->power.hp_pd[domain].force_no_iso = no_iso;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_power_force_power_down(pmu_dev_t *hw, pmu_hp_power_domain_t domain, bool fpd)
{
    hw->power.hp_pd[domain].force_pd = fpd;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_power_force_reset(pmu_dev_t *hw, bool rst)
{
    hw->power.lp_peri.force_reset = rst;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_power_force_isolate(pmu_dev_t *hw, bool iso)
{
    hw->power.lp_peri.force_iso = iso;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_power_force_power_up(pmu_dev_t *hw, bool fpu)
{
    hw->power.lp_peri.force_pu = fpu;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_power_force_no_reset(pmu_dev_t *hw, bool no_rst)
{
    hw->power.lp_peri.force_no_reset = no_rst;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_power_force_no_isolate(pmu_dev_t *hw, bool no_iso)
{
    hw->power.lp_peri.force_no_iso = no_iso;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_power_force_power_down(pmu_dev_t *hw, bool fpd)
{
    hw->power.lp_peri.force_pd = fpd;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_memory_isolate(pmu_dev_t *hw, uint32_t iso)
{
    hw->power.mem_cntl.force_hp_mem_iso = iso;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_memory_power_down(pmu_dev_t *hw, uint32_t fpd)
{
    hw->power.mem_cntl.force_hp_mem_pd = fpd;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_memory_no_isolate(pmu_dev_t *hw, uint32_t no_iso)
{
    hw->power.mem_cntl.force_hp_mem_no_iso = no_iso;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_memory_power_up(pmu_dev_t *hw, uint32_t fpu)
{
    hw->power.mem_cntl.force_hp_mem_pu = fpu;
}

/*** */
FORCE_INLINE_ATTR void pmu_ll_hp_set_sleep_enable(pmu_dev_t *hw)
{
    hw->wakeup.cntl0.sleep_req = 1;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_reject_enable(pmu_dev_t *hw, uint32_t reject)
{
    hw->wakeup.cntl1.sleep_reject_ena = reject;
    hw->wakeup.cntl1.slp_reject_en = 1;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_reject_disable(pmu_dev_t *hw)
{
    hw->wakeup.cntl1.slp_reject_en = 0;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_wakeup_enable(pmu_dev_t *hw, uint32_t wakeup)
{
    hw->wakeup.cntl2 = wakeup;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_sleep_protect_mode(pmu_dev_t *hw, int mode)
{
    hw->wakeup.cntl3.sleep_prt_sel = mode;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_min_sleep_cycle(pmu_dev_t *hw, uint32_t slow_clk_cycle)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->wakeup.cntl3, hp_min_slp_val, slow_clk_cycle);
}

FORCE_INLINE_ATTR void pmu_ll_hp_clear_reject_cause(pmu_dev_t *hw)
{
    hw->wakeup.cntl4.slp_reject_cause_clr = 1;
}

FORCE_INLINE_ATTR bool pmu_ll_hp_is_sleep_wakeup(pmu_dev_t *hw)
{
    return (hw->hp_ext.int_raw.wakeup == 1);
}

FORCE_INLINE_ATTR bool pmu_ll_hp_is_sleep_reject(pmu_dev_t *hw)
{
    return (hw->hp_ext.int_raw.reject == 1);
}

FORCE_INLINE_ATTR void pmu_ll_hp_clear_sw_intr_status(pmu_dev_t *hw)
{
    hw->hp_ext.int_clr.sw = 1;
}

FORCE_INLINE_ATTR void pmu_ll_hp_clear_wakeup_intr_status(pmu_dev_t *hw)
{
    hw->hp_ext.int_clr.wakeup = 1;
}

FORCE_INLINE_ATTR void pmu_ll_hp_clear_reject_intr_status(pmu_dev_t *hw)
{
    hw->hp_ext.int_clr.reject = 1;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_hp_get_wakeup_cause(pmu_dev_t *hw)
{
    return hw->wakeup.status0;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_hp_get_reject_cause(pmu_dev_t *hw)
{
    return hw->wakeup.status1;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_lp_get_interrupt_raw(pmu_dev_t *hw)
{
    return hw->lp_ext.int_raw.val;
}

FORCE_INLINE_ATTR void pmu_ll_lp_clear_intsts_mask(pmu_dev_t *hw, uint32_t mask)
{
    hw->lp_ext.int_clr.val = mask;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_min_sleep_cycle(pmu_dev_t *hw, uint32_t slow_clk_cycle)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->wakeup.cntl3, lp_min_slp_val, slow_clk_cycle);
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_modify_icg_cntl_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hp_ext.clk_cntl, modify_icg_cntl_wait, cycle);
}

FORCE_INLINE_ATTR uint32_t pmu_ll_hp_get_modify_icg_cntl_wait_cycle(pmu_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->hp_ext.clk_cntl, modify_icg_cntl_wait);
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_switch_icg_cntl_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hp_ext.clk_cntl, switch_icg_cntl_wait, cycle);
}

FORCE_INLINE_ATTR uint32_t pmu_ll_hp_get_switch_icg_cntl_wait_cycle(pmu_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->hp_ext.clk_cntl, switch_icg_cntl_wait);
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_digital_power_down_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    hw->power.wait_timer0.powerdown_timer = cycle;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_hp_get_digital_power_down_wait_cycle(pmu_dev_t *hw)
{
    return hw->power.wait_timer0.powerdown_timer;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_digital_power_down_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    hw->power.wait_timer1.powerdown_timer = cycle;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_lp_get_digital_power_down_wait_cycle(pmu_dev_t *hw)
{
    return hw->power.wait_timer1.powerdown_timer;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_isolate_wait_cycle(pmu_dev_t *hw, uint32_t isolate_wait_cycle)
{
    hw->power.wait_timer2.lp_iso_wait_timer = isolate_wait_cycle;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_reset_wait_cycle(pmu_dev_t *hw, uint32_t reset_wait_cycle)
{
    hw->power.wait_timer2.lp_rst_wait_timer = reset_wait_cycle;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_isolate_wait_cycle(pmu_dev_t *hw, uint32_t isolate_wait_cycle)
{
    hw->power.wait_timer2.hp_iso_wait_timer = isolate_wait_cycle;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_reset_wait_cycle(pmu_dev_t *hw, uint32_t reset_wait_cycle)
{
    hw->power.wait_timer2.hp_rst_wait_timer = reset_wait_cycle;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_analog_wait_target_cycle(pmu_dev_t *hw, uint32_t slow_clk_cycle)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->wakeup.cntl5, lp_ana_wait_target, slow_clk_cycle);
}

FORCE_INLINE_ATTR uint32_t pmu_ll_lp_get_analog_wait_target_cycle(pmu_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->wakeup.cntl5, lp_ana_wait_target);
}

FORCE_INLINE_ATTR void pmu_ll_set_modem_wait_target_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    hw->wakeup.cntl5.modem_wait_target = cycle;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_get_modem_wait_target_cycle(pmu_dev_t *hw)
{
    return hw->wakeup.cntl5.modem_wait_target;
}

FORCE_INLINE_ATTR void pmu_ll_set_xtal_stable_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->power.clk_wait, wait_xtal_stable, cycle);
}

FORCE_INLINE_ATTR uint32_t pmu_ll_get_xtal_stable_wait_cycle(pmu_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->power.clk_wait, wait_xtal_stable);
}

FORCE_INLINE_ATTR void pmu_ll_set_pll_stable_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->power.clk_wait, wait_pll_stable, cycle);
}

FORCE_INLINE_ATTR uint32_t pmu_ll_get_pll_stable_wait_cycle(pmu_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->power.clk_wait, wait_pll_stable);
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_digital_power_supply_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    hw->power.wait_timer1.wait_timer = cycle;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_lp_get_digital_power_supply_wait_cycle(pmu_dev_t *hw)
{
    return hw->power.wait_timer1.wait_timer;
}

FORCE_INLINE_ATTR void pmu_ll_lp_set_digital_power_up_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    hw->power.wait_timer1.powerup_timer = cycle;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_lp_get_digital_power_up_wait_cycle(pmu_dev_t *hw)
{
    return hw->power.wait_timer1.powerup_timer;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_analog_wait_target_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->wakeup.cntl7, ana_wait_target, cycle);
}

FORCE_INLINE_ATTR uint32_t pmu_ll_hp_get_analog_wait_target_cycle(pmu_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->wakeup.cntl7, ana_wait_target);
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_digital_power_supply_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    hw->power.wait_timer0.wait_timer = cycle;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_hp_get_digital_power_supply_wait_cycle(pmu_dev_t *hw)
{
    return hw->power.wait_timer0.wait_timer;
}

FORCE_INLINE_ATTR void pmu_ll_hp_set_digital_power_up_wait_cycle(pmu_dev_t *hw, uint32_t cycle)
{
    hw->power.wait_timer0.powerup_timer = cycle;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_hp_get_digital_power_up_wait_cycle(pmu_dev_t *hw)
{
    return hw->power.wait_timer0.powerup_timer;
}

FORCE_INLINE_ATTR uint32_t pmu_ll_get_sysclk_sleep_select_state(pmu_dev_t *hw)
{
    return hw->clk_state0.sysclk_slp_sel;
}

#ifdef __cplusplus
}
#endif
