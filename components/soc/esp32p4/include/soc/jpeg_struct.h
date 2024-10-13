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

/** Group: Control and configuration registers */
/** Type of config register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** fsm_rst : WT; bitpos: [0]; default: 0;
         *  fsm reset
         */
        uint32_t fsm_rst:1;
        /** jpeg_start : WT; bitpos: [1]; default: 0;
         *  start to compress a new pic(in dma reg mode)
         */
        uint32_t jpeg_start:1;
        /** qnr_presition : R/W; bitpos: [2]; default: 0;
         *  0:8bit qnr,1:12bit qnr(TBD)
         */
        uint32_t qnr_presition:1;
        /** ff_check_en : R/W; bitpos: [3]; default: 1;
         *  enable whether to add "00" after "ff"
         */
        uint32_t ff_check_en:1;
        /** sample_sel : R/W; bitpos: [5:4]; default: 1;
         *  0:yuv444,1:yuv422, 2:yuv420
         */
        uint32_t sample_sel:2;
        /** dma_linklist_mode : RO; bitpos: [6]; default: 1;
         *  1:use linklist to configure dma
         */
        uint32_t dma_linklist_mode:1;
        /** debug_direct_out_en : R/W; bitpos: [7]; default: 0;
         *  0:normal mode,1:debug mode for direct output from input
         */
        uint32_t debug_direct_out_en:1;
        /** qnr_fifo_en : R/W; bitpos: [8]; default: 1;
         *  0:use non-fifo way to access qnr ram,1:use fifo way to access qnr ram
         */
        uint32_t qnr_fifo_en:1;
        /** lqnr_tbl_sel : R/W; bitpos: [10:9]; default: 0;
         *  choose  luminance quntization table id(TBD)
         */
        uint32_t lqnr_tbl_sel:2;
        /** cqnr_tbl_sel : R/W; bitpos: [12:11]; default: 1;
         *  choose  chrominance quntization table id (TBD)
         */
        uint32_t cqnr_tbl_sel:2;
        /** color_space : R/W; bitpos: [14:13]; default: 0;
         *  configure picture's color space:0-rb888,1-yuv422,2-rgb565, 3-gray
         */
        uint32_t color_space:2;
        /** dht_fifo_en : R/W; bitpos: [15]; default: 1;
         *  0:use non-fifo way to write dht len_total/codemin/value table,1:use fifo way to
         *  write dht len_total/codemin/value table. Reading dht len_total/codemin/value table
         *  only has nonfifo way
         */
        uint32_t dht_fifo_en:1;
        /** mem_clk_force_on : R/W; bitpos: [16]; default: 0;
         *  force memory's clock enabled
         */
        uint32_t mem_clk_force_on:1;
        /** jfif_ver : R/W; bitpos: [22:17]; default: 32;
         *  decode pause period to trigger decode_timeout int, the timeout periods =2 power
         *  (reg_decode_timeout_thres) -1
         */
        uint32_t jfif_ver:6;
        /** decode_timeout_task_sel : R/W; bitpos: [23]; default: 0;
         *  0: software use reset to abort decode process ,1: decoder abort decode process by
         *  itself
         */
        uint32_t decode_timeout_task_sel:1;
        /** soft_rst : R/W; bitpos: [24]; default: 0;
         *  when set to 1, soft reset JPEG module except jpeg_reg module
         */
        uint32_t soft_rst:1;
        /** fifo_rst : R/W; bitpos: [25]; default: 0;
         *  fifo reset
         */
        uint32_t fifo_rst:1;
        /** pixel_rev : R/W; bitpos: [26]; default: 0;
         *  reverse the source color pixel
         */
        uint32_t pixel_rev:1;
        /** tailer_en : R/W; bitpos: [27]; default: 0;
         *  set this bit to add EOI of "0xffd9" at the end of bitstream
         */
        uint32_t tailer_en:1;
        /** pause_en : R/W; bitpos: [28]; default: 0;
         *  set this bit to pause jpeg encoding
         */
        uint32_t pause_en:1;
        /** mem_force_pd : R/W; bitpos: [29]; default: 0;
         *  0: no operation,1:force jpeg memory to power down
         */
        uint32_t mem_force_pd:1;
        /** mem_force_pu : R/W; bitpos: [30]; default: 0;
         *  0: no operation,1:force jpeg memory to power up
         */
        uint32_t mem_force_pu:1;
        /** mode : R/W; bitpos: [31]; default: 0;
         *  0:encoder mode, 1: decoder mode
         */
        uint32_t mode:1;
    };
    uint32_t val;
} jpeg_config_reg_t;

/** Type of dqt_info register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** t0_dqt_info : R/W; bitpos: [7:0]; default: 0;
         *  Configure dqt table0's quantization coefficient precision in bit[7:4], configure
         *  dqt table0's table id in bit[3:0]
         */
        uint32_t t0_dqt_info:8;
        /** t1_dqt_info : R/W; bitpos: [15:8]; default: 1;
         *  Configure dqt table1's quantization coefficient precision in bit[7:4], configure
         *  dqt table1's table id in bit[3:0]
         */
        uint32_t t1_dqt_info:8;
        /** t2_dqt_info : R/W; bitpos: [23:16]; default: 2;
         *  Configure dqt table2's quantization coefficient precision in bit[7:4], configure
         *  dqt table2's table id in bit[3:0]
         */
        uint32_t t2_dqt_info:8;
        /** t3_dqt_info : R/W; bitpos: [31:24]; default: 3;
         *  Configure dqt table3's quantization coefficient precision in bit[7:4], configure
         *  dqt table3's table id in bit[3:0]
         */
        uint32_t t3_dqt_info:8;
    };
    uint32_t val;
} jpeg_dqt_info_reg_t;

/** Type of pic_size register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** va : R/W; bitpos: [15:0]; default: 480;
         *  configure picture's height. when encode, the max configurable bits is 14, when
         *  decode, the max configurable bits is 16
         */
        uint32_t va:16;
        /** ha : R/W; bitpos: [31:16]; default: 640;
         *  configure picture's width. when encode, the max configurable bits is 14, when
         *  decode, the max configurable bits is 16
         */
        uint32_t ha:16;
    };
    uint32_t val;
} jpeg_pic_size_reg_t;

/** Type of t0qnr register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** t0_qnr_val : HRO; bitpos: [31:0]; default: 0;
         *  write this reg to configure 64 quantization coefficient in t0 table
         */
        uint32_t t0_qnr_val:32;
    };
    uint32_t val;
} jpeg_t0qnr_reg_t;

