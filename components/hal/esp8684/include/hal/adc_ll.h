/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "regi2c_ctrl.h"
#include "esp_attr.h"

#include "soc/adc_periph.h"
#include "hal/adc_types.h"
#include "soc/apb_saradc_struct.h"
#include "soc/apb_saradc_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_cntl_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_LL_CLKM_DIV_NUM_DEFAULT 15
#define ADC_LL_CLKM_DIV_B_DEFAULT   1
#define ADC_LL_CLKM_DIV_A_DEFAULT   0

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

//These values should be set according to the HW
typedef enum {
    ADC_LL_INTR_THRES1_LOW  = BIT(26),
    ADC_LL_INTR_THRES0_LOW  = BIT(27),
    ADC_LL_INTR_THRES1_HIGH = BIT(28),
    ADC_LL_INTR_THRES0_HIGH = BIT(29),
    ADC_LL_INTR_ADC2_DONE   = BIT(30),
    ADC_LL_INTR_ADC1_DONE   = BIT(31),
} adc_ll_intr_t;
FLAG_ATTR(adc_ll_intr_t)

/**
 * @brief ADC controller type selection.
 *
 * @note For ADC2, use the force option with care. The system power consumption detection will use ADC2.
 *       If it is forced to switch to another controller, it may cause the system to obtain incorrect values.
 * @note Normally, there is no need to switch the controller manually.
 */
typedef enum {
    ADC_CTRL_RTC = 0,   /*!<For ADC1. Select RTC controller. For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC_CTRL_DIG = 2,   /*!<For ADC1. Select DIG controller. For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC2_CTRL_PWDET = 3,/*!<For ADC2. The controller is selected by the arbiter. Arbiter in default mode. */
    ADC2_CTRL_FORCE_PWDET = 3,  /*!<For ADC2. Arbiter in shield mode. Force select Wi-Fi controller work. */
    ADC2_CTRL_FORCE_RTC = 4,    /*!<For ADC2. Arbiter in shield mode. Force select RTC controller work. */
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
    abort();// TODO: IDF-3901.
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
    abort();// TODO: IDF-3901.
}

/**
 * Set SAR ADC module clock division factor.
 * SAR ADC clock divided from digital controller clock.
 *
 * @param div Division factor.
 */
static inline void adc_ll_digi_set_clk_div(uint32_t div)
{
    abort();// TODO: IDF-3901.
}

/**
 * Set adc max conversion number for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param meas_num Max conversion number. Range: 0 ~ 255.
 */
static inline void adc_ll_digi_set_convert_limit_num(uint32_t meas_num)
{
    abort();// TODO: IDF-3901.
}

/**
 * Enable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 */
static inline void adc_ll_digi_convert_limit_enable(void)
{
    abort();// TODO: IDF-3901.
}

/**
 * Disable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 */
static inline void adc_ll_digi_convert_limit_disable(void)
{
    abort();// TODO: IDF-3901.
}

/**
 * Set pattern table length for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 8 items, in which channel selection,
 * and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 8 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param patt_len Items range: 1 ~ 8.
 */
static inline void adc_ll_digi_set_pattern_table_len(adc_ll_num_t adc_n, uint32_t patt_len)
{
    abort();// TODO: IDF-3901.
}

/**
 * Set pattern table for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 8 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 8 different rules before repeating itself.
 *
 * @param adc_n ADC unit.
 * @param pattern_index Items index. Range: 0 ~ 7.
 * @param pattern Stored conversion rules.
 */
static inline void adc_ll_digi_set_pattern_table(adc_ll_num_t adc_n, uint32_t pattern_index, adc_digi_pattern_table_t pattern)
{
    abort();// TODO: IDF-3901.
}

/**
 * Reset the pattern table pointer, then take the measurement rule from table header in next measurement.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_clear_pattern_table(adc_ll_num_t adc_n)
{
    abort();// TODO: IDF-3901.
}

/**
 * Sets the number of cycles required for the conversion to complete and wait for the arbiter to stabilize.
 *
 * @note Only ADC2 have arbiter function.
 * @param cycle range: 0 ~ 4.
 */
static inline void adc_ll_digi_set_arbiter_stable_cycle(uint32_t cycle)
{
    abort();// TODO: IDF-3901.
}

