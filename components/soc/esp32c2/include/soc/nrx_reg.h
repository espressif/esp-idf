/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"

/* Some of the WiFi RX control registers.
 * PU/PD fields defined here are used in sleep related functions.
 */

#define NRXPD_CTRL (DR_REG_NRX_BASE + 0x00d4)
#define NRX_CHAN_EST_FORCE_PU (BIT(7))
#define NRX_CHAN_EST_FORCE_PU_M (BIT(7))
#define NRX_CHAN_EST_FORCE_PU_V 1
#define NRX_CHAN_EST_FORCE_PU_S 7
#define NRX_CHAN_EST_FORCE_PD (BIT(6))
#define NRX_CHAN_EST_FORCE_PD_M (BIT(6))
#define NRX_CHAN_EST_FORCE_PD_V 1
#define NRX_CHAN_EST_FORCE_PD_S 6
#define NRX_RX_ROT_FORCE_PU (BIT(5))
#define NRX_RX_ROT_FORCE_PU_M (BIT(5))
#define NRX_RX_ROT_FORCE_PU_V 1
#define NRX_RX_ROT_FORCE_PU_S 5
#define NRX_RX_ROT_FORCE_PD (BIT(4))
#define NRX_RX_ROT_FORCE_PD_M (BIT(4))
#define NRX_RX_ROT_FORCE_PD_V 1
#define NRX_RX_ROT_FORCE_PD_S 4
#define NRX_VIT_FORCE_PU (BIT(3))
#define NRX_VIT_FORCE_PU_M (BIT(3))
#define NRX_VIT_FORCE_PU_V 1
#define NRX_VIT_FORCE_PU_S 3
#define NRX_VIT_FORCE_PD (BIT(2))
#define NRX_VIT_FORCE_PD_M (BIT(2))
#define NRX_VIT_FORCE_PD_V 1
#define NRX_VIT_FORCE_PD_S 2
#define NRX_DEMAP_FORCE_PU (BIT(1))
#define NRX_DEMAP_FORCE_PU_M (BIT(1))
#define NRX_DEMAP_FORCE_PU_V 1
#define NRX_DEMAP_FORCE_PU_S 1
#define NRX_DEMAP_FORCE_PD (BIT(0))
#define NRX_DEMAP_FORCE_PD_M (BIT(0))
#define NRX_DEMAP_FORCE_PD_V 1
#define NRX_DEMAP_FORCE_PD_S 0