/** Type of t1qnr register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chrominance_qnr_val : HRO; bitpos: [31:0]; default: 0;
         *  write this reg to configure 64 quantization coefficient in t1 table
         */
        uint32_t chrominance_qnr_val:32;
    };
    uint32_t val;
} jpeg_t1qnr_reg_t;

/** Type of t2qnr register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** t2_qnr_val : HRO; bitpos: [31:0]; default: 0;
         *  write this reg to configure 64 quantization coefficient in t2 table
         */
        uint32_t t2_qnr_val:32;
    };
    uint32_t val;
} jpeg_t2qnr_reg_t;

/** Type of t3qnr register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** t3_qnr_val : HRO; bitpos: [31:0]; default: 0;
         *  write this reg to configure 64 quantization coefficient in t3 table
         */
        uint32_t t3_qnr_val:32;
    };
    uint32_t val;
} jpeg_t3qnr_reg_t;

/** Type of decode_conf register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** restart_interval : R/W; bitpos: [15:0]; default: 0;
         *  configure restart interval in DRI marker when decode
         */
        uint32_t restart_interval:16;
        /** component_num : R/W; bitpos: [23:16]; default: 3;
         *  configure number of components in frame when decode
         */
        uint32_t component_num:8;
        /** sw_dht_en : RO; bitpos: [24]; default: 1;
         *  software decode dht table enable
         */
        uint32_t sw_dht_en:1;
        /** sos_check_byte_num : R/W; bitpos: [26:25]; default: 3;
         *  Configure the byte number to check next sos marker in the multi-scan picture after
         *  one scan is decoded down. The real check number is reg_sos_check_byte_num+1
         */
        uint32_t sos_check_byte_num:2;
        /** rst_check_byte_num : R/W; bitpos: [28:27]; default: 3;
         *  Configure the byte number to check next rst marker after one rst interval is
         *  decoded down. The real check number is reg_rst_check_byte_num+1
         */
        uint32_t rst_check_byte_num:2;
        /** multi_scan_err_check : R/W; bitpos: [29]; default: 0;
         *  reserved for decoder
         */
        uint32_t multi_scan_err_check:1;
        /** dezigzag_ready_ctl : R/W; bitpos: [30]; default: 1;
         *  reserved for decoder
         */
        uint32_t dezigzag_ready_ctl:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} jpeg_decode_conf_reg_t;

/** Type of c0 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** c0_dqt_tbl_sel : R/W; bitpos: [7:0]; default: 0;
         *  choose  c0 quntization table id (TBD)
         */
        uint32_t c0_dqt_tbl_sel:8;
        /** c0_y_factor : R/W; bitpos: [11:8]; default: 1;
         *  vertical sampling factor of c0
         */
        uint32_t c0_y_factor:4;
        /** c0_x_factor : R/W; bitpos: [15:12]; default: 1;
         *  horizontal sampling factor of c0
         */
        uint32_t c0_x_factor:4;
        /** c0_id : R/W; bitpos: [23:16]; default: 0;
         *  the identifier of c0
         */
        uint32_t c0_id:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} jpeg_c0_reg_t;

/** Type of c1 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** c1_dqt_tbl_sel : R/W; bitpos: [7:0]; default: 0;
         *  choose  c1 quntization table id (TBD)
         */
        uint32_t c1_dqt_tbl_sel:8;
        /** c1_y_factor : R/W; bitpos: [11:8]; default: 1;
         *  vertical sampling factor of c1
         */
        uint32_t c1_y_factor:4;
        /** c1_x_factor : R/W; bitpos: [15:12]; default: 1;
         *  horizontal sampling factor of c1
         */
        uint32_t c1_x_factor:4;
        /** c1_id : R/W; bitpos: [23:16]; default: 0;
         *  the identifier of c1
         */
        uint32_t c1_id:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} jpeg_c1_reg_t;

/** Type of c2 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** c2_dqt_tbl_sel : R/W; bitpos: [7:0]; default: 0;
         *  choose  c2 quntization table id (TBD)
         */
        uint32_t c2_dqt_tbl_sel:8;
        /** c2_y_factor : R/W; bitpos: [11:8]; default: 1;
         *  vertical sampling factor of c2
         */
        uint32_t c2_y_factor:4;
        /** c2_x_factor : R/W; bitpos: [15:12]; default: 1;
         *  horizontal sampling factor of c2
         */
        uint32_t c2_x_factor:4;
        /** c2_id : R/W; bitpos: [23:16]; default: 0;
         *  the identifier of c2
         */
        uint32_t c2_id:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} jpeg_c2_reg_t;

/** Type of c3 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** c3_dqt_tbl_sel : R/W; bitpos: [7:0]; default: 0;
         *  choose  c3 quntization table id (TBD)
         */
        uint32_t c3_dqt_tbl_sel:8;
        /** c3_y_factor : R/W; bitpos: [11:8]; default: 1;
         *  vertical sampling factor of c3
         */
        uint32_t c3_y_factor:4;
        /** c3_x_factor : R/W; bitpos: [15:12]; default: 1;
         *  horizontal sampling factor of c3
         */
        uint32_t c3_x_factor:4;
        /** c3_id : R/W; bitpos: [23:16]; default: 0;
         *  the identifier of c3
         */
        uint32_t c3_id:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} jpeg_c3_reg_t;

/** Type of dht_info register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** dc0_dht_id : R/W; bitpos: [3:0]; default: 0;
         *  configure dht dc table 0 id
         */
        uint32_t dc0_dht_id:4;
        /** dc1_dht_id : R/W; bitpos: [7:4]; default: 1;
         *  configure dht dc table 1 id
         */
        uint32_t dc1_dht_id:4;
        /** ac0_dht_id : R/W; bitpos: [11:8]; default: 0;
         *  configure dht ac table 0 id
         */
        uint32_t ac0_dht_id:4;
        /** ac1_dht_id : R/W; bitpos: [15:12]; default: 1;
         *  configure dht ac table 1 id
         */
        uint32_t ac1_dht_id:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} jpeg_dht_info_reg_t;


/** Group: Interrupt registers */
/** Type of int_raw register
 *  Interrupt raw registers
 */
