/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/pmu_struct.h"
#include "hal/lp_aon_hal.h"
#include "esp_private/esp_pmu.h"
#include "pmu_param.h"

#define HP(state)   (PMU_MODE_HP_ ## state)
#define LP(state)   (PMU_MODE_LP_ ## state)

uint32_t get_slp_lp_dbias(void)
{
    // TODO: IDF-12313
    uint32_t pmu_lp_dbias_sleep_0v7 = PMU_LP_DBIAS_SLEEP_0V7_DEFAULT;
    return pmu_lp_dbias_sleep_0v7;
}

static bool s_pmu_sleep_regdma_backup_enabled;

void pmu_sleep_enable_regdma_backup(void)
{
    if(!s_pmu_sleep_regdma_backup_enabled){
        assert(PMU_instance()->hal);
        /* entry 0, 1, 2 is used by pmu HP_SLEEP and HP_ACTIVE, HP_SLEEP
         * and HP_MODEM or HP_MODEM and HP_ACTIVE states switching,
         * respectively. entry 3 is reserved, not used yet! */
        pmu_hal_hp_set_sleep_active_backup_enable(PMU_instance()->hal);
        pmu_hal_hp_set_sleep_modem_backup_enable(PMU_instance()->hal);
        pmu_hal_hp_set_modem_active_backup_enable(PMU_instance()->hal);
        s_pmu_sleep_regdma_backup_enabled = true;
    }
}

void pmu_sleep_disable_regdma_backup(void)
{
    if(s_pmu_sleep_regdma_backup_enabled){
        assert(PMU_instance()->hal);
        pmu_hal_hp_set_sleep_active_backup_disable(PMU_instance()->hal);
        pmu_hal_hp_set_sleep_modem_backup_disable(PMU_instance()->hal);
        pmu_hal_hp_set_modem_active_backup_disable(PMU_instance()->hal);
        s_pmu_sleep_regdma_backup_enabled = false;
    }
}

uint32_t pmu_sleep_calculate_hw_wait_time(uint32_t sleep_flags, soc_rtc_slow_clk_src_t slowclk_src, uint32_t slowclk_period, uint32_t fastclk_period)
{
    pmu_sleep_machine_constant_t *mc = (pmu_sleep_machine_constant_t *)PMU_instance()->mc;

    /* LP core hardware wait time, microsecond */
    const int lp_clk_switch_time_us         = rtc_time_slowclk_to_us(mc->lp.clk_switch_cycle, slowclk_period);
    /* If XTAL is used as RTC_FAST clock source, it is started in LP_SLEEP -> LP_ACTIVE stage and the clock waiting time is counted into lp_hw_wait_time */
    const int lp_clk_power_on_wait_time_us  = ((sleep_flags & PMU_SLEEP_PD_XTAL) && (sleep_flags & RTC_SLEEP_XTAL_AS_RTC_FAST)) \
                                            ? mc->lp.xtal_wait_stable_time_us \
                                            : rtc_time_slowclk_to_us(mc->lp.clk_power_on_wait_cycle, slowclk_period);
    const int lp_hw_wait_time_us = mc->lp.min_slp_time_us + mc->lp.analog_wait_time_us + lp_clk_power_on_wait_time_us \
                                 + lp_clk_switch_time_us + mc->lp.power_supply_wait_time_us + mc->lp.power_up_wait_time_us;

    mc->hp.regdma_s2a_work_time_us = (sleep_flags & PMU_SLEEP_PD_TOP) ? PMU_REGDMA_S2A_WORK_TIME_PD_TOP_US : PMU_REGDMA_S2A_WORK_TIME_PU_TOP_US;

    /* HP core hardware wait time, microsecond */
    const int hp_digital_power_up_wait_time_us = mc->hp.power_supply_wait_time_us + mc->hp.power_up_wait_time_us;
    const int hp_control_wait_time_us = mc->hp.isolate_wait_time_us + mc->hp.reset_wait_time_us;
    const int hp_regdma_wait_time_us = MAX(mc->hp.regdma_s2m_work_time_us + mc->hp.regdma_m2a_work_time_us, mc->hp.regdma_s2a_work_time_us);
    /* If XTAL is not used as RTC_FAST clock source, it is started in HP_SLEEP -> HP_ACTIVE stage and the clock waiting time is counted into hp_hw_wait_time */
    const int hp_clock_wait_time_us = ((sleep_flags & PMU_SLEEP_PD_XTAL) && !(sleep_flags & RTC_SLEEP_XTAL_AS_RTC_FAST)) \
                                    ? mc->hp.xtal_wait_stable_time_us + mc->hp.pll_wait_stable_time_us \
                                    : mc->hp.pll_wait_stable_time_us;
    const int hp_hw_wait_time_us = mc->hp.analog_wait_time_us + hp_digital_power_up_wait_time_us + hp_regdma_wait_time_us + hp_clock_wait_time_us + hp_control_wait_time_us;

    const int rf_on_protect_time_us = 0;
    const int total_hw_wait_time_us = lp_hw_wait_time_us + hp_hw_wait_time_us;

    return total_hw_wait_time_us + rf_on_protect_time_us;
}

