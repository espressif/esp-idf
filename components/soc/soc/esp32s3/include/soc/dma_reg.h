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

#define DMA_CONF0_CH0_REG (DR_REG_DMA_BASE + 0x000)
/* DMA_MEM_TRANS_EN_CH0 : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory
 to memory via DMA.*/
#define DMA_MEM_TRANS_EN_CH0 (BIT(10))
#define DMA_MEM_TRANS_EN_CH0_M (BIT(10))
#define DMA_MEM_TRANS_EN_CH0_V 0x1
#define DMA_MEM_TRANS_EN_CH0_S 10
/* DMA_IN_DATA_BURST_EN_CH0 : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 0 receiving data when accessing internal SRAM.*/
#define DMA_IN_DATA_BURST_EN_CH0 (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH0_M (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH0_V 0x1
#define DMA_IN_DATA_BURST_EN_CH0_S 9
/* DMA_OUT_DATA_BURST_EN_CH0 : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 0 transmitting data when accessing internal SRAM.*/
#define DMA_OUT_DATA_BURST_EN_CH0 (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH0_M (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH0_V 0x1
#define DMA_OUT_DATA_BURST_EN_CH0_S 8
/* DMA_INDSCR_BURST_EN_CH0 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 0 reading link descriptor when accessing internal SRAM.*/
#define DMA_INDSCR_BURST_EN_CH0 (BIT(7))
#define DMA_INDSCR_BURST_EN_CH0_M (BIT(7))
#define DMA_INDSCR_BURST_EN_CH0_V 0x1
#define DMA_INDSCR_BURST_EN_CH0_S 7
/* DMA_OUTDSCR_BURST_EN_CH0 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 0 reading link descriptor when accessing internal SRAM.*/
#define DMA_OUTDSCR_BURST_EN_CH0 (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH0_M (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH0_V 0x1
#define DMA_OUTDSCR_BURST_EN_CH0_S 6
/* DMA_OUT_EOF_MODE_CH0 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag
 for Tx channel 0 is generated when data need to transmit has been popped from FIFO in DMA*/
#define DMA_OUT_EOF_MODE_CH0 (BIT(5))
#define DMA_OUT_EOF_MODE_CH0_M (BIT(5))
#define DMA_OUT_EOF_MODE_CH0_V 0x1
#define DMA_OUT_EOF_MODE_CH0_S 5
/* DMA_OUT_AUTO_WRBACK_CH0 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the
 data in tx buffer has been transmitted.*/
#define DMA_OUT_AUTO_WRBACK_CH0 (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH0_M (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH0_V 0x1
#define DMA_OUT_AUTO_WRBACK_CH0_S 4
/* DMA_OUT_LOOP_TEST_CH0 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_LOOP_TEST_CH0 (BIT(3))
#define DMA_OUT_LOOP_TEST_CH0_M (BIT(3))
#define DMA_OUT_LOOP_TEST_CH0_V 0x1
#define DMA_OUT_LOOP_TEST_CH0_S 3
/* DMA_IN_LOOP_TEST_CH0 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_LOOP_TEST_CH0 (BIT(2))
#define DMA_IN_LOOP_TEST_CH0_M (BIT(2))
#define DMA_IN_LOOP_TEST_CH0_V 0x1
#define DMA_IN_LOOP_TEST_CH0_S 2
/* DMA_OUT_RST_CH0 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 0 Tx FSM and Tx FIFO pointer.*/
#define DMA_OUT_RST_CH0 (BIT(1))
#define DMA_OUT_RST_CH0_M (BIT(1))
#define DMA_OUT_RST_CH0_V 0x1
#define DMA_OUT_RST_CH0_S 1
/* DMA_IN_RST_CH0 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 0 Rx FSM and Rx FIFO pointer.*/
#define DMA_IN_RST_CH0 (BIT(0))
#define DMA_IN_RST_CH0_M (BIT(0))
#define DMA_IN_RST_CH0_V 0x1
#define DMA_IN_RST_CH0_S 0

#define DMA_CONF0_CH1_REG (DR_REG_DMA_BASE + 0x004)
/* DMA_MEM_TRANS_EN_CH1 : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory
 to memory via DMA.*/
#define DMA_MEM_TRANS_EN_CH1 (BIT(10))
#define DMA_MEM_TRANS_EN_CH1_M (BIT(10))
#define DMA_MEM_TRANS_EN_CH1_V 0x1
#define DMA_MEM_TRANS_EN_CH1_S 10
/* DMA_IN_DATA_BURST_EN_CH1 : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 1 receiving data when accessing internal SRAM.*/
#define DMA_IN_DATA_BURST_EN_CH1 (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH1_M (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH1_V 0x1
#define DMA_IN_DATA_BURST_EN_CH1_S 9
/* DMA_OUT_DATA_BURST_EN_CH1 : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 1 transmitting data when accessing internal SRAM.*/
#define DMA_OUT_DATA_BURST_EN_CH1 (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH1_M (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH1_V 0x1
#define DMA_OUT_DATA_BURST_EN_CH1_S 8
/* DMA_INDSCR_BURST_EN_CH1 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 1 reading link descriptor when accessing internal SRAM.*/
#define DMA_INDSCR_BURST_EN_CH1 (BIT(7))
#define DMA_INDSCR_BURST_EN_CH1_M (BIT(7))
#define DMA_INDSCR_BURST_EN_CH1_V 0x1
#define DMA_INDSCR_BURST_EN_CH1_S 7
/* DMA_OUTDSCR_BURST_EN_CH1 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 1 reading link descriptor when accessing internal SRAM.*/
#define DMA_OUTDSCR_BURST_EN_CH1 (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH1_M (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH1_V 0x1
#define DMA_OUTDSCR_BURST_EN_CH1_S 6
/* DMA_OUT_EOF_MODE_CH1 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag
 for Tx channel 1 is generated when data need to transmit has been popped from FIFO in DMA*/
#define DMA_OUT_EOF_MODE_CH1 (BIT(5))
#define DMA_OUT_EOF_MODE_CH1_M (BIT(5))
#define DMA_OUT_EOF_MODE_CH1_V 0x1
#define DMA_OUT_EOF_MODE_CH1_S 5
/* DMA_OUT_AUTO_WRBACK_CH1 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the
 data in tx buffer has been transmitted.*/
#define DMA_OUT_AUTO_WRBACK_CH1 (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH1_M (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH1_V 0x1
#define DMA_OUT_AUTO_WRBACK_CH1_S 4
/* DMA_OUT_LOOP_TEST_CH1 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_LOOP_TEST_CH1 (BIT(3))
#define DMA_OUT_LOOP_TEST_CH1_M (BIT(3))
#define DMA_OUT_LOOP_TEST_CH1_V 0x1
#define DMA_OUT_LOOP_TEST_CH1_S 3
/* DMA_IN_LOOP_TEST_CH1 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_LOOP_TEST_CH1 (BIT(2))
#define DMA_IN_LOOP_TEST_CH1_M (BIT(2))
#define DMA_IN_LOOP_TEST_CH1_V 0x1
#define DMA_IN_LOOP_TEST_CH1_S 2
/* DMA_OUT_RST_CH1 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 1 Tx FSM and Tx FIFO pointer.*/
#define DMA_OUT_RST_CH1 (BIT(1))
#define DMA_OUT_RST_CH1_M (BIT(1))
#define DMA_OUT_RST_CH1_V 0x1
#define DMA_OUT_RST_CH1_S 1
/* DMA_IN_RST_CH1 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 1 Rx FSM and Rx FIFO pointer.*/
#define DMA_IN_RST_CH1 (BIT(0))
#define DMA_IN_RST_CH1_M (BIT(0))
#define DMA_IN_RST_CH1_V 0x1
#define DMA_IN_RST_CH1_S 0

#define DMA_CONF0_CH2_REG (DR_REG_DMA_BASE + 0x008)
/* DMA_MEM_TRANS_EN_CH2 : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory
 to memory via DMA.*/
#define DMA_MEM_TRANS_EN_CH2 (BIT(10))
#define DMA_MEM_TRANS_EN_CH2_M (BIT(10))
#define DMA_MEM_TRANS_EN_CH2_V 0x1
#define DMA_MEM_TRANS_EN_CH2_S 10
/* DMA_IN_DATA_BURST_EN_CH2 : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 2 receiving data when accessing internal SRAM.*/
#define DMA_IN_DATA_BURST_EN_CH2 (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH2_M (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH2_V 0x1
#define DMA_IN_DATA_BURST_EN_CH2_S 9
/* DMA_OUT_DATA_BURST_EN_CH2 : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 2 transmitting data when accessing internal SRAM.*/
#define DMA_OUT_DATA_BURST_EN_CH2 (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH2_M (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH2_V 0x1
#define DMA_OUT_DATA_BURST_EN_CH2_S 8
/* DMA_INDSCR_BURST_EN_CH2 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 2 reading link descriptor when accessing internal SRAM.*/
#define DMA_INDSCR_BURST_EN_CH2 (BIT(7))
#define DMA_INDSCR_BURST_EN_CH2_M (BIT(7))
#define DMA_INDSCR_BURST_EN_CH2_V 0x1
#define DMA_INDSCR_BURST_EN_CH2_S 7
/* DMA_OUTDSCR_BURST_EN_CH2 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 2 reading link descriptor when accessing internal SRAM.*/
#define DMA_OUTDSCR_BURST_EN_CH2 (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH2_M (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH2_V 0x1
#define DMA_OUTDSCR_BURST_EN_CH2_S 6
/* DMA_OUT_EOF_MODE_CH2 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag
 for Tx channel 2 is generated when data need to transmit has been popped from FIFO in DMA*/
#define DMA_OUT_EOF_MODE_CH2 (BIT(5))
#define DMA_OUT_EOF_MODE_CH2_M (BIT(5))
#define DMA_OUT_EOF_MODE_CH2_V 0x1
#define DMA_OUT_EOF_MODE_CH2_S 5
/* DMA_OUT_AUTO_WRBACK_CH2 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the
 data in tx buffer has been transmitted.*/
#define DMA_OUT_AUTO_WRBACK_CH2 (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH2_M (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH2_V 0x1
#define DMA_OUT_AUTO_WRBACK_CH2_S 4
/* DMA_OUT_LOOP_TEST_CH2 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_LOOP_TEST_CH2 (BIT(3))
#define DMA_OUT_LOOP_TEST_CH2_M (BIT(3))
#define DMA_OUT_LOOP_TEST_CH2_V 0x1
#define DMA_OUT_LOOP_TEST_CH2_S 3
/* DMA_IN_LOOP_TEST_CH2 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_LOOP_TEST_CH2 (BIT(2))
#define DMA_IN_LOOP_TEST_CH2_M (BIT(2))
#define DMA_IN_LOOP_TEST_CH2_V 0x1
#define DMA_IN_LOOP_TEST_CH2_S 2
/* DMA_OUT_RST_CH2 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 2 Tx FSM and Tx FIFO pointer.*/
#define DMA_OUT_RST_CH2 (BIT(1))
#define DMA_OUT_RST_CH2_M (BIT(1))
#define DMA_OUT_RST_CH2_V 0x1
#define DMA_OUT_RST_CH2_S 1
/* DMA_IN_RST_CH2 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 2 Rx FSM and Rx FIFO pointer.*/
#define DMA_IN_RST_CH2 (BIT(0))
#define DMA_IN_RST_CH2_M (BIT(0))
#define DMA_IN_RST_CH2_V 0x1
#define DMA_IN_RST_CH2_S 0

#define DMA_CONF0_CH3_REG (DR_REG_DMA_BASE + 0x00C)
/* DMA_MEM_TRANS_EN_CH3 : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory
 to memory via DMA.*/
#define DMA_MEM_TRANS_EN_CH3 (BIT(10))
#define DMA_MEM_TRANS_EN_CH3_M (BIT(10))
#define DMA_MEM_TRANS_EN_CH3_V 0x1
#define DMA_MEM_TRANS_EN_CH3_S 10
/* DMA_IN_DATA_BURST_EN_CH3 : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 3 receiving data when accessing internal SRAM.*/
#define DMA_IN_DATA_BURST_EN_CH3 (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH3_M (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH3_V 0x1
#define DMA_IN_DATA_BURST_EN_CH3_S 9
/* DMA_OUT_DATA_BURST_EN_CH3 : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 3 transmitting data when accessing internal SRAM.*/
#define DMA_OUT_DATA_BURST_EN_CH3 (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH3_M (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH3_V 0x1
#define DMA_OUT_DATA_BURST_EN_CH3_S 8
/* DMA_INDSCR_BURST_EN_CH3 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 3 reading link descriptor when accessing internal SRAM.*/
#define DMA_INDSCR_BURST_EN_CH3 (BIT(7))
#define DMA_INDSCR_BURST_EN_CH3_M (BIT(7))
#define DMA_INDSCR_BURST_EN_CH3_V 0x1
#define DMA_INDSCR_BURST_EN_CH3_S 7
/* DMA_OUTDSCR_BURST_EN_CH3 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 3 reading link descriptor when accessing internal SRAM.*/
#define DMA_OUTDSCR_BURST_EN_CH3 (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH3_M (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH3_V 0x1
#define DMA_OUTDSCR_BURST_EN_CH3_S 6
/* DMA_OUT_EOF_MODE_CH3 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag
 for Tx channel 3 is generated when data need to transmit has been popped from FIFO in DMA*/
#define DMA_OUT_EOF_MODE_CH3 (BIT(5))
#define DMA_OUT_EOF_MODE_CH3_M (BIT(5))
#define DMA_OUT_EOF_MODE_CH3_V 0x1
#define DMA_OUT_EOF_MODE_CH3_S 5
/* DMA_OUT_AUTO_WRBACK_CH3 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the
 data in tx buffer has been transmitted.*/
#define DMA_OUT_AUTO_WRBACK_CH3 (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH3_M (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH3_V 0x1
#define DMA_OUT_AUTO_WRBACK_CH3_S 4
/* DMA_OUT_LOOP_TEST_CH3 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_LOOP_TEST_CH3 (BIT(3))
#define DMA_OUT_LOOP_TEST_CH3_M (BIT(3))
#define DMA_OUT_LOOP_TEST_CH3_V 0x1
#define DMA_OUT_LOOP_TEST_CH3_S 3
/* DMA_IN_LOOP_TEST_CH3 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_LOOP_TEST_CH3 (BIT(2))
#define DMA_IN_LOOP_TEST_CH3_M (BIT(2))
#define DMA_IN_LOOP_TEST_CH3_V 0x1
#define DMA_IN_LOOP_TEST_CH3_S 2
/* DMA_OUT_RST_CH3 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 3 Tx FSM and Tx FIFO pointer.*/
#define DMA_OUT_RST_CH3 (BIT(1))
#define DMA_OUT_RST_CH3_M (BIT(1))
#define DMA_OUT_RST_CH3_V 0x1
#define DMA_OUT_RST_CH3_S 1
/* DMA_IN_RST_CH3 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 3 Rx FSM and Rx FIFO pointer.*/
#define DMA_IN_RST_CH3 (BIT(0))
#define DMA_IN_RST_CH3_M (BIT(0))
#define DMA_IN_RST_CH3_V 0x1
#define DMA_IN_RST_CH3_S 0

#define DMA_CONF0_CH4_REG (DR_REG_DMA_BASE + 0x010)
/* DMA_MEM_TRANS_EN_CH4 : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit 1 to enable automatic transmitting data from memory
 to memory via DMA.*/
#define DMA_MEM_TRANS_EN_CH4 (BIT(10))
#define DMA_MEM_TRANS_EN_CH4_M (BIT(10))
#define DMA_MEM_TRANS_EN_CH4_V 0x1
#define DMA_MEM_TRANS_EN_CH4_S 10
/* DMA_IN_DATA_BURST_EN_CH4 : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 4 receiving data when accessing internal SRAM.*/
#define DMA_IN_DATA_BURST_EN_CH4 (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH4_M (BIT(9))
#define DMA_IN_DATA_BURST_EN_CH4_V 0x1
#define DMA_IN_DATA_BURST_EN_CH4_S 9
/* DMA_OUT_DATA_BURST_EN_CH4 : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 4 transmitting data when accessing internal SRAM.*/
#define DMA_OUT_DATA_BURST_EN_CH4 (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH4_M (BIT(8))
#define DMA_OUT_DATA_BURST_EN_CH4_V 0x1
#define DMA_OUT_DATA_BURST_EN_CH4_S 8
/* DMA_INDSCR_BURST_EN_CH4 : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Rx channel
 4 reading link descriptor when accessing internal SRAM.*/
#define DMA_INDSCR_BURST_EN_CH4 (BIT(7))
#define DMA_INDSCR_BURST_EN_CH4_M (BIT(7))
#define DMA_INDSCR_BURST_EN_CH4_V 0x1
#define DMA_INDSCR_BURST_EN_CH4_S 7
/* DMA_OUTDSCR_BURST_EN_CH4 : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to 1 to enable INCR burst transfer for Tx channel
 4 reading link descriptor when accessing internal SRAM.*/
#define DMA_OUTDSCR_BURST_EN_CH4 (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH4_M (BIT(6))
#define DMA_OUTDSCR_BURST_EN_CH4_V 0x1
#define DMA_OUTDSCR_BURST_EN_CH4_S 6
/* DMA_OUT_EOF_MODE_CH4 : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: EOF flag generation mode when transmitting data. 1: EOF flag
 for Tx channel 4 is generated when data need to transmit has been popped from FIFO in DMA*/
#define DMA_OUT_EOF_MODE_CH4 (BIT(5))
#define DMA_OUT_EOF_MODE_CH4_M (BIT(5))
#define DMA_OUT_EOF_MODE_CH4_V 0x1
#define DMA_OUT_EOF_MODE_CH4_S 5
/* DMA_OUT_AUTO_WRBACK_CH4 : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to enable automatic outlink-writeback when all the
 data in tx buffer has been transmitted.*/
#define DMA_OUT_AUTO_WRBACK_CH4 (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH4_M (BIT(4))
#define DMA_OUT_AUTO_WRBACK_CH4_V 0x1
#define DMA_OUT_AUTO_WRBACK_CH4_S 4
/* DMA_OUT_LOOP_TEST_CH4 : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_LOOP_TEST_CH4 (BIT(3))
#define DMA_OUT_LOOP_TEST_CH4_M (BIT(3))
#define DMA_OUT_LOOP_TEST_CH4_V 0x1
#define DMA_OUT_LOOP_TEST_CH4_S 3
/* DMA_IN_LOOP_TEST_CH4 : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_LOOP_TEST_CH4 (BIT(2))
#define DMA_IN_LOOP_TEST_CH4_M (BIT(2))
#define DMA_IN_LOOP_TEST_CH4_V 0x1
#define DMA_IN_LOOP_TEST_CH4_S 2
/* DMA_OUT_RST_CH4 : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: This bit is used to reset DMA channel 4 Tx FSM and Tx FIFO pointer.*/
#define DMA_OUT_RST_CH4 (BIT(1))
#define DMA_OUT_RST_CH4_M (BIT(1))
#define DMA_OUT_RST_CH4_V 0x1
#define DMA_OUT_RST_CH4_S 1
/* DMA_IN_RST_CH4 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit is used to reset DMA channel 4 Rx FSM and Rx FIFO pointer.*/
#define DMA_IN_RST_CH4 (BIT(0))
#define DMA_IN_RST_CH4_M (BIT(0))
#define DMA_IN_RST_CH4_V 0x1
#define DMA_IN_RST_CH4_S 0

#define DMA_CONF1_CH0_REG (DR_REG_DMA_BASE + 0x014)
/* DMA_OUT_EXT_MEM_BK_SIZE_CH0 : R/W ;bitpos:[16:15] ;default: 2'b0 ; */
/*description: Block size of Tx channel 0 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_OUT_EXT_MEM_BK_SIZE_CH0 0x00000003
#define DMA_OUT_EXT_MEM_BK_SIZE_CH0_M ((DMA_OUT_EXT_MEM_BK_SIZE_CH0_V) << (DMA_OUT_EXT_MEM_BK_SIZE_CH0_S))
#define DMA_OUT_EXT_MEM_BK_SIZE_CH0_V 0x3
#define DMA_OUT_EXT_MEM_BK_SIZE_CH0_S 15
/* DMA_IN_EXT_MEM_BK_SIZE_CH0 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 0 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_IN_EXT_MEM_BK_SIZE_CH0 0x00000003
#define DMA_IN_EXT_MEM_BK_SIZE_CH0_M ((DMA_IN_EXT_MEM_BK_SIZE_CH0_V) << (DMA_IN_EXT_MEM_BK_SIZE_CH0_S))
#define DMA_IN_EXT_MEM_BK_SIZE_CH0_V 0x3
#define DMA_IN_EXT_MEM_BK_SIZE_CH0_S 13
/* DMA_CHECK_OWNER_CH0 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor.*/
#define DMA_CHECK_OWNER_CH0 (BIT(12))
#define DMA_CHECK_OWNER_CH0_M (BIT(12))
#define DMA_CHECK_OWNER_CH0_V 0x1
#define DMA_CHECK_OWNER_CH0_S 12
/* DMA_INFIFO_FULL_THRS_CH0 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt
 when Rx channel 0 received byte number in Rx FIFO is up to the value of the register.*/
#define DMA_INFIFO_FULL_THRS_CH0 0x00000FFF
#define DMA_INFIFO_FULL_THRS_CH0_M ((DMA_INFIFO_FULL_THRS_CH0_V) << (DMA_INFIFO_FULL_THRS_CH0_S))
#define DMA_INFIFO_FULL_THRS_CH0_V 0xFFF
#define DMA_INFIFO_FULL_THRS_CH0_S 0

#define DMA_CONF1_CH1_REG (DR_REG_DMA_BASE + 0x018)
/* DMA_OUT_EXT_MEM_BK_SIZE_CH1 : R/W ;bitpos:[16:15] ;default: 2'b0 ; */
/*description: Block size of Tx channel 1 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_OUT_EXT_MEM_BK_SIZE_CH1 0x00000003
#define DMA_OUT_EXT_MEM_BK_SIZE_CH1_M ((DMA_OUT_EXT_MEM_BK_SIZE_CH1_V) << (DMA_OUT_EXT_MEM_BK_SIZE_CH1_S))
#define DMA_OUT_EXT_MEM_BK_SIZE_CH1_V 0x3
#define DMA_OUT_EXT_MEM_BK_SIZE_CH1_S 15
/* DMA_IN_EXT_MEM_BK_SIZE_CH1 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 1 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_IN_EXT_MEM_BK_SIZE_CH1 0x00000003
#define DMA_IN_EXT_MEM_BK_SIZE_CH1_M ((DMA_IN_EXT_MEM_BK_SIZE_CH1_V) << (DMA_IN_EXT_MEM_BK_SIZE_CH1_S))
#define DMA_IN_EXT_MEM_BK_SIZE_CH1_V 0x3
#define DMA_IN_EXT_MEM_BK_SIZE_CH1_S 13
/* DMA_CHECK_OWNER_CH1 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor.*/
#define DMA_CHECK_OWNER_CH1 (BIT(12))
#define DMA_CHECK_OWNER_CH1_M (BIT(12))
#define DMA_CHECK_OWNER_CH1_V 0x1
#define DMA_CHECK_OWNER_CH1_S 12
/* DMA_INFIFO_FULL_THRS_CH1 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt
 when Rx channel 1 received byte number in Rx FIFO is up to the value of the register.*/
#define DMA_INFIFO_FULL_THRS_CH1 0x00000FFF
#define DMA_INFIFO_FULL_THRS_CH1_M ((DMA_INFIFO_FULL_THRS_CH1_V) << (DMA_INFIFO_FULL_THRS_CH1_S))
#define DMA_INFIFO_FULL_THRS_CH1_V 0xFFF
#define DMA_INFIFO_FULL_THRS_CH1_S 0

#define DMA_CONF1_CH2_REG (DR_REG_DMA_BASE + 0x01C)
/* DMA_OUT_EXT_MEM_BK_SIZE_CH2 : R/W ;bitpos:[16:15] ;default: 2'b0 ; */
/*description: Block size of Tx channel 2 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_OUT_EXT_MEM_BK_SIZE_CH2 0x00000003
#define DMA_OUT_EXT_MEM_BK_SIZE_CH2_M ((DMA_OUT_EXT_MEM_BK_SIZE_CH2_V) << (DMA_OUT_EXT_MEM_BK_SIZE_CH2_S))
#define DMA_OUT_EXT_MEM_BK_SIZE_CH2_V 0x3
#define DMA_OUT_EXT_MEM_BK_SIZE_CH2_S 15
/* DMA_IN_EXT_MEM_BK_SIZE_CH2 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 2 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_IN_EXT_MEM_BK_SIZE_CH2 0x00000003
#define DMA_IN_EXT_MEM_BK_SIZE_CH2_M ((DMA_IN_EXT_MEM_BK_SIZE_CH2_V) << (DMA_IN_EXT_MEM_BK_SIZE_CH2_S))
#define DMA_IN_EXT_MEM_BK_SIZE_CH2_V 0x3
#define DMA_IN_EXT_MEM_BK_SIZE_CH2_S 13
/* DMA_CHECK_OWNER_CH2 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor.*/
#define DMA_CHECK_OWNER_CH2 (BIT(12))
#define DMA_CHECK_OWNER_CH2_M (BIT(12))
#define DMA_CHECK_OWNER_CH2_V 0x1
#define DMA_CHECK_OWNER_CH2_S 12
/* DMA_INFIFO_FULL_THRS_CH2 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt
 when Rx channel 2 received byte number in Rx FIFO is up to the value of the register.*/
#define DMA_INFIFO_FULL_THRS_CH2 0x00000FFF
#define DMA_INFIFO_FULL_THRS_CH2_M ((DMA_INFIFO_FULL_THRS_CH2_V) << (DMA_INFIFO_FULL_THRS_CH2_S))
#define DMA_INFIFO_FULL_THRS_CH2_V 0xFFF
#define DMA_INFIFO_FULL_THRS_CH2_S 0

#define DMA_CONF1_CH3_REG (DR_REG_DMA_BASE + 0x020)
/* DMA_OUT_EXT_MEM_BK_SIZE_CH3 : R/W ;bitpos:[16:15] ;default: 2'b0 ; */
/*description: Block size of Tx channel 3 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_OUT_EXT_MEM_BK_SIZE_CH3 0x00000003
#define DMA_OUT_EXT_MEM_BK_SIZE_CH3_M ((DMA_OUT_EXT_MEM_BK_SIZE_CH3_V) << (DMA_OUT_EXT_MEM_BK_SIZE_CH3_S))
#define DMA_OUT_EXT_MEM_BK_SIZE_CH3_V 0x3
#define DMA_OUT_EXT_MEM_BK_SIZE_CH3_S 15
/* DMA_IN_EXT_MEM_BK_SIZE_CH3 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 3 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_IN_EXT_MEM_BK_SIZE_CH3 0x00000003
#define DMA_IN_EXT_MEM_BK_SIZE_CH3_M ((DMA_IN_EXT_MEM_BK_SIZE_CH3_V) << (DMA_IN_EXT_MEM_BK_SIZE_CH3_S))
#define DMA_IN_EXT_MEM_BK_SIZE_CH3_V 0x3
#define DMA_IN_EXT_MEM_BK_SIZE_CH3_S 13
/* DMA_CHECK_OWNER_CH3 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor.*/
#define DMA_CHECK_OWNER_CH3 (BIT(12))
#define DMA_CHECK_OWNER_CH3_M (BIT(12))
#define DMA_CHECK_OWNER_CH3_V 0x1
#define DMA_CHECK_OWNER_CH3_S 12
/* DMA_INFIFO_FULL_THRS_CH3 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt
 when Rx channel 3 received byte number in Rx FIFO is up to the value of the register.*/
#define DMA_INFIFO_FULL_THRS_CH3 0x00000FFF
#define DMA_INFIFO_FULL_THRS_CH3_M ((DMA_INFIFO_FULL_THRS_CH3_V) << (DMA_INFIFO_FULL_THRS_CH3_S))
#define DMA_INFIFO_FULL_THRS_CH3_V 0xFFF
#define DMA_INFIFO_FULL_THRS_CH3_S 0

#define DMA_CONF1_CH4_REG (DR_REG_DMA_BASE + 0x024)
/* DMA_OUT_EXT_MEM_BK_SIZE_CH4 : R/W ;bitpos:[16:15] ;default: 2'b0 ; */
/*description: Block size of Tx channel 4 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_OUT_EXT_MEM_BK_SIZE_CH4 0x00000003
#define DMA_OUT_EXT_MEM_BK_SIZE_CH4_M ((DMA_OUT_EXT_MEM_BK_SIZE_CH4_V) << (DMA_OUT_EXT_MEM_BK_SIZE_CH4_S))
#define DMA_OUT_EXT_MEM_BK_SIZE_CH4_V 0x3
#define DMA_OUT_EXT_MEM_BK_SIZE_CH4_S 15
/* DMA_IN_EXT_MEM_BK_SIZE_CH4 : R/W ;bitpos:[14:13] ;default: 2'b0 ; */
/*description: Block size of Rx channel 4 when DMA access external SRAM. 0:
 16 bytes      1: 32 bytes    2/3:reserved*/
#define DMA_IN_EXT_MEM_BK_SIZE_CH4 0x00000003
#define DMA_IN_EXT_MEM_BK_SIZE_CH4_M ((DMA_IN_EXT_MEM_BK_SIZE_CH4_V) << (DMA_IN_EXT_MEM_BK_SIZE_CH4_S))
#define DMA_IN_EXT_MEM_BK_SIZE_CH4_V 0x3
#define DMA_IN_EXT_MEM_BK_SIZE_CH4_S 13
/* DMA_CHECK_OWNER_CH4 : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to enable checking the owner attribute of the link descriptor.*/
#define DMA_CHECK_OWNER_CH4 (BIT(12))
#define DMA_CHECK_OWNER_CH4_M (BIT(12))
#define DMA_CHECK_OWNER_CH4_V 0x1
#define DMA_CHECK_OWNER_CH4_S 12
/* DMA_INFIFO_FULL_THRS_CH4 : R/W ;bitpos:[11:0] ;default: 12'hc ; */
/*description: This register is used to generate the INFIFO_FULL_WM_INT interrupt
 when Rx channel 4 received byte number in Rx FIFO is up to the value of the register.*/
#define DMA_INFIFO_FULL_THRS_CH4 0x00000FFF
#define DMA_INFIFO_FULL_THRS_CH4_M ((DMA_INFIFO_FULL_THRS_CH4_V) << (DMA_INFIFO_FULL_THRS_CH4_S))
#define DMA_INFIFO_FULL_THRS_CH4_V 0xFFF
#define DMA_INFIFO_FULL_THRS_CH4_S 0

#define DMA_INT_RAW_CH0_REG (DR_REG_DMA_BASE + 0x028)
/* DMA_OUTFIFO_UDF_L3_CH0_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 0 is underflow.*/
#define DMA_OUTFIFO_UDF_L3_CH0_INT_RAW (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH0_INT_RAW_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH0_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH0_INT_RAW_S 17
/* DMA_OUTFIFO_OVF_L3_CH0_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 0 is overflow.*/
#define DMA_OUTFIFO_OVF_L3_CH0_INT_RAW (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH0_INT_RAW_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH0_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH0_INT_RAW_S 16
/* DMA_OUTFIFO_UDF_L1_CH0_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 0 is underflow.*/
#define DMA_OUTFIFO_UDF_L1_CH0_INT_RAW (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH0_INT_RAW_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH0_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH0_INT_RAW_S 15
/* DMA_OUTFIFO_OVF_L1_CH0_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 0 is overflow.*/
#define DMA_OUTFIFO_OVF_L1_CH0_INT_RAW (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH0_INT_RAW_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH0_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH0_INT_RAW_S 14
/* DMA_INFIFO_UDF_L3_CH0_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 0 is underflow.*/
#define DMA_INFIFO_UDF_L3_CH0_INT_RAW (BIT(13))
#define DMA_INFIFO_UDF_L3_CH0_INT_RAW_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH0_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L3_CH0_INT_RAW_S 13
/* DMA_INFIFO_OVF_L3_CH0_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 0 is overflow.*/
#define DMA_INFIFO_OVF_L3_CH0_INT_RAW (BIT(12))
#define DMA_INFIFO_OVF_L3_CH0_INT_RAW_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH0_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L3_CH0_INT_RAW_S 12
/* DMA_INFIFO_UDF_L1_CH0_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 0 is underflow.*/
#define DMA_INFIFO_UDF_L1_CH0_INT_RAW (BIT(11))
#define DMA_INFIFO_UDF_L1_CH0_INT_RAW_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH0_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L1_CH0_INT_RAW_S 11
/* DMA_INFIFO_OVF_L1_CH0_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 0 is overflow.*/
#define DMA_INFIFO_OVF_L1_CH0_INT_RAW (BIT(10))
#define DMA_INFIFO_OVF_L1_CH0_INT_RAW_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH0_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L1_CH0_INT_RAW_S 10
/* DMA_INFIFO_FULL_WM_CH0_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data
 byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 0.*/
#define DMA_INFIFO_FULL_WM_CH0_INT_RAW (BIT(9))
#define DMA_INFIFO_FULL_WM_CH0_INT_RAW_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH0_INT_RAW_V 0x1
#define DMA_INFIFO_FULL_WM_CH0_INT_RAW_S 9
/* DMA_OUT_TOTAL_EOF_CH0_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding
 a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 0.*/
#define DMA_OUT_TOTAL_EOF_CH0_INT_RAW (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH0_INT_RAW_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH0_INT_RAW_V 0x1
#define DMA_OUT_TOTAL_EOF_CH0_INT_RAW_S 8
/* DMA_IN_DSCR_EMPTY_CH0_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed
 by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 0.*/
#define DMA_IN_DSCR_EMPTY_CH0_INT_RAW (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH0_INT_RAW_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH0_INT_RAW_V 0x1
#define DMA_IN_DSCR_EMPTY_CH0_INT_RAW_S 7
/* DMA_OUT_DSCR_ERR_CH0_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink
 descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 0.*/
