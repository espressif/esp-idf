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
#ifndef _SOC_EMAC_REG_H_
#define _SOC_EMAC_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define EMAC_DMABUSMODE_REG          (DR_REG_EMAC_BASE + 0x0000)
/* EMAC_DMAMIXEDBURST : R/W ;bitpos:[26] ;default: 1'h0 ; */
/*description: When this bit is set high and the FIXED_BURST bit is low  the
 AHB master interface starts all bursts of a length more than 16 with INCR (undefined burst)  whereas it reverts to fixed burst transfers (INCRx and SINGLE) for burst length of 16 and less.*/
#define EMAC_DMAMIXEDBURST  (BIT(26))
#define EMAC_DMAMIXEDBURST_M  (BIT(26))
#define EMAC_DMAMIXEDBURST_V  0x1
#define EMAC_DMAMIXEDBURST_S  26
/* EMAC_DMAADDRALIBEA : R/W ;bitpos:[25] ;default: 1'h0 ; */
/*description: When this bit is set high and the FIXED_BURST bit is 1  the AHB
 interface generates all bursts aligned to the start address LS bits. If the FIXED_BURST bit is 0  the first burst (accessing the start address of data buffer) is not aligned  but subsequent bursts are aligned to the address.*/
#define EMAC_DMAADDRALIBEA  (BIT(25))
#define EMAC_DMAADDRALIBEA_M  (BIT(25))
#define EMAC_DMAADDRALIBEA_V  0x1
#define EMAC_DMAADDRALIBEA_S  25
/* EMAC_PBLX8_MODE : R/W ;bitpos:[24] ;default: 1'h0 ; */
/*description: When set high  this bit multiplies the programmed PBL value (Bits[22:17]
 and Bits[13:8]) eight times. Therefore  the DMA transfers the data in 8  16  32  64  128  and 256 beats depending on the PBL value.*/
#define EMAC_PBLX8_MODE  (BIT(24))
#define EMAC_PBLX8_MODE_M  (BIT(24))
#define EMAC_PBLX8_MODE_V  0x1
#define EMAC_PBLX8_MODE_S  24
/* EMAC_USE_SEP_PBL : R/W ;bitpos:[23] ;default: 1'h0 ; */
/*description: When set high  this bit configures the Rx DMA to use the value
 configured in Bits[22:17] as PBL. The PBL value in Bits[13:8] is applicable only to the Tx DMA operations. When reset to low  the PBL value in Bits[13:8] is applicable for both DMA engines.*/
#define EMAC_USE_SEP_PBL  (BIT(23))
#define EMAC_USE_SEP_PBL_M  (BIT(23))
#define EMAC_USE_SEP_PBL_V  0x1
#define EMAC_USE_SEP_PBL_S  23
/* EMAC_RX_DMA_PBL : R/W ;bitpos:[22:17] ;default: 6'h1 ; */
/*description: This field indicates the maximum number of beats to be transferred
 in one Rx DMA transaction. This is the maximum value that is used in a single block Read or Write.The Rx DMA always attempts to burst as specified in the RPBL(RX_DMA_PBL) bit each time it starts a burst transfer on the host bus. You can program RPBL with values of 1  2  4  8  16  and 32. Any other value results in undefined behavior. This field is valid and applicable only when USP(USE_SEP_PBL) is set high.*/
#define EMAC_RX_DMA_PBL  0x0000003F
#define EMAC_RX_DMA_PBL_M  ((EMAC_RX_DMA_PBL_V)<<(EMAC_RX_DMA_PBL_S))
#define EMAC_RX_DMA_PBL_V  0x3F
#define EMAC_RX_DMA_PBL_S  17
/* EMAC_FIXED_BURST : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: This bit controls whether the AHB master interface performs fixed
 burst transfers or not. When set  the AHB interface uses only SINGLE  INCR4  INCR8  or INCR16 during start of the normal burst transfers. When reset  the AHB interface uses SINGLE and INCR burst transfer Operations.*/
#define EMAC_FIXED_BURST  (BIT(16))
#define EMAC_FIXED_BURST_M  (BIT(16))
#define EMAC_FIXED_BURST_V  0x1
#define EMAC_FIXED_BURST_S  16
/* EMAC_PRI_RATIO : R/W ;bitpos:[15:14] ;default: 2'h0 ; */
/*description: These bits control the priority ratio in the weighted round-robin
 arbitration between the Rx DMA and Tx DMA. These bits are valid only when Bit 1 (DA) is reset. The priority ratio Rx:Tx represented by each bit:  2'b00 -- 1: 1    2'b01 -- 2: 0    2'b10 -- 3: 1    2'b11 -- 4: 1*/
#define EMAC_PRI_RATIO  0x00000003
#define EMAC_PRI_RATIO_M  ((EMAC_PRI_RATIO_V)<<(EMAC_PRI_RATIO_S))
#define EMAC_PRI_RATIO_V  0x3
#define EMAC_PRI_RATIO_S  14
/* EMAC_PROG_BURST_LEN : R/W ;bitpos:[13:8] ;default: 6'h1 ; */
/*description: These bits indicate the maximum number of beats to be transferred
 in one DMA transaction. If the number of beats to be transferred is more than 32  then perform the following steps: 1. Set the PBLx8 mode  2. Set the PBL(PROG_BURST_LEN).*/
#define EMAC_PROG_BURST_LEN  0x0000003F
#define EMAC_PROG_BURST_LEN_M  ((EMAC_PROG_BURST_LEN_V)<<(EMAC_PROG_BURST_LEN_S))
#define EMAC_PROG_BURST_LEN_V  0x3F
#define EMAC_PROG_BURST_LEN_S  8
/* EMAC_ALT_DESC_SIZE : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: When set  the size of the alternate descriptor increases to 32 bytes.*/
#define EMAC_ALT_DESC_SIZE  (BIT(7))
#define EMAC_ALT_DESC_SIZE_M  (BIT(7))
#define EMAC_ALT_DESC_SIZE_V  0x1
#define EMAC_ALT_DESC_SIZE_S  7
/* EMAC_DESC_SKIP_LEN : R/W ;bitpos:[6:2] ;default: 5'h0 ; */
/*description: This bit specifies the number of Word to skip between two unchained
 descriptors.The address skipping starts from the end of current descriptor to the start of next descriptor. When the DSL(DESC_SKIP_LEN) value is equal to zero  the descriptor table is taken as contiguous by the DMA in Ring mode.*/
#define EMAC_DESC_SKIP_LEN  0x0000001F
#define EMAC_DESC_SKIP_LEN_M  ((EMAC_DESC_SKIP_LEN_V)<<(EMAC_DESC_SKIP_LEN_S))
#define EMAC_DESC_SKIP_LEN_V  0x1F
#define EMAC_DESC_SKIP_LEN_S  2
/* EMAC_DMA_ARB_SCH : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: This bit specifies the arbitration scheme between the transmit
 and receive paths.1'b0: weighted round-robin with RX:TX or TX:RX  priority specified in PR (bit[15:14]). 1'b1 Fixed priority (Rx priority to Tx).*/
#define EMAC_DMA_ARB_SCH  (BIT(1))
#define EMAC_DMA_ARB_SCH_M  (BIT(1))
#define EMAC_DMA_ARB_SCH_V  0x1
#define EMAC_DMA_ARB_SCH_S  1
/* EMAC_SW_RST : R_WS_SC ;bitpos:[0] ;default: 1'h1 ; */
/*description: When this bit is set  the MAC DMA Controller resets the logic
 and all internal registers of the MAC. It is cleared automatically after the reset operation is complete in all of the ETH_MAC clock domains. Before reprogramming any register of the ETH_MAC  you should read a zero (0) value in this bit.*/
#define EMAC_SW_RST  (BIT(0))
#define EMAC_SW_RST_M  (BIT(0))
#define EMAC_SW_RST_V  0x1
#define EMAC_SW_RST_S  0

#define EMAC_DMATXPOLLDEMAND_REG          (DR_REG_EMAC_BASE + 0x0004)
/* EMAC_TRANS_POLL_DEMAND : RO_WT ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: When these bits are written with any value  the DMA reads the
 current descriptor to which the Register (Current Host Transmit Descriptor Register) is pointing. If that descriptor is not available (owned by the Host)  the transmission returns to the suspend state and Bit[2] (TU) of Status Register is asserted. If the descriptor is available  the transmission resumes.*/
#define EMAC_TRANS_POLL_DEMAND  0xFFFFFFFF
#define EMAC_TRANS_POLL_DEMAND_M  ((EMAC_TRANS_POLL_DEMAND_V)<<(EMAC_TRANS_POLL_DEMAND_S))
#define EMAC_TRANS_POLL_DEMAND_V  0xFFFFFFFF
#define EMAC_TRANS_POLL_DEMAND_S  0

#define EMAC_DMARXPOLLDEMAND_REG          (DR_REG_EMAC_BASE + 0x0008)
/* EMAC_RECV_POLL_DEMAND : RO_WT ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: When these bits are written with any value  the DMA reads the
 current descriptor to which the Current Host Receive Descriptor Register is pointing. If that descriptor is not available (owned by the Host)  the reception returns to the Suspended state and Bit[7] (RU) of Status Register is asserted. If the descriptor is available  the Rx DMA returns to the active state.*/
#define EMAC_RECV_POLL_DEMAND  0xFFFFFFFF
#define EMAC_RECV_POLL_DEMAND_M  ((EMAC_RECV_POLL_DEMAND_V)<<(EMAC_RECV_POLL_DEMAND_S))
#define EMAC_RECV_POLL_DEMAND_V  0xFFFFFFFF
#define EMAC_RECV_POLL_DEMAND_S  0

#define EMAC_DMARXBASEADDR_REG          (DR_REG_EMAC_BASE + 0x000C)
/* EMAC_START_RECV_LIST : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This field contains the base address of the first descriptor
 in the Receive Descriptor list. The LSB Bits[1:0] are ignored and internally taken as all-zero by the DMA. Therefore these LSB bits are read-only.*/
#define EMAC_START_RECV_LIST  0xFFFFFFFF
#define EMAC_START_RECV_LIST_M  ((EMAC_START_RECV_LIST_V)<<(EMAC_START_RECV_LIST_S))
#define EMAC_START_RECV_LIST_V  0xFFFFFFFF
#define EMAC_START_RECV_LIST_S  0

#define EMAC_DMATXBASEADDR_REG          (DR_REG_EMAC_BASE + 0x0010)
/* EMAC_START_TRANS_LIST : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: This field contains the base address of the first descriptor
 in the Transmit Descriptor list. The LSB Bits[1:0] are ignored and are internally taken as all-zero by the DMA.Therefore  these LSB bits are read-only.*/
#define EMAC_START_TRANS_LIST  0xFFFFFFFF
#define EMAC_START_TRANS_LIST_M  ((EMAC_START_TRANS_LIST_V)<<(EMAC_START_TRANS_LIST_S))
#define EMAC_START_TRANS_LIST_V  0xFFFFFFFF
#define EMAC_START_TRANS_LIST_S  0

#define EMAC_DMASTATUS_REG          (DR_REG_EMAC_BASE + 0x0014)
/* EMAC_TS_TRI_INT : RO ;bitpos:[29] ;default: 1'h0 ; */
/*description: This bit indicates an interrupt event in the Timestamp Generator
 block of the ETH_MAC.The software must read the corresponding registers in the ETH_MAC to get the exact cause of the interrupt and clear its source to reset this bit to 1'b0.*/
#define EMAC_TS_TRI_INT  (BIT(29))
#define EMAC_TS_TRI_INT_M  (BIT(29))
#define EMAC_TS_TRI_INT_V  0x1
#define EMAC_TS_TRI_INT_S  29
/* EMAC_PMT_INT : RO ;bitpos:[28] ;default: 1'h0 ; */
/*description: This bit indicates an interrupt event in the PMT module of the
 ETH_MAC. The software must read the PMT Control and Status Register in the MAC to get the exact cause of interrupt and clear its source to reset this bit to 1'b0.*/
#define EMAC_PMT_INT  (BIT(28))
#define EMAC_PMT_INT_M  (BIT(28))
#define EMAC_PMT_INT_V  0x1
#define EMAC_PMT_INT_S  28
/* EMAC_ERROR_BITS : RO ;bitpos:[25:23] ;default: 3'h0 ; */
/*description: This field indicates the type of error that caused a Bus Error
  for example  error response on the AHB interface. This field is valid only when Bit[13] (FBI) is set. This field does not generate an interrupt.  3'b000: Error during Rx DMA Write Data Transfer.  3'b011: Error during Tx DMA Read Data Transfer.  3'b100: Error during Rx DMA Descriptor Write Access.  3'b101: Error during Tx DMA Descriptor Write Access.  3'b110: Error during Rx DMA Descriptor Read Access.  3'b111: Error during Tx DMA Descriptor Read Access.*/
#define EMAC_ERROR_BITS  0x00000007
#define EMAC_ERROR_BITS_M  ((EMAC_ERROR_BITS_V)<<(EMAC_ERROR_BITS_S))
#define EMAC_ERROR_BITS_V  0x7
#define EMAC_ERROR_BITS_S  23
/* EMAC_TRANS_PROC_STATE : RO ;bitpos:[22:20] ;default: 3'h0 ; */
/*description: This field indicates the Transmit DMA FSM state. This field does
 not generate an interrupt.  3'b000: Stopped. Reset or Stop Transmit Command issued.  3'b001: Running. Fetching Transmit Transfer Descriptor.  3'b010: Reserved for future use.  3'b011: Running. Waiting for TX packets.  3'b100: Suspended. Receive Descriptor Unavailable.  3'b101: Running. Closing Transmit Descriptor.  3'b110: TIME_STAMP write state.  3'b111: Running. Transferring the TX packets data from transmit buffer to host memory.*/
#define EMAC_TRANS_PROC_STATE  0x00000007
#define EMAC_TRANS_PROC_STATE_M  ((EMAC_TRANS_PROC_STATE_V)<<(EMAC_TRANS_PROC_STATE_S))
#define EMAC_TRANS_PROC_STATE_V  0x7
#define EMAC_TRANS_PROC_STATE_S  20
/* EMAC_RECV_PROC_STATE : RO ;bitpos:[19:17] ;default: 3'h0 ; */
/*description: This field indicates the Receive DMA FSM state. This field does
 not generate an interrupt.  3'b000: Stopped. Reset or Stop Receive Command issued.  3'b001: Running. Fetching Receive Transfer Descriptor.  3'b010: Reserved for future use.  3'b011: Running. Waiting for RX packets.  3'b100: Suspended. Receive Descriptor Unavailable.  3'b101: Running. Closing Receive Descriptor.  3'b110: TIME_STAMP write state.  3'b111: Running. Transferring the TX packets data from receive buffer to host memory.*/