#define rtc_time_us_to_fastclk(time_us, period)     rtc_time_us_to_slowclk((time_us), (period))

static inline pmu_sleep_param_config_t * pmu_sleep_param_config_default(
        pmu_sleep_param_config_t *param,
        pmu_sleep_power_config_t *power, /* We'll use the runtime power parameter to determine some hardware parameters */
        const uint32_t sleep_flags,
        const uint32_t adjustment,
        const uint32_t slowclk_period,
        const uint32_t fastclk_period
    )
{
    const pmu_sleep_machine_constant_t *mc = (pmu_sleep_machine_constant_t *)PMU_instance()->mc;

    param->hp_sys.min_slp_slow_clk_cycle          = rtc_time_us_to_slowclk(mc->hp.min_slp_time_us, slowclk_period);
    param->hp_sys.analog_wait_target_cycle        = rtc_time_us_to_fastclk(mc->hp.analog_wait_time_us, fastclk_period);
    param->hp_sys.digital_power_supply_wait_cycle = rtc_time_us_to_fastclk(mc->hp.power_supply_wait_time_us, fastclk_period);
    param->hp_sys.digital_power_up_wait_cycle     = rtc_time_us_to_fastclk(mc->hp.power_up_wait_time_us, fastclk_period);
    param->hp_sys.pll_stable_wait_cycle           = rtc_time_us_to_fastclk(mc->hp.pll_wait_stable_time_us, fastclk_period);
    param->hp_sys.isolate_wait_cycle              = rtc_time_us_to_fastclk(mc->hp.isolate_wait_time_us, fastclk_period);
    param->hp_sys.reset_wait_cycle                = rtc_time_us_to_fastclk(mc->hp.reset_wait_time_us, fastclk_period);

    param->lp_sys.min_slp_slow_clk_cycle          = rtc_time_us_to_slowclk(mc->lp.min_slp_time_us, slowclk_period);
    param->lp_sys.analog_wait_target_cycle        = rtc_time_us_to_slowclk(mc->lp.analog_wait_time_us, slowclk_period);
    param->lp_sys.digital_power_supply_wait_cycle = rtc_time_us_to_fastclk(mc->lp.power_supply_wait_time_us, fastclk_period);
    param->lp_sys.digital_power_up_wait_cycle     = rtc_time_us_to_fastclk(mc->lp.power_up_wait_time_us, fastclk_period);
    param->lp_sys.isolate_wait_cycle              = rtc_time_us_to_fastclk(mc->lp.isolate_wait_time_us, fastclk_period);
    param->lp_sys.reset_wait_cycle                = rtc_time_us_to_fastclk(mc->lp.reset_wait_time_us, fastclk_period);

    if (power->hp_sys.xtal.xpd_xtal) {
        param->hp_lp.xtal_stable_wait_slow_clk_cycle = rtc_time_us_to_slowclk(mc->lp.xtal_wait_stable_time_us, slowclk_period);
    } else {
        param->hp_lp.xtal_stable_wait_cycle       = rtc_time_us_to_fastclk(mc->hp.xtal_wait_stable_time_us, fastclk_period);
    }
    return param;
}

