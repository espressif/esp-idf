/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** HP_SYS_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG register
 *  EXTERNAL DEVICE ENCRYPTION/DECRYPTION configuration register
 */
#define HP_SYS_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG (DR_REG_HP_SYS_BASE + 0x0)
/** HP_SYS_ENABLE_SPI_MANUAL_ENCRYPT : R/W; bitpos: [0]; default: 0;
 *  Set this bit as 1 to enable mspi xts manual encrypt in spi boot mode.
 */
#define HP_SYS_ENABLE_SPI_MANUAL_ENCRYPT    (BIT(0))
#define HP_SYS_ENABLE_SPI_MANUAL_ENCRYPT_M  (HP_SYS_ENABLE_SPI_MANUAL_ENCRYPT_V << HP_SYS_ENABLE_SPI_MANUAL_ENCRYPT_S)
#define HP_SYS_ENABLE_SPI_MANUAL_ENCRYPT_V  0x00000001U
#define HP_SYS_ENABLE_SPI_MANUAL_ENCRYPT_S  0
/** HP_SYS_ENABLE_DOWNLOAD_DB_ENCRYPT : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define HP_SYS_ENABLE_DOWNLOAD_DB_ENCRYPT    (BIT(1))
#define HP_SYS_ENABLE_DOWNLOAD_DB_ENCRYPT_M  (HP_SYS_ENABLE_DOWNLOAD_DB_ENCRYPT_V << HP_SYS_ENABLE_DOWNLOAD_DB_ENCRYPT_S)
#define HP_SYS_ENABLE_DOWNLOAD_DB_ENCRYPT_V  0x00000001U
#define HP_SYS_ENABLE_DOWNLOAD_DB_ENCRYPT_S  1
/** HP_SYS_ENABLE_DOWNLOAD_G0CB_DECRYPT : R/W; bitpos: [2]; default: 0;
 *  Set this bit as 1 to enable mspi xts auto decrypt in download boot mode.
 */
#define HP_SYS_ENABLE_DOWNLOAD_G0CB_DECRYPT    (BIT(2))
#define HP_SYS_ENABLE_DOWNLOAD_G0CB_DECRYPT_M  (HP_SYS_ENABLE_DOWNLOAD_G0CB_DECRYPT_V << HP_SYS_ENABLE_DOWNLOAD_G0CB_DECRYPT_S)
#define HP_SYS_ENABLE_DOWNLOAD_G0CB_DECRYPT_V  0x00000001U
#define HP_SYS_ENABLE_DOWNLOAD_G0CB_DECRYPT_S  2
/** HP_SYS_ENABLE_DOWNLOAD_MANUAL_ENCRYPT : R/W; bitpos: [3]; default: 0;
 *  Set this bit as 1 to enable mspi xts manual encrypt in download boot mode.
 */
#define HP_SYS_ENABLE_DOWNLOAD_MANUAL_ENCRYPT    (BIT(3))
#define HP_SYS_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_M  (HP_SYS_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_V << HP_SYS_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_S)
#define HP_SYS_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_V  0x00000001U
#define HP_SYS_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_S  3

/** HP_SYS_SRAM_USAGE_CONF_REG register
 *  HP memory usage configuration register
 */
#define HP_SYS_SRAM_USAGE_CONF_REG (DR_REG_HP_SYS_BASE + 0x4)
/** HP_SYS_SRAM_USAGE : R/W; bitpos: [11:8]; default: 0;
 *  0: cpu use hp-memory. 1:mac-dump accessing hp-memory.
 */
#define HP_SYS_SRAM_USAGE    0x0000000FU
#define HP_SYS_SRAM_USAGE_M  (HP_SYS_SRAM_USAGE_V << HP_SYS_SRAM_USAGE_S)
#define HP_SYS_SRAM_USAGE_V  0x0000000FU
#define HP_SYS_SRAM_USAGE_S  8
/** HP_SYS_MAC_DUMP_ALLOC : R/W; bitpos: [16]; default: 0;
 *  Set this bit as 1 to add an offset (64KB) when mac-dump accessing hp-memory.
 */
