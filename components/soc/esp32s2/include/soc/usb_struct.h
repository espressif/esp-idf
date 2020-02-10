// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Control/Status registers */
/** Type of gotgctl register
 *  OTG Control and Status Register
 */
typedef volatile struct {
    /** sesreqscs : RO; bitpos: [0]; default: 0;
     *  Session Request Success.The core sets this bit when a session request initiation is
     *  successful
     */
    uint32_t sesreqscs: 1;
    /** sesreq : R/W; bitpos: [1]; default: 0;
     *  Session Request.The application sets this bit to initiate a session request on the
     *  USB. The application can clear this bit by writing a 0 when the Host Negotiation
     *  Success Status Change bit in the OTG Interrupt register
     *  (GOTGINT_REG.USB_HSTNEGSUCSTSCHNG) is SET. The core clears this bit when the
     *  USB_HSTNEGSUCSTSCHNG bit is cleared
     */
    uint32_t sesreq: 1;
    /** vbvalidoven : R/W; bitpos: [2]; default: 0;
     *  VBUS Valid Override Enable
     *  1'b1 : Internally Bvalid received from the PHY is overridden with
     *  GOTGCTL_REG.REG_VBVALIDOVVAl
     *  1'b0 : Override is disabled and bvalid signal from the respective PHY selected is
     *  used internally by the controller
     */
    uint32_t vbvalidoven: 1;
    /** vbvalidovval : R/W; bitpos: [3]; default: 0;
     *  VBUS Valid OverrideValue
     *  1'b0 : vbusvalid value is 1'b0 when GOTGCTL_REG.USB_VBVALIDOVEN =1
     *  1'b1 : vbusvalid value is 1'b1 when GOTGCTL_REG.USB_VBVALIDOVEN =1
     */
    uint32_t vbvalidovval: 1;
    /** avalidoven : R/W; bitpos: [4]; default: 0;
     *  This bit is used to enable/disable the software to override the Avalid signal using
     *  the GOTGCTL.AVALIDOVVAL
     *  1'b1: Internally Avalid received from the PHY is overridden with
     *  GOTGCTL_REG.REG_AVALIDOVVAL
     *  1'b0: Override is disabled and avalid signal from the respective PHY selected is
     *  used internally by the core
     */
    uint32_t avalidoven: 1;
    /** avalidovval : R/W; bitpos: [5]; default: 0;
     *  A-Peripheral Session Valid OverrideValue
     *  1'b0 : Avalid value is 1'b0 when GOTGCTL_REG.USB_AVALIDOVEN =1
     *  1'b1 : Avalid value is 1'b1 when GOTGCTL_REG.USB_AVALIDOVEN =1
     */
    uint32_t avalidovval: 1;
    /** bvalidoven : R/W; bitpos: [6]; default: 0;
     *  This bit is used to enable/disable the software to override the Bvalid signal using
     *  the GOTGCTLREG.BVALIDOVVAL
     *  1'b1 : Internally Bvalid received from the PHY is overridden with
     *  GOTGCTL_REG.USB_BVALIDOVVAL
     *  1'b0 : Override is disabled and bvalid signal from the respective PHY selected is
     *  used internally by the force
     */
    uint32_t bvalidoven: 1;
    /** bvalidovval : R/W; bitpos: [7]; default: 0;
     *  B-Peripheral Session Valid OverrideValue
     *  1'b0 : Bvalid value is 1'b0 when GOTGCTL_REG.USB_BVALIDOVEN =1
     *  1'b1 : Bvalid value is 1'b1 when GOTGCTL_REG.USB_BVALIDOVEN =1
     */
    uint32_t bvalidovval: 1;
    /** hstnegscs : RO; bitpos: [8]; default: 0;
     *  Host Negotiation Success.The controller sets this bit when host negotiation is
     *  successful. The controller clears this bit when the HNP Request (HNPReq) bit in
     *  this register is set.
     */
    uint32_t hstnegscs: 1;
    /** hnpreq : R/W; bitpos: [9]; default: 0;
     *  HNP Request .The application sets this bit to initiate an HNP request to the
     *  Connected USB host. The application can clear this bit by writing a 0 when the Host
     *  Negotiation Success Status Change bit in the OTG Interrupt register
     *  (GOTGINT_REG.HSTNEGSUCSTSCHNG) is SET. The controller clears this bit when the
     *  HSTNEGSUCSTSCHNG bit is cleared.
     */
    uint32_t hnpreq: 1;
    /** hstsethnpen : R/W; bitpos: [10]; default: 0;
     *  Host Set HNP Enable.The application sets this bit when it has successfully enabled
     *  HNP (using the SetFeature.SetHNPEnable command) on the connected device
     *  1'b0: Host Set HNP is not enabled
     *  1'b1: Host Set HNP is enabled
     */
    uint32_t hstsethnpen: 1;
    /** devhnpen : R/W; bitpos: [11]; default: 0;
     *  Device HNP Enabled.The application sets this bit when it successfully receives a
     *  SetFeature.SetHNPEnable command from the connected USB host
     *  1'b0: HNP is not enabled in the application
     *  1'b1: HNP is enabled in the application
     */
    uint32_t devhnpen: 1;
    /** ehen : R/W; bitpos: [12]; default: 0;
     *  Embedded Host Enable.It is used to select between OTG A Device state Machine and
     *  Embedded Host state machine
     *  1'b0: OTG A Device state machine is selected
     *  1'b1: Embedded Host State Machine is selected
     */
    uint32_t ehen: 1;
    uint32_t reserved_13: 2
    /** dbncefltrbypass : R/W; bitpos: [15]; default: 0;
     *  Bypass Debounce filters for avalid, bvalid, vbusvalid, sessend, iddig signals when
     *  enabled
     *  1'b0: Disabled
     *  1'b1: Enabled
     */
    uint32_t dbncefltrbypass: 1;
    /** conidsts : RO; bitpos: [16]; default: 0;
     *  Connector ID Status. Indicates the connector ID status on a connect event
     *  1'b0: The core is in A-Device mode
     *  1'b1: The core is in B-Device mode
     */
    uint32_t conidsts: 1;
    /** dbnctime : RO; bitpos: [17]; default: 0;
     *  Long/Short Debounce Time. Indicates the debounce time of a detected connection
     *  1'b0: Long debounce time, used for physical connections (100ms + 2.5 micro-sec)
     *  1'b1: Short debounce time, used for soft connections (2.5 micro-sec)
     */
    uint32_t dbnctime: 1;
    /** asesvld : RO; bitpos: [18]; default: 0;
     *  A-Session Valid. Indicates the Host mode transceiver status
     *  1'b0: A-session is not valid
     *  1'b1: A-session is valid
     */
    uint32_t asesvld: 1;
    /** bsesvld : RO; bitpos: [19]; default: 0;
     *  B-Session Valid.Indicates the Device mode transceiver status
     *  1'b0: B-session is not valid
     *  1'b1: B-session is valid
     */
    uint32_t bsesvld: 1;
    /** otgver : R/W; bitpos: [20]; default: 0;
     *  OTG Version
     *  1'b0:Supports OTG Version 1.3
     *  1'b1:Supports OTG Version 2.0
     */
    uint32_t otgver: 1;
    /** curmod : RO; bitpos: [21]; default: 0;
     *  Current Mode of Operation
     *  1'b0: Device mode
     *  1'b1:Host mode
     */
    uint32_t curmod: 1;


} usb_gotgctl_reg_t;

/** Type of gdfifocfg register
 *  Global DFIFO Configuration Register
 */
typedef volatile struct {
    /** gdfifocfg : R/W; bitpos: [16:0]; default: 0;
     *  GDFIFOCfg
     */
    uint32_t gdfifocfg: 16;
    /** epinfobaseaddr : R/W; bitpos: [32:16]; default: 0;
     *  EPInfoBaseAddr
     */
    uint32_t epinfobaseaddr: 16;


} usb_gdfifocfg_reg_t;

/** Type of hptxfsiz register
 *  Host Periodic Transmit FIFO Size Register
 */
typedef volatile struct {
    /** ptxfstaddr : R/W; bitpos: [16:0]; default: 512;
     *  Host Periodic TxFIFO Start Address.
     */
    uint32_t ptxfstaddr: 16;
    /** ptxfsize : R/W; bitpos: [32:16]; default: 4096;
     *  Host Periodic TxFIFO Depth,This value is in terms of 32-bit words..
     */
    uint32_t ptxfsize: 16;


} usb_hptxfsiz_reg_t;

/** Type of dieptxfn register
 *  Device IN Endpoint Transmit FIFO Size Register
 */
typedef volatile struct {
    /** inepntxfstaddr : R/W; bitpos: [16:0]; default: 512;
     *  IN Endpoint FIFOn Transmit RAM Start Address.
     */
    uint32_t inepntxfstaddr: 16;
    /** inepntxfdep : R/W; bitpos: [32:16]; default: 4096;
     *  IN Endpoint TxFIFO Depth
     */
    uint32_t inepntxfdep: 16;


} usb_dieptxfn_reg_t;

/** Type of hcfg register
 *  Host Configuration Register
 */
typedef volatile struct {
    /** h_fslspclksel : R/W; bitpos: [2:0]; default: 0;
     *  0x0 : PHY clock is running at 30/60 MHz
     *  0x1 : PHY clock is running at 48 MHz
     *  0x2 : PHY clock is running at 6 MHz
     */
    uint32_t h_fslspclksel: 2;
    /** h_fslssupp : R/W; bitpos: [2]; default: 0;
     *  FS- and LS-Only Support
     *  1'b0: HS/FS/LS, based on the maximum speed supported by the connected device
     *  1'b1: FS/LS-only, even If the connected device can support HS
     */
    uint32_t h_fslssupp: 1;
    uint32_t reserved_3: 4
    /** h_ena32khzs : R/W; bitpos: [7]; default: 0;
     *  1'b0:32 KHz Suspend mode disabled
     *  1'b1:32 KHz Suspend mode enabled
     */
    uint32_t h_ena32khzs: 1;
    uint32_t reserved_8: 15
    /** h_descdma : R/W; bitpos: [23]; default: 0;
     *  GAHBCFG_REG.USB_DMAEN=0,HCFG_REG.USB_DESCDMA=0 => Slave mode
     *  GAHBCFG_REG.USB_DMAEN=0,HCFG_REG.USB_DESCDMA=1 => Invalid
     *  GAHBCFG_REG.USB_DMAEN=1,HCFG_REG.USB_DESCDMA=0 => Buffered DMA
     *  GAHBCFG_REG.USB_DMAEN=1,HCFG_REG.USB_DESCDMA=1 => Scatter/Gather DMA mode
     */
    uint32_t h_descdma: 1;
    /** h_frlisten : R/W; bitpos: [26:24]; default: 0;
     *  Frame List Entries
     *  2'b00: 8 Entries
     *  2'b01: 16 Entries
     *  2'b10: 32 Entries
     *  2'b11: 64 Entries
     */
    uint32_t h_frlisten: 2;
    /** h_perschedena : R/W; bitpos: [26]; default: 0;
     *  0x0 (DISABLED): Disables periodic scheduling within the core
     *  0x1 (ENABLED): Enables periodic scheduling within the core
     */
    uint32_t h_perschedena: 1;
    uint32_t reserved_27: 4
    /** h_modechtimen : R/W; bitpos: [31]; default: 0;
     *  Mode Change Ready Timer Enable,
     *  1'b0 : The Host core waits for either 200 PHY clock cycles or a linestate of SE0 at
     *  the end of resume to the change the opmode from 2'b10 to 2'b00.
     *  1'b1 : The Host core waits only for a linstate of SE0 at the end of resume to
     *  change the opmode from 2'b10 to 2'b00.
     */
    uint32_t h_modechtimen: 1;


} usb_hcfg_reg_t;

/** Type of hfir register
 *  Host Frame Interval Register
 */
typedef volatile struct {
    /** frint : R/W; bitpos: [16:0]; default: 6103;
     *  Frame Interval .  1 ms * (PHY clock frequency for FS/LS)
     */
    uint32_t frint: 16;
    /** hfirrldctrl : R/W; bitpos: [16]; default: 0;
     *  Reload Control
     *  1'b0 : The HFIR cannot be reloaded dynamically
     *  1'b1: the HFIR can be dynamically reloaded during runtime
     */
    uint32_t hfirrldctrl: 1;


} usb_hfir_reg_t;

/** Type of hflbaddr register
 *  Host Frame List Base Address Register
 */
typedef volatile struct {
    /** hflbaddr : R/W; bitpos: [32:0]; default: 0;
     *  The starting address of the Frame list. This register is used only for Isochronous
     *  and Interrupt Channels.
     */
    uint32_t hflbaddr: 32;


} usb_hflbaddr_reg_t;

/** Type of hprt register
 *  Host Port Control and Status Register
 */
typedef volatile struct {
    /** prtconnsts : RO; bitpos: [0]; default: 0;
     *  Port Connect Status
     *  0x0: No device is attached to the port
     *  0x1: A device is attached to the port
     */
    uint32_t prtconnsts: 1;
    /** prtconndet : R/W; bitpos: [1]; default: 0;
     *  Port Connect Detected.
     *  0x1 : Device connection detected.
     *  0x0 : No device connection detected.
     */
    uint32_t prtconndet: 1;
    /** prtena : R/W; bitpos: [2]; default: 0;
     *  Port Enable
     *  1'b0: Port disabled
     *  1'b1: Port enabled
     */
    uint32_t prtena: 1;
    /** prtenchng : R/W; bitpos: [3]; default: 0;
     *  Port Enable/Disable Change
     *  0x0 : Port Enable bit 2 has not changed
     *  0x1 : Port Enable bit 2 changed
     */
    uint32_t prtenchng: 1;
    /** prtovrcurract : RO; bitpos: [4]; default: 0;
     *  Port Overcurrent Active
     *  1'b0: No overcurrent condition
     *  1'b1: Overcurrent condition
     */
    uint32_t prtovrcurract: 1;
    /** prtovrcurrchng : R/W; bitpos: [5]; default: 0;
     *  Port Overcurrent Change
     *  0x0: Status of port overcurrent status is not changed
     *  0x1: Status of port overcurrent changed
     */
    uint32_t prtovrcurrchng: 1;
    /** prtres : R/W; bitpos: [6]; default: 0;
     *  Port Resume
     *  1'b0: No resume driven
     *  1'b1: Resume driven
     */
    uint32_t prtres: 1;
    /** prtsusp : R/W; bitpos: [7]; default: 0;
     *  Port Suspend
     *  1'b0: Port not in Suspend mode
     *  1'b1: Port in Suspend mode
     */
    uint32_t prtsusp: 1;
    /** prtrst : R/W; bitpos: [8]; default: 0;
     *  Port Reset.
     *  1'b0: Port not in reset
     *  1'b1: Port in reset
     */
    uint32_t prtrst: 1;
    uint32_t reserved_9: 1
    /** prtlnsts : RO; bitpos: [12:10]; default: 0;
     *  Port Line Status
     *  Bit [10]: Logic level of D+
     *  Bit [11]: Logic level of D-
     */
    uint32_t prtlnsts: 2;
    /** prtpwr : R/W; bitpos: [12]; default: 0;
     *  Port Power
     *  1'b0: Power off
     *  1'b1: Power on
     */
    uint32_t prtpwr: 1;
    /** prttstctl : R/W; bitpos: [17:13]; default: 0;
     *  Port Test Control
     *  4'b0000: Test mode disabled
     *  4'b0001: Test_J mode
     *  4'b0010: Test_K mode
     *  4'b0011: Test_SE0_NAK mode
     *  4'b0100: Test_Packet mode
     *  4'b0101: Test_Force_Enable
     *  Others: Reserved
     */
    uint32_t prttstctl: 4;
    /** prtspd : RO; bitpos: [19:17]; default: 0;
     *  Port Speed
     *  2'b00: High speed
     *  2'b01: Full speed
     *  2'b10: Low speed
     *  2'b11: Reserved
     */
    uint32_t prtspd: 2;


} usb_hprt_reg_t;

/** Type of hccharn register
 *  Host Channel n Characteristics Register
 */
typedef volatile struct {
    /** h_mpsn : R/W; bitpos: [11:0]; default: 0;
     *  Maximum Packet Size.
     */
    uint32_t h_mpsn: 11;
    /** h_epnumn : R/W; bitpos: [15:11]; default: 0;
     *  Endpoint Number.
     */
    uint32_t h_epnumn: 4;
    /** h_epdirn : R/W; bitpos: [15]; default: 0;
     *  1'b0: OUT
     *  1'b1: IN
     */
    uint32_t h_epdirn: 1;
    uint32_t reserved_16: 1
    /** h_lspddevn : R/W; bitpos: [17]; default: 0;
     *  0x0: Not Communicating with low speed device
     *  0x1: Communicating with low speed device
     */
    uint32_t h_lspddevn: 1;
    /** h_eptypen : R/W; bitpos: [20:18]; default: 0;
     *  0x0 (CTRL): Contro
     *  0x1 (ISOC): Isochronous
     *  0x2 (BULK): Bulk
     *  0x3 (INTERR): Interrupt
     */
    uint32_t h_eptypen: 2;
    uint32_t reserved_20: 1
    /** h_ecn : R/W; bitpos: [21]; default: 0;
     *  Multi Count (MC) / Error Count(EC)
     *  0x0 (RESERVED): Reserved. This field yields undefined result
     *  0x1 (TRANSONE): 1 transaction
     *  0x2 (TRANSTWO):2 transactions to be issued for this endpoint per microframe
     *  0x3 (TRANSTHREE): 3 transactions to be issued for this endpoint per microframe
     */
    uint32_t h_ecn: 1;
    /** h_devaddrn : R/W; bitpos: [29:22]; default: 0;
     *  Device Address .    This field selects the specific device serving as the data
     *  source or sink.
     */
    uint32_t h_devaddrn: 7;
    /** h_oddfrmn : R/W; bitpos: [29]; default: 0;
     *  Odd Frame
     *  1'b0: Even (micro)Frame
     *  1'b1: Odd (micro)Frame
     */
    uint32_t h_oddfrmn: 1;
    /** h_chdisn : R/W; bitpos: [30]; default: 0;
     *  Channel Disable
     *  0x0 : Transmit/Recieve norma
     *  0x1 : Stop transmitting/receiving data on channel
     */
    uint32_t h_chdisn: 1;
    /** h_chenan : R/W; bitpos: [31]; default: 0;
     *  Channel Enable
     *  0x0 :If Scatter/Gather mode is enabled, indicates that the descriptor structure is
     *  not yet ready. If Scatter/Gather mode is disabled, indicates that the channel is
     *  disabled
     *  0x1 : If Scatter/Gather mode is enabled, indicates that the descriptor structure
     *  and data buffer with data is set up and this channel can access the descriptor. If
     *  Scatter/Gather mode is disabled, indicates that the channel is enabled
     */
    uint32_t h_chenan: 1;


} usb_hccharn_reg_t;

/** Type of hcdman register
 *  Host Channel n DMA Address Register
 */
typedef volatile struct {
    /** h_dmaaddrn : R/W; bitpos: [32:0]; default: 0;
     *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
     *  [31:9]: DMA Address
     *  [8:3]: Current Transfer Desc
     *  [2:0]: Reserved
     */
    uint32_t h_dmaaddrn: 32;


} usb_hcdman_reg_t;

