/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of config register
 *  Configures RMA algorithm
 */
typedef union {
    struct {
        /** config_work_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures the RMA module.
         *  00: REQUEST CERTIFICATE
         *  01: VERIFICATION CERTIFICATE
         *  10: FAST VERIFICATION
         */
        uint32_t config_work_mode:2;
        uint32_t reserved_2:1;
        /** config_reuse_jtag : R/W; bitpos: [3]; default: 0;
         *  Whether enable JTAG after RMA pass.
         *  0: Do not enable JTAG
         *  1: Enable JTAG
         */
        uint32_t config_reuse_jtag:1;
        /** config_reuse_download : R/W; bitpos: [4]; default: 0;
         *  Whether enable DOWNLOAD_MODE after RMA pass.
         *  0: Do not enable DOWNLOAD mode
         *  1: Enable DOWNLOAD
         */
        uint32_t config_reuse_download:1;
        /** config_force_spi : R/W; bitpos: [5]; default: 0;
         *  Whether enable FORCE_SPI_BOOT after RMA pass.
         *  0: Do not enable FORCE_SPI_BOOT mode
         *  1: Enable FORCE_SPI_BOOT
         */
        uint32_t config_force_spi:1;
        /** config_use_nonce : R/W; bitpos: [6]; default: 0;
         *  Whether use  NONCE.
         *  0: Do not use  NONCE
         *  1: Use  NONCE
         */
        uint32_t config_use_nonce:1;
        /** config_use_km : R/W; bitpos: [7]; default: 0;
         *  Whether use km to generate NONCE.
         *  0: Do not use KM to generate NONCE
         *  1: Use KM to generate NONCE
         */
        uint32_t config_use_km:1;
        /** config_usc_block_num : R/W; bitpos: [14:8]; default: 0;
         *  The length of USC.
         */
        uint32_t config_usc_block_num:7;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} rma_config_reg_t;


/** Group: Control Registers */
/** Type of trigger register
 *  Starts the RMA module.
 */
typedef union {
    struct {
        /** trigger_start : WO; bitpos: [0]; default: 0;
         *  Start trigger typical rma.
         */
        uint32_t trigger_start:1;
        /** trigger_load : WO; bitpos: [1]; default: 0;
         *  Rma load done.
         */
        uint32_t trigger_load:1;
        /** trigger_gain : WO; bitpos: [2]; default: 0;
         *  Rma gain done.
         */
        uint32_t trigger_gain:1;
        /** trigger_usc : WO; bitpos: [3]; default: 0;
         *  Rma usc done.
         */
        uint32_t trigger_usc:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} rma_trigger_reg_t;


/** Group: Status Registers */
/** Type of state register
 *  query state in rma
 */
typedef union {
    struct {
        /** state : RO; bitpos: [2:0]; default: 0;
         *  The status bits of RMA Accelerator. RMA is at 0: IDLE, 1: LOAD, 2: GAIN, 3: USC, 4:
         *  LOCK, others: BUSY state.
         */
        uint32_t state:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} rma_state_reg_t;


/** Group: Result Registers */
/** Type of log register
 *  Query result in rma
 */
typedef union {
    struct {
        /** log_cert_state : RO; bitpos: [0]; default: 0;
         *  Marks state of rma cert verification result.
         *  1: pass
         *  0: failed
         */
        uint32_t log_cert_state:1;
        /** log_public_key_state : RO; bitpos: [1]; default: 0;
         *  Marks state of rma public key verification result.
         *  1: pass
         *  0: failed
         */
        uint32_t log_public_key_state:1;
        /** log_sign_state : RO; bitpos: [2]; default: 0;
         *  Marks state of rma sign verification result.
         *  1: pass
         *  0: failed
         */
        uint32_t log_sign_state:1;
        uint32_t reserved_3:25;
        /** log_nonce_state : RO; bitpos: [28]; default: 0;
         *  Marks error nonce config .
         *  0 : Use efuse_hash_sg in CERT_REQ or FAST_VEF but key invalid
         *  1 : Nonce config pass.
         */
        uint32_t log_nonce_state:1;
        /** log_km_state : RO; bitpos: [29]; default: 0;
         *  Marks error KM config .
         *  0 : Use km but km key invalid
         *  1 : KM config pass
         */
        uint32_t log_km_state:1;
        /** log_mode_state : RO; bitpos: [30]; default: 0;
         *  Marks error mode config .
         *  0 : Invalid mode config
         *  1  : MODE config pass
         */
        uint32_t log_mode_state:1;
        /** log_efuse_state : RO; bitpos: [31]; default: 0;
         *  Marks efuse state .
         *  0 : Efuse disable RMA module
         *  1  : Efuse pass
         */
        uint32_t log_efuse_state:1;
    };
    uint32_t val;
} rma_log_reg_t;

/** Type of result_st register
 *  RMA result reg.
 */
typedef union {
    struct {
        /** reuse_jtag_st : RO/WTC/SS; bitpos: [0]; default: 0;
         *  The state of reuse jtag register.
         */
        uint32_t reuse_jtag_st:1;
        /** reuse_download_st : RO/WTC/SS; bitpos: [1]; default: 0;
         *  The state of reuse download register.
         */
        uint32_t reuse_download_st:1;
        /** force_spi_boot_st : RO/WTC/SS; bitpos: [2]; default: 0;
         *  The state of force spi boot reg register.
         */
        uint32_t force_spi_boot_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} rma_result_st_reg_t;

/** Type of result_clr register
 *  RMA clr result reg.
 */
typedef union {
    struct {
        /** reuse_jtag_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the reuse jtag state.
         */
        uint32_t reuse_jtag_clr:1;
        /** reuse_download_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the reuse download state.
         */
        uint32_t reuse_download_clr:1;
        /** force_spi_boot_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the force spi boot state.
         */
        uint32_t force_spi_boot_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} rma_result_clr_reg_t;


/** Group: Interrupt registers */
/** Type of int_raw register
 *  RMA interrupt raw register, valid in level.
 */
typedef union {
    struct {
        /** proc_done_int_raw : RO/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status bit  for the rma_proc_done_int interrupt
         */
        uint32_t proc_done_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rma_int_raw_reg_t;

/** Type of int_st register
 *  RMA interrupt status register.
 */
typedef union {
    struct {
        /** proc_done_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status bit  for the rma_proc_done_int interrupt
         */
        uint32_t proc_done_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rma_int_st_reg_t;

/** Type of int_ena register
 *  RMA interrupt enable register.
 */
typedef union {
    struct {
        /** proc_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit  for the rma_proc_done_int interrupt
         */
        uint32_t proc_done_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rma_int_ena_reg_t;

/** Type of int_clr register
 *  RMA interrupt clear register.
 */
typedef union {
    struct {
        /** proc_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the rma_proc_done_int interrupt
         */
        uint32_t proc_done_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} rma_int_clr_reg_t;


/** Group: Configuration registers */
/** Type of timeout_limit register
 *  RMA timeout limit configure register
 */
typedef union {
    struct {
        /** timeout_limit : R/W; bitpos: [31:0]; default: 4294967295;
         *  The timeout limitation for waiting sub-IP.
         */
        uint32_t timeout_limit:32;
    };
    uint32_t val;
} rma_timeout_limit_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 539297817;
         *  Version control register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} rma_date_reg_t;


/** Group: memory type */

typedef struct {
    volatile rma_config_reg_t config;
    volatile rma_trigger_reg_t trigger;
    volatile rma_state_reg_t state;
    volatile rma_log_reg_t log;
    volatile rma_int_raw_reg_t int_raw;
    volatile rma_int_st_reg_t int_st;
    volatile rma_int_ena_reg_t int_ena;
    volatile rma_int_clr_reg_t int_clr;
    volatile rma_result_st_reg_t result_st;
    volatile rma_result_clr_reg_t result_clr;
    volatile rma_timeout_limit_reg_t timeout_limit;
    uint32_t reserved_02c[20];
    volatile rma_date_reg_t date;
    volatile uint32_t chip_info_source[1];
    volatile uint32_t nonce[1];
    volatile uint32_t usc[1];
    volatile uint32_t public_key[1];
    volatile uint32_t sign[1];
    volatile uint32_t cert_hash[1];
    volatile uint32_t chip_info[1];
} rma_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(rma_dev_t) == 0x9c, "Invalid size of rma_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
