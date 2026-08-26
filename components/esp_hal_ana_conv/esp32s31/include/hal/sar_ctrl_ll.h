/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * SAR related peripherals are interdependent.
 * Related peripherals are:
 * - ADC
 * - PWDET
 *
 * All of above peripherals require SAR to work correctly.
 * As SAR has some registers that will influence above mentioned peripherals.
 * This file gives an abstraction for such registers
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/adc_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SAR_CTRL_LL_POWER_FSM,     //SAR power controlled by FSM
    SAR_CTRL_LL_POWER_ON,      //SAR power on
    SAR_CTRL_LL_POWER_OFF,     //SAR power off
} sar_ctrl_ll_power_t;

/*---------------------------------------------------------------
                    SAR power control
---------------------------------------------------------------*/

/**
 * Set SAR power mode
 *
 * S31 uses xpd_sar1_force / xpd_sar2_force in ADC ctrl registers:
 *   0 = FSM (auto), 2 = force power down, 3 = force power up
 *
 * @param mode  See `sar_ctrl_ll_power_t`
 */
__attribute__((always_inline))
static inline void sar_ctrl_ll_set_power_mode(sar_ctrl_ll_power_t mode)
{
    uint32_t force_val;
    if (mode == SAR_CTRL_LL_POWER_FSM) {
        force_val = 0;
    } else if (mode == SAR_CTRL_LL_POWER_ON) {
        force_val = 3;
    } else {
        force_val = 2;
    }
    ADC.ctrl0.xpd_sar1_force = force_val;
    ADC.ctrl1.xpd_sar2_force = force_val;
}

/**
 * @brief Set SAR power mode when controlled by PWDET
 *        S31 does not have separate PWDET power control, use unified SAR power mode.
 */
static inline void sar_ctrl_ll_set_power_mode_from_pwdet(sar_ctrl_ll_power_t mode)
{
    sar_ctrl_ll_set_power_mode(mode);
}

/**
 * @brief Set SAR power ctrl source
 *        S31 does not have PWDET-based SAR power mux, no-op.
 */
static inline void sar_ctrl_ll_force_power_ctrl_from_pwdet(bool force)
{
    (void)force;
}

#ifdef __cplusplus
}
#endif
