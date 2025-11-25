/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#if __has_include("crypto_performance_esp32.h")
#include "crypto_performance_esp32.h"
#elif __has_include("crypto_performance_esp32s2.h")
#include "crypto_performance_esp32s2.h"
#elif __has_include("crypto_performance_esp32s3.h")
#include "crypto_performance_esp32s3.h"
#elif __has_include("crypto_performance_esp32c2.h")
#include "crypto_performance_esp32c2.h"
#elif __has_include("crypto_performance_esp32c3.h")
#include "crypto_performance_esp32c3.h"
#elif __has_include("crypto_performance_esp32c6.h")
#include "crypto_performance_esp32c6.h"
#elif __has_include("crypto_performance_esp32h2.h")
#include "crypto_performance_esp32h2.h"
#endif
