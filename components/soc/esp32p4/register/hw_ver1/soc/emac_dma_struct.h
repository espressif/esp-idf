/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef struct emac_dma_dev_s {
    volatile union {
        struct {
            uint32_t sw_rst : 1;         /*When this bit is set  the MAC DMA Controller resets the logic and all internal registers of the MAC. It is cleared automatically after the reset operation is complete in all of the ETH_MAC clock domains. Before reprogramming any register of the ETH_MAC  you should read a zero (0) value in this bit.*/
            uint32_t dma_arb_sch : 1;    /*This bit specifies the arbitration scheme between the transmit and receive paths.1'b0: weighted round-robin with RX:TX or TX:RX  priority specified in PR (bit[15:14]). 1'b1 Fixed priority (Rx priority to Tx).*/
            uint32_t desc_skip_len : 5;  /*This bit specifies the number of Word to skip between two unchained descriptors.The address skipping starts from the end of current descriptor to the start of next descriptor. When the DSL(DESC_SKIP_LEN) value is equal to zero  the descriptor table is taken as contiguous by the DMA in Ring mode.*/
            uint32_t alt_desc_size : 1;  /*When set  the size of the alternate descriptor increases to 32 bytes.*/
            uint32_t prog_burst_len : 6; /*These bits indicate the maximum number of beats to be transferred in one DMA transaction. If the number of beats to be transferred is more than 32  then perform the following steps: 1. Set the PBLx8 mode  2. Set the PBL(PROG_BURST_LEN).*/
            uint32_t pri_ratio : 2;      /*These bits control the priority ratio in the weighted round-robin arbitration between the Rx DMA and Tx DMA. These bits are valid only when Bit 1 (DA) is reset. The priority ratio Rx:Tx represented by each bit:  2'b00 -- 1: 1    2'b01 -- 2: 0    2'b10 -- 3: 1    2'b11 -- 4: 1*/
            uint32_t fixed_burst : 1;    /*This bit controls whether the AHB master interface performs fixed burst transfers or not. When set  the AHB interface uses only SINGLE  INCR4  INCR8  or INCR16 during start of the normal burst transfers. When reset  the AHB interface uses SINGLE and INCR burst transfer Operations.*/
            uint32_t rx_dma_pbl : 6;     /*This field indicates the maximum number of beats to be transferred in one Rx DMA transaction. This is the maximum value that is used in a single block Read or Write.The Rx DMA always attempts to burst as specified in the RPBL(RX_DMA_PBL) bit each time it starts a burst transfer on the host bus. You can program RPBL with values of 1  2  4  8  16  and 32. Any other value results in undefined behavior. This field is valid and applicable only when USP(USE_SEP_PBL) is set high.*/
            uint32_t use_sep_pbl : 1;    /*When set high  this bit configures the Rx DMA to use the value configured in Bits[22:17] as PBL. The PBL value in Bits[13:8] is applicable only to the Tx DMA operations. When reset to low  the PBL value in Bits[13:8] is applicable for both DMA engines.*/
            uint32_t pblx8_mode : 1;     /*When set high  this bit multiplies the programmed PBL value (Bits[22:17] and Bits[13:8]) eight times. Therefore  the DMA transfers the data in 8  16  32  64  128  and 256 beats depending on the PBL value.*/
            uint32_t dmaaddralibea : 1;  /*When this bit is set high and the FIXED_BURST bit is 1  the AHB interface generates all bursts aligned to the start address LS bits. If the FIXED_BURST bit is 0  the first burst (accessing the start address of data buffer) is not aligned  but subsequent bursts are aligned to the address.*/
            uint32_t dmamixedburst : 1;  /*When this bit is set high and the FIXED_BURST bit is low  the AHB master interface starts all bursts of a length more than 16 with INCR (undefined burst)  whereas it reverts to fixed burst transfers (INCRx and SINGLE) for burst length of 16 and less.*/
            uint32_t reserved27 : 1;
            uint32_t reserved28 : 2;
            uint32_t reserved30 : 1;
            uint32_t reserved31 : 1;
        };
        uint32_t val;
    } dmabusmode;
    uint32_t dmatxpolldemand; /*When these bits are written with any value  the DMA reads the current descriptor to which the Register (Current Host Transmit Descriptor Register) is pointing. If that descriptor is not available (owned by the Host)  the transmission returns to the suspend state and Bit[2] (TU) of Status Register is asserted. If the descriptor is available  the transmission resumes.*/
    uint32_t dmarxpolldemand; /*When these bits are written with any value  the DMA reads the current descriptor to which the Current Host Receive Descriptor Register is pointing. If that descriptor is not available (owned by the Host)  the reception returns to the Suspended state and Bit[7] (RU) of Status Register is asserted. If the descriptor is available  the Rx DMA returns to the active state.*/
    uint32_t dmarxbaseaddr;   /*This field contains the base address of the first descriptor in the Receive Descriptor list. The LSB Bits[1:0] are ignored and internally taken as all-zero by the DMA. Therefore these LSB bits are read-only.*/
    uint32_t dmatxbaseaddr;   /*This field contains the base address of the first descriptor in the Transmit Descriptor list. The LSB Bits[1:0] are ignored and are internally taken as all-zero by the DMA.Therefore  these LSB bits are read-only.*/
    volatile union {
        struct {
            uint32_t trans_int : 1;         /*This bit indicates that the frame transmission is complete. When transmission is complete  Bit[31] (OWN) of TDES0 is reset  and the specific frame status information is updated in the Descriptor.*/
            uint32_t trans_proc_stop : 1;   /*This bit is set when the transmission is stopped.*/
            uint32_t trans_buf_unavail : 1; /*This bit indicates that the host owns the Next Descriptor in the Transmit List and the DMA cannot acquire it. Transmission is suspended. Bits[22:20] explain the Transmit Process state transitions. To resume processing Transmit descriptors  the host should change the ownership of the descriptor by setting TDES0[31] and then issue a Transmit Poll Demand Command.*/
            uint32_t trans_jabber_to : 1;   /*This bit indicates that the Transmit Jabber Timer expired  which happens when the frame size exceeds 2 048 (10 240 bytes when the Jumbo frame is enabled). When the Jabber Timeout occurs  the transmission process is aborted and placed in the Stopped state. This causes the Transmit Jabber Timeout TDES0[14] flag to assert.*/
            uint32_t recv_ovflow : 1;       /*This bit indicates that the Receive Buffer had an Overflow during frame reception. If the partial frame is transferred to the application  the overflow status is set in RDES0[11].*/
            uint32_t trans_undflow : 1;     /*This bit indicates that the Transmit Buffer had an Underflow during frame transmission. Transmission is suspended and an Underflow Error TDES0[1] is set.*/
            uint32_t recv_int : 1;          /*This bit indicates that the frame reception is complete. When reception is complete  the Bit[31] of RDES1 (Disable Interrupt on Completion) is reset in the last Descriptor  and the specific frame status information is updated in the descriptor. The reception remains in the Running state.*/
            uint32_t recv_buf_unavail : 1;  /*This bit indicates that the host owns the Next Descriptor in the Receive List and the DMA cannot acquire it. The Receive Process is suspended. To resume processing Receive descriptors  the host should change the ownership of the descriptor and issue a Receive Poll Demand command. If no Receive Poll Demand is issued  the Receive Process resumes when the next recognized incoming frame is received. This bit is set only when the previous Receive Descriptor is owned by the DMA.*/
            uint32_t recv_proc_stop : 1;    /*This bit is asserted when the Receive Process enters the Stopped state.*/
            uint32_t recv_wdt_to : 1;       /*When set  this bit indicates that the Receive Watchdog Timer expired while receiving the current frame and the current frame is truncated after the watchdog timeout.*/
            uint32_t early_trans_int : 1;   /*This bit indicates that the frame to be transmitted is fully transferred to the MTL Transmit FIFO.*/
            uint32_t reserved11 : 2;
            uint32_t fatal_bus_err_int : 1; /*This bit indicates that a bus error occurred  as described in Bits [25:23]. When this bit is set  the corresponding DMA engine disables all of its bus accesses.*/
            uint32_t early_recv_int : 1;    /*This bit indicates that the DMA filled the first data buffer of the packet. This bit is cleared when the software writes 1 to this bit or when Bit[6] (RI) of this register is set (whichever occurs earlier).*/
            uint32_t abn_int_summ : 1;      /*Abnormal Interrupt Summary bit value is the logical OR of the following when the corresponding interrupt bits are enabled in Interrupt Enable Register:   Bit[1]: Transmit Process Stopped.  Bit[3]: Transmit Jabber Timeout.  Bit[4]: Receive FIFO Overflow.  Bit[5]: Transmit Underflow.  Bit[7]: Receive Buffer Unavailable. Bit[8]: Receive Process Stopped.  Bit[9]: Receive Watchdog Timeout.  Bit[10]: Early Transmit Interrupt.  Bit[13]: Fatal Bus Error. Only unmasked bits affect the Abnormal Interrupt Summary bit. This is a sticky bit and must be cleared (by writing 1 to this bit) each time a corresponding bit which causes AIS to be set  is cleared.*/
            uint32_t norm_int_summ : 1;     /*Normal Interrupt Summary bit value is the logical OR of the following bits when the corresponding interrupt bits are enabled in Interrupt Enable Register:  Bit[0]: Transmit Interrupt.  Bit[2]: Transmit Buffer Unavailable.  Bit[6]: Receive Interrupt.  Bit[14]: Early Receive Interrupt. Only unmasked bits affect the Normal Interrupt Summary bit.This is a sticky bit and must be cleared (by writing 1 to this bit) each time a corresponding bit which causes NIS to be set  is cleared.*/
            uint32_t recv_proc_state : 3;   /*This field indicates the Receive DMA FSM state. This field does not generate an interrupt.  3'b000: Stopped. Reset or Stop Receive Command issued.  3'b001: Running. Fetching Receive Transfer Descriptor.  3'b010: Reserved for future use.  3'b011: Running. Waiting for RX packets.  3'b100: Suspended. Receive Descriptor Unavailable.  3'b101: Running. Closing Receive Descriptor.  3'b110: TIME_STAMP write state.  3'b111: Running. Transferring the TX packets data from receive buffer to host memory.*/
            uint32_t trans_proc_state : 3;  /*This field indicates the Transmit DMA FSM state. This field does not generate an interrupt.  3'b000: Stopped. Reset or Stop Transmit Command issued.  3'b001: Running. Fetching Transmit Transfer Descriptor.  3'b010: Reserved for future use.  3'b011: Running. Waiting for TX packets.  3'b100: Suspended. Receive Descriptor Unavailable.  3'b101: Running. Closing Transmit Descriptor.  3'b110: TIME_STAMP write state.  3'b111: Running. Transferring the TX packets data from transmit buffer to host memory.*/
            uint32_t error_bits : 3;        /*This field indicates the type of error that caused a Bus Error  for example  error response on the AHB interface. This field is valid only when Bit[13] (FBI) is set. This field does not generate an interrupt.  3'b000: Error during Rx DMA Write Data Transfer.  3'b011: Error during Tx DMA Read Data Transfer.  3'b100: Error during Rx DMA Descriptor Write Access.  3'b101: Error during Tx DMA Descriptor Write Access.  3'b110: Error during Rx DMA Descriptor Read Access.  3'b111: Error during Tx DMA Descriptor Read Access.*/
            uint32_t reserved26 : 1;
            uint32_t reserved27 : 1;
            uint32_t pmt_int : 1;    /*This bit indicates an interrupt event in the PMT module of the ETH_MAC. The software must read the PMT Control and Status Register in the MAC to get the exact cause of interrupt and clear its source to reset this bit to 1'b0.*/
            uint32_t ts_tri_int : 1; /*This bit indicates an interrupt event in the Timestamp Generator block of the ETH_MAC.The software must read the corresponding registers in the ETH_MAC to get the exact cause of the interrupt and clear its source to reset this bit to 1'b0.*/
            uint32_t reserved30 : 1;
            uint32_t reserved31 : 1;
        };
        uint32_t val;
    } dmastatus;
    volatile union {
        struct {
            uint32_t reserved0 : 1;
            uint32_t start_stop_rx : 1;    /*When this bit is set  the Receive process is placed in the Running state. The DMA attempts to acquire the descriptor from the Receive list and processes the incoming frames.When this bit is cleared  the Rx DMA operation is stopped after the transfer of the current frame.*/
            uint32_t opt_second_frame : 1; /*When this bit is set  it instructs the DMA to process the second frame of the Transmit data even before the status for the first frame is obtained.*/
            uint32_t rx_thresh_ctrl : 2;   /*These two bits control the threshold level of the MTL Receive FIFO. Transfer (request) to DMA starts when the frame size within the MTL Receive FIFO is larger than the threshold. 2'b00: 64， 2'b01: 32， 2'b10: 96， 2'b11: 128 .*/
            uint32_t drop_gfrm : 1;        /*When set  the MAC drops the received giant frames in the Rx FIFO  that is frames that are larger than the computed giant frame limit.*/
            uint32_t fwd_under_gf : 1;     /*When set  the Rx FIFO forwards Undersized frames (that is  frames with no Error and length less than 64 bytes) including pad-bytes and CRC.*/
            uint32_t fwd_err_frame : 1;    /*When this bit is reset  the Rx FIFO drops frames with error status (CRC error collision error   giant frame  watchdog timeout  or overflow).*/
            uint32_t reserved8 : 1;
            uint32_t reserved9 : 2;
            uint32_t reserved11 : 2;
            uint32_t start_stop_transmission_command : 1; /*When this bit is set  transmission is placed in the Running state  and the DMA checks the Transmit List at the current position for a frame to be transmitted.When this bit is reset  the transmission process is placed in the Stopped state after completing the transmission of the current frame.*/
            uint32_t tx_thresh_ctrl : 3;                  /*These bits control the threshold level of the MTL Transmit FIFO. Transmission starts when the frame size within the MTL Transmit FIFO is larger than the threshold. In addition  full frames with a length less than the threshold are also transmitted. These bits are used only when Tx_Str_fwd is reset. 3'b000: 64  3'b001: 128   3'b010: 192   3'b011: 256  3'b100: 40  3'b101: 32  3'b110: 24  3'b111: 16 .*/
            uint32_t reserved17 : 3;
            uint32_t flush_tx_fifo : 1; /*When this bit is set  the transmit FIFO controller logic is reset to its default values  and thus all data in the Tx FIFO is lost or flushed. This bit is cleared internally when the flushing operation is complete.*/
            uint32_t tx_str_fwd : 1;    /*When this bit is set  transmission starts when a full frame resides in the MTL Transmit FIFO. When this bit is set  the Tx_Thresh_Ctrl values specified in Tx_Thresh_Ctrl are ignored.*/
            uint32_t reserved22 : 1;
            uint32_t reserved23 : 1;
            uint32_t dis_flush_recv_frames : 1;   /*When this bit is set  the Rx DMA does not flush any frames because of the unavailability of receive descriptors or buffers.*/
            uint32_t rx_store_forward : 1;        /*When this bit is set  the MTL reads a frame from the Rx FIFO only after the complete frame has been written to it.*/
            uint32_t dis_drop_tcpip_err_fram : 1; /*When this bit is set  the MAC does not drop the frames which only have errors detected by the Receive Checksum engine.When this bit is reset  all error frames are dropped if the Fwd_Err_Frame bit is reset.*/
            uint32_t reserved27 : 5;
        };
        uint32_t val;
    } dmaoperation_mode;
    volatile union {
        struct {
            uint32_t dmain_tie : 1;  /*When this bit is set with Normal Interrupt Summary Enable (Bit[16])  the Transmit Interrupt is enabled. When this bit is reset  the Transmit Interrupt is disabled.*/
            uint32_t dmain_tse : 1;  /*When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])  the Transmission Stopped Interrupt is enabled. When this bit is reset  the Transmission Stopped Interrupt is disabled.*/
            uint32_t dmain_tbue : 1; /*When this bit is set with Normal Interrupt Summary Enable (Bit 16)  the Transmit Buffer Unavailable Interrupt is enabled. When this bit is reset  the Transmit Buffer Unavailable Interrupt is Disabled.*/
            uint32_t dmain_tjte : 1; /*When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])  the Transmit Jabber Timeout Interrupt is enabled. When this bit is reset  the Transmit Jabber Timeout Interrupt is disabled.*/
            uint32_t dmain_oie : 1;  /*When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])  the Receive Overflow Interrupt is enabled. When this bit is reset  the Overflow Interrupt is disabled.*/
            uint32_t dmain_uie : 1;  /*When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])  the Transmit Underflow Interrupt is enabled. When this bit is reset  the Underflow Interrupt is disabled.*/
            uint32_t dmain_rie : 1;  /*When this bit is set with Normal Interrupt Summary Enable (Bit[16])  the Receive Interrupt is enabled. When this bit is reset  the Receive Interrupt is disabled.*/
            uint32_t dmain_rbue : 1; /*When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])  the Receive Buffer Unavailable Interrupt is enabled. When this bit is reset  the Receive Buffer Unavailable Interrupt is disabled.*/
            uint32_t dmain_rse : 1;  /*When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])  the Receive Stopped Interrupt is enabled. When this bit is reset  the Receive Stopped Interrupt is disabled.*/
            uint32_t dmain_rwte : 1; /*When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])  the Receive Watchdog Timeout Interrupt is enabled. When this bit is reset  the Receive Watchdog Timeout Interrupt is disabled.*/
            uint32_t dmain_etie : 1; /*When this bit is set with an Abnormal Interrupt Summary Enable (Bit[15])  the Early Transmit Interrupt is enabled. When this bit is reset  the Early Transmit Interrupt is disabled.*/
            uint32_t reserved11 : 2;
            uint32_t dmain_fbee : 1; /*When this bit is set with Abnormal Interrupt Summary Enable (Bit[15])  the Fatal Bus Error Interrupt is enabled. When this bit is reset  the Fatal Bus Error Enable Interrupt is disabled.*/
            uint32_t dmain_erie : 1; /*When this bit is set with Normal Interrupt Summary Enable (Bit[16])  the Early Receive Interrupt is enabled. When this bit is reset  the Early Receive Interrupt is disabled.*/
            uint32_t dmain_aise : 1; /*When this bit is set  abnormal interrupt summary is enabled. When this bit is reset the abnormal interrupt summary is disabled. This bit enables the following interrupts in Status Register:  Bit[1]: Transmit Process Stopped.  Bit[3]: Transmit Jabber Timeout.  Bit[4]: Receive Overflow.  Bit[5]: Transmit Underflow.  Bit[7]: Receive Buffer Unavailable.  Bit[8]: Receive Process Stopped.  Bit[9]: Receive Watchdog Timeout.  Bit[10]: Early Transmit Interrupt.  Bit[13]: Fatal Bus Error.*/
            uint32_t dmain_nise : 1; /*When this bit is set  normal interrupt summary is enabled. When this bit is reset  normal interrupt summary is disabled. This bit enables the following interrupts in Status Register:  Bit[0]: Transmit Interrupt.  Bit[2]: Transmit Buffer Unavailable.  Bit[6]: Receive Interrupt.  Bit[14]: Early Receive Interrupt.*/
            uint32_t reserved17 : 15;
        };
        uint32_t val;
    } dmain_en;
    volatile union {
        struct {
            uint32_t missed_fc : 16;    /*This field indicates the number of frames missed by the controller because of the Host Receive Buffer being unavailable. This counter is incremented each time the DMA discards an incoming frame. The counter is cleared when this register is read.*/
            uint32_t overflow_bmfc : 1; /*This bit is set every time Missed Frame Counter (Bits[15:0]) overflows that is  the DMA discards an incoming frame because of the Host Receive  Buffer being unavailable with the missed frame counter at maximum value. In such a scenario  the Missed frame counter is reset to all-zeros and this bit indicates that the rollover happened.*/
            uint32_t overflow_fc : 11;  /*This field indicates the number of frames missed by the application. This counter is incremented each time the MTL FIFO overflows. The counter is cleared when this register is read.*/
            uint32_t overflow_bfoc : 1; /*This bit is set every time the Overflow Frame Counter (Bits[27:17]) overflows  that is  the Rx FIFO overflows with the overflow frame counter at maximum value. In such a scenario  the overflow frame counter is reset to all-zeros and this bit indicates that the rollover happened.*/
            uint32_t reserved29 : 3;
        };
        uint32_t val;
    } dmamissedfr;
    volatile union {
        struct {
            uint32_t riwtc : 8; /*This bit indicates the number of system clock cycles multiplied by 256 for which the watchdog timer is set. The watchdog timer gets triggered with the programmed value after the Rx DMA completes the transfer of a frame for which the RI(RECV_INT) status bit is not set because of the setting in the corresponding descriptor RDES1[31]. When the watchdog timer runs out  the RI bit is set and the timer is stopped. The watchdog timer is reset when the RI bit is set high because of automatic setting of RI as per RDES1[31] of any received frame.*/
            uint32_t reserved8 : 24;
        };
        uint32_t val;
    } dmarintwdtimer;
    uint32_t reserved_28;
    uint32_t reserved_2c;
    uint32_t reserved_30;
    uint32_t reserved_34;
    uint32_t reserved_38;
    uint32_t reserved_3c;
    uint32_t reserved_40;
    uint32_t reserved_44;
    uint32_t dmatxcurrdesc;     /*The address of the current receive descriptor list. Cleared on Reset.Pointer updated by the DMA during operation.*/
    uint32_t dmarxcurrdesc;     /*The address of the current receive descriptor list. Cleared on Reset.Pointer updated by the DMA during operation.*/
    uint32_t dmatxcurraddr_buf; /*The address of the current receive descriptor list. Cleared on Reset.Pointer updated by the DMA during operation.*/
    uint32_t dmarxcurraddr_buf; /*The address of the current receive descriptor list. Cleared on Reset.Pointer updated by the DMA during operation.*/
    uint32_t hwfeat;            /*Hardware features*/
} emac_dma_dev_t;

extern emac_dma_dev_t EMAC_DMA;

#ifdef __cplusplus
}
#endif
