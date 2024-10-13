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
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/pmu_struct.h"
#include "hal/lp_aon_hal.h"
#include "esp_private/esp_pmu.h"
#include "pmu_param.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"
#include "esp_hw_log.h"

static __attribute__((unused)) const char *TAG = "pmu_sleep";

#define HP(state)   (PMU_MODE_HP_ ## state)
#define LP(state)   (PMU_MODE_LP_ ## state)

static bool s_pmu_sleep_regdma_backup_enabled;

uint32_t get_lslp_dbg(void)
{
    uint32_t pmu_dbg_atten_lightsleep = PMU_DBG_ATTEN_LIGHTSLEEP_DEFAULT;
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 3) {
        pmu_dbg_atten_lightsleep = efuse_ll_get_lslp_dbg();
    } else {
        ESP_HW_LOGD(TAG, "blk_version is less than 3, lslp dbg not burnt in efuse\n");
    }

    return pmu_dbg_atten_lightsleep;
}

uint32_t get_lslp_hp_dbias(void)
{
    uint32_t pmu_hp_dbias_lightsleep_0v6 = PMU_HP_DBIAS_LIGHTSLEEP_0V6_DEFAULT;
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 3) {
        pmu_hp_dbias_lightsleep_0v6 = efuse_ll_get_lslp_hp_dbias();
    } else {
        ESP_HW_LOGD(TAG, "blk_version is less than 3, lslp hp dbias not burnt in efuse\n");
    }

    return pmu_hp_dbias_lightsleep_0v6;
}

uint32_t get_dslp_dbg(void)
{
    uint32_t pmu_dbg_atten_deepsleep = PMU_DBG_ATTEN_DEEPSLEEP_DEFAULT;
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 3) {
        pmu_dbg_atten_deepsleep = efuse_ll_get_dslp_dbg() + EFUSE_BURN_OFFSET_DSLP_DBG;
    } else {
        ESP_HW_LOGD(TAG, "blk_version is less than 3, dslp dbg not burnt in efuse\n");
    }

    return pmu_dbg_atten_deepsleep;
}

uint32_t get_dslp_lp_dbias(void)
{
    uint32_t pmu_lp_dbias_deepsleep_0v7 = PMU_LP_DBIAS_DEEPSLEEP_0V7_DEFAULT;
    uint32_t blk_version = efuse_hal_blk_version();
    if (blk_version >= 3) {
        pmu_lp_dbias_deepsleep_0v7 = efuse_ll_get_dslp_lp_dbias() + EFUSE_BURN_OFFSET_DSLP_LP_DBIAS;
    } else {
        ESP_HW_LOGD(TAG, "blk_version is less than 3, dslp lp dbias not burnt in efuse\n");
    }

    return pmu_lp_dbias_deepsleep_0v7;
}

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

uint32_t pmu_sleep_calculate_hw_wait_time(uint32_t pd_flags, uint32_t slowclk_period, uint32_t fastclk_period)
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

    /* HP core hardware wait time, microsecond */
    const int hp_digital_power_up_wait_time_us = mc->hp.power_supply_wait_time_us + mc->hp.power_up_wait_time_us;
    const int hp_regdma_wait_time_us = MAX(mc->hp.regdma_s2m_work_time_us + mc->hp.regdma_m2a_work_time_us, mc->hp.regdma_s2a_work_time_us);
    const int hp_clock_wait_time_us = mc->hp.xtal_wait_stable_time_us + mc->hp.pll_wait_stable_time_us;

    const int hp_hw_wait_time_us = mc->hp.analog_wait_time_us + MAX(hp_digital_power_up_wait_time_us + hp_regdma_wait_time_us, hp_clock_wait_time_us);

    /* When the SOC wakeup (lp timer or GPIO wakeup) and Modem wakeup (Beacon wakeup) complete, the soc
     * wakeup will be delayed until the RF is turned on in Modem state.
     *
     *                modem wakeup                      TBTT, RF on by HW
     *                     |                                    |
     *                    \|/                                  \|/
     *   PMU_HP_ACTIVE                                                                         /------
     *   PMU_HP_MODEM                                           /------------//////////////////
     *   PMU_HP_SLEEP   ----------------------//////////////////
     *                    /|\                /|\ /|\          /|\           /|\              /|\
     *                     |<- some hw wait ->|   |            |             |<- M2A switch ->|
     *                     |  slow cycles &   | soc wakeup     |                              |
     *                     |   FOSC cycles    |<- S2M switch ->|                              |
     *                     |                                                                  |
     *                     |<--      PMU guard time, also the maximum time for the SOC     -->|
     *                     |                           wake-up delay                          |
     */
