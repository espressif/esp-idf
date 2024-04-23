/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <sys/param.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp32p4/rom/cache.h"
#include "soc/soc.h"
#include "soc/regi2c_syspll.h"
#include "soc/regi2c_cpll.h"
#include "soc/rtc.h"
#include "soc/cache_reg.h"
#include "soc/pau_reg.h"
#include "soc/pmu_reg.h"
#include "soc/pmu_struct.h"
#include "hal/lp_aon_hal.h"
#include "soc/lp_system_reg.h"
#include "hal/pmu_hal.h"
#include "hal/lp_sys_ll.h"
#include "esp_private/esp_pmu.h"
#include "pmu_param.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"

#define HP(state)   (PMU_MODE_HP_ ## state)
#define LP(state)   (PMU_MODE_LP_ ## state)


static bool s_pmu_sleep_regdma_backup_enabled;

void pmu_sleep_enable_regdma_backup(void)
{
    if(!s_pmu_sleep_regdma_backup_enabled){
        assert(PMU_instance()->hal);
        /* entry 0, 1, 2 is used by pmu HP_SLEEP and HP_ACTIVE, HP_SLEEP
         * and HP_MODEM or HP_MODEM and HP_ACTIVE states switching,
         * respectively. entry 3 is reserved, not used yet! */
        pmu_hal_hp_set_sleep_active_backup_enable(PMU_instance()->hal);
        s_pmu_sleep_regdma_backup_enabled = true;
    }
}

void pmu_sleep_disable_regdma_backup(void)
{
    if(s_pmu_sleep_regdma_backup_enabled){
        assert(PMU_instance()->hal);
        pmu_hal_hp_set_sleep_active_backup_disable(PMU_instance()->hal);
        s_pmu_sleep_regdma_backup_enabled = false;
    }
}

uint32_t pmu_sleep_calculate_lp_hw_wait_time(uint32_t pd_flags, uint32_t slowclk_period, uint32_t fastclk_period)
{
    const pmu_sleep_machine_constant_t *mc = (pmu_sleep_machine_constant_t *)PMU_instance()->mc;
    /* LP core hardware wait time, microsecond */
    const int lp_wakeup_wait_time_us        = rtc_time_slowclk_to_us(mc->lp.wakeup_wait_cycle, slowclk_period);
    const int lp_clk_switch_time_us         = rtc_time_slowclk_to_us(mc->lp.clk_switch_cycle, slowclk_period);
    const int lp_clk_power_on_wait_time_us  = (pd_flags & PMU_SLEEP_PD_XTAL) ? mc->lp.xtal_wait_stable_time_us \
                            : rtc_time_slowclk_to_us(mc->lp.clk_power_on_wait_cycle, slowclk_period);

    const int lp_hw_wait_time_us = mc->lp.min_slp_time_us + mc->lp.analog_wait_time_us + lp_clk_power_on_wait_time_us \
                            + lp_wakeup_wait_time_us + lp_clk_switch_time_us + mc->lp.power_supply_wait_time_us \
                                + mc->lp.power_up_wait_time_us;

    return (uint32_t)lp_hw_wait_time_us;
}

uint32_t pmu_sleep_calculate_hp_hw_wait_time(uint32_t pd_flags, uint32_t slowclk_period, uint32_t fastclk_period)
{
    pmu_sleep_machine_constant_t *mc = (pmu_sleep_machine_constant_t *)PMU_instance()->mc;
    /* HP core hardware wait time, microsecond */
    const int hp_digital_power_up_wait_time_us = mc->hp.power_supply_wait_time_us + mc->hp.power_up_wait_time_us;
    const int hp_regdma_wait_time_us = (pd_flags & PMU_SLEEP_PD_TOP) ?  mc->hp.regdma_s2a_work_time_us : 0;
    const int hp_clock_wait_time_us = mc->hp.xtal_wait_stable_time_us + mc->hp.pll_wait_stable_time_us;

    if (pd_flags & PMU_SLEEP_PD_TOP) {
        mc->hp.analog_wait_time_us = PMU_HP_ANA_WAIT_TIME_PD_TOP_US;
    } else {
        mc->hp.analog_wait_time_us = PMU_HP_ANA_WAIT_TIME_PU_TOP_US;
    }

    const int hp_hw_wait_time_us = mc->hp.analog_wait_time_us + MAX(hp_digital_power_up_wait_time_us + hp_regdma_wait_time_us, hp_clock_wait_time_us);
    return (uint32_t)hp_hw_wait_time_us;
}

