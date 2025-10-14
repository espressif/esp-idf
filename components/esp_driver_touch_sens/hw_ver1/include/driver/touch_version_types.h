/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief This file is only applicable to the touch hardware version1
 *        Version 1 includes ESP32
 */

#pragma once

#include "soc/soc_caps.h"
#include "driver/touch_sens_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Helper macro to the default configurations of the touch sensor controller
 *
 * @param[in] sample_cfg_number     The number of the sample configurations, which can only be 1 here because there is only one sample configuration
 * @param[in] sample_cfg_ptr        The pointer to the sample configurations
 */
#define TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(sample_cfg_number, sample_cfg_ptr) { \
    .power_on_wait_us = 256, \
    .meas_interval_us = 320.0, \
    .intr_trig_mode = TOUCH_INTR_TRIG_ON_BELOW_THRESH,  \
    .intr_trig_group = TOUCH_INTR_TRIG_GROUP_BOTH,  \
    .sample_cfg_num = sample_cfg_number,  \
    .sample_cfg = sample_cfg_ptr,  \
}

/**
 * @brief Helper macro to the default sample configurations
 * @note  This default configuration uses `sample frequency = clock frequency / 1`
 *
 * @param[in] duration_ms   The measurement duration of the touch channel
 * @param[in] volt_low      The low voltage limit of the touch channel
 * @param[in] volt_high     The high voltage limit of the touch channel
 */
#define TOUCH_SENSOR_V1_DEFAULT_SAMPLE_CONFIG(duration_ms, volt_low, volt_high) {  \
    .charge_duration_ms = duration_ms,  \
    .charge_volt_lim_h = volt_high,  \
    .charge_volt_lim_l = volt_low,  \
}

/**
 * @brief Helper macro to the default filter configurations
 *
 */
#define TOUCH_SENSOR_DEFAULT_FILTER_CONFIG() { \
    .interval_ms = 10,  \
    .data_filter_fn = NULL,  /* Set NULL to use default software filter */  \
}

/**
 * @brief The data type of the touch channel
 *
 */
typedef enum {
    TOUCH_CHAN_DATA_TYPE_RAW,                   /*!< The raw data of the touch channel */
    TOUCH_CHAN_DATA_TYPE_SMOOTH,                /*!< The smooth data of the touch channel (need to config software filter fist) */
} touch_chan_data_type_t;

/**
 * @brief Touch channel trigger group
 *
 */
typedef enum {
    TOUCH_CHAN_TRIG_GROUP_1 = 0x01,             /*!< Channel will be added to the interrupt trigger group 1 */
    TOUCH_CHAN_TRIG_GROUP_2 = 0x02,             /*!< Channel will be added to the interrupt trigger group 2 */
    TOUCH_CHAN_TRIG_GROUP_BOTH = 0x03,          /*!< Channel will be added to both interrupt trigger group 1 and 2 */
} touch_chan_trig_group_t;

/**
 * @brief Sample configurations of the touch sensor
 *
 */
typedef struct {
    float                           charge_duration_ms; /*!< Charge duration time of one measurement on a touch channel */
    touch_volt_lim_h_t              charge_volt_lim_h;  /*!< The upper voltage limit while charging a touch pad. i.e., the touch controller won't charge the touch pad higher than this high voltage limitation. */
    touch_volt_lim_l_t              charge_volt_lim_l;  /*!< The lower voltage limit while discharging a touch pad. i.e., the touch controller won't discharge the touch pad lower than this low voltage limitation. */
} touch_sensor_sample_config_t;

/**
 * @brief Configurations of the touch sensor controller
 *
 */