#if SOC_PM_SUPPORT_PMU_MODEM_STATE && CONFIG_ESP_WIFI_ENHANCED_LIGHT_SLEEP
    const int rf_on_protect_time_us = mc->hp.regdma_rf_on_work_time_us;
    const int total_hw_wait_time_us = lp_hw_wait_time_us + hp_hw_wait_time_us + mc->hp.clock_domain_sync_time_us;
#else
    const int rf_on_protect_time_us = 0;
    const int total_hw_wait_time_us = lp_hw_wait_time_us + hp_hw_wait_time_us;
#endif
    return total_hw_wait_time_us + rf_on_protect_time_us;
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
    param->hp_sys.analog_wait_target_cycle        = rtc_time_us_to_fastclk(mc->hp.analog_wait_time_us, fastclk_period);
    param->hp_sys.digital_power_supply_wait_cycle = rtc_time_us_to_fastclk(mc->hp.power_supply_wait_time_us, fastclk_period);
    param->hp_sys.digital_power_up_wait_cycle     = rtc_time_us_to_fastclk(mc->hp.power_up_wait_time_us, fastclk_period);
    param->hp_sys.pll_stable_wait_cycle           = rtc_time_us_to_fastclk(mc->hp.pll_wait_stable_time_us, fastclk_period);

    const int hw_wait_time_us = pmu_sleep_calculate_hw_wait_time(pd_flags, slowclk_period, fastclk_period);
    const int modem_state_skip_time_us = mc->hp.regdma_m2a_work_time_us + mc->hp.system_dfs_up_work_time_us + mc->lp.min_slp_time_us;
    const int modem_wakeup_wait_time_us = adjustment - hw_wait_time_us + modem_state_skip_time_us + mc->hp.regdma_rf_on_work_time_us;
    param->hp_sys.modem_wakeup_wait_cycle         = rtc_time_us_to_fastclk(modem_wakeup_wait_time_us, fastclk_period);

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
        uint32_t sleep_flags,
        uint32_t adjustment,
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
        analog_default.lp_sys[LP(SLEEP)].analog.dbg_atten = get_dslp_dbg();
        analog_default.lp_sys[LP(SLEEP)].analog.dbias = get_dslp_lp_dbias();
        config->analog = analog_default;
    } else {
        pmu_sleep_digital_config_t digital_default = PMU_SLEEP_DIGITAL_LSLP_CONFIG_DEFAULT(sleep_flags);
        config->digital = digital_default;

        pmu_sleep_analog_config_t analog_default = PMU_SLEEP_ANALOG_LSLP_CONFIG_DEFAULT(sleep_flags);
        analog_default.hp_sys.analog.dbg_atten = get_lslp_dbg();
        analog_default.hp_sys.analog.dbias = get_lslp_hp_dbias();
        analog_default.lp_sys[LP(SLEEP)].analog.dbias = PMU_LP_DBIAS_LIGHTSLEEP_0V7_DEFAULT;

        if (!(sleep_flags & PMU_SLEEP_PD_XTAL)){
            analog_default.hp_sys.analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
            analog_default.hp_sys.analog.bias_sleep = PMU_BIASSLP_SLEEP_ON;
            analog_default.hp_sys.analog.dbg_atten = PMU_DBG_ATTEN_ACTIVE_DEFAULT;
            analog_default.hp_sys.analog.dbias = get_act_hp_dbias();

            analog_default.lp_sys[LP(SLEEP)].analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
            analog_default.lp_sys[LP(SLEEP)].analog.bias_sleep = PMU_BIASSLP_SLEEP_ON;
            analog_default.lp_sys[LP(SLEEP)].analog.dbg_atten = PMU_DBG_ATTEN_ACTIVE_DEFAULT;
            analog_default.lp_sys[LP(SLEEP)].analog.dbias = get_act_lp_dbias();
        } else if (!(sleep_flags & PMU_SLEEP_PD_RC_FAST)) {
            analog_default.hp_sys.analog.dbias = get_act_hp_dbias();
            analog_default.lp_sys[LP(SLEEP)].analog.dbg_atten = PMU_DBG_ATTEN_LIGHTSLEEP_NODROP;
            analog_default.lp_sys[LP(SLEEP)].analog.dbias = get_act_lp_dbias();
        }

        config->analog = analog_default;
    }

    if (sleep_flags & RTC_SLEEP_XTAL_AS_RTC_FAST) {
        // Keep XTAL on in HP_SLEEP state if it is the clock source of RTC_FAST
        power_default.hp_sys.xtal.xpd_xtal = 1;
        config->analog.hp_sys.analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
        config->analog.hp_sys.analog.bias_sleep = PMU_BIASSLP_SLEEP_ON;
        config->analog.hp_sys.analog.dbg_atten = PMU_DBG_ATTEN_ACTIVE_DEFAULT;
        config->analog.hp_sys.analog.dbias = get_act_hp_dbias();
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
    pmu_ll_hp_set_dbg_atten                   (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.dbg_atten);
    pmu_ll_hp_set_current_power_off           (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.pd_cur);
    pmu_ll_hp_set_bias_sleep_enable           (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.bias_sleep);
    pmu_ll_hp_set_regulator_xpd               (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.xpd);
    pmu_ll_hp_set_regulator_dbias             (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.dbias);
    pmu_ll_hp_set_regulator_driver_bar        (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.drv_b);

    pmu_ll_lp_set_dbg_atten            (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.dbg_atten);
    pmu_ll_lp_set_current_power_off    (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.pd_cur);
    pmu_ll_lp_set_bias_sleep_enable    (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.bias_sleep);
    pmu_ll_lp_set_regulator_slp_xpd    (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.slp_xpd);
    pmu_ll_lp_set_regulator_xpd        (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.xpd);
    pmu_ll_lp_set_regulator_sleep_dbias(ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.slp_dbias);
    pmu_ll_lp_set_regulator_dbias      (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.dbias);
    pmu_ll_lp_set_regulator_driver_bar (ctx->hal->dev, LP(SLEEP), analog->lp_sys[LP(SLEEP)].analog.drv_b);
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

    /* In pd_cpu lightsleep and deepsleep mode, we never get here */
    while (!pmu_ll_hp_is_sleep_wakeup(PMU_instance()->hal->dev) &&
        !pmu_ll_hp_is_sleep_reject(PMU_instance()->hal->dev)) {
        ;
    }

    return pmu_sleep_finish(dslp);
}

bool pmu_sleep_finish(bool dslp)
{
    (void)dslp;

    // Wait eFuse memory update done.
    while(efuse_ll_get_controller_state() != EFUSE_CONTROLLER_STATE_IDLE);

    return pmu_ll_hp_is_sleep_reject(PMU_instance()->hal->dev);
}

void pmu_sleep_enable_hp_sleep_sysclk(bool enable)
{
    pmu_ll_hp_set_icg_sysclk_enable(PMU_instance()->hal->dev, HP(SLEEP), enable);
}

uint32_t pmu_sleep_get_wakup_retention_cost(void)
{
    const pmu_sleep_machine_constant_t *mc = (pmu_sleep_machine_constant_t *)PMU_instance()->mc;
    return mc->hp.regdma_s2a_work_time_us;
}
