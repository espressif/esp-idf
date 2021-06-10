/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "hal/adc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32
/**** `adc1_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
typedef enum {
    ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO36 */
    ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO37 */
    ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO38 */
    ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO39 */
    ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO32 */
    ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO33 */
    ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO34 */
    ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO35 */
    ADC1_CHANNEL_MAX,
} adc1_channel_t;
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 // TODO ESP32-S3 channels are wrong IDF-1776
/**** `adc1_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
typedef enum {
    ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO1  */
    ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO2  */
    ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO3  */
    ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO4  */
    ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO5  */
    ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO6  */
    ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO7  */
    ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO8  */
    ADC1_CHANNEL_8,     /*!< ADC1 channel 8 is GPIO9  */
    ADC1_CHANNEL_9,     /*!< ADC1 channel 9 is GPIO10 */
    ADC1_CHANNEL_MAX,
} adc1_channel_t;
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
/**** `adc1_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
typedef enum {
    ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO0 */
    ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO1 */
    ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO2 */
    ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO3 */
    ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO4 */
    ADC1_CHANNEL_MAX,
} adc1_channel_t;
#endif // CONFIG_IDF_TARGET_*

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 // TODO ESP32-S3 channels are wrong IDF-1776
/**** `adc2_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
typedef enum {
    ADC2_CHANNEL_0 = 0, /*!< ADC2 channel 0 is GPIO4  (ESP32), GPIO11 (ESP32-S2) */
    ADC2_CHANNEL_1,     /*!< ADC2 channel 1 is GPIO0  (ESP32), GPIO12 (ESP32-S2) */
    ADC2_CHANNEL_2,     /*!< ADC2 channel 2 is GPIO2  (ESP32), GPIO13 (ESP32-S2) */
    ADC2_CHANNEL_3,     /*!< ADC2 channel 3 is GPIO15 (ESP32), GPIO14 (ESP32-S2) */
    ADC2_CHANNEL_4,     /*!< ADC2 channel 4 is GPIO13 (ESP32), GPIO15 (ESP32-S2) */
    ADC2_CHANNEL_5,     /*!< ADC2 channel 5 is GPIO12 (ESP32), GPIO16 (ESP32-S2) */
    ADC2_CHANNEL_6,     /*!< ADC2 channel 6 is GPIO14 (ESP32), GPIO17 (ESP32-S2) */
    ADC2_CHANNEL_7,     /*!< ADC2 channel 7 is GPIO27 (ESP32), GPIO18 (ESP32-S2) */
    ADC2_CHANNEL_8,     /*!< ADC2 channel 8 is GPIO25 (ESP32), GPIO19 (ESP32-S2) */
    ADC2_CHANNEL_9,     /*!< ADC2 channel 9 is GPIO26 (ESP32), GPIO20 (ESP32-S2) */
    ADC2_CHANNEL_MAX,
} adc2_channel_t;
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
/**** `adc2_channel_t` will be deprecated functions, combine into `adc_channel_t` ********/
typedef enum {
    ADC2_CHANNEL_0 = 0, /*!< ADC2 channel 0 is GPIO5 */
    ADC2_CHANNEL_MAX,
} adc2_channel_t;
#endif


/**
 * @brief ADC rtc controller attenuation option.
 *
 * @note  This definitions are only for being back-compatible
 */
#define ADC_ATTEN_0db   ADC_ATTEN_DB_0
#define ADC_ATTEN_2_5db ADC_ATTEN_DB_2_5
#define ADC_ATTEN_6db   ADC_ATTEN_DB_6
#define ADC_ATTEN_11db  ADC_ATTEN_DB_11

/**
 * The default (max) bit width of the ADC of current version. You can also get the maximum bitwidth
 * by `SOC_ADC_MAX_BITWIDTH` defined in soc_caps.h.
 */
#define ADC_WIDTH_BIT_DEFAULT   (ADC_WIDTH_MAX-1)

