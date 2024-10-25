/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "soc/chip_revision.h"
#include "soc/soc.h"
#include "soc/pmu_struct.h"
#include "hal/efuse_hal.h"
#include "hal/pmu_hal.h"
#include "pmu_param.h"
#include "esp_private/esp_pmu.h"
#include "soc/regi2c_dig_reg.h"
#include "regi2c_ctrl.h"

static __attribute__((unused)) const char *TAG = "pmu_init";

typedef struct {
    const pmu_hp_system_power_param_t     *power;
    const pmu_hp_system_clock_param_t     *clock;
    const pmu_hp_system_digital_param_t   *digital;
    const pmu_hp_system_analog_param_t    *analog;
    const pmu_hp_system_retention_param_t *retent;
} pmu_hp_system_param_t;

typedef struct {
    const pmu_lp_system_power_param_t  *power;
    const pmu_lp_system_analog_param_t *analog;
} pmu_lp_system_param_t;

pmu_context_t * __attribute__((weak)) TCM_IRAM_ATTR PMU_instance(void)
{
    /* It should be explicitly defined in the internal RAM, because this
     * instance will be used in pmu_sleep.c */
    static TCM_DRAM_ATTR pmu_hal_context_t pmu_hal = { .dev = &PMU };
    static TCM_DRAM_ATTR pmu_sleep_machine_constant_t pmu_mc = PMU_SLEEP_MC_DEFAULT();
    static TCM_DRAM_ATTR pmu_context_t pmu_context = { .hal = &pmu_hal, .mc = (void *)&pmu_mc };
    return &pmu_context;
}

void pmu_hp_system_init(pmu_context_t *ctx, pmu_hp_mode_t mode, pmu_hp_system_param_t *param)
{
    const pmu_hp_system_power_param_t *power = param->power;
    const pmu_hp_system_clock_param_t *clock = param->clock;
    const pmu_hp_system_digital_param_t *dig = param->digital;
    const pmu_hp_system_analog_param_t *anlg = param->analog;
    const pmu_hp_system_retention_param_t *ret = param->retent;

    assert(ctx->hal);
    /* Default configuration of hp-system power in active, modem and sleep modes */
    pmu_ll_hp_set_dig_power(ctx->hal->dev, mode, power->dig_power.val);
    pmu_ll_hp_set_clk_power(ctx->hal->dev, mode, power->clk_power.val);
    pmu_ll_hp_set_xtal_xpd (ctx->hal->dev, mode, power->xtal.xpd_xtal);

    /* Default configuration of hp-system clock in active, modem and sleep modes */
    pmu_ll_hp_set_icg_func          (ctx->hal->dev, mode, clock->icg_func);
    pmu_ll_hp_set_icg_apb           (ctx->hal->dev, mode, clock->icg_apb);
    pmu_ll_hp_set_sysclk_nodiv      (ctx->hal->dev, mode, clock->sysclk.dig_sysclk_nodiv);
    pmu_ll_hp_set_icg_sysclk_enable (ctx->hal->dev, mode, clock->sysclk.icg_sysclk_en);
    pmu_ll_hp_set_sysclk_slp_sel    (ctx->hal->dev, mode, clock->sysclk.sysclk_slp_sel);
    pmu_ll_hp_set_icg_sysclk_slp_sel(ctx->hal->dev, mode, clock->sysclk.icg_slp_sel);
    pmu_ll_hp_set_dig_sysclk        (ctx->hal->dev, mode, clock->sysclk.dig_sysclk_sel);

    /* Default configuration of hp-system digital sub-system in active, modem
     * and sleep modes */
    pmu_ll_hp_set_uart_wakeup_enable(ctx->hal->dev, mode, dig->syscntl.uart_wakeup_en);
    pmu_ll_hp_set_hold_all_lp_pad   (ctx->hal->dev, mode, dig->syscntl.lp_pad_hold_all);
    pmu_ll_hp_set_hold_all_hp_pad   (ctx->hal->dev, mode, dig->syscntl.hp_pad_hold_all);
    pmu_ll_hp_set_dig_pad_slp_sel   (ctx->hal->dev, mode, dig->syscntl.dig_pad_slp_sel);
    pmu_ll_hp_set_pause_watchdog    (ctx->hal->dev, mode, dig->syscntl.dig_pause_wdt);
    pmu_ll_hp_set_cpu_stall         (ctx->hal->dev, mode, dig->syscntl.dig_cpu_stall);

    /* Default configuration of hp-system analog sub-system in active, modem and
     * sleep modes */
    pmu_ll_hp_set_bias_xpd                    (ctx->hal->dev, mode, anlg->bias.xpd_bias);
    pmu_ll_hp_set_dcm_mode                    (ctx->hal->dev, mode, anlg->bias.dcm_mode);
    pmu_ll_hp_set_dcm_vset                    (ctx->hal->dev, mode, anlg->bias.dcm_vset);
    pmu_ll_hp_set_dbg_atten                   (ctx->hal->dev, mode, anlg->bias.dbg_atten);
    pmu_ll_hp_set_current_power_off           (ctx->hal->dev, mode, anlg->bias.pd_cur);
    pmu_ll_hp_set_bias_sleep_enable           (ctx->hal->dev, mode, anlg->bias.bias_sleep);
    pmu_ll_hp_set_regulator_sleep_memory_xpd  (ctx->hal->dev, mode, anlg->regulator0.slp_mem_xpd);
    pmu_ll_hp_set_regulator_sleep_logic_xpd   (ctx->hal->dev, mode, anlg->regulator0.slp_logic_xpd);
    if (ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 100) && (mode == PMU_MODE_HP_SLEEP)) {
        pmu_ll_hp_enable_sleep_flash_ldo_channel(ctx->hal->dev, anlg->regulator0.xpd_0p1a);
    }
    pmu_ll_hp_set_regulator_sleep_logic_dbias (ctx->hal->dev, mode, anlg->regulator0.slp_logic_dbias);
    pmu_ll_hp_set_regulator_driver_bar        (ctx->hal->dev, mode, anlg->regulator1.drv_b);

    /* Default configuration of hp-system retention sub-system in active, modem
     * and sleep modes */
    pmu_ll_hp_set_retention_param(ctx->hal->dev, mode, ret->retention.val);
    pmu_ll_hp_set_backup_icg_func(ctx->hal->dev, mode, ret->backup_clk);

    pmu_ll_hp_set_sleep_protect_mode(ctx->hal->dev, PMU_SLEEP_PROTECT_HP_LP_SLEEP);
}

