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

#if SOC_HAS(MCPWM)
#include "hal/mcpwm_ll.h"
#endif

#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
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

#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
typedef struct {
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
    const periph_retention_module_t retention_module;
} mcpwm_reg_retention_info_t;

extern const mcpwm_reg_retention_info_t mcpwm_reg_retention_info[MCPWM_LL_GET(GROUP_NUM)];
#endif // SOC_MCPWM_SUPPORT_SLEEP_RETENTION

#endif // SOC_HAS(MCPWM)

#ifdef __cplusplus
}
#endif
