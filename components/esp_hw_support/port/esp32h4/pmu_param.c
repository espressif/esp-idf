/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "pmu_param.h"
#include "soc/pmu_icg_mapping.h"
#include "esp_private/esp_pmu.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"
#include "esp_hw_log.h"
#include "soc/clk_tree_defs.h"

static __attribute__((unused)) const char *TAG = "pmu_param";

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof((a)[0]))
#endif

#define PMU_HP_ACTIVE_POWER_CONFIG_DEFAULT() { \
    .dig_power = {           \
        .vdd_flash_mode = 0, \
        .mem_dslp       = 0, \
        .mem_pd_en      = 0, \
        .wifi_pd_en     = 0, \
        .peri_pd_en     = 0, \
        .cpu_pd_en      = 0, \
        .aon_pd_en      = 0, \
        .top_pd_en      = 0  \
    }, \
    .clk_power = {           \
        .i2c_iso_en     = 0, \
        .i2c_retention  = 0, \
        .xpd_bb_i2c     = 1, \
        .xpd_bbpll_i2c  = 1, \
        .xpd_bbpll      = 1  \
    }, \
    .xtal = {                \
        .xpd_xtalx2     = 1, \
        .xpd_xtal       = 1  \
    } \
}

#define PMU_HP_MODEM_POWER_CONFIG_DEFAULT() { \
    .dig_power = {           \
        .vdd_flash_mode = 0, \
        .mem_dslp       = 0, \
        .mem_pd_en      = 0, \
        .wifi_pd_en     = 0, \
        .peri_pd_en     = 0, \
        .cpu_pd_en      = 1, \
        .aon_pd_en      = 0, \
        .top_pd_en      = 0  \
    }, \
    .clk_power = {           \
        .i2c_iso_en     = 0, \
        .i2c_retention  = 0, \
        .xpd_bb_i2c     = 1, \
        .xpd_bbpll_i2c  = 1, \
        .xpd_bbpll      = 1  \
    }, \
    .xtal = {                \
        .xpd_xtalx2     = 1, \
        .xpd_xtal       = 1  \
    } \
}

#define PMU_HP_SLEEP_POWER_CONFIG_DEFAULT() { \
    .dig_power = {           \
        .vdd_flash_mode = 1, \
        .mem_dslp       = 0, \
        .mem_pd_en      = 0, \
        .wifi_pd_en     = 1, \
        .peri_pd_en     = 0, \
        .cpu_pd_en      = 0, \
        .aon_pd_en      = 0, \
        .top_pd_en      = 0  \
    }, \
    .clk_power = {           \
        .i2c_iso_en     = 1, \
        .i2c_retention  = 1, \
        .xpd_bb_i2c     = 1, \
        .xpd_bbpll_i2c  = 0, \
        .xpd_bbpll      = 0, \
    }, \
    .xtal = {                \
        .xpd_xtalx2     = 0, \
        .xpd_xtal       = 0  \
    } \
}

const pmu_hp_system_power_param_t * pmu_hp_system_power_param_default(pmu_hp_mode_t mode)
{
    static const pmu_hp_system_power_param_t hp_power[] = {
        PMU_HP_ACTIVE_POWER_CONFIG_DEFAULT(),
        PMU_HP_MODEM_POWER_CONFIG_DEFAULT(),
        PMU_HP_SLEEP_POWER_CONFIG_DEFAULT()
    };
    assert(mode < ARRAY_SIZE(hp_power));
    return &hp_power[mode];
}

#define PMU_HP_ACTIVE_CLOCK_CONFIG_DEFAULT() {  \
    .icg_func   = 0xffffffff,                   \
    .icg_apb    = 0xffffffff,                   \
    .icg_modem  = {                             \
        .code = PMU_HP_ICG_MODEM_CODE_ACTIVE    \
    }, \
    .sysclk     = {                             \
        .dig_sysclk_nodiv = 0,                  \
        .icg_sysclk_en    = 1,                  \
        .sysclk_slp_sel   = 0,                  \
        .icg_slp_sel      = 0,                  \
        .dig_sysclk_sel   = SOC_CPU_CLK_SRC_XTAL\
    } \
}

#define PMU_HP_MODEM_CLOCK_CONFIG_DEFAULT() {   \
    .icg_func   = 0,                            \
    .icg_apb    = 0,                            \
    .icg_modem  = {                             \
        .code = PMU_HP_ICG_MODEM_CODE_MODEM     \
    }, \
    .sysclk     = {                             \
        .dig_sysclk_nodiv = 0,                  \
        .icg_sysclk_en    = 1,                  \
        .sysclk_slp_sel   = 1,                  \
        .icg_slp_sel      = 1,                  \
        .dig_sysclk_sel   = SOC_CPU_CLK_SRC_PLL \
    } \
}

