/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** KEYMNG_CLK_REG register
 *  Key Manager clock gate control register
 */
#define KEYMNG_CLK_REG (DR_REG_KEYMNG_BASE + 0x4)
/** KEYMNG_REG_CG_FORCE_ON : R/W; bitpos: [0]; default: 1;
 *  Write 1 to force on register clock gate.
 */
#define KEYMNG_REG_CG_FORCE_ON    (BIT(0))
#define KEYMNG_REG_CG_FORCE_ON_M  (KEYMNG_REG_CG_FORCE_ON_V << KEYMNG_REG_CG_FORCE_ON_S)
#define KEYMNG_REG_CG_FORCE_ON_V  0x00000001U
#define KEYMNG_REG_CG_FORCE_ON_S  0
/** KEYMNG_MEM_CG_FORCE_ON : R/W; bitpos: [1]; default: 0;
 *  Write 1 to force on memory clock gate.
 */
#define KEYMNG_MEM_CG_FORCE_ON    (BIT(1))
#define KEYMNG_MEM_CG_FORCE_ON_M  (KEYMNG_MEM_CG_FORCE_ON_V << KEYMNG_MEM_CG_FORCE_ON_S)
#define KEYMNG_MEM_CG_FORCE_ON_V  0x00000001U
#define KEYMNG_MEM_CG_FORCE_ON_S  1

/** KEYMNG_INT_RAW_REG register
 *  Key Manager interrupt raw register, valid in level.
 */
#define KEYMNG_INT_RAW_REG (DR_REG_KEYMNG_BASE + 0x8)
/** KEYMNG_PREP_DONE_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status bit  for the km_prep_done_int interrupt
 */
#define KEYMNG_PREP_DONE_INT_RAW    (BIT(0))
#define KEYMNG_PREP_DONE_INT_RAW_M  (KEYMNG_PREP_DONE_INT_RAW_V << KEYMNG_PREP_DONE_INT_RAW_S)
#define KEYMNG_PREP_DONE_INT_RAW_V  0x00000001U
#define KEYMNG_PREP_DONE_INT_RAW_S  0
/** KEYMNG_PROC_DONE_INT_RAW : RO/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt status bit  for the km_proc_done_int interrupt
 */
#define KEYMNG_PROC_DONE_INT_RAW    (BIT(1))
#define KEYMNG_PROC_DONE_INT_RAW_M  (KEYMNG_PROC_DONE_INT_RAW_V << KEYMNG_PROC_DONE_INT_RAW_S)
#define KEYMNG_PROC_DONE_INT_RAW_V  0x00000001U
#define KEYMNG_PROC_DONE_INT_RAW_S  1
/** KEYMNG_POST_DONE_INT_RAW : RO/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status bit  for the km_post_done_int interrupt
 */
#define KEYMNG_POST_DONE_INT_RAW    (BIT(2))
#define KEYMNG_POST_DONE_INT_RAW_M  (KEYMNG_POST_DONE_INT_RAW_V << KEYMNG_POST_DONE_INT_RAW_S)
#define KEYMNG_POST_DONE_INT_RAW_V  0x00000001U
#define KEYMNG_POST_DONE_INT_RAW_S  2

/** KEYMNG_INT_ST_REG register
 *  Key Manager interrupt status register.
 */
#define KEYMNG_INT_ST_REG (DR_REG_KEYMNG_BASE + 0xc)
/** KEYMNG_PREP_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit  for the km_prep_done_int interrupt
 */
#define KEYMNG_PREP_DONE_INT_ST    (BIT(0))
#define KEYMNG_PREP_DONE_INT_ST_M  (KEYMNG_PREP_DONE_INT_ST_V << KEYMNG_PREP_DONE_INT_ST_S)
#define KEYMNG_PREP_DONE_INT_ST_V  0x00000001U
#define KEYMNG_PREP_DONE_INT_ST_S  0
/** KEYMNG_PROC_DONE_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit  for the km_proc_done_int interrupt
 */