//this definitions are only for being back-compatible
#define ADC_WIDTH_9Bit  ADC_WIDTH_BIT_9
#define ADC_WIDTH_10Bit ADC_WIDTH_BIT_10
#define ADC_WIDTH_11Bit ADC_WIDTH_BIT_11
#define ADC_WIDTH_12Bit ADC_WIDTH_BIT_12

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
/**
 * @brief Digital ADC DMA read max timeout value, it may make the ``adc_digi_read_bytes`` block forever if the OS supports
 */
#define ADC_MAX_DELAY UINT32_MAX
#endif

/**
 * @brief ADC digital controller encode option.
 *
 * @deprecated The ESP32-S2 doesn't use I2S DMA. Call ``adc_digi_output_format_t`` instead.
 */
typedef enum {
    ADC_ENCODE_12BIT,        /*!< ADC to DMA data format,          , [15:12]-channel [11:0]-12 bits ADC data */
    ADC_ENCODE_11BIT,        /*!< ADC to DMA data format, [15]-unit, [14:11]-channel [10:0]-11 bits ADC data */
    ADC_ENCODE_MAX,
} adc_i2s_encode_t;

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
//This feature is currently supported on ESP32C3, will be supported on other chips soon
/**
 * @brief Digital ADC DMA configuration
 */
typedef struct adc_digi_init_config_s {
    uint32_t max_store_buf_size;    ///< Max length of the converted data that driver can store before they are processed.
    uint32_t conv_num_each_intr;    ///< Bytes of data that can be converted in 1 interrupt.
    uint32_t adc1_chan_mask;        ///< Channel list of ADC1 to be initialized.
    uint32_t adc2_chan_mask;        ///< Channel list of ADC2 to be initialized.
} adc_digi_init_config_t;
#endif

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/

/**
 * @brief Enable ADC power
 * @deprecated Use adc_power_acquire and adc_power_release instead.
 */
void adc_power_on(void) __attribute__((deprecated));

/**
 * @brief Power off SAR ADC
 * @deprecated Use adc_power_acquire and adc_power_release instead.
 * This function will force power down for ADC.
 * This function is deprecated because forcing power ADC power off may
 * disrupt operation of other components which may be using the ADC.
 */
void adc_power_off(void) __attribute__((deprecated));

/**
 * @brief Increment the usage counter for ADC module.
 * ADC will stay powered on while the counter is greater than 0.
 * Call adc_power_release when done using the ADC.
 */
void adc_power_acquire(void);

/**
 * @brief Decrement the usage counter for ADC module.
 * ADC will stay powered on while the counter is greater than 0.
 * Call this function when done using the ADC.
 */
void adc_power_release(void);

#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2
/**
 * @brief Initialize ADC pad
 * @param adc_unit ADC unit index
 * @param channel ADC channel index
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_gpio_init(adc_unit_t adc_unit, adc_channel_t channel);
#endif //#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2

/*---------------------------------------------------------------
                    ADC Single Read Setting
---------------------------------------------------------------*/
/**
 * @brief Get the GPIO number of a specific ADC1 channel.
 *
 * @param channel Channel to get the GPIO number
 * @param gpio_num output buffer to hold the GPIO number
 *
 * @return
 *   - ESP_OK if success
 *   - ESP_ERR_INVALID_ARG if channel not valid
 */
esp_err_t adc1_pad_get_io_num(adc1_channel_t channel, gpio_num_t *gpio_num);

