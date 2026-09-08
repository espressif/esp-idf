/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include "esp_err.h"
#include "esp_blockdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a whole-disk BDL backed by an SD/eMMC card.
 *
 * Initializes the card over the host peripheral selected in menuconfig
 * (SDMMC or SDSPI) and returns a block device that maps the whole card.
 *
 * @param[out] out  Handle of the created whole-disk block device.
 * @return ESP_OK on success, an error code otherwise.
 */
esp_err_t example_sd_card_bdl_create(esp_blockdev_handle_t *out);

/**
 * @brief Release a whole-disk BDL previously created with
 *        example_sd_card_bdl_create() and de-initialize the card host.
 *
 * @param disk  Handle returned by example_sd_card_bdl_create().
 * @return ESP_OK on success, an error code otherwise.
 */
esp_err_t example_sd_card_bdl_release(esp_blockdev_handle_t disk);

#ifdef __cplusplus
}
#endif
