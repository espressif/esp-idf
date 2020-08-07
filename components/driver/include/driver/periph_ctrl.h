// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DRIVER_PERIPH_CTRL_H_
#define _DRIVER_PERIPH_CTRL_H_

#include "esp_err.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
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

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_PERIPH_CTRL_H_ */
