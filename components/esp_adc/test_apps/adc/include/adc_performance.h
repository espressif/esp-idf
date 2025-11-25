/* SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#if __has_include("adc_performance_esp32.h")
#include "adc_performance_esp32.h"
#elif __has_include("adc_performance_esp32s2.h")
#include "adc_performance_esp32s2.h"
#elif __has_include("adc_performance_esp32s3.h")
#include "adc_performance_esp32s3.h"
#elif __has_include("adc_performance_esp32c2.h")
#include "adc_performance_esp32c2.h"
#elif __has_include("adc_performance_esp32c3.h")
#include "adc_performance_esp32c3.h"
#elif __has_include("adc_performance_esp32c5.h")
#include "adc_performance_esp32c5.h"
#elif __has_include("adc_performance_esp32c6.h")
#include "adc_performance_esp32c6.h"
#elif __has_include("adc_performance_esp32c61.h")
#include "adc_performance_esp32c61.h"
#elif __has_include("adc_performance_esp32h2.h")
#include "adc_performance_esp32h2.h"
#elif __has_include("adc_performance_esp32p4.h")
#include "adc_performance_esp32p4.h"
#endif