uint32_t pmu_sleep_calculate_hw_wait_time(uint32_t pd_flags, uint32_t slowclk_period, uint32_t fastclk_period)
{
    const uint32_t lp_hw_wait_time_us = pmu_sleep_calculate_lp_hw_wait_time(pd_flags, slowclk_period, fastclk_period);
    const uint32_t hp_hw_wait_time_us = pmu_sleep_calculate_hp_hw_wait_time(pd_flags, slowclk_period, fastclk_period);
    const uint32_t total_hw_wait_time_us = lp_hw_wait_time_us + hp_hw_wait_time_us;
    return total_hw_wait_time_us;
}

#define rtc_time_us_to_fastclk(time_us, period)     rtc_time_us_to_slowclk((time_us), (period))

static inline pmu_sleep_param_config_t * pmu_sleep_param_config_default(
        pmu_sleep_param_config_t *param,
        pmu_sleep_power_config_t *power, /* We'll use the runtime power parameter to determine some hardware parameters */
        const uint32_t pd_flags,
        const uint32_t adjustment,
        const uint32_t slowclk_period,
        const uint32_t fastclk_period
    )
{
    const pmu_sleep_machine_constant_t *mc = (pmu_sleep_machine_constant_t *)PMU_instance()->mc;

    param->hp_sys.min_slp_slow_clk_cycle          = rtc_time_us_to_slowclk(mc->hp.min_slp_time_us, slowclk_period);
    param->hp_sys.analog_wait_target_cycle        = rtc_time_us_to_slowclk(mc->hp.analog_wait_time_us, slowclk_period);
    param->hp_sys.digital_power_supply_wait_cycle = rtc_time_us_to_fastclk(mc->hp.power_supply_wait_time_us, fastclk_period);
    param->hp_sys.digital_power_up_wait_cycle     = rtc_time_us_to_fastclk(mc->hp.power_up_wait_time_us, fastclk_period);
    param->hp_sys.pll_stable_wait_cycle           = rtc_time_us_to_fastclk(mc->hp.pll_wait_stable_time_us, fastclk_period);

    param->lp_sys.min_slp_slow_clk_cycle          = rtc_time_us_to_slowclk(mc->lp.min_slp_time_us, slowclk_period);
    param->lp_sys.analog_wait_target_cycle        = rtc_time_us_to_slowclk(mc->lp.analog_wait_time_us, slowclk_period);
    param->lp_sys.digital_power_supply_wait_cycle = rtc_time_us_to_fastclk(mc->lp.power_supply_wait_time_us, fastclk_period);
    param->lp_sys.digital_power_up_wait_cycle     = rtc_time_us_to_fastclk(mc->lp.power_up_wait_time_us, fastclk_period);

    if (power->hp_sys.xtal.xpd_xtal) {
        param->hp_lp.xtal_stable_wait_slow_clk_cycle = rtc_time_us_to_slowclk(mc->lp.xtal_wait_stable_time_us, slowclk_period);
    } else {
        param->hp_lp.xtal_stable_wait_cycle       = rtc_time_us_to_fastclk(mc->hp.xtal_wait_stable_time_us, fastclk_period);
    }
    return param;
}

