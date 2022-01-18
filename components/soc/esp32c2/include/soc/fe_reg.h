/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
