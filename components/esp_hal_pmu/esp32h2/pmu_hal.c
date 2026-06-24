/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for PMU (ESP32-H2 specific part)

#include "soc/soc.h"
#include "esp_attr.h"
#include "hal/pmu_hal.h"
#include "hal/pmu_types.h"

void pmu_hal_hp_set_digital_power_up_wait_cycle(pmu_hal_context_t *hal, uint32_t power_supply_wait_cycle, uint32_t power_up_wait_cycle)
{
    pmu_ll_hp_set_digital_power_supply_wait_cycle(hal->dev, power_supply_wait_cycle);
    pmu_ll_hp_set_digital_power_up_wait_cycle(hal->dev, power_up_wait_cycle);
}

uint32_t pmu_hal_hp_get_digital_power_up_wait_cycle(pmu_hal_context_t *hal)
{
    uint32_t power_supply_wait_cycle = pmu_ll_hp_get_digital_power_supply_wait_cycle(hal->dev);
    uint32_t power_up_wait_cycle = pmu_ll_hp_get_digital_power_up_wait_cycle(hal->dev);
    return power_supply_wait_cycle + power_up_wait_cycle;
}

void pmu_hal_lp_set_digital_power_up_wait_cycle(pmu_hal_context_t *hal, uint32_t power_supply_wait_cycle, uint32_t power_up_wait_cycle)
{
    pmu_ll_lp_set_digital_power_supply_wait_cycle(hal->dev, power_supply_wait_cycle);
    pmu_ll_lp_set_digital_power_up_wait_cycle(hal->dev, power_up_wait_cycle);
}

uint32_t pmu_hal_lp_get_digital_power_up_wait_cycle(pmu_hal_context_t *hal)
{
    uint32_t power_supply_wait_cycle = pmu_ll_lp_get_digital_power_supply_wait_cycle(hal->dev);
    uint32_t power_up_wait_cycle = pmu_ll_lp_get_digital_power_up_wait_cycle(hal->dev);
    return power_supply_wait_cycle + power_up_wait_cycle;
}

void pmu_hal_hp_set_sleep_active_backup_enable(pmu_hal_context_t *hal)
{
    pmu_ll_hp_set_active_to_sleep_backup_enable(hal->dev);
    pmu_ll_hp_set_sleep_to_active_backup_enable(hal->dev);
}

void pmu_hal_hp_set_sleep_active_backup_disable(pmu_hal_context_t *hal)
{
    pmu_ll_hp_set_sleep_to_active_backup_disable(hal->dev);
    pmu_ll_hp_set_active_to_sleep_backup_disable(hal->dev);
}
