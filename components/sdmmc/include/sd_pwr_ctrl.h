/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SD power control handle
 */
typedef struct sd_pwr_ctrl_drv_t *sd_pwr_ctrl_handle_t;

/**
 * @brief Set SD IO voltage by a registered SD power control driver handle
 *
 * @param[in] handle      SD power control driver handle
 * @param[in] voltage_mv  Voltage in mV
 */
esp_err_t sd_pwr_ctrl_set_io_voltage(sd_pwr_ctrl_handle_t handle, int voltage_mv);

#ifdef __cplusplus
}
#endif
