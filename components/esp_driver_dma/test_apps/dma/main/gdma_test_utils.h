/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_private/gdma.h"
#include "esp_private/esp_mspi_align.h"

#if CONFIG_IDF_TARGET_ESP32S31
// ESP32-S31 LP AHB GDMA can't burst-access external PSRAM. Skip the
// MSPI-strict PSRAM test paths because Flash Encryption / PSRAM ECC require
// burst accesses aligned to the MSPI block size.
#define GDMA_TEST_LP_AHB_BURST_PSRAM_SUPPORTED 0
#else
#define GDMA_TEST_LP_AHB_BURST_PSRAM_SUPPORTED 1
#endif

/**
 * @brief Whether MSPI strict alignment is required (Flash Encryption and/or PSRAM ECC)
 */
static inline bool gdma_test_mspi_strict_alignment_required(void)
{
    return esp_mspi_get_alignment(NULL) > 1;
}

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
