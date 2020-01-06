#pragma once

#include "soc/adc_periph.h"
#include "hal/adc_types.h"
#include <stdbool.h>

typedef enum {
    ADC_DIG_FORMAT_12BIT,   /*!< ADC to I2S data format, [15:12]-channel [11:0]-12 bits ADC data.
                                 Note: In single convert mode. */
    ADC_DIG_FORMAT_11BIT,   /*!< ADC to I2S data format, [15]-1 [14:11]-channel [10:0]-11 bits ADC data.
                                 Note: In multi convert mode. */
    ADC_DIG_FORMAT_MAX,
} adc_ll_dig_output_format_t;

typedef enum {
    ADC_CONV_SINGLE_UNIT_1 = 1, /*!< SAR ADC 1*/
    ADC_CONV_SINGLE_UNIT_2 = 2, /*!< SAR ADC 2, not supported yet*/
    ADC_CONV_BOTH_UNIT     = 3, /*!< SAR ADC 1 and 2, not supported yet */
    ADC_CONV_ALTER_UNIT    = 7, /*!< SAR ADC 1 and 2 alternative mode, not supported yet */
    ADC_CONV_UNIT_MAX,
} adc_ll_convert_mode_t;

typedef enum {
    ADC_NUM_1 = 0,          /*!< SAR ADC 1 */
    ADC_NUM_2 = 1,          /*!< SAR ADC 2 */
    ADC_NUM_MAX,
} adc_ll_num_t;

typedef struct {
    union {
        struct {
            uint8_t atten:     2;   /*!< ADC sampling voltage attenuation configuration.
                                         0: input voltage * 1;
                                         1: input voltage * 1/1.34;
                                         2: input voltage * 1/2;
                                         3: input voltage * 1/3.6. */
            uint8_t bit_width: 2;   /*!< ADC resolution.
                                         0: 9 bit;
                                         1: 10 bit;
                                         2: 11 bit;
                                         3: 12 bit. */
            uint8_t channel:   4;   /*!< ADC channel index. */
        };
        uint8_t val;
    };
} adc_ll_pattern_table_t;

typedef enum {
    ADC_POWER_BY_FSM,   /*!< ADC XPD controled by FSM. Used for polling mode */
    ADC_POWER_SW_ON,    /*!< ADC XPD controled by SW. power on. Used for DMA mode */
    ADC_POWER_SW_OFF,   /*!< ADC XPD controled by SW. power off. */
    ADC_POWER_MAX,      /*!< For parameter check. */
} adc_ll_power_t;

typedef enum {
    ADC_HALL_CTRL_ULP = 0x0,/*!< Hall sensor controled by ULP */
    ADC_HALL_CTRL_RTC = 0x1 /*!< Hall sensor controled by RTC */
} adc_ll_hall_controller_t ;

typedef enum {
    ADC_CTRL_RTC = 0,
    ADC_CTRL_ULP = 1,
    ADC_CTRL_DIG = 2,
    ADC2_CTRL_PWDET = 3,
} adc_ll_controller_t ;

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
static inline void adc_ll_dig_set_fsm_time(uint32_t rst_wait, uint32_t start_wait, uint32_t standby_wait)
{
    // Internal FSM reset wait time
    SYSCON.saradc_fsm_wait.rstb_wait = rst_wait;
    // Internal FSM start wait time
    SYSCON.saradc_fsm_wait.xpd_wait = start_wait;
    // Internal FSM standby wait time
    SYSCON.saradc_fsm_wait.standby_wait = standby_wait;
}

/**
 * Set adc sample cycle for digital controller.
 *
 * @note Normally, please use default value.
 * @param sample_cycle Cycles between DIG ADC controller start ADC sensor and beginning to receive data from sensor.
 *                     Range: 2 ~ 0xFF.
 */
static inline void adc_ll_dig_set_sample_cycle(uint32_t sample_cycle)
{
    SYSCON.saradc_fsm.sample_cycle = sample_cycle;
}

/**
 * Set adc output data format for digital controller.
 *
 * @param format Output data format.
 */
static inline void adc_ll_dig_set_output_format(adc_ll_dig_output_format_t format)
{
    SYSCON.saradc_ctrl.data_sar_sel = format;
}

