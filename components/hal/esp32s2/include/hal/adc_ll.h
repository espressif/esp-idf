#pragma once

#include <stdbool.h>
#include "soc/adc_periph.h"
#include "hal/adc_types.h"
#include "soc/apb_saradc_struct.h"
#include "soc/apb_saradc_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "regi2c_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ADC_NUM_1 = 0,          /*!< SAR ADC 1 */
    ADC_NUM_2 = 1,          /*!< SAR ADC 2 */
    ADC_NUM_MAX,
} adc_ll_num_t;

typedef enum {
    ADC_POWER_BY_FSM,   /*!< ADC XPD controled by FSM. Used for polling mode */
    ADC_POWER_SW_ON,    /*!< ADC XPD controled by SW. power on. Used for DMA mode */
    ADC_POWER_SW_OFF,   /*!< ADC XPD controled by SW. power off. */
    ADC_POWER_MAX,      /*!< For parameter check. */
} adc_ll_power_t;

typedef enum {
    ADC_RTC_DATA_OK = 0,
    ADC_RTC_CTRL_UNSELECTED = 1,
    ADC_RTC_CTRL_BREAK = 2,
    ADC_RTC_DATA_FAIL = -1,
} adc_ll_rtc_raw_data_t;

/**
 * @brief Analyze whether the obtained raw data is correct.
 *        ADC2 use arbiter by default. The arbitration result can be judged by the flag bit in the original data.
 *
 */
typedef struct {
    union {
        struct {
            uint16_t data:     13;  /*!<ADC real output data info. Resolution: 13 bit. */
            uint16_t reserved:  1;  /*!<reserved */
            uint16_t flag:      2;  /*!<ADC data flag info.
                                        If (flag == 0), The data is valid.
                                        If (flag > 0), The data is invalid. */
        };
        uint16_t val;
    };
} adc_ll_rtc_output_data_t;

/**
 * @brief ADC controller type selection.
 *
 * @note For ADC2, use the force option with care. The system power consumption detection will use ADC2.
 *       If it is forced to switch to another controller, it may cause the system to obtain incorrect values.
 * @note Normally, there is no need to switch the controller manually.
 */
typedef enum {
    ADC_CTRL_RTC = 0,   /*!<For ADC1. Select RTC controller. For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC_CTRL_ULP = 1,   /*!<For ADC1. Select ULP controller. For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC_CTRL_DIG = 2,   /*!<For ADC1. Select DIG controller. For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC2_CTRL_PWDET = 3,/*!<For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC2_CTRL_FORCE_PWDET = 3,  /*!<For ADC2. Arbiter in shield mode. Force select Wi-Fi controller work. */
    ADC2_CTRL_FORCE_RTC = 4,    /*!<For ADC2. Arbiter in shield mode. Force select RTC controller work. */
    ADC2_CTRL_FORCE_ULP = 5,    /*!<For ADC2. Arbiter in shield mode. Force select RTC controller work. */
    ADC2_CTRL_FORCE_DIG = 6,    /*!<For ADC2. Arbiter in shield mode. Force select digital controller work. */
} adc_ll_controller_t;

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
    APB_SARADC.fsm_wait.rstb_wait = rst_wait;
    // Internal FSM start wait time
    APB_SARADC.fsm_wait.xpd_wait = start_wait;
    // Internal FSM standby wait time
    APB_SARADC.fsm_wait.standby_wait = standby_wait;
}

/**
 * Set adc sample cycle.
 *
 * @note Normally, please use default value.
 * @param sample_cycle The number of ADC sampling cycles. Range: 1 ~ 7.
 */
static inline void adc_ll_set_sample_cycle(uint32_t sample_cycle)
{
    /* Should be called before writing I2C registers. */
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PU_M);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_SAR_M);
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_SAR_CFG2_M);

    REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_SAMPLE_CYCLE_ADDR, sample_cycle);
}

/**
 * Set SAR ADC module clock division factor.
 * SAR ADC clock divided from digital controller clock.
 *
 * @param div Division factor.
 */
static inline void adc_ll_digi_set_clk_div(uint32_t div)
{
    /* ADC clock devided from digital controller clock clk */
    APB_SARADC.ctrl.sar_clk_div = div;
}

/**
 * Set adc output data format for digital controller.
 *
 * @param format Output data format.
 */
static inline void adc_ll_digi_set_output_format(adc_digi_output_format_t format)
{
    APB_SARADC.ctrl.data_sar_sel = format;
}

/**
 * Set adc max conversion number for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param meas_num Max conversion number. Range: 0 ~ 255.
 */
static inline void adc_ll_digi_set_convert_limit_num(uint32_t meas_num)
{
    APB_SARADC.ctrl2.max_meas_num = meas_num;
}

/**
 * Enable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 */
static inline void adc_ll_digi_convert_limit_enable(void)
{
    APB_SARADC.ctrl2.meas_num_limit = 1;
}

/**
 * Disable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 */