#define HP_SYS_MAC_DUMP_ALLOC    (BIT(16))
#define HP_SYS_MAC_DUMP_ALLOC_M  (HP_SYS_MAC_DUMP_ALLOC_V << HP_SYS_MAC_DUMP_ALLOC_S)
#define HP_SYS_MAC_DUMP_ALLOC_V  0x00000001U
#define HP_SYS_MAC_DUMP_ALLOC_S  16

/** HP_SYS_SEC_DPA_CONF_REG register
 *  HP anti-DPA security configuration register
 */
#define HP_SYS_SEC_DPA_CONF_REG (DR_REG_HP_SYS_BASE + 0x8)
/** HP_SYS_SEC_DPA_LEVEL : R/W; bitpos: [1:0]; default: 0;
 *  0: anti-DPA disable. 1~3: anti-DPA enable with different security level. The larger
 *  the number, the stronger the ability to resist DPA attacks and the higher the
 *  security level, but it will increase the computational overhead of the hardware
 *  crypto-accelerators. Only avaliable if HP_SYS_SEC_DPA_CFG_SEL is 0.
 */
#define HP_SYS_SEC_DPA_LEVEL    0x00000003U
#define HP_SYS_SEC_DPA_LEVEL_M  (HP_SYS_SEC_DPA_LEVEL_V << HP_SYS_SEC_DPA_LEVEL_S)
#define HP_SYS_SEC_DPA_LEVEL_V  0x00000003U
#define HP_SYS_SEC_DPA_LEVEL_S  0
/** HP_SYS_SEC_DPA_CFG_SEL : R/W; bitpos: [2]; default: 0;
 *  This field is used to select either HP_SYS_SEC_DPA_LEVEL or EFUSE_SEC_DPA_LEVEL
 *  (from efuse) to control dpa_level. 0: EFUSE_SEC_DPA_LEVEL, 1: HP_SYS_SEC_DPA_LEVEL.
 */
#define HP_SYS_SEC_DPA_CFG_SEL    (BIT(2))
#define HP_SYS_SEC_DPA_CFG_SEL_M  (HP_SYS_SEC_DPA_CFG_SEL_V << HP_SYS_SEC_DPA_CFG_SEL_S)
#define HP_SYS_SEC_DPA_CFG_SEL_V  0x00000001U
#define HP_SYS_SEC_DPA_CFG_SEL_S  2

/** HP_SYS_CPU_PERI_TIMEOUT_CONF_REG register
 *  CPU_PERI_TIMEOUT configuration register
 */
#define HP_SYS_CPU_PERI_TIMEOUT_CONF_REG (DR_REG_HP_SYS_BASE + 0xc)
/** HP_SYS_CPU_PERI_TIMEOUT_THRES : R/W; bitpos: [15:0]; default: 65535;
 *  Set the timeout threshold for bus access, corresponding to the number of clock
 *  cycles of the clock domain.
 */
#define HP_SYS_CPU_PERI_TIMEOUT_THRES    0x0000FFFFU
#define HP_SYS_CPU_PERI_TIMEOUT_THRES_M  (HP_SYS_CPU_PERI_TIMEOUT_THRES_V << HP_SYS_CPU_PERI_TIMEOUT_THRES_S)
#define HP_SYS_CPU_PERI_TIMEOUT_THRES_V  0x0000FFFFU
#define HP_SYS_CPU_PERI_TIMEOUT_THRES_S  0
/** HP_SYS_CPU_PERI_TIMEOUT_INT_CLEAR : WT; bitpos: [16]; default: 0;
 *  Set this bit as 1 to clear timeout interrupt
 */
