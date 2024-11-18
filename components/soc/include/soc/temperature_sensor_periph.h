/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/regdma.h"
#if SOC_TEMPERATURE_SENSOR_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define TEMPERATURE_SENSOR_ATTR_RANGE_NUM (5)

typedef struct {
    int offset;
    int reg_val;
    int range_min;
    int range_max;
    int error_max;
} temperature_sensor_attribute_t;

extern const temperature_sensor_attribute_t temperature_sensor_attributes[TEMPERATURE_SENSOR_ATTR_RANGE_NUM];

#if SOC_TEMPERATURE_SENSOR_SUPPORT_SLEEP_RETENTION
typedef struct {
    const regdma_entries_config_t *link_list;
    uint32_t link_num;
    periph_retention_module_t module_id;
} temperature_sensor_reg_ctx_link_t;

extern const temperature_sensor_reg_ctx_link_t temperature_sensor_regs_retention;
#endif // SOC_TEMPERATURE_SENSOR_SUPPORT_SLEEP_RETENTION

#ifdef __cplusplus
}
#endif
