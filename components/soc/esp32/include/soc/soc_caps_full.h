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

/*------------------------------- Touch Sensor ------------------------------------*/
#define _SOC_CAPS_TOUCH_CHAN_NUM                  10 // Number of touch sensor channels
