// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc.h"

#define EFUSE_RD_RS_ERR0_REG (DR_REG_EFUSE_BASE + 0x1c0)
/* EFUSE_RD_KEY4_FAIL : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable 1:
 Means that programming key$n failed and the number of error bytes is over 6.*/
#define EFUSE_RD_KEY4_FAIL (BIT(31))
#define EFUSE_RD_KEY4_FAIL_M (BIT(31))
#define EFUSE_RD_KEY4_FAIL_V 0x1
#define EFUSE_RD_KEY4_FAIL_S 31
/* EFUSE_RD_KEY4_ERR_NUM : RO ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_KEY4_ERR_NUM 0x00000007
#define EFUSE_RD_KEY4_ERR_NUM_M ((EFUSE_RD_KEY4_ERR_NUM_V) << (EFUSE_RD_KEY4_ERR_NUM_S))
#define EFUSE_RD_KEY4_ERR_NUM_V 0x7
#define EFUSE_RD_KEY4_ERR_NUM_S 28
/* EFUSE_RD_KEY3_FAIL : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable 1:
 Means that programming key$n failed and the number of error bytes is over 6.*/
#define EFUSE_RD_KEY3_FAIL (BIT(27))
#define EFUSE_RD_KEY3_FAIL_M (BIT(27))
#define EFUSE_RD_KEY3_FAIL_V 0x1
#define EFUSE_RD_KEY3_FAIL_S 27
/* EFUSE_RD_KEY3_ERR_NUM : RO ;bitpos:[26:24] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_KEY3_ERR_NUM 0x00000007
#define EFUSE_RD_KEY3_ERR_NUM_M ((EFUSE_RD_KEY3_ERR_NUM_V) << (EFUSE_RD_KEY3_ERR_NUM_S))
#define EFUSE_RD_KEY3_ERR_NUM_V 0x7
#define EFUSE_RD_KEY3_ERR_NUM_S 24
/* EFUSE_RD_KEY2_FAIL : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable 1:
 Means that programming key$n failed and the number of error bytes is over 6.*/
#define EFUSE_RD_KEY2_FAIL (BIT(23))
#define EFUSE_RD_KEY2_FAIL_M (BIT(23))
#define EFUSE_RD_KEY2_FAIL_V 0x1
#define EFUSE_RD_KEY2_FAIL_S 23
/* EFUSE_RD_KEY2_ERR_NUM : RO ;bitpos:[22:20] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_KEY2_ERR_NUM 0x00000007
#define EFUSE_RD_KEY2_ERR_NUM_M ((EFUSE_RD_KEY2_ERR_NUM_V) << (EFUSE_RD_KEY2_ERR_NUM_S))
#define EFUSE_RD_KEY2_ERR_NUM_V 0x7
#define EFUSE_RD_KEY2_ERR_NUM_S 20
/* EFUSE_RD_KEY1_FAIL : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable 1:
 Means that programming key$n failed and the number of error bytes is over 6.*/
#define EFUSE_RD_KEY1_FAIL (BIT(19))
#define EFUSE_RD_KEY1_FAIL_M (BIT(19))
#define EFUSE_RD_KEY1_FAIL_V 0x1
#define EFUSE_RD_KEY1_FAIL_S 19
/* EFUSE_RD_KEY1_ERR_NUM : RO ;bitpos:[18:16] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_KEY1_ERR_NUM 0x00000007
#define EFUSE_RD_KEY1_ERR_NUM_M ((EFUSE_RD_KEY1_ERR_NUM_V) << (EFUSE_RD_KEY1_ERR_NUM_S))
#define EFUSE_RD_KEY1_ERR_NUM_V 0x7
#define EFUSE_RD_KEY1_ERR_NUM_S 16
/* EFUSE_RD_KEY0_FAIL : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of key$n is reliable 1:
 Means that programming key$n failed and the number of error bytes is over 6.*/
