/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the MSPI memory barrier workaround
 *
 * On targets affected by the MSPI memory barrier hardware issue, this allocates the
 * dummy cacheline used by @ref esp_psram_mspi_mb to force a cache writeback/fence. On
 * other targets this is a no-op.
 *
 * @return ESP_OK on success, otherwise an error code
 */
esp_err_t esp_psram_mspi_mb_init(void);

/**
 * @brief MSPI memory barrier
 *
 * Some targets need this workaround after a DMA transfer into PSRAM to make sure the
 * data is visible to the CPU. This is a no-op on targets that don't need the workaround.
 */
void esp_psram_mspi_mb(void);

#ifdef __cplusplus
}
#endif
