/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/emac_clk.h"

/* MPLL VCO frequency step for targets that program MPLL from the 40 MHz XTAL (see clk_ll_mpll_set_*). */
#define EMAC_MPLL_GRID_STEP_HZ      (20000000)
#define EMAC_MPLL_GRID_MAX_STEPS    (32)

static const emac_clk_info_t emac_clk_mpll = {
    .clk_id = SOC_MOD_CLK_MPLL,
    .step_hz = EMAC_MPLL_GRID_STEP_HZ,
    .max_steps = EMAC_MPLL_GRID_MAX_STEPS,
    .clk_name = "MPLL",
};

const emac_clk_internal_info_t emac_clk_internal = {
    .clk_count = 0,
    .clk_src = NULL,
};

const emac_clk_info_t *emac_clk_phy_ref_src[] = {
    &emac_clk_mpll,
};

const emac_clk_phy_ref_info_t emac_clk_phy_ref = {
    .derived_clk_id = SOC_MOD_CLK_PLL_F50M,
    .clk_count = sizeof(emac_clk_phy_ref_src) / sizeof(emac_clk_phy_ref_src[0]),
    .clk_src = emac_clk_phy_ref_src,
};
