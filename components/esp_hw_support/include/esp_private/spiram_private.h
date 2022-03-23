/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include "esp_err.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_SPIRAM_SIZE_16MBITS = 0,   /*!< SPI RAM size is 16 MBits */
    ESP_SPIRAM_SIZE_32MBITS = 1,   /*!< SPI RAM size is 32 MBits */
    ESP_SPIRAM_SIZE_64MBITS = 2,   /*!< SPI RAM size is 64 MBits */
    ESP_SPIRAM_SIZE_INVALID,       /*!< SPI RAM size is invalid */
} esp_spiram_size_t;

/**
 * @brief Get the size of the attached SPI RAM chip selected in menuconfig
 *
 * @return Size in bytes, or 0 if no external RAM chip support compiled in.
 */
size_t esp_spiram_get_size(void);

/**
 * @brief Get the psram mapped vaddr range
 *
 * @param[out] out_vstart PSRAM virtual address start
 * @param[out] out_vend   PSRAM virtual address end
 *
 * @return
 *        - ESP_OK                  On success
 *        - ESP_ERR_INVALID_STATE   PSRAM is not initialized successfully
 */
esp_err_t esp_spiram_get_mapped_range(intptr_t *out_vstart, intptr_t *out_vend);

/**
 * @brief Get the psram alloced vaddr range
 *
 * @param[out] out_vstart PSRAM virtual address start
 * @param[out] out_vend   PSRAM virtual address end
 *
 * @return
 *        - ESP_OK                  On success
 *        - ESP_ERR_INVALID_STATE   PSRAM is not initialized successfully
 */
esp_err_t esp_spiram_get_alloced_range(intptr_t *out_vstart, intptr_t *out_vend);

#ifdef __cplusplus
}
#endif
