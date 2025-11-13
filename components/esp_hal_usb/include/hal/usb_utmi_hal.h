/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#if (SOC_USB_UTMI_PHY_NUM > 0)
#include "soc/usb_utmi_struct.h"
#include "hal/usb_utmi_ll.h"
#endif // (SOC_USB_UTMI_PHY_NUM > 0)

#ifdef __cplusplus
extern "C" {
#endif

#if (SOC_USB_UTMI_PHY_NUM > 0)

/**
 * @brief HAL context type of USB UTMI driver
 */
typedef struct {
    usb_utmi_dev_t *dev;
} usb_utmi_hal_context_t;

/**
 * @brief Sets UTMI defaults
 *
 * Enable clock, reset the peripheral, sets default options (LS support, disconnection detection)
 *
 * @param[in] hal USB UTMI HAL context
 */
void _usb_utmi_hal_init(usb_utmi_hal_context_t *hal);

#if SOC_RCC_IS_INDEPENDENT
#define usb_utmi_hal_init(...)   _usb_utmi_hal_init(__VA_ARGS__)
#else
// Use a macro to wrap the function, force the caller to use it in a critical section
// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define usb_utmi_hal_init(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; _usb_utmi_hal_init(__VA_ARGS__);} while(0)
#endif

/**
 * @brief Disable UTMI
 *
 * Disable clock to the peripheral
 */
void _usb_utmi_hal_disable(void);

#if SOC_RCC_IS_INDEPENDENT
#define usb_utmi_hal_disable(...)   _usb_utmi_hal_disable(__VA_ARGS__)
#else
// Use a macro to wrap the function, force the caller to use it in a critical section
// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define usb_utmi_hal_disable(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; _usb_utmi_hal_disable(__VA_ARGS__);} while(0)
#endif

#endif // (SOC_USB_UTMI_PHY_NUM > 0)

#ifdef __cplusplus
}
#endif
