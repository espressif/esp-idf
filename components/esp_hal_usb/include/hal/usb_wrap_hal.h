/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#if (SOC_USB_OTG_PERIPH_NUM > 0)
#include "soc/usb_wrap_struct.h"
#include "hal/usb_wrap_ll.h"
#endif // (SOC_USB_OTG_PERIPH_NUM > 0)
#include "hal/usb_wrap_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (SOC_USB_OTG_PERIPH_NUM > 0)

/**
 * @brief HAL context type of USB WRAP driver
 */
typedef struct {
    usb_wrap_dev_t *dev;
} usb_wrap_hal_context_t;

/**
 * @brief Initialize the USB WRAP HAL driver
 *
 * @param hal USB WRAP HAL context
 */
void _usb_wrap_hal_init(usb_wrap_hal_context_t *hal);

#if SOC_RCC_IS_INDEPENDENT
#define usb_wrap_hal_init(...)   _usb_wrap_hal_init(__VA_ARGS__)
#else
// Use a macro to wrap the function, force the caller to use it in a critical section
// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define usb_wrap_hal_init(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; _usb_wrap_hal_init(__VA_ARGS__);} while(0)
#endif

/**
 * @brief Disable USB WRAP
 *
 * Disable clock to the peripheral
 */
void _usb_wrap_hal_disable(void);

#if SOC_RCC_IS_INDEPENDENT
#define usb_wrap_hal_disable(...)   _usb_wrap_hal_disable(__VA_ARGS__)
#else
// Use a macro to wrap the function, force the caller to use it in a critical section
// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define usb_wrap_hal_disable(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; _usb_wrap_hal_disable(__VA_ARGS__);} while(0)
#endif

/* ---------------------------- USB PHY Control  ---------------------------- */

#if USB_WRAP_LL_EXT_PHY_SUPPORTED
/**
 * @brief Configure whether USB WRAP is routed to internal/external FSLS PHY
 *
 * @param hal USB WRAP HAL context
 * @param external True if external, False if internal
 */
void usb_wrap_hal_phy_set_external(usb_wrap_hal_context_t *hal, bool external);
#endif // USB_WRAP_LL_EXT_PHY_SUPPORTED

/**
 * @brief Enables and sets override of pull up/down resistors
 *
 * @param hal USB WRAP HAL context
 * @param vals Override values
 */
static inline void usb_wrap_hal_phy_enable_pull_override(usb_wrap_hal_context_t *hal, const usb_wrap_pull_override_vals_t *vals)
{
    usb_wrap_ll_phy_enable_pull_override(hal->dev, vals);
}

/**
 * @brief Disables pull up/down resistor override
 *
 * @param hal USB WRAP HAL context
 */
static inline void usb_wrap_hal_phy_disable_pull_override(usb_wrap_hal_context_t *hal)
{
    usb_wrap_ll_phy_disable_pull_override(hal->dev);
}

/**
 * @brief Enables/disables the USB FSLS PHY's test mode
 *
 * @param hal USB WRAP HAL context
 * @param enable Whether to enable test mode
 */
static inline void usb_wrap_hal_phy_enable_test_mode(usb_wrap_hal_context_t *hal, bool enable)
{
    usb_wrap_ll_phy_enable_test_mode(hal->dev, enable);
}

/**
 * @brief Set the USB FSLS PHY's signal test values
 *
 * @param hal USB WRAP HAL context
 * @param vals Test values
 */
static inline void usb_wrap_hal_phy_test_mode_set_signals(usb_wrap_hal_context_t *hal, const usb_wrap_test_mode_vals_t *vals)
{
    usb_wrap_ll_phy_test_mode_set_signals(hal->dev, vals);
}

#endif // (SOC_USB_OTG_PERIPH_NUM > 0)

#ifdef __cplusplus
}
#endif
