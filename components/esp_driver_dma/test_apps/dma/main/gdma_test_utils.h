/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_private/gdma.h"

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
