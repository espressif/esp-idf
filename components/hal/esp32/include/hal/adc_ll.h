/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/adc_types.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/adc_periph.h"
#include "soc/rtc_io_struct.h"
#include "soc/sens_struct.h"
#include "soc/sens_reg.h"
#include "soc/syscon_struct.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_LL_EVENT_ADC1_ONESHOT_DONE    (1 << 0)
#define ADC_LL_EVENT_ADC2_ONESHOT_DONE    (1 << 1)

/*---------------------------------------------------------------
                    Oneshot
---------------------------------------------------------------*/
#define ADC_LL_DATA_INVERT_DEFAULT(PERIPH_NUM)         (1)
#define ADC_LL_SAR_CLK_DIV_DEFAULT(PERIPH_NUM)         (1)
#define ADC_LL_DELAY_CYCLE_AFTER_DONE_SIGNAL           (0)

/*---------------------------------------------------------------
                    DMA
---------------------------------------------------------------*/
#define ADC_LL_DIGI_DATA_INVERT_DEFAULT(PERIPH_NUM)    (1)
#define ADC_LL_FSM_RSTB_WAIT_DEFAULT                   (8)
#define ADC_LL_FSM_START_WAIT_DEFAULT                  (ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT)
#define ADC_LL_FSM_STANDBY_WAIT_DEFAULT                (100)
#define ADC_LL_SAMPLE_CYCLE_DEFAULT                    (2)
#define ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT                (16)

//On esp32, ADC can only be continuously triggered when `ADC_LL_DEFAULT_CONV_LIMIT_EN == 1`, `ADC_LL_DEFAULT_CONV_LIMIT_NUM != 0`
#define ADC_LL_DEFAULT_CONV_LIMIT_EN      1
#define ADC_LL_DEFAULT_CONV_LIMIT_NUM     10

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

//Need a unit test for bit_width
typedef struct {
    union {
        struct {
            uint8_t atten:       2;
            uint8_t bit_width:   2;  //ADC resolution. 0: 9 bit; 1: 10 bit; 2: 11 bit; 3: 12 bit
            uint8_t channel:     4;
        };
        uint8_t val;
    };
} __attribute__((packed)) adc_ll_digi_pattern_table_t;

typedef enum {
    ADC_HALL_CTRL_ULP = 0x0,/*!< Hall sensor controlled by ULP */
    ADC_HALL_CTRL_RTC = 0x1 /*!< Hall sensor controlled by RTC */
} adc_ll_hall_controller_t ;

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
    HAL_FORCE_MODIFY_U32_REG_FIELD(SYSCON.saradc_fsm, rstb_wait, rst_wait);
    // Internal FSM start wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(SYSCON.saradc_fsm, start_wait, start_wait);
    // Internal FSM standby wait time
    HAL_FORCE_MODIFY_U32_REG_FIELD(SYSCON.saradc_fsm, standby_wait, standby_wait);
}

/**
 * Set adc sample cycle.
 *
 * @note Normally, please use default value.
 * @param sample_cycle The number of ADC sampling cycles. Range: 1 ~ 7.
 */
static inline void adc_ll_set_sample_cycle(uint32_t sample_cycle)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(SYSCON.saradc_fsm, sample_cycle, sample_cycle);
}

/**
 * ADC module clock division factor setting. ADC clock divided from APB clock.
 *
 * @param div Division factor.
 */
static inline void adc_ll_digi_set_clk_div(uint32_t div)
{
    /* ADC clock divided from APB clk, e.g. 80 / 2 = 40Mhz, */
    HAL_FORCE_MODIFY_U32_REG_FIELD(SYSCON.saradc_ctrl, sar_clk_div, div);
}

/**
 * Set adc max conversion number for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param meas_num Max conversion number. Range: 0 ~ 255.
 */
static inline void adc_ll_digi_set_convert_limit_num(uint32_t meas_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(SYSCON.saradc_ctrl2, max_meas_num, meas_num);
}

/**
 * Enable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 * @note On esp32, this should always be 1 to trigger the ADC continuously
 *
 * @param enable  true: enable; false: disable
 */
static inline void adc_ll_digi_convert_limit_enable(bool enable)
{
    SYSCON.saradc_ctrl2.meas_num_limit = enable;
}

