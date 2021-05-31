/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _DRIVER_PERIPH_CTRL_H_
#define _DRIVER_PERIPH_CTRL_H_

#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      enable peripheral module
 *
 * @param[in]  periph    :  Peripheral module name
 *
 * Clock for the module will be ungated, and reset de-asserted.
 *
 * @note If periph_module_enable is called a number of times,
 *       periph_module_disable has to be called the same number of times
 *       in order to put the peripheral into disabled state.
 *
 * @return     NULL
 *
 */
void periph_module_enable(periph_module_t periph);

/**
 * @brief      disable peripheral module
 *
 * @param[in]  periph    :  Peripheral module name
 *
 * Clock for the module will be gated, reset asserted.
 *
 * @note If periph_module_enable is called a number of times,
 *       periph_module_disable has to be called the same number of times
 *       in order to put the peripheral into disabled state.
 *
 * @return     NULL
 *
 */
void periph_module_disable(periph_module_t periph);

/**
 * @brief      reset peripheral module
 *
 * @param[in]  periph    :  Peripheral module name
 *
 * Reset will asserted then de-assrted for the peripheral.
 *
 * Calling this function does not enable or disable the clock for the module.
 *
 * @return     NULL
 *
 */
void periph_module_reset(periph_module_t periph);

/**
 * @brief      enable wifi bt common module
 *
 * @note If wifi_bt_common_module_enable is called a number of times,
 *       wifi_bt_common_module_disable has to be called the same number of times
 *       in order to put the peripheral into disabled state.
 *
 * @return     NULL
 *
 */
void wifi_bt_common_module_enable(void);

/**
 * @brief      disable wifi bt common module
 *
 * @note If wifi_bt_common_module_enable is called a number of times,
 *       wifi_bt_common_module_disable has to be called the same number of times
 *       in order to put the peripheral into disabled state.
 *
 * @return     NULL
 *
 */
void wifi_bt_common_module_disable(void);

/**
 * @brief      enable wifi module
 *
 * @note Enable wifi module only.
 *
 * @return     NULL
 *
 */
void wifi_module_enable(void);

/**
 * @brief      disable wifi module
 *
 * @note Disable wifi module only.
 *
 * @return     NULL
 *
 */
void wifi_module_disable(void);
#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_PERIPH_CTRL_H_ */
