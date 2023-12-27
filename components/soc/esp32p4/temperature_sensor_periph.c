/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/temperature_sensor_periph.h"

const temperature_sensor_attribute_t temperature_sensor_attributes[TEMPERATURE_SENSOR_ATTR_RANGE_NUM] = {
    /*Offset   reg_val  min  max  error */
    {-2,     5,    50,  125,   3},
    {-1,     7,    20,  100,   2},
    { 0,    15,   -10,   80,   1},
    { 1,    11,   -30,   50,   2},
    { 2,    10,   -40,   20,   3},
};
