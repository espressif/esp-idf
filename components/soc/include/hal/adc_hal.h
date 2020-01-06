#pragma once

#include "hal/adc_types.h"
#include "hal/adc_ll.h"

typedef struct {
    bool conv_limit_en;
    uint32_t conv_limit_num;
    uint32_t clk_div;
    uint32_t adc1_pattern_len;
    uint32_t adc2_pattern_len;
    adc_ll_pattern_table_t *adc1_pattern;
    adc_ll_pattern_table_t *adc2_pattern;
    adc_ll_convert_mode_t conv_mode;
    adc_ll_dig_output_format_t format;
} adc_hal_dig_config_t;

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/
/**
 * ADC module initialization.
 */
void adc_hal_init(void);

/**
 * Set adc sample cycle for digital controller.
 *
 * @note Normally, please use default value.
 * @param sample_cycle Cycles between DIG ADC controller start ADC sensor and beginning to receive data from sensor.
 *                     Range: 2 ~ 0xFF.
 */
#define adc_hal_dig_set_sample_cycle(sample_cycle) adc_ll_dig_set_sample_cycle(sample_cycle)

/**
 * Set ADC module power management.
 *
 * @prarm manage Set ADC power status.
 */
#define adc_hal_set_power_manage(manage) adc_ll_set_power_manage(manage)

/**
 * Get ADC module power management.
 *
 * @return
 *      - ADC power status.
 */
#define adc_hal_get_power_manage() adc_ll_get_power_manage()

/**
 * ADC module clock division factor setting. ADC clock devided from APB clock.
 *
 * @prarm div Division factor.
 */
#define adc_hal_set_clk_div(div) adc_ll_set_clk_div(div)

/**
 * ADC module output data invert or not.
 *
 * @prarm adc_n ADC unit.
 */
#define adc_hal_output_invert(adc_n, inv_en) adc_ll_output_invert(adc_n, inv_en)

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
#define adc_hal_set_controller(adc_n, ctrl) adc_ll_set_controller(adc_n, ctrl)

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
#define adc_hal_set_atten(adc_n, channel, atten) adc_ll_set_atten(adc_n, channel, atten)

/**
 * Close ADC AMP module if don't use it for power save.
 */
#define adc_hal_amp_disable() adc_ll_amp_disable()

/*---------------------------------------------------------------
                    PWDET(Power detect) controller setting
---------------------------------------------------------------*/

/**
 * Set adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @prarm cct Range: 0 ~ 7.
 */
#define adc_hal_pwdet_set_cct(cct) adc_ll_pwdet_set_cct(cct)

/**
 * Get adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @return cct Range: 0 ~ 7.
 */
#define adc_hal_pwdet_get_cct() adc_ll_pwdet_get_cct()

/*---------------------------------------------------------------
                    RTC controller setting
---------------------------------------------------------------*/

/**
 * Set adc output data format for RTC controller.
 *
 * @prarm adc_n ADC unit.
 * @prarm bits Output data bits width option.
 */
#define adc_hal_rtc_set_output_format(adc_n, bits) adc_ll_rtc_set_output_format(adc_n, bits)

/**
 * Get the converted value for each ADCn for RTC controller.
 *
 * @note It may be block to wait conversion finish.
 * @prarm adc_n ADC unit.
 * @return
 *      - Converted value.
 */
int adc_hal_convert(adc_ll_num_t adc_n, int channel);

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
/**
 * Setting the digital controller.
 *
 * @prarm adc_hal_dig_config_t cfg Pointer to digital controller paramter.
 */
void adc_hal_dig_controller_config(const adc_hal_dig_config_t *cfg);

/**
 * Set I2S DMA data source for digital controller.
 *
 * @param src i2s data source.
 */
#define adc_hal_dig_set_data_source(src) adc_ll_dig_set_data_source(src)

/*---------------------------------------------------------------
                    Hall sensor setting
---------------------------------------------------------------*/

/**
 * Enable hall sensor.
 */
#define adc_hal_hall_enable() adc_ll_hall_enable()

/**
 * Disable hall sensor.
 */
#define adc_hal_hall_disable() adc_ll_hall_disable()

/**
 *  Start hall convert and return the hall value.
 *
 *  @return Hall value.
 */
int adc_hal_hall_convert(void);

/**
 *  @brief Output ADC2 reference voltage to gpio
 *
 *  This function utilizes the testing mux exclusive to ADC2 to route the
 *  reference voltage one of ADC2's channels.
 *
 *  @param[in]  io    GPIO number
 *  @return
 *                  - true: v_ref successfully routed to selected gpio
 *                  - false: Unsupported gpio
 */
#define adc_hal_vref_output(io) adc_ll_vref_output(io)