#define DMA_OUT_DSCR_ERR_CH0_INT_RAW (BIT(6))
#define DMA_OUT_DSCR_ERR_CH0_INT_RAW_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH0_INT_RAW_V 0x1
#define DMA_OUT_DSCR_ERR_CH0_INT_RAW_S 6
/* DMA_IN_DSCR_ERR_CH0_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink
 descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 0.*/
#define DMA_IN_DSCR_ERR_CH0_INT_RAW (BIT(5))
#define DMA_IN_DSCR_ERR_CH0_INT_RAW_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH0_INT_RAW_V 0x1
#define DMA_IN_DSCR_ERR_CH0_INT_RAW_S 5
/* DMA_OUT_EOF_CH0_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been read from memory for Tx channel 0.*/
#define DMA_OUT_EOF_CH0_INT_RAW (BIT(4))
#define DMA_OUT_EOF_CH0_INT_RAW_M (BIT(4))
#define DMA_OUT_EOF_CH0_INT_RAW_V 0x1
#define DMA_OUT_EOF_CH0_INT_RAW_S 4
/* DMA_OUT_DONE_CH0_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 0.*/
#define DMA_OUT_DONE_CH0_INT_RAW (BIT(3))
#define DMA_OUT_DONE_CH0_INT_RAW_M (BIT(3))
#define DMA_OUT_DONE_CH0_INT_RAW_V 0x1
#define DMA_OUT_DONE_CH0_INT_RAW_S 3
/* DMA_IN_ERR_EOF_CH0_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is
 detected only in the case that the peripheral is UHCI0 for Rx channel 0. For other peripherals  this raw interrupt is reserved.*/
#define DMA_IN_ERR_EOF_CH0_INT_RAW (BIT(2))
#define DMA_IN_ERR_EOF_CH0_INT_RAW_M (BIT(2))
#define DMA_IN_ERR_EOF_CH0_INT_RAW_V 0x1
#define DMA_IN_ERR_EOF_CH0_INT_RAW_S 2
/* DMA_IN_SUC_EOF_CH0_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 0. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 0.*/
#define DMA_IN_SUC_EOF_CH0_INT_RAW (BIT(1))
#define DMA_IN_SUC_EOF_CH0_INT_RAW_M (BIT(1))
#define DMA_IN_SUC_EOF_CH0_INT_RAW_V 0x1
#define DMA_IN_SUC_EOF_CH0_INT_RAW_S 1
/* DMA_IN_DONE_CH0_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 0.*/
#define DMA_IN_DONE_CH0_INT_RAW (BIT(0))
#define DMA_IN_DONE_CH0_INT_RAW_M (BIT(0))
#define DMA_IN_DONE_CH0_INT_RAW_V 0x1
#define DMA_IN_DONE_CH0_INT_RAW_S 0

#define DMA_INT_RAW_CH1_REG (DR_REG_DMA_BASE + 0x02C)
/* DMA_OUTFIFO_UDF_L3_CH1_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 1 is underflow.*/
#define DMA_OUTFIFO_UDF_L3_CH1_INT_RAW (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH1_INT_RAW_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH1_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH1_INT_RAW_S 17
/* DMA_OUTFIFO_OVF_L3_CH1_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 1 is overflow.*/
#define DMA_OUTFIFO_OVF_L3_CH1_INT_RAW (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH1_INT_RAW_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH1_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH1_INT_RAW_S 16
/* DMA_OUTFIFO_UDF_L1_CH1_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 1 is underflow.*/
#define DMA_OUTFIFO_UDF_L1_CH1_INT_RAW (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH1_INT_RAW_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH1_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH1_INT_RAW_S 15
/* DMA_OUTFIFO_OVF_L1_CH1_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 1 is overflow.*/
#define DMA_OUTFIFO_OVF_L1_CH1_INT_RAW (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH1_INT_RAW_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH1_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH1_INT_RAW_S 14
/* DMA_INFIFO_UDF_L3_CH1_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 1 is underflow.*/
#define DMA_INFIFO_UDF_L3_CH1_INT_RAW (BIT(13))
#define DMA_INFIFO_UDF_L3_CH1_INT_RAW_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH1_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L3_CH1_INT_RAW_S 13
/* DMA_INFIFO_OVF_L3_CH1_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 1 is overflow.*/
#define DMA_INFIFO_OVF_L3_CH1_INT_RAW (BIT(12))
#define DMA_INFIFO_OVF_L3_CH1_INT_RAW_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH1_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L3_CH1_INT_RAW_S 12
/* DMA_INFIFO_UDF_L1_CH1_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 1 is underflow.*/
#define DMA_INFIFO_UDF_L1_CH1_INT_RAW (BIT(11))
#define DMA_INFIFO_UDF_L1_CH1_INT_RAW_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH1_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L1_CH1_INT_RAW_S 11
/* DMA_INFIFO_OVF_L1_CH1_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 1 is overflow.*/
#define DMA_INFIFO_OVF_L1_CH1_INT_RAW (BIT(10))
#define DMA_INFIFO_OVF_L1_CH1_INT_RAW_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH1_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L1_CH1_INT_RAW_S 10
/* DMA_INFIFO_FULL_WM_CH1_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data
 byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 1.*/
#define DMA_INFIFO_FULL_WM_CH1_INT_RAW (BIT(9))
#define DMA_INFIFO_FULL_WM_CH1_INT_RAW_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH1_INT_RAW_V 0x1
#define DMA_INFIFO_FULL_WM_CH1_INT_RAW_S 9
/* DMA_OUT_TOTAL_EOF_CH1_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding
 a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 1.*/
#define DMA_OUT_TOTAL_EOF_CH1_INT_RAW (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH1_INT_RAW_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH1_INT_RAW_V 0x1
#define DMA_OUT_TOTAL_EOF_CH1_INT_RAW_S 8
/* DMA_IN_DSCR_EMPTY_CH1_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed
 by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 1.*/
#define DMA_IN_DSCR_EMPTY_CH1_INT_RAW (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH1_INT_RAW_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH1_INT_RAW_V 0x1
#define DMA_IN_DSCR_EMPTY_CH1_INT_RAW_S 7
/* DMA_OUT_DSCR_ERR_CH1_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink
 descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 1.*/
#define DMA_OUT_DSCR_ERR_CH1_INT_RAW (BIT(6))
#define DMA_OUT_DSCR_ERR_CH1_INT_RAW_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH1_INT_RAW_V 0x1
#define DMA_OUT_DSCR_ERR_CH1_INT_RAW_S 6
/* DMA_IN_DSCR_ERR_CH1_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink
 descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 1.*/
#define DMA_IN_DSCR_ERR_CH1_INT_RAW (BIT(5))
#define DMA_IN_DSCR_ERR_CH1_INT_RAW_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH1_INT_RAW_V 0x1
#define DMA_IN_DSCR_ERR_CH1_INT_RAW_S 5
/* DMA_OUT_EOF_CH1_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been read from memory for Tx channel 1.*/
#define DMA_OUT_EOF_CH1_INT_RAW (BIT(4))
#define DMA_OUT_EOF_CH1_INT_RAW_M (BIT(4))
#define DMA_OUT_EOF_CH1_INT_RAW_V 0x1
#define DMA_OUT_EOF_CH1_INT_RAW_S 4
/* DMA_OUT_DONE_CH1_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 1.*/
#define DMA_OUT_DONE_CH1_INT_RAW (BIT(3))
#define DMA_OUT_DONE_CH1_INT_RAW_M (BIT(3))
#define DMA_OUT_DONE_CH1_INT_RAW_V 0x1
#define DMA_OUT_DONE_CH1_INT_RAW_S 3
/* DMA_IN_ERR_EOF_CH1_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is
 detected only in the case that the peripheral is UHCI0 for Rx channel 1. For other peripherals  this raw interrupt is reserved.*/
#define DMA_IN_ERR_EOF_CH1_INT_RAW (BIT(2))
#define DMA_IN_ERR_EOF_CH1_INT_RAW_M (BIT(2))
#define DMA_IN_ERR_EOF_CH1_INT_RAW_V 0x1
#define DMA_IN_ERR_EOF_CH1_INT_RAW_S 2
/* DMA_IN_SUC_EOF_CH1_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 1. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 1.*/
#define DMA_IN_SUC_EOF_CH1_INT_RAW (BIT(1))
#define DMA_IN_SUC_EOF_CH1_INT_RAW_M (BIT(1))
#define DMA_IN_SUC_EOF_CH1_INT_RAW_V 0x1
#define DMA_IN_SUC_EOF_CH1_INT_RAW_S 1
/* DMA_IN_DONE_CH1_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 1.*/
#define DMA_IN_DONE_CH1_INT_RAW (BIT(0))
#define DMA_IN_DONE_CH1_INT_RAW_M (BIT(0))
#define DMA_IN_DONE_CH1_INT_RAW_V 0x1
#define DMA_IN_DONE_CH1_INT_RAW_S 0

#define DMA_INT_RAW_CH2_REG (DR_REG_DMA_BASE + 0x030)
/* DMA_OUTFIFO_UDF_L3_CH2_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 2 is underflow.*/
#define DMA_OUTFIFO_UDF_L3_CH2_INT_RAW (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH2_INT_RAW_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH2_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH2_INT_RAW_S 17
/* DMA_OUTFIFO_OVF_L3_CH2_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 2 is overflow.*/
#define DMA_OUTFIFO_OVF_L3_CH2_INT_RAW (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH2_INT_RAW_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH2_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH2_INT_RAW_S 16
/* DMA_OUTFIFO_UDF_L1_CH2_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 2 is underflow.*/
#define DMA_OUTFIFO_UDF_L1_CH2_INT_RAW (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH2_INT_RAW_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH2_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH2_INT_RAW_S 15
/* DMA_OUTFIFO_OVF_L1_CH2_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 2 is overflow.*/
#define DMA_OUTFIFO_OVF_L1_CH2_INT_RAW (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH2_INT_RAW_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH2_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH2_INT_RAW_S 14
/* DMA_INFIFO_UDF_L3_CH2_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 2 is underflow.*/
#define DMA_INFIFO_UDF_L3_CH2_INT_RAW (BIT(13))
#define DMA_INFIFO_UDF_L3_CH2_INT_RAW_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH2_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L3_CH2_INT_RAW_S 13
/* DMA_INFIFO_OVF_L3_CH2_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 2 is overflow.*/
#define DMA_INFIFO_OVF_L3_CH2_INT_RAW (BIT(12))
#define DMA_INFIFO_OVF_L3_CH2_INT_RAW_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH2_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L3_CH2_INT_RAW_S 12
/* DMA_INFIFO_UDF_L1_CH2_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 2 is underflow.*/
#define DMA_INFIFO_UDF_L1_CH2_INT_RAW (BIT(11))
#define DMA_INFIFO_UDF_L1_CH2_INT_RAW_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH2_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L1_CH2_INT_RAW_S 11
/* DMA_INFIFO_OVF_L1_CH2_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 2 is overflow.*/
#define DMA_INFIFO_OVF_L1_CH2_INT_RAW (BIT(10))
#define DMA_INFIFO_OVF_L1_CH2_INT_RAW_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH2_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L1_CH2_INT_RAW_S 10
/* DMA_INFIFO_FULL_WM_CH2_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data
 byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 2.*/
#define DMA_INFIFO_FULL_WM_CH2_INT_RAW (BIT(9))
#define DMA_INFIFO_FULL_WM_CH2_INT_RAW_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH2_INT_RAW_V 0x1
#define DMA_INFIFO_FULL_WM_CH2_INT_RAW_S 9
/* DMA_OUT_TOTAL_EOF_CH2_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding
 a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 2.*/
#define DMA_OUT_TOTAL_EOF_CH2_INT_RAW (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH2_INT_RAW_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH2_INT_RAW_V 0x1
#define DMA_OUT_TOTAL_EOF_CH2_INT_RAW_S 8
/* DMA_IN_DSCR_EMPTY_CH2_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed
 by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 2.*/
#define DMA_IN_DSCR_EMPTY_CH2_INT_RAW (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH2_INT_RAW_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH2_INT_RAW_V 0x1
#define DMA_IN_DSCR_EMPTY_CH2_INT_RAW_S 7
/* DMA_OUT_DSCR_ERR_CH2_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink
 descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 2.*/
#define DMA_OUT_DSCR_ERR_CH2_INT_RAW (BIT(6))
#define DMA_OUT_DSCR_ERR_CH2_INT_RAW_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH2_INT_RAW_V 0x1
#define DMA_OUT_DSCR_ERR_CH2_INT_RAW_S 6
/* DMA_IN_DSCR_ERR_CH2_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink
 descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 2.*/
#define DMA_IN_DSCR_ERR_CH2_INT_RAW (BIT(5))
#define DMA_IN_DSCR_ERR_CH2_INT_RAW_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH2_INT_RAW_V 0x1
#define DMA_IN_DSCR_ERR_CH2_INT_RAW_S 5
/* DMA_OUT_EOF_CH2_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been read from memory for Tx channel 2.*/
#define DMA_OUT_EOF_CH2_INT_RAW (BIT(4))
#define DMA_OUT_EOF_CH2_INT_RAW_M (BIT(4))
#define DMA_OUT_EOF_CH2_INT_RAW_V 0x1
#define DMA_OUT_EOF_CH2_INT_RAW_S 4
/* DMA_OUT_DONE_CH2_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 2.*/
#define DMA_OUT_DONE_CH2_INT_RAW (BIT(3))
#define DMA_OUT_DONE_CH2_INT_RAW_M (BIT(3))
#define DMA_OUT_DONE_CH2_INT_RAW_V 0x1
#define DMA_OUT_DONE_CH2_INT_RAW_S 3
/* DMA_IN_ERR_EOF_CH2_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is
 detected only in the case that the peripheral is UHCI0 for Rx channel 2. For other peripherals  this raw interrupt is reserved.*/
#define DMA_IN_ERR_EOF_CH2_INT_RAW (BIT(2))
#define DMA_IN_ERR_EOF_CH2_INT_RAW_M (BIT(2))
#define DMA_IN_ERR_EOF_CH2_INT_RAW_V 0x1
#define DMA_IN_ERR_EOF_CH2_INT_RAW_S 2
/* DMA_IN_SUC_EOF_CH2_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 2. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 2.*/
#define DMA_IN_SUC_EOF_CH2_INT_RAW (BIT(1))
#define DMA_IN_SUC_EOF_CH2_INT_RAW_M (BIT(1))
#define DMA_IN_SUC_EOF_CH2_INT_RAW_V 0x1
#define DMA_IN_SUC_EOF_CH2_INT_RAW_S 1
/* DMA_IN_DONE_CH2_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 2.*/
#define DMA_IN_DONE_CH2_INT_RAW (BIT(0))
#define DMA_IN_DONE_CH2_INT_RAW_M (BIT(0))
#define DMA_IN_DONE_CH2_INT_RAW_V 0x1
#define DMA_IN_DONE_CH2_INT_RAW_S 0

#define DMA_INT_RAW_CH3_REG (DR_REG_DMA_BASE + 0x034)
/* DMA_OUTFIFO_UDF_L3_CH3_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 3 is underflow.*/
#define DMA_OUTFIFO_UDF_L3_CH3_INT_RAW (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH3_INT_RAW_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH3_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH3_INT_RAW_S 17
/* DMA_OUTFIFO_OVF_L3_CH3_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 3 is overflow.*/
#define DMA_OUTFIFO_OVF_L3_CH3_INT_RAW (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH3_INT_RAW_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH3_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH3_INT_RAW_S 16
/* DMA_OUTFIFO_UDF_L1_CH3_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 3 is underflow.*/
#define DMA_OUTFIFO_UDF_L1_CH3_INT_RAW (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH3_INT_RAW_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH3_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH3_INT_RAW_S 15
/* DMA_OUTFIFO_OVF_L1_CH3_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 3 is overflow.*/
#define DMA_OUTFIFO_OVF_L1_CH3_INT_RAW (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH3_INT_RAW_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH3_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH3_INT_RAW_S 14
/* DMA_INFIFO_UDF_L3_CH3_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 3 is underflow.*/
#define DMA_INFIFO_UDF_L3_CH3_INT_RAW (BIT(13))
#define DMA_INFIFO_UDF_L3_CH3_INT_RAW_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH3_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L3_CH3_INT_RAW_S 13
/* DMA_INFIFO_OVF_L3_CH3_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 3 is overflow.*/
#define DMA_INFIFO_OVF_L3_CH3_INT_RAW (BIT(12))
#define DMA_INFIFO_OVF_L3_CH3_INT_RAW_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH3_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L3_CH3_INT_RAW_S 12
/* DMA_INFIFO_UDF_L1_CH3_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 3 is underflow.*/
#define DMA_INFIFO_UDF_L1_CH3_INT_RAW (BIT(11))
#define DMA_INFIFO_UDF_L1_CH3_INT_RAW_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH3_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L1_CH3_INT_RAW_S 11
/* DMA_INFIFO_OVF_L1_CH3_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 3 is overflow.*/
#define DMA_INFIFO_OVF_L1_CH3_INT_RAW (BIT(10))
#define DMA_INFIFO_OVF_L1_CH3_INT_RAW_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH3_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L1_CH3_INT_RAW_S 10
/* DMA_INFIFO_FULL_WM_CH3_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data
 byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 3.*/
#define DMA_INFIFO_FULL_WM_CH3_INT_RAW (BIT(9))
#define DMA_INFIFO_FULL_WM_CH3_INT_RAW_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH3_INT_RAW_V 0x1
#define DMA_INFIFO_FULL_WM_CH3_INT_RAW_S 9
/* DMA_OUT_TOTAL_EOF_CH3_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding
 a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 3.*/
#define DMA_OUT_TOTAL_EOF_CH3_INT_RAW (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH3_INT_RAW_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH3_INT_RAW_V 0x1
#define DMA_OUT_TOTAL_EOF_CH3_INT_RAW_S 8
/* DMA_IN_DSCR_EMPTY_CH3_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed
 by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 3.*/
#define DMA_IN_DSCR_EMPTY_CH3_INT_RAW (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH3_INT_RAW_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH3_INT_RAW_V 0x1
#define DMA_IN_DSCR_EMPTY_CH3_INT_RAW_S 7
/* DMA_OUT_DSCR_ERR_CH3_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink
 descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 3.*/
#define DMA_OUT_DSCR_ERR_CH3_INT_RAW (BIT(6))
#define DMA_OUT_DSCR_ERR_CH3_INT_RAW_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH3_INT_RAW_V 0x1
#define DMA_OUT_DSCR_ERR_CH3_INT_RAW_S 6
/* DMA_IN_DSCR_ERR_CH3_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink
 descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 3.*/
#define DMA_IN_DSCR_ERR_CH3_INT_RAW (BIT(5))
#define DMA_IN_DSCR_ERR_CH3_INT_RAW_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH3_INT_RAW_V 0x1
#define DMA_IN_DSCR_ERR_CH3_INT_RAW_S 5
/* DMA_OUT_EOF_CH3_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been read from memory for Tx channel 3.*/
#define DMA_OUT_EOF_CH3_INT_RAW (BIT(4))
#define DMA_OUT_EOF_CH3_INT_RAW_M (BIT(4))
#define DMA_OUT_EOF_CH3_INT_RAW_V 0x1
#define DMA_OUT_EOF_CH3_INT_RAW_S 4
/* DMA_OUT_DONE_CH3_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 3.*/
#define DMA_OUT_DONE_CH3_INT_RAW (BIT(3))
#define DMA_OUT_DONE_CH3_INT_RAW_M (BIT(3))
#define DMA_OUT_DONE_CH3_INT_RAW_V 0x1
#define DMA_OUT_DONE_CH3_INT_RAW_S 3
/* DMA_IN_ERR_EOF_CH3_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is
 detected only in the case that the peripheral is UHCI0 for Rx channel 3. For other peripherals  this raw interrupt is reserved.*/
#define DMA_IN_ERR_EOF_CH3_INT_RAW (BIT(2))
#define DMA_IN_ERR_EOF_CH3_INT_RAW_M (BIT(2))
#define DMA_IN_ERR_EOF_CH3_INT_RAW_V 0x1
#define DMA_IN_ERR_EOF_CH3_INT_RAW_S 2
/* DMA_IN_SUC_EOF_CH3_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 3. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 3.*/
#define DMA_IN_SUC_EOF_CH3_INT_RAW (BIT(1))
#define DMA_IN_SUC_EOF_CH3_INT_RAW_M (BIT(1))
#define DMA_IN_SUC_EOF_CH3_INT_RAW_V 0x1
#define DMA_IN_SUC_EOF_CH3_INT_RAW_S 1
/* DMA_IN_DONE_CH3_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 3.*/
#define DMA_IN_DONE_CH3_INT_RAW (BIT(0))
#define DMA_IN_DONE_CH3_INT_RAW_M (BIT(0))
#define DMA_IN_DONE_CH3_INT_RAW_V 0x1
#define DMA_IN_DONE_CH3_INT_RAW_S 0

#define DMA_INT_RAW_CH4_REG (DR_REG_DMA_BASE + 0x038)
/* DMA_OUTFIFO_UDF_L3_CH4_INT_RAW : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 4 is underflow.*/
#define DMA_OUTFIFO_UDF_L3_CH4_INT_RAW (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH4_INT_RAW_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH4_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH4_INT_RAW_S 17
/* DMA_OUTFIFO_OVF_L3_CH4_INT_RAW : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Tx channel 4 is overflow.*/
#define DMA_OUTFIFO_OVF_L3_CH4_INT_RAW (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH4_INT_RAW_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH4_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH4_INT_RAW_S 16
/* DMA_OUTFIFO_UDF_L1_CH4_INT_RAW : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 4 is underflow.*/
#define DMA_OUTFIFO_UDF_L1_CH4_INT_RAW (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH4_INT_RAW_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH4_INT_RAW_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH4_INT_RAW_S 15
/* DMA_OUTFIFO_OVF_L1_CH4_INT_RAW : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Tx channel 4 is overflow.*/
#define DMA_OUTFIFO_OVF_L1_CH4_INT_RAW (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH4_INT_RAW_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH4_INT_RAW_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH4_INT_RAW_S 14
/* DMA_INFIFO_UDF_L3_CH4_INT_RAW : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 4 is underflow.*/
#define DMA_INFIFO_UDF_L3_CH4_INT_RAW (BIT(13))
#define DMA_INFIFO_UDF_L3_CH4_INT_RAW_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH4_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L3_CH4_INT_RAW_S 13
/* DMA_INFIFO_OVF_L3_CH4_INT_RAW : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 3 fifo
 of Rx channel 4 is overflow.*/
#define DMA_INFIFO_OVF_L3_CH4_INT_RAW (BIT(12))
#define DMA_INFIFO_OVF_L3_CH4_INT_RAW_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH4_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L3_CH4_INT_RAW_S 12
/* DMA_INFIFO_UDF_L1_CH4_INT_RAW : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 4 is underflow.*/
#define DMA_INFIFO_UDF_L1_CH4_INT_RAW (BIT(11))
#define DMA_INFIFO_UDF_L1_CH4_INT_RAW_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH4_INT_RAW_V 0x1
#define DMA_INFIFO_UDF_L1_CH4_INT_RAW_S 11
/* DMA_INFIFO_OVF_L1_CH4_INT_RAW : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: This raw interrupt bit turns to high level when level 1 fifo
 of Rx channel 4 is overflow.*/
#define DMA_INFIFO_OVF_L1_CH4_INT_RAW (BIT(10))
#define DMA_INFIFO_OVF_L1_CH4_INT_RAW_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH4_INT_RAW_V 0x1
#define DMA_INFIFO_OVF_L1_CH4_INT_RAW_S 10
/* DMA_INFIFO_FULL_WM_CH4_INT_RAW : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when received data
 byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 4.*/
#define DMA_INFIFO_FULL_WM_CH4_INT_RAW (BIT(9))
#define DMA_INFIFO_FULL_WM_CH4_INT_RAW_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH4_INT_RAW_V 0x1
#define DMA_INFIFO_FULL_WM_CH4_INT_RAW_S 9
/* DMA_OUT_TOTAL_EOF_CH4_INT_RAW : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data corresponding
 a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 4.*/
#define DMA_OUT_TOTAL_EOF_CH4_INT_RAW (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH4_INT_RAW_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH4_INT_RAW_V 0x1
#define DMA_OUT_TOTAL_EOF_CH4_INT_RAW_S 8
/* DMA_IN_DSCR_EMPTY_CH4_INT_RAW : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when Rx buffer pointed
 by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 4.*/
#define DMA_IN_DSCR_EMPTY_CH4_INT_RAW (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH4_INT_RAW_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH4_INT_RAW_V 0x1
#define DMA_IN_DSCR_EMPTY_CH4_INT_RAW_S 7
/* DMA_OUT_DSCR_ERR_CH4_INT_RAW : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting outlink
 descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 4.*/
#define DMA_OUT_DSCR_ERR_CH4_INT_RAW (BIT(6))
#define DMA_OUT_DSCR_ERR_CH4_INT_RAW_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH4_INT_RAW_V 0x1
#define DMA_OUT_DSCR_ERR_CH4_INT_RAW_S 6
/* DMA_IN_DSCR_ERR_CH4_INT_RAW : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when detecting inlink
 descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 4.*/
#define DMA_IN_DSCR_ERR_CH4_INT_RAW (BIT(5))
#define DMA_IN_DSCR_ERR_CH4_INT_RAW_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH4_INT_RAW_V 0x1
#define DMA_IN_DSCR_ERR_CH4_INT_RAW_S 5
/* DMA_OUT_EOF_CH4_INT_RAW : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been read from memory for Tx channel 4.*/
#define DMA_OUT_EOF_CH4_INT_RAW (BIT(4))
#define DMA_OUT_EOF_CH4_INT_RAW_M (BIT(4))
#define DMA_OUT_EOF_CH4_INT_RAW_V 0x1
#define DMA_OUT_EOF_CH4_INT_RAW_S 4
/* DMA_OUT_DONE_CH4_INT_RAW : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 4.*/
#define DMA_OUT_DONE_CH4_INT_RAW (BIT(3))
#define DMA_OUT_DONE_CH4_INT_RAW_M (BIT(3))
#define DMA_OUT_DONE_CH4_INT_RAW_V 0x1
#define DMA_OUT_DONE_CH4_INT_RAW_S 3
/* DMA_IN_ERR_EOF_CH4_INT_RAW : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when data error is
 detected only in the case that the peripheral is UHCI0 for Rx channel 4. For other peripherals  this raw interrupt is reserved.*/
#define DMA_IN_ERR_EOF_CH4_INT_RAW (BIT(2))
#define DMA_IN_ERR_EOF_CH4_INT_RAW_M (BIT(2))
#define DMA_IN_ERR_EOF_CH4_INT_RAW_V 0x1
#define DMA_IN_ERR_EOF_CH4_INT_RAW_S 2
/* DMA_IN_SUC_EOF_CH4_INT_RAW : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 4. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 4.*/
#define DMA_IN_SUC_EOF_CH4_INT_RAW (BIT(1))
#define DMA_IN_SUC_EOF_CH4_INT_RAW_M (BIT(1))
#define DMA_IN_SUC_EOF_CH4_INT_RAW_V 0x1
#define DMA_IN_SUC_EOF_CH4_INT_RAW_S 1
/* DMA_IN_DONE_CH4_INT_RAW : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt bit turns to high level when the last data
 pointed by one inlink descriptor has been received for Rx channel 4.*/
#define DMA_IN_DONE_CH4_INT_RAW (BIT(0))
#define DMA_IN_DONE_CH4_INT_RAW_M (BIT(0))
#define DMA_IN_DONE_CH4_INT_RAW_V 0x1
#define DMA_IN_DONE_CH4_INT_RAW_S 0

