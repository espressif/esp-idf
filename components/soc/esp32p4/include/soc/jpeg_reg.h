/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** JPEG_CONFIG_REG register
 *  Control and configuration registers
 */
#define JPEG_CONFIG_REG (DR_REG_JPEG_BASE + 0x0)
/** JPEG_FSM_RST : WT; bitpos: [0]; default: 0;
 *  fsm reset
 */
#define JPEG_FSM_RST    (BIT(0))
#define JPEG_FSM_RST_M  (JPEG_FSM_RST_V << JPEG_FSM_RST_S)
#define JPEG_FSM_RST_V  0x00000001U
#define JPEG_FSM_RST_S  0
/** JPEG_JPEG_START : WT; bitpos: [1]; default: 0;
 *  start to compress a new pic(in dma reg mode)
 */
#define JPEG_JPEG_START    (BIT(1))
#define JPEG_JPEG_START_M  (JPEG_JPEG_START_V << JPEG_JPEG_START_S)
#define JPEG_JPEG_START_V  0x00000001U
#define JPEG_JPEG_START_S  1
/** JPEG_QNR_PRESITION : R/W; bitpos: [2]; default: 0;
 *  0:8bit qnr,1:12bit qnr(TBD)
 */
#define JPEG_QNR_PRESITION    (BIT(2))
#define JPEG_QNR_PRESITION_M  (JPEG_QNR_PRESITION_V << JPEG_QNR_PRESITION_S)
#define JPEG_QNR_PRESITION_V  0x00000001U
#define JPEG_QNR_PRESITION_S  2
/** JPEG_FF_CHECK_EN : R/W; bitpos: [3]; default: 1;
 *  enable whether to add "00" after "ff"
 */
#define JPEG_FF_CHECK_EN    (BIT(3))
#define JPEG_FF_CHECK_EN_M  (JPEG_FF_CHECK_EN_V << JPEG_FF_CHECK_EN_S)
#define JPEG_FF_CHECK_EN_V  0x00000001U
#define JPEG_FF_CHECK_EN_S  3
/** JPEG_SAMPLE_SEL : R/W; bitpos: [5:4]; default: 1;
 *  0:yuv444,1:yuv422, 2:yuv420
 */
#define JPEG_SAMPLE_SEL    0x00000003U
#define JPEG_SAMPLE_SEL_M  (JPEG_SAMPLE_SEL_V << JPEG_SAMPLE_SEL_S)
#define JPEG_SAMPLE_SEL_V  0x00000003U
#define JPEG_SAMPLE_SEL_S  4
/** JPEG_DMA_LINKLIST_MODE : RO; bitpos: [6]; default: 1;
 *  1:use linklist to configure dma
 */
#define JPEG_DMA_LINKLIST_MODE    (BIT(6))
#define JPEG_DMA_LINKLIST_MODE_M  (JPEG_DMA_LINKLIST_MODE_V << JPEG_DMA_LINKLIST_MODE_S)
#define JPEG_DMA_LINKLIST_MODE_V  0x00000001U
#define JPEG_DMA_LINKLIST_MODE_S  6
/** JPEG_DEBUG_DIRECT_OUT_EN : R/W; bitpos: [7]; default: 0;
 *  0:normal mode,1:debug mode for direct output from input
 */
#define JPEG_DEBUG_DIRECT_OUT_EN    (BIT(7))
#define JPEG_DEBUG_DIRECT_OUT_EN_M  (JPEG_DEBUG_DIRECT_OUT_EN_V << JPEG_DEBUG_DIRECT_OUT_EN_S)
#define JPEG_DEBUG_DIRECT_OUT_EN_V  0x00000001U
#define JPEG_DEBUG_DIRECT_OUT_EN_S  7
/** JPEG_GRAY_SEL : R/W; bitpos: [8]; default: 1;
 *  0:use non-fifo way to access qnr ram,1:use fifo way to access qnr ram
 */
#define JPEG_GRAY_SEL    (BIT(8))
#define JPEG_GRAY_SEL_M  (JPEG_GRAY_SEL_V << JPEG_GRAY_SEL_S)
#define JPEG_GRAY_SEL_V  0x00000001U
#define JPEG_GRAY_SEL_S  8
/** JPEG_LQNR_TBL_SEL : R/W; bitpos: [10:9]; default: 0;
 *  choose  luminance quntization table id(TBD)
 */
#define JPEG_LQNR_TBL_SEL    0x00000003U
#define JPEG_LQNR_TBL_SEL_M  (JPEG_LQNR_TBL_SEL_V << JPEG_LQNR_TBL_SEL_S)
#define JPEG_LQNR_TBL_SEL_V  0x00000003U
#define JPEG_LQNR_TBL_SEL_S  9
/** JPEG_CQNR_TBL_SEL : R/W; bitpos: [12:11]; default: 1;
 *  choose  chrominance quntization table id (TBD)
 */
#define JPEG_CQNR_TBL_SEL    0x00000003U
#define JPEG_CQNR_TBL_SEL_M  (JPEG_CQNR_TBL_SEL_V << JPEG_CQNR_TBL_SEL_S)
#define JPEG_CQNR_TBL_SEL_V  0x00000003U
#define JPEG_CQNR_TBL_SEL_S  11
/** JPEG_COLOR_SPACE : R/W; bitpos: [14:13]; default: 0;
 *  configure picture's color space:0-rb888,1-yuv422,2-rgb565, 3-gray
 */
#define JPEG_COLOR_SPACE    0x00000003U
#define JPEG_COLOR_SPACE_M  (JPEG_COLOR_SPACE_V << JPEG_COLOR_SPACE_S)
#define JPEG_COLOR_SPACE_V  0x00000003U
#define JPEG_COLOR_SPACE_S  13
/** JPEG_DHT_FIFO_EN : R/W; bitpos: [15]; default: 1;
 *  0:use non-fifo way to write dht len_total/codemin/value table,1:use fifo way to
 *  write dht len_total/codemin/value table. Reading dht len_total/codemin/value table
 *  only has nonfifo way
 */
#define JPEG_DHT_FIFO_EN    (BIT(15))
#define JPEG_DHT_FIFO_EN_M  (JPEG_DHT_FIFO_EN_V << JPEG_DHT_FIFO_EN_S)
#define JPEG_DHT_FIFO_EN_V  0x00000001U
#define JPEG_DHT_FIFO_EN_S  15
/** JPEG_MEM_CLK_FORCE_ON : R/W; bitpos: [16]; default: 0;
 *  force memory's clock enabled
 */
#define JPEG_MEM_CLK_FORCE_ON    (BIT(16))
#define JPEG_MEM_CLK_FORCE_ON_M  (JPEG_MEM_CLK_FORCE_ON_V << JPEG_MEM_CLK_FORCE_ON_S)
#define JPEG_MEM_CLK_FORCE_ON_V  0x00000001U
#define JPEG_MEM_CLK_FORCE_ON_S  16
/** JPEG_JFIF_VER : R/W; bitpos: [22:17]; default: 32;
 *  decode pause period to trigger decode_timeout int, the timeout periods =2 power
 *  (reg_decode_timeout_thres) -1
 */
#define JPEG_JFIF_VER    0x0000003FU
#define JPEG_JFIF_VER_M  (JPEG_JFIF_VER_V << JPEG_JFIF_VER_S)
#define JPEG_JFIF_VER_V  0x0000003FU
#define JPEG_JFIF_VER_S  17
/** JPEG_DECODE_TIMEOUT_TASK_SEL : R/W; bitpos: [23]; default: 0;
 *  0: software use reset to abort decode process ,1: decoder abort decode process by
 *  itself
 */
#define JPEG_DECODE_TIMEOUT_TASK_SEL    (BIT(23))
#define JPEG_DECODE_TIMEOUT_TASK_SEL_M  (JPEG_DECODE_TIMEOUT_TASK_SEL_V << JPEG_DECODE_TIMEOUT_TASK_SEL_S)
#define JPEG_DECODE_TIMEOUT_TASK_SEL_V  0x00000001U
#define JPEG_DECODE_TIMEOUT_TASK_SEL_S  23
/** JPEG_SOFT_RST : R/W; bitpos: [24]; default: 0;
 *  when set to 1, soft reset JPEG module except jpeg_reg module
 */
#define JPEG_SOFT_RST    (BIT(24))
#define JPEG_SOFT_RST_M  (JPEG_SOFT_RST_V << JPEG_SOFT_RST_S)
#define JPEG_SOFT_RST_V  0x00000001U
#define JPEG_SOFT_RST_S  24
/** JPEG_FIFO_RST : R/W; bitpos: [25]; default: 0;
 *  fifo reset
 */
#define JPEG_FIFO_RST    (BIT(25))
#define JPEG_FIFO_RST_M  (JPEG_FIFO_RST_V << JPEG_FIFO_RST_S)
#define JPEG_FIFO_RST_V  0x00000001U
#define JPEG_FIFO_RST_S  25
/** JPEG_PIXEL_REV : R/W; bitpos: [26]; default: 0;
 *  reverse the source color pixel
 */
#define JPEG_PIXEL_REV    (BIT(26))
#define JPEG_PIXEL_REV_M  (JPEG_PIXEL_REV_V << JPEG_PIXEL_REV_S)
#define JPEG_PIXEL_REV_V  0x00000001U
#define JPEG_PIXEL_REV_S  26
/** JPEG_TAILER_EN : R/W; bitpos: [27]; default: 0;
 *  set this bit to add EOI of "0xffd9" at the end of bitstream
 */
#define JPEG_TAILER_EN    (BIT(27))
#define JPEG_TAILER_EN_M  (JPEG_TAILER_EN_V << JPEG_TAILER_EN_S)
#define JPEG_TAILER_EN_V  0x00000001U
#define JPEG_TAILER_EN_S  27
/** JPEG_PAUSE_EN : R/W; bitpos: [28]; default: 0;
 *  set this bit to pause jpeg encoding
 */
#define JPEG_PAUSE_EN    (BIT(28))
#define JPEG_PAUSE_EN_M  (JPEG_PAUSE_EN_V << JPEG_PAUSE_EN_S)
#define JPEG_PAUSE_EN_V  0x00000001U
#define JPEG_PAUSE_EN_S  28
/** JPEG_MEM_FORCE_PD : R/W; bitpos: [29]; default: 0;
 *  0: no operation,1:force jpeg memory to power down
 */
#define JPEG_MEM_FORCE_PD    (BIT(29))
#define JPEG_MEM_FORCE_PD_M  (JPEG_MEM_FORCE_PD_V << JPEG_MEM_FORCE_PD_S)
#define JPEG_MEM_FORCE_PD_V  0x00000001U
#define JPEG_MEM_FORCE_PD_S  29
/** JPEG_MEM_FORCE_PU : R/W; bitpos: [30]; default: 0;
 *  0: no operation,1:force jpeg memory to power up
 */
#define JPEG_MEM_FORCE_PU    (BIT(30))
#define JPEG_MEM_FORCE_PU_M  (JPEG_MEM_FORCE_PU_V << JPEG_MEM_FORCE_PU_S)
#define JPEG_MEM_FORCE_PU_V  0x00000001U
#define JPEG_MEM_FORCE_PU_S  30
/** JPEG_MODE : R/W; bitpos: [31]; default: 0;
 *  0:encoder mode, 1: decoder mode
 */
#define JPEG_MODE    (BIT(31))
#define JPEG_MODE_M  (JPEG_MODE_V << JPEG_MODE_S)
#define JPEG_MODE_V  0x00000001U
#define JPEG_MODE_S  31

/** JPEG_DQT_INFO_REG register
 *  Control and configuration registers
 */
#define JPEG_DQT_INFO_REG (DR_REG_JPEG_BASE + 0x4)
/** JPEG_T0_DQT_INFO : R/W; bitpos: [7:0]; default: 0;
 *  Configure dqt table0's quantization coefficient precision in bit[7:4], configure
 *  dqt table0's table id in bit[3:0]
 */
#define JPEG_T0_DQT_INFO    0x000000FFU
#define JPEG_T0_DQT_INFO_M  (JPEG_T0_DQT_INFO_V << JPEG_T0_DQT_INFO_S)
#define JPEG_T0_DQT_INFO_V  0x000000FFU
#define JPEG_T0_DQT_INFO_S  0
/** JPEG_T1_DQT_INFO : R/W; bitpos: [15:8]; default: 1;
 *  Configure dqt table1's quantization coefficient precision in bit[7:4], configure
 *  dqt table1's table id in bit[3:0]
 */
#define JPEG_T1_DQT_INFO    0x000000FFU
#define JPEG_T1_DQT_INFO_M  (JPEG_T1_DQT_INFO_V << JPEG_T1_DQT_INFO_S)
#define JPEG_T1_DQT_INFO_V  0x000000FFU
#define JPEG_T1_DQT_INFO_S  8
/** JPEG_T2_DQT_INFO : R/W; bitpos: [23:16]; default: 2;
 *  Configure dqt table2's quantization coefficient precision in bit[7:4], configure
 *  dqt table2's table id in bit[3:0]
 */
#define JPEG_T2_DQT_INFO    0x000000FFU
#define JPEG_T2_DQT_INFO_M  (JPEG_T2_DQT_INFO_V << JPEG_T2_DQT_INFO_S)
#define JPEG_T2_DQT_INFO_V  0x000000FFU
#define JPEG_T2_DQT_INFO_S  16
/** JPEG_T3_DQT_INFO : R/W; bitpos: [31:24]; default: 3;
 *  Configure dqt table3's quantization coefficient precision in bit[7:4], configure
 *  dqt table3's table id in bit[3:0]
 */
#define JPEG_T3_DQT_INFO    0x000000FFU
#define JPEG_T3_DQT_INFO_M  (JPEG_T3_DQT_INFO_V << JPEG_T3_DQT_INFO_S)
#define JPEG_T3_DQT_INFO_V  0x000000FFU
#define JPEG_T3_DQT_INFO_S  24

/** JPEG_PIC_SIZE_REG register
 *  Control and configuration registers
 */
#define JPEG_PIC_SIZE_REG (DR_REG_JPEG_BASE + 0x8)
/** JPEG_VA : R/W; bitpos: [15:0]; default: 480;
 *  configure picture's height. when encode, the max configurable bits is 14, when
 *  decode, the max configurable bits is 16
 */
#define JPEG_VA    0x0000FFFFU
#define JPEG_VA_M  (JPEG_VA_V << JPEG_VA_S)
#define JPEG_VA_V  0x0000FFFFU
#define JPEG_VA_S  0
/** JPEG_HA : R/W; bitpos: [31:16]; default: 640;
 *  configure picture's width. when encode, the max configurable bits is 14, when
 *  decode, the max configurable bits is 16
 */
#define JPEG_HA    0x0000FFFFU
#define JPEG_HA_M  (JPEG_HA_V << JPEG_HA_S)
#define JPEG_HA_V  0x0000FFFFU
#define JPEG_HA_S  16

/** JPEG_T0QNR_REG register
 *  Control and configuration registers
 */
#define JPEG_T0QNR_REG (DR_REG_JPEG_BASE + 0x10)
/** JPEG_T0_QNR_VAL : HRO; bitpos: [31:0]; default: 0;
 *  write this reg to configure 64 quantization coefficient in t0 table
 */
#define JPEG_T0_QNR_VAL    0xFFFFFFFFU
#define JPEG_T0_QNR_VAL_M  (JPEG_T0_QNR_VAL_V << JPEG_T0_QNR_VAL_S)
#define JPEG_T0_QNR_VAL_V  0xFFFFFFFFU
#define JPEG_T0_QNR_VAL_S  0

/** JPEG_T1QNR_REG register
 *  Control and configuration registers
 */
#define JPEG_T1QNR_REG (DR_REG_JPEG_BASE + 0x14)
/** JPEG_CHROMINANCE_QNR_VAL : HRO; bitpos: [31:0]; default: 0;
 *  write this reg to configure 64 quantization coefficient in t1 table
 */
