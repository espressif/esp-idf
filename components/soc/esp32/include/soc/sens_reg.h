// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_SENS_REG_H_
#define _SOC_SENS_REG_H_


#include "soc.h"
#define SENS_SAR_READ_CTRL_REG          (DR_REG_SENS_BASE + 0x0000)
/* SENS_SAR1_DATA_INV : R/W ;bitpos:[28] ;default: 1'd0 ; */
/*description: Invert SAR ADC1 data*/
#define SENS_SAR1_DATA_INV  (BIT(28))
#define SENS_SAR1_DATA_INV_M  (BIT(28))
#define SENS_SAR1_DATA_INV_V  0x1
#define SENS_SAR1_DATA_INV_S  28
/* SENS_SAR1_DIG_FORCE : R/W ;bitpos:[27] ;default: 1'd0 ; */
/*description: 1: SAR ADC1 controlled by DIG ADC1 CTRL  0: SAR ADC1 controlled by RTC ADC1 CTRL*/
#define SENS_SAR1_DIG_FORCE  (BIT(27))
#define SENS_SAR1_DIG_FORCE_M  (BIT(27))
#define SENS_SAR1_DIG_FORCE_V  0x1
#define SENS_SAR1_DIG_FORCE_S  27
/* SENS_SAR1_SAMPLE_NUM : R/W ;bitpos:[26:19] ;default: 8'd0 ; */
/*description: */
#define SENS_SAR1_SAMPLE_NUM  0x000000FF
#define SENS_SAR1_SAMPLE_NUM_M  ((SENS_SAR1_SAMPLE_NUM_V)<<(SENS_SAR1_SAMPLE_NUM_S))
#define SENS_SAR1_SAMPLE_NUM_V  0xFF
#define SENS_SAR1_SAMPLE_NUM_S  19
/* SENS_SAR1_CLK_GATED : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: */
#define SENS_SAR1_CLK_GATED  (BIT(18))
#define SENS_SAR1_CLK_GATED_M  (BIT(18))
#define SENS_SAR1_CLK_GATED_V  0x1
#define SENS_SAR1_CLK_GATED_S  18
/* SENS_SAR1_SAMPLE_BIT : R/W ;bitpos:[17:16] ;default: 2'd3 ; */
/*description: 00: for 9-bit width  01: for 10-bit width  10: for 11-bit width
  11: for 12-bit width*/
#define SENS_SAR1_SAMPLE_BIT  0x00000003
#define SENS_SAR1_SAMPLE_BIT_M  ((SENS_SAR1_SAMPLE_BIT_V)<<(SENS_SAR1_SAMPLE_BIT_S))
#define SENS_SAR1_SAMPLE_BIT_V  0x3
#define SENS_SAR1_SAMPLE_BIT_S  16
/* SENS_SAR1_SAMPLE_CYCLE : R/W ;bitpos:[15:8] ;default: 8'd9 ; */
/*description: sample cycles for SAR ADC1*/
#define SENS_SAR1_SAMPLE_CYCLE  0x000000FF
#define SENS_SAR1_SAMPLE_CYCLE_M  ((SENS_SAR1_SAMPLE_CYCLE_V)<<(SENS_SAR1_SAMPLE_CYCLE_S))
#define SENS_SAR1_SAMPLE_CYCLE_V  0xFF
#define SENS_SAR1_SAMPLE_CYCLE_S  8
/* SENS_SAR1_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd2 ; */
/*description: clock divider*/
#define SENS_SAR1_CLK_DIV  0x000000FF
#define SENS_SAR1_CLK_DIV_M  ((SENS_SAR1_CLK_DIV_V)<<(SENS_SAR1_CLK_DIV_S))
#define SENS_SAR1_CLK_DIV_V  0xFF
#define SENS_SAR1_CLK_DIV_S  0

#define SENS_SAR_READ_STATUS1_REG          (DR_REG_SENS_BASE + 0x0004)
/* SENS_SAR1_READER_STATUS : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SENS_SAR1_READER_STATUS  0xFFFFFFFF
#define SENS_SAR1_READER_STATUS_M  ((SENS_SAR1_READER_STATUS_V)<<(SENS_SAR1_READER_STATUS_S))
#define SENS_SAR1_READER_STATUS_V  0xFFFFFFFF
#define SENS_SAR1_READER_STATUS_S  0

#define SENS_SAR_MEAS_WAIT1_REG          (DR_REG_SENS_BASE + 0x0008)
/* SENS_SAR_AMP_WAIT2 : R/W ;bitpos:[31:16] ;default: 16'd10 ; */
/*description: */
#define SENS_SAR_AMP_WAIT2  0x0000FFFF
#define SENS_SAR_AMP_WAIT2_M  ((SENS_SAR_AMP_WAIT2_V)<<(SENS_SAR_AMP_WAIT2_S))
#define SENS_SAR_AMP_WAIT2_V  0xFFFF
#define SENS_SAR_AMP_WAIT2_S  16
/* SENS_SAR_AMP_WAIT1 : R/W ;bitpos:[15:0] ;default: 16'd10 ; */
/*description: */
#define SENS_SAR_AMP_WAIT1  0x0000FFFF
#define SENS_SAR_AMP_WAIT1_M  ((SENS_SAR_AMP_WAIT1_V)<<(SENS_SAR_AMP_WAIT1_S))
#define SENS_SAR_AMP_WAIT1_V  0xFFFF
#define SENS_SAR_AMP_WAIT1_S  0

#define SENS_SAR_MEAS_WAIT2_REG          (DR_REG_SENS_BASE + 0x000c)
/* SENS_SAR2_RSTB_WAIT : R/W ;bitpos:[27:20] ;default: 8'd2 ; */
/*description: */
#define SENS_SAR2_RSTB_WAIT  0x000000FF
#define SENS_SAR2_RSTB_WAIT_M  ((SENS_SAR2_RSTB_WAIT_V)<<(SENS_SAR2_RSTB_WAIT_S))
#define SENS_SAR2_RSTB_WAIT_V  0xFF
#define SENS_SAR2_RSTB_WAIT_S  20
/* SENS_FORCE_XPD_SAR : R/W ;bitpos:[19:18] ;default: 2'd0 ; */
/*description: */
#define SENS_FORCE_XPD_SAR  0x00000003
#define SENS_FORCE_XPD_SAR_M  ((SENS_FORCE_XPD_SAR_V)<<(SENS_FORCE_XPD_SAR_S))
#define SENS_FORCE_XPD_SAR_V  0x3
#define SENS_FORCE_XPD_SAR_S  18
#define SENS_FORCE_XPD_SAR_SW_M (BIT1)
#define SENS_FORCE_XPD_SAR_FSM 0 // Use FSM to control power down
#define SENS_FORCE_XPD_SAR_PD  2 // Force power down
#define SENS_FORCE_XPD_SAR_PU  3 // Force power up
/* SENS_FORCE_XPD_AMP : R/W ;bitpos:[17:16] ;default: 2'd0 ; */
/*description: */
#define SENS_FORCE_XPD_AMP  0x00000003
#define SENS_FORCE_XPD_AMP_M  ((SENS_FORCE_XPD_AMP_V)<<(SENS_FORCE_XPD_AMP_S))
#define SENS_FORCE_XPD_AMP_V  0x3
#define SENS_FORCE_XPD_AMP_S  16
#define SENS_FORCE_XPD_AMP_FSM 0 // Use FSM to control power down
#define SENS_FORCE_XPD_AMP_PD  2 // Force power down
#define SENS_FORCE_XPD_AMP_PU  3 // Force power up
/* SENS_SAR_AMP_WAIT3 : R/W ;bitpos:[15:0] ;default: 16'd10 ; */
/*description: */
#define SENS_SAR_AMP_WAIT3  0x0000FFFF
#define SENS_SAR_AMP_WAIT3_M  ((SENS_SAR_AMP_WAIT3_V)<<(SENS_SAR_AMP_WAIT3_S))
#define SENS_SAR_AMP_WAIT3_V  0xFFFF
#define SENS_SAR_AMP_WAIT3_S  0

