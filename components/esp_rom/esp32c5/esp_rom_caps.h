/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

// TODO: IDF-9197 This file is created to glob the version specific soc_caps correctly

#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
#include "beta3/esp32c5/esp_rom_caps.h"   // recursive, condition: IDF_TARGET_ESP32C5_BETA3_VERSION
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
#include "mp/esp32c5/esp_rom_caps.h"      // recursive, condition: IDF_TARGET_ESP32C5_MP_VERSION
#endif
