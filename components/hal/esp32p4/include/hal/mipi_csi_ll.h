/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/hal_utils.h"
#include "hal/mipi_csi_brg_ll.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                      CSI Bridge
---------------------------------------------------------------*/
/**
 * @brief Enable the bus clock for CSI Bridge module
 *
 * @param csi_bridge_id    CSI Bridge ID
 * @param en               enable / disable
 */
static inline void mipi_csi_ll_enable_brg_module_clock(int csi_bridge_id, bool en)
{
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_csi_brg_sys_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_enable_brg_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_csi_ll_enable_brg_module_clock(__VA_ARGS__)

/**
 * @brief Reset the CSI Bridge module
 *
 * @param csi_bridge_id    CSI Bridge ID
 */
static inline void mipi_csi_ll_reset_brg_module_clock(int csi_bridge_id)
{
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_brg = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_brg = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_reset_brg_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_csi_ll_reset_brg_module_clock(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
