/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_private/gdma.h"

#if CONFIG_IDF_TARGET_ESP32S31
// ESP32-S31 LP AHB GDMA can't burst-access external PSRAM. Skip the
// flash-encrypted PSRAM test paths because encrypted PSRAM requires burst
// accesses aligned to the encryption block size.
#define GDMA_TEST_LP_AHB_BURST_PSRAM_SUPPORTED 0
#else
#define GDMA_TEST_LP_AHB_BURST_PSRAM_SUPPORTED 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Trigger a "fake" sleep retention process.
 *
 * @note Call this help function after the gdma set up is completed. Then check the gdma functionality is still working.
 *
 * @param chan GDMA channel handle to be reset
 * @param ... Other GDMA channel handle if any
 */
void test_gdma_trigger_retention_backup(gdma_channel_handle_t chan, ...);

#ifdef __cplusplus
}
#endif