#define HP_SYS_CPU_PERI_TIMEOUT_INT_CLEAR    (BIT(16))
#define HP_SYS_CPU_PERI_TIMEOUT_INT_CLEAR_M  (HP_SYS_CPU_PERI_TIMEOUT_INT_CLEAR_V << HP_SYS_CPU_PERI_TIMEOUT_INT_CLEAR_S)
#define HP_SYS_CPU_PERI_TIMEOUT_INT_CLEAR_V  0x00000001U
#define HP_SYS_CPU_PERI_TIMEOUT_INT_CLEAR_S  16
/** HP_SYS_CPU_PERI_TIMEOUT_PROTECT_EN : R/W; bitpos: [17]; default: 1;
 *  Set this bit as 1 to enable timeout protection for accessing cpu peripheral
 *  registers
 */
#define HP_SYS_CPU_PERI_TIMEOUT_PROTECT_EN    (BIT(17))
#define HP_SYS_CPU_PERI_TIMEOUT_PROTECT_EN_M  (HP_SYS_CPU_PERI_TIMEOUT_PROTECT_EN_V << HP_SYS_CPU_PERI_TIMEOUT_PROTECT_EN_S)
#define HP_SYS_CPU_PERI_TIMEOUT_PROTECT_EN_V  0x00000001U
#define HP_SYS_CPU_PERI_TIMEOUT_PROTECT_EN_S  17

/** HP_SYS_CPU_PERI_TIMEOUT_ADDR_REG register
 *  CPU_PERI_TIMEOUT_ADDR register
 */
#define HP_SYS_CPU_PERI_TIMEOUT_ADDR_REG (DR_REG_HP_SYS_BASE + 0x10)
/** HP_SYS_CPU_PERI_TIMEOUT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Record the address information of abnormal access
 */
#define HP_SYS_CPU_PERI_TIMEOUT_ADDR    0xFFFFFFFFU
#define HP_SYS_CPU_PERI_TIMEOUT_ADDR_M  (HP_SYS_CPU_PERI_TIMEOUT_ADDR_V << HP_SYS_CPU_PERI_TIMEOUT_ADDR_S)
#define HP_SYS_CPU_PERI_TIMEOUT_ADDR_V  0xFFFFFFFFU
#define HP_SYS_CPU_PERI_TIMEOUT_ADDR_S  0

/** HP_SYS_CPU_PERI_TIMEOUT_UID_REG register
 *  CPU_PERI_TIMEOUT_UID register
 */
#define HP_SYS_CPU_PERI_TIMEOUT_UID_REG (DR_REG_HP_SYS_BASE + 0x14)
/** HP_SYS_CPU_PERI_TIMEOUT_UID : RO; bitpos: [6:0]; default: 0;
 *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
 *  will be cleared after the interrupt is cleared.
 */
#define HP_SYS_CPU_PERI_TIMEOUT_UID    0x0000007FU
#define HP_SYS_CPU_PERI_TIMEOUT_UID_M  (HP_SYS_CPU_PERI_TIMEOUT_UID_V << HP_SYS_CPU_PERI_TIMEOUT_UID_S)
#define HP_SYS_CPU_PERI_TIMEOUT_UID_V  0x0000007FU
#define HP_SYS_CPU_PERI_TIMEOUT_UID_S  0

/** HP_SYS_HP_PERI_TIMEOUT_CONF_REG register
 *  HP_PERI_TIMEOUT configuration register
 */
#define HP_SYS_HP_PERI_TIMEOUT_CONF_REG (DR_REG_HP_SYS_BASE + 0x18)
/** HP_SYS_HP_PERI_TIMEOUT_THRES : R/W; bitpos: [15:0]; default: 65535;
 *  Set the timeout threshold for bus access, corresponding to the number of clock
 *  cycles of the clock domain.
 */
#define HP_SYS_HP_PERI_TIMEOUT_THRES    0x0000FFFFU
#define HP_SYS_HP_PERI_TIMEOUT_THRES_M  (HP_SYS_HP_PERI_TIMEOUT_THRES_V << HP_SYS_HP_PERI_TIMEOUT_THRES_S)
#define HP_SYS_HP_PERI_TIMEOUT_THRES_V  0x0000FFFFU
#define HP_SYS_HP_PERI_TIMEOUT_THRES_S  0
/** HP_SYS_HP_PERI_TIMEOUT_INT_CLEAR : WT; bitpos: [16]; default: 0;
 *  Set this bit as 1 to clear timeout interrupt
 */
