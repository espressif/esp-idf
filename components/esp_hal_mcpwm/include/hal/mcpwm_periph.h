/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#if SOC_HAS(MCPWM)
#include "hal/mcpwm_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(MCPWM)
typedef struct {
    const char *module_name;
    const int irq_id;
    struct {
        struct {
            const uint32_t pwm_sig;
        } generators[MCPWM_LL_GET(GENERATORS_PER_OPERATOR)];
    } operators[MCPWM_LL_GET(OPERATORS_PER_GROUP)];
    struct {
        const uint32_t fault_sig;
    } gpio_faults[MCPWM_LL_GET(GPIO_FAULTS_PER_GROUP)];
    struct {
        const uint32_t cap_sig;
    } captures[MCPWM_LL_GET(CAPTURE_CHANNELS_PER_TIMER)];
    struct {
        const uint32_t sync_sig;
    } gpio_synchros[MCPWM_LL_GET(GPIO_SYNCHROS_PER_GROUP)];
} soc_mcpwm_signal_desc_t;

extern const soc_mcpwm_signal_desc_t soc_mcpwm_signals[MCPWM_LL_GET(GROUP_NUM)];

#endif // SOC_HAS(MCPWM)

#ifdef __cplusplus
}
#endif