#define SENS_SAR_MEAS_CTRL_REG          (DR_REG_SENS_BASE + 0x0010)
/* SENS_SAR2_XPD_WAIT : R/W ;bitpos:[31:24] ;default: 8'h7 ; */
/*description: */
#define SENS_SAR2_XPD_WAIT  0x000000FF
#define SENS_SAR2_XPD_WAIT_M  ((SENS_SAR2_XPD_WAIT_V)<<(SENS_SAR2_XPD_WAIT_S))
#define SENS_SAR2_XPD_WAIT_V  0xFF
#define SENS_SAR2_XPD_WAIT_S  24
/* SENS_SAR_RSTB_FSM : R/W ;bitpos:[23:20] ;default: 4'b0000 ; */
/*description: */
#define SENS_SAR_RSTB_FSM  0x0000000F
#define SENS_SAR_RSTB_FSM_M  ((SENS_SAR_RSTB_FSM_V)<<(SENS_SAR_RSTB_FSM_S))
#define SENS_SAR_RSTB_FSM_V  0xF
#define SENS_SAR_RSTB_FSM_S  20
/* SENS_XPD_SAR_FSM : R/W ;bitpos:[19:16] ;default: 4'b0111 ; */
/*description: */
#define SENS_XPD_SAR_FSM  0x0000000F
#define SENS_XPD_SAR_FSM_M  ((SENS_XPD_SAR_FSM_V)<<(SENS_XPD_SAR_FSM_S))
#define SENS_XPD_SAR_FSM_V  0xF
#define SENS_XPD_SAR_FSM_S  16
/* SENS_AMP_SHORT_REF_GND_FSM : R/W ;bitpos:[15:12] ;default: 4'b0011 ; */
/*description: */
#define SENS_AMP_SHORT_REF_GND_FSM  0x0000000F
#define SENS_AMP_SHORT_REF_GND_FSM_M  ((SENS_AMP_SHORT_REF_GND_FSM_V)<<(SENS_AMP_SHORT_REF_GND_FSM_S))
#define SENS_AMP_SHORT_REF_GND_FSM_V  0xF
#define SENS_AMP_SHORT_REF_GND_FSM_S  12
/* SENS_AMP_SHORT_REF_FSM : R/W ;bitpos:[11:8] ;default: 4'b0011 ; */
/*description: */
#define SENS_AMP_SHORT_REF_FSM  0x0000000F
#define SENS_AMP_SHORT_REF_FSM_M  ((SENS_AMP_SHORT_REF_FSM_V)<<(SENS_AMP_SHORT_REF_FSM_S))
#define SENS_AMP_SHORT_REF_FSM_V  0xF
#define SENS_AMP_SHORT_REF_FSM_S  8
/* SENS_AMP_RST_FB_FSM : R/W ;bitpos:[7:4] ;default: 4'b1000 ; */
/*description: */
#define SENS_AMP_RST_FB_FSM  0x0000000F
#define SENS_AMP_RST_FB_FSM_M  ((SENS_AMP_RST_FB_FSM_V)<<(SENS_AMP_RST_FB_FSM_S))
#define SENS_AMP_RST_FB_FSM_V  0xF
#define SENS_AMP_RST_FB_FSM_S  4
/* SENS_XPD_SAR_AMP_FSM : R/W ;bitpos:[3:0] ;default: 4'b1111 ; */
/*description: */
#define SENS_XPD_SAR_AMP_FSM  0x0000000F
#define SENS_XPD_SAR_AMP_FSM_M  ((SENS_XPD_SAR_AMP_FSM_V)<<(SENS_XPD_SAR_AMP_FSM_S))
#define SENS_XPD_SAR_AMP_FSM_V  0xF
#define SENS_XPD_SAR_AMP_FSM_S  0

#define SENS_SAR_READ_STATUS2_REG          (DR_REG_SENS_BASE + 0x0014)
/* SENS_SAR2_READER_STATUS : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SENS_SAR2_READER_STATUS  0xFFFFFFFF
#define SENS_SAR2_READER_STATUS_M  ((SENS_SAR2_READER_STATUS_V)<<(SENS_SAR2_READER_STATUS_S))
#define SENS_SAR2_READER_STATUS_V  0xFFFFFFFF
#define SENS_SAR2_READER_STATUS_S  0

#define SENS_ULP_CP_SLEEP_CYC0_REG          (DR_REG_SENS_BASE + 0x0018)
/* SENS_SLEEP_CYCLES_S0 : R/W ;bitpos:[31:0] ;default: 32'd200 ; */
/*description: sleep cycles for ULP-coprocessor timer*/
#define SENS_SLEEP_CYCLES_S0  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S0_M  ((SENS_SLEEP_CYCLES_S0_V)<<(SENS_SLEEP_CYCLES_S0_S))
#define SENS_SLEEP_CYCLES_S0_V  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S0_S  0

#define SENS_ULP_CP_SLEEP_CYC1_REG          (DR_REG_SENS_BASE + 0x001c)
/* SENS_SLEEP_CYCLES_S1 : R/W ;bitpos:[31:0] ;default: 32'd100 ; */
/*description: */
#define SENS_SLEEP_CYCLES_S1  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S1_M  ((SENS_SLEEP_CYCLES_S1_V)<<(SENS_SLEEP_CYCLES_S1_S))
#define SENS_SLEEP_CYCLES_S1_V  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S1_S  0

#define SENS_ULP_CP_SLEEP_CYC2_REG          (DR_REG_SENS_BASE + 0x0020)
/* SENS_SLEEP_CYCLES_S2 : R/W ;bitpos:[31:0] ;default: 32'd50 ; */
/*description: */
#define SENS_SLEEP_CYCLES_S2  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S2_M  ((SENS_SLEEP_CYCLES_S2_V)<<(SENS_SLEEP_CYCLES_S2_S))
#define SENS_SLEEP_CYCLES_S2_V  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S2_S  0

#define SENS_ULP_CP_SLEEP_CYC3_REG          (DR_REG_SENS_BASE + 0x0024)
/* SENS_SLEEP_CYCLES_S3 : R/W ;bitpos:[31:0] ;default: 32'd40 ; */
/*description: */
#define SENS_SLEEP_CYCLES_S3  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S3_M  ((SENS_SLEEP_CYCLES_S3_V)<<(SENS_SLEEP_CYCLES_S3_S))
#define SENS_SLEEP_CYCLES_S3_V  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S3_S  0

#define SENS_ULP_CP_SLEEP_CYC4_REG          (DR_REG_SENS_BASE + 0x0028)
/* SENS_SLEEP_CYCLES_S4 : R/W ;bitpos:[31:0] ;default: 32'd20 ; */
/*description: */
#define SENS_SLEEP_CYCLES_S4  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S4_M  ((SENS_SLEEP_CYCLES_S4_V)<<(SENS_SLEEP_CYCLES_S4_S))
#define SENS_SLEEP_CYCLES_S4_V  0xFFFFFFFF
#define SENS_SLEEP_CYCLES_S4_S  0

#define SENS_SAR_START_FORCE_REG          (DR_REG_SENS_BASE + 0x002c)
/* SENS_SAR2_PWDET_EN : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: N/A*/
#define SENS_SAR2_PWDET_EN  (BIT(24))
#define SENS_SAR2_PWDET_EN_M  (BIT(24))
#define SENS_SAR2_PWDET_EN_V  0x1
#define SENS_SAR2_PWDET_EN_S  24
/* SENS_SAR1_STOP : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: stop SAR ADC1 conversion*/
#define SENS_SAR1_STOP  (BIT(23))
#define SENS_SAR1_STOP_M  (BIT(23))
#define SENS_SAR1_STOP_V  0x1
#define SENS_SAR1_STOP_S  23
/* SENS_SAR2_STOP : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: stop SAR ADC2 conversion*/
#define SENS_SAR2_STOP  (BIT(22))
#define SENS_SAR2_STOP_M  (BIT(22))
#define SENS_SAR2_STOP_V  0x1
#define SENS_SAR2_STOP_S  22
/* SENS_PC_INIT : R/W ;bitpos:[21:11] ;default: 11'b0 ; */
/*description: initialized PC for ULP-coprocessor*/
#define SENS_PC_INIT  0x000007FF
#define SENS_PC_INIT_M  ((SENS_PC_INIT_V)<<(SENS_PC_INIT_S))
#define SENS_PC_INIT_V  0x7FF
#define SENS_PC_INIT_S  11
/* SENS_SARCLK_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SENS_SARCLK_EN  (BIT(10))
#define SENS_SARCLK_EN_M  (BIT(10))
#define SENS_SARCLK_EN_V  0x1
#define SENS_SARCLK_EN_S  10
/* SENS_ULP_CP_START_TOP : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Write 1 to start ULP-coprocessor  only active when reg_ulp_cp_force_start_top
 = 1*/
#define SENS_ULP_CP_START_TOP  (BIT(9))
#define SENS_ULP_CP_START_TOP_M  (BIT(9))
#define SENS_ULP_CP_START_TOP_V  0x1
#define SENS_ULP_CP_START_TOP_S  9
/* SENS_ULP_CP_FORCE_START_TOP : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: 1: ULP-coprocessor is started by SW  0: ULP-coprocessor is started by timer*/
#define SENS_ULP_CP_FORCE_START_TOP  (BIT(8))
#define SENS_ULP_CP_FORCE_START_TOP_M  (BIT(8))
#define SENS_ULP_CP_FORCE_START_TOP_V  0x1
#define SENS_ULP_CP_FORCE_START_TOP_S  8
/* SENS_SAR2_PWDET_CCT : R/W ;bitpos:[7:5] ;default: 3'b0 ; */
/*description: SAR2_PWDET_CCT  PA power detector capacitance tuning.*/
#define SENS_SAR2_PWDET_CCT  0x00000007
#define SENS_SAR2_PWDET_CCT_M  ((SENS_SAR2_PWDET_CCT_V)<<(SENS_SAR2_PWDET_CCT_S))
#define SENS_SAR2_PWDET_CCT_V  0x7
#define SENS_SAR2_PWDET_CCT_S  5
/* SENS_SAR2_EN_TEST : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: SAR2_EN_TEST  only active when reg_sar2_dig_force = 0*/
#define SENS_SAR2_EN_TEST  (BIT(4))
#define SENS_SAR2_EN_TEST_M  (BIT(4))
#define SENS_SAR2_EN_TEST_V  0x1
#define SENS_SAR2_EN_TEST_S  4
/* SENS_SAR2_BIT_WIDTH : R/W ;bitpos:[3:2] ;default: 2'b11 ; */
/*description: 00: 9 bit  01: 10 bits  10: 11bits  11: 12bits*/
#define SENS_SAR2_BIT_WIDTH  0x00000003
#define SENS_SAR2_BIT_WIDTH_M  ((SENS_SAR2_BIT_WIDTH_V)<<(SENS_SAR2_BIT_WIDTH_S))
#define SENS_SAR2_BIT_WIDTH_V  0x3
#define SENS_SAR2_BIT_WIDTH_S  2
/* SENS_SAR1_BIT_WIDTH : R/W ;bitpos:[1:0] ;default: 2'b11 ; */
/*description: 00: 9 bit  01: 10 bits  10: 11bits  11: 12bits*/
#define SENS_SAR1_BIT_WIDTH  0x00000003
#define SENS_SAR1_BIT_WIDTH_M  ((SENS_SAR1_BIT_WIDTH_V)<<(SENS_SAR1_BIT_WIDTH_S))
#define SENS_SAR1_BIT_WIDTH_V  0x3
#define SENS_SAR1_BIT_WIDTH_S  0