/**
 * Set adc conversion mode for digital controller.
 *
 * @note ESP32 only support ADC1 single mode.
 * @note For `data_sar_sel` register:
 *       1: [15] unit, [14:11] channel, [10:0] data, 11-bit-width at most. Only work under `ADC_LL_DIGI_CONV_BOTH_UNIT` or `ADC_LL_DIGI_CONV_ALTER_UNIT` mode.
 *       0: [15:12] channel, [11:0] data, 12-bit-width at most. Only work under `ADC_LL_DIGI_CONV_ONLY_ADC1` or `ADC_LL_DIGI_CONV_ONLY_ADC2` mode
 *
 * @param mode Conversion mode select.
 */
static inline void adc_ll_digi_set_convert_mode(adc_ll_digi_convert_mode_t mode)
{
    if (mode == ADC_LL_DIGI_CONV_ONLY_ADC1) {
        SYSCON.saradc_ctrl.work_mode = 0;
        SYSCON.saradc_ctrl.sar_sel = 0;
        SYSCON.saradc_ctrl.data_sar_sel = 0;
    } else if (mode == ADC_LL_DIGI_CONV_ONLY_ADC2) {
        SYSCON.saradc_ctrl.work_mode = 0;
        SYSCON.saradc_ctrl.sar_sel = 1;
        SYSCON.saradc_ctrl.data_sar_sel = 0;
    } else if (mode == ADC_LL_DIGI_CONV_BOTH_UNIT) {
        SYSCON.saradc_ctrl.work_mode = 1;
        SYSCON.saradc_ctrl.data_sar_sel = 1;
    } else if (mode == ADC_LL_DIGI_CONV_ALTER_UNIT) {
        SYSCON.saradc_ctrl.work_mode = 2;
        SYSCON.saradc_ctrl.data_sar_sel = 1;
    }
}

/**
 * ADC module Digital output data invert or not.
 *
 * @prarm adc_n ADC unit.
 */
static inline void adc_ll_digi_output_invert(adc_unit_t adc_n, bool inv_en)
{
    if (adc_n == ADC_UNIT_1) {
        SYSCON.saradc_ctrl2.sar1_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_UNIT_2
        SYSCON.saradc_ctrl2.sar2_inv = inv_en;  // Enable / Disable ADC data invert
    }
}

/**
 * Set I2S DMA data source for digital controller.
 *
 * @param src 1: I2S input data is from SAR ADC (for DMA)  0: I2S input data is from GPIO matrix
 */
static inline void adc_ll_digi_set_data_source(bool src)
{
    SYSCON.saradc_ctrl.data_to_i2s = src;
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
static inline void adc_ll_digi_set_pattern_table_len(adc_unit_t adc_n, uint32_t patt_len)
{
    if (adc_n == ADC_UNIT_1) {
        SYSCON.saradc_ctrl.sar1_patt_len = patt_len - 1;
    } else { // adc_n == ADC_UNIT_2
        SYSCON.saradc_ctrl.sar2_patt_len = patt_len - 1;
    }
}

/**
 * Set pattern table lenth for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param pattern_index Items index. Range: 0 ~ 15.
 * @param pattern Stored conversion rules, see ``adc_digi_pattern_table_t``.
 */
static inline void adc_ll_digi_set_pattern_table(adc_unit_t adc_n, uint32_t pattern_index, adc_digi_pattern_config_t table)
{
    uint32_t tab;
    uint8_t index = pattern_index / 4;
    uint8_t offset = (pattern_index % 4) * 8;
    adc_ll_digi_pattern_table_t pattern = {0};
    uint8_t bit_width;

    switch (table.bit_width) {
        case 9:
            bit_width = 0x0;
            break;
        case 10:
            bit_width = 0x1;
            break;
        case 11:
            bit_width = 0x2;
            break;
        case 12:
            bit_width = 0x3;
            break;
        default:
            bit_width = 0x3;
    }
    pattern.val = (table.atten & 0x3) | ((bit_width) << 2) | ((table.channel & 0xF) << 4);

    if (table.unit == ADC_UNIT_1) {
        tab = SYSCON.saradc_sar1_patt_tab[index];   // Read old register value
        tab &= (~(0xFF000000 >> offset));           // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        SYSCON.saradc_sar1_patt_tab[index] = tab;   // Write back
    } else { // adc_n == ADC_UNIT_2
        tab = SYSCON.saradc_sar2_patt_tab[index];   // Read old register value
        tab &= (~(0xFF000000 >> offset));           // clear old data
        tab |= ((uint32_t)pattern.val << 24) >> offset; // Fill in the new data
        SYSCON.saradc_sar2_patt_tab[index] = tab;   // Write back
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
        SYSCON.saradc_ctrl.sar1_patt_p_clear = 1;
        SYSCON.saradc_ctrl.sar1_patt_p_clear = 0;
    } else { // adc_n == ADC_UNIT_2
        SYSCON.saradc_ctrl.sar2_patt_p_clear = 1;
        SYSCON.saradc_ctrl.sar2_patt_p_clear = 0;
    }
}

/**
 * Disable clock for ADC digital controller.
 * @note Not used for esp32
 */
static inline void adc_ll_digi_controller_clk_disable(void)
{
    //Leave here for compatibility
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
    SENS.sar_start_force.sar2_pwdet_cct = cct;
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
    return SENS.sar_start_force.sar2_pwdet_cct;
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
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_read_ctrl, sar1_clk_div, div);
    } else { // adc_n == ADC_UNIT_2
        HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_read_ctrl2, sar2_clk_div, div);
    }
}