void pmu_lp_system_init(pmu_context_t *ctx, pmu_lp_mode_t mode, pmu_lp_system_param_t *param)
{
    const pmu_lp_system_power_param_t *power = param->power;
    const pmu_lp_system_analog_param_t *anlg = param->analog;

    assert(ctx->hal);
    /* Default configuration of lp-system power in active and sleep modes */
    pmu_ll_lp_set_dig_power(ctx->hal->dev, mode, power->dig_power.val);
    pmu_ll_lp_set_clk_power(ctx->hal->dev, mode, power->clk_power.val);
    pmu_ll_lp_set_xtal_xpd (ctx->hal->dev, PMU_MODE_LP_SLEEP, power->xtal.xpd_xtal);

    /* Default configuration of lp-system analog sub-system in active and
     * sleep modes */
    pmu_ll_lp_set_bias_xpd             (ctx->hal->dev, PMU_MODE_LP_SLEEP, anlg->bias.xpd_bias);
    pmu_ll_lp_set_dbg_atten            (ctx->hal->dev, PMU_MODE_LP_SLEEP, anlg->bias.dbg_atten);
    pmu_ll_lp_set_current_power_off    (ctx->hal->dev, PMU_MODE_LP_SLEEP, anlg->bias.pd_cur);
    pmu_ll_lp_set_bias_sleep_enable    (ctx->hal->dev, PMU_MODE_LP_SLEEP, anlg->bias.bias_sleep);
    pmu_ll_lp_set_regulator_slp_xpd    (ctx->hal->dev, mode, anlg->regulator0.slp_xpd);
    pmu_ll_lp_set_regulator_sleep_dbias(ctx->hal->dev, mode, anlg->regulator0.slp_dbias);
    pmu_ll_lp_set_regulator_driver_bar (ctx->hal->dev, mode, anlg->regulator1.drv_b);
}