/** Type of dcfg register
 *  Device Configuration Register
 */
typedef volatile struct {
    uint32_t reserved_0: 2
    /** nzstsouthshk : R/W; bitpos: [2]; default: 0;
     *  1'b0: Send the received OUT packet to the application (zero-length or non-zero
     *  length) and send a handshake based on NAK and STALL bits for the endpoint in the
     *  Devce Endpoint Control Register
     *  1'b1: Send a STALL handshake on a nonzero-length status OUT transaction and do not
     *  send the received OUT packet to the application
     */
    uint32_t nzstsouthshk: 1;
    uint32_t reserved_3: 1
    /** devaddr : R/W; bitpos: [11:4]; default: 0;
     *  Device Address.
     */
    uint32_t devaddr: 7;
    /** perfrlint : R/W; bitpos: [13:11]; default: 0;
     *  0x0 (EOPF80): 80% of the (micro)Frame interval
     *  0x1 (EOPF85): 85% of the (micro)Frame interval
     *  0x2 (EOPF90): 90% of the (micro)Frame interval
     *  0x3 (EOPF95): 95% of the (micro)Frame interval
     */
    uint32_t perfrlint: 2;
    /** endevoutnak : R/W; bitpos: [13]; default: 0;
     *  1'b0:The core does not set NAK after Bulk OUT transfer complete
     *  1'b1: The core sets NAK after Bulk OUT transfer complete
     */
    uint32_t endevoutnak: 1;
    /** xcvrdly : R/W; bitpos: [14]; default: 0;
     *  0x0 : No delay between xcvr_sel and txvalid during Device chirp
     *  0x1 : Enable delay between xcvr_sel and txvalid during Device chirp
     */
    uint32_t xcvrdly: 1;
    /** erraticintmsk : R/W; bitpos: [15]; default: 0;
     *  0x0 : Early suspend interrupt is generated on erratic error
     *  0x1: Mask early suspend interrupt on erratic error
     */
    uint32_t erraticintmsk: 1;
    uint32_t reserved_16: 2
    /** epmiscnt : R/W; bitpos: [23:18]; default: 4;
     *  IN Endpoint Mismatch Count.
     */
    uint32_t epmiscnt: 5;
    /** descdma : R/W; bitpos: [23]; default: 0;
     *  1'b0: Disable Scatter/Gather DMA
     *  1'b1: Enable Scatter/Gather DMA
     */
    uint32_t descdma: 1;
    /** perschintvl : R/W; bitpos: [26:24]; default: 0;
     *  Periodic Scheduling Interval
     *  0x0 (MF25): 25% of (micro)Frame
     *  0x1 (MF50): 50% of (micro)Frame
     *  0x2 (MF75): 75% of (micro)Frame
     *  0x3 (RESERVED): Reserved
     */
    uint32_t perschintvl: 2;
    /** resvalid : R/W; bitpos: [32:26]; default: 2;
     *  This field is effective only when DCFG.Ena32KHzSusp is set. It controls the resume
     *  period when the core resumes from suspend.
     *  The core counts for ResValid number of clock cycles to detect a valid resume when
     *  this bit is set
     */
    uint32_t resvalid: 6;


} usb_dcfg_reg_t;

/** Type of dctl register
 *  Device Control Register
 */
typedef volatile struct {
    /** rmtwkupsig : R/W; bitpos: [0]; default: 0;
     *  0x0 : Core does not send Remote Wakeup Signaling
     *  0x1 : Core sends Remote Wakeup Signalin
     */
    uint32_t rmtwkupsig: 1;
    /** sftdiscon : R/W; bitpos: [1]; default: 0;
     *  1'b0: Normal operation. When this bit is cleared after a soft disconnect, the core
     *  drives the phy_opmode_o signal on the UTMI+ to 2'b00, which generates a device
     *  connect event to the USB host. When the device is reconnected, the USB host
     *  restarts device enumeration
     *  1'b1: The core drives the phy_opmode_o signal on the UTMI+ to  2'b01, which
     *  generates a device disconnect event to the USB host
     */
    uint32_t sftdiscon: 1;
    /** gnpinnaksts : RO; bitpos: [2]; default: 0;
     *  0x0 : A handshake is sent out based on the data availability in the transmit FIFO
     *  0x1 : A NAK handshake is sent out on all non-periodic  IN endpoints, irrespective
     *  of the data availability in the transmit FIFO
     */
    uint32_t gnpinnaksts: 1;
    /** goutnaksts : RO; bitpos: [3]; default: 0;
     *  0x0 : A handshake is sent based on the FIFO Status and the NAK and STALL bit
     *  settings
     *  0x1 : No data is written to the RxFIFO, irrespective of  space availability. Sends
     *  a NAK handshake on all packets, except on SETUP transactions. All isochronous OUT
     *  packets are dropped
     */
    uint32_t goutnaksts: 1;
    /** tstctl : R/W; bitpos: [7:4]; default: 0;
     *  0x0: Test mode disabled
     *  0x1: Test_J mode
     *  0x2 : Test_K mode
     *  0x3 : Test_SE0_NAK mode
     *  0x4 : Test_Packet mode
     *  0x5 : Test_force_Enable
     */
    uint32_t tstctl: 3;
    /** sgnpinnak : WO; bitpos: [7]; default: 0;
     *  Set Global Non-periodic IN NAK. A write to this field sets the Global Non-periodic
     *  IN NAK.The application uses this bit to send a NAK handshake on all non-periodic IN
     *  endpoints. The core can also Set this bit when a timeout condition is detected on a
     *  non-periodic endpoint in shared FIFO operation. The application must Set this bit
     *  only after making sure that the Global IN NAK Effective bit in the Core Interrupt
     *  Register (GINTSTS.GINNakEff) is cleared
     */
    uint32_t sgnpinnak: 1;
    /** cgnpinnak : WO; bitpos: [8]; default: 0;
     *  Clear Global Non-periodic IN NAK. A write to this field clears the Global
     *  Non-periodic IN NAK.
     */
    uint32_t cgnpinnak: 1;
    /** sgoutnak : WO; bitpos: [9]; default: 0;
     *  Set Global OUT NAK. A write to this field sets the Global OUT NAK. The application
     *  uses this bit to send a NAK handshake on all OUT endpoints. The application must
     *  set the this bit only after making sure that the Global OUT NAK Effective bit in
     *  the Core Interrupt Register (GINTSTS.GOUTNakEff) is cleared.
     */
    uint32_t sgoutnak: 1;
    /** cgoutnak : WO; bitpos: [10]; default: 0;
     *  Clear Global OUT NAK. A write to this field clears the Global OUT NAK.
     */
    uint32_t cgoutnak: 1;
    /** pwronprgdone : R/W; bitpos: [11]; default: 0;
     *  1'b0: Power-On Programming not done
     *  1'b1: Power-On Programming Done
     */
    uint32_t pwronprgdone: 1;
    uint32_t reserved_12: 1
    /** gmc : R/W; bitpos: [15:13]; default: 1;
     *  Global Multi Count. applicable only for Scatter/Gather DMA mode
     *  0x0 : Invalid
     *  0x1 : 1 packet
     *  0x2 : 2 packets
     *  0x3 : 3 packets
     */
    uint32_t gmc: 2;
    /** ignrfrmnum : R/W; bitpos: [15]; default: 0;
     *  0: The core transmits the packets only in the frame number in which they are
     *  intended to be transmitted
     *  1: The core ignores the frame number, sending packets immediately as the packets
     *  are ready
     */
    uint32_t ignrfrmnum: 1;
    /** nakonbble : R/W; bitpos: [16]; default: 0;
     *  1'b0: Disable NAK on Babble Error
     *  1'b1: NAK on Babble Error
     */
    uint32_t nakonbble: 1;
    /** encountonbna : R/W; bitpos: [17]; default: 0;
     *  1'b0: After receiving BNA interrupt,the core disables the endpoint. When the
     *  endpoint is re-enabled by the application,the core starts processing from the
     *  DOEPDMA descriptor
     *  1'b1: After receiving BNA interrupt, the core disables the endpoint. When the
     *  endpoint is re-enabled by the application, the core starts processing from the
     *  descriptor that received the BNA interrupt
     */
    uint32_t encountonbna: 1;
    /** deepsleepbeslreject : R/W; bitpos: [18]; default: 0;
     *  1'b0: Deep Sleep BESL Reject feature is disabled
     *  1'b1: Deep Sleep BESL Reject feature is enabled
     */
    uint32_t deepsleepbeslreject: 1;


} usb_dctl_reg_t;

/** Type of dvbusdis register
 *  Device VBUS Discharge Time Register
 */
typedef volatile struct {
    /** dvbusdis : R/W; bitpos: [16:0]; default: 6103;
     *  Specifies the VBUS discharge time after VBUS pulsing during SRP. This value equals
     *  (VBUS discharge time in PHY clocks) / 1, 024. The value you use depends whether the
     *  PHY is operating at 30MHz (16-bit data width) or 60 MHz (8-bit data width).
     *  Depending on your VBUS load, this value can need adjustment.
     */
    uint32_t dvbusdis: 16;


} usb_dvbusdis_reg_t;

/** Type of dvbuspulse register
 *  Device VBUS Pulsing Time Register
 */
typedef volatile struct {
    /** dvbuspulse : R/W; bitpos: [12:0]; default: 1464;
     *  Specifies the VBUS pulsing time during SRP. This value equals (VBUS pulsing time in
     *  PHY clocks) / 1, 024 The value you use depends whether the PHY is operating at
     *  30MHz (16-bit data width) or 60 MHz (8-bit data width).
     */
    uint32_t dvbuspulse: 12;


} usb_dvbuspulse_reg_t;

/** Type of dthrctl register
 *  Device Threshold Control Register
 */
typedef volatile struct {
    /** nonisothren : R/W; bitpos: [0]; default: 0;
     *  Non-ISO IN Endpoints Threshold Enable
     *  0x0 : No thresholding
     *  0x1 : Enable thresholding for non-isochronous IN endpoints
     */
    uint32_t nonisothren: 1;
    /** isothren : R/W; bitpos: [1]; default: 0;
     *  ISO IN Endpoints Threshold Enable
     *  0x0 : No thresholding
     *  0x1 : Enables thresholding for isochronous IN endpoints
     */
    uint32_t isothren: 1;
    /** txthrlen : R/W; bitpos: [11:2]; default: 8;
     *  This field specifies Transmit thresholding size in DWORDS. This also forms the MAC
     *  threshold and specifies the amount of data in bytes to be in the corresponding
     *  endpoint transmit FIFO, before the core can start transmit on the USB. The
     *  threshold length has to be at least eight DWORDS when the value of AHBThrRatio is
     *  2'h00. In case the AHBThrRatio is non zero the application needs to ensure that the
     *  AHB Threshold value does not go below the recommended eight DWORD. This field
     *  controls both isochronous and non-isochronous IN endpoint thresholds. The
     *  recommended value for ThrLen is to be the same as the programmed AHB Burst Length
     *  (GAHBCFG.HBstLen).
     */
    uint32_t txthrlen: 9;
    /** ahbthrratio : R/W; bitpos: [13:11]; default: 0;
     *  2'b00: AHB threshold = MAC threshold
     *  2'b01: AHB threshold = MAC threshold/2
     *  2'b10: AHB threshold = MAC threshold/4
     *  2'b11: AHB threshold = MAC threshold/8
     */
    uint32_t ahbthrratio: 2;
    uint32_t reserved_13: 3
    /** rxthren : R/W; bitpos: [16]; default: 0;
     *  0x0 : Disable thresholding
     *  0x1 : Enable thresholding in the receive direction
     */
    uint32_t rxthren: 1;
    /** rxthrlen : R/W; bitpos: [26:17]; default: 1;
     *  Receive Threshold Length.  This field specifies Receive thresholding size in
     *  DWORDS. This field also specifies the amount of data received on the USB before the
     *  core can start transmitting on the AHB. The threshold length has to be at least
     *  eight DWORDS. The recommended value for ThrLen is to be the same as the programmed
     *  AHB Burst Length(GAHBCFG.HBstLen).
     */
    uint32_t rxthrlen: 9;
    uint32_t reserved_26: 1
    /** arbprken : R/W; bitpos: [27]; default: 1;
     *  0x0 : Disable DMA arbiter parking
     *  0x1 : Enable DMA arbiter parking for IN endpoints
     */
    uint32_t arbprken: 1;


} usb_dthrctl_reg_t;

/** Type of diepctl0 register
 *  Device Control IN Endpoint $n Control Register
 */
typedef volatile struct {
    /** d_mps0 : R/W; bitpos: [2:0]; default: 0;
     *  Maximum Packet Size
     *  0x0 : 64 bytes
     *  0x1 : 32 bytes
     *  0x2 : 16 bytes
     *  0x3 : 8 bytes
     */
    uint32_t d_mps0: 2;
    uint32_t reserved_2: 13
    /** d_usbactep0 : RO; bitpos: [15]; default: 1;
     *  USB Active Endpoint
     *  0x1 : Control endpoint is always active
     */
    uint32_t d_usbactep0: 1;
    uint32_t reserved_16: 1
    /** d_naksts0 : RO; bitpos: [17]; default: 0;
     *  NAK Status
     *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
     *  0x1 : The core is transmitting NAK handshakes on this endpoint
     */
    uint32_t d_naksts0: 1;
    /** d_eptype0 : RO; bitpos: [20:18]; default: 0;
     *  Endpoint Type
     *  0x0 : Endpoint Control 0
     */
    uint32_t d_eptype0: 2;
    uint32_t reserved_20: 1
    /** d_stall0 : R/W; bitpos: [21]; default: 0;
     *  The application can only set this bit, and the core clears it
     *  0x0 : No Stall
     *  0x1 : Stall Handshake
     */
    uint32_t d_stall0: 1;
    /** d_txfnum0 : R/W; bitpos: [26:22]; default: 0;
     *  TxFIFO Number.
     */
    uint32_t d_txfnum0: 4;
    /** d_cnak0 : WO; bitpos: [26]; default: 0;
     *  A write to this bit clears the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Clear NAK
     */
    uint32_t d_cnak0: 1;
    /** di_snak0 : WO; bitpos: [27]; default: 0;
     *  A write to this bit sets the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Set NAK
     */
    uint32_t di_snak0: 1;
    uint32_t reserved_28: 2
    /** d_epdis0 : R/W; bitpos: [30]; default: 0;
     *  Endpoint Disable
     *  0x0 : No action
     *  0x1 : Disabled Endpoint
     */
    uint32_t d_epdis0: 1;
    /** d_epena0 : R/W; bitpos: [31]; default: 0;
     *  Endpoint Enable
     *  0x0 : No action
     *  0x1 : Enable Endpoint
     */
    uint32_t d_epena0: 1;


} usb_diepctl0_reg_t;

/** Type of dieptsizn register
 *  Device IN Endpoint n Transfer Size Register
 */
typedef volatile struct {
    /** d_xfersizen : R/W; bitpos: [7:0]; default: 0;
     *  Transfer Size
     *  IN Endpoints: The core decrements this field every time a packet from the external
     *  memory is written to the TxFIFO
     *  OUT Endpoints: The core decrements this field every time a  packet is read from the
     *  RxFIFO and written to the external memory
     */
    uint32_t d_xfersizen: 7;
    uint32_t reserved_7: 12
    /** d_pktcntn : R/W; bitpos: [21:19]; default: 0;
     *  Packet Count
     *  IN Endpoints : This field is decremented every time a packet (maximum size or short
     *  packet) is read from the TxFIFO
     *  OUT Endpoints: This field is decremented every time a packet (maximum size or short
     *  packet) is written to the RxFIFO
     */
    uint32_t d_pktcntn: 2;


} usb_dieptsizn_reg_t;

/** Type of diepdman register
 *  Device IN Endpoint n DMA Address Register
 */
typedef volatile struct {
    /** d_dmaaddrn : R/W; bitpos: [32:0]; default: 0;
     *  This field holds the start address of the external memory for storing or fetching
     *  endpoint data.
     */
    uint32_t d_dmaaddrn: 32;


} usb_diepdman_reg_t;

/** Type of diepctl1 register
 *  Device Control IN Endpoint $n Control Register
 */
typedef volatile struct {
    /** d_mps1 : R/W; bitpos: [2:0]; default: 0;
     *  Maximum Packet Size
     *  0x0 : 64 bytes
     *  0x1 : 32 bytes
     *  0x2 : 16 bytes
     *  0x3 : 8 bytes
     */
    uint32_t d_mps1: 2;
    uint32_t reserved_2: 13
    /** d_usbactep1 : RO; bitpos: [15]; default: 1;
     *  USB Active Endpoint
     *  0x1 : Control endpoint is always active
     */
    uint32_t d_usbactep1: 1;
    uint32_t reserved_16: 1
    /** d_naksts1 : RO; bitpos: [17]; default: 0;
     *  NAK Status
     *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
     *  0x1 : The core is transmitting NAK handshakes on this endpoint
     */
    uint32_t d_naksts1: 1;
    /** d_eptype1 : RO; bitpos: [20:18]; default: 0;
     *  Endpoint Type
     *  0x0 : Endpoint Control 0
     */
    uint32_t d_eptype1: 2;
    uint32_t reserved_20: 1
    /** d_stall1 : R/W; bitpos: [21]; default: 0;
     *  The application can only set this bit, and the core clears it
     *  0x0 : No Stall
     *  0x1 : Stall Handshake
     */
    uint32_t d_stall1: 1;
    /** d_txfnum1 : R/W; bitpos: [26:22]; default: 0;
     *  TxFIFO Number.
     */
    uint32_t d_txfnum1: 4;
    /** d_cnak1 : WO; bitpos: [26]; default: 0;
     *  A write to this bit clears the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Clear NAK
     */
    uint32_t d_cnak1: 1;
    /** di_snak1 : WO; bitpos: [27]; default: 0;
     *  A write to this bit sets the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Set NAK
     */
    uint32_t di_snak1: 1;
    /** di_setd0pid1 : WO; bitpos: [28]; default: 0;
     *  Set DATA0 PID
     */
    uint32_t di_setd0pid1: 1;
    /** di_setd1pid1 : WO; bitpos: [29]; default: 0;
     *  Set DATA1 PID
     */
    uint32_t di_setd1pid1: 1;
    /** d_epdis1 : R/W; bitpos: [30]; default: 0;
     *  Endpoint Disable
     *  0x0 : No action
     *  0x1 : Disabled Endpoint
     */
    uint32_t d_epdis1: 1;
    /** d_epena1 : R/W; bitpos: [31]; default: 0;
     *  Endpoint Enable
     *  0x0 : No action
     *  0x1 : Enable Endpoint
     */
    uint32_t d_epena1: 1;


} usb_diepctl1_reg_t;

/** Type of diepctl2 register
 *  Device Control IN Endpoint 2 Control Register
 */
