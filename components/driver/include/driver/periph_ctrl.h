// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PERIPH_LEDC_MODULE = 0,
    PERIPH_UART0_MODULE,
    PERIPH_UART1_MODULE,
    PERIPH_UART2_MODULE,
    PERIPH_I2C0_MODULE,
    PERIPH_I2C1_MODULE,
    PERIPH_I2S0_MODULE,
    PERIPH_I2S1_MODULE,
    PERIPH_TIMG0_MODULE,
    PERIPH_TIMG1_MODULE,
    PERIPH_PWM0_MODULE,
    PERIPH_PWM1_MODULE,
    PERIPH_PWM2_MODULE,
    PERIPH_PWM3_MODULE,
    PERIPH_UHCI0_MODULE,
    PERIPH_UHCI1_MODULE,
    PERIPH_RMT_MODULE,
    PERIPH_PCNT_MODULE,
    PERIPH_SPI_MODULE,
    PERIPH_HSPI_MODULE,
    PERIPH_VSPI_MODULE,
    PERIPH_SPI_DMA_MODULE,
    PERIPH_SDMMC_MODULE,
    PERIPH_SDIO_SLAVE_MODULE,
    PERIPH_CAN_MODULE,
    PERIPH_EMAC_MODULE,
    PERIPH_RNG_MODULE,
    PERIPH_WIFI_MODULE,
    PERIPH_BT_MODULE,
    PERIPH_WIFI_BT_COMMON_MODULE,
} periph_module_t;

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


#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_PERIPH_CTRL_H_ */
