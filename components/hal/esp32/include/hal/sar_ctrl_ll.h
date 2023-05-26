/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "soc/sens_struct.h"

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
 * @param mode  See `sar_ctrl_ll_power_t`
 */
__attribute__((always_inline))
static inline void sar_ctrl_ll_set_power_mode(sar_ctrl_ll_power_t mode)
{
    if (mode == SAR_CTRL_LL_POWER_FSM) {
        SENS.sar_meas_wait2.force_xpd_sar = 0x0;
    } else if (mode == SAR_CTRL_LL_POWER_ON) {
        SENS.sar_meas_wait2.force_xpd_sar = 0x3;
    } else {
        SENS.sar_meas_wait2.force_xpd_sar = 0x2;
    }
}


#ifdef __cplusplus
}
#endif
