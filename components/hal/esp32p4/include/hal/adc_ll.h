/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "esp_attr.h"

#include "soc/adc_periph.h"
#include "soc/adc_struct.h"
#include "soc/clk_tree_defs.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/lpperi_struct.h"
#include "soc/regi2c_saradc.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/adc_types.h"
#include "hal/adc_types_private.h"
#include "hal/regi2c_ctrl.h"
#include "hal/sar_ctrl_ll.h"
#include "soc/lp_adc_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_LL_EVENT_ADC1_ONESHOT_DONE    BIT(31)
#define ADC_LL_EVENT_ADC2_ONESHOT_DONE    BIT(30)

#define LP_ADC_FORCE_XPD_SAR_FSM 0 // Use FSM to control power down
#define LP_ADC_FORCE_XPD_SAR_PD  2 // Force power down
#define LP_ADC_FORCE_XPD_SAR_PU  3 // Force power up

/*---------------------------------------------------------------
                    Oneshot
---------------------------------------------------------------*/
#define ADC_LL_DATA_INVERT_DEFAULT(PERIPH_NUM)         (0)
#define ADC_LL_SAR_CLK_DIV_DEFAULT(PERIPH_NUM)         (1)
#define LP_ADC_LL_SAR_CLK_DIV_DEFAULT(PERIPH_NUM)      (2)
#define ADC_LL_DELAY_CYCLE_AFTER_DONE_SIGNAL           (10)

/*---------------------------------------------------------------
                    DMA
---------------------------------------------------------------*/
#define ADC_LL_DIGI_DATA_INVERT_DEFAULT(PERIPH_NUM)    (0)
#define ADC_LL_FSM_RSTB_WAIT_DEFAULT                   (8)
#define ADC_LL_FSM_START_WAIT_DEFAULT                  (5)
#define ADC_LL_FSM_STANDBY_WAIT_DEFAULT                (100)
#define ADC_LL_SAMPLE_CYCLE_DEFAULT                    (2)
#define ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT                (1)

#define ADC_LL_CLKM_DIV_NUM_DEFAULT       15
#define ADC_LL_CLKM_DIV_B_DEFAULT         1
#define ADC_LL_CLKM_DIV_A_DEFAULT         0
#define ADC_LL_DEFAULT_CONV_LIMIT_EN      0
#define ADC_LL_DEFAULT_CONV_LIMIT_NUM     10

#define ADC_LL_POWER_MANAGE_SUPPORTED     1 //ESP32P4 supported to manage power mode
/*---------------------------------------------------------------
                    PWDET (Power Detect)
---------------------------------------------------------------*/
#define ADC_LL_PWDET_CCT_DEFAULT                       (4)

typedef enum {
    ADC_LL_CTRL_RTC   = 0,    ///< For ADC1 and ADC2. Select RTC controller.
    ADC_LL_CTRL_ULP   = 1,    ///< For ADC1 and ADC2. Select ULP controller.
    ADC_LL_CTRL_DIG   = 2,    ///< For ADC1 and ADC2. Select DIG controller.
    ADC_LL_CTRL_PWDET = 3,    ///< For ADC2. Select PWDET controller.
} adc_ll_controller_t;

typedef enum {
    ADC_LL_POWER_BY_FSM = SAR_CTRL_LL_POWER_FSM,    /*!< ADC XPD controlled by FSM. Used for polling mode */
    ADC_LL_POWER_SW_ON = SAR_CTRL_LL_POWER_ON,      /*!< ADC XPD controlled by SW. power on. Used for DMA mode */
    ADC_LL_POWER_SW_OFF = SAR_CTRL_LL_POWER_OFF,    /*!< ADC XPD controlled by SW. power off. */
} adc_ll_power_t;

/**
 * @brief ADC digital controller (DMA mode) work mode.
 *
 * @note  The conversion mode affects the sampling frequency:
 *        SINGLE_UNIT_1: When the measurement is triggered, only ADC1 is sampled once.
 *        SINGLE_UNIT_2: When the measurement is triggered, only ADC2 is sampled once.
 *        BOTH_UNIT    : When the measurement is triggered, ADC1 and ADC2 are sampled at the same time.
 *        ALTER_UNIT   : When the measurement is triggered, ADC1 or ADC2 samples alternately.
 */
typedef enum {
    ADC_LL_DIGI_CONV_ONLY_ADC1  = 0,    // Only use ADC1 for conversion
    ADC_LL_DIGI_CONV_ONLY_ADC2  = 1,    // Only use ADC2 for conversion
    ADC_LL_DIGI_CONV_BOTH_UNIT  = 2,    // Use Both ADC1 and ADC2 for conversion simultaneously
    ADC_LL_DIGI_CONV_ALTER_UNIT = 3     // Use both ADC1 and ADC2 for conversion by turn. e.g. ADC1 -> ADC2 -> ADC1 -> ADC2 .....
} adc_ll_digi_convert_mode_t;

