/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for touch sensor (ESP32-P4 specific part)

#pragma once

#include "hal/touch_sensor_ll.h"
#include "hal/touch_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sample configurations of the touch sensor
 *
 */
typedef struct {
    uint32_t                        div_num;            /*!< The division from the source clock to the sampling frequency */
    uint32_t                        charge_times;       /*!< The charge and discharge times of the sample configuration, the read data are positive correlation to the charge_times */
    uint8_t                         rc_filter_res;      /*!< The resistance of the RC filter of the sample configuration, range [0, 3], while 0 = 0K, 1 = 1.5K, 2 = 3K, 3 = 4.5K */
    uint8_t                         rc_filter_cap;      /*!< The capacitance of the RC filter of the sample configuration, range [0, 127], while 0 = 0pF, 1 = 20fF, ..., 127 = 2.54pF */
    uint8_t                         low_drv;            /*!< Low speed touch driver, only effective when high speed driver is disabled */
    uint8_t                         high_drv;           /*!< High speed touch driver */
    uint8_t                         bias_volt;          /*!< The Internal LDO voltage, which decide the bias voltage of the sample wave, range [0,15] */
    bool                            bypass_shield_output; /*!< Whether to bypass the shield output */
} touch_hal_sample_config_t;

/**
 * @brief Configurations of the touch sensor controller
 *
 */
typedef struct {
    uint32_t                        power_on_wait_ticks;    /*!< The waiting time between the channels power on and able to measure, to ensure the data stability */
    uint32_t                        meas_interval_ticks;    /*!< Measurement interval of each channels */ // TODO: Test the supported range
    uint32_t                        timeout_ticks;          /*!< The maximum time of measuring one channel, if the time exceeds this value, the timeout interrupt will be triggered.
                                                             *   Set to '0' to ignore the measurement time limitation, otherwise please set a proper time considering the configurations
                                                             *   of the sample configurations below.
                                                             */
    touch_out_mode_t                output_mode;            /*!< Touch channel counting mode of the binarized touch output */
    uint32_t                        sample_cfg_num;         /*!< The sample configuration number that used for sampling */
    touch_hal_sample_config_t       *sample_cfg;            /*!< The array of the sample configuration configurations, the length should be specified in `touch_hal_sample_config_t::sample_cfg_num` */
} touch_hal_config_t;

/**
 * @brief Configure the touch sensor hardware with the configuration
 *
 * @param[in]  cfg  Touch sensor hardware configuration
 */
void touch_hal_config_controller(const touch_hal_config_t *cfg);

/**
 * @brief Save the touch sensor hardware configuration
 * @note  The saved configurations will be applied before entering deep sleep
 *
 * @param[in]  deep_slp_chan    The touch sensor channel that can wake-up the chip from deep sleep
 * @param[in]  deep_slp_cfg     The hardware configuration that takes effect during the deep sleep
 */
void touch_hal_save_sleep_config(int deep_slp_chan, const touch_hal_config_t *deep_slp_cfg);

/**
 * @brief Prepare for the deep sleep
 * @note  Including apply the deep sleep configuration, clear interrupts, resetting benchmark
 */
void touch_hal_prepare_deep_sleep(void);

#ifdef __cplusplus
}
#endif
