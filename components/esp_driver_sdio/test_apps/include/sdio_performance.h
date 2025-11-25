/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#if __has_include("sdio_performance_esp32.h")
#include "sdio_performance_esp32.h"
#elif __has_include("sdio_performance_esp32c6.h")
#include "sdio_performance_esp32c6.h"
#endif
