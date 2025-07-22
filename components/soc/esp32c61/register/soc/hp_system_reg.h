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

/** HP_SYSTEM_SRAM_USAGE_CONF_REG register
 *  HP memory usage configuration register
 */
#define HP_SYSTEM_SRAM_USAGE_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x4)
/** HP_SYSTEM_SRAM_USAGE : R/W; bitpos: [12:8]; default: 0;
 *  0: cpu use hp-memory. 1:mac-dump accessing hp-memory.
 */
#define HP_SYSTEM_SRAM_USAGE    0x0000001FU
#define HP_SYSTEM_SRAM_USAGE_M  (HP_SYSTEM_SRAM_USAGE_V << HP_SYSTEM_SRAM_USAGE_S)
#define HP_SYSTEM_SRAM_USAGE_V  0x0000001FU
#define HP_SYSTEM_SRAM_USAGE_S  8
/** HP_SYSTEM_MAC_DUMP_ALLOC : R/W; bitpos: [16]; default: 0;
 *  Set this bit as 1 to add an offset (64KB) when mac-dump accessing hp-memory.
 */
#define HP_SYSTEM_MAC_DUMP_ALLOC    (BIT(16))
#define HP_SYSTEM_MAC_DUMP_ALLOC_M  (HP_SYSTEM_MAC_DUMP_ALLOC_V << HP_SYSTEM_MAC_DUMP_ALLOC_S)
#define HP_SYSTEM_MAC_DUMP_ALLOC_V  0x00000001U
#define HP_SYSTEM_MAC_DUMP_ALLOC_S  16

/** HP_SYSTEM_SEC_DPA_CONF_REG register
 *  HP anti-DPA security configuration register
 */
#define HP_SYSTEM_SEC_DPA_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x8)
/** HP_SYSTEM_SEC_DPA_LEVEL : R/W; bitpos: [1:0]; default: 0;
 *  Configures whether or not to enable anti-DPA attack. Valid only when
 *  HP_SYSTEM_SEC_DPA_CFG_SEL is 0.
 *  0: Disable
 *  1-3: Enable. The larger the number, the higher the security level, which represents
 *  the ability to resist DPA attacks, with increased computational overhead of the
 *  hardware crypto-accelerators at the same time.
 */
#define HP_SYSTEM_SEC_DPA_LEVEL    0x00000003U
#define HP_SYSTEM_SEC_DPA_LEVEL_M  (HP_SYSTEM_SEC_DPA_LEVEL_V << HP_SYSTEM_SEC_DPA_LEVEL_S)
#define HP_SYSTEM_SEC_DPA_LEVEL_V  0x00000003U
#define HP_SYSTEM_SEC_DPA_LEVEL_S  0
/** HP_SYSTEM_SEC_DPA_CFG_SEL : R/W; bitpos: [2]; default: 0;
 *  Configures whether to select HP_SYSTEM_SEC_DPA_LEVEL or EFUSE_SEC_DPA_LEVEL (from
 *  eFuse) to control DPA level.
 *  0: Select EFUSE_SEC_DPA_LEVEL
 *  1: Select HP_SYSTEM_SEC_DPA_LEVEL
 */
#define HP_SYSTEM_SEC_DPA_CFG_SEL    (BIT(2))
#define HP_SYSTEM_SEC_DPA_CFG_SEL_M  (HP_SYSTEM_SEC_DPA_CFG_SEL_V << HP_SYSTEM_SEC_DPA_CFG_SEL_S)
#define HP_SYSTEM_SEC_DPA_CFG_SEL_V  0x00000001U
#define HP_SYSTEM_SEC_DPA_CFG_SEL_S  2

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

/** HP_SYSTEM_ROM_TABLE_LOCK_REG register
 *  ROM-Table lock register
 */
#define HP_SYSTEM_ROM_TABLE_LOCK_REG (DR_REG_HP_SYSTEM_BASE + 0x30)
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
#define HP_SYSTEM_ROM_TABLE_REG (DR_REG_HP_SYSTEM_BASE + 0x34)
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
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x38)
/** HP_SYSTEM_CORE_RUNSTALLED : RO; bitpos: [1]; default: 0;
 *  Software can read this field to get the runstall status of hp-core. 1: stalled, 0:
 *  not stalled.
 */
#define HP_SYSTEM_CORE_RUNSTALLED    (BIT(1))
#define HP_SYSTEM_CORE_RUNSTALLED_M  (HP_SYSTEM_CORE_RUNSTALLED_V << HP_SYSTEM_CORE_RUNSTALLED_S)
#define HP_SYSTEM_CORE_RUNSTALLED_V  0x00000001U
#define HP_SYSTEM_CORE_RUNSTALLED_S  1

