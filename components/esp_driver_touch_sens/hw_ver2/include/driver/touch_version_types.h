/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief This file is only applicable to the touch hardware version2
 *        Version 2 includes ESP32-S2 and ESP32-S3
 */

#pragma once

#include "soc/soc_caps.h"
#include "driver/touch_sens_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TOUCH_SHIELD_CHAN_ID        14          /*!< The touch channel that can be used as the shield channel */

/**
 * @brief Helper macro to the default configurations of the touch sensor controller
 *
 * @param[in] sample_cfg_number     The number of the sample configurations, which can only be 1 here because there is only one sample configuration
 * @param[in] sample_cfg_ptr        The pointer to the sample configurations
 */
#define TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(sample_cfg_number, sample_cfg_ptr) { \
    .power_on_wait_us = 256, \
    .meas_interval_us = 32.0, \
    .max_meas_time_us = 0, \
    .sample_cfg_num = sample_cfg_number,  \
    .sample_cfg = sample_cfg_ptr,  \
}

/**
 * @brief Helper macro to the default sample configurations
 * @note  This default configuration uses `sample frequency = clock frequency / 1`
 *
 * @param[in] chg_times     The charge times of the touch channel
 * @param[in] volt_low      The low voltage limit of the touch channel
 * @param[in] volt_high     The high voltage limit of the touch channel
 */
#define TOUCH_SENSOR_V2_DEFAULT_SAMPLE_CONFIG(chg_times, volt_low, volt_high) {  \
    .charge_times = chg_times,  \
    .charge_volt_lim_h = volt_high,  \
    .charge_volt_lim_l = volt_low,  \
    .idle_conn = TOUCH_IDLE_CONN_GND,  \
    .bias_type = TOUCH_BIAS_TYPE_SELF,  \
}

/**
 * @brief Helper macro to the default filter configurations
 *
 */
#define TOUCH_SENSOR_DEFAULT_FILTER_CONFIG() { \
    .benchmark = { \
        .filter_mode = TOUCH_BM_IIR_FILTER_4, \
        .jitter_step = 4, \
        .denoise_lvl = 1, \
    }, \
    .data = { \
        .smooth_filter = TOUCH_SMOOTH_IIR_FILTER_2, \
        .active_hysteresis = 2, \
        .debounce_cnt = 2, \
    }, \
}

/**
 * @brief The data type of the touch channel
 *
 */
typedef enum {
    TOUCH_CHAN_DATA_TYPE_RAW,                   /*!< The raw data of the touch channel */
    TOUCH_CHAN_DATA_TYPE_SMOOTH,                /*!< The smooth data of the touch channel */
    TOUCH_CHAN_DATA_TYPE_BENCHMARK,             /*!< The benchmark of the touch channel */
    TOUCH_CHAN_DATA_TYPE_PROXIMITY,             /*!< The proximity data of the proximity channel */
} touch_chan_data_type_t;

/**
 * @brief Sample configurations of the touch sensor
 *
 */
typedef struct {
    uint32_t                        charge_times;       /*!< The charge and discharge times of this sample configuration, the read data are positive correlation to the charge_times */
    touch_volt_lim_h_t              charge_volt_lim_h;  /*!< The upper voltage limit while charging a touch pad. i.e., the touch controller won't charge the touch pad higher than this high voltage limitation. */
    touch_volt_lim_l_t              charge_volt_lim_l;  /*!< The lower voltage limit while discharging a touch pad. i.e., the touch controller won't discharge the touch pad lower than this low voltage limitation. */
    touch_idle_conn_t               idle_conn;          /*!< The connection of the idle touch channels.
                                                         *   The idle touch channel is a channel which is enabled and power-on but not under measuring.
                                                         */
    touch_bias_type_t               bias_type;          /*!< The type of the touch sensor bias. Which affects the charge/discharge stability and power consumption */
} touch_sensor_sample_config_t;

/**
 * @brief Configurations of the touch sensor controller
 *
 */