#define EFUSE_RD_KEY0_FAIL (BIT(15))
#define EFUSE_RD_KEY0_FAIL_M (BIT(15))
#define EFUSE_RD_KEY0_FAIL_V 0x1
#define EFUSE_RD_KEY0_FAIL_S 15
/* EFUSE_RD_KEY0_ERR_NUM : RO ;bitpos:[14:12] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_KEY0_ERR_NUM 0x00000007
#define EFUSE_RD_KEY0_ERR_NUM_M ((EFUSE_RD_KEY0_ERR_NUM_V) << (EFUSE_RD_KEY0_ERR_NUM_S))
#define EFUSE_RD_KEY0_ERR_NUM_V 0x7
#define EFUSE_RD_KEY0_ERR_NUM_S 12
/* EFUSE_RD_USR_DATA_FAIL : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the user data is reliable 1: Means
 that programming user data failed and the number of error bytes is over 6.*/
#define EFUSE_RD_USR_DATA_FAIL (BIT(11))
#define EFUSE_RD_USR_DATA_FAIL_M (BIT(11))
#define EFUSE_RD_USR_DATA_FAIL_V 0x1
#define EFUSE_RD_USR_DATA_FAIL_S 11
/* EFUSE_RD_USR_DATA_ERR_NUM : RO ;bitpos:[10:8] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_USR_DATA_ERR_NUM 0x00000007
#define EFUSE_RD_USR_DATA_ERR_NUM_M ((EFUSE_RD_USR_DATA_ERR_NUM_V) << (EFUSE_RD_USR_DATA_ERR_NUM_S))
#define EFUSE_RD_USR_DATA_ERR_NUM_V 0x7
#define EFUSE_RD_USR_DATA_ERR_NUM_S 8
/* EFUSE_RD_SYS_PART1_FAIL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of system part1 is reliable
 1: Means that programming user data failed and the number of error bytes is over 6.*/
#define EFUSE_RD_SYS_PART1_FAIL (BIT(7))
#define EFUSE_RD_SYS_PART1_FAIL_M (BIT(7))
#define EFUSE_RD_SYS_PART1_FAIL_V 0x1
#define EFUSE_RD_SYS_PART1_FAIL_S 7
/* EFUSE_RD_SYS_PART1_NUM : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_SYS_PART1_NUM 0x00000007
#define EFUSE_RD_SYS_PART1_NUM_M ((EFUSE_RD_SYS_PART1_NUM_V) << (EFUSE_RD_SYS_PART1_NUM_S))
#define EFUSE_RD_SYS_PART1_NUM_V 0x7
#define EFUSE_RD_SYS_PART1_NUM_S 4
/* EFUSE_RD_MAC_SPI_8M_FAIL : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of MAC_SPI_8M is reliable
 1: Means that programming user data failed and the number of error bytes is over 6.*/
#define EFUSE_RD_MAC_SPI_8M_FAIL (BIT(3))
#define EFUSE_RD_MAC_SPI_8M_FAIL_M (BIT(3))
#define EFUSE_RD_MAC_SPI_8M_FAIL_V 0x1
#define EFUSE_RD_MAC_SPI_8M_FAIL_S 3
/* EFUSE_RD_MAC_SPI_8M_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_MAC_SPI_8M_ERR_NUM 0x00000007
#define EFUSE_RD_MAC_SPI_8M_ERR_NUM_M ((EFUSE_RD_MAC_SPI_8M_ERR_NUM_V) << (EFUSE_RD_MAC_SPI_8M_ERR_NUM_S))
#define EFUSE_RD_MAC_SPI_8M_ERR_NUM_V 0x7
#define EFUSE_RD_MAC_SPI_8M_ERR_NUM_S 0

#define EFUSE_RD_RS_ERR1_REG (DR_REG_EFUSE_BASE + 0x1c4)
/* EFUSE_RD_SYS_PART2_FAIL : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of system part2 is reliable
 1: Means that programming user data failed and the number of error bytes is over 6.*/
#define EFUSE_RD_SYS_PART2_FAIL (BIT(7))
#define EFUSE_RD_SYS_PART2_FAIL_M (BIT(7))
#define EFUSE_RD_SYS_PART2_FAIL_V 0x1
#define EFUSE_RD_SYS_PART2_FAIL_S 7
/* EFUSE_RD_SYS_PART2_NUM : RO ;bitpos:[6:4] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_SYS_PART2_NUM 0x00000007
#define EFUSE_RD_SYS_PART2_NUM_M ((EFUSE_RD_SYS_PART2_NUM_V) << (EFUSE_RD_SYS_PART2_NUM_S))
#define EFUSE_RD_SYS_PART2_NUM_V 0x7
#define EFUSE_RD_SYS_PART2_NUM_S 4
/* EFUSE_RD_KEY5_FAIL : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: 0: Means no failure and that the data of KEY5 is reliable 1:
 Means that programming user data failed and the number of error bytes is over 6.*/