typedef union {
    struct {
        /** done_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  This raw interrupt bit turns to high level when JPEG finishes encoding a picture..
         */
        uint32_t done_int_raw:1;
        /** rle_parallel_err_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit to sign that rle parallel error when decoding.
         */
        uint32_t rle_parallel_err_int_raw:1;
        /** cid_err_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit to sign that scan id check with component fails when decoding.
         */
        uint32_t cid_err_int_raw:1;
        /** c_dht_dc_id_err_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit to sign that scan component's dc dht id check with dc dht
         *  table's id fails when decoding.
         */
        uint32_t c_dht_dc_id_err_int_raw:1;
        /** c_dht_ac_id_err_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit to sign that scan component's ac dht id check with ac dht
         *  table's id fails when decoding.
         */
        uint32_t c_dht_ac_id_err_int_raw:1;
        /** c_dqt_id_err_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit to sign that scan component's dqt id check with dqt table's
         *  id fails when decoding.
         */
        uint32_t c_dqt_id_err_int_raw:1;
        /** rst_uxp_err_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit to sign that RST header marker is detected but restart
         *  interval is 0 when decoding.
         */
        uint32_t rst_uxp_err_int_raw:1;
        /** rst_check_none_err_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit to sign that RST header marker is not detected but restart
         *  interval is not 0 when decoding.
         */
        uint32_t rst_check_none_err_int_raw:1;
        /** rst_check_pos_err_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit to sign that RST header marker position mismatches with
         *  restart interval when decoding.
         */
        uint32_t rst_check_pos_err_int_raw:1;
        /** out_eof_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt bit turns to high level when the last pixel of one square has
         *  been transmitted for Tx channel.
         */
        uint32_t out_eof_int_raw:1;
        /** sr_color_mode_err_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw interrupt bit to sign that the selected source color mode is not supported.
         */
        uint32_t sr_color_mode_err_int_raw:1;
        /** dct_done_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw interrupt bit to sign that one dct calculation is finished.
         */
        uint32_t dct_done_int_raw:1;
        /** bs_last_block_eof_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw interrupt bit to sign that the coding process for last block is finished.
         */
        uint32_t bs_last_block_eof_int_raw:1;
        /** scan_check_none_err_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw interrupt bit to sign that SOS header marker is not detected but there are
         *  still components left to be decoded.
         */
        uint32_t scan_check_none_err_int_raw:1;
        /** scan_check_pos_err_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The raw interrupt bit to sign that SOS header marker position wrong when decoding.
         */
        uint32_t scan_check_pos_err_int_raw:1;
        /** uxp_det_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  The raw interrupt bit to sign that unsupported header marker is detected when
         *  decoding.
         */
        uint32_t uxp_det_int_raw:1;
        /** en_frame_eof_err_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  The raw interrupt bit to sign that received pixel blocks are smaller than expected
         *  when encoding.
         */
        uint32_t en_frame_eof_err_int_raw:1;
        /** en_frame_eof_lack_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  The raw interrupt bit to sign that the frame eof sign bit from dma input is missing
         *  when encoding. But the number of pixel blocks is enough.
         */
        uint32_t en_frame_eof_lack_int_raw:1;
        /** de_frame_eof_err_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  The raw interrupt bit to sign that decoded blocks are smaller than expected when
         *  decoding.
         */
        uint32_t de_frame_eof_err_int_raw:1;
        /** de_frame_eof_lack_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  The raw interrupt bit to sign that the either frame eof from dma input or eoi
         *  marker is missing when encoding. But the number of decoded blocks is enough.
         */
        uint32_t de_frame_eof_lack_int_raw:1;
        /** sos_unmatch_err_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  The raw interrupt bit to sign that the component number of a scan is 0 or does not
         *  match the sos marker's length when decoding.
         */
        uint32_t sos_unmatch_err_int_raw:1;
        /** marker_err_fst_scan_int_raw : R/WTC/SS; bitpos: [21]; default: 0;
         *  The raw interrupt bit to sign that the first scan has header marker error when
         *  decoding.
         */
        uint32_t marker_err_fst_scan_int_raw:1;
        /** marker_err_other_scan_int_raw : R/WTC/SS; bitpos: [22]; default: 0;
         *  The raw interrupt bit to sign that the following scans but not the first scan have
         *  header marker error when decoding.
         */
        uint32_t marker_err_other_scan_int_raw:1;
        /** undet_int_raw : R/WTC/SS; bitpos: [23]; default: 0;
         *  The raw interrupt bit to sign that JPEG format is not detected at the eof data of a
         *  packet when decoding.
         */
        uint32_t undet_int_raw:1;
        /** decode_timeout_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  The raw interrupt bit to sign that decode pause time is longer than the setting
         *  decode timeout time when decoding.
         */
        uint32_t decode_timeout_int_raw:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} jpeg_int_raw_reg_t;

/** Type of int_ena register
 *  Interrupt enable registers
 */
typedef union {
    struct {
        /** done_int_ena : R/W; bitpos: [0]; default: 0;
         *  This enable interrupt bit turns to high level when JPEG finishes encoding a
         *  picture..
         */
        uint32_t done_int_ena:1;
        /** rle_parallel_err_int_ena : R/W; bitpos: [1]; default: 0;
         *  The enable interrupt bit to sign that rle parallel error when decoding.
         */
        uint32_t rle_parallel_err_int_ena:1;
        /** cid_err_int_ena : R/W; bitpos: [2]; default: 0;
         *  The enable interrupt bit to sign that scan id check with component fails when
         *  decoding.
         */
        uint32_t cid_err_int_ena:1;
        /** c_dht_dc_id_err_int_ena : R/W; bitpos: [3]; default: 0;
         *  The enable interrupt bit to sign that scan component's dc dht id check with dc dht
         *  table's id fails when decoding.
         */
        uint32_t c_dht_dc_id_err_int_ena:1;
        /** c_dht_ac_id_err_int_ena : R/W; bitpos: [4]; default: 0;
         *  The enable interrupt bit to sign that scan component's ac dht id check with ac dht
         *  table's id fails when decoding.
         */
        uint32_t c_dht_ac_id_err_int_ena:1;
        /** c_dqt_id_err_int_ena : R/W; bitpos: [5]; default: 0;
         *  The enable interrupt bit to sign that scan component's dqt id check with dqt
         *  table's id fails when decoding.
         */
        uint32_t c_dqt_id_err_int_ena:1;
        /** rst_uxp_err_int_ena : R/W; bitpos: [6]; default: 0;
         *  The enable interrupt bit to sign that RST header marker is detected but restart
         *  interval is 0 when decoding.
         */
        uint32_t rst_uxp_err_int_ena:1;
        /** rst_check_none_err_int_ena : R/W; bitpos: [7]; default: 0;
         *  The enable interrupt bit to sign that RST header marker is not detected but restart
         *  interval is not 0 when decoding.
         */
        uint32_t rst_check_none_err_int_ena:1;
        /** rst_check_pos_err_int_ena : R/W; bitpos: [8]; default: 0;
         *  The enable interrupt bit to sign that RST header marker position mismatches with
         *  restart interval when decoding.
         */
        uint32_t rst_check_pos_err_int_ena:1;
        /** out_eof_int_ena : R/W; bitpos: [9]; default: 0;
         *  The enable interrupt bit turns to high level when the last pixel of one square has
         *  been transmitted for Tx channel.
         */
        uint32_t out_eof_int_ena:1;
        /** sr_color_mode_err_int_ena : R/W; bitpos: [10]; default: 0;
         *  The enable interrupt bit to sign that the selected source color mode is not
         *  supported.
         */
        uint32_t sr_color_mode_err_int_ena:1;
        /** dct_done_int_ena : R/W; bitpos: [11]; default: 0;
         *  The enable interrupt bit to sign that one dct calculation is finished.
         */
        uint32_t dct_done_int_ena:1;
        /** bs_last_block_eof_int_ena : R/W; bitpos: [12]; default: 0;
         *  The enable interrupt bit to sign that the coding process for last block is finished.
         */
        uint32_t bs_last_block_eof_int_ena:1;
        /** scan_check_none_err_int_ena : R/W; bitpos: [13]; default: 0;
         *  The enable interrupt bit to sign that SOS header marker is not detected but there
         *  are still components left to be decoded.
         */
        uint32_t scan_check_none_err_int_ena:1;
        /** scan_check_pos_err_int_ena : R/W; bitpos: [14]; default: 0;
         *  The enable interrupt bit to sign that SOS header marker position wrong when
         *  decoding.
         */
        uint32_t scan_check_pos_err_int_ena:1;
        /** uxp_det_int_ena : R/W; bitpos: [15]; default: 0;
         *  The enable interrupt bit to sign that unsupported header marker is detected when
         *  decoding.
         */
        uint32_t uxp_det_int_ena:1;
        /** en_frame_eof_err_int_ena : R/W; bitpos: [16]; default: 0;
         *  The enable interrupt bit to sign that received pixel blocks are smaller than
         *  expected when encoding.
         */
        uint32_t en_frame_eof_err_int_ena:1;
        /** en_frame_eof_lack_int_ena : R/W; bitpos: [17]; default: 0;
         *  The enable interrupt bit to sign that the frame eof sign bit from dma input is
         *  missing when encoding. But the number of pixel blocks is enough.
         */
        uint32_t en_frame_eof_lack_int_ena:1;
        /** de_frame_eof_err_int_ena : R/W; bitpos: [18]; default: 0;
         *  The enable interrupt bit to sign that decoded blocks are smaller than expected when
         *  decoding.
         */
        uint32_t de_frame_eof_err_int_ena:1;
        /** de_frame_eof_lack_int_ena : R/W; bitpos: [19]; default: 0;
         *  The enable interrupt bit to sign that the either frame eof from dma input or eoi
         *  marker is missing when encoding. But the number of decoded blocks is enough.
         */
        uint32_t de_frame_eof_lack_int_ena:1;
        /** sos_unmatch_err_int_ena : R/W; bitpos: [20]; default: 0;
         *  The enable interrupt bit to sign that the component number of a scan is 0 or does
         *  not match the sos marker's length when decoding.
         */
        uint32_t sos_unmatch_err_int_ena:1;
        /** marker_err_fst_scan_int_ena : R/W; bitpos: [21]; default: 0;
         *  The enable interrupt bit to sign that the first scan has header marker error when
         *  decoding.
         */
        uint32_t marker_err_fst_scan_int_ena:1;
        /** marker_err_other_scan_int_ena : R/W; bitpos: [22]; default: 0;
         *  The enable interrupt bit to sign that the following scans but not the first scan
         *  have header marker error when decoding.
         */
        uint32_t marker_err_other_scan_int_ena:1;
        /** undet_int_ena : R/W; bitpos: [23]; default: 0;
         *  The enable interrupt bit to sign that JPEG format is not detected at the eof data
         *  of a packet when decoding.
         */
        uint32_t undet_int_ena:1;
        /** decode_timeout_int_ena : R/W; bitpos: [24]; default: 0;
         *  The enable interrupt bit to sign that decode pause time is longer than the setting
         *  decode timeout time when decoding.
         */
        uint32_t decode_timeout_int_ena:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} jpeg_int_ena_reg_t;

/** Type of int_st register
 *  Interrupt status registers
 */
typedef union {
    struct {
        /** done_int_st : RO; bitpos: [0]; default: 0;
         *  This status interrupt bit turns to high level when JPEG finishes encoding a
         *  picture..
         */
        uint32_t done_int_st:1;
        /** rle_parallel_err_int_st : RO; bitpos: [1]; default: 0;
         *  The status interrupt bit to sign that rle parallel error when decoding.
         */
        uint32_t rle_parallel_err_int_st:1;
        /** cid_err_int_st : RO; bitpos: [2]; default: 0;
         *  The status interrupt bit to sign that scan id check with component fails when
         *  decoding.
         */
        uint32_t cid_err_int_st:1;
        /** c_dht_dc_id_err_int_st : RO; bitpos: [3]; default: 0;
         *  The status interrupt bit to sign that scan component's dc dht id check with dc dht
         *  table's id fails when decoding.
         */
        uint32_t c_dht_dc_id_err_int_st:1;
        /** c_dht_ac_id_err_int_st : RO; bitpos: [4]; default: 0;
         *  The status interrupt bit to sign that scan component's ac dht id check with ac dht
         *  table's id fails when decoding.
         */
        uint32_t c_dht_ac_id_err_int_st:1;
        /** c_dqt_id_err_int_st : RO; bitpos: [5]; default: 0;
         *  The status interrupt bit to sign that scan component's dqt id check with dqt
         *  table's id fails when decoding.
         */
        uint32_t c_dqt_id_err_int_st:1;
        /** rst_uxp_err_int_st : RO; bitpos: [6]; default: 0;
         *  The status interrupt bit to sign that RST header marker is detected but restart
         *  interval is 0 when decoding.
         */
        uint32_t rst_uxp_err_int_st:1;
        /** rst_check_none_err_int_st : RO; bitpos: [7]; default: 0;
         *  The status interrupt bit to sign that RST header marker is not detected but restart
         *  interval is not 0 when decoding.
         */
        uint32_t rst_check_none_err_int_st:1;
        /** rst_check_pos_err_int_st : RO; bitpos: [8]; default: 0;
         *  The status interrupt bit to sign that RST header marker position mismatches with
         *  restart interval when decoding.
         */
        uint32_t rst_check_pos_err_int_st:1;
        /** out_eof_int_st : RO; bitpos: [9]; default: 0;
         *  The status interrupt bit turns to high level when the last pixel of one square has
         *  been transmitted for Tx channel.
         */
        uint32_t out_eof_int_st:1;
        /** sr_color_mode_err_int_st : RO; bitpos: [10]; default: 0;
         *  The status interrupt bit to sign that the selected source color mode is not
         *  supported.
         */
        uint32_t sr_color_mode_err_int_st:1;
        /** dct_done_int_st : RO; bitpos: [11]; default: 0;
         *  The status interrupt bit to sign that one dct calculation is finished.
         */
        uint32_t dct_done_int_st:1;
        /** bs_last_block_eof_int_st : RO; bitpos: [12]; default: 0;
         *  The status interrupt bit to sign that the coding process for last block is finished.
         */
        uint32_t bs_last_block_eof_int_st:1;
        /** scan_check_none_err_int_st : RO; bitpos: [13]; default: 0;
         *  The status interrupt bit to sign that SOS header marker is not detected but there
         *  are still components left to be decoded.
         */
        uint32_t scan_check_none_err_int_st:1;
        /** scan_check_pos_err_int_st : RO; bitpos: [14]; default: 0;
         *  The status interrupt bit to sign that SOS header marker position wrong when
         *  decoding.
         */
        uint32_t scan_check_pos_err_int_st:1;
        /** uxp_det_int_st : RO; bitpos: [15]; default: 0;
         *  The status interrupt bit to sign that unsupported header marker is detected when
         *  decoding.
         */
        uint32_t uxp_det_int_st:1;
        /** en_frame_eof_err_int_st : RO; bitpos: [16]; default: 0;
         *  The status interrupt bit to sign that received pixel blocks are smaller than
         *  expected when encoding.
         */
        uint32_t en_frame_eof_err_int_st:1;
        /** en_frame_eof_lack_int_st : RO; bitpos: [17]; default: 0;
         *  The status interrupt bit to sign that the frame eof sign bit from dma input is
         *  missing when encoding. But the number of pixel blocks is enough.
         */
        uint32_t en_frame_eof_lack_int_st:1;
        /** de_frame_eof_err_int_st : RO; bitpos: [18]; default: 0;
         *  The status interrupt bit to sign that decoded blocks are smaller than expected when
         *  decoding.
         */
        uint32_t de_frame_eof_err_int_st:1;
        /** de_frame_eof_lack_int_st : RO; bitpos: [19]; default: 0;
         *  The status interrupt bit to sign that the either frame eof from dma input or eoi
         *  marker is missing when encoding. But the number of decoded blocks is enough.
         */
        uint32_t de_frame_eof_lack_int_st:1;
        /** sos_unmatch_err_int_st : RO; bitpos: [20]; default: 0;
         *  The status interrupt bit to sign that the component number of a scan is 0 or does
         *  not match the sos marker's length when decoding.
         */
        uint32_t sos_unmatch_err_int_st:1;
        /** marker_err_fst_scan_int_st : RO; bitpos: [21]; default: 0;
         *  The status interrupt bit to sign that the first scan has header marker error when
         *  decoding.
         */
        uint32_t marker_err_fst_scan_int_st:1;
        /** marker_err_other_scan_int_st : RO; bitpos: [22]; default: 0;
         *  The status interrupt bit to sign that the following scans but not the first scan
         *  have header marker error when decoding.
         */
        uint32_t marker_err_other_scan_int_st:1;
        /** undet_int_st : RO; bitpos: [23]; default: 0;
         *  The status interrupt bit to sign that JPEG format is not detected at the eof data
         *  of a packet when decoding.
         */
        uint32_t undet_int_st:1;
        /** decode_timeout_int_st : RO; bitpos: [24]; default: 0;
         *  The status interrupt bit to sign that decode pause time is longer than the setting
         *  decode timeout time when decoding.
         */
        uint32_t decode_timeout_int_st:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} jpeg_int_st_reg_t;

/** Type of int_clr register
 *  Interrupt clear registers
 */
typedef union {
    struct {
        /** done_int_clr : WT; bitpos: [0]; default: 0;
         *  This clear interrupt bit turns to high level when JPEG finishes encoding a picture..
         */
        uint32_t done_int_clr:1;
        /** rle_parallel_err_int_clr : WT; bitpos: [1]; default: 0;
         *  The clear interrupt bit to sign that rle parallel error when decoding.
         */
        uint32_t rle_parallel_err_int_clr:1;
        /** cid_err_int_clr : WT; bitpos: [2]; default: 0;
         *  The clear interrupt bit to sign that scan id check with component fails when
         *  decoding.
         */
        uint32_t cid_err_int_clr:1;
        /** c_dht_dc_id_err_int_clr : WT; bitpos: [3]; default: 0;
         *  The clear interrupt bit to sign that scan component's dc dht id check with dc dht
         *  table's id fails when decoding.
         */
        uint32_t c_dht_dc_id_err_int_clr:1;
        /** c_dht_ac_id_err_int_clr : WT; bitpos: [4]; default: 0;
         *  The clear interrupt bit to sign that scan component's ac dht id check with ac dht
         *  table's id fails when decoding.
         */
        uint32_t c_dht_ac_id_err_int_clr:1;
        /** c_dqt_id_err_int_clr : WT; bitpos: [5]; default: 0;
         *  The clear interrupt bit to sign that scan component's dqt id check with dqt table's
         *  id fails when decoding.
         */
        uint32_t c_dqt_id_err_int_clr:1;
        /** rst_uxp_err_int_clr : WT; bitpos: [6]; default: 0;
         *  The clear interrupt bit to sign that RST header marker is detected but restart
         *  interval is 0 when decoding.
         */
        uint32_t rst_uxp_err_int_clr:1;
        /** rst_check_none_err_int_clr : WT; bitpos: [7]; default: 0;
         *  The clear interrupt bit to sign that RST header marker is not detected but restart
         *  interval is not 0 when decoding.
         */
        uint32_t rst_check_none_err_int_clr:1;
        /** rst_check_pos_err_int_clr : WT; bitpos: [8]; default: 0;
         *  The clear interrupt bit to sign that RST header marker position mismatches with
         *  restart interval when decoding.
         */
        uint32_t rst_check_pos_err_int_clr:1;
        /** out_eof_int_clr : WT; bitpos: [9]; default: 0;
         *  The clear interrupt bit turns to high level when the last pixel of one square has
         *  been transmitted for Tx channel.
         */
        uint32_t out_eof_int_clr:1;
        /** sr_color_mode_err_int_clr : WT; bitpos: [10]; default: 0;
         *  The clear interrupt bit to sign that the selected source color mode is not
         *  supported.
         */
        uint32_t sr_color_mode_err_int_clr:1;
        /** dct_done_int_clr : WT; bitpos: [11]; default: 0;
         *  The clear interrupt bit to sign that one dct calculation is finished.
         */
        uint32_t dct_done_int_clr:1;
        /** bs_last_block_eof_int_clr : WT; bitpos: [12]; default: 0;
         *  The clear interrupt bit to sign that the coding process for last block is finished.
         */
        uint32_t bs_last_block_eof_int_clr:1;
        /** scan_check_none_err_int_clr : WT; bitpos: [13]; default: 0;
         *  The clear interrupt bit to sign that SOS header marker is not detected but there
         *  are still components left to be decoded.
         */
        uint32_t scan_check_none_err_int_clr:1;
        /** scan_check_pos_err_int_clr : WT; bitpos: [14]; default: 0;
         *  The clear interrupt bit to sign that SOS header marker position wrong when decoding.
         */
        uint32_t scan_check_pos_err_int_clr:1;
        /** uxp_det_int_clr : WT; bitpos: [15]; default: 0;
         *  The clear interrupt bit to sign that unsupported header marker is detected when
         *  decoding.
         */
        uint32_t uxp_det_int_clr:1;
        /** en_frame_eof_err_int_clr : WT; bitpos: [16]; default: 0;
         *  The clear interrupt bit to sign that received pixel blocks are smaller than
         *  expected when encoding.
         */
        uint32_t en_frame_eof_err_int_clr:1;
        /** en_frame_eof_lack_int_clr : WT; bitpos: [17]; default: 0;
         *  The clear interrupt bit to sign that the frame eof sign bit from dma input is
         *  missing when encoding. But the number of pixel blocks is enough.
         */
        uint32_t en_frame_eof_lack_int_clr:1;
        /** de_frame_eof_err_int_clr : WT; bitpos: [18]; default: 0;
         *  The clear interrupt bit to sign that decoded blocks are smaller than expected when
         *  decoding.
         */
        uint32_t de_frame_eof_err_int_clr:1;
        /** de_frame_eof_lack_int_clr : WT; bitpos: [19]; default: 0;
         *  The clear interrupt bit to sign that the either frame eof from dma input or eoi
         *  marker is missing when encoding. But the number of decoded blocks is enough.
         */
        uint32_t de_frame_eof_lack_int_clr:1;
        /** sos_unmatch_err_int_clr : WT; bitpos: [20]; default: 0;
         *  The clear interrupt bit to sign that the component number of a scan is 0 or does
         *  not match the sos marker's length when decoding.
         */
        uint32_t sos_unmatch_err_int_clr:1;
        /** marker_err_fst_scan_int_clr : WT; bitpos: [21]; default: 0;
         *  The clear interrupt bit to sign that the first scan has header marker error when
         *  decoding.
         */
        uint32_t marker_err_fst_scan_int_clr:1;
        /** marker_err_other_scan_int_clr : WT; bitpos: [22]; default: 0;
         *  The clear interrupt bit to sign that the following scans but not the first scan
         *  have header marker error when decoding.
         */
        uint32_t marker_err_other_scan_int_clr:1;
        /** undet_int_clr : WT; bitpos: [23]; default: 0;
         *  The clear interrupt bit to sign that JPEG format is not detected at the eof data of
         *  a packet when decoding.
         */
        uint32_t undet_int_clr:1;
        /** decode_timeout_int_clr : WT; bitpos: [24]; default: 0;
         *  The clear interrupt bit to sign that decode pause time is longer than the setting
         *  decode timeout time when decoding.
         */
        uint32_t decode_timeout_int_clr:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} jpeg_int_clr_reg_t;


/** Group: Trace and Debug registers */
/** Type of status0 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        uint32_t reserved_0:11;
        /** bitstream_eof_vld_cnt : RO; bitpos: [16:11]; default: 0;
         *  the valid bit count for last bitstream
         */
        uint32_t bitstream_eof_vld_cnt:6;
        /** dctout_zzscan_addr : RO; bitpos: [22:17]; default: 0;
         *  the zig-zag read addr from dctout_ram
         */
        uint32_t dctout_zzscan_addr:6;
        /** qnrval_zzscan_addr : RO; bitpos: [28:23]; default: 0;
         *  the zig-zag read addr from qnrval_ram
         */
        uint32_t qnrval_zzscan_addr:6;
        /** reg_state_yuv : RO; bitpos: [31:29]; default: 0;
         *  the state of jpeg fsm
         */
        uint32_t reg_state_yuv:3;
    };
    uint32_t val;
} jpeg_status0_reg_t;

/** Type of status2 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** source_pixel : RO; bitpos: [23:0]; default: 0;
         *  source pixels fetched from dma
         */
        uint32_t source_pixel:24;
        /** last_block : RO; bitpos: [24]; default: 0;
         *  indicate the encoding process for the last mcu of the picture
         */
        uint32_t last_block:1;
        /** last_mcu : RO; bitpos: [25]; default: 0;
         *  indicate the encoding process for the last block of the picture
         */
        uint32_t last_mcu:1;
        /** last_dc : RO; bitpos: [26]; default: 0;
         *  indicate the encoding process is at the header of the last block of the picture
         */
        uint32_t last_dc:1;
        /** packfifo_ready : RO; bitpos: [27]; default: 1;
         *  the jpeg pack_fifo ready signal, high active
         */
        uint32_t packfifo_ready:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} jpeg_status2_reg_t;

/** Type of status3 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** yo : RO; bitpos: [8:0]; default: 0;
         *  component y transferred from rgb input
         */
        uint32_t yo:9;
        /** y_ready : RO; bitpos: [9]; default: 0;
         *  component y valid signal, high active
         */
        uint32_t y_ready:1;
        /** cbo : RO; bitpos: [18:10]; default: 0;
         *  component cb transferred from rgb input
         */
        uint32_t cbo:9;
        /** cb_ready : RO; bitpos: [19]; default: 0;
         *  component cb valid signal, high active
         */
        uint32_t cb_ready:1;
        /** cro : RO; bitpos: [28:20]; default: 0;
         *  component cr transferred from rgb input
         */
        uint32_t cro:9;
        /** cr_ready : RO; bitpos: [29]; default: 0;
         *  component cr valid signal, high active
         */
        uint32_t cr_ready:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} jpeg_status3_reg_t;