#define EMAC_RECV_PROC_STATE  0x00000007
#define EMAC_RECV_PROC_STATE_M  ((EMAC_RECV_PROC_STATE_V)<<(EMAC_RECV_PROC_STATE_S))
#define EMAC_RECV_PROC_STATE_V  0x7
#define EMAC_RECV_PROC_STATE_S  17
/* EMAC_NORM_INT_SUMM : R_SS_WC ;bitpos:[16] ;default: 1'h0 ; */
/*description: Normal Interrupt Summary bit value is the logical OR of the following
 bits when the corresponding interrupt bits are enabled in Interrupt Enable Register:  Bit[0]: Transmit Interrupt.  Bit[2]: Transmit Buffer Unavailable.  Bit[6]: Receive Interrupt.  Bit[14]: Early Receive Interrupt. Only unmasked bits affect the Normal Interrupt Summary bit.This is a sticky bit and must be cleared (by writing 1 to this bit) each time a corresponding bit which causes NIS to be set  is cleared.*/
#define EMAC_NORM_INT_SUMM  (BIT(16))
#define EMAC_NORM_INT_SUMM_M  (BIT(16))
#define EMAC_NORM_INT_SUMM_V  0x1
#define EMAC_NORM_INT_SUMM_S  16
/* EMAC_ABN_INT_SUMM : R_SS_WC ;bitpos:[15] ;default: 1'h0 ; */
/*description: Abnormal Interrupt Summary bit value is the logical OR of the
 following when the corresponding interrupt bits are enabled in Interrupt Enable Register:   Bit[1]: Transmit Process Stopped.  Bit[3]: Transmit Jabber Timeout.  Bit[4]: Receive FIFO Overflow.  Bit[5]: Transmit Underflow.  Bit[7]: Receive Buffer Unavailable. Bit[8]: Receive Process Stopped.  Bit[9]: Receive Watchdog Timeout.  Bit[10]: Early Transmit Interrupt.  Bit[13]: Fatal Bus Error. Only unmasked bits affect the Abnormal Interrupt Summary bit. This is a sticky bit and must be cleared (by writing 1 to this bit) each time a corresponding bit which causes AIS to be set  is cleared.*/
#define EMAC_ABN_INT_SUMM  (BIT(15))
#define EMAC_ABN_INT_SUMM_M  (BIT(15))
#define EMAC_ABN_INT_SUMM_V  0x1
#define EMAC_ABN_INT_SUMM_S  15
/* EMAC_EARLY_RECV_INT : R_SS_WC ;bitpos:[14] ;default: 1'h0 ; */
/*description: This bit indicates that the DMA filled the first data buffer
 of the packet. This bit is cleared when the software writes 1 to this bit or when Bit[6] (RI) of this register is set (whichever occurs earlier).*/
#define EMAC_EARLY_RECV_INT  (BIT(14))
#define EMAC_EARLY_RECV_INT_M  (BIT(14))
#define EMAC_EARLY_RECV_INT_V  0x1
#define EMAC_EARLY_RECV_INT_S  14
/* EMAC_FATAL_BUS_ERR_INT : R_SS_WC ;bitpos:[13] ;default: 1'h0 ; */
/*description: This bit indicates that a bus error occurred  as described in
 Bits [25:23]. When this bit is set  the corresponding DMA engine disables all of its bus accesses.*/
#define EMAC_FATAL_BUS_ERR_INT  (BIT(13))
#define EMAC_FATAL_BUS_ERR_INT_M  (BIT(13))
#define EMAC_FATAL_BUS_ERR_INT_V  0x1
#define EMAC_FATAL_BUS_ERR_INT_S  13
/* EMAC_EARLY_TRANS_INT : R_SS_WC ;bitpos:[10] ;default: 1'h0 ; */
/*description: This bit indicates that the frame to be transmitted is fully
 transferred to the MTL Transmit FIFO.*/
#define EMAC_EARLY_TRANS_INT  (BIT(10))
#define EMAC_EARLY_TRANS_INT_M  (BIT(10))
#define EMAC_EARLY_TRANS_INT_V  0x1
#define EMAC_EARLY_TRANS_INT_S  10
/* EMAC_RECV_WDT_TO : R_SS_WC ;bitpos:[9] ;default: 1'h0 ; */
/*description: When set  this bit indicates that the Receive Watchdog Timer
 expired while receiving the current frame and the current frame is truncated after the watchdog timeout.*/
#define EMAC_RECV_WDT_TO  (BIT(9))
#define EMAC_RECV_WDT_TO_M  (BIT(9))
#define EMAC_RECV_WDT_TO_V  0x1
#define EMAC_RECV_WDT_TO_S  9
/* EMAC_RECV_PROC_STOP : R_SS_WC ;bitpos:[8] ;default: 1'h0 ; */
/*description: This bit is asserted when the Receive Process enters the Stopped state.*/
#define EMAC_RECV_PROC_STOP  (BIT(8))
#define EMAC_RECV_PROC_STOP_M  (BIT(8))
#define EMAC_RECV_PROC_STOP_V  0x1
#define EMAC_RECV_PROC_STOP_S  8
/* EMAC_RECV_BUF_UNAVAIL : R_SS_WC ;bitpos:[7] ;default: 1'h0 ; */
/*description: This bit indicates that the host owns the Next Descriptor in
 the Receive List and the DMA cannot acquire it. The Receive Process is suspended. To resume processing Receive descriptors  the host should change the ownership of the descriptor and issue a Receive Poll Demand command. If no Receive Poll Demand is issued  the Receive Process resumes when the next recognized incoming frame is received. This bit is set only when the previous Receive Descriptor is owned by the DMA.*/
#define EMAC_RECV_BUF_UNAVAIL  (BIT(7))
#define EMAC_RECV_BUF_UNAVAIL_M  (BIT(7))
#define EMAC_RECV_BUF_UNAVAIL_V  0x1
#define EMAC_RECV_BUF_UNAVAIL_S  7
/* EMAC_RECV_INT : R_SS_WC ;bitpos:[6] ;default: 1'h0 ; */
/*description: This bit indicates that the frame reception is complete. When
 reception is complete  the Bit[31] of RDES1 (Disable Interrupt on Completion) is reset in the last Descriptor  and the specific frame status information is updated in the descriptor. The reception remains in the Running state.*/
#define EMAC_RECV_INT  (BIT(6))
#define EMAC_RECV_INT_M  (BIT(6))
#define EMAC_RECV_INT_V  0x1
#define EMAC_RECV_INT_S  6
/* EMAC_TRANS_UNDFLOW : R_SS_WC ;bitpos:[5] ;default: 1'h0 ; */
/*description: This bit indicates that the Transmit Buffer had an Underflow
 during frame transmission. Transmission is suspended and an Underflow Error TDES0[1] is set.*/
#define EMAC_TRANS_UNDFLOW  (BIT(5))
#define EMAC_TRANS_UNDFLOW_M  (BIT(5))
#define EMAC_TRANS_UNDFLOW_V  0x1
#define EMAC_TRANS_UNDFLOW_S  5
/* EMAC_RECV_OVFLOW : R_SS_WC ;bitpos:[4] ;default: 1'h0 ; */
/*description: This bit indicates that the Receive Buffer had an Overflow during
 frame reception. If the partial frame is transferred to the application  the overflow status is set in RDES0[11].*/
#define EMAC_RECV_OVFLOW  (BIT(4))
#define EMAC_RECV_OVFLOW_M  (BIT(4))
#define EMAC_RECV_OVFLOW_V  0x1
#define EMAC_RECV_OVFLOW_S  4
/* EMAC_TRANS_JABBER_TO : R_SS_WC ;bitpos:[3] ;default: 1'h0 ; */
/*description: This bit indicates that the Transmit Jabber Timer expired  which
 happens when the frame size exceeds 2 048 (10 240 bytes when the Jumbo frame is enabled). When the Jabber Timeout occurs  the transmission process is aborted and placed in the Stopped state. This causes the Transmit Jabber Timeout TDES0[14] flag to assert.*/
#define EMAC_TRANS_JABBER_TO  (BIT(3))
#define EMAC_TRANS_JABBER_TO_M  (BIT(3))
#define EMAC_TRANS_JABBER_TO_V  0x1
#define EMAC_TRANS_JABBER_TO_S  3
/* EMAC_TRANS_BUF_UNAVAIL : R_SS_WC ;bitpos:[2] ;default: 1'h0 ; */
/*description: This bit indicates that the host owns the Next Descriptor in
 the Transmit List and the DMA cannot acquire it. Transmission is suspended. Bits[22:20] explain the Transmit Process state transitions. To resume processing Transmit descriptors  the host should change the ownership of the descriptor by setting TDES0[31] and then issue a Transmit Poll Demand Command.*/
#define EMAC_TRANS_BUF_UNAVAIL  (BIT(2))
#define EMAC_TRANS_BUF_UNAVAIL_M  (BIT(2))
#define EMAC_TRANS_BUF_UNAVAIL_V  0x1
#define EMAC_TRANS_BUF_UNAVAIL_S  2
/* EMAC_TRANS_PROC_STOP : R_SS_WC ;bitpos:[1] ;default: 1'h0 ; */
/*description: This bit is set when the transmission is stopped.*/
#define EMAC_TRANS_PROC_STOP  (BIT(1))
#define EMAC_TRANS_PROC_STOP_M  (BIT(1))
#define EMAC_TRANS_PROC_STOP_V  0x1
#define EMAC_TRANS_PROC_STOP_S  1
/* EMAC_TRANS_INT : R_SS_WC ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit indicates that the frame transmission is complete. When
 transmission is complete  Bit[31] (OWN) of TDES0 is reset  and the specific frame status information is updated in the Descriptor.*/
#define EMAC_TRANS_INT  (BIT(0))
#define EMAC_TRANS_INT_M  (BIT(0))
#define EMAC_TRANS_INT_V  0x1
#define EMAC_TRANS_INT_S  0

#define EMAC_DMAOPERATION_MODE_REG          (DR_REG_EMAC_BASE + 0x0018)
/* EMAC_DIS_DROP_TCPIP_ERR_FRAM : R/W ;bitpos:[26] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC does not drop the frames which
 only have errors detected by the Receive Checksum engine.When this bit is reset  all error frames are dropped if the Fwd_Err_Frame bit is reset.*/
#define EMAC_DIS_DROP_TCPIP_ERR_FRAM  (BIT(26))
#define EMAC_DIS_DROP_TCPIP_ERR_FRAM_M  (BIT(26))
#define EMAC_DIS_DROP_TCPIP_ERR_FRAM_V  0x1
#define EMAC_DIS_DROP_TCPIP_ERR_FRAM_S  26
/* EMAC_RX_STORE_FORWARD : R/W ;bitpos:[25] ;default: 1'h0 ; */
/*description: When this bit is set  the MTL reads a frame from the Rx FIFO
 only after the complete frame has been written to it.*/
#define EMAC_RX_STORE_FORWARD  (BIT(25))
#define EMAC_RX_STORE_FORWARD_M  (BIT(25))
#define EMAC_RX_STORE_FORWARD_V  0x1
#define EMAC_RX_STORE_FORWARD_S  25
/* EMAC_DIS_FLUSH_RECV_FRAMES : R/W ;bitpos:[24] ;default: 1'h0 ; */
/*description: When this bit is set  the Rx DMA does not flush any frames because
 of the unavailability of receive descriptors or buffers.*/
#define EMAC_DIS_FLUSH_RECV_FRAMES  (BIT(24))
#define EMAC_DIS_FLUSH_RECV_FRAMES_M  (BIT(24))
#define EMAC_DIS_FLUSH_RECV_FRAMES_V  0x1
#define EMAC_DIS_FLUSH_RECV_FRAMES_S  24
/* EMAC_TX_STR_FWD : R/W ;bitpos:[21] ;default: 1'h0 ; */
/*description: When this bit is set  transmission starts when a full frame resides
 in the MTL Transmit FIFO. When this bit is set  the Tx_Thresh_Ctrl values specified in Tx_Thresh_Ctrl are ignored.*/
#define EMAC_TX_STR_FWD  (BIT(21))
#define EMAC_TX_STR_FWD_M  (BIT(21))
#define EMAC_TX_STR_FWD_V  0x1
#define EMAC_TX_STR_FWD_S  21
/* EMAC_FLUSH_TX_FIFO : R_WS_SC ;bitpos:[20] ;default: 1'h0 ; */
/*description: When this bit is set  the transmit FIFO controller logic is reset
 to its default values  and thus all data in the Tx FIFO is lost or flushed. This bit is cleared internally when the flushing operation is complete.*/
#define EMAC_FLUSH_TX_FIFO  (BIT(20))
#define EMAC_FLUSH_TX_FIFO_M  (BIT(20))
#define EMAC_FLUSH_TX_FIFO_V  0x1
#define EMAC_FLUSH_TX_FIFO_S  20
/* EMAC_TX_THRESH_CTRL : R/W ;bitpos:[16:14] ;default: 3'h0 ; */
/*description: These bits control the threshold level of the MTL Transmit FIFO.
 Transmission starts when the frame size within the MTL Transmit FIFO is larger than the threshold. In addition  full frames with a length less than the threshold are also transmitted. These bits are used only when Tx_Str_fwd is reset. 3'b000: 64  3'b001: 128   3'b010: 192   3'b011: 256  3'b100: 40  3'b101: 32  3'b110: 24  3'b111: 16 .*/
#define EMAC_TX_THRESH_CTRL  0x00000007
#define EMAC_TX_THRESH_CTRL_M  ((EMAC_TX_THRESH_CTRL_V)<<(EMAC_TX_THRESH_CTRL_S))
#define EMAC_TX_THRESH_CTRL_V  0x7
#define EMAC_TX_THRESH_CTRL_S  14
/* EMAC_START_STOP_TRANSMISSION_COMMAND : R/W ;bitpos:[13] ;default: 1'h0 ; */
/*description: When this bit is set  transmission is placed in the Running state
  and the DMA checks the Transmit List at the current position for a frame to be transmitted.When this bit is reset  the transmission process is placed in the Stopped state after completing the transmission of the current frame.*/
#define EMAC_START_STOP_TRANSMISSION_COMMAND  (BIT(13))
#define EMAC_START_STOP_TRANSMISSION_COMMAND_M  (BIT(13))
#define EMAC_START_STOP_TRANSMISSION_COMMAND_V  0x1
#define EMAC_START_STOP_TRANSMISSION_COMMAND_S  13
/* EMAC_FWD_ERR_FRAME : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: When this bit is reset  the Rx FIFO drops frames with error status
 (CRC error collision error   giant frame  watchdog timeout  or overflow).*/
#define EMAC_FWD_ERR_FRAME  (BIT(7))
#define EMAC_FWD_ERR_FRAME_M  (BIT(7))
#define EMAC_FWD_ERR_FRAME_V  0x1
#define EMAC_FWD_ERR_FRAME_S  7
/* EMAC_FWD_UNDER_GF : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: When set  the Rx FIFO forwards Undersized frames (that is  frames
 with no Error and length less than 64 bytes) including pad-bytes and CRC.*/