#define HP_SYS_HP_PERI_TIMEOUT_INT_CLEAR    (BIT(16))
#define HP_SYS_HP_PERI_TIMEOUT_INT_CLEAR_M  (HP_SYS_HP_PERI_TIMEOUT_INT_CLEAR_V << HP_SYS_HP_PERI_TIMEOUT_INT_CLEAR_S)
#define HP_SYS_HP_PERI_TIMEOUT_INT_CLEAR_V  0x00000001U
#define HP_SYS_HP_PERI_TIMEOUT_INT_CLEAR_S  16
/** HP_SYS_HP_PERI_TIMEOUT_PROTECT_EN : R/W; bitpos: [17]; default: 1;
 *  Set this bit as 1 to enable timeout protection for accessing hp peripheral registers
 */
#define HP_SYS_HP_PERI_TIMEOUT_PROTECT_EN    (BIT(17))
#define HP_SYS_HP_PERI_TIMEOUT_PROTECT_EN_M  (HP_SYS_HP_PERI_TIMEOUT_PROTECT_EN_V << HP_SYS_HP_PERI_TIMEOUT_PROTECT_EN_S)
#define HP_SYS_HP_PERI_TIMEOUT_PROTECT_EN_V  0x00000001U
#define HP_SYS_HP_PERI_TIMEOUT_PROTECT_EN_S  17

/** HP_SYS_HP_PERI_TIMEOUT_ADDR_REG register
 *  HP_PERI_TIMEOUT_ADDR register
 */
#define HP_SYS_HP_PERI_TIMEOUT_ADDR_REG (DR_REG_HP_SYS_BASE + 0x1c)
/** HP_SYS_HP_PERI_TIMEOUT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Record the address information of abnormal access
 */
#define HP_SYS_HP_PERI_TIMEOUT_ADDR    0xFFFFFFFFU
#define HP_SYS_HP_PERI_TIMEOUT_ADDR_M  (HP_SYS_HP_PERI_TIMEOUT_ADDR_V << HP_SYS_HP_PERI_TIMEOUT_ADDR_S)
#define HP_SYS_HP_PERI_TIMEOUT_ADDR_V  0xFFFFFFFFU
#define HP_SYS_HP_PERI_TIMEOUT_ADDR_S  0

/** HP_SYS_HP_PERI_TIMEOUT_UID_REG register
 *  HP_PERI_TIMEOUT_UID register
 */
#define HP_SYS_HP_PERI_TIMEOUT_UID_REG (DR_REG_HP_SYS_BASE + 0x20)
/** HP_SYS_HP_PERI_TIMEOUT_UID : RO; bitpos: [6:0]; default: 0;
 *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
 *  will be cleared after the interrupt is cleared.
 */
#define HP_SYS_HP_PERI_TIMEOUT_UID    0x0000007FU
#define HP_SYS_HP_PERI_TIMEOUT_UID_M  (HP_SYS_HP_PERI_TIMEOUT_UID_V << HP_SYS_HP_PERI_TIMEOUT_UID_S)
#define HP_SYS_HP_PERI_TIMEOUT_UID_V  0x0000007FU
#define HP_SYS_HP_PERI_TIMEOUT_UID_S  0

/** HP_SYS_SDIO_CTRL_REG register
 *  SDIO Control configuration register
 */
#define HP_SYS_SDIO_CTRL_REG (DR_REG_HP_SYS_BASE + 0x30)
/** HP_SYS_DIS_SDIO_PROB : R/W; bitpos: [0]; default: 1;
 *  Set this bit as 1 to disable SDIO_PROB function. disable by default.
 */
