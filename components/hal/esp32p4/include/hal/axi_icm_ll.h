/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
    AXI_ICM_MASTER_CPU        = 0,  // An aggregate master port for other users, like HP CPU, LP CPU, USB, EMAC, SDMMC, AHB-GDMA, etc
    AXI_ICM_MASTER_CACHE      = 1,  // Cache master port
    AXI_ICM_MASTER_DW_GDMA_M0 = 5,  // DW-GDMA master port 0
    AXI_ICM_MASTER_DW_GDMA_M1 = 6,  // DW-GDMA master port 1
    AXI_ICM_MASTER_GDMA       = 8,  // AXI-GDMA
    AXI_ICM_MASTER_DMA2D      = 10, // DMA2D
    AXI_ICM_MASTER_H264_M0    = 11, // H264 master port 0
    AXI_ICM_MASTER_H264_M1    = 12, // H264 master port 1
} axi_icm_ll_sys_master_id_t;

typedef enum {
    AXI_ICM_CPU_MASTER_HP_CPU0       = 0,   // HP CPU core 0
    AXI_ICM_CPU_MASTER_HP_CPU1       = 1,   // HP CPU core 1
    AXI_ICM_CPU_MASTER_LP_CPU        = 2,   // LP CPU
    AXI_ICM_CPU_MASTER_USBOTG_FS     = 3,   // USB OTG full-speed
    AXI_ICM_CPU_MASTER_REGDMA        = 4,   // REGDMA
    AXI_ICM_CPU_MASTER_GMAC          = 5,   // GMAC
    AXI_ICM_CPU_MASTER_SDMMC         = 6,   // SDMMC
    AXI_ICM_CPU_MASTER_USBOTG_HS     = 7,   // USB OTG high-speed
    AXI_ICM_CPU_MASTER_TRACE0        = 8,   // Trace 0
    AXI_ICM_CPU_MASTER_TRACE1        = 9,   // Trace 1
    AXI_ICM_CPU_MASTER_TCM_MON       = 10,  // TCM monitor
    AXI_ICM_CPU_MASTER_L2MEM_MON     = 11,  // L2MEM monitor
    AXI_ICM_CPU_MASTER_AHB_PDMA_I3C  = 16,  // AHB PDMA I3C
    AXI_ICM_CPU_MASTER_AHB_PDMA_UHCI = 18,  // AHB PDMA UHCI
    AXI_ICM_CPU_MASTER_AHB_PDMA_I2S0 = 19,  // AHB PDMA I2S0
    AXI_ICM_CPU_MASTER_AHB_PDMA_I2S1 = 20,  // AHB PDMA I2S1
    AXI_ICM_CPU_MASTER_AHB_PDMA_I2S2 = 21,  // AHB PDMA I2S
    AXI_ICM_CPU_MASTER_AHB_PDMA_ADC  = 24,  // AHB PDMA ADC
    AXI_ICM_CPU_MASTER_AHB_PDMA_RMT  = 26,  // AHB PDMA RMT
    AXI_ICM_CPU_MASTER_MAX           = 31,
} axi_icm_ll_cpu_master_id_t;

/**
 * @brief AXI ICM has independent channels for read and write access.
 */
typedef enum {
    AXI_ICM_ACCESS_READ = 0,
    AXI_ICM_ACCESS_WRITE = 1,
} axi_icm_ll_access_type_t;

/**
 * @brief AXI ICM interrupt types
 */
typedef enum {
    AXI_ICM_INTR_DEADLOCK     = 0,  /*!< Deadlock interrupt */
    AXI_ICM_INTR_SYS_ADDRHOLE = 1,  /*!< System addrhole interrupt */
    AXI_ICM_INTR_CPU_ADDRHOLE = 2,  /*!< CPU addrhole interrupt */
} axi_icm_ll_intr_type_t;

/**
 * @brief AXI ICM addrhole exception info
 */
typedef struct {
    uint32_t addr;      /*!< Faulting address */
    uint8_t id;         /*!< Master ID */
    bool is_wr;         /*!< Write access */
    bool is_secure;     /*!< Secure access */
} axi_icm_ll_excp_info_t;

