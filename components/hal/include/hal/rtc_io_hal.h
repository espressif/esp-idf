// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for RTC IO master (common part)

#pragma once

#include <esp_err.h>
#if !CONFIG_IDF_TARGET_ESP32C3
#include "soc/soc_caps.h"
#include "hal/rtc_io_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
/**
 * Select the rtcio function.
 *
 * @note The RTC function must be selected before the pad analog function is enabled.
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param func Select pin function.
 */
#define rtcio_hal_function_select(rtcio_num, func) rtcio_ll_function_select(rtcio_num, func)

/**
 * Enable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_output_enable(rtcio_num) rtcio_ll_output_enable(rtcio_num)

/**
 * Disable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_output_disable(rtcio_num) rtcio_ll_output_disable(rtcio_num)

/**
 * Set RTCIO output level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param level 0: output low; ~0: output high.
 */
#define rtcio_hal_set_level(rtcio_num, level) rtcio_ll_set_level(rtcio_num, level)

/**
 * Enable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_input_enable(rtcio_num) rtcio_ll_input_enable(rtcio_num)

/**
 * Disable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_input_disable(rtcio_num) rtcio_ll_input_disable(rtcio_num)

/**
 * Get RTCIO input level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @return 0: input low; ~0: input high.
 */
#define rtcio_hal_get_level(rtcio_num) rtcio_ll_get_level(rtcio_num)

/**
 * @brief Set RTC GPIO pad drive capability.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param strength Drive capability of the pad. Range: 0 ~ 3.
 */
#define rtcio_hal_set_drive_capability(rtcio_num, strength) rtcio_ll_set_drive_capability(rtcio_num, strength)

/**
 * @brief Get RTC GPIO pad drive capability.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @return Drive capability of the pad. Range: 0 ~ 3.
 */
#define rtcio_hal_get_drive_capability(rtcio_num) rtcio_ll_get_drive_capability(rtcio_num)

/**
 * Set RTCIO output level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param level 0: output low; ~0: output high.
 */
#define rtcio_hal_set_level(rtcio_num, level) rtcio_ll_set_level(rtcio_num, level)

/**
 * Get RTCIO input level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @return 0: input low; ~0: input high.
 */
#define rtcio_hal_get_level(rtcio_num) rtcio_ll_get_level(rtcio_num)

/**
 * Set RTC IO direction.
 *
 * Configure RTC IO direction, such as output only, input only,
 * output and input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param mode IO direction.
 */
void rtcio_hal_set_direction(int rtcio_num, rtc_gpio_mode_t mode);

/**
 * Set RTC IO direction in deep sleep or disable sleep status.
 *
 * NOTE: ESP32 support INPUT_ONLY mode.
 *       ESP32S2 support INPUT_ONLY, OUTPUT_ONLY, INPUT_OUTPUT mode.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param mode IO direction.
 */
void rtcio_hal_set_direction_in_sleep(int rtcio_num, rtc_gpio_mode_t mode);

/**
 * RTC GPIO pullup enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_pullup_enable(rtcio_num) rtcio_ll_pullup_enable(rtcio_num)

/**
 * RTC GPIO pullup disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_pullup_disable(rtcio_num) rtcio_ll_pullup_disable(rtcio_num)

/**
 * RTC GPIO pulldown enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_pulldown_enable(rtcio_num) rtcio_ll_pulldown_enable(rtcio_num)

/**
 * RTC GPIO pulldown disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_pulldown_disable(rtcio_num) rtcio_ll_pulldown_disable(rtcio_num)

#endif // SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

#if SOC_RTCIO_HOLD_SUPPORTED

/**
 * Enable force hold function for RTC IO pad.
 *
 * Enabling HOLD function will cause the pad to lock current status, such as,
 * input/output enable, input/output value, function, drive strength values.
 * This function is useful when going into light or deep sleep mode to prevent
 * the pin configuration from changing.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_hold_enable(rtcio_num) rtcio_ll_force_hold_enable(rtcio_num)

/**
 * Disable hold function on an RTC IO pad
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_hold_disable(rtcio_num) rtcio_ll_force_hold_disable(rtcio_num)

/**
 * Enable force hold function for RTC IO pads.
 *
 * Enabling HOLD function will cause the pad to lock current status, such as,
 * input/output enable, input/output value, function, drive strength values.
 * This function is useful when going into light or deep sleep mode to prevent
 * the pin configuration from changing.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_hold_all() rtcio_ll_force_hold_all()

/**
 * Disable hold function on an RTC IO pads.
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_unhold_all() rtcio_ll_force_unhold_all()
#endif // SOC_RTCIO_HOLD_SUPPORTED

#if SOC_RTCIO_WAKE_SUPPORTED

/**
 * Enable wakeup function and set wakeup type from light sleep status for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param type  Wakeup on high level or low level.
 */
#define rtcio_hal_wakeup_enable(rtcio_num, type) rtcio_ll_wakeup_enable(rtcio_num, type)

/**
 * Disable wakeup function from light sleep status for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_wakeup_disable(rtcio_num) rtcio_ll_wakeup_disable(rtcio_num)

/**
 * Set specific logic level on an RTC IO pin as a wakeup trigger.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param level Logic level (0)
 */
#define rtcio_hal_ext0_set_wakeup_pin(rtcio_num, level)     rtcio_ll_ext0_set_wakeup_pin(rtcio_num, level)

#endif

#if SOC_RTCIO_HOLD_SUPPORTED || SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

/**
 * Helper function to disconnect internal circuits from an RTC IO
 * This function disables input, output, pullup, pulldown, and enables
 * hold feature for an RTC IO.
 * Use this function if an RTC IO needs to be disconnected from internal
 * circuits in deep sleep, to minimize leakage current.
 *
 * In particular, for ESP32-WROVER module, call
 * rtc_gpio_isolate(GPIO_NUM_12) before entering deep sleep, to reduce
 * deep sleep current.
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
void rtcio_hal_isolate(int rtc_num);

#endif

#ifdef __cplusplus
}
#endif