#define EMAC_FWD_UNDER_GF  (BIT(6))
#define EMAC_FWD_UNDER_GF_M  (BIT(6))
#define EMAC_FWD_UNDER_GF_V  0x1
#define EMAC_FWD_UNDER_GF_S  6
/* EMAC_DROP_GFRM : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: When set  the MAC drops the received giant frames in the Rx FIFO
  that is frames that are larger than the computed giant frame limit.*/
#define EMAC_DROP_GFRM  (BIT(5))
#define EMAC_DROP_GFRM_M  (BIT(5))
#define EMAC_DROP_GFRM_V  0x1
#define EMAC_DROP_GFRM_S  5
/* EMAC_RX_THRESH_CTRL : R/W ;bitpos:[4:3] ;default: 2'h0 ; */
/*description: These two bits control the threshold level of the MTL Receive
 FIFO. Transfer (request) to DMA starts when the frame size within the MTL Receive FIFO is larger than the threshold. 2'b00: 64， 2'b01: 32， 2'b10: 96， 2'b11: 128 .*/
#define EMAC_RX_THRESH_CTRL  0x00000003
#define EMAC_RX_THRESH_CTRL_M  ((EMAC_RX_THRESH_CTRL_V)<<(EMAC_RX_THRESH_CTRL_S))
#define EMAC_RX_THRESH_CTRL_V  0x3
#define EMAC_RX_THRESH_CTRL_S  3
/* EMAC_OPT_SECOND_FRAME : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: When this bit is set  it instructs the DMA to process the second
 frame of the Transmit data even before the status for the first frame is obtained.*/
#define EMAC_OPT_SECOND_FRAME  (BIT(2))
#define EMAC_OPT_SECOND_FRAME_M  (BIT(2))
#define EMAC_OPT_SECOND_FRAME_V  0x1
#define EMAC_OPT_SECOND_FRAME_S  2
/* EMAC_START_STOP_RX : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: When this bit is set  the Receive process is placed in the Running
 state. The DMA attempts to acquire the descriptor from the Receive list and processes the incoming frames.When this bit is cleared  the Rx DMA operation is stopped after the transfer of the current frame.*/
#define EMAC_START_STOP_RX  (BIT(1))
#define EMAC_START_STOP_RX_M  (BIT(1))
#define EMAC_START_STOP_RX_V  0x1
#define EMAC_START_STOP_RX_S  1

#define EMAC_DMAIN_EN_REG          (DR_REG_EMAC_BASE + 0x001C)
/* EMAC_DMAIN_NISE : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: When this bit is set  normal interrupt summary is enabled. When
 this bit is reset  normal interrupt summary is disabled. This bit enables the following interrupts in Status Register:  Bit[0]: Transmit Interrupt.  Bit[2]: Transmit Buffer Unavailable.  Bit[6]: Receive Interrupt.  Bit[14]: Early Receive Interrupt.*/
#define EMAC_DMAIN_NISE  (BIT(16))
#define EMAC_DMAIN_NISE_M  (BIT(16))
#define EMAC_DMAIN_NISE_V  0x1
#define EMAC_DMAIN_NISE_S  16
/* EMAC_DMAIN_AISE : R/W ;bitpos:[15] ;default: 1'h0 ; */
/*description: When this bit is set  abnormal interrupt summary is enabled.
 When this bit is reset the abnormal interrupt summary is disabled. This bit enables the following interrupts in Status Register:  Bit[1]: Transmit Process Stopped.  Bit[3]: Transmit Jabber Timeout.  Bit[4]: Receive Overflow.  Bit[5]: Transmit Underflow.  Bit[7]: Receive Buffer Unavailable.  Bit[8]: Receive Process Stopped.  Bit[9]: Receive Watchdog Timeout.  Bit[10]: Early Transmit Interrupt.  Bit[13]: Fatal Bus Error.*/
#define EMAC_DMAIN_AISE  (BIT(15))
#define EMAC_DMAIN_AISE_M  (BIT(15))
#define EMAC_DMAIN_AISE_V  0x1
#define EMAC_DMAIN_AISE_S  15
/* EMAC_DMAIN_ERIE : R/W ;bitpos:[14] ;default: 1'h0 ; */
/*description: When this bit is set with Normal Interrupt Summary Enable (Bit[16])
  the Early Receive Interrupt is enabled. When this bit is reset  the Early Receive Interrupt is disabled.*/
#define EMAC_DMAIN_ERIE  (BIT(14))
#define EMAC_DMAIN_ERIE_M  (BIT(14))
#define EMAC_DMAIN_ERIE_V  0x1
#define EMAC_DMAIN_ERIE_S  14
/* EMAC_DMAIN_FBEE : R/W ;bitpos:[13] ;default: 1'h0 ; */
/*description: When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])
  the Fatal Bus Error Interrupt is enabled. When this bit is reset  the Fatal Bus Error Enable Interrupt is disabled.*/
#define EMAC_DMAIN_FBEE  (BIT(13))
#define EMAC_DMAIN_FBEE_M  (BIT(13))
#define EMAC_DMAIN_FBEE_V  0x1
#define EMAC_DMAIN_FBEE_S  13
/* EMAC_DMAIN_ETIE : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: When this bit is set with an Abnormal Interrupt Summary Enable
 (Bit[15])  the Early Transmit Interrupt is enabled. When this bit is reset  the Early Transmit Interrupt is disabled.*/
#define EMAC_DMAIN_ETIE  (BIT(10))
#define EMAC_DMAIN_ETIE_M  (BIT(10))
#define EMAC_DMAIN_ETIE_V  0x1
#define EMAC_DMAIN_ETIE_S  10
/* EMAC_DMAIN_RWTE : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])
  the Receive Watchdog Timeout Interrupt is enabled. When this bit is reset  the Receive Watchdog Timeout Interrupt is disabled.*/
#define EMAC_DMAIN_RWTE  (BIT(9))
#define EMAC_DMAIN_RWTE_M  (BIT(9))
#define EMAC_DMAIN_RWTE_V  0x1
#define EMAC_DMAIN_RWTE_S  9
/* EMAC_DMAIN_RSE : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])
  the Receive Stopped Interrupt is enabled. When this bit is reset  the Receive Stopped Interrupt is disabled.*/
#define EMAC_DMAIN_RSE  (BIT(8))
#define EMAC_DMAIN_RSE_M  (BIT(8))
#define EMAC_DMAIN_RSE_V  0x1
#define EMAC_DMAIN_RSE_S  8
/* EMAC_DMAIN_RBUE : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])
  the Receive Buffer Unavailable Interrupt is enabled. When this bit is reset  the Receive Buffer Unavailable Interrupt is disabled.*/
#define EMAC_DMAIN_RBUE  (BIT(7))
#define EMAC_DMAIN_RBUE_M  (BIT(7))
#define EMAC_DMAIN_RBUE_V  0x1
#define EMAC_DMAIN_RBUE_S  7
/* EMAC_DMAIN_RIE : R/W ;bitpos:[6] ;default: 1'h0 ; */
/*description: When this bit is set with Normal Interrupt Summary Enable (Bit[16])
  the Receive Interrupt is enabled. When this bit is reset  the Receive Interrupt is disabled.*/
#define EMAC_DMAIN_RIE  (BIT(6))
#define EMAC_DMAIN_RIE_M  (BIT(6))
#define EMAC_DMAIN_RIE_V  0x1
#define EMAC_DMAIN_RIE_S  6
/* EMAC_DMAIN_UIE : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])
  the Transmit Underflow Interrupt is enabled. When this bit is reset  the Underflow Interrupt is disabled.*/
#define EMAC_DMAIN_UIE  (BIT(5))
#define EMAC_DMAIN_UIE_M  (BIT(5))
#define EMAC_DMAIN_UIE_V  0x1
#define EMAC_DMAIN_UIE_S  5
/* EMAC_DMAIN_OIE : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])
  the Receive Overflow Interrupt is enabled. When this bit is reset  the Overflow Interrupt is disabled.*/
#define EMAC_DMAIN_OIE  (BIT(4))
#define EMAC_DMAIN_OIE_M  (BIT(4))
#define EMAC_DMAIN_OIE_V  0x1
#define EMAC_DMAIN_OIE_S  4
/* EMAC_DMAIN_TJTE : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])
  the Transmit Jabber Timeout Interrupt is enabled. When this bit is reset  the Transmit Jabber Timeout Interrupt is disabled.*/
#define EMAC_DMAIN_TJTE  (BIT(3))
#define EMAC_DMAIN_TJTE_M  (BIT(3))
#define EMAC_DMAIN_TJTE_V  0x1
#define EMAC_DMAIN_TJTE_S  3
/* EMAC_DMAIN_TBUE : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: When this bit is set with Normal Interrupt Summary Enable (Bit
 16)  the Transmit Buffer Unavailable Interrupt is enabled. When this bit is reset  the Transmit Buffer Unavailable Interrupt is Disabled.*/
#define EMAC_DMAIN_TBUE  (BIT(2))
#define EMAC_DMAIN_TBUE_M  (BIT(2))
#define EMAC_DMAIN_TBUE_V  0x1
#define EMAC_DMAIN_TBUE_S  2
/* EMAC_DMAIN_TSE : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])
  the Transmission Stopped Interrupt is enabled. When this bit is reset  the Transmission Stopped Interrupt is disabled.*/
#define EMAC_DMAIN_TSE  (BIT(1))
#define EMAC_DMAIN_TSE_M  (BIT(1))
#define EMAC_DMAIN_TSE_V  0x1
#define EMAC_DMAIN_TSE_S  1
/* EMAC_DMAIN_TIE : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: When this bit is set with Normal Interrupt Summary Enable (Bit[16])
  the Transmit Interrupt is enabled. When this bit is reset  the Transmit Interrupt is disabled.*/
#define EMAC_DMAIN_TIE  (BIT(0))
#define EMAC_DMAIN_TIE_M  (BIT(0))
#define EMAC_DMAIN_TIE_V  0x1
#define EMAC_DMAIN_TIE_S  0

#define EMAC_DMAMISSEDFR_REG          (DR_REG_EMAC_BASE + 0x0020)
/* EMAC_OVERFLOW_BFOC : R_SS_RC ;bitpos:[28] ;default: 1'h0 ; */
/*description: This bit is set every time the Overflow Frame Counter (Bits[27:17])
 overflows  that is  the Rx FIFO overflows with the overflow frame counter at maximum value. In such a scenario  the overflow frame counter is reset to all-zeros and this bit indicates that the rollover happened.*/
#define EMAC_OVERFLOW_BFOC  (BIT(28))
#define EMAC_OVERFLOW_BFOC_M  (BIT(28))
#define EMAC_OVERFLOW_BFOC_V  0x1
#define EMAC_OVERFLOW_BFOC_S  28
/* EMAC_OVERFLOW_FC : R_SS_RC ;bitpos:[27:17] ;default: 11'h0 ; */
/*description: This field indicates the number of frames missed by the application.
 This counter is incremented each time the MTL FIFO overflows. The counter is cleared when this register is read.*/
#define EMAC_OVERFLOW_FC  0x000007FF
#define EMAC_OVERFLOW_FC_M  ((EMAC_OVERFLOW_FC_V)<<(EMAC_OVERFLOW_FC_S))
#define EMAC_OVERFLOW_FC_V  0x7FF
#define EMAC_OVERFLOW_FC_S  17
/* EMAC_OVERFLOW_BMFC : R_SS_RC ;bitpos:[16] ;default: 1'h0 ; */
/*description: This bit is set every time Missed Frame Counter (Bits[15:0])
 overflows that is  the DMA discards an incoming frame because of the Host Receive  Buffer being unavailable with the missed frame counter at maximum value. In such a scenario  the Missed frame counter is reset to all-zeros and this bit indicates that the rollover happened.*/
#define EMAC_OVERFLOW_BMFC  (BIT(16))
#define EMAC_OVERFLOW_BMFC_M  (BIT(16))
#define EMAC_OVERFLOW_BMFC_V  0x1
#define EMAC_OVERFLOW_BMFC_S  16
/* EMAC_MISSED_FC : R_SS_RC ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: This field indicates the number of frames missed by the controller
 because of the Host Receive Buffer being unavailable. This counter is incremented each time the DMA discards an incoming frame. The counter is cleared when this register is read.*/
#define EMAC_MISSED_FC  0x0000FFFF
#define EMAC_MISSED_FC_M  ((EMAC_MISSED_FC_V)<<(EMAC_MISSED_FC_S))
#define EMAC_MISSED_FC_V  0xFFFF
#define EMAC_MISSED_FC_S  0

#define EMAC_DMARINTWDTIMER_REG          (DR_REG_EMAC_BASE + 0x0024)
/* EMAC_RIWTC : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: This bit indicates the number of system clock cycles multiplied
 by 256 for which the watchdog timer is set. The watchdog timer gets triggered with the programmed value after the Rx DMA completes the transfer of a frame for which the RI(RECV_INT) status bit is not set because of the setting in the corresponding descriptor RDES1[31]. When the watchdog timer runs out  the RI bit is set and the timer is stopped. The watchdog timer is reset when the RI bit is set high because of automatic setting of RI as per RDES1[31] of any received frame.*/
#define EMAC_RIWTC  0x000000FF
#define EMAC_RIWTC_M  ((EMAC_RIWTC_V)<<(EMAC_RIWTC_S))
#define EMAC_RIWTC_V  0xFF
#define EMAC_RIWTC_S  0

#define EMAC_DMATXCURRDESC_REG          (DR_REG_EMAC_BASE + 0x0048)
/* EMAC_TRANS_DSCR_ADDR_PTR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The address of the current receive descriptor list. Cleared on
 Reset.Pointer updated by the DMA during operation.*/
#define EMAC_TRANS_DSCR_ADDR_PTR  0xFFFFFFFF
#define EMAC_TRANS_DSCR_ADDR_PTR_M  ((EMAC_TRANS_DSCR_ADDR_PTR_V)<<(EMAC_TRANS_DSCR_ADDR_PTR_S))
#define EMAC_TRANS_DSCR_ADDR_PTR_V  0xFFFFFFFF
#define EMAC_TRANS_DSCR_ADDR_PTR_S  0

#define EMAC_DMARXCURRDESC_REG          (DR_REG_EMAC_BASE + 0x004C)
/* EMAC_RECV_DSCR_ADDR_PTR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The address of the current receive descriptor list. Cleared on
 Reset.Pointer updated by the DMA during operation.*/
#define EMAC_RECV_DSCR_ADDR_PTR  0xFFFFFFFF
#define EMAC_RECV_DSCR_ADDR_PTR_M  ((EMAC_RECV_DSCR_ADDR_PTR_V)<<(EMAC_RECV_DSCR_ADDR_PTR_S))
#define EMAC_RECV_DSCR_ADDR_PTR_V  0xFFFFFFFF
#define EMAC_RECV_DSCR_ADDR_PTR_S  0

#define EMAC_DMATXCURRADDR_BUF_REG          (DR_REG_EMAC_BASE + 0x0050)
/* EMAC_TRANS_BUFF_ADDR_PTR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The address of the current receive descriptor list. Cleared on
 Reset.Pointer updated by the DMA during operation.*/