#define JPEG_CHROMINANCE_QNR_VAL    0xFFFFFFFFU
#define JPEG_CHROMINANCE_QNR_VAL_M  (JPEG_CHROMINANCE_QNR_VAL_V << JPEG_CHROMINANCE_QNR_VAL_S)
#define JPEG_CHROMINANCE_QNR_VAL_V  0xFFFFFFFFU
#define JPEG_CHROMINANCE_QNR_VAL_S  0

/** JPEG_T2QNR_REG register
 *  Control and configuration registers
 */
#define JPEG_T2QNR_REG (DR_REG_JPEG_BASE + 0x18)
/** JPEG_T2_QNR_VAL : HRO; bitpos: [31:0]; default: 0;
 *  write this reg to configure 64 quantization coefficient in t2 table
 */
#define JPEG_T2_QNR_VAL    0xFFFFFFFFU
#define JPEG_T2_QNR_VAL_M  (JPEG_T2_QNR_VAL_V << JPEG_T2_QNR_VAL_S)
#define JPEG_T2_QNR_VAL_V  0xFFFFFFFFU
#define JPEG_T2_QNR_VAL_S  0

/** JPEG_T3QNR_REG register
 *  Control and configuration registers
 */
#define JPEG_T3QNR_REG (DR_REG_JPEG_BASE + 0x1c)
/** JPEG_T3_QNR_VAL : HRO; bitpos: [31:0]; default: 0;
 *  write this reg to configure 64 quantization coefficient in t3 table
 */
#define JPEG_T3_QNR_VAL    0xFFFFFFFFU
#define JPEG_T3_QNR_VAL_M  (JPEG_T3_QNR_VAL_V << JPEG_T3_QNR_VAL_S)
#define JPEG_T3_QNR_VAL_V  0xFFFFFFFFU
#define JPEG_T3_QNR_VAL_S  0

/** JPEG_DECODE_CONF_REG register
 *  Control and configuration registers
 */
#define JPEG_DECODE_CONF_REG (DR_REG_JPEG_BASE + 0x20)
/** JPEG_RESTART_INTERVAL : R/W; bitpos: [15:0]; default: 0;
 *  configure restart interval in DRI marker when decode
 */
#define JPEG_RESTART_INTERVAL    0x0000FFFFU
#define JPEG_RESTART_INTERVAL_M  (JPEG_RESTART_INTERVAL_V << JPEG_RESTART_INTERVAL_S)
#define JPEG_RESTART_INTERVAL_V  0x0000FFFFU
#define JPEG_RESTART_INTERVAL_S  0
/** JPEG_COMPONENT_NUM : R/W; bitpos: [23:16]; default: 3;
 *  configure number of components in frame when decode
 */
#define JPEG_COMPONENT_NUM    0x000000FFU
#define JPEG_COMPONENT_NUM_M  (JPEG_COMPONENT_NUM_V << JPEG_COMPONENT_NUM_S)
#define JPEG_COMPONENT_NUM_V  0x000000FFU
#define JPEG_COMPONENT_NUM_S  16
/** JPEG_SW_DHT_EN : RO; bitpos: [24]; default: 1;
 *  software decode dht table enable
 */
#define JPEG_SW_DHT_EN    (BIT(24))
#define JPEG_SW_DHT_EN_M  (JPEG_SW_DHT_EN_V << JPEG_SW_DHT_EN_S)
#define JPEG_SW_DHT_EN_V  0x00000001U
#define JPEG_SW_DHT_EN_S  24
/** JPEG_SOS_CHECK_BYTE_NUM : R/W; bitpos: [26:25]; default: 3;
 *  Configure the byte number to check next sos marker in the multi-scan picture after
 *  one scan is decoded down. The real check number is reg_sos_check_byte_num+1
 */
#define JPEG_SOS_CHECK_BYTE_NUM    0x00000003U
#define JPEG_SOS_CHECK_BYTE_NUM_M  (JPEG_SOS_CHECK_BYTE_NUM_V << JPEG_SOS_CHECK_BYTE_NUM_S)
#define JPEG_SOS_CHECK_BYTE_NUM_V  0x00000003U
#define JPEG_SOS_CHECK_BYTE_NUM_S  25
/** JPEG_RST_CHECK_BYTE_NUM : R/W; bitpos: [28:27]; default: 3;
 *  Configure the byte number to check next rst marker after one rst interval is
 *  decoded down. The real check number is reg_rst_check_byte_num+1
 */
#define JPEG_RST_CHECK_BYTE_NUM    0x00000003U
#define JPEG_RST_CHECK_BYTE_NUM_M  (JPEG_RST_CHECK_BYTE_NUM_V << JPEG_RST_CHECK_BYTE_NUM_S)
#define JPEG_RST_CHECK_BYTE_NUM_V  0x00000003U
#define JPEG_RST_CHECK_BYTE_NUM_S  27
/** JPEG_MULTI_SCAN_ERR_CHECK : R/W; bitpos: [29]; default: 0;
 *  reserved for decoder
 */
#define JPEG_MULTI_SCAN_ERR_CHECK    (BIT(29))
#define JPEG_MULTI_SCAN_ERR_CHECK_M  (JPEG_MULTI_SCAN_ERR_CHECK_V << JPEG_MULTI_SCAN_ERR_CHECK_S)
#define JPEG_MULTI_SCAN_ERR_CHECK_V  0x00000001U
#define JPEG_MULTI_SCAN_ERR_CHECK_S  29
/** JPEG_DEZIGZAG_READY_CTL : R/W; bitpos: [30]; default: 1;
 *  reserved for decoder
 */
#define JPEG_DEZIGZAG_READY_CTL    (BIT(30))
#define JPEG_DEZIGZAG_READY_CTL_M  (JPEG_DEZIGZAG_READY_CTL_V << JPEG_DEZIGZAG_READY_CTL_S)
#define JPEG_DEZIGZAG_READY_CTL_V  0x00000001U
#define JPEG_DEZIGZAG_READY_CTL_S  30

/** JPEG_C0_REG register
 *  Control and configuration registers
 */
#define JPEG_C0_REG (DR_REG_JPEG_BASE + 0x24)
/** JPEG_C0_DQT_TBL_SEL : R/W; bitpos: [7:0]; default: 0;
 *  choose  c0 quntization table id (TBD)
 */
#define JPEG_C0_DQT_TBL_SEL    0x000000FFU
#define JPEG_C0_DQT_TBL_SEL_M  (JPEG_C0_DQT_TBL_SEL_V << JPEG_C0_DQT_TBL_SEL_S)
#define JPEG_C0_DQT_TBL_SEL_V  0x000000FFU
#define JPEG_C0_DQT_TBL_SEL_S  0
/** JPEG_C0_Y_FACTOR : R/W; bitpos: [11:8]; default: 1;
 *  vertical sampling factor of c0
 */
#define JPEG_C0_Y_FACTOR    0x0000000FU
#define JPEG_C0_Y_FACTOR_M  (JPEG_C0_Y_FACTOR_V << JPEG_C0_Y_FACTOR_S)
#define JPEG_C0_Y_FACTOR_V  0x0000000FU
#define JPEG_C0_Y_FACTOR_S  8
/** JPEG_C0_X_FACTOR : R/W; bitpos: [15:12]; default: 1;
 *  horizontal sampling factor of c0
 */
#define JPEG_C0_X_FACTOR    0x0000000FU
#define JPEG_C0_X_FACTOR_M  (JPEG_C0_X_FACTOR_V << JPEG_C0_X_FACTOR_S)
#define JPEG_C0_X_FACTOR_V  0x0000000FU
#define JPEG_C0_X_FACTOR_S  12
/** JPEG_C0_ID : R/W; bitpos: [23:16]; default: 0;
 *  the identifier of c0
 */
#define JPEG_C0_ID    0x000000FFU
#define JPEG_C0_ID_M  (JPEG_C0_ID_V << JPEG_C0_ID_S)
#define JPEG_C0_ID_V  0x000000FFU
#define JPEG_C0_ID_S  16

/** JPEG_C1_REG register
 *  Control and configuration registers
 */
#define JPEG_C1_REG (DR_REG_JPEG_BASE + 0x28)
/** JPEG_C1_DQT_TBL_SEL : R/W; bitpos: [7:0]; default: 0;
 *  choose  c1 quntization table id (TBD)
 */
#define JPEG_C1_DQT_TBL_SEL    0x000000FFU
#define JPEG_C1_DQT_TBL_SEL_M  (JPEG_C1_DQT_TBL_SEL_V << JPEG_C1_DQT_TBL_SEL_S)
#define JPEG_C1_DQT_TBL_SEL_V  0x000000FFU
#define JPEG_C1_DQT_TBL_SEL_S  0
/** JPEG_C1_Y_FACTOR : R/W; bitpos: [11:8]; default: 1;
 *  vertical sampling factor of c1
 */
#define JPEG_C1_Y_FACTOR    0x0000000FU
#define JPEG_C1_Y_FACTOR_M  (JPEG_C1_Y_FACTOR_V << JPEG_C1_Y_FACTOR_S)
#define JPEG_C1_Y_FACTOR_V  0x0000000FU
#define JPEG_C1_Y_FACTOR_S  8
/** JPEG_C1_X_FACTOR : R/W; bitpos: [15:12]; default: 1;
 *  horizontal sampling factor of c1
 */
#define JPEG_C1_X_FACTOR    0x0000000FU
#define JPEG_C1_X_FACTOR_M  (JPEG_C1_X_FACTOR_V << JPEG_C1_X_FACTOR_S)
#define JPEG_C1_X_FACTOR_V  0x0000000FU
#define JPEG_C1_X_FACTOR_S  12
/** JPEG_C1_ID : R/W; bitpos: [23:16]; default: 0;
 *  the identifier of c1
 */
#define JPEG_C1_ID    0x000000FFU
#define JPEG_C1_ID_M  (JPEG_C1_ID_V << JPEG_C1_ID_S)
#define JPEG_C1_ID_V  0x000000FFU
#define JPEG_C1_ID_S  16

/** JPEG_C2_REG register
 *  Control and configuration registers
 */
#define JPEG_C2_REG (DR_REG_JPEG_BASE + 0x2c)
/** JPEG_C2_DQT_TBL_SEL : R/W; bitpos: [7:0]; default: 0;
 *  choose  c2 quntization table id (TBD)
 */
#define JPEG_C2_DQT_TBL_SEL    0x000000FFU
#define JPEG_C2_DQT_TBL_SEL_M  (JPEG_C2_DQT_TBL_SEL_V << JPEG_C2_DQT_TBL_SEL_S)
#define JPEG_C2_DQT_TBL_SEL_V  0x000000FFU
#define JPEG_C2_DQT_TBL_SEL_S  0
/** JPEG_C2_Y_FACTOR : R/W; bitpos: [11:8]; default: 1;
 *  vertical sampling factor of c2
 */
#define JPEG_C2_Y_FACTOR    0x0000000FU
#define JPEG_C2_Y_FACTOR_M  (JPEG_C2_Y_FACTOR_V << JPEG_C2_Y_FACTOR_S)
#define JPEG_C2_Y_FACTOR_V  0x0000000FU
#define JPEG_C2_Y_FACTOR_S  8
/** JPEG_C2_X_FACTOR : R/W; bitpos: [15:12]; default: 1;
 *  horizontal sampling factor of c2
 */
#define JPEG_C2_X_FACTOR    0x0000000FU
#define JPEG_C2_X_FACTOR_M  (JPEG_C2_X_FACTOR_V << JPEG_C2_X_FACTOR_S)
#define JPEG_C2_X_FACTOR_V  0x0000000FU
#define JPEG_C2_X_FACTOR_S  12
/** JPEG_C2_ID : R/W; bitpos: [23:16]; default: 0;
 *  the identifier of c2
 */
#define JPEG_C2_ID    0x000000FFU
#define JPEG_C2_ID_M  (JPEG_C2_ID_V << JPEG_C2_ID_S)
#define JPEG_C2_ID_V  0x000000FFU
#define JPEG_C2_ID_S  16

/** JPEG_C3_REG register
 *  Control and configuration registers
 */
#define JPEG_C3_REG (DR_REG_JPEG_BASE + 0x30)
/** JPEG_C3_DQT_TBL_SEL : R/W; bitpos: [7:0]; default: 0;
 *  choose  c3 quntization table id (TBD)
 */
#define JPEG_C3_DQT_TBL_SEL    0x000000FFU
#define JPEG_C3_DQT_TBL_SEL_M  (JPEG_C3_DQT_TBL_SEL_V << JPEG_C3_DQT_TBL_SEL_S)
#define JPEG_C3_DQT_TBL_SEL_V  0x000000FFU
#define JPEG_C3_DQT_TBL_SEL_S  0
/** JPEG_C3_Y_FACTOR : R/W; bitpos: [11:8]; default: 1;
 *  vertical sampling factor of c3
 */
#define JPEG_C3_Y_FACTOR    0x0000000FU
#define JPEG_C3_Y_FACTOR_M  (JPEG_C3_Y_FACTOR_V << JPEG_C3_Y_FACTOR_S)
#define JPEG_C3_Y_FACTOR_V  0x0000000FU
#define JPEG_C3_Y_FACTOR_S  8
/** JPEG_C3_X_FACTOR : R/W; bitpos: [15:12]; default: 1;
 *  horizontal sampling factor of c3
 */
#define JPEG_C3_X_FACTOR    0x0000000FU
#define JPEG_C3_X_FACTOR_M  (JPEG_C3_X_FACTOR_V << JPEG_C3_X_FACTOR_S)
#define JPEG_C3_X_FACTOR_V  0x0000000FU
#define JPEG_C3_X_FACTOR_S  12
/** JPEG_C3_ID : R/W; bitpos: [23:16]; default: 0;
 *  the identifier of c3
 */
#define JPEG_C3_ID    0x000000FFU
#define JPEG_C3_ID_M  (JPEG_C3_ID_V << JPEG_C3_ID_S)
#define JPEG_C3_ID_V  0x000000FFU
#define JPEG_C3_ID_S  16

/** JPEG_DHT_INFO_REG register
 *  Control and configuration registers
 */
#define JPEG_DHT_INFO_REG (DR_REG_JPEG_BASE + 0x34)
/** JPEG_DC0_DHT_ID : R/W; bitpos: [3:0]; default: 0;
 *  configure dht dc table 0 id
 */
#define JPEG_DC0_DHT_ID    0x0000000FU
#define JPEG_DC0_DHT_ID_M  (JPEG_DC0_DHT_ID_V << JPEG_DC0_DHT_ID_S)
#define JPEG_DC0_DHT_ID_V  0x0000000FU
#define JPEG_DC0_DHT_ID_S  0
/** JPEG_DC1_DHT_ID : R/W; bitpos: [7:4]; default: 1;
 *  configure dht dc table 1 id
 */
#define JPEG_DC1_DHT_ID    0x0000000FU
#define JPEG_DC1_DHT_ID_M  (JPEG_DC1_DHT_ID_V << JPEG_DC1_DHT_ID_S)
#define JPEG_DC1_DHT_ID_V  0x0000000FU
#define JPEG_DC1_DHT_ID_S  4
/** JPEG_AC0_DHT_ID : R/W; bitpos: [11:8]; default: 0;
 *  configure dht ac table 0 id
 */
#define JPEG_AC0_DHT_ID    0x0000000FU
#define JPEG_AC0_DHT_ID_M  (JPEG_AC0_DHT_ID_V << JPEG_AC0_DHT_ID_S)
#define JPEG_AC0_DHT_ID_V  0x0000000FU
#define JPEG_AC0_DHT_ID_S  8
/** JPEG_AC1_DHT_ID : R/W; bitpos: [15:12]; default: 1;
 *  configure dht ac table 1 id
 */
#define JPEG_AC1_DHT_ID    0x0000000FU
#define JPEG_AC1_DHT_ID_M  (JPEG_AC1_DHT_ID_V << JPEG_AC1_DHT_ID_S)
#define JPEG_AC1_DHT_ID_V  0x0000000FU
#define JPEG_AC1_DHT_ID_S  12

/** JPEG_INT_RAW_REG register
 *  Interrupt raw registers
 */
#define JPEG_INT_RAW_REG (DR_REG_JPEG_BASE + 0x38)
/** JPEG_DONE_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  This raw interrupt bit turns to high level when JPEG finishes encoding a picture..
 */
