/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// TODO: [ESP32S31] IDF-14785
// RTC IO LL implementation is temporarily bypassed for ESP32-S31
// This file provides stub function declarations to allow the build to succeed.
// Full RTC IO support for ESP32-S31 needs to be implemented in the future.

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/gpio_types.h"
#include "hal/misc.h"
#include "hal/assert.h"

#define RTCIO_LL_PIN_FUNC       1

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RTCIO_LL_FUNC_RTC = 0x0,         /*!< The pin controlled by RTC module. */
    RTCIO_LL_FUNC_DIGITAL = 0x1,     /*!< The pin controlled by DIGITAL module. */
} rtcio_ll_func_t;

typedef enum {
    RTCIO_LL_OUTPUT_NORMAL = 0,    /*!< RTCIO output mode is normal. */
    RTCIO_LL_OUTPUT_OD = 0x1,      /*!< RTCIO output mode is open-drain. */
} rtcio_ll_out_mode_t;

/**
 * @brief Select a RTC IOMUX function for the RTC IO
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param func Function to assign to the pin
 */
static inline void rtcio_ll_iomux_func_sel(int rtcio_num, int func)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)func;
}

/**
 * @brief Enable/Disable LP_GPIO peripheral clock.
 *
 * @param enable true to enable the clock / false to disable the clock
 */
static inline void _rtcio_ll_enable_io_clock(bool enable)
{
    // TODO: [ESP32S31] IDF-14785
    (void)enable;
}

#define rtcio_ll_enable_io_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _rtcio_ll_enable_io_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Select the rtcio function.
 *
 * @note The RTC function must be selected before the pad analog function is enabled.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param func Select pin function.
 */
static inline void rtcio_ll_function_select(int rtcio_num, rtcio_ll_func_t func)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)func;
}

/**
 * @brief Enable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_enable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Disable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_disable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Set RTCIO output level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param level 0: output low; ~0: output high.
 */
static inline void rtcio_ll_set_level(int rtcio_num, uint32_t level)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)level;
}

/**
 * @brief Enable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_enable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Disable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_disable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Get RTCIO input level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return 0: input low; ~0: input high.
 */
static inline uint32_t rtcio_ll_get_level(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    return 0;
}

/**
 * @brief Set RTC GPIO pad drive capability
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param strength Drive capability of the pad. Range: 0 ~ 3.
 */
static inline void rtcio_ll_set_drive_capability(int rtcio_num, uint32_t strength)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)strength;
}

/**
 * @brief Get RTC GPIO pad drive capability.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Drive capability of the pad. Range: 0 ~ 3.
 */
static inline uint32_t rtcio_ll_get_drive_capability(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    return 0;
}

/**
 * @brief Set RTC GPIO pad output mode.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return mode Output mode.
 */
static inline void rtcio_ll_output_mode_set(int rtcio_num, rtcio_ll_out_mode_t mode)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)mode;
}

/**
 * @brief RTC GPIO pullup enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_enable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief RTC GPIO pullup disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_disable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Get RTC GPIO pad pullup status.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Whether the pullup of the pad is enabled or not.
 */
static inline bool rtcio_ll_is_pullup_enabled(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    return false;
}

/**
 * @brief RTC GPIO pulldown enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_enable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief RTC GPIO pulldown disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_disable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Get RTC GPIO pad pulldown status.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Whether the pulldown of the pad is enabled or not.
 */
static inline bool rtcio_ll_is_pulldown_enabled(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    return false;
}

/**
 * @brief Enable force hold function for an RTC IO pad.
 *
 * Enabling HOLD function will cause the pad to lock current status, such as,
 * input/output enable, input/output value, function, drive strength values.
 * This function is useful when going into light or deep sleep mode to prevent
 * the pin configuration from changing.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_hold_enable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Disable hold function on an RTC IO pad
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_hold_disable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Enable force hold function for all RTC IO pads
 *
 * Enabling HOLD function will cause the pad to lock current status, such as,
 * input/output enable, input/output value, function, drive strength values.
 * This function is useful when going into light or deep sleep mode to prevent
 * the pin configuration from changing.
 */