#define PMU_HP_SLEEP_CLOCK_CONFIG_DEFAULT() {   \
    .icg_func   = 0,                            \
    .icg_apb    = 0,                            \
    .icg_modem  = {                             \
        .code = PMU_HP_ICG_MODEM_CODE_SLEEP     \
    }, \
    .sysclk     = {                             \
        .dig_sysclk_nodiv = 0,                  \
        .icg_sysclk_en    = 0,                  \
        .sysclk_slp_sel   = 1,                  \
        .icg_slp_sel      = 1,                  \
        .dig_sysclk_sel   = SOC_CPU_CLK_SRC_XTAL\
    } \
}

const pmu_hp_system_clock_param_t * pmu_hp_system_clock_param_default(pmu_hp_mode_t mode)
{
    static const pmu_hp_system_clock_param_t hp_clock[] = {
        PMU_HP_ACTIVE_CLOCK_CONFIG_DEFAULT(),
        PMU_HP_MODEM_CLOCK_CONFIG_DEFAULT(),
        PMU_HP_SLEEP_CLOCK_CONFIG_DEFAULT()
    };
    assert(mode < ARRAY_SIZE(hp_clock));
    return &hp_clock[mode];
}

#define PMU_HP_ACTIVE_DIGITAL_CONFIG_DEFAULT() { \
    .syscntl = {                \
        .uart_wakeup_en  = 0,   \
        .lp_pad_hold_all = 0,   \
        .hp_pad_hold_all = 0,   \
        .dig_pad_slp_sel = 0,   \
        .dig_pause_wdt   = 0,   \
        .dig_cpu_stall   = 0    \
    } \
}

#define PMU_HP_MODEM_DIGITAL_CONFIG_DEFAULT() { \
    .syscntl = {                \
        .uart_wakeup_en  = 1,   \
        .lp_pad_hold_all = 0,   \
        .hp_pad_hold_all = 0,   \
        .dig_pad_slp_sel = 0,   \
        .dig_pause_wdt   = 1,   \
        .dig_cpu_stall   = 1    \
    } \
}

#define PMU_HP_SLEEP_DIGITAL_CONFIG_DEFAULT() { \
    .syscntl = {                \
        .uart_wakeup_en  = 1,   \
        .lp_pad_hold_all = 0,   \
        .hp_pad_hold_all = 0,   \
        .dig_pad_slp_sel = 1,   \
        .dig_pause_wdt   = 1,   \
        .dig_cpu_stall   = 1    \
    } \
}

const pmu_hp_system_digital_param_t * pmu_hp_system_digital_param_default(pmu_hp_mode_t mode)
{
    static const pmu_hp_system_digital_param_t hp_digital[] = {
        PMU_HP_ACTIVE_DIGITAL_CONFIG_DEFAULT(),
        PMU_HP_MODEM_DIGITAL_CONFIG_DEFAULT(),
        PMU_HP_SLEEP_DIGITAL_CONFIG_DEFAULT()
    };
    assert(mode < ARRAY_SIZE(hp_digital));
    return &hp_digital[mode];
}

#define PMU_HP_ACTIVE_ANALOG_CONFIG_DEFAULT() { \
    .bias = {                       \
        .dcdc_ccm_enb       = 1,    \
        .dcdc_clear_rdy     = 0,    \
        .dig_reg_dpcur_bias = 3,    \
        .dig_reg_dsfmos     = 6,    \
        .dcm_vset           = 23,   \
        .dcm_mode           = 0,    \
        .xpd_trx            = 1,    \
        .xpd_bias           = 1,    \
        .discnnt_dig_rtc    = 0,    \
        .pd_cur             = 0,    \
        .bias_sleep         = 0     \
    }, \
    .regulator0 = {                 \
        .power_det_bypass   = 0,    \
        .lp_dbias_vol       = 0xd,  \
        .hp_dbias_vol       = 0x1c, \
        .dbias_sel          = 1,    \
        .dbias_init         = 1,    \
        .slp_mem_xpd        = 0,    \
        .slp_logic_xpd      = 0,    \
        .xpd                = 1,    \
        .slp_mem_dbias      = 0,    \
        .slp_logic_dbias    = 0,    \
        .dbias              = HP_CALI_DBIAS_DEFAULT \
    }, \
    .regulator1 = {                 \
        .drv_b              = 0x0   \
    } \
}