typedef volatile struct {
    /** d_mps2 : R/W; bitpos: [2:0]; default: 0;
     *  Maximum Packet Size
     *  0x0 : 64 bytes
     *  0x1 : 32 bytes
     *  0x2 : 16 bytes
     *  0x3 : 8 bytes
     */
    uint32_t d_mps2: 2;
    uint32_t reserved_2: 13
    /** d_usbactep2 : RO; bitpos: [15]; default: 1;
     *  USB Active Endpoint
     *  0x1 : Control endpoint is always active
     */
    uint32_t d_usbactep2: 1;
    uint32_t reserved_16: 1
    /** d_naksts2 : RO; bitpos: [17]; default: 0;
     *  NAK Status
     *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
     *  0x1 : The core is transmitting NAK handshakes on this endpoint
     */
    uint32_t d_naksts2: 1;
    /** d_eptype2 : RO; bitpos: [20:18]; default: 0;
     *  Endpoint Type
     *  0x0 : Endpoint Control 0
     */
    uint32_t d_eptype2: 2;
    uint32_t reserved_20: 1
    /** d_stall2 : R/W; bitpos: [21]; default: 0;
     *  The application can only set this bit, and the core clears it
     *  0x0 : No Stall
     *  0x1 : Stall Handshake
     */
    uint32_t d_stall2: 1;
    /** d_txfnum2 : R/W; bitpos: [26:22]; default: 0;
     *  TxFIFO Number.
     */
    uint32_t d_txfnum2: 4;
    /** d_cnak2 : WO; bitpos: [26]; default: 0;
     *  A write to this bit clears the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Clear NAK
     */
    uint32_t d_cnak2: 1;
    /** di_snak2 : WO; bitpos: [27]; default: 0;
     *  A write to this bit sets the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Set NAK
     */
    uint32_t di_snak2: 1;
    /** di_setd0pid2 : WO; bitpos: [28]; default: 0;
     *  Set DATA0 PID
     */
    uint32_t di_setd0pid2: 1;
    /** di_setd1pid2 : WO; bitpos: [29]; default: 0;
     *  Set DATA1 PID
     */
    uint32_t di_setd1pid2: 1;
    /** d_epdis2 : R/W; bitpos: [30]; default: 0;
     *  Endpoint Disable
     *  0x0 : No action
     *  0x1 : Disabled Endpoint
     */
    uint32_t d_epdis2: 1;
    /** d_epena2 : R/W; bitpos: [31]; default: 0;
     *  Endpoint Enable
     *  0x0 : No action
     *  0x1 : Enable Endpoint
     */
    uint32_t d_epena2: 1;


} usb_diepctl2_reg_t;

/** Type of diepctl3 register
 *  Device Control IN Endpoint $n Control Register
 */
typedef volatile struct {
    /** di_mps3 : R/W; bitpos: [2:0]; default: 0;
     *  Maximum Packet Size
     *  0x0 : 64 bytes
     *  0x1 : 32 bytes
     *  0x2 : 16 bytes
     *  0x3 : 8 bytes
     */
    uint32_t di_mps3: 2;
    uint32_t reserved_2: 13
    /** di_usbactep3 : RO; bitpos: [15]; default: 1;
     *  USB Active Endpoint
     *  0x1 : Control endpoint is always active
     */
    uint32_t di_usbactep3: 1;
    uint32_t reserved_16: 1
    /** di_naksts3 : RO; bitpos: [17]; default: 0;
     *  NAK Status
     *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
     *  0x1 : The core is transmitting NAK handshakes on this endpoint
     */
    uint32_t di_naksts3: 1;
    /** di_eptype3 : RO; bitpos: [20:18]; default: 0;
     *  Endpoint Type
     *  0x0 : Endpoint Control 0
     */
    uint32_t di_eptype3: 2;
    uint32_t reserved_20: 1
    /** di_stall3 : R/W; bitpos: [21]; default: 0;
     *  The application can only set this bit, and the core clears it
     *  0x0 : No Stall
     *  0x1 : Stall Handshake
     */
    uint32_t di_stall3: 1;
    /** di_txfnum3 : R/W; bitpos: [26:22]; default: 0;
     *  TxFIFO Number.
     */
    uint32_t di_txfnum3: 4;
    /** di_cnak3 : WO; bitpos: [26]; default: 0;
     *  A write to this bit clears the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Clear NAK
     */
    uint32_t di_cnak3: 1;
    /** di_snak3 : WO; bitpos: [27]; default: 0;
     *  A write to this bit sets the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Set NAK
     */
    uint32_t di_snak3: 1;
    /** di_setd0pid3 : WO; bitpos: [28]; default: 0;
     *  Set DATA0 PID
     */
    uint32_t di_setd0pid3: 1;
    /** di_setd1pid3 : WO; bitpos: [29]; default: 0;
     *  Set DATA1 PID
     */
    uint32_t di_setd1pid3: 1;
    /** di_epdis3 : R/W; bitpos: [30]; default: 0;
     *  Endpoint Disable
     *  0x0 : No action
     *  0x1 : Disabled Endpoint
     */
    uint32_t di_epdis3: 1;
    /** di_epena3 : R/W; bitpos: [31]; default: 0;
     *  Endpoint Enable
     *  0x0 : No action
     *  0x1 : Enable Endpoint
     */
    uint32_t di_epena3: 1;


} usb_diepctl3_reg_t;

/** Type of diepctl4 register
 *  Device Control IN Endpoint $n Control Register
 */
typedef volatile struct {
    /** d_mps4 : R/W; bitpos: [2:0]; default: 0;
     *  Maximum Packet Size
     *  0x0 : 64 bytes
     *  0x1 : 32 bytes
     *  0x2 : 16 bytes
     *  0x3 : 8 bytes
     */
    uint32_t d_mps4: 2;
    uint32_t reserved_2: 13
    /** d_usbactep4 : RO; bitpos: [15]; default: 1;
     *  USB Active Endpoint
     *  0x1 : Control endpoint is always active
     */
    uint32_t d_usbactep4: 1;
    uint32_t reserved_16: 1
    /** d_naksts4 : RO; bitpos: [17]; default: 0;
     *  NAK Status
     *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
     *  0x1 : The core is transmitting NAK handshakes on this endpoint
     */
    uint32_t d_naksts4: 1;
    /** d_eptype4 : RO; bitpos: [20:18]; default: 0;
     *  Endpoint Type
     *  0x0 : Endpoint Control 0
     */
    uint32_t d_eptype4: 2;
    uint32_t reserved_20: 1
    /** d_stall4 : R/W; bitpos: [21]; default: 0;
     *  The application can only set this bit, and the core clears it
     *  0x0 : No Stall
     *  0x1 : Stall Handshake
     */
    uint32_t d_stall4: 1;
    /** d_txfnum4 : R/W; bitpos: [26:22]; default: 0;
     *  TxFIFO Number.
     */
    uint32_t d_txfnum4: 4;
    /** d_cnak4 : WO; bitpos: [26]; default: 0;
     *  A write to this bit clears the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Clear NAK
     */
    uint32_t d_cnak4: 1;
    /** di_snak4 : WO; bitpos: [27]; default: 0;
     *  A write to this bit sets the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Set NAK
     */
    uint32_t di_snak4: 1;
    /** di_setd0pid4 : WO; bitpos: [28]; default: 0;
     *  Set DATA0 PID
     */
    uint32_t di_setd0pid4: 1;
    /** di_setd1pid4 : WO; bitpos: [29]; default: 0;
     *  Set DATA1 PID
     */
    uint32_t di_setd1pid4: 1;
    /** d_epdis4 : R/W; bitpos: [30]; default: 0;
     *  Endpoint Disable
     *  0x0 : No action
     *  0x1 : Disabled Endpoint
     */
    uint32_t d_epdis4: 1;
    /** d_epena4 : R/W; bitpos: [31]; default: 0;
     *  Endpoint Enable
     *  0x0 : No action
     *  0x1 : Enable Endpoint
     */
    uint32_t d_epena4: 1;


} usb_diepctl4_reg_t;

/** Type of diepctl5 register
 *  Device Control IN Endpoint $n Control Register
 */
typedef volatile struct {
    /** di_mps5 : R/W; bitpos: [2:0]; default: 0;
     *  Maximum Packet Size
     *  0x0 : 64 bytes
     *  0x1 : 32 bytes
     *  0x2 : 16 bytes
     *  0x3 : 8 bytes
     */
    uint32_t di_mps5: 2;
    uint32_t reserved_2: 13
    /** di_usbactep5 : RO; bitpos: [15]; default: 1;
     *  USB Active Endpoint
     *  0x1 : Control endpoint is always active
     */
    uint32_t di_usbactep5: 1;
    uint32_t reserved_16: 1
    /** di_naksts5 : RO; bitpos: [17]; default: 0;
     *  NAK Status
     *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
     *  0x1 : The core is transmitting NAK handshakes on this endpoint
     */
    uint32_t di_naksts5: 1;
    /** di_eptype5 : RO; bitpos: [20:18]; default: 0;
     *  Endpoint Type
     *  0x0 : Endpoint Control 0
     */
    uint32_t di_eptype5: 2;
    uint32_t reserved_20: 1
    /** di_stall5 : R/W; bitpos: [21]; default: 0;
     *  The application can only set this bit, and the core clears it
     *  0x0 : No Stall
     *  0x1 : Stall Handshake
     */
    uint32_t di_stall5: 1;
    /** di_txfnum5 : R/W; bitpos: [26:22]; default: 0;
     *  TxFIFO Number.
     */
    uint32_t di_txfnum5: 4;
    /** di_cnak5 : WO; bitpos: [26]; default: 0;
     *  A write to this bit clears the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Clear NAK
     */
    uint32_t di_cnak5: 1;
    /** di_snak5 : WO; bitpos: [27]; default: 0;
     *  A write to this bit sets the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Set NAK
     */
    uint32_t di_snak5: 1;
    /** di_setd0pid5 : WO; bitpos: [28]; default: 0;
     *  Set DATA0 PID
     */
    uint32_t di_setd0pid5: 1;
    /** di_setd1pid5 : WO; bitpos: [29]; default: 0;
     *  Set DATA1 PID
     */
    uint32_t di_setd1pid5: 1;
    /** di_epdis5 : R/W; bitpos: [30]; default: 0;
     *  Endpoint Disable
     *  0x0 : No action
     *  0x1 : Disabled Endpoint
     */
    uint32_t di_epdis5: 1;
    /** di_epena5 : R/W; bitpos: [31]; default: 0;
     *  Endpoint Enable
     *  0x0 : No action
     *  0x1 : Enable Endpoint
     */
    uint32_t di_epena5: 1;


} usb_diepctl5_reg_t;

/** Type of diepctl6 register
 *  Device Control IN Endpoint $n Control Register
 */
typedef volatile struct {
    /** d_mps6 : R/W; bitpos: [2:0]; default: 0;
     *  Maximum Packet Size
     *  0x0 : 64 bytes
     *  0x1 : 32 bytes
     *  0x2 : 16 bytes
     *  0x3 : 8 bytes
     */
    uint32_t d_mps6: 2;
    uint32_t reserved_2: 13
    /** d_usbactep6 : RO; bitpos: [15]; default: 1;
     *  USB Active Endpoint
     *  0x1 : Control endpoint is always active
     */
    uint32_t d_usbactep6: 1;
    uint32_t reserved_16: 1
    /** d_naksts6 : RO; bitpos: [17]; default: 0;
     *  NAK Status
     *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
     *  0x1 : The core is transmitting NAK handshakes on this endpoint
     */
    uint32_t d_naksts6: 1;
    /** d_eptype6 : RO; bitpos: [20:18]; default: 0;
     *  Endpoint Type
     *  0x0 : Endpoint Control 0
     */
    uint32_t d_eptype6: 2;
    uint32_t reserved_20: 1
    /** d_stall6 : R/W; bitpos: [21]; default: 0;
     *  The application can only set this bit, and the core clears it
     *  0x0 : No Stall
     *  0x1 : Stall Handshake
     */
    uint32_t d_stall6: 1;
    /** d_txfnum6 : R/W; bitpos: [26:22]; default: 0;
     *  TxFIFO Number.
     */
    uint32_t d_txfnum6: 4;
    /** d_cnak6 : WO; bitpos: [26]; default: 0;
     *  A write to this bit clears the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Clear NAK
     */
    uint32_t d_cnak6: 1;
    /** di_snak6 : WO; bitpos: [27]; default: 0;
     *  A write to this bit sets the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Set NAK
     */
    uint32_t di_snak6: 1;
    /** di_setd0pid6 : WO; bitpos: [28]; default: 0;
     *  Set DATA0 PID
     */
    uint32_t di_setd0pid6: 1;
    /** di_setd1pid6 : WO; bitpos: [29]; default: 0;
     *  Set DATA1 PID
     */
    uint32_t di_setd1pid6: 1;
    /** d_epdis6 : R/W; bitpos: [30]; default: 0;
     *  Endpoint Disable
     *  0x0 : No action
     *  0x1 : Disabled Endpoint
     */
    uint32_t d_epdis6: 1;
    /** d_epena6 : R/W; bitpos: [31]; default: 0;
     *  Endpoint Enable
     *  0x0 : No action
     *  0x1 : Enable Endpoint
     */
    uint32_t d_epena6: 1;


} usb_diepctl6_reg_t;

/** Type of doepctl0 register
 *  Device Control OUT Endpoint $n Control Register
 */
typedef volatile struct {
    /** mps0 : RO; bitpos: [2:0]; default: 0;
     *  Maximum Packet Size
     *  0x0 : 64 bytes
     *  0x1 : 32 bytes
     *  0x2 : 16 bytes
     *  0x3 : 8 bytes
     */
    uint32_t mps0: 2;
    uint32_t reserved_2: 13
    /** usbactep0 : RO; bitpos: [15]; default: 1;
     *  0x1: USB Active Endpoint 0
     */
    uint32_t usbactep0: 1;
    uint32_t reserved_16: 1
    /** naksts0 : RO; bitpos: [17]; default: 0;
     *  0x0 :  The core is transmitting non-NAK handshakes based on the FIFO status
     *  0x1 :The core is transmitting NAK handshakes on this endpoint
     */
    uint32_t naksts0: 1;
    /** eptype0 : RO; bitpos: [20:18]; default: 0;
     *  Endpoint Type
     *  0x0 : Endpoint Control $n
     */
    uint32_t eptype0: 2;
    /** snp0 : R/W; bitpos: [20]; default: 0;
     *  0x0 : Reserved 0
     *  0x1 : Reserved 1
     */
    uint32_t snp0: 1;
    /** stall0 : R/W; bitpos: [21]; default: 0;
     *  The application can only set this bit, and the core clears it, when a SETUP token
     *  is received for this endpoint
     *  0x0 (INACTIVE): No Stall
     *  0x1 (ACTIVE): Stall Handshake
     */
    uint32_t stall0: 1;
    uint32_t reserved_22: 4
    /** cnak0 : WO; bitpos: [26]; default: 0;
     *  0x0 : No action
     *  0x1 : Clear NAK
     */
    uint32_t cnak0: 1;
    /** do_snak0 : WO; bitpos: [27]; default: 0;
     *  0x0 : No action
     *  0x1 : Set NAK
     */
    uint32_t do_snak0: 1;
    uint32_t reserved_28: 2
    /** epdis0 : RO; bitpos: [30]; default: 0;
     *  Endpoint Disable
     *  0x0 : No Endpoint disable
     */
    uint32_t epdis0: 1;
    /** epena0 : R/W; bitpos: [31]; default: 0;
     *  Endpoint Enable
     *  0x0 : No action
     *  0x1 : Enable Endpoint
     */
    uint32_t epena0: 1;


} usb_doepctl0_reg_t;

/** Type of doeptsizn register
 *  Device OUT Endpoint n Transfer Size Register
 */
typedef volatile struct {
    /** xfersizen : R/W; bitpos: [7:0]; default: 0;
     *  Transfer Size.Indicates the transfer size in bytes for ENDPOINTn
     */
    uint32_t xfersizen: 7;
    uint32_t reserved_7: 12
    /** pktcntn : R/W; bitpos: [19]; default: 0;
     *  Packet Count (PktCnt).This field is decremented to zero after a packet is written
     *  into the RxFIFO.
     */
    uint32_t pktcntn: 1;
    uint32_t reserved_20: 9
    /** supcntn : R/W; bitpos: [31:29]; default: 0;
     *  SETUP Packet Count (SUPCnt).This field specifies the number of back-to-back SETUP
     *  data packets the endpoint can receive
     *  2'b01: 1 packet
     *  2'b10: 2 packets
     *  2'b11: 3 packets
     */
    uint32_t supcntn: 2;


} usb_doeptsizn_reg_t;

/** Type of doepdman register
 *  Device OUT Endpoint n DMA Address Register
 */
typedef volatile struct {
    /** dmaaddrn : R/W; bitpos: [32:0]; default: 0;
     *  Holds the start address of the external memory for storing or fetching endpoint
     *  data.
     */
    uint32_t dmaaddrn: 32;


} usb_doepdman_reg_t;

/** Type of doepdmabn register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
typedef volatile struct {
    /** dmabufferaddrn : R/W; bitpos: [32:0]; default: 0;
     *  Holds the current buffer address.This register is updated as and when the data
     *  transfer for the corresponding end point is in progress. This register is present
     *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
     */
    uint32_t dmabufferaddrn: 32;


} usb_doepdmabn_reg_t;

/** Type of doepctln register
 *  Device Control OUT Endpoint n Control Register
 */
typedef volatile struct {
    /** mpsn : RO; bitpos: [11:0]; default: 0;
     *  Maximum Packet Size in bytes
     */
    uint32_t mpsn: 11;
    uint32_t reserved_11: 4
    /** usbactepn : RO; bitpos: [15]; default: 1;
     *  0x1: USB Active Endpoint 0
     */
    uint32_t usbactepn: 1;
    uint32_t reserved_16: 1
    /** nakstsn : RO; bitpos: [17]; default: 0;
     *  0x0 :  The core is transmitting non-NAK handshakes based on the FIFO status
     *  0x1 :The core is transmitting NAK handshakes on this endpoint
     */
    uint32_t nakstsn: 1;
    /** eptypen : RO; bitpos: [20:18]; default: 0;
     *  Endpoint Type
     *  0x0 : Endpoint Control n
     */
    uint32_t eptypen: 2;
    /** snpn : R/W; bitpos: [20]; default: 0;
     *  0x0 : Reserved 0
     *  0x1 : Reserved 1
     */
    uint32_t snpn: 1;
    /** stalln : R/W; bitpos: [21]; default: 0;
     *  The application can only set this bit, and the core clears it, when a SETUP token
     *  is received for this endpoint
     *  0x0 (INACTIVE): No Stall
     *  0x1 (ACTIVE): Stall Handshake
     */
    uint32_t stalln: 1;
    uint32_t reserved_22: 4
    /** cnakn : WO; bitpos: [26]; default: 0;
     *  0x0 : No action
     *  0x1 : Clear NAK
     */
    uint32_t cnakn: 1;
    /** do_snakn : WO; bitpos: [27]; default: 0;
     *  A write to this bit sets the NAK bit for the endpoint
     *  0x0 : No action
     *  0x1 : Set NAK
     */
    uint32_t do_snakn: 1;
    /** do_setd0pidn : WO; bitpos: [28]; default: 0;
     *  Set DATA0 PID
     */
    uint32_t do_setd0pidn: 1;
    /** do_setd1pidn : WO; bitpos: [29]; default: 0;
     *  Set DATA1 PID
     */
    uint32_t do_setd1pidn: 1;
    /** epdisn : RO; bitpos: [30]; default: 0;
     *  Endpoint Disable
     *  0x0 : No Endpoint disable
     */
    uint32_t epdisn: 1;
    /** epenan : R/W; bitpos: [31]; default: 0;
     *  Endpoint Enable
     *  0x0 : No action
     *  0x1 : Enable Endpoint
     */
    uint32_t epenan: 1;


} usb_doepctln_reg_t;

