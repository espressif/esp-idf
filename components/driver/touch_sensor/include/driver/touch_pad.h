/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#if SOC_TOUCH_SENSOR_SUPPORTED && SOC_TOUCH_SENSOR_VERSION < 3
#include "driver/touch_sensor.h"
#endif