#define KEYMNG_PROC_DONE_INT_ST    (BIT(1))
#define KEYMNG_PROC_DONE_INT_ST_M  (KEYMNG_PROC_DONE_INT_ST_V << KEYMNG_PROC_DONE_INT_ST_S)
#define KEYMNG_PROC_DONE_INT_ST_V  0x00000001U
#define KEYMNG_PROC_DONE_INT_ST_S  1
/** KEYMNG_POST_DONE_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit  for the km_post_done_int interrupt
 */
#define KEYMNG_POST_DONE_INT_ST    (BIT(2))
#define KEYMNG_POST_DONE_INT_ST_M  (KEYMNG_POST_DONE_INT_ST_V << KEYMNG_POST_DONE_INT_ST_S)
#define KEYMNG_POST_DONE_INT_ST_V  0x00000001U
#define KEYMNG_POST_DONE_INT_ST_S  2

/** KEYMNG_INT_ENA_REG register
 *  Key Manager interrupt enable register.
 */
#define KEYMNG_INT_ENA_REG (DR_REG_KEYMNG_BASE + 0x10)
/** KEYMNG_PREP_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit  for the km_prep_done_int interrupt
 */
#define KEYMNG_PREP_DONE_INT_ENA    (BIT(0))
#define KEYMNG_PREP_DONE_INT_ENA_M  (KEYMNG_PREP_DONE_INT_ENA_V << KEYMNG_PREP_DONE_INT_ENA_S)
#define KEYMNG_PREP_DONE_INT_ENA_V  0x00000001U
#define KEYMNG_PREP_DONE_INT_ENA_S  0
/** KEYMNG_PROC_DONE_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit  for the km_proc_done_int interrupt
 */
#define KEYMNG_PROC_DONE_INT_ENA    (BIT(1))
#define KEYMNG_PROC_DONE_INT_ENA_M  (KEYMNG_PROC_DONE_INT_ENA_V << KEYMNG_PROC_DONE_INT_ENA_S)
#define KEYMNG_PROC_DONE_INT_ENA_V  0x00000001U
#define KEYMNG_PROC_DONE_INT_ENA_S  1
/** KEYMNG_POST_DONE_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit  for the km_post_done_int interrupt
 */
#define KEYMNG_POST_DONE_INT_ENA    (BIT(2))
#define KEYMNG_POST_DONE_INT_ENA_M  (KEYMNG_POST_DONE_INT_ENA_V << KEYMNG_POST_DONE_INT_ENA_S)
#define KEYMNG_POST_DONE_INT_ENA_V  0x00000001U
#define KEYMNG_POST_DONE_INT_ENA_S  2

/** KEYMNG_INT_CLR_REG register
 *  Key Manager interrupt clear register.
 */
#define KEYMNG_INT_CLR_REG (DR_REG_KEYMNG_BASE + 0x14)
/** KEYMNG_PREP_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the km_prep_done_int interrupt
 */
#define KEYMNG_PREP_DONE_INT_CLR    (BIT(0))
#define KEYMNG_PREP_DONE_INT_CLR_M  (KEYMNG_PREP_DONE_INT_CLR_V << KEYMNG_PREP_DONE_INT_CLR_S)
#define KEYMNG_PREP_DONE_INT_CLR_V  0x00000001U
#define KEYMNG_PREP_DONE_INT_CLR_S  0
/** KEYMNG_PROC_DONE_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the km_proc_done_int interrupt
 */
#define KEYMNG_PROC_DONE_INT_CLR    (BIT(1))
#define KEYMNG_PROC_DONE_INT_CLR_M  (KEYMNG_PROC_DONE_INT_CLR_V << KEYMNG_PROC_DONE_INT_CLR_S)
#define KEYMNG_PROC_DONE_INT_CLR_V  0x00000001U
#define KEYMNG_PROC_DONE_INT_CLR_S  1
/** KEYMNG_POST_DONE_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the km_post_done_int interrupt
 */
#define KEYMNG_POST_DONE_INT_CLR    (BIT(2))
#define KEYMNG_POST_DONE_INT_CLR_M  (KEYMNG_POST_DONE_INT_CLR_V << KEYMNG_POST_DONE_INT_CLR_S)
#define KEYMNG_POST_DONE_INT_CLR_V  0x00000001U
#define KEYMNG_POST_DONE_INT_CLR_S  2

/** KEYMNG_STATIC_REG register
 *  Key Manager static configuration register
 */