#define EMAC_TRANS_BUFF_ADDR_PTR  0xFFFFFFFF
#define EMAC_TRANS_BUFF_ADDR_PTR_M  ((EMAC_TRANS_BUFF_ADDR_PTR_V)<<(EMAC_TRANS_BUFF_ADDR_PTR_S))
#define EMAC_TRANS_BUFF_ADDR_PTR_V  0xFFFFFFFF
#define EMAC_TRANS_BUFF_ADDR_PTR_S  0

#define EMAC_DMARXCURRADDR_BUF_REG          (DR_REG_EMAC_BASE + 0x0054)
/* EMAC_RECV_BUFF_ADDR_PTR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The address of the current receive descriptor list. Cleared on
 Reset.Pointer updated by the DMA during operation.*/
#define EMAC_RECV_BUFF_ADDR_PTR  0xFFFFFFFF
#define EMAC_RECV_BUFF_ADDR_PTR_M  ((EMAC_RECV_BUFF_ADDR_PTR_V)<<(EMAC_RECV_BUFF_ADDR_PTR_S))
#define EMAC_RECV_BUFF_ADDR_PTR_V  0xFFFFFFFF
#define EMAC_RECV_BUFF_ADDR_PTR_S  0

#define EMAC_GMACCONFIG_REG          (DR_REG_EMAC_BASE + 0x1000)
/* EMAC_SAIRC : R/W ;bitpos:[30:28] ;default: 3'h0 ; */
/*description: This field controls the source address insertion or replacement
 for all transmitted frames.Bit[30] specifies which MAC Address register (0 or 1) is used for source address insertion or replacement based on the values of Bits [29:28]:  2'b0x: The input signals mti_sa_ctrl_i and ati_sa_ctrl_i control the SA field generation.  2'b10: If Bit[30] is set to 0  the MAC inserts the content of the MAC Address 0 registers in the SA field of all transmitted frames. If Bit[30] is set to 1 the MAC inserts the content of the MAC Address 1 registers in the SA field of all transmitted frames.  2'b11: If Bit[30] is set to 0  the MAC replaces the content of the MAC Address 0 registers in the SA field of all transmitted frames. If Bit[30] is set to 1  the MAC replaces the content of the MAC Address 1 registers in the SA field of all transmitted frames.*/
#define EMAC_SAIRC  0x00000007
#define EMAC_SAIRC_M  ((EMAC_SAIRC_V)<<(EMAC_SAIRC_S))
#define EMAC_SAIRC_V  0x7
#define EMAC_SAIRC_S  28
/* EMAC_ASS2KP : R/W ;bitpos:[27] ;default: 1'h0 ; */
/*description: When set  the MAC considers all frames  with up to 2 000 bytes
 length  as normal packets.When Bit[20] (JE) is not set  the MAC considers all received frames of size more than 2K bytes as Giant frames. When this bit is reset and Bit[20] (JE) is not set  the MAC considers all received frames of size more than 1 518 bytes (1 522 bytes for tagged) as Giant frames. When Bit[20] is set  setting this bit has no effect on Giant Frame status.*/
#define EMAC_ASS2KP  (BIT(27))
#define EMAC_ASS2KP_M  (BIT(27))
#define EMAC_ASS2KP_V  0x1
#define EMAC_ASS2KP_S  27
/* EMAC_EMACWATCHDOG : R/W ;bitpos:[23] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC disables the watchdog timer on
 the receiver. The MAC can receive frames of up to 16 383 bytes. When this bit is reset  the MAC does not allow a receive frame which more than 2 048 bytes (10 240 if JE is set high) or the value programmed in Register (Watchdog Timeout Register). The MAC cuts off any bytes received after the watchdog limit number of bytes.*/
#define EMAC_EMACWATCHDOG  (BIT(23))
#define EMAC_EMACWATCHDOG_M  (BIT(23))
#define EMAC_EMACWATCHDOG_V  0x1
#define EMAC_EMACWATCHDOG_S  23
/* EMAC_EMACJABBER : R/W ;bitpos:[22] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC disables the jabber timer on the
 transmitter. The MAC can transfer frames of up to 16 383 bytes. When this bit is reset  the MAC cuts off the transmitter if the application sends out more than 2 048 bytes of data (10 240 if JE is set high) during Transmission.*/
#define EMAC_EMACJABBER  (BIT(22))
#define EMAC_EMACJABBER_M  (BIT(22))
#define EMAC_EMACJABBER_V  0x1
#define EMAC_EMACJABBER_S  22
/* EMAC_EMACJUMBOFRAME : R/W ;bitpos:[20] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC allows Jumbo frames of 9 018 bytes
 (9 022 bytes for VLAN tagged frames) without reporting a giant frame error in the receive frame status.*/
#define EMAC_EMACJUMBOFRAME  (BIT(20))
#define EMAC_EMACJUMBOFRAME_M  (BIT(20))
#define EMAC_EMACJUMBOFRAME_V  0x1
#define EMAC_EMACJUMBOFRAME_S  20
/* EMAC_EMACINTERFRAMEGAP : R/W ;bitpos:[19:17] ;default: 1'h0 ; */
/*description: These bits control the minimum IFG between frames during transmission.
  3'b000: 96 bit times.  3'b001: 88 bit times.  3'b010: 80 bit times.  3'b111: 40 bit times. In the half-duplex mode  the minimum IFG can be configured only for 64 bit times (IFG = 100). Lower values are not considered.*/
#define EMAC_EMACINTERFRAMEGAP  0x00000007
#define EMAC_EMACINTERFRAMEGAP_M  ((EMAC_EMACINTERFRAMEGAP_V)<<(EMAC_EMACINTERFRAMEGAP_S))
#define EMAC_EMACINTERFRAMEGAP_V  0x7
#define EMAC_EMACINTERFRAMEGAP_S  17
/* EMAC_EMACDISABLECRS : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: When set high  this bit makes the MAC transmitter ignore the
 MII CRS signal during frame transmission in the half-duplex mode. This request results in no errors generated because of Loss of Carrier or No Carrier during such transmission. When this bit is low  the MAC transmitter generates such errors because of Carrier Sense and can even abort the transmissions.*/
#define EMAC_EMACDISABLECRS  (BIT(16))
#define EMAC_EMACDISABLECRS_M  (BIT(16))
#define EMAC_EMACDISABLECRS_V  0x1
#define EMAC_EMACDISABLECRS_S  16
/* EMAC_EMACMII : R/W ;bitpos:[15] ;default: 1'h0 ; */
/*description: This bit selects the Ethernet line speed. It should be set to
 1 for 10 or 100 Mbps operations.In 10 or 100 Mbps operations  this bit  along with FES(EMACFESPEED) bit  it selects the exact linespeed. In the 10/100 Mbps-only operations  the bit is always 1.*/
#define EMAC_EMACMII  (BIT(15))
#define EMAC_EMACMII_M  (BIT(15))
#define EMAC_EMACMII_V  0x1
#define EMAC_EMACMII_S  15
/* EMAC_EMACFESPEED : R/W ;bitpos:[14] ;default: 1'h0 ; */
/*description: This bit selects the speed in the MII  RMII interface. 0: 10 Mbps. 1: 100 Mbps.*/
#define EMAC_EMACFESPEED  (BIT(14))
#define EMAC_EMACFESPEED_M  (BIT(14))
#define EMAC_EMACFESPEED_V  0x1
#define EMAC_EMACFESPEED_S  14
/* EMAC_EMACRXOWN : R/W ;bitpos:[13] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC disables the reception of frames
 when the TX_EN is asserted in the half-duplex mode. When this bit is reset  the MAC receives all packets that are given by the PHY while transmitting. This bit is not applicable if the MAC is operating in the full duplex mode.*/
#define EMAC_EMACRXOWN  (BIT(13))
#define EMAC_EMACRXOWN_M  (BIT(13))
#define EMAC_EMACRXOWN_V  0x1
#define EMAC_EMACRXOWN_S  13
/* EMAC_EMACLOOPBACK : R/W ;bitpos:[12] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC operates in the loopback mode MII.
 The MII Receive clock input (CLK_RX) is required for the loopback to work properly  because the transmit clock is not looped-back internally.*/
#define EMAC_EMACLOOPBACK  (BIT(12))
#define EMAC_EMACLOOPBACK_M  (BIT(12))
#define EMAC_EMACLOOPBACK_V  0x1
#define EMAC_EMACLOOPBACK_S  12
/* EMAC_EMACDUPLEX : R/W ;bitpos:[11] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC operates in the full-duplex mode
 where it can transmit and receive simultaneously. This bit is read only with default value of 1'b1 in the full-duplex-mode.*/
#define EMAC_EMACDUPLEX  (BIT(11))
#define EMAC_EMACDUPLEX_M  (BIT(11))
#define EMAC_EMACDUPLEX_V  0x1
#define EMAC_EMACDUPLEX_S  11
/* EMAC_EMACRXIPCOFFLOAD : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC calculates the 16-bit one's complement
 of the one's complement sum of all received Ethernet frame payloads. It also checks whether the IPv4 Header checksum (assumed to be bytes 25/26 or 29/30 (VLAN-tagged) of the received Ethernet frame) is correct for the received frame and gives the status in the receive status word. The MAC also appends the 16-bit checksum calculated for the IP header datagram payload (bytes after the IPv4 header) and appends it to the Ethernet frame transferred to the application (when Type 2 COE is deselected). When this bit is reset  this function is disabled.*/
#define EMAC_EMACRXIPCOFFLOAD  (BIT(10))
#define EMAC_EMACRXIPCOFFLOAD_M  (BIT(10))
#define EMAC_EMACRXIPCOFFLOAD_V  0x1
#define EMAC_EMACRXIPCOFFLOAD_S  10
/* EMAC_EMACRETRY : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC attempts only one transmission.
 When a collision occurs on the MII interface  the MAC ignores the current frame transmission and reports a Frame Abort with excessive collision error in the transmit frame status. When this bit is reset  the MAC attempts retries based on the settings of the BL field (Bits [6:5]). This bit is applicable only in the half-duplex Mode.*/
#define EMAC_EMACRETRY  (BIT(9))
#define EMAC_EMACRETRY_M  (BIT(9))
#define EMAC_EMACRETRY_V  0x1
#define EMAC_EMACRETRY_S  9
/* EMAC_EMACPADCRCSTRIP : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC strips the Pad or FCS field on
 the incoming frames only if the value of the length field is less than 1 536 bytes. All received frames with length field greater than or equal to 1 536 bytes are passed to the application without stripping the Pad or FCS field. When this bit is reset  the MAC passes all incoming frames  without modifying them to the Host.*/
#define EMAC_EMACPADCRCSTRIP  (BIT(7))
#define EMAC_EMACPADCRCSTRIP_M  (BIT(7))
#define EMAC_EMACPADCRCSTRIP_V  0x1
#define EMAC_EMACPADCRCSTRIP_S  7
/* EMAC_EMACBACKOFFLIMIT : R/W ;bitpos:[6:5] ;default: 2'h0 ; */
/*description: The Back-Off limit determines the random integer number (r) of
 slot time delays (512 bit times for 10/100 Mbps) for which the MAC waits before rescheduling a transmission attempt during retries after a collision. This bit is applicable only in the half-duplex mode.  00: k= min (n  10).  01: k = min (n  8).  10: k = min (n  4).  11: k = min (n  1)  n = retransmission attempt. The random integer r takes the value in the Range 0 ~ 2000.*/
#define EMAC_EMACBACKOFFLIMIT  0x00000003
#define EMAC_EMACBACKOFFLIMIT_M  ((EMAC_EMACBACKOFFLIMIT_V)<<(EMAC_EMACBACKOFFLIMIT_S))
#define EMAC_EMACBACKOFFLIMIT_V  0x3
#define EMAC_EMACBACKOFFLIMIT_S  5
/* EMAC_EMACDEFERRALCHECK : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: Deferral Check.*/
#define EMAC_EMACDEFERRALCHECK  (BIT(4))
#define EMAC_EMACDEFERRALCHECK_M  (BIT(4))
#define EMAC_EMACDEFERRALCHECK_V  0x1
#define EMAC_EMACDEFERRALCHECK_S  4
/* EMAC_EMACTX : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: When this bit is set  the transmit state machine of the MAC is
 enabled for transmission on the MII. When this bit is reset  the MAC transmit state machine is disabled after the completion of the transmission of the current frame  and does not transmit any further frames.*/
#define EMAC_EMACTX  (BIT(3))
#define EMAC_EMACTX_M  (BIT(3))
#define EMAC_EMACTX_V  0x1
#define EMAC_EMACTX_S  3
/* EMAC_EMACRX : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: When this bit is set  the receiver state machine of the MAC is
 enabled for receiving frames from the MII. When this bit is reset  the MAC receive state machine is disabled after the completion of the reception of the current frame  and does not receive any further frames from the MII.*/
#define EMAC_EMACRX  (BIT(2))
#define EMAC_EMACRX_M  (BIT(2))
#define EMAC_EMACRX_V  0x1
#define EMAC_EMACRX_S  2
/* EMAC_PLTF : R/W ;bitpos:[1:0] ;default: 2'h0 ; */
/*description: These bits control the number of preamble bytes that are added
 to the beginning of every Transmit frame. The preamble reduction occurs only when the MAC is operating in the full-duplex mode.2'b00: 7 bytes of preamble. 2'b01: 5 bytes of preamble. 2'b10: 3 bytes of preamble.*/
#define EMAC_PLTF  0x00000003
#define EMAC_PLTF_M  ((EMAC_PLTF_V)<<(EMAC_PLTF_S))
#define EMAC_PLTF_V  0x3
#define EMAC_PLTF_S  0

#define EMAC_GMACFF_REG          (DR_REG_EMAC_BASE + 0x1004)
/* EMAC_RECEIVE_ALL : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC Receiver module passes all received
 frames  irrespective of whether they pass the address filter or not  to the Application. The result of the SA or DA filtering is updated (pass or fail) in the corresponding bits in the Receive Status Word. When this bit is reset the Receiver module passes only those frames to the Application that pass the SA or DA address Filter.*/
#define EMAC_RECEIVE_ALL  (BIT(31))
#define EMAC_RECEIVE_ALL_M  (BIT(31))
#define EMAC_RECEIVE_ALL_V  0x1
#define EMAC_RECEIVE_ALL_S  31
/* EMAC_SAFE : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC compares the SA field of the received
 frames with the values programmed in the enabled SA registers. If the comparison fails  the MAC drops the frame. When this bit is reset  the MAC forwards the received frame to the application with updated SAF bit of the Rx Status depending on the SA address comparison.*/
