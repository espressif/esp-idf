/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
#include "hal/regi2c_ctrl_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MODEM_CLOCK_SUPPORTED

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
void modem_clock_module_enable(shared_periph_module_t module);

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
void modem_clock_module_disable(shared_periph_module_t module);

/**
 * @brief Reset the mac of modem module
 *
 * @param module  modem module, must be one of
 *    PERIPH_WIFI_MODULE / PERIPH_BT_MODULE /PERIPH_IEEE802154_MODULE
 */
void modem_clock_module_mac_reset(shared_periph_module_t module);

/**
 * @brief Select the modem module lowpower clock source and configure the clock divider
 *
 * @param module  modem module
 * @param src     lowpower clock source
 * @param divider divider value to lowpower clock source
 */
void modem_clock_select_lp_clock_source(shared_periph_module_t module, modem_clock_lpclk_src_t src, uint32_t divider);

/**
 * @brief Disable lowpower clock source selection
 * @param module  modem module
 */
void modem_clock_deselect_lp_clock_source(shared_periph_module_t module);

/**
 * @brief Disable all modem module's lowpower clock source selection
 */
void modem_clock_deselect_all_module_lp_clock_source(void);

/**
 * @brief Gets the clock bitmask associated with the specified modem module.
 *
 * This function returns the complete set of clock-enable bits that correspond
 * to @p module.
 *
 * @param module  Target shared peripheral clock module.
 *
 * @return Bitmask of clock-enable bits for the given module.
 */
uint32_t modem_clock_module_bits_get(shared_periph_module_t module);

#if SOC_WIFI_SUPPORTED
/**
 * @brief Set Wi-Fi initialization status.
 *
 * @param inited Wi-Fi initialization status.
 */
void modem_clock_configure_wifi_status(bool inited);
#endif

#endif // SOC_MODEM_CLOCK_SUPPORTED

#ifdef __cplusplus
}
#endif