/** Type of pcgcctl register
 *  Power and Clock Gating Control Register
 */
typedef volatile struct {
    /** stoppclk : R/W; bitpos: [0]; default: 0;
     *  0x0 : Disable Stop Pclk
     *  0x1 : Enable Stop Pclk
     */
    uint32_t stoppclk: 1;
    /** gatehclk : R/W; bitpos: [1]; default: 0;
     *  gate hclk
     *  0x0:clears this bit when USB is resumed or a new session starts
     *  0x1:set this bit to gate hclk to modules, when the USB is suspended or the session
     *  is not valid
     */
    uint32_t gatehclk: 1;
    /** pwrclmp : R/W; bitpos: [2]; default: 0;
     *  0x0:Clears this bit to disable the clamping before the power is turned on
     *  0x1:In only Partial Power-Down mode, sets this bit to clamp the signals between the
     *  power-on modules and the power-off modules before the power is turned off
     */
    uint32_t pwrclmp: 1;
    /** rstpdwnmodule : R/W; bitpos: [3]; default: 0;
     *  Reset Power-Down Modules.
     *  0x0 : Power is turned on
     *  0x1 : Power is turned off
     */
    uint32_t rstpdwnmodule: 1;
    uint32_t reserved_4: 2
    /** physleep : RO; bitpos: [6]; default: 0;
     *  0x0 : Phy not in Sleep state
     *  0x1 : Phy in Sleep state
     */
    uint32_t physleep: 1;
    /** l1suspended : RO; bitpos: [7]; default: 0;
     *  L1 Deep Sleep
     *  0x0 : Non Deep Sleep
     *  0x1 : Deep Sleep
     */
    uint32_t l1suspended: 1;
    /** resetaftersusp : R/W; bitpos: [8]; default: 0;
     *  Reset after suspend
     *  0x0 : In Host-only mode, host issues Resume after Suspend
     *  0x1 : In Host-only mode, host sets this bit before clamp is removed if the host
     *  needs to issue Reset after Suspend
     */
    uint32_t resetaftersusp: 1;


} usb_pcgcctl_reg_t;



/** Interrupt registers */
/** Type of gotgint register
 *  OTG Interrupt Register
 */
typedef volatile struct {
    uint32_t reserved_0: 2
    /** sesenddet : R/W1C; bitpos: [2]; default: 0;
     *  Session End Detected.The controller sets this bit when the utmiotg_bvalid signal is
     *  deasserted. This bit can be set only by the core and the application should write 1
     *  to clear it
     */
    uint32_t sesenddet: 1;
    uint32_t reserved_3: 5
    /** sesreqsucstschng : R/W1C; bitpos: [8]; default: 0;
     *  Session Request Success Status Change.The core sets this bit on the success or
     *  failure of a session request.The application must read the Session Request Success
     *  bit in the OTG Control and Status register (GOTGCTL_REG.USB_SESREQSCS) to check for
     *  success or failure. This bit can be set only by the core and the application should
     *  write 1 to clear it.
     */
    uint32_t sesreqsucstschng: 1;
    /** hstnegsucstschng : R/W1C; bitpos: [9]; default: 0;
     *  Host Negotiation Success Status Change. The core sets this bit on the success or
     *  failure of a USB host negotiation request. The application must read the Host
     *  Negotiation Success bit of the OTG Control and Status register
     *  (GOTGCTL_REG.USB_HSTNEGSCS) to check for success or failure. This bit can be set
     *  only by the core and the application should write 1 to clear it
     */
    uint32_t hstnegsucstschng: 1;
    uint32_t reserved_10: 7
    /** hstnegdet : R/W1C; bitpos: [17]; default: 0;
     *  Host Negotiation Detected.The core sets this bit when it detects a host negotiation
     *  request on the USB. This bit can be set only by the core and the application should
     *  write 1 to clear it.
     */
    uint32_t hstnegdet: 1;
    /** adevtoutchg : R/W1C; bitpos: [18]; default: 0;
     *  A-Device Timeout Change. The core sets this bit to indicate that the A-device has
     *  timed out while waiting for the B-device to connect.This bit can be set only by the
     *  core and the application should write 1 to clear it
     */
    uint32_t adevtoutchg: 1;
    /** dbncedone : R/W1C; bitpos: [19]; default: 0;
     *  Debounce Done. The core sets this bit when the debounce is completed after the
     *  device connect. The application can start driving USB reset after seeing this
     *  interrupt. This bit is only valid when the HNP Capable or SRP Capable bit is SET in
     *  the Core USB Configuration register (GUSBCFG.HNPCap or GUSBCFG.SRPCap,
     *  respectively). This bit can be set only by the core and the application should
     *  write 1 to clear it
     */
    uint32_t dbncedone: 1;


} usb_gotgint_reg_t;

/** Type of gintsts register
 *  Interrupt Register
 */
typedef volatile struct {
    /** curmod_int : RO; bitpos: [0]; default: 0;
     *  Current Mode of Operation
     *  1'b0: Device mode
     *  1'b1: Host mode
     */
    uint32_t curmod_int: 1;
    /** modemis : R/W1C; bitpos: [1]; default: 0;
     *  Mode Mismatch Interrupt.The core sets this bit when the application is trying to
     *  access:A Host mode register, when the controller is operating in Device mode
     */
    uint32_t modemis: 1;
    /** otgint : RO; bitpos: [2]; default: 0;
     *  OTG Interrupt.The controller sets this bit to indicate an OTG protocol event. The
     *  application must read the OTG Interrupt Status (GOTGINT_REG) register to determine
     *  the exact event that caused this interrupt. The application must clear the
     *  appropriate status bit in the GOTGINT_REG register to clear this bit.
     */
    uint32_t otgint: 1;
    /** sof : R/W1C; bitpos: [3]; default: 0;
     *  Start of (micro)Frame (Sof)
     *  In Host mode, the core sets this bit to indicate that an SOF (FS), micro-SOF (HS),
     *  or Keep-Alive (LS) is transmitted on the USB. The application must write a 1 to
     *  this bit to clear the interrupt
     *  In Device mode, the controller sets this bit to indicate that an SOF token has been
     *  received on the USB. The application can read the Device Status register to get the
     *  current (micro)Frame number. This Interrupt is seen only when the core is operating
     *  at either HS or FS. This bit can be set only by the core and the application must
     *  write 1 to clear it
     */
    uint32_t sof: 1;
    /** rxflvi : RO; bitpos: [4]; default: 0;
     *  RxFIFO Non-Empty.Indicates that there is at least one packet pending to be read
     *  from the RxFIFO
     *  1'b0: Rx Fifo is empty
     *  1'b1: Rx Fifo is not empty
     */
    uint32_t rxflvi: 1;
    /** nptxfemp : RO; bitpos: [5]; default: 0;
     *  Non-periodic TxFIFO Empty.This interrupt is asserted when the Non-periodic TxFIFO
     *  is either half or completely empty, and there is space for at least one Entry to be
     *  written to the Non-periodic Transmit Request Queue. The half or completely empty
     *  status is determined by the Non-periodic TxFIFO Empty Level bit in the Core AHB
     *  Configuration register (GAHBCFG_REG.USB_NPTXFEMPLVL).
     */
    uint32_t nptxfemp: 1;
    /** ginnakeff : RO; bitpos: [6]; default: 0;
     *  Device only.Global IN Non-periodic NAK Effective.Indicates that the Set Global
     *  Non-periodic IN NAK bit in the Device Control register (DCTL.SGNPInNak) set by the
     *  application, has taken effect in the core. That is, the core has sampled the Global
     *  IN NAK bit Set by the application. This bit can be cleared by clearing the Clear
     *  Global Non-periodic IN NAK bit in the Device Control register (DCTL.CGNPInNak).
     *  This interrupt does not necessarily mean that a NAK handshake is sent out on the
     *  USB. The STALL bit takes precedence over the NAK bit.
     */
    uint32_t ginnakeff: 1;
    /** goutnakeff : RO; bitpos: [7]; default: 0;
     *  Device only.Global OUT NAK Effective.Indicates that the Set Global OUT NAK bit in
     *  the Device Control register (DCTL_REG.USB_SGOUTNAK), Set by the application, has
     *  taken effect in the core. This bit can be cleared by writing the Clear Global OUT
     *  NAK bit in the Device Control register (DCTL_REG.REG_CGOUTNAK).
     */
    uint32_t goutnakeff: 1;
    uint32_t reserved_8: 2
    /** erlysusp : R/W1C; bitpos: [10]; default: 0;
     *  Device only.Early Suspend.The controller sets this bit to indicate that an Idle
     *  state has been detected on the USB for 3 ms.
     */
    uint32_t erlysusp: 1;
    /** usbsusp : R/W1C; bitpos: [11]; default: 0;
     *  Device only.USB Suspend.The controller sets this bit to indicate that a suspend was
     *  detected on the USB. The controller enters the Suspended state when there is no
     *  activity on the linestate signal for an extended period of time.
     */
    uint32_t usbsusp: 1;
    /** usbrst : R/W1C; bitpos: [12]; default: 0;
     *  Device only.USB Reset.The controller sets this bit to indicate that a reset is
     *  detected on the USB
     */
    uint32_t usbrst: 1;
    /** enumdone : R/W1C; bitpos: [13]; default: 0;
     *  Device only.Enumeration Done.The core sets this bit to indicate that speed
     *  enumeration is complete. The application must read the Device Status (DSTS_REG)
     *  register to obtain the enumerated speed.
     */
    uint32_t enumdone: 1;
    /** isooutdrop : R/W1C; bitpos: [14]; default: 0;
     *  Device only.Isochronous OUT Packet Dropped Interrupt.The controller sets this bit
     *  when it fails to write an isochronous OUT packet into the RxFIFO because the RxFIFO
     *  does not have enough space to accommodate a maximum packet size packet for the
     *  isochronous OUT endpoint.
     */
    uint32_t isooutdrop: 1;
    /** eopf : R/W1C; bitpos: [15]; default: 0;
     *  Device only.End of Periodic Frame Interrupt.Indicates that the period specified in
     *  the Periodic Frame Interval field of the Device Configuration register
     *  (DCFG_REG.REG_PERFRINT) has been reached in the current microframe.
     */
    uint32_t eopf: 1;
    uint32_t reserved_16: 1
    /** epmis : R/W1C; bitpos: [17]; default: 0;
     *  Device only.Endpoint Mismatch Interrupt.This interrupt is valid only in shared FIFO
     *  operation.Indicates that an IN token has been received for a non-periodic endpoint,
     *  but the data for another endpoint is present in the top of the Non-periodic
     *  Transmit FIFO and the IN endpoint mismatch count programmed by the application has
     *  expired.
     */
    uint32_t epmis: 1;
    /** iepint : RO; bitpos: [18]; default: 0;
     *  Device only.IN Endpoints Interrupt.The core sets this bit to indicate that an
     *  interrupt is pending on one of the IN endpoints of the core (in Device mode). The
     *  application must read the Device All Endpoints Interrupt (DAINT) register to
     *  determine the exact number of the IN endpoint on Device IN Endpoint-n Interrupt
     *  (DIEPINTn) register to determine the exact cause of the interrupt. The application
     *  must clear the appropriate status bit in the corresponding DIEPINTn register to
     *  clear this bit.
     */
    uint32_t iepint: 1;
    /** oepint : RO; bitpos: [19]; default: 0;
     *  Device only.OUT Endpoints Interrupt.The controller sets this bit to indicate that
     *  an interrupt is pending on one of the OUT endpoints of the core (in Device mode).
     *  The application must read the Device All Endpoints Interrupt (DAINT) register to
     *  determine the exact number of the OUT endpoint on which the interrupt occurred, and
     *  then read the corresponding Device OUT Endpoint-n Interrupt (DOEPINTn) register to
     *  determine the exact cause of the interrupt. The application must clear the
     *  appropriate status bit in the corresponding DOEPINTn register to clear this bit.
     */
    uint32_t oepint: 1;
    /** incompisoin : R/W1C; bitpos: [20]; default: 0;
     *  Device only.Incomplete Isochronous IN Transfer.The core sets this interrupt to
     *  indicate that there is at least one isochronous IN endpoint on which the transfer
     *  is not completed in the current microframe. This interrupt is asserted along with
     *  the End of Periodic Frame Interrupt (EOPF) bit in this register.
     */
    uint32_t incompisoin: 1;
    /** incompip : R/W1C; bitpos: [21]; default: 0;
     *  In Host mode, the core sets this interrupt bit when there are incomplete periodic
     *  transactions still pending which are scheduled for the current microframe.The
     *  Device mode, the core sets this interrupt to indicate that thereis at least one
     *  isochronous OUT endpoint on which the transfer is not completed in the current
     *  microframe. This interrupt is asserted along with the End of Periodic Frame
     *  Interrupt (EOPF) bit in this register.
     */
    uint32_t incompip: 1;
    /** fetsusp : R/W1C; bitpos: [22]; default: 0;
     *  Device only.Data Fetch Suspended.This interrupt is valid only in DMA mode. This
     *  interrupt indicates that the core has stopped fetching data. For IN endpoints due
     *  to the unavailability of TxFIFO space or Request Queue space. This interrupt is
     *  used by the application for an endpoint mismatch algorithm
     */
    uint32_t fetsusp: 1;
    /** resetdet : R/W1C; bitpos: [23]; default: 0;
     *  Reset detected Interrupt
     *  In Device mode, this interrupt is asserted when a reset is detected on the USB in
     *  partial power-down mode when the device is in Suspend
     *  In Host mode, this interrupt is not asserted
     */
    uint32_t resetdet: 1;
    /** prtlnt : RO; bitpos: [24]; default: 0;
     *  Host only.Host Port Interrupt.The core sets this bit to indicate a change in port
     *  status of one of the controller ports in Host mode. The application must read the
     *  Host Port Control and Status (HPRT) register to determine the exact event that
     *  caused this interrupt. The application must clear the appropriate status bit in the
     *  Host Port Control and Status register to clear this bit.
     */
    uint32_t prtlnt: 1;
    /** hchlnt : RO; bitpos: [25]; default: 0;
     *  Host only.Host Channels Interrupt.The core sets this bit to indicate that an
     *  interrupt is pending on one of the channels of the core (in Host mode). The
     *  application must read the Host All Channels Interrupt (HAINT) register to determine
     *  the exact number of the channel on which the interrupt occurred,and Then read the
     *  corresponding Host Channel-n Interrupt (HCINTn) register to determine the exact
     *  cause of the interrupt.The application must clear the appropriate status bit in the
     *  HCINTn register to clear this bit.
     */
    uint32_t hchlnt: 1;
    /** ptxfemp : RO; bitpos: [26]; default: 0;
     *  Host only.Periodic TxFIFO Empty.This interrupt is asserted when the Periodic
     *  Transmit FIFO is either half or completely empty and there is space for at least
     *  one entry to be written in the Periodic Request Queue. The half or completelyempty
     *  status is determined by the Periodic TxFIFO Empty Level bit in the Core AHB
     *  Configuration register (GAHBCFG.PTxFEmpLvl)
     */
    uint32_t ptxfemp: 1;
    uint32_t reserved_27: 1
    /** conidstschng : R/W1C; bitpos: [28]; default: 0;
     *  Connector ID Status Change
     *  1'b0:Not active
     *  1'b1:Connector ID Status Change
     */
    uint32_t conidstschng: 1;
    /** disconnint : R/W1C; bitpos: [29]; default: 0;
     *  Disconnect Detected Interrupt
     *  1'b0:Not active
     *  1'b1:Disconnect Detected Interrupt
     */
    uint32_t disconnint: 1;
    /** sessreqint : R/W1C; bitpos: [30]; default: 0;
     *  1'b0:Not active
     *  1'b1:Session Request New Session Detected
     */
    uint32_t sessreqint: 1;
    /** wkupint : R/W1C; bitpos: [31]; default: 0;
     *  Resume/Remote Wakeup Detected Interrupt
     *  1'b0:Not active
     *  1'b1:Resume or Remote Wakeup Detected Interrupt
     */
    uint32_t wkupint: 1;


} usb_gintsts_reg_t;

/** Type of gintmsk register
 *  Interrupt Mask Register
 */