static inline void rtcio_ll_force_hold_all(void)
{
    // TODO: [ESP32S31] IDF-14785
}

/**
 * @brief Disable hold function fon all RTC IO pads
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 */
static inline void rtcio_ll_force_unhold_all(void)
{
    // TODO: [ESP32S31] IDF-14785
}

/**
 * @brief Enable wakeup function and set wakeup type from light sleep or deep sleep for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param type  Wakeup on high level or low level.
 */
static inline void rtcio_ll_wakeup_enable(int rtcio_num, gpio_int_type_t type)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)type;
}

/**
 * @brief Disable wakeup function from light sleep or deep sleep for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_wakeup_disable(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Enable interrupt function and set interrupt type
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param type  Interrupt type on high level or low level.
 */
static inline void rtcio_ll_intr_enable(int rtcio_num, gpio_int_type_t type)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)type;
}

/**
 * @brief Enable rtc io output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_output_in_sleep(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Disable rtc io output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_output_in_sleep(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Enable rtc io input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_input_in_sleep(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Disable rtc io input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_input_in_sleep(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Enable rtc io keep another setting in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_sleep_setting(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Disable rtc io keep another setting in deep sleep. (Default)
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_sleep_setting(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
}

/**
 * @brief Get the status of whether an IO is used for sleep wake-up.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return True if the pin is enabled to wake up from deep-sleep
 */
static inline bool rtcio_ll_wakeup_is_enabled(int rtcio_num)
{
    // TODO: [ESP32S31] IDF-14785
#if SOC_RTCIO_PIN_COUNT > 0
    HAL_ASSERT(rtcio_num >= 0 && rtcio_num < SOC_RTCIO_PIN_COUNT && "io does not support deep sleep wake-up function");
#endif
    (void)rtcio_num;
    return false;
}

/**
 * @brief Get the rtc io interrupt status
 *
 * @return  bit 0~7 corresponding to 0 ~ SOC_RTCIO_PIN_COUNT.
 */
static inline uint32_t rtcio_ll_get_interrupt_status(void)
{
    // TODO: [ESP32S31] IDF-14785
    return 0;
}

/**
 * @brief Clear all LP IO pads status
 */
static inline void rtcio_ll_clear_interrupt_status(void)
{
    // TODO: [ESP32S31] IDF-14785
}

#if SOC_LP_GPIO_MATRIX_SUPPORTED
/**
 * @brief Select signal input from a RTC GPIO
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param signal_idx LP peripheral signal index.
 * @param inv True to invert input signal; False then no invert.
 */
static inline void rtcio_ll_matrix_in(int rtcio_num, int signal_idx, bool inv)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)signal_idx;
    (void)inv;
}

/**
 * @brief Select signal output to a RTC GPIO
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param signal_idx LP peripheral signal index.
 * @param out_inv True to invert output signal; False then no invert.
 * @param oen_inv True to invert output enable signal; False then no invert.
 */
static inline void rtcio_ll_matrix_out(int rtcio_num, int signal_idx, bool out_inv, bool oen_inv)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)signal_idx;
    (void)out_inv;
    (void)oen_inv;
}
#endif // SOC_LP_GPIO_MATRIX_SUPPORTED

#if SOC_RTCIO_WAKE_SUPPORTED
/**
 * @brief Set specific logic level on an RTC IO pin as a ext0 wakeup trigger.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param level Logic level (0: low level trigger, 1: high level trigger)
 */
static inline void rtcio_ll_ext0_set_wakeup_pin(int rtcio_num, int level)
{
    // TODO: [ESP32S31] IDF-14785
    (void)rtcio_num;
    (void)level;
}
#endif // SOC_RTCIO_WAKE_SUPPORTED

#ifdef __cplusplus
}
#endif
