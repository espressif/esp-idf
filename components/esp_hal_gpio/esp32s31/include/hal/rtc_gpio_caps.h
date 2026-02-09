/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// TODO: [ESP32S31] IDF-14785
// RTC GPIO capabilities are temporarily bypassed for ESP32-S31
// This file provides stub definitions to allow the build to succeed.
// Full RTC GPIO capabilities for ESP32-S31 need to be implemented in the future.

#pragma once

#define RTC_GPIO_CAPS_GET(_attr) _RTC_GPIO_ ## _attr

// TODO: [ESP32S31] IDF-14785
