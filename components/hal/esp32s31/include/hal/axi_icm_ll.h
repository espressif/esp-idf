/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "soc/icm_sys_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set QoS priority for DMA2D master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_dma2d_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    ICM_SYS.mst_awqos_reg0.reg_dma2d_awqos = write_prio;
    ICM_SYS.mst_arqos_reg0.reg_dma2d_arqos = read_prio;
}

/**
 * @brief Set QoS priority for AXI-GDMA master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_gdma_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    ICM_SYS.mst_awqos_reg0.reg_pdma_int_awqos = write_prio;
    ICM_SYS.mst_arqos_reg0.reg_axi_pdma_int_arqos = read_prio;
}

/**
 * @brief Set QoS priority for GMAC master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_gmac_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    ICM_SYS.mst_awqos_reg0.reg_gmac_awqos = write_prio;
    ICM_SYS.mst_arqos_reg0.reg_gmac_arqos = read_prio;
}

/**
 * @brief Set QoS priority for SDMMC master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_sdmmc_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    ICM_SYS.mst_awqos_reg0.reg_axi_sdmmc_int_awqos = write_prio;
    ICM_SYS.mst_arqos_reg0.reg_axi_sdmmc_int_arqos = read_prio;
}

/**
 * @brief Set QoS priority for Cache master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_cache_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    ICM_SYS.mst_awqos_reg0.reg_cache_awqos = write_prio;
    ICM_SYS.mst_arqos_reg0.reg_cache_arqos = read_prio;
}

/**
 * @brief Set QoS priority for CPU master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_cpu_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    ICM_SYS.mst_awqos_reg0.reg_cpu_awqos = write_prio;
    ICM_SYS.mst_arqos_reg0.reg_cpu_arqos = read_prio;
}

#ifdef __cplusplus
}
#endif
