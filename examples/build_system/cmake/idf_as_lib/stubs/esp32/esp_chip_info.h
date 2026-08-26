/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CHIP_FEATURE_EMB_FLASH  (1UL << 0)
#define CHIP_FEATURE_BT         (1UL << 4)
#define CHIP_FEATURE_BLE        (1UL << 5)

typedef struct {
    uint32_t features;       //!< bit mask of CHIP_FEATURE_x feature flags
    uint16_t revision;       //!< chip revision number (in format MXX; where M - wafer major version, XX - wafer minor version)
    uint8_t cores;           //!< number of CPU cores
} esp_chip_info_t;

void esp_chip_info(esp_chip_info_t *out_info);

#ifdef __cplusplus
}
#endif
