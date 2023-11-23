/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_USB_SERIAL_JTAG_SUPPORTED
typedef struct {
    bool usj_clock_enabled;
    bool usj_pad_enabled;
} sleep_console_usj_enable_state_t;

/**
 * @brief Disable usb-serial-jtag pad during light sleep to avoid current leakage and
 *        backup the enable state before light sleep
 */
void sleep_console_usj_pad_backup_and_disable(void);

/**
 * @brief Restore initial usb-serial-jtag pad enable state when wakeup from light sleep
 */
void sleep_console_usj_pad_restore(void);
#endif

#ifdef __cplusplus
}
#endif
