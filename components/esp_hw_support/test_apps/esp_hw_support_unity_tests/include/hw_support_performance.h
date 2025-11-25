/* SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#if __has_include("hw_support_performance_esp32.h")
#include "hw_support_performance_esp32.h"
#elif __has_include("hw_support_performance_esp32c2.h")
#include "hw_support_performance_esp32c2.h"
#elif __has_include("hw_support_performance_esp32c3.h")
#include "hw_support_performance_esp32c3.h"
#elif __has_include("hw_support_performance_esp32h2.h")
#include "hw_support_performance_esp32h2.h"
#elif __has_include("hw_support_performance_esp32s3.h")
#include "hw_support_performance_esp32s3.h"
#endif
