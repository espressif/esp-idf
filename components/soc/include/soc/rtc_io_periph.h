/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once


#include "soc/soc.h"
//include soc related (generated) definitions
#include "soc/soc_caps.h"

#if SOC_RTCIO_PIN_COUNT > 0
#include "soc/rtc_io_channel.h"
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
#include "soc/rtc_io_reg.h"
#include "soc/rtc_io_struct.h"
#endif
#endif

#if SOC_ADC_RTC_CTRL_SUPPORTED
#include "soc/sens_struct.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if SOC_RTCIO_PIN_COUNT > 0
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
#endif // SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

/**
 * @brief Provides a constant table to get rtc io number with gpio number
 *
 * This is an internal function of the driver, and is not usually useful
 * for external use.
 */
extern const int rtc_io_num_map[SOC_GPIO_PIN_COUNT];
#endif //SOC_RTCIO_PIN_COUNT > 0

#ifdef __cplusplus
}
#endif
