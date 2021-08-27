/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/**
 * @brief Configure analog super WDT reset
 *
 * @param enable Boolean to enable or disable super WDT reset
 */
void bootloader_ana_super_wdt_reset_config(bool enable);

/**
 * @brief Configure analog brownout reset
 *
 * @param enable Boolean to enable or disable brownout reset
 */
void bootloader_ana_bod_reset_config(bool enable);

/**
 * @brief Configure analog clock glitch reset
 *
 * @param enable Boolean to enable or disable clock glitch reset
 */
void bootloader_ana_clock_glitch_reset_config(bool enable);