#define SENS_SAR_MEM_WR_CTRL_REG          (DR_REG_SENS_BASE + 0x0030)
/* SENS_RTC_MEM_WR_OFFST_CLR : WO ;bitpos:[22] ;default: 1'd0 ; */
/*description: */
#define SENS_RTC_MEM_WR_OFFST_CLR  (BIT(22))
#define SENS_RTC_MEM_WR_OFFST_CLR_M  (BIT(22))
#define SENS_RTC_MEM_WR_OFFST_CLR_V  0x1
#define SENS_RTC_MEM_WR_OFFST_CLR_S  22
/* SENS_MEM_WR_ADDR_SIZE : R/W ;bitpos:[21:11] ;default: 11'd512 ; */
/*description: */
#define SENS_MEM_WR_ADDR_SIZE  0x000007FF
#define SENS_MEM_WR_ADDR_SIZE_M  ((SENS_MEM_WR_ADDR_SIZE_V)<<(SENS_MEM_WR_ADDR_SIZE_S))
#define SENS_MEM_WR_ADDR_SIZE_V  0x7FF
#define SENS_MEM_WR_ADDR_SIZE_S  11
/* SENS_MEM_WR_ADDR_INIT : R/W ;bitpos:[10:0] ;default: 11'd512 ; */
/*description: */
#define SENS_MEM_WR_ADDR_INIT  0x000007FF
#define SENS_MEM_WR_ADDR_INIT_M  ((SENS_MEM_WR_ADDR_INIT_V)<<(SENS_MEM_WR_ADDR_INIT_S))
#define SENS_MEM_WR_ADDR_INIT_V  0x7FF
#define SENS_MEM_WR_ADDR_INIT_S  0

#define SENS_SAR_ATTEN1_REG          (DR_REG_SENS_BASE + 0x0034)
/* SENS_SAR1_ATTEN : R/W ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: 2-bit attenuation for each pad  11:1dB  10:6dB  01:3dB  00:0dB*/
#define SENS_SAR1_ATTEN  0xFFFFFFFF
#define SENS_SAR1_ATTEN_M  ((SENS_SAR1_ATTEN_V)<<(SENS_SAR1_ATTEN_S))
#define SENS_SAR1_ATTEN_V  0xFFFFFFFF
#define SENS_SAR1_ATTEN_S  0
#define SENS_SAR1_ATTEN_VAL_MASK   0x3
#define SENS_SAR2_ATTEN_VAL_MASK   0x3

#define SENS_SAR_ATTEN2_REG          (DR_REG_SENS_BASE + 0x0038)
/* SENS_SAR2_ATTEN : R/W ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: 2-bit attenuation for each pad  11:1dB  10:6dB  01:3dB  00:0dB*/
#define SENS_SAR2_ATTEN  0xFFFFFFFF
#define SENS_SAR2_ATTEN_M  ((SENS_SAR2_ATTEN_V)<<(SENS_SAR2_ATTEN_S))
#define SENS_SAR2_ATTEN_V  0xFFFFFFFF
#define SENS_SAR2_ATTEN_S  0

#define SENS_SAR_SLAVE_ADDR1_REG          (DR_REG_SENS_BASE + 0x003c)
/* SENS_MEAS_STATUS : RO ;bitpos:[29:22] ;default: 8'h0 ; */
/*description: */
#define SENS_MEAS_STATUS  0x000000FF
#define SENS_MEAS_STATUS_M  ((SENS_MEAS_STATUS_V)<<(SENS_MEAS_STATUS_S))
#define SENS_MEAS_STATUS_V  0xFF
#define SENS_MEAS_STATUS_S  22
/* SENS_I2C_SLAVE_ADDR0 : R/W ;bitpos:[21:11] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR0  0x000007FF
#define SENS_I2C_SLAVE_ADDR0_M  ((SENS_I2C_SLAVE_ADDR0_V)<<(SENS_I2C_SLAVE_ADDR0_S))
#define SENS_I2C_SLAVE_ADDR0_V  0x7FF
#define SENS_I2C_SLAVE_ADDR0_S  11
/* SENS_I2C_SLAVE_ADDR1 : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR1  0x000007FF
#define SENS_I2C_SLAVE_ADDR1_M  ((SENS_I2C_SLAVE_ADDR1_V)<<(SENS_I2C_SLAVE_ADDR1_S))
#define SENS_I2C_SLAVE_ADDR1_V  0x7FF
#define SENS_I2C_SLAVE_ADDR1_S  0

#define SENS_SAR_SLAVE_ADDR2_REG          (DR_REG_SENS_BASE + 0x0040)
/* SENS_I2C_SLAVE_ADDR2 : R/W ;bitpos:[21:11] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR2  0x000007FF
#define SENS_I2C_SLAVE_ADDR2_M  ((SENS_I2C_SLAVE_ADDR2_V)<<(SENS_I2C_SLAVE_ADDR2_S))
#define SENS_I2C_SLAVE_ADDR2_V  0x7FF
#define SENS_I2C_SLAVE_ADDR2_S  11
/* SENS_I2C_SLAVE_ADDR3 : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR3  0x000007FF
#define SENS_I2C_SLAVE_ADDR3_M  ((SENS_I2C_SLAVE_ADDR3_V)<<(SENS_I2C_SLAVE_ADDR3_S))
#define SENS_I2C_SLAVE_ADDR3_V  0x7FF
#define SENS_I2C_SLAVE_ADDR3_S  0

#define SENS_SAR_SLAVE_ADDR3_REG          (DR_REG_SENS_BASE + 0x0044)
/* SENS_TSENS_RDY_OUT : RO ;bitpos:[30] ;default: 1'h0 ; */
/*description: indicate temperature sensor out ready*/
#define SENS_TSENS_RDY_OUT  (BIT(30))
#define SENS_TSENS_RDY_OUT_M  (BIT(30))
#define SENS_TSENS_RDY_OUT_V  0x1
#define SENS_TSENS_RDY_OUT_S  30
/* SENS_TSENS_OUT : RO ;bitpos:[29:22] ;default: 8'h0 ; */
/*description: temperature sensor data out*/
#define SENS_TSENS_OUT  0x000000FF
#define SENS_TSENS_OUT_M  ((SENS_TSENS_OUT_V)<<(SENS_TSENS_OUT_S))
#define SENS_TSENS_OUT_V  0xFF
#define SENS_TSENS_OUT_S  22
/* SENS_I2C_SLAVE_ADDR4 : R/W ;bitpos:[21:11] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR4  0x000007FF
#define SENS_I2C_SLAVE_ADDR4_M  ((SENS_I2C_SLAVE_ADDR4_V)<<(SENS_I2C_SLAVE_ADDR4_S))
#define SENS_I2C_SLAVE_ADDR4_V  0x7FF
#define SENS_I2C_SLAVE_ADDR4_S  11
/* SENS_I2C_SLAVE_ADDR5 : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR5  0x000007FF
#define SENS_I2C_SLAVE_ADDR5_M  ((SENS_I2C_SLAVE_ADDR5_V)<<(SENS_I2C_SLAVE_ADDR5_S))
#define SENS_I2C_SLAVE_ADDR5_V  0x7FF
#define SENS_I2C_SLAVE_ADDR5_S  0

#define SENS_SAR_SLAVE_ADDR4_REG          (DR_REG_SENS_BASE + 0x0048)
/* SENS_I2C_DONE : RO ;bitpos:[30] ;default: 1'h0 ; */
/*description: indicate I2C done*/
#define SENS_I2C_DONE  (BIT(30))
#define SENS_I2C_DONE_M  (BIT(30))
#define SENS_I2C_DONE_V  0x1
#define SENS_I2C_DONE_S  30
/* SENS_I2C_RDATA : RO ;bitpos:[29:22] ;default: 8'h0 ; */
/*description: I2C read data*/
#define SENS_I2C_RDATA  0x000000FF
#define SENS_I2C_RDATA_M  ((SENS_I2C_RDATA_V)<<(SENS_I2C_RDATA_S))
#define SENS_I2C_RDATA_V  0xFF
#define SENS_I2C_RDATA_S  22
/* SENS_I2C_SLAVE_ADDR6 : R/W ;bitpos:[21:11] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR6  0x000007FF
#define SENS_I2C_SLAVE_ADDR6_M  ((SENS_I2C_SLAVE_ADDR6_V)<<(SENS_I2C_SLAVE_ADDR6_S))
#define SENS_I2C_SLAVE_ADDR6_V  0x7FF
#define SENS_I2C_SLAVE_ADDR6_S  11
/* SENS_I2C_SLAVE_ADDR7 : R/W ;bitpos:[10:0] ;default: 11'h0 ; */
/*description: */
#define SENS_I2C_SLAVE_ADDR7  0x000007FF
#define SENS_I2C_SLAVE_ADDR7_M  ((SENS_I2C_SLAVE_ADDR7_V)<<(SENS_I2C_SLAVE_ADDR7_S))
#define SENS_I2C_SLAVE_ADDR7_V  0x7FF
#define SENS_I2C_SLAVE_ADDR7_S  0

