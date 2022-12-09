// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>

typedef volatile struct {
    union {
        struct {
            uint32_t pltf : 2;          /*These bits control the number of preamble bytes that are added to the beginning of every Transmit frame. The preamble reduction occurs only when the MAC is operating in the full-duplex mode.2'b00: 7 bytes of preamble. 2'b01: 5 bytes of preamble. 2'b10: 3 bytes of preamble.*/
            uint32_t rx : 1;            /*When this bit is set  the receiver state machine of the MAC is enabled for receiving frames from the MII. When this bit is reset  the MAC receive state machine is disabled after the completion of the reception of the current frame  and does not receive any further frames from the MII.*/
            uint32_t tx : 1;            /*When this bit is set  the transmit state machine of the MAC is enabled for transmission on the MII. When this bit is reset  the MAC transmit state machine is disabled after the completion of the transmission of the current frame  and does not transmit any further frames.*/
            uint32_t deferralcheck : 1; /*Deferral Check.*/
            uint32_t backofflimit : 2;  /*The Back-Off limit determines the random integer number (r) of slot time delays (512 bit times for 10/100 Mbps) for which the MAC waits before rescheduling a transmission attempt during retries after a collision. This bit is applicable only in the half-duplex mode.  00: k= min (n  10).  01: k = min (n  8).  10: k = min (n  4).  11: k = min (n  1)  n = retransmission attempt. The random integer r takes the value in the Range 0 ~ 2000.*/
            uint32_t padcrcstrip : 1;   /*When this bit is set  the MAC strips the Pad or FCS field on the incoming frames only if the value of the length field is less than 1 536 bytes. All received frames with length field greater than or equal to 1 536 bytes are passed to the application without stripping the Pad or FCS field. When this bit is reset  the MAC passes all incoming frames  without modifying them to the Host.*/
            uint32_t reserved8 : 1;
            uint32_t retry : 1;         /*When this bit is set  the MAC attempts only one transmission. When a collision occurs on the MII interface  the MAC ignores the current frame transmission and reports a Frame Abort with excessive collision error in the transmit frame status. When this bit is reset  the MAC attempts retries based on the settings of the BL field (Bits [6:5]). This bit is applicable only in the half-duplex Mode.*/
            uint32_t rxipcoffload : 1;  /*When this bit is set  the MAC calculates the 16-bit one's complement of the one's complement sum of all received Ethernet frame payloads. It also checks whether the IPv4 Header checksum (assumed to be bytes 25/26 or 29/30 (VLAN-tagged) of the received Ethernet frame) is correct for the received frame and gives the status in the receive status word. The MAC also appends the 16-bit checksum calculated for the IP header datagram payload (bytes after the IPv4 header) and appends it to the Ethernet frame transferred to the application (when Type 2 COE is deselected). When this bit is reset  this function is disabled.*/
            uint32_t duplex : 1;        /*When this bit is set  the MAC operates in the full-duplex mode where it can transmit and receive simultaneously. This bit is read only with default value of 1'b1 in the full-duplex-mode.*/
            uint32_t loopback : 1;      /*When this bit is set  the MAC operates in the loopback mode MII. The MII Receive clock input (CLK_RX) is required for the loopback to work properly  because the transmit clock is not looped-back internally.*/
            uint32_t rxown : 1;         /*When this bit is set  the MAC disables the reception of frames when the TX_EN is asserted in the half-duplex mode. When this bit is reset  the MAC receives all packets that are given by the PHY while transmitting. This bit is not applicable if the MAC is operating in the full duplex mode.*/
            uint32_t fespeed : 1;       /*This bit selects the speed in the MII  RMII interface. 0: 10 Mbps. 1: 100 Mbps.*/
            uint32_t mii : 1;           /*This bit selects the Ethernet line speed. It should be set to 1 for 10 or 100 Mbps operations.In 10 or 100 Mbps operations  this bit  along with FES(EMACFESPEED) bit  it selects the exact linespeed. In the 10/100 Mbps-only operations  the bit is always 1.*/
            uint32_t disablecrs : 1;    /*When set high  this bit makes the MAC transmitter ignore the MII CRS signal during frame transmission in the half-duplex mode. This request results in no errors generated because of Loss of Carrier or No Carrier during such transmission. When this bit is low  the MAC transmitter generates such errors because of Carrier Sense and can even abort the transmissions.*/
            uint32_t interframegap : 3; /*These bits control the minimum IFG between frames during transmission.  3'b000: 96 bit times.  3'b001: 88 bit times.  3'b010: 80 bit times.  3'b111: 40 bit times. In the half-duplex mode  the minimum IFG can be configured only for 64 bit times (IFG = 100). Lower values are not considered.*/
            uint32_t jumboframe : 1;    /*When this bit is set  the MAC allows Jumbo frames of 9 018 bytes (9 022 bytes for VLAN tagged frames) without reporting a giant frame error in the receive frame status.*/
            uint32_t reserved21 : 1;
            uint32_t jabber : 1;   /*When this bit is set  the MAC disables the jabber timer on the transmitter. The MAC can transfer frames of up to 16 383 bytes. When this bit is reset  the MAC cuts off the transmitter if the application sends out more than 2 048 bytes of data (10 240 if JE is set high) during Transmission.*/
            uint32_t watchdog : 1; /*When this bit is set  the MAC disables the watchdog timer on the receiver. The MAC can receive frames of up to 16 383 bytes. When this bit is reset  the MAC does not allow a receive frame which more than 2 048 bytes (10 240 if JE is set high) or the value programmed in Register (Watchdog Timeout Register). The MAC cuts off any bytes received after the watchdog limit number of bytes.*/
            uint32_t reserved24 : 1;
            uint32_t reserved25 : 1;
            uint32_t reserved26 : 1;
            uint32_t ass2kp : 1; /*When set  the MAC considers all frames  with up to 2 000 bytes length  as normal packets.When Bit[20] (JE) is not set  the MAC considers all received frames of size more than 2K bytes as Giant frames. When this bit is reset and Bit[20] (JE) is not set  the MAC considers all received frames of size more than 1 518 bytes (1 522 bytes for tagged) as Giant frames. When Bit[20] is set  setting this bit has no effect on Giant Frame status.*/
            uint32_t sairc : 3;  /*This field controls the source address insertion or replacement for all transmitted frames.Bit[30] specifies which MAC Address register (0 or 1) is used for source address insertion or replacement based on the values of Bits [29:28]:  2'b0x: The input signals mti_sa_ctrl_i and ati_sa_ctrl_i control the SA field generation.  2'b10: If Bit[30] is set to 0  the MAC inserts the content of the MAC Address 0 registers in the SA field of all transmitted frames. If Bit[30] is set to 1 the MAC inserts the content of the MAC Address 1 registers in the SA field of all transmitted frames.  2'b11: If Bit[30] is set to 0  the MAC replaces the content of the MAC Address 0 registers in the SA field of all transmitted frames. If Bit[30] is set to 1  the MAC replaces the content of the MAC Address 1 registers in the SA field of all transmitted frames.*/
            uint32_t reserved31 : 1;
        };
        uint32_t val;
    } gmacconfig;
    union {
        struct {
            uint32_t pmode : 1; /*When this bit is set  the Address Filter module passes all incoming frames irrespective of the destination or source address. The SA or DA Filter Fails status bits of the Receive Status Word are always cleared when PR(PRI_RATIO) is set.*/
            uint32_t reserved1 : 1;
            uint32_t reserved2 : 1;
            uint32_t daif : 1; /*When this bit is set  the Address Check block operates in inverse filtering mode for the DA address comparison for both unicast and multicast frames. When reset  normal filtering of frames is performed.*/
            uint32_t pam : 1;  /*When set  this bit indicates that all received frames with a multicast destination address (first bit in the destination address field is '1') are passed.*/
            uint32_t dbf : 1;  /*When this bit is set  the AFM(Address Filtering Module) module blocks all incoming broadcast frames. In addition  it overrides all other filter settings. When this bit is reset  the AFM module passes all received broadcast Frames.*/
            uint32_t pcf : 2;  /*These bits control the forwarding of all control frames (including unicast and multicast Pause frames).   2'b00: MAC filters all control frames from reaching the application.  2'b01: MAC forwards all control frames except Pause frames to application even if they fail the Address filter.  2'b10: MAC forwards all control frames to application even if they fail the Address Filter.  2'b11: MAC forwards control frames that pass the Address Filter.The following conditions should be true for the Pause frames processing:  Condition 1: The MAC is in the full-duplex mode and flow control is enabled by setting Bit 2 (RFE) of Register (Flow Control Register) to 1.  Condition 2: The destination address (DA) of the received frame matches the special multicast address or the MAC Address 0 when Bit 3 (UP) of the Register(Flow Control Register) is set.  Condition 3: The Type field of the received frame is 0x8808 and the OPCODE field is 0x0001.*/
            uint32_t saif : 1; /*When this bit is set  the Address Check block operates in inverse filtering mode for the SA address comparison. The frames whose SA matches the SA registers are marked as failing the SA Address filter. When this bit is reset  frames whose SA does not match the SA registers are marked as failing the SA Address filter.*/
            uint32_t safe : 1; /*When this bit is set  the MAC compares the SA field of the received frames with the values programmed in the enabled SA registers. If the comparison fails  the MAC drops the frame. When this bit is reset  the MAC forwards the received frame to the application with updated SAF bit of the Rx Status depending on the SA address comparison.*/
            uint32_t reserved10 : 1;
            uint32_t reserved11 : 5;
            uint32_t reserved16 : 1;
            uint32_t reserved17 : 3;
            uint32_t reserved20 : 1;
            uint32_t reserved21 : 1;
            uint32_t reserved22 : 9;
            uint32_t receive_all : 1; /*When this bit is set  the MAC Receiver module passes all received frames  irrespective of whether they pass the address filter or not  to the Application. The result of the SA or DA filtering is updated (pass or fail) in the corresponding bits in the Receive Status Word. When this bit is reset the Receiver module passes only those frames to the Application that pass the SA or DA address Filter.*/
        };
        uint32_t val;
    } gmacff;
    uint32_t reserved_1008;
    uint32_t reserved_100c;
    union {
        struct {
            uint32_t miibusy : 1;   /*This bit should read logic 0 before writing to PHY Addr Register and PHY data Register.During a PHY register access  the software sets this bit to 1'b1 to indicate that a Read or Write access is in progress. PHY data Register is invalid until this bit is cleared by the MAC. Therefore PHY data Register (MII Data) should be kept valid until the MAC clears this bit during a PHY Write operation. Similarly for a read operation  the contents of Register 5 are not valid until this bit is cleared. The subsequent read or write operation should happen only after the previous operation is complete. Because there is no acknowledgment from the PHY to MAC after a read or write operation is completed  there is no change in the functionality of this bit even when the PHY is not Present.*/
            uint32_t miiwrite : 1;  /*When set  this bit indicates to the PHY that this is a Write operation using the MII Data register. If this bit is not set  it indicates that this is a Read operation  that is  placing the data in the MII Data register.*/
            uint32_t miicsrclk : 4; /*CSR clock range: 1.0 MHz ~ 2.5 MHz.  4'b0000: When the APB clock frequency is 80 MHz  the MDC clock frequency is APB CLK/42  4'b0000: When the APB clock frequency is 40 MHz  the MDC clock frequency is APB CLK/26.*/
            uint32_t miireg : 5;    /*These bits select the desired MII register in the selected PHY device.*/
            uint32_t miidev : 5;    /*This field indicates which of the 32 possible PHY devices are being accessed.*/
            uint32_t reserved16 : 16;
        };
        uint32_t val;
    } emacgmiiaddr;
    union {
        struct {
            uint32_t mii_data : 16; /*This field contains the 16-bit data value read from the PHY after a Management Read operation or the 16-bit data value to be written to the PHY before a Management Write operation.*/
            uint32_t reserved16 : 16;
        };
        uint32_t val;
    } emacmiidata;
    union {
        struct {
            uint32_t fcbba : 1; /*This bit initiates a Pause frame in the full-duplex mode and activates the backpressure function in the half-duplex mode if the TFCE bit is set. In the full-duplex mode  this bit should be read as 1'b0 before writing to the Flow Control register. To initiate a Pause frame  the Application must set this bit to 1'b1. During a transfer of the Control Frame  this bit continues to be set to signify that a frame transmission is in progress. After the completion of Pause frame transmission  the MAC resets this bit to 1'b0. The Flow Control register should not be written to until this bit is cleared. In the half-duplex mode  when this bit is set (and TFCE is set)  then backpressure is asserted by the MAC. During backpressure  when the MAC receives a new frame  the transmitter starts sending a JAM pattern resulting in a collision. When the MAC is configured for the full-duplex mode  the BPA(backpressure activate) is automatically disabled.*/
            uint32_t tfce : 1;  /*In the full-duplex mode  when this bit is set  the MAC enables the flow control operation to transmit Pause frames. When this bit is reset  the flow control operation in the MAC is disabled and the MAC does not transmit any Pause frames. In the half-duplex mode  when this bit is set the MAC enables the backpressure operation. When this bit is reset  the backpressure feature is Disabled.*/
            uint32_t rfce : 1;  /*When this bit is set  the MAC decodes the received Pause frame and disables its transmitter for a specified (Pause) time. When this bit is reset  the decode function of the Pause frame is disabled.*/
            uint32_t upfd : 1;  /*A pause frame is processed when it has the unique multicast address specified in the IEEE Std 802.3. When this bit is set  the MAC can also detect Pause frames with unicast address of the station. This unicast address should be as specified in the EMACADDR0 High Register and EMACADDR0 Low Register. When this bit is reset  the MAC only detects Pause frames with unique multicast address.*/
            uint32_t plt : 2;   /*This field configures the threshold of the Pause timer automatic retransmission of the Pause frame.The threshold values should be always less than the Pause Time configured in Bits[31:16]. For example  if PT = 100H (256 slot-times)  and PLT = 01  then a second Pause frame is automatically transmitted at 228 (256-28) slot times after the first Pause frame is transmitted. The following list provides the threshold values for different values:   2'b00: The threshold is Pause time minus 4 slot times (PT-4 slot times).  2'b01: The threshold is Pause time minus 28 slot times (PT-28 slot times).  2'b10: The threshold is Pause time minus 144 slot times (PT-144 slot times).  2'b11: The threshold is Pause time minus 256 slot times (PT-256 slot times). The slot time is defined as the time taken to transmit 512 bits (64 bytes) on the MII interface.*/
            uint32_t reserved6 : 1;
            uint32_t dzpq : 1; /*When this bit is set  it disables the automatic generation of the Zero-Quanta Pause frames on the de-assertion of the flow-control signal from the FIFO layer. When this bit is reset  normal operation with automatic Zero-Quanta Pause frame generation is enabled.*/
            uint32_t reserved8 : 8;
            uint32_t pause_time : 16; /*This field holds the value to be used in the Pause Time field in the transmit control frame. If the Pause Time bits is configured to be double-synchronized to the MII clock domain then consecutive writes to this register should be performed only after at least four clock cycles in the destination clock domain.*/
        };
        uint32_t val;
    } gmacfc;
    uint32_t reserved_101c;
    uint32_t reserved_1020;
    union {
        struct {
            uint32_t macrpes : 1;  /*When high  this bit indicates that the MAC MII receive protocol engine is actively receiving data and not in IDLE state.*/
            uint32_t macrffcs : 2; /*When high  this field indicates the active state of the FIFO Read and Write controllers of the MAC Receive Frame Controller Module. MACRFFCS[1] represents the status of FIFO Read controller. MACRFFCS[0] represents the status of small FIFO Write controller.*/
            uint32_t reserved3 : 1;
            uint32_t mtlrfwcas : 1; /*When high  this bit indicates that the MTL Rx FIFO Write Controller is active and is transferring a received frame to the FIFO.*/
            uint32_t mtlrfrcs : 2;  /*This field gives the state of the Rx FIFO read Controller: 2'b00: IDLE state.2'b01: Reading frame data.2'b10: Reading frame status (or timestamp).2'b11: Flushing the frame data and status.*/
            uint32_t reserved7 : 1;
            uint32_t mtlrffls : 2; /*This field gives the status of the fill-level of the Rx FIFO:   2'b00: Rx FIFO Empty.  2'b01: Rx FIFO fill-level below flow-control deactivate threshold.  2'b10: Rx FIFO fill-level above flow-control activate threshold.  2'b11: Rx FIFO Full.*/
            uint32_t reserved10 : 6;
            uint32_t mactpes : 1;  /*When high  this bit indicates that the MAC MII transmit protocol engine is actively transmitting data and is not in the IDLE state.*/
            uint32_t mactfcs : 2;  /*This field indicates the state of the MAC Transmit Frame Controller module:   2'b00: IDLE state.  2'b01: Waiting for status of previous frame or IFG or backoff period to be over.  2'b10: Generating and transmitting a Pause frame (in the full-duplex mode).  2'b11: Transferring input frame for transmission.*/
            uint32_t mactp : 1;    /*When high  this bit indicates that the MAC transmitter is in the Pause condition (in the full-duplex-mode) and hence does not schedule any frame for transmission.*/
            uint32_t mtltfrcs : 2; /*This field indicates the state of the Tx FIFO Read Controller:   2'b00: IDLE state.  2'b01: READ state (transferring data to the MAC transmitter).  2'b10: Waiting for TxStatus from the MAC transmitter.  2'b11: Writing the received TxStatus or flushing the Tx FIFO.*/
            uint32_t mtltfwcs : 1; /*When high  this bit indicates that the MTL Tx FIFO Write Controller is active and is transferring data to the Tx FIFO.*/
            uint32_t reserved23 : 1;
            uint32_t mtltfnes : 1; /*When high  this bit indicates that the MTL Tx FIFO is not empty and some data is left for Transmission.*/
            uint32_t mtltsffs : 1; /*When high  this bit indicates that the MTL TxStatus FIFO is full. Therefore  the MTL cannot accept any more frames for transmission.*/
            uint32_t reserved26 : 6;
        };
        uint32_t val;
    } emacdebug;
    uint32_t pmt_rwuffr; /*The MSB (31st bit) must be zero.Bit j[30:0] is the byte mask. If Bit 1/2/3/4 (byte number) of the byte mask is set  the CRC block processes the Filter 1/2/3/4 Offset + j of the incoming packet(PWKPTR is 0/1/2/3).RWKPTR is 0:Filter 0 Byte Mask .RWKPTR is 1:Filter 1 Byte Mask RWKPTR is 2:Filter 2 Byte Mask RWKPTR is 3:Filter 3 Byte Mask RWKPTR is 4:Bit 3/11/19/27 specifies the address type  defining the destination address type of the pattern.When the bit is set  the pattern applies to only multicast packets*/
    union {
        struct {
            uint32_t pwrdwn : 1;   /*When set  the MAC receiver drops all received frames until it receives the expected magic packet or remote wake-up frame.This bit must only be set when MGKPKTEN  GLBLUCAST  or RWKPKTEN bit is set high.*/
            uint32_t mgkpkten : 1; /*When set  enables generation of a power management event because of magic packet reception.*/
            uint32_t rwkpkten : 1; /*When set  enables generation of a power management event because of remote wake-up frame reception*/
            uint32_t reserved3 : 2;
            uint32_t mgkprcvd : 1; /*When set  this bit indicates that the power management event is generated because of the reception of a magic packet. This bit is cleared by a Read into this register.*/
            uint32_t rwkprcvd : 1; /*When set  this bit indicates the power management event is generated because of the reception of a remote wake-up frame. This bit is cleared by a Read into this register.*/
            uint32_t reserved7 : 2;
            uint32_t glblucast : 1; /*When set  enables any unicast packet filtered by the MAC (DAFilter) address recognition to be a remote wake-up frame.*/
            uint32_t reserved10 : 14;
            uint32_t rwkptr : 5; /*The maximum value of the pointer is 7  the detail information please refer to PMT_RWUFFR.*/
            uint32_t reserved29 : 2;
            uint32_t rwkfiltrst : 1; /*When this bit is set  it resets the RWKPTR register to 3’b000.*/
        };
        uint32_t val;
    } pmt_csr;
    union {
        struct {
            uint32_t tlpien : 1; /*When set  this bit indicates that the MAC Transmitter has entered the LPI state because of the setting of the LPIEN bit. This bit is cleared by a read into this register.*/
            uint32_t tlpiex : 1; /*When set  this bit indicates that the MAC transmitter has exited the LPI state after the user has cleared the LPIEN bit and the LPI_TW_Timer has expired.This bit is cleared by a read into this register.*/
            uint32_t rlpien : 1; /*When set  this bit indicates that the MAC Receiver has received an LPI pattern and entered the LPI state. This bit is cleared by a read into this register.*/
            uint32_t rlpiex : 1; /*When set  this bit indicates that the MAC Receiver has stopped receiving the LPI pattern on the MII interface  exited the LPI state  and resumed the normal reception. This bit is cleared by a read into this register.*/
            uint32_t reserved4 : 4;
            uint32_t tlpist : 1; /*When set  this bit indicates that the MAC is transmitting the LPI pattern on the MII interface.*/
            uint32_t rlpist : 1; /*When set  this bit indicates that the MAC is receiving the LPI pattern on the MII interface.*/
            uint32_t reserved10 : 6;
            uint32_t lpien : 1; /*When set  this bit instructs the MAC Transmitter to enter the LPI state. When reset  this bit instructs the MAC to exit the LPI state and resume normal transmission.This bit is cleared when the LPITXA bit is set and the MAC exits the LPI state because of the arrival of a new packet for transmission.*/
            uint32_t pls : 1;   /*This bit indicates the link status of the PHY.When set  the link is considered to be okay (up) and when reset  the link is considered to be down.*/
            uint32_t reserved18 : 1;
            uint32_t lpitxa : 1; /*This bit controls the behavior of the MAC when it is entering or coming out of the LPI mode on the transmit side.If the LPITXA and LPIEN bits are set to 1  the MAC enters the LPI mode only after all outstanding frames and pending frames have been transmitted. The MAC comes out of the LPI mode when the application sends any frame.When this bit is 0  the LPIEN bit directly controls behavior of the MAC when it is entering or coming out of the LPI mode.*/
            uint32_t reserved20 : 12;
        };
        uint32_t val;
    } gmaclpi_crs;
    union {
        struct {
            uint32_t lpi_tw_timer : 16; /*This field specifies the minimum time (in microseconds) for which the MAC waits  after it stops transmitting the LPI pattern to the PHY and before it resumes the normal transmission. The TLPIEX status bit is set after the expiry of this timer.*/
            uint32_t lpi_ls_timer : 10; /*This field specifies the minimum time (in milliseconds) for which the link status from the PHY should be up (OKAY) before the LPI pattern can be transmitted to the PHY. The MAC does not transmit the LPI pattern even when the LPIEN bit is set unless the LPI_LS_Timer reaches the programmed terminal count. The default value of the LPI_LS_Timer is 1000 (1 sec) as defined in the IEEE standard.*/
            uint32_t reserved26 : 6;
        };
        uint32_t val;
    } gmaclpitimerscontrol;
    union {
        struct {
            uint32_t reserved0 : 1;
            uint32_t reserved1 : 1;
            uint32_t reserved2 : 1;
            uint32_t pmtints : 1; /*This bit is set when a magic packet or remote wake-up frame is received in the power-down mode (see Bit[5] and Bit[6] in the PMT Control and Status Register). This bit is cleared when both Bits[6:5] are cleared because of a read operation to the PMT Control and Status register. This bit is valid only when you select the optional PMT module during core configuration.*/
            uint32_t reserved4 : 1;
            uint32_t reserved5 : 1;
            uint32_t reserved6 : 1;
            uint32_t reserved7 : 1;
            uint32_t reserved8 : 1;
            uint32_t reserved9 : 1;
            uint32_t lpiis : 1; /*When the Energy Efficient Ethernet feature is enabled  this bit is set for any LPI state entry or exit in the MAC Transmitter or Receiver. This bit is cleared on reading Bit[0] of Register (LPI Control and Status Register).*/
            uint32_t reserved11 : 1;
            uint32_t reserved12 : 20;
        };
        uint32_t val;
    } emacints;
    union {
        struct {
            uint32_t reserved0 : 1;
            uint32_t reserved1 : 1;
            uint32_t reserved2 : 1;
            uint32_t pmtintmask : 1; /*When set  this bit disables the assertion of the interrupt signal because of the setting of PMT Interrupt Status bit in Register (Interrupt Status Register).*/
            uint32_t reserved4 : 5;
            uint32_t reserved9 : 1;
            uint32_t lpiintmask : 1; /*When set  this bit disables the assertion of the interrupt signal because of the setting of the LPI Interrupt Status bit in Register (Interrupt Status Register).*/
            uint32_t reserved11 : 21;
        };
        uint32_t val;
    } emacintmask;
    union {
        struct {
            uint32_t address0_hi : 16; /*This field contains the upper 16 bits (47:32) of the first 6-byte MAC address.The MAC uses this field for filtering the received frames and inserting the MAC address in the Transmit Flow Control (Pause) Frames.*/
            uint32_t reserved16 : 15;
            uint32_t address_enable0 : 1; /*This bit is always set to 1.*/
        };
        uint32_t val;
    } emacaddr0high;
    uint32_t emacaddr0low; /*This field contains the lower 32 bits of the first 6-byte MAC address. This is used by the MAC for filtering the received frames and inserting the MAC address in the Transmit Flow Control (Pause) Frames.*/
    union {
        struct {
            uint32_t mac_address1_hi : 16; /*This field contains the upper 16 bits  Bits[47:32] of the second 6-byte MAC Address.*/
            uint32_t reserved16 : 8;
            uint32_t mask_byte_control : 6; /*These bits are mask control bits for comparison of each of the EMACADDR1 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR1 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR1 High [15:8].  Bit[28]: EMACADDR1 High [7:0].  Bit[27]: EMACADDR1 Low [31:24].  Bit[24]: EMACADDR1 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
            uint32_t source_address : 1;    /*When this bit is set  the EMACADDR1[47:0] is used to compare with the SA fields of the received frame. When this bit is reset  the EMACADDR1[47:0] is used to compare with the DA fields of the received frame.*/
            uint32_t address_enable1 : 1;   /*When this bit is set  the address filter module uses the second MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
        };
        uint32_t val;
    } emacaddr1high;
    uint32_t emacaddr1low; /*This field contains the lower 32 bits of the second 6-byte MAC address.The content of this field is undefined  so the register needs to be configured after the initialization Process.*/
    union {
        struct {
            uint32_t mac_address2_hi : 16; /*This field contains the upper 16 bits  Bits[47:32] of the third 6-byte MAC address.*/
            uint32_t reserved16 : 8;
            uint32_t mask_byte_control2 : 6; /*These bits are mask control bits for comparison of each of the EMACADDR2 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR2 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR2 High [15:8].  Bit[28]: EMACADDR2 High [7:0].  Bit[27]: EMACADDR2 Low [31:24].  Bit[24]: EMACADDR2 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
            uint32_t source_address2 : 1;    /*When this bit is set  the EMACADDR2[47:0] is used to compare with the SA fields of the received frame. When this bit is reset  the EMACADDR2[47:0] is used to compare with the DA fields of the received frame.*/
            uint32_t address_enable2 : 1;    /*When this bit is set  the address filter module uses the third MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
        };
        uint32_t val;
    } emacaddr2high;
    uint32_t emacaddr2low; /*This field contains the lower 32 bits of the third 6-byte MAC address. The content of this field is undefined  so the register needs to be configured after the initialization process.*/
    union {
        struct {
            uint32_t mac_address3_hi : 16; /*This field contains the upper 16 bits  Bits[47:32] of the fourth 6-byte MAC address.*/
            uint32_t reserved16 : 8;
            uint32_t mask_byte_control3 : 6; /*These bits are mask control bits for comparison of each of the EMACADDR3 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR3 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR3 High [15:8].  Bit[28]: EMACADDR3 High [7:0].  Bit[27]: EMACADDR3 Low [31:24].  Bit[24]: EMACADDR3 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
            uint32_t source_address3 : 1;    /*When this bit is set  the EMACADDR3[47:0] is used to compare with the SA fields of the received frame. When this bit is reset  the EMACADDR3[47:0] is used to compare with the DA fields of the received frame.*/
            uint32_t address_enable3 : 1;    /*When this bit is set  the address filter module uses the fourth MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
        };
        uint32_t val;
    } emacaddr3high;
    uint32_t emacaddr3low; /*This field contains the lower 32 bits of the fourth 6-byte MAC address.The content of this field is undefined  so the register needs to be configured after the initialization Process.*/
    union {
        struct {
            uint32_t mac_address4_hi : 16; /*This field contains the upper 16 bits  Bits[47:32] of the fifth 6-byte MAC address.*/
            uint32_t reserved16 : 8;
            uint32_t mask_byte_control4 : 6; /*These bits are mask control bits for comparison of each of the EMACADDR4 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR4 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR4 High [15:8].  Bit[28]: EMACADDR4 High [7:0].  Bit[27]: EMACADDR4 Low [31:24].  Bit[24]: EMACADDR4 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
            uint32_t source_address4 : 1;    /*When this bit is set  the EMACADDR4[47:0] is used to compare with the SA fields of the received frame. When this bit is reset  the EMACADDR4[47:0] is used to compare with the DA fields of the received frame.*/
            uint32_t address_enable4 : 1;    /*When this bit is set  the address filter module uses the fifth MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
        };
        uint32_t val;
    } emacaddr4high;
    uint32_t emacaddr4low; /*This field contains the lower 32 bits of the fifth 6-byte MAC address. The content of this field is undefined  so the register needs to be configured after the initialization process.*/
    union {
        struct {
            uint32_t mac_address5_hi : 16; /*This field contains the upper 16 bits  Bits[47:32] of the sixth 6-byte MAC address.*/
            uint32_t reserved16 : 8;
            uint32_t mask_byte_control5 : 6; /*These bits are mask control bits for comparison of each of the EMACADDR5 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR5 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR5 High [15:8].  Bit[28]: EMACADDR5 High [7:0].  Bit[27]: EMACADDR5 Low [31:24].  Bit[24]: EMACADDR5 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
            uint32_t source_address5 : 1;    /*When this bit is set  the EMACADDR5[47:0] is used to compare with the SA fields of the received frame. When this bit is reset  the EMACADDR5[47:0] is used to compare with the DA fields of the received frame.*/
            uint32_t address_enable5 : 1;    /*When this bit is set  the address filter module uses the sixth MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
        };
        uint32_t val;
    } emacaddr5high;
    uint32_t emacaddr5low; /*This field contains the lower 32 bits of the sixth 6-byte MAC address. The content of this field is undefined  so the register needs to be configured after the initialization process.*/
    union {
        struct {
            uint32_t mac_address6_hi : 16; /*This field contains the upper 16 bits  Bits[47:32] of the seventh 6-byte MAC Address.*/
            uint32_t reserved16 : 8;
            uint32_t mask_byte_control6 : 6; /*These bits are mask control bits for comparison of each of the EMACADDR6 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR6 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR6 High [15:8].  Bit[28]: EMACADDR6 High [7:0].  Bit[27]: EMACADDR6 Low [31:24].  Bit[24]: EMACADDR6 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
            uint32_t source_address6 : 1;    /*When this bit is set  the EMACADDR6[47:0] is used to compare with the SA fields of the received frame. When this bit is reset  the EMACADDR6[47:0] is used to compare with the DA fields of the received frame.*/
            uint32_t address_enable6 : 1;    /*When this bit is set  the address filter module uses the seventh MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
        };
        uint32_t val;
    } emacaddr6high;
    uint32_t emacaddr6low; /*This field contains the lower 32 bits of the seventh 6-byte MAC address.The content of this field is undefined  so the register needs to be configured after the initialization Process.*/
    union {
        struct {
            uint32_t mac_address7_hi : 16; /*This field contains the upper 16 bits  Bits[47:32] of the eighth 6-byte MAC Address.*/
            uint32_t reserved16 : 8;
            uint32_t mask_byte_control7 : 6; /*These bits are mask control bits for comparison of each of the EMACADDR7 bytes. When set high  the MAC does not compare the corresponding byte of received DA or SA with the contents of EMACADDR7 registers. Each bit controls the masking of the bytes as follows:  Bit[29]: EMACADDR7 High [15:8].  Bit[28]: EMACADDR7 High [7:0].  Bit[27]: EMACADDR7 Low [31:24].  Bit[24]: EMACADDR7 Low [7:0].You can filter a group of addresses (known as group address filtering) by masking one or more bytes of the address.*/
            uint32_t source_address7 : 1;    /*When this bit is set  the EMACADDR7[47:0] is used to compare with the SA fields of the received frame. When this bit is reset  the EMACADDR7[47:0] is used to compare with the DA fields of the received frame.*/
            uint32_t address_enable7 : 1;    /*When this bit is set  the address filter module uses the eighth MAC address for perfect filtering. When this bit is reset  the address filter module ignores the address for filtering.*/
        };
        uint32_t val;
    } emacaddr7high;
    uint32_t emacaddr7low; /*This field contains the lower 32 bits of the eighth 6-byte MAC address.The content of this field is undefined  so the register needs to be configured after the initialization Process.*/
    uint32_t reserved_1080;
    uint32_t reserved_1084;
    uint32_t reserved_1088;
    uint32_t reserved_108c;
    uint32_t reserved_1090;
    uint32_t reserved_1094;
    uint32_t reserved_1098;
    uint32_t reserved_109c;
    uint32_t reserved_10a0;
    uint32_t reserved_10a4;
    uint32_t reserved_10a8;
    uint32_t reserved_10ac;
    uint32_t reserved_10b0;
    uint32_t reserved_10b4;
    uint32_t reserved_10b8;
    uint32_t reserved_10bc;
    uint32_t reserved_10c0;
    uint32_t reserved_10c4;
    uint32_t reserved_10c8;
    uint32_t reserved_10cc;
    uint32_t reserved_10d0;
    uint32_t reserved_10d4;
    union {
        struct {
            uint32_t link_mode : 1;  /*This bit indicates the current mode of operation of the link:   1'b0: Half-duplex mode.  1'b1: Full-duplex mode.*/
            uint32_t link_speed : 2; /*This bit indicates the current speed of the link:   2'b00: 2.5 MHz.  2'b01: 25 MHz.  2'b10: 125 MHz.*/
            uint32_t reserved3 : 1;
            uint32_t jabber_timeout : 1; /*This bit indicates whether there is jabber timeout error (1'b1) in the received Frame.*/
            uint32_t reserved5 : 1;
            uint32_t reserved6 : 10;
            uint32_t reserved16 : 1;
            uint32_t reserved17 : 15;
        };
        uint32_t val;
    } emaccstatus;
    union {
        struct {
            uint32_t wdogto : 14; /*When Bit[16] (PWE) is set and Bit[23] (WD) of EMACCONFIG_REG is reset  this field is used as watchdog timeout for a received frame. If the length of a received frame exceeds the value of this field  such frame is terminated and declared as an error frame.*/
            uint32_t reserved14 : 2;
            uint32_t pwdogen : 1; /*When this bit is set and Bit[23] (WD) of EMACCONFIG_REG is reset  the WTO field (Bits[13:0]) is used as watchdog timeout for a received frame. When this bit is cleared  the watchdog timeout for a received frame is controlled by the setting of Bit[23] (WD) and Bit[20] (JE) in EMACCONFIG_REG.*/
            uint32_t reserved17 : 15;
        };
        uint32_t val;
    } emacwdogto;
} emac_mac_dev_t;

extern emac_mac_dev_t EMAC_MAC;

#ifdef __cplusplus
}
#endif
