// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SOC_RTC_PERIPH_H
#define _SOC_RTC_PERIPH_H
#include <stdint.h>
#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_gpio_channel.h"
#include "soc/gpio_pins.h"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Pin function information for a single GPIO pad's RTC functions.
 *
 * This is an internal function of the driver, and is not usually useful
 * for external use.
 */
typedef struct {
    uint32_t reg;       /*!< Register of RTC pad, or 0 if not an RTC GPIO */
    uint32_t mux;       /*!< Bit mask for selecting digital pad or RTC pad */
    uint32_t func;      /*!< Shift of pad function (FUN_SEL) field */
    uint32_t ie;        /*!< Mask of input enable */
    uint32_t pullup;    /*!< Mask of pullup enable */
    uint32_t pulldown;  /*!< Mask of pulldown enable */
    uint32_t slpsel;    /*!< If slpsel bit is set, slpie will be used as pad input enabled signal in sleep mode */
    uint32_t slpie;     /*!< Mask of input enable in sleep mode */
    uint32_t hold;      /*!< Mask of hold enable */
    uint32_t hold_force;/*!< Mask of hold_force bit for RTC IO in RTC_CNTL_HOLD_FORCE_REG */
    uint32_t drv_v;     /*!< Mask of drive capability */
    uint32_t drv_s;     /*!< Offset of drive capability */
    int rtc_num;        /*!< RTC IO number, or -1 if not an RTC GPIO */
} rtc_gpio_desc_t;

/**
 * @brief Provides access to a constant table of RTC I/O pin
 * function information.
 *
 * This is an internal function of the driver, and is not usually useful
 * for external use.
 */
extern const rtc_gpio_desc_t rtc_gpio_desc[GPIO_PIN_COUNT];

#ifdef __cplusplus
}
#endif

#endif // _SOC_RTC_PERIPH_H
