/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_SYS_CLKRST_REG_H_
#define _SOC_SYS_CLKRST_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"

#define SYS_CLKRST_SYS_CLK_VER_DATE_REG          (DR_REG_SYS_CLKRST_BASE + 0x0)
/* SYS_CLKRST_VER_DATE : R/W ;bitpos:[31:0] ;default: 32'h20210610 ; */
/*description: .*/
#define SYS_CLKRST_VER_DATE    0xFFFFFFFF
#define SYS_CLKRST_VER_DATE_M  ((SYS_CLKRST_VER_DATE_V)<<(SYS_CLKRST_VER_DATE_S))
#define SYS_CLKRST_VER_DATE_V  0xFFFFFFFF
#define SYS_CLKRST_VER_DATE_S  0

#define SYS_CLKRST_SYS_ICM_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x4)
/* SYS_CLKRST_SYS_ICM_APB_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: system icm clock enable.*/
#define SYS_CLKRST_SYS_ICM_APB_CLK_EN    (BIT(0))
#define SYS_CLKRST_SYS_ICM_APB_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_SYS_ICM_APB_CLK_EN_V  0x1
#define SYS_CLKRST_SYS_ICM_APB_CLK_EN_S  0

#define SYS_CLKRST_JPEG_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x8)
/* SYS_CLKRST_JPEG_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define SYS_CLKRST_JPEG_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_JPEG_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_JPEG_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_JPEG_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_JPEG_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_JPEG_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_JPEG_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: clock phase offset compare to hp clock sync signal.*/
#define SYS_CLKRST_JPEG_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_JPEG_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_JPEG_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_JPEG_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_JPEG_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_JPEG_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_JPEG_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define SYS_CLKRST_JPEG_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_JPEG_CLK_DIV_NUM_M  ((SYS_CLKRST_JPEG_CLK_DIV_NUM_V)<<(SYS_CLKRST_JPEG_CLK_DIV_NUM_S))
#define SYS_CLKRST_JPEG_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_JPEG_CLK_DIV_NUM_S  8
/* SYS_CLKRST_JPEG_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define SYS_CLKRST_JPEG_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_JPEG_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_JPEG_FORCE_NORST_V  0x1
#define SYS_CLKRST_JPEG_FORCE_NORST_S  5
/* SYS_CLKRST_JPEG_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: software reset : low active.*/
#define SYS_CLKRST_JPEG_RSTN    (BIT(4))
#define SYS_CLKRST_JPEG_RSTN_M  (BIT(4))
#define SYS_CLKRST_JPEG_RSTN_V  0x1
#define SYS_CLKRST_JPEG_RSTN_S  4
/* SYS_CLKRST_JPEG_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: apb clock enable.*/
#define SYS_CLKRST_JPEG_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_JPEG_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_JPEG_APB_CLK_EN_V  0x1
#define SYS_CLKRST_JPEG_APB_CLK_EN_S  3
/* SYS_CLKRST_JPEG_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define SYS_CLKRST_JPEG_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_JPEG_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_JPEG_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_JPEG_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_JPEG_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define SYS_CLKRST_JPEG_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_JPEG_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_JPEG_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_JPEG_CLK_SYNC_EN_S  1
/* SYS_CLKRST_JPEG_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define SYS_CLKRST_JPEG_CLK_EN    (BIT(0))
#define SYS_CLKRST_JPEG_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_JPEG_CLK_EN_V  0x1
#define SYS_CLKRST_JPEG_CLK_EN_S  0

#define SYS_CLKRST_GFX_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0xC)
/* SYS_CLKRST_GFX_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define SYS_CLKRST_GFX_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_GFX_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_GFX_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_GFX_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_GFX_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_GFX_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_GFX_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: clock phase offset compare to hp clock sync signal.*/
#define SYS_CLKRST_GFX_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_GFX_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_GFX_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_GFX_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_GFX_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_GFX_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_GFX_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define SYS_CLKRST_GFX_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_GFX_CLK_DIV_NUM_M  ((SYS_CLKRST_GFX_CLK_DIV_NUM_V)<<(SYS_CLKRST_GFX_CLK_DIV_NUM_S))
#define SYS_CLKRST_GFX_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_GFX_CLK_DIV_NUM_S  8
/* SYS_CLKRST_GFX_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define SYS_CLKRST_GFX_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_GFX_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_GFX_FORCE_NORST_V  0x1
#define SYS_CLKRST_GFX_FORCE_NORST_S  5
/* SYS_CLKRST_GFX_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: software reset : low active.*/
#define SYS_CLKRST_GFX_RSTN    (BIT(4))
#define SYS_CLKRST_GFX_RSTN_M  (BIT(4))
#define SYS_CLKRST_GFX_RSTN_V  0x1
#define SYS_CLKRST_GFX_RSTN_S  4
/* SYS_CLKRST_GFX_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: apb clock enable.*/
#define SYS_CLKRST_GFX_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_GFX_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_GFX_APB_CLK_EN_V  0x1
#define SYS_CLKRST_GFX_APB_CLK_EN_S  3
/* SYS_CLKRST_GFX_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define SYS_CLKRST_GFX_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_GFX_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_GFX_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_GFX_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_GFX_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define SYS_CLKRST_GFX_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_GFX_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_GFX_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_GFX_CLK_SYNC_EN_S  1
/* SYS_CLKRST_GFX_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define SYS_CLKRST_GFX_CLK_EN    (BIT(0))
#define SYS_CLKRST_GFX_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_GFX_CLK_EN_V  0x1
#define SYS_CLKRST_GFX_CLK_EN_S  0

#define SYS_CLKRST_PSRAM_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x10)
/* SYS_CLKRST_PSRAM_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define SYS_CLKRST_PSRAM_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_PSRAM_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_PSRAM_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_PSRAM_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_PSRAM_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_PSRAM_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_PSRAM_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: clock phase offset compare to hp clock sync signal.*/
#define SYS_CLKRST_PSRAM_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_PSRAM_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_PSRAM_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_PSRAM_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_PSRAM_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_PSRAM_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_PSRAM_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define SYS_CLKRST_PSRAM_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_PSRAM_CLK_DIV_NUM_M  ((SYS_CLKRST_PSRAM_CLK_DIV_NUM_V)<<(SYS_CLKRST_PSRAM_CLK_DIV_NUM_S))
#define SYS_CLKRST_PSRAM_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_PSRAM_CLK_DIV_NUM_S  8
/* SYS_CLKRST_PSRAM_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define SYS_CLKRST_PSRAM_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_PSRAM_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_PSRAM_FORCE_NORST_V  0x1
#define SYS_CLKRST_PSRAM_FORCE_NORST_S  5
/* SYS_CLKRST_PSRAM_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: software reset : low active.*/
#define SYS_CLKRST_PSRAM_RSTN    (BIT(4))
#define SYS_CLKRST_PSRAM_RSTN_M  (BIT(4))
#define SYS_CLKRST_PSRAM_RSTN_V  0x1
#define SYS_CLKRST_PSRAM_RSTN_S  4
/* SYS_CLKRST_PSRAM_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: apb clock enable.*/
#define SYS_CLKRST_PSRAM_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_PSRAM_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_PSRAM_APB_CLK_EN_V  0x1
#define SYS_CLKRST_PSRAM_APB_CLK_EN_S  3
/* SYS_CLKRST_PSRAM_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define SYS_CLKRST_PSRAM_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_PSRAM_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_PSRAM_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_PSRAM_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_PSRAM_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define SYS_CLKRST_PSRAM_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_PSRAM_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_PSRAM_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_PSRAM_CLK_SYNC_EN_S  1
/* SYS_CLKRST_PSRAM_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define SYS_CLKRST_PSRAM_CLK_EN    (BIT(0))
#define SYS_CLKRST_PSRAM_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_PSRAM_CLK_EN_V  0x1
#define SYS_CLKRST_PSRAM_CLK_EN_S  0

#define SYS_CLKRST_MSPI_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x14)
/* SYS_CLKRST_MSPI_FORCE_NORST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_MSPI_FORCE_NORST    (BIT(2))
#define SYS_CLKRST_MSPI_FORCE_NORST_M  (BIT(2))
#define SYS_CLKRST_MSPI_FORCE_NORST_V  0x1
#define SYS_CLKRST_MSPI_FORCE_NORST_S  2
/* SYS_CLKRST_MSPI_RSTN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_MSPI_RSTN    (BIT(1))
#define SYS_CLKRST_MSPI_RSTN_M  (BIT(1))
#define SYS_CLKRST_MSPI_RSTN_V  0x1
#define SYS_CLKRST_MSPI_RSTN_S  1
/* SYS_CLKRST_MSPI_APB_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_MSPI_APB_CLK_EN    (BIT(0))
#define SYS_CLKRST_MSPI_APB_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_MSPI_APB_CLK_EN_V  0x1
#define SYS_CLKRST_MSPI_APB_CLK_EN_S  0

#define SYS_CLKRST_DSI_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x18)
/* SYS_CLKRST_DSI_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define SYS_CLKRST_DSI_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_DSI_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_DSI_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_DSI_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_DSI_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_DSI_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_DSI_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: clock phase offset compare to hp clock sync signal.*/
#define SYS_CLKRST_DSI_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_DSI_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_DSI_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_DSI_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_DSI_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_DSI_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_DSI_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define SYS_CLKRST_DSI_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_DSI_CLK_DIV_NUM_M  ((SYS_CLKRST_DSI_CLK_DIV_NUM_V)<<(SYS_CLKRST_DSI_CLK_DIV_NUM_S))
#define SYS_CLKRST_DSI_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_DSI_CLK_DIV_NUM_S  8
/* SYS_CLKRST_DSI_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define SYS_CLKRST_DSI_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_DSI_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_DSI_FORCE_NORST_V  0x1
#define SYS_CLKRST_DSI_FORCE_NORST_S  5
/* SYS_CLKRST_DSI_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: software reset : low active.*/
#define SYS_CLKRST_DSI_RSTN    (BIT(4))
#define SYS_CLKRST_DSI_RSTN_M  (BIT(4))
#define SYS_CLKRST_DSI_RSTN_V  0x1
#define SYS_CLKRST_DSI_RSTN_S  4
/* SYS_CLKRST_DSI_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: apb clock enable.*/
#define SYS_CLKRST_DSI_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_DSI_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_DSI_APB_CLK_EN_V  0x1
#define SYS_CLKRST_DSI_APB_CLK_EN_S  3
/* SYS_CLKRST_DSI_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define SYS_CLKRST_DSI_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_DSI_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_DSI_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_DSI_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_DSI_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define SYS_CLKRST_DSI_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_DSI_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_DSI_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_DSI_CLK_SYNC_EN_S  1
/* SYS_CLKRST_DSI_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define SYS_CLKRST_DSI_CLK_EN    (BIT(0))
#define SYS_CLKRST_DSI_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_DSI_CLK_EN_V  0x1
#define SYS_CLKRST_DSI_CLK_EN_S  0

#define SYS_CLKRST_CSI_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x1C)
/* SYS_CLKRST_CSI_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define SYS_CLKRST_CSI_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_CSI_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_CSI_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_CSI_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_CSI_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_CSI_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_CSI_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: clock phase offset compare to hp clock sync signal.*/
#define SYS_CLKRST_CSI_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_CSI_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_CSI_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_CSI_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_CSI_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_CSI_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_CSI_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define SYS_CLKRST_CSI_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_CSI_CLK_DIV_NUM_M  ((SYS_CLKRST_CSI_CLK_DIV_NUM_V)<<(SYS_CLKRST_CSI_CLK_DIV_NUM_S))
#define SYS_CLKRST_CSI_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_CSI_CLK_DIV_NUM_S  8
/* SYS_CLKRST_CSI_BRG_RSTN : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: software reset : low active.*/
#define SYS_CLKRST_CSI_BRG_RSTN    (BIT(6))
#define SYS_CLKRST_CSI_BRG_RSTN_M  (BIT(6))
#define SYS_CLKRST_CSI_BRG_RSTN_V  0x1
#define SYS_CLKRST_CSI_BRG_RSTN_S  6
/* SYS_CLKRST_CSI_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define SYS_CLKRST_CSI_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_CSI_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_CSI_FORCE_NORST_V  0x1
#define SYS_CLKRST_CSI_FORCE_NORST_S  5
/* SYS_CLKRST_CSI_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: software reset : low active.*/
#define SYS_CLKRST_CSI_RSTN    (BIT(4))
#define SYS_CLKRST_CSI_RSTN_M  (BIT(4))
#define SYS_CLKRST_CSI_RSTN_V  0x1
#define SYS_CLKRST_CSI_RSTN_S  4
/* SYS_CLKRST_CSI_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: apb clock enable.*/
#define SYS_CLKRST_CSI_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_CSI_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_CSI_APB_CLK_EN_V  0x1
#define SYS_CLKRST_CSI_APB_CLK_EN_S  3
/* SYS_CLKRST_CSI_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define SYS_CLKRST_CSI_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_CSI_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_CSI_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_CSI_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_CSI_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define SYS_CLKRST_CSI_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_CSI_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_CSI_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_CSI_CLK_SYNC_EN_S  1
/* SYS_CLKRST_CSI_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define SYS_CLKRST_CSI_CLK_EN    (BIT(0))
#define SYS_CLKRST_CSI_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_CSI_CLK_EN_V  0x1
#define SYS_CLKRST_CSI_CLK_EN_S  0

#define SYS_CLKRST_USB_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x20)
/* SYS_CLKRST_USB_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define SYS_CLKRST_USB_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_USB_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_USB_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_USB_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_USB_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_USB_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_USB_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: clock phase offset compare to hp clock sync signal.*/
#define SYS_CLKRST_USB_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_USB_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_USB_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_USB_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_USB_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_USB_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_USB_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define SYS_CLKRST_USB_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_USB_CLK_DIV_NUM_M  ((SYS_CLKRST_USB_CLK_DIV_NUM_V)<<(SYS_CLKRST_USB_CLK_DIV_NUM_S))
#define SYS_CLKRST_USB_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_USB_CLK_DIV_NUM_S  8
/* SYS_CLKRST_USB_PHY_FORCE_NORST : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Usb phy software force no reset.*/
#define SYS_CLKRST_USB_PHY_FORCE_NORST    (BIT(7))
#define SYS_CLKRST_USB_PHY_FORCE_NORST_M  (BIT(7))
#define SYS_CLKRST_USB_PHY_FORCE_NORST_V  0x1
#define SYS_CLKRST_USB_PHY_FORCE_NORST_S  7
/* SYS_CLKRST_USB_FORCE_NORST : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Usb software force no reset.*/
#define SYS_CLKRST_USB_FORCE_NORST    (BIT(6))
#define SYS_CLKRST_USB_FORCE_NORST_M  (BIT(6))
#define SYS_CLKRST_USB_FORCE_NORST_V  0x1
#define SYS_CLKRST_USB_FORCE_NORST_S  6
/* SYS_CLKRST_USB_PHY_RSTN : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: Usb phy software reset : low active.*/
#define SYS_CLKRST_USB_PHY_RSTN    (BIT(5))
#define SYS_CLKRST_USB_PHY_RSTN_M  (BIT(5))
#define SYS_CLKRST_USB_PHY_RSTN_V  0x1
#define SYS_CLKRST_USB_PHY_RSTN_S  5
/* SYS_CLKRST_USB_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: Usb software reset : low active.*/
#define SYS_CLKRST_USB_RSTN    (BIT(4))
#define SYS_CLKRST_USB_RSTN_M  (BIT(4))
#define SYS_CLKRST_USB_RSTN_V  0x1
#define SYS_CLKRST_USB_RSTN_S  4
/* SYS_CLKRST_USB_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: apb clock enable.*/
#define SYS_CLKRST_USB_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_USB_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_USB_APB_CLK_EN_V  0x1
#define SYS_CLKRST_USB_APB_CLK_EN_S  3
/* SYS_CLKRST_USB_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define SYS_CLKRST_USB_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_USB_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_USB_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_USB_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_USB_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define SYS_CLKRST_USB_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_USB_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_USB_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_USB_CLK_SYNC_EN_S  1
/* SYS_CLKRST_USB_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define SYS_CLKRST_USB_CLK_EN    (BIT(0))
#define SYS_CLKRST_USB_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_USB_CLK_EN_V  0x1
#define SYS_CLKRST_USB_CLK_EN_S  0

#define SYS_CLKRST_GMAC_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x24)
/* SYS_CLKRST_GMAC_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'b0 ; */
/*description: current clock divider number.*/
#define SYS_CLKRST_GMAC_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_GMAC_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_GMAC_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_GMAC_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_GMAC_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_GMAC_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_GMAC_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: clock phase offset compare to hp clock sync signal.*/
#define SYS_CLKRST_GMAC_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_GMAC_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_GMAC_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_GMAC_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_GMAC_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_GMAC_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_GMAC_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define SYS_CLKRST_GMAC_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_GMAC_CLK_DIV_NUM_M  ((SYS_CLKRST_GMAC_CLK_DIV_NUM_V)<<(SYS_CLKRST_GMAC_CLK_DIV_NUM_S))
#define SYS_CLKRST_GMAC_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_GMAC_CLK_DIV_NUM_S  8
/* SYS_CLKRST_GMAC_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define SYS_CLKRST_GMAC_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_GMAC_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_GMAC_FORCE_NORST_V  0x1
#define SYS_CLKRST_GMAC_FORCE_NORST_S  5
/* SYS_CLKRST_GMAC_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: software reset : low active.*/
#define SYS_CLKRST_GMAC_RSTN    (BIT(4))
#define SYS_CLKRST_GMAC_RSTN_M  (BIT(4))
#define SYS_CLKRST_GMAC_RSTN_V  0x1
#define SYS_CLKRST_GMAC_RSTN_S  4
/* SYS_CLKRST_GMAC_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: apb clock enable.*/
#define SYS_CLKRST_GMAC_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_GMAC_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_GMAC_APB_CLK_EN_V  0x1
#define SYS_CLKRST_GMAC_APB_CLK_EN_S  3
/* SYS_CLKRST_GMAC_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define SYS_CLKRST_GMAC_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_GMAC_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_GMAC_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_GMAC_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_GMAC_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define SYS_CLKRST_GMAC_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_GMAC_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_GMAC_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_GMAC_CLK_SYNC_EN_S  1
/* SYS_CLKRST_GMAC_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define SYS_CLKRST_GMAC_CLK_EN    (BIT(0))
#define SYS_CLKRST_GMAC_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_GMAC_CLK_EN_V  0x1
#define SYS_CLKRST_GMAC_CLK_EN_S  0

#define SYS_CLKRST_SDMMC_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x28)
/* SYS_CLKRST_SDMMC_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define SYS_CLKRST_SDMMC_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_SDMMC_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_SDMMC_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_SDMMC_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_SDMMC_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_SDMMC_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_SDMMC_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: clock phase offset compare to hp clock sync signal.*/
#define SYS_CLKRST_SDMMC_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_SDMMC_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_SDMMC_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_SDMMC_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_SDMMC_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_SDMMC_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_SDMMC_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define SYS_CLKRST_SDMMC_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_SDMMC_CLK_DIV_NUM_M  ((SYS_CLKRST_SDMMC_CLK_DIV_NUM_V)<<(SYS_CLKRST_SDMMC_CLK_DIV_NUM_S))
#define SYS_CLKRST_SDMMC_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_SDMMC_CLK_DIV_NUM_S  8
/* SYS_CLKRST_SDMMC_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define SYS_CLKRST_SDMMC_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_SDMMC_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_SDMMC_FORCE_NORST_V  0x1
#define SYS_CLKRST_SDMMC_FORCE_NORST_S  5
/* SYS_CLKRST_SDMMC_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: software reset : low active.*/
#define SYS_CLKRST_SDMMC_RSTN    (BIT(4))
#define SYS_CLKRST_SDMMC_RSTN_M  (BIT(4))
#define SYS_CLKRST_SDMMC_RSTN_V  0x1
#define SYS_CLKRST_SDMMC_RSTN_S  4
/* SYS_CLKRST_SDMMC_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: apb clock enable.*/
#define SYS_CLKRST_SDMMC_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_SDMMC_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_SDMMC_APB_CLK_EN_V  0x1
#define SYS_CLKRST_SDMMC_APB_CLK_EN_S  3
/* SYS_CLKRST_SDMMC_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define SYS_CLKRST_SDMMC_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_SDMMC_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_SDMMC_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_SDMMC_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_SDMMC_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define SYS_CLKRST_SDMMC_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_SDMMC_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_SDMMC_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_SDMMC_CLK_SYNC_EN_S  1
/* SYS_CLKRST_SDMMC_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define SYS_CLKRST_SDMMC_CLK_EN    (BIT(0))
#define SYS_CLKRST_SDMMC_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_SDMMC_CLK_EN_V  0x1
#define SYS_CLKRST_SDMMC_CLK_EN_S  0

#define SYS_CLKRST_DDRC_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x2C)
/* SYS_CLKRST_DDRC_FORCE_NORST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_DDRC_FORCE_NORST    (BIT(2))
#define SYS_CLKRST_DDRC_FORCE_NORST_M  (BIT(2))
#define SYS_CLKRST_DDRC_FORCE_NORST_V  0x1
#define SYS_CLKRST_DDRC_FORCE_NORST_S  2
/* SYS_CLKRST_DDRC_RSTN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_DDRC_RSTN    (BIT(1))
#define SYS_CLKRST_DDRC_RSTN_M  (BIT(1))
#define SYS_CLKRST_DDRC_RSTN_V  0x1
#define SYS_CLKRST_DDRC_RSTN_S  1
/* SYS_CLKRST_DDRC_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_DDRC_CLK_EN    (BIT(0))
#define SYS_CLKRST_DDRC_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_DDRC_CLK_EN_V  0x1
#define SYS_CLKRST_DDRC_CLK_EN_S  0

#define SYS_CLKRST_GDMA_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x30)
/* SYS_CLKRST_GDMA_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define SYS_CLKRST_GDMA_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_GDMA_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_GDMA_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_GDMA_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_GDMA_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_GDMA_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_GDMA_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: clock phase offset compare to hp clock sync signal.*/
#define SYS_CLKRST_GDMA_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_GDMA_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_GDMA_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_GDMA_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_GDMA_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_GDMA_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_GDMA_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define SYS_CLKRST_GDMA_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_GDMA_CLK_DIV_NUM_M  ((SYS_CLKRST_GDMA_CLK_DIV_NUM_V)<<(SYS_CLKRST_GDMA_CLK_DIV_NUM_S))
#define SYS_CLKRST_GDMA_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_GDMA_CLK_DIV_NUM_S  8
/* SYS_CLKRST_GDMA_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define SYS_CLKRST_GDMA_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_GDMA_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_GDMA_FORCE_NORST_V  0x1
#define SYS_CLKRST_GDMA_FORCE_NORST_S  5
/* SYS_CLKRST_GDMA_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: software reset : low active.*/
#define SYS_CLKRST_GDMA_RSTN    (BIT(4))
#define SYS_CLKRST_GDMA_RSTN_M  (BIT(4))
#define SYS_CLKRST_GDMA_RSTN_V  0x1
#define SYS_CLKRST_GDMA_RSTN_S  4
/* SYS_CLKRST_GDMA_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: apb clock enable.*/
#define SYS_CLKRST_GDMA_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_GDMA_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_GDMA_APB_CLK_EN_V  0x1
#define SYS_CLKRST_GDMA_APB_CLK_EN_S  3
/* SYS_CLKRST_GDMA_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define SYS_CLKRST_GDMA_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_GDMA_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_GDMA_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_GDMA_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_GDMA_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define SYS_CLKRST_GDMA_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_GDMA_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_GDMA_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_GDMA_CLK_SYNC_EN_S  1
/* SYS_CLKRST_GDMA_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define SYS_CLKRST_GDMA_CLK_EN    (BIT(0))
#define SYS_CLKRST_GDMA_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_GDMA_CLK_EN_V  0x1
#define SYS_CLKRST_GDMA_CLK_EN_S  0

#define SYS_CLKRST_USBOTG_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x34)
/* SYS_CLKRST_USBOTG_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: .*/
#define SYS_CLKRST_USBOTG_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_USBOTG_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_USBOTG_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_USBOTG_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_USBOTG_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_USBOTG_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_USBOTG_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: .*/
#define SYS_CLKRST_USBOTG_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_USBOTG_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_USBOTG_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_USBOTG_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_USBOTG_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_USBOTG_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_USBOTG_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: .*/
#define SYS_CLKRST_USBOTG_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_USBOTG_CLK_DIV_NUM_M  ((SYS_CLKRST_USBOTG_CLK_DIV_NUM_V)<<(SYS_CLKRST_USBOTG_CLK_DIV_NUM_S))
#define SYS_CLKRST_USBOTG_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_USBOTG_CLK_DIV_NUM_S  8
/* SYS_CLKRST_USBOTG_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_USBOTG_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_USBOTG_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_USBOTG_FORCE_NORST_V  0x1
#define SYS_CLKRST_USBOTG_FORCE_NORST_S  5
/* SYS_CLKRST_USBOTG_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_USBOTG_RSTN    (BIT(4))
#define SYS_CLKRST_USBOTG_RSTN_M  (BIT(4))
#define SYS_CLKRST_USBOTG_RSTN_V  0x1
#define SYS_CLKRST_USBOTG_RSTN_S  4
/* SYS_CLKRST_USBOTG_APB_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_USBOTG_APB_CLK_EN    (BIT(3))
#define SYS_CLKRST_USBOTG_APB_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_USBOTG_APB_CLK_EN_V  0x1
#define SYS_CLKRST_USBOTG_APB_CLK_EN_S  3
/* SYS_CLKRST_USBOTG_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_USBOTG_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_USBOTG_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_USBOTG_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_USBOTG_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_USBOTG_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_USBOTG_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_USBOTG_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_USBOTG_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_USBOTG_CLK_SYNC_EN_S  1
/* SYS_CLKRST_USBOTG_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_USBOTG_CLK_EN    (BIT(0))
#define SYS_CLKRST_USBOTG_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_USBOTG_CLK_EN_V  0x1
#define SYS_CLKRST_USBOTG_CLK_EN_S  0

#define SYS_CLKRST_SDIO_SLAVE_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x38)
/* SYS_CLKRST_SDIO_SLAVE_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: .*/
#define SYS_CLKRST_SDIO_SLAVE_CLK_CUR_DIV_NUM    0x000000FF
#define SYS_CLKRST_SDIO_SLAVE_CLK_CUR_DIV_NUM_M  ((SYS_CLKRST_SDIO_SLAVE_CLK_CUR_DIV_NUM_V)<<(SYS_CLKRST_SDIO_SLAVE_CLK_CUR_DIV_NUM_S))
#define SYS_CLKRST_SDIO_SLAVE_CLK_CUR_DIV_NUM_V  0xFF
#define SYS_CLKRST_SDIO_SLAVE_CLK_CUR_DIV_NUM_S  24
/* SYS_CLKRST_SDIO_SLAVE_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: .*/
#define SYS_CLKRST_SDIO_SLAVE_CLK_PHASE_OFFSET    0x000000FF
#define SYS_CLKRST_SDIO_SLAVE_CLK_PHASE_OFFSET_M  ((SYS_CLKRST_SDIO_SLAVE_CLK_PHASE_OFFSET_V)<<(SYS_CLKRST_SDIO_SLAVE_CLK_PHASE_OFFSET_S))
#define SYS_CLKRST_SDIO_SLAVE_CLK_PHASE_OFFSET_V  0xFF
#define SYS_CLKRST_SDIO_SLAVE_CLK_PHASE_OFFSET_S  16
/* SYS_CLKRST_SDIO_SLAVE_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: .*/
#define SYS_CLKRST_SDIO_SLAVE_CLK_DIV_NUM    0x000000FF
#define SYS_CLKRST_SDIO_SLAVE_CLK_DIV_NUM_M  ((SYS_CLKRST_SDIO_SLAVE_CLK_DIV_NUM_V)<<(SYS_CLKRST_SDIO_SLAVE_CLK_DIV_NUM_S))
#define SYS_CLKRST_SDIO_SLAVE_CLK_DIV_NUM_V  0xFF
#define SYS_CLKRST_SDIO_SLAVE_CLK_DIV_NUM_S  8
/* SYS_CLKRST_SDIO_SLAVE_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_SDIO_SLAVE_FORCE_NORST    (BIT(5))
#define SYS_CLKRST_SDIO_SLAVE_FORCE_NORST_M  (BIT(5))
#define SYS_CLKRST_SDIO_SLAVE_FORCE_NORST_V  0x1
#define SYS_CLKRST_SDIO_SLAVE_FORCE_NORST_S  5
/* SYS_CLKRST_SDIO_SLAVE_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_SDIO_SLAVE_RSTN    (BIT(4))
#define SYS_CLKRST_SDIO_SLAVE_RSTN_M  (BIT(4))
#define SYS_CLKRST_SDIO_SLAVE_RSTN_V  0x1
#define SYS_CLKRST_SDIO_SLAVE_RSTN_S  4
/* SYS_CLKRST_SDIO_SLAVE_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_SDIO_SLAVE_CLK_FORCE_SYNC_EN    (BIT(2))
#define SYS_CLKRST_SDIO_SLAVE_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define SYS_CLKRST_SDIO_SLAVE_CLK_FORCE_SYNC_EN_V  0x1
#define SYS_CLKRST_SDIO_SLAVE_CLK_FORCE_SYNC_EN_S  2
/* SYS_CLKRST_SDIO_SLAVE_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_SDIO_SLAVE_CLK_SYNC_EN    (BIT(1))
#define SYS_CLKRST_SDIO_SLAVE_CLK_SYNC_EN_M  (BIT(1))
#define SYS_CLKRST_SDIO_SLAVE_CLK_SYNC_EN_V  0x1
#define SYS_CLKRST_SDIO_SLAVE_CLK_SYNC_EN_S  1
/* SYS_CLKRST_SDIO_SLAVE_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_SDIO_SLAVE_CLK_EN    (BIT(0))
#define SYS_CLKRST_SDIO_SLAVE_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_SDIO_SLAVE_CLK_EN_V  0x1
#define SYS_CLKRST_SDIO_SLAVE_CLK_EN_S  0

#define SYS_CLKRST_ISP_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x40)
/* SYS_CLKRST_ISP_CLK_DIV_NUM : R/W ;bitpos:[6:3] ;default: 4'd1 ; */
/*description: .*/
#define SYS_CLKRST_ISP_CLK_DIV_NUM    0x0000000F
#define SYS_CLKRST_ISP_CLK_DIV_NUM_M  ((SYS_CLKRST_ISP_CLK_DIV_NUM_V)<<(SYS_CLKRST_ISP_CLK_DIV_NUM_S))
#define SYS_CLKRST_ISP_CLK_DIV_NUM_V  0xF
#define SYS_CLKRST_ISP_CLK_DIV_NUM_S  3
/* SYS_CLKRST_ISP_FORCE_NORST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_ISP_FORCE_NORST    (BIT(2))
#define SYS_CLKRST_ISP_FORCE_NORST_M  (BIT(2))
#define SYS_CLKRST_ISP_FORCE_NORST_V  0x1
#define SYS_CLKRST_ISP_FORCE_NORST_S  2
/* SYS_CLKRST_ISP_RSTN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_ISP_RSTN    (BIT(1))
#define SYS_CLKRST_ISP_RSTN_M  (BIT(1))
#define SYS_CLKRST_ISP_RSTN_V  0x1
#define SYS_CLKRST_ISP_RSTN_S  1
/* SYS_CLKRST_ISP_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_ISP_CLK_EN    (BIT(0))
#define SYS_CLKRST_ISP_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_ISP_CLK_EN_V  0x1
#define SYS_CLKRST_ISP_CLK_EN_S  0

#define SYS_CLKRST_DMA2D_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x44)
/* SYS_CLKRST_DMA2D_FORCE_NORST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_DMA2D_FORCE_NORST    (BIT(2))
#define SYS_CLKRST_DMA2D_FORCE_NORST_M  (BIT(2))
#define SYS_CLKRST_DMA2D_FORCE_NORST_V  0x1
#define SYS_CLKRST_DMA2D_FORCE_NORST_S  2
/* SYS_CLKRST_DMA2D_RSTN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_DMA2D_RSTN    (BIT(1))
#define SYS_CLKRST_DMA2D_RSTN_M  (BIT(1))
#define SYS_CLKRST_DMA2D_RSTN_V  0x1
#define SYS_CLKRST_DMA2D_RSTN_S  1
/* SYS_CLKRST_DMA2D_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_DMA2D_CLK_EN    (BIT(0))
#define SYS_CLKRST_DMA2D_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_DMA2D_CLK_EN_V  0x1
#define SYS_CLKRST_DMA2D_CLK_EN_S  0

#define SYS_CLKRST_PPA_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x48)
/* SYS_CLKRST_PPA_FORCE_NORST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_PPA_FORCE_NORST    (BIT(2))
#define SYS_CLKRST_PPA_FORCE_NORST_M  (BIT(2))
#define SYS_CLKRST_PPA_FORCE_NORST_V  0x1
#define SYS_CLKRST_PPA_FORCE_NORST_S  2
/* SYS_CLKRST_PPA_RSTN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_PPA_RSTN    (BIT(1))
#define SYS_CLKRST_PPA_RSTN_M  (BIT(1))
#define SYS_CLKRST_PPA_RSTN_V  0x1
#define SYS_CLKRST_PPA_RSTN_S  1
/* SYS_CLKRST_PPA_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_PPA_CLK_EN    (BIT(0))
#define SYS_CLKRST_PPA_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_PPA_CLK_EN_V  0x1
#define SYS_CLKRST_PPA_CLK_EN_S  0

#define SYS_CLKRST_GDMA_DBG_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x50)
/* SYS_CLKRST_DEBUG_CH_NUM : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: .*/
#define SYS_CLKRST_DEBUG_CH_NUM    0x00000003
#define SYS_CLKRST_DEBUG_CH_NUM_M  ((SYS_CLKRST_DEBUG_CH_NUM_V)<<(SYS_CLKRST_DEBUG_CH_NUM_S))
#define SYS_CLKRST_DEBUG_CH_NUM_V  0x3
#define SYS_CLKRST_DEBUG_CH_NUM_S  0

