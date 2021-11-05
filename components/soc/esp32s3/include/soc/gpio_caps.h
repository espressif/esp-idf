/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ESP32-S3 has 1 GPIO peripheral
#define SOC_GPIO_PORT           (1U)
#define SOC_GPIO_PIN_COUNT      (49)

// On ESP32-S3, Digital IOs have their own registers to control pullup/down/capability, independent with RTC registers.
#define SOC_GPIO_SUPPORT_RTC_INDEPENDENT (1)
// Force hold is a new function of ESP32-S3
#define SOC_GPIO_SUPPORT_FORCE_HOLD      (1)

// 0~48 except from 22~25 are valid
#define SOC_GPIO_VALID_GPIO_MASK         (0x1FFFFFFFFFFFFULL & ~(0ULL | BIT22 | BIT23 | BIT24 | BIT25))
// No GPIO is input only
#define SOC_GPIO_VALID_OUTPUT_GPIO_MASK  (SOC_GPIO_VALID_GPIO_MASK)

// Support to configure slept status
#define SOC_GPIO_SUPPORT_SLP_SWITCH  (1)

#ifdef __cplusplus
}
#endif
