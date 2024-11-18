/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
 *  EXTERNAL DEVICE ENCRYPTION/DECRYPTION configuration register
 */
#define HP_SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG (DR_REG_HP_SYSTEM_BASE + 0x0)
/** HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT : R/W; bitpos: [0]; default: 0;
 *  Set this bit as 1 to enable mspi xts manual encrypt in spi boot mode.
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
 *  Set this bit as 1 to enable mspi xts auto decrypt in download boot mode.
 */
#define HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT    (BIT(2))
#define HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_M  (HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_V << HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_S)
#define HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_V  0x00000001U
#define HP_SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_S  2
/** HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT : R/W; bitpos: [3]; default: 0;
 *  Set this bit as 1 to enable mspi xts manual encrypt in download boot mode.
 */
#define HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT    (BIT(3))
#define HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_M  (HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_V << HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_S)
#define HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_V  0x00000001U
#define HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_S  3

/** HP_SYSTEM_SRAM_USAGE_CONF_REG register
 *  HP memory usage configuration register
 */
#define HP_SYSTEM_SRAM_USAGE_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x4)
/** HP_SYSTEM_CACHE_USAGE : HRO; bitpos: [0]; default: 0;
 *  reserved
 */
#define HP_SYSTEM_CACHE_USAGE    (BIT(0))
#define HP_SYSTEM_CACHE_USAGE_M  (HP_SYSTEM_CACHE_USAGE_V << HP_SYSTEM_CACHE_USAGE_S)
#define HP_SYSTEM_CACHE_USAGE_V  0x00000001U
#define HP_SYSTEM_CACHE_USAGE_S  0
/** HP_SYSTEM_SRAM_USAGE : R/W; bitpos: [11:8]; default: 0;
 *  0: cpu use hp-memory. 1:mac-dump accessing hp-memory.
 */
#define HP_SYSTEM_SRAM_USAGE    0x0000000FU
#define HP_SYSTEM_SRAM_USAGE_M  (HP_SYSTEM_SRAM_USAGE_V << HP_SYSTEM_SRAM_USAGE_S)
#define HP_SYSTEM_SRAM_USAGE_V  0x0000000FU
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
 *  0: anti-DPA disable. 1~3: anti-DPA enable with different security level. The larger
 *  the number, the stronger the ability to resist DPA attacks and the higher the
 *  security level, but it will increase the computational overhead of the hardware
 *  crypto-accelerators. Only available if HP_SYSTEM_SEC_DPA_CFG_SEL is 0.
 */
#define HP_SYSTEM_SEC_DPA_LEVEL    0x00000003U
#define HP_SYSTEM_SEC_DPA_LEVEL_M  (HP_SYSTEM_SEC_DPA_LEVEL_V << HP_SYSTEM_SEC_DPA_LEVEL_S)
#define HP_SYSTEM_SEC_DPA_LEVEL_V  0x00000003U
#define HP_SYSTEM_SEC_DPA_LEVEL_S  0
/** HP_SYSTEM_SEC_DPA_CFG_SEL : R/W; bitpos: [2]; default: 0;
 *  This field is used to select either HP_SYSTEM_SEC_DPA_LEVEL or EFUSE_SEC_DPA_LEVEL
 *  (from efuse) to control dpa_level. 0: EFUSE_SEC_DPA_LEVEL, 1: HP_SYSTEM_SEC_DPA_LEVEL.
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
 *  Set the timeout threshold for bus access, corresponding to the number of clock
 *  cycles of the clock domain.
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_THRES    0x0000FFFFU
#define HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_M  (HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_V << HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_S)
#define HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_V  0x0000FFFFU
#define HP_SYSTEM_CPU_PERI_TIMEOUT_THRES_S  0
/** HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR : WT; bitpos: [16]; default: 0;
 *  Set this bit as 1 to clear timeout interrupt
 */