typedef struct {
    uint32_t                        power_on_wait_us;   /*!< The waiting time between the channels power on and able to measure, to ensure the data stability */
    float                           meas_interval_us;   /*!< Measurement interval of each channels */
    touch_intr_trig_mode_t          intr_trig_mode;     /*!< Interrupt trigger mode, the hardware touch interrupt can be triggered either above or below the absolute threshold */
    touch_intr_trig_group_t         intr_trig_group;    /*!< Which channel group can trigger the hardware touch interrupt */
    /* Touch sensor sample configuration */
    uint32_t                        sample_cfg_num;     /*!< The sample configuration number that used for sampling, CANNOT exceed TOUCH_SAMPLE_CFG_NUM */
    touch_sensor_sample_config_t    *sample_cfg;        /*!< The array of this sample configuration configurations, the length should be specified in `touch_sensor_config_t::sample_cfg_num` */
} touch_sensor_config_t;

/**
 * @brief Configurations of the touch sensor channel
 *
 */
typedef struct {
    uint32_t                        abs_active_thresh[TOUCH_SAMPLE_CFG_NUM];    /*!< The absolute active threshould. The ``on_active`` and ``on_hw_active`` callback will trigger when
                                                                                 *   - If touch_sensor_config_t::intr_trig_mode = TOUCH_INTR_TRIG_ON_BELOW_THRESH, the callback will keep triggering when the touch channel value below the threshold.
                                                                                 *   - If touch_sensor_config_t::intr_trig_mode = TOUCH_INTR_TRIG_ON_ABOVE_THRESH, the callback will keep triggering when the touch channel value above the threshold.
                                                                                 */
    touch_charge_speed_t            charge_speed;       /*!< The speed of charging and discharging the touch pad, the higher the speed, the faster charging and discharging */
    touch_init_charge_volt_t        init_charge_volt;   /*!< The initial voltage before charging/discharging a touch pad */
    touch_chan_trig_group_t         group;              /*!< The channel group that the channel belongs to. The group will be used to trigger the interrupt */
} touch_channel_config_t;

/**
 * @brief Touch software filter data
 *
 */
typedef struct {
    uint32_t                        prev_output;        /*!< Previous output filtered data */
    uint32_t                        curr_input;         /*!< Current input raw data that need to be filtered */
} touch_sw_filter_data_t;

/**
 * @brief Touch software filter prototype
 * @note  Users can customize their own filter algorithm by this prototype
 * @param[in] chan_handle       The handle of the touch channel that need to be filtered
 * @param[in] filter_data       The data of the software filter
 * @param[in] user_filter_ctx   User customized filter context pointer
 * @return
 *      - uint32_t         The filtered data
 */
typedef uint32_t (*touch_sw_filter_t)(touch_channel_handle_t chan_handle, const touch_sw_filter_data_t *filter_data, void *user_filter_ctx);

/**
 * @brief The configuration of the software filter
 *
 */