#define JPEG_DONE_INT_RAW    (BIT(0))
#define JPEG_DONE_INT_RAW_M  (JPEG_DONE_INT_RAW_V << JPEG_DONE_INT_RAW_S)
#define JPEG_DONE_INT_RAW_V  0x00000001U
#define JPEG_DONE_INT_RAW_S  0
/** JPEG_RLE_PARALLEL_ERR_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit to sign that rle parallel error when decoding.
 */
#define JPEG_RLE_PARALLEL_ERR_INT_RAW    (BIT(1))
#define JPEG_RLE_PARALLEL_ERR_INT_RAW_M  (JPEG_RLE_PARALLEL_ERR_INT_RAW_V << JPEG_RLE_PARALLEL_ERR_INT_RAW_S)
#define JPEG_RLE_PARALLEL_ERR_INT_RAW_V  0x00000001U
#define JPEG_RLE_PARALLEL_ERR_INT_RAW_S  1
/** JPEG_CID_ERR_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit to sign that scan id check with component fails when decoding.
 */
#define JPEG_CID_ERR_INT_RAW    (BIT(2))
#define JPEG_CID_ERR_INT_RAW_M  (JPEG_CID_ERR_INT_RAW_V << JPEG_CID_ERR_INT_RAW_S)
#define JPEG_CID_ERR_INT_RAW_V  0x00000001U
#define JPEG_CID_ERR_INT_RAW_S  2
/** JPEG_C_DHT_DC_ID_ERR_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit to sign that scan component's dc dht id check with dc dht
 *  table's id fails when decoding.
 */
#define JPEG_C_DHT_DC_ID_ERR_INT_RAW    (BIT(3))
#define JPEG_C_DHT_DC_ID_ERR_INT_RAW_M  (JPEG_C_DHT_DC_ID_ERR_INT_RAW_V << JPEG_C_DHT_DC_ID_ERR_INT_RAW_S)
#define JPEG_C_DHT_DC_ID_ERR_INT_RAW_V  0x00000001U
#define JPEG_C_DHT_DC_ID_ERR_INT_RAW_S  3
/** JPEG_C_DHT_AC_ID_ERR_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit to sign that scan component's ac dht id check with ac dht
 *  table's id fails when decoding.
 */
#define JPEG_C_DHT_AC_ID_ERR_INT_RAW    (BIT(4))
#define JPEG_C_DHT_AC_ID_ERR_INT_RAW_M  (JPEG_C_DHT_AC_ID_ERR_INT_RAW_V << JPEG_C_DHT_AC_ID_ERR_INT_RAW_S)
#define JPEG_C_DHT_AC_ID_ERR_INT_RAW_V  0x00000001U
#define JPEG_C_DHT_AC_ID_ERR_INT_RAW_S  4
/** JPEG_C_DQT_ID_ERR_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit to sign that scan component's dqt id check with dqt table's
 *  id fails when decoding.
 */
#define JPEG_C_DQT_ID_ERR_INT_RAW    (BIT(5))
#define JPEG_C_DQT_ID_ERR_INT_RAW_M  (JPEG_C_DQT_ID_ERR_INT_RAW_V << JPEG_C_DQT_ID_ERR_INT_RAW_S)
#define JPEG_C_DQT_ID_ERR_INT_RAW_V  0x00000001U
#define JPEG_C_DQT_ID_ERR_INT_RAW_S  5
/** JPEG_RST_UXP_ERR_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit to sign that RST header marker is detected but restart
 *  interval is 0 when decoding.
 */
#define JPEG_RST_UXP_ERR_INT_RAW    (BIT(6))
#define JPEG_RST_UXP_ERR_INT_RAW_M  (JPEG_RST_UXP_ERR_INT_RAW_V << JPEG_RST_UXP_ERR_INT_RAW_S)
#define JPEG_RST_UXP_ERR_INT_RAW_V  0x00000001U
#define JPEG_RST_UXP_ERR_INT_RAW_S  6
/** JPEG_RST_CHECK_NONE_ERR_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit to sign that RST header marker is not detected but restart
 *  interval is not 0 when decoding.
 */
#define JPEG_RST_CHECK_NONE_ERR_INT_RAW    (BIT(7))
#define JPEG_RST_CHECK_NONE_ERR_INT_RAW_M  (JPEG_RST_CHECK_NONE_ERR_INT_RAW_V << JPEG_RST_CHECK_NONE_ERR_INT_RAW_S)
#define JPEG_RST_CHECK_NONE_ERR_INT_RAW_V  0x00000001U
#define JPEG_RST_CHECK_NONE_ERR_INT_RAW_S  7
/** JPEG_RST_CHECK_POS_ERR_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit to sign that RST header marker position mismatches with
 *  restart interval when decoding.
 */
#define JPEG_RST_CHECK_POS_ERR_INT_RAW    (BIT(8))
#define JPEG_RST_CHECK_POS_ERR_INT_RAW_M  (JPEG_RST_CHECK_POS_ERR_INT_RAW_V << JPEG_RST_CHECK_POS_ERR_INT_RAW_S)
#define JPEG_RST_CHECK_POS_ERR_INT_RAW_V  0x00000001U
#define JPEG_RST_CHECK_POS_ERR_INT_RAW_S  8
/** JPEG_OUT_EOF_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when the last pixel of one square has
 *  been transmitted for Tx channel.
 */
#define JPEG_OUT_EOF_INT_RAW    (BIT(9))
#define JPEG_OUT_EOF_INT_RAW_M  (JPEG_OUT_EOF_INT_RAW_V << JPEG_OUT_EOF_INT_RAW_S)
#define JPEG_OUT_EOF_INT_RAW_V  0x00000001U
#define JPEG_OUT_EOF_INT_RAW_S  9
/** JPEG_SR_COLOR_MODE_ERR_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt bit to sign that the selected source color mode is not supported.
 */
#define JPEG_SR_COLOR_MODE_ERR_INT_RAW    (BIT(10))
#define JPEG_SR_COLOR_MODE_ERR_INT_RAW_M  (JPEG_SR_COLOR_MODE_ERR_INT_RAW_V << JPEG_SR_COLOR_MODE_ERR_INT_RAW_S)
#define JPEG_SR_COLOR_MODE_ERR_INT_RAW_V  0x00000001U
#define JPEG_SR_COLOR_MODE_ERR_INT_RAW_S  10
/** JPEG_DCT_DONE_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt bit to sign that one dct calculation is finished.
 */
#define JPEG_DCT_DONE_INT_RAW    (BIT(11))
#define JPEG_DCT_DONE_INT_RAW_M  (JPEG_DCT_DONE_INT_RAW_V << JPEG_DCT_DONE_INT_RAW_S)
#define JPEG_DCT_DONE_INT_RAW_V  0x00000001U
#define JPEG_DCT_DONE_INT_RAW_S  11
/** JPEG_BS_LAST_BLOCK_EOF_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt bit to sign that the coding process for last block is finished.
 */
#define JPEG_BS_LAST_BLOCK_EOF_INT_RAW    (BIT(12))
#define JPEG_BS_LAST_BLOCK_EOF_INT_RAW_M  (JPEG_BS_LAST_BLOCK_EOF_INT_RAW_V << JPEG_BS_LAST_BLOCK_EOF_INT_RAW_S)
#define JPEG_BS_LAST_BLOCK_EOF_INT_RAW_V  0x00000001U
#define JPEG_BS_LAST_BLOCK_EOF_INT_RAW_S  12
/** JPEG_SCAN_CHECK_NONE_ERR_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw interrupt bit to sign that SOS header marker is not detected but there are
 *  still components left to be decoded.
 */
#define JPEG_SCAN_CHECK_NONE_ERR_INT_RAW    (BIT(13))
#define JPEG_SCAN_CHECK_NONE_ERR_INT_RAW_M  (JPEG_SCAN_CHECK_NONE_ERR_INT_RAW_V << JPEG_SCAN_CHECK_NONE_ERR_INT_RAW_S)
#define JPEG_SCAN_CHECK_NONE_ERR_INT_RAW_V  0x00000001U
#define JPEG_SCAN_CHECK_NONE_ERR_INT_RAW_S  13
/** JPEG_SCAN_CHECK_POS_ERR_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  The raw interrupt bit to sign that SOS header marker position wrong when decoding.
 */
#define JPEG_SCAN_CHECK_POS_ERR_INT_RAW    (BIT(14))
#define JPEG_SCAN_CHECK_POS_ERR_INT_RAW_M  (JPEG_SCAN_CHECK_POS_ERR_INT_RAW_V << JPEG_SCAN_CHECK_POS_ERR_INT_RAW_S)
#define JPEG_SCAN_CHECK_POS_ERR_INT_RAW_V  0x00000001U
#define JPEG_SCAN_CHECK_POS_ERR_INT_RAW_S  14
/** JPEG_UXP_DET_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  The raw interrupt bit to sign that unsupported header marker is detected when
 *  decoding.
 */
#define JPEG_UXP_DET_INT_RAW    (BIT(15))
#define JPEG_UXP_DET_INT_RAW_M  (JPEG_UXP_DET_INT_RAW_V << JPEG_UXP_DET_INT_RAW_S)
#define JPEG_UXP_DET_INT_RAW_V  0x00000001U
#define JPEG_UXP_DET_INT_RAW_S  15
/** JPEG_EN_FRAME_EOF_ERR_INT_RAW : R/WTC/SS; bitpos: [16]; default: 0;
 *  The raw interrupt bit to sign that received pixel blocks are smaller than expected
 *  when encoding.
 */
#define JPEG_EN_FRAME_EOF_ERR_INT_RAW    (BIT(16))
#define JPEG_EN_FRAME_EOF_ERR_INT_RAW_M  (JPEG_EN_FRAME_EOF_ERR_INT_RAW_V << JPEG_EN_FRAME_EOF_ERR_INT_RAW_S)
#define JPEG_EN_FRAME_EOF_ERR_INT_RAW_V  0x00000001U
#define JPEG_EN_FRAME_EOF_ERR_INT_RAW_S  16
/** JPEG_EN_FRAME_EOF_LACK_INT_RAW : R/WTC/SS; bitpos: [17]; default: 0;
 *  The raw interrupt bit to sign that the frame eof sign bit from dma input is missing
 *  when encoding. But the number of pixel blocks is enough.
 */
#define JPEG_EN_FRAME_EOF_LACK_INT_RAW    (BIT(17))
#define JPEG_EN_FRAME_EOF_LACK_INT_RAW_M  (JPEG_EN_FRAME_EOF_LACK_INT_RAW_V << JPEG_EN_FRAME_EOF_LACK_INT_RAW_S)
#define JPEG_EN_FRAME_EOF_LACK_INT_RAW_V  0x00000001U
#define JPEG_EN_FRAME_EOF_LACK_INT_RAW_S  17
/** JPEG_DE_FRAME_EOF_ERR_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  The raw interrupt bit to sign that decoded blocks are smaller than expected when
 *  decoding.
 */
#define JPEG_DE_FRAME_EOF_ERR_INT_RAW    (BIT(18))
#define JPEG_DE_FRAME_EOF_ERR_INT_RAW_M  (JPEG_DE_FRAME_EOF_ERR_INT_RAW_V << JPEG_DE_FRAME_EOF_ERR_INT_RAW_S)
#define JPEG_DE_FRAME_EOF_ERR_INT_RAW_V  0x00000001U
#define JPEG_DE_FRAME_EOF_ERR_INT_RAW_S  18
/** JPEG_DE_FRAME_EOF_LACK_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  The raw interrupt bit to sign that the either frame eof from dma input or eoi
 *  marker is missing when encoding. But the number of decoded blocks is enough.
 */
#define JPEG_DE_FRAME_EOF_LACK_INT_RAW    (BIT(19))
#define JPEG_DE_FRAME_EOF_LACK_INT_RAW_M  (JPEG_DE_FRAME_EOF_LACK_INT_RAW_V << JPEG_DE_FRAME_EOF_LACK_INT_RAW_S)
#define JPEG_DE_FRAME_EOF_LACK_INT_RAW_V  0x00000001U
#define JPEG_DE_FRAME_EOF_LACK_INT_RAW_S  19
/** JPEG_SOS_UNMATCH_ERR_INT_RAW : R/WTC/SS; bitpos: [20]; default: 0;
 *  The raw interrupt bit to sign that the component number of a scan is 0 or does not
 *  match the sos marker's length when decoding.
 */
#define JPEG_SOS_UNMATCH_ERR_INT_RAW    (BIT(20))
#define JPEG_SOS_UNMATCH_ERR_INT_RAW_M  (JPEG_SOS_UNMATCH_ERR_INT_RAW_V << JPEG_SOS_UNMATCH_ERR_INT_RAW_S)
#define JPEG_SOS_UNMATCH_ERR_INT_RAW_V  0x00000001U
#define JPEG_SOS_UNMATCH_ERR_INT_RAW_S  20
/** JPEG_MARKER_ERR_FST_SCAN_INT_RAW : R/WTC/SS; bitpos: [21]; default: 0;
 *  The raw interrupt bit to sign that the first scan has header marker error when
 *  decoding.
 */
#define JPEG_MARKER_ERR_FST_SCAN_INT_RAW    (BIT(21))
#define JPEG_MARKER_ERR_FST_SCAN_INT_RAW_M  (JPEG_MARKER_ERR_FST_SCAN_INT_RAW_V << JPEG_MARKER_ERR_FST_SCAN_INT_RAW_S)
#define JPEG_MARKER_ERR_FST_SCAN_INT_RAW_V  0x00000001U
#define JPEG_MARKER_ERR_FST_SCAN_INT_RAW_S  21
/** JPEG_MARKER_ERR_OTHER_SCAN_INT_RAW : R/WTC/SS; bitpos: [22]; default: 0;
 *  The raw interrupt bit to sign that the following scans but not the first scan have
 *  header marker error when decoding.
 */
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_RAW    (BIT(22))
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_RAW_M  (JPEG_MARKER_ERR_OTHER_SCAN_INT_RAW_V << JPEG_MARKER_ERR_OTHER_SCAN_INT_RAW_S)
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_RAW_V  0x00000001U
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_RAW_S  22
/** JPEG_UNDET_INT_RAW : R/WTC/SS; bitpos: [23]; default: 0;
 *  The raw interrupt bit to sign that JPEG format is not detected at the eof data of a
 *  packet when decoding.
 */
#define JPEG_UNDET_INT_RAW    (BIT(23))
#define JPEG_UNDET_INT_RAW_M  (JPEG_UNDET_INT_RAW_V << JPEG_UNDET_INT_RAW_S)
#define JPEG_UNDET_INT_RAW_V  0x00000001U
#define JPEG_UNDET_INT_RAW_S  23
/** JPEG_DECODE_TIMEOUT_INT_RAW : R/WTC/SS; bitpos: [24]; default: 0;
 *  The raw interrupt bit to sign that decode pause time is longer than the setting
 *  decode timeout time when decoding.
 */
#define JPEG_DECODE_TIMEOUT_INT_RAW    (BIT(24))
#define JPEG_DECODE_TIMEOUT_INT_RAW_M  (JPEG_DECODE_TIMEOUT_INT_RAW_V << JPEG_DECODE_TIMEOUT_INT_RAW_S)
#define JPEG_DECODE_TIMEOUT_INT_RAW_V  0x00000001U
#define JPEG_DECODE_TIMEOUT_INT_RAW_S  24

/** JPEG_INT_ENA_REG register
 *  Interrupt enable registers
 */
#define JPEG_INT_ENA_REG (DR_REG_JPEG_BASE + 0x3c)
/** JPEG_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  This enable interrupt bit turns to high level when JPEG finishes encoding a
 *  picture..
 */
#define JPEG_DONE_INT_ENA    (BIT(0))
#define JPEG_DONE_INT_ENA_M  (JPEG_DONE_INT_ENA_V << JPEG_DONE_INT_ENA_S)
#define JPEG_DONE_INT_ENA_V  0x00000001U
#define JPEG_DONE_INT_ENA_S  0
/** JPEG_RLE_PARALLEL_ERR_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The enable interrupt bit to sign that rle parallel error when decoding.
 */