#define SYS_CLKRST_GMAC_PTP_RD0_REG          (DR_REG_SYS_CLKRST_BASE + 0x54)
/* SYS_CLKRST_PTP_TIMESTAMP_LO : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: .*/
#define SYS_CLKRST_PTP_TIMESTAMP_LO    0xFFFFFFFF
#define SYS_CLKRST_PTP_TIMESTAMP_LO_M  ((SYS_CLKRST_PTP_TIMESTAMP_LO_V)<<(SYS_CLKRST_PTP_TIMESTAMP_LO_S))
#define SYS_CLKRST_PTP_TIMESTAMP_LO_V  0xFFFFFFFF
#define SYS_CLKRST_PTP_TIMESTAMP_LO_S  0

#define SYS_CLKRST_GMAC_PTP_RD1_REG          (DR_REG_SYS_CLKRST_BASE + 0x58)
/* SYS_CLKRST_PTP_TIMESTAMP_HI : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: .*/
#define SYS_CLKRST_PTP_TIMESTAMP_HI    0xFFFFFFFF
#define SYS_CLKRST_PTP_TIMESTAMP_HI_M  ((SYS_CLKRST_PTP_TIMESTAMP_HI_V)<<(SYS_CLKRST_PTP_TIMESTAMP_HI_S))
#define SYS_CLKRST_PTP_TIMESTAMP_HI_V  0xFFFFFFFF
#define SYS_CLKRST_PTP_TIMESTAMP_HI_S  0

