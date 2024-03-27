/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_private/esp_ldo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init PSRAM VDD LDO during early stage
 *
 * @return
 *      - LDO unit handle on success
 *      - NULL when external power supply is configured to be used
 */
esp_ldo_unit_handle_t esp_ldo_vdd_psram_early_init(void);

/**
 * @brief Init PSRAM VDD LDO
 *
 * @param[out] ldo_unit LDO unit handle
 *
 * @return
 *      - ESP_OK                 Successful.
 *      - ESP_ERR_INVALID_STATE  External power supply is configured to be used
 *      - ESP_ERR_INVALID_ARG    Arguments is NULL or invalid LDO configuration.
 *      - other error codes from lower-level driver.
 *
 */
esp_err_t esp_ldo_vdd_psram_init(esp_ldo_unit_handle_t *ldo_unit);

/**
 * @brief De-init PSRAM VDD LDO
 *
 * @param[in] ldo_unit LDO unit handle
 */
esp_err_t esp_ldo_vdd_psram_deinit(esp_ldo_unit_handle_t ldo_unit);

#ifdef __cplusplus
}
#endif