#define HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR    (BIT(16))
#define HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_M  (HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_V << HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_S)
#define HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_V  0x00000001U
#define HP_SYSTEM_CPU_PERI_TIMEOUT_INT_CLEAR_S  16
/** HP_SYSTEM_CPU_PERI_TIMEOUT_PROTECT_EN : R/W; bitpos: [17]; default: 1;
 *  Set this bit as 1 to enable timeout protection for accessing cpu peripheral
 *  registers
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
 *  Record the address information of abnormal access
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
 *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
 *  will be cleared after the interrupt is cleared.
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
 *  Set the timeout threshold for bus access, corresponding to the number of clock
 *  cycles of the clock domain.
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_THRES    0x0000FFFFU
#define HP_SYSTEM_HP_PERI_TIMEOUT_THRES_M  (HP_SYSTEM_HP_PERI_TIMEOUT_THRES_V << HP_SYSTEM_HP_PERI_TIMEOUT_THRES_S)
#define HP_SYSTEM_HP_PERI_TIMEOUT_THRES_V  0x0000FFFFU
#define HP_SYSTEM_HP_PERI_TIMEOUT_THRES_S  0
/** HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR : WT; bitpos: [16]; default: 0;
 *  Set this bit as 1 to clear timeout interrupt
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR    (BIT(16))
#define HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_M  (HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_V << HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_S)
#define HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_V  0x00000001U
#define HP_SYSTEM_HP_PERI_TIMEOUT_INT_CLEAR_S  16
/** HP_SYSTEM_HP_PERI_TIMEOUT_PROTECT_EN : R/W; bitpos: [17]; default: 1;
 *  Set this bit as 1 to enable timeout protection for accessing hp peripheral registers
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
 *  Record the address information of abnormal access
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
 *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
 *  will be cleared after the interrupt is cleared.
 */
#define HP_SYSTEM_HP_PERI_TIMEOUT_UID    0x0000007FU
#define HP_SYSTEM_HP_PERI_TIMEOUT_UID_M  (HP_SYSTEM_HP_PERI_TIMEOUT_UID_V << HP_SYSTEM_HP_PERI_TIMEOUT_UID_S)
#define HP_SYSTEM_HP_PERI_TIMEOUT_UID_V  0x0000007FU
#define HP_SYSTEM_HP_PERI_TIMEOUT_UID_S  0

/** HP_SYSTEM_MODEM_PERI_TIMEOUT_CONF_REG register
 *  MODEM_PERI_TIMEOUT configuration register
 */
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x24)
/** HP_SYSTEM_MODEM_PERI_TIMEOUT_THRES : R/W; bitpos: [15:0]; default: 65535;
 *  Set the timeout threshold for bus access, corresponding to the number of clock
 *  cycles of the clock domain.
 */
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_THRES    0x0000FFFFU
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_THRES_M  (HP_SYSTEM_MODEM_PERI_TIMEOUT_THRES_V << HP_SYSTEM_MODEM_PERI_TIMEOUT_THRES_S)
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_THRES_V  0x0000FFFFU
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_THRES_S  0
/** HP_SYSTEM_MODEM_PERI_TIMEOUT_INT_CLEAR : WT; bitpos: [16]; default: 0;
 *  Set this bit as 1 to clear timeout interrupt
 */
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_INT_CLEAR    (BIT(16))
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_INT_CLEAR_M  (HP_SYSTEM_MODEM_PERI_TIMEOUT_INT_CLEAR_V << HP_SYSTEM_MODEM_PERI_TIMEOUT_INT_CLEAR_S)
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_INT_CLEAR_V  0x00000001U
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_INT_CLEAR_S  16
/** HP_SYSTEM_MODEM_PERI_TIMEOUT_PROTECT_EN : R/W; bitpos: [17]; default: 1;
 *  Set this bit as 1 to enable timeout protection for accessing modem registers
 */
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_PROTECT_EN    (BIT(17))
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_PROTECT_EN_M  (HP_SYSTEM_MODEM_PERI_TIMEOUT_PROTECT_EN_V << HP_SYSTEM_MODEM_PERI_TIMEOUT_PROTECT_EN_S)
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_PROTECT_EN_V  0x00000001U
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_PROTECT_EN_S  17

/** HP_SYSTEM_MODEM_PERI_TIMEOUT_ADDR_REG register
 *  MODEM_PERI_TIMEOUT_ADDR register
 */
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_ADDR_REG (DR_REG_HP_SYSTEM_BASE + 0x28)
/** HP_SYSTEM_MODEM_PERI_TIMEOUT_ADDR : RO; bitpos: [31:0]; default: 0;
 *  Record the address information of abnormal access
 */
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_ADDR    0xFFFFFFFFU
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_ADDR_M  (HP_SYSTEM_MODEM_PERI_TIMEOUT_ADDR_V << HP_SYSTEM_MODEM_PERI_TIMEOUT_ADDR_S)
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_ADDR_V  0xFFFFFFFFU
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_ADDR_S  0

/** HP_SYSTEM_MODEM_PERI_TIMEOUT_UID_REG register
 *  MODEM_PERI_TIMEOUT_UID register
 */
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_UID_REG (DR_REG_HP_SYSTEM_BASE + 0x2c)
/** HP_SYSTEM_MODEM_PERI_TIMEOUT_UID : RO; bitpos: [6:0]; default: 0;
 *  Record master id[4:0] & master permission[6:5] when trigger timeout. This register
 *  will be cleared after the interrupt is cleared.
 */
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_UID    0x0000007FU
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_UID_M  (HP_SYSTEM_MODEM_PERI_TIMEOUT_UID_V << HP_SYSTEM_MODEM_PERI_TIMEOUT_UID_S)
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_UID_V  0x0000007FU
#define HP_SYSTEM_MODEM_PERI_TIMEOUT_UID_S  0

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