#define SYS_CLKRST_GMAC_PTP_PPS_REG          (DR_REG_SYS_CLKRST_BASE + 0x5C)
/* SYS_CLKRST_PTP_PPS : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_PTP_PPS    (BIT(0))
#define SYS_CLKRST_PTP_PPS_M  (BIT(0))
#define SYS_CLKRST_PTP_PPS_V  0x1
#define SYS_CLKRST_PTP_PPS_S  0

#define SYS_CLKRST_GMAC_CLK_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x60)
/* SYS_CLKRST_GMAC_FUNC_RX_CLK_FORCE_ON : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_GMAC_FUNC_RX_CLK_FORCE_ON    (BIT(13))
#define SYS_CLKRST_GMAC_FUNC_RX_CLK_FORCE_ON_M  (BIT(13))
#define SYS_CLKRST_GMAC_FUNC_RX_CLK_FORCE_ON_V  0x1
#define SYS_CLKRST_GMAC_FUNC_RX_CLK_FORCE_ON_S  13
/* SYS_CLKRST_GMAC_FUNC_TX_CLK_FORCE_ON : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_GMAC_FUNC_TX_CLK_FORCE_ON    (BIT(12))
#define SYS_CLKRST_GMAC_FUNC_TX_CLK_FORCE_ON_M  (BIT(12))
#define SYS_CLKRST_GMAC_FUNC_TX_CLK_FORCE_ON_V  0x1
#define SYS_CLKRST_GMAC_FUNC_TX_CLK_FORCE_ON_S  12
/* SYS_CLKRST_GMAC_FUNC_RX_CLK_EN : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_GMAC_FUNC_RX_CLK_EN    (BIT(11))
#define SYS_CLKRST_GMAC_FUNC_RX_CLK_EN_M  (BIT(11))
#define SYS_CLKRST_GMAC_FUNC_RX_CLK_EN_V  0x1
#define SYS_CLKRST_GMAC_FUNC_RX_CLK_EN_S  11
/* SYS_CLKRST_GMAC_FUNC_TX_CLK_EN : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_GMAC_FUNC_TX_CLK_EN    (BIT(10))
#define SYS_CLKRST_GMAC_FUNC_TX_CLK_EN_M  (BIT(10))
#define SYS_CLKRST_GMAC_FUNC_TX_CLK_EN_V  0x1
#define SYS_CLKRST_GMAC_FUNC_TX_CLK_EN_S  10
/* SYS_CLKRST_GMAC_MEM_CLK_FORCE_ON : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_GMAC_MEM_CLK_FORCE_ON    (BIT(9))
#define SYS_CLKRST_GMAC_MEM_CLK_FORCE_ON_M  (BIT(9))
#define SYS_CLKRST_GMAC_MEM_CLK_FORCE_ON_V  0x1
#define SYS_CLKRST_GMAC_MEM_CLK_FORCE_ON_S  9
/* SYS_CLKRST_REVMII_PMA_CLK_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_REVMII_PMA_CLK_EN    (BIT(8))
#define SYS_CLKRST_REVMII_PMA_CLK_EN_M  (BIT(8))
#define SYS_CLKRST_REVMII_PMA_CLK_EN_V  0x1
#define SYS_CLKRST_REVMII_PMA_CLK_EN_S  8
/* SYS_CLKRST_RMII_CLK_PORT_SEL : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_RMII_CLK_PORT_SEL    (BIT(7))
#define SYS_CLKRST_RMII_CLK_PORT_SEL_M  (BIT(7))
#define SYS_CLKRST_RMII_CLK_PORT_SEL_V  0x1
#define SYS_CLKRST_RMII_CLK_PORT_SEL_S  7
/* SYS_CLKRST_SBD_FLOWCTRL : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_SBD_FLOWCTRL    (BIT(6))
#define SYS_CLKRST_SBD_FLOWCTRL_M  (BIT(6))
#define SYS_CLKRST_SBD_FLOWCTRL_V  0x1
#define SYS_CLKRST_SBD_FLOWCTRL_S  6
/* SYS_CLKRST_PHY_INTF_SEL : R/W ;bitpos:[5:3] ;default: 3'b0 ; */
/*description: .*/
#define SYS_CLKRST_PHY_INTF_SEL    0x00000007
#define SYS_CLKRST_PHY_INTF_SEL_M  ((SYS_CLKRST_PHY_INTF_SEL_V)<<(SYS_CLKRST_PHY_INTF_SEL_S))
#define SYS_CLKRST_PHY_INTF_SEL_V  0x7
#define SYS_CLKRST_PHY_INTF_SEL_S  3
/* SYS_CLKRST_PTP_REF_CLK_SEL : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_PTP_REF_CLK_SEL    (BIT(2))
#define SYS_CLKRST_PTP_REF_CLK_SEL_M  (BIT(2))
#define SYS_CLKRST_PTP_REF_CLK_SEL_V  0x1
#define SYS_CLKRST_PTP_REF_CLK_SEL_S  2
/* SYS_CLKRST_REVERSE_GMAC_TX : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_REVERSE_GMAC_TX    (BIT(1))
#define SYS_CLKRST_REVERSE_GMAC_TX_M  (BIT(1))
#define SYS_CLKRST_REVERSE_GMAC_TX_V  0x1
#define SYS_CLKRST_REVERSE_GMAC_TX_S  1
/* SYS_CLKRST_REVERSE_GMAC_RX : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_REVERSE_GMAC_RX    (BIT(0))
#define SYS_CLKRST_REVERSE_GMAC_RX_M  (BIT(0))
#define SYS_CLKRST_REVERSE_GMAC_RX_V  0x1
#define SYS_CLKRST_REVERSE_GMAC_RX_S  0

#define SYS_CLKRST_OTG_PHY_CLK_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x64)
/* SYS_CLKRST_OTG_EXT_PHY_SEL : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_OTG_EXT_PHY_SEL    (BIT(10))
#define SYS_CLKRST_OTG_EXT_PHY_SEL_M  (BIT(10))
#define SYS_CLKRST_OTG_EXT_PHY_SEL_V  0x1
#define SYS_CLKRST_OTG_EXT_PHY_SEL_S  10
/* SYS_CLKRST_PHY_REF_CLK_SEL : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_PHY_REF_CLK_SEL    (BIT(9))
#define SYS_CLKRST_PHY_REF_CLK_SEL_M  (BIT(9))
#define SYS_CLKRST_PHY_REF_CLK_SEL_V  0x1
#define SYS_CLKRST_PHY_REF_CLK_SEL_S  9
/* SYS_CLKRST_PHY_RESET_FORCE_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_PHY_RESET_FORCE_EN    (BIT(8))
#define SYS_CLKRST_PHY_RESET_FORCE_EN_M  (BIT(8))
#define SYS_CLKRST_PHY_RESET_FORCE_EN_V  0x1
#define SYS_CLKRST_PHY_RESET_FORCE_EN_S  8
/* SYS_CLKRST_PHY_RSTN : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_PHY_RSTN    (BIT(7))
#define SYS_CLKRST_PHY_RSTN_M  (BIT(7))
#define SYS_CLKRST_PHY_RSTN_V  0x1
#define SYS_CLKRST_PHY_RSTN_S  7
/* SYS_CLKRST_PHY_PLL_FORCE_EN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_PHY_PLL_FORCE_EN    (BIT(6))
#define SYS_CLKRST_PHY_PLL_FORCE_EN_M  (BIT(6))
#define SYS_CLKRST_PHY_PLL_FORCE_EN_V  0x1
#define SYS_CLKRST_PHY_PLL_FORCE_EN_S  6
/* SYS_CLKRST_PHY_PLL_EN : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_PHY_PLL_EN    (BIT(5))
#define SYS_CLKRST_PHY_PLL_EN_M  (BIT(5))
#define SYS_CLKRST_PHY_PLL_EN_V  0x1
#define SYS_CLKRST_PHY_PLL_EN_S  5
/* SYS_CLKRST_PHY_SUSPEND_FORCE_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_PHY_SUSPEND_FORCE_EN    (BIT(4))
#define SYS_CLKRST_PHY_SUSPEND_FORCE_EN_M  (BIT(4))
#define SYS_CLKRST_PHY_SUSPEND_FORCE_EN_V  0x1
#define SYS_CLKRST_PHY_SUSPEND_FORCE_EN_S  4
/* SYS_CLKRST_PHY_SUSPENDM : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_PHY_SUSPENDM    (BIT(3))
#define SYS_CLKRST_PHY_SUSPENDM_M  (BIT(3))
#define SYS_CLKRST_PHY_SUSPENDM_V  0x1
#define SYS_CLKRST_PHY_SUSPENDM_S  3
/* SYS_CLKRST_OTG_PHY_TXBITSTUFF_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_OTG_PHY_TXBITSTUFF_EN    (BIT(2))
#define SYS_CLKRST_OTG_PHY_TXBITSTUFF_EN_M  (BIT(2))
#define SYS_CLKRST_OTG_PHY_TXBITSTUFF_EN_V  0x1
#define SYS_CLKRST_OTG_PHY_TXBITSTUFF_EN_S  2
/* SYS_CLKRST_OTG_SUSPENDM : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_OTG_SUSPENDM    (BIT(1))
#define SYS_CLKRST_OTG_SUSPENDM_M  (BIT(1))
#define SYS_CLKRST_OTG_SUSPENDM_V  0x1
#define SYS_CLKRST_OTG_SUSPENDM_S  1
/* SYS_CLKRST_OTG_PHY_REFCLK_MODE : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_OTG_PHY_REFCLK_MODE    (BIT(0))
#define SYS_CLKRST_OTG_PHY_REFCLK_MODE_M  (BIT(0))
#define SYS_CLKRST_OTG_PHY_REFCLK_MODE_V  0x1
#define SYS_CLKRST_OTG_PHY_REFCLK_MODE_S  0

#define SYS_CLKRST_SYS_PERI_APB_POSTW_CNTL_REG          (DR_REG_SYS_CLKRST_BASE + 0x68)
/* SYS_CLKRST_DSI_HOST_APB_POSTW_EN : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_DSI_HOST_APB_POSTW_EN    (BIT(6))
#define SYS_CLKRST_DSI_HOST_APB_POSTW_EN_M  (BIT(6))
#define SYS_CLKRST_DSI_HOST_APB_POSTW_EN_V  0x1
#define SYS_CLKRST_DSI_HOST_APB_POSTW_EN_S  6
/* SYS_CLKRST_CSI_HOST_APB_POSTW_EN : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_CSI_HOST_APB_POSTW_EN    (BIT(5))
#define SYS_CLKRST_CSI_HOST_APB_POSTW_EN_M  (BIT(5))
#define SYS_CLKRST_CSI_HOST_APB_POSTW_EN_V  0x1
#define SYS_CLKRST_CSI_HOST_APB_POSTW_EN_S  5
/* SYS_CLKRST_SDIO_SLAVE_APB_POSTW_EN : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_SDIO_SLAVE_APB_POSTW_EN    (BIT(4))
#define SYS_CLKRST_SDIO_SLAVE_APB_POSTW_EN_M  (BIT(4))
#define SYS_CLKRST_SDIO_SLAVE_APB_POSTW_EN_V  0x1
#define SYS_CLKRST_SDIO_SLAVE_APB_POSTW_EN_S  4
/* SYS_CLKRST_USB11_APB_POSTW_EN : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_USB11_APB_POSTW_EN    (BIT(3))
#define SYS_CLKRST_USB11_APB_POSTW_EN_M  (BIT(3))
#define SYS_CLKRST_USB11_APB_POSTW_EN_V  0x1
#define SYS_CLKRST_USB11_APB_POSTW_EN_S  3
/* SYS_CLKRST_DMA2D_APB_POSTW_EN : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_DMA2D_APB_POSTW_EN    (BIT(2))
#define SYS_CLKRST_DMA2D_APB_POSTW_EN_M  (BIT(2))
#define SYS_CLKRST_DMA2D_APB_POSTW_EN_V  0x1
#define SYS_CLKRST_DMA2D_APB_POSTW_EN_S  2
/* SYS_CLKRST_JPEG_APB_POSTW_EN : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_JPEG_APB_POSTW_EN    (BIT(1))
#define SYS_CLKRST_JPEG_APB_POSTW_EN_M  (BIT(1))
#define SYS_CLKRST_JPEG_APB_POSTW_EN_V  0x1
#define SYS_CLKRST_JPEG_APB_POSTW_EN_S  1
/* SYS_CLKRST_GMAC_APB_POSTW_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_GMAC_APB_POSTW_EN    (BIT(0))
#define SYS_CLKRST_GMAC_APB_POSTW_EN_M  (BIT(0))
#define SYS_CLKRST_GMAC_APB_POSTW_EN_V  0x1
#define SYS_CLKRST_GMAC_APB_POSTW_EN_S  0

#define SYS_CLKRST_SYS_LSLP_MEM_PD_REG          (DR_REG_SYS_CLKRST_BASE + 0x6C)
/* SYS_CLKRST_JPEG_SDSLP_MEM_PD : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_JPEG_SDSLP_MEM_PD    (BIT(4))
#define SYS_CLKRST_JPEG_SDSLP_MEM_PD_M  (BIT(4))
#define SYS_CLKRST_JPEG_SDSLP_MEM_PD_V  0x1
#define SYS_CLKRST_JPEG_SDSLP_MEM_PD_S  4
/* SYS_CLKRST_JPEG_DSLP_MEM_PD : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_JPEG_DSLP_MEM_PD    (BIT(3))
#define SYS_CLKRST_JPEG_DSLP_MEM_PD_M  (BIT(3))
#define SYS_CLKRST_JPEG_DSLP_MEM_PD_V  0x1
#define SYS_CLKRST_JPEG_DSLP_MEM_PD_S  3
/* SYS_CLKRST_JPEG_LSLP_MEM_PD : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_JPEG_LSLP_MEM_PD    (BIT(2))
#define SYS_CLKRST_JPEG_LSLP_MEM_PD_M  (BIT(2))
#define SYS_CLKRST_JPEG_LSLP_MEM_PD_V  0x1
#define SYS_CLKRST_JPEG_LSLP_MEM_PD_S  2
/* SYS_CLKRST_PPA_LSLP_MEM_PD : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_PPA_LSLP_MEM_PD    (BIT(1))
#define SYS_CLKRST_PPA_LSLP_MEM_PD_M  (BIT(1))
#define SYS_CLKRST_PPA_LSLP_MEM_PD_V  0x1
#define SYS_CLKRST_PPA_LSLP_MEM_PD_S  1
/* SYS_CLKRST_DMA2D_LSLP_MEM_PD : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_DMA2D_LSLP_MEM_PD    (BIT(0))
#define SYS_CLKRST_DMA2D_LSLP_MEM_PD_M  (BIT(0))
#define SYS_CLKRST_DMA2D_LSLP_MEM_PD_V  0x1
#define SYS_CLKRST_DMA2D_LSLP_MEM_PD_S  0

#define SYS_CLKRST_ECO_CELL_EN_AND_DATA_REG          (DR_REG_SYS_CLKRST_BASE + 0x70)
/* SYS_CLKRST_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_CLK_EN    (BIT(3))
#define SYS_CLKRST_CLK_EN_M  (BIT(3))
#define SYS_CLKRST_CLK_EN_V  0x1
#define SYS_CLKRST_CLK_EN_S  3
/* SYS_CLKRST_BIT_IN : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_BIT_IN    (BIT(2))
#define SYS_CLKRST_BIT_IN_M  (BIT(2))
#define SYS_CLKRST_BIT_IN_V  0x1
#define SYS_CLKRST_BIT_IN_S  2
/* SYS_CLKRST_BIT_OUT : RO ;bitpos:[1] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_BIT_OUT    (BIT(1))
#define SYS_CLKRST_BIT_OUT_M  (BIT(1))
#define SYS_CLKRST_BIT_OUT_V  0x1
#define SYS_CLKRST_BIT_OUT_S  1
/* SYS_CLKRST_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: .*/
#define SYS_CLKRST_EN    (BIT(0))
#define SYS_CLKRST_EN_M  (BIT(0))
#define SYS_CLKRST_EN_V  0x1
#define SYS_CLKRST_EN_S  0

