/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/assert.h"
#include "hal/apm_hal.h"
#include "hal/apm_ll.h"

void apm_tee_hal_set_master_secure_mode(apm_ll_master_id_t master_id, apm_ll_secure_mode_t sec_mode)
{
    apm_tee_ll_set_master_secure_mode(master_id, sec_mode);
}

void apm_tee_hal_clk_gating_enable(bool enable)
{
    apm_tee_ll_clk_gating_enable(enable);
}

void apm_hp_hal_region_filter_enable(uint32_t regn_num, bool enable)
{
    apm_hp_ll_region_filter_enable(regn_num, enable);
}

void apm_hp_hal_m_filter_enable(apm_ll_hp_access_path_t hp_m_path, bool enable)
{
    apm_hp_ll_m_filter_enable(hp_m_path, enable);
}

void apm_hp_hal_region_config(const apm_hp_hal_region_config_data_t *pms_data)
{
    HAL_ASSERT((!pms_data) || (pms_data->regn_num > APM_LL_HP_MAX_REGION_NUM));

    apm_hp_ll_set_region_start_address(pms_data->regn_num, pms_data->regn_start_addr);
    apm_hp_ll_set_region_end_address(pms_data->regn_num, pms_data->regn_end_addr);
    apm_hp_ll_sec_mode_region_attr_config(pms_data->regn_num, pms_data->sec_mode, pms_data->regn_pms);
}

uint8_t apm_hp_hal_m_exception_status(apm_ll_hp_access_path_t hp_m_path)
{
    return apm_hp_ll_m_exception_status(hp_m_path);
}

void apm_hp_hal_m_exception_clear(apm_ll_hp_access_path_t hp_m_path)
{
    apm_hp_ll_m_exception_clear(hp_m_path);
}

void apm_hp_hal_get_m_exception_info(apm_hp_m_exception_info_t *excp_info)
{
    apm_hp_ll_get_m_exception_info(excp_info);
}

void apm_hp_hal_m_interrupt_enable(apm_ll_hp_access_path_t hp_m_path, bool enable)
{
    apm_hp_ll_m_interrupt_enable(hp_m_path, enable);
}

void apm_hp_hal_clk_gating_enable(bool enable)
{
    apm_hp_ll_clk_gating_enable(enable);
}

/* TBD: IDF-6759 */
void apm_hp_hal_master_sec_mode_config(apm_hp_secure_mode_config_t *sec_mode_data)
{
    for (int i = 0; i < APM_LL_MASTER_MAX; i++) {
        if (sec_mode_data->master_ids & (1 << i)) {
            apm_tee_hal_set_master_secure_mode(i, sec_mode_data->sec_mode);
        }
    }
    sec_mode_data->pms_data->sec_mode = sec_mode_data->sec_mode;
    apm_hp_hal_region_config(sec_mode_data->pms_data);
}

void apm_hp_hal_reset_event_enable(bool enable)
{
    apm_hp_ll_reset_event_enable(enable);
}