static inline void adc_ll_digi_convert_limit_disable(void)
{
    APB_SARADC.ctrl2.meas_num_limit = 0;
}

/**
 * Set adc conversion mode for digital controller.
 *
 * @note ESP32 only support ADC1 single mode.
 *
 * @param mode Conversion mode select.
 */
static inline void adc_ll_digi_set_convert_mode(adc_digi_convert_mode_t mode)
{
    if (mode == ADC_CONV_SINGLE_UNIT_1) {
        APB_SARADC.ctrl.work_mode = 0;
        APB_SARADC.ctrl.sar_sel = 0;
    } else if (mode == ADC_CONV_SINGLE_UNIT_2) {
        APB_SARADC.ctrl.work_mode = 0;
        APB_SARADC.ctrl.sar_sel = 1;
    } else if (mode == ADC_CONV_BOTH_UNIT) {
        APB_SARADC.ctrl.work_mode = 1;
    } else if (mode == ADC_CONV_ALTER_UNIT) {
        APB_SARADC.ctrl.work_mode = 2;
    }
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
static inline void adc_ll_digi_set_pattern_table_len(adc_ll_num_t adc_n, uint32_t patt_len)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.ctrl.sar1_patt_len = patt_len - 1;
    } else { // adc_n == ADC_NUM_2
        APB_SARADC.ctrl.sar2_patt_len = patt_len - 1;
    }
}

/**
 * Set pattern table for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param pattern_index Items index. Range: 0 ~ 15.
 * @param pattern Stored conversion rules.
 */
static inline void adc_ll_digi_set_pattern_table(adc_ll_num_t adc_n, uint32_t pattern_index, adc_digi_pattern_table_t pattern)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 8;
    if (adc_n == ADC_NUM_1) {
        tab = APB_SARADC.sar1_patt_tab[index];  // Read old register value
        tab &= (~(0xFF000000 >> offset));       // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        APB_SARADC.sar1_patt_tab[index] = tab;  // Write back
    } else { // adc_n == ADC_NUM_2
        tab = APB_SARADC.sar2_patt_tab[index];  // Read old register value
        tab &= (~(0xFF000000 >> offset));       // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        APB_SARADC.sar2_patt_tab[index] = tab;  // Write back
    }
}

/**
 * Reset the pattern table pointer, then take the measurement rule from table header in next measurement.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_clear_pattern_table(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.ctrl.sar1_patt_p_clear = 1;
        APB_SARADC.ctrl.sar1_patt_p_clear = 0;
    } else { // adc_n == ADC_NUM_2
        APB_SARADC.ctrl.sar2_patt_p_clear = 1;
        APB_SARADC.ctrl.sar2_patt_p_clear = 0;
    }
}

/**
 * Sets the number of cycles required for the conversion to complete and wait for the arbiter to stabilize.
 *
 * @note Only ADC2 have arbiter function.
 * @param cycle range: 0 ~ 4.
 */
static inline void adc_ll_digi_set_arbiter_stable_cycle(uint32_t cycle)
{
    APB_SARADC.ctrl.wait_arb_cycle = cycle;
}

/**
 * ADC Digital controller output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
static inline void adc_ll_digi_output_invert(adc_ll_num_t adc_n, bool inv_en)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.ctrl2.sar1_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_NUM_2
        APB_SARADC.ctrl2.sar2_inv = inv_en;   // Enable / Disable ADC data invert
    }
}

/**
 * Set the interval clock cycle for the digital controller to trigger the measurement.
 * Expression: `trigger_meas_freq` = `controller_clk` / 2 / interval.
 *
 * @note The trigger interval should be larger than the sampling time of the SAR ADC.
 * @param cycle The clock cycle (trigger interval) of the measurement. Range: 40 ~ 4095.
 */
static inline void adc_ll_digi_set_trigger_interval(uint32_t cycle)
{
    APB_SARADC.ctrl2.timer_target = cycle;
}

/**
 * Enable digital controller timer to trigger the measurement.
 */
static inline void adc_ll_digi_trigger_enable(void)
{
    APB_SARADC.ctrl2.timer_sel = 1;
    APB_SARADC.ctrl2.timer_en = 1;
}

/**
 * Disable digital controller timer to trigger the measurement.
 */
static inline void adc_ll_digi_trigger_disable(void)
{
    APB_SARADC.ctrl2.timer_en = 0;
    APB_SARADC.ctrl2.timer_sel = 0;
}

/**
 * Set ADC digital controller clock division factor. The clock divided from `APLL` or `APB` clock.
 * Expression: controller_clk = (`APLL` or `APB`) / (div_num + div_a / div_b + 1).
 *
 * @param div_num Division factor. Range: 0 ~ 255.
 * @param div_b Division factor. Range: 1 ~ 63.
 * @param div_a Division factor. Range: 0 ~ 63.
 */