#define EFUSE_RD_KEY5_FAIL (BIT(3))
#define EFUSE_RD_KEY5_FAIL_M (BIT(3))
#define EFUSE_RD_KEY5_FAIL_V 0x1
#define EFUSE_RD_KEY5_FAIL_S 3
/* EFUSE_RD_KEY5_ERR_NUM : RO ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: The value of this signal means the number of error bytes.*/
#define EFUSE_RD_KEY5_ERR_NUM 0x00000007
#define EFUSE_RD_KEY5_ERR_NUM_M ((EFUSE_RD_KEY5_ERR_NUM_V) << (EFUSE_RD_KEY5_ERR_NUM_S))
#define EFUSE_RD_KEY5_ERR_NUM_V 0x7
#define EFUSE_RD_KEY5_ERR_NUM_S 0

#define EFUSE_CLK_REG (DR_REG_EFUSE_BASE + 0x1c8)
/* EFUSE_CLK_EN : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit and force to enable clock signal of eFuse memory.*/
#define EFUSE_CLK_EN (BIT(16))
#define EFUSE_CLK_EN_M (BIT(16))
#define EFUSE_CLK_EN_V 0x1
#define EFUSE_CLK_EN_S 16
/* EFUSE_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to force eFuse SRAM into working mode.*/
#define EFUSE_MEM_FORCE_PU (BIT(2))
#define EFUSE_MEM_FORCE_PU_M (BIT(2))
#define EFUSE_MEM_FORCE_PU_V 0x1
#define EFUSE_MEM_FORCE_PU_S 2
/* EFUSE_MEM_CLK_FORCE_ON : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: Set this bit and force to activate clock signal of eFuse SRAM.*/
#define EFUSE_MEM_CLK_FORCE_ON (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_M (BIT(1))
#define EFUSE_MEM_CLK_FORCE_ON_V 0x1
#define EFUSE_MEM_CLK_FORCE_ON_S 1
/* EFUSE_MEM_FORCE_PD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to force eFuse SRAM into power-saving mode.*/
#define EFUSE_MEM_FORCE_PD (BIT(0))
#define EFUSE_MEM_FORCE_PD_M (BIT(0))
#define EFUSE_MEM_FORCE_PD_V 0x1
#define EFUSE_MEM_FORCE_PD_S 0

#define EFUSE_CONF_REG (DR_REG_EFUSE_BASE + 0x1cc)
/* EFUSE_OP_CODE : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: 0x5A5A: Operate programming command 0x5AA5: Operate read command.*/
#define EFUSE_OP_CODE 0x0000FFFF
#define EFUSE_OP_CODE_M ((EFUSE_OP_CODE_V) << (EFUSE_OP_CODE_S))
#define EFUSE_OP_CODE_V 0xFFFF
#define EFUSE_OP_CODE_S 0

