/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/rtc_clk.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp32p4/rom/cache.h"
#include "soc/chip_revision.h"
#include "soc/soc.h"
#include "soc/regi2c_syspll.h"
#include "soc/regi2c_cpll.h"
#include "soc/rtc.h"
#include "soc/cache_reg.h"
#include "soc/pau_reg.h"
#include "soc/pmu_reg.h"
#include "soc/pmu_struct.h"
#include "hal/clk_tree_hal.h"
#include "hal/gpio_hal.h"
#include "hal/lp_aon_hal.h"
#include "soc/lp_system_reg.h"
#include "hal/pmu_hal.h"
#include "hal/psram_ctrlr_ll.h"
#include "hal/lp_sys_ll.h"
#include "hal/lp_clkrst_ll.h"
#include "esp_private/esp_pmu.h"
#include "pmu_param.h"
#include "esp_rom_sys.h"
#include "esp_rom_serial_output.h"
#include "hal/efuse_hal.h"
#if CONFIG_SPIRAM
#include "hal/ldo_ll.h"
#endif

#if (CONFIG_ESP_REV_MIN_FULL == 300)
#include "soc/hp_system_reg.h"
#include "hal/mmu_ll.h"
#include "hal/mspi_ll.h"
#endif

#define HP(state)   (PMU_MODE_HP_ ## state)
#define LP(state)   (PMU_MODE_LP_ ## state)

#define DCDC_STARTUP_TIME_US    (950)

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

uint32_t pmu_sleep_calculate_lp_hw_wait_time(uint32_t sleep_flags, uint32_t slowclk_period, uint32_t fastclk_period)
{
    const pmu_sleep_machine_constant_t *mc = (pmu_sleep_machine_constant_t *)PMU_instance()->mc;
    /* LP core hardware wait time, microsecond */
    const int lp_wakeup_wait_time_us        = rtc_time_slowclk_to_us(mc->lp.wakeup_wait_cycle, slowclk_period);
    const int lp_clk_switch_time_us         = rtc_time_slowclk_to_us(mc->lp.clk_switch_cycle, slowclk_period);
    /* If XTAL is used as RTC_FAST clock source, it is started in LP_SLEEP -> LP_ACTIVE stage and the clock waiting time is counted into lp_hw_wait_time */
    const int lp_clk_power_on_wait_time_us  = ((sleep_flags & PMU_SLEEP_PD_XTAL) && (sleep_flags & RTC_SLEEP_XTAL_AS_RTC_FAST)) \
                                            ? mc->lp.xtal_wait_stable_time_us \
                                            : rtc_time_slowclk_to_us(mc->lp.clk_power_on_wait_cycle, slowclk_period);

    const int lp_hw_wait_time_us = mc->lp.min_slp_time_us + mc->lp.analog_wait_time_us + lp_clk_power_on_wait_time_us \
                            + lp_wakeup_wait_time_us + lp_clk_switch_time_us + mc->lp.power_supply_wait_time_us \
                                + mc->lp.power_up_wait_time_us;

    return (uint32_t)lp_hw_wait_time_us;
}

uint32_t pmu_sleep_calculate_hp_hw_wait_time(uint32_t sleep_flags, uint32_t slowclk_period, uint32_t fastclk_period)
{
    pmu_sleep_machine_constant_t *mc = (pmu_sleep_machine_constant_t *)PMU_instance()->mc;
    /* HP core hardware wait time, microsecond */
    const int hp_digital_power_up_wait_time_us = mc->hp.power_supply_wait_time_us + mc->hp.power_up_wait_time_us;
    const int hp_regdma_wait_time_us = s_pmu_sleep_regdma_backup_enabled ? mc->hp.regdma_s2a_work_time_us : 0;
    /* If XTAL is not used as RTC_FAST clock source, it is started in HP_SLEEP -> HP_ACTIVE stage and the clock waiting time is counted into hp_hw_wait_time */
    const int hp_clock_wait_time_us = ((sleep_flags & PMU_SLEEP_PD_XTAL) && !(sleep_flags & RTC_SLEEP_XTAL_AS_RTC_FAST)) \
                                    ? mc->hp.xtal_wait_stable_time_us + mc->hp.pll_wait_stable_time_us \
                                    : mc->hp.pll_wait_stable_time_us;

    if (sleep_flags & PMU_SLEEP_PD_TOP) {
        mc->hp.analog_wait_time_us = PMU_HP_ANA_WAIT_TIME_PD_TOP_US;
    } else {
        mc->hp.analog_wait_time_us = PMU_HP_ANA_WAIT_TIME_PU_TOP_US;
    }

    const int hp_hw_wait_time_us = mc->hp.analog_wait_time_us + hp_digital_power_up_wait_time_us + hp_regdma_wait_time_us + hp_clock_wait_time_us;
    return (uint32_t)hp_hw_wait_time_us;
}