static inline void adc_ll_digi_controller_clk_div(uint32_t div_num, uint32_t div_b, uint32_t div_a)
{
    APB_SARADC.apb_adc_clkm_conf.clkm_div_num = div_num;
    APB_SARADC.apb_adc_clkm_conf.clkm_div_b = div_b;
    APB_SARADC.apb_adc_clkm_conf.clkm_div_a = div_a;
}

/**
 * Enable clock and select clock source for ADC digital controller.
 *
 * @param use_apll true: use APLL clock; false: use APB clock.
 */
static inline void adc_ll_digi_controller_clk_enable(bool use_apll)
{
    if (use_apll) {
        APB_SARADC.apb_adc_clkm_conf.clk_sel = 1;   // APLL clock
    } else {
        APB_SARADC.apb_adc_clkm_conf.clk_sel = 2;   // APB clock
    }
    APB_SARADC.ctrl.sar_clk_gated = 1;
}

/**
 * Disable clock for ADC digital controller.
 */
static inline void adc_ll_digi_controller_clk_disable(void)
{
    APB_SARADC.ctrl.sar_clk_gated = 0;
    APB_SARADC.apb_adc_clkm_conf.clk_sel = 0;
}

/**
 * Reset adc digital controller filter.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_filter_reset(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.filter_ctrl.adc1_filter_reset = 1;
        APB_SARADC.filter_ctrl.adc1_filter_reset = 0;
    } else { // adc_n == ADC_NUM_2
        APB_SARADC.filter_ctrl.adc2_filter_reset = 1;
        APB_SARADC.filter_ctrl.adc2_filter_reset = 0;
    }
}

/**
 * Set adc digital controller filter factor.
 *
 * @param adc_n ADC unit.
 * @param factor Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
static inline void adc_ll_digi_filter_set_factor(adc_ll_num_t adc_n, adc_digi_filter_mode_t factor)
{
    int mode = 0;
    switch (factor) {
    case ADC_DIGI_FILTER_IIR_2:  mode = 2;  break;
    case ADC_DIGI_FILTER_IIR_4:  mode = 4;  break;
    case ADC_DIGI_FILTER_IIR_8:  mode = 8;  break;
    case ADC_DIGI_FILTER_IIR_16: mode = 16; break;
    case ADC_DIGI_FILTER_IIR_64: mode = 64; break;
    default: mode = 8; break;
    }
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.filter_ctrl.adc1_filter_factor = mode;
    } else { // adc_n == ADC_NUM_2
        APB_SARADC.filter_ctrl.adc2_filter_factor = mode;
    }
}

/**
 * Get adc digital controller filter factor.
 *
 * @param adc_n ADC unit.
 * @param factor Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
static inline void adc_ll_digi_filter_get_factor(adc_ll_num_t adc_n, adc_digi_filter_mode_t *factor)
{
    int mode = 0;
    if (adc_n == ADC_NUM_1) {
        mode = APB_SARADC.filter_ctrl.adc1_filter_factor;
    } else { // adc_n == ADC_NUM_2
        mode = APB_SARADC.filter_ctrl.adc2_filter_factor;
    }
    switch (mode) {
    case 2:  *factor = ADC_DIGI_FILTER_IIR_2;  break;
    case 4:  *factor = ADC_DIGI_FILTER_IIR_4;  break;
    case 8:  *factor = ADC_DIGI_FILTER_IIR_8;  break;
    case 16: *factor = ADC_DIGI_FILTER_IIR_16; break;
    case 64: *factor = ADC_DIGI_FILTER_IIR_64; break;
    default: *factor = ADC_DIGI_FILTER_IIR_MAX;    break;
    }
}

/**
 * Enable/disable adc digital controller filter.
 * Filtering the ADC data to obtain smooth data at higher sampling rates.
 *
 * @note The filter will filter all the enabled channel data of the each ADC unit at the same time.
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_filter_enable(adc_ll_num_t adc_n, bool enable)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.filter_ctrl.adc1_filter_en = enable;
    } else { // adc_n == ADC_NUM_2
        APB_SARADC.filter_ctrl.adc2_filter_en = enable;
    }
}

/**
 * Get the filtered data of adc digital controller filter.
 * The data after each measurement and filtering is updated to the DMA by the digital controller. But it can also be obtained manually through this API.
 *
 * @note The filter will filter all the enabled channel data of the each ADC unit at the same time.
 * @param adc_n ADC unit.
 * @return Filtered data.
 */
static inline uint32_t adc_ll_digi_filter_read_data(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
        return APB_SARADC.filter_status.adc1_filter_data;
    } else { // adc_n == ADC_NUM_2
        return APB_SARADC.filter_status.adc2_filter_data;
    }
}

/**
 * Set monitor mode of adc digital controller.
 *
 * @note The monitor will monitor all the enabled channel data of the each ADC unit at the same time.
 * @param adc_n ADC unit.
 * @param is_larger true:  If ADC_OUT >  threshold, Generates monitor interrupt.
 *                  false: If ADC_OUT <  threshold, Generates monitor interrupt.
 */