/** HP_SYSTEM_RETENTION_CONF_REG register
 *  Retention configuration register
 */
#define HP_SYSTEM_RETENTION_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x34)
/** HP_SYSTEM_RETENTION_DISABLE : R/W; bitpos: [0]; default: 0;
 *  Set this bit as 1 to disable retention function. Not disable by default.
 */
#define HP_SYSTEM_RETENTION_DISABLE    (BIT(0))
#define HP_SYSTEM_RETENTION_DISABLE_M  (HP_SYSTEM_RETENTION_DISABLE_V << HP_SYSTEM_RETENTION_DISABLE_S)
#define HP_SYSTEM_RETENTION_DISABLE_V  0x00000001U
#define HP_SYSTEM_RETENTION_DISABLE_S  0

/** HP_SYSTEM_ROM_TABLE_LOCK_REG register
 *  Rom-Table lock register
 */
#define HP_SYSTEM_ROM_TABLE_LOCK_REG (DR_REG_HP_SYSTEM_BASE + 0x38)
/** HP_SYSTEM_ROM_TABLE_LOCK : R/W; bitpos: [0]; default: 0;
 *  XXXX
 */
#define HP_SYSTEM_ROM_TABLE_LOCK    (BIT(0))
#define HP_SYSTEM_ROM_TABLE_LOCK_M  (HP_SYSTEM_ROM_TABLE_LOCK_V << HP_SYSTEM_ROM_TABLE_LOCK_S)
#define HP_SYSTEM_ROM_TABLE_LOCK_V  0x00000001U
#define HP_SYSTEM_ROM_TABLE_LOCK_S  0

/** HP_SYSTEM_ROM_TABLE_REG register
 *  Rom-Table register
 */
#define HP_SYSTEM_ROM_TABLE_REG (DR_REG_HP_SYSTEM_BASE + 0x3c)
/** HP_SYSTEM_ROM_TABLE : R/W; bitpos: [31:0]; default: 0;
 *  XXXX
 */
#define HP_SYSTEM_ROM_TABLE    0xFFFFFFFFU
#define HP_SYSTEM_ROM_TABLE_M  (HP_SYSTEM_ROM_TABLE_V << HP_SYSTEM_ROM_TABLE_S)
#define HP_SYSTEM_ROM_TABLE_V  0xFFFFFFFFU
#define HP_SYSTEM_ROM_TABLE_S  0

/** HP_SYSTEM_CORE_DEBUG_RUNSTALL_CONF_REG register
 *  Core Debug runstall configure register
 */
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x40)
/** HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE : R/W; bitpos: [0]; default: 0;
 *  Set this field to 1 to enable debug runstall feature between HP-core and LP-core.
 */
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE    (BIT(0))
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_M  (HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_V << HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_S)
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_V  0x00000001U
#define HP_SYSTEM_CORE_DEBUG_RUNSTALL_ENABLE_S  0

/** HP_SYSTEM_MEM_TEST_CONF_REG register
 *  MEM_TEST configuration register
 */
#define HP_SYSTEM_MEM_TEST_CONF_REG (DR_REG_HP_SYSTEM_BASE + 0x44)
/** HP_SYSTEM_HP_MEM_WPULSE : R/W; bitpos: [2:0]; default: 0;
 *  This field controls hp system memory WPULSE parameter.
 */
#define HP_SYSTEM_HP_MEM_WPULSE    0x00000007U
#define HP_SYSTEM_HP_MEM_WPULSE_M  (HP_SYSTEM_HP_MEM_WPULSE_V << HP_SYSTEM_HP_MEM_WPULSE_S)
#define HP_SYSTEM_HP_MEM_WPULSE_V  0x00000007U
#define HP_SYSTEM_HP_MEM_WPULSE_S  0
/** HP_SYSTEM_HP_MEM_WA : R/W; bitpos: [5:3]; default: 4;
 *  This field controls hp system memory WA parameter.
 */
#define HP_SYSTEM_HP_MEM_WA    0x00000007U
#define HP_SYSTEM_HP_MEM_WA_M  (HP_SYSTEM_HP_MEM_WA_V << HP_SYSTEM_HP_MEM_WA_S)
#define HP_SYSTEM_HP_MEM_WA_V  0x00000007U
#define HP_SYSTEM_HP_MEM_WA_S  3
/** HP_SYSTEM_HP_MEM_RA : R/W; bitpos: [7:6]; default: 0;
 *  This field controls hp system memory RA parameter.
 */
