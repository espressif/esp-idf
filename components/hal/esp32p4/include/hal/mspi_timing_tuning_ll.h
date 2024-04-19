/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/assert.h"
#include "soc/soc.h"
#include "soc/iomux_mspi_pin_reg.h"
#include "soc/iomux_mspi_pin_struct.h"
#include "soc/hp_sys_clkrst_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MSPI DQS ID
 */
typedef enum {
    MSPI_LL_DQS_ID_0,
    MSPI_LL_DQS_ID_1,
} mspi_ll_dqs_id_t;

/**
 * MSPI DQS Phase
 */
typedef enum {
    MSPI_LL_DQS_PHASE_67_5,
    MSPI_LL_DQS_PHASE_78_75,
    MSPI_LL_DQS_PHASE_90,
    MSPI_LL_DQS_PHASE_101_25,
    MSPI_LL_DQS_PHASE_MAX,
} mspi_ll_dqs_phase_t;

/**
 * MSPI Delayline
 *
 * @note The Sequence of these enums should not be changed
 */
typedef enum {
    MSPI_LL_PIN_D = 0,
    MSPI_LL_PIN_Q,
    MSPI_LL_PIN_WP,
    MSPI_LL_PIN_HD,
    MSPI_LL_PIN_D4,
    MSPI_LL_PIN_D5,
    MSPI_LL_PIN_D6,
    MSPI_LL_PIN_D7,
    MSPI_LL_PIN_DQS0,
    MSPI_LL_PIN_CLK,
    MSPI_LL_PIN_CS,
    MSPI_LL_PIN_D8,
    MSPI_LL_PIN_D9,
    MSPI_LL_PIN_D10,
    MSPI_LL_PIN_D11,
    MSPI_LL_PIN_D12,
    MSPI_LL_PIN_D13,
    MSPI_LL_PIN_D14,
    MSPI_LL_PIN_D15,
    MSPI_LL_PIN_DQS1,
    MSPI_LL_PIN_MAX,
} mspi_ll_pin_t;

/**
 * Reset the MSPI clock
 */
__attribute__((always_inline))
static inline void _mspi_timing_ll_reset_mspi(void)
{
   REG_SET_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI);
   REG_CLR_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define mspi_timing_ll_reset_mspi(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _mspi_timing_ll_reset_mspi(__VA_ARGS__)

/**
 * Set all MSPI DQS phase
 *
 * @param dqs_id  DQS ID
 * @param phase   Phase
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_dqs_phase(mspi_ll_dqs_id_t dqs_id, mspi_ll_dqs_phase_t phase)
{
    HAL_ASSERT(dqs_id < 2);
    if (dqs_id == MSPI_LL_DQS_ID_0) {
        REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_PHASE, phase);
    } else {
        REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_1_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_1_PHASE, phase);
    }
}

/**
 * Set all MSPI delayline
 *
 * @param pin        Pin
 * @param delayline  Delayline
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_delayline(mspi_ll_pin_t pin, uint8_t delayline)
{
    HAL_ASSERT(pin < MSPI_LL_PIN_MAX);

    switch (pin) {
    case MSPI_LL_PIN_DQS0:
        MSPI_IOMUX.psram_pin_group.dqs0.reg_psram_dqs_delay_90 = delayline;
        MSPI_IOMUX.psram_pin_group.dqs0.reg_psram_dqs_delay_270 = delayline;
        break;
    case MSPI_LL_PIN_DQS1:
        MSPI_IOMUX.psram_pin_group.dqs1.reg_psram_dqs_delay_90 = delayline;
        MSPI_IOMUX.psram_pin_group.dqs1.reg_psram_dqs_delay_270 = delayline;
        break;
    case MSPI_LL_PIN_D:
    case MSPI_LL_PIN_Q:
    case MSPI_LL_PIN_WP:
    case MSPI_LL_PIN_HD:
    case MSPI_LL_PIN_D4:
    case MSPI_LL_PIN_D5:
    case MSPI_LL_PIN_D6:
    case MSPI_LL_PIN_D7:
        MSPI_IOMUX.psram_pin_group.pin_group0[pin].reg_psram_pin_dlc = delayline;
        break;
    case MSPI_LL_PIN_CLK:
    case MSPI_LL_PIN_CS:
    case MSPI_LL_PIN_D8:
    case MSPI_LL_PIN_D9:
    case MSPI_LL_PIN_D10:
    case MSPI_LL_PIN_D11:
    case MSPI_LL_PIN_D12:
    case MSPI_LL_PIN_D13:
    case MSPI_LL_PIN_D14:
    case MSPI_LL_PIN_D15:
        pin = (mspi_ll_pin_t)(pin - MSPI_LL_PIN_CLK);
        MSPI_IOMUX.psram_pin_group.pin_group1[pin].reg_psram_pin_dlc = delayline;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * Enable DQS
 *
 * @param en  Enable/disable
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_enable_dqs(bool en)
{
    if (en) {
        REG_SET_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_XPD);
        REG_SET_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_1_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_1_XPD);
    } else {
        REG_CLR_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_XPD);
        REG_CLR_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_1_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_1_XPD);
    }
}


/**
 * Set all MSPI pin drive
 *
 * @param drv  Pin drive
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_pin_drv_set(uint8_t drv)
{
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_D_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_Q_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_Q_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_WP_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_WP_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_HOLD_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_HOLD_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ4_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ4_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ5_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ5_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ6_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ6_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ7_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ7_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ8_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ8_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ9_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ9_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ10_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ10_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ11_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ11_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ12_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ12_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ13_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ13_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ14_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ14_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ15_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ15_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_1_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_1_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_CK_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_CK_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_CS_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_CS_DRV, drv);
}

#ifdef __cplusplus
}
#endif
