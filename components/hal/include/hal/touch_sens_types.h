/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

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
#if SOC_TOUCH_SENSOR_VERSION == 2
    TOUCH_BM_IIR_FILTER_256,                /*!< IIR Filter for benchmark, 1/256 raw_value + 255/256 benchmark */
#endif
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
 * @brief Touch sensor upper charging voltage limit
 */
typedef enum {
    TOUCH_VOLT_LIM_H_0V9,                   /*!< Touch sensor upper voltage limit is 0.9V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V0,                   /*!< Touch sensor upper voltage limit is 1.0V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V1,                   /*!< Touch sensor upper voltage limit is 1.1V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V2,                   /*!< Touch sensor upper voltage limit is 1.2V while charging a touch pad */
    // No 1V3
    TOUCH_VOLT_LIM_H_1V4,                   /*!< Touch sensor upper voltage limit is 1.4V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V5,                   /*!< Touch sensor upper voltage limit is 1.5V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V6,                   /*!< Touch sensor upper voltage limit is 1.6V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V7,                   /*!< Touch sensor upper voltage limit is 1.7V while charging a touch pad */
    // No 1V8
    TOUCH_VOLT_LIM_H_1V9,                   /*!< Touch sensor upper voltage limit is 1.9V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V0,                   /*!< Touch sensor upper voltage limit is 2.0V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V1,                   /*!< Touch sensor upper voltage limit is 2.1V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V2,                   /*!< Touch sensor upper voltage limit is 2.2V while charging a touch pad */
    // No 2V3
    TOUCH_VOLT_LIM_H_2V4,                   /*!< Touch sensor upper voltage limit is 2.4V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V5,                   /*!< Touch sensor upper voltage limit is 2.5V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V6,                   /*!< Touch sensor upper voltage limit is 2.6V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V7,                   /*!< Touch sensor upper voltage limit is 2.7V while charging a touch pad */
} touch_volt_lim_h_t;

/**
 * @brief Touch sensor lower discharging voltage limit
 */
typedef enum {
    TOUCH_VOLT_LIM_L_0V5,                   /*!< Touch sensor lower voltage limit is 0.5V while discharging a touch pad */
    TOUCH_VOLT_LIM_L_0V6,                   /*!< Touch sensor lower voltage limit is 0.6V while discharging a touch pad */
    TOUCH_VOLT_LIM_L_0V7,                   /*!< Touch sensor lower voltage limit is 0.7V while discharging a touch pad */
    TOUCH_VOLT_LIM_L_0V8,                   /*!< Touch sensor lower voltage limit is 0.8V while discharging a touch pad */
} touch_volt_lim_l_t;

/**
 * @brief Touch sensor charge and discharge speed
 */
typedef enum {
    TOUCH_CHARGE_SPEED_0 = 0,               /*!< Touch sensor charge and discharge speed, no charge, always zero  */
    TOUCH_CHARGE_SPEED_1 = 1,               /*!< Touch sensor charge and discharge speed, slowest  */
    TOUCH_CHARGE_SPEED_2 = 2,               /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_3 = 3,               /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_4 = 4,               /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_5 = 5,               /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_6 = 6,               /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_7 = 7,               /*!< Touch sensor charge and discharge speed, fastest  */
} touch_charge_speed_t;

/**
 * @brief Touch sensor initial voltage before charging
 */
typedef enum {
    TOUCH_INIT_CHARGE_VOLT_LOW = 0,         /*!< Tie the initial charge voltage to low. */
    TOUCH_INIT_CHARGE_VOLT_HIGH = 1,        /*!< Tie the initial charge voltage to high. */
    TOUCH_INIT_CHARGE_VOLT_FLOAT = 2,       /*!< The initial charge voltage will be float. The touch pad will be powered off between two measurements */
    TOUCH_INIT_CHARGE_VOLT_DEFAULT = TOUCH_INIT_CHARGE_VOLT_FLOAT,       /*!< The initial charge voltage is default to be float. */
} touch_init_charge_volt_t;

/**
 *  @brief Touch channel idle state configuration
 */