/** Type of status4 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** hfm_bitstream : RO; bitpos: [31:0]; default: 0;
         *  the hufman bitstream during encoding process
         */
        uint32_t hfm_bitstream:32;
    };
    uint32_t val;
} jpeg_status4_reg_t;

/** Type of dht_totlen_dc0 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_totlen_dc0 : HRO; bitpos: [31:0]; default: 0;
         *  write the numbers of 1~n codeword length sum from 1~16 of dc0 table
         */
        uint32_t dht_totlen_dc0:32;
    };
    uint32_t val;
} jpeg_dht_totlen_dc0_reg_t;

/** Type of dht_val_dc0 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_val_dc0 : HRO; bitpos: [31:0]; default: 0;
         *  write codeword corresponding huffman values of dc0 table
         */
        uint32_t dht_val_dc0:32;
    };
    uint32_t val;
} jpeg_dht_val_dc0_reg_t;

/** Type of dht_totlen_ac0 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_totlen_ac0 : HRO; bitpos: [31:0]; default: 0;
         *  write the numbers of 1~n codeword length sum from 1~16 of ac0 table
         */
        uint32_t dht_totlen_ac0:32;
    };
    uint32_t val;
} jpeg_dht_totlen_ac0_reg_t;

/** Type of dht_val_ac0 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_val_ac0 : HRO; bitpos: [31:0]; default: 0;
         *  write codeword corresponding huffman values of ac0 table
         */
        uint32_t dht_val_ac0:32;
    };
    uint32_t val;
} jpeg_dht_val_ac0_reg_t;

