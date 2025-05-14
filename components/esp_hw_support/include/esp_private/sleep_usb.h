/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_USB_OTG_SUPPORTED
#if SOC_PM_SUPPORT_CNNT_PD
/**
 * @brief Backup usb OTG phy bus_clock / stoppclk configuration and
 *         before light sleep to avoid current leakage
 */
void sleep_usb_otg_phy_backup_and_disable(void);

/**
 * @brief Restore initial usb OTG phy configuration when wakeup from light sleep
 */
void sleep_usb_otg_phy_restore(void);
#endif

#if SOC_USB_UTMI_PHY_NO_POWER_OFF_ISO
/**
 * @brief The DP/DM part of the UTMI PHY circuit of esp32p4 that converts logic level to digital
 *        has no power off isolation, which will cause leakage when entering deepsleep.
 *        This problem can be workarounded by enabling USB-OTG's HNP (Host negotiation protocol)
 *        to enable DM pull-down to suppress leakage.
 */
void sleep_usb_suppress_deepsleep_leakage(void);
#endif
#endif
#ifdef __cplusplus
}
#endif