typedef enum {
    TOUCH_IDLE_CONN_HIGHZ = 0,              /*!< The idle (enabled but not measuring) touch channel is at high resistance state */
    TOUCH_IDLE_CONN_GND = 1,                /*!< The idle (enabled but not measuring) touch channel is connected to the ground */
} touch_idle_conn_t;

/**
 * @brief Touch sensor denoise channel internal reference capacitance
 */
typedef enum {
    TOUCH_DENOISE_CHAN_CAP_5PF   = 0,       /*!< Denoise channel internal reference capacitance is 5.0pf */
    TOUCH_DENOISE_CHAN_CAP_6PF   = 1,       /*!< Denoise channel internal reference capacitance is 6.4pf */
    TOUCH_DENOISE_CHAN_CAP_7PF   = 2,       /*!< Denoise channel internal reference capacitance is 7.8pf */
    TOUCH_DENOISE_CHAN_CAP_9PF   = 3,       /*!< Denoise channel internal reference capacitance is 9.2pf */
    TOUCH_DENOISE_CHAN_CAP_10PF  = 4,       /*!< Denoise channel internal reference capacitance is 10.6pf */
    TOUCH_DENOISE_CHAN_CAP_12PF  = 5,       /*!< Denoise channel internal reference capacitance is 12.0pf */
    TOUCH_DENOISE_CHAN_CAP_13PF  = 6,       /*!< Denoise channel internal reference capacitance is 13.4pf */
    TOUCH_DENOISE_CHAN_CAP_14PF  = 7,       /*!< Denoise channel internal reference capacitance is 14.8pf */
} touch_denoise_chan_cap_t;

/**
 * @brief Touch sensor denoise channel noise suppression resolution
 */
typedef enum {
    TOUCH_DENOISE_CHAN_RESOLUTION_BIT12 = 0,    /*!< Denoise channel noise suppression resolution is 12bit */
    TOUCH_DENOISE_CHAN_RESOLUTION_BIT10 = 1,    /*!< Denoise channel noise suppression resolution is 10bit */
    TOUCH_DENOISE_CHAN_RESOLUTION_BIT8 = 2,     /*!< Denoise channel noise suppression resolution is 8bit */
    TOUCH_DENOISE_CHAN_RESOLUTION_BIT4 = 3,     /*!< Denoise channel noise suppression resolution is 4bit */
} touch_denoise_chan_resolution_t;

/**
 * @brief Touch sensor bias type
 */
typedef enum {
    TOUCH_BIAS_TYPE_BANDGAP,                /*!< Use bandgap-bias to charge/discharge the touch channel, which is more stable but power-consuming */
    TOUCH_BIAS_TYPE_SELF,                   /*!< Use self-bias to charge/discharge the touch channel, which is less stable but power-saving */
} touch_bias_type_t;

/**
 * @brief Touch channel binarized output counting mode
 */
typedef enum {
    TOUCH_PAD_OUT_AS_DATA,                  /*!< Counting the output of touch channel as data.
                                             *   The value will be smaller than actual value but more sensitive when the frequency of touch_out is close to the source clock
                                             *   Normally we treat the output as data when it is lower than the sample clock
                                             */
    TOUCH_PAD_OUT_AS_CLOCK,                 /*!< Counting the output of touch channel as clock.
                                             *   The value is accurate but less sensitive when the frequency of touch_out is close to the source clock
                                             *   Normally we treat the output as clock when it is higher than the sample clock
                                             */
} touch_out_mode_t;

/**
 * @brief Touch interrupt trigger mode
 *
 */
typedef enum {
    TOUCH_INTR_TRIG_ON_BELOW_THRESH,        /*!< The touch active interrupt will trigger when the measured data below the absolute threshold */
    TOUCH_INTR_TRIG_ON_ABOVE_THRESH,        /*!< The touch active interrupt will trigger when the measured data above the absolute threshold */
} touch_intr_trig_mode_t;

/**
 * @brief Touch interrupt trigger group
 *
 */
typedef enum {
    TOUCH_INTR_TRIG_GROUP_BOTH,             /*!< Both channel groups can trigger the interrupt */
    TOUCH_INTR_TRIG_GROUP_1,                /*!< Only the channels in group 1 can trigger the interrupt */
} touch_intr_trig_group_t;

#ifdef __cplusplus
}
#endif