const pmu_sleep_config_t* pmu_sleep_config_default(
        pmu_sleep_config_t *config,
        uint32_t pd_flags,
        uint32_t adjustment,
        uint32_t slowclk_period,
        uint32_t fastclk_period,
        bool dslp
    )
{
    pmu_sleep_power_config_t power_default = PMU_SLEEP_POWER_CONFIG_DEFAULT(pd_flags);

    uint32_t iram_pd_flags = 0;
    iram_pd_flags |= (pd_flags & PMU_SLEEP_PD_MEM_G0) ? BIT(0) : 0;
    iram_pd_flags |= (pd_flags & PMU_SLEEP_PD_MEM_G1) ? BIT(1) : 0;
    iram_pd_flags |= (pd_flags & PMU_SLEEP_PD_MEM_G2) ? BIT(2) : 0;
    iram_pd_flags |= (pd_flags & PMU_SLEEP_PD_MEM_G3) ? BIT(3) : 0;
    config->power = power_default;

    pmu_sleep_param_config_t param_default = PMU_SLEEP_PARAM_CONFIG_DEFAULT(pd_flags);
    config->param = *pmu_sleep_param_config_default(&param_default, &power_default, pd_flags, adjustment, slowclk_period, fastclk_period);

    if (dslp) {
        config->param.lp_sys.analog_wait_target_cycle  = rtc_time_us_to_slowclk(PMU_LP_ANALOG_WAIT_TARGET_TIME_DSLP_US, slowclk_period);
        pmu_sleep_analog_config_t analog_default = PMU_SLEEP_ANALOG_DSLP_CONFIG_DEFAULT(pd_flags);
        config->analog = analog_default;
    } else {
        pmu_sleep_digital_config_t digital_default = PMU_SLEEP_DIGITAL_LSLP_CONFIG_DEFAULT(pd_flags);
        config->digital = digital_default;

        pmu_sleep_analog_config_t analog_default = PMU_SLEEP_ANALOG_LSLP_CONFIG_DEFAULT(pd_flags);
#if CONFIG_SPIRAM
        analog_default.hp_sys.analog.pd_cur = 1;
        analog_default.lp_sys[PMU_MODE_LP_SLEEP].analog.pd_cur = 1;
#endif
        config->analog = analog_default;
    }
    return config;
}

static void pmu_sleep_power_init(pmu_context_t *ctx, const pmu_sleep_power_config_t *power, bool dslp)
{
    pmu_ll_hp_set_dig_power(ctx->hal->dev, HP(SLEEP), power->hp_sys.dig_power.val);
    pmu_ll_hp_set_clk_power(ctx->hal->dev, HP(SLEEP), power->hp_sys.clk_power.val);
    pmu_ll_hp_set_xtal_xpd (ctx->hal->dev, HP(SLEEP), power->hp_sys.xtal.xpd_xtal);

    pmu_ll_lp_set_dig_power(ctx->hal->dev, LP(ACTIVE), power->lp_sys[LP(ACTIVE)].dig_power.val);
    pmu_ll_lp_set_clk_power(ctx->hal->dev, LP(ACTIVE), power->lp_sys[LP(ACTIVE)].clk_power.val);

    pmu_ll_lp_set_dig_power(ctx->hal->dev, LP(SLEEP), power->lp_sys[LP(SLEEP)].dig_power.val);
    pmu_ll_lp_set_clk_power(ctx->hal->dev, LP(SLEEP), power->lp_sys[LP(SLEEP)].clk_power.val);
    pmu_ll_lp_set_xtal_xpd (ctx->hal->dev, LP(SLEEP), power->lp_sys[LP(SLEEP)].xtal.xpd_xtal);
}

static void pmu_sleep_digital_init(pmu_context_t *ctx, const pmu_sleep_digital_config_t *dig)
{
    pmu_ll_hp_set_dig_pad_slp_sel   (ctx->hal->dev, HP(SLEEP), dig->syscntl.dig_pad_slp_sel);
}

