/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "driver/touch_sens_types.h"
#include "driver/touch_version_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Allocate a new touch sensor controller
 * @note  The touch sensor driver will be in INIT state after this function is called successfully.
 *
 * @param[in]  sens_cfg         Touch sensor controller configurations
 * @param[out] ret_sens_handle  The return handle of the touch controller instance
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_NO_MEM        No memory for the touch sensor controller
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE The touch sensor controller is already allocated
 */
esp_err_t touch_sensor_new_controller(const touch_sensor_config_t *sens_cfg, touch_sensor_handle_t *ret_sens_handle);

/**
 * @brief Delete the touch sensor controller
 * @note  This function can be called when the touch sensor controller is NOT enabled (i.e. INIT state).
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE Controller not disabled or still some touch channels not deleted
 */
esp_err_t touch_sensor_del_controller(touch_sensor_handle_t sens_handle);

/**
 * @brief Allocate a new touch channel from the touch sensor controller
 * @note  This function can be called when the touch sensor controller is NOT enabled (i.e. INIT state).
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @param[in]  chan_id          Touch channel index
 * @param[in]  chan_cfg         Touch channel configurations
 * @param[out] ret_chan_handle  The return handle of the touch channel instance
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_NO_MEM        No memory for the touch sensor channel
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE The touch sensor controller has not disabled or this channel has been allocated
 */
esp_err_t touch_sensor_new_channel(touch_sensor_handle_t sens_handle, int chan_id,
                                   const touch_channel_config_t *chan_cfg,
                                   touch_channel_handle_t *ret_chan_handle);

/**
 * @brief Delete the touch channel
 * @note  This function can be called when the touch sensor controller is NOT enabled (i.e. INIT state).
 *
 * @param[in]  chan_handle      Touch channel handle
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE The touch sensor controller has not disabled
 */
esp_err_t touch_sensor_del_channel(touch_channel_handle_t chan_handle);

/**
 * @brief Re-configure the touch sensor controller
 * @note  This function can be called when the touch sensor controller is NOT enabled (i.e. INIT state).
 * @note  The re-configuration only applies to the touch controller,
 *        so it requires the controller handle that allocated from ``touch_sensor_new_controller``,
 *        meanwhile, it won't affect the touch channels, no matter the channels are allocated or not.
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @param[in]  sens_cfg         Touch sensor controller configurations
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE The touch sensor controller has not disabled
 */
esp_err_t touch_sensor_reconfig_controller(touch_sensor_handle_t sens_handle, const touch_sensor_config_t *sens_cfg);

/**
 * @brief Re-configure the touch channel
 * @note  This function can be called when the touch sensor controller is NOT enabled (i.e. INIT state).
 * @note  The re-configuration only applies to a particular touch channel,
 *        so it requires the channel handle that allocated from ``touch_sensor_new_channel``
 *
 * @param[in]  chan_handle      Touch channel handle
 * @param[in]  chan_cfg         Touch channel configurations
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE The touch sensor controller has not disabled
 */
esp_err_t touch_sensor_reconfig_channel(touch_channel_handle_t chan_handle, const touch_channel_config_t *chan_cfg);

/**
 * @brief Configure the touch sensor filter
 * @note  This function is allowed to be called after the touch sensor is enabled
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @param[in]  filter_cfg       Filter configurations, set NULL to disable the filter
 * @return
 *      - ESP_OK:                   Configure the filter success
 *      - ESP_ERR_INVALID_ARG:      The sensor handle is NULL
 */
esp_err_t touch_sensor_config_filter(touch_sensor_handle_t sens_handle, const touch_sensor_filter_config_t *filter_cfg);

/**
 * @brief Enable the touch sensor controller
 * @note  This function can be called when the touch sensor controller is NOT enabled (i.e. INIT state).
 *        And the touch sensor driver will be in ENABLED state after this function is called successfully.
 * @note  Enable the touch sensor will power on the registered touch channels
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE The touch sensor controller has already enabled
 */