/** Type of dht_totlen_dc1 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_totlen_dc1 : HRO; bitpos: [31:0]; default: 0;
         *  write the numbers of 1~n codeword length sum from 1~16 of dc1 table
         */
        uint32_t dht_totlen_dc1:32;
    };
    uint32_t val;
} jpeg_dht_totlen_dc1_reg_t;

/** Type of dht_val_dc1 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_val_dc1 : HRO; bitpos: [31:0]; default: 0;
         *  write codeword corresponding huffman values of dc1 table
         */
        uint32_t dht_val_dc1:32;
    };
    uint32_t val;
} jpeg_dht_val_dc1_reg_t;

/** Type of dht_totlen_ac1 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_totlen_ac1 : HRO; bitpos: [31:0]; default: 0;
         *  write the numbers of 1~n codeword length sum from 1~16 of ac1 table
         */
        uint32_t dht_totlen_ac1:32;
    };
    uint32_t val;
} jpeg_dht_totlen_ac1_reg_t;

/** Type of dht_val_ac1 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_val_ac1 : HRO; bitpos: [31:0]; default: 0;
         *  write codeword corresponding huffman values of ac1 table
         */
        uint32_t dht_val_ac1:32;
    };
    uint32_t val;
} jpeg_dht_val_ac1_reg_t;

