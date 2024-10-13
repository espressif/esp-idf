/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "soc/apb_saradc_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PWDET_LL_SAR_POWER_FORCE_BIT BIT(24)
#define PWDET_LL_SAR_POWER_CNTL_BIT  BIT(23)


typedef enum {
    SAR_CTRL_LL_POWER_FSM,     //SAR power controlled by FSM
    SAR_CTRL_LL_POWER_ON,      //SAR power on
    SAR_CTRL_LL_POWER_OFF,     //SAR power off
} sar_ctrl_ll_power_t;

/*---------------------------------------------------------------
                    SAR power control
---------------------------------------------------------------*/
/**
 * @brief Set SAR power mode when controlled by PWDET
 *
 * @param[in] mode  See `sar_ctrl_ll_power_t`
 */
__attribute__((always_inline))
static inline void sar_ctrl_ll_set_power_mode_from_pwdet(sar_ctrl_ll_power_t mode)
{
    if (mode == SAR_CTRL_LL_POWER_FSM) {
        REG_CLR_BIT(PWDET_CONF_REG, PWDET_LL_SAR_POWER_FORCE_BIT);
    } else if (mode == SAR_CTRL_LL_POWER_ON) {
        REG_SET_BIT(PWDET_CONF_REG, PWDET_LL_SAR_POWER_FORCE_BIT);
        REG_SET_BIT(PWDET_CONF_REG, PWDET_LL_SAR_POWER_CNTL_BIT);
    } else if (mode == SAR_CTRL_LL_POWER_OFF) {
        REG_SET_BIT(PWDET_CONF_REG, PWDET_LL_SAR_POWER_FORCE_BIT);
        REG_CLR_BIT(PWDET_CONF_REG, PWDET_LL_SAR_POWER_CNTL_BIT);
    }
}

/**
 * @brief Set SAR power ctrl source
 *
 * @param[in] force  set PWDET as SAR power ctrl source when force is true
 */
static inline void sar_ctrl_ll_force_power_ctrl_from_pwdet(bool force)
{
    APB_SARADC.saradc_ctrl.saradc_saradc2_pwdet_drv = force;
}

#ifdef __cplusplus
}
#endif
