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
#include "soc/lp_adc_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

//TODO: IDF-7460
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
 * Set SAR power mode
 *
 * @param mode  See `sar_ctrl_ll_power_t`
 */
__attribute__((always_inline))
static inline void sar_ctrl_ll_set_power_mode(sar_ctrl_ll_power_t mode)
{
    if (mode == SAR_CTRL_LL_POWER_FSM) {
        LP_ADC.force_wpd_sar.force_xpd_sar1 = 0x0;
        LP_ADC.force_wpd_sar.force_xpd_sar2 = 0x0;
    } else if (mode == SAR_CTRL_LL_POWER_ON) {
        LP_ADC.force_wpd_sar.force_xpd_sar1 = 0x3;
        LP_ADC.force_wpd_sar.force_xpd_sar2 = 0x3;
    } else {
        LP_ADC.force_wpd_sar.force_xpd_sar1 = 0x2;
        LP_ADC.force_wpd_sar.force_xpd_sar2 = 0x2;
    }
}

/**
 * @brief Set SAR power mode when controlled by PWDET
 *
 * @param[in] mode  See `sar_ctrl_ll_power_t`
 */
static inline void sar_ctrl_ll_set_power_mode_from_pwdet(sar_ctrl_ll_power_t mode)
{
}

/**
 * @brief Set SAR power ctrl source
 *
 * @param[in] force  set PWDET as SAR power ctrl source when force is true
 */
static inline void sar_ctrl_ll_force_power_ctrl_from_pwdet(bool force)
{
}

#ifdef __cplusplus
}
#endif