#define KEYMNG_STATIC_REG (DR_REG_KEYMNG_BASE + 0x18)
/** KEYMNG_USE_EFUSE_KEY : R/W; bitpos: [4:0]; default: 0;
 *  Set each bit to choose efuse key instead of key manager deployed key. Each bit
 *  stands for a key type:bit 4 for psram_key; bit 3 for ds_key; bit 2 for hmac_key;
 *  bit 1 for flash_key; bit 0 for ecdsa_key
 */
#define KEYMNG_USE_EFUSE_KEY    0x0000001FU
#define KEYMNG_USE_EFUSE_KEY_M  (KEYMNG_USE_EFUSE_KEY_V << KEYMNG_USE_EFUSE_KEY_S)
#define KEYMNG_USE_EFUSE_KEY_V  0x0000001FU
#define KEYMNG_USE_EFUSE_KEY_S  0
/** KEYMNG_RND_SWITCH_CYCLE : R/W; bitpos: [9:5]; default: 15;
 *  The core clock cycle number to sample one rng input data. Please set it bigger than
 *  the clock cycle ratio: T_rng/T_km
 */
#define KEYMNG_RND_SWITCH_CYCLE    0x0000001FU
#define KEYMNG_RND_SWITCH_CYCLE_M  (KEYMNG_RND_SWITCH_CYCLE_V << KEYMNG_RND_SWITCH_CYCLE_S)
#define KEYMNG_RND_SWITCH_CYCLE_V  0x0000001FU
#define KEYMNG_RND_SWITCH_CYCLE_S  5
/** KEYMNG_USE_SW_INIT_KEY : R/W; bitpos: [10]; default: 0;
 *  Set this bit to use software written init key instead of efuse_init_key.
 */
#define KEYMNG_USE_SW_INIT_KEY    (BIT(10))
#define KEYMNG_USE_SW_INIT_KEY_M  (KEYMNG_USE_SW_INIT_KEY_V << KEYMNG_USE_SW_INIT_KEY_S)
#define KEYMNG_USE_SW_INIT_KEY_V  0x00000001U
#define KEYMNG_USE_SW_INIT_KEY_S  10
/** KEYMNG_FLASH_KEY_LEN : R/W; bitpos: [11]; default: 0;
 *  Set this bit to choose flash crypt using xts-aes-256 or xts-aes-128. 1: use
 *  xts-aes-256. 0: use xts-aes-128.
 */
#define KEYMNG_FLASH_KEY_LEN    (BIT(11))
#define KEYMNG_FLASH_KEY_LEN_M  (KEYMNG_FLASH_KEY_LEN_V << KEYMNG_FLASH_KEY_LEN_S)
#define KEYMNG_FLASH_KEY_LEN_V  0x00000001U
#define KEYMNG_FLASH_KEY_LEN_S  11
/** KEYMNG_PSRAM_KEY_LEN : R/W; bitpos: [12]; default: 0;
 *  Set this bit to choose psram crypt using xts-aes-256 or xts-aes-128. 1: use
 *  xts-aes-256. 0: use xts-aes-128.
 */
#define KEYMNG_PSRAM_KEY_LEN    (BIT(12))
#define KEYMNG_PSRAM_KEY_LEN_M  (KEYMNG_PSRAM_KEY_LEN_V << KEYMNG_PSRAM_KEY_LEN_S)
#define KEYMNG_PSRAM_KEY_LEN_V  0x00000001U
#define KEYMNG_PSRAM_KEY_LEN_S  12

/** KEYMNG_LOCK_REG register
 *  Key Manager static configuration locker register
 */
#define KEYMNG_LOCK_REG (DR_REG_KEYMNG_BASE + 0x1c)
/** KEYMNG_USE_EFUSE_KEY_LOCK : R/W1; bitpos: [4:0]; default: 0;
 *  Write 1 to lock reg_use_efuse_key. Each bit locks the corresponding bit of
 *  reg_use_efuse_key.
 */
