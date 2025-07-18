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

/** Group: Version Register */
/** Type of id0 register
 *  NA
 */
typedef union {
    struct {
        /** dmac_id : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dmac_id:32;
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
        uint32_t dmac_compver:32;
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
        uint32_t dmac_en:1;
        /** int_en : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t int_en:1;
        uint32_t reserved_2:30;
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
        uint32_t ch1_en:1;
        /** ch2_en : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_en:1;
        /** ch3_en : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_en:1;
        /** ch4_en : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_en:1;
        uint32_t reserved_4:4;
        /** ch1_en_we : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch1_en_we:1;
        /** ch2_en_we : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch2_en_we:1;
        /** ch3_en_we : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch3_en_we:1;
        /** ch4_en_we : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch4_en_we:1;
        uint32_t reserved_12:4;
        /** ch1_susp : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch1_susp:1;
        /** ch2_susp : R/W; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch2_susp:1;
        /** ch3_susp : R/W; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch3_susp:1;
        /** ch4_susp : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch4_susp:1;
        uint32_t reserved_20:4;
        /** ch1_susp_we : WO; bitpos: [24]; default: 0;
         *  NA
         */
        uint32_t ch1_susp_we:1;
        /** ch2_susp_we : WO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch2_susp_we:1;
        /** ch3_susp_we : WO; bitpos: [26]; default: 0;
         *  NA
         */
        uint32_t ch3_susp_we:1;
        /** ch4_susp_we : WO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch4_susp_we:1;
        uint32_t reserved_28:4;
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
        uint32_t ch1_abort:1;
        /** ch2_abort : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_abort:1;
        /** ch3_abort : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_abort:1;
        /** ch4_abort : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_abort:1;
        uint32_t reserved_4:4;
        /** ch1_abort_we : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch1_abort_we:1;
        /** ch2_abort_we : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch2_abort_we:1;
        /** ch3_abort_we : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch3_abort_we:1;
        /** ch4_abort_we : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch4_abort_we:1;
        uint32_t reserved_12:20;
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
        uint32_t dmac_rst:1;
        uint32_t reserved_1:31;
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
        uint32_t gbl_cslp_en:1;
        /** chnl_cslp_en : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t chnl_cslp_en:1;
        /** sbiu_cslp_en : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t sbiu_cslp_en:1;
        /** mxif_cslp_en : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t mxif_cslp_en:1;
        uint32_t reserved_4:28;
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
        uint32_t glch_lpdly:8;
        /** sbiu_lpdly : R/W; bitpos: [15:8]; default: 64;
         *  NA
         */
        uint32_t sbiu_lpdly:8;
        /** mxif_lpdly : R/W; bitpos: [23:16]; default: 64;
         *  NA
         */
        uint32_t mxif_lpdly:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} dmac_lowpower_cfg1_reg_t;

/** Type of ch1_sar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_sar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_sar0:32;
    };
    uint32_t val;
} dmac_ch1_sar0_reg_t;

/** Type of ch1_sar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_sar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_sar1:32;
    };
    uint32_t val;
} dmac_ch1_sar1_reg_t;

/** Type of ch1_dar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_dar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_dar0:32;
    };
    uint32_t val;
} dmac_ch1_dar0_reg_t;

/** Type of ch1_dar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_dar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_dar1:32;
    };
    uint32_t val;
} dmac_ch1_dar1_reg_t;

/** Type of ch1_block_ts0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_block_ts : R/W; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t ch1_block_ts:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} dmac_ch1_block_ts0_reg_t;

/** Type of ch1_ctl0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_sms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_sms:1;
        uint32_t reserved_1:1;
        /** ch1_dms : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch1_dms:1;
        uint32_t reserved_3:1;
        /** ch1_sinc : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch1_sinc:1;
        uint32_t reserved_5:1;
        /** ch1_dinc : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch1_dinc:1;
        uint32_t reserved_7:1;
        /** ch1_src_tr_width : R/W; bitpos: [10:8]; default: 2;
         *  NA
         */
        uint32_t ch1_src_tr_width:3;
        /** ch1_dst_tr_width : R/W; bitpos: [13:11]; default: 2;
         *  NA
         */
        uint32_t ch1_dst_tr_width:3;
        /** ch1_src_msize : R/W; bitpos: [17:14]; default: 0;
         *  NA
         */
        uint32_t ch1_src_msize:4;
        /** ch1_dst_msize : R/W; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t ch1_dst_msize:4;
        /** ch1_ar_cache : R/W; bitpos: [25:22]; default: 0;
         *  NA
         */
        uint32_t ch1_ar_cache:4;
        /** ch1_aw_cache : R/W; bitpos: [29:26]; default: 0;
         *  NA
         */
        uint32_t ch1_aw_cache:4;
        /** ch1_nonposted_lastwrite_en : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch1_nonposted_lastwrite_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} dmac_ch1_ctl0_reg_t;

/** Type of ch1_ctl1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_ar_prot : R/W; bitpos: [2:0]; default: 0;
         *  NA
         */
        uint32_t ch1_ar_prot:3;
        /** ch1_aw_prot : R/W; bitpos: [5:3]; default: 0;
         *  NA
         */
        uint32_t ch1_aw_prot:3;
        /** ch1_arlen_en : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch1_arlen_en:1;
        /** ch1_arlen : R/W; bitpos: [14:7]; default: 0;
         *  NA
         */
        uint32_t ch1_arlen:8;
        /** ch1_awlen_en : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t ch1_awlen_en:1;
        /** ch1_awlen : R/W; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t ch1_awlen:8;
        /** ch1_src_stat_en : R/W; bitpos: [24]; default: 0;
         *  NA
         */
        uint32_t ch1_src_stat_en:1;
        /** ch1_dst_stat_en : R/W; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch1_dst_stat_en:1;
        /** ch1_ioc_blktfr : R/W; bitpos: [26]; default: 0;
         *  NA
         */
        uint32_t ch1_ioc_blktfr:1;
        uint32_t reserved_27:3;
        /** ch1_shadowreg_or_lli_last : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch1_shadowreg_or_lli_last:1;
        /** ch1_shadowreg_or_lli_valid : R/W; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch1_shadowreg_or_lli_valid:1;
    };
    uint32_t val;
} dmac_ch1_ctl1_reg_t;

/** Type of ch1_cfg0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_src_multblk_type : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t ch1_src_multblk_type:2;
        /** ch1_dst_multblk_type : R/W; bitpos: [3:2]; default: 0;
         *  NA
         */
        uint32_t ch1_dst_multblk_type:2;
        uint32_t reserved_4:14;
        /** ch1_rd_uid : RO; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t ch1_rd_uid:4;
        uint32_t reserved_22:3;
        /** ch1_wr_uid : RO; bitpos: [28:25]; default: 0;
         *  NA
         */
        uint32_t ch1_wr_uid:4;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} dmac_ch1_cfg0_reg_t;

/** Type of ch1_cfg1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_tt_fc : R/W; bitpos: [2:0]; default: 3;
         *  NA
         */
        uint32_t ch1_tt_fc:3;
        /** ch1_hs_sel_src : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch1_hs_sel_src:1;
        /** ch1_hs_sel_dst : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch1_hs_sel_dst:1;
        /** ch1_src_hwhs_pol : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch1_src_hwhs_pol:1;
        /** ch1_dst_hwhs_pol : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch1_dst_hwhs_pol:1;
        /** ch1_src_per : R/W; bitpos: [8:7]; default: 0;
         *  NA
         */
        uint32_t ch1_src_per:2;
        uint32_t reserved_9:3;
        /** ch1_dst_per : R/W; bitpos: [13:12]; default: 0;
         *  NA
         */
        uint32_t ch1_dst_per:2;
        uint32_t reserved_14:3;
        /** ch1_ch_prior : R/W; bitpos: [19:17]; default: 3;
         *  NA
         */
        uint32_t ch1_ch_prior:3;
        /** ch1_lock_ch : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch1_lock_ch:1;
        /** ch1_lock_ch_l : RO; bitpos: [22:21]; default: 0;
         *  NA
         */
        uint32_t ch1_lock_ch_l:2;
        /** ch1_src_osr_lmt : R/W; bitpos: [26:23]; default: 0;
         *  NA
         */
        uint32_t ch1_src_osr_lmt:4;
        /** ch1_dst_osr_lmt : R/W; bitpos: [30:27]; default: 0;
         *  NA
         */
        uint32_t ch1_dst_osr_lmt:4;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} dmac_ch1_cfg1_reg_t;

/** Type of ch1_llp0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_lms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_lms:1;
        uint32_t reserved_1:5;
        /** ch1_loc0 : R/W; bitpos: [31:6]; default: 0;
         *  NA
         */
        uint32_t ch1_loc0:26;
    };
    uint32_t val;
} dmac_ch1_llp0_reg_t;

/** Type of ch1_llp1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_loc1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_loc1:32;
    };
    uint32_t val;
} dmac_ch1_llp1_reg_t;

/** Type of ch1_swhssrc0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_swhs_req_src : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_req_src:1;
        /** ch1_swhs_req_src_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_req_src_we:1;
        /** ch1_swhs_sglreq_src : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_sglreq_src:1;
        /** ch1_swhs_sglreq_src_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_sglreq_src_we:1;
        /** ch1_swhs_lst_src : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_lst_src:1;
        /** ch1_swhs_lst_src_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_lst_src_we:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dmac_ch1_swhssrc0_reg_t;

/** Type of ch1_swhsdst0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_swhs_req_dst : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_req_dst:1;
        /** ch1_swhs_req_dst_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_req_dst_we:1;
        /** ch1_swhs_sglreq_dst : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_sglreq_dst:1;
        /** ch1_swhs_sglreq_dst_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_sglreq_dst_we:1;
        /** ch1_swhs_lst_dst : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_lst_dst:1;
        /** ch1_swhs_lst_dst_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch1_swhs_lst_dst_we:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dmac_ch1_swhsdst0_reg_t;

/** Type of ch1_blk_tfr_resumereq0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_blk_tfr_resumereq : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_blk_tfr_resumereq:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dmac_ch1_blk_tfr_resumereq0_reg_t;

/** Type of ch1_axi_id0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_axi_read_id_suffix : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_axi_read_id_suffix:1;
        uint32_t reserved_1:15;
        /** ch1_axi_write_id_suffix : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch1_axi_write_id_suffix:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dmac_ch1_axi_id0_reg_t;

/** Type of ch1_axi_qos0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_axi_awqos : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t ch1_axi_awqos:4;
        /** ch1_axi_arqos : R/W; bitpos: [7:4]; default: 0;
         *  NA
         */
        uint32_t ch1_axi_arqos:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} dmac_ch1_axi_qos0_reg_t;

/** Type of ch2_sar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_sar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_sar0:32;
    };
    uint32_t val;
} dmac_ch2_sar0_reg_t;

/** Type of ch2_sar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_sar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_sar1:32;
    };
    uint32_t val;
} dmac_ch2_sar1_reg_t;

/** Type of ch2_dar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_dar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_dar0:32;
    };
    uint32_t val;
} dmac_ch2_dar0_reg_t;

/** Type of ch2_dar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_dar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_dar1:32;
    };
    uint32_t val;
} dmac_ch2_dar1_reg_t;

/** Type of ch2_block_ts0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_block_ts : R/W; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t ch2_block_ts:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} dmac_ch2_block_ts0_reg_t;

/** Type of ch2_ctl0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_sms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_sms:1;
        uint32_t reserved_1:1;
        /** ch2_dms : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch2_dms:1;
        uint32_t reserved_3:1;
        /** ch2_sinc : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch2_sinc:1;
        uint32_t reserved_5:1;
        /** ch2_dinc : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch2_dinc:1;
        uint32_t reserved_7:1;
        /** ch2_src_tr_width : R/W; bitpos: [10:8]; default: 2;
         *  NA
         */
        uint32_t ch2_src_tr_width:3;
        /** ch2_dst_tr_width : R/W; bitpos: [13:11]; default: 2;
         *  NA
         */
        uint32_t ch2_dst_tr_width:3;
        /** ch2_src_msize : R/W; bitpos: [17:14]; default: 0;
         *  NA
         */
        uint32_t ch2_src_msize:4;
        /** ch2_dst_msize : R/W; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t ch2_dst_msize:4;
        /** ch2_ar_cache : R/W; bitpos: [25:22]; default: 0;
         *  NA
         */
        uint32_t ch2_ar_cache:4;
        /** ch2_aw_cache : R/W; bitpos: [29:26]; default: 0;
         *  NA
         */
        uint32_t ch2_aw_cache:4;
        /** ch2_nonposted_lastwrite_en : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch2_nonposted_lastwrite_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} dmac_ch2_ctl0_reg_t;

/** Type of ch2_ctl1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_ar_prot : R/W; bitpos: [2:0]; default: 0;
         *  NA
         */
        uint32_t ch2_ar_prot:3;
        /** ch2_aw_prot : R/W; bitpos: [5:3]; default: 0;
         *  NA
         */
        uint32_t ch2_aw_prot:3;
        /** ch2_arlen_en : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch2_arlen_en:1;
        /** ch2_arlen : R/W; bitpos: [14:7]; default: 0;
         *  NA
         */
        uint32_t ch2_arlen:8;
        /** ch2_awlen_en : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t ch2_awlen_en:1;
        /** ch2_awlen : R/W; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t ch2_awlen:8;
        /** ch2_src_stat_en : R/W; bitpos: [24]; default: 0;
         *  NA
         */
        uint32_t ch2_src_stat_en:1;
        /** ch2_dst_stat_en : R/W; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch2_dst_stat_en:1;
        /** ch2_ioc_blktfr : R/W; bitpos: [26]; default: 0;
         *  NA
         */
        uint32_t ch2_ioc_blktfr:1;
        uint32_t reserved_27:3;
        /** ch2_shadowreg_or_lli_last : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch2_shadowreg_or_lli_last:1;
        /** ch2_shadowreg_or_lli_valid : R/W; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch2_shadowreg_or_lli_valid:1;
    };
    uint32_t val;
} dmac_ch2_ctl1_reg_t;

/** Type of ch2_cfg0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_src_multblk_type : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t ch2_src_multblk_type:2;
        /** ch2_dst_multblk_type : R/W; bitpos: [3:2]; default: 0;
         *  NA
         */
        uint32_t ch2_dst_multblk_type:2;
        uint32_t reserved_4:14;
        /** ch2_rd_uid : RO; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t ch2_rd_uid:4;
        uint32_t reserved_22:3;
        /** ch2_wr_uid : RO; bitpos: [28:25]; default: 0;
         *  NA
         */
        uint32_t ch2_wr_uid:4;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} dmac_ch2_cfg0_reg_t;

