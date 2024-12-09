/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/i2c_master.h"
#include "esp_cam_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration of SCCB interface and sensor
 */
typedef struct {
    int i2c_port_num;               /* SCCB: i2c port */
    int i2c_sda_io_num;             /* SCCB: i2c SDA IO number */
    int i2c_scl_io_num;             /* SCCB: i2c SCL IO number */
    esp_cam_sensor_port_t port;     /* Sensor: interface of the camera sensor */
    const char *format_name;        /* Sensor: format to be set for the camera sensor */
} example_sensor_config_t;

/**
 * @brief SCCB Interface and Sensor Init
 *
 * @param[in]  sensor_config         Camera sensor configuration
 * @param[out] out_i2c_bus_handle    I2C bus handle
 */
void example_sensor_init(example_sensor_config_t *sensor_config, i2c_master_bus_handle_t *out_i2c_bus_handle);

#ifdef __cplusplus
}
#endif
