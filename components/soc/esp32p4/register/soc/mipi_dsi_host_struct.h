/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Version Register */
/** Type of version register
 *  NA
 */
typedef union {
    struct {
        /** version : RO; bitpos: [31:0]; default: 825504042;
         *  NA
         */
        uint32_t version:32;
    };
    uint32_t val;
} dsi_host_version_reg_t;


/** Group: Configuration Registers */
/** Type of pwr_up register
 *  NA
 */
typedef union {
    struct {
        /** shutdownz : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t shutdownz:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_host_pwr_up_reg_t;

/** Type of clkmgr_cfg register
 *  NA
 */
typedef union {
    struct {
        /** tx_esc_clk_division : R/W; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t tx_esc_clk_division:8;
        /** to_clk_division : R/W; bitpos: [15:8]; default: 0;
         *  NA
         */
        uint32_t to_clk_division:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_host_clkmgr_cfg_reg_t;

/** Type of dpi_vcid register
 *  NA
 */
typedef union {
    struct {
        /** dpi_vcid : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t dpi_vcid:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dsi_host_dpi_vcid_reg_t;

/** Type of dpi_color_coding register
 *  NA
 */
typedef union {
    struct {
        /** dpi_color_coding : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t dpi_color_coding:4;
        uint32_t reserved_4:4;
        /** loosely18_en : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t loosely18_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} dsi_host_dpi_color_coding_reg_t;

/** Type of dpi_cfg_pol register
 *  NA
 */
typedef union {
    struct {
        /** dataen_active_low : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t dataen_active_low:1;
        /** vsync_active_low : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t vsync_active_low:1;
        /** hsync_active_low : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t hsync_active_low:1;
        /** shutd_active_low : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t shutd_active_low:1;
        /** colorm_active_low : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t colorm_active_low:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} dsi_host_dpi_cfg_pol_reg_t;

/** Type of dpi_lp_cmd_tim register
 *  NA
 */
typedef union {
    struct {
        /** invact_lpcmd_time : R/W; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t invact_lpcmd_time:8;
        uint32_t reserved_8:8;
        /** outvact_lpcmd_time : R/W; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t outvact_lpcmd_time:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} dsi_host_dpi_lp_cmd_tim_reg_t;

/** Type of dbi_vcid register
 *  NA
 */
typedef union {
    struct {
        /** dbi_vcid : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t dbi_vcid:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dsi_host_dbi_vcid_reg_t;

/** Type of dbi_cfg register
 *  NA
 */
typedef union {
    struct {
        /** in_dbi_conf : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t in_dbi_conf:4;
        uint32_t reserved_4:4;
        /** out_dbi_conf : R/W; bitpos: [11:8]; default: 0;
         *  NA
         */
        uint32_t out_dbi_conf:4;
        uint32_t reserved_12:4;
        /** lut_size_conf : R/W; bitpos: [17:16]; default: 0;
         *  NA
         */
        uint32_t lut_size_conf:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} dsi_host_dbi_cfg_reg_t;

/** Type of dbi_partitioning_en register
 *  NA
 */
typedef union {
    struct {
        /** partitioning_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t partitioning_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_host_dbi_partitioning_en_reg_t;

/** Type of dbi_cmdsize register
 *  NA
 */
typedef union {
    struct {
        /** wr_cmd_size : R/W; bitpos: [15:0]; default: 0;
         *  NA
         */
        uint32_t wr_cmd_size:16;
        /** allowed_cmd_size : R/W; bitpos: [31:16]; default: 0;
         *  NA
         */
        uint32_t allowed_cmd_size:16;
    };
    uint32_t val;
} dsi_host_dbi_cmdsize_reg_t;

/** Type of pckhdl_cfg register
 *  NA
 */
typedef union {
    struct {
        /** eotp_tx_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t eotp_tx_en:1;
        /** eotp_rx_en : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t eotp_rx_en:1;
        /** bta_en : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t bta_en:1;
        /** ecc_rx_en : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ecc_rx_en:1;
        /** crc_rx_en : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t crc_rx_en:1;
        /** eotp_tx_lp_en : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t eotp_tx_lp_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dsi_host_pckhdl_cfg_reg_t;

/** Type of gen_vcid register
 *  NA
 */
typedef union {
    struct {
        /** gen_vcid_rx : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t gen_vcid_rx:2;
        uint32_t reserved_2:6;
        /** gen_vcid_tear_auto : R/W; bitpos: [9:8]; default: 0;
         *  NA
         */
        uint32_t gen_vcid_tear_auto:2;
        uint32_t reserved_10:6;
        /** gen_vcid_tx_auto : R/W; bitpos: [17:16]; default: 0;
         *  NA
         */
        uint32_t gen_vcid_tx_auto:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} dsi_host_gen_vcid_reg_t;

/** Type of mode_cfg register
 *  NA
 */
typedef union {
    struct {
        /** cmd_video_mode : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t cmd_video_mode:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_host_mode_cfg_reg_t;

/** Type of vid_mode_cfg register
 *  NA
 */
typedef union {
    struct {
        /** vid_mode_type : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t vid_mode_type:2;
        uint32_t reserved_2:6;
        /** lp_vsa_en : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t lp_vsa_en:1;
        /** lp_vbp_en : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t lp_vbp_en:1;
        /** lp_vfp_en : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t lp_vfp_en:1;
        /** lp_vact_en : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t lp_vact_en:1;
        /** lp_hbp_en : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t lp_hbp_en:1;
        /** lp_hfp_en : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t lp_hfp_en:1;
        /** frame_bta_ack_en : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t frame_bta_ack_en:1;
        /** lp_cmd_en : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t lp_cmd_en:1;
        /** vpg_en : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t vpg_en:1;
        uint32_t reserved_17:3;
        /** vpg_mode : R/W; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t vpg_mode:1;
        uint32_t reserved_21:3;
        /** vpg_orientation : R/W; bitpos: [24]; default: 0;
         *  NA
         */
        uint32_t vpg_orientation:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} dsi_host_vid_mode_cfg_reg_t;

/** Type of vid_pkt_size register
 *  NA
 */
typedef union {
    struct {
        /** vid_pkt_size : R/W; bitpos: [13:0]; default: 0;
         *  NA
         */
        uint32_t vid_pkt_size:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dsi_host_vid_pkt_size_reg_t;

/** Type of vid_num_chunks register
 *  NA
 */
typedef union {
    struct {
        /** vid_num_chunks : R/W; bitpos: [12:0]; default: 0;
         *  NA
         */
        uint32_t vid_num_chunks:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} dsi_host_vid_num_chunks_reg_t;

/** Type of vid_null_size register
 *  NA
 */
typedef union {
    struct {
        /** vid_null_size : R/W; bitpos: [12:0]; default: 0;
         *  NA
         */
        uint32_t vid_null_size:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} dsi_host_vid_null_size_reg_t;

/** Type of vid_hsa_time register
 *  NA
 */
typedef union {
    struct {
        /** vid_hsa_time : R/W; bitpos: [11:0]; default: 0;
         *  NA
         */
        uint32_t vid_hsa_time:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} dsi_host_vid_hsa_time_reg_t;

/** Type of vid_hbp_time register
 *  NA
 */
typedef union {
    struct {
        /** vid_hbp_time : R/W; bitpos: [11:0]; default: 0;
         *  NA
         */
        uint32_t vid_hbp_time:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} dsi_host_vid_hbp_time_reg_t;

/** Type of vid_hline_time register
 *  NA
 */
typedef union {
    struct {
        /** vid_hline_time : R/W; bitpos: [14:0]; default: 0;
         *  NA
         */
        uint32_t vid_hline_time:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} dsi_host_vid_hline_time_reg_t;

/** Type of vid_vsa_lines register
 *  NA
 */
typedef union {
    struct {
        /** vsa_lines : R/W; bitpos: [9:0]; default: 0;
         *  NA
         */
        uint32_t vsa_lines:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} dsi_host_vid_vsa_lines_reg_t;

/** Type of vid_vbp_lines register
 *  NA
 */
typedef union {
    struct {
        /** vbp_lines : R/W; bitpos: [9:0]; default: 0;
         *  NA
         */
        uint32_t vbp_lines:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} dsi_host_vid_vbp_lines_reg_t;

/** Type of vid_vfp_lines register
 *  NA
 */
typedef union {
    struct {
        /** vfp_lines : R/W; bitpos: [9:0]; default: 0;
         *  NA
         */
        uint32_t vfp_lines:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} dsi_host_vid_vfp_lines_reg_t;

/** Type of vid_vactive_lines register
 *  NA
 */
typedef union {
    struct {
        /** v_active_lines : R/W; bitpos: [13:0]; default: 0;
         *  NA
         */
        uint32_t v_active_lines:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dsi_host_vid_vactive_lines_reg_t;

/** Type of edpi_cmd_size register
 *  NA
 */
typedef union {
    struct {
        /** edpi_allowed_cmd_size : R/W; bitpos: [15:0]; default: 0;
         *  NA
         */
        uint32_t edpi_allowed_cmd_size:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_host_edpi_cmd_size_reg_t;

/** Type of cmd_mode_cfg register
 *  NA
 */
typedef union {
    struct {
        /** tear_fx_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t tear_fx_en:1;
        /** ack_rqst_en : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ack_rqst_en:1;
        uint32_t reserved_2:6;
        /** gen_sw_0p_tx : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t gen_sw_0p_tx:1;
        /** gen_sw_1p_tx : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t gen_sw_1p_tx:1;
        /** gen_sw_2p_tx : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t gen_sw_2p_tx:1;
        /** gen_sr_0p_tx : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t gen_sr_0p_tx:1;
        /** gen_sr_1p_tx : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t gen_sr_1p_tx:1;
        /** gen_sr_2p_tx : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t gen_sr_2p_tx:1;
        /** gen_lw_tx : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t gen_lw_tx:1;
        uint32_t reserved_15:1;
        /** dcs_sw_0p_tx : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t dcs_sw_0p_tx:1;
        /** dcs_sw_1p_tx : R/W; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t dcs_sw_1p_tx:1;
        /** dcs_sr_0p_tx : R/W; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t dcs_sr_0p_tx:1;
        /** dcs_lw_tx : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t dcs_lw_tx:1;
        uint32_t reserved_20:4;
        /** max_rd_pkt_size : R/W; bitpos: [24]; default: 0;
         *  NA
         */
        uint32_t max_rd_pkt_size:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} dsi_host_cmd_mode_cfg_reg_t;

/** Type of gen_hdr register
 *  NA
 */
typedef union {
    struct {
        /** gen_dt : R/W; bitpos: [5:0]; default: 0;
         *  NA
         */
        uint32_t gen_dt:6;
        /** gen_vc : R/W; bitpos: [7:6]; default: 0;
         *  NA
         */
        uint32_t gen_vc:2;
        /** gen_wc_lsbyte : R/W; bitpos: [15:8]; default: 0;
         *  NA
         */
        uint32_t gen_wc_lsbyte:8;
        /** gen_wc_msbyte : R/W; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t gen_wc_msbyte:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} dsi_host_gen_hdr_reg_t;

/** Type of gen_pld_data register
 *  NA
 */
typedef union {
    struct {
        /** gen_pld_b1 : R/W; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t gen_pld_b1:8;
        /** gen_pld_b2 : R/W; bitpos: [15:8]; default: 0;
         *  NA
         */
        uint32_t gen_pld_b2:8;
        /** gen_pld_b3 : R/W; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t gen_pld_b3:8;
        /** gen_pld_b4 : R/W; bitpos: [31:24]; default: 0;
         *  NA
         */
        uint32_t gen_pld_b4:8;
    };
    uint32_t val;
} dsi_host_gen_pld_data_reg_t;

/** Type of to_cnt_cfg register
 *  NA
 */
typedef union {
    struct {
        /** lprx_to_cnt : R/W; bitpos: [15:0]; default: 0;
         *  NA
         */
        uint32_t lprx_to_cnt:16;
        /** hstx_to_cnt : R/W; bitpos: [31:16]; default: 0;
         *  NA
         */
        uint32_t hstx_to_cnt:16;
    };
    uint32_t val;
} dsi_host_to_cnt_cfg_reg_t;

/** Type of hs_rd_to_cnt register
 *  NA
 */
typedef union {
    struct {
        /** hs_rd_to_cnt : R/W; bitpos: [15:0]; default: 0;
         *  NA
         */
        uint32_t hs_rd_to_cnt:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_host_hs_rd_to_cnt_reg_t;

/** Type of lp_rd_to_cnt register
 *  NA
 */
typedef union {
    struct {
        /** lp_rd_to_cnt : R/W; bitpos: [15:0]; default: 0;
         *  NA
         */
        uint32_t lp_rd_to_cnt:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_host_lp_rd_to_cnt_reg_t;

/** Type of hs_wr_to_cnt register
 *  NA
 */
typedef union {
    struct {
        /** hs_wr_to_cnt : R/W; bitpos: [15:0]; default: 0;
         *  NA
         */
        uint32_t hs_wr_to_cnt:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_host_hs_wr_to_cnt_reg_t;

/** Type of lp_wr_to_cnt register
 *  NA
 */
typedef union {
    struct {
        /** lp_wr_to_cnt : R/W; bitpos: [15:0]; default: 0;
         *  NA
         */
        uint32_t lp_wr_to_cnt:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_host_lp_wr_to_cnt_reg_t;

/** Type of bta_to_cnt register
 *  NA
 */
typedef union {
    struct {
        /** bta_to_cnt : R/W; bitpos: [15:0]; default: 0;
         *  NA
         */
        uint32_t bta_to_cnt:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_host_bta_to_cnt_reg_t;

/** Type of sdf_3d register
 *  NA
 */
typedef union {
    struct {
        /** mode_3d : R/W; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t mode_3d:2;
        /** format_3d : R/W; bitpos: [3:2]; default: 0;
         *  NA
         */
        uint32_t format_3d:2;
        /** second_vsync : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t second_vsync:1;
        /** right_first : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t right_first:1;
        uint32_t reserved_6:10;
        /** send_3d_cfg : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t send_3d_cfg:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dsi_host_sdf_3d_reg_t;

/** Type of lpclk_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** phy_txrequestclkhs : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t phy_txrequestclkhs:1;
        /** auto_clklane_ctrl : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t auto_clklane_ctrl:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dsi_host_lpclk_ctrl_reg_t;

/** Type of phy_tmr_lpclk_cfg register
 *  NA
 */
typedef union {
    struct {
        /** phy_clklp2hs_time : R/W; bitpos: [9:0]; default: 0;
         *  NA
         */
        uint32_t phy_clklp2hs_time:10;
        uint32_t reserved_10:6;
        /** phy_clkhs2lp_time : R/W; bitpos: [25:16]; default: 0;
         *  NA
         */
        uint32_t phy_clkhs2lp_time:10;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} dsi_host_phy_tmr_lpclk_cfg_reg_t;

/** Type of phy_tmr_cfg register
 *  NA
 */
typedef union {
    struct {
        /** phy_lp2hs_time : R/W; bitpos: [9:0]; default: 0;
         *  NA
         */
        uint32_t phy_lp2hs_time:10;
        uint32_t reserved_10:6;
        /** phy_hs2lp_time : R/W; bitpos: [25:16]; default: 0;
         *  NA
         */
        uint32_t phy_hs2lp_time:10;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} dsi_host_phy_tmr_cfg_reg_t;

/** Type of phy_rstz register
 *  NA
 */
typedef union {
    struct {
        /** phy_shutdownz : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t phy_shutdownz:1;
        /** phy_rstz : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t phy_rstz:1;
        /** phy_enableclk : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t phy_enableclk:1;
        /** phy_forcepll : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t phy_forcepll:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dsi_host_phy_rstz_reg_t;

/** Type of phy_if_cfg register
 *  NA
 */
typedef union {
    struct {
        /** n_lanes : R/W; bitpos: [1:0]; default: 1;
         *  NA
         */
        uint32_t n_lanes:2;
        uint32_t reserved_2:6;
        /** phy_stop_wait_time : R/W; bitpos: [15:8]; default: 0;
         *  NA
         */
        uint32_t phy_stop_wait_time:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_host_phy_if_cfg_reg_t;

/** Type of phy_ulps_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** phy_txrequlpsclk : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t phy_txrequlpsclk:1;
        /** phy_txexitulpsclk : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t phy_txexitulpsclk:1;
        /** phy_txrequlpslan : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t phy_txrequlpslan:1;
        /** phy_txexitulpslan : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t phy_txexitulpslan:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dsi_host_phy_ulps_ctrl_reg_t;

/** Type of phy_tx_triggers register
 *  NA
 */
typedef union {
    struct {
        /** phy_tx_triggers : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t phy_tx_triggers:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dsi_host_phy_tx_triggers_reg_t;

/** Type of phy_tst_ctrl0 register
 *  NA
 */
typedef union {
    struct {
        /** phy_testclr : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t phy_testclr:1;
        /** phy_testclk : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t phy_testclk:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dsi_host_phy_tst_ctrl0_reg_t;

/** Type of phy_tst_ctrl1 register
 *  NA
 */
typedef union {
    struct {
        /** phy_testdin : R/W; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t phy_testdin:8;
        /** pht_testdout : RO; bitpos: [15:8]; default: 0;
         *  NA
         */
        uint32_t pht_testdout:8;
        /** phy_testen : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t phy_testen:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dsi_host_phy_tst_ctrl1_reg_t;

/** Type of phy_cal register
 *  NA
 */
typedef union {
    struct {
        /** txskewcalhs : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t txskewcalhs:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_host_phy_cal_reg_t;

/** Type of dsc_parameter register
 *  NA
 */
typedef union {
    struct {
        /** compression_mode : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t compression_mode:1;
        uint32_t reserved_1:7;
        /** compress_algo : R/W; bitpos: [9:8]; default: 0;
         *  NA
         */
        uint32_t compress_algo:2;
        uint32_t reserved_10:6;
        /** pps_sel : R/W; bitpos: [17:16]; default: 0;
         *  NA
         */
        uint32_t pps_sel:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} dsi_host_dsc_parameter_reg_t;

/** Type of phy_tmr_rd_cfg register
 *  NA
 */
typedef union {
    struct {
        /** max_rd_time : R/W; bitpos: [14:0]; default: 0;
         *  NA
         */
        uint32_t max_rd_time:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} dsi_host_phy_tmr_rd_cfg_reg_t;

/** Type of vid_shadow_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** vid_shadow_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t vid_shadow_en:1;
        uint32_t reserved_1:7;
        /** vid_shadow_req : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t vid_shadow_req:1;
        uint32_t reserved_9:7;
        /** vid_shadow_pin_req : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t vid_shadow_pin_req:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dsi_host_vid_shadow_ctrl_reg_t;

/** Type of edpi_te_hw_cfg register
 *  NA
 */
typedef union {
    struct {
        /** hw_tear_effect_on : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t hw_tear_effect_on:1;
        /** hw_tear_effect_gen : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t hw_tear_effect_gen:1;
        uint32_t reserved_2:2;
        /** hw_set_scan_line : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t hw_set_scan_line:1;
        uint32_t reserved_5:11;
        /** scan_line_parameter : R/W; bitpos: [31:16]; default: 0;
         *  NA
         */
        uint32_t scan_line_parameter:16;
    };
    uint32_t val;
} dsi_host_edpi_te_hw_cfg_reg_t;


/** Group: Status Registers */
/** Type of cmd_pkt_status register
 *  NA
 */
typedef union {
    struct {
        /** gen_cmd_empty : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t gen_cmd_empty:1;
        /** gen_cmd_full : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t gen_cmd_full:1;
        /** gen_pld_w_empty : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t gen_pld_w_empty:1;
        /** gen_pld_w_full : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t gen_pld_w_full:1;
        /** gen_pld_r_empty : RO; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t gen_pld_r_empty:1;
        /** gen_pld_r_full : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t gen_pld_r_full:1;
        /** gen_rd_cmd_busy : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t gen_rd_cmd_busy:1;
        uint32_t reserved_7:9;
        /** gen_buff_cmd_empty : RO; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t gen_buff_cmd_empty:1;
        /** gen_buff_cmd_full : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t gen_buff_cmd_full:1;
        /** gen_buff_pld_empty : RO; bitpos: [18]; default: 1;
         *  NA
         */
        uint32_t gen_buff_pld_empty:1;
        /** gen_buff_pld_full : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t gen_buff_pld_full:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} dsi_host_cmd_pkt_status_reg_t;

/** Type of phy_status register
 *  NA
 */
typedef union {
    struct {
        /** phy_lock : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t phy_lock:1;
        /** phy_direction : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t phy_direction:1;
        /** phy_stopstateclklane : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t phy_stopstateclklane:1;
        /** phy_ulpsactivenotclk : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t phy_ulpsactivenotclk:1;
        /** phy_stopstate0lane : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t phy_stopstate0lane:1;
        /** phy_ulpsactivenot0lane : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t phy_ulpsactivenot0lane:1;
        /** phy_rxulpsesc0lane : RO; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t phy_rxulpsesc0lane:1;
        /** phy_stopstate1lane : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t phy_stopstate1lane:1;
        /** phy_ulpsactivenot1lane : RO; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t phy_ulpsactivenot1lane:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} dsi_host_phy_status_reg_t;

/** Type of dpi_vcid_act register
 *  NA
 */
typedef union {
    struct {
        /** dpi_vcid_act : RO; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t dpi_vcid_act:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dsi_host_dpi_vcid_act_reg_t;

/** Type of dpi_color_coding_act register
 *  NA
 */
typedef union {
    struct {
        /** dpi_color_coding_act : RO; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t dpi_color_coding_act:4;
        uint32_t reserved_4:4;
        /** loosely18_en_act : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t loosely18_en_act:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} dsi_host_dpi_color_coding_act_reg_t;

/** Type of dpi_lp_cmd_tim_act register
 *  NA
 */
typedef union {
    struct {
        /** invact_lpcmd_time_act : RO; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t invact_lpcmd_time_act:8;
        uint32_t reserved_8:8;
        /** outvact_lpcmd_time_act : RO; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t outvact_lpcmd_time_act:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} dsi_host_dpi_lp_cmd_tim_act_reg_t;

/** Type of vid_mode_cfg_act register
 *  NA
 */
typedef union {
    struct {
        /** vid_mode_type_act : RO; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t vid_mode_type_act:2;
        /** lp_vsa_en_act : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t lp_vsa_en_act:1;
        /** lp_vbp_en_act : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t lp_vbp_en_act:1;
        /** lp_vfp_en_act : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t lp_vfp_en_act:1;
        /** lp_vact_en_act : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t lp_vact_en_act:1;
        /** lp_hbp_en_act : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t lp_hbp_en_act:1;
        /** lp_hfp_en_act : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t lp_hfp_en_act:1;
        /** frame_bta_ack_en_act : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t frame_bta_ack_en_act:1;
        /** lp_cmd_en_act : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t lp_cmd_en_act:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} dsi_host_vid_mode_cfg_act_reg_t;

/** Type of vid_pkt_size_act register
 *  NA
 */
typedef union {
    struct {
        /** vid_pkt_size_act : RO; bitpos: [13:0]; default: 0;
         *  NA
         */
        uint32_t vid_pkt_size_act:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dsi_host_vid_pkt_size_act_reg_t;

/** Type of vid_num_chunks_act register
 *  NA
 */
typedef union {
    struct {
        /** vid_num_chunks_act : RO; bitpos: [12:0]; default: 0;
         *  NA
         */
        uint32_t vid_num_chunks_act:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} dsi_host_vid_num_chunks_act_reg_t;

/** Type of vid_null_size_act register
 *  NA
 */
typedef union {
    struct {
        /** vid_null_size_act : RO; bitpos: [12:0]; default: 0;
         *  NA
         */
        uint32_t vid_null_size_act:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} dsi_host_vid_null_size_act_reg_t;

/** Type of vid_hsa_time_act register
 *  NA
 */
typedef union {
    struct {
        /** vid_hsa_time_act : RO; bitpos: [11:0]; default: 0;
         *  NA
         */
        uint32_t vid_hsa_time_act:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} dsi_host_vid_hsa_time_act_reg_t;

/** Type of vid_hbp_time_act register
 *  NA
 */
typedef union {
    struct {
        /** vid_hbp_time_act : RO; bitpos: [11:0]; default: 0;
         *  NA
         */
        uint32_t vid_hbp_time_act:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} dsi_host_vid_hbp_time_act_reg_t;

/** Type of vid_hline_time_act register
 *  NA
 */
typedef union {
    struct {
        /** vid_hline_time_act : RO; bitpos: [14:0]; default: 0;
         *  NA
         */
        uint32_t vid_hline_time_act:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} dsi_host_vid_hline_time_act_reg_t;

/** Type of vid_vsa_lines_act register
 *  NA
 */
typedef union {
    struct {
        /** vsa_lines_act : RO; bitpos: [9:0]; default: 0;
         *  NA
         */
        uint32_t vsa_lines_act:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} dsi_host_vid_vsa_lines_act_reg_t;

/** Type of vid_vbp_lines_act register
 *  NA
 */
typedef union {
    struct {
        /** vbp_lines_act : RO; bitpos: [9:0]; default: 0;
         *  NA
         */
        uint32_t vbp_lines_act:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} dsi_host_vid_vbp_lines_act_reg_t;

/** Type of vid_vfp_lines_act register
 *  NA
 */
typedef union {
    struct {
        /** vfp_lines_act : RO; bitpos: [9:0]; default: 0;
         *  NA
         */
        uint32_t vfp_lines_act:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} dsi_host_vid_vfp_lines_act_reg_t;

/** Type of vid_vactive_lines_act register
 *  NA
 */
typedef union {
    struct {
        /** v_active_lines_act : RO; bitpos: [13:0]; default: 0;
         *  NA
         */
        uint32_t v_active_lines_act:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dsi_host_vid_vactive_lines_act_reg_t;

/** Type of vid_pkt_status register
 *  NA
 */
typedef union {
    struct {
        /** dpi_cmd_w_empty : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t dpi_cmd_w_empty:1;
        /** dpi_cmd_w_full : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t dpi_cmd_w_full:1;
        /** dpi_pld_w_empty : RO; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t dpi_pld_w_empty:1;
        /** dpi_pld_w_full : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t dpi_pld_w_full:1;
        uint32_t reserved_4:12;
        /** dpi_buff_pld_empty : RO; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t dpi_buff_pld_empty:1;
        /** dpi_buff_pld_full : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t dpi_buff_pld_full:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} dsi_host_vid_pkt_status_reg_t;

/** Type of sdf_3d_act register
 *  NA
 */
typedef union {
    struct {
        /** mode_3d_act : RO; bitpos: [1:0]; default: 0;
         *  NA
         */
        uint32_t mode_3d_act:2;
        /** format_3d_act : RO; bitpos: [3:2]; default: 0;
         *  NA
         */
        uint32_t format_3d_act:2;
        /** second_vsync_act : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t second_vsync_act:1;
        /** right_first_act : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t right_first_act:1;
        uint32_t reserved_6:10;
        /** send_3d_cfg_act : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t send_3d_cfg_act:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dsi_host_sdf_3d_act_reg_t;


/** Group: Interrupt Registers */
/** Type of int_st0 register
 *  NA
 */
typedef union {
    struct {
        /** ack_with_err_0 : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_0:1;
        /** ack_with_err_1 : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_1:1;
        /** ack_with_err_2 : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_2:1;
        /** ack_with_err_3 : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_3:1;
        /** ack_with_err_4 : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_4:1;
        /** ack_with_err_5 : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_5:1;
        /** ack_with_err_6 : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_6:1;
        /** ack_with_err_7 : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_7:1;
        /** ack_with_err_8 : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_8:1;
        /** ack_with_err_9 : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_9:1;
        /** ack_with_err_10 : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_10:1;
        /** ack_with_err_11 : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_11:1;
        /** ack_with_err_12 : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_12:1;
        /** ack_with_err_13 : RO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_13:1;
        /** ack_with_err_14 : RO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_14:1;
        /** ack_with_err_15 : RO; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t ack_with_err_15:1;
        /** dphy_errors_0 : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t dphy_errors_0:1;
        /** dphy_errors_1 : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t dphy_errors_1:1;
        /** dphy_errors_2 : RO; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t dphy_errors_2:1;
        /** dphy_errors_3 : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t dphy_errors_3:1;
        /** dphy_errors_4 : RO; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t dphy_errors_4:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} dsi_host_int_st0_reg_t;

/** Type of int_st1 register
 *  NA
 */
typedef union {
    struct {
        /** to_hs_tx : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t to_hs_tx:1;
        /** to_lp_rx : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t to_lp_rx:1;
        /** ecc_single_err : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ecc_single_err:1;
        /** ecc_milti_err : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t ecc_milti_err:1;
        /** crc_err : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t crc_err:1;
        /** pkt_size_err : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t pkt_size_err:1;
        /** eopt_err : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t eopt_err:1;
        /** dpi_pld_wr_err : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t dpi_pld_wr_err:1;
        /** gen_cmd_wr_err : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t gen_cmd_wr_err:1;
        /** gen_pld_wr_err : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t gen_pld_wr_err:1;
        /** gen_pld_send_err : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t gen_pld_send_err:1;
        /** gen_pld_rd_err : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t gen_pld_rd_err:1;
        /** gen_pld_recev_err : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t gen_pld_recev_err:1;
        uint32_t reserved_13:6;
        /** dpi_buff_pld_under : RO; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t dpi_buff_pld_under:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} dsi_host_int_st1_reg_t;

/** Type of int_msk0 register
 *  NA
 */
typedef union {
    struct {
        /** mask_ack_with_err_0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_0:1;
        /** mask_ack_with_err_1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_1:1;
        /** mask_ack_with_err_2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_2:1;
        /** mask_ack_with_err_3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_3:1;
        /** mask_ack_with_err_4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_4:1;
        /** mask_ack_with_err_5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_5:1;
        /** mask_ack_with_err_6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_6:1;
        /** mask_ack_with_err_7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_7:1;
        /** mask_ack_with_err_8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_8:1;
        /** mask_ack_with_err_9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_9:1;
        /** mask_ack_with_err_10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_10:1;
        /** mask_ack_with_err_11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_11:1;
        /** mask_ack_with_err_12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_12:1;
        /** mask_ack_with_err_13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_13:1;
        /** mask_ack_with_err_14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_14:1;
        /** mask_ack_with_err_15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t mask_ack_with_err_15:1;
        /** mask_dphy_errors_0 : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t mask_dphy_errors_0:1;
        /** mask_dphy_errors_1 : R/W; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t mask_dphy_errors_1:1;
        /** mask_dphy_errors_2 : R/W; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t mask_dphy_errors_2:1;
        /** mask_dphy_errors_3 : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t mask_dphy_errors_3:1;
        /** mask_dphy_errors_4 : R/W; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t mask_dphy_errors_4:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} dsi_host_int_msk0_reg_t;

/** Type of int_msk1 register
 *  NA
 */
typedef union {
    struct {
        /** mask_to_hs_tx : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_to_hs_tx:1;
        /** mask_to_lp_rx : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_to_lp_rx:1;
        /** mask_ecc_single_err : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t mask_ecc_single_err:1;
        /** mask_ecc_milti_err : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t mask_ecc_milti_err:1;
        /** mask_crc_err : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t mask_crc_err:1;
        /** mask_pkt_size_err : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t mask_pkt_size_err:1;
        /** mask_eopt_err : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t mask_eopt_err:1;
        /** mask_dpi_pld_wr_err : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t mask_dpi_pld_wr_err:1;
        /** mask_gen_cmd_wr_err : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t mask_gen_cmd_wr_err:1;
        /** mask_gen_pld_wr_err : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mask_gen_pld_wr_err:1;
        /** mask_gen_pld_send_err : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mask_gen_pld_send_err:1;
        /** mask_gen_pld_rd_err : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mask_gen_pld_rd_err:1;
        /** mask_gen_pld_recev_err : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mask_gen_pld_recev_err:1;
        uint32_t reserved_13:6;
        /** mask_dpi_buff_pld_under : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t mask_dpi_buff_pld_under:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} dsi_host_int_msk1_reg_t;

/** Type of int_force0 register
 *  NA
 */
typedef union {
    struct {
        /** force_ack_with_err_0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_0:1;
        /** force_ack_with_err_1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_1:1;
        /** force_ack_with_err_2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_2:1;
        /** force_ack_with_err_3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_3:1;
        /** force_ack_with_err_4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_4:1;
        /** force_ack_with_err_5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_5:1;
        /** force_ack_with_err_6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_6:1;
        /** force_ack_with_err_7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_7:1;
        /** force_ack_with_err_8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_8:1;
        /** force_ack_with_err_9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_9:1;
        /** force_ack_with_err_10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_10:1;
        /** force_ack_with_err_11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_11:1;
        /** force_ack_with_err_12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_12:1;
        /** force_ack_with_err_13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_13:1;
        /** force_ack_with_err_14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_14:1;
        /** force_ack_with_err_15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t force_ack_with_err_15:1;
        /** force_dphy_errors_0 : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t force_dphy_errors_0:1;
        /** force_dphy_errors_1 : R/W; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t force_dphy_errors_1:1;
        /** force_dphy_errors_2 : R/W; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t force_dphy_errors_2:1;
        /** force_dphy_errors_3 : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t force_dphy_errors_3:1;
        /** force_dphy_errors_4 : R/W; bitpos: [20]; default: 0;
         *  NA
         */
        uint32_t force_dphy_errors_4:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} dsi_host_int_force0_reg_t;

/** Type of int_force1 register
 *  NA
 */
typedef union {
    struct {
        /** force_to_hs_tx : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_to_hs_tx:1;
        /** force_to_lp_rx : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_to_lp_rx:1;
        /** force_ecc_single_err : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t force_ecc_single_err:1;
        /** force_ecc_milti_err : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t force_ecc_milti_err:1;
        /** force_crc_err : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t force_crc_err:1;
        /** force_pkt_size_err : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t force_pkt_size_err:1;
        /** force_eopt_err : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t force_eopt_err:1;
        /** force_dpi_pld_wr_err : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t force_dpi_pld_wr_err:1;
        /** force_gen_cmd_wr_err : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t force_gen_cmd_wr_err:1;
        /** force_gen_pld_wr_err : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t force_gen_pld_wr_err:1;
        /** force_gen_pld_send_err : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t force_gen_pld_send_err:1;
        /** force_gen_pld_rd_err : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t force_gen_pld_rd_err:1;
        /** force_gen_pld_recev_err : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t force_gen_pld_recev_err:1;
        uint32_t reserved_13:6;
        /** force_dpi_buff_pld_under : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t force_dpi_buff_pld_under:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} dsi_host_int_force1_reg_t;


typedef struct dsi_host_dev_t {
    volatile dsi_host_version_reg_t version;
    volatile dsi_host_pwr_up_reg_t pwr_up;
    volatile dsi_host_clkmgr_cfg_reg_t clkmgr_cfg;
    volatile dsi_host_dpi_vcid_reg_t dpi_vcid;
    volatile dsi_host_dpi_color_coding_reg_t dpi_color_coding;
    volatile dsi_host_dpi_cfg_pol_reg_t dpi_cfg_pol;
    volatile dsi_host_dpi_lp_cmd_tim_reg_t dpi_lp_cmd_tim;
    volatile dsi_host_dbi_vcid_reg_t dbi_vcid;
    volatile dsi_host_dbi_cfg_reg_t dbi_cfg;
    volatile dsi_host_dbi_partitioning_en_reg_t dbi_partitioning_en;
    volatile dsi_host_dbi_cmdsize_reg_t dbi_cmdsize;
    volatile dsi_host_pckhdl_cfg_reg_t pckhdl_cfg;
    volatile dsi_host_gen_vcid_reg_t gen_vcid;
    volatile dsi_host_mode_cfg_reg_t mode_cfg;
    volatile dsi_host_vid_mode_cfg_reg_t vid_mode_cfg;
    volatile dsi_host_vid_pkt_size_reg_t vid_pkt_size;
    volatile dsi_host_vid_num_chunks_reg_t vid_num_chunks;
    volatile dsi_host_vid_null_size_reg_t vid_null_size;
    volatile dsi_host_vid_hsa_time_reg_t vid_hsa_time;
    volatile dsi_host_vid_hbp_time_reg_t vid_hbp_time;
    volatile dsi_host_vid_hline_time_reg_t vid_hline_time;
    volatile dsi_host_vid_vsa_lines_reg_t vid_vsa_lines;
    volatile dsi_host_vid_vbp_lines_reg_t vid_vbp_lines;
    volatile dsi_host_vid_vfp_lines_reg_t vid_vfp_lines;
    volatile dsi_host_vid_vactive_lines_reg_t vid_vactive_lines;
    volatile dsi_host_edpi_cmd_size_reg_t edpi_cmd_size;
    volatile dsi_host_cmd_mode_cfg_reg_t cmd_mode_cfg;
    volatile dsi_host_gen_hdr_reg_t gen_hdr;
    volatile dsi_host_gen_pld_data_reg_t gen_pld_data;
    volatile dsi_host_cmd_pkt_status_reg_t cmd_pkt_status;
    volatile dsi_host_to_cnt_cfg_reg_t to_cnt_cfg;
    volatile dsi_host_hs_rd_to_cnt_reg_t hs_rd_timeout_cnt;
    volatile dsi_host_lp_rd_to_cnt_reg_t lp_rd_timeout_cnt;
    volatile dsi_host_hs_wr_to_cnt_reg_t hs_wr_timeout_cnt;
    volatile dsi_host_lp_wr_to_cnt_reg_t lp_wr_timeout_cnt;
    volatile dsi_host_bta_to_cnt_reg_t bta_timeout_cnt;
    volatile dsi_host_sdf_3d_reg_t sdf_3d;
    volatile dsi_host_lpclk_ctrl_reg_t lpclk_ctrl;
    volatile dsi_host_phy_tmr_lpclk_cfg_reg_t phy_tmr_lpclk_cfg;
    volatile dsi_host_phy_tmr_cfg_reg_t phy_tmr_cfg;
    volatile dsi_host_phy_rstz_reg_t phy_rstz;
    volatile dsi_host_phy_if_cfg_reg_t phy_if_cfg;
    volatile dsi_host_phy_ulps_ctrl_reg_t phy_ulps_ctrl;
    volatile dsi_host_phy_tx_triggers_reg_t phy_tx_triggers;
    volatile dsi_host_phy_status_reg_t phy_status;
    volatile dsi_host_phy_tst_ctrl0_reg_t phy_tst_ctrl0;
    volatile dsi_host_phy_tst_ctrl1_reg_t phy_tst_ctrl1;
    volatile dsi_host_int_st0_reg_t int_st0;
    volatile dsi_host_int_st1_reg_t int_st1;
    volatile dsi_host_int_msk0_reg_t int_msk0;
    volatile dsi_host_int_msk1_reg_t int_msk1;
    volatile dsi_host_phy_cal_reg_t phy_cal;
    uint32_t reserved_0d0[2];
    volatile dsi_host_int_force0_reg_t int_force0;
    volatile dsi_host_int_force1_reg_t int_force1;
    uint32_t reserved_0e0[4];
    volatile dsi_host_dsc_parameter_reg_t dsc_parameter;
    volatile dsi_host_phy_tmr_rd_cfg_reg_t phy_tmr_rd_cfg;
    uint32_t reserved_0f8[2];
    volatile dsi_host_vid_shadow_ctrl_reg_t vid_shadow_ctrl;
    uint32_t reserved_104[2];
    volatile dsi_host_dpi_vcid_act_reg_t dpi_vcid_act;
    volatile dsi_host_dpi_color_coding_act_reg_t dpi_color_coding_act;
    uint32_t reserved_114;
    volatile dsi_host_dpi_lp_cmd_tim_act_reg_t dpi_lp_cmd_tim_act;
    volatile dsi_host_edpi_te_hw_cfg_reg_t edpi_te_hw_cfg;
    uint32_t reserved_120[6];
    volatile dsi_host_vid_mode_cfg_act_reg_t vid_mode_cfg_act;
    volatile dsi_host_vid_pkt_size_act_reg_t vid_pkt_size_act;
    volatile dsi_host_vid_num_chunks_act_reg_t vid_num_chunks_act;
    volatile dsi_host_vid_null_size_act_reg_t vid_null_size_act;
    volatile dsi_host_vid_hsa_time_act_reg_t vid_hsa_time_act;
    volatile dsi_host_vid_hbp_time_act_reg_t vid_hbp_time_act;
    volatile dsi_host_vid_hline_time_act_reg_t vid_hline_time_act;
    volatile dsi_host_vid_vsa_lines_act_reg_t vid_vsa_lines_act;
    volatile dsi_host_vid_vbp_lines_act_reg_t vid_vbp_lines_act;
    volatile dsi_host_vid_vfp_lines_act_reg_t vid_vfp_lines_act;
    volatile dsi_host_vid_vactive_lines_act_reg_t vid_vactive_lines_act;
    uint32_t reserved_164;
    volatile dsi_host_vid_pkt_status_reg_t vid_pkt_status;
    uint32_t reserved_16c[9];
    volatile dsi_host_sdf_3d_act_reg_t sdf_3d_act;
} dsi_host_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(dsi_host_dev_t) == 0x194, "Invalid size of dsi_host_dev_t structure");
#endif

extern dsi_host_dev_t MIPI_DSI_HOST;

#ifdef __cplusplus
}
#endif