const pmu_sleep_config_t* pmu_sleep_config_default(
        pmu_sleep_config_t *config,
        uint32_t sleep_flags,
        uint32_t clk_flags,
        uint32_t adjustment,
        soc_rtc_slow_clk_src_t slowclk_src,
        uint32_t slowclk_period,
        uint32_t fastclk_period,
        bool dslp
    )
{
    pmu_sleep_power_config_t power_default = PMU_SLEEP_POWER_CONFIG_DEFAULT(sleep_flags);
    config->power = power_default;

    pmu_sleep_param_config_t param_default = PMU_SLEEP_PARAM_CONFIG_DEFAULT(sleep_flags);
    config->param = *pmu_sleep_param_config_default(&param_default, &power_default, sleep_flags, adjustment, slowclk_period, fastclk_period);

    if (dslp) {
        config->param.lp_sys.analog_wait_target_cycle  = rtc_time_us_to_slowclk(PMU_LP_ANALOG_WAIT_TARGET_TIME_DSLP_US, slowclk_period);
        pmu_sleep_analog_config_t analog_default = PMU_SLEEP_ANALOG_DSLP_CONFIG_DEFAULT(sleep_flags);
        config->analog = analog_default;
    } else {
        pmu_sleep_digital_config_t digital_default = PMU_SLEEP_DIGITAL_LSLP_CONFIG_DEFAULT(sleep_flags);
        config->digital = digital_default;

        pmu_sleep_analog_config_t analog_default = PMU_SLEEP_ANALOG_LSLP_CONFIG_DEFAULT(sleep_flags);
        analog_default.hp_sys.analog.drv_b = PMU_HP_DRVB_LIGHTSLEEP;
        analog_default.lp_sys[LP(SLEEP)].analog.dbias = get_slp_lp_dbias();
        if (!(sleep_flags & PMU_SLEEP_PD_XTAL)){
            analog_default.hp_sys.analog.xpd_trx = PMU_XPD_TRX_SLEEP_ON;
            analog_default.hp_sys.analog.drv_b = get_act_hp_drvb();
            analog_default.hp_sys.analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
            analog_default.hp_sys.analog.bias_sleep = PMU_BIASSLP_SLEEP_ON;

            analog_default.lp_sys[LP(SLEEP)].analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
            analog_default.lp_sys[LP(SLEEP)].analog.bias_sleep = PMU_BIASSLP_SLEEP_ON;
            analog_default.lp_sys[LP(SLEEP)].analog.dbias = get_act_lp_dbias();
        } else if (!(sleep_flags & PMU_SLEEP_PD_RC_FAST)) {
            analog_default.hp_sys.analog.drv_b = get_act_hp_drvb();
            analog_default.lp_sys[LP(SLEEP)].analog.dbias = get_act_lp_dbias();
        }
        config->analog = analog_default;
    }
    return config;
}

static void pmu_sleep_power_init(pmu_context_t *ctx, const pmu_sleep_power_config_t *power, bool dslp)
{
    pmu_ll_hp_set_dig_power(ctx->hal->dev, HP(SLEEP), power->hp_sys.dig_power.val);
    pmu_ll_hp_set_clk_power(ctx->hal->dev, HP(SLEEP), power->hp_sys.clk_power.val);
    pmu_ll_hp_set_xtal_xpd (ctx->hal->dev, HP(SLEEP), power->hp_sys.xtal.xpd_xtal);
    pmu_ll_hp_set_xtalx2_xpd (ctx->hal->dev, HP(SLEEP), power->hp_sys.xtal.xpd_xtalx2);

    if (dslp) {
        pmu_ll_hp_set_memory_power_on_mask(ctx->hal->dev, 0);
    } else {
        pmu_ll_hp_set_memory_power_on_mask(ctx->hal->dev, 0xf);
    }

    pmu_ll_lp_set_dig_power(ctx->hal->dev, LP(ACTIVE), power->lp_sys[LP(ACTIVE)].dig_power.val);
    pmu_ll_lp_set_clk_power(ctx->hal->dev, LP(ACTIVE), power->lp_sys[LP(ACTIVE)].clk_power.val);

    pmu_ll_lp_set_dig_power(ctx->hal->dev, LP(SLEEP), power->lp_sys[LP(SLEEP)].dig_power.val);
    pmu_ll_lp_set_clk_power(ctx->hal->dev, LP(SLEEP), power->lp_sys[LP(SLEEP)].clk_power.val);
    pmu_ll_lp_set_xtal_xpd (ctx->hal->dev, LP(SLEEP), power->lp_sys[LP(SLEEP)].xtal.xpd_xtal);
    pmu_ll_lp_set_xtalx2_xpd (ctx->hal->dev, LP(SLEEP), power->lp_sys[LP(SLEEP)].xtal.xpd_xtalx2);
}

