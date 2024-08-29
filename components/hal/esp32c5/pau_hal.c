/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "esp_attr.h"
#include "hal/pau_hal.h"
#include "hal/pau_types.h"
#include "hal/lp_aon_ll.h"

void pau_hal_set_regdma_entry_link_addr(pau_hal_context_t *hal, pau_regdma_link_addr_t *link_addr)
{
    lp_aon_ll_set_regdma_link_addr((uint32_t)(*link_addr)[0]);
}

void IRAM_ATTR pau_hal_start_regdma_modem_link(pau_hal_context_t *hal, bool backup_or_restore)
{
    pau_ll_clear_regdma_backup_done_intr_state(hal->dev);
    pau_ll_set_regdma_select_wifimac_link(hal->dev);
    pau_ll_set_regdma_wifimac_link_backup_direction(hal->dev, backup_or_restore);
    pau_ll_set_regdma_wifimac_link_backup_start_enable(hal->dev);

    while (!(pau_ll_get_regdma_intr_raw_signal(hal->dev) & PAU_DONE_INT_RAW));
}

void IRAM_ATTR pau_hal_stop_regdma_modem_link(pau_hal_context_t *hal)
{
    pau_ll_set_regdma_wifimac_link_backup_start_disable(hal->dev);
    pau_ll_set_regdma_deselect_wifimac_link(hal->dev);
    pau_ll_clear_regdma_backup_done_intr_state(hal->dev);
}

void IRAM_ATTR pau_hal_start_regdma_extra_link(pau_hal_context_t *hal, bool backup_or_restore)
{
    pau_ll_clear_regdma_backup_done_intr_state(hal->dev);
    /* The link 3 of REGDMA is reserved, we use it as an extra linked list to
     * provide backup and restore services for BLE, IEEE802.15.4 and possibly
     * other modules.
     * It is also used as software trigger REGDMA to backup and restore, and is
     * used by the UT to test module driver retention function.
     */
    pau_ll_select_regdma_entry_link(hal->dev, 3);
    pau_ll_set_regdma_entry_link_backup_direction(hal->dev, backup_or_restore);
    pau_ll_set_regdma_entry_link_backup_start_enable(hal->dev);

    while (!(pau_ll_get_regdma_intr_raw_signal(hal->dev) & PAU_DONE_INT_RAW));
}

void IRAM_ATTR pau_hal_stop_regdma_extra_link(pau_hal_context_t *hal)
{
    pau_ll_set_regdma_entry_link_backup_start_disable(hal->dev);
    pau_ll_select_regdma_entry_link(hal->dev, 0); /* restore link select to default */
    pau_ll_clear_regdma_backup_done_intr_state(hal->dev);
}

#if SOC_PM_PAU_REGDMA_LINK_CONFIGURABLE
void pau_hal_regdma_link_count_config(pau_hal_context_t *hal, int count)
{
    HAL_ASSERT(count > 0);
    lp_aon_ll_set_regdma_link_count(count - 1);
}
#endif

void pau_hal_set_regdma_work_timeout(pau_hal_context_t *hal, uint32_t loop_num, uint32_t time)
{
    HAL_ASSERT(loop_num > 0 && time > 0);
    lp_aon_ll_set_regdma_link_loop_threshold(loop_num);
    lp_aon_ll_set_regdma_link_reg_access_tout_threshold(time);
}

void pau_hal_set_regdma_wait_timeout(pau_hal_context_t *hal, int count, int interval)
{
    HAL_ASSERT(count > 0 && interval > 0);
    lp_aon_ll_set_regdma_link_wait_retry_count(count);
    lp_aon_ll_set_regdma_link_wait_read_interval(interval);
}
