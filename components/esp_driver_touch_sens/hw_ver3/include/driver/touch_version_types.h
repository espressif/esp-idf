/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief This file is only applicable to the touch hardware version3
 *        Version 3 includes ESP32-P4
 */

#pragma once

#include "soc/soc_caps.h"
#include "driver/touch_sens_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TOUCH_MIN_CHAN_ID           0           /*!< The minimum available channel id of the touch pad */
#define TOUCH_MAX_CHAN_ID           13          /*!< The maximum available channel id of the touch pad */

/**
 * @brief Helper macro to the default configurations of the touch sensor controller
 *
 */
#define TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(sample_cfg_number, sample_cfg_array) { \
    .power_on_wait_us = 256, \
    .meas_interval_us = 32.0, \
    .max_meas_time_us = 0, \
    .output_mode = TOUCH_PAD_OUT_AS_CLOCK, \
    .sample_cfg_num = sample_cfg_number, \
    .sample_cfg = sample_cfg_array, \
}

/**
 * @brief Helper macro to the default sample configurations
 * @note  This default configuration uses `sample frequency = clock frequency / 1`
 *
 */
#define TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG(_div_num, coarse_freq_tune, fine_freq_tune) { \
    .div_num = _div_num, \
    .charge_times = 500, \
    .rc_filter_res = 1, \
    .rc_filter_cap = 1, \
    .low_drv = fine_freq_tune, \
    .high_drv = coarse_freq_tune, \
    .bias_volt = 5, \
    .bypass_shield_output = false, \
}

#define TOUCH_SENSOR_DEFAULT_FILTER_CONFIG() { \
    .benchmark = { \
        .filter_mode = TOUCH_BM_IIR_FILTER_4, \
        .jitter_step = 4, \
        .denoise_lvl = 1, \
    }, \
    .data = { \
        .smooth_filter = TOUCH_SMOOTH_IIR_FILTER_2, \
        .active_hysteresis = 2, \
    }, \
}

/**
 * @brief The data type of the touch channel
 *
 */
typedef enum {
    TOUCH_CHAN_DATA_TYPE_SMOOTH,                /*!< The smooth data of the touch channel */
    TOUCH_CHAN_DATA_TYPE_BENCHMARK,             /*!< The benchmark of the touch channel */
    TOUCH_CHAN_DATA_TYPE_PROXIMITY,             /*!< The proximity data of the proximity channel */
} touch_chan_data_type_t;

/**
 * @brief The chip sleep level that allows the touch sensor to wake-up
 *
 */
typedef enum {
    TOUCH_LIGHT_SLEEP_WAKEUP,                /*!< Only enable the touch sensor to wake up the chip from light sleep */
    TOUCH_DEEP_SLEEP_WAKEUP,                 /*!< Enable the touch sensor to wake up the chip from deep sleep or light sleep */
} touch_sleep_wakeup_level_t;

/**
 * @brief Touch sensor shield channel drive capability level
 *
 */
typedef enum {
    TOUCH_SHIELD_CAP_40PF,                  /*!< The max equivalent capacitance in shield channel is 40pf */
    TOUCH_SHIELD_CAP_80PF,                  /*!< The max equivalent capacitance in shield channel is 80pf */
    TOUCH_SHIELD_CAP_120PF,                 /*!< The max equivalent capacitance in shield channel is 120pf */
    TOUCH_SHIELD_CAP_160PF,                 /*!< The max equivalent capacitance in shield channel is 160pf */
    TOUCH_SHIELD_CAP_200PF,                 /*!< The max equivalent capacitance in shield channel is 200pf */
    TOUCH_SHIELD_CAP_240PF,                 /*!< The max equivalent capacitance in shield channel is 240pf */
    TOUCH_SHIELD_CAP_280PF,                 /*!< The max equivalent capacitance in shield channel is 280pf */
    TOUCH_SHIELD_CAP_320PF,                 /*!< The max equivalent capacitance in shield channel is 320pf */
} touch_chan_shield_cap_t;

/**
 * @brief Touch channel Infinite Impulse Response (IIR) filter or Jitter filter for benchmark
 * @note Recommended filter coefficient selection is `IIR_16`.
 */
typedef enum {
    TOUCH_BM_IIR_FILTER_4,                  /*!< IIR Filter for benchmark, 1/4 raw_value + 3/4 benchmark */
    TOUCH_BM_IIR_FILTER_8,                  /*!< IIR Filter for benchmark, 1/8 raw_value + 7/8 benchmark */
    TOUCH_BM_IIR_FILTER_16,                 /*!< IIR Filter for benchmark, 1/16 raw_value + 15/16 benchmark (typical) */
    TOUCH_BM_IIR_FILTER_32,                 /*!< IIR Filter for benchmark, 1/32 raw_value + 31/32 benchmark */
    TOUCH_BM_IIR_FILTER_64,                 /*!< IIR Filter for benchmark, 1/64 raw_value + 63/64 benchmark */
    TOUCH_BM_IIR_FILTER_128,                /*!< IIR Filter for benchmark, 1/128 raw_value + 127/128 benchmark */
    TOUCH_BM_JITTER_FILTER,                 /*!< Jitter Filter for benchmark, raw value +/- jitter_step */
} touch_benchmark_filter_mode_t;

/**
 * @brief Touch channel Infinite Impulse Response (IIR) filter for smooth data
 *
 */