uint32_t pmu_sleep_calculate_hw_wait_time(uint32_t sleep_flags, soc_rtc_slow_clk_src_t slowclk_src, uint32_t slowclk_period, uint32_t fastclk_period)
{
    const uint32_t lp_hw_wait_time_us = pmu_sleep_calculate_lp_hw_wait_time(sleep_flags, slowclk_period, fastclk_period);
    const uint32_t hp_hw_wait_time_us = pmu_sleep_calculate_hp_hw_wait_time(sleep_flags, slowclk_period, fastclk_period);
    const uint32_t total_hw_wait_time_us = lp_hw_wait_time_us + hp_hw_wait_time_us;
    return total_hw_wait_time_us;
}

#define rtc_time_us_to_fastclk(time_us, period)     rtc_time_us_to_slowclk((time_us), (period))

static inline pmu_sleep_param_config_t * pmu_sleep_param_config_default(
        pmu_sleep_param_config_t *param,
        pmu_sleep_power_config_t *power, /* We'll use the runtime power parameter to determine some hardware parameters */
        const uint32_t sleep_flags,
        const uint32_t adjustment,
        soc_rtc_slow_clk_src_t slowclk_src,
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
#if !CONFIG_ESP32P4_SELECTS_REV_LESS_V3
    power_default.hp_sys.dig_power.cpu_pd_en = (sleep_flags & PMU_SLEEP_PD_CPU) ? 1 : 0;
#endif
    if (dslp) {
        config->param.lp_sys.analog_wait_target_cycle  = rtc_time_us_to_slowclk(PMU_LP_ANALOG_WAIT_TARGET_TIME_DSLP_US, slowclk_period);

        pmu_sleep_digital_config_t digital_default = PMU_SLEEP_DIGITAL_DSLP_CONFIG_DEFAULT(sleep_flags);
        config->digital = digital_default;

        pmu_sleep_analog_config_t analog_default = PMU_SLEEP_ANALOG_DSLP_CONFIG_DEFAULT(sleep_flags);
        if (sleep_flags & PMU_SLEEP_PD_VDDSDIO) {
            analog_default.hp_sys.analog.xpd_0p1a = 0;
        } else {
            analog_default.hp_sys.analog.xpd_0p1a = 1;
        }
        config->analog = analog_default;

        if (sleep_flags & RTC_SLEEP_POWER_BY_VBAT) {
            power_default.lp_sys[PMU_MODE_LP_SLEEP].dig_power.vddbat_mode = 1;
            power_default.lp_sys[PMU_MODE_LP_SLEEP].dig_power.bod_source_sel = 1;
        }
    } else {
        // Get light sleep digital_default
        pmu_sleep_digital_config_t digital_default = PMU_SLEEP_DIGITAL_LSLP_CONFIG_DEFAULT(sleep_flags);
        config->digital = digital_default;

        // Get light sleep analog default
        pmu_sleep_analog_config_t analog_default = PMU_SLEEP_ANALOG_LSLP_CONFIG_DEFAULT(sleep_flags);

#if CONFIG_SPIRAM
        // Adjust analog parameters to keep EXT_LDO PSRAM channel volt outputting during light-sleep.
        analog_default.hp_sys.analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
        analog_default.lp_sys[PMU_MODE_LP_SLEEP].analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
#endif

        if (!(sleep_flags & PMU_SLEEP_PD_XTAL))
        {
            // Analog parameters in HP_SLEEP
            analog_default.hp_sys.analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
            analog_default.hp_sys.analog.bias_sleep = PMU_BIASSLP_SLEEP_ON;
            analog_default.hp_sys.analog.dbg_atten = PMU_DBG_ATTEN_ACTIVE_DEFAULT;
            analog_default.hp_sys.analog.dbias = HP_CALI_ACTIVE_DBIAS_DEFAULT;

            // Analog parameters in LP_SLEEP
            analog_default.lp_sys[LP(SLEEP)].analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
            analog_default.lp_sys[LP(SLEEP)].analog.bias_sleep = PMU_BIASSLP_SLEEP_ON;
            analog_default.lp_sys[LP(SLEEP)].analog.dbg_atten = PMU_DBG_ATTEN_ACTIVE_DEFAULT;
        }
        power_default.hp_sys.dig_power.dcdc_switch_pd_en = 0;
        analog_default.hp_sys.analog.dcm_vset = CONFIG_ESP_SLEEP_DCM_VSET_VAL_IN_SLEEP;
        if (sleep_flags & PMU_SLEEP_PD_VDDSDIO) {
            analog_default.hp_sys.analog.xpd_0p1a = 0;
        } else {
            analog_default.hp_sys.analog.xpd_0p1a = 1;
        }
        config->analog = analog_default;
    }

    if ((sleep_flags & RTC_SLEEP_USE_ADC_TESEN_MONITOR) || (sleep_flags & RTC_SLEEP_XTAL_AS_RTC_FAST)) {
        config->analog.hp_sys.analog.pd_cur = PMU_PD_CUR_SLEEP_ON;
        config->analog.hp_sys.analog.bias_sleep = PMU_BIASSLP_SLEEP_ON;
    }

    if (sleep_flags & RTC_SLEEP_XTAL_AS_RTC_FAST) {
        // Keep XTAL on in HP_SLEEP state if it is the clock source of RTC_FAST
        power_default.hp_sys.xtal.xpd_xtal = 1;
        config->analog.hp_sys.analog.dbg_atten = PMU_DBG_ATTEN_ACTIVE_DEFAULT;
        config->analog.hp_sys.analog.dbias = HP_CALI_ACTIVE_DBIAS_DEFAULT;
    }

    if (sleep_flags & RTC_SLEEP_LP_PERIPH_USE_RC_FAST) {
        config->analog.hp_sys.analog.dbias = get_act_hp_dbias();
        config->analog.lp_sys[LP(SLEEP)].analog.dbg_atten = 0;
        config->analog.lp_sys[LP(SLEEP)].analog.dbias = get_act_lp_dbias();
    }

    config->power = power_default;
    pmu_sleep_param_config_t param_default = PMU_SLEEP_PARAM_CONFIG_DEFAULT(sleep_flags);
    config->param = *pmu_sleep_param_config_default(&param_default, &power_default, sleep_flags, adjustment, slowclk_src, slowclk_period, fastclk_period);

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
    pmu_ll_hp_set_hold_all_lp_pad   (ctx->hal->dev, HP(SLEEP), dig->syscntl.lp_pad_hold_all);
    pmu_ll_hp_set_pause_watchdog    (ctx->hal->dev, HP(SLEEP), dig->syscntl.dig_pause_wdt);

    // Lowpower workaround for LP pad holding, JTAG IOs is located on lp_pad on esp32p4, if hold its
    // default state on sleep, there's a high current leakage.
    if (dig->syscntl.lp_pad_hold_all) {
        gpio_hal_context_t gpio_hal = {
            .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
        };
        if ((LP_IOMUX.pad[2].mux_sel == 0) && (IO_MUX.gpio[2].mcu_sel == 0)) {
            gpio_hal_isolate_in_sleep(&gpio_hal, 2); // MTCK
        }
        if ((LP_IOMUX.pad[3].mux_sel == 0) && (IO_MUX.gpio[3].mcu_sel == 0)) {
            gpio_hal_isolate_in_sleep(&gpio_hal, 3); // MTDI
        }
        if ((LP_IOMUX.pad[4].mux_sel == 0) && (IO_MUX.gpio[4].mcu_sel == 0)) {
            gpio_hal_isolate_in_sleep(&gpio_hal, 4); // MTMS
        }
        if ((LP_IOMUX.pad[5].mux_sel == 0) && (IO_MUX.gpio[5].mcu_sel == 0)) {
            gpio_hal_isolate_in_sleep(&gpio_hal, 5); // MTDO
        }
    }
}