typedef struct  {
    union {
        struct {
            uint8_t atten:      2;
            uint8_t channel:    3;
            uint8_t unit:       1;
            uint8_t reserved:   2;
        };
        uint8_t val;
    };
} __attribute__((packed)) adc_ll_digi_pattern_table_t;

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

/**
 * Set adc fsm interval parameter for digital controller. These values are fixed for same platforms.
 *
 * @param rst_wait cycles between DIG ADC controller reset ADC sensor and start ADC sensor.
 * @param start_wait Delay time after open xpd.
 * @param standby_wait Delay time to close xpd.
 */
static inline void adc_ll_digi_set_fsm_time(uint32_t rst_wait, uint32_t start_wait, uint32_t standby_wait)
{
    // Internal FSM reset wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.fsm_wait, rstb_wait, rst_wait);
    // Internal FSM start wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.fsm_wait, xpd_wait, start_wait);
    // Internal FSM standby wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.fsm_wait, standby_wait, standby_wait);
}

/**
 * Set adc sample cycle for digital controller.
 *
 * @note Normally, please use default value.
 * @param sample_cycle Cycles between DIG ADC controller start ADC sensor and beginning to receive data from sensor.
 *                     Range: 2 ~ 0xFF.
 */
static inline void adc_ll_set_sample_cycle(uint32_t sample_cycle)
{
    /* Analog i2c master clock needs to be enabled for regi2c operations (done inside REGI2C_WRITE_MASK) */
    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_SAMPLE_CYCLE_ADDR, sample_cycle);
}

/**
 * Set SAR ADC module clock division factor.
 * SAR ADC clock divided from digital controller clock.
 *
 * @param div Division factor.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_clk_div(uint32_t div)
{
    /* ADC clock divided from digital controller clock clk */
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.ctrl_reg, sar_clk_div, div);
}

/**
 * Set adc max conversion number for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param meas_num Max conversion number. Range: 0 ~ 255.
 */
static inline void adc_ll_digi_set_convert_limit_num(uint32_t meas_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.ctrl2, max_meas_num, meas_num);
}

/**
 * Enable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param enable  true: enable; false: disable
 */
static inline void adc_ll_digi_convert_limit_enable(bool enable)
{
    ADC.ctrl2.meas_num_limit = enable;
}

/**
 * Set adc conversion mode for digital controller.
 *
 * @param mode Conversion mode select.
 */
static inline void adc_ll_digi_set_convert_mode(adc_ll_digi_convert_mode_t mode)
{
    if (mode == ADC_LL_DIGI_CONV_ONLY_ADC1) {
        ADC.ctrl_reg.work_mode = 0;
        ADC.ctrl_reg.sar_sel = 0;
    } else if (mode == ADC_LL_DIGI_CONV_ONLY_ADC2) {
        ADC.ctrl_reg.work_mode = 0;
        ADC.ctrl_reg.sar_sel = 1;
    } else if (mode == ADC_LL_DIGI_CONV_BOTH_UNIT) {
        ADC.ctrl_reg.work_mode = 1;
    } else if (mode == ADC_LL_DIGI_CONV_ALTER_UNIT) {
        ADC.ctrl_reg.work_mode = 2;
    }
    ADC.ctrl_reg.data_sar_sel = 1;
}

/**
 * Set ADC digital controller clock division factor. The clock divided from `APLL` or `APB` clock.
 * Expression: controller_clk = (APLL or APB) / (div_num + div_a / div_b + 1).
 *
 * @param div_num Division factor. Range: 0 ~ 255.
 * @param div_b Division factor. Range: 1 ~ 63.
 * @param div_a Division factor. Range: 0 ~ 63.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_controller_clk_div(uint32_t div_num, uint32_t div_b, uint32_t div_a)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl23, reg_adc_clk_div_num, div_num);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl23, reg_adc_clk_div_numerator, div_a);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl23, reg_adc_clk_div_denominator, div_b);
}

/**
 * Enable clock and select clock source for ADC digital controller.
 *
 * @param clk_src clock source for ADC digital controller.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_clk_sel(adc_continuous_clk_src_t clk_src)
{
    switch (clk_src) {
        case ADC_DIGI_CLK_SRC_XTAL:
            HP_SYS_CLKRST.peri_clk_ctrl22.reg_adc_clk_src_sel = 0;
            break;
        case ADC_DIGI_CLK_SRC_RC_FAST:
            HP_SYS_CLKRST.peri_clk_ctrl22.reg_adc_clk_src_sel = 1;
            break;
        case ADC_DIGI_CLK_SRC_PLL_F80M:
            HP_SYS_CLKRST.peri_clk_ctrl22.reg_adc_clk_src_sel = 2;
            break;
        default:
            HAL_ASSERT(false && "unsupported clock");
    }
    // Enable ADC_CTRL_CLK (i.e. digital domain clock)
    ADC.ctrl_reg.sar_clk_gated = 1;
}

/**
 * Disable clock for ADC digital controller.
 */
