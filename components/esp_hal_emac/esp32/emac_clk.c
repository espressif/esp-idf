/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/emac_clk.h"

static const emac_clk_info_t emac_clk_apll = {
    .clk_id = SOC_MOD_CLK_APLL,
    .step_hz = -1,
    .max_steps = -1,
    .clk_name = "APLL",
};

static const emac_clk_info_t *emac_clk_internal_src[] = {
    &emac_clk_apll,
};

const emac_clk_internal_info_t emac_clk_internal = {
    .clk_count = sizeof(emac_clk_internal_src) / sizeof(emac_clk_internal_src[0]),
    .clk_src = emac_clk_internal_src,
};

/* ESP32 does not support EMAC PHY_REF_CLK derived clock. */
const emac_clk_phy_ref_info_t emac_clk_phy_ref = {
    .derived_clk_id = SOC_MOD_CLK_INVALID,
    .clk_count = 0,
    .clk_src = NULL,
};
