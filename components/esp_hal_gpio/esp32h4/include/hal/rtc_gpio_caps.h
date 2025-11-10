/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define RTC_GPIO_CAPS_GET(_attr) _RTC_GPIO_ ## _attr

#define _RTC_GPIO_EDGE_WAKEUP_SUPPORTED      1