static inline void pmu_power_domain_force_default(pmu_context_t *ctx)
{
    assert(ctx);
    // for bypass reserved power domain
    const pmu_hp_power_domain_t pmu_hp_domains[] = {
        PMU_HP_PD_TOP,
        PMU_HP_PD_CNNT,
        PMU_HP_PD_HPMEM,
    };

    for (uint8_t idx = 0; idx < (sizeof(pmu_hp_domains) / sizeof(pmu_hp_power_domain_t)); idx++) {
        pmu_ll_hp_set_power_force_power_up  (ctx->hal->dev, pmu_hp_domains[idx], false);
        pmu_ll_hp_set_power_force_no_reset  (ctx->hal->dev, pmu_hp_domains[idx], false);
        pmu_ll_hp_set_power_force_no_isolate(ctx->hal->dev, pmu_hp_domains[idx], false);
        pmu_ll_hp_set_power_force_power_down(ctx->hal->dev, pmu_hp_domains[idx], false);
        pmu_ll_hp_set_power_force_isolate   (ctx->hal->dev, pmu_hp_domains[idx], false);
        pmu_ll_hp_set_power_force_reset     (ctx->hal->dev, pmu_hp_domains[idx], false);
    }
    /* Isolate all memory banks while sleeping, avoid memory leakage current */
    pmu_ll_hp_set_memory_no_isolate     (ctx->hal->dev, 0);

    pmu_ll_lp_set_power_force_power_up  (ctx->hal->dev, false);
    pmu_ll_lp_set_power_force_no_reset  (ctx->hal->dev, false);
    pmu_ll_lp_set_power_force_no_isolate(ctx->hal->dev, false);
    pmu_ll_lp_set_power_force_power_down(ctx->hal->dev, false);
    pmu_ll_lp_set_power_force_isolate   (ctx->hal->dev, false);
    pmu_ll_lp_set_power_force_reset     (ctx->hal->dev, false);
    pmu_ll_set_dcdc_force_power_up(ctx->hal->dev, false);
    pmu_ll_set_dcdc_force_power_down(ctx->hal->dev, false);
}

static inline void pmu_hp_system_param_default(pmu_hp_mode_t mode, pmu_hp_system_param_t *param)
{
    param->power = pmu_hp_system_power_param_default(mode);
    param->clock = pmu_hp_system_clock_param_default(mode);
    param->digital = pmu_hp_system_digital_param_default(mode);
    param->analog = pmu_hp_system_analog_param_default(mode);
    param->retent = pmu_hp_system_retention_param_default(mode);
}

static void pmu_hp_system_init_default(pmu_context_t *ctx)
{
    assert(ctx);
    pmu_hp_system_param_t param = { 0 };
    for (pmu_hp_mode_t mode = PMU_MODE_HP_ACTIVE; mode < PMU_MODE_HP_MAX; mode++) {
        if (mode == PMU_MODE_HP_MODEM) continue;
        pmu_hp_system_param_default(mode, &param);
        pmu_hp_system_init(ctx, mode, &param);
    }
}

static inline void pmu_lp_system_param_default(pmu_lp_mode_t mode, pmu_lp_system_param_t *param)
{
    param->power = pmu_lp_system_power_param_default(mode);
    param->analog = pmu_lp_system_analog_param_default(mode);
}

static void pmu_lp_system_init_default(pmu_context_t *ctx)
{
    assert(ctx);
    pmu_lp_system_param_t param;
    for (pmu_lp_mode_t mode = PMU_MODE_LP_ACTIVE; mode < PMU_MODE_LP_MAX; mode++) {
        pmu_lp_system_param_default(mode, &param);
        pmu_lp_system_init(ctx, mode, &param);
    }
}

void pmu_init(void)
{
    pmu_hp_system_init_default(PMU_instance());
    pmu_lp_system_init_default(PMU_instance());
    pmu_power_domain_force_default(PMU_instance());
}