static void pmu_sleep_digital_init(pmu_context_t *ctx, const pmu_sleep_digital_config_t *dig)
{
    pmu_ll_hp_set_dig_pad_slp_sel   (ctx->hal->dev, HP(SLEEP), dig->syscntl.dig_pad_slp_sel);
}

static void pmu_sleep_analog_init(pmu_context_t *ctx, const pmu_sleep_analog_config_t *analog, bool dslp)
{
    assert(ctx->hal);
    // Core power supply (include DCDC, HP LDO, LP LDO) configuration
    pmu_ll_hp_set_current_power_off    (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.pd_cur);
    pmu_ll_hp_set_bias_sleep_enable    (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.bias_sleep);
    pmu_ll_hp_set_regulator_xpd        (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.xpd);
    pmu_ll_hp_set_regulator_driver_bar (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.drv_b);
    pmu_ll_hp_set_trx_xpd              (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.xpd_trx);

    pmu_ll_lp_set_current_power_off    (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.pd_cur);
    pmu_ll_lp_set_bias_sleep_enable    (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.bias_sleep);
    pmu_ll_lp_set_regulator_slp_xpd    (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.slp_xpd);
    pmu_ll_lp_set_regulator_xpd        (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.xpd);
    pmu_ll_lp_set_regulator_sleep_dbias(ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.slp_dbias);
    pmu_ll_lp_set_regulator_dbias      (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.dbias);
    pmu_ll_lp_set_regulator_driver_bar (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.drv_b);

    pmu_ll_hp_set_dcm_mode             (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.dcm_mode);
    pmu_ll_hp_set_dcm_vset             (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.dcm_vset);
    pmu_ll_lp_set_dcm_mode             (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.dcm_mode);
    pmu_ll_lp_set_dcm_vset             (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.dcm_vset);
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

    pmu_hal_hp_set_control_ready_wait_cycle(ctx->hal, param->hp_sys.isolate_wait_cycle, param->hp_sys.reset_wait_cycle);
    pmu_hal_lp_set_control_ready_wait_cycle(ctx->hal, param->lp_sys.isolate_wait_cycle, param->lp_sys.reset_wait_cycle);

    pmu_ll_set_modem_wait_target_cycle(ctx->hal->dev, param->hp_sys.modem_wakeup_wait_cycle);
    pmu_ll_set_xtal_stable_wait_cycle(ctx->hal->dev, param->hp_lp.xtal_stable_wait_slow_clk_cycle);
    pmu_ll_set_pll_stable_wait_cycle(ctx->hal->dev, param->hp_sys.pll_stable_wait_cycle);
}

bool pmu_sleep_pll_already_enabled(void)
{
    return (pmu_ll_get_sysclk_sleep_select_state(PMU_instance()->hal->dev) != 0);
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
}

uint32_t pmu_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp)
{
    assert(PMU_instance()->hal);

    lp_aon_hal_inform_wakeup_type(dslp);

    pmu_ll_hp_set_wakeup_enable(PMU_instance()->hal->dev, wakeup_opt);
    pmu_ll_hp_set_reject_enable(PMU_instance()->hal->dev, reject_opt);

    pmu_ll_hp_clear_wakeup_intr_status(PMU_instance()->hal->dev);
    pmu_ll_hp_clear_reject_intr_status(PMU_instance()->hal->dev);
    pmu_ll_hp_clear_reject_cause(PMU_instance()->hal->dev);

    /* Start entry into sleep mode */
    pmu_ll_hp_set_sleep_enable(PMU_instance()->hal->dev);

    while (!pmu_ll_hp_is_sleep_wakeup(PMU_instance()->hal->dev) &&
        !pmu_ll_hp_is_sleep_reject(PMU_instance()->hal->dev)) {
        ;
    }

    return pmu_sleep_finish(dslp);
}

bool pmu_sleep_finish(bool dslp)
{
    (void)dslp;
    return pmu_ll_hp_is_sleep_reject(PMU_instance()->hal->dev);
}

uint32_t pmu_sleep_get_wakup_retention_cost(void)
{
    const pmu_sleep_machine_constant_t *mc = (pmu_sleep_machine_constant_t *)PMU_instance()->mc;
    return mc->hp.regdma_s2a_work_time_us;
}