/** Type of ch2_cfg1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_tt_fc : R/W; bitpos: [2:0]; default: 3;
         *  NA
         */
        uint32_t ch2_tt_fc:3;
        /** ch2_hs_sel_src : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch2_hs_sel_src:1;
        /** ch2_hs_sel_dst : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch2_hs_sel_dst:1;
        /** ch2_src_hwhs_pol : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch2_src_hwhs_pol:1;
        /** ch2_dst_hwhs_pol : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch2_dst_hwhs_pol:1;
        /** ch2_src_per : R/W; bitpos: [8:7]; default: 0;
         *  NA
         */
        uint32_t ch2_src_per:2;
        uint32_t reserved_9:3;
        /** ch2_dst_per : R/W; bitpos: [13:12]; default: 0;
         *  NA
         */
        uint32_t ch2_dst_per:2;
        uint32_t reserved_14:3;
        /** ch2_ch_prior : R/W; bitpos: [19:17]; default: 2;
         *  NA
         */
        uint32_t ch2_ch_prior:3;
        /** ch2_lock_ch : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch2_lock_ch:1;
        /** ch2_lock_ch_l : RO; bitpos: [22:21]; default: 0;
         *  NA
         */
        uint32_t ch2_lock_ch_l:2;
        /** ch2_src_osr_lmt : R/W; bitpos: [26:23]; default: 0;
         *  NA
         */
        uint32_t ch2_src_osr_lmt:4;
        /** ch2_dst_osr_lmt : R/W; bitpos: [30:27]; default: 0;
         *  NA
         */
        uint32_t ch2_dst_osr_lmt:4;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} dmac_ch2_cfg1_reg_t;

/** Type of ch2_llp0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_lms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_lms:1;
        uint32_t reserved_1:5;
        /** ch2_loc0 : R/W; bitpos: [31:6]; default: 0;
         *  NA
         */
        uint32_t ch2_loc0:26;
    };
    uint32_t val;
} dmac_ch2_llp0_reg_t;

/** Type of ch2_llp1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_loc1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_loc1:32;
    };
    uint32_t val;
} dmac_ch2_llp1_reg_t;

/** Type of ch2_swhssrc0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_swhs_req_src : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_req_src:1;
        /** ch2_swhs_req_src_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_req_src_we:1;
        /** ch2_swhs_sglreq_src : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_sglreq_src:1;
        /** ch2_swhs_sglreq_src_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_sglreq_src_we:1;
        /** ch2_swhs_lst_src : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_lst_src:1;
        /** ch2_swhs_lst_src_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_lst_src_we:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dmac_ch2_swhssrc0_reg_t;

/** Type of ch2_swhsdst0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_swhs_req_dst : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_req_dst:1;
        /** ch2_swhs_req_dst_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_req_dst_we:1;
        /** ch2_swhs_sglreq_dst : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_sglreq_dst:1;
        /** ch2_swhs_sglreq_dst_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_sglreq_dst_we:1;
        /** ch2_swhs_lst_dst : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_lst_dst:1;
        /** ch2_swhs_lst_dst_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch2_swhs_lst_dst_we:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dmac_ch2_swhsdst0_reg_t;

/** Type of ch2_blk_tfr_resumereq0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_blk_tfr_resumereq : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_blk_tfr_resumereq:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dmac_ch2_blk_tfr_resumereq0_reg_t;

/** Type of ch2_axi_id0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_axi_read_id_suffix : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_axi_read_id_suffix:1;
        uint32_t reserved_1:15;
        /** ch2_axi_write_id_suffix : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch2_axi_write_id_suffix:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dmac_ch2_axi_id0_reg_t;

/** Type of ch2_axi_qos0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_axi_awqos : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t ch2_axi_awqos:4;
        /** ch2_axi_arqos : R/W; bitpos: [7:4]; default: 0;
         *  NA
         */
        uint32_t ch2_axi_arqos:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} dmac_ch2_axi_qos0_reg_t;

/** Type of ch3_sar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_sar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_sar0:32;
    };
    uint32_t val;
} dmac_ch3_sar0_reg_t;

/** Type of ch3_sar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_sar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_sar1:32;
    };
    uint32_t val;
} dmac_ch3_sar1_reg_t;

/** Type of ch3_dar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_dar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_dar0:32;
    };
    uint32_t val;
} dmac_ch3_dar0_reg_t;

/** Type of ch3_dar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_dar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_dar1:32;
    };
    uint32_t val;
} dmac_ch3_dar1_reg_t;

/** Type of ch3_block_ts0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_block_ts : R/W; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t ch3_block_ts:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} dmac_ch3_block_ts0_reg_t;

/** Type of ch3_ctl0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_sms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_sms:1;
        uint32_t reserved_1:1;
        /** ch3_dms : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_dms:1;
        uint32_t reserved_3:1;
        /** ch3_sinc : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch3_sinc:1;
        uint32_t reserved_5:1;
        /** ch3_dinc : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch3_dinc:1;
        uint32_t reserved_7:1;
        /** ch3_src_tr_width : R/W; bitpos: [10:8]; default: 2;
         *  NA
         */
        uint32_t ch3_src_tr_width:3;
        /** ch3_dst_tr_width : R/W; bitpos: [13:11]; default: 2;
         *  NA
         */
        uint32_t ch3_dst_tr_width:3;
        /** ch3_src_msize : R/W; bitpos: [17:14]; default: 0;
         *  NA
         */
        uint32_t ch3_src_msize:4;
        /** ch3_dst_msize : R/W; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t ch3_dst_msize:4;
        /** ch3_ar_cache : R/W; bitpos: [25:22]; default: 0;
         *  NA
         */
        uint32_t ch3_ar_cache:4;
        /** ch3_aw_cache : R/W; bitpos: [29:26]; default: 0;
         *  NA
         */
        uint32_t ch3_aw_cache:4;
        /** ch3_nonposted_lastwrite_en : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch3_nonposted_lastwrite_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} dmac_ch3_ctl0_reg_t;

/** Type of ch3_ctl1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_ar_prot : R/W; bitpos: [2:0]; default: 0;
         *  NA
         */
        uint32_t ch3_ar_prot:3;
        /** ch3_aw_prot : R/W; bitpos: [5:3]; default: 0;
         *  NA
         */
        uint32_t ch3_aw_prot:3;
        /** ch3_arlen_en : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch3_arlen_en:1;
        /** ch3_arlen : R/W; bitpos: [14:7]; default: 0;
         *  NA
         */
        uint32_t ch3_arlen:8;
        /** ch3_awlen_en : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t ch3_awlen_en:1;
        /** ch3_awlen : R/W; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t ch3_awlen:8;
        /** ch3_src_stat_en : R/W; bitpos: [24]; default: 0;
         *  NA
         */
        uint32_t ch3_src_stat_en:1;
        /** ch3_dst_stat_en : R/W; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch3_dst_stat_en:1;
        /** ch3_ioc_blktfr : R/W; bitpos: [26]; default: 0;
         *  NA
         */
        uint32_t ch3_ioc_blktfr:1;
        uint32_t reserved_27:3;
        /** ch3_shadowreg_or_lli_last : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch3_shadowreg_or_lli_last:1;
        /** ch3_shadowreg_or_lli_valid : R/W; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch3_shadowreg_or_lli_valid:1;
    };
    uint32_t val;
} dmac_ch3_ctl1_reg_t;

/** Type of ch3_cfg0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_src_multblk_type : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t ch3_src_multblk_type:2;
        /** ch3_dst_multblk_type : R/W; bitpos: [3:2]; default: 0;
         *  NA
         */
        uint32_t ch3_dst_multblk_type:2;
        uint32_t reserved_4:14;
        /** ch3_rd_uid : RO; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t ch3_rd_uid:4;
        uint32_t reserved_22:3;
        /** ch3_wr_uid : RO; bitpos: [28:25]; default: 0;
         *  NA
         */
        uint32_t ch3_wr_uid:4;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} dmac_ch3_cfg0_reg_t;

/** Type of ch3_cfg1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_tt_fc : R/W; bitpos: [2:0]; default: 3;
         *  NA
         */
        uint32_t ch3_tt_fc:3;
        /** ch3_hs_sel_src : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch3_hs_sel_src:1;
        /** ch3_hs_sel_dst : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch3_hs_sel_dst:1;
        /** ch3_src_hwhs_pol : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch3_src_hwhs_pol:1;
        /** ch3_dst_hwhs_pol : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch3_dst_hwhs_pol:1;
        /** ch3_src_per : R/W; bitpos: [8:7]; default: 0;
         *  NA
         */
        uint32_t ch3_src_per:2;
        uint32_t reserved_9:3;
        /** ch3_dst_per : R/W; bitpos: [13:12]; default: 0;
         *  NA
         */
        uint32_t ch3_dst_per:2;
        uint32_t reserved_14:3;
        /** ch3_ch_prior : R/W; bitpos: [19:17]; default: 1;
         *  NA
         */
        uint32_t ch3_ch_prior:3;
        /** ch3_lock_ch : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch3_lock_ch:1;
        /** ch3_lock_ch_l : RO; bitpos: [22:21]; default: 0;
         *  NA
         */
        uint32_t ch3_lock_ch_l:2;
        /** ch3_src_osr_lmt : R/W; bitpos: [26:23]; default: 0;
         *  NA
         */
        uint32_t ch3_src_osr_lmt:4;
        /** ch3_dst_osr_lmt : R/W; bitpos: [30:27]; default: 0;
         *  NA
         */
        uint32_t ch3_dst_osr_lmt:4;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} dmac_ch3_cfg1_reg_t;

/** Type of ch3_llp0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_lms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_lms:1;
        uint32_t reserved_1:5;
        /** ch3_loc0 : R/W; bitpos: [31:6]; default: 0;
         *  NA
         */
        uint32_t ch3_loc0:26;
    };
    uint32_t val;
} dmac_ch3_llp0_reg_t;

/** Type of ch3_llp1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_loc1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_loc1:32;
    };
    uint32_t val;
} dmac_ch3_llp1_reg_t;

/** Type of ch3_swhssrc0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_swhs_req_src : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_req_src:1;
        /** ch3_swhs_req_src_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_req_src_we:1;
        /** ch3_swhs_sglreq_src : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_sglreq_src:1;
        /** ch3_swhs_sglreq_src_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_sglreq_src_we:1;
        /** ch3_swhs_lst_src : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_lst_src:1;
        /** ch3_swhs_lst_src_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_lst_src_we:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dmac_ch3_swhssrc0_reg_t;

/** Type of ch3_swhsdst0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_swhs_req_dst : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_req_dst:1;
        /** ch3_swhs_req_dst_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_req_dst_we:1;
        /** ch3_swhs_sglreq_dst : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_sglreq_dst:1;
        /** ch3_swhs_sglreq_dst_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_sglreq_dst_we:1;
        /** ch3_swhs_lst_dst : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_lst_dst:1;
        /** ch3_swhs_lst_dst_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch3_swhs_lst_dst_we:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dmac_ch3_swhsdst0_reg_t;

/** Type of ch3_blk_tfr_resumereq0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_blk_tfr_resumereq : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_blk_tfr_resumereq:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dmac_ch3_blk_tfr_resumereq0_reg_t;

/** Type of ch3_axi_id0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_axi_read_id_suffix : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_axi_read_id_suffix:1;
        uint32_t reserved_1:15;
        /** ch3_axi_write_id_suffix : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch3_axi_write_id_suffix:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dmac_ch3_axi_id0_reg_t;

/** Type of ch3_axi_qos0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_axi_awqos : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t ch3_axi_awqos:4;
        /** ch3_axi_arqos : R/W; bitpos: [7:4]; default: 0;
         *  NA
         */
        uint32_t ch3_axi_arqos:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} dmac_ch3_axi_qos0_reg_t;