static void pmu_sleep_analog_init(pmu_context_t *ctx, const pmu_sleep_analog_config_t *analog, bool dslp)
{
    assert(ctx->hal);
    pmu_ll_hp_set_current_power_off           (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.pd_cur);
    pmu_ll_hp_set_bias_sleep_enable           (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.bias_sleep);
    pmu_ll_hp_set_regulator_sleep_memory_xpd  (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.slp_mem_xpd);
    pmu_ll_hp_set_regulator_sleep_logic_xpd   (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.slp_logic_xpd);
    pmu_ll_hp_set_regulator_xpd               (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.xpd);
    pmu_ll_hp_set_regulator_sleep_memory_dbias(ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.slp_mem_dbias);
    pmu_ll_hp_set_regulator_sleep_logic_dbias (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.slp_logic_dbias);
    pmu_ll_hp_set_dbg_atten                   (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.dbg_atten);
    pmu_ll_hp_set_regulator_dbias             (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.dbias);
    pmu_ll_hp_set_regulator_driver_bar        (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.drv_b);

    pmu_ll_lp_set_regulator_sleep_dbias(ctx->hal->dev, LP(ACTIVE), analog->lp_sys[LP(ACTIVE)].analog.slp_dbias);
    pmu_ll_lp_set_regulator_dbias      (ctx->hal->dev, LP(ACTIVE), analog->lp_sys[LP(ACTIVE)].analog.dbias);
    pmu_ll_lp_set_regulator_driver_bar (ctx->hal->dev, LP(ACTIVE), analog->lp_sys[LP(ACTIVE)].analog.drv_b);

    pmu_ll_lp_set_current_power_off    (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.pd_cur);
    pmu_ll_lp_set_bias_sleep_enable    (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.bias_sleep);
    pmu_ll_lp_set_regulator_xpd        (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.xpd);
    pmu_ll_lp_set_regulator_slp_xpd    (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.slp_xpd);
    pmu_ll_lp_set_regulator_sleep_dbias(ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.slp_dbias);
    pmu_ll_lp_set_regulator_dbias      (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.dbias);
    pmu_ll_lp_set_dbg_atten            (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.dbg_atten);
    pmu_ll_lp_set_regulator_driver_bar (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.drv_b);

    pmu_ll_lp_set_regulator_slp_xpd    (ctx->hal->dev, LP(ACTIVE), analog->lp_sys[LP(ACTIVE)].analog.slp_xpd);
    pmu_ll_lp_set_regulator_xpd        (ctx->hal->dev, LP(ACTIVE), analog->lp_sys[LP(ACTIVE)].analog.xpd);
}

static void pmu_sleep_param_init(pmu_context_t *ctx, const pmu_sleep_param_config_t *param, bool dslp)
{
    assert(ctx->hal);
    pmu_ll_hp_set_min_sleep_cycle(ctx->hal->dev, param->hp_sys.min_slp_slow_clk_cycle);
    pmu_ll_lp_set_min_sleep_cycle(ctx->hal->dev, param->lp_sys.min_slp_slow_clk_cycle);

    pmu_ll_hp_set_analog_wait_target_cycle(ctx->hal->dev, param->hp_sys.analog_wait_target_cycle);
    pmu_ll_lp_set_analog_wait_target_cycle(ctx->hal->dev, param->lp_sys.analog_wait_target_cycle);

    pmu_hal_hp_set_digital_power_up_wait_cycle(ctx->hal, param->hp_sys.digital_power_supply_wait_cycle, param->hp_sys.digital_power_up_wait_cycle);
    pmu_hal_lp_set_digital_power_up_wait_cycle(ctx->hal, param->lp_sys.digital_power_supply_wait_cycle, param->lp_sys.digital_power_up_wait_cycle);

    pmu_ll_set_xtal_stable_wait_cycle(ctx->hal->dev, param->hp_lp.xtal_stable_wait_slow_clk_cycle);
    pmu_ll_set_pll_stable_wait_cycle(ctx->hal->dev, param->hp_sys.pll_stable_wait_cycle);
}

void pmu_sleep_init(const pmu_sleep_config_t *config, bool dslp)
{
    assert(PMU_instance());
    pmu_sleep_power_init(PMU_instance(), &config->power, dslp);
    if(!dslp){
        pmu_sleep_digital_init(PMU_instance(), &config->digital);
    }
    pmu_sleep_analog_init(PMU_instance(), &config->analog, dslp);
    pmu_sleep_param_init(PMU_instance(), &config->param, dslp);

    // When light sleep (PD_TOP), the PAU will power down. so need use LP_SYS_BACKUP_DMA_CFG2_REG to store recover link address.
    if (!dslp && PMU.hp_sys[PMU_MODE_HP_SLEEP].dig_power.top_pd_en) {
        if (PMU.hp_sys[PMU_MODE_HP_SLEEP].backup.hp_active2sleep_backup_en ||
            PMU.hp_sys[PMU_MODE_HP_ACTIVE].backup.hp_sleep2active_backup_en) {
            uint32_t link_sel = PMU.hp_sys[PMU_MODE_HP_SLEEP].backup.hp_active2sleep_backup_mode & 0x3;
            uint32_t link_addr = REG_READ(PAU_REGDMA_LINK_0_ADDR_REG + link_sel * 4);
            lp_sys_ll_set_pau_link_addr(link_addr);
            pmu_sleep_enable_regdma_backup();
        }
    } else {
        pmu_sleep_disable_regdma_backup();
    }
}

