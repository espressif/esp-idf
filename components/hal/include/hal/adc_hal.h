#pragma once

#include "hal/adc_types.h"
#include "hal/adc_ll.h"

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/
/**
 * ADC module initialization.
 */
void adc_hal_init(void);

/**
 * ADC module deinitialization.
 */
void adc_hal_deinit(void);

/**
 * Set adc sample cycle.
 *
 * @note Normally, please use default value.
 * @param sample_cycle The number of ADC sampling cycles. Range: 1 ~ 7.
 */
#define adc_hal_set_sample_cycle(sample_cycle) adc_ll_set_sample_cycle(sample_cycle)

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
#define adc_hal_digi_set_clk_div(div) adc_ll_digi_set_clk_div(div)

/**
 * ADC SAR clock division factor setting. ADC SAR clock devided from `RTC_FAST_CLK`.
 *
 * @prarm div Division factor.
 */
#define adc_hal_set_sar_clk_div(adc_n, div) adc_ll_set_sar_clk_div(adc_n, div)

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
 * Get the attenuation of a particular channel on ADCn.
 *
 * @param adc_n ADC unit.
 * @param channel ADCn channel number.
 * @return atten The attenuation option.
 */
#define adc_hal_get_atten(adc_n, channel) adc_ll_get_atten(adc_n, channel)

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
 * Get the converted value for each ADCn for RTC controller.
 *
 * @note It may be block to wait conversion finish.
 *
 * @prarm adc_n ADC unit.
 * @param channel adc channel number.
 * @param value Pointer for touch value.
 *
 * @return
 *      - 0: The value is valid.
 *      - ~0: The value is invalid.
 */
int adc_hal_convert(adc_ll_num_t adc_n, int channel, int *value);

/**
 * Set adc output data format for RTC controller.
 *
 * @prarm adc_n ADC unit.
 * @prarm bits Output data bits width option.
 */
#define adc_hal_rtc_set_output_format(adc_n, bits) adc_ll_rtc_set_output_format(adc_n, bits)

/**
 * ADC module output data invert or not.
 *
 * @prarm adc_n ADC unit.
 */
#define adc_hal_rtc_output_invert(adc_n, inv_en) adc_ll_rtc_output_invert(adc_n, inv_en)

/**
 *  Enable/disable the output of ADCn's internal reference voltage to one of ADC2's channels.
 *
 *  This function routes the internal reference voltage of ADCn to one of
 *  ADC2's channels. This reference voltage can then be manually measured
 *  for calibration purposes.
 *
 *  @note  ESP32 only supports output of ADC2's internal reference voltage.
 *  @param[in]  adc ADC unit select
 *  @param[in]  channel ADC2 channel number
 *  @param[in]  en Enable/disable the reference voltage output
 */
#define adc_hal_vref_output(adc, channel, en) adc_ll_vref_output(adc, channel, en)

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
/**
 * Digital controller initialization.
 */
void adc_hal_digi_init(void);

/**
 * Digital controller deinitialization.
 */
void adc_hal_digi_deinit(void);

/**
 * Setting the digital controller.
 *
 * @param cfg Pointer to digital controller paramter.
 */
void adc_hal_digi_controller_config(const adc_digi_config_t *cfg);

/**
 * Reset the pattern table pointer, then take the measurement rule from table header in next measurement.
 *
 * @param adc_n ADC unit.
 */
#define adc_hal_digi_clear_pattern_table(adc_n) adc_ll_digi_clear_pattern_table(adc_n)