static inline void adc_ll_digi_monitor_set_mode(adc_ll_num_t adc_n, bool is_larger)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.thres_ctrl.adc1_thres_mode = is_larger;
    } else { // adc_n == ADC_NUM_2
        APB_SARADC.thres_ctrl.adc2_thres_mode = is_larger;
    }
}

/**
 * Set monitor threshold of adc digital controller.
 *
 * @note The monitor will monitor all the enabled channel data of the each ADC unit at the same time.
 * @param adc_n ADC unit.
 * @param threshold Monitor threshold.
 */
static inline void adc_ll_digi_monitor_set_thres(adc_ll_num_t adc_n, uint32_t threshold)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.thres_ctrl.adc1_thres = threshold;
    } else { // adc_n == ADC_NUM_2
        APB_SARADC.thres_ctrl.adc2_thres = threshold;
    }
}

/**
 * Enable/disable monitor of adc digital controller.
 *
 * @note The monitor will monitor all the enabled channel data of the each ADC unit at the same time.
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_monitor_enable(adc_ll_num_t adc_n, bool enable)
{
    if (adc_n == ADC_NUM_1) {
        APB_SARADC.thres_ctrl.adc1_thres_en = enable;
    } else { // adc_n == ADC_NUM_2
        APB_SARADC.thres_ctrl.adc2_thres_en = enable;
    }
}

/**
 * Enable interrupt of adc digital controller by bitmask.
 *
 * @param adc_n ADC unit.
 * @param intr Interrupt bitmask.
 */
static inline void adc_ll_digi_intr_enable(adc_ll_num_t adc_n, adc_digi_intr_t intr)
{
    if (adc_n == ADC_NUM_1) {
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_ena.adc1_thres = 1;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_ena.adc1_done = 1;
        }
    } else { // adc_n == ADC_NUM_2
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_ena.adc2_thres = 1;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_ena.adc2_done = 1;
        }
    }
}

/**
 * Disable interrupt of adc digital controller by bitmask.
 *
 * @param adc_n ADC unit.
 * @param intr Interrupt bitmask.
 */
static inline void adc_ll_digi_intr_disable(adc_ll_num_t adc_n, adc_digi_intr_t intr)
{
    if (adc_n == ADC_NUM_1) {
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_ena.adc1_thres = 0;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_ena.adc1_done = 0;
        }
    } else { // adc_n == ADC_NUM_2
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_ena.adc2_thres = 0;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_ena.adc2_done = 0;
        }
    }
}

/**
 * Clear interrupt of adc digital controller by bitmask.
 *
 * @param adc_n ADC unit.
 * @param intr Interrupt bitmask.
 */
static inline void adc_ll_digi_intr_clear(adc_ll_num_t adc_n, adc_digi_intr_t intr)
{
    if (adc_n == ADC_NUM_1) {
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_clr.adc1_thres = 1;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_clr.adc1_done = 1;
        }
    } else { // adc_n == ADC_NUM_2
        if (intr & ADC_DIGI_INTR_MASK_MONITOR) {
            APB_SARADC.int_clr.adc2_thres = 1;
        }
        if (intr & ADC_DIGI_INTR_MASK_MEAS_DONE) {
            APB_SARADC.int_clr.adc2_done = 1;
        }
    }
}

/**
 * Get interrupt status mask of adc digital controller.
 *
 * @param adc_n ADC unit.
 * @return
 *     - intr Interrupt bitmask.
 */
static inline uint32_t adc_ll_digi_get_intr_status(adc_ll_num_t adc_n)
{
    uint32_t int_st = APB_SARADC.int_st.val;
    uint32_t ret_msk = 0;

    if (adc_n == ADC_NUM_1) {
        if (int_st & APB_SARADC_ADC1_DONE_INT_ST_M) {
            ret_msk |= ADC_DIGI_INTR_MASK_MEAS_DONE;
        }
        if (int_st & APB_SARADC_ADC1_THRES_INT_ST) {
            ret_msk |= ADC_DIGI_INTR_MASK_MONITOR;
        }
    } else { // adc_n == ADC_NUM_2
        if (int_st & APB_SARADC_ADC2_DONE_INT_ST_M) {
            ret_msk |= ADC_DIGI_INTR_MASK_MEAS_DONE;
        }
        if (int_st & APB_SARADC_ADC2_THRES_INT_ST_M) {
            ret_msk |= ADC_DIGI_INTR_MASK_MONITOR;
        }
    }

    return ret_msk;
}

/**
 * Set DMA eof num of adc digital controller.
 * If the number of measurements reaches `dma_eof_num`, then `dma_in_suc_eof` signal is generated.
 *
 * @param num eof num of DMA.
 */
static inline void adc_ll_digi_dma_set_eof_num(uint32_t num)
{
    APB_SARADC.dma_conf.apb_adc_eof_num = num;
}

/**
 * Enable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_enable(void)
{
    APB_SARADC.dma_conf.apb_adc_trans = 1;
}

/**
 * Disable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_disable(void)
{
    APB_SARADC.dma_conf.apb_adc_trans = 0;
}

/**
 * Reset adc digital controller.
 */