#define EFUSE_STATUS_REG (DR_REG_EFUSE_BASE + 0x1d0)
/* EFUSE_REPEAT_ERR_CNT : RO ;bitpos:[17:10] ;default: 8'h0 ; */
/*description: Indicates the number of error bits during programming BLOCK0.*/
#define EFUSE_REPEAT_ERR_CNT 0x000000FF
#define EFUSE_REPEAT_ERR_CNT_M ((EFUSE_REPEAT_ERR_CNT_V) << (EFUSE_REPEAT_ERR_CNT_S))
#define EFUSE_REPEAT_ERR_CNT_V 0xFF
#define EFUSE_REPEAT_ERR_CNT_S 10
/* EFUSE_OTP_VDDQ_IS_SW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The value of OTP_VDDQ_IS_SW.*/
#define EFUSE_OTP_VDDQ_IS_SW (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_M (BIT(9))
#define EFUSE_OTP_VDDQ_IS_SW_V 0x1
#define EFUSE_OTP_VDDQ_IS_SW_S 9
/* EFUSE_OTP_PGENB_SW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The value of OTP_PGENB_SW.*/
#define EFUSE_OTP_PGENB_SW (BIT(8))
#define EFUSE_OTP_PGENB_SW_M (BIT(8))
#define EFUSE_OTP_PGENB_SW_V 0x1
#define EFUSE_OTP_PGENB_SW_S 8
/* EFUSE_OTP_CSB_SW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The value of OTP_CSB_SW.*/
#define EFUSE_OTP_CSB_SW (BIT(7))
#define EFUSE_OTP_CSB_SW_M (BIT(7))
#define EFUSE_OTP_CSB_SW_V 0x1
#define EFUSE_OTP_CSB_SW_S 7
/* EFUSE_OTP_STROBE_SW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The value of OTP_STROBE_SW.*/
#define EFUSE_OTP_STROBE_SW (BIT(6))
#define EFUSE_OTP_STROBE_SW_M (BIT(6))
#define EFUSE_OTP_STROBE_SW_V 0x1
#define EFUSE_OTP_STROBE_SW_S 6
/* EFUSE_OTP_VDDQ_C_SYNC2 : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The value of OTP_VDDQ_C_SYNC2.*/
#define EFUSE_OTP_VDDQ_C_SYNC2 (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_M (BIT(5))
#define EFUSE_OTP_VDDQ_C_SYNC2_V 0x1
#define EFUSE_OTP_VDDQ_C_SYNC2_S 5
/* EFUSE_OTP_LOAD_SW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The value of OTP_LOAD_SW.*/
#define EFUSE_OTP_LOAD_SW (BIT(4))
#define EFUSE_OTP_LOAD_SW_M (BIT(4))
#define EFUSE_OTP_LOAD_SW_V 0x1
#define EFUSE_OTP_LOAD_SW_S 4
/* EFUSE_STATE : RO ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: Indicates the state of the eFuse state machine.*/
#define EFUSE_STATE 0x0000000F
#define EFUSE_STATE_M ((EFUSE_STATE_V) << (EFUSE_STATE_S))
#define EFUSE_STATE_V 0xF
#define EFUSE_STATE_S 0

#define EFUSE_CMD_REG (DR_REG_EFUSE_BASE + 0x1d4)
/* EFUSE_BLK_NUM : R/W ;bitpos:[5:2] ;default: 4'h0 ; */
/*description: The serial number of the block to be programmed. Value 0-10 corresponds
 to block number 0-10  respectively.*/
#define EFUSE_BLK_NUM 0x0000000F
#define EFUSE_BLK_NUM_M ((EFUSE_BLK_NUM_V) << (EFUSE_BLK_NUM_S))
#define EFUSE_BLK_NUM_V 0xF
#define EFUSE_BLK_NUM_S 2
/* EFUSE_PGM_CMD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to send programming command.*/
#define EFUSE_PGM_CMD (BIT(1))
#define EFUSE_PGM_CMD_M (BIT(1))
#define EFUSE_PGM_CMD_V 0x1
#define EFUSE_PGM_CMD_S 1
/* EFUSE_READ_CMD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to send read command.*/
#define EFUSE_READ_CMD (BIT(0))
#define EFUSE_READ_CMD_M (BIT(0))
#define EFUSE_READ_CMD_V 0x1
#define EFUSE_READ_CMD_S 0

