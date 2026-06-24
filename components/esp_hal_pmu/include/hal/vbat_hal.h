/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t interrupt_mask;              // interrupt mask.
    uint16_t charger_resistor_value;      // charger resistor value
    bool enable_vbat_charger;             // whether enable vbat charger comparator
    uint8_t low_threshold;                // low voltage threshold
    uint8_t high_threshold;               // high voltage threshold
    uint8_t brownout_threshold;           // brownout threshold
    uint16_t undervoltage_filter_time;     // under voltage filter time
    uint16_t upvoltage_filter_time;        // up voltage filter time
} vbat_hal_config_t;

/**
 * @brief Config vbat hal.
 *
 * @param cfg Pointer of vbat configuration structure.
 */
void vbat_hal_config(const vbat_hal_config_t *cfg);

#ifdef __cplusplus
}
#endif
