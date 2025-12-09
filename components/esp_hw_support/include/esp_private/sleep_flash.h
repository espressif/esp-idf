/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32P4 && (CONFIG_ESP_REV_MIN_FULL == 300)
/**
 * Workaround for esp32p4 v3 MPSI access failure after power up.
 */
void sleep_flash_p4_rev3_workaround(void);
#endif

#ifdef __cplusplus
}
#endif