#define EMAC_SAFE  (BIT(9))
#define EMAC_SAFE_M  (BIT(9))
#define EMAC_SAFE_V  0x1
#define EMAC_SAFE_S  9
/* EMAC_SAIF : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: When this bit is set  the Address Check block operates in inverse
 filtering mode for the SA address comparison. The frames whose SA matches the SA registers are marked as failing the SA Address filter. When this bit is reset  frames whose SA does not match the SA registers are marked as failing the SA Address filter.*/
#define EMAC_SAIF  (BIT(8))
#define EMAC_SAIF_M  (BIT(8))
#define EMAC_SAIF_V  0x1
#define EMAC_SAIF_S  8
/* EMAC_PCF : R/W ;bitpos:[7:6] ;default: 2'h0 ; */
/*description: These bits control the forwarding of all control frames (including
 unicast and multicast Pause frames).   2'b00: MAC filters all control frames from reaching the application.  2'b01: MAC forwards all control frames except Pause frames to application even if they fail the Address filter.  2'b10: MAC forwards all control frames to application even if they fail the Address Filter.  2'b11: MAC forwards control frames that pass the Address Filter.The following conditions should be true for the Pause frames processing:  Condition 1: The MAC is in the full-duplex mode and flow control is enabled by setting Bit 2 (RFE) of Register (Flow Control Register) to 1.  Condition 2: The destination address (DA) of the received frame matches the special multicast address or the MAC Address 0 when Bit 3 (UP) of the Register(Flow Control Register) is set.  Condition 3: The Type field of the received frame is 0x8808 and the OPCODE field is 0x0001.*/
#define EMAC_PCF  0x00000003
#define EMAC_PCF_M  ((EMAC_PCF_V)<<(EMAC_PCF_S))
#define EMAC_PCF_V  0x3
#define EMAC_PCF_S  6
/* EMAC_DBF : R/W ;bitpos:[5] ;default: 1'h0 ; */
/*description: When this bit is set  the AFM(Address Filtering Module) module
 blocks all incoming broadcast frames. In addition  it overrides all other filter settings. When this bit is reset  the AFM module passes all received broadcast Frames.*/
#define EMAC_DBF  (BIT(5))
#define EMAC_DBF_M  (BIT(5))
#define EMAC_DBF_V  0x1
#define EMAC_DBF_S  5
/* EMAC_PAM : R/W ;bitpos:[4] ;default: 1'h0 ; */
/*description: When set  this bit indicates that all received frames with a
 multicast destination address (first bit in the destination address field is '1') are passed.*/
#define EMAC_PAM  (BIT(4))
#define EMAC_PAM_M  (BIT(4))
#define EMAC_PAM_V  0x1
#define EMAC_PAM_S  4
/* EMAC_DAIF : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: When this bit is set  the Address Check block operates in inverse
 filtering mode for the DA address comparison for both unicast and multicast frames. When reset  normal filtering of frames is performed.*/
#define EMAC_DAIF  (BIT(3))
#define EMAC_DAIF_M  (BIT(3))
#define EMAC_DAIF_V  0x1
#define EMAC_DAIF_S  3
/* EMAC_PMODE : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: When this bit is set  the Address Filter module passes all incoming
 frames irrespective of the destination or source address. The SA or DA Filter Fails status bits of the Receive Status Word are always cleared when PR(PRI_RATIO) is set.*/
#define EMAC_PMODE  (BIT(0))
#define EMAC_PMODE_M  (BIT(0))
#define EMAC_PMODE_V  0x1
#define EMAC_PMODE_S  0

#define EMAC_GMIIADDR_REG          (DR_REG_EMAC_BASE + 0x1010)
/* EMAC_MIIDEV : R/W ;bitpos:[15:11] ;default: 6'h0 ; */
/*description: This field indicates which of the 32 possible PHY devices are being accessed.*/
#define EMAC_MIIDEV  0x0000001F
#define EMAC_MIIDEV_M  ((EMAC_MIIDEV_V)<<(EMAC_MIIDEV_S))
#define EMAC_MIIDEV_V  0x1F
#define EMAC_MIIDEV_S  11
/* EMAC_MIIREG : R/W ;bitpos:[10:6] ;default: 5'h0 ; */
/*description: These bits select the desired MII register in the selected PHY device.*/
#define EMAC_MIIREG  0x0000001F
#define EMAC_MIIREG_M  ((EMAC_MIIREG_V)<<(EMAC_MIIREG_S))
#define EMAC_MIIREG_V  0x1F
#define EMAC_MIIREG_S  6
/* EMAC_MIICSRCLK : R/W ;bitpos:[5:2] ;default: 5'h0 ; */
/*description: CSR clock range: 1.0 MHz ~ 2.5 MHz.  4'b0000: When the APB clock
 frequency is 80 MHz  the MDC clock frequency is APB CLK/42  4'b0000: When the APB clock frequency is 40 MHz  the MDC clock frequency is APB CLK/26.*/
#define EMAC_MIICSRCLK  0x0000000F
#define EMAC_MIICSRCLK_M  ((EMAC_MIICSRCLK_V)<<(EMAC_MIICSRCLK_S))
#define EMAC_MIICSRCLK_V  0xF
#define EMAC_MIICSRCLK_S  2
/* EMAC_MIIWRITE : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: When set  this bit indicates to the PHY that this is a Write
 operation using the MII Data register. If this bit is not set  it indicates that this is a Read operation  that is  placing the data in the MII Data register.*/
#define EMAC_MIIWRITE  (BIT(1))
#define EMAC_MIIWRITE_M  (BIT(1))
#define EMAC_MIIWRITE_V  0x1
#define EMAC_MIIWRITE_S  1
/* EMAC_MIIBUSY : R_WS_SC ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit should read logic 0 before writing to PHY Addr Register
 and PHY data Register.During a PHY register access  the software sets this bit to 1'b1 to indicate that a Read or Write access is in progress. PHY data Register is invalid until this bit is cleared by the MAC. Therefore PHY data Register (MII Data) should be kept valid until the MAC clears this bit during a PHY Write operation. Similarly for a read operation  the contents of Register 5 are not valid until this bit is cleared. The subsequent read or write operation should happen only after the previous operation is complete. Because there is no acknowledgment from the PHY to MAC after a read or write operation is completed  there is no change in the functionality of this bit even when the PHY is not Present.*/
#define EMAC_MIIBUSY  (BIT(0))
#define EMAC_MIIBUSY_M  (BIT(0))
#define EMAC_MIIBUSY_V  0x1
#define EMAC_MIIBUSY_S  0

#define EMAC_MIIDATA_REG          (DR_REG_EMAC_BASE + 0x1014)
/* EMAC_MII_DATA : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: This field contains the 16-bit data value read from the PHY after
 a Management Read operation or the 16-bit data value to be written to the PHY before a Management Write operation.*/
#define EMAC_MII_DATA  0x0000FFFF
#define EMAC_MII_DATA_M  ((EMAC_MII_DATA_V)<<(EMAC_MII_DATA_S))
#define EMAC_MII_DATA_V  0xFFFF
#define EMAC_MII_DATA_S  0

#define EMAC_GMACFC_REG          (DR_REG_EMAC_BASE + 0x1018)
/* EMAC_PAUSE_TIME : R/W ;bitpos:[31:16] ;default: 16'h0 ; */
/*description: This field holds the value to be used in the Pause Time field
 in the transmit control frame. If the Pause Time bits is configured to be double-synchronized to the MII clock domain then consecutive writes to this register should be performed only after at least four clock cycles in the destination clock domain.*/
#define EMAC_PAUSE_TIME  0x0000FFFF
#define EMAC_PAUSE_TIME_M  ((EMAC_PAUSE_TIME_V)<<(EMAC_PAUSE_TIME_S))
#define EMAC_PAUSE_TIME_V  0xFFFF
#define EMAC_PAUSE_TIME_S  16
/* EMAC_DZPQ : R/W ;bitpos:[7] ;default: 1'h0 ; */
/*description: When this bit is set  it disables the automatic generation of
 the Zero-Quanta Pause frames on the de-assertion of the flow-control signal from the FIFO layer. When this bit is reset  normal operation with automatic Zero-Quanta Pause frame generation is enabled.*/
#define EMAC_DZPQ  (BIT(7))
#define EMAC_DZPQ_M  (BIT(7))
#define EMAC_DZPQ_V  0x1
#define EMAC_DZPQ_S  7
/* EMAC_PLT : R/W ;bitpos:[5:4] ;default: 2'h0 ; */
/*description: This field configures the threshold of the Pause timer automatic
 retransmission of the Pause frame.The threshold values should be always less than the Pause Time configured in Bits[31:16]. For example  if PT = 100H (256 slot-times)  and PLT = 01  then a second Pause frame is automatically transmitted at 228 (256-28) slot times after the first Pause frame is transmitted. The following list provides the threshold values for different values:   2'b00: The threshold is Pause time minus 4 slot times (PT-4 slot times).  2'b01: The threshold is Pause time minus 28 slot times (PT-28 slot times).  2'b10: The threshold is Pause time minus 144 slot times (PT-144 slot times).  2'b11: The threshold is Pause time minus 256 slot times (PT-256 slot times). The slot time is defined as the time taken to transmit 512 bits (64 bytes) on the MII interface.*/
#define EMAC_PLT  0x00000003
#define EMAC_PLT_M  ((EMAC_PLT_V)<<(EMAC_PLT_S))
#define EMAC_PLT_V  0x3
#define EMAC_PLT_S  4
/* EMAC_UPFD : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: A pause frame is processed when it has the unique multicast address
 specified in the IEEE Std 802.3. When this bit is set  the MAC can also detect Pause frames with unicast address of the station. This unicast address should be as specified in the EMACADDR0 High Register and EMACADDR0 Low Register. When this bit is reset  the MAC only detects Pause frames with unique multicast address.*/
#define EMAC_UPFD  (BIT(3))
#define EMAC_UPFD_M  (BIT(3))
#define EMAC_UPFD_V  0x1
#define EMAC_UPFD_S  3
/* EMAC_RFCE : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: When this bit is set  the MAC decodes the received Pause frame
 and disables its transmitter for a specified (Pause) time. When this bit is reset  the decode function of the Pause frame is disabled.*/
#define EMAC_RFCE  (BIT(2))
#define EMAC_RFCE_M  (BIT(2))
#define EMAC_RFCE_V  0x1
#define EMAC_RFCE_S  2
/* EMAC_TFCE : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: In the full-duplex mode  when this bit is set  the MAC enables
 the flow control operation to transmit Pause frames. When this bit is reset  the flow control operation in the MAC is disabled and the MAC does not transmit any Pause frames. In the half-duplex mode  when this bit is set the MAC enables the backpressure operation. When this bit is reset  the backpressure feature is Disabled.*/
#define EMAC_TFCE  (BIT(1))
#define EMAC_TFCE_M  (BIT(1))
#define EMAC_TFCE_V  0x1
#define EMAC_TFCE_S  1
/* EMAC_FCBBA : R_WS_SC(FCB)/R_W(BPA) ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit initiates a Pause frame in the full-duplex mode and
 activates the backpressure function in the half-duplex mode if the TFCE bit is set. In the full-duplex mode  this bit should be read as 1'b0 before writing to the Flow Control register. To initiate a Pause frame  the Application must set this bit to 1'b1. During a transfer of the Control Frame  this bit continues to be set to signify that a frame transmission is in progress. After the completion of Pause frame transmission  the MAC resets this bit to 1'b0. The Flow Control register should not be written to until this bit is cleared. In the half-duplex mode  when this bit is set (and TFCE is set)  then backpressure is asserted by the MAC. During backpressure  when the MAC receives a new frame  the transmitter starts sending a JAM pattern resulting in a collision. When the MAC is configured for the full-duplex mode  the BPA(backpressure activate) is automatically disabled.*/
#define EMAC_FCBBA  (BIT(0))
#define EMAC_FCBBA_M  (BIT(0))
#define EMAC_FCBBA_V  0x1
#define EMAC_FCBBA_S  0

#define EMAC_DEBUG_REG          (DR_REG_EMAC_BASE + 0x1024)
/* EMAC_MTLTSFFS : RO ;bitpos:[25] ;default: 1'h0 ; */
/*description: When high  this bit indicates that the MTL TxStatus FIFO is full.
 Therefore  the MTL cannot accept any more frames for transmission.*/
#define EMAC_MTLTSFFS  (BIT(25))
#define EMAC_MTLTSFFS_M  (BIT(25))
#define EMAC_MTLTSFFS_V  0x1
#define EMAC_MTLTSFFS_S  25
/* EMAC_MTLTFNES : RO ;bitpos:[24] ;default: 1'h0 ; */
/*description: When high  this bit indicates that the MTL Tx FIFO is not empty
 and some data is left for Transmission.*/
#define EMAC_MTLTFNES  (BIT(24))
#define EMAC_MTLTFNES_M  (BIT(24))
#define EMAC_MTLTFNES_V  0x1
#define EMAC_MTLTFNES_S  24
/* EMAC_MTLTFWCS : RO ;bitpos:[22] ;default: 1'h0 ; */
/*description: When high  this bit indicates that the MTL Tx FIFO Write Controller
 is active and is transferring data to the Tx FIFO.*/
#define EMAC_MTLTFWCS  (BIT(22))
#define EMAC_MTLTFWCS_M  (BIT(22))
#define EMAC_MTLTFWCS_V  0x1
#define EMAC_MTLTFWCS_S  22
/* EMAC_MTLTFRCS : RO ;bitpos:[21:20] ;default: 2'h0 ; */
/*description: This field indicates the state of the Tx FIFO Read Controller:
   2'b00: IDLE state.  2'b01: READ state (transferring data to the MAC transmitter).  2'b10: Waiting for TxStatus from the MAC transmitter.  2'b11: Writing the received TxStatus or flushing the Tx FIFO.*/
#define EMAC_MTLTFRCS  0x00000003
#define EMAC_MTLTFRCS_M  ((EMAC_MTLTFRCS_V)<<(EMAC_MTLTFRCS_S))
#define EMAC_MTLTFRCS_V  0x3
#define EMAC_MTLTFRCS_S  20
/* EMAC_MACTP : RO ;bitpos:[19] ;default: 1'h0 ; */
/*description: When high  this bit indicates that the MAC transmitter is in
 the Pause condition (in the full-duplex-mode) and hence does not schedule any frame for transmission.*/
#define EMAC_MACTP  (BIT(19))
#define EMAC_MACTP_M  (BIT(19))
#define EMAC_MACTP_V  0x1
#define EMAC_MACTP_S  19
/* EMAC_MACTFCS : RO ;bitpos:[18:17] ;default: 2'h0 ; */
/*description: This field indicates the state of the MAC Transmit Frame Controller
 module:   2'b00: IDLE state.  2'b01: Waiting for status of previous frame or IFG or backoff period to be over.  2'b10: Generating and transmitting a Pause frame (in the full-duplex mode).  2'b11: Transferring input frame for transmission.*/