/**
 * @brief Set the attenuation of a particular channel on ADC1, and configure its associated GPIO pin mux.
 *
 * The default ADC voltage is for attenuation 0 dB and listed in the table below.
 * By setting higher attenuation it is possible to read higher voltages.
 *
 * Due to ADC characteristics, most accurate results are obtained within the "suggested range"
 * shown in the following table.
 *
 *     +----------+-------------+-----------------+
 *     |          | attenuation | suggested range |
 *     |    SoC   |     (dB)    |      (mV)       |
 *     +==========+=============+=================+
 *     |          |       0     |    100 ~  950   |
 *     |          +-------------+-----------------+
 *     |          |       2.5   |    100 ~ 1250   |
 *     |   ESP32  +-------------+-----------------+
 *     |          |       6     |    150 ~ 1750   |
 *     |          +-------------+-----------------+
 *     |          |      11     |    150 ~ 2450   |
 *     +----------+-------------+-----------------+
 *     |          |       0     |      0 ~  750   |
 *     |          +-------------+-----------------+
 *     |          |       2.5   |      0 ~ 1050   |
 *     | ESP32-S2 +-------------+-----------------+
 *     |          |       6     |      0 ~ 1300   |
 *     |          +-------------+-----------------+
 *     |          |      11     |      0 ~ 2500   |
 *     +----------+-------------+-----------------+
 *
 * For maximum accuracy, use the ADC calibration APIs and measure voltages within these recommended ranges.
 *
 * @note For any given channel, this function must be called before the first time ``adc1_get_raw()`` is called for that channel.
 *
 * @note This function can be called multiple times to configure multiple
 *       ADC channels simultaneously. You may call ``adc1_get_raw()`` only after configuring a channel.
 *
 * @param channel ADC1 channel to configure
 * @param atten  Attenuation level
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten);

/**
 * @brief Configure ADC1 capture width, meanwhile enable output invert for ADC1.
 *        The configuration is for all channels of ADC1
 * @param width_bit Bit capture width for ADC1
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc1_config_width(adc_bits_width_t width_bit);

/**
 * @brief Take an ADC1 reading from a single channel.
 * @note ESP32:
 *       When the power switch of SARADC1, SARADC2, HALL sensor and AMP sensor is turned on,
 *       the input of GPIO36 and GPIO39 will be pulled down for about 80ns.
 *       When enabling power for any of these peripherals, ignore input from GPIO36 and GPIO39.
 *       Please refer to section 3.11 of 'ECO_and_Workarounds_for_Bugs_in_ESP32' for the description of this issue.
 *       As a workaround, call adc_power_acquire() in the app. This will result in higher power consumption (by ~1mA),
 *       but will remove the glitches on GPIO36 and GPIO39.
 *
 * @note Call ``adc1_config_width()`` before the first time this
 *       function is called.
 *
 * @note For any given channel, adc1_config_channel_atten(channel)
 *       must be called before the first time this function is called. Configuring
 *       a new channel does not prevent a previously configured channel from being read.
 *
 * @param  channel ADC1 channel to read
 *
 * @return
 *     - -1: Parameter error
 *     -  Other: ADC1 channel reading.
 */
int adc1_get_raw(adc1_channel_t channel);

#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2
/**
 * @brief Set ADC data invert
 * @param adc_unit ADC unit index
 * @param inv_en whether enable data invert
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_set_data_inv(adc_unit_t adc_unit, bool inv_en);

/**
 * @brief Set ADC source clock
 * @param clk_div ADC clock divider, ADC clock is divided from APB clock
 * @return
 *     - ESP_OK success
 */
esp_err_t adc_set_clk_div(uint8_t clk_div);

/**
 * @brief Configure ADC capture width.
 *
 * @param adc_unit ADC unit index
 * @param width_bit Bit capture width for ADC unit.
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_set_data_width(adc_unit_t adc_unit, adc_bits_width_t width_bit);

/**
 * @brief Configure ADC1 to be usable by the ULP
 *
 * This function reconfigures ADC1 to be controlled by the ULP.
 * Effect of this function can be reverted using ``adc1_get_raw()`` function.
 *
 * Note that adc1_config_channel_atten, ``adc1_config_width()`` functions need
 * to be called to configure ADC1 channels, before ADC1 is used by the ULP.
 */
void adc1_ulp_enable(void);
#endif  //#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2

/**
 * @brief Get the GPIO number of a specific ADC2 channel.
 *
 * @param channel Channel to get the GPIO number
 *
 * @param gpio_num output buffer to hold the GPIO number
 *
 * @return
 *   - ESP_OK if success
 *   - ESP_ERR_INVALID_ARG if channel not valid
 */
