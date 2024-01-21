/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "esp_types.h"
#include "esp_err.h"
#include "sd_pwr_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sd_pwr_ctrl_drv_t sd_pwr_ctrl_drv_t;

struct sd_pwr_ctrl_drv_t {
    /**
     * @brief Set SD IO voltage
     *
     * @param[in] ctx         SD power control specific driver context
     * @param[in] voltage_mv  Voltage in mV
     *
     * @return
     *         - ESP_OK:                On success
     *         - ESP_ERR_INVALID_ARG:   Invalid argument
     */
    esp_err_t (*set_io_voltage)(void *ctx, int voltage_mv);

    /**
     * @brief SD power control driver context
     * Can be customized to difference power control methods
     */
    void *ctx;
};


#ifdef __cplusplus
}
#endif
