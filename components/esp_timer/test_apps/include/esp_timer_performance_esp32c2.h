/* SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef CONFIG_XTAL_FREQ_26
#define IDF_PERFORMANCE_MAX_ESP_TIMER_GET_TIME_PER_CALL                         1900
#else
#define IDF_PERFORMANCE_MAX_ESP_TIMER_GET_TIME_PER_CALL                         1200
#endif