typedef volatile struct {
    uint32_t reserved_0: 1
    /** modemismsk : R/W; bitpos: [1]; default: 0;
     *  1'b0:Mode Mismatch Interrupt Mask
     *  1'b1:No Mode Mismatch Interrupt Mask
     */
    uint32_t modemismsk: 1;
    /** otgintmsk : R/W; bitpos: [2]; default: 0;
     *  1'b0:OTG Interrupt Mask
     *  1'b1:No OTG Interrupt Mask
     */
    uint32_t otgintmsk: 1;
    /** sofmsk : R/W; bitpos: [3]; default: 0;
     *  1'b0:Start of (micro)Frame Mask
     *  1'b1:No Start of (micro)Frame Mask
     */
    uint32_t sofmsk: 1;
    /** rxflvimsk : R/W; bitpos: [4]; default: 0;
     *  1'b0:Receive FIFO Non-Empty Mask
     *  1'b1:No Receive FIFO Non-Empty Mask
     */
    uint32_t rxflvimsk: 1;
    /** nptxfempmsk : R/W; bitpos: [5]; default: 0;
     *  1'b0:Non-periodic TxFIFO Empty Mask
     *  1'b1:No Non-periodic TxFIFO Empty Mask
     */
    uint32_t nptxfempmsk: 1;
    /** ginnakeffmsk : R/W; bitpos: [6]; default: 0;
     *  1'b0:Global Non-periodic IN NAK Effective Mask
     *  1'b1:No Global Non-periodic IN NAK Effective Mask
     */
    uint32_t ginnakeffmsk: 1;
    /** goutnackeffmsk : R/W; bitpos: [7]; default: 0;
     *  1'b0:Global OUT NAK Effective Mask
     *  1'b1:No Global OUT NAK Effective Mask
     */
    uint32_t goutnackeffmsk: 1;
    uint32_t reserved_8: 2
    /** erlysuspmsk : R/W; bitpos: [10]; default: 0;
     *  1'b0:Early Suspend Mask
     *  1'b1:No Early Suspend Mask
     */
    uint32_t erlysuspmsk: 1;
    /** usbsuspmsk : R/W; bitpos: [11]; default: 0;
     *  1'b0:USB Suspend Mask
     *  1'b1:No USB Suspend Mask
     */
    uint32_t usbsuspmsk: 1;
    /** usbrstmsk : R/W; bitpos: [12]; default: 0;
     *  1'b0:USB Reset Mask
     *  1'b1:No USB Reset Mask
     */
    uint32_t usbrstmsk: 1;
    /** enumdonemsk : R/W; bitpos: [13]; default: 0;
     *  1'b0: Enumeration Done Mask
     *  1'b1: No Enumeration Done Mask
     */
    uint32_t enumdonemsk: 1;
    /** isooutdropmsk : R/W; bitpos: [14]; default: 0;
     *  1'b0: Isochronous OUT Packet Dropped Interrupt Mask
     *  1'b1: No Isochronous OUT Packet Dropped Interrupt Mask
     */
    uint32_t isooutdropmsk: 1;
    /** eopfmsk : R/W; bitpos: [15]; default: 0;
     *  1'b0: End of Periodic Frame Interrupt Mask
     *  1'b1: No End of Periodic Frame Interrupt Mask
     */
    uint32_t eopfmsk: 1;
    uint32_t reserved_16: 1
    /** epmismsk : R/W; bitpos: [17]; default: 0;
     *  1'b0: Endpoint Mismatch Interrupt Mask
     *  1'b1: No Endpoint Mismatch Interrupt Mask
     */
    uint32_t epmismsk: 1;
    /** iepintmsk : R/W; bitpos: [18]; default: 0;
     *  1'b0: IN Endpoints Interrupt Mask
     *  1'b1: No IN Endpoints Interrupt Mask
     */
    uint32_t iepintmsk: 1;
    /** oepintmsk : R/W; bitpos: [19]; default: 0;
     *  1'b0: OUT Endpoints Interrupt Mask
     *  1'b1: No OUT Endpoints Interrupt Mask
     */
    uint32_t oepintmsk: 1;
    /** incompisoinmsk : R/W; bitpos: [20]; default: 0;
     *  1'b0: Incomplete Isochronous IN Transfer Mask
     *  1'b1: No Incomplete Isochronous IN Transfer Mask
     */
    uint32_t incompisoinmsk: 1;
    /** incompipmsk : R/W; bitpos: [21]; default: 0;
     *  1'b0: Host mode: Incomplete Periodic Transfer Mask Device mode: Incomplete
     *  Isochronous OUT Transfer Mask
     *  1'b1: Host mode: No Incomplete Periodic Transfer Mask  Device mode: No Incomplete
     *  Isochronous OUT Transfer Mask
     */
    uint32_t incompipmsk: 1;
    /** fetsuspmsk : R/W; bitpos: [22]; default: 0;
     *  1'b0: Data Fetch Suspended Mask
     *  1'b1: No Data Fetch Suspended Mask
     */
    uint32_t fetsuspmsk: 1;
    /** resetdetmsk : R/W; bitpos: [23]; default: 0;
     *  1'b0: Reset detected Interrupt Mask
     *  1'b1: No Reset detected Interrupt Mask
     */
    uint32_t resetdetmsk: 1;
    /** prtlntmsk : R/W; bitpos: [24]; default: 0;
     *  1'b0: Host Port Interrupt Mask
     *  1'b1: No Host Port Interrupt Mask
     */
    uint32_t prtlntmsk: 1;
    /** hchintmsk : R/W; bitpos: [25]; default: 0;
     *  1'b0: Host Channels Interrupt Mask
     *  1'b1: No Host Channels Interrupt Mask
     */
    uint32_t hchintmsk: 1;
    /** ptxfempmsk : R/W; bitpos: [26]; default: 0;
     *  1'b0: Periodic TxFIFO Empty Mask
     *  1'b1: No Periodic TxFIFO Empty Mask
     */
    uint32_t ptxfempmsk: 1;
    uint32_t reserved_27: 1
    /** conidstschngmsk : R/W; bitpos: [28]; default: 0;
     *  1'b0: Connector ID Status Change Mask
     *  1'b1: No Connector ID Status Change Mask
     */
    uint32_t conidstschngmsk: 1;
    /** disconnintmsk : R/W; bitpos: [29]; default: 0;
     *  1'b0: Disconnect Detected Interrupt Mask
     *  1'b1: No Disconnect Detected Interrupt Mask
     */
    uint32_t disconnintmsk: 1;
    /** sessreqintmsk : R/W; bitpos: [30]; default: 0;
     *  1'b0: Session Request or New Session Detected Interrupt Mask
     *  1'b1: No Session Request or New Session Detected Interrupt Mask
     */
    uint32_t sessreqintmsk: 1;
    /** wkupintmsk : R/W; bitpos: [31]; default: 0;
     *  1'b0 : Resume or Remote Wakeup Detected Interrupt Mask
     *  1'b1 : Unmask Resume Remote Wakeup Detected Interrupt
     */
    uint32_t wkupintmsk: 1;


} usb_gintmsk_reg_t;

/** Type of haint register
 *  Host All Channels Interrupt Register
 */
typedef volatile struct {
    /** haint : RO; bitpos: [8:0]; default: 0;
     *  Channel Interrupt for channel no.
     */
    uint32_t haint: 8;


} usb_haint_reg_t;

/** Type of haintmsk register
 *  Host All Channels Interrupt Mask Register
 */
typedef volatile struct {
    /** haintmsk : R/W; bitpos: [8:0]; default: 0;
     *  Channel Interrupt Mask (HAINTMSK_REG) One bit per channel: Bit 0 for channel 0, bit
     *  15 for channel 15.
     */
    uint32_t haintmsk: 8;


} usb_haintmsk_reg_t;

/** Type of hcintn register
 *  Host Channel n Interrupt Register
 */
typedef volatile struct {
    /** h_xfercompln : R/W1C; bitpos: [0]; default: 0;
     *  1'b0: Transfer in progress or No Active Transfer
     *  1'b1: Transfer completed normally without any errors
     */
    uint32_t h_xfercompln: 1;
    /** h_chhltdn : R/W1C; bitpos: [1]; default: 0;
     *  1'b0: Channel not halted
     *  1'b1: Channel Halted
     */
    uint32_t h_chhltdn: 1;
    /** h_ahberrn : R/W1C; bitpos: [2]; default: 0;
     *  1'b0: No AHB error
     *  1'b1: AHB error during AHB read/write
     */
    uint32_t h_ahberrn: 1;
    /** h_stalln : R/W1C; bitpos: [3]; default: 0;
     *  1'b0: No Stall Response Received Interrupt
     *  1'b1: Stall Response Received Interrupt
     */
    uint32_t h_stalln: 1;
    /** h_nackn : R/W1C; bitpos: [4]; default: 0;
     *  1'b0: No NAK Response Received Interrupt
     *  1'b1: NAK Response Received Interrupt
     */
    uint32_t h_nackn: 1;
    /** h_ackn : R/W1C; bitpos: [5]; default: 0;
     *  1'b0: No ACK Response Received or Transmitted Interrupt
     *  1'b1: ACK Response Received or Transmitted Interrup
     */
    uint32_t h_ackn: 1;
    /** h_nyetn : R/W1C; bitpos: [6]; default: 0;
     *  1'b0: No NYET Response Received Interrupt
     *  1'b1: NYET Response Received Interrupt
     */
    uint32_t h_nyetn: 1;
    /** h_xacterrn : R/W1C; bitpos: [7]; default: 0;
     *  Indicates one of the following errors occurred on the USB:
     *  CRC check failure
     *  Timeout
     *  Bit stuff error
     *  False EOP
     */
    uint32_t h_xacterrn: 1;
    /** h_bblerrn : R/W1C; bitpos: [8]; default: 0;
     *  1'b0: No Babble Error
     *  1'b1: Babble Error
     */
    uint32_t h_bblerrn: 1;
    /** h_frmovrunn : R/W1C; bitpos: [9]; default: 0;
     *  1'b0: No Frame Overrun
     *  1'b1: Frame Overrun
     */
    uint32_t h_frmovrunn: 1;
    /** h_datatglerrn : R/W1C; bitpos: [10]; default: 0;
     *  1'b0: No Data Toggle Error
     *  1'b1: Data Toggle Error
     */
    uint32_t h_datatglerrn: 1;
    /** h_bnaintrn : R/W1C; bitpos: [11]; default: 0;
     *  1'b0: No BNA Interrupt
     *  1'b1: BNA Interrupt
     */
    uint32_t h_bnaintrn: 1;
    /** h_xcs_xact_errn : R/W1C; bitpos: [12]; default: 0;
     *  1'b0: No Excessive Transaction Error
     *  1'b1: Excessive Transaction Error
     */
    uint32_t h_xcs_xact_errn: 1;
    /** h_desc_lst_rollintrn : R/W1C; bitpos: [13]; default: 0;
     *  1'b0: No Descriptor rollover interrupt
     *  1'b1: Descriptor rollover interrupt
     */
    uint32_t h_desc_lst_rollintrn: 1;


} usb_hcintn_reg_t;

/** Type of hcintmskn register
 *  Host Channel n Interrupt Mask Register
 */
typedef volatile struct {
    /** h_xfercomplmskn : R/W; bitpos: [0]; default: 0;
     *  1'b0: Transfer Completed Mask
     *  1'b1: No Transfer Completed Mask
     */
    uint32_t h_xfercomplmskn: 1;
    /** h_chhltdmskn : R/W; bitpos: [1]; default: 0;
     *  1'b0: Channel Halted Mask
     *  1'b1: No Channel Halted Mask
     */
    uint32_t h_chhltdmskn: 1;
    /** h_ahberrmskn : R/W; bitpos: [2]; default: 0;
     *  1'b0: AHB Error Mask
     *  1'b1: No AHB Error Mask
     */
    uint32_t h_ahberrmskn: 1;
    /** h_stallmskn : R/W; bitpos: [3]; default: 0;
     *  1'b0: Mask STALL Response Received Interrupt
     *  1'b1: No STALL Response Received Interrupt Mask
     */
    uint32_t h_stallmskn: 1;
    /** h_nakmskn : R/W; bitpos: [4]; default: 0;
     *  1'b0: Mask NAK Response Received Interrupt
     *  1'b1: No NAK Response Received Interrupt Mask
     */
    uint32_t h_nakmskn: 1;
    /** h_ackmskn : R/W; bitpos: [5]; default: 0;
     *  1'b0: Mask ACK Response Received/Transmitted Interrupt
     *  1'b1: No ACK Response Received/Transmitted Interrupt Mask
     */
    uint32_t h_ackmskn: 1;
    /** h_nyetmskn : R/W; bitpos: [6]; default: 0;
     *  1'b0: Mask NYET Response Received Interrupt
     *  1'b1: No NYET Response Received Interrupt Mask
     */
    uint32_t h_nyetmskn: 1;
    /** h_xacterrmskn : R/W; bitpos: [7]; default: 0;
     *  1'b0: Mask Transaction Error
     *  1'b1: No Transaction Error Mask
     */
    uint32_t h_xacterrmskn: 1;
    /** h_bblerrmskn : R/W; bitpos: [8]; default: 0;
     *  Babble Error Mask
     *  1'b0: Mask Babble Error
     *  1'b1: No Babble Error Mask
     */
    uint32_t h_bblerrmskn: 1;
    /** h_frmovrunmskn : R/W; bitpos: [9]; default: 0;
     *  Frame Overrun Mask
     *  0x0 (MASK): Mask Overrun Mask
     *  0x1 (NOMASK): No Frame Overrun Mask
     */
    uint32_t h_frmovrunmskn: 1;
    /** h_datatglerrmskn : R/W; bitpos: [10]; default: 0;
     *  Data Toggle Error Mask n scatter/gather DMA mode for host
     *  1'b0: Mask Data Toggle Error
     *  1'b1: No Data Toggle Error Mask
     */
    uint32_t h_datatglerrmskn: 1;
    /** h_bnaintrmskn : R/W; bitpos: [11]; default: 0;
     *  BNA (Buffer Not Available) Interrupt mask register
     *  1'b0: BNA Interrupt Masked
     *  1'b1: BNA Interrupt not masked
     */
    uint32_t h_bnaintrmskn: 1;
    uint32_t reserved_12: 1
    /** h_desc_lst_rollintrmskn : R/W; bitpos: [13]; default: 0;
     *  Descriptor List rollover interrupt Mask
     *  1'b0: Descriptor Rollover Interrupt Mask
     *  1'b1: Descriptor Rollover Interrupt not masked
     */
    uint32_t h_desc_lst_rollintrmskn: 1;


} usb_hcintmskn_reg_t;

/** Type of diepmsk register
 *  Device IN Endpoint Common Interrupt Mask Register
 */
typedef volatile struct {
    /** di_xfercomplmsk : R/W; bitpos: [0]; default: 0;
     *  0x0 : Mask Transfer Completed Interrupt
     *  0x1 : No Transfer Completed Interrupt Mask
     */
    uint32_t di_xfercomplmsk: 1;
    /** di_epdisbldmsk : R/W; bitpos: [1]; default: 0;
     *  0x0 : Mask Endpoint Disabled Interrupt
     *  0x1 : No Endpoint Disabled Interrupt Mask
     */
    uint32_t di_epdisbldmsk: 1;
    /** di_ahbermsk : R/W; bitpos: [2]; default: 0;
     *  0x0 : Mask AHB Error Interrupt
     *  0x1 : No AHB Error Interrupt Mask
     */
    uint32_t di_ahbermsk: 1;
    /** timeoutmsk : R/W; bitpos: [3]; default: 0;
     *  0x0 : Mask Timeout Condition Interrupt
     *  0x1 : No Timeout Condition Interrupt Mask
     */
    uint32_t timeoutmsk: 1;
    /** intkntxfempmsk : R/W; bitpos: [4]; default: 0;
     *  0x0 : Mask IN Token Received When TxFIFO Empty Interrupt
     *  0x1 : No IN Token Received When TxFIFO Empty Interrupt
     */
    uint32_t intkntxfempmsk: 1;
    /** intknepmismsk : R/W; bitpos: [5]; default: 0;
     *  0x0 : Mask IN Token received with EP Mismatch Interrupt
     *  0x1 : No Mask IN Token received with EP Mismatch Interrupt
     */
    uint32_t intknepmismsk: 1;
    /** inepnakeffmsk : R/W; bitpos: [6]; default: 0;
     *  0x0 : Mask IN Endpoint NAK Effective Interrupt
     *  0x1 : No IN Endpoint NAK Effective Interrupt Mask
     */
    uint32_t inepnakeffmsk: 1;
    uint32_t reserved_7: 1
    /** txfifoundrnmsk : R/W; bitpos: [8]; default: 0;
     *  0x0 : Mask Fifo Underrun Interrupt
     *  0x1 : No Fifo Underrun Interrupt Mask
     */
    uint32_t txfifoundrnmsk: 1;
    /** bnainintrmsk : R/W; bitpos: [9]; default: 0;
     *  0x0 : Mask BNA Interrupt
     *  0x1 : No BNA Interrupt Mask
     */
    uint32_t bnainintrmsk: 1;
    uint32_t reserved_10: 3
    /** di_nakmsk : R/W; bitpos: [13]; default: 0;
     *  0x0 : Mask NAK Interrupt
     *  0x1 : No Mask NAK Interrupt
     */
    uint32_t di_nakmsk: 1;


} usb_diepmsk_reg_t;

/** Type of doepmsk register
 *  Device OUT Endpoint Common Interrupt Mask Register
 */
typedef volatile struct {
    /** xfercomplmsk : R/W; bitpos: [0]; default: 0;
     *  0x0 : Mask Transfer Completed Interrupt
     *  0x1 : No Transfer Completed Interrupt Mask
     */
    uint32_t xfercomplmsk: 1;
    /** epdisbldmsk : R/W; bitpos: [1]; default: 0;
     *  0x0 : Mask Endpoint Disabled Interrupt
     *  0x1 : No Endpoint Disabled Interrupt Mask
     */
    uint32_t epdisbldmsk: 1;
    /** ahbermsk : R/W; bitpos: [2]; default: 0;
     *  0x0 : Mask AHB Error Interrupt
     *  0x1 : No AHB Error Interrupt Mask
     */
    uint32_t ahbermsk: 1;
    /** setupmsk : R/W; bitpos: [3]; default: 0;
     *  0x0 : Mask SETUP Phase Done Interrupt
     *  0x1 : No SETUP Phase Done Interrupt Mask
     */
    uint32_t setupmsk: 1;
    /** outtknepdismsk : R/W; bitpos: [4]; default: 0;
     *  0x0 : Mask OUT Token Received when Endpoint Disabled Interrupt
     *  0x1 : No OUT Token Received when Endpoint Disabled Interrupt Mask
     */
    uint32_t outtknepdismsk: 1;
    /** stsphsercvdmsk : R/W; bitpos: [5]; default: 0;
     *  0x0 : Status Phase Received Mask
     *  0x1 : No Status Phase Received Mask
     */
    uint32_t stsphsercvdmsk: 1;
    /** back2backsetup : R/W; bitpos: [6]; default: 0;
     *  0x0 : Mask Back-to-Back SETUP Packets Received Interrupt
     *  0x1 : No Back-to-Back SETUP Packets Received Interrupt Mask
     */
    uint32_t back2backsetup: 1;
    uint32_t reserved_7: 1
    /** outpkterrmsk : R/W; bitpos: [8]; default: 0;
     *  0x0 : Mask OUT Packet Error Interrupt
     *  0x1 : No OUT Packet Error Interrupt Mask
     */
    uint32_t outpkterrmsk: 1;
    /** bnaoutintrmsk : R/W; bitpos: [9]; default: 0;
     *  0x0 : Mask BNA Interrupt
     *  0x1 : No BNA Interrupt Mask
     */
    uint32_t bnaoutintrmsk: 1;
    uint32_t reserved_10: 2
    /** bbleerrmsk : R/W; bitpos: [12]; default: 0;
     *  0x0 : Mask Babble Error Interrupt
     *  0x1 : No Babble Error Interrupt Mask
     */
    uint32_t bbleerrmsk: 1;
    /** nakmsk : R/W; bitpos: [13]; default: 0;
     *  0x0 : Mask NAK Interrupt
     *  0x1 : No NAK Interrupt Mask
     */
    uint32_t nakmsk: 1;
    /** nyetmsk : R/W; bitpos: [14]; default: 0;
     *  NYET interrupt Mask
     *  0x0 : Mask NYET Interrupt
     *  0x1 : No NYET Interrupt Mask
     */
    uint32_t nyetmsk: 1;


} usb_doepmsk_reg_t;

/** Type of daint register
 *  Device All Endpoints Interrupt Register
 */
typedef volatile struct {
    /** inepint0 : RO; bitpos: [0]; default: 0;
     *  IN Endpoint 0 Interrupt Bit.
     */
    uint32_t inepint0: 1;
    /** inepint1 : RO; bitpos: [1]; default: 0;
     *  IN Endpoint 1 Interrupt Bit.
     */
    uint32_t inepint1: 1;
    /** inepint2 : RO; bitpos: [2]; default: 0;
     *  IN Endpoint 2 Interrupt Bit.
     */
    uint32_t inepint2: 1;
    /** inepint3 : RO; bitpos: [3]; default: 0;
     *  IN Endpoint 3 Interrupt Bit.
     */
    uint32_t inepint3: 1;
    /** inepint4 : RO; bitpos: [4]; default: 0;
     *  IN Endpoint 4 Interrupt Bit.
     */
    uint32_t inepint4: 1;
    /** inepint5 : RO; bitpos: [5]; default: 0;
     *  IN Endpoint 5 Interrupt Bit.
     */
    uint32_t inepint5: 1;
    /** inepint6 : RO; bitpos: [6]; default: 0;
     *  IN Endpoint 6 Interrupt Bit.
     */
    uint32_t inepint6: 1;
    uint32_t reserved_7: 9
    /** outepint0 : RO; bitpos: [16]; default: 0;
     *  OUT Endpoint 0 Interrupt Bit.
     */
    uint32_t outepint0: 1;
    /** outepint1 : RO; bitpos: [17]; default: 0;
     *  OUT Endpoint 1 Interrupt Bit.
     */
    uint32_t outepint1: 1;
    /** outepint2 : RO; bitpos: [18]; default: 0;
     *  OUT Endpoint 2 Interrupt Bit.
     */
    uint32_t outepint2: 1;
    /** outepint3 : RO; bitpos: [19]; default: 0;
     *  OUT Endpoint 3 Interrupt Bit.
     */
    uint32_t outepint3: 1;
    /** outepint4 : RO; bitpos: [20]; default: 0;
     *  OUT Endpoint 4 Interrupt Bit.
     */
    uint32_t outepint4: 1;
    /** outepint5 : RO; bitpos: [21]; default: 0;
     *  OUT Endpoint 5 Interrupt Bit.
     */
    uint32_t outepint5: 1;
    /** outepint6 : RO; bitpos: [22]; default: 0;
     *  OUT Endpoint 6 Interrupt Bit.
     */
    uint32_t outepint6: 1;


} usb_daint_reg_t;