#define HP_SYS_DIS_SDIO_PROB    (BIT(0))
#define HP_SYS_DIS_SDIO_PROB_M  (HP_SYS_DIS_SDIO_PROB_V << HP_SYS_DIS_SDIO_PROB_S)
#define HP_SYS_DIS_SDIO_PROB_V  0x00000001U
#define HP_SYS_DIS_SDIO_PROB_S  0
/** HP_SYS_SDIO_WIN_ACCESS_EN : R/W; bitpos: [1]; default: 1;
 *  Enable sdio slave to access other peripherals on the chip
 */
#define HP_SYS_SDIO_WIN_ACCESS_EN    (BIT(1))
#define HP_SYS_SDIO_WIN_ACCESS_EN_M  (HP_SYS_SDIO_WIN_ACCESS_EN_V << HP_SYS_SDIO_WIN_ACCESS_EN_S)
#define HP_SYS_SDIO_WIN_ACCESS_EN_V  0x00000001U
#define HP_SYS_SDIO_WIN_ACCESS_EN_S  1

/** HP_SYS_ROM_TABLE_LOCK_REG register
 *  Rom-Table lock register
 */
#define HP_SYS_ROM_TABLE_LOCK_REG (DR_REG_HP_SYS_BASE + 0x38)
/** HP_SYS_ROM_TABLE_LOCK : R/W; bitpos: [0]; default: 0;
 *  XXXX
 */
#define HP_SYS_ROM_TABLE_LOCK    (BIT(0))
#define HP_SYS_ROM_TABLE_LOCK_M  (HP_SYS_ROM_TABLE_LOCK_V << HP_SYS_ROM_TABLE_LOCK_S)
#define HP_SYS_ROM_TABLE_LOCK_V  0x00000001U
#define HP_SYS_ROM_TABLE_LOCK_S  0

/** HP_SYS_ROM_TABLE_REG register
 *  Rom-Table register
 */
#define HP_SYS_ROM_TABLE_REG (DR_REG_HP_SYS_BASE + 0x3c)
/** HP_SYS_ROM_TABLE : R/W; bitpos: [31:0]; default: 0;
 *  XXXX
 */
#define HP_SYS_ROM_TABLE    0xFFFFFFFFU
#define HP_SYS_ROM_TABLE_M  (HP_SYS_ROM_TABLE_V << HP_SYS_ROM_TABLE_S)
#define HP_SYS_ROM_TABLE_V  0xFFFFFFFFU
#define HP_SYS_ROM_TABLE_S  0

/** HP_SYS_CORE_DEBUG_RUNSTALL_CONF_REG register
 *  Core Debug runstall configure register
 */
#define HP_SYS_CORE_DEBUG_RUNSTALL_CONF_REG (DR_REG_HP_SYS_BASE + 0x40)
/** HP_SYS_CORE_DEBUG_RUNSTALL_ENABLE : R/W; bitpos: [0]; default: 0;
 *  Set this field to 1 to enable debug runstall feature between HP-core and LP-core.
 */
#define HP_SYS_CORE_DEBUG_RUNSTALL_ENABLE    (BIT(0))
#define HP_SYS_CORE_DEBUG_RUNSTALL_ENABLE_M  (HP_SYS_CORE_DEBUG_RUNSTALL_ENABLE_V << HP_SYS_CORE_DEBUG_RUNSTALL_ENABLE_S)
#define HP_SYS_CORE_DEBUG_RUNSTALL_ENABLE_V  0x00000001U
#define HP_SYS_CORE_DEBUG_RUNSTALL_ENABLE_S  0
/** HP_SYS_CORE_RUNSTALLED : RO; bitpos: [1]; default: 0;
 *  Software can read this field to get the runstall status of hp-core. 1: stalled, 0:
 *  not stalled.
 */
#define HP_SYS_CORE_RUNSTALLED    (BIT(1))
#define HP_SYS_CORE_RUNSTALLED_M  (HP_SYS_CORE_RUNSTALLED_V << HP_SYS_CORE_RUNSTALLED_S)
#define HP_SYS_CORE_RUNSTALLED_V  0x00000001U
#define HP_SYS_CORE_RUNSTALLED_S  1

