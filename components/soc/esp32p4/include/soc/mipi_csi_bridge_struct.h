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

/** Group: csi bridge regbank clock gating control register. */
/** Type of clk_en register
 *  csi bridge register mapping unit clock gating.
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  0: enable clock gating. 1: disable clock gating, clock always on.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} csi_brg_clk_en_reg_t;


/** Group: csi bridge control registers. */
/** Type of csi_en register
 *  csi bridge enable.
 */
typedef union {
    struct {
        /** csi_brg_en : R/W; bitpos: [0]; default: 0;
         *  0: disable csi bridge. 1: enable csi bridge.
         */
        uint32_t csi_brg_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} csi_brg_csi_en_reg_t;

/** Type of buf_flow_ctl register
 *  csi bridge buffer control.
 */
typedef union {
    struct {
        /** csi_buf_afull_thrd : R/W; bitpos: [13:0]; default: 2040;
         *  buffer almost full threshold.
         */
        uint32_t csi_buf_afull_thrd:14;
        uint32_t reserved_14:2;
        /** csi_buf_depth : RO; bitpos: [29:16]; default: 0;
         *  buffer data count.
         */
        uint32_t csi_buf_depth:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} csi_brg_buf_flow_ctl_reg_t;


/** Group: csi bridge dma control registers. */
/** Type of dma_req_cfg register
 *  dma request configuration.
 */
typedef union {
    struct {
        /** dma_burst_len : R/W; bitpos: [11:0]; default: 128;
         *  DMA burst length.
         */
        uint32_t dma_burst_len:12;
        /** dma_cfg_upd_by_blk : R/W; bitpos: [12]; default: 0;
         *  1: reg_dma_burst_len & reg_dma_burst_len will be updated by dma block finish. 0:
         *  updated by frame.
         */
        uint32_t dma_cfg_upd_by_blk:1;
        uint32_t reserved_13:3;
        /** dma_force_rd_status : R/W; bitpos: [16]; default: 0;
         *  1: mask dma request when reading frame info. 0: disable mask.
         */
        uint32_t dma_force_rd_status:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} csi_brg_dma_req_cfg_reg_t;

/** Type of dma_req_interval register
 *  DMA interval configuration.
 */
typedef union {
    struct {
        /** dma_req_interval : R/W; bitpos: [15:0]; default: 1;
         *  16'b1: 1 cycle. 16'b11: 2 cycle. ... ... 16'hFFFF: 16 cycle.
         */
        uint32_t dma_req_interval:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_brg_dma_req_interval_reg_t;

/** Type of dmablk_size register
 *  DMA block size configuration.
 */
typedef union {
    struct {
        /** dmablk_size : R/W; bitpos: [12:0]; default: 8191;
         *  the number of reg_dma_burst_len in a block
         */
        uint32_t dmablk_size:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} csi_brg_dmablk_size_reg_t;


/** Group: csi bridge frame format configuration registers. */
/** Type of data_type_cfg register
 *  pixel data type configuration.
 */
typedef union {
    struct {
        /** data_type_min : R/W; bitpos: [5:0]; default: 24;
         *  the min value of data type used for pixel filter.
         */
        uint32_t data_type_min:6;
        uint32_t reserved_6:2;
        /** data_type_max : R/W; bitpos: [13:8]; default: 47;
         *  the max value of data type used for pixel filter.
         */
        uint32_t data_type_max:6;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} csi_brg_data_type_cfg_reg_t;

/** Type of frame_cfg register
 *  frame configuration.
 */
typedef union {
    struct {
        /** vadr_num : R/W; bitpos: [11:0]; default: 480;
         *  vadr of frame data.
         */
        uint32_t vadr_num:12;
        /** hadr_num : R/W; bitpos: [23:12]; default: 480;
         *  hadr of frame data.
         */
        uint32_t hadr_num:12;
        /** has_hsync_e : R/W; bitpos: [24]; default: 1;
         *  0: frame data doesn't contain hsync. 1: frame data contains hsync.
         */
        uint32_t has_hsync_e:1;
        /** vadr_num_check : R/W; bitpos: [25]; default: 0;
         *  0: disable vadr check. 1: enable vadr check.
         */
        uint32_t vadr_num_check:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} csi_brg_frame_cfg_reg_t;

/** Type of endian_mode register
 *  data endianness order configuration.
 */
typedef union {
    struct {
        /** byte_endian_order : R/W; bitpos: [0]; default: 0;
         *  endianness order in bytes. 2'h0 is normal mode and 2'h3 is useful to YUV420(Legacy)
         *  when isp is bapassed.
         */
        uint32_t byte_endian_order:1;
        /** bit_endian_order : R/W; bitpos: [1]; default: 0;
         *  N/A
         */
        uint32_t bit_endian_order:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} csi_brg_endian_mode_reg_t;


/** Group: csi bridge interrupt registers. */
/** Type of int_raw register
 *  csi bridge interrupt raw.
 */
typedef union {
    struct {
        /** vadr_num_gt_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  reg_vadr_num is greater than real interrupt raw.
         */
        uint32_t vadr_num_gt_int_raw:1;
        /** vadr_num_lt_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  reg_vadr_num is less than real interrupt raw.
         */
        uint32_t vadr_num_lt_int_raw:1;
        /** discard_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  an incomplete frame of data was sent interrupt raw.
         */
        uint32_t discard_int_raw:1;
        /** csi_buf_overrun_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  buffer overrun interrupt raw.
         */
        uint32_t csi_buf_overrun_int_raw:1;
        /** csi_async_fifo_ovf_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  buffer overflow interrupt raw.
         */
        uint32_t csi_async_fifo_ovf_int_raw:1;
        /** dma_cfg_has_updated_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  dma configuration update complete interrupt raw.
         */
        uint32_t dma_cfg_has_updated_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} csi_brg_int_raw_reg_t;

/** Type of int_clr register
 *  csi bridge interrupt clr.
 */
typedef union {
    struct {
        /** vadr_num_gt_real_int_clr : WT; bitpos: [0]; default: 0;
         *  reg_vadr_num is greater than real interrupt clr.
         */
        uint32_t vadr_num_gt_real_int_clr:1;
        /** vadr_num_lt_real_int_clr : WT; bitpos: [1]; default: 0;
         *  reg_vadr_num is less than real interrupt clr.
         */
        uint32_t vadr_num_lt_real_int_clr:1;
        /** discard_int_clr : WT; bitpos: [2]; default: 0;
         *  an incomplete frame of data was sent interrupt clr.
         */
        uint32_t discard_int_clr:1;
        /** csi_buf_overrun_int_clr : WT; bitpos: [3]; default: 0;
         *  buffer overrun interrupt clr.
         */
        uint32_t csi_buf_overrun_int_clr:1;
        /** csi_async_fifo_ovf_int_clr : WT; bitpos: [4]; default: 0;
         *  buffer overflow interrupt clr.
         */
        uint32_t csi_async_fifo_ovf_int_clr:1;
        /** dma_cfg_has_updated_int_clr : WT; bitpos: [5]; default: 0;
         *  dma configuration update complete interrupt clr.
         */
        uint32_t dma_cfg_has_updated_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} csi_brg_int_clr_reg_t;

/** Type of int_st register
 *  csi bridge interrupt st.
 */
typedef union {
    struct {
        /** vadr_num_gt_int_st : RO; bitpos: [0]; default: 0;
         *  reg_vadr_num is greater than real interrupt st.
         */
        uint32_t vadr_num_gt_int_st:1;
        /** vadr_num_lt_int_st : RO; bitpos: [1]; default: 0;
         *  reg_vadr_num is less than real interrupt st.
         */
        uint32_t vadr_num_lt_int_st:1;
        /** discard_int_st : RO; bitpos: [2]; default: 0;
         *  an incomplete frame of data was sent interrupt st.
         */
        uint32_t discard_int_st:1;
        /** csi_buf_overrun_int_st : RO; bitpos: [3]; default: 0;
         *  buffer overrun interrupt st.
         */
        uint32_t csi_buf_overrun_int_st:1;
        /** csi_async_fifo_ovf_int_st : RO; bitpos: [4]; default: 0;
         *  buffer overflow interrupt st.
         */
        uint32_t csi_async_fifo_ovf_int_st:1;
        /** dma_cfg_has_updated_int_st : RO; bitpos: [5]; default: 0;
         *  dma configuration update complete interrupt st.
         */
        uint32_t dma_cfg_has_updated_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} csi_brg_int_st_reg_t;

/** Type of int_ena register
 *  csi bridge interrupt enable.
 */
typedef union {
    struct {
        /** vadr_num_gt_int_ena : R/W; bitpos: [0]; default: 0;
         *  reg_vadr_num is greater than real interrupt enable.
         */
        uint32_t vadr_num_gt_int_ena:1;
        /** vadr_num_lt_int_ena : R/W; bitpos: [1]; default: 0;
         *  reg_vadr_num is less than real interrupt enable.
         */
        uint32_t vadr_num_lt_int_ena:1;
        /** discard_int_ena : R/W; bitpos: [2]; default: 0;
         *  an incomplete frame of data was sent interrupt enable.
         */
        uint32_t discard_int_ena:1;
        /** csi_buf_overrun_int_ena : R/W; bitpos: [3]; default: 0;
         *  buffer overrun interrupt enable.
         */
        uint32_t csi_buf_overrun_int_ena:1;
        /** csi_async_fifo_ovf_int_ena : R/W; bitpos: [4]; default: 0;
         *  buffer overflow interrupt enable.
         */
        uint32_t csi_async_fifo_ovf_int_ena:1;
        /** dma_cfg_has_updated_int_ena : R/W; bitpos: [5]; default: 0;
         *  dma configuration update complete interrupt enable.
         */
        uint32_t dma_cfg_has_updated_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} csi_brg_int_ena_reg_t;


/** Group: csi-host control registers from csi bridge regbank. */
/** Type of host_ctrl register
 *  csi host control by csi bridge.
 */
typedef union {
    struct {
        /** csi_enableclk : R/W; bitpos: [0]; default: 1;
         *  enable clock lane module of csi phy.
         */
        uint32_t csi_enableclk:1;
        /** csi_cfg_clk_en : R/W; bitpos: [1]; default: 1;
         *  enable cfg_clk of csi host module.
         */
        uint32_t csi_cfg_clk_en:1;
        /** loopbk_test_en : R/W; bitpos: [2]; default: 0;
         *  for phy test by loopback dsi phy to csi phy.
         */
        uint32_t loopbk_test_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} csi_brg_host_ctrl_reg_t;


typedef struct {
    volatile csi_brg_clk_en_reg_t clk_en;
    volatile csi_brg_csi_en_reg_t csi_en;
    volatile csi_brg_dma_req_cfg_reg_t dma_req_cfg;
    volatile csi_brg_buf_flow_ctl_reg_t buf_flow_ctl;
    volatile csi_brg_data_type_cfg_reg_t data_type_cfg;
    volatile csi_brg_frame_cfg_reg_t frame_cfg;
    volatile csi_brg_endian_mode_reg_t endian_mode;
    volatile csi_brg_int_raw_reg_t int_raw;
    volatile csi_brg_int_clr_reg_t int_clr;
    volatile csi_brg_int_st_reg_t int_st;
    volatile csi_brg_int_ena_reg_t int_ena;
    volatile csi_brg_dma_req_interval_reg_t dma_req_interval;
    volatile csi_brg_dmablk_size_reg_t dmablk_size;
    uint32_t reserved_034[3];
    volatile csi_brg_host_ctrl_reg_t host_ctrl;
} csi_brg_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(csi_brg_dev_t) == 0x44, "Invalid size of csi_brg_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