/** Type of dht_codemin_dc0 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_codemin_dc0 : HRO; bitpos: [31:0]; default: 0;
         *  write the minimum codeword of  code length from 1~16 of dc0 table. The codeword is
         *  left shifted to the MSB position of a 16bit word
         */
        uint32_t dht_codemin_dc0:32;
    };
    uint32_t val;
} jpeg_dht_codemin_dc0_reg_t;

/** Type of dht_codemin_ac0 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_codemin_ac0 : HRO; bitpos: [31:0]; default: 0;
         *  write the minimum codeword of  code length from 1~16 of ac0 table. The codeword is
         *  left shifted to the MSB position of a 16bit word
         */
        uint32_t dht_codemin_ac0:32;
    };
    uint32_t val;
} jpeg_dht_codemin_ac0_reg_t;

/** Type of dht_codemin_dc1 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_codemin_dc1 : HRO; bitpos: [31:0]; default: 0;
         *  write the minimum codeword of  code length from 1~16 of dc1 table. The codeword is
         *  left shifted to the MSB position of a 16bit word
         */
        uint32_t dht_codemin_dc1:32;
    };
    uint32_t val;
} jpeg_dht_codemin_dc1_reg_t;

/** Type of dht_codemin_ac1 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** dht_codemin_ac1 : HRO; bitpos: [31:0]; default: 0;
         *  write the minimum codeword of  code length from 1~16 of ac1 table. The codeword is
         *  left shifted to the MSB position of a 16bit word
         */
        uint32_t dht_codemin_ac1:32;
    };
    uint32_t val;
} jpeg_dht_codemin_ac1_reg_t;

