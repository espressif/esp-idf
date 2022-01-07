/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "soc/soc_caps.h"
#include "hal/dma_types.h"
#include "hal/adc_types.h"
#include "hal/adc_ll.h"
#include "esp_err.h"

#if SOC_GDMA_SUPPORTED
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"
#endif

#if CONFIG_IDF_TARGET_ESP32S2
//ADC utilises SPI3 DMA on ESP32S2
#include "hal/spi_ll.h"
#endif

#if CONFIG_IDF_TARGET_ESP32
//ADC utilises I2S0 DMA on ESP32
#include "hal/i2s_ll.h"
#endif

#if SOC_GDMA_SUPPORTED
#define ADC_HAL_DMA_INTR_MASK                           GDMA_LL_EVENT_RX_SUC_EOF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_HAL_DMA_INTR_MASK                           SPI_LL_INTR_IN_SUC_EOF
#else //CONFIG_IDF_TARGET_ESP32
#define ADC_HAL_DMA_INTR_MASK                           BIT(9)
#endif

//For ADC module, each conversion contains 4 bytes
#define ADC_HAL_DATA_LEN_PER_CONV 4

typedef enum adc_hal_work_mode_t {
    ADC_HAL_ULP_MODE,
    ADC_HAL_SINGLE_READ_MODE,
    ADC_HAL_CONTINUOUS_READ_MODE,
    ADC_HAL_PWDET_MODE
} adc_hal_work_mode_t;

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
    void                *dev;               ///< DMA peripheral address
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
    dma_descriptor_t    desc_dummy_head;    ///< Dummy DMA descriptor for ``cur_desc_ptr`` to start
    dma_descriptor_t    *cur_desc_ptr;      ///< Pointer to the current descriptor

    /**< these need to be configured by `adc_hal_config_t` via driver layer*/
    void                *dev;               ///< DMA address
    uint32_t            desc_max_num;       ///< Number of the descriptors linked once
    uint32_t            dma_chan;           ///< DMA channel to be used
    uint32_t            eof_num;            ///< Words between 2 in_suc_eof interrupts
} adc_hal_context_t;

typedef struct adc_hal_digi_ctrlr_cfg_t {
    bool                        conv_limit_en;      //1: adc conversion will stop when `conv_limit_num` reaches. 0: won't stop. NOTE: esp32 should always be set to 1.
    uint32_t                    conv_limit_num;     //see `conv_limit_en`
    uint32_t                    adc_pattern_len;    //total pattern item number, including ADC1 and ADC2
    adc_digi_pattern_config_t   *adc_pattern;       //pattern item
    uint32_t                    sample_freq_hz;     //ADC sample frequency
    adc_digi_convert_mode_t     conv_mode;          //controller work mode
    uint32_t                    bit_width;          //output data width
} adc_hal_digi_ctrlr_cfg_t;


/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/
/**
 * Set ADC module power management.
 *
 * @prarm manage Set ADC power status.
 */
#define adc_hal_set_power_manage(manage) adc_ll_set_power_manage(manage)

void adc_hal_set_controller(adc_ll_num_t unit, adc_hal_work_mode_t work_mode);

#if SOC_ADC_ARBITER_SUPPORTED
//No ADC2 controller arbiter on ESP32
/**
 * Config ADC2 module arbiter.
 * The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 * the low priority controller will read the invalid ADC2 data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 * @note Default priority: Wi-Fi > RTC > Digital;
 *
 * @param config Refer to ``adc_arbiter_t``.
 */
void adc_hal_arbiter_config(adc_arbiter_t *config);
#endif  //#if SOC_ADC_ARBITER_SUPPORTED

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
 * ADC module initialization.
 */
void adc_hal_init(void);

/**
 * Digital controller deinitialization.
 *
 * @param hal Context of the HAL
 */
void adc_hal_digi_deinit(adc_hal_context_t *hal);

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
 * Setting the digital controller.
 *
 * @param hal    Context of the HAL
 * @param cfg    Pointer to digital controller paramter.
 */
void adc_hal_digi_controller_config(adc_hal_context_t *hal, const adc_hal_digi_ctrlr_cfg_t *cfg);

/**
 * @brief Start Conversion
 *
 * @param hal Context of the HAL
 * @param data_buf Pointer to the data buffer, the length should be multiple of ``desc_max_num`` and ``eof_num`` in ``adc_hal_context_t``
 */
void adc_hal_digi_start(adc_hal_context_t *hal, uint8_t *data_buf);

#if !SOC_GDMA_SUPPORTED
/**
 * @brief Get the DMA descriptor that Hardware has finished processing.
 *
 * @param hal Context of the HAL
 *
 * @return DMA descriptor address
 */
intptr_t adc_hal_get_desc_addr(adc_hal_context_t *hal);

/**
 * @brief Check the hardware interrupt event
 *
 * @param hal Context of the HAL
 * @param mask Event mask
 *
 * @return True: the event is triggered. False: the event is not triggered yet.
 */
bool adc_hal_check_event(adc_hal_context_t *hal, uint32_t mask);
#endif

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
 * @brief Stop conversion
 *
 * @param hal Context of the HAL
 */
void adc_hal_digi_stop(adc_hal_context_t *hal);


/*---------------------------------------------------------------
                    ADC Single Read
---------------------------------------------------------------*/
#if SOC_ADC_RTC_CTRL_SUPPORTED
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

#else // #if !SOC_ADC_RTC_CTRL_SUPPORTED
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
#endif  //#if SOC_ADC_RTC_CTRL_SUPPORTED

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
#if SOC_ADC_CALIBRATION_V1_SUPPORTED

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

#endif //SOC_ADC_CALIBRATION_V1_SUPPORTED


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
 * ADC module output data invert or not.
 *
 * @prarm adc_n ADC unit.
 */
#define adc_hal_rtc_output_invert(adc_n, inv_en) adc_ll_rtc_output_invert(adc_n, inv_en)
