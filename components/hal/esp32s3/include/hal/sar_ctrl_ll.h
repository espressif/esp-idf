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
 * - Temp Sensor
 *
 * All of above peripherals require SAR to work correctly.
 * As SAR has some registers that will influence above mentioned peripherals.
 * This file gives an abstraction for such registers
 */

#pragma once

#include <stdlib.h>
#include "soc/soc.h"
#include "soc/sens_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PWDET_CONF_REG        0x6000E060
#define PWDET_SAR_POWER_FORCE BIT(7)
#define PWDET_SAR_POWER_CNTL  BIT(6)


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
        SENS.sar_peri_clk_gate_conf.saradc_clk_en = 1;
        SENS.sar_power_xpd_sar.force_xpd_sar = 0x0;
    } else if (mode == SAR_CTRL_LL_POWER_ON) {
        SENS.sar_peri_clk_gate_conf.saradc_clk_en = 1;
        SENS.sar_power_xpd_sar.force_xpd_sar = 0x3;
    } else {
        SENS.sar_peri_clk_gate_conf.saradc_clk_en = 0;
        SENS.sar_power_xpd_sar.force_xpd_sar = 0x2;
    }
}

/**
 * @brief Set SAR power mode when controlled by PWDET
 *
 * @param[in] mode  See `sar_ctrl_ll_power_t`
 */
static inline void sar_ctrl_ll_set_power_mode_from_pwdet(sar_ctrl_ll_power_t mode)
{
    if (mode == SAR_CTRL_LL_POWER_FSM) {
        REG_CLR_BIT(PWDET_CONF_REG, PWDET_SAR_POWER_FORCE);
    } else if (mode == SAR_CTRL_LL_POWER_ON) {
        REG_SET_BIT(PWDET_CONF_REG, PWDET_SAR_POWER_FORCE);
        REG_SET_BIT(PWDET_CONF_REG, PWDET_SAR_POWER_CNTL);
    } else if (mode == SAR_CTRL_LL_POWER_OFF) {
        REG_SET_BIT(PWDET_CONF_REG, PWDET_SAR_POWER_FORCE);
        REG_CLR_BIT(PWDET_CONF_REG, PWDET_SAR_POWER_CNTL);
    }
}


#ifdef __cplusplus
}
#endif