#define DMA_INT_ST_CH0_REG (DR_REG_DMA_BASE + 0x040)
/* DMA_OUTFIFO_UDF_L3_CH0_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH0_INT_ST (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH0_INT_ST_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH0_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH0_INT_ST_S 17
/* DMA_OUTFIFO_OVF_L3_CH0_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH0_INT_ST (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH0_INT_ST_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH0_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH0_INT_ST_S 16
/* DMA_OUTFIFO_UDF_L1_CH0_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH0_INT_ST (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH0_INT_ST_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH0_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH0_INT_ST_S 15
/* DMA_OUTFIFO_OVF_L1_CH0_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH0_INT_ST (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH0_INT_ST_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH0_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH0_INT_ST_S 14
/* DMA_INFIFO_UDF_L3_CH0_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH0_INT_ST (BIT(13))
#define DMA_INFIFO_UDF_L3_CH0_INT_ST_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH0_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L3_CH0_INT_ST_S 13
/* DMA_INFIFO_OVF_L3_CH0_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH0_INT_ST (BIT(12))
#define DMA_INFIFO_OVF_L3_CH0_INT_ST_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH0_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L3_CH0_INT_ST_S 12
/* DMA_INFIFO_UDF_L1_CH0_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH0_INT_ST (BIT(11))
#define DMA_INFIFO_UDF_L1_CH0_INT_ST_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH0_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L1_CH0_INT_ST_S 11
/* DMA_INFIFO_OVF_L1_CH0_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH0_INT_ST (BIT(10))
#define DMA_INFIFO_OVF_L1_CH0_INT_ST_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH0_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L1_CH0_INT_ST_S 10
/* DMA_INFIFO_FULL_WM_CH0_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH0_INT_ST (BIT(9))
#define DMA_INFIFO_FULL_WM_CH0_INT_ST_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH0_INT_ST_V 0x1
#define DMA_INFIFO_FULL_WM_CH0_INT_ST_S 9
/* DMA_OUT_TOTAL_EOF_CH0_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH0_INT_ST (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH0_INT_ST_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH0_INT_ST_V 0x1
#define DMA_OUT_TOTAL_EOF_CH0_INT_ST_S 8
/* DMA_IN_DSCR_EMPTY_CH0_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH0_INT_ST (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH0_INT_ST_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH0_INT_ST_V 0x1
#define DMA_IN_DSCR_EMPTY_CH0_INT_ST_S 7
/* DMA_OUT_DSCR_ERR_CH0_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH0_INT_ST (BIT(6))
#define DMA_OUT_DSCR_ERR_CH0_INT_ST_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH0_INT_ST_V 0x1
#define DMA_OUT_DSCR_ERR_CH0_INT_ST_S 6
/* DMA_IN_DSCR_ERR_CH0_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH0_INT_ST (BIT(5))
#define DMA_IN_DSCR_ERR_CH0_INT_ST_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH0_INT_ST_V 0x1
#define DMA_IN_DSCR_ERR_CH0_INT_ST_S 5
/* DMA_OUT_EOF_CH0_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH0_INT_ST (BIT(4))
#define DMA_OUT_EOF_CH0_INT_ST_M (BIT(4))
#define DMA_OUT_EOF_CH0_INT_ST_V 0x1
#define DMA_OUT_EOF_CH0_INT_ST_S 4
/* DMA_OUT_DONE_CH0_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH0_INT_ST (BIT(3))
#define DMA_OUT_DONE_CH0_INT_ST_M (BIT(3))
#define DMA_OUT_DONE_CH0_INT_ST_V 0x1
#define DMA_OUT_DONE_CH0_INT_ST_S 3
/* DMA_IN_ERR_EOF_CH0_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH0_INT_ST (BIT(2))
#define DMA_IN_ERR_EOF_CH0_INT_ST_M (BIT(2))
#define DMA_IN_ERR_EOF_CH0_INT_ST_V 0x1
#define DMA_IN_ERR_EOF_CH0_INT_ST_S 2
/* DMA_IN_SUC_EOF_CH0_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH0_INT_ST (BIT(1))
#define DMA_IN_SUC_EOF_CH0_INT_ST_M (BIT(1))
#define DMA_IN_SUC_EOF_CH0_INT_ST_V 0x1
#define DMA_IN_SUC_EOF_CH0_INT_ST_S 1
/* DMA_IN_DONE_CH0_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH0_INT_ST (BIT(0))
#define DMA_IN_DONE_CH0_INT_ST_M (BIT(0))
#define DMA_IN_DONE_CH0_INT_ST_V 0x1
#define DMA_IN_DONE_CH0_INT_ST_S 0

#define DMA_INT_ST_CH1_REG (DR_REG_DMA_BASE + 0x044)
/* DMA_OUTFIFO_UDF_L3_CH1_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH1_INT_ST (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH1_INT_ST_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH1_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH1_INT_ST_S 17
/* DMA_OUTFIFO_OVF_L3_CH1_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH1_INT_ST (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH1_INT_ST_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH1_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH1_INT_ST_S 16
/* DMA_OUTFIFO_UDF_L1_CH1_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH1_INT_ST (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH1_INT_ST_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH1_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH1_INT_ST_S 15
/* DMA_OUTFIFO_OVF_L1_CH1_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH1_INT_ST (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH1_INT_ST_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH1_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH1_INT_ST_S 14
/* DMA_INFIFO_UDF_L3_CH1_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH1_INT_ST (BIT(13))
#define DMA_INFIFO_UDF_L3_CH1_INT_ST_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH1_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L3_CH1_INT_ST_S 13
/* DMA_INFIFO_OVF_L3_CH1_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH1_INT_ST (BIT(12))
#define DMA_INFIFO_OVF_L3_CH1_INT_ST_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH1_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L3_CH1_INT_ST_S 12
/* DMA_INFIFO_UDF_L1_CH1_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH1_INT_ST (BIT(11))
#define DMA_INFIFO_UDF_L1_CH1_INT_ST_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH1_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L1_CH1_INT_ST_S 11
/* DMA_INFIFO_OVF_L1_CH1_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH1_INT_ST (BIT(10))
#define DMA_INFIFO_OVF_L1_CH1_INT_ST_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH1_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L1_CH1_INT_ST_S 10
/* DMA_INFIFO_FULL_WM_CH1_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH1_INT_ST (BIT(9))
#define DMA_INFIFO_FULL_WM_CH1_INT_ST_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH1_INT_ST_V 0x1
#define DMA_INFIFO_FULL_WM_CH1_INT_ST_S 9
/* DMA_OUT_TOTAL_EOF_CH1_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH1_INT_ST (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH1_INT_ST_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH1_INT_ST_V 0x1
#define DMA_OUT_TOTAL_EOF_CH1_INT_ST_S 8
/* DMA_IN_DSCR_EMPTY_CH1_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH1_INT_ST (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH1_INT_ST_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH1_INT_ST_V 0x1
#define DMA_IN_DSCR_EMPTY_CH1_INT_ST_S 7
/* DMA_OUT_DSCR_ERR_CH1_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH1_INT_ST (BIT(6))
#define DMA_OUT_DSCR_ERR_CH1_INT_ST_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH1_INT_ST_V 0x1
#define DMA_OUT_DSCR_ERR_CH1_INT_ST_S 6
/* DMA_IN_DSCR_ERR_CH1_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH1_INT_ST (BIT(5))
#define DMA_IN_DSCR_ERR_CH1_INT_ST_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH1_INT_ST_V 0x1
#define DMA_IN_DSCR_ERR_CH1_INT_ST_S 5
/* DMA_OUT_EOF_CH1_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH1_INT_ST (BIT(4))
#define DMA_OUT_EOF_CH1_INT_ST_M (BIT(4))
#define DMA_OUT_EOF_CH1_INT_ST_V 0x1
#define DMA_OUT_EOF_CH1_INT_ST_S 4
/* DMA_OUT_DONE_CH1_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH1_INT_ST (BIT(3))
#define DMA_OUT_DONE_CH1_INT_ST_M (BIT(3))
#define DMA_OUT_DONE_CH1_INT_ST_V 0x1
#define DMA_OUT_DONE_CH1_INT_ST_S 3
/* DMA_IN_ERR_EOF_CH1_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH1_INT_ST (BIT(2))
#define DMA_IN_ERR_EOF_CH1_INT_ST_M (BIT(2))
#define DMA_IN_ERR_EOF_CH1_INT_ST_V 0x1
#define DMA_IN_ERR_EOF_CH1_INT_ST_S 2
/* DMA_IN_SUC_EOF_CH1_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH1_INT_ST (BIT(1))
#define DMA_IN_SUC_EOF_CH1_INT_ST_M (BIT(1))
#define DMA_IN_SUC_EOF_CH1_INT_ST_V 0x1
#define DMA_IN_SUC_EOF_CH1_INT_ST_S 1
/* DMA_IN_DONE_CH1_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH1_INT_ST (BIT(0))
#define DMA_IN_DONE_CH1_INT_ST_M (BIT(0))
#define DMA_IN_DONE_CH1_INT_ST_V 0x1
#define DMA_IN_DONE_CH1_INT_ST_S 0

#define DMA_INT_ST_CH2_REG (DR_REG_DMA_BASE + 0x048)
/* DMA_OUTFIFO_UDF_L3_CH2_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH2_INT_ST (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH2_INT_ST_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH2_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH2_INT_ST_S 17
/* DMA_OUTFIFO_OVF_L3_CH2_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH2_INT_ST (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH2_INT_ST_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH2_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH2_INT_ST_S 16
/* DMA_OUTFIFO_UDF_L1_CH2_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH2_INT_ST (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH2_INT_ST_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH2_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH2_INT_ST_S 15
/* DMA_OUTFIFO_OVF_L1_CH2_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH2_INT_ST (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH2_INT_ST_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH2_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH2_INT_ST_S 14
/* DMA_INFIFO_UDF_L3_CH2_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH2_INT_ST (BIT(13))
#define DMA_INFIFO_UDF_L3_CH2_INT_ST_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH2_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L3_CH2_INT_ST_S 13
/* DMA_INFIFO_OVF_L3_CH2_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH2_INT_ST (BIT(12))
#define DMA_INFIFO_OVF_L3_CH2_INT_ST_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH2_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L3_CH2_INT_ST_S 12
/* DMA_INFIFO_UDF_L1_CH2_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH2_INT_ST (BIT(11))
#define DMA_INFIFO_UDF_L1_CH2_INT_ST_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH2_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L1_CH2_INT_ST_S 11
/* DMA_INFIFO_OVF_L1_CH2_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH2_INT_ST (BIT(10))
#define DMA_INFIFO_OVF_L1_CH2_INT_ST_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH2_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L1_CH2_INT_ST_S 10
/* DMA_INFIFO_FULL_WM_CH2_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH2_INT_ST (BIT(9))
#define DMA_INFIFO_FULL_WM_CH2_INT_ST_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH2_INT_ST_V 0x1
#define DMA_INFIFO_FULL_WM_CH2_INT_ST_S 9
/* DMA_OUT_TOTAL_EOF_CH2_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH2_INT_ST (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH2_INT_ST_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH2_INT_ST_V 0x1
#define DMA_OUT_TOTAL_EOF_CH2_INT_ST_S 8
/* DMA_IN_DSCR_EMPTY_CH2_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH2_INT_ST (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH2_INT_ST_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH2_INT_ST_V 0x1
#define DMA_IN_DSCR_EMPTY_CH2_INT_ST_S 7
/* DMA_OUT_DSCR_ERR_CH2_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH2_INT_ST (BIT(6))
#define DMA_OUT_DSCR_ERR_CH2_INT_ST_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH2_INT_ST_V 0x1
#define DMA_OUT_DSCR_ERR_CH2_INT_ST_S 6
/* DMA_IN_DSCR_ERR_CH2_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH2_INT_ST (BIT(5))
#define DMA_IN_DSCR_ERR_CH2_INT_ST_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH2_INT_ST_V 0x1
#define DMA_IN_DSCR_ERR_CH2_INT_ST_S 5
/* DMA_OUT_EOF_CH2_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH2_INT_ST (BIT(4))
#define DMA_OUT_EOF_CH2_INT_ST_M (BIT(4))
#define DMA_OUT_EOF_CH2_INT_ST_V 0x1
#define DMA_OUT_EOF_CH2_INT_ST_S 4
/* DMA_OUT_DONE_CH2_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH2_INT_ST (BIT(3))
#define DMA_OUT_DONE_CH2_INT_ST_M (BIT(3))
#define DMA_OUT_DONE_CH2_INT_ST_V 0x1
#define DMA_OUT_DONE_CH2_INT_ST_S 3
/* DMA_IN_ERR_EOF_CH2_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH2_INT_ST (BIT(2))
#define DMA_IN_ERR_EOF_CH2_INT_ST_M (BIT(2))
#define DMA_IN_ERR_EOF_CH2_INT_ST_V 0x1
#define DMA_IN_ERR_EOF_CH2_INT_ST_S 2
/* DMA_IN_SUC_EOF_CH2_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH2_INT_ST (BIT(1))
#define DMA_IN_SUC_EOF_CH2_INT_ST_M (BIT(1))
#define DMA_IN_SUC_EOF_CH2_INT_ST_V 0x1
#define DMA_IN_SUC_EOF_CH2_INT_ST_S 1
/* DMA_IN_DONE_CH2_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH2_INT_ST (BIT(0))
#define DMA_IN_DONE_CH2_INT_ST_M (BIT(0))
#define DMA_IN_DONE_CH2_INT_ST_V 0x1
#define DMA_IN_DONE_CH2_INT_ST_S 0

#define DMA_INT_ST_CH3_REG (DR_REG_DMA_BASE + 0x04C)
/* DMA_OUTFIFO_UDF_L3_CH3_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH3_INT_ST (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH3_INT_ST_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH3_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH3_INT_ST_S 17
/* DMA_OUTFIFO_OVF_L3_CH3_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH3_INT_ST (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH3_INT_ST_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH3_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH3_INT_ST_S 16
/* DMA_OUTFIFO_UDF_L1_CH3_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH3_INT_ST (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH3_INT_ST_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH3_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH3_INT_ST_S 15
/* DMA_OUTFIFO_OVF_L1_CH3_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH3_INT_ST (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH3_INT_ST_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH3_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH3_INT_ST_S 14
/* DMA_INFIFO_UDF_L3_CH3_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH3_INT_ST (BIT(13))
#define DMA_INFIFO_UDF_L3_CH3_INT_ST_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH3_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L3_CH3_INT_ST_S 13
/* DMA_INFIFO_OVF_L3_CH3_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH3_INT_ST (BIT(12))
#define DMA_INFIFO_OVF_L3_CH3_INT_ST_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH3_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L3_CH3_INT_ST_S 12
/* DMA_INFIFO_UDF_L1_CH3_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH3_INT_ST (BIT(11))
#define DMA_INFIFO_UDF_L1_CH3_INT_ST_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH3_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L1_CH3_INT_ST_S 11
/* DMA_INFIFO_OVF_L1_CH3_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH3_INT_ST (BIT(10))
#define DMA_INFIFO_OVF_L1_CH3_INT_ST_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH3_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L1_CH3_INT_ST_S 10
/* DMA_INFIFO_FULL_WM_CH3_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH3_INT_ST (BIT(9))
#define DMA_INFIFO_FULL_WM_CH3_INT_ST_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH3_INT_ST_V 0x1
#define DMA_INFIFO_FULL_WM_CH3_INT_ST_S 9
/* DMA_OUT_TOTAL_EOF_CH3_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH3_INT_ST (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH3_INT_ST_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH3_INT_ST_V 0x1
#define DMA_OUT_TOTAL_EOF_CH3_INT_ST_S 8
/* DMA_IN_DSCR_EMPTY_CH3_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH3_INT_ST (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH3_INT_ST_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH3_INT_ST_V 0x1
#define DMA_IN_DSCR_EMPTY_CH3_INT_ST_S 7
/* DMA_OUT_DSCR_ERR_CH3_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH3_INT_ST (BIT(6))
#define DMA_OUT_DSCR_ERR_CH3_INT_ST_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH3_INT_ST_V 0x1
#define DMA_OUT_DSCR_ERR_CH3_INT_ST_S 6
/* DMA_IN_DSCR_ERR_CH3_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH3_INT_ST (BIT(5))
#define DMA_IN_DSCR_ERR_CH3_INT_ST_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH3_INT_ST_V 0x1
#define DMA_IN_DSCR_ERR_CH3_INT_ST_S 5
/* DMA_OUT_EOF_CH3_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH3_INT_ST (BIT(4))
#define DMA_OUT_EOF_CH3_INT_ST_M (BIT(4))
#define DMA_OUT_EOF_CH3_INT_ST_V 0x1
#define DMA_OUT_EOF_CH3_INT_ST_S 4
/* DMA_OUT_DONE_CH3_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH3_INT_ST (BIT(3))
#define DMA_OUT_DONE_CH3_INT_ST_M (BIT(3))
#define DMA_OUT_DONE_CH3_INT_ST_V 0x1
#define DMA_OUT_DONE_CH3_INT_ST_S 3
/* DMA_IN_ERR_EOF_CH3_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH3_INT_ST (BIT(2))
#define DMA_IN_ERR_EOF_CH3_INT_ST_M (BIT(2))
#define DMA_IN_ERR_EOF_CH3_INT_ST_V 0x1
#define DMA_IN_ERR_EOF_CH3_INT_ST_S 2
/* DMA_IN_SUC_EOF_CH3_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH3_INT_ST (BIT(1))
#define DMA_IN_SUC_EOF_CH3_INT_ST_M (BIT(1))
#define DMA_IN_SUC_EOF_CH3_INT_ST_V 0x1
#define DMA_IN_SUC_EOF_CH3_INT_ST_S 1
/* DMA_IN_DONE_CH3_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH3_INT_ST (BIT(0))
#define DMA_IN_DONE_CH3_INT_ST_M (BIT(0))
#define DMA_IN_DONE_CH3_INT_ST_V 0x1
#define DMA_IN_DONE_CH3_INT_ST_S 0

#define DMA_INT_ST_CH4_REG (DR_REG_DMA_BASE + 0x050)
/* DMA_OUTFIFO_UDF_L3_CH4_INT_ST : RO ;bitpos:[17] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH4_INT_ST (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH4_INT_ST_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH4_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH4_INT_ST_S 17
/* DMA_OUTFIFO_OVF_L3_CH4_INT_ST : RO ;bitpos:[16] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH4_INT_ST (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH4_INT_ST_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH4_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH4_INT_ST_S 16
/* DMA_OUTFIFO_UDF_L1_CH4_INT_ST : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH4_INT_ST (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH4_INT_ST_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH4_INT_ST_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH4_INT_ST_S 15
/* DMA_OUTFIFO_OVF_L1_CH4_INT_ST : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH4_INT_ST (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH4_INT_ST_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH4_INT_ST_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH4_INT_ST_S 14
/* DMA_INFIFO_UDF_L3_CH4_INT_ST : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH4_INT_ST (BIT(13))
#define DMA_INFIFO_UDF_L3_CH4_INT_ST_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH4_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L3_CH4_INT_ST_S 13
/* DMA_INFIFO_OVF_L3_CH4_INT_ST : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH4_INT_ST (BIT(12))
#define DMA_INFIFO_OVF_L3_CH4_INT_ST_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH4_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L3_CH4_INT_ST_S 12
/* DMA_INFIFO_UDF_L1_CH4_INT_ST : RO ;bitpos:[11] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH4_INT_ST (BIT(11))
#define DMA_INFIFO_UDF_L1_CH4_INT_ST_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH4_INT_ST_V 0x1
#define DMA_INFIFO_UDF_L1_CH4_INT_ST_S 11
/* DMA_INFIFO_OVF_L1_CH4_INT_ST : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH4_INT_ST (BIT(10))
#define DMA_INFIFO_OVF_L1_CH4_INT_ST_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH4_INT_ST_V 0x1
#define DMA_INFIFO_OVF_L1_CH4_INT_ST_S 10
/* DMA_INFIFO_FULL_WM_CH4_INT_ST : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH4_INT_ST (BIT(9))
#define DMA_INFIFO_FULL_WM_CH4_INT_ST_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH4_INT_ST_V 0x1
#define DMA_INFIFO_FULL_WM_CH4_INT_ST_S 9
/* DMA_OUT_TOTAL_EOF_CH4_INT_ST : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH4_INT_ST (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH4_INT_ST_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH4_INT_ST_V 0x1
#define DMA_OUT_TOTAL_EOF_CH4_INT_ST_S 8
/* DMA_IN_DSCR_EMPTY_CH4_INT_ST : RO ;bitpos:[7] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH4_INT_ST (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH4_INT_ST_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH4_INT_ST_V 0x1
#define DMA_IN_DSCR_EMPTY_CH4_INT_ST_S 7
/* DMA_OUT_DSCR_ERR_CH4_INT_ST : RO ;bitpos:[6] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH4_INT_ST (BIT(6))
#define DMA_OUT_DSCR_ERR_CH4_INT_ST_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH4_INT_ST_V 0x1
#define DMA_OUT_DSCR_ERR_CH4_INT_ST_S 6
/* DMA_IN_DSCR_ERR_CH4_INT_ST : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH4_INT_ST (BIT(5))
#define DMA_IN_DSCR_ERR_CH4_INT_ST_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH4_INT_ST_V 0x1
#define DMA_IN_DSCR_ERR_CH4_INT_ST_S 5
/* DMA_OUT_EOF_CH4_INT_ST : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH4_INT_ST (BIT(4))
#define DMA_OUT_EOF_CH4_INT_ST_M (BIT(4))
#define DMA_OUT_EOF_CH4_INT_ST_V 0x1
#define DMA_OUT_EOF_CH4_INT_ST_S 4
/* DMA_OUT_DONE_CH4_INT_ST : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH4_INT_ST (BIT(3))
#define DMA_OUT_DONE_CH4_INT_ST_M (BIT(3))
#define DMA_OUT_DONE_CH4_INT_ST_V 0x1
#define DMA_OUT_DONE_CH4_INT_ST_S 3
/* DMA_IN_ERR_EOF_CH4_INT_ST : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH4_INT_ST (BIT(2))
#define DMA_IN_ERR_EOF_CH4_INT_ST_M (BIT(2))
#define DMA_IN_ERR_EOF_CH4_INT_ST_V 0x1
#define DMA_IN_ERR_EOF_CH4_INT_ST_S 2
/* DMA_IN_SUC_EOF_CH4_INT_ST : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH4_INT_ST (BIT(1))
#define DMA_IN_SUC_EOF_CH4_INT_ST_M (BIT(1))
#define DMA_IN_SUC_EOF_CH4_INT_ST_V 0x1
#define DMA_IN_SUC_EOF_CH4_INT_ST_S 1
/* DMA_IN_DONE_CH4_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH4_INT_ST (BIT(0))
#define DMA_IN_DONE_CH4_INT_ST_M (BIT(0))
#define DMA_IN_DONE_CH4_INT_ST_V 0x1
#define DMA_IN_DONE_CH4_INT_ST_S 0

#define DMA_INT_ENA_CH0_REG (DR_REG_DMA_BASE + 0x054)
/* DMA_OUTFIFO_UDF_L3_CH0_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH0_INT_ENA (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH0_INT_ENA_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH0_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH0_INT_ENA_S 17
/* DMA_OUTFIFO_OVF_L3_CH0_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH0_INT_ENA (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH0_INT_ENA_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH0_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH0_INT_ENA_S 16
/* DMA_OUTFIFO_UDF_L1_CH0_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH0_INT_ENA (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH0_INT_ENA_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH0_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH0_INT_ENA_S 15
/* DMA_OUTFIFO_OVF_L1_CH0_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH0_INT_ENA (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH0_INT_ENA_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH0_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH0_INT_ENA_S 14
/* DMA_INFIFO_UDF_L3_CH0_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH0_INT_ENA (BIT(13))
#define DMA_INFIFO_UDF_L3_CH0_INT_ENA_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH0_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L3_CH0_INT_ENA_S 13
/* DMA_INFIFO_OVF_L3_CH0_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH0_INT_ENA (BIT(12))
#define DMA_INFIFO_OVF_L3_CH0_INT_ENA_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH0_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L3_CH0_INT_ENA_S 12
/* DMA_INFIFO_UDF_L1_CH0_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH0_INT_ENA (BIT(11))
#define DMA_INFIFO_UDF_L1_CH0_INT_ENA_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH0_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L1_CH0_INT_ENA_S 11
/* DMA_INFIFO_OVF_L1_CH0_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH0_INT_ENA (BIT(10))
#define DMA_INFIFO_OVF_L1_CH0_INT_ENA_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH0_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L1_CH0_INT_ENA_S 10
/* DMA_INFIFO_FULL_WM_CH0_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH0_INT_ENA (BIT(9))
#define DMA_INFIFO_FULL_WM_CH0_INT_ENA_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH0_INT_ENA_V 0x1
#define DMA_INFIFO_FULL_WM_CH0_INT_ENA_S 9
/* DMA_OUT_TOTAL_EOF_CH0_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH0_INT_ENA (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH0_INT_ENA_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH0_INT_ENA_V 0x1
#define DMA_OUT_TOTAL_EOF_CH0_INT_ENA_S 8
/* DMA_IN_DSCR_EMPTY_CH0_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH0_INT_ENA (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH0_INT_ENA_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH0_INT_ENA_V 0x1
#define DMA_IN_DSCR_EMPTY_CH0_INT_ENA_S 7
/* DMA_OUT_DSCR_ERR_CH0_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH0_INT_ENA (BIT(6))
#define DMA_OUT_DSCR_ERR_CH0_INT_ENA_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH0_INT_ENA_V 0x1
#define DMA_OUT_DSCR_ERR_CH0_INT_ENA_S 6
/* DMA_IN_DSCR_ERR_CH0_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH0_INT_ENA (BIT(5))
#define DMA_IN_DSCR_ERR_CH0_INT_ENA_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH0_INT_ENA_V 0x1
#define DMA_IN_DSCR_ERR_CH0_INT_ENA_S 5
/* DMA_OUT_EOF_CH0_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH0_INT_ENA (BIT(4))
#define DMA_OUT_EOF_CH0_INT_ENA_M (BIT(4))
#define DMA_OUT_EOF_CH0_INT_ENA_V 0x1
#define DMA_OUT_EOF_CH0_INT_ENA_S 4
/* DMA_OUT_DONE_CH0_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH0_INT_ENA (BIT(3))
#define DMA_OUT_DONE_CH0_INT_ENA_M (BIT(3))
#define DMA_OUT_DONE_CH0_INT_ENA_V 0x1
#define DMA_OUT_DONE_CH0_INT_ENA_S 3
/* DMA_IN_ERR_EOF_CH0_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH0_INT_ENA (BIT(2))
#define DMA_IN_ERR_EOF_CH0_INT_ENA_M (BIT(2))
#define DMA_IN_ERR_EOF_CH0_INT_ENA_V 0x1
#define DMA_IN_ERR_EOF_CH0_INT_ENA_S 2
/* DMA_IN_SUC_EOF_CH0_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH0_INT_ENA (BIT(1))
#define DMA_IN_SUC_EOF_CH0_INT_ENA_M (BIT(1))
#define DMA_IN_SUC_EOF_CH0_INT_ENA_V 0x1
#define DMA_IN_SUC_EOF_CH0_INT_ENA_S 1
/* DMA_IN_DONE_CH0_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH0_INT_ENA (BIT(0))
#define DMA_IN_DONE_CH0_INT_ENA_M (BIT(0))
#define DMA_IN_DONE_CH0_INT_ENA_V 0x1
#define DMA_IN_DONE_CH0_INT_ENA_S 0

#define DMA_INT_ENA_CH1_REG (DR_REG_DMA_BASE + 0x058)
/* DMA_OUTFIFO_UDF_L3_CH1_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH1_INT_ENA (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH1_INT_ENA_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH1_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH1_INT_ENA_S 17
/* DMA_OUTFIFO_OVF_L3_CH1_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH1_INT_ENA (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH1_INT_ENA_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH1_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH1_INT_ENA_S 16
/* DMA_OUTFIFO_UDF_L1_CH1_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH1_INT_ENA (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH1_INT_ENA_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH1_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH1_INT_ENA_S 15
/* DMA_OUTFIFO_OVF_L1_CH1_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH1_INT_ENA (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH1_INT_ENA_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH1_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH1_INT_ENA_S 14
/* DMA_INFIFO_UDF_L3_CH1_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH1_INT_ENA (BIT(13))
#define DMA_INFIFO_UDF_L3_CH1_INT_ENA_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH1_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L3_CH1_INT_ENA_S 13
/* DMA_INFIFO_OVF_L3_CH1_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH1_INT_ENA (BIT(12))
#define DMA_INFIFO_OVF_L3_CH1_INT_ENA_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH1_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L3_CH1_INT_ENA_S 12
/* DMA_INFIFO_UDF_L1_CH1_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH1_INT_ENA (BIT(11))
#define DMA_INFIFO_UDF_L1_CH1_INT_ENA_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH1_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L1_CH1_INT_ENA_S 11
/* DMA_INFIFO_OVF_L1_CH1_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH1_INT_ENA (BIT(10))
#define DMA_INFIFO_OVF_L1_CH1_INT_ENA_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH1_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L1_CH1_INT_ENA_S 10
/* DMA_INFIFO_FULL_WM_CH1_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH1_INT_ENA (BIT(9))
#define DMA_INFIFO_FULL_WM_CH1_INT_ENA_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH1_INT_ENA_V 0x1
#define DMA_INFIFO_FULL_WM_CH1_INT_ENA_S 9
/* DMA_OUT_TOTAL_EOF_CH1_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH1_INT_ENA (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH1_INT_ENA_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH1_INT_ENA_V 0x1
#define DMA_OUT_TOTAL_EOF_CH1_INT_ENA_S 8
/* DMA_IN_DSCR_EMPTY_CH1_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH1_INT_ENA (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH1_INT_ENA_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH1_INT_ENA_V 0x1
#define DMA_IN_DSCR_EMPTY_CH1_INT_ENA_S 7
/* DMA_OUT_DSCR_ERR_CH1_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH1_INT_ENA (BIT(6))
#define DMA_OUT_DSCR_ERR_CH1_INT_ENA_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH1_INT_ENA_V 0x1
#define DMA_OUT_DSCR_ERR_CH1_INT_ENA_S 6
/* DMA_IN_DSCR_ERR_CH1_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH1_INT_ENA (BIT(5))
#define DMA_IN_DSCR_ERR_CH1_INT_ENA_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH1_INT_ENA_V 0x1
#define DMA_IN_DSCR_ERR_CH1_INT_ENA_S 5
/* DMA_OUT_EOF_CH1_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH1_INT_ENA (BIT(4))
#define DMA_OUT_EOF_CH1_INT_ENA_M (BIT(4))
#define DMA_OUT_EOF_CH1_INT_ENA_V 0x1
#define DMA_OUT_EOF_CH1_INT_ENA_S 4
/* DMA_OUT_DONE_CH1_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH1_INT_ENA (BIT(3))
#define DMA_OUT_DONE_CH1_INT_ENA_M (BIT(3))
#define DMA_OUT_DONE_CH1_INT_ENA_V 0x1
#define DMA_OUT_DONE_CH1_INT_ENA_S 3
/* DMA_IN_ERR_EOF_CH1_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH1_INT_ENA (BIT(2))
#define DMA_IN_ERR_EOF_CH1_INT_ENA_M (BIT(2))
#define DMA_IN_ERR_EOF_CH1_INT_ENA_V 0x1
#define DMA_IN_ERR_EOF_CH1_INT_ENA_S 2
/* DMA_IN_SUC_EOF_CH1_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH1_INT_ENA (BIT(1))
#define DMA_IN_SUC_EOF_CH1_INT_ENA_M (BIT(1))
#define DMA_IN_SUC_EOF_CH1_INT_ENA_V 0x1
#define DMA_IN_SUC_EOF_CH1_INT_ENA_S 1
/* DMA_IN_DONE_CH1_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH1_INT_ENA (BIT(0))
#define DMA_IN_DONE_CH1_INT_ENA_M (BIT(0))
#define DMA_IN_DONE_CH1_INT_ENA_V 0x1
#define DMA_IN_DONE_CH1_INT_ENA_S 0

#define DMA_INT_ENA_CH2_REG (DR_REG_DMA_BASE + 0x05C)
/* DMA_OUTFIFO_UDF_L3_CH2_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH2_INT_ENA (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH2_INT_ENA_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH2_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH2_INT_ENA_S 17
/* DMA_OUTFIFO_OVF_L3_CH2_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH2_INT_ENA (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH2_INT_ENA_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH2_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH2_INT_ENA_S 16
/* DMA_OUTFIFO_UDF_L1_CH2_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH2_INT_ENA (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH2_INT_ENA_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH2_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH2_INT_ENA_S 15
/* DMA_OUTFIFO_OVF_L1_CH2_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH2_INT_ENA (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH2_INT_ENA_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH2_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH2_INT_ENA_S 14
/* DMA_INFIFO_UDF_L3_CH2_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH2_INT_ENA (BIT(13))
#define DMA_INFIFO_UDF_L3_CH2_INT_ENA_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH2_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L3_CH2_INT_ENA_S 13
/* DMA_INFIFO_OVF_L3_CH2_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH2_INT_ENA (BIT(12))
#define DMA_INFIFO_OVF_L3_CH2_INT_ENA_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH2_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L3_CH2_INT_ENA_S 12
/* DMA_INFIFO_UDF_L1_CH2_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH2_INT_ENA (BIT(11))
#define DMA_INFIFO_UDF_L1_CH2_INT_ENA_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH2_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L1_CH2_INT_ENA_S 11
/* DMA_INFIFO_OVF_L1_CH2_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH2_INT_ENA (BIT(10))
#define DMA_INFIFO_OVF_L1_CH2_INT_ENA_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH2_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L1_CH2_INT_ENA_S 10
/* DMA_INFIFO_FULL_WM_CH2_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH2_INT_ENA (BIT(9))
#define DMA_INFIFO_FULL_WM_CH2_INT_ENA_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH2_INT_ENA_V 0x1
#define DMA_INFIFO_FULL_WM_CH2_INT_ENA_S 9
/* DMA_OUT_TOTAL_EOF_CH2_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH2_INT_ENA (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH2_INT_ENA_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH2_INT_ENA_V 0x1
#define DMA_OUT_TOTAL_EOF_CH2_INT_ENA_S 8
/* DMA_IN_DSCR_EMPTY_CH2_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH2_INT_ENA (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH2_INT_ENA_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH2_INT_ENA_V 0x1
#define DMA_IN_DSCR_EMPTY_CH2_INT_ENA_S 7
/* DMA_OUT_DSCR_ERR_CH2_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH2_INT_ENA (BIT(6))
#define DMA_OUT_DSCR_ERR_CH2_INT_ENA_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH2_INT_ENA_V 0x1
#define DMA_OUT_DSCR_ERR_CH2_INT_ENA_S 6
/* DMA_IN_DSCR_ERR_CH2_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH2_INT_ENA (BIT(5))
#define DMA_IN_DSCR_ERR_CH2_INT_ENA_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH2_INT_ENA_V 0x1
#define DMA_IN_DSCR_ERR_CH2_INT_ENA_S 5
/* DMA_OUT_EOF_CH2_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH2_INT_ENA (BIT(4))
#define DMA_OUT_EOF_CH2_INT_ENA_M (BIT(4))
#define DMA_OUT_EOF_CH2_INT_ENA_V 0x1
#define DMA_OUT_EOF_CH2_INT_ENA_S 4
/* DMA_OUT_DONE_CH2_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH2_INT_ENA (BIT(3))
#define DMA_OUT_DONE_CH2_INT_ENA_M (BIT(3))
#define DMA_OUT_DONE_CH2_INT_ENA_V 0x1
#define DMA_OUT_DONE_CH2_INT_ENA_S 3
/* DMA_IN_ERR_EOF_CH2_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH2_INT_ENA (BIT(2))
#define DMA_IN_ERR_EOF_CH2_INT_ENA_M (BIT(2))
#define DMA_IN_ERR_EOF_CH2_INT_ENA_V 0x1
#define DMA_IN_ERR_EOF_CH2_INT_ENA_S 2
/* DMA_IN_SUC_EOF_CH2_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH2_INT_ENA (BIT(1))
#define DMA_IN_SUC_EOF_CH2_INT_ENA_M (BIT(1))
#define DMA_IN_SUC_EOF_CH2_INT_ENA_V 0x1
#define DMA_IN_SUC_EOF_CH2_INT_ENA_S 1
/* DMA_IN_DONE_CH2_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH2_INT_ENA (BIT(0))
#define DMA_IN_DONE_CH2_INT_ENA_M (BIT(0))
#define DMA_IN_DONE_CH2_INT_ENA_V 0x1
#define DMA_IN_DONE_CH2_INT_ENA_S 0

#define DMA_INT_ENA_CH3_REG (DR_REG_DMA_BASE + 0x060)
/* DMA_OUTFIFO_UDF_L3_CH3_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH3_INT_ENA (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH3_INT_ENA_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH3_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH3_INT_ENA_S 17
/* DMA_OUTFIFO_OVF_L3_CH3_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH3_INT_ENA (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH3_INT_ENA_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH3_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH3_INT_ENA_S 16
/* DMA_OUTFIFO_UDF_L1_CH3_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH3_INT_ENA (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH3_INT_ENA_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH3_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH3_INT_ENA_S 15
/* DMA_OUTFIFO_OVF_L1_CH3_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH3_INT_ENA (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH3_INT_ENA_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH3_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH3_INT_ENA_S 14
/* DMA_INFIFO_UDF_L3_CH3_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH3_INT_ENA (BIT(13))
#define DMA_INFIFO_UDF_L3_CH3_INT_ENA_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH3_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L3_CH3_INT_ENA_S 13
/* DMA_INFIFO_OVF_L3_CH3_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH3_INT_ENA (BIT(12))
#define DMA_INFIFO_OVF_L3_CH3_INT_ENA_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH3_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L3_CH3_INT_ENA_S 12
/* DMA_INFIFO_UDF_L1_CH3_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH3_INT_ENA (BIT(11))
#define DMA_INFIFO_UDF_L1_CH3_INT_ENA_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH3_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L1_CH3_INT_ENA_S 11
/* DMA_INFIFO_OVF_L1_CH3_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH3_INT_ENA (BIT(10))
#define DMA_INFIFO_OVF_L1_CH3_INT_ENA_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH3_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L1_CH3_INT_ENA_S 10
/* DMA_INFIFO_FULL_WM_CH3_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH3_INT_ENA (BIT(9))
#define DMA_INFIFO_FULL_WM_CH3_INT_ENA_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH3_INT_ENA_V 0x1
#define DMA_INFIFO_FULL_WM_CH3_INT_ENA_S 9
/* DMA_OUT_TOTAL_EOF_CH3_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH3_INT_ENA (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH3_INT_ENA_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH3_INT_ENA_V 0x1
#define DMA_OUT_TOTAL_EOF_CH3_INT_ENA_S 8
/* DMA_IN_DSCR_EMPTY_CH3_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH3_INT_ENA (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH3_INT_ENA_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH3_INT_ENA_V 0x1
#define DMA_IN_DSCR_EMPTY_CH3_INT_ENA_S 7
/* DMA_OUT_DSCR_ERR_CH3_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH3_INT_ENA (BIT(6))
#define DMA_OUT_DSCR_ERR_CH3_INT_ENA_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH3_INT_ENA_V 0x1
#define DMA_OUT_DSCR_ERR_CH3_INT_ENA_S 6
/* DMA_IN_DSCR_ERR_CH3_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH3_INT_ENA (BIT(5))
#define DMA_IN_DSCR_ERR_CH3_INT_ENA_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH3_INT_ENA_V 0x1
#define DMA_IN_DSCR_ERR_CH3_INT_ENA_S 5
/* DMA_OUT_EOF_CH3_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH3_INT_ENA (BIT(4))
#define DMA_OUT_EOF_CH3_INT_ENA_M (BIT(4))
#define DMA_OUT_EOF_CH3_INT_ENA_V 0x1
#define DMA_OUT_EOF_CH3_INT_ENA_S 4
/* DMA_OUT_DONE_CH3_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH3_INT_ENA (BIT(3))
#define DMA_OUT_DONE_CH3_INT_ENA_M (BIT(3))
#define DMA_OUT_DONE_CH3_INT_ENA_V 0x1
#define DMA_OUT_DONE_CH3_INT_ENA_S 3
/* DMA_IN_ERR_EOF_CH3_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH3_INT_ENA (BIT(2))
#define DMA_IN_ERR_EOF_CH3_INT_ENA_M (BIT(2))
#define DMA_IN_ERR_EOF_CH3_INT_ENA_V 0x1
#define DMA_IN_ERR_EOF_CH3_INT_ENA_S 2
/* DMA_IN_SUC_EOF_CH3_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH3_INT_ENA (BIT(1))
#define DMA_IN_SUC_EOF_CH3_INT_ENA_M (BIT(1))
#define DMA_IN_SUC_EOF_CH3_INT_ENA_V 0x1
#define DMA_IN_SUC_EOF_CH3_INT_ENA_S 1
/* DMA_IN_DONE_CH3_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH3_INT_ENA (BIT(0))
#define DMA_IN_DONE_CH3_INT_ENA_M (BIT(0))
#define DMA_IN_DONE_CH3_INT_ENA_V 0x1
#define DMA_IN_DONE_CH3_INT_ENA_S 0

#define DMA_INT_ENA_CH4_REG (DR_REG_DMA_BASE + 0x064)
/* DMA_OUTFIFO_UDF_L3_CH4_INT_ENA : R/W ;bitpos:[17] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH4_INT_ENA (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH4_INT_ENA_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH4_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH4_INT_ENA_S 17
/* DMA_OUTFIFO_OVF_L3_CH4_INT_ENA : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH4_INT_ENA (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH4_INT_ENA_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH4_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH4_INT_ENA_S 16
/* DMA_OUTFIFO_UDF_L1_CH4_INT_ENA : R/W ;bitpos:[15] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH4_INT_ENA (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH4_INT_ENA_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH4_INT_ENA_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH4_INT_ENA_S 15
/* DMA_OUTFIFO_OVF_L1_CH4_INT_ENA : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH4_INT_ENA (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH4_INT_ENA_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH4_INT_ENA_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH4_INT_ENA_S 14
/* DMA_INFIFO_UDF_L3_CH4_INT_ENA : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH4_INT_ENA (BIT(13))
#define DMA_INFIFO_UDF_L3_CH4_INT_ENA_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH4_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L3_CH4_INT_ENA_S 13
/* DMA_INFIFO_OVF_L3_CH4_INT_ENA : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH4_INT_ENA (BIT(12))
#define DMA_INFIFO_OVF_L3_CH4_INT_ENA_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH4_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L3_CH4_INT_ENA_S 12
/* DMA_INFIFO_UDF_L1_CH4_INT_ENA : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH4_INT_ENA (BIT(11))
#define DMA_INFIFO_UDF_L1_CH4_INT_ENA_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH4_INT_ENA_V 0x1
#define DMA_INFIFO_UDF_L1_CH4_INT_ENA_S 11
/* DMA_INFIFO_OVF_L1_CH4_INT_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH4_INT_ENA (BIT(10))
#define DMA_INFIFO_OVF_L1_CH4_INT_ENA_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH4_INT_ENA_V 0x1
#define DMA_INFIFO_OVF_L1_CH4_INT_ENA_S 10
/* DMA_INFIFO_FULL_WM_CH4_INT_ENA : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH4_INT_ENA (BIT(9))
#define DMA_INFIFO_FULL_WM_CH4_INT_ENA_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH4_INT_ENA_V 0x1
#define DMA_INFIFO_FULL_WM_CH4_INT_ENA_S 9
/* DMA_OUT_TOTAL_EOF_CH4_INT_ENA : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH4_INT_ENA (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH4_INT_ENA_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH4_INT_ENA_V 0x1
#define DMA_OUT_TOTAL_EOF_CH4_INT_ENA_S 8
/* DMA_IN_DSCR_EMPTY_CH4_INT_ENA : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH4_INT_ENA (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH4_INT_ENA_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH4_INT_ENA_V 0x1
#define DMA_IN_DSCR_EMPTY_CH4_INT_ENA_S 7
/* DMA_OUT_DSCR_ERR_CH4_INT_ENA : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH4_INT_ENA (BIT(6))
#define DMA_OUT_DSCR_ERR_CH4_INT_ENA_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH4_INT_ENA_V 0x1
#define DMA_OUT_DSCR_ERR_CH4_INT_ENA_S 6
/* DMA_IN_DSCR_ERR_CH4_INT_ENA : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH4_INT_ENA (BIT(5))
#define DMA_IN_DSCR_ERR_CH4_INT_ENA_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH4_INT_ENA_V 0x1
#define DMA_IN_DSCR_ERR_CH4_INT_ENA_S 5
/* DMA_OUT_EOF_CH4_INT_ENA : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH4_INT_ENA (BIT(4))
#define DMA_OUT_EOF_CH4_INT_ENA_M (BIT(4))
#define DMA_OUT_EOF_CH4_INT_ENA_V 0x1
#define DMA_OUT_EOF_CH4_INT_ENA_S 4
/* DMA_OUT_DONE_CH4_INT_ENA : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH4_INT_ENA (BIT(3))
#define DMA_OUT_DONE_CH4_INT_ENA_M (BIT(3))
#define DMA_OUT_DONE_CH4_INT_ENA_V 0x1
#define DMA_OUT_DONE_CH4_INT_ENA_S 3
/* DMA_IN_ERR_EOF_CH4_INT_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH4_INT_ENA (BIT(2))
#define DMA_IN_ERR_EOF_CH4_INT_ENA_M (BIT(2))
#define DMA_IN_ERR_EOF_CH4_INT_ENA_V 0x1
#define DMA_IN_ERR_EOF_CH4_INT_ENA_S 2
/* DMA_IN_SUC_EOF_CH4_INT_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH4_INT_ENA (BIT(1))
#define DMA_IN_SUC_EOF_CH4_INT_ENA_M (BIT(1))
#define DMA_IN_SUC_EOF_CH4_INT_ENA_V 0x1
#define DMA_IN_SUC_EOF_CH4_INT_ENA_S 1
/* DMA_IN_DONE_CH4_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH4_INT_ENA (BIT(0))
#define DMA_IN_DONE_CH4_INT_ENA_M (BIT(0))
#define DMA_IN_DONE_CH4_INT_ENA_V 0x1
#define DMA_IN_DONE_CH4_INT_ENA_S 0

#define DMA_INT_CLR_CH0_REG (DR_REG_DMA_BASE + 0x068)
/* DMA_OUTFIFO_UDF_L3_CH0_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH0_INT_CLR (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH0_INT_CLR_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH0_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH0_INT_CLR_S 17
/* DMA_OUTFIFO_OVF_L3_CH0_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH0_INT_CLR (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH0_INT_CLR_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH0_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH0_INT_CLR_S 16
/* DMA_OUTFIFO_UDF_L1_CH0_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH0_INT_CLR (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH0_INT_CLR_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH0_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH0_INT_CLR_S 15
/* DMA_OUTFIFO_OVF_L1_CH0_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH0_INT_CLR (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH0_INT_CLR_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH0_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH0_INT_CLR_S 14
/* DMA_INFIFO_UDF_L3_CH0_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH0_INT_CLR (BIT(13))
#define DMA_INFIFO_UDF_L3_CH0_INT_CLR_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH0_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L3_CH0_INT_CLR_S 13
/* DMA_INFIFO_OVF_L3_CH0_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH0_INT_CLR (BIT(12))
#define DMA_INFIFO_OVF_L3_CH0_INT_CLR_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH0_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L3_CH0_INT_CLR_S 12
/* DMA_INFIFO_UDF_L1_CH0_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH0_INT_CLR (BIT(11))
#define DMA_INFIFO_UDF_L1_CH0_INT_CLR_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH0_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L1_CH0_INT_CLR_S 11
/* DMA_INFIFO_OVF_L1_CH0_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH0_INT_CLR (BIT(10))
#define DMA_INFIFO_OVF_L1_CH0_INT_CLR_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH0_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L1_CH0_INT_CLR_S 10
/* DMA_INFIFO_FULL_WM_CH0_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH0_INT_CLR (BIT(9))
#define DMA_INFIFO_FULL_WM_CH0_INT_CLR_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH0_INT_CLR_V 0x1
#define DMA_INFIFO_FULL_WM_CH0_INT_CLR_S 9
/* DMA_OUT_TOTAL_EOF_CH0_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH0_INT_CLR (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH0_INT_CLR_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH0_INT_CLR_V 0x1
#define DMA_OUT_TOTAL_EOF_CH0_INT_CLR_S 8
/* DMA_IN_DSCR_EMPTY_CH0_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH0_INT_CLR (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH0_INT_CLR_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH0_INT_CLR_V 0x1
#define DMA_IN_DSCR_EMPTY_CH0_INT_CLR_S 7
/* DMA_OUT_DSCR_ERR_CH0_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH0_INT_CLR (BIT(6))
#define DMA_OUT_DSCR_ERR_CH0_INT_CLR_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH0_INT_CLR_V 0x1
#define DMA_OUT_DSCR_ERR_CH0_INT_CLR_S 6
/* DMA_IN_DSCR_ERR_CH0_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH0_INT_CLR (BIT(5))
#define DMA_IN_DSCR_ERR_CH0_INT_CLR_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH0_INT_CLR_V 0x1
#define DMA_IN_DSCR_ERR_CH0_INT_CLR_S 5
/* DMA_OUT_EOF_CH0_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH0_INT_CLR (BIT(4))
#define DMA_OUT_EOF_CH0_INT_CLR_M (BIT(4))
#define DMA_OUT_EOF_CH0_INT_CLR_V 0x1
#define DMA_OUT_EOF_CH0_INT_CLR_S 4
/* DMA_OUT_DONE_CH0_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH0_INT_CLR (BIT(3))
#define DMA_OUT_DONE_CH0_INT_CLR_M (BIT(3))
#define DMA_OUT_DONE_CH0_INT_CLR_V 0x1
#define DMA_OUT_DONE_CH0_INT_CLR_S 3
/* DMA_IN_ERR_EOF_CH0_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH0_INT_CLR (BIT(2))
#define DMA_IN_ERR_EOF_CH0_INT_CLR_M (BIT(2))
#define DMA_IN_ERR_EOF_CH0_INT_CLR_V 0x1
#define DMA_IN_ERR_EOF_CH0_INT_CLR_S 2
/* DMA_IN_SUC_EOF_CH0_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH0_INT_CLR (BIT(1))
#define DMA_IN_SUC_EOF_CH0_INT_CLR_M (BIT(1))
#define DMA_IN_SUC_EOF_CH0_INT_CLR_V 0x1
#define DMA_IN_SUC_EOF_CH0_INT_CLR_S 1
/* DMA_IN_DONE_CH0_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH0_INT_CLR (BIT(0))
#define DMA_IN_DONE_CH0_INT_CLR_M (BIT(0))
#define DMA_IN_DONE_CH0_INT_CLR_V 0x1
#define DMA_IN_DONE_CH0_INT_CLR_S 0

#define DMA_INT_CLR_CH1_REG (DR_REG_DMA_BASE + 0x06C)
/* DMA_OUTFIFO_UDF_L3_CH1_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH1_INT_CLR (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH1_INT_CLR_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH1_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH1_INT_CLR_S 17
/* DMA_OUTFIFO_OVF_L3_CH1_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH1_INT_CLR (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH1_INT_CLR_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH1_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH1_INT_CLR_S 16
/* DMA_OUTFIFO_UDF_L1_CH1_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH1_INT_CLR (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH1_INT_CLR_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH1_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH1_INT_CLR_S 15
/* DMA_OUTFIFO_OVF_L1_CH1_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH1_INT_CLR (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH1_INT_CLR_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH1_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH1_INT_CLR_S 14
/* DMA_INFIFO_UDF_L3_CH1_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH1_INT_CLR (BIT(13))
#define DMA_INFIFO_UDF_L3_CH1_INT_CLR_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH1_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L3_CH1_INT_CLR_S 13
/* DMA_INFIFO_OVF_L3_CH1_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH1_INT_CLR (BIT(12))
#define DMA_INFIFO_OVF_L3_CH1_INT_CLR_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH1_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L3_CH1_INT_CLR_S 12
/* DMA_INFIFO_UDF_L1_CH1_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH1_INT_CLR (BIT(11))
#define DMA_INFIFO_UDF_L1_CH1_INT_CLR_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH1_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L1_CH1_INT_CLR_S 11
/* DMA_INFIFO_OVF_L1_CH1_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH1_INT_CLR (BIT(10))
#define DMA_INFIFO_OVF_L1_CH1_INT_CLR_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH1_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L1_CH1_INT_CLR_S 10
/* DMA_INFIFO_FULL_WM_CH1_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH1_INT_CLR (BIT(9))
#define DMA_INFIFO_FULL_WM_CH1_INT_CLR_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH1_INT_CLR_V 0x1
#define DMA_INFIFO_FULL_WM_CH1_INT_CLR_S 9
/* DMA_OUT_TOTAL_EOF_CH1_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH1_INT_CLR (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH1_INT_CLR_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH1_INT_CLR_V 0x1
#define DMA_OUT_TOTAL_EOF_CH1_INT_CLR_S 8
/* DMA_IN_DSCR_EMPTY_CH1_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH1_INT_CLR (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH1_INT_CLR_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH1_INT_CLR_V 0x1
#define DMA_IN_DSCR_EMPTY_CH1_INT_CLR_S 7
/* DMA_OUT_DSCR_ERR_CH1_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH1_INT_CLR (BIT(6))
#define DMA_OUT_DSCR_ERR_CH1_INT_CLR_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH1_INT_CLR_V 0x1
#define DMA_OUT_DSCR_ERR_CH1_INT_CLR_S 6
/* DMA_IN_DSCR_ERR_CH1_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH1_INT_CLR (BIT(5))
#define DMA_IN_DSCR_ERR_CH1_INT_CLR_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH1_INT_CLR_V 0x1
#define DMA_IN_DSCR_ERR_CH1_INT_CLR_S 5
/* DMA_OUT_EOF_CH1_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH1_INT_CLR (BIT(4))
#define DMA_OUT_EOF_CH1_INT_CLR_M (BIT(4))
#define DMA_OUT_EOF_CH1_INT_CLR_V 0x1
#define DMA_OUT_EOF_CH1_INT_CLR_S 4
/* DMA_OUT_DONE_CH1_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH1_INT_CLR (BIT(3))
#define DMA_OUT_DONE_CH1_INT_CLR_M (BIT(3))
#define DMA_OUT_DONE_CH1_INT_CLR_V 0x1
#define DMA_OUT_DONE_CH1_INT_CLR_S 3
/* DMA_IN_ERR_EOF_CH1_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH1_INT_CLR (BIT(2))
#define DMA_IN_ERR_EOF_CH1_INT_CLR_M (BIT(2))
#define DMA_IN_ERR_EOF_CH1_INT_CLR_V 0x1
#define DMA_IN_ERR_EOF_CH1_INT_CLR_S 2
/* DMA_IN_SUC_EOF_CH1_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH1_INT_CLR (BIT(1))
#define DMA_IN_SUC_EOF_CH1_INT_CLR_M (BIT(1))
#define DMA_IN_SUC_EOF_CH1_INT_CLR_V 0x1
#define DMA_IN_SUC_EOF_CH1_INT_CLR_S 1
/* DMA_IN_DONE_CH1_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH1_INT_CLR (BIT(0))
#define DMA_IN_DONE_CH1_INT_CLR_M (BIT(0))
#define DMA_IN_DONE_CH1_INT_CLR_V 0x1
#define DMA_IN_DONE_CH1_INT_CLR_S 0

#define DMA_INT_CLR_CH2_REG (DR_REG_DMA_BASE + 0x070)
/* DMA_OUTFIFO_UDF_L3_CH2_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH2_INT_CLR (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH2_INT_CLR_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH2_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH2_INT_CLR_S 17
/* DMA_OUTFIFO_OVF_L3_CH2_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH2_INT_CLR (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH2_INT_CLR_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH2_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH2_INT_CLR_S 16
/* DMA_OUTFIFO_UDF_L1_CH2_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH2_INT_CLR (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH2_INT_CLR_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH2_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH2_INT_CLR_S 15
/* DMA_OUTFIFO_OVF_L1_CH2_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH2_INT_CLR (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH2_INT_CLR_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH2_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH2_INT_CLR_S 14
/* DMA_INFIFO_UDF_L3_CH2_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH2_INT_CLR (BIT(13))
#define DMA_INFIFO_UDF_L3_CH2_INT_CLR_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH2_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L3_CH2_INT_CLR_S 13
/* DMA_INFIFO_OVF_L3_CH2_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH2_INT_CLR (BIT(12))
#define DMA_INFIFO_OVF_L3_CH2_INT_CLR_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH2_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L3_CH2_INT_CLR_S 12
/* DMA_INFIFO_UDF_L1_CH2_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH2_INT_CLR (BIT(11))
#define DMA_INFIFO_UDF_L1_CH2_INT_CLR_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH2_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L1_CH2_INT_CLR_S 11
/* DMA_INFIFO_OVF_L1_CH2_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH2_INT_CLR (BIT(10))
#define DMA_INFIFO_OVF_L1_CH2_INT_CLR_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH2_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L1_CH2_INT_CLR_S 10
/* DMA_INFIFO_FULL_WM_CH2_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH2_INT_CLR (BIT(9))
#define DMA_INFIFO_FULL_WM_CH2_INT_CLR_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH2_INT_CLR_V 0x1
#define DMA_INFIFO_FULL_WM_CH2_INT_CLR_S 9
/* DMA_OUT_TOTAL_EOF_CH2_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH2_INT_CLR (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH2_INT_CLR_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH2_INT_CLR_V 0x1
#define DMA_OUT_TOTAL_EOF_CH2_INT_CLR_S 8
/* DMA_IN_DSCR_EMPTY_CH2_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH2_INT_CLR (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH2_INT_CLR_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH2_INT_CLR_V 0x1
#define DMA_IN_DSCR_EMPTY_CH2_INT_CLR_S 7
/* DMA_OUT_DSCR_ERR_CH2_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH2_INT_CLR (BIT(6))
#define DMA_OUT_DSCR_ERR_CH2_INT_CLR_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH2_INT_CLR_V 0x1
#define DMA_OUT_DSCR_ERR_CH2_INT_CLR_S 6
/* DMA_IN_DSCR_ERR_CH2_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH2_INT_CLR (BIT(5))
#define DMA_IN_DSCR_ERR_CH2_INT_CLR_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH2_INT_CLR_V 0x1
#define DMA_IN_DSCR_ERR_CH2_INT_CLR_S 5
/* DMA_OUT_EOF_CH2_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH2_INT_CLR (BIT(4))
#define DMA_OUT_EOF_CH2_INT_CLR_M (BIT(4))
#define DMA_OUT_EOF_CH2_INT_CLR_V 0x1
#define DMA_OUT_EOF_CH2_INT_CLR_S 4
/* DMA_OUT_DONE_CH2_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH2_INT_CLR (BIT(3))
#define DMA_OUT_DONE_CH2_INT_CLR_M (BIT(3))
#define DMA_OUT_DONE_CH2_INT_CLR_V 0x1
#define DMA_OUT_DONE_CH2_INT_CLR_S 3
/* DMA_IN_ERR_EOF_CH2_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH2_INT_CLR (BIT(2))
#define DMA_IN_ERR_EOF_CH2_INT_CLR_M (BIT(2))
#define DMA_IN_ERR_EOF_CH2_INT_CLR_V 0x1
#define DMA_IN_ERR_EOF_CH2_INT_CLR_S 2
/* DMA_IN_SUC_EOF_CH2_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH2_INT_CLR (BIT(1))
#define DMA_IN_SUC_EOF_CH2_INT_CLR_M (BIT(1))
#define DMA_IN_SUC_EOF_CH2_INT_CLR_V 0x1
#define DMA_IN_SUC_EOF_CH2_INT_CLR_S 1
/* DMA_IN_DONE_CH2_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH2_INT_CLR (BIT(0))
#define DMA_IN_DONE_CH2_INT_CLR_M (BIT(0))
#define DMA_IN_DONE_CH2_INT_CLR_V 0x1
#define DMA_IN_DONE_CH2_INT_CLR_S 0

#define DMA_INT_CLR_CH3_REG (DR_REG_DMA_BASE + 0x074)
/* DMA_OUTFIFO_UDF_L3_CH3_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH3_INT_CLR (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH3_INT_CLR_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH3_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH3_INT_CLR_S 17
/* DMA_OUTFIFO_OVF_L3_CH3_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH3_INT_CLR (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH3_INT_CLR_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH3_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH3_INT_CLR_S 16
/* DMA_OUTFIFO_UDF_L1_CH3_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH3_INT_CLR (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH3_INT_CLR_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH3_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH3_INT_CLR_S 15
/* DMA_OUTFIFO_OVF_L1_CH3_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH3_INT_CLR (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH3_INT_CLR_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH3_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH3_INT_CLR_S 14
/* DMA_INFIFO_UDF_L3_CH3_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH3_INT_CLR (BIT(13))
#define DMA_INFIFO_UDF_L3_CH3_INT_CLR_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH3_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L3_CH3_INT_CLR_S 13
/* DMA_INFIFO_OVF_L3_CH3_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH3_INT_CLR (BIT(12))
#define DMA_INFIFO_OVF_L3_CH3_INT_CLR_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH3_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L3_CH3_INT_CLR_S 12
/* DMA_INFIFO_UDF_L1_CH3_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH3_INT_CLR (BIT(11))
#define DMA_INFIFO_UDF_L1_CH3_INT_CLR_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH3_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L1_CH3_INT_CLR_S 11
/* DMA_INFIFO_OVF_L1_CH3_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH3_INT_CLR (BIT(10))
#define DMA_INFIFO_OVF_L1_CH3_INT_CLR_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH3_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L1_CH3_INT_CLR_S 10
/* DMA_INFIFO_FULL_WM_CH3_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH3_INT_CLR (BIT(9))
#define DMA_INFIFO_FULL_WM_CH3_INT_CLR_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH3_INT_CLR_V 0x1
#define DMA_INFIFO_FULL_WM_CH3_INT_CLR_S 9
/* DMA_OUT_TOTAL_EOF_CH3_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH3_INT_CLR (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH3_INT_CLR_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH3_INT_CLR_V 0x1
#define DMA_OUT_TOTAL_EOF_CH3_INT_CLR_S 8
/* DMA_IN_DSCR_EMPTY_CH3_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH3_INT_CLR (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH3_INT_CLR_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH3_INT_CLR_V 0x1
#define DMA_IN_DSCR_EMPTY_CH3_INT_CLR_S 7
/* DMA_OUT_DSCR_ERR_CH3_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH3_INT_CLR (BIT(6))
#define DMA_OUT_DSCR_ERR_CH3_INT_CLR_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH3_INT_CLR_V 0x1
#define DMA_OUT_DSCR_ERR_CH3_INT_CLR_S 6
/* DMA_IN_DSCR_ERR_CH3_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH3_INT_CLR (BIT(5))
#define DMA_IN_DSCR_ERR_CH3_INT_CLR_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH3_INT_CLR_V 0x1
#define DMA_IN_DSCR_ERR_CH3_INT_CLR_S 5
/* DMA_OUT_EOF_CH3_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH3_INT_CLR (BIT(4))
#define DMA_OUT_EOF_CH3_INT_CLR_M (BIT(4))
#define DMA_OUT_EOF_CH3_INT_CLR_V 0x1
#define DMA_OUT_EOF_CH3_INT_CLR_S 4
/* DMA_OUT_DONE_CH3_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH3_INT_CLR (BIT(3))
#define DMA_OUT_DONE_CH3_INT_CLR_M (BIT(3))
#define DMA_OUT_DONE_CH3_INT_CLR_V 0x1
#define DMA_OUT_DONE_CH3_INT_CLR_S 3
/* DMA_IN_ERR_EOF_CH3_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH3_INT_CLR (BIT(2))
#define DMA_IN_ERR_EOF_CH3_INT_CLR_M (BIT(2))
#define DMA_IN_ERR_EOF_CH3_INT_CLR_V 0x1
#define DMA_IN_ERR_EOF_CH3_INT_CLR_S 2
/* DMA_IN_SUC_EOF_CH3_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH3_INT_CLR (BIT(1))
#define DMA_IN_SUC_EOF_CH3_INT_CLR_M (BIT(1))
#define DMA_IN_SUC_EOF_CH3_INT_CLR_V 0x1
#define DMA_IN_SUC_EOF_CH3_INT_CLR_S 1
/* DMA_IN_DONE_CH3_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH3_INT_CLR (BIT(0))
#define DMA_IN_DONE_CH3_INT_CLR_M (BIT(0))
#define DMA_IN_DONE_CH3_INT_CLR_V 0x1
#define DMA_IN_DONE_CH3_INT_CLR_S 0

#define DMA_INT_CLR_CH4_REG (DR_REG_DMA_BASE + 0x078)
/* DMA_OUTFIFO_UDF_L3_CH4_INT_CLR : WO ;bitpos:[17] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L3_CH4_INT_CLR (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH4_INT_CLR_M (BIT(17))
#define DMA_OUTFIFO_UDF_L3_CH4_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L3_CH4_INT_CLR_S 17
/* DMA_OUTFIFO_OVF_L3_CH4_INT_CLR : WO ;bitpos:[16] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L3_CH4_INT_CLR (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH4_INT_CLR_M (BIT(16))
#define DMA_OUTFIFO_OVF_L3_CH4_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L3_CH4_INT_CLR_S 16
/* DMA_OUTFIFO_UDF_L1_CH4_INT_CLR : WO ;bitpos:[15] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_UDF_L1_CH4_INT_CLR (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH4_INT_CLR_M (BIT(15))
#define DMA_OUTFIFO_UDF_L1_CH4_INT_CLR_V 0x1
#define DMA_OUTFIFO_UDF_L1_CH4_INT_CLR_S 15
/* DMA_OUTFIFO_OVF_L1_CH4_INT_CLR : WO ;bitpos:[14] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_OUTFIFO_OVF_L1_CH4_INT_CLR (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH4_INT_CLR_M (BIT(14))
#define DMA_OUTFIFO_OVF_L1_CH4_INT_CLR_V 0x1
#define DMA_OUTFIFO_OVF_L1_CH4_INT_CLR_S 14
/* DMA_INFIFO_UDF_L3_CH4_INT_CLR : WO ;bitpos:[13] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L3_CH4_INT_CLR (BIT(13))
#define DMA_INFIFO_UDF_L3_CH4_INT_CLR_M (BIT(13))
#define DMA_INFIFO_UDF_L3_CH4_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L3_CH4_INT_CLR_S 13
/* DMA_INFIFO_OVF_L3_CH4_INT_CLR : WO ;bitpos:[12] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L3_CH4_INT_CLR (BIT(12))
#define DMA_INFIFO_OVF_L3_CH4_INT_CLR_M (BIT(12))
#define DMA_INFIFO_OVF_L3_CH4_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L3_CH4_INT_CLR_S 12
/* DMA_INFIFO_UDF_L1_CH4_INT_CLR : WO ;bitpos:[11] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_UDF_L1_CH4_INT_CLR (BIT(11))
#define DMA_INFIFO_UDF_L1_CH4_INT_CLR_M (BIT(11))
#define DMA_INFIFO_UDF_L1_CH4_INT_CLR_V 0x1
#define DMA_INFIFO_UDF_L1_CH4_INT_CLR_S 11
/* DMA_INFIFO_OVF_L1_CH4_INT_CLR : WO ;bitpos:[10] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
#define DMA_INFIFO_OVF_L1_CH4_INT_CLR (BIT(10))
#define DMA_INFIFO_OVF_L1_CH4_INT_CLR_M (BIT(10))
#define DMA_INFIFO_OVF_L1_CH4_INT_CLR_V 0x1
#define DMA_INFIFO_OVF_L1_CH4_INT_CLR_S 10
/* DMA_INFIFO_FULL_WM_CH4_INT_CLR : WO ;bitpos:[9] ;default: 1'b0 ; */
/*description: Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
#define DMA_INFIFO_FULL_WM_CH4_INT_CLR (BIT(9))
#define DMA_INFIFO_FULL_WM_CH4_INT_CLR_M (BIT(9))
#define DMA_INFIFO_FULL_WM_CH4_INT_CLR_V 0x1
#define DMA_INFIFO_FULL_WM_CH4_INT_CLR_S 9
/* DMA_OUT_TOTAL_EOF_CH4_INT_CLR : WO ;bitpos:[8] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
#define DMA_OUT_TOTAL_EOF_CH4_INT_CLR (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH4_INT_CLR_M (BIT(8))
#define DMA_OUT_TOTAL_EOF_CH4_INT_CLR_V 0x1
#define DMA_OUT_TOTAL_EOF_CH4_INT_CLR_S 8
/* DMA_IN_DSCR_EMPTY_CH4_INT_CLR : WO ;bitpos:[7] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
#define DMA_IN_DSCR_EMPTY_CH4_INT_CLR (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH4_INT_CLR_M (BIT(7))
#define DMA_IN_DSCR_EMPTY_CH4_INT_CLR_V 0x1
#define DMA_IN_DSCR_EMPTY_CH4_INT_CLR_S 7
/* DMA_OUT_DSCR_ERR_CH4_INT_CLR : WO ;bitpos:[6] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
#define DMA_OUT_DSCR_ERR_CH4_INT_CLR (BIT(6))
#define DMA_OUT_DSCR_ERR_CH4_INT_CLR_M (BIT(6))
#define DMA_OUT_DSCR_ERR_CH4_INT_CLR_V 0x1
#define DMA_OUT_DSCR_ERR_CH4_INT_CLR_S 6
/* DMA_IN_DSCR_ERR_CH4_INT_CLR : WO ;bitpos:[5] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
#define DMA_IN_DSCR_ERR_CH4_INT_CLR (BIT(5))
#define DMA_IN_DSCR_ERR_CH4_INT_CLR_M (BIT(5))
#define DMA_IN_DSCR_ERR_CH4_INT_CLR_V 0x1
#define DMA_IN_DSCR_ERR_CH4_INT_CLR_S 5
/* DMA_OUT_EOF_CH4_INT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
#define DMA_OUT_EOF_CH4_INT_CLR (BIT(4))
#define DMA_OUT_EOF_CH4_INT_CLR_M (BIT(4))
#define DMA_OUT_EOF_CH4_INT_CLR_V 0x1
#define DMA_OUT_EOF_CH4_INT_CLR_S 4
/* DMA_OUT_DONE_CH4_INT_CLR : WO ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
#define DMA_OUT_DONE_CH4_INT_CLR (BIT(3))
#define DMA_OUT_DONE_CH4_INT_CLR_M (BIT(3))
#define DMA_OUT_DONE_CH4_INT_CLR_V 0x1
#define DMA_OUT_DONE_CH4_INT_CLR_S 3
/* DMA_IN_ERR_EOF_CH4_INT_CLR : WO ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
#define DMA_IN_ERR_EOF_CH4_INT_CLR (BIT(2))
#define DMA_IN_ERR_EOF_CH4_INT_CLR_M (BIT(2))
#define DMA_IN_ERR_EOF_CH4_INT_CLR_V 0x1
#define DMA_IN_ERR_EOF_CH4_INT_CLR_S 2
/* DMA_IN_SUC_EOF_CH4_INT_CLR : WO ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
#define DMA_IN_SUC_EOF_CH4_INT_CLR (BIT(1))
#define DMA_IN_SUC_EOF_CH4_INT_CLR_M (BIT(1))
#define DMA_IN_SUC_EOF_CH4_INT_CLR_V 0x1
#define DMA_IN_SUC_EOF_CH4_INT_CLR_S 1
/* DMA_IN_DONE_CH4_INT_CLR : WO ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the IN_DONE_CH_INT interrupt.*/
#define DMA_IN_DONE_CH4_INT_CLR (BIT(0))
#define DMA_IN_DONE_CH4_INT_CLR_M (BIT(0))
#define DMA_IN_DONE_CH4_INT_CLR_V 0x1
#define DMA_IN_DONE_CH4_INT_CLR_S 0

