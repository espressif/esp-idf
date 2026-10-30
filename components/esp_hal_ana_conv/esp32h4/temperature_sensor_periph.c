/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/temperature_sensor_periph.h"

const temperature_sensor_attribute_t temperature_sensor_attributes[TEMPERATURE_SENSOR_ATTR_RANGE_NUM] = {
    /* Offset x10, reg_val, min, max, error */
    {-26,     5,    50,  125,   3},
    {-10,     7,    20,  100,   2},
    {  0,    15,   -10,   80,   1},
    { 10,    11,   -30,   50,   2},
    { 25,    10,   -40,   20,   3},
};
