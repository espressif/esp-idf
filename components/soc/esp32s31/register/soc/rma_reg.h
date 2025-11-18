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

/** RMA_CONFIG_REG register
 *  Configures RMA algorithm
 */
#define RMA_CONFIG_REG (DR_REG_RMA_BASE + 0x0)
/** RMA_CONFIG_WORK_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Configures the RMA module.
 *  00: REQUEST CERTIFICATE
 *  01: VERIFICATION CERTIFICATE
 *  10: FAST VERIFICATION
 */
#define RMA_CONFIG_WORK_MODE    0x00000003U
#define RMA_CONFIG_WORK_MODE_M  (RMA_CONFIG_WORK_MODE_V << RMA_CONFIG_WORK_MODE_S)
#define RMA_CONFIG_WORK_MODE_V  0x00000003U
#define RMA_CONFIG_WORK_MODE_S  0
/** RMA_CONFIG_REUSE_JTAG : R/W; bitpos: [3]; default: 0;
 *  Whether enable JTAG after RMA pass.
 *  0: Do not enable JTAG
 *  1: Enable JTAG
 */
#define RMA_CONFIG_REUSE_JTAG    (BIT(3))
#define RMA_CONFIG_REUSE_JTAG_M  (RMA_CONFIG_REUSE_JTAG_V << RMA_CONFIG_REUSE_JTAG_S)
#define RMA_CONFIG_REUSE_JTAG_V  0x00000001U
#define RMA_CONFIG_REUSE_JTAG_S  3
/** RMA_CONFIG_REUSE_DOWNLOAD : R/W; bitpos: [4]; default: 0;
 *  Whether enable DOWNLOAD_MODE after RMA pass.
 *  0: Do not enable DOWNLOAD mode
 *  1: Enable DOWNLOAD
 */
#define RMA_CONFIG_REUSE_DOWNLOAD    (BIT(4))
#define RMA_CONFIG_REUSE_DOWNLOAD_M  (RMA_CONFIG_REUSE_DOWNLOAD_V << RMA_CONFIG_REUSE_DOWNLOAD_S)
#define RMA_CONFIG_REUSE_DOWNLOAD_V  0x00000001U
#define RMA_CONFIG_REUSE_DOWNLOAD_S  4
/** RMA_CONFIG_FORCE_SPI : R/W; bitpos: [5]; default: 0;
 *  Whether enable FORCE_SPI_BOOT after RMA pass.
 *  0: Do not enable FORCE_SPI_BOOT mode
 *  1: Enable FORCE_SPI_BOOT
 */
#define RMA_CONFIG_FORCE_SPI    (BIT(5))
#define RMA_CONFIG_FORCE_SPI_M  (RMA_CONFIG_FORCE_SPI_V << RMA_CONFIG_FORCE_SPI_S)
#define RMA_CONFIG_FORCE_SPI_V  0x00000001U
#define RMA_CONFIG_FORCE_SPI_S  5
/** RMA_CONFIG_USE_NONCE : R/W; bitpos: [6]; default: 0;
 *  Whether use  NONCE.
 *  0: Do not use  NONCE
 *  1: Use  NONCE
 */
#define RMA_CONFIG_USE_NONCE    (BIT(6))
#define RMA_CONFIG_USE_NONCE_M  (RMA_CONFIG_USE_NONCE_V << RMA_CONFIG_USE_NONCE_S)
#define RMA_CONFIG_USE_NONCE_V  0x00000001U
#define RMA_CONFIG_USE_NONCE_S  6
/** RMA_CONFIG_USE_KM : R/W; bitpos: [7]; default: 0;
 *  Whether use km to generate NONCE.
 *  0: Do not use KM to generate NONCE
 *  1: Use KM to generate NONCE
 */
#define RMA_CONFIG_USE_KM    (BIT(7))
#define RMA_CONFIG_USE_KM_M  (RMA_CONFIG_USE_KM_V << RMA_CONFIG_USE_KM_S)
#define RMA_CONFIG_USE_KM_V  0x00000001U
#define RMA_CONFIG_USE_KM_S  7
/** RMA_CONFIG_USC_BLOCK_NUM : R/W; bitpos: [14:8]; default: 0;
 *  The length of USC.
 */