#define JPEG_RLE_PARALLEL_ERR_INT_ENA    (BIT(1))
#define JPEG_RLE_PARALLEL_ERR_INT_ENA_M  (JPEG_RLE_PARALLEL_ERR_INT_ENA_V << JPEG_RLE_PARALLEL_ERR_INT_ENA_S)
#define JPEG_RLE_PARALLEL_ERR_INT_ENA_V  0x00000001U
#define JPEG_RLE_PARALLEL_ERR_INT_ENA_S  1
/** JPEG_CID_ERR_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The enable interrupt bit to sign that scan id check with component fails when
 *  decoding.
 */
#define JPEG_CID_ERR_INT_ENA    (BIT(2))
#define JPEG_CID_ERR_INT_ENA_M  (JPEG_CID_ERR_INT_ENA_V << JPEG_CID_ERR_INT_ENA_S)
#define JPEG_CID_ERR_INT_ENA_V  0x00000001U
#define JPEG_CID_ERR_INT_ENA_S  2
/** JPEG_C_DHT_DC_ID_ERR_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The enable interrupt bit to sign that scan component's dc dht id check with dc dht
 *  table's id fails when decoding.
 */
#define JPEG_C_DHT_DC_ID_ERR_INT_ENA    (BIT(3))
#define JPEG_C_DHT_DC_ID_ERR_INT_ENA_M  (JPEG_C_DHT_DC_ID_ERR_INT_ENA_V << JPEG_C_DHT_DC_ID_ERR_INT_ENA_S)
#define JPEG_C_DHT_DC_ID_ERR_INT_ENA_V  0x00000001U
#define JPEG_C_DHT_DC_ID_ERR_INT_ENA_S  3
/** JPEG_C_DHT_AC_ID_ERR_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The enable interrupt bit to sign that scan component's ac dht id check with ac dht
 *  table's id fails when decoding.
 */
#define JPEG_C_DHT_AC_ID_ERR_INT_ENA    (BIT(4))
#define JPEG_C_DHT_AC_ID_ERR_INT_ENA_M  (JPEG_C_DHT_AC_ID_ERR_INT_ENA_V << JPEG_C_DHT_AC_ID_ERR_INT_ENA_S)
#define JPEG_C_DHT_AC_ID_ERR_INT_ENA_V  0x00000001U
#define JPEG_C_DHT_AC_ID_ERR_INT_ENA_S  4
/** JPEG_C_DQT_ID_ERR_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The enable interrupt bit to sign that scan component's dqt id check with dqt
 *  table's id fails when decoding.
 */
#define JPEG_C_DQT_ID_ERR_INT_ENA    (BIT(5))
#define JPEG_C_DQT_ID_ERR_INT_ENA_M  (JPEG_C_DQT_ID_ERR_INT_ENA_V << JPEG_C_DQT_ID_ERR_INT_ENA_S)
#define JPEG_C_DQT_ID_ERR_INT_ENA_V  0x00000001U
#define JPEG_C_DQT_ID_ERR_INT_ENA_S  5
/** JPEG_RST_UXP_ERR_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The enable interrupt bit to sign that RST header marker is detected but restart
 *  interval is 0 when decoding.
 */
#define JPEG_RST_UXP_ERR_INT_ENA    (BIT(6))
#define JPEG_RST_UXP_ERR_INT_ENA_M  (JPEG_RST_UXP_ERR_INT_ENA_V << JPEG_RST_UXP_ERR_INT_ENA_S)
#define JPEG_RST_UXP_ERR_INT_ENA_V  0x00000001U
#define JPEG_RST_UXP_ERR_INT_ENA_S  6
/** JPEG_RST_CHECK_NONE_ERR_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The enable interrupt bit to sign that RST header marker is not detected but restart
 *  interval is not 0 when decoding.
 */
#define JPEG_RST_CHECK_NONE_ERR_INT_ENA    (BIT(7))
#define JPEG_RST_CHECK_NONE_ERR_INT_ENA_M  (JPEG_RST_CHECK_NONE_ERR_INT_ENA_V << JPEG_RST_CHECK_NONE_ERR_INT_ENA_S)
#define JPEG_RST_CHECK_NONE_ERR_INT_ENA_V  0x00000001U
#define JPEG_RST_CHECK_NONE_ERR_INT_ENA_S  7
/** JPEG_RST_CHECK_POS_ERR_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The enable interrupt bit to sign that RST header marker position mismatches with
 *  restart interval when decoding.
 */
#define JPEG_RST_CHECK_POS_ERR_INT_ENA    (BIT(8))
#define JPEG_RST_CHECK_POS_ERR_INT_ENA_M  (JPEG_RST_CHECK_POS_ERR_INT_ENA_V << JPEG_RST_CHECK_POS_ERR_INT_ENA_S)
#define JPEG_RST_CHECK_POS_ERR_INT_ENA_V  0x00000001U
#define JPEG_RST_CHECK_POS_ERR_INT_ENA_S  8
/** JPEG_OUT_EOF_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The enable interrupt bit turns to high level when the last pixel of one square has
 *  been transmitted for Tx channel.
 */
#define JPEG_OUT_EOF_INT_ENA    (BIT(9))
#define JPEG_OUT_EOF_INT_ENA_M  (JPEG_OUT_EOF_INT_ENA_V << JPEG_OUT_EOF_INT_ENA_S)
#define JPEG_OUT_EOF_INT_ENA_V  0x00000001U
#define JPEG_OUT_EOF_INT_ENA_S  9
/** JPEG_SR_COLOR_MODE_ERR_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The enable interrupt bit to sign that the selected source color mode is not
 *  supported.
 */
#define JPEG_SR_COLOR_MODE_ERR_INT_ENA    (BIT(10))
#define JPEG_SR_COLOR_MODE_ERR_INT_ENA_M  (JPEG_SR_COLOR_MODE_ERR_INT_ENA_V << JPEG_SR_COLOR_MODE_ERR_INT_ENA_S)
#define JPEG_SR_COLOR_MODE_ERR_INT_ENA_V  0x00000001U
#define JPEG_SR_COLOR_MODE_ERR_INT_ENA_S  10
/** JPEG_DCT_DONE_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The enable interrupt bit to sign that one dct calculation is finished.
 */
#define JPEG_DCT_DONE_INT_ENA    (BIT(11))
#define JPEG_DCT_DONE_INT_ENA_M  (JPEG_DCT_DONE_INT_ENA_V << JPEG_DCT_DONE_INT_ENA_S)
#define JPEG_DCT_DONE_INT_ENA_V  0x00000001U
#define JPEG_DCT_DONE_INT_ENA_S  11
/** JPEG_BS_LAST_BLOCK_EOF_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The enable interrupt bit to sign that the coding process for last block is finished.
 */
#define JPEG_BS_LAST_BLOCK_EOF_INT_ENA    (BIT(12))
#define JPEG_BS_LAST_BLOCK_EOF_INT_ENA_M  (JPEG_BS_LAST_BLOCK_EOF_INT_ENA_V << JPEG_BS_LAST_BLOCK_EOF_INT_ENA_S)
#define JPEG_BS_LAST_BLOCK_EOF_INT_ENA_V  0x00000001U
#define JPEG_BS_LAST_BLOCK_EOF_INT_ENA_S  12
/** JPEG_SCAN_CHECK_NONE_ERR_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The enable interrupt bit to sign that SOS header marker is not detected but there
 *  are still components left to be decoded.
 */
#define JPEG_SCAN_CHECK_NONE_ERR_INT_ENA    (BIT(13))
#define JPEG_SCAN_CHECK_NONE_ERR_INT_ENA_M  (JPEG_SCAN_CHECK_NONE_ERR_INT_ENA_V << JPEG_SCAN_CHECK_NONE_ERR_INT_ENA_S)
#define JPEG_SCAN_CHECK_NONE_ERR_INT_ENA_V  0x00000001U
#define JPEG_SCAN_CHECK_NONE_ERR_INT_ENA_S  13
/** JPEG_SCAN_CHECK_POS_ERR_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  The enable interrupt bit to sign that SOS header marker position wrong when
 *  decoding.
 */
#define JPEG_SCAN_CHECK_POS_ERR_INT_ENA    (BIT(14))
#define JPEG_SCAN_CHECK_POS_ERR_INT_ENA_M  (JPEG_SCAN_CHECK_POS_ERR_INT_ENA_V << JPEG_SCAN_CHECK_POS_ERR_INT_ENA_S)
#define JPEG_SCAN_CHECK_POS_ERR_INT_ENA_V  0x00000001U
#define JPEG_SCAN_CHECK_POS_ERR_INT_ENA_S  14
/** JPEG_UXP_DET_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  The enable interrupt bit to sign that unsupported header marker is detected when
 *  decoding.
 */
#define JPEG_UXP_DET_INT_ENA    (BIT(15))
#define JPEG_UXP_DET_INT_ENA_M  (JPEG_UXP_DET_INT_ENA_V << JPEG_UXP_DET_INT_ENA_S)
#define JPEG_UXP_DET_INT_ENA_V  0x00000001U
#define JPEG_UXP_DET_INT_ENA_S  15
/** JPEG_EN_FRAME_EOF_ERR_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  The enable interrupt bit to sign that received pixel blocks are smaller than
 *  expected when encoding.
 */
#define JPEG_EN_FRAME_EOF_ERR_INT_ENA    (BIT(16))
#define JPEG_EN_FRAME_EOF_ERR_INT_ENA_M  (JPEG_EN_FRAME_EOF_ERR_INT_ENA_V << JPEG_EN_FRAME_EOF_ERR_INT_ENA_S)
#define JPEG_EN_FRAME_EOF_ERR_INT_ENA_V  0x00000001U
#define JPEG_EN_FRAME_EOF_ERR_INT_ENA_S  16
/** JPEG_EN_FRAME_EOF_LACK_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  The enable interrupt bit to sign that the frame eof sign bit from dma input is
 *  missing when encoding. But the number of pixel blocks is enough.
 */
#define JPEG_EN_FRAME_EOF_LACK_INT_ENA    (BIT(17))
#define JPEG_EN_FRAME_EOF_LACK_INT_ENA_M  (JPEG_EN_FRAME_EOF_LACK_INT_ENA_V << JPEG_EN_FRAME_EOF_LACK_INT_ENA_S)
#define JPEG_EN_FRAME_EOF_LACK_INT_ENA_V  0x00000001U
#define JPEG_EN_FRAME_EOF_LACK_INT_ENA_S  17
/** JPEG_DE_FRAME_EOF_ERR_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  The enable interrupt bit to sign that decoded blocks are smaller than expected when
 *  decoding.
 */
#define JPEG_DE_FRAME_EOF_ERR_INT_ENA    (BIT(18))
#define JPEG_DE_FRAME_EOF_ERR_INT_ENA_M  (JPEG_DE_FRAME_EOF_ERR_INT_ENA_V << JPEG_DE_FRAME_EOF_ERR_INT_ENA_S)
#define JPEG_DE_FRAME_EOF_ERR_INT_ENA_V  0x00000001U
#define JPEG_DE_FRAME_EOF_ERR_INT_ENA_S  18
/** JPEG_DE_FRAME_EOF_LACK_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  The enable interrupt bit to sign that the either frame eof from dma input or eoi
 *  marker is missing when encoding. But the number of decoded blocks is enough.
 */
#define JPEG_DE_FRAME_EOF_LACK_INT_ENA    (BIT(19))
#define JPEG_DE_FRAME_EOF_LACK_INT_ENA_M  (JPEG_DE_FRAME_EOF_LACK_INT_ENA_V << JPEG_DE_FRAME_EOF_LACK_INT_ENA_S)
#define JPEG_DE_FRAME_EOF_LACK_INT_ENA_V  0x00000001U
#define JPEG_DE_FRAME_EOF_LACK_INT_ENA_S  19
/** JPEG_SOS_UNMATCH_ERR_INT_ENA : R/W; bitpos: [20]; default: 0;
 *  The enable interrupt bit to sign that the component number of a scan is 0 or does
 *  not match the sos marker's length when decoding.
 */
#define JPEG_SOS_UNMATCH_ERR_INT_ENA    (BIT(20))
#define JPEG_SOS_UNMATCH_ERR_INT_ENA_M  (JPEG_SOS_UNMATCH_ERR_INT_ENA_V << JPEG_SOS_UNMATCH_ERR_INT_ENA_S)
#define JPEG_SOS_UNMATCH_ERR_INT_ENA_V  0x00000001U
#define JPEG_SOS_UNMATCH_ERR_INT_ENA_S  20
/** JPEG_MARKER_ERR_FST_SCAN_INT_ENA : R/W; bitpos: [21]; default: 0;
 *  The enable interrupt bit to sign that the first scan has header marker error when
 *  decoding.
 */
#define JPEG_MARKER_ERR_FST_SCAN_INT_ENA    (BIT(21))
#define JPEG_MARKER_ERR_FST_SCAN_INT_ENA_M  (JPEG_MARKER_ERR_FST_SCAN_INT_ENA_V << JPEG_MARKER_ERR_FST_SCAN_INT_ENA_S)
#define JPEG_MARKER_ERR_FST_SCAN_INT_ENA_V  0x00000001U
#define JPEG_MARKER_ERR_FST_SCAN_INT_ENA_S  21
/** JPEG_MARKER_ERR_OTHER_SCAN_INT_ENA : R/W; bitpos: [22]; default: 0;
 *  The enable interrupt bit to sign that the following scans but not the first scan
 *  have header marker error when decoding.
 */
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_ENA    (BIT(22))
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_ENA_M  (JPEG_MARKER_ERR_OTHER_SCAN_INT_ENA_V << JPEG_MARKER_ERR_OTHER_SCAN_INT_ENA_S)
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_ENA_V  0x00000001U
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_ENA_S  22
/** JPEG_UNDET_INT_ENA : R/W; bitpos: [23]; default: 0;
 *  The enable interrupt bit to sign that JPEG format is not detected at the eof data
 *  of a packet when decoding.
 */
#define JPEG_UNDET_INT_ENA    (BIT(23))
#define JPEG_UNDET_INT_ENA_M  (JPEG_UNDET_INT_ENA_V << JPEG_UNDET_INT_ENA_S)
#define JPEG_UNDET_INT_ENA_V  0x00000001U
#define JPEG_UNDET_INT_ENA_S  23
/** JPEG_DECODE_TIMEOUT_INT_ENA : R/W; bitpos: [24]; default: 0;
 *  The enable interrupt bit to sign that decode pause time is longer than the setting
 *  decode timeout time when decoding.
 */
#define JPEG_DECODE_TIMEOUT_INT_ENA    (BIT(24))
#define JPEG_DECODE_TIMEOUT_INT_ENA_M  (JPEG_DECODE_TIMEOUT_INT_ENA_V << JPEG_DECODE_TIMEOUT_INT_ENA_S)
#define JPEG_DECODE_TIMEOUT_INT_ENA_V  0x00000001U
#define JPEG_DECODE_TIMEOUT_INT_ENA_S  24

/** JPEG_INT_ST_REG register
 *  Interrupt status registers
 */
#define JPEG_INT_ST_REG (DR_REG_JPEG_BASE + 0x40)
/** JPEG_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  This status interrupt bit turns to high level when JPEG finishes encoding a
 *  picture..
 */
#define JPEG_DONE_INT_ST    (BIT(0))
#define JPEG_DONE_INT_ST_M  (JPEG_DONE_INT_ST_V << JPEG_DONE_INT_ST_S)
#define JPEG_DONE_INT_ST_V  0x00000001U
#define JPEG_DONE_INT_ST_S  0
/** JPEG_RLE_PARALLEL_ERR_INT_ST : RO; bitpos: [1]; default: 0;
 *  The status interrupt bit to sign that rle parallel error when decoding.
 */
#define JPEG_RLE_PARALLEL_ERR_INT_ST    (BIT(1))
#define JPEG_RLE_PARALLEL_ERR_INT_ST_M  (JPEG_RLE_PARALLEL_ERR_INT_ST_V << JPEG_RLE_PARALLEL_ERR_INT_ST_S)
#define JPEG_RLE_PARALLEL_ERR_INT_ST_V  0x00000001U
#define JPEG_RLE_PARALLEL_ERR_INT_ST_S  1
/** JPEG_CID_ERR_INT_ST : RO; bitpos: [2]; default: 0;
 *  The status interrupt bit to sign that scan id check with component fails when
 *  decoding.
 */
