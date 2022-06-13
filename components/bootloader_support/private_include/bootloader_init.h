/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_image_format.h"

/**@{*/
/**
 * @brief labels from bootloader linker script: bootloader.ld
 *
 */
extern int _bss_start;
extern int _bss_end;
extern int _data_start;
extern int _data_end;
/**@}*/

/**
 * @brief bootloader image header
 *
 */
extern esp_image_header_t bootloader_image_hdr;

/**@{*/
/**
 * @brief Common initialization steps that are applied to all targets.
 *
 */
esp_err_t bootloader_read_bootloader_header(void);
esp_err_t bootloader_check_bootloader_validity(void);
void bootloader_clear_bss_section(void);
void bootloader_config_wdt(void);
void bootloader_enable_random(void);
void bootloader_print_banner(void);
/**@}*/

/* @brief Prepares hardware for work.
 *
 * Setting up:
 * - Disable Cache access for both CPUs;
 * - Initialise cache mmu;
 * - Setting up pins and mode for SD, SPI, UART, Clocking.

 *  @return ESP_OK   - If the setting is successful.
 *          ESP_FAIL - If the setting is not successful.
 */
esp_err_t bootloader_init(void);