#define SYS_CLKRST_USB_MEM_AUX_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x74)
/* SYS_CLKRST_OTG_PHY_BISTEN : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_OTG_PHY_BISTEN    (BIT(15))
#define SYS_CLKRST_OTG_PHY_BISTEN_M  (BIT(15))
#define SYS_CLKRST_OTG_PHY_BISTEN_V  0x1
#define SYS_CLKRST_OTG_PHY_BISTEN_S  15
/* SYS_CLKRST_OTG_PHY_TEST_DONE : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_OTG_PHY_TEST_DONE    (BIT(14))
#define SYS_CLKRST_OTG_PHY_TEST_DONE_M  (BIT(14))
#define SYS_CLKRST_OTG_PHY_TEST_DONE_V  0x1
#define SYS_CLKRST_OTG_PHY_TEST_DONE_S  14
/* SYS_CLKRST_USB_MEM_AUX_CTRL : R/W ;bitpos:[13:0] ;default: 14'h1320 ; */
/*description: .*/
#define SYS_CLKRST_USB_MEM_AUX_CTRL    0x00003FFF
#define SYS_CLKRST_USB_MEM_AUX_CTRL_M  ((SYS_CLKRST_USB_MEM_AUX_CTRL_V)<<(SYS_CLKRST_USB_MEM_AUX_CTRL_S))
#define SYS_CLKRST_USB_MEM_AUX_CTRL_V  0x3FFF
#define SYS_CLKRST_USB_MEM_AUX_CTRL_S  0

