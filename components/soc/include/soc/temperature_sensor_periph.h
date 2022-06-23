/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define TEMPERATURE_SENSOR_ATTR_RANGE_NUM (5)

typedef struct {
    int offset;
    int reg_val;
    int range_min;
    int range_max;
    int error_max;
} temperature_sensor_attribute_t;

extern const temperature_sensor_attribute_t temperature_sensor_attributes[TEMPERATURE_SENSOR_ATTR_RANGE_NUM];

#ifdef __cplusplus
}
#endif
