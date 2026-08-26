/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/emac_clk.h"

/* MPLL VCO frequency step for targets that program MPLL from the 40 MHz XTAL (see clk_ll_mpll_set_*). */
#define EMAC_MPLL_GRID_STEP_HZ      (20000000)
#define EMAC_MPLL_GRID_MAX_STEPS    (32)

/* CPLL grid: f = 40 MHz XTAL * fb_div / ref_div (clk_ll_cpll_*); model fb_div 1..N with ref_div 1 => 40 MHz steps.
 * Max CPLL 320 MHz => N = 8. */
#define EMAC_CPLL_GRID_STEP_HZ      (40000000)
#define EMAC_CPLL_GRID_MAX_STEPS    (8)

static const emac_clk_info_t emac_clk_cpll = {
    .clk_id = SOC_MOD_CLK_CPLL,
    .step_hz = EMAC_CPLL_GRID_STEP_HZ,
    .max_steps = EMAC_CPLL_GRID_MAX_STEPS,
    .clk_name = "CPLL",
};

static const emac_clk_info_t emac_clk_mpll = {
    .clk_id = SOC_MOD_CLK_MPLL,
    .step_hz = EMAC_MPLL_GRID_STEP_HZ,
    .max_steps = EMAC_MPLL_GRID_MAX_STEPS,
    .clk_name = "MPLL",
};

static const emac_clk_info_t emac_clk_apll = {
    .clk_id = SOC_MOD_CLK_APLL,
    .step_hz = -1,
    .max_steps = -1,
    .clk_name = "APLL",
};

// Order matters! Position can be used to determine the clock source selection
static const emac_clk_info_t *emac_clk_internal_src[] = {
    &emac_clk_mpll,
    &emac_clk_cpll,
    &emac_clk_apll,
};

const emac_clk_internal_info_t emac_clk_internal = {
    .clk_count = sizeof(emac_clk_internal_src) / sizeof(emac_clk_internal_src[0]),
    .clk_src = emac_clk_internal_src,
};

// Order matters! Position can be used to determine the clock source selection
const emac_clk_info_t *emac_clk_phy_ref_src[] = {
    &emac_clk_cpll,
    &emac_clk_mpll,
};

const emac_clk_phy_ref_info_t emac_clk_phy_ref = {
    .derived_clk_id = SOC_MOD_CLK_PLL_F50M,
    .clk_count = sizeof(emac_clk_phy_ref_src) / sizeof(emac_clk_phy_ref_src[0]),
    .clk_src = emac_clk_phy_ref_src,
};
