/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#define MODEM_BASE                 0x20100000
#define MODEM_PWR_BASE             0x2010D000

#define DR_REG_FE_BASE             (MODEM_BASE + 0x0000)
#define DR_REG_WIFI_MAC_BASE       (MODEM_BASE + 0x4000)
#define DR_REG_WIFI_BB_BASE        (MODEM_BASE + 0x7000)
#define DR_REG_WIFI_BB_BRX_BASE    (MODEM_BASE + 0x8000)
#define DR_REG_MODEM_TIMER_BASE    (MODEM_BASE + 0x8400)
#define DR_REG_MODEM_ETM_BASE      (MODEM_BASE + 0x8800)
#define DR_REG_MODEM_DATADUMP_BASE (MODEM_BASE + 0x9000)
#define DR_REG_MODEM_WIDGETS_BASE  (MODEM_BASE + 0x9400)
#define DR_REG_MODEM_SYSCON_BASE   (MODEM_BASE + 0x9C00)

#define DR_REG_MODEM_LPCON_BASE    (MODEM_PWR_BASE + 0x2000)

#define DR_REG_I2C_ANA_MST_BASE     (MODEM_PWR_BASE + 0x2800)
#define DR_REG_I2C_ANA_MST_MEM_BASE (MODEM_PWR_BASE + 0x2c00)
