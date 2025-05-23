/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"
#if SOC_PCNT_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PCNT_SUPPORTED

typedef struct {
    struct {
        const char *module_name;
        struct {
            struct {
                const uint32_t pulse_sig;
                const uint32_t control_sig;
            } channels[SOC_PCNT_CHANNELS_PER_UNIT];
            const uint32_t clear_sig;
        } units[SOC_PCNT_UNITS_PER_GROUP];
        const uint32_t irq;
    } groups[SOC_PCNT_GROUPS];
} pcnt_signal_conn_t;

extern const pcnt_signal_conn_t pcnt_periph_signals;

#if SOC_PCNT_SUPPORT_SLEEP_RETENTION
typedef struct {
    const periph_retention_module_t retention_module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} pcnt_reg_retention_info_t;

extern const pcnt_reg_retention_info_t pcnt_reg_retention_info[SOC_PCNT_GROUPS];
#endif // SOC_PCNT_SUPPORT_SLEEP_RETENTION

#endif // SOC_PCNT_SUPPORTED

#ifdef __cplusplus
}
#endif
