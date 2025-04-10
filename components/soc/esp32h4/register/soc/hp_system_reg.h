/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** HP_SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG register
 *  External device encryption/decryption configuration register
 */
#define HP_SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG (DR_REG_HP_SYSTEM_BASE + 0x0)
/** HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable MSPI XTS manual encryption in SPI boot mode.
 *  0: Disable
 *  1: Enable
 */
#define HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT    (BIT(0))
#define HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT_M  (HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT_V << HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT_S)
#define HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT_V  0x00000001U
#define HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT_S  0
/** HP_SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define HP_SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT    (BIT(1))
#define HP_SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT_M  (HP_SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT_V << HP_SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT_S)
#define HP_SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT_V  0x00000001U
#define HP_SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT_S  1
/** HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable MSPI XTS auto decryption in download boot mode.
 *  0: Disable
 *  1: Enable
 */
#define HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT    (BIT(2))
#define HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_M  (HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_V << HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_S)
#define HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_V  0x00000001U
#define HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_S  2
/** HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable MSPI XTS manual encryption in download boot
 *  mode.
 *  0: Disable
 *  1: Enable
 */
#define HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT    (BIT(3))
#define HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_M  (HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_V << HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_S)
#define HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_V  0x00000001U
#define HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_S  3

/** HP_SYSTEM_CPU_PERI_TIMEOUT_CONF_REG register
 *  CPU_PERI_TIMEOUT configuration register
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0xc)
/** HP_SYSTEM_CPU_PERI_TIMEOUT_THRES : R/W; bitpos: [15:0]; default: 65535;
 *  Configures the timeout threshold for bus access for accessing CPU peripheral
 *  register in the number of clock cycles of the clock domain.
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_THRES    0x0000FFFFU
#define HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_M  (HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_V << HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_S)
#define HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_V  0x0000FFFFU
#define HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_S  0
/** HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR : WT; bitpos: [16]; default: 0;
 *  Write 1 to clear timeout interrupt.
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR    (BIT(16))
#define HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_M  (HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_V << HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_S)
#define HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_V  0x00000001U
#define HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_S  16
/** HP_SYSTEM_CPU_PERI_TIMEOUT_PROTECT_EN : R/W; bitpos: [17]; default: 1;
 *  Configures whether or not to enable timeout protection for accessing CPU peripheral
 *  registers.
 *  0: Disable
 *  1: Enable
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_PROTECT_EN    (BIT(17))
#define HP_SYSTEM_CPU_PERI_TIMEOUT_PROTECT_EN_M  (HP_SYSTEM_CPU_PERI_TIMEOUT_PROTECT_EN_V << HP_SYSTEM_CPU_PERI_TIMEOUT_PROTECT_EN_S)
#define HP_SYSTEM_CPU_PERI_TIMEOUT_PROTECT_EN_V  0x00000001U
#define HP_SYSTEM_CPU_PERI_TIMEOUT_PROTECT_EN_S  17

/** HP_SYSTEM_CPU_PERI_TIMEOUT_ADDR_REG register
 *  CPU_PERI_TIMEOUT_ADDR register
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_ADDR_REG (DR_REG_HP_SYSTEM_BASE + 0x10)
/** HP_SYSTEM_CPU_PERI_TIMEOUT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the address information of abnormal access.
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_ADDR    0xFFFFFFFFU
#define HP_SYSTEM_CPU_PERI_TIMEOUT_ADDR_M  (HP_SYSTEM_CPU_PERI_TIMEOUT_ADDR_V << HP_SYSTEM_CPU_PERI_TIMEOUT_ADDR_S)
#define HP_SYSTEM_CPU_PERI_TIMEOUT_ADDR_V  0xFFFFFFFFU
#define HP_SYSTEM_CPU_PERI_TIMEOUT_ADDR_S  0

/** HP_SYSTEM_CPU_PERI_TIMEOUT_UID_REG register
 *  CPU_PERI_TIMEOUT_UID register
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_UID_REG (DR_REG_HP_SYSTEM_BASE + 0x14)
/** HP_SYSTEM_CPU_PERI_TIMEOUT_UID : RO; bitpos: [6:0]; default: 0;
 *  Represents the master id[4:0] and master permission[6:5] when trigger timeout. This
 *  register will be cleared after the interrupt is cleared.
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_UID    0x0000007FU
#define HP_SYSTEM_CPU_PERI_TIMEOUT_UID_M  (HP_SYSTEM_CPU_PERI_TIMEOUT_UID_V << HP_SYSTEM_CPU_PERI_TIMEOUT_UID_S)
#define HP_SYSTEM_CPU_PERI_TIMEOUT_UID_V  0x0000007FU
#define HP_SYSTEM_CPU_PERI_TIMEOUT_UID_S  0

/** HP_SYSTEM_HP_PERI_TIMEOUT_CONF_REG register
 *  HP_PERI_TIMEOUT configuration register
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x18)
/** HP_SYSTEM_HP_PERI_TIMEOUT_THRES : R/W; bitpos: [15:0]; default: 65535;
 *  Configures the timeout threshold for bus access for accessing HP peripheral
 *  register, corresponding to the number of clock cycles of the clock domain.
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_THRES    0x0000FFFFU
#define HP_SYSTEM_HP_PERI_TIMEOUT_THRES_M  (HP_SYSTEM_HP_PERI_TIMEOUT_THRES_V << HP_SYSTEM_HP_PERI_TIMEOUT_THRES_S)
#define HP_SYSTEM_HP_PERI_TIMEOUT_THRES_V  0x0000FFFFU
#define HP_SYSTEM_HP_PERI_TIMEOUT_THRES_S  0
/** HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to clear timeout interrupt.
 *  0: No effect
 *  1: Clear timeout interrupt
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR    (BIT(16))
#define HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_M  (HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_V << HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_S)
#define HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_V  0x00000001U
#define HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_S  16
/** HP_SYSTEM_HP_PERI_TIMEOUT_PROTECT_EN : R/W; bitpos: [17]; default: 1;
 *  Configures whether or not to enable timeout protection for accessing HP peripheral
 *  registers.
 *  0: Disable
 *  1: Enable
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_PROTECT_EN    (BIT(17))
#define HP_SYSTEM_HP_PERI_TIMEOUT_PROTECT_EN_M  (HP_SYSTEM_HP_PERI_TIMEOUT_PROTECT_EN_V << HP_SYSTEM_HP_PERI_TIMEOUT_PROTECT_EN_S)
#define HP_SYSTEM_HP_PERI_TIMEOUT_PROTECT_EN_V  0x00000001U
#define HP_SYSTEM_HP_PERI_TIMEOUT_PROTECT_EN_S  17

/** HP_SYSTEM_HP_PERI_TIMEOUT_ADDR_REG register
 *  HP_PERI_TIMEOUT_ADDR register
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_ADDR_REG (DR_REG_HP_SYSTEM_BASE + 0x1c)
/** HP_SYSTEM_HP_PERI_TIMEOUT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Represents the address information of abnormal access.
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_ADDR    0xFFFFFFFFU
#define HP_SYSTEM_HP_PERI_TIMEOUT_ADDR_M  (HP_SYSTEM_HP_PERI_TIMEOUT_ADDR_V << HP_SYSTEM_HP_PERI_TIMEOUT_ADDR_S)
#define HP_SYSTEM_HP_PERI_TIMEOUT_ADDR_V  0xFFFFFFFFU
#define HP_SYSTEM_HP_PERI_TIMEOUT_ADDR_S  0

/** HP_SYSTEM_HP_PERI_TIMEOUT_UID_REG register
 *  HP_PERI_TIMEOUT_UID register
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_UID_REG (DR_REG_HP_SYSTEM_BASE + 0x20)
/** HP_SYSTEM_HP_PERI_TIMEOUT_UID : RO; bitpos: [6:0]; default: 0;
 *  Represents the master id[4:0] and master permission[6:5] when trigger timeout. This
 *  register will be cleared after the interrupt is cleared.
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_UID    0x0000007FU
#define HP_SYSTEM_HP_PERI_TIMEOUT_UID_M  (HP_SYSTEM_HP_PERI_TIMEOUT_UID_V << HP_SYSTEM_HP_PERI_TIMEOUT_UID_S)
#define HP_SYSTEM_HP_PERI_TIMEOUT_UID_V  0x0000007FU
#define HP_SYSTEM_HP_PERI_TIMEOUT_UID_S  0

/** HP_SYSTEM_SDIO_CTRL_REG register
 *  SDIO Control configuration register
 */
