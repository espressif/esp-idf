/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Interrupt Registers */
/** Type of in_int_raw_ch0 register
 *  Raw interrupt status of RX channel 0
 */
typedef union {
    struct {
        /** in_done_ch0_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DONE_CH0_INT
         */
        uint32_t in_done_ch0_int_raw:1;
        /** in_suc_eof_ch0_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_SUC_EOF_CH0_INT
         */
        uint32_t in_suc_eof_ch0_int_raw:1;
        /** in_err_eof_ch0_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_ERR_EOF_CH0_INT
         */
        uint32_t in_err_eof_ch0_int_raw:1;
        /** in_dscr_err_ch0_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_ERR_CH0_INT
         */
        uint32_t in_dscr_err_ch0_int_raw:1;
        /** in_dscr_empty_ch0_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_EMPTY_CH0_INT
         */
        uint32_t in_dscr_empty_ch0_int_raw:1;
        /** infifo_ovf_ch0_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_OVF_CH0_INT
         */
        uint32_t infifo_ovf_ch0_int_raw:1;
        /** infifo_udf_ch0_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_UDF_CH0_INT
         */
        uint32_t infifo_udf_ch0_int_raw:1;
        /** in_ahbinf_resp_err_ch0_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_RESP_ERR_CH0_INT
         */
        uint32_t in_ahbinf_resp_err_ch0_int_raw:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_raw_ch0_reg_t;


/** Group: Status Registers */
/** Type of in_int_st_ch0 register
 *  Masked interrupt status of RX channel 0
 */
typedef union {
    struct {
        /** in_done_ch0_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DONE_CH0_INT
         */
        uint32_t in_done_ch0_int_st:1;
        /** in_suc_eof_ch0_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_SUC_EOF_CH0_INT
         */
        uint32_t in_suc_eof_ch0_int_st:1;
        /** in_err_eof_ch0_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_ERR_EOF_CH0_INT
         */
        uint32_t in_err_eof_ch0_int_st:1;
        /** in_dscr_err_ch0_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_ERR_CH0_INT
         */
        uint32_t in_dscr_err_ch0_int_st:1;
        /** in_dscr_empty_ch0_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_EMPTY_CH0_INT
         */
        uint32_t in_dscr_empty_ch0_int_st:1;
        /** infifo_ovf_ch0_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_OVF_CH0_INT
         */
        uint32_t infifo_ovf_ch0_int_st:1;
        /** infifo_udf_ch0_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_UDF_CH0_INT
         */
        uint32_t infifo_udf_ch0_int_st:1;
        /** in_ahbinf_resp_err_ch0_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_RESP_ERR_CH0_INT
         */
        uint32_t in_ahbinf_resp_err_ch0_int_st:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_st_ch0_reg_t;

/** Type of in_int_ena_ch0 register
 *  Interrupt enable bits of RX channel 0
 */
typedef union {
    struct {
        /** in_done_ch0_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DONE_CH0_INT
         */
        uint32_t in_done_ch0_int_ena:1;
        /** in_suc_eof_ch0_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_SUC_EOF_CH0_INT
         */
        uint32_t in_suc_eof_ch0_int_ena:1;
        /** in_err_eof_ch0_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_ERR_EOF_CH0_INT
         */
        uint32_t in_err_eof_ch0_int_ena:1;
        /** in_dscr_err_ch0_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_ERR_CH0_INT
         */
        uint32_t in_dscr_err_ch0_int_ena:1;
        /** in_dscr_empty_ch0_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_EMPTY_CH0_INT
         */
        uint32_t in_dscr_empty_ch0_int_ena:1;
        /** infifo_ovf_ch0_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_OVF_CH0_INT
         */
        uint32_t infifo_ovf_ch0_int_ena:1;
        /** infifo_udf_ch0_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_UDF_CH0_INT
         */
        uint32_t infifo_udf_ch0_int_ena:1;
        /** in_ahbinf_resp_err_ch0_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_RESP_ERR_CH0_INT
         */
        uint32_t in_ahbinf_resp_err_ch0_int_ena:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_ena_ch0_reg_t;

/** Type of in_int_clr_ch0 register
 *  Interrupt clear bits of RX channel 0
 */
typedef union {
    struct {
        /** in_done_ch0_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DONE_CH0_INT
         */
        uint32_t in_done_ch0_int_clr:1;
        /** in_suc_eof_ch0_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_SUC_EOF_CH0_INT
         */
        uint32_t in_suc_eof_ch0_int_clr:1;
        /** in_err_eof_ch0_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_ERR_EOF_CH0_INT
         */
        uint32_t in_err_eof_ch0_int_clr:1;
        /** in_dscr_err_ch0_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_ERR_CH0_INT
         */
        uint32_t in_dscr_err_ch0_int_clr:1;
        /** in_dscr_empty_ch0_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_EMPTY_CH0_INT
         */
        uint32_t in_dscr_empty_ch0_int_clr:1;
        /** infifo_ovf_ch0_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_OVF_CH0_INT
         */
        uint32_t infifo_ovf_ch0_int_clr:1;
        /** infifo_udf_ch0_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_UDF_CH0_INT
         */
        uint32_t infifo_udf_ch0_int_clr:1;
        /** in_ahbinf_resp_err_ch0_int_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_RESP_ERR_CH0_INT
         */
        uint32_t in_ahbinf_resp_err_ch0_int_clr:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_clr_ch0_reg_t;

/** Type of in_int_raw_ch1 register
 *  Raw interrupt status of RX channel 1
 */
typedef union {
    struct {
        /** in_done_ch1_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DONE_CH1_INT
         */
        uint32_t in_done_ch1_int_raw:1;
        /** in_suc_eof_ch1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_SUC_EOF_CH1_INT
         */
        uint32_t in_suc_eof_ch1_int_raw:1;
        /** in_err_eof_ch1_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_ERR_EOF_CH1_INT
         */
        uint32_t in_err_eof_ch1_int_raw:1;
        /** in_dscr_err_ch1_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_ERR_CH1_INT
         */
        uint32_t in_dscr_err_ch1_int_raw:1;
        /** in_dscr_empty_ch1_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_EMPTY_CH1_INT
         */
        uint32_t in_dscr_empty_ch1_int_raw:1;
        /** infifo_ovf_ch1_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_OVF_CH1_INT
         */
        uint32_t infifo_ovf_ch1_int_raw:1;
        /** infifo_udf_ch1_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_UDF_CH1_INT
         */
        uint32_t infifo_udf_ch1_int_raw:1;
        /** in_ahbinf_resp_err_ch1_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_RESP_ERR_CH1_INT
         */
        uint32_t in_ahbinf_resp_err_ch1_int_raw:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_raw_ch1_reg_t;

/** Type of in_int_st_ch1 register
 *  Masked interrupt status of RX channel 1
 */
typedef union {
    struct {
        /** in_done_ch1_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DONE_CH1_INT
         */
        uint32_t in_done_ch1_int_st:1;
        /** in_suc_eof_ch1_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_SUC_EOF_CH1_INT
         */
        uint32_t in_suc_eof_ch1_int_st:1;
        /** in_err_eof_ch1_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_ERR_EOF_CH1_INT
         */
        uint32_t in_err_eof_ch1_int_st:1;
        /** in_dscr_err_ch1_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_ERR_CH1_INT
         */
        uint32_t in_dscr_err_ch1_int_st:1;
        /** in_dscr_empty_ch1_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_EMPTY_CH1_INT
         */
        uint32_t in_dscr_empty_ch1_int_st:1;
        /** infifo_ovf_ch1_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_OVF_CH1_INT
         */
        uint32_t infifo_ovf_ch1_int_st:1;
        /** infifo_udf_ch1_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_UDF_CH1_INT
         */
        uint32_t infifo_udf_ch1_int_st:1;
        /** in_ahbinf_resp_err_ch1_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_RESP_ERR_CH1_INT
         */
        uint32_t in_ahbinf_resp_err_ch1_int_st:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_st_ch1_reg_t;

/** Type of in_int_ena_ch1 register
 *  Interrupt enable bits of RX channel 1
 */
typedef union {
    struct {
        /** in_done_ch1_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DONE_CH1_INT
         */
        uint32_t in_done_ch1_int_ena:1;
        /** in_suc_eof_ch1_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_SUC_EOF_CH1_INT
         */
        uint32_t in_suc_eof_ch1_int_ena:1;
        /** in_err_eof_ch1_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_ERR_EOF_CH1_INT
         */
        uint32_t in_err_eof_ch1_int_ena:1;
        /** in_dscr_err_ch1_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_ERR_CH1_INT
         */
        uint32_t in_dscr_err_ch1_int_ena:1;
        /** in_dscr_empty_ch1_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_EMPTY_CH1_INT
         */
        uint32_t in_dscr_empty_ch1_int_ena:1;
        /** infifo_ovf_ch1_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_OVF_CH1_INT
         */
        uint32_t infifo_ovf_ch1_int_ena:1;
        /** infifo_udf_ch1_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_UDF_CH1_INT
         */
        uint32_t infifo_udf_ch1_int_ena:1;
        /** in_ahbinf_resp_err_ch1_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_RESP_ERR_CH1_INT
         */
        uint32_t in_ahbinf_resp_err_ch1_int_ena:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_ena_ch1_reg_t;

/** Type of in_int_clr_ch1 register
 *  Interrupt clear bits of RX channel 1
 */
typedef union {
    struct {
        /** in_done_ch1_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DONE_CH1_INT
         */
        uint32_t in_done_ch1_int_clr:1;
        /** in_suc_eof_ch1_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_SUC_EOF_CH1_INT
         */
        uint32_t in_suc_eof_ch1_int_clr:1;
        /** in_err_eof_ch1_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_ERR_EOF_CH1_INT
         */
        uint32_t in_err_eof_ch1_int_clr:1;
        /** in_dscr_err_ch1_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_ERR_CH1_INT
         */
        uint32_t in_dscr_err_ch1_int_clr:1;
        /** in_dscr_empty_ch1_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_EMPTY_CH1_INT
         */
        uint32_t in_dscr_empty_ch1_int_clr:1;
        /** infifo_ovf_ch1_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_OVF_CH1_INT
         */
        uint32_t infifo_ovf_ch1_int_clr:1;
        /** infifo_udf_ch1_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_UDF_CH1_INT
         */
        uint32_t infifo_udf_ch1_int_clr:1;
        /** in_ahbinf_resp_err_ch1_int_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_RESP_ERR_CH1_INT
         */
        uint32_t in_ahbinf_resp_err_ch1_int_clr:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_clr_ch1_reg_t;

/** Type of in_int_raw_ch2 register
 *  Raw interrupt status of RX channel 2
 */
typedef union {
    struct {
        /** in_done_ch2_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DONE_CH2_INT
         */
        uint32_t in_done_ch2_int_raw:1;
        /** in_suc_eof_ch2_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_SUC_EOF_CH2_INT
         */
        uint32_t in_suc_eof_ch2_int_raw:1;
        /** in_err_eof_ch2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_ERR_EOF_CH2_INT
         */
        uint32_t in_err_eof_ch2_int_raw:1;
        /** in_dscr_err_ch2_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_ERR_CH2_INT
         */
        uint32_t in_dscr_err_ch2_int_raw:1;
        /** in_dscr_empty_ch2_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_EMPTY_CH2_INT
         */
        uint32_t in_dscr_empty_ch2_int_raw:1;
        /** infifo_ovf_ch2_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_OVF_CH2_INT
         */
        uint32_t infifo_ovf_ch2_int_raw:1;
        /** infifo_udf_ch2_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_UDF_CH2_INT
         */
        uint32_t infifo_udf_ch2_int_raw:1;
        /** in_ahbinf_resp_err_ch2_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_RESP_ERR_CH2_INT
         */
        uint32_t in_ahbinf_resp_err_ch2_int_raw:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_raw_ch2_reg_t;

/** Type of in_int_st_ch2 register
 *  Masked interrupt status of RX channel 2
 */
typedef union {
    struct {
        /** in_done_ch2_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DONE_CH2_INT
         */
        uint32_t in_done_ch2_int_st:1;
        /** in_suc_eof_ch2_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_SUC_EOF_CH2_INT
         */
        uint32_t in_suc_eof_ch2_int_st:1;
        /** in_err_eof_ch2_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_ERR_EOF_CH2_INT
         */
        uint32_t in_err_eof_ch2_int_st:1;
        /** in_dscr_err_ch2_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_ERR_CH2_INT
         */
        uint32_t in_dscr_err_ch2_int_st:1;
        /** in_dscr_empty_ch2_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_EMPTY_CH2_INT
         */
        uint32_t in_dscr_empty_ch2_int_st:1;
        /** infifo_ovf_ch2_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_OVF_CH2_INT
         */
        uint32_t infifo_ovf_ch2_int_st:1;
        /** infifo_udf_ch2_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_UDF_CH2_INT
         */
        uint32_t infifo_udf_ch2_int_st:1;
        /** in_ahbinf_resp_err_ch2_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_RESP_ERR_CH2_INT
         */
        uint32_t in_ahbinf_resp_err_ch2_int_st:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_st_ch2_reg_t;

/** Type of in_int_ena_ch2 register
 *  Interrupt enable bits of RX channel 2
 */
typedef union {
    struct {
        /** in_done_ch2_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DONE_CH2_INT
         */
        uint32_t in_done_ch2_int_ena:1;
        /** in_suc_eof_ch2_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_SUC_EOF_CH2_INT
         */
        uint32_t in_suc_eof_ch2_int_ena:1;
        /** in_err_eof_ch2_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_ERR_EOF_CH2_INT
         */
        uint32_t in_err_eof_ch2_int_ena:1;
        /** in_dscr_err_ch2_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_ERR_CH2_INT
         */
        uint32_t in_dscr_err_ch2_int_ena:1;
        /** in_dscr_empty_ch2_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_EMPTY_CH2_INT
         */
        uint32_t in_dscr_empty_ch2_int_ena:1;
        /** infifo_ovf_ch2_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_OVF_CH2_INT
         */
        uint32_t infifo_ovf_ch2_int_ena:1;
        /** infifo_udf_ch2_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_UDF_CH2_INT
         */
        uint32_t infifo_udf_ch2_int_ena:1;
        /** in_ahbinf_resp_err_ch2_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_RESP_ERR_CH2_INT
         */
        uint32_t in_ahbinf_resp_err_ch2_int_ena:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_ena_ch2_reg_t;

/** Type of in_int_clr_ch2 register
 *  Interrupt clear bits of RX channel 2
 */
typedef union {
    struct {
        /** in_done_ch2_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DONE_CH2_INT
         */
        uint32_t in_done_ch2_int_clr:1;
        /** in_suc_eof_ch2_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_SUC_EOF_CH2_INT
         */
        uint32_t in_suc_eof_ch2_int_clr:1;
        /** in_err_eof_ch2_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_ERR_EOF_CH2_INT
         */
        uint32_t in_err_eof_ch2_int_clr:1;
        /** in_dscr_err_ch2_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_ERR_CH2_INT
         */
        uint32_t in_dscr_err_ch2_int_clr:1;
        /** in_dscr_empty_ch2_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_EMPTY_CH2_INT
         */
        uint32_t in_dscr_empty_ch2_int_clr:1;
        /** infifo_ovf_ch2_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_OVF_CH2_INT
         */
        uint32_t infifo_ovf_ch2_int_clr:1;
        /** infifo_udf_ch2_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_UDF_CH2_INT
         */
        uint32_t infifo_udf_ch2_int_clr:1;
        /** in_ahbinf_resp_err_ch2_int_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_RESP_ERR_CH2_INT
         */
        uint32_t in_ahbinf_resp_err_ch2_int_clr:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_clr_ch2_reg_t;

/** Type of out_int_raw_ch0 register
 *  //Raw interrupt status of TX channel 0
 */
typedef union {
    struct {
        /** out_done_ch0_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DONE_CH0_INT
         */
        uint32_t out_done_ch0_int_raw:1;
        /** out_eof_ch0_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_EOF_CH0_INT
         */
        uint32_t out_eof_ch0_int_raw:1;
        /** out_dscr_err_ch0_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DSCR_ERR_CH0_INT
         */
        uint32_t out_dscr_err_ch0_int_raw:1;
        /** out_total_eof_ch0_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_TOTAL_EOF_CH0_INT
         */
        uint32_t out_total_eof_ch0_int_raw:1;
        /** outfifo_ovf_ch0_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_OVF_CH0_INT
         */
        uint32_t outfifo_ovf_ch0_int_raw:1;
        /** outfifo_udf_ch0_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_UDF_CH0_INT
         */
        uint32_t outfifo_udf_ch0_int_raw:1;
        /** out_ahbinf_resp_err_ch0_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_RESP_ERR_CH0_INT
         */
        uint32_t out_ahbinf_resp_err_ch0_int_raw:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_raw_ch0_reg_t;

/** Type of out_int_st_ch0 register
 *  Masked interrupt status of TX channel 0
 */
typedef union {
    struct {
        /** out_done_ch0_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DONE_CH0_INT
         */
        uint32_t out_done_ch0_int_st:1;
        /** out_eof_ch0_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_EOF_CH0_INT
         */
        uint32_t out_eof_ch0_int_st:1;
        /** out_dscr_err_ch0_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DSCR_ERR_CH0_INT
         */
        uint32_t out_dscr_err_ch0_int_st:1;
        /** out_total_eof_ch0_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_TOTAL_EOF_CH0_INT
         */
        uint32_t out_total_eof_ch0_int_st:1;
        /** outfifo_ovf_ch0_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_OVF_CH0_INT
         */
        uint32_t outfifo_ovf_ch0_int_st:1;
        /** outfifo_udf_ch0_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_UDF_CH0_INT
         */
        uint32_t outfifo_udf_ch0_int_st:1;
        /** out_ahbinf_resp_err_ch0_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_RESP_ERR_CH0_INT
         */
        uint32_t out_ahbinf_resp_err_ch0_int_st:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_st_ch0_reg_t;

/** Type of out_int_ena_ch0 register
 *  Interrupt enable bits of TX channel 0
 */
typedef union {
    struct {
        /** out_done_ch0_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DONE_CH0_INT
         */
        uint32_t out_done_ch0_int_ena:1;
        /** out_eof_ch0_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_EOF_CH0_INT
         */
        uint32_t out_eof_ch0_int_ena:1;
        /** out_dscr_err_ch0_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DSCR_ERR_CH0_INT
         */
        uint32_t out_dscr_err_ch0_int_ena:1;
        /** out_total_eof_ch0_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_TOTAL_EOF_CH0_INT
         */
        uint32_t out_total_eof_ch0_int_ena:1;
        /** outfifo_ovf_ch0_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_OVF_CH0_INT
         */
        uint32_t outfifo_ovf_ch0_int_ena:1;
        /** outfifo_udf_ch0_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_UDF_CH0_INT
         */
        uint32_t outfifo_udf_ch0_int_ena:1;
        /** out_ahbinf_resp_err_ch0_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_RESP_ERR_CH0_INT
         */
        uint32_t out_ahbinf_resp_err_ch0_int_ena:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_ena_ch0_reg_t;

/** Type of out_int_clr_ch0 register
 *  Interrupt clear bits of TX channel 0
 */
typedef union {
    struct {
        /** out_done_ch0_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DONE_CH0_INT
         */
        uint32_t out_done_ch0_int_clr:1;
        /** out_eof_ch0_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_EOF_CH0_INT
         */
        uint32_t out_eof_ch0_int_clr:1;
        /** out_dscr_err_ch0_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DSCR_ERR_CH0_INT
         */
        uint32_t out_dscr_err_ch0_int_clr:1;
        /** out_total_eof_ch0_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_TOTAL_EOF_CH0_INT
         */
        uint32_t out_total_eof_ch0_int_clr:1;
        /** outfifo_ovf_ch0_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_OVF_CH0_INT
         */
        uint32_t outfifo_ovf_ch0_int_clr:1;
        /** outfifo_udf_ch0_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_UDF_CH0_INT
         */
        uint32_t outfifo_udf_ch0_int_clr:1;
        /** out_ahbinf_resp_err_ch0_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_RESP_ERR_CH0_INT
         */
        uint32_t out_ahbinf_resp_err_ch0_int_clr:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_clr_ch0_reg_t;

/** Type of out_int_raw_ch1 register
 *  //Raw interrupt status of TX channel 1
 */
typedef union {
    struct {
        /** out_done_ch1_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DONE_CH1_INT
         */
        uint32_t out_done_ch1_int_raw:1;
        /** out_eof_ch1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_EOF_CH1_INT
         */
        uint32_t out_eof_ch1_int_raw:1;
        /** out_dscr_err_ch1_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DSCR_ERR_CH1_INT
         */
        uint32_t out_dscr_err_ch1_int_raw:1;
        /** out_total_eof_ch1_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_TOTAL_EOF_CH1_INT
         */
        uint32_t out_total_eof_ch1_int_raw:1;
        /** outfifo_ovf_ch1_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_OVF_CH1_INT
         */
        uint32_t outfifo_ovf_ch1_int_raw:1;
        /** outfifo_udf_ch1_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_UDF_CH1_INT
         */
        uint32_t outfifo_udf_ch1_int_raw:1;
        /** out_ahbinf_resp_err_ch1_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_RESP_ERR_CH1_INT
         */
        uint32_t out_ahbinf_resp_err_ch1_int_raw:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_raw_ch1_reg_t;

/** Type of out_int_st_ch1 register
 *  Masked interrupt status of TX channel 1
 */
typedef union {
    struct {
        /** out_done_ch1_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DONE_CH1_INT
         */
        uint32_t out_done_ch1_int_st:1;
        /** out_eof_ch1_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_EOF_CH1_INT
         */
        uint32_t out_eof_ch1_int_st:1;
        /** out_dscr_err_ch1_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DSCR_ERR_CH1_INT
         */
        uint32_t out_dscr_err_ch1_int_st:1;
        /** out_total_eof_ch1_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_TOTAL_EOF_CH1_INT
         */
        uint32_t out_total_eof_ch1_int_st:1;
        /** outfifo_ovf_ch1_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_OVF_CH1_INT
         */
        uint32_t outfifo_ovf_ch1_int_st:1;
        /** outfifo_udf_ch1_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_UDF_CH1_INT
         */
        uint32_t outfifo_udf_ch1_int_st:1;
        /** out_ahbinf_resp_err_ch1_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_RESP_ERR_CH1_INT
         */
        uint32_t out_ahbinf_resp_err_ch1_int_st:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_st_ch1_reg_t;

/** Type of out_int_ena_ch1 register
 *  Interrupt enable bits of TX channel 1
 */
typedef union {
    struct {
        /** out_done_ch1_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DONE_CH1_INT
         */
        uint32_t out_done_ch1_int_ena:1;
        /** out_eof_ch1_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_EOF_CH1_INT
         */
        uint32_t out_eof_ch1_int_ena:1;
        /** out_dscr_err_ch1_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DSCR_ERR_CH1_INT
         */
        uint32_t out_dscr_err_ch1_int_ena:1;
        /** out_total_eof_ch1_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_TOTAL_EOF_CH1_INT
         */
        uint32_t out_total_eof_ch1_int_ena:1;
        /** outfifo_ovf_ch1_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_OVF_CH1_INT
         */
        uint32_t outfifo_ovf_ch1_int_ena:1;
        /** outfifo_udf_ch1_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_UDF_CH1_INT
         */
        uint32_t outfifo_udf_ch1_int_ena:1;
        /** out_ahbinf_resp_err_ch1_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_RESP_ERR_CH1_INT
         */
        uint32_t out_ahbinf_resp_err_ch1_int_ena:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_ena_ch1_reg_t;

/** Type of out_int_clr_ch1 register
 *  Interrupt clear bits of TX channel 1
 */
typedef union {
    struct {
        /** out_done_ch1_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DONE_CH1_INT
         */
        uint32_t out_done_ch1_int_clr:1;
        /** out_eof_ch1_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_EOF_CH1_INT
         */
        uint32_t out_eof_ch1_int_clr:1;
        /** out_dscr_err_ch1_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DSCR_ERR_CH1_INT
         */
        uint32_t out_dscr_err_ch1_int_clr:1;
        /** out_total_eof_ch1_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_TOTAL_EOF_CH1_INT
         */
        uint32_t out_total_eof_ch1_int_clr:1;
        /** outfifo_ovf_ch1_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_OVF_CH1_INT
         */
        uint32_t outfifo_ovf_ch1_int_clr:1;
        /** outfifo_udf_ch1_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_UDF_CH1_INT
         */
        uint32_t outfifo_udf_ch1_int_clr:1;
        /** out_ahbinf_resp_err_ch1_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_RESP_ERR_CH1_INT
         */
        uint32_t out_ahbinf_resp_err_ch1_int_clr:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_clr_ch1_reg_t;

/** Type of out_int_raw_ch2 register
 *  //Raw interrupt status of TX channel 2
 */
typedef union {
    struct {
        /** out_done_ch2_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DONE_CH2_INT
         */
        uint32_t out_done_ch2_int_raw:1;
        /** out_eof_ch2_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_EOF_CH2_INT
         */
        uint32_t out_eof_ch2_int_raw:1;
        /** out_dscr_err_ch2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DSCR_ERR_CH2_INT
         */
        uint32_t out_dscr_err_ch2_int_raw:1;
        /** out_total_eof_ch2_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_TOTAL_EOF_CH2_INT
         */
        uint32_t out_total_eof_ch2_int_raw:1;
        /** outfifo_ovf_ch2_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_OVF_CH2_INT
         */
        uint32_t outfifo_ovf_ch2_int_raw:1;
        /** outfifo_udf_ch2_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_UDF_CH2_INT
         */
        uint32_t outfifo_udf_ch2_int_raw:1;
        /** out_ahbinf_resp_err_ch2_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_RESP_ERR_CH2_INT
         */
        uint32_t out_ahbinf_resp_err_ch2_int_raw:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_raw_ch2_reg_t;

/** Type of out_int_st_ch2 register
 *  Masked interrupt status of TX channel 2
 */
typedef union {
    struct {
        /** out_done_ch2_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DONE_CH2_INT
         */
        uint32_t out_done_ch2_int_st:1;
        /** out_eof_ch2_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_EOF_CH2_INT
         */
        uint32_t out_eof_ch2_int_st:1;
        /** out_dscr_err_ch2_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DSCR_ERR_CH2_INT
         */
        uint32_t out_dscr_err_ch2_int_st:1;
        /** out_total_eof_ch2_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_TOTAL_EOF_CH2_INT
         */
        uint32_t out_total_eof_ch2_int_st:1;
        /** outfifo_ovf_ch2_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_OVF_CH2_INT
         */
        uint32_t outfifo_ovf_ch2_int_st:1;
        /** outfifo_udf_ch2_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_UDF_CH2_INT
         */
        uint32_t outfifo_udf_ch2_int_st:1;
        /** out_ahbinf_resp_err_ch2_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_RESP_ERR_CH2_INT
         */
        uint32_t out_ahbinf_resp_err_ch2_int_st:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_st_ch2_reg_t;

/** Type of out_int_ena_ch2 register
 *  Interrupt enable bits of TX channel 2
 */
typedef union {
    struct {
        /** out_done_ch2_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DONE_CH2_INT
         */
        uint32_t out_done_ch2_int_ena:1;
        /** out_eof_ch2_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_EOF_CH2_INT
         */
        uint32_t out_eof_ch2_int_ena:1;
        /** out_dscr_err_ch2_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DSCR_ERR_CH2_INT
         */
        uint32_t out_dscr_err_ch2_int_ena:1;
        /** out_total_eof_ch2_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_TOTAL_EOF_CH2_INT
         */
        uint32_t out_total_eof_ch2_int_ena:1;
        /** outfifo_ovf_ch2_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_OVF_CH2_INT
         */
        uint32_t outfifo_ovf_ch2_int_ena:1;
        /** outfifo_udf_ch2_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_UDF_CH2_INT
         */
        uint32_t outfifo_udf_ch2_int_ena:1;
        /** out_ahbinf_resp_err_ch2_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_RESP_ERR_CH2_INT
         */
        uint32_t out_ahbinf_resp_err_ch2_int_ena:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_ena_ch2_reg_t;

/** Type of out_int_clr_ch2 register
 *  Interrupt clear bits of TX channel 2
 */
typedef union {
    struct {
        /** out_done_ch2_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DONE_CH2_INT
         */
        uint32_t out_done_ch2_int_clr:1;
        /** out_eof_ch2_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_EOF_CH2_INT
         */
        uint32_t out_eof_ch2_int_clr:1;
        /** out_dscr_err_ch2_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DSCR_ERR_CH2_INT
         */
        uint32_t out_dscr_err_ch2_int_clr:1;
        /** out_total_eof_ch2_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_TOTAL_EOF_CH2_INT
         */
        uint32_t out_total_eof_ch2_int_clr:1;
        /** outfifo_ovf_ch2_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_OVF_CH2_INT
         */
        uint32_t outfifo_ovf_ch2_int_clr:1;
        /** outfifo_udf_ch2_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_UDF_CH2_INT
         */
        uint32_t outfifo_udf_ch2_int_clr:1;
        /** out_ahbinf_resp_err_ch2_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_RESP_ERR_CH2_INT
         */
        uint32_t out_ahbinf_resp_err_ch2_int_clr:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_clr_ch2_reg_t;

/** Type of ahb_test register
 *  only for test
 */
typedef union {
    struct {
        /** ahb_testmode : R/W; bitpos: [2:0]; default: 0;
         *  reserved
         */
        uint32_t ahb_testmode:3;
        uint32_t reserved_3:1;
        /** ahb_testaddr : R/W; bitpos: [5:4]; default: 0;
         *  reserved
         */
        uint32_t ahb_testaddr:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_ahb_test_reg_t;

/** Type of misc_conf register
 *  reserved
 */
typedef union {
    struct {
        /** ahbm_rst_inter : R/W; bitpos: [0]; default: 0;
         *  Write 1 and then 0 to reset the internal AHB FSM
         */
        uint32_t ahbm_rst_inter:1;
        uint32_t reserved_1:1;
        /** arb_pri_dis : R/W; bitpos: [2]; default: 0;
         *  Configures whether to disable the fixed-priority channel arbitration.
         *  0: Enable
         *  1: Disable
         */
        uint32_t arb_pri_dis:1;
        /** clk_en : R/W; bitpos: [3]; default: 0;
         *  Configures clock gating.
         *  0: Support clock only when the application writes registers.
         *  1: Always force the clock on for registers.
         */
        uint32_t clk_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_misc_conf_reg_t;

/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 2425376;
         *  Version control register
         */
        uint32_t date:32;
    };
    uint32_t val;
} ahb_dma_date_reg_t;

/** Type of in_conf0_ch0 register
 *  Configuration register 0 of RX channel 0
 */
typedef union {
    struct {
        /** in_rst_ch0 : R/W; bitpos: [0]; default: 0;
         *  Write 1 and then 0 to reset AHB_DMA channel 0 RX FSM and RX FIFO pointer.
         */
        uint32_t in_rst_ch0:1;
        /** in_loop_test_ch0 : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t in_loop_test_ch0:1;
        /** indscr_burst_en_ch0 : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable INCR burst transfer for RX channel 0 to read
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t indscr_burst_en_ch0:1;
        /** in_data_burst_en_ch0 : R/W; bitpos: [3]; default: 0;
         *  Set this bit to 1 to enable INCR4 burst transfer for Rx channel 0 receiving data
         *  when accessing internal SRAM.
         */
        uint32_t in_data_burst_en_ch0:1;
        /** mem_trans_en_ch0 : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable memory-to-memory data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t mem_trans_en_ch0:1;
        /** in_etm_en_ch0 : R/W; bitpos: [5]; default: 0;
         *  Configures whether to enable ETM control for RX channel0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t in_etm_en_ch0:1;
        /** in_data_burst_mode_sel_ch0 : R/W; bitpos: [7:6]; default: 1;
         *  Configures max burst size for Rx channel0.
         *  2'b00: single
         *  2'b01: incr4
         *  2'b10: incr8
         *  2'b11: incr16
         */
        uint32_t in_data_burst_mode_sel_ch0:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_conf0_ch0_reg_t;

/** Type of in_conf1_ch0 register
 *  Configuration register 1 of RX channel 0
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** in_check_owner_ch0 : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for RX channel 0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t in_check_owner_ch0:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_conf1_ch0_reg_t;

/** Type of infifo_status_ch0 register
 *  Receive FIFO status of RX channel 0
 */
typedef union {
    struct {
        /** infifo_full_ch0 : RO; bitpos: [0]; default: 1;
         *  Represents whether L1 RX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t infifo_full_ch0:1;
        /** infifo_empty_ch0 : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 RX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t infifo_empty_ch0:1;
        uint32_t reserved_2:6;
        /** infifo_cnt_ch0 : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 RX FIFO for RX channel 0
         */
        uint32_t infifo_cnt_ch0:7;
        uint32_t reserved_15:8;
        /** in_remain_under_1b_ch0 : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_1b_ch0:1;
        /** in_remain_under_2b_ch0 : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_2b_ch0:1;
        /** in_remain_under_3b_ch0 : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_3b_ch0:1;
        /** in_remain_under_4b_ch0 : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_4b_ch0:1;
        /** in_buf_hungry_ch0 : RO; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t in_buf_hungry_ch0:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ahb_dma_infifo_status_ch0_reg_t;

/** Type of in_pop_ch0 register
 *  Receive FIFO status of RX channel 0
 */
typedef union {
    struct {
        /** infifo_rdata_ch0 : RO; bitpos: [11:0]; default: 2048;
         *  Represents the data popped from AHB_DMA FIFO.
         */
        uint32_t infifo_rdata_ch0:12;
        /** infifo_pop_ch0 : WT; bitpos: [12]; default: 0;
         *  Configures whether to pop data from AHB_DMA FIFO.
         *  0: Invalid. No effect
         *  1: Pop
         */
        uint32_t infifo_pop_ch0:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_pop_ch0_reg_t;

/** Type of in_link_ch0 register
 *  Receive FIFO status of RX channel 0
 */
typedef union {
    struct {
        /** inlink_auto_ret_ch0 : R/W; bitpos: [0]; default: 1;
         *  Configures whether to return to current receive descriptor's address when there are
         *  some errors in current receiving data.
         *  0: Not return
         *  1: Return
         *  .
         */
        uint32_t inlink_auto_ret_ch0:1;
        /** inlink_stop_ch0 : WT; bitpos: [1]; default: 0;
         *  Configures whether to stop AHB_DMA's RX channel 0 from receiving data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t inlink_stop_ch0:1;
        /** inlink_start_ch0 : WT; bitpos: [2]; default: 0;
         *  Configures whether to enable AHB_DMA's RX channel 0 for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t inlink_start_ch0:1;
        /** inlink_restart_ch0 : WT; bitpos: [3]; default: 0;
         *  Configures whether to restart RX channel 0 for AHB_DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t inlink_restart_ch0:1;
        /** inlink_park_ch0 : RO; bitpos: [4]; default: 1;
         *  Represents the status of the receive descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t inlink_park_ch0:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} ahb_dma_in_link_ch0_reg_t;

/** Type of in_state_ch0 register
 *  Receive status of RX channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_addr_ch0 : RO; bitpos: [17:0]; default: 0;
         *  reserved
         */
        uint32_t inlink_dscr_addr_ch0:18;
        /** in_dscr_state_ch0 : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t in_dscr_state_ch0:2;
        /** in_state_ch0 : RO; bitpos: [22:20]; default: 0;
         *  Represents the address of the lower 18 bits of the next receive descriptor to be
         *  processed.
         */
        uint32_t in_state_ch0:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_in_state_ch0_reg_t;

/** Type of in_suc_eof_des_addr_ch0 register
 *  Receive descriptor address when EOF occurs on RX channel 0
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_ch0:32;
    };
    uint32_t val;
} ahb_dma_in_suc_eof_des_addr_ch0_reg_t;

/** Type of in_err_eof_des_addr_ch0 register
 *  Receive descriptor address when errors occur of RX channel 0
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when there are some errors in the
         *  currently received data.
         */
        uint32_t in_err_eof_des_addr_ch0:32;
    };
    uint32_t val;
} ahb_dma_in_err_eof_des_addr_ch0_reg_t;

/** Type of in_dscr_ch0 register
 *  Current receive descriptor address of RX channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next receive descriptor x+1 pointed by the current
         *  receive descriptor that has already been fetched.
         */
        uint32_t inlink_dscr_ch0:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_ch0_reg_t;

/** Type of in_dscr_bf0_ch0 register
 *  The last receive descriptor address of RX channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current receive descriptor x that has already been
         *  fetched.
         */
        uint32_t inlink_dscr_bf0_ch0:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf0_ch0_reg_t;

/** Type of in_dscr_bf1_ch0 register
 *  The second-to-last receive descriptor address of RX channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous receive descriptor x-1 that has already been
         *  fetched.
         */
        uint32_t inlink_dscr_bf1_ch0:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf1_ch0_reg_t;

/** Type of in_peri_ch0 register
 *  Priority register of RX channel 0
 */
typedef union {
    struct {
        /** rx_pri_ch0 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of RX channel 0.The larger of the value, the higher of the
         *  priority..
         */
        uint32_t rx_pri_ch0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_in_peri_ch0_reg_t;

/** Type of in_peri_sel_ch0 register
 *  Peripheral selection register of RX channel 0
 */
typedef union {
    struct {
        /** peri_in_sel_ch0 : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to RX channel 0.
         *  0: I3C
         *  1: Dummy
         *  2: UHCI0
         *  3: I2S0
         *  4: I2S1
         *  5: I2S2
         *  6: Dummy
         *  7: Dummy
         *  8: ADC_DAC
         *  9: Dummy
         *  10: RMT
         *  11~15: Dummy
         */
        uint32_t peri_in_sel_ch0:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_in_peri_sel_ch0_reg_t;

/** Type of out_conf0_ch0 register
 *  Configuration register 0 of TX channel 0
 */
typedef union {
    struct {
        /** out_rst_ch0 : R/W; bitpos: [0]; default: 0;
         *  Configures the reset state of AHB_DMA channel 0 TX FSM and TX FIFO pointer.
         *  0: Release reset
         *  1: Reset
         */
        uint32_t out_rst_ch0:1;
        /** out_loop_test_ch0 : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t out_loop_test_ch0:1;
        /** out_auto_wrback_ch0 : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable automatic outlink write-back when all the data in TX
         *  FIFO has been transmitted.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_auto_wrback_ch0:1;
        /** out_eof_mode_ch0 : R/W; bitpos: [3]; default: 1;
         *  Configures when to generate EOF flag.
         *  0: EOF flag for TX channel 0 is generated when data to be transmitted has been
         *  pushed into FIFO in AHB_DMA.
         *  1: EOF flag for TX channel 0 is generated when data to be transmitted has been
         *  popped from FIFO in AHB_DMA.
         */
        uint32_t out_eof_mode_ch0:1;
        /** outdscr_burst_en_ch0 : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable INCR burst transfer for TX channel 0 reading
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t outdscr_burst_en_ch0:1;
        /** out_data_burst_en_ch0 : R/W; bitpos: [5]; default: 0;
         *  Set this bit to 1 to enable INCR4 burst transfer for Tx channel 0 transmitting data
         *  when accessing internal SRAM.
         */
        uint32_t out_data_burst_en_ch0:1;
        /** out_etm_en_ch0 : R/W; bitpos: [6]; default: 0;
         *  Configures whether to enable ETM control for TX channel 0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_etm_en_ch0:1;
        uint32_t reserved_7:1;
        /** out_data_burst_mode_sel_ch0 : R/W; bitpos: [9:8]; default: 1;
         *  Configures max burst size for TX channel0.
         *  2'b00: single
         *  2'b01: incr4
         *  2'b10: incr8
         *  2'b11: incr16
         */
        uint32_t out_data_burst_mode_sel_ch0:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_conf0_ch0_reg_t;

/** Type of out_conf1_ch0 register
 *  Configuration register 1 of TX channel 0
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** out_check_owner_ch0 : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for TX channel 0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_check_owner_ch0:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_out_conf1_ch0_reg_t;

/** Type of outfifo_status_ch0 register
 *  Receive FIFO status of RX channel 0
 */
typedef union {
    struct {
        /** outfifo_full_ch0 : RO; bitpos: [0]; default: 0;
         *  Represents whether L1 TX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t outfifo_full_ch0:1;
        /** outfifo_empty_ch0 : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 TX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t outfifo_empty_ch0:1;
        uint32_t reserved_2:6;
        /** outfifo_cnt_ch0 : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 TX FIFO for TX channel 0
         */
        uint32_t outfifo_cnt_ch0:7;
        uint32_t reserved_15:8;
        /** out_remain_under_1b_ch0 : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_1b_ch0:1;
        /** out_remain_under_2b_ch0 : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_2b_ch0:1;
        /** out_remain_under_3b_ch0 : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_3b_ch0:1;
        /** out_remain_under_4b_ch0 : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_4b_ch0:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} ahb_dma_outfifo_status_ch0_reg_t;

/** Type of out_push_ch0 register
 *  Push control register of TX channel 0
 */
typedef union {
    struct {
        /** outfifo_wdata_ch0 : R/W; bitpos: [8:0]; default: 0;
         *  Configures whether to push data into AHB_DMA FIFO.
         *  0: Invalid. No effect
         *  1: Push
         */
        uint32_t outfifo_wdata_ch0:9;
        /** outfifo_push_ch0 : WT; bitpos: [9]; default: 0;
         *  Configures the data that need to be pushed into AHB_DMA FIFO.
         */
        uint32_t outfifo_push_ch0:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_push_ch0_reg_t;

/** Type of out_link_ch0 register
 *  Push control register of TX channel 0
 */
typedef union {
    struct {
        /** outlink_stop_ch0 : WT; bitpos: [0]; default: 0;
         *  Configures whether to stop AHB_DMA's TX channel 0 from transmitting data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t outlink_stop_ch0:1;
        /** outlink_start_ch0 : WT; bitpos: [1]; default: 0;
         *  Configures whether to enable AHB_DMA's TX channel 0 for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t outlink_start_ch0:1;
        /** outlink_restart_ch0 : WT; bitpos: [2]; default: 0;
         *  Configures whether to restart TX channel 0 for AHB_DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t outlink_restart_ch0:1;
        /** outlink_park_ch0 : RO; bitpos: [3]; default: 1;
         *  Represents the status of the transmit descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t outlink_park_ch0:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_link_ch0_reg_t;

/** Type of out_state_ch0 register
 *  Transmit status of TX channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_addr_ch0 : RO; bitpos: [17:0]; default: 0;
         *  Represents the lower 18 bits of the address of the next transmit descriptor to be
         *  processed.
         */
        uint32_t outlink_dscr_addr_ch0:18;
        /** out_dscr_state_ch0 : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t out_dscr_state_ch0:2;
        /** out_state_ch0 : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t out_state_ch0:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_out_state_ch0_reg_t;

/** Type of out_eof_des_addr_ch0 register
 *  Transmit descriptor address when EOF occurs on TX channel 0
 */
typedef union {
    struct {
        /** out_eof_des_addr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_ch0:32;
    };
    uint32_t val;
} ahb_dma_out_eof_des_addr_ch0_reg_t;

/** Type of out_eof_bfr_des_addr_ch0 register
 *  The last transmit descriptor address when EOF occurs on TX channel 0
 */
typedef union {
    struct {
        /** out_eof_bfr_des_addr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor before the last transmit
         *  descriptor.
         */
        uint32_t out_eof_bfr_des_addr_ch0:32;
    };
    uint32_t val;
} ahb_dma_out_eof_bfr_des_addr_ch0_reg_t;

/** Type of out_dscr_ch0 register
 *  Current transmit descriptor address of TX channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next transmit descriptor y+1 pointed by the current
         *  transmit descriptor that has already been fetched.
         */
        uint32_t outlink_dscr_ch0:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_ch0_reg_t;

/** Type of out_dscr_bf0_ch0 register
 *  The last transmit descriptor address of TX channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current transmit descriptor y that has already been
         *  fetched.
         */
        uint32_t outlink_dscr_bf0_ch0:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf0_ch0_reg_t;

/** Type of out_dscr_bf1_ch0 register
 *  The second-to-last transmit descriptor address of TX channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous transmit descriptor y-1 that has already
         *  been fetched.
         */
        uint32_t outlink_dscr_bf1_ch0:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf1_ch0_reg_t;

/** Type of out_peri_ch0 register
 *  Priority register of TX channel 0
 */
typedef union {
    struct {
        /** tx_pri_ch0 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of TX channel 0.The larger of the value, the higher of the
         *  priority..
         */
        uint32_t tx_pri_ch0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_peri_ch0_reg_t;

/** Type of out_peri_sel_ch0 register
 *  Peripheral selection register of TX channel 0
 */
typedef union {
    struct {
        /** peri_out_sel_ch0 : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to TX channel 0.
         *  0: I3C
         *  1: Dummy
         *  2: UHCI0
         *  3: I2S0
         *  4: I2S1
         *  5: I2S2
         *  6: Dummy
         *  7: Dummy
         *  8: ADC_DAC
         *  9: Dummy
         *  10: RMT
         *  11~15: Dummy
         */
        uint32_t peri_out_sel_ch0:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_out_peri_sel_ch0_reg_t;

/** Type of in_conf0_ch1 register
 *  Configuration register 0 of RX channel 1
 */
typedef union {
    struct {
        /** in_rst_ch1 : R/W; bitpos: [0]; default: 0;
         *  Write 1 and then 0 to reset AHB_DMA channel 1 RX FSM and RX FIFO pointer.
         */
        uint32_t in_rst_ch1:1;
        /** in_loop_test_ch1 : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t in_loop_test_ch1:1;
        /** indscr_burst_en_ch1 : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable INCR burst transfer for RX channel 1 to read
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t indscr_burst_en_ch1:1;
        /** in_data_burst_en_ch1 : R/W; bitpos: [3]; default: 0;
         *  Set this bit to 1 to enable INCR4 burst transfer for Rx channel 1 receiving data
         *  when accessing internal SRAM.
         */
        uint32_t in_data_burst_en_ch1:1;
        /** mem_trans_en_ch1 : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable memory-to-memory data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t mem_trans_en_ch1:1;
        /** in_etm_en_ch1 : R/W; bitpos: [5]; default: 0;
         *  Configures whether to enable ETM control for RX channel1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t in_etm_en_ch1:1;
        /** in_data_burst_mode_sel_ch1 : R/W; bitpos: [7:6]; default: 1;
         *  Configures max burst size for Rx channel1.
         *  2'b00: single
         *  2'b01: incr4
         *  2'b10: incr8
         *  2'b11: incr16
         */
        uint32_t in_data_burst_mode_sel_ch1:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_conf0_ch1_reg_t;

/** Type of in_conf1_ch1 register
 *  Configuration register 1 of RX channel 1
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** in_check_owner_ch1 : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for RX channel 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t in_check_owner_ch1:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_conf1_ch1_reg_t;

/** Type of infifo_status_ch1 register
 *  Receive FIFO status of RX channel 1
 */
typedef union {
    struct {
        /** infifo_full_ch1 : RO; bitpos: [0]; default: 1;
         *  Represents whether L1 RX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t infifo_full_ch1:1;
        /** infifo_empty_ch1 : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 RX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t infifo_empty_ch1:1;
        uint32_t reserved_2:6;
        /** infifo_cnt_ch1 : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 RX FIFO for RX channel 1
         */
        uint32_t infifo_cnt_ch1:7;
        uint32_t reserved_15:8;
        /** in_remain_under_1b_ch1 : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_1b_ch1:1;
        /** in_remain_under_2b_ch1 : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_2b_ch1:1;
        /** in_remain_under_3b_ch1 : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_3b_ch1:1;
        /** in_remain_under_4b_ch1 : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_4b_ch1:1;
        /** in_buf_hungry_ch1 : RO; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t in_buf_hungry_ch1:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ahb_dma_infifo_status_ch1_reg_t;

/** Type of in_pop_ch1 register
 *  Receive FIFO status of RX channel 1
 */
typedef union {
    struct {
        /** infifo_rdata_ch1 : RO; bitpos: [11:0]; default: 2048;
         *  Represents the data popped from AHB_DMA FIFO.
         */
        uint32_t infifo_rdata_ch1:12;
        /** infifo_pop_ch1 : WT; bitpos: [12]; default: 0;
         *  Configures whether to pop data from AHB_DMA FIFO.
         *  0: Invalid. No effect
         *  1: Pop
         */
        uint32_t infifo_pop_ch1:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_pop_ch1_reg_t;

/** Type of in_link_ch1 register
 *  Receive FIFO status of RX channel 1
 */
typedef union {
    struct {
        /** inlink_auto_ret_ch1 : R/W; bitpos: [0]; default: 1;
         *  Configures whether to return to current receive descriptor's address when there are
         *  some errors in current receiving data.
         *  0: Not return
         *  1: Return
         *  .
         */
        uint32_t inlink_auto_ret_ch1:1;
        /** inlink_stop_ch1 : WT; bitpos: [1]; default: 0;
         *  Configures whether to stop AHB_DMA's RX channel 1 from receiving data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t inlink_stop_ch1:1;
        /** inlink_start_ch1 : WT; bitpos: [2]; default: 0;
         *  Configures whether to enable AHB_DMA's RX channel 1 for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t inlink_start_ch1:1;
        /** inlink_restart_ch1 : WT; bitpos: [3]; default: 0;
         *  Configures whether to restart RX channel 1 for AHB_DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t inlink_restart_ch1:1;
        /** inlink_park_ch1 : RO; bitpos: [4]; default: 1;
         *  Represents the status of the receive descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t inlink_park_ch1:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} ahb_dma_in_link_ch1_reg_t;

/** Type of in_state_ch1 register
 *  Receive status of RX channel 1
 */
typedef union {
    struct {
        /** inlink_dscr_addr_ch1 : RO; bitpos: [17:0]; default: 0;
         *  reserved
         */
        uint32_t inlink_dscr_addr_ch1:18;
        /** in_dscr_state_ch1 : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t in_dscr_state_ch1:2;
        /** in_state_ch1 : RO; bitpos: [22:20]; default: 0;
         *  Represents the address of the lower 18 bits of the next receive descriptor to be
         *  processed.
         */
        uint32_t in_state_ch1:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_in_state_ch1_reg_t;

/** Type of in_suc_eof_des_addr_ch1 register
 *  Receive descriptor address when EOF occurs on RX channel 1
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_ch1:32;
    };
    uint32_t val;
} ahb_dma_in_suc_eof_des_addr_ch1_reg_t;

/** Type of in_err_eof_des_addr_ch1 register
 *  Receive descriptor address when errors occur of RX channel 1
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when there are some errors in the
         *  currently received data.
         */
        uint32_t in_err_eof_des_addr_ch1:32;
    };
    uint32_t val;
} ahb_dma_in_err_eof_des_addr_ch1_reg_t;

/** Type of in_dscr_ch1 register
 *  Current receive descriptor address of RX channel 1
 */
typedef union {
    struct {
        /** inlink_dscr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next receive descriptor x+1 pointed by the current
         *  receive descriptor that has already been fetched.
         */
        uint32_t inlink_dscr_ch1:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_ch1_reg_t;

/** Type of in_dscr_bf0_ch1 register
 *  The last receive descriptor address of RX channel 1
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current receive descriptor x that has already been
         *  fetched.
         */
        uint32_t inlink_dscr_bf0_ch1:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf0_ch1_reg_t;

/** Type of in_dscr_bf1_ch1 register
 *  The second-to-last receive descriptor address of RX channel 1
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous receive descriptor x-1 that has already been
         *  fetched.
         */
        uint32_t inlink_dscr_bf1_ch1:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf1_ch1_reg_t;

/** Type of in_peri_ch1 register
 *  Priority register of RX channel 1
 */
typedef union {
    struct {
        /** rx_pri_ch1 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of RX channel 1.The larger of the value, the higher of the
         *  priority..
         */
        uint32_t rx_pri_ch1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_in_peri_ch1_reg_t;

/** Type of in_peri_sel_ch1 register
 *  Peripheral selection register of RX channel 1
 */
typedef union {
    struct {
        /** peri_in_sel_ch1 : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to RX channel 1.
         *  0: I3C
         *  1: Dummy
         *  2: UHCI0
         *  3: I2S0
         *  4: I2S1
         *  5: I2S2
         *  6: Dummy
         *  7: Dummy
         *  8: ADC_DAC
         *  9: Dummy
         *  10: RMT
         *  11~15: Dummy
         */
        uint32_t peri_in_sel_ch1:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_in_peri_sel_ch1_reg_t;

/** Type of out_conf0_ch1 register
 *  Configuration register 0 of TX channel 1
 */
typedef union {
    struct {
        /** out_rst_ch1 : R/W; bitpos: [0]; default: 0;
         *  Configures the reset state of AHB_DMA channel 1 TX FSM and TX FIFO pointer.
         *  0: Release reset
         *  1: Reset
         */
        uint32_t out_rst_ch1:1;
        /** out_loop_test_ch1 : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t out_loop_test_ch1:1;
        /** out_auto_wrback_ch1 : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable automatic outlink write-back when all the data in TX
         *  FIFO has been transmitted.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_auto_wrback_ch1:1;
        /** out_eof_mode_ch1 : R/W; bitpos: [3]; default: 1;
         *  Configures when to generate EOF flag.
         *  0: EOF flag for TX channel 1 is generated when data to be transmitted has been
         *  pushed into FIFO in AHB_DMA.
         *  1: EOF flag for TX channel 1 is generated when data to be transmitted has been
         *  popped from FIFO in AHB_DMA.
         */
        uint32_t out_eof_mode_ch1:1;
        /** outdscr_burst_en_ch1 : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable INCR burst transfer for TX channel 1 reading
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t outdscr_burst_en_ch1:1;
        /** out_data_burst_en_ch1 : R/W; bitpos: [5]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 1 transmitting data
         *  when accessing internal SRAM.
         */
        uint32_t out_data_burst_en_ch1:1;
        /** out_etm_en_ch1 : R/W; bitpos: [6]; default: 0;
         *  Configures whether to enable ETM control for TX channel 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_etm_en_ch1:1;
        uint32_t reserved_7:1;
        /** out_data_burst_mode_sel_ch1 : R/W; bitpos: [9:8]; default: 1;
         *  Configures max burst size for TX channel1.
         *  2'b00: single
         *  2'b01: incr4
         *  2'b10: incr8
         *  2'b11: incr16
         */
        uint32_t out_data_burst_mode_sel_ch1:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_conf0_ch1_reg_t;

/** Type of out_conf1_ch1 register
 *  Configuration register 1 of TX channel 1
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** out_check_owner_ch1 : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for TX channel 1.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_check_owner_ch1:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_out_conf1_ch1_reg_t;

/** Type of outfifo_status_ch1 register
 *  Receive FIFO status of RX channel 1
 */
typedef union {
    struct {
        /** outfifo_full_ch1 : RO; bitpos: [0]; default: 0;
         *  Represents whether L1 TX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t outfifo_full_ch1:1;
        /** outfifo_empty_ch1 : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 TX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t outfifo_empty_ch1:1;
        uint32_t reserved_2:6;
        /** outfifo_cnt_ch1 : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 TX FIFO for TX channel 1
         */
        uint32_t outfifo_cnt_ch1:7;
        uint32_t reserved_15:8;
        /** out_remain_under_1b_ch1 : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_1b_ch1:1;
        /** out_remain_under_2b_ch1 : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_2b_ch1:1;
        /** out_remain_under_3b_ch1 : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_3b_ch1:1;
        /** out_remain_under_4b_ch1 : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_4b_ch1:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} ahb_dma_outfifo_status_ch1_reg_t;

/** Type of out_push_ch1 register
 *  Push control register of TX channel 1
 */
typedef union {
    struct {
        /** outfifo_wdata_ch1 : R/W; bitpos: [8:0]; default: 0;
         *  Configures whether to push data into AHB_DMA FIFO.
         *  0: Invalid. No effect
         *  1: Push
         */
        uint32_t outfifo_wdata_ch1:9;
        /** outfifo_push_ch1 : WT; bitpos: [9]; default: 0;
         *  Configures the data that need to be pushed into AHB_DMA FIFO.
         */
        uint32_t outfifo_push_ch1:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_push_ch1_reg_t;

/** Type of out_link_ch1 register
 *  Push control register of TX channel 1
 */
typedef union {
    struct {
        /** outlink_stop_ch1 : WT; bitpos: [0]; default: 0;
         *  Configures whether to stop AHB_DMA's TX channel 1 from transmitting data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t outlink_stop_ch1:1;
        /** outlink_start_ch1 : WT; bitpos: [1]; default: 0;
         *  Configures whether to enable AHB_DMA's TX channel 1 for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t outlink_start_ch1:1;
        /** outlink_restart_ch1 : WT; bitpos: [2]; default: 0;
         *  Configures whether to restart TX channel 1 for AHB_DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t outlink_restart_ch1:1;
        /** outlink_park_ch1 : RO; bitpos: [3]; default: 1;
         *  Represents the status of the transmit descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t outlink_park_ch1:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_link_ch1_reg_t;

/** Type of out_state_ch1 register
 *  Transmit status of TX channel 1
 */
typedef union {
    struct {
        /** outlink_dscr_addr_ch1 : RO; bitpos: [17:0]; default: 0;
         *  Represents the lower 18 bits of the address of the next transmit descriptor to be
         *  processed.
         */
        uint32_t outlink_dscr_addr_ch1:18;
        /** out_dscr_state_ch1 : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t out_dscr_state_ch1:2;
        /** out_state_ch1 : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t out_state_ch1:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_out_state_ch1_reg_t;

/** Type of out_eof_des_addr_ch1 register
 *  Transmit descriptor address when EOF occurs on TX channel 1
 */
typedef union {
    struct {
        /** out_eof_des_addr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_ch1:32;
    };
    uint32_t val;
} ahb_dma_out_eof_des_addr_ch1_reg_t;

/** Type of out_eof_bfr_des_addr_ch1 register
 *  The last transmit descriptor address when EOF occurs on TX channel 1
 */
typedef union {
    struct {
        /** out_eof_bfr_des_addr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor before the last transmit
         *  descriptor.
         */
        uint32_t out_eof_bfr_des_addr_ch1:32;
    };
    uint32_t val;
} ahb_dma_out_eof_bfr_des_addr_ch1_reg_t;

/** Type of out_dscr_ch1 register
 *  Current transmit descriptor address of TX channel 1
 */
typedef union {
    struct {
        /** outlink_dscr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next transmit descriptor y+1 pointed by the current
         *  transmit descriptor that has already been fetched.
         */
        uint32_t outlink_dscr_ch1:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_ch1_reg_t;

/** Type of out_dscr_bf0_ch1 register
 *  The last transmit descriptor address of TX channel 1
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current transmit descriptor y that has already been
         *  fetched.
         */
        uint32_t outlink_dscr_bf0_ch1:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf0_ch1_reg_t;

/** Type of out_dscr_bf1_ch1 register
 *  The second-to-last transmit descriptor address of TX channel 1
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous transmit descriptor y-1 that has already
         *  been fetched.
         */
        uint32_t outlink_dscr_bf1_ch1:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf1_ch1_reg_t;

/** Type of out_peri_ch1 register
 *  Priority register of TX channel 1
 */
typedef union {
    struct {
        /** tx_pri_ch1 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of TX channel 1.The larger of the value, the higher of the
         *  priority..
         */
        uint32_t tx_pri_ch1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_peri_ch1_reg_t;

/** Type of out_peri_sel_ch1 register
 *  Peripheral selection register of TX channel 1
 */
typedef union {
    struct {
        /** peri_out_sel_ch1 : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to TX channel 1.
         *  0: I3C
         *  1: Dummy
         *  2: UHCI0
         *  3: I2S0
         *  4: I2S1
         *  5: I2S2
         *  6: Dummy
         *  7: Dummy
         *  8: ADC_DAC
         *  9: Dummy
         *  10: RMT
         *  11~15: Dummy
         */
        uint32_t peri_out_sel_ch1:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_out_peri_sel_ch1_reg_t;

/** Type of in_conf0_ch2 register
 *  Configuration register 0 of RX channel 2
 */
typedef union {
    struct {
        /** in_rst_ch2 : R/W; bitpos: [0]; default: 0;
         *  Write 1 and then 0 to reset AHB_DMA channel 2 RX FSM and RX FIFO pointer.
         */
        uint32_t in_rst_ch2:1;
        /** in_loop_test_ch2 : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t in_loop_test_ch2:1;
        /** indscr_burst_en_ch2 : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable INCR burst transfer for RX channel 2 to read
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t indscr_burst_en_ch2:1;
        /** in_data_burst_en_ch2 : R/W; bitpos: [3]; default: 0;
         *  Set this bit to 1 to enable INCR4 burst transfer for Rx channel 2 receiving data
         *  when accessing internal SRAM.
         */
        uint32_t in_data_burst_en_ch2:1;
        /** mem_trans_en_ch2 : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable memory-to-memory data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t mem_trans_en_ch2:1;
        /** in_etm_en_ch2 : R/W; bitpos: [5]; default: 0;
         *  Configures whether to enable ETM control for RX channel2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t in_etm_en_ch2:1;
        /** in_data_burst_mode_sel_ch2 : R/W; bitpos: [7:6]; default: 1;
         *  Configures max burst size for Rx channel2.
         *  2'b00: single
         *  2'b01: incr4
         *  2'b10: incr8
         *  2'b11: incr16
         */
        uint32_t in_data_burst_mode_sel_ch2:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_conf0_ch2_reg_t;

/** Type of in_conf1_ch2 register
 *  Configuration register 1 of RX channel 2
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** in_check_owner_ch2 : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for RX channel 2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t in_check_owner_ch2:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_conf1_ch2_reg_t;

/** Type of infifo_status_ch2 register
 *  Receive FIFO status of RX channel 2
 */
typedef union {
    struct {
        /** infifo_full_ch2 : RO; bitpos: [0]; default: 1;
         *  Represents whether L1 RX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t infifo_full_ch2:1;
        /** infifo_empty_ch2 : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 RX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t infifo_empty_ch2:1;
        uint32_t reserved_2:6;
        /** infifo_cnt_ch2 : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 RX FIFO for RX channel 2
         */
        uint32_t infifo_cnt_ch2:7;
        uint32_t reserved_15:8;
        /** in_remain_under_1b_ch2 : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_1b_ch2:1;
        /** in_remain_under_2b_ch2 : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_2b_ch2:1;
        /** in_remain_under_3b_ch2 : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_3b_ch2:1;
        /** in_remain_under_4b_ch2 : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_4b_ch2:1;
        /** in_buf_hungry_ch2 : RO; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t in_buf_hungry_ch2:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ahb_dma_infifo_status_ch2_reg_t;

/** Type of in_pop_ch2 register
 *  Receive FIFO status of RX channel 2
 */
typedef union {
    struct {
        /** infifo_rdata_ch2 : RO; bitpos: [11:0]; default: 2048;
         *  Represents the data popped from AHB_DMA FIFO.
         */
        uint32_t infifo_rdata_ch2:12;
        /** infifo_pop_ch2 : WT; bitpos: [12]; default: 0;
         *  Configures whether to pop data from AHB_DMA FIFO.
         *  0: Invalid. No effect
         *  1: Pop
         */
        uint32_t infifo_pop_ch2:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_pop_ch2_reg_t;

/** Type of in_link_ch2 register
 *  Receive FIFO status of RX channel 2
 */
typedef union {
    struct {
        /** inlink_auto_ret_ch2 : R/W; bitpos: [0]; default: 1;
         *  Configures whether to return to current receive descriptor's address when there are
         *  some errors in current receiving data.
         *  0: Not return
         *  1: Return
         *  .
         */
        uint32_t inlink_auto_ret_ch2:1;
        /** inlink_stop_ch2 : WT; bitpos: [1]; default: 0;
         *  Configures whether to stop AHB_DMA's RX channel 2 from receiving data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t inlink_stop_ch2:1;
        /** inlink_start_ch2 : WT; bitpos: [2]; default: 0;
         *  Configures whether to enable AHB_DMA's RX channel 2 for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t inlink_start_ch2:1;
        /** inlink_restart_ch2 : WT; bitpos: [3]; default: 0;
         *  Configures whether to restart RX channel 2 for AHB_DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t inlink_restart_ch2:1;
        /** inlink_park_ch2 : RO; bitpos: [4]; default: 1;
         *  Represents the status of the receive descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t inlink_park_ch2:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} ahb_dma_in_link_ch2_reg_t;

/** Type of in_state_ch2 register
 *  Receive status of RX channel 2
 */
typedef union {
    struct {
        /** inlink_dscr_addr_ch2 : RO; bitpos: [17:0]; default: 0;
         *  reserved
         */
        uint32_t inlink_dscr_addr_ch2:18;
        /** in_dscr_state_ch2 : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t in_dscr_state_ch2:2;
        /** in_state_ch2 : RO; bitpos: [22:20]; default: 0;
         *  Represents the address of the lower 18 bits of the next receive descriptor to be
         *  processed.
         */
        uint32_t in_state_ch2:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_in_state_ch2_reg_t;

/** Type of in_suc_eof_des_addr_ch2 register
 *  Receive descriptor address when EOF occurs on RX channel 2
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_ch2:32;
    };
    uint32_t val;
} ahb_dma_in_suc_eof_des_addr_ch2_reg_t;

/** Type of in_err_eof_des_addr_ch2 register
 *  Receive descriptor address when errors occur of RX channel 2
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when there are some errors in the
         *  currently received data.
         */
        uint32_t in_err_eof_des_addr_ch2:32;
    };
    uint32_t val;
} ahb_dma_in_err_eof_des_addr_ch2_reg_t;

/** Type of in_dscr_ch2 register
 *  Current receive descriptor address of RX channel 2
 */
typedef union {
    struct {
        /** inlink_dscr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next receive descriptor x+1 pointed by the current
         *  receive descriptor that has already been fetched.
         */
        uint32_t inlink_dscr_ch2:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_ch2_reg_t;

/** Type of in_dscr_bf0_ch2 register
 *  The last receive descriptor address of RX channel 2
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current receive descriptor x that has already been
         *  fetched.
         */
        uint32_t inlink_dscr_bf0_ch2:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf0_ch2_reg_t;

/** Type of in_dscr_bf1_ch2 register
 *  The second-to-last receive descriptor address of RX channel 2
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous receive descriptor x-1 that has already been
         *  fetched.
         */
        uint32_t inlink_dscr_bf1_ch2:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf1_ch2_reg_t;

/** Type of in_peri_ch2 register
 *  Priority register of RX channel 2
 */
typedef union {
    struct {
        /** rx_pri_ch2 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of RX channel 2.The larger of the value, the higher of the
         *  priority..
         */
        uint32_t rx_pri_ch2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_in_peri_ch2_reg_t;

/** Type of in_peri_sel_ch2 register
 *  Peripheral selection register of RX channel 2
 */
typedef union {
    struct {
        /** peri_in_sel_ch2 : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to RX channel 2.
         *  0: I3C
         *  1: Dummy
         *  2: UHCI0
         *  3: I2S0
         *  4: I2S1
         *  5: I2S2
         *  6: Dummy
         *  7: Dummy
         *  8: ADC_DAC
         *  9: Dummy
         *  10: RMT
         *  11~15: Dummy
         */
        uint32_t peri_in_sel_ch2:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_in_peri_sel_ch2_reg_t;

/** Type of out_conf0_ch2 register
 *  Configuration register 0 of TX channel 2
 */
typedef union {
    struct {
        /** out_rst_ch2 : R/W; bitpos: [0]; default: 0;
         *  Configures the reset state of AHB_DMA channel 2 TX FSM and TX FIFO pointer.
         *  0: Release reset
         *  1: Reset
         */
        uint32_t out_rst_ch2:1;
        /** out_loop_test_ch2 : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t out_loop_test_ch2:1;
        /** out_auto_wrback_ch2 : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable automatic outlink write-back when all the data in TX
         *  FIFO has been transmitted.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_auto_wrback_ch2:1;
        /** out_eof_mode_ch2 : R/W; bitpos: [3]; default: 1;
         *  Configures when to generate EOF flag.
         *  0: EOF flag for TX channel 2 is generated when data to be transmitted has been
         *  pushed into FIFO in AHB_DMA.
         *  1: EOF flag for TX channel 2 is generated when data to be transmitted has been
         *  popped from FIFO in AHB_DMA.
         */
        uint32_t out_eof_mode_ch2:1;
        /** outdscr_burst_en_ch2 : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable INCR burst transfer for TX channel 2 reading
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t outdscr_burst_en_ch2:1;
        /** out_data_burst_en_ch2 : R/W; bitpos: [5]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 2 transmitting data
         *  when accessing internal SRAM.
         */
        uint32_t out_data_burst_en_ch2:1;
        /** out_etm_en_ch2 : R/W; bitpos: [6]; default: 0;
         *  Configures whether to enable ETM control for TX channel 2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_etm_en_ch2:1;
        uint32_t reserved_7:1;
        /** out_data_burst_mode_sel_ch2 : R/W; bitpos: [9:8]; default: 1;
         *  Configures max burst size for TX channel2.
         *  2'b00: single
         *  2'b01: incr4
         *  2'b10: incr8
         *  2'b11: incr16
         */
        uint32_t out_data_burst_mode_sel_ch2:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_conf0_ch2_reg_t;

/** Type of out_conf1_ch2 register
 *  Configuration register 1 of TX channel 2
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** out_check_owner_ch2 : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for TX channel 2.
         *  0: Disable
         *  1: Enable
         */
        uint32_t out_check_owner_ch2:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_out_conf1_ch2_reg_t;

/** Type of outfifo_status_ch2 register
 *  Receive FIFO status of RX channel 2
 */
typedef union {
    struct {
        /** outfifo_full_ch2 : RO; bitpos: [0]; default: 0;
         *  Represents whether L1 TX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t outfifo_full_ch2:1;
        /** outfifo_empty_ch2 : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 TX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t outfifo_empty_ch2:1;
        uint32_t reserved_2:6;
        /** outfifo_cnt_ch2 : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 TX FIFO for TX channel 2
         */
        uint32_t outfifo_cnt_ch2:7;
        uint32_t reserved_15:8;
        /** out_remain_under_1b_ch2 : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_1b_ch2:1;
        /** out_remain_under_2b_ch2 : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_2b_ch2:1;
        /** out_remain_under_3b_ch2 : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_3b_ch2:1;
        /** out_remain_under_4b_ch2 : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_4b_ch2:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} ahb_dma_outfifo_status_ch2_reg_t;

/** Type of out_push_ch2 register
 *  Push control register of TX channel 2
 */
typedef union {
    struct {
        /** outfifo_wdata_ch2 : R/W; bitpos: [8:0]; default: 0;
         *  Configures whether to push data into AHB_DMA FIFO.
         *  0: Invalid. No effect
         *  1: Push
         */
        uint32_t outfifo_wdata_ch2:9;
        /** outfifo_push_ch2 : WT; bitpos: [9]; default: 0;
         *  Configures the data that need to be pushed into AHB_DMA FIFO.
         */
        uint32_t outfifo_push_ch2:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_push_ch2_reg_t;

/** Type of out_link_ch2 register
 *  Push control register of TX channel 2
 */
typedef union {
    struct {
        /** outlink_stop_ch2 : WT; bitpos: [0]; default: 0;
         *  Configures whether to stop AHB_DMA's TX channel 2 from transmitting data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t outlink_stop_ch2:1;
        /** outlink_start_ch2 : WT; bitpos: [1]; default: 0;
         *  Configures whether to enable AHB_DMA's TX channel 2 for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t outlink_start_ch2:1;
        /** outlink_restart_ch2 : WT; bitpos: [2]; default: 0;
         *  Configures whether to restart TX channel 2 for AHB_DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t outlink_restart_ch2:1;
        /** outlink_park_ch2 : RO; bitpos: [3]; default: 1;
         *  Represents the status of the transmit descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t outlink_park_ch2:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_link_ch2_reg_t;

/** Type of out_state_ch2 register
 *  Transmit status of TX channel 2
 */
typedef union {
    struct {
        /** outlink_dscr_addr_ch2 : RO; bitpos: [17:0]; default: 0;
         *  Represents the lower 18 bits of the address of the next transmit descriptor to be
         *  processed.
         */
        uint32_t outlink_dscr_addr_ch2:18;
        /** out_dscr_state_ch2 : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t out_dscr_state_ch2:2;
        /** out_state_ch2 : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t out_state_ch2:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_out_state_ch2_reg_t;

/** Type of out_eof_des_addr_ch2 register
 *  Transmit descriptor address when EOF occurs on TX channel 2
 */
typedef union {
    struct {
        /** out_eof_des_addr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_ch2:32;
    };
    uint32_t val;
} ahb_dma_out_eof_des_addr_ch2_reg_t;

/** Type of out_eof_bfr_des_addr_ch2 register
 *  The last transmit descriptor address when EOF occurs on TX channel 2
 */
typedef union {
    struct {
        /** out_eof_bfr_des_addr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor before the last transmit
         *  descriptor.
         */
        uint32_t out_eof_bfr_des_addr_ch2:32;
    };
    uint32_t val;
} ahb_dma_out_eof_bfr_des_addr_ch2_reg_t;

/** Type of out_dscr_ch2 register
 *  Current transmit descriptor address of TX channel 2
 */
typedef union {
    struct {
        /** outlink_dscr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next transmit descriptor y+1 pointed by the current
         *  transmit descriptor that has already been fetched.
         */
        uint32_t outlink_dscr_ch2:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_ch2_reg_t;

/** Type of out_dscr_bf0_ch2 register
 *  The last transmit descriptor address of TX channel 2
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current transmit descriptor y that has already been
         *  fetched.
         */
        uint32_t outlink_dscr_bf0_ch2:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf0_ch2_reg_t;

/** Type of out_dscr_bf1_ch2 register
 *  The second-to-last transmit descriptor address of TX channel 2
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous transmit descriptor y-1 that has already
         *  been fetched.
         */
        uint32_t outlink_dscr_bf1_ch2:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf1_ch2_reg_t;

/** Type of out_peri_ch2 register
 *  Priority register of TX channel 2
 */
typedef union {
    struct {
        /** tx_pri_ch2 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of TX channel 2.The larger of the value, the higher of the
         *  priority..
         */
        uint32_t tx_pri_ch2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_peri_ch2_reg_t;

/** Type of out_peri_sel_ch2 register
 *  Peripheral selection register of TX channel 2
 */
typedef union {
    struct {
        /** peri_out_sel_ch2 : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to TX channel 2.
         *  0: I3C
         *  1: Dummy
         *  2: UHCI0
         *  3: I2S0
         *  4: I2S1
         *  5: I2S2
         *  6: Dummy
         *  7: Dummy
         *  8: ADC_DAC
         *  9: Dummy
         *  10: RMT
         *  11~15: Dummy
         */
        uint32_t peri_out_sel_ch2:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_out_peri_sel_ch2_reg_t;

/** Type of tx_ch_arb_weight_ch0 register
 *  TX channel 0 arbitration weight configuration register
 */
typedef union {
    struct {
        /** tx_arb_weight_value_ch0 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight(i.e the number of tokens) of TX channel0
         */
        uint32_t tx_arb_weight_value_ch0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_tx_ch_arb_weight_ch0_reg_t;

/** Type of tx_arb_weight_opt_dir_ch0 register
 *  TX channel 0 weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** tx_arb_weight_opt_dis_ch0 : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t tx_arb_weight_opt_dis_ch0:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_tx_arb_weight_opt_dir_ch0_reg_t;

/** Type of tx_ch_arb_weight_ch1 register
 *  TX channel 1 arbitration weight configuration register
 */
typedef union {
    struct {
        /** tx_arb_weight_value_ch1 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight(i.e the number of tokens) of TX channel1
         */
        uint32_t tx_arb_weight_value_ch1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_tx_ch_arb_weight_ch1_reg_t;

/** Type of tx_arb_weight_opt_dir_ch1 register
 *  TX channel 1 weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** tx_arb_weight_opt_dis_ch1 : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t tx_arb_weight_opt_dis_ch1:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_tx_arb_weight_opt_dir_ch1_reg_t;

/** Type of tx_ch_arb_weight_ch2 register
 *  TX channel 2 arbitration weight configuration register
 */
typedef union {
    struct {
        /** tx_arb_weight_value_ch2 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight(i.e the number of tokens) of TX channel2
         */
        uint32_t tx_arb_weight_value_ch2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_tx_ch_arb_weight_ch2_reg_t;

/** Type of tx_arb_weight_opt_dir_ch2 register
 *  TX channel 2 weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** tx_arb_weight_opt_dis_ch2 : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t tx_arb_weight_opt_dis_ch2:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_tx_arb_weight_opt_dir_ch2_reg_t;

/** Type of rx_ch_arb_weight_ch0 register
 *  RX channel 0 arbitration weight configuration register
 */
typedef union {
    struct {
        /** rx_arb_weight_value_ch0 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight(i.e the number of tokens) of RX channel0
         */
        uint32_t rx_arb_weight_value_ch0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_rx_ch_arb_weight_ch0_reg_t;

/** Type of rx_arb_weight_opt_dir_ch0 register
 *  RX channel 0 weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** rx_arb_weight_opt_dis_ch0 : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t rx_arb_weight_opt_dis_ch0:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_rx_arb_weight_opt_dir_ch0_reg_t;

/** Type of rx_ch_arb_weight_ch1 register
 *  RX channel 1 arbitration weight configuration register
 */
typedef union {
    struct {
        /** rx_arb_weight_value_ch1 : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight(i.e the number of tokens) of RX channel1
         */
        uint32_t rx_arb_weight_value_ch1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_rx_ch_arb_weight_ch1_reg_t;

/** Type of rx_arb_weight_opt_dir_ch1 register
 *  RX channel 1 weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** rx_arb_weight_opt_dis_ch1 : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t rx_arb_weight_opt_dis_ch1:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_rx_arb_weight_opt_dir_ch1_reg_t;

/** Type of in_link_addr_ch0 register
 *  Link list descriptor address configuration of RX channel 0
 */
typedef union {
    struct {
        /** inlink_addr_ch0 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 32 bits of the first receive descriptor's address
         */
        uint32_t inlink_addr_ch0:32;
    };
    uint32_t val;
} ahb_dma_in_link_addr_ch0_reg_t;

/** Type of in_link_addr_ch1 register
 *  Link list descriptor address configuration of RX channel 1
 */
typedef union {
    struct {
        /** inlink_addr_ch1 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 32 bits of the first receive descriptor's address
         */
        uint32_t inlink_addr_ch1:32;
    };
    uint32_t val;
} ahb_dma_in_link_addr_ch1_reg_t;

/** Type of in_link_addr_ch2 register
 *  Link list descriptor address configuration of RX channel 2
 */
typedef union {
    struct {
        /** inlink_addr_ch2 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 32 bits of the first receive descriptor's address
         */
        uint32_t inlink_addr_ch2:32;
    };
    uint32_t val;
} ahb_dma_in_link_addr_ch2_reg_t;

/** Type of out_link_addr_ch0 register
 *  Link list descriptor address configuration of TX channel 0
 */
typedef union {
    struct {
        /** outlink_addr_ch0 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 32 bits of the first receive descriptor's address.
         */
        uint32_t outlink_addr_ch0:32;
    };
    uint32_t val;
} ahb_dma_out_link_addr_ch0_reg_t;

/** Type of out_link_addr_ch1 register
 *  Link list descriptor address configuration of TX channel 1
 */
typedef union {
    struct {
        /** outlink_addr_ch1 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 32 bits of the first receive descriptor's address.
         */
        uint32_t outlink_addr_ch1:32;
    };
    uint32_t val;
} ahb_dma_out_link_addr_ch1_reg_t;

/** Type of out_link_addr_ch2 register
 *  Link list descriptor address configuration of TX channel 2
 */
typedef union {
    struct {
        /** outlink_addr_ch2 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 32 bits of the first receive descriptor's address.
         */
        uint32_t outlink_addr_ch2:32;
    };
    uint32_t val;
} ahb_dma_out_link_addr_ch2_reg_t;

/** Type of intr_mem_start_addr register
 *  Accessible address space start address configuration register
 */
typedef union {
    struct {
        /** access_intr_mem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  Accessible address space start address configuration register
         */
        uint32_t access_intr_mem_start_addr:32;
    };
    uint32_t val;
} ahb_dma_intr_mem_start_addr_reg_t;

/** Type of intr_mem_end_addr register
 *  Accessible address space end address configuration register
 */
typedef union {
    struct {
        /** access_intr_mem_end_addr : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the end address of accessible address space.
         */
        uint32_t access_intr_mem_end_addr:32;
    };
    uint32_t val;
} ahb_dma_intr_mem_end_addr_reg_t;

/** Type of arb_timeout register
 *  TX arbitration timeout configuration register
 */
typedef union {
    struct {
        /** arb_timeout_num : R/W; bitpos: [15:0]; default: 0;
         *  Configures the time slot. Measurement unit: AHB bus clock cycle.
         */
        uint32_t arb_timeout_num:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} ahb_dma_arb_timeout_reg_t;

/** Type of weight_en register
 *  TX weight arbitration enable register
 */
typedef union {
    struct {
        /** weight_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable weight arbitration.
         *  0: Disable
         *  1: Enable
         */
        uint32_t weight_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_weight_en_reg_t;

/** Type of module_clk_en register
 *  Module clock force on register
 */
typedef union {
    struct {
        /** ahb_apb_sync_clk_en : R/W; bitpos: [2:0]; default: 7;
         *  Configures whether to force on ahb_apb_sync 2~0 module clock. For bit n:
         *  0 : Not force on ahb_apb_sync n clock
         *  1 : Force on ahb_apb_sync n clock
         */
        uint32_t ahb_apb_sync_clk_en:3;
        /** out_dscr_clk_en : R/W; bitpos: [5:3]; default: 7;
         *  Configures whether to force on out_dscr 2~0 module clock. For bit n:
         *  0 : Not force on out_dscr n clock
         *  1 : Force on out_dscr n clock
         */
        uint32_t out_dscr_clk_en:3;
        /** out_ctrl_clk_en : R/W; bitpos: [8:6]; default: 7;
         *  Configures whether to force on out_ctrl 2~0 module clock. For bit n:
         *  0 : Not force on out_ctrl n clock
         *  1 : Force on out_ctrl n clock
         */
        uint32_t out_ctrl_clk_en:3;
        /** in_dscr_clk_en : R/W; bitpos: [11:9]; default: 7;
         *  Configures whether to force on in_dscr 2~0 module clock. For bit n:
         *  0 : Not force on in_dscr n clock
         *  1 : Force on in_dscr n clock
         */
        uint32_t in_dscr_clk_en:3;
        /** in_ctrl_clk_en : R/W; bitpos: [14:12]; default: 7;
         *  Configures whether to force on in_ctrl 2~0 module clock. For bit n:
         *  0 : Not force on in_ctrl n clock
         *  1 : Force on in_ctrl n clock
         */
        uint32_t in_ctrl_clk_en:3;
        uint32_t reserved_15:12;
        /** cmd_arb_clk_en : R/W; bitpos: [27]; default: 0;
         *  Configures whether to force on cmd_arb module clock.
         *  0 : Not force on cmd_arb clock
         *  1 : Force on cmd_arb clock
         */
        uint32_t cmd_arb_clk_en:1;
        /** ahbinf_clk_en : R/W; bitpos: [28]; default: 0;
         *  Configures whether to force on ahbinf module clock.
         *  0 : Not force on ahbinf clock
         *  1 : Force on ahbinf clock
         */
        uint32_t ahbinf_clk_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} ahb_dma_module_clk_en_reg_t;

/** Type of ahbinf_resp_err_status0 register
 *  AHB response error status 0 register
 */
typedef union {
    struct {
        /** ahbinf_resp_err_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the AHB response error.
         */
        uint32_t ahbinf_resp_err_addr:32;
    };
    uint32_t val;
} ahb_dma_ahbinf_resp_err_status0_reg_t;

/** Type of ahbinf_resp_err_status1 register
 *  AHB response error status 1 register
 */
typedef union {
    struct {
        /** ahbinf_resp_err_wr : RO; bitpos: [0]; default: 0;
         *  Represents the AHB response error is write request.
         */
        uint32_t ahbinf_resp_err_wr:1;
        /** ahbinf_resp_err_id : RO; bitpos: [4:1]; default: 15;
         *  Represents the AHB response error request id.
         */
        uint32_t ahbinf_resp_err_id:4;
        /** ahbinf_resp_err_ch_id : RO; bitpos: [7:5]; default: 0;
         *  Represents the AHB response error request channel id.bit[2]=1:TX channel.
         *  bit[2]=0:RX channel.
         */
        uint32_t ahbinf_resp_err_ch_id:3;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_ahbinf_resp_err_status1_reg_t;

/** Type of in_done_des_addr_ch0 register
 *  RX_done Inlink descriptor address of RX channel 0
 */
typedef union {
    struct {
        /** in_done_des_addr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the inlink descriptor when  this descriptor is completed .
         */
        uint32_t in_done_des_addr_ch0:32;
    };
    uint32_t val;
} ahb_dma_in_done_des_addr_ch0_reg_t;

/** Type of out_done_des_addr_ch0 register
 *  TX done outlink descriptor address of TX channel 0
 */
typedef union {
    struct {
        /** out_done_des_addr_ch0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the outlink descriptor when this descriptor is completed.
         */
        uint32_t out_done_des_addr_ch0:32;
    };
    uint32_t val;
} ahb_dma_out_done_des_addr_ch0_reg_t;

/** Type of in_done_des_addr_ch1 register
 *  RX_done Inlink descriptor address of RX channel 1
 */
typedef union {
    struct {
        /** in_done_des_addr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the inlink descriptor when  this descriptor is completed .
         */
        uint32_t in_done_des_addr_ch1:32;
    };
    uint32_t val;
} ahb_dma_in_done_des_addr_ch1_reg_t;

/** Type of out_done_des_addr_ch1 register
 *  TX done outlink descriptor address of TX channel 1
 */
typedef union {
    struct {
        /** out_done_des_addr_ch1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the outlink descriptor when this descriptor is completed.
         */
        uint32_t out_done_des_addr_ch1:32;
    };
    uint32_t val;
} ahb_dma_out_done_des_addr_ch1_reg_t;

/** Type of in_done_des_addr_ch2 register
 *  RX_done Inlink descriptor address of RX channel 2
 */
typedef union {
    struct {
        /** in_done_des_addr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the inlink descriptor when  this descriptor is completed .
         */
        uint32_t in_done_des_addr_ch2:32;
    };
    uint32_t val;
} ahb_dma_in_done_des_addr_ch2_reg_t;

/** Type of out_done_des_addr_ch2 register
 *  TX done outlink descriptor address of TX channel 2
 */
typedef union {
    struct {
        /** out_done_des_addr_ch2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the outlink descriptor when this descriptor is completed.
         */
        uint32_t out_done_des_addr_ch2:32;
    };
    uint32_t val;
} ahb_dma_out_done_des_addr_ch2_reg_t;


/** Group: Configuration Registers */
/** Type of out_crc_init_data_chn register
 *  This register is used to config chn crc initial data(max 32 bit)
 */
typedef union {
    struct {
        /** out_crc_init_data_chn : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is used to config ch0 of tx crc initial value
         */
        uint32_t out_crc_init_data_chn:32;
    };
    uint32_t val;
} ahb_dma_out_crc_init_data_chn_reg_t;

/** Type of tx_crc_width_chn register
 *  This register is used to confiig tx chn crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
typedef union {
    struct {
        /** tx_crc_width_chn : R/W; bitpos: [1:0]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_width_chn:2;
        /** tx_crc_lautch_flga_chn : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_lautch_flga_chn:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} ahb_dma_tx_crc_width_chn_reg_t;

/** Type of out_crc_clear_chn register
 *  This register is used to clear chn crc result
 */
typedef union {
    struct {
        /** out_crc_clear_chn_reg : R/W; bitpos: [0]; default: 0;
         *  This register is used to clear ch0 of tx crc result
         */
        uint32_t out_crc_clear_chn_reg:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_out_crc_clear_chn_reg_t;

/** Type of out_crc_final_result_chn register
 *  This register is used to store chn crc result
 */
typedef union {
    struct {
        /** out_crc_final_result_chn : RO; bitpos: [31:0]; default: 0;
         *  This register is used to store result ch0 of tx
         */
        uint32_t out_crc_final_result_chn:32;
    };
    uint32_t val;
} ahb_dma_out_crc_final_result_chn_reg_t;

/** Type of tx_crc_en_wr_data_chn register
 *  This resister is used to config chn crc en for every bit
 */
typedef union {
    struct {
        /** tx_crc_en_wr_data_chn : R/W; bitpos: [31:0]; default: 0;
         *  This register is used to enable tx ch0 crc 32bit on/off
         */
        uint32_t tx_crc_en_wr_data_chn:32;
    };
    uint32_t val;
} ahb_dma_tx_crc_en_wr_data_chn_reg_t;

/** Type of tx_crc_en_addr_chn register
 *  This register is used to config chn crc en addr
 */
typedef union {
    struct {
        /** tx_crc_en_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_en_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_tx_crc_en_addr_chn_reg_t;

/** Type of tx_crc_data_en_wr_data_chn register
 *  This register is used to config crc data_8bit en
 */
typedef union {
    struct {
        /** tx_crc_data_en_wr_data_chn : R/W; bitpos: [7:0]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_data_en_wr_data_chn:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_tx_crc_data_en_wr_data_chn_reg_t;

/** Type of tx_crc_data_en_addr_chn register
 *  This register is used to config addr of crc data_8bit en
 */
typedef union {
    struct {
        /** tx_crc_data_en_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_data_en_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_tx_crc_data_en_addr_chn_reg_t;

/** Type of in_crc_init_data_chn register
 *  This register is used to config chn crc initial data(max 32 bit)
 */
typedef union {
    struct {
        /** in_crc_init_data_chn : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is used to config ch0 of rx crc initial value
         */
        uint32_t in_crc_init_data_chn:32;
    };
    uint32_t val;
} ahb_dma_in_crc_init_data_chn_reg_t;

/** Type of rx_crc_width_chn register
 *  This register is used to confiig rx chn crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
typedef union {
    struct {
        /** rx_crc_width_chn : R/W; bitpos: [1:0]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_width_chn:2;
        /** rx_crc_lautch_flga_chn : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_lautch_flga_chn:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} ahb_dma_rx_crc_width_chn_reg_t;

/** Type of in_crc_clear_chn register
 *  This register is used to clear chn crc result
 */
typedef union {
    struct {
        /** in_crc_clear_chn_reg : R/W; bitpos: [0]; default: 0;
         *  This register is used to clear ch0 of rx crc result
         */
        uint32_t in_crc_clear_chn_reg:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_in_crc_clear_chn_reg_t;

/** Type of in_crc_final_result_chn register
 *  This register is used to store chn crc result
 */
typedef union {
    struct {
        /** in_crc_final_result_chn : RO; bitpos: [31:0]; default: 0;
         *  This register is used to store result ch0 of rx
         */
        uint32_t in_crc_final_result_chn:32;
    };
    uint32_t val;
} ahb_dma_in_crc_final_result_chn_reg_t;

/** Type of rx_crc_en_wr_data_chn register
 *  This resister is used to config chn crc en for every bit
 */
typedef union {
    struct {
        /** rx_crc_en_wr_data_chn : R/W; bitpos: [31:0]; default: 0;
         *  This register is used to enable rx ch0 crc 32bit on/off
         */
        uint32_t rx_crc_en_wr_data_chn:32;
    };
    uint32_t val;
} ahb_dma_rx_crc_en_wr_data_chn_reg_t;

/** Type of rx_crc_en_addr_chn register
 *  This register is used to config chn crc en addr
 */
typedef union {
    struct {
        /** rx_crc_en_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_en_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_rx_crc_en_addr_chn_reg_t;

/** Type of rx_crc_data_en_wr_data_chn register
 *  This register is used to config crc data_8bit en
 */
typedef union {
    struct {
        /** rx_crc_data_en_wr_data_chn : R/W; bitpos: [7:0]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_data_en_wr_data_chn:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_rx_crc_data_en_wr_data_chn_reg_t;

/** Type of rx_crc_data_en_addr_chn register
 *  This register is used to config addr of crc data_8bit en
 */
typedef union {
    struct {
        /** rx_crc_data_en_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_data_en_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_rx_crc_data_en_addr_chn_reg_t;

/** Type of rx_ch_arb_weigh_chn register
 *  RX channel 2 arbitration weight configuration register
 */
typedef union {
    struct {
        /** rx_ch_arb_weigh_chn : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight(i.e the number of tokens) of RX channeln
         */
        uint32_t rx_ch_arb_weigh_chn:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_rx_ch_arb_weigh_chn_reg_t;

/** Type of rx_arb_weigh_opt_dir_chn register
 *  RX channel 2 weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** rx_arb_weigh_opt_dir_chn : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t rx_arb_weigh_opt_dir_chn:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t;

/** Type of arb_timeout_tx register
 *  TX arbitration timeout configuration register
 */
typedef union {
    struct {
        /** arb_timeout_tx : R/W; bitpos: [15:0]; default: 0;
         *  Configures the time slot for TX. Measurement unit: AHB bus clock cycle.
         */
        uint32_t arb_timeout_tx:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} ahb_dma_arb_timeout_tx_reg_t;

/** Type of arb_timeout_rx register
 *  RX arbitration timeout configuration register
 */
typedef union {
    struct {
        /** arb_timeout_rx : R/W; bitpos: [15:0]; default: 0;
         *  Configures the time slot for RX. Measurement unit: AHB bus clock cycle.
         */
        uint32_t arb_timeout_rx:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} ahb_dma_arb_timeout_rx_reg_t;

/** Type of weight_en_tx register
 *  TX weight arbitration enable register
 */
typedef union {
    struct {
        /** weight_en_tx : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable weight arbitration for TX.
         *  0: Disable
         *  1: Enable
         */
        uint32_t weight_en_tx:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_weight_en_tx_reg_t;

/** Type of weight_en_rx register
 *  RX weight arbitration enable register
 */
typedef union {
    struct {
        /** weight_en_rx : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable weight arbitration for RX.
         *  0: Disable
         *  1: Enable
         */
        uint32_t weight_en_rx:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_weight_en_rx_reg_t;


typedef struct {
    volatile ahb_dma_in_int_raw_ch0_reg_t in_int_raw_ch0;
    volatile ahb_dma_in_int_st_ch0_reg_t in_int_st_ch0;
    volatile ahb_dma_in_int_ena_ch0_reg_t in_int_ena_ch0;
    volatile ahb_dma_in_int_clr_ch0_reg_t in_int_clr_ch0;
    volatile ahb_dma_in_int_raw_ch1_reg_t in_int_raw_ch1;
    volatile ahb_dma_in_int_st_ch1_reg_t in_int_st_ch1;
    volatile ahb_dma_in_int_ena_ch1_reg_t in_int_ena_ch1;
    volatile ahb_dma_in_int_clr_ch1_reg_t in_int_clr_ch1;
    volatile ahb_dma_in_int_raw_ch2_reg_t in_int_raw_ch2;
    volatile ahb_dma_in_int_st_ch2_reg_t in_int_st_ch2;
    volatile ahb_dma_in_int_ena_ch2_reg_t in_int_ena_ch2;
    volatile ahb_dma_in_int_clr_ch2_reg_t in_int_clr_ch2;
    volatile ahb_dma_out_int_raw_ch0_reg_t out_int_raw_ch0;
    volatile ahb_dma_out_int_st_ch0_reg_t out_int_st_ch0;
    volatile ahb_dma_out_int_ena_ch0_reg_t out_int_ena_ch0;
    volatile ahb_dma_out_int_clr_ch0_reg_t out_int_clr_ch0;
    volatile ahb_dma_out_int_raw_ch1_reg_t out_int_raw_ch1;
    volatile ahb_dma_out_int_st_ch1_reg_t out_int_st_ch1;
    volatile ahb_dma_out_int_ena_ch1_reg_t out_int_ena_ch1;
    volatile ahb_dma_out_int_clr_ch1_reg_t out_int_clr_ch1;
    volatile ahb_dma_out_int_raw_ch2_reg_t out_int_raw_ch2;
    volatile ahb_dma_out_int_st_ch2_reg_t out_int_st_ch2;
    volatile ahb_dma_out_int_ena_ch2_reg_t out_int_ena_ch2;
    volatile ahb_dma_out_int_clr_ch2_reg_t out_int_clr_ch2;
    volatile ahb_dma_ahb_test_reg_t ahb_test;
    volatile ahb_dma_misc_conf_reg_t misc_conf;
    volatile ahb_dma_date_reg_t date;
    uint32_t reserved_06c;
    volatile ahb_dma_in_conf0_ch0_reg_t in_conf0_ch0;
    volatile ahb_dma_in_conf1_ch0_reg_t in_conf1_ch0;
    volatile ahb_dma_infifo_status_ch0_reg_t infifo_status_ch0;
    volatile ahb_dma_in_pop_ch0_reg_t in_pop_ch0;
    volatile ahb_dma_in_link_ch0_reg_t in_link_ch0;
    volatile ahb_dma_in_state_ch0_reg_t in_state_ch0;
    volatile ahb_dma_in_suc_eof_des_addr_ch0_reg_t in_suc_eof_des_addr_ch0;
    volatile ahb_dma_in_err_eof_des_addr_ch0_reg_t in_err_eof_des_addr_ch0;
    volatile ahb_dma_in_dscr_ch0_reg_t in_dscr_ch0;
    volatile ahb_dma_in_dscr_bf0_ch0_reg_t in_dscr_bf0_ch0;
    volatile ahb_dma_in_dscr_bf1_ch0_reg_t in_dscr_bf1_ch0;
    volatile ahb_dma_in_peri_ch0_reg_t in_peri_ch0;
    volatile ahb_dma_in_peri_sel_ch0_reg_t in_peri_sel_ch0;
    uint32_t reserved_0a4[11];
    volatile ahb_dma_out_conf0_ch0_reg_t out_conf0_ch0;
    volatile ahb_dma_out_conf1_ch0_reg_t out_conf1_ch0;
    volatile ahb_dma_outfifo_status_ch0_reg_t outfifo_status_ch0;
    volatile ahb_dma_out_push_ch0_reg_t out_push_ch0;
    volatile ahb_dma_out_link_ch0_reg_t out_link_ch0;
    volatile ahb_dma_out_state_ch0_reg_t out_state_ch0;
    volatile ahb_dma_out_eof_des_addr_ch0_reg_t out_eof_des_addr_ch0;
    volatile ahb_dma_out_eof_bfr_des_addr_ch0_reg_t out_eof_bfr_des_addr_ch0;
    volatile ahb_dma_out_dscr_ch0_reg_t out_dscr_ch0;
    volatile ahb_dma_out_dscr_bf0_ch0_reg_t out_dscr_bf0_ch0;
    volatile ahb_dma_out_dscr_bf1_ch0_reg_t out_dscr_bf1_ch0;
    volatile ahb_dma_out_peri_ch0_reg_t out_peri_ch0;
    volatile ahb_dma_out_peri_sel_ch0_reg_t out_peri_sel_ch0;
    uint32_t reserved_104[11];
    volatile ahb_dma_in_conf0_ch1_reg_t in_conf0_ch1;
    volatile ahb_dma_in_conf1_ch1_reg_t in_conf1_ch1;
    volatile ahb_dma_infifo_status_ch1_reg_t infifo_status_ch1;
    volatile ahb_dma_in_pop_ch1_reg_t in_pop_ch1;
    volatile ahb_dma_in_link_ch1_reg_t in_link_ch1;
    volatile ahb_dma_in_state_ch1_reg_t in_state_ch1;
    volatile ahb_dma_in_suc_eof_des_addr_ch1_reg_t in_suc_eof_des_addr_ch1;
    volatile ahb_dma_in_err_eof_des_addr_ch1_reg_t in_err_eof_des_addr_ch1;
    volatile ahb_dma_in_dscr_ch1_reg_t in_dscr_ch1;
    volatile ahb_dma_in_dscr_bf0_ch1_reg_t in_dscr_bf0_ch1;
    volatile ahb_dma_in_dscr_bf1_ch1_reg_t in_dscr_bf1_ch1;
    volatile ahb_dma_in_peri_ch1_reg_t in_peri_ch1;
    volatile ahb_dma_in_peri_sel_ch1_reg_t in_peri_sel_ch1;
    uint32_t reserved_164[11];
    volatile ahb_dma_out_conf0_ch1_reg_t out_conf0_ch1;
    volatile ahb_dma_out_conf1_ch1_reg_t out_conf1_ch1;
    volatile ahb_dma_outfifo_status_ch1_reg_t outfifo_status_ch1;
    volatile ahb_dma_out_push_ch1_reg_t out_push_ch1;
    volatile ahb_dma_out_link_ch1_reg_t out_link_ch1;
    volatile ahb_dma_out_state_ch1_reg_t out_state_ch1;
    volatile ahb_dma_out_eof_des_addr_ch1_reg_t out_eof_des_addr_ch1;
    volatile ahb_dma_out_eof_bfr_des_addr_ch1_reg_t out_eof_bfr_des_addr_ch1;
    volatile ahb_dma_out_dscr_ch1_reg_t out_dscr_ch1;
    volatile ahb_dma_out_dscr_bf0_ch1_reg_t out_dscr_bf0_ch1;
    volatile ahb_dma_out_dscr_bf1_ch1_reg_t out_dscr_bf1_ch1;
    volatile ahb_dma_out_peri_ch1_reg_t out_peri_ch1;
    volatile ahb_dma_out_peri_sel_ch1_reg_t out_peri_sel_ch1;
    uint32_t reserved_1c4[11];
    volatile ahb_dma_in_conf0_ch2_reg_t in_conf0_ch2;
    volatile ahb_dma_in_conf1_ch2_reg_t in_conf1_ch2;
    volatile ahb_dma_infifo_status_ch2_reg_t infifo_status_ch2;
    volatile ahb_dma_in_pop_ch2_reg_t in_pop_ch2;
    volatile ahb_dma_in_link_ch2_reg_t in_link_ch2;
    volatile ahb_dma_in_state_ch2_reg_t in_state_ch2;
    volatile ahb_dma_in_suc_eof_des_addr_ch2_reg_t in_suc_eof_des_addr_ch2;
    volatile ahb_dma_in_err_eof_des_addr_ch2_reg_t in_err_eof_des_addr_ch2;
    volatile ahb_dma_in_dscr_ch2_reg_t in_dscr_ch2;
    volatile ahb_dma_in_dscr_bf0_ch2_reg_t in_dscr_bf0_ch2;
    volatile ahb_dma_in_dscr_bf1_ch2_reg_t in_dscr_bf1_ch2;
    volatile ahb_dma_in_peri_ch2_reg_t in_peri_ch2;
    volatile ahb_dma_in_peri_sel_ch2_reg_t in_peri_sel_ch2;
    uint32_t reserved_224[11];
    volatile ahb_dma_out_conf0_ch2_reg_t out_conf0_ch2;
    volatile ahb_dma_out_conf1_ch2_reg_t out_conf1_ch2;
    volatile ahb_dma_outfifo_status_ch2_reg_t outfifo_status_ch2;
    volatile ahb_dma_out_push_ch2_reg_t out_push_ch2;
    volatile ahb_dma_out_link_ch2_reg_t out_link_ch2;
    volatile ahb_dma_out_state_ch2_reg_t out_state_ch2;
    volatile ahb_dma_out_eof_des_addr_ch2_reg_t out_eof_des_addr_ch2;
    volatile ahb_dma_out_eof_bfr_des_addr_ch2_reg_t out_eof_bfr_des_addr_ch2;
    volatile ahb_dma_out_dscr_ch2_reg_t out_dscr_ch2;
    volatile ahb_dma_out_dscr_bf0_ch2_reg_t out_dscr_bf0_ch2;
    volatile ahb_dma_out_dscr_bf1_ch2_reg_t out_dscr_bf1_ch2;
    volatile ahb_dma_out_peri_ch2_reg_t out_peri_ch2;
    volatile ahb_dma_out_peri_sel_ch2_reg_t out_peri_sel_ch2;
    uint32_t reserved_284[14];
    volatile ahb_dma_out_crc_init_data_chn_reg_t out_crc_init_data_ch0;
    volatile ahb_dma_tx_crc_width_chn_reg_t tx_crc_width_ch0;
    volatile ahb_dma_out_crc_clear_chn_reg_t out_crc_clear_ch0;
    volatile ahb_dma_out_crc_final_result_chn_reg_t out_crc_final_result_ch0;
    volatile ahb_dma_tx_crc_en_wr_data_chn_reg_t tx_crc_en_wr_data_ch0;
    volatile ahb_dma_tx_crc_en_addr_chn_reg_t tx_crc_en_addr_ch0;
    volatile ahb_dma_tx_crc_data_en_wr_data_chn_reg_t tx_crc_data_en_wr_data_ch0;
    volatile ahb_dma_tx_crc_data_en_addr_chn_reg_t tx_crc_data_en_addr_ch0;
    volatile ahb_dma_tx_ch_arb_weight_ch0_reg_t tx_ch_arb_weight_ch0;
    volatile ahb_dma_tx_arb_weight_opt_dir_ch0_reg_t tx_arb_weight_opt_dir_ch0;
    volatile ahb_dma_out_crc_init_data_chn_reg_t out_crc_init_data_ch1;
    volatile ahb_dma_tx_crc_width_chn_reg_t tx_crc_width_ch1;
    volatile ahb_dma_out_crc_clear_chn_reg_t out_crc_clear_ch1;
    volatile ahb_dma_out_crc_final_result_chn_reg_t out_crc_final_result_ch1;
    volatile ahb_dma_tx_crc_en_wr_data_chn_reg_t tx_crc_en_wr_data_ch1;
    volatile ahb_dma_tx_crc_en_addr_chn_reg_t tx_crc_en_addr_ch1;
    volatile ahb_dma_tx_crc_data_en_wr_data_chn_reg_t tx_crc_data_en_wr_data_ch1;
    volatile ahb_dma_tx_crc_data_en_addr_chn_reg_t tx_crc_data_en_addr_ch1;
    volatile ahb_dma_tx_ch_arb_weight_ch1_reg_t tx_ch_arb_weight_ch1;
    volatile ahb_dma_tx_arb_weight_opt_dir_ch1_reg_t tx_arb_weight_opt_dir_ch1;
    volatile ahb_dma_out_crc_init_data_chn_reg_t out_crc_init_data_ch2;
    volatile ahb_dma_tx_crc_width_chn_reg_t tx_crc_width_ch2;
    volatile ahb_dma_out_crc_clear_chn_reg_t out_crc_clear_ch2;
    volatile ahb_dma_out_crc_final_result_chn_reg_t out_crc_final_result_ch2;
    volatile ahb_dma_tx_crc_en_wr_data_chn_reg_t tx_crc_en_wr_data_ch2;
    volatile ahb_dma_tx_crc_en_addr_chn_reg_t tx_crc_en_addr_ch2;
    volatile ahb_dma_tx_crc_data_en_wr_data_chn_reg_t tx_crc_data_en_wr_data_ch2;
    volatile ahb_dma_tx_crc_data_en_addr_chn_reg_t tx_crc_data_en_addr_ch2;
    volatile ahb_dma_tx_ch_arb_weight_ch2_reg_t tx_ch_arb_weight_ch2;
    volatile ahb_dma_tx_arb_weight_opt_dir_ch2_reg_t tx_arb_weight_opt_dir_ch2;
    volatile ahb_dma_in_crc_init_data_chn_reg_t in_crc_init_data_ch0;
    volatile ahb_dma_rx_crc_width_chn_reg_t rx_crc_width_ch0;
    volatile ahb_dma_in_crc_clear_chn_reg_t in_crc_clear_ch0;
    volatile ahb_dma_in_crc_final_result_chn_reg_t in_crc_final_result_ch0;
    volatile ahb_dma_rx_crc_en_wr_data_chn_reg_t rx_crc_en_wr_data_ch0;
    volatile ahb_dma_rx_crc_en_addr_chn_reg_t rx_crc_en_addr_ch0;
    volatile ahb_dma_rx_crc_data_en_wr_data_chn_reg_t rx_crc_data_en_wr_data_ch0;
    volatile ahb_dma_rx_crc_data_en_addr_chn_reg_t rx_crc_data_en_addr_ch0;
    volatile ahb_dma_rx_ch_arb_weight_ch0_reg_t rx_ch_arb_weight_ch0;
    volatile ahb_dma_rx_arb_weight_opt_dir_ch0_reg_t rx_arb_weight_opt_dir_ch0;
    volatile ahb_dma_in_crc_init_data_chn_reg_t in_crc_init_data_ch1;
    volatile ahb_dma_rx_crc_width_chn_reg_t rx_crc_width_ch1;
    volatile ahb_dma_in_crc_clear_chn_reg_t in_crc_clear_ch1;
    volatile ahb_dma_in_crc_final_result_chn_reg_t in_crc_final_result_ch1;
    volatile ahb_dma_rx_crc_en_wr_data_chn_reg_t rx_crc_en_wr_data_ch1;
    volatile ahb_dma_rx_crc_en_addr_chn_reg_t rx_crc_en_addr_ch1;
    volatile ahb_dma_rx_crc_data_en_wr_data_chn_reg_t rx_crc_data_en_wr_data_ch1;
    volatile ahb_dma_rx_crc_data_en_addr_chn_reg_t rx_crc_data_en_addr_ch1;
    volatile ahb_dma_rx_ch_arb_weight_ch1_reg_t rx_ch_arb_weight_ch1;
    volatile ahb_dma_rx_arb_weight_opt_dir_ch1_reg_t rx_arb_weight_opt_dir_ch1;
    volatile ahb_dma_in_crc_init_data_chn_reg_t in_crc_init_data_ch2;
    volatile ahb_dma_rx_crc_width_chn_reg_t rx_crc_width_ch2;
    volatile ahb_dma_in_crc_clear_chn_reg_t in_crc_clear_ch2;
    volatile ahb_dma_in_crc_final_result_chn_reg_t in_crc_final_result_ch2;
    volatile ahb_dma_rx_crc_en_wr_data_chn_reg_t rx_crc_en_wr_data_ch2;
    volatile ahb_dma_rx_crc_en_addr_chn_reg_t rx_crc_en_addr_ch2;
    volatile ahb_dma_rx_crc_data_en_wr_data_chn_reg_t rx_crc_data_en_wr_data_ch2;
    volatile ahb_dma_rx_crc_data_en_addr_chn_reg_t rx_crc_data_en_addr_ch2;
    volatile ahb_dma_rx_ch_arb_weigh_chn_reg_t rx_ch_arb_weigh_ch2;
    volatile ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t rx_arb_weigh_opt_dir_ch2;
    volatile ahb_dma_in_link_addr_ch0_reg_t in_link_addr_ch0;
    volatile ahb_dma_in_link_addr_ch1_reg_t in_link_addr_ch1;
    volatile ahb_dma_in_link_addr_ch2_reg_t in_link_addr_ch2;
    volatile ahb_dma_out_link_addr_ch0_reg_t out_link_addr_ch0;
    volatile ahb_dma_out_link_addr_ch1_reg_t out_link_addr_ch1;
    volatile ahb_dma_out_link_addr_ch2_reg_t out_link_addr_ch2;
    volatile ahb_dma_intr_mem_start_addr_reg_t intr_mem_start_addr;
    volatile ahb_dma_intr_mem_end_addr_reg_t intr_mem_end_addr;
    volatile ahb_dma_arb_timeout_tx_reg_t arb_timeout_tx;
    volatile ahb_dma_arb_timeout_rx_reg_t arb_timeout_rx;
    volatile ahb_dma_weight_en_tx_reg_t weight_en_tx;
    volatile ahb_dma_weight_en_rx_reg_t weight_en_rx;
    volatile ahb_dma_arb_timeout_reg_t arb_timeout;
    uint32_t reserved_3e0[8];
    volatile ahb_dma_weight_en_reg_t weight_en;
    volatile ahb_dma_module_clk_en_reg_t module_clk_en;
    volatile ahb_dma_ahbinf_resp_err_status0_reg_t ahbinf_resp_err_status0;
    volatile ahb_dma_ahbinf_resp_err_status1_reg_t ahbinf_resp_err_status1;
    volatile ahb_dma_in_done_des_addr_ch0_reg_t in_done_des_addr_ch0;
    volatile ahb_dma_out_done_des_addr_ch0_reg_t out_done_des_addr_ch0;
    volatile ahb_dma_in_done_des_addr_ch1_reg_t in_done_des_addr_ch1;
    volatile ahb_dma_out_done_des_addr_ch1_reg_t out_done_des_addr_ch1;
    volatile ahb_dma_in_done_des_addr_ch2_reg_t in_done_des_addr_ch2;
    volatile ahb_dma_out_done_des_addr_ch2_reg_t out_done_des_addr_ch2;
} ahb_dma_dev_t;

extern ahb_dma_dev_t AHB_DMA;

#ifndef __cplusplus
_Static_assert(sizeof(ahb_dma_dev_t) == 0x428, "Invalid size of ahb_dma_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