typedef struct {
    uint32_t                        power_on_wait_us;   /*!< The waiting time between the channels power on and able to measure, to ensure the data stability */
    float                           meas_interval_us;   /*!< Measurement interval of each channels */
    uint32_t                        max_meas_time_us;   /*!< The maximum time of measuring one channel, if the time exceeds this value, the timeout interrupt will be triggered.
                                                         *   Set to '0' to ignore the measurement time limitation, otherwise please set a proper time considering the configurations
                                                         *   of this sample configurations below.
                                                         */
    /* Touch sensor sample configuration */
    uint32_t                        sample_cfg_num;     /*!< The sample configuration number that used for sampling, CANNOT exceed TOUCH_SAMPLE_CFG_NUM */
    touch_sensor_sample_config_t    *sample_cfg;        /*!< The array of this sample configuration configurations, the length should be specified in `touch_sensor_config_t::sample_cfg_num` */
} touch_sensor_config_t;

/**
 * @brief Configurations of the touch sensor channel
 *
 */
typedef struct {
    uint32_t                        active_thresh[TOUCH_SAMPLE_CFG_NUM];      /*!< The relative active threshold of each sample configuration,
                                                                               *   while the touch channel smooth value minus benchmark value exceed this threshold,
                                                                               *   will be regarded as activated
                                                                               */
    touch_charge_speed_t            charge_speed;       /*!< The speed of charging and discharging the touch pad, the higher the speed, the faster charging and discharging */
    touch_init_charge_volt_t        init_charge_volt;   /*!< The initial voltage before charging/discharging a touch pad */
} touch_channel_config_t;

/**
 * @brief Configurations of the touch sensor filter
 *
 */
typedef struct {
    /**
     * @brief Benchmark configuration
     */
    struct {
        touch_benchmark_filter_mode_t   filter_mode;        /*!< Benchmark filter mode. IIR filter and Jitter filter can be selected,
                                                             *   TOUCH_BM_IIR_FILTER_16 is recommended
                                                             */
        uint32_t                        jitter_step;        /*!< Jitter filter step size, only takes effect when the `filter_mode` is TOUCH_BM_JITTER_FILTER. Range: [0 ~ 15] */
        int                             denoise_lvl;        /*!< The denoise level, which determines the noise bouncing range that won't trigger benchmark update.
                                                             *   Range: [0 ~ 4]. The greater the denoise_lvl is, more noise resistance will be. Specially, `0` stands for no denoise
                                                             *   Typically, recommend to set this field to 1.
                                                             */
    } benchmark;                                            /*!< Benchmark filter */
    /**
     * @brief Data configuration
     */
    struct {
        touch_smooth_filter_mode_t      smooth_filter;      /*!< Smooth data IIR filter mode */
        uint32_t                        active_hysteresis;  /*!< The hysteresis threshold to judge whether the touch channel is active
                                                             *   If the channel data exceed the 'touch_channel_config_t::active_thresh + active_hysteresis'
                                                             *   The channel will be activated. If the channel data is below to
                                                             *   'touch_channel_config_t::active_thresh - active_hysteresis' the channel will be inactivated.
                                                             */
        uint32_t                        debounce_cnt;       /*!< The debounce count of the touch channel.
                                                             *   Only when the channel data exceed the `touch_channel_config_t::active_thresh + active_hysteresis` for `debounce_cnt` times
                                                             *   The channel will be activated. And only if the channel data is below to the `touch_channel_config_t::active_thresh - active_hysteresis`
                                                             *   for `debounce_cnt` times, the channel will be inactivated.
                                                             *   (The unit of `debounce_cnt` is the tick of the slow clock source)
                                                             */
    } data;                                                 /*!< Channel data filter */
} touch_sensor_filter_config_t;

/**
 * @brief Touch sensor configuration during the deep sleep
 * @note  Currently it is the same as the normal controller configuration.
 *        The deep sleep configuration only takes effect when the chip entered sleep,
 *        so that to update a more power efficient configuration.
 *
 */
typedef touch_sensor_config_t touch_sensor_config_dslp_t;

/**
 * @brief Helper macro to the default light sleep wake-up configurations
 * @note  RTC_PERIPH will keep power on during the light sleep.
 *        Any enabled touch channel can wake-up the chip from light sleep.
 */