static inline void adc_ll_digi_controller_clk_disable(void)
{
    ADC.ctrl_reg.sar_clk_gated = 0;
}

/**
 * Reset adc digital controller filter.
 *
 * @param idx   Filter index
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_filter_reset(adc_digi_iir_filter_t idx, adc_unit_t adc_n)
{
    (void)adc_n;
    ADC.filter_ctrl0.filter_reset = 1;
    ADC.filter_ctrl0.filter_reset = 0;
}

/**
 * Set adc digital controller filter coeff.
 *
 * @param idx      filter index
 * @param adc_n    adc unit
 * @param channel  adc channel
 * @param coeff    filter coeff
 */
static inline void adc_ll_digi_filter_set_factor(adc_digi_iir_filter_t idx, adc_unit_t adc_n, adc_channel_t channel, adc_digi_iir_filter_coeff_t coeff)
{
    uint32_t factor_reg_val = 0;
    switch (coeff) {
        case ADC_DIGI_IIR_FILTER_COEFF_2:
            factor_reg_val = 1;
            break;
        case ADC_DIGI_IIR_FILTER_COEFF_4:
            factor_reg_val = 2;
            break;
        case ADC_DIGI_IIR_FILTER_COEFF_8:
            factor_reg_val = 3;
            break;
        case ADC_DIGI_IIR_FILTER_COEFF_16:
            factor_reg_val = 4;
            break;
        case ADC_DIGI_IIR_FILTER_COEFF_64:
            factor_reg_val = 6;
            break;
        default:
            HAL_ASSERT(false);
    }

    if (idx == ADC_DIGI_IIR_FILTER_0) {
        ADC.filter_ctrl0.filter_channel0 = ((adc_n + 1) << 3) | (channel & 0x7);
        ADC.filter_ctrl1.filter_factor0 = factor_reg_val;
    } else if (idx == ADC_DIGI_IIR_FILTER_1) {
        ADC.filter_ctrl0.filter_channel1 = ((adc_n + 1) << 3) | (channel & 0x7);
        ADC.filter_ctrl1.filter_factor1 = factor_reg_val;
    }
}

/**
 * Enable adc digital controller filter.
 * Filtering the ADC data to obtain smooth data at higher sampling rates.
 *
 * @param idx      filter index
 * @param adc_n    ADC unit
 * @param enable   Enable / Disable
 */
static inline void adc_ll_digi_filter_enable(adc_digi_iir_filter_t idx, adc_unit_t adc_n, bool enable)
{
    (void)adc_n;
    if (!enable) {
        if (idx == ADC_DIGI_IIR_FILTER_0) {
            ADC.filter_ctrl0.filter_channel0 = 0xF;
            ADC.filter_ctrl1.filter_factor0 = 0;
        } else if (idx == ADC_DIGI_IIR_FILTER_1) {
            ADC.filter_ctrl0.filter_channel1 = 0xF;
            ADC.filter_ctrl1.filter_factor1 = 0;
        }
    }
    //nothing to do to enable, after adc_ll_digi_filter_set_factor, it's enabled.
}

/**
 * Set pattern table length for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param patt_len Items range: 1 ~ 16.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_pattern_table_len(adc_unit_t adc_n, uint32_t patt_len)
{
    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl_reg.sar1_patt_len = patt_len - 1;
    } else { // adc_n == ADC_UNIT_2
        ADC.ctrl_reg.sar2_patt_len = patt_len - 1;
    }
}

/**
 * Set pattern table for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 12 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 12 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param pattern_index Items index. Range: 0 ~ 11.
 * @param pattern Stored conversion rules.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_pattern_table(adc_unit_t adc_n, uint32_t pattern_index, adc_digi_pattern_config_t table)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 6;
    adc_ll_digi_pattern_table_t pattern = {0};

    pattern.val = (table.atten & 0x3) | ((table.channel & 0xF) << 2);
    if (table.unit == ADC_UNIT_1){
        tab = ADC.sar1_patt_tab[index].sar1_patt_tab;               //Read old register value
        tab &= (~(0xFC0000 >> offset));                             //Clear old data
        tab |= ((uint32_t)(pattern.val & 0x3F) << 18) >> offset;    //Fill in the new data
        ADC.sar1_patt_tab[index].sar1_patt_tab = tab;               //Write back
    } else {
        tab = ADC.sar2_patt_tab[index].sar2_patt_tab;               //Read old register value
        tab &= (~(0xFC0000 >> offset));                             //clear old data
        tab |= ((uint32_t)(pattern.val & 0x3F) << 18) >> offset;    //Fill in the new data
        ADC.sar2_patt_tab[index].sar2_patt_tab = tab;               //Write back
    }
}


/**
 * Rest pattern table to default value
 */
