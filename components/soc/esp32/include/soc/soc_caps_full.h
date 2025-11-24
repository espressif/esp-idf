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

/*------------------------------- I2S ---------------------------------------*/
// helper macros to access module attributes
#define _SOC_CAPS_I2S_INST_NUM                    2  // Number of I2S instances

/*------------------------------- Touch Sensor ------------------------------------*/
#define _SOC_CAPS_TOUCH_CHAN_NUM                  10 // Number of touch sensor channels
