#pragma once

#include "soc/soc_caps.h"
#include "hal/adc_types.h"
#include "hal/adc_ll.h"
#include "esp_err.h"

#if CONFIG_IDF_TARGET_ESP32C3
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"
#include "hal/dma_types.h"
#include "hal/adc_ll.h"
#include "hal/dma_types.h"
#include "esp_err.h"

//For ADC module, each conversion contains 4 bytes
#define ADC_HAL_DATA_LEN_PER_CONV 4

/**
 * @brief Enum for DMA descriptor status
 */
typedef enum adc_hal_dma_desc_status_t {
    ADC_HAL_DMA_DESC_VALID   = 0,            ///< This DMA descriptor is written by HW already
    ADC_HAL_DMA_DESC_WAITING = 1,            ///< This DMA descriptor is not written by HW yet
    ADC_HAL_DMA_DESC_NULL    = 2             ///< This DMA descriptor is NULL
} adc_hal_dma_desc_status_t;

/**
 * @brief Configuration of the HAL
 */
typedef struct adc_hal_config_t {
    uint32_t            desc_max_num;       ///< Number of the descriptors linked once
    uint32_t            dma_chan;           ///< DMA channel to be used
    uint32_t            eof_num;            ///< Bytes between 2 in_suc_eof interrupts
} adc_hal_config_t;

/**
 * @brief Context of the HAL
 */
typedef struct adc_hal_context_t {
    /**< this needs to be malloced by the driver layer first */
    dma_descriptor_t    *rx_desc;           ///< DMA descriptors

    /**< these will be assigned by hal layer itself */
    gdma_dev_t          *dev;               ///< GDMA address
    dma_descriptor_t    desc_dummy_head;    ///< Dummy DMA descriptor for ``cur_desc_ptr`` to start
    dma_descriptor_t    *cur_desc_ptr;      ///< Pointer to the current descriptor

    /**< these need to be configured by `adc_hal_config_t` via driver layer*/
    uint32_t            desc_max_num;       ///< Number of the descriptors linked once
    uint32_t            dma_chan;           ///< DMA channel to be used
    uint32_t            eof_num;            ///< Words between 2 in_suc_eof interrupts
} adc_hal_context_t;
#endif

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/
/**
 * ADC module initialization.
 */
void adc_hal_init(void);

/**
 * Set ADC module power management.
 *
 * @prarm manage Set ADC power status.
 */
#define adc_hal_set_power_manage(manage) adc_ll_set_power_manage(manage)

/**
 * ADC module clock division factor setting. ADC clock devided from APB clock.
 *
 * @prarm div Division factor.
 */
#define adc_hal_digi_set_clk_div(div) adc_ll_digi_set_clk_div(div)

#if !CONFIG_IDF_TARGET_ESP32C3
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
#endif  //#if !CONFIG_IDF_TARGET_ESP32C3

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
/**
 * Get the attenuation of a particular channel on ADCn.
 *
 * @param adc_n ADC unit.
 * @param channel ADCn channel number.
 * @return atten The attenuation option.
 */
#define adc_hal_get_atten(adc_n, channel) adc_ll_get_atten(adc_n, channel)
#endif

#if CONFIG_IDF_TARGET_ESP32
/**
 * Close ADC AMP module if don't use it for power save.
 */
#define adc_hal_amp_disable() adc_ll_amp_disable()
#endif

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
#if !CONFIG_IDF_TARGET_ESP32C3
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
#endif  //#if !CONFIG_IDF_TARGET_ESP32C3

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

/*---------------------------------------------------------------
                    ADC Single Read
---------------------------------------------------------------*/
#if !CONFIG_IDF_TARGET_ESP32C3
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
 * @param adc_n   ADC unit.
 * @param channel ADCn channel number.
 * @param atten   ADC attenuation. See ``adc_atten_t``
 */
#define adc_hal_set_atten(adc_n, channel, atten) adc_ll_set_atten(adc_n, channel, atten)

#else // CONFIG_IDF_TARGET_ESP32C3
/**
 * Set the attenuation for ADC to single read
 *
 * @note All ADC units and channels will share the setting. So PLEASE DO save your attenuations and reset them by calling this API again in your driver
 *
 * @param adc_n    Not used, leave here for chip version compatibility
 * @param channel  Not used, leave here for chip version compatibility
 * @param atten    ADC attenuation. See ``adc_atten_t``
 */
#define adc_hal_set_atten(adc_n, channel, atten) adc_ll_onetime_set_atten(atten)
#endif