/** HP_SYS_SPROM_CTRL_REG register
 *  reserved
 */
#define HP_SYS_SPROM_CTRL_REG (DR_REG_HP_SYS_BASE + 0x70)
/** HP_SYS_SPROM_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 112;
 *  reserved
 */
#define HP_SYS_SPROM_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYS_SPROM_MEM_AUX_CTRL_M  (HP_SYS_SPROM_MEM_AUX_CTRL_V << HP_SYS_SPROM_MEM_AUX_CTRL_S)
#define HP_SYS_SPROM_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYS_SPROM_MEM_AUX_CTRL_S  0

/** HP_SYS_SPRAM_CTRL_REG register
 *  reserved
 */
#define HP_SYS_SPRAM_CTRL_REG (DR_REG_HP_SYS_BASE + 0x74)
/** HP_SYS_SPRAM_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 8304;
 *  reserved
 */
#define HP_SYS_SPRAM_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYS_SPRAM_MEM_AUX_CTRL_M  (HP_SYS_SPRAM_MEM_AUX_CTRL_V << HP_SYS_SPRAM_MEM_AUX_CTRL_S)
#define HP_SYS_SPRAM_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYS_SPRAM_MEM_AUX_CTRL_S  0

/** HP_SYS_SPRF_CTRL_REG register
 *  reserved
 */
#define HP_SYS_SPRF_CTRL_REG (DR_REG_HP_SYS_BASE + 0x78)
/** HP_SYS_SPRF_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 8304;
 *  reserved
 */
#define HP_SYS_SPRF_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYS_SPRF_MEM_AUX_CTRL_M  (HP_SYS_SPRF_MEM_AUX_CTRL_V << HP_SYS_SPRF_MEM_AUX_CTRL_S)
#define HP_SYS_SPRF_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYS_SPRF_MEM_AUX_CTRL_S  0

/** HP_SYS_SDPRF_CTRL_REG register
 *  reserved
 */
#define HP_SYS_SDPRF_CTRL_REG (DR_REG_HP_SYS_BASE + 0x7c)
/** HP_SYS_SDPRF_MEM_AUX_CTRL : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define HP_SYS_SDPRF_MEM_AUX_CTRL    0xFFFFFFFFU
#define HP_SYS_SDPRF_MEM_AUX_CTRL_M  (HP_SYS_SDPRF_MEM_AUX_CTRL_V << HP_SYS_SDPRF_MEM_AUX_CTRL_S)
#define HP_SYS_SDPRF_MEM_AUX_CTRL_V  0xFFFFFFFFU
#define HP_SYS_SDPRF_MEM_AUX_CTRL_S  0

/** HP_SYS_AUDIO_CODEX_CTRL0_REG register
 *  reserved
 */
#define HP_SYS_AUDIO_CODEX_CTRL0_REG (DR_REG_HP_SYS_BASE + 0x80)
/** HP_SYS_DAC_IN_R1_IE : R/W; bitpos: [0]; default: 1;
 *  reserved
 */
#define HP_SYS_DAC_IN_R1_IE    (BIT(0))
#define HP_SYS_DAC_IN_R1_IE_M  (HP_SYS_DAC_IN_R1_IE_V << HP_SYS_DAC_IN_R1_IE_S)
#define HP_SYS_DAC_IN_R1_IE_V  0x00000001U
#define HP_SYS_DAC_IN_R1_IE_S  0
/** HP_SYS_DAC_IN_R1_OE : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define HP_SYS_DAC_IN_R1_OE    (BIT(1))
#define HP_SYS_DAC_IN_R1_OE_M  (HP_SYS_DAC_IN_R1_OE_V << HP_SYS_DAC_IN_R1_OE_S)
#define HP_SYS_DAC_IN_R1_OE_V  0x00000001U
#define HP_SYS_DAC_IN_R1_OE_S  1
/** HP_SYS_DAC_IN_R0_IE : R/W; bitpos: [2]; default: 1;
 *  reserved
 */
