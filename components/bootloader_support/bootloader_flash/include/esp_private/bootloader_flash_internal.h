/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <stdint.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize spi_flash in bootloader and print flash info
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t bootloader_init_spi_flash(void);

#ifdef __cplusplus
}
#endif
