/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for UHCI register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once
#include <stdio.h>
#include "hal/uhci_types.h"
#include "soc/uhci_struct.h"
#include "soc/dport_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock for UHCI module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void _uhci_ll_enable_bus_clock(int group_id, bool enable)
{
    uint32_t reg_val = DPORT_READ_PERI_REG(DPORT_PERIP_CLK_EN_REG);
    if (group_id == 0) {
        reg_val &= ~DPORT_UHCI0_CLK_EN;
        reg_val |= enable << 8;
    } else {
        reg_val &= ~DPORT_UHCI1_CLK_EN;
        reg_val |= enable << 12;
    }
    DPORT_WRITE_PERI_REG(DPORT_PERIP_CLK_EN_REG, reg_val);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define uhci_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _uhci_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the UHCI module
 *
 * @param group_id Group ID
 */
static inline void _uhci_ll_reset_register(int group_id)
{
    if (group_id == 0) {
        DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, DPORT_UHCI0_RST);
        DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, 0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, DPORT_UHCI1_RST);
        DPORT_WRITE_PERI_REG(DPORT_PERIP_RST_EN_REG, 0);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define uhci_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; _uhci_ll_reset_register(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
