/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#if CONFIG_DAC_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for DAC driver
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "hal/dac_types.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

ESP_LOG_ATTR_TAG(TAG, "dac");

/**
 * DAC driver spinlock
 *
 * - ESP32 / ESP32-S2: use the global rtc_spinlock
 * - ESP32-S31: dedicated DAC register block, use dac_priv_spinlock
 */
#if SOC_IS(ESP32) || SOC_IS(ESP32S2)
extern portMUX_TYPE rtc_spinlock;

#define DAC_ENTER_CRITICAL()         portENTER_CRITICAL(&rtc_spinlock)
#define DAC_EXIT_CRITICAL()          portEXIT_CRITICAL(&rtc_spinlock)
#define DAC_ENTER_CRITICAL_SAFE()    portENTER_CRITICAL_SAFE(&rtc_spinlock)
#define DAC_EXIT_CRITICAL_SAFE()     portEXIT_CRITICAL_SAFE(&rtc_spinlock)
#else
extern portMUX_TYPE dac_priv_spinlock;

#define DAC_ENTER_CRITICAL()         portENTER_CRITICAL(&dac_priv_spinlock)
#define DAC_EXIT_CRITICAL()          portEXIT_CRITICAL(&dac_priv_spinlock)
#define DAC_ENTER_CRITICAL_SAFE()    portENTER_CRITICAL_SAFE(&dac_priv_spinlock)
#define DAC_EXIT_CRITICAL_SAFE()     portEXIT_CRITICAL_SAFE(&dac_priv_spinlock)
#endif // SOC_IS(ESP32) || SOC_IS(ESP32S2)

#define DAC_NULL_POINTER_CHECK(p)     ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")
#define DAC_NULL_POINTER_CHECK_ISR(p) ESP_RETURN_ON_FALSE_ISR((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")

#if CONFIG_DAC_ISR_IRAM_SAFE || CONFIG_DAC_CTRL_FUNC_IN_IRAM
#define DAC_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define DAC_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

/**
 * @brief Register dac channel in the driver, in case a same channel is reused by different modes
 *
 * @param[in] chan_id   DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has been occupied
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Register the channel success
 */
esp_err_t dac_priv_register_channel(dac_channel_t chan_id);

/**
 * @brief Deregister dac channel in the driver
 *
 * @param[in] chan_id   DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has been freed or not disabled
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Deregister the channel success
 */
esp_err_t dac_priv_deregister_channel(dac_channel_t chan_id);

/**
 * @brief Enable the DAC channel and turn on its power
 *
 * @param chan_id       DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has not been registered or already enabled
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Enable the channel success
 */
esp_err_t dac_priv_enable_channel(dac_channel_t chan_id);

/**
 * @brief Disable the DAC channel and turn off its power
 *
 * @param chan_id       DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel is not enabled
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Disable the channel success
 */
esp_err_t dac_priv_disable_channel(dac_channel_t chan_id);

#ifdef __cplusplus
}
#endif