#define KEYMNG_USE_EFUSE_KEY_LOCK    0x0000001FU
#define KEYMNG_USE_EFUSE_KEY_LOCK_M  (KEYMNG_USE_EFUSE_KEY_LOCK_V << KEYMNG_USE_EFUSE_KEY_LOCK_S)
#define KEYMNG_USE_EFUSE_KEY_LOCK_V  0x0000001FU
#define KEYMNG_USE_EFUSE_KEY_LOCK_S  0
/** KEYMNG_RND_SWITCH_CYCLE_LOCK : R/W1; bitpos: [5]; default: 0;
 *  Write 1 to lock reg_rnd_switch_cycle.
 */
#define KEYMNG_RND_SWITCH_CYCLE_LOCK    (BIT(5))
#define KEYMNG_RND_SWITCH_CYCLE_LOCK_M  (KEYMNG_RND_SWITCH_CYCLE_LOCK_V << KEYMNG_RND_SWITCH_CYCLE_LOCK_S)
#define KEYMNG_RND_SWITCH_CYCLE_LOCK_V  0x00000001U
#define KEYMNG_RND_SWITCH_CYCLE_LOCK_S  5
/** KEYMNG_USE_SW_INIT_KEY_LOCK : R/W1; bitpos: [6]; default: 0;
 *  Write 1 to lock reg_use_sw_init_key.
 */
#define KEYMNG_USE_SW_INIT_KEY_LOCK    (BIT(6))
#define KEYMNG_USE_SW_INIT_KEY_LOCK_M  (KEYMNG_USE_SW_INIT_KEY_LOCK_V << KEYMNG_USE_SW_INIT_KEY_LOCK_S)
#define KEYMNG_USE_SW_INIT_KEY_LOCK_V  0x00000001U
#define KEYMNG_USE_SW_INIT_KEY_LOCK_S  6
/** KEYMNG_FLASH_KEY_LEN_LOCK : R/W1; bitpos: [7]; default: 0;
 *  Write 1 to lock reg_flash_key_len.
 */
#define KEYMNG_FLASH_KEY_LEN_LOCK    (BIT(7))
#define KEYMNG_FLASH_KEY_LEN_LOCK_M  (KEYMNG_FLASH_KEY_LEN_LOCK_V << KEYMNG_FLASH_KEY_LEN_LOCK_S)
#define KEYMNG_FLASH_KEY_LEN_LOCK_V  0x00000001U
#define KEYMNG_FLASH_KEY_LEN_LOCK_S  7
/** KEYMNG_PSRAM_KEY_LEN_LOCK : R/W1; bitpos: [8]; default: 0;
 *  Write 1 to lock reg_psram_key_len.
 */
#define KEYMNG_PSRAM_KEY_LEN_LOCK    (BIT(8))
#define KEYMNG_PSRAM_KEY_LEN_LOCK_M  (KEYMNG_PSRAM_KEY_LEN_LOCK_V << KEYMNG_PSRAM_KEY_LEN_LOCK_S)
#define KEYMNG_PSRAM_KEY_LEN_LOCK_V  0x00000001U
#define KEYMNG_PSRAM_KEY_LEN_LOCK_S  8

/** KEYMNG_CONF_REG register
 *  Key Manager configuration register
 */
#define KEYMNG_CONF_REG (DR_REG_KEYMNG_BASE + 0x20)
/** KEYMNG_KGEN_MODE : R/W; bitpos: [2:0]; default: 0;
 *  Set this field to choose the key generator deployment mode. 0: random mode. 1: AES
 *  mode. 2: ECDH0 mode. 3: ECDH1 mode. 4: recover mode. 5: export mode. 6-7: reserved.
 */
#define KEYMNG_KGEN_MODE    0x00000007U
#define KEYMNG_KGEN_MODE_M  (KEYMNG_KGEN_MODE_V << KEYMNG_KGEN_MODE_S)
#define KEYMNG_KGEN_MODE_V  0x00000007U
#define KEYMNG_KGEN_MODE_S  0
/** KEYMNG_KEY_PURPOSE : R/W; bitpos: [6:3]; default: 0;
 *  Set this field to choose the key purpose. 1: ecdsa_key 2: flash_256_1_key. 3:
 *  flash_256_2_key. 4: flash_128_key. 6: hmac_key. 7: ds_key. 8: psram_256_1_key. 9:
 *  psram_256_2_key. 10: psram_128_key. Others: reserved.
 */