/**
 * Set adc max conversion number for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 *
 * @param meas_num Max conversion number. Range: 0 ~ 255.
 */
static inline void adc_ll_dig_set_convert_limit_num(uint32_t meas_num)
{
    SYSCON.saradc_ctrl2.max_meas_num = meas_num;
}

/**
 * Enable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 */
static inline void adc_ll_dig_convert_limit_enable(void)
{
    SYSCON.saradc_ctrl2.meas_num_limit = 1;
}

/**
 * Disable max conversion number detection for digital controller.
 * If the number of ADC conversion is equal to the maximum, the conversion is stopped.
 */
static inline void adc_ll_dig_convert_limit_disable(void)
{
    SYSCON.saradc_ctrl2.meas_num_limit = 0;
}

/**
 * Set adc conversion mode for digital controller.
 *
 * @note ESP32 only support ADC1 single mode.
 *
 * @param mode Conversion mode select.
 */
static inline void adc_ll_dig_set_convert_mode(adc_ll_convert_mode_t mode)
{
    if (mode == ADC_CONV_SINGLE_UNIT_1) {
        SYSCON.saradc_ctrl.work_mode = 0;
        SYSCON.saradc_ctrl.sar_sel = 0;
    } else if (mode == ADC_CONV_SINGLE_UNIT_2) {
        SYSCON.saradc_ctrl.work_mode = 0;
        SYSCON.saradc_ctrl.sar_sel = 1;
    } else if (mode == ADC_CONV_BOTH_UNIT) {
        SYSCON.saradc_ctrl.work_mode = 1;
    } else if (mode == ADC_CONV_ALTER_UNIT) {
        SYSCON.saradc_ctrl.work_mode = 2;
    }
}

/**
 * Set I2S DMA data source for digital controller.
 *
 * @param src i2s data source.
 */
static inline void adc_ll_dig_set_data_source(adc_i2s_source_t src)
{
    /* 1: I2S input data is from SAR ADC (for DMA)  0: I2S input data is from GPIO matrix */
    SYSCON.saradc_ctrl.data_to_i2s = src;
}

/**
 * Set pattern table lenth for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself.
 *
 * @prarm adc_n ADC unit.
 * @param patt_len Items range: 1 ~ 16.
 */
static inline void adc_ll_set_pattern_table_len(adc_ll_num_t adc_n, uint32_t patt_len)
{
    if (adc_n == ADC_NUM_1) {
        SYSCON.saradc_ctrl.sar1_patt_len = patt_len - 1;
    } else { // adc_n == ADC_NUM_2
        SYSCON.saradc_ctrl.sar2_patt_len = patt_len - 1;
    }
}

/**
 * Set pattern table lenth for digital controller.
 * The pattern table that defines the conversion rules for each SAR ADC. Each table has 16 items, in which channel selection,
 * resolution and attenuation are stored. When the conversion is started, the controller reads conversion rules from the
 * pattern table one by one. For each controller the scan sequence has at most 16 different rules before repeating itself.
 *
 * @prarm adc_n ADC unit.
 * @param pattern_index Items index. Range: 1 ~ 16.
 * @param pattern Stored conversion rules.
 */
static inline void adc_ll_set_pattern_table(adc_ll_num_t adc_n, uint32_t pattern_index, adc_ll_pattern_table_t pattern)
{
    uint32_t tab;
    uint8_t *arg;
    if (adc_n == ADC_NUM_1) {
        tab = SYSCON.saradc_sar1_patt_tab[pattern_index / 4];
        arg = (uint8_t *)&tab;
        arg[pattern_index % 4] = pattern.val;
        SYSCON.saradc_sar1_patt_tab[pattern_index / 4] = tab;
    } else { // adc_n == ADC_NUM_2
        tab = SYSCON.saradc_sar2_patt_tab[pattern_index / 4];
        arg = (uint8_t *)&tab;
        arg[pattern_index % 4] = pattern.val;
        SYSCON.saradc_sar2_patt_tab[pattern_index / 4] = tab;
    }
}

/*---------------------------------------------------------------
                    PWDET(Power detect) controller setting
---------------------------------------------------------------*/
/**
 * Set adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @prarm cct Range: 0 ~ 7.
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
 * @prarm adc_n ADC unit.
 * @prarm bits Output data bits width option.
 */
