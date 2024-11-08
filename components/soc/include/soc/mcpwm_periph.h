/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"
#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MCPWM_SUPPORTED
typedef struct {
    struct {
        const periph_module_t module; // Peripheral module
        const int irq_id;
        struct {
            struct {
                const uint32_t pwm_sig;
            } generators[SOC_MCPWM_GENERATORS_PER_OPERATOR];
        } operators[SOC_MCPWM_OPERATORS_PER_GROUP];
        struct {
            const uint32_t fault_sig;
        } gpio_faults[SOC_MCPWM_GPIO_FAULTS_PER_GROUP];
        struct {
            const uint32_t cap_sig;
        } captures[SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER];
        struct {
            const uint32_t sync_sig;
        } gpio_synchros[SOC_MCPWM_GPIO_SYNCHROS_PER_GROUP];
    } groups[SOC_MCPWM_GROUPS];
} mcpwm_signal_conn_t;

extern const mcpwm_signal_conn_t mcpwm_periph_signals;

#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
typedef struct {
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
    const periph_retention_module_t retention_module;
} mcpwm_reg_retention_info_t;

extern const mcpwm_reg_retention_info_t mcpwm_reg_retention_info[SOC_MCPWM_GROUPS];
#endif // SOC_MCPWM_SUPPORT_SLEEP_RETENTION

#endif // SOC_MCPWM_SUPPORTED

#ifdef __cplusplus
}
#endif
