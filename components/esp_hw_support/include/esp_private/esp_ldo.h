/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "hal/ldo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of LDO unit handle
 */
typedef struct ldo_unit_ctx_t *esp_ldo_unit_handle_t;

/**
 * @brief LDO unit configurations
 */
typedef struct {
    int voltage_mv;                  ///< LDO output voltage in mV
} esp_ldo_unit_cfg_t;

/**
 * @brief LDO driver initial configurations
 */
typedef struct {
    int unit_id;                    ///< LDO ID, this is aligned with datasheet, e.g. you should set this to 1, if using LDO ID 1
    esp_ldo_unit_cfg_t cfg;         ///< LDO unit configuration
    struct {
        uint32_t enable_unit: 1;    ///< Enable the LDO unit after it's initialised
        uint32_t shared_ldo:  1;    ///< Mark this LDO unit as shared
    } flags;                        ///< LDO unit flags
} esp_ldo_unit_init_cfg_t;

/**
 * @brief Init a LDO during early stage
 *
 * @note This API is only for early stage usage
 *
 * @param[in] unit_id  LDO unit ID
 * @param[in] cfg      LDO unit configuration
 *
 * @return LDO unit handle
 */
esp_ldo_unit_handle_t esp_ldo_init_unit_early(const esp_ldo_unit_init_cfg_t *init_config);

/**
 * @Brief Init a LDO
 *
 * @param[in]  init_config  LDO initial configurations
 * @param[out] ret_unit     LDO unit handle
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 *        - ESP_ERR_NOT_FOUND:   LDO unit is in use already
 */
esp_err_t esp_ldo_init_unit(const esp_ldo_unit_init_cfg_t *init_config, esp_ldo_unit_handle_t *ret_unit);

/**
 * @Brief Enable a LDO
 *
 * @param[in]  unit  LDO unit handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: LDO is enabled already
 */
esp_err_t esp_ldo_enable_unit(esp_ldo_unit_handle_t unit);

/**
 * @Brief Disable a LDO
 *
 * @param[in]  unit  LDO unit handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: LDO is disabled already
 */
esp_err_t esp_ldo_disable_unit(esp_ldo_unit_handle_t unit);

/**
 * @Brief Deinit a LDO
 *
 * @param[in]  unit  LDO unit handle
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: LDO is still enabled
 */
esp_err_t esp_ldo_deinit_unit(esp_ldo_unit_handle_t unit);

/**
 * Dump LDO usages
 *
 * @note This API shall not be called from an ISR.
 * @note This API does not guarantee thread safety
 *
 * @param stream stream to print information to; use stdout or stderr to print
 *               to the console; use fmemopen/open_memstream to print to a
 *               string buffer.
 * @return
 *        - ESP_OK
 */
esp_err_t esp_ldo_usage_dump(FILE* stream);

#ifdef __cplusplus
}
#endif