#define RMA_CONFIG_USC_BLOCK_NUM    0x0000007FU
#define RMA_CONFIG_USC_BLOCK_NUM_M  (RMA_CONFIG_USC_BLOCK_NUM_V << RMA_CONFIG_USC_BLOCK_NUM_S)
#define RMA_CONFIG_USC_BLOCK_NUM_V  0x0000007FU
#define RMA_CONFIG_USC_BLOCK_NUM_S  8

/** RMA_TRIGGER_REG register
 *  Starts the RMA module.
 */
#define RMA_TRIGGER_REG (DR_REG_RMA_BASE + 0x4)
/** RMA_TRIGGER_START : WO; bitpos: [0]; default: 0;
 *  Start trigger typical rma.
 */
#define RMA_TRIGGER_START    (BIT(0))
#define RMA_TRIGGER_START_M  (RMA_TRIGGER_START_V << RMA_TRIGGER_START_S)
#define RMA_TRIGGER_START_V  0x00000001U
#define RMA_TRIGGER_START_S  0
/** RMA_TRIGGER_LOAD : WO; bitpos: [1]; default: 0;
 *  Rma load done.
 */
#define RMA_TRIGGER_LOAD    (BIT(1))
#define RMA_TRIGGER_LOAD_M  (RMA_TRIGGER_LOAD_V << RMA_TRIGGER_LOAD_S)
#define RMA_TRIGGER_LOAD_V  0x00000001U
#define RMA_TRIGGER_LOAD_S  1
/** RMA_TRIGGER_GAIN : WO; bitpos: [2]; default: 0;
 *  Rma gain done.
 */
#define RMA_TRIGGER_GAIN    (BIT(2))
#define RMA_TRIGGER_GAIN_M  (RMA_TRIGGER_GAIN_V << RMA_TRIGGER_GAIN_S)
#define RMA_TRIGGER_GAIN_V  0x00000001U
#define RMA_TRIGGER_GAIN_S  2
/** RMA_TRIGGER_USC : WO; bitpos: [3]; default: 0;
 *  Rma usc done.
 */
#define RMA_TRIGGER_USC    (BIT(3))
#define RMA_TRIGGER_USC_M  (RMA_TRIGGER_USC_V << RMA_TRIGGER_USC_S)
#define RMA_TRIGGER_USC_V  0x00000001U
#define RMA_TRIGGER_USC_S  3

/** RMA_STATE_REG register
 *  query state in rma
 */
#define RMA_STATE_REG (DR_REG_RMA_BASE + 0x8)
/** RMA_STATE : RO; bitpos: [2:0]; default: 0;
 *  The status bits of RMA Accelerator. RMA is at 0: IDLE, 1: LOAD, 2: GAIN, 3: USC, 4:
 *  LOCK, others: BUSY state.
 */
#define RMA_STATE    0x00000007U
#define RMA_STATE_M  (RMA_STATE_V << RMA_STATE_S)
#define RMA_STATE_V  0x00000007U
#define RMA_STATE_S  0

/** RMA_LOG_REG register
 *  Query result in rma
 */
#define RMA_LOG_REG (DR_REG_RMA_BASE + 0xc)
/** RMA_LOG_CERT_STATE : RO; bitpos: [0]; default: 0;
 *  Marks state of rma cert verification result.
 *  1: pass
 *  0: failed
 */
#define RMA_LOG_CERT_STATE    (BIT(0))
#define RMA_LOG_CERT_STATE_M  (RMA_LOG_CERT_STATE_V << RMA_LOG_CERT_STATE_S)
#define RMA_LOG_CERT_STATE_V  0x00000001U
#define RMA_LOG_CERT_STATE_S  0
/** RMA_LOG_PUBLIC_KEY_STATE : RO; bitpos: [1]; default: 0;
 *  Marks state of rma public key verification result.
 *  1: pass
 *  0: failed
 */
