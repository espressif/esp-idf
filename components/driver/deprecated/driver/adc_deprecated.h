/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*----------------------------------------------------------------------------------
              This file contains Deprecated ADC APIs
-----------------------------------------------------------------------------------*/

#pragma once
#include "esp_err.h"
#include "hal/adc_types.h"
#include "driver/adc_types_deprecated.h"

#ifdef __cplusplus
extern "C" {
#endif


#if CONFIG_IDF_TARGET_ESP32S2
/*---------------------------------------------------------------
                    ESP32S2 Deprecated ADC APIs
---------------------------------------------------------------*/
/**
 * @brief Config ADC module arbiter.
 *        The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 *        the low priority controller will read the invalid ADC2 data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note Default priority: Wi-Fi > RTC > Digital;
 * @note In normal use, there is no need to call this interface to config arbiter.
 *
 * @param adc_unit ADC unit.
 * @param config Refer to `adc_arbiter_t`.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_NOT_SUPPORTED ADC unit not support arbiter.
 */
esp_err_t adc_arbiter_config(adc_unit_t adc_unit, adc_arbiter_t *config) __attribute__((deprecated));

/**
 * @brief Enable interrupt of adc digital controller by bitmask.
 *
 * @param adc_unit ADC unit.
 * @param intr_mask Interrupt bitmask. See ``adc_digi_intr_t``.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_intr_enable(adc_unit_t adc_unit, adc_digi_intr_t intr_mask) __attribute__((deprecated));

/**
 * @brief Disable interrupt of adc digital controller by bitmask.
 *
 * @param adc_unit ADC unit.
 * @param intr_mask Interrupt bitmask. See ``adc_digi_intr_t``.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_intr_disable(adc_unit_t adc_unit, adc_digi_intr_t intr_mask) __attribute__((deprecated));

/**
 * @brief Clear interrupt of adc digital controller by bitmask.
 *
 * @param adc_unit ADC unit.
 * @param intr_mask Interrupt bitmask. See ``adc_digi_intr_t``.
 *
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_intr_clear(adc_unit_t adc_unit, adc_digi_intr_t intr_mask) __attribute__((deprecated));

/**
 * @brief Get interrupt status mask of adc digital controller.
 *
 * @param adc_unit ADC unit.
 * @return
 *     - intr Interrupt bitmask, See ``adc_digi_intr_t``.
 */
uint32_t adc_digi_intr_get_status(adc_unit_t adc_unit) __attribute__((deprecated));

/**
 * @brief Register ADC interrupt handler, the handler is an ISR.
 *        The handler will be attached to the same CPU core that this function is running on.
 *
 * @param fn Interrupt handler function.
 * @param arg Parameter for handler function
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_NOT_FOUND Can not find the interrupt that matches the flags.
 *     - ESP_ERR_INVALID_ARG Function pointer error.
 */
esp_err_t adc_digi_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags) __attribute__((deprecated));

/**
 * @brief Deregister ADC interrupt handler, the handler is an ISR.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG hander error.
 *     - ESP_FAIL ISR not be registered.
 */
esp_err_t adc_digi_isr_deregister(void) __attribute__((deprecated));
#endif  // #if CONFIG_IDF_TARGET_ESP32S2


#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
/*---------------------------------------------------------------
            ESP32, ESP32S2 Deprecated ADC APIs
---------------------------------------------------------------*/
/**
 * @brief ADC digital controller initialization.
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_init(void) __attribute__((deprecated));

/**
 * @brief ADC digital controller deinitialization.
 * @return
 *      - ESP_OK Success
 */
esp_err_t adc_digi_deinit(void) __attribute__((deprecated));
#endif  //#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2


#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
/*---------------------------------------------------------------
            ESP32, ESP32S2, ESP32C3 Deprecated ADC APIs
---------------------------------------------------------------*/
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
esp_err_t adc_digi_controller_config(const adc_digi_config_t *config) __attribute__((deprecated));

/**
 * @brief Initialize ADC pad
 * @param adc_unit ADC unit index
 * @param channel ADC channel index
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_gpio_init(adc_unit_t adc_unit, adc_channel_t channel) __attribute__((deprecated));;
#endif //#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3


#ifdef __cplusplus
}
#endif