#define HP_SYSTEM_SDIO_CTRL_REG (DR_REG_HP_SYSTEM_BASE + 0x30)
/** HP_SYSTEM_DIS_SDIO_PROB : R/W; bitpos: [0]; default: 1;
 *  Set this bit as 1 to disable SDIO_PROB function. disable by default.
 */
#define HP_SYSTEM_DIS_SDIO_PROB    (BIT(0))
#define HP_SYSTEM_DIS_SDIO_PROB_M  (HP_SYSTEM_DIS_SDIO_PROB_V << HP_SYSTEM_DIS_SDIO_PROB_S)
#define HP_SYSTEM_DIS_SDIO_PROB_V  0x00000001U
#define HP_SYSTEM_DIS_SDIO_PROB_S  0
/** HP_SYSTEM_SDIO_WIN_ACCESS_EN : R/W; bitpos: [1]; default: 1;
 *  Enable sdio slave to access other peripherals on the chip
 */
#define HP_SYSTEM_SDIO_WIN_ACCESS_EN    (BIT(1))
#define HP_SYSTEM_SDIO_WIN_ACCESS_EN_M  (HP_SYSTEM_SDIO_WIN_ACCESS_EN_V << HP_SYSTEM_SDIO_WIN_ACCESS_EN_S)
#define HP_SYSTEM_SDIO_WIN_ACCESS_EN_V  0x00000001U
#define HP_SYSTEM_SDIO_WIN_ACCESS_EN_S  1

