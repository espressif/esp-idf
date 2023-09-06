/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Reset and Clock Control APIs
 * @{
 */

/**
 * @brief Acquire the RCC lock for a peripheral module
 *
 * @note User code protected by this macro should be as short as possible, because it's a critical section
 * @note This macro will increase the reference lock of that peripheral.
 *       You can get the value before the increment from the `rc_name` local variable
 */
#define PERIPH_RCC_ACQUIRE_ATOMIC(periph, rc_name)                \
    for (uint8_t rc_name, i = 1, __DECLARE_RCC_RC_ATOMIC_ENV;     \
         i ? (rc_name = periph_rcc_acquire_enter(periph), 1) : 0; \
         periph_rcc_acquire_exit(periph, rc_name), i--)

/**
 * @brief Release the RCC lock for a peripheral module
 *
 * @note User code protected by this macro should be as short as possible, because it's a critical section
 * @note This macro will decrease the reference lock of that peripheral.
 *       You can get the value after the decrease from the `rc_name` local variable
 */
#define PERIPH_RCC_RELEASE_ATOMIC(periph, rc_name)                \
    for (uint8_t rc_name, i = 1, __DECLARE_RCC_RC_ATOMIC_ENV;     \
         i ? (rc_name = periph_rcc_release_enter(periph), 1) : 0; \
         periph_rcc_release_exit(periph, rc_name), i--)

/**
 * @brief A simplified version of `PERIPH_RCC_ACQUIRE/RELEASE_ATOMIC`, without a reference count
 *
 * @note User code protected by this macro should be as short as possible, because it's a critical section
 */
#define PERIPH_RCC_ATOMIC()                   \
    for (int i = 1, __DECLARE_RCC_ATOMIC_ENV; \
         i ? (periph_rcc_enter(), 1) : 0;     \
         periph_rcc_exit(), i--)

/** @cond */
// The following functions are not intended to be used directly by the developers
uint8_t periph_rcc_acquire_enter(periph_module_t periph);
void periph_rcc_acquire_exit(periph_module_t periph, uint8_t ref_count);
uint8_t periph_rcc_release_enter(periph_module_t periph);
void periph_rcc_release_exit(periph_module_t periph, uint8_t ref_count);
void periph_rcc_enter(void);
void periph_rcc_exit(void);
/** @endcond */

/**
 * @}
 */

/*************************************************************************************************************
 * @note The following APIs are no longer supported since ESP32P4, please use the RCC lock macros instead.
 *************************************************************************************************************/
/**
 * @brief Enable peripheral module by un-gating the clock and de-asserting the reset signal.
 *
 * @param[in] periph Peripheral module
 *
 * @note If @c periph_module_enable() is called a number of times,
 *       @c periph_module_disable() has to be called the same number of times,
 *       in order to put the peripheral into disabled state.
 */
void periph_module_enable(periph_module_t periph);

/**
 * @brief Disable peripheral module by gating the clock and asserting the reset signal.
 *
 * @param[in] periph Peripheral module
 *
 * @note If @c periph_module_enable() is called a number of times,
 *       @c periph_module_disable() has to be called the same number of times,
 *       in order to put the peripheral into disabled state.
 */
void periph_module_disable(periph_module_t periph);

/**
 * @brief Reset peripheral module by asserting and de-asserting the reset signal.
 *
 * @param[in] periph Peripheral module
 *
 * @note Calling this function does not enable or disable the clock for the module.
 */
void periph_module_reset(periph_module_t periph);

/**
 * @brief Enable Wi-Fi and BT common module
 *
 * @note If @c wifi_bt_common_module_enable() is called a number of times,
 *       @c wifi_bt_common_module_disable() has to be called the same number of times,
 *       in order to put the peripheral into disabled state.
 */
void wifi_bt_common_module_enable(void);

/**
 * @brief Disable Wi-Fi and BT common module
 *
 * @note If @c wifi_bt_common_module_enable() is called a number of times,
 *       @c wifi_bt_common_module_disable() has to be called the same number of times,
 *       in order to put the peripheral into disabled state.
 */
void wifi_bt_common_module_disable(void);

/**
 * @brief Enable Wi-Fi module
 *
 * @note Calling this function will only enable Wi-Fi module.
 */
void wifi_module_enable(void);

/**
 * @brief Disable Wi-Fi module
 *
 * @note Calling this function will only disable Wi-Fi module.
 */
void wifi_module_disable(void);

#ifdef __cplusplus
}
#endif
