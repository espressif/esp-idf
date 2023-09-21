/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for RTC IO master (common part)

#pragma once

#include <esp_err.h>
#include "sdkconfig.h"

#include "soc/soc_caps.h"
#if SOC_RTCIO_PIN_COUNT > 0
#include "soc/rtc_io_periph.h"
#include "hal/rtc_io_ll.h"
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
#include "hal/rtc_io_types.h"
#endif
#endif //SOC_RTCIO_PIN_COUNT > 0

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_RTCIO_PIN_COUNT > 0
/**
 * Select the rtcio function.
 *
 * @note The RTC function must be selected before the pad analog function is enabled.
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param func Select pin function.
 */
#define rtcio_hal_function_select(rtcio_num, func) rtcio_ll_function_select(rtcio_num, func)

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
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
 * NOTE: ESP32 supports INPUT_ONLY mode.
 *       The rest targets support INPUT_ONLY, OUTPUT_ONLY, INPUT_OUTPUT mode.
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

/**
 * Select a RTC IOMUX function for the RTC IO
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param func Function to assign to the pin
 */
#define rtcio_hal_iomux_func_sel(rtcio_num, func) rtcio_ll_iomux_func_sel(rtcio_num, func)

#if SOC_LP_GPIO_MATRIX_SUPPORTED
/**
 * Select RTC GPIO input to a signal
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param signal_idx LP peripheral signal index.
 * @param inv inv True to invert input signal; False then no invert.
 */
#define rtcio_hal_matrix_in(rtcio_num, signal_idx, inv) rtcio_ll_matrix_in(rtcio_num, signal_idx, inv)

/**
 * Select signal output to a RTC GPIO
 *
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @param signal_idx LP peripheral signal index.
 * @param out_inv True to invert output signal; False then no invert.
 * @param oen_inv True to invert output enable signal; False then no invert.
 */
#define rtcio_hal_matrix_out(rtcio_num, signal_idx, out_inv, oen_inv) rtcio_ll_matrix_out(rtcio_num, signal_idx, out_inv, oen_inv)
#endif // SOC_LP_GPIO_MATRIX_SUPPORTED

#endif // SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

#if SOC_RTCIO_HOLD_SUPPORTED

/**
 * Enable force hold function on an RTC IO pad.
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
 * Disable hold function on an RTC IO pad.
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_hold_disable(rtcio_num) rtcio_ll_force_hold_disable(rtcio_num)

/**
 * Enable force hold function on all RTC IO pads.
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
 * Disable hold function on all RTC IO pads.
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

#if SOC_RTCIO_HOLD_SUPPORTED && SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

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

#endif //SOC_RTCIO_PIN_COUNT > 0

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP && (SOC_RTCIO_PIN_COUNT > 0)

#define gpio_hal_deepsleep_wakeup_enable(hal, gpio_num, intr_type)  rtcio_hal_wakeup_enable(rtc_io_num_map[gpio_num], intr_type)
#define gpio_hal_deepsleep_wakeup_disable(hal, gpio_num)            rtcio_hal_wakeup_disable(rtc_io_num_map[gpio_num])
#define gpio_hal_deepsleep_wakeup_is_enabled(hal, gpio_num)         rtcio_hal_wakeup_is_enabled(rtc_io_num_map[gpio_num])
#define rtc_hal_gpio_get_wakeup_status()                            rtcio_hal_get_interrupt_status()
#define rtc_hal_gpio_clear_wakeup_status()                          rtcio_hal_clear_interrupt_status()

/**
 * @brief Get the status of whether an IO is used for sleep wake-up.
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param rtcio_num The index of rtcio. 0 ~ SOC_RTCIO_PIN_COUNT.
 * @return True if the pin is enabled to wake up from deep-sleep
 */
#define rtcio_hal_wakeup_is_enabled(rtcio_num) rtcio_ll_wakeup_is_enabled(rtcio_num)

/**
 * @brief Get the rtc io interrupt status
 *
 * @return  bit 0~7 corresponding to 0 ~ SOC_RTCIO_PIN_COUNT.
 */
#define rtcio_hal_get_interrupt_status()        rtcio_ll_get_interrupt_status()

/**
 * @brief Clear all LP IO pads status
 */
#define rtcio_hal_clear_interrupt_status()      rtcio_ll_clear_interrupt_status()

#endif //SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP && (SOC_RTCIO_PIN_COUNT > 0)

#ifdef __cplusplus
}
#endif
