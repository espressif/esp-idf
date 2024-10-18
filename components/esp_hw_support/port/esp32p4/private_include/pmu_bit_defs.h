/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define PMU_SDIO_WAKEUP_EN              BIT(0)
#define PMU_LP_CORE_WAKEUP_EN           BIT(1)
#define PMU_GPIO_WAKEUP_EN              BIT(2)
#define PMU_USB_WAKEUP_EN               BIT(3)
#define PMU_UART4_WAKEUP_EN             BIT(4)
#define PMU_UART3_WAKEUP_EN             BIT(5)
#define PMU_UART2_WAKEUP_EN             BIT(6)
#define PMU_UART1_WAKEUP_EN             BIT(7)
#define PMU_UART0_WAKEUP_EN             BIT(8)
#define PMU_LP_GPIO_WAKEUP_EN           BIT(9)
#define PMU_LP_UART_WAKEUP_EN           BIT(10)
#define PMU_TOUCH_WAKEUP_EN             BIT(11)
#define PMU_EXT1_WAKEUP_EN              BIT(12)
#define PMU_LP_TIMER_WAKEUP_EN          BIT(13)
#define PMU_BOD_WAKEUP_EN               BIT(14)
#define PMU_VDDBAT_UNDERVOLT_WAKEUP_EN  BIT(15)
#define PMU_LP_CORE_TRAP_WAKEUP_EN      BIT(16)
#define PMU_ETM_WAKEUP_EN               BIT(17)
#define PMU_LP_TIMER1_WAKEUP_EN         BIT(18)
#define PMU_LP_I2S_WAKEUP_EN            BIT(19)

// BIT order in PLL control registers in PMU
#define PMU_CPLL_CTRL                   BIT(0)
#define PMU_SPLL_CTRL                   BIT(1)
#define PMU_APLL_CTRL                   BIT(2)
#define PMU_SDIOPLL_CTRL                BIT(3)

#ifdef __cplusplus
}
#endif