esp_err_t touch_sensor_enable(touch_sensor_handle_t sens_handle);

/**
 * @brief Disable the touch sensor controller
 * @note  This function can only be called after the touch sensor controller is enabled (i.e. ENABLED state).
 *        And the touch sensor driver will be in INIT state after this function is called successfully.
 * @note  Disable the touch sensor will power off the registered touch channels
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE The touch sensor controller has already disabled
 */
esp_err_t touch_sensor_disable(touch_sensor_handle_t sens_handle);

/**
 * @brief Start the scanning of the registered touch channels continuously
 * @note  This function can only be called after the touch sensor controller is enabled (i.e. ENABLED state).
 *        And the touch sensor driver will be in SCANNING state after this function is called successfully.
 *        And it can also be called in ISR/callback context.
 * @note  The hardware FSM (Finite-State Machine) of touch sensor will be driven by
 *        its hardware timer continuously and repeatedly.
 *        i.e., the registered channels will be scanned one by one repeatedly.
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE The touch sensor controller is not enabled or the continuous scanning has started
 */
esp_err_t touch_sensor_start_continuous_scanning(touch_sensor_handle_t sens_handle);

/**
 * @brief Stop the continuous scanning of the registered touch channels immediately
 * @note  This function can only be called after the continuous scanning started  (i.e. SCANNING state).
 *        And the touch sensor driver will be in ENABLED state after this function is called successfully.
 *        And it can also be called in ISR/callback context.
 * @note  Stop the hardware timer and reset the FSM (Finite-State Machine) of the touch sensor controller
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 *      - ESP_ERR_INVALID_STATE The continuous scanning has stopped
 */
esp_err_t touch_sensor_stop_continuous_scanning(touch_sensor_handle_t sens_handle);

/**
 * @brief Trigger an oneshot scanning for all the registered channels
 * @note  This function can only be called after the touch sensor controller is enabled (i.e. ENABLED state).
 *        And the touch sensor driver will be in SCANNING state after this function is called successfully,
 *        and then switch back to ENABLED state after the scanning is done or timeout.
 * @note  The block time of this function depends on various factors,
 *        In common practice, recommend to set the timeout to several seconds or wait forever,
 *        because oneshot scanning can't last for so long.
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @param[in]  timeout_ms       Set a positive value or zero means scan timeout in milliseconds
 *                              Set a negative value means wait forever until finished scanning
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_TIMEOUT       Timeout to finish the oneshot scanning
 *      - ESP_ERR_INVALID_ARG   Invalid argument
 *      - ESP_ERR_INVALID_STATE The touch sensor controller is not enabled or the continuous scanning has started
 */
esp_err_t touch_sensor_trigger_oneshot_scanning(touch_sensor_handle_t sens_handle, int timeout_ms);

/**
 * @brief Register the touch sensor callbacks
 * @note  This function can be called when the touch sensor controller is NOT enabled (i.e. INIT state).
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @param[in]  callbacks        Callback functions
 * @param[in]  user_ctx         User context that will be passed to the callback functions
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 *      - ESP_ERR_INVALID_STATE Touch sensor controller has not disabled
 */
esp_err_t touch_sensor_register_callbacks(touch_sensor_handle_t sens_handle, const touch_event_callbacks_t *callbacks, void *user_ctx);

/**
 * @brief Confiture the touch sensor benchmark for all the registered channels
 * @note  This function can be called no matter the touch sensor controller is enabled or not (i.e. ENABLED or SCANNING state).
 *        And it can also be called in ISR/callback context.
 *
 * @param[in]  chan_handle      Touch channel handle
 * @param[in]  benchmark_cfg    The benchmark configurations
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 */
esp_err_t touch_channel_config_benchmark(touch_channel_handle_t chan_handle, const touch_chan_benchmark_config_t *benchmark_cfg);