#define RMA_LOG_PUBLIC_KEY_STATE    (BIT(1))
#define RMA_LOG_PUBLIC_KEY_STATE_M  (RMA_LOG_PUBLIC_KEY_STATE_V << RMA_LOG_PUBLIC_KEY_STATE_S)
#define RMA_LOG_PUBLIC_KEY_STATE_V  0x00000001U
#define RMA_LOG_PUBLIC_KEY_STATE_S  1
/** RMA_LOG_SIGN_STATE : RO; bitpos: [2]; default: 0;
 *  Marks state of rma sign verification result.
 *  1: pass
 *  0: failed
 */
#define RMA_LOG_SIGN_STATE    (BIT(2))
#define RMA_LOG_SIGN_STATE_M  (RMA_LOG_SIGN_STATE_V << RMA_LOG_SIGN_STATE_S)
#define RMA_LOG_SIGN_STATE_V  0x00000001U
#define RMA_LOG_SIGN_STATE_S  2
/** RMA_LOG_NONCE_STATE : RO; bitpos: [28]; default: 0;
 *  Marks error nonce config .
 *  0 : Use efuse_hash_sg in CERT_REQ or FAST_VEF but key invalid
 *  1 : Nonce config pass.
 */
#define RMA_LOG_NONCE_STATE    (BIT(28))
#define RMA_LOG_NONCE_STATE_M  (RMA_LOG_NONCE_STATE_V << RMA_LOG_NONCE_STATE_S)
#define RMA_LOG_NONCE_STATE_V  0x00000001U
#define RMA_LOG_NONCE_STATE_S  28
/** RMA_LOG_KM_STATE : RO; bitpos: [29]; default: 0;
 *  Marks error KM config .
 *  0 : Use km but km key invalid
 *  1 : KM config pass
 */
#define RMA_LOG_KM_STATE    (BIT(29))
#define RMA_LOG_KM_STATE_M  (RMA_LOG_KM_STATE_V << RMA_LOG_KM_STATE_S)
#define RMA_LOG_KM_STATE_V  0x00000001U
#define RMA_LOG_KM_STATE_S  29
/** RMA_LOG_MODE_STATE : RO; bitpos: [30]; default: 0;
 *  Marks error mode config .
 *  0 : Invalid mode config
 *  1  : MODE config pass
 */
#define RMA_LOG_MODE_STATE    (BIT(30))
#define RMA_LOG_MODE_STATE_M  (RMA_LOG_MODE_STATE_V << RMA_LOG_MODE_STATE_S)
#define RMA_LOG_MODE_STATE_V  0x00000001U
#define RMA_LOG_MODE_STATE_S  30
/** RMA_LOG_EFUSE_STATE : RO; bitpos: [31]; default: 0;
 *  Marks efuse state .
 *  0 : Efuse disable RMA module
 *  1  : Efuse pass
 */
#define RMA_LOG_EFUSE_STATE    (BIT(31))
#define RMA_LOG_EFUSE_STATE_M  (RMA_LOG_EFUSE_STATE_V << RMA_LOG_EFUSE_STATE_S)
#define RMA_LOG_EFUSE_STATE_V  0x00000001U
#define RMA_LOG_EFUSE_STATE_S  31

/** RMA_INT_RAW_REG register
 *  RMA interrupt raw register, valid in level.
 */
#define RMA_INT_RAW_REG (DR_REG_RMA_BASE + 0x10)
/** RMA_PROC_DONE_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status bit  for the rma_proc_done_int interrupt
 */
#define RMA_PROC_DONE_INT_RAW    (BIT(0))
#define RMA_PROC_DONE_INT_RAW_M  (RMA_PROC_DONE_INT_RAW_V << RMA_PROC_DONE_INT_RAW_S)
#define RMA_PROC_DONE_INT_RAW_V  0x00000001U
#define RMA_PROC_DONE_INT_RAW_S  0

/** RMA_INT_ST_REG register
 *  RMA interrupt status register.
 */
#define RMA_INT_ST_REG (DR_REG_RMA_BASE + 0x14)
/** RMA_PROC_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit  for the rma_proc_done_int interrupt
 */