#define EFUSE_INT_RAW_REG (DR_REG_EFUSE_BASE + 0x1d8)
/* EFUSE_PGM_DONE_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw bit signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_RAW (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_M (BIT(1))
#define EFUSE_PGM_DONE_INT_RAW_V 0x1
#define EFUSE_PGM_DONE_INT_RAW_S 1
/* EFUSE_READ_DONE_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw bit signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_RAW (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_M (BIT(0))
#define EFUSE_READ_DONE_INT_RAW_V 0x1
#define EFUSE_READ_DONE_INT_RAW_S 0

#define EFUSE_INT_ST_REG (DR_REG_EFUSE_BASE + 0x1dc)
/* EFUSE_PGM_DONE_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The status signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_ST (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_M (BIT(1))
#define EFUSE_PGM_DONE_INT_ST_V 0x1
#define EFUSE_PGM_DONE_INT_ST_S 1
/* EFUSE_READ_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The status signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_ST (BIT(0))
#define EFUSE_READ_DONE_INT_ST_M (BIT(0))
#define EFUSE_READ_DONE_INT_ST_V 0x1
#define EFUSE_READ_DONE_INT_ST_S 0

#define EFUSE_INT_ENA_REG (DR_REG_EFUSE_BASE + 0x1e0)
/* EFUSE_PGM_DONE_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The enable signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_ENA (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_M (BIT(1))
#define EFUSE_PGM_DONE_INT_ENA_V 0x1
#define EFUSE_PGM_DONE_INT_ENA_S 1
/* EFUSE_READ_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The enable signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_ENA (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_M (BIT(0))
#define EFUSE_READ_DONE_INT_ENA_V 0x1
#define EFUSE_READ_DONE_INT_ENA_S 0

#define EFUSE_INT_CLR_REG (DR_REG_EFUSE_BASE + 0x1e4)
/* EFUSE_PGM_DONE_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The clear signal for pgm_done interrupt.*/
#define EFUSE_PGM_DONE_INT_CLR (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_M (BIT(1))
#define EFUSE_PGM_DONE_INT_CLR_V 0x1
#define EFUSE_PGM_DONE_INT_CLR_S 1
/* EFUSE_READ_DONE_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The clear signal for read_done interrupt.*/
#define EFUSE_READ_DONE_INT_CLR (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_M (BIT(0))
#define EFUSE_READ_DONE_INT_CLR_V 0x1
#define EFUSE_READ_DONE_INT_CLR_S 0

#define EFUSE_DAC_CONF_REG (DR_REG_EFUSE_BASE + 0x1e8)
/* EFUSE_OE_CLR : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: Reduces the power supply of the programming voltage.*/
#define EFUSE_OE_CLR (BIT(17))
#define EFUSE_OE_CLR_M (BIT(17))
#define EFUSE_OE_CLR_V 0x1
#define EFUSE_OE_CLR_S 17
/* EFUSE_DAC_NUM : R/W ;bitpos:[16:9] ;default: 8'd255 ; */
/*description: Controls the rising period of the programming voltage.*/
#define EFUSE_DAC_NUM 0x000000FF
#define EFUSE_DAC_NUM_M ((EFUSE_DAC_NUM_V) << (EFUSE_DAC_NUM_S))
#define EFUSE_DAC_NUM_V 0xFF
#define EFUSE_DAC_NUM_S 9
/* EFUSE_DAC_CLK_PAD_SEL : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Don't care.*/
#define EFUSE_DAC_CLK_PAD_SEL (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_M (BIT(8))
#define EFUSE_DAC_CLK_PAD_SEL_V 0x1
#define EFUSE_DAC_CLK_PAD_SEL_S 8
/* EFUSE_DAC_CLK_DIV : R/W ;bitpos:[7:0] ;default: 8'h28 ; */
/*description: Controls the division factor of the rising clock of the programming voltage.*/
#define EFUSE_DAC_CLK_DIV 0x000000FF
#define EFUSE_DAC_CLK_DIV_M ((EFUSE_DAC_CLK_DIV_V) << (EFUSE_DAC_CLK_DIV_S))
#define EFUSE_DAC_CLK_DIV_V 0xFF
#define EFUSE_DAC_CLK_DIV_S 0

#define EFUSE_RD_TIM_CONF_REG (DR_REG_EFUSE_BASE + 0x1ec)
/* EFUSE_READ_INIT_NUM : R/W ;bitpos:[31:24] ;default: 8'h12 ; */
/*description: Configures the initial read time of eFuse.*/
#define EFUSE_READ_INIT_NUM 0x000000FF
#define EFUSE_READ_INIT_NUM_M ((EFUSE_READ_INIT_NUM_V) << (EFUSE_READ_INIT_NUM_S))
#define EFUSE_READ_INIT_NUM_V 0xFF
#define EFUSE_READ_INIT_NUM_S 24
/* EFUSE_TSUR_A : R/W ;bitpos:[23:16] ;default: 8'h1 ; */
/*description: Configures the setup time of read operation.*/
#define EFUSE_TSUR_A 0x000000FF
#define EFUSE_TSUR_A_M ((EFUSE_TSUR_A_V) << (EFUSE_TSUR_A_S))
#define EFUSE_TSUR_A_V 0xFF
#define EFUSE_TSUR_A_S 16
/* EFUSE_TRD : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: Configures the length of pulse of read operation.*/
#define EFUSE_TRD 0x000000FF
#define EFUSE_TRD_M ((EFUSE_TRD_V) << (EFUSE_TRD_S))
#define EFUSE_TRD_V 0xFF
#define EFUSE_TRD_S 8
/* EFUSE_THR_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: Configures the hold time of read operation.*/
#define EFUSE_THR_A 0x000000FF
#define EFUSE_THR_A_M ((EFUSE_THR_A_V) << (EFUSE_THR_A_S))
#define EFUSE_THR_A_V 0xFF
#define EFUSE_THR_A_S 0