static inline void adc_ll_digi_reset(void)
{
    APB_SARADC.dma_conf.apb_adc_reset_fsm = 1;
    APB_SARADC.dma_conf.apb_adc_reset_fsm = 0;
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
    SENS.sar_meas2_mux.sar2_pwdet_cct = cct;
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
    return SENS.sar_meas2_mux.sar2_pwdet_cct;
}

/*---------------------------------------------------------------
                    RTC controller setting
---------------------------------------------------------------*/
/**
 * Set adc output data format for RTC controller.
 *
 * @note ESP32S2 RTC controller only support 13bit.
 * @prarm adc_n ADC unit.
 * @prarm bits Output data bits width option.
 */
static inline void adc_ll_rtc_set_output_format(adc_ll_num_t adc_n, adc_bits_width_t bits)
{
    return;
}

/**
 * Enable adc channel to start convert.
 *
 * @note Only one channel can be selected for once measurement.
 *
 * @param adc_n ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_ll_rtc_enable_channel(adc_ll_num_t adc_n, int channel)
{
    if (adc_n == ADC_NUM_1) {
        SENS.sar_meas1_ctrl2.sar1_en_pad = (1 << channel); //only one channel is selected.
    } else { // adc_n == ADC_NUM_2
        SENS.sar_meas2_ctrl2.sar2_en_pad = (1 << channel); //only one channel is selected.
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
static inline void adc_ll_rtc_disable_channel(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
        SENS.sar_meas1_ctrl2.sar1_en_pad = 0; //only one channel is selected.
    } else { // adc_n == ADC_NUM_2
        SENS.sar_meas2_ctrl2.sar2_en_pad = 0; //only one channel is selected.
    }
}

/**
 * Start conversion once by software for RTC controller.
 *
 * @note It may be block to wait conversion idle for ADC1.
 *
 * @param adc_n ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_ll_rtc_start_convert(adc_ll_num_t adc_n, int channel)
{
    if (adc_n == ADC_NUM_1) {
        while (SENS.sar_slave_addr1.meas_status != 0);
        SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
        SENS.sar_meas1_ctrl2.meas1_start_sar = 1;
    } else { // adc_n == ADC_NUM_2
        SENS.sar_meas2_ctrl2.meas2_start_sar = 0; //start force 0
        SENS.sar_meas2_ctrl2.meas2_start_sar = 1; //start force 1
    }
}

/**
 * Check the conversion done flag for each ADCn for RTC controller.
 *
 * @param adc_n ADC unit.
 * @return
 *      -true  : The conversion process is finish.
 *      -false : The conversion process is not finish.
 */