typedef struct {
    uint32_t                        interval_ms;        /*!< The software filter interval in milliseconds.
                                                         *   The software filter will trigger periodically based on esp_timer
                                                         */
    touch_sw_filter_t               data_filter_fn;     /*!< The data filter function pointer.
                                                         *   You can specify your own filter algorithm, or set NULL to use default software filter
                                                         */
    void                            *user_filter_ctx;   /*!< User customized filter context pointer.
                                                         *   This pointer will be passed to the second parameter of `touch_sw_filter_t` .
                                                         *   So that users can access their customized filter context in the software filter function.
                                                         */
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
 * @brief Configuration of the touch sensor sleep function
 *
 */
typedef struct {
    touch_sleep_wakeup_level_t      slp_wakeup_lvl;     /*!< The sleep level that can be woke up by touch sensor. */
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
 * @brief Base event structure used in touch event queue
 */
typedef struct {
    touch_channel_handle_t          chan;               /*!< the current triggered touch channel handle */
    int                             chan_id;            /*!< the current triggered touch channel number */
} touch_base_event_data_t;

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
 * @brief Active event data
 *
 */
typedef struct {
    uint32_t                        active_mask;    /*!< The current active channel mask.
                                                     *   For the bits in the status mask,
                                                     *   if the bit is set, the corresponding channel is active
                                                     *   if the bit is cleared, the corresponding channel is inactive
                                                     */
} touch_hw_active_event_data_t;

/**
 * @brief Touch sensor callbacks
 * @note  Set NULL for the unused callbacks.
 * @note  The Touch Sensor V1 hardware interrupt callback is different compare to other versions.
 *        To align the behavior of the `on_active` and `on_inactive`, they are added as the software filter callbacks.
 *        Please configure the software filter by `touch_sensor_config_filter` before using hardware interrupt.
 *
 */
typedef struct {
    union {
        /**
         * @brief Touch sensor on software active event callback.
         * @note  This callback is triggered by the software periodical filter.
         *        It callbacks when any touch channel is activated.
         * @param[in] sens_handle   Touch sensor controller handle, created from `touch_sensor_new_controller()`
         * @param[in] event         Touch sensor active event data
         * @param[in] user_ctx      User registered context, passed from `touch_sensor_register_callbacks()`
         *
         * @return Whether a high priority task has been waken up by this callback function
         */
        bool (*on_sw_active)(touch_sensor_handle_t sens_handle, const touch_active_event_data_t *event, void *user_ctx);
        /**
         * @brief Touch sensor on software active event callback. (Alias of `on_sw_active` for compatibility)
         * @note  This callback is triggered by the software periodical filter.
         *        It callbacks when any touch channel is activated.
         * @param[in] sens_handle   Touch sensor controller handle, created from `touch_sensor_new_controller()`
         * @param[in] event         Touch sensor active event data
         * @param[in] user_ctx      User registered context, passed from `touch_sensor_register_callbacks()`
         *
         * @return Whether a high priority task has been waken up by this callback function
         */
        bool (*on_active)(touch_sensor_handle_t sens_handle, const touch_active_event_data_t *event, void *user_ctx);
    };

    union {
        /**
         * @brief Touch sensor on software inactive event callback.
         * @note  This callback is triggered by the software periodical filter.
         *        It callbacks when any touch channel is de-activated.
         * @param[in] sens_handle   Touch sensor controller handle, created from `touch_sensor_new_controller()`
         * @param[in] event         Touch sensor active event data
         * @param[in] user_ctx      User registered context, passed from `touch_sensor_register_callbacks()`
         * @return Whether a high priority task has been waken up by this callback function
         */
        bool (*on_sw_inactive)(touch_sensor_handle_t sens_handle, const touch_inactive_event_data_t *event, void *user_ctx);
        /**
         * @brief Touch sensor on software inactive event callback. (Alias of `on_sw_active` for compatibility)
         * @note  This callback is triggered by the software periodical filter.
         *        It callbacks when any touch channel is de-activated.
         * @param[in] sens_handle   Touch sensor controller handle, created from `touch_sensor_new_controller()`
         * @param[in] event         Touch sensor active event data
         * @param[in] user_ctx      User registered context, passed from `touch_sensor_register_callbacks()`
         * @return Whether a high priority task has been waken up by this callback function
         */
        bool (*on_inactive)(touch_sensor_handle_t sens_handle, const touch_inactive_event_data_t *event, void *user_ctx);
    };

    /**
     * @brief Touch sensor on hardware active interrupt event callback.
     * @note  This callback is triggered by the hardware interrupt.
     *        It callbacks every time when any of the touch channel below or above (depended on touch_sensor_config_t::intr_trig_mode) the threshold.
     * @param[in] sens_handle   Touch sensor controller handle, created from `touch_sensor_new_controller()`
     * @param[in] event         Touch sensor active event data
     * @param[in] user_ctx      User registered context, passed from `touch_sensor_register_callbacks()`
     *
     * @return Whether a high priority task has been waken up by this callback function
     */
    bool (*on_hw_active)(touch_sensor_handle_t sens_handle, const touch_hw_active_event_data_t *event, void *user_ctx);
} touch_event_callbacks_t;

#ifdef __cplusplus
}
#endif