/**
 * ADC Digital controller output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
static inline void adc_ll_digi_output_invert(adc_ll_num_t adc_n, bool inv_en)
{
    abort();// TODO: IDF-3901.
}

/**
 * Set the interval clock cycle for the digital controller to trigger the measurement.
 * Expression: `trigger_meas_freq` = `controller_clk` / 2 / interval.
 *
 * @note The trigger interval should not be smaller than the sampling time of the SAR ADC.
 * @param cycle The clock cycle (trigger interval) of the measurement. Range: 30 ~ 4095.
 */
static inline void adc_ll_digi_set_trigger_interval(uint32_t cycle)
{
    abort();// TODO: IDF-3901.
}

/**
 * Enable digital controller timer to trigger the measurement.
 */
static inline void adc_ll_digi_trigger_enable(void)
{
    abort();// TODO: IDF-3901.
}

/**
 * Disable digital controller timer to trigger the measurement.
 */
static inline void adc_ll_digi_trigger_disable(void)
{
    abort();// TODO: IDF-3901.
}

/**
 * Set ADC digital controller clock division factor. The clock divided from `APLL` or `APB` clock.
 * Expression: controller_clk = APLL/APB * (div_num  + div_b / div_a).
 *
 * @param div_num Division factor. Range: 1 ~ 255.
 * @param div_b Division factor. Range: 1 ~ 63.
 * @param div_a Division factor. Range: 0 ~ 63.
 */
static inline void adc_ll_digi_controller_clk_div(uint32_t div_num, uint32_t div_b, uint32_t div_a)
{
    abort();// TODO: IDF-3901.
}

/**
 * Enable clock and select clock source for ADC digital controller.
 *
 * @param use_apll true: use APLL clock; false: use APB clock.
 */
static inline void adc_ll_digi_controller_clk_enable(bool use_apll)
{
    abort();// TODO: IDF-3901.
}

/**
 * Disable clock for ADC digital controller.
 */
static inline void adc_ll_digi_controller_clk_disable(void)
{
    abort();// TODO: IDF-3901.
}

/**
 * Reset adc digital controller filter.
 *
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_filter_reset(adc_ll_num_t adc_n)
{
    abort();// TODO: IDF-3901.
}

/**
 * Set adc digital controller filter factor.
 *
 * @note If the channel info is not supported, the filter function will not be enabled.
 * @param idx ADC filter unit.
 * @param filter Filter config. Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
static inline void adc_ll_digi_filter_set_factor(adc_digi_filter_idx_t idx, adc_digi_filter_t *filter)
{
    abort();// TODO: IDF-3901.
}

/**
 * Get adc digital controller filter factor.
 *
 * @param adc_n ADC unit.
 * @param factor Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
static inline void adc_ll_digi_filter_get_factor(adc_digi_filter_idx_t idx, adc_digi_filter_t *filter)
{
    abort();// TODO: IDF-3901.
}

/**
 * Disable adc digital controller filter.
 * Filtering the ADC data to obtain smooth data at higher sampling rates.
 *
 * @note If the channel info is not supported, the filter function will not be enabled.
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_filter_disable(adc_digi_filter_idx_t idx)
{
    abort();// TODO: IDF-3901.
}

/**
 * Set monitor mode of adc digital controller.
 *
 * @note If the channel info is not supported, the monitor function will not be enabled.
 * @param adc_n ADC unit.
 * @param is_larger true:  If ADC_OUT >  threshold, Generates monitor interrupt.
 *                  false: If ADC_OUT <  threshold, Generates monitor interrupt.
 */
static inline void adc_ll_digi_monitor_set_mode(adc_digi_monitor_idx_t idx, adc_digi_monitor_t *cfg)
{
    abort();// TODO: IDF-3901.
}

/**
 * Enable/disable monitor of adc digital controller.
 *
 * @note If the channel info is not supported, the monitor function will not be enabled.
 * @param adc_n ADC unit.
 */
static inline void adc_ll_digi_monitor_disable(adc_digi_monitor_idx_t idx)
{
    abort();// TODO: IDF-3901.
}

/**
 * Set DMA eof num of adc digital controller.
 * If the number of measurements reaches `dma_eof_num`, then `dma_in_suc_eof` signal is generated.
 *
 * @param num eof num of DMA.
 */
static inline void adc_ll_digi_dma_set_eof_num(uint32_t num)
{
    abort();// TODO: IDF-3901.
}

/**
 * Enable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_enable(void)
{
    abort();// TODO: IDF-3901.
}

/**
 * Disable output data to DMA from adc digital controller.
 */
static inline void adc_ll_digi_dma_disable(void)
{
    abort();// TODO: IDF-3901.
}

/**
 * Reset adc digital controller.
 */