/**
 * Set adc output data format for RTC controller.
 *
 * @param adc_n ADC unit.
 * @param bits Output data bits width option
 */
static inline void adc_oneshot_ll_set_output_bits(adc_unit_t adc_n, adc_bitwidth_t bits)
{
    uint32_t reg_val = 0;
    switch (bits) {
        case ADC_BITWIDTH_9:
            reg_val = 0;
            break;
        case ADC_BITWIDTH_10:
            reg_val = 1;
            break;
        case ADC_BITWIDTH_11:
            reg_val = 2;
            break;
        case ADC_BITWIDTH_12:
            reg_val = 3;
            break;
        case ADC_BITWIDTH_DEFAULT:
            reg_val = 3;
            break;
        default:
            HAL_ASSERT(false);
    }
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_start_force.sar1_bit_width = reg_val;
        SENS.sar_read_ctrl.sar1_sample_bit = reg_val;
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_start_force.sar2_bit_width = reg_val;
        SENS.sar_read_ctrl2.sar2_sample_bit = reg_val;
    }
}

/**
 * Enable adc channel to start convert.
 *
 * @note Only one channel can be selected in once measurement.
 *
 * @param adc_n ADC unit.
 * @param channel ADC channel number for each ADCn.
 */
static inline void adc_oneshot_ll_set_channel(adc_unit_t adc_n, int channel)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_meas_start1.sar1_en_pad = (1 << channel); //only one channel is selected.
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_meas_start2.sar2_en_pad = (1 << channel); //only one channel is selected.
    }
}

/**
 * Disable adc channel to start convert.
 *
 * @note Only one channel can be selected in once measurement.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_oneshot_ll_disable_channel(adc_unit_t adc_n)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_meas_start1.sar1_en_pad = 0; //only one channel is selected.
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_meas_start2.sar2_en_pad = 0; //only one channel is selected.
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
        while (HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_slave_addr1, meas_status) != 0) {}
        SENS.sar_meas_start1.meas1_start_sar = 0;
        SENS.sar_meas_start1.meas1_start_sar = 1;
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_meas_start2.meas2_start_sar = 0; //start force 0
        SENS.sar_meas_start2.meas2_start_sar = 1; //start force 1
    }
}

/**
 * Clear the event for each ADCn for Oneshot mode
 *
 * @param event ADC event
 */
static inline void adc_oneshot_ll_clear_event(uint32_t event)
{
    //For compatibility
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
        ret = (bool)SENS.sar_meas_start1.meas1_done_sar;
    } else if (event == ADC_LL_EVENT_ADC2_ONESHOT_DONE) {
        ret = (bool)SENS.sar_meas_start2.meas2_done_sar;
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
        ret_val = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas_start1, meas1_data_sar);
    } else { // adc_n == ADC_UNIT_2
        ret_val = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas_start2, meas2_data_sar);
    }
    return ret_val;
}

