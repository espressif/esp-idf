/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SCCB Interface and Sensor Init
 *
 * @param[in]  i2c_port              I2C port
 * @param[out] out_i2c_bus_handle    I2C bus handle
 */
void example_sensor_init(int i2c_port, i2c_master_bus_handle_t *out_i2c_bus_handle);

#ifdef __cplusplus
}
#endif