/** HP_SYSTEM_ROM_TABLE_LOCK_REG register
 *  ROM-Table lock register
 */
#define HP_SYSTEM_ROM_TABLE_LOCK_REG (DR_REG_HP_SYSTEM_BASE + 0x38)
/** HP_SYSTEM_ROM_TABLE_LOCK : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to lock the value contained in HP_SYSTEM_ROM_TABLE.
 *  0: Unlock
 *  1: Lock
 */
#define HP_SYSTEM_ROM_TABLE_LOCK    (BIT(0))
#define HP_SYSTEM_ROM_TABLE_LOCK_M  (HP_SYSTEM_ROM_TABLE_LOCK_V << HP_SYSTEM_ROM_TABLE_LOCK_S)
#define HP_SYSTEM_ROM_TABLE_LOCK_V  0x00000001U
#define HP_SYSTEM_ROM_TABLE_LOCK_S  0

/** HP_SYSTEM_ROM_TABLE_REG register
 *  ROM-Table register
 */
#define HP_SYSTEM_ROM_TABLE_REG (DR_REG_HP_SYSTEM_BASE + 0x3c)
/** HP_SYSTEM_ROM_TABLE : R/W; bitpos: [31:0]; default: 0;
 *  Software ROM-Table register, whose content can be modified only when
 *  HP_SYSTEM_ROM_TABLE_LOCK is 0.
 */
#define HP_SYSTEM_ROM_TABLE    0xFFFFFFFFU
#define HP_SYSTEM_ROM_TABLE_M  (HP_SYSTEM_ROM_TABLE_V << HP_SYSTEM_ROM_TABLE_S)
#define HP_SYSTEM_ROM_TABLE_V  0xFFFFFFFFU
#define HP_SYSTEM_ROM_TABLE_S  0

