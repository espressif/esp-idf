/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/assert.h"
#include "hal/apm_hal.h"
#include "hal/apm_ll.h"
#include "hal/log.h"

void apm_tee_hal_set_master_secure_mode(apm_ll_apm_ctrl_t apm_ctrl, apm_ll_master_id_t master_id, apm_ll_secure_mode_t sec_mode)
{
    apm_tee_ll_set_master_secure_mode(apm_ctrl, master_id, sec_mode);
}

void apm_tee_hal_clk_gating_enable(bool enable)
{
    apm_tee_ll_clk_gating_enable(enable);
}

void apm_hal_apm_ctrl_region_filter_enable(apm_ll_apm_ctrl_t apm_ctrl, uint32_t regn_num, bool enable)
{
    apm_ll_apm_ctrl_region_filter_enable(apm_ctrl, regn_num, enable);
}

void apm_hal_apm_ctrl_filter_enable(apm_ctrl_path_t *apm_path, bool enable)
{
    apm_ll_apm_ctrl_filter_enable(apm_path->apm_ctrl, apm_path->apm_m_path, enable);
}

void apm_hal_apm_ctrl_filter_enable_all(bool enable)
{
    apm_ctrl_path_t apm_path;

    for (int i = 0; i < HP_APM_MAX_ACCESS_PATH; i++) {
        apm_path.apm_ctrl = HP_APM_CTRL;
        apm_path.apm_m_path = i;
        apm_hal_apm_ctrl_filter_enable(&apm_path, enable);
    }
    for (int i = 0; i < LP_APM_MAX_ACCESS_PATH; i++) {
        apm_path.apm_ctrl = LP_APM_CTRL;
        apm_path.apm_m_path = i;
        apm_hal_apm_ctrl_filter_enable(&apm_path, enable);
    }
#if CONFIG_IDF_TARGET_ESP32C6
    for (int i = 0; i < LP_APM0_MAX_ACCESS_PATH; i++) {
        apm_path.apm_ctrl = LP_APM0_CTRL;
        apm_path.apm_m_path = i;
        apm_hal_apm_ctrl_filter_enable(&apm_path, enable);
    }

#endif
}

void apm_hal_apm_ctrl_region_config(const apm_ctrl_region_config_data_t *pms_data)
{
    HAL_ASSERT(pms_data);

    apm_ll_apm_ctrl_set_region_start_address(pms_data->apm_ctrl, pms_data->regn_num, pms_data->regn_start_addr);
    apm_ll_apm_ctrl_set_region_end_address(pms_data->apm_ctrl, pms_data->regn_num, pms_data->regn_end_addr);
    apm_ll_apm_ctrl_sec_mode_region_attr_config(pms_data->apm_ctrl, pms_data->regn_num, pms_data->sec_mode, pms_data->regn_pms);
}

uint8_t apm_hal_apm_ctrl_exception_status(apm_ctrl_path_t *apm_path)
{
    return apm_ll_apm_ctrl_exception_status(apm_path->apm_ctrl, apm_path->apm_m_path);
}

void apm_hal_apm_ctrl_exception_clear(apm_ctrl_path_t *apm_path)
{
    apm_ll_apm_ctrl_exception_clear(apm_path->apm_ctrl, apm_path->apm_m_path);
}

void apm_hal_apm_ctrl_get_exception_info(apm_ctrl_exception_info_t *excp_info)
{
    apm_ll_apm_ctrl_get_exception_info(excp_info);
}

void apm_hal_apm_ctrl_interrupt_enable(apm_ctrl_path_t *apm_path, bool enable)
{
    apm_ll_apm_ctrl_interrupt_enable(apm_path->apm_ctrl, apm_path->apm_m_path, enable);
}

void apm_hal_apm_ctrl_clk_gating_enable(apm_ll_apm_ctrl_t apm_ctrl, bool enable)
{
    apm_ll_apm_ctrl_clk_gating_enable(apm_ctrl, enable);
}

void apm_hal_apm_ctrl_master_sec_mode_config(apm_ctrl_secure_mode_config_t *sec_mode_data)
{
    apm_ctrl_path_t apm_path;

    /* Configure given secure mode for all specified Masters. */
    for (int i = 0; i < APM_LL_MASTER_MAX; i++) {
        if (sec_mode_data->master_ids & (1 << i)) {
            apm_tee_hal_set_master_secure_mode(sec_mode_data->apm_ctrl, i, sec_mode_data->sec_mode);
        }
    }

    /* Configure the given APM Ctrl for all Masters for the:
     * - Secure mode,
     * - Regions range,
     * - access permissions and
     * - region filter
     */
    for (int i = 0; i < sec_mode_data->regn_count; i++) {
        sec_mode_data->pms_data[i].sec_mode = sec_mode_data->sec_mode;
        sec_mode_data->pms_data[i].apm_ctrl = sec_mode_data->apm_ctrl;
        apm_hal_apm_ctrl_region_config(&sec_mode_data->pms_data[i]);
        apm_hal_apm_ctrl_region_filter_enable(sec_mode_data->pms_data[i].apm_ctrl, i,
                                              sec_mode_data->pms_data[i].filter_enable);
    }

    /* Configure APM Ctrl access path(M[0:n]) */
    for (int i = 0; i < sec_mode_data->apm_m_cnt; i++) {
        apm_path.apm_ctrl = sec_mode_data->apm_ctrl;
        apm_path.apm_m_path = i;
        apm_hal_apm_ctrl_filter_enable(&apm_path, 1);
    }
}

void apm_hal_apm_ctrl_reset_event_enable(bool enable)
{
    apm_ll_apm_ctrl_reset_event_enable(enable);
}

esp_err_t apm_hal_apm_ctrl_get_int_src_num(apm_ctrl_path_t *apm_path)
{
    return apm_ll_apm_ctrl_get_int_src_num(apm_path->apm_ctrl, apm_path->apm_m_path);
}