/** Type of daintmsk register
 *  Device All Endpoints Interrupt Mask Register
 */
typedef volatile struct {
    /** inepmsk0 : R/W; bitpos: [0]; default: 0;
     *  IN Endpoint 0 Interrupt mask Bit.
     */
    uint32_t inepmsk0: 1;
    /** inepmsk1 : R/W; bitpos: [1]; default: 0;
     *  IN Endpoint 1 Interrupt mask Bit.
     */
    uint32_t inepmsk1: 1;
    /** inepmsk2 : R/W; bitpos: [2]; default: 0;
     *  IN Endpoint 2 Interrupt mask Bit.
     */
    uint32_t inepmsk2: 1;
    /** inepmsk3 : R/W; bitpos: [3]; default: 0;
     *  IN Endpoint 3 Interrupt mask Bit.
     */
    uint32_t inepmsk3: 1;
    /** inepmsk4 : R/W; bitpos: [4]; default: 0;
     *  IN Endpoint 4 Interrupt mask Bit.
     */
    uint32_t inepmsk4: 1;
    /** inepmsk5 : R/W; bitpos: [5]; default: 0;
     *  IN Endpoint 5 Interrupt mask Bit.
     */
    uint32_t inepmsk5: 1;
    /** inepmsk6 : R/W; bitpos: [6]; default: 0;
     *  IN Endpoint 6 Interrupt mask Bit.
     */
    uint32_t inepmsk6: 1;
    uint32_t reserved_7: 9
    /** outepmsk0 : R/W; bitpos: [16]; default: 0;
     *  OUT Endpoint 0 Interrupt mask Bit.
     */
    uint32_t outepmsk0: 1;
    /** outepmsk1 : R/W; bitpos: [17]; default: 0;
     *  OUT Endpoint 1 Interrupt mask Bit.
     */
    uint32_t outepmsk1: 1;
    /** outepmsk2 : R/W; bitpos: [18]; default: 0;
     *  OUT Endpoint 2 Interrupt mask Bit.
     */
    uint32_t outepmsk2: 1;
    /** outepmsk3 : R/W; bitpos: [19]; default: 0;
     *  OUT Endpoint 3 Interrupt mask Bit.
     */
    uint32_t outepmsk3: 1;
    /** outepmsk4 : R/W; bitpos: [20]; default: 0;
     *  OUT Endpoint 4 Interrupt mask Bit.
     */
    uint32_t outepmsk4: 1;
    /** outepmsk5 : R/W; bitpos: [21]; default: 0;
     *  OUT Endpoint 5 Interrupt mask Bit.
     */
    uint32_t outepmsk5: 1;
    /** outepmsk6 : R/W; bitpos: [22]; default: 0;
     *  OUT Endpoint 6 Interrupt mask Bit.
     */
    uint32_t outepmsk6: 1;


} usb_daintmsk_reg_t;

/** Type of diepempmsk register
 *  Device IN Endpoint FIFO Empty Interrupt Mask Register
 */
typedef volatile struct {
    /** d_ineptxfempmsk : R/W; bitpos: [16:0]; default: 0;
     *  IN EP Tx FIFO Empty Interrupt Mask Bits
     *  0x1 (EP0_MASK): Mask IN EP0 Tx FIFO Empty Interrupt
     *  0x2 (EP1_MASK): Mask IN EP1 Tx FIFO Empty Interrupt
     *  0x4 (EP2_MASK): Mask IN EP2 Tx FIFO Empty Interrupt
     *  0x8 (EP3_MASK): Mask IN EP3 Tx FIFO Empty Interrupt
     *  0x10 (EP4_MASK): Mask IN EP4 Tx FIFO Empty Interrupt
     *  0x20 (EP5_MASK): Mask IN EP5 Tx FIFO Empty Interrupt
     *  0x40 (EP6_MASK): Mask IN EP6 Tx FIFO Empty Interrupt
     *  0x80 (EP7_MASK): Mask IN EP7 Tx FIFO Empty Interrupt
     *  0x100 (EP8_MASK): Mask IN EP8 Tx FIFO Empty Interrupt
     *  0x200 (EP9_MASK): Mask IN EP9 Tx FIFO Empty Interrupt
     *  0x400 (EP10_MASK): Mask IN EP10 Tx FIFO Empty Interrupt
     *  0x800 (EP11_MASK): Mask IN EP11 Tx FIFO Empty Interrupt
     *  0x1000 (EP12_MASK): Mask IN EP12 Tx FIFO Empty Interrupt
     *  0x2000 (EP13_MASK): Mask IN EP13 Tx FIFO Empty Interrupt
     *  0x4000 (EP14_MASK): Mask IN EP14 Tx FIFO Empty Interrupt
     *  0x8000 (EP15_MASK): Mask IN EP15 Tx FIFO Empty Interrupt
     */
    uint32_t d_ineptxfempmsk: 16;


} usb_diepempmsk_reg_t;

/** Type of diepintn register
 *  Device IN Endpoint n Interrupt Register
 */
typedef volatile struct {
    /** d_xfercompln : R/W1C; bitpos: [0]; default: 0;
     *  0x0 : No Transfer Complete Interrupt
     *  0x1 : Transfer Completed Interrupt
     */
    uint32_t d_xfercompln: 1;
    /** d_epdisbldn : R/W1C; bitpos: [1]; default: 0;
     *  0x0 : No Endpoint Disabled Interrupt
     *  0x1 : Endpoint Disabled Interrupt
     */
    uint32_t d_epdisbldn: 1;
    /** d_ahberrn : R/W1C; bitpos: [2]; default: 0;
     *  0x0 : No AHB Error Interrupt
     *  0x1 : AHB Error interrupt
     */
    uint32_t d_ahberrn: 1;
    /** d_timeoutn : R/W1C; bitpos: [3]; default: 0;
     *  0x0 : No Timeout interrupt
     *  0x1 : Timeout interrupt
     */
    uint32_t d_timeoutn: 1;
    /** d_intkntxfempn : R/W1C; bitpos: [4]; default: 0;
     *  0x0 : No IN Token Received when TxFIFO Empty Interrupt
     *  0x1 : IN Token Received when TxFIFO Empty Interrupt
     */
    uint32_t d_intkntxfempn: 1;
    /** d_intknepmisn : R/W1C; bitpos: [5]; default: 0;
     *  0x0 : No IN Token Received with EP Mismatch Interrupt
     *  0x1 : IN Token Received with EP Mismatch interrupt
     */
    uint32_t d_intknepmisn: 1;
    /** d_inepnakeffn : R/W1C; bitpos: [6]; default: 0;
     *  IN Endpoint NAK Effective
     *  0x0 : No IN Endpoint NAK Effective interrupt
     *  0x1 : IN Endpoint NAK Effective interrupt
     */
    uint32_t d_inepnakeffn: 1;
    /** d_txfempn : RO; bitpos: [7]; default: 0;
     *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
     *  completely empty
     *  0x0 : No Transmit FIFO Empty interrupt
     *  0x1 : Transmit FIFO Empty interrupt
     */
    uint32_t d_txfempn: 1;
    /** d_txfifoundrnn : R/W1C; bitpos: [8]; default: 0;
     *  The core generates this interrupt when it detects a transmit FIFO underrun
     *  condition in threshold mode for this endpoint
     *  0x0 : No Fifo Underrun interrupt
     *  0x1 : Fifo Underrun interrupt
     */
    uint32_t d_txfifoundrnn: 1;
    /** d_bnaintrn : R/W1C; bitpos: [9]; default: 0;
     *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
     *  this interrupt when the descriptor accessed is not ready for the Core to process,
     *  such as Host busy or DMA done
     *  0x0 : No BNA interrupt
     *  0x1 : BNA interrupt
     */
    uint32_t d_bnaintrn: 1;
    uint32_t reserved_10: 1
    /** d_pktdrpstsn : R/W1C; bitpos: [11]; default: 0;
     *  Packet Drop Status. This bit indicates to the application that an ISOC OUT packet
     *  has been dropped. This bit does not have an associated mask bit and does not
     *  generate an interrupt
     *  0x0 : No interrupt
     *  0x1 : Packet Drop Status
     */
    uint32_t d_pktdrpstsn: 1;
    /** d_bbleerrn : R/W1C; bitpos: [12]; default: 0;
     *  The core generates this interrupt when babble is received for the endpoint
     *  0x0 : No interrupt
     *  0x1 : BbleErr interrupt
     */
    uint32_t d_bbleerrn: 1;
    /** d_nakintrptn : R/W1C; bitpos: [13]; default: 0;
     *  NAK Interrupt. The core generates this interrupt when a NAK is transmitted or
     *  received by the device
     *  0x0 : No interrupt
     *  0x1 : NAK Interrupt
     */
    uint32_t d_nakintrptn: 1;
    /** d_nyetintrptn : R/W1C; bitpos: [14]; default: 0;
     *  NYET Interrupt. The core generates this interrupt when a NYET response is
     *  transmitted for a non isochronous OUT endpoint
     *  0x0 : No interrupt
     *  0x1 : NYET Interrupt
     */
    uint32_t d_nyetintrptn: 1;


} usb_diepintn_reg_t;

/** Type of doepintn register
 *  Device OUT Endpoint n Interrupt Register
 */
typedef volatile struct {
    /** xfercompln : R/W1C; bitpos: [0]; default: 0;
     *  Transfer Completed Interrupt
     *  0x0 : No Transfer Complete Interrupt
     *  0x1 : Transfer Complete Interrupt
     */
    uint32_t xfercompln: 1;
    /** epdisbldn : R/W1C; bitpos: [1]; default: 0;
     *  Endpoint Disabled Interrupt
     *  0x0 : No Endpoint Disabled Interrupt
     *  0x1 : Endpoint Disabled Interrupt
     */
    uint32_t epdisbldn: 1;
    /** ahberrn : R/W1C; bitpos: [2]; default: 0;
     *  AHB Error
     *  0x0 : No AHB Error Interrupt
     *  0x1 : AHB Error interrupt
     */
    uint32_t ahberrn: 1;
    /** setupn : R/W1C; bitpos: [3]; default: 0;
     *  SETUP Phase Done
     *  0x0 : No SETUP Phase Done
     *  0x1 : SETUP Phase Done
     */
    uint32_t setupn: 1;
    /** outtknepdisn : R/W1C; bitpos: [4]; default: 0;
     *  OUT Token Received When Endpoint Disabled
     *  0x0 : No OUT Token Received When Endpoint Disabled
     *  0x1 : OUT Token Received When Endpoint Disabled
     */
    uint32_t outtknepdisn: 1;
    /** stsphsercvdn : R/W1C; bitpos: [5]; default: 0;
     *  Status Phase Received for Control Write
     *  0x0 : No Status Phase Received for Control Write
     *  0x1 : Status Phase Received for Control Write
     */
    uint32_t stsphsercvdn: 1;
    /** back2backsetupn : R/W1C; bitpos: [6]; default: 0;
     *  Back-to-Back SETUP Packets Received
     *  0x0 : No Back-to-Back SETUP Packets Received
     *  0x1 : Back-to-Back SETUP Packets Received
     */
    uint32_t back2backsetupn: 1;
    uint32_t reserved_7: 1
    /** outpkterrn : R/W1C; bitpos: [8]; default: 0;
     *  OUT Packet Error
     *  0x0 : No OUT Packet Error
     *  0x1 : OUT Packet Error
     */
    uint32_t outpkterrn: 1;
    /** bnaintrn : R/W1C; bitpos: [9]; default: 0;
     *  Buffer Not Available Interrupt
     *  0x0 : No BNA interrupt
     *  0x1 : BNA interrupt
     */
    uint32_t bnaintrn: 1;
    uint32_t reserved_10: 1
    /** pktdrpstsn : R/W1C; bitpos: [11]; default: 0;
     *  0x0 : No interrupt
     *  0x1 : Packet Drop Status interrupt
     */
    uint32_t pktdrpstsn: 1;
    /** bbleerrn : R/W1C; bitpos: [12]; default: 0;
     *  0x0 : No BbleErr interrupt
     *  0x1 : BbleErr interrupt
     */
    uint32_t bbleerrn: 1;
    /** nakintrptn : R/W1C; bitpos: [13]; default: 0;
     *  0x0 : No NAK interrupt
     *  0x1 : NAK Interrupt
     */
    uint32_t nakintrptn: 1;
    /** nyepintrptn : R/W1C; bitpos: [14]; default: 0;
     *  0x0 : No NYET interrupt
     *  0x1 : NYET Interrupt
     */
    uint32_t nyepintrptn: 1;
    /** stuppktrcvdn : R/W1C; bitpos: [15]; default: 0;
     *  0x0 : No Setup packet received
     *  0x1 : Setup packet received
     */
    uint32_t stuppktrcvdn: 1;


} usb_doepintn_reg_t;



/** configuration registers */
/** Type of gahbcfg register
 *  AHB Configuration Register
 */
typedef volatile struct {
    /** glbllntrmsk : R/W; bitpos: [0]; default: 0;
     *  1'b0: Mask the interrupt assertion to the application.
     *  1'b1: Unmask the interrupt assertion to the application
     */
    uint32_t glbllntrmsk: 1;
    /** hbstlen : R/W; bitpos: [5:1]; default: 0;
     *  this field is used in Internal DMA modes
     *  4'b0000 Single
     *  4'b0001: INCR
     *  4'b0011 INCR4
     *  4'b0101 INCR8
     *  4'b0111 INCR16
     *  Others: Reserved
     */
    uint32_t hbstlen: 4;
    /** dmaen : R/W; bitpos: [5]; default: 0;
     *  This bit is always 0 when Slave-Only mode has been selected
     *  1'b0:Core operates in Slave mode
     *  1'b1:Core operates in a DMA mode
     */
    uint32_t dmaen: 1;
    uint32_t reserved_6: 1
    /** nptxfemplvl : R/W; bitpos: [7]; default: 0;
     *  Non-Periodic TxFIFO Empty Level
     *  1'b0: DIEPINTn_REG.REG_TXFEMP interrupt indicates that the Non-Periodic TxFIFO is
     *  half empty or that the IN Endpoint TxFIFO is half empty
     *  1'b1: GINTSTS_REG.USB_NPTXFEMP interrupt indicates that the Non-Periodic TxFIFO is
     *  completely empty or that the IN Endpoint TxFIFO is completely empty
     */
    uint32_t nptxfemplvl: 1;
    /** ptxfemplvl : R/W; bitpos: [8]; default: 0;
     *  Indicates when the Periodic TxFIFO Empty Interrupt bit in the Core Interrupt
     *  register (GINTSTS_REG.USB_PTXFEMP) is triggered. This bit is used only in Slave
     *  mode
     *  1'b0: GINTSTS_REG.USB_PTXFEMP interrupt indicates that the Periodic TxFIFO is half
     *  empty
     *  1'b1: GINTSTS_REG.USB_PTXFEMP interrupt indicates that the Periodic TxFIFO is
     *  completely empty
     */
    uint32_t ptxfemplvl: 1;
    uint32_t reserved_9: 12
    /** remmemsupp : R/W; bitpos: [21]; default: 0;
     *  Remote Memory Support (RemMemSupp) This bit is programmed to enable the
     *  functionality to wait for thesystem DMA Done Signal for the DMA Write Transfers
     *  1'b0:Remote Memory Support Feature disabled
     *  1'b1:Remote Memory Support Feature enabled
     */
    uint32_t remmemsupp: 1;
    /** notialldmawrit : R/W; bitpos: [22]; default: 0;
     *  Notify All DMA Write Transactions (NotiAllDmaWrit) This bit is programmed to enable
     *  the System DMA Done functionality for all the DMA write Transactions corresponding
     *  to the Channel/Endpoint. This bit is valid only when GAHBCFG.RemMemSupp is set to 1
     */
    uint32_t notialldmawrit: 1;
    /** ahbsingle : R/W; bitpos: [23]; default: 0;
     *  AHB Single Support (AHBSingle) This bit when programmed supports Single transfers
     *  for the remaining data in a transfer when the core is operating in DMA mode
     *  1'b0: The remaining data in the transfer is sent using INCR burst size
     *  1'b1: The remaining data in the transfer is sent using Single burst size
     */
    uint32_t ahbsingle: 1;
    /** invdescendianess : R/W; bitpos: [24]; default: 0;
     *  Invert Descriptor Endianess
     *  1'b0: Descriptor Endianness is same as AHB Master Endianness
     *  1'b1:Invert Descriptor Endianess according to AHB Master endianness
     */
    uint32_t invdescendianess: 1;


} usb_gahbcfg_reg_t;

/** Type of gusbcfg register
 *  USB Configuration Register
 */
typedef volatile struct {
    /** toutcal : R/W; bitpos: [3:0]; default: 0;
     *  FS Timeout Calibration
     */
    uint32_t toutcal: 3;
    /** phyif : R/W; bitpos: [3]; default: 0;
     *  The application uses this bit to configure the core to support a
     *  UTMI+ PHY with an 8- or 16-bit interface. When a ULPI PHY is
     *  chosen, this must be Set to 8-bit mode
     *  1'b0: 8 bits
     *  1'b1: 16 bits
     */
    uint32_t phyif: 1;
    /** ulpi_utmi_sel : RO; bitpos: [4]; default: 0;
     *  1'b0: UTMI+ Interface
     *  1'b1: ULPI Interface
     */
    uint32_t ulpi_utmi_sel: 1;
    /** fsintf : R/W; bitpos: [5]; default: 0;
     *  1'b0: 6-pin unidirectional full-speed serial interface
     *  1'b1: 3-pin bidirectional full-speed serial interface
     */
    uint32_t fsintf: 1;
    /** physel : RO; bitpos: [6]; default: 1;
     *  1'b0: USB 2.0 high-speed UTMI+ or ULPI PHY
     *  1'b1: USB 1.1 full-speed serial transceiver
     */
    uint32_t physel: 1;
    uint32_t reserved_7: 1
    /** srpcap : R/W; bitpos: [8]; default: 0;
     *  1'b0: SRP capability is not enabled
     *  1'b1: SRP capability is enabled
     */
    uint32_t srpcap: 1;
    /** hnpcap : R/W; bitpos: [9]; default: 0;
     *  1'b0: HNP capability is not enabled
     *  1'b1: HNP capability is enabled
     */
    uint32_t hnpcap: 1;
    /** usbtrdtim : R/W; bitpos: [14:10]; default: 5;
     *  4'h5: When the MAC interface is 16-bit UTMI+
     *  4'h9: When the MAC interface is 8-bit UTMI+
     */
    uint32_t usbtrdtim: 4;
    uint32_t reserved_14: 8
    /** termseldlpulse : R/W; bitpos: [22]; default: 0;
     *  TermSel DLine Pulsing Selection
     *  1'b0: Data line pulsing using utmi_txvalid (Default)
     *  1'b1: Data line pulsing using utmi_termsel
     */
    uint32_t termseldlpulse: 1;
    uint32_t reserved_23: 5
    /** txenddelay : R/W; bitpos: [28]; default: 0;
     *  1'b0 : Normal Mode
     *  1'b1 : Tx End delay
     */
    uint32_t txenddelay: 1;
    /** forcehstmode : R/W; bitpos: [29]; default: 0;
     *  1'b0 : Normal Mode
     *  1'b1 : Force Host Mode
     */
    uint32_t forcehstmode: 1;
    /** forcedevmode : R/W; bitpos: [30]; default: 0;
     *  1'b0 : Normal Mode
     *  1'b1 : Force Device Mode
     */
    uint32_t forcedevmode: 1;
    /** corrupttxpkt : R/W; bitpos: [31]; default: 0;
     *  This bit is for debug purposes only. Never Set this bit to 1. The application
     *  should always write 1'b0 to this bit
     *  1'b0:Normal Mode
     *  1'b1:Debug Mode
     */
    uint32_t corrupttxpkt: 1;


} usb_gusbcfg_reg_t;