/** Type of ch4_sar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_sar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_sar0:32;
    };
    uint32_t val;
} dmac_ch4_sar0_reg_t;

/** Type of ch4_sar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_sar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_sar1:32;
    };
    uint32_t val;
} dmac_ch4_sar1_reg_t;

/** Type of ch4_dar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_dar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_dar0:32;
    };
    uint32_t val;
} dmac_ch4_dar0_reg_t;

/** Type of ch4_dar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_dar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_dar1:32;
    };
    uint32_t val;
} dmac_ch4_dar1_reg_t;

/** Type of ch4_block_ts0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_block_ts : R/W; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t ch4_block_ts:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} dmac_ch4_block_ts0_reg_t;

/** Type of ch4_ctl0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_sms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_sms:1;
        uint32_t reserved_1:1;
        /** ch4_dms : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch4_dms:1;
        uint32_t reserved_3:1;
        /** ch4_sinc : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch4_sinc:1;
        uint32_t reserved_5:1;
        /** ch4_dinc : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch4_dinc:1;
        uint32_t reserved_7:1;
        /** ch4_src_tr_width : R/W; bitpos: [10:8]; default: 2;
         *  NA
         */
        uint32_t ch4_src_tr_width:3;
        /** ch4_dst_tr_width : R/W; bitpos: [13:11]; default: 2;
         *  NA
         */
        uint32_t ch4_dst_tr_width:3;
        /** ch4_src_msize : R/W; bitpos: [17:14]; default: 0;
         *  NA
         */
        uint32_t ch4_src_msize:4;
        /** ch4_dst_msize : R/W; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t ch4_dst_msize:4;
        /** ch4_ar_cache : R/W; bitpos: [25:22]; default: 0;
         *  NA
         */
        uint32_t ch4_ar_cache:4;
        /** ch4_aw_cache : R/W; bitpos: [29:26]; default: 0;
         *  NA
         */
        uint32_t ch4_aw_cache:4;
        /** ch4_nonposted_lastwrite_en : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch4_nonposted_lastwrite_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} dmac_ch4_ctl0_reg_t;

/** Type of ch4_ctl1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_ar_prot : R/W; bitpos: [2:0]; default: 0;
         *  NA
         */
        uint32_t ch4_ar_prot:3;
        /** ch4_aw_prot : R/W; bitpos: [5:3]; default: 0;
         *  NA
         */
        uint32_t ch4_aw_prot:3;
        /** ch4_arlen_en : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch4_arlen_en:1;
        /** ch4_arlen : R/W; bitpos: [14:7]; default: 0;
         *  NA
         */
        uint32_t ch4_arlen:8;
        /** ch4_awlen_en : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t ch4_awlen_en:1;
        /** ch4_awlen : R/W; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t ch4_awlen:8;
        /** ch4_src_stat_en : R/W; bitpos: [24]; default: 0;
         *  NA
         */
        uint32_t ch4_src_stat_en:1;
        /** ch4_dst_stat_en : R/W; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch4_dst_stat_en:1;
        /** ch4_ioc_blktfr : R/W; bitpos: [26]; default: 0;
         *  NA
         */
        uint32_t ch4_ioc_blktfr:1;
        uint32_t reserved_27:3;
        /** ch4_shadowreg_or_lli_last : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch4_shadowreg_or_lli_last:1;
        /** ch4_shadowreg_or_lli_valid : R/W; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch4_shadowreg_or_lli_valid:1;
    };
    uint32_t val;
} dmac_ch4_ctl1_reg_t;

/** Type of ch4_cfg0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_src_multblk_type : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t ch4_src_multblk_type:2;
        /** ch4_dst_multblk_type : R/W; bitpos: [3:2]; default: 0;
         *  NA
         */
        uint32_t ch4_dst_multblk_type:2;
        uint32_t reserved_4:14;
        /** ch4_rd_uid : RO; bitpos: [21:18]; default: 0;
         *  NA
         */
        uint32_t ch4_rd_uid:4;
        uint32_t reserved_22:3;
        /** ch4_wr_uid : RO; bitpos: [28:25]; default: 0;
         *  NA
         */
        uint32_t ch4_wr_uid:4;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} dmac_ch4_cfg0_reg_t;

/** Type of ch4_cfg1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_tt_fc : R/W; bitpos: [2:0]; default: 3;
         *  NA
         */
        uint32_t ch4_tt_fc:3;
        /** ch4_hs_sel_src : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch4_hs_sel_src:1;
        /** ch4_hs_sel_dst : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch4_hs_sel_dst:1;
        /** ch4_src_hwhs_pol : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch4_src_hwhs_pol:1;
        /** ch4_dst_hwhs_pol : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch4_dst_hwhs_pol:1;
        /** ch4_src_per : R/W; bitpos: [8:7]; default: 0;
         *  NA
         */
        uint32_t ch4_src_per:2;
        uint32_t reserved_9:3;
        /** ch4_dst_per : R/W; bitpos: [13:12]; default: 0;
         *  NA
         */
        uint32_t ch4_dst_per:2;
        uint32_t reserved_14:3;
        /** ch4_ch_prior : R/W; bitpos: [19:17]; default: 0;
         *  NA
         */
        uint32_t ch4_ch_prior:3;
        /** ch4_lock_ch : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch4_lock_ch:1;
        /** ch4_lock_ch_l : RO; bitpos: [22:21]; default: 0;
         *  NA
         */
        uint32_t ch4_lock_ch_l:2;
        /** ch4_src_osr_lmt : R/W; bitpos: [26:23]; default: 0;
         *  NA
         */
        uint32_t ch4_src_osr_lmt:4;
        /** ch4_dst_osr_lmt : R/W; bitpos: [30:27]; default: 0;
         *  NA
         */
        uint32_t ch4_dst_osr_lmt:4;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} dmac_ch4_cfg1_reg_t;

/** Type of ch4_llp0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_lms : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_lms:1;
        uint32_t reserved_1:5;
        /** ch4_loc0 : R/W; bitpos: [31:6]; default: 0;
         *  NA
         */
        uint32_t ch4_loc0:26;
    };
    uint32_t val;
} dmac_ch4_llp0_reg_t;

/** Type of ch4_llp1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_loc1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_loc1:32;
    };
    uint32_t val;
} dmac_ch4_llp1_reg_t;

/** Type of ch4_swhssrc0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_swhs_req_src : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_req_src:1;
        /** ch4_swhs_req_src_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_req_src_we:1;
        /** ch4_swhs_sglreq_src : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_sglreq_src:1;
        /** ch4_swhs_sglreq_src_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_sglreq_src_we:1;
        /** ch4_swhs_lst_src : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_lst_src:1;
        /** ch4_swhs_lst_src_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_lst_src_we:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dmac_ch4_swhssrc0_reg_t;

/** Type of ch4_swhsdst0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_swhs_req_dst : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_req_dst:1;
        /** ch4_swhs_req_dst_we : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_req_dst_we:1;
        /** ch4_swhs_sglreq_dst : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_sglreq_dst:1;
        /** ch4_swhs_sglreq_dst_we : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_sglreq_dst_we:1;
        /** ch4_swhs_lst_dst : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_lst_dst:1;
        /** ch4_swhs_lst_dst_we : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch4_swhs_lst_dst_we:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dmac_ch4_swhsdst0_reg_t;

/** Type of ch4_blk_tfr_resumereq0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_blk_tfr_resumereq : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_blk_tfr_resumereq:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dmac_ch4_blk_tfr_resumereq0_reg_t;

/** Type of ch4_axi_id0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_axi_read_id_suffix : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_axi_read_id_suffix:1;
        uint32_t reserved_1:15;
        /** ch4_axi_write_id_suffix : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch4_axi_write_id_suffix:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dmac_ch4_axi_id0_reg_t;

/** Type of ch4_axi_qos0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_axi_awqos : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t ch4_axi_awqos:4;
        /** ch4_axi_arqos : R/W; bitpos: [7:4]; default: 0;
         *  NA
         */
        uint32_t ch4_axi_arqos:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} dmac_ch4_axi_qos0_reg_t;