/**
 * Start an ADC conversion and get the converted value.
 *
 * @note It may be block to wait conversion finish.
 *
 * @param      adc_n   ADC unit.
 * @param      channel ADC channel number.
 * @param[out] out_raw ADC converted result
 *
 * @return
 *      - ESP_OK:                The value is valid.
 *      - ESP_ERR_INVALID_STATE: The value is invalid.
 */
esp_err_t adc_hal_convert(adc_ll_num_t adc_n, int channel, int *out_raw);

/*---------------------------------------------------------------
                    ADC calibration setting
---------------------------------------------------------------*/
#if SOC_ADC_HW_CALIBRATION_V1
// ESP32-S2 and C3 support HW offset calibration.

/**
 * @brief Initialize default parameter for the calibration block.
 *
 * @param adc_n ADC index numer
 */
void adc_hal_calibration_init(adc_ll_num_t adc_n);

/**
 * Set the calibration result (initial data) to ADC.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 * @param param the calibration parameter to configure
 */
void adc_hal_set_calibration_param(adc_ll_num_t adc_n, uint32_t param);

/**
 * Calibrate the ADC using internal connections.
 *
 * @note  Different ADC units and different attenuation options use different calibration data (initial data).
 *
 * @param adc_n ADC index number.
 * @param channel adc channel number.
 * @param atten The attenuation for the channel
 * @param internal_gnd true:  Disconnect from the IO port and use the internal GND as the calibration voltage.
 *                     false: Use IO external voltage as calibration voltage.
 *
 * @return
 *      - The calibration result (initial data) to ADC, use `adc_hal_set_calibration_param` to set.
 */
uint32_t adc_hal_self_calibration(adc_ll_num_t adc_n, adc_channel_t channel, adc_atten_t atten, bool internal_gnd);

#endif //SOC_ADC_HW_CALIBRATION_V1

#if CONFIG_IDF_TARGET_ESP32C3
/*---------------------------------------------------------------
                    DMA setting
---------------------------------------------------------------*/
/**
 * @brief Initialize the hal context
 *
 * @param hal    Context of the HAL
 * @param config Configuration of the HAL
 */
void adc_hal_context_config(adc_hal_context_t *hal, const adc_hal_config_t *config);

/**
 * @brief Initialize the HW
 *
 * @param hal Context of the HAL
 */
void adc_hal_digi_init(adc_hal_context_t *hal);

/**
 * @brief Reset ADC / DMA fifo
 *
 * @param hal Context of the HAL
 */
void adc_hal_fifo_reset(adc_hal_context_t *hal);

/**
 * @brief Start DMA
 *
 * @param hal      Context of the HAL
 * @param data_buf Pointer to the data buffer, the length should be multiple of ``desc_max_num`` and ``eof_num`` in ``adc_hal_context_t``
 */
void adc_hal_digi_rxdma_start(adc_hal_context_t *hal, uint8_t *data_buf);

/**
 * @brief Start ADC
 *
 * @param hal Context of the HAL
 */
void adc_hal_digi_start(adc_hal_context_t *hal);

/**
 * @brief Get the ADC reading result
 *
 * @param      hal           Context of the HAL
 * @param      eof_desc_addr The last descriptor that is finished by HW. Should be got from DMA
 * @param[out] cur_desc      The descriptor with ADC reading result (from the 1st one to the last one (``eof_desc_addr``))
 *
 * @return                   See ``adc_hal_dma_desc_status_t``
 */
adc_hal_dma_desc_status_t adc_hal_get_reading_result(adc_hal_context_t *hal, const intptr_t eof_desc_addr, dma_descriptor_t **cur_desc);

/**
 * @brief Stop DMA
 *
 * @param hal Context of the HAL
 */
void adc_hal_digi_rxdma_stop(adc_hal_context_t *hal);

/**
 * @brief Clear interrupt
 *
 * @param hal  Context of the HAL
 * @param mask mask of the interrupt
 */
void adc_hal_digi_clr_intr(adc_hal_context_t *hal, uint32_t mask);

/**
 * @brief Enable interrupt
 *
 * @param hal  Context of the HAL
 * @param mask mask of the interrupt
 */
void adc_hal_digi_dis_intr(adc_hal_context_t *hal, uint32_t mask);

/**
 * @brief Stop ADC
 *
 * @param hal Context of the HAL
 */
void adc_hal_digi_stop(adc_hal_context_t *hal);

#endif  //#if CONFIG_IDF_TARGET_ESP32C3
