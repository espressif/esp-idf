/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "driver/touch_sensor_legacy.h"

#if !CONFIG_TOUCH_SUPPRESS_DEPRECATE_WARN
#warning "This set of Touch APIs has been deprecated, \
please include 'driver/touch_sens.h' instead. \
if you want to keep using the old APIs and ignore this warning, \
you can enable 'Suppress legacy driver deprecated warning' option under 'Touch Configuration' menu in Kconfig"
#endif