/** HP_SYSTEM_SPROM_CTRL_REG register
 *  reserved
 */
#define HP_SYSTEM_SPROM_CTRL_REG (DR_REG_HP_SYSTEM_BASE + 0x3c)
/** HP_SYSTEM_SPROM_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 112;
 *  reserved
 */
#define HP_SYSTEM_SPROM_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYSTEM_SPROM_MEM_AUX_CTRL_M  (HP_SYSTEM_SPROM_MEM_AUX_CTRL_V << HP_SYSTEM_SPROM_MEM_AUX_CTRL_S)
#define HP_SYSTEM_SPROM_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYSTEM_SPROM_MEM_AUX_CTRL_S  0

/** HP_SYSTEM_SPRAM_CTRL_REG register
 *  reserved
 */
#define HP_SYSTEM_SPRAM_CTRL_REG (DR_REG_HP_SYSTEM_BASE + 0x40)
/** HP_SYSTEM_SPRAM_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 8304;
 *  reserved
 */
#define HP_SYSTEM_SPRAM_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYSTEM_SPRAM_MEM_AUX_CTRL_M  (HP_SYSTEM_SPRAM_MEM_AUX_CTRL_V << HP_SYSTEM_SPRAM_MEM_AUX_CTRL_S)
#define HP_SYSTEM_SPRAM_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYSTEM_SPRAM_MEM_AUX_CTRL_S  0

/** HP_SYSTEM_SPRF_CTRL_REG register
 *  reserved
 */
#define HP_SYSTEM_SPRF_CTRL_REG (DR_REG_HP_SYSTEM_BASE + 0x44)
/** HP_SYSTEM_SPRF_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 8304;
 *  reserved
 */
#define HP_SYSTEM_SPRF_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYSTEM_SPRF_MEM_AUX_CTRL_M  (HP_SYSTEM_SPRF_MEM_AUX_CTRL_V << HP_SYSTEM_SPRF_MEM_AUX_CTRL_S)
#define HP_SYSTEM_SPRF_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYSTEM_SPRF_MEM_AUX_CTRL_S  0

/** HP_SYSTEM_SDPRF_CTRL_REG register
 *  reserved
 */
#define HP_SYSTEM_SDPRF_CTRL_REG (DR_REG_HP_SYSTEM_BASE + 0x48)
/** HP_SYSTEM_SDPRF_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define HP_SYSTEM_SDPRF_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYSTEM_SDPRF_MEM_AUX_CTRL_M  (HP_SYSTEM_SDPRF_MEM_AUX_CTRL_V << HP_SYSTEM_SDPRF_MEM_AUX_CTRL_S)
#define HP_SYSTEM_SDPRF_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYSTEM_SDPRF_MEM_AUX_CTRL_S  0

/** HP_SYSTEM_CLOCK_GATE_REG register
 *  HP-SYSTEM clock gating configure register
 *  This register is only for internal debugging purposes. Do not use it in
 *  applications.
 */
#define HP_SYSTEM_CLOCK_GATE_REG (DR_REG_HP_SYSTEM_BASE + 0x5c)
/** HP_SYSTEM_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set this bit as 1 to force on clock gating.
 *  This field is only for internal debugging purposes. Do not use it in applications.
 */
#define HP_SYSTEM_CLK_EN    (BIT(0))
#define HP_SYSTEM_CLK_EN_M  (HP_SYSTEM_CLK_EN_V << HP_SYSTEM_CLK_EN_S)
#define HP_SYSTEM_CLK_EN_V  0x00000001U
#define HP_SYSTEM_CLK_EN_S  0

/** HP_SYSTEM_AXI_MST_PRI_REG register
 *  AXI mst priority configuration register
 */
#define HP_SYSTEM_AXI_MST_PRI_REG (DR_REG_HP_SYSTEM_BASE + 0x60)
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

/** HP_SYSTEM_SDIO_CTRL_REG register
 *  SDIO Control configuration register
 */
#define HP_SYSTEM_SDIO_CTRL_REG (DR_REG_HP_SYSTEM_BASE + 0x64)
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

/** HP_SYSTEM_DATE_REG register
 *  Date control and version control register
 */
#define HP_SYSTEM_DATE_REG (DR_REG_HP_SYSTEM_BASE + 0x3fc)
/** HP_SYSTEM_DATE : R/W; bitpos: [27:0]; default: 37823088;
 *  Version control register.
 */
#define HP_SYSTEM_DATE    0x0FFFFFFFU
#define HP_SYSTEM_DATE_M  (HP_SYSTEM_DATE_V << HP_SYSTEM_DATE_S)
#define HP_SYSTEM_DATE_V  0x0FFFFFFFU
#define HP_SYSTEM_DATE_S  0

#ifdef __cplusplus
}
#endif