#define EFUSE_WR_TIM_CONF0_REG (DR_REG_EFUSE_BASE + 0x1F0)
/* EFUSE_TPGM : R/W ;bitpos:[31:16] ;default: 16'hc8 ; */
/*description: Configures the length of pulse during programming 1 to eFuse.*/
#define EFUSE_TPGM 0x0000FFFF
#define EFUSE_TPGM_M ((EFUSE_TPGM_V) << (EFUSE_TPGM_S))
#define EFUSE_TPGM_V 0xFFFF
#define EFUSE_TPGM_S 16
/* EFUSE_TPGM_INACTIVE : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: Configures the length of pulse during programming 0 to eFuse.*/
#define EFUSE_TPGM_INACTIVE 0x000000FF
#define EFUSE_TPGM_INACTIVE_M ((EFUSE_TPGM_INACTIVE_V) << (EFUSE_TPGM_INACTIVE_S))
#define EFUSE_TPGM_INACTIVE_V 0xFF
#define EFUSE_TPGM_INACTIVE_S 8
/* EFUSE_THP_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: Configures the hold time of programming operation.*/
#define EFUSE_THP_A 0x000000FF
#define EFUSE_THP_A_M ((EFUSE_THP_A_V) << (EFUSE_THP_A_S))
#define EFUSE_THP_A_V 0xFF
#define EFUSE_THP_A_S 0

#define EFUSE_WR_TIM_CONF1_REG (DR_REG_EFUSE_BASE + 0x1F4)
/* EFUSE_PWR_ON_NUM : R/W ;bitpos:[23:8] ;default: 16'h2880 ; */
/*description: Configures the power up time for VDDQ.*/
#define EFUSE_PWR_ON_NUM 0x0000FFFF
#define EFUSE_PWR_ON_NUM_M ((EFUSE_PWR_ON_NUM_V) << (EFUSE_PWR_ON_NUM_S))
#define EFUSE_PWR_ON_NUM_V 0xFFFF
#define EFUSE_PWR_ON_NUM_S 8
/* EFUSE_TSUP_A : R/W ;bitpos:[7:0] ;default: 8'h1 ; */
/*description: Configures the setup time of programming operation.*/
#define EFUSE_TSUP_A 0x000000FF
#define EFUSE_TSUP_A_M ((EFUSE_TSUP_A_V) << (EFUSE_TSUP_A_S))
#define EFUSE_TSUP_A_V 0xFF
#define EFUSE_TSUP_A_S 0

#define EFUSE_WR_TIM_CONF2_REG (DR_REG_EFUSE_BASE + 0x1F8)
/* EFUSE_PWR_OFF_NUM : R/W ;bitpos:[15:0] ;default: 16'h190 ; */
/*description: Configures the power outage time for VDDQ.*/
#define EFUSE_PWR_OFF_NUM 0x0000FFFF
#define EFUSE_PWR_OFF_NUM_M ((EFUSE_PWR_OFF_NUM_V) << (EFUSE_PWR_OFF_NUM_S))
#define EFUSE_PWR_OFF_NUM_V 0xFFFF
#define EFUSE_PWR_OFF_NUM_S 0

#define EFUSE_DATE_REG (DR_REG_EFUSE_BASE + 0x1FC)
/* EFUSE_DATE : R/W ;bitpos:[27:0] ;default: 28'h2003310 ; */
/*description: Stores eFuse version.*/
#define EFUSE_DATE 0x0FFFFFFF
#define EFUSE_DATE_M ((EFUSE_DATE_V) << (EFUSE_DATE_S))
#define EFUSE_DATE_V 0xFFFFFFF
#define EFUSE_DATE_S 0

#ifdef __cplusplus
}
#endif