static inline void adc_ll_rtc_set_output_format(adc_ll_num_t adc_n, adc_bits_width_t bits)
{
    if (adc_n == ADC_NUM_1) {
        SENS.sar_meas1_ctrl1.sar1_bit_width = bits;
        SENS.sar_reader1_ctrl.sar1_sample_bit = bits;
    } else { // adc_n == ADC_NUM_2
        SENS.sar_meas2_ctrl1.sar2_bit_width = bits;
        SENS.sar_reader2_ctrl.sar2_sample_bit = bits;
    }
}

/**
 * Enable adc channel to start convert.
 *
 * @note Only one channel can be selected for once measurement.
 *
 * @prarm adc_n ADC unit.
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
 * Start conversion once by software for RTC controller.
 *
 * @note It may be block to wait conversion idle for ADC1.
 *
 * @prarm adc_n ADC unit.
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
 * @prarm adc_n ADC unit.
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
 * @prarm adc_n ADC unit.
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

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/
/**
 * Set ADC module power management.
 *
 * @prarm manage Set ADC power status.
 */
static inline void adc_ll_set_power_manage(adc_ll_power_t manage)
{
    /* Bit1  0:Fsm  1: SW mode
       Bit0  0:SW mode power down  1: SW mode power on */
    if (manage == ADC_POWER_SW_ON) {
        SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
    } else if (manage == ADC_POWER_BY_FSM) {
        SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_FSM;
    } else if (manage == ADC_POWER_SW_OFF) {
        SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_PD;
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
 * ADC module clock division factor setting. ADC clock devided from APB clock.
 *
 * @prarm div Division factor.
 */
static inline void adc_ll_set_clk_div(uint32_t div)
{
    /* ADC clock devided from APB clk, e.g. 80 / 2 = 40Mhz, */
    SYSCON.saradc_ctrl.sar_clk_div = div;
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
 * @prarm adc_n ADC unit.
 * @prarm channel ADCn channel number.
 * @prarm atten The attenuation option.
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
 * ADC module output data invert or not.
 *
 * @prarm adc_n ADC unit.
 */
static inline void adc_ll_output_invert(adc_ll_num_t adc_n, bool inv_en)
{
    if (adc_n == ADC_NUM_1) {
        SENS.sar_reader1_ctrl.sar1_data_inv = inv_en;   // Enable / Disable ADC data invert
    } else { // adc_n == ADC_NUM_2
        SENS.sar_reader2_ctrl.sar2_data_inv = inv_en;  // Enable / Disable ADC data invert
    }
}

/**
 * Set ADC module controller.
 * There are five SAR ADC controllers:
 * Two digital controller: Continuous conversion mode (DMA). High performance with multiple channel scan modes;
 * Two RTC controller: Single conversion modes (Polling). For low power purpose working during deep sleep;
 * the other is dedicated for Power detect (PWDET / PKDET), Only support ADC2.
 *
 * @prarm adc_n ADC unit.
 * @prarm ctrl ADC controller.
 */
static inline void adc_ll_set_controller(adc_ll_num_t adc_n, adc_ll_controller_t ctrl)
{
    if (adc_n == ADC_NUM_1) {
        switch ( ctrl ) {
        case ADC_CTRL_RTC:
            SENS.sar_meas1_mux.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_hall_ctrl.xpd_hall_force     = 1;    // 1: SW control HALL power;        0: ULP FSM control HALL power.
            SENS.sar_hall_ctrl.hall_phase_force   = 1;    // 1: SW control HALL phase;        0: ULP FSM control HALL phase.
            break;
        case ADC_CTRL_ULP:
            SENS.sar_meas1_mux.sar1_dig_force       = 0;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_meas1_ctrl2.meas1_start_force  = 0;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 0;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_hall_ctrl.xpd_hall_force     = 0;    // 1: SW control HALL power;        0: ULP FSM control HALL power.
            SENS.sar_hall_ctrl.hall_phase_force   = 0;    // 1: SW control HALL phase;        0: ULP FSM control HALL phase.
            break;
        case ADC_CTRL_DIG:
            SENS.sar_meas1_mux.sar1_dig_force       = 1;    // 1: Select digital control;       0: Select RTC control.
            SENS.sar_meas1_ctrl2.meas1_start_force  = 1;    // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
            SENS.sar_meas1_ctrl2.sar1_en_pad_force  = 1;    // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
            SENS.sar_hall_ctrl.xpd_hall_force     = 1;    // 1: SW control HALL power;        0: ULP FSM control HALL power.
            SENS.sar_hall_ctrl.hall_phase_force   = 1;    // 1: SW control HALL phase;        0: ULP FSM control HALL phase.
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
 * Close ADC AMP module if don't use it for power save.
 */
static inline void adc_ll_amp_disable(void)
{
    //channel is set in the  convert function
    SENS.sar_meas1_ctrl1.force_xpd_amp = SENS_FORCE_XPD_AMP_PD;
    //disable FSM, it's only used by the LNA.
    SENS.sar_amp_ctrl3.amp_rst_fb_fsm = 0;
    SENS.sar_amp_ctrl3.amp_short_ref_fsm = 0;
    SENS.sar_amp_ctrl3.amp_short_ref_gnd_fsm = 0;
    SENS.sar_amp_ctrl1.sar_amp_wait1 = 1;
    SENS.sar_amp_ctrl1.sar_amp_wait2 = 1;
    SENS.sar_amp_ctrl2.sar_amp_wait3 = 1;
}

/*---------------------------------------------------------------
                    Hall sensor setting
---------------------------------------------------------------*/

/**
 * Enable hall sensor.
 */
static inline void adc_ll_hall_enable(void)
{
    SENS.sar_hall_ctrl.xpd_hall = 1;
}

/**
 * Disable hall sensor.
 */
static inline void adc_ll_hall_disable(void)
{
    SENS.sar_hall_ctrl.xpd_hall = 0;
}

/**
 * Reverse phase of hall sensor.
 */
static inline void adc_ll_hall_phase_enable(void)
{
    SENS.sar_hall_ctrl.hall_phase = 1;
}

/**
 * Don't reverse phase of hall sensor.
 */
static inline void adc_ll_hall_phase_disable(void)
{
    SENS.sar_hall_ctrl.hall_phase = 0;
}

/**
 * Set hall sensor controller.
 *
 * @param hall_ctrl Hall controller.
 */
static inline void adc_ll_set_hall_controller(adc_ll_hall_controller_t hall_ctrl)
{
    SENS.sar_hall_ctrl.xpd_hall_force = hall_ctrl;    // 1: SW control HALL power;    0: ULP FSM control HALL power.
    SENS.sar_hall_ctrl.hall_phase_force = hall_ctrl;  // 1: SW control HALL phase;    0: ULP FSM control HALL phase.
}

/**
 *  Output ADC2 reference voltage to gpio 25 or 26 or 27
 *
 *  This function utilizes the testing mux exclusive to ADC 2 to route the
 *  reference voltage one of ADC2's channels. Supported gpios are gpios
 *  25, 26, and 27. This refernce voltage can be manually read from the pin
 *  and used in the esp_adc_cal component.
 *
 *  @param[in]  io    GPIO number (gpios 25,26,27 supported)
 *
 *  @return
 *                  - true: v_ref successfully routed to selected gpio
 *                  - false: Unsupported gpio
 */
static inline bool adc_ll_vref_output(int io)
{
    int channel;
    if (io == 25) {
        channel = 8;    //Channel 8 bit
    } else if (io == 26) {
        channel = 9;    //Channel 9 bit
    } else if (io == 27) {
        channel = 7;    //Channel 7 bit
    } else {
        return false;
    }
    RTCCNTL.bias_conf.dbg_atten = 0;     //Check DBG effect outside sleep mode
    //set dtest (MUX_SEL : 0 -> RTC; 1-> vdd_sar2)
    RTCCNTL.test_mux.dtest_rtc = 1;      //Config test mux to route v_ref to ADC2 Channels
    //set ent
    RTCCNTL.test_mux.ent_rtc = 1;
    //set sar2_en_test
    SENS.sar_meas2_ctrl1.sar2_en_test = 1;
    //set sar2 en force
    SENS.sar_meas2_ctrl2.sar2_en_pad_force = 1;      //Pad bitmap controlled by SW
    //set en_pad for channels 7,8,9 (bits 0x380)
    SENS.sar_meas2_ctrl2.sar2_en_pad = 1 << channel;
    return true;
}