#define JPEG_CID_ERR_INT_ST    (BIT(2))
#define JPEG_CID_ERR_INT_ST_M  (JPEG_CID_ERR_INT_ST_V << JPEG_CID_ERR_INT_ST_S)
#define JPEG_CID_ERR_INT_ST_V  0x00000001U
#define JPEG_CID_ERR_INT_ST_S  2
/** JPEG_C_DHT_DC_ID_ERR_INT_ST : RO; bitpos: [3]; default: 0;
 *  The status interrupt bit to sign that scan component's dc dht id check with dc dht
 *  table's id fails when decoding.
 */
#define JPEG_C_DHT_DC_ID_ERR_INT_ST    (BIT(3))
#define JPEG_C_DHT_DC_ID_ERR_INT_ST_M  (JPEG_C_DHT_DC_ID_ERR_INT_ST_V << JPEG_C_DHT_DC_ID_ERR_INT_ST_S)
#define JPEG_C_DHT_DC_ID_ERR_INT_ST_V  0x00000001U
#define JPEG_C_DHT_DC_ID_ERR_INT_ST_S  3
/** JPEG_C_DHT_AC_ID_ERR_INT_ST : RO; bitpos: [4]; default: 0;
 *  The status interrupt bit to sign that scan component's ac dht id check with ac dht
 *  table's id fails when decoding.
 */
#define JPEG_C_DHT_AC_ID_ERR_INT_ST    (BIT(4))
#define JPEG_C_DHT_AC_ID_ERR_INT_ST_M  (JPEG_C_DHT_AC_ID_ERR_INT_ST_V << JPEG_C_DHT_AC_ID_ERR_INT_ST_S)
#define JPEG_C_DHT_AC_ID_ERR_INT_ST_V  0x00000001U
#define JPEG_C_DHT_AC_ID_ERR_INT_ST_S  4
/** JPEG_C_DQT_ID_ERR_INT_ST : RO; bitpos: [5]; default: 0;
 *  The status interrupt bit to sign that scan component's dqt id check with dqt
 *  table's id fails when decoding.
 */
#define JPEG_C_DQT_ID_ERR_INT_ST    (BIT(5))
#define JPEG_C_DQT_ID_ERR_INT_ST_M  (JPEG_C_DQT_ID_ERR_INT_ST_V << JPEG_C_DQT_ID_ERR_INT_ST_S)
#define JPEG_C_DQT_ID_ERR_INT_ST_V  0x00000001U
#define JPEG_C_DQT_ID_ERR_INT_ST_S  5
/** JPEG_RST_UXP_ERR_INT_ST : RO; bitpos: [6]; default: 0;
 *  The status interrupt bit to sign that RST header marker is detected but restart
 *  interval is 0 when decoding.
 */
#define JPEG_RST_UXP_ERR_INT_ST    (BIT(6))
#define JPEG_RST_UXP_ERR_INT_ST_M  (JPEG_RST_UXP_ERR_INT_ST_V << JPEG_RST_UXP_ERR_INT_ST_S)
#define JPEG_RST_UXP_ERR_INT_ST_V  0x00000001U
#define JPEG_RST_UXP_ERR_INT_ST_S  6
/** JPEG_RST_CHECK_NONE_ERR_INT_ST : RO; bitpos: [7]; default: 0;
 *  The status interrupt bit to sign that RST header marker is not detected but restart
 *  interval is not 0 when decoding.
 */
#define JPEG_RST_CHECK_NONE_ERR_INT_ST    (BIT(7))
#define JPEG_RST_CHECK_NONE_ERR_INT_ST_M  (JPEG_RST_CHECK_NONE_ERR_INT_ST_V << JPEG_RST_CHECK_NONE_ERR_INT_ST_S)
#define JPEG_RST_CHECK_NONE_ERR_INT_ST_V  0x00000001U
#define JPEG_RST_CHECK_NONE_ERR_INT_ST_S  7
/** JPEG_RST_CHECK_POS_ERR_INT_ST : RO; bitpos: [8]; default: 0;
 *  The status interrupt bit to sign that RST header marker position mismatches with
 *  restart interval when decoding.
 */
#define JPEG_RST_CHECK_POS_ERR_INT_ST    (BIT(8))
#define JPEG_RST_CHECK_POS_ERR_INT_ST_M  (JPEG_RST_CHECK_POS_ERR_INT_ST_V << JPEG_RST_CHECK_POS_ERR_INT_ST_S)
#define JPEG_RST_CHECK_POS_ERR_INT_ST_V  0x00000001U
#define JPEG_RST_CHECK_POS_ERR_INT_ST_S  8
/** JPEG_OUT_EOF_INT_ST : RO; bitpos: [9]; default: 0;
 *  The status interrupt bit turns to high level when the last pixel of one square has
 *  been transmitted for Tx channel.
 */
#define JPEG_OUT_EOF_INT_ST    (BIT(9))
#define JPEG_OUT_EOF_INT_ST_M  (JPEG_OUT_EOF_INT_ST_V << JPEG_OUT_EOF_INT_ST_S)
#define JPEG_OUT_EOF_INT_ST_V  0x00000001U
#define JPEG_OUT_EOF_INT_ST_S  9
/** JPEG_SR_COLOR_MODE_ERR_INT_ST : RO; bitpos: [10]; default: 0;
 *  The status interrupt bit to sign that the selected source color mode is not
 *  supported.
 */
#define JPEG_SR_COLOR_MODE_ERR_INT_ST    (BIT(10))
#define JPEG_SR_COLOR_MODE_ERR_INT_ST_M  (JPEG_SR_COLOR_MODE_ERR_INT_ST_V << JPEG_SR_COLOR_MODE_ERR_INT_ST_S)
#define JPEG_SR_COLOR_MODE_ERR_INT_ST_V  0x00000001U
#define JPEG_SR_COLOR_MODE_ERR_INT_ST_S  10
/** JPEG_DCT_DONE_INT_ST : RO; bitpos: [11]; default: 0;
 *  The status interrupt bit to sign that one dct calculation is finished.
 */
#define JPEG_DCT_DONE_INT_ST    (BIT(11))
#define JPEG_DCT_DONE_INT_ST_M  (JPEG_DCT_DONE_INT_ST_V << JPEG_DCT_DONE_INT_ST_S)
#define JPEG_DCT_DONE_INT_ST_V  0x00000001U
#define JPEG_DCT_DONE_INT_ST_S  11
/** JPEG_BS_LAST_BLOCK_EOF_INT_ST : RO; bitpos: [12]; default: 0;
 *  The status interrupt bit to sign that the coding process for last block is finished.
 */
#define JPEG_BS_LAST_BLOCK_EOF_INT_ST    (BIT(12))
#define JPEG_BS_LAST_BLOCK_EOF_INT_ST_M  (JPEG_BS_LAST_BLOCK_EOF_INT_ST_V << JPEG_BS_LAST_BLOCK_EOF_INT_ST_S)
#define JPEG_BS_LAST_BLOCK_EOF_INT_ST_V  0x00000001U
#define JPEG_BS_LAST_BLOCK_EOF_INT_ST_S  12
/** JPEG_SCAN_CHECK_NONE_ERR_INT_ST : RO; bitpos: [13]; default: 0;
 *  The status interrupt bit to sign that SOS header marker is not detected but there
 *  are still components left to be decoded.
 */
#define JPEG_SCAN_CHECK_NONE_ERR_INT_ST    (BIT(13))
#define JPEG_SCAN_CHECK_NONE_ERR_INT_ST_M  (JPEG_SCAN_CHECK_NONE_ERR_INT_ST_V << JPEG_SCAN_CHECK_NONE_ERR_INT_ST_S)
#define JPEG_SCAN_CHECK_NONE_ERR_INT_ST_V  0x00000001U
#define JPEG_SCAN_CHECK_NONE_ERR_INT_ST_S  13
/** JPEG_SCAN_CHECK_POS_ERR_INT_ST : RO; bitpos: [14]; default: 0;
 *  The status interrupt bit to sign that SOS header marker position wrong when
 *  decoding.
 */
#define JPEG_SCAN_CHECK_POS_ERR_INT_ST    (BIT(14))
#define JPEG_SCAN_CHECK_POS_ERR_INT_ST_M  (JPEG_SCAN_CHECK_POS_ERR_INT_ST_V << JPEG_SCAN_CHECK_POS_ERR_INT_ST_S)
#define JPEG_SCAN_CHECK_POS_ERR_INT_ST_V  0x00000001U
#define JPEG_SCAN_CHECK_POS_ERR_INT_ST_S  14
/** JPEG_UXP_DET_INT_ST : RO; bitpos: [15]; default: 0;
 *  The status interrupt bit to sign that unsupported header marker is detected when
 *  decoding.
 */
#define JPEG_UXP_DET_INT_ST    (BIT(15))
#define JPEG_UXP_DET_INT_ST_M  (JPEG_UXP_DET_INT_ST_V << JPEG_UXP_DET_INT_ST_S)
#define JPEG_UXP_DET_INT_ST_V  0x00000001U
#define JPEG_UXP_DET_INT_ST_S  15
/** JPEG_EN_FRAME_EOF_ERR_INT_ST : RO; bitpos: [16]; default: 0;
 *  The status interrupt bit to sign that received pixel blocks are smaller than
 *  expected when encoding.
 */
#define JPEG_EN_FRAME_EOF_ERR_INT_ST    (BIT(16))
#define JPEG_EN_FRAME_EOF_ERR_INT_ST_M  (JPEG_EN_FRAME_EOF_ERR_INT_ST_V << JPEG_EN_FRAME_EOF_ERR_INT_ST_S)
#define JPEG_EN_FRAME_EOF_ERR_INT_ST_V  0x00000001U
#define JPEG_EN_FRAME_EOF_ERR_INT_ST_S  16
/** JPEG_EN_FRAME_EOF_LACK_INT_ST : RO; bitpos: [17]; default: 0;
 *  The status interrupt bit to sign that the frame eof sign bit from dma input is
 *  missing when encoding. But the number of pixel blocks is enough.
 */
#define JPEG_EN_FRAME_EOF_LACK_INT_ST    (BIT(17))
#define JPEG_EN_FRAME_EOF_LACK_INT_ST_M  (JPEG_EN_FRAME_EOF_LACK_INT_ST_V << JPEG_EN_FRAME_EOF_LACK_INT_ST_S)
#define JPEG_EN_FRAME_EOF_LACK_INT_ST_V  0x00000001U
#define JPEG_EN_FRAME_EOF_LACK_INT_ST_S  17
/** JPEG_DE_FRAME_EOF_ERR_INT_ST : RO; bitpos: [18]; default: 0;
 *  The status interrupt bit to sign that decoded blocks are smaller than expected when
 *  decoding.
 */
#define JPEG_DE_FRAME_EOF_ERR_INT_ST    (BIT(18))
#define JPEG_DE_FRAME_EOF_ERR_INT_ST_M  (JPEG_DE_FRAME_EOF_ERR_INT_ST_V << JPEG_DE_FRAME_EOF_ERR_INT_ST_S)
#define JPEG_DE_FRAME_EOF_ERR_INT_ST_V  0x00000001U
#define JPEG_DE_FRAME_EOF_ERR_INT_ST_S  18
/** JPEG_DE_FRAME_EOF_LACK_INT_ST : RO; bitpos: [19]; default: 0;
 *  The status interrupt bit to sign that the either frame eof from dma input or eoi
 *  marker is missing when encoding. But the number of decoded blocks is enough.
 */
#define JPEG_DE_FRAME_EOF_LACK_INT_ST    (BIT(19))
#define JPEG_DE_FRAME_EOF_LACK_INT_ST_M  (JPEG_DE_FRAME_EOF_LACK_INT_ST_V << JPEG_DE_FRAME_EOF_LACK_INT_ST_S)
#define JPEG_DE_FRAME_EOF_LACK_INT_ST_V  0x00000001U
#define JPEG_DE_FRAME_EOF_LACK_INT_ST_S  19
/** JPEG_SOS_UNMATCH_ERR_INT_ST : RO; bitpos: [20]; default: 0;
 *  The status interrupt bit to sign that the component number of a scan is 0 or does
 *  not match the sos marker's length when decoding.
 */
#define JPEG_SOS_UNMATCH_ERR_INT_ST    (BIT(20))
#define JPEG_SOS_UNMATCH_ERR_INT_ST_M  (JPEG_SOS_UNMATCH_ERR_INT_ST_V << JPEG_SOS_UNMATCH_ERR_INT_ST_S)
#define JPEG_SOS_UNMATCH_ERR_INT_ST_V  0x00000001U
#define JPEG_SOS_UNMATCH_ERR_INT_ST_S  20
/** JPEG_MARKER_ERR_FST_SCAN_INT_ST : RO; bitpos: [21]; default: 0;
 *  The status interrupt bit to sign that the first scan has header marker error when
 *  decoding.
 */
#define JPEG_MARKER_ERR_FST_SCAN_INT_ST    (BIT(21))
#define JPEG_MARKER_ERR_FST_SCAN_INT_ST_M  (JPEG_MARKER_ERR_FST_SCAN_INT_ST_V << JPEG_MARKER_ERR_FST_SCAN_INT_ST_S)
#define JPEG_MARKER_ERR_FST_SCAN_INT_ST_V  0x00000001U
#define JPEG_MARKER_ERR_FST_SCAN_INT_ST_S  21
/** JPEG_MARKER_ERR_OTHER_SCAN_INT_ST : RO; bitpos: [22]; default: 0;
 *  The status interrupt bit to sign that the following scans but not the first scan
 *  have header marker error when decoding.
 */
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_ST    (BIT(22))
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_ST_M  (JPEG_MARKER_ERR_OTHER_SCAN_INT_ST_V << JPEG_MARKER_ERR_OTHER_SCAN_INT_ST_S)
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_ST_V  0x00000001U
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_ST_S  22
/** JPEG_UNDET_INT_ST : RO; bitpos: [23]; default: 0;
 *  The status interrupt bit to sign that JPEG format is not detected at the eof data
 *  of a packet when decoding.
 */
#define JPEG_UNDET_INT_ST    (BIT(23))
#define JPEG_UNDET_INT_ST_M  (JPEG_UNDET_INT_ST_V << JPEG_UNDET_INT_ST_S)
#define JPEG_UNDET_INT_ST_V  0x00000001U
#define JPEG_UNDET_INT_ST_S  23
/** JPEG_DECODE_TIMEOUT_INT_ST : RO; bitpos: [24]; default: 0;
 *  The status interrupt bit to sign that decode pause time is longer than the setting
 *  decode timeout time when decoding.
 */
#define JPEG_DECODE_TIMEOUT_INT_ST    (BIT(24))
#define JPEG_DECODE_TIMEOUT_INT_ST_M  (JPEG_DECODE_TIMEOUT_INT_ST_V << JPEG_DECODE_TIMEOUT_INT_ST_S)
#define JPEG_DECODE_TIMEOUT_INT_ST_V  0x00000001U
#define JPEG_DECODE_TIMEOUT_INT_ST_S  24

/** JPEG_INT_CLR_REG register
 *  Interrupt clear registers
 */
#define JPEG_INT_CLR_REG (DR_REG_JPEG_BASE + 0x44)
/** JPEG_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  This clear interrupt bit turns to high level when JPEG finishes encoding a picture..
 */
#define JPEG_DONE_INT_CLR    (BIT(0))
#define JPEG_DONE_INT_CLR_M  (JPEG_DONE_INT_CLR_V << JPEG_DONE_INT_CLR_S)
#define JPEG_DONE_INT_CLR_V  0x00000001U
#define JPEG_DONE_INT_CLR_S  0
/** JPEG_RLE_PARALLEL_ERR_INT_CLR : WT; bitpos: [1]; default: 0;
 *  The clear interrupt bit to sign that rle parallel error when decoding.
 */