#define HP_SYS_DAC_IN_R0_IE    (BIT(2))
#define HP_SYS_DAC_IN_R0_IE_M  (HP_SYS_DAC_IN_R0_IE_V << HP_SYS_DAC_IN_R0_IE_S)
#define HP_SYS_DAC_IN_R0_IE_V  0x00000001U
#define HP_SYS_DAC_IN_R0_IE_S  2
/** HP_SYS_DAC_IN_R0_OE : R/W; bitpos: [3]; default: 0;
 *  reserved
 */
#define HP_SYS_DAC_IN_R0_OE    (BIT(3))
#define HP_SYS_DAC_IN_R0_OE_M  (HP_SYS_DAC_IN_R0_OE_V << HP_SYS_DAC_IN_R0_OE_S)
#define HP_SYS_DAC_IN_R0_OE_V  0x00000001U
#define HP_SYS_DAC_IN_R0_OE_S  3
/** HP_SYS_ADC_DATA_4_IE : R/W; bitpos: [4]; default: 0;
 *  reserved
 */
#define HP_SYS_ADC_DATA_4_IE    (BIT(4))
#define HP_SYS_ADC_DATA_4_IE_M  (HP_SYS_ADC_DATA_4_IE_V << HP_SYS_ADC_DATA_4_IE_S)
#define HP_SYS_ADC_DATA_4_IE_V  0x00000001U
#define HP_SYS_ADC_DATA_4_IE_S  4
/** HP_SYS_ADC_DATA_4_OE : R/W; bitpos: [5]; default: 1;
 *  reserved
 */
#define HP_SYS_ADC_DATA_4_OE    (BIT(5))
#define HP_SYS_ADC_DATA_4_OE_M  (HP_SYS_ADC_DATA_4_OE_V << HP_SYS_ADC_DATA_4_OE_S)
#define HP_SYS_ADC_DATA_4_OE_V  0x00000001U
#define HP_SYS_ADC_DATA_4_OE_S  5
/** HP_SYS_ADC_DATA_3_IE : R/W; bitpos: [6]; default: 0;
 *  reserved
 */
#define HP_SYS_ADC_DATA_3_IE    (BIT(6))
#define HP_SYS_ADC_DATA_3_IE_M  (HP_SYS_ADC_DATA_3_IE_V << HP_SYS_ADC_DATA_3_IE_S)
#define HP_SYS_ADC_DATA_3_IE_V  0x00000001U
#define HP_SYS_ADC_DATA_3_IE_S  6
/** HP_SYS_ADC_DATA_3_OE : R/W; bitpos: [7]; default: 1;
 *  reserved
 */
#define HP_SYS_ADC_DATA_3_OE    (BIT(7))
#define HP_SYS_ADC_DATA_3_OE_M  (HP_SYS_ADC_DATA_3_OE_V << HP_SYS_ADC_DATA_3_OE_S)
#define HP_SYS_ADC_DATA_3_OE_V  0x00000001U
#define HP_SYS_ADC_DATA_3_OE_S  7
/** HP_SYS_ADC_DATA_2_IE : R/W; bitpos: [8]; default: 0;
 *  reserved
 */
#define HP_SYS_ADC_DATA_2_IE    (BIT(8))
#define HP_SYS_ADC_DATA_2_IE_M  (HP_SYS_ADC_DATA_2_IE_V << HP_SYS_ADC_DATA_2_IE_S)
#define HP_SYS_ADC_DATA_2_IE_V  0x00000001U
#define HP_SYS_ADC_DATA_2_IE_S  8
/** HP_SYS_ADC_DATA_2_OE : R/W; bitpos: [9]; default: 1;
 *  reserved
 */
#define HP_SYS_ADC_DATA_2_OE    (BIT(9))
#define HP_SYS_ADC_DATA_2_OE_M  (HP_SYS_ADC_DATA_2_OE_V << HP_SYS_ADC_DATA_2_OE_S)
#define HP_SYS_ADC_DATA_2_OE_V  0x00000001U
#define HP_SYS_ADC_DATA_2_OE_S  9
/** HP_SYS_ADC_DATA_1_IE : R/W; bitpos: [10]; default: 0;
 *  reserved
 */