#define DMA_INFIFO_STATUS_CH0_REG (DR_REG_DMA_BASE + 0x07C)
/* DMA_IN_BUF_HUNGRY_CH0 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_BUF_HUNGRY_CH0 (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH0_M (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH0_V 0x1
#define DMA_IN_BUF_HUNGRY_CH0_S 27
/* DMA_IN_REMAIN_UNDER_4B_L3_CH0 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_4B_L3_CH0 (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH0_M (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH0_V 0x1
#define DMA_IN_REMAIN_UNDER_4B_L3_CH0_S 26
/* DMA_IN_REMAIN_UNDER_3B_L3_CH0 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_3B_L3_CH0 (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH0_M (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH0_V 0x1
#define DMA_IN_REMAIN_UNDER_3B_L3_CH0_S 25
/* DMA_IN_REMAIN_UNDER_2B_L3_CH0 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_2B_L3_CH0 (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH0_M (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH0_V 0x1
#define DMA_IN_REMAIN_UNDER_2B_L3_CH0_S 24
/* DMA_IN_REMAIN_UNDER_1B_L3_CH0 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_1B_L3_CH0 (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH0_M (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH0_V 0x1
#define DMA_IN_REMAIN_UNDER_1B_L3_CH0_S 23
/* DMA_INFIFO_CNT_L3_CH0 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 0.*/
#define DMA_INFIFO_CNT_L3_CH0 0x0000001F
#define DMA_INFIFO_CNT_L3_CH0_M ((DMA_INFIFO_CNT_L3_CH0_V) << (DMA_INFIFO_CNT_L3_CH0_S))
#define DMA_INFIFO_CNT_L3_CH0_V 0x1F
#define DMA_INFIFO_CNT_L3_CH0_S 18
/* DMA_INFIFO_CNT_L2_CH0 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 0.*/
#define DMA_INFIFO_CNT_L2_CH0 0x0000007F
#define DMA_INFIFO_CNT_L2_CH0_M ((DMA_INFIFO_CNT_L2_CH0_V) << (DMA_INFIFO_CNT_L2_CH0_S))
#define DMA_INFIFO_CNT_L2_CH0_V 0x7F
#define DMA_INFIFO_CNT_L2_CH0_S 11
/* DMA_INFIFO_CNT_L1_CH0 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 0.*/
#define DMA_INFIFO_CNT_L1_CH0 0x0000001F
#define DMA_INFIFO_CNT_L1_CH0_M ((DMA_INFIFO_CNT_L1_CH0_V) << (DMA_INFIFO_CNT_L1_CH0_S))
#define DMA_INFIFO_CNT_L1_CH0_V 0x1F
#define DMA_INFIFO_CNT_L1_CH0_S 6
/* DMA_INFIFO_EMPTY_L3_CH0 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 0.*/
#define DMA_INFIFO_EMPTY_L3_CH0 (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH0_M (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH0_V 0x1
#define DMA_INFIFO_EMPTY_L3_CH0_S 5
/* DMA_INFIFO_FULL_L3_CH0 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Rx FIFO full signal for Rx channel 0.*/
#define DMA_INFIFO_FULL_L3_CH0 (BIT(4))
#define DMA_INFIFO_FULL_L3_CH0_M (BIT(4))
#define DMA_INFIFO_FULL_L3_CH0_V 0x1
#define DMA_INFIFO_FULL_L3_CH0_S 4
/* DMA_INFIFO_EMPTY_L2_CH0 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 0.*/
#define DMA_INFIFO_EMPTY_L2_CH0 (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH0_M (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH0_V 0x1
#define DMA_INFIFO_EMPTY_L2_CH0_S 3
/* DMA_INFIFO_FULL_L2_CH0 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 0.*/
#define DMA_INFIFO_FULL_L2_CH0 (BIT(2))
#define DMA_INFIFO_FULL_L2_CH0_M (BIT(2))
#define DMA_INFIFO_FULL_L2_CH0_V 0x1
#define DMA_INFIFO_FULL_L2_CH0_S 2
/* DMA_INFIFO_EMPTY_L1_CH0 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 0.*/
#define DMA_INFIFO_EMPTY_L1_CH0 (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH0_M (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH0_V 0x1
#define DMA_INFIFO_EMPTY_L1_CH0_S 1
/* DMA_INFIFO_FULL_L1_CH0 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 0.*/
#define DMA_INFIFO_FULL_L1_CH0 (BIT(0))
#define DMA_INFIFO_FULL_L1_CH0_M (BIT(0))
#define DMA_INFIFO_FULL_L1_CH0_V 0x1
#define DMA_INFIFO_FULL_L1_CH0_S 0