#define JPEG_RLE_PARALLEL_ERR_INT_CLR    (BIT(1))
#define JPEG_RLE_PARALLEL_ERR_INT_CLR_M  (JPEG_RLE_PARALLEL_ERR_INT_CLR_V << JPEG_RLE_PARALLEL_ERR_INT_CLR_S)
#define JPEG_RLE_PARALLEL_ERR_INT_CLR_V  0x00000001U
#define JPEG_RLE_PARALLEL_ERR_INT_CLR_S  1
/** JPEG_CID_ERR_INT_CLR : WT; bitpos: [2]; default: 0;
 *  The clear interrupt bit to sign that scan id check with component fails when
 *  decoding.
 */
#define JPEG_CID_ERR_INT_CLR    (BIT(2))
#define JPEG_CID_ERR_INT_CLR_M  (JPEG_CID_ERR_INT_CLR_V << JPEG_CID_ERR_INT_CLR_S)
#define JPEG_CID_ERR_INT_CLR_V  0x00000001U
#define JPEG_CID_ERR_INT_CLR_S  2
/** JPEG_C_DHT_DC_ID_ERR_INT_CLR : WT; bitpos: [3]; default: 0;
 *  The clear interrupt bit to sign that scan component's dc dht id check with dc dht
 *  table's id fails when decoding.
 */
#define JPEG_C_DHT_DC_ID_ERR_INT_CLR    (BIT(3))
#define JPEG_C_DHT_DC_ID_ERR_INT_CLR_M  (JPEG_C_DHT_DC_ID_ERR_INT_CLR_V << JPEG_C_DHT_DC_ID_ERR_INT_CLR_S)
#define JPEG_C_DHT_DC_ID_ERR_INT_CLR_V  0x00000001U
#define JPEG_C_DHT_DC_ID_ERR_INT_CLR_S  3
/** JPEG_C_DHT_AC_ID_ERR_INT_CLR : WT; bitpos: [4]; default: 0;
 *  The clear interrupt bit to sign that scan component's ac dht id check with ac dht
 *  table's id fails when decoding.
 */
#define JPEG_C_DHT_AC_ID_ERR_INT_CLR    (BIT(4))
#define JPEG_C_DHT_AC_ID_ERR_INT_CLR_M  (JPEG_C_DHT_AC_ID_ERR_INT_CLR_V << JPEG_C_DHT_AC_ID_ERR_INT_CLR_S)
#define JPEG_C_DHT_AC_ID_ERR_INT_CLR_V  0x00000001U
#define JPEG_C_DHT_AC_ID_ERR_INT_CLR_S  4
/** JPEG_C_DQT_ID_ERR_INT_CLR : WT; bitpos: [5]; default: 0;
 *  The clear interrupt bit to sign that scan component's dqt id check with dqt table's
 *  id fails when decoding.
 */
#define JPEG_C_DQT_ID_ERR_INT_CLR    (BIT(5))
#define JPEG_C_DQT_ID_ERR_INT_CLR_M  (JPEG_C_DQT_ID_ERR_INT_CLR_V << JPEG_C_DQT_ID_ERR_INT_CLR_S)
#define JPEG_C_DQT_ID_ERR_INT_CLR_V  0x00000001U
#define JPEG_C_DQT_ID_ERR_INT_CLR_S  5
/** JPEG_RST_UXP_ERR_INT_CLR : WT; bitpos: [6]; default: 0;
 *  The clear interrupt bit to sign that RST header marker is detected but restart
 *  interval is 0 when decoding.
 */
#define JPEG_RST_UXP_ERR_INT_CLR    (BIT(6))
#define JPEG_RST_UXP_ERR_INT_CLR_M  (JPEG_RST_UXP_ERR_INT_CLR_V << JPEG_RST_UXP_ERR_INT_CLR_S)
#define JPEG_RST_UXP_ERR_INT_CLR_V  0x00000001U
#define JPEG_RST_UXP_ERR_INT_CLR_S  6
/** JPEG_RST_CHECK_NONE_ERR_INT_CLR : WT; bitpos: [7]; default: 0;
 *  The clear interrupt bit to sign that RST header marker is not detected but restart
 *  interval is not 0 when decoding.
 */
#define JPEG_RST_CHECK_NONE_ERR_INT_CLR    (BIT(7))
#define JPEG_RST_CHECK_NONE_ERR_INT_CLR_M  (JPEG_RST_CHECK_NONE_ERR_INT_CLR_V << JPEG_RST_CHECK_NONE_ERR_INT_CLR_S)
#define JPEG_RST_CHECK_NONE_ERR_INT_CLR_V  0x00000001U
#define JPEG_RST_CHECK_NONE_ERR_INT_CLR_S  7
/** JPEG_RST_CHECK_POS_ERR_INT_CLR : WT; bitpos: [8]; default: 0;
 *  The clear interrupt bit to sign that RST header marker position mismatches with
 *  restart interval when decoding.
 */
#define JPEG_RST_CHECK_POS_ERR_INT_CLR    (BIT(8))
#define JPEG_RST_CHECK_POS_ERR_INT_CLR_M  (JPEG_RST_CHECK_POS_ERR_INT_CLR_V << JPEG_RST_CHECK_POS_ERR_INT_CLR_S)
#define JPEG_RST_CHECK_POS_ERR_INT_CLR_V  0x00000001U
#define JPEG_RST_CHECK_POS_ERR_INT_CLR_S  8
/** JPEG_OUT_EOF_INT_CLR : WT; bitpos: [9]; default: 0;
 *  The clear interrupt bit turns to high level when the last pixel of one square has
 *  been transmitted for Tx channel.
 */
#define JPEG_OUT_EOF_INT_CLR    (BIT(9))
#define JPEG_OUT_EOF_INT_CLR_M  (JPEG_OUT_EOF_INT_CLR_V << JPEG_OUT_EOF_INT_CLR_S)
#define JPEG_OUT_EOF_INT_CLR_V  0x00000001U
#define JPEG_OUT_EOF_INT_CLR_S  9
/** JPEG_SR_COLOR_MODE_ERR_INT_CLR : WT; bitpos: [10]; default: 0;
 *  The clear interrupt bit to sign that the selected source color mode is not
 *  supported.
 */
#define JPEG_SR_COLOR_MODE_ERR_INT_CLR    (BIT(10))
#define JPEG_SR_COLOR_MODE_ERR_INT_CLR_M  (JPEG_SR_COLOR_MODE_ERR_INT_CLR_V << JPEG_SR_COLOR_MODE_ERR_INT_CLR_S)
#define JPEG_SR_COLOR_MODE_ERR_INT_CLR_V  0x00000001U
#define JPEG_SR_COLOR_MODE_ERR_INT_CLR_S  10
/** JPEG_DCT_DONE_INT_CLR : WT; bitpos: [11]; default: 0;
 *  The clear interrupt bit to sign that one dct calculation is finished.
 */
#define JPEG_DCT_DONE_INT_CLR    (BIT(11))
#define JPEG_DCT_DONE_INT_CLR_M  (JPEG_DCT_DONE_INT_CLR_V << JPEG_DCT_DONE_INT_CLR_S)
#define JPEG_DCT_DONE_INT_CLR_V  0x00000001U
#define JPEG_DCT_DONE_INT_CLR_S  11
/** JPEG_BS_LAST_BLOCK_EOF_INT_CLR : WT; bitpos: [12]; default: 0;
 *  The clear interrupt bit to sign that the coding process for last block is finished.
 */
#define JPEG_BS_LAST_BLOCK_EOF_INT_CLR    (BIT(12))
#define JPEG_BS_LAST_BLOCK_EOF_INT_CLR_M  (JPEG_BS_LAST_BLOCK_EOF_INT_CLR_V << JPEG_BS_LAST_BLOCK_EOF_INT_CLR_S)
#define JPEG_BS_LAST_BLOCK_EOF_INT_CLR_V  0x00000001U
#define JPEG_BS_LAST_BLOCK_EOF_INT_CLR_S  12
/** JPEG_SCAN_CHECK_NONE_ERR_INT_CLR : WT; bitpos: [13]; default: 0;
 *  The clear interrupt bit to sign that SOS header marker is not detected but there
 *  are still components left to be decoded.
 */
#define JPEG_SCAN_CHECK_NONE_ERR_INT_CLR    (BIT(13))
#define JPEG_SCAN_CHECK_NONE_ERR_INT_CLR_M  (JPEG_SCAN_CHECK_NONE_ERR_INT_CLR_V << JPEG_SCAN_CHECK_NONE_ERR_INT_CLR_S)
#define JPEG_SCAN_CHECK_NONE_ERR_INT_CLR_V  0x00000001U
#define JPEG_SCAN_CHECK_NONE_ERR_INT_CLR_S  13
/** JPEG_SCAN_CHECK_POS_ERR_INT_CLR : WT; bitpos: [14]; default: 0;
 *  The clear interrupt bit to sign that SOS header marker position wrong when decoding.
 */
#define JPEG_SCAN_CHECK_POS_ERR_INT_CLR    (BIT(14))
#define JPEG_SCAN_CHECK_POS_ERR_INT_CLR_M  (JPEG_SCAN_CHECK_POS_ERR_INT_CLR_V << JPEG_SCAN_CHECK_POS_ERR_INT_CLR_S)
#define JPEG_SCAN_CHECK_POS_ERR_INT_CLR_V  0x00000001U
#define JPEG_SCAN_CHECK_POS_ERR_INT_CLR_S  14
/** JPEG_UXP_DET_INT_CLR : WT; bitpos: [15]; default: 0;
 *  The clear interrupt bit to sign that unsupported header marker is detected when
 *  decoding.
 */
#define JPEG_UXP_DET_INT_CLR    (BIT(15))
#define JPEG_UXP_DET_INT_CLR_M  (JPEG_UXP_DET_INT_CLR_V << JPEG_UXP_DET_INT_CLR_S)
#define JPEG_UXP_DET_INT_CLR_V  0x00000001U
#define JPEG_UXP_DET_INT_CLR_S  15
/** JPEG_EN_FRAME_EOF_ERR_INT_CLR : WT; bitpos: [16]; default: 0;
 *  The clear interrupt bit to sign that received pixel blocks are smaller than
 *  expected when encoding.
 */
#define JPEG_EN_FRAME_EOF_ERR_INT_CLR    (BIT(16))
#define JPEG_EN_FRAME_EOF_ERR_INT_CLR_M  (JPEG_EN_FRAME_EOF_ERR_INT_CLR_V << JPEG_EN_FRAME_EOF_ERR_INT_CLR_S)
#define JPEG_EN_FRAME_EOF_ERR_INT_CLR_V  0x00000001U
#define JPEG_EN_FRAME_EOF_ERR_INT_CLR_S  16
/** JPEG_EN_FRAME_EOF_LACK_INT_CLR : WT; bitpos: [17]; default: 0;
 *  The clear interrupt bit to sign that the frame eof sign bit from dma input is
 *  missing when encoding. But the number of pixel blocks is enough.
 */
#define JPEG_EN_FRAME_EOF_LACK_INT_CLR    (BIT(17))
#define JPEG_EN_FRAME_EOF_LACK_INT_CLR_M  (JPEG_EN_FRAME_EOF_LACK_INT_CLR_V << JPEG_EN_FRAME_EOF_LACK_INT_CLR_S)
#define JPEG_EN_FRAME_EOF_LACK_INT_CLR_V  0x00000001U
#define JPEG_EN_FRAME_EOF_LACK_INT_CLR_S  17
/** JPEG_DE_FRAME_EOF_ERR_INT_CLR : WT; bitpos: [18]; default: 0;
 *  The clear interrupt bit to sign that decoded blocks are smaller than expected when
 *  decoding.
 */
#define JPEG_DE_FRAME_EOF_ERR_INT_CLR    (BIT(18))
#define JPEG_DE_FRAME_EOF_ERR_INT_CLR_M  (JPEG_DE_FRAME_EOF_ERR_INT_CLR_V << JPEG_DE_FRAME_EOF_ERR_INT_CLR_S)
#define JPEG_DE_FRAME_EOF_ERR_INT_CLR_V  0x00000001U
#define JPEG_DE_FRAME_EOF_ERR_INT_CLR_S  18
/** JPEG_DE_FRAME_EOF_LACK_INT_CLR : WT; bitpos: [19]; default: 0;
 *  The clear interrupt bit to sign that the either frame eof from dma input or eoi
 *  marker is missing when encoding. But the number of decoded blocks is enough.
 */
#define JPEG_DE_FRAME_EOF_LACK_INT_CLR    (BIT(19))
#define JPEG_DE_FRAME_EOF_LACK_INT_CLR_M  (JPEG_DE_FRAME_EOF_LACK_INT_CLR_V << JPEG_DE_FRAME_EOF_LACK_INT_CLR_S)
#define JPEG_DE_FRAME_EOF_LACK_INT_CLR_V  0x00000001U
#define JPEG_DE_FRAME_EOF_LACK_INT_CLR_S  19
/** JPEG_SOS_UNMATCH_ERR_INT_CLR : WT; bitpos: [20]; default: 0;
 *  The clear interrupt bit to sign that the component number of a scan is 0 or does
 *  not match the sos marker's length when decoding.
 */
#define JPEG_SOS_UNMATCH_ERR_INT_CLR    (BIT(20))
#define JPEG_SOS_UNMATCH_ERR_INT_CLR_M  (JPEG_SOS_UNMATCH_ERR_INT_CLR_V << JPEG_SOS_UNMATCH_ERR_INT_CLR_S)
#define JPEG_SOS_UNMATCH_ERR_INT_CLR_V  0x00000001U
#define JPEG_SOS_UNMATCH_ERR_INT_CLR_S  20
/** JPEG_MARKER_ERR_FST_SCAN_INT_CLR : WT; bitpos: [21]; default: 0;
 *  The clear interrupt bit to sign that the first scan has header marker error when
 *  decoding.
 */
#define JPEG_MARKER_ERR_FST_SCAN_INT_CLR    (BIT(21))
#define JPEG_MARKER_ERR_FST_SCAN_INT_CLR_M  (JPEG_MARKER_ERR_FST_SCAN_INT_CLR_V << JPEG_MARKER_ERR_FST_SCAN_INT_CLR_S)
#define JPEG_MARKER_ERR_FST_SCAN_INT_CLR_V  0x00000001U
#define JPEG_MARKER_ERR_FST_SCAN_INT_CLR_S  21
/** JPEG_MARKER_ERR_OTHER_SCAN_INT_CLR : WT; bitpos: [22]; default: 0;
 *  The clear interrupt bit to sign that the following scans but not the first scan
 *  have header marker error when decoding.
 */
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_CLR    (BIT(22))
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_CLR_M  (JPEG_MARKER_ERR_OTHER_SCAN_INT_CLR_V << JPEG_MARKER_ERR_OTHER_SCAN_INT_CLR_S)
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_CLR_V  0x00000001U
#define JPEG_MARKER_ERR_OTHER_SCAN_INT_CLR_S  22
/** JPEG_UNDET_INT_CLR : WT; bitpos: [23]; default: 0;
 *  The clear interrupt bit to sign that JPEG format is not detected at the eof data of
 *  a packet when decoding.
 */
#define JPEG_UNDET_INT_CLR    (BIT(23))
#define JPEG_UNDET_INT_CLR_M  (JPEG_UNDET_INT_CLR_V << JPEG_UNDET_INT_CLR_S)
#define JPEG_UNDET_INT_CLR_V  0x00000001U
#define JPEG_UNDET_INT_CLR_S  23
/** JPEG_DECODE_TIMEOUT_INT_CLR : WT; bitpos: [24]; default: 0;
 *  The clear interrupt bit to sign that decode pause time is longer than the setting
 *  decode timeout time when decoding.
 */
#define JPEG_DECODE_TIMEOUT_INT_CLR    (BIT(24))
#define JPEG_DECODE_TIMEOUT_INT_CLR_M  (JPEG_DECODE_TIMEOUT_INT_CLR_V << JPEG_DECODE_TIMEOUT_INT_CLR_S)
#define JPEG_DECODE_TIMEOUT_INT_CLR_V  0x00000001U
#define JPEG_DECODE_TIMEOUT_INT_CLR_S  24

/** JPEG_STATUS0_REG register
 *  Trace and Debug registers
 */
