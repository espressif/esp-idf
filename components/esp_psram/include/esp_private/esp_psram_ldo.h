/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration for PSRAM domain LDO supply initialization
 */
typedef struct {
    uint32_t voltage_mv; /*!< PSRAM supply voltage in mV */
} esp_psram_power_cfg_t;

/**
 * @brief Initialize the PSRAM supply (LDO voltage and power switch)
 *
 * @param config Configuration for the PSRAM supply
 */
void esp_psram_power_init(const esp_psram_power_cfg_t *config);

#ifdef __cplusplus
}
#endif
