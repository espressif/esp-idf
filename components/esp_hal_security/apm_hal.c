/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/interrupts.h"
#include "hal/assert.h"
#include "hal/apm_hal.h"
#include "hal/apm_ll.h"
#include "hal/log.h"
#if !SOC_APM_CTRL_FILTER_SUPPORTED
#include "hal/axi_icm_ll.h"
#include "hal/lp_sys_ll.h"
#endif

#if SOC_APM_CTRL_FILTER_SUPPORTED
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
#else
void apm_hal_dma_pms_set_region_bounds(uint32_t regn_num, uint32_t start_addr, uint32_t end_addr)
{
    apm_ll_dma_pms_set_region_start_addr(regn_num, start_addr);
    apm_ll_dma_pms_set_region_end_addr(regn_num, end_addr);
}

void apm_hal_dma_pms_set_master_region_attr(apm_master_dma_id_t mid, uint32_t regn_rd_mask, uint32_t regn_wr_mask)
{
    apm_ll_dma_pms_set_region_attr_r(mid, regn_rd_mask);
    apm_ll_dma_pms_set_region_attr_w(mid, regn_wr_mask);
}

void apm_hal_dma_pms_set_region_cfg(uint32_t regn_count, const apm_hal_pms_regn_cfg_t *regn_cfg)
{
    HAL_ASSERT(regn_cfg);

    for (uint32_t idx = 0; idx < regn_count; idx++) {
        const apm_hal_pms_regn_cfg_t *region = &regn_cfg[idx];
        apm_hal_dma_pms_set_region_bounds(region->regn_num, region->regn_start_addr, region->regn_end_addr);
    }
}

void apm_hal_dma_pms_set_master_cfg(uint32_t master_count, const apm_hal_pms_master_cfg_t *master_cfg)
{
    HAL_ASSERT(master_cfg);

    for (uint32_t idx = 0; idx < master_count; idx++) {
        const apm_hal_pms_master_cfg_t *master = &master_cfg[idx];
        apm_ll_dma_pms_set_region_attr_r(master->dma.id, master->dma.regn_pms_rd);
        apm_ll_dma_pms_set_region_attr_w(master->dma.id, master->dma.regn_pms_wr);
    }
}

void apm_hal_hp2lp_peri_pms_set_hpcpu_access(int core_id, apm_security_mode_t mode, uint32_t enable_mask)
{
    HAL_ASSERT(core_id < SOC_CPU_CORES_NUM);

    enable_mask &= APM_SLAVE_LP_PERI_ALL_MASK;
    for (int sid = 0; sid < APM_SLAVE_LP_PERIPH_MAX; sid++) {
        apm_ll_hp2lp_peri_pms_enable_hpcpu_access(core_id, mode, (apm_slave_lp_periph_t)sid,
                                                  (enable_mask >> sid) & 1U);
    }
}

void apm_hal_lp_peri_pms_set_lpcpu_access(uint32_t enable_mask)
{
    enable_mask &= APM_SLAVE_LP_PERI_ALL_MASK;
    for (int sid = 0; sid < APM_SLAVE_LP_PERIPH_MAX; sid++) {
        apm_ll_lp_peri_pms_enable_lpcpu_access((apm_slave_lp_periph_t)sid,
                                               (enable_mask >> sid) & 1U);
    }
}

void apm_hal_hp_peri_pms_set_hpcpu_access(int core_id, apm_security_mode_t mode, const apm_hal_pms_hp_peri_cfg_t *cfg)
{
    HAL_ASSERT(core_id < SOC_CPU_CORES_NUM);
    HAL_ASSERT(cfg);

    const uint32_t cpu_peri = cfg->cpu_peri & APM_SLAVE_CPU_PERI_ALL_MASK;
    const uint32_t hp_peri0 = cfg->hp_peri0 & APM_SLAVE_HP_PERI0_ALL_MASK;
    const uint64_t hp_peri1 = cfg->hp_peri1 & APM_SLAVE_HP_PERI1_ALL_MASK;

    /* CPU_PERI */
    for (int sid = 0; sid < APM_SLAVE_CPU_PERIPH_MAX; sid++) {
        apm_ll_hp_peri_pms_enable_hpcpu_cpu_peri_access(core_id, mode, (apm_slave_cpu_periph_id_t)sid,
                                                        (cpu_peri >> sid) & 1U);
    }
    /* HP_PERI0 */
    for (int sid = 0; sid < APM_SLAVE_HP_PERIPH0_MAX; sid++) {
        apm_ll_hp_peri_pms_enable_hpcpu_hp_peri0_access(core_id, mode, (apm_slave_hp_periph0_id_t)sid,
                                                        (hp_peri0 >> sid) & 1U);
    }
    /* HP_PERI1 */
    for (int sid = 0; sid < APM_SLAVE_HP_PERIPH1_MAX; sid++) {
        apm_ll_hp_peri_pms_enable_hpcpu_hp_peri1_access(core_id, mode, (apm_slave_hp_periph1_id_t)sid,
                                                        (hp_peri1 >> sid) & 1ULL);
    }
}

