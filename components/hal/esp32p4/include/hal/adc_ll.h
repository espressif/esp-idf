/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "esp_attr.h"

#include "soc/adc_periph.h"
// #include "soc/ADC_struct.h"
// #include "soc/ADC_reg.h"
#include "soc/pmu_reg.h"
#include "soc/clk_tree_defs.h"
// #include "soc/pcr_struct.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/adc_types.h"
#include "hal/adc_types_private.h"
#include "hal/regi2c_ctrl.h"
#include "hal/sar_ctrl_ll.h"

#include "soc/regi2c_saradc.h"

#include "soc/hp_sys_clkrst_struct.h"
#include "soc/adc_struct.h"
#include "soc/lp_adc_struct.h"
#include "soc/lpperi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_LL_EVENT_ADC1_ONESHOT_DONE    BIT(31)
#define ADC_LL_EVENT_ADC2_ONESHOT_DONE    BIT(30)

/*---------------------------------------------------------------
                    Oneshot
---------------------------------------------------------------*/
#define ADC_LL_DATA_INVERT_DEFAULT(PERIPH_NUM)         (0)
#define ADC_LL_SAR_CLK_DIV_DEFAULT(PERIPH_NUM)         (1)
#define ADC_LL_DELAY_CYCLE_AFTER_DONE_SIGNAL           (0)

/*---------------------------------------------------------------
                    DMA
---------------------------------------------------------------*/

/*---------------------------------------------------------------
                    PWDET (Power Detect)
---------------------------------------------------------------*/
#define ADC_LL_PWDET_CCT_DEFAULT                       (4)

typedef enum {
    ADC_LL_CTRL_RTC   = 0,    ///< For ADC1 and ADC2. Select RTC controller.
    ADC_LL_CTRL_ULP   = 1,    ///< For ADC1 and ADC2. Select ULP controller.
    ADC_LL_CTRL_DIG   = 2,    ///< For ADC1 and ADC2. Select DIG controller.
    ADC_LL_CTRL_PWDET = 3,    ///< ???
    ADC_LL_CTRL_ARB   = 4,    ///< ???
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

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

/**
 * Set SAR ADC module clock division factor.
 * SAR ADC clock divided from digital controller clock.
 *
 * @param div Division factor.
 */
static inline void adc_ll_digi_set_clk_div(uint32_t div)
{
    /* ADC clock divided from digital controller clock clk */
    HAL_FORCE_MODIFY_U32_REG_FIELD(ADC.ctrl_reg, sar_clk_div, div);
}

/**
 * Set ADC digital controller clock division factor. The clock divided from `APLL` or `APB` clock.
 * Expression: controller_clk = (APLL or APB) / (div_num + div_a / div_b + 1).
 *
 * @param div_num Division factor. Range: 0 ~ 255.
 * @param div_b Division factor. Range: 1 ~ 63.
 * @param div_a Division factor. Range: 0 ~ 63.
 */
static inline void adc_ll_digi_controller_clk_div(uint32_t div_num, uint32_t div_b, uint32_t div_a)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl23, reg_adc_clk_div_num, div_num);
    HP_SYS_CLKRST.root_clk_ctrl0.reg_cpu_clk_div_numerator = div_b;
    HP_SYS_CLKRST.root_clk_ctrl0.reg_cpu_clk_div_denominator = div_a;
}

/**
 * Enable clock and select clock source for ADC digital controller.
 *
 * @param clk_src clock source for ADC digital controller.
 */
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

/**
 * @brief Enable the ADC clock
 * @param enable true to enable, false to disable
 */
static inline void adc_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.soc_clk_ctrl2.reg_adc_apb_clk_en = enable;
}
// HP_SYS_CLKRST.soc_clk_ctrl2 are shared registers, so this function must be used in an atomic way
#define adc_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; adc_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset ADC module
 */
static inline void adc_ll_reset_register(void)
{
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_adc = 1;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_adc = 0;
}
//  HP_SYS_CLKRST.hp_rst_en2 is a shared register, so this function must be used in an atomic way
#define adc_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; adc_ll_reset_register(__VA_ARGS__)

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
                LP_ADC.meas1_ctrl2.meas1_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                LP_ADC.meas1_ctrl2.sar1_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
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
                LP_ADC.meas2_ctrl2.meas2_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
                LP_ADC.meas2_ctrl2.sar2_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
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
__attribute__((always_inline))
static inline void adc_ll_set_arbiter_priority(uint8_t pri_rtc, uint8_t pri_dig, uint8_t pri_pwdet)
{
    if (pri_rtc != pri_dig && pri_rtc != pri_pwdet && pri_dig != pri_pwdet) {
        ADC.arb_ctrl.arb_rtc_priority = pri_rtc;
        ADC.arb_ctrl.arb_apb_priority = pri_dig;
        ADC.arb_ctrl.arb_wifi_priority = pri_pwdet;
    }
    /* Should select highest priority controller. */
    if (pri_rtc > pri_dig) {
        ADC.arb_ctrl.arb_apb_force = 0;
        ADC.arb_ctrl.arb_rtc_force = 1;
        ADC.arb_ctrl.arb_wifi_force = 0;
    } else {
        ADC.arb_ctrl.arb_apb_force = 1;
        ADC.arb_ctrl.arb_rtc_force = 0;
        ADC.arb_ctrl.arb_wifi_force = 0;
    }
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
 * ADC2 can use arbiter. The arbitration result can be judged by the flag bit in the original data.
 *
 * @param adc_n ADC unit.
 * @param raw   ADC raw data input (convert value).
 * @return
 *      - true: raw data is valid
 *      - false: raw data is invalid
 */
static inline bool adc_oneshot_ll_raw_check_valid(adc_unit_t adc_n, uint32_t raw)
{
    if (adc_n == ADC_UNIT_1) {
        return true;
    }
    adc_ll_rtc_output_data_t *temp = (adc_ll_rtc_output_data_t *)&raw;
    if (temp->flag == 0) {
        return true;
    } else {
        //Could be ADC_LL_RTC_CTRL_UNSELECTED, ADC_LL_RTC_CTRL_BREAK or ADC_LL_RTC_DATA_FAIL
        return false;
    }
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
    HP_SYS_CLKRST.soc_clk_ctrl2.reg_adc_apb_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl23.reg_adc_clk_en = 1;
}

/**
 * Disable oneshot conversion trigger for all the ADC units
 */
static inline void adc_oneshot_ll_disable_all_unit(void)
{
    HP_SYS_CLKRST.soc_clk_ctrl2.reg_adc_apb_clk_en = 0;
    HP_SYS_CLKRST.peri_clk_ctrl23.reg_adc_clk_en = 0;
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