static inline void adc_ll_digi_reset_pattern_table(void)
{
    for(int i = 0; i < 4; i++) {
        ADC.sar1_patt_tab[i].sar1_patt_tab = 0xffffff;
        ADC.sar2_patt_tab[i].sar2_patt_tab = 0xffffff;
    }
}

/**
 * Reset the pattern table pointer, then take the measurement rule from table header in next measurement.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_clear_pattern_table(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl_reg.sar1_patt_p_clear = 1;
        ADC.ctrl_reg.sar1_patt_p_clear = 0;
    } else { // adc_n == ADC_UNIT_2
        ADC.ctrl_reg.sar2_patt_p_clear = 1;
        ADC.ctrl_reg.sar2_patt_p_clear = 0;
    }
}

/**
 * ADC Digital controller output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
static inline void adc_ll_digi_output_invert(adc_unit_t adc_n, bool inv_en)
{
    if (adc_n == ADC_UNIT_1) {
        ADC.ctrl2.sar1_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_UNIT_2
        ADC.ctrl2.sar2_inv = inv_en;   // Enable / Disable ADC data invert
    }
}

/**
 * Set the interval clock cycle for the digital controller to trigger the measurement.
 * Expression: `trigger_meas_freq` = `controller_clk` / 2 / interval.
 *
 * @note The trigger interval should not be smaller than the sampling time of the SAR ADC.
 * @param cycle The clock cycle (trigger interval) of the measurement. Range: 30 ~ 4095.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_trigger_interval(uint32_t cycle)
{
    ADC.ctrl2.timer_target = cycle;
}

/**
 * Set DMA eof num of adc digital controller.
 * If the number of measurements reaches `dma_eof_num`, then `dma_in_suc_eof` signal is generated.
 *
 * @param num eof num of DMA.
 */
static inline void adc_ll_digi_dma_set_eof_num(uint32_t num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.dma_conf, apb_adc_eof_num, num);
}

/**
 * Enable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_enable(void)
{
    ADC.dma_conf.apb_adc_trans = 1;
}

/**
 * Disable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_disable(void)
{
    ADC.dma_conf.apb_adc_trans = 0;
}

/**
 * Reset adc digital controller.
 */
static inline void adc_ll_digi_reset(void)
{
    ADC.dma_conf.apb_adc_reset_fsm = 1;
    ADC.dma_conf.apb_adc_reset_fsm = 0;
}

/**
 * Enable digital controller timer to trigger the measurement.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_trigger_enable(void)
{
    ADC.ctrl2.timer_sel = 1;
    ADC.ctrl2.timer_en = 1;
}

/**
 * Disable digital controller timer to trigger the measurement.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_trigger_disable(void)
{
    ADC.ctrl2.timer_en = 0;
}

/*---------------------------------------------------------------
                    PWDET(Power detect) controller setting
---------------------------------------------------------------*/

/**
 * Set adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @param cct Range: 0 ~ 7.
 */
static inline void adc_ll_pwdet_set_cct(uint32_t cct)
{
    /* Capacitor tuning of the PA power monitor. cct set to the same value with PHY. */
    LP_ADC.meas2_mux.sar2_pwdet_cct = cct;
}

/**
 * Get adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @return cct Range: 0 ~ 7.
 */
static inline uint32_t adc_ll_pwdet_get_cct(void)
{
    /* Capacitor tuning of the PA power monitor. cct set to the same value with PHY. */
    return LP_ADC.meas2_mux.sar2_pwdet_cct;
}

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/

static inline void _adc_ll_sar1_clock_force_en(bool enable)
{
    HP_SYS_CLKRST.clk_force_on_ctrl0.reg_sar1_clk_force_on = enable;
}

// HP_SYS_CLKRST.clk_force_on_ctrl0 are shared registers, so this function must be used in an atomic way
#define adc_ll_sar1_clock_force_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _adc_ll_sar1_clock_force_en(__VA_ARGS__); \
    } while(0)

static inline void _adc_ll_sar2_clock_force_en(bool enable)
{
    HP_SYS_CLKRST.clk_force_on_ctrl0.reg_sar2_clk_force_on = enable;
}

// HP_SYS_CLKRST.clk_force_on_ctrl0 are shared registers, so this function must be used in an atomic way
#define adc_ll_sar2_clock_force_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _adc_ll_sar2_clock_force_en(__VA_ARGS__); \
    } while(0)

/**
 * @brief Enable the ADC clock
 * @param enable true to enable, false to disable
 */
