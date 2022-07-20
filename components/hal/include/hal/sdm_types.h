/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SDM_SUPPORTED
typedef soc_periph_sdm_clk_src_t sdm_clock_source_t; // sigma delta modulator clock source
#else
typedef int sdm_clock_source_t; // sigma delta modulator clock source, fallback to integer type
#endif // SOC_SDM_SUPPORTED

#ifdef __cplusplus
}
#endif