#define DMA_INFIFO_STATUS_CH1_REG (DR_REG_DMA_BASE + 0x080)
/* DMA_IN_BUF_HUNGRY_CH1 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_BUF_HUNGRY_CH1 (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH1_M (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH1_V 0x1
#define DMA_IN_BUF_HUNGRY_CH1_S 27
/* DMA_IN_REMAIN_UNDER_4B_L3_CH1 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_4B_L3_CH1 (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH1_M (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH1_V 0x1
#define DMA_IN_REMAIN_UNDER_4B_L3_CH1_S 26
/* DMA_IN_REMAIN_UNDER_3B_L3_CH1 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_3B_L3_CH1 (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH1_M (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH1_V 0x1
#define DMA_IN_REMAIN_UNDER_3B_L3_CH1_S 25
/* DMA_IN_REMAIN_UNDER_2B_L3_CH1 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_2B_L3_CH1 (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH1_M (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH1_V 0x1
#define DMA_IN_REMAIN_UNDER_2B_L3_CH1_S 24
/* DMA_IN_REMAIN_UNDER_1B_L3_CH1 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_1B_L3_CH1 (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH1_M (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH1_V 0x1
#define DMA_IN_REMAIN_UNDER_1B_L3_CH1_S 23
/* DMA_INFIFO_CNT_L3_CH1 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 1.*/
#define DMA_INFIFO_CNT_L3_CH1 0x0000001F
#define DMA_INFIFO_CNT_L3_CH1_M ((DMA_INFIFO_CNT_L3_CH1_V) << (DMA_INFIFO_CNT_L3_CH1_S))
#define DMA_INFIFO_CNT_L3_CH1_V 0x1F
#define DMA_INFIFO_CNT_L3_CH1_S 18
/* DMA_INFIFO_CNT_L2_CH1 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 1.*/
#define DMA_INFIFO_CNT_L2_CH1 0x0000007F
#define DMA_INFIFO_CNT_L2_CH1_M ((DMA_INFIFO_CNT_L2_CH1_V) << (DMA_INFIFO_CNT_L2_CH1_S))
#define DMA_INFIFO_CNT_L2_CH1_V 0x7F
#define DMA_INFIFO_CNT_L2_CH1_S 11
/* DMA_INFIFO_CNT_L1_CH1 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 1.*/
#define DMA_INFIFO_CNT_L1_CH1 0x0000001F
#define DMA_INFIFO_CNT_L1_CH1_M ((DMA_INFIFO_CNT_L1_CH1_V) << (DMA_INFIFO_CNT_L1_CH1_S))
#define DMA_INFIFO_CNT_L1_CH1_V 0x1F
#define DMA_INFIFO_CNT_L1_CH1_S 6
/* DMA_INFIFO_EMPTY_L3_CH1 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 1.*/
#define DMA_INFIFO_EMPTY_L3_CH1 (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH1_M (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH1_V 0x1
#define DMA_INFIFO_EMPTY_L3_CH1_S 5
/* DMA_INFIFO_FULL_L3_CH1 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Rx FIFO full signal for Rx channel 1.*/
#define DMA_INFIFO_FULL_L3_CH1 (BIT(4))
#define DMA_INFIFO_FULL_L3_CH1_M (BIT(4))
#define DMA_INFIFO_FULL_L3_CH1_V 0x1
#define DMA_INFIFO_FULL_L3_CH1_S 4
/* DMA_INFIFO_EMPTY_L2_CH1 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 1.*/
#define DMA_INFIFO_EMPTY_L2_CH1 (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH1_M (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH1_V 0x1
#define DMA_INFIFO_EMPTY_L2_CH1_S 3
/* DMA_INFIFO_FULL_L2_CH1 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 1.*/
#define DMA_INFIFO_FULL_L2_CH1 (BIT(2))
#define DMA_INFIFO_FULL_L2_CH1_M (BIT(2))
#define DMA_INFIFO_FULL_L2_CH1_V 0x1
#define DMA_INFIFO_FULL_L2_CH1_S 2
/* DMA_INFIFO_EMPTY_L1_CH1 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 1.*/
#define DMA_INFIFO_EMPTY_L1_CH1 (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH1_M (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH1_V 0x1
#define DMA_INFIFO_EMPTY_L1_CH1_S 1
/* DMA_INFIFO_FULL_L1_CH1 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 1.*/
#define DMA_INFIFO_FULL_L1_CH1 (BIT(0))
#define DMA_INFIFO_FULL_L1_CH1_M (BIT(0))
#define DMA_INFIFO_FULL_L1_CH1_V 0x1
#define DMA_INFIFO_FULL_L1_CH1_S 0

#define DMA_INFIFO_STATUS_CH2_REG (DR_REG_DMA_BASE + 0x084)
/* DMA_IN_BUF_HUNGRY_CH2 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_BUF_HUNGRY_CH2 (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH2_M (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH2_V 0x1
#define DMA_IN_BUF_HUNGRY_CH2_S 27
/* DMA_IN_REMAIN_UNDER_4B_L3_CH2 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_4B_L3_CH2 (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH2_M (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH2_V 0x1
#define DMA_IN_REMAIN_UNDER_4B_L3_CH2_S 26
/* DMA_IN_REMAIN_UNDER_3B_L3_CH2 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_3B_L3_CH2 (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH2_M (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH2_V 0x1
#define DMA_IN_REMAIN_UNDER_3B_L3_CH2_S 25
/* DMA_IN_REMAIN_UNDER_2B_L3_CH2 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_2B_L3_CH2 (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH2_M (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH2_V 0x1
#define DMA_IN_REMAIN_UNDER_2B_L3_CH2_S 24
/* DMA_IN_REMAIN_UNDER_1B_L3_CH2 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_1B_L3_CH2 (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH2_M (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH2_V 0x1
#define DMA_IN_REMAIN_UNDER_1B_L3_CH2_S 23
/* DMA_INFIFO_CNT_L3_CH2 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 2.*/
#define DMA_INFIFO_CNT_L3_CH2 0x0000001F
#define DMA_INFIFO_CNT_L3_CH2_M ((DMA_INFIFO_CNT_L3_CH2_V) << (DMA_INFIFO_CNT_L3_CH2_S))
#define DMA_INFIFO_CNT_L3_CH2_V 0x1F
#define DMA_INFIFO_CNT_L3_CH2_S 18
/* DMA_INFIFO_CNT_L2_CH2 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 2.*/
#define DMA_INFIFO_CNT_L2_CH2 0x0000007F
#define DMA_INFIFO_CNT_L2_CH2_M ((DMA_INFIFO_CNT_L2_CH2_V) << (DMA_INFIFO_CNT_L2_CH2_S))
#define DMA_INFIFO_CNT_L2_CH2_V 0x7F
#define DMA_INFIFO_CNT_L2_CH2_S 11
/* DMA_INFIFO_CNT_L1_CH2 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 2.*/
#define DMA_INFIFO_CNT_L1_CH2 0x0000001F
#define DMA_INFIFO_CNT_L1_CH2_M ((DMA_INFIFO_CNT_L1_CH2_V) << (DMA_INFIFO_CNT_L1_CH2_S))
#define DMA_INFIFO_CNT_L1_CH2_V 0x1F
#define DMA_INFIFO_CNT_L1_CH2_S 6
/* DMA_INFIFO_EMPTY_L3_CH2 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 2.*/
#define DMA_INFIFO_EMPTY_L3_CH2 (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH2_M (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH2_V 0x1
#define DMA_INFIFO_EMPTY_L3_CH2_S 5
/* DMA_INFIFO_FULL_L3_CH2 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Rx FIFO full signal for Rx channel 2.*/
#define DMA_INFIFO_FULL_L3_CH2 (BIT(4))
#define DMA_INFIFO_FULL_L3_CH2_M (BIT(4))
#define DMA_INFIFO_FULL_L3_CH2_V 0x1
#define DMA_INFIFO_FULL_L3_CH2_S 4
/* DMA_INFIFO_EMPTY_L2_CH2 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 2.*/
#define DMA_INFIFO_EMPTY_L2_CH2 (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH2_M (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH2_V 0x1
#define DMA_INFIFO_EMPTY_L2_CH2_S 3
/* DMA_INFIFO_FULL_L2_CH2 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 2.*/
#define DMA_INFIFO_FULL_L2_CH2 (BIT(2))
#define DMA_INFIFO_FULL_L2_CH2_M (BIT(2))
#define DMA_INFIFO_FULL_L2_CH2_V 0x1
#define DMA_INFIFO_FULL_L2_CH2_S 2
/* DMA_INFIFO_EMPTY_L1_CH2 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 2.*/
#define DMA_INFIFO_EMPTY_L1_CH2 (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH2_M (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH2_V 0x1
#define DMA_INFIFO_EMPTY_L1_CH2_S 1
/* DMA_INFIFO_FULL_L1_CH2 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 2.*/
#define DMA_INFIFO_FULL_L1_CH2 (BIT(0))
#define DMA_INFIFO_FULL_L1_CH2_M (BIT(0))
#define DMA_INFIFO_FULL_L1_CH2_V 0x1
#define DMA_INFIFO_FULL_L1_CH2_S 0

#define DMA_INFIFO_STATUS_CH3_REG (DR_REG_DMA_BASE + 0x088)
/* DMA_IN_BUF_HUNGRY_CH3 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_BUF_HUNGRY_CH3 (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH3_M (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH3_V 0x1
#define DMA_IN_BUF_HUNGRY_CH3_S 27
/* DMA_IN_REMAIN_UNDER_4B_L3_CH3 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_4B_L3_CH3 (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH3_M (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH3_V 0x1
#define DMA_IN_REMAIN_UNDER_4B_L3_CH3_S 26
/* DMA_IN_REMAIN_UNDER_3B_L3_CH3 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_3B_L3_CH3 (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH3_M (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH3_V 0x1
#define DMA_IN_REMAIN_UNDER_3B_L3_CH3_S 25
/* DMA_IN_REMAIN_UNDER_2B_L3_CH3 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_2B_L3_CH3 (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH3_M (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH3_V 0x1
#define DMA_IN_REMAIN_UNDER_2B_L3_CH3_S 24
/* DMA_IN_REMAIN_UNDER_1B_L3_CH3 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_1B_L3_CH3 (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH3_M (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH3_V 0x1
#define DMA_IN_REMAIN_UNDER_1B_L3_CH3_S 23
/* DMA_INFIFO_CNT_L3_CH3 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 3.*/
#define DMA_INFIFO_CNT_L3_CH3 0x0000001F
#define DMA_INFIFO_CNT_L3_CH3_M ((DMA_INFIFO_CNT_L3_CH3_V) << (DMA_INFIFO_CNT_L3_CH3_S))
#define DMA_INFIFO_CNT_L3_CH3_V 0x1F
#define DMA_INFIFO_CNT_L3_CH3_S 18
/* DMA_INFIFO_CNT_L2_CH3 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 3.*/
#define DMA_INFIFO_CNT_L2_CH3 0x0000007F
#define DMA_INFIFO_CNT_L2_CH3_M ((DMA_INFIFO_CNT_L2_CH3_V) << (DMA_INFIFO_CNT_L2_CH3_S))
#define DMA_INFIFO_CNT_L2_CH3_V 0x7F
#define DMA_INFIFO_CNT_L2_CH3_S 11
/* DMA_INFIFO_CNT_L1_CH3 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 3.*/
#define DMA_INFIFO_CNT_L1_CH3 0x0000001F
#define DMA_INFIFO_CNT_L1_CH3_M ((DMA_INFIFO_CNT_L1_CH3_V) << (DMA_INFIFO_CNT_L1_CH3_S))
#define DMA_INFIFO_CNT_L1_CH3_V 0x1F
#define DMA_INFIFO_CNT_L1_CH3_S 6
/* DMA_INFIFO_EMPTY_L3_CH3 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 3.*/
#define DMA_INFIFO_EMPTY_L3_CH3 (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH3_M (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH3_V 0x1
#define DMA_INFIFO_EMPTY_L3_CH3_S 5
/* DMA_INFIFO_FULL_L3_CH3 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Rx FIFO full signal for Rx channel 3.*/
#define DMA_INFIFO_FULL_L3_CH3 (BIT(4))
#define DMA_INFIFO_FULL_L3_CH3_M (BIT(4))
#define DMA_INFIFO_FULL_L3_CH3_V 0x1
#define DMA_INFIFO_FULL_L3_CH3_S 4
/* DMA_INFIFO_EMPTY_L2_CH3 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 3.*/
#define DMA_INFIFO_EMPTY_L2_CH3 (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH3_M (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH3_V 0x1
#define DMA_INFIFO_EMPTY_L2_CH3_S 3
/* DMA_INFIFO_FULL_L2_CH3 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 3.*/
#define DMA_INFIFO_FULL_L2_CH3 (BIT(2))
#define DMA_INFIFO_FULL_L2_CH3_M (BIT(2))
#define DMA_INFIFO_FULL_L2_CH3_V 0x1
#define DMA_INFIFO_FULL_L2_CH3_S 2
/* DMA_INFIFO_EMPTY_L1_CH3 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 3.*/
#define DMA_INFIFO_EMPTY_L1_CH3 (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH3_M (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH3_V 0x1
#define DMA_INFIFO_EMPTY_L1_CH3_S 1
/* DMA_INFIFO_FULL_L1_CH3 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 3.*/
#define DMA_INFIFO_FULL_L1_CH3 (BIT(0))
#define DMA_INFIFO_FULL_L1_CH3_M (BIT(0))
#define DMA_INFIFO_FULL_L1_CH3_V 0x1
#define DMA_INFIFO_FULL_L1_CH3_S 0

#define DMA_INFIFO_STATUS_CH4_REG (DR_REG_DMA_BASE + 0x08C)
/* DMA_IN_BUF_HUNGRY_CH4 : RO ;bitpos:[27] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_BUF_HUNGRY_CH4 (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH4_M (BIT(27))
#define DMA_IN_BUF_HUNGRY_CH4_V 0x1
#define DMA_IN_BUF_HUNGRY_CH4_S 27
/* DMA_IN_REMAIN_UNDER_4B_L3_CH4 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_4B_L3_CH4 (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH4_M (BIT(26))
#define DMA_IN_REMAIN_UNDER_4B_L3_CH4_V 0x1
#define DMA_IN_REMAIN_UNDER_4B_L3_CH4_S 26
/* DMA_IN_REMAIN_UNDER_3B_L3_CH4 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_3B_L3_CH4 (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH4_M (BIT(25))
#define DMA_IN_REMAIN_UNDER_3B_L3_CH4_V 0x1
#define DMA_IN_REMAIN_UNDER_3B_L3_CH4_S 25
/* DMA_IN_REMAIN_UNDER_2B_L3_CH4 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_2B_L3_CH4 (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH4_M (BIT(24))
#define DMA_IN_REMAIN_UNDER_2B_L3_CH4_V 0x1
#define DMA_IN_REMAIN_UNDER_2B_L3_CH4_S 24
/* DMA_IN_REMAIN_UNDER_1B_L3_CH4 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_IN_REMAIN_UNDER_1B_L3_CH4 (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH4_M (BIT(23))
#define DMA_IN_REMAIN_UNDER_1B_L3_CH4_V 0x1
#define DMA_IN_REMAIN_UNDER_1B_L3_CH4_S 23
/* DMA_INFIFO_CNT_L3_CH4 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Rx FIFO for Rx channel 4.*/
#define DMA_INFIFO_CNT_L3_CH4 0x0000001F
#define DMA_INFIFO_CNT_L3_CH4_M ((DMA_INFIFO_CNT_L3_CH4_V) << (DMA_INFIFO_CNT_L3_CH4_S))
#define DMA_INFIFO_CNT_L3_CH4_V 0x1F
#define DMA_INFIFO_CNT_L3_CH4_S 18
/* DMA_INFIFO_CNT_L2_CH4 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Rx FIFO for Rx channel 4.*/
#define DMA_INFIFO_CNT_L2_CH4 0x0000007F
#define DMA_INFIFO_CNT_L2_CH4_M ((DMA_INFIFO_CNT_L2_CH4_V) << (DMA_INFIFO_CNT_L2_CH4_S))
#define DMA_INFIFO_CNT_L2_CH4_V 0x7F
#define DMA_INFIFO_CNT_L2_CH4_S 11
/* DMA_INFIFO_CNT_L1_CH4 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Rx FIFO for Rx channel 4.*/
#define DMA_INFIFO_CNT_L1_CH4 0x0000001F
#define DMA_INFIFO_CNT_L1_CH4_M ((DMA_INFIFO_CNT_L1_CH4_V) << (DMA_INFIFO_CNT_L1_CH4_S))
#define DMA_INFIFO_CNT_L1_CH4_V 0x1F
#define DMA_INFIFO_CNT_L1_CH4_S 6
/* DMA_INFIFO_EMPTY_L3_CH4 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Rx FIFO empty signal for Rx channel 4.*/
#define DMA_INFIFO_EMPTY_L3_CH4 (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH4_M (BIT(5))
#define DMA_INFIFO_EMPTY_L3_CH4_V 0x1
#define DMA_INFIFO_EMPTY_L3_CH4_S 5
/* DMA_INFIFO_FULL_L3_CH4 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Rx FIFO full signal for Rx channel 4.*/
#define DMA_INFIFO_FULL_L3_CH4 (BIT(4))
#define DMA_INFIFO_FULL_L3_CH4_M (BIT(4))
#define DMA_INFIFO_FULL_L3_CH4_V 0x1
#define DMA_INFIFO_FULL_L3_CH4_S 4
/* DMA_INFIFO_EMPTY_L2_CH4 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Rx FIFO empty signal for Rx channel 4.*/
#define DMA_INFIFO_EMPTY_L2_CH4 (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH4_M (BIT(3))
#define DMA_INFIFO_EMPTY_L2_CH4_V 0x1
#define DMA_INFIFO_EMPTY_L2_CH4_S 3
/* DMA_INFIFO_FULL_L2_CH4 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Rx FIFO full signal for Rx channel 4.*/
#define DMA_INFIFO_FULL_L2_CH4 (BIT(2))
#define DMA_INFIFO_FULL_L2_CH4_M (BIT(2))
#define DMA_INFIFO_FULL_L2_CH4_V 0x1
#define DMA_INFIFO_FULL_L2_CH4_S 2
/* DMA_INFIFO_EMPTY_L1_CH4 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Rx FIFO empty signal for Rx channel 4.*/
#define DMA_INFIFO_EMPTY_L1_CH4 (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH4_M (BIT(1))
#define DMA_INFIFO_EMPTY_L1_CH4_V 0x1
#define DMA_INFIFO_EMPTY_L1_CH4_S 1
/* DMA_INFIFO_FULL_L1_CH4 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Rx FIFO full signal for Rx channel 4.*/
#define DMA_INFIFO_FULL_L1_CH4 (BIT(0))
#define DMA_INFIFO_FULL_L1_CH4_M (BIT(0))
#define DMA_INFIFO_FULL_L1_CH4_V 0x1
#define DMA_INFIFO_FULL_L1_CH4_S 0

#define DMA_OUTFIFO_STATUS_CH0_REG (DR_REG_DMA_BASE + 0x090)
/* DMA_OUT_REMAIN_UNDER_4B_L3_CH0 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH0 (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH0_M (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH0_V 0x1
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH0_S 26
/* DMA_OUT_REMAIN_UNDER_3B_L3_CH0 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH0 (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH0_M (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH0_V 0x1
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH0_S 25
/* DMA_OUT_REMAIN_UNDER_2B_L3_CH0 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH0 (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH0_M (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH0_V 0x1
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH0_S 24
/* DMA_OUT_REMAIN_UNDER_1B_L3_CH0 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH0 (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH0_M (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH0_V 0x1
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH0_S 23
/* DMA_OUTFIFO_CNT_L3_CH0 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 0.*/
#define DMA_OUTFIFO_CNT_L3_CH0 0x0000001F
#define DMA_OUTFIFO_CNT_L3_CH0_M ((DMA_OUTFIFO_CNT_L3_CH0_V) << (DMA_OUTFIFO_CNT_L3_CH0_S))
#define DMA_OUTFIFO_CNT_L3_CH0_V 0x1F
#define DMA_OUTFIFO_CNT_L3_CH0_S 18
/* DMA_OUTFIFO_CNT_L2_CH0 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 0.*/
#define DMA_OUTFIFO_CNT_L2_CH0 0x0000007F
#define DMA_OUTFIFO_CNT_L2_CH0_M ((DMA_OUTFIFO_CNT_L2_CH0_V) << (DMA_OUTFIFO_CNT_L2_CH0_S))
#define DMA_OUTFIFO_CNT_L2_CH0_V 0x7F
#define DMA_OUTFIFO_CNT_L2_CH0_S 11
/* DMA_OUTFIFO_CNT_L1_CH0 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 0.*/
#define DMA_OUTFIFO_CNT_L1_CH0 0x0000001F
#define DMA_OUTFIFO_CNT_L1_CH0_M ((DMA_OUTFIFO_CNT_L1_CH0_V) << (DMA_OUTFIFO_CNT_L1_CH0_S))
#define DMA_OUTFIFO_CNT_L1_CH0_V 0x1F
#define DMA_OUTFIFO_CNT_L1_CH0_S 6
/* DMA_OUTFIFO_EMPTY_L3_CH0 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 0.*/
#define DMA_OUTFIFO_EMPTY_L3_CH0 (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH0_M (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH0_V 0x1
#define DMA_OUTFIFO_EMPTY_L3_CH0_S 5
/* DMA_OUTFIFO_FULL_L3_CH0 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 0.*/
#define DMA_OUTFIFO_FULL_L3_CH0 (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH0_M (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH0_V 0x1
#define DMA_OUTFIFO_FULL_L3_CH0_S 4
/* DMA_OUTFIFO_EMPTY_L2_CH0 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 0.*/
#define DMA_OUTFIFO_EMPTY_L2_CH0 (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH0_M (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH0_V 0x1
#define DMA_OUTFIFO_EMPTY_L2_CH0_S 3
/* DMA_OUTFIFO_FULL_L2_CH0 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 0.*/
#define DMA_OUTFIFO_FULL_L2_CH0 (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH0_M (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH0_V 0x1
#define DMA_OUTFIFO_FULL_L2_CH0_S 2
/* DMA_OUTFIFO_EMPTY_L1_CH0 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 0.*/
#define DMA_OUTFIFO_EMPTY_L1_CH0 (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH0_M (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH0_V 0x1
#define DMA_OUTFIFO_EMPTY_L1_CH0_S 1
/* DMA_OUTFIFO_FULL_L1_CH0 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 0.*/
#define DMA_OUTFIFO_FULL_L1_CH0 (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH0_M (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH0_V 0x1
#define DMA_OUTFIFO_FULL_L1_CH0_S 0

#define DMA_OUTFIFO_STATUS_CH1_REG (DR_REG_DMA_BASE + 0x094)
/* DMA_OUT_REMAIN_UNDER_4B_L3_CH1 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH1 (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH1_M (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH1_V 0x1
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH1_S 26
/* DMA_OUT_REMAIN_UNDER_3B_L3_CH1 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH1 (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH1_M (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH1_V 0x1
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH1_S 25
/* DMA_OUT_REMAIN_UNDER_2B_L3_CH1 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH1 (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH1_M (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH1_V 0x1
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH1_S 24
/* DMA_OUT_REMAIN_UNDER_1B_L3_CH1 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH1 (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH1_M (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH1_V 0x1
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH1_S 23
/* DMA_OUTFIFO_CNT_L3_CH1 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 1.*/
#define DMA_OUTFIFO_CNT_L3_CH1 0x0000001F
#define DMA_OUTFIFO_CNT_L3_CH1_M ((DMA_OUTFIFO_CNT_L3_CH1_V) << (DMA_OUTFIFO_CNT_L3_CH1_S))
#define DMA_OUTFIFO_CNT_L3_CH1_V 0x1F
#define DMA_OUTFIFO_CNT_L3_CH1_S 18
/* DMA_OUTFIFO_CNT_L2_CH1 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 1.*/
#define DMA_OUTFIFO_CNT_L2_CH1 0x0000007F
#define DMA_OUTFIFO_CNT_L2_CH1_M ((DMA_OUTFIFO_CNT_L2_CH1_V) << (DMA_OUTFIFO_CNT_L2_CH1_S))
#define DMA_OUTFIFO_CNT_L2_CH1_V 0x7F
#define DMA_OUTFIFO_CNT_L2_CH1_S 11
/* DMA_OUTFIFO_CNT_L1_CH1 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 1.*/
#define DMA_OUTFIFO_CNT_L1_CH1 0x0000001F
#define DMA_OUTFIFO_CNT_L1_CH1_M ((DMA_OUTFIFO_CNT_L1_CH1_V) << (DMA_OUTFIFO_CNT_L1_CH1_S))
#define DMA_OUTFIFO_CNT_L1_CH1_V 0x1F
#define DMA_OUTFIFO_CNT_L1_CH1_S 6
/* DMA_OUTFIFO_EMPTY_L3_CH1 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 1.*/
#define DMA_OUTFIFO_EMPTY_L3_CH1 (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH1_M (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH1_V 0x1
#define DMA_OUTFIFO_EMPTY_L3_CH1_S 5
/* DMA_OUTFIFO_FULL_L3_CH1 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 1.*/
#define DMA_OUTFIFO_FULL_L3_CH1 (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH1_M (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH1_V 0x1
#define DMA_OUTFIFO_FULL_L3_CH1_S 4
/* DMA_OUTFIFO_EMPTY_L2_CH1 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 1.*/
#define DMA_OUTFIFO_EMPTY_L2_CH1 (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH1_M (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH1_V 0x1
#define DMA_OUTFIFO_EMPTY_L2_CH1_S 3
/* DMA_OUTFIFO_FULL_L2_CH1 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 1.*/
#define DMA_OUTFIFO_FULL_L2_CH1 (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH1_M (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH1_V 0x1
#define DMA_OUTFIFO_FULL_L2_CH1_S 2
/* DMA_OUTFIFO_EMPTY_L1_CH1 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 1.*/
#define DMA_OUTFIFO_EMPTY_L1_CH1 (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH1_M (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH1_V 0x1
#define DMA_OUTFIFO_EMPTY_L1_CH1_S 1
/* DMA_OUTFIFO_FULL_L1_CH1 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 1.*/
#define DMA_OUTFIFO_FULL_L1_CH1 (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH1_M (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH1_V 0x1
#define DMA_OUTFIFO_FULL_L1_CH1_S 0

#define DMA_OUTFIFO_STATUS_CH2_REG (DR_REG_DMA_BASE + 0x098)
/* DMA_OUT_REMAIN_UNDER_4B_L3_CH2 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH2 (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH2_M (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH2_V 0x1
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH2_S 26
/* DMA_OUT_REMAIN_UNDER_3B_L3_CH2 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH2 (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH2_M (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH2_V 0x1
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH2_S 25
/* DMA_OUT_REMAIN_UNDER_2B_L3_CH2 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH2 (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH2_M (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH2_V 0x1
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH2_S 24
/* DMA_OUT_REMAIN_UNDER_1B_L3_CH2 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH2 (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH2_M (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH2_V 0x1
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH2_S 23
/* DMA_OUTFIFO_CNT_L3_CH2 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 2.*/
#define DMA_OUTFIFO_CNT_L3_CH2 0x0000001F
#define DMA_OUTFIFO_CNT_L3_CH2_M ((DMA_OUTFIFO_CNT_L3_CH2_V) << (DMA_OUTFIFO_CNT_L3_CH2_S))
#define DMA_OUTFIFO_CNT_L3_CH2_V 0x1F
#define DMA_OUTFIFO_CNT_L3_CH2_S 18
/* DMA_OUTFIFO_CNT_L2_CH2 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 2.*/
#define DMA_OUTFIFO_CNT_L2_CH2 0x0000007F
#define DMA_OUTFIFO_CNT_L2_CH2_M ((DMA_OUTFIFO_CNT_L2_CH2_V) << (DMA_OUTFIFO_CNT_L2_CH2_S))
#define DMA_OUTFIFO_CNT_L2_CH2_V 0x7F
#define DMA_OUTFIFO_CNT_L2_CH2_S 11
/* DMA_OUTFIFO_CNT_L1_CH2 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 2.*/
#define DMA_OUTFIFO_CNT_L1_CH2 0x0000001F
#define DMA_OUTFIFO_CNT_L1_CH2_M ((DMA_OUTFIFO_CNT_L1_CH2_V) << (DMA_OUTFIFO_CNT_L1_CH2_S))
#define DMA_OUTFIFO_CNT_L1_CH2_V 0x1F
#define DMA_OUTFIFO_CNT_L1_CH2_S 6
/* DMA_OUTFIFO_EMPTY_L3_CH2 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 2.*/
#define DMA_OUTFIFO_EMPTY_L3_CH2 (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH2_M (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH2_V 0x1
#define DMA_OUTFIFO_EMPTY_L3_CH2_S 5
/* DMA_OUTFIFO_FULL_L3_CH2 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 2.*/
#define DMA_OUTFIFO_FULL_L3_CH2 (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH2_M (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH2_V 0x1
#define DMA_OUTFIFO_FULL_L3_CH2_S 4
/* DMA_OUTFIFO_EMPTY_L2_CH2 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 2.*/
#define DMA_OUTFIFO_EMPTY_L2_CH2 (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH2_M (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH2_V 0x1
#define DMA_OUTFIFO_EMPTY_L2_CH2_S 3
/* DMA_OUTFIFO_FULL_L2_CH2 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 2.*/
#define DMA_OUTFIFO_FULL_L2_CH2 (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH2_M (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH2_V 0x1
#define DMA_OUTFIFO_FULL_L2_CH2_S 2
/* DMA_OUTFIFO_EMPTY_L1_CH2 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 2.*/
#define DMA_OUTFIFO_EMPTY_L1_CH2 (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH2_M (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH2_V 0x1
#define DMA_OUTFIFO_EMPTY_L1_CH2_S 1
/* DMA_OUTFIFO_FULL_L1_CH2 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 2.*/
#define DMA_OUTFIFO_FULL_L1_CH2 (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH2_M (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH2_V 0x1
#define DMA_OUTFIFO_FULL_L1_CH2_S 0

