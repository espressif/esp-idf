/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/apm_hal.h"
#include "hal/apm_ll.h"
#include "hal/log.h"

#if SOC_IS(ESP32P4)
void apm_hal_hp_peri_access_enable(apm_ll_master_id_t master_id, apm_ll_hp_peri_t hp_peri,
                                   apm_ll_secure_mode_t sec_mode, bool enable)
{
    apm_ll_hp_peri_access_enable(master_id, hp_peri, sec_mode, enable);
}

void apm_hal_lp_peri_access_enable(apm_ll_lp_peri_t lp_peri, bool enable)
{
    apm_ll_lp_peri_access_enable(lp_peri, enable);
}

void apm_hal_peri_region_config(uint32_t regn_num, uint32_t regn_low_addr, uint32_t regn_high_addr)
{
    apm_ll_peri_region_config(regn_num, regn_low_addr, regn_high_addr);
}

int apm_hal_peri_region_pms(apm_ll_master_id_t master_id, apm_ll_secure_mode_t sec_mode,
                            uint32_t regn_num, uint32_t regn_pms)
{
    return apm_ll_peri_region_pms(master_id, sec_mode, regn_num, regn_pms);
}

int apm_hal_apm_ctrl_clk_gating_enable(apm_ll_apm_ctrl_t apm_ctrl, bool enable)
{
    return apm_ll_apm_ctrl_clk_gating_enable(apm_ctrl, enable);
}

void apm_hal_dma_region_config(uint32_t regn_num, uint32_t regn_low_addr, uint32_t regn_high_addr)
{
    apm_ll_dma_region_set_low_address(regn_num, regn_low_addr);
    apm_ll_dma_region_set_high_address(regn_num, regn_high_addr);
}

void apm_hal_dma_region_pms(apm_hal_dma_region_config_data_t *pms_data)
{
    HAL_ASSERT(pms_data);

    apm_ll_dma_region_r_pms(pms_data->dma_master, pms_data->pms_r_mask);
    apm_ll_dma_region_w_pms(pms_data->dma_master, pms_data->pms_w_mask);
}
#else

void apm_hal_set_master_sec_mode(uint32_t master_mask, apm_security_mode_t mode)
{
    master_mask &= APM_MASTER_MASK_ALL;
    while (master_mask) {
        uint32_t master = __builtin_ctz(master_mask);
        master_mask &= ~(1U << master);
        apm_ll_hp_tee_set_master_sec_mode(master, mode);
#if SOC_APM_SUPPORT_LP_TEE_CTRL
        if (master == APM_MASTER_LPCORE) {
            apm_ll_lp_tee_set_master_sec_mode(master, mode);
        }
#endif
    }
}

void apm_hal_set_master_sec_mode_all(apm_security_mode_t mode)
{
    apm_hal_set_master_sec_mode(APM_MASTER_MASK_ALL, mode);
}

#if SOC_APM_SUPPORT_CTRL_CFG_LOCK
void apm_hal_lock_master_sec_mode(uint32_t master_mask)
{
    master_mask &= APM_MASTER_MASK_ALL;
    while (master_mask) {
        uint32_t master = __builtin_ctz(master_mask);
        master_mask &= ~(1U << master);
        apm_ll_hp_tee_lock_master_sec_mode(master);
#if SOC_APM_SUPPORT_LP_TEE_CTRL
        if (master == APM_MASTER_LPCORE) {
            apm_ll_lp_tee_lock_master_sec_mode(master);
        }
#endif
    }
}

void apm_hal_lock_master_sec_mode_all(void)
{
    apm_hal_lock_master_sec_mode(APM_MASTER_MASK_ALL);
}
#endif