/**
 * ADC module RTC output data invert or not.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_oneshot_ll_output_invert(adc_unit_t adc_n, bool inv_en)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_read_ctrl.sar1_data_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_UNIT_2
        SENS.sar_read_ctrl2.sar2_data_inv = inv_en;  // Enable / Disable ADC data invert
    }
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
 * Set the attenuation of a particular channel on ADCn.
 */
static inline void adc_oneshot_ll_set_atten(adc_unit_t adc_n, adc_channel_t channel, adc_atten_t atten)
{
    if (adc_n == ADC_UNIT_1) {
        SENS.sar_atten1 = ( SENS.sar_atten1 & ~(0x3 << (channel * 2)) ) | ((atten & 0x3) << (channel * 2));
    } else { // adc_n == ADC_UNIT_2
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
__attribute__((always_inline))
static inline adc_atten_t adc_ll_get_atten(adc_unit_t adc_n, adc_channel_t channel)
{
    if (adc_n == ADC_UNIT_1) {
        return (adc_atten_t)((SENS.sar_atten1 >> (channel * 2)) & 0x3);
    } else {
        return (adc_atten_t)((SENS.sar_atten2 >> (channel * 2)) & 0x3);
    }
}

/**
 * Enable oneshot conversion trigger
 *
 * @param adc_n  Not used, for compatibility
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
                    Common setting
---------------------------------------------------------------*/
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
        switch ( ctrl ) {
        case ADC_LL_CTRL_RTC:
            SENS.sar_read_ctrl.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_meas_start1.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas_start1.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_touch_ctrl1.xpd_hall_force     = 1;    // 1: SW control HALL power;        0: ULP FSM control HALL power.
            SENS.sar_touch_ctrl1.hall_phase_force   = 1;    // 1: SW control HALL phase;        0: ULP FSM control HALL phase.
            break;
        case ADC_LL_CTRL_ULP:
            SENS.sar_read_ctrl.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_meas_start1.meas1_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas_start1.sar1_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_touch_ctrl1.xpd_hall_force     = 0;    // 1: SW control HALL power;        0: ULP FSM control HALL power.
            SENS.sar_touch_ctrl1.hall_phase_force   = 0;    // 1: SW control HALL phase;        0: ULP FSM control HALL phase.
            break;
        case ADC_LL_CTRL_DIG:
            SENS.sar_read_ctrl.sar1_dig_force       = 1;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_meas_start1.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas_start1.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_touch_ctrl1.xpd_hall_force     = 1;    // 1: SW control HALL power;        0: ULP FSM control HALL power.
            SENS.sar_touch_ctrl1.hall_phase_force   = 1;    // 1: SW control HALL phase;        0: ULP FSM control HALL phase.
            break;
        default:
            break;
        }
    } else { // adc_n == ADC_UNIT_2
        switch ( ctrl ) {
        case ADC_LL_CTRL_RTC:
            SENS.sar_meas_start2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas_start2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_read_ctrl2.sar2_dig_force      = 0;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_read_ctrl2.sar2_pwdet_force    = 0;    // 1: Select power detect control;  0: Select RTC control.
            SYSCON.saradc_ctrl.sar2_mux             = 1;    // 1: Select digital control;       0: Select power detect control.
            break;
        case ADC_LL_CTRL_ULP:
            SENS.sar_meas_start2.meas2_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas_start2.sar2_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_read_ctrl2.sar2_dig_force      = 0;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_read_ctrl2.sar2_pwdet_force    = 0;    // 1: Select power detect control;  0: Select RTC control.
            SYSCON.saradc_ctrl.sar2_mux             = 1;    // 1: Select digital control;       0: Select power detect control.
            break;
        case ADC_LL_CTRL_DIG:
            SENS.sar_meas_start2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas_start2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_read_ctrl2.sar2_dig_force      = 1;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_read_ctrl2.sar2_pwdet_force    = 0;    // 1: Select power detect control;  0: Select RTC control.
            SYSCON.saradc_ctrl.sar2_mux             = 1;    // 1: Select digital control;       0: Select power detect control.
            break;
        case ADC_LL_CTRL_PWDET:   // currently only used by Wi-Fi
            SENS.sar_meas_start2.meas2_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas_start2.sar2_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_read_ctrl2.sar2_dig_force      = 0;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_read_ctrl2.sar2_pwdet_force    = 1;    // 1: Select power detect control;  0: Select RTC control.
            SYSCON.saradc_ctrl.sar2_mux             = 0;    // 1: Select digital control;       0: Select power detect control.
            break;
        default:
            break;
        }
    }
}

