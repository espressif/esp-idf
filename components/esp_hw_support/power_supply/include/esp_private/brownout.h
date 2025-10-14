/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the brownout detection system
 *
 * This function configures and enables the brownout detection hardware, which monitors
 * the power supply voltage and triggers appropriate system actions if the voltage
 * drops below a predefined threshold. Brownout detection helps ensure the stability
 * and reliability of the system under low-voltage conditions.
 */
void esp_brownout_init(void);

/**
 * @brief Disable the brownout detection system
 *
 * This function disables the brownout detection hardware, stopping voltage monitoring
 * and associated system actions. Use this function with caution, as disabling brownout
 * detection may increase the risk of system instability or malfunction under low-voltage
 * conditions.
 */
void esp_brownout_disable(void);

#ifdef __cplusplus
}
#endif