/**
 * @brief Read the touch channel data according to the types
 * @note  This function can be called no matter the touch sensor controller is enabled or not (i.e. ENABLED or SCANNING state).
 *        And it can also be called in ISR/callback context.
 * @note  Specially, `TOUCH_CHAN_DATA_TYPE_PROXIMITY` data type will be read from the cached data in the driver,
 *        because the proximity data need to be accumulated in several scan times that specified by `touch_proximity_config_t::scan_times`.
 *        For other data types, the data are read from the hardware registers directly (not cached in the driver).
 *        The channel data in the register will be updated once the measurement of this channels is done,
 *        And keep locked until the next measurement is done.
 *
 * @param[in]  chan_handle      Touch channel handle
 * @param[in]  type             Specify the data type to read
 * @param[out] data             The data array pointer. If the target supports multiple sample configurations (SOC_TOUCH_SAMPLE_CFG_NUM), the array length should be equal to
 *                              the frequency hopping number that specified in `touch_sensor_config_t::sample_cfg_num`, otherwise the array length should be 1.
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid argument or NULL pointer
 */
esp_err_t touch_channel_read_data(touch_channel_handle_t chan_handle, touch_chan_data_type_t type, uint32_t *data);

#if SOC_TOUCH_SUPPORT_WATERPROOF
/**
 * @brief Configure the touch sensor water proof channels
 * @note  Once waterproof is enabled, the other touch channels won't be updated unless the shield channels is activated.
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @param[in]  wp_cfg           Water proof channel configurations, set NULL to disable the water proof function
 * @return
 *      - ESP_OK:                   Configure the water proof success
 *      - ESP_ERR_INVALID_ARG:      The sensor handle is NULL
 *      - ESP_ERR_INVALID_STATE:    The touch sensor is enabled
 */
esp_err_t touch_sensor_config_waterproof(touch_sensor_handle_t sens_handle, const touch_waterproof_config_t *wp_cfg);
#endif

#if SOC_TOUCH_SUPPORT_PROX_SENSING
/**
 * @brief Configure the touch sensor proximity sensing channels
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @param[in]  prox_cfg         Proximity channels configurations, set NULL to disable the proximity sensing
 * @return
 *      - ESP_OK:                   Configure the proximity channel success
 *      - ESP_ERR_INVALID_ARG:      The sensor handle is NULL
 *      - ESP_ERR_INVALID_STATE:    The touch sensor is enabled
 */
esp_err_t touch_sensor_config_proximity_sensing(touch_sensor_handle_t sens_handle, const touch_proximity_config_t *prox_cfg);
#endif

#if SOC_TOUCH_SUPPORT_SLEEP_WAKEUP
/**
 * @brief Configure the touch sensor to wake-up the chip from sleep
 * @note  Call this function to enable/disable the touch sensor wake-up the chip from deep/light sleep
 *        To only enable the touch sensor wake-up the chip from light sleep, set the `touch_sleep_config_t::deep_slp_chan` to NULL.
 *        During the light sleep, all enabled touch channels will keep working, and any one of them can wake-up the chip from light sleep.
 *        To enable the touch sensor wake-up the chip from both light and deep sleep, set the `touch_sleep_config_t::deep_slp_chan` to an enabled channel.
 *        During the deep sleep, only this specified channels can work and wake-up the chip from the deep sleep,
 *        and other enabled channels can only wake-up the chip from light sleep.
 *
 * @param[in]  sens_handle      Touch sensor controller handle
 * @param[in]  sleep_cfg        Sleep wake-up configurations, set NULL to disable the touch sensor wake-up the chip from sleep
 * @return
 *      - ESP_OK:                   Configure the sleep channel success
 *      - ESP_ERR_INVALID_ARG:      The sensor handle is NULL
 *      - ESP_ERR_INVALID_STATE:    The touch sensor is enabled
 */
esp_err_t touch_sensor_config_sleep_wakeup(touch_sensor_handle_t sens_handle, const touch_sleep_config_t *sleep_cfg);
#endif

#ifdef __cplusplus
}
#endif