#define JPEG_STATUS0_REG (DR_REG_JPEG_BASE + 0x48)
/** JPEG_BITSTREAM_EOF_VLD_CNT : RO; bitpos: [16:11]; default: 0;
 *  the valid bit count for last bitstream
 */
#define JPEG_BITSTREAM_EOF_VLD_CNT    0x0000003FU
#define JPEG_BITSTREAM_EOF_VLD_CNT_M  (JPEG_BITSTREAM_EOF_VLD_CNT_V << JPEG_BITSTREAM_EOF_VLD_CNT_S)
#define JPEG_BITSTREAM_EOF_VLD_CNT_V  0x0000003FU
#define JPEG_BITSTREAM_EOF_VLD_CNT_S  11
/** JPEG_DCTOUT_ZZSCAN_ADDR : RO; bitpos: [22:17]; default: 0;
 *  the zig-zag read addr from dctout_ram
 */
#define JPEG_DCTOUT_ZZSCAN_ADDR    0x0000003FU
#define JPEG_DCTOUT_ZZSCAN_ADDR_M  (JPEG_DCTOUT_ZZSCAN_ADDR_V << JPEG_DCTOUT_ZZSCAN_ADDR_S)
#define JPEG_DCTOUT_ZZSCAN_ADDR_V  0x0000003FU
#define JPEG_DCTOUT_ZZSCAN_ADDR_S  17
/** JPEG_QNRVAL_ZZSCAN_ADDR : RO; bitpos: [28:23]; default: 0;
 *  the zig-zag read addr from qnrval_ram
 */
#define JPEG_QNRVAL_ZZSCAN_ADDR    0x0000003FU
#define JPEG_QNRVAL_ZZSCAN_ADDR_M  (JPEG_QNRVAL_ZZSCAN_ADDR_V << JPEG_QNRVAL_ZZSCAN_ADDR_S)
#define JPEG_QNRVAL_ZZSCAN_ADDR_V  0x0000003FU
#define JPEG_QNRVAL_ZZSCAN_ADDR_S  23
/** JPEG_REG_STATE_YUV : RO; bitpos: [31:29]; default: 0;
 *  the state of jpeg fsm
 */
#define JPEG_REG_STATE_YUV    0x00000007U
#define JPEG_REG_STATE_YUV_M  (JPEG_REG_STATE_YUV_V << JPEG_REG_STATE_YUV_S)
#define JPEG_REG_STATE_YUV_V  0x00000007U
#define JPEG_REG_STATE_YUV_S  29

/** JPEG_STATUS2_REG register
 *  Trace and Debug registers
 */
#define JPEG_STATUS2_REG (DR_REG_JPEG_BASE + 0x4c)
/** JPEG_SOURCE_PIXEL : RO; bitpos: [23:0]; default: 0;
 *  source pixels fetched from dma
 */
#define JPEG_SOURCE_PIXEL    0x00FFFFFFU
#define JPEG_SOURCE_PIXEL_M  (JPEG_SOURCE_PIXEL_V << JPEG_SOURCE_PIXEL_S)
#define JPEG_SOURCE_PIXEL_V  0x00FFFFFFU
#define JPEG_SOURCE_PIXEL_S  0
/** JPEG_LAST_BLOCK : RO; bitpos: [24]; default: 0;
 *  indicate the encoding process for the last mcu of the picture
 */
#define JPEG_LAST_BLOCK    (BIT(24))
#define JPEG_LAST_BLOCK_M  (JPEG_LAST_BLOCK_V << JPEG_LAST_BLOCK_S)
#define JPEG_LAST_BLOCK_V  0x00000001U
#define JPEG_LAST_BLOCK_S  24
/** JPEG_LAST_MCU : RO; bitpos: [25]; default: 0;
 *  indicate the encoding process for the last block of the picture
 */
#define JPEG_LAST_MCU    (BIT(25))
#define JPEG_LAST_MCU_M  (JPEG_LAST_MCU_V << JPEG_LAST_MCU_S)
#define JPEG_LAST_MCU_V  0x00000001U
#define JPEG_LAST_MCU_S  25
/** JPEG_LAST_DC : RO; bitpos: [26]; default: 0;
 *  indicate the encoding process is at the header of the last block of the picture
 */
#define JPEG_LAST_DC    (BIT(26))
#define JPEG_LAST_DC_M  (JPEG_LAST_DC_V << JPEG_LAST_DC_S)
#define JPEG_LAST_DC_V  0x00000001U
#define JPEG_LAST_DC_S  26
/** JPEG_PACKFIFO_READY : RO; bitpos: [27]; default: 1;
 *  the jpeg pack_fifo ready signal, high active
 */
#define JPEG_PACKFIFO_READY    (BIT(27))
#define JPEG_PACKFIFO_READY_M  (JPEG_PACKFIFO_READY_V << JPEG_PACKFIFO_READY_S)
#define JPEG_PACKFIFO_READY_V  0x00000001U
#define JPEG_PACKFIFO_READY_S  27

/** JPEG_STATUS3_REG register
 *  Trace and Debug registers
 */
#define JPEG_STATUS3_REG (DR_REG_JPEG_BASE + 0x50)
/** JPEG_YO : RO; bitpos: [8:0]; default: 0;
 *  component y transferred from rgb input
 */
#define JPEG_YO    0x000001FFU
#define JPEG_YO_M  (JPEG_YO_V << JPEG_YO_S)
#define JPEG_YO_V  0x000001FFU
#define JPEG_YO_S  0
/** JPEG_Y_READY : RO; bitpos: [9]; default: 0;
 *  component y valid signal, high active
 */
#define JPEG_Y_READY    (BIT(9))
#define JPEG_Y_READY_M  (JPEG_Y_READY_V << JPEG_Y_READY_S)
#define JPEG_Y_READY_V  0x00000001U
#define JPEG_Y_READY_S  9
/** JPEG_CBO : RO; bitpos: [18:10]; default: 0;
 *  component cb transferred from rgb input
 */
#define JPEG_CBO    0x000001FFU
#define JPEG_CBO_M  (JPEG_CBO_V << JPEG_CBO_S)
#define JPEG_CBO_V  0x000001FFU
#define JPEG_CBO_S  10
/** JPEG_CB_READY : RO; bitpos: [19]; default: 0;
 *  component cb valid signal, high active
 */
#define JPEG_CB_READY    (BIT(19))
#define JPEG_CB_READY_M  (JPEG_CB_READY_V << JPEG_CB_READY_S)
#define JPEG_CB_READY_V  0x00000001U
#define JPEG_CB_READY_S  19
/** JPEG_CRO : RO; bitpos: [28:20]; default: 0;
 *  component cr transferred from rgb input
 */
#define JPEG_CRO    0x000001FFU
#define JPEG_CRO_M  (JPEG_CRO_V << JPEG_CRO_S)
#define JPEG_CRO_V  0x000001FFU
#define JPEG_CRO_S  20
/** JPEG_CR_READY : RO; bitpos: [29]; default: 0;
 *  component cr valid signal, high active
 */
#define JPEG_CR_READY    (BIT(29))
#define JPEG_CR_READY_M  (JPEG_CR_READY_V << JPEG_CR_READY_S)
#define JPEG_CR_READY_V  0x00000001U
#define JPEG_CR_READY_S  29

/** JPEG_STATUS4_REG register
 *  Trace and Debug registers
 */
#define JPEG_STATUS4_REG (DR_REG_JPEG_BASE + 0x54)
/** JPEG_HFM_BITSTREAM : RO; bitpos: [31:0]; default: 0;
 *  the hufman bitstream during encoding process
 */
#define JPEG_HFM_BITSTREAM    0xFFFFFFFFU
#define JPEG_HFM_BITSTREAM_M  (JPEG_HFM_BITSTREAM_V << JPEG_HFM_BITSTREAM_S)
#define JPEG_HFM_BITSTREAM_V  0xFFFFFFFFU
#define JPEG_HFM_BITSTREAM_S  0

/** JPEG_DHT_TOTLEN_DC0_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_TOTLEN_DC0_REG (DR_REG_JPEG_BASE + 0x58)
/** JPEG_DHT_TOTLEN_DC0 : HRO; bitpos: [31:0]; default: 0;
 *  write the numbers of 1~n codeword length sum from 1~16 of dc0 table
 */
#define JPEG_DHT_TOTLEN_DC0    0xFFFFFFFFU
#define JPEG_DHT_TOTLEN_DC0_M  (JPEG_DHT_TOTLEN_DC0_V << JPEG_DHT_TOTLEN_DC0_S)
#define JPEG_DHT_TOTLEN_DC0_V  0xFFFFFFFFU
#define JPEG_DHT_TOTLEN_DC0_S  0

/** JPEG_DHT_VAl_DC0_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_VAL_DC0_REG (DR_REG_JPEG_BASE + 0x5c)
/** JPEG_DHT_VAL_DC0 : HRO; bitpos: [31:0]; default: 0;
 *  write codeword corresponding huffman values of dc0 table
 */
#define JPEG_DHT_VAL_DC0    0xFFFFFFFFU
#define JPEG_DHT_VAL_DC0_M  (JPEG_DHT_VAL_DC0_V << JPEG_DHT_VAL_DC0_S)
#define JPEG_DHT_VAL_DC0_V  0xFFFFFFFFU
#define JPEG_DHT_VAL_DC0_S  0

/** JPEG_DHT_TOTLEN_AC0_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_TOTLEN_AC0_REG (DR_REG_JPEG_BASE + 0x60)
/** JPEG_DHT_TOTLEN_AC0 : HRO; bitpos: [31:0]; default: 0;
 *  write the numbers of 1~n codeword length sum from 1~16 of ac0 table
 */
#define JPEG_DHT_TOTLEN_AC0    0xFFFFFFFFU
#define JPEG_DHT_TOTLEN_AC0_M  (JPEG_DHT_TOTLEN_AC0_V << JPEG_DHT_TOTLEN_AC0_S)
#define JPEG_DHT_TOTLEN_AC0_V  0xFFFFFFFFU
#define JPEG_DHT_TOTLEN_AC0_S  0

/** JPEG_DHT_VAl_AC0_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_VAL_AC0_REG (DR_REG_JPEG_BASE + 0x64)
/** JPEG_DHT_VAL_AC0 : HRO; bitpos: [31:0]; default: 0;
 *  write codeword corresponding huffman values of ac0 table
 */
#define JPEG_DHT_VAL_AC0    0xFFFFFFFFU
#define JPEG_DHT_VAL_AC0_M  (JPEG_DHT_VAL_AC0_V << JPEG_DHT_VAL_AC0_S)
#define JPEG_DHT_VAL_AC0_V  0xFFFFFFFFU
#define JPEG_DHT_VAL_AC0_S  0

/** JPEG_DHT_TOTLEN_DC1_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_TOTLEN_DC1_REG (DR_REG_JPEG_BASE + 0x68)
/** JPEG_DHT_TOTLEN_DC1 : HRO; bitpos: [31:0]; default: 0;
 *  write the numbers of 1~n codeword length sum from 1~16 of dc1 table
 */
#define JPEG_DHT_TOTLEN_DC1    0xFFFFFFFFU
#define JPEG_DHT_TOTLEN_DC1_M  (JPEG_DHT_TOTLEN_DC1_V << JPEG_DHT_TOTLEN_DC1_S)
#define JPEG_DHT_TOTLEN_DC1_V  0xFFFFFFFFU
#define JPEG_DHT_TOTLEN_DC1_S  0

/** JPEG_DHT_VAl_DC1_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_VAL_DC1_REG (DR_REG_JPEG_BASE + 0x6c)
/** JPEG_DHT_VAL_DC1 : HRO; bitpos: [31:0]; default: 0;
 *  write codeword corresponding huffman values of dc1 table
 */
#define JPEG_DHT_VAL_DC1    0xFFFFFFFFU
#define JPEG_DHT_VAL_DC1_M  (JPEG_DHT_VAL_DC1_V << JPEG_DHT_VAL_DC1_S)
#define JPEG_DHT_VAL_DC1_V  0xFFFFFFFFU
#define JPEG_DHT_VAL_DC1_S  0

/** JPEG_DHT_TOTLEN_AC1_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_TOTLEN_AC1_REG (DR_REG_JPEG_BASE + 0x70)
/** JPEG_DHT_TOTLEN_AC1 : HRO; bitpos: [31:0]; default: 0;
 *  write the numbers of 1~n codeword length sum from 1~16 of ac1 table
 */
#define JPEG_DHT_TOTLEN_AC1    0xFFFFFFFFU
#define JPEG_DHT_TOTLEN_AC1_M  (JPEG_DHT_TOTLEN_AC1_V << JPEG_DHT_TOTLEN_AC1_S)
#define JPEG_DHT_TOTLEN_AC1_V  0xFFFFFFFFU
#define JPEG_DHT_TOTLEN_AC1_S  0

/** JPEG_DHT_VAl_AC1_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_VAL_AC1_REG (DR_REG_JPEG_BASE + 0x74)
/** JPEG_DHT_VAL_AC1 : HRO; bitpos: [31:0]; default: 0;
 *  write codeword corresponding huffman values of ac1 table
 */
#define JPEG_DHT_VAL_AC1    0xFFFFFFFFU
#define JPEG_DHT_VAL_AC1_M  (JPEG_DHT_VAL_AC1_V << JPEG_DHT_VAL_AC1_S)
#define JPEG_DHT_VAL_AC1_V  0xFFFFFFFFU
#define JPEG_DHT_VAL_AC1_S  0

/** JPEG_DHT_CODEMIN_DC0_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_CODEMIN_DC0_REG (DR_REG_JPEG_BASE + 0x78)
/** JPEG_DHT_CODEMIN_DC0 : HRO; bitpos: [31:0]; default: 0;
 *  write the minimum codeword of  code length from 1~16 of dc0 table. The codeword is
 *  left shifted to the MSB position of a 16bit word
 */
#define JPEG_DHT_CODEMIN_DC0    0xFFFFFFFFU
#define JPEG_DHT_CODEMIN_DC0_M  (JPEG_DHT_CODEMIN_DC0_V << JPEG_DHT_CODEMIN_DC0_S)
#define JPEG_DHT_CODEMIN_DC0_V  0xFFFFFFFFU
#define JPEG_DHT_CODEMIN_DC0_S  0

/** JPEG_DHT_CODEMIN_AC0_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_CODEMIN_AC0_REG (DR_REG_JPEG_BASE + 0x7c)
/** JPEG_DHT_CODEMIN_AC0 : HRO; bitpos: [31:0]; default: 0;
 *  write the minimum codeword of  code length from 1~16 of ac0 table. The codeword is
 *  left shifted to the MSB position of a 16bit word
 */
#define JPEG_DHT_CODEMIN_AC0    0xFFFFFFFFU
#define JPEG_DHT_CODEMIN_AC0_M  (JPEG_DHT_CODEMIN_AC0_V << JPEG_DHT_CODEMIN_AC0_S)
#define JPEG_DHT_CODEMIN_AC0_V  0xFFFFFFFFU
#define JPEG_DHT_CODEMIN_AC0_S  0

/** JPEG_DHT_CODEMIN_DC1_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_CODEMIN_DC1_REG (DR_REG_JPEG_BASE + 0x80)
/** JPEG_DHT_CODEMIN_DC1 : HRO; bitpos: [31:0]; default: 0;
 *  write the minimum codeword of  code length from 1~16 of dc1 table. The codeword is
 *  left shifted to the MSB position of a 16bit word
 */
#define JPEG_DHT_CODEMIN_DC1    0xFFFFFFFFU
#define JPEG_DHT_CODEMIN_DC1_M  (JPEG_DHT_CODEMIN_DC1_V << JPEG_DHT_CODEMIN_DC1_S)
#define JPEG_DHT_CODEMIN_DC1_V  0xFFFFFFFFU
#define JPEG_DHT_CODEMIN_DC1_S  0

/** JPEG_DHT_CODEMIN_AC1_REG register
 *  Trace and Debug registers
 */