/** Group: Interrupt Registers */
/** Type of intstatus0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_intstat:1;
        /** ch2_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_intstat:1;
        /** ch3_intstat : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_intstat:1;
        /** ch4_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_intstat:1;
        uint32_t reserved_4:12;
        /** commonreg_intstat : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t commonreg_intstat:1;
        uint32_t reserved_17:15;
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
        uint32_t clear_slvif_commonreg_dec_err_intstat:1;
        /** clear_slvif_commonreg_wr2ro_err_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_commonreg_wr2ro_err_intstat:1;
        /** clear_slvif_commonreg_rd2wo_err_intstat : WO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_commonreg_rd2wo_err_intstat:1;
        /** clear_slvif_commonreg_wronhold_err_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_commonreg_wronhold_err_intstat:1;
        uint32_t reserved_4:3;
        /** clear_slvif_commonreg_wrparity_err_intstat : WO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_commonreg_wrparity_err_intstat:1;
        /** clear_slvif_undefinedreg_dec_err_intstat : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t clear_slvif_undefinedreg_dec_err_intstat:1;
        /** clear_mxif1_rch0_eccprot_correrr_intstat : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_rch0_eccprot_correrr_intstat:1;
        /** clear_mxif1_rch0_eccprot_uncorrerr_intstat : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_rch0_eccprot_uncorrerr_intstat:1;
        /** clear_mxif1_rch1_eccprot_correrr_intstat : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_rch1_eccprot_correrr_intstat:1;
        /** clear_mxif1_rch1_eccprot_uncorrerr_intstat : WO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_rch1_eccprot_uncorrerr_intstat:1;
        /** clear_mxif1_bch_eccprot_correrr_intstat : WO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_bch_eccprot_correrr_intstat:1;
        /** clear_mxif1_bch_eccprot_uncorrerr_intstat : WO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t clear_mxif1_bch_eccprot_uncorrerr_intstat:1;
        /** clear_mxif2_rch0_eccprot_correrr_intstat : WO; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_rch0_eccprot_correrr_intstat:1;
        /** clear_mxif2_rch0_eccprot_uncorrerr_intstat : WO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_rch0_eccprot_uncorrerr_intstat:1;
        /** clear_mxif2_rch1_eccprot_correrr_intstat : WO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_rch1_eccprot_correrr_intstat:1;
        /** clear_mxif2_rch1_eccprot_uncorrerr_intstat : WO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_rch1_eccprot_uncorrerr_intstat:1;
        /** clear_mxif2_bch_eccprot_correrr_intstat : WO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_bch_eccprot_correrr_intstat:1;
        /** clear_mxif2_bch_eccprot_uncorrerr_intstat : WO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t clear_mxif2_bch_eccprot_uncorrerr_intstat:1;
        uint32_t reserved_21:11;
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
        uint32_t enable_slvif_commonreg_dec_err_intstat:1;
        /** enable_slvif_commonreg_wr2ro_err_intstat : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wr2ro_err_intstat:1;
        /** enable_slvif_commonreg_rd2wo_err_intstat : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_rd2wo_err_intstat:1;
        /** enable_slvif_commonreg_wronhold_err_intstat : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wronhold_err_intstat:1;
        uint32_t reserved_4:3;
        /** enable_slvif_commonreg_wrparity_err_intstat : RO; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wrparity_err_intstat:1;
        /** enable_slvif_undefinedreg_dec_err_intstat : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_undefinedreg_dec_err_intstat:1;
        /** enable_mxif1_rch0_eccprot_correrr_intstat : RO; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch0_eccprot_correrr_intstat:1;
        /** enable_mxif1_rch0_eccprot_uncorrerr_intstat : RO; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch0_eccprot_uncorrerr_intstat:1;
        /** enable_mxif1_rch1_eccprot_correrr_intstat : RO; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch1_eccprot_correrr_intstat:1;
        /** enable_mxif1_rch1_eccprot_uncorrerr_intstat : RO; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch1_eccprot_uncorrerr_intstat:1;
        /** enable_mxif1_bch_eccprot_correrr_intstat : RO; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_bch_eccprot_correrr_intstat:1;
        /** enable_mxif1_bch_eccprot_uncorrerr_intstat : RO; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_bch_eccprot_uncorrerr_intstat:1;
        /** enable_mxif2_rch0_eccprot_correrr_intstat : RO; bitpos: [15]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch0_eccprot_correrr_intstat:1;
        /** enable_mxif2_rch0_eccprot_uncorrerr_intstat : RO; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch0_eccprot_uncorrerr_intstat:1;
        /** enable_mxif2_rch1_eccprot_correrr_intstat : RO; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch1_eccprot_correrr_intstat:1;
        /** enable_mxif2_rch1_eccprot_uncorrerr_intstat : RO; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch1_eccprot_uncorrerr_intstat:1;
        /** enable_mxif2_bch_eccprot_correrr_intstat : RO; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_bch_eccprot_correrr_intstat:1;
        /** enable_mxif2_bch_eccprot_uncorrerr_intstat : RO; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_bch_eccprot_uncorrerr_intstat:1;
        uint32_t reserved_21:11;
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
        uint32_t enable_slvif_commonreg_dec_err_intsignal:1;
        /** enable_slvif_commonreg_wr2ro_err_intsignal : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wr2ro_err_intsignal:1;
        /** enable_slvif_commonreg_rd2wo_err_intsignal : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_rd2wo_err_intsignal:1;
        /** enable_slvif_commonreg_wronhold_err_intsignal : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wronhold_err_intsignal:1;
        uint32_t reserved_4:3;
        /** enable_slvif_commonreg_wrparity_err_intsignal : RO; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_commonreg_wrparity_err_intsignal:1;
        /** enable_slvif_undefinedreg_dec_err_intsignal : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t enable_slvif_undefinedreg_dec_err_intsignal:1;
        /** enable_mxif1_rch0_eccprot_correrr_intsignal : RO; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch0_eccprot_correrr_intsignal:1;
        /** enable_mxif1_rch0_eccprot_uncorrerr_intsignal : RO; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch0_eccprot_uncorrerr_intsignal:1;
        /** enable_mxif1_rch1_eccprot_correrr_intsignal : RO; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch1_eccprot_correrr_intsignal:1;
        /** enable_mxif1_rch1_eccprot_uncorrerr_intsignal : RO; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_rch1_eccprot_uncorrerr_intsignal:1;
        /** enable_mxif1_bch_eccprot_correrr_intsignal : RO; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_bch_eccprot_correrr_intsignal:1;
        /** enable_mxif1_bch_eccprot_uncorrerr_intsignal : RO; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t enable_mxif1_bch_eccprot_uncorrerr_intsignal:1;
        /** enable_mxif2_rch0_eccprot_correrr_intsignal : RO; bitpos: [15]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch0_eccprot_correrr_intsignal:1;
        /** enable_mxif2_rch0_eccprot_uncorrerr_intsignal : RO; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch0_eccprot_uncorrerr_intsignal:1;
        /** enable_mxif2_rch1_eccprot_correrr_intsignal : RO; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch1_eccprot_correrr_intsignal:1;
        /** enable_mxif2_rch1_eccprot_uncorrerr_intsignal : RO; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_rch1_eccprot_uncorrerr_intsignal:1;
        /** enable_mxif2_bch_eccprot_correrr_intsignal : RO; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_bch_eccprot_correrr_intsignal:1;
        /** enable_mxif2_bch_eccprot_uncorrerr_intsignal : RO; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t enable_mxif2_bch_eccprot_uncorrerr_intsignal:1;
        uint32_t reserved_21:11;
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
        uint32_t slvif_commonreg_dec_err_intstat:1;
        /** slvif_commonreg_wr2ro_err_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t slvif_commonreg_wr2ro_err_intstat:1;
        /** slvif_commonreg_rd2wo_err_intstat : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t slvif_commonreg_rd2wo_err_intstat:1;
        /** slvif_commonreg_wronhold_err_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t slvif_commonreg_wronhold_err_intstat:1;
        uint32_t reserved_4:3;
        /** slvif_commonreg_wrparity_err_intstat : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t slvif_commonreg_wrparity_err_intstat:1;
        /** slvif_undefinedreg_dec_err_intstat : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t slvif_undefinedreg_dec_err_intstat:1;
        /** mxif1_rch0_eccprot_correrr_intstat : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mxif1_rch0_eccprot_correrr_intstat:1;
        /** mxif1_rch0_eccprot_uncorrerr_intstat : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mxif1_rch0_eccprot_uncorrerr_intstat:1;
        /** mxif1_rch1_eccprot_correrr_intstat : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mxif1_rch1_eccprot_correrr_intstat:1;
        /** mxif1_rch1_eccprot_uncorrerr_intstat : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mxif1_rch1_eccprot_uncorrerr_intstat:1;
        /** mxif1_bch_eccprot_correrr_intstat : RO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t mxif1_bch_eccprot_correrr_intstat:1;
        /** mxif1_bch_eccprot_uncorrerr_intstat : RO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t mxif1_bch_eccprot_uncorrerr_intstat:1;
        /** mxif2_rch0_eccprot_correrr_intstat : RO; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t mxif2_rch0_eccprot_correrr_intstat:1;
        /** mxif2_rch0_eccprot_uncorrerr_intstat : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t mxif2_rch0_eccprot_uncorrerr_intstat:1;
        /** mxif2_rch1_eccprot_correrr_intstat : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t mxif2_rch1_eccprot_correrr_intstat:1;
        /** mxif2_rch1_eccprot_uncorrerr_intstat : RO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t mxif2_rch1_eccprot_uncorrerr_intstat:1;
        /** mxif2_bch_eccprot_correrr_intstat : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t mxif2_bch_eccprot_correrr_intstat:1;
        /** mxif2_bch_eccprot_uncorrerr_intstat : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t mxif2_bch_eccprot_uncorrerr_intstat:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} dmac_commonreg_intstatus0_reg_t;

/** Type of ch1_intstatus_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_enable_block_tfr_done_intstat : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_block_tfr_done_intstat:1;
        /** ch1_enable_dma_tfr_done_intstat : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch1_enable_src_transcomp_intstat : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_src_transcomp_intstat:1;
        /** ch1_enable_dst_transcomp_intstat : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_dst_transcomp_intstat:1;
        /** ch1_enable_src_dec_err_intstat : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_src_dec_err_intstat:1;
        /** ch1_enable_dst_dec_err_intstat : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_dst_dec_err_intstat:1;
        /** ch1_enable_src_slv_err_intstat : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_src_slv_err_intstat:1;
        /** ch1_enable_dst_slv_err_intstat : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_dst_slv_err_intstat:1;
        /** ch1_enable_lli_rd_dec_err_intstat : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_lli_rd_dec_err_intstat:1;
        /** ch1_enable_lli_wr_dec_err_intstat : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_lli_wr_dec_err_intstat:1;
        /** ch1_enable_lli_rd_slv_err_intstat : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_lli_rd_slv_err_intstat:1;
        /** ch1_enable_lli_wr_slv_err_intstat : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_lli_wr_slv_err_intstat:1;
        /** ch1_enable_shadowreg_or_lli_invalid_err_intstat : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch1_enable_slvif_multiblktype_err_intstat : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch1_enable_slvif_dec_err_intstat : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_dec_err_intstat:1;
        /** ch1_enable_slvif_wr2ro_err_intstat : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_wr2ro_err_intstat:1;
        /** ch1_enable_slvif_rd2rwo_err_intstat : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_rd2rwo_err_intstat:1;
        /** ch1_enable_slvif_wronchen_err_intstat : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_wronchen_err_intstat:1;
        /** ch1_enable_slvif_shadowreg_wron_valid_err_intstat : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch1_enable_slvif_wronhold_err_intstat : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch1_enable_slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch1_enable_ch_lock_cleared_intstat : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_lock_cleared_intstat:1;
        /** ch1_enable_ch_src_suspended_intstat : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_src_suspended_intstat:1;
        /** ch1_enable_ch_suspended_intstat : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_suspended_intstat:1;
        /** ch1_enable_ch_disabled_intstat : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_disabled_intstat:1;
        /** ch1_enable_ch_aborted_intstat : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch1_intstatus_enable0_reg_t;

/** Type of ch1_intstatus_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_enable_ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ecc_prot_chmem_correrr_intstat:1;
        /** ch1_enable_ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch1_enable_ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ecc_prot_uidmem_correrr_intstat:1;
        /** ch1_enable_ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch1_intstatus_enable1_reg_t;

/** Type of ch1_intstatus0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_block_tfr_done_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_block_tfr_done_intstat:1;
        /** ch1_dma_tfr_done_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch1_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch1_src_transcomp_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch1_src_transcomp_intstat:1;
        /** ch1_dst_transcomp_intstat : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch1_dst_transcomp_intstat:1;
        /** ch1_src_dec_err_intstat : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch1_src_dec_err_intstat:1;
        /** ch1_dst_dec_err_intstat : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch1_dst_dec_err_intstat:1;
        /** ch1_src_slv_err_intstat : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t ch1_src_slv_err_intstat:1;
        /** ch1_dst_slv_err_intstat : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch1_dst_slv_err_intstat:1;
        /** ch1_lli_rd_dec_err_intstat : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch1_lli_rd_dec_err_intstat:1;
        /** ch1_lli_wr_dec_err_intstat : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch1_lli_wr_dec_err_intstat:1;
        /** ch1_lli_rd_slv_err_intstat : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch1_lli_rd_slv_err_intstat:1;
        /** ch1_lli_wr_slv_err_intstat : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ch1_lli_wr_slv_err_intstat:1;
        /** ch1_shadowreg_or_lli_invalid_err_intstat : RO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ch1_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch1_slvif_multiblktype_err_intstat : RO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ch1_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch1_slvif_dec_err_intstat : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch1_slvif_dec_err_intstat:1;
        /** ch1_slvif_wr2ro_err_intstat : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch1_slvif_wr2ro_err_intstat:1;
        /** ch1_slvif_rd2rwo_err_intstat : RO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch1_slvif_rd2rwo_err_intstat:1;
        /** ch1_slvif_wronchen_err_intstat : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch1_slvif_wronchen_err_intstat:1;
        /** ch1_slvif_shadowreg_wron_valid_err_intstat : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch1_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch1_slvif_wronhold_err_intstat : RO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t ch1_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch1_slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch1_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch1_ch_lock_cleared_intstat : RO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch1_ch_lock_cleared_intstat:1;
        /** ch1_ch_src_suspended_intstat : RO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t ch1_ch_src_suspended_intstat:1;
        /** ch1_ch_suspended_intstat : RO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t ch1_ch_suspended_intstat:1;
        /** ch1_ch_disabled_intstat : RO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch1_ch_disabled_intstat:1;
        /** ch1_ch_aborted_intstat : RO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch1_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch1_intstatus0_reg_t;

/** Type of ch1_intstatus1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_ecc_prot_chmem_correrr_intstat:1;
        /** ch1_ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch1_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch1_ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch1_ecc_prot_uidmem_correrr_intstat:1;
        /** ch1_ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch1_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch1_intstatus1_reg_t;

/** Type of ch1_intsignal_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_enable_block_tfr_done_intsignal : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_block_tfr_done_intsignal:1;
        /** ch1_enable_dma_tfr_done_intsignal : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_dma_tfr_done_intsignal:1;
        uint32_t reserved_2:1;
        /** ch1_enable_src_transcomp_intsignal : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_src_transcomp_intsignal:1;
        /** ch1_enable_dst_transcomp_intsignal : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_dst_transcomp_intsignal:1;
        /** ch1_enable_src_dec_err_intsignal : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_src_dec_err_intsignal:1;
        /** ch1_enable_dst_dec_err_intsignal : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_dst_dec_err_intsignal:1;
        /** ch1_enable_src_slv_err_intsignal : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_src_slv_err_intsignal:1;
        /** ch1_enable_dst_slv_err_intsignal : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_dst_slv_err_intsignal:1;
        /** ch1_enable_lli_rd_dec_err_intsignal : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_lli_rd_dec_err_intsignal:1;
        /** ch1_enable_lli_wr_dec_err_intsignal : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_lli_wr_dec_err_intsignal:1;
        /** ch1_enable_lli_rd_slv_err_intsignal : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_lli_rd_slv_err_intsignal:1;
        /** ch1_enable_lli_wr_slv_err_intsignal : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_lli_wr_slv_err_intsignal:1;
        /** ch1_enable_shadowreg_or_lli_invalid_err_intsignal : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_shadowreg_or_lli_invalid_err_intsignal:1;
        /** ch1_enable_slvif_multiblktype_err_intsignal : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_multiblktype_err_intsignal:1;
        uint32_t reserved_15:1;
        /** ch1_enable_slvif_dec_err_intsignal : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_dec_err_intsignal:1;
        /** ch1_enable_slvif_wr2ro_err_intsignal : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_wr2ro_err_intsignal:1;
        /** ch1_enable_slvif_rd2rwo_err_intsignal : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_rd2rwo_err_intsignal:1;
        /** ch1_enable_slvif_wronchen_err_intsignal : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_wronchen_err_intsignal:1;
        /** ch1_enable_slvif_shadowreg_wron_valid_err_intsignal : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_shadowreg_wron_valid_err_intsignal:1;
        /** ch1_enable_slvif_wronhold_err_intsignal : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_wronhold_err_intsignal:1;
        uint32_t reserved_22:3;
        /** ch1_enable_slvif_wrparity_err_intsignal : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_slvif_wrparity_err_intsignal:1;
        uint32_t reserved_26:1;
        /** ch1_enable_ch_lock_cleared_intsignal : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_lock_cleared_intsignal:1;
        /** ch1_enable_ch_src_suspended_intsignal : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_src_suspended_intsignal:1;
        /** ch1_enable_ch_suspended_intsignal : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_suspended_intsignal:1;
        /** ch1_enable_ch_disabled_intsignal : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_disabled_intsignal:1;
        /** ch1_enable_ch_aborted_intsignal : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ch_aborted_intsignal:1;
    };
    uint32_t val;
} dmac_ch1_intsignal_enable0_reg_t;

/** Type of ch1_intsignal_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_enable_ecc_prot_chmem_correrr_intsignal : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ecc_prot_chmem_correrr_intsignal:1;
        /** ch1_enable_ecc_prot_chmem_uncorrerr_intsignal : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ecc_prot_chmem_uncorrerr_intsignal:1;
        /** ch1_enable_ecc_prot_uidmem_correrr_intsignal : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ecc_prot_uidmem_correrr_intsignal:1;
        /** ch1_enable_ecc_prot_uidmem_uncorrerr_intsignal : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch1_enable_ecc_prot_uidmem_uncorrerr_intsignal:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch1_intsignal_enable1_reg_t;

/** Type of ch1_intclear0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_clear_block_tfr_done_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_block_tfr_done_intstat:1;
        /** ch1_clear_dma_tfr_done_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch1_clear_src_transcomp_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_src_transcomp_intstat:1;
        /** ch1_clear_dst_transcomp_intstat : WO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_dst_transcomp_intstat:1;
        /** ch1_clear_src_dec_err_intstat : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_src_dec_err_intstat:1;
        /** ch1_clear_dst_dec_err_intstat : WO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_dst_dec_err_intstat:1;
        /** ch1_clear_src_slv_err_intstat : WO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_src_slv_err_intstat:1;
        /** ch1_clear_dst_slv_err_intstat : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_dst_slv_err_intstat:1;
        /** ch1_clear_lli_rd_dec_err_intstat : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_lli_rd_dec_err_intstat:1;
        /** ch1_clear_lli_wr_dec_err_intstat : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_lli_wr_dec_err_intstat:1;
        /** ch1_clear_lli_rd_slv_err_intstat : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_lli_rd_slv_err_intstat:1;
        /** ch1_clear_lli_wr_slv_err_intstat : WO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_lli_wr_slv_err_intstat:1;
        /** ch1_clear_shadowreg_or_lli_invalid_err_intstat : WO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch1_clear_slvif_multiblktype_err_intstat : WO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch1_clear_slvif_dec_err_intstat : WO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_slvif_dec_err_intstat:1;
        /** ch1_clear_slvif_wr2ro_err_intstat : WO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_slvif_wr2ro_err_intstat:1;
        /** ch1_clear_slvif_rd2rwo_err_intstat : WO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_slvif_rd2rwo_err_intstat:1;
        /** ch1_clear_slvif_wronchen_err_intstat : WO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_slvif_wronchen_err_intstat:1;
        /** ch1_clear_slvif_shadowreg_wron_valid_err_intstat : WO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch1_clear_slvif_wronhold_err_intstat : WO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch1_clear_slvif_wrparity_err_intstat : WO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch1_clear_ch_lock_cleared_intstat : WO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_ch_lock_cleared_intstat:1;
        /** ch1_clear_ch_src_suspended_intstat : WO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_ch_src_suspended_intstat:1;
        /** ch1_clear_ch_suspended_intstat : WO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_ch_suspended_intstat:1;
        /** ch1_clear_ch_disabled_intstat : WO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_ch_disabled_intstat:1;
        /** ch1_clear_ch_aborted_intstat : WO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch1_intclear0_reg_t;

/** Type of ch1_intclear1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_clear_ecc_prot_chmem_correrr_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_ecc_prot_chmem_correrr_intstat:1;
        /** ch1_clear_ecc_prot_chmem_uncorrerr_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch1_clear_ecc_prot_uidmem_correrr_intstat : WO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_ecc_prot_uidmem_correrr_intstat:1;
        /** ch1_clear_ecc_prot_uidmem_uncorrerr_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch1_clear_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch1_intclear1_reg_t;

/** Type of ch2_intstatus_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_enable_block_tfr_done_intstat : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_block_tfr_done_intstat:1;
        /** ch2_enable_dma_tfr_done_intstat : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch2_enable_src_transcomp_intstat : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_src_transcomp_intstat:1;
        /** ch2_enable_dst_transcomp_intstat : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_dst_transcomp_intstat:1;
        /** ch2_enable_src_dec_err_intstat : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_src_dec_err_intstat:1;
        /** ch2_enable_dst_dec_err_intstat : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_dst_dec_err_intstat:1;
        /** ch2_enable_src_slv_err_intstat : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_src_slv_err_intstat:1;
        /** ch2_enable_dst_slv_err_intstat : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_dst_slv_err_intstat:1;
        /** ch2_enable_lli_rd_dec_err_intstat : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_lli_rd_dec_err_intstat:1;
        /** ch2_enable_lli_wr_dec_err_intstat : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_lli_wr_dec_err_intstat:1;
        /** ch2_enable_lli_rd_slv_err_intstat : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_lli_rd_slv_err_intstat:1;
        /** ch2_enable_lli_wr_slv_err_intstat : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_lli_wr_slv_err_intstat:1;
        /** ch2_enable_shadowreg_or_lli_invalid_err_intstat : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch2_enable_slvif_multiblktype_err_intstat : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch2_enable_slvif_dec_err_intstat : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_dec_err_intstat:1;
        /** ch2_enable_slvif_wr2ro_err_intstat : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_wr2ro_err_intstat:1;
        /** ch2_enable_slvif_rd2rwo_err_intstat : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_rd2rwo_err_intstat:1;
        /** ch2_enable_slvif_wronchen_err_intstat : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_wronchen_err_intstat:1;
        /** ch2_enable_slvif_shadowreg_wron_valid_err_intstat : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch2_enable_slvif_wronhold_err_intstat : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch2_enable_slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch2_enable_ch_lock_cleared_intstat : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_lock_cleared_intstat:1;
        /** ch2_enable_ch_src_suspended_intstat : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_src_suspended_intstat:1;
        /** ch2_enable_ch_suspended_intstat : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_suspended_intstat:1;
        /** ch2_enable_ch_disabled_intstat : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_disabled_intstat:1;
        /** ch2_enable_ch_aborted_intstat : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch2_intstatus_enable0_reg_t;

/** Type of ch2_intstatus_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_enable_ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ecc_prot_chmem_correrr_intstat:1;
        /** ch2_enable_ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch2_enable_ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ecc_prot_uidmem_correrr_intstat:1;
        /** ch2_enable_ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch2_intstatus_enable1_reg_t;

/** Type of ch2_intstatus0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_block_tfr_done_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_block_tfr_done_intstat:1;
        /** ch2_dma_tfr_done_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch2_src_transcomp_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch2_src_transcomp_intstat:1;
        /** ch2_dst_transcomp_intstat : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch2_dst_transcomp_intstat:1;
        /** ch2_src_dec_err_intstat : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch2_src_dec_err_intstat:1;
        /** ch2_dst_dec_err_intstat : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch2_dst_dec_err_intstat:1;
        /** ch2_src_slv_err_intstat : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t ch2_src_slv_err_intstat:1;
        /** ch2_dst_slv_err_intstat : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch2_dst_slv_err_intstat:1;
        /** ch2_lli_rd_dec_err_intstat : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch2_lli_rd_dec_err_intstat:1;
        /** ch2_lli_wr_dec_err_intstat : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch2_lli_wr_dec_err_intstat:1;
        /** ch2_lli_rd_slv_err_intstat : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch2_lli_rd_slv_err_intstat:1;
        /** ch2_lli_wr_slv_err_intstat : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ch2_lli_wr_slv_err_intstat:1;
        /** ch2_shadowreg_or_lli_invalid_err_intstat : RO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ch2_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch2_slvif_multiblktype_err_intstat : RO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ch2_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch2_slvif_dec_err_intstat : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch2_slvif_dec_err_intstat:1;
        /** ch2_slvif_wr2ro_err_intstat : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch2_slvif_wr2ro_err_intstat:1;
        /** ch2_slvif_rd2rwo_err_intstat : RO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch2_slvif_rd2rwo_err_intstat:1;
        /** ch2_slvif_wronchen_err_intstat : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch2_slvif_wronchen_err_intstat:1;
        /** ch2_slvif_shadowreg_wron_valid_err_intstat : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch2_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch2_slvif_wronhold_err_intstat : RO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t ch2_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch2_slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch2_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch2_ch_lock_cleared_intstat : RO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch2_ch_lock_cleared_intstat:1;
        /** ch2_ch_src_suspended_intstat : RO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t ch2_ch_src_suspended_intstat:1;
        /** ch2_ch_suspended_intstat : RO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t ch2_ch_suspended_intstat:1;
        /** ch2_ch_disabled_intstat : RO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch2_ch_disabled_intstat:1;
        /** ch2_ch_aborted_intstat : RO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch2_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch2_intstatus0_reg_t;

/** Type of ch2_intstatus1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_ecc_prot_chmem_correrr_intstat:1;
        /** ch2_ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch2_ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch2_ecc_prot_uidmem_correrr_intstat:1;
        /** ch2_ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch2_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch2_intstatus1_reg_t;

/** Type of ch2_intsignal_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_enable_block_tfr_done_intsignal : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_block_tfr_done_intsignal:1;
        /** ch2_enable_dma_tfr_done_intsignal : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_dma_tfr_done_intsignal:1;
        uint32_t reserved_2:1;
        /** ch2_enable_src_transcomp_intsignal : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_src_transcomp_intsignal:1;
        /** ch2_enable_dst_transcomp_intsignal : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_dst_transcomp_intsignal:1;
        /** ch2_enable_src_dec_err_intsignal : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_src_dec_err_intsignal:1;
        /** ch2_enable_dst_dec_err_intsignal : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_dst_dec_err_intsignal:1;
        /** ch2_enable_src_slv_err_intsignal : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_src_slv_err_intsignal:1;
        /** ch2_enable_dst_slv_err_intsignal : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_dst_slv_err_intsignal:1;
        /** ch2_enable_lli_rd_dec_err_intsignal : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_lli_rd_dec_err_intsignal:1;
        /** ch2_enable_lli_wr_dec_err_intsignal : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_lli_wr_dec_err_intsignal:1;
        /** ch2_enable_lli_rd_slv_err_intsignal : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_lli_rd_slv_err_intsignal:1;
        /** ch2_enable_lli_wr_slv_err_intsignal : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_lli_wr_slv_err_intsignal:1;
        /** ch2_enable_shadowreg_or_lli_invalid_err_intsignal : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_shadowreg_or_lli_invalid_err_intsignal:1;
        /** ch2_enable_slvif_multiblktype_err_intsignal : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_multiblktype_err_intsignal:1;
        uint32_t reserved_15:1;
        /** ch2_enable_slvif_dec_err_intsignal : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_dec_err_intsignal:1;
        /** ch2_enable_slvif_wr2ro_err_intsignal : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_wr2ro_err_intsignal:1;
        /** ch2_enable_slvif_rd2rwo_err_intsignal : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_rd2rwo_err_intsignal:1;
        /** ch2_enable_slvif_wronchen_err_intsignal : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_wronchen_err_intsignal:1;
        /** ch2_enable_slvif_shadowreg_wron_valid_err_intsignal : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_shadowreg_wron_valid_err_intsignal:1;
        /** ch2_enable_slvif_wronhold_err_intsignal : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_wronhold_err_intsignal:1;
        uint32_t reserved_22:3;
        /** ch2_enable_slvif_wrparity_err_intsignal : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_slvif_wrparity_err_intsignal:1;
        uint32_t reserved_26:1;
        /** ch2_enable_ch_lock_cleared_intsignal : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_lock_cleared_intsignal:1;
        /** ch2_enable_ch_src_suspended_intsignal : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_src_suspended_intsignal:1;
        /** ch2_enable_ch_suspended_intsignal : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_suspended_intsignal:1;
        /** ch2_enable_ch_disabled_intsignal : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_disabled_intsignal:1;
        /** ch2_enable_ch_aborted_intsignal : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ch_aborted_intsignal:1;
    };
    uint32_t val;
} dmac_ch2_intsignal_enable0_reg_t;

/** Type of ch2_intsignal_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_enable_ecc_prot_chmem_correrr_intsignal : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ecc_prot_chmem_correrr_intsignal:1;
        /** ch2_enable_ecc_prot_chmem_uncorrerr_intsignal : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ecc_prot_chmem_uncorrerr_intsignal:1;
        /** ch2_enable_ecc_prot_uidmem_correrr_intsignal : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ecc_prot_uidmem_correrr_intsignal:1;
        /** ch2_enable_ecc_prot_uidmem_uncorrerr_intsignal : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch2_enable_ecc_prot_uidmem_uncorrerr_intsignal:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch2_intsignal_enable1_reg_t;

/** Type of ch2_intclear0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_clear_block_tfr_done_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_block_tfr_done_intstat:1;
        /** ch2_clear_dma_tfr_done_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch2_clear_src_transcomp_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_src_transcomp_intstat:1;
        /** ch2_clear_dst_transcomp_intstat : WO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_dst_transcomp_intstat:1;
        /** ch2_clear_src_dec_err_intstat : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_src_dec_err_intstat:1;
        /** ch2_clear_dst_dec_err_intstat : WO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_dst_dec_err_intstat:1;
        /** ch2_clear_src_slv_err_intstat : WO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_src_slv_err_intstat:1;
        /** ch2_clear_dst_slv_err_intstat : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_dst_slv_err_intstat:1;
        /** ch2_clear_lli_rd_dec_err_intstat : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_lli_rd_dec_err_intstat:1;
        /** ch2_clear_lli_wr_dec_err_intstat : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_lli_wr_dec_err_intstat:1;
        /** ch2_clear_lli_rd_slv_err_intstat : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_lli_rd_slv_err_intstat:1;
        /** ch2_clear_lli_wr_slv_err_intstat : WO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_lli_wr_slv_err_intstat:1;
        /** ch2_clear_shadowreg_or_lli_invalid_err_intstat : WO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch2_clear_slvif_multiblktype_err_intstat : WO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch2_clear_slvif_dec_err_intstat : WO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_slvif_dec_err_intstat:1;
        /** ch2_clear_slvif_wr2ro_err_intstat : WO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_slvif_wr2ro_err_intstat:1;
        /** ch2_clear_slvif_rd2rwo_err_intstat : WO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_slvif_rd2rwo_err_intstat:1;
        /** ch2_clear_slvif_wronchen_err_intstat : WO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_slvif_wronchen_err_intstat:1;
        /** ch2_clear_slvif_shadowreg_wron_valid_err_intstat : WO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch2_clear_slvif_wronhold_err_intstat : WO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch2_clear_slvif_wrparity_err_intstat : WO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch2_clear_ch_lock_cleared_intstat : WO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_ch_lock_cleared_intstat:1;
        /** ch2_clear_ch_src_suspended_intstat : WO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_ch_src_suspended_intstat:1;
        /** ch2_clear_ch_suspended_intstat : WO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_ch_suspended_intstat:1;
        /** ch2_clear_ch_disabled_intstat : WO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_ch_disabled_intstat:1;
        /** ch2_clear_ch_aborted_intstat : WO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch2_intclear0_reg_t;

/** Type of ch2_intclear1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_clear_ecc_prot_chmem_correrr_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_ecc_prot_chmem_correrr_intstat:1;
        /** ch2_clear_ecc_prot_chmem_uncorrerr_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch2_clear_ecc_prot_uidmem_correrr_intstat : WO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_ecc_prot_uidmem_correrr_intstat:1;
        /** ch2_clear_ecc_prot_uidmem_uncorrerr_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch2_clear_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch2_intclear1_reg_t;

/** Type of ch3_intstatus_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_enable_block_tfr_done_intstat : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_block_tfr_done_intstat:1;
        /** ch3_enable_dma_tfr_done_intstat : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch3_enable_src_transcomp_intstat : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_src_transcomp_intstat:1;
        /** ch3_enable_dst_transcomp_intstat : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_dst_transcomp_intstat:1;
        /** ch3_enable_src_dec_err_intstat : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_src_dec_err_intstat:1;
        /** ch3_enable_dst_dec_err_intstat : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_dst_dec_err_intstat:1;
        /** ch3_enable_src_slv_err_intstat : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_src_slv_err_intstat:1;
        /** ch3_enable_dst_slv_err_intstat : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_dst_slv_err_intstat:1;
        /** ch3_enable_lli_rd_dec_err_intstat : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_lli_rd_dec_err_intstat:1;
        /** ch3_enable_lli_wr_dec_err_intstat : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_lli_wr_dec_err_intstat:1;
        /** ch3_enable_lli_rd_slv_err_intstat : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_lli_rd_slv_err_intstat:1;
        /** ch3_enable_lli_wr_slv_err_intstat : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_lli_wr_slv_err_intstat:1;
        /** ch3_enable_shadowreg_or_lli_invalid_err_intstat : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch3_enable_slvif_multiblktype_err_intstat : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch3_enable_slvif_dec_err_intstat : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_dec_err_intstat:1;
        /** ch3_enable_slvif_wr2ro_err_intstat : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_wr2ro_err_intstat:1;
        /** ch3_enable_slvif_rd2rwo_err_intstat : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_rd2rwo_err_intstat:1;
        /** ch3_enable_slvif_wronchen_err_intstat : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_wronchen_err_intstat:1;
        /** ch3_enable_slvif_shadowreg_wron_valid_err_intstat : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch3_enable_slvif_wronhold_err_intstat : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch3_enable_slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch3_enable_ch_lock_cleared_intstat : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_lock_cleared_intstat:1;
        /** ch3_enable_ch_src_suspended_intstat : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_src_suspended_intstat:1;
        /** ch3_enable_ch_suspended_intstat : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_suspended_intstat:1;
        /** ch3_enable_ch_disabled_intstat : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_disabled_intstat:1;
        /** ch3_enable_ch_aborted_intstat : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch3_intstatus_enable0_reg_t;

/** Type of ch3_intstatus_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_enable_ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ecc_prot_chmem_correrr_intstat:1;
        /** ch3_enable_ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch3_enable_ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ecc_prot_uidmem_correrr_intstat:1;
        /** ch3_enable_ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch3_intstatus_enable1_reg_t;

/** Type of ch3_intstatus0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_block_tfr_done_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_block_tfr_done_intstat:1;
        /** ch3_dma_tfr_done_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch3_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch3_src_transcomp_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch3_src_transcomp_intstat:1;
        /** ch3_dst_transcomp_intstat : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch3_dst_transcomp_intstat:1;
        /** ch3_src_dec_err_intstat : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch3_src_dec_err_intstat:1;
        /** ch3_dst_dec_err_intstat : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch3_dst_dec_err_intstat:1;
        /** ch3_src_slv_err_intstat : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t ch3_src_slv_err_intstat:1;
        /** ch3_dst_slv_err_intstat : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch3_dst_slv_err_intstat:1;
        /** ch3_lli_rd_dec_err_intstat : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch3_lli_rd_dec_err_intstat:1;
        /** ch3_lli_wr_dec_err_intstat : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch3_lli_wr_dec_err_intstat:1;
        /** ch3_lli_rd_slv_err_intstat : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch3_lli_rd_slv_err_intstat:1;
        /** ch3_lli_wr_slv_err_intstat : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ch3_lli_wr_slv_err_intstat:1;
        /** ch3_shadowreg_or_lli_invalid_err_intstat : RO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ch3_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch3_slvif_multiblktype_err_intstat : RO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ch3_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch3_slvif_dec_err_intstat : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch3_slvif_dec_err_intstat:1;
        /** ch3_slvif_wr2ro_err_intstat : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch3_slvif_wr2ro_err_intstat:1;
        /** ch3_slvif_rd2rwo_err_intstat : RO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch3_slvif_rd2rwo_err_intstat:1;
        /** ch3_slvif_wronchen_err_intstat : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch3_slvif_wronchen_err_intstat:1;
        /** ch3_slvif_shadowreg_wron_valid_err_intstat : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch3_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch3_slvif_wronhold_err_intstat : RO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t ch3_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch3_slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch3_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch3_ch_lock_cleared_intstat : RO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch3_ch_lock_cleared_intstat:1;
        /** ch3_ch_src_suspended_intstat : RO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t ch3_ch_src_suspended_intstat:1;
        /** ch3_ch_suspended_intstat : RO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t ch3_ch_suspended_intstat:1;
        /** ch3_ch_disabled_intstat : RO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch3_ch_disabled_intstat:1;
        /** ch3_ch_aborted_intstat : RO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch3_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch3_intstatus0_reg_t;

/** Type of ch3_intstatus1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_ecc_prot_chmem_correrr_intstat:1;
        /** ch3_ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch3_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch3_ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_ecc_prot_uidmem_correrr_intstat:1;
        /** ch3_ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch3_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch3_intstatus1_reg_t;

/** Type of ch3_intsignal_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_enable_block_tfr_done_intsignal : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_block_tfr_done_intsignal:1;
        /** ch3_enable_dma_tfr_done_intsignal : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_dma_tfr_done_intsignal:1;
        uint32_t reserved_2:1;
        /** ch3_enable_src_transcomp_intsignal : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_src_transcomp_intsignal:1;
        /** ch3_enable_dst_transcomp_intsignal : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_dst_transcomp_intsignal:1;
        /** ch3_enable_src_dec_err_intsignal : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_src_dec_err_intsignal:1;
        /** ch3_enable_dst_dec_err_intsignal : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_dst_dec_err_intsignal:1;
        /** ch3_enable_src_slv_err_intsignal : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_src_slv_err_intsignal:1;
        /** ch3_enable_dst_slv_err_intsignal : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_dst_slv_err_intsignal:1;
        /** ch3_enable_lli_rd_dec_err_intsignal : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_lli_rd_dec_err_intsignal:1;
        /** ch3_enable_lli_wr_dec_err_intsignal : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_lli_wr_dec_err_intsignal:1;
        /** ch3_enable_lli_rd_slv_err_intsignal : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_lli_rd_slv_err_intsignal:1;
        /** ch3_enable_lli_wr_slv_err_intsignal : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_lli_wr_slv_err_intsignal:1;
        /** ch3_enable_shadowreg_or_lli_invalid_err_intsignal : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_shadowreg_or_lli_invalid_err_intsignal:1;
        /** ch3_enable_slvif_multiblktype_err_intsignal : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_multiblktype_err_intsignal:1;
        uint32_t reserved_15:1;
        /** ch3_enable_slvif_dec_err_intsignal : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_dec_err_intsignal:1;
        /** ch3_enable_slvif_wr2ro_err_intsignal : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_wr2ro_err_intsignal:1;
        /** ch3_enable_slvif_rd2rwo_err_intsignal : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_rd2rwo_err_intsignal:1;
        /** ch3_enable_slvif_wronchen_err_intsignal : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_wronchen_err_intsignal:1;
        /** ch3_enable_slvif_shadowreg_wron_valid_err_intsignal : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_shadowreg_wron_valid_err_intsignal:1;
        /** ch3_enable_slvif_wronhold_err_intsignal : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_wronhold_err_intsignal:1;
        uint32_t reserved_22:3;
        /** ch3_enable_slvif_wrparity_err_intsignal : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_slvif_wrparity_err_intsignal:1;
        uint32_t reserved_26:1;
        /** ch3_enable_ch_lock_cleared_intsignal : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_lock_cleared_intsignal:1;
        /** ch3_enable_ch_src_suspended_intsignal : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_src_suspended_intsignal:1;
        /** ch3_enable_ch_suspended_intsignal : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_suspended_intsignal:1;
        /** ch3_enable_ch_disabled_intsignal : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_disabled_intsignal:1;
        /** ch3_enable_ch_aborted_intsignal : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ch_aborted_intsignal:1;
    };
    uint32_t val;
} dmac_ch3_intsignal_enable0_reg_t;

/** Type of ch3_intsignal_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_enable_ecc_prot_chmem_correrr_intsignal : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ecc_prot_chmem_correrr_intsignal:1;
        /** ch3_enable_ecc_prot_chmem_uncorrerr_intsignal : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ecc_prot_chmem_uncorrerr_intsignal:1;
        /** ch3_enable_ecc_prot_uidmem_correrr_intsignal : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ecc_prot_uidmem_correrr_intsignal:1;
        /** ch3_enable_ecc_prot_uidmem_uncorrerr_intsignal : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch3_enable_ecc_prot_uidmem_uncorrerr_intsignal:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch3_intsignal_enable1_reg_t;

/** Type of ch3_intclear0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_clear_block_tfr_done_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_block_tfr_done_intstat:1;
        /** ch3_clear_dma_tfr_done_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch3_clear_src_transcomp_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_src_transcomp_intstat:1;
        /** ch3_clear_dst_transcomp_intstat : WO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_dst_transcomp_intstat:1;
        /** ch3_clear_src_dec_err_intstat : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_src_dec_err_intstat:1;
        /** ch3_clear_dst_dec_err_intstat : WO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_dst_dec_err_intstat:1;
        /** ch3_clear_src_slv_err_intstat : WO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_src_slv_err_intstat:1;
        /** ch3_clear_dst_slv_err_intstat : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_dst_slv_err_intstat:1;
        /** ch3_clear_lli_rd_dec_err_intstat : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_lli_rd_dec_err_intstat:1;
        /** ch3_clear_lli_wr_dec_err_intstat : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_lli_wr_dec_err_intstat:1;
        /** ch3_clear_lli_rd_slv_err_intstat : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_lli_rd_slv_err_intstat:1;
        /** ch3_clear_lli_wr_slv_err_intstat : WO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_lli_wr_slv_err_intstat:1;
        /** ch3_clear_shadowreg_or_lli_invalid_err_intstat : WO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch3_clear_slvif_multiblktype_err_intstat : WO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch3_clear_slvif_dec_err_intstat : WO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_slvif_dec_err_intstat:1;
        /** ch3_clear_slvif_wr2ro_err_intstat : WO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_slvif_wr2ro_err_intstat:1;
        /** ch3_clear_slvif_rd2rwo_err_intstat : WO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_slvif_rd2rwo_err_intstat:1;
        /** ch3_clear_slvif_wronchen_err_intstat : WO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_slvif_wronchen_err_intstat:1;
        /** ch3_clear_slvif_shadowreg_wron_valid_err_intstat : WO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch3_clear_slvif_wronhold_err_intstat : WO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch3_clear_slvif_wrparity_err_intstat : WO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch3_clear_ch_lock_cleared_intstat : WO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_ch_lock_cleared_intstat:1;
        /** ch3_clear_ch_src_suspended_intstat : WO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_ch_src_suspended_intstat:1;
        /** ch3_clear_ch_suspended_intstat : WO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_ch_suspended_intstat:1;
        /** ch3_clear_ch_disabled_intstat : WO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_ch_disabled_intstat:1;
        /** ch3_clear_ch_aborted_intstat : WO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch3_intclear0_reg_t;

/** Type of ch3_intclear1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_clear_ecc_prot_chmem_correrr_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_ecc_prot_chmem_correrr_intstat:1;
        /** ch3_clear_ecc_prot_chmem_uncorrerr_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch3_clear_ecc_prot_uidmem_correrr_intstat : WO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_ecc_prot_uidmem_correrr_intstat:1;
        /** ch3_clear_ecc_prot_uidmem_uncorrerr_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch3_clear_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch3_intclear1_reg_t;

/** Type of ch4_intstatus_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_enable_block_tfr_done_intstat : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_block_tfr_done_intstat:1;
        /** ch4_enable_dma_tfr_done_intstat : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch4_enable_src_transcomp_intstat : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_src_transcomp_intstat:1;
        /** ch4_enable_dst_transcomp_intstat : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_dst_transcomp_intstat:1;
        /** ch4_enable_src_dec_err_intstat : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_src_dec_err_intstat:1;
        /** ch4_enable_dst_dec_err_intstat : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_dst_dec_err_intstat:1;
        /** ch4_enable_src_slv_err_intstat : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_src_slv_err_intstat:1;
        /** ch4_enable_dst_slv_err_intstat : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_dst_slv_err_intstat:1;
        /** ch4_enable_lli_rd_dec_err_intstat : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_lli_rd_dec_err_intstat:1;
        /** ch4_enable_lli_wr_dec_err_intstat : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_lli_wr_dec_err_intstat:1;
        /** ch4_enable_lli_rd_slv_err_intstat : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_lli_rd_slv_err_intstat:1;
        /** ch4_enable_lli_wr_slv_err_intstat : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_lli_wr_slv_err_intstat:1;
        /** ch4_enable_shadowreg_or_lli_invalid_err_intstat : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch4_enable_slvif_multiblktype_err_intstat : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch4_enable_slvif_dec_err_intstat : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_dec_err_intstat:1;
        /** ch4_enable_slvif_wr2ro_err_intstat : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_wr2ro_err_intstat:1;
        /** ch4_enable_slvif_rd2rwo_err_intstat : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_rd2rwo_err_intstat:1;
        /** ch4_enable_slvif_wronchen_err_intstat : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_wronchen_err_intstat:1;
        /** ch4_enable_slvif_shadowreg_wron_valid_err_intstat : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch4_enable_slvif_wronhold_err_intstat : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch4_enable_slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch4_enable_ch_lock_cleared_intstat : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_lock_cleared_intstat:1;
        /** ch4_enable_ch_src_suspended_intstat : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_src_suspended_intstat:1;
        /** ch4_enable_ch_suspended_intstat : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_suspended_intstat:1;
        /** ch4_enable_ch_disabled_intstat : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_disabled_intstat:1;
        /** ch4_enable_ch_aborted_intstat : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch4_intstatus_enable0_reg_t;

/** Type of ch4_intstatus_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_enable_ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ecc_prot_chmem_correrr_intstat:1;
        /** ch4_enable_ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch4_enable_ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ecc_prot_uidmem_correrr_intstat:1;
        /** ch4_enable_ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch4_intstatus_enable1_reg_t;

/** Type of ch4_intstatus0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_block_tfr_done_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_block_tfr_done_intstat:1;
        /** ch4_dma_tfr_done_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch4_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch4_src_transcomp_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_src_transcomp_intstat:1;
        /** ch4_dst_transcomp_intstat : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch4_dst_transcomp_intstat:1;
        /** ch4_src_dec_err_intstat : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch4_src_dec_err_intstat:1;
        /** ch4_dst_dec_err_intstat : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch4_dst_dec_err_intstat:1;
        /** ch4_src_slv_err_intstat : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t ch4_src_slv_err_intstat:1;
        /** ch4_dst_slv_err_intstat : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch4_dst_slv_err_intstat:1;
        /** ch4_lli_rd_dec_err_intstat : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch4_lli_rd_dec_err_intstat:1;
        /** ch4_lli_wr_dec_err_intstat : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch4_lli_wr_dec_err_intstat:1;
        /** ch4_lli_rd_slv_err_intstat : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch4_lli_rd_slv_err_intstat:1;
        /** ch4_lli_wr_slv_err_intstat : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ch4_lli_wr_slv_err_intstat:1;
        /** ch4_shadowreg_or_lli_invalid_err_intstat : RO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ch4_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch4_slvif_multiblktype_err_intstat : RO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ch4_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch4_slvif_dec_err_intstat : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch4_slvif_dec_err_intstat:1;
        /** ch4_slvif_wr2ro_err_intstat : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch4_slvif_wr2ro_err_intstat:1;
        /** ch4_slvif_rd2rwo_err_intstat : RO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch4_slvif_rd2rwo_err_intstat:1;
        /** ch4_slvif_wronchen_err_intstat : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch4_slvif_wronchen_err_intstat:1;
        /** ch4_slvif_shadowreg_wron_valid_err_intstat : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch4_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch4_slvif_wronhold_err_intstat : RO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t ch4_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch4_slvif_wrparity_err_intstat : RO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch4_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch4_ch_lock_cleared_intstat : RO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch4_ch_lock_cleared_intstat:1;
        /** ch4_ch_src_suspended_intstat : RO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t ch4_ch_src_suspended_intstat:1;
        /** ch4_ch_suspended_intstat : RO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t ch4_ch_suspended_intstat:1;
        /** ch4_ch_disabled_intstat : RO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch4_ch_disabled_intstat:1;
        /** ch4_ch_aborted_intstat : RO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch4_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch4_intstatus0_reg_t;

/** Type of ch4_intstatus1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_ecc_prot_chmem_correrr_intstat : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_ecc_prot_chmem_correrr_intstat:1;
        /** ch4_ecc_prot_chmem_uncorrerr_intstat : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch4_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch4_ecc_prot_uidmem_correrr_intstat : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch4_ecc_prot_uidmem_correrr_intstat:1;
        /** ch4_ecc_prot_uidmem_uncorrerr_intstat : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch4_intstatus1_reg_t;

/** Type of ch4_intsignal_enable0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_enable_block_tfr_done_intsignal : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_block_tfr_done_intsignal:1;
        /** ch4_enable_dma_tfr_done_intsignal : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_dma_tfr_done_intsignal:1;
        uint32_t reserved_2:1;
        /** ch4_enable_src_transcomp_intsignal : R/W; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_src_transcomp_intsignal:1;
        /** ch4_enable_dst_transcomp_intsignal : R/W; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_dst_transcomp_intsignal:1;
        /** ch4_enable_src_dec_err_intsignal : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_src_dec_err_intsignal:1;
        /** ch4_enable_dst_dec_err_intsignal : R/W; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_dst_dec_err_intsignal:1;
        /** ch4_enable_src_slv_err_intsignal : R/W; bitpos: [7]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_src_slv_err_intsignal:1;
        /** ch4_enable_dst_slv_err_intsignal : R/W; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_dst_slv_err_intsignal:1;
        /** ch4_enable_lli_rd_dec_err_intsignal : R/W; bitpos: [9]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_lli_rd_dec_err_intsignal:1;
        /** ch4_enable_lli_wr_dec_err_intsignal : R/W; bitpos: [10]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_lli_wr_dec_err_intsignal:1;
        /** ch4_enable_lli_rd_slv_err_intsignal : R/W; bitpos: [11]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_lli_rd_slv_err_intsignal:1;
        /** ch4_enable_lli_wr_slv_err_intsignal : R/W; bitpos: [12]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_lli_wr_slv_err_intsignal:1;
        /** ch4_enable_shadowreg_or_lli_invalid_err_intsignal : R/W; bitpos: [13]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_shadowreg_or_lli_invalid_err_intsignal:1;
        /** ch4_enable_slvif_multiblktype_err_intsignal : R/W; bitpos: [14]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_multiblktype_err_intsignal:1;
        uint32_t reserved_15:1;
        /** ch4_enable_slvif_dec_err_intsignal : R/W; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_dec_err_intsignal:1;
        /** ch4_enable_slvif_wr2ro_err_intsignal : R/W; bitpos: [17]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_wr2ro_err_intsignal:1;
        /** ch4_enable_slvif_rd2rwo_err_intsignal : R/W; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_rd2rwo_err_intsignal:1;
        /** ch4_enable_slvif_wronchen_err_intsignal : R/W; bitpos: [19]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_wronchen_err_intsignal:1;
        /** ch4_enable_slvif_shadowreg_wron_valid_err_intsignal : R/W; bitpos: [20]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_shadowreg_wron_valid_err_intsignal:1;
        /** ch4_enable_slvif_wronhold_err_intsignal : R/W; bitpos: [21]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_wronhold_err_intsignal:1;
        uint32_t reserved_22:3;
        /** ch4_enable_slvif_wrparity_err_intsignal : RO; bitpos: [25]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_slvif_wrparity_err_intsignal:1;
        uint32_t reserved_26:1;
        /** ch4_enable_ch_lock_cleared_intsignal : R/W; bitpos: [27]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_lock_cleared_intsignal:1;
        /** ch4_enable_ch_src_suspended_intsignal : R/W; bitpos: [28]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_src_suspended_intsignal:1;
        /** ch4_enable_ch_suspended_intsignal : R/W; bitpos: [29]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_suspended_intsignal:1;
        /** ch4_enable_ch_disabled_intsignal : R/W; bitpos: [30]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_disabled_intsignal:1;
        /** ch4_enable_ch_aborted_intsignal : R/W; bitpos: [31]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ch_aborted_intsignal:1;
    };
    uint32_t val;
} dmac_ch4_intsignal_enable0_reg_t;

/** Type of ch4_intsignal_enable1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_enable_ecc_prot_chmem_correrr_intsignal : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ecc_prot_chmem_correrr_intsignal:1;
        /** ch4_enable_ecc_prot_chmem_uncorrerr_intsignal : RO; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ecc_prot_chmem_uncorrerr_intsignal:1;
        /** ch4_enable_ecc_prot_uidmem_correrr_intsignal : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ecc_prot_uidmem_correrr_intsignal:1;
        /** ch4_enable_ecc_prot_uidmem_uncorrerr_intsignal : RO; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t ch4_enable_ecc_prot_uidmem_uncorrerr_intsignal:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch4_intsignal_enable1_reg_t;

/** Type of ch4_intclear0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_clear_block_tfr_done_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_block_tfr_done_intstat:1;
        /** ch4_clear_dma_tfr_done_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_dma_tfr_done_intstat:1;
        uint32_t reserved_2:1;
        /** ch4_clear_src_transcomp_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_src_transcomp_intstat:1;
        /** ch4_clear_dst_transcomp_intstat : WO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_dst_transcomp_intstat:1;
        /** ch4_clear_src_dec_err_intstat : WO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_src_dec_err_intstat:1;
        /** ch4_clear_dst_dec_err_intstat : WO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_dst_dec_err_intstat:1;
        /** ch4_clear_src_slv_err_intstat : WO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_src_slv_err_intstat:1;
        /** ch4_clear_dst_slv_err_intstat : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_dst_slv_err_intstat:1;
        /** ch4_clear_lli_rd_dec_err_intstat : WO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_lli_rd_dec_err_intstat:1;
        /** ch4_clear_lli_wr_dec_err_intstat : WO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_lli_wr_dec_err_intstat:1;
        /** ch4_clear_lli_rd_slv_err_intstat : WO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_lli_rd_slv_err_intstat:1;
        /** ch4_clear_lli_wr_slv_err_intstat : WO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_lli_wr_slv_err_intstat:1;
        /** ch4_clear_shadowreg_or_lli_invalid_err_intstat : WO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_shadowreg_or_lli_invalid_err_intstat:1;
        /** ch4_clear_slvif_multiblktype_err_intstat : WO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_slvif_multiblktype_err_intstat:1;
        uint32_t reserved_15:1;
        /** ch4_clear_slvif_dec_err_intstat : WO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_slvif_dec_err_intstat:1;
        /** ch4_clear_slvif_wr2ro_err_intstat : WO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_slvif_wr2ro_err_intstat:1;
        /** ch4_clear_slvif_rd2rwo_err_intstat : WO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_slvif_rd2rwo_err_intstat:1;
        /** ch4_clear_slvif_wronchen_err_intstat : WO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_slvif_wronchen_err_intstat:1;
        /** ch4_clear_slvif_shadowreg_wron_valid_err_intstat : WO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_slvif_shadowreg_wron_valid_err_intstat:1;
        /** ch4_clear_slvif_wronhold_err_intstat : WO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_slvif_wronhold_err_intstat:1;
        uint32_t reserved_22:3;
        /** ch4_clear_slvif_wrparity_err_intstat : WO; bitpos: [25]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_slvif_wrparity_err_intstat:1;
        uint32_t reserved_26:1;
        /** ch4_clear_ch_lock_cleared_intstat : WO; bitpos: [27]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_ch_lock_cleared_intstat:1;
        /** ch4_clear_ch_src_suspended_intstat : WO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_ch_src_suspended_intstat:1;
        /** ch4_clear_ch_suspended_intstat : WO; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_ch_suspended_intstat:1;
        /** ch4_clear_ch_disabled_intstat : WO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_ch_disabled_intstat:1;
        /** ch4_clear_ch_aborted_intstat : WO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_ch_aborted_intstat:1;
    };
    uint32_t val;
} dmac_ch4_intclear0_reg_t;

/** Type of ch4_intclear1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_clear_ecc_prot_chmem_correrr_intstat : WO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_ecc_prot_chmem_correrr_intstat:1;
        /** ch4_clear_ecc_prot_chmem_uncorrerr_intstat : WO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_ecc_prot_chmem_uncorrerr_intstat:1;
        /** ch4_clear_ecc_prot_uidmem_correrr_intstat : WO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_ecc_prot_uidmem_correrr_intstat:1;
        /** ch4_clear_ecc_prot_uidmem_uncorrerr_intstat : WO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ch4_clear_ecc_prot_uidmem_uncorrerr_intstat:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dmac_ch4_intclear1_reg_t;


/** Group: Status Registers */
/** Type of ch1_status0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_cmpltd_blk_tfr_size : RO; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t ch1_cmpltd_blk_tfr_size:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} dmac_ch1_status0_reg_t;

/** Type of ch1_status1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_data_left_in_fifo : RO; bitpos: [14:0]; default: 0;
         *  NA
         */
        uint32_t ch1_data_left_in_fifo:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} dmac_ch1_status1_reg_t;