#define PMU_HP_MODEM_ANALOG_CONFIG_DEFAULT() { \
    .bias = {                       \
        .dcdc_ccm_enb       = 0,    \
        .dcdc_clear_rdy     = 0,    \
        .dig_reg_dpcur_bias = 1,    \
        .dig_reg_dsfmos     = 4,    \
        .dcm_vset           = 23,   \
        .dcm_mode           = 0,    \
        .xpd_trx            = 1,    \
        .xpd_bias           = 0,    \
        .discnnt_dig_rtc    = 0,    \
        .pd_cur             = 0,    \
        .bias_sleep         = 0     \
    }, \
    .regulator0 = {                 \
        .power_det_bypass   = 0,    \
        .slp_mem_xpd        = 0,    \
        .slp_logic_xpd      = 0,    \
        .xpd                = 1,    \
        .slp_mem_dbias      = 0,    \
        .slp_logic_dbias    = 0,    \
        .dbias              = HP_CALI_DBIAS_DEFAULT  \
    }, \
    .regulator1 = {                 \
        .drv_b              = 0x0   \
    } \
}

#define PMU_HP_SLEEP_ANALOG_CONFIG_DEFAULT() { \
    .bias = {                       \
        .dcdc_ccm_enb       = 0,    \
        .dcdc_clear_rdy     = 0,    \
        .dig_reg_dpcur_bias = 1,    \
        .dig_reg_dsfmos     = 4,    \
        .dcm_vset           = 23,   \
        .dcm_mode           = 0,    \
        .xpd_trx            = 0,    \
        .xpd_bias           = 0,    \
        .discnnt_dig_rtc    = 0,    \
        .pd_cur             = 0,    \
        .bias_sleep         = 0     \
    }, \
    .regulator0 = {                 \
        .power_det_bypass   = 0,    \
        .slp_mem_xpd        = 0,    \
        .slp_logic_xpd      = 0,    \
        .xpd                = 1,    \
        .slp_mem_dbias      = 0,    \
        .slp_logic_dbias    = 0,    \
        .dbias              = 1     \
    }, \
    .regulator1 = {                 \
        .drv_b              = 0x0   \
    } \
}

const pmu_hp_system_analog_param_t * pmu_hp_system_analog_param_default(pmu_hp_mode_t mode)
{
    static const pmu_hp_system_analog_param_t hp_analog[] = {
        PMU_HP_ACTIVE_ANALOG_CONFIG_DEFAULT(),
        PMU_HP_MODEM_ANALOG_CONFIG_DEFAULT(),
        PMU_HP_SLEEP_ANALOG_CONFIG_DEFAULT()
    };
    assert(mode < ARRAY_SIZE(hp_analog));
    return &hp_analog[mode];
}

#define PMU_HP_RETENTION_REGDMA_CONFIG(dir, entry)  ((((dir)<<4) | (entry & 0xf)) & 0x1f)

#define PMU_HP_ACTIVE_RETENTION_CONFIG_DEFAULT() {  \
    .retention = {                                  \
        .hp_sleep2active_backup_modem_clk_code = 2, \
        .hp_modem2active_backup_modem_clk_code = 2, \
        .hp_sleep2active_backup_clk_sel = 0, \
        .hp_modem2active_backup_clk_sel = 1, \
        .hp_sleep2active_backup_mode    = PMU_HP_RETENTION_REGDMA_CONFIG(0, 0), \
        .hp_modem2active_backup_mode    = PMU_HP_RETENTION_REGDMA_CONFIG(0, 2), \
        .hp_sleep2active_backup_en      = 0, \
        .hp_modem2active_backup_en      = 0, \
    }, \
    .backup_clk = 0xffffffff,   \
}

#define PMU_HP_MODEM_RETENTION_CONFIG_DEFAULT() {   \
    .retention = {                                  \
        .hp_sleep2modem_backup_modem_clk_code  = 1, \
        .hp_sleep2modem_backup_clk_sel  = 0, \
        .hp_sleep2modem_backup_mode     = PMU_HP_RETENTION_REGDMA_CONFIG(0, 1), \
        .hp_sleep2modem_backup_en       = 0, \
    }, \
    .backup_clk = 0xffffffff,   \
}

#define PMU_HP_SLEEP_RETENTION_CONFIG_DEFAULT() {   \
    .retention = {                                  \
        .hp_modem2sleep_backup_modem_clk_code  = 0, \
        .hp_active2sleep_backup_modem_clk_code = 2, \
        .hp_modem2sleep_backup_clk_sel  = 0, \
        .hp_active2sleep_backup_clk_sel = 0, \
        .hp_modem2sleep_backup_mode     = PMU_HP_RETENTION_REGDMA_CONFIG(1, 1), \
        .hp_active2sleep_backup_mode    = PMU_HP_RETENTION_REGDMA_CONFIG(1, 0), \
        .hp_modem2sleep_backup_en       = 0, \
        .hp_active2sleep_backup_en      = 0, \
    }, \
    .backup_clk = 0xffffffff,   \
}

