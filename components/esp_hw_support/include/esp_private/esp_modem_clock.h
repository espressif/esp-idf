/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "hal/modem_clock_types.h"

#if SOC_MODEM_CLOCK_IS_INDEPENDENT
#include "hal/modem_clock_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the clock of modem module
 *
 * Solve the clock dependency between modem modules, For example, the wifi
 * module depends on the wifi mac, wifi baseband and FE, when wifi module
 * clock is enabled, the wifi MAC, baseband and FE clocks will be enabled
 *
 * @param module  modem module
 */
void modem_clock_module_enable(periph_module_t module);

/**
 * @brief Disable the clock of modem module
 *
 * @param module  modem module
 */
void modem_clock_module_disable(periph_module_t module);

/**
 * @brief Initialize the clock gating control signal of each clock domain of the modem
 *
 * This is a global modem clock gating signal initialization interface, which is
 * only configured during system initialization, and each modem module cannot
 * use this interface.
 */
void modem_clock_domain_pmu_state_icg_map_init(void);

/**
 * @brief Select the modem module lowpower clock source and configure the clock divider
 *
 * @param module  modem module
 * @param src     lowpower clock source
 * @param divider divider value to lowpower clock source
 */
void modem_clock_select_lp_clock_source(periph_module_t module, modem_clock_lpclk_src_t src, uint32_t divider);

/**
 * @brief Disable lowpower clock source selection
 */
void modem_clock_deselect_lp_clock_source(periph_module_t module);

/**
 * @brief Reset wifi mac
 */
void modem_clock_wifi_mac_reset(void);

#ifdef __cplusplus
}
#endif
