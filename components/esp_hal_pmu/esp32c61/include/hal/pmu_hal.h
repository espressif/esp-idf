/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for PMU

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/soc_caps.h"
#include "hal/pmu_ll.h"
#include "hal/pmu_types.h"

typedef struct {
    pmu_dev_t *dev;
} pmu_hal_context_t;

void pmu_hal_hp_set_digital_power_up_wait_cycle(pmu_hal_context_t *hal, uint32_t power_supply_wait_cycle, uint32_t power_up_wait_cycle);

uint32_t pmu_hal_hp_get_digital_power_up_wait_cycle(pmu_hal_context_t *hal);

void pmu_hal_lp_set_digital_power_up_wait_cycle(pmu_hal_context_t *hal, uint32_t power_supply_wait_cycle, uint32_t power_up_wait_cycle);

uint32_t pmu_hal_lp_get_digital_power_up_wait_cycle(pmu_hal_context_t *hal);

void pmu_hal_hp_set_control_ready_wait_cycle(pmu_hal_context_t *hal, uint32_t isolate_wait_cycle, uint32_t reset_wait_cycle);

void pmu_hal_lp_set_control_ready_wait_cycle(pmu_hal_context_t *hal, uint32_t isolate_wait_cycle, uint32_t reset_wait_cycle);

void pmu_hal_hp_set_sleep_active_backup_enable(pmu_hal_context_t *hal);

void pmu_hal_hp_set_sleep_active_backup_disable(pmu_hal_context_t *hal);

void pmu_hal_hp_set_sleep_modem_backup_enable(pmu_hal_context_t *hal);

void pmu_hal_hp_set_sleep_modem_backup_disable(pmu_hal_context_t *hal);

void pmu_hal_hp_set_modem_active_backup_enable(pmu_hal_context_t *hal);

void pmu_hal_hp_set_modem_active_backup_disable(pmu_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
