/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "sd_pwr_ctrl.h"
#include "sd_protocol_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LDO configurations
 */
typedef struct {
    int ldo_chan_id;     ///< On-chip LDO channel ID, e.g. set to `4` is the `LDO_VO4` is connected to power the SDMMC IO
} sd_pwr_ctrl_ldo_config_t;

/**
 * @brief New an SD power control driver via on-chip LDO
 *
 * @param[in]  configs  On-chip LDO power control driver configurations
 * @param[out] ret_drv  Created power control driver handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG  Invalid arguments
 *        - ESP_ERR_NO_MEM       Out of memory
 */
esp_err_t sd_pwr_ctrl_new_on_chip_ldo(const sd_pwr_ctrl_ldo_config_t *configs, sd_pwr_ctrl_handle_t *ret_drv);

/**
 * @brief Delete a previously created on-chip LDO power control driver
 *
 * @param[in] ctrl_handle  Power control driver handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG  Invalid arguments
 */
esp_err_t sd_pwr_ctrl_del_on_chip_ldo(sd_pwr_ctrl_handle_t ctrl_handle);

#ifdef __cplusplus
}
#endif
