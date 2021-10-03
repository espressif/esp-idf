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
#include "soc/soc.h"
#include "soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Control/Status registers */
/** USB_GOTGCTL_REG register
 *  OTG Control and Status Register
 */
#define USB_GOTGCTL_REG (SOC_DPORT_USB_BASE + 0x0)
/** USB_SESREQSCS : RO; bitpos: [0]; default: 0;
 *  Session Request Success.The core sets this bit when a session request initiation is
 *  successful
 */
#define USB_SESREQSCS    (BIT(0))
#define USB_SESREQSCS_M  (USB_SESREQSCS_V << USB_SESREQSCS_S)
#define USB_SESREQSCS_V  0x00000001
#define USB_SESREQSCS_S  0
/** USB_SESREQ : R/W; bitpos: [1]; default: 0;
 *  Session Request.The application sets this bit to initiate a session request on the
 *  USB. The application can clear this bit by writing a 0 when the Host Negotiation
 *  Success Status Change bit in the OTG Interrupt register
 *  (GOTGINT_REG.USB_HSTNEGSUCSTSCHNG) is SET. The core clears this bit when the
 *  USB_HSTNEGSUCSTSCHNG bit is cleared
 */
#define USB_SESREQ    (BIT(1))
#define USB_SESREQ_M  (USB_SESREQ_V << USB_SESREQ_S)
#define USB_SESREQ_V  0x00000001
#define USB_SESREQ_S  1
/** USB_VBVALIDOVEN : R/W; bitpos: [2]; default: 0;
 *  VBUS Valid Override Enable
 *  1'b1 : Internally Bvalid received from the PHY is overridden with
 *  GOTGCTL_REG.REG_VBVALIDOVVAl
 *  1'b0 : Override is disabled and bvalid signal from the respective PHY selected is
 *  used internally by the controller
 */
#define USB_VBVALIDOVEN    (BIT(2))
#define USB_VBVALIDOVEN_M  (USB_VBVALIDOVEN_V << USB_VBVALIDOVEN_S)
#define USB_VBVALIDOVEN_V  0x00000001
#define USB_VBVALIDOVEN_S  2
/** USB_VBVALIDOVVAL : R/W; bitpos: [3]; default: 0;
 *  VBUS Valid OverrideValue
 *  1'b0 : vbusvalid value is 1'b0 when GOTGCTL_REG.USB_VBVALIDOVEN =1
 *  1'b1 : vbusvalid value is 1'b1 when GOTGCTL_REG.USB_VBVALIDOVEN =1
 */
#define USB_VBVALIDOVVAL    (BIT(3))
#define USB_VBVALIDOVVAL_M  (USB_VBVALIDOVVAL_V << USB_VBVALIDOVVAL_S)
#define USB_VBVALIDOVVAL_V  0x00000001
#define USB_VBVALIDOVVAL_S  3
/** USB_AVALIDOVEN : R/W; bitpos: [4]; default: 0;
 *  This bit is used to enable/disable the software to override the Avalid signal using
 *  the GOTGCTL.AVALIDOVVAL
 *  1'b1: Internally Avalid received from the PHY is overridden with
 *  GOTGCTL_REG.REG_AVALIDOVVAL
 *  1'b0: Override is disabled and avalid signal from the respective PHY selected is
 *  used internally by the core
 */
#define USB_AVALIDOVEN    (BIT(4))
#define USB_AVALIDOVEN_M  (USB_AVALIDOVEN_V << USB_AVALIDOVEN_S)
#define USB_AVALIDOVEN_V  0x00000001
#define USB_AVALIDOVEN_S  4
/** USB_AVALIDOVVAL : R/W; bitpos: [5]; default: 0;
 *  A-Peripheral Session Valid OverrideValue
 *  1'b0 : Avalid value is 1'b0 when GOTGCTL_REG.USB_AVALIDOVEN =1
 *  1'b1 : Avalid value is 1'b1 when GOTGCTL_REG.USB_AVALIDOVEN =1
 */
#define USB_AVALIDOVVAL    (BIT(5))
#define USB_AVALIDOVVAL_M  (USB_AVALIDOVVAL_V << USB_AVALIDOVVAL_S)
#define USB_AVALIDOVVAL_V  0x00000001
#define USB_AVALIDOVVAL_S  5
/** USB_BVALIDOVEN : R/W; bitpos: [6]; default: 0;
 *  This bit is used to enable/disable the software to override the Bvalid signal using
 *  the GOTGCTLREG.BVALIDOVVAL
 *  1'b1 : Internally Bvalid received from the PHY is overridden with
 *  GOTGCTL_REG.USB_BVALIDOVVAL
 *  1'b0 : Override is disabled and bvalid signal from the respective PHY selected is
 *  used internally by the force
 */
#define USB_BVALIDOVEN    (BIT(6))
#define USB_BVALIDOVEN_M  (USB_BVALIDOVEN_V << USB_BVALIDOVEN_S)
#define USB_BVALIDOVEN_V  0x00000001
#define USB_BVALIDOVEN_S  6
/** USB_BVALIDOVVAL : R/W; bitpos: [7]; default: 0;
 *  B-Peripheral Session Valid OverrideValue
 *  1'b0 : Bvalid value is 1'b0 when GOTGCTL_REG.USB_BVALIDOVEN =1
 *  1'b1 : Bvalid value is 1'b1 when GOTGCTL_REG.USB_BVALIDOVEN =1
 */
#define USB_BVALIDOVVAL    (BIT(7))
#define USB_BVALIDOVVAL_M  (USB_BVALIDOVVAL_V << USB_BVALIDOVVAL_S)
#define USB_BVALIDOVVAL_V  0x00000001
#define USB_BVALIDOVVAL_S  7
/** USB_HSTNEGSCS : RO; bitpos: [8]; default: 0;
 *  Host Negotiation Success.The controller sets this bit when host negotiation is
 *  successful. The controller clears this bit when the HNP Request (HNPReq) bit in
 *  this register is set.
 */
#define USB_HSTNEGSCS    (BIT(8))
#define USB_HSTNEGSCS_M  (USB_HSTNEGSCS_V << USB_HSTNEGSCS_S)
#define USB_HSTNEGSCS_V  0x00000001
#define USB_HSTNEGSCS_S  8
/** USB_HNPREQ : R/W; bitpos: [9]; default: 0;
 *  HNP Request .The application sets this bit to initiate an HNP request to the
 *  Connected USB host. The application can clear this bit by writing a 0 when the Host
 *  Negotiation Success Status Change bit in the OTG Interrupt register
 *  (GOTGINT_REG.HSTNEGSUCSTSCHNG) is SET. The controller clears this bit when the
 *  HSTNEGSUCSTSCHNG bit is cleared.
 */
#define USB_HNPREQ    (BIT(9))
#define USB_HNPREQ_M  (USB_HNPREQ_V << USB_HNPREQ_S)
#define USB_HNPREQ_V  0x00000001
#define USB_HNPREQ_S  9
/** USB_HSTSETHNPEN : R/W; bitpos: [10]; default: 0;
 *  Host Set HNP Enable.The application sets this bit when it has successfully enabled
 *  HNP (using the SetFeature.SetHNPEnable command) on the connected device
 *  1'b0: Host Set HNP is not enabled
 *  1'b1: Host Set HNP is enabled
 */
#define USB_HSTSETHNPEN    (BIT(10))
#define USB_HSTSETHNPEN_M  (USB_HSTSETHNPEN_V << USB_HSTSETHNPEN_S)
#define USB_HSTSETHNPEN_V  0x00000001
#define USB_HSTSETHNPEN_S  10
/** USB_DEVHNPEN : R/W; bitpos: [11]; default: 0;
 *  Device HNP Enabled.The application sets this bit when it successfully receives a
 *  SetFeature.SetHNPEnable command from the connected USB host
 *  1'b0: HNP is not enabled in the application
 *  1'b1: HNP is enabled in the application
 */
#define USB_DEVHNPEN    (BIT(11))
#define USB_DEVHNPEN_M  (USB_DEVHNPEN_V << USB_DEVHNPEN_S)
#define USB_DEVHNPEN_V  0x00000001
#define USB_DEVHNPEN_S  11
/** USB_EHEN : R/W; bitpos: [12]; default: 0;
 *  Embedded Host Enable.It is used to select between OTG A Device state Machine and
 *  Embedded Host state machine
 *  1'b0: OTG A Device state machine is selected
 *  1'b1: Embedded Host State Machine is selected
 */
#define USB_EHEN    (BIT(12))
#define USB_EHEN_M  (USB_EHEN_V << USB_EHEN_S)
#define USB_EHEN_V  0x00000001
#define USB_EHEN_S  12
/** USB_DBNCEFLTRBYPASS : R/W; bitpos: [15]; default: 0;
 *  Bypass Debounce filters for avalid, bvalid, vbusvalid, sessend, iddig signals when
 *  enabled
 *  1'b0: Disabled
 *  1'b1: Enabled
 */
#define USB_DBNCEFLTRBYPASS    (BIT(15))
#define USB_DBNCEFLTRBYPASS_M  (USB_DBNCEFLTRBYPASS_V << USB_DBNCEFLTRBYPASS_S)
#define USB_DBNCEFLTRBYPASS_V  0x00000001
#define USB_DBNCEFLTRBYPASS_S  15
/** USB_CONIDSTS : RO; bitpos: [16]; default: 0;
 *  Connector ID Status. Indicates the connector ID status on a connect event
 *  1'b0: The core is in A-Device mode
 *  1'b1: The core is in B-Device mode
 */
#define USB_CONIDSTS    (BIT(16))
#define USB_CONIDSTS_M  (USB_CONIDSTS_V << USB_CONIDSTS_S)
#define USB_CONIDSTS_V  0x00000001
#define USB_CONIDSTS_S  16
/** USB_DBNCTIME : RO; bitpos: [17]; default: 0;
 *  Long/Short Debounce Time. Indicates the debounce time of a detected connection
 *  1'b0: Long debounce time, used for physical connections (100ms + 2.5 micro-sec)
 *  1'b1: Short debounce time, used for soft connections (2.5 micro-sec)
 */
#define USB_DBNCTIME    (BIT(17))
#define USB_DBNCTIME_M  (USB_DBNCTIME_V << USB_DBNCTIME_S)
#define USB_DBNCTIME_V  0x00000001
#define USB_DBNCTIME_S  17
/** USB_ASESVLD : RO; bitpos: [18]; default: 0;
 *  A-Session Valid. Indicates the Host mode transceiver status
 *  1'b0: A-session is not valid
 *  1'b1: A-session is valid
 */
#define USB_ASESVLD    (BIT(18))
#define USB_ASESVLD_M  (USB_ASESVLD_V << USB_ASESVLD_S)
#define USB_ASESVLD_V  0x00000001
#define USB_ASESVLD_S  18
/** USB_BSESVLD : RO; bitpos: [19]; default: 0;
 *  B-Session Valid.Indicates the Device mode transceiver status
 *  1'b0: B-session is not valid
 *  1'b1: B-session is valid
 */
#define USB_BSESVLD    (BIT(19))
#define USB_BSESVLD_M  (USB_BSESVLD_V << USB_BSESVLD_S)
#define USB_BSESVLD_V  0x00000001
#define USB_BSESVLD_S  19
/** USB_OTGVER : R/W; bitpos: [20]; default: 0;
 *  OTG Version
 *  1'b0:Supports OTG Version 1.3
 *  1'b1:Supports OTG Version 2.0
 */
#define USB_OTGVER    (BIT(20))
#define USB_OTGVER_M  (USB_OTGVER_V << USB_OTGVER_S)
#define USB_OTGVER_V  0x00000001
#define USB_OTGVER_S  20
/** USB_CURMOD : RO; bitpos: [21]; default: 0;
 *  Current Mode of Operation
 *  1'b0: Device mode
 *  1'b1:Host mode
 */
#define USB_CURMOD    (BIT(21))
#define USB_CURMOD_M  (USB_CURMOD_V << USB_CURMOD_S)
#define USB_CURMOD_V  0x00000001
#define USB_CURMOD_S  21


/** USB_GDFIFOCFG_REG register
 *  Global DFIFO Configuration Register
 */
#define USB_GDFIFOCFG_REG (SOC_DPORT_USB_BASE + 0x5c)
/** USB_GDFIFOCFG : R/W; bitpos: [16:0]; default: 0;
 *  GDFIFOCfg
 */
#define USB_GDFIFOCFG    0x0000FFFF
#define USB_GDFIFOCFG_M  (USB_GDFIFOCFG_V << USB_GDFIFOCFG_S)
#define USB_GDFIFOCFG_V  0x0000FFFF
#define USB_GDFIFOCFG_S  0
/** USB_EPINFOBASEADDR : R/W; bitpos: [32:16]; default: 0;
 *  EPInfoBaseAddr
 */
#define USB_EPINFOBASEADDR    0x0000FFFF
#define USB_EPINFOBASEADDR_M  (USB_EPINFOBASEADDR_V << USB_EPINFOBASEADDR_S)
#define USB_EPINFOBASEADDR_V  0x0000FFFF
#define USB_EPINFOBASEADDR_S  16


/** USB_HPTXFSIZ_REG register
 *  Host Periodic Transmit FIFO Size Register
 */
#define USB_HPTXFSIZ_REG (SOC_DPORT_USB_BASE + 0x100)
/** USB_PTXFSTADDR : R/W; bitpos: [16:0]; default: 512;
 *  Host Periodic TxFIFO Start Address.
 */
#define USB_PTXFSTADDR    0x0000FFFF
#define USB_PTXFSTADDR_M  (USB_PTXFSTADDR_V << USB_PTXFSTADDR_S)
#define USB_PTXFSTADDR_V  0x0000FFFF
#define USB_PTXFSTADDR_S  0
/** USB_PTXFSIZE : R/W; bitpos: [32:16]; default: 4096;
 *  Host Periodic TxFIFO Depth,This value is in terms of 32-bit words..
 */
#define USB_PTXFSIZE    0x0000FFFF
#define USB_PTXFSIZE_M  (USB_PTXFSIZE_V << USB_PTXFSIZE_S)
#define USB_PTXFSIZE_V  0x0000FFFF
#define USB_PTXFSIZE_S  16


/** USB_DIEPTXF1_REG register
 *  Device IN Endpoint Transmit FIFO Size Register
 */
#define USB_DIEPTXF1_REG (SOC_DPORT_USB_BASE + 0x104)
/** USB_INEP1TXFSTADDR : R/W; bitpos: [16:0]; default: 512;
 *  IN Endpoint FIFOn Transmit RAM Start Address.
 */
#define USB_INEP1TXFSTADDR    0x0000FFFF
#define USB_INEP1TXFSTADDR_M  (USB_INEP1TXFSTADDR_V << USB_INEP1TXFSTADDR_S)
#define USB_INEP1TXFSTADDR_V  0x0000FFFF
#define USB_INEP1TXFSTADDR_S  0
/** USB_INEP1TXFDEP : R/W; bitpos: [32:16]; default: 4096;
 *  IN Endpoint TxFIFO Depth
 */
#define USB_INEP1TXFDEP    0x0000FFFF
#define USB_INEP1TXFDEP_M  (USB_INEP1TXFDEP_V << USB_INEP1TXFDEP_S)
#define USB_INEP1TXFDEP_V  0x0000FFFF
#define USB_INEP1TXFDEP_S  16


/** USB_DIEPTXF2_REG register
 *  Device IN Endpoint Transmit FIFO Size Register
 */
#define USB_DIEPTXF2_REG (SOC_DPORT_USB_BASE + 0x108)
/** USB_INEP2TXFSTADDR : R/W; bitpos: [16:0]; default: 512;
 *  IN Endpoint FIFOn Transmit RAM Start Address.
 */
#define USB_INEP2TXFSTADDR    0x0000FFFF
#define USB_INEP2TXFSTADDR_M  (USB_INEP2TXFSTADDR_V << USB_INEP2TXFSTADDR_S)
#define USB_INEP2TXFSTADDR_V  0x0000FFFF
#define USB_INEP2TXFSTADDR_S  0
/** USB_INEP2TXFDEP : R/W; bitpos: [32:16]; default: 4096;
 *  IN Endpoint TxFIFO Depth
 */
#define USB_INEP2TXFDEP    0x0000FFFF
#define USB_INEP2TXFDEP_M  (USB_INEP2TXFDEP_V << USB_INEP2TXFDEP_S)
#define USB_INEP2TXFDEP_V  0x0000FFFF
#define USB_INEP2TXFDEP_S  16


/** USB_DIEPTXF3_REG register
 *  Device IN Endpoint Transmit FIFO Size Register
 */
#define USB_DIEPTXF3_REG (SOC_DPORT_USB_BASE + 0x10c)
/** USB_INEP3TXFSTADDR : R/W; bitpos: [16:0]; default: 512;
 *  IN Endpoint FIFOn Transmit RAM Start Address.
 */
#define USB_INEP3TXFSTADDR    0x0000FFFF
#define USB_INEP3TXFSTADDR_M  (USB_INEP3TXFSTADDR_V << USB_INEP3TXFSTADDR_S)
#define USB_INEP3TXFSTADDR_V  0x0000FFFF
#define USB_INEP3TXFSTADDR_S  0
/** USB_INEP3TXFDEP : R/W; bitpos: [32:16]; default: 4096;
 *  IN Endpoint TxFIFO Depth
 */
#define USB_INEP3TXFDEP    0x0000FFFF
#define USB_INEP3TXFDEP_M  (USB_INEP3TXFDEP_V << USB_INEP3TXFDEP_S)
#define USB_INEP3TXFDEP_V  0x0000FFFF
#define USB_INEP3TXFDEP_S  16


/** USB_DIEPTXF4_REG register
 *  Device IN Endpoint Transmit FIFO Size Register
 */
#define USB_DIEPTXF4_REG (SOC_DPORT_USB_BASE + 0x110)
/** USB_INEP4TXFSTADDR : R/W; bitpos: [16:0]; default: 512;
 *  IN Endpoint FIFOn Transmit RAM Start Address.
 */
#define USB_INEP4TXFSTADDR    0x0000FFFF
#define USB_INEP4TXFSTADDR_M  (USB_INEP4TXFSTADDR_V << USB_INEP4TXFSTADDR_S)
#define USB_INEP4TXFSTADDR_V  0x0000FFFF
#define USB_INEP4TXFSTADDR_S  0
/** USB_INEP4TXFDEP : R/W; bitpos: [32:16]; default: 4096;
 *  IN Endpoint TxFIFO Depth
 */
#define USB_INEP4TXFDEP    0x0000FFFF
#define USB_INEP4TXFDEP_M  (USB_INEP4TXFDEP_V << USB_INEP4TXFDEP_S)
#define USB_INEP4TXFDEP_V  0x0000FFFF
#define USB_INEP4TXFDEP_S  16


/** USB_HCFG_REG register
 *  Host Configuration Register
 */
#define USB_HCFG_REG (SOC_DPORT_USB_BASE + 0x400)
/** USB_H_FSLSPCLKSEL : R/W; bitpos: [2:0]; default: 0;
 *  0x0 : PHY clock is running at 30/60 MHz
 *  0x1 : PHY clock is running at 48 MHz
 *  0x2 : PHY clock is running at 6 MHz
 */
#define USB_H_FSLSPCLKSEL    0x00000003
#define USB_H_FSLSPCLKSEL_M  (USB_H_FSLSPCLKSEL_V << USB_H_FSLSPCLKSEL_S)
#define USB_H_FSLSPCLKSEL_V  0x00000003
#define USB_H_FSLSPCLKSEL_S  0
/** USB_H_FSLSSUPP : R/W; bitpos: [2]; default: 0;
 *  FS- and LS-Only Support
 *  1'b0: HS/FS/LS, based on the maximum speed supported by the connected device
 *  1'b1: FS/LS-only, even If the connected device can support HS
 */
#define USB_H_FSLSSUPP    (BIT(2))
#define USB_H_FSLSSUPP_M  (USB_H_FSLSSUPP_V << USB_H_FSLSSUPP_S)
#define USB_H_FSLSSUPP_V  0x00000001
#define USB_H_FSLSSUPP_S  2
/** USB_H_ENA32KHZS : R/W; bitpos: [7]; default: 0;
 *  1'b0:32 KHz Suspend mode disabled
 *  1'b1:32 KHz Suspend mode enabled
 */
#define USB_H_ENA32KHZS    (BIT(7))
#define USB_H_ENA32KHZS_M  (USB_H_ENA32KHZS_V << USB_H_ENA32KHZS_S)
#define USB_H_ENA32KHZS_V  0x00000001
#define USB_H_ENA32KHZS_S  7
/** USB_H_DESCDMA : R/W; bitpos: [23]; default: 0;
 *  GAHBCFG_REG.USB_DMAEN=0,HCFG_REG.USB_DESCDMA=0 => Slave mode
 *  GAHBCFG_REG.USB_DMAEN=0,HCFG_REG.USB_DESCDMA=1 => Invalid
 *  GAHBCFG_REG.USB_DMAEN=1,HCFG_REG.USB_DESCDMA=0 => Buffered DMA
 *  GAHBCFG_REG.USB_DMAEN=1,HCFG_REG.USB_DESCDMA=1 => Scatter/Gather DMA mode
 */
#define USB_H_DESCDMA    (BIT(23))
#define USB_H_DESCDMA_M  (USB_H_DESCDMA_V << USB_H_DESCDMA_S)
#define USB_H_DESCDMA_V  0x00000001
#define USB_H_DESCDMA_S  23
/** USB_H_FRLISTEN : R/W; bitpos: [26:24]; default: 0;
 *  Frame List Entries
 *  2'b00: 8 Entries
 *  2'b01: 16 Entries
 *  2'b10: 32 Entries
 *  2'b11: 64 Entries
 */
#define USB_H_FRLISTEN    0x00000003
#define USB_H_FRLISTEN_M  (USB_H_FRLISTEN_V << USB_H_FRLISTEN_S)
#define USB_H_FRLISTEN_V  0x00000003
#define USB_H_FRLISTEN_S  24
/** USB_H_PERSCHEDENA : R/W; bitpos: [26]; default: 0;
 *  0x0 (DISABLED): Disables periodic scheduling within the core
 *  0x1 (ENABLED): Enables periodic scheduling within the core
 */
#define USB_H_PERSCHEDENA    (BIT(26))
#define USB_H_PERSCHEDENA_M  (USB_H_PERSCHEDENA_V << USB_H_PERSCHEDENA_S)
#define USB_H_PERSCHEDENA_V  0x00000001
#define USB_H_PERSCHEDENA_S  26
/** USB_H_MODECHTIMEN : R/W; bitpos: [31]; default: 0;
 *  Mode Change Ready Timer Enable,
 *  1'b0 : The Host core waits for either 200 PHY clock cycles or a linestate of SE0 at
 *  the end of resume to the change the opmode from 2'b10 to 2'b00.
 *  1'b1 : The Host core waits only for a linstate of SE0 at the end of resume to
 *  change the opmode from 2'b10 to 2'b00.
 */
#define USB_H_MODECHTIMEN    (BIT(31))
#define USB_H_MODECHTIMEN_M  (USB_H_MODECHTIMEN_V << USB_H_MODECHTIMEN_S)
#define USB_H_MODECHTIMEN_V  0x00000001
#define USB_H_MODECHTIMEN_S  31


/** USB_HFIR_REG register
 *  Host Frame Interval Register
 */
#define USB_HFIR_REG (SOC_DPORT_USB_BASE + 0x404)
/** USB_FRINT : R/W; bitpos: [16:0]; default: 6103;
 *  Frame Interval .  1 ms * (PHY clock frequency for FS/LS)
 */
#define USB_FRINT    0x0000FFFF
#define USB_FRINT_M  (USB_FRINT_V << USB_FRINT_S)
#define USB_FRINT_V  0x0000FFFF
#define USB_FRINT_S  0
/** USB_HFIRRLDCTRL : R/W; bitpos: [16]; default: 0;
 *  Reload Control
 *  1'b0 : The HFIR cannot be reloaded dynamically
 *  1'b1: the HFIR can be dynamically reloaded during runtime
 */
#define USB_HFIRRLDCTRL    (BIT(16))
#define USB_HFIRRLDCTRL_M  (USB_HFIRRLDCTRL_V << USB_HFIRRLDCTRL_S)
#define USB_HFIRRLDCTRL_V  0x00000001
#define USB_HFIRRLDCTRL_S  16


/** USB_HFLBADDR_REG register
 *  Host Frame List Base Address Register
 */
#define USB_HFLBADDR_REG (SOC_DPORT_USB_BASE + 0x41c)
/** USB_HFLBADDR : R/W; bitpos: [32:0]; default: 0;
 *  The starting address of the Frame list. This register is used only for Isochronous
 *  and Interrupt Channels.
 */
#define USB_HFLBADDR    0xFFFFFFFF
#define USB_HFLBADDR_M  (USB_HFLBADDR_V << USB_HFLBADDR_S)
#define USB_HFLBADDR_V  0xFFFFFFFF
#define USB_HFLBADDR_S  0


/** USB_HPRT_REG register
 *  Host Port Control and Status Register
 */
#define USB_HPRT_REG (SOC_DPORT_USB_BASE + 0x440)
/** USB_PRTCONNSTS : RO; bitpos: [0]; default: 0;
 *  Port Connect Status
 *  0x0: No device is attached to the port
 *  0x1: A device is attached to the port
 */
#define USB_PRTCONNSTS    (BIT(0))
#define USB_PRTCONNSTS_M  (USB_PRTCONNSTS_V << USB_PRTCONNSTS_S)
#define USB_PRTCONNSTS_V  0x00000001
#define USB_PRTCONNSTS_S  0
/** USB_PRTCONNDET : R/W; bitpos: [1]; default: 0;
 *  Port Connect Detected.
 *  0x1 : Device connection detected.
 *  0x0 : No device connection detected.
 */
#define USB_PRTCONNDET    (BIT(1))
#define USB_PRTCONNDET_M  (USB_PRTCONNDET_V << USB_PRTCONNDET_S)
#define USB_PRTCONNDET_V  0x00000001
#define USB_PRTCONNDET_S  1
/** USB_PRTENA : R/W; bitpos: [2]; default: 0;
 *  Port Enable
 *  1'b0: Port disabled
 *  1'b1: Port enabled
 */
#define USB_PRTENA    (BIT(2))
#define USB_PRTENA_M  (USB_PRTENA_V << USB_PRTENA_S)
#define USB_PRTENA_V  0x00000001
#define USB_PRTENA_S  2
/** USB_PRTENCHNG : R/W; bitpos: [3]; default: 0;
 *  Port Enable/Disable Change
 *  0x0 : Port Enable bit 2 has not changed
 *  0x1 : Port Enable bit 2 changed
 */
#define USB_PRTENCHNG    (BIT(3))
#define USB_PRTENCHNG_M  (USB_PRTENCHNG_V << USB_PRTENCHNG_S)
#define USB_PRTENCHNG_V  0x00000001
#define USB_PRTENCHNG_S  3
/** USB_PRTOVRCURRACT : RO; bitpos: [4]; default: 0;
 *  Port Overcurrent Active
 *  1'b0: No overcurrent condition
 *  1'b1: Overcurrent condition
 */
#define USB_PRTOVRCURRACT    (BIT(4))
#define USB_PRTOVRCURRACT_M  (USB_PRTOVRCURRACT_V << USB_PRTOVRCURRACT_S)
#define USB_PRTOVRCURRACT_V  0x00000001
#define USB_PRTOVRCURRACT_S  4
/** USB_PRTOVRCURRCHNG : R/W; bitpos: [5]; default: 0;
 *  Port Overcurrent Change
 *  0x0: Status of port overcurrent status is not changed
 *  0x1: Status of port overcurrent changed
 */
#define USB_PRTOVRCURRCHNG    (BIT(5))
#define USB_PRTOVRCURRCHNG_M  (USB_PRTOVRCURRCHNG_V << USB_PRTOVRCURRCHNG_S)
#define USB_PRTOVRCURRCHNG_V  0x00000001
#define USB_PRTOVRCURRCHNG_S  5
/** USB_PRTRES : R/W; bitpos: [6]; default: 0;
 *  Port Resume
 *  1'b0: No resume driven
 *  1'b1: Resume driven
 */
#define USB_PRTRES    (BIT(6))
#define USB_PRTRES_M  (USB_PRTRES_V << USB_PRTRES_S)
#define USB_PRTRES_V  0x00000001
#define USB_PRTRES_S  6
/** USB_PRTSUSP : R/W; bitpos: [7]; default: 0;
 *  Port Suspend
 *  1'b0: Port not in Suspend mode
 *  1'b1: Port in Suspend mode
 */
#define USB_PRTSUSP    (BIT(7))
#define USB_PRTSUSP_M  (USB_PRTSUSP_V << USB_PRTSUSP_S)
#define USB_PRTSUSP_V  0x00000001
#define USB_PRTSUSP_S  7
/** USB_PRTRST : R/W; bitpos: [8]; default: 0;
 *  Port Reset.
 *  1'b0: Port not in reset
 *  1'b1: Port in reset
 */
#define USB_PRTRST    (BIT(8))
#define USB_PRTRST_M  (USB_PRTRST_V << USB_PRTRST_S)
#define USB_PRTRST_V  0x00000001
#define USB_PRTRST_S  8
/** USB_PRTLNSTS : RO; bitpos: [12:10]; default: 0;
 *  Port Line Status
 *  Bit [10]: Logic level of D+
 *  Bit [11]: Logic level of D-
 */
#define USB_PRTLNSTS    0x00000003
#define USB_PRTLNSTS_M  (USB_PRTLNSTS_V << USB_PRTLNSTS_S)
#define USB_PRTLNSTS_V  0x00000003
#define USB_PRTLNSTS_S  10
/** USB_PRTPWR : R/W; bitpos: [12]; default: 0;
 *  Port Power
 *  1'b0: Power off
 *  1'b1: Power on
 */
#define USB_PRTPWR    (BIT(12))
#define USB_PRTPWR_M  (USB_PRTPWR_V << USB_PRTPWR_S)
#define USB_PRTPWR_V  0x00000001
#define USB_PRTPWR_S  12
/** USB_PRTTSTCTL : R/W; bitpos: [17:13]; default: 0;
 *  Port Test Control
 *  4'b0000: Test mode disabled
 *  4'b0001: Test_J mode
 *  4'b0010: Test_K mode
 *  4'b0011: Test_SE0_NAK mode
 *  4'b0100: Test_Packet mode
 *  4'b0101: Test_Force_Enable
 *  Others: Reserved
 */
#define USB_PRTTSTCTL    0x0000000F
#define USB_PRTTSTCTL_M  (USB_PRTTSTCTL_V << USB_PRTTSTCTL_S)
#define USB_PRTTSTCTL_V  0x0000000F
#define USB_PRTTSTCTL_S  13
/** USB_PRTSPD : RO; bitpos: [19:17]; default: 0;
 *  Port Speed
 *  2'b00: High speed
 *  2'b01: Full speed
 *  2'b10: Low speed
 *  2'b11: Reserved
 */
#define USB_PRTSPD    0x00000003
#define USB_PRTSPD_M  (USB_PRTSPD_V << USB_PRTSPD_S)
#define USB_PRTSPD_V  0x00000003
#define USB_PRTSPD_S  17


/** USB_HCCHAR0_REG register
 *  Host Channel 0 Characteristics Register
 */
#define USB_HCCHAR0_REG (SOC_DPORT_USB_BASE + 0x500)
/** USB_H_MPS0 : R/W; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size.
 */
#define USB_H_MPS0    0x000007FF
#define USB_H_MPS0_M  (USB_H_MPS0_V << USB_H_MPS0_S)
#define USB_H_MPS0_V  0x000007FF
#define USB_H_MPS0_S  0
/** USB_H_EPNUM0 : R/W; bitpos: [15:11]; default: 0;
 *  Endpoint Number.
 */
#define USB_H_EPNUM0    0x0000000F
#define USB_H_EPNUM0_M  (USB_H_EPNUM0_V << USB_H_EPNUM0_S)
#define USB_H_EPNUM0_V  0x0000000F
#define USB_H_EPNUM0_S  11
/** USB_H_EPDIR0 : R/W; bitpos: [15]; default: 0;
 *  1'b0: OUT
 *  1'b1: IN
 */
#define USB_H_EPDIR0    (BIT(15))
#define USB_H_EPDIR0_M  (USB_H_EPDIR0_V << USB_H_EPDIR0_S)
#define USB_H_EPDIR0_V  0x00000001
#define USB_H_EPDIR0_S  15
/** USB_H_LSPDDEV0 : R/W; bitpos: [17]; default: 0;
 *  0x0: Not Communicating with low speed device
 *  0x1: Communicating with low speed device
 */
#define USB_H_LSPDDEV0    (BIT(17))
#define USB_H_LSPDDEV0_M  (USB_H_LSPDDEV0_V << USB_H_LSPDDEV0_S)
#define USB_H_LSPDDEV0_V  0x00000001
#define USB_H_LSPDDEV0_S  17
/** USB_H_EPTYPE0 : R/W; bitpos: [20:18]; default: 0;
 *  0x0 (CTRL): Contro
 *  0x1 (ISOC): Isochronous
 *  0x2 (BULK): Bulk
 *  0x3 (INTERR): Interrupt
 */
#define USB_H_EPTYPE0    0x00000003
#define USB_H_EPTYPE0_M  (USB_H_EPTYPE0_V << USB_H_EPTYPE0_S)
#define USB_H_EPTYPE0_V  0x00000003
#define USB_H_EPTYPE0_S  18
/** USB_H_EC0 : R/W; bitpos: [21]; default: 0;
 *  Multi Count (MC) / Error Count(EC)
 *  0x0 (RESERVED): Reserved. This field yields undefined result
 *  0x1 (TRANSONE): 1 transaction
 *  0x2 (TRANSTWO):2 transactions to be issued for this endpoint per microframe
 *  0x3 (TRANSTHREE): 3 transactions to be issued for this endpoint per microframe
 */
#define USB_H_EC0    (BIT(21))
#define USB_H_EC0_M  (USB_H_EC0_V << USB_H_EC0_S)
#define USB_H_EC0_V  0x00000001
#define USB_H_EC0_S  21
/** USB_H_DEVADDR0 : R/W; bitpos: [29:22]; default: 0;
 *  Device Address .    This field selects the specific device serving as the data
 *  source or sink.
 */
#define USB_H_DEVADDR0    0x0000007F
#define USB_H_DEVADDR0_M  (USB_H_DEVADDR0_V << USB_H_DEVADDR0_S)
#define USB_H_DEVADDR0_V  0x0000007F
#define USB_H_DEVADDR0_S  22
/** USB_H_ODDFRM0 : R/W; bitpos: [29]; default: 0;
 *  Odd Frame
 *  1'b0: Even (micro)Frame
 *  1'b1: Odd (micro)Frame
 */
#define USB_H_ODDFRM0    (BIT(29))
#define USB_H_ODDFRM0_M  (USB_H_ODDFRM0_V << USB_H_ODDFRM0_S)
#define USB_H_ODDFRM0_V  0x00000001
#define USB_H_ODDFRM0_S  29
/** USB_H_CHDIS0 : R/W; bitpos: [30]; default: 0;
 *  Channel Disable
 *  0x0 : Transmit/Recieve norma
 *  0x1 : Stop transmitting/receiving data on channel
 */
#define USB_H_CHDIS0    (BIT(30))
#define USB_H_CHDIS0_M  (USB_H_CHDIS0_V << USB_H_CHDIS0_S)
#define USB_H_CHDIS0_V  0x00000001
#define USB_H_CHDIS0_S  30
/** USB_H_CHENA0 : R/W; bitpos: [31]; default: 0;
 *  Channel Enable
 *  0x0 :If Scatter/Gather mode is enabled, indicates that the descriptor structure is
 *  not yet ready. If Scatter/Gather mode is disabled, indicates that the channel is
 *  disabled
 *  0x1 : If Scatter/Gather mode is enabled, indicates that the descriptor structure
 *  and data buffer with data is set up and this channel can access the descriptor. If
 *  Scatter/Gather mode is disabled, indicates that the channel is enabled
 */
#define USB_H_CHENA0    (BIT(31))
#define USB_H_CHENA0_M  (USB_H_CHENA0_V << USB_H_CHENA0_S)
#define USB_H_CHENA0_V  0x00000001
#define USB_H_CHENA0_S  31


/** USB_HCDMA0_REG register
 *  Host Channel 0 DMA Address Register
 */
#define USB_HCDMA0_REG (SOC_DPORT_USB_BASE + 0x514)
/** USB_H_DMAADDR0 : R/W; bitpos: [32:0]; default: 0;
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *  [31:9]: DMA Address
 *  [8:3]: Current Transfer Desc
 *  [2:0]: Reserved
 */
#define USB_H_DMAADDR0    0xFFFFFFFF
#define USB_H_DMAADDR0_M  (USB_H_DMAADDR0_V << USB_H_DMAADDR0_S)
#define USB_H_DMAADDR0_V  0xFFFFFFFF
#define USB_H_DMAADDR0_S  0


/** USB_HCCHAR1_REG register
 *  Host Channel 1 Characteristics Register
 */
#define USB_HCCHAR1_REG (SOC_DPORT_USB_BASE + 0x520)
/** USB_H_MPS1 : R/W; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size.
 */
#define USB_H_MPS1    0x000007FF
#define USB_H_MPS1_M  (USB_H_MPS1_V << USB_H_MPS1_S)
#define USB_H_MPS1_V  0x000007FF
#define USB_H_MPS1_S  0
/** USB_H_EPNUM1 : R/W; bitpos: [15:11]; default: 0;
 *  Endpoint Number.
 */
#define USB_H_EPNUM1    0x0000000F
#define USB_H_EPNUM1_M  (USB_H_EPNUM1_V << USB_H_EPNUM1_S)
#define USB_H_EPNUM1_V  0x0000000F
#define USB_H_EPNUM1_S  11
/** USB_H_EPDIR1 : R/W; bitpos: [15]; default: 0;
 *  1'b0: OUT
 *  1'b1: IN
 */
#define USB_H_EPDIR1    (BIT(15))
#define USB_H_EPDIR1_M  (USB_H_EPDIR1_V << USB_H_EPDIR1_S)
#define USB_H_EPDIR1_V  0x00000001
#define USB_H_EPDIR1_S  15
/** USB_H_LSPDDEV1 : R/W; bitpos: [17]; default: 0;
 *  0x0: Not Communicating with low speed device
 *  0x1: Communicating with low speed device
 */
#define USB_H_LSPDDEV1    (BIT(17))
#define USB_H_LSPDDEV1_M  (USB_H_LSPDDEV1_V << USB_H_LSPDDEV1_S)
#define USB_H_LSPDDEV1_V  0x00000001
#define USB_H_LSPDDEV1_S  17
/** USB_H_EPTYPE1 : R/W; bitpos: [20:18]; default: 0;
 *  0x0 (CTRL): Contro
 *  0x1 (ISOC): Isochronous
 *  0x2 (BULK): Bulk
 *  0x3 (INTERR): Interrupt
 */
#define USB_H_EPTYPE1    0x00000003
#define USB_H_EPTYPE1_M  (USB_H_EPTYPE1_V << USB_H_EPTYPE1_S)
#define USB_H_EPTYPE1_V  0x00000003
#define USB_H_EPTYPE1_S  18
/** USB_H_EC1 : R/W; bitpos: [21]; default: 0;
 *  Multi Count (MC) / Error Count(EC)
 *  0x0 (RESERVED): Reserved. This field yields undefined result
 *  0x1 (TRANSONE): 1 transaction
 *  0x2 (TRANSTWO):2 transactions to be issued for this endpoint per microframe
 *  0x3 (TRANSTHREE): 3 transactions to be issued for this endpoint per microframe
 */
#define USB_H_EC1    (BIT(21))
#define USB_H_EC1_M  (USB_H_EC1_V << USB_H_EC1_S)
#define USB_H_EC1_V  0x00000001
#define USB_H_EC1_S  21
/** USB_H_DEVADDR1 : R/W; bitpos: [29:22]; default: 0;
 *  Device Address .    This field selects the specific device serving as the data
 *  source or sink.
 */
#define USB_H_DEVADDR1    0x0000007F
#define USB_H_DEVADDR1_M  (USB_H_DEVADDR1_V << USB_H_DEVADDR1_S)
#define USB_H_DEVADDR1_V  0x0000007F
#define USB_H_DEVADDR1_S  22
/** USB_H_ODDFRM1 : R/W; bitpos: [29]; default: 0;
 *  Odd Frame
 *  1'b0: Even (micro)Frame
 *  1'b1: Odd (micro)Frame
 */
#define USB_H_ODDFRM1    (BIT(29))
#define USB_H_ODDFRM1_M  (USB_H_ODDFRM1_V << USB_H_ODDFRM1_S)
#define USB_H_ODDFRM1_V  0x00000001
#define USB_H_ODDFRM1_S  29
/** USB_H_CHDIS1 : R/W; bitpos: [30]; default: 0;
 *  Channel Disable
 *  0x0 : Transmit/Recieve norma
 *  0x1 : Stop transmitting/receiving data on channel
 */
#define USB_H_CHDIS1    (BIT(30))
#define USB_H_CHDIS1_M  (USB_H_CHDIS1_V << USB_H_CHDIS1_S)
#define USB_H_CHDIS1_V  0x00000001
#define USB_H_CHDIS1_S  30
/** USB_H_CHENA1 : R/W; bitpos: [31]; default: 0;
 *  Channel Enable
 *  0x0 :If Scatter/Gather mode is enabled, indicates that the descriptor structure is
 *  not yet ready. If Scatter/Gather mode is disabled, indicates that the channel is
 *  disabled
 *  0x1 : If Scatter/Gather mode is enabled, indicates that the descriptor structure
 *  and data buffer with data is set up and this channel can access the descriptor. If
 *  Scatter/Gather mode is disabled, indicates that the channel is enabled
 */
#define USB_H_CHENA1    (BIT(31))
#define USB_H_CHENA1_M  (USB_H_CHENA1_V << USB_H_CHENA1_S)
#define USB_H_CHENA1_V  0x00000001
#define USB_H_CHENA1_S  31


/** USB_HCDMA1_REG register
 *  Host Channel 1 DMA Address Register
 */
#define USB_HCDMA1_REG (SOC_DPORT_USB_BASE + 0x534)
/** USB_H_DMAADDR1 : R/W; bitpos: [32:0]; default: 0;
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *  [31:9]: DMA Address
 *  [8:3]: Current Transfer Desc
 *  [2:0]: Reserved
 */
#define USB_H_DMAADDR1    0xFFFFFFFF
#define USB_H_DMAADDR1_M  (USB_H_DMAADDR1_V << USB_H_DMAADDR1_S)
#define USB_H_DMAADDR1_V  0xFFFFFFFF
#define USB_H_DMAADDR1_S  0


/** USB_HCCHAR2_REG register
 *  Host Channel 2 Characteristics Register
 */
#define USB_HCCHAR2_REG (SOC_DPORT_USB_BASE + 0x540)
/** USB_H_MPS2 : R/W; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size.
 */
#define USB_H_MPS2    0x000007FF
#define USB_H_MPS2_M  (USB_H_MPS2_V << USB_H_MPS2_S)
#define USB_H_MPS2_V  0x000007FF
#define USB_H_MPS2_S  0
/** USB_H_EPNUM2 : R/W; bitpos: [15:11]; default: 0;
 *  Endpoint Number.
 */
#define USB_H_EPNUM2    0x0000000F
#define USB_H_EPNUM2_M  (USB_H_EPNUM2_V << USB_H_EPNUM2_S)
#define USB_H_EPNUM2_V  0x0000000F
#define USB_H_EPNUM2_S  11
/** USB_H_EPDIR2 : R/W; bitpos: [15]; default: 0;
 *  1'b0: OUT
 *  1'b1: IN
 */
#define USB_H_EPDIR2    (BIT(15))
#define USB_H_EPDIR2_M  (USB_H_EPDIR2_V << USB_H_EPDIR2_S)
#define USB_H_EPDIR2_V  0x00000001
#define USB_H_EPDIR2_S  15
/** USB_H_LSPDDEV2 : R/W; bitpos: [17]; default: 0;
 *  0x0: Not Communicating with low speed device
 *  0x1: Communicating with low speed device
 */
#define USB_H_LSPDDEV2    (BIT(17))
#define USB_H_LSPDDEV2_M  (USB_H_LSPDDEV2_V << USB_H_LSPDDEV2_S)
#define USB_H_LSPDDEV2_V  0x00000001
#define USB_H_LSPDDEV2_S  17
/** USB_H_EPTYPE2 : R/W; bitpos: [20:18]; default: 0;
 *  0x0 (CTRL): Contro
 *  0x1 (ISOC): Isochronous
 *  0x2 (BULK): Bulk
 *  0x3 (INTERR): Interrupt
 */
#define USB_H_EPTYPE2    0x00000003
#define USB_H_EPTYPE2_M  (USB_H_EPTYPE2_V << USB_H_EPTYPE2_S)
#define USB_H_EPTYPE2_V  0x00000003
#define USB_H_EPTYPE2_S  18
/** USB_H_EC2 : R/W; bitpos: [21]; default: 0;
 *  Multi Count (MC) / Error Count(EC)
 *  0x0 (RESERVED): Reserved. This field yields undefined result
 *  0x1 (TRANSONE): 1 transaction
 *  0x2 (TRANSTWO):2 transactions to be issued for this endpoint per microframe
 *  0x3 (TRANSTHREE): 3 transactions to be issued for this endpoint per microframe
 */
#define USB_H_EC2    (BIT(21))
#define USB_H_EC2_M  (USB_H_EC2_V << USB_H_EC2_S)
#define USB_H_EC2_V  0x00000001
#define USB_H_EC2_S  21
/** USB_H_DEVADDR2 : R/W; bitpos: [29:22]; default: 0;
 *  Device Address .    This field selects the specific device serving as the data
 *  source or sink.
 */
#define USB_H_DEVADDR2    0x0000007F
#define USB_H_DEVADDR2_M  (USB_H_DEVADDR2_V << USB_H_DEVADDR2_S)
#define USB_H_DEVADDR2_V  0x0000007F
#define USB_H_DEVADDR2_S  22
/** USB_H_ODDFRM2 : R/W; bitpos: [29]; default: 0;
 *  Odd Frame
 *  1'b0: Even (micro)Frame
 *  1'b1: Odd (micro)Frame
 */
#define USB_H_ODDFRM2    (BIT(29))
#define USB_H_ODDFRM2_M  (USB_H_ODDFRM2_V << USB_H_ODDFRM2_S)
#define USB_H_ODDFRM2_V  0x00000001
#define USB_H_ODDFRM2_S  29
/** USB_H_CHDIS2 : R/W; bitpos: [30]; default: 0;
 *  Channel Disable
 *  0x0 : Transmit/Recieve norma
 *  0x1 : Stop transmitting/receiving data on channel
 */
#define USB_H_CHDIS2    (BIT(30))
#define USB_H_CHDIS2_M  (USB_H_CHDIS2_V << USB_H_CHDIS2_S)
#define USB_H_CHDIS2_V  0x00000001
#define USB_H_CHDIS2_S  30
/** USB_H_CHENA2 : R/W; bitpos: [31]; default: 0;
 *  Channel Enable
 *  0x0 :If Scatter/Gather mode is enabled, indicates that the descriptor structure is
 *  not yet ready. If Scatter/Gather mode is disabled, indicates that the channel is
 *  disabled
 *  0x1 : If Scatter/Gather mode is enabled, indicates that the descriptor structure
 *  and data buffer with data is set up and this channel can access the descriptor. If
 *  Scatter/Gather mode is disabled, indicates that the channel is enabled
 */
#define USB_H_CHENA2    (BIT(31))
#define USB_H_CHENA2_M  (USB_H_CHENA2_V << USB_H_CHENA2_S)
#define USB_H_CHENA2_V  0x00000001
#define USB_H_CHENA2_S  31


/** USB_HCDMA2_REG register
 *  Host Channel 2 DMA Address Register
 */
#define USB_HCDMA2_REG (SOC_DPORT_USB_BASE + 0x554)
/** USB_H_DMAADDR2 : R/W; bitpos: [32:0]; default: 0;
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *  [31:9]: DMA Address
 *  [8:3]: Current Transfer Desc
 *  [2:0]: Reserved
 */
#define USB_H_DMAADDR2    0xFFFFFFFF
#define USB_H_DMAADDR2_M  (USB_H_DMAADDR2_V << USB_H_DMAADDR2_S)
#define USB_H_DMAADDR2_V  0xFFFFFFFF
#define USB_H_DMAADDR2_S  0


/** USB_HCCHAR3_REG register
 *  Host Channel 3 Characteristics Register
 */
#define USB_HCCHAR3_REG (SOC_DPORT_USB_BASE + 0x560)
/** USB_H_MPS3 : R/W; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size.
 */
#define USB_H_MPS3    0x000007FF
#define USB_H_MPS3_M  (USB_H_MPS3_V << USB_H_MPS3_S)
#define USB_H_MPS3_V  0x000007FF
#define USB_H_MPS3_S  0
/** USB_H_EPNUM3 : R/W; bitpos: [15:11]; default: 0;
 *  Endpoint Number.
 */
#define USB_H_EPNUM3    0x0000000F
#define USB_H_EPNUM3_M  (USB_H_EPNUM3_V << USB_H_EPNUM3_S)
#define USB_H_EPNUM3_V  0x0000000F
#define USB_H_EPNUM3_S  11
/** USB_H_EPDIR3 : R/W; bitpos: [15]; default: 0;
 *  1'b0: OUT
 *  1'b1: IN
 */
#define USB_H_EPDIR3    (BIT(15))
#define USB_H_EPDIR3_M  (USB_H_EPDIR3_V << USB_H_EPDIR3_S)
#define USB_H_EPDIR3_V  0x00000001
#define USB_H_EPDIR3_S  15
/** USB_H_LSPDDEV3 : R/W; bitpos: [17]; default: 0;
 *  0x0: Not Communicating with low speed device
 *  0x1: Communicating with low speed device
 */
#define USB_H_LSPDDEV3    (BIT(17))
#define USB_H_LSPDDEV3_M  (USB_H_LSPDDEV3_V << USB_H_LSPDDEV3_S)
#define USB_H_LSPDDEV3_V  0x00000001
#define USB_H_LSPDDEV3_S  17
/** USB_H_EPTYPE3 : R/W; bitpos: [20:18]; default: 0;
 *  0x0 (CTRL): Contro
 *  0x1 (ISOC): Isochronous
 *  0x2 (BULK): Bulk
 *  0x3 (INTERR): Interrupt
 */
#define USB_H_EPTYPE3    0x00000003
#define USB_H_EPTYPE3_M  (USB_H_EPTYPE3_V << USB_H_EPTYPE3_S)
#define USB_H_EPTYPE3_V  0x00000003
#define USB_H_EPTYPE3_S  18
/** USB_H_EC3 : R/W; bitpos: [21]; default: 0;
 *  Multi Count (MC) / Error Count(EC)
 *  0x0 (RESERVED): Reserved. This field yields undefined result
 *  0x1 (TRANSONE): 1 transaction
 *  0x2 (TRANSTWO):2 transactions to be issued for this endpoint per microframe
 *  0x3 (TRANSTHREE): 3 transactions to be issued for this endpoint per microframe
 */
#define USB_H_EC3    (BIT(21))
#define USB_H_EC3_M  (USB_H_EC3_V << USB_H_EC3_S)
#define USB_H_EC3_V  0x00000001
#define USB_H_EC3_S  21
/** USB_H_DEVADDR3 : R/W; bitpos: [29:22]; default: 0;
 *  Device Address .    This field selects the specific device serving as the data
 *  source or sink.
 */
#define USB_H_DEVADDR3    0x0000007F
#define USB_H_DEVADDR3_M  (USB_H_DEVADDR3_V << USB_H_DEVADDR3_S)
#define USB_H_DEVADDR3_V  0x0000007F
#define USB_H_DEVADDR3_S  22
/** USB_H_ODDFRM3 : R/W; bitpos: [29]; default: 0;
 *  Odd Frame
 *  1'b0: Even (micro)Frame
 *  1'b1: Odd (micro)Frame
 */
#define USB_H_ODDFRM3    (BIT(29))
#define USB_H_ODDFRM3_M  (USB_H_ODDFRM3_V << USB_H_ODDFRM3_S)
#define USB_H_ODDFRM3_V  0x00000001
#define USB_H_ODDFRM3_S  29
/** USB_H_CHDIS3 : R/W; bitpos: [30]; default: 0;
 *  Channel Disable
 *  0x0 : Transmit/Recieve norma
 *  0x1 : Stop transmitting/receiving data on channel
 */
#define USB_H_CHDIS3    (BIT(30))
#define USB_H_CHDIS3_M  (USB_H_CHDIS3_V << USB_H_CHDIS3_S)
#define USB_H_CHDIS3_V  0x00000001
#define USB_H_CHDIS3_S  30
/** USB_H_CHENA3 : R/W; bitpos: [31]; default: 0;
 *  Channel Enable
 *  0x0 :If Scatter/Gather mode is enabled, indicates that the descriptor structure is
 *  not yet ready. If Scatter/Gather mode is disabled, indicates that the channel is
 *  disabled
 *  0x1 : If Scatter/Gather mode is enabled, indicates that the descriptor structure
 *  and data buffer with data is set up and this channel can access the descriptor. If
 *  Scatter/Gather mode is disabled, indicates that the channel is enabled
 */
#define USB_H_CHENA3    (BIT(31))
#define USB_H_CHENA3_M  (USB_H_CHENA3_V << USB_H_CHENA3_S)
#define USB_H_CHENA3_V  0x00000001
#define USB_H_CHENA3_S  31


/** USB_HCDMA3_REG register
 *  Host Channel 3 DMA Address Register
 */
#define USB_HCDMA3_REG (SOC_DPORT_USB_BASE + 0x574)
/** USB_H_DMAADDR3 : R/W; bitpos: [32:0]; default: 0;
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *  [31:9]: DMA Address
 *  [8:3]: Current Transfer Desc
 *  [2:0]: Reserved
 */
#define USB_H_DMAADDR3    0xFFFFFFFF
#define USB_H_DMAADDR3_M  (USB_H_DMAADDR3_V << USB_H_DMAADDR3_S)
#define USB_H_DMAADDR3_V  0xFFFFFFFF
#define USB_H_DMAADDR3_S  0


/** USB_HCCHAR4_REG register
 *  Host Channel 4 Characteristics Register
 */
#define USB_HCCHAR4_REG (SOC_DPORT_USB_BASE + 0x580)
/** USB_H_MPS4 : R/W; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size.
 */
#define USB_H_MPS4    0x000007FF
#define USB_H_MPS4_M  (USB_H_MPS4_V << USB_H_MPS4_S)
#define USB_H_MPS4_V  0x000007FF
#define USB_H_MPS4_S  0
/** USB_H_EPNUM4 : R/W; bitpos: [15:11]; default: 0;
 *  Endpoint Number.
 */
#define USB_H_EPNUM4    0x0000000F
#define USB_H_EPNUM4_M  (USB_H_EPNUM4_V << USB_H_EPNUM4_S)
#define USB_H_EPNUM4_V  0x0000000F
#define USB_H_EPNUM4_S  11
/** USB_H_EPDIR4 : R/W; bitpos: [15]; default: 0;
 *  1'b0: OUT
 *  1'b1: IN
 */
#define USB_H_EPDIR4    (BIT(15))
#define USB_H_EPDIR4_M  (USB_H_EPDIR4_V << USB_H_EPDIR4_S)
#define USB_H_EPDIR4_V  0x00000001
#define USB_H_EPDIR4_S  15
/** USB_H_LSPDDEV4 : R/W; bitpos: [17]; default: 0;
 *  0x0: Not Communicating with low speed device
 *  0x1: Communicating with low speed device
 */
#define USB_H_LSPDDEV4    (BIT(17))
#define USB_H_LSPDDEV4_M  (USB_H_LSPDDEV4_V << USB_H_LSPDDEV4_S)
#define USB_H_LSPDDEV4_V  0x00000001
#define USB_H_LSPDDEV4_S  17
/** USB_H_EPTYPE4 : R/W; bitpos: [20:18]; default: 0;
 *  0x0 (CTRL): Contro
 *  0x1 (ISOC): Isochronous
 *  0x2 (BULK): Bulk
 *  0x3 (INTERR): Interrupt
 */
#define USB_H_EPTYPE4    0x00000003
#define USB_H_EPTYPE4_M  (USB_H_EPTYPE4_V << USB_H_EPTYPE4_S)
#define USB_H_EPTYPE4_V  0x00000003
#define USB_H_EPTYPE4_S  18
/** USB_H_EC4 : R/W; bitpos: [21]; default: 0;
 *  Multi Count (MC) / Error Count(EC)
 *  0x0 (RESERVED): Reserved. This field yields undefined result
 *  0x1 (TRANSONE): 1 transaction
 *  0x2 (TRANSTWO):2 transactions to be issued for this endpoint per microframe
 *  0x3 (TRANSTHREE): 3 transactions to be issued for this endpoint per microframe
 */
#define USB_H_EC4    (BIT(21))
#define USB_H_EC4_M  (USB_H_EC4_V << USB_H_EC4_S)
#define USB_H_EC4_V  0x00000001
#define USB_H_EC4_S  21
/** USB_H_DEVADDR4 : R/W; bitpos: [29:22]; default: 0;
 *  Device Address .    This field selects the specific device serving as the data
 *  source or sink.
 */
#define USB_H_DEVADDR4    0x0000007F
#define USB_H_DEVADDR4_M  (USB_H_DEVADDR4_V << USB_H_DEVADDR4_S)
#define USB_H_DEVADDR4_V  0x0000007F
#define USB_H_DEVADDR4_S  22
/** USB_H_ODDFRM4 : R/W; bitpos: [29]; default: 0;
 *  Odd Frame
 *  1'b0: Even (micro)Frame
 *  1'b1: Odd (micro)Frame
 */
#define USB_H_ODDFRM4    (BIT(29))
#define USB_H_ODDFRM4_M  (USB_H_ODDFRM4_V << USB_H_ODDFRM4_S)
#define USB_H_ODDFRM4_V  0x00000001
#define USB_H_ODDFRM4_S  29
/** USB_H_CHDIS4 : R/W; bitpos: [30]; default: 0;
 *  Channel Disable
 *  0x0 : Transmit/Recieve norma
 *  0x1 : Stop transmitting/receiving data on channel
 */
#define USB_H_CHDIS4    (BIT(30))
#define USB_H_CHDIS4_M  (USB_H_CHDIS4_V << USB_H_CHDIS4_S)
#define USB_H_CHDIS4_V  0x00000001
#define USB_H_CHDIS4_S  30
/** USB_H_CHENA4 : R/W; bitpos: [31]; default: 0;
 *  Channel Enable
 *  0x0 :If Scatter/Gather mode is enabled, indicates that the descriptor structure is
 *  not yet ready. If Scatter/Gather mode is disabled, indicates that the channel is
 *  disabled
 *  0x1 : If Scatter/Gather mode is enabled, indicates that the descriptor structure
 *  and data buffer with data is set up and this channel can access the descriptor. If
 *  Scatter/Gather mode is disabled, indicates that the channel is enabled
 */
#define USB_H_CHENA4    (BIT(31))
#define USB_H_CHENA4_M  (USB_H_CHENA4_V << USB_H_CHENA4_S)
#define USB_H_CHENA4_V  0x00000001
#define USB_H_CHENA4_S  31


/** USB_HCDMA4_REG register
 *  Host Channel 4 DMA Address Register
 */
#define USB_HCDMA4_REG (SOC_DPORT_USB_BASE + 0x594)
/** USB_H_DMAADDR4 : R/W; bitpos: [32:0]; default: 0;
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *  [31:9]: DMA Address
 *  [8:3]: Current Transfer Desc
 *  [2:0]: Reserved
 */
#define USB_H_DMAADDR4    0xFFFFFFFF
#define USB_H_DMAADDR4_M  (USB_H_DMAADDR4_V << USB_H_DMAADDR4_S)
#define USB_H_DMAADDR4_V  0xFFFFFFFF
#define USB_H_DMAADDR4_S  0


/** USB_HCCHAR5_REG register
 *  Host Channel 5 Characteristics Register
 */
#define USB_HCCHAR5_REG (SOC_DPORT_USB_BASE + 0x5a0)
/** USB_H_MPS5 : R/W; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size.
 */
#define USB_H_MPS5    0x000007FF
#define USB_H_MPS5_M  (USB_H_MPS5_V << USB_H_MPS5_S)
#define USB_H_MPS5_V  0x000007FF
#define USB_H_MPS5_S  0
/** USB_H_EPNUM5 : R/W; bitpos: [15:11]; default: 0;
 *  Endpoint Number.
 */
#define USB_H_EPNUM5    0x0000000F
#define USB_H_EPNUM5_M  (USB_H_EPNUM5_V << USB_H_EPNUM5_S)
#define USB_H_EPNUM5_V  0x0000000F
#define USB_H_EPNUM5_S  11
/** USB_H_EPDIR5 : R/W; bitpos: [15]; default: 0;
 *  1'b0: OUT
 *  1'b1: IN
 */
#define USB_H_EPDIR5    (BIT(15))
#define USB_H_EPDIR5_M  (USB_H_EPDIR5_V << USB_H_EPDIR5_S)
#define USB_H_EPDIR5_V  0x00000001
#define USB_H_EPDIR5_S  15
/** USB_H_LSPDDEV5 : R/W; bitpos: [17]; default: 0;
 *  0x0: Not Communicating with low speed device
 *  0x1: Communicating with low speed device
 */
#define USB_H_LSPDDEV5    (BIT(17))
#define USB_H_LSPDDEV5_M  (USB_H_LSPDDEV5_V << USB_H_LSPDDEV5_S)
#define USB_H_LSPDDEV5_V  0x00000001
#define USB_H_LSPDDEV5_S  17
/** USB_H_EPTYPE5 : R/W; bitpos: [20:18]; default: 0;
 *  0x0 (CTRL): Contro
 *  0x1 (ISOC): Isochronous
 *  0x2 (BULK): Bulk
 *  0x3 (INTERR): Interrupt
 */
#define USB_H_EPTYPE5    0x00000003
#define USB_H_EPTYPE5_M  (USB_H_EPTYPE5_V << USB_H_EPTYPE5_S)
#define USB_H_EPTYPE5_V  0x00000003
#define USB_H_EPTYPE5_S  18
/** USB_H_EC5 : R/W; bitpos: [21]; default: 0;
 *  Multi Count (MC) / Error Count(EC)
 *  0x0 (RESERVED): Reserved. This field yields undefined result
 *  0x1 (TRANSONE): 1 transaction
 *  0x2 (TRANSTWO):2 transactions to be issued for this endpoint per microframe
 *  0x3 (TRANSTHREE): 3 transactions to be issued for this endpoint per microframe
 */
#define USB_H_EC5    (BIT(21))
#define USB_H_EC5_M  (USB_H_EC5_V << USB_H_EC5_S)
#define USB_H_EC5_V  0x00000001
#define USB_H_EC5_S  21
/** USB_H_DEVADDR5 : R/W; bitpos: [29:22]; default: 0;
 *  Device Address .    This field selects the specific device serving as the data
 *  source or sink.
 */
#define USB_H_DEVADDR5    0x0000007F
#define USB_H_DEVADDR5_M  (USB_H_DEVADDR5_V << USB_H_DEVADDR5_S)
#define USB_H_DEVADDR5_V  0x0000007F
#define USB_H_DEVADDR5_S  22
/** USB_H_ODDFRM5 : R/W; bitpos: [29]; default: 0;
 *  Odd Frame
 *  1'b0: Even (micro)Frame
 *  1'b1: Odd (micro)Frame
 */
#define USB_H_ODDFRM5    (BIT(29))
#define USB_H_ODDFRM5_M  (USB_H_ODDFRM5_V << USB_H_ODDFRM5_S)
#define USB_H_ODDFRM5_V  0x00000001
#define USB_H_ODDFRM5_S  29
/** USB_H_CHDIS5 : R/W; bitpos: [30]; default: 0;
 *  Channel Disable
 *  0x0 : Transmit/Recieve norma
 *  0x1 : Stop transmitting/receiving data on channel
 */
#define USB_H_CHDIS5    (BIT(30))
#define USB_H_CHDIS5_M  (USB_H_CHDIS5_V << USB_H_CHDIS5_S)
#define USB_H_CHDIS5_V  0x00000001
#define USB_H_CHDIS5_S  30
/** USB_H_CHENA5 : R/W; bitpos: [31]; default: 0;
 *  Channel Enable
 *  0x0 :If Scatter/Gather mode is enabled, indicates that the descriptor structure is
 *  not yet ready. If Scatter/Gather mode is disabled, indicates that the channel is
 *  disabled
 *  0x1 : If Scatter/Gather mode is enabled, indicates that the descriptor structure
 *  and data buffer with data is set up and this channel can access the descriptor. If
 *  Scatter/Gather mode is disabled, indicates that the channel is enabled
 */
#define USB_H_CHENA5    (BIT(31))
#define USB_H_CHENA5_M  (USB_H_CHENA5_V << USB_H_CHENA5_S)
#define USB_H_CHENA5_V  0x00000001
#define USB_H_CHENA5_S  31


/** USB_HCDMA5_REG register
 *  Host Channel 5 DMA Address Register
 */
#define USB_HCDMA5_REG (SOC_DPORT_USB_BASE + 0x5b4)
/** USB_H_DMAADDR5 : R/W; bitpos: [32:0]; default: 0;
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *  [31:9]: DMA Address
 *  [8:3]: Current Transfer Desc
 *  [2:0]: Reserved
 */
#define USB_H_DMAADDR5    0xFFFFFFFF
#define USB_H_DMAADDR5_M  (USB_H_DMAADDR5_V << USB_H_DMAADDR5_S)
#define USB_H_DMAADDR5_V  0xFFFFFFFF
#define USB_H_DMAADDR5_S  0


/** USB_HCCHAR6_REG register
 *  Host Channel 6 Characteristics Register
 */
#define USB_HCCHAR6_REG (SOC_DPORT_USB_BASE + 0x5c0)
/** USB_H_MPS6 : R/W; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size.
 */
#define USB_H_MPS6    0x000007FF
#define USB_H_MPS6_M  (USB_H_MPS6_V << USB_H_MPS6_S)
#define USB_H_MPS6_V  0x000007FF
#define USB_H_MPS6_S  0
/** USB_H_EPNUM6 : R/W; bitpos: [15:11]; default: 0;
 *  Endpoint Number.
 */
#define USB_H_EPNUM6    0x0000000F
#define USB_H_EPNUM6_M  (USB_H_EPNUM6_V << USB_H_EPNUM6_S)
#define USB_H_EPNUM6_V  0x0000000F
#define USB_H_EPNUM6_S  11
/** USB_H_EPDIR6 : R/W; bitpos: [15]; default: 0;
 *  1'b0: OUT
 *  1'b1: IN
 */
#define USB_H_EPDIR6    (BIT(15))
#define USB_H_EPDIR6_M  (USB_H_EPDIR6_V << USB_H_EPDIR6_S)
#define USB_H_EPDIR6_V  0x00000001
#define USB_H_EPDIR6_S  15
/** USB_H_LSPDDEV6 : R/W; bitpos: [17]; default: 0;
 *  0x0: Not Communicating with low speed device
 *  0x1: Communicating with low speed device
 */
#define USB_H_LSPDDEV6    (BIT(17))
#define USB_H_LSPDDEV6_M  (USB_H_LSPDDEV6_V << USB_H_LSPDDEV6_S)
#define USB_H_LSPDDEV6_V  0x00000001
#define USB_H_LSPDDEV6_S  17
/** USB_H_EPTYPE6 : R/W; bitpos: [20:18]; default: 0;
 *  0x0 (CTRL): Contro
 *  0x1 (ISOC): Isochronous
 *  0x2 (BULK): Bulk
 *  0x3 (INTERR): Interrupt
 */
#define USB_H_EPTYPE6    0x00000003
#define USB_H_EPTYPE6_M  (USB_H_EPTYPE6_V << USB_H_EPTYPE6_S)
#define USB_H_EPTYPE6_V  0x00000003
#define USB_H_EPTYPE6_S  18
/** USB_H_EC6 : R/W; bitpos: [21]; default: 0;
 *  Multi Count (MC) / Error Count(EC)
 *  0x0 (RESERVED): Reserved. This field yields undefined result
 *  0x1 (TRANSONE): 1 transaction
 *  0x2 (TRANSTWO):2 transactions to be issued for this endpoint per microframe
 *  0x3 (TRANSTHREE): 3 transactions to be issued for this endpoint per microframe
 */
#define USB_H_EC6    (BIT(21))
#define USB_H_EC6_M  (USB_H_EC6_V << USB_H_EC6_S)
#define USB_H_EC6_V  0x00000001
#define USB_H_EC6_S  21
/** USB_H_DEVADDR6 : R/W; bitpos: [29:22]; default: 0;
 *  Device Address .    This field selects the specific device serving as the data
 *  source or sink.
 */
#define USB_H_DEVADDR6    0x0000007F
#define USB_H_DEVADDR6_M  (USB_H_DEVADDR6_V << USB_H_DEVADDR6_S)
#define USB_H_DEVADDR6_V  0x0000007F
#define USB_H_DEVADDR6_S  22
/** USB_H_ODDFRM6 : R/W; bitpos: [29]; default: 0;
 *  Odd Frame
 *  1'b0: Even (micro)Frame
 *  1'b1: Odd (micro)Frame
 */
#define USB_H_ODDFRM6    (BIT(29))
#define USB_H_ODDFRM6_M  (USB_H_ODDFRM6_V << USB_H_ODDFRM6_S)
#define USB_H_ODDFRM6_V  0x00000001
#define USB_H_ODDFRM6_S  29
/** USB_H_CHDIS6 : R/W; bitpos: [30]; default: 0;
 *  Channel Disable
 *  0x0 : Transmit/Recieve norma
 *  0x1 : Stop transmitting/receiving data on channel
 */
#define USB_H_CHDIS6    (BIT(30))
#define USB_H_CHDIS6_M  (USB_H_CHDIS6_V << USB_H_CHDIS6_S)
#define USB_H_CHDIS6_V  0x00000001
#define USB_H_CHDIS6_S  30
/** USB_H_CHENA6 : R/W; bitpos: [31]; default: 0;
 *  Channel Enable
 *  0x0 :If Scatter/Gather mode is enabled, indicates that the descriptor structure is
 *  not yet ready. If Scatter/Gather mode is disabled, indicates that the channel is
 *  disabled
 *  0x1 : If Scatter/Gather mode is enabled, indicates that the descriptor structure
 *  and data buffer with data is set up and this channel can access the descriptor. If
 *  Scatter/Gather mode is disabled, indicates that the channel is enabled
 */
#define USB_H_CHENA6    (BIT(31))
#define USB_H_CHENA6_M  (USB_H_CHENA6_V << USB_H_CHENA6_S)
#define USB_H_CHENA6_V  0x00000001
#define USB_H_CHENA6_S  31


/** USB_HCDMA6_REG register
 *  Host Channel 6 DMA Address Register
 */
#define USB_HCDMA6_REG (SOC_DPORT_USB_BASE + 0x5d4)
/** USB_H_DMAADDR6 : R/W; bitpos: [32:0]; default: 0;
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *  [31:9]: DMA Address
 *  [8:3]: Current Transfer Desc
 *  [2:0]: Reserved
 */
#define USB_H_DMAADDR6    0xFFFFFFFF
#define USB_H_DMAADDR6_M  (USB_H_DMAADDR6_V << USB_H_DMAADDR6_S)
#define USB_H_DMAADDR6_V  0xFFFFFFFF
#define USB_H_DMAADDR6_S  0


/** USB_HCCHAR7_REG register
 *  Host Channel 7 Characteristics Register
 */
#define USB_HCCHAR7_REG (SOC_DPORT_USB_BASE + 0x5e0)
/** USB_H_MPS7 : R/W; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size.
 */
#define USB_H_MPS7    0x000007FF
#define USB_H_MPS7_M  (USB_H_MPS7_V << USB_H_MPS7_S)
#define USB_H_MPS7_V  0x000007FF
#define USB_H_MPS7_S  0
/** USB_H_EPNUM7 : R/W; bitpos: [15:11]; default: 0;
 *  Endpoint Number.
 */
#define USB_H_EPNUM7    0x0000000F
#define USB_H_EPNUM7_M  (USB_H_EPNUM7_V << USB_H_EPNUM7_S)
#define USB_H_EPNUM7_V  0x0000000F
#define USB_H_EPNUM7_S  11
/** USB_H_EPDIR7 : R/W; bitpos: [15]; default: 0;
 *  1'b0: OUT
 *  1'b1: IN
 */
#define USB_H_EPDIR7    (BIT(15))
#define USB_H_EPDIR7_M  (USB_H_EPDIR7_V << USB_H_EPDIR7_S)
#define USB_H_EPDIR7_V  0x00000001
#define USB_H_EPDIR7_S  15
/** USB_H_LSPDDEV7 : R/W; bitpos: [17]; default: 0;
 *  0x0: Not Communicating with low speed device
 *  0x1: Communicating with low speed device
 */
#define USB_H_LSPDDEV7    (BIT(17))
#define USB_H_LSPDDEV7_M  (USB_H_LSPDDEV7_V << USB_H_LSPDDEV7_S)
#define USB_H_LSPDDEV7_V  0x00000001
#define USB_H_LSPDDEV7_S  17
/** USB_H_EPTYPE7 : R/W; bitpos: [20:18]; default: 0;
 *  0x0 (CTRL): Contro
 *  0x1 (ISOC): Isochronous
 *  0x2 (BULK): Bulk
 *  0x3 (INTERR): Interrupt
 */
#define USB_H_EPTYPE7    0x00000003
#define USB_H_EPTYPE7_M  (USB_H_EPTYPE7_V << USB_H_EPTYPE7_S)
#define USB_H_EPTYPE7_V  0x00000003
#define USB_H_EPTYPE7_S  18
/** USB_H_EC7 : R/W; bitpos: [21]; default: 0;
 *  Multi Count (MC) / Error Count(EC)
 *  0x0 (RESERVED): Reserved. This field yields undefined result
 *  0x1 (TRANSONE): 1 transaction
 *  0x2 (TRANSTWO):2 transactions to be issued for this endpoint per microframe
 *  0x3 (TRANSTHREE): 3 transactions to be issued for this endpoint per microframe
 */
#define USB_H_EC7    (BIT(21))
#define USB_H_EC7_M  (USB_H_EC7_V << USB_H_EC7_S)
#define USB_H_EC7_V  0x00000001
#define USB_H_EC7_S  21
/** USB_H_DEVADDR7 : R/W; bitpos: [29:22]; default: 0;
 *  Device Address .    This field selects the specific device serving as the data
 *  source or sink.
 */
#define USB_H_DEVADDR7    0x0000007F
#define USB_H_DEVADDR7_M  (USB_H_DEVADDR7_V << USB_H_DEVADDR7_S)
#define USB_H_DEVADDR7_V  0x0000007F
#define USB_H_DEVADDR7_S  22
/** USB_H_ODDFRM7 : R/W; bitpos: [29]; default: 0;
 *  Odd Frame
 *  1'b0: Even (micro)Frame
 *  1'b1: Odd (micro)Frame
 */
#define USB_H_ODDFRM7    (BIT(29))
#define USB_H_ODDFRM7_M  (USB_H_ODDFRM7_V << USB_H_ODDFRM7_S)
#define USB_H_ODDFRM7_V  0x00000001
#define USB_H_ODDFRM7_S  29
/** USB_H_CHDIS7 : R/W; bitpos: [30]; default: 0;
 *  Channel Disable
 *  0x0 : Transmit/Recieve norma
 *  0x1 : Stop transmitting/receiving data on channel
 */
#define USB_H_CHDIS7    (BIT(30))
#define USB_H_CHDIS7_M  (USB_H_CHDIS7_V << USB_H_CHDIS7_S)
#define USB_H_CHDIS7_V  0x00000001
#define USB_H_CHDIS7_S  30
/** USB_H_CHENA7 : R/W; bitpos: [31]; default: 0;
 *  Channel Enable
 *  0x0 :If Scatter/Gather mode is enabled, indicates that the descriptor structure is
 *  not yet ready. If Scatter/Gather mode is disabled, indicates that the channel is
 *  disabled
 *  0x1 : If Scatter/Gather mode is enabled, indicates that the descriptor structure
 *  and data buffer with data is set up and this channel can access the descriptor. If
 *  Scatter/Gather mode is disabled, indicates that the channel is enabled
 */
#define USB_H_CHENA7    (BIT(31))
#define USB_H_CHENA7_M  (USB_H_CHENA7_V << USB_H_CHENA7_S)
#define USB_H_CHENA7_V  0x00000001
#define USB_H_CHENA7_S  31


/** USB_HCDMA7_REG register
 *  Host Channel 7 DMA Address Register
 */
#define USB_HCDMA7_REG (SOC_DPORT_USB_BASE + 0x5f4)
/** USB_H_DMAADDR7 : R/W; bitpos: [32:0]; default: 0;
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *  [31:9]: DMA Address
 *  [8:3]: Current Transfer Desc
 *  [2:0]: Reserved
 */
#define USB_H_DMAADDR7    0xFFFFFFFF
#define USB_H_DMAADDR7_M  (USB_H_DMAADDR7_V << USB_H_DMAADDR7_S)
#define USB_H_DMAADDR7_V  0xFFFFFFFF
#define USB_H_DMAADDR7_S  0


/** USB_DCFG_REG register
 *  Device Configuration Register
 */
#define USB_DCFG_REG (SOC_DPORT_USB_BASE + 0x800)
/** USB_NZSTSOUTHSHK : R/W; bitpos: [2]; default: 0;
 *  1'b0: Send the received OUT packet to the application (zero-length or non-zero
 *  length) and send a handshake based on NAK and STALL bits for the endpoint in the
 *  Devce Endpoint Control Register
 *  1'b1: Send a STALL handshake on a nonzero-length status OUT transaction and do not
 *  send the received OUT packet to the application
 */
#define USB_NZSTSOUTHSHK    (BIT(2))
#define USB_NZSTSOUTHSHK_M  (USB_NZSTSOUTHSHK_V << USB_NZSTSOUTHSHK_S)
#define USB_NZSTSOUTHSHK_V  0x00000001
#define USB_NZSTSOUTHSHK_S  2
/** USB_DEVADDR : R/W; bitpos: [11:4]; default: 0;
 *  Device Address.
 */
#define USB_DEVADDR    0x0000007F
#define USB_DEVADDR_M  (USB_DEVADDR_V << USB_DEVADDR_S)
#define USB_DEVADDR_V  0x0000007F
#define USB_DEVADDR_S  4
/** USB_PERFRLINT : R/W; bitpos: [13:11]; default: 0;
 *  0x0 (EOPF80): 80% of the (micro)Frame interval
 *  0x1 (EOPF85): 85% of the (micro)Frame interval
 *  0x2 (EOPF90): 90% of the (micro)Frame interval
 *  0x3 (EOPF95): 95% of the (micro)Frame interval
 */
#define USB_PERFRLINT    0x00000003
#define USB_PERFRLINT_M  (USB_PERFRLINT_V << USB_PERFRLINT_S)
#define USB_PERFRLINT_V  0x00000003
#define USB_PERFRLINT_S  11
/** USB_ENDEVOUTNAK : R/W; bitpos: [13]; default: 0;
 *  1'b0:The core does not set NAK after Bulk OUT transfer complete
 *  1'b1: The core sets NAK after Bulk OUT transfer complete
 */
#define USB_ENDEVOUTNAK    (BIT(13))
#define USB_ENDEVOUTNAK_M  (USB_ENDEVOUTNAK_V << USB_ENDEVOUTNAK_S)
#define USB_ENDEVOUTNAK_V  0x00000001
#define USB_ENDEVOUTNAK_S  13
/** USB_XCVRDLY : R/W; bitpos: [14]; default: 0;
 *  0x0 : No delay between xcvr_sel and txvalid during Device chirp
 *  0x1 : Enable delay between xcvr_sel and txvalid during Device chirp
 */
#define USB_XCVRDLY    (BIT(14))
#define USB_XCVRDLY_M  (USB_XCVRDLY_V << USB_XCVRDLY_S)
#define USB_XCVRDLY_V  0x00000001
#define USB_XCVRDLY_S  14
/** USB_ERRATICINTMSK : R/W; bitpos: [15]; default: 0;
 *  0x0 : Early suspend interrupt is generated on erratic error
 *  0x1: Mask early suspend interrupt on erratic error
 */
#define USB_ERRATICINTMSK    (BIT(15))
#define USB_ERRATICINTMSK_M  (USB_ERRATICINTMSK_V << USB_ERRATICINTMSK_S)
#define USB_ERRATICINTMSK_V  0x00000001
#define USB_ERRATICINTMSK_S  15
/** USB_EPMISCNT : R/W; bitpos: [23:18]; default: 4;
 *  IN Endpoint Mismatch Count.
 */
#define USB_EPMISCNT    0x0000001F
#define USB_EPMISCNT_M  (USB_EPMISCNT_V << USB_EPMISCNT_S)
#define USB_EPMISCNT_V  0x0000001F
#define USB_EPMISCNT_S  18
/** USB_DESCDMA : R/W; bitpos: [23]; default: 0;
 *  1'b0: Disable Scatter/Gather DMA
 *  1'b1: Enable Scatter/Gather DMA
 */
#define USB_DESCDMA    (BIT(23))
#define USB_DESCDMA_M  (USB_DESCDMA_V << USB_DESCDMA_S)
#define USB_DESCDMA_V  0x00000001
#define USB_DESCDMA_S  23
/** USB_PERSCHINTVL : R/W; bitpos: [26:24]; default: 0;
 *  Periodic Scheduling Interval
 *  0x0 (MF25): 25% of (micro)Frame
 *  0x1 (MF50): 50% of (micro)Frame
 *  0x2 (MF75): 75% of (micro)Frame
 *  0x3 (RESERVED): Reserved
 */
#define USB_PERSCHINTVL    0x00000003
#define USB_PERSCHINTVL_M  (USB_PERSCHINTVL_V << USB_PERSCHINTVL_S)
#define USB_PERSCHINTVL_V  0x00000003
#define USB_PERSCHINTVL_S  24
/** USB_RESVALID : R/W; bitpos: [32:26]; default: 2;
 *  This field is effective only when DCFG.Ena32KHzSusp is set. It controls the resume
 *  period when the core resumes from suspend.
 *  The core counts for ResValid number of clock cycles to detect a valid resume when
 *  this bit is set
 */
#define USB_RESVALID    0x0000003F
#define USB_RESVALID_M  (USB_RESVALID_V << USB_RESVALID_S)
#define USB_RESVALID_V  0x0000003F
#define USB_RESVALID_S  26


/** USB_DCTL_REG register
 *  Device Control Register
 */
#define USB_DCTL_REG (SOC_DPORT_USB_BASE + 0x804)
/** USB_RMTWKUPSIG : R/W; bitpos: [0]; default: 0;
 *  0x0 : Core does not send Remote Wakeup Signaling
 *  0x1 : Core sends Remote Wakeup Signalin
 */
#define USB_RMTWKUPSIG    (BIT(0))
#define USB_RMTWKUPSIG_M  (USB_RMTWKUPSIG_V << USB_RMTWKUPSIG_S)
#define USB_RMTWKUPSIG_V  0x00000001
#define USB_RMTWKUPSIG_S  0
/** USB_SFTDISCON : R/W; bitpos: [1]; default: 0;
 *  1'b0: Normal operation. When this bit is cleared after a soft disconnect, the core
 *  drives the phy_opmode_o signal on the UTMI+ to 2'b00, which generates a device
 *  connect event to the USB host. When the device is reconnected, the USB host
 *  restarts device enumeration
 *  1'b1: The core drives the phy_opmode_o signal on the UTMI+ to  2'b01, which
 *  generates a device disconnect event to the USB host
 */
#define USB_SFTDISCON    (BIT(1))
#define USB_SFTDISCON_M  (USB_SFTDISCON_V << USB_SFTDISCON_S)
#define USB_SFTDISCON_V  0x00000001
#define USB_SFTDISCON_S  1
/** USB_GNPINNAKSTS : RO; bitpos: [2]; default: 0;
 *  0x0 : A handshake is sent out based on the data availability in the transmit FIFO
 *  0x1 : A NAK handshake is sent out on all non-periodic  IN endpoints, irrespective
 *  of the data availability in the transmit FIFO
 */
#define USB_GNPINNAKSTS    (BIT(2))
#define USB_GNPINNAKSTS_M  (USB_GNPINNAKSTS_V << USB_GNPINNAKSTS_S)
#define USB_GNPINNAKSTS_V  0x00000001
#define USB_GNPINNAKSTS_S  2
/** USB_GOUTNAKSTS : RO; bitpos: [3]; default: 0;
 *  0x0 : A handshake is sent based on the FIFO Status and the NAK and STALL bit
 *  settings
 *  0x1 : No data is written to the RxFIFO, irrespective of  space availability. Sends
 *  a NAK handshake on all packets, except on SETUP transactions. All isochronous OUT
 *  packets are dropped
 */
#define USB_GOUTNAKSTS    (BIT(3))
#define USB_GOUTNAKSTS_M  (USB_GOUTNAKSTS_V << USB_GOUTNAKSTS_S)
#define USB_GOUTNAKSTS_V  0x00000001
#define USB_GOUTNAKSTS_S  3
/** USB_TSTCTL : R/W; bitpos: [7:4]; default: 0;
 *  0x0: Test mode disabled
 *  0x1: Test_J mode
 *  0x2 : Test_K mode
 *  0x3 : Test_SE0_NAK mode
 *  0x4 : Test_Packet mode
 *  0x5 : Test_force_Enable
 */
#define USB_TSTCTL    0x00000007
#define USB_TSTCTL_M  (USB_TSTCTL_V << USB_TSTCTL_S)
#define USB_TSTCTL_V  0x00000007
#define USB_TSTCTL_S  4
/** USB_SGNPINNAK : WO; bitpos: [7]; default: 0;
 *  Set Global Non-periodic IN NAK. A write to this field sets the Global Non-periodic
 *  IN NAK.The application uses this bit to send a NAK handshake on all non-periodic IN
 *  endpoints. The core can also Set this bit when a timeout condition is detected on a
 *  non-periodic endpoint in shared FIFO operation. The application must Set this bit
 *  only after making sure that the Global IN NAK Effective bit in the Core Interrupt
 *  Register (GINTSTS.GINNakEff) is cleared
 */
#define USB_SGNPINNAK    (BIT(7))
#define USB_SGNPINNAK_M  (USB_SGNPINNAK_V << USB_SGNPINNAK_S)
#define USB_SGNPINNAK_V  0x00000001
#define USB_SGNPINNAK_S  7
/** USB_CGNPINNAK : WO; bitpos: [8]; default: 0;
 *  Clear Global Non-periodic IN NAK. A write to this field clears the Global
 *  Non-periodic IN NAK.
 */
#define USB_CGNPINNAK    (BIT(8))
#define USB_CGNPINNAK_M  (USB_CGNPINNAK_V << USB_CGNPINNAK_S)
#define USB_CGNPINNAK_V  0x00000001
#define USB_CGNPINNAK_S  8
/** USB_SGOUTNAK : WO; bitpos: [9]; default: 0;
 *  Set Global OUT NAK. A write to this field sets the Global OUT NAK. The application
 *  uses this bit to send a NAK handshake on all OUT endpoints. The application must
 *  set the this bit only after making sure that the Global OUT NAK Effective bit in
 *  the Core Interrupt Register (GINTSTS.GOUTNakEff) is cleared.
 */
#define USB_SGOUTNAK    (BIT(9))
#define USB_SGOUTNAK_M  (USB_SGOUTNAK_V << USB_SGOUTNAK_S)
#define USB_SGOUTNAK_V  0x00000001
#define USB_SGOUTNAK_S  9
/** USB_CGOUTNAK : WO; bitpos: [10]; default: 0;
 *  Clear Global OUT NAK. A write to this field clears the Global OUT NAK.
 */
#define USB_CGOUTNAK    (BIT(10))
#define USB_CGOUTNAK_M  (USB_CGOUTNAK_V << USB_CGOUTNAK_S)
#define USB_CGOUTNAK_V  0x00000001
#define USB_CGOUTNAK_S  10
/** USB_PWRONPRGDONE : R/W; bitpos: [11]; default: 0;
 *  1'b0: Power-On Programming not done
 *  1'b1: Power-On Programming Done
 */
#define USB_PWRONPRGDONE    (BIT(11))
#define USB_PWRONPRGDONE_M  (USB_PWRONPRGDONE_V << USB_PWRONPRGDONE_S)
#define USB_PWRONPRGDONE_V  0x00000001
#define USB_PWRONPRGDONE_S  11
/** USB_GMC : R/W; bitpos: [15:13]; default: 1;
 *  Global Multi Count. applicable only for Scatter/Gather DMA mode
 *  0x0 : Invalid
 *  0x1 : 1 packet
 *  0x2 : 2 packets
 *  0x3 : 3 packets
 */
#define USB_GMC    0x00000003
#define USB_GMC_M  (USB_GMC_V << USB_GMC_S)
#define USB_GMC_V  0x00000003
#define USB_GMC_S  13
/** USB_IGNRFRMNUM : R/W; bitpos: [15]; default: 0;
 *  0: The core transmits the packets only in the frame number in which they are
 *  intended to be transmitted
 *  1: The core ignores the frame number, sending packets immediately as the packets
 *  are ready
 */
#define USB_IGNRFRMNUM    (BIT(15))
#define USB_IGNRFRMNUM_M  (USB_IGNRFRMNUM_V << USB_IGNRFRMNUM_S)
#define USB_IGNRFRMNUM_V  0x00000001
#define USB_IGNRFRMNUM_S  15
/** USB_NAKONBBLE : R/W; bitpos: [16]; default: 0;
 *  1'b0: Disable NAK on Babble Error
 *  1'b1: NAK on Babble Error
 */
#define USB_NAKONBBLE    (BIT(16))
#define USB_NAKONBBLE_M  (USB_NAKONBBLE_V << USB_NAKONBBLE_S)
#define USB_NAKONBBLE_V  0x00000001
#define USB_NAKONBBLE_S  16
/** USB_ENCOUNTONBNA : R/W; bitpos: [17]; default: 0;
 *  1'b0: After receiving BNA interrupt,the core disables the endpoint. When the
 *  endpoint is re-enabled by the application,the core starts processing from the
 *  DOEPDMA descriptor
 *  1'b1: After receiving BNA interrupt, the core disables the endpoint. When the
 *  endpoint is re-enabled by the application, the core starts processing from the
 *  descriptor that received the BNA interrupt
 */
#define USB_ENCOUNTONBNA    (BIT(17))
#define USB_ENCOUNTONBNA_M  (USB_ENCOUNTONBNA_V << USB_ENCOUNTONBNA_S)
#define USB_ENCOUNTONBNA_V  0x00000001
#define USB_ENCOUNTONBNA_S  17
/** USB_DEEPSLEEPBESLREJECT : R/W; bitpos: [18]; default: 0;
 *  1'b0: Deep Sleep BESL Reject feature is disabled
 *  1'b1: Deep Sleep BESL Reject feature is enabled
 */
#define USB_DEEPSLEEPBESLREJECT    (BIT(18))
#define USB_DEEPSLEEPBESLREJECT_M  (USB_DEEPSLEEPBESLREJECT_V << USB_DEEPSLEEPBESLREJECT_S)
#define USB_DEEPSLEEPBESLREJECT_V  0x00000001
#define USB_DEEPSLEEPBESLREJECT_S  18


/** USB_DVBUSDIS_REG register
 *  Device VBUS Discharge Time Register
 */
#define USB_DVBUSDIS_REG (SOC_DPORT_USB_BASE + 0x828)
/** USB_DVBUSDIS : R/W; bitpos: [16:0]; default: 6103;
 *  Specifies the VBUS discharge time after VBUS pulsing during SRP. This value equals
 *  (VBUS discharge time in PHY clocks) / 1, 024. The value you use depends whether the
 *  PHY is operating at 30MHz (16-bit data width) or 60 MHz (8-bit data width).
 *  Depending on your VBUS load, this value can need adjustment.
 */
#define USB_DVBUSDIS    0x0000FFFF
#define USB_DVBUSDIS_M  (USB_DVBUSDIS_V << USB_DVBUSDIS_S)
#define USB_DVBUSDIS_V  0x0000FFFF
#define USB_DVBUSDIS_S  0


/** USB_DVBUSPULSE_REG register
 *  Device VBUS Pulsing Time Register
 */
#define USB_DVBUSPULSE_REG (SOC_DPORT_USB_BASE + 0x82c)
/** USB_DVBUSPULSE : R/W; bitpos: [12:0]; default: 1464;
 *  Specifies the VBUS pulsing time during SRP. This value equals (VBUS pulsing time in
 *  PHY clocks) / 1, 024 The value you use depends whether the PHY is operating at
 *  30MHz (16-bit data width) or 60 MHz (8-bit data width).
 */
#define USB_DVBUSPULSE    0x00000FFF
#define USB_DVBUSPULSE_M  (USB_DVBUSPULSE_V << USB_DVBUSPULSE_S)
#define USB_DVBUSPULSE_V  0x00000FFF
#define USB_DVBUSPULSE_S  0


/** USB_DTHRCTL_REG register
 *  Device Threshold Control Register
 */
#define USB_DTHRCTL_REG (SOC_DPORT_USB_BASE + 0x830)
/** USB_NONISOTHREN : R/W; bitpos: [0]; default: 0;
 *  Non-ISO IN Endpoints Threshold Enable
 *  0x0 : No thresholding
 *  0x1 : Enable thresholding for non-isochronous IN endpoints
 */
#define USB_NONISOTHREN    (BIT(0))
#define USB_NONISOTHREN_M  (USB_NONISOTHREN_V << USB_NONISOTHREN_S)
#define USB_NONISOTHREN_V  0x00000001
#define USB_NONISOTHREN_S  0
/** USB_ISOTHREN : R/W; bitpos: [1]; default: 0;
 *  ISO IN Endpoints Threshold Enable
 *  0x0 : No thresholding
 *  0x1 : Enables thresholding for isochronous IN endpoints
 */
#define USB_ISOTHREN    (BIT(1))
#define USB_ISOTHREN_M  (USB_ISOTHREN_V << USB_ISOTHREN_S)
#define USB_ISOTHREN_V  0x00000001
#define USB_ISOTHREN_S  1
/** USB_TXTHRLEN : R/W; bitpos: [11:2]; default: 8;
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
#define USB_TXTHRLEN    0x000001FF
#define USB_TXTHRLEN_M  (USB_TXTHRLEN_V << USB_TXTHRLEN_S)
#define USB_TXTHRLEN_V  0x000001FF
#define USB_TXTHRLEN_S  2
/** USB_AHBTHRRATIO : R/W; bitpos: [13:11]; default: 0;
 *  2'b00: AHB threshold = MAC threshold
 *  2'b01: AHB threshold = MAC threshold/2
 *  2'b10: AHB threshold = MAC threshold/4
 *  2'b11: AHB threshold = MAC threshold/8
 */
#define USB_AHBTHRRATIO    0x00000003
#define USB_AHBTHRRATIO_M  (USB_AHBTHRRATIO_V << USB_AHBTHRRATIO_S)
#define USB_AHBTHRRATIO_V  0x00000003
#define USB_AHBTHRRATIO_S  11
/** USB_RXTHREN : R/W; bitpos: [16]; default: 0;
 *  0x0 : Disable thresholding
 *  0x1 : Enable thresholding in the receive direction
 */
#define USB_RXTHREN    (BIT(16))
#define USB_RXTHREN_M  (USB_RXTHREN_V << USB_RXTHREN_S)
#define USB_RXTHREN_V  0x00000001
#define USB_RXTHREN_S  16
/** USB_RXTHRLEN : R/W; bitpos: [26:17]; default: 1;
 *  Receive Threshold Length.  This field specifies Receive thresholding size in
 *  DWORDS. This field also specifies the amount of data received on the USB before the
 *  core can start transmitting on the AHB. The threshold length has to be at least
 *  eight DWORDS. The recommended value for ThrLen is to be the same as the programmed
 *  AHB Burst Length(GAHBCFG.HBstLen).
 */
#define USB_RXTHRLEN    0x000001FF
#define USB_RXTHRLEN_M  (USB_RXTHRLEN_V << USB_RXTHRLEN_S)
#define USB_RXTHRLEN_V  0x000001FF
#define USB_RXTHRLEN_S  17
/** USB_ARBPRKEN : R/W; bitpos: [27]; default: 1;
 *  0x0 : Disable DMA arbiter parking
 *  0x1 : Enable DMA arbiter parking for IN endpoints
 */
#define USB_ARBPRKEN    (BIT(27))
#define USB_ARBPRKEN_M  (USB_ARBPRKEN_V << USB_ARBPRKEN_S)
#define USB_ARBPRKEN_V  0x00000001
#define USB_ARBPRKEN_S  27


/** USB_DIEPCTL0_REG register
 *  Device Control IN Endpoint $n Control Register
 */
#define USB_DIEPCTL0_REG (SOC_DPORT_USB_BASE + 0x900)
/** USB_D_MPS0 : R/W; bitpos: [2:0]; default: 0;
 *  Maximum Packet Size
 *  0x0 : 64 bytes
 *  0x1 : 32 bytes
 *  0x2 : 16 bytes
 *  0x3 : 8 bytes
 */
#define USB_D_MPS0    0x00000003
#define USB_D_MPS0_M  (USB_D_MPS0_V << USB_D_MPS0_S)
#define USB_D_MPS0_V  0x00000003
#define USB_D_MPS0_S  0
/** USB_D_USBACTEP0 : RO; bitpos: [15]; default: 1;
 *  USB Active Endpoint
 *  0x1 : Control endpoint is always active
 */
#define USB_D_USBACTEP0    (BIT(15))
#define USB_D_USBACTEP0_M  (USB_D_USBACTEP0_V << USB_D_USBACTEP0_S)
#define USB_D_USBACTEP0_V  0x00000001
#define USB_D_USBACTEP0_S  15
/** USB_D_NAKSTS0 : RO; bitpos: [17]; default: 0;
 *  NAK Status
 *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
 *  0x1 : The core is transmitting NAK handshakes on this endpoint
 */
#define USB_D_NAKSTS0    (BIT(17))
#define USB_D_NAKSTS0_M  (USB_D_NAKSTS0_V << USB_D_NAKSTS0_S)
#define USB_D_NAKSTS0_V  0x00000001
#define USB_D_NAKSTS0_S  17
/** USB_D_EPTYPE0 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 0
 */
#define USB_D_EPTYPE0    0x00000003
#define USB_D_EPTYPE0_M  (USB_D_EPTYPE0_V << USB_D_EPTYPE0_S)
#define USB_D_EPTYPE0_V  0x00000003
#define USB_D_EPTYPE0_S  18
/** USB_D_STALL0 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it
 *  0x0 : No Stall
 *  0x1 : Stall Handshake
 */
#define USB_D_STALL0    (BIT(21))
#define USB_D_STALL0_M  (USB_D_STALL0_V << USB_D_STALL0_S)
#define USB_D_STALL0_V  0x00000001
#define USB_D_STALL0_S  21
/** USB_D_TXFNUM0 : R/W; bitpos: [26:22]; default: 0;
 *  TxFIFO Number.
 */
#define USB_D_TXFNUM0    0x0000000F
#define USB_D_TXFNUM0_M  (USB_D_TXFNUM0_V << USB_D_TXFNUM0_S)
#define USB_D_TXFNUM0_V  0x0000000F
#define USB_D_TXFNUM0_S  22
/** USB_D_CNAK0 : WO; bitpos: [26]; default: 0;
 *  A write to this bit clears the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_D_CNAK0    (BIT(26))
#define USB_D_CNAK0_M  (USB_D_CNAK0_V << USB_D_CNAK0_S)
#define USB_D_CNAK0_V  0x00000001
#define USB_D_CNAK0_S  26
/** USB_DI_SNAK0 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DI_SNAK0    (BIT(27))
#define USB_DI_SNAK0_M  (USB_DI_SNAK0_V << USB_DI_SNAK0_S)
#define USB_DI_SNAK0_V  0x00000001
#define USB_DI_SNAK0_S  27
/** USB_D_EPDIS0 : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No action
 *  0x1 : Disabled Endpoint
 */
#define USB_D_EPDIS0    (BIT(30))
#define USB_D_EPDIS0_M  (USB_D_EPDIS0_V << USB_D_EPDIS0_S)
#define USB_D_EPDIS0_V  0x00000001
#define USB_D_EPDIS0_S  30
/** USB_D_EPENA0 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_D_EPENA0    (BIT(31))
#define USB_D_EPENA0_M  (USB_D_EPENA0_V << USB_D_EPENA0_S)
#define USB_D_EPENA0_V  0x00000001
#define USB_D_EPENA0_S  31


/** USB_DIEPTSIZ0_REG register
 *  Device IN Endpoint 0 Transfer Size Register
 */
#define USB_DIEPTSIZ0_REG (SOC_DPORT_USB_BASE + 0x910)
/** USB_D_XFERSIZE0 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size
 *  IN Endpoints: The core decrements this field every time a packet from the external
 *  memory is written to the TxFIFO
 *  OUT Endpoints: The core decrements this field every time a  packet is read from the
 *  RxFIFO and written to the external memory
 */
#define USB_D_XFERSIZE0    0x0000007F
#define USB_D_XFERSIZE0_M  (USB_D_XFERSIZE0_V << USB_D_XFERSIZE0_S)
#define USB_D_XFERSIZE0_V  0x0000007F
#define USB_D_XFERSIZE0_S  0
/** USB_D_PKTCNT0 : R/W; bitpos: [21:19]; default: 0;
 *  Packet Count
 *  IN Endpoints : This field is decremented every time a packet (maximum size or short
 *  packet) is read from the TxFIFO
 *  OUT Endpoints: This field is decremented every time a packet (maximum size or short
 *  packet) is written to the RxFIFO
 */
#define USB_D_PKTCNT0    0x00000003
#define USB_D_PKTCNT0_M  (USB_D_PKTCNT0_V << USB_D_PKTCNT0_S)
#define USB_D_PKTCNT0_V  0x00000003
#define USB_D_PKTCNT0_S  19


/** USB_DIEPDMA0_REG register
 *  Device IN Endpoint 0 DMA Address Register
 */
#define USB_DIEPDMA0_REG (SOC_DPORT_USB_BASE + 0x914)
/** USB_D_DMAADDR0 : R/W; bitpos: [32:0]; default: 0;
 *  This field holds the start address of the external memory for storing or fetching
 *  endpoint data.
 */
#define USB_D_DMAADDR0    0xFFFFFFFF
#define USB_D_DMAADDR0_M  (USB_D_DMAADDR0_V << USB_D_DMAADDR0_S)
#define USB_D_DMAADDR0_V  0xFFFFFFFF
#define USB_D_DMAADDR0_S  0


/** USB_DIEPCTL1_REG register
 *  Device Control IN Endpoint $n Control Register
 */
#define USB_DIEPCTL1_REG (SOC_DPORT_USB_BASE + 0x920)
/** USB_D_MPS1 : R/W; bitpos: [2:0]; default: 0;
 *  Maximum Packet Size
 *  0x0 : 64 bytes
 *  0x1 : 32 bytes
 *  0x2 : 16 bytes
 *  0x3 : 8 bytes
 */
#define USB_D_MPS1    0x00000003
#define USB_D_MPS1_M  (USB_D_MPS1_V << USB_D_MPS1_S)
#define USB_D_MPS1_V  0x00000003
#define USB_D_MPS1_S  0
/** USB_D_USBACTEP1 : RO; bitpos: [15]; default: 1;
 *  USB Active Endpoint
 *  0x1 : Control endpoint is always active
 */
#define USB_D_USBACTEP1    (BIT(15))
#define USB_D_USBACTEP1_M  (USB_D_USBACTEP1_V << USB_D_USBACTEP1_S)
#define USB_D_USBACTEP1_V  0x00000001
#define USB_D_USBACTEP1_S  15
/** USB_D_NAKSTS1 : RO; bitpos: [17]; default: 0;
 *  NAK Status
 *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
 *  0x1 : The core is transmitting NAK handshakes on this endpoint
 */
#define USB_D_NAKSTS1    (BIT(17))
#define USB_D_NAKSTS1_M  (USB_D_NAKSTS1_V << USB_D_NAKSTS1_S)
#define USB_D_NAKSTS1_V  0x00000001
#define USB_D_NAKSTS1_S  17
/** USB_D_EPTYPE1 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 0
 */
#define USB_D_EPTYPE1    0x00000003
#define USB_D_EPTYPE1_M  (USB_D_EPTYPE1_V << USB_D_EPTYPE1_S)
#define USB_D_EPTYPE1_V  0x00000003
#define USB_D_EPTYPE1_S  18
/** USB_D_STALL1 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it
 *  0x0 : No Stall
 *  0x1 : Stall Handshake
 */
#define USB_D_STALL1    (BIT(21))
#define USB_D_STALL1_M  (USB_D_STALL1_V << USB_D_STALL1_S)
#define USB_D_STALL1_V  0x00000001
#define USB_D_STALL1_S  21
/** USB_D_TXFNUM1 : R/W; bitpos: [26:22]; default: 0;
 *  TxFIFO Number.
 */
#define USB_D_TXFNUM1    0x0000000F
#define USB_D_TXFNUM1_M  (USB_D_TXFNUM1_V << USB_D_TXFNUM1_S)
#define USB_D_TXFNUM1_V  0x0000000F
#define USB_D_TXFNUM1_S  22
/** USB_D_CNAK1 : WO; bitpos: [26]; default: 0;
 *  A write to this bit clears the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_D_CNAK1    (BIT(26))
#define USB_D_CNAK1_M  (USB_D_CNAK1_V << USB_D_CNAK1_S)
#define USB_D_CNAK1_V  0x00000001
#define USB_D_CNAK1_S  26
/** USB_DI_SNAK1 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DI_SNAK1    (BIT(27))
#define USB_DI_SNAK1_M  (USB_DI_SNAK1_V << USB_DI_SNAK1_S)
#define USB_DI_SNAK1_V  0x00000001
#define USB_DI_SNAK1_S  27
/** USB_DI_SETD0PID1 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DI_SETD0PID1    (BIT(28))
#define USB_DI_SETD0PID1_M  (USB_DI_SETD0PID1_V << USB_DI_SETD0PID1_S)
#define USB_DI_SETD0PID1_V  0x00000001
#define USB_DI_SETD0PID1_S  28
/** USB_DI_SETD1PID1 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DI_SETD1PID1    (BIT(29))
#define USB_DI_SETD1PID1_M  (USB_DI_SETD1PID1_V << USB_DI_SETD1PID1_S)
#define USB_DI_SETD1PID1_V  0x00000001
#define USB_DI_SETD1PID1_S  29
/** USB_D_EPDIS1 : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No action
 *  0x1 : Disabled Endpoint
 */
#define USB_D_EPDIS1    (BIT(30))
#define USB_D_EPDIS1_M  (USB_D_EPDIS1_V << USB_D_EPDIS1_S)
#define USB_D_EPDIS1_V  0x00000001
#define USB_D_EPDIS1_S  30
/** USB_D_EPENA1 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_D_EPENA1    (BIT(31))
#define USB_D_EPENA1_M  (USB_D_EPENA1_V << USB_D_EPENA1_S)
#define USB_D_EPENA1_V  0x00000001
#define USB_D_EPENA1_S  31


/** USB_DIEPTSIZ1_REG register
 *  Device IN Endpoint 1 Transfer Size Register
 */
#define USB_DIEPTSIZ1_REG (SOC_DPORT_USB_BASE + 0x930)
/** USB_D_XFERSIZE1 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size
 *  IN Endpoints: The core decrements this field every time a packet from the external
 *  memory is written to the TxFIFO
 *  OUT Endpoints: The core decrements this field every time a  packet is read from the
 *  RxFIFO and written to the external memory
 */
#define USB_D_XFERSIZE1    0x0000007F
#define USB_D_XFERSIZE1_M  (USB_D_XFERSIZE1_V << USB_D_XFERSIZE1_S)
#define USB_D_XFERSIZE1_V  0x0000007F
#define USB_D_XFERSIZE1_S  0
/** USB_D_PKTCNT1 : R/W; bitpos: [21:19]; default: 0;
 *  Packet Count
 *  IN Endpoints : This field is decremented every time a packet (maximum size or short
 *  packet) is read from the TxFIFO
 *  OUT Endpoints: This field is decremented every time a packet (maximum size or short
 *  packet) is written to the RxFIFO
 */
#define USB_D_PKTCNT1    0x00000003
#define USB_D_PKTCNT1_M  (USB_D_PKTCNT1_V << USB_D_PKTCNT1_S)
#define USB_D_PKTCNT1_V  0x00000003
#define USB_D_PKTCNT1_S  19


/** USB_DIEPDMA1_REG register
 *  Device IN Endpoint 1 DMA Address Register
 */
#define USB_DIEPDMA1_REG (SOC_DPORT_USB_BASE + 0x934)
/** USB_D_DMAADDR1 : R/W; bitpos: [32:0]; default: 0;
 *  This field holds the start address of the external memory for storing or fetching
 *  endpoint data.
 */
#define USB_D_DMAADDR1    0xFFFFFFFF
#define USB_D_DMAADDR1_M  (USB_D_DMAADDR1_V << USB_D_DMAADDR1_S)
#define USB_D_DMAADDR1_V  0xFFFFFFFF
#define USB_D_DMAADDR1_S  0


/** USB_DIEPCTL2_REG register
 *  Device Control IN Endpoint 2 Control Register
 */
#define USB_DIEPCTL2_REG (SOC_DPORT_USB_BASE + 0x940)
/** USB_D_MPS2 : R/W; bitpos: [2:0]; default: 0;
 *  Maximum Packet Size
 *  0x0 : 64 bytes
 *  0x1 : 32 bytes
 *  0x2 : 16 bytes
 *  0x3 : 8 bytes
 */
#define USB_D_MPS2    0x00000003
#define USB_D_MPS2_M  (USB_D_MPS2_V << USB_D_MPS2_S)
#define USB_D_MPS2_V  0x00000003
#define USB_D_MPS2_S  0
/** USB_D_USBACTEP2 : RO; bitpos: [15]; default: 1;
 *  USB Active Endpoint
 *  0x1 : Control endpoint is always active
 */
#define USB_D_USBACTEP2    (BIT(15))
#define USB_D_USBACTEP2_M  (USB_D_USBACTEP2_V << USB_D_USBACTEP2_S)
#define USB_D_USBACTEP2_V  0x00000001
#define USB_D_USBACTEP2_S  15
/** USB_D_NAKSTS2 : RO; bitpos: [17]; default: 0;
 *  NAK Status
 *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
 *  0x1 : The core is transmitting NAK handshakes on this endpoint
 */
#define USB_D_NAKSTS2    (BIT(17))
#define USB_D_NAKSTS2_M  (USB_D_NAKSTS2_V << USB_D_NAKSTS2_S)
#define USB_D_NAKSTS2_V  0x00000001
#define USB_D_NAKSTS2_S  17
/** USB_D_EPTYPE2 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 0
 */
#define USB_D_EPTYPE2    0x00000003
#define USB_D_EPTYPE2_M  (USB_D_EPTYPE2_V << USB_D_EPTYPE2_S)
#define USB_D_EPTYPE2_V  0x00000003
#define USB_D_EPTYPE2_S  18
/** USB_D_STALL2 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it
 *  0x0 : No Stall
 *  0x1 : Stall Handshake
 */
#define USB_D_STALL2    (BIT(21))
#define USB_D_STALL2_M  (USB_D_STALL2_V << USB_D_STALL2_S)
#define USB_D_STALL2_V  0x00000001
#define USB_D_STALL2_S  21
/** USB_D_TXFNUM2 : R/W; bitpos: [26:22]; default: 0;
 *  TxFIFO Number.
 */
#define USB_D_TXFNUM2    0x0000000F
#define USB_D_TXFNUM2_M  (USB_D_TXFNUM2_V << USB_D_TXFNUM2_S)
#define USB_D_TXFNUM2_V  0x0000000F
#define USB_D_TXFNUM2_S  22
/** USB_D_CNAK2 : WO; bitpos: [26]; default: 0;
 *  A write to this bit clears the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_D_CNAK2    (BIT(26))
#define USB_D_CNAK2_M  (USB_D_CNAK2_V << USB_D_CNAK2_S)
#define USB_D_CNAK2_V  0x00000001
#define USB_D_CNAK2_S  26
/** USB_DI_SNAK2 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DI_SNAK2    (BIT(27))
#define USB_DI_SNAK2_M  (USB_DI_SNAK2_V << USB_DI_SNAK2_S)
#define USB_DI_SNAK2_V  0x00000001
#define USB_DI_SNAK2_S  27
/** USB_DI_SETD0PID2 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DI_SETD0PID2    (BIT(28))
#define USB_DI_SETD0PID2_M  (USB_DI_SETD0PID2_V << USB_DI_SETD0PID2_S)
#define USB_DI_SETD0PID2_V  0x00000001
#define USB_DI_SETD0PID2_S  28
/** USB_DI_SETD1PID2 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DI_SETD1PID2    (BIT(29))
#define USB_DI_SETD1PID2_M  (USB_DI_SETD1PID2_V << USB_DI_SETD1PID2_S)
#define USB_DI_SETD1PID2_V  0x00000001
#define USB_DI_SETD1PID2_S  29
/** USB_D_EPDIS2 : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No action
 *  0x1 : Disabled Endpoint
 */
#define USB_D_EPDIS2    (BIT(30))
#define USB_D_EPDIS2_M  (USB_D_EPDIS2_V << USB_D_EPDIS2_S)
#define USB_D_EPDIS2_V  0x00000001
#define USB_D_EPDIS2_S  30
/** USB_D_EPENA2 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_D_EPENA2    (BIT(31))
#define USB_D_EPENA2_M  (USB_D_EPENA2_V << USB_D_EPENA2_S)
#define USB_D_EPENA2_V  0x00000001
#define USB_D_EPENA2_S  31


/** USB_DIEPTSIZ2_REG register
 *  Device IN Endpoint 2 Transfer Size Register
 */
#define USB_DIEPTSIZ2_REG (SOC_DPORT_USB_BASE + 0x950)
/** USB_D_XFERSIZE2 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size
 *  IN Endpoints: The core decrements this field every time a packet from the external
 *  memory is written to the TxFIFO
 *  OUT Endpoints: The core decrements this field every time a  packet is read from the
 *  RxFIFO and written to the external memory
 */
#define USB_D_XFERSIZE2    0x0000007F
#define USB_D_XFERSIZE2_M  (USB_D_XFERSIZE2_V << USB_D_XFERSIZE2_S)
#define USB_D_XFERSIZE2_V  0x0000007F
#define USB_D_XFERSIZE2_S  0
/** USB_D_PKTCNT2 : R/W; bitpos: [21:19]; default: 0;
 *  Packet Count
 *  IN Endpoints : This field is decremented every time a packet (maximum size or short
 *  packet) is read from the TxFIFO
 *  OUT Endpoints: This field is decremented every time a packet (maximum size or short
 *  packet) is written to the RxFIFO
 */
#define USB_D_PKTCNT2    0x00000003
#define USB_D_PKTCNT2_M  (USB_D_PKTCNT2_V << USB_D_PKTCNT2_S)
#define USB_D_PKTCNT2_V  0x00000003
#define USB_D_PKTCNT2_S  19


/** USB_DIEPDMA2_REG register
 *  Device IN Endpoint 2 DMA Address Register
 */
#define USB_DIEPDMA2_REG (SOC_DPORT_USB_BASE + 0x954)
/** USB_D_DMAADDR2 : R/W; bitpos: [32:0]; default: 0;
 *  This field holds the start address of the external memory for storing or fetching
 *  endpoint data.
 */
#define USB_D_DMAADDR2    0xFFFFFFFF
#define USB_D_DMAADDR2_M  (USB_D_DMAADDR2_V << USB_D_DMAADDR2_S)
#define USB_D_DMAADDR2_V  0xFFFFFFFF
#define USB_D_DMAADDR2_S  0


/** USB_DIEPCTL3_REG register
 *  Device Control IN Endpoint $n Control Register
 */
#define USB_DIEPCTL3_REG (SOC_DPORT_USB_BASE + 0x960)
/** USB_DI_MPS3 : R/W; bitpos: [2:0]; default: 0;
 *  Maximum Packet Size
 *  0x0 : 64 bytes
 *  0x1 : 32 bytes
 *  0x2 : 16 bytes
 *  0x3 : 8 bytes
 */
#define USB_DI_MPS3    0x00000003
#define USB_DI_MPS3_M  (USB_DI_MPS3_V << USB_DI_MPS3_S)
#define USB_DI_MPS3_V  0x00000003
#define USB_DI_MPS3_S  0
/** USB_DI_USBACTEP3 : RO; bitpos: [15]; default: 1;
 *  USB Active Endpoint
 *  0x1 : Control endpoint is always active
 */
#define USB_DI_USBACTEP3    (BIT(15))
#define USB_DI_USBACTEP3_M  (USB_DI_USBACTEP3_V << USB_DI_USBACTEP3_S)
#define USB_DI_USBACTEP3_V  0x00000001
#define USB_DI_USBACTEP3_S  15
/** USB_DI_NAKSTS3 : RO; bitpos: [17]; default: 0;
 *  NAK Status
 *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
 *  0x1 : The core is transmitting NAK handshakes on this endpoint
 */
#define USB_DI_NAKSTS3    (BIT(17))
#define USB_DI_NAKSTS3_M  (USB_DI_NAKSTS3_V << USB_DI_NAKSTS3_S)
#define USB_DI_NAKSTS3_V  0x00000001
#define USB_DI_NAKSTS3_S  17
/** USB_DI_EPTYPE3 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 0
 */
#define USB_DI_EPTYPE3    0x00000003
#define USB_DI_EPTYPE3_M  (USB_DI_EPTYPE3_V << USB_DI_EPTYPE3_S)
#define USB_DI_EPTYPE3_V  0x00000003
#define USB_DI_EPTYPE3_S  18
/** USB_DI_STALL3 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it
 *  0x0 : No Stall
 *  0x1 : Stall Handshake
 */
#define USB_DI_STALL3    (BIT(21))
#define USB_DI_STALL3_M  (USB_DI_STALL3_V << USB_DI_STALL3_S)
#define USB_DI_STALL3_V  0x00000001
#define USB_DI_STALL3_S  21
/** USB_DI_TXFNUM3 : R/W; bitpos: [26:22]; default: 0;
 *  TxFIFO Number.
 */
#define USB_DI_TXFNUM3    0x0000000F
#define USB_DI_TXFNUM3_M  (USB_DI_TXFNUM3_V << USB_DI_TXFNUM3_S)
#define USB_DI_TXFNUM3_V  0x0000000F
#define USB_DI_TXFNUM3_S  22
/** USB_DI_CNAK3 : WO; bitpos: [26]; default: 0;
 *  A write to this bit clears the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_DI_CNAK3    (BIT(26))
#define USB_DI_CNAK3_M  (USB_DI_CNAK3_V << USB_DI_CNAK3_S)
#define USB_DI_CNAK3_V  0x00000001
#define USB_DI_CNAK3_S  26
/** USB_DI_SNAK3 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DI_SNAK3    (BIT(27))
#define USB_DI_SNAK3_M  (USB_DI_SNAK3_V << USB_DI_SNAK3_S)
#define USB_DI_SNAK3_V  0x00000001
#define USB_DI_SNAK3_S  27
/** USB_DI_SETD0PID3 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DI_SETD0PID3    (BIT(28))
#define USB_DI_SETD0PID3_M  (USB_DI_SETD0PID3_V << USB_DI_SETD0PID3_S)
#define USB_DI_SETD0PID3_V  0x00000001
#define USB_DI_SETD0PID3_S  28
/** USB_DI_SETD1PID3 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DI_SETD1PID3    (BIT(29))
#define USB_DI_SETD1PID3_M  (USB_DI_SETD1PID3_V << USB_DI_SETD1PID3_S)
#define USB_DI_SETD1PID3_V  0x00000001
#define USB_DI_SETD1PID3_S  29
/** USB_DI_EPDIS3 : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No action
 *  0x1 : Disabled Endpoint
 */
#define USB_DI_EPDIS3    (BIT(30))
#define USB_DI_EPDIS3_M  (USB_DI_EPDIS3_V << USB_DI_EPDIS3_S)
#define USB_DI_EPDIS3_V  0x00000001
#define USB_DI_EPDIS3_S  30
/** USB_DI_EPENA3 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_DI_EPENA3    (BIT(31))
#define USB_DI_EPENA3_M  (USB_DI_EPENA3_V << USB_DI_EPENA3_S)
#define USB_DI_EPENA3_V  0x00000001
#define USB_DI_EPENA3_S  31


/** USB_DIEPTSIZ3_REG register
 *  Device IN Endpoint 3 Transfer Size Register
 */
#define USB_DIEPTSIZ3_REG (SOC_DPORT_USB_BASE + 0x970)
/** USB_D_XFERSIZE3 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size
 *  IN Endpoints: The core decrements this field every time a packet from the external
 *  memory is written to the TxFIFO
 *  OUT Endpoints: The core decrements this field every time a  packet is read from the
 *  RxFIFO and written to the external memory
 */
#define USB_D_XFERSIZE3    0x0000007F
#define USB_D_XFERSIZE3_M  (USB_D_XFERSIZE3_V << USB_D_XFERSIZE3_S)
#define USB_D_XFERSIZE3_V  0x0000007F
#define USB_D_XFERSIZE3_S  0
/** USB_D_PKTCNT3 : R/W; bitpos: [21:19]; default: 0;
 *  Packet Count
 *  IN Endpoints : This field is decremented every time a packet (maximum size or short
 *  packet) is read from the TxFIFO
 *  OUT Endpoints: This field is decremented every time a packet (maximum size or short
 *  packet) is written to the RxFIFO
 */
#define USB_D_PKTCNT3    0x00000003
#define USB_D_PKTCNT3_M  (USB_D_PKTCNT3_V << USB_D_PKTCNT3_S)
#define USB_D_PKTCNT3_V  0x00000003
#define USB_D_PKTCNT3_S  19


/** USB_DIEPDMA3_REG register
 *  Device IN Endpoint 3 DMA Address Register
 */
#define USB_DIEPDMA3_REG (SOC_DPORT_USB_BASE + 0x974)
/** USB_D_DMAADDR3 : R/W; bitpos: [32:0]; default: 0;
 *  This field holds the start address of the external memory for storing or fetching
 *  endpoint data.
 */
#define USB_D_DMAADDR3    0xFFFFFFFF
#define USB_D_DMAADDR3_M  (USB_D_DMAADDR3_V << USB_D_DMAADDR3_S)
#define USB_D_DMAADDR3_V  0xFFFFFFFF
#define USB_D_DMAADDR3_S  0


/** USB_DIEPCTL4_REG register
 *  Device Control IN Endpoint $n Control Register
 */
#define USB_DIEPCTL4_REG (SOC_DPORT_USB_BASE + 0x980)
/** USB_D_MPS4 : R/W; bitpos: [2:0]; default: 0;
 *  Maximum Packet Size
 *  0x0 : 64 bytes
 *  0x1 : 32 bytes
 *  0x2 : 16 bytes
 *  0x3 : 8 bytes
 */
#define USB_D_MPS4    0x00000003
#define USB_D_MPS4_M  (USB_D_MPS4_V << USB_D_MPS4_S)
#define USB_D_MPS4_V  0x00000003
#define USB_D_MPS4_S  0
/** USB_D_USBACTEP4 : RO; bitpos: [15]; default: 1;
 *  USB Active Endpoint
 *  0x1 : Control endpoint is always active
 */
#define USB_D_USBACTEP4    (BIT(15))
#define USB_D_USBACTEP4_M  (USB_D_USBACTEP4_V << USB_D_USBACTEP4_S)
#define USB_D_USBACTEP4_V  0x00000001
#define USB_D_USBACTEP4_S  15
/** USB_D_NAKSTS4 : RO; bitpos: [17]; default: 0;
 *  NAK Status
 *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
 *  0x1 : The core is transmitting NAK handshakes on this endpoint
 */
#define USB_D_NAKSTS4    (BIT(17))
#define USB_D_NAKSTS4_M  (USB_D_NAKSTS4_V << USB_D_NAKSTS4_S)
#define USB_D_NAKSTS4_V  0x00000001
#define USB_D_NAKSTS4_S  17
/** USB_D_EPTYPE4 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 0
 */
#define USB_D_EPTYPE4    0x00000003
#define USB_D_EPTYPE4_M  (USB_D_EPTYPE4_V << USB_D_EPTYPE4_S)
#define USB_D_EPTYPE4_V  0x00000003
#define USB_D_EPTYPE4_S  18
/** USB_D_STALL4 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it
 *  0x0 : No Stall
 *  0x1 : Stall Handshake
 */
#define USB_D_STALL4    (BIT(21))
#define USB_D_STALL4_M  (USB_D_STALL4_V << USB_D_STALL4_S)
#define USB_D_STALL4_V  0x00000001
#define USB_D_STALL4_S  21
/** USB_D_TXFNUM4 : R/W; bitpos: [26:22]; default: 0;
 *  TxFIFO Number.
 */
#define USB_D_TXFNUM4    0x0000000F
#define USB_D_TXFNUM4_M  (USB_D_TXFNUM4_V << USB_D_TXFNUM4_S)
#define USB_D_TXFNUM4_V  0x0000000F
#define USB_D_TXFNUM4_S  22
/** USB_D_CNAK4 : WO; bitpos: [26]; default: 0;
 *  A write to this bit clears the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_D_CNAK4    (BIT(26))
#define USB_D_CNAK4_M  (USB_D_CNAK4_V << USB_D_CNAK4_S)
#define USB_D_CNAK4_V  0x00000001
#define USB_D_CNAK4_S  26
/** USB_DI_SNAK4 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DI_SNAK4    (BIT(27))
#define USB_DI_SNAK4_M  (USB_DI_SNAK4_V << USB_DI_SNAK4_S)
#define USB_DI_SNAK4_V  0x00000001
#define USB_DI_SNAK4_S  27
/** USB_DI_SETD0PID4 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DI_SETD0PID4    (BIT(28))
#define USB_DI_SETD0PID4_M  (USB_DI_SETD0PID4_V << USB_DI_SETD0PID4_S)
#define USB_DI_SETD0PID4_V  0x00000001
#define USB_DI_SETD0PID4_S  28
/** USB_DI_SETD1PID4 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DI_SETD1PID4    (BIT(29))
#define USB_DI_SETD1PID4_M  (USB_DI_SETD1PID4_V << USB_DI_SETD1PID4_S)
#define USB_DI_SETD1PID4_V  0x00000001
#define USB_DI_SETD1PID4_S  29
/** USB_D_EPDIS4 : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No action
 *  0x1 : Disabled Endpoint
 */
#define USB_D_EPDIS4    (BIT(30))
#define USB_D_EPDIS4_M  (USB_D_EPDIS4_V << USB_D_EPDIS4_S)
#define USB_D_EPDIS4_V  0x00000001
#define USB_D_EPDIS4_S  30
/** USB_D_EPENA4 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_D_EPENA4    (BIT(31))
#define USB_D_EPENA4_M  (USB_D_EPENA4_V << USB_D_EPENA4_S)
#define USB_D_EPENA4_V  0x00000001
#define USB_D_EPENA4_S  31


/** USB_DIEPTSIZ4_REG register
 *  Device IN Endpoint 4 Transfer Size Register
 */
#define USB_DIEPTSIZ4_REG (SOC_DPORT_USB_BASE + 0x990)
/** USB_D_XFERSIZE4 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size
 *  IN Endpoints: The core decrements this field every time a packet from the external
 *  memory is written to the TxFIFO
 *  OUT Endpoints: The core decrements this field every time a  packet is read from the
 *  RxFIFO and written to the external memory
 */
#define USB_D_XFERSIZE4    0x0000007F
#define USB_D_XFERSIZE4_M  (USB_D_XFERSIZE4_V << USB_D_XFERSIZE4_S)
#define USB_D_XFERSIZE4_V  0x0000007F
#define USB_D_XFERSIZE4_S  0
/** USB_D_PKTCNT4 : R/W; bitpos: [21:19]; default: 0;
 *  Packet Count
 *  IN Endpoints : This field is decremented every time a packet (maximum size or short
 *  packet) is read from the TxFIFO
 *  OUT Endpoints: This field is decremented every time a packet (maximum size or short
 *  packet) is written to the RxFIFO
 */
#define USB_D_PKTCNT4    0x00000003
#define USB_D_PKTCNT4_M  (USB_D_PKTCNT4_V << USB_D_PKTCNT4_S)
#define USB_D_PKTCNT4_V  0x00000003
#define USB_D_PKTCNT4_S  19


/** USB_DIEPDMA4_REG register
 *  Device IN Endpoint 4 DMA Address Register
 */
#define USB_DIEPDMA4_REG (SOC_DPORT_USB_BASE + 0x994)
/** USB_D_DMAADDR4 : R/W; bitpos: [32:0]; default: 0;
 *  This field holds the start address of the external memory for storing or fetching
 *  endpoint data.
 */
#define USB_D_DMAADDR4    0xFFFFFFFF
#define USB_D_DMAADDR4_M  (USB_D_DMAADDR4_V << USB_D_DMAADDR4_S)
#define USB_D_DMAADDR4_V  0xFFFFFFFF
#define USB_D_DMAADDR4_S  0


/** USB_DIEPCTL5_REG register
 *  Device Control IN Endpoint $n Control Register
 */
#define USB_DIEPCTL5_REG (SOC_DPORT_USB_BASE + 0x9a0)
/** USB_DI_MPS5 : R/W; bitpos: [2:0]; default: 0;
 *  Maximum Packet Size
 *  0x0 : 64 bytes
 *  0x1 : 32 bytes
 *  0x2 : 16 bytes
 *  0x3 : 8 bytes
 */
#define USB_DI_MPS5    0x00000003
#define USB_DI_MPS5_M  (USB_DI_MPS5_V << USB_DI_MPS5_S)
#define USB_DI_MPS5_V  0x00000003
#define USB_DI_MPS5_S  0
/** USB_DI_USBACTEP5 : RO; bitpos: [15]; default: 1;
 *  USB Active Endpoint
 *  0x1 : Control endpoint is always active
 */
#define USB_DI_USBACTEP5    (BIT(15))
#define USB_DI_USBACTEP5_M  (USB_DI_USBACTEP5_V << USB_DI_USBACTEP5_S)
#define USB_DI_USBACTEP5_V  0x00000001
#define USB_DI_USBACTEP5_S  15
/** USB_DI_NAKSTS5 : RO; bitpos: [17]; default: 0;
 *  NAK Status
 *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
 *  0x1 : The core is transmitting NAK handshakes on this endpoint
 */
#define USB_DI_NAKSTS5    (BIT(17))
#define USB_DI_NAKSTS5_M  (USB_DI_NAKSTS5_V << USB_DI_NAKSTS5_S)
#define USB_DI_NAKSTS5_V  0x00000001
#define USB_DI_NAKSTS5_S  17
/** USB_DI_EPTYPE5 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 0
 */
#define USB_DI_EPTYPE5    0x00000003
#define USB_DI_EPTYPE5_M  (USB_DI_EPTYPE5_V << USB_DI_EPTYPE5_S)
#define USB_DI_EPTYPE5_V  0x00000003
#define USB_DI_EPTYPE5_S  18
/** USB_DI_STALL5 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it
 *  0x0 : No Stall
 *  0x1 : Stall Handshake
 */
#define USB_DI_STALL5    (BIT(21))
#define USB_DI_STALL5_M  (USB_DI_STALL5_V << USB_DI_STALL5_S)
#define USB_DI_STALL5_V  0x00000001
#define USB_DI_STALL5_S  21
/** USB_DI_TXFNUM5 : R/W; bitpos: [26:22]; default: 0;
 *  TxFIFO Number.
 */
#define USB_DI_TXFNUM5    0x0000000F
#define USB_DI_TXFNUM5_M  (USB_DI_TXFNUM5_V << USB_DI_TXFNUM5_S)
#define USB_DI_TXFNUM5_V  0x0000000F
#define USB_DI_TXFNUM5_S  22
/** USB_DI_CNAK5 : WO; bitpos: [26]; default: 0;
 *  A write to this bit clears the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_DI_CNAK5    (BIT(26))
#define USB_DI_CNAK5_M  (USB_DI_CNAK5_V << USB_DI_CNAK5_S)
#define USB_DI_CNAK5_V  0x00000001
#define USB_DI_CNAK5_S  26
/** USB_DI_SNAK5 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DI_SNAK5    (BIT(27))
#define USB_DI_SNAK5_M  (USB_DI_SNAK5_V << USB_DI_SNAK5_S)
#define USB_DI_SNAK5_V  0x00000001
#define USB_DI_SNAK5_S  27
/** USB_DI_SETD0PID5 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DI_SETD0PID5    (BIT(28))
#define USB_DI_SETD0PID5_M  (USB_DI_SETD0PID5_V << USB_DI_SETD0PID5_S)
#define USB_DI_SETD0PID5_V  0x00000001
#define USB_DI_SETD0PID5_S  28
/** USB_DI_SETD1PID5 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DI_SETD1PID5    (BIT(29))
#define USB_DI_SETD1PID5_M  (USB_DI_SETD1PID5_V << USB_DI_SETD1PID5_S)
#define USB_DI_SETD1PID5_V  0x00000001
#define USB_DI_SETD1PID5_S  29
/** USB_DI_EPDIS5 : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No action
 *  0x1 : Disabled Endpoint
 */
#define USB_DI_EPDIS5    (BIT(30))
#define USB_DI_EPDIS5_M  (USB_DI_EPDIS5_V << USB_DI_EPDIS5_S)
#define USB_DI_EPDIS5_V  0x00000001
#define USB_DI_EPDIS5_S  30
/** USB_DI_EPENA5 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_DI_EPENA5    (BIT(31))
#define USB_DI_EPENA5_M  (USB_DI_EPENA5_V << USB_DI_EPENA5_S)
#define USB_DI_EPENA5_V  0x00000001
#define USB_DI_EPENA5_S  31


/** USB_DIEPTSIZ5_REG register
 *  Device IN Endpoint 5 Transfer Size Register
 */
#define USB_DIEPTSIZ5_REG (SOC_DPORT_USB_BASE + 0x9b0)
/** USB_D_XFERSIZE5 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size
 *  IN Endpoints: The core decrements this field every time a packet from the external
 *  memory is written to the TxFIFO
 *  OUT Endpoints: The core decrements this field every time a  packet is read from the
 *  RxFIFO and written to the external memory
 */
#define USB_D_XFERSIZE5    0x0000007F
#define USB_D_XFERSIZE5_M  (USB_D_XFERSIZE5_V << USB_D_XFERSIZE5_S)
#define USB_D_XFERSIZE5_V  0x0000007F
#define USB_D_XFERSIZE5_S  0
/** USB_D_PKTCNT5 : R/W; bitpos: [21:19]; default: 0;
 *  Packet Count
 *  IN Endpoints : This field is decremented every time a packet (maximum size or short
 *  packet) is read from the TxFIFO
 *  OUT Endpoints: This field is decremented every time a packet (maximum size or short
 *  packet) is written to the RxFIFO
 */
#define USB_D_PKTCNT5    0x00000003
#define USB_D_PKTCNT5_M  (USB_D_PKTCNT5_V << USB_D_PKTCNT5_S)
#define USB_D_PKTCNT5_V  0x00000003
#define USB_D_PKTCNT5_S  19


/** USB_DIEPDMA5_REG register
 *  Device IN Endpoint 5 DMA Address Register
 */
#define USB_DIEPDMA5_REG (SOC_DPORT_USB_BASE + 0x9b4)
/** USB_D_DMAADDR5 : R/W; bitpos: [32:0]; default: 0;
 *  This field holds the start address of the external memory for storing or fetching
 *  endpoint data.
 */
#define USB_D_DMAADDR5    0xFFFFFFFF
#define USB_D_DMAADDR5_M  (USB_D_DMAADDR5_V << USB_D_DMAADDR5_S)
#define USB_D_DMAADDR5_V  0xFFFFFFFF
#define USB_D_DMAADDR5_S  0


/** USB_DIEPCTL6_REG register
 *  Device Control IN Endpoint $n Control Register
 */
#define USB_DIEPCTL6_REG (SOC_DPORT_USB_BASE + 0x9c0)
/** USB_D_MPS6 : R/W; bitpos: [2:0]; default: 0;
 *  Maximum Packet Size
 *  0x0 : 64 bytes
 *  0x1 : 32 bytes
 *  0x2 : 16 bytes
 *  0x3 : 8 bytes
 */
#define USB_D_MPS6    0x00000003
#define USB_D_MPS6_M  (USB_D_MPS6_V << USB_D_MPS6_S)
#define USB_D_MPS6_V  0x00000003
#define USB_D_MPS6_S  0
/** USB_D_USBACTEP6 : RO; bitpos: [15]; default: 1;
 *  USB Active Endpoint
 *  0x1 : Control endpoint is always active
 */
#define USB_D_USBACTEP6    (BIT(15))
#define USB_D_USBACTEP6_M  (USB_D_USBACTEP6_V << USB_D_USBACTEP6_S)
#define USB_D_USBACTEP6_V  0x00000001
#define USB_D_USBACTEP6_S  15
/** USB_D_NAKSTS6 : RO; bitpos: [17]; default: 0;
 *  NAK Status
 *  0x0 : The core is transmitting non-NAK handshakes based on the FIFO Status
 *  0x1 : The core is transmitting NAK handshakes on this endpoint
 */
#define USB_D_NAKSTS6    (BIT(17))
#define USB_D_NAKSTS6_M  (USB_D_NAKSTS6_V << USB_D_NAKSTS6_S)
#define USB_D_NAKSTS6_V  0x00000001
#define USB_D_NAKSTS6_S  17
/** USB_D_EPTYPE6 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 0
 */
#define USB_D_EPTYPE6    0x00000003
#define USB_D_EPTYPE6_M  (USB_D_EPTYPE6_V << USB_D_EPTYPE6_S)
#define USB_D_EPTYPE6_V  0x00000003
#define USB_D_EPTYPE6_S  18
/** USB_D_STALL6 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it
 *  0x0 : No Stall
 *  0x1 : Stall Handshake
 */
#define USB_D_STALL6    (BIT(21))
#define USB_D_STALL6_M  (USB_D_STALL6_V << USB_D_STALL6_S)
#define USB_D_STALL6_V  0x00000001
#define USB_D_STALL6_S  21
/** USB_D_TXFNUM6 : R/W; bitpos: [26:22]; default: 0;
 *  TxFIFO Number.
 */
#define USB_D_TXFNUM6    0x0000000F
#define USB_D_TXFNUM6_M  (USB_D_TXFNUM6_V << USB_D_TXFNUM6_S)
#define USB_D_TXFNUM6_V  0x0000000F
#define USB_D_TXFNUM6_S  22
/** USB_D_CNAK6 : WO; bitpos: [26]; default: 0;
 *  A write to this bit clears the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_D_CNAK6    (BIT(26))
#define USB_D_CNAK6_M  (USB_D_CNAK6_V << USB_D_CNAK6_S)
#define USB_D_CNAK6_V  0x00000001
#define USB_D_CNAK6_S  26
/** USB_DI_SNAK6 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DI_SNAK6    (BIT(27))
#define USB_DI_SNAK6_M  (USB_DI_SNAK6_V << USB_DI_SNAK6_S)
#define USB_DI_SNAK6_V  0x00000001
#define USB_DI_SNAK6_S  27
/** USB_DI_SETD0PID6 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DI_SETD0PID6    (BIT(28))
#define USB_DI_SETD0PID6_M  (USB_DI_SETD0PID6_V << USB_DI_SETD0PID6_S)
#define USB_DI_SETD0PID6_V  0x00000001
#define USB_DI_SETD0PID6_S  28
/** USB_DI_SETD1PID6 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DI_SETD1PID6    (BIT(29))
#define USB_DI_SETD1PID6_M  (USB_DI_SETD1PID6_V << USB_DI_SETD1PID6_S)
#define USB_DI_SETD1PID6_V  0x00000001
#define USB_DI_SETD1PID6_S  29
/** USB_D_EPDIS6 : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No action
 *  0x1 : Disabled Endpoint
 */
#define USB_D_EPDIS6    (BIT(30))
#define USB_D_EPDIS6_M  (USB_D_EPDIS6_V << USB_D_EPDIS6_S)
#define USB_D_EPDIS6_V  0x00000001
#define USB_D_EPDIS6_S  30
/** USB_D_EPENA6 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_D_EPENA6    (BIT(31))
#define USB_D_EPENA6_M  (USB_D_EPENA6_V << USB_D_EPENA6_S)
#define USB_D_EPENA6_V  0x00000001
#define USB_D_EPENA6_S  31


/** USB_DIEPTSIZ6_REG register
 *  Device IN Endpoint 6 Transfer Size Register
 */
#define USB_DIEPTSIZ6_REG (SOC_DPORT_USB_BASE + 0x9d0)
/** USB_D_XFERSIZE6 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size
 *  IN Endpoints: The core decrements this field every time a packet from the external
 *  memory is written to the TxFIFO
 *  OUT Endpoints: The core decrements this field every time a  packet is read from the
 *  RxFIFO and written to the external memory
 */
#define USB_D_XFERSIZE6    0x0000007F
#define USB_D_XFERSIZE6_M  (USB_D_XFERSIZE6_V << USB_D_XFERSIZE6_S)
#define USB_D_XFERSIZE6_V  0x0000007F
#define USB_D_XFERSIZE6_S  0
/** USB_D_PKTCNT6 : R/W; bitpos: [21:19]; default: 0;
 *  Packet Count
 *  IN Endpoints : This field is decremented every time a packet (maximum size or short
 *  packet) is read from the TxFIFO
 *  OUT Endpoints: This field is decremented every time a packet (maximum size or short
 *  packet) is written to the RxFIFO
 */
#define USB_D_PKTCNT6    0x00000003
#define USB_D_PKTCNT6_M  (USB_D_PKTCNT6_V << USB_D_PKTCNT6_S)
#define USB_D_PKTCNT6_V  0x00000003
#define USB_D_PKTCNT6_S  19


/** USB_DIEPDMA6_REG register
 *  Device IN Endpoint 6 DMA Address Register
 */
#define USB_DIEPDMA6_REG (SOC_DPORT_USB_BASE + 0x9d4)
/** USB_D_DMAADDR6 : R/W; bitpos: [32:0]; default: 0;
 *  This field holds the start address of the external memory for storing or fetching
 *  endpoint data.
 */
#define USB_D_DMAADDR6    0xFFFFFFFF
#define USB_D_DMAADDR6_M  (USB_D_DMAADDR6_V << USB_D_DMAADDR6_S)
#define USB_D_DMAADDR6_V  0xFFFFFFFF
#define USB_D_DMAADDR6_S  0


/** USB_DOEPCTL0_REG register
 *  Device Control OUT Endpoint $n Control Register
 */
#define USB_DOEPCTL0_REG (SOC_DPORT_USB_BASE + 0xb00)
/** USB_MPS0 : RO; bitpos: [2:0]; default: 0;
 *  Maximum Packet Size
 *  0x0 : 64 bytes
 *  0x1 : 32 bytes
 *  0x2 : 16 bytes
 *  0x3 : 8 bytes
 */
#define USB_MPS0    0x00000003
#define USB_MPS0_M  (USB_MPS0_V << USB_MPS0_S)
#define USB_MPS0_V  0x00000003
#define USB_MPS0_S  0
/** USB_USBACTEP0 : RO; bitpos: [15]; default: 1;
 *  0x1: USB Active Endpoint 0
 */
#define USB_USBACTEP0    (BIT(15))
#define USB_USBACTEP0_M  (USB_USBACTEP0_V << USB_USBACTEP0_S)
#define USB_USBACTEP0_V  0x00000001
#define USB_USBACTEP0_S  15
/** USB_NAKSTS0 : RO; bitpos: [17]; default: 0;
 *  0x0 :  The core is transmitting non-NAK handshakes based on the FIFO status
 *  0x1 :The core is transmitting NAK handshakes on this endpoint
 */
#define USB_NAKSTS0    (BIT(17))
#define USB_NAKSTS0_M  (USB_NAKSTS0_V << USB_NAKSTS0_S)
#define USB_NAKSTS0_V  0x00000001
#define USB_NAKSTS0_S  17
/** USB_EPTYPE0 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control $n
 */
#define USB_EPTYPE0    0x00000003
#define USB_EPTYPE0_M  (USB_EPTYPE0_V << USB_EPTYPE0_S)
#define USB_EPTYPE0_V  0x00000003
#define USB_EPTYPE0_S  18
/** USB_SNP0 : R/W; bitpos: [20]; default: 0;
 *  0x0 : Reserved 0
 *  0x1 : Reserved 1
 */
#define USB_SNP0    (BIT(20))
#define USB_SNP0_M  (USB_SNP0_V << USB_SNP0_S)
#define USB_SNP0_V  0x00000001
#define USB_SNP0_S  20
/** USB_STALL0 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it, when a SETUP token
 *  is received for this endpoint
 *  0x0 (INACTIVE): No Stall
 *  0x1 (ACTIVE): Stall Handshake
 */
#define USB_STALL0    (BIT(21))
#define USB_STALL0_M  (USB_STALL0_V << USB_STALL0_S)
#define USB_STALL0_V  0x00000001
#define USB_STALL0_S  21
/** USB_CNAK0 : WO; bitpos: [26]; default: 0;
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_CNAK0    (BIT(26))
#define USB_CNAK0_M  (USB_CNAK0_V << USB_CNAK0_S)
#define USB_CNAK0_V  0x00000001
#define USB_CNAK0_S  26
/** USB_DO_SNAK0 : WO; bitpos: [27]; default: 0;
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DO_SNAK0    (BIT(27))
#define USB_DO_SNAK0_M  (USB_DO_SNAK0_V << USB_DO_SNAK0_S)
#define USB_DO_SNAK0_V  0x00000001
#define USB_DO_SNAK0_S  27
/** USB_EPDIS0 : RO; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No Endpoint disable
 */
#define USB_EPDIS0    (BIT(30))
#define USB_EPDIS0_M  (USB_EPDIS0_V << USB_EPDIS0_S)
#define USB_EPDIS0_V  0x00000001
#define USB_EPDIS0_S  30
/** USB_EPENA0 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_EPENA0    (BIT(31))
#define USB_EPENA0_M  (USB_EPENA0_V << USB_EPENA0_S)
#define USB_EPENA0_V  0x00000001
#define USB_EPENA0_S  31


/** USB_DOEPTSIZ0_REG register
 *  Device OUT Endpoint 0 Transfer Size Register
 */
#define USB_DOEPTSIZ0_REG (SOC_DPORT_USB_BASE + 0xb10)
/** USB_XFERSIZE0 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size.Indicates the transfer size in bytes for ENDPOINT0
 */
#define USB_XFERSIZE0    0x0000007F
#define USB_XFERSIZE0_M  (USB_XFERSIZE0_V << USB_XFERSIZE0_S)
#define USB_XFERSIZE0_V  0x0000007F
#define USB_XFERSIZE0_S  0
/** USB_PKTCNT0 : R/W; bitpos: [19]; default: 0;
 *  Packet Count (PktCnt).This field is decremented to zero after a packet is written
 *  into the RxFIFO.
 */
#define USB_PKTCNT0    (BIT(19))
#define USB_PKTCNT0_M  (USB_PKTCNT0_V << USB_PKTCNT0_S)
#define USB_PKTCNT0_V  0x00000001
#define USB_PKTCNT0_S  19
/** USB_SUPCNT0 : R/W; bitpos: [31:29]; default: 0;
 *  SETUP Packet Count (SUPCnt).This field specifies the number of back-to-back SETUP
 *  data packets the endpoint can receive
 *  2'b01: 1 packet
 *  2'b10: 2 packets
 *  2'b11: 3 packets
 */
#define USB_SUPCNT0    0x00000003
#define USB_SUPCNT0_M  (USB_SUPCNT0_V << USB_SUPCNT0_S)
#define USB_SUPCNT0_V  0x00000003
#define USB_SUPCNT0_S  29


/** USB_DOEPDMA0_REG register
 *  Device OUT Endpoint 0 DMA Address Register
 */
#define USB_DOEPDMA0_REG (SOC_DPORT_USB_BASE + 0xb14)
/** USB_DMAADDR0 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 */
#define USB_DMAADDR0    0xFFFFFFFF
#define USB_DMAADDR0_M  (USB_DMAADDR0_V << USB_DMAADDR0_S)
#define USB_DMAADDR0_V  0xFFFFFFFF
#define USB_DMAADDR0_S  0


/** USB_DOEPDMAB0_REG register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
#define USB_DOEPDMAB0_REG (SOC_DPORT_USB_BASE + 0xb1c)
/** USB_DMABUFFERADDR0 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_DMABUFFERADDR0    0xFFFFFFFF
#define USB_DMABUFFERADDR0_M  (USB_DMABUFFERADDR0_V << USB_DMABUFFERADDR0_S)
#define USB_DMABUFFERADDR0_V  0xFFFFFFFF
#define USB_DMABUFFERADDR0_S  0


/** USB_DOEPCTL1_REG register
 *  Device Control OUT Endpoint 1 Control Register
 */
#define USB_DOEPCTL1_REG (SOC_DPORT_USB_BASE + 0xb20)
/** USB_MPS1 : RO; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size in bytes
 */
#define USB_MPS1    0x000007FF
#define USB_MPS1_M  (USB_MPS1_V << USB_MPS1_S)
#define USB_MPS1_V  0x000007FF
#define USB_MPS1_S  0
/** USB_USBACTEP1 : RO; bitpos: [15]; default: 1;
 *  0x1: USB Active Endpoint 0
 */
#define USB_USBACTEP1    (BIT(15))
#define USB_USBACTEP1_M  (USB_USBACTEP1_V << USB_USBACTEP1_S)
#define USB_USBACTEP1_V  0x00000001
#define USB_USBACTEP1_S  15
/** USB_NAKSTS1 : RO; bitpos: [17]; default: 0;
 *  0x0 :  The core is transmitting non-NAK handshakes based on the FIFO status
 *  0x1 :The core is transmitting NAK handshakes on this endpoint
 */
#define USB_NAKSTS1    (BIT(17))
#define USB_NAKSTS1_M  (USB_NAKSTS1_V << USB_NAKSTS1_S)
#define USB_NAKSTS1_V  0x00000001
#define USB_NAKSTS1_S  17
/** USB_EPTYPE1 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 1
 */
#define USB_EPTYPE1    0x00000003
#define USB_EPTYPE1_M  (USB_EPTYPE1_V << USB_EPTYPE1_S)
#define USB_EPTYPE1_V  0x00000003
#define USB_EPTYPE1_S  18
/** USB_SNP1 : R/W; bitpos: [20]; default: 0;
 *  0x0 : Reserved 0
 *  0x1 : Reserved 1
 */
#define USB_SNP1    (BIT(20))
#define USB_SNP1_M  (USB_SNP1_V << USB_SNP1_S)
#define USB_SNP1_V  0x00000001
#define USB_SNP1_S  20
/** USB_STALL1 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it, when a SETUP token
 *  is received for this endpoint
 *  0x0 (INACTIVE): No Stall
 *  0x1 (ACTIVE): Stall Handshake
 */
#define USB_STALL1    (BIT(21))
#define USB_STALL1_M  (USB_STALL1_V << USB_STALL1_S)
#define USB_STALL1_V  0x00000001
#define USB_STALL1_S  21
/** USB_CNAK1 : WO; bitpos: [26]; default: 0;
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_CNAK1    (BIT(26))
#define USB_CNAK1_M  (USB_CNAK1_V << USB_CNAK1_S)
#define USB_CNAK1_V  0x00000001
#define USB_CNAK1_S  26
/** USB_DO_SNAK1 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DO_SNAK1    (BIT(27))
#define USB_DO_SNAK1_M  (USB_DO_SNAK1_V << USB_DO_SNAK1_S)
#define USB_DO_SNAK1_V  0x00000001
#define USB_DO_SNAK1_S  27
/** USB_DO_SETD0PID1 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DO_SETD0PID1    (BIT(28))
#define USB_DO_SETD0PID1_M  (USB_DO_SETD0PID1_V << USB_DO_SETD0PID1_S)
#define USB_DO_SETD0PID1_V  0x00000001
#define USB_DO_SETD0PID1_S  28
/** USB_DO_SETD1PID1 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DO_SETD1PID1    (BIT(29))
#define USB_DO_SETD1PID1_M  (USB_DO_SETD1PID1_V << USB_DO_SETD1PID1_S)
#define USB_DO_SETD1PID1_V  0x00000001
#define USB_DO_SETD1PID1_S  29
/** USB_EPDIS1 : RO; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No Endpoint disable
 */
#define USB_EPDIS1    (BIT(30))
#define USB_EPDIS1_M  (USB_EPDIS1_V << USB_EPDIS1_S)
#define USB_EPDIS1_V  0x00000001
#define USB_EPDIS1_S  30
/** USB_EPENA1 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_EPENA1    (BIT(31))
#define USB_EPENA1_M  (USB_EPENA1_V << USB_EPENA1_S)
#define USB_EPENA1_V  0x00000001
#define USB_EPENA1_S  31


/** USB_DOEPTSIZ1_REG register
 *  Device OUT Endpoint 1 Transfer Size Register
 */
#define USB_DOEPTSIZ1_REG (SOC_DPORT_USB_BASE + 0xb30)
/** USB_XFERSIZE1 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size.Indicates the transfer size in bytes for ENDPOINT1
 */
#define USB_XFERSIZE1    0x0000007F
#define USB_XFERSIZE1_M  (USB_XFERSIZE1_V << USB_XFERSIZE1_S)
#define USB_XFERSIZE1_V  0x0000007F
#define USB_XFERSIZE1_S  0
/** USB_PKTCNT1 : R/W; bitpos: [19]; default: 0;
 *  Packet Count (PktCnt).This field is decremented to zero after a packet is written
 *  into the RxFIFO.
 */
#define USB_PKTCNT1    (BIT(19))
#define USB_PKTCNT1_M  (USB_PKTCNT1_V << USB_PKTCNT1_S)
#define USB_PKTCNT1_V  0x00000001
#define USB_PKTCNT1_S  19
/** USB_SUPCNT1 : R/W; bitpos: [31:29]; default: 0;
 *  SETUP Packet Count (SUPCnt).This field specifies the number of back-to-back SETUP
 *  data packets the endpoint can receive
 *  2'b01: 1 packet
 *  2'b10: 2 packets
 *  2'b11: 3 packets
 */
#define USB_SUPCNT1    0x00000003
#define USB_SUPCNT1_M  (USB_SUPCNT1_V << USB_SUPCNT1_S)
#define USB_SUPCNT1_V  0x00000003
#define USB_SUPCNT1_S  29


/** USB_DOEPDMA1_REG register
 *  Device OUT Endpoint 1 DMA Address Register
 */
#define USB_DOEPDMA1_REG (SOC_DPORT_USB_BASE + 0xb34)
/** USB_DMAADDR1 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 */
#define USB_DMAADDR1    0xFFFFFFFF
#define USB_DMAADDR1_M  (USB_DMAADDR1_V << USB_DMAADDR1_S)
#define USB_DMAADDR1_V  0xFFFFFFFF
#define USB_DMAADDR1_S  0


/** USB_DOEPDMAB1_REG register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
#define USB_DOEPDMAB1_REG (SOC_DPORT_USB_BASE + 0xb3c)
/** USB_DMABUFFERADDR1 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_DMABUFFERADDR1    0xFFFFFFFF
#define USB_DMABUFFERADDR1_M  (USB_DMABUFFERADDR1_V << USB_DMABUFFERADDR1_S)
#define USB_DMABUFFERADDR1_V  0xFFFFFFFF
#define USB_DMABUFFERADDR1_S  0


/** USB_DOEPCTL2_REG register
 *  Device Control OUT Endpoint 2 Control Register
 */
#define USB_DOEPCTL2_REG (SOC_DPORT_USB_BASE + 0xb40)
/** USB_MPS2 : RO; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size in bytes
 */
#define USB_MPS2    0x000007FF
#define USB_MPS2_M  (USB_MPS2_V << USB_MPS2_S)
#define USB_MPS2_V  0x000007FF
#define USB_MPS2_S  0
/** USB_USBACTEP2 : RO; bitpos: [15]; default: 1;
 *  0x1: USB Active Endpoint 0
 */
#define USB_USBACTEP2    (BIT(15))
#define USB_USBACTEP2_M  (USB_USBACTEP2_V << USB_USBACTEP2_S)
#define USB_USBACTEP2_V  0x00000001
#define USB_USBACTEP2_S  15
/** USB_NAKSTS2 : RO; bitpos: [17]; default: 0;
 *  0x0 :  The core is transmitting non-NAK handshakes based on the FIFO status
 *  0x1 :The core is transmitting NAK handshakes on this endpoint
 */
#define USB_NAKSTS2    (BIT(17))
#define USB_NAKSTS2_M  (USB_NAKSTS2_V << USB_NAKSTS2_S)
#define USB_NAKSTS2_V  0x00000001
#define USB_NAKSTS2_S  17
/** USB_EPTYPE2 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 2
 */
#define USB_EPTYPE2    0x00000003
#define USB_EPTYPE2_M  (USB_EPTYPE2_V << USB_EPTYPE2_S)
#define USB_EPTYPE2_V  0x00000003
#define USB_EPTYPE2_S  18
/** USB_SNP2 : R/W; bitpos: [20]; default: 0;
 *  0x0 : Reserved 0
 *  0x1 : Reserved 1
 */
#define USB_SNP2    (BIT(20))
#define USB_SNP2_M  (USB_SNP2_V << USB_SNP2_S)
#define USB_SNP2_V  0x00000001
#define USB_SNP2_S  20
/** USB_STALL2 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it, when a SETUP token
 *  is received for this endpoint
 *  0x0 (INACTIVE): No Stall
 *  0x1 (ACTIVE): Stall Handshake
 */
#define USB_STALL2    (BIT(21))
#define USB_STALL2_M  (USB_STALL2_V << USB_STALL2_S)
#define USB_STALL2_V  0x00000001
#define USB_STALL2_S  21
/** USB_CNAK2 : WO; bitpos: [26]; default: 0;
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_CNAK2    (BIT(26))
#define USB_CNAK2_M  (USB_CNAK2_V << USB_CNAK2_S)
#define USB_CNAK2_V  0x00000001
#define USB_CNAK2_S  26
/** USB_DO_SNAK2 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DO_SNAK2    (BIT(27))
#define USB_DO_SNAK2_M  (USB_DO_SNAK2_V << USB_DO_SNAK2_S)
#define USB_DO_SNAK2_V  0x00000001
#define USB_DO_SNAK2_S  27
/** USB_DO_SETD0PID2 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DO_SETD0PID2    (BIT(28))
#define USB_DO_SETD0PID2_M  (USB_DO_SETD0PID2_V << USB_DO_SETD0PID2_S)
#define USB_DO_SETD0PID2_V  0x00000001
#define USB_DO_SETD0PID2_S  28
/** USB_DO_SETD1PID2 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DO_SETD1PID2    (BIT(29))
#define USB_DO_SETD1PID2_M  (USB_DO_SETD1PID2_V << USB_DO_SETD1PID2_S)
#define USB_DO_SETD1PID2_V  0x00000001
#define USB_DO_SETD1PID2_S  29
/** USB_EPDIS2 : RO; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No Endpoint disable
 */
#define USB_EPDIS2    (BIT(30))
#define USB_EPDIS2_M  (USB_EPDIS2_V << USB_EPDIS2_S)
#define USB_EPDIS2_V  0x00000001
#define USB_EPDIS2_S  30
/** USB_EPENA2 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_EPENA2    (BIT(31))
#define USB_EPENA2_M  (USB_EPENA2_V << USB_EPENA2_S)
#define USB_EPENA2_V  0x00000001
#define USB_EPENA2_S  31


/** USB_DOEPTSIZ2_REG register
 *  Device OUT Endpoint 2 Transfer Size Register
 */
#define USB_DOEPTSIZ2_REG (SOC_DPORT_USB_BASE + 0xb50)
/** USB_XFERSIZE2 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size.Indicates the transfer size in bytes for ENDPOINT2
 */
#define USB_XFERSIZE2    0x0000007F
#define USB_XFERSIZE2_M  (USB_XFERSIZE2_V << USB_XFERSIZE2_S)
#define USB_XFERSIZE2_V  0x0000007F
#define USB_XFERSIZE2_S  0
/** USB_PKTCNT2 : R/W; bitpos: [19]; default: 0;
 *  Packet Count (PktCnt).This field is decremented to zero after a packet is written
 *  into the RxFIFO.
 */
#define USB_PKTCNT2    (BIT(19))
#define USB_PKTCNT2_M  (USB_PKTCNT2_V << USB_PKTCNT2_S)
#define USB_PKTCNT2_V  0x00000001
#define USB_PKTCNT2_S  19
/** USB_SUPCNT2 : R/W; bitpos: [31:29]; default: 0;
 *  SETUP Packet Count (SUPCnt).This field specifies the number of back-to-back SETUP
 *  data packets the endpoint can receive
 *  2'b01: 1 packet
 *  2'b10: 2 packets
 *  2'b11: 3 packets
 */
#define USB_SUPCNT2    0x00000003
#define USB_SUPCNT2_M  (USB_SUPCNT2_V << USB_SUPCNT2_S)
#define USB_SUPCNT2_V  0x00000003
#define USB_SUPCNT2_S  29


/** USB_DOEPDMA2_REG register
 *  Device OUT Endpoint 2 DMA Address Register
 */
#define USB_DOEPDMA2_REG (SOC_DPORT_USB_BASE + 0xb54)
/** USB_DMAADDR2 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 */
#define USB_DMAADDR2    0xFFFFFFFF
#define USB_DMAADDR2_M  (USB_DMAADDR2_V << USB_DMAADDR2_S)
#define USB_DMAADDR2_V  0xFFFFFFFF
#define USB_DMAADDR2_S  0


/** USB_DOEPDMAB2_REG register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
#define USB_DOEPDMAB2_REG (SOC_DPORT_USB_BASE + 0xb5c)
/** USB_DMABUFFERADDR2 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_DMABUFFERADDR2    0xFFFFFFFF
#define USB_DMABUFFERADDR2_M  (USB_DMABUFFERADDR2_V << USB_DMABUFFERADDR2_S)
#define USB_DMABUFFERADDR2_V  0xFFFFFFFF
#define USB_DMABUFFERADDR2_S  0


/** USB_DOEPCTL3_REG register
 *  Device Control OUT Endpoint 3 Control Register
 */
#define USB_DOEPCTL3_REG (SOC_DPORT_USB_BASE + 0xb60)
/** USB_MPS3 : RO; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size in bytes
 */
#define USB_MPS3    0x000007FF
#define USB_MPS3_M  (USB_MPS3_V << USB_MPS3_S)
#define USB_MPS3_V  0x000007FF
#define USB_MPS3_S  0
/** USB_USBACTEP3 : RO; bitpos: [15]; default: 1;
 *  0x1: USB Active Endpoint 0
 */
#define USB_USBACTEP3    (BIT(15))
#define USB_USBACTEP3_M  (USB_USBACTEP3_V << USB_USBACTEP3_S)
#define USB_USBACTEP3_V  0x00000001
#define USB_USBACTEP3_S  15
/** USB_NAKSTS3 : RO; bitpos: [17]; default: 0;
 *  0x0 :  The core is transmitting non-NAK handshakes based on the FIFO status
 *  0x1 :The core is transmitting NAK handshakes on this endpoint
 */
#define USB_NAKSTS3    (BIT(17))
#define USB_NAKSTS3_M  (USB_NAKSTS3_V << USB_NAKSTS3_S)
#define USB_NAKSTS3_V  0x00000001
#define USB_NAKSTS3_S  17
/** USB_EPTYPE3 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 3
 */
#define USB_EPTYPE3    0x00000003
#define USB_EPTYPE3_M  (USB_EPTYPE3_V << USB_EPTYPE3_S)
#define USB_EPTYPE3_V  0x00000003
#define USB_EPTYPE3_S  18
/** USB_SNP3 : R/W; bitpos: [20]; default: 0;
 *  0x0 : Reserved 0
 *  0x1 : Reserved 1
 */
#define USB_SNP3    (BIT(20))
#define USB_SNP3_M  (USB_SNP3_V << USB_SNP3_S)
#define USB_SNP3_V  0x00000001
#define USB_SNP3_S  20
/** USB_STALL3 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it, when a SETUP token
 *  is received for this endpoint
 *  0x0 (INACTIVE): No Stall
 *  0x1 (ACTIVE): Stall Handshake
 */
#define USB_STALL3    (BIT(21))
#define USB_STALL3_M  (USB_STALL3_V << USB_STALL3_S)
#define USB_STALL3_V  0x00000001
#define USB_STALL3_S  21
/** USB_CNAK3 : WO; bitpos: [26]; default: 0;
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_CNAK3    (BIT(26))
#define USB_CNAK3_M  (USB_CNAK3_V << USB_CNAK3_S)
#define USB_CNAK3_V  0x00000001
#define USB_CNAK3_S  26
/** USB_DO_SNAK3 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DO_SNAK3    (BIT(27))
#define USB_DO_SNAK3_M  (USB_DO_SNAK3_V << USB_DO_SNAK3_S)
#define USB_DO_SNAK3_V  0x00000001
#define USB_DO_SNAK3_S  27
/** USB_DO_SETD0PID3 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DO_SETD0PID3    (BIT(28))
#define USB_DO_SETD0PID3_M  (USB_DO_SETD0PID3_V << USB_DO_SETD0PID3_S)
#define USB_DO_SETD0PID3_V  0x00000001
#define USB_DO_SETD0PID3_S  28
/** USB_DO_SETD1PID3 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DO_SETD1PID3    (BIT(29))
#define USB_DO_SETD1PID3_M  (USB_DO_SETD1PID3_V << USB_DO_SETD1PID3_S)
#define USB_DO_SETD1PID3_V  0x00000001
#define USB_DO_SETD1PID3_S  29
/** USB_EPDIS3 : RO; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No Endpoint disable
 */
#define USB_EPDIS3    (BIT(30))
#define USB_EPDIS3_M  (USB_EPDIS3_V << USB_EPDIS3_S)
#define USB_EPDIS3_V  0x00000001
#define USB_EPDIS3_S  30
/** USB_EPENA3 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_EPENA3    (BIT(31))
#define USB_EPENA3_M  (USB_EPENA3_V << USB_EPENA3_S)
#define USB_EPENA3_V  0x00000001
#define USB_EPENA3_S  31


/** USB_DOEPTSIZ3_REG register
 *  Device OUT Endpoint 3 Transfer Size Register
 */
#define USB_DOEPTSIZ3_REG (SOC_DPORT_USB_BASE + 0xb70)
/** USB_XFERSIZE3 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size.Indicates the transfer size in bytes for ENDPOINT3
 */
#define USB_XFERSIZE3    0x0000007F
#define USB_XFERSIZE3_M  (USB_XFERSIZE3_V << USB_XFERSIZE3_S)
#define USB_XFERSIZE3_V  0x0000007F
#define USB_XFERSIZE3_S  0
/** USB_PKTCNT3 : R/W; bitpos: [19]; default: 0;
 *  Packet Count (PktCnt).This field is decremented to zero after a packet is written
 *  into the RxFIFO.
 */
#define USB_PKTCNT3    (BIT(19))
#define USB_PKTCNT3_M  (USB_PKTCNT3_V << USB_PKTCNT3_S)
#define USB_PKTCNT3_V  0x00000001
#define USB_PKTCNT3_S  19
/** USB_SUPCNT3 : R/W; bitpos: [31:29]; default: 0;
 *  SETUP Packet Count (SUPCnt).This field specifies the number of back-to-back SETUP
 *  data packets the endpoint can receive
 *  2'b01: 1 packet
 *  2'b10: 2 packets
 *  2'b11: 3 packets
 */
#define USB_SUPCNT3    0x00000003
#define USB_SUPCNT3_M  (USB_SUPCNT3_V << USB_SUPCNT3_S)
#define USB_SUPCNT3_V  0x00000003
#define USB_SUPCNT3_S  29


/** USB_DOEPDMA3_REG register
 *  Device OUT Endpoint 3 DMA Address Register
 */
#define USB_DOEPDMA3_REG (SOC_DPORT_USB_BASE + 0xb74)
/** USB_DMAADDR3 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 */
#define USB_DMAADDR3    0xFFFFFFFF
#define USB_DMAADDR3_M  (USB_DMAADDR3_V << USB_DMAADDR3_S)
#define USB_DMAADDR3_V  0xFFFFFFFF
#define USB_DMAADDR3_S  0


/** USB_DOEPDMAB3_REG register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
#define USB_DOEPDMAB3_REG (SOC_DPORT_USB_BASE + 0xb7c)
/** USB_DMABUFFERADDR3 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_DMABUFFERADDR3    0xFFFFFFFF
#define USB_DMABUFFERADDR3_M  (USB_DMABUFFERADDR3_V << USB_DMABUFFERADDR3_S)
#define USB_DMABUFFERADDR3_V  0xFFFFFFFF
#define USB_DMABUFFERADDR3_S  0


/** USB_DOEPCTL4_REG register
 *  Device Control OUT Endpoint 4 Control Register
 */
#define USB_DOEPCTL4_REG (SOC_DPORT_USB_BASE + 0xb80)
/** USB_MPS4 : RO; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size in bytes
 */
#define USB_MPS4    0x000007FF
#define USB_MPS4_M  (USB_MPS4_V << USB_MPS4_S)
#define USB_MPS4_V  0x000007FF
#define USB_MPS4_S  0
/** USB_USBACTEP4 : RO; bitpos: [15]; default: 1;
 *  0x1: USB Active Endpoint 0
 */
#define USB_USBACTEP4    (BIT(15))
#define USB_USBACTEP4_M  (USB_USBACTEP4_V << USB_USBACTEP4_S)
#define USB_USBACTEP4_V  0x00000001
#define USB_USBACTEP4_S  15
/** USB_NAKSTS4 : RO; bitpos: [17]; default: 0;
 *  0x0 :  The core is transmitting non-NAK handshakes based on the FIFO status
 *  0x1 :The core is transmitting NAK handshakes on this endpoint
 */
#define USB_NAKSTS4    (BIT(17))
#define USB_NAKSTS4_M  (USB_NAKSTS4_V << USB_NAKSTS4_S)
#define USB_NAKSTS4_V  0x00000001
#define USB_NAKSTS4_S  17
/** USB_EPTYPE4 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 4
 */
#define USB_EPTYPE4    0x00000003
#define USB_EPTYPE4_M  (USB_EPTYPE4_V << USB_EPTYPE4_S)
#define USB_EPTYPE4_V  0x00000003
#define USB_EPTYPE4_S  18
/** USB_SNP4 : R/W; bitpos: [20]; default: 0;
 *  0x0 : Reserved 0
 *  0x1 : Reserved 1
 */
#define USB_SNP4    (BIT(20))
#define USB_SNP4_M  (USB_SNP4_V << USB_SNP4_S)
#define USB_SNP4_V  0x00000001
#define USB_SNP4_S  20
/** USB_STALL4 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it, when a SETUP token
 *  is received for this endpoint
 *  0x0 (INACTIVE): No Stall
 *  0x1 (ACTIVE): Stall Handshake
 */
#define USB_STALL4    (BIT(21))
#define USB_STALL4_M  (USB_STALL4_V << USB_STALL4_S)
#define USB_STALL4_V  0x00000001
#define USB_STALL4_S  21
/** USB_CNAK4 : WO; bitpos: [26]; default: 0;
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_CNAK4    (BIT(26))
#define USB_CNAK4_M  (USB_CNAK4_V << USB_CNAK4_S)
#define USB_CNAK4_V  0x00000001
#define USB_CNAK4_S  26
/** USB_DO_SNAK4 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DO_SNAK4    (BIT(27))
#define USB_DO_SNAK4_M  (USB_DO_SNAK4_V << USB_DO_SNAK4_S)
#define USB_DO_SNAK4_V  0x00000001
#define USB_DO_SNAK4_S  27
/** USB_DO_SETD0PID4 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DO_SETD0PID4    (BIT(28))
#define USB_DO_SETD0PID4_M  (USB_DO_SETD0PID4_V << USB_DO_SETD0PID4_S)
#define USB_DO_SETD0PID4_V  0x00000001
#define USB_DO_SETD0PID4_S  28
/** USB_DO_SETD1PID4 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DO_SETD1PID4    (BIT(29))
#define USB_DO_SETD1PID4_M  (USB_DO_SETD1PID4_V << USB_DO_SETD1PID4_S)
#define USB_DO_SETD1PID4_V  0x00000001
#define USB_DO_SETD1PID4_S  29
/** USB_EPDIS4 : RO; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No Endpoint disable
 */
#define USB_EPDIS4    (BIT(30))
#define USB_EPDIS4_M  (USB_EPDIS4_V << USB_EPDIS4_S)
#define USB_EPDIS4_V  0x00000001
#define USB_EPDIS4_S  30
/** USB_EPENA4 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_EPENA4    (BIT(31))
#define USB_EPENA4_M  (USB_EPENA4_V << USB_EPENA4_S)
#define USB_EPENA4_V  0x00000001
#define USB_EPENA4_S  31


/** USB_DOEPTSIZ4_REG register
 *  Device OUT Endpoint 4 Transfer Size Register
 */
#define USB_DOEPTSIZ4_REG (SOC_DPORT_USB_BASE + 0xb90)
/** USB_XFERSIZE4 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size.Indicates the transfer size in bytes for ENDPOINT4
 */
#define USB_XFERSIZE4    0x0000007F
#define USB_XFERSIZE4_M  (USB_XFERSIZE4_V << USB_XFERSIZE4_S)
#define USB_XFERSIZE4_V  0x0000007F
#define USB_XFERSIZE4_S  0
/** USB_PKTCNT4 : R/W; bitpos: [19]; default: 0;
 *  Packet Count (PktCnt).This field is decremented to zero after a packet is written
 *  into the RxFIFO.
 */
#define USB_PKTCNT4    (BIT(19))
#define USB_PKTCNT4_M  (USB_PKTCNT4_V << USB_PKTCNT4_S)
#define USB_PKTCNT4_V  0x00000001
#define USB_PKTCNT4_S  19
/** USB_SUPCNT4 : R/W; bitpos: [31:29]; default: 0;
 *  SETUP Packet Count (SUPCnt).This field specifies the number of back-to-back SETUP
 *  data packets the endpoint can receive
 *  2'b01: 1 packet
 *  2'b10: 2 packets
 *  2'b11: 3 packets
 */
#define USB_SUPCNT4    0x00000003
#define USB_SUPCNT4_M  (USB_SUPCNT4_V << USB_SUPCNT4_S)
#define USB_SUPCNT4_V  0x00000003
#define USB_SUPCNT4_S  29


/** USB_DOEPDMA4_REG register
 *  Device OUT Endpoint 4 DMA Address Register
 */
#define USB_DOEPDMA4_REG (SOC_DPORT_USB_BASE + 0xb94)
/** USB_DMAADDR4 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 */
#define USB_DMAADDR4    0xFFFFFFFF
#define USB_DMAADDR4_M  (USB_DMAADDR4_V << USB_DMAADDR4_S)
#define USB_DMAADDR4_V  0xFFFFFFFF
#define USB_DMAADDR4_S  0


/** USB_DOEPDMAB4_REG register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
#define USB_DOEPDMAB4_REG (SOC_DPORT_USB_BASE + 0xb9c)
/** USB_DMABUFFERADDR4 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_DMABUFFERADDR4    0xFFFFFFFF
#define USB_DMABUFFERADDR4_M  (USB_DMABUFFERADDR4_V << USB_DMABUFFERADDR4_S)
#define USB_DMABUFFERADDR4_V  0xFFFFFFFF
#define USB_DMABUFFERADDR4_S  0


/** USB_DOEPCTL5_REG register
 *  Device Control OUT Endpoint 5 Control Register
 */
#define USB_DOEPCTL5_REG (SOC_DPORT_USB_BASE + 0xba0)
/** USB_MPS5 : RO; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size in bytes
 */
#define USB_MPS5    0x000007FF
#define USB_MPS5_M  (USB_MPS5_V << USB_MPS5_S)
#define USB_MPS5_V  0x000007FF
#define USB_MPS5_S  0
/** USB_USBACTEP5 : RO; bitpos: [15]; default: 1;
 *  0x1: USB Active Endpoint 0
 */
#define USB_USBACTEP5    (BIT(15))
#define USB_USBACTEP5_M  (USB_USBACTEP5_V << USB_USBACTEP5_S)
#define USB_USBACTEP5_V  0x00000001
#define USB_USBACTEP5_S  15
/** USB_NAKSTS5 : RO; bitpos: [17]; default: 0;
 *  0x0 :  The core is transmitting non-NAK handshakes based on the FIFO status
 *  0x1 :The core is transmitting NAK handshakes on this endpoint
 */
#define USB_NAKSTS5    (BIT(17))
#define USB_NAKSTS5_M  (USB_NAKSTS5_V << USB_NAKSTS5_S)
#define USB_NAKSTS5_V  0x00000001
#define USB_NAKSTS5_S  17
/** USB_EPTYPE5 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 5
 */
#define USB_EPTYPE5    0x00000003
#define USB_EPTYPE5_M  (USB_EPTYPE5_V << USB_EPTYPE5_S)
#define USB_EPTYPE5_V  0x00000003
#define USB_EPTYPE5_S  18
/** USB_SNP5 : R/W; bitpos: [20]; default: 0;
 *  0x0 : Reserved 0
 *  0x1 : Reserved 1
 */
#define USB_SNP5    (BIT(20))
#define USB_SNP5_M  (USB_SNP5_V << USB_SNP5_S)
#define USB_SNP5_V  0x00000001
#define USB_SNP5_S  20
/** USB_STALL5 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it, when a SETUP token
 *  is received for this endpoint
 *  0x0 (INACTIVE): No Stall
 *  0x1 (ACTIVE): Stall Handshake
 */
#define USB_STALL5    (BIT(21))
#define USB_STALL5_M  (USB_STALL5_V << USB_STALL5_S)
#define USB_STALL5_V  0x00000001
#define USB_STALL5_S  21
/** USB_CNAK5 : WO; bitpos: [26]; default: 0;
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_CNAK5    (BIT(26))
#define USB_CNAK5_M  (USB_CNAK5_V << USB_CNAK5_S)
#define USB_CNAK5_V  0x00000001
#define USB_CNAK5_S  26
/** USB_DO_SNAK5 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DO_SNAK5    (BIT(27))
#define USB_DO_SNAK5_M  (USB_DO_SNAK5_V << USB_DO_SNAK5_S)
#define USB_DO_SNAK5_V  0x00000001
#define USB_DO_SNAK5_S  27
/** USB_DO_SETD0PID5 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DO_SETD0PID5    (BIT(28))
#define USB_DO_SETD0PID5_M  (USB_DO_SETD0PID5_V << USB_DO_SETD0PID5_S)
#define USB_DO_SETD0PID5_V  0x00000001
#define USB_DO_SETD0PID5_S  28
/** USB_DO_SETD1PID5 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DO_SETD1PID5    (BIT(29))
#define USB_DO_SETD1PID5_M  (USB_DO_SETD1PID5_V << USB_DO_SETD1PID5_S)
#define USB_DO_SETD1PID5_V  0x00000001
#define USB_DO_SETD1PID5_S  29
/** USB_EPDIS5 : RO; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No Endpoint disable
 */
#define USB_EPDIS5    (BIT(30))
#define USB_EPDIS5_M  (USB_EPDIS5_V << USB_EPDIS5_S)
#define USB_EPDIS5_V  0x00000001
#define USB_EPDIS5_S  30
/** USB_EPENA5 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_EPENA5    (BIT(31))
#define USB_EPENA5_M  (USB_EPENA5_V << USB_EPENA5_S)
#define USB_EPENA5_V  0x00000001
#define USB_EPENA5_S  31


/** USB_DOEPTSIZ5_REG register
 *  Device OUT Endpoint 5 Transfer Size Register
 */
#define USB_DOEPTSIZ5_REG (SOC_DPORT_USB_BASE + 0xbb0)
/** USB_XFERSIZE5 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size.Indicates the transfer size in bytes for ENDPOINT5
 */
#define USB_XFERSIZE5    0x0000007F
#define USB_XFERSIZE5_M  (USB_XFERSIZE5_V << USB_XFERSIZE5_S)
#define USB_XFERSIZE5_V  0x0000007F
#define USB_XFERSIZE5_S  0
/** USB_PKTCNT5 : R/W; bitpos: [19]; default: 0;
 *  Packet Count (PktCnt).This field is decremented to zero after a packet is written
 *  into the RxFIFO.
 */
#define USB_PKTCNT5    (BIT(19))
#define USB_PKTCNT5_M  (USB_PKTCNT5_V << USB_PKTCNT5_S)
#define USB_PKTCNT5_V  0x00000001
#define USB_PKTCNT5_S  19
/** USB_SUPCNT5 : R/W; bitpos: [31:29]; default: 0;
 *  SETUP Packet Count (SUPCnt).This field specifies the number of back-to-back SETUP
 *  data packets the endpoint can receive
 *  2'b01: 1 packet
 *  2'b10: 2 packets
 *  2'b11: 3 packets
 */
#define USB_SUPCNT5    0x00000003
#define USB_SUPCNT5_M  (USB_SUPCNT5_V << USB_SUPCNT5_S)
#define USB_SUPCNT5_V  0x00000003
#define USB_SUPCNT5_S  29


/** USB_DOEPDMA5_REG register
 *  Device OUT Endpoint 5 DMA Address Register
 */
#define USB_DOEPDMA5_REG (SOC_DPORT_USB_BASE + 0xbb4)
/** USB_DMAADDR5 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 */
#define USB_DMAADDR5    0xFFFFFFFF
#define USB_DMAADDR5_M  (USB_DMAADDR5_V << USB_DMAADDR5_S)
#define USB_DMAADDR5_V  0xFFFFFFFF
#define USB_DMAADDR5_S  0


/** USB_DOEPDMAB5_REG register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
#define USB_DOEPDMAB5_REG (SOC_DPORT_USB_BASE + 0xbbc)
/** USB_DMABUFFERADDR5 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_DMABUFFERADDR5    0xFFFFFFFF
#define USB_DMABUFFERADDR5_M  (USB_DMABUFFERADDR5_V << USB_DMABUFFERADDR5_S)
#define USB_DMABUFFERADDR5_V  0xFFFFFFFF
#define USB_DMABUFFERADDR5_S  0


/** USB_DOEPCTL6_REG register
 *  Device Control OUT Endpoint 6 Control Register
 */
#define USB_DOEPCTL6_REG (SOC_DPORT_USB_BASE + 0xbc0)
/** USB_MPS6 : RO; bitpos: [11:0]; default: 0;
 *  Maximum Packet Size in bytes
 */
#define USB_MPS6    0x000007FF
#define USB_MPS6_M  (USB_MPS6_V << USB_MPS6_S)
#define USB_MPS6_V  0x000007FF
#define USB_MPS6_S  0
/** USB_USBACTEP6 : RO; bitpos: [15]; default: 1;
 *  0x1: USB Active Endpoint 0
 */
#define USB_USBACTEP6    (BIT(15))
#define USB_USBACTEP6_M  (USB_USBACTEP6_V << USB_USBACTEP6_S)
#define USB_USBACTEP6_V  0x00000001
#define USB_USBACTEP6_S  15
/** USB_NAKSTS6 : RO; bitpos: [17]; default: 0;
 *  0x0 :  The core is transmitting non-NAK handshakes based on the FIFO status
 *  0x1 :The core is transmitting NAK handshakes on this endpoint
 */
#define USB_NAKSTS6    (BIT(17))
#define USB_NAKSTS6_M  (USB_NAKSTS6_V << USB_NAKSTS6_S)
#define USB_NAKSTS6_V  0x00000001
#define USB_NAKSTS6_S  17
/** USB_EPTYPE6 : RO; bitpos: [20:18]; default: 0;
 *  Endpoint Type
 *  0x0 : Endpoint Control 6
 */
#define USB_EPTYPE6    0x00000003
#define USB_EPTYPE6_M  (USB_EPTYPE6_V << USB_EPTYPE6_S)
#define USB_EPTYPE6_V  0x00000003
#define USB_EPTYPE6_S  18
/** USB_SNP6 : R/W; bitpos: [20]; default: 0;
 *  0x0 : Reserved 0
 *  0x1 : Reserved 1
 */
#define USB_SNP6    (BIT(20))
#define USB_SNP6_M  (USB_SNP6_V << USB_SNP6_S)
#define USB_SNP6_V  0x00000001
#define USB_SNP6_S  20
/** USB_STALL6 : R/W; bitpos: [21]; default: 0;
 *  The application can only set this bit, and the core clears it, when a SETUP token
 *  is received for this endpoint
 *  0x0 (INACTIVE): No Stall
 *  0x1 (ACTIVE): Stall Handshake
 */
#define USB_STALL6    (BIT(21))
#define USB_STALL6_M  (USB_STALL6_V << USB_STALL6_S)
#define USB_STALL6_V  0x00000001
#define USB_STALL6_S  21
/** USB_CNAK6 : WO; bitpos: [26]; default: 0;
 *  0x0 : No action
 *  0x1 : Clear NAK
 */
#define USB_CNAK6    (BIT(26))
#define USB_CNAK6_M  (USB_CNAK6_V << USB_CNAK6_S)
#define USB_CNAK6_V  0x00000001
#define USB_CNAK6_S  26
/** USB_DO_SNAK6 : WO; bitpos: [27]; default: 0;
 *  A write to this bit sets the NAK bit for the endpoint
 *  0x0 : No action
 *  0x1 : Set NAK
 */
#define USB_DO_SNAK6    (BIT(27))
#define USB_DO_SNAK6_M  (USB_DO_SNAK6_V << USB_DO_SNAK6_S)
#define USB_DO_SNAK6_V  0x00000001
#define USB_DO_SNAK6_S  27
/** USB_DO_SETD0PID6 : WO; bitpos: [28]; default: 0;
 *  Set DATA0 PID
 */
#define USB_DO_SETD0PID6    (BIT(28))
#define USB_DO_SETD0PID6_M  (USB_DO_SETD0PID6_V << USB_DO_SETD0PID6_S)
#define USB_DO_SETD0PID6_V  0x00000001
#define USB_DO_SETD0PID6_S  28
/** USB_DO_SETD1PID6 : WO; bitpos: [29]; default: 0;
 *  Set DATA1 PID
 */
#define USB_DO_SETD1PID6    (BIT(29))
#define USB_DO_SETD1PID6_M  (USB_DO_SETD1PID6_V << USB_DO_SETD1PID6_S)
#define USB_DO_SETD1PID6_V  0x00000001
#define USB_DO_SETD1PID6_S  29
/** USB_EPDIS6 : RO; bitpos: [30]; default: 0;
 *  Endpoint Disable
 *  0x0 : No Endpoint disable
 */
#define USB_EPDIS6    (BIT(30))
#define USB_EPDIS6_M  (USB_EPDIS6_V << USB_EPDIS6_S)
#define USB_EPDIS6_V  0x00000001
#define USB_EPDIS6_S  30
/** USB_EPENA6 : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable
 *  0x0 : No action
 *  0x1 : Enable Endpoint
 */
#define USB_EPENA6    (BIT(31))
#define USB_EPENA6_M  (USB_EPENA6_V << USB_EPENA6_S)
#define USB_EPENA6_V  0x00000001
#define USB_EPENA6_S  31


/** USB_DOEPTSIZ6_REG register
 *  Device OUT Endpoint 6 Transfer Size Register
 */
#define USB_DOEPTSIZ6_REG (SOC_DPORT_USB_BASE + 0xbd0)
/** USB_XFERSIZE6 : R/W; bitpos: [7:0]; default: 0;
 *  Transfer Size.Indicates the transfer size in bytes for ENDPOINT6
 */
#define USB_XFERSIZE6    0x0000007F
#define USB_XFERSIZE6_M  (USB_XFERSIZE6_V << USB_XFERSIZE6_S)
#define USB_XFERSIZE6_V  0x0000007F
#define USB_XFERSIZE6_S  0
/** USB_PKTCNT6 : R/W; bitpos: [19]; default: 0;
 *  Packet Count (PktCnt).This field is decremented to zero after a packet is written
 *  into the RxFIFO.
 */
#define USB_PKTCNT6    (BIT(19))
#define USB_PKTCNT6_M  (USB_PKTCNT6_V << USB_PKTCNT6_S)
#define USB_PKTCNT6_V  0x00000001
#define USB_PKTCNT6_S  19
/** USB_SUPCNT6 : R/W; bitpos: [31:29]; default: 0;
 *  SETUP Packet Count (SUPCnt).This field specifies the number of back-to-back SETUP
 *  data packets the endpoint can receive
 *  2'b01: 1 packet
 *  2'b10: 2 packets
 *  2'b11: 3 packets
 */
#define USB_SUPCNT6    0x00000003
#define USB_SUPCNT6_M  (USB_SUPCNT6_V << USB_SUPCNT6_S)
#define USB_SUPCNT6_V  0x00000003
#define USB_SUPCNT6_S  29


/** USB_DOEPDMA6_REG register
 *  Device OUT Endpoint 6 DMA Address Register
 */
#define USB_DOEPDMA6_REG (SOC_DPORT_USB_BASE + 0xbd4)
/** USB_DMAADDR6 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 */
#define USB_DMAADDR6    0xFFFFFFFF
#define USB_DMAADDR6_M  (USB_DMAADDR6_V << USB_DMAADDR6_S)
#define USB_DMAADDR6_V  0xFFFFFFFF
#define USB_DMAADDR6_S  0


/** USB_DOEPDMAB6_REG register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
#define USB_DOEPDMAB6_REG (SOC_DPORT_USB_BASE + 0xbdc)
/** USB_DMABUFFERADDR6 : R/W; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_DMABUFFERADDR6    0xFFFFFFFF
#define USB_DMABUFFERADDR6_M  (USB_DMABUFFERADDR6_V << USB_DMABUFFERADDR6_S)
#define USB_DMABUFFERADDR6_V  0xFFFFFFFF
#define USB_DMABUFFERADDR6_S  0


/** USB_PCGCCTL_REG register
 *  Power and Clock Gating Control Register
 */
#define USB_PCGCCTL_REG (SOC_DPORT_USB_BASE + 0xe00)
/** USB_STOPPCLK : R/W; bitpos: [0]; default: 0;
 *  0x0 : Disable Stop Pclk
 *  0x1 : Enable Stop Pclk
 */
#define USB_STOPPCLK    (BIT(0))
#define USB_STOPPCLK_M  (USB_STOPPCLK_V << USB_STOPPCLK_S)
#define USB_STOPPCLK_V  0x00000001
#define USB_STOPPCLK_S  0
/** USB_GATEHCLK : R/W; bitpos: [1]; default: 0;
 *  gate hclk
 *  0x0:clears this bit when USB is resumed or a new session starts
 *  0x1:set this bit to gate hclk to modules, when the USB is suspended or the session
 *  is not valid
 */
#define USB_GATEHCLK    (BIT(1))
#define USB_GATEHCLK_M  (USB_GATEHCLK_V << USB_GATEHCLK_S)
#define USB_GATEHCLK_V  0x00000001
#define USB_GATEHCLK_S  1
/** USB_PWRCLMP : R/W; bitpos: [2]; default: 0;
 *  0x0:Clears this bit to disable the clamping before the power is turned on
 *  0x1:In only Partial Power-Down mode, sets this bit to clamp the signals between the
 *  power-on modules and the power-off modules before the power is turned off
 */
#define USB_PWRCLMP    (BIT(2))
#define USB_PWRCLMP_M  (USB_PWRCLMP_V << USB_PWRCLMP_S)
#define USB_PWRCLMP_V  0x00000001
#define USB_PWRCLMP_S  2
/** USB_RSTPDWNMODULE : R/W; bitpos: [3]; default: 0;
 *  Reset Power-Down Modules.
 *  0x0 : Power is turned on
 *  0x1 : Power is turned off
 */
#define USB_RSTPDWNMODULE    (BIT(3))
#define USB_RSTPDWNMODULE_M  (USB_RSTPDWNMODULE_V << USB_RSTPDWNMODULE_S)
#define USB_RSTPDWNMODULE_V  0x00000001
#define USB_RSTPDWNMODULE_S  3
/** USB_PHYSLEEP : RO; bitpos: [6]; default: 0;
 *  0x0 : Phy not in Sleep state
 *  0x1 : Phy in Sleep state
 */
#define USB_PHYSLEEP    (BIT(6))
#define USB_PHYSLEEP_M  (USB_PHYSLEEP_V << USB_PHYSLEEP_S)
#define USB_PHYSLEEP_V  0x00000001
#define USB_PHYSLEEP_S  6
/** USB_L1SUSPENDED : RO; bitpos: [7]; default: 0;
 *  L1 Deep Sleep
 *  0x0 : Non Deep Sleep
 *  0x1 : Deep Sleep
 */
#define USB_L1SUSPENDED    (BIT(7))
#define USB_L1SUSPENDED_M  (USB_L1SUSPENDED_V << USB_L1SUSPENDED_S)
#define USB_L1SUSPENDED_V  0x00000001
#define USB_L1SUSPENDED_S  7
/** USB_RESETAFTERSUSP : R/W; bitpos: [8]; default: 0;
 *  Reset after suspend
 *  0x0 : In Host-only mode, host issues Resume after Suspend
 *  0x1 : In Host-only mode, host sets this bit before clamp is removed if the host
 *  needs to issue Reset after Suspend
 */
#define USB_RESETAFTERSUSP    (BIT(8))
#define USB_RESETAFTERSUSP_M  (USB_RESETAFTERSUSP_V << USB_RESETAFTERSUSP_S)
#define USB_RESETAFTERSUSP_V  0x00000001
#define USB_RESETAFTERSUSP_S  8




/** Interrupt registers */
/** USB_GOTGINT_REG register
 *  OTG Interrupt Register
 */
#define USB_GOTGINT_REG (SOC_DPORT_USB_BASE + 0x4)
/** USB_SESENDDET : R/W1C; bitpos: [2]; default: 0;
 *  Session End Detected.The controller sets this bit when the utmiotg_bvalid signal is
 *  deasserted. This bit can be set only by the core and the application should write 1
 *  to clear it
 */
#define USB_SESENDDET    (BIT(2))
#define USB_SESENDDET_M  (USB_SESENDDET_V << USB_SESENDDET_S)
#define USB_SESENDDET_V  0x00000001
#define USB_SESENDDET_S  2
/** USB_SESREQSUCSTSCHNG : R/W1C; bitpos: [8]; default: 0;
 *  Session Request Success Status Change.The core sets this bit on the success or
 *  failure of a session request.The application must read the Session Request Success
 *  bit in the OTG Control and Status register (GOTGCTL_REG.USB_SESREQSCS) to check for
 *  success or failure. This bit can be set only by the core and the application should
 *  write 1 to clear it.
 */
#define USB_SESREQSUCSTSCHNG    (BIT(8))
#define USB_SESREQSUCSTSCHNG_M  (USB_SESREQSUCSTSCHNG_V << USB_SESREQSUCSTSCHNG_S)
#define USB_SESREQSUCSTSCHNG_V  0x00000001
#define USB_SESREQSUCSTSCHNG_S  8
/** USB_HSTNEGSUCSTSCHNG : R/W1C; bitpos: [9]; default: 0;
 *  Host Negotiation Success Status Change. The core sets this bit on the success or
 *  failure of a USB host negotiation request. The application must read the Host
 *  Negotiation Success bit of the OTG Control and Status register
 *  (GOTGCTL_REG.USB_HSTNEGSCS) to check for success or failure. This bit can be set
 *  only by the core and the application should write 1 to clear it
 */
#define USB_HSTNEGSUCSTSCHNG    (BIT(9))
#define USB_HSTNEGSUCSTSCHNG_M  (USB_HSTNEGSUCSTSCHNG_V << USB_HSTNEGSUCSTSCHNG_S)
#define USB_HSTNEGSUCSTSCHNG_V  0x00000001
#define USB_HSTNEGSUCSTSCHNG_S  9
/** USB_HSTNEGDET : R/W1C; bitpos: [17]; default: 0;
 *  Host Negotiation Detected.The core sets this bit when it detects a host negotiation
 *  request on the USB. This bit can be set only by the core and the application should
 *  write 1 to clear it.
 */
#define USB_HSTNEGDET    (BIT(17))
#define USB_HSTNEGDET_M  (USB_HSTNEGDET_V << USB_HSTNEGDET_S)
#define USB_HSTNEGDET_V  0x00000001
#define USB_HSTNEGDET_S  17
/** USB_ADEVTOUTCHG : R/W1C; bitpos: [18]; default: 0;
 *  A-Device Timeout Change. The core sets this bit to indicate that the A-device has
 *  timed out while waiting for the B-device to connect.This bit can be set only by the
 *  core and the application should write 1 to clear it
 */
#define USB_ADEVTOUTCHG    (BIT(18))
#define USB_ADEVTOUTCHG_M  (USB_ADEVTOUTCHG_V << USB_ADEVTOUTCHG_S)
#define USB_ADEVTOUTCHG_V  0x00000001
#define USB_ADEVTOUTCHG_S  18
/** USB_DBNCEDONE : R/W1C; bitpos: [19]; default: 0;
 *  Debounce Done. The core sets this bit when the debounce is completed after the
 *  device connect. The application can start driving USB reset after seeing this
 *  interrupt. This bit is only valid when the HNP Capable or SRP Capable bit is SET in
 *  the Core USB Configuration register (GUSBCFG.HNPCap or GUSBCFG.SRPCap,
 *  respectively). This bit can be set only by the core and the application should
 *  write 1 to clear it
 */
#define USB_DBNCEDONE    (BIT(19))
#define USB_DBNCEDONE_M  (USB_DBNCEDONE_V << USB_DBNCEDONE_S)
#define USB_DBNCEDONE_V  0x00000001
#define USB_DBNCEDONE_S  19


/** USB_GINTSTS_REG register
 *  Interrupt Register
 */
#define USB_GINTSTS_REG (SOC_DPORT_USB_BASE + 0x14)
/** USB_CURMOD_INT : RO; bitpos: [0]; default: 0;
 *  Current Mode of Operation
 *  1'b0: Device mode
 *  1'b1: Host mode
 */
#define USB_CURMOD_INT    (BIT(0))
#define USB_CURMOD_INT_M  (USB_CURMOD_INT_V << USB_CURMOD_INT_S)
#define USB_CURMOD_INT_V  0x00000001
#define USB_CURMOD_INT_S  0
/** USB_MODEMIS : R/W1C; bitpos: [1]; default: 0;
 *  Mode Mismatch Interrupt.The core sets this bit when the application is trying to
 *  access:A Host mode register, when the controller is operating in Device mode
 */
#define USB_MODEMIS    (BIT(1))
#define USB_MODEMIS_M  (USB_MODEMIS_V << USB_MODEMIS_S)
#define USB_MODEMIS_V  0x00000001
#define USB_MODEMIS_S  1
/** USB_OTGINT : RO; bitpos: [2]; default: 0;
 *  OTG Interrupt.The controller sets this bit to indicate an OTG protocol event. The
 *  application must read the OTG Interrupt Status (GOTGINT_REG) register to determine
 *  the exact event that caused this interrupt. The application must clear the
 *  appropriate status bit in the GOTGINT_REG register to clear this bit.
 */
#define USB_OTGINT    (BIT(2))
#define USB_OTGINT_M  (USB_OTGINT_V << USB_OTGINT_S)
#define USB_OTGINT_V  0x00000001
#define USB_OTGINT_S  2
/** USB_SOF : R/W1C; bitpos: [3]; default: 0;
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
#define USB_SOF    (BIT(3))
#define USB_SOF_M  (USB_SOF_V << USB_SOF_S)
#define USB_SOF_V  0x00000001
#define USB_SOF_S  3
/** USB_RXFLVI : RO; bitpos: [4]; default: 0;
 *  RxFIFO Non-Empty.Indicates that there is at least one packet pending to be read
 *  from the RxFIFO
 *  1'b0: Rx Fifo is empty
 *  1'b1: Rx Fifo is not empty
 */
#define USB_RXFLVI    (BIT(4))
#define USB_RXFLVI_M  (USB_RXFLVI_V << USB_RXFLVI_S)
#define USB_RXFLVI_V  0x00000001
#define USB_RXFLVI_S  4
/** USB_NPTXFEMP : RO; bitpos: [5]; default: 0;
 *  Non-periodic TxFIFO Empty.This interrupt is asserted when the Non-periodic TxFIFO
 *  is either half or completely empty, and there is space for at least one Entry to be
 *  written to the Non-periodic Transmit Request Queue. The half or completely empty
 *  status is determined by the Non-periodic TxFIFO Empty Level bit in the Core AHB
 *  Configuration register (GAHBCFG_REG.USB_NPTXFEMPLVL).
 */
#define USB_NPTXFEMP    (BIT(5))
#define USB_NPTXFEMP_M  (USB_NPTXFEMP_V << USB_NPTXFEMP_S)
#define USB_NPTXFEMP_V  0x00000001
#define USB_NPTXFEMP_S  5
/** USB_GINNAKEFF : RO; bitpos: [6]; default: 0;
 *  Device only.Global IN Non-periodic NAK Effective.Indicates that the Set Global
 *  Non-periodic IN NAK bit in the Device Control register (DCTL.SGNPInNak) set by the
 *  application, has taken effect in the core. That is, the core has sampled the Global
 *  IN NAK bit Set by the application. This bit can be cleared by clearing the Clear
 *  Global Non-periodic IN NAK bit in the Device Control register (DCTL.CGNPInNak).
 *  This interrupt does not necessarily mean that a NAK handshake is sent out on the
 *  USB. The STALL bit takes precedence over the NAK bit.
 */
#define USB_GINNAKEFF    (BIT(6))
#define USB_GINNAKEFF_M  (USB_GINNAKEFF_V << USB_GINNAKEFF_S)
#define USB_GINNAKEFF_V  0x00000001
#define USB_GINNAKEFF_S  6
/** USB_GOUTNAKEFF : RO; bitpos: [7]; default: 0;
 *  Device only.Global OUT NAK Effective.Indicates that the Set Global OUT NAK bit in
 *  the Device Control register (DCTL_REG.USB_SGOUTNAK), Set by the application, has
 *  taken effect in the core. This bit can be cleared by writing the Clear Global OUT
 *  NAK bit in the Device Control register (DCTL_REG.REG_CGOUTNAK).
 */
#define USB_GOUTNAKEFF    (BIT(7))
#define USB_GOUTNAKEFF_M  (USB_GOUTNAKEFF_V << USB_GOUTNAKEFF_S)
#define USB_GOUTNAKEFF_V  0x00000001
#define USB_GOUTNAKEFF_S  7
/** USB_ERLYSUSP : R/W1C; bitpos: [10]; default: 0;
 *  Device only.Early Suspend.The controller sets this bit to indicate that an Idle
 *  state has been detected on the USB for 3 ms.
 */
#define USB_ERLYSUSP    (BIT(10))
#define USB_ERLYSUSP_M  (USB_ERLYSUSP_V << USB_ERLYSUSP_S)
#define USB_ERLYSUSP_V  0x00000001
#define USB_ERLYSUSP_S  10
/** USB_USBSUSP : R/W1C; bitpos: [11]; default: 0;
 *  Device only.USB Suspend.The controller sets this bit to indicate that a suspend was
 *  detected on the USB. The controller enters the Suspended state when there is no
 *  activity on the linestate signal for an extended period of time.
 */
#define USB_USBSUSP    (BIT(11))
#define USB_USBSUSP_M  (USB_USBSUSP_V << USB_USBSUSP_S)
#define USB_USBSUSP_V  0x00000001
#define USB_USBSUSP_S  11
/** USB_USBRST : R/W1C; bitpos: [12]; default: 0;
 *  Device only.USB Reset.The controller sets this bit to indicate that a reset is
 *  detected on the USB
 */
#define USB_USBRST    (BIT(12))
#define USB_USBRST_M  (USB_USBRST_V << USB_USBRST_S)
#define USB_USBRST_V  0x00000001
#define USB_USBRST_S  12
/** USB_ENUMDONE : R/W1C; bitpos: [13]; default: 0;
 *  Device only.Enumeration Done.The core sets this bit to indicate that speed
 *  enumeration is complete. The application must read the Device Status (DSTS_REG)
 *  register to obtain the enumerated speed.
 */
#define USB_ENUMDONE    (BIT(13))
#define USB_ENUMDONE_M  (USB_ENUMDONE_V << USB_ENUMDONE_S)
#define USB_ENUMDONE_V  0x00000001
#define USB_ENUMDONE_S  13
/** USB_ISOOUTDROP : R/W1C; bitpos: [14]; default: 0;
 *  Device only.Isochronous OUT Packet Dropped Interrupt.The controller sets this bit
 *  when it fails to write an isochronous OUT packet into the RxFIFO because the RxFIFO
 *  does not have enough space to accommodate a maximum packet size packet for the
 *  isochronous OUT endpoint.
 */
#define USB_ISOOUTDROP    (BIT(14))
#define USB_ISOOUTDROP_M  (USB_ISOOUTDROP_V << USB_ISOOUTDROP_S)
#define USB_ISOOUTDROP_V  0x00000001
#define USB_ISOOUTDROP_S  14
/** USB_EOPF : R/W1C; bitpos: [15]; default: 0;
 *  Device only.End of Periodic Frame Interrupt.Indicates that the period specified in
 *  the Periodic Frame Interval field of the Device Configuration register
 *  (DCFG_REG.REG_PERFRINT) has been reached in the current microframe.
 */
#define USB_EOPF    (BIT(15))
#define USB_EOPF_M  (USB_EOPF_V << USB_EOPF_S)
#define USB_EOPF_V  0x00000001
#define USB_EOPF_S  15
/** USB_EPMIS : R/W1C; bitpos: [17]; default: 0;
 *  Device only.Endpoint Mismatch Interrupt.This interrupt is valid only in shared FIFO
 *  operation.Indicates that an IN token has been received for a non-periodic endpoint,
 *  but the data for another endpoint is present in the top of the Non-periodic
 *  Transmit FIFO and the IN endpoint mismatch count programmed by the application has
 *  expired.
 */
#define USB_EPMIS    (BIT(17))
#define USB_EPMIS_M  (USB_EPMIS_V << USB_EPMIS_S)
#define USB_EPMIS_V  0x00000001
#define USB_EPMIS_S  17
/** USB_IEPINT : RO; bitpos: [18]; default: 0;
 *  Device only.IN Endpoints Interrupt.The core sets this bit to indicate that an
 *  interrupt is pending on one of the IN endpoints of the core (in Device mode). The
 *  application must read the Device All Endpoints Interrupt (DAINT) register to
 *  determine the exact number of the IN endpoint on Device IN Endpoint-n Interrupt
 *  (DIEPINTn) register to determine the exact cause of the interrupt. The application
 *  must clear the appropriate status bit in the corresponding DIEPINTn register to
 *  clear this bit.
 */
#define USB_IEPINT    (BIT(18))
#define USB_IEPINT_M  (USB_IEPINT_V << USB_IEPINT_S)
#define USB_IEPINT_V  0x00000001
#define USB_IEPINT_S  18
/** USB_OEPINT : RO; bitpos: [19]; default: 0;
 *  Device only.OUT Endpoints Interrupt.The controller sets this bit to indicate that
 *  an interrupt is pending on one of the OUT endpoints of the core (in Device mode).
 *  The application must read the Device All Endpoints Interrupt (DAINT) register to
 *  determine the exact number of the OUT endpoint on which the interrupt occurred, and
 *  then read the corresponding Device OUT Endpoint-n Interrupt (DOEPINTn) register to
 *  determine the exact cause of the interrupt. The application must clear the
 *  appropriate status bit in the corresponding DOEPINTn register to clear this bit.
 */
#define USB_OEPINT    (BIT(19))
#define USB_OEPINT_M  (USB_OEPINT_V << USB_OEPINT_S)
#define USB_OEPINT_V  0x00000001
#define USB_OEPINT_S  19
/** USB_INCOMPISOIN : R/W1C; bitpos: [20]; default: 0;
 *  Device only.Incomplete Isochronous IN Transfer.The core sets this interrupt to
 *  indicate that there is at least one isochronous IN endpoint on which the transfer
 *  is not completed in the current microframe. This interrupt is asserted along with
 *  the End of Periodic Frame Interrupt (EOPF) bit in this register.
 */
#define USB_INCOMPISOIN    (BIT(20))
#define USB_INCOMPISOIN_M  (USB_INCOMPISOIN_V << USB_INCOMPISOIN_S)
#define USB_INCOMPISOIN_V  0x00000001
#define USB_INCOMPISOIN_S  20
/** USB_INCOMPIP : R/W1C; bitpos: [21]; default: 0;
 *  In Host mode, the core sets this interrupt bit when there are incomplete periodic
 *  transactions still pending which are scheduled for the current microframe.The
 *  Device mode, the core sets this interrupt to indicate that thereis at least one
 *  isochronous OUT endpoint on which the transfer is not completed in the current
 *  microframe. This interrupt is asserted along with the End of Periodic Frame
 *  Interrupt (EOPF) bit in this register.
 */
#define USB_INCOMPIP    (BIT(21))
#define USB_INCOMPIP_M  (USB_INCOMPIP_V << USB_INCOMPIP_S)
#define USB_INCOMPIP_V  0x00000001
#define USB_INCOMPIP_S  21
/** USB_FETSUSP : R/W1C; bitpos: [22]; default: 0;
 *  Device only.Data Fetch Suspended.This interrupt is valid only in DMA mode. This
 *  interrupt indicates that the core has stopped fetching data. For IN endpoints due
 *  to the unavailability of TxFIFO space or Request Queue space. This interrupt is
 *  used by the application for an endpoint mismatch algorithm
 */
#define USB_FETSUSP    (BIT(22))
#define USB_FETSUSP_M  (USB_FETSUSP_V << USB_FETSUSP_S)
#define USB_FETSUSP_V  0x00000001
#define USB_FETSUSP_S  22
/** USB_RESETDET : R/W1C; bitpos: [23]; default: 0;
 *  Reset detected Interrupt
 *  In Device mode, this interrupt is asserted when a reset is detected on the USB in
 *  partial power-down mode when the device is in Suspend
 *  In Host mode, this interrupt is not asserted
 */
#define USB_RESETDET    (BIT(23))
#define USB_RESETDET_M  (USB_RESETDET_V << USB_RESETDET_S)
#define USB_RESETDET_V  0x00000001
#define USB_RESETDET_S  23
/** USB_PRTLNT : RO; bitpos: [24]; default: 0;
 *  Host only.Host Port Interrupt.The core sets this bit to indicate a change in port
 *  status of one of the controller ports in Host mode. The application must read the
 *  Host Port Control and Status (HPRT) register to determine the exact event that
 *  caused this interrupt. The application must clear the appropriate status bit in the
 *  Host Port Control and Status register to clear this bit.
 */
#define USB_PRTLNT    (BIT(24))
#define USB_PRTLNT_M  (USB_PRTLNT_V << USB_PRTLNT_S)
#define USB_PRTLNT_V  0x00000001
#define USB_PRTLNT_S  24
/** USB_HCHLNT : RO; bitpos: [25]; default: 0;
 *  Host only.Host Channels Interrupt.The core sets this bit to indicate that an
 *  interrupt is pending on one of the channels of the core (in Host mode). The
 *  application must read the Host All Channels Interrupt (HAINT) register to determine
 *  the exact number of the channel on which the interrupt occurred,and Then read the
 *  corresponding Host Channel-n Interrupt (HCINTn) register to determine the exact
 *  cause of the interrupt.The application must clear the appropriate status bit in the
 *  HCINTn register to clear this bit.
 */
#define USB_HCHLNT    (BIT(25))
#define USB_HCHLNT_M  (USB_HCHLNT_V << USB_HCHLNT_S)
#define USB_HCHLNT_V  0x00000001
#define USB_HCHLNT_S  25
/** USB_PTXFEMP : RO; bitpos: [26]; default: 0;
 *  Host only.Periodic TxFIFO Empty.This interrupt is asserted when the Periodic
 *  Transmit FIFO is either half or completely empty and there is space for at least
 *  one entry to be written in the Periodic Request Queue. The half or completelyempty
 *  status is determined by the Periodic TxFIFO Empty Level bit in the Core AHB
 *  Configuration register (GAHBCFG.PTxFEmpLvl)
 */
#define USB_PTXFEMP    (BIT(26))
#define USB_PTXFEMP_M  (USB_PTXFEMP_V << USB_PTXFEMP_S)
#define USB_PTXFEMP_V  0x00000001
#define USB_PTXFEMP_S  26
/** USB_CONIDSTSCHNG : R/W1C; bitpos: [28]; default: 0;
 *  Connector ID Status Change
 *  1'b0:Not active
 *  1'b1:Connector ID Status Change
 */
#define USB_CONIDSTSCHNG    (BIT(28))
#define USB_CONIDSTSCHNG_M  (USB_CONIDSTSCHNG_V << USB_CONIDSTSCHNG_S)
#define USB_CONIDSTSCHNG_V  0x00000001
#define USB_CONIDSTSCHNG_S  28
/** USB_DISCONNINT : R/W1C; bitpos: [29]; default: 0;
 *  Disconnect Detected Interrupt
 *  1'b0:Not active
 *  1'b1:Disconnect Detected Interrupt
 */
#define USB_DISCONNINT    (BIT(29))
#define USB_DISCONNINT_M  (USB_DISCONNINT_V << USB_DISCONNINT_S)
#define USB_DISCONNINT_V  0x00000001
#define USB_DISCONNINT_S  29
/** USB_SESSREQINT : R/W1C; bitpos: [30]; default: 0;
 *  1'b0:Not active
 *  1'b1:Session Request New Session Detected
 */
#define USB_SESSREQINT    (BIT(30))
#define USB_SESSREQINT_M  (USB_SESSREQINT_V << USB_SESSREQINT_S)
#define USB_SESSREQINT_V  0x00000001
#define USB_SESSREQINT_S  30
/** USB_WKUPINT : R/W1C; bitpos: [31]; default: 0;
 *  Resume/Remote Wakeup Detected Interrupt
 *  1'b0:Not active
 *  1'b1:Resume or Remote Wakeup Detected Interrupt
 */
#define USB_WKUPINT    (BIT(31))
#define USB_WKUPINT_M  (USB_WKUPINT_V << USB_WKUPINT_S)
#define USB_WKUPINT_V  0x00000001
#define USB_WKUPINT_S  31


/** USB_GINTMSK_REG register
 *  Interrupt Mask Register
 */
#define USB_GINTMSK_REG (SOC_DPORT_USB_BASE + 0x18)
/** USB_MODEMISMSK : R/W; bitpos: [1]; default: 0;
 *  1'b0:Mode Mismatch Interrupt Mask
 *  1'b1:No Mode Mismatch Interrupt Mask
 */
#define USB_MODEMISMSK    (BIT(1))
#define USB_MODEMISMSK_M  (USB_MODEMISMSK_V << USB_MODEMISMSK_S)
#define USB_MODEMISMSK_V  0x00000001
#define USB_MODEMISMSK_S  1
/** USB_OTGINTMSK : R/W; bitpos: [2]; default: 0;
 *  1'b0:OTG Interrupt Mask
 *  1'b1:No OTG Interrupt Mask
 */
#define USB_OTGINTMSK    (BIT(2))
#define USB_OTGINTMSK_M  (USB_OTGINTMSK_V << USB_OTGINTMSK_S)
#define USB_OTGINTMSK_V  0x00000001
#define USB_OTGINTMSK_S  2
/** USB_SOFMSK : R/W; bitpos: [3]; default: 0;
 *  1'b0:Start of (micro)Frame Mask
 *  1'b1:No Start of (micro)Frame Mask
 */
#define USB_SOFMSK    (BIT(3))
#define USB_SOFMSK_M  (USB_SOFMSK_V << USB_SOFMSK_S)
#define USB_SOFMSK_V  0x00000001
#define USB_SOFMSK_S  3
/** USB_RXFLVIMSK : R/W; bitpos: [4]; default: 0;
 *  1'b0:Receive FIFO Non-Empty Mask
 *  1'b1:No Receive FIFO Non-Empty Mask
 */
#define USB_RXFLVIMSK    (BIT(4))
#define USB_RXFLVIMSK_M  (USB_RXFLVIMSK_V << USB_RXFLVIMSK_S)
#define USB_RXFLVIMSK_V  0x00000001
#define USB_RXFLVIMSK_S  4
/** USB_NPTXFEMPMSK : R/W; bitpos: [5]; default: 0;
 *  1'b0:Non-periodic TxFIFO Empty Mask
 *  1'b1:No Non-periodic TxFIFO Empty Mask
 */
#define USB_NPTXFEMPMSK    (BIT(5))
#define USB_NPTXFEMPMSK_M  (USB_NPTXFEMPMSK_V << USB_NPTXFEMPMSK_S)
#define USB_NPTXFEMPMSK_V  0x00000001
#define USB_NPTXFEMPMSK_S  5
/** USB_GINNAKEFFMSK : R/W; bitpos: [6]; default: 0;
 *  1'b0:Global Non-periodic IN NAK Effective Mask
 *  1'b1:No Global Non-periodic IN NAK Effective Mask
 */
#define USB_GINNAKEFFMSK    (BIT(6))
#define USB_GINNAKEFFMSK_M  (USB_GINNAKEFFMSK_V << USB_GINNAKEFFMSK_S)
#define USB_GINNAKEFFMSK_V  0x00000001
#define USB_GINNAKEFFMSK_S  6
/** USB_GOUTNACKEFFMSK : R/W; bitpos: [7]; default: 0;
 *  1'b0:Global OUT NAK Effective Mask
 *  1'b1:No Global OUT NAK Effective Mask
 */
#define USB_GOUTNACKEFFMSK    (BIT(7))
#define USB_GOUTNACKEFFMSK_M  (USB_GOUTNACKEFFMSK_V << USB_GOUTNACKEFFMSK_S)
#define USB_GOUTNACKEFFMSK_V  0x00000001
#define USB_GOUTNACKEFFMSK_S  7
/** USB_ERLYSUSPMSK : R/W; bitpos: [10]; default: 0;
 *  1'b0:Early Suspend Mask
 *  1'b1:No Early Suspend Mask
 */
#define USB_ERLYSUSPMSK    (BIT(10))
#define USB_ERLYSUSPMSK_M  (USB_ERLYSUSPMSK_V << USB_ERLYSUSPMSK_S)
#define USB_ERLYSUSPMSK_V  0x00000001
#define USB_ERLYSUSPMSK_S  10
/** USB_USBSUSPMSK : R/W; bitpos: [11]; default: 0;
 *  1'b0:USB Suspend Mask
 *  1'b1:No USB Suspend Mask
 */
#define USB_USBSUSPMSK    (BIT(11))
#define USB_USBSUSPMSK_M  (USB_USBSUSPMSK_V << USB_USBSUSPMSK_S)
#define USB_USBSUSPMSK_V  0x00000001
#define USB_USBSUSPMSK_S  11
/** USB_USBRSTMSK : R/W; bitpos: [12]; default: 0;
 *  1'b0:USB Reset Mask
 *  1'b1:No USB Reset Mask
 */
#define USB_USBRSTMSK    (BIT(12))
#define USB_USBRSTMSK_M  (USB_USBRSTMSK_V << USB_USBRSTMSK_S)
#define USB_USBRSTMSK_V  0x00000001
#define USB_USBRSTMSK_S  12
/** USB_ENUMDONEMSK : R/W; bitpos: [13]; default: 0;
 *  1'b0: Enumeration Done Mask
 *  1'b1: No Enumeration Done Mask
 */
#define USB_ENUMDONEMSK    (BIT(13))
#define USB_ENUMDONEMSK_M  (USB_ENUMDONEMSK_V << USB_ENUMDONEMSK_S)
#define USB_ENUMDONEMSK_V  0x00000001
#define USB_ENUMDONEMSK_S  13
/** USB_ISOOUTDROPMSK : R/W; bitpos: [14]; default: 0;
 *  1'b0: Isochronous OUT Packet Dropped Interrupt Mask
 *  1'b1: No Isochronous OUT Packet Dropped Interrupt Mask
 */
#define USB_ISOOUTDROPMSK    (BIT(14))
#define USB_ISOOUTDROPMSK_M  (USB_ISOOUTDROPMSK_V << USB_ISOOUTDROPMSK_S)
#define USB_ISOOUTDROPMSK_V  0x00000001
#define USB_ISOOUTDROPMSK_S  14
/** USB_EOPFMSK : R/W; bitpos: [15]; default: 0;
 *  1'b0: End of Periodic Frame Interrupt Mask
 *  1'b1: No End of Periodic Frame Interrupt Mask
 */
#define USB_EOPFMSK    (BIT(15))
#define USB_EOPFMSK_M  (USB_EOPFMSK_V << USB_EOPFMSK_S)
#define USB_EOPFMSK_V  0x00000001
#define USB_EOPFMSK_S  15
/** USB_EPMISMSK : R/W; bitpos: [17]; default: 0;
 *  1'b0: Endpoint Mismatch Interrupt Mask
 *  1'b1: No Endpoint Mismatch Interrupt Mask
 */
#define USB_EPMISMSK    (BIT(17))
#define USB_EPMISMSK_M  (USB_EPMISMSK_V << USB_EPMISMSK_S)
#define USB_EPMISMSK_V  0x00000001
#define USB_EPMISMSK_S  17
/** USB_IEPINTMSK : R/W; bitpos: [18]; default: 0;
 *  1'b0: IN Endpoints Interrupt Mask
 *  1'b1: No IN Endpoints Interrupt Mask
 */
#define USB_IEPINTMSK    (BIT(18))
#define USB_IEPINTMSK_M  (USB_IEPINTMSK_V << USB_IEPINTMSK_S)
#define USB_IEPINTMSK_V  0x00000001
#define USB_IEPINTMSK_S  18
/** USB_OEPINTMSK : R/W; bitpos: [19]; default: 0;
 *  1'b0: OUT Endpoints Interrupt Mask
 *  1'b1: No OUT Endpoints Interrupt Mask
 */
#define USB_OEPINTMSK    (BIT(19))
#define USB_OEPINTMSK_M  (USB_OEPINTMSK_V << USB_OEPINTMSK_S)
#define USB_OEPINTMSK_V  0x00000001
#define USB_OEPINTMSK_S  19
/** USB_INCOMPISOINMSK : R/W; bitpos: [20]; default: 0;
 *  1'b0: Incomplete Isochronous IN Transfer Mask
 *  1'b1: No Incomplete Isochronous IN Transfer Mask
 */
#define USB_INCOMPISOINMSK    (BIT(20))
#define USB_INCOMPISOINMSK_M  (USB_INCOMPISOINMSK_V << USB_INCOMPISOINMSK_S)
#define USB_INCOMPISOINMSK_V  0x00000001
#define USB_INCOMPISOINMSK_S  20
/** USB_INCOMPIPMSK : R/W; bitpos: [21]; default: 0;
 *  1'b0: Host mode: Incomplete Periodic Transfer Mask Device mode: Incomplete
 *  Isochronous OUT Transfer Mask
 *  1'b1: Host mode: No Incomplete Periodic Transfer Mask  Device mode: No Incomplete
 *  Isochronous OUT Transfer Mask
 */
#define USB_INCOMPIPMSK    (BIT(21))
#define USB_INCOMPIPMSK_M  (USB_INCOMPIPMSK_V << USB_INCOMPIPMSK_S)
#define USB_INCOMPIPMSK_V  0x00000001
#define USB_INCOMPIPMSK_S  21
/** USB_FETSUSPMSK : R/W; bitpos: [22]; default: 0;
 *  1'b0: Data Fetch Suspended Mask
 *  1'b1: No Data Fetch Suspended Mask
 */
#define USB_FETSUSPMSK    (BIT(22))
#define USB_FETSUSPMSK_M  (USB_FETSUSPMSK_V << USB_FETSUSPMSK_S)
#define USB_FETSUSPMSK_V  0x00000001
#define USB_FETSUSPMSK_S  22
/** USB_RESETDETMSK : R/W; bitpos: [23]; default: 0;
 *  1'b0: Reset detected Interrupt Mask
 *  1'b1: No Reset detected Interrupt Mask
 */
#define USB_RESETDETMSK    (BIT(23))
#define USB_RESETDETMSK_M  (USB_RESETDETMSK_V << USB_RESETDETMSK_S)
#define USB_RESETDETMSK_V  0x00000001
#define USB_RESETDETMSK_S  23
/** USB_PRTLNTMSK : R/W; bitpos: [24]; default: 0;
 *  1'b0: Host Port Interrupt Mask
 *  1'b1: No Host Port Interrupt Mask
 */
#define USB_PRTLNTMSK    (BIT(24))
#define USB_PRTLNTMSK_M  (USB_PRTLNTMSK_V << USB_PRTLNTMSK_S)
#define USB_PRTLNTMSK_V  0x00000001
#define USB_PRTLNTMSK_S  24
/** USB_HCHINTMSK : R/W; bitpos: [25]; default: 0;
 *  1'b0: Host Channels Interrupt Mask
 *  1'b1: No Host Channels Interrupt Mask
 */
#define USB_HCHINTMSK    (BIT(25))
#define USB_HCHINTMSK_M  (USB_HCHINTMSK_V << USB_HCHINTMSK_S)
#define USB_HCHINTMSK_V  0x00000001
#define USB_HCHINTMSK_S  25
/** USB_PTXFEMPMSK : R/W; bitpos: [26]; default: 0;
 *  1'b0: Periodic TxFIFO Empty Mask
 *  1'b1: No Periodic TxFIFO Empty Mask
 */
#define USB_PTXFEMPMSK    (BIT(26))
#define USB_PTXFEMPMSK_M  (USB_PTXFEMPMSK_V << USB_PTXFEMPMSK_S)
#define USB_PTXFEMPMSK_V  0x00000001
#define USB_PTXFEMPMSK_S  26
/** USB_CONIDSTSCHNGMSK : R/W; bitpos: [28]; default: 0;
 *  1'b0: Connector ID Status Change Mask
 *  1'b1: No Connector ID Status Change Mask
 */
#define USB_CONIDSTSCHNGMSK    (BIT(28))
#define USB_CONIDSTSCHNGMSK_M  (USB_CONIDSTSCHNGMSK_V << USB_CONIDSTSCHNGMSK_S)
#define USB_CONIDSTSCHNGMSK_V  0x00000001
#define USB_CONIDSTSCHNGMSK_S  28
/** USB_DISCONNINTMSK : R/W; bitpos: [29]; default: 0;
 *  1'b0: Disconnect Detected Interrupt Mask
 *  1'b1: No Disconnect Detected Interrupt Mask
 */
#define USB_DISCONNINTMSK    (BIT(29))
#define USB_DISCONNINTMSK_M  (USB_DISCONNINTMSK_V << USB_DISCONNINTMSK_S)
#define USB_DISCONNINTMSK_V  0x00000001
#define USB_DISCONNINTMSK_S  29
/** USB_SESSREQINTMSK : R/W; bitpos: [30]; default: 0;
 *  1'b0: Session Request or New Session Detected Interrupt Mask
 *  1'b1: No Session Request or New Session Detected Interrupt Mask
 */
#define USB_SESSREQINTMSK    (BIT(30))
#define USB_SESSREQINTMSK_M  (USB_SESSREQINTMSK_V << USB_SESSREQINTMSK_S)
#define USB_SESSREQINTMSK_V  0x00000001
#define USB_SESSREQINTMSK_S  30
/** USB_WKUPINTMSK : R/W; bitpos: [31]; default: 0;
 *  1'b0 : Resume or Remote Wakeup Detected Interrupt Mask
 *  1'b1 : Unmask Resume Remote Wakeup Detected Interrupt
 */
#define USB_WKUPINTMSK    (BIT(31))
#define USB_WKUPINTMSK_M  (USB_WKUPINTMSK_V << USB_WKUPINTMSK_S)
#define USB_WKUPINTMSK_V  0x00000001
#define USB_WKUPINTMSK_S  31


/** USB_HAINT_REG register
 *  Host All Channels Interrupt Register
 */
#define USB_HAINT_REG (SOC_DPORT_USB_BASE + 0x414)
/** USB_HAINT : RO; bitpos: [8:0]; default: 0;
 *  Channel Interrupt for channel no.
 */
#define USB_HAINT    0x000000FF
#define USB_HAINT_M  (USB_HAINT_V << USB_HAINT_S)
#define USB_HAINT_V  0x000000FF
#define USB_HAINT_S  0


/** USB_HAINTMSK_REG register
 *  Host All Channels Interrupt Mask Register
 */
#define USB_HAINTMSK_REG (SOC_DPORT_USB_BASE + 0x418)
/** USB_HAINTMSK : R/W; bitpos: [8:0]; default: 0;
 *  Channel Interrupt Mask (HAINTMSK_REG) One bit per channel: Bit 0 for channel 0, bit
 *  15 for channel 15.
 */
#define USB_HAINTMSK    0x000000FF
#define USB_HAINTMSK_M  (USB_HAINTMSK_V << USB_HAINTMSK_S)
#define USB_HAINTMSK_V  0x000000FF
#define USB_HAINTMSK_S  0


/** USB_HCINT0_REG register
 *  Host Channel 0 Interrupt Register
 */
#define USB_HCINT0_REG (SOC_DPORT_USB_BASE + 0x508)
/** USB_H_XFERCOMPL0 : R/W1C; bitpos: [0]; default: 0;
 *  1'b0: Transfer in progress or No Active Transfer
 *  1'b1: Transfer completed normally without any errors
 */
#define USB_H_XFERCOMPL0    (BIT(0))
#define USB_H_XFERCOMPL0_M  (USB_H_XFERCOMPL0_V << USB_H_XFERCOMPL0_S)
#define USB_H_XFERCOMPL0_V  0x00000001
#define USB_H_XFERCOMPL0_S  0
/** USB_H_CHHLTD0 : R/W1C; bitpos: [1]; default: 0;
 *  1'b0: Channel not halted
 *  1'b1: Channel Halted
 */
#define USB_H_CHHLTD0    (BIT(1))
#define USB_H_CHHLTD0_M  (USB_H_CHHLTD0_V << USB_H_CHHLTD0_S)
#define USB_H_CHHLTD0_V  0x00000001
#define USB_H_CHHLTD0_S  1
/** USB_H_AHBERR0 : R/W1C; bitpos: [2]; default: 0;
 *  1'b0: No AHB error
 *  1'b1: AHB error during AHB read/write
 */
#define USB_H_AHBERR0    (BIT(2))
#define USB_H_AHBERR0_M  (USB_H_AHBERR0_V << USB_H_AHBERR0_S)
#define USB_H_AHBERR0_V  0x00000001
#define USB_H_AHBERR0_S  2
/** USB_H_STALL0 : R/W1C; bitpos: [3]; default: 0;
 *  1'b0: No Stall Response Received Interrupt
 *  1'b1: Stall Response Received Interrupt
 */
#define USB_H_STALL0    (BIT(3))
#define USB_H_STALL0_M  (USB_H_STALL0_V << USB_H_STALL0_S)
#define USB_H_STALL0_V  0x00000001
#define USB_H_STALL0_S  3
/** USB_H_NACK0 : R/W1C; bitpos: [4]; default: 0;
 *  1'b0: No NAK Response Received Interrupt
 *  1'b1: NAK Response Received Interrupt
 */
#define USB_H_NACK0    (BIT(4))
#define USB_H_NACK0_M  (USB_H_NACK0_V << USB_H_NACK0_S)
#define USB_H_NACK0_V  0x00000001
#define USB_H_NACK0_S  4
/** USB_H_ACK0 : R/W1C; bitpos: [5]; default: 0;
 *  1'b0: No ACK Response Received or Transmitted Interrupt
 *  1'b1: ACK Response Received or Transmitted Interrup
 */
#define USB_H_ACK0    (BIT(5))
#define USB_H_ACK0_M  (USB_H_ACK0_V << USB_H_ACK0_S)
#define USB_H_ACK0_V  0x00000001
#define USB_H_ACK0_S  5
/** USB_H_NYET0 : R/W1C; bitpos: [6]; default: 0;
 *  1'b0: No NYET Response Received Interrupt
 *  1'b1: NYET Response Received Interrupt
 */
#define USB_H_NYET0    (BIT(6))
#define USB_H_NYET0_M  (USB_H_NYET0_V << USB_H_NYET0_S)
#define USB_H_NYET0_V  0x00000001
#define USB_H_NYET0_S  6
/** USB_H_XACTERR0 : R/W1C; bitpos: [7]; default: 0;
 *  Indicates one of the following errors occurred on the USB:
 *  CRC check failure
 *  Timeout
 *  Bit stuff error
 *  False EOP
 */
#define USB_H_XACTERR0    (BIT(7))
#define USB_H_XACTERR0_M  (USB_H_XACTERR0_V << USB_H_XACTERR0_S)
#define USB_H_XACTERR0_V  0x00000001
#define USB_H_XACTERR0_S  7
/** USB_H_BBLERR0 : R/W1C; bitpos: [8]; default: 0;
 *  1'b0: No Babble Error
 *  1'b1: Babble Error
 */
#define USB_H_BBLERR0    (BIT(8))
#define USB_H_BBLERR0_M  (USB_H_BBLERR0_V << USB_H_BBLERR0_S)
#define USB_H_BBLERR0_V  0x00000001
#define USB_H_BBLERR0_S  8
/** USB_H_FRMOVRUN0 : R/W1C; bitpos: [9]; default: 0;
 *  1'b0: No Frame Overrun
 *  1'b1: Frame Overrun
 */
#define USB_H_FRMOVRUN0    (BIT(9))
#define USB_H_FRMOVRUN0_M  (USB_H_FRMOVRUN0_V << USB_H_FRMOVRUN0_S)
#define USB_H_FRMOVRUN0_V  0x00000001
#define USB_H_FRMOVRUN0_S  9
/** USB_H_DATATGLERR0 : R/W1C; bitpos: [10]; default: 0;
 *  1'b0: No Data Toggle Error
 *  1'b1: Data Toggle Error
 */
#define USB_H_DATATGLERR0    (BIT(10))
#define USB_H_DATATGLERR0_M  (USB_H_DATATGLERR0_V << USB_H_DATATGLERR0_S)
#define USB_H_DATATGLERR0_V  0x00000001
#define USB_H_DATATGLERR0_S  10
/** USB_H_BNAINTR0 : R/W1C; bitpos: [11]; default: 0;
 *  1'b0: No BNA Interrupt
 *  1'b1: BNA Interrupt
 */
#define USB_H_BNAINTR0    (BIT(11))
#define USB_H_BNAINTR0_M  (USB_H_BNAINTR0_V << USB_H_BNAINTR0_S)
#define USB_H_BNAINTR0_V  0x00000001
#define USB_H_BNAINTR0_S  11
/** USB_H_XCS_XACT_ERR0 : R/W1C; bitpos: [12]; default: 0;
 *  1'b0: No Excessive Transaction Error
 *  1'b1: Excessive Transaction Error
 */
#define USB_H_XCS_XACT_ERR0    (BIT(12))
#define USB_H_XCS_XACT_ERR0_M  (USB_H_XCS_XACT_ERR0_V << USB_H_XCS_XACT_ERR0_S)
#define USB_H_XCS_XACT_ERR0_V  0x00000001
#define USB_H_XCS_XACT_ERR0_S  12
/** USB_H_DESC_LST_ROLLINTR0 : R/W1C; bitpos: [13]; default: 0;
 *  1'b0: No Descriptor rollover interrupt
 *  1'b1: Descriptor rollover interrupt
 */
#define USB_H_DESC_LST_ROLLINTR0    (BIT(13))
#define USB_H_DESC_LST_ROLLINTR0_M  (USB_H_DESC_LST_ROLLINTR0_V << USB_H_DESC_LST_ROLLINTR0_S)
#define USB_H_DESC_LST_ROLLINTR0_V  0x00000001
#define USB_H_DESC_LST_ROLLINTR0_S  13


/** USB_HCINTMSK0_REG register
 *  Host Channel 0 Interrupt Mask Register
 */
#define USB_HCINTMSK0_REG (SOC_DPORT_USB_BASE + 0x50c)
/** USB_H_XFERCOMPLMSK0 : R/W; bitpos: [0]; default: 0;
 *  1'b0: Transfer Completed Mask
 *  1'b1: No Transfer Completed Mask
 */
#define USB_H_XFERCOMPLMSK0    (BIT(0))
#define USB_H_XFERCOMPLMSK0_M  (USB_H_XFERCOMPLMSK0_V << USB_H_XFERCOMPLMSK0_S)
#define USB_H_XFERCOMPLMSK0_V  0x00000001
#define USB_H_XFERCOMPLMSK0_S  0
/** USB_H_CHHLTDMSK0 : R/W; bitpos: [1]; default: 0;
 *  1'b0: Channel Halted Mask
 *  1'b1: No Channel Halted Mask
 */
#define USB_H_CHHLTDMSK0    (BIT(1))
#define USB_H_CHHLTDMSK0_M  (USB_H_CHHLTDMSK0_V << USB_H_CHHLTDMSK0_S)
#define USB_H_CHHLTDMSK0_V  0x00000001
#define USB_H_CHHLTDMSK0_S  1
/** USB_H_AHBERRMSK0 : R/W; bitpos: [2]; default: 0;
 *  1'b0: AHB Error Mask
 *  1'b1: No AHB Error Mask
 */
#define USB_H_AHBERRMSK0    (BIT(2))
#define USB_H_AHBERRMSK0_M  (USB_H_AHBERRMSK0_V << USB_H_AHBERRMSK0_S)
#define USB_H_AHBERRMSK0_V  0x00000001
#define USB_H_AHBERRMSK0_S  2
/** USB_H_STALLMSK0 : R/W; bitpos: [3]; default: 0;
 *  1'b0: Mask STALL Response Received Interrupt
 *  1'b1: No STALL Response Received Interrupt Mask
 */
#define USB_H_STALLMSK0    (BIT(3))
#define USB_H_STALLMSK0_M  (USB_H_STALLMSK0_V << USB_H_STALLMSK0_S)
#define USB_H_STALLMSK0_V  0x00000001
#define USB_H_STALLMSK0_S  3
/** USB_H_NAKMSK0 : R/W; bitpos: [4]; default: 0;
 *  1'b0: Mask NAK Response Received Interrupt
 *  1'b1: No NAK Response Received Interrupt Mask
 */
#define USB_H_NAKMSK0    (BIT(4))
#define USB_H_NAKMSK0_M  (USB_H_NAKMSK0_V << USB_H_NAKMSK0_S)
#define USB_H_NAKMSK0_V  0x00000001
#define USB_H_NAKMSK0_S  4
/** USB_H_ACKMSK0 : R/W; bitpos: [5]; default: 0;
 *  1'b0: Mask ACK Response Received/Transmitted Interrupt
 *  1'b1: No ACK Response Received/Transmitted Interrupt Mask
 */
#define USB_H_ACKMSK0    (BIT(5))
#define USB_H_ACKMSK0_M  (USB_H_ACKMSK0_V << USB_H_ACKMSK0_S)
#define USB_H_ACKMSK0_V  0x00000001
#define USB_H_ACKMSK0_S  5
/** USB_H_NYETMSK0 : R/W; bitpos: [6]; default: 0;
 *  1'b0: Mask NYET Response Received Interrupt
 *  1'b1: No NYET Response Received Interrupt Mask
 */
#define USB_H_NYETMSK0    (BIT(6))
#define USB_H_NYETMSK0_M  (USB_H_NYETMSK0_V << USB_H_NYETMSK0_S)
#define USB_H_NYETMSK0_V  0x00000001
#define USB_H_NYETMSK0_S  6
/** USB_H_XACTERRMSK0 : R/W; bitpos: [7]; default: 0;
 *  1'b0: Mask Transaction Error
 *  1'b1: No Transaction Error Mask
 */
#define USB_H_XACTERRMSK0    (BIT(7))
#define USB_H_XACTERRMSK0_M  (USB_H_XACTERRMSK0_V << USB_H_XACTERRMSK0_S)
#define USB_H_XACTERRMSK0_V  0x00000001
#define USB_H_XACTERRMSK0_S  7
/** USB_H_BBLERRMSK0 : R/W; bitpos: [8]; default: 0;
 *  Babble Error Mask
 *  1'b0: Mask Babble Error
 *  1'b1: No Babble Error Mask
 */
#define USB_H_BBLERRMSK0    (BIT(8))
#define USB_H_BBLERRMSK0_M  (USB_H_BBLERRMSK0_V << USB_H_BBLERRMSK0_S)
#define USB_H_BBLERRMSK0_V  0x00000001
#define USB_H_BBLERRMSK0_S  8
/** USB_H_FRMOVRUNMSK0 : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun Mask
 *  0x0 (MASK): Mask Overrun Mask
 *  0x1 (NOMASK): No Frame Overrun Mask
 */
#define USB_H_FRMOVRUNMSK0    (BIT(9))
#define USB_H_FRMOVRUNMSK0_M  (USB_H_FRMOVRUNMSK0_V << USB_H_FRMOVRUNMSK0_S)
#define USB_H_FRMOVRUNMSK0_V  0x00000001
#define USB_H_FRMOVRUNMSK0_S  9
/** USB_H_DATATGLERRMSK0 : R/W; bitpos: [10]; default: 0;
 *  Data Toggle Error Mask n scatter/gather DMA mode for host
 *  1'b0: Mask Data Toggle Error
 *  1'b1: No Data Toggle Error Mask
 */
#define USB_H_DATATGLERRMSK0    (BIT(10))
#define USB_H_DATATGLERRMSK0_M  (USB_H_DATATGLERRMSK0_V << USB_H_DATATGLERRMSK0_S)
#define USB_H_DATATGLERRMSK0_V  0x00000001
#define USB_H_DATATGLERRMSK0_S  10
/** USB_H_BNAINTRMSK0 : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt mask register
 *  1'b0: BNA Interrupt Masked
 *  1'b1: BNA Interrupt not masked
 */
#define USB_H_BNAINTRMSK0    (BIT(11))
#define USB_H_BNAINTRMSK0_M  (USB_H_BNAINTRMSK0_V << USB_H_BNAINTRMSK0_S)
#define USB_H_BNAINTRMSK0_V  0x00000001
#define USB_H_BNAINTRMSK0_S  11
/** USB_H_DESC_LST_ROLLINTRMSK0 : R/W; bitpos: [13]; default: 0;
 *  Descriptor List rollover interrupt Mask
 *  1'b0: Descriptor Rollover Interrupt Mask
 *  1'b1: Descriptor Rollover Interrupt not masked
 */
#define USB_H_DESC_LST_ROLLINTRMSK0    (BIT(13))
#define USB_H_DESC_LST_ROLLINTRMSK0_M  (USB_H_DESC_LST_ROLLINTRMSK0_V << USB_H_DESC_LST_ROLLINTRMSK0_S)
#define USB_H_DESC_LST_ROLLINTRMSK0_V  0x00000001
#define USB_H_DESC_LST_ROLLINTRMSK0_S  13


/** USB_HCINT1_REG register
 *  Host Channel 1 Interrupt Register
 */
#define USB_HCINT1_REG (SOC_DPORT_USB_BASE + 0x528)
/** USB_H_XFERCOMPL1 : R/W1C; bitpos: [0]; default: 0;
 *  1'b0: Transfer in progress or No Active Transfer
 *  1'b1: Transfer completed normally without any errors
 */
#define USB_H_XFERCOMPL1    (BIT(0))
#define USB_H_XFERCOMPL1_M  (USB_H_XFERCOMPL1_V << USB_H_XFERCOMPL1_S)
#define USB_H_XFERCOMPL1_V  0x00000001
#define USB_H_XFERCOMPL1_S  0
/** USB_H_CHHLTD1 : R/W1C; bitpos: [1]; default: 0;
 *  1'b0: Channel not halted
 *  1'b1: Channel Halted
 */
#define USB_H_CHHLTD1    (BIT(1))
#define USB_H_CHHLTD1_M  (USB_H_CHHLTD1_V << USB_H_CHHLTD1_S)
#define USB_H_CHHLTD1_V  0x00000001
#define USB_H_CHHLTD1_S  1
/** USB_H_AHBERR1 : R/W1C; bitpos: [2]; default: 0;
 *  1'b0: No AHB error
 *  1'b1: AHB error during AHB read/write
 */
#define USB_H_AHBERR1    (BIT(2))
#define USB_H_AHBERR1_M  (USB_H_AHBERR1_V << USB_H_AHBERR1_S)
#define USB_H_AHBERR1_V  0x00000001
#define USB_H_AHBERR1_S  2
/** USB_H_STALL1 : R/W1C; bitpos: [3]; default: 0;
 *  1'b0: No Stall Response Received Interrupt
 *  1'b1: Stall Response Received Interrupt
 */
#define USB_H_STALL1    (BIT(3))
#define USB_H_STALL1_M  (USB_H_STALL1_V << USB_H_STALL1_S)
#define USB_H_STALL1_V  0x00000001
#define USB_H_STALL1_S  3
/** USB_H_NACK1 : R/W1C; bitpos: [4]; default: 0;
 *  1'b0: No NAK Response Received Interrupt
 *  1'b1: NAK Response Received Interrupt
 */
#define USB_H_NACK1    (BIT(4))
#define USB_H_NACK1_M  (USB_H_NACK1_V << USB_H_NACK1_S)
#define USB_H_NACK1_V  0x00000001
#define USB_H_NACK1_S  4
/** USB_H_ACK1 : R/W1C; bitpos: [5]; default: 0;
 *  1'b0: No ACK Response Received or Transmitted Interrupt
 *  1'b1: ACK Response Received or Transmitted Interrup
 */
#define USB_H_ACK1    (BIT(5))
#define USB_H_ACK1_M  (USB_H_ACK1_V << USB_H_ACK1_S)
#define USB_H_ACK1_V  0x00000001
#define USB_H_ACK1_S  5
/** USB_H_NYET1 : R/W1C; bitpos: [6]; default: 0;
 *  1'b0: No NYET Response Received Interrupt
 *  1'b1: NYET Response Received Interrupt
 */
#define USB_H_NYET1    (BIT(6))
#define USB_H_NYET1_M  (USB_H_NYET1_V << USB_H_NYET1_S)
#define USB_H_NYET1_V  0x00000001
#define USB_H_NYET1_S  6
/** USB_H_XACTERR1 : R/W1C; bitpos: [7]; default: 0;
 *  Indicates one of the following errors occurred on the USB:
 *  CRC check failure
 *  Timeout
 *  Bit stuff error
 *  False EOP
 */
#define USB_H_XACTERR1    (BIT(7))
#define USB_H_XACTERR1_M  (USB_H_XACTERR1_V << USB_H_XACTERR1_S)
#define USB_H_XACTERR1_V  0x00000001
#define USB_H_XACTERR1_S  7
/** USB_H_BBLERR1 : R/W1C; bitpos: [8]; default: 0;
 *  1'b0: No Babble Error
 *  1'b1: Babble Error
 */
#define USB_H_BBLERR1    (BIT(8))
#define USB_H_BBLERR1_M  (USB_H_BBLERR1_V << USB_H_BBLERR1_S)
#define USB_H_BBLERR1_V  0x00000001
#define USB_H_BBLERR1_S  8
/** USB_H_FRMOVRUN1 : R/W1C; bitpos: [9]; default: 0;
 *  1'b0: No Frame Overrun
 *  1'b1: Frame Overrun
 */
#define USB_H_FRMOVRUN1    (BIT(9))
#define USB_H_FRMOVRUN1_M  (USB_H_FRMOVRUN1_V << USB_H_FRMOVRUN1_S)
#define USB_H_FRMOVRUN1_V  0x00000001
#define USB_H_FRMOVRUN1_S  9
/** USB_H_DATATGLERR1 : R/W1C; bitpos: [10]; default: 0;
 *  1'b0: No Data Toggle Error
 *  1'b1: Data Toggle Error
 */
#define USB_H_DATATGLERR1    (BIT(10))
#define USB_H_DATATGLERR1_M  (USB_H_DATATGLERR1_V << USB_H_DATATGLERR1_S)
#define USB_H_DATATGLERR1_V  0x00000001
#define USB_H_DATATGLERR1_S  10
/** USB_H_BNAINTR1 : R/W1C; bitpos: [11]; default: 0;
 *  1'b0: No BNA Interrupt
 *  1'b1: BNA Interrupt
 */
#define USB_H_BNAINTR1    (BIT(11))
#define USB_H_BNAINTR1_M  (USB_H_BNAINTR1_V << USB_H_BNAINTR1_S)
#define USB_H_BNAINTR1_V  0x00000001
#define USB_H_BNAINTR1_S  11
/** USB_H_XCS_XACT_ERR1 : R/W1C; bitpos: [12]; default: 0;
 *  1'b0: No Excessive Transaction Error
 *  1'b1: Excessive Transaction Error
 */
#define USB_H_XCS_XACT_ERR1    (BIT(12))
#define USB_H_XCS_XACT_ERR1_M  (USB_H_XCS_XACT_ERR1_V << USB_H_XCS_XACT_ERR1_S)
#define USB_H_XCS_XACT_ERR1_V  0x00000001
#define USB_H_XCS_XACT_ERR1_S  12
/** USB_H_DESC_LST_ROLLINTR1 : R/W1C; bitpos: [13]; default: 0;
 *  1'b0: No Descriptor rollover interrupt
 *  1'b1: Descriptor rollover interrupt
 */
#define USB_H_DESC_LST_ROLLINTR1    (BIT(13))
#define USB_H_DESC_LST_ROLLINTR1_M  (USB_H_DESC_LST_ROLLINTR1_V << USB_H_DESC_LST_ROLLINTR1_S)
#define USB_H_DESC_LST_ROLLINTR1_V  0x00000001
#define USB_H_DESC_LST_ROLLINTR1_S  13


/** USB_HCINTMSK1_REG register
 *  Host Channel 1 Interrupt Mask Register
 */
#define USB_HCINTMSK1_REG (SOC_DPORT_USB_BASE + 0x52c)
/** USB_H_XFERCOMPLMSK1 : R/W; bitpos: [0]; default: 0;
 *  1'b0: Transfer Completed Mask
 *  1'b1: No Transfer Completed Mask
 */
#define USB_H_XFERCOMPLMSK1    (BIT(0))
#define USB_H_XFERCOMPLMSK1_M  (USB_H_XFERCOMPLMSK1_V << USB_H_XFERCOMPLMSK1_S)
#define USB_H_XFERCOMPLMSK1_V  0x00000001
#define USB_H_XFERCOMPLMSK1_S  0
/** USB_H_CHHLTDMSK1 : R/W; bitpos: [1]; default: 0;
 *  1'b0: Channel Halted Mask
 *  1'b1: No Channel Halted Mask
 */
#define USB_H_CHHLTDMSK1    (BIT(1))
#define USB_H_CHHLTDMSK1_M  (USB_H_CHHLTDMSK1_V << USB_H_CHHLTDMSK1_S)
#define USB_H_CHHLTDMSK1_V  0x00000001
#define USB_H_CHHLTDMSK1_S  1
/** USB_H_AHBERRMSK1 : R/W; bitpos: [2]; default: 0;
 *  1'b0: AHB Error Mask
 *  1'b1: No AHB Error Mask
 */
#define USB_H_AHBERRMSK1    (BIT(2))
#define USB_H_AHBERRMSK1_M  (USB_H_AHBERRMSK1_V << USB_H_AHBERRMSK1_S)
#define USB_H_AHBERRMSK1_V  0x00000001
#define USB_H_AHBERRMSK1_S  2
/** USB_H_STALLMSK1 : R/W; bitpos: [3]; default: 0;
 *  1'b0: Mask STALL Response Received Interrupt
 *  1'b1: No STALL Response Received Interrupt Mask
 */
#define USB_H_STALLMSK1    (BIT(3))
#define USB_H_STALLMSK1_M  (USB_H_STALLMSK1_V << USB_H_STALLMSK1_S)
#define USB_H_STALLMSK1_V  0x00000001
#define USB_H_STALLMSK1_S  3
/** USB_H_NAKMSK1 : R/W; bitpos: [4]; default: 0;
 *  1'b0: Mask NAK Response Received Interrupt
 *  1'b1: No NAK Response Received Interrupt Mask
 */
#define USB_H_NAKMSK1    (BIT(4))
#define USB_H_NAKMSK1_M  (USB_H_NAKMSK1_V << USB_H_NAKMSK1_S)
#define USB_H_NAKMSK1_V  0x00000001
#define USB_H_NAKMSK1_S  4
/** USB_H_ACKMSK1 : R/W; bitpos: [5]; default: 0;
 *  1'b0: Mask ACK Response Received/Transmitted Interrupt
 *  1'b1: No ACK Response Received/Transmitted Interrupt Mask
 */
#define USB_H_ACKMSK1    (BIT(5))
#define USB_H_ACKMSK1_M  (USB_H_ACKMSK1_V << USB_H_ACKMSK1_S)
#define USB_H_ACKMSK1_V  0x00000001
#define USB_H_ACKMSK1_S  5
/** USB_H_NYETMSK1 : R/W; bitpos: [6]; default: 0;
 *  1'b0: Mask NYET Response Received Interrupt
 *  1'b1: No NYET Response Received Interrupt Mask
 */
#define USB_H_NYETMSK1    (BIT(6))
#define USB_H_NYETMSK1_M  (USB_H_NYETMSK1_V << USB_H_NYETMSK1_S)
#define USB_H_NYETMSK1_V  0x00000001
#define USB_H_NYETMSK1_S  6
/** USB_H_XACTERRMSK1 : R/W; bitpos: [7]; default: 0;
 *  1'b0: Mask Transaction Error
 *  1'b1: No Transaction Error Mask
 */
#define USB_H_XACTERRMSK1    (BIT(7))
#define USB_H_XACTERRMSK1_M  (USB_H_XACTERRMSK1_V << USB_H_XACTERRMSK1_S)
#define USB_H_XACTERRMSK1_V  0x00000001
#define USB_H_XACTERRMSK1_S  7
/** USB_H_BBLERRMSK1 : R/W; bitpos: [8]; default: 0;
 *  Babble Error Mask
 *  1'b0: Mask Babble Error
 *  1'b1: No Babble Error Mask
 */
#define USB_H_BBLERRMSK1    (BIT(8))
#define USB_H_BBLERRMSK1_M  (USB_H_BBLERRMSK1_V << USB_H_BBLERRMSK1_S)
#define USB_H_BBLERRMSK1_V  0x00000001
#define USB_H_BBLERRMSK1_S  8
/** USB_H_FRMOVRUNMSK1 : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun Mask
 *  0x0 (MASK): Mask Overrun Mask
 *  0x1 (NOMASK): No Frame Overrun Mask
 */
#define USB_H_FRMOVRUNMSK1    (BIT(9))
#define USB_H_FRMOVRUNMSK1_M  (USB_H_FRMOVRUNMSK1_V << USB_H_FRMOVRUNMSK1_S)
#define USB_H_FRMOVRUNMSK1_V  0x00000001
#define USB_H_FRMOVRUNMSK1_S  9
/** USB_H_DATATGLERRMSK1 : R/W; bitpos: [10]; default: 0;
 *  Data Toggle Error Mask n scatter/gather DMA mode for host
 *  1'b0: Mask Data Toggle Error
 *  1'b1: No Data Toggle Error Mask
 */
#define USB_H_DATATGLERRMSK1    (BIT(10))
#define USB_H_DATATGLERRMSK1_M  (USB_H_DATATGLERRMSK1_V << USB_H_DATATGLERRMSK1_S)
#define USB_H_DATATGLERRMSK1_V  0x00000001
#define USB_H_DATATGLERRMSK1_S  10
/** USB_H_BNAINTRMSK1 : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt mask register
 *  1'b0: BNA Interrupt Masked
 *  1'b1: BNA Interrupt not masked
 */
#define USB_H_BNAINTRMSK1    (BIT(11))
#define USB_H_BNAINTRMSK1_M  (USB_H_BNAINTRMSK1_V << USB_H_BNAINTRMSK1_S)
#define USB_H_BNAINTRMSK1_V  0x00000001
#define USB_H_BNAINTRMSK1_S  11
/** USB_H_DESC_LST_ROLLINTRMSK1 : R/W; bitpos: [13]; default: 0;
 *  Descriptor List rollover interrupt Mask
 *  1'b0: Descriptor Rollover Interrupt Mask
 *  1'b1: Descriptor Rollover Interrupt not masked
 */
#define USB_H_DESC_LST_ROLLINTRMSK1    (BIT(13))
#define USB_H_DESC_LST_ROLLINTRMSK1_M  (USB_H_DESC_LST_ROLLINTRMSK1_V << USB_H_DESC_LST_ROLLINTRMSK1_S)
#define USB_H_DESC_LST_ROLLINTRMSK1_V  0x00000001
#define USB_H_DESC_LST_ROLLINTRMSK1_S  13


/** USB_HCINT2_REG register
 *  Host Channel 2 Interrupt Register
 */
#define USB_HCINT2_REG (SOC_DPORT_USB_BASE + 0x548)
/** USB_H_XFERCOMPL2 : R/W1C; bitpos: [0]; default: 0;
 *  1'b0: Transfer in progress or No Active Transfer
 *  1'b1: Transfer completed normally without any errors
 */
#define USB_H_XFERCOMPL2    (BIT(0))
#define USB_H_XFERCOMPL2_M  (USB_H_XFERCOMPL2_V << USB_H_XFERCOMPL2_S)
#define USB_H_XFERCOMPL2_V  0x00000001
#define USB_H_XFERCOMPL2_S  0
/** USB_H_CHHLTD2 : R/W1C; bitpos: [1]; default: 0;
 *  1'b0: Channel not halted
 *  1'b1: Channel Halted
 */
#define USB_H_CHHLTD2    (BIT(1))
#define USB_H_CHHLTD2_M  (USB_H_CHHLTD2_V << USB_H_CHHLTD2_S)
#define USB_H_CHHLTD2_V  0x00000001
#define USB_H_CHHLTD2_S  1
/** USB_H_AHBERR2 : R/W1C; bitpos: [2]; default: 0;
 *  1'b0: No AHB error
 *  1'b1: AHB error during AHB read/write
 */
#define USB_H_AHBERR2    (BIT(2))
#define USB_H_AHBERR2_M  (USB_H_AHBERR2_V << USB_H_AHBERR2_S)
#define USB_H_AHBERR2_V  0x00000001
#define USB_H_AHBERR2_S  2
/** USB_H_STALL2 : R/W1C; bitpos: [3]; default: 0;
 *  1'b0: No Stall Response Received Interrupt
 *  1'b1: Stall Response Received Interrupt
 */
#define USB_H_STALL2    (BIT(3))
#define USB_H_STALL2_M  (USB_H_STALL2_V << USB_H_STALL2_S)
#define USB_H_STALL2_V  0x00000001
#define USB_H_STALL2_S  3
/** USB_H_NACK2 : R/W1C; bitpos: [4]; default: 0;
 *  1'b0: No NAK Response Received Interrupt
 *  1'b1: NAK Response Received Interrupt
 */
#define USB_H_NACK2    (BIT(4))
#define USB_H_NACK2_M  (USB_H_NACK2_V << USB_H_NACK2_S)
#define USB_H_NACK2_V  0x00000001
#define USB_H_NACK2_S  4
/** USB_H_ACK2 : R/W1C; bitpos: [5]; default: 0;
 *  1'b0: No ACK Response Received or Transmitted Interrupt
 *  1'b1: ACK Response Received or Transmitted Interrup
 */
#define USB_H_ACK2    (BIT(5))
#define USB_H_ACK2_M  (USB_H_ACK2_V << USB_H_ACK2_S)
#define USB_H_ACK2_V  0x00000001
#define USB_H_ACK2_S  5
/** USB_H_NYET2 : R/W1C; bitpos: [6]; default: 0;
 *  1'b0: No NYET Response Received Interrupt
 *  1'b1: NYET Response Received Interrupt
 */
#define USB_H_NYET2    (BIT(6))
#define USB_H_NYET2_M  (USB_H_NYET2_V << USB_H_NYET2_S)
#define USB_H_NYET2_V  0x00000001
#define USB_H_NYET2_S  6
/** USB_H_XACTERR2 : R/W1C; bitpos: [7]; default: 0;
 *  Indicates one of the following errors occurred on the USB:
 *  CRC check failure
 *  Timeout
 *  Bit stuff error
 *  False EOP
 */
#define USB_H_XACTERR2    (BIT(7))
#define USB_H_XACTERR2_M  (USB_H_XACTERR2_V << USB_H_XACTERR2_S)
#define USB_H_XACTERR2_V  0x00000001
#define USB_H_XACTERR2_S  7
/** USB_H_BBLERR2 : R/W1C; bitpos: [8]; default: 0;
 *  1'b0: No Babble Error
 *  1'b1: Babble Error
 */
#define USB_H_BBLERR2    (BIT(8))
#define USB_H_BBLERR2_M  (USB_H_BBLERR2_V << USB_H_BBLERR2_S)
#define USB_H_BBLERR2_V  0x00000001
#define USB_H_BBLERR2_S  8
/** USB_H_FRMOVRUN2 : R/W1C; bitpos: [9]; default: 0;
 *  1'b0: No Frame Overrun
 *  1'b1: Frame Overrun
 */
#define USB_H_FRMOVRUN2    (BIT(9))
#define USB_H_FRMOVRUN2_M  (USB_H_FRMOVRUN2_V << USB_H_FRMOVRUN2_S)
#define USB_H_FRMOVRUN2_V  0x00000001
#define USB_H_FRMOVRUN2_S  9
/** USB_H_DATATGLERR2 : R/W1C; bitpos: [10]; default: 0;
 *  1'b0: No Data Toggle Error
 *  1'b1: Data Toggle Error
 */
#define USB_H_DATATGLERR2    (BIT(10))
#define USB_H_DATATGLERR2_M  (USB_H_DATATGLERR2_V << USB_H_DATATGLERR2_S)
#define USB_H_DATATGLERR2_V  0x00000001
#define USB_H_DATATGLERR2_S  10
/** USB_H_BNAINTR2 : R/W1C; bitpos: [11]; default: 0;
 *  1'b0: No BNA Interrupt
 *  1'b1: BNA Interrupt
 */
#define USB_H_BNAINTR2    (BIT(11))
#define USB_H_BNAINTR2_M  (USB_H_BNAINTR2_V << USB_H_BNAINTR2_S)
#define USB_H_BNAINTR2_V  0x00000001
#define USB_H_BNAINTR2_S  11
/** USB_H_XCS_XACT_ERR2 : R/W1C; bitpos: [12]; default: 0;
 *  1'b0: No Excessive Transaction Error
 *  1'b1: Excessive Transaction Error
 */
#define USB_H_XCS_XACT_ERR2    (BIT(12))
#define USB_H_XCS_XACT_ERR2_M  (USB_H_XCS_XACT_ERR2_V << USB_H_XCS_XACT_ERR2_S)
#define USB_H_XCS_XACT_ERR2_V  0x00000001
#define USB_H_XCS_XACT_ERR2_S  12
/** USB_H_DESC_LST_ROLLINTR2 : R/W1C; bitpos: [13]; default: 0;
 *  1'b0: No Descriptor rollover interrupt
 *  1'b1: Descriptor rollover interrupt
 */
#define USB_H_DESC_LST_ROLLINTR2    (BIT(13))
#define USB_H_DESC_LST_ROLLINTR2_M  (USB_H_DESC_LST_ROLLINTR2_V << USB_H_DESC_LST_ROLLINTR2_S)
#define USB_H_DESC_LST_ROLLINTR2_V  0x00000001
#define USB_H_DESC_LST_ROLLINTR2_S  13


/** USB_HCINTMSK2_REG register
 *  Host Channel 2 Interrupt Mask Register
 */
#define USB_HCINTMSK2_REG (SOC_DPORT_USB_BASE + 0x54c)
/** USB_H_XFERCOMPLMSK2 : R/W; bitpos: [0]; default: 0;
 *  1'b0: Transfer Completed Mask
 *  1'b1: No Transfer Completed Mask
 */
#define USB_H_XFERCOMPLMSK2    (BIT(0))
#define USB_H_XFERCOMPLMSK2_M  (USB_H_XFERCOMPLMSK2_V << USB_H_XFERCOMPLMSK2_S)
#define USB_H_XFERCOMPLMSK2_V  0x00000001
#define USB_H_XFERCOMPLMSK2_S  0
/** USB_H_CHHLTDMSK2 : R/W; bitpos: [1]; default: 0;
 *  1'b0: Channel Halted Mask
 *  1'b1: No Channel Halted Mask
 */
#define USB_H_CHHLTDMSK2    (BIT(1))
#define USB_H_CHHLTDMSK2_M  (USB_H_CHHLTDMSK2_V << USB_H_CHHLTDMSK2_S)
#define USB_H_CHHLTDMSK2_V  0x00000001
#define USB_H_CHHLTDMSK2_S  1
/** USB_H_AHBERRMSK2 : R/W; bitpos: [2]; default: 0;
 *  1'b0: AHB Error Mask
 *  1'b1: No AHB Error Mask
 */
#define USB_H_AHBERRMSK2    (BIT(2))
#define USB_H_AHBERRMSK2_M  (USB_H_AHBERRMSK2_V << USB_H_AHBERRMSK2_S)
#define USB_H_AHBERRMSK2_V  0x00000001
#define USB_H_AHBERRMSK2_S  2
/** USB_H_STALLMSK2 : R/W; bitpos: [3]; default: 0;
 *  1'b0: Mask STALL Response Received Interrupt
 *  1'b1: No STALL Response Received Interrupt Mask
 */
#define USB_H_STALLMSK2    (BIT(3))
#define USB_H_STALLMSK2_M  (USB_H_STALLMSK2_V << USB_H_STALLMSK2_S)
#define USB_H_STALLMSK2_V  0x00000001
#define USB_H_STALLMSK2_S  3
/** USB_H_NAKMSK2 : R/W; bitpos: [4]; default: 0;
 *  1'b0: Mask NAK Response Received Interrupt
 *  1'b1: No NAK Response Received Interrupt Mask
 */
#define USB_H_NAKMSK2    (BIT(4))
#define USB_H_NAKMSK2_M  (USB_H_NAKMSK2_V << USB_H_NAKMSK2_S)
#define USB_H_NAKMSK2_V  0x00000001
#define USB_H_NAKMSK2_S  4
/** USB_H_ACKMSK2 : R/W; bitpos: [5]; default: 0;
 *  1'b0: Mask ACK Response Received/Transmitted Interrupt
 *  1'b1: No ACK Response Received/Transmitted Interrupt Mask
 */
#define USB_H_ACKMSK2    (BIT(5))
#define USB_H_ACKMSK2_M  (USB_H_ACKMSK2_V << USB_H_ACKMSK2_S)
#define USB_H_ACKMSK2_V  0x00000001
#define USB_H_ACKMSK2_S  5
/** USB_H_NYETMSK2 : R/W; bitpos: [6]; default: 0;
 *  1'b0: Mask NYET Response Received Interrupt
 *  1'b1: No NYET Response Received Interrupt Mask
 */
#define USB_H_NYETMSK2    (BIT(6))
#define USB_H_NYETMSK2_M  (USB_H_NYETMSK2_V << USB_H_NYETMSK2_S)
#define USB_H_NYETMSK2_V  0x00000001
#define USB_H_NYETMSK2_S  6
/** USB_H_XACTERRMSK2 : R/W; bitpos: [7]; default: 0;
 *  1'b0: Mask Transaction Error
 *  1'b1: No Transaction Error Mask
 */
#define USB_H_XACTERRMSK2    (BIT(7))
#define USB_H_XACTERRMSK2_M  (USB_H_XACTERRMSK2_V << USB_H_XACTERRMSK2_S)
#define USB_H_XACTERRMSK2_V  0x00000001
#define USB_H_XACTERRMSK2_S  7
/** USB_H_BBLERRMSK2 : R/W; bitpos: [8]; default: 0;
 *  Babble Error Mask
 *  1'b0: Mask Babble Error
 *  1'b1: No Babble Error Mask
 */
#define USB_H_BBLERRMSK2    (BIT(8))
#define USB_H_BBLERRMSK2_M  (USB_H_BBLERRMSK2_V << USB_H_BBLERRMSK2_S)
#define USB_H_BBLERRMSK2_V  0x00000001
#define USB_H_BBLERRMSK2_S  8
/** USB_H_FRMOVRUNMSK2 : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun Mask
 *  0x0 (MASK): Mask Overrun Mask
 *  0x1 (NOMASK): No Frame Overrun Mask
 */
#define USB_H_FRMOVRUNMSK2    (BIT(9))
#define USB_H_FRMOVRUNMSK2_M  (USB_H_FRMOVRUNMSK2_V << USB_H_FRMOVRUNMSK2_S)
#define USB_H_FRMOVRUNMSK2_V  0x00000001
#define USB_H_FRMOVRUNMSK2_S  9
/** USB_H_DATATGLERRMSK2 : R/W; bitpos: [10]; default: 0;
 *  Data Toggle Error Mask n scatter/gather DMA mode for host
 *  1'b0: Mask Data Toggle Error
 *  1'b1: No Data Toggle Error Mask
 */
#define USB_H_DATATGLERRMSK2    (BIT(10))
#define USB_H_DATATGLERRMSK2_M  (USB_H_DATATGLERRMSK2_V << USB_H_DATATGLERRMSK2_S)
#define USB_H_DATATGLERRMSK2_V  0x00000001
#define USB_H_DATATGLERRMSK2_S  10
/** USB_H_BNAINTRMSK2 : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt mask register
 *  1'b0: BNA Interrupt Masked
 *  1'b1: BNA Interrupt not masked
 */
#define USB_H_BNAINTRMSK2    (BIT(11))
#define USB_H_BNAINTRMSK2_M  (USB_H_BNAINTRMSK2_V << USB_H_BNAINTRMSK2_S)
#define USB_H_BNAINTRMSK2_V  0x00000001
#define USB_H_BNAINTRMSK2_S  11
/** USB_H_DESC_LST_ROLLINTRMSK2 : R/W; bitpos: [13]; default: 0;
 *  Descriptor List rollover interrupt Mask
 *  1'b0: Descriptor Rollover Interrupt Mask
 *  1'b1: Descriptor Rollover Interrupt not masked
 */
#define USB_H_DESC_LST_ROLLINTRMSK2    (BIT(13))
#define USB_H_DESC_LST_ROLLINTRMSK2_M  (USB_H_DESC_LST_ROLLINTRMSK2_V << USB_H_DESC_LST_ROLLINTRMSK2_S)
#define USB_H_DESC_LST_ROLLINTRMSK2_V  0x00000001
#define USB_H_DESC_LST_ROLLINTRMSK2_S  13


/** USB_HCINT3_REG register
 *  Host Channel 3 Interrupt Register
 */
#define USB_HCINT3_REG (SOC_DPORT_USB_BASE + 0x568)
/** USB_H_XFERCOMPL3 : R/W1C; bitpos: [0]; default: 0;
 *  1'b0: Transfer in progress or No Active Transfer
 *  1'b1: Transfer completed normally without any errors
 */
#define USB_H_XFERCOMPL3    (BIT(0))
#define USB_H_XFERCOMPL3_M  (USB_H_XFERCOMPL3_V << USB_H_XFERCOMPL3_S)
#define USB_H_XFERCOMPL3_V  0x00000001
#define USB_H_XFERCOMPL3_S  0
/** USB_H_CHHLTD3 : R/W1C; bitpos: [1]; default: 0;
 *  1'b0: Channel not halted
 *  1'b1: Channel Halted
 */
#define USB_H_CHHLTD3    (BIT(1))
#define USB_H_CHHLTD3_M  (USB_H_CHHLTD3_V << USB_H_CHHLTD3_S)
#define USB_H_CHHLTD3_V  0x00000001
#define USB_H_CHHLTD3_S  1
/** USB_H_AHBERR3 : R/W1C; bitpos: [2]; default: 0;
 *  1'b0: No AHB error
 *  1'b1: AHB error during AHB read/write
 */
#define USB_H_AHBERR3    (BIT(2))
#define USB_H_AHBERR3_M  (USB_H_AHBERR3_V << USB_H_AHBERR3_S)
#define USB_H_AHBERR3_V  0x00000001
#define USB_H_AHBERR3_S  2
/** USB_H_STALL3 : R/W1C; bitpos: [3]; default: 0;
 *  1'b0: No Stall Response Received Interrupt
 *  1'b1: Stall Response Received Interrupt
 */
#define USB_H_STALL3    (BIT(3))
#define USB_H_STALL3_M  (USB_H_STALL3_V << USB_H_STALL3_S)
#define USB_H_STALL3_V  0x00000001
#define USB_H_STALL3_S  3
/** USB_H_NACK3 : R/W1C; bitpos: [4]; default: 0;
 *  1'b0: No NAK Response Received Interrupt
 *  1'b1: NAK Response Received Interrupt
 */
#define USB_H_NACK3    (BIT(4))
#define USB_H_NACK3_M  (USB_H_NACK3_V << USB_H_NACK3_S)
#define USB_H_NACK3_V  0x00000001
#define USB_H_NACK3_S  4
/** USB_H_ACK3 : R/W1C; bitpos: [5]; default: 0;
 *  1'b0: No ACK Response Received or Transmitted Interrupt
 *  1'b1: ACK Response Received or Transmitted Interrup
 */
#define USB_H_ACK3    (BIT(5))
#define USB_H_ACK3_M  (USB_H_ACK3_V << USB_H_ACK3_S)
#define USB_H_ACK3_V  0x00000001
#define USB_H_ACK3_S  5
/** USB_H_NYET3 : R/W1C; bitpos: [6]; default: 0;
 *  1'b0: No NYET Response Received Interrupt
 *  1'b1: NYET Response Received Interrupt
 */
#define USB_H_NYET3    (BIT(6))
#define USB_H_NYET3_M  (USB_H_NYET3_V << USB_H_NYET3_S)
#define USB_H_NYET3_V  0x00000001
#define USB_H_NYET3_S  6
/** USB_H_XACTERR3 : R/W1C; bitpos: [7]; default: 0;
 *  Indicates one of the following errors occurred on the USB:
 *  CRC check failure
 *  Timeout
 *  Bit stuff error
 *  False EOP
 */
#define USB_H_XACTERR3    (BIT(7))
#define USB_H_XACTERR3_M  (USB_H_XACTERR3_V << USB_H_XACTERR3_S)
#define USB_H_XACTERR3_V  0x00000001
#define USB_H_XACTERR3_S  7
/** USB_H_BBLERR3 : R/W1C; bitpos: [8]; default: 0;
 *  1'b0: No Babble Error
 *  1'b1: Babble Error
 */
#define USB_H_BBLERR3    (BIT(8))
#define USB_H_BBLERR3_M  (USB_H_BBLERR3_V << USB_H_BBLERR3_S)
#define USB_H_BBLERR3_V  0x00000001
#define USB_H_BBLERR3_S  8
/** USB_H_FRMOVRUN3 : R/W1C; bitpos: [9]; default: 0;
 *  1'b0: No Frame Overrun
 *  1'b1: Frame Overrun
 */
#define USB_H_FRMOVRUN3    (BIT(9))
#define USB_H_FRMOVRUN3_M  (USB_H_FRMOVRUN3_V << USB_H_FRMOVRUN3_S)
#define USB_H_FRMOVRUN3_V  0x00000001
#define USB_H_FRMOVRUN3_S  9
/** USB_H_DATATGLERR3 : R/W1C; bitpos: [10]; default: 0;
 *  1'b0: No Data Toggle Error
 *  1'b1: Data Toggle Error
 */
#define USB_H_DATATGLERR3    (BIT(10))
#define USB_H_DATATGLERR3_M  (USB_H_DATATGLERR3_V << USB_H_DATATGLERR3_S)
#define USB_H_DATATGLERR3_V  0x00000001
#define USB_H_DATATGLERR3_S  10
/** USB_H_BNAINTR3 : R/W1C; bitpos: [11]; default: 0;
 *  1'b0: No BNA Interrupt
 *  1'b1: BNA Interrupt
 */
#define USB_H_BNAINTR3    (BIT(11))
#define USB_H_BNAINTR3_M  (USB_H_BNAINTR3_V << USB_H_BNAINTR3_S)
#define USB_H_BNAINTR3_V  0x00000001
#define USB_H_BNAINTR3_S  11
/** USB_H_XCS_XACT_ERR3 : R/W1C; bitpos: [12]; default: 0;
 *  1'b0: No Excessive Transaction Error
 *  1'b1: Excessive Transaction Error
 */
#define USB_H_XCS_XACT_ERR3    (BIT(12))
#define USB_H_XCS_XACT_ERR3_M  (USB_H_XCS_XACT_ERR3_V << USB_H_XCS_XACT_ERR3_S)
#define USB_H_XCS_XACT_ERR3_V  0x00000001
#define USB_H_XCS_XACT_ERR3_S  12
/** USB_H_DESC_LST_ROLLINTR3 : R/W1C; bitpos: [13]; default: 0;
 *  1'b0: No Descriptor rollover interrupt
 *  1'b1: Descriptor rollover interrupt
 */
#define USB_H_DESC_LST_ROLLINTR3    (BIT(13))
#define USB_H_DESC_LST_ROLLINTR3_M  (USB_H_DESC_LST_ROLLINTR3_V << USB_H_DESC_LST_ROLLINTR3_S)
#define USB_H_DESC_LST_ROLLINTR3_V  0x00000001
#define USB_H_DESC_LST_ROLLINTR3_S  13


/** USB_HCINTMSK3_REG register
 *  Host Channel 3 Interrupt Mask Register
 */
#define USB_HCINTMSK3_REG (SOC_DPORT_USB_BASE + 0x56c)
/** USB_H_XFERCOMPLMSK3 : R/W; bitpos: [0]; default: 0;
 *  1'b0: Transfer Completed Mask
 *  1'b1: No Transfer Completed Mask
 */
#define USB_H_XFERCOMPLMSK3    (BIT(0))
#define USB_H_XFERCOMPLMSK3_M  (USB_H_XFERCOMPLMSK3_V << USB_H_XFERCOMPLMSK3_S)
#define USB_H_XFERCOMPLMSK3_V  0x00000001
#define USB_H_XFERCOMPLMSK3_S  0
/** USB_H_CHHLTDMSK3 : R/W; bitpos: [1]; default: 0;
 *  1'b0: Channel Halted Mask
 *  1'b1: No Channel Halted Mask
 */
#define USB_H_CHHLTDMSK3    (BIT(1))
#define USB_H_CHHLTDMSK3_M  (USB_H_CHHLTDMSK3_V << USB_H_CHHLTDMSK3_S)
#define USB_H_CHHLTDMSK3_V  0x00000001
#define USB_H_CHHLTDMSK3_S  1
/** USB_H_AHBERRMSK3 : R/W; bitpos: [2]; default: 0;
 *  1'b0: AHB Error Mask
 *  1'b1: No AHB Error Mask
 */
#define USB_H_AHBERRMSK3    (BIT(2))
#define USB_H_AHBERRMSK3_M  (USB_H_AHBERRMSK3_V << USB_H_AHBERRMSK3_S)
#define USB_H_AHBERRMSK3_V  0x00000001
#define USB_H_AHBERRMSK3_S  2
/** USB_H_STALLMSK3 : R/W; bitpos: [3]; default: 0;
 *  1'b0: Mask STALL Response Received Interrupt
 *  1'b1: No STALL Response Received Interrupt Mask
 */
#define USB_H_STALLMSK3    (BIT(3))
#define USB_H_STALLMSK3_M  (USB_H_STALLMSK3_V << USB_H_STALLMSK3_S)
#define USB_H_STALLMSK3_V  0x00000001
#define USB_H_STALLMSK3_S  3
/** USB_H_NAKMSK3 : R/W; bitpos: [4]; default: 0;
 *  1'b0: Mask NAK Response Received Interrupt
 *  1'b1: No NAK Response Received Interrupt Mask
 */
#define USB_H_NAKMSK3    (BIT(4))
#define USB_H_NAKMSK3_M  (USB_H_NAKMSK3_V << USB_H_NAKMSK3_S)
#define USB_H_NAKMSK3_V  0x00000001
#define USB_H_NAKMSK3_S  4
/** USB_H_ACKMSK3 : R/W; bitpos: [5]; default: 0;
 *  1'b0: Mask ACK Response Received/Transmitted Interrupt
 *  1'b1: No ACK Response Received/Transmitted Interrupt Mask
 */
#define USB_H_ACKMSK3    (BIT(5))
#define USB_H_ACKMSK3_M  (USB_H_ACKMSK3_V << USB_H_ACKMSK3_S)
#define USB_H_ACKMSK3_V  0x00000001
#define USB_H_ACKMSK3_S  5
/** USB_H_NYETMSK3 : R/W; bitpos: [6]; default: 0;
 *  1'b0: Mask NYET Response Received Interrupt
 *  1'b1: No NYET Response Received Interrupt Mask
 */
#define USB_H_NYETMSK3    (BIT(6))
#define USB_H_NYETMSK3_M  (USB_H_NYETMSK3_V << USB_H_NYETMSK3_S)
#define USB_H_NYETMSK3_V  0x00000001
#define USB_H_NYETMSK3_S  6
/** USB_H_XACTERRMSK3 : R/W; bitpos: [7]; default: 0;
 *  1'b0: Mask Transaction Error
 *  1'b1: No Transaction Error Mask
 */
#define USB_H_XACTERRMSK3    (BIT(7))
#define USB_H_XACTERRMSK3_M  (USB_H_XACTERRMSK3_V << USB_H_XACTERRMSK3_S)
#define USB_H_XACTERRMSK3_V  0x00000001
#define USB_H_XACTERRMSK3_S  7
/** USB_H_BBLERRMSK3 : R/W; bitpos: [8]; default: 0;
 *  Babble Error Mask
 *  1'b0: Mask Babble Error
 *  1'b1: No Babble Error Mask
 */
#define USB_H_BBLERRMSK3    (BIT(8))
#define USB_H_BBLERRMSK3_M  (USB_H_BBLERRMSK3_V << USB_H_BBLERRMSK3_S)
#define USB_H_BBLERRMSK3_V  0x00000001
#define USB_H_BBLERRMSK3_S  8
/** USB_H_FRMOVRUNMSK3 : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun Mask
 *  0x0 (MASK): Mask Overrun Mask
 *  0x1 (NOMASK): No Frame Overrun Mask
 */
#define USB_H_FRMOVRUNMSK3    (BIT(9))
#define USB_H_FRMOVRUNMSK3_M  (USB_H_FRMOVRUNMSK3_V << USB_H_FRMOVRUNMSK3_S)
#define USB_H_FRMOVRUNMSK3_V  0x00000001
#define USB_H_FRMOVRUNMSK3_S  9
/** USB_H_DATATGLERRMSK3 : R/W; bitpos: [10]; default: 0;
 *  Data Toggle Error Mask n scatter/gather DMA mode for host
 *  1'b0: Mask Data Toggle Error
 *  1'b1: No Data Toggle Error Mask
 */
#define USB_H_DATATGLERRMSK3    (BIT(10))
#define USB_H_DATATGLERRMSK3_M  (USB_H_DATATGLERRMSK3_V << USB_H_DATATGLERRMSK3_S)
#define USB_H_DATATGLERRMSK3_V  0x00000001
#define USB_H_DATATGLERRMSK3_S  10
/** USB_H_BNAINTRMSK3 : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt mask register
 *  1'b0: BNA Interrupt Masked
 *  1'b1: BNA Interrupt not masked
 */
#define USB_H_BNAINTRMSK3    (BIT(11))
#define USB_H_BNAINTRMSK3_M  (USB_H_BNAINTRMSK3_V << USB_H_BNAINTRMSK3_S)
#define USB_H_BNAINTRMSK3_V  0x00000001
#define USB_H_BNAINTRMSK3_S  11
/** USB_H_DESC_LST_ROLLINTRMSK3 : R/W; bitpos: [13]; default: 0;
 *  Descriptor List rollover interrupt Mask
 *  1'b0: Descriptor Rollover Interrupt Mask
 *  1'b1: Descriptor Rollover Interrupt not masked
 */
#define USB_H_DESC_LST_ROLLINTRMSK3    (BIT(13))
#define USB_H_DESC_LST_ROLLINTRMSK3_M  (USB_H_DESC_LST_ROLLINTRMSK3_V << USB_H_DESC_LST_ROLLINTRMSK3_S)
#define USB_H_DESC_LST_ROLLINTRMSK3_V  0x00000001
#define USB_H_DESC_LST_ROLLINTRMSK3_S  13


/** USB_HCINT4_REG register
 *  Host Channel 4 Interrupt Register
 */
#define USB_HCINT4_REG (SOC_DPORT_USB_BASE + 0x588)
/** USB_H_XFERCOMPL4 : R/W1C; bitpos: [0]; default: 0;
 *  1'b0: Transfer in progress or No Active Transfer
 *  1'b1: Transfer completed normally without any errors
 */
#define USB_H_XFERCOMPL4    (BIT(0))
#define USB_H_XFERCOMPL4_M  (USB_H_XFERCOMPL4_V << USB_H_XFERCOMPL4_S)
#define USB_H_XFERCOMPL4_V  0x00000001
#define USB_H_XFERCOMPL4_S  0
/** USB_H_CHHLTD4 : R/W1C; bitpos: [1]; default: 0;
 *  1'b0: Channel not halted
 *  1'b1: Channel Halted
 */
#define USB_H_CHHLTD4    (BIT(1))
#define USB_H_CHHLTD4_M  (USB_H_CHHLTD4_V << USB_H_CHHLTD4_S)
#define USB_H_CHHLTD4_V  0x00000001
#define USB_H_CHHLTD4_S  1
/** USB_H_AHBERR4 : R/W1C; bitpos: [2]; default: 0;
 *  1'b0: No AHB error
 *  1'b1: AHB error during AHB read/write
 */
#define USB_H_AHBERR4    (BIT(2))
#define USB_H_AHBERR4_M  (USB_H_AHBERR4_V << USB_H_AHBERR4_S)
#define USB_H_AHBERR4_V  0x00000001
#define USB_H_AHBERR4_S  2
/** USB_H_STALL4 : R/W1C; bitpos: [3]; default: 0;
 *  1'b0: No Stall Response Received Interrupt
 *  1'b1: Stall Response Received Interrupt
 */
#define USB_H_STALL4    (BIT(3))
#define USB_H_STALL4_M  (USB_H_STALL4_V << USB_H_STALL4_S)
#define USB_H_STALL4_V  0x00000001
#define USB_H_STALL4_S  3
/** USB_H_NACK4 : R/W1C; bitpos: [4]; default: 0;
 *  1'b0: No NAK Response Received Interrupt
 *  1'b1: NAK Response Received Interrupt
 */
#define USB_H_NACK4    (BIT(4))
#define USB_H_NACK4_M  (USB_H_NACK4_V << USB_H_NACK4_S)
#define USB_H_NACK4_V  0x00000001
#define USB_H_NACK4_S  4
/** USB_H_ACK4 : R/W1C; bitpos: [5]; default: 0;
 *  1'b0: No ACK Response Received or Transmitted Interrupt
 *  1'b1: ACK Response Received or Transmitted Interrup
 */
#define USB_H_ACK4    (BIT(5))
#define USB_H_ACK4_M  (USB_H_ACK4_V << USB_H_ACK4_S)
#define USB_H_ACK4_V  0x00000001
#define USB_H_ACK4_S  5
/** USB_H_NYET4 : R/W1C; bitpos: [6]; default: 0;
 *  1'b0: No NYET Response Received Interrupt
 *  1'b1: NYET Response Received Interrupt
 */
#define USB_H_NYET4    (BIT(6))
#define USB_H_NYET4_M  (USB_H_NYET4_V << USB_H_NYET4_S)
#define USB_H_NYET4_V  0x00000001
#define USB_H_NYET4_S  6
/** USB_H_XACTERR4 : R/W1C; bitpos: [7]; default: 0;
 *  Indicates one of the following errors occurred on the USB:
 *  CRC check failure
 *  Timeout
 *  Bit stuff error
 *  False EOP
 */
#define USB_H_XACTERR4    (BIT(7))
#define USB_H_XACTERR4_M  (USB_H_XACTERR4_V << USB_H_XACTERR4_S)
#define USB_H_XACTERR4_V  0x00000001
#define USB_H_XACTERR4_S  7
/** USB_H_BBLERR4 : R/W1C; bitpos: [8]; default: 0;
 *  1'b0: No Babble Error
 *  1'b1: Babble Error
 */
#define USB_H_BBLERR4    (BIT(8))
#define USB_H_BBLERR4_M  (USB_H_BBLERR4_V << USB_H_BBLERR4_S)
#define USB_H_BBLERR4_V  0x00000001
#define USB_H_BBLERR4_S  8
/** USB_H_FRMOVRUN4 : R/W1C; bitpos: [9]; default: 0;
 *  1'b0: No Frame Overrun
 *  1'b1: Frame Overrun
 */
#define USB_H_FRMOVRUN4    (BIT(9))
#define USB_H_FRMOVRUN4_M  (USB_H_FRMOVRUN4_V << USB_H_FRMOVRUN4_S)
#define USB_H_FRMOVRUN4_V  0x00000001
#define USB_H_FRMOVRUN4_S  9
/** USB_H_DATATGLERR4 : R/W1C; bitpos: [10]; default: 0;
 *  1'b0: No Data Toggle Error
 *  1'b1: Data Toggle Error
 */
#define USB_H_DATATGLERR4    (BIT(10))
#define USB_H_DATATGLERR4_M  (USB_H_DATATGLERR4_V << USB_H_DATATGLERR4_S)
#define USB_H_DATATGLERR4_V  0x00000001
#define USB_H_DATATGLERR4_S  10
/** USB_H_BNAINTR4 : R/W1C; bitpos: [11]; default: 0;
 *  1'b0: No BNA Interrupt
 *  1'b1: BNA Interrupt
 */
#define USB_H_BNAINTR4    (BIT(11))
#define USB_H_BNAINTR4_M  (USB_H_BNAINTR4_V << USB_H_BNAINTR4_S)
#define USB_H_BNAINTR4_V  0x00000001
#define USB_H_BNAINTR4_S  11
/** USB_H_XCS_XACT_ERR4 : R/W1C; bitpos: [12]; default: 0;
 *  1'b0: No Excessive Transaction Error
 *  1'b1: Excessive Transaction Error
 */
#define USB_H_XCS_XACT_ERR4    (BIT(12))
#define USB_H_XCS_XACT_ERR4_M  (USB_H_XCS_XACT_ERR4_V << USB_H_XCS_XACT_ERR4_S)
#define USB_H_XCS_XACT_ERR4_V  0x00000001
#define USB_H_XCS_XACT_ERR4_S  12
/** USB_H_DESC_LST_ROLLINTR4 : R/W1C; bitpos: [13]; default: 0;
 *  1'b0: No Descriptor rollover interrupt
 *  1'b1: Descriptor rollover interrupt
 */
#define USB_H_DESC_LST_ROLLINTR4    (BIT(13))
#define USB_H_DESC_LST_ROLLINTR4_M  (USB_H_DESC_LST_ROLLINTR4_V << USB_H_DESC_LST_ROLLINTR4_S)
#define USB_H_DESC_LST_ROLLINTR4_V  0x00000001
#define USB_H_DESC_LST_ROLLINTR4_S  13


/** USB_HCINTMSK4_REG register
 *  Host Channel 4 Interrupt Mask Register
 */
#define USB_HCINTMSK4_REG (SOC_DPORT_USB_BASE + 0x58c)
/** USB_H_XFERCOMPLMSK4 : R/W; bitpos: [0]; default: 0;
 *  1'b0: Transfer Completed Mask
 *  1'b1: No Transfer Completed Mask
 */
#define USB_H_XFERCOMPLMSK4    (BIT(0))
#define USB_H_XFERCOMPLMSK4_M  (USB_H_XFERCOMPLMSK4_V << USB_H_XFERCOMPLMSK4_S)
#define USB_H_XFERCOMPLMSK4_V  0x00000001
#define USB_H_XFERCOMPLMSK4_S  0
/** USB_H_CHHLTDMSK4 : R/W; bitpos: [1]; default: 0;
 *  1'b0: Channel Halted Mask
 *  1'b1: No Channel Halted Mask
 */
#define USB_H_CHHLTDMSK4    (BIT(1))
#define USB_H_CHHLTDMSK4_M  (USB_H_CHHLTDMSK4_V << USB_H_CHHLTDMSK4_S)
#define USB_H_CHHLTDMSK4_V  0x00000001
#define USB_H_CHHLTDMSK4_S  1
/** USB_H_AHBERRMSK4 : R/W; bitpos: [2]; default: 0;
 *  1'b0: AHB Error Mask
 *  1'b1: No AHB Error Mask
 */
#define USB_H_AHBERRMSK4    (BIT(2))
#define USB_H_AHBERRMSK4_M  (USB_H_AHBERRMSK4_V << USB_H_AHBERRMSK4_S)
#define USB_H_AHBERRMSK4_V  0x00000001
#define USB_H_AHBERRMSK4_S  2
/** USB_H_STALLMSK4 : R/W; bitpos: [3]; default: 0;
 *  1'b0: Mask STALL Response Received Interrupt
 *  1'b1: No STALL Response Received Interrupt Mask
 */
#define USB_H_STALLMSK4    (BIT(3))
#define USB_H_STALLMSK4_M  (USB_H_STALLMSK4_V << USB_H_STALLMSK4_S)
#define USB_H_STALLMSK4_V  0x00000001
#define USB_H_STALLMSK4_S  3
/** USB_H_NAKMSK4 : R/W; bitpos: [4]; default: 0;
 *  1'b0: Mask NAK Response Received Interrupt
 *  1'b1: No NAK Response Received Interrupt Mask
 */
#define USB_H_NAKMSK4    (BIT(4))
#define USB_H_NAKMSK4_M  (USB_H_NAKMSK4_V << USB_H_NAKMSK4_S)
#define USB_H_NAKMSK4_V  0x00000001
#define USB_H_NAKMSK4_S  4
/** USB_H_ACKMSK4 : R/W; bitpos: [5]; default: 0;
 *  1'b0: Mask ACK Response Received/Transmitted Interrupt
 *  1'b1: No ACK Response Received/Transmitted Interrupt Mask
 */
#define USB_H_ACKMSK4    (BIT(5))
#define USB_H_ACKMSK4_M  (USB_H_ACKMSK4_V << USB_H_ACKMSK4_S)
#define USB_H_ACKMSK4_V  0x00000001
#define USB_H_ACKMSK4_S  5
/** USB_H_NYETMSK4 : R/W; bitpos: [6]; default: 0;
 *  1'b0: Mask NYET Response Received Interrupt
 *  1'b1: No NYET Response Received Interrupt Mask
 */
#define USB_H_NYETMSK4    (BIT(6))
#define USB_H_NYETMSK4_M  (USB_H_NYETMSK4_V << USB_H_NYETMSK4_S)
#define USB_H_NYETMSK4_V  0x00000001
#define USB_H_NYETMSK4_S  6
/** USB_H_XACTERRMSK4 : R/W; bitpos: [7]; default: 0;
 *  1'b0: Mask Transaction Error
 *  1'b1: No Transaction Error Mask
 */
#define USB_H_XACTERRMSK4    (BIT(7))
#define USB_H_XACTERRMSK4_M  (USB_H_XACTERRMSK4_V << USB_H_XACTERRMSK4_S)
#define USB_H_XACTERRMSK4_V  0x00000001
#define USB_H_XACTERRMSK4_S  7
/** USB_H_BBLERRMSK4 : R/W; bitpos: [8]; default: 0;
 *  Babble Error Mask
 *  1'b0: Mask Babble Error
 *  1'b1: No Babble Error Mask
 */
#define USB_H_BBLERRMSK4    (BIT(8))
#define USB_H_BBLERRMSK4_M  (USB_H_BBLERRMSK4_V << USB_H_BBLERRMSK4_S)
#define USB_H_BBLERRMSK4_V  0x00000001
#define USB_H_BBLERRMSK4_S  8
/** USB_H_FRMOVRUNMSK4 : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun Mask
 *  0x0 (MASK): Mask Overrun Mask
 *  0x1 (NOMASK): No Frame Overrun Mask
 */
#define USB_H_FRMOVRUNMSK4    (BIT(9))
#define USB_H_FRMOVRUNMSK4_M  (USB_H_FRMOVRUNMSK4_V << USB_H_FRMOVRUNMSK4_S)
#define USB_H_FRMOVRUNMSK4_V  0x00000001
#define USB_H_FRMOVRUNMSK4_S  9
/** USB_H_DATATGLERRMSK4 : R/W; bitpos: [10]; default: 0;
 *  Data Toggle Error Mask n scatter/gather DMA mode for host
 *  1'b0: Mask Data Toggle Error
 *  1'b1: No Data Toggle Error Mask
 */
#define USB_H_DATATGLERRMSK4    (BIT(10))
#define USB_H_DATATGLERRMSK4_M  (USB_H_DATATGLERRMSK4_V << USB_H_DATATGLERRMSK4_S)
#define USB_H_DATATGLERRMSK4_V  0x00000001
#define USB_H_DATATGLERRMSK4_S  10
/** USB_H_BNAINTRMSK4 : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt mask register
 *  1'b0: BNA Interrupt Masked
 *  1'b1: BNA Interrupt not masked
 */
#define USB_H_BNAINTRMSK4    (BIT(11))
#define USB_H_BNAINTRMSK4_M  (USB_H_BNAINTRMSK4_V << USB_H_BNAINTRMSK4_S)
#define USB_H_BNAINTRMSK4_V  0x00000001
#define USB_H_BNAINTRMSK4_S  11
/** USB_H_DESC_LST_ROLLINTRMSK4 : R/W; bitpos: [13]; default: 0;
 *  Descriptor List rollover interrupt Mask
 *  1'b0: Descriptor Rollover Interrupt Mask
 *  1'b1: Descriptor Rollover Interrupt not masked
 */
#define USB_H_DESC_LST_ROLLINTRMSK4    (BIT(13))
#define USB_H_DESC_LST_ROLLINTRMSK4_M  (USB_H_DESC_LST_ROLLINTRMSK4_V << USB_H_DESC_LST_ROLLINTRMSK4_S)
#define USB_H_DESC_LST_ROLLINTRMSK4_V  0x00000001
#define USB_H_DESC_LST_ROLLINTRMSK4_S  13


/** USB_HCINT5_REG register
 *  Host Channel 5 Interrupt Register
 */
#define USB_HCINT5_REG (SOC_DPORT_USB_BASE + 0x5a8)
/** USB_H_XFERCOMPL5 : R/W1C; bitpos: [0]; default: 0;
 *  1'b0: Transfer in progress or No Active Transfer
 *  1'b1: Transfer completed normally without any errors
 */
#define USB_H_XFERCOMPL5    (BIT(0))
#define USB_H_XFERCOMPL5_M  (USB_H_XFERCOMPL5_V << USB_H_XFERCOMPL5_S)
#define USB_H_XFERCOMPL5_V  0x00000001
#define USB_H_XFERCOMPL5_S  0
/** USB_H_CHHLTD5 : R/W1C; bitpos: [1]; default: 0;
 *  1'b0: Channel not halted
 *  1'b1: Channel Halted
 */
#define USB_H_CHHLTD5    (BIT(1))
#define USB_H_CHHLTD5_M  (USB_H_CHHLTD5_V << USB_H_CHHLTD5_S)
#define USB_H_CHHLTD5_V  0x00000001
#define USB_H_CHHLTD5_S  1
/** USB_H_AHBERR5 : R/W1C; bitpos: [2]; default: 0;
 *  1'b0: No AHB error
 *  1'b1: AHB error during AHB read/write
 */
#define USB_H_AHBERR5    (BIT(2))
#define USB_H_AHBERR5_M  (USB_H_AHBERR5_V << USB_H_AHBERR5_S)
#define USB_H_AHBERR5_V  0x00000001
#define USB_H_AHBERR5_S  2
/** USB_H_STALL5 : R/W1C; bitpos: [3]; default: 0;
 *  1'b0: No Stall Response Received Interrupt
 *  1'b1: Stall Response Received Interrupt
 */
#define USB_H_STALL5    (BIT(3))
#define USB_H_STALL5_M  (USB_H_STALL5_V << USB_H_STALL5_S)
#define USB_H_STALL5_V  0x00000001
#define USB_H_STALL5_S  3
/** USB_H_NACK5 : R/W1C; bitpos: [4]; default: 0;
 *  1'b0: No NAK Response Received Interrupt
 *  1'b1: NAK Response Received Interrupt
 */
#define USB_H_NACK5    (BIT(4))
#define USB_H_NACK5_M  (USB_H_NACK5_V << USB_H_NACK5_S)
#define USB_H_NACK5_V  0x00000001
#define USB_H_NACK5_S  4
/** USB_H_ACK5 : R/W1C; bitpos: [5]; default: 0;
 *  1'b0: No ACK Response Received or Transmitted Interrupt
 *  1'b1: ACK Response Received or Transmitted Interrup
 */
#define USB_H_ACK5    (BIT(5))
#define USB_H_ACK5_M  (USB_H_ACK5_V << USB_H_ACK5_S)
#define USB_H_ACK5_V  0x00000001
#define USB_H_ACK5_S  5
/** USB_H_NYET5 : R/W1C; bitpos: [6]; default: 0;
 *  1'b0: No NYET Response Received Interrupt
 *  1'b1: NYET Response Received Interrupt
 */
#define USB_H_NYET5    (BIT(6))
#define USB_H_NYET5_M  (USB_H_NYET5_V << USB_H_NYET5_S)
#define USB_H_NYET5_V  0x00000001
#define USB_H_NYET5_S  6
/** USB_H_XACTERR5 : R/W1C; bitpos: [7]; default: 0;
 *  Indicates one of the following errors occurred on the USB:
 *  CRC check failure
 *  Timeout
 *  Bit stuff error
 *  False EOP
 */
#define USB_H_XACTERR5    (BIT(7))
#define USB_H_XACTERR5_M  (USB_H_XACTERR5_V << USB_H_XACTERR5_S)
#define USB_H_XACTERR5_V  0x00000001
#define USB_H_XACTERR5_S  7
/** USB_H_BBLERR5 : R/W1C; bitpos: [8]; default: 0;
 *  1'b0: No Babble Error
 *  1'b1: Babble Error
 */
#define USB_H_BBLERR5    (BIT(8))
#define USB_H_BBLERR5_M  (USB_H_BBLERR5_V << USB_H_BBLERR5_S)
#define USB_H_BBLERR5_V  0x00000001
#define USB_H_BBLERR5_S  8
/** USB_H_FRMOVRUN5 : R/W1C; bitpos: [9]; default: 0;
 *  1'b0: No Frame Overrun
 *  1'b1: Frame Overrun
 */
#define USB_H_FRMOVRUN5    (BIT(9))
#define USB_H_FRMOVRUN5_M  (USB_H_FRMOVRUN5_V << USB_H_FRMOVRUN5_S)
#define USB_H_FRMOVRUN5_V  0x00000001
#define USB_H_FRMOVRUN5_S  9
/** USB_H_DATATGLERR5 : R/W1C; bitpos: [10]; default: 0;
 *  1'b0: No Data Toggle Error
 *  1'b1: Data Toggle Error
 */
#define USB_H_DATATGLERR5    (BIT(10))
#define USB_H_DATATGLERR5_M  (USB_H_DATATGLERR5_V << USB_H_DATATGLERR5_S)
#define USB_H_DATATGLERR5_V  0x00000001
#define USB_H_DATATGLERR5_S  10
/** USB_H_BNAINTR5 : R/W1C; bitpos: [11]; default: 0;
 *  1'b0: No BNA Interrupt
 *  1'b1: BNA Interrupt
 */
#define USB_H_BNAINTR5    (BIT(11))
#define USB_H_BNAINTR5_M  (USB_H_BNAINTR5_V << USB_H_BNAINTR5_S)
#define USB_H_BNAINTR5_V  0x00000001
#define USB_H_BNAINTR5_S  11
/** USB_H_XCS_XACT_ERR5 : R/W1C; bitpos: [12]; default: 0;
 *  1'b0: No Excessive Transaction Error
 *  1'b1: Excessive Transaction Error
 */
#define USB_H_XCS_XACT_ERR5    (BIT(12))
#define USB_H_XCS_XACT_ERR5_M  (USB_H_XCS_XACT_ERR5_V << USB_H_XCS_XACT_ERR5_S)
#define USB_H_XCS_XACT_ERR5_V  0x00000001
#define USB_H_XCS_XACT_ERR5_S  12
/** USB_H_DESC_LST_ROLLINTR5 : R/W1C; bitpos: [13]; default: 0;
 *  1'b0: No Descriptor rollover interrupt
 *  1'b1: Descriptor rollover interrupt
 */
#define USB_H_DESC_LST_ROLLINTR5    (BIT(13))
#define USB_H_DESC_LST_ROLLINTR5_M  (USB_H_DESC_LST_ROLLINTR5_V << USB_H_DESC_LST_ROLLINTR5_S)
#define USB_H_DESC_LST_ROLLINTR5_V  0x00000001
#define USB_H_DESC_LST_ROLLINTR5_S  13


/** USB_HCINTMSK5_REG register
 *  Host Channel 5 Interrupt Mask Register
 */
#define USB_HCINTMSK5_REG (SOC_DPORT_USB_BASE + 0x5ac)
/** USB_H_XFERCOMPLMSK5 : R/W; bitpos: [0]; default: 0;
 *  1'b0: Transfer Completed Mask
 *  1'b1: No Transfer Completed Mask
 */
#define USB_H_XFERCOMPLMSK5    (BIT(0))
#define USB_H_XFERCOMPLMSK5_M  (USB_H_XFERCOMPLMSK5_V << USB_H_XFERCOMPLMSK5_S)
#define USB_H_XFERCOMPLMSK5_V  0x00000001
#define USB_H_XFERCOMPLMSK5_S  0
/** USB_H_CHHLTDMSK5 : R/W; bitpos: [1]; default: 0;
 *  1'b0: Channel Halted Mask
 *  1'b1: No Channel Halted Mask
 */
#define USB_H_CHHLTDMSK5    (BIT(1))
#define USB_H_CHHLTDMSK5_M  (USB_H_CHHLTDMSK5_V << USB_H_CHHLTDMSK5_S)
#define USB_H_CHHLTDMSK5_V  0x00000001
#define USB_H_CHHLTDMSK5_S  1
/** USB_H_AHBERRMSK5 : R/W; bitpos: [2]; default: 0;
 *  1'b0: AHB Error Mask
 *  1'b1: No AHB Error Mask
 */
#define USB_H_AHBERRMSK5    (BIT(2))
#define USB_H_AHBERRMSK5_M  (USB_H_AHBERRMSK5_V << USB_H_AHBERRMSK5_S)
#define USB_H_AHBERRMSK5_V  0x00000001
#define USB_H_AHBERRMSK5_S  2
/** USB_H_STALLMSK5 : R/W; bitpos: [3]; default: 0;
 *  1'b0: Mask STALL Response Received Interrupt
 *  1'b1: No STALL Response Received Interrupt Mask
 */
#define USB_H_STALLMSK5    (BIT(3))
#define USB_H_STALLMSK5_M  (USB_H_STALLMSK5_V << USB_H_STALLMSK5_S)
#define USB_H_STALLMSK5_V  0x00000001
#define USB_H_STALLMSK5_S  3
/** USB_H_NAKMSK5 : R/W; bitpos: [4]; default: 0;
 *  1'b0: Mask NAK Response Received Interrupt
 *  1'b1: No NAK Response Received Interrupt Mask
 */
#define USB_H_NAKMSK5    (BIT(4))
#define USB_H_NAKMSK5_M  (USB_H_NAKMSK5_V << USB_H_NAKMSK5_S)
#define USB_H_NAKMSK5_V  0x00000001
#define USB_H_NAKMSK5_S  4
/** USB_H_ACKMSK5 : R/W; bitpos: [5]; default: 0;
 *  1'b0: Mask ACK Response Received/Transmitted Interrupt
 *  1'b1: No ACK Response Received/Transmitted Interrupt Mask
 */
#define USB_H_ACKMSK5    (BIT(5))
#define USB_H_ACKMSK5_M  (USB_H_ACKMSK5_V << USB_H_ACKMSK5_S)
#define USB_H_ACKMSK5_V  0x00000001
#define USB_H_ACKMSK5_S  5
/** USB_H_NYETMSK5 : R/W; bitpos: [6]; default: 0;
 *  1'b0: Mask NYET Response Received Interrupt
 *  1'b1: No NYET Response Received Interrupt Mask
 */
#define USB_H_NYETMSK5    (BIT(6))
#define USB_H_NYETMSK5_M  (USB_H_NYETMSK5_V << USB_H_NYETMSK5_S)
#define USB_H_NYETMSK5_V  0x00000001
#define USB_H_NYETMSK5_S  6
/** USB_H_XACTERRMSK5 : R/W; bitpos: [7]; default: 0;
 *  1'b0: Mask Transaction Error
 *  1'b1: No Transaction Error Mask
 */
#define USB_H_XACTERRMSK5    (BIT(7))
#define USB_H_XACTERRMSK5_M  (USB_H_XACTERRMSK5_V << USB_H_XACTERRMSK5_S)
#define USB_H_XACTERRMSK5_V  0x00000001
#define USB_H_XACTERRMSK5_S  7
/** USB_H_BBLERRMSK5 : R/W; bitpos: [8]; default: 0;
 *  Babble Error Mask
 *  1'b0: Mask Babble Error
 *  1'b1: No Babble Error Mask
 */
#define USB_H_BBLERRMSK5    (BIT(8))
#define USB_H_BBLERRMSK5_M  (USB_H_BBLERRMSK5_V << USB_H_BBLERRMSK5_S)
#define USB_H_BBLERRMSK5_V  0x00000001
#define USB_H_BBLERRMSK5_S  8
/** USB_H_FRMOVRUNMSK5 : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun Mask
 *  0x0 (MASK): Mask Overrun Mask
 *  0x1 (NOMASK): No Frame Overrun Mask
 */
#define USB_H_FRMOVRUNMSK5    (BIT(9))
#define USB_H_FRMOVRUNMSK5_M  (USB_H_FRMOVRUNMSK5_V << USB_H_FRMOVRUNMSK5_S)
#define USB_H_FRMOVRUNMSK5_V  0x00000001
#define USB_H_FRMOVRUNMSK5_S  9
/** USB_H_DATATGLERRMSK5 : R/W; bitpos: [10]; default: 0;
 *  Data Toggle Error Mask n scatter/gather DMA mode for host
 *  1'b0: Mask Data Toggle Error
 *  1'b1: No Data Toggle Error Mask
 */
#define USB_H_DATATGLERRMSK5    (BIT(10))
#define USB_H_DATATGLERRMSK5_M  (USB_H_DATATGLERRMSK5_V << USB_H_DATATGLERRMSK5_S)
#define USB_H_DATATGLERRMSK5_V  0x00000001
#define USB_H_DATATGLERRMSK5_S  10
/** USB_H_BNAINTRMSK5 : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt mask register
 *  1'b0: BNA Interrupt Masked
 *  1'b1: BNA Interrupt not masked
 */
#define USB_H_BNAINTRMSK5    (BIT(11))
#define USB_H_BNAINTRMSK5_M  (USB_H_BNAINTRMSK5_V << USB_H_BNAINTRMSK5_S)
#define USB_H_BNAINTRMSK5_V  0x00000001
#define USB_H_BNAINTRMSK5_S  11
/** USB_H_DESC_LST_ROLLINTRMSK5 : R/W; bitpos: [13]; default: 0;
 *  Descriptor List rollover interrupt Mask
 *  1'b0: Descriptor Rollover Interrupt Mask
 *  1'b1: Descriptor Rollover Interrupt not masked
 */
#define USB_H_DESC_LST_ROLLINTRMSK5    (BIT(13))
#define USB_H_DESC_LST_ROLLINTRMSK5_M  (USB_H_DESC_LST_ROLLINTRMSK5_V << USB_H_DESC_LST_ROLLINTRMSK5_S)
#define USB_H_DESC_LST_ROLLINTRMSK5_V  0x00000001
#define USB_H_DESC_LST_ROLLINTRMSK5_S  13


/** USB_HCINT6_REG register
 *  Host Channel 6 Interrupt Register
 */
#define USB_HCINT6_REG (SOC_DPORT_USB_BASE + 0x5c8)
/** USB_H_XFERCOMPL6 : R/W1C; bitpos: [0]; default: 0;
 *  1'b0: Transfer in progress or No Active Transfer
 *  1'b1: Transfer completed normally without any errors
 */
#define USB_H_XFERCOMPL6    (BIT(0))
#define USB_H_XFERCOMPL6_M  (USB_H_XFERCOMPL6_V << USB_H_XFERCOMPL6_S)
#define USB_H_XFERCOMPL6_V  0x00000001
#define USB_H_XFERCOMPL6_S  0
/** USB_H_CHHLTD6 : R/W1C; bitpos: [1]; default: 0;
 *  1'b0: Channel not halted
 *  1'b1: Channel Halted
 */
#define USB_H_CHHLTD6    (BIT(1))
#define USB_H_CHHLTD6_M  (USB_H_CHHLTD6_V << USB_H_CHHLTD6_S)
#define USB_H_CHHLTD6_V  0x00000001
#define USB_H_CHHLTD6_S  1
/** USB_H_AHBERR6 : R/W1C; bitpos: [2]; default: 0;
 *  1'b0: No AHB error
 *  1'b1: AHB error during AHB read/write
 */
#define USB_H_AHBERR6    (BIT(2))
#define USB_H_AHBERR6_M  (USB_H_AHBERR6_V << USB_H_AHBERR6_S)
#define USB_H_AHBERR6_V  0x00000001
#define USB_H_AHBERR6_S  2
/** USB_H_STALL6 : R/W1C; bitpos: [3]; default: 0;
 *  1'b0: No Stall Response Received Interrupt
 *  1'b1: Stall Response Received Interrupt
 */
#define USB_H_STALL6    (BIT(3))
#define USB_H_STALL6_M  (USB_H_STALL6_V << USB_H_STALL6_S)
#define USB_H_STALL6_V  0x00000001
#define USB_H_STALL6_S  3
/** USB_H_NACK6 : R/W1C; bitpos: [4]; default: 0;
 *  1'b0: No NAK Response Received Interrupt
 *  1'b1: NAK Response Received Interrupt
 */
#define USB_H_NACK6    (BIT(4))
#define USB_H_NACK6_M  (USB_H_NACK6_V << USB_H_NACK6_S)
#define USB_H_NACK6_V  0x00000001
#define USB_H_NACK6_S  4
/** USB_H_ACK6 : R/W1C; bitpos: [5]; default: 0;
 *  1'b0: No ACK Response Received or Transmitted Interrupt
 *  1'b1: ACK Response Received or Transmitted Interrup
 */
#define USB_H_ACK6    (BIT(5))
#define USB_H_ACK6_M  (USB_H_ACK6_V << USB_H_ACK6_S)
#define USB_H_ACK6_V  0x00000001
#define USB_H_ACK6_S  5
/** USB_H_NYET6 : R/W1C; bitpos: [6]; default: 0;
 *  1'b0: No NYET Response Received Interrupt
 *  1'b1: NYET Response Received Interrupt
 */
#define USB_H_NYET6    (BIT(6))
#define USB_H_NYET6_M  (USB_H_NYET6_V << USB_H_NYET6_S)
#define USB_H_NYET6_V  0x00000001
#define USB_H_NYET6_S  6
/** USB_H_XACTERR6 : R/W1C; bitpos: [7]; default: 0;
 *  Indicates one of the following errors occurred on the USB:
 *  CRC check failure
 *  Timeout
 *  Bit stuff error
 *  False EOP
 */
#define USB_H_XACTERR6    (BIT(7))
#define USB_H_XACTERR6_M  (USB_H_XACTERR6_V << USB_H_XACTERR6_S)
#define USB_H_XACTERR6_V  0x00000001
#define USB_H_XACTERR6_S  7
/** USB_H_BBLERR6 : R/W1C; bitpos: [8]; default: 0;
 *  1'b0: No Babble Error
 *  1'b1: Babble Error
 */
#define USB_H_BBLERR6    (BIT(8))
#define USB_H_BBLERR6_M  (USB_H_BBLERR6_V << USB_H_BBLERR6_S)
#define USB_H_BBLERR6_V  0x00000001
#define USB_H_BBLERR6_S  8
/** USB_H_FRMOVRUN6 : R/W1C; bitpos: [9]; default: 0;
 *  1'b0: No Frame Overrun
 *  1'b1: Frame Overrun
 */
#define USB_H_FRMOVRUN6    (BIT(9))
#define USB_H_FRMOVRUN6_M  (USB_H_FRMOVRUN6_V << USB_H_FRMOVRUN6_S)
#define USB_H_FRMOVRUN6_V  0x00000001
#define USB_H_FRMOVRUN6_S  9
/** USB_H_DATATGLERR6 : R/W1C; bitpos: [10]; default: 0;
 *  1'b0: No Data Toggle Error
 *  1'b1: Data Toggle Error
 */
#define USB_H_DATATGLERR6    (BIT(10))
#define USB_H_DATATGLERR6_M  (USB_H_DATATGLERR6_V << USB_H_DATATGLERR6_S)
#define USB_H_DATATGLERR6_V  0x00000001
#define USB_H_DATATGLERR6_S  10
/** USB_H_BNAINTR6 : R/W1C; bitpos: [11]; default: 0;
 *  1'b0: No BNA Interrupt
 *  1'b1: BNA Interrupt
 */
#define USB_H_BNAINTR6    (BIT(11))
#define USB_H_BNAINTR6_M  (USB_H_BNAINTR6_V << USB_H_BNAINTR6_S)
#define USB_H_BNAINTR6_V  0x00000001
#define USB_H_BNAINTR6_S  11
/** USB_H_XCS_XACT_ERR6 : R/W1C; bitpos: [12]; default: 0;
 *  1'b0: No Excessive Transaction Error
 *  1'b1: Excessive Transaction Error
 */
#define USB_H_XCS_XACT_ERR6    (BIT(12))
#define USB_H_XCS_XACT_ERR6_M  (USB_H_XCS_XACT_ERR6_V << USB_H_XCS_XACT_ERR6_S)
#define USB_H_XCS_XACT_ERR6_V  0x00000001
#define USB_H_XCS_XACT_ERR6_S  12
/** USB_H_DESC_LST_ROLLINTR6 : R/W1C; bitpos: [13]; default: 0;
 *  1'b0: No Descriptor rollover interrupt
 *  1'b1: Descriptor rollover interrupt
 */
#define USB_H_DESC_LST_ROLLINTR6    (BIT(13))
#define USB_H_DESC_LST_ROLLINTR6_M  (USB_H_DESC_LST_ROLLINTR6_V << USB_H_DESC_LST_ROLLINTR6_S)
#define USB_H_DESC_LST_ROLLINTR6_V  0x00000001
#define USB_H_DESC_LST_ROLLINTR6_S  13


/** USB_HCINTMSK6_REG register
 *  Host Channel 6 Interrupt Mask Register
 */
#define USB_HCINTMSK6_REG (SOC_DPORT_USB_BASE + 0x5cc)
/** USB_H_XFERCOMPLMSK6 : R/W; bitpos: [0]; default: 0;
 *  1'b0: Transfer Completed Mask
 *  1'b1: No Transfer Completed Mask
 */
#define USB_H_XFERCOMPLMSK6    (BIT(0))
#define USB_H_XFERCOMPLMSK6_M  (USB_H_XFERCOMPLMSK6_V << USB_H_XFERCOMPLMSK6_S)
#define USB_H_XFERCOMPLMSK6_V  0x00000001
#define USB_H_XFERCOMPLMSK6_S  0
/** USB_H_CHHLTDMSK6 : R/W; bitpos: [1]; default: 0;
 *  1'b0: Channel Halted Mask
 *  1'b1: No Channel Halted Mask
 */
#define USB_H_CHHLTDMSK6    (BIT(1))
#define USB_H_CHHLTDMSK6_M  (USB_H_CHHLTDMSK6_V << USB_H_CHHLTDMSK6_S)
#define USB_H_CHHLTDMSK6_V  0x00000001
#define USB_H_CHHLTDMSK6_S  1
/** USB_H_AHBERRMSK6 : R/W; bitpos: [2]; default: 0;
 *  1'b0: AHB Error Mask
 *  1'b1: No AHB Error Mask
 */
#define USB_H_AHBERRMSK6    (BIT(2))
#define USB_H_AHBERRMSK6_M  (USB_H_AHBERRMSK6_V << USB_H_AHBERRMSK6_S)
#define USB_H_AHBERRMSK6_V  0x00000001
#define USB_H_AHBERRMSK6_S  2
/** USB_H_STALLMSK6 : R/W; bitpos: [3]; default: 0;
 *  1'b0: Mask STALL Response Received Interrupt
 *  1'b1: No STALL Response Received Interrupt Mask
 */
#define USB_H_STALLMSK6    (BIT(3))
#define USB_H_STALLMSK6_M  (USB_H_STALLMSK6_V << USB_H_STALLMSK6_S)
#define USB_H_STALLMSK6_V  0x00000001
#define USB_H_STALLMSK6_S  3
/** USB_H_NAKMSK6 : R/W; bitpos: [4]; default: 0;
 *  1'b0: Mask NAK Response Received Interrupt
 *  1'b1: No NAK Response Received Interrupt Mask
 */
#define USB_H_NAKMSK6    (BIT(4))
#define USB_H_NAKMSK6_M  (USB_H_NAKMSK6_V << USB_H_NAKMSK6_S)
#define USB_H_NAKMSK6_V  0x00000001
#define USB_H_NAKMSK6_S  4
/** USB_H_ACKMSK6 : R/W; bitpos: [5]; default: 0;
 *  1'b0: Mask ACK Response Received/Transmitted Interrupt
 *  1'b1: No ACK Response Received/Transmitted Interrupt Mask
 */
#define USB_H_ACKMSK6    (BIT(5))
#define USB_H_ACKMSK6_M  (USB_H_ACKMSK6_V << USB_H_ACKMSK6_S)
#define USB_H_ACKMSK6_V  0x00000001
#define USB_H_ACKMSK6_S  5
/** USB_H_NYETMSK6 : R/W; bitpos: [6]; default: 0;
 *  1'b0: Mask NYET Response Received Interrupt
 *  1'b1: No NYET Response Received Interrupt Mask
 */
#define USB_H_NYETMSK6    (BIT(6))
#define USB_H_NYETMSK6_M  (USB_H_NYETMSK6_V << USB_H_NYETMSK6_S)
#define USB_H_NYETMSK6_V  0x00000001
#define USB_H_NYETMSK6_S  6
/** USB_H_XACTERRMSK6 : R/W; bitpos: [7]; default: 0;
 *  1'b0: Mask Transaction Error
 *  1'b1: No Transaction Error Mask
 */
#define USB_H_XACTERRMSK6    (BIT(7))
#define USB_H_XACTERRMSK6_M  (USB_H_XACTERRMSK6_V << USB_H_XACTERRMSK6_S)
#define USB_H_XACTERRMSK6_V  0x00000001
#define USB_H_XACTERRMSK6_S  7
/** USB_H_BBLERRMSK6 : R/W; bitpos: [8]; default: 0;
 *  Babble Error Mask
 *  1'b0: Mask Babble Error
 *  1'b1: No Babble Error Mask
 */
#define USB_H_BBLERRMSK6    (BIT(8))
#define USB_H_BBLERRMSK6_M  (USB_H_BBLERRMSK6_V << USB_H_BBLERRMSK6_S)
#define USB_H_BBLERRMSK6_V  0x00000001
#define USB_H_BBLERRMSK6_S  8
/** USB_H_FRMOVRUNMSK6 : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun Mask
 *  0x0 (MASK): Mask Overrun Mask
 *  0x1 (NOMASK): No Frame Overrun Mask
 */
#define USB_H_FRMOVRUNMSK6    (BIT(9))
#define USB_H_FRMOVRUNMSK6_M  (USB_H_FRMOVRUNMSK6_V << USB_H_FRMOVRUNMSK6_S)
#define USB_H_FRMOVRUNMSK6_V  0x00000001
#define USB_H_FRMOVRUNMSK6_S  9
/** USB_H_DATATGLERRMSK6 : R/W; bitpos: [10]; default: 0;
 *  Data Toggle Error Mask n scatter/gather DMA mode for host
 *  1'b0: Mask Data Toggle Error
 *  1'b1: No Data Toggle Error Mask
 */
#define USB_H_DATATGLERRMSK6    (BIT(10))
#define USB_H_DATATGLERRMSK6_M  (USB_H_DATATGLERRMSK6_V << USB_H_DATATGLERRMSK6_S)
#define USB_H_DATATGLERRMSK6_V  0x00000001
#define USB_H_DATATGLERRMSK6_S  10
/** USB_H_BNAINTRMSK6 : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt mask register
 *  1'b0: BNA Interrupt Masked
 *  1'b1: BNA Interrupt not masked
 */
#define USB_H_BNAINTRMSK6    (BIT(11))
#define USB_H_BNAINTRMSK6_M  (USB_H_BNAINTRMSK6_V << USB_H_BNAINTRMSK6_S)
#define USB_H_BNAINTRMSK6_V  0x00000001
#define USB_H_BNAINTRMSK6_S  11
/** USB_H_DESC_LST_ROLLINTRMSK6 : R/W; bitpos: [13]; default: 0;
 *  Descriptor List rollover interrupt Mask
 *  1'b0: Descriptor Rollover Interrupt Mask
 *  1'b1: Descriptor Rollover Interrupt not masked
 */
#define USB_H_DESC_LST_ROLLINTRMSK6    (BIT(13))
#define USB_H_DESC_LST_ROLLINTRMSK6_M  (USB_H_DESC_LST_ROLLINTRMSK6_V << USB_H_DESC_LST_ROLLINTRMSK6_S)
#define USB_H_DESC_LST_ROLLINTRMSK6_V  0x00000001
#define USB_H_DESC_LST_ROLLINTRMSK6_S  13


/** USB_HCINT7_REG register
 *  Host Channel 7 Interrupt Register
 */
#define USB_HCINT7_REG (SOC_DPORT_USB_BASE + 0x5e8)
/** USB_H_XFERCOMPL7 : R/W1C; bitpos: [0]; default: 0;
 *  1'b0: Transfer in progress or No Active Transfer
 *  1'b1: Transfer completed normally without any errors
 */
#define USB_H_XFERCOMPL7    (BIT(0))
#define USB_H_XFERCOMPL7_M  (USB_H_XFERCOMPL7_V << USB_H_XFERCOMPL7_S)
#define USB_H_XFERCOMPL7_V  0x00000001
#define USB_H_XFERCOMPL7_S  0
/** USB_H_CHHLTD7 : R/W1C; bitpos: [1]; default: 0;
 *  1'b0: Channel not halted
 *  1'b1: Channel Halted
 */
#define USB_H_CHHLTD7    (BIT(1))
#define USB_H_CHHLTD7_M  (USB_H_CHHLTD7_V << USB_H_CHHLTD7_S)
#define USB_H_CHHLTD7_V  0x00000001
#define USB_H_CHHLTD7_S  1
/** USB_H_AHBERR7 : R/W1C; bitpos: [2]; default: 0;
 *  1'b0: No AHB error
 *  1'b1: AHB error during AHB read/write
 */
#define USB_H_AHBERR7    (BIT(2))
#define USB_H_AHBERR7_M  (USB_H_AHBERR7_V << USB_H_AHBERR7_S)
#define USB_H_AHBERR7_V  0x00000001
#define USB_H_AHBERR7_S  2
/** USB_H_STALL7 : R/W1C; bitpos: [3]; default: 0;
 *  1'b0: No Stall Response Received Interrupt
 *  1'b1: Stall Response Received Interrupt
 */
#define USB_H_STALL7    (BIT(3))
#define USB_H_STALL7_M  (USB_H_STALL7_V << USB_H_STALL7_S)
#define USB_H_STALL7_V  0x00000001
#define USB_H_STALL7_S  3
/** USB_H_NACK7 : R/W1C; bitpos: [4]; default: 0;
 *  1'b0: No NAK Response Received Interrupt
 *  1'b1: NAK Response Received Interrupt
 */
#define USB_H_NACK7    (BIT(4))
#define USB_H_NACK7_M  (USB_H_NACK7_V << USB_H_NACK7_S)
#define USB_H_NACK7_V  0x00000001
#define USB_H_NACK7_S  4
/** USB_H_ACK7 : R/W1C; bitpos: [5]; default: 0;
 *  1'b0: No ACK Response Received or Transmitted Interrupt
 *  1'b1: ACK Response Received or Transmitted Interrup
 */
#define USB_H_ACK7    (BIT(5))
#define USB_H_ACK7_M  (USB_H_ACK7_V << USB_H_ACK7_S)
#define USB_H_ACK7_V  0x00000001
#define USB_H_ACK7_S  5
/** USB_H_NYET7 : R/W1C; bitpos: [6]; default: 0;
 *  1'b0: No NYET Response Received Interrupt
 *  1'b1: NYET Response Received Interrupt
 */
#define USB_H_NYET7    (BIT(6))
#define USB_H_NYET7_M  (USB_H_NYET7_V << USB_H_NYET7_S)
#define USB_H_NYET7_V  0x00000001
#define USB_H_NYET7_S  6
/** USB_H_XACTERR7 : R/W1C; bitpos: [7]; default: 0;
 *  Indicates one of the following errors occurred on the USB:
 *  CRC check failure
 *  Timeout
 *  Bit stuff error
 *  False EOP
 */
#define USB_H_XACTERR7    (BIT(7))
#define USB_H_XACTERR7_M  (USB_H_XACTERR7_V << USB_H_XACTERR7_S)
#define USB_H_XACTERR7_V  0x00000001
#define USB_H_XACTERR7_S  7
/** USB_H_BBLERR7 : R/W1C; bitpos: [8]; default: 0;
 *  1'b0: No Babble Error
 *  1'b1: Babble Error
 */
#define USB_H_BBLERR7    (BIT(8))
#define USB_H_BBLERR7_M  (USB_H_BBLERR7_V << USB_H_BBLERR7_S)
#define USB_H_BBLERR7_V  0x00000001
#define USB_H_BBLERR7_S  8
/** USB_H_FRMOVRUN7 : R/W1C; bitpos: [9]; default: 0;
 *  1'b0: No Frame Overrun
 *  1'b1: Frame Overrun
 */
#define USB_H_FRMOVRUN7    (BIT(9))
#define USB_H_FRMOVRUN7_M  (USB_H_FRMOVRUN7_V << USB_H_FRMOVRUN7_S)
#define USB_H_FRMOVRUN7_V  0x00000001
#define USB_H_FRMOVRUN7_S  9
/** USB_H_DATATGLERR7 : R/W1C; bitpos: [10]; default: 0;
 *  1'b0: No Data Toggle Error
 *  1'b1: Data Toggle Error
 */
#define USB_H_DATATGLERR7    (BIT(10))
#define USB_H_DATATGLERR7_M  (USB_H_DATATGLERR7_V << USB_H_DATATGLERR7_S)
#define USB_H_DATATGLERR7_V  0x00000001
#define USB_H_DATATGLERR7_S  10
/** USB_H_BNAINTR7 : R/W1C; bitpos: [11]; default: 0;
 *  1'b0: No BNA Interrupt
 *  1'b1: BNA Interrupt
 */
#define USB_H_BNAINTR7    (BIT(11))
#define USB_H_BNAINTR7_M  (USB_H_BNAINTR7_V << USB_H_BNAINTR7_S)
#define USB_H_BNAINTR7_V  0x00000001
#define USB_H_BNAINTR7_S  11
/** USB_H_XCS_XACT_ERR7 : R/W1C; bitpos: [12]; default: 0;
 *  1'b0: No Excessive Transaction Error
 *  1'b1: Excessive Transaction Error
 */
#define USB_H_XCS_XACT_ERR7    (BIT(12))
#define USB_H_XCS_XACT_ERR7_M  (USB_H_XCS_XACT_ERR7_V << USB_H_XCS_XACT_ERR7_S)
#define USB_H_XCS_XACT_ERR7_V  0x00000001
#define USB_H_XCS_XACT_ERR7_S  12
/** USB_H_DESC_LST_ROLLINTR7 : R/W1C; bitpos: [13]; default: 0;
 *  1'b0: No Descriptor rollover interrupt
 *  1'b1: Descriptor rollover interrupt
 */
#define USB_H_DESC_LST_ROLLINTR7    (BIT(13))
#define USB_H_DESC_LST_ROLLINTR7_M  (USB_H_DESC_LST_ROLLINTR7_V << USB_H_DESC_LST_ROLLINTR7_S)
#define USB_H_DESC_LST_ROLLINTR7_V  0x00000001
#define USB_H_DESC_LST_ROLLINTR7_S  13


/** USB_HCINTMSK7_REG register
 *  Host Channel 7 Interrupt Mask Register
 */
#define USB_HCINTMSK7_REG (SOC_DPORT_USB_BASE + 0x5ec)
/** USB_H_XFERCOMPLMSK7 : R/W; bitpos: [0]; default: 0;
 *  1'b0: Transfer Completed Mask
 *  1'b1: No Transfer Completed Mask
 */
#define USB_H_XFERCOMPLMSK7    (BIT(0))
#define USB_H_XFERCOMPLMSK7_M  (USB_H_XFERCOMPLMSK7_V << USB_H_XFERCOMPLMSK7_S)
#define USB_H_XFERCOMPLMSK7_V  0x00000001
#define USB_H_XFERCOMPLMSK7_S  0
/** USB_H_CHHLTDMSK7 : R/W; bitpos: [1]; default: 0;
 *  1'b0: Channel Halted Mask
 *  1'b1: No Channel Halted Mask
 */
#define USB_H_CHHLTDMSK7    (BIT(1))
#define USB_H_CHHLTDMSK7_M  (USB_H_CHHLTDMSK7_V << USB_H_CHHLTDMSK7_S)
#define USB_H_CHHLTDMSK7_V  0x00000001
#define USB_H_CHHLTDMSK7_S  1
/** USB_H_AHBERRMSK7 : R/W; bitpos: [2]; default: 0;
 *  1'b0: AHB Error Mask
 *  1'b1: No AHB Error Mask
 */
#define USB_H_AHBERRMSK7    (BIT(2))
#define USB_H_AHBERRMSK7_M  (USB_H_AHBERRMSK7_V << USB_H_AHBERRMSK7_S)
#define USB_H_AHBERRMSK7_V  0x00000001
#define USB_H_AHBERRMSK7_S  2
/** USB_H_STALLMSK7 : R/W; bitpos: [3]; default: 0;
 *  1'b0: Mask STALL Response Received Interrupt
 *  1'b1: No STALL Response Received Interrupt Mask
 */
#define USB_H_STALLMSK7    (BIT(3))
#define USB_H_STALLMSK7_M  (USB_H_STALLMSK7_V << USB_H_STALLMSK7_S)
#define USB_H_STALLMSK7_V  0x00000001
#define USB_H_STALLMSK7_S  3
/** USB_H_NAKMSK7 : R/W; bitpos: [4]; default: 0;
 *  1'b0: Mask NAK Response Received Interrupt
 *  1'b1: No NAK Response Received Interrupt Mask
 */
#define USB_H_NAKMSK7    (BIT(4))
#define USB_H_NAKMSK7_M  (USB_H_NAKMSK7_V << USB_H_NAKMSK7_S)
#define USB_H_NAKMSK7_V  0x00000001
#define USB_H_NAKMSK7_S  4
/** USB_H_ACKMSK7 : R/W; bitpos: [5]; default: 0;
 *  1'b0: Mask ACK Response Received/Transmitted Interrupt
 *  1'b1: No ACK Response Received/Transmitted Interrupt Mask
 */
#define USB_H_ACKMSK7    (BIT(5))
#define USB_H_ACKMSK7_M  (USB_H_ACKMSK7_V << USB_H_ACKMSK7_S)
#define USB_H_ACKMSK7_V  0x00000001
#define USB_H_ACKMSK7_S  5
/** USB_H_NYETMSK7 : R/W; bitpos: [6]; default: 0;
 *  1'b0: Mask NYET Response Received Interrupt
 *  1'b1: No NYET Response Received Interrupt Mask
 */
#define USB_H_NYETMSK7    (BIT(6))
#define USB_H_NYETMSK7_M  (USB_H_NYETMSK7_V << USB_H_NYETMSK7_S)
#define USB_H_NYETMSK7_V  0x00000001
#define USB_H_NYETMSK7_S  6
/** USB_H_XACTERRMSK7 : R/W; bitpos: [7]; default: 0;
 *  1'b0: Mask Transaction Error
 *  1'b1: No Transaction Error Mask
 */
#define USB_H_XACTERRMSK7    (BIT(7))
#define USB_H_XACTERRMSK7_M  (USB_H_XACTERRMSK7_V << USB_H_XACTERRMSK7_S)
#define USB_H_XACTERRMSK7_V  0x00000001
#define USB_H_XACTERRMSK7_S  7
/** USB_H_BBLERRMSK7 : R/W; bitpos: [8]; default: 0;
 *  Babble Error Mask
 *  1'b0: Mask Babble Error
 *  1'b1: No Babble Error Mask
 */
#define USB_H_BBLERRMSK7    (BIT(8))
#define USB_H_BBLERRMSK7_M  (USB_H_BBLERRMSK7_V << USB_H_BBLERRMSK7_S)
#define USB_H_BBLERRMSK7_V  0x00000001
#define USB_H_BBLERRMSK7_S  8
/** USB_H_FRMOVRUNMSK7 : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun Mask
 *  0x0 (MASK): Mask Overrun Mask
 *  0x1 (NOMASK): No Frame Overrun Mask
 */
#define USB_H_FRMOVRUNMSK7    (BIT(9))
#define USB_H_FRMOVRUNMSK7_M  (USB_H_FRMOVRUNMSK7_V << USB_H_FRMOVRUNMSK7_S)
#define USB_H_FRMOVRUNMSK7_V  0x00000001
#define USB_H_FRMOVRUNMSK7_S  9
/** USB_H_DATATGLERRMSK7 : R/W; bitpos: [10]; default: 0;
 *  Data Toggle Error Mask n scatter/gather DMA mode for host
 *  1'b0: Mask Data Toggle Error
 *  1'b1: No Data Toggle Error Mask
 */
#define USB_H_DATATGLERRMSK7    (BIT(10))
#define USB_H_DATATGLERRMSK7_M  (USB_H_DATATGLERRMSK7_V << USB_H_DATATGLERRMSK7_S)
#define USB_H_DATATGLERRMSK7_V  0x00000001
#define USB_H_DATATGLERRMSK7_S  10
/** USB_H_BNAINTRMSK7 : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt mask register
 *  1'b0: BNA Interrupt Masked
 *  1'b1: BNA Interrupt not masked
 */
#define USB_H_BNAINTRMSK7    (BIT(11))
#define USB_H_BNAINTRMSK7_M  (USB_H_BNAINTRMSK7_V << USB_H_BNAINTRMSK7_S)
#define USB_H_BNAINTRMSK7_V  0x00000001
#define USB_H_BNAINTRMSK7_S  11
/** USB_H_DESC_LST_ROLLINTRMSK7 : R/W; bitpos: [13]; default: 0;
 *  Descriptor List rollover interrupt Mask
 *  1'b0: Descriptor Rollover Interrupt Mask
 *  1'b1: Descriptor Rollover Interrupt not masked
 */
#define USB_H_DESC_LST_ROLLINTRMSK7    (BIT(13))
#define USB_H_DESC_LST_ROLLINTRMSK7_M  (USB_H_DESC_LST_ROLLINTRMSK7_V << USB_H_DESC_LST_ROLLINTRMSK7_S)
#define USB_H_DESC_LST_ROLLINTRMSK7_V  0x00000001
#define USB_H_DESC_LST_ROLLINTRMSK7_S  13


/** USB_DIEPMSK_REG register
 *  Device IN Endpoint Common Interrupt Mask Register
 */
#define USB_DIEPMSK_REG (SOC_DPORT_USB_BASE + 0x810)
/** USB_DI_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *  0x0 : Mask Transfer Completed Interrupt
 *  0x1 : No Transfer Completed Interrupt Mask
 */
#define USB_DI_XFERCOMPLMSK    (BIT(0))
#define USB_DI_XFERCOMPLMSK_M  (USB_DI_XFERCOMPLMSK_V << USB_DI_XFERCOMPLMSK_S)
#define USB_DI_XFERCOMPLMSK_V  0x00000001
#define USB_DI_XFERCOMPLMSK_S  0
/** USB_DI_EPDISBLDMSK : R/W; bitpos: [1]; default: 0;
 *  0x0 : Mask Endpoint Disabled Interrupt
 *  0x1 : No Endpoint Disabled Interrupt Mask
 */
#define USB_DI_EPDISBLDMSK    (BIT(1))
#define USB_DI_EPDISBLDMSK_M  (USB_DI_EPDISBLDMSK_V << USB_DI_EPDISBLDMSK_S)
#define USB_DI_EPDISBLDMSK_V  0x00000001
#define USB_DI_EPDISBLDMSK_S  1
/** USB_DI_AHBERMSK : R/W; bitpos: [2]; default: 0;
 *  0x0 : Mask AHB Error Interrupt
 *  0x1 : No AHB Error Interrupt Mask
 */
#define USB_DI_AHBERMSK    (BIT(2))
#define USB_DI_AHBERMSK_M  (USB_DI_AHBERMSK_V << USB_DI_AHBERMSK_S)
#define USB_DI_AHBERMSK_V  0x00000001
#define USB_DI_AHBERMSK_S  2
/** USB_TIMEOUTMSK : R/W; bitpos: [3]; default: 0;
 *  0x0 : Mask Timeout Condition Interrupt
 *  0x1 : No Timeout Condition Interrupt Mask
 */
#define USB_TIMEOUTMSK    (BIT(3))
#define USB_TIMEOUTMSK_M  (USB_TIMEOUTMSK_V << USB_TIMEOUTMSK_S)
#define USB_TIMEOUTMSK_V  0x00000001
#define USB_TIMEOUTMSK_S  3
/** USB_INTKNTXFEMPMSK : R/W; bitpos: [4]; default: 0;
 *  0x0 : Mask IN Token Received When TxFIFO Empty Interrupt
 *  0x1 : No IN Token Received When TxFIFO Empty Interrupt
 */
#define USB_INTKNTXFEMPMSK    (BIT(4))
#define USB_INTKNTXFEMPMSK_M  (USB_INTKNTXFEMPMSK_V << USB_INTKNTXFEMPMSK_S)
#define USB_INTKNTXFEMPMSK_V  0x00000001
#define USB_INTKNTXFEMPMSK_S  4
/** USB_INTKNEPMISMSK : R/W; bitpos: [5]; default: 0;
 *  0x0 : Mask IN Token received with EP Mismatch Interrupt
 *  0x1 : No Mask IN Token received with EP Mismatch Interrupt
 */
#define USB_INTKNEPMISMSK    (BIT(5))
#define USB_INTKNEPMISMSK_M  (USB_INTKNEPMISMSK_V << USB_INTKNEPMISMSK_S)
#define USB_INTKNEPMISMSK_V  0x00000001
#define USB_INTKNEPMISMSK_S  5
/** USB_INEPNAKEFFMSK : R/W; bitpos: [6]; default: 0;
 *  0x0 : Mask IN Endpoint NAK Effective Interrupt
 *  0x1 : No IN Endpoint NAK Effective Interrupt Mask
 */
#define USB_INEPNAKEFFMSK    (BIT(6))
#define USB_INEPNAKEFFMSK_M  (USB_INEPNAKEFFMSK_V << USB_INEPNAKEFFMSK_S)
#define USB_INEPNAKEFFMSK_V  0x00000001
#define USB_INEPNAKEFFMSK_S  6
/** USB_TXFIFOUNDRNMSK : R/W; bitpos: [8]; default: 0;
 *  0x0 : Mask Fifo Underrun Interrupt
 *  0x1 : No Fifo Underrun Interrupt Mask
 */
#define USB_TXFIFOUNDRNMSK    (BIT(8))
#define USB_TXFIFOUNDRNMSK_M  (USB_TXFIFOUNDRNMSK_V << USB_TXFIFOUNDRNMSK_S)
#define USB_TXFIFOUNDRNMSK_V  0x00000001
#define USB_TXFIFOUNDRNMSK_S  8
/** USB_BNAININTRMSK : R/W; bitpos: [9]; default: 0;
 *  0x0 : Mask BNA Interrupt
 *  0x1 : No BNA Interrupt Mask
 */
#define USB_BNAININTRMSK    (BIT(9))
#define USB_BNAININTRMSK_M  (USB_BNAININTRMSK_V << USB_BNAININTRMSK_S)
#define USB_BNAININTRMSK_V  0x00000001
#define USB_BNAININTRMSK_S  9
/** USB_DI_NAKMSK : R/W; bitpos: [13]; default: 0;
 *  0x0 : Mask NAK Interrupt
 *  0x1 : No Mask NAK Interrupt
 */
#define USB_DI_NAKMSK    (BIT(13))
#define USB_DI_NAKMSK_M  (USB_DI_NAKMSK_V << USB_DI_NAKMSK_S)
#define USB_DI_NAKMSK_V  0x00000001
#define USB_DI_NAKMSK_S  13


/** USB_DOEPMSK_REG register
 *  Device OUT Endpoint Common Interrupt Mask Register
 */
#define USB_DOEPMSK_REG (SOC_DPORT_USB_BASE + 0x814)
/** USB_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *  0x0 : Mask Transfer Completed Interrupt
 *  0x1 : No Transfer Completed Interrupt Mask
 */
#define USB_XFERCOMPLMSK    (BIT(0))
#define USB_XFERCOMPLMSK_M  (USB_XFERCOMPLMSK_V << USB_XFERCOMPLMSK_S)
#define USB_XFERCOMPLMSK_V  0x00000001
#define USB_XFERCOMPLMSK_S  0
/** USB_EPDISBLDMSK : R/W; bitpos: [1]; default: 0;
 *  0x0 : Mask Endpoint Disabled Interrupt
 *  0x1 : No Endpoint Disabled Interrupt Mask
 */
#define USB_EPDISBLDMSK    (BIT(1))
#define USB_EPDISBLDMSK_M  (USB_EPDISBLDMSK_V << USB_EPDISBLDMSK_S)
#define USB_EPDISBLDMSK_V  0x00000001
#define USB_EPDISBLDMSK_S  1
/** USB_AHBERMSK : R/W; bitpos: [2]; default: 0;
 *  0x0 : Mask AHB Error Interrupt
 *  0x1 : No AHB Error Interrupt Mask
 */
#define USB_AHBERMSK    (BIT(2))
#define USB_AHBERMSK_M  (USB_AHBERMSK_V << USB_AHBERMSK_S)
#define USB_AHBERMSK_V  0x00000001
#define USB_AHBERMSK_S  2
/** USB_SETUPMSK : R/W; bitpos: [3]; default: 0;
 *  0x0 : Mask SETUP Phase Done Interrupt
 *  0x1 : No SETUP Phase Done Interrupt Mask
 */
#define USB_SETUPMSK    (BIT(3))
#define USB_SETUPMSK_M  (USB_SETUPMSK_V << USB_SETUPMSK_S)
#define USB_SETUPMSK_V  0x00000001
#define USB_SETUPMSK_S  3
/** USB_OUTTKNEPDISMSK : R/W; bitpos: [4]; default: 0;
 *  0x0 : Mask OUT Token Received when Endpoint Disabled Interrupt
 *  0x1 : No OUT Token Received when Endpoint Disabled Interrupt Mask
 */
#define USB_OUTTKNEPDISMSK    (BIT(4))
#define USB_OUTTKNEPDISMSK_M  (USB_OUTTKNEPDISMSK_V << USB_OUTTKNEPDISMSK_S)
#define USB_OUTTKNEPDISMSK_V  0x00000001
#define USB_OUTTKNEPDISMSK_S  4
/** USB_STSPHSERCVDMSK : R/W; bitpos: [5]; default: 0;
 *  0x0 : Status Phase Received Mask
 *  0x1 : No Status Phase Received Mask
 */
#define USB_STSPHSERCVDMSK    (BIT(5))
#define USB_STSPHSERCVDMSK_M  (USB_STSPHSERCVDMSK_V << USB_STSPHSERCVDMSK_S)
#define USB_STSPHSERCVDMSK_V  0x00000001
#define USB_STSPHSERCVDMSK_S  5
/** USB_BACK2BACKSETUP : R/W; bitpos: [6]; default: 0;
 *  0x0 : Mask Back-to-Back SETUP Packets Received Interrupt
 *  0x1 : No Back-to-Back SETUP Packets Received Interrupt Mask
 */
#define USB_BACK2BACKSETUP    (BIT(6))
#define USB_BACK2BACKSETUP_M  (USB_BACK2BACKSETUP_V << USB_BACK2BACKSETUP_S)
#define USB_BACK2BACKSETUP_V  0x00000001
#define USB_BACK2BACKSETUP_S  6
/** USB_OUTPKTERRMSK : R/W; bitpos: [8]; default: 0;
 *  0x0 : Mask OUT Packet Error Interrupt
 *  0x1 : No OUT Packet Error Interrupt Mask
 */
#define USB_OUTPKTERRMSK    (BIT(8))
#define USB_OUTPKTERRMSK_M  (USB_OUTPKTERRMSK_V << USB_OUTPKTERRMSK_S)
#define USB_OUTPKTERRMSK_V  0x00000001
#define USB_OUTPKTERRMSK_S  8
/** USB_BNAOUTINTRMSK : R/W; bitpos: [9]; default: 0;
 *  0x0 : Mask BNA Interrupt
 *  0x1 : No BNA Interrupt Mask
 */
#define USB_BNAOUTINTRMSK    (BIT(9))
#define USB_BNAOUTINTRMSK_M  (USB_BNAOUTINTRMSK_V << USB_BNAOUTINTRMSK_S)
#define USB_BNAOUTINTRMSK_V  0x00000001
#define USB_BNAOUTINTRMSK_S  9
/** USB_BBLEERRMSK : R/W; bitpos: [12]; default: 0;
 *  0x0 : Mask Babble Error Interrupt
 *  0x1 : No Babble Error Interrupt Mask
 */
#define USB_BBLEERRMSK    (BIT(12))
#define USB_BBLEERRMSK_M  (USB_BBLEERRMSK_V << USB_BBLEERRMSK_S)
#define USB_BBLEERRMSK_V  0x00000001
#define USB_BBLEERRMSK_S  12
/** USB_NAKMSK : R/W; bitpos: [13]; default: 0;
 *  0x0 : Mask NAK Interrupt
 *  0x1 : No NAK Interrupt Mask
 */
#define USB_NAKMSK    (BIT(13))
#define USB_NAKMSK_M  (USB_NAKMSK_V << USB_NAKMSK_S)
#define USB_NAKMSK_V  0x00000001
#define USB_NAKMSK_S  13
/** USB_NYETMSK : R/W; bitpos: [14]; default: 0;
 *  NYET interrupt Mask
 *  0x0 : Mask NYET Interrupt
 *  0x1 : No NYET Interrupt Mask
 */
#define USB_NYETMSK    (BIT(14))
#define USB_NYETMSK_M  (USB_NYETMSK_V << USB_NYETMSK_S)
#define USB_NYETMSK_V  0x00000001
#define USB_NYETMSK_S  14


/** USB_DAINT_REG register
 *  Device All Endpoints Interrupt Register
 */
#define USB_DAINT_REG (SOC_DPORT_USB_BASE + 0x818)
/** USB_INEPINT0 : RO; bitpos: [0]; default: 0;
 *  IN Endpoint 0 Interrupt Bit.
 */
#define USB_INEPINT0    (BIT(0))
#define USB_INEPINT0_M  (USB_INEPINT0_V << USB_INEPINT0_S)
#define USB_INEPINT0_V  0x00000001
#define USB_INEPINT0_S  0
/** USB_INEPINT1 : RO; bitpos: [1]; default: 0;
 *  IN Endpoint 1 Interrupt Bit.
 */
#define USB_INEPINT1    (BIT(1))
#define USB_INEPINT1_M  (USB_INEPINT1_V << USB_INEPINT1_S)
#define USB_INEPINT1_V  0x00000001
#define USB_INEPINT1_S  1
/** USB_INEPINT2 : RO; bitpos: [2]; default: 0;
 *  IN Endpoint 2 Interrupt Bit.
 */
#define USB_INEPINT2    (BIT(2))
#define USB_INEPINT2_M  (USB_INEPINT2_V << USB_INEPINT2_S)
#define USB_INEPINT2_V  0x00000001
#define USB_INEPINT2_S  2
/** USB_INEPINT3 : RO; bitpos: [3]; default: 0;
 *  IN Endpoint 3 Interrupt Bit.
 */
#define USB_INEPINT3    (BIT(3))
#define USB_INEPINT3_M  (USB_INEPINT3_V << USB_INEPINT3_S)
#define USB_INEPINT3_V  0x00000001
#define USB_INEPINT3_S  3
/** USB_INEPINT4 : RO; bitpos: [4]; default: 0;
 *  IN Endpoint 4 Interrupt Bit.
 */
#define USB_INEPINT4    (BIT(4))
#define USB_INEPINT4_M  (USB_INEPINT4_V << USB_INEPINT4_S)
#define USB_INEPINT4_V  0x00000001
#define USB_INEPINT4_S  4
/** USB_INEPINT5 : RO; bitpos: [5]; default: 0;
 *  IN Endpoint 5 Interrupt Bit.
 */
#define USB_INEPINT5    (BIT(5))
#define USB_INEPINT5_M  (USB_INEPINT5_V << USB_INEPINT5_S)
#define USB_INEPINT5_V  0x00000001
#define USB_INEPINT5_S  5
/** USB_INEPINT6 : RO; bitpos: [6]; default: 0;
 *  IN Endpoint 6 Interrupt Bit.
 */
#define USB_INEPINT6    (BIT(6))
#define USB_INEPINT6_M  (USB_INEPINT6_V << USB_INEPINT6_S)
#define USB_INEPINT6_V  0x00000001
#define USB_INEPINT6_S  6
/** USB_OUTEPINT0 : RO; bitpos: [16]; default: 0;
 *  OUT Endpoint 0 Interrupt Bit.
 */
#define USB_OUTEPINT0    (BIT(16))
#define USB_OUTEPINT0_M  (USB_OUTEPINT0_V << USB_OUTEPINT0_S)
#define USB_OUTEPINT0_V  0x00000001
#define USB_OUTEPINT0_S  16
/** USB_OUTEPINT1 : RO; bitpos: [17]; default: 0;
 *  OUT Endpoint 1 Interrupt Bit.
 */
#define USB_OUTEPINT1    (BIT(17))
#define USB_OUTEPINT1_M  (USB_OUTEPINT1_V << USB_OUTEPINT1_S)
#define USB_OUTEPINT1_V  0x00000001
#define USB_OUTEPINT1_S  17
/** USB_OUTEPINT2 : RO; bitpos: [18]; default: 0;
 *  OUT Endpoint 2 Interrupt Bit.
 */
#define USB_OUTEPINT2    (BIT(18))
#define USB_OUTEPINT2_M  (USB_OUTEPINT2_V << USB_OUTEPINT2_S)
#define USB_OUTEPINT2_V  0x00000001
#define USB_OUTEPINT2_S  18
/** USB_OUTEPINT3 : RO; bitpos: [19]; default: 0;
 *  OUT Endpoint 3 Interrupt Bit.
 */
#define USB_OUTEPINT3    (BIT(19))
#define USB_OUTEPINT3_M  (USB_OUTEPINT3_V << USB_OUTEPINT3_S)
#define USB_OUTEPINT3_V  0x00000001
#define USB_OUTEPINT3_S  19
/** USB_OUTEPINT4 : RO; bitpos: [20]; default: 0;
 *  OUT Endpoint 4 Interrupt Bit.
 */
#define USB_OUTEPINT4    (BIT(20))
#define USB_OUTEPINT4_M  (USB_OUTEPINT4_V << USB_OUTEPINT4_S)
#define USB_OUTEPINT4_V  0x00000001
#define USB_OUTEPINT4_S  20
/** USB_OUTEPINT5 : RO; bitpos: [21]; default: 0;
 *  OUT Endpoint 5 Interrupt Bit.
 */
#define USB_OUTEPINT5    (BIT(21))
#define USB_OUTEPINT5_M  (USB_OUTEPINT5_V << USB_OUTEPINT5_S)
#define USB_OUTEPINT5_V  0x00000001
#define USB_OUTEPINT5_S  21
/** USB_OUTEPINT6 : RO; bitpos: [22]; default: 0;
 *  OUT Endpoint 6 Interrupt Bit.
 */
#define USB_OUTEPINT6    (BIT(22))
#define USB_OUTEPINT6_M  (USB_OUTEPINT6_V << USB_OUTEPINT6_S)
#define USB_OUTEPINT6_V  0x00000001
#define USB_OUTEPINT6_S  22


/** USB_DAINTMSK_REG register
 *  Device All Endpoints Interrupt Mask Register
 */
#define USB_DAINTMSK_REG (SOC_DPORT_USB_BASE + 0x81c)
/** USB_INEPMSK0 : R/W; bitpos: [0]; default: 0;
 *  IN Endpoint 0 Interrupt mask Bit.
 */
#define USB_INEPMSK0    (BIT(0))
#define USB_INEPMSK0_M  (USB_INEPMSK0_V << USB_INEPMSK0_S)
#define USB_INEPMSK0_V  0x00000001
#define USB_INEPMSK0_S  0
/** USB_INEPMSK1 : R/W; bitpos: [1]; default: 0;
 *  IN Endpoint 1 Interrupt mask Bit.
 */
#define USB_INEPMSK1    (BIT(1))
#define USB_INEPMSK1_M  (USB_INEPMSK1_V << USB_INEPMSK1_S)
#define USB_INEPMSK1_V  0x00000001
#define USB_INEPMSK1_S  1
/** USB_INEPMSK2 : R/W; bitpos: [2]; default: 0;
 *  IN Endpoint 2 Interrupt mask Bit.
 */
#define USB_INEPMSK2    (BIT(2))
#define USB_INEPMSK2_M  (USB_INEPMSK2_V << USB_INEPMSK2_S)
#define USB_INEPMSK2_V  0x00000001
#define USB_INEPMSK2_S  2
/** USB_INEPMSK3 : R/W; bitpos: [3]; default: 0;
 *  IN Endpoint 3 Interrupt mask Bit.
 */
#define USB_INEPMSK3    (BIT(3))
#define USB_INEPMSK3_M  (USB_INEPMSK3_V << USB_INEPMSK3_S)
#define USB_INEPMSK3_V  0x00000001
#define USB_INEPMSK3_S  3
/** USB_INEPMSK4 : R/W; bitpos: [4]; default: 0;
 *  IN Endpoint 4 Interrupt mask Bit.
 */
#define USB_INEPMSK4    (BIT(4))
#define USB_INEPMSK4_M  (USB_INEPMSK4_V << USB_INEPMSK4_S)
#define USB_INEPMSK4_V  0x00000001
#define USB_INEPMSK4_S  4
/** USB_INEPMSK5 : R/W; bitpos: [5]; default: 0;
 *  IN Endpoint 5 Interrupt mask Bit.
 */
#define USB_INEPMSK5    (BIT(5))
#define USB_INEPMSK5_M  (USB_INEPMSK5_V << USB_INEPMSK5_S)
#define USB_INEPMSK5_V  0x00000001
#define USB_INEPMSK5_S  5
/** USB_INEPMSK6 : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint 6 Interrupt mask Bit.
 */
#define USB_INEPMSK6    (BIT(6))
#define USB_INEPMSK6_M  (USB_INEPMSK6_V << USB_INEPMSK6_S)
#define USB_INEPMSK6_V  0x00000001
#define USB_INEPMSK6_S  6
/** USB_OUTEPMSK0 : R/W; bitpos: [16]; default: 0;
 *  OUT Endpoint 0 Interrupt mask Bit.
 */
#define USB_OUTEPMSK0    (BIT(16))
#define USB_OUTEPMSK0_M  (USB_OUTEPMSK0_V << USB_OUTEPMSK0_S)
#define USB_OUTEPMSK0_V  0x00000001
#define USB_OUTEPMSK0_S  16
/** USB_OUTEPMSK1 : R/W; bitpos: [17]; default: 0;
 *  OUT Endpoint 1 Interrupt mask Bit.
 */
#define USB_OUTEPMSK1    (BIT(17))
#define USB_OUTEPMSK1_M  (USB_OUTEPMSK1_V << USB_OUTEPMSK1_S)
#define USB_OUTEPMSK1_V  0x00000001
#define USB_OUTEPMSK1_S  17
/** USB_OUTEPMSK2 : R/W; bitpos: [18]; default: 0;
 *  OUT Endpoint 2 Interrupt mask Bit.
 */
#define USB_OUTEPMSK2    (BIT(18))
#define USB_OUTEPMSK2_M  (USB_OUTEPMSK2_V << USB_OUTEPMSK2_S)
#define USB_OUTEPMSK2_V  0x00000001
#define USB_OUTEPMSK2_S  18
/** USB_OUTEPMSK3 : R/W; bitpos: [19]; default: 0;
 *  OUT Endpoint 3 Interrupt mask Bit.
 */
#define USB_OUTEPMSK3    (BIT(19))
#define USB_OUTEPMSK3_M  (USB_OUTEPMSK3_V << USB_OUTEPMSK3_S)
#define USB_OUTEPMSK3_V  0x00000001
#define USB_OUTEPMSK3_S  19
/** USB_OUTEPMSK4 : R/W; bitpos: [20]; default: 0;
 *  OUT Endpoint 4 Interrupt mask Bit.
 */
#define USB_OUTEPMSK4    (BIT(20))
#define USB_OUTEPMSK4_M  (USB_OUTEPMSK4_V << USB_OUTEPMSK4_S)
#define USB_OUTEPMSK4_V  0x00000001
#define USB_OUTEPMSK4_S  20
/** USB_OUTEPMSK5 : R/W; bitpos: [21]; default: 0;
 *  OUT Endpoint 5 Interrupt mask Bit.
 */
#define USB_OUTEPMSK5    (BIT(21))
#define USB_OUTEPMSK5_M  (USB_OUTEPMSK5_V << USB_OUTEPMSK5_S)
#define USB_OUTEPMSK5_V  0x00000001
#define USB_OUTEPMSK5_S  21
/** USB_OUTEPMSK6 : R/W; bitpos: [22]; default: 0;
 *  OUT Endpoint 6 Interrupt mask Bit.
 */
#define USB_OUTEPMSK6    (BIT(22))
#define USB_OUTEPMSK6_M  (USB_OUTEPMSK6_V << USB_OUTEPMSK6_S)
#define USB_OUTEPMSK6_V  0x00000001
#define USB_OUTEPMSK6_S  22


/** USB_DIEPEMPMSK_REG register
 *  Device IN Endpoint FIFO Empty Interrupt Mask Register
 */
#define USB_DIEPEMPMSK_REG (SOC_DPORT_USB_BASE + 0x834)
/** USB_D_INEPTXFEMPMSK : R/W; bitpos: [16:0]; default: 0;
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
#define USB_D_INEPTXFEMPMSK    0x0000FFFF
#define USB_D_INEPTXFEMPMSK_M  (USB_D_INEPTXFEMPMSK_V << USB_D_INEPTXFEMPMSK_S)
#define USB_D_INEPTXFEMPMSK_V  0x0000FFFF
#define USB_D_INEPTXFEMPMSK_S  0


/** USB_DIEPINT0_REG register
 *  Device IN Endpoint 0 Interrupt Register
 */
#define USB_DIEPINT0_REG (SOC_DPORT_USB_BASE + 0x908)
/** USB_D_XFERCOMPL0 : R/W1C; bitpos: [0]; default: 0;
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Completed Interrupt
 */
#define USB_D_XFERCOMPL0    (BIT(0))
#define USB_D_XFERCOMPL0_M  (USB_D_XFERCOMPL0_V << USB_D_XFERCOMPL0_S)
#define USB_D_XFERCOMPL0_V  0x00000001
#define USB_D_XFERCOMPL0_S  0
/** USB_D_EPDISBLD0 : R/W1C; bitpos: [1]; default: 0;
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_D_EPDISBLD0    (BIT(1))
#define USB_D_EPDISBLD0_M  (USB_D_EPDISBLD0_V << USB_D_EPDISBLD0_S)
#define USB_D_EPDISBLD0_V  0x00000001
#define USB_D_EPDISBLD0_S  1
/** USB_D_AHBERR0 : R/W1C; bitpos: [2]; default: 0;
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_D_AHBERR0    (BIT(2))
#define USB_D_AHBERR0_M  (USB_D_AHBERR0_V << USB_D_AHBERR0_S)
#define USB_D_AHBERR0_V  0x00000001
#define USB_D_AHBERR0_S  2
/** USB_D_TIMEOUT0 : R/W1C; bitpos: [3]; default: 0;
 *  0x0 : No Timeout interrupt
 *  0x1 : Timeout interrupt
 */
#define USB_D_TIMEOUT0    (BIT(3))
#define USB_D_TIMEOUT0_M  (USB_D_TIMEOUT0_V << USB_D_TIMEOUT0_S)
#define USB_D_TIMEOUT0_V  0x00000001
#define USB_D_TIMEOUT0_S  3
/** USB_D_INTKNTXFEMP0 : R/W1C; bitpos: [4]; default: 0;
 *  0x0 : No IN Token Received when TxFIFO Empty Interrupt
 *  0x1 : IN Token Received when TxFIFO Empty Interrupt
 */
#define USB_D_INTKNTXFEMP0    (BIT(4))
#define USB_D_INTKNTXFEMP0_M  (USB_D_INTKNTXFEMP0_V << USB_D_INTKNTXFEMP0_S)
#define USB_D_INTKNTXFEMP0_V  0x00000001
#define USB_D_INTKNTXFEMP0_S  4
/** USB_D_INTKNEPMIS0 : R/W1C; bitpos: [5]; default: 0;
 *  0x0 : No IN Token Received with EP Mismatch Interrupt
 *  0x1 : IN Token Received with EP Mismatch interrupt
 */
#define USB_D_INTKNEPMIS0    (BIT(5))
#define USB_D_INTKNEPMIS0_M  (USB_D_INTKNEPMIS0_V << USB_D_INTKNEPMIS0_S)
#define USB_D_INTKNEPMIS0_V  0x00000001
#define USB_D_INTKNEPMIS0_S  5
/** USB_D_INEPNAKEFF0 : R/W1C; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective
 *  0x0 : No IN Endpoint NAK Effective interrupt
 *  0x1 : IN Endpoint NAK Effective interrupt
 */
#define USB_D_INEPNAKEFF0    (BIT(6))
#define USB_D_INEPNAKEFF0_M  (USB_D_INEPNAKEFF0_V << USB_D_INEPNAKEFF0_S)
#define USB_D_INEPNAKEFF0_V  0x00000001
#define USB_D_INEPNAKEFF0_S  6
/** USB_D_TXFEMP0 : RO; bitpos: [7]; default: 0;
 *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
 *  completely empty
 *  0x0 : No Transmit FIFO Empty interrupt
 *  0x1 : Transmit FIFO Empty interrupt
 */
#define USB_D_TXFEMP0    (BIT(7))
#define USB_D_TXFEMP0_M  (USB_D_TXFEMP0_V << USB_D_TXFEMP0_S)
#define USB_D_TXFEMP0_V  0x00000001
#define USB_D_TXFEMP0_S  7
/** USB_D_TXFIFOUNDRN0 : R/W1C; bitpos: [8]; default: 0;
 *  The core generates this interrupt when it detects a transmit FIFO underrun
 *  condition in threshold mode for this endpoint
 *  0x0 : No Fifo Underrun interrupt
 *  0x1 : Fifo Underrun interrupt
 */
#define USB_D_TXFIFOUNDRN0    (BIT(8))
#define USB_D_TXFIFOUNDRN0_M  (USB_D_TXFIFOUNDRN0_V << USB_D_TXFIFOUNDRN0_S)
#define USB_D_TXFIFOUNDRN0_V  0x00000001
#define USB_D_TXFIFOUNDRN0_S  8
/** USB_D_BNAINTR0 : R/W1C; bitpos: [9]; default: 0;
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
 *  this interrupt when the descriptor accessed is not ready for the Core to process,
 *  such as Host busy or DMA done
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_D_BNAINTR0    (BIT(9))
#define USB_D_BNAINTR0_M  (USB_D_BNAINTR0_V << USB_D_BNAINTR0_S)
#define USB_D_BNAINTR0_V  0x00000001
#define USB_D_BNAINTR0_S  9
/** USB_D_PKTDRPSTS0 : R/W1C; bitpos: [11]; default: 0;
 *  Packet Drop Status. This bit indicates to the application that an ISOC OUT packet
 *  has been dropped. This bit does not have an associated mask bit and does not
 *  generate an interrupt
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status
 */
#define USB_D_PKTDRPSTS0    (BIT(11))
#define USB_D_PKTDRPSTS0_M  (USB_D_PKTDRPSTS0_V << USB_D_PKTDRPSTS0_S)
#define USB_D_PKTDRPSTS0_V  0x00000001
#define USB_D_PKTDRPSTS0_S  11
/** USB_D_BBLEERR0 : R/W1C; bitpos: [12]; default: 0;
 *  The core generates this interrupt when babble is received for the endpoint
 *  0x0 : No interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_D_BBLEERR0    (BIT(12))
#define USB_D_BBLEERR0_M  (USB_D_BBLEERR0_V << USB_D_BBLEERR0_S)
#define USB_D_BBLEERR0_V  0x00000001
#define USB_D_BBLEERR0_S  12
/** USB_D_NAKINTRPT0 : R/W1C; bitpos: [13]; default: 0;
 *  NAK Interrupt. The core generates this interrupt when a NAK is transmitted or
 *  received by the device
 *  0x0 : No interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_D_NAKINTRPT0    (BIT(13))
#define USB_D_NAKINTRPT0_M  (USB_D_NAKINTRPT0_V << USB_D_NAKINTRPT0_S)
#define USB_D_NAKINTRPT0_V  0x00000001
#define USB_D_NAKINTRPT0_S  13
/** USB_D_NYETINTRPT0 : R/W1C; bitpos: [14]; default: 0;
 *  NYET Interrupt. The core generates this interrupt when a NYET response is
 *  transmitted for a non isochronous OUT endpoint
 *  0x0 : No interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_D_NYETINTRPT0    (BIT(14))
#define USB_D_NYETINTRPT0_M  (USB_D_NYETINTRPT0_V << USB_D_NYETINTRPT0_S)
#define USB_D_NYETINTRPT0_V  0x00000001
#define USB_D_NYETINTRPT0_S  14


/** USB_DIEPINT1_REG register
 *  Device IN Endpoint 1 Interrupt Register
 */
#define USB_DIEPINT1_REG (SOC_DPORT_USB_BASE + 0x928)
/** USB_D_XFERCOMPL1 : R/W1C; bitpos: [0]; default: 0;
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Completed Interrupt
 */
#define USB_D_XFERCOMPL1    (BIT(0))
#define USB_D_XFERCOMPL1_M  (USB_D_XFERCOMPL1_V << USB_D_XFERCOMPL1_S)
#define USB_D_XFERCOMPL1_V  0x00000001
#define USB_D_XFERCOMPL1_S  0
/** USB_D_EPDISBLD1 : R/W1C; bitpos: [1]; default: 0;
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_D_EPDISBLD1    (BIT(1))
#define USB_D_EPDISBLD1_M  (USB_D_EPDISBLD1_V << USB_D_EPDISBLD1_S)
#define USB_D_EPDISBLD1_V  0x00000001
#define USB_D_EPDISBLD1_S  1
/** USB_D_AHBERR1 : R/W1C; bitpos: [2]; default: 0;
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_D_AHBERR1    (BIT(2))
#define USB_D_AHBERR1_M  (USB_D_AHBERR1_V << USB_D_AHBERR1_S)
#define USB_D_AHBERR1_V  0x00000001
#define USB_D_AHBERR1_S  2
/** USB_D_TIMEOUT1 : R/W1C; bitpos: [3]; default: 0;
 *  0x0 : No Timeout interrupt
 *  0x1 : Timeout interrupt
 */
#define USB_D_TIMEOUT1    (BIT(3))
#define USB_D_TIMEOUT1_M  (USB_D_TIMEOUT1_V << USB_D_TIMEOUT1_S)
#define USB_D_TIMEOUT1_V  0x00000001
#define USB_D_TIMEOUT1_S  3
/** USB_D_INTKNTXFEMP1 : R/W1C; bitpos: [4]; default: 0;
 *  0x0 : No IN Token Received when TxFIFO Empty Interrupt
 *  0x1 : IN Token Received when TxFIFO Empty Interrupt
 */
#define USB_D_INTKNTXFEMP1    (BIT(4))
#define USB_D_INTKNTXFEMP1_M  (USB_D_INTKNTXFEMP1_V << USB_D_INTKNTXFEMP1_S)
#define USB_D_INTKNTXFEMP1_V  0x00000001
#define USB_D_INTKNTXFEMP1_S  4
/** USB_D_INTKNEPMIS1 : R/W1C; bitpos: [5]; default: 0;
 *  0x0 : No IN Token Received with EP Mismatch Interrupt
 *  0x1 : IN Token Received with EP Mismatch interrupt
 */
#define USB_D_INTKNEPMIS1    (BIT(5))
#define USB_D_INTKNEPMIS1_M  (USB_D_INTKNEPMIS1_V << USB_D_INTKNEPMIS1_S)
#define USB_D_INTKNEPMIS1_V  0x00000001
#define USB_D_INTKNEPMIS1_S  5
/** USB_D_INEPNAKEFF1 : R/W1C; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective
 *  0x0 : No IN Endpoint NAK Effective interrupt
 *  0x1 : IN Endpoint NAK Effective interrupt
 */
#define USB_D_INEPNAKEFF1    (BIT(6))
#define USB_D_INEPNAKEFF1_M  (USB_D_INEPNAKEFF1_V << USB_D_INEPNAKEFF1_S)
#define USB_D_INEPNAKEFF1_V  0x00000001
#define USB_D_INEPNAKEFF1_S  6
/** USB_D_TXFEMP1 : RO; bitpos: [7]; default: 0;
 *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
 *  completely empty
 *  0x0 : No Transmit FIFO Empty interrupt
 *  0x1 : Transmit FIFO Empty interrupt
 */
#define USB_D_TXFEMP1    (BIT(7))
#define USB_D_TXFEMP1_M  (USB_D_TXFEMP1_V << USB_D_TXFEMP1_S)
#define USB_D_TXFEMP1_V  0x00000001
#define USB_D_TXFEMP1_S  7
/** USB_D_TXFIFOUNDRN1 : R/W1C; bitpos: [8]; default: 0;
 *  The core generates this interrupt when it detects a transmit FIFO underrun
 *  condition in threshold mode for this endpoint
 *  0x0 : No Fifo Underrun interrupt
 *  0x1 : Fifo Underrun interrupt
 */
#define USB_D_TXFIFOUNDRN1    (BIT(8))
#define USB_D_TXFIFOUNDRN1_M  (USB_D_TXFIFOUNDRN1_V << USB_D_TXFIFOUNDRN1_S)
#define USB_D_TXFIFOUNDRN1_V  0x00000001
#define USB_D_TXFIFOUNDRN1_S  8
/** USB_D_BNAINTR1 : R/W1C; bitpos: [9]; default: 0;
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
 *  this interrupt when the descriptor accessed is not ready for the Core to process,
 *  such as Host busy or DMA done
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_D_BNAINTR1    (BIT(9))
#define USB_D_BNAINTR1_M  (USB_D_BNAINTR1_V << USB_D_BNAINTR1_S)
#define USB_D_BNAINTR1_V  0x00000001
#define USB_D_BNAINTR1_S  9
/** USB_D_PKTDRPSTS1 : R/W1C; bitpos: [11]; default: 0;
 *  Packet Drop Status. This bit indicates to the application that an ISOC OUT packet
 *  has been dropped. This bit does not have an associated mask bit and does not
 *  generate an interrupt
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status
 */
#define USB_D_PKTDRPSTS1    (BIT(11))
#define USB_D_PKTDRPSTS1_M  (USB_D_PKTDRPSTS1_V << USB_D_PKTDRPSTS1_S)
#define USB_D_PKTDRPSTS1_V  0x00000001
#define USB_D_PKTDRPSTS1_S  11
/** USB_D_BBLEERR1 : R/W1C; bitpos: [12]; default: 0;
 *  The core generates this interrupt when babble is received for the endpoint
 *  0x0 : No interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_D_BBLEERR1    (BIT(12))
#define USB_D_BBLEERR1_M  (USB_D_BBLEERR1_V << USB_D_BBLEERR1_S)
#define USB_D_BBLEERR1_V  0x00000001
#define USB_D_BBLEERR1_S  12
/** USB_D_NAKINTRPT1 : R/W1C; bitpos: [13]; default: 0;
 *  NAK Interrupt. The core generates this interrupt when a NAK is transmitted or
 *  received by the device
 *  0x0 : No interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_D_NAKINTRPT1    (BIT(13))
#define USB_D_NAKINTRPT1_M  (USB_D_NAKINTRPT1_V << USB_D_NAKINTRPT1_S)
#define USB_D_NAKINTRPT1_V  0x00000001
#define USB_D_NAKINTRPT1_S  13
/** USB_D_NYETINTRPT1 : R/W1C; bitpos: [14]; default: 0;
 *  NYET Interrupt. The core generates this interrupt when a NYET response is
 *  transmitted for a non isochronous OUT endpoint
 *  0x0 : No interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_D_NYETINTRPT1    (BIT(14))
#define USB_D_NYETINTRPT1_M  (USB_D_NYETINTRPT1_V << USB_D_NYETINTRPT1_S)
#define USB_D_NYETINTRPT1_V  0x00000001
#define USB_D_NYETINTRPT1_S  14


/** USB_DIEPINT2_REG register
 *  Device IN Endpoint 2 Interrupt Register
 */
#define USB_DIEPINT2_REG (SOC_DPORT_USB_BASE + 0x948)
/** USB_D_XFERCOMPL2 : R/W1C; bitpos: [0]; default: 0;
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Completed Interrupt
 */
#define USB_D_XFERCOMPL2    (BIT(0))
#define USB_D_XFERCOMPL2_M  (USB_D_XFERCOMPL2_V << USB_D_XFERCOMPL2_S)
#define USB_D_XFERCOMPL2_V  0x00000001
#define USB_D_XFERCOMPL2_S  0
/** USB_D_EPDISBLD2 : R/W1C; bitpos: [1]; default: 0;
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_D_EPDISBLD2    (BIT(1))
#define USB_D_EPDISBLD2_M  (USB_D_EPDISBLD2_V << USB_D_EPDISBLD2_S)
#define USB_D_EPDISBLD2_V  0x00000001
#define USB_D_EPDISBLD2_S  1
/** USB_D_AHBERR2 : R/W1C; bitpos: [2]; default: 0;
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_D_AHBERR2    (BIT(2))
#define USB_D_AHBERR2_M  (USB_D_AHBERR2_V << USB_D_AHBERR2_S)
#define USB_D_AHBERR2_V  0x00000001
#define USB_D_AHBERR2_S  2
/** USB_D_TIMEOUT2 : R/W1C; bitpos: [3]; default: 0;
 *  0x0 : No Timeout interrupt
 *  0x1 : Timeout interrupt
 */
#define USB_D_TIMEOUT2    (BIT(3))
#define USB_D_TIMEOUT2_M  (USB_D_TIMEOUT2_V << USB_D_TIMEOUT2_S)
#define USB_D_TIMEOUT2_V  0x00000001
#define USB_D_TIMEOUT2_S  3
/** USB_D_INTKNTXFEMP2 : R/W1C; bitpos: [4]; default: 0;
 *  0x0 : No IN Token Received when TxFIFO Empty Interrupt
 *  0x1 : IN Token Received when TxFIFO Empty Interrupt
 */
#define USB_D_INTKNTXFEMP2    (BIT(4))
#define USB_D_INTKNTXFEMP2_M  (USB_D_INTKNTXFEMP2_V << USB_D_INTKNTXFEMP2_S)
#define USB_D_INTKNTXFEMP2_V  0x00000001
#define USB_D_INTKNTXFEMP2_S  4
/** USB_D_INTKNEPMIS2 : R/W1C; bitpos: [5]; default: 0;
 *  0x0 : No IN Token Received with EP Mismatch Interrupt
 *  0x1 : IN Token Received with EP Mismatch interrupt
 */
#define USB_D_INTKNEPMIS2    (BIT(5))
#define USB_D_INTKNEPMIS2_M  (USB_D_INTKNEPMIS2_V << USB_D_INTKNEPMIS2_S)
#define USB_D_INTKNEPMIS2_V  0x00000001
#define USB_D_INTKNEPMIS2_S  5
/** USB_D_INEPNAKEFF2 : R/W1C; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective
 *  0x0 : No IN Endpoint NAK Effective interrupt
 *  0x1 : IN Endpoint NAK Effective interrupt
 */
#define USB_D_INEPNAKEFF2    (BIT(6))
#define USB_D_INEPNAKEFF2_M  (USB_D_INEPNAKEFF2_V << USB_D_INEPNAKEFF2_S)
#define USB_D_INEPNAKEFF2_V  0x00000001
#define USB_D_INEPNAKEFF2_S  6
/** USB_D_TXFEMP2 : RO; bitpos: [7]; default: 0;
 *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
 *  completely empty
 *  0x0 : No Transmit FIFO Empty interrupt
 *  0x1 : Transmit FIFO Empty interrupt
 */
#define USB_D_TXFEMP2    (BIT(7))
#define USB_D_TXFEMP2_M  (USB_D_TXFEMP2_V << USB_D_TXFEMP2_S)
#define USB_D_TXFEMP2_V  0x00000001
#define USB_D_TXFEMP2_S  7
/** USB_D_TXFIFOUNDRN2 : R/W1C; bitpos: [8]; default: 0;
 *  The core generates this interrupt when it detects a transmit FIFO underrun
 *  condition in threshold mode for this endpoint
 *  0x0 : No Fifo Underrun interrupt
 *  0x1 : Fifo Underrun interrupt
 */
#define USB_D_TXFIFOUNDRN2    (BIT(8))
#define USB_D_TXFIFOUNDRN2_M  (USB_D_TXFIFOUNDRN2_V << USB_D_TXFIFOUNDRN2_S)
#define USB_D_TXFIFOUNDRN2_V  0x00000001
#define USB_D_TXFIFOUNDRN2_S  8
/** USB_D_BNAINTR2 : R/W1C; bitpos: [9]; default: 0;
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
 *  this interrupt when the descriptor accessed is not ready for the Core to process,
 *  such as Host busy or DMA done
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_D_BNAINTR2    (BIT(9))
#define USB_D_BNAINTR2_M  (USB_D_BNAINTR2_V << USB_D_BNAINTR2_S)
#define USB_D_BNAINTR2_V  0x00000001
#define USB_D_BNAINTR2_S  9
/** USB_D_PKTDRPSTS2 : R/W1C; bitpos: [11]; default: 0;
 *  Packet Drop Status. This bit indicates to the application that an ISOC OUT packet
 *  has been dropped. This bit does not have an associated mask bit and does not
 *  generate an interrupt
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status
 */
#define USB_D_PKTDRPSTS2    (BIT(11))
#define USB_D_PKTDRPSTS2_M  (USB_D_PKTDRPSTS2_V << USB_D_PKTDRPSTS2_S)
#define USB_D_PKTDRPSTS2_V  0x00000001
#define USB_D_PKTDRPSTS2_S  11
/** USB_D_BBLEERR2 : R/W1C; bitpos: [12]; default: 0;
 *  The core generates this interrupt when babble is received for the endpoint
 *  0x0 : No interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_D_BBLEERR2    (BIT(12))
#define USB_D_BBLEERR2_M  (USB_D_BBLEERR2_V << USB_D_BBLEERR2_S)
#define USB_D_BBLEERR2_V  0x00000001
#define USB_D_BBLEERR2_S  12
/** USB_D_NAKINTRPT2 : R/W1C; bitpos: [13]; default: 0;
 *  NAK Interrupt. The core generates this interrupt when a NAK is transmitted or
 *  received by the device
 *  0x0 : No interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_D_NAKINTRPT2    (BIT(13))
#define USB_D_NAKINTRPT2_M  (USB_D_NAKINTRPT2_V << USB_D_NAKINTRPT2_S)
#define USB_D_NAKINTRPT2_V  0x00000001
#define USB_D_NAKINTRPT2_S  13
/** USB_D_NYETINTRPT2 : R/W1C; bitpos: [14]; default: 0;
 *  NYET Interrupt. The core generates this interrupt when a NYET response is
 *  transmitted for a non isochronous OUT endpoint
 *  0x0 : No interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_D_NYETINTRPT2    (BIT(14))
#define USB_D_NYETINTRPT2_M  (USB_D_NYETINTRPT2_V << USB_D_NYETINTRPT2_S)
#define USB_D_NYETINTRPT2_V  0x00000001
#define USB_D_NYETINTRPT2_S  14


/** USB_DIEPINT3_REG register
 *  Device IN Endpoint 3 Interrupt Register
 */
#define USB_DIEPINT3_REG (SOC_DPORT_USB_BASE + 0x968)
/** USB_D_XFERCOMPL3 : R/W1C; bitpos: [0]; default: 0;
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Completed Interrupt
 */
#define USB_D_XFERCOMPL3    (BIT(0))
#define USB_D_XFERCOMPL3_M  (USB_D_XFERCOMPL3_V << USB_D_XFERCOMPL3_S)
#define USB_D_XFERCOMPL3_V  0x00000001
#define USB_D_XFERCOMPL3_S  0
/** USB_D_EPDISBLD3 : R/W1C; bitpos: [1]; default: 0;
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_D_EPDISBLD3    (BIT(1))
#define USB_D_EPDISBLD3_M  (USB_D_EPDISBLD3_V << USB_D_EPDISBLD3_S)
#define USB_D_EPDISBLD3_V  0x00000001
#define USB_D_EPDISBLD3_S  1
/** USB_D_AHBERR3 : R/W1C; bitpos: [2]; default: 0;
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_D_AHBERR3    (BIT(2))
#define USB_D_AHBERR3_M  (USB_D_AHBERR3_V << USB_D_AHBERR3_S)
#define USB_D_AHBERR3_V  0x00000001
#define USB_D_AHBERR3_S  2
/** USB_D_TIMEOUT3 : R/W1C; bitpos: [3]; default: 0;
 *  0x0 : No Timeout interrupt
 *  0x1 : Timeout interrupt
 */
#define USB_D_TIMEOUT3    (BIT(3))
#define USB_D_TIMEOUT3_M  (USB_D_TIMEOUT3_V << USB_D_TIMEOUT3_S)
#define USB_D_TIMEOUT3_V  0x00000001
#define USB_D_TIMEOUT3_S  3
/** USB_D_INTKNTXFEMP3 : R/W1C; bitpos: [4]; default: 0;
 *  0x0 : No IN Token Received when TxFIFO Empty Interrupt
 *  0x1 : IN Token Received when TxFIFO Empty Interrupt
 */
#define USB_D_INTKNTXFEMP3    (BIT(4))
#define USB_D_INTKNTXFEMP3_M  (USB_D_INTKNTXFEMP3_V << USB_D_INTKNTXFEMP3_S)
#define USB_D_INTKNTXFEMP3_V  0x00000001
#define USB_D_INTKNTXFEMP3_S  4
/** USB_D_INTKNEPMIS3 : R/W1C; bitpos: [5]; default: 0;
 *  0x0 : No IN Token Received with EP Mismatch Interrupt
 *  0x1 : IN Token Received with EP Mismatch interrupt
 */
#define USB_D_INTKNEPMIS3    (BIT(5))
#define USB_D_INTKNEPMIS3_M  (USB_D_INTKNEPMIS3_V << USB_D_INTKNEPMIS3_S)
#define USB_D_INTKNEPMIS3_V  0x00000001
#define USB_D_INTKNEPMIS3_S  5
/** USB_D_INEPNAKEFF3 : R/W1C; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective
 *  0x0 : No IN Endpoint NAK Effective interrupt
 *  0x1 : IN Endpoint NAK Effective interrupt
 */
#define USB_D_INEPNAKEFF3    (BIT(6))
#define USB_D_INEPNAKEFF3_M  (USB_D_INEPNAKEFF3_V << USB_D_INEPNAKEFF3_S)
#define USB_D_INEPNAKEFF3_V  0x00000001
#define USB_D_INEPNAKEFF3_S  6
/** USB_D_TXFEMP3 : RO; bitpos: [7]; default: 0;
 *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
 *  completely empty
 *  0x0 : No Transmit FIFO Empty interrupt
 *  0x1 : Transmit FIFO Empty interrupt
 */
#define USB_D_TXFEMP3    (BIT(7))
#define USB_D_TXFEMP3_M  (USB_D_TXFEMP3_V << USB_D_TXFEMP3_S)
#define USB_D_TXFEMP3_V  0x00000001
#define USB_D_TXFEMP3_S  7
/** USB_D_TXFIFOUNDRN3 : R/W1C; bitpos: [8]; default: 0;
 *  The core generates this interrupt when it detects a transmit FIFO underrun
 *  condition in threshold mode for this endpoint
 *  0x0 : No Fifo Underrun interrupt
 *  0x1 : Fifo Underrun interrupt
 */
#define USB_D_TXFIFOUNDRN3    (BIT(8))
#define USB_D_TXFIFOUNDRN3_M  (USB_D_TXFIFOUNDRN3_V << USB_D_TXFIFOUNDRN3_S)
#define USB_D_TXFIFOUNDRN3_V  0x00000001
#define USB_D_TXFIFOUNDRN3_S  8
/** USB_D_BNAINTR3 : R/W1C; bitpos: [9]; default: 0;
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
 *  this interrupt when the descriptor accessed is not ready for the Core to process,
 *  such as Host busy or DMA done
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_D_BNAINTR3    (BIT(9))
#define USB_D_BNAINTR3_M  (USB_D_BNAINTR3_V << USB_D_BNAINTR3_S)
#define USB_D_BNAINTR3_V  0x00000001
#define USB_D_BNAINTR3_S  9
/** USB_D_PKTDRPSTS3 : R/W1C; bitpos: [11]; default: 0;
 *  Packet Drop Status. This bit indicates to the application that an ISOC OUT packet
 *  has been dropped. This bit does not have an associated mask bit and does not
 *  generate an interrupt
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status
 */
#define USB_D_PKTDRPSTS3    (BIT(11))
#define USB_D_PKTDRPSTS3_M  (USB_D_PKTDRPSTS3_V << USB_D_PKTDRPSTS3_S)
#define USB_D_PKTDRPSTS3_V  0x00000001
#define USB_D_PKTDRPSTS3_S  11
/** USB_D_BBLEERR3 : R/W1C; bitpos: [12]; default: 0;
 *  The core generates this interrupt when babble is received for the endpoint
 *  0x0 : No interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_D_BBLEERR3    (BIT(12))
#define USB_D_BBLEERR3_M  (USB_D_BBLEERR3_V << USB_D_BBLEERR3_S)
#define USB_D_BBLEERR3_V  0x00000001
#define USB_D_BBLEERR3_S  12
/** USB_D_NAKINTRPT3 : R/W1C; bitpos: [13]; default: 0;
 *  NAK Interrupt. The core generates this interrupt when a NAK is transmitted or
 *  received by the device
 *  0x0 : No interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_D_NAKINTRPT3    (BIT(13))
#define USB_D_NAKINTRPT3_M  (USB_D_NAKINTRPT3_V << USB_D_NAKINTRPT3_S)
#define USB_D_NAKINTRPT3_V  0x00000001
#define USB_D_NAKINTRPT3_S  13
/** USB_D_NYETINTRPT3 : R/W1C; bitpos: [14]; default: 0;
 *  NYET Interrupt. The core generates this interrupt when a NYET response is
 *  transmitted for a non isochronous OUT endpoint
 *  0x0 : No interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_D_NYETINTRPT3    (BIT(14))
#define USB_D_NYETINTRPT3_M  (USB_D_NYETINTRPT3_V << USB_D_NYETINTRPT3_S)
#define USB_D_NYETINTRPT3_V  0x00000001
#define USB_D_NYETINTRPT3_S  14


/** USB_DIEPINT4_REG register
 *  Device IN Endpoint 4 Interrupt Register
 */
#define USB_DIEPINT4_REG (SOC_DPORT_USB_BASE + 0x988)
/** USB_D_XFERCOMPL4 : R/W1C; bitpos: [0]; default: 0;
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Completed Interrupt
 */
#define USB_D_XFERCOMPL4    (BIT(0))
#define USB_D_XFERCOMPL4_M  (USB_D_XFERCOMPL4_V << USB_D_XFERCOMPL4_S)
#define USB_D_XFERCOMPL4_V  0x00000001
#define USB_D_XFERCOMPL4_S  0
/** USB_D_EPDISBLD4 : R/W1C; bitpos: [1]; default: 0;
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_D_EPDISBLD4    (BIT(1))
#define USB_D_EPDISBLD4_M  (USB_D_EPDISBLD4_V << USB_D_EPDISBLD4_S)
#define USB_D_EPDISBLD4_V  0x00000001
#define USB_D_EPDISBLD4_S  1
/** USB_D_AHBERR4 : R/W1C; bitpos: [2]; default: 0;
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_D_AHBERR4    (BIT(2))
#define USB_D_AHBERR4_M  (USB_D_AHBERR4_V << USB_D_AHBERR4_S)
#define USB_D_AHBERR4_V  0x00000001
#define USB_D_AHBERR4_S  2
/** USB_D_TIMEOUT4 : R/W1C; bitpos: [3]; default: 0;
 *  0x0 : No Timeout interrupt
 *  0x1 : Timeout interrupt
 */
#define USB_D_TIMEOUT4    (BIT(3))
#define USB_D_TIMEOUT4_M  (USB_D_TIMEOUT4_V << USB_D_TIMEOUT4_S)
#define USB_D_TIMEOUT4_V  0x00000001
#define USB_D_TIMEOUT4_S  3
/** USB_D_INTKNTXFEMP4 : R/W1C; bitpos: [4]; default: 0;
 *  0x0 : No IN Token Received when TxFIFO Empty Interrupt
 *  0x1 : IN Token Received when TxFIFO Empty Interrupt
 */
#define USB_D_INTKNTXFEMP4    (BIT(4))
#define USB_D_INTKNTXFEMP4_M  (USB_D_INTKNTXFEMP4_V << USB_D_INTKNTXFEMP4_S)
#define USB_D_INTKNTXFEMP4_V  0x00000001
#define USB_D_INTKNTXFEMP4_S  4
/** USB_D_INTKNEPMIS4 : R/W1C; bitpos: [5]; default: 0;
 *  0x0 : No IN Token Received with EP Mismatch Interrupt
 *  0x1 : IN Token Received with EP Mismatch interrupt
 */
#define USB_D_INTKNEPMIS4    (BIT(5))
#define USB_D_INTKNEPMIS4_M  (USB_D_INTKNEPMIS4_V << USB_D_INTKNEPMIS4_S)
#define USB_D_INTKNEPMIS4_V  0x00000001
#define USB_D_INTKNEPMIS4_S  5
/** USB_D_INEPNAKEFF4 : R/W1C; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective
 *  0x0 : No IN Endpoint NAK Effective interrupt
 *  0x1 : IN Endpoint NAK Effective interrupt
 */
#define USB_D_INEPNAKEFF4    (BIT(6))
#define USB_D_INEPNAKEFF4_M  (USB_D_INEPNAKEFF4_V << USB_D_INEPNAKEFF4_S)
#define USB_D_INEPNAKEFF4_V  0x00000001
#define USB_D_INEPNAKEFF4_S  6
/** USB_D_TXFEMP4 : RO; bitpos: [7]; default: 0;
 *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
 *  completely empty
 *  0x0 : No Transmit FIFO Empty interrupt
 *  0x1 : Transmit FIFO Empty interrupt
 */
#define USB_D_TXFEMP4    (BIT(7))
#define USB_D_TXFEMP4_M  (USB_D_TXFEMP4_V << USB_D_TXFEMP4_S)
#define USB_D_TXFEMP4_V  0x00000001
#define USB_D_TXFEMP4_S  7
/** USB_D_TXFIFOUNDRN4 : R/W1C; bitpos: [8]; default: 0;
 *  The core generates this interrupt when it detects a transmit FIFO underrun
 *  condition in threshold mode for this endpoint
 *  0x0 : No Fifo Underrun interrupt
 *  0x1 : Fifo Underrun interrupt
 */
#define USB_D_TXFIFOUNDRN4    (BIT(8))
#define USB_D_TXFIFOUNDRN4_M  (USB_D_TXFIFOUNDRN4_V << USB_D_TXFIFOUNDRN4_S)
#define USB_D_TXFIFOUNDRN4_V  0x00000001
#define USB_D_TXFIFOUNDRN4_S  8
/** USB_D_BNAINTR4 : R/W1C; bitpos: [9]; default: 0;
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
 *  this interrupt when the descriptor accessed is not ready for the Core to process,
 *  such as Host busy or DMA done
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_D_BNAINTR4    (BIT(9))
#define USB_D_BNAINTR4_M  (USB_D_BNAINTR4_V << USB_D_BNAINTR4_S)
#define USB_D_BNAINTR4_V  0x00000001
#define USB_D_BNAINTR4_S  9
/** USB_D_PKTDRPSTS4 : R/W1C; bitpos: [11]; default: 0;
 *  Packet Drop Status. This bit indicates to the application that an ISOC OUT packet
 *  has been dropped. This bit does not have an associated mask bit and does not
 *  generate an interrupt
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status
 */
#define USB_D_PKTDRPSTS4    (BIT(11))
#define USB_D_PKTDRPSTS4_M  (USB_D_PKTDRPSTS4_V << USB_D_PKTDRPSTS4_S)
#define USB_D_PKTDRPSTS4_V  0x00000001
#define USB_D_PKTDRPSTS4_S  11
/** USB_D_BBLEERR4 : R/W1C; bitpos: [12]; default: 0;
 *  The core generates this interrupt when babble is received for the endpoint
 *  0x0 : No interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_D_BBLEERR4    (BIT(12))
#define USB_D_BBLEERR4_M  (USB_D_BBLEERR4_V << USB_D_BBLEERR4_S)
#define USB_D_BBLEERR4_V  0x00000001
#define USB_D_BBLEERR4_S  12
/** USB_D_NAKINTRPT4 : R/W1C; bitpos: [13]; default: 0;
 *  NAK Interrupt. The core generates this interrupt when a NAK is transmitted or
 *  received by the device
 *  0x0 : No interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_D_NAKINTRPT4    (BIT(13))
#define USB_D_NAKINTRPT4_M  (USB_D_NAKINTRPT4_V << USB_D_NAKINTRPT4_S)
#define USB_D_NAKINTRPT4_V  0x00000001
#define USB_D_NAKINTRPT4_S  13
/** USB_D_NYETINTRPT4 : R/W1C; bitpos: [14]; default: 0;
 *  NYET Interrupt. The core generates this interrupt when a NYET response is
 *  transmitted for a non isochronous OUT endpoint
 *  0x0 : No interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_D_NYETINTRPT4    (BIT(14))
#define USB_D_NYETINTRPT4_M  (USB_D_NYETINTRPT4_V << USB_D_NYETINTRPT4_S)
#define USB_D_NYETINTRPT4_V  0x00000001
#define USB_D_NYETINTRPT4_S  14


/** USB_DIEPINT5_REG register
 *  Device IN Endpoint 5 Interrupt Register
 */
#define USB_DIEPINT5_REG (SOC_DPORT_USB_BASE + 0x9a8)
/** USB_D_XFERCOMPL5 : R/W1C; bitpos: [0]; default: 0;
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Completed Interrupt
 */
#define USB_D_XFERCOMPL5    (BIT(0))
#define USB_D_XFERCOMPL5_M  (USB_D_XFERCOMPL5_V << USB_D_XFERCOMPL5_S)
#define USB_D_XFERCOMPL5_V  0x00000001
#define USB_D_XFERCOMPL5_S  0
/** USB_D_EPDISBLD5 : R/W1C; bitpos: [1]; default: 0;
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_D_EPDISBLD5    (BIT(1))
#define USB_D_EPDISBLD5_M  (USB_D_EPDISBLD5_V << USB_D_EPDISBLD5_S)
#define USB_D_EPDISBLD5_V  0x00000001
#define USB_D_EPDISBLD5_S  1
/** USB_D_AHBERR5 : R/W1C; bitpos: [2]; default: 0;
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_D_AHBERR5    (BIT(2))
#define USB_D_AHBERR5_M  (USB_D_AHBERR5_V << USB_D_AHBERR5_S)
#define USB_D_AHBERR5_V  0x00000001
#define USB_D_AHBERR5_S  2
/** USB_D_TIMEOUT5 : R/W1C; bitpos: [3]; default: 0;
 *  0x0 : No Timeout interrupt
 *  0x1 : Timeout interrupt
 */
#define USB_D_TIMEOUT5    (BIT(3))
#define USB_D_TIMEOUT5_M  (USB_D_TIMEOUT5_V << USB_D_TIMEOUT5_S)
#define USB_D_TIMEOUT5_V  0x00000001
#define USB_D_TIMEOUT5_S  3
/** USB_D_INTKNTXFEMP5 : R/W1C; bitpos: [4]; default: 0;
 *  0x0 : No IN Token Received when TxFIFO Empty Interrupt
 *  0x1 : IN Token Received when TxFIFO Empty Interrupt
 */
#define USB_D_INTKNTXFEMP5    (BIT(4))
#define USB_D_INTKNTXFEMP5_M  (USB_D_INTKNTXFEMP5_V << USB_D_INTKNTXFEMP5_S)
#define USB_D_INTKNTXFEMP5_V  0x00000001
#define USB_D_INTKNTXFEMP5_S  4
/** USB_D_INTKNEPMIS5 : R/W1C; bitpos: [5]; default: 0;
 *  0x0 : No IN Token Received with EP Mismatch Interrupt
 *  0x1 : IN Token Received with EP Mismatch interrupt
 */
#define USB_D_INTKNEPMIS5    (BIT(5))
#define USB_D_INTKNEPMIS5_M  (USB_D_INTKNEPMIS5_V << USB_D_INTKNEPMIS5_S)
#define USB_D_INTKNEPMIS5_V  0x00000001
#define USB_D_INTKNEPMIS5_S  5
/** USB_D_INEPNAKEFF5 : R/W1C; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective
 *  0x0 : No IN Endpoint NAK Effective interrupt
 *  0x1 : IN Endpoint NAK Effective interrupt
 */
#define USB_D_INEPNAKEFF5    (BIT(6))
#define USB_D_INEPNAKEFF5_M  (USB_D_INEPNAKEFF5_V << USB_D_INEPNAKEFF5_S)
#define USB_D_INEPNAKEFF5_V  0x00000001
#define USB_D_INEPNAKEFF5_S  6
/** USB_D_TXFEMP5 : RO; bitpos: [7]; default: 0;
 *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
 *  completely empty
 *  0x0 : No Transmit FIFO Empty interrupt
 *  0x1 : Transmit FIFO Empty interrupt
 */
#define USB_D_TXFEMP5    (BIT(7))
#define USB_D_TXFEMP5_M  (USB_D_TXFEMP5_V << USB_D_TXFEMP5_S)
#define USB_D_TXFEMP5_V  0x00000001
#define USB_D_TXFEMP5_S  7
/** USB_D_TXFIFOUNDRN5 : R/W1C; bitpos: [8]; default: 0;
 *  The core generates this interrupt when it detects a transmit FIFO underrun
 *  condition in threshold mode for this endpoint
 *  0x0 : No Fifo Underrun interrupt
 *  0x1 : Fifo Underrun interrupt
 */
#define USB_D_TXFIFOUNDRN5    (BIT(8))
#define USB_D_TXFIFOUNDRN5_M  (USB_D_TXFIFOUNDRN5_V << USB_D_TXFIFOUNDRN5_S)
#define USB_D_TXFIFOUNDRN5_V  0x00000001
#define USB_D_TXFIFOUNDRN5_S  8
/** USB_D_BNAINTR5 : R/W1C; bitpos: [9]; default: 0;
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
 *  this interrupt when the descriptor accessed is not ready for the Core to process,
 *  such as Host busy or DMA done
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_D_BNAINTR5    (BIT(9))
#define USB_D_BNAINTR5_M  (USB_D_BNAINTR5_V << USB_D_BNAINTR5_S)
#define USB_D_BNAINTR5_V  0x00000001
#define USB_D_BNAINTR5_S  9
/** USB_D_PKTDRPSTS5 : R/W1C; bitpos: [11]; default: 0;
 *  Packet Drop Status. This bit indicates to the application that an ISOC OUT packet
 *  has been dropped. This bit does not have an associated mask bit and does not
 *  generate an interrupt
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status
 */
#define USB_D_PKTDRPSTS5    (BIT(11))
#define USB_D_PKTDRPSTS5_M  (USB_D_PKTDRPSTS5_V << USB_D_PKTDRPSTS5_S)
#define USB_D_PKTDRPSTS5_V  0x00000001
#define USB_D_PKTDRPSTS5_S  11
/** USB_D_BBLEERR5 : R/W1C; bitpos: [12]; default: 0;
 *  The core generates this interrupt when babble is received for the endpoint
 *  0x0 : No interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_D_BBLEERR5    (BIT(12))
#define USB_D_BBLEERR5_M  (USB_D_BBLEERR5_V << USB_D_BBLEERR5_S)
#define USB_D_BBLEERR5_V  0x00000001
#define USB_D_BBLEERR5_S  12
/** USB_D_NAKINTRPT5 : R/W1C; bitpos: [13]; default: 0;
 *  NAK Interrupt. The core generates this interrupt when a NAK is transmitted or
 *  received by the device
 *  0x0 : No interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_D_NAKINTRPT5    (BIT(13))
#define USB_D_NAKINTRPT5_M  (USB_D_NAKINTRPT5_V << USB_D_NAKINTRPT5_S)
#define USB_D_NAKINTRPT5_V  0x00000001
#define USB_D_NAKINTRPT5_S  13
/** USB_D_NYETINTRPT5 : R/W1C; bitpos: [14]; default: 0;
 *  NYET Interrupt. The core generates this interrupt when a NYET response is
 *  transmitted for a non isochronous OUT endpoint
 *  0x0 : No interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_D_NYETINTRPT5    (BIT(14))
#define USB_D_NYETINTRPT5_M  (USB_D_NYETINTRPT5_V << USB_D_NYETINTRPT5_S)
#define USB_D_NYETINTRPT5_V  0x00000001
#define USB_D_NYETINTRPT5_S  14


/** USB_DIEPINT6_REG register
 *  Device IN Endpoint 6 Interrupt Register
 */
#define USB_DIEPINT6_REG (SOC_DPORT_USB_BASE + 0x9c8)
/** USB_D_XFERCOMPL6 : R/W1C; bitpos: [0]; default: 0;
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Completed Interrupt
 */
#define USB_D_XFERCOMPL6    (BIT(0))
#define USB_D_XFERCOMPL6_M  (USB_D_XFERCOMPL6_V << USB_D_XFERCOMPL6_S)
#define USB_D_XFERCOMPL6_V  0x00000001
#define USB_D_XFERCOMPL6_S  0
/** USB_D_EPDISBLD6 : R/W1C; bitpos: [1]; default: 0;
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_D_EPDISBLD6    (BIT(1))
#define USB_D_EPDISBLD6_M  (USB_D_EPDISBLD6_V << USB_D_EPDISBLD6_S)
#define USB_D_EPDISBLD6_V  0x00000001
#define USB_D_EPDISBLD6_S  1
/** USB_D_AHBERR6 : R/W1C; bitpos: [2]; default: 0;
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_D_AHBERR6    (BIT(2))
#define USB_D_AHBERR6_M  (USB_D_AHBERR6_V << USB_D_AHBERR6_S)
#define USB_D_AHBERR6_V  0x00000001
#define USB_D_AHBERR6_S  2
/** USB_D_TIMEOUT6 : R/W1C; bitpos: [3]; default: 0;
 *  0x0 : No Timeout interrupt
 *  0x1 : Timeout interrupt
 */
#define USB_D_TIMEOUT6    (BIT(3))
#define USB_D_TIMEOUT6_M  (USB_D_TIMEOUT6_V << USB_D_TIMEOUT6_S)
#define USB_D_TIMEOUT6_V  0x00000001
#define USB_D_TIMEOUT6_S  3
/** USB_D_INTKNTXFEMP6 : R/W1C; bitpos: [4]; default: 0;
 *  0x0 : No IN Token Received when TxFIFO Empty Interrupt
 *  0x1 : IN Token Received when TxFIFO Empty Interrupt
 */
#define USB_D_INTKNTXFEMP6    (BIT(4))
#define USB_D_INTKNTXFEMP6_M  (USB_D_INTKNTXFEMP6_V << USB_D_INTKNTXFEMP6_S)
#define USB_D_INTKNTXFEMP6_V  0x00000001
#define USB_D_INTKNTXFEMP6_S  4
/** USB_D_INTKNEPMIS6 : R/W1C; bitpos: [5]; default: 0;
 *  0x0 : No IN Token Received with EP Mismatch Interrupt
 *  0x1 : IN Token Received with EP Mismatch interrupt
 */
#define USB_D_INTKNEPMIS6    (BIT(5))
#define USB_D_INTKNEPMIS6_M  (USB_D_INTKNEPMIS6_V << USB_D_INTKNEPMIS6_S)
#define USB_D_INTKNEPMIS6_V  0x00000001
#define USB_D_INTKNEPMIS6_S  5
/** USB_D_INEPNAKEFF6 : R/W1C; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective
 *  0x0 : No IN Endpoint NAK Effective interrupt
 *  0x1 : IN Endpoint NAK Effective interrupt
 */
#define USB_D_INEPNAKEFF6    (BIT(6))
#define USB_D_INEPNAKEFF6_M  (USB_D_INEPNAKEFF6_V << USB_D_INEPNAKEFF6_S)
#define USB_D_INEPNAKEFF6_V  0x00000001
#define USB_D_INEPNAKEFF6_S  6
/** USB_D_TXFEMP6 : RO; bitpos: [7]; default: 0;
 *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
 *  completely empty
 *  0x0 : No Transmit FIFO Empty interrupt
 *  0x1 : Transmit FIFO Empty interrupt
 */
#define USB_D_TXFEMP6    (BIT(7))
#define USB_D_TXFEMP6_M  (USB_D_TXFEMP6_V << USB_D_TXFEMP6_S)
#define USB_D_TXFEMP6_V  0x00000001
#define USB_D_TXFEMP6_S  7
/** USB_D_TXFIFOUNDRN6 : R/W1C; bitpos: [8]; default: 0;
 *  The core generates this interrupt when it detects a transmit FIFO underrun
 *  condition in threshold mode for this endpoint
 *  0x0 : No Fifo Underrun interrupt
 *  0x1 : Fifo Underrun interrupt
 */
#define USB_D_TXFIFOUNDRN6    (BIT(8))
#define USB_D_TXFIFOUNDRN6_M  (USB_D_TXFIFOUNDRN6_V << USB_D_TXFIFOUNDRN6_S)
#define USB_D_TXFIFOUNDRN6_V  0x00000001
#define USB_D_TXFIFOUNDRN6_S  8
/** USB_D_BNAINTR6 : R/W1C; bitpos: [9]; default: 0;
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
 *  this interrupt when the descriptor accessed is not ready for the Core to process,
 *  such as Host busy or DMA done
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_D_BNAINTR6    (BIT(9))
#define USB_D_BNAINTR6_M  (USB_D_BNAINTR6_V << USB_D_BNAINTR6_S)
#define USB_D_BNAINTR6_V  0x00000001
#define USB_D_BNAINTR6_S  9
/** USB_D_PKTDRPSTS6 : R/W1C; bitpos: [11]; default: 0;
 *  Packet Drop Status. This bit indicates to the application that an ISOC OUT packet
 *  has been dropped. This bit does not have an associated mask bit and does not
 *  generate an interrupt
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status
 */
#define USB_D_PKTDRPSTS6    (BIT(11))
#define USB_D_PKTDRPSTS6_M  (USB_D_PKTDRPSTS6_V << USB_D_PKTDRPSTS6_S)
#define USB_D_PKTDRPSTS6_V  0x00000001
#define USB_D_PKTDRPSTS6_S  11
/** USB_D_BBLEERR6 : R/W1C; bitpos: [12]; default: 0;
 *  The core generates this interrupt when babble is received for the endpoint
 *  0x0 : No interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_D_BBLEERR6    (BIT(12))
#define USB_D_BBLEERR6_M  (USB_D_BBLEERR6_V << USB_D_BBLEERR6_S)
#define USB_D_BBLEERR6_V  0x00000001
#define USB_D_BBLEERR6_S  12
/** USB_D_NAKINTRPT6 : R/W1C; bitpos: [13]; default: 0;
 *  NAK Interrupt. The core generates this interrupt when a NAK is transmitted or
 *  received by the device
 *  0x0 : No interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_D_NAKINTRPT6    (BIT(13))
#define USB_D_NAKINTRPT6_M  (USB_D_NAKINTRPT6_V << USB_D_NAKINTRPT6_S)
#define USB_D_NAKINTRPT6_V  0x00000001
#define USB_D_NAKINTRPT6_S  13
/** USB_D_NYETINTRPT6 : R/W1C; bitpos: [14]; default: 0;
 *  NYET Interrupt. The core generates this interrupt when a NYET response is
 *  transmitted for a non isochronous OUT endpoint
 *  0x0 : No interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_D_NYETINTRPT6    (BIT(14))
#define USB_D_NYETINTRPT6_M  (USB_D_NYETINTRPT6_V << USB_D_NYETINTRPT6_S)
#define USB_D_NYETINTRPT6_V  0x00000001
#define USB_D_NYETINTRPT6_S  14


/** USB_DOEPINT0_REG register
 *  Device OUT Endpoint 0 Interrupt Register
 */
#define USB_DOEPINT0_REG (SOC_DPORT_USB_BASE + 0xb08)
/** USB_XFERCOMPL0 : R/W1C; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Complete Interrupt
 */
#define USB_XFERCOMPL0    (BIT(0))
#define USB_XFERCOMPL0_M  (USB_XFERCOMPL0_V << USB_XFERCOMPL0_S)
#define USB_XFERCOMPL0_V  0x00000001
#define USB_XFERCOMPL0_S  0
/** USB_EPDISBLD0 : R/W1C; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_EPDISBLD0    (BIT(1))
#define USB_EPDISBLD0_M  (USB_EPDISBLD0_V << USB_EPDISBLD0_S)
#define USB_EPDISBLD0_V  0x00000001
#define USB_EPDISBLD0_S  1
/** USB_AHBERR0 : R/W1C; bitpos: [2]; default: 0;
 *  AHB Error
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_AHBERR0    (BIT(2))
#define USB_AHBERR0_M  (USB_AHBERR0_V << USB_AHBERR0_S)
#define USB_AHBERR0_V  0x00000001
#define USB_AHBERR0_S  2
/** USB_SETUP0 : R/W1C; bitpos: [3]; default: 0;
 *  SETUP Phase Done
 *  0x0 : No SETUP Phase Done
 *  0x1 : SETUP Phase Done
 */
#define USB_SETUP0    (BIT(3))
#define USB_SETUP0_M  (USB_SETUP0_V << USB_SETUP0_S)
#define USB_SETUP0_V  0x00000001
#define USB_SETUP0_S  3
/** USB_OUTTKNEPDIS0 : R/W1C; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled
 *  0x0 : No OUT Token Received When Endpoint Disabled
 *  0x1 : OUT Token Received When Endpoint Disabled
 */
#define USB_OUTTKNEPDIS0    (BIT(4))
#define USB_OUTTKNEPDIS0_M  (USB_OUTTKNEPDIS0_V << USB_OUTTKNEPDIS0_S)
#define USB_OUTTKNEPDIS0_V  0x00000001
#define USB_OUTTKNEPDIS0_S  4
/** USB_STSPHSERCVD0 : R/W1C; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write
 *  0x0 : No Status Phase Received for Control Write
 *  0x1 : Status Phase Received for Control Write
 */
#define USB_STSPHSERCVD0    (BIT(5))
#define USB_STSPHSERCVD0_M  (USB_STSPHSERCVD0_V << USB_STSPHSERCVD0_S)
#define USB_STSPHSERCVD0_V  0x00000001
#define USB_STSPHSERCVD0_S  5
/** USB_BACK2BACKSETUP0 : R/W1C; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received
 *  0x0 : No Back-to-Back SETUP Packets Received
 *  0x1 : Back-to-Back SETUP Packets Received
 */
#define USB_BACK2BACKSETUP0    (BIT(6))
#define USB_BACK2BACKSETUP0_M  (USB_BACK2BACKSETUP0_V << USB_BACK2BACKSETUP0_S)
#define USB_BACK2BACKSETUP0_V  0x00000001
#define USB_BACK2BACKSETUP0_S  6
/** USB_OUTPKTERR0 : R/W1C; bitpos: [8]; default: 0;
 *  OUT Packet Error
 *  0x0 : No OUT Packet Error
 *  0x1 : OUT Packet Error
 */
#define USB_OUTPKTERR0    (BIT(8))
#define USB_OUTPKTERR0_M  (USB_OUTPKTERR0_V << USB_OUTPKTERR0_S)
#define USB_OUTPKTERR0_V  0x00000001
#define USB_OUTPKTERR0_S  8
/** USB_BNAINTR0 : R/W1C; bitpos: [9]; default: 0;
 *  Buffer Not Available Interrupt
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_BNAINTR0    (BIT(9))
#define USB_BNAINTR0_M  (USB_BNAINTR0_V << USB_BNAINTR0_S)
#define USB_BNAINTR0_V  0x00000001
#define USB_BNAINTR0_S  9
/** USB_PKTDRPSTS0 : R/W1C; bitpos: [11]; default: 0;
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status interrupt
 */
#define USB_PKTDRPSTS0    (BIT(11))
#define USB_PKTDRPSTS0_M  (USB_PKTDRPSTS0_V << USB_PKTDRPSTS0_S)
#define USB_PKTDRPSTS0_V  0x00000001
#define USB_PKTDRPSTS0_S  11
/** USB_BBLEERR0 : R/W1C; bitpos: [12]; default: 0;
 *  0x0 : No BbleErr interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_BBLEERR0    (BIT(12))
#define USB_BBLEERR0_M  (USB_BBLEERR0_V << USB_BBLEERR0_S)
#define USB_BBLEERR0_V  0x00000001
#define USB_BBLEERR0_S  12
/** USB_NAKINTRPT0 : R/W1C; bitpos: [13]; default: 0;
 *  0x0 : No NAK interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_NAKINTRPT0    (BIT(13))
#define USB_NAKINTRPT0_M  (USB_NAKINTRPT0_V << USB_NAKINTRPT0_S)
#define USB_NAKINTRPT0_V  0x00000001
#define USB_NAKINTRPT0_S  13
/** USB_NYEPINTRPT0 : R/W1C; bitpos: [14]; default: 0;
 *  0x0 : No NYET interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_NYEPINTRPT0    (BIT(14))
#define USB_NYEPINTRPT0_M  (USB_NYEPINTRPT0_V << USB_NYEPINTRPT0_S)
#define USB_NYEPINTRPT0_V  0x00000001
#define USB_NYEPINTRPT0_S  14
/** USB_STUPPKTRCVD0 : R/W1C; bitpos: [15]; default: 0;
 *  0x0 : No Setup packet received
 *  0x1 : Setup packet received
 */
#define USB_STUPPKTRCVD0    (BIT(15))
#define USB_STUPPKTRCVD0_M  (USB_STUPPKTRCVD0_V << USB_STUPPKTRCVD0_S)
#define USB_STUPPKTRCVD0_V  0x00000001
#define USB_STUPPKTRCVD0_S  15


/** USB_DOEPINT1_REG register
 *  Device OUT Endpoint 1 Interrupt Register
 */
#define USB_DOEPINT1_REG (SOC_DPORT_USB_BASE + 0xb28)
/** USB_XFERCOMPL1 : R/W1C; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Complete Interrupt
 */
#define USB_XFERCOMPL1    (BIT(0))
#define USB_XFERCOMPL1_M  (USB_XFERCOMPL1_V << USB_XFERCOMPL1_S)
#define USB_XFERCOMPL1_V  0x00000001
#define USB_XFERCOMPL1_S  0
/** USB_EPDISBLD1 : R/W1C; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_EPDISBLD1    (BIT(1))
#define USB_EPDISBLD1_M  (USB_EPDISBLD1_V << USB_EPDISBLD1_S)
#define USB_EPDISBLD1_V  0x00000001
#define USB_EPDISBLD1_S  1
/** USB_AHBERR1 : R/W1C; bitpos: [2]; default: 0;
 *  AHB Error
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_AHBERR1    (BIT(2))
#define USB_AHBERR1_M  (USB_AHBERR1_V << USB_AHBERR1_S)
#define USB_AHBERR1_V  0x00000001
#define USB_AHBERR1_S  2
/** USB_SETUP1 : R/W1C; bitpos: [3]; default: 0;
 *  SETUP Phase Done
 *  0x0 : No SETUP Phase Done
 *  0x1 : SETUP Phase Done
 */
#define USB_SETUP1    (BIT(3))
#define USB_SETUP1_M  (USB_SETUP1_V << USB_SETUP1_S)
#define USB_SETUP1_V  0x00000001
#define USB_SETUP1_S  3
/** USB_OUTTKNEPDIS1 : R/W1C; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled
 *  0x0 : No OUT Token Received When Endpoint Disabled
 *  0x1 : OUT Token Received When Endpoint Disabled
 */
#define USB_OUTTKNEPDIS1    (BIT(4))
#define USB_OUTTKNEPDIS1_M  (USB_OUTTKNEPDIS1_V << USB_OUTTKNEPDIS1_S)
#define USB_OUTTKNEPDIS1_V  0x00000001
#define USB_OUTTKNEPDIS1_S  4
/** USB_STSPHSERCVD1 : R/W1C; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write
 *  0x0 : No Status Phase Received for Control Write
 *  0x1 : Status Phase Received for Control Write
 */
#define USB_STSPHSERCVD1    (BIT(5))
#define USB_STSPHSERCVD1_M  (USB_STSPHSERCVD1_V << USB_STSPHSERCVD1_S)
#define USB_STSPHSERCVD1_V  0x00000001
#define USB_STSPHSERCVD1_S  5
/** USB_BACK2BACKSETUP1 : R/W1C; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received
 *  0x0 : No Back-to-Back SETUP Packets Received
 *  0x1 : Back-to-Back SETUP Packets Received
 */
#define USB_BACK2BACKSETUP1    (BIT(6))
#define USB_BACK2BACKSETUP1_M  (USB_BACK2BACKSETUP1_V << USB_BACK2BACKSETUP1_S)
#define USB_BACK2BACKSETUP1_V  0x00000001
#define USB_BACK2BACKSETUP1_S  6
/** USB_OUTPKTERR1 : R/W1C; bitpos: [8]; default: 0;
 *  OUT Packet Error
 *  0x0 : No OUT Packet Error
 *  0x1 : OUT Packet Error
 */
#define USB_OUTPKTERR1    (BIT(8))
#define USB_OUTPKTERR1_M  (USB_OUTPKTERR1_V << USB_OUTPKTERR1_S)
#define USB_OUTPKTERR1_V  0x00000001
#define USB_OUTPKTERR1_S  8
/** USB_BNAINTR1 : R/W1C; bitpos: [9]; default: 0;
 *  Buffer Not Available Interrupt
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_BNAINTR1    (BIT(9))
#define USB_BNAINTR1_M  (USB_BNAINTR1_V << USB_BNAINTR1_S)
#define USB_BNAINTR1_V  0x00000001
#define USB_BNAINTR1_S  9
/** USB_PKTDRPSTS1 : R/W1C; bitpos: [11]; default: 0;
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status interrupt
 */
#define USB_PKTDRPSTS1    (BIT(11))
#define USB_PKTDRPSTS1_M  (USB_PKTDRPSTS1_V << USB_PKTDRPSTS1_S)
#define USB_PKTDRPSTS1_V  0x00000001
#define USB_PKTDRPSTS1_S  11
/** USB_BBLEERR1 : R/W1C; bitpos: [12]; default: 0;
 *  0x0 : No BbleErr interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_BBLEERR1    (BIT(12))
#define USB_BBLEERR1_M  (USB_BBLEERR1_V << USB_BBLEERR1_S)
#define USB_BBLEERR1_V  0x00000001
#define USB_BBLEERR1_S  12
/** USB_NAKINTRPT1 : R/W1C; bitpos: [13]; default: 0;
 *  0x0 : No NAK interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_NAKINTRPT1    (BIT(13))
#define USB_NAKINTRPT1_M  (USB_NAKINTRPT1_V << USB_NAKINTRPT1_S)
#define USB_NAKINTRPT1_V  0x00000001
#define USB_NAKINTRPT1_S  13
/** USB_NYEPINTRPT1 : R/W1C; bitpos: [14]; default: 0;
 *  0x0 : No NYET interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_NYEPINTRPT1    (BIT(14))
#define USB_NYEPINTRPT1_M  (USB_NYEPINTRPT1_V << USB_NYEPINTRPT1_S)
#define USB_NYEPINTRPT1_V  0x00000001
#define USB_NYEPINTRPT1_S  14
/** USB_STUPPKTRCVD1 : R/W1C; bitpos: [15]; default: 0;
 *  0x0 : No Setup packet received
 *  0x1 : Setup packet received
 */
#define USB_STUPPKTRCVD1    (BIT(15))
#define USB_STUPPKTRCVD1_M  (USB_STUPPKTRCVD1_V << USB_STUPPKTRCVD1_S)
#define USB_STUPPKTRCVD1_V  0x00000001
#define USB_STUPPKTRCVD1_S  15


/** USB_DOEPINT2_REG register
 *  Device OUT Endpoint 2 Interrupt Register
 */
#define USB_DOEPINT2_REG (SOC_DPORT_USB_BASE + 0xb48)
/** USB_XFERCOMPL2 : R/W1C; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Complete Interrupt
 */
#define USB_XFERCOMPL2    (BIT(0))
#define USB_XFERCOMPL2_M  (USB_XFERCOMPL2_V << USB_XFERCOMPL2_S)
#define USB_XFERCOMPL2_V  0x00000001
#define USB_XFERCOMPL2_S  0
/** USB_EPDISBLD2 : R/W1C; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_EPDISBLD2    (BIT(1))
#define USB_EPDISBLD2_M  (USB_EPDISBLD2_V << USB_EPDISBLD2_S)
#define USB_EPDISBLD2_V  0x00000001
#define USB_EPDISBLD2_S  1
/** USB_AHBERR2 : R/W1C; bitpos: [2]; default: 0;
 *  AHB Error
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_AHBERR2    (BIT(2))
#define USB_AHBERR2_M  (USB_AHBERR2_V << USB_AHBERR2_S)
#define USB_AHBERR2_V  0x00000001
#define USB_AHBERR2_S  2
/** USB_SETUP2 : R/W1C; bitpos: [3]; default: 0;
 *  SETUP Phase Done
 *  0x0 : No SETUP Phase Done
 *  0x1 : SETUP Phase Done
 */
#define USB_SETUP2    (BIT(3))
#define USB_SETUP2_M  (USB_SETUP2_V << USB_SETUP2_S)
#define USB_SETUP2_V  0x00000001
#define USB_SETUP2_S  3
/** USB_OUTTKNEPDIS2 : R/W1C; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled
 *  0x0 : No OUT Token Received When Endpoint Disabled
 *  0x1 : OUT Token Received When Endpoint Disabled
 */
#define USB_OUTTKNEPDIS2    (BIT(4))
#define USB_OUTTKNEPDIS2_M  (USB_OUTTKNEPDIS2_V << USB_OUTTKNEPDIS2_S)
#define USB_OUTTKNEPDIS2_V  0x00000001
#define USB_OUTTKNEPDIS2_S  4
/** USB_STSPHSERCVD2 : R/W1C; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write
 *  0x0 : No Status Phase Received for Control Write
 *  0x1 : Status Phase Received for Control Write
 */
#define USB_STSPHSERCVD2    (BIT(5))
#define USB_STSPHSERCVD2_M  (USB_STSPHSERCVD2_V << USB_STSPHSERCVD2_S)
#define USB_STSPHSERCVD2_V  0x00000001
#define USB_STSPHSERCVD2_S  5
/** USB_BACK2BACKSETUP2 : R/W1C; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received
 *  0x0 : No Back-to-Back SETUP Packets Received
 *  0x1 : Back-to-Back SETUP Packets Received
 */
#define USB_BACK2BACKSETUP2    (BIT(6))
#define USB_BACK2BACKSETUP2_M  (USB_BACK2BACKSETUP2_V << USB_BACK2BACKSETUP2_S)
#define USB_BACK2BACKSETUP2_V  0x00000001
#define USB_BACK2BACKSETUP2_S  6
/** USB_OUTPKTERR2 : R/W1C; bitpos: [8]; default: 0;
 *  OUT Packet Error
 *  0x0 : No OUT Packet Error
 *  0x1 : OUT Packet Error
 */
#define USB_OUTPKTERR2    (BIT(8))
#define USB_OUTPKTERR2_M  (USB_OUTPKTERR2_V << USB_OUTPKTERR2_S)
#define USB_OUTPKTERR2_V  0x00000001
#define USB_OUTPKTERR2_S  8
/** USB_BNAINTR2 : R/W1C; bitpos: [9]; default: 0;
 *  Buffer Not Available Interrupt
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_BNAINTR2    (BIT(9))
#define USB_BNAINTR2_M  (USB_BNAINTR2_V << USB_BNAINTR2_S)
#define USB_BNAINTR2_V  0x00000001
#define USB_BNAINTR2_S  9
/** USB_PKTDRPSTS2 : R/W1C; bitpos: [11]; default: 0;
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status interrupt
 */
#define USB_PKTDRPSTS2    (BIT(11))
#define USB_PKTDRPSTS2_M  (USB_PKTDRPSTS2_V << USB_PKTDRPSTS2_S)
#define USB_PKTDRPSTS2_V  0x00000001
#define USB_PKTDRPSTS2_S  11
/** USB_BBLEERR2 : R/W1C; bitpos: [12]; default: 0;
 *  0x0 : No BbleErr interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_BBLEERR2    (BIT(12))
#define USB_BBLEERR2_M  (USB_BBLEERR2_V << USB_BBLEERR2_S)
#define USB_BBLEERR2_V  0x00000001
#define USB_BBLEERR2_S  12
/** USB_NAKINTRPT2 : R/W1C; bitpos: [13]; default: 0;
 *  0x0 : No NAK interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_NAKINTRPT2    (BIT(13))
#define USB_NAKINTRPT2_M  (USB_NAKINTRPT2_V << USB_NAKINTRPT2_S)
#define USB_NAKINTRPT2_V  0x00000001
#define USB_NAKINTRPT2_S  13
/** USB_NYEPINTRPT2 : R/W1C; bitpos: [14]; default: 0;
 *  0x0 : No NYET interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_NYEPINTRPT2    (BIT(14))
#define USB_NYEPINTRPT2_M  (USB_NYEPINTRPT2_V << USB_NYEPINTRPT2_S)
#define USB_NYEPINTRPT2_V  0x00000001
#define USB_NYEPINTRPT2_S  14
/** USB_STUPPKTRCVD2 : R/W1C; bitpos: [15]; default: 0;
 *  0x0 : No Setup packet received
 *  0x1 : Setup packet received
 */
#define USB_STUPPKTRCVD2    (BIT(15))
#define USB_STUPPKTRCVD2_M  (USB_STUPPKTRCVD2_V << USB_STUPPKTRCVD2_S)
#define USB_STUPPKTRCVD2_V  0x00000001
#define USB_STUPPKTRCVD2_S  15


/** USB_DOEPINT3_REG register
 *  Device OUT Endpoint 3 Interrupt Register
 */
#define USB_DOEPINT3_REG (SOC_DPORT_USB_BASE + 0xb68)
/** USB_XFERCOMPL3 : R/W1C; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Complete Interrupt
 */
#define USB_XFERCOMPL3    (BIT(0))
#define USB_XFERCOMPL3_M  (USB_XFERCOMPL3_V << USB_XFERCOMPL3_S)
#define USB_XFERCOMPL3_V  0x00000001
#define USB_XFERCOMPL3_S  0
/** USB_EPDISBLD3 : R/W1C; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_EPDISBLD3    (BIT(1))
#define USB_EPDISBLD3_M  (USB_EPDISBLD3_V << USB_EPDISBLD3_S)
#define USB_EPDISBLD3_V  0x00000001
#define USB_EPDISBLD3_S  1
/** USB_AHBERR3 : R/W1C; bitpos: [2]; default: 0;
 *  AHB Error
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_AHBERR3    (BIT(2))
#define USB_AHBERR3_M  (USB_AHBERR3_V << USB_AHBERR3_S)
#define USB_AHBERR3_V  0x00000001
#define USB_AHBERR3_S  2
/** USB_SETUP3 : R/W1C; bitpos: [3]; default: 0;
 *  SETUP Phase Done
 *  0x0 : No SETUP Phase Done
 *  0x1 : SETUP Phase Done
 */
#define USB_SETUP3    (BIT(3))
#define USB_SETUP3_M  (USB_SETUP3_V << USB_SETUP3_S)
#define USB_SETUP3_V  0x00000001
#define USB_SETUP3_S  3
/** USB_OUTTKNEPDIS3 : R/W1C; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled
 *  0x0 : No OUT Token Received When Endpoint Disabled
 *  0x1 : OUT Token Received When Endpoint Disabled
 */
#define USB_OUTTKNEPDIS3    (BIT(4))
#define USB_OUTTKNEPDIS3_M  (USB_OUTTKNEPDIS3_V << USB_OUTTKNEPDIS3_S)
#define USB_OUTTKNEPDIS3_V  0x00000001
#define USB_OUTTKNEPDIS3_S  4
/** USB_STSPHSERCVD3 : R/W1C; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write
 *  0x0 : No Status Phase Received for Control Write
 *  0x1 : Status Phase Received for Control Write
 */
#define USB_STSPHSERCVD3    (BIT(5))
#define USB_STSPHSERCVD3_M  (USB_STSPHSERCVD3_V << USB_STSPHSERCVD3_S)
#define USB_STSPHSERCVD3_V  0x00000001
#define USB_STSPHSERCVD3_S  5
/** USB_BACK2BACKSETUP3 : R/W1C; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received
 *  0x0 : No Back-to-Back SETUP Packets Received
 *  0x1 : Back-to-Back SETUP Packets Received
 */
#define USB_BACK2BACKSETUP3    (BIT(6))
#define USB_BACK2BACKSETUP3_M  (USB_BACK2BACKSETUP3_V << USB_BACK2BACKSETUP3_S)
#define USB_BACK2BACKSETUP3_V  0x00000001
#define USB_BACK2BACKSETUP3_S  6
/** USB_OUTPKTERR3 : R/W1C; bitpos: [8]; default: 0;
 *  OUT Packet Error
 *  0x0 : No OUT Packet Error
 *  0x1 : OUT Packet Error
 */
#define USB_OUTPKTERR3    (BIT(8))
#define USB_OUTPKTERR3_M  (USB_OUTPKTERR3_V << USB_OUTPKTERR3_S)
#define USB_OUTPKTERR3_V  0x00000001
#define USB_OUTPKTERR3_S  8
/** USB_BNAINTR3 : R/W1C; bitpos: [9]; default: 0;
 *  Buffer Not Available Interrupt
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_BNAINTR3    (BIT(9))
#define USB_BNAINTR3_M  (USB_BNAINTR3_V << USB_BNAINTR3_S)
#define USB_BNAINTR3_V  0x00000001
#define USB_BNAINTR3_S  9
/** USB_PKTDRPSTS3 : R/W1C; bitpos: [11]; default: 0;
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status interrupt
 */
#define USB_PKTDRPSTS3    (BIT(11))
#define USB_PKTDRPSTS3_M  (USB_PKTDRPSTS3_V << USB_PKTDRPSTS3_S)
#define USB_PKTDRPSTS3_V  0x00000001
#define USB_PKTDRPSTS3_S  11
/** USB_BBLEERR3 : R/W1C; bitpos: [12]; default: 0;
 *  0x0 : No BbleErr interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_BBLEERR3    (BIT(12))
#define USB_BBLEERR3_M  (USB_BBLEERR3_V << USB_BBLEERR3_S)
#define USB_BBLEERR3_V  0x00000001
#define USB_BBLEERR3_S  12
/** USB_NAKINTRPT3 : R/W1C; bitpos: [13]; default: 0;
 *  0x0 : No NAK interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_NAKINTRPT3    (BIT(13))
#define USB_NAKINTRPT3_M  (USB_NAKINTRPT3_V << USB_NAKINTRPT3_S)
#define USB_NAKINTRPT3_V  0x00000001
#define USB_NAKINTRPT3_S  13
/** USB_NYEPINTRPT3 : R/W1C; bitpos: [14]; default: 0;
 *  0x0 : No NYET interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_NYEPINTRPT3    (BIT(14))
#define USB_NYEPINTRPT3_M  (USB_NYEPINTRPT3_V << USB_NYEPINTRPT3_S)
#define USB_NYEPINTRPT3_V  0x00000001
#define USB_NYEPINTRPT3_S  14
/** USB_STUPPKTRCVD3 : R/W1C; bitpos: [15]; default: 0;
 *  0x0 : No Setup packet received
 *  0x1 : Setup packet received
 */
#define USB_STUPPKTRCVD3    (BIT(15))
#define USB_STUPPKTRCVD3_M  (USB_STUPPKTRCVD3_V << USB_STUPPKTRCVD3_S)
#define USB_STUPPKTRCVD3_V  0x00000001
#define USB_STUPPKTRCVD3_S  15


/** USB_DOEPINT4_REG register
 *  Device OUT Endpoint 4 Interrupt Register
 */
#define USB_DOEPINT4_REG (SOC_DPORT_USB_BASE + 0xb88)
/** USB_XFERCOMPL4 : R/W1C; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Complete Interrupt
 */
#define USB_XFERCOMPL4    (BIT(0))
#define USB_XFERCOMPL4_M  (USB_XFERCOMPL4_V << USB_XFERCOMPL4_S)
#define USB_XFERCOMPL4_V  0x00000001
#define USB_XFERCOMPL4_S  0
/** USB_EPDISBLD4 : R/W1C; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_EPDISBLD4    (BIT(1))
#define USB_EPDISBLD4_M  (USB_EPDISBLD4_V << USB_EPDISBLD4_S)
#define USB_EPDISBLD4_V  0x00000001
#define USB_EPDISBLD4_S  1
/** USB_AHBERR4 : R/W1C; bitpos: [2]; default: 0;
 *  AHB Error
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_AHBERR4    (BIT(2))
#define USB_AHBERR4_M  (USB_AHBERR4_V << USB_AHBERR4_S)
#define USB_AHBERR4_V  0x00000001
#define USB_AHBERR4_S  2
/** USB_SETUP4 : R/W1C; bitpos: [3]; default: 0;
 *  SETUP Phase Done
 *  0x0 : No SETUP Phase Done
 *  0x1 : SETUP Phase Done
 */
#define USB_SETUP4    (BIT(3))
#define USB_SETUP4_M  (USB_SETUP4_V << USB_SETUP4_S)
#define USB_SETUP4_V  0x00000001
#define USB_SETUP4_S  3
/** USB_OUTTKNEPDIS4 : R/W1C; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled
 *  0x0 : No OUT Token Received When Endpoint Disabled
 *  0x1 : OUT Token Received When Endpoint Disabled
 */
#define USB_OUTTKNEPDIS4    (BIT(4))
#define USB_OUTTKNEPDIS4_M  (USB_OUTTKNEPDIS4_V << USB_OUTTKNEPDIS4_S)
#define USB_OUTTKNEPDIS4_V  0x00000001
#define USB_OUTTKNEPDIS4_S  4
/** USB_STSPHSERCVD4 : R/W1C; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write
 *  0x0 : No Status Phase Received for Control Write
 *  0x1 : Status Phase Received for Control Write
 */
#define USB_STSPHSERCVD4    (BIT(5))
#define USB_STSPHSERCVD4_M  (USB_STSPHSERCVD4_V << USB_STSPHSERCVD4_S)
#define USB_STSPHSERCVD4_V  0x00000001
#define USB_STSPHSERCVD4_S  5
/** USB_BACK2BACKSETUP4 : R/W1C; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received
 *  0x0 : No Back-to-Back SETUP Packets Received
 *  0x1 : Back-to-Back SETUP Packets Received
 */
#define USB_BACK2BACKSETUP4    (BIT(6))
#define USB_BACK2BACKSETUP4_M  (USB_BACK2BACKSETUP4_V << USB_BACK2BACKSETUP4_S)
#define USB_BACK2BACKSETUP4_V  0x00000001
#define USB_BACK2BACKSETUP4_S  6
/** USB_OUTPKTERR4 : R/W1C; bitpos: [8]; default: 0;
 *  OUT Packet Error
 *  0x0 : No OUT Packet Error
 *  0x1 : OUT Packet Error
 */
#define USB_OUTPKTERR4    (BIT(8))
#define USB_OUTPKTERR4_M  (USB_OUTPKTERR4_V << USB_OUTPKTERR4_S)
#define USB_OUTPKTERR4_V  0x00000001
#define USB_OUTPKTERR4_S  8
/** USB_BNAINTR4 : R/W1C; bitpos: [9]; default: 0;
 *  Buffer Not Available Interrupt
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_BNAINTR4    (BIT(9))
#define USB_BNAINTR4_M  (USB_BNAINTR4_V << USB_BNAINTR4_S)
#define USB_BNAINTR4_V  0x00000001
#define USB_BNAINTR4_S  9
/** USB_PKTDRPSTS4 : R/W1C; bitpos: [11]; default: 0;
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status interrupt
 */
#define USB_PKTDRPSTS4    (BIT(11))
#define USB_PKTDRPSTS4_M  (USB_PKTDRPSTS4_V << USB_PKTDRPSTS4_S)
#define USB_PKTDRPSTS4_V  0x00000001
#define USB_PKTDRPSTS4_S  11
/** USB_BBLEERR4 : R/W1C; bitpos: [12]; default: 0;
 *  0x0 : No BbleErr interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_BBLEERR4    (BIT(12))
#define USB_BBLEERR4_M  (USB_BBLEERR4_V << USB_BBLEERR4_S)
#define USB_BBLEERR4_V  0x00000001
#define USB_BBLEERR4_S  12
/** USB_NAKINTRPT4 : R/W1C; bitpos: [13]; default: 0;
 *  0x0 : No NAK interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_NAKINTRPT4    (BIT(13))
#define USB_NAKINTRPT4_M  (USB_NAKINTRPT4_V << USB_NAKINTRPT4_S)
#define USB_NAKINTRPT4_V  0x00000001
#define USB_NAKINTRPT4_S  13
/** USB_NYEPINTRPT4 : R/W1C; bitpos: [14]; default: 0;
 *  0x0 : No NYET interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_NYEPINTRPT4    (BIT(14))
#define USB_NYEPINTRPT4_M  (USB_NYEPINTRPT4_V << USB_NYEPINTRPT4_S)
#define USB_NYEPINTRPT4_V  0x00000001
#define USB_NYEPINTRPT4_S  14
/** USB_STUPPKTRCVD4 : R/W1C; bitpos: [15]; default: 0;
 *  0x0 : No Setup packet received
 *  0x1 : Setup packet received
 */
#define USB_STUPPKTRCVD4    (BIT(15))
#define USB_STUPPKTRCVD4_M  (USB_STUPPKTRCVD4_V << USB_STUPPKTRCVD4_S)
#define USB_STUPPKTRCVD4_V  0x00000001
#define USB_STUPPKTRCVD4_S  15


/** USB_DOEPINT5_REG register
 *  Device OUT Endpoint 5 Interrupt Register
 */
#define USB_DOEPINT5_REG (SOC_DPORT_USB_BASE + 0xba8)
/** USB_XFERCOMPL5 : R/W1C; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Complete Interrupt
 */
#define USB_XFERCOMPL5    (BIT(0))
#define USB_XFERCOMPL5_M  (USB_XFERCOMPL5_V << USB_XFERCOMPL5_S)
#define USB_XFERCOMPL5_V  0x00000001
#define USB_XFERCOMPL5_S  0
/** USB_EPDISBLD5 : R/W1C; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_EPDISBLD5    (BIT(1))
#define USB_EPDISBLD5_M  (USB_EPDISBLD5_V << USB_EPDISBLD5_S)
#define USB_EPDISBLD5_V  0x00000001
#define USB_EPDISBLD5_S  1
/** USB_AHBERR5 : R/W1C; bitpos: [2]; default: 0;
 *  AHB Error
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_AHBERR5    (BIT(2))
#define USB_AHBERR5_M  (USB_AHBERR5_V << USB_AHBERR5_S)
#define USB_AHBERR5_V  0x00000001
#define USB_AHBERR5_S  2
/** USB_SETUP5 : R/W1C; bitpos: [3]; default: 0;
 *  SETUP Phase Done
 *  0x0 : No SETUP Phase Done
 *  0x1 : SETUP Phase Done
 */
#define USB_SETUP5    (BIT(3))
#define USB_SETUP5_M  (USB_SETUP5_V << USB_SETUP5_S)
#define USB_SETUP5_V  0x00000001
#define USB_SETUP5_S  3
/** USB_OUTTKNEPDIS5 : R/W1C; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled
 *  0x0 : No OUT Token Received When Endpoint Disabled
 *  0x1 : OUT Token Received When Endpoint Disabled
 */
#define USB_OUTTKNEPDIS5    (BIT(4))
#define USB_OUTTKNEPDIS5_M  (USB_OUTTKNEPDIS5_V << USB_OUTTKNEPDIS5_S)
#define USB_OUTTKNEPDIS5_V  0x00000001
#define USB_OUTTKNEPDIS5_S  4
/** USB_STSPHSERCVD5 : R/W1C; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write
 *  0x0 : No Status Phase Received for Control Write
 *  0x1 : Status Phase Received for Control Write
 */
#define USB_STSPHSERCVD5    (BIT(5))
#define USB_STSPHSERCVD5_M  (USB_STSPHSERCVD5_V << USB_STSPHSERCVD5_S)
#define USB_STSPHSERCVD5_V  0x00000001
#define USB_STSPHSERCVD5_S  5
/** USB_BACK2BACKSETUP5 : R/W1C; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received
 *  0x0 : No Back-to-Back SETUP Packets Received
 *  0x1 : Back-to-Back SETUP Packets Received
 */
#define USB_BACK2BACKSETUP5    (BIT(6))
#define USB_BACK2BACKSETUP5_M  (USB_BACK2BACKSETUP5_V << USB_BACK2BACKSETUP5_S)
#define USB_BACK2BACKSETUP5_V  0x00000001
#define USB_BACK2BACKSETUP5_S  6
/** USB_OUTPKTERR5 : R/W1C; bitpos: [8]; default: 0;
 *  OUT Packet Error
 *  0x0 : No OUT Packet Error
 *  0x1 : OUT Packet Error
 */
#define USB_OUTPKTERR5    (BIT(8))
#define USB_OUTPKTERR5_M  (USB_OUTPKTERR5_V << USB_OUTPKTERR5_S)
#define USB_OUTPKTERR5_V  0x00000001
#define USB_OUTPKTERR5_S  8
/** USB_BNAINTR5 : R/W1C; bitpos: [9]; default: 0;
 *  Buffer Not Available Interrupt
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_BNAINTR5    (BIT(9))
#define USB_BNAINTR5_M  (USB_BNAINTR5_V << USB_BNAINTR5_S)
#define USB_BNAINTR5_V  0x00000001
#define USB_BNAINTR5_S  9
/** USB_PKTDRPSTS5 : R/W1C; bitpos: [11]; default: 0;
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status interrupt
 */
#define USB_PKTDRPSTS5    (BIT(11))
#define USB_PKTDRPSTS5_M  (USB_PKTDRPSTS5_V << USB_PKTDRPSTS5_S)
#define USB_PKTDRPSTS5_V  0x00000001
#define USB_PKTDRPSTS5_S  11
/** USB_BBLEERR5 : R/W1C; bitpos: [12]; default: 0;
 *  0x0 : No BbleErr interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_BBLEERR5    (BIT(12))
#define USB_BBLEERR5_M  (USB_BBLEERR5_V << USB_BBLEERR5_S)
#define USB_BBLEERR5_V  0x00000001
#define USB_BBLEERR5_S  12
/** USB_NAKINTRPT5 : R/W1C; bitpos: [13]; default: 0;
 *  0x0 : No NAK interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_NAKINTRPT5    (BIT(13))
#define USB_NAKINTRPT5_M  (USB_NAKINTRPT5_V << USB_NAKINTRPT5_S)
#define USB_NAKINTRPT5_V  0x00000001
#define USB_NAKINTRPT5_S  13
/** USB_NYEPINTRPT5 : R/W1C; bitpos: [14]; default: 0;
 *  0x0 : No NYET interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_NYEPINTRPT5    (BIT(14))
#define USB_NYEPINTRPT5_M  (USB_NYEPINTRPT5_V << USB_NYEPINTRPT5_S)
#define USB_NYEPINTRPT5_V  0x00000001
#define USB_NYEPINTRPT5_S  14
/** USB_STUPPKTRCVD5 : R/W1C; bitpos: [15]; default: 0;
 *  0x0 : No Setup packet received
 *  0x1 : Setup packet received
 */
#define USB_STUPPKTRCVD5    (BIT(15))
#define USB_STUPPKTRCVD5_M  (USB_STUPPKTRCVD5_V << USB_STUPPKTRCVD5_S)
#define USB_STUPPKTRCVD5_V  0x00000001
#define USB_STUPPKTRCVD5_S  15


/** USB_DOEPINT6_REG register
 *  Device OUT Endpoint 6 Interrupt Register
 */
#define USB_DOEPINT6_REG (SOC_DPORT_USB_BASE + 0xbc8)
/** USB_XFERCOMPL6 : R/W1C; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt
 *  0x0 : No Transfer Complete Interrupt
 *  0x1 : Transfer Complete Interrupt
 */
#define USB_XFERCOMPL6    (BIT(0))
#define USB_XFERCOMPL6_M  (USB_XFERCOMPL6_V << USB_XFERCOMPL6_S)
#define USB_XFERCOMPL6_V  0x00000001
#define USB_XFERCOMPL6_S  0
/** USB_EPDISBLD6 : R/W1C; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt
 *  0x0 : No Endpoint Disabled Interrupt
 *  0x1 : Endpoint Disabled Interrupt
 */
#define USB_EPDISBLD6    (BIT(1))
#define USB_EPDISBLD6_M  (USB_EPDISBLD6_V << USB_EPDISBLD6_S)
#define USB_EPDISBLD6_V  0x00000001
#define USB_EPDISBLD6_S  1
/** USB_AHBERR6 : R/W1C; bitpos: [2]; default: 0;
 *  AHB Error
 *  0x0 : No AHB Error Interrupt
 *  0x1 : AHB Error interrupt
 */
#define USB_AHBERR6    (BIT(2))
#define USB_AHBERR6_M  (USB_AHBERR6_V << USB_AHBERR6_S)
#define USB_AHBERR6_V  0x00000001
#define USB_AHBERR6_S  2
/** USB_SETUP6 : R/W1C; bitpos: [3]; default: 0;
 *  SETUP Phase Done
 *  0x0 : No SETUP Phase Done
 *  0x1 : SETUP Phase Done
 */
#define USB_SETUP6    (BIT(3))
#define USB_SETUP6_M  (USB_SETUP6_V << USB_SETUP6_S)
#define USB_SETUP6_V  0x00000001
#define USB_SETUP6_S  3
/** USB_OUTTKNEPDIS6 : R/W1C; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled
 *  0x0 : No OUT Token Received When Endpoint Disabled
 *  0x1 : OUT Token Received When Endpoint Disabled
 */
#define USB_OUTTKNEPDIS6    (BIT(4))
#define USB_OUTTKNEPDIS6_M  (USB_OUTTKNEPDIS6_V << USB_OUTTKNEPDIS6_S)
#define USB_OUTTKNEPDIS6_V  0x00000001
#define USB_OUTTKNEPDIS6_S  4
/** USB_STSPHSERCVD6 : R/W1C; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write
 *  0x0 : No Status Phase Received for Control Write
 *  0x1 : Status Phase Received for Control Write
 */
#define USB_STSPHSERCVD6    (BIT(5))
#define USB_STSPHSERCVD6_M  (USB_STSPHSERCVD6_V << USB_STSPHSERCVD6_S)
#define USB_STSPHSERCVD6_V  0x00000001
#define USB_STSPHSERCVD6_S  5
/** USB_BACK2BACKSETUP6 : R/W1C; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received
 *  0x0 : No Back-to-Back SETUP Packets Received
 *  0x1 : Back-to-Back SETUP Packets Received
 */
#define USB_BACK2BACKSETUP6    (BIT(6))
#define USB_BACK2BACKSETUP6_M  (USB_BACK2BACKSETUP6_V << USB_BACK2BACKSETUP6_S)
#define USB_BACK2BACKSETUP6_V  0x00000001
#define USB_BACK2BACKSETUP6_S  6
/** USB_OUTPKTERR6 : R/W1C; bitpos: [8]; default: 0;
 *  OUT Packet Error
 *  0x0 : No OUT Packet Error
 *  0x1 : OUT Packet Error
 */
#define USB_OUTPKTERR6    (BIT(8))
#define USB_OUTPKTERR6_M  (USB_OUTPKTERR6_V << USB_OUTPKTERR6_S)
#define USB_OUTPKTERR6_V  0x00000001
#define USB_OUTPKTERR6_S  8
/** USB_BNAINTR6 : R/W1C; bitpos: [9]; default: 0;
 *  Buffer Not Available Interrupt
 *  0x0 : No BNA interrupt
 *  0x1 : BNA interrupt
 */
#define USB_BNAINTR6    (BIT(9))
#define USB_BNAINTR6_M  (USB_BNAINTR6_V << USB_BNAINTR6_S)
#define USB_BNAINTR6_V  0x00000001
#define USB_BNAINTR6_S  9
/** USB_PKTDRPSTS6 : R/W1C; bitpos: [11]; default: 0;
 *  0x0 : No interrupt
 *  0x1 : Packet Drop Status interrupt
 */
#define USB_PKTDRPSTS6    (BIT(11))
#define USB_PKTDRPSTS6_M  (USB_PKTDRPSTS6_V << USB_PKTDRPSTS6_S)
#define USB_PKTDRPSTS6_V  0x00000001
#define USB_PKTDRPSTS6_S  11
/** USB_BBLEERR6 : R/W1C; bitpos: [12]; default: 0;
 *  0x0 : No BbleErr interrupt
 *  0x1 : BbleErr interrupt
 */
#define USB_BBLEERR6    (BIT(12))
#define USB_BBLEERR6_M  (USB_BBLEERR6_V << USB_BBLEERR6_S)
#define USB_BBLEERR6_V  0x00000001
#define USB_BBLEERR6_S  12
/** USB_NAKINTRPT6 : R/W1C; bitpos: [13]; default: 0;
 *  0x0 : No NAK interrupt
 *  0x1 : NAK Interrupt
 */
#define USB_NAKINTRPT6    (BIT(13))
#define USB_NAKINTRPT6_M  (USB_NAKINTRPT6_V << USB_NAKINTRPT6_S)
#define USB_NAKINTRPT6_V  0x00000001
#define USB_NAKINTRPT6_S  13
/** USB_NYEPINTRPT6 : R/W1C; bitpos: [14]; default: 0;
 *  0x0 : No NYET interrupt
 *  0x1 : NYET Interrupt
 */
#define USB_NYEPINTRPT6    (BIT(14))
#define USB_NYEPINTRPT6_M  (USB_NYEPINTRPT6_V << USB_NYEPINTRPT6_S)
#define USB_NYEPINTRPT6_V  0x00000001
#define USB_NYEPINTRPT6_S  14
/** USB_STUPPKTRCVD6 : R/W1C; bitpos: [15]; default: 0;
 *  0x0 : No Setup packet received
 *  0x1 : Setup packet received
 */
#define USB_STUPPKTRCVD6    (BIT(15))
#define USB_STUPPKTRCVD6_M  (USB_STUPPKTRCVD6_V << USB_STUPPKTRCVD6_S)
#define USB_STUPPKTRCVD6_V  0x00000001
#define USB_STUPPKTRCVD6_S  15




/** configuration registers */
/** USB_GAHBCFG_REG register
 *  AHB Configuration Register
 */
#define USB_GAHBCFG_REG (SOC_DPORT_USB_BASE + 0x8)
/** USB_GLBLLNTRMSK : R/W; bitpos: [0]; default: 0;
 *  1'b0: Mask the interrupt assertion to the application.
 *  1'b1: Unmask the interrupt assertion to the application
 */
#define USB_GLBLLNTRMSK    (BIT(0))
#define USB_GLBLLNTRMSK_M  (USB_GLBLLNTRMSK_V << USB_GLBLLNTRMSK_S)
#define USB_GLBLLNTRMSK_V  0x00000001
#define USB_GLBLLNTRMSK_S  0
/** USB_HBSTLEN : R/W; bitpos: [5:1]; default: 0;
 *  this field is used in Internal DMA modes
 *  4'b0000 Single
 *  4'b0001: INCR
 *  4'b0011 INCR4
 *  4'b0101 INCR8
 *  4'b0111 INCR16
 *  Others: Reserved
 */
#define USB_HBSTLEN    0x0000000F
#define USB_HBSTLEN_M  (USB_HBSTLEN_V << USB_HBSTLEN_S)
#define USB_HBSTLEN_V  0x0000000F
#define USB_HBSTLEN_S  1
/** USB_DMAEN : R/W; bitpos: [5]; default: 0;
 *  This bit is always 0 when Slave-Only mode has been selected
 *  1'b0:Core operates in Slave mode
 *  1'b1:Core operates in a DMA mode
 */
#define USB_DMAEN    (BIT(5))
#define USB_DMAEN_M  (USB_DMAEN_V << USB_DMAEN_S)
#define USB_DMAEN_V  0x00000001
#define USB_DMAEN_S  5
/** USB_NPTXFEMPLVL : R/W; bitpos: [7]; default: 0;
 *  Non-Periodic TxFIFO Empty Level
 *  1'b0: DIEPINTn_REG.REG_TXFEMP interrupt indicates that the Non-Periodic TxFIFO is
 *  half empty or that the IN Endpoint TxFIFO is half empty
 *  1'b1: GINTSTS_REG.USB_NPTXFEMP interrupt indicates that the Non-Periodic TxFIFO is
 *  completely empty or that the IN Endpoint TxFIFO is completely empty
 */
#define USB_NPTXFEMPLVL    (BIT(7))
#define USB_NPTXFEMPLVL_M  (USB_NPTXFEMPLVL_V << USB_NPTXFEMPLVL_S)
#define USB_NPTXFEMPLVL_V  0x00000001
#define USB_NPTXFEMPLVL_S  7
/** USB_PTXFEMPLVL : R/W; bitpos: [8]; default: 0;
 *  Indicates when the Periodic TxFIFO Empty Interrupt bit in the Core Interrupt
 *  register (GINTSTS_REG.USB_PTXFEMP) is triggered. This bit is used only in Slave
 *  mode
 *  1'b0: GINTSTS_REG.USB_PTXFEMP interrupt indicates that the Periodic TxFIFO is half
 *  empty
 *  1'b1: GINTSTS_REG.USB_PTXFEMP interrupt indicates that the Periodic TxFIFO is
 *  completely empty
 */
#define USB_PTXFEMPLVL    (BIT(8))
#define USB_PTXFEMPLVL_M  (USB_PTXFEMPLVL_V << USB_PTXFEMPLVL_S)
#define USB_PTXFEMPLVL_V  0x00000001
#define USB_PTXFEMPLVL_S  8
/** USB_REMMEMSUPP : R/W; bitpos: [21]; default: 0;
 *  Remote Memory Support (RemMemSupp) This bit is programmed to enable the
 *  functionality to wait for thesystem DMA Done Signal for the DMA Write Transfers
 *  1'b0:Remote Memory Support Feature disabled
 *  1'b1:Remote Memory Support Feature enabled
 */
#define USB_REMMEMSUPP    (BIT(21))
#define USB_REMMEMSUPP_M  (USB_REMMEMSUPP_V << USB_REMMEMSUPP_S)
#define USB_REMMEMSUPP_V  0x00000001
#define USB_REMMEMSUPP_S  21
/** USB_NOTIALLDMAWRIT : R/W; bitpos: [22]; default: 0;
 *  Notify All DMA Write Transactions (NotiAllDmaWrit) This bit is programmed to enable
 *  the System DMA Done functionality for all the DMA write Transactions corresponding
 *  to the Channel/Endpoint. This bit is valid only when GAHBCFG.RemMemSupp is set to 1
 */
#define USB_NOTIALLDMAWRIT    (BIT(22))
#define USB_NOTIALLDMAWRIT_M  (USB_NOTIALLDMAWRIT_V << USB_NOTIALLDMAWRIT_S)
#define USB_NOTIALLDMAWRIT_V  0x00000001
#define USB_NOTIALLDMAWRIT_S  22
/** USB_AHBSINGLE : R/W; bitpos: [23]; default: 0;
 *  AHB Single Support (AHBSingle) This bit when programmed supports Single transfers
 *  for the remaining data in a transfer when the core is operating in DMA mode
 *  1'b0: The remaining data in the transfer is sent using INCR burst size
 *  1'b1: The remaining data in the transfer is sent using Single burst size
 */
#define USB_AHBSINGLE    (BIT(23))
#define USB_AHBSINGLE_M  (USB_AHBSINGLE_V << USB_AHBSINGLE_S)
#define USB_AHBSINGLE_V  0x00000001
#define USB_AHBSINGLE_S  23
/** USB_INVDESCENDIANESS : R/W; bitpos: [24]; default: 0;
 *  Invert Descriptor Endianess
 *  1'b0: Descriptor Endianness is same as AHB Master Endianness
 *  1'b1:Invert Descriptor Endianess according to AHB Master endianness
 */
#define USB_INVDESCENDIANESS    (BIT(24))
#define USB_INVDESCENDIANESS_M  (USB_INVDESCENDIANESS_V << USB_INVDESCENDIANESS_S)
#define USB_INVDESCENDIANESS_V  0x00000001
#define USB_INVDESCENDIANESS_S  24


/** USB_GUSBCFG_REG register
 *  USB Configuration Register
 */
#define USB_GUSBCFG_REG (SOC_DPORT_USB_BASE + 0xc)
/** USB_TOUTCAL : R/W; bitpos: [3:0]; default: 0;
 *  FS Timeout Calibration
 */
#define USB_TOUTCAL    0x00000007
#define USB_TOUTCAL_M  (USB_TOUTCAL_V << USB_TOUTCAL_S)
#define USB_TOUTCAL_V  0x00000007
#define USB_TOUTCAL_S  0
/** USB_PHYIF : R/W; bitpos: [3]; default: 0;
 *  The application uses this bit to configure the core to support a
 *  UTMI+ PHY with an 8- or 16-bit interface. When a ULPI PHY is
 *  chosen, this must be Set to 8-bit mode
 *  1'b0: 8 bits
 *  1'b1: 16 bits
 */
#define USB_PHYIF    (BIT(3))
#define USB_PHYIF_M  (USB_PHYIF_V << USB_PHYIF_S)
#define USB_PHYIF_V  0x00000001
#define USB_PHYIF_S  3
/** USB_ULPI_UTMI_SEL : RO; bitpos: [4]; default: 0;
 *  1'b0: UTMI+ Interface
 *  1'b1: ULPI Interface
 */
#define USB_ULPI_UTMI_SEL    (BIT(4))
#define USB_ULPI_UTMI_SEL_M  (USB_ULPI_UTMI_SEL_V << USB_ULPI_UTMI_SEL_S)
#define USB_ULPI_UTMI_SEL_V  0x00000001
#define USB_ULPI_UTMI_SEL_S  4
/** USB_FSINTF : R/W; bitpos: [5]; default: 0;
 *  1'b0: 6-pin unidirectional full-speed serial interface
 *  1'b1: 3-pin bidirectional full-speed serial interface
 */
#define USB_FSINTF    (BIT(5))
#define USB_FSINTF_M  (USB_FSINTF_V << USB_FSINTF_S)
#define USB_FSINTF_V  0x00000001
#define USB_FSINTF_S  5
/** USB_PHYSEL : RO; bitpos: [6]; default: 1;
 *  1'b0: USB 2.0 high-speed UTMI+ or ULPI PHY
 *  1'b1: USB 1.1 full-speed serial transceiver
 */
#define USB_PHYSEL    (BIT(6))
#define USB_PHYSEL_M  (USB_PHYSEL_V << USB_PHYSEL_S)
#define USB_PHYSEL_V  0x00000001
#define USB_PHYSEL_S  6
/** USB_SRPCAP : R/W; bitpos: [8]; default: 0;
 *  1'b0: SRP capability is not enabled
 *  1'b1: SRP capability is enabled
 */
#define USB_SRPCAP    (BIT(8))
#define USB_SRPCAP_M  (USB_SRPCAP_V << USB_SRPCAP_S)
#define USB_SRPCAP_V  0x00000001
#define USB_SRPCAP_S  8
/** USB_HNPCAP : R/W; bitpos: [9]; default: 0;
 *  1'b0: HNP capability is not enabled
 *  1'b1: HNP capability is enabled
 */
#define USB_HNPCAP    (BIT(9))
#define USB_HNPCAP_M  (USB_HNPCAP_V << USB_HNPCAP_S)
#define USB_HNPCAP_V  0x00000001
#define USB_HNPCAP_S  9
/** USB_USBTRDTIM : R/W; bitpos: [14:10]; default: 5;
 *  4'h5: When the MAC interface is 16-bit UTMI+
 *  4'h9: When the MAC interface is 8-bit UTMI+
 */
#define USB_USBTRDTIM    0x0000000F
#define USB_USBTRDTIM_M  (USB_USBTRDTIM_V << USB_USBTRDTIM_S)
#define USB_USBTRDTIM_V  0x0000000F
#define USB_USBTRDTIM_S  10
/** USB_TERMSELDLPULSE : R/W; bitpos: [22]; default: 0;
 *  TermSel DLine Pulsing Selection
 *  1'b0: Data line pulsing using utmi_txvalid (Default)
 *  1'b1: Data line pulsing using utmi_termsel
 */
#define USB_TERMSELDLPULSE    (BIT(22))
#define USB_TERMSELDLPULSE_M  (USB_TERMSELDLPULSE_V << USB_TERMSELDLPULSE_S)
#define USB_TERMSELDLPULSE_V  0x00000001
#define USB_TERMSELDLPULSE_S  22
/** USB_TXENDDELAY : R/W; bitpos: [28]; default: 0;
 *  1'b0 : Normal Mode
 *  1'b1 : Tx End delay
 */
#define USB_TXENDDELAY    (BIT(28))
#define USB_TXENDDELAY_M  (USB_TXENDDELAY_V << USB_TXENDDELAY_S)
#define USB_TXENDDELAY_V  0x00000001
#define USB_TXENDDELAY_S  28
/** USB_FORCEHSTMODE : R/W; bitpos: [29]; default: 0;
 *  1'b0 : Normal Mode
 *  1'b1 : Force Host Mode
 */
#define USB_FORCEHSTMODE    (BIT(29))
#define USB_FORCEHSTMODE_M  (USB_FORCEHSTMODE_V << USB_FORCEHSTMODE_S)
#define USB_FORCEHSTMODE_V  0x00000001
#define USB_FORCEHSTMODE_S  29
/** USB_FORCEDEVMODE : R/W; bitpos: [30]; default: 0;
 *  1'b0 : Normal Mode
 *  1'b1 : Force Device Mode
 */
#define USB_FORCEDEVMODE    (BIT(30))
#define USB_FORCEDEVMODE_M  (USB_FORCEDEVMODE_V << USB_FORCEDEVMODE_S)
#define USB_FORCEDEVMODE_V  0x00000001
#define USB_FORCEDEVMODE_S  30
/** USB_CORRUPTTXPKT : R/W; bitpos: [31]; default: 0;
 *  This bit is for debug purposes only. Never Set this bit to 1. The application
 *  should always write 1'b0 to this bit
 *  1'b0:Normal Mode
 *  1'b1:Debug Mode
 */
#define USB_CORRUPTTXPKT    (BIT(31))
#define USB_CORRUPTTXPKT_M  (USB_CORRUPTTXPKT_V << USB_CORRUPTTXPKT_S)
#define USB_CORRUPTTXPKT_V  0x00000001
#define USB_CORRUPTTXPKT_S  31


/** USB_GRSTCTL_REG register
 *  Reset Register
 */
#define USB_GRSTCTL_REG (SOC_DPORT_USB_BASE + 0x10)
/** USB_CSFTRST : R_WS_SC; bitpos: [0]; default: 0;
 *  Core Soft Reset
 */
#define USB_CSFTRST    (BIT(0))
#define USB_CSFTRST_M  (USB_CSFTRST_V << USB_CSFTRST_S)
#define USB_CSFTRST_V  0x00000001
#define USB_CSFTRST_S  0
/** USB_PIUFSSFTRST : R_WS_SC; bitpos: [1]; default: 0;
 *  1'b0: No Reset
 *  1'b1:PIU FS Dedicated Controller Soft Reset
 */
#define USB_PIUFSSFTRST    (BIT(1))
#define USB_PIUFSSFTRST_M  (USB_PIUFSSFTRST_V << USB_PIUFSSFTRST_S)
#define USB_PIUFSSFTRST_V  0x00000001
#define USB_PIUFSSFTRST_S  1
/** USB_FRMCNTRRST : R/W1S; bitpos: [2]; default: 0;
 *  Host only. Host Frame Counter Reset.The application writes this bit to reset the
 *  (micro)Frame number counter inside the core. When the (micro)Frame counter is
 *  reset, the subsequent SOF sent out by the core has a (micro)Frame number of 0
 */
#define USB_FRMCNTRRST    (BIT(2))
#define USB_FRMCNTRRST_M  (USB_FRMCNTRRST_V << USB_FRMCNTRRST_S)
#define USB_FRMCNTRRST_V  0x00000001
#define USB_FRMCNTRRST_S  2
/** USB_RXFFLSH : R/W1S; bitpos: [4]; default: 0;
 *  RxFIFO Flush. The application can flush the entire RxFIFO using this bit, but must
 *  first ensure that the core is not in the middle of a transaction.The application
 *  must only write to this bit after checking that the controller is neither reading
 *  from the RxFIFO nor writing to the RxFIFO
 *  1'b0:Does not flush the entire RxFIFO
 *  1'b1:flushes the entire RxFIFO
 */
#define USB_RXFFLSH    (BIT(4))
#define USB_RXFFLSH_M  (USB_RXFFLSH_V << USB_RXFFLSH_S)
#define USB_RXFFLSH_V  0x00000001
#define USB_RXFFLSH_S  4
/** USB_TXFFLSH : R/W1S; bitpos: [5]; default: 0;
 *  TxFIFO Flush.This bit selectively flushes a single or all transmit FIFOs, but
 *  cannot do so If the core is in the midst of a transaction.The application must
 *  write this bit only after checking that the core is neither writing to the TxFIFO
 *  nor reading from the TxFIFO.
 */
#define USB_TXFFLSH    (BIT(5))
#define USB_TXFFLSH_M  (USB_TXFFLSH_V << USB_TXFFLSH_S)
#define USB_TXFFLSH_V  0x00000001
#define USB_TXFFLSH_S  5
/** USB_TXFNUM : R/W; bitpos: [11:6]; default: 0;
 *  TxFIFO Number.This is the FIFO number that must be flushed using the TxFIFO Flush
 *  bit. This field must not be changed until the core clears the TxFIFO Flush bit
 */
#define USB_TXFNUM    0x0000001F
#define USB_TXFNUM_M  (USB_TXFNUM_V << USB_TXFNUM_S)
#define USB_TXFNUM_V  0x0000001F
#define USB_TXFNUM_S  6
/** USB_DMAREQ : RO; bitpos: [30]; default: 0;
 *  DMA Request Signal
 *  1'b0:No DMA request
 *  1'b1:DMA request is in progress
 */
#define USB_DMAREQ    (BIT(30))
#define USB_DMAREQ_M  (USB_DMAREQ_V << USB_DMAREQ_S)
#define USB_DMAREQ_V  0x00000001
#define USB_DMAREQ_S  30
/** USB_AHBIDLE : RO; bitpos: [31]; default: 0;
 *  AHB Master Idle
 *  1'b0:Not Idle
 *  1'b1:AHB Master Idle
 */
#define USB_AHBIDLE    (BIT(31))
#define USB_AHBIDLE_M  (USB_AHBIDLE_V << USB_AHBIDLE_S)
#define USB_AHBIDLE_V  0x00000001
#define USB_AHBIDLE_S  31


/** USB_GRXFSIZ_REG register
 *  Receive FIFO Size Register
 */
#define USB_GRXFSIZ_REG (SOC_DPORT_USB_BASE + 0x24)
/** USB_RXFDEP : R/W; bitpos: [16:0]; default: 256;
 *  RxFIFO Depth.This value is in terms of 32-bit words.Minimum value is 16,Maximum
 *  value is 32,768
 */
#define USB_RXFDEP    0x0000FFFF
#define USB_RXFDEP_M  (USB_RXFDEP_V << USB_RXFDEP_S)
#define USB_RXFDEP_V  0x0000FFFF
#define USB_RXFDEP_S  0


/** USB_GNPTXFSIZ_REG register
 *  Non-periodic Transmit FIFO Size Register
 */
#define USB_GNPTXFSIZ_REG (SOC_DPORT_USB_BASE + 0x28)
/** USB_NPTXFSTADDR : R/W; bitpos: [16:0]; default: 256;
 *  The NPTxFStAddr field description is valid only for host mode.This field contains
 *  the memory start address for Non-periodic Transmit FIFO RAM.
 */
#define USB_NPTXFSTADDR    0x0000FFFF
#define USB_NPTXFSTADDR_M  (USB_NPTXFSTADDR_V << USB_NPTXFSTADDR_S)
#define USB_NPTXFSTADDR_V  0x0000FFFF
#define USB_NPTXFSTADDR_S  0
/** USB_NPTXFDEP : R/W; bitpos: [32:16]; default: 256;
 *  The NPTxFDep field description is valid only for host mode or device mode when
 *  OTG_EN_DED_TX_FIFO=0.Minimum value is 16,Maximum value is 32,768.
 */
#define USB_NPTXFDEP    0x0000FFFF
#define USB_NPTXFDEP_M  (USB_NPTXFDEP_V << USB_NPTXFDEP_S)
#define USB_NPTXFDEP_V  0x0000FFFF
#define USB_NPTXFDEP_S  16


/** USB_GNPTXSTS_REG register
 *  Non-periodic Transmit FIFO/Queue Status Register
 */
#define USB_GNPTXSTS_REG (SOC_DPORT_USB_BASE + 0x2c)
/** USB_NPTXFSPCAVAIL : RO; bitpos: [16:0]; default: 256;
 *  Non-periodic TxFIFO Space Avail.Indicates the amount of free space available in the
 *  Non-periodic TxFIFO.Values are in terms of 32-bit words.
 */
#define USB_NPTXFSPCAVAIL    0x0000FFFF
#define USB_NPTXFSPCAVAIL_M  (USB_NPTXFSPCAVAIL_V << USB_NPTXFSPCAVAIL_S)
#define USB_NPTXFSPCAVAIL_V  0x0000FFFF
#define USB_NPTXFSPCAVAIL_S  0
/** USB_NPTXQSPCAVAIL : RO; bitpos: [20:16]; default: 4;
 *  Non-periodic Transmit Request Queue Space Available.Indicates the amount of free
 *  space available in the Non-periodic Transmit Request Queue. This queue holds both
 *  IN and OUT requests in Host mode. Device mode has only IN requests.
 */
#define USB_NPTXQSPCAVAIL    0x0000000F
#define USB_NPTXQSPCAVAIL_M  (USB_NPTXQSPCAVAIL_V << USB_NPTXQSPCAVAIL_S)
#define USB_NPTXQSPCAVAIL_V  0x0000000F
#define USB_NPTXQSPCAVAIL_S  16
/** USB_NPTXQTOP : RO; bitpos: [31:24]; default: 0;
 *  Top of the Non-periodic Transmit Request Queue.
 *  Bits [30:27]: Channel/endpoint number.
 *  Bits [26:25]:  2'b00: IN/OUT token   2'b01: Zero-length transmit packet (device
 *  IN/host OUT)   2'b10: PING/CSPLIT token  2'b11: Channel halt command.
 *  Bit [24]: Terminate (last Entry for selected channel/endpoint).
 */
#define USB_NPTXQTOP    0x0000007F
#define USB_NPTXQTOP_M  (USB_NPTXQTOP_V << USB_NPTXQTOP_S)
#define USB_NPTXQTOP_V  0x0000007F
#define USB_NPTXQTOP_S  24


/** USB_HCTSIZ0_REG register
 *  Host Channel 0Transfer Size Register
 */
#define USB_HCTSIZ0_REG (SOC_DPORT_USB_BASE + 0x510)
/** USB_H_XFERSIZE0 : R/W; bitpos: [19:0]; default: 0;
 *  Non-Scatter/Gather DMA Mode: Transfer Size.
 *  Scatter/Gather DMA Mode:
 *  [18:16]: Reserved
 *  [15:8]: NTD (Number of Transfer Descriptors)
 */
#define USB_H_XFERSIZE0    0x0007FFFF
#define USB_H_XFERSIZE0_M  (USB_H_XFERSIZE0_V << USB_H_XFERSIZE0_S)
#define USB_H_XFERSIZE0_V  0x0007FFFF
#define USB_H_XFERSIZE0_S  0
/** USB_H_PKTCNT0 : R/W; bitpos: [29:19]; default: 0;
 *  when in Non-Scatter/Gather DMA mode: Packet Count . This field is programmed by the
 *  application with the expected number of packets to be transmitted (OUT) or received
 *  (IN).
 *  Scatter/Gather DMA Mode: Reserved.
 */
#define USB_H_PKTCNT0    0x000003FF
#define USB_H_PKTCNT0_M  (USB_H_PKTCNT0_V << USB_H_PKTCNT0_S)
#define USB_H_PKTCNT0_V  0x000003FF
#define USB_H_PKTCNT0_S  19
/** USB_H_PID0 : R/W; bitpos: [31:29]; default: 0;
 *  2'b00: DATA0
 *  2'b01: DATA2
 *  2'b10: DATA1
 *  2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_H_PID0    0x00000003
#define USB_H_PID0_M  (USB_H_PID0_V << USB_H_PID0_S)
#define USB_H_PID0_V  0x00000003
#define USB_H_PID0_S  29
/** USB_H_DOPNG0 : R/W; bitpos: [31]; default: 0;
 *  This bit is used only for OUT transfers. Setting this field to 1 directs the host
 *  to do PING protocol
 *  1'b0: No ping protocol
 *  1'b1: Ping protocol
 */
#define USB_H_DOPNG0    (BIT(31))
#define USB_H_DOPNG0_M  (USB_H_DOPNG0_V << USB_H_DOPNG0_S)
#define USB_H_DOPNG0_V  0x00000001
#define USB_H_DOPNG0_S  31


/** USB_HCTSIZ1_REG register
 *  Host Channel 1Transfer Size Register
 */
#define USB_HCTSIZ1_REG (SOC_DPORT_USB_BASE + 0x530)
/** USB_H_XFERSIZE1 : R/W; bitpos: [19:0]; default: 0;
 *  Non-Scatter/Gather DMA Mode: Transfer Size.
 *  Scatter/Gather DMA Mode:
 *  [18:16]: Reserved
 *  [15:8]: NTD (Number of Transfer Descriptors)
 */
#define USB_H_XFERSIZE1    0x0007FFFF
#define USB_H_XFERSIZE1_M  (USB_H_XFERSIZE1_V << USB_H_XFERSIZE1_S)
#define USB_H_XFERSIZE1_V  0x0007FFFF
#define USB_H_XFERSIZE1_S  0
/** USB_H_PKTCNT1 : R/W; bitpos: [29:19]; default: 0;
 *  when in Non-Scatter/Gather DMA mode: Packet Count . This field is programmed by the
 *  application with the expected number of packets to be transmitted (OUT) or received
 *  (IN).
 *  Scatter/Gather DMA Mode: Reserved.
 */
#define USB_H_PKTCNT1    0x000003FF
#define USB_H_PKTCNT1_M  (USB_H_PKTCNT1_V << USB_H_PKTCNT1_S)
#define USB_H_PKTCNT1_V  0x000003FF
#define USB_H_PKTCNT1_S  19
/** USB_H_PID1 : R/W; bitpos: [31:29]; default: 0;
 *  2'b00: DATA0
 *  2'b01: DATA2
 *  2'b10: DATA1
 *  2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_H_PID1    0x00000003
#define USB_H_PID1_M  (USB_H_PID1_V << USB_H_PID1_S)
#define USB_H_PID1_V  0x00000003
#define USB_H_PID1_S  29
/** USB_H_DOPNG1 : R/W; bitpos: [31]; default: 0;
 *  This bit is used only for OUT transfers. Setting this field to 1 directs the host
 *  to do PING protocol
 *  1'b0: No ping protocol
 *  1'b1: Ping protocol
 */
#define USB_H_DOPNG1    (BIT(31))
#define USB_H_DOPNG1_M  (USB_H_DOPNG1_V << USB_H_DOPNG1_S)
#define USB_H_DOPNG1_V  0x00000001
#define USB_H_DOPNG1_S  31


/** USB_HCTSIZ2_REG register
 *  Host Channel 2Transfer Size Register
 */
#define USB_HCTSIZ2_REG (SOC_DPORT_USB_BASE + 0x550)
/** USB_H_XFERSIZE2 : R/W; bitpos: [19:0]; default: 0;
 *  Non-Scatter/Gather DMA Mode: Transfer Size.
 *  Scatter/Gather DMA Mode:
 *  [18:16]: Reserved
 *  [15:8]: NTD (Number of Transfer Descriptors)
 */
#define USB_H_XFERSIZE2    0x0007FFFF
#define USB_H_XFERSIZE2_M  (USB_H_XFERSIZE2_V << USB_H_XFERSIZE2_S)
#define USB_H_XFERSIZE2_V  0x0007FFFF
#define USB_H_XFERSIZE2_S  0
/** USB_H_PKTCNT2 : R/W; bitpos: [29:19]; default: 0;
 *  when in Non-Scatter/Gather DMA mode: Packet Count . This field is programmed by the
 *  application with the expected number of packets to be transmitted (OUT) or received
 *  (IN).
 *  Scatter/Gather DMA Mode: Reserved.
 */
#define USB_H_PKTCNT2    0x000003FF
#define USB_H_PKTCNT2_M  (USB_H_PKTCNT2_V << USB_H_PKTCNT2_S)
#define USB_H_PKTCNT2_V  0x000003FF
#define USB_H_PKTCNT2_S  19
/** USB_H_PID2 : R/W; bitpos: [31:29]; default: 0;
 *  2'b00: DATA0
 *  2'b01: DATA2
 *  2'b10: DATA1
 *  2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_H_PID2    0x00000003
#define USB_H_PID2_M  (USB_H_PID2_V << USB_H_PID2_S)
#define USB_H_PID2_V  0x00000003
#define USB_H_PID2_S  29
/** USB_H_DOPNG2 : R/W; bitpos: [31]; default: 0;
 *  This bit is used only for OUT transfers. Setting this field to 1 directs the host
 *  to do PING protocol
 *  1'b0: No ping protocol
 *  1'b1: Ping protocol
 */
#define USB_H_DOPNG2    (BIT(31))
#define USB_H_DOPNG2_M  (USB_H_DOPNG2_V << USB_H_DOPNG2_S)
#define USB_H_DOPNG2_V  0x00000001
#define USB_H_DOPNG2_S  31


/** USB_HCTSIZ3_REG register
 *  Host Channel 3Transfer Size Register
 */
#define USB_HCTSIZ3_REG (SOC_DPORT_USB_BASE + 0x570)
/** USB_H_XFERSIZE3 : R/W; bitpos: [19:0]; default: 0;
 *  Non-Scatter/Gather DMA Mode: Transfer Size.
 *  Scatter/Gather DMA Mode:
 *  [18:16]: Reserved
 *  [15:8]: NTD (Number of Transfer Descriptors)
 */
#define USB_H_XFERSIZE3    0x0007FFFF
#define USB_H_XFERSIZE3_M  (USB_H_XFERSIZE3_V << USB_H_XFERSIZE3_S)
#define USB_H_XFERSIZE3_V  0x0007FFFF
#define USB_H_XFERSIZE3_S  0
/** USB_H_PKTCNT3 : R/W; bitpos: [29:19]; default: 0;
 *  when in Non-Scatter/Gather DMA mode: Packet Count . This field is programmed by the
 *  application with the expected number of packets to be transmitted (OUT) or received
 *  (IN).
 *  Scatter/Gather DMA Mode: Reserved.
 */
#define USB_H_PKTCNT3    0x000003FF
#define USB_H_PKTCNT3_M  (USB_H_PKTCNT3_V << USB_H_PKTCNT3_S)
#define USB_H_PKTCNT3_V  0x000003FF
#define USB_H_PKTCNT3_S  19
/** USB_H_PID3 : R/W; bitpos: [31:29]; default: 0;
 *  2'b00: DATA0
 *  2'b01: DATA2
 *  2'b10: DATA1
 *  2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_H_PID3    0x00000003
#define USB_H_PID3_M  (USB_H_PID3_V << USB_H_PID3_S)
#define USB_H_PID3_V  0x00000003
#define USB_H_PID3_S  29
/** USB_H_DOPNG3 : R/W; bitpos: [31]; default: 0;
 *  This bit is used only for OUT transfers. Setting this field to 1 directs the host
 *  to do PING protocol
 *  1'b0: No ping protocol
 *  1'b1: Ping protocol
 */
#define USB_H_DOPNG3    (BIT(31))
#define USB_H_DOPNG3_M  (USB_H_DOPNG3_V << USB_H_DOPNG3_S)
#define USB_H_DOPNG3_V  0x00000001
#define USB_H_DOPNG3_S  31


/** USB_HCTSIZ4_REG register
 *  Host Channel 4Transfer Size Register
 */
#define USB_HCTSIZ4_REG (SOC_DPORT_USB_BASE + 0x590)
/** USB_H_XFERSIZE4 : R/W; bitpos: [19:0]; default: 0;
 *  Non-Scatter/Gather DMA Mode: Transfer Size.
 *  Scatter/Gather DMA Mode:
 *  [18:16]: Reserved
 *  [15:8]: NTD (Number of Transfer Descriptors)
 */
#define USB_H_XFERSIZE4    0x0007FFFF
#define USB_H_XFERSIZE4_M  (USB_H_XFERSIZE4_V << USB_H_XFERSIZE4_S)
#define USB_H_XFERSIZE4_V  0x0007FFFF
#define USB_H_XFERSIZE4_S  0
/** USB_H_PKTCNT4 : R/W; bitpos: [29:19]; default: 0;
 *  when in Non-Scatter/Gather DMA mode: Packet Count . This field is programmed by the
 *  application with the expected number of packets to be transmitted (OUT) or received
 *  (IN).
 *  Scatter/Gather DMA Mode: Reserved.
 */
#define USB_H_PKTCNT4    0x000003FF
#define USB_H_PKTCNT4_M  (USB_H_PKTCNT4_V << USB_H_PKTCNT4_S)
#define USB_H_PKTCNT4_V  0x000003FF
#define USB_H_PKTCNT4_S  19
/** USB_H_PID4 : R/W; bitpos: [31:29]; default: 0;
 *  2'b00: DATA0
 *  2'b01: DATA2
 *  2'b10: DATA1
 *  2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_H_PID4    0x00000003
#define USB_H_PID4_M  (USB_H_PID4_V << USB_H_PID4_S)
#define USB_H_PID4_V  0x00000003
#define USB_H_PID4_S  29
/** USB_H_DOPNG4 : R/W; bitpos: [31]; default: 0;
 *  This bit is used only for OUT transfers. Setting this field to 1 directs the host
 *  to do PING protocol
 *  1'b0: No ping protocol
 *  1'b1: Ping protocol
 */
#define USB_H_DOPNG4    (BIT(31))
#define USB_H_DOPNG4_M  (USB_H_DOPNG4_V << USB_H_DOPNG4_S)
#define USB_H_DOPNG4_V  0x00000001
#define USB_H_DOPNG4_S  31


/** USB_HCTSIZ5_REG register
 *  Host Channel 5Transfer Size Register
 */
#define USB_HCTSIZ5_REG (SOC_DPORT_USB_BASE + 0x5b0)
/** USB_H_XFERSIZE5 : R/W; bitpos: [19:0]; default: 0;
 *  Non-Scatter/Gather DMA Mode: Transfer Size.
 *  Scatter/Gather DMA Mode:
 *  [18:16]: Reserved
 *  [15:8]: NTD (Number of Transfer Descriptors)
 */
#define USB_H_XFERSIZE5    0x0007FFFF
#define USB_H_XFERSIZE5_M  (USB_H_XFERSIZE5_V << USB_H_XFERSIZE5_S)
#define USB_H_XFERSIZE5_V  0x0007FFFF
#define USB_H_XFERSIZE5_S  0
/** USB_H_PKTCNT5 : R/W; bitpos: [29:19]; default: 0;
 *  when in Non-Scatter/Gather DMA mode: Packet Count . This field is programmed by the
 *  application with the expected number of packets to be transmitted (OUT) or received
 *  (IN).
 *  Scatter/Gather DMA Mode: Reserved.
 */
#define USB_H_PKTCNT5    0x000003FF
#define USB_H_PKTCNT5_M  (USB_H_PKTCNT5_V << USB_H_PKTCNT5_S)
#define USB_H_PKTCNT5_V  0x000003FF
#define USB_H_PKTCNT5_S  19
/** USB_H_PID5 : R/W; bitpos: [31:29]; default: 0;
 *  2'b00: DATA0
 *  2'b01: DATA2
 *  2'b10: DATA1
 *  2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_H_PID5    0x00000003
#define USB_H_PID5_M  (USB_H_PID5_V << USB_H_PID5_S)
#define USB_H_PID5_V  0x00000003
#define USB_H_PID5_S  29
/** USB_H_DOPNG5 : R/W; bitpos: [31]; default: 0;
 *  This bit is used only for OUT transfers. Setting this field to 1 directs the host
 *  to do PING protocol
 *  1'b0: No ping protocol
 *  1'b1: Ping protocol
 */
#define USB_H_DOPNG5    (BIT(31))
#define USB_H_DOPNG5_M  (USB_H_DOPNG5_V << USB_H_DOPNG5_S)
#define USB_H_DOPNG5_V  0x00000001
#define USB_H_DOPNG5_S  31


/** USB_HCTSIZ6_REG register
 *  Host Channel 6Transfer Size Register
 */
#define USB_HCTSIZ6_REG (SOC_DPORT_USB_BASE + 0x5d0)
/** USB_H_XFERSIZE6 : R/W; bitpos: [19:0]; default: 0;
 *  Non-Scatter/Gather DMA Mode: Transfer Size.
 *  Scatter/Gather DMA Mode:
 *  [18:16]: Reserved
 *  [15:8]: NTD (Number of Transfer Descriptors)
 */
#define USB_H_XFERSIZE6    0x0007FFFF
#define USB_H_XFERSIZE6_M  (USB_H_XFERSIZE6_V << USB_H_XFERSIZE6_S)
#define USB_H_XFERSIZE6_V  0x0007FFFF
#define USB_H_XFERSIZE6_S  0
/** USB_H_PKTCNT6 : R/W; bitpos: [29:19]; default: 0;
 *  when in Non-Scatter/Gather DMA mode: Packet Count . This field is programmed by the
 *  application with the expected number of packets to be transmitted (OUT) or received
 *  (IN).
 *  Scatter/Gather DMA Mode: Reserved.
 */
#define USB_H_PKTCNT6    0x000003FF
#define USB_H_PKTCNT6_M  (USB_H_PKTCNT6_V << USB_H_PKTCNT6_S)
#define USB_H_PKTCNT6_V  0x000003FF
#define USB_H_PKTCNT6_S  19
/** USB_H_PID6 : R/W; bitpos: [31:29]; default: 0;
 *  2'b00: DATA0
 *  2'b01: DATA2
 *  2'b10: DATA1
 *  2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_H_PID6    0x00000003
#define USB_H_PID6_M  (USB_H_PID6_V << USB_H_PID6_S)
#define USB_H_PID6_V  0x00000003
#define USB_H_PID6_S  29
/** USB_H_DOPNG6 : R/W; bitpos: [31]; default: 0;
 *  This bit is used only for OUT transfers. Setting this field to 1 directs the host
 *  to do PING protocol
 *  1'b0: No ping protocol
 *  1'b1: Ping protocol
 */
#define USB_H_DOPNG6    (BIT(31))
#define USB_H_DOPNG6_M  (USB_H_DOPNG6_V << USB_H_DOPNG6_S)
#define USB_H_DOPNG6_V  0x00000001
#define USB_H_DOPNG6_S  31


/** USB_HCTSIZ7_REG register
 *  Host Channel 7Transfer Size Register
 */
#define USB_HCTSIZ7_REG (SOC_DPORT_USB_BASE + 0x5f0)
/** USB_H_XFERSIZE7 : R/W; bitpos: [19:0]; default: 0;
 *  Non-Scatter/Gather DMA Mode: Transfer Size.
 *  Scatter/Gather DMA Mode:
 *  [18:16]: Reserved
 *  [15:8]: NTD (Number of Transfer Descriptors)
 */
#define USB_H_XFERSIZE7    0x0007FFFF
#define USB_H_XFERSIZE7_M  (USB_H_XFERSIZE7_V << USB_H_XFERSIZE7_S)
#define USB_H_XFERSIZE7_V  0x0007FFFF
#define USB_H_XFERSIZE7_S  0
/** USB_H_PKTCNT7 : R/W; bitpos: [29:19]; default: 0;
 *  when in Non-Scatter/Gather DMA mode: Packet Count . This field is programmed by the
 *  application with the expected number of packets to be transmitted (OUT) or received
 *  (IN).
 *  Scatter/Gather DMA Mode: Reserved.
 */
#define USB_H_PKTCNT7    0x000003FF
#define USB_H_PKTCNT7_M  (USB_H_PKTCNT7_V << USB_H_PKTCNT7_S)
#define USB_H_PKTCNT7_V  0x000003FF
#define USB_H_PKTCNT7_S  19
/** USB_H_PID7 : R/W; bitpos: [31:29]; default: 0;
 *  2'b00: DATA0
 *  2'b01: DATA2
 *  2'b10: DATA1
 *  2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_H_PID7    0x00000003
#define USB_H_PID7_M  (USB_H_PID7_V << USB_H_PID7_S)
#define USB_H_PID7_V  0x00000003
#define USB_H_PID7_S  29
/** USB_H_DOPNG7 : R/W; bitpos: [31]; default: 0;
 *  This bit is used only for OUT transfers. Setting this field to 1 directs the host
 *  to do PING protocol
 *  1'b0: No ping protocol
 *  1'b1: Ping protocol
 */
#define USB_H_DOPNG7    (BIT(31))
#define USB_H_DOPNG7_M  (USB_H_DOPNG7_V << USB_H_DOPNG7_S)
#define USB_H_DOPNG7_V  0x00000001
#define USB_H_DOPNG7_S  31




/** Status registers */
/** USB_GRXSTSR_REG register
 *  Receive Status Debug Read Register
 */
#define USB_GRXSTSR_REG (SOC_DPORT_USB_BASE + 0x1c)
/** USB_G_CHNUM : RO; bitpos: [4:0]; default: 0;
 *  Channel Number: Host only. Indicates the channel number to which the current
 *  received packet belongs.
 *  Endpoint Number: Device only Indicates the endpoint number to which the current
 *  received packet belongs
 */
#define USB_G_CHNUM    0x0000000F
#define USB_G_CHNUM_M  (USB_G_CHNUM_V << USB_G_CHNUM_S)
#define USB_G_CHNUM_V  0x0000000F
#define USB_G_CHNUM_S  0
/** USB_G_BCNT : RO; bitpos: [15:4]; default: 0;
 *  In host mode, indicates the byte count of the received IN data Packet
 *  In device mode, indicates the byte count of the received data packet
 */
#define USB_G_BCNT    0x000007FF
#define USB_G_BCNT_M  (USB_G_BCNT_V << USB_G_BCNT_S)
#define USB_G_BCNT_V  0x000007FF
#define USB_G_BCNT_S  4
/** USB_G_DPID : RO; bitpos: [17:15]; default: 0;
 *  Data PID
 *  0x0 (DATA0): DATA0
 *  0x2 (DATA1): DATA1
 *  0x1 (DATA2): DATA2
 *  0x3 (MDATA): MDATA
 */
#define USB_G_DPID    0x00000003
#define USB_G_DPID_M  (USB_G_DPID_V << USB_G_DPID_S)
#define USB_G_DPID_V  0x00000003
#define USB_G_DPID_S  15
/** USB_G_PKTSTS : RO; bitpos: [21:17]; default: 0;
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
#define USB_G_PKTSTS    0x0000000F
#define USB_G_PKTSTS_M  (USB_G_PKTSTS_V << USB_G_PKTSTS_S)
#define USB_G_PKTSTS_V  0x0000000F
#define USB_G_PKTSTS_S  17
/** USB_G_FN : RO; bitpos: [25:21]; default: 0;
 *  This is the least significant 4 bits of the (micro)Frame number in which the packet
 *  is received on the USB. This field is supported only when isochronous OUT endpoints
 *  are supported
 */
#define USB_G_FN    0x0000000F
#define USB_G_FN_M  (USB_G_FN_V << USB_G_FN_S)
#define USB_G_FN_V  0x0000000F
#define USB_G_FN_S  21


/** USB_GRXSTSP_REG register
 *  Receive Status Read/Pop Register
 */
#define USB_GRXSTSP_REG (SOC_DPORT_USB_BASE + 0x20)
/** USB_CHNUM : RO; bitpos: [4:0]; default: 0;
 *  Channel Number: Host only. Indicates the channel number to which the current
 *  received packet belongs.
 *  Endpoint Number: Device only Indicates the endpoint number to which the current
 *  received packet belongs
 */
#define USB_CHNUM    0x0000000F
#define USB_CHNUM_M  (USB_CHNUM_V << USB_CHNUM_S)
#define USB_CHNUM_V  0x0000000F
#define USB_CHNUM_S  0
/** USB_BCNT : RO; bitpos: [15:4]; default: 0;
 *  In host mode, indicates the byte count of the received IN data Packet
 *  In device mode, indicates the byte count of the received data packet
 */
#define USB_BCNT    0x000007FF
#define USB_BCNT_M  (USB_BCNT_V << USB_BCNT_S)
#define USB_BCNT_V  0x000007FF
#define USB_BCNT_S  4
/** USB_DPID : RO; bitpos: [17:15]; default: 0;
 *  Data PID
 *  0x0 (DATA0): DATA0
 *  0x2 (DATA1): DATA1
 *  0x1 (DATA2): DATA2
 *  0x3 (MDATA): MDATA
 */
#define USB_DPID    0x00000003
#define USB_DPID_M  (USB_DPID_V << USB_DPID_S)
#define USB_DPID_V  0x00000003
#define USB_DPID_S  15
/** USB_PKTSTS : RO; bitpos: [21:17]; default: 0;
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
#define USB_PKTSTS    0x0000000F
#define USB_PKTSTS_M  (USB_PKTSTS_V << USB_PKTSTS_S)
#define USB_PKTSTS_V  0x0000000F
#define USB_PKTSTS_S  17
/** USB_FN : RO; bitpos: [25:21]; default: 0;
 *  This is the least significant 4 bits of the (micro)Frame number in which the packet
 *  is received on the USB. This field is supported only when isochronous OUT endpoints
 *  are supported
 */
#define USB_FN    0x0000000F
#define USB_FN_M  (USB_FN_V << USB_FN_S)
#define USB_FN_V  0x0000000F
#define USB_FN_S  21


/** USB_GSNPSID_REG register
 *  Synopsys ID Register
 */
#define USB_GSNPSID_REG (SOC_DPORT_USB_BASE + 0x40)
/** USB_SYNOPSYSID : RO; bitpos: [32:0]; default: 1330921482;
 *  ID register
 */
#define USB_SYNOPSYSID    0xFFFFFFFF
#define USB_SYNOPSYSID_M  (USB_SYNOPSYSID_V << USB_SYNOPSYSID_S)
#define USB_SYNOPSYSID_V  0xFFFFFFFF
#define USB_SYNOPSYSID_S  0


/** USB_GHWCFG1_REG register
 *  User Hardware Configuration 1 Register
 */
#define USB_GHWCFG1_REG (SOC_DPORT_USB_BASE + 0x44)
/** USB_EPDIR : RO; bitpos: [32:0]; default: 0;
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
#define USB_EPDIR    0xFFFFFFFF
#define USB_EPDIR_M  (USB_EPDIR_V << USB_EPDIR_S)
#define USB_EPDIR_V  0xFFFFFFFF
#define USB_EPDIR_S  0


/** USB_GHWCFG2_REG register
 *  User Hardware Configuration 2 Register
 */
#define USB_GHWCFG2_REG (SOC_DPORT_USB_BASE + 0x48)
/** USB_OTGMODE : RO; bitpos: [3:0]; default: 0;
 *  3'b000: HNP- and SRP-Capable OTG (Host  Device)
 */
#define USB_OTGMODE    0x00000007
#define USB_OTGMODE_M  (USB_OTGMODE_V << USB_OTGMODE_S)
#define USB_OTGMODE_V  0x00000007
#define USB_OTGMODE_S  0
/** USB_OTGARCH : RO; bitpos: [5:3]; default: 2;
 *  2'b10: Internal DMA
 */
#define USB_OTGARCH    0x00000003
#define USB_OTGARCH_M  (USB_OTGARCH_V << USB_OTGARCH_S)
#define USB_OTGARCH_V  0x00000003
#define USB_OTGARCH_S  3
/** USB_SINGPNT : RO; bitpos: [5]; default: 1;
 *  Point-to-Point
 *  1'b1: Single-point application (no hub and split support).
 */
#define USB_SINGPNT    (BIT(5))
#define USB_SINGPNT_M  (USB_SINGPNT_V << USB_SINGPNT_S)
#define USB_SINGPNT_V  0x00000001
#define USB_SINGPNT_S  5
/** USB_HSPHYTYPE : RO; bitpos: [8:6]; default: 0;
 *  High-Speed PHY Interface Type
 *  2'b00: High-Speed interface not supported
 */
#define USB_HSPHYTYPE    0x00000003
#define USB_HSPHYTYPE_M  (USB_HSPHYTYPE_V << USB_HSPHYTYPE_S)
#define USB_HSPHYTYPE_V  0x00000003
#define USB_HSPHYTYPE_S  6
/** USB_FSPHYTYPE : RO; bitpos: [10:8]; default: 1;
 *  Full-Speed PHY Interface Type.
 */
#define USB_FSPHYTYPE    0x00000003
#define USB_FSPHYTYPE_M  (USB_FSPHYTYPE_V << USB_FSPHYTYPE_S)
#define USB_FSPHYTYPE_V  0x00000003
#define USB_FSPHYTYPE_S  8
/** USB_NUMDEVEPS : RO; bitpos: [14:10]; default: 6;
 *  Number of Device Endpoints.
 */
#define USB_NUMDEVEPS    0x0000000F
#define USB_NUMDEVEPS_M  (USB_NUMDEVEPS_V << USB_NUMDEVEPS_S)
#define USB_NUMDEVEPS_V  0x0000000F
#define USB_NUMDEVEPS_S  10
/** USB_NUMHSTCHNL : RO; bitpos: [18:14]; default: 7;
 *  Number of Host Channels.
 */
#define USB_NUMHSTCHNL    0x0000000F
#define USB_NUMHSTCHNL_M  (USB_NUMHSTCHNL_V << USB_NUMHSTCHNL_S)
#define USB_NUMHSTCHNL_V  0x0000000F
#define USB_NUMHSTCHNL_S  14
/** USB_PERIOSUPPORT : RO; bitpos: [18]; default: 1;
 *  1'b0:Periodic OUT Channels is not Supported in Host Mode
 *  1'b1:Periodic OUT Channels Supported in Host Mode
 */
#define USB_PERIOSUPPORT    (BIT(18))
#define USB_PERIOSUPPORT_M  (USB_PERIOSUPPORT_V << USB_PERIOSUPPORT_S)
#define USB_PERIOSUPPORT_V  0x00000001
#define USB_PERIOSUPPORT_S  18
/** USB_DYNFIFOSIZING : RO; bitpos: [19]; default: 1;
 *  1'b0:Dynamic FIFO Sizing Disabled
 *  1'b1:Dynamic FIFO Sizing Enabled
 */
#define USB_DYNFIFOSIZING    (BIT(19))
#define USB_DYNFIFOSIZING_M  (USB_DYNFIFOSIZING_V << USB_DYNFIFOSIZING_S)
#define USB_DYNFIFOSIZING_V  0x00000001
#define USB_DYNFIFOSIZING_S  19
/** USB_MULTIPROCINTRPT : RO; bitpos: [20]; default: 0;
 *  1'b0: No Multi Processor Interrupt Enabled
 *  1'b1:Multi Processor Interrupt Enabled
 */
#define USB_MULTIPROCINTRPT    (BIT(20))
#define USB_MULTIPROCINTRPT_M  (USB_MULTIPROCINTRPT_V << USB_MULTIPROCINTRPT_S)
#define USB_MULTIPROCINTRPT_V  0x00000001
#define USB_MULTIPROCINTRPT_S  20
/** USB_NPTXQDEPTH : RO; bitpos: [24:22]; default: 1;
 *  Non-periodic Request Queue Depth
 *  2'b01: 4
 */
#define USB_NPTXQDEPTH    0x00000003
#define USB_NPTXQDEPTH_M  (USB_NPTXQDEPTH_V << USB_NPTXQDEPTH_S)
#define USB_NPTXQDEPTH_V  0x00000003
#define USB_NPTXQDEPTH_S  22
/** USB_PTXQDEPTH : RO; bitpos: [26:24]; default: 2;
 *  Host Mode Periodic Request Queue Depth.
 *  2'b10: 8
 */
#define USB_PTXQDEPTH    0x00000003
#define USB_PTXQDEPTH_M  (USB_PTXQDEPTH_V << USB_PTXQDEPTH_S)
#define USB_PTXQDEPTH_V  0x00000003
#define USB_PTXQDEPTH_S  24
/** USB_TKNQDEPTH : RO; bitpos: [31:26]; default: 8;
 *  Device Mode IN Token Sequence Learning Queue Depth.
 */
#define USB_TKNQDEPTH    0x0000001F
#define USB_TKNQDEPTH_M  (USB_TKNQDEPTH_V << USB_TKNQDEPTH_S)
#define USB_TKNQDEPTH_V  0x0000001F
#define USB_TKNQDEPTH_S  26
/** USB_OTG_ENABLE_IC_USB : RO; bitpos: [31]; default: 0;
 *  0x0 (DISABLE): Disabled the IC_USB Full-Speed Serial Transceiver interface
 *  0x1 (ENABLE): Enabled the IC_USB Full-Speed Serial Transceiver interface
 */
#define USB_OTG_ENABLE_IC_USB    (BIT(31))
#define USB_OTG_ENABLE_IC_USB_M  (USB_OTG_ENABLE_IC_USB_V << USB_OTG_ENABLE_IC_USB_S)
#define USB_OTG_ENABLE_IC_USB_V  0x00000001
#define USB_OTG_ENABLE_IC_USB_S  31


/** USB_GHWCFG3_REG register
 *  User Hardware Configuration 3 Register
 */
#define USB_GHWCFG3_REG (SOC_DPORT_USB_BASE + 0x4c)
/** USB_XFERSIZEWIDTH : RO; bitpos: [4:0]; default: 5;
 *  Width of Transfer Size Counters
 *  0x5 (WIDTH16): Width of Transfer Size Counter 16 bits
 */
#define USB_XFERSIZEWIDTH    0x0000000F
#define USB_XFERSIZEWIDTH_M  (USB_XFERSIZEWIDTH_V << USB_XFERSIZEWIDTH_S)
#define USB_XFERSIZEWIDTH_V  0x0000000F
#define USB_XFERSIZEWIDTH_S  0
/** USB_PKTSIZEWIDTH : RO; bitpos: [7:4]; default: 3;
 *  Width of Packet Size Counters
 *  3'b011: 7 bits
 */
#define USB_PKTSIZEWIDTH    0x00000007
#define USB_PKTSIZEWIDTH_M  (USB_PKTSIZEWIDTH_V << USB_PKTSIZEWIDTH_S)
#define USB_PKTSIZEWIDTH_V  0x00000007
#define USB_PKTSIZEWIDTH_S  4
/** USB_OTGEN : RO; bitpos: [7]; default: 1;
 *  OTG Function Enabled.
 */
#define USB_OTGEN    (BIT(7))
#define USB_OTGEN_M  (USB_OTGEN_V << USB_OTGEN_S)
#define USB_OTGEN_V  0x00000001
#define USB_OTGEN_S  7
/** USB_I2CINTSEL : RO; bitpos: [8]; default: 0;
 *  1'b0: I2C Interface is not available on the controller.
 */
#define USB_I2CINTSEL    (BIT(8))
#define USB_I2CINTSEL_M  (USB_I2CINTSEL_V << USB_I2CINTSEL_S)
#define USB_I2CINTSEL_V  0x00000001
#define USB_I2CINTSEL_S  8
/** USB_VNDCTLSUPT : RO; bitpos: [9]; default: 0;
 *  Vendor Control Interface is not available .
 */
#define USB_VNDCTLSUPT    (BIT(9))
#define USB_VNDCTLSUPT_M  (USB_VNDCTLSUPT_V << USB_VNDCTLSUPT_S)
#define USB_VNDCTLSUPT_V  0x00000001
#define USB_VNDCTLSUPT_S  9
/** USB_OPTFEATURE : RO; bitpos: [10]; default: 1;
 *  Optional Features have been Removed.
 */
#define USB_OPTFEATURE    (BIT(10))
#define USB_OPTFEATURE_M  (USB_OPTFEATURE_V << USB_OPTFEATURE_S)
#define USB_OPTFEATURE_V  0x00000001
#define USB_OPTFEATURE_S  10
/** USB_RSTTYPE : RO; bitpos: [11]; default: 0;
 *  Asynchronous reset is used in the core
 */
#define USB_RSTTYPE    (BIT(11))
#define USB_RSTTYPE_M  (USB_RSTTYPE_V << USB_RSTTYPE_S)
#define USB_RSTTYPE_V  0x00000001
#define USB_RSTTYPE_S  11
/** USB_ADPSUPPORT : RO; bitpos: [12]; default: 0;
 *  ADP logic is not present along with the controller.
 */
#define USB_ADPSUPPORT    (BIT(12))
#define USB_ADPSUPPORT_M  (USB_ADPSUPPORT_V << USB_ADPSUPPORT_S)
#define USB_ADPSUPPORT_V  0x00000001
#define USB_ADPSUPPORT_S  12
/** USB_HSICMODE : RO; bitpos: [13]; default: 0;
 *  HSIC mode specified for Mode of Operation.
 *  1'b0: Non-HSIC-capable
 */
#define USB_HSICMODE    (BIT(13))
#define USB_HSICMODE_M  (USB_HSICMODE_V << USB_HSICMODE_S)
#define USB_HSICMODE_V  0x00000001
#define USB_HSICMODE_S  13
/** USB_BCSUPPORT : RO; bitpos: [14]; default: 0;
 *  1'b0: No Battery Charger Support
 */
#define USB_BCSUPPORT    (BIT(14))
#define USB_BCSUPPORT_M  (USB_BCSUPPORT_V << USB_BCSUPPORT_S)
#define USB_BCSUPPORT_V  0x00000001
#define USB_BCSUPPORT_S  14
/** USB_LPMMODE : RO; bitpos: [15]; default: 0;
 *  LPM mode specified for Mode of Operation.
 */
#define USB_LPMMODE    (BIT(15))
#define USB_LPMMODE_M  (USB_LPMMODE_V << USB_LPMMODE_S)
#define USB_LPMMODE_V  0x00000001
#define USB_LPMMODE_S  15
/** USB_DFIFODEPTH : RO; bitpos: [32:16]; default: 256;
 *  DFIFO Depth.This value is in terms of 32-bit words.
 */
#define USB_DFIFODEPTH    0x0000FFFF
#define USB_DFIFODEPTH_M  (USB_DFIFODEPTH_V << USB_DFIFODEPTH_S)
#define USB_DFIFODEPTH_V  0x0000FFFF
#define USB_DFIFODEPTH_S  16


/** USB_GHWCFG4_REG register
 *  User Hardware Configuration 4 Register
 */
#define USB_GHWCFG4_REG (SOC_DPORT_USB_BASE + 0x50)
/** USB_G_NUMDEVPERIOEPS : RO; bitpos: [4:0]; default: 0;
 *  Number of Device Mode Periodic IN Endpoints.
 */
#define USB_G_NUMDEVPERIOEPS    0x0000000F
#define USB_G_NUMDEVPERIOEPS_M  (USB_G_NUMDEVPERIOEPS_V << USB_G_NUMDEVPERIOEPS_S)
#define USB_G_NUMDEVPERIOEPS_V  0x0000000F
#define USB_G_NUMDEVPERIOEPS_S  0
/** USB_G_PARTIALPWRDN : RO; bitpos: [4]; default: 1;
 *  Enable Partial Power Down.
 */
#define USB_G_PARTIALPWRDN    (BIT(4))
#define USB_G_PARTIALPWRDN_M  (USB_G_PARTIALPWRDN_V << USB_G_PARTIALPWRDN_S)
#define USB_G_PARTIALPWRDN_V  0x00000001
#define USB_G_PARTIALPWRDN_S  4
/** USB_G_AHBFREQ : RO; bitpos: [5]; default: 1;
 *  Minimum AHB Frequency Less Than 60 MHz
 */
#define USB_G_AHBFREQ    (BIT(5))
#define USB_G_AHBFREQ_M  (USB_G_AHBFREQ_V << USB_G_AHBFREQ_S)
#define USB_G_AHBFREQ_V  0x00000001
#define USB_G_AHBFREQ_S  5
/** USB_G_HIBERNATION : RO; bitpos: [6]; default: 0;
 *  1'b0: Hibernation feature not enabled.
 */
#define USB_G_HIBERNATION    (BIT(6))
#define USB_G_HIBERNATION_M  (USB_G_HIBERNATION_V << USB_G_HIBERNATION_S)
#define USB_G_HIBERNATION_V  0x00000001
#define USB_G_HIBERNATION_S  6
/** USB_G_EXTENDEDHIBERNATION : RO; bitpos: [7]; default: 0;
 *  Extended Hibernation feature not enabled
 */
#define USB_G_EXTENDEDHIBERNATION    (BIT(7))
#define USB_G_EXTENDEDHIBERNATION_M  (USB_G_EXTENDEDHIBERNATION_V << USB_G_EXTENDEDHIBERNATION_S)
#define USB_G_EXTENDEDHIBERNATION_V  0x00000001
#define USB_G_EXTENDEDHIBERNATION_S  7
/** USB_G_ACGSUPT : RO; bitpos: [12]; default: 0;
 *  Active Clock Gating is not enabled.
 */
#define USB_G_ACGSUPT    (BIT(12))
#define USB_G_ACGSUPT_M  (USB_G_ACGSUPT_V << USB_G_ACGSUPT_S)
#define USB_G_ACGSUPT_V  0x00000001
#define USB_G_ACGSUPT_S  12
/** USB_G_ENHANCEDLPMSUPT : RO; bitpos: [13]; default: 1;
 *  Enhanced LPM Support.
 */
#define USB_G_ENHANCEDLPMSUPT    (BIT(13))
#define USB_G_ENHANCEDLPMSUPT_M  (USB_G_ENHANCEDLPMSUPT_V << USB_G_ENHANCEDLPMSUPT_S)
#define USB_G_ENHANCEDLPMSUPT_V  0x00000001
#define USB_G_ENHANCEDLPMSUPT_S  13
/** USB_G_PHYDATAWIDTH : RO; bitpos: [16:14]; default: 2;
 *  UTMI+ PHY/ULPI-to-Internal UTMI+ Wrapper Data Width (PhyDataWidth) When a ULPI PHY
 *  is used, an internal wrapper converts ULPI to UTMI+
 *  2'b10: 8/16 bits, software selectable
 */
#define USB_G_PHYDATAWIDTH    0x00000003
#define USB_G_PHYDATAWIDTH_M  (USB_G_PHYDATAWIDTH_V << USB_G_PHYDATAWIDTH_S)
#define USB_G_PHYDATAWIDTH_V  0x00000003
#define USB_G_PHYDATAWIDTH_S  14
/** USB_G_NUMCTLEPS : RO; bitpos: [20:16]; default: 0;
 *  Number of Device Mode Control Endpoints in Addition to Endpoint 0
 */
#define USB_G_NUMCTLEPS    0x0000000F
#define USB_G_NUMCTLEPS_M  (USB_G_NUMCTLEPS_V << USB_G_NUMCTLEPS_S)
#define USB_G_NUMCTLEPS_V  0x0000000F
#define USB_G_NUMCTLEPS_S  16
/** USB_G_IDDQFLTR : RO; bitpos: [20]; default: 1;
 *  IDDIG Filter Enable.
 */
#define USB_G_IDDQFLTR    (BIT(20))
#define USB_G_IDDQFLTR_M  (USB_G_IDDQFLTR_V << USB_G_IDDQFLTR_S)
#define USB_G_IDDQFLTR_V  0x00000001
#define USB_G_IDDQFLTR_S  20
/** USB_G_VBUSVALIDFLTR : RO; bitpos: [21]; default: 1;
 *  VBUS Valid Filter Enabled.
 */
#define USB_G_VBUSVALIDFLTR    (BIT(21))
#define USB_G_VBUSVALIDFLTR_M  (USB_G_VBUSVALIDFLTR_V << USB_G_VBUSVALIDFLTR_S)
#define USB_G_VBUSVALIDFLTR_V  0x00000001
#define USB_G_VBUSVALIDFLTR_S  21
/** USB_G_AVALIDFLTR : RO; bitpos: [22]; default: 1;
 *  a_valid Filter Enabled.
 */
#define USB_G_AVALIDFLTR    (BIT(22))
#define USB_G_AVALIDFLTR_M  (USB_G_AVALIDFLTR_V << USB_G_AVALIDFLTR_S)
#define USB_G_AVALIDFLTR_V  0x00000001
#define USB_G_AVALIDFLTR_S  22
/** USB_G_BVALIDFLTR : RO; bitpos: [23]; default: 1;
 *  b_valid Filter Enabled.
 */
#define USB_G_BVALIDFLTR    (BIT(23))
#define USB_G_BVALIDFLTR_M  (USB_G_BVALIDFLTR_V << USB_G_BVALIDFLTR_S)
#define USB_G_BVALIDFLTR_V  0x00000001
#define USB_G_BVALIDFLTR_S  23
/** USB_G_SESSENDFLTR : RO; bitpos: [24]; default: 1;
 *  session_end Filter Enabled.
 */
#define USB_G_SESSENDFLTR    (BIT(24))
#define USB_G_SESSENDFLTR_M  (USB_G_SESSENDFLTR_V << USB_G_SESSENDFLTR_S)
#define USB_G_SESSENDFLTR_V  0x00000001
#define USB_G_SESSENDFLTR_S  24
/** USB_G_DEDFIFOMODE : RO; bitpos: [25]; default: 1;
 *  Enable Dedicated Transmit FIFO for device IN Endpoints
 */
#define USB_G_DEDFIFOMODE    (BIT(25))
#define USB_G_DEDFIFOMODE_M  (USB_G_DEDFIFOMODE_V << USB_G_DEDFIFOMODE_S)
#define USB_G_DEDFIFOMODE_V  0x00000001
#define USB_G_DEDFIFOMODE_S  25
/** USB_G_INEPS : RO; bitpos: [30:26]; default: 4;
 *  Number of Device Mode IN Endpoints Including Control Endpoints.
 */
#define USB_G_INEPS    0x0000000F
#define USB_G_INEPS_M  (USB_G_INEPS_V << USB_G_INEPS_S)
#define USB_G_INEPS_V  0x0000000F
#define USB_G_INEPS_S  26
/** USB_G_DESCDMAENABLED : RO; bitpos: [30]; default: 1;
 *  0x1: Scatter/Gather DMA configuration.
 */
#define USB_G_DESCDMAENABLED    (BIT(30))
#define USB_G_DESCDMAENABLED_M  (USB_G_DESCDMAENABLED_V << USB_G_DESCDMAENABLED_S)
#define USB_G_DESCDMAENABLED_V  0x00000001
#define USB_G_DESCDMAENABLED_S  30
/** USB_G_DESCDMA : RO; bitpos: [31]; default: 1;
 *  Scatter/Gather DMA configuration
 *  1'b1: Dynamic configuration
 */
#define USB_G_DESCDMA    (BIT(31))
#define USB_G_DESCDMA_M  (USB_G_DESCDMA_V << USB_G_DESCDMA_S)
#define USB_G_DESCDMA_V  0x00000001
#define USB_G_DESCDMA_S  31


/** USB_HFNUM_REG register
 *  Frame Number configure Resigster
 */
#define USB_HFNUM_REG (SOC_DPORT_USB_BASE + 0x408)
/** USB_FRNUM : RO; bitpos: [14:0]; default: 16383;
 *  Frame Number
 *  0x0 (INACTIVE): No SOF is transmitted
 *  0x1 (ACTIVE): SOF is transmitted
 */
#define USB_FRNUM    0x00003FFF
#define USB_FRNUM_M  (USB_FRNUM_V << USB_FRNUM_S)
#define USB_FRNUM_V  0x00003FFF
#define USB_FRNUM_S  0
/** USB_FRREM : RO; bitpos: [32:16]; default: 0;
 *  Frame Time Remaining.ndicates the amount of time remaining in the current
 *  microframe
 *  (HS) or Frame (FS/LS), in terms of PHY clocks.
 */
#define USB_FRREM    0x0000FFFF
#define USB_FRREM_M  (USB_FRREM_V << USB_FRREM_S)
#define USB_FRREM_V  0x0000FFFF
#define USB_FRREM_S  16


/** USB_HPTXSTS_REG register
 *  Host Periodic Transmit FIFO/Queue Status Register
 */
#define USB_HPTXSTS_REG (SOC_DPORT_USB_BASE + 0x410)
/** USB_PTXFSPCAVAIL : RO; bitpos: [16:0]; default: 256;
 *  Periodic Transmit Data FIFO Space Available. Values are in terms of 32-bit words.
 */
#define USB_PTXFSPCAVAIL    0x0000FFFF
#define USB_PTXFSPCAVAIL_M  (USB_PTXFSPCAVAIL_V << USB_PTXFSPCAVAIL_S)
#define USB_PTXFSPCAVAIL_V  0x0000FFFF
#define USB_PTXFSPCAVAIL_S  0
/** USB_PTXQSPCAVAIL : RO; bitpos: [21:16]; default: 8;
 *  Periodic Transmit Request Queue Space Available.
 */
#define USB_PTXQSPCAVAIL    0x0000001F
#define USB_PTXQSPCAVAIL_M  (USB_PTXQSPCAVAIL_V << USB_PTXQSPCAVAIL_S)
#define USB_PTXQSPCAVAIL_V  0x0000001F
#define USB_PTXQSPCAVAIL_S  16
/** USB_PTXQTOP : RO; bitpos: [32:24]; default: 0;
 *  Bit [31]: Odd/Even (micro)Frame. 1'b0: send in even (micro)Frame
 *  Bits [30:27]: Channel/endpoint number
 *  Bits [26:25]: Type. 2'b00: IN/OUT.   2'b01: Zero-length packet.     2'b10: CSPLIT
 *    2'b11: Disable channel command
 *  Bit [24]: Terminate
 */
#define USB_PTXQTOP    0x000000FF
#define USB_PTXQTOP_M  (USB_PTXQTOP_V << USB_PTXQTOP_S)
#define USB_PTXQTOP_V  0x000000FF
#define USB_PTXQTOP_S  24


/** USB_HCDMAB$n_REG register
 *  Host Channel $n DMA Buffer Address Register
 */
#define USB_HCDMAB$N_REG (SOC_DPORT_USB_BASE + 0x51c)
/** USB_H_HCDMAB0 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address
 */
#define USB_H_HCDMAB0    0xFFFFFFFF
#define USB_H_HCDMAB0_M  (USB_H_HCDMAB0_V << USB_H_HCDMAB0_S)
#define USB_H_HCDMAB0_V  0xFFFFFFFF
#define USB_H_HCDMAB0_S  0


/** USB_HCDMAB1_REG register
 *  Host Channel 1 DMA Buffer Address Register
 */
#define USB_HCDMAB1_REG (SOC_DPORT_USB_BASE + 0x53c)
/** USB_H_HCDMAB1 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address
 */
#define USB_H_HCDMAB1    0xFFFFFFFF
#define USB_H_HCDMAB1_M  (USB_H_HCDMAB1_V << USB_H_HCDMAB1_S)
#define USB_H_HCDMAB1_V  0xFFFFFFFF
#define USB_H_HCDMAB1_S  0


/** USB_HCDMAB2_REG register
 *  Host Channel 2 DMA Buffer Address Register
 */
#define USB_HCDMAB2_REG (SOC_DPORT_USB_BASE + 0x55c)
/** USB_H_HCDMAB2 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address
 */
#define USB_H_HCDMAB2    0xFFFFFFFF
#define USB_H_HCDMAB2_M  (USB_H_HCDMAB2_V << USB_H_HCDMAB2_S)
#define USB_H_HCDMAB2_V  0xFFFFFFFF
#define USB_H_HCDMAB2_S  0


/** USB_HCDMAB3_REG register
 *  Host Channel 3 DMA Buffer Address Register
 */
#define USB_HCDMAB3_REG (SOC_DPORT_USB_BASE + 0x57c)
/** USB_H_HCDMAB3 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address
 */
#define USB_H_HCDMAB3    0xFFFFFFFF
#define USB_H_HCDMAB3_M  (USB_H_HCDMAB3_V << USB_H_HCDMAB3_S)
#define USB_H_HCDMAB3_V  0xFFFFFFFF
#define USB_H_HCDMAB3_S  0


/** USB_HCDMAB4_REG register
 *  Host Channel 4 DMA Buffer Address Register
 */
#define USB_HCDMAB4_REG (SOC_DPORT_USB_BASE + 0x59c)
/** USB_H_HCDMAB4 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address
 */
#define USB_H_HCDMAB4    0xFFFFFFFF
#define USB_H_HCDMAB4_M  (USB_H_HCDMAB4_V << USB_H_HCDMAB4_S)
#define USB_H_HCDMAB4_V  0xFFFFFFFF
#define USB_H_HCDMAB4_S  0


/** USB_HCDMAB5_REG register
 *  Host Channel 5 DMA Buffer Address Register
 */
#define USB_HCDMAB5_REG (SOC_DPORT_USB_BASE + 0x5bc)
/** USB_H_HCDMAB5 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address
 */
#define USB_H_HCDMAB5    0xFFFFFFFF
#define USB_H_HCDMAB5_M  (USB_H_HCDMAB5_V << USB_H_HCDMAB5_S)
#define USB_H_HCDMAB5_V  0xFFFFFFFF
#define USB_H_HCDMAB5_S  0


/** USB_HCDMAB6_REG register
 *  Host Channel 6 DMA Buffer Address Register
 */
#define USB_HCDMAB6_REG (SOC_DPORT_USB_BASE + 0x5dc)
/** USB_H_HCDMAB6 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address
 */
#define USB_H_HCDMAB6    0xFFFFFFFF
#define USB_H_HCDMAB6_M  (USB_H_HCDMAB6_V << USB_H_HCDMAB6_S)
#define USB_H_HCDMAB6_V  0xFFFFFFFF
#define USB_H_HCDMAB6_S  0


/** USB_HCDMAB7_REG register
 *  Host Channel 7 DMA Buffer Address Register
 */
#define USB_HCDMAB7_REG (SOC_DPORT_USB_BASE + 0x5fc)
/** USB_H_HCDMAB7 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address
 */
#define USB_H_HCDMAB7    0xFFFFFFFF
#define USB_H_HCDMAB7_M  (USB_H_HCDMAB7_V << USB_H_HCDMAB7_S)
#define USB_H_HCDMAB7_V  0xFFFFFFFF
#define USB_H_HCDMAB7_S  0


/** USB_DSTS_REG register
 *  Device Status Register
 */
#define USB_DSTS_REG (SOC_DPORT_USB_BASE + 0x808)
/** USB_SUSPSTS : RO; bitpos: [0]; default: 0;
 *  Suspend Status
 *  0x0 : No suspend state
 *  0x1 : Suspend state
 */
#define USB_SUSPSTS    (BIT(0))
#define USB_SUSPSTS_M  (USB_SUSPSTS_V << USB_SUSPSTS_S)
#define USB_SUSPSTS_V  0x00000001
#define USB_SUSPSTS_S  0
/** USB_ENUMSPD : RO; bitpos: [3:1]; default: 1;
 *  0x0 : High speed (PHY clock is running at 30 or 60 MHz)
 *  0x1 : Full speed (PHY clock is running at 30 or 60 MHz)
 *  0x2 : Low speed (PHY clock is running at 6 MHz)
 *  0x3 : Full speed (PHY clock is running at 48 MHz)
 */
#define USB_ENUMSPD    0x00000003
#define USB_ENUMSPD_M  (USB_ENUMSPD_V << USB_ENUMSPD_S)
#define USB_ENUMSPD_V  0x00000003
#define USB_ENUMSPD_S  1
/** USB_ERRTICERR : RO; bitpos: [3]; default: 0;
 *  0x0 : No Erratic Error
 *  0x1 : Erratic Error
 */
#define USB_ERRTICERR    (BIT(3))
#define USB_ERRTICERR_M  (USB_ERRTICERR_V << USB_ERRTICERR_S)
#define USB_ERRTICERR_V  0x00000001
#define USB_ERRTICERR_S  3
/** USB_SOFFN : RO; bitpos: [22:8]; default: 0;
 *  Frame or Microframe Number of the Received SOF (SOFFN).  This field contains a
 *  Frame number.
 */
#define USB_SOFFN    0x00003FFF
#define USB_SOFFN_M  (USB_SOFFN_V << USB_SOFFN_S)
#define USB_SOFFN_V  0x00003FFF
#define USB_SOFFN_S  8
/** USB_DEVLNSTS : RO; bitpos: [24:22]; default: 0;
 *  Device Line Status
 *  DevLnSts[1]: Logic level of D+
 *  DevLnSts[0]: Logic level of D-
 */
#define USB_DEVLNSTS    0x00000003
#define USB_DEVLNSTS_M  (USB_DEVLNSTS_V << USB_DEVLNSTS_S)
#define USB_DEVLNSTS_V  0x00000003
#define USB_DEVLNSTS_S  22


/** USB_DTXFSTS0_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 0
 */
#define USB_DTXFSTS0_REG (SOC_DPORT_USB_BASE + 0x918)
/** USB_D_INEPTXFSPCAVAIL0 : RO; bitpos: [16:0]; default: 0;
 *  Indicates the amount of free space available in the Endpoint  TxFIFO.
 */
#define USB_D_INEPTXFSPCAVAIL0    0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL0_M  (USB_D_INEPTXFSPCAVAIL0_V << USB_D_INEPTXFSPCAVAIL0_S)
#define USB_D_INEPTXFSPCAVAIL0_V  0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL0_S  0


/** USB_DIEPDMAB0_REG register
 *  Device IN Endpoint 16 Buffer Address Register
 */
#define USB_DIEPDMAB0_REG (SOC_DPORT_USB_BASE + 0x91c)
/** USB_D_DMABUFFERADDR0 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_D_DMABUFFERADDR0    0xFFFFFFFF
#define USB_D_DMABUFFERADDR0_M  (USB_D_DMABUFFERADDR0_V << USB_D_DMABUFFERADDR0_S)
#define USB_D_DMABUFFERADDR0_V  0xFFFFFFFF
#define USB_D_DMABUFFERADDR0_S  0


/** USB_DTXFSTS1_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 1
 */
#define USB_DTXFSTS1_REG (SOC_DPORT_USB_BASE + 0x938)
/** USB_D_INEPTXFSPCAVAIL1 : RO; bitpos: [16:0]; default: 0;
 *  Indicates the amount of free space available in the Endpoint  TxFIFO.
 */
#define USB_D_INEPTXFSPCAVAIL1    0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL1_M  (USB_D_INEPTXFSPCAVAIL1_V << USB_D_INEPTXFSPCAVAIL1_S)
#define USB_D_INEPTXFSPCAVAIL1_V  0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL1_S  0


/** USB_DIEPDMAB1_REG register
 *  Device IN Endpoint 16 Buffer Address Register
 */
#define USB_DIEPDMAB1_REG (SOC_DPORT_USB_BASE + 0x93c)
/** USB_D_DMABUFFERADDR1 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_D_DMABUFFERADDR1    0xFFFFFFFF
#define USB_D_DMABUFFERADDR1_M  (USB_D_DMABUFFERADDR1_V << USB_D_DMABUFFERADDR1_S)
#define USB_D_DMABUFFERADDR1_V  0xFFFFFFFF
#define USB_D_DMABUFFERADDR1_S  0


/** USB_DTXFSTS2_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 2
 */
#define USB_DTXFSTS2_REG (SOC_DPORT_USB_BASE + 0x958)
/** USB_D_INEPTXFSPCAVAIL2 : RO; bitpos: [16:0]; default: 0;
 *  Indicates the amount of free space available in the Endpoint  TxFIFO.
 */
#define USB_D_INEPTXFSPCAVAIL2    0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL2_M  (USB_D_INEPTXFSPCAVAIL2_V << USB_D_INEPTXFSPCAVAIL2_S)
#define USB_D_INEPTXFSPCAVAIL2_V  0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL2_S  0


/** USB_DIEPDMAB2_REG register
 *  Device IN Endpoint 16 Buffer Address Register
 */
#define USB_DIEPDMAB2_REG (SOC_DPORT_USB_BASE + 0x95c)
/** USB_D_DMABUFFERADDR2 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_D_DMABUFFERADDR2    0xFFFFFFFF
#define USB_D_DMABUFFERADDR2_M  (USB_D_DMABUFFERADDR2_V << USB_D_DMABUFFERADDR2_S)
#define USB_D_DMABUFFERADDR2_V  0xFFFFFFFF
#define USB_D_DMABUFFERADDR2_S  0


/** USB_DTXFSTS3_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 3
 */
#define USB_DTXFSTS3_REG (SOC_DPORT_USB_BASE + 0x978)
/** USB_D_INEPTXFSPCAVAIL3 : RO; bitpos: [16:0]; default: 0;
 *  Indicates the amount of free space available in the Endpoint  TxFIFO.
 */
#define USB_D_INEPTXFSPCAVAIL3    0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL3_M  (USB_D_INEPTXFSPCAVAIL3_V << USB_D_INEPTXFSPCAVAIL3_S)
#define USB_D_INEPTXFSPCAVAIL3_V  0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL3_S  0


/** USB_DIEPDMAB3_REG register
 *  Device IN Endpoint 16 Buffer Address Register
 */
#define USB_DIEPDMAB3_REG (SOC_DPORT_USB_BASE + 0x97c)
/** USB_D_DMABUFFERADDR3 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_D_DMABUFFERADDR3    0xFFFFFFFF
#define USB_D_DMABUFFERADDR3_M  (USB_D_DMABUFFERADDR3_V << USB_D_DMABUFFERADDR3_S)
#define USB_D_DMABUFFERADDR3_V  0xFFFFFFFF
#define USB_D_DMABUFFERADDR3_S  0


/** USB_DTXFSTS4_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 4
 */
#define USB_DTXFSTS4_REG (SOC_DPORT_USB_BASE + 0x998)
/** USB_D_INEPTXFSPCAVAIL4 : RO; bitpos: [16:0]; default: 0;
 *  Indicates the amount of free space available in the Endpoint  TxFIFO.
 */
#define USB_D_INEPTXFSPCAVAIL4    0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL4_M  (USB_D_INEPTXFSPCAVAIL4_V << USB_D_INEPTXFSPCAVAIL4_S)
#define USB_D_INEPTXFSPCAVAIL4_V  0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL4_S  0


/** USB_DIEPDMAB4_REG register
 *  Device IN Endpoint 16 Buffer Address Register
 */
#define USB_DIEPDMAB4_REG (SOC_DPORT_USB_BASE + 0x99c)
/** USB_D_DMABUFFERADDR4 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_D_DMABUFFERADDR4    0xFFFFFFFF
#define USB_D_DMABUFFERADDR4_M  (USB_D_DMABUFFERADDR4_V << USB_D_DMABUFFERADDR4_S)
#define USB_D_DMABUFFERADDR4_V  0xFFFFFFFF
#define USB_D_DMABUFFERADDR4_S  0


/** USB_DTXFSTS5_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 5
 */
#define USB_DTXFSTS5_REG (SOC_DPORT_USB_BASE + 0x9b8)
/** USB_D_INEPTXFSPCAVAIL5 : RO; bitpos: [16:0]; default: 0;
 *  Indicates the amount of free space available in the Endpoint  TxFIFO.
 */
#define USB_D_INEPTXFSPCAVAIL5    0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL5_M  (USB_D_INEPTXFSPCAVAIL5_V << USB_D_INEPTXFSPCAVAIL5_S)
#define USB_D_INEPTXFSPCAVAIL5_V  0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL5_S  0


/** USB_DIEPDMAB5_REG register
 *  Device IN Endpoint 16 Buffer Address Register
 */
#define USB_DIEPDMAB5_REG (SOC_DPORT_USB_BASE + 0x9bc)
/** USB_D_DMABUFFERADDR5 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_D_DMABUFFERADDR5    0xFFFFFFFF
#define USB_D_DMABUFFERADDR5_M  (USB_D_DMABUFFERADDR5_V << USB_D_DMABUFFERADDR5_S)
#define USB_D_DMABUFFERADDR5_V  0xFFFFFFFF
#define USB_D_DMABUFFERADDR5_S  0


/** USB_DTXFSTS6_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 6
 */
#define USB_DTXFSTS6_REG (SOC_DPORT_USB_BASE + 0x9d8)
/** USB_D_INEPTXFSPCAVAIL6 : RO; bitpos: [16:0]; default: 0;
 *  Indicates the amount of free space available in the Endpoint  TxFIFO.
 */
#define USB_D_INEPTXFSPCAVAIL6    0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL6_M  (USB_D_INEPTXFSPCAVAIL6_V << USB_D_INEPTXFSPCAVAIL6_S)
#define USB_D_INEPTXFSPCAVAIL6_V  0x0000FFFF
#define USB_D_INEPTXFSPCAVAIL6_S  0


/** USB_DIEPDMAB6_REG register
 *  Device IN Endpoint 16 Buffer Address Register
 */
#define USB_DIEPDMAB6_REG (SOC_DPORT_USB_BASE + 0x9dc)
/** USB_D_DMABUFFERADDR6 : RO; bitpos: [32:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress. This register is present
 *  only in Scatter/Gather DMA mode. Otherwise this field is reserved.
 */
#define USB_D_DMABUFFERADDR6    0xFFFFFFFF
#define USB_D_DMABUFFERADDR6_M  (USB_D_DMABUFFERADDR6_V << USB_D_DMABUFFERADDR6_S)
#define USB_D_DMABUFFERADDR6_V  0xFFFFFFFF
#define USB_D_DMABUFFERADDR6_S  0




#ifdef __cplusplus
}
#endif