#define KEYMNG_KEY_PURPOSE    0x0000000FU
#define KEYMNG_KEY_PURPOSE_M  (KEYMNG_KEY_PURPOSE_V << KEYMNG_KEY_PURPOSE_S)
#define KEYMNG_KEY_PURPOSE_V  0x0000000FU
#define KEYMNG_KEY_PURPOSE_S  3

/** KEYMNG_START_REG register
 *  Key Manager control register
 */
#define KEYMNG_START_REG (DR_REG_KEYMNG_BASE + 0x24)
/** KEYMNG_START : WT; bitpos: [0]; default: 0;
 *  Write 1 to continue Key Manager operation at LOAD/GAIN state.
 */
#define KEYMNG_START    (BIT(0))
#define KEYMNG_START_M  (KEYMNG_START_V << KEYMNG_START_S)
#define KEYMNG_START_V  0x00000001U
#define KEYMNG_START_S  0
/** KEYMNG_CONTINUE : WT; bitpos: [1]; default: 0;
 *  Write 1 to start Key Manager at IDLE state.
 */
#define KEYMNG_CONTINUE    (BIT(1))
#define KEYMNG_CONTINUE_M  (KEYMNG_CONTINUE_V << KEYMNG_CONTINUE_S)
#define KEYMNG_CONTINUE_V  0x00000001U
#define KEYMNG_CONTINUE_S  1

/** KEYMNG_STATE_REG register
 *  Key Manager state register
 */
#define KEYMNG_STATE_REG (DR_REG_KEYMNG_BASE + 0x28)
/** KEYMNG_STATE : RO; bitpos: [1:0]; default: 0;
 *  The state of Key Manager. 0: IDLE. 1: LOAD. 2: GAIN. 3: BUSY.
 */
#define KEYMNG_STATE    0x00000003U
#define KEYMNG_STATE_M  (KEYMNG_STATE_V << KEYMNG_STATE_S)
#define KEYMNG_STATE_V  0x00000003U
#define KEYMNG_STATE_S  0

/** KEYMNG_RESULT_REG register
 *  Key Manager operation result register
 */
#define KEYMNG_RESULT_REG (DR_REG_KEYMNG_BASE + 0x2c)
/** KEYMNG_PROC_RESULT : RO/SS; bitpos: [0]; default: 0;
 *  The procedure result bit of Key Manager, only valid when Key Manager procedure is
 *  done. 1: Key Manager procedure succeeded. 0: Key Manager procedure failed.
 */
#define KEYMNG_PROC_RESULT    (BIT(0))
#define KEYMNG_PROC_RESULT_M  (KEYMNG_PROC_RESULT_V << KEYMNG_PROC_RESULT_S)
#define KEYMNG_PROC_RESULT_V  0x00000001U
#define KEYMNG_PROC_RESULT_S  0

/** KEYMNG_KEY_VLD_REG register
 *  Key Manager key status register
 */
#define KEYMNG_KEY_VLD_REG (DR_REG_KEYMNG_BASE + 0x30)
/** KEYMNG_KEY_ECDSA_VLD : RO; bitpos: [0]; default: 0;
 *  The status bit for key_ecdsa.   1: The key has been deployed correctly. 0: The key
 *  has not been deployed yet.
 */
#define KEYMNG_KEY_ECDSA_VLD    (BIT(0))
#define KEYMNG_KEY_ECDSA_VLD_M  (KEYMNG_KEY_ECDSA_VLD_V << KEYMNG_KEY_ECDSA_VLD_S)
#define KEYMNG_KEY_ECDSA_VLD_V  0x00000001U
#define KEYMNG_KEY_ECDSA_VLD_S  0
/** KEYMNG_KEY_FLASH_VLD : RO; bitpos: [1]; default: 0;
 *  The status bit for key_flash.     1: The key has been deployed correctly. 0: The
 *  key has not been deployed yet.
 */