#define SENS_SAR_TSENS_CTRL_REG          (DR_REG_SENS_BASE + 0x004c)
/* SENS_TSENS_DUMP_OUT : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: temperature sensor dump out  only active when reg_tsens_power_up_force = 1*/
#define SENS_TSENS_DUMP_OUT  (BIT(26))
#define SENS_TSENS_DUMP_OUT_M  (BIT(26))
#define SENS_TSENS_DUMP_OUT_V  0x1
#define SENS_TSENS_DUMP_OUT_S  26
/* SENS_TSENS_POWER_UP_FORCE : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: 1: dump out & power up controlled by SW  0: by FSM*/
#define SENS_TSENS_POWER_UP_FORCE  (BIT(25))
#define SENS_TSENS_POWER_UP_FORCE_M  (BIT(25))
#define SENS_TSENS_POWER_UP_FORCE_V  0x1
#define SENS_TSENS_POWER_UP_FORCE_S  25
/* SENS_TSENS_POWER_UP : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: temperature sensor power up*/
#define SENS_TSENS_POWER_UP  (BIT(24))
#define SENS_TSENS_POWER_UP_M  (BIT(24))
#define SENS_TSENS_POWER_UP_V  0x1
#define SENS_TSENS_POWER_UP_S  24
/* SENS_TSENS_CLK_DIV : R/W ;bitpos:[23:16] ;default: 8'd6 ; */
/*description: temperature sensor clock divider*/
#define SENS_TSENS_CLK_DIV  0x000000FF
#define SENS_TSENS_CLK_DIV_M  ((SENS_TSENS_CLK_DIV_V)<<(SENS_TSENS_CLK_DIV_S))
#define SENS_TSENS_CLK_DIV_V  0xFF
#define SENS_TSENS_CLK_DIV_S  16
/* SENS_TSENS_IN_INV : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: invert temperature sensor data*/
#define SENS_TSENS_IN_INV  (BIT(15))
#define SENS_TSENS_IN_INV_M  (BIT(15))
#define SENS_TSENS_IN_INV_V  0x1
#define SENS_TSENS_IN_INV_S  15
/* SENS_TSENS_CLK_GATED : R/W ;bitpos:[14] ;default: 1'b1 ; */
/*description: */
#define SENS_TSENS_CLK_GATED  (BIT(14))
#define SENS_TSENS_CLK_GATED_M  (BIT(14))
#define SENS_TSENS_CLK_GATED_V  0x1
#define SENS_TSENS_CLK_GATED_S  14
/* SENS_TSENS_CLK_INV : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: */
#define SENS_TSENS_CLK_INV  (BIT(13))
#define SENS_TSENS_CLK_INV_M  (BIT(13))
#define SENS_TSENS_CLK_INV_V  0x1
#define SENS_TSENS_CLK_INV_S  13
/* SENS_TSENS_XPD_FORCE : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SENS_TSENS_XPD_FORCE  (BIT(12))
#define SENS_TSENS_XPD_FORCE_M  (BIT(12))
#define SENS_TSENS_XPD_FORCE_V  0x1
#define SENS_TSENS_XPD_FORCE_S  12
/* SENS_TSENS_XPD_WAIT : R/W ;bitpos:[11:0] ;default: 12'h2 ; */
/*description: */
#define SENS_TSENS_XPD_WAIT  0x00000FFF
#define SENS_TSENS_XPD_WAIT_M  ((SENS_TSENS_XPD_WAIT_V)<<(SENS_TSENS_XPD_WAIT_S))
#define SENS_TSENS_XPD_WAIT_V  0xFFF
#define SENS_TSENS_XPD_WAIT_S  0

#define SENS_SAR_I2C_CTRL_REG          (DR_REG_SENS_BASE + 0x0050)
/* SENS_SAR_I2C_START_FORCE : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: 1: I2C started by SW  0: I2C started by FSM*/
#define SENS_SAR_I2C_START_FORCE  (BIT(29))
#define SENS_SAR_I2C_START_FORCE_M  (BIT(29))
#define SENS_SAR_I2C_START_FORCE_V  0x1
#define SENS_SAR_I2C_START_FORCE_S  29
/* SENS_SAR_I2C_START : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: start I2C  only active when reg_sar_i2c_start_force = 1*/
#define SENS_SAR_I2C_START  (BIT(28))
#define SENS_SAR_I2C_START_M  (BIT(28))
#define SENS_SAR_I2C_START_V  0x1
#define SENS_SAR_I2C_START_S  28
/* SENS_SAR_I2C_CTRL : R/W ;bitpos:[27:0] ;default: 28'b0 ; */
/*description: I2C control data  only active when reg_sar_i2c_start_force = 1*/
#define SENS_SAR_I2C_CTRL  0x0FFFFFFF
#define SENS_SAR_I2C_CTRL_M  ((SENS_SAR_I2C_CTRL_V)<<(SENS_SAR_I2C_CTRL_S))
#define SENS_SAR_I2C_CTRL_V  0xFFFFFFF
#define SENS_SAR_I2C_CTRL_S  0

#define SENS_SAR_MEAS_START1_REG          (DR_REG_SENS_BASE + 0x0054)
/* SENS_SAR1_EN_PAD_FORCE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 1: SAR ADC1 pad enable bitmap is controlled by SW  0: SAR ADC1
 pad enable bitmap is controlled by ULP-coprocessor*/
#define SENS_SAR1_EN_PAD_FORCE  (BIT(31))
#define SENS_SAR1_EN_PAD_FORCE_M  (BIT(31))
#define SENS_SAR1_EN_PAD_FORCE_V  0x1
#define SENS_SAR1_EN_PAD_FORCE_S  31
/* SENS_SAR1_EN_PAD : R/W ;bitpos:[30:19] ;default: 12'b0 ; */
/*description: SAR ADC1 pad enable bitmap  only active when reg_sar1_en_pad_force = 1*/
#define SENS_SAR1_EN_PAD  0x00000FFF
#define SENS_SAR1_EN_PAD_M  ((SENS_SAR1_EN_PAD_V)<<(SENS_SAR1_EN_PAD_S))
#define SENS_SAR1_EN_PAD_V  0xFFF
#define SENS_SAR1_EN_PAD_S  19
/* SENS_MEAS1_START_FORCE : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: 1: SAR ADC1 controller (in RTC) is started by SW  0: SAR ADC1
 controller is started by ULP-coprocessor*/
#define SENS_MEAS1_START_FORCE  (BIT(18))
#define SENS_MEAS1_START_FORCE_M  (BIT(18))
#define SENS_MEAS1_START_FORCE_V  0x1
#define SENS_MEAS1_START_FORCE_S  18
/* SENS_MEAS1_START_SAR : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: SAR ADC1 controller (in RTC) starts conversion  only active when
 reg_meas1_start_force = 1*/
#define SENS_MEAS1_START_SAR  (BIT(17))
#define SENS_MEAS1_START_SAR_M  (BIT(17))
#define SENS_MEAS1_START_SAR_V  0x1
#define SENS_MEAS1_START_SAR_S  17
/* SENS_MEAS1_DONE_SAR : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: SAR ADC1 conversion done indication*/
#define SENS_MEAS1_DONE_SAR  (BIT(16))
#define SENS_MEAS1_DONE_SAR_M  (BIT(16))
#define SENS_MEAS1_DONE_SAR_V  0x1
#define SENS_MEAS1_DONE_SAR_S  16
/* SENS_MEAS1_DATA_SAR : RO ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: SAR ADC1 data*/
#define SENS_MEAS1_DATA_SAR  0x0000FFFF
#define SENS_MEAS1_DATA_SAR_M  ((SENS_MEAS1_DATA_SAR_V)<<(SENS_MEAS1_DATA_SAR_S))
#define SENS_MEAS1_DATA_SAR_V  0xFFFF
#define SENS_MEAS1_DATA_SAR_S  0

#define SENS_SAR_TOUCH_CTRL1_REG          (DR_REG_SENS_BASE + 0x0058)
/* SENS_HALL_PHASE_FORCE : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: 1: HALL PHASE is controlled by SW  0: HALL PHASE is controlled
 by FSM in ULP-coprocessor*/
#define SENS_HALL_PHASE_FORCE  (BIT(27))
#define SENS_HALL_PHASE_FORCE_M  (BIT(27))
#define SENS_HALL_PHASE_FORCE_V  0x1
#define SENS_HALL_PHASE_FORCE_S  27
/* SENS_XPD_HALL_FORCE : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: 1: XPD HALL is controlled by SW. 0: XPD HALL is controlled by
 FSM in ULP-coprocessor*/
#define SENS_XPD_HALL_FORCE  (BIT(26))
#define SENS_XPD_HALL_FORCE_M  (BIT(26))
#define SENS_XPD_HALL_FORCE_V  0x1
#define SENS_XPD_HALL_FORCE_S  26
/* SENS_TOUCH_OUT_1EN : R/W ;bitpos:[25] ;default: 1'b1 ; */
/*description: 1: wakeup interrupt is generated if SET1 is "touched"  0:
 wakeup interrupt is generated only if SET1 & SET2 is both "touched"*/
#define SENS_TOUCH_OUT_1EN  (BIT(25))
#define SENS_TOUCH_OUT_1EN_M  (BIT(25))
#define SENS_TOUCH_OUT_1EN_V  0x1
#define SENS_TOUCH_OUT_1EN_S  25
/* SENS_TOUCH_OUT_SEL : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: 1: when the counter is greater then the threshold  the touch
 pad is considered as "touched"  0: when the counter is less than the threshold  the touch pad is considered as "touched"*/