/** Type of decoder_status0 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** decode_byte_cnt : RO; bitpos: [25:0]; default: 0;
         *  Reserved
         */
        uint32_t decode_byte_cnt:26;
        /** header_dec_st : RO; bitpos: [29:26]; default: 0;
         *  Reserved
         */
        uint32_t header_dec_st:4;
        /** decode_sample_sel : RO; bitpos: [31:30]; default: 0;
         *  Reserved
         */
        uint32_t decode_sample_sel:2;
    };
    uint32_t val;
} jpeg_decoder_status0_reg_t;

/** Type of decoder_status1 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** encode_data : RO; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t encode_data:16;
        /** count_q : RO; bitpos: [22:16]; default: 0;
         *  Reserved
         */
        uint32_t count_q:7;
        /** mcu_fsm_ready : RO; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t mcu_fsm_ready:1;
        /** decode_data : RO; bitpos: [31:24]; default: 0;
         *  Reserved
         */
        uint32_t decode_data:8;
    };
    uint32_t val;
} jpeg_decoder_status1_reg_t;

/** Type of decoder_status2 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** comp_block_num : RO; bitpos: [25:0]; default: 0;
         *  Reserved
         */
        uint32_t comp_block_num:26;
        /** scan_num : RO; bitpos: [28:26]; default: 0;
         *  Reserved
         */
        uint32_t scan_num:3;
        /** rst_check_wait : RO; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t rst_check_wait:1;
        /** scan_check_wait : RO; bitpos: [30]; default: 0;
         *  Reserved
         */
        uint32_t scan_check_wait:1;
        /** mcu_in_proc : RO; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t mcu_in_proc:1;
    };
    uint32_t val;
} jpeg_decoder_status2_reg_t;

/** Type of decoder_status3 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** lookup_data : RO; bitpos: [31:0]; default: 0;
         *  Reserved
         */
        uint32_t lookup_data:32;
    };
    uint32_t val;
} jpeg_decoder_status3_reg_t;

