/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "hal/modem_clock_types.h"
#include "esp_private/esp_pmu.h"

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
 * This interface and modem_clock_module_disable will jointly maintain the
 * ref_cnt of each device clock source. The ref_cnt indicates how many modules
 * are relying on the clock source. Each enable ops will add 1 to the ref_cnt of
 * the clock source that the module depends on, and only when the ref_cnt of
 * the module is from 0 to 1 will the clock enable be actually configured.
 *
 * !!! Do not use the hal/ll layer interface to configure the clock for the
 * consistency of the hardware state maintained in the driver and the hardware
 * actual state.
 *
 * @param module  modem module
 */
void modem_clock_module_enable(periph_module_t module);

/**
 * @brief Disable the clock of modem module
 *
 * This interface and modem_clock_module_enable will jointly maintain the ref_cnt
 * of each device clock source. The ref_cnt indicates how many modules are relying
 * on the clock source. Each disable ops will minus 1 to the ref_cnt of the clock
 * source that the module depends on, and only when the ref_cnt of the module is
 * from 1 to 0 will the clock disable be actually configured.
 *
 * !!! Do not use the hal/ll layer interface to configure the clock for the
 * consistency of the hardware state maintained in the driver and the hardware
 * actual state.
 *
 * @param module  modem module
 */
void modem_clock_module_disable(periph_module_t module);

/**
 * @brief Reset the mac of modem module
 *
 * @param module  modem module, must be one of
 *    PERIPH_WIFI_MODULE / PERIPH_BT_MODULE /PERIPH_IEEE802154_MODULE
 */
void modem_clock_module_mac_reset(periph_module_t module);

#if SOC_PMU_SUPPORTED
/**
 * @brief Enable modem clock domain clock gate to gate it's output
 *
 * @param domain modem module clock domain
 * @param mode   PMU HP system ACTIVE, MODEM and SLEEP state
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the argument value are not correct
 */
esp_err_t modem_clock_domain_clk_gate_enable(modem_clock_domain_t domain, pmu_hp_icg_modem_mode_t mode);

/**
 * @brief Disable modem clock domain clock gate to ungate it's output
 *
 * @param domain modem module clock domain
 * @param mode   PMU HP system ACTIVE, MODEM and SLEEP state
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the argument value are not correct
 */
esp_err_t modem_clock_domain_clk_gate_disable(modem_clock_domain_t domain, pmu_hp_icg_modem_mode_t mode);
#endif

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
 * @param module  modem module
 */
void modem_clock_deselect_lp_clock_source(periph_module_t module);

/**
* @brief Disable all modem module's lowpower clock source selection
 */
void modem_clock_deselect_all_module_lp_clock_source(void);

/**
 * @brief Reset wifi mac
 */
void modem_clock_wifi_mac_reset(void);

/**
 * @brief Enable clock registers which shared by both modem and ADC. Need a ref count to enable/disable them
 *
 * @param enable true: enable; false: disable
 */
void modem_clock_shared_enable(bool enable);

#ifdef __cplusplus
}
#endif
