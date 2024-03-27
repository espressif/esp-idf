/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "hal/dac_types.h"
#include "hal/dac_ll.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

extern portMUX_TYPE rtc_spinlock;  /*!< Extern global rtc spinlock */

#define DAC_RTC_ENTER_CRITICAL()    portENTER_CRITICAL(&rtc_spinlock)
#define DAC_RTC_EXIT_CRITICAL()     portEXIT_CRITICAL(&rtc_spinlock)

#define DAC_RTC_ENTER_CRITICAL_SAFE()    portENTER_CRITICAL_SAFE(&rtc_spinlock)
#define DAC_RTC_EXIT_CRITICAL_SAFE()     portEXIT_CRITICAL_SAFE(&rtc_spinlock)

#define DAC_NULL_POINTER_CHECK(p)     ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")
#define DAC_NULL_POINTER_CHECK_ISR(p) ESP_RETURN_ON_FALSE_ISR((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")

/**
 * @brief Register dac channel in the driver, in case a same channel is reused by different modes
 *
 * @param[in] chan_id   DAC channel id
 * @param[in] mode_name The const string of mode name
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has been occupied
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Register the channel success
 */
esp_err_t dac_priv_register_channel(dac_channel_t chan_id, const char *mode_name);

/**
 * @brief Deregister dac channel in the driver
 *
 * @param[in] chan_id   DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has been freed
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Deregister the channel success
 */
esp_err_t dac_priv_deregister_channel(dac_channel_t chan_id);

/**
 * @brief Enable the DAC channel and turn on its power
 *
 * @param chan_id       DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has not been registered
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Deregister the channel success
 */
esp_err_t dac_priv_enable_channel(dac_channel_t chan_id);

/**
 * @brief Disable the DAC channel and turn off its power
 *
 * @param chan_id       DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has not been registered
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Deregister the channel success
 */
esp_err_t dac_priv_disable_channel(dac_channel_t chan_id);

#ifdef __cplusplus
}
#endif
