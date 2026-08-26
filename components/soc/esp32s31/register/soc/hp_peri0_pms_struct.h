/**
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: read write control register */
/** Type of trace0_ctrl register
 *  trace0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_trace0 : R/W; bitpos: [0]; default: 1;
         *  Configures trace0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_trace0:1;
        /** read_ree0_trace0 : R/W; bitpos: [1]; default: 0;
         *  Configures trace0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_trace0:1;
        /** read_ree1_trace0 : R/W; bitpos: [2]; default: 0;
         *  Configures trace0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_trace0:1;
        /** read_ree2_trace0 : R/W; bitpos: [3]; default: 0;
         *  Configures trace0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_trace0:1;
        /** write_tee_trace0 : R/W; bitpos: [4]; default: 1;
         *  Configures trace0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_trace0:1;
        /** write_ree0_trace0 : R/W; bitpos: [5]; default: 0;
         *  Configures trace0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_trace0:1;
        /** write_ree1_trace0 : R/W; bitpos: [6]; default: 0;
         *  Configures trace0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_trace0:1;
        /** write_ree2_trace0 : R/W; bitpos: [7]; default: 0;
         *  Configures trace0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_trace0:1;
        /** trace0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock trace0 peri_apm configuration
         */
        uint32_t trace0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_trace0_ctrl_reg_t;

/** Type of trace1_ctrl register
 *  trace1 read/write control register
 */
