/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <stdint.h>
#include <esp_err.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize spi_flash in bootloader and print flash info
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t bootloader_init_spi_flash(void);

#if CONFIG_APP_BUILD_TYPE_RAM && !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
/**
 * @brief Config all flash related stuff according to the header. The consistency of all flash configs is ensured.
 *
 * @return None
 */
void bootloader_flash_hardware_init(void);
#endif

/**
 * @brief Initialise mspi core clock
 */
void bootloader_init_mspi_clock(void);

#ifdef __cplusplus
}
#endif