void apm_hal_lp2hp_peri_pms_set_lpcpu_access(const apm_hal_pms_hp_peri_cfg_t *cfg)
{
    HAL_ASSERT(cfg);

    const uint32_t cpu_peri = cfg->cpu_peri & APM_SLAVE_CPU_PERI_ALL_MASK;
    const uint32_t hp_peri0 = cfg->hp_peri0 & APM_SLAVE_HP_PERI0_ALL_MASK;
    const uint64_t hp_peri1 = cfg->hp_peri1 & APM_SLAVE_HP_PERI1_ALL_MASK;

    /* CPU_PERI */
    for (int sid = 0; sid < APM_SLAVE_CPU_PERIPH_MAX; sid++) {
        apm_ll_lp2hp_peri_pms_enable_lpcpu_cpu_peri_access((apm_slave_cpu_periph_id_t)sid,
                                                           (cpu_peri >> sid) & 1U);
    }
    /* HP_PERI0 */
    for (int sid = 0; sid < APM_SLAVE_HP_PERIPH0_MAX; sid++) {
        apm_ll_lp2hp_peri_pms_enable_lpcpu_hp_peri0_access((apm_slave_hp_periph0_id_t)sid,
                                                           (hp_peri0 >> sid) & 1U);
    }
    /* HP_PERI1 */
    for (int sid = 0; sid < APM_SLAVE_HP_PERIPH1_MAX; sid++) {
        apm_ll_lp2hp_peri_pms_enable_lpcpu_hp_peri1_access((apm_slave_hp_periph1_id_t)sid,
                                                           (hp_peri1 >> sid) & 1ULL);
    }
}

void apm_hal_lp_peri_pms_set_region_bounds(uint32_t regn_num, uint32_t start_addr, uint32_t end_addr)
{
    apm_ll_lp_peri_pms_set_region_start_addr(regn_num, start_addr);
    apm_ll_lp_peri_pms_set_region_end_addr(regn_num, end_addr);
}

void apm_hal_lp_peri_pms_set_master_region_attr(apm_master_id_t mid, apm_security_mode_t mode, uint32_t regn_pms_mask)
{
    switch (mid) {
    case APM_MASTER_HPCPU0:
        apm_ll_lp_peri_pms_hpcpu0_set_region_access(mode, regn_pms_mask);
        break;
    case APM_MASTER_HPCPU1:
        apm_ll_lp_peri_pms_hpcpu1_set_region_access(mode, regn_pms_mask);
        break;
    case APM_MASTER_LPCPU:
        apm_ll_lp_peri_pms_lpcpu_set_region_access(regn_pms_mask);
        break;
    default:
        break;
    }
}

void apm_hal_lp_peri_pms_set_region_cfg(uint32_t regn_count, const apm_hal_pms_regn_cfg_t *regn_cfg)
{
    HAL_ASSERT(regn_cfg);

    for (uint32_t idx = 0; idx < regn_count; idx++) {
        const apm_hal_pms_regn_cfg_t *region = &regn_cfg[idx];
        apm_hal_lp_peri_pms_set_region_bounds(region->regn_num, region->regn_start_addr, region->regn_end_addr);
    }
}

void apm_hal_lp_peri_pms_set_master_cfg(uint32_t master_count, const apm_hal_pms_master_cfg_t *master_cfg)
{
    HAL_ASSERT(master_cfg);

    for (int idx = 0; idx < master_count; idx++) {
        const apm_hal_pms_master_cfg_t *master = &master_cfg[idx];
        apm_hal_lp_peri_pms_set_master_region_attr(master->lp_peri.id, master->lp_peri.mode, master->lp_peri.regn_pms);
    }
}

void apm_hal_enable_intr(apm_ctrl_module_t ctrl_mod, bool enable)
{
    uint32_t mask = 0;

    switch (ctrl_mod) {
    case APM_CTRL_DMA_PMS:
    case APM_CTRL_HP_PERI_PMS:
    case APM_CTRL_LP2HP_PERI_PMS:
        mask = BIT(AXI_ICM_INTR_SYS_ADDRHOLE) | BIT(AXI_ICM_INTR_CPU_ADDRHOLE);
        axi_icm_ll_enable_intr(mask, enable);
        break;
    case APM_CTRL_LP_PERI_PMS:
    case APM_CTRL_HP2LP_PERI_PMS:
        mask = BIT(LP_SYS_INTR_LP_ADDRHOLE) | BIT(LP_SYS_INTR_IDBUS_ADDRHOLE);
        lp_sys_ll_enable_intr(mask, enable);
        break;
    default:
        (void)mask;
        break;
    }
}