#define KEYMNG_KEY_FLASH_VLD    (BIT(1))
#define KEYMNG_KEY_FLASH_VLD_M  (KEYMNG_KEY_FLASH_VLD_V << KEYMNG_KEY_FLASH_VLD_S)
#define KEYMNG_KEY_FLASH_VLD_V  0x00000001U
#define KEYMNG_KEY_FLASH_VLD_S  1
/** KEYMNG_KEY_HMAC_VLD : RO; bitpos: [2]; default: 0;
 *  The status bit for key_hmac.    1: The key has been deployed correctly. 0: The key
 *  has not been deployed yet.
 */
#define KEYMNG_KEY_HMAC_VLD    (BIT(2))
#define KEYMNG_KEY_HMAC_VLD_M  (KEYMNG_KEY_HMAC_VLD_V << KEYMNG_KEY_HMAC_VLD_S)
#define KEYMNG_KEY_HMAC_VLD_V  0x00000001U
#define KEYMNG_KEY_HMAC_VLD_S  2
/** KEYMNG_KEY_DS_VLD : RO; bitpos: [3]; default: 0;
 *  The status bit for key_ds.         1: The key has been deployed correctly. 0: The
 *  key has not been deployed yet.
 */
#define KEYMNG_KEY_DS_VLD    (BIT(3))
#define KEYMNG_KEY_DS_VLD_M  (KEYMNG_KEY_DS_VLD_V << KEYMNG_KEY_DS_VLD_S)
#define KEYMNG_KEY_DS_VLD_V  0x00000001U
#define KEYMNG_KEY_DS_VLD_S  3
/** KEYMNG_KEY_PSRAM_VLD : RO; bitpos: [4]; default: 0;
 *  The status bit for key_psram.   1: The key has been deployed correctly. 0: The key
 *  has not been deployed yet.
 */
#define KEYMNG_KEY_PSRAM_VLD    (BIT(4))
#define KEYMNG_KEY_PSRAM_VLD_M  (KEYMNG_KEY_PSRAM_VLD_V << KEYMNG_KEY_PSRAM_VLD_S)
#define KEYMNG_KEY_PSRAM_VLD_V  0x00000001U
#define KEYMNG_KEY_PSRAM_VLD_S  4

/** KEYMNG_HUK_VLD_REG register
 *  Key Manager HUK status register
 */
#define KEYMNG_HUK_VLD_REG (DR_REG_KEYMNG_BASE + 0x34)
/** KEYMNG_HUK_VALID : RO; bitpos: [0]; default: 0;
 *  The HUK status. 0: HUK is not valid. 1: HUK is valid.
 */
#define KEYMNG_HUK_VALID    (BIT(0))
#define KEYMNG_HUK_VALID_M  (KEYMNG_HUK_VALID_V << KEYMNG_HUK_VALID_S)
#define KEYMNG_HUK_VALID_V  0x00000001U
#define KEYMNG_HUK_VALID_S  0

/** KEYMNG_DATE_REG register
 *  Version control register
 */
#define KEYMNG_DATE_REG (DR_REG_KEYMNG_BASE + 0xfc)
/** KEYMNG_DATE : R/W; bitpos: [27:0]; default: 36774224;
 *  Key Manager version control register.
 */
#define KEYMNG_DATE    0x0FFFFFFFU
#define KEYMNG_DATE_M  (KEYMNG_DATE_V << KEYMNG_DATE_S)
#define KEYMNG_DATE_V  0x0FFFFFFFU
#define KEYMNG_DATE_S  0

/** KEYMNG_ASSIST_INFO_MEM register
 *  The memory that stores assist key info.
 */
#define KEYMNG_ASSIST_INFO_MEM (DR_REG_KEYMNG_BASE + 0x100)
#define KEYMNG_ASSIST_INFO_MEM_SIZE_BYTES 64

/** KEYMNG_PUBLIC_INFO_MEM register
 *  The memory that stores public key info.
 */
#define KEYMNG_PUBLIC_INFO_MEM (DR_REG_KEYMNG_BASE + 0x140)
#define KEYMNG_PUBLIC_INFO_MEM_SIZE_BYTES 64

/** KEYMNG_SW_INIT_KEY_MEM register
 *  The memory that stores software written init key.
 */
#define KEYMNG_SW_INIT_KEY_MEM (DR_REG_KEYMNG_BASE + 0x180)
#define KEYMNG_SW_INIT_KEY_MEM_SIZE_BYTES 32

#ifdef __cplusplus
}
#endif