#define SENS_TOUCH_OUT_SEL  (BIT(24))
#define SENS_TOUCH_OUT_SEL_M  (BIT(24))
#define SENS_TOUCH_OUT_SEL_V  0x1
#define SENS_TOUCH_OUT_SEL_S  24
/* SENS_TOUCH_XPD_WAIT : R/W ;bitpos:[23:16] ;default: 8'h4 ; */
/*description: the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD*/
#define SENS_TOUCH_XPD_WAIT  0x000000FF
#define SENS_TOUCH_XPD_WAIT_M  ((SENS_TOUCH_XPD_WAIT_V)<<(SENS_TOUCH_XPD_WAIT_S))
#define SENS_TOUCH_XPD_WAIT_V  0xFF
#define SENS_TOUCH_XPD_WAIT_S  16
/* SENS_TOUCH_MEAS_DELAY : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: the meas length (in 8MHz)*/
#define SENS_TOUCH_MEAS_DELAY  0x0000FFFF
#define SENS_TOUCH_MEAS_DELAY_M  ((SENS_TOUCH_MEAS_DELAY_V)<<(SENS_TOUCH_MEAS_DELAY_S))
#define SENS_TOUCH_MEAS_DELAY_V  0xFFFF
#define SENS_TOUCH_MEAS_DELAY_S  0

#define SENS_SAR_TOUCH_THRES1_REG          (DR_REG_SENS_BASE + 0x005c)
/* SENS_TOUCH_OUT_TH0 : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the threshold for touch pad 0*/
#define SENS_TOUCH_OUT_TH0  0x0000FFFF
#define SENS_TOUCH_OUT_TH0_M  ((SENS_TOUCH_OUT_TH0_V)<<(SENS_TOUCH_OUT_TH0_S))
#define SENS_TOUCH_OUT_TH0_V  0xFFFF
#define SENS_TOUCH_OUT_TH0_S  16
/* SENS_TOUCH_OUT_TH1 : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the threshold for touch pad 1*/
#define SENS_TOUCH_OUT_TH1  0x0000FFFF
#define SENS_TOUCH_OUT_TH1_M  ((SENS_TOUCH_OUT_TH1_V)<<(SENS_TOUCH_OUT_TH1_S))
#define SENS_TOUCH_OUT_TH1_V  0xFFFF
#define SENS_TOUCH_OUT_TH1_S  0

#define SENS_SAR_TOUCH_THRES2_REG          (DR_REG_SENS_BASE + 0x0060)
/* SENS_TOUCH_OUT_TH2 : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the threshold for touch pad 2*/
#define SENS_TOUCH_OUT_TH2  0x0000FFFF
#define SENS_TOUCH_OUT_TH2_M  ((SENS_TOUCH_OUT_TH2_V)<<(SENS_TOUCH_OUT_TH2_S))
#define SENS_TOUCH_OUT_TH2_V  0xFFFF
#define SENS_TOUCH_OUT_TH2_S  16
/* SENS_TOUCH_OUT_TH3 : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the threshold for touch pad 3*/
#define SENS_TOUCH_OUT_TH3  0x0000FFFF
#define SENS_TOUCH_OUT_TH3_M  ((SENS_TOUCH_OUT_TH3_V)<<(SENS_TOUCH_OUT_TH3_S))
#define SENS_TOUCH_OUT_TH3_V  0xFFFF
#define SENS_TOUCH_OUT_TH3_S  0

#define SENS_SAR_TOUCH_THRES3_REG          (DR_REG_SENS_BASE + 0x0064)
/* SENS_TOUCH_OUT_TH4 : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the threshold for touch pad 4*/
#define SENS_TOUCH_OUT_TH4  0x0000FFFF
#define SENS_TOUCH_OUT_TH4_M  ((SENS_TOUCH_OUT_TH4_V)<<(SENS_TOUCH_OUT_TH4_S))
#define SENS_TOUCH_OUT_TH4_V  0xFFFF
#define SENS_TOUCH_OUT_TH4_S  16
/* SENS_TOUCH_OUT_TH5 : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the threshold for touch pad 5*/
#define SENS_TOUCH_OUT_TH5  0x0000FFFF
#define SENS_TOUCH_OUT_TH5_M  ((SENS_TOUCH_OUT_TH5_V)<<(SENS_TOUCH_OUT_TH5_S))
#define SENS_TOUCH_OUT_TH5_V  0xFFFF
#define SENS_TOUCH_OUT_TH5_S  0

#define SENS_SAR_TOUCH_THRES4_REG          (DR_REG_SENS_BASE + 0x0068)
/* SENS_TOUCH_OUT_TH6 : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the threshold for touch pad 6*/
#define SENS_TOUCH_OUT_TH6  0x0000FFFF
#define SENS_TOUCH_OUT_TH6_M  ((SENS_TOUCH_OUT_TH6_V)<<(SENS_TOUCH_OUT_TH6_S))
#define SENS_TOUCH_OUT_TH6_V  0xFFFF
#define SENS_TOUCH_OUT_TH6_S  16
/* SENS_TOUCH_OUT_TH7 : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the threshold for touch pad 7*/
#define SENS_TOUCH_OUT_TH7  0x0000FFFF
#define SENS_TOUCH_OUT_TH7_M  ((SENS_TOUCH_OUT_TH7_V)<<(SENS_TOUCH_OUT_TH7_S))
#define SENS_TOUCH_OUT_TH7_V  0xFFFF
#define SENS_TOUCH_OUT_TH7_S  0

#define SENS_SAR_TOUCH_THRES5_REG          (DR_REG_SENS_BASE + 0x006c)
/* SENS_TOUCH_OUT_TH8 : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the threshold for touch pad 8*/
#define SENS_TOUCH_OUT_TH8  0x0000FFFF
#define SENS_TOUCH_OUT_TH8_M  ((SENS_TOUCH_OUT_TH8_V)<<(SENS_TOUCH_OUT_TH8_S))
#define SENS_TOUCH_OUT_TH8_V  0xFFFF
#define SENS_TOUCH_OUT_TH8_S  16
/* SENS_TOUCH_OUT_TH9 : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the threshold for touch pad 9*/
#define SENS_TOUCH_OUT_TH9  0x0000FFFF
#define SENS_TOUCH_OUT_TH9_M  ((SENS_TOUCH_OUT_TH9_V)<<(SENS_TOUCH_OUT_TH9_S))
#define SENS_TOUCH_OUT_TH9_V  0xFFFF
#define SENS_TOUCH_OUT_TH9_S  0

#define SENS_SAR_TOUCH_OUT1_REG          (DR_REG_SENS_BASE + 0x0070)
/* SENS_TOUCH_MEAS_OUT0 : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the counter for touch pad 0*/
#define SENS_TOUCH_MEAS_OUT0  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT0_M  ((SENS_TOUCH_MEAS_OUT0_V)<<(SENS_TOUCH_MEAS_OUT0_S))
#define SENS_TOUCH_MEAS_OUT0_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT0_S  16
/* SENS_TOUCH_MEAS_OUT1 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the counter for touch pad 1*/
#define SENS_TOUCH_MEAS_OUT1  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT1_M  ((SENS_TOUCH_MEAS_OUT1_V)<<(SENS_TOUCH_MEAS_OUT1_S))
#define SENS_TOUCH_MEAS_OUT1_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT1_S  0

#define SENS_SAR_TOUCH_OUT2_REG          (DR_REG_SENS_BASE + 0x0074)
/* SENS_TOUCH_MEAS_OUT2 : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the counter for touch pad 2*/
#define SENS_TOUCH_MEAS_OUT2  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT2_M  ((SENS_TOUCH_MEAS_OUT2_V)<<(SENS_TOUCH_MEAS_OUT2_S))
#define SENS_TOUCH_MEAS_OUT2_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT2_S  16
/* SENS_TOUCH_MEAS_OUT3 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the counter for touch pad 3*/
#define SENS_TOUCH_MEAS_OUT3  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT3_M  ((SENS_TOUCH_MEAS_OUT3_V)<<(SENS_TOUCH_MEAS_OUT3_S))
#define SENS_TOUCH_MEAS_OUT3_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT3_S  0

#define SENS_SAR_TOUCH_OUT3_REG          (DR_REG_SENS_BASE + 0x0078)
/* SENS_TOUCH_MEAS_OUT4 : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the counter for touch pad 4*/
#define SENS_TOUCH_MEAS_OUT4  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT4_M  ((SENS_TOUCH_MEAS_OUT4_V)<<(SENS_TOUCH_MEAS_OUT4_S))
#define SENS_TOUCH_MEAS_OUT4_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT4_S  16
/* SENS_TOUCH_MEAS_OUT5 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the counter for touch pad 5*/
#define SENS_TOUCH_MEAS_OUT5  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT5_M  ((SENS_TOUCH_MEAS_OUT5_V)<<(SENS_TOUCH_MEAS_OUT5_S))
#define SENS_TOUCH_MEAS_OUT5_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT5_S  0