static inline bool adc_ll_rtc_convert_is_done(adc_ll_num_t adc_n)
{
    bool ret = true;
    if (adc_n == ADC_NUM_1) {
        ret = (bool)SENS.sar_meas1_ctrl2.meas1_done_sar;
    } else { // adc_n == ADC_NUM_2
        ret = (bool)SENS.sar_meas2_ctrl2.meas2_done_sar;
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
static inline int adc_ll_rtc_get_convert_value(adc_ll_num_t adc_n)
{
    int ret_val = 0;
    if (adc_n == ADC_NUM_1) {
        ret_val = SENS.sar_meas1_ctrl2.meas1_data_sar;
    } else { // adc_n == ADC_NUM_2
        ret_val = SENS.sar_meas2_ctrl2.meas2_data_sar;
    }
    return ret_val;
}

/**
 * ADC module RTC output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
static inline void adc_ll_rtc_output_invert(adc_ll_num_t adc_n, bool inv_en)
{
    if (adc_n == ADC_NUM_1) {
        SENS.sar_reader1_ctrl.sar1_data_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_NUM_2
        SENS.sar_reader2_ctrl.sar2_data_inv = inv_en;  // Enable / Disable ADC data invert
    }
}

/**
 * Enable ADCn conversion complete interrupt for RTC controller.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_rtc_intr_enable(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
        SENS.sar_reader1_ctrl.sar1_int_en = 1;
        RTCCNTL.int_ena.rtc_saradc1 = 1;
    } else { // adc_n == ADC_NUM_2
        SENS.sar_reader2_ctrl.sar2_int_en = 1;
        RTCCNTL.int_ena.rtc_saradc2 = 1;
    }
}

/**
 * Disable ADCn conversion complete interrupt for RTC controller.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_rtc_intr_disable(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
        SENS.sar_reader1_ctrl.sar1_int_en = 0;
        RTCCNTL.int_ena.rtc_saradc1 = 0;
    } else { // adc_n == ADC_NUM_2
        SENS.sar_reader2_ctrl.sar2_int_en = 0;
        RTCCNTL.int_ena.rtc_saradc2 = 0;
    }
}

/**
 * Reset RTC controller FSM.
 */
static inline void adc_ll_rtc_reset(void)
{
    SENS.sar_meas1_ctrl1.rtc_saradc_reset = 1;
    SENS.sar_meas1_ctrl1.rtc_saradc_reset = 0;
}

/**
 * Sets the number of cycles required for the conversion to complete and wait for the arbiter to stabilize.
 *
 * @note Only ADC2 have arbiter function.
 * @param cycle range: [0,4].
 */
static inline void adc_ll_rtc_set_arbiter_stable_cycle(uint32_t cycle)
{
    SENS.sar_reader2_ctrl.sar2_wait_arb_cycle = cycle;
}

/**
 * Analyze whether the obtained raw data is correct.
 * ADC2 can use arbiter. The arbitration result can be judged by the flag bit in the original data.
 *
 * @param adc_n ADC unit.
 * @param raw_data ADC raw data input (convert value).
 * @return
 *      - 0: The data is correct to use.
 *      - 1: The data is invalid. The current controller is not enabled by the arbiter.
 *      - 2: The data is invalid. The current controller process was interrupted by a higher priority controller.
 *      - -1: The data is error.
 */
static inline adc_ll_rtc_raw_data_t adc_ll_rtc_analysis_raw_data(adc_ll_num_t adc_n, uint16_t raw_data)
{
    /* ADC1 don't need check data */
    if (adc_n == ADC_NUM_1) {
        return ADC_RTC_DATA_OK;
    }
    adc_ll_rtc_output_data_t *temp = (adc_ll_rtc_output_data_t *)&raw_data;
    if (temp->flag == 0) {
        return ADC_RTC_DATA_OK;
    } else if (temp->flag == 1) {
        return ADC_RTC_CTRL_UNSELECTED;
    } else if (temp->flag == 2) {
        return ADC_RTC_CTRL_BREAK;
    } else {
        return ADC_RTC_DATA_FAIL;
    }
}

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/
/**
 * Set ADC module power management.
 *
 * @param manage Set ADC power status.
 */
static inline void adc_ll_set_power_manage(adc_ll_power_t manage)
{
    /* Bit1  0:Fsm  1: SW mode
       Bit0  0:SW mode power down  1: SW mode power on */
    if (manage == ADC_POWER_SW_ON) {
        SENS.sar_meas1_ctrl1.rtc_saradc_clkgate_en = 1;
        SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
    } else if (manage == ADC_POWER_BY_FSM) {
        SENS.sar_meas1_ctrl1.rtc_saradc_clkgate_en = 1;
        SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_FSM;
    } else if (manage == ADC_POWER_SW_OFF) {
        SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_PD;
        SENS.sar_meas1_ctrl1.rtc_saradc_clkgate_en = 0;
    }
}

/**
 * Get ADC module power management.
 *
 * @return
 *      - ADC power status.
 */
static inline adc_ll_power_t adc_ll_get_power_manage(void)
{
    /* Bit1  0:Fsm  1: SW mode
       Bit0  0:SW mode power down  1: SW mode power on */
    adc_ll_power_t manage;
    if (SENS.sar_power_xpd_sar.force_xpd_sar == SENS_FORCE_XPD_SAR_PU) {
        manage = ADC_POWER_SW_ON;
    } else if (SENS.sar_power_xpd_sar.force_xpd_sar == SENS_FORCE_XPD_SAR_PD) {
        manage = ADC_POWER_SW_OFF;
    } else {
        manage = ADC_POWER_BY_FSM;
    }
    return manage;
}

/**
 * ADC SAR clock division factor setting. ADC SAR clock devided from `RTC_FAST_CLK`.
 *
 * @param div Division factor.
 */
static inline void adc_ll_set_sar_clk_div(adc_ll_num_t adc_n, uint32_t div)
{
    if (adc_n == ADC_NUM_1) {
        SENS.sar_reader1_ctrl.sar1_clk_div = div;
    } else { // adc_n == ADC_NUM_2
        SENS.sar_reader2_ctrl.sar2_clk_div = div;
    }
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
 * - 0dB attenuaton (ADC_ATTEN_DB_0) gives full-scale voltage 1.1V
 * - 2.5dB attenuation (ADC_ATTEN_DB_2_5) gives full-scale voltage 1.5V
 * - 6dB attenuation (ADC_ATTEN_DB_6) gives full-scale voltage 2.2V
 * - 11dB attenuation (ADC_ATTEN_DB_11) gives full-scale voltage 3.9V (see note below)
 *
 * @note The full-scale voltage is the voltage corresponding to a maximum reading (depending on ADC1 configured
 * bit width, this value is: 4095 for 12-bits, 2047 for 11-bits, 1023 for 10-bits, 511 for 9 bits.)
 *
 * @note At 11dB attenuation the maximum voltage is limited by VDD_A, not the full scale voltage.
 *
 * Due to ADC characteristics, most accurate results are obtained within the following approximate voltage ranges:
 *
 * - 0dB attenuaton (ADC_ATTEN_DB_0) between 100 and 950mV
 * - 2.5dB attenuation (ADC_ATTEN_DB_2_5) between 100 and 1250mV
 * - 6dB attenuation (ADC_ATTEN_DB_6) between 150 to 1750mV
 * - 11dB attenuation (ADC_ATTEN_DB_11) between 150 to 2450mV
 *
 * For maximum accuracy, use the ADC calibration APIs and measure voltages within these recommended ranges.
 *
 * @param adc_n ADC unit.
 * @param channel ADCn channel number.
 * @param atten The attenuation option.
 */
static inline void adc_ll_set_atten(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten)
{
    if (adc_n == ADC_NUM_1) {
        SENS.sar_atten1 = ( SENS.sar_atten1 & ~(0x3 << (channel * 2)) ) | ((atten & 0x3) << (channel * 2));
    } else { // adc_n == ADC_NUM_2
        SENS.sar_atten2 = ( SENS.sar_atten2 & ~(0x3 << (channel * 2)) ) | ((atten & 0x3) << (channel * 2));
    }
}

/**
 * Get the attenuation of a particular channel on ADCn.
 *
 * @param adc_n ADC unit.
 * @param channel ADCn channel number.
 * @return atten The attenuation option.
 */
static inline adc_atten_t adc_ll_get_atten(adc_ll_num_t adc_n, adc_channel_t channel)
{
    if (adc_n == ADC_NUM_1) {
        return (adc_atten_t)((SENS.sar_atten1 >> (channel * 2)) & 0x3);
    } else {
        return (adc_atten_t)((SENS.sar_atten2 >> (channel * 2)) & 0x3);
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
static inline void adc_ll_set_controller(adc_ll_num_t adc_n, adc_ll_controller_t ctrl)
{
    if (adc_n == ADC_NUM_1) {
        switch ( ctrl ) {
        case ADC_CTRL_RTC:
            SENS.sar_meas1_mux.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            break;
        case ADC_CTRL_ULP:
            SENS.sar_meas1_mux.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_meas1_ctrl2.meas1_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            break;
        case ADC_CTRL_DIG:
            SENS.sar_meas1_mux.sar1_dig_force       = 1;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            break;
        default:
            break;
        }
    } else { // adc_n == ADC_NUM_2
        switch ( ctrl ) {
        case ADC_CTRL_RTC:
            SENS.sar_meas2_ctrl2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas2_ctrl2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            break;
        case ADC_CTRL_ULP:
            SENS.sar_meas2_ctrl2.meas2_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas2_ctrl2.sar2_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            break;
        case ADC_CTRL_DIG:
            SENS.sar_meas2_ctrl2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas2_ctrl2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            break;
        case ADC2_CTRL_PWDET:   // currently only used by Wi-Fi
            SENS.sar_meas2_ctrl2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas2_ctrl2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            break;
        default:
            break;
        }
    }
}

/**
 * Set ADC2 module arbiter work mode.
 * The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 * the low priority controller will read the invalid ADC data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 *
 * @param mode Refer to ``adc_arbiter_mode_t``.
 */
static inline void adc_ll_set_arbiter_work_mode(adc_arbiter_mode_t mode)
{
    SENS.sar_meas2_mux.sar2_rtc_force = 0;  // Enable arbiter in wakeup mode
    if (mode == ADC_ARB_MODE_FIX) {
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_grant_force = 0;
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_fix_priority = 1;
    } else if (mode == ADC_ARB_MODE_LOOP) {
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_grant_force = 0;
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_fix_priority = 0;
    } else {
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_grant_force = 1;    // Shield arbiter.
    }
}

/**
 * Set ADC2 module controller priority in arbiter.
 * The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 * the low priority controller will read the invalid ADC data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 * @note Default priority: Wi-Fi(2) > RTC(1) > Digital(0);
 *
 * @param pri_rtc RTC controller priority. Range: 0 ~ 2.
 * @param pri_dig Digital controller priority. Range: 0 ~ 2.
 * @param pri_pwdet Wi-Fi controller priority. Range: 0 ~ 2.
 */
static inline void adc_ll_set_arbiter_priority(uint8_t pri_rtc, uint8_t pri_dig, uint8_t pri_pwdet)
{
    if (pri_rtc != pri_dig && pri_rtc != pri_pwdet && pri_dig != pri_pwdet) {
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_priority = pri_rtc;
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_priority = pri_dig;
        APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_priority = pri_pwdet;
    }
    /* Should select highest priority controller. */
    if (pri_rtc > pri_dig) {
        if (pri_rtc > pri_pwdet) {
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_force = 1;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_force = 0;
        } else {
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_force = 1;
        }
    } else {
        if (pri_dig > pri_pwdet) {
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_force = 1;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_force = 0;
        } else {
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_apb_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_rtc_force = 0;
            APB_SARADC.apb_adc_arb_ctrl.adc_arb_wifi_force = 1;
        }
    }
}

/**
 * Force switch ADC2 to RTC controller in sleep mode. Shield arbiter.
 * In sleep mode, the arbiter is in power-down mode.
 * Need to switch the controller to RTC to shield the control of the arbiter.
 * After waking up, it needs to switch to arbiter control.
 *
 * @note The hardware will do this automatically. In normal use, there is no need to call this interface to manually switch the controller.
 * @note Only support ADC2.
 */
static inline void adc_ll_enable_sleep_controller(void)
{
    SENS.sar_meas2_mux.sar2_rtc_force = 1;
}

/**
 * Force switch ADC2 to arbiter in wakeup mode.
 * In sleep mode, the arbiter is in power-down mode.
 * Need to switch the controller to RTC to shield the control of the arbiter.
 * After waking up, it needs to switch to arbiter control.
 *
 * @note The hardware will do this automatically. In normal use, there is no need to call this interface to manually switch the controller.
 * @note Only support ADC2.
 */
static inline void adc_ll_disable_sleep_controller(void)
{
    SENS.sar_meas2_mux.sar2_rtc_force = 0;
}

/* ADC calibration code. */
/**
 * @brief Set common calibration configuration. Should be shared with other parts (PWDET).
 */
static inline void adc_ll_calibration_init(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
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
 * @param channel adc channel number.
 * @param internal_gnd true:  Disconnect from the IO port and use the internal GND as the calibration voltage.
 *                     false: Use IO external voltage as calibration voltage.
 */
static inline void adc_ll_calibration_prepare(adc_ll_num_t adc_n, adc_channel_t channel, bool internal_gnd)
{
    /* Should be called before writing I2C registers. */
    CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PD_M);
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PU_M);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_SAR_M);
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_SAR_CFG2_M);

    /* Enable/disable internal connect GND (for calibration). */
    if (adc_n == ADC_NUM_1) {
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
static inline void adc_ll_calibration_finish(adc_ll_num_t adc_n)
{
    if (adc_n == ADC_NUM_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_ENCAL_GND_ADDR, 0);
    } else {
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
static inline void adc_ll_set_calibration_param(adc_ll_num_t adc_n, uint32_t param)
{
    uint8_t msb = param >> 8;
    uint8_t lsb = param & 0xFF;
    /* Should be called before writing I2C registers. */
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PU_M);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_SAR_M);
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_SAR_CFG2_M);

    if (adc_n == ADC_NUM_1) {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_HIGH_ADDR, msb);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR1_INITIAL_CODE_LOW_ADDR, lsb);
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_HIGH_ADDR, msb);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SAR2_INITIAL_CODE_LOW_ADDR, lsb);
    }
}
/* Temp code end. */