#define JPEG_DHT_CODEMIN_AC1_REG (DR_REG_JPEG_BASE + 0x84)
/** JPEG_DHT_CODEMIN_AC1 : HRO; bitpos: [31:0]; default: 0;
 *  write the minimum codeword of  code length from 1~16 of ac1 table. The codeword is
 *  left shifted to the MSB position of a 16bit word
 */
#define JPEG_DHT_CODEMIN_AC1    0xFFFFFFFFU
#define JPEG_DHT_CODEMIN_AC1_M  (JPEG_DHT_CODEMIN_AC1_V << JPEG_DHT_CODEMIN_AC1_S)
#define JPEG_DHT_CODEMIN_AC1_V  0xFFFFFFFFU
#define JPEG_DHT_CODEMIN_AC1_S  0

/** JPEG_DECODER_STATUS0_REG register
 *  Trace and Debug registers
 */
#define JPEG_DECODER_STATUS0_REG (DR_REG_JPEG_BASE + 0x88)
/** JPEG_DECODE_BYTE_CNT : RO; bitpos: [25:0]; default: 0;
 *  Reserved
 */
#define JPEG_DECODE_BYTE_CNT    0x03FFFFFFU
#define JPEG_DECODE_BYTE_CNT_M  (JPEG_DECODE_BYTE_CNT_V << JPEG_DECODE_BYTE_CNT_S)
#define JPEG_DECODE_BYTE_CNT_V  0x03FFFFFFU
#define JPEG_DECODE_BYTE_CNT_S  0
/** JPEG_HEADER_DEC_ST : RO; bitpos: [29:26]; default: 0;
 *  Reserved
 */
#define JPEG_HEADER_DEC_ST    0x0000000FU
#define JPEG_HEADER_DEC_ST_M  (JPEG_HEADER_DEC_ST_V << JPEG_HEADER_DEC_ST_S)
#define JPEG_HEADER_DEC_ST_V  0x0000000FU
#define JPEG_HEADER_DEC_ST_S  26
/** JPEG_DECODE_SAMPLE_SEL : RO; bitpos: [31:30]; default: 0;
 *  Reserved
 */
#define JPEG_DECODE_SAMPLE_SEL    0x00000003U
#define JPEG_DECODE_SAMPLE_SEL_M  (JPEG_DECODE_SAMPLE_SEL_V << JPEG_DECODE_SAMPLE_SEL_S)
#define JPEG_DECODE_SAMPLE_SEL_V  0x00000003U
#define JPEG_DECODE_SAMPLE_SEL_S  30

/** JPEG_DECODER_STATUS1_REG register
 *  Trace and Debug registers
 */
#define JPEG_DECODER_STATUS1_REG (DR_REG_JPEG_BASE + 0x8c)
/** JPEG_ENCODE_DATA : RO; bitpos: [15:0]; default: 0;
 *  Reserved
 */
#define JPEG_ENCODE_DATA    0x0000FFFFU
#define JPEG_ENCODE_DATA_M  (JPEG_ENCODE_DATA_V << JPEG_ENCODE_DATA_S)
#define JPEG_ENCODE_DATA_V  0x0000FFFFU
#define JPEG_ENCODE_DATA_S  0
/** JPEG_COUNT_Q : RO; bitpos: [22:16]; default: 0;
 *  Reserved
 */
#define JPEG_COUNT_Q    0x0000007FU
#define JPEG_COUNT_Q_M  (JPEG_COUNT_Q_V << JPEG_COUNT_Q_S)
#define JPEG_COUNT_Q_V  0x0000007FU
#define JPEG_COUNT_Q_S  16
/** JPEG_MCU_FSM_READY : RO; bitpos: [23]; default: 0;
 *  Reserved
 */
#define JPEG_MCU_FSM_READY    (BIT(23))
#define JPEG_MCU_FSM_READY_M  (JPEG_MCU_FSM_READY_V << JPEG_MCU_FSM_READY_S)
#define JPEG_MCU_FSM_READY_V  0x00000001U
#define JPEG_MCU_FSM_READY_S  23
/** JPEG_DECODE_DATA : RO; bitpos: [31:24]; default: 0;
 *  Reserved
 */
#define JPEG_DECODE_DATA    0x000000FFU
#define JPEG_DECODE_DATA_M  (JPEG_DECODE_DATA_V << JPEG_DECODE_DATA_S)
#define JPEG_DECODE_DATA_V  0x000000FFU
#define JPEG_DECODE_DATA_S  24

/** JPEG_DECODER_STATUS2_REG register
 *  Trace and Debug registers
 */
#define JPEG_DECODER_STATUS2_REG (DR_REG_JPEG_BASE + 0x90)
/** JPEG_COMP_BLOCK_NUM : RO; bitpos: [25:0]; default: 0;
 *  Reserved
 */
#define JPEG_COMP_BLOCK_NUM    0x03FFFFFFU
#define JPEG_COMP_BLOCK_NUM_M  (JPEG_COMP_BLOCK_NUM_V << JPEG_COMP_BLOCK_NUM_S)
#define JPEG_COMP_BLOCK_NUM_V  0x03FFFFFFU
#define JPEG_COMP_BLOCK_NUM_S  0
/** JPEG_SCAN_NUM : RO; bitpos: [28:26]; default: 0;
 *  Reserved
 */
#define JPEG_SCAN_NUM    0x00000007U
#define JPEG_SCAN_NUM_M  (JPEG_SCAN_NUM_V << JPEG_SCAN_NUM_S)
#define JPEG_SCAN_NUM_V  0x00000007U
#define JPEG_SCAN_NUM_S  26
/** JPEG_RST_CHECK_WAIT : RO; bitpos: [29]; default: 0;
 *  Reserved
 */
#define JPEG_RST_CHECK_WAIT    (BIT(29))
#define JPEG_RST_CHECK_WAIT_M  (JPEG_RST_CHECK_WAIT_V << JPEG_RST_CHECK_WAIT_S)
#define JPEG_RST_CHECK_WAIT_V  0x00000001U
#define JPEG_RST_CHECK_WAIT_S  29
/** JPEG_SCAN_CHECK_WAIT : RO; bitpos: [30]; default: 0;
 *  Reserved
 */
#define JPEG_SCAN_CHECK_WAIT    (BIT(30))
#define JPEG_SCAN_CHECK_WAIT_M  (JPEG_SCAN_CHECK_WAIT_V << JPEG_SCAN_CHECK_WAIT_S)
#define JPEG_SCAN_CHECK_WAIT_V  0x00000001U
#define JPEG_SCAN_CHECK_WAIT_S  30
/** JPEG_MCU_IN_PROC : RO; bitpos: [31]; default: 0;
 *  Reserved
 */
#define JPEG_MCU_IN_PROC    (BIT(31))
#define JPEG_MCU_IN_PROC_M  (JPEG_MCU_IN_PROC_V << JPEG_MCU_IN_PROC_S)
#define JPEG_MCU_IN_PROC_V  0x00000001U
#define JPEG_MCU_IN_PROC_S  31

/** JPEG_DECODER_STATUS3_REG register
 *  Trace and Debug registers
 */
#define JPEG_DECODER_STATUS3_REG (DR_REG_JPEG_BASE + 0x94)
/** JPEG_LOOKUP_DATA : RO; bitpos: [31:0]; default: 0;
 *  Reserved
 */
#define JPEG_LOOKUP_DATA    0xFFFFFFFFU
#define JPEG_LOOKUP_DATA_M  (JPEG_LOOKUP_DATA_V << JPEG_LOOKUP_DATA_S)
#define JPEG_LOOKUP_DATA_V  0xFFFFFFFFU
#define JPEG_LOOKUP_DATA_S  0

/** JPEG_DECODER_STATUS4_REG register
 *  Trace and Debug registers
 */
#define JPEG_DECODER_STATUS4_REG (DR_REG_JPEG_BASE + 0x98)
/** JPEG_BLOCK_EOF_CNT : RO; bitpos: [25:0]; default: 0;
 *  Reserved
 */
#define JPEG_BLOCK_EOF_CNT    0x03FFFFFFU
#define JPEG_BLOCK_EOF_CNT_M  (JPEG_BLOCK_EOF_CNT_V << JPEG_BLOCK_EOF_CNT_S)
#define JPEG_BLOCK_EOF_CNT_V  0x03FFFFFFU
#define JPEG_BLOCK_EOF_CNT_S  0
/** JPEG_DEZIGZAG_READY : RO; bitpos: [26]; default: 0;
 *  Reserved
 */
#define JPEG_DEZIGZAG_READY    (BIT(26))
#define JPEG_DEZIGZAG_READY_M  (JPEG_DEZIGZAG_READY_V << JPEG_DEZIGZAG_READY_S)
#define JPEG_DEZIGZAG_READY_V  0x00000001U
#define JPEG_DEZIGZAG_READY_S  26
/** JPEG_DE_FRAME_EOF_CHECK : RO; bitpos: [27]; default: 0;
 *  Reserved
 */
#define JPEG_DE_FRAME_EOF_CHECK    (BIT(27))
#define JPEG_DE_FRAME_EOF_CHECK_M  (JPEG_DE_FRAME_EOF_CHECK_V << JPEG_DE_FRAME_EOF_CHECK_S)
#define JPEG_DE_FRAME_EOF_CHECK_V  0x00000001U
#define JPEG_DE_FRAME_EOF_CHECK_S  27
/** JPEG_DE_DMA2D_IN_PUSH : RO; bitpos: [28]; default: 0;
 *  Reserved
 */
#define JPEG_DE_DMA2D_IN_PUSH    (BIT(28))
#define JPEG_DE_DMA2D_IN_PUSH_M  (JPEG_DE_DMA2D_IN_PUSH_V << JPEG_DE_DMA2D_IN_PUSH_S)
#define JPEG_DE_DMA2D_IN_PUSH_V  0x00000001U
#define JPEG_DE_DMA2D_IN_PUSH_S  28

/** JPEG_DECODER_STATUS5_REG register
 *  Trace and Debug registers
 */
#define JPEG_DECODER_STATUS5_REG (DR_REG_JPEG_BASE + 0x9c)
/** JPEG_IDCT_HFM_DATA : RO; bitpos: [15:0]; default: 0;
 *  Reserved
 */
#define JPEG_IDCT_HFM_DATA    0x0000FFFFU
#define JPEG_IDCT_HFM_DATA_M  (JPEG_IDCT_HFM_DATA_V << JPEG_IDCT_HFM_DATA_S)
#define JPEG_IDCT_HFM_DATA_V  0x0000FFFFU
#define JPEG_IDCT_HFM_DATA_S  0
/** JPEG_NS0 : RO; bitpos: [18:16]; default: 0;
 *  Reserved
 */
#define JPEG_NS0    0x00000007U
#define JPEG_NS0_M  (JPEG_NS0_V << JPEG_NS0_S)
#define JPEG_NS0_V  0x00000007U
#define JPEG_NS0_S  16
/** JPEG_NS1 : RO; bitpos: [21:19]; default: 0;
 *  Reserved
 */
#define JPEG_NS1    0x00000007U
#define JPEG_NS1_M  (JPEG_NS1_V << JPEG_NS1_S)
#define JPEG_NS1_V  0x00000007U
#define JPEG_NS1_S  19
/** JPEG_NS2 : RO; bitpos: [24:22]; default: 0;
 *  Reserved
 */
#define JPEG_NS2    0x00000007U
#define JPEG_NS2_M  (JPEG_NS2_V << JPEG_NS2_S)
#define JPEG_NS2_V  0x00000007U
#define JPEG_NS2_S  22
/** JPEG_NS3 : RO; bitpos: [27:25]; default: 0;
 *  Reserved
 */
#define JPEG_NS3    0x00000007U
#define JPEG_NS3_M  (JPEG_NS3_V << JPEG_NS3_S)
#define JPEG_NS3_V  0x00000007U
#define JPEG_NS3_S  25
/** JPEG_DATA_LAST_O : RO; bitpos: [28]; default: 0;
 *  Reserved
 */
#define JPEG_DATA_LAST_O    (BIT(28))
#define JPEG_DATA_LAST_O_M  (JPEG_DATA_LAST_O_V << JPEG_DATA_LAST_O_S)
#define JPEG_DATA_LAST_O_V  0x00000001U
#define JPEG_DATA_LAST_O_S  28
/** JPEG_RDN_RESULT : RO; bitpos: [29]; default: 0;
 *  redundant registers for jpeg
 */
#define JPEG_RDN_RESULT    (BIT(29))
#define JPEG_RDN_RESULT_M  (JPEG_RDN_RESULT_V << JPEG_RDN_RESULT_S)
#define JPEG_RDN_RESULT_V  0x00000001U
#define JPEG_RDN_RESULT_S  29
/** JPEG_RDN_ENA : R/W; bitpos: [30]; default: 0;
 *  redundant control registers for jpeg
 */
#define JPEG_RDN_ENA    (BIT(30))
#define JPEG_RDN_ENA_M  (JPEG_RDN_ENA_V << JPEG_RDN_ENA_S)
#define JPEG_RDN_ENA_V  0x00000001U
#define JPEG_RDN_ENA_S  30

/** JPEG_STATUS5_REG register
 *  Trace and Debug registers
 */
#define JPEG_STATUS5_REG (DR_REG_JPEG_BASE + 0xa0)
/** JPEG_PIC_BLOCK_NUM : RO; bitpos: [23:0]; default: 0;
 *  Reserved
 */
#define JPEG_PIC_BLOCK_NUM    0x00FFFFFFU
#define JPEG_PIC_BLOCK_NUM_M  (JPEG_PIC_BLOCK_NUM_V << JPEG_PIC_BLOCK_NUM_S)
#define JPEG_PIC_BLOCK_NUM_V  0x00FFFFFFU
#define JPEG_PIC_BLOCK_NUM_S  0

/** JPEG_ECO_LOW_REG register
 *  Trace and Debug registers
 */
#define JPEG_ECO_LOW_REG (DR_REG_JPEG_BASE + 0xa4)
/** JPEG_RDN_ECO_LOW : R/W; bitpos: [31:0]; default: 0;
 *  redundant registers for jpeg
 */
#define JPEG_RDN_ECO_LOW    0xFFFFFFFFU
#define JPEG_RDN_ECO_LOW_M  (JPEG_RDN_ECO_LOW_V << JPEG_RDN_ECO_LOW_S)
#define JPEG_RDN_ECO_LOW_V  0xFFFFFFFFU
#define JPEG_RDN_ECO_LOW_S  0

/** JPEG_ECO_HIGH_REG register
 *  Trace and Debug registers
 */
#define JPEG_ECO_HIGH_REG (DR_REG_JPEG_BASE + 0xa8)
/** JPEG_RDN_ECO_HIGH : R/W; bitpos: [31:0]; default: 4294967295;
 *  redundant registers for jpeg
 */
#define JPEG_RDN_ECO_HIGH    0xFFFFFFFFU
#define JPEG_RDN_ECO_HIGH_M  (JPEG_RDN_ECO_HIGH_V << JPEG_RDN_ECO_HIGH_S)
#define JPEG_RDN_ECO_HIGH_V  0xFFFFFFFFU
#define JPEG_RDN_ECO_HIGH_S  0

/** JPEG_SYS_REG register
 *  Trace and Debug registers
 */
#define JPEG_SYS_REG (DR_REG_JPEG_BASE + 0xf8)
/** JPEG_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Reserved
 */
#define JPEG_CLK_EN    (BIT(31))
#define JPEG_CLK_EN_M  (JPEG_CLK_EN_V << JPEG_CLK_EN_S)
#define JPEG_CLK_EN_V  0x00000001U
#define JPEG_CLK_EN_S  31

/** JPEG_VERSION_REG register
 *  Trace and Debug registers
 */
#define JPEG_VERSION_REG (DR_REG_JPEG_BASE + 0xfc)
/** JPEG_JPEG_VER : R/W; bitpos: [27:0]; default: 34673040;
 *  Reserved
 */
#define JPEG_JPEG_VER    0x0FFFFFFFU
#define JPEG_JPEG_VER_M  (JPEG_JPEG_VER_V << JPEG_JPEG_VER_S)
#define JPEG_JPEG_VER_V  0x0FFFFFFFU
#define JPEG_JPEG_VER_S  0

#ifdef __cplusplus
}
#endif
