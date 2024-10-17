/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Touch sensor upper charging voltage limit
 */
typedef enum {
    TOUCH_VOLT_LIM_H_0V9,         /*!< Touch sensor upper voltage limit is 0.9V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V0,         /*!< Touch sensor upper voltage limit is 1.0V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V1,         /*!< Touch sensor upper voltage limit is 1.1V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V2,         /*!< Touch sensor upper voltage limit is 1.2V while charging a touch pad */
    // No 1V3
    TOUCH_VOLT_LIM_H_1V4,         /*!< Touch sensor upper voltage limit is 1.4V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V5,         /*!< Touch sensor upper voltage limit is 1.5V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V6,         /*!< Touch sensor upper voltage limit is 1.6V while charging a touch pad */
    TOUCH_VOLT_LIM_H_1V7,         /*!< Touch sensor upper voltage limit is 1.7V while charging a touch pad */
    // No 1V8
    TOUCH_VOLT_LIM_H_1V9,         /*!< Touch sensor upper voltage limit is 1.9V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V0,         /*!< Touch sensor upper voltage limit is 2.0V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V1,         /*!< Touch sensor upper voltage limit is 2.1V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V2,         /*!< Touch sensor upper voltage limit is 2.2V while charging a touch pad */
    // No 2V3
    TOUCH_VOLT_LIM_H_2V4,         /*!< Touch sensor upper voltage limit is 2.4V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V5,         /*!< Touch sensor upper voltage limit is 2.5V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V6,         /*!< Touch sensor upper voltage limit is 2.6V while charging a touch pad */
    TOUCH_VOLT_LIM_H_2V7,         /*!< Touch sensor upper voltage limit is 2.7V while charging a touch pad */
} touch_volt_lim_h_t;

/**
 * @brief Touch sensor lower discharging voltage limit
 */
typedef enum {
    TOUCH_VOLT_LIM_L_0V5,         /*!< Touch sensor lower voltage limit is 0.5V while charging a touch pad */
    TOUCH_VOLT_LIM_L_0V6,         /*!< Touch sensor lower voltage limit is 0.5V while charging a touch pad */
    TOUCH_VOLT_LIM_L_0V7,         /*!< Touch sensor lower voltage limit is 0.5V while charging a touch pad */
    TOUCH_VOLT_LIM_L_0V8,         /*!< Touch sensor lower voltage limit is 0.5V while charging a touch pad */
} touch_volt_lim_l_t;

/**
 * @brief Touch sensor charge and discharge speed
 */
typedef enum {
    TOUCH_CHARGE_SPEED_0 = 0,     /*!< Touch sensor charge and discharge speed, no charge, always zero  */
    TOUCH_CHARGE_SPEED_1 = 1,     /*!< Touch sensor charge and discharge speed, slowest  */
    TOUCH_CHARGE_SPEED_2 = 2,     /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_3 = 3,     /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_4 = 4,     /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_5 = 5,     /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_6 = 6,     /*!< Touch sensor charge and discharge speed */
    TOUCH_CHARGE_SPEED_7 = 7,     /*!< Touch sensor charge and discharge speed, fastest  */
} touch_charge_speed_t;

/**
 * @brief Touch sensor initial voltage before charging
 */
typedef enum {
    TOUCH_INIT_CHARGE_VOLT_LOW = 0, /*!< Tie the initial charge voltage to low */
    TOUCH_INIT_CHARGE_VOLT_HIGH = 1,/*!< Tie the initial charge voltage to high */
} touch_init_charge_volt_t;

/** Touch channel idle state configuration */
typedef enum {
    TOUCH_IDLE_CONN_HIGHZ = 0,      /*!< The idle (enabled but not measuring) touch channel is in high resistance state */
    TOUCH_IDLE_CONN_GND = 1,        /*!< The idle (enabled but not measuring) touch channel is connected to the ground */
} touch_idle_conn_t;

/**
 * @brief Touch sensor denoise channel internal reference capacitance
 *
 */
typedef enum {
    TOUCH_DENOISE_CHAN_CAP_L0 = 0,  /*!< Denoise channel internal reference capacitance is 5pf */
    TOUCH_DENOISE_CHAN_CAP_L1 = 1,  /*!< Denoise channel internal reference capacitance is 6.4pf */
    TOUCH_DENOISE_CHAN_CAP_L2 = 2,  /*!< Denoise channel internal reference capacitance is 7.8pf */
    TOUCH_DENOISE_CHAN_CAP_L3 = 3,  /*!< Denoise channel internal reference capacitance is 9.2pf */
    TOUCH_DENOISE_CHAN_CAP_L4 = 4,  /*!< Denoise channel internal reference capacitance is 10.6pf */
    TOUCH_DENOISE_CHAN_CAP_L5 = 5,  /*!< Denoise channel internal reference capacitance is 12.0pf */
    TOUCH_DENOISE_CHAN_CAP_L6 = 6,  /*!< Denoise channel internal reference capacitance is 13.4pf */
    TOUCH_DENOISE_CHAN_CAP_L7 = 7,  /*!< Denoise channel internal reference capacitance is 14.8pf */
} touch_denoise_chan_cap_t;

/**
 * @brief Touch sensor denoise channel noise suppression resolution
 *
 */
typedef enum {
    TOUCH_DENOISE_CHAN_RES_BIT12 = 0,    /*!< Denoise channel noise suppression resolution is 12bit */
    TOUCH_DENOISE_CHAN_RES_BIT10 = 1,    /*!< Denoise channel noise suppression resolution is 10bit */
    TOUCH_DENOISE_CHAN_RES_BIT8 = 2,     /*!< Denoise channel noise suppression resolution is 8bit */
    TOUCH_DENOISE_CHAN_RES_BIT4 = 3,     /*!< Denoise channel noise suppression resolution is 4bit */
} touch_denoise_chan_res_t;

/**
 * @brief Touch sensor bias type
 *
 */
typedef enum {
    TOUCH_BIAS_TYPE_BANDGAP,            /*!< Use bandgap-bias to charge/discharge the touch channel, which is more stable but power-consuming */
    TOUCH_BIAS_TYPE_SELF,               /*!< Use self-bias to charge/discharge the touch channel, which is less stable but power-saving */
} touch_bias_type_t;

/**
 * @brief Touch channel counting mode of the binarized touch output
 *
 */
typedef enum {
    TOUCH_PAD_OUT_AS_DATA,      /*!< Counting the output of touch channel as data.
                                 *   The value will be smaller than actual value but more sensitive when the frequency of touch_out is close to the source clock
                                 *   Normally we treat the output as data when it is lower than the sample clock
                                 */
    TOUCH_PAD_OUT_AS_CLOCK,     /*!< Counting the output of touch channel as clock.
                                 *   The value is accurate but less sensitive when the frequency of touch_out is close to the source clock
                                 *   Normally we treat the output as clock when it is higher than the sample clock
                                 */
} touch_out_mode_t;

#ifdef __cplusplus
}
#endif
