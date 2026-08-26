/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once
#define DR_REG_MODEM_BASE         0x600C0000
#define DR_REG_MODEM_PWR_BASE     0x600CD000

#define DR_REG_MODEM_SYSCON_BASE  0x600C9C00
#define DR_REG_MODEM_LPCON_BASE   0x600CF000 // (DR_REG_MODEM_PWR_BASE + 0x2000)
#define DR_REG_I2C_ANA_MST_BASE   0x600CF800 // (DR_REG_MODEM_PWR_BASE + 0x2800)