/** Type of grstctl register
 *  Reset Register
 */
typedef volatile struct {
    /** csftrst : R_WS_SC; bitpos: [0]; default: 0;
     *  Core Soft Reset
     */
    uint32_t csftrst: 1;
    /** piufssftrst : R_WS_SC; bitpos: [1]; default: 0;
     *  1'b0: No Reset
     *  1'b1:PIU FS Dedicated Controller Soft Reset
     */
    uint32_t piufssftrst: 1;
    /** frmcntrrst : R/W1S; bitpos: [2]; default: 0;
     *  Host only. Host Frame Counter Reset.The application writes this bit to reset the
     *  (micro)Frame number counter inside the core. When the (micro)Frame counter is
     *  reset, the subsequent SOF sent out by the core has a (micro)Frame number of 0
     */
    uint32_t frmcntrrst: 1;
    uint32_t reserved_3: 1
    /** rxfflsh : R/W1S; bitpos: [4]; default: 0;
     *  RxFIFO Flush. The application can flush the entire RxFIFO using this bit, but must
     *  first ensure that the core is not in the middle of a transaction.The application
     *  must only write to this bit after checking that the controller is neither reading
     *  from the RxFIFO nor writing to the RxFIFO
     *  1'b0:Does not flush the entire RxFIFO
     *  1'b1:flushes the entire RxFIFO
     */
    uint32_t rxfflsh: 1;
    /** txfflsh : R/W1S; bitpos: [5]; default: 0;
     *  TxFIFO Flush.This bit selectively flushes a single or all transmit FIFOs, but
     *  cannot do so If the core is in the midst of a transaction.The application must
     *  write this bit only after checking that the core is neither writing to the TxFIFO
     *  nor reading from the TxFIFO.
     */
    uint32_t txfflsh: 1;
    /** txfnum : R/W; bitpos: [11:6]; default: 0;
     *  TxFIFO Number.This is the FIFO number that must be flushed using the TxFIFO Flush
     *  bit. This field must not be changed until the core clears the TxFIFO Flush bit
     */
    uint32_t txfnum: 5;
    uint32_t reserved_11: 19
    /** dmareq : RO; bitpos: [30]; default: 0;
     *  DMA Request Signal
     *  1'b0:No DMA request
     *  1'b1:DMA request is in progress
     */
    uint32_t dmareq: 1;
    /** ahbidle : RO; bitpos: [31]; default: 0;
     *  AHB Master Idle
     *  1'b0:Not Idle
     *  1'b1:AHB Master Idle
     */
    uint32_t ahbidle: 1;


} usb_grstctl_reg_t;

/** Type of grxfsiz register
 *  Receive FIFO Size Register
 */
typedef volatile struct {
    /** rxfdep : R/W; bitpos: [16:0]; default: 256;
     *  RxFIFO Depth.This value is in terms of 32-bit words.Minimum value is 16,Maximum
     *  value is 32,768
     */
    uint32_t rxfdep: 16;


} usb_grxfsiz_reg_t;

/** Type of gnptxfsiz register
 *  Non-periodic Transmit FIFO Size Register
 */
typedef volatile struct {
    /** nptxfstaddr : R/W; bitpos: [16:0]; default: 256;
     *  The NPTxFStAddr field description is valid only for host mode.This field contains
     *  the memory start address for Non-periodic Transmit FIFO RAM.
     */
    uint32_t nptxfstaddr: 16;
    /** nptxfdep : R/W; bitpos: [32:16]; default: 256;
     *  The NPTxFDep field description is valid only for host mode or device mode when
     *  OTG_EN_DED_TX_FIFO=0.Minimum value is 16,Maximum value is 32,768.
     */
    uint32_t nptxfdep: 16;


} usb_gnptxfsiz_reg_t;

/** Type of gnptxsts register
 *  Non-periodic Transmit FIFO/Queue Status Register
 */
typedef volatile struct {
    /** nptxfspcavail : RO; bitpos: [16:0]; default: 256;
     *  Non-periodic TxFIFO Space Avail.Indicates the amount of free space available in the
     *  Non-periodic TxFIFO.Values are in terms of 32-bit words.
     */
    uint32_t nptxfspcavail: 16;
    /** nptxqspcavail : RO; bitpos: [20:16]; default: 4;
     *  Non-periodic Transmit Request Queue Space Available.Indicates the amount of free
     *  space available in the Non-periodic Transmit Request Queue. This queue holds both
     *  IN and OUT requests in Host mode. Device mode has only IN requests.
     */
    uint32_t nptxqspcavail: 4;
    uint32_t reserved_20: 4
    /** nptxqtop : RO; bitpos: [31:24]; default: 0;
     *  Top of the Non-periodic Transmit Request Queue.
     *  Bits [30:27]: Channel/endpoint number.
     *  Bits [26:25]:  2'b00: IN/OUT token   2'b01: Zero-length transmit packet (device
     *  IN/host OUT)   2'b10: PING/CSPLIT token  2'b11: Channel halt command.
     *  Bit [24]: Terminate (last Entry for selected channel/endpoint).
     */
    uint32_t nptxqtop: 7;


} usb_gnptxsts_reg_t;

/** Type of hctsizn register
 *  Host Channel nTransfer Size Register
 */
typedef volatile struct {
    /** h_xfersizen : R/W; bitpos: [19:0]; default: 0;
     *  Non-Scatter/Gather DMA Mode: Transfer Size.
     *  Scatter/Gather DMA Mode:
     *  [18:16]: Reserved
     *  [15:8]: NTD (Number of Transfer Descriptors)
     */
    uint32_t h_xfersizen: 19;
    /** h_pktcntn : R/W; bitpos: [29:19]; default: 0;
     *  when in Non-Scatter/Gather DMA mode: Packet Count . This field is programmed by the
     *  application with the expected number of packets to be transmitted (OUT) or received
     *  (IN).
     *  Scatter/Gather DMA Mode: Reserved.
     */
    uint32_t h_pktcntn: 10;
    /** h_pidn : R/W; bitpos: [31:29]; default: 0;
     *  2'b00: DATA0
     *  2'b01: DATA2
     *  2'b10: DATA1
     *  2'b11: MDATA (non-control)/SETUP (control)
     */
    uint32_t h_pidn: 2;
    /** h_dopngn : R/W; bitpos: [31]; default: 0;
     *  This bit is used only for OUT transfers. Setting this field to 1 directs the host
     *  to do PING protocol
     *  1'b0: No ping protocol
     *  1'b1: Ping protocol
     */
    uint32_t h_dopngn: 1;


} usb_hctsizn_reg_t;



/** Status registers */
/** Type of grxstsr register
 *  Receive Status Debug Read Register
 */
typedef volatile struct {
    /** g_chnum : RO; bitpos: [4:0]; default: 0;
     *  Channel Number: Host only. Indicates the channel number to which the current
     *  received packet belongs.
     *  Endpoint Number: Device only Indicates the endpoint number to which the current
     *  received packet belongs
     */
    uint32_t g_chnum: 4;
    /** g_bcnt : RO; bitpos: [15:4]; default: 0;
     *  In host mode, indicates the byte count of the received IN data Packet
     *  In device mode, indicates the byte count of the received data packet
     */
    uint32_t g_bcnt: 11;
    /** g_dpid : RO; bitpos: [17:15]; default: 0;
     *  Data PID
     *  0x0 (DATA0): DATA0
     *  0x2 (DATA1): DATA1
     *  0x1 (DATA2): DATA2
     *  0x3 (MDATA): MDATA
     */
    uint32_t g_dpid: 2;
    /** g_pktsts : RO; bitpos: [21:17]; default: 0;
     *  0x1 (OUTNAK): Global OUT NAK in device mode (triggers an Interrupt)
     *  0x2 (INOUTDPRX): IN data packet received in host mode and  OUT data received in
     *  device mode
     *  0x3 (INOUTTRCOM): IN or OUT transfer completed in both host  and device (triggers
     *  an interrupt)
     *  0x4 (DSETUPCOM): SETUP transaction completed in device mode (triggers an interrupt)
     *  0x5 (DTTOG): Data toggle error (triggers an interrupt) in host Mode
     *  0x6 (DSETUPRX): SETUP data packet received in device mode
     *  0x7 (CHHALT): Channel halted in host mode (triggers an  interrupt)
     */
    uint32_t g_pktsts: 4;
    /** g_fn : RO; bitpos: [25:21]; default: 0;
     *  This is the least significant 4 bits of the (micro)Frame number in which the packet
     *  is received on the USB. This field is supported only when isochronous OUT endpoints
     *  are supported
     */
    uint32_t g_fn: 4;


} usb_grxstsr_reg_t;

/** Type of grxstsp register
 *  Receive Status Read/Pop Register
 */
typedef volatile struct {
    /** chnum : RO; bitpos: [4:0]; default: 0;
     *  Channel Number: Host only. Indicates the channel number to which the current
     *  received packet belongs.
     *  Endpoint Number: Device only Indicates the endpoint number to which the current
     *  received packet belongs
     */
    uint32_t chnum: 4;
    /** bcnt : RO; bitpos: [15:4]; default: 0;
     *  In host mode, indicates the byte count of the received IN data Packet
     *  In device mode, indicates the byte count of the received data packet
     */
    uint32_t bcnt: 11;
    /** dpid : RO; bitpos: [17:15]; default: 0;
     *  Data PID
     *  0x0 (DATA0): DATA0
     *  0x2 (DATA1): DATA1
     *  0x1 (DATA2): DATA2
     *  0x3 (MDATA): MDATA
     */
    uint32_t dpid: 2;
    /** pktsts : RO; bitpos: [21:17]; default: 0;
     *  0x1 (OUTNAK): Global OUT NAK in device mode (triggers an Interrupt)
     *  0x2 (INOUTDPRX): IN data packet received in host mode and  OUT data packet received
     *  in device mode
     *  0x3 (INOUTTRCOM): IN or OUT transfer completed in both host  and device mode
     *  (triggers an interrupt)
     *  0x4 (DSETUPCOM): SETUP transaction completed in device mode (triggers an interrupt)
     *  0x5 (DTTOG): Data toggle error (triggers an interrupt) in host Mode
     *  0x6 (DSETUPRX): SETUP data packet received in device mode
     *  0x7 (CHHALT): Channel halted in host mode (triggers an  interrupt)
     */
    uint32_t pktsts: 4;
    /** fn : RO; bitpos: [25:21]; default: 0;
     *  This is the least significant 4 bits of the (micro)Frame number in which the packet
     *  is received on the USB. This field is supported only when isochronous OUT endpoints
     *  are supported
     */
    uint32_t fn: 4;


} usb_grxstsp_reg_t;

/** Type of gsnpsid register
 *  Synopsys ID Register
 */
typedef volatile struct {
    /** synopsysid : RO; bitpos: [32:0]; default: 1330921482;
     *  ID register
     */
    uint32_t synopsysid: 32;


} usb_gsnpsid_reg_t;

/** Type of ghwcfg1 register
 *  User Hardware Configuration 1 Register
 */
typedef volatile struct {
    /** epdir : RO; bitpos: [32:0]; default: 0;
     *  This 32-bit field uses two bits per endpoint to determine the endpoint direction.
     *  Bits [31:30]: Endpoint 15 direction
     *  Bits [29:28]: Endpoint 14 direction
     *  ...
     *  Direction:
     *  2'b00: BIDIR (IN and OUT) endpoint
     *  2'b01: IN endpoint
     *  2'b10: OUT endpoint
     *  2'b11: Reserved
     */
    uint32_t epdir: 32;


} usb_ghwcfg1_reg_t;

/** Type of ghwcfg2 register
 *  User Hardware Configuration 2 Register
 */
typedef volatile struct {
    /** otgmode : RO; bitpos: [3:0]; default: 0;
     *  3'b000: HNP- and SRP-Capable OTG (Host  Device)
     */
    uint32_t otgmode: 3;
    /** otgarch : RO; bitpos: [5:3]; default: 2;
     *  2'b10: Internal DMA
     */
    uint32_t otgarch: 2;
    /** singpnt : RO; bitpos: [5]; default: 1;
     *  Point-to-Point
     *  1'b1: Single-point application (no hub and split support).
     */
    uint32_t singpnt: 1;
    /** hsphytype : RO; bitpos: [8:6]; default: 0;
     *  High-Speed PHY Interface Type
     *  2'b00: High-Speed interface not supported
     */
    uint32_t hsphytype: 2;
    /** fsphytype : RO; bitpos: [10:8]; default: 1;
     *  Full-Speed PHY Interface Type.
     */
    uint32_t fsphytype: 2;
    /** numdeveps : RO; bitpos: [14:10]; default: 6;
     *  Number of Device Endpoints.
     */
    uint32_t numdeveps: 4;
    /** numhstchnl : RO; bitpos: [18:14]; default: 7;
     *  Number of Host Channels.
     */
    uint32_t numhstchnl: 4;
    /** periosupport : RO; bitpos: [18]; default: 1;
     *  1'b0:Periodic OUT Channels is not Supported in Host Mode
     *  1'b1:Periodic OUT Channels Supported in Host Mode
     */
    uint32_t periosupport: 1;
    /** dynfifosizing : RO; bitpos: [19]; default: 1;
     *  1'b0:Dynamic FIFO Sizing Disabled
     *  1'b1:Dynamic FIFO Sizing Enabled
     */
    uint32_t dynfifosizing: 1;
    /** multiprocintrpt : RO; bitpos: [20]; default: 0;
     *  1'b0: No Multi Processor Interrupt Enabled
     *  1'b1:Multi Processor Interrupt Enabled
     */
    uint32_t multiprocintrpt: 1;
    uint32_t reserved_21: 1
    /** nptxqdepth : RO; bitpos: [24:22]; default: 1;
     *  Non-periodic Request Queue Depth
     *  2'b01: 4
     */
    uint32_t nptxqdepth: 2;
    /** ptxqdepth : RO; bitpos: [26:24]; default: 2;
     *  Host Mode Periodic Request Queue Depth.
     *  2'b10: 8
     */
    uint32_t ptxqdepth: 2;
    /** tknqdepth : RO; bitpos: [31:26]; default: 8;
     *  Device Mode IN Token Sequence Learning Queue Depth.
     */
    uint32_t tknqdepth: 5;
    /** otg_enable_ic_usb : RO; bitpos: [31]; default: 0;
     *  0x0 (DISABLE): Disabled the IC_USB Full-Speed Serial Transceiver interface
     *  0x1 (ENABLE): Enabled the IC_USB Full-Speed Serial Transceiver interface
     */
    uint32_t otg_enable_ic_usb: 1;


} usb_ghwcfg2_reg_t;

/** Type of ghwcfg3 register
 *  User Hardware Configuration 3 Register
 */
typedef volatile struct {
    /** xfersizewidth : RO; bitpos: [4:0]; default: 5;
     *  Width of Transfer Size Counters
     *  0x5 (WIDTH16): Width of Transfer Size Counter 16 bits
     */
    uint32_t xfersizewidth: 4;
    /** pktsizewidth : RO; bitpos: [7:4]; default: 3;
     *  Width of Packet Size Counters
     *  3'b011: 7 bits
     */
    uint32_t pktsizewidth: 3;
    /** otgen : RO; bitpos: [7]; default: 1;
     *  OTG Function Enabled.
     */
    uint32_t otgen: 1;
    /** i2cintsel : RO; bitpos: [8]; default: 0;
     *  1'b0: I2C Interface is not available on the controller.
     */
    uint32_t i2cintsel: 1;
    /** vndctlsupt : RO; bitpos: [9]; default: 0;
     *  Vendor Control Interface is not available .
     */
    uint32_t vndctlsupt: 1;
    /** optfeature : RO; bitpos: [10]; default: 1;
     *  Optional Features have been Removed.
     */
    uint32_t optfeature: 1;
    /** rsttype : RO; bitpos: [11]; default: 0;
     *  Asynchronous reset is used in the core
     */
    uint32_t rsttype: 1;
    /** adpsupport : RO; bitpos: [12]; default: 0;
     *  ADP logic is not present along with the controller.
     */
    uint32_t adpsupport: 1;
    /** hsicmode : RO; bitpos: [13]; default: 0;
     *  HSIC mode specified for Mode of Operation.
     *  1'b0: Non-HSIC-capable
     */
    uint32_t hsicmode: 1;
    /** bcsupport : RO; bitpos: [14]; default: 0;
     *  1'b0: No Battery Charger Support
     */
    uint32_t bcsupport: 1;
    /** lpmmode : RO; bitpos: [15]; default: 0;
     *  LPM mode specified for Mode of Operation.
     */
    uint32_t lpmmode: 1;
    /** dfifodepth : RO; bitpos: [32:16]; default: 256;
     *  DFIFO Depth.This value is in terms of 32-bit words.
     */
    uint32_t dfifodepth: 16;


} usb_ghwcfg3_reg_t;

/** Type of ghwcfg4 register
 *  User Hardware Configuration 4 Register
 */