#define SYS_CLKRST_DUAL_MSPI_CTRL_REG          (DR_REG_SYS_CLKRST_BASE + 0x78)
/* SYS_CLKRST_DUAL_MSPI_FORCE_NORST : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_DUAL_MSPI_FORCE_NORST    (BIT(2))
#define SYS_CLKRST_DUAL_MSPI_FORCE_NORST_M  (BIT(2))
#define SYS_CLKRST_DUAL_MSPI_FORCE_NORST_V  0x1
#define SYS_CLKRST_DUAL_MSPI_FORCE_NORST_S  2
/* SYS_CLKRST_DUAL_MSPI_RSTN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_DUAL_MSPI_RSTN    (BIT(1))
#define SYS_CLKRST_DUAL_MSPI_RSTN_M  (BIT(1))
#define SYS_CLKRST_DUAL_MSPI_RSTN_V  0x1
#define SYS_CLKRST_DUAL_MSPI_RSTN_S  1
/* SYS_CLKRST_DUAL_MSPI_APB_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define SYS_CLKRST_DUAL_MSPI_APB_CLK_EN    (BIT(0))
#define SYS_CLKRST_DUAL_MSPI_APB_CLK_EN_M  (BIT(0))
#define SYS_CLKRST_DUAL_MSPI_APB_CLK_EN_V  0x1
#define SYS_CLKRST_DUAL_MSPI_APB_CLK_EN_S  0

#define SYS_CLKRST_HP_PERI_RDN_ECO_CS_REG          (DR_REG_SYS_CLKRST_BASE + 0x7C)
/* SYS_CLKRST_HP_PERI_RDN_ECO_RESULT : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_HP_PERI_RDN_ECO_RESULT    (BIT(1))
#define SYS_CLKRST_HP_PERI_RDN_ECO_RESULT_M  (BIT(1))
#define SYS_CLKRST_HP_PERI_RDN_ECO_RESULT_V  0x1
#define SYS_CLKRST_HP_PERI_RDN_ECO_RESULT_S  1
/* SYS_CLKRST_HP_PERI_RDN_ECO_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: .*/
#define SYS_CLKRST_HP_PERI_RDN_ECO_EN    (BIT(0))
#define SYS_CLKRST_HP_PERI_RDN_ECO_EN_M  (BIT(0))
#define SYS_CLKRST_HP_PERI_RDN_ECO_EN_V  0x1
#define SYS_CLKRST_HP_PERI_RDN_ECO_EN_S  0