/** HP_SYSTEM_CORE_DEBUG_RUNSTALL_CONF_REG register
 *  Core Debug RunStall configurion register
 */
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x40)
/** HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable debug RunStall functionality between HP CPU and
 *  LP CPU.
 *  0: Disable
 *  1: Enable
 */
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE    (BIT(0))
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_M  (HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_V << HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_S)
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_V  0x00000001U
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_S  0
/** HP_SYSTEM_CORE0_RUNSTALLED : RO; bitpos: [1]; default: 0;
 *  Software can read this field to get the runstall status of hp-core0. 1: stalled, 0:
 *  not stalled.
 */
#define HP_SYSTEM_CORE0_RUNSTALLED    (BIT(1))
#define HP_SYSTEM_CORE0_RUNSTALLED_M  (HP_SYSTEM_CORE0_RUNSTALLED_V << HP_SYSTEM_CORE0_RUNSTALLED_S)
#define HP_SYSTEM_CORE0_RUNSTALLED_V  0x00000001U
#define HP_SYSTEM_CORE0_RUNSTALLED_S  1
/** HP_SYSTEM_CORE1_RUNSTALLED : RO; bitpos: [2]; default: 0;
 *  Software can read this field to get the runstall status of hp-core1. 1: stalled, 0:
 *  not stalled.
 */
#define HP_SYSTEM_CORE1_RUNSTALLED    (BIT(2))
#define HP_SYSTEM_CORE1_RUNSTALLED_M  (HP_SYSTEM_CORE1_RUNSTALLED_V << HP_SYSTEM_CORE1_RUNSTALLED_S)
#define HP_SYSTEM_CORE1_RUNSTALLED_V  0x00000001U
#define HP_SYSTEM_CORE1_RUNSTALLED_S  2

/** HP_SYSTEM_SPROM_CTRL_REG register
 *  reserved
 */
#define HP_SYSTEM_SPROM_CTRL_REG (DR_REG_HP_SYSTEM_BASE + 0x70)
/** HP_SYSTEM_SPROM_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 80;
 *  reserved
 */
#define HP_SYSTEM_SPROM_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYSTEM_SPROM_MEM_AUX_CTRL_M  (HP_SYSTEM_SPROM_MEM_AUX_CTRL_V << HP_SYSTEM_SPROM_MEM_AUX_CTRL_S)
#define HP_SYSTEM_SPROM_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYSTEM_SPROM_MEM_AUX_CTRL_S  0

/** HP_SYSTEM_SPRAM_CTRL_REG register
 *  reserved
 */
#define HP_SYSTEM_SPRAM_CTRL_REG (DR_REG_HP_SYSTEM_BASE + 0x74)
/** HP_SYSTEM_SPRAM_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 10320;
 *  reserved
 */
#define HP_SYSTEM_SPRAM_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYSTEM_SPRAM_MEM_AUX_CTRL_M  (HP_SYSTEM_SPRAM_MEM_AUX_CTRL_V << HP_SYSTEM_SPRAM_MEM_AUX_CTRL_S)
#define HP_SYSTEM_SPRAM_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYSTEM_SPRAM_MEM_AUX_CTRL_S  0

/** HP_SYSTEM_SPRF_CTRL_REG register
 *  reserved
 */
#define HP_SYSTEM_SPRF_CTRL_REG (DR_REG_HP_SYSTEM_BASE + 0x78)
/** HP_SYSTEM_SPRF_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 10320;
 *  reserved
 */
#define HP_SYSTEM_SPRF_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYSTEM_SPRF_MEM_AUX_CTRL_M  (HP_SYSTEM_SPRF_MEM_AUX_CTRL_V << HP_SYSTEM_SPRF_MEM_AUX_CTRL_S)
#define HP_SYSTEM_SPRF_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYSTEM_SPRF_MEM_AUX_CTRL_S  0

/** HP_SYSTEM_BITSCRAMBLER_PERI_SEL_REG register
 *  reserved
 */
#define HP_SYSTEM_BITSCRAMBLER_PERI_SEL_REG (DR_REG_HP_SYSTEM_BASE + 0x80)
/** HP_SYSTEM_BITSCRAMBLER_RX_SEL : R/W; bitpos: [3:0]; default: 0;
 *  select peri that will be connected to bitscrambler,dir : receive data from bs
 */