static inline void adc_ll_digi_reset(void)
{
    abort();// TODO: IDF-3901.
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
    abort();// TODO: IDF-3901.
}

/**
 * Get adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @return cct Range: 0 ~ 7.
 */
static inline uint32_t adc_ll_pwdet_get_cct(void)
{
    // TODO: IDF-3901.
    abort();
}

/**
 * Analyze whether the obtained raw data is correct.
 * ADC2 can use arbiter. The arbitration result is stored in the channel information of the returned data.
 *
 * @param adc_n ADC unit.
 * @param raw_data ADC raw data input (convert value).
 * @return
 *        -  0: The data is correct to use.
 *        - -1: The data is invalid.
 */
static inline adc_ll_rtc_raw_data_t adc_ll_analysis_raw_data(adc_ll_num_t adc_n, int raw_data)
{
    // TODO: IDF-3901.
    abort();
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
    abort();// TODO: IDF-3901.
}

/**
 * Get ADC module power management.
 *
 * @return
 *      - ADC power status.
 */
static inline adc_ll_power_t adc_ll_get_power_manage(void)
{
    abort();// TODO: IDF-3901.
}

/**
 * Set ADC2 module arbiter work mode.
 * The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 * the low priority controller will read the invalid ADC data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 *
 * @param mode Refer to `adc_arbiter_mode_t`.
 */
static inline void adc_ll_set_arbiter_work_mode(adc_arbiter_mode_t mode)
{
    abort();// TODO: IDF-3901.
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
    abort();// TODO: IDF-3901.
}

/* ADC calibration code. */
/**
 * @brief Set common calibration configuration. Should be shared with other parts (PWDET).
 */
static inline void adc_ll_calibration_init(adc_ll_num_t adc_n)
{
    abort();// TODO: IDF-3901.
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
    abort();// TODO: IDF-3901.
}

/**
 * Resume register status after calibration.
 *
 * @param adc_n ADC index number.
 */
static inline void adc_ll_calibration_finish(adc_ll_num_t adc_n)
{
    abort();// TODO: IDF-3901.
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
    abort();// TODO: IDF-3901.
}
/* Temp code end. */

/**
 *  Output ADCn inter reference voltage to ADC2 channels.
 *
 *  This function routes the internal reference voltage of ADCn to one of
 *  ADC1's channels. This reference voltage can then be manually measured
 *  for calibration purposes.
 *
 *  @param[in]  adc ADC unit select
 *  @param[in]  channel ADC1 channel number
 *  @param[in]  en Enable/disable the reference voltage output
 */
static inline void adc_ll_vref_output(adc_ll_num_t adc, adc_channel_t channel, bool en)
{
    abort();// TODO: IDF-3901.
}

/*---------------------------------------------------------------
                    Single Read
---------------------------------------------------------------*/
/**
 * Trigger single read
 *
 * @param val Usage: set to 1 to start the ADC conversion. The step signal should at least keep 3 ADC digital controller clock cycle,
 *            otherwise the step signal may not be captured by the ADC digital controller when its frequency is slow.
 *            This hardware limitation will be removed in future versions.
 */
static inline void adc_ll_onetime_start(bool val)
{
    abort();// TODO: IDF-3901.
}

static inline void adc_ll_onetime_set_channel(adc_ll_num_t unit, adc_channel_t channel)
{
    abort();// TODO: IDF-3901.
}

static inline void adc_ll_onetime_set_atten(adc_atten_t atten)
{
    abort();// TODO: IDF-3901.
}

static inline void adc_ll_intr_enable(adc_ll_intr_t mask)
{
    abort();// TODO: IDF-3901.
}

static inline void adc_ll_intr_disable(adc_ll_intr_t mask)
{
    abort();// TODO: IDF-3901.
}

static inline void adc_ll_intr_clear(adc_ll_intr_t mask)
{
    abort();// TODO: IDF-3901.
}

static inline bool adc_ll_intr_get_raw(adc_ll_intr_t mask)
{
    abort();// TODO: IDF-3901.
}

static inline bool adc_ll_intr_get_status(adc_ll_intr_t mask)
{
    abort();// TODO: IDF-3901.
}

static inline void adc_ll_onetime_sample_enable(adc_ll_num_t adc_n, bool enable)
{
    abort();// TODO: IDF-3901.
}

static inline uint32_t adc_ll_adc1_read(void)
{
    abort();// TODO: IDF-3901.
}

static inline uint32_t adc_ll_adc2_read(void)
{
    abort();// TODO: IDF-3901.
}

#ifdef __cplusplus
}
#endif
