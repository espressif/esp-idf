/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Version Register */
/** Type of id0 register
 *  NA
 */
typedef union {
    struct {
        /** dmac_id : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dmac_id: 32;
    };
    uint32_t val;
} dmac_id0_reg_t;

/** Type of compver0 register
 *  NA
 */
typedef union {
    struct {
        /** dmac_compver : RO; bitpos: [31:0]; default: 842018858;
         *  NA
         */
        uint32_t dmac_compver: 32;
    };
    uint32_t val;
} dmac_compver0_reg_t;

/** Group: Configuration Registers */
/** Type of cfg0 register
 *  NA
 */
typedef union {
    struct {
        /** dmac_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t dmac_en: 1;
        /** int_en : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t int_en: 1;
        uint32_t reserved_2: 30;
    };
    uint32_t val;
} dmac_cfg0_reg_t;

/** Type of chen0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_en: 1;
        /** ch2_en : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_en: 1;
        /** ch3_en : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_en: 1;
        /** ch4_en : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_en: 1;
        uint32_t reserved_4: 4;
        /** ch1_en_we : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch1_en_we: 1;
        /** ch2_en_we : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch2_en_we: 1;
        /** ch3_en_we : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch3_en_we: 1;
        /** ch4_en_we : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch4_en_we: 1;
        uint32_t reserved_12: 4;
        /** ch1_susp : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch1_susp: 1;
        /** ch2_susp : R/W; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch2_susp: 1;
        /** ch3_susp : R/W; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch3_susp: 1;
        /** ch4_susp : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch4_susp: 1;
        uint32_t reserved_20: 4;
        /** ch1_susp_we : WO; bitpos: [24]; default: 0;
         *  NA
         */
        uint32_t ch1_susp_we: 1;
        /** ch2_susp_we : WO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch2_susp_we: 1;
        /** ch3_susp_we : WO; bitpos: [26]; default: 0;
         *  NA
         */
        uint32_t ch3_susp_we: 1;
        /** ch4_susp_we : WO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch4_susp_we: 1;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} dmac_chen0_reg_t;

/** Type of chen1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_abort : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_abort: 1;
        /** ch2_abort : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_abort: 1;
        /** ch3_abort : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_abort: 1;
        /** ch4_abort : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_abort: 1;
        uint32_t reserved_4: 4;
        /** ch1_abort_we : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch1_abort_we: 1;
        /** ch2_abort_we : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch2_abort_we: 1;
        /** ch3_abort_we : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch3_abort_we: 1;
        /** ch4_abort_we : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch4_abort_we: 1;
        uint32_t reserved_12: 20;
    };
    uint32_t val;
} dmac_chen1_reg_t;

/** Type of reset0 register
 *  NA
 */
typedef union {
    struct {
        /** dmac_rst : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t dmac_rst: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} dmac_reset0_reg_t;

/** Type of lowpower_cfg0 register
 *  NA
 */
typedef union {
    struct {
        /** gbl_cslp_en : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t gbl_cslp_en: 1;
        /** chnl_cslp_en : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t chnl_cslp_en: 1;
        /** sbiu_cslp_en : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t sbiu_cslp_en: 1;
        /** mxif_cslp_en : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t mxif_cslp_en: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} dmac_lowpower_cfg0_reg_t;

/** Type of lowpower_cfg1 register
 *  NA
 */
typedef union {
    struct {
        /** glch_lpdly : R/W; bitpos: [7:0]; default: 64;
         *  NA
         */
        uint32_t glch_lpdly: 8;
        /** sbiu_lpdly : R/W; bitpos: [15:8]; default: 64;
         *  NA
         */
        uint32_t sbiu_lpdly: 8;
        /** mxif_lpdly : R/W; bitpos: [23:16]; default: 64;
         *  NA
         */
        uint32_t mxif_lpdly: 8;
        uint32_t reserved_24: 8;
    };
    uint32_t val;
} dmac_lowpower_cfg1_reg_t;

/** Type of chn_sar0 register
 *  NA
 */
typedef union {
    struct {
        /** sar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t sar0: 32;
    };
    uint32_t val;
} dmac_chn_sar0_reg_t;

/** Type of chn_sar1 register
 *  NA
 */
typedef union {
    struct {
        /** sar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t sar1: 32;
    };
    uint32_t val;
} dmac_chn_sar1_reg_t;

/** Type of chn_dar0 register
 *  NA
 */
typedef union {
    struct {
        /** dar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dar0: 32;
    };
    uint32_t val;
} dmac_chn_dar0_reg_t;

/** Type of chn_dar1 register
 *  NA
 */
typedef union {
    struct {
        /** dar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dar1: 32;
    };
    uint32_t val;
} dmac_chn_dar1_reg_t;

/** Type of chn_block_ts0 register
 *  NA
 */
typedef union {
    struct {
        /** block_ts : R/W; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t block_ts: 22;
        uint32_t reserved_22: 10;
    };
    uint32_t val;
} dmac_chn_block_ts0_reg_t;

/** Type of chn_ctl0 register
 *  NA
 */
typedef union {
    struct {
        /** sms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t sms: 1;
        uint32_t reserved_1: 1;
        /** dms : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t dms: 1;
        uint32_t reserved_3: 1;
        /** sinc : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t sinc: 1;
        uint32_t reserved_5: 1;
        /** dinc : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t dinc: 1;
        uint32_t reserved_7: 1;
        /** src_tr_width : R/W; bitpos: [10:8]; default: 2;
         *  NA
         */
        uint32_t src_tr_width: 3;
        /** dst_tr_width : R/W; bitpos: [13:11]; default: 2;
         *  NA
         */
        uint32_t dst_tr_width: 3;
        /** src_msize : R/W; bitpos: [17:14]; default: 0;
         *  NA
         */
        uint32_t src_msize: 4;
        /** dst_msize : R/W; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t dst_msize: 4;
        /** ar_cache : R/W; bitpos: [25:22]; default: 0;
         *  NA
         */
        uint32_t ar_cache: 4;
        /** aw_cache : R/W; bitpos: [29:26]; default: 0;
         *  NA
         */
        uint32_t aw_cache: 4;
        /** nonposted_lastwrite_en : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t nonposted_lastwrite_en: 1;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} dmac_chn_ctl0_reg_t;

/** Type of chn_ctl1 register
 *  NA
 */
typedef union {
    struct {
        /** ar_prot : R/W; bitpos: [2:0]; default: 0;
         *  NA
         */
        uint32_t ar_prot: 3;
        /** aw_prot : R/W; bitpos: [5:3]; default: 0;
         *  NA
         */
        uint32_t aw_prot: 3;
        /** arlen_en : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t arlen_en: 1;
        /** arlen : R/W; bitpos: [14:7]; default: 0;
         *  NA
         */
        uint32_t arlen: 8;
        /** awlen_en : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t awlen_en: 1;
        /** awlen : R/W; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t awlen: 8;
        /** src_stat_en : R/W; bitpos: [24]; default: 0;
         *  NA
         */
        uint32_t src_stat_en: 1;
        /** dst_stat_en : R/W; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t dst_stat_en: 1;
        /** ioc_blktfr : R/W; bitpos: [26]; default: 0;
         *  NA
         */
        uint32_t ioc_blktfr: 1;
        uint32_t reserved_27: 3;
        /** shadowreg_or_lli_last : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t shadowreg_or_lli_last: 1;
        /** shadowreg_or_lli_valid : R/W; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t shadowreg_or_lli_valid: 1;
    };
    uint32_t val;
} dmac_chn_ctl1_reg_t;

/** Type of chn_cfg0 register
 *  NA
 */
typedef union {
    struct {
        /** src_multblk_type : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t src_multblk_type: 2;
        /** dst_multblk_type : R/W; bitpos: [3:2]; default: 0;
         *  NA
         */
        uint32_t dst_multblk_type: 2;
        uint32_t reserved_4: 14;
        /** rd_uid : RO; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t rd_uid: 4;
        uint32_t reserved_22: 3;
        /** wr_uid : RO; bitpos: [28:25]; default: 0;
         *  NA
         */
        uint32_t wr_uid: 4;
        uint32_t reserved_29: 3;
    };
    uint32_t val;
} dmac_chn_cfg0_reg_t;

/** Type of chn_cfg1 register
 *  NA
 */
typedef union {
    struct {
        /** tt_fc : R/W; bitpos: [2:0]; default: 3;
         *  NA
         */
        uint32_t tt_fc: 3;
        /** hs_sel_src : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t hs_sel_src: 1;
        /** hs_sel_dst : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t hs_sel_dst: 1;
        /** src_hwhs_pol : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t src_hwhs_pol: 1;
        /** dst_hwhs_pol : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t dst_hwhs_pol: 1;
        /** src_per : R/W; bitpos: [8:7]; default: 0;
         *  NA
         */
        uint32_t src_per: 2;
        uint32_t reserved_9: 3;
        /** dst_per : R/W; bitpos: [13:12]; default: 0;
         *  NA
         */
        uint32_t dst_per: 2;
        uint32_t reserved_14: 3;
        /** ch_prior : R/W; bitpos: [19:17]; default: 3;
         *  NA
         */
        uint32_t ch_prior: 3;
        /** lock_ch : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t lock_ch: 1;
        /** lock_ch_l : RO; bitpos: [22:21]; default: 0;
         *  NA
         */
        uint32_t lock_ch_l: 2;
        /** src_osr_lmt : R/W; bitpos: [26:23]; default: 0;
         *  NA
         */
        uint32_t src_osr_lmt: 4;
        /** dst_osr_lmt : R/W; bitpos: [30:27]; default: 0;
         *  NA
         */
        uint32_t dst_osr_lmt: 4;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} dmac_chn_cfg1_reg_t;

/** Type of chn_llp0 register
 *  NA
 */
typedef union {
    struct {
        /** lms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t lms: 1;
        uint32_t reserved_1: 5;
        /** loc0 : R/W; bitpos: [31:6]; default: 0;
         *  NA
         */
        uint32_t loc0: 26;
    };
    uint32_t val;
} dmac_chn_llp0_reg_t;

/** Type of chn_llp1 register
 *  NA
 */
typedef union {
    struct {
        /** loc1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t loc1: 32;
    };
    uint32_t val;
} dmac_chn_llp1_reg_t;

/** Type of chn_swhssrc0 register
 *  NA
 */
typedef union {
    struct {
        /** swhs_req_src : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t swhs_req_src: 1;
        /** swhs_req_src_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t swhs_req_src_we: 1;
        /** swhs_sglreq_src : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t swhs_sglreq_src: 1;
        /** swhs_sglreq_src_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t swhs_sglreq_src_we: 1;
        /** swhs_lst_src : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t swhs_lst_src: 1;
        /** swhs_lst_src_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t swhs_lst_src_we: 1;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} dmac_chn_swhssrc0_reg_t;

/** Type of chn_swhsdst0 register
 *  NA
 */
typedef union {
    struct {
        /** swhs_req_dst : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t swhs_req_dst: 1;
        /** swhs_req_dst_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t swhs_req_dst_we: 1;
        /** swhs_sglreq_dst : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t swhs_sglreq_dst: 1;
        /** swhs_sglreq_dst_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t swhs_sglreq_dst_we: 1;
        /** swhs_lst_dst : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t swhs_lst_dst: 1;
        /** swhs_lst_dst_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t swhs_lst_dst_we: 1;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} dmac_chn_swhsdst0_reg_t;

/** Type of chn_blk_tfr_resumereq0 register
 *  NA
 */
typedef union {
    struct {
        /** blk_tfr_resumereq : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t blk_tfr_resumereq: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} dmac_chn_blk_tfr_resumereq0_reg_t;

/** Type of chn_axi_id0 register
 *  NA
 */
typedef union {
    struct {
        /** axi_read_id_suffix : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t axi_read_id_suffix: 1;
        uint32_t reserved_1: 15;
        /** axi_write_id_suffix : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t axi_write_id_suffix: 1;
        uint32_t reserved_17: 15;
    };
    uint32_t val;
} dmac_chn_axi_id0_reg_t;

/** Type of chn_axi_qos0 register
 *  NA
 */
typedef union {
    struct {
        /** axi_awqos : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t axi_awqos: 4;
        /** axi_arqos : R/W; bitpos: [7:4]; default: 0;
         *  NA
         */
        uint32_t axi_arqos: 4;
        uint32_t reserved_8: 24;
    };
    uint32_t val;
} dmac_chn_axi_qos0_reg_t;

/** Group: Interrupt Registers */
/** Type of intstatus0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_intstat: 1;
        /** ch2_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_intstat: 1;
        /** ch3_intstat : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_intstat: 1;
        /** ch4_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_intstat: 1;
        uint32_t reserved_4: 12;
        /** commonreg_intstat : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t commonreg_intstat: 1;
        uint32_t reserved_17: 15;
    };
    uint32_t val;
} dmac_intstatus0_reg_t;

/** Type of commonreg_intclear0 register
 *  NA
 */
typedef union {
    struct {
        /** clear_slvif_commonreg_dec_err_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_commonreg_dec_err_intstat: 1;
        /** clear_slvif_commonreg_wr2ro_err_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_commonreg_wr2ro_err_intstat: 1;
        /** clear_slvif_commonreg_rd2wo_err_intstat : WO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_commonreg_rd2wo_err_intstat: 1;
        /** clear_slvif_commonreg_wronhold_err_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_commonreg_wronhold_err_intstat: 1;
        uint32_t reserved_4: 3;
        /** clear_slvif_commonreg_wrparity_err_intstat : WO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_commonreg_wrparity_err_intstat: 1;
        /** clear_slvif_undefinedreg_dec_err_intstat : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_undefinedreg_dec_err_intstat: 1;
        /** clear_mxif1_rch0_eccprot_correrr_intstat : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_rch0_eccprot_correrr_intstat: 1;
        /** clear_mxif1_rch0_eccprot_uncorrerr_intstat : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_rch0_eccprot_uncorrerr_intstat: 1;
        /** clear_mxif1_rch1_eccprot_correrr_intstat : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_rch1_eccprot_correrr_intstat: 1;
        /** clear_mxif1_rch1_eccprot_uncorrerr_intstat : WO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_rch1_eccprot_uncorrerr_intstat: 1;
        /** clear_mxif1_bch_eccprot_correrr_intstat : WO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_bch_eccprot_correrr_intstat: 1;
        /** clear_mxif1_bch_eccprot_uncorrerr_intstat : WO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_bch_eccprot_uncorrerr_intstat: 1;
        /** clear_mxif2_rch0_eccprot_correrr_intstat : WO; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_rch0_eccprot_correrr_intstat: 1;
        /** clear_mxif2_rch0_eccprot_uncorrerr_intstat : WO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_rch0_eccprot_uncorrerr_intstat: 1;
        /** clear_mxif2_rch1_eccprot_correrr_intstat : WO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_rch1_eccprot_correrr_intstat: 1;
        /** clear_mxif2_rch1_eccprot_uncorrerr_intstat : WO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_rch1_eccprot_uncorrerr_intstat: 1;
        /** clear_mxif2_bch_eccprot_correrr_intstat : WO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_bch_eccprot_correrr_intstat: 1;
        /** clear_mxif2_bch_eccprot_uncorrerr_intstat : WO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_bch_eccprot_uncorrerr_intstat: 1;
        uint32_t reserved_21: 11;
    };
    uint32_t val;
} dmac_commonreg_intclear0_reg_t;

/** Type of commonreg_intstatus_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** enable_slvif_commonreg_dec_err_intstat : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_dec_err_intstat: 1;
        /** enable_slvif_commonreg_wr2ro_err_intstat : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wr2ro_err_intstat: 1;
        /** enable_slvif_commonreg_rd2wo_err_intstat : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_rd2wo_err_intstat: 1;
        /** enable_slvif_commonreg_wronhold_err_intstat : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wronhold_err_intstat: 1;
        uint32_t reserved_4: 3;
        /** enable_slvif_commonreg_wrparity_err_intstat : RO; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wrparity_err_intstat: 1;
        /** enable_slvif_undefinedreg_dec_err_intstat : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_undefinedreg_dec_err_intstat: 1;
        /** enable_mxif1_rch0_eccprot_correrr_intstat : RO; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch0_eccprot_correrr_intstat: 1;
        /** enable_mxif1_rch0_eccprot_uncorrerr_intstat : RO; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch0_eccprot_uncorrerr_intstat: 1;
        /** enable_mxif1_rch1_eccprot_correrr_intstat : RO; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch1_eccprot_correrr_intstat: 1;
        /** enable_mxif1_rch1_eccprot_uncorrerr_intstat : RO; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch1_eccprot_uncorrerr_intstat: 1;
        /** enable_mxif1_bch_eccprot_correrr_intstat : RO; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_bch_eccprot_correrr_intstat: 1;
        /** enable_mxif1_bch_eccprot_uncorrerr_intstat : RO; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_bch_eccprot_uncorrerr_intstat: 1;
        /** enable_mxif2_rch0_eccprot_correrr_intstat : RO; bitpos: [15]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch0_eccprot_correrr_intstat: 1;
        /** enable_mxif2_rch0_eccprot_uncorrerr_intstat : RO; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch0_eccprot_uncorrerr_intstat: 1;
        /** enable_mxif2_rch1_eccprot_correrr_intstat : RO; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch1_eccprot_correrr_intstat: 1;
        /** enable_mxif2_rch1_eccprot_uncorrerr_intstat : RO; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch1_eccprot_uncorrerr_intstat: 1;
        /** enable_mxif2_bch_eccprot_correrr_intstat : RO; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_bch_eccprot_correrr_intstat: 1;
        /** enable_mxif2_bch_eccprot_uncorrerr_intstat : RO; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_bch_eccprot_uncorrerr_intstat: 1;
        uint32_t reserved_21: 11;
    };
    uint32_t val;
} dmac_commonreg_intstatus_enable0_reg_t;

/** Type of commonreg_intsignal_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** enable_slvif_commonreg_dec_err_intsignal : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_dec_err_intsignal: 1;
        /** enable_slvif_commonreg_wr2ro_err_intsignal : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wr2ro_err_intsignal: 1;
        /** enable_slvif_commonreg_rd2wo_err_intsignal : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_rd2wo_err_intsignal: 1;
        /** enable_slvif_commonreg_wronhold_err_intsignal : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wronhold_err_intsignal: 1;
        uint32_t reserved_4: 3;
        /** enable_slvif_commonreg_wrparity_err_intsignal : RO; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wrparity_err_intsignal: 1;
        /** enable_slvif_undefinedreg_dec_err_intsignal : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_undefinedreg_dec_err_intsignal: 1;
        /** enable_mxif1_rch0_eccprot_correrr_intsignal : RO; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch0_eccprot_correrr_intsignal: 1;
        /** enable_mxif1_rch0_eccprot_uncorrerr_intsignal : RO; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch0_eccprot_uncorrerr_intsignal: 1;
        /** enable_mxif1_rch1_eccprot_correrr_intsignal : RO; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch1_eccprot_correrr_intsignal: 1;
        /** enable_mxif1_rch1_eccprot_uncorrerr_intsignal : RO; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch1_eccprot_uncorrerr_intsignal: 1;
        /** enable_mxif1_bch_eccprot_correrr_intsignal : RO; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_bch_eccprot_correrr_intsignal: 1;
        /** enable_mxif1_bch_eccprot_uncorrerr_intsignal : RO; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_bch_eccprot_uncorrerr_intsignal: 1;
        /** enable_mxif2_rch0_eccprot_correrr_intsignal : RO; bitpos: [15]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch0_eccprot_correrr_intsignal: 1;
        /** enable_mxif2_rch0_eccprot_uncorrerr_intsignal : RO; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch0_eccprot_uncorrerr_intsignal: 1;
        /** enable_mxif2_rch1_eccprot_correrr_intsignal : RO; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch1_eccprot_correrr_intsignal: 1;
        /** enable_mxif2_rch1_eccprot_uncorrerr_intsignal : RO; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch1_eccprot_uncorrerr_intsignal: 1;
        /** enable_mxif2_bch_eccprot_correrr_intsignal : RO; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_bch_eccprot_correrr_intsignal: 1;
        /** enable_mxif2_bch_eccprot_uncorrerr_intsignal : RO; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_bch_eccprot_uncorrerr_intsignal: 1;
        uint32_t reserved_21: 11;
    };
    uint32_t val;
} dmac_commonreg_intsignal_enable0_reg_t;

/** Type of commonreg_intstatus0 register
 *  NA
 */
typedef union {
    struct {
        /** slvif_commonreg_dec_err_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t slvif_commonreg_dec_err_intstat: 1;
        /** slvif_commonreg_wr2ro_err_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t slvif_commonreg_wr2ro_err_intstat: 1;
        /** slvif_commonreg_rd2wo_err_intstat : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t slvif_commonreg_rd2wo_err_intstat: 1;
        /** slvif_commonreg_wronhold_err_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t slvif_commonreg_wronhold_err_intstat: 1;
        uint32_t reserved_4: 3;
        /** slvif_commonreg_wrparity_err_intstat : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t slvif_commonreg_wrparity_err_intstat: 1;
        /** slvif_undefinedreg_dec_err_intstat : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t slvif_undefinedreg_dec_err_intstat: 1;
        /** mxif1_rch0_eccprot_correrr_intstat : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mxif1_rch0_eccprot_correrr_intstat: 1;
        /** mxif1_rch0_eccprot_uncorrerr_intstat : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mxif1_rch0_eccprot_uncorrerr_intstat: 1;
        /** mxif1_rch1_eccprot_correrr_intstat : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mxif1_rch1_eccprot_correrr_intstat: 1;
        /** mxif1_rch1_eccprot_uncorrerr_intstat : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mxif1_rch1_eccprot_uncorrerr_intstat: 1;
        /** mxif1_bch_eccprot_correrr_intstat : RO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t mxif1_bch_eccprot_correrr_intstat: 1;
        /** mxif1_bch_eccprot_uncorrerr_intstat : RO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t mxif1_bch_eccprot_uncorrerr_intstat: 1;
        /** mxif2_rch0_eccprot_correrr_intstat : RO; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t mxif2_rch0_eccprot_correrr_intstat: 1;
        /** mxif2_rch0_eccprot_uncorrerr_intstat : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t mxif2_rch0_eccprot_uncorrerr_intstat: 1;
        /** mxif2_rch1_eccprot_correrr_intstat : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t mxif2_rch1_eccprot_correrr_intstat: 1;
        /** mxif2_rch1_eccprot_uncorrerr_intstat : RO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t mxif2_rch1_eccprot_uncorrerr_intstat: 1;
        /** mxif2_bch_eccprot_correrr_intstat : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t mxif2_bch_eccprot_correrr_intstat: 1;
        /** mxif2_bch_eccprot_uncorrerr_intstat : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t mxif2_bch_eccprot_uncorrerr_intstat: 1;
        uint32_t reserved_21: 11;
    };
    uint32_t val;
} dmac_commonreg_intstatus0_reg_t;

/** Type of chn_intstatus_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** enable_block_tfr_done_intstat : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t enable_block_tfr_done_intstat: 1;
        /** enable_dma_tfr_done_intstat : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t enable_dma_tfr_done_intstat: 1;
        uint32_t reserved_2: 1;
        /** enable_src_transcomp_intstat : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t enable_src_transcomp_intstat: 1;
        /** enable_dst_transcomp_intstat : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t enable_dst_transcomp_intstat: 1;
        /** enable_src_dec_err_intstat : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t enable_src_dec_err_intstat: 1;
        /** enable_dst_dec_err_intstat : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t enable_dst_dec_err_intstat: 1;
        /** enable_src_slv_err_intstat : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t enable_src_slv_err_intstat: 1;
        /** enable_dst_slv_err_intstat : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t enable_dst_slv_err_intstat: 1;
        /** enable_lli_rd_dec_err_intstat : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t enable_lli_rd_dec_err_intstat: 1;
        /** enable_lli_wr_dec_err_intstat : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t enable_lli_wr_dec_err_intstat: 1;
        /** enable_lli_rd_slv_err_intstat : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t enable_lli_rd_slv_err_intstat: 1;
        /** enable_lli_wr_slv_err_intstat : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t enable_lli_wr_slv_err_intstat: 1;
        /** enable_shadowreg_or_lli_invalid_err_intstat : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t enable_shadowreg_or_lli_invalid_err_intstat: 1;
        /** enable_slvif_multiblktype_err_intstat : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_multiblktype_err_intstat: 1;
        uint32_t reserved_15: 1;
        /** enable_slvif_dec_err_intstat : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_dec_err_intstat: 1;
        /** enable_slvif_wr2ro_err_intstat : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_wr2ro_err_intstat: 1;
        /** enable_slvif_rd2rwo_err_intstat : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_rd2rwo_err_intstat: 1;
        /** enable_slvif_wronchen_err_intstat : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_wronchen_err_intstat: 1;
        /** enable_slvif_shadowreg_wron_valid_err_intstat : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_shadowreg_wron_valid_err_intstat: 1;
        /** enable_slvif_wronhold_err_intstat : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_wronhold_err_intstat: 1;
        uint32_t reserved_22: 3;
        /** enable_slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_wrparity_err_intstat: 1;
        uint32_t reserved_26: 1;
        /** enable_ch_lock_cleared_intstat : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t enable_ch_lock_cleared_intstat: 1;
        /** enable_ch_src_suspended_intstat : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t enable_ch_src_suspended_intstat: 1;
        /** enable_ch_suspended_intstat : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t enable_ch_suspended_intstat: 1;
        /** enable_ch_disabled_intstat : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t enable_ch_disabled_intstat: 1;
        /** enable_ch_aborted_intstat : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t enable_ch_aborted_intstat: 1;
    };
    uint32_t val;
} dmac_chn_intstatus_enable0_reg_t;

/** Type of chn_intstatus_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** enable_ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t enable_ecc_prot_chmem_correrr_intstat: 1;
        /** enable_ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t enable_ecc_prot_chmem_uncorrerr_intstat: 1;
        /** enable_ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t enable_ecc_prot_uidmem_correrr_intstat: 1;
        /** enable_ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t enable_ecc_prot_uidmem_uncorrerr_intstat: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} dmac_chn_intstatus_enable1_reg_t;

/** Type of chn_intstatus0 register
 *  NA
 */
typedef union {
    struct {
        /** block_tfr_done_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t block_tfr_done_intstat: 1;
        /** dma_tfr_done_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t dma_tfr_done_intstat: 1;
        uint32_t reserved_2: 1;
        /** src_transcomp_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t src_transcomp_intstat: 1;
        /** dst_transcomp_intstat : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t dst_transcomp_intstat: 1;
        /** src_dec_err_intstat : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t src_dec_err_intstat: 1;
        /** dst_dec_err_intstat : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t dst_dec_err_intstat: 1;
        /** src_slv_err_intstat : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t src_slv_err_intstat: 1;
        /** dst_slv_err_intstat : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t dst_slv_err_intstat: 1;
        /** lli_rd_dec_err_intstat : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t lli_rd_dec_err_intstat: 1;
        /** lli_wr_dec_err_intstat : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t lli_wr_dec_err_intstat: 1;
        /** lli_rd_slv_err_intstat : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t lli_rd_slv_err_intstat: 1;
        /** lli_wr_slv_err_intstat : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t lli_wr_slv_err_intstat: 1;
        /** shadowreg_or_lli_invalid_err_intstat : RO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t shadowreg_or_lli_invalid_err_intstat: 1;
        /** slvif_multiblktype_err_intstat : RO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t slvif_multiblktype_err_intstat: 1;
        uint32_t reserved_15: 1;
        /** slvif_dec_err_intstat : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t slvif_dec_err_intstat: 1;
        /** slvif_wr2ro_err_intstat : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t slvif_wr2ro_err_intstat: 1;
        /** slvif_rd2rwo_err_intstat : RO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t slvif_rd2rwo_err_intstat: 1;
        /** slvif_wronchen_err_intstat : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t slvif_wronchen_err_intstat: 1;
        /** slvif_shadowreg_wron_valid_err_intstat : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t slvif_shadowreg_wron_valid_err_intstat: 1;
        /** slvif_wronhold_err_intstat : RO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t slvif_wronhold_err_intstat: 1;
        uint32_t reserved_22: 3;
        /** slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t slvif_wrparity_err_intstat: 1;
        uint32_t reserved_26: 1;
        /** ch_lock_cleared_intstat : RO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch_lock_cleared_intstat: 1;
        /** ch_src_suspended_intstat : RO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t ch_src_suspended_intstat: 1;
        /** ch_suspended_intstat : RO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t ch_suspended_intstat: 1;
        /** ch_disabled_intstat : RO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch_disabled_intstat: 1;
        /** ch_aborted_intstat : RO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch_aborted_intstat: 1;
    };
    uint32_t val;
} dmac_chn_intstatus0_reg_t;

/** Type of chn_intstatus1 register
 *  NA
 */
typedef union {
    struct {
        /** ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ecc_prot_chmem_correrr_intstat: 1;
        /** ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ecc_prot_chmem_uncorrerr_intstat: 1;
        /** ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ecc_prot_uidmem_correrr_intstat: 1;
        /** ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ecc_prot_uidmem_uncorrerr_intstat: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} dmac_chn_intstatus1_reg_t;

/** Type of chn_intsignal_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** enable_block_tfr_done_intsignal : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t enable_block_tfr_done_intsignal: 1;
        /** enable_dma_tfr_done_intsignal : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t enable_dma_tfr_done_intsignal: 1;
        uint32_t reserved_2: 1;
        /** enable_src_transcomp_intsignal : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t enable_src_transcomp_intsignal: 1;
        /** enable_dst_transcomp_intsignal : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t enable_dst_transcomp_intsignal: 1;
        /** enable_src_dec_err_intsignal : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t enable_src_dec_err_intsignal: 1;
        /** enable_dst_dec_err_intsignal : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t enable_dst_dec_err_intsignal: 1;
        /** enable_src_slv_err_intsignal : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t enable_src_slv_err_intsignal: 1;
        /** enable_dst_slv_err_intsignal : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t enable_dst_slv_err_intsignal: 1;
        /** enable_lli_rd_dec_err_intsignal : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t enable_lli_rd_dec_err_intsignal: 1;
        /** enable_lli_wr_dec_err_intsignal : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t enable_lli_wr_dec_err_intsignal: 1;
        /** enable_lli_rd_slv_err_intsignal : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t enable_lli_rd_slv_err_intsignal: 1;
        /** enable_lli_wr_slv_err_intsignal : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t enable_lli_wr_slv_err_intsignal: 1;
        /** enable_shadowreg_or_lli_invalid_err_intsignal : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t enable_shadowreg_or_lli_invalid_err_intsignal: 1;
        /** enable_slvif_multiblktype_err_intsignal : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_multiblktype_err_intsignal: 1;
        uint32_t reserved_15: 1;
        /** enable_slvif_dec_err_intsignal : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_dec_err_intsignal: 1;
        /** enable_slvif_wr2ro_err_intsignal : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_wr2ro_err_intsignal: 1;
        /** enable_slvif_rd2rwo_err_intsignal : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_rd2rwo_err_intsignal: 1;
        /** enable_slvif_wronchen_err_intsignal : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_wronchen_err_intsignal: 1;
        /** enable_slvif_shadowreg_wron_valid_err_intsignal : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_shadowreg_wron_valid_err_intsignal: 1;
        /** enable_slvif_wronhold_err_intsignal : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_wronhold_err_intsignal: 1;
        uint32_t reserved_22: 3;
        /** enable_slvif_wrparity_err_intsignal : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_wrparity_err_intsignal: 1;
        uint32_t reserved_26: 1;
        /** enable_ch_lock_cleared_intsignal : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t enable_ch_lock_cleared_intsignal: 1;
        /** enable_ch_src_suspended_intsignal : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t enable_ch_src_suspended_intsignal: 1;
        /** enable_ch_suspended_intsignal : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t enable_ch_suspended_intsignal: 1;
        /** enable_ch_disabled_intsignal : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t enable_ch_disabled_intsignal: 1;
        /** enable_ch_aborted_intsignal : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t enable_ch_aborted_intsignal: 1;
    };
    uint32_t val;
} dmac_chn_intsignal_enable0_reg_t;

/** Type of chn_intsignal_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** enable_ecc_prot_chmem_correrr_intsignal : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t enable_ecc_prot_chmem_correrr_intsignal: 1;
        /** enable_ecc_prot_chmem_uncorrerr_intsignal : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t enable_ecc_prot_chmem_uncorrerr_intsignal: 1;
        /** enable_ecc_prot_uidmem_correrr_intsignal : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t enable_ecc_prot_uidmem_correrr_intsignal: 1;
        /** enable_ecc_prot_uidmem_uncorrerr_intsignal : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t enable_ecc_prot_uidmem_uncorrerr_intsignal: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} dmac_chn_intsignal_enable1_reg_t;

/** Type of chn_intclear0 register
 *  NA
 */
typedef union {
    struct {
        /** clear_block_tfr_done_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t clear_block_tfr_done_intstat: 1;
        /** clear_dma_tfr_done_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t clear_dma_tfr_done_intstat: 1;
        uint32_t reserved_2: 1;
        /** clear_src_transcomp_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t clear_src_transcomp_intstat: 1;
        /** clear_dst_transcomp_intstat : WO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t clear_dst_transcomp_intstat: 1;
        /** clear_src_dec_err_intstat : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t clear_src_dec_err_intstat: 1;
        /** clear_dst_dec_err_intstat : WO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t clear_dst_dec_err_intstat: 1;
        /** clear_src_slv_err_intstat : WO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t clear_src_slv_err_intstat: 1;
        /** clear_dst_slv_err_intstat : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t clear_dst_slv_err_intstat: 1;
        /** clear_lli_rd_dec_err_intstat : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t clear_lli_rd_dec_err_intstat: 1;
        /** clear_lli_wr_dec_err_intstat : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t clear_lli_wr_dec_err_intstat: 1;
        /** clear_lli_rd_slv_err_intstat : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t clear_lli_rd_slv_err_intstat: 1;
        /** clear_lli_wr_slv_err_intstat : WO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t clear_lli_wr_slv_err_intstat: 1;
        /** clear_shadowreg_or_lli_invalid_err_intstat : WO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t clear_shadowreg_or_lli_invalid_err_intstat: 1;
        /** clear_slvif_multiblktype_err_intstat : WO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_multiblktype_err_intstat: 1;
        uint32_t reserved_15: 1;
        /** clear_slvif_dec_err_intstat : WO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_dec_err_intstat: 1;
        /** clear_slvif_wr2ro_err_intstat : WO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_wr2ro_err_intstat: 1;
        /** clear_slvif_rd2rwo_err_intstat : WO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_rd2rwo_err_intstat: 1;
        /** clear_slvif_wronchen_err_intstat : WO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_wronchen_err_intstat: 1;
        /** clear_slvif_shadowreg_wron_valid_err_intstat : WO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_shadowreg_wron_valid_err_intstat: 1;
        /** clear_slvif_wronhold_err_intstat : WO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_wronhold_err_intstat: 1;
        uint32_t reserved_22: 3;
        /** clear_slvif_wrparity_err_intstat : WO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_wrparity_err_intstat: 1;
        uint32_t reserved_26: 1;
        /** clear_ch_lock_cleared_intstat : WO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t clear_ch_lock_cleared_intstat: 1;
        /** clear_ch_src_suspended_intstat : WO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t clear_ch_src_suspended_intstat: 1;
        /** clear_ch_suspended_intstat : WO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t clear_ch_suspended_intstat: 1;
        /** clear_ch_disabled_intstat : WO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t clear_ch_disabled_intstat: 1;
        /** clear_ch_aborted_intstat : WO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t clear_ch_aborted_intstat: 1;
    };
    uint32_t val;
} dmac_chn_intclear0_reg_t;

/** Type of chn_intclear1 register
 *  NA
 */
typedef union {
    struct {
        /** clear_ecc_prot_chmem_correrr_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t clear_ecc_prot_chmem_correrr_intstat: 1;
        /** clear_ecc_prot_chmem_uncorrerr_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t clear_ecc_prot_chmem_uncorrerr_intstat: 1;
        /** clear_ecc_prot_uidmem_correrr_intstat : WO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t clear_ecc_prot_uidmem_correrr_intstat: 1;
        /** clear_ecc_prot_uidmem_uncorrerr_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t clear_ecc_prot_uidmem_uncorrerr_intstat: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} dmac_chn_intclear1_reg_t;

/** Group: Status Registers */
/** Type of chn_status0 register
 *  NA
 */
typedef union {
    struct {
        /** cmpltd_blk_tfr_size : RO; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t cmpltd_blk_tfr_size: 22;
        uint32_t reserved_22: 10;
    };
    uint32_t val;
} dmac_chn_status0_reg_t;

/** Type of chn_status1 register
 *  NA
 */
typedef union {
    struct {
        /** data_left_in_fifo : RO; bitpos: [14:0]; default: 0;
         *  NA
         */
        uint32_t data_left_in_fifo: 15;
        uint32_t reserved_15: 17;
    };
    uint32_t val;
} dmac_chn_status1_reg_t;

/** Type of chn_sstat0 register
 *  NA
 */
typedef union {
    struct {
        /** sstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t sstat: 32;
    };
    uint32_t val;
} dmac_chn_sstat0_reg_t;

/** Type of chn_dstat0 register
 *  NA
 */
typedef union {
    struct {
        /** dstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dstat: 32;
    };
    uint32_t val;
} dmac_chn_dstat0_reg_t;

/** Type of chn_sstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** sstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t sstatar0: 32;
    };
    uint32_t val;
} dmac_chn_sstatar0_reg_t;

/** Type of chn_sstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** sstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t sstatar1: 32;
    };
    uint32_t val;
} dmac_chn_sstatar1_reg_t;

/** Type of chn_dstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** dstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dstatar0: 32;
    };
    uint32_t val;
} dmac_chn_dstatar0_reg_t;

/** Type of chn_dstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** dstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dstatar1: 32;
    };
    uint32_t val;
} dmac_chn_dstatar1_reg_t;

typedef struct {
    volatile dmac_chn_sar0_reg_t sar0;
    volatile dmac_chn_sar1_reg_t sar1;
    volatile dmac_chn_dar0_reg_t dar0;
    volatile dmac_chn_dar1_reg_t dar1;
    volatile dmac_chn_block_ts0_reg_t block_ts0;
    uint32_t reserved_114;
    volatile dmac_chn_ctl0_reg_t ctl0;
    volatile dmac_chn_ctl1_reg_t ctl1;
    volatile dmac_chn_cfg0_reg_t cfg0;
    volatile dmac_chn_cfg1_reg_t cfg1;
    volatile dmac_chn_llp0_reg_t llp0;
    volatile dmac_chn_llp1_reg_t llp1;
    volatile dmac_chn_status0_reg_t status0;
    volatile dmac_chn_status1_reg_t status1;
    volatile dmac_chn_swhssrc0_reg_t swhssrc0;
    uint32_t reserved_13c;
    volatile dmac_chn_swhsdst0_reg_t swhsdst0;
    uint32_t reserved_144;
    volatile dmac_chn_blk_tfr_resumereq0_reg_t blk_tfr_resumereq0;
    uint32_t reserved_14c;
    volatile dmac_chn_axi_id0_reg_t axi_id0;
    uint32_t reserved_154;
    volatile dmac_chn_axi_qos0_reg_t axi_qos0;
    uint32_t reserved_15c;
    volatile dmac_chn_sstat0_reg_t sstat0;
    uint32_t reserved_164;
    volatile dmac_chn_dstat0_reg_t dstat0;
    uint32_t reserved_16c;
    volatile dmac_chn_sstatar0_reg_t sstatar0;
    volatile dmac_chn_sstatar1_reg_t sstatar1;
    volatile dmac_chn_dstatar0_reg_t dstatar0;
    volatile dmac_chn_dstatar1_reg_t dstatar1;
    volatile dmac_chn_intstatus_enable0_reg_t int_st_ena0;
    volatile dmac_chn_intstatus_enable1_reg_t int_st_ena1;
    volatile dmac_chn_intstatus0_reg_t int_st0;
    volatile dmac_chn_intstatus1_reg_t int_st1;
    volatile dmac_chn_intsignal_enable0_reg_t int_sig_ena0;
    volatile dmac_chn_intsignal_enable1_reg_t int_sig_ena1;
    volatile dmac_chn_intclear0_reg_t int_clr0;
    volatile dmac_chn_intclear1_reg_t int_clr1;
    uint32_t reserved_1a0[24];
} dmac_channel_reg_t;

typedef struct dw_gdma_dev_t {
    volatile dmac_id0_reg_t id0;
    uint32_t reserved_004;
    volatile dmac_compver0_reg_t compver0;
    uint32_t reserved_00c;
    volatile dmac_cfg0_reg_t cfg0;
    uint32_t reserved_014;
    volatile dmac_chen0_reg_t chen0;
    volatile dmac_chen1_reg_t chen1;
    uint32_t reserved_020[4];
    volatile dmac_intstatus0_reg_t int_st0;
    uint32_t reserved_034;
    volatile dmac_commonreg_intclear0_reg_t common_int_clr0;
    uint32_t reserved_03c;
    volatile dmac_commonreg_intstatus_enable0_reg_t common_int_st_ena0;
    uint32_t reserved_044;
    volatile dmac_commonreg_intsignal_enable0_reg_t common_int_sig_ena0;
    uint32_t reserved_04c;
    volatile dmac_commonreg_intstatus0_reg_t common_int_st0;
    uint32_t reserved_054;
    volatile dmac_reset0_reg_t reset0;
    uint32_t reserved_05c;
    volatile dmac_lowpower_cfg0_reg_t lowpower_cfg0;
    volatile dmac_lowpower_cfg1_reg_t lowpower_cfg1;
    uint32_t reserved_068[38];
    volatile dmac_channel_reg_t ch[4];
} dw_gdma_dev_t;

extern dw_gdma_dev_t DW_GDMA;

#ifndef __cplusplus
_Static_assert(sizeof(dw_gdma_dev_t) == 0x500, "Invalid size of dw_gdma_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
