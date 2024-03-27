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

/** Group: Configuration Registers */
/** Type of clk_en register
 *  dsi bridge clk control register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  this bit configures force_on of dsi_bridge register clock gate
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_brg_clk_en_reg_t;

/** Type of en register
 *  dsi bridge en register
 */
typedef union {
    struct {
        /** dsi_en : R/W; bitpos: [0]; default: 0;
         *  this bit configures module enable of dsi_bridge. 0: disable, 1: enable
         */
        uint32_t dsi_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_brg_en_reg_t;

/** Type of dma_req_cfg register
 *  dsi bridge dma burst len register
 */
typedef union {
    struct {
        /** dma_burst_len : R/W; bitpos: [11:0]; default: 128;
         *  this field configures the num of 64-bit in one dma burst transfer, valid only when
         *  dsi_bridge as flow controller
         */
        uint32_t dma_burst_len:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} dsi_brg_dma_req_cfg_reg_t;

/** Type of raw_num_cfg register
 *  dsi bridge raw number control register
 */
typedef union {
    struct {
        /** raw_num_total : R/W; bitpos: [21:0]; default: 230400;
         *  this field configures number of total pix bits/64
         */
        uint32_t raw_num_total:22;
        /** unalign_64bit_en : R/W; bitpos: [22]; default: 0;
         *  this field configures whether the total pix bits is a multiple of 64bits. 0: align
         *  to 64-bit, 1: unalign to 64-bit
         */
        uint32_t unalign_64bit_en:1;
        uint32_t reserved_23:8;
        /** raw_num_total_set : WT; bitpos: [31]; default: 0;
         *  this bit configures enable of reload reg_raw_num_total to internal cnt. 0: disable,
         *  1: enable. valid only when dsi_bridge as flow controller
         */
        uint32_t raw_num_total_set:1;
    };
    uint32_t val;
} dsi_brg_raw_num_cfg_reg_t;

/** Type of raw_buf_credit_ctl register
 *  dsi bridge credit register
 */
typedef union {
    struct {
        /** credit_thrd : R/W; bitpos: [14:0]; default: 1024;
         *  this field configures the threshold whether dsi_bridge fifo can receive one more
         *  64-bit, valid only when dsi_bridge as flow controller
         */
        uint32_t credit_thrd:15;
        uint32_t reserved_15:1;
        /** credit_burst_thrd : R/W; bitpos: [30:16]; default: 800;
         *  this field configures the threshold whether dsi_bridge fifo can receive one more
         *  dma burst, valid only when dsi_bridge as flow controller
         */
        uint32_t credit_burst_thrd:15;
        /** credit_reset : R/W; bitpos: [31]; default: 0;
         *  this bit configures internal credit cnt clear, 0: non, 1: reset. valid only when
         *  dsi_bridge as flow controller
         */
        uint32_t credit_reset:1;
    };
    uint32_t val;
} dsi_brg_raw_buf_credit_ctl_reg_t;

/** Type of pixel_type register
 *  dsi bridge dpi type control register
 */
typedef union {
    struct {
        /** raw_type : R/W; bitpos: [3:0]; default: 0;
         *  this field configures the pixel type. 0: rgb888, 1:rgb666, 2:rgb565
         */
        uint32_t raw_type:4;
        /** dpi_config : R/W; bitpos: [5:4]; default: 0;
         *  this field configures the pixel arrange type of dpi interface
         */
        uint32_t dpi_config:2;
        /** data_in_type : R/W; bitpos: [6]; default: 0;
         *  input data type, 0: rgb, 1: yuv
         */
        uint32_t data_in_type:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} dsi_brg_pixel_type_reg_t;

/** Type of dma_block_interval register
 *  dsi bridge dma block interval control register
 */
typedef union {
    struct {
        /** dma_block_slot : R/W; bitpos: [9:0]; default: 9;
         *  this field configures the max block_slot_cnt
         */
        uint32_t dma_block_slot:10;
        /** dma_block_interval : R/W; bitpos: [27:10]; default: 9;
         *  this field configures the max block_interval_cnt, block_interval_cnt increased by 1
         *  when block_slot_cnt if full
         */
        uint32_t dma_block_interval:18;
        /** raw_num_total_auto_reload : R/W; bitpos: [28]; default: 1;
         *  this bit configures enable of auto reload reg_raw_num_total, 0: disable, 1: enable
         */
        uint32_t raw_num_total_auto_reload:1;
        /** dma_block_interval_en : R/W; bitpos: [29]; default: 1;
         *  this bit configures enable of interval between dma block transfer, 0: disable, 1:
         *  enable
         */
        uint32_t dma_block_interval_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} dsi_brg_dma_block_interval_reg_t;

/** Type of dma_req_interval register
 *  dsi bridge dma req interval control register
 */
typedef union {
    struct {
        /** dma_req_interval : R/W; bitpos: [15:0]; default: 1;
         *  this field configures the interval between dma req events
         */
        uint32_t dma_req_interval:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_brg_dma_req_interval_reg_t;

/** Type of dpi_lcd_ctl register
 *  dsi bridge dpi signal control register
 */
typedef union {
    struct {
        /** dpishutdn : R/W; bitpos: [0]; default: 0;
         *  this bit configures dpishutdn signal in dpi interface
         */
        uint32_t dpishutdn:1;
        /** dpicolorm : R/W; bitpos: [1]; default: 0;
         *  this bit configures dpicolorm signal in dpi interface
         */
        uint32_t dpicolorm:1;
        /** dpiupdatecfg : R/W; bitpos: [2]; default: 0;
         *  this bit configures dpiupdatecfg signal in dpi interface
         */
        uint32_t dpiupdatecfg:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} dsi_brg_dpi_lcd_ctl_reg_t;

/** Type of dpi_rsv_dpi_data register
 *  dsi bridge dpi reserved data register
 */
typedef union {
    struct {
        /** dpi_rsv_data : R/W; bitpos: [29:0]; default: 16383;
         *  this field controls the pixel data sent to dsi_host when dsi_bridge fifo underflow
         */
        uint32_t dpi_rsv_data:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} dsi_brg_dpi_rsv_dpi_data_reg_t;

/** Type of dpi_v_cfg0 register
 *  dsi bridge dpi v config register 0
 */
typedef union {
    struct {
        /** vtotal : R/W; bitpos: [11:0]; default: 525;
         *  this field configures the total length of one frame (by line) for dpi output, must
         *  meet: reg_vtotal > reg_vdisp+reg_vsync+reg_vbank
         */
        uint32_t vtotal:12;
        uint32_t reserved_12:4;
        /** vdisp : R/W; bitpos: [27:16]; default: 480;
         *  this field configures the length of valid line (by line) for dpi output
         */
        uint32_t vdisp:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} dsi_brg_dpi_v_cfg0_reg_t;

/** Type of dpi_v_cfg1 register
 *  dsi bridge dpi v config register 1
 */
typedef union {
    struct {
        /** vbank : R/W; bitpos: [11:0]; default: 33;
         *  this field configures the length between vsync and valid line (by line) for dpi
         *  output
         */
        uint32_t vbank:12;
        uint32_t reserved_12:4;
        /** vsync : R/W; bitpos: [27:16]; default: 2;
         *  this field configures the length of vsync (by line) for dpi output
         */
        uint32_t vsync:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} dsi_brg_dpi_v_cfg1_reg_t;

/** Type of dpi_h_cfg0 register
 *  dsi bridge dpi h config register 0
 */
typedef union {
    struct {
        /** htotal : R/W; bitpos: [11:0]; default: 800;
         *  this field configures the total length of one line (by pixel num) for dpi output,
         *  must meet: reg_htotal > reg_hdisp+reg_hsync+reg_hbank
         */
        uint32_t htotal:12;
        uint32_t reserved_12:4;
        /** hdisp : R/W; bitpos: [27:16]; default: 640;
         *  this field configures the length of valid pixel data (by pixel num) for dpi output
         */
        uint32_t hdisp:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} dsi_brg_dpi_h_cfg0_reg_t;

/** Type of dpi_h_cfg1 register
 *  dsi bridge dpi h config register 1
 */
typedef union {
    struct {
        /** hbank : R/W; bitpos: [11:0]; default: 48;
         *  this field configures the length between hsync and pixel data valid (by pixel num)
         *  for dpi output
         */
        uint32_t hbank:12;
        uint32_t reserved_12:4;
        /** hsync : R/W; bitpos: [27:16]; default: 96;
         *  this field configures the length of hsync (by pixel num) for dpi output
         */
        uint32_t hsync:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} dsi_brg_dpi_h_cfg1_reg_t;

/** Type of dpi_misc_config register
 *  dsi_bridge dpi misc config register
 */
typedef union {
    struct {
        /** dpi_en : R/W; bitpos: [0]; default: 0;
         *  this bit configures enable of dpi output, 0: disable, 1: enable
         */
        uint32_t dpi_en:1;
        uint32_t reserved_1:3;
        /** fifo_underrun_discard_vcnt : R/W; bitpos: [15:4]; default: 413;
         *  this field configures the underrun interrupt musk, when underrun occurs and line
         *  cnt is less then this field
         */
        uint32_t fifo_underrun_discard_vcnt:12;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} dsi_brg_dpi_misc_config_reg_t;

/** Type of dpi_config_update register
 *  dsi_bridge dpi config update register
 */
typedef union {
    struct {
        /** dpi_config_update : WT; bitpos: [0]; default: 0;
         *  write 1 to this bit to update dpi config register MIPI_DSI_BRG_DPI_*
         */
        uint32_t dpi_config_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_brg_dpi_config_update_reg_t;

/** Type of host_trigger_rev register
 *  dsi_bridge host trigger reverse control register
 */
typedef union {
    struct {
        /** tx_trigger_rev_en : R/W; bitpos: [0]; default: 0;
         *  tx_trigger reverse. 0: disable, 1: enable
         */
        uint32_t tx_trigger_rev_en:1;
        /** rx_trigger_rev_en : R/W; bitpos: [1]; default: 0;
         *  rx_trigger reverse. 0: disable, 1: enable
         */
        uint32_t rx_trigger_rev_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dsi_brg_host_trigger_rev_reg_t;

/** Type of blk_raw_num_cfg register
 *  dsi_bridge block raw number control register
 */
typedef union {
    struct {
        /** blk_raw_num_total : R/W; bitpos: [21:0]; default: 230400;
         *  this field configures number of total block pix bits/64
         */
        uint32_t blk_raw_num_total:22;
        uint32_t reserved_22:9;
        /** blk_raw_num_total_set : WT; bitpos: [31]; default: 0;
         *  write 1 to reload reg_blk_raw_num_total to internal cnt
         */
        uint32_t blk_raw_num_total_set:1;
    };
    uint32_t val;
} dsi_brg_blk_raw_num_cfg_reg_t;

/** Type of dma_frame_interval register
 *  dsi_bridge dam frame interval control register
 */
typedef union {
    struct {
        /** dma_frame_slot : R/W; bitpos: [9:0]; default: 9;
         *  this field configures the max frame_slot_cnt
         */
        uint32_t dma_frame_slot:10;
        /** dma_frame_interval : R/W; bitpos: [27:10]; default: 9;
         *  this field configures the max frame_interval_cnt, frame_interval_cnt increased by 1
         *  when frame_slot_cnt if full
         */
        uint32_t dma_frame_interval:18;
        /** dma_multiblk_en : R/W; bitpos: [28]; default: 0;
         *  this bit configures enable multi-blk transfer, 0: disable, 1: enable
         */
        uint32_t dma_multiblk_en:1;
        /** dma_frame_interval_en : R/W; bitpos: [29]; default: 1;
         *  this bit configures enable interval between frame transfer, 0: disable, 1: enable
         */
        uint32_t dma_frame_interval_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} dsi_brg_dma_frame_interval_reg_t;

/** Type of mem_aux_ctrl register
 *  dsi_bridge mem aux control register
 */
typedef union {
    struct {
        /** dsi_mem_aux_ctrl : R/W; bitpos: [13:0]; default: 4896;
         *  this field configures dsi_bridge fifo memory aux ctrl
         */
        uint32_t dsi_mem_aux_ctrl:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dsi_brg_mem_aux_ctrl_reg_t;

/** Type of rdn_eco_low register
 *  dsi_bridge rdn eco all low register
 */
typedef union {
    struct {
        /** rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  rdn_eco_low
         */
        uint32_t rdn_eco_low:32;
    };
    uint32_t val;
} dsi_brg_rdn_eco_low_reg_t;

/** Type of rdn_eco_high register
 *  dsi_bridge rdn eco all high register
 */
typedef union {
    struct {
        /** rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  rdn_eco_high
         */
        uint32_t rdn_eco_high:32;
    };
    uint32_t val;
} dsi_brg_rdn_eco_high_reg_t;

/** Type of host_ctrl register
 *  dsi_bridge host control register
 */
typedef union {
    struct {
        /** dsi_cfg_ref_clk_en : R/W; bitpos: [0]; default: 1;
         *  this bit configures the clk enable refclk and cfg_clk of dsi_host. 0: disable, 1:
         *  enable
         */
        uint32_t dsi_cfg_ref_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_brg_host_ctrl_reg_t;

/** Type of mem_clk_ctrl register
 *  dsi_bridge mem force on control register
 */
typedef union {
    struct {
        /** dsi_bridge_mem_clk_force_on : R/W; bitpos: [0]; default: 0;
         *  this bit configures the clock force on of dsi_bridge fifo memory. 0: disable, 1:
         *  force on
         */
        uint32_t dsi_bridge_mem_clk_force_on:1;
        /** dsi_mem_clk_force_on : R/W; bitpos: [1]; default: 0;
         *  this bit configures the clock force on of dpi fifo memory. 0: disable, 1: force on
         */
        uint32_t dsi_mem_clk_force_on:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dsi_brg_mem_clk_ctrl_reg_t;

/** Type of dma_flow_ctrl register
 *  dsi_bridge dma flow controller register
 */
typedef union {
    struct {
        /** dsi_dma_flow_controller : R/W; bitpos: [0]; default: 1;
         *  this bit configures the flow controller, 0: dmac as flow controller, 1:dsi_bridge
         *  as flow controller
         */
        uint32_t dsi_dma_flow_controller:1;
        uint32_t reserved_1:3;
        /** dma_flow_multiblk_num : R/W; bitpos: [7:4]; default: 1;
         *  this field configures the num of blocks when multi-blk is enable and dmac as flow
         *  controller
         */
        uint32_t dma_flow_multiblk_num:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} dsi_brg_dma_flow_ctrl_reg_t;

/** Type of raw_buf_almost_empty_thrd register
 *  dsi_bridge buffer empty threshold register
 */
typedef union {
    struct {
        /** dsi_raw_buf_almost_empty_thrd : R/W; bitpos: [10:0]; default: 512;
         *  this field configures the fifo almost empty threshold, is valid only when dmac as
         *  flow controller
         */
        uint32_t dsi_raw_buf_almost_empty_thrd:11;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} dsi_brg_raw_buf_almost_empty_thrd_reg_t;

/** Type of yuv_cfg register
 *  dsi_bridge yuv format config register
 */
typedef union {
    struct {
        /** protocol : R/W; bitpos: [0]; default: 0;
         *  this bit configures yuv protocol, 0: bt.601, 1: bt.709
         */
        uint32_t protocol:1;
        /** yuv_pix_endian : R/W; bitpos: [1]; default: 0;
         *  this bit configures yuv pixel endian, 0: y0u0y1v1y2u2y3v3, 1: y3u3y2v2y1u1y0v0
         */
        uint32_t yuv_pix_endian:1;
        /** yuv422_format : R/W; bitpos: [3:2]; default: 0;
         *  this field configures yuv422 store format, 0: yuyv, 1: yvyu, 2: uyvy, 3: vyuy
         */
        uint32_t yuv422_format:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dsi_brg_yuv_cfg_reg_t;

/** Type of phy_lp_loopback_ctrl register
 *  dsi phy lp_loopback test ctrl
 */
typedef union {
    struct {
        /** phy_lp_txdataesc_1 : R/W; bitpos: [7:0]; default: 0;
         *  txdataesc_1 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_txdataesc_1:8;
        /** phy_lp_txrequestesc_1 : R/W; bitpos: [8]; default: 0;
         *  txrequestesc_1 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_txrequestesc_1:1;
        /** phy_lp_txvalidesc_1 : R/W; bitpos: [9]; default: 0;
         *  txvalidesc_1 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_txvalidesc_1:1;
        /** phy_lp_txlpdtesc_1 : R/W; bitpos: [10]; default: 0;
         *  txlpdtesc_1 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_txlpdtesc_1:1;
        /** phy_lp_basedir_1 : R/W; bitpos: [11]; default: 0;
         *  basedir_1 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_basedir_1:1;
        uint32_t reserved_12:4;
        /** phy_lp_txdataesc_0 : R/W; bitpos: [23:16]; default: 0;
         *  txdataesc_0 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_txdataesc_0:8;
        /** phy_lp_txrequestesc_0 : R/W; bitpos: [24]; default: 0;
         *  txrequestesc_0 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_txrequestesc_0:1;
        /** phy_lp_txvalidesc_0 : R/W; bitpos: [25]; default: 0;
         *  txvalidesc_0 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_txvalidesc_0:1;
        /** phy_lp_txlpdtesc_0 : R/W; bitpos: [26]; default: 0;
         *  txlpdtesc_0 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_txlpdtesc_0:1;
        /** phy_lp_basedir_0 : R/W; bitpos: [27]; default: 0;
         *  basedir_0 ctrl when enable dsi phy lp_loopback_test
         */
        uint32_t phy_lp_basedir_0:1;
        /** phy_lp_loopback_check : WT; bitpos: [28]; default: 0;
         *  dsi phy lp_loopback test start check
         */
        uint32_t phy_lp_loopback_check:1;
        /** phy_lp_loopback_check_done : RO; bitpos: [29]; default: 0;
         *  dsi phy lp_loopback test check done
         */
        uint32_t phy_lp_loopback_check_done:1;
        /** phy_lp_loopback_en : R/W; bitpos: [30]; default: 0;
         *  dsi phy lp_loopback ctrl en
         */
        uint32_t phy_lp_loopback_en:1;
        /** phy_lp_loopback_ok : RO; bitpos: [31]; default: 0;
         *  result of dsi phy lp_loopback test
         */
        uint32_t phy_lp_loopback_ok:1;
    };
    uint32_t val;
} dsi_brg_phy_lp_loopback_ctrl_reg_t;

/** Type of phy_hs_loopback_ctrl register
 *  dsi phy hp_loopback test ctrl
 */
typedef union {
    struct {
        /** phy_hs_txdatahs_1 : R/W; bitpos: [7:0]; default: 0;
         *  txdatahs_1 ctrl when enable dsi phy hs_loopback_test
         */
        uint32_t phy_hs_txdatahs_1:8;
        /** phy_hs_txrequestdatahs_1 : R/W; bitpos: [8]; default: 0;
         *  txrequestdatahs_1 ctrl when enable dsi phy hs_loopback_test
         */
        uint32_t phy_hs_txrequestdatahs_1:1;
        /** phy_hs_basedir_1 : R/W; bitpos: [9]; default: 1;
         *  basedir_1 ctrl when enable dsi phy hs_loopback_test
         */
        uint32_t phy_hs_basedir_1:1;
        uint32_t reserved_10:6;
        /** phy_hs_txdatahs_0 : R/W; bitpos: [23:16]; default: 0;
         *  txdatahs_0 ctrl when enable dsi phy hs_loopback_test
         */
        uint32_t phy_hs_txdatahs_0:8;
        /** phy_hs_txrequestdatahs_0 : R/W; bitpos: [24]; default: 0;
         *  txrequestdatahs_0 ctrl when enable dsi phy hs_loopback_test
         */
        uint32_t phy_hs_txrequestdatahs_0:1;
        /** phy_hs_basedir_0 : R/W; bitpos: [25]; default: 0;
         *  basedir_0 ctrl when enable dsi phy hs_loopback_test
         */
        uint32_t phy_hs_basedir_0:1;
        uint32_t reserved_26:1;
        /** phy_hs_txrequesthsclk : R/W; bitpos: [27]; default: 0;
         *  txrequesthsclk when  enable dsi phy hs_loopback_test
         */
        uint32_t phy_hs_txrequesthsclk:1;
        /** phy_hs_loopback_check : WT; bitpos: [28]; default: 0;
         *  dsi phy hs_loopback test start check
         */
        uint32_t phy_hs_loopback_check:1;
        /** phy_hs_loopback_check_done : RO; bitpos: [29]; default: 0;
         *  dsi phy hs_loopback test check done
         */
        uint32_t phy_hs_loopback_check_done:1;
        /** phy_hs_loopback_en : R/W; bitpos: [30]; default: 0;
         *  dsi phy hs_loopback ctrl en
         */
        uint32_t phy_hs_loopback_en:1;
        /** phy_hs_loopback_ok : RO; bitpos: [31]; default: 0;
         *  result of dsi phy hs_loopback test
         */
        uint32_t phy_hs_loopback_ok:1;
    };
    uint32_t val;
} dsi_brg_phy_hs_loopback_ctrl_reg_t;

/** Type of phy_loopback_cnt register
 *  loopback test cnt
 */
typedef union {
    struct {
        /** phy_hs_check_cnt_th : R/W; bitpos: [7:0]; default: 64;
         *  hs_loopback test check cnt
         */
        uint32_t phy_hs_check_cnt_th:8;
        uint32_t reserved_8:8;
        /** phy_lp_check_cnt_th : R/W; bitpos: [23:16]; default: 64;
         *  lp_loopback test check cnt
         */
        uint32_t phy_lp_check_cnt_th:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} dsi_brg_phy_loopback_cnt_reg_t;


/** Group: Status Registers */
/** Type of fifo_flow_status register
 *  dsi bridge raw buffer depth register
 */
typedef union {
    struct {
        /** raw_buf_depth : RO; bitpos: [13:0]; default: 0;
         *  this field configures the depth of dsi_bridge fifo depth
         */
        uint32_t raw_buf_depth:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dsi_brg_fifo_flow_status_reg_t;

/** Type of host_bist_ctl register
 *  dsi_bridge host bist control register
 */
typedef union {
    struct {
        /** bistok : RO; bitpos: [0]; default: 0;
         *  bistok
         */
        uint32_t bistok:1;
        /** biston : R/W; bitpos: [1]; default: 0;
         *  biston
         */
        uint32_t biston:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dsi_brg_host_bist_ctl_reg_t;

/** Type of rdn_eco_cs register
 *  dsi_bridge rdn eco cs register
 */
typedef union {
    struct {
        /** rdn_eco_en : R/W; bitpos: [0]; default: 0;
         *  rdn_eco_en
         */
        uint32_t rdn_eco_en:1;
        /** rdn_eco_result : RO; bitpos: [1]; default: 0;
         *  rdn_eco_result
         */
        uint32_t rdn_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dsi_brg_rdn_eco_cs_reg_t;


/** Group: Interrupt Registers */
/** Type of int_ena register
 *  dsi_bridge interrupt enable register
 */
typedef union {
    struct {
        /** underrun_int_ena : R/W; bitpos: [0]; default: 0;
         *  write 1 to enables dpi_underrun_int_st field of MIPI_DSI_BRG_INT_ST_REG controlled
         *  by dpi_underrun  interrupt signal
         */
        uint32_t underrun_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_brg_int_ena_reg_t;

/** Type of int_clr register
 *  dsi_bridge interrupt clear register
 */
typedef union {
    struct {
        /** underrun_int_clr : WT; bitpos: [0]; default: 0;
         *  write 1 to this bit to clear dpi_underrun_int_raw field of MIPI_DSI_BRG_INT_RAW_REG
         */
        uint32_t underrun_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_brg_int_clr_reg_t;

/** Type of int_raw register
 *  dsi_bridge raw interrupt register
 */
typedef union {
    struct {
        /** underrun_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  the raw interrupt status of dpi_underrun
         */
        uint32_t underrun_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_brg_int_raw_reg_t;

/** Type of int_st register
 *  dsi_bridge masked interrupt register
 */
typedef union {
    struct {
        /** underrun_int_st : RO; bitpos: [0]; default: 0;
         *  the masked interrupt status of dpi_underrun
         */
        uint32_t underrun_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} dsi_brg_int_st_reg_t;


typedef struct dsi_brg_dev_t {
    volatile dsi_brg_clk_en_reg_t clk_en;
    volatile dsi_brg_en_reg_t en;
    volatile dsi_brg_dma_req_cfg_reg_t dma_req_cfg;
    volatile dsi_brg_raw_num_cfg_reg_t raw_num_cfg;
    volatile dsi_brg_raw_buf_credit_ctl_reg_t raw_buf_credit_ctl;
    volatile dsi_brg_fifo_flow_status_reg_t fifo_flow_status;
    volatile dsi_brg_pixel_type_reg_t pixel_type;
    volatile dsi_brg_dma_block_interval_reg_t dma_block_interval;
    volatile dsi_brg_dma_req_interval_reg_t dma_req_interval;
    volatile dsi_brg_dpi_lcd_ctl_reg_t dpi_lcd_ctl;
    volatile dsi_brg_dpi_rsv_dpi_data_reg_t dpi_rsv_dpi_data;
    uint32_t reserved_02c;
    volatile dsi_brg_dpi_v_cfg0_reg_t dpi_v_cfg0;
    volatile dsi_brg_dpi_v_cfg1_reg_t dpi_v_cfg1;
    volatile dsi_brg_dpi_h_cfg0_reg_t dpi_h_cfg0;
    volatile dsi_brg_dpi_h_cfg1_reg_t dpi_h_cfg1;
    volatile dsi_brg_dpi_misc_config_reg_t dpi_misc_config;
    volatile dsi_brg_dpi_config_update_reg_t dpi_config_update;
    uint32_t reserved_048[2];
    volatile dsi_brg_int_ena_reg_t int_ena;
    volatile dsi_brg_int_clr_reg_t int_clr;
    volatile dsi_brg_int_raw_reg_t int_raw;
    volatile dsi_brg_int_st_reg_t int_st;
    volatile dsi_brg_host_bist_ctl_reg_t host_bist_ctl;
    volatile dsi_brg_host_trigger_rev_reg_t host_trigger_rev;
    volatile dsi_brg_blk_raw_num_cfg_reg_t blk_raw_num_cfg;
    volatile dsi_brg_dma_frame_interval_reg_t dma_frame_interval;
    volatile dsi_brg_mem_aux_ctrl_reg_t mem_aux_ctrl;
    volatile dsi_brg_rdn_eco_cs_reg_t rdn_eco_cs;
    volatile dsi_brg_rdn_eco_low_reg_t rdn_eco_low;
    volatile dsi_brg_rdn_eco_high_reg_t rdn_eco_high;
    volatile dsi_brg_host_ctrl_reg_t host_ctrl;
    volatile dsi_brg_mem_clk_ctrl_reg_t mem_clk_ctrl;
    volatile dsi_brg_dma_flow_ctrl_reg_t dma_flow_ctrl;
    volatile dsi_brg_raw_buf_almost_empty_thrd_reg_t raw_buf_almost_empty_thrd;
    volatile dsi_brg_yuv_cfg_reg_t yuv_cfg;
    volatile dsi_brg_phy_lp_loopback_ctrl_reg_t phy_lp_loopback_ctrl;
    volatile dsi_brg_phy_hs_loopback_ctrl_reg_t phy_hs_loopback_ctrl;
    volatile dsi_brg_phy_loopback_cnt_reg_t phy_loopback_cnt;
} dsi_brg_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(dsi_brg_dev_t) == 0xa0, "Invalid size of dsi_brg_dev_t structure");
#endif

extern dsi_brg_dev_t MIPI_DSI_BRIDGE;

#ifdef __cplusplus
}
#endif
