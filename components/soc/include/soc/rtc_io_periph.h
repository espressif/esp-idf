// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "soc/soc.h"
//include soc related (generated) definitions
#include "soc/soc_caps.h"

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
#include "soc/rtc_io_channel.h"
#include "soc/rtc_io_reg.h"
#include "soc/rtc_io_struct.h"
#endif

#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"

#if SOC_ADC_SUPPORT_RTC_CTRL
#include "soc/sens_struct.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

/**
 * @brief Pin function information for a single RTCIO pad's.
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
    uint32_t slpoe;     /*!< Mask of output enable in sleep mode */
    uint32_t hold;      /*!< Mask of hold enable */
    uint32_t hold_force;/*!< Mask of hold_force bit for RTC IO in RTC_CNTL_HOLD_REG */
    uint32_t drv_v;     /*!< Mask of drive capability */
    uint32_t drv_s;     /*!< Offset of drive capability */
    int rtc_num;        /*!< GPIO number (corresponds to RTC pad) */
} rtc_io_desc_t;

/**
 * @brief Provides access to a constant table of RTC I/O pin
 * function information.
 * The index of table is the index of rtcio.
 *
 * This is an internal function of the driver, and is not usually useful
 * for external use.
 */
extern const rtc_io_desc_t rtc_io_desc[SOC_RTCIO_PIN_COUNT];

/**
 * @brief Provides a constant table to get rtc io number with gpio number
 *
 * This is an internal function of the driver, and is not usually useful
 * for external use.
 */
extern const int rtc_io_num_map[SOC_GPIO_PIN_COUNT];

#ifdef CONFIG_RTCIO_SUPPORT_RTC_GPIO_DESC
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
extern const rtc_gpio_desc_t rtc_gpio_desc[SOC_GPIO_PIN_COUNT];

#endif // CONFIG_RTCIO_SUPPORT_RTC_GPIO_DESC

#endif // SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

#ifdef __cplusplus
}
#endif