__attribute__((always_inline))
static inline void _adc_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.soc_clk_ctrl2.reg_adc_apb_clk_en = enable;
    HP_SYS_CLKRST.peri_clk_ctrl23.reg_adc_clk_en = enable;
}
// HP_SYS_CLKRST.soc_clk_ctrl2 are shared registers, so this function must be used in an atomic way
#define adc_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _adc_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset ADC module
 */
__attribute__((always_inline))
static inline void _adc_ll_reset_register(void)
{
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_adc = 1;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_adc = 0;
}
//  HP_SYS_CLKRST.hp_rst_en2 is a shared register, so this function must be used in an atomic way
#define adc_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _adc_ll_reset_register(__VA_ARGS__); \
    } while(0)



/**
 * Set ADC digital controller power management.
 *
 * @param adc_n ADC unit.
 * @param manage Set ADC power status.
 */
__attribute__((always_inline))
static inline void adc_ll_digi_set_power_manage(adc_unit_t adc_n, adc_ll_power_t manage)
{
    if (adc_n == ADC_UNIT_1) {
        /* Bit1  0:Fsm  1: SW mode
        Bit0  0:SW mode power down  1: SW mode power on */
        if (manage == ADC_LL_POWER_SW_ON) {
            ADC.ctrl_reg.sar_clk_gated = 1;
            ADC.ctrl_reg.xpd_sar1_force = LP_ADC_FORCE_XPD_SAR_PU;
        } else if (manage == ADC_LL_POWER_BY_FSM) {
            ADC.ctrl_reg.sar_clk_gated = 1;
            ADC.ctrl_reg.xpd_sar1_force = LP_ADC_FORCE_XPD_SAR_FSM;
        } else if (manage == ADC_LL_POWER_SW_OFF) {
            ADC.ctrl_reg.sar_clk_gated = 0;
            ADC.ctrl_reg.xpd_sar1_force = LP_ADC_FORCE_XPD_SAR_PD;
        }
    } else {
        /* Bit1  0:Fsm  1: SW mode
        Bit0  0:SW mode power down  1: SW mode power on */
        if (manage == ADC_LL_POWER_SW_ON) {
            ADC.ctrl_reg.sar_clk_gated = 1;
            ADC.ctrl_reg.xpd_sar2_force = LP_ADC_FORCE_XPD_SAR_PU;
        } else if (manage == ADC_LL_POWER_BY_FSM) {
            ADC.ctrl_reg.sar_clk_gated = 1;
            ADC.ctrl_reg.xpd_sar2_force = LP_ADC_FORCE_XPD_SAR_FSM;
        } else if (manage == ADC_LL_POWER_SW_OFF) {
            ADC.ctrl_reg.sar_clk_gated = 0;
            ADC.ctrl_reg.xpd_sar2_force = LP_ADC_FORCE_XPD_SAR_PD;
        }
    }
}

/**
 * Set ADC module power management.
 *
 * @param adc_n ADC unit.
 * @param manage Set ADC power status.
 */
static inline void adc_ll_set_power_manage(adc_unit_t adc_n, adc_ll_power_t manage)
{
    adc_ll_digi_set_power_manage(adc_n, manage);
    if (adc_n == ADC_UNIT_1) {
        /* Bit1  0:Fsm  1: SW mode
        Bit0  0:SW mode power down  1: SW mode power on */
        if (manage == ADC_LL_POWER_SW_ON) {
            LPPERI.clk_en.ck_en_lp_adc = 1;
            LP_ADC.force_wpd_sar.force_xpd_sar1 = LP_ADC_FORCE_XPD_SAR_PU;
        } else if (manage == ADC_LL_POWER_BY_FSM) {
            LPPERI.clk_en.ck_en_lp_adc = 1;
            LP_ADC.force_wpd_sar.force_xpd_sar1 = LP_ADC_FORCE_XPD_SAR_FSM;
        } else if (manage == ADC_LL_POWER_SW_OFF) {
            LP_ADC.force_wpd_sar.force_xpd_sar1 = LP_ADC_FORCE_XPD_SAR_PD;
            LPPERI.clk_en.ck_en_lp_adc = 0;
        }
    } else {
        /* Bit1  0:Fsm  1: SW mode
        Bit0  0:SW mode power down  1: SW mode power on */
        if (manage == ADC_LL_POWER_SW_ON) {
            LPPERI.clk_en.ck_en_lp_adc = 1;
            LP_ADC.force_wpd_sar.force_xpd_sar2 = LP_ADC_FORCE_XPD_SAR_PU;
        } else if (manage == ADC_LL_POWER_BY_FSM) {
            LPPERI.clk_en.ck_en_lp_adc = 1;
            LP_ADC.force_wpd_sar.force_xpd_sar2 = LP_ADC_FORCE_XPD_SAR_FSM;
        } else if (manage == ADC_LL_POWER_SW_OFF) {
            LP_ADC.force_wpd_sar.force_xpd_sar2 = LP_ADC_FORCE_XPD_SAR_PD;
            LPPERI.clk_en.ck_en_lp_adc = 0;
        }
    }
}