#define HP_SYSTEM_HP_MEM_RA    0x00000003U
#define HP_SYSTEM_HP_MEM_RA_M  (HP_SYSTEM_HP_MEM_RA_V << HP_SYSTEM_HP_MEM_RA_S)
#define HP_SYSTEM_HP_MEM_RA_V  0x00000003U
#define HP_SYSTEM_HP_MEM_RA_S  6

/** HP_SYSTEM_RND_ECO_REG register
 *  redcy eco register.
 */
#define HP_SYSTEM_RND_ECO_REG (DR_REG_HP_SYSTEM_BASE + 0x3e0)
/** HP_SYSTEM_REDCY_ENA : W/R; bitpos: [0]; default: 0;
 *  Only reserved for ECO.
 */
#define HP_SYSTEM_REDCY_ENA    (BIT(0))
#define HP_SYSTEM_REDCY_ENA_M  (HP_SYSTEM_REDCY_ENA_V << HP_SYSTEM_REDCY_ENA_S)
#define HP_SYSTEM_REDCY_ENA_V  0x00000001U
#define HP_SYSTEM_REDCY_ENA_S  0
/** HP_SYSTEM_REDCY_RESULT : RO; bitpos: [1]; default: 0;
 *  Only reserved for ECO.
 */
#define HP_SYSTEM_REDCY_RESULT    (BIT(1))
#define HP_SYSTEM_REDCY_RESULT_M  (HP_SYSTEM_REDCY_RESULT_V << HP_SYSTEM_REDCY_RESULT_S)
#define HP_SYSTEM_REDCY_RESULT_V  0x00000001U
#define HP_SYSTEM_REDCY_RESULT_S  1

/** HP_SYSTEM_RND_ECO_LOW_REG register
 *  redcy eco low register.
 */
#define HP_SYSTEM_RND_ECO_LOW_REG (DR_REG_HP_SYSTEM_BASE + 0x3e4)
/** HP_SYSTEM_REDCY_LOW : W/R; bitpos: [31:0]; default: 0;
 *  Only reserved for ECO.
 */
#define HP_SYSTEM_REDCY_LOW    0xFFFFFFFFU
#define HP_SYSTEM_REDCY_LOW_M  (HP_SYSTEM_REDCY_LOW_V << HP_SYSTEM_REDCY_LOW_S)
#define HP_SYSTEM_REDCY_LOW_V  0xFFFFFFFFU
#define HP_SYSTEM_REDCY_LOW_S  0

/** HP_SYSTEM_RND_ECO_HIGH_REG register
 *  redcy eco high register.
 */
#define HP_SYSTEM_RND_ECO_HIGH_REG (DR_REG_HP_SYSTEM_BASE + 0x3e8)
/** HP_SYSTEM_REDCY_HIGH : W/R; bitpos: [31:0]; default: 4294967295;
 *  Only reserved for ECO.
 */
#define HP_SYSTEM_REDCY_HIGH    0xFFFFFFFFU
#define HP_SYSTEM_REDCY_HIGH_M  (HP_SYSTEM_REDCY_HIGH_V << HP_SYSTEM_REDCY_HIGH_S)
#define HP_SYSTEM_REDCY_HIGH_V  0xFFFFFFFFU
#define HP_SYSTEM_REDCY_HIGH_S  0

/** HP_SYSTEM_CLOCK_GATE_REG register
 *  HP-SYSTEM clock gating configure register
 */
#define HP_SYSTEM_CLOCK_GATE_REG (DR_REG_HP_SYSTEM_BASE + 0x3f8)
/** HP_SYSTEM_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Set this bit as 1 to force on clock gating.
 */
#define HP_SYSTEM_CLK_EN    (BIT(0))
#define HP_SYSTEM_CLK_EN_M  (HP_SYSTEM_CLK_EN_V << HP_SYSTEM_CLK_EN_S)
#define HP_SYSTEM_CLK_EN_V  0x00000001U
#define HP_SYSTEM_CLK_EN_S  0

/** HP_SYSTEM_DATE_REG register
 *  Date register.
 */
#define HP_SYSTEM_DATE_REG (DR_REG_HP_SYSTEM_BASE + 0x3fc)
/** HP_SYSTEM_DATE : R/W; bitpos: [27:0]; default: 35676432;
 *  HP-SYSTEM date information/ HP-SYSTEM version information.
 */
#define HP_SYSTEM_DATE    0x0FFFFFFFU
#define HP_SYSTEM_DATE_M  (HP_SYSTEM_DATE_V << HP_SYSTEM_DATE_S)
#define HP_SYSTEM_DATE_V  0x0FFFFFFFU
#define HP_SYSTEM_DATE_S  0

#ifdef __cplusplus
}
#endif