esp_err_t adc2_pad_get_io_num(adc2_channel_t channel, gpio_num_t *gpio_num);

/**
 * @brief Configure the ADC2 channel, including setting attenuation.
 *
 * The default ADC voltage is for attenuation 0 dB and listed in the table below.
 * By setting higher attenuation it is possible to read higher voltages.
 *
 * Due to ADC characteristics, most accurate results are obtained within the "suggested range"
 * shown in the following table.
 *
 *     +----------+-------------+-----------------+
 *     |          | attenuation | suggested range |
 *     |    SoC   |     (dB)    |      (mV)       |
 *     +==========+=============+=================+
 *     |          |       0     |    100 ~  950   |
 *     |          +-------------+-----------------+
 *     |          |       2.5   |    100 ~ 1250   |
 *     |   ESP32  +-------------+-----------------+
 *     |          |       6     |    150 ~ 1750   |
 *     |          +-------------+-----------------+
 *     |          |      11     |    150 ~ 2450   |
 *     +----------+-------------+-----------------+
 *     |          |       0     |      0 ~  750   |
 *     |          +-------------+-----------------+
 *     |          |       2.5   |      0 ~ 1050   |
 *     | ESP32-S2 +-------------+-----------------+
 *     |          |       6     |      0 ~ 1300   |
 *     |          +-------------+-----------------+
 *     |          |      11     |      0 ~ 2500   |
 *     +----------+-------------+-----------------+
 *
 * For maximum accuracy, use the ADC calibration APIs and measure voltages within these recommended ranges.
 *
 * @note This function also configures the input GPIO pin mux to
 *       connect it to the ADC2 channel. It must be called before calling
 *       ``adc2_get_raw()`` for this channel.
 *
 * @note For any given channel, this function must be called before the first time ``adc2_get_raw()`` is called for that channel.
 *
 * @param channel ADC2 channel to configure
 * @param atten  Attenuation level
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc2_config_channel_atten(adc2_channel_t channel, adc_atten_t atten);

/**
 * @brief Take an ADC2 reading on a single channel
 *
 * @note ESP32:
 *       When the power switch of SARADC1, SARADC2, HALL sensor and AMP sensor is turned on,
 *       the input of GPIO36 and GPIO39 will be pulled down for about 80ns.
 *       When enabling power for any of these peripherals, ignore input from GPIO36 and GPIO39.
 *       Please refer to section 3.11 of 'ECO_and_Workarounds_for_Bugs_in_ESP32' for the description of this issue.
 *       As a workaround, call adc_power_acquire() in the app. This will result in higher power consumption (by ~1mA),
 *       but will remove the glitches on GPIO36 and GPIO39.
 *
 *
 * @note ESP32:
 *       For a given channel, ``adc2_config_channel_atten()``
 *       must be called before the first time this function is called. If Wi-Fi is started via ``esp_wifi_start()``, this
 *       function will always fail with ``ESP_ERR_TIMEOUT``.
 *
 * @note ESP32-S2:
 *       ADC2 support hardware arbiter. The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 *       the low priority controller will read the invalid ADC2 data. Default priority: Wi-Fi > RTC > Digital;
 *
 * @param channel ADC2 channel to read
 * @param width_bit Bit capture width for ADC2
 * @param raw_out the variable to hold the output data.
 *
 * @return
 *     - ESP_OK if success
 *     - ESP_ERR_TIMEOUT ADC2 is being used by other controller and the request timed out.
 *     - ESP_ERR_INVALID_STATE The controller status is invalid. Please try again.
 */
esp_err_t adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width_bit, int *raw_out);

