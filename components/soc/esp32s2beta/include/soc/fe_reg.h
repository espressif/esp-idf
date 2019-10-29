// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "soc/soc.h"

/* Some of the RF frontend control registers.
 * PU/PD fields defined here are used in sleep related functions.
 */

#define FE_GEN_CTRL (DR_REG_FE_BASE + 0x0090)
#define FE_IQ_EST_FORCE_PU (BIT(5))
#define FE_IQ_EST_FORCE_PU_M (BIT(5))
#define FE_IQ_EST_FORCE_PU_V 1
#define FE_IQ_EST_FORCE_PU_S 5
#define FE_IQ_EST_FORCE_PD (BIT(4))
#define FE_IQ_EST_FORCE_PD_M (BIT(4))
#define FE_IQ_EST_FORCE_PD_V 1
#define FE_IQ_EST_FORCE_PD_S 4

#define FE2_TX_INTERP_CTRL (DR_REG_FE2_BASE + 0x00f0)
#define FE2_TX_INF_FORCE_PU (BIT(10))
#define FE2_TX_INF_FORCE_PU_M (BIT(10))
#define FE2_TX_INF_FORCE_PU_V 1
#define FE2_TX_INF_FORCE_PU_S 10
#define FE2_TX_INF_FORCE_PD (BIT(9))
#define FE2_TX_INF_FORCE_PD_M (BIT(9))
#define FE2_TX_INF_FORCE_PD_V 1
#define FE2_TX_INF_FORCE_PD_S 9