const pmu_hp_system_retention_param_t * pmu_hp_system_retention_param_default(pmu_hp_mode_t mode)
{
    static const pmu_hp_system_retention_param_t hp_retention[] = {
        PMU_HP_ACTIVE_RETENTION_CONFIG_DEFAULT(),
        PMU_HP_MODEM_RETENTION_CONFIG_DEFAULT(),
        PMU_HP_SLEEP_RETENTION_CONFIG_DEFAULT()
    };
    assert(mode < ARRAY_SIZE(hp_retention));
    return &hp_retention[mode];
}


/** LP system default parameter */

#if CONFIG_ESP_SYSTEM_RTC_EXT_XTAL
# define PMU_SLOW_CLK_USE_EXT_XTAL  (1)
#else
# define PMU_SLOW_CLK_USE_EXT_XTAL  (0)
#endif

#define PMU_LP_ACTIVE_POWER_CONFIG_DEFAULT() { \
    .dig_power = {              \
        .vdd_io_mode    = 0,    \
        .bod_source_sel = 0,    \
        .vddbat_mode    = 0,    \
        .mem_dslp       = 0,    \
        .peri_pd_en     = 0,    \
    }, \
    .clk_power = {              \
        .xpd_lppll      = 0,    \
        .xpd_xtal32k    = PMU_SLOW_CLK_USE_EXT_XTAL,    \
        .xpd_rc32k      = 0,    \
        .xpd_fosc       = 1,    \
        .pd_osc         = 0     \
    } \
}

#define PMU_LP_SLEEP_POWER_CONFIG_DEFAULT() { \
    .dig_power = {              \
        .vdd_io_mode    = 0,    \
        .bod_source_sel = 0,    \
        .vddbat_mode    = 0,    \
        .mem_dslp       = 1,    \
        .peri_pd_en     = 0,    \
    }, \
    .clk_power = {              \
        .xpd_lppll      = 0,    \
        .xpd_xtal32k    = 0,    \
        .xpd_rc32k      = 0,    \
        .xpd_fosc       = 0,    \
        .pd_osc         = 0     \
    }, \
    .xtal = {                   \
        .xpd_xtalx2     = 0,    \
        .xpd_xtal       = 0     \
    } \
}

const pmu_lp_system_power_param_t * pmu_lp_system_power_param_default(pmu_lp_mode_t mode)
{
    static const pmu_lp_system_power_param_t lp_power[] = {
        PMU_LP_ACTIVE_POWER_CONFIG_DEFAULT(),
        PMU_LP_SLEEP_POWER_CONFIG_DEFAULT()
    };
    assert(mode < ARRAY_SIZE(lp_power));
    return &lp_power[mode];
}

#define PMU_LP_ACTIVE_ANALOG_CONFIG_DEFAULT() { \
    .regulator0 = {                 \
        .slp_xpd    = 0,            \
        .xpd        = 1,            \
        .slp_dbias          = 0, \
        .dbias              = LP_CALI_DBIAS_DEFAULT  \
    }, \
    .regulator1 = {                 \
        .drv_b      = 0x0           \
    } \
}

#define PMU_LP_SLEEP_ANALOG_CONFIG_DEFAULT() { \
    .bias = {                       \
        .dcdc_ccm_enb       = 0,    \
        .dcdc_clear_rdy     = 0,    \
        .dig_reg_dpcur_bias = 1,    \
        .dig_reg_dsfmos     = 4,    \
        .dcm_vset           = 23,   \
        .dcm_mode           = 0,    \
        .xpd_bias           = 0,    \
        .discnnt_dig_rtc    = 0,    \
        .pd_cur             = 1,    \
        .bias_sleep         = 1,    \
    }, \
    .regulator0 = {                 \
        .slp_xpd            = 0,    \
        .xpd                = 1,    \
        .slp_dbias          = 0,    \
        .dbias              = 12    \
    }, \
    .regulator1 = {                 \
        .drv_b              = 0x0   \
    } \
}

const pmu_lp_system_analog_param_t * pmu_lp_system_analog_param_default(pmu_lp_mode_t mode)
{
    static const pmu_lp_system_analog_param_t lp_analog[] = {
        PMU_LP_ACTIVE_ANALOG_CONFIG_DEFAULT(),
        PMU_LP_SLEEP_ANALOG_CONFIG_DEFAULT()
    };
    assert(mode < ARRAY_SIZE(lp_analog));
    return &lp_analog[mode];
}

uint32_t get_act_hp_dbias(void)
{
    // TODO: IDF-12313
    return HP_CALI_DBIAS_DEFAULT;
}

uint32_t get_act_lp_dbias(void)
{
    // TODO: IDF-12313
     return LP_CALI_DBIAS_DEFAULT;
}