#define HP_SYS_ADC_DATA_1_IE    (BIT(10))
#define HP_SYS_ADC_DATA_1_IE_M  (HP_SYS_ADC_DATA_1_IE_V << HP_SYS_ADC_DATA_1_IE_S)
#define HP_SYS_ADC_DATA_1_IE_V  0x00000001U
#define HP_SYS_ADC_DATA_1_IE_S  10
/** HP_SYS_ADC_DATA_1_OE : R/W; bitpos: [11]; default: 1;
 *  reserved
 */
#define HP_SYS_ADC_DATA_1_OE    (BIT(11))
#define HP_SYS_ADC_DATA_1_OE_M  (HP_SYS_ADC_DATA_1_OE_V << HP_SYS_ADC_DATA_1_OE_S)
#define HP_SYS_ADC_DATA_1_OE_V  0x00000001U
#define HP_SYS_ADC_DATA_1_OE_S  11
/** HP_SYS_ADC_DATA_0_IE : R/W; bitpos: [12]; default: 0;
 *  reserved
 */
#define HP_SYS_ADC_DATA_0_IE    (BIT(12))
#define HP_SYS_ADC_DATA_0_IE_M  (HP_SYS_ADC_DATA_0_IE_V << HP_SYS_ADC_DATA_0_IE_S)
#define HP_SYS_ADC_DATA_0_IE_V  0x00000001U
#define HP_SYS_ADC_DATA_0_IE_S  12
/** HP_SYS_ADC_DATA_0_OE : R/W; bitpos: [13]; default: 1;
 *  reserved
 */
#define HP_SYS_ADC_DATA_0_OE    (BIT(13))
#define HP_SYS_ADC_DATA_0_OE_M  (HP_SYS_ADC_DATA_0_OE_V << HP_SYS_ADC_DATA_0_OE_S)
#define HP_SYS_ADC_DATA_0_OE_V  0x00000001U
#define HP_SYS_ADC_DATA_0_OE_S  13
/** HP_SYS_ADC_CK_DATA_IE : R/W; bitpos: [14]; default: 0;
 *  reserved
 */
#define HP_SYS_ADC_CK_DATA_IE    (BIT(14))
#define HP_SYS_ADC_CK_DATA_IE_M  (HP_SYS_ADC_CK_DATA_IE_V << HP_SYS_ADC_CK_DATA_IE_S)
#define HP_SYS_ADC_CK_DATA_IE_V  0x00000001U
#define HP_SYS_ADC_CK_DATA_IE_S  14
/** HP_SYS_ADC_CK_DATA_OE : R/W; bitpos: [15]; default: 1;
 *  reserved
 */
#define HP_SYS_ADC_CK_DATA_OE    (BIT(15))
#define HP_SYS_ADC_CK_DATA_OE_M  (HP_SYS_ADC_CK_DATA_OE_V << HP_SYS_ADC_CK_DATA_OE_S)
#define HP_SYS_ADC_CK_DATA_OE_V  0x00000001U
#define HP_SYS_ADC_CK_DATA_OE_S  15

/** HP_SYS_DATE_REG register
 *  Date register.
 */
#define HP_SYS_DATE_REG (DR_REG_HP_SYS_BASE + 0x3fc)
/** HP_SYS_DATE : R/W; bitpos: [27:0]; default: 36720768;
 *  HP-SYSTEM date information/ HP-SYSTEM version information.
 */
#define HP_SYS_DATE    0x0FFFFFFFU
#define HP_SYS_DATE_M  (HP_SYS_DATE_V << HP_SYS_DATE_S)
#define HP_SYS_DATE_V  0x0FFFFFFFU
#define HP_SYS_DATE_S  0

#ifdef __cplusplus
}
#endif
