/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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

#if SOC_USB_OTG_SUPPORTED && SOC_PM_SUPPORT_CNNT_PD
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

#ifdef __cplusplus
}
#endif
