/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal/assert.h"
#include "soc/icm_sys_qos_struct.h"
#include "soc/icm_sys_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AXI_ICM_MASTER_CPU = 0,        // An aggregate master port for other users, like HP CPU, LP CPU, USB, EMAC, SDMMC, AHB-GDMA, etc
    AXI_ICM_MASTER_CACHE = 1,      // Cache master port
    AXI_ICM_MASTER_DW_GDMA_M0 = 5, // DW-GDMA master port 0
    AXI_ICM_MASTER_DW_GDMA_M1 = 6, // DW-GDMA master port 1
    AXI_ICM_MASTER_GDMA = 8,       // AXI-GDMA
    AXI_ICM_MASTER_DMA2D = 10,     // DMA2D
    AXI_ICM_MASTER_H264_M0 = 11,   // H264 master port 0
    AXI_ICM_MASTER_H264_M1 = 12,   // H264 master port 1
} axi_icm_ll_master_id_t;

/**
 * @brief Set QoS burstiness for a master port, also enable the regulator
 *
 * @param mid Master port ID
 * @param burstiness Burstiness value. It represents the depth of the token bucket.
 */
static inline void axi_icm_ll_set_qos_burstiness(axi_icm_ll_master_id_t mid, uint32_t burstiness)
{
    HAL_ASSERT(burstiness >= 1 && burstiness <= 256);
    // wait for the previous command to finish
    while (AXI_ICM_QOS.cmd.reg_axi_cmd_en);
    // write data register
    // data[23:16] - burstiness, data[0] - enable regulator
    AXI_ICM_QOS.data.val = (burstiness - 1) << 16 | 0x1;
    // command write operation
    AXI_ICM_QOS.cmd.reg_axi_rd_wr_cmd = 1;
    // write addr channel
    AXI_ICM_QOS.cmd.reg_rd_wr_chan = 1;
    // select master port
    AXI_ICM_QOS.cmd.reg_axi_master_port = mid;
    // set command type: burstiness regulator
    AXI_ICM_QOS.cmd.reg_axi_cmd = 0;
    // command enable bit
    AXI_ICM_QOS.cmd.reg_axi_cmd_en = 1;
    // wait for the data to be synced to internal register
    while (AXI_ICM_QOS.cmd.reg_axi_cmd_en);
}

/**
 * @brief Set QoS peak and transaction rate for a master port
 *
 * @note Relationship between "level" and fractional rate:
 *      level = 0: 1/2
 *      level = 1: 1/4
 *      ...
 *      level = 11: 1/4096
 *
 * @note if the transaction rate is set to 1/N, every N cycles, the master port can accept one transfer request (offer a token).
 *
 * @param mid Master port ID
 * @param peak_level Peak level, lower value means higher rate
 * @param transaction_level Transaction level, lower value means higher rate
 */
static inline void axi_icm_ll_set_qos_peak_transaction_rate(axi_icm_ll_master_id_t mid, uint32_t peak_level, uint32_t transaction_level)
{
    HAL_ASSERT(peak_level < transaction_level && transaction_level <= 11);
    while (AXI_ICM_QOS.cmd.reg_axi_cmd_en);
    // program data register
    // data[31:20] - peak_rate, data[15:4] - transaction_rate
    AXI_ICM_QOS.data.val = (0x80000000 >> peak_level) + (0x8000 >> transaction_level);
    // command write operation
    AXI_ICM_QOS.cmd.reg_axi_rd_wr_cmd = 1;
    // write addr channel
    AXI_ICM_QOS.cmd.reg_rd_wr_chan = 1;
    // select master port
    AXI_ICM_QOS.cmd.reg_axi_master_port = mid;
    // set command type: peak rate xct rate
    AXI_ICM_QOS.cmd.reg_axi_cmd = 1;
    // command enable bit
    AXI_ICM_QOS.cmd.reg_axi_cmd_en = 1;
    // wait for the data to be synced to internal register
    while (AXI_ICM_QOS.cmd.reg_axi_cmd_en);
}

/**
 * @brief Set QoS priority for DMA2D master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_dma2d_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    AXI_ICM.mst_awqos_reg0.reg_dma2d_awqos = write_prio;
    AXI_ICM.mst_arqos_reg0.reg_dma2d_arqos = read_prio;
}

/**
 * @brief Set QoS priority for AXI-GDMA master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_gdma_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    AXI_ICM.mst_awqos_reg0.reg_pdma_int_awqos = write_prio;
    AXI_ICM.mst_arqos_reg0.reg_axi_pdma_int_arqos = read_prio;
}

/**
 * @brief Set QoS priority for DW_GDMA master port
 *
 * @param master_port DW_GDMA master port ID
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_dw_gdma_qos_arbiter_prio(uint32_t master_port, uint32_t write_prio, uint32_t read_prio)
{
    if (master_port == 0) {
        AXI_ICM.mst_awqos_reg0.reg_gdma_mst1_awqos = write_prio;
        AXI_ICM.mst_arqos_reg0.reg_gdma_mst1_arqos = read_prio;
    } else {
        AXI_ICM.mst_awqos_reg0.reg_gdma_mst2_awqos = write_prio;
        AXI_ICM.mst_arqos_reg0.reg_gdma_mst2_arqos = read_prio;
    }
}

/**
 * @brief Set QoS priority for H264 master port
 *
 * @param master_port H264 master port ID
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_h264_dma_qos_arbiter_prio(uint32_t master_port, uint32_t write_prio, uint32_t read_prio)
{
    if (master_port == 0) {
        AXI_ICM.mst_awqos_reg0.reg_h264_dma2d_m1_awqos = write_prio;
        AXI_ICM.mst_arqos_reg0.reg_h264_dma2d_m1_arqos = read_prio;
    } else {
        AXI_ICM.mst_awqos_reg0.reg_h264_dma2d_m2_awqos = write_prio;
        AXI_ICM.mst_arqos_reg0.reg_h264_dma2d_m2_arqos = read_prio;
    }
}

/**
 * @brief Set QoS priority for Cache master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_cache_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    AXI_ICM.mst_awqos_reg0.reg_cache_awqos = write_prio;
    AXI_ICM.mst_arqos_reg0.reg_cache_arqos = read_prio;
}

/**
 * @brief Set QoS priority for CPU master port
 *
 * @param write_prio Write priority
 * @param read_prio Read priority
 */
static inline void axi_icm_ll_set_cpu_qos_arbiter_prio(uint32_t write_prio, uint32_t read_prio)
{
    AXI_ICM.mst_awqos_reg0.reg_cpu_awqos = write_prio;
    AXI_ICM.mst_arqos_reg0.reg_cpu_arqos = read_prio;
}

#ifdef __cplusplus
}
#endif