#define SENS_SAR_TOUCH_OUT4_REG          (DR_REG_SENS_BASE + 0x007c)
/* SENS_TOUCH_MEAS_OUT6 : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the counter for touch pad 6*/
#define SENS_TOUCH_MEAS_OUT6  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT6_M  ((SENS_TOUCH_MEAS_OUT6_V)<<(SENS_TOUCH_MEAS_OUT6_S))
#define SENS_TOUCH_MEAS_OUT6_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT6_S  16
/* SENS_TOUCH_MEAS_OUT7 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the counter for touch pad 7*/
#define SENS_TOUCH_MEAS_OUT7  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT7_M  ((SENS_TOUCH_MEAS_OUT7_V)<<(SENS_TOUCH_MEAS_OUT7_S))
#define SENS_TOUCH_MEAS_OUT7_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT7_S  0

#define SENS_SAR_TOUCH_OUT5_REG          (DR_REG_SENS_BASE + 0x0080)
/* SENS_TOUCH_MEAS_OUT8 : RO ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: the counter for touch pad 8*/
#define SENS_TOUCH_MEAS_OUT8  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT8_M  ((SENS_TOUCH_MEAS_OUT8_V)<<(SENS_TOUCH_MEAS_OUT8_S))
#define SENS_TOUCH_MEAS_OUT8_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT8_S  16
/* SENS_TOUCH_MEAS_OUT9 : RO ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: the counter for touch pad 9*/
#define SENS_TOUCH_MEAS_OUT9  0x0000FFFF
#define SENS_TOUCH_MEAS_OUT9_M  ((SENS_TOUCH_MEAS_OUT9_V)<<(SENS_TOUCH_MEAS_OUT9_S))
#define SENS_TOUCH_MEAS_OUT9_V  0xFFFF
#define SENS_TOUCH_MEAS_OUT9_S  0

#define SENS_SAR_TOUCH_CTRL2_REG          (DR_REG_SENS_BASE + 0x0084)
/* SENS_TOUCH_MEAS_EN_CLR : WO ;bitpos:[30] ;default: 1'h0 ; */
/*description: to clear reg_touch_meas_en*/
#define SENS_TOUCH_MEAS_EN_CLR  (BIT(30))
#define SENS_TOUCH_MEAS_EN_CLR_M  (BIT(30))
#define SENS_TOUCH_MEAS_EN_CLR_V  0x1
#define SENS_TOUCH_MEAS_EN_CLR_S  30
/* SENS_TOUCH_SLEEP_CYCLES : R/W ;bitpos:[29:14] ;default: 16'h100 ; */
/*description: sleep cycles for timer*/
#define SENS_TOUCH_SLEEP_CYCLES  0x0000FFFF
#define SENS_TOUCH_SLEEP_CYCLES_M  ((SENS_TOUCH_SLEEP_CYCLES_V)<<(SENS_TOUCH_SLEEP_CYCLES_S))
#define SENS_TOUCH_SLEEP_CYCLES_V  0xFFFF
#define SENS_TOUCH_SLEEP_CYCLES_S  14
/* SENS_TOUCH_START_FORCE : R/W ;bitpos:[13] ;default: 1'h0 ; */
/*description: 1: to start touch fsm by SW  0: to start touch fsm by timer*/
#define SENS_TOUCH_START_FORCE  (BIT(13))
#define SENS_TOUCH_START_FORCE_M  (BIT(13))
#define SENS_TOUCH_START_FORCE_V  0x1
#define SENS_TOUCH_START_FORCE_S  13
/* SENS_TOUCH_START_EN : R/W ;bitpos:[12] ;default: 1'h0 ; */
/*description: 1: start touch fsm  valid when reg_touch_start_force is set*/
#define SENS_TOUCH_START_EN  (BIT(12))
#define SENS_TOUCH_START_EN_M  (BIT(12))
#define SENS_TOUCH_START_EN_V  0x1
#define SENS_TOUCH_START_EN_S  12
/* SENS_TOUCH_START_FSM_EN : R/W ;bitpos:[11] ;default: 1'h1 ; */
/*description: 1: TOUCH_START & TOUCH_XPD is controlled by touch fsm  0: TOUCH_START
 & TOUCH_XPD is controlled by registers*/
#define SENS_TOUCH_START_FSM_EN  (BIT(11))
#define SENS_TOUCH_START_FSM_EN_M  (BIT(11))
#define SENS_TOUCH_START_FSM_EN_V  0x1
#define SENS_TOUCH_START_FSM_EN_S  11
/* SENS_TOUCH_MEAS_DONE : RO ;bitpos:[10] ;default: 1'h0 ; */
/*description: fsm set 1 to indicate touch touch meas is done*/
#define SENS_TOUCH_MEAS_DONE  (BIT(10))
#define SENS_TOUCH_MEAS_DONE_M  (BIT(10))
#define SENS_TOUCH_MEAS_DONE_V  0x1
#define SENS_TOUCH_MEAS_DONE_S  10
/* SENS_TOUCH_MEAS_EN : RO ;bitpos:[9:0] ;default: 10'h0 ; */
/*description: 10-bit register to indicate which pads are "touched"*/
#define SENS_TOUCH_MEAS_EN  0x000003FF
#define SENS_TOUCH_MEAS_EN_M  ((SENS_TOUCH_MEAS_EN_V)<<(SENS_TOUCH_MEAS_EN_S))
#define SENS_TOUCH_MEAS_EN_V  0x3FF
#define SENS_TOUCH_MEAS_EN_S  0

#define SENS_SAR_TOUCH_ENABLE_REG          (DR_REG_SENS_BASE + 0x008c)
/* SENS_TOUCH_PAD_OUTEN1 : R/W ;bitpos:[29:20] ;default: 10'h3ff ; */
/*description: Bitmap defining SET1 for generating wakeup interrupt. SET1 is
 "touched" only if at least one of touch pad in SET1 is "touched".*/
#define SENS_TOUCH_PAD_OUTEN1  0x000003FF
#define SENS_TOUCH_PAD_OUTEN1_M  ((SENS_TOUCH_PAD_OUTEN1_V)<<(SENS_TOUCH_PAD_OUTEN1_S))
#define SENS_TOUCH_PAD_OUTEN1_V  0x3FF
#define SENS_TOUCH_PAD_OUTEN1_S  20
/* SENS_TOUCH_PAD_OUTEN2 : R/W ;bitpos:[19:10] ;default: 10'h3ff ; */
/*description: Bitmap defining SET2 for generating wakeup interrupt. SET2 is
 "touched" only if at least one of touch pad in SET2 is "touched".*/
#define SENS_TOUCH_PAD_OUTEN2  0x000003FF
#define SENS_TOUCH_PAD_OUTEN2_M  ((SENS_TOUCH_PAD_OUTEN2_V)<<(SENS_TOUCH_PAD_OUTEN2_S))
#define SENS_TOUCH_PAD_OUTEN2_V  0x3FF
#define SENS_TOUCH_PAD_OUTEN2_S  10
/* SENS_TOUCH_PAD_WORKEN : R/W ;bitpos:[9:0] ;default: 10'h3ff ; */
/*description: Bitmap defining the working set during the measurement.*/
#define SENS_TOUCH_PAD_WORKEN  0x000003FF
#define SENS_TOUCH_PAD_WORKEN_M  ((SENS_TOUCH_PAD_WORKEN_V)<<(SENS_TOUCH_PAD_WORKEN_S))
#define SENS_TOUCH_PAD_WORKEN_V  0x3FF
#define SENS_TOUCH_PAD_WORKEN_S  0

#define SENS_SAR_READ_CTRL2_REG          (DR_REG_SENS_BASE + 0x0090)
/* SENS_SAR2_DATA_INV : R/W ;bitpos:[29] ;default: 1'b0 ; */
/*description: Invert SAR ADC2 data*/
#define SENS_SAR2_DATA_INV  (BIT(29))
#define SENS_SAR2_DATA_INV_M  (BIT(29))
#define SENS_SAR2_DATA_INV_V  0x1
#define SENS_SAR2_DATA_INV_S  29
/* SENS_SAR2_DIG_FORCE : R/W ;bitpos:[28] ;default: 1'b0 ; */
/*description: 1: SAR ADC2 controlled by DIG ADC2 CTRL or PWDET CTRL  0: SAR
 ADC2 controlled by RTC ADC2 CTRL*/
#define SENS_SAR2_DIG_FORCE  (BIT(28))
#define SENS_SAR2_DIG_FORCE_M  (BIT(28))
#define SENS_SAR2_DIG_FORCE_V  0x1
#define SENS_SAR2_DIG_FORCE_S  28
/* SENS_SAR2_PWDET_FORCE : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define SENS_SAR2_PWDET_FORCE  (BIT(27))
#define SENS_SAR2_PWDET_FORCE_M  (BIT(27))
#define SENS_SAR2_PWDET_FORCE_V  0x1
#define SENS_SAR2_PWDET_FORCE_S  27
/* SENS_SAR2_SAMPLE_NUM : R/W ;bitpos:[26:19] ;default: 8'd0 ; */
/*description: */
#define SENS_SAR2_SAMPLE_NUM  0x000000FF
#define SENS_SAR2_SAMPLE_NUM_M  ((SENS_SAR2_SAMPLE_NUM_V)<<(SENS_SAR2_SAMPLE_NUM_S))
#define SENS_SAR2_SAMPLE_NUM_V  0xFF
#define SENS_SAR2_SAMPLE_NUM_S  19
/* SENS_SAR2_CLK_GATED : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: */
#define SENS_SAR2_CLK_GATED  (BIT(18))
#define SENS_SAR2_CLK_GATED_M  (BIT(18))
#define SENS_SAR2_CLK_GATED_V  0x1
#define SENS_SAR2_CLK_GATED_S  18
/* SENS_SAR2_SAMPLE_BIT : R/W ;bitpos:[17:16] ;default: 2'd3 ; */
/*description: 00: for 9-bit width  01: for 10-bit width  10: for 11-bit width
  11: for 12-bit width*/