static void pmu_sleep_analog_init(pmu_context_t *ctx, const pmu_sleep_analog_config_t *analog, bool dslp)
{
    assert(ctx->hal);
    /* For deepsleep, DCDC_EN will be controlled by software to avoid DCDC working in a non-feedback state,
       which may cause input glitch voltage when waking up and switching to LDO. After chip wake up from deepsleep,
       set DCDC_EN in rtc_clk_init. */
    pmu_ll_hp_set_dcm_mode                    (ctx->hal->dev, HP(ACTIVE), dslp ? 0 : 1);
    pmu_ll_hp_set_dcm_mode                    (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.dcm_mode);
    pmu_ll_hp_set_dcm_vset                    (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.dcm_vset);
    pmu_ll_hp_set_current_power_off           (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.pd_cur);
    pmu_ll_hp_set_bias_sleep_enable           (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.bias_sleep);
    pmu_ll_hp_set_regulator_sleep_memory_xpd  (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.slp_mem_xpd);
    pmu_ll_hp_set_regulator_sleep_logic_xpd   (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.slp_logic_xpd);
    pmu_ll_hp_set_regulator_xpd               (ctx->hal->dev, HP(SLEEP), analog->hp_sys.analog.xpd);
    if (ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 300)) {
        pmu_ll_hp_enable_sleep_flash_ldo_channel(ctx->hal->dev, analog->hp_sys.analog.xpd_0p1a);
    }
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
    pmu_sleep_digital_init(PMU_instance(), &config->digital);
    pmu_sleep_analog_init(PMU_instance(), &config->analog, dslp);
    pmu_sleep_param_init(PMU_instance(), &config->param, dslp);
}