/**
 * Set ADC module controller.
 * There are five SAR ADC controllers:
 * Two digital controller: Continuous conversion mode (DMA). High performance with multiple channel scan modes;
 * Two RTC controller: Single conversion modes (Polling). For low power purpose working during deep sleep;
 * the other is dedicated for Power detect (PWDET / PKDET), Only support ADC2.
 *
 * @param adc_n ADC unit.
 * @param ctrl ADC controller.
 */
__attribute__((always_inline))
static inline void adc_ll_set_controller(adc_unit_t adc_n, adc_ll_controller_t ctrl)
{
    if (adc_n == ADC_UNIT_1) {
        switch (ctrl) {
            case ADC_LL_CTRL_RTC:
                LP_ADC.meas1_mux.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
                LP_ADC.meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                LP_ADC.meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            case ADC_LL_CTRL_ULP:
                LP_ADC.meas1_mux.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
                LP_ADC.meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                LP_ADC.meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            case ADC_LL_CTRL_DIG:
                LP_ADC.meas1_mux.sar1_dig_force       = 1;    // 1: Select digital control;       0: Select RTC control.
                LP_ADC.meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                LP_ADC.meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            default:
                break;
        }
    } else { // adc_n == ADC_UNIT_2
        switch (ctrl) {
            case ADC_LL_CTRL_RTC:
                LP_ADC.meas2_mux.sar2_rtc_force       = 1;    // 1: Select digital control;       0: Select RTC control.
                LP_ADC.meas2_ctrl2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                LP_ADC.meas2_ctrl2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            case ADC_LL_CTRL_ULP:
                LP_ADC.meas2_mux.sar2_rtc_force       = 0;    // 1: Select digital control;       0: Select RTC control.
                LP_ADC.meas2_ctrl2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                LP_ADC.meas2_ctrl2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            case ADC_LL_CTRL_DIG:
                LP_ADC.meas2_mux.sar2_rtc_force       = 0;    // 1: Select digital control;       0: Select RTC control.
                LP_ADC.meas2_ctrl2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                LP_ADC.meas2_ctrl2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
                break;
            default:
                break;
        }
    }
}

/*---------------------------------------------------------------
                    Calibration
---------------------------------------------------------------*/

/**
 * @brief Set common calibration configuration. Should be shared with other parts (PWDET).
 */
__attribute__((always_inline))
static inline void adc_ll_calibration_init(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_DREF_ADDR, 4);
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_DREF_ADDR, 4);
    }
}

/**
 * Configure the registers for ADC calibration. You need to call the ``adc_ll_calibration_finish`` interface to resume after calibration.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 * @param internal_gnd true:  Disconnect from the IO port and use the internal GND as the calibration voltage.
 *                     false: Use IO external voltage as calibration voltage.
 */
static inline void adc_ll_calibration_prepare(adc_unit_t adc_n, bool internal_gnd)
{
    /* Enable/disable internal connect GND (for calibration). */
    if (adc_n == ADC_UNIT_1) {
        if (internal_gnd) {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 1);
        } else {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 0);
        }
    } else {
        if (internal_gnd) {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_ENCAL_GND_ADDR, 1);
        } else {
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_ENCAL_GND_ADDR, 0);
        }
    }
}

/**
 * Resume register status after calibration.
 *
 * @param adc_n ADC index number.
 */
static inline void adc_ll_calibration_finish(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 0);
    } else {    //adc_n == ADC_UNIT_2
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_ENCAL_GND_ADDR, 0);
    }
}

/**
 * Set the calibration result to ADC.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 */
__attribute__((always_inline))
static inline void adc_ll_set_calibration_param(adc_unit_t adc_n, uint32_t param)
{
    uint8_t msb = param >> 8;
    uint8_t lsb = param & 0xFF;

    if (adc_n == ADC_UNIT_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_HIGH_ADDR, msb);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_LOW_ADDR, lsb);
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_HIGH_ADDR, msb);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_LOW_ADDR, lsb);
    }
}

/**
 * Set the SAR DTEST param
 *
 * @param param DTEST value
 */
__attribute__((always_inline))
static inline void adc_ll_set_dtest_param(uint32_t param)
{
    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SAR_ADC_DTEST_VDD_GRP1, param);
}

/**
 * Set the SAR ENT param
 *
 * @param param ENT value
 */
__attribute__((always_inline))
static inline void adc_ll_set_ent_param(uint32_t param)
{
    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SAR_ADC_ENT_VDD_GRP1, param);
}

/**
 * Init regi2c SARADC registers
 */
__attribute__((always_inline))
static inline void adc_ll_regi2c_init(void)
{
    adc_ll_set_dtest_param(0);
    adc_ll_set_ent_param(1);
}