#define TOUCH_SENSOR_DEFAULT_LSLP_CONFIG() {  \
    .slp_wakeup_lvl = TOUCH_LIGHT_SLEEP_WAKEUP,  \
}

/**
 * @brief Helper macro to the default deep sleep wake-up configurations
 * @note  RTC_PERIPH will keep power on during the deep sleep.
 *        Any enabled touch channel can wake-up the chip from deep sleep.
 */
#define TOUCH_SENSOR_DEFAULT_DSLP_CONFIG() {  \
    .slp_wakeup_lvl = TOUCH_DEEP_SLEEP_WAKEUP,  \
}

/**
 * @brief Helper macro to the default deep sleep wake-up configurations
 *        (allow RTC_PERIPH power down in deep sleep)
 * @note  RTC_PERIPH might be powered down during the deep sleep.
 *        If the RTC_PERIPH is powered down, only the specified sleep channel can wake-up the chip from deep sleep.
 * @param[in] dslp_chan  The specified sleep channel that can wake-up the chip from deep sleep while RTC_PERIPH is powered down.
 * @param[in] ...        The threshold of each sample configuration of the specified sleep channel.
 */
#define TOUCH_SENSOR_DEFAULT_DSLP_PD_CONFIG(dslp_chan, ...) {  \
    .slp_wakeup_lvl = TOUCH_DEEP_SLEEP_WAKEUP,  \
    .deep_slp_allow_pd = true,  \
    .deep_slp_chan = dslp_chan,  \
    .deep_slp_thresh = {__VA_ARGS__},  \
}

/**
 * @brief Configuration of the touch sensor sleep function
 *
 */
typedef struct {
    touch_sleep_wakeup_level_t      slp_wakeup_lvl;     /*!< The sleep level that can be woke up by touch sensor. */
    bool                            deep_slp_allow_pd;  /*!< Whether allow RTC power down during the deep sleep.
                                                         *   Only effective when the `touch_sleep_config_t::slp_wakeup_lvl` is `TOUCH_DEEP_SLEEP_WAKEUP`.
                                                         *     - If true, the RTC power domain will be powered down during the deep sleep.
                                                         *     - If false, the RTC power domain will keep power on during the deep sleep.
                                                         */
    touch_channel_handle_t          deep_slp_chan;      /*!< The touch channel handle that supposed to work in the deep sleep even RTC domain is powered down.
                                                         *   Only effective when the `touch_sleep_config_t::slp_wakeup_lvl` is `TOUCH_DEEP_SLEEP_WAKEUP`
                                                         *      - Not NULL: Only this channel can wake up the chip from deep sleep.
                                                         *      - NULL: `deep_slp_allow_pd` must be false, and any enabled channels can wake up the chip from deep sleep.
                                                         */
    uint32_t                        deep_slp_thresh[TOUCH_SAMPLE_CFG_NUM];    /*!< The active threshold of the deep sleep channel during deep sleep,
                                                         *   while the sleep channel exceed this threshold, it will be regarded as activated
                                                         *   Only effective when the `touch_sleep_config_t::slp_wakeup_lvl` is `TOUCH_DEEP_SLEEP_WAKEUP`
                                                         */
    touch_sensor_config_dslp_t      *deep_slp_sens_cfg; /*!< Specify the touch sensor configuration during the deep sleep.
                                                         *   Note that these configurations will no take effect immediately,
                                                         *   they will be set automatically while the chip prepare to enter sleep.
                                                         *   Set NULL to not change the configurations before entering sleep.
                                                         *   The sleep configuration mainly aims at lower down the charging and measuring times,
                                                         *   so that to save power consumption during the sleep.
                                                         *   Only effective when the `touch_sleep_config_t::slp_wakeup_lvl` is `TOUCH_DEEP_SLEEP_WAKEUP`
                                                         */
} touch_sleep_config_t;

/**
 * @brief Configuration of the touch sensor waterproof function
 *
 */