#if SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL
void apm_hal_tee_set_peri_access(apm_tee_ctrl_module_t ctrl_mod, uint64_t periph_mask, apm_security_mode_t mode, apm_perm_t pms)
{
    switch (ctrl_mod) {
    case APM_TEE_CTRL_HP:
        uint64_t hp_tee_peri_mask = periph_mask & APM_TEE_HP_PERIPH_MASK_ALL;
        for (uint32_t periph = 0; periph < APM_TEE_HP_PERIPH_MAX; periph++) {
            if (hp_tee_peri_mask & (1ULL << periph)) {
                apm_ll_hp_tee_set_peri_access((apm_tee_hp_periph_t)periph, mode, pms);
            }
        }
        apm_ll_hp_tee_enable_bus_err_resp(true);
        break;
#if SOC_APM_SUPPORT_LP_TEE_CTRL
    case APM_TEE_CTRL_LP:
        uint32_t lp_tee_peri_mask = (uint32_t)periph_mask & (uint32_t)APM_TEE_LP_PERIPH_MASK_ALL;
        while (lp_tee_peri_mask) {
            uint32_t periph = __builtin_ctz(lp_tee_peri_mask);
            apm_ll_lp_tee_set_peri_access((apm_tee_lp_periph_t)periph, mode, pms);
            lp_tee_peri_mask &= ~(1U << periph);
        }
        apm_ll_lp_tee_enable_bus_err_resp(true);
        break;
#endif
    default:
        break;
    }
}

void apm_hal_tee_set_peri_access_all(apm_tee_ctrl_module_t ctrl_mod, apm_security_mode_t mode, apm_perm_t pms)
{
    switch (ctrl_mod) {
    case APM_TEE_CTRL_HP:
        apm_hal_tee_set_peri_access(APM_TEE_CTRL_HP, (uint64_t)(APM_TEE_HP_PERIPH_MASK_ALL), mode, pms);
        break;
#if SOC_APM_SUPPORT_LP_TEE_CTRL
    case APM_TEE_CTRL_LP:
        apm_hal_tee_set_peri_access(APM_TEE_CTRL_LP, (uint64_t)(APM_TEE_LP_PERIPH_MASK_ALL), mode, pms);
        break;
#endif
    default:
        break;
    }
}
#endif

void apm_hal_tee_enable_clk_gating(apm_tee_ctrl_module_t ctrl_mod, bool enable)
{
    switch (ctrl_mod) {
    case APM_TEE_CTRL_HP:
        apm_ll_hp_tee_enable_clk_gating(enable);
        break;
#if SOC_APM_SUPPORT_LP_TEE_CTRL
    case APM_TEE_CTRL_LP:
        apm_ll_lp_tee_enable_clk_gating(enable);
        break;
#endif
    default:
        break;
    }
}

void apm_hal_enable_ctrl_filter(apm_ctrl_module_t ctrl_mod, apm_ctrl_access_path_t path, bool enable)
{
    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_enable_ctrl_filter(path, enable);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_enable_ctrl_filter(path, enable);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_enable_ctrl_filter(path, enable);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_enable_ctrl_filter(path, enable);
        break;
#endif
    default:
        break;
    }
}

void apm_hal_enable_ctrl_filter_all(bool enable)
{
    apm_ll_hp_apm_enable_ctrl_filter_all(enable);
#if SOC_APM_LP_APM0_SUPPORTED
    apm_ll_lp_apm0_enable_ctrl_filter_all(enable);
#endif
    apm_ll_lp_apm_enable_ctrl_filter_all(enable);
#if SOC_APM_CPU_APM_SUPPORTED
    apm_ll_cpu_apm_enable_ctrl_filter_all(enable);
#endif
}

void apm_hal_enable_region_filter(apm_ctrl_module_t ctrl_mod, uint32_t regn_num, bool enable)
{
    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_enable_region_filter(regn_num, enable);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_enable_region_filter(regn_num, enable);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_enable_region_filter(regn_num, enable);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_enable_region_filter(regn_num, enable);
        break;
#endif
    default:
        break;
    }
}

void apm_hal_set_region_start_addr(apm_ctrl_module_t ctrl_mod, uint32_t regn_num, uint32_t addr)
{
    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_set_region_start_addr(regn_num, addr);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_set_region_start_addr(regn_num, addr);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_set_region_start_addr(regn_num, addr);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_set_region_start_addr(regn_num, addr);
        break;
#endif
    default:
        break;
    }
}