/**
 * Deinit regi2c SARADC registers
 */
__attribute__((always_inline))
static inline void adc_ll_regi2c_adc_deinit(void)
{
    adc_ll_set_dtest_param(0);
    adc_ll_set_ent_param(0);
}

/*---------------------------------------------------------------
                    Oneshot Read
---------------------------------------------------------------*/

static inline void adc_ll_vref_output(adc_unit_t adc, adc_channel_t channel, bool en)
{
    abort();
}

/**
 * Set adc output data format for RTC controller.
 *
 * @note ESP32P4 RTC controller only support 12bit.
 * @param adc_n ADC unit.
 * @param bits Output data bits width option.
 */
static inline void adc_oneshot_ll_set_output_bits(adc_unit_t adc_n, adc_bitwidth_t bits)
{
    //ESP32P4 only supports 12bit, leave here for compatibility
    HAL_ASSERT(bits == ADC_BITWIDTH_12 || bits == ADC_BITWIDTH_DEFAULT);
}

/**
 * Enable adc channel to start convert.
 *
 * @note Only one channel can be selected for once measurement.
 *
 * @param adc_n ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_oneshot_ll_set_channel(adc_unit_t adc_n, adc_channel_t channel)
{
    if (adc_n == ADC_UNIT_1) {
        LP_ADC.meas1_ctrl2.sar1_en_pad = (1 << channel); //only one channel is selected.
    } else { // adc_n == ADC_UNIT_2
        LP_ADC.meas2_ctrl2.sar2_en_pad = (1 << ((channel + 2))); //only one channel is selected.
    }
}

/**
 * Disable adc channel to start convert.
 *
 * @note Only one channel can be selected in once measurement.
 *
 * @param adc_n ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_oneshot_ll_disable_channel(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        LP_ADC.meas1_ctrl2.sar1_en_pad = 0; //only one channel is selected.
    } else { // adc_n == ADC_UNIT_2
        LP_ADC.meas2_ctrl2.sar2_en_pad = 0; //only one channel is selected.
    }
}

/**
 * Start conversion once by software for RTC controller.
 *
 * @note It may be block to wait conversion idle for ADC1.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_oneshot_ll_start(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        LP_ADC.meas1_ctrl2.meas1_start_sar = 0;
        LP_ADC.meas1_ctrl2.meas1_start_sar = 1;
    } else { // adc_n == ADC_UNIT_2
        LP_ADC.meas2_ctrl2.meas2_start_sar = 0;
        LP_ADC.meas2_ctrl2.meas2_start_sar = 1;
    }
}

/**
 * Clear the event for each ADCn for Oneshot mode
 *
 * @param event ADC event
 */
static inline void adc_oneshot_ll_clear_event(uint32_t event_mask)
{
    // ADC.int_clr.val |= event_mask;
}

/**
 * Check the event for each ADCn for Oneshot mode
 *
 * @param event ADC event
 *
 * @return
 *      -true  : The conversion process is finish.
 *      -false : The conversion process is not finish.
 */
static inline bool adc_oneshot_ll_get_event(uint32_t event)
{
    bool ret = true;
    if (event == ADC_LL_EVENT_ADC1_ONESHOT_DONE) {
        ret = (bool)LP_ADC.meas1_ctrl2.meas1_done_sar;
    } else if (event == ADC_LL_EVENT_ADC2_ONESHOT_DONE) {
        ret = (bool)LP_ADC.meas2_ctrl2.meas2_done_sar;
    } else {
        HAL_ASSERT(false);
    }
    return ret;
}

/**
 * Get the converted value for each ADCn for RTC controller.
 *
 * @param adc_n ADC unit.
 * @return
 *      - Converted value.
 */
static inline uint32_t adc_oneshot_ll_get_raw_result(adc_unit_t adc_n)
{
    uint32_t ret_val = 0;
    if (adc_n == ADC_UNIT_1) {
        ret_val = HAL_FORCE_READ_U32_REG_FIELD(LP_ADC.meas1_ctrl2, meas1_data_sar);
    } else { // adc_n == ADC_UNIT_2
        ret_val = HAL_FORCE_READ_U32_REG_FIELD(LP_ADC.meas2_ctrl2, meas2_data_sar);
    }
    return ret_val;
}

/**
 * Analyze whether the obtained raw data is correct.
 *
 * @param adc_n ADC unit.
 * @param raw   ADC raw data input (convert value).
 * @return
 *      - true: raw data is valid
 */
static inline bool adc_oneshot_ll_raw_check_valid(adc_unit_t adc_n, uint32_t raw)
{
    /* No arbiter, don't need check data */
    return true;
}