/**
 *  Output ADCn inter reference voltage to ADC2 channels.
 *
 *  This function routes the internal reference voltage of ADCn to one of
 *  ADC2's channels. This reference voltage can then be manually measured
 *  for calibration purposes.
 *
 *  @param[in]  adc ADC unit select
 *  @param[in]  channel ADC2 channel number
 *  @param[in]  en Enable/disable the reference voltage output
 */
static inline void adc_ll_vref_output(adc_ll_num_t adc, adc_channel_t channel, bool en)
{
    /* Should be called before writing I2C registers. */
    SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PU_M);
    CLEAR_PERI_REG_MASK(ANA_CONFIG_REG, I2C_SAR_M);
    SET_PERI_REG_MASK(ANA_CONFIG2_REG, ANA_SAR_CFG2_M);

    if (en) {
        if (adc == ADC_NUM_1) {
            /* Config test mux to route v_ref to ADC1 Channels */
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_DTEST_RTC_ADDR, 1);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_TSENS_ADDR, 0);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 1);
        } else {
            /* Config test mux to route v_ref to ADC2 Channels */
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_DTEST_RTC_ADDR, 0);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_TSENS_ADDR, 1);
            REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 0);
        }
        //in sleep force to use rtc to control ADC
        SENS.sar_meas2_mux.sar2_rtc_force = 1;
        //set sar2_en_test
        SENS.sar_meas2_ctrl1.sar2_en_test = 1;
        //set sar2 en force
        SENS.sar_meas2_ctrl2.sar2_en_pad_force = 1;      //Pad bitmap controlled by SW
        //set en_pad for ADC2 channels (bits 0x380)
        SENS.sar_meas2_ctrl2.sar2_en_pad = 1 << channel;
    } else {
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_TSENS_ADDR, 0);
        REGI2C_WRITE_MASK(I2C_SAR_ADC, ADC_SARADC_ENT_RTC_ADDR, 0);
        SENS.sar_meas2_mux.sar2_rtc_force = 0;
        //set sar2_en_test
        SENS.sar_meas2_ctrl1.sar2_en_test = 0;
        //set sar2 en force
        SENS.sar_meas2_ctrl2.sar2_en_pad_force = 0;      //Pad bitmap controlled by SW
        //set en_pad for ADC2 channels (bits 0x380)
        SENS.sar_meas2_ctrl2.sar2_en_pad = 0;
    }
}

#ifdef __cplusplus
}
#endif