void pmu_sleep_increase_ldo_volt(void) {
    pmu_ll_hp_set_regulator_dbias(&PMU, PMU_MODE_HP_ACTIVE, 30);
    pmu_ll_hp_set_regulator_xpd(&PMU, PMU_MODE_HP_ACTIVE, 1);
    // Decrease the DCDC voltage to reduce the voltage difference between the DCDC and the LDO to avoid overshooting the DCDC voltage during wake-up.
    pmu_ll_hp_set_dcm_vset(&PMU, PMU_MODE_HP_ACTIVE, 24);
}

void pmu_sleep_shutdown_dcdc(void) {
    // Keep dcdc_switch on, will be disabled by PMU when entered sleep.
    pmu_ll_set_dcdc_en(&PMU, false);
    // Decrease hp_ldo voltage.
    pmu_ll_hp_set_regulator_dbias(&PMU, PMU_MODE_HP_ACTIVE, HP_CALI_ACTIVE_DBIAS_DEFAULT);
}

FORCE_INLINE_ATTR void pmu_sleep_enable_dcdc(void) {
    pmu_ll_set_dcdc_switch_force_power_down(&PMU, false);
    pmu_ll_set_dcdc_en(&PMU, true);
    pmu_ll_hp_set_dcm_vset(&PMU, PMU_MODE_HP_ACTIVE, HP_CALI_ACTIVE_DCM_VSET_DEFAULT);
}

FORCE_INLINE_ATTR void pmu_sleep_shutdown_ldo(void) {
    pmu_ll_hp_set_regulator_xpd(&PMU, PMU_MODE_HP_ACTIVE, 0);
}

