/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/soc_caps_eval.h"

/*--------------------------- SDM (Sigma-Delta Modulator) ------------------------*/
#define _SOC_CAPS_SDM_INST_NUM                    1  // Number of SDM instances
#define _SOC_CAPS_SDM_CHANS_PER_INST              8  // Number of channels in each SDM instance

/*--------------------------- PCNT (Pulse Counter) ------------------------*/
#define _SOC_CAPS_PCNT_INST_NUM                   1  // Number of PCNT instances
#define _SOC_CAPS_PCNT_UNITS_PER_INST             8  // Number of units in each PCNT instance
#define _SOC_CAPS_PCNT_CHANS_PER_UNIT             2  // Number of channels in each PCNT unit
#define _SOC_CAPS_PCNT_THRES_POINT_PER_UNIT       2  // Number of threshold points in each PCNT unit

/*------------------------------- I2S ---------------------------------------*/
// helper macros to access module attributes
#define _SOC_CAPS_I2S_INST_NUM                    2  // Number of I2S instances

/*------------------------------- Touch Sensor ------------------------------------*/
#define _SOC_CAPS_TOUCH_CHAN_NUM                  10 // Number of touch sensor channels
