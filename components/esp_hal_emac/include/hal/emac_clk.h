/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stddef.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_EMAC_SUPPORTED

typedef struct {
    soc_module_clk_t clk_id;
    int32_t step_hz;
    int16_t max_steps;
    const char *clk_name;
} emac_clk_info_t;

typedef struct {
    size_t clk_count;
    const emac_clk_info_t **clk_src;
} emac_clk_internal_info_t;

typedef struct {
    soc_module_clk_t derived_clk_id;
    size_t clk_count;
    const emac_clk_info_t **clk_src;
} emac_clk_phy_ref_info_t;

extern const emac_clk_internal_info_t emac_clk_internal;
extern const emac_clk_phy_ref_info_t emac_clk_phy_ref;

#endif

#ifdef __cplusplus
}
#endif