#define RMA_PROC_DONE_INT_ST    (BIT(0))
#define RMA_PROC_DONE_INT_ST_M  (RMA_PROC_DONE_INT_ST_V << RMA_PROC_DONE_INT_ST_S)
#define RMA_PROC_DONE_INT_ST_V  0x00000001U
#define RMA_PROC_DONE_INT_ST_S  0

/** RMA_INT_ENA_REG register
 *  RMA interrupt enable register.
 */
#define RMA_INT_ENA_REG (DR_REG_RMA_BASE + 0x18)
/** RMA_PROC_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit  for the rma_proc_done_int interrupt
 */
#define RMA_PROC_DONE_INT_ENA    (BIT(0))
#define RMA_PROC_DONE_INT_ENA_M  (RMA_PROC_DONE_INT_ENA_V << RMA_PROC_DONE_INT_ENA_S)
#define RMA_PROC_DONE_INT_ENA_V  0x00000001U
#define RMA_PROC_DONE_INT_ENA_S  0

/** RMA_INT_CLR_REG register
 *  RMA interrupt clear register.
 */
#define RMA_INT_CLR_REG (DR_REG_RMA_BASE + 0x1c)
/** RMA_PROC_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the rma_proc_done_int interrupt
 */
#define RMA_PROC_DONE_INT_CLR    (BIT(0))
#define RMA_PROC_DONE_INT_CLR_M  (RMA_PROC_DONE_INT_CLR_V << RMA_PROC_DONE_INT_CLR_S)
#define RMA_PROC_DONE_INT_CLR_V  0x00000001U
#define RMA_PROC_DONE_INT_CLR_S  0

/** RMA_RESULT_ST_REG register
 *  RMA result reg.
 */
#define RMA_RESULT_ST_REG (DR_REG_RMA_BASE + 0x20)
/** RMA_REUSE_JTAG_ST : RO/WTC/SS; bitpos: [0]; default: 0;
 *  The state of reuse jtag register.
 */
#define RMA_REUSE_JTAG_ST    (BIT(0))
#define RMA_REUSE_JTAG_ST_M  (RMA_REUSE_JTAG_ST_V << RMA_REUSE_JTAG_ST_S)
#define RMA_REUSE_JTAG_ST_V  0x00000001U
#define RMA_REUSE_JTAG_ST_S  0
/** RMA_REUSE_DOWNLOAD_ST : RO/WTC/SS; bitpos: [1]; default: 0;
 *  The state of reuse download register.
 */
#define RMA_REUSE_DOWNLOAD_ST    (BIT(1))
#define RMA_REUSE_DOWNLOAD_ST_M  (RMA_REUSE_DOWNLOAD_ST_V << RMA_REUSE_DOWNLOAD_ST_S)
#define RMA_REUSE_DOWNLOAD_ST_V  0x00000001U
#define RMA_REUSE_DOWNLOAD_ST_S  1
/** RMA_FORCE_SPI_BOOT_ST : RO/WTC/SS; bitpos: [2]; default: 0;
 *  The state of force spi boot reg register.
 */
#define RMA_FORCE_SPI_BOOT_ST    (BIT(2))
#define RMA_FORCE_SPI_BOOT_ST_M  (RMA_FORCE_SPI_BOOT_ST_V << RMA_FORCE_SPI_BOOT_ST_S)
#define RMA_FORCE_SPI_BOOT_ST_V  0x00000001U
#define RMA_FORCE_SPI_BOOT_ST_S  2

/** RMA_RESULT_CLR_REG register
 *  RMA clr result reg.
 */
#define RMA_RESULT_CLR_REG (DR_REG_RMA_BASE + 0x24)
/** RMA_REUSE_JTAG_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the reuse jtag state.
 */
#define RMA_REUSE_JTAG_CLR    (BIT(0))
#define RMA_REUSE_JTAG_CLR_M  (RMA_REUSE_JTAG_CLR_V << RMA_REUSE_JTAG_CLR_S)
#define RMA_REUSE_JTAG_CLR_V  0x00000001U
#define RMA_REUSE_JTAG_CLR_S  0
/** RMA_REUSE_DOWNLOAD_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the reuse download state.
 */
