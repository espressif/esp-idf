/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "hal/pau_hal.h"
#include "hal/pau_types.h"
#if SOC_PAU_IN_TOP_DOMAIN
#include "hal/lp_sys_ll.h"
#endif

void pau_hal_set_regdma_entry_link_addr(pau_hal_context_t *hal, pau_regdma_link_addr_t *link_addr)
{
    pau_ll_set_regdma_link0_addr(hal->dev, (*link_addr)[0]);
    pau_ll_set_regdma_link1_addr(hal->dev, (*link_addr)[1]);
    pau_ll_set_regdma_link2_addr(hal->dev, (*link_addr)[2]);
    /* The link 3 of REGDMA is reserved, PMU state switching will not use
     * REGDMA link 3 */
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
     * other modules */
    pau_ll_select_regdma_entry_link(hal->dev, 3);
    pau_ll_set_regdma_entry_link_backup_direction(hal->dev, backup_or_restore);
    pau_ll_set_regdma_entry_link_backup_start_enable(hal->dev);

    while (!(pau_ll_get_regdma_intr_raw_signal(hal->dev) & PAU_DONE_INT_RAW));
}

void IRAM_ATTR pau_hal_stop_regdma_extra_link(pau_hal_context_t *hal)
{
    pau_ll_set_regdma_entry_link_backup_start_disable(hal->dev);
    pau_ll_select_regdma_entry_link(hal->dev, 3); /* restore link select to default */
    pau_ll_clear_regdma_backup_done_intr_state(hal->dev);
}

#if SOC_PAU_IN_TOP_DOMAIN
void IRAM_ATTR pau_hal_lp_sys_initialize(void)
{
    lp_sys_ll_set_pau_aon_bypass(true);
    lp_sys_ll_set_pau_link_backup_tout_thres(300);
    lp_sys_ll_set_pau_link_tout_thres(200);
    lp_sys_ll_set_pau_reg_read_interval(50);
}
#endif
