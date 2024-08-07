/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// TODO: IDF-9247

#define PMU_EXT0_WAKEUP_EN          BIT(0)
#define PMU_EXT1_WAKEUP_EN          BIT(1)
#define PMU_GPIO_WAKEUP_EN          BIT(2)
#define PMU_WIFI_BEACON_WAKEUP_EN   BIT(3)
#define PMU_LP_TIMER_WAKEUP_EN      BIT(4)
#define PMU_WIFI_SOC_WAKEUP_EN      BIT(5)
#define PMU_UART0_WAKEUP_EN         BIT(6)
#define PMU_UART1_WAKEUP_EN         BIT(7)
#define PMU_BLE_SOC_WAKEUP_EN       BIT(10)
// #define PMU_LP_CORE_WAKEUP_EN       BIT(11)
#define PMU_USB_WAKEUP_EN           BIT(14)

#ifdef __cplusplus
}
#endif
