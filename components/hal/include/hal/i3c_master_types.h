/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I3C_MASTER_SUPPORTED
/**
 * @brief I2C group clock source
 */
typedef soc_periph_i3c_master_clk_src_t i3c_master_clock_source_t;
#else
/**
 * @brief default type
 */
typedef int                             i3c_master_clock_source_t;
#endif

#ifdef __cplusplus
}
#endif
