/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init PSRAM VDD LDO during early stage
 */
void esp_ldo_vdd_psram_early_init(void);

#ifdef __cplusplus
}
#endif