#define RMA_REUSE_DOWNLOAD_CLR    (BIT(1))
#define RMA_REUSE_DOWNLOAD_CLR_M  (RMA_REUSE_DOWNLOAD_CLR_V << RMA_REUSE_DOWNLOAD_CLR_S)
#define RMA_REUSE_DOWNLOAD_CLR_V  0x00000001U
#define RMA_REUSE_DOWNLOAD_CLR_S  1
/** RMA_FORCE_SPI_BOOT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the force spi boot state.
 */
#define RMA_FORCE_SPI_BOOT_CLR    (BIT(2))
#define RMA_FORCE_SPI_BOOT_CLR_M  (RMA_FORCE_SPI_BOOT_CLR_V << RMA_FORCE_SPI_BOOT_CLR_S)
#define RMA_FORCE_SPI_BOOT_CLR_V  0x00000001U
#define RMA_FORCE_SPI_BOOT_CLR_S  2

/** RMA_TIMEOUT_LIMIT_REG register
 *  RMA timeout limit configure register
 */
#define RMA_TIMEOUT_LIMIT_REG (DR_REG_RMA_BASE + 0x28)
/** RMA_TIMEOUT_LIMIT : R/W; bitpos: [31:0]; default: 4294967295;
 *  The timeout limitation for waiting sub-IP.
 */
#define RMA_TIMEOUT_LIMIT    0xFFFFFFFFU
#define RMA_TIMEOUT_LIMIT_M  (RMA_TIMEOUT_LIMIT_V << RMA_TIMEOUT_LIMIT_S)
#define RMA_TIMEOUT_LIMIT_V  0xFFFFFFFFU
#define RMA_TIMEOUT_LIMIT_S  0

/** RMA_DATE_REG register
 *  Version control register
 */
#define RMA_DATE_REG (DR_REG_RMA_BASE + 0x7c)
/** RMA_DATE : R/W; bitpos: [31:0]; default: 539297817;
 *  Version control register.
 */
#define RMA_DATE    0xFFFFFFFFU
#define RMA_DATE_M  (RMA_DATE_V << RMA_DATE_S)
#define RMA_DATE_V  0xFFFFFFFFU
#define RMA_DATE_S  0

/** RMA_CHIP_INFO_SOURCE_MEM register
 *  RMA chip info memory.
 */
#define RMA_CHIP_INFO_SOURCE_MEM (DR_REG_RMA_BASE + 0x80)
#define RMA_CHIP_INFO_SOURCE_MEM_SIZE_BYTES 4

/** RMA_NONCE_MEM register
 *  RMA nonce memory.
 */
#define RMA_NONCE_MEM (DR_REG_RMA_BASE + 0x84)
#define RMA_NONCE_MEM_SIZE_BYTES 4

/** RMA_USC_MEM register
 *  RMA USC memory.
 */
#define RMA_USC_MEM (DR_REG_RMA_BASE + 0x88)
#define RMA_USC_MEM_SIZE_BYTES 4

/** RMA_PUBLIC_KEY_MEM register
 *  RMA public key memory .
 */
#define RMA_PUBLIC_KEY_MEM (DR_REG_RMA_BASE + 0x8c)
#define RMA_PUBLIC_KEY_MEM_SIZE_BYTES 4

/** RMA_SIGN_MEM register
 *  RMA signature r memory .
 */
#define RMA_SIGN_MEM (DR_REG_RMA_BASE + 0x90)
#define RMA_SIGN_MEM_SIZE_BYTES 4

/** RMA_CERT_HASH_MEM register
 *  RMA cert hash memory.
 */
#define RMA_CERT_HASH_MEM (DR_REG_RMA_BASE + 0x94)
#define RMA_CERT_HASH_MEM_SIZE_BYTES 4

/** RMA_CHIP_INFO_MEM register
 *  RMA chip info memory .
 */
#define RMA_CHIP_INFO_MEM (DR_REG_RMA_BASE + 0x98)
#define RMA_CHIP_INFO_MEM_SIZE_BYTES 4

#ifdef __cplusplus
}
#endif