/** Type of ch1_sstat0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_sstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_sstat:32;
    };
    uint32_t val;
} dmac_ch1_sstat0_reg_t;

/** Type of ch1_dstat0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_dstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_dstat:32;
    };
    uint32_t val;
} dmac_ch1_dstat0_reg_t;

/** Type of ch1_sstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_sstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_sstatar0:32;
    };
    uint32_t val;
} dmac_ch1_sstatar0_reg_t;

/** Type of ch1_sstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_sstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_sstatar1:32;
    };
    uint32_t val;
} dmac_ch1_sstatar1_reg_t;

/** Type of ch1_dstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_dstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_dstatar0:32;
    };
    uint32_t val;
} dmac_ch1_dstatar0_reg_t;

/** Type of ch1_dstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch1_dstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch1_dstatar1:32;
    };
    uint32_t val;
} dmac_ch1_dstatar1_reg_t;

/** Type of ch2_status0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_cmpltd_blk_tfr_size : RO; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t ch2_cmpltd_blk_tfr_size:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} dmac_ch2_status0_reg_t;

/** Type of ch2_status1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_data_left_in_fifo : RO; bitpos: [14:0]; default: 0;
         *  NA
         */
        uint32_t ch2_data_left_in_fifo:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} dmac_ch2_status1_reg_t;