#define EMAC_MACTFCS  0x00000003
#define EMAC_MACTFCS_M  ((EMAC_MACTFCS_V)<<(EMAC_MACTFCS_S))
#define EMAC_MACTFCS_V  0x3
#define EMAC_MACTFCS_S  17
/* EMAC_MACTPES : RO ;bitpos:[16] ;default: 1'h0 ; */
/*description: When high  this bit indicates that the MAC MII transmit protocol
 engine is actively transmitting data and is not in the IDLE state.*/
#define EMAC_MACTPES  (BIT(16))
#define EMAC_MACTPES_M  (BIT(16))
#define EMAC_MACTPES_V  0x1
#define EMAC_MACTPES_S  16
/* EMAC_MTLRFFLS : RO ;bitpos:[9:8] ;default: 2'h0 ; */
/*description: This field gives the status of the fill-level of the Rx FIFO:
   2'b00: Rx FIFO Empty.  2'b01: Rx FIFO fill-level below flow-control deactivate threshold.  2'b10: Rx FIFO fill-level above flow-control activate threshold.  2'b11: Rx FIFO Full.*/
#define EMAC_MTLRFFLS  0x00000003
#define EMAC_MTLRFFLS_M  ((EMAC_MTLRFFLS_V)<<(EMAC_MTLRFFLS_S))
#define EMAC_MTLRFFLS_V  0x3
#define EMAC_MTLRFFLS_S  8
/* EMAC_MTLRFRCS : RO ;bitpos:[6:5] ;default: 2'h0 ; */
/*description: This field gives the state of the Rx FIFO read Controller: 2'b00:
 IDLE state.2'b01: Reading frame data.2'b10: Reading frame status (or timestamp).2'b11: Flushing the frame data and status.*/
#define EMAC_MTLRFRCS  0x00000003
#define EMAC_MTLRFRCS_M  ((EMAC_MTLRFRCS_V)<<(EMAC_MTLRFRCS_S))
#define EMAC_MTLRFRCS_V  0x3
#define EMAC_MTLRFRCS_S  5
/* EMAC_MTLRFWCAS : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: When high  this bit indicates that the MTL Rx FIFO Write Controller
 is active and is transferring a received frame to the FIFO.*/
#define EMAC_MTLRFWCAS  (BIT(4))
#define EMAC_MTLRFWCAS_M  (BIT(4))
#define EMAC_MTLRFWCAS_V  0x1
#define EMAC_MTLRFWCAS_S  4
/* EMAC_MACRFFCS : RO ;bitpos:[2:1] ;default: 2'h0 ; */
/*description: When high  this field indicates the active state of the FIFO
 Read and Write controllers of the MAC Receive Frame Controller Module. MACRFFCS[1] represents the status of FIFO Read controller. MACRFFCS[0] represents the status of small FIFO Write controller.*/
#define EMAC_MACRFFCS  0x00000003
#define EMAC_MACRFFCS_M  ((EMAC_MACRFFCS_V)<<(EMAC_MACRFFCS_S))
#define EMAC_MACRFFCS_V  0x3
#define EMAC_MACRFFCS_S  1
/* EMAC_MACRPES : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: When high  this bit indicates that the MAC MII receive protocol
 engine is actively receiving data and not in IDLE state.*/
#define EMAC_MACRPES  (BIT(0))
#define EMAC_MACRPES_M  (BIT(0))
#define EMAC_MACRPES_V  0x1
#define EMAC_MACRPES_S  0

#define EMAC_PMT_RWUFFR_REG          (DR_REG_EMAC_BASE + 0x1028)
/* EMAC_WKUPPKTFILTER : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: The MSB (31st bit) must be zero.Bit j[30:0] is the byte mask.
 If Bit 1/2/3/4 (byte number) of the byte mask is set  the CRC block processes the Filter 0/1/2/3 Offset + j of the incoming packet(PWKPTR is 0/1/2/3).RWKPTR is 0:Filter 0 Byte Mask .RWKPTR is 1:Filter 1 Byte Mask RWKPTR is 2:Filter 2 Byte Mask RWKPTR is 3:Filter 3 Byte Mask RWKPTR is 4:Bit 3/11/19/27 specifies the address type  defining the destination address type of the pattern.When the bit is set  the pattern applies to only multicast packets*/
#define EMAC_WKUPPKTFILTER  0xFFFFFFFF
#define EMAC_WKUPPKTFILTER_M  ((EMAC_WKUPPKTFILTER_V)<<(EMAC_WKUPPKTFILTER_S))
#define EMAC_WKUPPKTFILTER_V  0xFFFFFFFF
#define EMAC_WKUPPKTFILTER_S  0

#define EMAC_PMT_CSR_REG          (DR_REG_EMAC_BASE + 0x102C)
/* EMAC_RWKFILTRST : R_WS_SC ;bitpos:[31] ;default: 1'h0 ; */
/*description: When this bit is set  it resets the RWKPTR register to 3’b000.*/
#define EMAC_RWKFILTRST  (BIT(31))
#define EMAC_RWKFILTRST_M  (BIT(31))
#define EMAC_RWKFILTRST_V  0x1
#define EMAC_RWKFILTRST_S  31
/* EMAC_RWKPTR : RO ;bitpos:[28:24] ;default: 6'h0 ; */
/*description: The maximum value of the pointer is 7  the detail information
 please refer to PMT_RWUFFR.*/
#define EMAC_RWKPTR  0x0000001F
#define EMAC_RWKPTR_M  ((EMAC_RWKPTR_V)<<(EMAC_RWKPTR_S))
#define EMAC_RWKPTR_V  0x1F
#define EMAC_RWKPTR_S  24
/* EMAC_GLBLUCAST : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: When set  enables any unicast packet filtered by the MAC (DAFilter)
 address recognition to be a remote wake-up frame.*/
#define EMAC_GLBLUCAST  (BIT(9))
#define EMAC_GLBLUCAST_M  (BIT(9))
#define EMAC_GLBLUCAST_V  0x1
#define EMAC_GLBLUCAST_S  9
/* EMAC_RWKPRCVD : R_SS_RC ;bitpos:[6] ;default: 1'h0 ; */
/*description: When set  this bit indicates the power management event is generated
 because of the reception of a remote wake-up frame. This bit is cleared by a Read into this register.*/
#define EMAC_RWKPRCVD  (BIT(6))
#define EMAC_RWKPRCVD_M  (BIT(6))
#define EMAC_RWKPRCVD_V  0x1
#define EMAC_RWKPRCVD_S  6
/* EMAC_MGKPRCVD : R_SS_RC ;bitpos:[5] ;default: 1'h0 ; */
/*description: When set  this bit indicates that the power management event
 is generated because of the reception of a magic packet. This bit is cleared by a Read into this register.*/
#define EMAC_MGKPRCVD  (BIT(5))
#define EMAC_MGKPRCVD_M  (BIT(5))
#define EMAC_MGKPRCVD_V  0x1
#define EMAC_MGKPRCVD_S  5
/* EMAC_RWKPKTEN : R/W ;bitpos:[2] ;default: 1'h0 ; */
/*description: When set  enables generation of a power management event because
 of remote wake-up frame reception*/
#define EMAC_RWKPKTEN  (BIT(2))
#define EMAC_RWKPKTEN_M  (BIT(2))
#define EMAC_RWKPKTEN_V  0x1
#define EMAC_RWKPKTEN_S  2
/* EMAC_MGKPKTEN : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: When set  enables generation of a power management event because
 of magic packet reception.*/
#define EMAC_MGKPKTEN  (BIT(1))
#define EMAC_MGKPKTEN_M  (BIT(1))
#define EMAC_MGKPKTEN_V  0x1
#define EMAC_MGKPKTEN_S  1
/* EMAC_PWRDWN : R_WS_SC ;bitpos:[0] ;default: 1'h0 ; */
/*description: When set  the MAC receiver drops all received frames until it
 receives the expected magic packet or remote wake-up frame.This bit must only be set when MGKPKTEN  GLBLUCAST  or RWKPKTEN bit is set high.*/
#define EMAC_PWRDWN  (BIT(0))
#define EMAC_PWRDWN_M  (BIT(0))
#define EMAC_PWRDWN_V  0x1
#define EMAC_PWRDWN_S  0

#define EMAC_GMACLPI_CRS_REG          (DR_REG_EMAC_BASE + 0x1030)
/* EMAC_LPITXA : R/W ;bitpos:[19] ;default: 1'h0 ; */
/*description: This bit controls the behavior of the MAC when it is entering
 or coming out of the LPI mode on the transmit side.If the LPITXA and LPIEN bits are set to 1  the MAC enters the LPI mode only after all outstanding frames and pending frames have been transmitted. The MAC comes out of the LPI mode when the application sends any frame.When this bit is 0  the LPIEN bit directly controls behavior of the MAC when it is entering or coming out of the LPI mode.*/
#define EMAC_LPITXA  (BIT(19))
#define EMAC_LPITXA_M  (BIT(19))
#define EMAC_LPITXA_V  0x1
#define EMAC_LPITXA_S  19
/* EMAC_PLS : R/W ;bitpos:[17] ;default: 1'h0 ; */
/*description: This bit indicates the link status of the PHY.When set  the link
 is considered to be okay (up) and when reset  the link is considered to be down.*/
#define EMAC_PLS  (BIT(17))
#define EMAC_PLS_M  (BIT(17))
#define EMAC_PLS_V  0x1
#define EMAC_PLS_S  17
/* EMAC_LPIEN : R_W_SC ;bitpos:[16] ;default: 1'h0 ; */
/*description: When set  this bit instructs the MAC Transmitter to enter the
 LPI state. When reset  this bit instructs the MAC to exit the LPI state and resume normal transmission.This bit is cleared when the LPITXA bit is set and the MAC exits the LPI state because of the arrival of a new packet for transmission.*/
#define EMAC_LPIEN  (BIT(16))
#define EMAC_LPIEN_M  (BIT(16))
#define EMAC_LPIEN_V  0x1
#define EMAC_LPIEN_S  16
/* EMAC_RLPIST : R/W ;bitpos:[9] ;default: 1'h0 ; */
/*description: When set  this bit indicates that the MAC is receiving the LPI
 pattern on the MII interface.*/
#define EMAC_RLPIST  (BIT(9))
#define EMAC_RLPIST_M  (BIT(9))
#define EMAC_RLPIST_V  0x1
#define EMAC_RLPIST_S  9
/* EMAC_TLPIST : R/W ;bitpos:[8] ;default: 1'h0 ; */
/*description: When set  this bit indicates that the MAC is transmitting the
 LPI pattern on the MII interface.*/
#define EMAC_TLPIST  (BIT(8))
#define EMAC_TLPIST_M  (BIT(8))
#define EMAC_TLPIST_V  0x1
#define EMAC_TLPIST_S  8
/* EMAC_RLPIEX : R_SS_RC ;bitpos:[3] ;default: 1'h0 ; */
/*description: When set  this bit indicates that the MAC Receiver has stopped
 receiving the LPI pattern on the MII interface  exited the LPI state  and resumed the normal reception. This bit is cleared by a read into this register.*/
#define EMAC_RLPIEX  (BIT(3))
#define EMAC_RLPIEX_M  (BIT(3))
#define EMAC_RLPIEX_V  0x1
#define EMAC_RLPIEX_S  3
/* EMAC_RLPIEN : R_SS_RC ;bitpos:[2] ;default: 1'h0 ; */
/*description: When set  this bit indicates that the MAC Receiver has received
 an LPI pattern and entered the LPI state. This bit is cleared by a read into this register.*/
#define EMAC_RLPIEN  (BIT(2))
#define EMAC_RLPIEN_M  (BIT(2))
#define EMAC_RLPIEN_V  0x1
#define EMAC_RLPIEN_S  2
/* EMAC_TLPIEX : R_SS_RC ;bitpos:[1] ;default: 1'h0 ; */
/*description: When set  this bit indicates that the MAC transmitter has exited
 the LPI state after the user has cleared the LPIEN bit and the LPI_TW_Timer has expired.This bit is cleared by a read into this register.*/
#define EMAC_TLPIEX  (BIT(1))
#define EMAC_TLPIEX_M  (BIT(1))
#define EMAC_TLPIEX_V  0x1
#define EMAC_TLPIEX_S  1
/* EMAC_TLPIEN : R_SS_RC ;bitpos:[0] ;default: 1'h0 ; */
/*description: When set  this bit indicates that the MAC Transmitter has entered
 the LPI state because of the setting of the LPIEN bit. This bit is cleared by a read into this register.*/
#define EMAC_TLPIEN  (BIT(0))
#define EMAC_TLPIEN_M  (BIT(0))
#define EMAC_TLPIEN_V  0x1
#define EMAC_TLPIEN_S  0

#define EMAC_GMACLPITIMERSCONTROL_REG          (DR_REG_EMAC_BASE + 0x1034)
/* EMAC_LPI_LS_TIMER : R/W ;bitpos:[25:16] ;default: 10'h3E8 ; */
/*description: This field specifies the minimum time (in milliseconds) for which
 the link status from the PHY should be up (OKAY) before the LPI pattern can be transmitted to the PHY. The MAC does not transmit the LPI pattern even when the LPIEN bit is set unless the LPI_LS_Timer reaches the programmed terminal count. The default value of the LPI_LS_Timer is 1000 (1 sec) as defined in the IEEE standard.*/
#define EMAC_LPI_LS_TIMER  0x000003FF
#define EMAC_LPI_LS_TIMER_M  ((EMAC_LPI_LS_TIMER_V)<<(EMAC_LPI_LS_TIMER_S))
#define EMAC_LPI_LS_TIMER_V  0x3FF
#define EMAC_LPI_LS_TIMER_S  16
/* EMAC_LPI_TW_TIMER : R/W ;bitpos:[15:0] ;default: 16'h0 ; */
/*description: This field specifies the minimum time (in microseconds) for which
 the MAC waits  after it stops transmitting the LPI pattern to the PHY and before it resumes the normal transmission. The TLPIEX status bit is set after the expiry of this timer.*/
#define EMAC_LPI_TW_TIMER  0x0000FFFF
#define EMAC_LPI_TW_TIMER_M  ((EMAC_LPI_TW_TIMER_V)<<(EMAC_LPI_TW_TIMER_S))
#define EMAC_LPI_TW_TIMER_V  0xFFFF
#define EMAC_LPI_TW_TIMER_S  0

#define EMAC_INTS_REG          (DR_REG_EMAC_BASE + 0x1038)
/* EMAC_LPIIS : RO ;bitpos:[10] ;default: 1'h0 ; */
/*description: When the Energy Efficient Ethernet feature is enabled  this bit
 is set for any LPI state entry or exit in the MAC Transmitter or Receiver. This bit is cleared on reading Bit[0] of Register (LPI Control and Status Register).*/
#define EMAC_LPIIS  (BIT(10))
#define EMAC_LPIIS_M  (BIT(10))
#define EMAC_LPIIS_V  0x1
#define EMAC_LPIIS_S  10
/* EMAC_PMTINTS : RO ;bitpos:[3] ;default: 1'h0 ; */
/*description: This bit is set when a magic packet or remote wake-up frame is
 received in the power-down mode (see Bit[5] and Bit[6] in the PMT Control and Status Register). This bit is cleared when both Bits[6:5] are cleared because of a read operation to the PMT Control and Status register. This bit is valid only when you select the optional PMT module during core configuration.*/
