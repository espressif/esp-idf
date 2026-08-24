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
#include "hal/dac_types_private.h"
#include "hal/dac_ll.h"
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

#define DAC_DIV_ROUND(n, d) (((n) + (d) / 2) / (d))
#define DAC_DIV_CEIL(n, d) (((n) + (d) - 1) / (d))
#define DAC_MAX(a, b) ((a) > (b) ? (a) : (b))
#define DAC_CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

/**
 * @brief Register dac channel in the driver, in case a same channel is reused by different modes
 *
 * @param[in] chan_id   DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has been occupied
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Register the channel success
 */
esp_err_t dac_priv_channel_register(dac_channel_t chan_id);

/**
 * @brief Deregister dac channel in the driver
 *
 * @param[in] chan_id   DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has been freed or not disabled
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Deregister the channel success
 */
esp_err_t dac_priv_channel_deregister(dac_channel_t chan_id);

/**
 * @brief Enable the DAC channel and turn on its power
 *
 * @param chan_id       DAC channel id
 * @param source        Channel data source
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has not been registered or already enabled
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Enable the channel success
 */
esp_err_t dac_priv_channel_enable(dac_channel_t chan_id, dac_data_source_t source);

/**
 * @brief Disable the DAC channel and turn off its power
 *
 * @param chan_id       DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel is not enabled
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Disable the channel success
 */
esp_err_t dac_priv_channel_disable(dac_channel_t chan_id);

/**
 * @brief Acquire the shared cosine wave (Sintx/tone) generator (reference counted)
 *
 * @note  The cosine wave generator is a single shared resource: all DAC channels that output a
 *        cosine wave share one generator (and thus one frequency). This claims the generator in tone
 *        mode, programs the wave frequency, and starts the generator on the first acquire. The first
 *        acquirer (or any acquirer passing `force_set_freq`) programs the frequency; a later acquirer
 *        requesting a different frequency without `force_set_freq` is rejected. On targets where the
 *        tone generator and the direct (DC) output share one Sintx generator, a tone acquire is also
 *        rejected while the DC path is in use.
 *
 * @param[in] freq_hz         The cosine wave frequency in Hz
 * @param[in] clk_freq_hz     The clock frequency that drives the generator in Hz
 * @param[in] force_set_freq  Force (re)programming the frequency even if the generator is in use
 * @return
 *      - ESP_ERR_INVALID_STATE The generator is busy on a conflicting mode or frequency
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_sintx_acquire_tone(uint32_t freq_hz, uint32_t clk_freq_hz, bool force_set_freq);

#if SOC_DAC_DC_VIA_SINTX
/**
 * @brief Acquire the shared Sintx generator for the direct (DC) software output (reference counted)
 *
 * @note  On some targets, the direct (DC) software output runs through the same Sintx generator as
 *        the cosine wave output, so the two are mutually exclusive. This claims the generator in DC
 *        mode and, on the first acquire, starts the Sintx timer that pushes the written DC value to
 *        the pad.
 *
 * @return
 *      - ESP_ERR_INVALID_STATE The generator is busy on the cosine path
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_sintx_acquire_dc(void);
#endif // SOC_DAC_DC_VIA_SINTX

/**
 * @brief Release the shared Sintx generator (reference counted)
 *
 * @note  Releases one reference acquired by `dac_priv_sintx_acquire_tone()` or
 *        `dac_priv_sintx_acquire_dc()`. The generator is stopped once the last reference is
 *        released.
 *
 * @return
 *      - ESP_ERR_INVALID_STATE The generator is not in use
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_sintx_release(void);

#ifdef __cplusplus
}
#endif