#define DMA_OUTFIFO_STATUS_CH3_REG (DR_REG_DMA_BASE + 0x09C)
/* DMA_OUT_REMAIN_UNDER_4B_L3_CH3 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH3 (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH3_M (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH3_V 0x1
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH3_S 26
/* DMA_OUT_REMAIN_UNDER_3B_L3_CH3 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH3 (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH3_M (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH3_V 0x1
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH3_S 25
/* DMA_OUT_REMAIN_UNDER_2B_L3_CH3 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH3 (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH3_M (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH3_V 0x1
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH3_S 24
/* DMA_OUT_REMAIN_UNDER_1B_L3_CH3 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH3 (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH3_M (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH3_V 0x1
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH3_S 23
/* DMA_OUTFIFO_CNT_L3_CH3 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 3.*/
#define DMA_OUTFIFO_CNT_L3_CH3 0x0000001F
#define DMA_OUTFIFO_CNT_L3_CH3_M ((DMA_OUTFIFO_CNT_L3_CH3_V) << (DMA_OUTFIFO_CNT_L3_CH3_S))
#define DMA_OUTFIFO_CNT_L3_CH3_V 0x1F
#define DMA_OUTFIFO_CNT_L3_CH3_S 18
/* DMA_OUTFIFO_CNT_L2_CH3 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 3.*/
#define DMA_OUTFIFO_CNT_L2_CH3 0x0000007F
#define DMA_OUTFIFO_CNT_L2_CH3_M ((DMA_OUTFIFO_CNT_L2_CH3_V) << (DMA_OUTFIFO_CNT_L2_CH3_S))
#define DMA_OUTFIFO_CNT_L2_CH3_V 0x7F
#define DMA_OUTFIFO_CNT_L2_CH3_S 11
/* DMA_OUTFIFO_CNT_L1_CH3 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 3.*/
#define DMA_OUTFIFO_CNT_L1_CH3 0x0000001F
#define DMA_OUTFIFO_CNT_L1_CH3_M ((DMA_OUTFIFO_CNT_L1_CH3_V) << (DMA_OUTFIFO_CNT_L1_CH3_S))
#define DMA_OUTFIFO_CNT_L1_CH3_V 0x1F
#define DMA_OUTFIFO_CNT_L1_CH3_S 6
/* DMA_OUTFIFO_EMPTY_L3_CH3 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 3.*/
#define DMA_OUTFIFO_EMPTY_L3_CH3 (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH3_M (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH3_V 0x1
#define DMA_OUTFIFO_EMPTY_L3_CH3_S 5
/* DMA_OUTFIFO_FULL_L3_CH3 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 3.*/
#define DMA_OUTFIFO_FULL_L3_CH3 (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH3_M (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH3_V 0x1
#define DMA_OUTFIFO_FULL_L3_CH3_S 4
/* DMA_OUTFIFO_EMPTY_L2_CH3 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 3.*/
#define DMA_OUTFIFO_EMPTY_L2_CH3 (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH3_M (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH3_V 0x1
#define DMA_OUTFIFO_EMPTY_L2_CH3_S 3
/* DMA_OUTFIFO_FULL_L2_CH3 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 3.*/
#define DMA_OUTFIFO_FULL_L2_CH3 (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH3_M (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH3_V 0x1
#define DMA_OUTFIFO_FULL_L2_CH3_S 2
/* DMA_OUTFIFO_EMPTY_L1_CH3 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 3.*/
#define DMA_OUTFIFO_EMPTY_L1_CH3 (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH3_M (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH3_V 0x1
#define DMA_OUTFIFO_EMPTY_L1_CH3_S 1
/* DMA_OUTFIFO_FULL_L1_CH3 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 3.*/
#define DMA_OUTFIFO_FULL_L1_CH3 (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH3_M (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH3_V 0x1
#define DMA_OUTFIFO_FULL_L1_CH3_S 0

