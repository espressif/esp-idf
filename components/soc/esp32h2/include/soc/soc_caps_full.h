/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/soc_caps_eval.h"

/*------------------------------- Dedicated GPIO ------------------------------*/
#define _SOC_CAPS_DEDIC_GPIO_OUT_CHANS_PER_CPU     8 /*!< 8 outward channels on each CPU core */
#define _SOC_CAPS_DEDIC_GPIO_IN_CHANS_PER_CPU      8 /*!< 8 inward channels on each CPU core */

/*--------------------------- SDM (Sigma-Delta Modulator) ------------------------*/
#define _SOC_CAPS_SDM_INST_NUM                    1  // Number of SDM instances
#define _SOC_CAPS_SDM_CHANS_PER_INST              4  // Number of channels in each SDM instance