/** Type of ch2_sstat0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_sstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_sstat:32;
    };
    uint32_t val;
} dmac_ch2_sstat0_reg_t;

/** Type of ch2_dstat0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_dstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_dstat:32;
    };
    uint32_t val;
} dmac_ch2_dstat0_reg_t;

/** Type of ch2_sstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_sstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_sstatar0:32;
    };
    uint32_t val;
} dmac_ch2_sstatar0_reg_t;

/** Type of ch2_sstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_sstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_sstatar1:32;
    };
    uint32_t val;
} dmac_ch2_sstatar1_reg_t;

/** Type of ch2_dstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_dstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_dstatar0:32;
    };
    uint32_t val;
} dmac_ch2_dstatar0_reg_t;

/** Type of ch2_dstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch2_dstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch2_dstatar1:32;
    };
    uint32_t val;
} dmac_ch2_dstatar1_reg_t;

/** Type of ch3_status0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_cmpltd_blk_tfr_size : RO; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t ch3_cmpltd_blk_tfr_size:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} dmac_ch3_status0_reg_t;

/** Type of ch3_status1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_data_left_in_fifo : RO; bitpos: [14:0]; default: 0;
         *  NA
         */
        uint32_t ch3_data_left_in_fifo:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} dmac_ch3_status1_reg_t;