void apm_hal_clear_intr(apm_ctrl_module_t ctrl_mod)
{
    uint32_t mask = 0;

    switch (ctrl_mod) {
    case APM_CTRL_DMA_PMS:
    case APM_CTRL_HP_PERI_PMS:
    case APM_CTRL_LP2HP_PERI_PMS:
        mask = BIT(AXI_ICM_INTR_SYS_ADDRHOLE) | BIT(AXI_ICM_INTR_CPU_ADDRHOLE);
        axi_icm_ll_clear_intr(mask);
        break;
    case APM_CTRL_LP_PERI_PMS:
    case APM_CTRL_HP2LP_PERI_PMS:
        mask = BIT(LP_SYS_INTR_LP_ADDRHOLE) | BIT(LP_SYS_INTR_IDBUS_ADDRHOLE);
        lp_sys_ll_clear_intr(mask);
        break;
    default:
        (void)mask;
        break;
    }
}

uint32_t apm_hal_get_intr_status(void)
{
    uint32_t status = 0, mask = 0;

    mask = BIT(AXI_ICM_INTR_SYS_ADDRHOLE) | BIT(AXI_ICM_INTR_CPU_ADDRHOLE);
    uint32_t hp_intr_st = axi_icm_ll_get_intr_status(mask);
    if (hp_intr_st & BIT(AXI_ICM_INTR_SYS_ADDRHOLE)) {
        status |= APM_EXCP_HP_AXI;
    }
    if (hp_intr_st & BIT(AXI_ICM_INTR_CPU_ADDRHOLE)) {
        status |= APM_EXCP_HP_AHB;
    }

    mask = BIT(LP_SYS_INTR_LP_ADDRHOLE) | BIT(LP_SYS_INTR_IDBUS_ADDRHOLE);
    uint32_t lp_intr_st = lp_sys_ll_get_intr_status(mask);
    if (lp_intr_st & BIT(LP_SYS_INTR_LP_ADDRHOLE)) {
        status |= APM_EXCP_LP_AHB;
    }
    if (lp_intr_st & BIT(LP_SYS_INTR_IDBUS_ADDRHOLE)) {
        status |= APM_EXCP_LP_IDBUS;
    }

    return status;
}

void apm_hal_get_exception_info(apm_ctrl_exception_type type, apm_ctrl_exception_info_t *excp_info)
{
    HAL_ASSERT(excp_info);

    switch (type) {
    case APM_EXCP_HP_AXI:
    case APM_EXCP_HP_AHB: {
        axi_icm_ll_excp_info_t info = {0};
        if (type == APM_EXCP_HP_AXI) {
            axi_icm_ll_get_sys_excp_info(&info);
            /* NOTE: Master ID = 4-bit CID + 4-bit UID, extracting only CID here */
            info.id = (info.id >> 0x04U) & 0x0FU;
        } else {
            axi_icm_ll_get_cpu_excp_info(&info);
        }
        excp_info->addr = info.addr;
        excp_info->id = info.id;
        excp_info->is_wr = info.is_wr;
        excp_info->is_secure = info.is_secure;
        break;
    }
    case APM_EXCP_LP_AHB:
    case APM_EXCP_LP_IDBUS: {
        lp_sys_ll_excp_info_t info = {0};
        if (type == APM_EXCP_LP_AHB) {
            lp_sys_ll_get_ahb_excp_info(&info);
        } else {
            lp_sys_ll_get_idbus_excp_info(&info);
        }
        excp_info->addr = info.addr;
        excp_info->id = info.id;
        excp_info->is_wr = info.is_wr;
        excp_info->is_secure = info.is_secure;
        break;
    }
    default:
        break;
    }
}

int apm_hal_get_intr_src_num(apm_ctrl_module_t ctrl_mod)
{
    switch (ctrl_mod) {
    case APM_CTRL_DMA_PMS:
    case APM_CTRL_HP_PERI_PMS:
    case APM_CTRL_LP2HP_PERI_PMS:
        return ETS_SYS_ICM_INTR_SOURCE;
    case APM_CTRL_LP_PERI_PMS:
    case APM_CTRL_HP2LP_PERI_PMS:
        return ETS_LP_SYSREG_INTR_SOURCE;
    default:
        return -1;
    }
}

void apm_hal_enable_ctrl_clk_gating(apm_ctrl_module_t ctrl_mod, bool enable)
{
    switch (ctrl_mod) {
    case APM_CTRL_DMA_PMS:
        apm_ll_dma_pms_enable_clk_gating(enable);
        break;
    case APM_CTRL_HP_PERI_PMS:
        apm_ll_hp_peri_pms_enable_clk_gating(enable);
        break;
    case APM_CTRL_HP2LP_PERI_PMS:
        apm_ll_hp2lp_peri_pms_enable_clk_gating(enable);
        break;
    case APM_CTRL_LP_PERI_PMS:
        apm_ll_lp_peri_pms_enable_clk_gating(enable);
        break;
    case APM_CTRL_LP2HP_PERI_PMS:
        apm_ll_lp2hp_peri_pms_enable_clk_gating(enable);
        break;
    default:
        break;
    }
}
#endif // SOC_APM_CTRL_FILTER_SUPPORTED