typedef enum {
    TOUCH_SMOOTH_NO_FILTER,                 /*!< No filter adopted for smooth data, smooth data equals raw data */
    TOUCH_SMOOTH_IIR_FILTER_2,              /*!< IIR filter adopted for smooth data, smooth data equals 1/2 raw data + 1/2 last smooth data (typical) */
    TOUCH_SMOOTH_IIR_FILTER_4,              /*!< IIR filter adopted for smooth data, smooth data equals 1/4 raw data + 3/4 last smooth data */
    TOUCH_SMOOTH_IIR_FILTER_8,              /*!< IIR filter adopted for smooth data, smooth data equals 1/8 raw data + 7/8 last smooth data */
} touch_smooth_filter_mode_t;

/**
 * @brief Interrupt events
 *
 */
typedef enum {
    TOUCH_INTR_EVENT_ACTIVE,                /*!< Touch channel active event */
    TOUCH_INTR_EVENT_INACTIVE,              /*!< Touch channel inactive event */
    TOUCH_INTR_EVENT_MEASURE_DONE,          /*!< Touch channel measure done event */
    TOUCH_INTR_EVENT_SCAN_DONE,             /*!< All touch channels scan done event */
    TOUCH_INTR_EVENT_TIMEOUT,               /*!< Touch channel measurement timeout event */
    TOUCH_INTR_EVENT_PROXIMITY_DONE,        /*!< Proximity channel measurement done event */
} touch_intr_event_t;

/**
 * @brief Sample configurations of the touch sensor
 *
 */
typedef struct {
    uint32_t                        div_num;            /*!< Division of the touch output pulse, `touch_out_pulse / div_num = charge_times` */
    uint32_t                        charge_times;       /*!< The charge and discharge times of this sample configuration, the read data are positive correlation to the charge_times */
    uint8_t                         rc_filter_res;      /*!< The resistance of the RC filter of this sample configuration, range [0, 3], while 0 = 0K, 1 = 1.5K, 2 = 3K, 3 = 4.5K */
    uint8_t                         rc_filter_cap;      /*!< The capacitance of the RC filter of this sample configuration, range [0, 127], while 0 = 0pF, 1 = 20fF, ..., 127 = 2.54pF */
    uint8_t                         low_drv;            /*!< Low speed touch driver, only effective when high speed driver is disabled */
    uint8_t                         high_drv;           /*!< High speed touch driver */
    uint8_t                         bias_volt;          /*!< The Internal LDO voltage, which decide the bias voltage of the sample wave, range [0,15] */
    bool                            bypass_shield_output; /*!< Whether to bypass the shield output, enable when the charging/discharging rate greater than 10MHz */
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
    touch_out_mode_t                output_mode;        /*!< Touch channel counting mode of the binarized touch output */
    uint32_t                        sample_cfg_num;     /*!< The sample configuration number that used for sampling */
    touch_sensor_sample_config_t    *sample_cfg;        /*!< The array of this sample configuration configurations, the length should be specified in `touch_sensor_config_t::sample_cfg_num` */
} touch_sensor_config_t;

/**
 * @brief Configurations of the touch sensor channel
 *
 */
typedef struct {
    uint32_t                        active_thresh[TOUCH_SAMPLE_CFG_NUM];  /*!< The active threshold of each sample configuration,
                                                                           *   while the touch channel smooth value minus benchmark value exceed this threshold,
                                                                           *   will be regarded as activated
                                                                           */
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
 * @brief Configure the touch sensor sleep function
 *
 */
typedef struct {
    touch_sleep_wakeup_level_t      slp_wakeup_lvl;     /*!< The sleep level that can be woke up by touch sensor. */
    touch_channel_handle_t          deep_slp_chan;      /*!< The touch channel handle that supposed to work in the deep sleep. It can wake up the chip
                                                         *   from deep sleep when this channel is activated.
                                                         *   Only effective when the `touch_sleep_config_t::slp_wakeup_lvl` is `TOUCH_DEEP_SLEEP_WAKEUP`
                                                         */
    uint32_t                        deep_slp_thresh[TOUCH_SAMPLE_CFG_NUM];  /*!< The active threshold of the deep sleep channel during deep sleep,
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
 * @brief Configure the touch sensor waterproof function
 *
 */
typedef struct {
    touch_channel_handle_t          guard_chan;         /*!< The guard channel of that used for immersion detect. Set NULL if you don't need the guard channel.
                                                         *   Typically, the guard channel is a ring that surrounds the touch panels,
                                                         *   it is used to detect the large area that covered by water.
                                                         *   While large area of water covers the touch panel, the guard channel will be activated.
                                                         */
    touch_channel_handle_t          shield_chan;        /*!< The shield channel that used for water droplets shield, can't be NULL.
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
 * @brief Configure the touch sensor proximity function
 *
 */
typedef struct {
    touch_channel_handle_t          proximity_chan[TOUCH_PROXIMITY_CHAN_NUM]; /*!< The touch channel handles that will be configured as proximity sensing channels */
    uint32_t                        scan_times;                               /*!< The total scan times of EACH sample configuration, all sample configurations share a same `scan_times`.
                                                                               *   The measurement result of each scanning will be accumulated together to get the final result.
                                                                               */
    uint32_t                        charge_times[TOUCH_SAMPLE_CFG_NUM];       /*!< The charge times of EACH scanning, different sample configurations can set different `charge_times`.
                                                                               *   The measurement result of each scanning is positive correlation to the `charge_times`,
                                                                               *   please set a proper value in case of the final accumulated result overflow.
                                                                               */
} touch_proximity_config_t;

/**
 * @brief Base event structure used in touch event queue
 */
typedef struct {
    touch_channel_handle_t  chan;           /*!< the current triggered touch channel handle */
    int                     chan_id;        /*!< the current triggered touch channel number */
    uint32_t                status_mask;    /*!< the current channel triggered status.
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