#define DMA_OUTFIFO_STATUS_CH4_REG (DR_REG_DMA_BASE + 0x0A0)
/* DMA_OUT_REMAIN_UNDER_4B_L3_CH4 : RO ;bitpos:[26] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH4 (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH4_M (BIT(26))
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH4_V 0x1
#define DMA_OUT_REMAIN_UNDER_4B_L3_CH4_S 26
/* DMA_OUT_REMAIN_UNDER_3B_L3_CH4 : RO ;bitpos:[25] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH4 (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH4_M (BIT(25))
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH4_V 0x1
#define DMA_OUT_REMAIN_UNDER_3B_L3_CH4_S 25
/* DMA_OUT_REMAIN_UNDER_2B_L3_CH4 : RO ;bitpos:[24] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH4 (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH4_M (BIT(24))
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH4_V 0x1
#define DMA_OUT_REMAIN_UNDER_2B_L3_CH4_S 24
/* DMA_OUT_REMAIN_UNDER_1B_L3_CH4 : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: reserved*/
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH4 (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH4_M (BIT(23))
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH4_V 0x1
#define DMA_OUT_REMAIN_UNDER_1B_L3_CH4_S 23
/* DMA_OUTFIFO_CNT_L3_CH4 : RO ;bitpos:[22:18] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L3 Tx FIFO for Tx channel 4.*/
#define DMA_OUTFIFO_CNT_L3_CH4 0x0000001F
#define DMA_OUTFIFO_CNT_L3_CH4_M ((DMA_OUTFIFO_CNT_L3_CH4_V) << (DMA_OUTFIFO_CNT_L3_CH4_S))
#define DMA_OUTFIFO_CNT_L3_CH4_V 0x1F
#define DMA_OUTFIFO_CNT_L3_CH4_S 18
/* DMA_OUTFIFO_CNT_L2_CH4 : RO ;bitpos:[17:11] ;default: 7'b0 ; */
/*description: The register stores the byte number of the data in L2 Tx FIFO for Tx channel 4.*/
#define DMA_OUTFIFO_CNT_L2_CH4 0x0000007F
#define DMA_OUTFIFO_CNT_L2_CH4_M ((DMA_OUTFIFO_CNT_L2_CH4_V) << (DMA_OUTFIFO_CNT_L2_CH4_S))
#define DMA_OUTFIFO_CNT_L2_CH4_V 0x7F
#define DMA_OUTFIFO_CNT_L2_CH4_S 11
/* DMA_OUTFIFO_CNT_L1_CH4 : RO ;bitpos:[10:6] ;default: 5'b0 ; */
/*description: The register stores the byte number of the data in L1 Tx FIFO for Tx channel 4.*/
#define DMA_OUTFIFO_CNT_L1_CH4 0x0000001F
#define DMA_OUTFIFO_CNT_L1_CH4_M ((DMA_OUTFIFO_CNT_L1_CH4_V) << (DMA_OUTFIFO_CNT_L1_CH4_S))
#define DMA_OUTFIFO_CNT_L1_CH4_V 0x1F
#define DMA_OUTFIFO_CNT_L1_CH4_S 6
/* DMA_OUTFIFO_EMPTY_L3_CH4 : RO ;bitpos:[5] ;default: 1'b1 ; */
/*description: L3 Tx FIFO empty signal for Tx channel 4.*/
#define DMA_OUTFIFO_EMPTY_L3_CH4 (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH4_M (BIT(5))
#define DMA_OUTFIFO_EMPTY_L3_CH4_V 0x1
#define DMA_OUTFIFO_EMPTY_L3_CH4_S 5
/* DMA_OUTFIFO_FULL_L3_CH4 : RO ;bitpos:[4] ;default: 1'b0 ; */
/*description: L3 Tx FIFO full signal for Tx channel 4.*/
#define DMA_OUTFIFO_FULL_L3_CH4 (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH4_M (BIT(4))
#define DMA_OUTFIFO_FULL_L3_CH4_V 0x1
#define DMA_OUTFIFO_FULL_L3_CH4_S 4
/* DMA_OUTFIFO_EMPTY_L2_CH4 : RO ;bitpos:[3] ;default: 1'b1 ; */
/*description: L2 Tx FIFO empty signal for Tx channel 4.*/
#define DMA_OUTFIFO_EMPTY_L2_CH4 (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH4_M (BIT(3))
#define DMA_OUTFIFO_EMPTY_L2_CH4_V 0x1
#define DMA_OUTFIFO_EMPTY_L2_CH4_S 3
/* DMA_OUTFIFO_FULL_L2_CH4 : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: L2 Tx FIFO full signal for Tx channel 4.*/
#define DMA_OUTFIFO_FULL_L2_CH4 (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH4_M (BIT(2))
#define DMA_OUTFIFO_FULL_L2_CH4_V 0x1
#define DMA_OUTFIFO_FULL_L2_CH4_S 2
/* DMA_OUTFIFO_EMPTY_L1_CH4 : RO ;bitpos:[1] ;default: 1'b1 ; */
/*description: L1 Tx FIFO empty signal for Tx channel 4.*/
#define DMA_OUTFIFO_EMPTY_L1_CH4 (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH4_M (BIT(1))
#define DMA_OUTFIFO_EMPTY_L1_CH4_V 0x1
#define DMA_OUTFIFO_EMPTY_L1_CH4_S 1
/* DMA_OUTFIFO_FULL_L1_CH4 : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: L1 Tx FIFO full signal for Tx channel 4.*/
#define DMA_OUTFIFO_FULL_L1_CH4 (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH4_M (BIT(0))
#define DMA_OUTFIFO_FULL_L1_CH4_V 0x1
#define DMA_OUTFIFO_FULL_L1_CH4_S 0

#define DMA_OUT_PUSH_CH0_REG (DR_REG_DMA_BASE + 0x0A4)
/* DMA_OUTFIFO_PUSH_CH0 : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO.*/
#define DMA_OUTFIFO_PUSH_CH0 (BIT(9))
#define DMA_OUTFIFO_PUSH_CH0_M (BIT(9))
#define DMA_OUTFIFO_PUSH_CH0_V 0x1
#define DMA_OUTFIFO_PUSH_CH0_S 9
/* DMA_OUTFIFO_WDATA_CH0 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO.*/
#define DMA_OUTFIFO_WDATA_CH0 0x000001FF
#define DMA_OUTFIFO_WDATA_CH0_M ((DMA_OUTFIFO_WDATA_CH0_V) << (DMA_OUTFIFO_WDATA_CH0_S))
#define DMA_OUTFIFO_WDATA_CH0_V 0x1FF
#define DMA_OUTFIFO_WDATA_CH0_S 0

#define DMA_OUT_PUSH_CH1_REG (DR_REG_DMA_BASE + 0x0A8)
/* DMA_OUTFIFO_PUSH_CH1 : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO.*/
#define DMA_OUTFIFO_PUSH_CH1 (BIT(9))
#define DMA_OUTFIFO_PUSH_CH1_M (BIT(9))
#define DMA_OUTFIFO_PUSH_CH1_V 0x1
#define DMA_OUTFIFO_PUSH_CH1_S 9
/* DMA_OUTFIFO_WDATA_CH1 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO.*/
#define DMA_OUTFIFO_WDATA_CH1 0x000001FF
#define DMA_OUTFIFO_WDATA_CH1_M ((DMA_OUTFIFO_WDATA_CH1_V) << (DMA_OUTFIFO_WDATA_CH1_S))
#define DMA_OUTFIFO_WDATA_CH1_V 0x1FF
#define DMA_OUTFIFO_WDATA_CH1_S 0

#define DMA_OUT_PUSH_CH2_REG (DR_REG_DMA_BASE + 0x0AC)
/* DMA_OUTFIFO_PUSH_CH2 : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO.*/
#define DMA_OUTFIFO_PUSH_CH2 (BIT(9))
#define DMA_OUTFIFO_PUSH_CH2_M (BIT(9))
#define DMA_OUTFIFO_PUSH_CH2_V 0x1
#define DMA_OUTFIFO_PUSH_CH2_S 9
/* DMA_OUTFIFO_WDATA_CH2 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO.*/
#define DMA_OUTFIFO_WDATA_CH2 0x000001FF
#define DMA_OUTFIFO_WDATA_CH2_M ((DMA_OUTFIFO_WDATA_CH2_V) << (DMA_OUTFIFO_WDATA_CH2_S))
#define DMA_OUTFIFO_WDATA_CH2_V 0x1FF
#define DMA_OUTFIFO_WDATA_CH2_S 0

#define DMA_OUT_PUSH_CH3_REG (DR_REG_DMA_BASE + 0x0B0)
/* DMA_OUTFIFO_PUSH_CH3 : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO.*/
#define DMA_OUTFIFO_PUSH_CH3 (BIT(9))
#define DMA_OUTFIFO_PUSH_CH3_M (BIT(9))
#define DMA_OUTFIFO_PUSH_CH3_V 0x1
#define DMA_OUTFIFO_PUSH_CH3_S 9
/* DMA_OUTFIFO_WDATA_CH3 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO.*/
#define DMA_OUTFIFO_WDATA_CH3 0x000001FF
#define DMA_OUTFIFO_WDATA_CH3_M ((DMA_OUTFIFO_WDATA_CH3_V) << (DMA_OUTFIFO_WDATA_CH3_S))
#define DMA_OUTFIFO_WDATA_CH3_V 0x1FF
#define DMA_OUTFIFO_WDATA_CH3_S 0

#define DMA_OUT_PUSH_CH4_REG (DR_REG_DMA_BASE + 0x0B4)
/* DMA_OUTFIFO_PUSH_CH4 : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: Set this bit to push data into DMA FIFO.*/
#define DMA_OUTFIFO_PUSH_CH4 (BIT(9))
#define DMA_OUTFIFO_PUSH_CH4_M (BIT(9))
#define DMA_OUTFIFO_PUSH_CH4_V 0x1
#define DMA_OUTFIFO_PUSH_CH4_S 9
/* DMA_OUTFIFO_WDATA_CH4 : R/W ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: This register stores the data that need to be pushed into DMA FIFO.*/
#define DMA_OUTFIFO_WDATA_CH4 0x000001FF
#define DMA_OUTFIFO_WDATA_CH4_M ((DMA_OUTFIFO_WDATA_CH4_V) << (DMA_OUTFIFO_WDATA_CH4_S))
#define DMA_OUTFIFO_WDATA_CH4_V 0x1FF
#define DMA_OUTFIFO_WDATA_CH4_S 0

#define DMA_IN_POP_CH0_REG (DR_REG_DMA_BASE + 0x0B8)
/* DMA_INFIFO_POP_CH0 : R/W ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO.*/
#define DMA_INFIFO_POP_CH0 (BIT(12))
#define DMA_INFIFO_POP_CH0_M (BIT(12))
#define DMA_INFIFO_POP_CH0_V 0x1
#define DMA_INFIFO_POP_CH0_S 12
/* DMA_INFIFO_RDATA_CH0 : RO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: This register stores the data popping from DMA FIFO.*/
#define DMA_INFIFO_RDATA_CH0 0x00000FFF
#define DMA_INFIFO_RDATA_CH0_M ((DMA_INFIFO_RDATA_CH0_V) << (DMA_INFIFO_RDATA_CH0_S))
#define DMA_INFIFO_RDATA_CH0_V 0xFFF
#define DMA_INFIFO_RDATA_CH0_S 0

#define DMA_IN_POP_CH1_REG (DR_REG_DMA_BASE + 0x0BC)
/* DMA_INFIFO_POP_CH1 : R/W ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO.*/
#define DMA_INFIFO_POP_CH1 (BIT(12))
#define DMA_INFIFO_POP_CH1_M (BIT(12))
#define DMA_INFIFO_POP_CH1_V 0x1
#define DMA_INFIFO_POP_CH1_S 12
/* DMA_INFIFO_RDATA_CH1 : RO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: This register stores the data popping from DMA FIFO.*/
#define DMA_INFIFO_RDATA_CH1 0x00000FFF
#define DMA_INFIFO_RDATA_CH1_M ((DMA_INFIFO_RDATA_CH1_V) << (DMA_INFIFO_RDATA_CH1_S))
#define DMA_INFIFO_RDATA_CH1_V 0xFFF
#define DMA_INFIFO_RDATA_CH1_S 0

#define DMA_IN_POP_CH2_REG (DR_REG_DMA_BASE + 0x0C0)
/* DMA_INFIFO_POP_CH2 : R/W ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO.*/
#define DMA_INFIFO_POP_CH2 (BIT(12))
#define DMA_INFIFO_POP_CH2_M (BIT(12))
#define DMA_INFIFO_POP_CH2_V 0x1
#define DMA_INFIFO_POP_CH2_S 12
/* DMA_INFIFO_RDATA_CH2 : RO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: This register stores the data popping from DMA FIFO.*/
#define DMA_INFIFO_RDATA_CH2 0x00000FFF
#define DMA_INFIFO_RDATA_CH2_M ((DMA_INFIFO_RDATA_CH2_V) << (DMA_INFIFO_RDATA_CH2_S))
#define DMA_INFIFO_RDATA_CH2_V 0xFFF
#define DMA_INFIFO_RDATA_CH2_S 0

#define DMA_IN_POP_CH3_REG (DR_REG_DMA_BASE + 0x0C4)
/* DMA_INFIFO_POP_CH3 : R/W ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO.*/
#define DMA_INFIFO_POP_CH3 (BIT(12))
#define DMA_INFIFO_POP_CH3_M (BIT(12))
#define DMA_INFIFO_POP_CH3_V 0x1
#define DMA_INFIFO_POP_CH3_S 12
/* DMA_INFIFO_RDATA_CH3 : RO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: This register stores the data popping from DMA FIFO.*/
#define DMA_INFIFO_RDATA_CH3 0x00000FFF
#define DMA_INFIFO_RDATA_CH3_M ((DMA_INFIFO_RDATA_CH3_V) << (DMA_INFIFO_RDATA_CH3_S))
#define DMA_INFIFO_RDATA_CH3_V 0xFFF
#define DMA_INFIFO_RDATA_CH3_S 0

#define DMA_IN_POP_CH4_REG (DR_REG_DMA_BASE + 0x0C8)
/* DMA_INFIFO_POP_CH4 : R/W ;bitpos:[12] ;default: 1'h0 ; */
/*description: Set this bit to pop data from DMA FIFO.*/
#define DMA_INFIFO_POP_CH4 (BIT(12))
#define DMA_INFIFO_POP_CH4_M (BIT(12))
#define DMA_INFIFO_POP_CH4_V 0x1
#define DMA_INFIFO_POP_CH4_S 12
/* DMA_INFIFO_RDATA_CH4 : RO ;bitpos:[11:0] ;default: 12'h0 ; */
/*description: This register stores the data popping from DMA FIFO.*/
#define DMA_INFIFO_RDATA_CH4 0x00000FFF
#define DMA_INFIFO_RDATA_CH4_M ((DMA_INFIFO_RDATA_CH4_V) << (DMA_INFIFO_RDATA_CH4_S))
#define DMA_INFIFO_RDATA_CH4_V 0xFFF
#define DMA_INFIFO_RDATA_CH4_S 0

#define DMA_OUT_LINK_CH0_REG (DR_REG_DMA_BASE + 0x0CC)
/* DMA_OUTLINK_PARK_CH0 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink
 descriptor's FSM is working.*/
#define DMA_OUTLINK_PARK_CH0 (BIT(23))
#define DMA_OUTLINK_PARK_CH0_M (BIT(23))
#define DMA_OUTLINK_PARK_CH0_V 0x1
#define DMA_OUTLINK_PARK_CH0_S 23
/* DMA_OUTLINK_RESTART_CH0 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address.*/
#define DMA_OUTLINK_RESTART_CH0 (BIT(22))
#define DMA_OUTLINK_RESTART_CH0_M (BIT(22))
#define DMA_OUTLINK_RESTART_CH0_V 0x1
#define DMA_OUTLINK_RESTART_CH0_S 22
/* DMA_OUTLINK_START_CH0 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors.*/
#define DMA_OUTLINK_START_CH0 (BIT(21))
#define DMA_OUTLINK_START_CH0_M (BIT(21))
#define DMA_OUTLINK_START_CH0_V 0x1
#define DMA_OUTLINK_START_CH0_S 21
/* DMA_OUTLINK_STOP_CH0 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors.*/
#define DMA_OUTLINK_STOP_CH0 (BIT(20))
#define DMA_OUTLINK_STOP_CH0_M (BIT(20))
#define DMA_OUTLINK_STOP_CH0_V 0x1
#define DMA_OUTLINK_STOP_CH0_S 20
/* DMA_OUTLINK_ADDR_CH0 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 outlink descriptor's address.*/
#define DMA_OUTLINK_ADDR_CH0 0x000FFFFF
#define DMA_OUTLINK_ADDR_CH0_M ((DMA_OUTLINK_ADDR_CH0_V) << (DMA_OUTLINK_ADDR_CH0_S))
#define DMA_OUTLINK_ADDR_CH0_V 0xFFFFF
#define DMA_OUTLINK_ADDR_CH0_S 0

#define DMA_OUT_LINK_CH1_REG (DR_REG_DMA_BASE + 0x0D0)
/* DMA_OUTLINK_PARK_CH1 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink
 descriptor's FSM is working.*/
#define DMA_OUTLINK_PARK_CH1 (BIT(23))
#define DMA_OUTLINK_PARK_CH1_M (BIT(23))
#define DMA_OUTLINK_PARK_CH1_V 0x1
#define DMA_OUTLINK_PARK_CH1_S 23
/* DMA_OUTLINK_RESTART_CH1 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address.*/
#define DMA_OUTLINK_RESTART_CH1 (BIT(22))
#define DMA_OUTLINK_RESTART_CH1_M (BIT(22))
#define DMA_OUTLINK_RESTART_CH1_V 0x1
#define DMA_OUTLINK_RESTART_CH1_S 22
/* DMA_OUTLINK_START_CH1 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors.*/
#define DMA_OUTLINK_START_CH1 (BIT(21))
#define DMA_OUTLINK_START_CH1_M (BIT(21))
#define DMA_OUTLINK_START_CH1_V 0x1
#define DMA_OUTLINK_START_CH1_S 21
/* DMA_OUTLINK_STOP_CH1 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors.*/
#define DMA_OUTLINK_STOP_CH1 (BIT(20))
#define DMA_OUTLINK_STOP_CH1_M (BIT(20))
#define DMA_OUTLINK_STOP_CH1_V 0x1
#define DMA_OUTLINK_STOP_CH1_S 20
/* DMA_OUTLINK_ADDR_CH1 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 outlink descriptor's address.*/
#define DMA_OUTLINK_ADDR_CH1 0x000FFFFF
#define DMA_OUTLINK_ADDR_CH1_M ((DMA_OUTLINK_ADDR_CH1_V) << (DMA_OUTLINK_ADDR_CH1_S))
#define DMA_OUTLINK_ADDR_CH1_V 0xFFFFF
#define DMA_OUTLINK_ADDR_CH1_S 0

#define DMA_OUT_LINK_CH2_REG (DR_REG_DMA_BASE + 0x0D4)
/* DMA_OUTLINK_PARK_CH2 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink
 descriptor's FSM is working.*/
#define DMA_OUTLINK_PARK_CH2 (BIT(23))
#define DMA_OUTLINK_PARK_CH2_M (BIT(23))
#define DMA_OUTLINK_PARK_CH2_V 0x1
#define DMA_OUTLINK_PARK_CH2_S 23
/* DMA_OUTLINK_RESTART_CH2 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address.*/
#define DMA_OUTLINK_RESTART_CH2 (BIT(22))
#define DMA_OUTLINK_RESTART_CH2_M (BIT(22))
#define DMA_OUTLINK_RESTART_CH2_V 0x1
#define DMA_OUTLINK_RESTART_CH2_S 22
/* DMA_OUTLINK_START_CH2 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors.*/
#define DMA_OUTLINK_START_CH2 (BIT(21))
#define DMA_OUTLINK_START_CH2_M (BIT(21))
#define DMA_OUTLINK_START_CH2_V 0x1
#define DMA_OUTLINK_START_CH2_S 21
/* DMA_OUTLINK_STOP_CH2 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors.*/
#define DMA_OUTLINK_STOP_CH2 (BIT(20))
#define DMA_OUTLINK_STOP_CH2_M (BIT(20))
#define DMA_OUTLINK_STOP_CH2_V 0x1
#define DMA_OUTLINK_STOP_CH2_S 20
/* DMA_OUTLINK_ADDR_CH2 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 outlink descriptor's address.*/
#define DMA_OUTLINK_ADDR_CH2 0x000FFFFF
#define DMA_OUTLINK_ADDR_CH2_M ((DMA_OUTLINK_ADDR_CH2_V) << (DMA_OUTLINK_ADDR_CH2_S))
#define DMA_OUTLINK_ADDR_CH2_V 0xFFFFF
#define DMA_OUTLINK_ADDR_CH2_S 0

#define DMA_OUT_LINK_CH3_REG (DR_REG_DMA_BASE + 0x0D8)
/* DMA_OUTLINK_PARK_CH3 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink
 descriptor's FSM is working.*/
#define DMA_OUTLINK_PARK_CH3 (BIT(23))
#define DMA_OUTLINK_PARK_CH3_M (BIT(23))
#define DMA_OUTLINK_PARK_CH3_V 0x1
#define DMA_OUTLINK_PARK_CH3_S 23
/* DMA_OUTLINK_RESTART_CH3 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address.*/
#define DMA_OUTLINK_RESTART_CH3 (BIT(22))
#define DMA_OUTLINK_RESTART_CH3_M (BIT(22))
#define DMA_OUTLINK_RESTART_CH3_V 0x1
#define DMA_OUTLINK_RESTART_CH3_S 22
/* DMA_OUTLINK_START_CH3 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors.*/
#define DMA_OUTLINK_START_CH3 (BIT(21))
#define DMA_OUTLINK_START_CH3_M (BIT(21))
#define DMA_OUTLINK_START_CH3_V 0x1
#define DMA_OUTLINK_START_CH3_S 21
/* DMA_OUTLINK_STOP_CH3 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors.*/
#define DMA_OUTLINK_STOP_CH3 (BIT(20))
#define DMA_OUTLINK_STOP_CH3_M (BIT(20))
#define DMA_OUTLINK_STOP_CH3_V 0x1
#define DMA_OUTLINK_STOP_CH3_S 20
/* DMA_OUTLINK_ADDR_CH3 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 outlink descriptor's address.*/
#define DMA_OUTLINK_ADDR_CH3 0x000FFFFF
#define DMA_OUTLINK_ADDR_CH3_M ((DMA_OUTLINK_ADDR_CH3_V) << (DMA_OUTLINK_ADDR_CH3_S))
#define DMA_OUTLINK_ADDR_CH3_V 0xFFFFF
#define DMA_OUTLINK_ADDR_CH3_S 0

#define DMA_OUT_LINK_CH4_REG (DR_REG_DMA_BASE + 0x0DC)
/* DMA_OUTLINK_PARK_CH4 : RO ;bitpos:[23] ;default: 1'h1 ; */
/*description: 1: the outlink descriptor's FSM is in idle state.  0: the outlink
 descriptor's FSM is working.*/
#define DMA_OUTLINK_PARK_CH4 (BIT(23))
#define DMA_OUTLINK_PARK_CH4_M (BIT(23))
#define DMA_OUTLINK_PARK_CH4_V 0x1
#define DMA_OUTLINK_PARK_CH4_S 23
/* DMA_OUTLINK_RESTART_CH4 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to restart a new outlink from the last address.*/
#define DMA_OUTLINK_RESTART_CH4 (BIT(22))
#define DMA_OUTLINK_RESTART_CH4_M (BIT(22))
#define DMA_OUTLINK_RESTART_CH4_V 0x1
#define DMA_OUTLINK_RESTART_CH4_S 22
/* DMA_OUTLINK_START_CH4 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the outlink descriptors.*/
#define DMA_OUTLINK_START_CH4 (BIT(21))
#define DMA_OUTLINK_START_CH4_M (BIT(21))
#define DMA_OUTLINK_START_CH4_V 0x1
#define DMA_OUTLINK_START_CH4_S 21
/* DMA_OUTLINK_STOP_CH4 : R/W ;bitpos:[20] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the outlink descriptors.*/
#define DMA_OUTLINK_STOP_CH4 (BIT(20))
#define DMA_OUTLINK_STOP_CH4_M (BIT(20))
#define DMA_OUTLINK_STOP_CH4_V 0x1
#define DMA_OUTLINK_STOP_CH4_S 20
/* DMA_OUTLINK_ADDR_CH4 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 outlink descriptor's address.*/
#define DMA_OUTLINK_ADDR_CH4 0x000FFFFF
#define DMA_OUTLINK_ADDR_CH4_M ((DMA_OUTLINK_ADDR_CH4_V) << (DMA_OUTLINK_ADDR_CH4_S))
#define DMA_OUTLINK_ADDR_CH4_V 0xFFFFF
#define DMA_OUTLINK_ADDR_CH4_S 0

#define DMA_IN_LINK_CH0_REG (DR_REG_DMA_BASE + 0x0E0)
/* DMA_INLINK_PARK_CH0 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink
 descriptor's FSM is working.*/
#define DMA_INLINK_PARK_CH0 (BIT(24))
#define DMA_INLINK_PARK_CH0_M (BIT(24))
#define DMA_INLINK_PARK_CH0_V 0x1
#define DMA_INLINK_PARK_CH0_S 24
/* DMA_INLINK_RESTART_CH0 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor.*/
#define DMA_INLINK_RESTART_CH0 (BIT(23))
#define DMA_INLINK_RESTART_CH0_M (BIT(23))
#define DMA_INLINK_RESTART_CH0_V 0x1
#define DMA_INLINK_RESTART_CH0_S 23
/* DMA_INLINK_START_CH0 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors.*/
#define DMA_INLINK_START_CH0 (BIT(22))
#define DMA_INLINK_START_CH0_M (BIT(22))
#define DMA_INLINK_START_CH0_V 0x1
#define DMA_INLINK_START_CH0_S 22
/* DMA_INLINK_STOP_CH0 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors.*/
#define DMA_INLINK_STOP_CH0 (BIT(21))
#define DMA_INLINK_STOP_CH0_M (BIT(21))
#define DMA_INLINK_STOP_CH0_V 0x1
#define DMA_INLINK_STOP_CH0_S 21
/* DMA_INLINK_AUTO_RET_CH0 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address
  when there are some errors in current receiving data.*/
#define DMA_INLINK_AUTO_RET_CH0 (BIT(20))
#define DMA_INLINK_AUTO_RET_CH0_M (BIT(20))
#define DMA_INLINK_AUTO_RET_CH0_V 0x1
#define DMA_INLINK_AUTO_RET_CH0_S 20
/* DMA_INLINK_ADDR_CH0 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 inlink descriptor's address.*/
#define DMA_INLINK_ADDR_CH0 0x000FFFFF
#define DMA_INLINK_ADDR_CH0_M ((DMA_INLINK_ADDR_CH0_V) << (DMA_INLINK_ADDR_CH0_S))
#define DMA_INLINK_ADDR_CH0_V 0xFFFFF
#define DMA_INLINK_ADDR_CH0_S 0

#define DMA_IN_LINK_CH1_REG (DR_REG_DMA_BASE + 0x0E4)
/* DMA_INLINK_PARK_CH1 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink
 descriptor's FSM is working.*/
#define DMA_INLINK_PARK_CH1 (BIT(24))
#define DMA_INLINK_PARK_CH1_M (BIT(24))
#define DMA_INLINK_PARK_CH1_V 0x1
#define DMA_INLINK_PARK_CH1_S 24
/* DMA_INLINK_RESTART_CH1 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor.*/
#define DMA_INLINK_RESTART_CH1 (BIT(23))
#define DMA_INLINK_RESTART_CH1_M (BIT(23))
#define DMA_INLINK_RESTART_CH1_V 0x1
#define DMA_INLINK_RESTART_CH1_S 23
/* DMA_INLINK_START_CH1 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors.*/
#define DMA_INLINK_START_CH1 (BIT(22))
#define DMA_INLINK_START_CH1_M (BIT(22))
#define DMA_INLINK_START_CH1_V 0x1
#define DMA_INLINK_START_CH1_S 22
/* DMA_INLINK_STOP_CH1 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors.*/
#define DMA_INLINK_STOP_CH1 (BIT(21))
#define DMA_INLINK_STOP_CH1_M (BIT(21))
#define DMA_INLINK_STOP_CH1_V 0x1
#define DMA_INLINK_STOP_CH1_S 21
/* DMA_INLINK_AUTO_RET_CH1 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address
  when there are some errors in current receiving data.*/
#define DMA_INLINK_AUTO_RET_CH1 (BIT(20))
#define DMA_INLINK_AUTO_RET_CH1_M (BIT(20))
#define DMA_INLINK_AUTO_RET_CH1_V 0x1
#define DMA_INLINK_AUTO_RET_CH1_S 20
/* DMA_INLINK_ADDR_CH1 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 inlink descriptor's address.*/
#define DMA_INLINK_ADDR_CH1 0x000FFFFF
#define DMA_INLINK_ADDR_CH1_M ((DMA_INLINK_ADDR_CH1_V) << (DMA_INLINK_ADDR_CH1_S))
#define DMA_INLINK_ADDR_CH1_V 0xFFFFF
#define DMA_INLINK_ADDR_CH1_S 0

#define DMA_IN_LINK_CH2_REG (DR_REG_DMA_BASE + 0x0E8)
/* DMA_INLINK_PARK_CH2 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink
 descriptor's FSM is working.*/
#define DMA_INLINK_PARK_CH2 (BIT(24))
#define DMA_INLINK_PARK_CH2_M (BIT(24))
#define DMA_INLINK_PARK_CH2_V 0x1
#define DMA_INLINK_PARK_CH2_S 24
/* DMA_INLINK_RESTART_CH2 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor.*/
#define DMA_INLINK_RESTART_CH2 (BIT(23))
#define DMA_INLINK_RESTART_CH2_M (BIT(23))
#define DMA_INLINK_RESTART_CH2_V 0x1
#define DMA_INLINK_RESTART_CH2_S 23
/* DMA_INLINK_START_CH2 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors.*/
#define DMA_INLINK_START_CH2 (BIT(22))
#define DMA_INLINK_START_CH2_M (BIT(22))
#define DMA_INLINK_START_CH2_V 0x1
#define DMA_INLINK_START_CH2_S 22
/* DMA_INLINK_STOP_CH2 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors.*/
#define DMA_INLINK_STOP_CH2 (BIT(21))
#define DMA_INLINK_STOP_CH2_M (BIT(21))
#define DMA_INLINK_STOP_CH2_V 0x1
#define DMA_INLINK_STOP_CH2_S 21
/* DMA_INLINK_AUTO_RET_CH2 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address
  when there are some errors in current receiving data.*/
#define DMA_INLINK_AUTO_RET_CH2 (BIT(20))
#define DMA_INLINK_AUTO_RET_CH2_M (BIT(20))
#define DMA_INLINK_AUTO_RET_CH2_V 0x1
#define DMA_INLINK_AUTO_RET_CH2_S 20
/* DMA_INLINK_ADDR_CH2 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 inlink descriptor's address.*/
#define DMA_INLINK_ADDR_CH2 0x000FFFFF
#define DMA_INLINK_ADDR_CH2_M ((DMA_INLINK_ADDR_CH2_V) << (DMA_INLINK_ADDR_CH2_S))
#define DMA_INLINK_ADDR_CH2_V 0xFFFFF
#define DMA_INLINK_ADDR_CH2_S 0

#define DMA_IN_LINK_CH3_REG (DR_REG_DMA_BASE + 0x0EC)
/* DMA_INLINK_PARK_CH3 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink
 descriptor's FSM is working.*/
#define DMA_INLINK_PARK_CH3 (BIT(24))
#define DMA_INLINK_PARK_CH3_M (BIT(24))
#define DMA_INLINK_PARK_CH3_V 0x1
#define DMA_INLINK_PARK_CH3_S 24
/* DMA_INLINK_RESTART_CH3 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor.*/
#define DMA_INLINK_RESTART_CH3 (BIT(23))
#define DMA_INLINK_RESTART_CH3_M (BIT(23))
#define DMA_INLINK_RESTART_CH3_V 0x1
#define DMA_INLINK_RESTART_CH3_S 23
/* DMA_INLINK_START_CH3 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors.*/
#define DMA_INLINK_START_CH3 (BIT(22))
#define DMA_INLINK_START_CH3_M (BIT(22))
#define DMA_INLINK_START_CH3_V 0x1
#define DMA_INLINK_START_CH3_S 22
/* DMA_INLINK_STOP_CH3 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors.*/
#define DMA_INLINK_STOP_CH3 (BIT(21))
#define DMA_INLINK_STOP_CH3_M (BIT(21))
#define DMA_INLINK_STOP_CH3_V 0x1
#define DMA_INLINK_STOP_CH3_S 21
/* DMA_INLINK_AUTO_RET_CH3 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address
  when there are some errors in current receiving data.*/
#define DMA_INLINK_AUTO_RET_CH3 (BIT(20))
#define DMA_INLINK_AUTO_RET_CH3_M (BIT(20))
#define DMA_INLINK_AUTO_RET_CH3_V 0x1
#define DMA_INLINK_AUTO_RET_CH3_S 20
/* DMA_INLINK_ADDR_CH3 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 inlink descriptor's address.*/
#define DMA_INLINK_ADDR_CH3 0x000FFFFF
#define DMA_INLINK_ADDR_CH3_M ((DMA_INLINK_ADDR_CH3_V) << (DMA_INLINK_ADDR_CH3_S))
#define DMA_INLINK_ADDR_CH3_V 0xFFFFF
#define DMA_INLINK_ADDR_CH3_S 0

#define DMA_IN_LINK_CH4_REG (DR_REG_DMA_BASE + 0x0F0)
/* DMA_INLINK_PARK_CH4 : RO ;bitpos:[24] ;default: 1'h1 ; */
/*description: 1: the inlink descriptor's FSM is in idle state.  0: the inlink
 descriptor's FSM is working.*/
#define DMA_INLINK_PARK_CH4 (BIT(24))
#define DMA_INLINK_PARK_CH4_M (BIT(24))
#define DMA_INLINK_PARK_CH4_V 0x1
#define DMA_INLINK_PARK_CH4_S 24
/* DMA_INLINK_RESTART_CH4 : R/W ;bitpos:[23] ;default: 1'b0 ; */
/*description: Set this bit to mount a new inlink descriptor.*/
#define DMA_INLINK_RESTART_CH4 (BIT(23))
#define DMA_INLINK_RESTART_CH4_M (BIT(23))
#define DMA_INLINK_RESTART_CH4_V 0x1
#define DMA_INLINK_RESTART_CH4_S 23
/* DMA_INLINK_START_CH4 : R/W ;bitpos:[22] ;default: 1'b0 ; */
/*description: Set this bit to start dealing with the inlink descriptors.*/
#define DMA_INLINK_START_CH4 (BIT(22))
#define DMA_INLINK_START_CH4_M (BIT(22))
#define DMA_INLINK_START_CH4_V 0x1
#define DMA_INLINK_START_CH4_S 22
/* DMA_INLINK_STOP_CH4 : R/W ;bitpos:[21] ;default: 1'b0 ; */
/*description: Set this bit to stop dealing with the inlink descriptors.*/
#define DMA_INLINK_STOP_CH4 (BIT(21))
#define DMA_INLINK_STOP_CH4_M (BIT(21))
#define DMA_INLINK_STOP_CH4_V 0x1
#define DMA_INLINK_STOP_CH4_S 21
/* DMA_INLINK_AUTO_RET_CH4 : R/W ;bitpos:[20] ;default: 1'b1 ; */
/*description: Set this bit to return to current inlink descriptor's address
  when there are some errors in current receiving data.*/
#define DMA_INLINK_AUTO_RET_CH4 (BIT(20))
#define DMA_INLINK_AUTO_RET_CH4_M (BIT(20))
#define DMA_INLINK_AUTO_RET_CH4_V 0x1
#define DMA_INLINK_AUTO_RET_CH4_S 20
/* DMA_INLINK_ADDR_CH4 : R/W ;bitpos:[19:0] ;default: 20'h0 ; */
/*description: This register stores the 20 least significant bits of the first
 inlink descriptor's address.*/
#define DMA_INLINK_ADDR_CH4 0x000FFFFF
#define DMA_INLINK_ADDR_CH4_M ((DMA_INLINK_ADDR_CH4_V) << (DMA_INLINK_ADDR_CH4_S))
#define DMA_INLINK_ADDR_CH4_V 0xFFFFF
#define DMA_INLINK_ADDR_CH4_S 0

#define DMA_IN_STATE_CH0_REG (DR_REG_DMA_BASE + 0x0F4)
/* DMA_IN_STATE_CH0 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_IN_STATE_CH0 0x00000007
#define DMA_IN_STATE_CH0_M ((DMA_IN_STATE_CH0_V) << (DMA_IN_STATE_CH0_S))
#define DMA_IN_STATE_CH0_V 0x7
#define DMA_IN_STATE_CH0_S 20
/* DMA_IN_DSCR_STATE_CH0 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_IN_DSCR_STATE_CH0 0x00000003
#define DMA_IN_DSCR_STATE_CH0_M ((DMA_IN_DSCR_STATE_CH0_V) << (DMA_IN_DSCR_STATE_CH0_S))
#define DMA_IN_DSCR_STATE_CH0_V 0x3
#define DMA_IN_DSCR_STATE_CH0_S 18
/* DMA_INLINK_DSCR_ADDR_CH0 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address.*/
#define DMA_INLINK_DSCR_ADDR_CH0 0x0003FFFF
#define DMA_INLINK_DSCR_ADDR_CH0_M ((DMA_INLINK_DSCR_ADDR_CH0_V) << (DMA_INLINK_DSCR_ADDR_CH0_S))
#define DMA_INLINK_DSCR_ADDR_CH0_V 0x3FFFF
#define DMA_INLINK_DSCR_ADDR_CH0_S 0

#define DMA_IN_STATE_CH1_REG (DR_REG_DMA_BASE + 0x0F8)
/* DMA_IN_STATE_CH1 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_IN_STATE_CH1 0x00000007
#define DMA_IN_STATE_CH1_M ((DMA_IN_STATE_CH1_V) << (DMA_IN_STATE_CH1_S))
#define DMA_IN_STATE_CH1_V 0x7
#define DMA_IN_STATE_CH1_S 20
/* DMA_IN_DSCR_STATE_CH1 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_IN_DSCR_STATE_CH1 0x00000003
#define DMA_IN_DSCR_STATE_CH1_M ((DMA_IN_DSCR_STATE_CH1_V) << (DMA_IN_DSCR_STATE_CH1_S))
#define DMA_IN_DSCR_STATE_CH1_V 0x3
#define DMA_IN_DSCR_STATE_CH1_S 18
/* DMA_INLINK_DSCR_ADDR_CH1 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address.*/
#define DMA_INLINK_DSCR_ADDR_CH1 0x0003FFFF
#define DMA_INLINK_DSCR_ADDR_CH1_M ((DMA_INLINK_DSCR_ADDR_CH1_V) << (DMA_INLINK_DSCR_ADDR_CH1_S))
#define DMA_INLINK_DSCR_ADDR_CH1_V 0x3FFFF
#define DMA_INLINK_DSCR_ADDR_CH1_S 0

#define DMA_IN_STATE_CH2_REG (DR_REG_DMA_BASE + 0x0FC)
/* DMA_IN_STATE_CH2 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_IN_STATE_CH2 0x00000007
#define DMA_IN_STATE_CH2_M ((DMA_IN_STATE_CH2_V) << (DMA_IN_STATE_CH2_S))
#define DMA_IN_STATE_CH2_V 0x7
#define DMA_IN_STATE_CH2_S 20
/* DMA_IN_DSCR_STATE_CH2 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_IN_DSCR_STATE_CH2 0x00000003
#define DMA_IN_DSCR_STATE_CH2_M ((DMA_IN_DSCR_STATE_CH2_V) << (DMA_IN_DSCR_STATE_CH2_S))
#define DMA_IN_DSCR_STATE_CH2_V 0x3
#define DMA_IN_DSCR_STATE_CH2_S 18
/* DMA_INLINK_DSCR_ADDR_CH2 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address.*/
#define DMA_INLINK_DSCR_ADDR_CH2 0x0003FFFF
#define DMA_INLINK_DSCR_ADDR_CH2_M ((DMA_INLINK_DSCR_ADDR_CH2_V) << (DMA_INLINK_DSCR_ADDR_CH2_S))
#define DMA_INLINK_DSCR_ADDR_CH2_V 0x3FFFF
#define DMA_INLINK_DSCR_ADDR_CH2_S 0

#define DMA_IN_STATE_CH3_REG (DR_REG_DMA_BASE + 0x100)
/* DMA_IN_STATE_CH3 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_IN_STATE_CH3 0x00000007
#define DMA_IN_STATE_CH3_M ((DMA_IN_STATE_CH3_V) << (DMA_IN_STATE_CH3_S))
#define DMA_IN_STATE_CH3_V 0x7
#define DMA_IN_STATE_CH3_S 20
/* DMA_IN_DSCR_STATE_CH3 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_IN_DSCR_STATE_CH3 0x00000003
#define DMA_IN_DSCR_STATE_CH3_M ((DMA_IN_DSCR_STATE_CH3_V) << (DMA_IN_DSCR_STATE_CH3_S))
#define DMA_IN_DSCR_STATE_CH3_V 0x3
#define DMA_IN_DSCR_STATE_CH3_S 18
/* DMA_INLINK_DSCR_ADDR_CH3 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address.*/
#define DMA_INLINK_DSCR_ADDR_CH3 0x0003FFFF
#define DMA_INLINK_DSCR_ADDR_CH3_M ((DMA_INLINK_DSCR_ADDR_CH3_V) << (DMA_INLINK_DSCR_ADDR_CH3_S))
#define DMA_INLINK_DSCR_ADDR_CH3_V 0x3FFFF
#define DMA_INLINK_DSCR_ADDR_CH3_S 0

#define DMA_IN_STATE_CH4_REG (DR_REG_DMA_BASE + 0x104)
/* DMA_IN_STATE_CH4 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_IN_STATE_CH4 0x00000007
#define DMA_IN_STATE_CH4_M ((DMA_IN_STATE_CH4_V) << (DMA_IN_STATE_CH4_S))
#define DMA_IN_STATE_CH4_V 0x7
#define DMA_IN_STATE_CH4_S 20
/* DMA_IN_DSCR_STATE_CH4 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_IN_DSCR_STATE_CH4 0x00000003
#define DMA_IN_DSCR_STATE_CH4_M ((DMA_IN_DSCR_STATE_CH4_V) << (DMA_IN_DSCR_STATE_CH4_S))
#define DMA_IN_DSCR_STATE_CH4_V 0x3
#define DMA_IN_DSCR_STATE_CH4_S 18
/* DMA_INLINK_DSCR_ADDR_CH4 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current inlink descriptor's address.*/
#define DMA_INLINK_DSCR_ADDR_CH4 0x0003FFFF
#define DMA_INLINK_DSCR_ADDR_CH4_M ((DMA_INLINK_DSCR_ADDR_CH4_V) << (DMA_INLINK_DSCR_ADDR_CH4_S))
#define DMA_INLINK_DSCR_ADDR_CH4_V 0x3FFFF
#define DMA_INLINK_DSCR_ADDR_CH4_S 0

#define DMA_OUT_STATE_CH0_REG (DR_REG_DMA_BASE + 0x108)
/* DMA_OUT_STATE_CH0 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_OUT_STATE_CH0 0x00000007
#define DMA_OUT_STATE_CH0_M ((DMA_OUT_STATE_CH0_V) << (DMA_OUT_STATE_CH0_S))
#define DMA_OUT_STATE_CH0_V 0x7
#define DMA_OUT_STATE_CH0_S 20
/* DMA_OUT_DSCR_STATE_CH0 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_OUT_DSCR_STATE_CH0 0x00000003
#define DMA_OUT_DSCR_STATE_CH0_M ((DMA_OUT_DSCR_STATE_CH0_V) << (DMA_OUT_DSCR_STATE_CH0_S))
#define DMA_OUT_DSCR_STATE_CH0_V 0x3
#define DMA_OUT_DSCR_STATE_CH0_S 18
/* DMA_OUTLINK_DSCR_ADDR_CH0 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address.*/
#define DMA_OUTLINK_DSCR_ADDR_CH0 0x0003FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH0_M ((DMA_OUTLINK_DSCR_ADDR_CH0_V) << (DMA_OUTLINK_DSCR_ADDR_CH0_S))
#define DMA_OUTLINK_DSCR_ADDR_CH0_V 0x3FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH0_S 0

#define DMA_OUT_STATE_CH1_REG (DR_REG_DMA_BASE + 0x10C)
/* DMA_OUT_STATE_CH1 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_OUT_STATE_CH1 0x00000007
#define DMA_OUT_STATE_CH1_M ((DMA_OUT_STATE_CH1_V) << (DMA_OUT_STATE_CH1_S))
#define DMA_OUT_STATE_CH1_V 0x7
#define DMA_OUT_STATE_CH1_S 20
/* DMA_OUT_DSCR_STATE_CH1 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_OUT_DSCR_STATE_CH1 0x00000003
#define DMA_OUT_DSCR_STATE_CH1_M ((DMA_OUT_DSCR_STATE_CH1_V) << (DMA_OUT_DSCR_STATE_CH1_S))
#define DMA_OUT_DSCR_STATE_CH1_V 0x3
#define DMA_OUT_DSCR_STATE_CH1_S 18
/* DMA_OUTLINK_DSCR_ADDR_CH1 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address.*/
#define DMA_OUTLINK_DSCR_ADDR_CH1 0x0003FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH1_M ((DMA_OUTLINK_DSCR_ADDR_CH1_V) << (DMA_OUTLINK_DSCR_ADDR_CH1_S))
#define DMA_OUTLINK_DSCR_ADDR_CH1_V 0x3FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH1_S 0

#define DMA_OUT_STATE_CH2_REG (DR_REG_DMA_BASE + 0x110)
/* DMA_OUT_STATE_CH2 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_OUT_STATE_CH2 0x00000007
#define DMA_OUT_STATE_CH2_M ((DMA_OUT_STATE_CH2_V) << (DMA_OUT_STATE_CH2_S))
#define DMA_OUT_STATE_CH2_V 0x7
#define DMA_OUT_STATE_CH2_S 20
/* DMA_OUT_DSCR_STATE_CH2 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_OUT_DSCR_STATE_CH2 0x00000003
#define DMA_OUT_DSCR_STATE_CH2_M ((DMA_OUT_DSCR_STATE_CH2_V) << (DMA_OUT_DSCR_STATE_CH2_S))
#define DMA_OUT_DSCR_STATE_CH2_V 0x3
#define DMA_OUT_DSCR_STATE_CH2_S 18
/* DMA_OUTLINK_DSCR_ADDR_CH2 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address.*/
#define DMA_OUTLINK_DSCR_ADDR_CH2 0x0003FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH2_M ((DMA_OUTLINK_DSCR_ADDR_CH2_V) << (DMA_OUTLINK_DSCR_ADDR_CH2_S))
#define DMA_OUTLINK_DSCR_ADDR_CH2_V 0x3FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH2_S 0

#define DMA_OUT_STATE_CH3_REG (DR_REG_DMA_BASE + 0x114)
/* DMA_OUT_STATE_CH3 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_OUT_STATE_CH3 0x00000007
#define DMA_OUT_STATE_CH3_M ((DMA_OUT_STATE_CH3_V) << (DMA_OUT_STATE_CH3_S))
#define DMA_OUT_STATE_CH3_V 0x7
#define DMA_OUT_STATE_CH3_S 20
/* DMA_OUT_DSCR_STATE_CH3 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_OUT_DSCR_STATE_CH3 0x00000003
#define DMA_OUT_DSCR_STATE_CH3_M ((DMA_OUT_DSCR_STATE_CH3_V) << (DMA_OUT_DSCR_STATE_CH3_S))
#define DMA_OUT_DSCR_STATE_CH3_V 0x3
#define DMA_OUT_DSCR_STATE_CH3_S 18
/* DMA_OUTLINK_DSCR_ADDR_CH3 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address.*/
#define DMA_OUTLINK_DSCR_ADDR_CH3 0x0003FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH3_M ((DMA_OUTLINK_DSCR_ADDR_CH3_V) << (DMA_OUTLINK_DSCR_ADDR_CH3_S))
#define DMA_OUTLINK_DSCR_ADDR_CH3_V 0x3FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH3_S 0

#define DMA_OUT_STATE_CH4_REG (DR_REG_DMA_BASE + 0x118)
/* DMA_OUT_STATE_CH4 : RO ;bitpos:[22:20] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_OUT_STATE_CH4 0x00000007
#define DMA_OUT_STATE_CH4_M ((DMA_OUT_STATE_CH4_V) << (DMA_OUT_STATE_CH4_S))
#define DMA_OUT_STATE_CH4_V 0x7
#define DMA_OUT_STATE_CH4_S 20
/* DMA_OUT_DSCR_STATE_CH4 : RO ;bitpos:[19:18] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_OUT_DSCR_STATE_CH4 0x00000003
#define DMA_OUT_DSCR_STATE_CH4_M ((DMA_OUT_DSCR_STATE_CH4_V) << (DMA_OUT_DSCR_STATE_CH4_S))
#define DMA_OUT_DSCR_STATE_CH4_V 0x3
#define DMA_OUT_DSCR_STATE_CH4_S 18
/* DMA_OUTLINK_DSCR_ADDR_CH4 : RO ;bitpos:[17:0] ;default: 18'b0 ; */
/*description: This register stores the current outlink descriptor's address.*/
#define DMA_OUTLINK_DSCR_ADDR_CH4 0x0003FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH4_M ((DMA_OUTLINK_DSCR_ADDR_CH4_V) << (DMA_OUTLINK_DSCR_ADDR_CH4_S))
#define DMA_OUTLINK_DSCR_ADDR_CH4_V 0x3FFFF
#define DMA_OUTLINK_DSCR_ADDR_CH4_S 0

#define DMA_OUT_EOF_DES_ADDR_CH0_REG (DR_REG_DMA_BASE + 0x11C)
/* DMA_OUT_EOF_DES_ADDR_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_OUT_EOF_DES_ADDR_CH0 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH0_M ((DMA_OUT_EOF_DES_ADDR_CH0_V) << (DMA_OUT_EOF_DES_ADDR_CH0_S))
#define DMA_OUT_EOF_DES_ADDR_CH0_V 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH0_S 0

#define DMA_OUT_EOF_DES_ADDR_CH1_REG (DR_REG_DMA_BASE + 0x120)
/* DMA_OUT_EOF_DES_ADDR_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_OUT_EOF_DES_ADDR_CH1 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH1_M ((DMA_OUT_EOF_DES_ADDR_CH1_V) << (DMA_OUT_EOF_DES_ADDR_CH1_S))
#define DMA_OUT_EOF_DES_ADDR_CH1_V 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH1_S 0

#define DMA_OUT_EOF_DES_ADDR_CH2_REG (DR_REG_DMA_BASE + 0x124)
/* DMA_OUT_EOF_DES_ADDR_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_OUT_EOF_DES_ADDR_CH2 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH2_M ((DMA_OUT_EOF_DES_ADDR_CH2_V) << (DMA_OUT_EOF_DES_ADDR_CH2_S))
#define DMA_OUT_EOF_DES_ADDR_CH2_V 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH2_S 0

#define DMA_OUT_EOF_DES_ADDR_CH3_REG (DR_REG_DMA_BASE + 0x128)
/* DMA_OUT_EOF_DES_ADDR_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_OUT_EOF_DES_ADDR_CH3 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH3_M ((DMA_OUT_EOF_DES_ADDR_CH3_V) << (DMA_OUT_EOF_DES_ADDR_CH3_S))
#define DMA_OUT_EOF_DES_ADDR_CH3_V 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH3_S 0

#define DMA_OUT_EOF_DES_ADDR_CH4_REG (DR_REG_DMA_BASE + 0x12C)
/* DMA_OUT_EOF_DES_ADDR_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_OUT_EOF_DES_ADDR_CH4 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH4_M ((DMA_OUT_EOF_DES_ADDR_CH4_V) << (DMA_OUT_EOF_DES_ADDR_CH4_S))
#define DMA_OUT_EOF_DES_ADDR_CH4_V 0xFFFFFFFF
#define DMA_OUT_EOF_DES_ADDR_CH4_S 0

#define DMA_IN_SUC_EOF_DES_ADDR_CH0_REG (DR_REG_DMA_BASE + 0x130)
/* DMA_IN_SUC_EOF_DES_ADDR_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_IN_SUC_EOF_DES_ADDR_CH0 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH0_M ((DMA_IN_SUC_EOF_DES_ADDR_CH0_V) << (DMA_IN_SUC_EOF_DES_ADDR_CH0_S))
#define DMA_IN_SUC_EOF_DES_ADDR_CH0_V 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH0_S 0

#define DMA_IN_SUC_EOF_DES_ADDR_CH1_REG (DR_REG_DMA_BASE + 0x134)
/* DMA_IN_SUC_EOF_DES_ADDR_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_IN_SUC_EOF_DES_ADDR_CH1 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH1_M ((DMA_IN_SUC_EOF_DES_ADDR_CH1_V) << (DMA_IN_SUC_EOF_DES_ADDR_CH1_S))
#define DMA_IN_SUC_EOF_DES_ADDR_CH1_V 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH1_S 0

#define DMA_IN_SUC_EOF_DES_ADDR_CH2_REG (DR_REG_DMA_BASE + 0x138)
/* DMA_IN_SUC_EOF_DES_ADDR_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_IN_SUC_EOF_DES_ADDR_CH2 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH2_M ((DMA_IN_SUC_EOF_DES_ADDR_CH2_V) << (DMA_IN_SUC_EOF_DES_ADDR_CH2_S))
#define DMA_IN_SUC_EOF_DES_ADDR_CH2_V 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH2_S 0

#define DMA_IN_SUC_EOF_DES_ADDR_CH3_REG (DR_REG_DMA_BASE + 0x13C)
/* DMA_IN_SUC_EOF_DES_ADDR_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_IN_SUC_EOF_DES_ADDR_CH3 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH3_M ((DMA_IN_SUC_EOF_DES_ADDR_CH3_V) << (DMA_IN_SUC_EOF_DES_ADDR_CH3_S))
#define DMA_IN_SUC_EOF_DES_ADDR_CH3_V 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH3_S 0

#define DMA_IN_SUC_EOF_DES_ADDR_CH4_REG (DR_REG_DMA_BASE + 0x140)
/* DMA_IN_SUC_EOF_DES_ADDR_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 the EOF bit in this descriptor is 1.*/
#define DMA_IN_SUC_EOF_DES_ADDR_CH4 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH4_M ((DMA_IN_SUC_EOF_DES_ADDR_CH4_V) << (DMA_IN_SUC_EOF_DES_ADDR_CH4_S))
#define DMA_IN_SUC_EOF_DES_ADDR_CH4_V 0xFFFFFFFF
#define DMA_IN_SUC_EOF_DES_ADDR_CH4_S 0

#define DMA_IN_ERR_EOF_DES_ADDR_CH0_REG (DR_REG_DMA_BASE + 0x144)
/* DMA_IN_ERR_EOF_DES_ADDR_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
#define DMA_IN_ERR_EOF_DES_ADDR_CH0 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH0_M ((DMA_IN_ERR_EOF_DES_ADDR_CH0_V) << (DMA_IN_ERR_EOF_DES_ADDR_CH0_S))
#define DMA_IN_ERR_EOF_DES_ADDR_CH0_V 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH0_S 0

#define DMA_IN_ERR_EOF_DES_ADDR_CH1_REG (DR_REG_DMA_BASE + 0x148)
/* DMA_IN_ERR_EOF_DES_ADDR_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
#define DMA_IN_ERR_EOF_DES_ADDR_CH1 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH1_M ((DMA_IN_ERR_EOF_DES_ADDR_CH1_V) << (DMA_IN_ERR_EOF_DES_ADDR_CH1_S))
#define DMA_IN_ERR_EOF_DES_ADDR_CH1_V 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH1_S 0

#define DMA_IN_ERR_EOF_DES_ADDR_CH2_REG (DR_REG_DMA_BASE + 0x14C)
/* DMA_IN_ERR_EOF_DES_ADDR_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
#define DMA_IN_ERR_EOF_DES_ADDR_CH2 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH2_M ((DMA_IN_ERR_EOF_DES_ADDR_CH2_V) << (DMA_IN_ERR_EOF_DES_ADDR_CH2_S))
#define DMA_IN_ERR_EOF_DES_ADDR_CH2_V 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH2_S 0

#define DMA_IN_ERR_EOF_DES_ADDR_CH3_REG (DR_REG_DMA_BASE + 0x150)
/* DMA_IN_ERR_EOF_DES_ADDR_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
#define DMA_IN_ERR_EOF_DES_ADDR_CH3 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH3_M ((DMA_IN_ERR_EOF_DES_ADDR_CH3_V) << (DMA_IN_ERR_EOF_DES_ADDR_CH3_S))
#define DMA_IN_ERR_EOF_DES_ADDR_CH3_V 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH3_S 0

#define DMA_IN_ERR_EOF_DES_ADDR_CH4_REG (DR_REG_DMA_BASE + 0x154)
/* DMA_IN_ERR_EOF_DES_ADDR_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the inlink descriptor when
 there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
#define DMA_IN_ERR_EOF_DES_ADDR_CH4 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH4_M ((DMA_IN_ERR_EOF_DES_ADDR_CH4_V) << (DMA_IN_ERR_EOF_DES_ADDR_CH4_S))
#define DMA_IN_ERR_EOF_DES_ADDR_CH4_V 0xFFFFFFFF
#define DMA_IN_ERR_EOF_DES_ADDR_CH4_S 0

#define DMA_OUT_EOF_BFR_DES_ADDR_CH0_REG (DR_REG_DMA_BASE + 0x158)
/* DMA_OUT_EOF_BFR_DES_ADDR_CH0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before
 the last outlink descriptor.*/
#define DMA_OUT_EOF_BFR_DES_ADDR_CH0 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH0_M ((DMA_OUT_EOF_BFR_DES_ADDR_CH0_V) << (DMA_OUT_EOF_BFR_DES_ADDR_CH0_S))
#define DMA_OUT_EOF_BFR_DES_ADDR_CH0_V 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH0_S 0

#define DMA_OUT_EOF_BFR_DES_ADDR_CH1_REG (DR_REG_DMA_BASE + 0x15C)
/* DMA_OUT_EOF_BFR_DES_ADDR_CH1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before
 the last outlink descriptor.*/
#define DMA_OUT_EOF_BFR_DES_ADDR_CH1 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH1_M ((DMA_OUT_EOF_BFR_DES_ADDR_CH1_V) << (DMA_OUT_EOF_BFR_DES_ADDR_CH1_S))
#define DMA_OUT_EOF_BFR_DES_ADDR_CH1_V 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH1_S 0

#define DMA_OUT_EOF_BFR_DES_ADDR_CH2_REG (DR_REG_DMA_BASE + 0x160)
/* DMA_OUT_EOF_BFR_DES_ADDR_CH2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before
 the last outlink descriptor.*/
#define DMA_OUT_EOF_BFR_DES_ADDR_CH2 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH2_M ((DMA_OUT_EOF_BFR_DES_ADDR_CH2_V) << (DMA_OUT_EOF_BFR_DES_ADDR_CH2_S))
#define DMA_OUT_EOF_BFR_DES_ADDR_CH2_V 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH2_S 0

#define DMA_OUT_EOF_BFR_DES_ADDR_CH3_REG (DR_REG_DMA_BASE + 0x164)
/* DMA_OUT_EOF_BFR_DES_ADDR_CH3 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before
 the last outlink descriptor.*/
#define DMA_OUT_EOF_BFR_DES_ADDR_CH3 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH3_M ((DMA_OUT_EOF_BFR_DES_ADDR_CH3_V) << (DMA_OUT_EOF_BFR_DES_ADDR_CH3_S))
#define DMA_OUT_EOF_BFR_DES_ADDR_CH3_V 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH3_S 0

#define DMA_OUT_EOF_BFR_DES_ADDR_CH4_REG (DR_REG_DMA_BASE + 0x168)
/* DMA_OUT_EOF_BFR_DES_ADDR_CH4 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This register stores the address of the outlink descriptor before
 the last outlink descriptor.*/
#define DMA_OUT_EOF_BFR_DES_ADDR_CH4 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH4_M ((DMA_OUT_EOF_BFR_DES_ADDR_CH4_V) << (DMA_OUT_EOF_BFR_DES_ADDR_CH4_S))
#define DMA_OUT_EOF_BFR_DES_ADDR_CH4_V 0xFFFFFFFF
#define DMA_OUT_EOF_BFR_DES_ADDR_CH4_S 0

#define DMA_AHB_TEST_REG (DR_REG_DMA_BASE + 0x16C)
/* DMA_AHB_TESTADDR : R/W ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: reserved*/
#define DMA_AHB_TESTADDR 0x00000003
#define DMA_AHB_TESTADDR_M ((DMA_AHB_TESTADDR_V) << (DMA_AHB_TESTADDR_S))
#define DMA_AHB_TESTADDR_V 0x3
#define DMA_AHB_TESTADDR_S 4
/* DMA_AHB_TESTMODE : R/W ;bitpos:[2:0] ;default: 3'b0 ; */
/*description: reserved*/
#define DMA_AHB_TESTMODE 0x00000007
#define DMA_AHB_TESTMODE_M ((DMA_AHB_TESTMODE_V) << (DMA_AHB_TESTMODE_S))
#define DMA_AHB_TESTMODE_V 0x7
#define DMA_AHB_TESTMODE_S 0

#define DMA_IN_DSCR_CH0_REG (DR_REG_DMA_BASE + 0x170)
/* DMA_INLINK_DSCR_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x.*/
#define DMA_INLINK_DSCR_CH0 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH0_M ((DMA_INLINK_DSCR_CH0_V) << (DMA_INLINK_DSCR_CH0_S))
#define DMA_INLINK_DSCR_CH0_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH0_S 0

#define DMA_IN_DSCR_CH1_REG (DR_REG_DMA_BASE + 0x174)
/* DMA_INLINK_DSCR_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x.*/
#define DMA_INLINK_DSCR_CH1 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH1_M ((DMA_INLINK_DSCR_CH1_V) << (DMA_INLINK_DSCR_CH1_S))
#define DMA_INLINK_DSCR_CH1_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH1_S 0

#define DMA_IN_DSCR_CH2_REG (DR_REG_DMA_BASE + 0x178)
/* DMA_INLINK_DSCR_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x.*/
#define DMA_INLINK_DSCR_CH2 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH2_M ((DMA_INLINK_DSCR_CH2_V) << (DMA_INLINK_DSCR_CH2_S))
#define DMA_INLINK_DSCR_CH2_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH2_S 0

#define DMA_IN_DSCR_CH3_REG (DR_REG_DMA_BASE + 0x17C)
/* DMA_INLINK_DSCR_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x.*/
#define DMA_INLINK_DSCR_CH3 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH3_M ((DMA_INLINK_DSCR_CH3_V) << (DMA_INLINK_DSCR_CH3_S))
#define DMA_INLINK_DSCR_CH3_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH3_S 0

#define DMA_IN_DSCR_CH4_REG (DR_REG_DMA_BASE + 0x180)
/* DMA_INLINK_DSCR_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current inlink descriptor x.*/
#define DMA_INLINK_DSCR_CH4 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH4_M ((DMA_INLINK_DSCR_CH4_V) << (DMA_INLINK_DSCR_CH4_S))
#define DMA_INLINK_DSCR_CH4_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_CH4_S 0

#define DMA_IN_DSCR_BF0_CH0_REG (DR_REG_DMA_BASE + 0x184)
/* DMA_INLINK_DSCR_BF0_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1.*/
#define DMA_INLINK_DSCR_BF0_CH0 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH0_M ((DMA_INLINK_DSCR_BF0_CH0_V) << (DMA_INLINK_DSCR_BF0_CH0_S))
#define DMA_INLINK_DSCR_BF0_CH0_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH0_S 0

#define DMA_IN_DSCR_BF0_CH1_REG (DR_REG_DMA_BASE + 0x188)
/* DMA_INLINK_DSCR_BF0_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1.*/
#define DMA_INLINK_DSCR_BF0_CH1 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH1_M ((DMA_INLINK_DSCR_BF0_CH1_V) << (DMA_INLINK_DSCR_BF0_CH1_S))
#define DMA_INLINK_DSCR_BF0_CH1_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH1_S 0

#define DMA_IN_DSCR_BF0_CH2_REG (DR_REG_DMA_BASE + 0x18C)
/* DMA_INLINK_DSCR_BF0_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1.*/
#define DMA_INLINK_DSCR_BF0_CH2 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH2_M ((DMA_INLINK_DSCR_BF0_CH2_V) << (DMA_INLINK_DSCR_BF0_CH2_S))
#define DMA_INLINK_DSCR_BF0_CH2_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH2_S 0

#define DMA_IN_DSCR_BF0_CH3_REG (DR_REG_DMA_BASE + 0x190)
/* DMA_INLINK_DSCR_BF0_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1.*/
#define DMA_INLINK_DSCR_BF0_CH3 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH3_M ((DMA_INLINK_DSCR_BF0_CH3_V) << (DMA_INLINK_DSCR_BF0_CH3_S))
#define DMA_INLINK_DSCR_BF0_CH3_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH3_S 0

#define DMA_IN_DSCR_BF0_CH4_REG (DR_REG_DMA_BASE + 0x194)
/* DMA_INLINK_DSCR_BF0_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last inlink descriptor x-1.*/
#define DMA_INLINK_DSCR_BF0_CH4 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH4_M ((DMA_INLINK_DSCR_BF0_CH4_V) << (DMA_INLINK_DSCR_BF0_CH4_S))
#define DMA_INLINK_DSCR_BF0_CH4_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF0_CH4_S 0

#define DMA_IN_DSCR_BF1_CH0_REG (DR_REG_DMA_BASE + 0x198)
/* DMA_INLINK_DSCR_BF1_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_INLINK_DSCR_BF1_CH0 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH0_M ((DMA_INLINK_DSCR_BF1_CH0_V) << (DMA_INLINK_DSCR_BF1_CH0_S))
#define DMA_INLINK_DSCR_BF1_CH0_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH0_S 0

#define DMA_IN_DSCR_BF1_CH1_REG (DR_REG_DMA_BASE + 0x19C)
/* DMA_INLINK_DSCR_BF1_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_INLINK_DSCR_BF1_CH1 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH1_M ((DMA_INLINK_DSCR_BF1_CH1_V) << (DMA_INLINK_DSCR_BF1_CH1_S))
#define DMA_INLINK_DSCR_BF1_CH1_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH1_S 0

#define DMA_IN_DSCR_BF1_CH2_REG (DR_REG_DMA_BASE + 0x1A0)
/* DMA_INLINK_DSCR_BF1_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_INLINK_DSCR_BF1_CH2 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH2_M ((DMA_INLINK_DSCR_BF1_CH2_V) << (DMA_INLINK_DSCR_BF1_CH2_S))
#define DMA_INLINK_DSCR_BF1_CH2_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH2_S 0

#define DMA_IN_DSCR_BF1_CH3_REG (DR_REG_DMA_BASE + 0x1A4)
/* DMA_INLINK_DSCR_BF1_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_INLINK_DSCR_BF1_CH3 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH3_M ((DMA_INLINK_DSCR_BF1_CH3_V) << (DMA_INLINK_DSCR_BF1_CH3_S))
#define DMA_INLINK_DSCR_BF1_CH3_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH3_S 0

#define DMA_IN_DSCR_BF1_CH4_REG (DR_REG_DMA_BASE + 0x1A8)
/* DMA_INLINK_DSCR_BF1_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_INLINK_DSCR_BF1_CH4 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH4_M ((DMA_INLINK_DSCR_BF1_CH4_V) << (DMA_INLINK_DSCR_BF1_CH4_S))
#define DMA_INLINK_DSCR_BF1_CH4_V 0xFFFFFFFF
#define DMA_INLINK_DSCR_BF1_CH4_S 0

#define DMA_OUT_DSCR_CH0_REG (DR_REG_DMA_BASE + 0x1AC)
/* DMA_OUTLINK_DSCR_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y.*/
#define DMA_OUTLINK_DSCR_CH0 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH0_M ((DMA_OUTLINK_DSCR_CH0_V) << (DMA_OUTLINK_DSCR_CH0_S))
#define DMA_OUTLINK_DSCR_CH0_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH0_S 0

#define DMA_OUT_DSCR_CH1_REG (DR_REG_DMA_BASE + 0x1B0)
/* DMA_OUTLINK_DSCR_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y.*/
#define DMA_OUTLINK_DSCR_CH1 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH1_M ((DMA_OUTLINK_DSCR_CH1_V) << (DMA_OUTLINK_DSCR_CH1_S))
#define DMA_OUTLINK_DSCR_CH1_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH1_S 0

#define DMA_OUT_DSCR_CH2_REG (DR_REG_DMA_BASE + 0x1B4)
/* DMA_OUTLINK_DSCR_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y.*/
#define DMA_OUTLINK_DSCR_CH2 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH2_M ((DMA_OUTLINK_DSCR_CH2_V) << (DMA_OUTLINK_DSCR_CH2_S))
#define DMA_OUTLINK_DSCR_CH2_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH2_S 0

#define DMA_OUT_DSCR_CH3_REG (DR_REG_DMA_BASE + 0x1B8)
/* DMA_OUTLINK_DSCR_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y.*/
#define DMA_OUTLINK_DSCR_CH3 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH3_M ((DMA_OUTLINK_DSCR_CH3_V) << (DMA_OUTLINK_DSCR_CH3_S))
#define DMA_OUTLINK_DSCR_CH3_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH3_S 0

#define DMA_OUT_DSCR_CH4_REG (DR_REG_DMA_BASE + 0x1BC)
/* DMA_OUTLINK_DSCR_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the current outlink descriptor y.*/
#define DMA_OUTLINK_DSCR_CH4 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH4_M ((DMA_OUTLINK_DSCR_CH4_V) << (DMA_OUTLINK_DSCR_CH4_S))
#define DMA_OUTLINK_DSCR_CH4_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_CH4_S 0

#define DMA_OUT_DSCR_BF0_CH0_REG (DR_REG_DMA_BASE + 0x1C0)
/* DMA_OUTLINK_DSCR_BF0_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1.*/
#define DMA_OUTLINK_DSCR_BF0_CH0 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH0_M ((DMA_OUTLINK_DSCR_BF0_CH0_V) << (DMA_OUTLINK_DSCR_BF0_CH0_S))
#define DMA_OUTLINK_DSCR_BF0_CH0_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH0_S 0

#define DMA_OUT_DSCR_BF0_CH1_REG (DR_REG_DMA_BASE + 0x1C4)
/* DMA_OUTLINK_DSCR_BF0_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1.*/
#define DMA_OUTLINK_DSCR_BF0_CH1 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH1_M ((DMA_OUTLINK_DSCR_BF0_CH1_V) << (DMA_OUTLINK_DSCR_BF0_CH1_S))
#define DMA_OUTLINK_DSCR_BF0_CH1_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH1_S 0

#define DMA_OUT_DSCR_BF0_CH2_REG (DR_REG_DMA_BASE + 0x1C8)
/* DMA_OUTLINK_DSCR_BF0_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1.*/
#define DMA_OUTLINK_DSCR_BF0_CH2 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH2_M ((DMA_OUTLINK_DSCR_BF0_CH2_V) << (DMA_OUTLINK_DSCR_BF0_CH2_S))
#define DMA_OUTLINK_DSCR_BF0_CH2_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH2_S 0

#define DMA_OUT_DSCR_BF0_CH3_REG (DR_REG_DMA_BASE + 0x1CC)
/* DMA_OUTLINK_DSCR_BF0_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1.*/
#define DMA_OUTLINK_DSCR_BF0_CH3 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH3_M ((DMA_OUTLINK_DSCR_BF0_CH3_V) << (DMA_OUTLINK_DSCR_BF0_CH3_S))
#define DMA_OUTLINK_DSCR_BF0_CH3_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH3_S 0

#define DMA_OUT_DSCR_BF0_CH4_REG (DR_REG_DMA_BASE + 0x1D0)
/* DMA_OUTLINK_DSCR_BF0_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the last outlink descriptor y-1.*/
#define DMA_OUTLINK_DSCR_BF0_CH4 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH4_M ((DMA_OUTLINK_DSCR_BF0_CH4_V) << (DMA_OUTLINK_DSCR_BF0_CH4_S))
#define DMA_OUTLINK_DSCR_BF0_CH4_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF0_CH4_S 0

#define DMA_OUT_DSCR_BF1_CH0_REG (DR_REG_DMA_BASE + 0x1D4)
/* DMA_OUTLINK_DSCR_BF1_CH0 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_OUTLINK_DSCR_BF1_CH0 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH0_M ((DMA_OUTLINK_DSCR_BF1_CH0_V) << (DMA_OUTLINK_DSCR_BF1_CH0_S))
#define DMA_OUTLINK_DSCR_BF1_CH0_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH0_S 0

#define DMA_OUT_DSCR_BF1_CH1_REG (DR_REG_DMA_BASE + 0x1D8)
/* DMA_OUTLINK_DSCR_BF1_CH1 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_OUTLINK_DSCR_BF1_CH1 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH1_M ((DMA_OUTLINK_DSCR_BF1_CH1_V) << (DMA_OUTLINK_DSCR_BF1_CH1_S))
#define DMA_OUTLINK_DSCR_BF1_CH1_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH1_S 0

#define DMA_OUT_DSCR_BF1_CH2_REG (DR_REG_DMA_BASE + 0x1DC)
/* DMA_OUTLINK_DSCR_BF1_CH2 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_OUTLINK_DSCR_BF1_CH2 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH2_M ((DMA_OUTLINK_DSCR_BF1_CH2_V) << (DMA_OUTLINK_DSCR_BF1_CH2_S))
#define DMA_OUTLINK_DSCR_BF1_CH2_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH2_S 0

#define DMA_OUT_DSCR_BF1_CH3_REG (DR_REG_DMA_BASE + 0x1E0)
/* DMA_OUTLINK_DSCR_BF1_CH3 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_OUTLINK_DSCR_BF1_CH3 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH3_M ((DMA_OUTLINK_DSCR_BF1_CH3_V) << (DMA_OUTLINK_DSCR_BF1_CH3_S))
#define DMA_OUTLINK_DSCR_BF1_CH3_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH3_S 0

#define DMA_OUT_DSCR_BF1_CH4_REG (DR_REG_DMA_BASE + 0x1E4)
/* DMA_OUTLINK_DSCR_BF1_CH4 : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The address of the second-to-last inlink descriptor x-2.*/
#define DMA_OUTLINK_DSCR_BF1_CH4 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH4_M ((DMA_OUTLINK_DSCR_BF1_CH4_V) << (DMA_OUTLINK_DSCR_BF1_CH4_S))
#define DMA_OUTLINK_DSCR_BF1_CH4_V 0xFFFFFFFF
#define DMA_OUTLINK_DSCR_BF1_CH4_S 0

#define DMA_PD_CONF_REG (DR_REG_DMA_BASE + 0x1E8)
/* DMA_RAM_CLK_FO : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: 1: Force to open the clock and bypass the gate-clock when accessing
 the RAM in DMA. 0: A gate-clock will be used when accessing the RAM in DMA.*/
#define DMA_RAM_CLK_FO (BIT(6))
#define DMA_RAM_CLK_FO_M (BIT(6))
#define DMA_RAM_CLK_FO_V 0x1
#define DMA_RAM_CLK_FO_S 6
/* DMA_RAM_FORCE_PU : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define DMA_RAM_FORCE_PU (BIT(5))
#define DMA_RAM_FORCE_PU_M (BIT(5))
#define DMA_RAM_FORCE_PU_V 0x1
#define DMA_RAM_FORCE_PU_S 5
/* DMA_RAM_FORCE_PD : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: power down*/
#define DMA_RAM_FORCE_PD (BIT(4))
#define DMA_RAM_FORCE_PD_M (BIT(4))
#define DMA_RAM_FORCE_PD_V 0x1
#define DMA_RAM_FORCE_PD_S 4

#define DMA_WIGHT_CH0_REG (DR_REG_DMA_BASE + 0x1EC)
/* DMA_RX_WEIGHT_CH0 : R/W ;bitpos:[7:4] ;default: 4'hF ; */
/*description: The weight of Rx channel 0.*/
#define DMA_RX_WEIGHT_CH0 0x0000000F
#define DMA_RX_WEIGHT_CH0_M ((DMA_RX_WEIGHT_CH0_V) << (DMA_RX_WEIGHT_CH0_S))
#define DMA_RX_WEIGHT_CH0_V 0xF
#define DMA_RX_WEIGHT_CH0_S 4
/* DMA_TX_WEIGHT_CH0 : R/W ;bitpos:[3:0] ;default: 4'hF ; */
/*description: The weight of Tx channel 0.*/
#define DMA_TX_WEIGHT_CH0 0x0000000F
#define DMA_TX_WEIGHT_CH0_M ((DMA_TX_WEIGHT_CH0_V) << (DMA_TX_WEIGHT_CH0_S))
#define DMA_TX_WEIGHT_CH0_V 0xF
#define DMA_TX_WEIGHT_CH0_S 0

#define DMA_WIGHT_CH1_REG (DR_REG_DMA_BASE + 0x1F0)
/* DMA_RX_WEIGHT_CH1 : R/W ;bitpos:[7:4] ;default: 4'hF ; */
/*description: The weight of Rx channel 1.*/
#define DMA_RX_WEIGHT_CH1 0x0000000F
#define DMA_RX_WEIGHT_CH1_M ((DMA_RX_WEIGHT_CH1_V) << (DMA_RX_WEIGHT_CH1_S))
#define DMA_RX_WEIGHT_CH1_V 0xF
#define DMA_RX_WEIGHT_CH1_S 4
/* DMA_TX_WEIGHT_CH1 : R/W ;bitpos:[3:0] ;default: 4'hF ; */
/*description: The weight of Tx channel 1.*/
#define DMA_TX_WEIGHT_CH1 0x0000000F
#define DMA_TX_WEIGHT_CH1_M ((DMA_TX_WEIGHT_CH1_V) << (DMA_TX_WEIGHT_CH1_S))
#define DMA_TX_WEIGHT_CH1_V 0xF
#define DMA_TX_WEIGHT_CH1_S 0

#define DMA_WIGHT_CH2_REG (DR_REG_DMA_BASE + 0x1F4)
/* DMA_RX_WEIGHT_CH2 : R/W ;bitpos:[7:4] ;default: 4'hF ; */
/*description: The weight of Rx channel 2.*/
#define DMA_RX_WEIGHT_CH2 0x0000000F
#define DMA_RX_WEIGHT_CH2_M ((DMA_RX_WEIGHT_CH2_V) << (DMA_RX_WEIGHT_CH2_S))
#define DMA_RX_WEIGHT_CH2_V 0xF
#define DMA_RX_WEIGHT_CH2_S 4
/* DMA_TX_WEIGHT_CH2 : R/W ;bitpos:[3:0] ;default: 4'hF ; */
/*description: The weight of Tx channel 2.*/
#define DMA_TX_WEIGHT_CH2 0x0000000F
#define DMA_TX_WEIGHT_CH2_M ((DMA_TX_WEIGHT_CH2_V) << (DMA_TX_WEIGHT_CH2_S))
#define DMA_TX_WEIGHT_CH2_V 0xF
#define DMA_TX_WEIGHT_CH2_S 0

#define DMA_WIGHT_CH3_REG (DR_REG_DMA_BASE + 0x1F8)
/* DMA_RX_WEIGHT_CH3 : R/W ;bitpos:[7:4] ;default: 4'hF ; */
/*description: The weight of Rx channel 3.*/
#define DMA_RX_WEIGHT_CH3 0x0000000F
#define DMA_RX_WEIGHT_CH3_M ((DMA_RX_WEIGHT_CH3_V) << (DMA_RX_WEIGHT_CH3_S))
#define DMA_RX_WEIGHT_CH3_V 0xF
#define DMA_RX_WEIGHT_CH3_S 4
/* DMA_TX_WEIGHT_CH3 : R/W ;bitpos:[3:0] ;default: 4'hF ; */
/*description: The weight of Tx channel 3.*/
#define DMA_TX_WEIGHT_CH3 0x0000000F
#define DMA_TX_WEIGHT_CH3_M ((DMA_TX_WEIGHT_CH3_V) << (DMA_TX_WEIGHT_CH3_S))
#define DMA_TX_WEIGHT_CH3_V 0xF
#define DMA_TX_WEIGHT_CH3_S 0

#define DMA_WIGHT_CH4_REG (DR_REG_DMA_BASE + 0x1FC)
/* DMA_RX_WEIGHT_CH4 : R/W ;bitpos:[7:4] ;default: 4'hF ; */
/*description: The weight of Rx channel 4.*/
#define DMA_RX_WEIGHT_CH4 0x0000000F
#define DMA_RX_WEIGHT_CH4_M ((DMA_RX_WEIGHT_CH4_V) << (DMA_RX_WEIGHT_CH4_S))
#define DMA_RX_WEIGHT_CH4_V 0xF
#define DMA_RX_WEIGHT_CH4_S 4
/* DMA_TX_WEIGHT_CH4 : R/W ;bitpos:[3:0] ;default: 4'hF ; */
/*description: The weight of Tx channel 4.*/
#define DMA_TX_WEIGHT_CH4 0x0000000F
#define DMA_TX_WEIGHT_CH4_M ((DMA_TX_WEIGHT_CH4_V) << (DMA_TX_WEIGHT_CH4_S))
#define DMA_TX_WEIGHT_CH4_V 0xF
#define DMA_TX_WEIGHT_CH4_S 0

#define DMA_PRI_CH0_REG (DR_REG_DMA_BASE + 0x200)
/* DMA_RX_PRI_CH0 : R/W ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: The priority of Rx channel 0. The larger of the value  the higher
 of the priority.*/
#define DMA_RX_PRI_CH0 0x0000000F
#define DMA_RX_PRI_CH0_M ((DMA_RX_PRI_CH0_V) << (DMA_RX_PRI_CH0_S))
#define DMA_RX_PRI_CH0_V 0xF
#define DMA_RX_PRI_CH0_S 4
/* DMA_TX_PRI_CH0 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 0. The larger of the value  the higher
 of the priority.*/
#define DMA_TX_PRI_CH0 0x0000000F
#define DMA_TX_PRI_CH0_M ((DMA_TX_PRI_CH0_V) << (DMA_TX_PRI_CH0_S))
#define DMA_TX_PRI_CH0_V 0xF
#define DMA_TX_PRI_CH0_S 0

#define DMA_PRI_CH1_REG (DR_REG_DMA_BASE + 0x204)
/* DMA_RX_PRI_CH1 : R/W ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: The priority of Rx channel 1. The larger of the value  the higher
 of the priority.*/
#define DMA_RX_PRI_CH1 0x0000000F
#define DMA_RX_PRI_CH1_M ((DMA_RX_PRI_CH1_V) << (DMA_RX_PRI_CH1_S))
#define DMA_RX_PRI_CH1_V 0xF
#define DMA_RX_PRI_CH1_S 4
/* DMA_TX_PRI_CH1 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 1. The larger of the value  the higher
 of the priority.*/
#define DMA_TX_PRI_CH1 0x0000000F
#define DMA_TX_PRI_CH1_M ((DMA_TX_PRI_CH1_V) << (DMA_TX_PRI_CH1_S))
#define DMA_TX_PRI_CH1_V 0xF
#define DMA_TX_PRI_CH1_S 0

#define DMA_PRI_CH2_REG (DR_REG_DMA_BASE + 0x208)
/* DMA_RX_PRI_CH2 : R/W ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: The priority of Rx channel 2. The larger of the value  the higher
 of the priority.*/
#define DMA_RX_PRI_CH2 0x0000000F
#define DMA_RX_PRI_CH2_M ((DMA_RX_PRI_CH2_V) << (DMA_RX_PRI_CH2_S))
#define DMA_RX_PRI_CH2_V 0xF
#define DMA_RX_PRI_CH2_S 4
/* DMA_TX_PRI_CH2 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 2. The larger of the value  the higher
 of the priority.*/
#define DMA_TX_PRI_CH2 0x0000000F
#define DMA_TX_PRI_CH2_M ((DMA_TX_PRI_CH2_V) << (DMA_TX_PRI_CH2_S))
#define DMA_TX_PRI_CH2_V 0xF
#define DMA_TX_PRI_CH2_S 0

#define DMA_PRI_CH3_REG (DR_REG_DMA_BASE + 0x20C)
/* DMA_RX_PRI_CH3 : R/W ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: The priority of Rx channel 3. The larger of the value  the higher
 of the priority.*/
#define DMA_RX_PRI_CH3 0x0000000F
#define DMA_RX_PRI_CH3_M ((DMA_RX_PRI_CH3_V) << (DMA_RX_PRI_CH3_S))
#define DMA_RX_PRI_CH3_V 0xF
#define DMA_RX_PRI_CH3_S 4
/* DMA_TX_PRI_CH3 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 3. The larger of the value  the higher
 of the priority.*/
#define DMA_TX_PRI_CH3 0x0000000F
#define DMA_TX_PRI_CH3_M ((DMA_TX_PRI_CH3_V) << (DMA_TX_PRI_CH3_S))
#define DMA_TX_PRI_CH3_V 0xF
#define DMA_TX_PRI_CH3_S 0

#define DMA_PRI_CH4_REG (DR_REG_DMA_BASE + 0x210)
/* DMA_RX_PRI_CH4 : R/W ;bitpos:[7:4] ;default: 4'b0 ; */
/*description: The priority of Rx channel 4. The larger of the value  the higher
 of the priority.*/
#define DMA_RX_PRI_CH4 0x0000000F
#define DMA_RX_PRI_CH4_M ((DMA_RX_PRI_CH4_V) << (DMA_RX_PRI_CH4_S))
#define DMA_RX_PRI_CH4_V 0xF
#define DMA_RX_PRI_CH4_S 4
/* DMA_TX_PRI_CH4 : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: The priority of Tx channel 4. The larger of the value  the higher
 of the priority.*/
#define DMA_TX_PRI_CH4 0x0000000F
#define DMA_TX_PRI_CH4_M ((DMA_TX_PRI_CH4_V) << (DMA_TX_PRI_CH4_S))
#define DMA_TX_PRI_CH4_V 0xF
#define DMA_TX_PRI_CH4_S 0

#define DMA_MISC_CONF_REG (DR_REG_DMA_BASE + 0x214)
/* DMA_CLK_EN : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DMA_CLK_EN (BIT(3))
#define DMA_CLK_EN_M (BIT(3))
#define DMA_CLK_EN_V 0x1
#define DMA_CLK_EN_S 3
/* DMA_ARB_PRI_DIS : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to disable priority arbitration function.*/
#define DMA_ARB_PRI_DIS (BIT(2))
#define DMA_ARB_PRI_DIS_M (BIT(2))
#define DMA_ARB_PRI_DIS_V 0x1
#define DMA_ARB_PRI_DIS_S 2
/* DMA_AHBM_RST_EXTER : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit  then clear this bit to reset the external ahb FSM.*/
#define DMA_AHBM_RST_EXTER (BIT(1))
#define DMA_AHBM_RST_EXTER_M (BIT(1))
#define DMA_AHBM_RST_EXTER_V 0x1
#define DMA_AHBM_RST_EXTER_S 1
/* DMA_AHBM_RST_INTER : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit  then clear this bit to reset the internal ahb FSM.*/
#define DMA_AHBM_RST_INTER (BIT(0))
#define DMA_AHBM_RST_INTER_M (BIT(0))
#define DMA_AHBM_RST_INTER_V 0x1
#define DMA_AHBM_RST_INTER_S 0

#define DMA_PERI_SEL_CH0_REG (DR_REG_DMA_BASE + 0x218)
/* DMA_PERI_OUT_SEL_CH0 : R/W ;bitpos:[11:6] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Tx channel 0. 0:SPI2*/
#define DMA_PERI_OUT_SEL_CH0 0x0000003F
#define DMA_PERI_OUT_SEL_CH0_M ((DMA_PERI_OUT_SEL_CH0_V) << (DMA_PERI_OUT_SEL_CH0_S))
#define DMA_PERI_OUT_SEL_CH0_V 0x3F
#define DMA_PERI_OUT_SEL_CH0_S 6
/* DMA_PERI_IN_SEL_CH0 : R/W ;bitpos:[5:0] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Rx channel 0. 0:SPI2*/
#define DMA_PERI_IN_SEL_CH0 0x0000003F
#define DMA_PERI_IN_SEL_CH0_M ((DMA_PERI_IN_SEL_CH0_V) << (DMA_PERI_IN_SEL_CH0_S))
#define DMA_PERI_IN_SEL_CH0_V 0x3F
#define DMA_PERI_IN_SEL_CH0_S 0

#define DMA_PERI_SEL_CH1_REG (DR_REG_DMA_BASE + 0x21C)
/* DMA_PERI_OUT_SEL_CH1 : R/W ;bitpos:[11:6] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Tx channel 1. 0:SPI2*/
#define DMA_PERI_OUT_SEL_CH1 0x0000003F
#define DMA_PERI_OUT_SEL_CH1_M ((DMA_PERI_OUT_SEL_CH1_V) << (DMA_PERI_OUT_SEL_CH1_S))
#define DMA_PERI_OUT_SEL_CH1_V 0x3F
#define DMA_PERI_OUT_SEL_CH1_S 6
/* DMA_PERI_IN_SEL_CH1 : R/W ;bitpos:[5:0] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Rx channel 1. 0:SPI2*/
#define DMA_PERI_IN_SEL_CH1 0x0000003F
#define DMA_PERI_IN_SEL_CH1_M ((DMA_PERI_IN_SEL_CH1_V) << (DMA_PERI_IN_SEL_CH1_S))
#define DMA_PERI_IN_SEL_CH1_V 0x3F
#define DMA_PERI_IN_SEL_CH1_S 0

#define DMA_PERI_SEL_CH2_REG (DR_REG_DMA_BASE + 0x220)
/* DMA_PERI_OUT_SEL_CH2 : R/W ;bitpos:[11:6] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Tx channel 2. 0:SPI2*/
#define DMA_PERI_OUT_SEL_CH2 0x0000003F
#define DMA_PERI_OUT_SEL_CH2_M ((DMA_PERI_OUT_SEL_CH2_V) << (DMA_PERI_OUT_SEL_CH2_S))
#define DMA_PERI_OUT_SEL_CH2_V 0x3F
#define DMA_PERI_OUT_SEL_CH2_S 6
/* DMA_PERI_IN_SEL_CH2 : R/W ;bitpos:[5:0] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Rx channel 2. 0:SPI2*/
#define DMA_PERI_IN_SEL_CH2 0x0000003F
#define DMA_PERI_IN_SEL_CH2_M ((DMA_PERI_IN_SEL_CH2_V) << (DMA_PERI_IN_SEL_CH2_S))
#define DMA_PERI_IN_SEL_CH2_V 0x3F
#define DMA_PERI_IN_SEL_CH2_S 0

#define DMA_PERI_SEL_CH3_REG (DR_REG_DMA_BASE + 0x224)
/* DMA_PERI_OUT_SEL_CH3 : R/W ;bitpos:[11:6] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Tx channel 3. 0:SPI2*/
#define DMA_PERI_OUT_SEL_CH3 0x0000003F
#define DMA_PERI_OUT_SEL_CH3_M ((DMA_PERI_OUT_SEL_CH3_V) << (DMA_PERI_OUT_SEL_CH3_S))
#define DMA_PERI_OUT_SEL_CH3_V 0x3F
#define DMA_PERI_OUT_SEL_CH3_S 6
/* DMA_PERI_IN_SEL_CH3 : R/W ;bitpos:[5:0] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Rx channel 3. 0:SPI2*/
#define DMA_PERI_IN_SEL_CH3 0x0000003F
#define DMA_PERI_IN_SEL_CH3_M ((DMA_PERI_IN_SEL_CH3_V) << (DMA_PERI_IN_SEL_CH3_S))
#define DMA_PERI_IN_SEL_CH3_V 0x3F
#define DMA_PERI_IN_SEL_CH3_S 0

#define DMA_PERI_SEL_CH4_REG (DR_REG_DMA_BASE + 0x228)
/* DMA_PERI_OUT_SEL_CH4 : R/W ;bitpos:[11:6] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Tx channel 4. 0:SPI2*/
#define DMA_PERI_OUT_SEL_CH4 0x0000003F
#define DMA_PERI_OUT_SEL_CH4_M ((DMA_PERI_OUT_SEL_CH4_V) << (DMA_PERI_OUT_SEL_CH4_S))
#define DMA_PERI_OUT_SEL_CH4_V 0x3F
#define DMA_PERI_OUT_SEL_CH4_S 6
/* DMA_PERI_IN_SEL_CH4 : R/W ;bitpos:[5:0] ;default: 6'h3F ; */
/*description: This register is used to select peripheral for Rx channel 4. 0:SPI2*/
#define DMA_PERI_IN_SEL_CH4 0x0000003F
#define DMA_PERI_IN_SEL_CH4_M ((DMA_PERI_IN_SEL_CH4_V) << (DMA_PERI_IN_SEL_CH4_S))
#define DMA_PERI_IN_SEL_CH4_V 0x3F
#define DMA_PERI_IN_SEL_CH4_S 0

#define DMA_SRAM_SIZE_CH0_REG (DR_REG_DMA_BASE + 0x22C)
/* DMA_OUT_SIZE_CH0 : R/W ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for
 Tx channel 0. 0:16 bytes*/
#define DMA_OUT_SIZE_CH0 0x0000001F
#define DMA_OUT_SIZE_CH0_M ((DMA_OUT_SIZE_CH0_V) << (DMA_OUT_SIZE_CH0_S))
#define DMA_OUT_SIZE_CH0_V 0x1F
#define DMA_OUT_SIZE_CH0_S 5
/* DMA_IN_SIZE_CH0 : R/W ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Rx FIFO for
 Rx channel 0. 0:16 bytes*/
#define DMA_IN_SIZE_CH0 0x0000001F
#define DMA_IN_SIZE_CH0_M ((DMA_IN_SIZE_CH0_V) << (DMA_IN_SIZE_CH0_S))
#define DMA_IN_SIZE_CH0_V 0x1F
#define DMA_IN_SIZE_CH0_S 0

#define DMA_SRAM_SIZE_CH1_REG (DR_REG_DMA_BASE + 0x230)
/* DMA_OUT_SIZE_CH1 : R/W ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for
 Tx channel 1. 0:16 bytes*/
#define DMA_OUT_SIZE_CH1 0x0000001F
#define DMA_OUT_SIZE_CH1_M ((DMA_OUT_SIZE_CH1_V) << (DMA_OUT_SIZE_CH1_S))
#define DMA_OUT_SIZE_CH1_V 0x1F
#define DMA_OUT_SIZE_CH1_S 5
/* DMA_IN_SIZE_CH1 : R/W ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Rx FIFO for
 Rx channel 1. 0:16 bytes*/
#define DMA_IN_SIZE_CH1 0x0000001F
#define DMA_IN_SIZE_CH1_M ((DMA_IN_SIZE_CH1_V) << (DMA_IN_SIZE_CH1_S))
#define DMA_IN_SIZE_CH1_V 0x1F
#define DMA_IN_SIZE_CH1_S 0

#define DMA_SRAM_SIZE_CH2_REG (DR_REG_DMA_BASE + 0x234)
/* DMA_OUT_SIZE_CH2 : R/W ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for
 Tx channel 2. 0:16 bytes*/
#define DMA_OUT_SIZE_CH2 0x0000001F
#define DMA_OUT_SIZE_CH2_M ((DMA_OUT_SIZE_CH2_V) << (DMA_OUT_SIZE_CH2_S))
#define DMA_OUT_SIZE_CH2_V 0x1F
#define DMA_OUT_SIZE_CH2_S 5
/* DMA_IN_SIZE_CH2 : R/W ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Rx FIFO for
 Rx channel 2. 0:16 bytes*/
#define DMA_IN_SIZE_CH2 0x0000001F
#define DMA_IN_SIZE_CH2_M ((DMA_IN_SIZE_CH2_V) << (DMA_IN_SIZE_CH2_S))
#define DMA_IN_SIZE_CH2_V 0x1F
#define DMA_IN_SIZE_CH2_S 0

#define DMA_SRAM_SIZE_CH3_REG (DR_REG_DMA_BASE + 0x238)
/* DMA_OUT_SIZE_CH3 : R/W ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for
 Tx channel 3. 0:16 bytes*/
#define DMA_OUT_SIZE_CH3 0x0000001F
#define DMA_OUT_SIZE_CH3_M ((DMA_OUT_SIZE_CH3_V) << (DMA_OUT_SIZE_CH3_S))
#define DMA_OUT_SIZE_CH3_V 0x1F
#define DMA_OUT_SIZE_CH3_S 5
/* DMA_IN_SIZE_CH3 : R/W ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Rx FIFO for
 Rx channel 3. 0:16 bytes*/
#define DMA_IN_SIZE_CH3 0x0000001F
#define DMA_IN_SIZE_CH3_M ((DMA_IN_SIZE_CH3_V) << (DMA_IN_SIZE_CH3_S))
#define DMA_IN_SIZE_CH3_V 0x1F
#define DMA_IN_SIZE_CH3_S 0

#define DMA_SRAM_SIZE_CH4_REG (DR_REG_DMA_BASE + 0x23C)
/* DMA_OUT_SIZE_CH4 : R/W ;bitpos:[9:5] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Tx FIFO for
 Tx channel 4. 0:16 bytes*/
#define DMA_OUT_SIZE_CH4 0x0000001F
#define DMA_OUT_SIZE_CH4_M ((DMA_OUT_SIZE_CH4_V) << (DMA_OUT_SIZE_CH4_S))
#define DMA_OUT_SIZE_CH4_V 0x1F
#define DMA_OUT_SIZE_CH4_S 5
/* DMA_IN_SIZE_CH4 : R/W ;bitpos:[4:0] ;default: 5'b0 ; */
/*description: This register is used to configure the size of L2 Rx FIFO for
 Rx channel 4. 0:16 bytes*/
#define DMA_IN_SIZE_CH4 0x0000001F
#define DMA_IN_SIZE_CH4_M ((DMA_IN_SIZE_CH4_V) << (DMA_IN_SIZE_CH4_S))
#define DMA_IN_SIZE_CH4_V 0x1F
#define DMA_IN_SIZE_CH4_S 0

#define DMA_DATE_REG (DR_REG_DMA_BASE + 0x240)
/* DMA_DATE : R/W ;bitpos:[31:0] ;default: 32'h2002260 ; */
/*description: register version.*/
#define DMA_DATE 0xFFFFFFFF
#define DMA_DATE_M ((DMA_DATE_V) << (DMA_DATE_S))
#define DMA_DATE_V 0xFFFFFFFF
#define DMA_DATE_S 0

#ifdef __cplusplus
}
#endif