FORCE_INLINE_ATTR void pmu_sleep_cache_sync_items(uint32_t gid, uint32_t type, uint32_t map, uint32_t addr, uint32_t bytes)
{
    REG_WRITE(CACHE_SYNC_ADDR_REG, addr);
    REG_WRITE(CACHE_SYNC_SIZE_REG, bytes);
    REG_WRITE(CACHE_SYNC_MAP_REG, map);
    REG_SET_FIELD(CACHE_SYNC_CTRL_REG, CACHE_SYNC_RGID, gid);
    REG_SET_BIT(CACHE_SYNC_CTRL_REG, type);
    while (!REG_GET_BIT(CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE))
        ;
}

static TCM_DRAM_ATTR uint32_t s_mpll_freq_mhz_before_sleep = 0;

__attribute__((optimize("-O2")))
TCM_IRAM_ATTR uint32_t pmu_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp)
{
    lp_aon_hal_inform_wakeup_type(dslp);

    pmu_ll_hp_set_wakeup_enable(&PMU, wakeup_opt);
    pmu_ll_hp_set_reject_enable(&PMU, reject_opt);

    pmu_ll_hp_clear_wakeup_intr_status(&PMU);
    pmu_ll_hp_clear_reject_intr_status(&PMU);
    pmu_ll_hp_clear_reject_cause(&PMU);

    // 1. For the sleep where powered down the TOP domain, the L1 cache data memory will be lost and needs to be written back here.
    // 2. For the sleep without power down the TOP domain, regdma retention may still be enabled, and dirty data in the L1 cache needs
    //    to be written back so that regdma can get the correct link.
    // 3. We cannot use the API provided by ROM to invalidate the cache, since it is a function calling that writes data to the stack during
    //    the return process, which results in dirty cachelines in L1 Cache again.
    pmu_sleep_cache_sync_items(SMMU_GID_DEFAULT, CACHE_SYNC_WRITEBACK, CACHE_MAP_L1_DCACHE, 0, 0);

    if (!dslp) {
#if CONFIG_SPIRAM
        psram_ctrlr_ll_wait_all_transaction_done();
#endif
        s_mpll_freq_mhz_before_sleep = rtc_clk_mpll_get_freq();
        if (s_mpll_freq_mhz_before_sleep) {
#if CONFIG_SPIRAM
            _psram_ctrlr_ll_select_clk_source(PSRAM_CTRLR_LL_MSPI_ID_2, PSRAM_CLK_SRC_XTAL);
            _psram_ctrlr_ll_select_clk_source(PSRAM_CTRLR_LL_MSPI_ID_3, PSRAM_CLK_SRC_XTAL);
            if (!s_pmu_sleep_regdma_backup_enabled) {
                // MSPI2 and MSPI3 share the register for core clock. So we only set MSPI2 here.
                // If it's a PD_TOP sleep, psram MSPI core clock will be disabled by REGDMA
                // !!! Need to manually check that data in PSRAM will not be accessed from now on. !!!
                _psram_ctrlr_ll_enable_core_clock(PSRAM_CTRLR_LL_MSPI_ID_2, false);
                _psram_ctrlr_ll_enable_module_clock(PSRAM_CTRLR_LL_MSPI_ID_2, false);
            }
#endif
            rtc_clk_mpll_disable();
        }
    } else {
#if CONFIG_P4_REV3_MSPI_CRASH_AFTER_POWER_UP_WORKAROUND
    if (efuse_hal_chip_revision() == 300) {
        lp_clkrst_ll_boot_from_lp_ram(true);
    }
#endif
    }


#if CONFIG_SPIRAM && CONFIG_ESP_LDO_RESERVE_PSRAM
    // Disable PSRAM chip power supply
    if (dslp) {
        ldo_ll_enable(LDO_ID2UNIT(CONFIG_ESP_LDO_CHAN_PSRAM_DOMAIN), false);
    }
#endif

    // The PMU state machine will switch PAD to sleep setting and do IO holding at the same stage.
    // IO may be held to an indeterminate state, so the software needs to trigger the PAD to switch
    // to the sleep setting before starting the PMU state machine.
    pmu_ll_imm_set_pad_slp_sel(&PMU, true);

    /* Start entry into sleep mode */
    pmu_ll_hp_set_sleep_enable(&PMU);

    while (!pmu_ll_hp_is_sleep_wakeup(&PMU) &&
        !pmu_ll_hp_is_sleep_reject(&PMU)) {
        ;
    }

    if (dslp) {
#if CONFIG_SPIRAM && CONFIG_ESP_LDO_RESERVE_PSRAM
        // Enable PSRAM chip power supply after deepsleep request rejected
        ldo_ll_enable(LDO_ID2UNIT(CONFIG_ESP_LDO_CHAN_PSRAM_DOMAIN), true);
#endif
#if CONFIG_P4_REV3_MSPI_CRASH_AFTER_POWER_UP_WORKAROUND
        if (efuse_hal_chip_revision() == 300) {
            // Set reset vector back to HP ROM after deepsleep request rejected
            lp_clkrst_ll_boot_from_lp_ram(false);
        }
#endif
    }

    return pmu_sleep_finish(dslp);
}