#define HP_SYSTEM_BITSCRAMBLER_RX_SEL    0x0000000FU
#define HP_SYSTEM_BITSCRAMBLER_RX_SEL_M  (HP_SYSTEM_BITSCRAMBLER_RX_SEL_V << HP_SYSTEM_BITSCRAMBLER_RX_SEL_S)
#define HP_SYSTEM_BITSCRAMBLER_RX_SEL_V  0x0000000FU
#define HP_SYSTEM_BITSCRAMBLER_RX_SEL_S  0
/** HP_SYSTEM_BITSCRAMBLER_TX_SEL : R/W; bitpos: [7:4]; default: 0;
 *  select peri that will be connected to bitscrambler,dir : transfer data to peri
 */
#define HP_SYSTEM_BITSCRAMBLER_TX_SEL    0x0000000FU
#define HP_SYSTEM_BITSCRAMBLER_TX_SEL_M  (HP_SYSTEM_BITSCRAMBLER_TX_SEL_V << HP_SYSTEM_BITSCRAMBLER_TX_SEL_S)
#define HP_SYSTEM_BITSCRAMBLER_TX_SEL_V  0x0000000FU
#define HP_SYSTEM_BITSCRAMBLER_TX_SEL_S  4

/** HP_SYSTEM_APPCPU_BOOT_ADDR_REG register
 *  reserved
 */
#define HP_SYSTEM_APPCPU_BOOT_ADDR_REG (DR_REG_HP_SYSTEM_BASE + 0x84)
/** HP_SYSTEM_APPCPU_BOOT_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define HP_SYSTEM_APPCPU_BOOT_ADDR    0xFFFFFFFFU
#define HP_SYSTEM_APPCPU_BOOT_ADDR_M  (HP_SYSTEM_APPCPU_BOOT_ADDR_V << HP_SYSTEM_APPCPU_BOOT_ADDR_S)
#define HP_SYSTEM_APPCPU_BOOT_ADDR_V  0xFFFFFFFFU
#define HP_SYSTEM_APPCPU_BOOT_ADDR_S  0

/** HP_SYSTEM_AXI_MST_PRI_REG register
 *  AXI mst priority configuration register
 */
#define HP_SYSTEM_AXI_MST_PRI_REG (DR_REG_HP_SYSTEM_BASE + 0x88)
/** HP_SYSTEM_DMA_PRIORITY : R/W; bitpos: [0]; default: 0;
 *  AHB-DMA arbitration priority for command channels between masters connected to
 *  ext_mem_DW_axi
 */
#define HP_SYSTEM_DMA_PRIORITY    (BIT(0))
#define HP_SYSTEM_DMA_PRIORITY_M  (HP_SYSTEM_DMA_PRIORITY_V << HP_SYSTEM_DMA_PRIORITY_S)
#define HP_SYSTEM_DMA_PRIORITY_V  0x00000001U
#define HP_SYSTEM_DMA_PRIORITY_S  0
/** HP_SYSTEM_CACHE_PRIORITY : R/W; bitpos: [1]; default: 0;
 *  CACHE arbitration priority for command channels between masters connected to
 *  ext_mem_DW_axi
 */
#define HP_SYSTEM_CACHE_PRIORITY    (BIT(1))
#define HP_SYSTEM_CACHE_PRIORITY_M  (HP_SYSTEM_CACHE_PRIORITY_V << HP_SYSTEM_CACHE_PRIORITY_S)
#define HP_SYSTEM_CACHE_PRIORITY_V  0x00000001U
#define HP_SYSTEM_CACHE_PRIORITY_S  1

/** HP_SYSTEM_CPU_PERI_PMS_CONF_REG register
 *  CPU Peripherals PMS configuration register
 */