/**
 *  @brief Output ADC1 or ADC2's reference voltage to ``adc2_channe_t``'s IO.
 *
 *  This function routes the internal reference voltage of ADCn to one of
 *  ADC2's channels. This reference voltage can then be manually measured
 *  for calibration purposes.
 *
 *  @note  ESP32 only supports output of ADC2's internal reference voltage.
 *  @param[in]  adc_unit ADC unit index
 *  @param[in]  gpio     GPIO number (Only ADC2's channels IO are supported)
 *
 *  @return
 *                  - ESP_OK: v_ref successfully routed to selected GPIO
 *                  - ESP_ERR_INVALID_ARG: Unsupported GPIO
 */
esp_err_t adc_vref_to_gpio(adc_unit_t adc_unit, gpio_num_t gpio);

/**
 *  @brief Output ADC2 reference voltage to ``adc2_channe_t``'s IO.
 *
 *  This function routes the internal reference voltage of ADCn to one of
 *  ADC2's channels. This reference voltage can then be manually measured
 *  for calibration purposes.
 *
 *  @deprecated Use ``adc_vref_to_gpio`` instead.
 *
 *  @param[in]  gpio    GPIO number (ADC2's channels are supported)
 *
 *  @return
 *                  - ESP_OK: v_ref successfully routed to selected GPIO
 *                  - ESP_ERR_INVALID_ARG: Unsupported GPIO
 */
esp_err_t adc2_vref_to_gpio(gpio_num_t gpio) __attribute__((deprecated));

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
//These APIs are only supported on ESP32 and ESP32-S2. On ESP32-C3 and later chips, please use ``adc_digi_initialize`` and ``adc_digi_deinitialize``
/**
 * @brief ADC digital controller initialization.
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_init(void);

/**
 * @brief ADC digital controller deinitialization.
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_deinit(void);
#endif

/**
 * @brief Setting the digital controller.
 *
 * @param config Pointer to digital controller paramter. Refer to ``adc_digi_config_t``.
 *
 * @return
 *      - ESP_ERR_INVALID_STATE Driver state is invalid.
 *      - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *      - ESP_OK                On success
 */
esp_err_t adc_digi_controller_config(const adc_digi_config_t *config);

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
//This feature is currently supported on ESP32C3, will be supported on other chips soon
/*---------------------------------------------------------------
                    DMA setting
---------------------------------------------------------------*/
/**
 * @brief Initialize the Digital ADC.
 *
 * @param init_config Pointer to Digital ADC initilization config. Refer to ``adc_digi_init_config_t``.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_NOT_FOUND     No free interrupt found with the specified flags
 *         - ESP_ERR_NO_MEM        If out of memory
 *         - ESP_OK                On success
 */
esp_err_t adc_digi_initialize(const adc_digi_init_config_t *init_config);

/**
 * @brief Start the Digital ADC and DMA peripherals. After this, the hardware starts working.
 *
 * @return
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 *         - ESP_OK                On success
 */
esp_err_t adc_digi_start(void);

/**
 * @brief Stop the Digital ADC and DMA peripherals. After this, the hardware stops working.
 *
 * @return
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 *         - ESP_OK                On success
 */
esp_err_t adc_digi_stop(void);

/**
 * @brief Read bytes from Digital ADC through DMA.
 *
 * @param[out] buf                 Buffer to read from ADC.
 * @param[in]  length_max          Expected length of data read from the ADC.
 * @param[out] out_length          Real length of data read from the ADC via this API.
 * @param[in]  timeout_ms          Time to wait for data via this API, in millisecond.
 *
 * @return
 *         - ESP_ERR_INVALID_STATE Driver state is invalid. Usually it means the ADC sampling rate is faster than the task processing rate.
 *         - ESP_ERR_TIMEOUT       Operation timed out
 *         - ESP_OK                On success
 */
esp_err_t adc_digi_read_bytes(uint8_t *buf, uint32_t length_max, uint32_t *out_length, uint32_t timeout_ms);

/**
 * @brief Deinitialize the Digital ADC.
 *
 * @return
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 *         - ESP_OK                On success
 */
esp_err_t adc_digi_deinitialize(void);

#endif //#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2

#ifdef __cplusplus
}
#endif