typedef union {
    struct {
        /** read_tee_trace1 : R/W; bitpos: [0]; default: 1;
         *  Configures trace1 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_trace1:1;
        /** read_ree0_trace1 : R/W; bitpos: [1]; default: 0;
         *  Configures trace1 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_trace1:1;
        /** read_ree1_trace1 : R/W; bitpos: [2]; default: 0;
         *  Configures trace1 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_trace1:1;
        /** read_ree2_trace1 : R/W; bitpos: [3]; default: 0;
         *  Configures trace1 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_trace1:1;
        /** write_tee_trace1 : R/W; bitpos: [4]; default: 1;
         *  Configures trace1 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_trace1:1;
        /** write_ree0_trace1 : R/W; bitpos: [5]; default: 0;
         *  Configures trace1 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_trace1:1;
        /** write_ree1_trace1 : R/W; bitpos: [6]; default: 0;
         *  Configures trace1 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_trace1:1;
        /** write_ree2_trace1 : R/W; bitpos: [7]; default: 0;
         *  Configures trace1 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_trace1:1;
        /** trace1_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock trace1 peri_apm configuration
         */
        uint32_t trace1_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_trace1_ctrl_reg_t;

/** Type of cpu_bus_mon_ctrl register
 *  cpu_bus_mon read/write control register
 */
typedef union {
    struct {
        /** read_tee_cpu_bus_mon : R/W; bitpos: [0]; default: 1;
         *  Configures cpu_bus_mon registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_cpu_bus_mon:1;
        /** read_ree0_cpu_bus_mon : R/W; bitpos: [1]; default: 0;
         *  Configures cpu_bus_mon registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_cpu_bus_mon:1;
        /** read_ree1_cpu_bus_mon : R/W; bitpos: [2]; default: 0;
         *  Configures cpu_bus_mon registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_cpu_bus_mon:1;
        /** read_ree2_cpu_bus_mon : R/W; bitpos: [3]; default: 0;
         *  Configures cpu_bus_mon registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_cpu_bus_mon:1;
        /** write_tee_cpu_bus_mon : R/W; bitpos: [4]; default: 1;
         *  Configures cpu_bus_mon registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_cpu_bus_mon:1;
        /** write_ree0_cpu_bus_mon : R/W; bitpos: [5]; default: 0;
         *  Configures cpu_bus_mon registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_cpu_bus_mon:1;
        /** write_ree1_cpu_bus_mon : R/W; bitpos: [6]; default: 0;
         *  Configures cpu_bus_mon registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_cpu_bus_mon:1;
        /** write_ree2_cpu_bus_mon : R/W; bitpos: [7]; default: 0;
         *  Configures cpu_bus_mon registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_cpu_bus_mon:1;
        /** cpu_bus_mon_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock cpu_bus_mon peri_apm configuration
         */
        uint32_t cpu_bus_mon_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_cpu_bus_mon_ctrl_reg_t;

/** Type of psram_mon_ctrl register
 *  psram_mon read/write control register
 */
typedef union {
    struct {
        /** read_tee_psram_mon : R/W; bitpos: [0]; default: 1;
         *  Configures psram_mon registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_psram_mon:1;
        /** read_ree0_psram_mon : R/W; bitpos: [1]; default: 0;
         *  Configures psram_mon registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_psram_mon:1;
        /** read_ree1_psram_mon : R/W; bitpos: [2]; default: 0;
         *  Configures psram_mon registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_psram_mon:1;
        /** read_ree2_psram_mon : R/W; bitpos: [3]; default: 0;
         *  Configures psram_mon registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_psram_mon:1;
        /** write_tee_psram_mon : R/W; bitpos: [4]; default: 1;
         *  Configures psram_mon registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_psram_mon:1;
        /** write_ree0_psram_mon : R/W; bitpos: [5]; default: 0;
         *  Configures psram_mon registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_psram_mon:1;
        /** write_ree1_psram_mon : R/W; bitpos: [6]; default: 0;
         *  Configures psram_mon registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_psram_mon:1;
        /** write_ree2_psram_mon : R/W; bitpos: [7]; default: 0;
         *  Configures psram_mon registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_psram_mon:1;
        /** psram_mon_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock psram_mon peri_apm configuration
         */
        uint32_t psram_mon_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_psram_mon_ctrl_reg_t;

/** Type of tcm_mon_ctrl register
 *  tcm_mon read/write control register
 */
typedef union {
    struct {
        /** read_tee_tcm_mon : R/W; bitpos: [0]; default: 1;
         *  Configures tcm_mon registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_tcm_mon:1;
        /** read_ree0_tcm_mon : R/W; bitpos: [1]; default: 0;
         *  Configures tcm_mon registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_tcm_mon:1;
        /** read_ree1_tcm_mon : R/W; bitpos: [2]; default: 0;
         *  Configures tcm_mon registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_tcm_mon:1;
        /** read_ree2_tcm_mon : R/W; bitpos: [3]; default: 0;
         *  Configures tcm_mon registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_tcm_mon:1;
        /** write_tee_tcm_mon : R/W; bitpos: [4]; default: 1;
         *  Configures tcm_mon registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_tcm_mon:1;
        /** write_ree0_tcm_mon : R/W; bitpos: [5]; default: 0;
         *  Configures tcm_mon registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_tcm_mon:1;
        /** write_ree1_tcm_mon : R/W; bitpos: [6]; default: 0;
         *  Configures tcm_mon registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_tcm_mon:1;
        /** write_ree2_tcm_mon : R/W; bitpos: [7]; default: 0;
         *  Configures tcm_mon registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_tcm_mon:1;
        /** tcm_mon_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock tcm_mon peri_apm configuration
         */
        uint32_t tcm_mon_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_tcm_mon_ctrl_reg_t;

/** Type of cache_ctrl register
 *  cache read/write control register
 */
typedef union {
    struct {
        /** read_tee_cache : R/W; bitpos: [0]; default: 1;
         *  Configures cache registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_cache:1;
        /** read_ree0_cache : R/W; bitpos: [1]; default: 0;
         *  Configures cache registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_cache:1;
        /** read_ree1_cache : R/W; bitpos: [2]; default: 0;
         *  Configures cache registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_cache:1;
        /** read_ree2_cache : R/W; bitpos: [3]; default: 0;
         *  Configures cache registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_cache:1;
        /** write_tee_cache : R/W; bitpos: [4]; default: 1;
         *  Configures cache registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_cache:1;
        /** write_ree0_cache : R/W; bitpos: [5]; default: 0;
         *  Configures cache registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_cache:1;
        /** write_ree1_cache : R/W; bitpos: [6]; default: 0;
         *  Configures cache registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_cache:1;
        /** write_ree2_cache : R/W; bitpos: [7]; default: 0;
         *  Configures cache registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_cache:1;
        /** cache_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock cache peri_apm configuration
         */
        uint32_t cache_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_cache_ctrl_reg_t;

/** Type of hp_usbotg_core_ctrl register
 *  hp_usbotg_core read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_usbotg_core : R/W; bitpos: [0]; default: 1;
         *  Configures hp_usbotg_core registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_usbotg_core:1;
        /** read_ree0_hp_usbotg_core : R/W; bitpos: [1]; default: 0;
         *  Configures hp_usbotg_core registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_usbotg_core:1;
        /** read_ree1_hp_usbotg_core : R/W; bitpos: [2]; default: 0;
         *  Configures hp_usbotg_core registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_usbotg_core:1;
        /** read_ree2_hp_usbotg_core : R/W; bitpos: [3]; default: 0;
         *  Configures hp_usbotg_core registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_usbotg_core:1;
        /** write_tee_hp_usbotg_core : R/W; bitpos: [4]; default: 1;
         *  Configures hp_usbotg_core registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_usbotg_core:1;
        /** write_ree0_hp_usbotg_core : R/W; bitpos: [5]; default: 0;
         *  Configures hp_usbotg_core registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_usbotg_core:1;
        /** write_ree1_hp_usbotg_core : R/W; bitpos: [6]; default: 0;
         *  Configures hp_usbotg_core registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_usbotg_core:1;
        /** write_ree2_hp_usbotg_core : R/W; bitpos: [7]; default: 0;
         *  Configures hp_usbotg_core registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_usbotg_core:1;
        /** hp_usbotg_core_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_usbotg_core peri_apm configuration
         */
        uint32_t hp_usbotg_core_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_usbotg_core_ctrl_reg_t;

/** Type of hp_regdma_ctrl register
 *  hp_regdma read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_regdma : R/W; bitpos: [0]; default: 1;
         *  Configures hp_regdma registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_regdma:1;
        /** read_ree0_hp_regdma : R/W; bitpos: [1]; default: 0;
         *  Configures hp_regdma registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_regdma:1;
        /** read_ree1_hp_regdma : R/W; bitpos: [2]; default: 0;
         *  Configures hp_regdma registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_regdma:1;
        /** read_ree2_hp_regdma : R/W; bitpos: [3]; default: 0;
         *  Configures hp_regdma registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_regdma:1;
        /** write_tee_hp_regdma : R/W; bitpos: [4]; default: 1;
         *  Configures hp_regdma registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_regdma:1;
        /** write_ree0_hp_regdma : R/W; bitpos: [5]; default: 0;
         *  Configures hp_regdma registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_regdma:1;
        /** write_ree1_hp_regdma : R/W; bitpos: [6]; default: 0;
         *  Configures hp_regdma registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_regdma:1;
        /** write_ree2_hp_regdma : R/W; bitpos: [7]; default: 0;
         *  Configures hp_regdma registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_regdma:1;
        /** hp_regdma_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_regdma peri_apm configuration
         */
        uint32_t hp_regdma_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_regdma_ctrl_reg_t;

/** Type of hp_sdmmc_ctrl register
 *  hp_sdmmc read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_sdmmc : R/W; bitpos: [0]; default: 1;
         *  Configures hp_sdmmc registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_sdmmc:1;
        /** read_ree0_hp_sdmmc : R/W; bitpos: [1]; default: 0;
         *  Configures hp_sdmmc registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_sdmmc:1;
        /** read_ree1_hp_sdmmc : R/W; bitpos: [2]; default: 0;
         *  Configures hp_sdmmc registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_sdmmc:1;
        /** read_ree2_hp_sdmmc : R/W; bitpos: [3]; default: 0;
         *  Configures hp_sdmmc registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_sdmmc:1;
        /** write_tee_hp_sdmmc : R/W; bitpos: [4]; default: 1;
         *  Configures hp_sdmmc registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_sdmmc:1;
        /** write_ree0_hp_sdmmc : R/W; bitpos: [5]; default: 0;
         *  Configures hp_sdmmc registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_sdmmc:1;
        /** write_ree1_hp_sdmmc : R/W; bitpos: [6]; default: 0;
         *  Configures hp_sdmmc registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_sdmmc:1;
        /** write_ree2_hp_sdmmc : R/W; bitpos: [7]; default: 0;
         *  Configures hp_sdmmc registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_sdmmc:1;
        /** hp_sdmmc_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_sdmmc peri_apm configuration
         */
        uint32_t hp_sdmmc_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_sdmmc_ctrl_reg_t;

/** Type of hp_ahb_pdma_ctrl register
 *  hp_ahb_pdma read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_ahb_pdma : R/W; bitpos: [0]; default: 1;
         *  Configures hp_ahb_pdma registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_ahb_pdma:1;
        /** read_ree0_hp_ahb_pdma : R/W; bitpos: [1]; default: 0;
         *  Configures hp_ahb_pdma registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_ahb_pdma:1;
        /** read_ree1_hp_ahb_pdma : R/W; bitpos: [2]; default: 0;
         *  Configures hp_ahb_pdma registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_ahb_pdma:1;
        /** read_ree2_hp_ahb_pdma : R/W; bitpos: [3]; default: 0;
         *  Configures hp_ahb_pdma registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_ahb_pdma:1;
        /** write_tee_hp_ahb_pdma : R/W; bitpos: [4]; default: 1;
         *  Configures hp_ahb_pdma registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_ahb_pdma:1;
        /** write_ree0_hp_ahb_pdma : R/W; bitpos: [5]; default: 0;
         *  Configures hp_ahb_pdma registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_ahb_pdma:1;
        /** write_ree1_hp_ahb_pdma : R/W; bitpos: [6]; default: 0;
         *  Configures hp_ahb_pdma registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_ahb_pdma:1;
        /** write_ree2_hp_ahb_pdma : R/W; bitpos: [7]; default: 0;
         *  Configures hp_ahb_pdma registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_ahb_pdma:1;
        /** hp_ahb_pdma_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_ahb_pdma peri_apm configuration
         */
        uint32_t hp_ahb_pdma_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_ahb_pdma_ctrl_reg_t;

/** Type of hp_jpeg_ctrl register
 *  hp_jpeg read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_jpeg : R/W; bitpos: [0]; default: 1;
         *  Configures hp_jpeg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_jpeg:1;
        /** read_ree0_hp_jpeg : R/W; bitpos: [1]; default: 0;
         *  Configures hp_jpeg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_jpeg:1;
        /** read_ree1_hp_jpeg : R/W; bitpos: [2]; default: 0;
         *  Configures hp_jpeg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_jpeg:1;
        /** read_ree2_hp_jpeg : R/W; bitpos: [3]; default: 0;
         *  Configures hp_jpeg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_jpeg:1;
        /** write_tee_hp_jpeg : R/W; bitpos: [4]; default: 1;
         *  Configures hp_jpeg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_jpeg:1;
        /** write_ree0_hp_jpeg : R/W; bitpos: [5]; default: 0;
         *  Configures hp_jpeg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_jpeg:1;
        /** write_ree1_hp_jpeg : R/W; bitpos: [6]; default: 0;
         *  Configures hp_jpeg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_jpeg:1;
        /** write_ree2_hp_jpeg : R/W; bitpos: [7]; default: 0;
         *  Configures hp_jpeg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_jpeg:1;
        /** hp_jpeg_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_jpeg peri_apm configuration
         */
        uint32_t hp_jpeg_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_jpeg_ctrl_reg_t;

/** Type of hp_ppa_ctrl register
 *  hp_ppa read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_ppa : R/W; bitpos: [0]; default: 1;
         *  Configures hp_ppa registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_ppa:1;
        /** read_ree0_hp_ppa : R/W; bitpos: [1]; default: 0;
         *  Configures hp_ppa registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_ppa:1;
        /** read_ree1_hp_ppa : R/W; bitpos: [2]; default: 0;
         *  Configures hp_ppa registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_ppa:1;
        /** read_ree2_hp_ppa : R/W; bitpos: [3]; default: 0;
         *  Configures hp_ppa registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_ppa:1;
        /** write_tee_hp_ppa : R/W; bitpos: [4]; default: 1;
         *  Configures hp_ppa registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_ppa:1;
        /** write_ree0_hp_ppa : R/W; bitpos: [5]; default: 0;
         *  Configures hp_ppa registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_ppa:1;
        /** write_ree1_hp_ppa : R/W; bitpos: [6]; default: 0;
         *  Configures hp_ppa registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_ppa:1;
        /** write_ree2_hp_ppa : R/W; bitpos: [7]; default: 0;
         *  Configures hp_ppa registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_ppa:1;
        /** hp_ppa_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_ppa peri_apm configuration
         */
        uint32_t hp_ppa_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_ppa_ctrl_reg_t;

/** Type of hp_dma2d_ctrl register
 *  hp_dma2d read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_dma2d : R/W; bitpos: [0]; default: 1;
         *  Configures hp_dma2d registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_dma2d:1;
        /** read_ree0_hp_dma2d : R/W; bitpos: [1]; default: 0;
         *  Configures hp_dma2d registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_dma2d:1;
        /** read_ree1_hp_dma2d : R/W; bitpos: [2]; default: 0;
         *  Configures hp_dma2d registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_dma2d:1;
        /** read_ree2_hp_dma2d : R/W; bitpos: [3]; default: 0;
         *  Configures hp_dma2d registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_dma2d:1;
        /** write_tee_hp_dma2d : R/W; bitpos: [4]; default: 1;
         *  Configures hp_dma2d registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_dma2d:1;
        /** write_ree0_hp_dma2d : R/W; bitpos: [5]; default: 0;
         *  Configures hp_dma2d registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_dma2d:1;
        /** write_ree1_hp_dma2d : R/W; bitpos: [6]; default: 0;
         *  Configures hp_dma2d registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_dma2d:1;
        /** write_ree2_hp_dma2d : R/W; bitpos: [7]; default: 0;
         *  Configures hp_dma2d registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_dma2d:1;
        /** hp_dma2d_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_dma2d peri_apm configuration
         */
        uint32_t hp_dma2d_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_dma2d_ctrl_reg_t;

/** Type of hp_axi_pdma_ctrl register
 *  hp_axi_pdma read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_axi_pdma : R/W; bitpos: [0]; default: 1;
         *  Configures hp_axi_pdma registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_axi_pdma:1;
        /** read_ree0_hp_axi_pdma : R/W; bitpos: [1]; default: 0;
         *  Configures hp_axi_pdma registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_axi_pdma:1;
        /** read_ree1_hp_axi_pdma : R/W; bitpos: [2]; default: 0;
         *  Configures hp_axi_pdma registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_axi_pdma:1;
        /** read_ree2_hp_axi_pdma : R/W; bitpos: [3]; default: 0;
         *  Configures hp_axi_pdma registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_axi_pdma:1;
        /** write_tee_hp_axi_pdma : R/W; bitpos: [4]; default: 1;
         *  Configures hp_axi_pdma registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_axi_pdma:1;
        /** write_ree0_hp_axi_pdma : R/W; bitpos: [5]; default: 0;
         *  Configures hp_axi_pdma registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_axi_pdma:1;
        /** write_ree1_hp_axi_pdma : R/W; bitpos: [6]; default: 0;
         *  Configures hp_axi_pdma registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_axi_pdma:1;
        /** write_ree2_hp_axi_pdma : R/W; bitpos: [7]; default: 0;
         *  Configures hp_axi_pdma registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_axi_pdma:1;
        /** hp_axi_pdma_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_axi_pdma peri_apm configuration
         */
        uint32_t hp_axi_pdma_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_axi_pdma_ctrl_reg_t;

/** Type of hp_gmac_ctrl register
 *  hp_gmac read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_gmac : R/W; bitpos: [0]; default: 1;
         *  Configures hp_gmac registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_gmac:1;
        /** read_ree0_hp_gmac : R/W; bitpos: [1]; default: 0;
         *  Configures hp_gmac registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_gmac:1;
        /** read_ree1_hp_gmac : R/W; bitpos: [2]; default: 0;
         *  Configures hp_gmac registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_gmac:1;
        /** read_ree2_hp_gmac : R/W; bitpos: [3]; default: 0;
         *  Configures hp_gmac registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_gmac:1;
        /** write_tee_hp_gmac : R/W; bitpos: [4]; default: 1;
         *  Configures hp_gmac registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_gmac:1;
        /** write_ree0_hp_gmac : R/W; bitpos: [5]; default: 0;
         *  Configures hp_gmac registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_gmac:1;
        /** write_ree1_hp_gmac : R/W; bitpos: [6]; default: 0;
         *  Configures hp_gmac registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_gmac:1;
        /** write_ree2_hp_gmac : R/W; bitpos: [7]; default: 0;
         *  Configures hp_gmac registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_gmac:1;
        /** hp_gmac_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_gmac peri_apm configuration
         */
        uint32_t hp_gmac_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_gmac_ctrl_reg_t;

/** Type of hp_pvt0_ctrl register
 *  hp_pvt0 read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_pvt0 : R/W; bitpos: [0]; default: 1;
         *  Configures hp_pvt0 registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_pvt0:1;
        /** read_ree0_hp_pvt0 : R/W; bitpos: [1]; default: 0;
         *  Configures hp_pvt0 registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_pvt0:1;
        /** read_ree1_hp_pvt0 : R/W; bitpos: [2]; default: 0;
         *  Configures hp_pvt0 registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_pvt0:1;
        /** read_ree2_hp_pvt0 : R/W; bitpos: [3]; default: 0;
         *  Configures hp_pvt0 registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_pvt0:1;
        /** write_tee_hp_pvt0 : R/W; bitpos: [4]; default: 1;
         *  Configures hp_pvt0 registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_pvt0:1;
        /** write_ree0_hp_pvt0 : R/W; bitpos: [5]; default: 0;
         *  Configures hp_pvt0 registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_pvt0:1;
        /** write_ree1_hp_pvt0 : R/W; bitpos: [6]; default: 0;
         *  Configures hp_pvt0 registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_pvt0:1;
        /** write_ree2_hp_pvt0 : R/W; bitpos: [7]; default: 0;
         *  Configures hp_pvt0 registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_pvt0:1;
        /** hp_pvt0_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_pvt0 peri_apm configuration
         */
        uint32_t hp_pvt0_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_pvt0_ctrl_reg_t;

/** Type of hp_rmt_ctrl register
 *  hp_rmt read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_rmt : R/W; bitpos: [0]; default: 1;
         *  Configures hp_rmt registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_rmt:1;
        /** read_ree0_hp_rmt : R/W; bitpos: [1]; default: 0;
         *  Configures hp_rmt registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_rmt:1;
        /** read_ree1_hp_rmt : R/W; bitpos: [2]; default: 0;
         *  Configures hp_rmt registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_rmt:1;
        /** read_ree2_hp_rmt : R/W; bitpos: [3]; default: 0;
         *  Configures hp_rmt registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_rmt:1;
        /** write_tee_hp_rmt : R/W; bitpos: [4]; default: 1;
         *  Configures hp_rmt registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_rmt:1;
        /** write_ree0_hp_rmt : R/W; bitpos: [5]; default: 0;
         *  Configures hp_rmt registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_rmt:1;
        /** write_ree1_hp_rmt : R/W; bitpos: [6]; default: 0;
         *  Configures hp_rmt registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_rmt:1;
        /** write_ree2_hp_rmt : R/W; bitpos: [7]; default: 0;
         *  Configures hp_rmt registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_rmt:1;
        /** hp_rmt_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_rmt peri_apm configuration
         */
        uint32_t hp_rmt_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_rmt_ctrl_reg_t;

/** Type of hp_bitscrambler_ctrl register
 *  hp_bitscrambler read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_bitscrambler : R/W; bitpos: [0]; default: 1;
         *  Configures hp_bitscrambler registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_bitscrambler:1;
        /** read_ree0_hp_bitscrambler : R/W; bitpos: [1]; default: 0;
         *  Configures hp_bitscrambler registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_bitscrambler:1;
        /** read_ree1_hp_bitscrambler : R/W; bitpos: [2]; default: 0;
         *  Configures hp_bitscrambler registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_bitscrambler:1;
        /** read_ree2_hp_bitscrambler : R/W; bitpos: [3]; default: 0;
         *  Configures hp_bitscrambler registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_bitscrambler:1;
        /** write_tee_hp_bitscrambler : R/W; bitpos: [4]; default: 1;
         *  Configures hp_bitscrambler registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_bitscrambler:1;
        /** write_ree0_hp_bitscrambler : R/W; bitpos: [5]; default: 0;
         *  Configures hp_bitscrambler registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_bitscrambler:1;
        /** write_ree1_hp_bitscrambler : R/W; bitpos: [6]; default: 0;
         *  Configures hp_bitscrambler registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_bitscrambler:1;
        /** write_ree2_hp_bitscrambler : R/W; bitpos: [7]; default: 0;
         *  Configures hp_bitscrambler registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_bitscrambler:1;
        /** hp_bitscrambler_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_bitscrambler peri_apm configuration
         */
        uint32_t hp_bitscrambler_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_bitscrambler_ctrl_reg_t;

/** Type of hp_asrc_ctrl register
 *  hp_asrc read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_asrc : R/W; bitpos: [0]; default: 1;
         *  Configures hp_asrc registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_asrc:1;
        /** read_ree0_hp_asrc : R/W; bitpos: [1]; default: 0;
         *  Configures hp_asrc registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_asrc:1;
        /** read_ree1_hp_asrc : R/W; bitpos: [2]; default: 0;
         *  Configures hp_asrc registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_asrc:1;
        /** read_ree2_hp_asrc : R/W; bitpos: [3]; default: 0;
         *  Configures hp_asrc registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_asrc:1;
        /** write_tee_hp_asrc : R/W; bitpos: [4]; default: 1;
         *  Configures hp_asrc registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_asrc:1;
        /** write_ree0_hp_asrc : R/W; bitpos: [5]; default: 0;
         *  Configures hp_asrc registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_asrc:1;
        /** write_ree1_hp_asrc : R/W; bitpos: [6]; default: 0;
         *  Configures hp_asrc registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_asrc:1;
        /** write_ree2_hp_asrc : R/W; bitpos: [7]; default: 0;
         *  Configures hp_asrc registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_asrc:1;
        /** hp_asrc_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_asrc peri_apm configuration
         */
        uint32_t hp_asrc_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_asrc_ctrl_reg_t;

/** Type of cnnt_sys_reg_ctrl register
 *  cnnt_sys_reg read/write control register
 */
typedef union {
    struct {
        /** read_tee_cnnt_sys_reg : R/W; bitpos: [0]; default: 1;
         *  Configures cnnt_sys_reg registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_cnnt_sys_reg:1;
        /** read_ree0_cnnt_sys_reg : R/W; bitpos: [1]; default: 0;
         *  Configures cnnt_sys_reg registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_cnnt_sys_reg:1;
        /** read_ree1_cnnt_sys_reg : R/W; bitpos: [2]; default: 0;
         *  Configures cnnt_sys_reg registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_cnnt_sys_reg:1;
        /** read_ree2_cnnt_sys_reg : R/W; bitpos: [3]; default: 0;
         *  Configures cnnt_sys_reg registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_cnnt_sys_reg:1;
        /** write_tee_cnnt_sys_reg : R/W; bitpos: [4]; default: 1;
         *  Configures cnnt_sys_reg registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_cnnt_sys_reg:1;
        /** write_ree0_cnnt_sys_reg : R/W; bitpos: [5]; default: 0;
         *  Configures cnnt_sys_reg registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_cnnt_sys_reg:1;
        /** write_ree1_cnnt_sys_reg : R/W; bitpos: [6]; default: 0;
         *  Configures cnnt_sys_reg registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_cnnt_sys_reg:1;
        /** write_ree2_cnnt_sys_reg : R/W; bitpos: [7]; default: 0;
         *  Configures cnnt_sys_reg registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_cnnt_sys_reg:1;
        /** cnnt_sys_reg_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock cnnt_sys_reg peri_apm configuration
         */
        uint32_t cnnt_sys_reg_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_cnnt_sys_reg_ctrl_reg_t;

/** Type of hp_flash_ctrl register
 *  hp_flash read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_flash : R/W; bitpos: [0]; default: 1;
         *  Configures hp_flash registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_flash:1;
        /** read_ree0_hp_flash : R/W; bitpos: [1]; default: 0;
         *  Configures hp_flash registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_flash:1;
        /** read_ree1_hp_flash : R/W; bitpos: [2]; default: 0;
         *  Configures hp_flash registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_flash:1;
        /** read_ree2_hp_flash : R/W; bitpos: [3]; default: 0;
         *  Configures hp_flash registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_flash:1;
        /** write_tee_hp_flash : R/W; bitpos: [4]; default: 1;
         *  Configures hp_flash registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_flash:1;
        /** write_ree0_hp_flash : R/W; bitpos: [5]; default: 0;
         *  Configures hp_flash registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_flash:1;
        /** write_ree1_hp_flash : R/W; bitpos: [6]; default: 0;
         *  Configures hp_flash registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_flash:1;
        /** write_ree2_hp_flash : R/W; bitpos: [7]; default: 0;
         *  Configures hp_flash registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_flash:1;
        /** hp_flash_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_flash peri_apm configuration
         */
        uint32_t hp_flash_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_flash_ctrl_reg_t;

/** Type of hp_psram_ctrl register
 *  hp_psram read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_psram : R/W; bitpos: [0]; default: 1;
         *  Configures hp_psram registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_psram:1;
        /** read_ree0_hp_psram : R/W; bitpos: [1]; default: 0;
         *  Configures hp_psram registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_psram:1;
        /** read_ree1_hp_psram : R/W; bitpos: [2]; default: 0;
         *  Configures hp_psram registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_psram:1;
        /** read_ree2_hp_psram : R/W; bitpos: [3]; default: 0;
         *  Configures hp_psram registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_psram:1;
        /** write_tee_hp_psram : R/W; bitpos: [4]; default: 1;
         *  Configures hp_psram registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_psram:1;
        /** write_ree0_hp_psram : R/W; bitpos: [5]; default: 0;
         *  Configures hp_psram registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_psram:1;
        /** write_ree1_hp_psram : R/W; bitpos: [6]; default: 0;
         *  Configures hp_psram registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_psram:1;
        /** write_ree2_hp_psram : R/W; bitpos: [7]; default: 0;
         *  Configures hp_psram registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_psram:1;
        /** hp_psram_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_psram peri_apm configuration
         */
        uint32_t hp_psram_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_psram_ctrl_reg_t;

/** Type of tee_ctrl register
 *  tee read/write control register
 */
typedef union {
    struct {
        /** read_tee_tee : R/W; bitpos: [0]; default: 1;
         *  Configures tee registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_tee:1;
        /** read_ree0_tee : HRO; bitpos: [1]; default: 0;
         *  Configures tee registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_tee:1;
        /** read_ree1_tee : HRO; bitpos: [2]; default: 0;
         *  Configures tee registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_tee:1;
        /** read_ree2_tee : HRO; bitpos: [3]; default: 0;
         *  Configures tee registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_tee:1;
        /** write_tee_tee : R/W; bitpos: [4]; default: 1;
         *  Configures tee registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_tee:1;
        /** write_ree0_tee : HRO; bitpos: [5]; default: 0;
         *  Configures tee registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_tee:1;
        /** write_ree1_tee : HRO; bitpos: [6]; default: 0;
         *  Configures tee registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_tee:1;
        /** write_ree2_tee : HRO; bitpos: [7]; default: 0;
         *  Configures tee registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_tee:1;
        /** tee_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock tee peri_apm configuration
         */
        uint32_t tee_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_tee_ctrl_reg_t;

/** Type of hp_apm_ctrl register
 *  hp_apm read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_apm : R/W; bitpos: [0]; default: 1;
         *  Configures hp_apm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_apm:1;
        /** read_ree0_hp_apm : HRO; bitpos: [1]; default: 0;
         *  Configures hp_apm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_apm:1;
        /** read_ree1_hp_apm : HRO; bitpos: [2]; default: 0;
         *  Configures hp_apm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_apm:1;
        /** read_ree2_hp_apm : HRO; bitpos: [3]; default: 0;
         *  Configures hp_apm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_apm:1;
        /** write_tee_hp_apm : R/W; bitpos: [4]; default: 1;
         *  Configures hp_apm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_apm:1;
        /** write_ree0_hp_apm : HRO; bitpos: [5]; default: 0;
         *  Configures hp_apm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_apm:1;
        /** write_ree1_hp_apm : HRO; bitpos: [6]; default: 0;
         *  Configures hp_apm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_apm:1;
        /** write_ree2_hp_apm : HRO; bitpos: [7]; default: 0;
         *  Configures hp_apm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_apm:1;
        /** hp_apm_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_apm peri_apm configuration
         */
        uint32_t hp_apm_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_apm_ctrl_reg_t;

/** Type of hp_mem_apm_ctrl register
 *  hp_mem_apm read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_mem_apm : R/W; bitpos: [0]; default: 1;
         *  Configures hp_mem_apm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_mem_apm:1;
        /** read_ree0_hp_mem_apm : HRO; bitpos: [1]; default: 0;
         *  Configures hp_mem_apm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_mem_apm:1;
        /** read_ree1_hp_mem_apm : HRO; bitpos: [2]; default: 0;
         *  Configures hp_mem_apm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_mem_apm:1;
        /** read_ree2_hp_mem_apm : HRO; bitpos: [3]; default: 0;
         *  Configures hp_mem_apm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_mem_apm:1;
        /** write_tee_hp_mem_apm : R/W; bitpos: [4]; default: 1;
         *  Configures hp_mem_apm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_mem_apm:1;
        /** write_ree0_hp_mem_apm : HRO; bitpos: [5]; default: 0;
         *  Configures hp_mem_apm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_mem_apm:1;
        /** write_ree1_hp_mem_apm : HRO; bitpos: [6]; default: 0;
         *  Configures hp_mem_apm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_mem_apm:1;
        /** write_ree2_hp_mem_apm : HRO; bitpos: [7]; default: 0;
         *  Configures hp_mem_apm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_mem_apm:1;
        /** hp_mem_apm_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_mem_apm peri_apm configuration
         */
        uint32_t hp_mem_apm_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_mem_apm_ctrl_reg_t;

/** Type of cpu_apm_ctrl register
 *  cpu_apm read/write control register
 */
typedef union {
    struct {
        /** read_tee_cpu_apm : R/W; bitpos: [0]; default: 1;
         *  Configures cpu_apm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_cpu_apm:1;
        /** read_ree0_cpu_apm : HRO; bitpos: [1]; default: 0;
         *  Configures cpu_apm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_cpu_apm:1;
        /** read_ree1_cpu_apm : HRO; bitpos: [2]; default: 0;
         *  Configures cpu_apm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_cpu_apm:1;
        /** read_ree2_cpu_apm : HRO; bitpos: [3]; default: 0;
         *  Configures cpu_apm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_cpu_apm:1;
        /** write_tee_cpu_apm : R/W; bitpos: [4]; default: 1;
         *  Configures cpu_apm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_cpu_apm:1;
        /** write_ree0_cpu_apm : HRO; bitpos: [5]; default: 0;
         *  Configures cpu_apm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_cpu_apm:1;
        /** write_ree1_cpu_apm : HRO; bitpos: [6]; default: 0;
         *  Configures cpu_apm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_cpu_apm:1;
        /** write_ree2_cpu_apm : HRO; bitpos: [7]; default: 0;
         *  Configures cpu_apm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_cpu_apm:1;
        /** cpu_apm_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock cpu_apm peri_apm configuration
         */
        uint32_t cpu_apm_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_cpu_apm_ctrl_reg_t;

/** Type of hp_peri0_pms_ctrl register
 *  hp_peri0_pms read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_peri0_pms : R/W; bitpos: [0]; default: 1;
         *  Configures hp_peri0_pms registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_peri0_pms:1;
        /** read_ree0_hp_peri0_pms : HRO; bitpos: [1]; default: 0;
         *  Configures hp_peri0_pms registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_peri0_pms:1;
        /** read_ree1_hp_peri0_pms : HRO; bitpos: [2]; default: 0;
         *  Configures hp_peri0_pms registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_peri0_pms:1;
        /** read_ree2_hp_peri0_pms : HRO; bitpos: [3]; default: 0;
         *  Configures hp_peri0_pms registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_peri0_pms:1;
        /** write_tee_hp_peri0_pms : R/W; bitpos: [4]; default: 1;
         *  Configures hp_peri0_pms registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_peri0_pms:1;
        /** write_ree0_hp_peri0_pms : HRO; bitpos: [5]; default: 0;
         *  Configures hp_peri0_pms registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_peri0_pms:1;
        /** write_ree1_hp_peri0_pms : HRO; bitpos: [6]; default: 0;
         *  Configures hp_peri0_pms registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_peri0_pms:1;
        /** write_ree2_hp_peri0_pms : HRO; bitpos: [7]; default: 0;
         *  Configures hp_peri0_pms registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_peri0_pms:1;
        /** hp_peri0_pms_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_peri0_pms peri_apm configuration
         */
        uint32_t hp_peri0_pms_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_peri0_pms_ctrl_reg_t;

/** Type of hp_key_manager_ctrl register
 *  hp_key_manager read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_key_manager : R/W; bitpos: [0]; default: 1;
         *  Configures hp_key_manager registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_key_manager:1;
        /** read_ree0_hp_key_manager : R/W; bitpos: [1]; default: 0;
         *  Configures hp_key_manager registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_key_manager:1;
        /** read_ree1_hp_key_manager : R/W; bitpos: [2]; default: 0;
         *  Configures hp_key_manager registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_key_manager:1;
        /** read_ree2_hp_key_manager : R/W; bitpos: [3]; default: 0;
         *  Configures hp_key_manager registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_key_manager:1;
        /** write_tee_hp_key_manager : R/W; bitpos: [4]; default: 1;
         *  Configures hp_key_manager registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_key_manager:1;
        /** write_ree0_hp_key_manager : R/W; bitpos: [5]; default: 0;
         *  Configures hp_key_manager registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_key_manager:1;
        /** write_ree1_hp_key_manager : R/W; bitpos: [6]; default: 0;
         *  Configures hp_key_manager registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_key_manager:1;
        /** write_ree2_hp_key_manager : R/W; bitpos: [7]; default: 0;
         *  Configures hp_key_manager registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_key_manager:1;
        /** hp_key_manager_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_key_manager peri_apm configuration
         */
        uint32_t hp_key_manager_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_key_manager_ctrl_reg_t;

/** Type of hp_crypto_ctrl register
 *  hp_crypto read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_crypto : R/W; bitpos: [0]; default: 1;
         *  Configures hp_crypto registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_crypto:1;
        /** read_ree0_hp_crypto : R/W; bitpos: [1]; default: 0;
         *  Configures hp_crypto registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_crypto:1;
        /** read_ree1_hp_crypto : R/W; bitpos: [2]; default: 0;
         *  Configures hp_crypto registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_crypto:1;
        /** read_ree2_hp_crypto : R/W; bitpos: [3]; default: 0;
         *  Configures hp_crypto registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_crypto:1;
        /** write_tee_hp_crypto : R/W; bitpos: [4]; default: 1;
         *  Configures hp_crypto registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_crypto:1;
        /** write_ree0_hp_crypto : R/W; bitpos: [5]; default: 0;
         *  Configures hp_crypto registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_crypto:1;
        /** write_ree1_hp_crypto : R/W; bitpos: [6]; default: 0;
         *  Configures hp_crypto registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_crypto:1;
        /** write_ree2_hp_crypto : R/W; bitpos: [7]; default: 0;
         *  Configures hp_crypto registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_crypto:1;
        /** hp_crypto_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_crypto peri_apm configuration
         */
        uint32_t hp_crypto_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_crypto_ctrl_reg_t;

/** Type of hp_axi_icm_ctrl register
 *  hp_axi_icm read/write control register
 */
typedef union {
    struct {
        /** read_tee_hp_axi_icm : R/W; bitpos: [0]; default: 1;
         *  Configures hp_axi_icm registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_hp_axi_icm:1;
        /** read_ree0_hp_axi_icm : R/W; bitpos: [1]; default: 0;
         *  Configures hp_axi_icm registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_hp_axi_icm:1;
        /** read_ree1_hp_axi_icm : R/W; bitpos: [2]; default: 0;
         *  Configures hp_axi_icm registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_hp_axi_icm:1;
        /** read_ree2_hp_axi_icm : R/W; bitpos: [3]; default: 0;
         *  Configures hp_axi_icm registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_hp_axi_icm:1;
        /** write_tee_hp_axi_icm : R/W; bitpos: [4]; default: 1;
         *  Configures hp_axi_icm registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_hp_axi_icm:1;
        /** write_ree0_hp_axi_icm : R/W; bitpos: [5]; default: 0;
         *  Configures hp_axi_icm registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_hp_axi_icm:1;
        /** write_ree1_hp_axi_icm : R/W; bitpos: [6]; default: 0;
         *  Configures hp_axi_icm registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_hp_axi_icm:1;
        /** write_ree2_hp_axi_icm : R/W; bitpos: [7]; default: 0;
         *  Configures hp_axi_icm registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_hp_axi_icm:1;
        /** hp_axi_icm_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock hp_axi_icm peri_apm configuration
         */
        uint32_t hp_axi_icm_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_hp_axi_icm_ctrl_reg_t;

/** Type of axi_perf_mon_ctrl register
 *  axi_perf_mon read/write control register
 */
typedef union {
    struct {
        /** read_tee_axi_perf_mon : R/W; bitpos: [0]; default: 1;
         *  Configures axi_perf_mon registers read permission in tee mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_tee_axi_perf_mon:1;
        /** read_ree0_axi_perf_mon : R/W; bitpos: [1]; default: 0;
         *  Configures axi_perf_mon registers read permission in ree0 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree0_axi_perf_mon:1;
        /** read_ree1_axi_perf_mon : R/W; bitpos: [2]; default: 0;
         *  Configures axi_perf_mon registers read permission in ree1 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree1_axi_perf_mon:1;
        /** read_ree2_axi_perf_mon : R/W; bitpos: [3]; default: 0;
         *  Configures axi_perf_mon registers read permission in ree2 mode.
         *  0: can not be read
         *  1: can be read
         */
        uint32_t read_ree2_axi_perf_mon:1;
        /** write_tee_axi_perf_mon : R/W; bitpos: [4]; default: 1;
         *  Configures axi_perf_mon registers write permission in tee mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_tee_axi_perf_mon:1;
        /** write_ree0_axi_perf_mon : R/W; bitpos: [5]; default: 0;
         *  Configures axi_perf_mon registers write permission in ree0 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree0_axi_perf_mon:1;
        /** write_ree1_axi_perf_mon : R/W; bitpos: [6]; default: 0;
         *  Configures axi_perf_mon registers write permission in ree1 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree1_axi_perf_mon:1;
        /** write_ree2_axi_perf_mon : R/W; bitpos: [7]; default: 0;
         *  Configures axi_perf_mon registers write permission in ree2 mode.
         *  0: can not be write
         *  1: can be write
         */
        uint32_t write_ree2_axi_perf_mon:1;
        /** axi_perf_mon_lock : R/W; bitpos: [8]; default: 0;
         *  Set 1 to lock axi_perf_mon peri_apm configuration
         */
        uint32_t axi_perf_mon_lock:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_peri0_pms_axi_perf_mon_ctrl_reg_t;


/** Group: PMS Register */
/** Type of hp_cpuperi0_0 register
 *  HP_CPUPERI0 PMS configuration & info register
 */
typedef union {
    struct {
        /** hp_cpuperi0_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear hp_cpuperi0 peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t hp_cpuperi0_pms_exception_clr:1;
        /** hp_cpuperi0_pms_exception_det : RO; bitpos: [1]; default: 0;
         *  Represents whether the hp_cpuperi0 pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t hp_cpuperi0_pms_exception_det:1;
        /** hp_cpuperi0_pms_exception_id : RO; bitpos: [7:2]; default: 0;
         *  Represents the master id when hp_cpuperi0 pms has been triggered.
         */
        uint32_t hp_cpuperi0_pms_exception_id:6;
        /** hp_cpuperi0_pms_exception_mode : RO; bitpos: [9:8]; default: 0;
         *  Represents the security mode when hp_cpuperi0 pms has been triggered.
         */
        uint32_t hp_cpuperi0_pms_exception_mode:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} hp_peri0_pms_hp_cpuperi0_0_reg_t;

/** Type of hp_cpuperi0_1 register
 *  HP_CPUPERI0 PMS exception addr record register
 */
typedef union {
    struct {
        /** hp_cpuperi0_pms_exception_addr : RO; bitpos: [27:0]; default: 0;
         *  Represents the access address (bit27~bit0) when hp_cpuperi0 pms has been triggered.
         */
        uint32_t hp_cpuperi0_pms_exception_addr:28;
        /** hp_cpuperi0_pms_exception_addr_const : HRO; bitpos: [31:28]; default: 2;
         *  Represents the access address (bit31~bit28) when hp_cpuperi0 pms has been triggered.
         */
        uint32_t hp_cpuperi0_pms_exception_addr_const:4;
    };
    uint32_t val;
} hp_peri0_pms_hp_cpuperi0_1_reg_t;

/** Type of hp_cpuperi1_0 register
 *  HP_CPUPERI1 PMS configuration & info register
 */
typedef union {
    struct {
        /** hp_cpuperi1_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear hp_cpuperi1 peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t hp_cpuperi1_pms_exception_clr:1;
        /** hp_cpuperi1_pms_exception_det : RO; bitpos: [1]; default: 0;
         *  Represents whether the hp_cpuperi1 pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t hp_cpuperi1_pms_exception_det:1;
        /** hp_cpuperi1_pms_exception_id : RO; bitpos: [7:2]; default: 0;
         *  Represents the master id when hp_cpuperi1 pms has been triggered.
         */
        uint32_t hp_cpuperi1_pms_exception_id:6;
        /** hp_cpuperi1_pms_exception_mode : RO; bitpos: [9:8]; default: 0;
         *  Represents the security mode when hp_cpuperi1 pms has been triggered.
         */
        uint32_t hp_cpuperi1_pms_exception_mode:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} hp_peri0_pms_hp_cpuperi1_0_reg_t;

/** Type of hp_cpuperi1_1 register
 *  HP_CPUPERI1 PMS exception addr record register
 */
typedef union {
    struct {
        /** hp_cpuperi1_pms_exception_addr : RO; bitpos: [27:0]; default: 0;
         *  Represents the access address (bit27~bit0) when hp_cpuperi1 pms has been triggered.
         */
        uint32_t hp_cpuperi1_pms_exception_addr:28;
        /** hp_cpuperi1_pms_exception_addr_const : HRO; bitpos: [31:28]; default: 2;
         *  Represents the access address (bit31~bit28) when hp_cpuperi1 pms has been triggered.
         */
        uint32_t hp_cpuperi1_pms_exception_addr_const:4;
    };
    uint32_t val;
} hp_peri0_pms_hp_cpuperi1_1_reg_t;

/** Type of hp_peri0_0 register
 *  HP_PERI0 PMS configuration & info register
 */
typedef union {
    struct {
        /** hp_peri0_pms_exception_clr : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to clear hp_peri0 peri_pms_record_reg.
         *  0: No clear
         *  1: Clear peri_pms_record_reg
         */
        uint32_t hp_peri0_pms_exception_clr:1;
        /** hp_peri0_pms_exception_det : RO; bitpos: [1]; default: 0;
         *  Represents whether the hp_peri0 pms has been triggered.
         *  0: No triggered
         *  1: Has been triggered
         */
        uint32_t hp_peri0_pms_exception_det:1;
        /** hp_peri0_pms_exception_id : RO; bitpos: [7:2]; default: 0;
         *  Represents the master id when hp_peri0 pms has been triggered.
         */
        uint32_t hp_peri0_pms_exception_id:6;
        /** hp_peri0_pms_exception_mode : RO; bitpos: [9:8]; default: 0;
         *  Represents the security mode when hp_peri0 pms has been triggered.
         */
        uint32_t hp_peri0_pms_exception_mode:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} hp_peri0_pms_hp_peri0_0_reg_t;

/** Type of hp_peri0_1 register
 *  HP_PERI0 PMS exception addr record register
 */
typedef union {
    struct {
        /** hp_peri0_pms_exception_addr : RO; bitpos: [27:0]; default: 0;
         *  Represents the access address (bit27~bit0) when hp_peri0 pms has been triggered.
         */
        uint32_t hp_peri0_pms_exception_addr:28;
        /** hp_peri0_pms_exception_addr_const : HRO; bitpos: [31:28]; default: 2;
         *  Represents the access address (bit31~bit28) when hp_peri0 pms has been triggered.
         */
        uint32_t hp_peri0_pms_exception_addr_const:4;
    };
    uint32_t val;
} hp_peri0_pms_hp_peri0_1_reg_t;

/** Type of int_en register
 *  APM interrupt enable register
 */
typedef union {
    struct {
        /** hp_peri0_pms_int_en : R/W; bitpos: [0]; default: 0;
         *  Configures to enable hp_peri0 pms interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t hp_peri0_pms_int_en:1;
        /** hp_cpuperi1_pms_int_en : R/W; bitpos: [1]; default: 0;
         *  Configures to enable hp_cpuperi1 pms interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t hp_cpuperi1_pms_int_en:1;
        /** hp_cpuperi0_pms_int_en : R/W; bitpos: [2]; default: 0;
         *  Configures to enable hp_cpuperi0 pms interrupt.
         *  0: disable
         *  1: enable
         */
        uint32_t hp_cpuperi0_pms_int_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_peri0_pms_int_en_reg_t;


/** Group: config register */
/** Type of bus_err_conf register
 *  Clock gating register
 */
typedef union {
    struct {
        /** bus_err_resp_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether return error response to cpu when access blocked
         *  0: disable error response
         *  1: enable error response
         */
        uint32_t bus_err_resp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_peri0_pms_bus_err_conf_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  Clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether to keep the clock always on.
         *  0: enable automatic clock gating
         *  1: keep the clock always on
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_peri0_pms_clock_gate_reg_t;


/** Group: Version control register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38834736;
         *  Version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_peri0_pms_date_reg_t;


typedef struct {
    volatile hp_peri0_pms_trace0_ctrl_reg_t trace0_ctrl;
    volatile hp_peri0_pms_trace1_ctrl_reg_t trace1_ctrl;
    volatile hp_peri0_pms_cpu_bus_mon_ctrl_reg_t cpu_bus_mon_ctrl;
    volatile hp_peri0_pms_psram_mon_ctrl_reg_t psram_mon_ctrl;
    volatile hp_peri0_pms_tcm_mon_ctrl_reg_t tcm_mon_ctrl;
    volatile hp_peri0_pms_cache_ctrl_reg_t cache_ctrl;
    volatile hp_peri0_pms_hp_usbotg_core_ctrl_reg_t hp_usbotg_core_ctrl;
    volatile hp_peri0_pms_hp_regdma_ctrl_reg_t hp_regdma_ctrl;
    volatile hp_peri0_pms_hp_sdmmc_ctrl_reg_t hp_sdmmc_ctrl;
    volatile hp_peri0_pms_hp_ahb_pdma_ctrl_reg_t hp_ahb_pdma_ctrl;
    volatile hp_peri0_pms_hp_jpeg_ctrl_reg_t hp_jpeg_ctrl;
    volatile hp_peri0_pms_hp_ppa_ctrl_reg_t hp_ppa_ctrl;
    volatile hp_peri0_pms_hp_dma2d_ctrl_reg_t hp_dma2d_ctrl;
    volatile hp_peri0_pms_hp_axi_pdma_ctrl_reg_t hp_axi_pdma_ctrl;
    volatile hp_peri0_pms_hp_gmac_ctrl_reg_t hp_gmac_ctrl;
    volatile hp_peri0_pms_hp_pvt0_ctrl_reg_t hp_pvt0_ctrl;
    volatile hp_peri0_pms_hp_rmt_ctrl_reg_t hp_rmt_ctrl;
    volatile hp_peri0_pms_hp_bitscrambler_ctrl_reg_t hp_bitscrambler_ctrl;
    volatile hp_peri0_pms_hp_asrc_ctrl_reg_t hp_asrc_ctrl;
    volatile hp_peri0_pms_cnnt_sys_reg_ctrl_reg_t cnnt_sys_reg_ctrl;
    volatile hp_peri0_pms_hp_flash_ctrl_reg_t hp_flash_ctrl;
    volatile hp_peri0_pms_hp_psram_ctrl_reg_t hp_psram_ctrl;
    volatile hp_peri0_pms_tee_ctrl_reg_t tee_ctrl;
    volatile hp_peri0_pms_hp_apm_ctrl_reg_t hp_apm_ctrl;
    volatile hp_peri0_pms_hp_mem_apm_ctrl_reg_t hp_mem_apm_ctrl;
    volatile hp_peri0_pms_cpu_apm_ctrl_reg_t cpu_apm_ctrl;
    volatile hp_peri0_pms_hp_peri0_pms_ctrl_reg_t hp_peri0_pms_ctrl;
    volatile hp_peri0_pms_hp_key_manager_ctrl_reg_t hp_key_manager_ctrl;
    volatile hp_peri0_pms_hp_crypto_ctrl_reg_t hp_crypto_ctrl;
    volatile hp_peri0_pms_hp_axi_icm_ctrl_reg_t hp_axi_icm_ctrl;
    volatile hp_peri0_pms_axi_perf_mon_ctrl_reg_t axi_perf_mon_ctrl;
    uint32_t reserved_07c[97];
    volatile hp_peri0_pms_hp_cpuperi0_0_reg_t hp_cpuperi0_0;
    volatile hp_peri0_pms_hp_cpuperi0_1_reg_t hp_cpuperi0_1;
    volatile hp_peri0_pms_hp_cpuperi1_0_reg_t hp_cpuperi1_0;
    volatile hp_peri0_pms_hp_cpuperi1_1_reg_t hp_cpuperi1_1;
    volatile hp_peri0_pms_hp_peri0_0_reg_t hp_peri0_0;
    volatile hp_peri0_pms_hp_peri0_1_reg_t hp_peri0_1;
    uint32_t reserved_218[58];
    volatile hp_peri0_pms_int_en_reg_t int_en;
    uint32_t reserved_304[827];
    volatile hp_peri0_pms_bus_err_conf_reg_t bus_err_conf;
    uint32_t reserved_ff4;
    volatile hp_peri0_pms_clock_gate_reg_t clock_gate;
    volatile hp_peri0_pms_date_reg_t date;
} hp_peri0_pms_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(hp_peri0_pms_dev_t) == 0x1000, "Invalid size of hp_peri0_pms_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
