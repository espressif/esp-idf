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