#define EMAC_PMTINTS  (BIT(3))
#define EMAC_PMTINTS_M  (BIT(3))
#define EMAC_PMTINTS_V  0x1
#define EMAC_PMTINTS_S  3

#define EMAC_INTMASK_REG          (DR_REG_EMAC_BASE + 0x103C)
/* EMAC_LPIINTMASK : R/W ;bitpos:[10] ;default: 1'h0 ; */
/*description: When set  this bit disables the assertion of the interrupt signal
 because of the setting of the LPI Interrupt Status bit in Register (Interrupt Status Register).*/
#define EMAC_LPIINTMASK  (BIT(10))
#define EMAC_LPIINTMASK_M  (BIT(10))
#define EMAC_LPIINTMASK_V  0x1
#define EMAC_LPIINTMASK_S  10
/* EMAC_PMTINTMASK : R/W ;bitpos:[3] ;default: 1'h0 ; */
/*description: When set  this bit disables the assertion of the interrupt signal
 because of the setting of PMT Interrupt Status bit in Register (Interrupt Status Register).*/
#define EMAC_PMTINTMASK  (BIT(3))
#define EMAC_PMTINTMASK_M  (BIT(3))
#define EMAC_PMTINTMASK_V  0x1
#define EMAC_PMTINTMASK_S  3

#define EMAC_ADDR0HIGH_REG          (DR_REG_EMAC_BASE + 0x1040)
/* EMAC_ADDRESS_ENABLE0 : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: This bit is always set to 1.*/
#define EMAC_ADDRESS_ENABLE0  (BIT(31))
#define EMAC_ADDRESS_ENABLE0_M  (BIT(31))
#define EMAC_ADDRESS_ENABLE0_V  0x1
#define EMAC_ADDRESS_ENABLE0_S  31
/* EMAC_ADDRESS0_HI : R/W ;bitpos:[15:0] ;default: 16'hFFFF ; */
/*description: This field contains the upper 16 bits (47:32) of the first 6-byte
 MAC address.The MAC uses this field for filtering the received frames and inserting the MAC address in the Transmit Flow Control (Pause) Frames.*/
#define EMAC_ADDRESS0_HI  0x0000FFFF
#define EMAC_ADDRESS0_HI_M  ((EMAC_ADDRESS0_HI_V)<<(EMAC_ADDRESS0_HI_S))
#define EMAC_ADDRESS0_HI_V  0xFFFF
#define EMAC_ADDRESS0_HI_S  0

#define EMAC_ADDR0LOW_REG          (DR_REG_EMAC_BASE + 0x1044)
/* EMAC_MAC_ADDRESS0_LOW : R/W ;bitpos:[31:0] ;default: 32'hFFFFFFFF ; */
/*description: This field contains the lower 32 bits of the first 6-byte MAC
 address. This is used by the MAC for filtering the received frames and inserting the MAC address in the Transmit Flow Control (Pause) Frames.*/
#define EMAC_MAC_ADDRESS0_LOW  0xFFFFFFFF
#define EMAC_MAC_ADDRESS0_LOW_M  ((EMAC_MAC_ADDRESS0_LOW_V)<<(EMAC_MAC_ADDRESS0_LOW_S))
#define EMAC_MAC_ADDRESS0_LOW_V  0xFFFFFFFF
#define EMAC_MAC_ADDRESS0_LOW_S  0

#define EMAC_ADDR1HIGH_REG          (DR_REG_EMAC_BASE + 0x1048)
/* EMAC_ADDRESS_ENABLE1 : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: When this bit is set  the address filter module uses the second
 MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
#define EMAC_ADDRESS_ENABLE1  (BIT(31))
#define EMAC_ADDRESS_ENABLE1_M  (BIT(31))
#define EMAC_ADDRESS_ENABLE1_V  0x1
#define EMAC_ADDRESS_ENABLE1_S  31
/* EMAC_SOURCE_ADDRESS : R/W ;bitpos:[30] ;default: 1'h0 ; */
/*description: When this bit is set  the EMACADDR1[47:0] is used to compare
 with the SA fields of the received frame. When this bit is reset  the EMACADDR1[47:0] is used to compare with the DA fields of the received frame.*/
#define EMAC_SOURCE_ADDRESS  (BIT(30))
#define EMAC_SOURCE_ADDRESS_M  (BIT(30))
#define EMAC_SOURCE_ADDRESS_V  0x1
#define EMAC_SOURCE_ADDRESS_S  30
/* EMAC_MASK_BYTE_CONTROL : R/W ;bitpos:[29:24] ;default: 6'h0 ; */
/*description: These bits are mask control bits for comparison of each of the
 EMACADDR1 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR1 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR1 High [15:8].  Bit[28]: EMACADDR1 High [7:0].  Bit[27]: EMACADDR1 Low [31:24].  Bit[24]: EMACADDR1 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
#define EMAC_MASK_BYTE_CONTROL  0x0000003F
#define EMAC_MASK_BYTE_CONTROL_M  ((EMAC_MASK_BYTE_CONTROL_V)<<(EMAC_MASK_BYTE_CONTROL_S))
#define EMAC_MASK_BYTE_CONTROL_V  0x3F
#define EMAC_MASK_BYTE_CONTROL_S  24
/* EMAC_MAC_ADDRESS1_HI : R/W ;bitpos:[15:0] ;default: 16'hFFFF ; */
/*description: This field contains the upper 16 bits  Bits[47:32] of the second
 6-byte MAC Address.*/
#define EMAC_MAC_ADDRESS1_HI  0x0000FFFF
#define EMAC_MAC_ADDRESS1_HI_M  ((EMAC_MAC_ADDRESS1_HI_V)<<(EMAC_MAC_ADDRESS1_HI_S))
#define EMAC_MAC_ADDRESS1_HI_V  0xFFFF
#define EMAC_MAC_ADDRESS1_HI_S  0

#define EMAC_ADDR1LOW_REG          (DR_REG_EMAC_BASE + 0x104C)
/* EMAC_MAC_ADDRESS1_LOW : R/W ;bitpos:[31:0] ;default: 32'hFFFFFFFF ; */
/*description: This field contains the lower 32 bits of the second 6-byte MAC
 address.The content of this field is undefined  so the register needs to be configured after the initialization Process.*/
#define EMAC_MAC_ADDRESS1_LOW  0xFFFFFFFF
#define EMAC_MAC_ADDRESS1_LOW_M  ((EMAC_MAC_ADDRESS1_LOW_V)<<(EMAC_MAC_ADDRESS1_LOW_S))
#define EMAC_MAC_ADDRESS1_LOW_V  0xFFFFFFFF
#define EMAC_MAC_ADDRESS1_LOW_S  0

#define EMAC_ADDR2HIGH_REG          (DR_REG_EMAC_BASE + 0x1050)
/* EMAC_ADDRESS_ENABLE2 : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: When this bit is set  the address filter module uses the third
 MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
#define EMAC_ADDRESS_ENABLE2  (BIT(31))
#define EMAC_ADDRESS_ENABLE2_M  (BIT(31))
#define EMAC_ADDRESS_ENABLE2_V  0x1
#define EMAC_ADDRESS_ENABLE2_S  31
/* EMAC_SOURCE_ADDRESS2 : R/W ;bitpos:[30] ;default: 1'h0 ; */
/*description: When this bit is set  the EMACADDR2[47:0] is used to compare
 with the SA fields of the received frame. When this bit is reset  the EMACADDR2[47:0] is used to compare with the DA fields of the received frame.*/
#define EMAC_SOURCE_ADDRESS2  (BIT(30))
#define EMAC_SOURCE_ADDRESS2_M  (BIT(30))
#define EMAC_SOURCE_ADDRESS2_V  0x1
#define EMAC_SOURCE_ADDRESS2_S  30
/* EMAC_MASK_BYTE_CONTROL2 : R/W ;bitpos:[29:24] ;default: 6'h0 ; */
/*description: These bits are mask control bits for comparison of each of the
 EMACADDR2 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR2 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR2 High [15:8].  Bit[28]: EMACADDR2 High [7:0].  Bit[27]: EMACADDR2 Low [31:24].  Bit[24]: EMACADDR2 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
#define EMAC_MASK_BYTE_CONTROL2  0x0000003F
#define EMAC_MASK_BYTE_CONTROL2_M  ((EMAC_MASK_BYTE_CONTROL2_V)<<(EMAC_MASK_BYTE_CONTROL2_S))
#define EMAC_MASK_BYTE_CONTROL2_V  0x3F
#define EMAC_MASK_BYTE_CONTROL2_S  24
/* EMAC_MAC_ADDRESS2_HI : R/W ;bitpos:[15:0] ;default: 16'hFFFF ; */
/*description: This field contains the upper 16 bits  Bits[47:32] of the third
 6-byte MAC address.*/
#define EMAC_MAC_ADDRESS2_HI  0x0000FFFF
#define EMAC_MAC_ADDRESS2_HI_M  ((EMAC_MAC_ADDRESS2_HI_V)<<(EMAC_MAC_ADDRESS2_HI_S))
#define EMAC_MAC_ADDRESS2_HI_V  0xFFFF
#define EMAC_MAC_ADDRESS2_HI_S  0

#define EMAC_ADDR2LOW_REG          (DR_REG_EMAC_BASE + 0x1054)
/* EMAC_MAC_ADDRESS2_LOW : R/W ;bitpos:[31:0] ;default: 32'hFFFFFFFF ; */
/*description: This field contains the lower 32 bits of the third 6-byte MAC
 address. The content of this field is undefined  so the register needs to be configured after the initialization process.*/
#define EMAC_MAC_ADDRESS2_LOW  0xFFFFFFFF
#define EMAC_MAC_ADDRESS2_LOW_M  ((EMAC_MAC_ADDRESS2_LOW_V)<<(EMAC_MAC_ADDRESS2_LOW_S))
#define EMAC_MAC_ADDRESS2_LOW_V  0xFFFFFFFF
#define EMAC_MAC_ADDRESS2_LOW_S  0

#define EMAC_ADDR3HIGH_REG          (DR_REG_EMAC_BASE + 0x1058)
/* EMAC_ADDRESS_ENABLE3 : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: When this bit is set  the address filter module uses the fourth
 MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
#define EMAC_ADDRESS_ENABLE3  (BIT(31))
#define EMAC_ADDRESS_ENABLE3_M  (BIT(31))
#define EMAC_ADDRESS_ENABLE3_V  0x1
#define EMAC_ADDRESS_ENABLE3_S  31
/* EMAC_SOURCE_ADDRESS3 : R/W ;bitpos:[30] ;default: 1'h0 ; */
/*description: When this bit is set  the EMACADDR3[47:0] is used to compare
 with the SA fields of the received frame. When this bit is reset  the EMACADDR3[47:0] is used to compare with the DA fields of the received frame.*/
#define EMAC_SOURCE_ADDRESS3  (BIT(30))
#define EMAC_SOURCE_ADDRESS3_M  (BIT(30))
#define EMAC_SOURCE_ADDRESS3_V  0x1
#define EMAC_SOURCE_ADDRESS3_S  30
/* EMAC_MASK_BYTE_CONTROL3 : R/W ;bitpos:[29:24] ;default: 6'h0 ; */
/*description: These bits are mask control bits for comparison of each of the
 EMACADDR3 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR3 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR3 High [15:8].  Bit[28]: EMACADDR3 High [7:0].  Bit[27]: EMACADDR3 Low [31:24].  Bit[24]: EMACADDR3 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
#define EMAC_MASK_BYTE_CONTROL3  0x0000003F
#define EMAC_MASK_BYTE_CONTROL3_M  ((EMAC_MASK_BYTE_CONTROL3_V)<<(EMAC_MASK_BYTE_CONTROL3_S))
#define EMAC_MASK_BYTE_CONTROL3_V  0x3F
#define EMAC_MASK_BYTE_CONTROL3_S  24
/* EMAC_MAC_ADDRESS3_HI : R/W ;bitpos:[15:0] ;default: 16'hFFFF ; */
/*description: This field contains the upper 16 bits  Bits[47:32] of the fourth
 6-byte MAC address.*/
#define EMAC_MAC_ADDRESS3_HI  0x0000FFFF
#define EMAC_MAC_ADDRESS3_HI_M  ((EMAC_MAC_ADDRESS3_HI_V)<<(EMAC_MAC_ADDRESS3_HI_S))
#define EMAC_MAC_ADDRESS3_HI_V  0xFFFF
#define EMAC_MAC_ADDRESS3_HI_S  0

#define EMAC_ADDR3LOW_REG          (DR_REG_EMAC_BASE + 0x105C)
/* EMAC_MAC_ADDRESS3_LOW : R/W ;bitpos:[31:0] ;default: 32'hFFFFFFFF ; */
/*description: This field contains the lower 32 bits of the fourth 6-byte MAC
 address.The content of this field is undefined  so the register needs to be configured after the initialization Process.*/
#define EMAC_MAC_ADDRESS3_LOW  0xFFFFFFFF
#define EMAC_MAC_ADDRESS3_LOW_M  ((EMAC_MAC_ADDRESS3_LOW_V)<<(EMAC_MAC_ADDRESS3_LOW_S))
#define EMAC_MAC_ADDRESS3_LOW_V  0xFFFFFFFF
#define EMAC_MAC_ADDRESS3_LOW_S  0

#define EMAC_ADDR4HIGH_REG          (DR_REG_EMAC_BASE + 0x1060)
/* EMAC_ADDRESS_ENABLE4 : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: When this bit is set  the address filter module uses the fifth
 MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
#define EMAC_ADDRESS_ENABLE4  (BIT(31))
#define EMAC_ADDRESS_ENABLE4_M  (BIT(31))
#define EMAC_ADDRESS_ENABLE4_V  0x1
#define EMAC_ADDRESS_ENABLE4_S  31
/* EMAC_SOURCE_ADDRESS4 : R/W ;bitpos:[30] ;default: 1'h0 ; */
/*description: When this bit is set  the EMACADDR4[47:0] is used to compare
 with the SA fields of the received frame. When this bit is reset  the EMACADDR4[47:0] is used to compare with the DA fields of the received frame.*/
