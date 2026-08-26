/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Image process driver
 */
typedef struct image_process_driver_s image_process_driver_t;

/**
 * @brief Image process flow
 * @note This API first reads the image header, then process the segments from the image header.
 *       This API can be further inserted with more steps about the image processing by registering
 *       more function pointer in `image_process_driver_t`.
 *
 * @return
 *        - ESP_OK
 *        - ESP_FAIL: image process flow fails
 */
esp_err_t image_process(void);

/**
 * @brief get flash segments info, only available after image_process() has been called
 *
 * @param[out] out_drom_paddr_start  drom paddr start
 * @param[out] out_irom_paddr_start  irom paddr start
 */
void image_process_get_flash_segments_info(uint32_t *out_drom_paddr_start, uint32_t *out_irom_paddr_start);

#ifdef __cplusplus
}
#endif