/** Type of decoder_status4 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** block_eof_cnt : RO; bitpos: [25:0]; default: 0;
         *  Reserved
         */
        uint32_t block_eof_cnt:26;
        /** dezigzag_ready : RO; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t dezigzag_ready:1;
        /** de_frame_eof_check : RO; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t de_frame_eof_check:1;
        /** de_dma2d_in_push : RO; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t de_dma2d_in_push:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} jpeg_decoder_status4_reg_t;

/** Type of decoder_status5 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** idct_hfm_data : RO; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t idct_hfm_data:16;
        /** ns0 : RO; bitpos: [18:16]; default: 0;
         *  Reserved
         */
        uint32_t ns0:3;
        /** ns1 : RO; bitpos: [21:19]; default: 0;
         *  Reserved
         */
        uint32_t ns1:3;
        /** ns2 : RO; bitpos: [24:22]; default: 0;
         *  Reserved
         */
        uint32_t ns2:3;
        /** ns3 : RO; bitpos: [27:25]; default: 0;
         *  Reserved
         */
        uint32_t ns3:3;
        /** data_last_o : RO; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t data_last_o:1;
        /** rdn_result : RO; bitpos: [29]; default: 0;
         *  redundant registers for jpeg
         */
        uint32_t rdn_result:1;
        /** rdn_ena : R/W; bitpos: [30]; default: 0;
         *  redundant control registers for jpeg
         */
        uint32_t rdn_ena:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} jpeg_decoder_status5_reg_t;

/** Type of status5 register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** pic_block_num : RO; bitpos: [23:0]; default: 0;
         *  Reserved
         */
        uint32_t pic_block_num:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} jpeg_status5_reg_t;

/** Type of eco_low register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  redundant registers for jpeg
         */
        uint32_t rdn_eco_low:32;
    };
    uint32_t val;
} jpeg_eco_low_reg_t;

/** Type of eco_high register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  redundant registers for jpeg
         */
        uint32_t rdn_eco_high:32;
    };
    uint32_t val;
} jpeg_eco_high_reg_t;

/** Type of sys register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} jpeg_sys_reg_t;

/** Type of version register
 *  Trace and Debug registers
 */
typedef union {
    struct {
        /** jpeg_ver : R/W; bitpos: [27:0]; default: 34673040;
         *  Reserved
         */
        uint32_t jpeg_ver:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} jpeg_version_reg_t;


typedef struct jpeg_dev_t {
    volatile jpeg_config_reg_t config;
    volatile jpeg_dqt_info_reg_t dqt_info;
    volatile jpeg_pic_size_reg_t pic_size;
    uint32_t reserved_00c;
    volatile jpeg_t0qnr_reg_t t0qnr;
    volatile jpeg_t1qnr_reg_t t1qnr;
    volatile jpeg_t2qnr_reg_t t2qnr;
    volatile jpeg_t3qnr_reg_t t3qnr;
    volatile jpeg_decode_conf_reg_t decode_conf;
    volatile jpeg_c0_reg_t c0;
    volatile jpeg_c1_reg_t c1;
    volatile jpeg_c2_reg_t c2;
    volatile jpeg_c3_reg_t c3;
    volatile jpeg_dht_info_reg_t dht_info;
    volatile jpeg_int_raw_reg_t int_raw;
    volatile jpeg_int_ena_reg_t int_ena;
    volatile jpeg_int_st_reg_t int_st;
    volatile jpeg_int_clr_reg_t int_clr;
    volatile jpeg_status0_reg_t status0;
    volatile jpeg_status2_reg_t status2;
    volatile jpeg_status3_reg_t status3;
    volatile jpeg_status4_reg_t status4;
    volatile jpeg_dht_totlen_dc0_reg_t dht_totlen_dc0;
    volatile jpeg_dht_val_dc0_reg_t dht_val_dc0;
    volatile jpeg_dht_totlen_ac0_reg_t dht_totlen_ac0;
    volatile jpeg_dht_val_ac0_reg_t dht_val_ac0;
    volatile jpeg_dht_totlen_dc1_reg_t dht_totlen_dc1;
    volatile jpeg_dht_val_dc1_reg_t dht_val_dc1;
    volatile jpeg_dht_totlen_ac1_reg_t dht_totlen_ac1;
    volatile jpeg_dht_val_ac1_reg_t dht_val_ac1;
    volatile jpeg_dht_codemin_dc0_reg_t dht_codemin_dc0;
    volatile jpeg_dht_codemin_ac0_reg_t dht_codemin_ac0;
    volatile jpeg_dht_codemin_dc1_reg_t dht_codemin_dc1;
    volatile jpeg_dht_codemin_ac1_reg_t dht_codemin_ac1;
    volatile jpeg_decoder_status0_reg_t decoder_status0;
    volatile jpeg_decoder_status1_reg_t decoder_status1;
    volatile jpeg_decoder_status2_reg_t decoder_status2;
    volatile jpeg_decoder_status3_reg_t decoder_status3;
    volatile jpeg_decoder_status4_reg_t decoder_status4;
    volatile jpeg_decoder_status5_reg_t decoder_status5;
    volatile jpeg_status5_reg_t status5;
    volatile jpeg_eco_low_reg_t eco_low;
    volatile jpeg_eco_high_reg_t eco_high;
    uint32_t reserved_0ac[19];
    volatile jpeg_sys_reg_t sys;
    volatile jpeg_version_reg_t version;
} jpeg_dev_t;

extern jpeg_dev_t JPEG;

#ifndef __cplusplus
_Static_assert(sizeof(jpeg_dev_t) == 0x100, "Invalid size of jpeg_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
