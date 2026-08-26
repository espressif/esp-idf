/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "sdkconfig.h"
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
#define PERIPH_RCC_ACQUIRE_ATOMIC(rc_periph, rc_name)                                       \
    for (uint8_t rc_name, _rc_cnt = 1, __DECLARE_RCC_RC_ATOMIC_ENV __attribute__((unused)); \
         _rc_cnt ? (rc_name = periph_rcc_acquire_enter(rc_periph), 1) : 0;                  \
         periph_rcc_acquire_exit(rc_periph, rc_name), _rc_cnt--)

/**
 * @brief Release the RCC lock for a peripheral module
 *
 * @note User code protected by this macro should be as short as possible, because it's a critical section
 * @note This macro will decrease the reference lock of that peripheral.
 *       You can get the value after the decrease from the `rc_name` local variable
 */
#define PERIPH_RCC_RELEASE_ATOMIC(rc_periph, rc_name)                                       \
    for (uint8_t rc_name, _rc_cnt = 1, __DECLARE_RCC_RC_ATOMIC_ENV __attribute__((unused)); \
         _rc_cnt ? (rc_name = periph_rcc_release_enter(rc_periph), 1) : 0;                  \
         periph_rcc_release_exit(rc_periph, rc_name), _rc_cnt--)

/**
 * @brief A simplified version of `PERIPH_RCC_ACQUIRE/RELEASE_ATOMIC`, without a reference count
 *
 * @note User code protected by this macro should be as short as possible, because it's a critical section
 */
#define PERIPH_RCC_ATOMIC()                                                 \
    for (int _rc_cnt = 1, __DECLARE_RCC_ATOMIC_ENV __attribute__((unused)); \
         _rc_cnt ? (periph_rcc_enter(), 1) : 0;                             \
         periph_rcc_exit(), _rc_cnt--)

/** @cond */
// The following functions are not intended to be used directly by the developers
uint8_t periph_rcc_acquire_enter(shared_periph_module_t periph);
void periph_rcc_acquire_exit(shared_periph_module_t periph, uint8_t ref_count);
uint8_t periph_rcc_release_enter(shared_periph_module_t periph);
void periph_rcc_release_exit(shared_periph_module_t periph, uint8_t ref_count);
void periph_rcc_enter(void);
void periph_rcc_exit(void);
/** @endcond */

/**
 * @}
 */

/*************************************************************************************************************
 * @note The following APIs are no longer supported since ESP32P4, please use the RCC lock macros instead.
 *************************************************************************************************************/
// allow the following targets to use the legacy periph_module_xyz APIs, to maintain backward compatibility,
// because periph_module_xyz is also used outside of the ESP-IDF
#if defined(CONFIG_IDF_TARGET_ESP32)   || \
    defined(CONFIG_IDF_TARGET_ESP32S2) || \
    defined(CONFIG_IDF_TARGET_ESP32S3) || \
    defined(CONFIG_IDF_TARGET_ESP32C2) || \
    defined(CONFIG_IDF_TARGET_ESP32C3) || \
    defined(CONFIG_IDF_TARGET_ESP32C6) || \
    defined(CONFIG_IDF_TARGET_ESP32H2)
#define __PERIPH_CTRL_ALLOW_LEGACY_API
#endif

#ifdef __PERIPH_CTRL_ALLOW_LEGACY_API
#define __PERIPH_CTRL_DEPRECATE_ATTR
#else
#define __PERIPH_CTRL_DEPRECATE_ATTR __attribute__((deprecated("This function is not functional on " CONFIG_IDF_TARGET)))
#endif

/**
 * @brief Enable peripheral module by un-gating the clock and de-asserting the reset signal.
 *
 * @param[in] periph Peripheral module
 *
 * @note If @c periph_module_enable() is called a number of times,
 *       @c periph_module_disable() has to be called the same number of times,
 *       in order to put the peripheral into disabled state.
 */
__PERIPH_CTRL_DEPRECATE_ATTR
void periph_module_enable(shared_periph_module_t periph);

/**
 * @brief Disable peripheral module by gating the clock and asserting the reset signal.
 *
 * @param[in] periph Peripheral module
 *
 * @note If @c periph_module_enable() is called a number of times,
 *       @c periph_module_disable() has to be called the same number of times,
 *       in order to put the peripheral into disabled state.
 */
__PERIPH_CTRL_DEPRECATE_ATTR
void periph_module_disable(shared_periph_module_t periph);

/**
 * @brief Reset peripheral module by asserting and de-asserting the reset signal.
 *
 * @param[in] periph Peripheral module
 *
 * @note Calling this function does not enable or disable the clock for the module.
 */
__PERIPH_CTRL_DEPRECATE_ATTR
void periph_module_reset(shared_periph_module_t periph);

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

/**
 * @brief Enable phy module by un-gating related clock and de-asserting the reset signal.
 *
 * @note This function acquires clocks required during the PHY enable sequence.
 */
void phy_module_enable(void);

/**
 * @brief Disable phy module by gating related clock and asserting the reset signal.
 *
 * @note This function releases clocks required during the PHY enable sequence.
 */
void phy_module_disable(void);

/**
 * @brief Checks whether phy module has all bits in @p mask set.
 *
 * @return true if all bits in @p mask are set; false otherwise.
 */
bool phy_module_has_clock_bits(uint32_t mask);

/**
 * @brief Enable coex module
 *
 * @note Calling this function will only enable coex module.
 * @note For ESP32S2, ESP32S3, and ESP32C3, this function has no effect because
 *       the coex module clock is controlled by the modem clock. On these chips,
 *       you must call esp_wifi_init() to enable the modem clock before using
 *       external coexistence features.
 */
void coex_module_enable(void);

/**
 * @brief Disable coex module
 *
 * @note Calling this function will only disable coex module.
 * @note For ESP32S2, ESP32S3, and ESP32C3, this function has no effect because
 *       the coex module clock is controlled by the modem clock.
 */
void coex_module_disable(void);
#undef __PERIPH_CTRL_DEPRECATE_ATTR

#ifdef __cplusplus
}
#endif