typedef struct {
    touch_channel_handle_t          guard_chan;         /*!< The guard channel of that used for immersion detect. Set NULL if you don't need the guard channel.
                                                         *   Typically, the guard channel is a ring that surrounds the touch panels,
                                                         *   it is used to detect the large area that covered by water.
                                                         *   While large area of water covers the touch panel, the guard channel will be activated.
                                                         */
    touch_channel_handle_t          shield_chan;        /*!< The shield channel that used for water droplets shield, can't be NULL.
                                                         *   The shield channel can only be the No.14 channel on touch version 2.
                                                         *   Typically, the shield channel uses grid layout which covers the touch area,
                                                         *   it is used to shield the influence of water droplets covering both the touch panel and the shield channel.
                                                         *   The shield channel will be paralleled to the current measuring channel (except the guard channel) to reduce the influence of water droplets.
                                                         */
    uint32_t                        shield_drv;         /*!< The shield channel driver, which controls the drive capability of shield channel, range: 0 ~ 7
                                                         *   The larger the parasitic capacitance on the shielding channel, the higher the drive capability needs to be set.
                                                         */
    struct {
        uint32_t                    immersion_proof: 1; /*!< Enable to protect the touch sensor pad when immersion detected.
                                                         *   It will temporary disable the touch scanning if the guard channel triggered, and enable again if guard channel released.
                                                         *   So that to avoid the fake touch when the touch panel is immersed in water.
                                                         */
    } flags;                                            /*!< Flags of the water proof function */
} touch_waterproof_config_t;

/**
 * @brief Configuration of the touch sensor proximity function
 *
 */
typedef struct {
    touch_channel_handle_t          proximity_chan[TOUCH_PROXIMITY_CHAN_NUM]; /*!< The touch channel handles that will be configured as proximity sensing channels */
    uint32_t                        scan_times;                               /*!< The total scan times of EACH sample configuration, all sample configurations share a same `scan_times`.
                                                                               *   The measurement result of each scanning will be accumulated together to get the final result.
                                                                               */
} touch_proximity_config_t;

/**
 * @brief Configuration of denoise channel
 *
 */
typedef struct {
    touch_charge_speed_t            charge_speed;       /*!< The speed of charging and discharging the denoise touch channel, the higher the speed, the faster charging and discharging */
    touch_init_charge_volt_t        init_charge_volt;   /*!< The initial voltage before starting charging/discharging the denoise channel */
    touch_denoise_chan_cap_t        ref_cap;            /*!< The reference capacitance of the denoise channel. */
    touch_denoise_chan_resolution_t resolution;         /*!< The noise suppression resolution of the denoise channel.
                                                         *   The higher the resolution, the better the suppression effect,
                                                         *   but at the same time, the attenuation of other touch channel sampling values also increases.
                                                         */
} touch_denoise_chan_config_t;

/**
 * @brief Base event structure used in touch event queue
 */
typedef struct {
    touch_channel_handle_t          chan;           /*!< the current triggered touch channel handle */
    int                             chan_id;        /*!< the current triggered touch channel number */
    uint32_t                        status_mask;    /*!< the current channel triggered status.
                                                     *   For the bits in the status mask,
                                                     *   if the bit is set, the corresponding channel is active
                                                     *   if the bit is cleared, the corresponding channel is inactive
                                                     */
} touch_base_event_data_t;

/**
 * @brief Measure done event data
 * @note  Currently same as base event data
 *
 */
typedef touch_base_event_data_t touch_meas_done_event_data_t;

/**
 * @brief Scan done event data
 * @note  Currently same as base event data
 *
 */
typedef touch_base_event_data_t touch_scan_done_event_data_t;

/**
 * @brief Active event data
 * @note  Currently same as base event data
 *
 */
typedef touch_base_event_data_t touch_active_event_data_t;

/**
 * @brief Inactive event data
 * @note  Currently same as base event data
 *
 */
typedef touch_base_event_data_t touch_inactive_event_data_t;

/**
 * @brief Proximity sensing measure done event data
 * @note  Currently same as base event data
 *
 */
typedef touch_base_event_data_t touch_prox_done_event_data_t;

/**
 * @brief Timeout event data
 * @note  Currently same as base event data
 *
 */