/** Type of ch3_sstat0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_sstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_sstat:32;
    };
    uint32_t val;
} dmac_ch3_sstat0_reg_t;

/** Type of ch3_dstat0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_dstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_dstat:32;
    };
    uint32_t val;
} dmac_ch3_dstat0_reg_t;

/** Type of ch3_sstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_sstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_sstatar0:32;
    };
    uint32_t val;
} dmac_ch3_sstatar0_reg_t;

/** Type of ch3_sstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_sstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_sstatar1:32;
    };
    uint32_t val;
} dmac_ch3_sstatar1_reg_t;

/** Type of ch3_dstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_dstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_dstatar0:32;
    };
    uint32_t val;
} dmac_ch3_dstatar0_reg_t;

/** Type of ch3_dstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch3_dstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch3_dstatar1:32;
    };
    uint32_t val;
} dmac_ch3_dstatar1_reg_t;

/** Type of ch4_status0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_cmpltd_blk_tfr_size : RO; bitpos: [21:0]; default: 0;
         *  NA
         */
        uint32_t ch4_cmpltd_blk_tfr_size:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} dmac_ch4_status0_reg_t;

/** Type of ch4_status1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_data_left_in_fifo : RO; bitpos: [14:0]; default: 0;
         *  NA
         */
        uint32_t ch4_data_left_in_fifo:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} dmac_ch4_status1_reg_t;