/**
 * ADC module RTC output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
static inline void adc_oneshot_ll_output_invert(adc_unit_t adc_n, bool inv_en)
{
    if (adc_n == ADC_UNIT_1) {
        LP_ADC.reader1_ctrl.sar1_data_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_UNIT_2
        LP_ADC.reader2_ctrl.sar2_data_inv = inv_en;  // Enable / Disable ADC data invert
    }
}

/**
 * Enable oneshot conversion trigger
 *
 * @param adc_n  ADC unit
 */
static inline void adc_oneshot_ll_enable(adc_unit_t adc_n)
{
    (void)adc_n;
    //For compatibility
}

/**
 * Disable oneshot conversion trigger for all the ADC units
 */
static inline void adc_oneshot_ll_disable_all_unit(void)
{
    //For compatibility
}

/*---------------------------------------------------------------
                    RTC controller setting
---------------------------------------------------------------*/
/**
 * ADC SAR clock division factor setting. ADC SAR clock divided from `RTC_FAST_CLK`.
 *
 * @param div Division factor.
 */
static inline void adc_ll_set_sar_clk_div(adc_unit_t adc_n, uint32_t div)
{
    if (adc_n == ADC_UNIT_1) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_ADC.reader1_ctrl, sar1_clk_div, div);
    } else { // adc_n == ADC_UNIT_2
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_ADC.reader2_ctrl, sar2_clk_div, div);
    }
}

/**
 * Reset RTC controller FSM.
 */
static inline void adc_ll_rtc_reset(void)
{
    LPPERI.reset_en.rst_en_lp_adc = 1;
    LPPERI.reset_en.rst_en_lp_adc = 0;
}


/**
 * Set the attenuation of a particular channel on ADCn.
 *
 * @note For any given channel, this function must be called before the first time conversion.
 *
 * The default ADC full-scale voltage is 1.1V. To read higher voltages (up to the pin maximum voltage,
 * usually 3.3V) requires setting >0dB signal attenuation for that ADC channel.
 *
 * When VDD_A is 3.3V:
 *
 * - 0dB attenuation (ADC_ATTEN_DB_0) gives full-scale voltage 1.1V
 * - 2.5dB attenuation (ADC_ATTEN_DB_2_5) gives full-scale voltage 1.5V
 * - 6dB attenuation (ADC_ATTEN_DB_6) gives full-scale voltage 2.2V
 * - 11dB attenuation (ADC_ATTEN_DB_12) gives full-scale voltage 3.9V (see note below)
 *
 * @note The full-scale voltage is the voltage corresponding to a maximum reading (depending on ADC1 configured
 * bit width, this value is: 4095 for 12-bits, 2047 for 11-bits, 1023 for 10-bits, 511 for 9 bits.)
 *
 * @note At 11dB attenuation the maximum voltage is limited by VDD_A, not the full scale voltage.
 *
 * Due to ADC characteristics, most accurate results are obtained within the following approximate voltage ranges:
 *
 * - 0dB attenuation (ADC_ATTEN_DB_0) between 100 and 950mV
 * - 2.5dB attenuation (ADC_ATTEN_DB_2_5) between 100 and 1250mV
 * - 6dB attenuation (ADC_ATTEN_DB_6) between 150 to 1750mV
 * - 11dB attenuation (ADC_ATTEN_DB_12) between 150 to 2450mV
 *
 * For maximum accuracy, use the ADC calibration APIs and measure voltages within these recommended ranges.
 *
 * @param adc_n ADC unit.
 * @param channel ADCn channel number.
 * @param atten The attenuation option.
 */
static inline void adc_oneshot_ll_set_atten(adc_unit_t adc_n, adc_channel_t channel, adc_atten_t atten)
{
    if (adc_n == ADC_UNIT_1) {
        LP_ADC.atten1.sar1_atten = ( LP_ADC.atten1.sar1_atten & ~(0x3 << (channel * 2)) ) | ((atten & 0x3) << (channel * 2));
    } else { // adc_n == ADC_UNIT_2
        LP_ADC.atten2.sar2_atten = ( LP_ADC.atten2.sar2_atten & ~(0x3 << ((channel + 2) * 2)) ) | ((atten & 0x3) << ((channel + 2) * 2));
    }
}

/**
 * Get the attenuation of a particular channel on ADCn.
 *
 * @param adc_n ADC unit.
 * @param channel ADCn channel number.
 * @return atten The attenuation option.
 */
__attribute__((always_inline))
static inline adc_atten_t adc_ll_get_atten(adc_unit_t adc_n, adc_channel_t channel)
{
    if (adc_n == ADC_UNIT_1) {
        return (adc_atten_t)((LP_ADC.atten1.sar1_atten >> (channel * 2)) & 0x3);
    } else {
        return (adc_atten_t)((LP_ADC.atten2.sar2_atten >> ((channel + 2) * 2)) & 0x3);
    }
}

#ifdef __cplusplus
}
#endif
