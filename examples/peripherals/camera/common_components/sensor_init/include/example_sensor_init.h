/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
 * @brief Handle of SCCB interface and sensor
 */
typedef struct {
    esp_sccb_io_handle_t sccb_handle;            /*!< SCCB io handle that created by `sccb_new_i2c_io` */
    i2c_master_bus_handle_t i2c_bus_handle;      /*!< I2C bus handle that created by `i2c_new_master_bus` */
} example_sensor_handle_t;

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
 * @param[out] out_sensor_handle     Camera sensor handle
 */
void example_sensor_init(example_sensor_config_t *sensor_config, example_sensor_handle_t *out_sensor_handle);

/**
 * @brief SCCB Interface and Sensor Deinit
 *
 * @param[in] out_sensor_handle      Camera sensor handle
 */
void example_sensor_deinit(example_sensor_handle_t sensor_handle);

#ifdef __cplusplus
}
#endif