/** Type of ch4_sstat0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_sstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_sstat:32;
    };
    uint32_t val;
} dmac_ch4_sstat0_reg_t;

/** Type of ch4_dstat0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_dstat : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_dstat:32;
    };
    uint32_t val;
} dmac_ch4_dstat0_reg_t;

/** Type of ch4_sstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_sstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_sstatar0:32;
    };
    uint32_t val;
} dmac_ch4_sstatar0_reg_t;

/** Type of ch4_sstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_sstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_sstatar1:32;
    };
    uint32_t val;
} dmac_ch4_sstatar1_reg_t;

/** Type of ch4_dstatar0 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_dstatar0 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_dstatar0:32;
    };
    uint32_t val;
} dmac_ch4_dstatar0_reg_t;

/** Type of ch4_dstatar1 register
 *  NA
 */
typedef union {
    struct {
        /** ch4_dstatar1 : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ch4_dstatar1:32;
    };
    uint32_t val;
} dmac_ch4_dstatar1_reg_t;


typedef struct {
    volatile dmac_id0_reg_t id0;
    uint32_t reserved_004;
    volatile dmac_compver0_reg_t compver0;
    uint32_t reserved_00c;
    volatile dmac_cfg0_reg_t cfg0;
    uint32_t reserved_014;
    volatile dmac_chen0_reg_t chen0;
    volatile dmac_chen1_reg_t chen1;
    uint32_t reserved_020[4];
    volatile dmac_intstatus0_reg_t intstatus0;
    uint32_t reserved_034;
    volatile dmac_commonreg_intclear0_reg_t commonreg_intclear0;
    uint32_t reserved_03c;
    volatile dmac_commonreg_intstatus_enable0_reg_t commonreg_intstatus_enable0;
    uint32_t reserved_044;
    volatile dmac_commonreg_intsignal_enable0_reg_t commonreg_intsignal_enable0;
    uint32_t reserved_04c;
    volatile dmac_commonreg_intstatus0_reg_t commonreg_intstatus0;
    uint32_t reserved_054;
    volatile dmac_reset0_reg_t reset0;
    uint32_t reserved_05c;
    volatile dmac_lowpower_cfg0_reg_t lowpower_cfg0;
    volatile dmac_lowpower_cfg1_reg_t lowpower_cfg1;
    uint32_t reserved_068[38];
    volatile dmac_ch1_sar0_reg_t ch1_sar0;
    volatile dmac_ch1_sar1_reg_t ch1_sar1;
    volatile dmac_ch1_dar0_reg_t ch1_dar0;
    volatile dmac_ch1_dar1_reg_t ch1_dar1;
    volatile dmac_ch1_block_ts0_reg_t ch1_block_ts0;
    uint32_t reserved_114;
    volatile dmac_ch1_ctl0_reg_t ch1_ctl0;
    volatile dmac_ch1_ctl1_reg_t ch1_ctl1;
    volatile dmac_ch1_cfg0_reg_t ch1_cfg0;
    volatile dmac_ch1_cfg1_reg_t ch1_cfg1;
    volatile dmac_ch1_llp0_reg_t ch1_llp0;
    volatile dmac_ch1_llp1_reg_t ch1_llp1;
    volatile dmac_ch1_status0_reg_t ch1_status0;
    volatile dmac_ch1_status1_reg_t ch1_status1;
    volatile dmac_ch1_swhssrc0_reg_t ch1_swhssrc0;
    uint32_t reserved_13c;
    volatile dmac_ch1_swhsdst0_reg_t ch1_swhsdst0;
    uint32_t reserved_144;
    volatile dmac_ch1_blk_tfr_resumereq0_reg_t ch1_blk_tfr_resumereq0;
    uint32_t reserved_14c;
    volatile dmac_ch1_axi_id0_reg_t ch1_axi_id0;
    uint32_t reserved_154;
    volatile dmac_ch1_axi_qos0_reg_t ch1_axi_qos0;
    uint32_t reserved_15c;
    volatile dmac_ch1_sstat0_reg_t ch1_sstat0;
    uint32_t reserved_164;
    volatile dmac_ch1_dstat0_reg_t ch1_dstat0;
    uint32_t reserved_16c;
    volatile dmac_ch1_sstatar0_reg_t ch1_sstatar0;
    volatile dmac_ch1_sstatar1_reg_t ch1_sstatar1;
    volatile dmac_ch1_dstatar0_reg_t ch1_dstatar0;
    volatile dmac_ch1_dstatar1_reg_t ch1_dstatar1;
    volatile dmac_ch1_intstatus_enable0_reg_t ch1_intstatus_enable0;
    volatile dmac_ch1_intstatus_enable1_reg_t ch1_intstatus_enable1;
    volatile dmac_ch1_intstatus0_reg_t ch1_intstatus0;
    volatile dmac_ch1_intstatus1_reg_t ch1_intstatus1;
    volatile dmac_ch1_intsignal_enable0_reg_t ch1_intsignal_enable0;
    volatile dmac_ch1_intsignal_enable1_reg_t ch1_intsignal_enable1;
    volatile dmac_ch1_intclear0_reg_t ch1_intclear0;
    volatile dmac_ch1_intclear1_reg_t ch1_intclear1;
    uint32_t reserved_1a0[24];
    volatile dmac_ch2_sar0_reg_t ch2_sar0;
    volatile dmac_ch2_sar1_reg_t ch2_sar1;
    volatile dmac_ch2_dar0_reg_t ch2_dar0;
    volatile dmac_ch2_dar1_reg_t ch2_dar1;
    volatile dmac_ch2_block_ts0_reg_t ch2_block_ts0;
    uint32_t reserved_214;
    volatile dmac_ch2_ctl0_reg_t ch2_ctl0;
    volatile dmac_ch2_ctl1_reg_t ch2_ctl1;
    volatile dmac_ch2_cfg0_reg_t ch2_cfg0;
    volatile dmac_ch2_cfg1_reg_t ch2_cfg1;
    volatile dmac_ch2_llp0_reg_t ch2_llp0;
    volatile dmac_ch2_llp1_reg_t ch2_llp1;
    volatile dmac_ch2_status0_reg_t ch2_status0;
    volatile dmac_ch2_status1_reg_t ch2_status1;
    volatile dmac_ch2_swhssrc0_reg_t ch2_swhssrc0;
    uint32_t reserved_23c;
    volatile dmac_ch2_swhsdst0_reg_t ch2_swhsdst0;
    uint32_t reserved_244;
    volatile dmac_ch2_blk_tfr_resumereq0_reg_t ch2_blk_tfr_resumereq0;
    uint32_t reserved_24c;
    volatile dmac_ch2_axi_id0_reg_t ch2_axi_id0;
    uint32_t reserved_254;
    volatile dmac_ch2_axi_qos0_reg_t ch2_axi_qos0;
    uint32_t reserved_25c;
    volatile dmac_ch2_sstat0_reg_t ch2_sstat0;
    uint32_t reserved_264;
    volatile dmac_ch2_dstat0_reg_t ch2_dstat0;
    uint32_t reserved_26c;
    volatile dmac_ch2_sstatar0_reg_t ch2_sstatar0;
    volatile dmac_ch2_sstatar1_reg_t ch2_sstatar1;
    volatile dmac_ch2_dstatar0_reg_t ch2_dstatar0;
    volatile dmac_ch2_dstatar1_reg_t ch2_dstatar1;
    volatile dmac_ch2_intstatus_enable0_reg_t ch2_intstatus_enable0;
    volatile dmac_ch2_intstatus_enable1_reg_t ch2_intstatus_enable1;
    volatile dmac_ch2_intstatus0_reg_t ch2_intstatus0;
    volatile dmac_ch2_intstatus1_reg_t ch2_intstatus1;
    volatile dmac_ch2_intsignal_enable0_reg_t ch2_intsignal_enable0;
    volatile dmac_ch2_intsignal_enable1_reg_t ch2_intsignal_enable1;
    volatile dmac_ch2_intclear0_reg_t ch2_intclear0;
    volatile dmac_ch2_intclear1_reg_t ch2_intclear1;
    uint32_t reserved_2a0[24];
    volatile dmac_ch3_sar0_reg_t ch3_sar0;
    volatile dmac_ch3_sar1_reg_t ch3_sar1;
    volatile dmac_ch3_dar0_reg_t ch3_dar0;
    volatile dmac_ch3_dar1_reg_t ch3_dar1;
    volatile dmac_ch3_block_ts0_reg_t ch3_block_ts0;
    uint32_t reserved_314;
    volatile dmac_ch3_ctl0_reg_t ch3_ctl0;
    volatile dmac_ch3_ctl1_reg_t ch3_ctl1;
    volatile dmac_ch3_cfg0_reg_t ch3_cfg0;
    volatile dmac_ch3_cfg1_reg_t ch3_cfg1;
    volatile dmac_ch3_llp0_reg_t ch3_llp0;
    volatile dmac_ch3_llp1_reg_t ch3_llp1;
    volatile dmac_ch3_status0_reg_t ch3_status0;
    volatile dmac_ch3_status1_reg_t ch3_status1;
    volatile dmac_ch3_swhssrc0_reg_t ch3_swhssrc0;
    uint32_t reserved_33c;
    volatile dmac_ch3_swhsdst0_reg_t ch3_swhsdst0;
    uint32_t reserved_344;
    volatile dmac_ch3_blk_tfr_resumereq0_reg_t ch3_blk_tfr_resumereq0;
    uint32_t reserved_34c;
    volatile dmac_ch3_axi_id0_reg_t ch3_axi_id0;
    uint32_t reserved_354;
    volatile dmac_ch3_axi_qos0_reg_t ch3_axi_qos0;
    uint32_t reserved_35c;
    volatile dmac_ch3_sstat0_reg_t ch3_sstat0;
    uint32_t reserved_364;
    volatile dmac_ch3_dstat0_reg_t ch3_dstat0;
    uint32_t reserved_36c;
    volatile dmac_ch3_sstatar0_reg_t ch3_sstatar0;
    volatile dmac_ch3_sstatar1_reg_t ch3_sstatar1;
    volatile dmac_ch3_dstatar0_reg_t ch3_dstatar0;
    volatile dmac_ch3_dstatar1_reg_t ch3_dstatar1;
    volatile dmac_ch3_intstatus_enable0_reg_t ch3_intstatus_enable0;
    volatile dmac_ch3_intstatus_enable1_reg_t ch3_intstatus_enable1;
    volatile dmac_ch3_intstatus0_reg_t ch3_intstatus0;
    volatile dmac_ch3_intstatus1_reg_t ch3_intstatus1;
    volatile dmac_ch3_intsignal_enable0_reg_t ch3_intsignal_enable0;
    volatile dmac_ch3_intsignal_enable1_reg_t ch3_intsignal_enable1;
    volatile dmac_ch3_intclear0_reg_t ch3_intclear0;
    volatile dmac_ch3_intclear1_reg_t ch3_intclear1;
    uint32_t reserved_3a0[24];
    volatile dmac_ch4_sar0_reg_t ch4_sar0;
    volatile dmac_ch4_sar1_reg_t ch4_sar1;
    volatile dmac_ch4_dar0_reg_t ch4_dar0;
    volatile dmac_ch4_dar1_reg_t ch4_dar1;
    volatile dmac_ch4_block_ts0_reg_t ch4_block_ts0;
    uint32_t reserved_414;
    volatile dmac_ch4_ctl0_reg_t ch4_ctl0;
    volatile dmac_ch4_ctl1_reg_t ch4_ctl1;
    volatile dmac_ch4_cfg0_reg_t ch4_cfg0;
    volatile dmac_ch4_cfg1_reg_t ch4_cfg1;
    volatile dmac_ch4_llp0_reg_t ch4_llp0;
    volatile dmac_ch4_llp1_reg_t ch4_llp1;
    volatile dmac_ch4_status0_reg_t ch4_status0;
    volatile dmac_ch4_status1_reg_t ch4_status1;
    volatile dmac_ch4_swhssrc0_reg_t ch4_swhssrc0;
    uint32_t reserved_43c;
    volatile dmac_ch4_swhsdst0_reg_t ch4_swhsdst0;
    uint32_t reserved_444;
    volatile dmac_ch4_blk_tfr_resumereq0_reg_t ch4_blk_tfr_resumereq0;
    uint32_t reserved_44c;
    volatile dmac_ch4_axi_id0_reg_t ch4_axi_id0;
    uint32_t reserved_454;
    volatile dmac_ch4_axi_qos0_reg_t ch4_axi_qos0;
    uint32_t reserved_45c;
    volatile dmac_ch4_sstat0_reg_t ch4_sstat0;
    uint32_t reserved_464;
    volatile dmac_ch4_dstat0_reg_t ch4_dstat0;
    uint32_t reserved_46c;
    volatile dmac_ch4_sstatar0_reg_t ch4_sstatar0;
    volatile dmac_ch4_sstatar1_reg_t ch4_sstatar1;
    volatile dmac_ch4_dstatar0_reg_t ch4_dstatar0;
    volatile dmac_ch4_dstatar1_reg_t ch4_dstatar1;
    volatile dmac_ch4_intstatus_enable0_reg_t ch4_intstatus_enable0;
    volatile dmac_ch4_intstatus_enable1_reg_t ch4_intstatus_enable1;
    volatile dmac_ch4_intstatus0_reg_t ch4_intstatus0;
    volatile dmac_ch4_intstatus1_reg_t ch4_intstatus1;
    volatile dmac_ch4_intsignal_enable0_reg_t ch4_intsignal_enable0;
    volatile dmac_ch4_intsignal_enable1_reg_t ch4_intsignal_enable1;
    volatile dmac_ch4_intclear0_reg_t ch4_intclear0;
    volatile dmac_ch4_intclear1_reg_t ch4_intclear1;
} dmac_dev_t;

extern dmac_dev_t GDMA;

#ifndef __cplusplus
_Static_assert(sizeof(dmac_dev_t) == 0x4a0, "Invalid size of dmac_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
