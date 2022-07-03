/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
