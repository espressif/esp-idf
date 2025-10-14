/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/soc_caps_eval.h"

/*--------------------------- Timer Group -------------------------------------------*/
#define _SOC_CAPS_TIMG_INST_NUM                   2 // Number of Timer Group instances

/*--------------------------- GPTIMER ---------------------------------------*/
#define _SOC_CAPS_GPTIMER_COUNTER_BIT_WIDTH       54 // Bit width of GPTIMER counter
#define _SOC_CAPS_GPTIMER_TIMERS_PER_TIMG         1  // Number of general purpose timers in each Timer Group
#define _SOC_CAPS_GPTIMER_TIMERS_TOTAL            (_SOC_CAPS_TIMG_INST_NUM * _SOC_CAPS_GPTIMER_TIMERS_PER_TIMG)

/*--------------------------- Watch Dog ------------------------------------------*/
#define _SOC_CAPS_WDT_MWDTS_PER_TIMG              1  // Number of main watchdog timers in each Timer Group

/*--------------------------- SDM (Sigma-Delta Modulator) ------------------------*/
#define _SOC_CAPS_SDM_INST_NUM                    1  // Number of SDM instances
#define _SOC_CAPS_SDM_CHANS_PER_INST              4  // Number of channels in each SDM instance

/*------------------------------- Dedicated GPIO ------------------------------*/
#define _SOC_CAPS_DEDIC_GPIO_OUT_CHANS_PER_CPU     8 /*!< 8 outward channels on each CPU core */
#define _SOC_CAPS_DEDIC_GPIO_IN_CHANS_PER_CPU      8 /*!< 8 inward channels on each CPU core */

/*------------------------------- I2S ---------------------------------------*/
// helper macros to access module attributes
#define _SOC_CAPS_I2S_INST_NUM                    1  // Number of I2S instances