/**
 * @brief Set QoS burstiness for a master port, also enable the regulator
 *
 * @param mid Master port ID
 * @param burstiness Burstiness value. It represents the depth of the token bucket.
 * @param access_type 0: read, 1: write
 */
static inline void axi_icm_ll_set_qos_burstiness(axi_icm_ll_sys_master_id_t mid, uint32_t burstiness, axi_icm_ll_access_type_t access_type)
{
    HAL_ASSERT(burstiness >= 1 && burstiness <= 256);
    // wait for the previous command to finish
    while (AXI_ICM_QOS.cmd.reg_axi_cmd_en);
    // write data register
    // data[23:16] - burstiness, data[0] - enable regulator
    AXI_ICM_QOS.data.val = (burstiness - 1) << 16 | 0x1;
    // command write operation
    AXI_ICM_QOS.cmd.reg_axi_rd_wr_cmd = 1;
    // set the qos for read channel or write channel
    AXI_ICM_QOS.cmd.reg_rd_wr_chan = access_type;
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
 * @param access_type 0: read, 1: write
 */
static inline void axi_icm_ll_set_qos_peak_transaction_rate(axi_icm_ll_sys_master_id_t mid, uint32_t peak_level,
                                                            uint32_t transaction_level, axi_icm_ll_access_type_t access_type)
{
    HAL_ASSERT(peak_level < transaction_level && transaction_level <= 11);
    while (AXI_ICM_QOS.cmd.reg_axi_cmd_en);
    // program data register
    // data[31:20] - peak_rate, data[15:4] - transaction_rate
    AXI_ICM_QOS.data.val = (0x80000000 >> peak_level) + (0x8000 >> transaction_level);
    // command write operation
    AXI_ICM_QOS.cmd.reg_axi_rd_wr_cmd = 1;
    // set the qos for read channel or write channel
    AXI_ICM_QOS.cmd.reg_rd_wr_chan = access_type;
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

/**
 * @brief Enable/disable AXI ICM interrupt
 *
 * @param mask Interrupt mask (use BIT(axi_icm_ll_intr_type_t))
 * @param enable true to enable, false to disable
 */
static inline void axi_icm_ll_enable_intr(uint32_t mask, bool enable)
{
    if (enable) {
        AXI_ICM.int_ena.val |= mask;
    } else {
        AXI_ICM.int_ena.val &= ~mask;
    }
}

/**
 * @brief Get AXI ICM interrupt status
 *
 * @param mask Interrupt mask
 * @return Status mask
 */
static inline uint32_t axi_icm_ll_get_intr_status(uint32_t mask)
{
    return AXI_ICM.int_st.val & mask;
}

/**
 * @brief Clear AXI ICM interrupt
 *
 * @param mask Interrupt mask
 */
static inline void axi_icm_ll_clear_intr(uint32_t mask)
{
    AXI_ICM.int_clr.val = mask;
}

/**
 * @brief Get SYS addrhole exception info
 *
 * @param[out] info Pointer to store exception information
 */
static inline void axi_icm_ll_get_sys_excp_info(axi_icm_ll_excp_info_t *info)
{
    info->addr = AXI_ICM.sys_addrhole_addr.reg_icm_sys_addrhole_addr;
    info->id = AXI_ICM.sys_addrhole_info.reg_icm_sys_addrhole_id;
    info->is_wr = AXI_ICM.sys_addrhole_info.reg_icm_sys_addrhole_wr;
    info->is_secure = AXI_ICM.sys_addrhole_info.reg_icm_sys_addrhole_secure;
}

/**
 * @brief Get CPU addrhole exception info
 *
 * @param[out] info Pointer to store exception information
 */
static inline void axi_icm_ll_get_cpu_excp_info(axi_icm_ll_excp_info_t *info)
{
    info->addr = AXI_ICM.cpu_addrhole_addr.reg_icm_cpu_addrhole_addr;
    info->id = AXI_ICM.cpu_addrhole_info.reg_icm_cpu_addrhole_id;
    info->is_wr = AXI_ICM.cpu_addrhole_info.reg_icm_cpu_addrhole_wr;
    info->is_secure = AXI_ICM.cpu_addrhole_info.reg_icm_cpu_addrhole_secure;
}

#ifdef __cplusplus
}
#endif