typedef touch_base_event_data_t touch_timeout_event_data_t;

/**
 * @brief Touch sensor callbacks
 * @note  Set NULL for the used callbacks.
 *
 */
typedef struct {
    /**
     * @brief Touch sensor on active event callback.
     *        Callback when any touch channel is activated.
     * @param[in] sens_handle    Touch sensor controller handle, created from `touch_sensor_new_controller()`
     * @param[in] event     Touch sensor active event data
     * @param[in] user_ctx  User registered context, passed from `touch_sensor_register_callbacks()`
     *
     * @return Whether a high priority task has been waken up by this callback function
     */
    bool (*on_active)(touch_sensor_handle_t sens_handle, const touch_active_event_data_t *event, void *user_ctx);
    /**
     * @brief Touch sensor on inactive event callback.
     *        Callback when any touch channel is inactivated.
     * @param[in] sens_handle    Touch sensor controller handle, created from `touch_sensor_new_controller()`
     * @param[in] event     Touch sensor inactive event data
     * @param[in] user_ctx  User registered context, passed from `touch_sensor_register_callbacks()`
     *
     * @return Whether a high priority task has been waken up by this callback function
     */
    bool (*on_inactive)(touch_sensor_handle_t sens_handle, const touch_inactive_event_data_t *event, void *user_ctx);
    /**
     * @brief Touch sensor on measure done event callback.
     *        Callback when the measurement of all the sample configurations on the current touch channel is done.
     * @param[in] sens_handle    Touch sensor controller handle, created from `touch_sensor_new_controller()`
     * @param[in] event     Touch sensor measure done event data
     * @param[in] user_ctx  User registered context, passed from `touch_sensor_register_callbacks()`
     *
     * @return Whether a high priority task has been waken up by this callback function
     */
    bool (*on_measure_done)(touch_sensor_handle_t sens_handle, const touch_meas_done_event_data_t *event, void *user_ctx);
    /**
     * @brief Touch sensor on scan done event callback.
     *        Callback when finished scanning all the registered touch channels.
     * @param[in] sens_handle    Touch sensor controller handle, created from `touch_sensor_new_controller()`
     * @param[in] event     Touch sensor scan done event data
     * @param[in] user_ctx  User registered context, passed from `touch_sensor_register_callbacks()`
     *
     * @return Whether a high priority task has been waken up by this callback function
     */
    bool (*on_scan_done)(touch_sensor_handle_t sens_handle, const touch_scan_done_event_data_t *event, void *user_ctx);
    /**
     * @brief Touch sensor on measurement timeout event callback.
     *        Callback when measure the current touch channel timeout.
     * @param[in] sens_handle    Touch sensor controller handle, created from `touch_sensor_new_controller()`
     * @param[in] event     Touch sensor timeout event data
     * @param[in] user_ctx  User registered context, passed from `touch_sensor_register_callbacks()`
     *
     * @return Whether a high priority task has been waken up by this callback function
     */
    bool (*on_timeout)(touch_sensor_handle_t sens_handle, const touch_timeout_event_data_t *event, void *user_ctx);
    /**
     * @brief Touch sensor on proximity sensing measurement done event callback.
     *        Callback when proximity sensing measurement of the current channel is done.
     * @param[in] sens_handle    Touch sensor controller handle, created from `touch_sensor_new_controller()`
     * @param[in] event     Touch sensor proximity sensing measure done event data
     * @param[in] user_ctx  User registered context, passed from `touch_sensor_register_callbacks()`
     *
     * @return Whether a high priority task has been waken up by this callback function
     */
    bool (*on_proximity_meas_done)(touch_sensor_handle_t sens_handle, const touch_prox_done_event_data_t *event, void *user_ctx);
} touch_event_callbacks_t;

/**
 * @brief Touch sensor benchmark configurations, to set or reset the benchmark of the channel
 *
 */
typedef struct {
    bool                            do_reset;                /*!< Whether to reset the benchmark to the channel's latest smooth data */
} touch_chan_benchmark_config_t;

#ifdef __cplusplus
}
#endif