void apm_hal_set_region_end_addr(apm_ctrl_module_t ctrl_mod, uint32_t regn_num, uint32_t addr)
{
    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_set_region_end_addr(regn_num, addr);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_set_region_end_addr(regn_num, addr);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_set_region_end_addr(regn_num, addr);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_set_region_end_addr(regn_num, addr);
        break;
#endif
    default:
        break;
    }
}

void apm_hal_set_sec_mode_region_attr(apm_ctrl_module_t ctrl_mod, uint32_t regn_num, apm_security_mode_t mode, uint32_t regn_pms)
{
    HAL_ASSERT(mode != APM_SEC_MODE_TEE);

    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_set_sec_mode_region_attr(regn_num, mode, regn_pms);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_set_sec_mode_region_attr(regn_num, mode, regn_pms);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_set_sec_mode_region_attr(regn_num, mode, regn_pms);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_set_sec_mode_region_attr(regn_num, mode, regn_pms);
        break;
#endif
    default:
        break;
    }
}

void apm_hal_set_region_filter_cfg(apm_ctrl_module_t ctrl_mod, apm_security_mode_t mode, const apm_hal_ctrl_region_cfg_t *regn_cfg)
{
    HAL_ASSERT(regn_cfg);
    HAL_ASSERT(mode != APM_SEC_MODE_TEE);

    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_set_region_start_addr(regn_cfg->regn_num, regn_cfg->regn_start_addr);
        apm_ll_hp_apm_set_region_end_addr(regn_cfg->regn_num, regn_cfg->regn_end_addr);
        apm_ll_hp_apm_set_sec_mode_region_attr(regn_cfg->regn_num, mode, regn_cfg->regn_pms);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_set_region_start_addr(regn_cfg->regn_num, regn_cfg->regn_start_addr);
        apm_ll_lp_apm0_set_region_end_addr(regn_cfg->regn_num, regn_cfg->regn_end_addr);
        apm_ll_lp_apm0_set_sec_mode_region_attr(regn_cfg->regn_num, mode, regn_cfg->regn_pms);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_set_region_start_addr(regn_cfg->regn_num, regn_cfg->regn_start_addr);
        apm_ll_lp_apm_set_region_end_addr(regn_cfg->regn_num, regn_cfg->regn_end_addr);
        apm_ll_lp_apm_set_sec_mode_region_attr(regn_cfg->regn_num, mode, regn_cfg->regn_pms);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_set_region_start_addr(regn_cfg->regn_num, regn_cfg->regn_start_addr);
        apm_ll_cpu_apm_set_region_end_addr(regn_cfg->regn_num, regn_cfg->regn_end_addr);
        apm_ll_cpu_apm_set_sec_mode_region_attr(regn_cfg->regn_num, mode, regn_cfg->regn_pms);
        break;
#endif
    default:
        break;
    }
}

#if SOC_APM_SUPPORT_CTRL_CFG_LOCK
void apm_hal_lock_region_filter_cfg(apm_ctrl_module_t ctrl_mod, uint32_t regn_num)
{
    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_lock_sec_mode_region_attr(regn_num);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_lock_sec_mode_region_attr(regn_num);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_lock_sec_mode_region_attr(regn_num);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_lock_sec_mode_region_attr(regn_num);
        break;
#endif
    default:
        break;
    }
}
#endif

void apm_hal_set_ctrl_sec_mode_cfg(const apm_hal_ctrl_sec_mode_cfg_t *cfg)
{
    HAL_ASSERT(cfg);
    HAL_ASSERT(cfg->regions);

    for (uint32_t regn_idx = 0; regn_idx < cfg->regn_count; regn_idx++) {
        const apm_hal_ctrl_region_cfg_t *region = &cfg->regions[regn_idx];
        apm_hal_set_region_filter_cfg(cfg->ctrl_mod, cfg->mode, region);
        apm_hal_enable_region_filter(cfg->ctrl_mod, region->regn_num, region->filter_en);
#if SOC_APM_SUPPORT_CTRL_CFG_LOCK
        if (region->lock_en) {
            apm_hal_lock_region_filter_cfg(cfg->ctrl_mod, region->regn_num);
        }
#endif
        apm_hal_enable_ctrl_filter(cfg->ctrl_mod, region->path_id, true);
    }
}