/**
 * Close ADC AMP module if don't use it for power save.
 */
static inline void adc_ll_amp_disable(void)
{
    //channel is set in the  convert function
    SENS.sar_meas_wait2.force_xpd_amp = SENS_FORCE_XPD_AMP_PD;
    //disable FSM, it's only used by the LNA.
    SENS.sar_meas_ctrl.amp_rst_fb_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_gnd_fsm = 0;
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_meas_wait1, sar_amp_wait1, 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_meas_wait1, sar_amp_wait2, 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_meas_wait2, sar_amp_wait3, 1);
}

/*---------------------------------------------------------------
                    Hall sensor setting
---------------------------------------------------------------*/

/**
 * Enable hall sensor.
 */
static inline void adc_ll_hall_enable(void)
{
    RTCIO.hall_sens.xpd_hall = 1;
}

/**
 * Disable hall sensor.
 */
static inline void adc_ll_hall_disable(void)
{
    RTCIO.hall_sens.xpd_hall = 0;
}

/**
 * Reverse phase of hall sensor.
 */
static inline void adc_ll_hall_phase_enable(void)
{
    RTCIO.hall_sens.hall_phase = 1;
}

/**
 * Don't reverse phase of hall sensor.
 */
static inline void adc_ll_hall_phase_disable(void)
{
    RTCIO.hall_sens.hall_phase = 0;
}

/**
 * Set hall sensor controller.
 *
 * @param hall_ctrl Hall controller.
 */
static inline void adc_ll_set_hall_controller(adc_ll_hall_controller_t hall_ctrl)
{
    SENS.sar_touch_ctrl1.xpd_hall_force = hall_ctrl;    // 1: SW control HALL power;    0: ULP FSM control HALL power.
    SENS.sar_touch_ctrl1.hall_phase_force = hall_ctrl;  // 1: SW control HALL phase;    0: ULP FSM control HALL phase.
}

/**
 *  Output ADC internal reference voltage to channels, only available for ADC2 on ESP32.
 *
 *  This function routes the internal reference voltage of ADCn to one of
 *  ADC2's channels. This reference voltage can then be manually measured
 *  for calibration purposes.
 *
 *  @param[in]  adc ADC unit select
 *  @param[in]  channel ADC2 channel number
 *  @param[in]  en Enable/disable the reference voltage output
 */
static inline void adc_ll_vref_output(adc_unit_t adc, adc_channel_t channel, bool en)
{
    if (adc != ADC_UNIT_2) {
        return;
    }

    if (en) {
        RTCCNTL.bias_conf.dbg_atten = 0;     //Check DBG effect outside sleep mode
        //set dtest (MUX_SEL : 0 -> RTC; 1-> vdd_sar2)
        RTCCNTL.test_mux.dtest_rtc = 1;      //Config test mux to route v_ref to ADC2 Channels
        //set ent
        RTCCNTL.test_mux.ent_rtc = 1;
        //set sar2_en_test
        SENS.sar_start_force.sar2_en_test = 1;
        //set sar2 en force
        SENS.sar_meas_start2.sar2_en_pad_force = 1;      //Pad bitmap controlled by SW
        //set en_pad for channels 7,8,9 (bits 0x380)
        SENS.sar_meas_start2.sar2_en_pad = 1 << channel;
    } else {
        RTCCNTL.test_mux.dtest_rtc = 0;      //Config test mux to route v_ref to ADC2 Channels
        //set ent
        RTCCNTL.test_mux.ent_rtc = 0;
        //set sar2_en_test
        SENS.sar_start_force.sar2_en_test = 0;
        //set sar2 en force
        SENS.sar_meas_start2.sar2_en_pad_force = 0;      //Pad bitmap controlled by SW
        //set en_pad for channels 7,8,9 (bits 0x380)
        SENS.sar_meas_start2.sar2_en_pad = 0;
    }
}

#ifdef __cplusplus
}
#endif