#define HP_SYSTEM_CPU_PERI_PMS_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x90)
/** HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear cpu peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_CLR    (BIT(0))
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_CLR_M  (HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_CLR_V << HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_CLR_S)
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_CLR_V  0x00000001U
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_CLR_S  0

/** HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_INFO_REG register
 *  CPU Peripherals PMS exception info record register
 */
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_INFO_REG (DR_REG_HP_SYSTEM_BASE + 0x94)
/** HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_DET : RO; bitpos: [0]; default: 0;
 *  Represents whether the cpu peripheral pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_DET    (BIT(0))
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_DET_M  (HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_DET_V << HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_DET_S)
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_DET_V  0x00000001U
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_DET_S  0
/** HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ID : RO; bitpos: [5:1]; default: 0;
 *  Represents the master id when cpu peripheral pms has been triggered.
 */
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ID    0x0000001FU
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ID_M  (HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ID_V << HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ID_S)
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ID_V  0x0000001FU
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ID_S  1
/** HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_MODE : RO; bitpos: [7:6]; default: 0;
 *  Represents the security mode when cpu peripheral pms has been triggered.
 */
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_MODE    0x00000003U
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_MODE_M  (HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_MODE_V << HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_MODE_S)
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_MODE_V  0x00000003U
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_MODE_S  6
/** HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ADDR : RO; bitpos: [31:8]; default: 0;
 *  Represents the access address (bit23~bit0) when cpu peripheral pms has been
 *  triggered.
 */
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ADDR    0x00FFFFFFU
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ADDR_M  (HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ADDR_V << HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ADDR_S)
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ADDR_V  0x00FFFFFFU
#define HP_SYSTEM_CPU_PERI_PMS_EXCEPTION_ADDR_S  8

/** HP_SYSTEM_HP_PERI_PMS_CONF_REG register
 *  HP Peripherals PMS configuration register
 */
#define HP_SYSTEM_HP_PERI_PMS_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x98)
/** HP_SYSTEM_HP_PERI_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear hp peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_CLR    (BIT(0))
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_CLR_M  (HP_SYSTEM_HP_PERI_PMS_EXCEPTION_CLR_V << HP_SYSTEM_HP_PERI_PMS_EXCEPTION_CLR_S)
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_CLR_V  0x00000001U
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_CLR_S  0

/** HP_SYSTEM_HP_PERI_PMS_EXCEPTION_INFO_REG register
 *  HP Peripherals PMS exception info record register
 */
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_INFO_REG (DR_REG_HP_SYSTEM_BASE + 0x9c)
/** HP_SYSTEM_HP_PERI_PMS_EXCEPTION_DET : RO; bitpos: [0]; default: 0;
 *  Represents whether the hp peripheral pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_DET    (BIT(0))
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_DET_M  (HP_SYSTEM_HP_PERI_PMS_EXCEPTION_DET_V << HP_SYSTEM_HP_PERI_PMS_EXCEPTION_DET_S)
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_DET_V  0x00000001U
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_DET_S  0
/** HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ID : RO; bitpos: [5:1]; default: 0;
 *  Represents the master id when hp peripheral pms has been triggered.
 */
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ID    0x0000001FU
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ID_M  (HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ID_V << HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ID_S)
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ID_V  0x0000001FU
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ID_S  1
/** HP_SYSTEM_HP_PERI_PMS_EXCEPTION_MODE : RO; bitpos: [7:6]; default: 0;
 *  Represents the security mode when hp peripheral pms has been triggered.
 */
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_MODE    0x00000003U
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_MODE_M  (HP_SYSTEM_HP_PERI_PMS_EXCEPTION_MODE_V << HP_SYSTEM_HP_PERI_PMS_EXCEPTION_MODE_S)
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_MODE_V  0x00000003U
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_MODE_S  6
/** HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ADDR : RO; bitpos: [31:8]; default: 0;
 *  Represents the access address (bit23~bit0) when hp peripheral pms has been
 *  triggered.
 */
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ADDR    0x00FFFFFFU
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ADDR_M  (HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ADDR_V << HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ADDR_S)
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ADDR_V  0x00FFFFFFU
#define HP_SYSTEM_HP_PERI_PMS_EXCEPTION_ADDR_S  8

/** HP_SYSTEM_MODEM_PERI_PMS_CONF_REG register
 *  MODEM Peripherals PMS configuration register
 */