uint32_t apm_hal_get_exception_type(apm_hal_ctrl_info_t *ctrl_info)
{
    HAL_ASSERT(ctrl_info);

    switch (ctrl_info->ctrl_mod) {
    case APM_CTRL_HP_APM:
        return apm_ll_hp_apm_get_excp_type(ctrl_info->path);
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        return apm_ll_lp_apm0_get_excp_type(ctrl_info->path);
#endif
    case APM_CTRL_LP_APM:
        return apm_ll_lp_apm_get_excp_type(ctrl_info->path);
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        return apm_ll_cpu_apm_get_excp_type(ctrl_info->path);
#endif
    default:
        return UINT8_MAX;
    }
}

void apm_hal_clear_exception_status(apm_hal_ctrl_info_t *ctrl_info)
{
    HAL_ASSERT(ctrl_info);

    switch (ctrl_info->ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_clear_ctrl_excp_status(ctrl_info->path);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_clear_ctrl_excp_status(ctrl_info->path);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_clear_ctrl_excp_status(ctrl_info->path);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_clear_ctrl_excp_status(ctrl_info->path);
        break;
#endif
    default:
        break;
    }
}

void apm_hal_get_exception_info(apm_hal_ctrl_info_t *ctrl_info, apm_ctrl_exception_info_t *excp_info)
{
    HAL_ASSERT(ctrl_info);
    HAL_ASSERT(excp_info);

    switch (ctrl_info->ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_get_excp_info(ctrl_info->path, excp_info);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_get_excp_info(ctrl_info->path, excp_info);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_get_excp_info(ctrl_info->path, excp_info);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_get_excp_info(ctrl_info->path, excp_info);
        break;
#endif
    default:
        break;
    }
}

void apm_hal_enable_intr(apm_hal_ctrl_info_t *ctrl_info, bool enable)
{
    HAL_ASSERT(ctrl_info);

    switch (ctrl_info->ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_enable_ctrl_intr(ctrl_info->path, enable);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_enable_ctrl_intr(ctrl_info->path, enable);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_enable_ctrl_intr(ctrl_info->path, enable);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_enable_ctrl_intr(ctrl_info->path, enable);
        break;
#endif
    default:
        break;
    }
}

int apm_hal_get_intr_src_num(apm_hal_ctrl_info_t *ctrl_info)
{
    HAL_ASSERT(ctrl_info);

    switch (ctrl_info->ctrl_mod) {
    case APM_CTRL_HP_APM:
        return apm_ll_hp_apm_get_ctrl_intr_src(ctrl_info->path);
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        return apm_ll_lp_apm0_get_ctrl_intr_src(ctrl_info->path);
#endif
    case APM_CTRL_LP_APM:
        return apm_ll_lp_apm_get_ctrl_intr_src(ctrl_info->path);
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        return apm_ll_cpu_apm_get_ctrl_intr_src(ctrl_info->path);
#endif
    default:
        return -1;
    }
}

void apm_hal_enable_reset_event_bypass(bool enable)
{
    apm_ll_enable_reset_event_bypass(enable);
}

void apm_hal_enable_ctrl_clk_gating(apm_ctrl_module_t ctrl_mod, bool enable)
{
    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        apm_ll_hp_apm_enable_ctrl_clk_gating(enable);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        apm_ll_lp_apm0_enable_ctrl_clk_gating(enable);
        break;
#endif
    case APM_CTRL_LP_APM:
        apm_ll_lp_apm_enable_ctrl_clk_gating(enable);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        apm_ll_cpu_apm_enable_ctrl_clk_gating(enable);
        break;
#endif
    default:
        break;
    }
}

#endif //SOC_IS(ESP32P4)
