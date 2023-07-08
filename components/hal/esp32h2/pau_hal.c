/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for PAU (ESP32-H2 specific part)

#include "soc/soc.h"
#include "soc/pcr_struct.h"
#include "esp_attr.h"
#include "hal/misc.h"
#include "hal/pau_hal.h"
#include "hal/pau_types.h"

void pau_hal_set_regdma_entry_link_addr(pau_hal_context_t *hal, pau_regdma_link_addr_t *link_addr)
{
    /* ESP32H2 does not have PMU HP_AON power domain. because the registers
     * of PAU REGDMA is included to PMU TOP power domain, cause the contents
     * of PAU REGDMA registers will be lost when the TOP domain is powered down
     * during light sleep, so we does not need to enable REGDMA backup here.
     * We will use the software to trigger REGDMA to backup or restore. */
}

void IRAM_ATTR pau_hal_start_regdma_system_link(pau_hal_context_t *hal, bool backup_or_restore)
{
    pau_ll_clear_regdma_backup_done_intr_state(hal->dev);

    pau_ll_select_regdma_entry_link(hal->dev, 0);
    pau_ll_set_regdma_entry_link_backup_direction(hal->dev, backup_or_restore);
    pau_ll_set_regdma_entry_link_backup_start_enable(hal->dev);

    while (!(pau_ll_get_regdma_intr_raw_signal(hal->dev) & PAU_DONE_INT_RAW));
}

void IRAM_ATTR pau_hal_stop_regdma_system_link(pau_hal_context_t *hal)
{
    pau_ll_set_regdma_entry_link_backup_start_disable(hal->dev);
    pau_ll_select_regdma_entry_link(hal->dev, 0); /* restore link select to default */
    pau_ll_clear_regdma_backup_done_intr_state(hal->dev);
}

void pau_hal_start_regdma_extra_link(pau_hal_context_t *hal, bool backup_or_restore)
{
    pau_ll_clear_regdma_backup_done_intr_state(hal->dev);
    /* The link 3 of REGDMA is reserved, we use it as an extra linked list to
     * provide backup and restore services for BLE, IEEE802.15.4 and possibly
     * other modules */
    pau_ll_select_regdma_entry_link(hal->dev, 3);
    pau_ll_set_regdma_entry_link_backup_direction(hal->dev, backup_or_restore);
    pau_ll_set_regdma_entry_link_backup_start_enable(hal->dev);

    while (!(pau_ll_get_regdma_intr_raw_signal(hal->dev) & PAU_DONE_INT_RAW));
}

void pau_hal_stop_regdma_extra_link(pau_hal_context_t *hal)
{
    pau_ll_set_regdma_entry_link_backup_start_disable(hal->dev);
    pau_ll_select_regdma_entry_link(hal->dev, 0); /* restore link select to default */
    pau_ll_clear_regdma_backup_done_intr_state(hal->dev);
}

void IRAM_ATTR pau_hal_regdma_clock_configure(pau_hal_context_t *hal, bool enable)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.regdma_conf, regdma_rst_en, !enable);
    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.regdma_conf, regdma_clk_en, enable);
}