#define EMAC_SOURCE_ADDRESS4  (BIT(30))
#define EMAC_SOURCE_ADDRESS4_M  (BIT(30))
#define EMAC_SOURCE_ADDRESS4_V  0x1
#define EMAC_SOURCE_ADDRESS4_S  30
/* EMAC_MASK_BYTE_CONTROL4 : R/W ;bitpos:[29:24] ;default: 6'h0 ; */
/*description: These bits are mask control bits for comparison of each of the
 EMACADDR4 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR4 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR4 High [15:8].  Bit[28]: EMACADDR4 High [7:0].  Bit[27]: EMACADDR4 Low [31:24].  Bit[24]: EMACADDR4 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
#define EMAC_MASK_BYTE_CONTROL4  0x0000003F
#define EMAC_MASK_BYTE_CONTROL4_M  ((EMAC_MASK_BYTE_CONTROL4_V)<<(EMAC_MASK_BYTE_CONTROL4_S))
#define EMAC_MASK_BYTE_CONTROL4_V  0x3F
#define EMAC_MASK_BYTE_CONTROL4_S  24
/* EMAC_MAC_ADDRESS4_HI : R/W ;bitpos:[15:0] ;default: 16'hFFFF ; */
/*description: This field contains the upper 16 bits  Bits[47:32] of the fifth
 6-byte MAC address.*/
#define EMAC_MAC_ADDRESS4_HI  0x0000FFFF
#define EMAC_MAC_ADDRESS4_HI_M  ((EMAC_MAC_ADDRESS4_HI_V)<<(EMAC_MAC_ADDRESS4_HI_S))
#define EMAC_MAC_ADDRESS4_HI_V  0xFFFF
#define EMAC_MAC_ADDRESS4_HI_S  0

#define EMAC_ADDR4LOW_REG          (DR_REG_EMAC_BASE + 0x1064)
/* EMAC_MAC_ADDRESS4_LOW : R/W ;bitpos:[31:0] ;default: 32'hFFFFFFFF ; */
/*description: This field contains the lower 32 bits of the fifth 6-byte MAC
 address. The content of this field is undefined  so the register needs to be configured after the initialization process.*/
#define EMAC_MAC_ADDRESS4_LOW  0xFFFFFFFF
#define EMAC_MAC_ADDRESS4_LOW_M  ((EMAC_MAC_ADDRESS4_LOW_V)<<(EMAC_MAC_ADDRESS4_LOW_S))
#define EMAC_MAC_ADDRESS4_LOW_V  0xFFFFFFFF
#define EMAC_MAC_ADDRESS4_LOW_S  0

#define EMAC_ADDR5HIGH_REG          (DR_REG_EMAC_BASE + 0x1068)
/* EMAC_ADDRESS_ENABLE5 : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: When this bit is set  the address filter module uses the sixth
 MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
#define EMAC_ADDRESS_ENABLE5  (BIT(31))
#define EMAC_ADDRESS_ENABLE5_M  (BIT(31))
#define EMAC_ADDRESS_ENABLE5_V  0x1
#define EMAC_ADDRESS_ENABLE5_S  31
/* EMAC_SOURCE_ADDRESS5 : R/W ;bitpos:[30] ;default: 1'h0 ; */
/*description: When this bit is set  the EMACADDR5[47:0] is used to compare
 with the SA fields of the received frame. When this bit is reset  the EMACADDR5[47:0] is used to compare with the DA fields of the received frame.*/
#define EMAC_SOURCE_ADDRESS5  (BIT(30))
#define EMAC_SOURCE_ADDRESS5_M  (BIT(30))
#define EMAC_SOURCE_ADDRESS5_V  0x1
#define EMAC_SOURCE_ADDRESS5_S  30
/* EMAC_MASK_BYTE_CONTROL5 : R/W ;bitpos:[29:24] ;default: 6'h0 ; */
/*description: These bits are mask control bits for comparison of each of the
 EMACADDR5 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR5 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR5 High [15:8].  Bit[28]: EMACADDR5 High [7:0].  Bit[27]: EMACADDR5 Low [31:24].  Bit[24]: EMACADDR5 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
#define EMAC_MASK_BYTE_CONTROL5  0x0000003F
#define EMAC_MASK_BYTE_CONTROL5_M  ((EMAC_MASK_BYTE_CONTROL5_V)<<(EMAC_MASK_BYTE_CONTROL5_S))
#define EMAC_MASK_BYTE_CONTROL5_V  0x3F
#define EMAC_MASK_BYTE_CONTROL5_S  24
/* EMAC_MAC_ADDRESS5_HI : R/W ;bitpos:[15:0] ;default: 16'hFFFF ; */
/*description: This field contains the upper 16 bits  Bits[47:32] of the sixth
 6-byte MAC address.*/
#define EMAC_MAC_ADDRESS5_HI  0x0000FFFF
#define EMAC_MAC_ADDRESS5_HI_M  ((EMAC_MAC_ADDRESS5_HI_V)<<(EMAC_MAC_ADDRESS5_HI_S))
#define EMAC_MAC_ADDRESS5_HI_V  0xFFFF
#define EMAC_MAC_ADDRESS5_HI_S  0

#define EMAC_ADDR5LOW_REG          (DR_REG_EMAC_BASE + 0x106C)
/* EMAC_MAC_ADDRESS5_LOW : R/W ;bitpos:[31:0] ;default: 32'hFFFFFFFF ; */
/*description: This field contains the lower 32 bits of the sixth 6-byte MAC
 address. The content of this field is undefined  so the register needs to be configured after the initialization process.*/
#define EMAC_MAC_ADDRESS5_LOW  0xFFFFFFFF
#define EMAC_MAC_ADDRESS5_LOW_M  ((EMAC_MAC_ADDRESS5_LOW_V)<<(EMAC_MAC_ADDRESS5_LOW_S))
#define EMAC_MAC_ADDRESS5_LOW_V  0xFFFFFFFF
#define EMAC_MAC_ADDRESS5_LOW_S  0

#define EMAC_ADDR6HIGH_REG          (DR_REG_EMAC_BASE + 0x1070)
/* EMAC_ADDRESS_ENABLE6 : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: When this bit is set  the address filter module uses the seventh
 MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
#define EMAC_ADDRESS_ENABLE6  (BIT(31))
#define EMAC_ADDRESS_ENABLE6_M  (BIT(31))
#define EMAC_ADDRESS_ENABLE6_V  0x1
#define EMAC_ADDRESS_ENABLE6_S  31
/* EMAC_SOURCE_ADDRESS6 : R/W ;bitpos:[30] ;default: 1'h0 ; */
/*description: When this bit is set  the EMACADDR6[47:0] is used to compare
 with the SA fields of the received frame. When this bit is reset  the EMACADDR6[47:0] is used to compare with the DA fields of the received frame.*/
#define EMAC_SOURCE_ADDRESS6  (BIT(30))
#define EMAC_SOURCE_ADDRESS6_M  (BIT(30))
#define EMAC_SOURCE_ADDRESS6_V  0x1
#define EMAC_SOURCE_ADDRESS6_S  30
/* EMAC_MASK_BYTE_CONTROL6 : R/W ;bitpos:[29:24] ;default: 6'h0 ; */
/*description: These bits are mask control bits for comparison of each of the
 EMACADDR6 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR6 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR6 High [15:8].  Bit[28]: EMACADDR6 High [7:0].  Bit[27]: EMACADDR6 Low [31:24].  Bit[24]: EMACADDR6 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
#define EMAC_MASK_BYTE_CONTROL6  0x0000003F
#define EMAC_MASK_BYTE_CONTROL6_M  ((EMAC_MASK_BYTE_CONTROL6_V)<<(EMAC_MASK_BYTE_CONTROL6_S))
#define EMAC_MASK_BYTE_CONTROL6_V  0x3F
#define EMAC_MASK_BYTE_CONTROL6_S  24
/* EMAC_MAC_ADDRESS6_HI : R/W ;bitpos:[15:0] ;default: 16'hFFFF ; */
/*description: This field contains the upper 16 bits  Bits[47:32] of the seventh
 6-byte MAC Address.*/
#define EMAC_MAC_ADDRESS6_HI  0x0000FFFF
#define EMAC_MAC_ADDRESS6_HI_M  ((EMAC_MAC_ADDRESS6_HI_V)<<(EMAC_MAC_ADDRESS6_HI_S))
#define EMAC_MAC_ADDRESS6_HI_V  0xFFFF
#define EMAC_MAC_ADDRESS6_HI_S  0

#define EMAC_ADDR6LOW_REG          (DR_REG_EMAC_BASE + 0x1074)
/* EMAC_MAC_ADDRESS6_LOW : R/W ;bitpos:[31:0] ;default: 32'hFFFFFFFF ; */
/*description: This field contains the lower 32 bits of the seventh 6-byte MAC
 address.The content of this field is undefined  so the register needs to be configured after the initialization Process.*/
#define EMAC_MAC_ADDRESS6_LOW  0xFFFFFFFF
#define EMAC_MAC_ADDRESS6_LOW_M  ((EMAC_MAC_ADDRESS6_LOW_V)<<(EMAC_MAC_ADDRESS6_LOW_S))
#define EMAC_MAC_ADDRESS6_LOW_V  0xFFFFFFFF
#define EMAC_MAC_ADDRESS6_LOW_S  0

#define EMAC_ADDR7HIGH_REG          (DR_REG_EMAC_BASE + 0x1078)
/* EMAC_ADDRESS_ENABLE7 : R/W ;bitpos:[31] ;default: 1'h0 ; */
/*description: When this bit is set  the address filter module uses the eighth
 MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
#define EMAC_ADDRESS_ENABLE7  (BIT(31))
#define EMAC_ADDRESS_ENABLE7_M  (BIT(31))
#define EMAC_ADDRESS_ENABLE7_V  0x1
#define EMAC_ADDRESS_ENABLE7_S  31
/* EMAC_SOURCE_ADDRESS7 : R/W ;bitpos:[30] ;default: 1'h0 ; */
/*description: When this bit is set  the EMACADDR7[47:0] is used to compare
 with the SA fields of the received frame. When this bit is reset  the EMACADDR7[47:0] is used to compare with the DA fields of the received frame.*/
#define EMAC_SOURCE_ADDRESS7  (BIT(30))
#define EMAC_SOURCE_ADDRESS7_M  (BIT(30))
#define EMAC_SOURCE_ADDRESS7_V  0x1
#define EMAC_SOURCE_ADDRESS7_S  30
/* EMAC_MASK_BYTE_CONTROL7 : R/W ;bitpos:[29:24] ;default: 6'h0 ; */
/*description: These bits are mask control bits for comparison of each of the
 EMACADDR7 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR7 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR7 High [15:8].  Bit[28]: EMACADDR7 High [7:0].  Bit[27]: EMACADDR7 Low [31:24].  Bit[24]: EMACADDR7 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
#define EMAC_MASK_BYTE_CONTROL7  0x0000003F
#define EMAC_MASK_BYTE_CONTROL7_M  ((EMAC_MASK_BYTE_CONTROL7_V)<<(EMAC_MASK_BYTE_CONTROL7_S))
#define EMAC_MASK_BYTE_CONTROL7_V  0x3F
#define EMAC_MASK_BYTE_CONTROL7_S  24
/* EMAC_MAC_ADDRESS7_HI : R/W ;bitpos:[15:0] ;default: 16'hFFFF ; */
/*description: This field contains the upper 16 bits  Bits[47:32] of the eighth
 6-byte MAC Address.*/
#define EMAC_MAC_ADDRESS7_HI  0x0000FFFF
#define EMAC_MAC_ADDRESS7_HI_M  ((EMAC_MAC_ADDRESS7_HI_V)<<(EMAC_MAC_ADDRESS7_HI_S))
#define EMAC_MAC_ADDRESS7_HI_V  0xFFFF
#define EMAC_MAC_ADDRESS7_HI_S  0

#define EMAC_ADDR7LOW_REG          (DR_REG_EMAC_BASE + 0x107C)
/* EMAC_MAC_ADDRESS7_LOW : R/W ;bitpos:[31:0] ;default: 32'hFFFFFFFF ; */
/*description: This field contains the lower 32 bits of the eighth 6-byte MAC
 address.The content of this field is undefined  so the register needs to be configured after the initialization Process.*/
#define EMAC_MAC_ADDRESS7_LOW  0xFFFFFFFF
#define EMAC_MAC_ADDRESS7_LOW_M  ((EMAC_MAC_ADDRESS7_LOW_V)<<(EMAC_MAC_ADDRESS7_LOW_S))
#define EMAC_MAC_ADDRESS7_LOW_V  0xFFFFFFFF
#define EMAC_MAC_ADDRESS7_LOW_S  0

#define EMAC_CSTATUS_REG          (DR_REG_EMAC_BASE + 0x10D8)
/* EMAC_JABBER_TIMEOUT : RO ;bitpos:[4] ;default: 1'h0 ; */
/*description: This bit indicates whether there is jabber timeout error (1'b1)
 in the received Frame.*/
#define EMAC_JABBER_TIMEOUT  (BIT(4))
#define EMAC_JABBER_TIMEOUT_M  (BIT(4))
#define EMAC_JABBER_TIMEOUT_V  0x1
#define EMAC_JABBER_TIMEOUT_S  4
/* EMAC_LINK_SPEED : RO ;bitpos:[2:1] ;default: 1'h0 ; */
/*description: This bit indicates the current speed of the link:   2'b00: 2.5
 MHz.  2'b01: 25 MHz.  2'b10: 125 MHz.*/
#define EMAC_LINK_SPEED  0x00000003
#define EMAC_LINK_SPEED_M  ((EMAC_LINK_SPEED_V)<<(EMAC_LINK_SPEED_S))
#define EMAC_LINK_SPEED_V  0x3
#define EMAC_LINK_SPEED_S  1
/* EMAC_LINK_MODE : RO ;bitpos:[0] ;default: 1'h0 ; */
/*description: This bit indicates the current mode of operation of the link:
   1'b0: Half-duplex mode.  1'b1: Full-duplex mode.*/
#define EMAC_LINK_MODE  (BIT(0))
#define EMAC_LINK_MODE_M  (BIT(0))
#define EMAC_LINK_MODE_V  0x1
#define EMAC_LINK_MODE_S  0

#define EMAC_WDOGTO_REG          (DR_REG_EMAC_BASE + 0x10DC)
/* EMAC_PWDOGEN : R/W ;bitpos:[16] ;default: 1'h0 ; */
/*description: When this bit is set and Bit[23] (WD) of EMACCONFIG_REG is reset
  the WTO field (Bits[13:0]) is used as watchdog timeout for a received frame. When this bit is cleared  the watchdog timeout for a received frame is controlled by the setting of Bit[23] (WD) and Bit[20] (JE) in EMACCONFIG_REG.*/
#define EMAC_PWDOGEN  (BIT(16))
#define EMAC_PWDOGEN_M  (BIT(16))
#define EMAC_PWDOGEN_V  0x1
#define EMAC_PWDOGEN_S  16
/* EMAC_WDOGTO : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: When Bit[16] (PWE) is set and Bit[23] (WD) of EMACCONFIG_REG
 is reset  this field is used as watchdog timeout for a received frame. If the length of a received frame exceeds the value of this field  such frame is terminated and declared as an error frame.*/
#define EMAC_WDOGTO  0x00003FFF
#define EMAC_WDOGTO_M  ((EMAC_WDOGTO_V)<<(EMAC_WDOGTO_S))
#define EMAC_WDOGTO_V  0x3FFF
#define EMAC_WDOGTO_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_EMAC_REG_H_ */