#define SENS_SAR2_SAMPLE_BIT  0x00000003
#define SENS_SAR2_SAMPLE_BIT_M  ((SENS_SAR2_SAMPLE_BIT_V)<<(SENS_SAR2_SAMPLE_BIT_S))
#define SENS_SAR2_SAMPLE_BIT_V  0x3
#define SENS_SAR2_SAMPLE_BIT_S  16
/* SENS_SAR2_SAMPLE_CYCLE : R/W ;bitpos:[15:8] ;default: 8'd9 ; */
/*description: sample cycles for SAR ADC2*/
#define SENS_SAR2_SAMPLE_CYCLE  0x000000FF
#define SENS_SAR2_SAMPLE_CYCLE_M  ((SENS_SAR2_SAMPLE_CYCLE_V)<<(SENS_SAR2_SAMPLE_CYCLE_S))
#define SENS_SAR2_SAMPLE_CYCLE_V  0xFF
#define SENS_SAR2_SAMPLE_CYCLE_S  8
/* SENS_SAR2_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'd2 ; */
/*description: clock divider*/
#define SENS_SAR2_CLK_DIV  0x000000FF
#define SENS_SAR2_CLK_DIV_M  ((SENS_SAR2_CLK_DIV_V)<<(SENS_SAR2_CLK_DIV_S))
#define SENS_SAR2_CLK_DIV_V  0xFF
#define SENS_SAR2_CLK_DIV_S  0

#define SENS_SAR_MEAS_START2_REG          (DR_REG_SENS_BASE + 0x0094)
/* SENS_SAR2_EN_PAD_FORCE : R/W ;bitpos:[31] ;default: 1'b0 ; */
/*description: 1: SAR ADC2 pad enable bitmap is controlled by SW  0: SAR ADC2
 pad enable bitmap is controlled by ULP-coprocessor*/
#define SENS_SAR2_EN_PAD_FORCE  (BIT(31))
#define SENS_SAR2_EN_PAD_FORCE_M  (BIT(31))
#define SENS_SAR2_EN_PAD_FORCE_V  0x1
#define SENS_SAR2_EN_PAD_FORCE_S  31
/* SENS_SAR2_EN_PAD : R/W ;bitpos:[30:19] ;default: 12'b0 ; */
/*description: SAR ADC2 pad enable bitmap  only active when reg_sar2_en_pad_force = 1*/
#define SENS_SAR2_EN_PAD  0x00000FFF
#define SENS_SAR2_EN_PAD_M  ((SENS_SAR2_EN_PAD_V)<<(SENS_SAR2_EN_PAD_S))
#define SENS_SAR2_EN_PAD_V  0xFFF
#define SENS_SAR2_EN_PAD_S  19
/* SENS_MEAS2_START_FORCE : R/W ;bitpos:[18] ;default: 1'b0 ; */
/*description: 1: SAR ADC2 controller (in RTC) is started by SW  0: SAR ADC2
 controller is started by ULP-coprocessor*/
#define SENS_MEAS2_START_FORCE  (BIT(18))
#define SENS_MEAS2_START_FORCE_M  (BIT(18))
#define SENS_MEAS2_START_FORCE_V  0x1
#define SENS_MEAS2_START_FORCE_S  18
/* SENS_MEAS2_START_SAR : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: SAR ADC2 controller (in RTC) starts conversion  only active when
 reg_meas2_start_force = 1*/
#define SENS_MEAS2_START_SAR  (BIT(17))
#define SENS_MEAS2_START_SAR_M  (BIT(17))
#define SENS_MEAS2_START_SAR_V  0x1
#define SENS_MEAS2_START_SAR_S  17
/* SENS_MEAS2_DONE_SAR : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: SAR ADC2 conversion done indication*/
#define SENS_MEAS2_DONE_SAR  (BIT(16))
#define SENS_MEAS2_DONE_SAR_M  (BIT(16))
#define SENS_MEAS2_DONE_SAR_V  0x1
#define SENS_MEAS2_DONE_SAR_S  16
/* SENS_MEAS2_DATA_SAR : RO ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: SAR ADC2 data*/
#define SENS_MEAS2_DATA_SAR  0x0000FFFF
#define SENS_MEAS2_DATA_SAR_M  ((SENS_MEAS2_DATA_SAR_V)<<(SENS_MEAS2_DATA_SAR_S))
#define SENS_MEAS2_DATA_SAR_V  0xFFFF
#define SENS_MEAS2_DATA_SAR_S  0

#define SENS_SAR_DAC_CTRL1_REG          (DR_REG_SENS_BASE + 0x0098)
/* SENS_DAC_CLK_INV : R/W ;bitpos:[25] ;default: 1'b0 ; */
/*description: 1: invert PDAC_CLK*/
#define SENS_DAC_CLK_INV  (BIT(25))
#define SENS_DAC_CLK_INV_M  (BIT(25))
#define SENS_DAC_CLK_INV_V  0x1
#define SENS_DAC_CLK_INV_S  25
/* SENS_DAC_CLK_FORCE_HIGH : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: 1: force PDAC_CLK to high*/
#define SENS_DAC_CLK_FORCE_HIGH  (BIT(24))
#define SENS_DAC_CLK_FORCE_HIGH_M  (BIT(24))
#define SENS_DAC_CLK_FORCE_HIGH_V  0x1
#define SENS_DAC_CLK_FORCE_HIGH_S  24
/* SENS_DAC_CLK_FORCE_LOW : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: 1: force PDAC_CLK to low*/
#define SENS_DAC_CLK_FORCE_LOW  (BIT(23))
#define SENS_DAC_CLK_FORCE_LOW_M  (BIT(23))
#define SENS_DAC_CLK_FORCE_LOW_V  0x1
#define SENS_DAC_CLK_FORCE_LOW_S  23
/* SENS_DAC_DIG_FORCE : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: 1: DAC1 & DAC2 use DMA  0: DAC1 & DAC2 do not use DMA*/
#define SENS_DAC_DIG_FORCE  (BIT(22))
#define SENS_DAC_DIG_FORCE_M  (BIT(22))
#define SENS_DAC_DIG_FORCE_V  0x1
#define SENS_DAC_DIG_FORCE_S  22
/* SENS_DEBUG_BIT_SEL : R/W ;bitpos:[21:17] ;default: 5'b0 ; */
/*description: */
#define SENS_DEBUG_BIT_SEL  0x0000001F
#define SENS_DEBUG_BIT_SEL_M  ((SENS_DEBUG_BIT_SEL_V)<<(SENS_DEBUG_BIT_SEL_S))
#define SENS_DEBUG_BIT_SEL_V  0x1F
#define SENS_DEBUG_BIT_SEL_S  17
/* SENS_SW_TONE_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: 1: enable CW generator  0: disable CW generator*/
#define SENS_SW_TONE_EN  (BIT(16))
#define SENS_SW_TONE_EN_M  (BIT(16))
#define SENS_SW_TONE_EN_V  0x1
#define SENS_SW_TONE_EN_S  16
/* SENS_SW_FSTEP : R/W ;bitpos:[15:0] ;default: 16'b0 ; */
/*description: frequency step for CW generator  can be used to adjust the frequency*/
#define SENS_SW_FSTEP  0x0000FFFF
#define SENS_SW_FSTEP_M  ((SENS_SW_FSTEP_V)<<(SENS_SW_FSTEP_S))
#define SENS_SW_FSTEP_V  0xFFFF
#define SENS_SW_FSTEP_S  0

#define SENS_SAR_DAC_CTRL2_REG          (DR_REG_SENS_BASE + 0x009c)
/* SENS_DAC_CW_EN2 : R/W ;bitpos:[25] ;default: 1'b1 ; */
/*description: 1: to select CW generator as source to PDAC2_DAC[7:0]  0: to
 select register reg_pdac2_dac[7:0] as source to PDAC2_DAC[7:0]*/
#define SENS_DAC_CW_EN2  (BIT(25))
#define SENS_DAC_CW_EN2_M  (BIT(25))
#define SENS_DAC_CW_EN2_V  0x1
#define SENS_DAC_CW_EN2_S  25
/* SENS_DAC_CW_EN1 : R/W ;bitpos:[24] ;default: 1'b1 ; */
/*description: 1: to select CW generator as source to PDAC1_DAC[7:0]  0: to
 select register reg_pdac1_dac[7:0] as source to PDAC1_DAC[7:0]*/
#define SENS_DAC_CW_EN1  (BIT(24))
#define SENS_DAC_CW_EN1_M  (BIT(24))
#define SENS_DAC_CW_EN1_V  0x1
#define SENS_DAC_CW_EN1_S  24
/* SENS_DAC_INV2 : R/W ;bitpos:[23:22] ;default: 2'b0 ; */
/*description: 00: do not invert any bits  01: invert all bits  10: invert MSB
  11: invert all bits except MSB*/
#define SENS_DAC_INV2  0x00000003
#define SENS_DAC_INV2_M  ((SENS_DAC_INV2_V)<<(SENS_DAC_INV2_S))
#define SENS_DAC_INV2_V  0x3
#define SENS_DAC_INV2_S  22
/* SENS_DAC_INV1 : R/W ;bitpos:[21:20] ;default: 2'b0 ; */
/*description: 00: do not invert any bits  01: invert all bits  10: invert MSB
  11: invert all bits except MSB*/