TCM_IRAM_ATTR bool pmu_sleep_finish(bool dslp)
{
    if (dslp) {
        pmu_ll_hp_set_dcm_vset(&PMU, PMU_MODE_HP_ACTIVE, HP_CALI_ACTIVE_DCM_VSET_DEFAULT);
        pmu_sleep_enable_dcdc();
        if (pmu_ll_hp_is_sleep_reject(&PMU)) {
            // If sleep is rejected or regdma restore is skipped, the hardware wake-up process that
            // turns on DCDC is skipped, and wait DCDC volt rise up by software here.
            esp_rom_delay_us(DCDC_STARTUP_TIME_US);
        }
        pmu_sleep_shutdown_ldo();
    }

    pmu_ll_imm_set_pad_slp_sel(&PMU, false);

    // Wait eFuse memory update done.
    while(efuse_ll_get_controller_state() != EFUSE_CONTROLLER_STATE_IDLE);

    if (s_mpll_freq_mhz_before_sleep && !dslp) {
        rtc_clk_mpll_enable();
        rtc_clk_mpll_configure(clk_hal_xtal_get_freq_mhz(), s_mpll_freq_mhz_before_sleep, true);
#if CONFIG_SPIRAM
        if (!s_pmu_sleep_regdma_backup_enabled) {
            // MSPI2 and MSPI3 share the register for core clock. So we only set MSPI2 here.
            // If it's a PD_TOP sleep, psram MSPI core clock will be enabled by REGDMA
            _psram_ctrlr_ll_enable_core_clock(PSRAM_CTRLR_LL_MSPI_ID_2, true);
            _psram_ctrlr_ll_enable_module_clock(PSRAM_CTRLR_LL_MSPI_ID_2, true);
        }
        _psram_ctrlr_ll_select_clk_source(PSRAM_CTRLR_LL_MSPI_ID_2, PSRAM_CLK_SRC_MPLL);
        _psram_ctrlr_ll_select_clk_source(PSRAM_CTRLR_LL_MSPI_ID_3, PSRAM_CLK_SRC_MPLL);
#endif
    }

    unsigned chip_version = efuse_hal_chip_revision();
    if (!ESP_CHIP_REV_ABOVE(chip_version, 1)) {
        REGI2C_WRITE_MASK(I2C_CPLL, I2C_CPLL_OC_DIV_7_0, 6); // lower default cpu_pll freq to 400M
        REGI2C_WRITE_MASK(I2C_SYSPLL, I2C_SYSPLL_OC_DIV_7_0, 8); // lower default sys_pll freq to 480M
    }
    return pmu_ll_hp_is_sleep_reject(&PMU);
}

uint32_t pmu_sleep_get_wakup_retention_cost(void)
{
    return PMU_REGDMA_S2A_WORK_TIME_US;
}