typedef volatile struct {
    /** g_numdevperioeps : RO; bitpos: [4:0]; default: 0;
     *  Number of Device Mode Periodic IN Endpoints.
     */
    uint32_t g_numdevperioeps: 4;
    /** g_partialpwrdn : RO; bitpos: [4]; default: 1;
     *  Enable Partial Power Down.
     */
    uint32_t g_partialpwrdn: 1;
    /** g_ahbfreq : RO; bitpos: [5]; default: 1;
     *  Minimum AHB Frequency Less Than 60 MHz
     */
    uint32_t g_ahbfreq: 1;
    /** g_hibernation : RO; bitpos: [6]; default: 0;
     *  1'b0: Hibernation feature not enabled.
     */
    uint32_t g_hibernation: 1;
    /** g_extendedhibernation : RO; bitpos: [7]; default: 0;
     *  Extended Hibernation feature not enabled
     */
    uint32_t g_extendedhibernation: 1;
    uint32_t reserved_8: 4
    /** g_acgsupt : RO; bitpos: [12]; default: 0;
     *  Active Clock Gating is not enabled.
     */
    uint32_t g_acgsupt: 1;
    /** g_enhancedlpmsupt : RO; bitpos: [13]; default: 1;
     *  Enhanced LPM Support.
     */
    uint32_t g_enhancedlpmsupt: 1;
    /** g_phydatawidth : RO; bitpos: [16:14]; default: 2;
     *  UTMI+ PHY/ULPI-to-Internal UTMI+ Wrapper Data Width (PhyDataWidth) When a ULPI PHY
     *  is used, an internal wrapper converts ULPI to UTMI+
     *  2'b10: 8/16 bits, software selectable
     */
    uint32_t g_phydatawidth: 2;
    /** g_numctleps : RO; bitpos: [20:16]; default: 0;
     *  Number of Device Mode Control Endpoints in Addition to Endpoint 0
     */
    uint32_t g_numctleps: 4;
    /** g_iddqfltr : RO; bitpos: [20]; default: 1;
     *  IDDIG Filter Enable.
     */
    uint32_t g_iddqfltr: 1;
    /** g_vbusvalidfltr : RO; bitpos: [21]; default: 1;
     *  VBUS Valid Filter Enabled.
     */
    uint32_t g_vbusvalidfltr: 1;
    /** g_avalidfltr : RO; bitpos: [22]; default: 1;
     *  a_valid Filter Enabled.
     */
    uint32_t g_avalidfltr: 1;
    /** g_bvalidfltr : RO; bitpos: [23]; default: 1;
     *  b_valid Filter Enabled.
     */
    uint32_t g_bvalidfltr: 1;
    /** g_sessendfltr : RO; bitpos: [24]; default: 1;
     *  session_end Filter Enabled.
     */
    uint32_t g_sessendfltr: 1;
    /** g_dedfifomode : RO; bitpos: [25]; default: 1;
     *  Enable Dedicated Transmit FIFO for device IN Endpoints
     */
    uint32_t g_dedfifomode: 1;
    /** g_ineps : RO; bitpos: [30:26]; default: 4;
     *  Number of Device Mode IN Endpoints Including Control Endpoints.
     */
    uint32_t g_ineps: 4;
    /** g_descdmaenabled : RO; bitpos: [30]; default: 1;
     *  0x1: Scatter/Gather DMA configuration.
     */
    uint32_t g_descdmaenabled: 1;
    /** g_descdma : RO; bitpos: [31]; default: 1;
     *  Scatter/Gather DMA configuration
     *  1'b1: Dynamic configuration
     */
    uint32_t g_descdma: 1;


} usb_ghwcfg4_reg_t;

/** Type of hfnum register
 *  Frame Number configure Resigster
 */
typedef volatile struct {
    /** frnum : RO; bitpos: [14:0]; default: 16383;
     *  Frame Number
     *  0x0 (INACTIVE): No SOF is transmitted
     *  0x1 (ACTIVE): SOF is transmitted
     */
    uint32_t frnum: 14;
    uint32_t reserved_14: 2
    /** frrem : RO; bitpos: [32:16]; default: 0;
     *  Frame Time Remaining.ndicates the amount of time remaining in the current
     *  microframe
     *  (HS) or Frame (FS/LS), in terms of PHY clocks.
     */
    uint32_t frrem: 16;


} usb_hfnum_reg_t;

/** Type of hptxsts register
 *  Host Periodic Transmit FIFO/Queue Status Register
 */
typedef volatile struct {
    /** ptxfspcavail : RO; bitpos: [16:0]; default: 256;
     *  Periodic Transmit Data FIFO Space Available. Values are in terms of 32-bit words.
     */
    uint32_t ptxfspcavail: 16;
    /** ptxqspcavail : RO; bitpos: [21:16]; default: 8;
     *  Periodic Transmit Request Queue Space Available.
     */
    uint32_t ptxqspcavail: 5;
    uint32_t reserved_21: 3
    /** ptxqtop : RO; bitpos: [32:24]; default: 0;
     *  Bit [31]: Odd/Even (micro)Frame. 1'b0: send in even (micro)Frame
     *  Bits [30:27]: Channel/endpoint number
     *  Bits [26:25]: Type. 2'b00: IN/OUT.   2'b01: Zero-length packet.     2'b10: CSPLIT
     *    2'b11: Disable channel command
     *  Bit [24]: Terminate
     */
    uint32_t ptxqtop: 8;


} usb_hptxsts_reg_t;

/** Type of hcdmab$n register
 *  Host Channel $n DMA Buffer Address Register
 */
typedef volatile struct {
    /** h_hcdmab0 : RO; bitpos: [32:0]; default: 0;
     *  Holds the current buffer address
     */
    uint32_t h_hcdmab0: 32;


} usb_hcdmab$n_reg_t;

/** Type of hcdmabn register
 *  Host Channel n DMA Buffer Address Register
 */
typedef volatile struct {
    /** h_hcdmabn : RO; bitpos: [32:0]; default: 0;
     *  Holds the current buffer address
     */
    uint32_t h_hcdmabn: 32;


} usb_hcdmabn_reg_t;

/** Type of dsts register
 *  Device Status Register
 */
typedef volatile struct {
    /** suspsts : RO; bitpos: [0]; default: 0;
     *  Suspend Status
     *  0x0 : No suspend state
     *  0x1 : Suspend state
     */
    uint32_t suspsts: 1;
    /** enumspd : RO; bitpos: [3:1]; default: 1;
     *  0x0 : High speed (PHY clock is running at 30 or 60 MHz)
     *  0x1 : Full speed (PHY clock is running at 30 or 60 MHz)
     *  0x2 : Low speed (PHY clock is running at 6 MHz)
     *  0x3 : Full speed (PHY clock is running at 48 MHz)
     */
    uint32_t enumspd: 2;
    /** errticerr : RO; bitpos: [3]; default: 0;
     *  0x0 : No Erratic Error
     *  0x1 : Erratic Error
     */
    uint32_t errticerr: 1;
    uint32_t reserved_4: 4
    /** soffn : RO; bitpos: [22:8]; default: 0;
     *  Frame or Microframe Number of the Received SOF (SOFFN).  This field contains a
     *  Frame number.
     */
    uint32_t soffn: 14;
    /** devlnsts : RO; bitpos: [24:22]; default: 0;
     *  Device Line Status
     *  DevLnSts[1]: Logic level of D+
     *  DevLnSts[0]: Logic level of D-
     */
    uint32_t devlnsts: 2;


} usb_dsts_reg_t;

/** Type of dtxfstsn register
 *  Device IN Endpoint Transmit FIFO Status Register n
 */
typedef volatile struct {
    /** d_ineptxfspcavailn : RO; bitpos: [16:0]; default: 0;
     *  Indicates the amount of free space available in the Endpoint  TxFIFO.
     */
    uint32_t d_ineptxfspcavailn: 16;


} usb_dtxfstsn_reg_t;

/** Type of diepdmabn register
 *  Device IN Endpoint 16 Buffer Address Register
 */
typedef volatile struct {
    /** d_dmabufferaddrn : RO; bitpos: [32:0]; default: 0;
     *  Holds the current buffer address.This register is updated as and when the data
     *  transfer for the corresponding end point is in progress. This register is present
     *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
     */
    uint32_t d_dmabufferaddrn: 32;


} usb_diepdmabn_reg_t;



typedef struct {
    volatile usb_gotgctl_reg_t gotgctl;
    volatile usb_gdfifocfg_reg_t gdfifocfg;
    volatile usb_hptxfsiz_reg_t hptxfsiz;
    union {
        volatile usb_dieptxfn_reg_t dieptxfn[4];
        struct {
            volatile usb_dieptxfn_reg_t dieptxf1;
            volatile usb_dieptxfn_reg_t dieptxf2;
            volatile usb_dieptxfn_reg_t dieptxf3;
            volatile usb_dieptxfn_reg_t dieptxf4;
        };
    };
    volatile usb_hcfg_reg_t hcfg;
    volatile usb_hfir_reg_t hfir;
    volatile usb_hflbaddr_reg_t hflbaddr;
    volatile usb_hprt_reg_t hprt;
    union {
        volatile usb_hccharn_reg_t hccharn[8];
        struct {
            volatile usb_hccharn_reg_t hcchar0;
            volatile usb_hccharn_reg_t hcchar1;
            volatile usb_hccharn_reg_t hcchar2;
            volatile usb_hccharn_reg_t hcchar3;
            volatile usb_hccharn_reg_t hcchar4;
            volatile usb_hccharn_reg_t hcchar5;
            volatile usb_hccharn_reg_t hcchar6;
            volatile usb_hccharn_reg_t hcchar7;
        };
    };
    union {
        volatile usb_hcdman_reg_t hcdman[8];
        struct {
            volatile usb_hcdman_reg_t hcdma0;
            volatile usb_hcdman_reg_t hcdma1;
            volatile usb_hcdman_reg_t hcdma2;
            volatile usb_hcdman_reg_t hcdma3;
            volatile usb_hcdman_reg_t hcdma4;
            volatile usb_hcdman_reg_t hcdma5;
            volatile usb_hcdman_reg_t hcdma6;
            volatile usb_hcdman_reg_t hcdma7;
        };
    };
    volatile usb_dcfg_reg_t dcfg;
    volatile usb_dctl_reg_t dctl;
    volatile usb_dvbusdis_reg_t dvbusdis;
    volatile usb_dvbuspulse_reg_t dvbuspulse;
    volatile usb_dthrctl_reg_t dthrctl;
    volatile usb_diepctl0_reg_t diepctl0;
    union {
        volatile usb_dieptsizn_reg_t dieptsizn[7];
        struct {
            volatile usb_dieptsizn_reg_t dieptsiz0;
            volatile usb_dieptsizn_reg_t dieptsiz1;
            volatile usb_dieptsizn_reg_t dieptsiz2;
            volatile usb_dieptsizn_reg_t dieptsiz3;
            volatile usb_dieptsizn_reg_t dieptsiz4;
            volatile usb_dieptsizn_reg_t dieptsiz5;
            volatile usb_dieptsizn_reg_t dieptsiz6;
        };
    };
    union {
        volatile usb_diepdman_reg_t diepdman[7];
        struct {
            volatile usb_diepdman_reg_t diepdma0;
            volatile usb_diepdman_reg_t diepdma1;
            volatile usb_diepdman_reg_t diepdma2;
            volatile usb_diepdman_reg_t diepdma3;
            volatile usb_diepdman_reg_t diepdma4;
            volatile usb_diepdman_reg_t diepdma5;
            volatile usb_diepdman_reg_t diepdma6;
        };
    };
    volatile usb_diepctl1_reg_t diepctl1;
    volatile usb_diepctl2_reg_t diepctl2;
    volatile usb_diepctl3_reg_t diepctl3;
    volatile usb_diepctl4_reg_t diepctl4;
    volatile usb_diepctl5_reg_t diepctl5;
    volatile usb_diepctl6_reg_t diepctl6;
    volatile usb_doepctl0_reg_t doepctl0;
    union {
        volatile usb_doeptsizn_reg_t doeptsizn[7];
        struct {
            volatile usb_doeptsizn_reg_t doeptsiz0;
            volatile usb_doeptsizn_reg_t doeptsiz1;
            volatile usb_doeptsizn_reg_t doeptsiz2;
            volatile usb_doeptsizn_reg_t doeptsiz3;
            volatile usb_doeptsizn_reg_t doeptsiz4;
            volatile usb_doeptsizn_reg_t doeptsiz5;
            volatile usb_doeptsizn_reg_t doeptsiz6;
        };
    };
    union {
        volatile usb_doepdman_reg_t doepdman[7];
        struct {
            volatile usb_doepdman_reg_t doepdma0;
            volatile usb_doepdman_reg_t doepdma1;
            volatile usb_doepdman_reg_t doepdma2;
            volatile usb_doepdman_reg_t doepdma3;
            volatile usb_doepdman_reg_t doepdma4;
            volatile usb_doepdman_reg_t doepdma5;
            volatile usb_doepdman_reg_t doepdma6;
        };
    };
    union {
        volatile usb_doepdmabn_reg_t doepdmabn[7];
        struct {
            volatile usb_doepdmabn_reg_t doepdmab0;
            volatile usb_doepdmabn_reg_t doepdmab1;
            volatile usb_doepdmabn_reg_t doepdmab2;
            volatile usb_doepdmabn_reg_t doepdmab3;
            volatile usb_doepdmabn_reg_t doepdmab4;
            volatile usb_doepdmabn_reg_t doepdmab5;
            volatile usb_doepdmabn_reg_t doepdmab6;
        };
    };
    union {
        volatile usb_doepctln_reg_t doepctln[6];
        struct {
            volatile usb_doepctln_reg_t doepctl1;
            volatile usb_doepctln_reg_t doepctl2;
            volatile usb_doepctln_reg_t doepctl3;
            volatile usb_doepctln_reg_t doepctl4;
            volatile usb_doepctln_reg_t doepctl5;
            volatile usb_doepctln_reg_t doepctl6;
        };
    };
    volatile usb_pcgcctl_reg_t pcgcctl;
    volatile usb_gotgint_reg_t gotgint;
    volatile usb_gintsts_reg_t gintsts;
    volatile usb_gintmsk_reg_t gintmsk;
    volatile usb_haint_reg_t haint;
    volatile usb_haintmsk_reg_t haintmsk;
    union {
        volatile usb_hcintn_reg_t hcintn[8];
        struct {
            volatile usb_hcintn_reg_t hcint0;
            volatile usb_hcintn_reg_t hcint1;
            volatile usb_hcintn_reg_t hcint2;
            volatile usb_hcintn_reg_t hcint3;
            volatile usb_hcintn_reg_t hcint4;
            volatile usb_hcintn_reg_t hcint5;
            volatile usb_hcintn_reg_t hcint6;
            volatile usb_hcintn_reg_t hcint7;
        };
    };
    union {
        volatile usb_hcintmskn_reg_t hcintmskn[8];
        struct {
            volatile usb_hcintmskn_reg_t hcintmsk0;
            volatile usb_hcintmskn_reg_t hcintmsk1;
            volatile usb_hcintmskn_reg_t hcintmsk2;
            volatile usb_hcintmskn_reg_t hcintmsk3;
            volatile usb_hcintmskn_reg_t hcintmsk4;
            volatile usb_hcintmskn_reg_t hcintmsk5;
            volatile usb_hcintmskn_reg_t hcintmsk6;
            volatile usb_hcintmskn_reg_t hcintmsk7;
        };
    };
    volatile usb_diepmsk_reg_t diepmsk;
    volatile usb_doepmsk_reg_t doepmsk;
    volatile usb_daint_reg_t daint;
    volatile usb_daintmsk_reg_t daintmsk;
    volatile usb_diepempmsk_reg_t diepempmsk;
    union {
        volatile usb_diepintn_reg_t diepintn[7];
        struct {
            volatile usb_diepintn_reg_t diepint0;
            volatile usb_diepintn_reg_t diepint1;
            volatile usb_diepintn_reg_t diepint2;
            volatile usb_diepintn_reg_t diepint3;
            volatile usb_diepintn_reg_t diepint4;
            volatile usb_diepintn_reg_t diepint5;
            volatile usb_diepintn_reg_t diepint6;
        };
    };
    union {
        volatile usb_doepintn_reg_t doepintn[7];
        struct {
            volatile usb_doepintn_reg_t doepint0;
            volatile usb_doepintn_reg_t doepint1;
            volatile usb_doepintn_reg_t doepint2;
            volatile usb_doepintn_reg_t doepint3;
            volatile usb_doepintn_reg_t doepint4;
            volatile usb_doepintn_reg_t doepint5;
            volatile usb_doepintn_reg_t doepint6;
        };
    };
    volatile usb_gahbcfg_reg_t gahbcfg;
    volatile usb_gusbcfg_reg_t gusbcfg;
    volatile usb_grstctl_reg_t grstctl;
    volatile usb_grxfsiz_reg_t grxfsiz;
    volatile usb_gnptxfsiz_reg_t gnptxfsiz;
    volatile usb_gnptxsts_reg_t gnptxsts;
    union {
        volatile usb_hctsizn_reg_t hctsizn[8];
        struct {
            volatile usb_hctsizn_reg_t hctsiz0;
            volatile usb_hctsizn_reg_t hctsiz1;
            volatile usb_hctsizn_reg_t hctsiz2;
            volatile usb_hctsizn_reg_t hctsiz3;
            volatile usb_hctsizn_reg_t hctsiz4;
            volatile usb_hctsizn_reg_t hctsiz5;
            volatile usb_hctsizn_reg_t hctsiz6;
            volatile usb_hctsizn_reg_t hctsiz7;
        };
    };
    volatile usb_grxstsr_reg_t grxstsr;
    volatile usb_grxstsp_reg_t grxstsp;
    volatile usb_gsnpsid_reg_t gsnpsid;
    volatile usb_ghwcfg1_reg_t ghwcfg1;
    volatile usb_ghwcfg2_reg_t ghwcfg2;
    volatile usb_ghwcfg3_reg_t ghwcfg3;
    volatile usb_ghwcfg4_reg_t ghwcfg4;
    volatile usb_hfnum_reg_t hfnum;
    volatile usb_hptxsts_reg_t hptxsts;
    volatile usb_hcdmab$n_reg_t hcdmab$n;
    union {
        volatile usb_hcdmabn_reg_t hcdmabn[7];
        struct {
            volatile usb_hcdmabn_reg_t hcdmab1;
            volatile usb_hcdmabn_reg_t hcdmab2;
            volatile usb_hcdmabn_reg_t hcdmab3;
            volatile usb_hcdmabn_reg_t hcdmab4;
            volatile usb_hcdmabn_reg_t hcdmab5;
            volatile usb_hcdmabn_reg_t hcdmab6;
            volatile usb_hcdmabn_reg_t hcdmab7;
        };
    };
    volatile usb_dsts_reg_t dsts;
    union {
        volatile usb_dtxfstsn_reg_t dtxfstsn[7];
        struct {
            volatile usb_dtxfstsn_reg_t dtxfsts0;
            volatile usb_dtxfstsn_reg_t dtxfsts1;
            volatile usb_dtxfstsn_reg_t dtxfsts2;
            volatile usb_dtxfstsn_reg_t dtxfsts3;
            volatile usb_dtxfstsn_reg_t dtxfsts4;
            volatile usb_dtxfstsn_reg_t dtxfsts5;
            volatile usb_dtxfstsn_reg_t dtxfsts6;
        };
    };
    union {
        volatile usb_diepdmabn_reg_t diepdmabn[7];
        struct {
            volatile usb_diepdmabn_reg_t diepdmab0;
            volatile usb_diepdmabn_reg_t diepdmab1;
            volatile usb_diepdmabn_reg_t diepdmab2;
            volatile usb_diepdmabn_reg_t diepdmab3;
            volatile usb_diepdmabn_reg_t diepdmab4;
            volatile usb_diepdmabn_reg_t diepdmab5;
            volatile usb_diepdmabn_reg_t diepdmab6;
        };
    };
} usb_dev_gen_t;

#ifdef __cplusplus
}
#endif