#define SYS_CLKRST_HP_PERI_RDN_ECO_LOW_REG          (DR_REG_SYS_CLKRST_BASE + 0x80)
/* SYS_CLKRST_HP_PERI_RDN_ECO_LOW : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: .*/
#define SYS_CLKRST_HP_PERI_RDN_ECO_LOW    0xFFFFFFFF
#define SYS_CLKRST_HP_PERI_RDN_ECO_LOW_M  ((SYS_CLKRST_HP_PERI_RDN_ECO_LOW_V)<<(SYS_CLKRST_HP_PERI_RDN_ECO_LOW_S))
#define SYS_CLKRST_HP_PERI_RDN_ECO_LOW_V  0xFFFFFFFF
#define SYS_CLKRST_HP_PERI_RDN_ECO_LOW_S  0

#define SYS_CLKRST_HP_PERI_RDN_ECO_HIGH_REG          (DR_REG_SYS_CLKRST_BASE + 0x84)
/* SYS_CLKRST_HP_PERI_RDN_ECO_HIGH : R/W ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: .*/
#define SYS_CLKRST_HP_PERI_RDN_ECO_HIGH    0xFFFFFFFF
#define SYS_CLKRST_HP_PERI_RDN_ECO_HIGH_M  ((SYS_CLKRST_HP_PERI_RDN_ECO_HIGH_V)<<(SYS_CLKRST_HP_PERI_RDN_ECO_HIGH_S))
#define SYS_CLKRST_HP_PERI_RDN_ECO_HIGH_V  0xFFFFFFFF
#define SYS_CLKRST_HP_PERI_RDN_ECO_HIGH_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_SYS_CLKRST_REG_H_ */