#define SENS_DAC_INV1  0x00000003
#define SENS_DAC_INV1_M  ((SENS_DAC_INV1_V)<<(SENS_DAC_INV1_S))
#define SENS_DAC_INV1_V  0x3
#define SENS_DAC_INV1_S  20
/* SENS_DAC_SCALE2 : R/W ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: 00: no scale  01: scale to 1/2  10: scale to 1/4  scale to 1/8*/
#define SENS_DAC_SCALE2  0x00000003
#define SENS_DAC_SCALE2_M  ((SENS_DAC_SCALE2_V)<<(SENS_DAC_SCALE2_S))
#define SENS_DAC_SCALE2_V  0x3
#define SENS_DAC_SCALE2_S  18
/* SENS_DAC_SCALE1 : R/W ;bitpos:[17:16] ;default: 2'b0 ; */
/*description: 00: no scale  01: scale to 1/2  10: scale to 1/4  scale to 1/8*/
#define SENS_DAC_SCALE1  0x00000003
#define SENS_DAC_SCALE1_M  ((SENS_DAC_SCALE1_V)<<(SENS_DAC_SCALE1_S))
#define SENS_DAC_SCALE1_V  0x3
#define SENS_DAC_SCALE1_S  16
/* SENS_DAC_DC2 : R/W ;bitpos:[15:8] ;default: 8'b0 ; */
/*description: DC offset for DAC2 CW generator*/
#define SENS_DAC_DC2  0x000000FF
#define SENS_DAC_DC2_M  ((SENS_DAC_DC2_V)<<(SENS_DAC_DC2_S))
#define SENS_DAC_DC2_V  0xFF
#define SENS_DAC_DC2_S  8
/* SENS_DAC_DC1 : R/W ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: DC offset for DAC1 CW generator*/
#define SENS_DAC_DC1  0x000000FF
#define SENS_DAC_DC1_M  ((SENS_DAC_DC1_V)<<(SENS_DAC_DC1_S))
#define SENS_DAC_DC1_V  0xFF
#define SENS_DAC_DC1_S  0

#define SENS_SAR_MEAS_CTRL2_REG          (DR_REG_SENS_BASE + 0x0a0)
/* SENS_AMP_SHORT_REF_GND_FORCE : R/W ;bitpos:[18:17] ;default: 2'b0 ; */
/*description: */
#define SENS_AMP_SHORT_REF_GND_FORCE  0x00000003
#define SENS_AMP_SHORT_REF_GND_FORCE_M  ((SENS_AMP_SHORT_REF_GND_FORCE_V)<<(SENS_AMP_SHORT_REF_GND_FORCE_S))
#define SENS_AMP_SHORT_REF_GND_FORCE_V  0x3
#define SENS_AMP_SHORT_REF_GND_FORCE_S  17
#define SENS_AMP_SHORT_REF_GND_FORCE_FSM 0 // Use FSM to control power down
#define SENS_AMP_SHORT_REF_GND_FORCE_PD  2 // Force power down
#define SENS_AMP_SHORT_REF_GND_FORCE_PU  3 // Force power up
/* SENS_AMP_SHORT_REF_FORCE : R/W ;bitpos:[16:15] ;default: 2'b0 ; */
/*description: */
#define SENS_AMP_SHORT_REF_FORCE  0x00000003
#define SENS_AMP_SHORT_REF_FORCE_M  ((SENS_AMP_SHORT_REF_FORCE_V)<<(SENS_AMP_SHORT_REF_FORCE_S))
#define SENS_AMP_SHORT_REF_FORCE_V  0x3
#define SENS_AMP_SHORT_REF_FORCE_S  15
#define SENS_AMP_SHORT_REF_FORCE_FSM 0 // Use FSM to control power down
#define SENS_AMP_SHORT_REF_FORCE_PD  2 // Force power down
#define SENS_AMP_SHORT_REF_FORCE_PU  3 // Force power up
/* SENS_AMP_RST_FB_FORCE : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: */
#define SENS_AMP_RST_FB_FORCE  0x00000003
#define SENS_AMP_RST_FB_FORCE_M  ((SENS_AMP_RST_FB_FORCE_V)<<(SENS_AMP_RST_FB_FORCE_S))
#define SENS_AMP_RST_FB_FORCE_V  0x3
#define SENS_AMP_RST_FB_FORCE_S  13
#define SENS_AMP_RST_FB_FORCE_FSM 0 // Use FSM to control power down
#define SENS_AMP_RST_FB_FORCE_PD  2 // Force power down
#define SENS_AMP_RST_FB_FORCE_PU  3 // Force power up
/* SENS_SAR2_RSTB_FORCE : R/W ;bitpos:[12:11] ;default: 2'b0 ; */
/*description: */
#define SENS_SAR2_RSTB_FORCE  0x00000003
#define SENS_SAR2_RSTB_FORCE_M  ((SENS_SAR2_RSTB_FORCE_V)<<(SENS_SAR2_RSTB_FORCE_S))
#define SENS_SAR2_RSTB_FORCE_V  0x3
#define SENS_SAR2_RSTB_FORCE_S  11
#define SENS_SAR2_RSTB_FORCE_FSM 0 // Use FSM to control power down
#define SENS_SAR2_RSTB_FORCE_PD  2 // Force power down
#define SENS_SAR2_RSTB_FORCE_PU  3 // Force power up
/* SENS_SAR_RSTB_FSM_IDLE : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SENS_SAR_RSTB_FSM_IDLE  (BIT(10))
#define SENS_SAR_RSTB_FSM_IDLE_M  (BIT(10))
#define SENS_SAR_RSTB_FSM_IDLE_V  0x1
#define SENS_SAR_RSTB_FSM_IDLE_S  10
/* SENS_XPD_SAR_FSM_IDLE : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define SENS_XPD_SAR_FSM_IDLE  (BIT(9))
#define SENS_XPD_SAR_FSM_IDLE_M  (BIT(9))
#define SENS_XPD_SAR_FSM_IDLE_V  0x1
#define SENS_XPD_SAR_FSM_IDLE_S  9
/* SENS_AMP_SHORT_REF_GND_FSM_IDLE : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SENS_AMP_SHORT_REF_GND_FSM_IDLE  (BIT(8))
#define SENS_AMP_SHORT_REF_GND_FSM_IDLE_M  (BIT(8))
#define SENS_AMP_SHORT_REF_GND_FSM_IDLE_V  0x1
#define SENS_AMP_SHORT_REF_GND_FSM_IDLE_S  8
/* SENS_AMP_SHORT_REF_FSM_IDLE : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SENS_AMP_SHORT_REF_FSM_IDLE  (BIT(7))
#define SENS_AMP_SHORT_REF_FSM_IDLE_M  (BIT(7))
#define SENS_AMP_SHORT_REF_FSM_IDLE_V  0x1
#define SENS_AMP_SHORT_REF_FSM_IDLE_S  7
/* SENS_AMP_RST_FB_FSM_IDLE : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SENS_AMP_RST_FB_FSM_IDLE  (BIT(6))
#define SENS_AMP_RST_FB_FSM_IDLE_M  (BIT(6))
#define SENS_AMP_RST_FB_FSM_IDLE_V  0x1
#define SENS_AMP_RST_FB_FSM_IDLE_S  6
/* SENS_XPD_SAR_AMP_FSM_IDLE : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SENS_XPD_SAR_AMP_FSM_IDLE  (BIT(5))
#define SENS_XPD_SAR_AMP_FSM_IDLE_M  (BIT(5))
#define SENS_XPD_SAR_AMP_FSM_IDLE_V  0x1
#define SENS_XPD_SAR_AMP_FSM_IDLE_S  5
/* SENS_SAR1_DAC_XPD_FSM_IDLE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SENS_SAR1_DAC_XPD_FSM_IDLE  (BIT(4))
#define SENS_SAR1_DAC_XPD_FSM_IDLE_M  (BIT(4))
#define SENS_SAR1_DAC_XPD_FSM_IDLE_V  0x1
#define SENS_SAR1_DAC_XPD_FSM_IDLE_S  4
/* SENS_SAR1_DAC_XPD_FSM : R/W ;bitpos:[3:0] ;default: 4'b0011 ; */
/*description: */
#define SENS_SAR1_DAC_XPD_FSM  0x0000000F
#define SENS_SAR1_DAC_XPD_FSM_M  ((SENS_SAR1_DAC_XPD_FSM_V)<<(SENS_SAR1_DAC_XPD_FSM_S))
#define SENS_SAR1_DAC_XPD_FSM_V  0xF
#define SENS_SAR1_DAC_XPD_FSM_S  0

#define SENS_SAR_NOUSE_REG          (DR_REG_SENS_BASE + 0x00F8)
/* SENS_SAR_NOUSE : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SENS_SAR_NOUSE  0xFFFFFFFF
#define SENS_SAR_NOUSE_M  ((SENS_SAR_NOUSE_V)<<(SENS_SAR_NOUSE_S))
#define SENS_SAR_NOUSE_V  0xFFFFFFFF
#define SENS_SAR_NOUSE_S  0

#define SENS_SARDATE_REG          (DR_REG_SENS_BASE + 0x00FC)
/* SENS_SAR_DATE : R/W ;bitpos:[27:0] ;default: 28'h1605180 ; */
/*description: */
#define SENS_SAR_DATE  0x0FFFFFFF
#define SENS_SAR_DATE_M  ((SENS_SAR_DATE_V)<<(SENS_SAR_DATE_S))
#define SENS_SAR_DATE_V  0xFFFFFFF
#define SENS_SAR_DATE_S  0




#endif /*_SOC_SENS_REG_H_ */