#define HP_SYSTEM_MODEM_PERI_PMS_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0xa0)
/** HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear modem peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_CLR    (BIT(0))
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_CLR_M  (HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_CLR_V << HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_CLR_S)
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_CLR_V  0x00000001U
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_CLR_S  0

/** HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_INFO_REG register
 *  MODEM Peripherals PMS exception info record register
 */
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_INFO_REG (DR_REG_HP_SYSTEM_BASE + 0xa4)
/** HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_DET : RO; bitpos: [0]; default: 0;
 *  Represents whether the modem peripheral pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_DET    (BIT(0))
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_DET_M  (HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_DET_V << HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_DET_S)
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_DET_V  0x00000001U
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_DET_S  0
/** HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ID : RO; bitpos: [5:1]; default: 0;
 *  Represents the master id when modem peripheral pms has been triggered.
 */
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ID    0x0000001FU
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ID_M  (HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ID_V << HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ID_S)
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ID_V  0x0000001FU
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ID_S  1
/** HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_MODE : RO; bitpos: [7:6]; default: 0;
 *  Represents the security mode when modem peripheral pms has been triggered.
 */
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_MODE    0x00000003U
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_MODE_M  (HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_MODE_V << HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_MODE_S)
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_MODE_V  0x00000003U
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_MODE_S  6
/** HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ADDR : RO; bitpos: [31:8]; default: 0;
 *  Represents the access address (bit23~bit0) when modem peripheral pms has been
 *  triggered.
 */
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ADDR    0x00FFFFFFU
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ADDR_M  (HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ADDR_V << HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ADDR_S)
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ADDR_V  0x00FFFFFFU
#define HP_SYSTEM_MODEM_PERI_PMS_EXCEPTION_ADDR_S  8

/** HP_SYSTEM_ID_REG register
 *  ID register
 */
#define HP_SYSTEM_ID_REG (DR_REG_HP_SYSTEM_BASE + 0x3dc)
/** HP_SYSTEM_ROM_ID : RO; bitpos: [27:12]; default: 0;
 *  Represents the ROM ID of chip
 */
#define HP_SYSTEM_ROM_ID    0x0000FFFFU
#define HP_SYSTEM_ROM_ID_M  (HP_SYSTEM_ROM_ID_V << HP_SYSTEM_ROM_ID_S)
#define HP_SYSTEM_ROM_ID_V  0x0000FFFFU
#define HP_SYSTEM_ROM_ID_S  12

/** HP_SYSTEM_RST_EN_REG register
 *  PCR clock gating configure register
 */
#define HP_SYSTEM_RST_EN_REG (DR_REG_HP_SYSTEM_BASE + 0x3f0)
/** HP_SYSTEM_HPSYSREG_RST_EN : R/W; bitpos: [0]; default: 0;
 *  Set 0 to reset hp_system_reg module
 */
#define HP_SYSTEM_HPSYSREG_RST_EN    (BIT(0))
#define HP_SYSTEM_HPSYSREG_RST_EN_M  (HP_SYSTEM_HPSYSREG_RST_EN_V << HP_SYSTEM_HPSYSREG_RST_EN_S)
#define HP_SYSTEM_HPSYSREG_RST_EN_V  0x00000001U
#define HP_SYSTEM_HPSYSREG_RST_EN_S  0

/** HP_SYSTEM_DATE_REG register
 *  Date control and version control register
 */
#define HP_SYSTEM_DATE_REG (DR_REG_HP_SYSTEM_BASE + 0x3fc)
/** HP_SYSTEM_DATE : R/W; bitpos: [27:0]; default: 37823056;
 *  Version control register.
 */
#define HP_SYSTEM_DATE    0x0FFFFFFFU
#define HP_SYSTEM_DATE_M  (HP_SYSTEM_DATE_V << HP_SYSTEM_DATE_S)
#define HP_SYSTEM_DATE_V  0x0FFFFFFFU
#define HP_SYSTEM_DATE_S  0

#ifdef __cplusplus
}
#endif