void pmu_sleep_increase_ldo_volt(void) {
    REG_SET_FIELD(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_ACTIVE_HP_REGULATOR_DBIAS, 30);
    REG_SET_BIT(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_ACTIVE_HP_REGULATOR_XPD);
    // Decrease the DCDC voltage to reduce the voltage difference between the DCDC and the LDO to avoid overshooting the DCDC voltage during wake-up.
    REG_SET_FIELD(PMU_HP_ACTIVE_BIAS_REG, PMU_HP_ACTIVE_DCM_VSET, 24);
}

void pmu_sleep_shutdown_dcdc(void) {
    SET_PERI_REG_MASK(LP_SYSTEM_REG_SYS_CTRL_REG, LP_SYSTEM_REG_LP_FIB_DCDC_SWITCH); //0: enable, 1: disable
    REG_SET_BIT(PMU_DCM_CTRL_REG, PMU_DCDC_OFF_REQ);
    // Decrease hp_ldo voltage.
    REG_SET_FIELD(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_ACTIVE_HP_REGULATOR_DBIAS, 24);
}

void pmu_sleep_enable_dcdc(void) {
    CLEAR_PERI_REG_MASK(LP_SYSTEM_REG_SYS_CTRL_REG, LP_SYSTEM_REG_LP_FIB_DCDC_SWITCH); //0: enable, 1: disable
    SET_PERI_REG_MASK(PMU_DCM_CTRL_REG, PMU_DCDC_ON_REQ);
    REG_SET_FIELD(PMU_HP_ACTIVE_BIAS_REG, PMU_HP_ACTIVE_DCM_VSET, 27);
}

void pmu_sleep_shutdown_ldo(void) {
    CLEAR_PERI_REG_MASK(LP_SYSTEM_REG_SYS_CTRL_REG, LP_SYSTEM_REG_LP_FIB_DCDC_SWITCH); //0: enable, 1: disable
    CLEAR_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_ACTIVE_HP_REGULATOR_XPD);
}

FORCE_INLINE_ATTR void sleep_writeback_l1_dcache(void) {
    Cache_WriteBack_All(CACHE_MAP_L1_DCACHE);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE));
}

TCM_IRAM_ATTR uint32_t pmu_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp)
{
    lp_aon_hal_inform_wakeup_type(dslp);

    assert(PMU_instance()->hal);
    pmu_ll_hp_set_wakeup_enable(PMU_instance()->hal->dev, wakeup_opt);
    pmu_ll_hp_set_reject_enable(PMU_instance()->hal->dev, reject_opt);

    pmu_ll_hp_clear_wakeup_intr_status(PMU_instance()->hal->dev);
    pmu_ll_hp_clear_reject_intr_status(PMU_instance()->hal->dev);
    pmu_ll_hp_clear_reject_cause(PMU_instance()->hal->dev);

    sleep_writeback_l1_dcache();

    /* Start entry into sleep mode */
    pmu_ll_hp_set_sleep_enable(PMU_instance()->hal->dev);

    while (!pmu_ll_hp_is_sleep_wakeup(PMU_instance()->hal->dev) &&
        !pmu_ll_hp_is_sleep_reject(PMU_instance()->hal->dev)) {
        ;
    }

    return pmu_sleep_finish();
}

TCM_IRAM_ATTR bool pmu_sleep_finish(void)
{
    REG_SET_FIELD(PMU_HP_ACTIVE_BIAS_REG, PMU_HP_ACTIVE_DCM_VSET, 27);
    if (pmu_ll_hp_is_sleep_reject(PMU_instance()->hal->dev)) {
        // If sleep is rejected, the hardware wake-up process that turns on DCDC
        // is skipped, and software is used to enable DCDC here.
        pmu_sleep_enable_dcdc();
        esp_rom_delay_us(950);
    }
    pmu_sleep_shutdown_ldo();

    REGI2C_WRITE_MASK(I2C_CPLL, I2C_CPLL_OC_DIV_7_0, 6); // lower default cpu_pll freq to 400M
    REGI2C_WRITE_MASK(I2C_SYSPLL, I2C_SYSPLL_OC_DIV_7_0, 8); // lower default sys_pll freq to 480M
    return pmu_ll_hp_is_sleep_reject(PMU_instance()->hal->dev);
}

uint32_t pmu_sleep_get_wakup_retention_cost(void)
{
    return PMU_REGDMA_S2A_WORK_TIME_US;
}
