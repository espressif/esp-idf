/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** USB_OTGFS_GOTGCTL_REG register
 *  OTG Control and Status Register The OTG Control and Status register controls the
 *  behavior and reflects the status of the OTG function of the controller.
 */
#define USB_OTGFS_GOTGCTL_REG(i) (DR_REG_USB_BASE(i) + 0x0)
/** USB_OTGFS_GOTGCTL_SESREQSCS : RO; bitpos: [0]; default: 0;
 *  Mode: Device only
 *  Session Request Success (SesReqScs)
 *  The core sets this bit when a session request initiation is successful.
 *  - 1'b0: Session request failure
 *  - 1'b1: Session request success
 */
#define USB_OTGFS_GOTGCTL_SESREQSCS    (BIT(0))
#define USB_OTGFS_GOTGCTL_SESREQSCS_M  (USB_OTGFS_GOTGCTL_SESREQSCS_V << USB_OTGFS_GOTGCTL_SESREQSCS_S)
#define USB_OTGFS_GOTGCTL_SESREQSCS_V  0x00000001U
#define USB_OTGFS_GOTGCTL_SESREQSCS_S  0
/** USB_OTGFS_GOTGCTL_SESREQ : R/W; bitpos: [1]; default: 0;
 *  Mode: Device only
 *  Session Request (SesReq)
 *  The application sets this bit to initiate a session request on the USB. The
 *  application can clear this bit by writing a 0 when the Host Negotiation Success
 *  Status Change bit in the OTG Interrupt register (GOTGINT.HstNegSucStsChng) is SET.
 *  The core clears this bit when the HstNegSucStsChng bit is cleared.
 *  If you use the USB 1.1 Full-Speed Serial Transceiver interface to initiate the
 *  session request, the application must wait until the VBUS discharges to 0.2 V,
 *  after the B-Session Valid bit in this register (GOTGCTL.BSesVld) is cleared. This
 *  discharge time varies between different PHYs and can be obtained from the PHY
 *  vendor.
 *  - 1'b0: No session request
 *  - 1'b1: Session request
 */
#define USB_OTGFS_GOTGCTL_SESREQ    (BIT(1))
#define USB_OTGFS_GOTGCTL_SESREQ_M  (USB_OTGFS_GOTGCTL_SESREQ_V << USB_OTGFS_GOTGCTL_SESREQ_S)
#define USB_OTGFS_GOTGCTL_SESREQ_V  0x00000001U
#define USB_OTGFS_GOTGCTL_SESREQ_S  1
/** USB_OTGFS_GOTGCTL_VBVALIDOVEN : R/W; bitpos: [2]; default: 0;
 *  Mode: Host only
 *  VBUS Valid Override Enable (VbvalidOvEn)
 *  This bit is used to enable/disable the software to override the Bvalid signal using
 *  the GOTGCTL.VbvalidOvVal.
 *  - 1'b1 : Internally Bvalid received from the PHY is overridden with
 *  GOTGCTL.VbvalidOvVal.
 *  - 1'b0 : Override is disabled and bvalid signal from the respective PHY selected is
 *  used internally by the controller.
 */
#define USB_OTGFS_GOTGCTL_VBVALIDOVEN    (BIT(2))
#define USB_OTGFS_GOTGCTL_VBVALIDOVEN_M  (USB_OTGFS_GOTGCTL_VBVALIDOVEN_V << USB_OTGFS_GOTGCTL_VBVALIDOVEN_S)
#define USB_OTGFS_GOTGCTL_VBVALIDOVEN_V  0x00000001U
#define USB_OTGFS_GOTGCTL_VBVALIDOVEN_S  2
/** USB_OTGFS_GOTGCTL_VBVALIDOVVAL : R/W; bitpos: [3]; default: 0;
 *  Mode: Host only
 *  VBUS Valid OverrideValue (VbvalidOvVal)
 *  This bit is used to set Override value for vbusvalid signal when
 *  GOTGCTL.VbvalidOvEn is set.
 *  - 1'b0 : vbusvalid value is 1'b0 when GOTGCTL.VbvalidOvEn =1
 *  - 1'b1 : vbusvalid value is 1'b1 when GOTGCTL.VbvalidOvEn =1
 */
#define USB_OTGFS_GOTGCTL_VBVALIDOVVAL    (BIT(3))
#define USB_OTGFS_GOTGCTL_VBVALIDOVVAL_M  (USB_OTGFS_GOTGCTL_VBVALIDOVVAL_V << USB_OTGFS_GOTGCTL_VBVALIDOVVAL_S)
#define USB_OTGFS_GOTGCTL_VBVALIDOVVAL_V  0x00000001U
#define USB_OTGFS_GOTGCTL_VBVALIDOVVAL_S  3
/** USB_OTGFS_GOTGCTL_AVALIDOVEN : R/W; bitpos: [4]; default: 0;
 *  Mode: Host only
 *  A-Peripheral Session Valid Override Enable (AvalidOvEn)
 *  This bit is used to enable/disable the software to override the Avalid signal using
 *  the GOTGCTL.AvalidOvVal.
 *  - 1'b1: Internally Avalid received from the PHY is overridden with
 *  GOTGCTL.AvalidOvVal.
 *  - 1'b0: Override is disabled and avalid signal from the respective PHY selected is
 *  used internally by the core
 */
#define USB_OTGFS_GOTGCTL_AVALIDOVEN    (BIT(4))
#define USB_OTGFS_GOTGCTL_AVALIDOVEN_M  (USB_OTGFS_GOTGCTL_AVALIDOVEN_V << USB_OTGFS_GOTGCTL_AVALIDOVEN_S)
#define USB_OTGFS_GOTGCTL_AVALIDOVEN_V  0x00000001U
#define USB_OTGFS_GOTGCTL_AVALIDOVEN_S  4
/** USB_OTGFS_GOTGCTL_AVALIDOVVAL : R/W; bitpos: [5]; default: 0;
 *  Mode: Host only
 *  A-Peripheral Session Valid OverrideValue (AvalidOvVal)
 *  This bit is used to set Override value for Avalid signal when GOTGCTL.AvalidOvEn is
 *  set.
 *  - 1'b0 : Avalid value is 1'b0 when GOTGCTL.AvalidOvEn =1
 *  - 1'b1 : Avalid value is 1'b1 when GOTGCTL.AvalidOvEn =1
 */
#define USB_OTGFS_GOTGCTL_AVALIDOVVAL    (BIT(5))
#define USB_OTGFS_GOTGCTL_AVALIDOVVAL_M  (USB_OTGFS_GOTGCTL_AVALIDOVVAL_V << USB_OTGFS_GOTGCTL_AVALIDOVVAL_S)
#define USB_OTGFS_GOTGCTL_AVALIDOVVAL_V  0x00000001U
#define USB_OTGFS_GOTGCTL_AVALIDOVVAL_S  5
/** USB_OTGFS_GOTGCTL_BVALIDOVEN : R/W; bitpos: [6]; default: 0;
 *  Mode: Device only
 *  B-Peripheral Session Valid Override Value (BvalidOvEn)
 *  This bit is used to enable/disable the software to override the Bvalid signal using
 *  the GOTGCTL.BvalidOvVal.
 *  - 1'b1 : Internally Bvalid received from the PHY is overridden with
 *  GOTGCTL.BvalidOvVal.
 *  - 1'b0 : Override is disabled and bvalid signal from the respective PHY selected is
 *  used internally by the force
 */
#define USB_OTGFS_GOTGCTL_BVALIDOVEN    (BIT(6))
#define USB_OTGFS_GOTGCTL_BVALIDOVEN_M  (USB_OTGFS_GOTGCTL_BVALIDOVEN_V << USB_OTGFS_GOTGCTL_BVALIDOVEN_S)
#define USB_OTGFS_GOTGCTL_BVALIDOVEN_V  0x00000001U
#define USB_OTGFS_GOTGCTL_BVALIDOVEN_S  6
/** USB_OTGFS_GOTGCTL_BVALIDOVVAL : R/W; bitpos: [7]; default: 0;
 *  Mode: Device only
 *  B-Peripheral Session Valid OverrideValue (BvalidOvVal)
 *  This bit is used to set Override value for Bvalid signal when GOTGCTL.BvalidOvEn is
 *  set.
 *  - 1'b0 : Bvalid value is 1'b0 when GOTGCTL.BvalidOvEn =1
 *  - 1'b1 : Bvalid value is 1'b1 when GOTGCTL.BvalidOvEn =1
 */
#define USB_OTGFS_GOTGCTL_BVALIDOVVAL    (BIT(7))
#define USB_OTGFS_GOTGCTL_BVALIDOVVAL_M  (USB_OTGFS_GOTGCTL_BVALIDOVVAL_V << USB_OTGFS_GOTGCTL_BVALIDOVVAL_S)
#define USB_OTGFS_GOTGCTL_BVALIDOVVAL_V  0x00000001U
#define USB_OTGFS_GOTGCTL_BVALIDOVVAL_S  7
/** USB_OTGFS_GOTGCTL_HSTNEGSCS : RO; bitpos: [8]; default: 0;
 *  Mode: HNP-capable Device
 *  Host Negotiation Success (HstNegScs)
 *  The controller sets this bit when host negotiation is successful. The controller
 *  clears this bit when the HNP Request (HNPReq) bit in this register is set.
 *  - 1'b0: Host negotiation failure
 *  - 1'b1: Host negotiation success
 */
#define USB_OTGFS_GOTGCTL_HSTNEGSCS    (BIT(8))
#define USB_OTGFS_GOTGCTL_HSTNEGSCS_M  (USB_OTGFS_GOTGCTL_HSTNEGSCS_V << USB_OTGFS_GOTGCTL_HSTNEGSCS_S)
#define USB_OTGFS_GOTGCTL_HSTNEGSCS_V  0x00000001U
#define USB_OTGFS_GOTGCTL_HSTNEGSCS_S  8
/** USB_OTGFS_GOTGCTL_HNPREQ : R/W; bitpos: [9]; default: 0;
 *  Mode: HNP Capable OTG Device
 *  HNP Request (HNPReq)
 *  The application sets this bit to initiate an HNP request to the connected USB host.
 *  The application can clear this bit by writing a 0 when the Host Negotiation Success
 *  Status Change bit in the OTG Interrupt register (GOTGINT.HstNegSucStsChng) is SET.
 *  The controller clears this bit when the HstNegSucStsChng bit is cleared.
 *  - 1'b0: No HNP request
 *  - 1'b1: HNP request
 */
#define USB_OTGFS_GOTGCTL_HNPREQ    (BIT(9))
#define USB_OTGFS_GOTGCTL_HNPREQ_M  (USB_OTGFS_GOTGCTL_HNPREQ_V << USB_OTGFS_GOTGCTL_HNPREQ_S)
#define USB_OTGFS_GOTGCTL_HNPREQ_V  0x00000001U
#define USB_OTGFS_GOTGCTL_HNPREQ_S  9
/** USB_OTGFS_GOTGCTL_HSTSETHNPEN : R/W; bitpos: [10]; default: 0;
 *  Mode: HNP Capable OTG Host
 *  Host Set HNP Enable (HstSetHNPEn)
 *  The application sets this bit when it has successfully enabled HNP (using the
 *  SetFeature.SetHNPEnable command) on the connected device.
 *  - 1'b0: Host Set HNP is not enabled
 *  - 1'b1: Host Set HNP is enabled
 */
#define USB_OTGFS_GOTGCTL_HSTSETHNPEN    (BIT(10))
#define USB_OTGFS_GOTGCTL_HSTSETHNPEN_M  (USB_OTGFS_GOTGCTL_HSTSETHNPEN_V << USB_OTGFS_GOTGCTL_HSTSETHNPEN_S)
#define USB_OTGFS_GOTGCTL_HSTSETHNPEN_V  0x00000001U
#define USB_OTGFS_GOTGCTL_HSTSETHNPEN_S  10
/** USB_OTGFS_GOTGCTL_DEVHNPEN : R/W; bitpos: [11]; default: 0;
 *  Mode: HNP Capable OTG Device
 *  Device HNP Enabled (DevHNPEn)
 *  The application sets this bit when it successfully receives a
 *  SetFeature.SetHNPEnable command from the connected USB host.
 *  - 1'b0: HNP is not enabled in the application
 *  - 1'b1: HNP is enabled in the application
 */
#define USB_OTGFS_GOTGCTL_DEVHNPEN    (BIT(11))
#define USB_OTGFS_GOTGCTL_DEVHNPEN_M  (USB_OTGFS_GOTGCTL_DEVHNPEN_V << USB_OTGFS_GOTGCTL_DEVHNPEN_S)
#define USB_OTGFS_GOTGCTL_DEVHNPEN_V  0x00000001U
#define USB_OTGFS_GOTGCTL_DEVHNPEN_S  11
/** USB_OTGFS_GOTGCTL_EHEN : R/W; bitpos: [12]; default: 0;
 *  Mode: SRP Capable Host
 *  Embedded Host Enable
 *  It is used to select between OTG A Device state Machine and Embedded Host state
 *  machine.
 *  - 1'b0: OTG A Device state machine is selected
 *  - 1'b1: Embedded Host State Machine is selected
 *  Note:
 *  This field is valid only in SRP-Capable OTG Mode (OTG_MODE=0,1).
 */
#define USB_OTGFS_GOTGCTL_EHEN    (BIT(12))
#define USB_OTGFS_GOTGCTL_EHEN_M  (USB_OTGFS_GOTGCTL_EHEN_V << USB_OTGFS_GOTGCTL_EHEN_S)
#define USB_OTGFS_GOTGCTL_EHEN_V  0x00000001U
#define USB_OTGFS_GOTGCTL_EHEN_S  12
/** USB_OTGFS_GOTGCTL_DBNCEFLTRBYPASS : R/W; bitpos: [15]; default: 0;
 *  Mode: Host and Device
 *  Debounce Filter Bypass
 *  Bypass Debounce filters for avalid, bvalid, vbusvalid, sessend, iddig signals when
 *  enabled.
 *  - 1'b0: Disabled
 *  - 1'b1: Enabled
 *
 *
 *  Note:  This register bit is valid only when debounce filters are present in core.
 */
#define USB_OTGFS_GOTGCTL_DBNCEFLTRBYPASS    (BIT(15))
#define USB_OTGFS_GOTGCTL_DBNCEFLTRBYPASS_M  (USB_OTGFS_GOTGCTL_DBNCEFLTRBYPASS_V << USB_OTGFS_GOTGCTL_DBNCEFLTRBYPASS_S)
#define USB_OTGFS_GOTGCTL_DBNCEFLTRBYPASS_V  0x00000001U
#define USB_OTGFS_GOTGCTL_DBNCEFLTRBYPASS_S  15
/** USB_OTGFS_GOTGCTL_CONIDSTS : RO; bitpos: [16]; default: 1;
 *  Mode: Host and Device
 *  Connector ID Status (ConIDSts)
 *  Indicates the connector ID status on a connect event.
 *  - 1'b0: The core is in A-Device mode.
 *  - 1'b1: The core is in B-Device mode.
 *
 *  Note:
 *  The reset value of this register field can be read only after the PHY clock is
 *  stable, or if IDDIG_FILTER is enabled, wait for the filter timer to expire to read
 *  the correct reset value which ever event is later.
 *  Reset:
 *  - 1'b0: in host only mode (OTG_MODE = 5 or 6)
 *  - 1'b1: in all other configurations
 */
#define USB_OTGFS_GOTGCTL_CONIDSTS    (BIT(16))
#define USB_OTGFS_GOTGCTL_CONIDSTS_M  (USB_OTGFS_GOTGCTL_CONIDSTS_V << USB_OTGFS_GOTGCTL_CONIDSTS_S)
#define USB_OTGFS_GOTGCTL_CONIDSTS_V  0x00000001U
#define USB_OTGFS_GOTGCTL_CONIDSTS_S  16
/** USB_OTGFS_GOTGCTL_DBNCTIME : RO; bitpos: [17]; default: 0;
 *  Mode: Host only
 *  Long/Short Debounce Time (DbncTime)
 *  Indicates the debounce time of a detected connection.
 *  - 1'b0: Long debounce time, used for physical connections (100 ms + 2.5 micro-sec)
 *  - 1'b1: Short debounce time, used for soft connections (2.5 micro-sec)
 */
#define USB_OTGFS_GOTGCTL_DBNCTIME    (BIT(17))
#define USB_OTGFS_GOTGCTL_DBNCTIME_M  (USB_OTGFS_GOTGCTL_DBNCTIME_V << USB_OTGFS_GOTGCTL_DBNCTIME_S)
#define USB_OTGFS_GOTGCTL_DBNCTIME_V  0x00000001U
#define USB_OTGFS_GOTGCTL_DBNCTIME_S  17
/** USB_OTGFS_GOTGCTL_ASESVLD : RO; bitpos: [18]; default: 0;
 *  Mode: Host only
 *  A-Session Valid (ASesVld)
 *  Indicates the Host mode transceiver status.
 *  - 1'b0: A-session is not valid
 *  - 1'b1: A-session is valid
 *  Note: If you do not enabled OTG features (such as SRP and HNP), the read reset
 *  value will be 1. The vbus assigns the values internally for non-SRP or non-HNP
 *  configurations.
 *  In case of OTG_MODE=0, the reset value of this bit is 1'b0.
 */
#define USB_OTGFS_GOTGCTL_ASESVLD    (BIT(18))
#define USB_OTGFS_GOTGCTL_ASESVLD_M  (USB_OTGFS_GOTGCTL_ASESVLD_V << USB_OTGFS_GOTGCTL_ASESVLD_S)
#define USB_OTGFS_GOTGCTL_ASESVLD_V  0x00000001U
#define USB_OTGFS_GOTGCTL_ASESVLD_S  18
/** USB_OTGFS_GOTGCTL_BSESVLD : RO; bitpos: [19]; default: 0;
 *  Mode: Device only
 *  B-Session Valid (BSesVld)
 *  Indicates the Device mode transceiver status.
 *  - 1'b0: B-session is not valid.
 *  - 1'b1: B-session is valid.
 *  In OTG mode, you can use this bit to determine if the device is connected or
 *  disconnected.
 *
 *  Note:
 *  - If you do not enable OTG features (such as SRP and HNP), the read reset value
 *  will be 1.The vbus assigns the values internally for non- SRP or non-HNP
 *  configurations.
 *  - In case of OTG_MODE=0, the reset value of this bit is 1'b0.
 *  - The reset value of this register field can be read only after the PHY clock is
 *  stable, or if IDDIG_FILTER is enabled, wait for the filter timer to expire to read
 *  the correct reset value which ever event is later.
 */
#define USB_OTGFS_GOTGCTL_BSESVLD    (BIT(19))
#define USB_OTGFS_GOTGCTL_BSESVLD_M  (USB_OTGFS_GOTGCTL_BSESVLD_V << USB_OTGFS_GOTGCTL_BSESVLD_S)
#define USB_OTGFS_GOTGCTL_BSESVLD_V  0x00000001U
#define USB_OTGFS_GOTGCTL_BSESVLD_S  19
/** USB_OTGFS_GOTGCTL_OTGVER : R/W; bitpos: [20]; default: 0;
 *  OTG Version (OTGVer)
 *  Indicates the OTG revision.
 *  - 1'b0: OTG Version 1.3. In this version the core supports Data line pulsing and
 *  VBus pulsing for SRP.
 *  - 1'b1: OTG Version 2.0. In this version the core supports only Data line pulsing
 *  for SRP.
 */
#define USB_OTGFS_GOTGCTL_OTGVER    (BIT(20))
#define USB_OTGFS_GOTGCTL_OTGVER_M  (USB_OTGFS_GOTGCTL_OTGVER_V << USB_OTGFS_GOTGCTL_OTGVER_S)
#define USB_OTGFS_GOTGCTL_OTGVER_V  0x00000001U
#define USB_OTGFS_GOTGCTL_OTGVER_S  20
/** USB_OTGFS_GOTGCTL_CURMOD : RO; bitpos: [21]; default: 0;
 *  Current Mode of Operation (CurMod)
 *  Mode: Host and Device
 *  Indicates the current mode.
 *  - 1'b0: Device mode
 *  - 1'b1: Host mode
 *  Reset:
 *  - 1'b1 in Host-only mode (OTG_MODE=5 or 6)
 *  - 1'b0 in all other configurations
 *  Note: The reset value of this register field can be read only after the PHY clock
 *  is stable, or if IDDIG_FILTER is enabled, wait for the filter timer to expire to
 *  read the correct reset value which ever event is later.
 */
#define USB_OTGFS_GOTGCTL_CURMOD    (BIT(21))
#define USB_OTGFS_GOTGCTL_CURMOD_M  (USB_OTGFS_GOTGCTL_CURMOD_V << USB_OTGFS_GOTGCTL_CURMOD_S)
#define USB_OTGFS_GOTGCTL_CURMOD_V  0x00000001U
#define USB_OTGFS_GOTGCTL_CURMOD_S  21

/** USB_OTGFS_GOTGINT_REG register
 *  OTG Interrupt Register The application reads this register whenever there is an OTG
 *  interrupt and clears the bits in this register to clear the OTG interrupt.
 */
#define USB_OTGFS_GOTGINT_REG(i) (DR_REG_USB_BASE(i) + 0x4)
/** USB_OTGFS_GOTGINT_SESENDDET : R/W; bitpos: [2]; default: 0;
 *  Mode: Host and Device
 *  Session End Detected (SesEndDet)
 *  The controller sets this bit when the utmiotg_bvalid signal is deasserted. This bit
 *  can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_GOTGINT_SESENDDET    (BIT(2))
#define USB_OTGFS_GOTGINT_SESENDDET_M  (USB_OTGFS_GOTGINT_SESENDDET_V << USB_OTGFS_GOTGINT_SESENDDET_S)
#define USB_OTGFS_GOTGINT_SESENDDET_V  0x00000001U
#define USB_OTGFS_GOTGINT_SESENDDET_S  2
/** USB_OTGFS_GOTGINT_SESREQSUCSTSCHNG : R/W; bitpos: [8]; default: 0;
 *  Mode: Host and Device
 *  Session Request Success Status Change (SesReqSucStsChng)
 *  The core sets this bit on the success or failure of a session request. The
 *  application must read the Session Request Success bit in the OTG Control and Status
 *  register (GOTGCTL.SesReqScs) to check for success or failure. This bit can be set
 *  only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_GOTGINT_SESREQSUCSTSCHNG    (BIT(8))
#define USB_OTGFS_GOTGINT_SESREQSUCSTSCHNG_M  (USB_OTGFS_GOTGINT_SESREQSUCSTSCHNG_V << USB_OTGFS_GOTGINT_SESREQSUCSTSCHNG_S)
#define USB_OTGFS_GOTGINT_SESREQSUCSTSCHNG_V  0x00000001U
#define USB_OTGFS_GOTGINT_SESREQSUCSTSCHNG_S  8
/** USB_OTGFS_GOTGINT_HSTNEGSUCSTSCHNG : R/W; bitpos: [9]; default: 0;
 *  Mode: Host and Device
 *  Host Negotiation Success Status Change (HstNegSucStsChng)
 *  The core sets this bit on the success or failure of a USB host negotiation request.
 *  The application must read the Host Negotiation Success bit of the OTG Control and
 *  Status register (GOTGCTL.HstNegScs) to check for success or failure. This bit can
 *  be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_GOTGINT_HSTNEGSUCSTSCHNG    (BIT(9))
#define USB_OTGFS_GOTGINT_HSTNEGSUCSTSCHNG_M  (USB_OTGFS_GOTGINT_HSTNEGSUCSTSCHNG_V << USB_OTGFS_GOTGINT_HSTNEGSUCSTSCHNG_S)
#define USB_OTGFS_GOTGINT_HSTNEGSUCSTSCHNG_V  0x00000001U
#define USB_OTGFS_GOTGINT_HSTNEGSUCSTSCHNG_S  9
/** USB_OTGFS_GOTGINT_HSTNEGDET : R/W; bitpos: [17]; default: 0;
 *  Mode:Host and Device
 *  Host Negotiation Detected (HstNegDet)
 *  The core sets this bit when it detects a host negotiation request on the USB. This
 *  bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_GOTGINT_HSTNEGDET    (BIT(17))
#define USB_OTGFS_GOTGINT_HSTNEGDET_M  (USB_OTGFS_GOTGINT_HSTNEGDET_V << USB_OTGFS_GOTGINT_HSTNEGDET_S)
#define USB_OTGFS_GOTGINT_HSTNEGDET_V  0x00000001U
#define USB_OTGFS_GOTGINT_HSTNEGDET_S  17
/** USB_OTGFS_GOTGINT_ADEVTOUTCHG : R/W; bitpos: [18]; default: 0;
 *  Mode: Host and Device
 *  A-Device Timeout Change (ADevTOUTChg)
 *  The core sets this bit to indicate that the A-device has timed out while waiting
 *  for the B-device to connect.This bit can be set only by the core and the
 *  application should write 1 to clear it.
 */
#define USB_OTGFS_GOTGINT_ADEVTOUTCHG    (BIT(18))
#define USB_OTGFS_GOTGINT_ADEVTOUTCHG_M  (USB_OTGFS_GOTGINT_ADEVTOUTCHG_V << USB_OTGFS_GOTGINT_ADEVTOUTCHG_S)
#define USB_OTGFS_GOTGINT_ADEVTOUTCHG_V  0x00000001U
#define USB_OTGFS_GOTGINT_ADEVTOUTCHG_S  18
/** USB_OTGFS_GOTGINT_DBNCEDONE : R/W; bitpos: [19]; default: 0;
 *  Mode: Host only
 *  Debounce Done (DbnceDone)
 *  The core sets this bit when the debounce is completed after the device connect. The
 *  application can start driving USB reset after seeing this interrupt. This bit is
 *  only valid when the HNP Capable or SRP Capable bit is SET in the Core USB
 *  Configuration register (GUSBCFG.HNPCap or GUSBCFG.SRPCap, respectively). This bit
 *  can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_GOTGINT_DBNCEDONE    (BIT(19))
#define USB_OTGFS_GOTGINT_DBNCEDONE_M  (USB_OTGFS_GOTGINT_DBNCEDONE_V << USB_OTGFS_GOTGINT_DBNCEDONE_S)
#define USB_OTGFS_GOTGINT_DBNCEDONE_V  0x00000001U
#define USB_OTGFS_GOTGINT_DBNCEDONE_S  19

/** USB_OTGFS_GAHBCFG_REG register
 *  AHB Configuration Register This register can be used to configure the core after
 *  power-on or a change in mode. This register mainly contains AHB system-related
 *  configuration parameters. Do not change this register after the initial
 *  programming. The application must program this register before starting any
 *  transactions on either the AHB or the USB.
 */
#define USB_OTGFS_GAHBCFG_REG(i) (DR_REG_USB_BASE(i) + 0x8)
/** USB_OTGFS_GAHBCFG_GLBLINTRMSK : R/W; bitpos: [0]; default: 0;
 *  Mode: Host and device
 *  Global Interrupt Mask (GlblIntrMsk)
 *  The application uses this bit to mask or unmask the interrupt line assertion to
 *  itself. Irrespective of this bit's setting, the interrupt status registers are
 *  updated by the controller.
 *  - 1'b0: Mask the interrupt assertion to the application.
 *  - 1'b1: Unmask the interrupt assertion to the application.
 */
#define USB_OTGFS_GAHBCFG_GLBLINTRMSK    (BIT(0))
#define USB_OTGFS_GAHBCFG_GLBLINTRMSK_M  (USB_OTGFS_GAHBCFG_GLBLINTRMSK_V << USB_OTGFS_GAHBCFG_GLBLINTRMSK_S)
#define USB_OTGFS_GAHBCFG_GLBLINTRMSK_V  0x00000001U
#define USB_OTGFS_GAHBCFG_GLBLINTRMSK_S  0
/** USB_OTGFS_GAHBCFG_HBSTLEN : R/W; bitpos: [4:1]; default: 0;
 *  Mode: Host and device
 *  Burst Length/Type (HBstLen)
 *  This field is used in both External and Internal DMA modes. In External DMA mode,
 *  these bits appear on dma_burst[3:0] ports, which can be used by an external wrapper
 *  to interface the External DMA Controller interface to Synopsis DW_ahb_dmac or ARM
 *  PrimeCell.
 *  External DMA Mode defines the DMA burst length in terms of 32-bit words:
 *  - 4'b0000: 1 word
 *  - 4'b0001: 4 words
 *  - 4'b0010: 8 words
 *  - 4'b0011: 16 words
 *  - 4'b0100: 32 words
 *  - 4'b0101: 64 words
 *  - 4'b0110: 128 words
 *  - 4'b0111: 256 words
 *  - Others: Reserved
 *  Internal DMA Mode AHB Master burst type:
 *  - 4'b0000 Single
 *  - 4'b0001 INCR
 *  - 4'b0011 INCR4
 *  - 4'b0101 INCR8
 *  - 4'b0111 INCR16
 *  - Others: Reserved
 */
#define USB_OTGFS_GAHBCFG_HBSTLEN    0x0000000FU
#define USB_OTGFS_GAHBCFG_HBSTLEN_M  (USB_OTGFS_GAHBCFG_HBSTLEN_V << USB_OTGFS_GAHBCFG_HBSTLEN_S)
#define USB_OTGFS_GAHBCFG_HBSTLEN_V  0x0000000FU
#define USB_OTGFS_GAHBCFG_HBSTLEN_S  1
/** USB_OTGFS_GAHBCFG_DMAEN : R/W; bitpos: [5]; default: 0;
 *  Mode: Host and device
 *  DMA Enable (DMAEn)
 *
 *  This bit is always 0 when Slave-Only mode has been selected.
 *
 *  Reset: 1'b0
 */
#define USB_OTGFS_GAHBCFG_DMAEN    (BIT(5))
#define USB_OTGFS_GAHBCFG_DMAEN_M  (USB_OTGFS_GAHBCFG_DMAEN_V << USB_OTGFS_GAHBCFG_DMAEN_S)
#define USB_OTGFS_GAHBCFG_DMAEN_V  0x00000001U
#define USB_OTGFS_GAHBCFG_DMAEN_S  5
/** USB_OTGFS_GAHBCFG_NPTXFEMPLVL : R/W; bitpos: [7]; default: 0;
 *  Mode: Host and device
 *  Non-Periodic TxFIFO Empty Level (NPTxFEmpLvl)
 *  This bit is used only in Slave mode. In host mode and with Shared FIFO with device
 *  mode, this bit indicates when the Non-Periodic TxFIFO Empty Interrupt bit in the
 *  Core Interrupt register (GINTSTS.NPTxFEmp) is triggered.
 *  With dedicated FIFO in device mode, this bit indicates when IN endpoint Transmit
 *  FIFO empty interrupt (DIEPINTn.TxFEmp) is triggered.
 *  Host mode and with Shared FIFO with device mode:
 *  - 1'b0: GINTSTS.NPTxFEmp interrupt indicates that the Non-Periodic TxFIFO is half
 *  empty
 *  - 1'b1: GINTSTS.NPTxFEmp interrupt indicates that the Non-Periodic TxFIFO is
 *  completely empty
 *  Dedicated FIFO in device mode:
 *  - 1'b0: DIEPINTn.TxFEmp interrupt indicates that the IN Endpoint TxFIFO is half
 *  empty
 *  - 1'b1: DIEPINTn.TxFEmp interrupt indicates that the IN Endpoint TxFIFO is
 *  completely empty
 */
#define USB_OTGFS_GAHBCFG_NPTXFEMPLVL    (BIT(7))
#define USB_OTGFS_GAHBCFG_NPTXFEMPLVL_M  (USB_OTGFS_GAHBCFG_NPTXFEMPLVL_V << USB_OTGFS_GAHBCFG_NPTXFEMPLVL_S)
#define USB_OTGFS_GAHBCFG_NPTXFEMPLVL_V  0x00000001U
#define USB_OTGFS_GAHBCFG_NPTXFEMPLVL_S  7
/** USB_OTGFS_GAHBCFG_PTXFEMPLVL : R/W; bitpos: [8]; default: 0;
 *  Mode: Host only
 *  Periodic TxFIFO Empty Level (PTxFEmpLvl)
 *  Indicates when the Periodic TxFIFO Empty Interrupt bit in the Core Interrupt
 *  register (GINTSTS.PTxFEmp) is triggered. This bit is used only in Slave mode.
 *  - 1'b0: GINTSTS.PTxFEmp interrupt indicates that the Periodic TxFIFO is half empty
 *  - 1'b1: GINTSTS.PTxFEmp interrupt indicates that the Periodic TxFIFO is completely
 *  empty
 */
#define USB_OTGFS_GAHBCFG_PTXFEMPLVL    (BIT(8))
#define USB_OTGFS_GAHBCFG_PTXFEMPLVL_M  (USB_OTGFS_GAHBCFG_PTXFEMPLVL_V << USB_OTGFS_GAHBCFG_PTXFEMPLVL_S)
#define USB_OTGFS_GAHBCFG_PTXFEMPLVL_V  0x00000001U
#define USB_OTGFS_GAHBCFG_PTXFEMPLVL_S  8
/** USB_OTGFS_GAHBCFG_REMMEMSUPP : R/W; bitpos: [21]; default: 0;
 *  Mode: Host and Device
 *  Remote Memory Support (RemMemSupp)
 *  This bit is programmed to enable the functionality to wait for the system DMA Done
 *  Signal for the DMA Write Transfers.
 *  - GAHBCFG.RemMemSupp=1
 *  The int_dma_req output signal is asserted when the DMA starts write transfer to the
 *  external memory. When the core is done with the Transfers it asserts int_dma_done
 *  signal to flag the completion of DMA writes from the controller. The core then
 *  waits for sys_dma_done signal from the system to proceed further and complete the
 *  Data Transfer corresponding to a particular Channel/Endpoint.
 *  - GAHBCFG.RemMemSupp=0
 *  The int_dma_req and int_dma_done signals are not asserted and the core proceeds
 *  with the assertion of the XferComp interrupt as soon as the DMA write transfer is
 *  done at the Core Boundary and it does not wait for the sys_dma_done signal to
 *  complete the DATA transfers.
 */
#define USB_OTGFS_GAHBCFG_REMMEMSUPP    (BIT(21))
#define USB_OTGFS_GAHBCFG_REMMEMSUPP_M  (USB_OTGFS_GAHBCFG_REMMEMSUPP_V << USB_OTGFS_GAHBCFG_REMMEMSUPP_S)
#define USB_OTGFS_GAHBCFG_REMMEMSUPP_V  0x00000001U
#define USB_OTGFS_GAHBCFG_REMMEMSUPP_S  21
/** USB_OTGFS_GAHBCFG_NOTIALLDMAWRIT : R/W; bitpos: [22]; default: 0;
 *  Mode: Host and Device
 *  Notify All DMA Write Transactions (NotiAllDmaWrit)
 *  This bit is programmed to enable the System DMA Done functionality for all the DMA
 *  write Transactions corresponding to the Channel/Endpoint. This bit is valid only
 *  when GAHBCFG.RemMemSupp is set to 1.
 *  - GAHBCFG.NotiAllDmaWrit = 1
 *  The core asserts int_dma_req for all the DMA write transactions on the AHB
 *  interface along with int_dma_done, chep_last_transact and chep_number signal
 *  information. The core waits for sys_dma_done signal for all the DMA write
 *  transactions in order to complete the transfer of a particular Channel/Endpoint.
 *  - GAHBCFG.NotiAllDmaWrit = 0
 *  The core asserts int_dma_req signal only for the last transaction of DMA write
 *  transfer corresponding to a particular Channel/Endpoint. Similarly, the core waits
 *  for sys_dma_done signal only for that transaction of DMA write to complete the
 *  transfer of a particular Channel/Endpoint.
 */
#define USB_OTGFS_GAHBCFG_NOTIALLDMAWRIT    (BIT(22))
#define USB_OTGFS_GAHBCFG_NOTIALLDMAWRIT_M  (USB_OTGFS_GAHBCFG_NOTIALLDMAWRIT_V << USB_OTGFS_GAHBCFG_NOTIALLDMAWRIT_S)
#define USB_OTGFS_GAHBCFG_NOTIALLDMAWRIT_V  0x00000001U
#define USB_OTGFS_GAHBCFG_NOTIALLDMAWRIT_S  22
/** USB_OTGFS_GAHBCFG_AHBSINGLE : R/W; bitpos: [23]; default: 0;
 *  Mode: Host and Device
 *  AHB Single Support (AHBSingle)
 *  This bit when programmed supports Single transfers for the remaining data in a
 *  transfer when the core is operating in DMA mode.
 *  - 1'b0: The remaining data in the transfer is sent using INCR burst size.
 *  - 1'b1: The remaining data in the transfer is sent using Single burst size.
 *  Note: If this feature is enabled, the AHB RETRY and SPLIT transfers still have INCR
 *  burst type. Enable this feature when the AHB Slave connected to the core does not
 *  support INCR burst (and when Split, and Retry transactions are not being used in
 *  the bus).
 */
#define USB_OTGFS_GAHBCFG_AHBSINGLE    (BIT(23))
#define USB_OTGFS_GAHBCFG_AHBSINGLE_M  (USB_OTGFS_GAHBCFG_AHBSINGLE_V << USB_OTGFS_GAHBCFG_AHBSINGLE_S)
#define USB_OTGFS_GAHBCFG_AHBSINGLE_V  0x00000001U
#define USB_OTGFS_GAHBCFG_AHBSINGLE_S  23
/** USB_OTGFS_GAHBCFG_INVDESCENDIANESS : R/W; bitpos: [24]; default: 0;
 *  Mode: Host and Device
 *  Invert Descriptor Endianness (InvDescEndianess)
 *  - 1'b0: Descriptor Endianness is same as AHB Master Endianness.
 *  - 1'b1:
 *  -- If the AHB Master endianness is Big Endian, the Descriptor Endianness is Little
 *  Endian.
 *  -- If the AHB Master endianness is Little Endian, the Descriptor Endianness is Big
 *  Endian.
 */
#define USB_OTGFS_GAHBCFG_INVDESCENDIANESS    (BIT(24))
#define USB_OTGFS_GAHBCFG_INVDESCENDIANESS_M  (USB_OTGFS_GAHBCFG_INVDESCENDIANESS_V << USB_OTGFS_GAHBCFG_INVDESCENDIANESS_S)
#define USB_OTGFS_GAHBCFG_INVDESCENDIANESS_V  0x00000001U
#define USB_OTGFS_GAHBCFG_INVDESCENDIANESS_S  24

/** USB_OTGFS_GUSBCFG_REG register
 *  USB Configuration Register This register can be used to configure the core after
 *  power-on or a changing to Host mode or Device mode. It contains USB and USB-PHY
 *  related configuration parameters. The application must program this register before
 *  starting any transactions on either the AHB or the USB. Do not make changes to this
 *  register after the initial programming.
 */
#define USB_OTGFS_GUSBCFG_REG(i) (DR_REG_USB_BASE(i) + 0xc)
/** USB_OTGFS_GUSBCFG_TOUTCAL : R/W; bitpos: [2:0]; default: 0;
 *  Mode: Host and Device
 *  HS/FS Timeout Calibration (TOutCal)
 *
 *  The number of PHY clocks that the application programs in this field is added to
 *  the high-speed/full-speed interpacket timeout duration in the core to account for
 *  any additional delays introduced by the PHY. This can be required, because the
 *  delay introduced by the PHY in generating the linestate condition can vary from one
 *  PHY to another.
 *
 *  The USB standard timeout value for high-speed operation is 736 to 816 (inclusive)
 *  bit times. The USB standard timeout value for full-speed operation is 16 to 18
 *  (inclusive) bit times. The application must program this field based on the speed
 *  of enumeration. The number of bit times added per PHY clock are as follows:
 *
 *  High-speed operation:
 *  - One 30-MHz PHY clock = 16 bit times
 *  - One 60-MHz PHY clock = 8 bit times
 *  Full-speed operation:
 *  - One 30-MHz PHY clock = 0.4 bit times
 *  - One 60-MHz PHY clock = 0.2 bit times
 *  - One 48-MHz PHY clock = 0.25 bit times
 */
#define USB_OTGFS_GUSBCFG_TOUTCAL    0x00000007U
#define USB_OTGFS_GUSBCFG_TOUTCAL_M  (USB_OTGFS_GUSBCFG_TOUTCAL_V << USB_OTGFS_GUSBCFG_TOUTCAL_S)
#define USB_OTGFS_GUSBCFG_TOUTCAL_V  0x00000007U
#define USB_OTGFS_GUSBCFG_TOUTCAL_S  0
/** USB_OTGFS_GUSBCFG_PHYIF : R/W; bitpos: [3]; default: 0;
 *  Mode: Host and Device
 *  PHY Interface (PHYIf)
 *  The application uses this bit to configure the core to support a UTMI+ PHY with an
 *  8- or 16-bit interface. When a ULPI PHY is chosen, this must be Set to 8-bit mode.
 *  - 1'b0: 8 bits
 *  - 1'b1: 16 bits
 *  This bit is writable only If UTMI+ and ULPI were selected. Otherwise, this bit
 *  returns the value for the power-on interface selected during configuration.
 */
#define USB_OTGFS_GUSBCFG_PHYIF    (BIT(3))
#define USB_OTGFS_GUSBCFG_PHYIF_M  (USB_OTGFS_GUSBCFG_PHYIF_V << USB_OTGFS_GUSBCFG_PHYIF_S)
#define USB_OTGFS_GUSBCFG_PHYIF_V  0x00000001U
#define USB_OTGFS_GUSBCFG_PHYIF_S  3
/** USB_OTGFS_GUSBCFG_FSINTF : R/W; bitpos: [5]; default: 0;
 *  Mode: Host and Device
 *  Full-Speed Serial Interface Select (FSIntf)
 *
 *  The application uses this bit to select either a unidirectional or bidirectional
 *  USB 1.1 full-speed serial transceiver interface.
 *  - 1'b0: 6-pin unidirectional full-speed serial interface
 *  - 1'b1: 3-pin bidirectional full-speed serial interface
 *  If a USB 1.1 Full-Speed Serial Transceiver interface was not selected, this bit is
 *  always 0, with Write Only access. If a USB 1.1 FS interface was selected, Then the
 *  application can Set this bit to select between the 3- and 6-pin interfaces, and
 *  access is Read and Write.
 *
 *  Note: For supporting the new 4-pin bi-directional interface, you need to select
 *  6-pin unidirectional FS serial mode, and add an external control to convert it to a
 *  4-pin interface.
 */
#define USB_OTGFS_GUSBCFG_FSINTF    (BIT(5))
#define USB_OTGFS_GUSBCFG_FSINTF_M  (USB_OTGFS_GUSBCFG_FSINTF_V << USB_OTGFS_GUSBCFG_FSINTF_S)
#define USB_OTGFS_GUSBCFG_FSINTF_V  0x00000001U
#define USB_OTGFS_GUSBCFG_FSINTF_S  5
/** USB_OTGFS_GUSBCFG_PHYSEL : RO; bitpos: [6]; default: 1;
 *  PHYSel
 *
 *  Mode: Host and Device
 *
 *  USB 2.0 High-Speed PHY or USB 1.1 Full-Speed Serial Transceiver Select (PHYSel)
 *  The application uses this bit to select either a high-speed UTMI+ or ULPI PHY, or a
 *  full-speed transceiver.
 *  - 1'b0: USB 2.0 high-speed UTMI+ or ULPI PHY
 *  - 1'b1: USB 1.1 full-speed serial transceiver
 *  If a USB 1.1 Full-Speed Serial Transceiver interface was not selected in, this bit
 *  is always 0, with Write Only access.
 *  If a high-speed PHY interface was not selected in, this bit is always 1, with Write
 *  Only access.
 *  If both interface types were selected (parameters have non-zero values), the
 *  application uses this bit to select which interface is active, and access is Read
 *  and Write.
 */
#define USB_OTGFS_GUSBCFG_PHYSEL    (BIT(6))
#define USB_OTGFS_GUSBCFG_PHYSEL_M  (USB_OTGFS_GUSBCFG_PHYSEL_V << USB_OTGFS_GUSBCFG_PHYSEL_S)
#define USB_OTGFS_GUSBCFG_PHYSEL_V  0x00000001U
#define USB_OTGFS_GUSBCFG_PHYSEL_S  6
/** USB_OTGFS_GUSBCFG_SRPCAP : R/W; bitpos: [8]; default: 0;
 *  Mode: Host and Device
 *  SRP-Capable (SRPCap)
 *  The application uses this bit to control the controller's SRP capabilities. If the
 *  core operates as a non-SRP-capable B-device, it cannot request the connected
 *  A-device (host) to
 *  activate VBUS and start a session.
 *  - 1'b0: SRP capability is not enabled.
 *  - 1'b1: SRP capability is enabled.
 *  If SRP functionality is disabled by the software, the OTG signals on the PHY domain
 *  must be tied to the appropriate values.
 */
#define USB_OTGFS_GUSBCFG_SRPCAP    (BIT(8))
#define USB_OTGFS_GUSBCFG_SRPCAP_M  (USB_OTGFS_GUSBCFG_SRPCAP_V << USB_OTGFS_GUSBCFG_SRPCAP_S)
#define USB_OTGFS_GUSBCFG_SRPCAP_V  0x00000001U
#define USB_OTGFS_GUSBCFG_SRPCAP_S  8
/** USB_OTGFS_GUSBCFG_HNPCAP : R/W; bitpos: [9]; default: 0;
 *  Mode: Host and Device
 *  HNP-Capable (HNPCap)
 *  The application uses this bit to control the controller's HNP capabilities.
 *  - 1'b0: HNP capability is not enabled.
 *  - 1'b1: HNP capability is enabled.
 *  If HNP functionality is disabled by the software, the OTG signals on the PHY domain
 *  must be tied to the appropriate values.
 */
#define USB_OTGFS_GUSBCFG_HNPCAP    (BIT(9))
#define USB_OTGFS_GUSBCFG_HNPCAP_M  (USB_OTGFS_GUSBCFG_HNPCAP_V << USB_OTGFS_GUSBCFG_HNPCAP_S)
#define USB_OTGFS_GUSBCFG_HNPCAP_V  0x00000001U
#define USB_OTGFS_GUSBCFG_HNPCAP_S  9
/** USB_OTGFS_GUSBCFG_USBTRDTIM : R/W; bitpos: [13:10]; default: 5;
 *  Mode: Device only
 *  USB Turnaround Time (USBTrdTim)
 *  Sets the turnaround time in PHY clocks. Specifies the response time for a MAC
 *  request to the Packet FIFO Controller (PFC) to fetch data from the DFIFO (SPRAM).
 *  This must be programmed to
 *  - 4'h5: When the MAC interface is 16-bit UTMI+ .
 *  - 4'h9: When the MAC interface is 8-bit UTMI+ .
 *  Note: The previous values are calculated for the minimum AHB frequency of 30 MHz.
 *  USB turnaround time is critical for certification where long cables and 5-Hubs are
 *  used. If you need the AHB to run at less than 30 MHz, and if USB turnaround time is
 *  not critical, these bits can be programmed to a larger value.
 */
#define USB_OTGFS_GUSBCFG_USBTRDTIM    0x0000000FU
#define USB_OTGFS_GUSBCFG_USBTRDTIM_M  (USB_OTGFS_GUSBCFG_USBTRDTIM_V << USB_OTGFS_GUSBCFG_USBTRDTIM_S)
#define USB_OTGFS_GUSBCFG_USBTRDTIM_V  0x0000000FU
#define USB_OTGFS_GUSBCFG_USBTRDTIM_S  10
/** USB_OTGFS_GUSBCFG_TERMSELDLPULSE : RO; bitpos: [22]; default: 0;
 *  Mode: Device only
 *  TermSel DLine Pulsing Selection (TermSelDLPulse)
 *  This bit selects utmi_termselect to drive data line pulse during SRP.
 *  - 1'b0: Data line pulsing using utmi_txvalid (Default).
 *  - 1'b1: Data line pulsing using utmi_termsel.
 */
#define USB_OTGFS_GUSBCFG_TERMSELDLPULSE    (BIT(22))
#define USB_OTGFS_GUSBCFG_TERMSELDLPULSE_M  (USB_OTGFS_GUSBCFG_TERMSELDLPULSE_V << USB_OTGFS_GUSBCFG_TERMSELDLPULSE_S)
#define USB_OTGFS_GUSBCFG_TERMSELDLPULSE_V  0x00000001U
#define USB_OTGFS_GUSBCFG_TERMSELDLPULSE_S  22
/** USB_OTGFS_GUSBCFG_IC_USBCAP : RO; bitpos: [26]; default: 0;
 *  Mode: Host and Device
 *  IC_USB-Capable (IC_USBCap)
 *  The application uses this bit to control the core's IC_USB capabilities.
 *  - 1'b0: IC_USB PHY Interface is not selected.
 *  - 1'b1: IC_USB PHY Interface is selected.
 *  This bit is writable only if OTG_ENABLE_IC_USB=1 and OTG_FSPHY_INTERFACE!=0.
 *  The reset value depends on the configuration parameter OTG_SELECT_IC_USB when
 *  OTG_ENABLE_IC_USB = 1. In all other cases, this bit is set to 1'b0 and the bit is
 *  read only.
 */
#define USB_OTGFS_GUSBCFG_IC_USBCAP    (BIT(26))
#define USB_OTGFS_GUSBCFG_IC_USBCAP_M  (USB_OTGFS_GUSBCFG_IC_USBCAP_V << USB_OTGFS_GUSBCFG_IC_USBCAP_S)
#define USB_OTGFS_GUSBCFG_IC_USBCAP_V  0x00000001U
#define USB_OTGFS_GUSBCFG_IC_USBCAP_S  26
/** USB_OTGFS_GUSBCFG_TXENDDELAY : R/W; bitpos: [28]; default: 0;
 *  Mode: Device only
 *  Tx End Delay (TxEndDelay)
 *  Writing 1'b1 to this bit enables the controller to follow the TxEndDelay timings as
 *  per UTMI+ specification 1.05 section 4.1.5 for opmode signal during remote wakeup.
 *  - 1'b0 : Normal Mode.
 *  - 1'b1 : Tx End delay.
 */
#define USB_OTGFS_GUSBCFG_TXENDDELAY    (BIT(28))
#define USB_OTGFS_GUSBCFG_TXENDDELAY_M  (USB_OTGFS_GUSBCFG_TXENDDELAY_V << USB_OTGFS_GUSBCFG_TXENDDELAY_S)
#define USB_OTGFS_GUSBCFG_TXENDDELAY_V  0x00000001U
#define USB_OTGFS_GUSBCFG_TXENDDELAY_S  28
/** USB_OTGFS_GUSBCFG_FORCEHSTMODE : R/W; bitpos: [29]; default: 0;
 *  Mode: Host and device
 *  Force Host Mode (ForceHstMode)
 *  Writing a 1 to this bit forces the core to host mode irrespective of utmiotg_iddig
 *  input pin.
 *  - 1'b0 : Normal Mode.
 *  - 1'b1 : Force Host Mode.
 *  After setting the force bit, the application must wait at least 25 ms before the
 *  change to take effect. When the simulation is in scale down mode, waiting for 500
 *  micro sec is sufficient. This bit is valid only when OTG_MODE = 0, 1 or 2. In all
 *  other cases, this bit reads 0.
 */
#define USB_OTGFS_GUSBCFG_FORCEHSTMODE    (BIT(29))
#define USB_OTGFS_GUSBCFG_FORCEHSTMODE_M  (USB_OTGFS_GUSBCFG_FORCEHSTMODE_V << USB_OTGFS_GUSBCFG_FORCEHSTMODE_S)
#define USB_OTGFS_GUSBCFG_FORCEHSTMODE_V  0x00000001U
#define USB_OTGFS_GUSBCFG_FORCEHSTMODE_S  29
/** USB_OTGFS_GUSBCFG_FORCEDEVMODE : R/W; bitpos: [30]; default: 0;
 *  Mode:Host and device
 *  Force Device Mode (ForceDevMode)
 *  Writing a 1 to this bit forces the controller to device mode irrespective of
 *  utmiotg_iddig input pin.
 *  - 1'b0 : Normal Mode.
 *  - 1'b1 : Force Device Mode.
 *  After setting the force bit, the application must wait at least 25 ms before the
 *  change to take effect. When the simulation is in scale down mode, waiting for 500
 *  micro sec is sufficient. This bit is valid only when OTG_MODE = 0, 1 or 2. In all
 *  other cases, this bit reads 0.
 */
#define USB_OTGFS_GUSBCFG_FORCEDEVMODE    (BIT(30))
#define USB_OTGFS_GUSBCFG_FORCEDEVMODE_M  (USB_OTGFS_GUSBCFG_FORCEDEVMODE_V << USB_OTGFS_GUSBCFG_FORCEDEVMODE_S)
#define USB_OTGFS_GUSBCFG_FORCEDEVMODE_V  0x00000001U
#define USB_OTGFS_GUSBCFG_FORCEDEVMODE_S  30
/** USB_OTGFS_GUSBCFG_CORRUPTTXPKT : R/W; bitpos: [31]; default: 0;
 *  Mode: Host and device
 *  Corrupt Tx packet (CorruptTxPkt)
 *  This bit is for debug purposes only. Never Set this bit to 1. The application
 *  should always write 1'b0 to this bit.
 */
#define USB_OTGFS_GUSBCFG_CORRUPTTXPKT    (BIT(31))
#define USB_OTGFS_GUSBCFG_CORRUPTTXPKT_M  (USB_OTGFS_GUSBCFG_CORRUPTTXPKT_V << USB_OTGFS_GUSBCFG_CORRUPTTXPKT_S)
#define USB_OTGFS_GUSBCFG_CORRUPTTXPKT_V  0x00000001U
#define USB_OTGFS_GUSBCFG_CORRUPTTXPKT_S  31

/** USB_OTGFS_GRSTCTL_REG register
 *  Reset Register The application uses this register to reset various hardware
 *  features inside the controller.
 */
#define USB_OTGFS_GRSTCTL_REG(i) (DR_REG_USB_BASE(i) + 0x10)
/** USB_OTGFS_GRSTCTL_CSFTRST : R/W; bitpos: [0]; default: 0;
 *  Mode: Host and Device
 *  Core Soft Reset (CSftRst)
 *  Resets the hclk and phy_clock domains as follows:
 *  - Clears the interrupts and all the CSR registers except the following register
 *  bits:
 *  -- PCGCCTL.RstPdwnModule
 *  -- PCGCCTL.GateHclk
 *  -- PCGCCTL.PwrClmp
 *  -- PCGCCTL.StopPPhyLPwrClkSelclk
 *  -- GUSBCFG.ForceDevMode
 *  -- GUSBCFG.ForceHstMode
 *  -- GUSBCFG.PhyLPwrClkSel
 *  -- GUSBCFG.DDRSel
 *  -- GUSBCFG.PHYSel
 *  -- GUSBCFG.FSIntf
 *  -- GUSBCFG.ULPI_UTMI_Sel
 *  -- GUSBCFG.TxEndDelay
 *  -- GUSBCFG.TermSelDLPulse
 *  -- GUSBCFG.ULPIClkSusM
 *  -- GUSBCFG.ULPIAutoRes
 *  -- GUSBCFG.ULPIFsLs
 *  -- GGPIO
 *  -- GPWRDN
 *  -- GADPCTL
 *  -- HCFG.FSLSPclkSel
 *  -- DCFG.DevSpd
 *  -- DCTL.SftDiscon
 *  - All module state machines
 *  - All module state machines (except the AHB Slave Unit) are reset to the IDLE
 *  state, and all the transmit FIFOs and the receive FIFO are flushed.
 *  - Any transactions on the AHB Master are terminated as soon as possible, after
 *  gracefully completing the last data phase of an AHB transfer. Any transactions on
 *  the USB are terminated immediately.
 *  - When Hibernation or ADP feature is enabled, the PMU module is not reset by the
 *  Core Soft Reset.
 *  The application can write to this bit any time it wants to reset the core. This is
 *  a self-clearing bit and the core clears this bit after
 *  all the necessary logic is reset in the core, which can take several clocks,
 *  depending on the current state of the core. Once this bit is cleared software must
 *  wait at least 3 PHY clocks before doing any access to the PHY domain
 *  (synchronization delay). Software must also must check that bit 31 of this register
 *  is 1 (AHB Master is IDLE) before starting any operation.
 *
 *  Typically software reset is used during software development and also when you
 *  dynamically change the PHY selection bits in the USB configuration registers listed
 *  above. When you change the PHY, the corresponding clock for the PHY is selected and
 *  used in the PHY domain. Once a new clock is selected, the PHY domain has to be
 *  reset for proper operation.
 */
#define USB_OTGFS_GRSTCTL_CSFTRST    (BIT(0))
#define USB_OTGFS_GRSTCTL_CSFTRST_M  (USB_OTGFS_GRSTCTL_CSFTRST_V << USB_OTGFS_GRSTCTL_CSFTRST_S)
#define USB_OTGFS_GRSTCTL_CSFTRST_V  0x00000001U
#define USB_OTGFS_GRSTCTL_CSFTRST_S  0
/** USB_OTGFS_GRSTCTL_PIUFSSFTRST : R/W; bitpos: [1]; default: 0;
 *  Mode: Host and Device
 *  PIU FS Dedicated Controller Soft Reset (PIUFSSftRst)
 *
 *  Resets the PIU FS Dedicated Controller
 *  All module state machines in FS Dedicated Controller of PIU are reset to the IDLE
 *  state. Used to reset the FS Dedicated controller in PIU in case of any PHY Errors
 *  like Loss of activity or Babble Error resulting in the PHY remaining in RX state
 *  for more than one frame boundary.
 *  This is a self clearing bit and core clears this bit after all the necessary logic
 *  is reset in the core.
 */
#define USB_OTGFS_GRSTCTL_PIUFSSFTRST    (BIT(1))
#define USB_OTGFS_GRSTCTL_PIUFSSFTRST_M  (USB_OTGFS_GRSTCTL_PIUFSSFTRST_V << USB_OTGFS_GRSTCTL_PIUFSSFTRST_S)
#define USB_OTGFS_GRSTCTL_PIUFSSFTRST_V  0x00000001U
#define USB_OTGFS_GRSTCTL_PIUFSSFTRST_S  1
/** USB_OTGFS_GRSTCTL_FRMCNTRRST : R/W; bitpos: [2]; default: 0;
 *  Mode: Host only
 *  Host Frame Counter Reset (FrmCntrRst)
 *  The application writes this bit to reset the (micro)Frame number counter inside the
 *  core. When the (micro)Frame counter is reset, the subsequent SOF sent out by the
 *  core has a (micro)Frame number of 0.
 *  When application writes 1 to the bit, it might not be able to read back the value
 *  as it will get cleared by the core in a few clock cycles.
 */
#define USB_OTGFS_GRSTCTL_FRMCNTRRST    (BIT(2))
#define USB_OTGFS_GRSTCTL_FRMCNTRRST_M  (USB_OTGFS_GRSTCTL_FRMCNTRRST_V << USB_OTGFS_GRSTCTL_FRMCNTRRST_S)
#define USB_OTGFS_GRSTCTL_FRMCNTRRST_V  0x00000001U
#define USB_OTGFS_GRSTCTL_FRMCNTRRST_S  2
/** USB_OTGFS_GRSTCTL_RXFFLSH : R/W; bitpos: [4]; default: 0;
 *  Mode: Host and Device
 *  RxFIFO Flush (RxFFlsh)
 *  The application can flush the entire RxFIFO using this bit, but must first ensure
 *  that the core is not in the middle of a transaction.
 *  The application must only write to this bit after checking that the controller is
 *  neither reading from the RxFIFO nor writing to the RxFIFO.
 *
 *  The application must wait until the bit is cleared before performing any other
 *  operations. This bit requires eight clocks (slowest of PHY or AHB clock) to clear.
 */
#define USB_OTGFS_GRSTCTL_RXFFLSH    (BIT(4))
#define USB_OTGFS_GRSTCTL_RXFFLSH_M  (USB_OTGFS_GRSTCTL_RXFFLSH_V << USB_OTGFS_GRSTCTL_RXFFLSH_S)
#define USB_OTGFS_GRSTCTL_RXFFLSH_V  0x00000001U
#define USB_OTGFS_GRSTCTL_RXFFLSH_S  4
/** USB_OTGFS_GRSTCTL_TXFFLSH : R/W; bitpos: [5]; default: 0;
 *  Mode: Host and Device
 *  TxFIFO Flush (TxFFlsh)
 *  This bit selectively flushes a single or all transmit FIFOs, but cannot do so If
 *  the core is in the midst of a transaction.
 *  The application must write this bit only after checking that the core is neither
 *  writing to the TxFIFO nor reading from the TxFIFO.
 *  Verify using these registers:
 *  - ReadNAK Effective Interrupt ensures the core is not reading from the FIFO
 *  - WriteGRSTCTL.AHBIdle ensures the core is not writing anything to the FIFO.
 *  Flushing is normally recommended when FIFOs are reconfigured or when switching
 *  between Shared FIFO and Dedicated Transmit FIFO operation. FIFO flushing is also
 *  recommended during device endpoint disable. The application must wait until the
 *  core clears this bit before performing any operations. This bit takes eight clocks
 *  to clear, using the slower clock of phy_clk or hclk.
 */
#define USB_OTGFS_GRSTCTL_TXFFLSH    (BIT(5))
#define USB_OTGFS_GRSTCTL_TXFFLSH_M  (USB_OTGFS_GRSTCTL_TXFFLSH_V << USB_OTGFS_GRSTCTL_TXFFLSH_S)
#define USB_OTGFS_GRSTCTL_TXFFLSH_V  0x00000001U
#define USB_OTGFS_GRSTCTL_TXFFLSH_S  5
/** USB_OTGFS_GRSTCTL_TXFNUM : R/W; bitpos: [10:6]; default: 0;
 *  Mode: Host and Device
 *  TxFIFO Number (TxFNum)
 *  This is the FIFO number that must be flushed using the TxFIFO Flush bit. This field
 *  must not be changed until the core clears the TxFIFO Flush bit.
 *  - 5'h0:
 *  -- Non-periodic TxFIFO flush in Host mode
 *  -- Non-periodic TxFIFO flush in device mode when in shared FIFO operation
 *  -- Tx FIFO 0 flush in device mode when in dedicated FIFO mode
 *  - 5'h1:
 *  -- Periodic TxFIFO flush in Host mode
 *  -- Periodic TxFIFO 1 flush in Device mode when in shared FIFO operation
 *  -- TXFIFO 1 flush in device mode when in dedicated FIFO mode
 *  - 5'h2:
 *  -- Periodic TxFIFO 2 flush in Device mode when in shared FIFO operation
 *  -- TXFIFO 2 flush in device mode when in dedicated FIFO mode
 *  ...
 *  - 5'hF
 *  -- Periodic TxFIFO 15 flush in Device mode when in shared FIFO operation
 *  -- TXFIFO 15 flush in device mode when in dedicated FIFO mode
 *  - 5'h10: Flush all the transmit FIFOs in device or host mode
 */
#define USB_OTGFS_GRSTCTL_TXFNUM    0x0000001FU
#define USB_OTGFS_GRSTCTL_TXFNUM_M  (USB_OTGFS_GRSTCTL_TXFNUM_V << USB_OTGFS_GRSTCTL_TXFNUM_S)
#define USB_OTGFS_GRSTCTL_TXFNUM_V  0x0000001FU
#define USB_OTGFS_GRSTCTL_TXFNUM_S  6
/** USB_OTGFS_GRSTCTL_DMAREQ : RO; bitpos: [30]; default: 0;
 *  Mode: Host and Device
 *  DMA Request Signal (DMAReq)
 *  Indicates that the DMA request is in progress. Used for debug.
 */
#define USB_OTGFS_GRSTCTL_DMAREQ    (BIT(30))
#define USB_OTGFS_GRSTCTL_DMAREQ_M  (USB_OTGFS_GRSTCTL_DMAREQ_V << USB_OTGFS_GRSTCTL_DMAREQ_S)
#define USB_OTGFS_GRSTCTL_DMAREQ_V  0x00000001U
#define USB_OTGFS_GRSTCTL_DMAREQ_S  30
/** USB_OTGFS_GRSTCTL_AHBIDLE : RO; bitpos: [31]; default: 1;
 *  Mode: Host and Device
 *  AHB Master Idle (AHBIdle)
 *  Indicates that the AHB Master State Machine is in the IDLE condition.
 */
#define USB_OTGFS_GRSTCTL_AHBIDLE    (BIT(31))
#define USB_OTGFS_GRSTCTL_AHBIDLE_M  (USB_OTGFS_GRSTCTL_AHBIDLE_V << USB_OTGFS_GRSTCTL_AHBIDLE_S)
#define USB_OTGFS_GRSTCTL_AHBIDLE_V  0x00000001U
#define USB_OTGFS_GRSTCTL_AHBIDLE_S  31

/** USB_OTGFS_GINTSTS_REG register
 *  Interrupt Register  This register interrupts the application for system-level
 *  events in the current mode (Device mode or Host mode). Some of the bits in this
 *  register are valid only in Host mode, while others are valid in Device mode only.
 *  This register also indicates the current mode. To clear the interrupt status bits
 *  of type R_SS_WC, the application must write 1'b1 into the bit. The FIFO status
 *  interrupts are read only; once software reads from or writes to the FIFO while
 *  servicing these interrupts, FIFO interrupt conditions are cleared automatically.
 *  The application must clear the GINTSTS register at initialization before unmasking
 *  the interrupt bit to avoid any interrupts generated prior to initialization.  Note:
 *  Read the reset value of GINTSTS.CurMod only after the following conditions:  - If
 *  IDDIG_FILTER is disabled, read only after PHY clock is stable.  - If IDDIG_FILTER
 *  is enabled, read only after the filter timer expires.
 */
#define USB_OTGFS_GINTSTS_REG(i) (DR_REG_USB_BASE(i) + 0x14)
/** USB_OTGFS_GINTSTS_CURMOD : RO; bitpos: [0]; default: 0;
 *  Mode: Host and Device
 *  Current Mode of Operation (CurMod)
 *  Indicates the current mode.
 *  - 1'b0: Device mode
 *  - 1'b1: Host mode
 *
 *
 *  Note: The reset value of this register field can be read only after the PHY clock
 *  is stable, or if IDDIG_FILTER is enabled, wait for the filter timer to expire to
 *  read the correct reset value which ever event is later.
 */
#define USB_OTGFS_GINTSTS_CURMOD    (BIT(0))
#define USB_OTGFS_GINTSTS_CURMOD_M  (USB_OTGFS_GINTSTS_CURMOD_V << USB_OTGFS_GINTSTS_CURMOD_S)
#define USB_OTGFS_GINTSTS_CURMOD_V  0x00000001U
#define USB_OTGFS_GINTSTS_CURMOD_S  0
/** USB_OTGFS_GINTSTS_MODEMIS : R/W; bitpos: [1]; default: 0;
 *  Mode: Host and Device
 *  Mode Mismatch Interrupt (ModeMis)
 *  The core sets this bit when the application is trying to access:
 *  - A Host mode register, when the controller is operating in Device mode
 *  - A Device mode register, when the controller is operating in Host mode
 *  The register access is completed on the AHB with an OKAY response, but is ignored
 *  by the controller internally and does not affect the operation of the controller.
 *  This bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_GINTSTS_MODEMIS    (BIT(1))
#define USB_OTGFS_GINTSTS_MODEMIS_M  (USB_OTGFS_GINTSTS_MODEMIS_V << USB_OTGFS_GINTSTS_MODEMIS_S)
#define USB_OTGFS_GINTSTS_MODEMIS_V  0x00000001U
#define USB_OTGFS_GINTSTS_MODEMIS_S  1
/** USB_OTGFS_GINTSTS_OTGINT : RO; bitpos: [2]; default: 0;
 *  Mode: Host and Device
 *  OTG Interrupt (OTGInt)
 *  The controller sets this bit to indicate an OTG protocol event. The application
 *  must read the OTG Interrupt Status (GOTGINT) register to determine the exact event
 *  that caused this interrupt. The application must clear the appropriate status bit
 *  in the GOTGINT register to clear this bit.
 */
#define USB_OTGFS_GINTSTS_OTGINT    (BIT(2))
#define USB_OTGFS_GINTSTS_OTGINT_M  (USB_OTGFS_GINTSTS_OTGINT_V << USB_OTGFS_GINTSTS_OTGINT_S)
#define USB_OTGFS_GINTSTS_OTGINT_V  0x00000001U
#define USB_OTGFS_GINTSTS_OTGINT_S  2
/** USB_OTGFS_GINTSTS_SOF : R/W; bitpos: [3]; default: 0;
 *  Mode: Host and Device
 *  Start of (micro)Frame (Sof)
 *
 *  In Host mode, the core sets this bit to indicate that an SOF (FS), micro-SOF (HS),
 *  or Keep-Alive (LS) is transmitted on the USB. The application must write a 1 to
 *  this bit to clear the interrupt.
 *
 *  In Device mode, the controller sets this bit to indicate that an SOF token has been
 *  received on the USB. The application can read the Device Status register to get the
 *  current (micro)Frame number. This interrupt is seen only when the core is operating
 *  at either HS or FS. This bit can be set only by the core and the application must
 *  write 1 to clear it.
 *
 *  Note: This register may return 1'b1 if read immediately after power-on reset.
 *  If the register bit reads 1'b1 immediately after power-on reset, it does not
 *  indicate that an SOF has been sent (in case of host mode) or SOF has been received
 *  (in case of device mode).
 *  The read value of this interrupt is valid only after a valid connection between
 *  host and device is established. If the bit is set after power on reset the
 *  application can clear the bit.
 */
#define USB_OTGFS_GINTSTS_SOF    (BIT(3))
#define USB_OTGFS_GINTSTS_SOF_M  (USB_OTGFS_GINTSTS_SOF_V << USB_OTGFS_GINTSTS_SOF_S)
#define USB_OTGFS_GINTSTS_SOF_V  0x00000001U
#define USB_OTGFS_GINTSTS_SOF_S  3
/** USB_OTGFS_GINTSTS_RXFLVL : RO; bitpos: [4]; default: 0;
 *  Mode: Host and Device
 *  RxFIFO Non-Empty (RxFLvl)
 *
 *  Indicates that there is at least one packet pending to be read from the RxFIFO.
 */
#define USB_OTGFS_GINTSTS_RXFLVL    (BIT(4))
#define USB_OTGFS_GINTSTS_RXFLVL_M  (USB_OTGFS_GINTSTS_RXFLVL_V << USB_OTGFS_GINTSTS_RXFLVL_S)
#define USB_OTGFS_GINTSTS_RXFLVL_V  0x00000001U
#define USB_OTGFS_GINTSTS_RXFLVL_S  4
/** USB_OTGFS_GINTSTS_NPTXFEMP : RO; bitpos: [5]; default: 1;
 *  Mode: Host and Device
 *  Non-periodic TxFIFO Empty (NPTxFEmp)
 *  This interrupt is asserted when the Non-periodic TxFIFO is either half or
 *  completely empty, and there is space for at least one Entry to be written to the
 *  Non-periodic Transmit Request Queue. The half or completely empty status is
 *  determined by the Non-periodic TxFIFO Empty Level bit in the Core AHB Configuration
 *  register (GAHBCFG.NPTxFEmpLvl).
 *  In host mode, the application can use GINTSTS.NPTxFEmp with the OTG_EN_DED_TX_FIFO
 *  parameter set to either 1 or 0.
 *  In device mode, the application uses GINTSTS.NPTxFEmp when OTG_EN_DED_TX_FIFO=0.
 *  When OTG_EN_DED_TX_FIFO=1, the application uses DIEPINTn.TxFEmp.
 */
#define USB_OTGFS_GINTSTS_NPTXFEMP    (BIT(5))
#define USB_OTGFS_GINTSTS_NPTXFEMP_M  (USB_OTGFS_GINTSTS_NPTXFEMP_V << USB_OTGFS_GINTSTS_NPTXFEMP_S)
#define USB_OTGFS_GINTSTS_NPTXFEMP_V  0x00000001U
#define USB_OTGFS_GINTSTS_NPTXFEMP_S  5
/** USB_OTGFS_GINTSTS_GINNAKEFF : RO; bitpos: [6]; default: 0;
 *  Mode: Device only
 *  Global IN Non-periodic NAK Effective (GINNakEff)
 *  Indicates that the Set Global Non-periodic IN NAK bit in the Device Control
 *  register (DCTL.SGNPInNak) set by the application, has taken effect in the core.
 *  That is, the core has sampled the Global IN NAK bit Set by the application. This
 *  bit can be cleared by clearing the Clear Global Non-periodic IN NAK bit in the
 *  Device Control register (DCTL.CGNPInNak). This interrupt does not necessarily mean
 *  that a NAK handshake
 *  is sent out on the USB. The STALL bit takes precedence over the NAK bit.
 */
#define USB_OTGFS_GINTSTS_GINNAKEFF    (BIT(6))
#define USB_OTGFS_GINTSTS_GINNAKEFF_M  (USB_OTGFS_GINTSTS_GINNAKEFF_V << USB_OTGFS_GINTSTS_GINNAKEFF_S)
#define USB_OTGFS_GINTSTS_GINNAKEFF_V  0x00000001U
#define USB_OTGFS_GINTSTS_GINNAKEFF_S  6
/** USB_OTGFS_GINTSTS_GOUTNAKEFF : RO; bitpos: [7]; default: 0;
 *  Mode: Device only
 *  Global OUT NAK Effective (GOUTNakEff)
 *  Indicates that the Set Global OUT NAK bit in the Device Control register
 *  (DCTL.SGOUTNak), Set by the application, has taken effect in the core. This bit can
 *  be cleared by writing the Clear Global OUT NAK bit in the Device Control register
 *  (DCTL.CGOUTNak).
 */
#define USB_OTGFS_GINTSTS_GOUTNAKEFF    (BIT(7))
#define USB_OTGFS_GINTSTS_GOUTNAKEFF_M  (USB_OTGFS_GINTSTS_GOUTNAKEFF_V << USB_OTGFS_GINTSTS_GOUTNAKEFF_S)
#define USB_OTGFS_GINTSTS_GOUTNAKEFF_V  0x00000001U
#define USB_OTGFS_GINTSTS_GOUTNAKEFF_S  7
/** USB_OTGFS_GINTSTS_ERLYSUSP : R/W; bitpos: [10]; default: 0;
 *  Mode: Device only
 *  Early Suspend (ErlySusp)
 *  The controller sets this bit to indicate that an Idle state has been detected on
 *  the USB for 3 ms.
 */
#define USB_OTGFS_GINTSTS_ERLYSUSP    (BIT(10))
#define USB_OTGFS_GINTSTS_ERLYSUSP_M  (USB_OTGFS_GINTSTS_ERLYSUSP_V << USB_OTGFS_GINTSTS_ERLYSUSP_S)
#define USB_OTGFS_GINTSTS_ERLYSUSP_V  0x00000001U
#define USB_OTGFS_GINTSTS_ERLYSUSP_S  10
/** USB_OTGFS_GINTSTS_USBSUSP : R/W; bitpos: [11]; default: 0;
 *  Mode: Device only
 *  USB Suspend (USBSusp)
 *  The controller sets this bit to indicate that a suspend was detected on the USB.
 *  The controller enters the Suspended state when there is no activity on the
 *  linestate signal for an extended period of time.
 */
#define USB_OTGFS_GINTSTS_USBSUSP    (BIT(11))
#define USB_OTGFS_GINTSTS_USBSUSP_M  (USB_OTGFS_GINTSTS_USBSUSP_V << USB_OTGFS_GINTSTS_USBSUSP_S)
#define USB_OTGFS_GINTSTS_USBSUSP_V  0x00000001U
#define USB_OTGFS_GINTSTS_USBSUSP_S  11
/** USB_OTGFS_GINTSTS_USBRST : R/W; bitpos: [12]; default: 0;
 *  Mode: Device only
 *  USB Reset (USBRst)
 *  The controller sets this bit to indicate that a reset is detected on the USB.
 */
#define USB_OTGFS_GINTSTS_USBRST    (BIT(12))
#define USB_OTGFS_GINTSTS_USBRST_M  (USB_OTGFS_GINTSTS_USBRST_V << USB_OTGFS_GINTSTS_USBRST_S)
#define USB_OTGFS_GINTSTS_USBRST_V  0x00000001U
#define USB_OTGFS_GINTSTS_USBRST_S  12
/** USB_OTGFS_GINTSTS_ENUMDONE : R/W; bitpos: [13]; default: 0;
 *  Mode: Device only
 *  Enumeration Done (EnumDone)
 *  The core sets this bit to indicate that speed enumeration is complete. The
 *  application must read the Device Status (DSTS) register to obtain the enumerated
 *  speed.
 */
#define USB_OTGFS_GINTSTS_ENUMDONE    (BIT(13))
#define USB_OTGFS_GINTSTS_ENUMDONE_M  (USB_OTGFS_GINTSTS_ENUMDONE_V << USB_OTGFS_GINTSTS_ENUMDONE_S)
#define USB_OTGFS_GINTSTS_ENUMDONE_V  0x00000001U
#define USB_OTGFS_GINTSTS_ENUMDONE_S  13
/** USB_OTGFS_GINTSTS_ISOOUTDROP : R/W; bitpos: [14]; default: 0;
 *  Mode: Device only
 *  Isochronous OUT Packet Dropped Interrupt (ISOOutDrop)
 *  The controller sets this bit when it fails to write an isochronous OUT packet into
 *  the RxFIFO because the RxFIFO does not have enough space to accommodate a maximum
 *  packet size packet for the isochronous OUT endpoint.
 */
#define USB_OTGFS_GINTSTS_ISOOUTDROP    (BIT(14))
#define USB_OTGFS_GINTSTS_ISOOUTDROP_M  (USB_OTGFS_GINTSTS_ISOOUTDROP_V << USB_OTGFS_GINTSTS_ISOOUTDROP_S)
#define USB_OTGFS_GINTSTS_ISOOUTDROP_V  0x00000001U
#define USB_OTGFS_GINTSTS_ISOOUTDROP_S  14
/** USB_OTGFS_GINTSTS_EOPF : R/W; bitpos: [15]; default: 0;
 *  Mode: Device only
 *  End of Periodic Frame Interrupt (EOPF)
 *  Indicates that the period specified in the Periodic Frame Interval field of the
 *  Device Configuration register (DCFG.PerFrInt) has been reached in the current
 *  microframe.
 */
#define USB_OTGFS_GINTSTS_EOPF    (BIT(15))
#define USB_OTGFS_GINTSTS_EOPF_M  (USB_OTGFS_GINTSTS_EOPF_V << USB_OTGFS_GINTSTS_EOPF_S)
#define USB_OTGFS_GINTSTS_EOPF_V  0x00000001U
#define USB_OTGFS_GINTSTS_EOPF_S  15
/** USB_OTGFS_GINTSTS_EPMIS : R/W; bitpos: [17]; default: 0;
 *  Mode: Device only
 *  Endpoint Mismatch Interrupt (EPMis)
 *  Note: This interrupt is valid only in shared FIFO operation.
 *  Indicates that an IN token has been received for a non-periodic endpoint, but the
 *  data for another endpoint is present in the top of the Non-periodic Transmit FIFO
 *  and the IN endpoint mismatch count programmed by the application has expired.
 */
#define USB_OTGFS_GINTSTS_EPMIS    (BIT(17))
#define USB_OTGFS_GINTSTS_EPMIS_M  (USB_OTGFS_GINTSTS_EPMIS_V << USB_OTGFS_GINTSTS_EPMIS_S)
#define USB_OTGFS_GINTSTS_EPMIS_V  0x00000001U
#define USB_OTGFS_GINTSTS_EPMIS_S  17
/** USB_OTGFS_GINTSTS_IEPINT : RO; bitpos: [18]; default: 0;
 *  Mode: Device only
 *  IN Endpoints Interrupt (IEPInt)
 *  The core sets this bit to indicate that an interrupt is pending on one of the IN
 *  endpoints of the core (in Device mode). The application must read the Device All
 *  Endpoints Interrupt (DAINT) register to determine the exact number of the IN
 *  endpoint on Device IN Endpoint-n Interrupt (DIEPINTn) register to determine the
 *  exact cause of the interrupt. The application must clear the appropriate status bit
 *  in the corresponding DIEPINTn register to
 *  clear this bit.
 */
#define USB_OTGFS_GINTSTS_IEPINT    (BIT(18))
#define USB_OTGFS_GINTSTS_IEPINT_M  (USB_OTGFS_GINTSTS_IEPINT_V << USB_OTGFS_GINTSTS_IEPINT_S)
#define USB_OTGFS_GINTSTS_IEPINT_V  0x00000001U
#define USB_OTGFS_GINTSTS_IEPINT_S  18
/** USB_OTGFS_GINTSTS_OEPINT : RO; bitpos: [19]; default: 0;
 *  Mode: Device only
 *  OUT Endpoints Interrupt (OEPInt)
 *  The controller sets this bit to indicate that an interrupt is pending on one of the
 *  OUT endpoints of the core (in Device mode). The application must read the Device
 *  All Endpoints Interrupt (DAINT) register to determine the exact number of the OUT
 *  endpoint on which the interrupt occurred, and then read the corresponding Device
 *  OUT Endpoint-n Interrupt (DOEPINTn) register to determine the exact cause of the
 *  interrupt. The application must
 *  clear the appropriate status bit in the corresponding DOEPINTn register to clear
 *  this bit.
 */
#define USB_OTGFS_GINTSTS_OEPINT    (BIT(19))
#define USB_OTGFS_GINTSTS_OEPINT_M  (USB_OTGFS_GINTSTS_OEPINT_V << USB_OTGFS_GINTSTS_OEPINT_S)
#define USB_OTGFS_GINTSTS_OEPINT_V  0x00000001U
#define USB_OTGFS_GINTSTS_OEPINT_S  19
/** USB_OTGFS_GINTSTS_INCOMPISOIN : R/W; bitpos: [20]; default: 0;
 *  Mode: Device only
 *  Incomplete Isochronous IN Transfer (incompISOIN)
 *  The core sets this interrupt to indicate that there is at least one isochronous IN
 *  endpoint on which the transfer is not completed in the current microframe. This
 *  interrupt is asserted along with the End of Periodic Frame Interrupt (EOPF) bit in
 *  this register.
 *  Note: This interrupt is not asserted in Scatter/Gather DMA mode.
 */
#define USB_OTGFS_GINTSTS_INCOMPISOIN    (BIT(20))
#define USB_OTGFS_GINTSTS_INCOMPISOIN_M  (USB_OTGFS_GINTSTS_INCOMPISOIN_V << USB_OTGFS_GINTSTS_INCOMPISOIN_S)
#define USB_OTGFS_GINTSTS_INCOMPISOIN_V  0x00000001U
#define USB_OTGFS_GINTSTS_INCOMPISOIN_S  20
/** USB_OTGFS_GINTSTS_INCOMPLP : R/W; bitpos: [21]; default: 0;
 *  Incomplete Periodic Transfer (incomplP)
 *  Mode: Host only
 *  In Host mode, the core sets this interrupt bit when there are incomplete periodic
 *  transactions still pending which are scheduled for the current microframe.
 *  Incomplete Isochronous OUT Transfer (incompISOOUT)
 *  Mode: Device only
 *  The Device mode, the core sets this interrupt to indicate that there is at least
 *  one isochronous OUT endpoint on which the transfer is not completed in the current
 *  microframe. This interrupt is asserted along with the End of Periodic Frame
 *  Interrupt (EOPF) bit in this register.
 */
#define USB_OTGFS_GINTSTS_INCOMPLP    (BIT(21))
#define USB_OTGFS_GINTSTS_INCOMPLP_M  (USB_OTGFS_GINTSTS_INCOMPLP_V << USB_OTGFS_GINTSTS_INCOMPLP_S)
#define USB_OTGFS_GINTSTS_INCOMPLP_V  0x00000001U
#define USB_OTGFS_GINTSTS_INCOMPLP_S  21
/** USB_OTGFS_GINTSTS_FETSUSP : R/W; bitpos: [22]; default: 0;
 *  Mode: Device only
 *  Data Fetch Suspended (FetSusp)
 *  This interrupt is valid only in DMA mode. This interrupt indicates that the core
 *  has stopped fetching data. For IN endpoints due to the unavailability of TxFIFO
 *  space or Request Queue space. This interrupt is used by the application for an
 *  endpoint mismatch algorithm.
 *
 *  For example, after detecting an endpoint mismatch, the application:
 *  - Sets a Global non-periodic IN NAK handshake
 *  - Disables IN endpoints
 *  - Flushes the FIFO
 *  - Determines the token sequence from the IN Token Sequence Learning Queue
 *  - Re-enables the endpoints
 *  - Clears the Global non-periodic IN NAK handshake
 *  If the Global non-periodic IN NAK is cleared, the core has not yet fetched data for
 *  the IN endpoint, and the IN token is received. The core generates an 'IN token
 *  received when FIFO empty' interrupt. The DWC_otg then sends the host a NAK
 *  response. To avoid this scenario, the application can check the GINTSTS.FetSusp
 *  interrupt, which ensures that the FIFO is full before clearing a Global NAK
 *  handshake.
 *
 *  Alternatively, the application can mask the IN token received when FIFO empty
 *  interrupt when clearing a Global IN NAK handshake.
 */
#define USB_OTGFS_GINTSTS_FETSUSP    (BIT(22))
#define USB_OTGFS_GINTSTS_FETSUSP_M  (USB_OTGFS_GINTSTS_FETSUSP_V << USB_OTGFS_GINTSTS_FETSUSP_S)
#define USB_OTGFS_GINTSTS_FETSUSP_V  0x00000001U
#define USB_OTGFS_GINTSTS_FETSUSP_S  22
/** USB_OTGFS_GINTSTS_RESETDET : R/W; bitpos: [23]; default: 0;
 *  Mode: Device only
 *  Reset detected Interrupt (ResetDet)
 *  In Device mode, this interrupt is asserted when a reset is detected on the USB in
 *  partial power-down mode when the device is in Suspend.
 *
 *  In Host mode, this interrupt is not asserted.
 */
#define USB_OTGFS_GINTSTS_RESETDET    (BIT(23))
#define USB_OTGFS_GINTSTS_RESETDET_M  (USB_OTGFS_GINTSTS_RESETDET_V << USB_OTGFS_GINTSTS_RESETDET_S)
#define USB_OTGFS_GINTSTS_RESETDET_V  0x00000001U
#define USB_OTGFS_GINTSTS_RESETDET_S  23
/** USB_OTGFS_GINTSTS_PRTINT : RO; bitpos: [24]; default: 0;
 *  Mode: Host only
 *  Host Port Interrupt (PrtInt)
 *  The core sets this bit to indicate a change in port status of one of the controller
 *  ports in Host mode. The application must read the Host Port Control and Status
 *  (HPRT) register to determine the exact event that caused this interrupt. The
 *  application must clear the appropriate status bit in the Host Port
 *  Control and Status register to clear this bit.
 */
#define USB_OTGFS_GINTSTS_PRTINT    (BIT(24))
#define USB_OTGFS_GINTSTS_PRTINT_M  (USB_OTGFS_GINTSTS_PRTINT_V << USB_OTGFS_GINTSTS_PRTINT_S)
#define USB_OTGFS_GINTSTS_PRTINT_V  0x00000001U
#define USB_OTGFS_GINTSTS_PRTINT_S  24
/** USB_OTGFS_GINTSTS_HCHINT : RO; bitpos: [25]; default: 0;
 *  Mode: Host only
 *  Host Channels Interrupt (HChInt)
 *  The core sets this bit to indicate that an interrupt is pending on one of the
 *  channels of the core (in Host mode). The application must read the Host All
 *  Channels Interrupt (HAINT) register to determine the exact number of the channel on
 *  which the interrupt occurred, and Then read the corresponding Host
 *  Channel-n Interrupt (HCINTn) register to determine the exact cause of the
 *  interrupt. The application must clear the appropriate status bit in the HCINTn
 *  register to clear this bit.
 */
#define USB_OTGFS_GINTSTS_HCHINT    (BIT(25))
#define USB_OTGFS_GINTSTS_HCHINT_M  (USB_OTGFS_GINTSTS_HCHINT_V << USB_OTGFS_GINTSTS_HCHINT_S)
#define USB_OTGFS_GINTSTS_HCHINT_V  0x00000001U
#define USB_OTGFS_GINTSTS_HCHINT_S  25
/** USB_OTGFS_GINTSTS_PTXFEMP : RO; bitpos: [26]; default: 1;
 *  Mode: Host only
 *  Periodic TxFIFO Empty (PTxFEmp)
 *  This interrupt is asserted when the Periodic Transmit FIFO is either half or
 *  completely empty and there is space for at least one entry to be written in the
 *  Periodic Request Queue. The half or completely empty status is determined by the
 *  Periodic TxFIFO Empty Level bit in the Core AHB Configuration register
 *  (GAHBCFG.PTxFEmpLvl).
 */
#define USB_OTGFS_GINTSTS_PTXFEMP    (BIT(26))
#define USB_OTGFS_GINTSTS_PTXFEMP_M  (USB_OTGFS_GINTSTS_PTXFEMP_V << USB_OTGFS_GINTSTS_PTXFEMP_S)
#define USB_OTGFS_GINTSTS_PTXFEMP_V  0x00000001U
#define USB_OTGFS_GINTSTS_PTXFEMP_S  26
/** USB_OTGFS_GINTSTS_CONIDSTSCHNG : R/W; bitpos: [28]; default: 0;
 *  Mode: Host and Device
 *  Connector ID Status Change (ConIDStsChng)
 *  The core sets this bit when there is a change in connector ID status.
 */
#define USB_OTGFS_GINTSTS_CONIDSTSCHNG    (BIT(28))
#define USB_OTGFS_GINTSTS_CONIDSTSCHNG_M  (USB_OTGFS_GINTSTS_CONIDSTSCHNG_V << USB_OTGFS_GINTSTS_CONIDSTSCHNG_S)
#define USB_OTGFS_GINTSTS_CONIDSTSCHNG_V  0x00000001U
#define USB_OTGFS_GINTSTS_CONIDSTSCHNG_S  28
/** USB_OTGFS_GINTSTS_DISCONNINT : R/W; bitpos: [29]; default: 0;
 *  Mode: Host only
 *  Disconnect Detected Interrupt (DisconnInt)
 *  Asserted when a device disconnect is detected.
 */
#define USB_OTGFS_GINTSTS_DISCONNINT    (BIT(29))
#define USB_OTGFS_GINTSTS_DISCONNINT_M  (USB_OTGFS_GINTSTS_DISCONNINT_V << USB_OTGFS_GINTSTS_DISCONNINT_S)
#define USB_OTGFS_GINTSTS_DISCONNINT_V  0x00000001U
#define USB_OTGFS_GINTSTS_DISCONNINT_S  29
/** USB_OTGFS_GINTSTS_SESSREQINT : R/W; bitpos: [30]; default: 0;
 *  Mode: Host and Device
 *  Session Request/New Session Detected Interrupt (SessReqInt)
 *  In Host mode, this interrupt is asserted when a session request is detected from
 *  the device. In Host mode, this interrupt is asserted when a session request is
 *  detected from the device.
 *  In Device mode, this interrupt is asserted when the utmisrp_bvalid signal goes high.
 *  For more information on how to use this interrupt, see 'Partial Power-Down and
 *  Clock Gating Programming Model' in the Programming Guide.
 */
#define USB_OTGFS_GINTSTS_SESSREQINT    (BIT(30))
#define USB_OTGFS_GINTSTS_SESSREQINT_M  (USB_OTGFS_GINTSTS_SESSREQINT_V << USB_OTGFS_GINTSTS_SESSREQINT_S)
#define USB_OTGFS_GINTSTS_SESSREQINT_V  0x00000001U
#define USB_OTGFS_GINTSTS_SESSREQINT_S  30
/** USB_OTGFS_GINTSTS_WKUPINT : R/W; bitpos: [31]; default: 0;
 *  Mode: Host and Device
 *  Resume/Remote Wakeup Detected Interrupt (WkUpInt)
 *  Wakeup Interrupt during Suspend(L2) or LPM(L1) state.
 *  - During Suspend(L2):
 *  -- Device Mode: This interrupt is asserted only when Host Initiated Resume is
 *  detected on USB.
 *  -- Host Mode: This interrupt is asserted only when Device Initiated Remote Wakeup
 *  is detected on USB.
 *  For more information, see 'Partial Power-Down and Clock Gating Programming Model'
 *  in the Programming Guide.
 *  - During LPM(L1):
 *  -- Device Mode: This interrupt is asserted for either Host Initiated Resume or
 *  Device Initiated Remote Wakeup on USB.
 *  -- Host Mode: This interrupt is asserted for either Host Initiated Resume or Device
 *  Initiated Remote Wakeup on USB.
 *  For more information, see 'LPM Entry and Exit Programming Model' in the Programming
 *  Guide.
 */
#define USB_OTGFS_GINTSTS_WKUPINT    (BIT(31))
#define USB_OTGFS_GINTSTS_WKUPINT_M  (USB_OTGFS_GINTSTS_WKUPINT_V << USB_OTGFS_GINTSTS_WKUPINT_S)
#define USB_OTGFS_GINTSTS_WKUPINT_V  0x00000001U
#define USB_OTGFS_GINTSTS_WKUPINT_S  31

/** USB_OTGFS_GINTMSK_REG register
 *  Interrupt Mask Register This register works with the Interrupt Register (GINTSTS)
 *  to interrupt the application. When an interrupt bit is masked, the interrupt
 *  associated with that bit is not generated. However, the GINTSTS register bit
 *  corresponding to that interrupt is still set.  Note: The fields of this register
 *  change depending on host or device mode.
 */
#define USB_OTGFS_GINTMSK_REG(i) (DR_REG_USB_BASE(i) + 0x18)
/** USB_OTGFS_GINTMSK_MODEMISMSK : R/W; bitpos: [1]; default: 0;
 *  Mode: Host and Device
 *  Mode Mismatch Interrupt Mask (ModeMisMsk)
 */
#define USB_OTGFS_GINTMSK_MODEMISMSK    (BIT(1))
#define USB_OTGFS_GINTMSK_MODEMISMSK_M  (USB_OTGFS_GINTMSK_MODEMISMSK_V << USB_OTGFS_GINTMSK_MODEMISMSK_S)
#define USB_OTGFS_GINTMSK_MODEMISMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_MODEMISMSK_S  1
/** USB_OTGFS_GINTMSK_OTGINTMSK : R/W; bitpos: [2]; default: 0;
 *  Mode: Host and Device
 *  OTG Interrupt Mask (OTGIntMsk)
 */
#define USB_OTGFS_GINTMSK_OTGINTMSK    (BIT(2))
#define USB_OTGFS_GINTMSK_OTGINTMSK_M  (USB_OTGFS_GINTMSK_OTGINTMSK_V << USB_OTGFS_GINTMSK_OTGINTMSK_S)
#define USB_OTGFS_GINTMSK_OTGINTMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_OTGINTMSK_S  2
/** USB_OTGFS_GINTMSK_SOFMSK : R/W; bitpos: [3]; default: 0;
 *  Mode: Host and Device
 *  Start of (micro)Frame Mask (SofMsk)
 */
#define USB_OTGFS_GINTMSK_SOFMSK    (BIT(3))
#define USB_OTGFS_GINTMSK_SOFMSK_M  (USB_OTGFS_GINTMSK_SOFMSK_V << USB_OTGFS_GINTMSK_SOFMSK_S)
#define USB_OTGFS_GINTMSK_SOFMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_SOFMSK_S  3
/** USB_OTGFS_GINTMSK_RXFLVLMSK : R/W; bitpos: [4]; default: 0;
 *  Mode: Host and Device
 *  Receive FIFO Non-Empty Mask (RxFLvlMsk)
 */
#define USB_OTGFS_GINTMSK_RXFLVLMSK    (BIT(4))
#define USB_OTGFS_GINTMSK_RXFLVLMSK_M  (USB_OTGFS_GINTMSK_RXFLVLMSK_V << USB_OTGFS_GINTMSK_RXFLVLMSK_S)
#define USB_OTGFS_GINTMSK_RXFLVLMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_RXFLVLMSK_S  4
/** USB_OTGFS_GINTMSK_NPTXFEMPMSK : R/W; bitpos: [5]; default: 0;
 *  Mode: Host and Device
 *  Non-periodic TxFIFO Empty Mask (NPTxFEmpMsk)
 */
#define USB_OTGFS_GINTMSK_NPTXFEMPMSK    (BIT(5))
#define USB_OTGFS_GINTMSK_NPTXFEMPMSK_M  (USB_OTGFS_GINTMSK_NPTXFEMPMSK_V << USB_OTGFS_GINTMSK_NPTXFEMPMSK_S)
#define USB_OTGFS_GINTMSK_NPTXFEMPMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_NPTXFEMPMSK_S  5
/** USB_OTGFS_GINTMSK_GINNAKEFFMSK : R/W; bitpos: [6]; default: 0;
 *  Mode: Device only,
 *  Global Non-periodic IN NAK Effective Mask (GINNakEffMsk)
 */
#define USB_OTGFS_GINTMSK_GINNAKEFFMSK    (BIT(6))
#define USB_OTGFS_GINTMSK_GINNAKEFFMSK_M  (USB_OTGFS_GINTMSK_GINNAKEFFMSK_V << USB_OTGFS_GINTMSK_GINNAKEFFMSK_S)
#define USB_OTGFS_GINTMSK_GINNAKEFFMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_GINNAKEFFMSK_S  6
/** USB_OTGFS_GINTMSK_GOUTNAKEFFMSK : R/W; bitpos: [7]; default: 0;
 *  Mode: Device only
 *  Global OUT NAK Effective Mask (GOUTNakEffMsk)
 */
#define USB_OTGFS_GINTMSK_GOUTNAKEFFMSK    (BIT(7))
#define USB_OTGFS_GINTMSK_GOUTNAKEFFMSK_M  (USB_OTGFS_GINTMSK_GOUTNAKEFFMSK_V << USB_OTGFS_GINTMSK_GOUTNAKEFFMSK_S)
#define USB_OTGFS_GINTMSK_GOUTNAKEFFMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_GOUTNAKEFFMSK_S  7
/** USB_OTGFS_GINTMSK_ERLYSUSPMSK : R/W; bitpos: [10]; default: 0;
 *  Mode: Device only
 *  Early Suspend Mask (ErlySuspMsk)
 */
#define USB_OTGFS_GINTMSK_ERLYSUSPMSK    (BIT(10))
#define USB_OTGFS_GINTMSK_ERLYSUSPMSK_M  (USB_OTGFS_GINTMSK_ERLYSUSPMSK_V << USB_OTGFS_GINTMSK_ERLYSUSPMSK_S)
#define USB_OTGFS_GINTMSK_ERLYSUSPMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_ERLYSUSPMSK_S  10
/** USB_OTGFS_GINTMSK_USBSUSPMSK : R/W; bitpos: [11]; default: 0;
 *  Mode: Device only
 *  USB Suspend Mask (USBSuspMsk)
 */
#define USB_OTGFS_GINTMSK_USBSUSPMSK    (BIT(11))
#define USB_OTGFS_GINTMSK_USBSUSPMSK_M  (USB_OTGFS_GINTMSK_USBSUSPMSK_V << USB_OTGFS_GINTMSK_USBSUSPMSK_S)
#define USB_OTGFS_GINTMSK_USBSUSPMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_USBSUSPMSK_S  11
/** USB_OTGFS_GINTMSK_USBRSTMSK : R/W; bitpos: [12]; default: 0;
 *  Mode: Device only
 *  USB Reset Mask (USBRstMsk)
 */
#define USB_OTGFS_GINTMSK_USBRSTMSK    (BIT(12))
#define USB_OTGFS_GINTMSK_USBRSTMSK_M  (USB_OTGFS_GINTMSK_USBRSTMSK_V << USB_OTGFS_GINTMSK_USBRSTMSK_S)
#define USB_OTGFS_GINTMSK_USBRSTMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_USBRSTMSK_S  12
/** USB_OTGFS_GINTMSK_ENUMDONEMSK : R/W; bitpos: [13]; default: 0;
 *  Mode: Device only
 *  Enumeration Done Mask (EnumDoneMsk)
 */
#define USB_OTGFS_GINTMSK_ENUMDONEMSK    (BIT(13))
#define USB_OTGFS_GINTMSK_ENUMDONEMSK_M  (USB_OTGFS_GINTMSK_ENUMDONEMSK_V << USB_OTGFS_GINTMSK_ENUMDONEMSK_S)
#define USB_OTGFS_GINTMSK_ENUMDONEMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_ENUMDONEMSK_S  13
/** USB_OTGFS_GINTMSK_ISOOUTDROPMSK : R/W; bitpos: [14]; default: 0;
 *  Mode: Device only
 *  Isochronous OUT Packet Dropped Interrupt Mask (ISOOutDropMsk)
 */
#define USB_OTGFS_GINTMSK_ISOOUTDROPMSK    (BIT(14))
#define USB_OTGFS_GINTMSK_ISOOUTDROPMSK_M  (USB_OTGFS_GINTMSK_ISOOUTDROPMSK_V << USB_OTGFS_GINTMSK_ISOOUTDROPMSK_S)
#define USB_OTGFS_GINTMSK_ISOOUTDROPMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_ISOOUTDROPMSK_S  14
/** USB_OTGFS_GINTMSK_EOPFMSK : R/W; bitpos: [15]; default: 0;
 *  Mode: Device only
 *  End of Periodic Frame Interrupt Mask (EOPFMsk)
 */
#define USB_OTGFS_GINTMSK_EOPFMSK    (BIT(15))
#define USB_OTGFS_GINTMSK_EOPFMSK_M  (USB_OTGFS_GINTMSK_EOPFMSK_V << USB_OTGFS_GINTMSK_EOPFMSK_S)
#define USB_OTGFS_GINTMSK_EOPFMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_EOPFMSK_S  15
/** USB_OTGFS_GINTMSK_EPMISMSK : R/W; bitpos: [17]; default: 0;
 *  Mode: Device only
 *  Endpoint Mismatch Interrupt Mask (EPMisMsk)
 */
#define USB_OTGFS_GINTMSK_EPMISMSK    (BIT(17))
#define USB_OTGFS_GINTMSK_EPMISMSK_M  (USB_OTGFS_GINTMSK_EPMISMSK_V << USB_OTGFS_GINTMSK_EPMISMSK_S)
#define USB_OTGFS_GINTMSK_EPMISMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_EPMISMSK_S  17
/** USB_OTGFS_GINTMSK_IEPINTMSK : R/W; bitpos: [18]; default: 0;
 *  Mode: Device only
 *  IN Endpoints Interrupt Mask (IEPIntMsk)
 */
#define USB_OTGFS_GINTMSK_IEPINTMSK    (BIT(18))
#define USB_OTGFS_GINTMSK_IEPINTMSK_M  (USB_OTGFS_GINTMSK_IEPINTMSK_V << USB_OTGFS_GINTMSK_IEPINTMSK_S)
#define USB_OTGFS_GINTMSK_IEPINTMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_IEPINTMSK_S  18
/** USB_OTGFS_GINTMSK_OEPINTMSK : R/W; bitpos: [19]; default: 0;
 *  Mode: Device only
 *  OUT Endpoints Interrupt Mask (OEPIntMsk)
 */
#define USB_OTGFS_GINTMSK_OEPINTMSK    (BIT(19))
#define USB_OTGFS_GINTMSK_OEPINTMSK_M  (USB_OTGFS_GINTMSK_OEPINTMSK_V << USB_OTGFS_GINTMSK_OEPINTMSK_S)
#define USB_OTGFS_GINTMSK_OEPINTMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_OEPINTMSK_S  19
/** USB_OTGFS_GINTMSK_INCOMPLPMSK : R/W; bitpos: [21]; default: 0;
 *  Incomplete Periodic Transfer Mask (incomplPMsk)
 *  Mode: Host only
 *  Incomplete Isochronous OUT Transfer Interrupt Mask (incompISOOUTMsk)
 *  Mode: Device only
 */
#define USB_OTGFS_GINTMSK_INCOMPLPMSK    (BIT(21))
#define USB_OTGFS_GINTMSK_INCOMPLPMSK_M  (USB_OTGFS_GINTMSK_INCOMPLPMSK_V << USB_OTGFS_GINTMSK_INCOMPLPMSK_S)
#define USB_OTGFS_GINTMSK_INCOMPLPMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_INCOMPLPMSK_S  21
/** USB_OTGFS_GINTMSK_FETSUSPMSK : R/W; bitpos: [22]; default: 0;
 *  Mode: Device only
 *  Data Fetch Suspended Mask (FetSuspMsk)
 */
#define USB_OTGFS_GINTMSK_FETSUSPMSK    (BIT(22))
#define USB_OTGFS_GINTMSK_FETSUSPMSK_M  (USB_OTGFS_GINTMSK_FETSUSPMSK_V << USB_OTGFS_GINTMSK_FETSUSPMSK_S)
#define USB_OTGFS_GINTMSK_FETSUSPMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_FETSUSPMSK_S  22
/** USB_OTGFS_GINTMSK_RESETDETMSK : R/W; bitpos: [23]; default: 0;
 *  Mode: Device only
 *  Reset detected Interrupt Mask (ResetDetMsk)
 */
#define USB_OTGFS_GINTMSK_RESETDETMSK    (BIT(23))
#define USB_OTGFS_GINTMSK_RESETDETMSK_M  (USB_OTGFS_GINTMSK_RESETDETMSK_V << USB_OTGFS_GINTMSK_RESETDETMSK_S)
#define USB_OTGFS_GINTMSK_RESETDETMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_RESETDETMSK_S  23
/** USB_OTGFS_GINTMSK_PRTINTMSK : R/W; bitpos: [24]; default: 0;
 *  Mode: Host only
 *  Host Port Interrupt Mask (PrtIntMsk)
 */
#define USB_OTGFS_GINTMSK_PRTINTMSK    (BIT(24))
#define USB_OTGFS_GINTMSK_PRTINTMSK_M  (USB_OTGFS_GINTMSK_PRTINTMSK_V << USB_OTGFS_GINTMSK_PRTINTMSK_S)
#define USB_OTGFS_GINTMSK_PRTINTMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_PRTINTMSK_S  24
/** USB_OTGFS_GINTMSK_HCHINTMSK : R/W; bitpos: [25]; default: 0;
 *  Mode: Host only
 *  Host Channels Interrupt Mask (HChIntMsk)
 */
#define USB_OTGFS_GINTMSK_HCHINTMSK    (BIT(25))
#define USB_OTGFS_GINTMSK_HCHINTMSK_M  (USB_OTGFS_GINTMSK_HCHINTMSK_V << USB_OTGFS_GINTMSK_HCHINTMSK_S)
#define USB_OTGFS_GINTMSK_HCHINTMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_HCHINTMSK_S  25
/** USB_OTGFS_GINTMSK_PTXFEMPMSK : R/W; bitpos: [26]; default: 0;
 *  Mode: Host only
 *  Periodic TxFIFO Empty Mask (PTxFEmpMsk)
 */
#define USB_OTGFS_GINTMSK_PTXFEMPMSK    (BIT(26))
#define USB_OTGFS_GINTMSK_PTXFEMPMSK_M  (USB_OTGFS_GINTMSK_PTXFEMPMSK_V << USB_OTGFS_GINTMSK_PTXFEMPMSK_S)
#define USB_OTGFS_GINTMSK_PTXFEMPMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_PTXFEMPMSK_S  26
/** USB_OTGFS_GINTMSK_CONIDSTSCHNGMSK : R/W; bitpos: [28]; default: 0;
 *  Mode: Host and Device
 *  Connector ID Status Change Mask (ConIDStsChngMsk)
 */
#define USB_OTGFS_GINTMSK_CONIDSTSCHNGMSK    (BIT(28))
#define USB_OTGFS_GINTMSK_CONIDSTSCHNGMSK_M  (USB_OTGFS_GINTMSK_CONIDSTSCHNGMSK_V << USB_OTGFS_GINTMSK_CONIDSTSCHNGMSK_S)
#define USB_OTGFS_GINTMSK_CONIDSTSCHNGMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_CONIDSTSCHNGMSK_S  28
/** USB_OTGFS_GINTMSK_DISCONNINTMSK : R/W; bitpos: [29]; default: 0;
 *  Mode: Host and Device
 *  Disconnect Detected Interrupt Mask (DisconnIntMsk)
 */
#define USB_OTGFS_GINTMSK_DISCONNINTMSK    (BIT(29))
#define USB_OTGFS_GINTMSK_DISCONNINTMSK_M  (USB_OTGFS_GINTMSK_DISCONNINTMSK_V << USB_OTGFS_GINTMSK_DISCONNINTMSK_S)
#define USB_OTGFS_GINTMSK_DISCONNINTMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_DISCONNINTMSK_S  29
/** USB_OTGFS_GINTMSK_SESSREQINTMSK : R/W; bitpos: [30]; default: 0;
 *  Mode: Host and Device
 *  Session Request/New Session Detected Interrupt Mask (SessReqIntMsk)
 */
#define USB_OTGFS_GINTMSK_SESSREQINTMSK    (BIT(30))
#define USB_OTGFS_GINTMSK_SESSREQINTMSK_M  (USB_OTGFS_GINTMSK_SESSREQINTMSK_V << USB_OTGFS_GINTMSK_SESSREQINTMSK_S)
#define USB_OTGFS_GINTMSK_SESSREQINTMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_SESSREQINTMSK_S  30
/** USB_OTGFS_GINTMSK_WKUPINTMSK : R/W; bitpos: [31]; default: 0;
 *  Mode: Host and Device
 *  Resume/Remote Wakeup Detected Interrupt Mask (WkUpIntMsk)
 *  The WakeUp bit is used for LPM state wake up in a way similar to that of wake up in
 *  suspend state.
 */
#define USB_OTGFS_GINTMSK_WKUPINTMSK    (BIT(31))
#define USB_OTGFS_GINTMSK_WKUPINTMSK_M  (USB_OTGFS_GINTMSK_WKUPINTMSK_V << USB_OTGFS_GINTMSK_WKUPINTMSK_S)
#define USB_OTGFS_GINTMSK_WKUPINTMSK_V  0x00000001U
#define USB_OTGFS_GINTMSK_WKUPINTMSK_S  31

/** USB_OTGFS_GRXSTSR_REG register
 *  Receive Status Debug Read Register A read to the Receive Status Debug Read register
 *  returns the contents of the top of the Receive FIFO.  The receive status contents
 *  must be interpreted differently in Host and Device modes. The core ignores the
 *  receive status read when the receive FIFO is empty and returns a value of
 *  32'h0000_0000.  Note:  - Use of these fields vary based on whether the core is
 *  functioning as a host or a device.  - Do not read this register's reset value
 *  before configuring the core because the read value is 'X' in the simulation.
 */
#define USB_OTGFS_GRXSTSR_REG(i) (DR_REG_USB_BASE(i) + 0x1c)
/** USB_OTGFS_GRXSTSR_CHNUM : RO; bitpos: [3:0]; default: 0;
 *  Channel Number (ChNum)
 *  Mode: Host only
 *  Indicates the channel number to which the current received packet belongs.
 *  Endpoint Number (EPNum)
 *  Mode: Device only
 *  Indicates the endpoint number to which the current received packet belongs.
 */
#define USB_OTGFS_GRXSTSR_CHNUM    0x0000000FU
#define USB_OTGFS_GRXSTSR_CHNUM_M  (USB_OTGFS_GRXSTSR_CHNUM_V << USB_OTGFS_GRXSTSR_CHNUM_S)
#define USB_OTGFS_GRXSTSR_CHNUM_V  0x0000000FU
#define USB_OTGFS_GRXSTSR_CHNUM_S  0
/** USB_OTGFS_GRXSTSR_BCNT : RO; bitpos: [14:4]; default: 0;
 *  Byte Count (BCnt)
 *
 *  In host mode, indicates the byte count of the received IN data packet.
 *
 *  In device mode, indicates the byte count of the received data packet.
 */
#define USB_OTGFS_GRXSTSR_BCNT    0x000007FFU
#define USB_OTGFS_GRXSTSR_BCNT_M  (USB_OTGFS_GRXSTSR_BCNT_V << USB_OTGFS_GRXSTSR_BCNT_S)
#define USB_OTGFS_GRXSTSR_BCNT_V  0x000007FFU
#define USB_OTGFS_GRXSTSR_BCNT_S  4
/** USB_OTGFS_GRXSTSR_DPID : RO; bitpos: [16:15]; default: 0;
 *  Data PID (DPID)
 *
 *  In host mode, indicates the Data PID of the received packet. In device mode,
 *  indicates the Data PID of the received OUT data packet.
 *  - 2'b00: DATA0
 *  - 2'b10: DATA1
 *  - 2'b01: DATA2
 *  - 2'b11: MDATA
 *  Reset: 2'h0
 */
#define USB_OTGFS_GRXSTSR_DPID    0x00000003U
#define USB_OTGFS_GRXSTSR_DPID_M  (USB_OTGFS_GRXSTSR_DPID_V << USB_OTGFS_GRXSTSR_DPID_S)
#define USB_OTGFS_GRXSTSR_DPID_V  0x00000003U
#define USB_OTGFS_GRXSTSR_DPID_S  15
/** USB_OTGFS_GRXSTSR_PKTSTS : RO; bitpos: [20:17]; default: 0;
 *  Packet Status (PktSts) indicates the status of the received packet.
 *  In host mode,
 *  - 4'b0010: IN data packet received
 *  - 4'b0011: IN transfer completed (triggers an interrupt)
 *  - 4'b0101: Data toggle error (triggers an interrupt)
 *  - 4'b0111: Channel halted (triggers an interrupt)
 *  - Others: Reserved
 *  Reset:4'b0
 *
 *  In device mode,
 *  - 4'b0001: Global OUT NAK (triggers an interrupt)
 *  - 4'b0010: OUT data packet received
 *  - 4'b0011: OUT transfer completed (triggers an interrupt)
 *  - 4'b0100: SETUP transaction completed (triggers an interrupt)
 *  - 4'b0110: SETUP data packet received
 *  - Others: Reserved
 *  Reset:4'h0
 */
#define USB_OTGFS_GRXSTSR_PKTSTS    0x0000000FU
#define USB_OTGFS_GRXSTSR_PKTSTS_M  (USB_OTGFS_GRXSTSR_PKTSTS_V << USB_OTGFS_GRXSTSR_PKTSTS_S)
#define USB_OTGFS_GRXSTSR_PKTSTS_V  0x0000000FU
#define USB_OTGFS_GRXSTSR_PKTSTS_S  17
/** USB_OTGFS_GRXSTSR_FN : RO; bitpos: [24:21]; default: 0;
 *  Mode: Device only
 *  Frame Number (FN)
 *  This is the least significant 4 bits of the (micro)Frame number in which the packet
 *  is received on the USB. This field is supported only when isochronous OUT endpoints
 *  are supported.
 */
#define USB_OTGFS_GRXSTSR_FN    0x0000000FU
#define USB_OTGFS_GRXSTSR_FN_M  (USB_OTGFS_GRXSTSR_FN_V << USB_OTGFS_GRXSTSR_FN_S)
#define USB_OTGFS_GRXSTSR_FN_V  0x0000000FU
#define USB_OTGFS_GRXSTSR_FN_S  21

/** USB_OTGFS_GRXSTSP_REG register
 *  Receive Status Read/Pop Register A read to the Receive Status Read and Pop register
 *  returns the contents of the top of the Receive FIFO and additionally pops the top
 *  data entry out of the RxFIFO. The receive status contents must be interpreted
 *  differently in Host and Device modes. The core ignores the receive status pop/read
 *  when the receive FIFO is empty and returns a value of 32'h0000_0000. The
 *  application must only pop the Receive Status FIFO when the Receive FIFO Non-Empty
 *  bit of the Core Interrupt register (GINTSTS.RxFLvl) is asserted.  Note:  - Use of
 *  these fields vary based on whether the core is functioning as a host or a device.
 *  - Do not read this register's reset value before configuring the core because the
 *  read value is 'X' in the simulation.
 */
#define USB_OTGFS_GRXSTSP_REG(i) (DR_REG_USB_BASE(i) + 0x20)
/** USB_OTGFS_GRXSTSP_CHNUM : RO; bitpos: [3:0]; default: 0;
 *  Channel Number (ChNum)
 *  Mode: Host only
 *  Indicates the channel number to which the current received packet belongs.
 *  Endpoint Number (EPNum)
 *  Mode: Device only
 *  Indicates the endpoint number to which the current received packet belongs.
 */
#define USB_OTGFS_GRXSTSP_CHNUM    0x0000000FU
#define USB_OTGFS_GRXSTSP_CHNUM_M  (USB_OTGFS_GRXSTSP_CHNUM_V << USB_OTGFS_GRXSTSP_CHNUM_S)
#define USB_OTGFS_GRXSTSP_CHNUM_V  0x0000000FU
#define USB_OTGFS_GRXSTSP_CHNUM_S  0
/** USB_OTGFS_GRXSTSP_BCNT : RO; bitpos: [14:4]; default: 0;
 *  Byte Count (BCnt)
 *
 *  In host mode, indicates the byte count of the received IN data packet.
 *
 *  In device mode, indicates the byte count of the received data packet.
 */
#define USB_OTGFS_GRXSTSP_BCNT    0x000007FFU
#define USB_OTGFS_GRXSTSP_BCNT_M  (USB_OTGFS_GRXSTSP_BCNT_V << USB_OTGFS_GRXSTSP_BCNT_S)
#define USB_OTGFS_GRXSTSP_BCNT_V  0x000007FFU
#define USB_OTGFS_GRXSTSP_BCNT_S  4
/** USB_OTGFS_GRXSTSP_DPID : RO; bitpos: [16:15]; default: 0;
 *  Data PID (DPID)
 *
 *  In host mode, indicates the Data PID of the received packet. In device mode,
 *  indicates the Data PID of the received OUT data packet.
 *  - 2'b00: DATA0
 *  - 2'b10: DATA1
 *  - 2'b01: DATA2
 *  - 2'b11: MDATA
 *  Reset: 2'h0
 */
#define USB_OTGFS_GRXSTSP_DPID    0x00000003U
#define USB_OTGFS_GRXSTSP_DPID_M  (USB_OTGFS_GRXSTSP_DPID_V << USB_OTGFS_GRXSTSP_DPID_S)
#define USB_OTGFS_GRXSTSP_DPID_V  0x00000003U
#define USB_OTGFS_GRXSTSP_DPID_S  15
/** USB_OTGFS_GRXSTSP_PKTSTS : RO; bitpos: [20:17]; default: 0;
 *  Packet Status (PktSts) indicates the status of the received packet.
 *  In host mode,
 *  - 4'b0010: IN data packet received
 *  - 4'b0011: IN transfer completed (triggers an interrupt)
 *  - 4'b0101: Data toggle error (triggers an interrupt)
 *  - 4'b0111: Channel halted (triggers an interrupt)
 *  - Others: Reserved
 *  Reset:4'b0
 *
 *  In device mode,
 *  - 4'b0001: Global OUT NAK (triggers an interrupt)
 *  - 4'b0010: OUT data packet received
 *  - 4'b0011: OUT transfer completed (triggers an interrupt)
 *  - 4'b0100: SETUP transaction completed (triggers an interrupt)
 *  - 4'b0110: SETUP data packet received
 *  - Others: Reserved
 *  Reset:4'h0
 */
#define USB_OTGFS_GRXSTSP_PKTSTS    0x0000000FU
#define USB_OTGFS_GRXSTSP_PKTSTS_M  (USB_OTGFS_GRXSTSP_PKTSTS_V << USB_OTGFS_GRXSTSP_PKTSTS_S)
#define USB_OTGFS_GRXSTSP_PKTSTS_V  0x0000000FU
#define USB_OTGFS_GRXSTSP_PKTSTS_S  17
/** USB_OTGFS_GRXSTSP_FN : RO; bitpos: [24:21]; default: 0;
 *  Mode: Device only
 *  Frame Number (FN)
 *  This is the least significant 4 bits of the (micro)Frame number in which the packet
 *  is received on the USB. This field is supported only when isochronous OUT endpoints
 *  are supported.
 */
#define USB_OTGFS_GRXSTSP_FN    0x0000000FU
#define USB_OTGFS_GRXSTSP_FN_M  (USB_OTGFS_GRXSTSP_FN_V << USB_OTGFS_GRXSTSP_FN_S)
#define USB_OTGFS_GRXSTSP_FN_V  0x0000000FU
#define USB_OTGFS_GRXSTSP_FN_S  21

/** USB_OTGFS_GRXFSIZ_REG register
 *  Receive FIFO Size Register The application can program the RAM size that must be
 *  allocated to the RxFIFO.
 */
#define USB_OTGFS_GRXFSIZ_REG(i) (DR_REG_USB_BASE(i) + 0x24)
/** USB_OTGFS_GRXFSIZ_RXFDEP : R/W; bitpos: [8:0]; default: 256;
 *  Mode: Host and Device
 *  RxFIFO Depth (RxFDep)
 *  This value is in terms of 32-bit words.
 *  - Minimum value is 16
 *  - Maximum value is 32,768
 *  The power-on reset value of this register is specified as the Largest Rx Data FIFO
 *  Depth during configuration.
 *  If Enable Dynamic FIFO Sizing is selected in coreConsultant, these flops are
 *  optimized, and reads return the power-on value.
 *  If Enable Dynamic FIFO Sizing is selected in coreConsultant, you can write a new
 *  value in this field. Programmed values must not exceed the power-on value.
 */
#define USB_OTGFS_GRXFSIZ_RXFDEP    0x000001FFU
#define USB_OTGFS_GRXFSIZ_RXFDEP_M  (USB_OTGFS_GRXFSIZ_RXFDEP_V << USB_OTGFS_GRXFSIZ_RXFDEP_S)
#define USB_OTGFS_GRXFSIZ_RXFDEP_V  0x000001FFU
#define USB_OTGFS_GRXFSIZ_RXFDEP_S  0

/** USB_OTGFS_GNPTXFSIZ_REG register
 *  Non-periodic Transmit FIFO Size Register The application can program the RAM size
 *  and the memory start address for the Non-periodic TxFIFO  Note: The fields of this
 *  register change depending on host or device mode.
 */
#define USB_OTGFS_GNPTXFSIZ_REG(i) (DR_REG_USB_BASE(i) + 0x28)
/** USB_OTGFS_GNPTXFSIZ_NPTXFSTADDR : R/W; bitpos: [8:0]; default: 256;
 *  Non-periodic Transmit RAM Start Address (NPTxFStAddr)
 *  For host mode, this field is always valid.
 *  This field contains the memory start address for Non-periodic Transmit FIFO RAM.
 *  - This field is determined during coreConsultant configuration by Enable Dynamic
 *  FIFO Sizing? (OTG_DFIFO_DYNAMIC):OTG_DFIFO_DYNAMIC = 0
 *  These flops are optimized, and reads return the power-on value.
 *  - OTG_DFIFO_DYNAMIC = 1 The application can write a new value in this field.
 *  Programmed values must not exceed the power-on value set in coreConsultant.
 *  Programmed values must not exceed the power-on value set in coreConsultant.
 *  The power-on reset value of this field is specified during coreConsultant
 *  configuration by Largest Rx Data FIFO Depth (parameter OTG_RX_DFIFO_DEPTH).
 */
#define USB_OTGFS_GNPTXFSIZ_NPTXFSTADDR    0x000001FFU
#define USB_OTGFS_GNPTXFSIZ_NPTXFSTADDR_M  (USB_OTGFS_GNPTXFSIZ_NPTXFSTADDR_V << USB_OTGFS_GNPTXFSIZ_NPTXFSTADDR_S)
#define USB_OTGFS_GNPTXFSIZ_NPTXFSTADDR_V  0x000001FFU
#define USB_OTGFS_GNPTXFSIZ_NPTXFSTADDR_S  0
/** USB_OTGFS_GNPTXFSIZ_NPTXFDEP : R/W; bitpos: [24:16]; default: 256;
 *  Mode: Host only
 *  Non-periodic TxFIFO Depth (NPTxFDep)
 *  For host mode, this field is always valid.
 *  For device mode, this field is valid only when OTG_EN_DED_TX_FIFO=0.
 *  This value is in terms of 32-bit words.
 *  - Minimum value is 16
 *  - Maximum value is 32,768
 *  This attribute of field is determined during coreConsultant configuration by Enable
 *  Dynamic FIFO Sizing? (OTG_DFIFO_DYNAMIC):
 *  - OTG_DFIFO_DYNAMIC = 0: These flops are optimized, and reads return the power-on
 *  value.
 *  - OTG_DFIFO_DYNAMIC = 1: The application can write a new value in this field.
 *  Programmed values must not exceed the power-on value set in coreConsultant.
 *  The power-on reset value of this field is specified during coreConsultant
 *  configuration as Largest IN Endpoint FIFO 0 Depth (parameter
 *  OTG_TX_DINEP_DFIFO_DEPTH_0).
 */
#define USB_OTGFS_GNPTXFSIZ_NPTXFDEP    0x000001FFU
#define USB_OTGFS_GNPTXFSIZ_NPTXFDEP_M  (USB_OTGFS_GNPTXFSIZ_NPTXFDEP_V << USB_OTGFS_GNPTXFSIZ_NPTXFDEP_S)
#define USB_OTGFS_GNPTXFSIZ_NPTXFDEP_V  0x000001FFU
#define USB_OTGFS_GNPTXFSIZ_NPTXFDEP_S  16

/** USB_OTGFS_GNPTXSTS_REG register
 *  Non-periodic Transmit FIFO/Queue Status Register In Device mode, this register is
 *  valid only in Shared FIFO operation. This read-only register contains the free
 *  space information for the Non-periodic TxFIFO and the Non-periodic Transmit Request
 *  Queue.
 */
#define USB_OTGFS_GNPTXSTS_REG(i) (DR_REG_USB_BASE(i) + 0x2c)
/** USB_OTGFS_GNPTXSTS_NPTXFSPCAVAIL : RO; bitpos: [15:0]; default: 256;
 *  Non-periodic TxFIFO Space Avail (NPTxFSpcAvail)
 *  Indicates the amount of free space available in the Non-periodic TxFIFO.
 *  Values are in terms of 32-bit words.
 *  - 16'h0: Non-periodic TxFIFO is full
 *  - 16'h1: 1 word available
 *  - 16'h2: 2 words available
 *  - 16'hn: n words available (where 0 <= n <= 32,768)
 *  - 16'h8000: 32,768 words available
 *  - Others: Reserved
 *  Reset: Configurable
 */
#define USB_OTGFS_GNPTXSTS_NPTXFSPCAVAIL    0x0000FFFFU
#define USB_OTGFS_GNPTXSTS_NPTXFSPCAVAIL_M  (USB_OTGFS_GNPTXSTS_NPTXFSPCAVAIL_V << USB_OTGFS_GNPTXSTS_NPTXFSPCAVAIL_S)
#define USB_OTGFS_GNPTXSTS_NPTXFSPCAVAIL_V  0x0000FFFFU
#define USB_OTGFS_GNPTXSTS_NPTXFSPCAVAIL_S  0
/** USB_OTGFS_GNPTXSTS_NPTXQSPCAVAIL : RO; bitpos: [23:16]; default: 4;
 *  Non-periodic Transmit Request Queue Space Available (NPTxQSpcAvail)
 *  Indicates the amount of free space available in the Non-periodic Transmit Request
 *  Queue. This queue holds both IN and OUT requests in Host mode. Device mode has only
 *  IN requests.
 *  - 8'h0: Non-periodic Transmit Request Queue is full
 *  - 8'h1: 1 location available
 *  - 8'h2: 2 locations available
 *  - n: n locations available (0 <= n <= 8)
 *  - Others: Reserved
 *  Reset: Configurable
 */
#define USB_OTGFS_GNPTXSTS_NPTXQSPCAVAIL    0x000000FFU
#define USB_OTGFS_GNPTXSTS_NPTXQSPCAVAIL_M  (USB_OTGFS_GNPTXSTS_NPTXQSPCAVAIL_V << USB_OTGFS_GNPTXSTS_NPTXQSPCAVAIL_S)
#define USB_OTGFS_GNPTXSTS_NPTXQSPCAVAIL_V  0x000000FFU
#define USB_OTGFS_GNPTXSTS_NPTXQSPCAVAIL_S  16
/** USB_OTGFS_GNPTXSTS_NPTXQTOP : RO; bitpos: [30:24]; default: 0;
 *  Top of the Non-periodic Transmit Request Queue (NPTxQTop)
 *  Entry in the Non-periodic Tx Request Queue that is currently being processed by the
 *  MAC.
 *
 *  - Bits [30:27]: Channel/endpoint number
 *  - Bits [26:25]:
 *  - 2'b00: IN/OUT token
 *  -- 2'b01: Zero-length transmit packet (device IN/host OUT)
 *  -- 2'b10: PING/CSPLIT token
 *  -- 2'b11: Channel halt command
 *  - Bit [24]: Terminate (last Entry for selected channel/endpoint)
 *  Reset: 7'h0
 */
#define USB_OTGFS_GNPTXSTS_NPTXQTOP    0x0000007FU
#define USB_OTGFS_GNPTXSTS_NPTXQTOP_M  (USB_OTGFS_GNPTXSTS_NPTXQTOP_V << USB_OTGFS_GNPTXSTS_NPTXQTOP_S)
#define USB_OTGFS_GNPTXSTS_NPTXQTOP_V  0x0000007FU
#define USB_OTGFS_GNPTXSTS_NPTXQTOP_S  24

/** USB_OTGFS_GSNPSID_REG register
 *  Synopsis ID Register  This read-only register contains the release number of the
 *  core being used.
 */
#define USB_OTGFS_GSNPSID_REG(i) (DR_REG_USB_BASE(i) + 0x40)
/** USB_OTGFS_GSNPSID_SYNOPSYSID : RO; bitpos: [31:0]; default: 1330921482;
 *  Release number of the controller being used currently.
 */
#define USB_OTGFS_GSNPSID_SYNOPSYSID    0xFFFFFFFFU
#define USB_OTGFS_GSNPSID_SYNOPSYSID_M  (USB_OTGFS_GSNPSID_SYNOPSYSID_V << USB_OTGFS_GSNPSID_SYNOPSYSID_S)
#define USB_OTGFS_GSNPSID_SYNOPSYSID_V  0xFFFFFFFFU
#define USB_OTGFS_GSNPSID_SYNOPSYSID_S  0

/** USB_OTGFS_GHWCFG1_REG register
 *  User Hardware Configuration 1 Register  This register contains the logical endpoint
 *  direction(s) selected using coreConsultant.
 */
#define USB_OTGFS_GHWCFG1_REG(i) (DR_REG_USB_BASE(i) + 0x44)
/** USB_OTGFS_GHWCFG1_EPDIR : RO; bitpos: [31:0]; default: 0;
 *  This 32-bit field uses two bits per
 *  endpoint to determine the endpoint direction.
 *
 *  Endpoint
 *  - Bits [31:30]: Endpoint 15 direction
 *  - Bits [29:28]: Endpoint 14 direction
 *  ...
 *  - Bits [3:2]: Endpoint 1 direction
 *  - Bits[1:0]: Endpoint 0 direction (always BIDIR)
 *  Direction
 *  - 2'b00: BIDIR (IN and OUT) endpoint
 *  - 2'b01: IN endpoint
 *  - 2'b10: OUT endpoint
 *  - 2'b11: Reserved
 *  Note: This field is configured using the OTG_EP_DIR_1(n) parameter.
 */
#define USB_OTGFS_GHWCFG1_EPDIR    0xFFFFFFFFU
#define USB_OTGFS_GHWCFG1_EPDIR_M  (USB_OTGFS_GHWCFG1_EPDIR_V << USB_OTGFS_GHWCFG1_EPDIR_S)
#define USB_OTGFS_GHWCFG1_EPDIR_V  0xFFFFFFFFU
#define USB_OTGFS_GHWCFG1_EPDIR_S  0

/** USB_OTGFS_GHWCFG2_REG register
 *  User Hardware Configuration 2 Register  This register contains configuration
 *  options selected using coreConsultant.
 */
#define USB_OTGFS_GHWCFG2_REG(i) (DR_REG_USB_BASE(i) + 0x48)
/** USB_OTGFS_GHWCFG2_OTGMODE : RO; bitpos: [2:0]; default: 0;
 *  Mode of Operation (OtgMode)
 *  - 3'b000: HNP- and SRP-Capable OTG (Host & Device)
 *  - 3'b001: SRP-Capable OTG (Host & Device)
 *  - 3'b010: Non-HNP and Non-SRP Capable OTG (Host and Device)
 *  - 3'b011: SRP-Capable Device
 *  - 3'b100: Non-OTG Device
 *  - 3'b101: SRP-Capable Host
 *  - 3'b110: Non-OTG Host
 *  - Others: Reserved
 *  Note: This field is configured using the OTG_MODE parameter.
 */
#define USB_OTGFS_GHWCFG2_OTGMODE    0x00000007U
#define USB_OTGFS_GHWCFG2_OTGMODE_M  (USB_OTGFS_GHWCFG2_OTGMODE_V << USB_OTGFS_GHWCFG2_OTGMODE_S)
#define USB_OTGFS_GHWCFG2_OTGMODE_V  0x00000007U
#define USB_OTGFS_GHWCFG2_OTGMODE_S  0
/** USB_OTGFS_GHWCFG2_OTGARCH : RO; bitpos: [4:3]; default: 2;
 *  Architecture (OtgArch)
 *  - 2'b00: Slave-Only
 *  - 2'b01: External DMA
 *  - 2'b10: Internal DMA
 *  - Others: Reserved
 *  Note: This field is configured using the OTG_ARCHITECTURE parameter.
 */
#define USB_OTGFS_GHWCFG2_OTGARCH    0x00000003U
#define USB_OTGFS_GHWCFG2_OTGARCH_M  (USB_OTGFS_GHWCFG2_OTGARCH_V << USB_OTGFS_GHWCFG2_OTGARCH_S)
#define USB_OTGFS_GHWCFG2_OTGARCH_V  0x00000003U
#define USB_OTGFS_GHWCFG2_OTGARCH_S  3
/** USB_OTGFS_GHWCFG2_SINGPNT : RO; bitpos: [5]; default: 1;
 *  Point-to-Point (SingPnt)
 *  - 1'b0: Multi-point application  (hub and split support)
 *  - 1'b1: Single-point application (no hub and split support)
 *  Note: This field is configured using the OTG_SINGLE_POINT parameter.
 */
#define USB_OTGFS_GHWCFG2_SINGPNT    (BIT(5))
#define USB_OTGFS_GHWCFG2_SINGPNT_M  (USB_OTGFS_GHWCFG2_SINGPNT_V << USB_OTGFS_GHWCFG2_SINGPNT_S)
#define USB_OTGFS_GHWCFG2_SINGPNT_V  0x00000001U
#define USB_OTGFS_GHWCFG2_SINGPNT_S  5
/** USB_OTGFS_GHWCFG2_HSPHYTYPE : RO; bitpos: [7:6]; default: 0;
 *  High-Speed PHY Interface Type (HSPhyType)
 *  - 2'b00: High-Speed interface not supported
 *  - 2'b01: UTMI+
 *  - 2'b10: ULPI
 *  - 2'b11: UTMI+ and ULPI
 *  Note: This field is configured using the OTG_HSPHY_INTERFACE parameter.
 */
#define USB_OTGFS_GHWCFG2_HSPHYTYPE    0x00000003U
#define USB_OTGFS_GHWCFG2_HSPHYTYPE_M  (USB_OTGFS_GHWCFG2_HSPHYTYPE_V << USB_OTGFS_GHWCFG2_HSPHYTYPE_S)
#define USB_OTGFS_GHWCFG2_HSPHYTYPE_V  0x00000003U
#define USB_OTGFS_GHWCFG2_HSPHYTYPE_S  6
/** USB_OTGFS_GHWCFG2_FSPHYTYPE : RO; bitpos: [9:8]; default: 1;
 *  Full-Speed PHY Interface Type (FSPhyType)
 *  - 2'b00: Full-speed interface not supported
 *  - 2'b01: Dedicated full-speed interface
 *  - 2'b10: FS pins shared with UTMI+ pins
 *  - 2'b11: FS pins shared with ULPI pins
 *  Note: This field is configured using the OTG_FSPHY_INTERFACE parameter.
 */
#define USB_OTGFS_GHWCFG2_FSPHYTYPE    0x00000003U
#define USB_OTGFS_GHWCFG2_FSPHYTYPE_M  (USB_OTGFS_GHWCFG2_FSPHYTYPE_V << USB_OTGFS_GHWCFG2_FSPHYTYPE_S)
#define USB_OTGFS_GHWCFG2_FSPHYTYPE_V  0x00000003U
#define USB_OTGFS_GHWCFG2_FSPHYTYPE_S  8
/** USB_OTGFS_GHWCFG2_NUMDEVEPS : RO; bitpos: [13:10]; default: 6;
 *  Number of Device Endpoints (NumDevEps)
 *
 *  Indicates the number of device endpoints supported by the core in Device mode.
 *
 *  The range of this field is 0-15.
 *
 *  Note: This field is configured using the OTG_NUM_EPS parameter.
 */
#define USB_OTGFS_GHWCFG2_NUMDEVEPS    0x0000000FU
#define USB_OTGFS_GHWCFG2_NUMDEVEPS_M  (USB_OTGFS_GHWCFG2_NUMDEVEPS_V << USB_OTGFS_GHWCFG2_NUMDEVEPS_S)
#define USB_OTGFS_GHWCFG2_NUMDEVEPS_V  0x0000000FU
#define USB_OTGFS_GHWCFG2_NUMDEVEPS_S  10
/** USB_OTGFS_GHWCFG2_NUMHSTCHNL : RO; bitpos: [17:14]; default: 7;
 *  Number of Host Channels (NumHstChnl)
 *  Indicates the number of host channels supported by the core in Host mode. The range
 *  of this field is 0-15: 0 specifies 1 channel, 15 specifies 16 channels.
 *
 *  Note: This field is configured using the OTG_NUM_HOST_CHAN parameter.
 */
#define USB_OTGFS_GHWCFG2_NUMHSTCHNL    0x0000000FU
#define USB_OTGFS_GHWCFG2_NUMHSTCHNL_M  (USB_OTGFS_GHWCFG2_NUMHSTCHNL_V << USB_OTGFS_GHWCFG2_NUMHSTCHNL_S)
#define USB_OTGFS_GHWCFG2_NUMHSTCHNL_V  0x0000000FU
#define USB_OTGFS_GHWCFG2_NUMHSTCHNL_S  14
/** USB_OTGFS_GHWCFG2_PERIOSUPPORT : RO; bitpos: [18]; default: 1;
 *  Periodic OUT Channels Supported in Host Mode (PerioSupport)
 *  - 1'b0: No
 *  - 1'b1: Yes
 *  Note: This field is configured using the OTG_EN_PERIO_HOST parameter.
 */
#define USB_OTGFS_GHWCFG2_PERIOSUPPORT    (BIT(18))
#define USB_OTGFS_GHWCFG2_PERIOSUPPORT_M  (USB_OTGFS_GHWCFG2_PERIOSUPPORT_V << USB_OTGFS_GHWCFG2_PERIOSUPPORT_S)
#define USB_OTGFS_GHWCFG2_PERIOSUPPORT_V  0x00000001U
#define USB_OTGFS_GHWCFG2_PERIOSUPPORT_S  18
/** USB_OTGFS_GHWCFG2_DYNFIFOSIZING : RO; bitpos: [19]; default: 1;
 *  Dynamic FIFO Sizing Enabled (DynFifoSizing)
 *  - 1'b0: No
 *  - 1'b1: Yes
 *  Note: This field is configured using the OTG_DFIFO_DYNAMIC parameter.
 */
#define USB_OTGFS_GHWCFG2_DYNFIFOSIZING    (BIT(19))
#define USB_OTGFS_GHWCFG2_DYNFIFOSIZING_M  (USB_OTGFS_GHWCFG2_DYNFIFOSIZING_V << USB_OTGFS_GHWCFG2_DYNFIFOSIZING_S)
#define USB_OTGFS_GHWCFG2_DYNFIFOSIZING_V  0x00000001U
#define USB_OTGFS_GHWCFG2_DYNFIFOSIZING_S  19
/** USB_OTGFS_GHWCFG2_MULTIPROCINTRPT : RO; bitpos: [20]; default: 0;
 *  Multi Processor Interrupt Enabled (MultiProcIntrpt)
 *  - 1'b0: No
 *  - 1'b1: Yes
 *  Note: This field is configured using the OTG_MULTI_PROC_INTRPT parameter.
 */
#define USB_OTGFS_GHWCFG2_MULTIPROCINTRPT    (BIT(20))
#define USB_OTGFS_GHWCFG2_MULTIPROCINTRPT_M  (USB_OTGFS_GHWCFG2_MULTIPROCINTRPT_V << USB_OTGFS_GHWCFG2_MULTIPROCINTRPT_S)
#define USB_OTGFS_GHWCFG2_MULTIPROCINTRPT_V  0x00000001U
#define USB_OTGFS_GHWCFG2_MULTIPROCINTRPT_S  20
/** USB_OTGFS_GHWCFG2_NPTXQDEPTH : RO; bitpos: [23:22]; default: 1;
 *  Non-periodic Request Queue Depth (NPTxQDepth)
 *  - 2'b00: 2
 *  - 2'b01: 4
 *  - 2'b10: 8
 *  - Others: Reserved
 *  Note: This field is configured using the OTG_NPERIO_TX_QUEUE_DEPTH parameter.
 */
#define USB_OTGFS_GHWCFG2_NPTXQDEPTH    0x00000003U
#define USB_OTGFS_GHWCFG2_NPTXQDEPTH_M  (USB_OTGFS_GHWCFG2_NPTXQDEPTH_V << USB_OTGFS_GHWCFG2_NPTXQDEPTH_S)
#define USB_OTGFS_GHWCFG2_NPTXQDEPTH_V  0x00000003U
#define USB_OTGFS_GHWCFG2_NPTXQDEPTH_S  22
/** USB_OTGFS_GHWCFG2_PTXQDEPTH : RO; bitpos: [25:24]; default: 2;
 *  Host Mode Periodic Request Queue Depth (PTxQDepth)
 *  - 2'b00: 2
 *  - 2'b01: 4
 *  - 2'b10: 8
 *  - 2'b11:16
 *  Note: This field is configured using the OTG_PERIO_TX_QUEUE_DEPTH parameter.
 */
#define USB_OTGFS_GHWCFG2_PTXQDEPTH    0x00000003U
#define USB_OTGFS_GHWCFG2_PTXQDEPTH_M  (USB_OTGFS_GHWCFG2_PTXQDEPTH_V << USB_OTGFS_GHWCFG2_PTXQDEPTH_S)
#define USB_OTGFS_GHWCFG2_PTXQDEPTH_V  0x00000003U
#define USB_OTGFS_GHWCFG2_PTXQDEPTH_S  24
/** USB_OTGFS_GHWCFG2_TKNQDEPTH : RO; bitpos: [30:26]; default: 8;
 *  Device Mode IN Token Sequence Learning Queue Depth (TknQDepth)
 *  Range: 0-30
 *
 *  Note: This field is configured using the OTG_TOKEN_QUEUE_DEPTH parameter.
 */
#define USB_OTGFS_GHWCFG2_TKNQDEPTH    0x0000001FU
#define USB_OTGFS_GHWCFG2_TKNQDEPTH_M  (USB_OTGFS_GHWCFG2_TKNQDEPTH_V << USB_OTGFS_GHWCFG2_TKNQDEPTH_S)
#define USB_OTGFS_GHWCFG2_TKNQDEPTH_V  0x0000001FU
#define USB_OTGFS_GHWCFG2_TKNQDEPTH_S  26

/** USB_OTGFS_GHWCFG3_REG register
 *  User Hardware Configuration 3 Register
 */
#define USB_OTGFS_GHWCFG3_REG(i) (DR_REG_USB_BASE(i) + 0x4c)
/** USB_OTGFS_GHWCFG3_XFERSIZEWIDTH : RO; bitpos: [3:0]; default: 5;
 *  Width of Transfer Size Counters (XferSizeWidth)
 *  - 4'b0000: 11 bits
 *  - 4'b0001: 12 bits
 *  ...
 *  - 4'b1000: 19 bits
 *  - Others: Reserved
 *  Note: This field is configured using the OTG_PACKET_COUNT_WIDTH parameter.
 */
#define USB_OTGFS_GHWCFG3_XFERSIZEWIDTH    0x0000000FU
#define USB_OTGFS_GHWCFG3_XFERSIZEWIDTH_M  (USB_OTGFS_GHWCFG3_XFERSIZEWIDTH_V << USB_OTGFS_GHWCFG3_XFERSIZEWIDTH_S)
#define USB_OTGFS_GHWCFG3_XFERSIZEWIDTH_V  0x0000000FU
#define USB_OTGFS_GHWCFG3_XFERSIZEWIDTH_S  0
/** USB_OTGFS_GHWCFG3_PKTSIZEWIDTH : RO; bitpos: [6:4]; default: 3;
 *  Width of Packet Size Counters (PktSizeWidth)
 *  - 3'b000: 4 bits
 *  - 3'b001: 5 bits
 *  - 3'b010: 6 bits
 *  - 3'b011: 7 bits
 *  - 3'b100: 8 bits
 *  - 3'b101: 9 bits
 *  - 3'b110: 10 bits
 *  - Others: Reserved
 *  Note: This field is configured using the OTG_PACKET_COUNT_WIDTH parameter.
 */
#define USB_OTGFS_GHWCFG3_PKTSIZEWIDTH    0x00000007U
#define USB_OTGFS_GHWCFG3_PKTSIZEWIDTH_M  (USB_OTGFS_GHWCFG3_PKTSIZEWIDTH_V << USB_OTGFS_GHWCFG3_PKTSIZEWIDTH_S)
#define USB_OTGFS_GHWCFG3_PKTSIZEWIDTH_V  0x00000007U
#define USB_OTGFS_GHWCFG3_PKTSIZEWIDTH_S  4
/** USB_OTGFS_GHWCFG3_OTGEN : RO; bitpos: [7]; default: 1;
 *  OTG Function Enabled (OtgEn)
 *
 *  The application uses this bit to indicate the OTG capabilities of the controller .
 *  - 1'b0: Not OTG capable
 *  - 1'b1: OTG Capable
 *  Note: This field is configured using the OTG_MODE parameter.
 */
#define USB_OTGFS_GHWCFG3_OTGEN    (BIT(7))
#define USB_OTGFS_GHWCFG3_OTGEN_M  (USB_OTGFS_GHWCFG3_OTGEN_V << USB_OTGFS_GHWCFG3_OTGEN_S)
#define USB_OTGFS_GHWCFG3_OTGEN_V  0x00000001U
#define USB_OTGFS_GHWCFG3_OTGEN_S  7
/** USB_OTGFS_GHWCFG3_I2CINTSEL : RO; bitpos: [8]; default: 0;
 *  I2C Selection (I2CIntSel)
 *  - 1'b0: I2C Interface is not available on the controller.
 *  - 1'b1: I2C Interface is available on the controller.
 *  Note: This field is configured using the OTG_I2C_INTERFACE parameter.
 */
#define USB_OTGFS_GHWCFG3_I2CINTSEL    (BIT(8))
#define USB_OTGFS_GHWCFG3_I2CINTSEL_M  (USB_OTGFS_GHWCFG3_I2CINTSEL_V << USB_OTGFS_GHWCFG3_I2CINTSEL_S)
#define USB_OTGFS_GHWCFG3_I2CINTSEL_V  0x00000001U
#define USB_OTGFS_GHWCFG3_I2CINTSEL_S  8
/** USB_OTGFS_GHWCFG3_VNDCTLSUPT : RO; bitpos: [9]; default: 0;
 *  Vendor Control Interface Support (VndctlSupt)
 *
 *  - 1'b0: Vendor Control Interface is not available on the core.
 *  - 1'b1: Vendor Control Interface is available.
 *  Note: This field is configured using the OTG_VENDOR_CTL_INTERFACE parameter.
 */
#define USB_OTGFS_GHWCFG3_VNDCTLSUPT    (BIT(9))
#define USB_OTGFS_GHWCFG3_VNDCTLSUPT_M  (USB_OTGFS_GHWCFG3_VNDCTLSUPT_V << USB_OTGFS_GHWCFG3_VNDCTLSUPT_S)
#define USB_OTGFS_GHWCFG3_VNDCTLSUPT_V  0x00000001U
#define USB_OTGFS_GHWCFG3_VNDCTLSUPT_S  9
/** USB_OTGFS_GHWCFG3_OPTFEATURE : RO; bitpos: [10]; default: 1;
 *  Optional Features Removed (OptFeature)
 *  Indicates whether the User ID register, GPIO interface ports, and SOF toggle and
 *  counter ports were removed for gate count optimization by enabling Remove Optional
 *  Features.
 *  - 1'b0: No
 *  - 1'b1: Yes
 *  Note: This field is configured using the OTG_RM_OPT_FEATURES parameter.
 */
#define USB_OTGFS_GHWCFG3_OPTFEATURE    (BIT(10))
#define USB_OTGFS_GHWCFG3_OPTFEATURE_M  (USB_OTGFS_GHWCFG3_OPTFEATURE_V << USB_OTGFS_GHWCFG3_OPTFEATURE_S)
#define USB_OTGFS_GHWCFG3_OPTFEATURE_V  0x00000001U
#define USB_OTGFS_GHWCFG3_OPTFEATURE_S  10
/** USB_OTGFS_GHWCFG3_RSTTYPE : RO; bitpos: [11]; default: 0;
 *  Reset Style for Clocked always Blocks in RTL (RstType)
 *  - 1'b0: Asynchronous reset is used in the controller
 *  - 1'b1: Synchronous reset is used in the controller
 *  Note: This field is configured using the OTG_SYNC_RESET_TYPE parameter.
 */
#define USB_OTGFS_GHWCFG3_RSTTYPE    (BIT(11))
#define USB_OTGFS_GHWCFG3_RSTTYPE_M  (USB_OTGFS_GHWCFG3_RSTTYPE_V << USB_OTGFS_GHWCFG3_RSTTYPE_S)
#define USB_OTGFS_GHWCFG3_RSTTYPE_V  0x00000001U
#define USB_OTGFS_GHWCFG3_RSTTYPE_S  11
/** USB_OTGFS_GHWCFG3_ADPSUPPORT : RO; bitpos: [12]; default: 0;
 *  This bit indicates whether ADP logic is present within or external to the controller
 *  - 0: No ADP logic present with the controller
 *  - 1: ADP logic is present along with the controller.
 */
#define USB_OTGFS_GHWCFG3_ADPSUPPORT    (BIT(12))
#define USB_OTGFS_GHWCFG3_ADPSUPPORT_M  (USB_OTGFS_GHWCFG3_ADPSUPPORT_V << USB_OTGFS_GHWCFG3_ADPSUPPORT_S)
#define USB_OTGFS_GHWCFG3_ADPSUPPORT_V  0x00000001U
#define USB_OTGFS_GHWCFG3_ADPSUPPORT_S  12
/** USB_OTGFS_GHWCFG3_HSICMODE : RO; bitpos: [13]; default: 0;
 *  HSIC mode specified for Mode of Operation
 *  Value Range: 0 - 1
 *  - 1: HSIC-capable with shared UTMI PHY interface
 *  - 0: Non-HSIC-capable
 */
#define USB_OTGFS_GHWCFG3_HSICMODE    (BIT(13))
#define USB_OTGFS_GHWCFG3_HSICMODE_M  (USB_OTGFS_GHWCFG3_HSICMODE_V << USB_OTGFS_GHWCFG3_HSICMODE_S)
#define USB_OTGFS_GHWCFG3_HSICMODE_V  0x00000001U
#define USB_OTGFS_GHWCFG3_HSICMODE_S  13
/** USB_OTGFS_GHWCFG3_BCSUPPORT : RO; bitpos: [14]; default: 0;
 *  This bit indicates the controller support for Battery Charger.
 *  - 0 - No Battery Charger Support
 *  - 1 - Battery Charger support present
 */
#define USB_OTGFS_GHWCFG3_BCSUPPORT    (BIT(14))
#define USB_OTGFS_GHWCFG3_BCSUPPORT_M  (USB_OTGFS_GHWCFG3_BCSUPPORT_V << USB_OTGFS_GHWCFG3_BCSUPPORT_S)
#define USB_OTGFS_GHWCFG3_BCSUPPORT_V  0x00000001U
#define USB_OTGFS_GHWCFG3_BCSUPPORT_S  14
/** USB_OTGFS_GHWCFG3_LPMMODE : RO; bitpos: [15]; default: 0;
 *  LPM mode specified for Mode of Operation.
 */
#define USB_OTGFS_GHWCFG3_LPMMODE    (BIT(15))
#define USB_OTGFS_GHWCFG3_LPMMODE_M  (USB_OTGFS_GHWCFG3_LPMMODE_V << USB_OTGFS_GHWCFG3_LPMMODE_S)
#define USB_OTGFS_GHWCFG3_LPMMODE_V  0x00000001U
#define USB_OTGFS_GHWCFG3_LPMMODE_S  15
/** USB_OTGFS_GHWCFG3_DFIFODEPTH : RO; bitpos: [31:16]; default: 200;
 *  DFIFO Depth (DfifoDepth - EP_LOC_CNT)
 *
 *  This value is in terms of 32-bit words.
 *  - Minimum value is 32
 *  - Maximum value is 32,768
 *  Note: This field is configured using the OTG_DFIFO_DEPTH parameter. For more
 *  information on EP_LOC_CNT, see the Endpoint Information Controller (EPINFO_CTL)
 *  section.
 */
#define USB_OTGFS_GHWCFG3_DFIFODEPTH    0x0000FFFFU
#define USB_OTGFS_GHWCFG3_DFIFODEPTH_M  (USB_OTGFS_GHWCFG3_DFIFODEPTH_V << USB_OTGFS_GHWCFG3_DFIFODEPTH_S)
#define USB_OTGFS_GHWCFG3_DFIFODEPTH_V  0x0000FFFFU
#define USB_OTGFS_GHWCFG3_DFIFODEPTH_S  16

/** USB_OTGFS_GHWCFG4_REG register
 *  User Hardware Configuration 4 Register  Note Bit [31] is available only when
 *  Scatter/Gather DMA mode is enabled. When Scatter/Gather DMA mode is disabled, this
 *  field is reserved.
 */
#define USB_OTGFS_GHWCFG4_REG(i) (DR_REG_USB_BASE(i) + 0x50)
/** USB_OTGFS_GHWCFG4_NUMDEVPERIOEPS : RO; bitpos: [3:0]; default: 0;
 *  Number of Device Mode Periodic IN Endpoints (NumDevPerioEps)
 *
 *  Range: 0-15
 */
#define USB_OTGFS_GHWCFG4_NUMDEVPERIOEPS    0x0000000FU
#define USB_OTGFS_GHWCFG4_NUMDEVPERIOEPS_M  (USB_OTGFS_GHWCFG4_NUMDEVPERIOEPS_V << USB_OTGFS_GHWCFG4_NUMDEVPERIOEPS_S)
#define USB_OTGFS_GHWCFG4_NUMDEVPERIOEPS_V  0x0000000FU
#define USB_OTGFS_GHWCFG4_NUMDEVPERIOEPS_S  0
/** USB_OTGFS_GHWCFG4_PARTIALPWRDN : RO; bitpos: [4]; default: 1;
 *  Enable Partial Power Down (PartialPwrDn)
 *  - 1'b0: Partial Power Down Not Enabled
 *  - 1'b1: Partial Power Down Enabled
 */
#define USB_OTGFS_GHWCFG4_PARTIALPWRDN    (BIT(4))
#define USB_OTGFS_GHWCFG4_PARTIALPWRDN_M  (USB_OTGFS_GHWCFG4_PARTIALPWRDN_V << USB_OTGFS_GHWCFG4_PARTIALPWRDN_S)
#define USB_OTGFS_GHWCFG4_PARTIALPWRDN_V  0x00000001U
#define USB_OTGFS_GHWCFG4_PARTIALPWRDN_S  4
/** USB_OTGFS_GHWCFG4_AHBFREQ : RO; bitpos: [5]; default: 1;
 *  Minimum AHB Frequency Less Than 60 MHz (AhbFreq)
 *  - 1'b0: No
 *  - 1'b1: Yes
 */
#define USB_OTGFS_GHWCFG4_AHBFREQ    (BIT(5))
#define USB_OTGFS_GHWCFG4_AHBFREQ_M  (USB_OTGFS_GHWCFG4_AHBFREQ_V << USB_OTGFS_GHWCFG4_AHBFREQ_S)
#define USB_OTGFS_GHWCFG4_AHBFREQ_V  0x00000001U
#define USB_OTGFS_GHWCFG4_AHBFREQ_S  5
/** USB_OTGFS_GHWCFG4_HIBERNATION : RO; bitpos: [6]; default: 0;
 *  Enable Hibernation (Hibernation)
 *  - 1'b0: Hibernation feature not enabled
 *  - 1'b1: Hibernation feature enabled
 */
#define USB_OTGFS_GHWCFG4_HIBERNATION    (BIT(6))
#define USB_OTGFS_GHWCFG4_HIBERNATION_M  (USB_OTGFS_GHWCFG4_HIBERNATION_V << USB_OTGFS_GHWCFG4_HIBERNATION_S)
#define USB_OTGFS_GHWCFG4_HIBERNATION_V  0x00000001U
#define USB_OTGFS_GHWCFG4_HIBERNATION_S  6
/** USB_OTGFS_GHWCFG4_EXTENDEDHIBERNATION : RO; bitpos: [7]; default: 0;
 *  Enable Hibernation
 *  - 1'b0: Extended Hibernation feature not enabled
 *  - 1'b1: Extended Hibernation feature enabled
 */
#define USB_OTGFS_GHWCFG4_EXTENDEDHIBERNATION    (BIT(7))
#define USB_OTGFS_GHWCFG4_EXTENDEDHIBERNATION_M  (USB_OTGFS_GHWCFG4_EXTENDEDHIBERNATION_V << USB_OTGFS_GHWCFG4_EXTENDEDHIBERNATION_S)
#define USB_OTGFS_GHWCFG4_EXTENDEDHIBERNATION_V  0x00000001U
#define USB_OTGFS_GHWCFG4_EXTENDEDHIBERNATION_S  7
/** USB_OTGFS_GHWCFG4_ACGSUPT : RO; bitpos: [12]; default: 0;
 *  Active Clock Gating Support
 *
 *  This bit indicates that the controller supports the Dynamic (Switching) Power
 *  Reduction during periods
 *  when there is no USB and AHB Traffic.
 *  - 1'b0:  Active Clock Gating is not enabled.
 *  - 1'b1:  Active Clock Gating Enabled.
 */
#define USB_OTGFS_GHWCFG4_ACGSUPT    (BIT(12))
#define USB_OTGFS_GHWCFG4_ACGSUPT_M  (USB_OTGFS_GHWCFG4_ACGSUPT_V << USB_OTGFS_GHWCFG4_ACGSUPT_S)
#define USB_OTGFS_GHWCFG4_ACGSUPT_V  0x00000001U
#define USB_OTGFS_GHWCFG4_ACGSUPT_S  12
/** USB_OTGFS_GHWCFG4_ENHANCEDLPMSUPT : RO; bitpos: [13]; default: 1;
 *  Enhanced LPM Support (EnhancedLPMSupt)
 *
 *  This bit indicates that the controller supports the following behavior:
 *  L1 Entry Behavior based on FIFO Status
 *  - TX FIFO
 *  - Accept L1 Request even if ISOC OUT TX FIFO is not empty.
 *  - Reject L1 Request if Non-Periodic TX FIFO is not empty.
 *  - Ensure application can flush the TX FIFO while the Controller is in L1.
 *  - RX FIFO
 *  - Accept L1 Request even if RX FIFO (common to Periodic and Non-Periodic) is not
 *  empty.
 *  - Accept L1 Request but delay SLEEPM assertion until RX SINK Buffer is empty.
 *
 *  Prevent L1 Entry if a Control Transfer is in progress on any Control Endpoint.
 *  Ability to Flush TxFIFO even if PHY Clock is gated.
 *
 */
#define USB_OTGFS_GHWCFG4_ENHANCEDLPMSUPT    (BIT(13))
#define USB_OTGFS_GHWCFG4_ENHANCEDLPMSUPT_M  (USB_OTGFS_GHWCFG4_ENHANCEDLPMSUPT_V << USB_OTGFS_GHWCFG4_ENHANCEDLPMSUPT_S)
#define USB_OTGFS_GHWCFG4_ENHANCEDLPMSUPT_V  0x00000001U
#define USB_OTGFS_GHWCFG4_ENHANCEDLPMSUPT_S  13
/** USB_OTGFS_GHWCFG4_PHYDATAWIDTH : RO; bitpos: [15:14]; default: 2;
 *  UTMI+ PHY/ULPI-to-Internal UTMI+ Wrapper Data Width
 *  (PhyDataWidth)<vr>When a ULPI PHY is used, an internal wrapper converts ULPI to
 *  UTMI+.
 *  - 2'b00: 8 bits
 *  - 2'b01: 16 bits
 *  - 2'b10: 8/16 bits, software selectable
 *  - Others: Reserved
 */
#define USB_OTGFS_GHWCFG4_PHYDATAWIDTH    0x00000003U
#define USB_OTGFS_GHWCFG4_PHYDATAWIDTH_M  (USB_OTGFS_GHWCFG4_PHYDATAWIDTH_V << USB_OTGFS_GHWCFG4_PHYDATAWIDTH_S)
#define USB_OTGFS_GHWCFG4_PHYDATAWIDTH_V  0x00000003U
#define USB_OTGFS_GHWCFG4_PHYDATAWIDTH_S  14
/** USB_OTGFS_GHWCFG4_NUMCTLEPS : RO; bitpos: [19:16]; default: 0;
 *  Number of Device Mode Control Endpoints in Addition to
 *  Endpoint 0 (NumCtlEps)
 *  Range: 0-15
 */
#define USB_OTGFS_GHWCFG4_NUMCTLEPS    0x0000000FU
#define USB_OTGFS_GHWCFG4_NUMCTLEPS_M  (USB_OTGFS_GHWCFG4_NUMCTLEPS_V << USB_OTGFS_GHWCFG4_NUMCTLEPS_S)
#define USB_OTGFS_GHWCFG4_NUMCTLEPS_V  0x0000000FU
#define USB_OTGFS_GHWCFG4_NUMCTLEPS_S  16
/** USB_OTGFS_GHWCFG4_IDDGFLTR : RO; bitpos: [20]; default: 1;
 *  IDDIG Filter Enable (IddgFltr)
 *  - 1'b0: No filter
 *  - 1'b1: Filter
 */
#define USB_OTGFS_GHWCFG4_IDDGFLTR    (BIT(20))
#define USB_OTGFS_GHWCFG4_IDDGFLTR_M  (USB_OTGFS_GHWCFG4_IDDGFLTR_V << USB_OTGFS_GHWCFG4_IDDGFLTR_S)
#define USB_OTGFS_GHWCFG4_IDDGFLTR_V  0x00000001U
#define USB_OTGFS_GHWCFG4_IDDGFLTR_S  20
/** USB_OTGFS_GHWCFG4_VBUSVALIDFLTR : RO; bitpos: [21]; default: 1;
 *  VBUS Valid Filter Enabled (VBusValidFltr)
 *  - 1'b0: No filter
 *  - 1'b1: Filter
 */
#define USB_OTGFS_GHWCFG4_VBUSVALIDFLTR    (BIT(21))
#define USB_OTGFS_GHWCFG4_VBUSVALIDFLTR_M  (USB_OTGFS_GHWCFG4_VBUSVALIDFLTR_V << USB_OTGFS_GHWCFG4_VBUSVALIDFLTR_S)
#define USB_OTGFS_GHWCFG4_VBUSVALIDFLTR_V  0x00000001U
#define USB_OTGFS_GHWCFG4_VBUSVALIDFLTR_S  21
/** USB_OTGFS_GHWCFG4_AVALIDFLTR : RO; bitpos: [22]; default: 1;
 *  a_valid Filter Enabled (AValidFltr)
 *  - 1'b0: No filter
 *  - 1'b1: Filter
 */
#define USB_OTGFS_GHWCFG4_AVALIDFLTR    (BIT(22))
#define USB_OTGFS_GHWCFG4_AVALIDFLTR_M  (USB_OTGFS_GHWCFG4_AVALIDFLTR_V << USB_OTGFS_GHWCFG4_AVALIDFLTR_S)
#define USB_OTGFS_GHWCFG4_AVALIDFLTR_V  0x00000001U
#define USB_OTGFS_GHWCFG4_AVALIDFLTR_S  22
/** USB_OTGFS_GHWCFG4_BVALIDFLTR : RO; bitpos: [23]; default: 1;
 *  b_valid Filter Enabled (BValidFltr)
 *  - 1'b0: No filter
 *  - 1'b1: Filter
 */
#define USB_OTGFS_GHWCFG4_BVALIDFLTR    (BIT(23))
#define USB_OTGFS_GHWCFG4_BVALIDFLTR_M  (USB_OTGFS_GHWCFG4_BVALIDFLTR_V << USB_OTGFS_GHWCFG4_BVALIDFLTR_S)
#define USB_OTGFS_GHWCFG4_BVALIDFLTR_V  0x00000001U
#define USB_OTGFS_GHWCFG4_BVALIDFLTR_S  23
/** USB_OTGFS_GHWCFG4_SESSENDFLTR : RO; bitpos: [24]; default: 1;
 *  session_end Filter Enabled (SessEndFltr)
 *  - 1'b0: No filter
 *  - 1'b1: Filter
 */
#define USB_OTGFS_GHWCFG4_SESSENDFLTR    (BIT(24))
#define USB_OTGFS_GHWCFG4_SESSENDFLTR_M  (USB_OTGFS_GHWCFG4_SESSENDFLTR_V << USB_OTGFS_GHWCFG4_SESSENDFLTR_S)
#define USB_OTGFS_GHWCFG4_SESSENDFLTR_V  0x00000001U
#define USB_OTGFS_GHWCFG4_SESSENDFLTR_S  24
/** USB_OTGFS_GHWCFG4_DEDFIFOMODE : RO; bitpos: [25]; default: 1;
 *  Enable Dedicated Transmit FIFO for device IN Endpoints
 *  (DedFifoMode)
 *  - 1'b0 : Dedicated Transmit FIFO Operation not enabled.
 *  - 1'b1 : Dedicated Transmit FIFO Operation enabled.
 */
#define USB_OTGFS_GHWCFG4_DEDFIFOMODE    (BIT(25))
#define USB_OTGFS_GHWCFG4_DEDFIFOMODE_M  (USB_OTGFS_GHWCFG4_DEDFIFOMODE_V << USB_OTGFS_GHWCFG4_DEDFIFOMODE_S)
#define USB_OTGFS_GHWCFG4_DEDFIFOMODE_V  0x00000001U
#define USB_OTGFS_GHWCFG4_DEDFIFOMODE_S  25
/** USB_OTGFS_GHWCFG4_INEPS : RO; bitpos: [29:26]; default: 4;
 *  Number of Device Mode IN Endpoints Including Control Endpoints (INEps)
 *  - 0: 1 IN Endpoint
 *  - 1: 2 IN Endpoints
 *  ....
 *  - 15: 16 IN Endpoints
 */
#define USB_OTGFS_GHWCFG4_INEPS    0x0000000FU
#define USB_OTGFS_GHWCFG4_INEPS_M  (USB_OTGFS_GHWCFG4_INEPS_V << USB_OTGFS_GHWCFG4_INEPS_S)
#define USB_OTGFS_GHWCFG4_INEPS_V  0x0000000FU
#define USB_OTGFS_GHWCFG4_INEPS_S  26
/** USB_OTGFS_GHWCFG4_DESCDMAENABLED : RO; bitpos: [30]; default: 1;
 *  Scatter/Gather DMA configuration
 *  - 1'b0: Non-Scatter/Gather DMA configuration
 *  - 1'b1: Scatter/Gather DMA configuration
 */
#define USB_OTGFS_GHWCFG4_DESCDMAENABLED    (BIT(30))
#define USB_OTGFS_GHWCFG4_DESCDMAENABLED_M  (USB_OTGFS_GHWCFG4_DESCDMAENABLED_V << USB_OTGFS_GHWCFG4_DESCDMAENABLED_S)
#define USB_OTGFS_GHWCFG4_DESCDMAENABLED_V  0x00000001U
#define USB_OTGFS_GHWCFG4_DESCDMAENABLED_S  30
/** USB_OTGFS_GHWCFG4_DESCDMA : RO; bitpos: [31]; default: 1;
 *  Scatter/Gather DMA configuration
 *  - 1'b0: Non Dynamic configuration
 *  - 1'b1: Dynamic configuration
 *  Note: This field is configured using the OTG_EN_DESC_DMA parameter.
 */
#define USB_OTGFS_GHWCFG4_DESCDMA    (BIT(31))
#define USB_OTGFS_GHWCFG4_DESCDMA_M  (USB_OTGFS_GHWCFG4_DESCDMA_V << USB_OTGFS_GHWCFG4_DESCDMA_S)
#define USB_OTGFS_GHWCFG4_DESCDMA_V  0x00000001U
#define USB_OTGFS_GHWCFG4_DESCDMA_S  31

/** USB_OTGFS_GDFIFOCFG_REG register
 *  Global DFIFO Configuration Register
 */
#define USB_OTGFS_GDFIFOCFG_REG(i) (DR_REG_USB_BASE(i) + 0x5c)
/** USB_OTGFS_GDFIFOCFG_GDFIFOCFG : R/W; bitpos: [15:0]; default: 256;
 *  GDFIFOCfg
 *
 *  This field is for dynamic programming of the DFIFO Size. This value takes effect
 *  only when the application programs a non zero value to this register. The value
 *  programmed must conform to the guidelines described in 'FIFO RAM Allocation'. The
 *  core does not have any corrective logic if the FIFO sizes are programmed
 *  incorrectly.
 */
#define USB_OTGFS_GDFIFOCFG_GDFIFOCFG    0x0000FFFFU
#define USB_OTGFS_GDFIFOCFG_GDFIFOCFG_M  (USB_OTGFS_GDFIFOCFG_GDFIFOCFG_V << USB_OTGFS_GDFIFOCFG_GDFIFOCFG_S)
#define USB_OTGFS_GDFIFOCFG_GDFIFOCFG_V  0x0000FFFFU
#define USB_OTGFS_GDFIFOCFG_GDFIFOCFG_S  0
/** USB_OTGFS_GDFIFOCFG_EPINFOBASEADDR : R/W; bitpos: [31:16]; default: 200;
 *  EPInfoBaseAddr
 *
 *  This field provides the start address of the EP info controller.
 */
#define USB_OTGFS_GDFIFOCFG_EPINFOBASEADDR    0x0000FFFFU
#define USB_OTGFS_GDFIFOCFG_EPINFOBASEADDR_M  (USB_OTGFS_GDFIFOCFG_EPINFOBASEADDR_V << USB_OTGFS_GDFIFOCFG_EPINFOBASEADDR_S)
#define USB_OTGFS_GDFIFOCFG_EPINFOBASEADDR_V  0x0000FFFFU
#define USB_OTGFS_GDFIFOCFG_EPINFOBASEADDR_S  16

/** USB_OTGFS_HPTXFSIZ_REG register
 *  Host Periodic Transmit FIFO Size Register  This register holds the size and the
 *  memory start address of the Periodic TxFIFO.  Note: Read the reset value of this
 *  register only after the following conditions:  - If IDDIG_FILTER is disabled, read
 *  only after PHY clock is stable.  - If IDDIG_FILTER is enabled, read only after the
 *  filter timer expires.
 */
#define USB_OTGFS_HPTXFSIZ_REG(i) (DR_REG_USB_BASE(i) + 0x100)
/** USB_OTGFS_HPTXFSIZ_PTXFSTADDR : R/W; bitpos: [9:0]; default: 512;
 *  Host Periodic TxFIFO Start Address (PTxFStAddr)
 *
 *  The power-on reset value of this register is the sum of the Largest Rx Data FIFO
 *  Depth and Largest Non-periodic Tx Data FIFO Depth.These parameters are:
 *
 *  In shared FIFO operation:
 *  - OTG_RX_DFIFO_DEPTH + OTG_TX_NPERIO_DFIFO_DEPTH
 *
 *  In dedicated FIFO mode:
 *  - OTG_RX_DFIFO_DEPTH + OTG_TX_HNPERIO_DFIFO_DEPTH If Enable Dynamic FIFO Sizing?
 *  was deselected in coreConsultant (parameter OTG_DFIFO_DYNAMIC = 0), these flops are
 *  optimized, and reads return the power-on value. If Enable Dynamic FIFO Sizing? was
 *  selected in coreConsultant (parameter OTG_DFIFO_DYNAMIC = 1), you can write a new
 *  value in this field.
 *
 *  Programmed values must not exceed the power-on value set in coreConsultant.
 */
#define USB_OTGFS_HPTXFSIZ_PTXFSTADDR    0x000003FFU
#define USB_OTGFS_HPTXFSIZ_PTXFSTADDR_M  (USB_OTGFS_HPTXFSIZ_PTXFSTADDR_V << USB_OTGFS_HPTXFSIZ_PTXFSTADDR_S)
#define USB_OTGFS_HPTXFSIZ_PTXFSTADDR_V  0x000003FFU
#define USB_OTGFS_HPTXFSIZ_PTXFSTADDR_S  0
/** USB_OTGFS_HPTXFSIZ_PTXFSIZE : R/W; bitpos: [24:16]; default: 256;
 *  Host Periodic TxFIFO Depth (PTxFSize)
 *
 *  This value is in terms of 32-bit words.
 *  - Minimum value is 16
 *  - Maximum value is 32,768
 *  The power-on reset value of this register is specified as the Largest Host Mode
 *  Periodic Tx Data FIFO Depth.
 *  - If Enable Dynamic FIFO Sizing? was deselected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 0), these flops are optimized, and reads return the power-on
 *  value.
 *  - If Enable Dynamic FIFO Sizing? was selected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 1), you can write a new value in this field.
 *  Programmed values must not exceed the power-on value set in coreConsultant.
 */
#define USB_OTGFS_HPTXFSIZ_PTXFSIZE    0x000001FFU
#define USB_OTGFS_HPTXFSIZ_PTXFSIZE_M  (USB_OTGFS_HPTXFSIZ_PTXFSIZE_V << USB_OTGFS_HPTXFSIZ_PTXFSIZE_S)
#define USB_OTGFS_HPTXFSIZ_PTXFSIZE_V  0x000001FFU
#define USB_OTGFS_HPTXFSIZ_PTXFSIZE_S  16

/** USB_OTGFS_DIEPTXF1_REG register
 *  Device IN Endpoint Transmit FIFO Size Register $i This register is valid only in
 *  dedicated FIFO mode (OTG_EN_DED_TX_FIFO=1). It holds the size and memory start
 *  address of IN endpoint TxFIFOs implemented in Device mode. Each FIFO holds the data
 *  for one IN endpoint. This register is repeated for instantiated IN endpoint FIFOs 1
 *  to 15. For IN endpoint FIFO 0, use GNPTXFSIZ register for programming the size and
 *  memory start address.
 */
#define USB_OTGFS_DIEPTXF1_REG(i) (DR_REG_USB_BASE(i) + 0x104)
/** USB_OTGFS_DIEPTXF1_INEPNTXFSTADDR : R/W; bitpos: [9:0]; default: 512;
 *  IN Endpoint FIFOn Transmit RAM Start Address (INEPnTxFStAddr)
 *
 *  This field contains the memory start address for IN endpoint Transmit FIFOn (0<n< =
 *  15).
 *  The power-on reset value of this register is specified as the Largest Rx Data FIFO
 *  Depth. The power-on reset value of this register is calculated according to the
 *  following formula: OTG_RX_DFIFO_DEPTH + SUM of OTG_TX_DINEP_DFIFO_DEPTH_'i' (where
 *  x = 0 to n  1) If at POR the calculated value (C) exceeds 65535, then the Reset
 *  value becomes Reset Value(A) = (C 65536). Example: If start address of IN endpoint
 *  FIFO 1 is OTG_RX_DFIFO_DEPTH + OTG_TX_DINEP_DFIFO_DEPTH_ 0 and start address of IN
 *  endpoint FIFO 2 is OTG_RX_DFIFO_DEPTH + OTG_TX_DINEP_DFIFO_DEPTH_ 0 +
 *  OTG_TX_DINEP_DFIFO_DEPTH_ 1.
 *  - If Enable Dynamic FIFO Sizing is deselected in coreConsultant (OTG_DFIFO_DYNAMIC
 *  = 0), this field is read-only and read value is the power-on reset value.
 *  - If Enable Dynamic FIFO Sizing is selected in coreConsultant (OTG_DFIFO_DYNAMIC =
 *  1), and you have calculated or programmed a new value for RxFIFO depth or TX FIFO
 *  depths, you can program their values according to the above formula. Programmed
 *  values must not exceed the power-on value set in coreConsultant.
 */
#define USB_OTGFS_DIEPTXF1_INEPNTXFSTADDR    0x000003FFU
#define USB_OTGFS_DIEPTXF1_INEPNTXFSTADDR_M  (USB_OTGFS_DIEPTXF1_INEPNTXFSTADDR_V << USB_OTGFS_DIEPTXF1_INEPNTXFSTADDR_S)
#define USB_OTGFS_DIEPTXF1_INEPNTXFSTADDR_V  0x000003FFU
#define USB_OTGFS_DIEPTXF1_INEPNTXFSTADDR_S  0
/** USB_OTGFS_DIEPTXF1_INEPNTXFDEP : R/W; bitpos: [24:16]; default: 256;
 *  IN Endpoint TxFIFO Depth (INEPnTxFDep)
 *
 *  This value is in terms of 32-bit words.
 *  - Minimum value is 16
 *  - Maximum value is 32,768
 *  The power-on reset value of this register is specified as the Largest IN Endpoint
 *  FIFO number Depth (parameter OTG_TX_DINEP_DFIFO_DEPTH_n) during coreConsultant
 *  configuration (0 < i <= 15).
 *  - If Enable Dynamic FIFO Sizing? was deselected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 0), these flops are optimized, and reads return the power-on
 *  value.
 *  - If Enable Dynamic FIFO Sizing? was selected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 1), you can write a new value in this field. .
 *  Programmed values must not exceed the power-on value
 */
#define USB_OTGFS_DIEPTXF1_INEPNTXFDEP    0x000001FFU
#define USB_OTGFS_DIEPTXF1_INEPNTXFDEP_M  (USB_OTGFS_DIEPTXF1_INEPNTXFDEP_V << USB_OTGFS_DIEPTXF1_INEPNTXFDEP_S)
#define USB_OTGFS_DIEPTXF1_INEPNTXFDEP_V  0x000001FFU
#define USB_OTGFS_DIEPTXF1_INEPNTXFDEP_S  16

/** USB_OTGFS_DIEPTXF2_REG register
 *  Device IN Endpoint Transmit FIFO Size Register $i This register is valid only in
 *  dedicated FIFO mode (OTG_EN_DED_TX_FIFO=1). It holds the size and memory start
 *  address of IN endpoint TxFIFOs implemented in Device mode. Each FIFO holds the data
 *  for one IN endpoint. This register is repeated for instantiated IN endpoint FIFOs 1
 *  to 15. For IN endpoint FIFO 0, use GNPTXFSIZ register for programming the size and
 *  memory start address.
 */
#define USB_OTGFS_DIEPTXF2_REG(i) (DR_REG_USB_BASE(i) + 0x108)
/** USB_OTGFS_DIEPTXF2_INEPNTXFSTADDR : R/W; bitpos: [9:0]; default: 768;
 *  IN Endpoint FIFOn Transmit RAM Start Address (INEPnTxFStAddr)
 *
 *  This field contains the memory start address for IN endpoint Transmit FIFOn (0<n< =
 *  15).
 *  The power-on reset value of this register is specified as the Largest Rx Data FIFO
 *  Depth. The power-on reset value of this register is calculated according to the
 *  following formula: OTG_RX_DFIFO_DEPTH + SUM of OTG_TX_DINEP_DFIFO_DEPTH_'i' (where
 *  x = 0 to n  1) If at POR the calculated value (C) exceeds 65535, then the Reset
 *  value becomes Reset Value(A) = (C 65536). Example: If start address of IN endpoint
 *  FIFO 1 is OTG_RX_DFIFO_DEPTH + OTG_TX_DINEP_DFIFO_DEPTH_ 0 and start address of IN
 *  endpoint FIFO 2 is OTG_RX_DFIFO_DEPTH + OTG_TX_DINEP_DFIFO_DEPTH_ 0 +
 *  OTG_TX_DINEP_DFIFO_DEPTH_ 1.
 *  - If Enable Dynamic FIFO Sizing is deselected in coreConsultant (OTG_DFIFO_DYNAMIC
 *  = 0), this field is read-only and read value is the power-on reset value.
 *  - If Enable Dynamic FIFO Sizing is selected in coreConsultant (OTG_DFIFO_DYNAMIC =
 *  1), and you have calculated or programmed a new value for RxFIFO depth or TX FIFO
 *  depths, you can program their values according to the above formula. Programmed
 *  values must not exceed the power-on value set in coreConsultant.
 */
#define USB_OTGFS_DIEPTXF2_INEPNTXFSTADDR    0x000003FFU
#define USB_OTGFS_DIEPTXF2_INEPNTXFSTADDR_M  (USB_OTGFS_DIEPTXF2_INEPNTXFSTADDR_V << USB_OTGFS_DIEPTXF2_INEPNTXFSTADDR_S)
#define USB_OTGFS_DIEPTXF2_INEPNTXFSTADDR_V  0x000003FFU
#define USB_OTGFS_DIEPTXF2_INEPNTXFSTADDR_S  0
/** USB_OTGFS_DIEPTXF2_INEPNTXFDEP : R/W; bitpos: [24:16]; default: 256;
 *  IN Endpoint TxFIFO Depth (INEPnTxFDep)
 *
 *  This value is in terms of 32-bit words.
 *  - Minimum value is 16
 *  - Maximum value is 32,768
 *  The power-on reset value of this register is specified as the Largest IN Endpoint
 *  FIFO number Depth (parameter OTG_TX_DINEP_DFIFO_DEPTH_n) during coreConsultant
 *  configuration (0 < i <= 15).
 *  - If Enable Dynamic FIFO Sizing? was deselected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 0), these flops are optimized, and reads return the power-on
 *  value.
 *  - If Enable Dynamic FIFO Sizing? was selected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 1), you can write a new value in this field. .
 *  Programmed values must not exceed the power-on value
 */
#define USB_OTGFS_DIEPTXF2_INEPNTXFDEP    0x000001FFU
#define USB_OTGFS_DIEPTXF2_INEPNTXFDEP_M  (USB_OTGFS_DIEPTXF2_INEPNTXFDEP_V << USB_OTGFS_DIEPTXF2_INEPNTXFDEP_S)
#define USB_OTGFS_DIEPTXF2_INEPNTXFDEP_V  0x000001FFU
#define USB_OTGFS_DIEPTXF2_INEPNTXFDEP_S  16

/** USB_OTGFS_DIEPTXF3_REG register
 *  Device IN Endpoint Transmit FIFO Size Register $i This register is valid only in
 *  dedicated FIFO mode (OTG_EN_DED_TX_FIFO=1). It holds the size and memory start
 *  address of IN endpoint TxFIFOs implemented in Device mode. Each FIFO holds the data
 *  for one IN endpoint. This register is repeated for instantiated IN endpoint FIFOs 1
 *  to 15. For IN endpoint FIFO 0, use GNPTXFSIZ register for programming the size and
 *  memory start address.
 */
#define USB_OTGFS_DIEPTXF3_REG(i) (DR_REG_USB_BASE(i) + 0x10c)
/** USB_OTGFS_DIEPTXF3_INEPNTXFSTADDR : R/W; bitpos: [10:0]; default: 1024;
 *  IN Endpoint FIFOn Transmit RAM Start Address (INEPnTxFStAddr)
 *
 *  This field contains the memory start address for IN endpoint Transmit FIFOn (0<n< =
 *  15).
 *  The power-on reset value of this register is specified as the Largest Rx Data FIFO
 *  Depth. The power-on reset value of this register is calculated according to the
 *  following formula: OTG_RX_DFIFO_DEPTH + SUM of OTG_TX_DINEP_DFIFO_DEPTH_'i' (where
 *  x = 0 to n  1) If at POR the calculated value (C) exceeds 65535, then the Reset
 *  value becomes Reset Value(A) = (C 65536). Example: If start address of IN endpoint
 *  FIFO 1 is OTG_RX_DFIFO_DEPTH + OTG_TX_DINEP_DFIFO_DEPTH_ 0 and start address of IN
 *  endpoint FIFO 2 is OTG_RX_DFIFO_DEPTH + OTG_TX_DINEP_DFIFO_DEPTH_ 0 +
 *  OTG_TX_DINEP_DFIFO_DEPTH_ 1.
 *  - If Enable Dynamic FIFO Sizing is deselected in coreConsultant (OTG_DFIFO_DYNAMIC
 *  = 0), this field is read-only and read value is the power-on reset value.
 *  - If Enable Dynamic FIFO Sizing is selected in coreConsultant (OTG_DFIFO_DYNAMIC =
 *  1), and you have calculated or programmed a new value for RxFIFO depth or TX FIFO
 *  depths, you can program their values according to the above formula. Programmed
 *  values must not exceed the power-on value set in coreConsultant.
 */
#define USB_OTGFS_DIEPTXF3_INEPNTXFSTADDR    0x000007FFU
#define USB_OTGFS_DIEPTXF3_INEPNTXFSTADDR_M  (USB_OTGFS_DIEPTXF3_INEPNTXFSTADDR_V << USB_OTGFS_DIEPTXF3_INEPNTXFSTADDR_S)
#define USB_OTGFS_DIEPTXF3_INEPNTXFSTADDR_V  0x000007FFU
#define USB_OTGFS_DIEPTXF3_INEPNTXFSTADDR_S  0
/** USB_OTGFS_DIEPTXF3_INEPNTXFDEP : R/W; bitpos: [24:16]; default: 256;
 *  IN Endpoint TxFIFO Depth (INEPnTxFDep)
 *
 *  This value is in terms of 32-bit words.
 *  - Minimum value is 16
 *  - Maximum value is 32,768
 *  The power-on reset value of this register is specified as the Largest IN Endpoint
 *  FIFO number Depth (parameter OTG_TX_DINEP_DFIFO_DEPTH_n) during coreConsultant
 *  configuration (0 < i <= 15).
 *  - If Enable Dynamic FIFO Sizing? was deselected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 0), these flops are optimized, and reads return the power-on
 *  value.
 *  - If Enable Dynamic FIFO Sizing? was selected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 1), you can write a new value in this field. .
 *  Programmed values must not exceed the power-on value
 */
#define USB_OTGFS_DIEPTXF3_INEPNTXFDEP    0x000001FFU
#define USB_OTGFS_DIEPTXF3_INEPNTXFDEP_M  (USB_OTGFS_DIEPTXF3_INEPNTXFDEP_V << USB_OTGFS_DIEPTXF3_INEPNTXFDEP_S)
#define USB_OTGFS_DIEPTXF3_INEPNTXFDEP_V  0x000001FFU
#define USB_OTGFS_DIEPTXF3_INEPNTXFDEP_S  16

/** USB_OTGFS_DIEPTXF4_REG register
 *  Device IN Endpoint Transmit FIFO Size Register $i This register is valid only in
 *  dedicated FIFO mode (OTG_EN_DED_TX_FIFO=1). It holds the size and memory start
 *  address of IN endpoint TxFIFOs implemented in Device mode. Each FIFO holds the data
 *  for one IN endpoint. This register is repeated for instantiated IN endpoint FIFOs 1
 *  to 15. For IN endpoint FIFO 0, use GNPTXFSIZ register for programming the size and
 *  memory start address.
 */
#define USB_OTGFS_DIEPTXF4_REG(i) (DR_REG_USB_BASE(i) + 0x110)
/** USB_OTGFS_DIEPTXF4_INEPNTXFSTADDR : R/W; bitpos: [10:0]; default: 1280;
 *  IN Endpoint FIFOn Transmit RAM Start Address (INEPnTxFStAddr)
 *
 *  This field contains the memory start address for IN endpoint Transmit FIFOn (0<n< =
 *  15).
 *  The power-on reset value of this register is specified as the Largest Rx Data FIFO
 *  Depth. The power-on reset value of this register is calculated according to the
 *  following formula: OTG_RX_DFIFO_DEPTH + SUM of OTG_TX_DINEP_DFIFO_DEPTH_'i' (where
 *  x = 0 to n  1) If at POR the calculated value (C) exceeds 65535, then the Reset
 *  value becomes Reset Value(A) = (C 65536). Example: If start address of IN endpoint
 *  FIFO 1 is OTG_RX_DFIFO_DEPTH + OTG_TX_DINEP_DFIFO_DEPTH_ 0 and start address of IN
 *  endpoint FIFO 2 is OTG_RX_DFIFO_DEPTH + OTG_TX_DINEP_DFIFO_DEPTH_ 0 +
 *  OTG_TX_DINEP_DFIFO_DEPTH_ 1.
 *  - If Enable Dynamic FIFO Sizing is deselected in coreConsultant (OTG_DFIFO_DYNAMIC
 *  = 0), this field is read-only and read value is the power-on reset value.
 *  - If Enable Dynamic FIFO Sizing is selected in coreConsultant (OTG_DFIFO_DYNAMIC =
 *  1), and you have calculated or programmed a new value for RxFIFO depth or TX FIFO
 *  depths, you can program their values according to the above formula. Programmed
 *  values must not exceed the power-on value set in coreConsultant.
 */
#define USB_OTGFS_DIEPTXF4_INEPNTXFSTADDR    0x000007FFU
#define USB_OTGFS_DIEPTXF4_INEPNTXFSTADDR_M  (USB_OTGFS_DIEPTXF4_INEPNTXFSTADDR_V << USB_OTGFS_DIEPTXF4_INEPNTXFSTADDR_S)
#define USB_OTGFS_DIEPTXF4_INEPNTXFSTADDR_V  0x000007FFU
#define USB_OTGFS_DIEPTXF4_INEPNTXFSTADDR_S  0
/** USB_OTGFS_DIEPTXF4_INEPNTXFDEP : R/W; bitpos: [24:16]; default: 256;
 *  IN Endpoint TxFIFO Depth (INEPnTxFDep)
 *
 *  This value is in terms of 32-bit words.
 *  - Minimum value is 16
 *  - Maximum value is 32,768
 *  The power-on reset value of this register is specified as the Largest IN Endpoint
 *  FIFO number Depth (parameter OTG_TX_DINEP_DFIFO_DEPTH_n) during coreConsultant
 *  configuration (0 < i <= 15).
 *  - If Enable Dynamic FIFO Sizing? was deselected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 0), these flops are optimized, and reads return the power-on
 *  value.
 *  - If Enable Dynamic FIFO Sizing? was selected in coreConsultant (parameter
 *  OTG_DFIFO_DYNAMIC = 1), you can write a new value in this field. .
 *  Programmed values must not exceed the power-on value
 */
#define USB_OTGFS_DIEPTXF4_INEPNTXFDEP    0x000001FFU
#define USB_OTGFS_DIEPTXF4_INEPNTXFDEP_M  (USB_OTGFS_DIEPTXF4_INEPNTXFDEP_V << USB_OTGFS_DIEPTXF4_INEPNTXFDEP_S)
#define USB_OTGFS_DIEPTXF4_INEPNTXFDEP_V  0x000001FFU
#define USB_OTGFS_DIEPTXF4_INEPNTXFDEP_S  16

/** USB_OTGFS_HCFG_REG register
 *  Host Configuration Register
 */
#define USB_OTGFS_HCFG_REG(i) (DR_REG_USB_BASE(i) + 0x400)
/** USB_OTGFS_HCFG_FSLSPCLKSEL : R/W; bitpos: [1:0]; default: 0;
 *  FS/LS PHY Clock Select (FSLSPclkSel)
 *
 *  When the core is in FS Host mode
 *  - 2'b00: PHY clock is running at 30/60 MHz
 *  - 2'b01: PHY clock is running at 48 MHz
 *  - Others: Reserved
 *  When the core is in LS Host mode
 *  - 2'b00: PHY clock is running at 30/60 MHz. When the UTMI+/ULPI PHY Low Power mode
 *  is not selected, use 30/60 MHz.
 *  - 2'b01: PHY clock is running at 48 MHz. When the UTMI+ PHY Low Power mode is
 *  selected, use 48MHz If the PHY supplies a 48 MHz clock during LS mode.
 *  - 2'b10: PHY clock is running at 6 MHz. In USB 1.1 FS mode, use 6 MHz when the
 *  UTMI+ PHY Low Power mode is selected and the PHY supplies a 6 MHz clock during LS
 *  mode. If you select a 6 MHz clock during LS mode, you must do a soft reset.
 *  - 2'b11: Reserved
 *  Notes:
 *  - When Core in FS mode, the internal and external clocks have the same frequency.
 *  - When Core in LS mode,
 *  -- If FSLSPclkSel = 2'b00: Internal and external clocks have the same frequency
 *  -- If FSLSPclkSel = 2'b10: Internal clock is the divided by eight version of
 *  external 48 MHz clock
 */
#define USB_OTGFS_HCFG_FSLSPCLKSEL    0x00000003U
#define USB_OTGFS_HCFG_FSLSPCLKSEL_M  (USB_OTGFS_HCFG_FSLSPCLKSEL_V << USB_OTGFS_HCFG_FSLSPCLKSEL_S)
#define USB_OTGFS_HCFG_FSLSPCLKSEL_V  0x00000003U
#define USB_OTGFS_HCFG_FSLSPCLKSEL_S  0
/** USB_OTGFS_HCFG_FSLSSUPP : R/W; bitpos: [2]; default: 0;
 *  FS- and LS-Only Support (FSLSSupp)
 *
 *  The application uses this bit to control the core's enumeration speed. Using this
 *  bit, the application can make the core
 *  enumerate as a FS host, even If the connected device supports HS traffic. Do not
 *  make changes to this field after initial
 *  programming.
 *  - 1'b0: HS/FS/LS, based on the maximum speed supported by the connected device
 *  - 1'b1: FS/LS-only, even If the connected device can support HS
 */
#define USB_OTGFS_HCFG_FSLSSUPP    (BIT(2))
#define USB_OTGFS_HCFG_FSLSSUPP_M  (USB_OTGFS_HCFG_FSLSSUPP_V << USB_OTGFS_HCFG_FSLSSUPP_S)
#define USB_OTGFS_HCFG_FSLSSUPP_V  0x00000001U
#define USB_OTGFS_HCFG_FSLSSUPP_S  2
/** USB_OTGFS_HCFG_ENA32KHZS : R/W; bitpos: [7]; default: 0;
 *  Enable 32 KHz Suspend mode (Ena32KHzS)
 *
 *  This bit can be set only in FS PHY interface is selected.
 *  Else, this bit needs to be set to zero.
 *  When FS PHY interface is chosen and this bit is set,
 *  the core expects that the PHY clock during Suspend is switched
 *  from 48 MHz to 32 KHz.
 */
#define USB_OTGFS_HCFG_ENA32KHZS    (BIT(7))
#define USB_OTGFS_HCFG_ENA32KHZS_M  (USB_OTGFS_HCFG_ENA32KHZS_V << USB_OTGFS_HCFG_ENA32KHZS_S)
#define USB_OTGFS_HCFG_ENA32KHZS_V  0x00000001U
#define USB_OTGFS_HCFG_ENA32KHZS_S  7
/** USB_OTGFS_HCFG_RESVALID : R/W; bitpos: [15:8]; default: 2;
 *  Resume Validation Period  (ResValid)
 *
 *  This field is effective only when HCFG.Ena32KHzS is set.
 *  It will control the resume period when the core resumes from suspend.
 *  The core counts for 'ResValid' number of clock cycles to detect a
 *  valid resume when this is set.
 */
#define USB_OTGFS_HCFG_RESVALID    0x000000FFU
#define USB_OTGFS_HCFG_RESVALID_M  (USB_OTGFS_HCFG_RESVALID_V << USB_OTGFS_HCFG_RESVALID_S)
#define USB_OTGFS_HCFG_RESVALID_V  0x000000FFU
#define USB_OTGFS_HCFG_RESVALID_S  8
/** USB_OTGFS_HCFG_DESCDMA : R/W; bitpos: [23]; default: 0;
 *  Enable Scatter/gather DMA in Host mode (DescDMA)
 *
 *  When the Scatter/Gather DMA option selected during configuration of the RTL, the
 *  application can set this bit during initialization
 *  to enable the Scatter/Gather DMA operation.
 *
 *  Note: This bit must be modified only once after a reset.
 *
 *  The following combinations are available for programming:
 *  - GAHBCFG.DMAEn=0,HCFG.DescDMA=0 => Slave mode
 *  - GAHBCFG.DMAEn=0,HCFG.DescDMA=1 => Invalid
 *  - GAHBCFG.DMAEn=1,HCFG.DescDMA=0 => Buffered DMA mode
 *  - GAHBCFG.DMAEn=1,HCFG.DescDMA=1 => Scatter/Gather DMA mode
 */
#define USB_OTGFS_HCFG_DESCDMA    (BIT(23))
#define USB_OTGFS_HCFG_DESCDMA_M  (USB_OTGFS_HCFG_DESCDMA_V << USB_OTGFS_HCFG_DESCDMA_S)
#define USB_OTGFS_HCFG_DESCDMA_V  0x00000001U
#define USB_OTGFS_HCFG_DESCDMA_S  23
/** USB_OTGFS_HCFG_FRLISTEN : R/W; bitpos: [25:24]; default: 0;
 *  Frame List Entries(FrListEn)
 *
 *  The value in the register specifies the number of entries in the Frame list.
 *  This field is valid only in Scatter/Gather DMA mode.
 *  - 2'b00: 8 Entries
 *  - 2'b01: 16 Entries
 *  - 2'b10: 32 Entries
 *  - 2'b11: 64 Entries
 */
#define USB_OTGFS_HCFG_FRLISTEN    0x00000003U
#define USB_OTGFS_HCFG_FRLISTEN_M  (USB_OTGFS_HCFG_FRLISTEN_V << USB_OTGFS_HCFG_FRLISTEN_S)
#define USB_OTGFS_HCFG_FRLISTEN_V  0x00000003U
#define USB_OTGFS_HCFG_FRLISTEN_S  24
/** USB_OTGFS_HCFG_PERSCHEDENA : R/W; bitpos: [26]; default: 0;
 *  Enable Periodic Scheduling (PerSchedEna):
 *
 *  Applicable in host DDMA mode only.
 *  Enables periodic scheduling within the core. Initially, the bit is reset.
 *  The core will not process any periodic channels.
 *
 *  As soon as this bit is set,
 *  the core will get ready to start scheduling periodic channels and
 *  sets HCFG.PerSchedStat. The setting of HCFG.PerSchedStat indicates the core
 *  has enabled periodic scheduling. Once HCFG.PerSchedEna is set,
 *  the application is not supposed to again reset the bit unless HCFG.PerSchedStat
 *  is set.
 *
 *  As soon as this bit is reset, the core will get ready to
 *  stop scheduling periodic channels and resets HCFG.PerSchedStat.
 */
#define USB_OTGFS_HCFG_PERSCHEDENA    (BIT(26))
#define USB_OTGFS_HCFG_PERSCHEDENA_M  (USB_OTGFS_HCFG_PERSCHEDENA_V << USB_OTGFS_HCFG_PERSCHEDENA_S)
#define USB_OTGFS_HCFG_PERSCHEDENA_V  0x00000001U
#define USB_OTGFS_HCFG_PERSCHEDENA_S  26
/** USB_OTGFS_HCFG_MODECHTIMEN : R/W; bitpos: [31]; default: 0;
 *  Mode Change Ready Timer Enable (ModeChTimEn)
 *
 *  This bit is used to enable/disable the Host core to wait  200 PHY clock cycles at
 *  the end of Resume to change the opmode signal to the PHY to 00
 *  after Suspend or LPM.
 *  - 1'b0 : The Host core waits for either 200 PHY clock cycles or a linestate of SE0
 *  at the end of resume to the change the opmode from 2'b10 to 2'b00
 *  - 1'b1 : The Host core waits only for a linstate of SE0 at the end of resume to
 *  change the opmode from 2'b10 to 2'b00.
 */
#define USB_OTGFS_HCFG_MODECHTIMEN    (BIT(31))
#define USB_OTGFS_HCFG_MODECHTIMEN_M  (USB_OTGFS_HCFG_MODECHTIMEN_V << USB_OTGFS_HCFG_MODECHTIMEN_S)
#define USB_OTGFS_HCFG_MODECHTIMEN_V  0x00000001U
#define USB_OTGFS_HCFG_MODECHTIMEN_S  31

/** USB_OTGFS_HFIR_REG register
 *  Host Frame Interval Register
 */
#define USB_OTGFS_HFIR_REG(i) (DR_REG_USB_BASE(i) + 0x404)
/** USB_OTGFS_HFIR_FRINT : R/W; bitpos: [15:0]; default: 60000;
 *  Frame Interval (FrInt)
 *
 *  The value that the application programs to this field specifies
 *  the interval between two consecutive SOFs (FS) or micro-
 *  SOFs (HS) or Keep-Alive tokens (HS). This field contains the
 *  number of PHY clocks that constitute the required frame
 *  interval. The Default value set in this field  is for FS operation
 *  when the PHY clock frequency is 60 MHz. The application can
 *  write a value to this register only after the Port Enable bit of the
 *  Host Port Control and Status register (HPRT.PrtEnaPort) has
 *  been Set. If no value is programmed, the core calculates the
 *  value based on the PHY clock specified in the FS/LS PHY
 *  Clock Select field of the Host Configuration register
 *  (HCFG.FSLSPclkSel). Do not change the value of this field
 *  after the initial configuration.
 *  - 125 s * (PHY clock frequency for HS)
 *  - 1 ms * (PHY clock frequency for FS/LS)
 */
#define USB_OTGFS_HFIR_FRINT    0x0000FFFFU
#define USB_OTGFS_HFIR_FRINT_M  (USB_OTGFS_HFIR_FRINT_V << USB_OTGFS_HFIR_FRINT_S)
#define USB_OTGFS_HFIR_FRINT_V  0x0000FFFFU
#define USB_OTGFS_HFIR_FRINT_S  0
/** USB_OTGFS_HFIR_HFIRRLDCTRL : R/W; bitpos: [16]; default: 0;
 *  Reload Control (HFIRRldCtrl)
 *
 *  This bit allows dynamic reloading of the HFIR register during run time.
 *  - 1'b0 : The HFIR cannot be reloaded dynamically
 *  - 1'b1: the HFIR can be dynamically reloaded during runtime.
 *  This bit needs to be programmed during initial configuration and its value should
 *  not be changed during runtime.
 */
#define USB_OTGFS_HFIR_HFIRRLDCTRL    (BIT(16))
#define USB_OTGFS_HFIR_HFIRRLDCTRL_M  (USB_OTGFS_HFIR_HFIRRLDCTRL_V << USB_OTGFS_HFIR_HFIRRLDCTRL_S)
#define USB_OTGFS_HFIR_HFIRRLDCTRL_V  0x00000001U
#define USB_OTGFS_HFIR_HFIRRLDCTRL_S  16

/** USB_OTGFS_HFNUM_REG register
 *  Host Frame Number/Frame Time Remaining Register  This register indicates the
 *  current frame number. It also indicates the time remaining (in terms of the number
 *  of PHY clocks) in the current (micro)frame.  Note: Read the reset value of this
 *  register only after the following conditions:  - If IDDIG_FILTER is disabled, read
 *  only when the PHY clock is stable.  - If IDDIG_FILTER is enabled, read only after
 *  the filter timer expires.
 */
#define USB_OTGFS_HFNUM_REG(i) (DR_REG_USB_BASE(i) + 0x408)
/** USB_OTGFS_HFNUM_FRNUM : RO; bitpos: [15:0]; default: 16383;
 *  Frame Number (FrNum)
 *
 *  This field increments when a new SOF is transmitted on the
 *  USB, and is reset to 0 when it reaches 16'h3FFF.
 */
#define USB_OTGFS_HFNUM_FRNUM    0x0000FFFFU
#define USB_OTGFS_HFNUM_FRNUM_M  (USB_OTGFS_HFNUM_FRNUM_V << USB_OTGFS_HFNUM_FRNUM_S)
#define USB_OTGFS_HFNUM_FRNUM_V  0x0000FFFFU
#define USB_OTGFS_HFNUM_FRNUM_S  0
/** USB_OTGFS_HFNUM_FRREM : RO; bitpos: [31:16]; default: 0;
 *  Frame Time Remaining (FrRem)
 *
 *  Indicates the amount of time remaining in the current
 *  microframe (HS) or Frame (FS/LS), in terms of PHY clocks. This
 *  field decrements on each PHY clock. When it reaches zero, this
 *  field is reloaded with the value in the Frame Interval register and
 *  a new SOF is transmitted on the USB.
 */
#define USB_OTGFS_HFNUM_FRREM    0x0000FFFFU
#define USB_OTGFS_HFNUM_FRREM_M  (USB_OTGFS_HFNUM_FRREM_V << USB_OTGFS_HFNUM_FRREM_S)
#define USB_OTGFS_HFNUM_FRREM_V  0x0000FFFFU
#define USB_OTGFS_HFNUM_FRREM_S  16

/** USB_OTGFS_HPTXSTS_REG register
 *  Host Periodic Transmit FIFO/Queue Status Register
 */
#define USB_OTGFS_HPTXSTS_REG(i) (DR_REG_USB_BASE(i) + 0x410)
/** USB_OTGFS_HPTXSTS_PTXFSPCAVAIL : RO; bitpos: [15:0]; default: 256;
 *  Periodic Transmit Data FIFO Space Available (PTxFSpcAvail)
 *
 *  Indicates the number of free locations available to be written to in the Periodic
 *  TxFIFO.
 *
 *  Values are in terms of 32-bit words
 *  - 16'h0    : Periodic TxFIFO is full
 *  - 16'h1    : 1 word available
 *  - 16'h2    : 2 words available
 *  - 16'hn    : n words available (where 0  n  32,768)
 *  - 16'h8000 : 32,768 words
 *  - Others   : Reserved
 */
#define USB_OTGFS_HPTXSTS_PTXFSPCAVAIL    0x0000FFFFU
#define USB_OTGFS_HPTXSTS_PTXFSPCAVAIL_M  (USB_OTGFS_HPTXSTS_PTXFSPCAVAIL_V << USB_OTGFS_HPTXSTS_PTXFSPCAVAIL_S)
#define USB_OTGFS_HPTXSTS_PTXFSPCAVAIL_V  0x0000FFFFU
#define USB_OTGFS_HPTXSTS_PTXFSPCAVAIL_S  0
/** USB_OTGFS_HPTXSTS_PTXQSPCAVAIL : RO; bitpos: [23:16]; default: 8;
 *  Periodic Transmit Request Queue Space Available (PTxQSpcAvail)
 *
 *  Indicates the number of free locations available to be written in the Periodic
 *  Transmit Request Queue. This queue holds both IN and OUT requests.
 *  - 8'h0: Periodic Transmit Request Queue is full
 *  - 8'h1: 1 location available
 *  - 8'h2: 2 locations available
 *  - n: n locations available (0 <= n <= 16)
 *  - Others: Reserved
 */
#define USB_OTGFS_HPTXSTS_PTXQSPCAVAIL    0x000000FFU
#define USB_OTGFS_HPTXSTS_PTXQSPCAVAIL_M  (USB_OTGFS_HPTXSTS_PTXQSPCAVAIL_V << USB_OTGFS_HPTXSTS_PTXQSPCAVAIL_S)
#define USB_OTGFS_HPTXSTS_PTXQSPCAVAIL_V  0x000000FFU
#define USB_OTGFS_HPTXSTS_PTXQSPCAVAIL_S  16
/** USB_OTGFS_HPTXSTS_PTXQTOP : RO; bitpos: [31:24]; default: 0;
 *  Top of the Periodic Transmit Request Queue (PTxQTop)
 *
 *  This indicates the Entry in the Periodic Tx Request Queue that is
 *  currently being processes by the MAC.
 *
 *  This register is used for debugging.
 *  - Bit [31]: Odd/Even (micro)Frame
 *  -- 1'b0: send in even (micro)Frame
 *  -- 1'b1: send in odd (micro)Frame
 *  - Bits [30:27]: Channel/endpoint number
 *  - Bits [26:25]: Type
 *  -- 2'b00: IN/OUT
 *  -- 2'b01: Zero-length packet
 *  -- 2'b10: CSPLIT
 *  -- 2'b11: Disable channel command
 *  - Bit [24]: Terminate (last Entry for the selected channel/endpoint)
 */
#define USB_OTGFS_HPTXSTS_PTXQTOP    0x000000FFU
#define USB_OTGFS_HPTXSTS_PTXQTOP_M  (USB_OTGFS_HPTXSTS_PTXQTOP_V << USB_OTGFS_HPTXSTS_PTXQTOP_S)
#define USB_OTGFS_HPTXSTS_PTXQTOP_V  0x000000FFU
#define USB_OTGFS_HPTXSTS_PTXQTOP_S  24

/** USB_OTGFS_HAINT_REG register
 *  Host All Channels Interrupt Register  When a significant event occurs on a channel,
 *  the Host All Channels Interrupt register interrupts the application using the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt). This is
 *  shown in the Interrupt Hierarchy figure in the databook. There is one interrupt bit
 *  per channel, up to a maximum of 16 bits. Bits in this register are set and cleared
 *  when the application sets and clears bits in the corresponding Host Channel-n
 *  Interrupt register.
 */
#define USB_OTGFS_HAINT_REG(i) (DR_REG_USB_BASE(i) + 0x414)
/** USB_OTGFS_HAINT_HAINT : RO; bitpos: [7:0]; default: 0;
 *
 *  Channel Interrupt for channel no.
 */
#define USB_OTGFS_HAINT_HAINT    0x000000FFU
#define USB_OTGFS_HAINT_HAINT_M  (USB_OTGFS_HAINT_HAINT_V << USB_OTGFS_HAINT_HAINT_S)
#define USB_OTGFS_HAINT_HAINT_V  0x000000FFU
#define USB_OTGFS_HAINT_HAINT_S  0

/** USB_OTGFS_HAINTMSK_REG register
 *  Host All Channels Interrupt Mask Register  The Host All Channel Interrupt Mask
 *  register works with the Host All Channel Interrupt register to interrupt the
 *  application when an event occurs on a channel. There is one interrupt mask bit per
 *  channel, up to a maximum of 16 bits.
 */
#define USB_OTGFS_HAINTMSK_REG(i) (DR_REG_USB_BASE(i) + 0x418)
/** USB_OTGFS_HAINTMSK_HAINTMSK : R/W; bitpos: [7:0]; default: 0;
 *  Channel Interrupt Mask (HAINTMsk)
 *  One bit per channel: Bit 0 for channel 0, bit 15 for channel 15
 */
#define USB_OTGFS_HAINTMSK_HAINTMSK    0x000000FFU
#define USB_OTGFS_HAINTMSK_HAINTMSK_M  (USB_OTGFS_HAINTMSK_HAINTMSK_V << USB_OTGFS_HAINTMSK_HAINTMSK_S)
#define USB_OTGFS_HAINTMSK_HAINTMSK_V  0x000000FFU
#define USB_OTGFS_HAINTMSK_HAINTMSK_S  0

/** USB_OTGFS_HFLBAddr_REG register
 *  Host Frame List Base Address Register  This register is present only in case of
 *  Scatter/Gather DMA. It is implemented as flops. This register holds the starting
 *  address of the Frame list information.
 */
#define USB_OTGFS_HFLBADDR_REG(i) (DR_REG_USB_BASE(i) + 0x41c)
/** USB_OTGFS_HFLBADDR_HFLBADDR : R/W; bitpos: [31:0]; default: 0;
 *  The starting address of the Frame list.
 *  This register is used only for Isochronous and Interrupt Channels.
 */
#define USB_OTGFS_HFLBADDR_HFLBADDR    0xFFFFFFFFU
#define USB_OTGFS_HFLBADDR_HFLBADDR_M  (USB_OTGFS_HFLBADDR_HFLBADDR_V << USB_OTGFS_HFLBADDR_HFLBADDR_S)
#define USB_OTGFS_HFLBADDR_HFLBADDR_V  0xFFFFFFFFU
#define USB_OTGFS_HFLBADDR_HFLBADDR_S  0

/** USB_OTGFS_HPRT_REG register
 *  Host Port Control and Status Register  This register is available only in Host
 *  mode. Currently, the OTG Host supports only one port. A single register holds USB
 *  port-related information such as USB reset, enable, suspend, resume, connect
 *  status, and test mode for each port. It is shown in the Interrupt Hierarchy figure
 *  in the databook. The R_SS_WC bits in this register can trigger an interrupt to the
 *  application through the Host Port Interrupt bit of the Core Interrupt register
 *  (GINTSTS.PrtInt). On a Port Interrupt, the application must read this register and
 *  clear the bit that caused the interrupt. For the R_SS_WC bits, the application must
 *  write a 1 to the bit to clear the interrupt.
 */
#define USB_OTGFS_HPRT_REG(i) (DR_REG_USB_BASE(i) + 0x440)
/** USB_OTGFS_HPRT_PRTCONNSTS : RO; bitpos: [0]; default: 0;
 *  Port Connect Status (PrtConnSts)
 *  - 0: No device is attached to the port.
 *  - 1: A device is attached to the port.
 */
#define USB_OTGFS_HPRT_PRTCONNSTS    (BIT(0))
#define USB_OTGFS_HPRT_PRTCONNSTS_M  (USB_OTGFS_HPRT_PRTCONNSTS_V << USB_OTGFS_HPRT_PRTCONNSTS_S)
#define USB_OTGFS_HPRT_PRTCONNSTS_V  0x00000001U
#define USB_OTGFS_HPRT_PRTCONNSTS_S  0
/** USB_OTGFS_HPRT_PRTCONNDET : R/W; bitpos: [1]; default: 0;
 *  Port Connect Detected (PrtConnDet)
 *
 *  The core sets this bit when a device connection is detected
 *  to trigger an interrupt to the application using the Host Port
 *  Interrupt bit of the Core Interrupt register (GINTSTS.PrtInt).This bit can be set
 *  only by the core and the application should write 1 to clear it.The application
 *  must write a 1 to this bit to clear the
 *  interrupt.
 */
#define USB_OTGFS_HPRT_PRTCONNDET    (BIT(1))
#define USB_OTGFS_HPRT_PRTCONNDET_M  (USB_OTGFS_HPRT_PRTCONNDET_V << USB_OTGFS_HPRT_PRTCONNDET_S)
#define USB_OTGFS_HPRT_PRTCONNDET_V  0x00000001U
#define USB_OTGFS_HPRT_PRTCONNDET_S  1
/** USB_OTGFS_HPRT_PRTENA : R/W; bitpos: [2]; default: 0;
 *  Port Enable (PrtEna)
 *
 *  A port is enabled only by the core after a reset sequence,
 *  and is disabled by an overcurrent condition, a disconnect
 *  condition, or by the application clearing this bit. The
 *  application cannot Set this bit by a register write. It can only
 *  clear it to disable the port by writing 1. This bit does not trigger any
 *  interrupt to the application.
 *  - 1'b0: Port disabled
 *  - 1'b1: Port enabled
 */
#define USB_OTGFS_HPRT_PRTENA    (BIT(2))
#define USB_OTGFS_HPRT_PRTENA_M  (USB_OTGFS_HPRT_PRTENA_V << USB_OTGFS_HPRT_PRTENA_S)
#define USB_OTGFS_HPRT_PRTENA_V  0x00000001U
#define USB_OTGFS_HPRT_PRTENA_S  2
/** USB_OTGFS_HPRT_PRTENCHNG : R/W; bitpos: [3]; default: 0;
 *  Port Enable/Disable Change (PrtEnChng)
 *
 *  The core sets this bit when the status of the Port Enable bit [2] of this register
 *  changes.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HPRT_PRTENCHNG    (BIT(3))
#define USB_OTGFS_HPRT_PRTENCHNG_M  (USB_OTGFS_HPRT_PRTENCHNG_V << USB_OTGFS_HPRT_PRTENCHNG_S)
#define USB_OTGFS_HPRT_PRTENCHNG_V  0x00000001U
#define USB_OTGFS_HPRT_PRTENCHNG_S  3
/** USB_OTGFS_HPRT_PRTOVRCURRACT : RO; bitpos: [4]; default: 0;
 *  Port Overcurrent Active (PrtOvrCurrAct)
 *
 *  Indicates the overcurrent condition of the port.
 *  - 1'b0: No overcurrent condition
 *  - 1'b1: Overcurrent condition
 */
#define USB_OTGFS_HPRT_PRTOVRCURRACT    (BIT(4))
#define USB_OTGFS_HPRT_PRTOVRCURRACT_M  (USB_OTGFS_HPRT_PRTOVRCURRACT_V << USB_OTGFS_HPRT_PRTOVRCURRACT_S)
#define USB_OTGFS_HPRT_PRTOVRCURRACT_V  0x00000001U
#define USB_OTGFS_HPRT_PRTOVRCURRACT_S  4
/** USB_OTGFS_HPRT_PRTOVRCURRCHNG : R/W; bitpos: [5]; default: 0;
 *  Port Overcurrent Change (PrtOvrCurrChng)
 *
 *  The core sets this bit when the status of the Port Overcurrent Active bit (bit 4)
 *  in this register changes.This bit can be set only by the core and the application
 *  should write 1 to clear it
 */
#define USB_OTGFS_HPRT_PRTOVRCURRCHNG    (BIT(5))
#define USB_OTGFS_HPRT_PRTOVRCURRCHNG_M  (USB_OTGFS_HPRT_PRTOVRCURRCHNG_V << USB_OTGFS_HPRT_PRTOVRCURRCHNG_S)
#define USB_OTGFS_HPRT_PRTOVRCURRCHNG_V  0x00000001U
#define USB_OTGFS_HPRT_PRTOVRCURRCHNG_S  5
/** USB_OTGFS_HPRT_PRTRES : R/W; bitpos: [6]; default: 0;
 *  Port Resume (PrtRes)
 *
 *  The application sets this bit to drive resume signaling on the
 *  port. The core continues to drive the resume signal until the
 *  application clears this bit.
 *
 *  If the core detects a USB remote wakeup sequence, as
 *  indicated by the Port Resume/Remote Wakeup Detected
 *  Interrupt bit of the Core Interrupt register
 *  (GINTSTS.WkUpInt), the core starts driving resume
 *  signaling without application intervention and clears this bit
 *  when it detects a disconnect condition. The read value of
 *  this bit indicates whether the core is currently driving
 *  resume signaling.
 *  - 1'b0: No resume driven
 *  - 1'b1: Resume driven
 *  When LPM is enabled, In L1 state the behavior of this bit is as follows:
 *  The application sets this bit to drive resume signaling on the port.
 *  The core continues to drive the resume signal until a pre-determined time
 *  specified in GLPMCFG.HIRD_Thres[3:0] field. If the core detects a USB remote
 *  wakeup sequence, as indicated by the Port L1Resume/Remote L1Wakeup Detected
 *  Interrupt bit of the Core Interrupt register (GINTSTS.L1WkUpInt),
 *  the core starts driving resume signaling without application intervention
 *  and clears this bit at the end of resume.This bit can be set by both core or
 *  application
 *  and also cleared by core or application. This bit is cleared by the core even if
 *  there is
 *  no device connected to the Host.
 */
#define USB_OTGFS_HPRT_PRTRES    (BIT(6))
#define USB_OTGFS_HPRT_PRTRES_M  (USB_OTGFS_HPRT_PRTRES_V << USB_OTGFS_HPRT_PRTRES_S)
#define USB_OTGFS_HPRT_PRTRES_V  0x00000001U
#define USB_OTGFS_HPRT_PRTRES_S  6
/** USB_OTGFS_HPRT_PRTSUSP : R/W; bitpos: [7]; default: 0;
 *  Port Suspend (PrtSusp)
 *
 *  The application sets this bit to put this port in Suspend
 *  mode. The core only stops sending SOFs when this is Set.
 *  To stop the PHY clock, the application must Set the Port
 *  Clock Stop bit, which asserts the suspend input pin of the
 *  PHY.
 *
 *  The read value of this bit reflects the current suspend status
 *  of the port. This bit is cleared by the core after a remote
 *  wakeup signal is detected or the application sets the Port
 *  Reset bit or Port Resume bit in this register or the
 *  Resume/Remote Wakeup Detected Interrupt bit or
 *  Disconnect Detected Interrupt bit in the Core Interrupt
 *  register (GINTSTS.WkUpInt or GINTSTS.DisconnInt,
 *  respectively).This bit is cleared by the core even if there is
 *  no device connected to the Host.
 *  - 1'b0: Port not in Suspend mode
 *  - 1'b1: Port in Suspend mode
 */
#define USB_OTGFS_HPRT_PRTSUSP    (BIT(7))
#define USB_OTGFS_HPRT_PRTSUSP_M  (USB_OTGFS_HPRT_PRTSUSP_V << USB_OTGFS_HPRT_PRTSUSP_S)
#define USB_OTGFS_HPRT_PRTSUSP_V  0x00000001U
#define USB_OTGFS_HPRT_PRTSUSP_S  7
/** USB_OTGFS_HPRT_PRTRST : R/W; bitpos: [8]; default: 0;
 *  Port Reset (PrtRst)
 *
 *  When the application sets this bit, a reset sequence is
 *  started on this port. The application must time the reset
 *  period and clear this bit after the reset sequence is
 *  complete.
 *  - 1'b0: Port not in reset
 *  - 1'b1: Port in reset
 *  The application must leave this bit set for at least a
 *  minimum duration mentioned below to start a reset on the
 *  port. The application can leave it set for another 10 ms in
 *  addition to the required minimum duration, before clearing
 *  the bit, even though there is no maximum limit Set by the
 *  USB standard.This bit is cleared by the core even if there is
 *  no device connected to the Host.
 *  - High speed: 50 ms
 *  - Full speed/Low speed: 10 ms
 */
#define USB_OTGFS_HPRT_PRTRST    (BIT(8))
#define USB_OTGFS_HPRT_PRTRST_M  (USB_OTGFS_HPRT_PRTRST_V << USB_OTGFS_HPRT_PRTRST_S)
#define USB_OTGFS_HPRT_PRTRST_V  0x00000001U
#define USB_OTGFS_HPRT_PRTRST_S  8
/** USB_OTGFS_HPRT_PRTLNSTS : RO; bitpos: [11:10]; default: 0;
 *  Port Line Status (PrtLnSts)
 *
 *  Indicates the current logic level USB data lines
 *  - Bit [10]: Logic level of D+
 *  - Bit [11]: Logic level of D-
 */
#define USB_OTGFS_HPRT_PRTLNSTS    0x00000003U
#define USB_OTGFS_HPRT_PRTLNSTS_M  (USB_OTGFS_HPRT_PRTLNSTS_V << USB_OTGFS_HPRT_PRTLNSTS_S)
#define USB_OTGFS_HPRT_PRTLNSTS_V  0x00000003U
#define USB_OTGFS_HPRT_PRTLNSTS_S  10
/** USB_OTGFS_HPRT_PRTPWR : R/W; bitpos: [12]; default: 0;
 *  Port Power (PrtPwr)
 *
 *  The application uses this field to control power to this port (write 1'b1 to set to
 *  1'b1
 *  and write 1'b0 to set to 1'b0), and the core can clear this bit on an over current
 *  condition.
 *  - 1'b0: Power off
 *  - 1'b1: Power on
 *
 *  Note: This bit is interface independent. The application needs to program this bit
 *  for all interfaces as described in the host programming flow in the Programming
 *  Guide.
 */
#define USB_OTGFS_HPRT_PRTPWR    (BIT(12))
#define USB_OTGFS_HPRT_PRTPWR_M  (USB_OTGFS_HPRT_PRTPWR_V << USB_OTGFS_HPRT_PRTPWR_S)
#define USB_OTGFS_HPRT_PRTPWR_V  0x00000001U
#define USB_OTGFS_HPRT_PRTPWR_S  12
/** USB_OTGFS_HPRT_PRTTSTCTL : R/W; bitpos: [16:13]; default: 0;
 *  Port Test Control (PrtTstCtl)
 *
 *  The application writes a nonzero value to this field to put the port into a Test
 *  mode, and the corresponding pattern is signaled on the port.
 *  - 4'b0000: Test mode disabled
 *  - 4'b0001: Test_J mode
 *  - 4'b0010: Test_K mode
 *  - 4'b0011: Test_SE0_NAK mode
 *  - 4'b0100: Test_Packet mode
 *  - 4'b0101: Test_Force_Enable
 *  - Others: Reserved
 *
 *  To move the DWC_otg controller to test mode, you must set this field. Complete the
 *  following steps to move the DWC_otg core to test mode:
 *  - 1. Power on the core.
 *  - 2. Load the DWC_otg driver.
 *  - 3. Connect an HS device and enumerate to HS mode.
 *  - 4. Access the HPRT register to send test packets.
 *  - 5. Remove the device and connect to fixture (OPT) port. The DWC_otg host core
 *  continues sending out test packets.
 *  - 6. Test the eye diagram.
 */
#define USB_OTGFS_HPRT_PRTTSTCTL    0x0000000FU
#define USB_OTGFS_HPRT_PRTTSTCTL_M  (USB_OTGFS_HPRT_PRTTSTCTL_V << USB_OTGFS_HPRT_PRTTSTCTL_S)
#define USB_OTGFS_HPRT_PRTTSTCTL_V  0x0000000FU
#define USB_OTGFS_HPRT_PRTTSTCTL_S  13
/** USB_OTGFS_HPRT_PRTSPD : RO; bitpos: [18:17]; default: 0;
 *  Port Speed (PrtSpd)
 *
 *  Indicates the speed of the device attached to this port.
 *  - 2'b00: High speed
 *  - 2'b01: Full speed
 *  - 2'b10: Low speed
 *  - 2'b11: Reserved
 */
#define USB_OTGFS_HPRT_PRTSPD    0x00000003U
#define USB_OTGFS_HPRT_PRTSPD_M  (USB_OTGFS_HPRT_PRTSPD_V << USB_OTGFS_HPRT_PRTSPD_S)
#define USB_OTGFS_HPRT_PRTSPD_V  0x00000003U
#define USB_OTGFS_HPRT_PRTSPD_S  17

/** USB_OTGFS_HCCHAR0_REG register
 *  Host Channel 0 Characteristics Register
 */
#define USB_OTGFS_HCCHAR0_REG(i) (DR_REG_USB_BASE(i) + 0x500)
/** USB_OTGFS_HCCHAR0_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  Indicates the maximum packet size of the associated endpoint.
 */
#define USB_OTGFS_HCCHAR0_MPS    0x000007FFU
#define USB_OTGFS_HCCHAR0_MPS_M  (USB_OTGFS_HCCHAR0_MPS_V << USB_OTGFS_HCCHAR0_MPS_S)
#define USB_OTGFS_HCCHAR0_MPS_V  0x000007FFU
#define USB_OTGFS_HCCHAR0_MPS_S  0
/** USB_OTGFS_HCCHAR0_EPNUM : R/W; bitpos: [14:11]; default: 0;
 *  Endpoint Number (EPNum)
 *
 *  Indicates the endpoint number on the device serving as the data source or sink.
 */
#define USB_OTGFS_HCCHAR0_EPNUM    0x0000000FU
#define USB_OTGFS_HCCHAR0_EPNUM_M  (USB_OTGFS_HCCHAR0_EPNUM_V << USB_OTGFS_HCCHAR0_EPNUM_S)
#define USB_OTGFS_HCCHAR0_EPNUM_V  0x0000000FU
#define USB_OTGFS_HCCHAR0_EPNUM_S  11
/** USB_OTGFS_HCCHAR0_EPDIR : R/W; bitpos: [15]; default: 0;
 *  Endpoint Direction (EPDir)
 *
 *  Indicates whether the transaction is IN or OUT.
 *  - 1'b0: OUT
 *  - 1'b1: IN
 */
#define USB_OTGFS_HCCHAR0_EPDIR    (BIT(15))
#define USB_OTGFS_HCCHAR0_EPDIR_M  (USB_OTGFS_HCCHAR0_EPDIR_V << USB_OTGFS_HCCHAR0_EPDIR_S)
#define USB_OTGFS_HCCHAR0_EPDIR_V  0x00000001U
#define USB_OTGFS_HCCHAR0_EPDIR_S  15
/** USB_OTGFS_HCCHAR0_LSPDDEV : R/W; bitpos: [17]; default: 0;
 *  Low-Speed Device (LSpdDev)
 *
 *  This field is Set by the application to indicate that this channel is communicating
 *  to a low-speed device.
 *
 *  The application must program this bit when a low speed device is connected to the
 *  host through an FS HUB. The DWC_otg Host core uses this field to drive the
 *  XCVR_SELECT signal to 2'b11 while communicating to the LS Device through the FS hub.
 *
 *  Note: In a peer to peer setup, the DWC_otg Host core ignores this bit even if it is
 *  set by the application software.
 */
#define USB_OTGFS_HCCHAR0_LSPDDEV    (BIT(17))
#define USB_OTGFS_HCCHAR0_LSPDDEV_M  (USB_OTGFS_HCCHAR0_LSPDDEV_V << USB_OTGFS_HCCHAR0_LSPDDEV_S)
#define USB_OTGFS_HCCHAR0_LSPDDEV_V  0x00000001U
#define USB_OTGFS_HCCHAR0_LSPDDEV_S  17
/** USB_OTGFS_HCCHAR0_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Indicates the transfer type selected.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_HCCHAR0_EPTYPE    0x00000003U
#define USB_OTGFS_HCCHAR0_EPTYPE_M  (USB_OTGFS_HCCHAR0_EPTYPE_V << USB_OTGFS_HCCHAR0_EPTYPE_S)
#define USB_OTGFS_HCCHAR0_EPTYPE_V  0x00000003U
#define USB_OTGFS_HCCHAR0_EPTYPE_S  18
/** USB_OTGFS_HCCHAR0_EC : R/W; bitpos: [21:20]; default: 0;
 *  Multi Count (MC) / Error Count (EC)
 *
 *  When the Split Enable bit of the Host Channel-n Split Control
 *  register (HCSPLTn.SpltEna) is reset (1'b0), this field indicates to
 *  the host the number of transactions that must be executed per
 *  microframe for this periodic endpoint. For non periodic transfers,
 *  this field is used only in DMA mode, and specifies the number
 *  packets to be fetched for this channel before the internal DMA
 *  engine changes arbitration.
 *  - 2'b00: Reserved This field yields undefined results.
 *  - 2'b01: 1 transaction
 *  - 2'b10: 2 transactions to be issued for this endpoint per microframe
 *  - 2'b11: 3 transactions to be issued for this endpoint per microframe
 *  When HCSPLTn.SpltEna is Set (1'b1), this field indicates the
 *  number of immediate retries to be performed for a periodic split
 *  transactions on transaction errors. This field must be Set to at
 *  least 2'b01.
 */
#define USB_OTGFS_HCCHAR0_EC    0x00000003U
#define USB_OTGFS_HCCHAR0_EC_M  (USB_OTGFS_HCCHAR0_EC_V << USB_OTGFS_HCCHAR0_EC_S)
#define USB_OTGFS_HCCHAR0_EC_V  0x00000003U
#define USB_OTGFS_HCCHAR0_EC_S  20
/** USB_OTGFS_HCCHAR0_DEVADDR : R/W; bitpos: [28:22]; default: 0;
 *  Device Address (DevAddr)
 *
 *  This field selects the specific device serving as the data source
 *  or sink.
 */
#define USB_OTGFS_HCCHAR0_DEVADDR    0x0000007FU
#define USB_OTGFS_HCCHAR0_DEVADDR_M  (USB_OTGFS_HCCHAR0_DEVADDR_V << USB_OTGFS_HCCHAR0_DEVADDR_S)
#define USB_OTGFS_HCCHAR0_DEVADDR_V  0x0000007FU
#define USB_OTGFS_HCCHAR0_DEVADDR_S  22
/** USB_OTGFS_HCCHAR0_ODDFRM : R/W; bitpos: [29]; default: 0;
 *  Odd Frame (OddFrm)
 *
 *  This field is set (reset) by the application to indicate that the OTG host must
 *  perform
 *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
 *  (isochronous and interrupt) transactions.
 *  - 1'b0: Even (micro)Frame
 *  - 1'b1: Odd (micro)Frame
 */
#define USB_OTGFS_HCCHAR0_ODDFRM    (BIT(29))
#define USB_OTGFS_HCCHAR0_ODDFRM_M  (USB_OTGFS_HCCHAR0_ODDFRM_V << USB_OTGFS_HCCHAR0_ODDFRM_S)
#define USB_OTGFS_HCCHAR0_ODDFRM_V  0x00000001U
#define USB_OTGFS_HCCHAR0_ODDFRM_S  29
/** USB_OTGFS_HCCHAR0_CHDIS : R/W; bitpos: [30]; default: 0;
 *  Channel Disable (ChDis)
 *
 *  The application sets this bit to stop transmitting/receiving data
 *  on a channel, even before the transfer for that channel is
 *  complete. The application must wait for the Channel Disabled
 *  interrupt before treating the channel as disabled.
 */
#define USB_OTGFS_HCCHAR0_CHDIS    (BIT(30))
#define USB_OTGFS_HCCHAR0_CHDIS_M  (USB_OTGFS_HCCHAR0_CHDIS_V << USB_OTGFS_HCCHAR0_CHDIS_S)
#define USB_OTGFS_HCCHAR0_CHDIS_V  0x00000001U
#define USB_OTGFS_HCCHAR0_CHDIS_S  30
/** USB_OTGFS_HCCHAR0_CHENA : R/W; bitpos: [31]; default: 0;
 *  Channel Enable (ChEna)
 *
 *  When Scatter/Gather mode is enabled
 *  - 1'b0: Indicates that the descriptor structure is not yet ready.
 *  - 1'b1:  Indicates  that  the  descriptor  structure  and  data  buffer  with data
 *  is setup and this channel can access the descriptor.
 *  When Scatter/Gather mode is disabled
 *
 *  This field is set by the application and cleared by the OTG host.
 *  - 1'b0: Channel disabled
 *  - 1'b1: Channel enabled
 */
#define USB_OTGFS_HCCHAR0_CHENA    (BIT(31))
#define USB_OTGFS_HCCHAR0_CHENA_M  (USB_OTGFS_HCCHAR0_CHENA_V << USB_OTGFS_HCCHAR0_CHENA_S)
#define USB_OTGFS_HCCHAR0_CHENA_V  0x00000001U
#define USB_OTGFS_HCCHAR0_CHENA_S  31

/** USB_OTGFS_HCINT0_REG register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
#define USB_OTGFS_HCINT0_REG(i) (DR_REG_USB_BASE(i) + 0x508)
/** USB_OTGFS_HCINT0_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed (XferCompl)
 *
 *  Transfer completed normally without any errors.This bit can be set only by the core
 *  and the application should write 1 to clear it.
 *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
 *  completed with IOC bit set in its descriptor.
 *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
 *  without any errors.
 */
#define USB_OTGFS_HCINT0_XFERCOMPL    (BIT(0))
#define USB_OTGFS_HCINT0_XFERCOMPL_M  (USB_OTGFS_HCINT0_XFERCOMPL_V << USB_OTGFS_HCINT0_XFERCOMPL_S)
#define USB_OTGFS_HCINT0_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_HCINT0_XFERCOMPL_S  0
/** USB_OTGFS_HCINT0_CHHLTD : R/W; bitpos: [1]; default: 0;
 *  Channel Halted (ChHltd)
 *
 *  In non Scatter/Gather DMA mode, it indicates the transfer completed abnormally
 *  either because of any USB transaction error or in response to disable request by
 *  the application or because of a completed transfer.
 *
 *  In Scatter/gather DMA mode, this indicates that transfer completed due to any of
 *  the following
 *  - EOL being set in descriptor
 *  - AHB error
 *  - Excessive transaction errors
 *  - Babble
 *  - Stall
 */
#define USB_OTGFS_HCINT0_CHHLTD    (BIT(1))
#define USB_OTGFS_HCINT0_CHHLTD_M  (USB_OTGFS_HCINT0_CHHLTD_V << USB_OTGFS_HCINT0_CHHLTD_S)
#define USB_OTGFS_HCINT0_CHHLTD_V  0x00000001U
#define USB_OTGFS_HCINT0_CHHLTD_S  1
/** USB_OTGFS_HCINT0_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  This is generated only in Internal DMA mode when there is an AHB error during AHB
 *  read/write. The application can read the corresponding channel's DMA address
 *  register to get the error address.
 */
#define USB_OTGFS_HCINT0_AHBERR    (BIT(2))
#define USB_OTGFS_HCINT0_AHBERR_M  (USB_OTGFS_HCINT0_AHBERR_V << USB_OTGFS_HCINT0_AHBERR_S)
#define USB_OTGFS_HCINT0_AHBERR_V  0x00000001U
#define USB_OTGFS_HCINT0_AHBERR_S  2
/** USB_OTGFS_HCINT0_STALL : R/W; bitpos: [3]; default: 0;
 *  STALL Response Received Interrupt (STALL)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT0_STALL    (BIT(3))
#define USB_OTGFS_HCINT0_STALL_M  (USB_OTGFS_HCINT0_STALL_V << USB_OTGFS_HCINT0_STALL_S)
#define USB_OTGFS_HCINT0_STALL_V  0x00000001U
#define USB_OTGFS_HCINT0_STALL_S  3
/** USB_OTGFS_HCINT0_NAK : R/W; bitpos: [4]; default: 0;
 *  NAK Response Received Interrupt (NAK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT0_NAK    (BIT(4))
#define USB_OTGFS_HCINT0_NAK_M  (USB_OTGFS_HCINT0_NAK_V << USB_OTGFS_HCINT0_NAK_S)
#define USB_OTGFS_HCINT0_NAK_V  0x00000001U
#define USB_OTGFS_HCINT0_NAK_S  4
/** USB_OTGFS_HCINT0_ACK : R/W; bitpos: [5]; default: 0;
 *  ACK Response Received/Transmitted Interrupt (ACK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT0_ACK    (BIT(5))
#define USB_OTGFS_HCINT0_ACK_M  (USB_OTGFS_HCINT0_ACK_V << USB_OTGFS_HCINT0_ACK_S)
#define USB_OTGFS_HCINT0_ACK_V  0x00000001U
#define USB_OTGFS_HCINT0_ACK_S  5
/** USB_OTGFS_HCINT0_NYET : R/W; bitpos: [6]; default: 0;
 *  NYET Response Received Interrupt (NYET)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT0_NYET    (BIT(6))
#define USB_OTGFS_HCINT0_NYET_M  (USB_OTGFS_HCINT0_NYET_V << USB_OTGFS_HCINT0_NYET_S)
#define USB_OTGFS_HCINT0_NYET_V  0x00000001U
#define USB_OTGFS_HCINT0_NYET_S  6
/** USB_OTGFS_HCINT0_XACTERR : R/W; bitpos: [7]; default: 0;
 *  Transaction Error (XactErr)
 *
 *  Indicates one of the following errors occurred on the USB.
 *  - CRC check failure
 *  - Timeout
 *  - Bit stuff error
 *  - False EOP
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT0_XACTERR    (BIT(7))
#define USB_OTGFS_HCINT0_XACTERR_M  (USB_OTGFS_HCINT0_XACTERR_V << USB_OTGFS_HCINT0_XACTERR_S)
#define USB_OTGFS_HCINT0_XACTERR_V  0x00000001U
#define USB_OTGFS_HCINT0_XACTERR_S  7
/** USB_OTGFS_HCINT0_BBLERR : R/W; bitpos: [8]; default: 0;
 *  Babble Error (BblErr)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
 *  This bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_HCINT0_BBLERR    (BIT(8))
#define USB_OTGFS_HCINT0_BBLERR_M  (USB_OTGFS_HCINT0_BBLERR_V << USB_OTGFS_HCINT0_BBLERR_S)
#define USB_OTGFS_HCINT0_BBLERR_V  0x00000001U
#define USB_OTGFS_HCINT0_BBLERR_S  8
/** USB_OTGFS_HCINT0_FRMOVRUN : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun (FrmOvrun).
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core. This bit can be set only by the core and the application should write
 *  1 to clear
 *  it.
 */
#define USB_OTGFS_HCINT0_FRMOVRUN    (BIT(9))
#define USB_OTGFS_HCINT0_FRMOVRUN_M  (USB_OTGFS_HCINT0_FRMOVRUN_V << USB_OTGFS_HCINT0_FRMOVRUN_S)
#define USB_OTGFS_HCINT0_FRMOVRUN_V  0x00000001U
#define USB_OTGFS_HCINT0_FRMOVRUN_S  9
/** USB_OTGFS_HCINT0_DATATGLERR : R/W; bitpos: [10]; default: 0;
 *
 *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
 *  application should write 1 to clear
 *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core.
 */
#define USB_OTGFS_HCINT0_DATATGLERR    (BIT(10))
#define USB_OTGFS_HCINT0_DATATGLERR_M  (USB_OTGFS_HCINT0_DATATGLERR_V << USB_OTGFS_HCINT0_DATATGLERR_S)
#define USB_OTGFS_HCINT0_DATATGLERR_V  0x00000001U
#define USB_OTGFS_HCINT0_DATATGLERR_S  10
/** USB_OTGFS_HCINT0_BNAINTR : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process. BNA will not be generated
 *  for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT0_BNAINTR    (BIT(11))
#define USB_OTGFS_HCINT0_BNAINTR_M  (USB_OTGFS_HCINT0_BNAINTR_V << USB_OTGFS_HCINT0_BNAINTR_S)
#define USB_OTGFS_HCINT0_BNAINTR_V  0x00000001U
#define USB_OTGFS_HCINT0_BNAINTR_S  11
/** USB_OTGFS_HCINT0_XCS_XACT_ERR : R/W; bitpos: [12]; default: 0;
 *  Excessive Transaction Error (XCS_XACT_ERR)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
 *  not be generated for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT0_XCS_XACT_ERR    (BIT(12))
#define USB_OTGFS_HCINT0_XCS_XACT_ERR_M  (USB_OTGFS_HCINT0_XCS_XACT_ERR_V << USB_OTGFS_HCINT0_XCS_XACT_ERR_S)
#define USB_OTGFS_HCINT0_XCS_XACT_ERR_V  0x00000001U
#define USB_OTGFS_HCINT0_XCS_XACT_ERR_S  12
/** USB_OTGFS_HCINT0_DESC_LST_ROLLINTR : R/W; bitpos: [13]; default: 0;
 *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when the corresponding channel's descriptor list rolls over.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT0_DESC_LST_ROLLINTR    (BIT(13))
#define USB_OTGFS_HCINT0_DESC_LST_ROLLINTR_M  (USB_OTGFS_HCINT0_DESC_LST_ROLLINTR_V << USB_OTGFS_HCINT0_DESC_LST_ROLLINTR_S)
#define USB_OTGFS_HCINT0_DESC_LST_ROLLINTR_V  0x00000001U
#define USB_OTGFS_HCINT0_DESC_LST_ROLLINTR_S  13

/** USB_OTGFS_HCINTMSK0_REG register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
#define USB_OTGFS_HCINTMSK0_REG(i) (DR_REG_USB_BASE(i) + 0x50c)
/** USB_OTGFS_HCINTMSK0_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *
 *  Transfer Completed Mask (XferComplMsk)
 */
#define USB_OTGFS_HCINTMSK0_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_HCINTMSK0_XFERCOMPLMSK_M  (USB_OTGFS_HCINTMSK0_XFERCOMPLMSK_V << USB_OTGFS_HCINTMSK0_XFERCOMPLMSK_S)
#define USB_OTGFS_HCINTMSK0_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK0_XFERCOMPLMSK_S  0
/** USB_OTGFS_HCINTMSK0_CHHLTDMSK : R/W; bitpos: [1]; default: 0;
 *
 *  Channel Halted Mask (ChHltdMsk)
 */
#define USB_OTGFS_HCINTMSK0_CHHLTDMSK    (BIT(1))
#define USB_OTGFS_HCINTMSK0_CHHLTDMSK_M  (USB_OTGFS_HCINTMSK0_CHHLTDMSK_V << USB_OTGFS_HCINTMSK0_CHHLTDMSK_S)
#define USB_OTGFS_HCINTMSK0_CHHLTDMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK0_CHHLTDMSK_S  1
/** USB_OTGFS_HCINTMSK0_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *
 *  AHB Error Mask (AHBErrMsk)
 *  In  scatter/gather  DMA  mode  for  host,
 *  interrupts will not be generated due to the corresponding bits set in
 *  HCINTn.
 */
#define USB_OTGFS_HCINTMSK0_AHBERRMSK    (BIT(2))
#define USB_OTGFS_HCINTMSK0_AHBERRMSK_M  (USB_OTGFS_HCINTMSK0_AHBERRMSK_V << USB_OTGFS_HCINTMSK0_AHBERRMSK_S)
#define USB_OTGFS_HCINTMSK0_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK0_AHBERRMSK_S  2
/** USB_OTGFS_HCINTMSK0_BNAINTRMSK : R/W; bitpos: [11]; default: 0;
 *
 *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK0_BNAINTRMSK    (BIT(11))
#define USB_OTGFS_HCINTMSK0_BNAINTRMSK_M  (USB_OTGFS_HCINTMSK0_BNAINTRMSK_V << USB_OTGFS_HCINTMSK0_BNAINTRMSK_S)
#define USB_OTGFS_HCINTMSK0_BNAINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK0_BNAINTRMSK_S  11
/** USB_OTGFS_HCINTMSK0_DESC_LST_ROLLINTRMSK : R/W; bitpos: [13]; default: 0;
 *
 *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK0_DESC_LST_ROLLINTRMSK    (BIT(13))
#define USB_OTGFS_HCINTMSK0_DESC_LST_ROLLINTRMSK_M  (USB_OTGFS_HCINTMSK0_DESC_LST_ROLLINTRMSK_V << USB_OTGFS_HCINTMSK0_DESC_LST_ROLLINTRMSK_S)
#define USB_OTGFS_HCINTMSK0_DESC_LST_ROLLINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK0_DESC_LST_ROLLINTRMSK_S  13

/** USB_OTGFS_HCTSIZ0_REG register
 *  Host Channel 0 Transfer Size Register
 */
#define USB_OTGFS_HCTSIZ0_REG(i) (DR_REG_USB_BASE(i) + 0x510)
/** USB_OTGFS_HCTSIZ0_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  For an OUT, this field is the number of data bytes the host sends during the
 *  transfer.
 *
 *  For an IN, this field is the buffer size that the application has Reserved for the
 *  transfer. The application is expected to program this field as an integer multiple
 *  of the maximum packet size for IN transactions (periodic and non-periodic).
 *
 *  The width of this counter is specified as Width of Transfer Size Counters during
 *  coreConsultant configuration (parameter OTG_TRANS_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ0_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_HCTSIZ0_XFERSIZE_M  (USB_OTGFS_HCTSIZ0_XFERSIZE_V << USB_OTGFS_HCTSIZ0_XFERSIZE_S)
#define USB_OTGFS_HCTSIZ0_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_HCTSIZ0_XFERSIZE_S  0
/** USB_OTGFS_HCTSIZ0_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  This field is programmed by the application with the expected number of packets to
 *  be transmitted (OUT) or received (IN).
 *
 *  The host decrements this count on every successful transmission or reception of an
 *  OUT/IN packet. Once this count reaches zero, the application is interrupted to
 *  indicate normal completion.
 *
 *  The width of this counter is specified as Width of Packet Counters during
 *  coreConsultant configuration (parameter OTG_PACKET_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ0_PKTCNT    0x0000007FU
#define USB_OTGFS_HCTSIZ0_PKTCNT_M  (USB_OTGFS_HCTSIZ0_PKTCNT_V << USB_OTGFS_HCTSIZ0_PKTCNT_S)
#define USB_OTGFS_HCTSIZ0_PKTCNT_V  0x0000007FU
#define USB_OTGFS_HCTSIZ0_PKTCNT_S  19
/** USB_OTGFS_HCTSIZ0_PID : R/W; bitpos: [30:29]; default: 0;
 *  PID (Pid)
 *
 *  The application programs this field with the type of PID to use for the initial
 *  transaction. The host maintains this field for the rest of the transfer.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_OTGFS_HCTSIZ0_PID    0x00000003U
#define USB_OTGFS_HCTSIZ0_PID_M  (USB_OTGFS_HCTSIZ0_PID_V << USB_OTGFS_HCTSIZ0_PID_S)
#define USB_OTGFS_HCTSIZ0_PID_V  0x00000003U
#define USB_OTGFS_HCTSIZ0_PID_S  29
/** USB_OTGFS_HCTSIZ0_DOPNG : R/W; bitpos: [31]; default: 0;
 *  Do Ping (DoPng)
 *
 *  This bit is used only for OUT transfers.
 *  Setting this field to 1 directs the host to do PING protocol.
 *
 *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
 *  it disables the channel.
 */
#define USB_OTGFS_HCTSIZ0_DOPNG    (BIT(31))
#define USB_OTGFS_HCTSIZ0_DOPNG_M  (USB_OTGFS_HCTSIZ0_DOPNG_V << USB_OTGFS_HCTSIZ0_DOPNG_S)
#define USB_OTGFS_HCTSIZ0_DOPNG_V  0x00000001U
#define USB_OTGFS_HCTSIZ0_DOPNG_S  31

/** USB_OTGFS_HCDMA0_REG register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
#define USB_OTGFS_HCDMA0_REG(i) (DR_REG_USB_BASE(i) + 0x514)
/** USB_OTGFS_HCDMA0_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  In Buffer DMA Mode:
 *
 *  [31:0]: DMA Address (DMAAddr)
 *
 *  This field holds the start address in the external memory from which the data for
 *  the endpoint must be fetched or to which it must be stored. This register is
 *  incremented on every AHB transaction.
 *
 *  Reset: X if not programmed as the register is in SPRAM.
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *
 *  [31:9]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the start address of the 512 bytes page. The first descriptor in
 *  the list should be located in this address. The first descriptor may be or may not
 *  be ready. The core starts processing the list from the CTD value.
 *
 *  [8:3]: Current Transfer Desc(CTD)
 *
 *  This value is in terms of number of descriptors. The values can be from 0 to 63.
 *  - 0 -  1 descriptor.
 *  - 63 - 64 descriptors.
 *  This field indicates the current descriptor processed in the list. This field is
 *  updated both by application and the core. For example, if the application enables
 *  the channel after programming CTD=5, then the core will start processing the sixth
 *  descriptor. The address is obtained by adding a value of (8bytes*5=) 40(decimal) to
 *  DMAAddr.
 *
 *  Reset: 6'h0
 *
 *  [2:0]: Reserved
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Isochronous:
 *
 *  [31:N]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the address of the 2*(nTD+1) bytes of locations in which the
 *  isochronous descriptors are present where N is based on nTD as follows:
 *  - [31:N]: Base Address
 *  - [N-1:3]: Offset
 *  - [2:0]: 000
 *  For HS ISOC, if nTD is,
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  - 127, N=10
 *  - 255, N=11
 *  For FS ISOC, if nTD is,
 *  - 1, N=4
 *  - 3, N=5
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  [N-1:3]: Current Transfer Desc(CTD)
 *
 *  CTD for isochronous is based on the current frame/(micro)frame value. Need to be
 *  set to zero by application.
 *
 *  Reset: (N+1:3)'h0
 *
 *  [2:0]: Reserved
 */
#define USB_OTGFS_HCDMA0_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_HCDMA0_DMAADDR_M  (USB_OTGFS_HCDMA0_DMAADDR_V << USB_OTGFS_HCDMA0_DMAADDR_S)
#define USB_OTGFS_HCDMA0_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMA0_DMAADDR_S  0

/** USB_OTGFS_HCDMAB0_REG register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
#define USB_OTGFS_HCDMAB0_REG(i) (DR_REG_USB_BASE(i) + 0x51c)
/** USB_OTGFS_HCDMAB0_HCDMAB : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.
 *  This register is updated as and when the data transfer for the corresponding end
 *  point
 *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
 *  this
 *  field is reserved.
 */
#define USB_OTGFS_HCDMAB0_HCDMAB    0xFFFFFFFFU
#define USB_OTGFS_HCDMAB0_HCDMAB_M  (USB_OTGFS_HCDMAB0_HCDMAB_V << USB_OTGFS_HCDMAB0_HCDMAB_S)
#define USB_OTGFS_HCDMAB0_HCDMAB_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMAB0_HCDMAB_S  0

/** USB_OTGFS_HCCHAR1_REG register
 *  Host Channel 1 Characteristics Register
 */
#define USB_OTGFS_HCCHAR1_REG(i) (DR_REG_USB_BASE(i) + 0x520)
/** USB_OTGFS_HCCHAR1_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  Indicates the maximum packet size of the associated endpoint.
 */
#define USB_OTGFS_HCCHAR1_MPS    0x000007FFU
#define USB_OTGFS_HCCHAR1_MPS_M  (USB_OTGFS_HCCHAR1_MPS_V << USB_OTGFS_HCCHAR1_MPS_S)
#define USB_OTGFS_HCCHAR1_MPS_V  0x000007FFU
#define USB_OTGFS_HCCHAR1_MPS_S  0
/** USB_OTGFS_HCCHAR1_EPNUM : R/W; bitpos: [14:11]; default: 0;
 *  Endpoint Number (EPNum)
 *
 *  Indicates the endpoint number on the device serving as the data source or sink.
 */
#define USB_OTGFS_HCCHAR1_EPNUM    0x0000000FU
#define USB_OTGFS_HCCHAR1_EPNUM_M  (USB_OTGFS_HCCHAR1_EPNUM_V << USB_OTGFS_HCCHAR1_EPNUM_S)
#define USB_OTGFS_HCCHAR1_EPNUM_V  0x0000000FU
#define USB_OTGFS_HCCHAR1_EPNUM_S  11
/** USB_OTGFS_HCCHAR1_EPDIR : R/W; bitpos: [15]; default: 0;
 *  Endpoint Direction (EPDir)
 *
 *  Indicates whether the transaction is IN or OUT.
 *  - 1'b0: OUT
 *  - 1'b1: IN
 */
#define USB_OTGFS_HCCHAR1_EPDIR    (BIT(15))
#define USB_OTGFS_HCCHAR1_EPDIR_M  (USB_OTGFS_HCCHAR1_EPDIR_V << USB_OTGFS_HCCHAR1_EPDIR_S)
#define USB_OTGFS_HCCHAR1_EPDIR_V  0x00000001U
#define USB_OTGFS_HCCHAR1_EPDIR_S  15
/** USB_OTGFS_HCCHAR1_LSPDDEV : R/W; bitpos: [17]; default: 0;
 *  Low-Speed Device (LSpdDev)
 *
 *  This field is Set by the application to indicate that this channel is communicating
 *  to a low-speed device.
 *
 *  The application must program this bit when a low speed device is connected to the
 *  host through an FS HUB. The DWC_otg Host core uses this field to drive the
 *  XCVR_SELECT signal to 2'b11 while communicating to the LS Device through the FS hub.
 *
 *  Note: In a peer to peer setup, the DWC_otg Host core ignores this bit even if it is
 *  set by the application software.
 */
#define USB_OTGFS_HCCHAR1_LSPDDEV    (BIT(17))
#define USB_OTGFS_HCCHAR1_LSPDDEV_M  (USB_OTGFS_HCCHAR1_LSPDDEV_V << USB_OTGFS_HCCHAR1_LSPDDEV_S)
#define USB_OTGFS_HCCHAR1_LSPDDEV_V  0x00000001U
#define USB_OTGFS_HCCHAR1_LSPDDEV_S  17
/** USB_OTGFS_HCCHAR1_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Indicates the transfer type selected.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_HCCHAR1_EPTYPE    0x00000003U
#define USB_OTGFS_HCCHAR1_EPTYPE_M  (USB_OTGFS_HCCHAR1_EPTYPE_V << USB_OTGFS_HCCHAR1_EPTYPE_S)
#define USB_OTGFS_HCCHAR1_EPTYPE_V  0x00000003U
#define USB_OTGFS_HCCHAR1_EPTYPE_S  18
/** USB_OTGFS_HCCHAR1_EC : R/W; bitpos: [21:20]; default: 0;
 *  Multi Count (MC) / Error Count (EC)
 *
 *  When the Split Enable bit of the Host Channel-n Split Control
 *  register (HCSPLTn.SpltEna) is reset (1'b0), this field indicates to
 *  the host the number of transactions that must be executed per
 *  microframe for this periodic endpoint. For non periodic transfers,
 *  this field is used only in DMA mode, and specifies the number
 *  packets to be fetched for this channel before the internal DMA
 *  engine changes arbitration.
 *  - 2'b00: Reserved This field yields undefined results.
 *  - 2'b01: 1 transaction
 *  - 2'b10: 2 transactions to be issued for this endpoint per microframe
 *  - 2'b11: 3 transactions to be issued for this endpoint per microframe
 *  When HCSPLTn.SpltEna is Set (1'b1), this field indicates the
 *  number of immediate retries to be performed for a periodic split
 *  transactions on transaction errors. This field must be Set to at
 *  least 2'b01.
 */
#define USB_OTGFS_HCCHAR1_EC    0x00000003U
#define USB_OTGFS_HCCHAR1_EC_M  (USB_OTGFS_HCCHAR1_EC_V << USB_OTGFS_HCCHAR1_EC_S)
#define USB_OTGFS_HCCHAR1_EC_V  0x00000003U
#define USB_OTGFS_HCCHAR1_EC_S  20
/** USB_OTGFS_HCCHAR1_DEVADDR : R/W; bitpos: [28:22]; default: 0;
 *  Device Address (DevAddr)
 *
 *  This field selects the specific device serving as the data source
 *  or sink.
 */
#define USB_OTGFS_HCCHAR1_DEVADDR    0x0000007FU
#define USB_OTGFS_HCCHAR1_DEVADDR_M  (USB_OTGFS_HCCHAR1_DEVADDR_V << USB_OTGFS_HCCHAR1_DEVADDR_S)
#define USB_OTGFS_HCCHAR1_DEVADDR_V  0x0000007FU
#define USB_OTGFS_HCCHAR1_DEVADDR_S  22
/** USB_OTGFS_HCCHAR1_ODDFRM : R/W; bitpos: [29]; default: 0;
 *  Odd Frame (OddFrm)
 *
 *  This field is set (reset) by the application to indicate that the OTG host must
 *  perform
 *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
 *  (isochronous and interrupt) transactions.
 *  - 1'b0: Even (micro)Frame
 *  - 1'b1: Odd (micro)Frame
 */
#define USB_OTGFS_HCCHAR1_ODDFRM    (BIT(29))
#define USB_OTGFS_HCCHAR1_ODDFRM_M  (USB_OTGFS_HCCHAR1_ODDFRM_V << USB_OTGFS_HCCHAR1_ODDFRM_S)
#define USB_OTGFS_HCCHAR1_ODDFRM_V  0x00000001U
#define USB_OTGFS_HCCHAR1_ODDFRM_S  29
/** USB_OTGFS_HCCHAR1_CHDIS : R/W; bitpos: [30]; default: 0;
 *  Channel Disable (ChDis)
 *
 *  The application sets this bit to stop transmitting/receiving data
 *  on a channel, even before the transfer for that channel is
 *  complete. The application must wait for the Channel Disabled
 *  interrupt before treating the channel as disabled.
 */
#define USB_OTGFS_HCCHAR1_CHDIS    (BIT(30))
#define USB_OTGFS_HCCHAR1_CHDIS_M  (USB_OTGFS_HCCHAR1_CHDIS_V << USB_OTGFS_HCCHAR1_CHDIS_S)
#define USB_OTGFS_HCCHAR1_CHDIS_V  0x00000001U
#define USB_OTGFS_HCCHAR1_CHDIS_S  30
/** USB_OTGFS_HCCHAR1_CHENA : R/W; bitpos: [31]; default: 0;
 *  Channel Enable (ChEna)
 *
 *  When Scatter/Gather mode is enabled
 *  - 1'b0: Indicates that the descriptor structure is not yet ready.
 *  - 1'b1:  Indicates  that  the  descriptor  structure  and  data  buffer  with data
 *  is setup and this channel can access the descriptor.
 *  When Scatter/Gather mode is disabled
 *
 *  This field is set by the application and cleared by the OTG host.
 *  - 1'b0: Channel disabled
 *  - 1'b1: Channel enabled
 */
#define USB_OTGFS_HCCHAR1_CHENA    (BIT(31))
#define USB_OTGFS_HCCHAR1_CHENA_M  (USB_OTGFS_HCCHAR1_CHENA_V << USB_OTGFS_HCCHAR1_CHENA_S)
#define USB_OTGFS_HCCHAR1_CHENA_V  0x00000001U
#define USB_OTGFS_HCCHAR1_CHENA_S  31

/** USB_OTGFS_HCINT1_REG register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
#define USB_OTGFS_HCINT1_REG(i) (DR_REG_USB_BASE(i) + 0x528)
/** USB_OTGFS_HCINT1_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed (XferCompl)
 *
 *  Transfer completed normally without any errors.This bit can be set only by the core
 *  and the application should write 1 to clear it.
 *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
 *  completed with IOC bit set in its descriptor.
 *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
 *  without any errors.
 */
#define USB_OTGFS_HCINT1_XFERCOMPL    (BIT(0))
#define USB_OTGFS_HCINT1_XFERCOMPL_M  (USB_OTGFS_HCINT1_XFERCOMPL_V << USB_OTGFS_HCINT1_XFERCOMPL_S)
#define USB_OTGFS_HCINT1_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_HCINT1_XFERCOMPL_S  0
/** USB_OTGFS_HCINT1_CHHLTD : R/W; bitpos: [1]; default: 0;
 *  Channel Halted (ChHltd)
 *
 *  In non Scatter/Gather DMA mode, it indicates the transfer completed abnormally
 *  either because of any USB transaction error or in response to disable request by
 *  the application or because of a completed transfer.
 *
 *  In Scatter/gather DMA mode, this indicates that transfer completed due to any of
 *  the following
 *  - EOL being set in descriptor
 *  - AHB error
 *  - Excessive transaction errors
 *  - Babble
 *  - Stall
 */
#define USB_OTGFS_HCINT1_CHHLTD    (BIT(1))
#define USB_OTGFS_HCINT1_CHHLTD_M  (USB_OTGFS_HCINT1_CHHLTD_V << USB_OTGFS_HCINT1_CHHLTD_S)
#define USB_OTGFS_HCINT1_CHHLTD_V  0x00000001U
#define USB_OTGFS_HCINT1_CHHLTD_S  1
/** USB_OTGFS_HCINT1_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  This is generated only in Internal DMA mode when there is an AHB error during AHB
 *  read/write. The application can read the corresponding channel's DMA address
 *  register to get the error address.
 */
#define USB_OTGFS_HCINT1_AHBERR    (BIT(2))
#define USB_OTGFS_HCINT1_AHBERR_M  (USB_OTGFS_HCINT1_AHBERR_V << USB_OTGFS_HCINT1_AHBERR_S)
#define USB_OTGFS_HCINT1_AHBERR_V  0x00000001U
#define USB_OTGFS_HCINT1_AHBERR_S  2
/** USB_OTGFS_HCINT1_STALL : R/W; bitpos: [3]; default: 0;
 *  STALL Response Received Interrupt (STALL)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT1_STALL    (BIT(3))
#define USB_OTGFS_HCINT1_STALL_M  (USB_OTGFS_HCINT1_STALL_V << USB_OTGFS_HCINT1_STALL_S)
#define USB_OTGFS_HCINT1_STALL_V  0x00000001U
#define USB_OTGFS_HCINT1_STALL_S  3
/** USB_OTGFS_HCINT1_NAK : R/W; bitpos: [4]; default: 0;
 *  NAK Response Received Interrupt (NAK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT1_NAK    (BIT(4))
#define USB_OTGFS_HCINT1_NAK_M  (USB_OTGFS_HCINT1_NAK_V << USB_OTGFS_HCINT1_NAK_S)
#define USB_OTGFS_HCINT1_NAK_V  0x00000001U
#define USB_OTGFS_HCINT1_NAK_S  4
/** USB_OTGFS_HCINT1_ACK : R/W; bitpos: [5]; default: 0;
 *  ACK Response Received/Transmitted Interrupt (ACK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT1_ACK    (BIT(5))
#define USB_OTGFS_HCINT1_ACK_M  (USB_OTGFS_HCINT1_ACK_V << USB_OTGFS_HCINT1_ACK_S)
#define USB_OTGFS_HCINT1_ACK_V  0x00000001U
#define USB_OTGFS_HCINT1_ACK_S  5
/** USB_OTGFS_HCINT1_NYET : R/W; bitpos: [6]; default: 0;
 *  NYET Response Received Interrupt (NYET)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT1_NYET    (BIT(6))
#define USB_OTGFS_HCINT1_NYET_M  (USB_OTGFS_HCINT1_NYET_V << USB_OTGFS_HCINT1_NYET_S)
#define USB_OTGFS_HCINT1_NYET_V  0x00000001U
#define USB_OTGFS_HCINT1_NYET_S  6
/** USB_OTGFS_HCINT1_XACTERR : R/W; bitpos: [7]; default: 0;
 *  Transaction Error (XactErr)
 *
 *  Indicates one of the following errors occurred on the USB.
 *  - CRC check failure
 *  - Timeout
 *  - Bit stuff error
 *  - False EOP
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT1_XACTERR    (BIT(7))
#define USB_OTGFS_HCINT1_XACTERR_M  (USB_OTGFS_HCINT1_XACTERR_V << USB_OTGFS_HCINT1_XACTERR_S)
#define USB_OTGFS_HCINT1_XACTERR_V  0x00000001U
#define USB_OTGFS_HCINT1_XACTERR_S  7
/** USB_OTGFS_HCINT1_BBLERR : R/W; bitpos: [8]; default: 0;
 *  Babble Error (BblErr)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
 *  This bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_HCINT1_BBLERR    (BIT(8))
#define USB_OTGFS_HCINT1_BBLERR_M  (USB_OTGFS_HCINT1_BBLERR_V << USB_OTGFS_HCINT1_BBLERR_S)
#define USB_OTGFS_HCINT1_BBLERR_V  0x00000001U
#define USB_OTGFS_HCINT1_BBLERR_S  8
/** USB_OTGFS_HCINT1_FRMOVRUN : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun (FrmOvrun).
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core. This bit can be set only by the core and the application should write
 *  1 to clear
 *  it.
 */
#define USB_OTGFS_HCINT1_FRMOVRUN    (BIT(9))
#define USB_OTGFS_HCINT1_FRMOVRUN_M  (USB_OTGFS_HCINT1_FRMOVRUN_V << USB_OTGFS_HCINT1_FRMOVRUN_S)
#define USB_OTGFS_HCINT1_FRMOVRUN_V  0x00000001U
#define USB_OTGFS_HCINT1_FRMOVRUN_S  9
/** USB_OTGFS_HCINT1_DATATGLERR : R/W; bitpos: [10]; default: 0;
 *
 *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
 *  application should write 1 to clear
 *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core.
 */
#define USB_OTGFS_HCINT1_DATATGLERR    (BIT(10))
#define USB_OTGFS_HCINT1_DATATGLERR_M  (USB_OTGFS_HCINT1_DATATGLERR_V << USB_OTGFS_HCINT1_DATATGLERR_S)
#define USB_OTGFS_HCINT1_DATATGLERR_V  0x00000001U
#define USB_OTGFS_HCINT1_DATATGLERR_S  10
/** USB_OTGFS_HCINT1_BNAINTR : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process. BNA will not be generated
 *  for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT1_BNAINTR    (BIT(11))
#define USB_OTGFS_HCINT1_BNAINTR_M  (USB_OTGFS_HCINT1_BNAINTR_V << USB_OTGFS_HCINT1_BNAINTR_S)
#define USB_OTGFS_HCINT1_BNAINTR_V  0x00000001U
#define USB_OTGFS_HCINT1_BNAINTR_S  11
/** USB_OTGFS_HCINT1_XCS_XACT_ERR : R/W; bitpos: [12]; default: 0;
 *  Excessive Transaction Error (XCS_XACT_ERR)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
 *  not be generated for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT1_XCS_XACT_ERR    (BIT(12))
#define USB_OTGFS_HCINT1_XCS_XACT_ERR_M  (USB_OTGFS_HCINT1_XCS_XACT_ERR_V << USB_OTGFS_HCINT1_XCS_XACT_ERR_S)
#define USB_OTGFS_HCINT1_XCS_XACT_ERR_V  0x00000001U
#define USB_OTGFS_HCINT1_XCS_XACT_ERR_S  12
/** USB_OTGFS_HCINT1_DESC_LST_ROLLINTR : R/W; bitpos: [13]; default: 0;
 *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when the corresponding channel's descriptor list rolls over.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT1_DESC_LST_ROLLINTR    (BIT(13))
#define USB_OTGFS_HCINT1_DESC_LST_ROLLINTR_M  (USB_OTGFS_HCINT1_DESC_LST_ROLLINTR_V << USB_OTGFS_HCINT1_DESC_LST_ROLLINTR_S)
#define USB_OTGFS_HCINT1_DESC_LST_ROLLINTR_V  0x00000001U
#define USB_OTGFS_HCINT1_DESC_LST_ROLLINTR_S  13

/** USB_OTGFS_HCINTMSK1_REG register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
#define USB_OTGFS_HCINTMSK1_REG(i) (DR_REG_USB_BASE(i) + 0x52c)
/** USB_OTGFS_HCINTMSK1_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *
 *  Transfer Completed Mask (XferComplMsk)
 */
#define USB_OTGFS_HCINTMSK1_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_HCINTMSK1_XFERCOMPLMSK_M  (USB_OTGFS_HCINTMSK1_XFERCOMPLMSK_V << USB_OTGFS_HCINTMSK1_XFERCOMPLMSK_S)
#define USB_OTGFS_HCINTMSK1_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK1_XFERCOMPLMSK_S  0
/** USB_OTGFS_HCINTMSK1_CHHLTDMSK : R/W; bitpos: [1]; default: 0;
 *
 *  Channel Halted Mask (ChHltdMsk)
 */
#define USB_OTGFS_HCINTMSK1_CHHLTDMSK    (BIT(1))
#define USB_OTGFS_HCINTMSK1_CHHLTDMSK_M  (USB_OTGFS_HCINTMSK1_CHHLTDMSK_V << USB_OTGFS_HCINTMSK1_CHHLTDMSK_S)
#define USB_OTGFS_HCINTMSK1_CHHLTDMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK1_CHHLTDMSK_S  1
/** USB_OTGFS_HCINTMSK1_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *
 *  AHB Error Mask (AHBErrMsk)
 *  In  scatter/gather  DMA  mode  for  host,
 *  interrupts will not be generated due to the corresponding bits set in
 *  HCINTn.
 */
#define USB_OTGFS_HCINTMSK1_AHBERRMSK    (BIT(2))
#define USB_OTGFS_HCINTMSK1_AHBERRMSK_M  (USB_OTGFS_HCINTMSK1_AHBERRMSK_V << USB_OTGFS_HCINTMSK1_AHBERRMSK_S)
#define USB_OTGFS_HCINTMSK1_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK1_AHBERRMSK_S  2
/** USB_OTGFS_HCINTMSK1_BNAINTRMSK : R/W; bitpos: [11]; default: 0;
 *
 *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK1_BNAINTRMSK    (BIT(11))
#define USB_OTGFS_HCINTMSK1_BNAINTRMSK_M  (USB_OTGFS_HCINTMSK1_BNAINTRMSK_V << USB_OTGFS_HCINTMSK1_BNAINTRMSK_S)
#define USB_OTGFS_HCINTMSK1_BNAINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK1_BNAINTRMSK_S  11
/** USB_OTGFS_HCINTMSK1_DESC_LST_ROLLINTRMSK : R/W; bitpos: [13]; default: 0;
 *
 *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK1_DESC_LST_ROLLINTRMSK    (BIT(13))
#define USB_OTGFS_HCINTMSK1_DESC_LST_ROLLINTRMSK_M  (USB_OTGFS_HCINTMSK1_DESC_LST_ROLLINTRMSK_V << USB_OTGFS_HCINTMSK1_DESC_LST_ROLLINTRMSK_S)
#define USB_OTGFS_HCINTMSK1_DESC_LST_ROLLINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK1_DESC_LST_ROLLINTRMSK_S  13

/** USB_OTGFS_HCTSIZ1_REG register
 *  Host Channel 1 Transfer Size Register
 */
#define USB_OTGFS_HCTSIZ1_REG(i) (DR_REG_USB_BASE(i) + 0x530)
/** USB_OTGFS_HCTSIZ1_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  For an OUT, this field is the number of data bytes the host sends during the
 *  transfer.
 *
 *  For an IN, this field is the buffer size that the application has Reserved for the
 *  transfer. The application is expected to program this field as an integer multiple
 *  of the maximum packet size for IN transactions (periodic and non-periodic).
 *
 *  The width of this counter is specified as Width of Transfer Size Counters during
 *  coreConsultant configuration (parameter OTG_TRANS_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ1_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_HCTSIZ1_XFERSIZE_M  (USB_OTGFS_HCTSIZ1_XFERSIZE_V << USB_OTGFS_HCTSIZ1_XFERSIZE_S)
#define USB_OTGFS_HCTSIZ1_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_HCTSIZ1_XFERSIZE_S  0
/** USB_OTGFS_HCTSIZ1_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  This field is programmed by the application with the expected number of packets to
 *  be transmitted (OUT) or received (IN).
 *
 *  The host decrements this count on every successful transmission or reception of an
 *  OUT/IN packet. Once this count reaches zero, the application is interrupted to
 *  indicate normal completion.
 *
 *  The width of this counter is specified as Width of Packet Counters during
 *  coreConsultant configuration (parameter OTG_PACKET_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ1_PKTCNT    0x0000007FU
#define USB_OTGFS_HCTSIZ1_PKTCNT_M  (USB_OTGFS_HCTSIZ1_PKTCNT_V << USB_OTGFS_HCTSIZ1_PKTCNT_S)
#define USB_OTGFS_HCTSIZ1_PKTCNT_V  0x0000007FU
#define USB_OTGFS_HCTSIZ1_PKTCNT_S  19
/** USB_OTGFS_HCTSIZ1_PID : R/W; bitpos: [30:29]; default: 0;
 *  PID (Pid)
 *
 *  The application programs this field with the type of PID to use for the initial
 *  transaction. The host maintains this field for the rest of the transfer.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_OTGFS_HCTSIZ1_PID    0x00000003U
#define USB_OTGFS_HCTSIZ1_PID_M  (USB_OTGFS_HCTSIZ1_PID_V << USB_OTGFS_HCTSIZ1_PID_S)
#define USB_OTGFS_HCTSIZ1_PID_V  0x00000003U
#define USB_OTGFS_HCTSIZ1_PID_S  29
/** USB_OTGFS_HCTSIZ1_DOPNG : R/W; bitpos: [31]; default: 0;
 *  Do Ping (DoPng)
 *
 *  This bit is used only for OUT transfers.
 *  Setting this field to 1 directs the host to do PING protocol.
 *
 *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
 *  it disables the channel.
 */
#define USB_OTGFS_HCTSIZ1_DOPNG    (BIT(31))
#define USB_OTGFS_HCTSIZ1_DOPNG_M  (USB_OTGFS_HCTSIZ1_DOPNG_V << USB_OTGFS_HCTSIZ1_DOPNG_S)
#define USB_OTGFS_HCTSIZ1_DOPNG_V  0x00000001U
#define USB_OTGFS_HCTSIZ1_DOPNG_S  31

/** USB_OTGFS_HCDMA1_REG register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
#define USB_OTGFS_HCDMA1_REG(i) (DR_REG_USB_BASE(i) + 0x534)
/** USB_OTGFS_HCDMA1_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  In Buffer DMA Mode:
 *
 *  [31:0]: DMA Address (DMAAddr)
 *
 *  This field holds the start address in the external memory from which the data for
 *  the endpoint must be fetched or to which it must be stored. This register is
 *  incremented on every AHB transaction.
 *
 *  Reset: X if not programmed as the register is in SPRAM.
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *
 *  [31:9]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the start address of the 512 bytes page. The first descriptor in
 *  the list should be located in this address. The first descriptor may be or may not
 *  be ready. The core starts processing the list from the CTD value.
 *
 *  [8:3]: Current Transfer Desc(CTD)
 *
 *  This value is in terms of number of descriptors. The values can be from 0 to 63.
 *  - 0 -  1 descriptor.
 *  - 63 - 64 descriptors.
 *  This field indicates the current descriptor processed in the list. This field is
 *  updated both by application and the core. For example, if the application enables
 *  the channel after programming CTD=5, then the core will start processing the sixth
 *  descriptor. The address is obtained by adding a value of (8bytes*5=) 40(decimal) to
 *  DMAAddr.
 *
 *  Reset: 6'h0
 *
 *  [2:0]: Reserved
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Isochronous:
 *
 *  [31:N]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the address of the 2*(nTD+1) bytes of locations in which the
 *  isochronous descriptors are present where N is based on nTD as follows:
 *  - [31:N]: Base Address
 *  - [N-1:3]: Offset
 *  - [2:0]: 000
 *  For HS ISOC, if nTD is,
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  - 127, N=10
 *  - 255, N=11
 *  For FS ISOC, if nTD is,
 *  - 1, N=4
 *  - 3, N=5
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  [N-1:3]: Current Transfer Desc(CTD)
 *
 *  CTD for isochronous is based on the current frame/(micro)frame value. Need to be
 *  set to zero by application.
 *
 *  Reset: (N+1:3)'h0
 *
 *  [2:0]: Reserved
 */
#define USB_OTGFS_HCDMA1_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_HCDMA1_DMAADDR_M  (USB_OTGFS_HCDMA1_DMAADDR_V << USB_OTGFS_HCDMA1_DMAADDR_S)
#define USB_OTGFS_HCDMA1_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMA1_DMAADDR_S  0

/** USB_OTGFS_HCDMAB1_REG register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
#define USB_OTGFS_HCDMAB1_REG(i) (DR_REG_USB_BASE(i) + 0x53c)
/** USB_OTGFS_HCDMAB1_HCDMAB : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.
 *  This register is updated as and when the data transfer for the corresponding end
 *  point
 *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
 *  this
 *  field is reserved.
 */
#define USB_OTGFS_HCDMAB1_HCDMAB    0xFFFFFFFFU
#define USB_OTGFS_HCDMAB1_HCDMAB_M  (USB_OTGFS_HCDMAB1_HCDMAB_V << USB_OTGFS_HCDMAB1_HCDMAB_S)
#define USB_OTGFS_HCDMAB1_HCDMAB_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMAB1_HCDMAB_S  0

/** USB_OTGFS_HCCHAR2_REG register
 *  Host Channel 2 Characteristics Register
 */
#define USB_OTGFS_HCCHAR2_REG(i) (DR_REG_USB_BASE(i) + 0x540)
/** USB_OTGFS_HCCHAR2_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  Indicates the maximum packet size of the associated endpoint.
 */
#define USB_OTGFS_HCCHAR2_MPS    0x000007FFU
#define USB_OTGFS_HCCHAR2_MPS_M  (USB_OTGFS_HCCHAR2_MPS_V << USB_OTGFS_HCCHAR2_MPS_S)
#define USB_OTGFS_HCCHAR2_MPS_V  0x000007FFU
#define USB_OTGFS_HCCHAR2_MPS_S  0
/** USB_OTGFS_HCCHAR2_EPNUM : R/W; bitpos: [14:11]; default: 0;
 *  Endpoint Number (EPNum)
 *
 *  Indicates the endpoint number on the device serving as the data source or sink.
 */
#define USB_OTGFS_HCCHAR2_EPNUM    0x0000000FU
#define USB_OTGFS_HCCHAR2_EPNUM_M  (USB_OTGFS_HCCHAR2_EPNUM_V << USB_OTGFS_HCCHAR2_EPNUM_S)
#define USB_OTGFS_HCCHAR2_EPNUM_V  0x0000000FU
#define USB_OTGFS_HCCHAR2_EPNUM_S  11
/** USB_OTGFS_HCCHAR2_EPDIR : R/W; bitpos: [15]; default: 0;
 *  Endpoint Direction (EPDir)
 *
 *  Indicates whether the transaction is IN or OUT.
 *  - 1'b0: OUT
 *  - 1'b1: IN
 */
#define USB_OTGFS_HCCHAR2_EPDIR    (BIT(15))
#define USB_OTGFS_HCCHAR2_EPDIR_M  (USB_OTGFS_HCCHAR2_EPDIR_V << USB_OTGFS_HCCHAR2_EPDIR_S)
#define USB_OTGFS_HCCHAR2_EPDIR_V  0x00000001U
#define USB_OTGFS_HCCHAR2_EPDIR_S  15
/** USB_OTGFS_HCCHAR2_LSPDDEV : R/W; bitpos: [17]; default: 0;
 *  Low-Speed Device (LSpdDev)
 *
 *  This field is Set by the application to indicate that this channel is communicating
 *  to a low-speed device.
 *
 *  The application must program this bit when a low speed device is connected to the
 *  host through an FS HUB. The DWC_otg Host core uses this field to drive the
 *  XCVR_SELECT signal to 2'b11 while communicating to the LS Device through the FS hub.
 *
 *  Note: In a peer to peer setup, the DWC_otg Host core ignores this bit even if it is
 *  set by the application software.
 */
#define USB_OTGFS_HCCHAR2_LSPDDEV    (BIT(17))
#define USB_OTGFS_HCCHAR2_LSPDDEV_M  (USB_OTGFS_HCCHAR2_LSPDDEV_V << USB_OTGFS_HCCHAR2_LSPDDEV_S)
#define USB_OTGFS_HCCHAR2_LSPDDEV_V  0x00000001U
#define USB_OTGFS_HCCHAR2_LSPDDEV_S  17
/** USB_OTGFS_HCCHAR2_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Indicates the transfer type selected.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_HCCHAR2_EPTYPE    0x00000003U
#define USB_OTGFS_HCCHAR2_EPTYPE_M  (USB_OTGFS_HCCHAR2_EPTYPE_V << USB_OTGFS_HCCHAR2_EPTYPE_S)
#define USB_OTGFS_HCCHAR2_EPTYPE_V  0x00000003U
#define USB_OTGFS_HCCHAR2_EPTYPE_S  18
/** USB_OTGFS_HCCHAR2_EC : R/W; bitpos: [21:20]; default: 0;
 *  Multi Count (MC) / Error Count (EC)
 *
 *  When the Split Enable bit of the Host Channel-n Split Control
 *  register (HCSPLTn.SpltEna) is reset (1'b0), this field indicates to
 *  the host the number of transactions that must be executed per
 *  microframe for this periodic endpoint. For non periodic transfers,
 *  this field is used only in DMA mode, and specifies the number
 *  packets to be fetched for this channel before the internal DMA
 *  engine changes arbitration.
 *  - 2'b00: Reserved This field yields undefined results.
 *  - 2'b01: 1 transaction
 *  - 2'b10: 2 transactions to be issued for this endpoint per microframe
 *  - 2'b11: 3 transactions to be issued for this endpoint per microframe
 *  When HCSPLTn.SpltEna is Set (1'b1), this field indicates the
 *  number of immediate retries to be performed for a periodic split
 *  transactions on transaction errors. This field must be Set to at
 *  least 2'b01.
 */
#define USB_OTGFS_HCCHAR2_EC    0x00000003U
#define USB_OTGFS_HCCHAR2_EC_M  (USB_OTGFS_HCCHAR2_EC_V << USB_OTGFS_HCCHAR2_EC_S)
#define USB_OTGFS_HCCHAR2_EC_V  0x00000003U
#define USB_OTGFS_HCCHAR2_EC_S  20
/** USB_OTGFS_HCCHAR2_DEVADDR : R/W; bitpos: [28:22]; default: 0;
 *  Device Address (DevAddr)
 *
 *  This field selects the specific device serving as the data source
 *  or sink.
 */
#define USB_OTGFS_HCCHAR2_DEVADDR    0x0000007FU
#define USB_OTGFS_HCCHAR2_DEVADDR_M  (USB_OTGFS_HCCHAR2_DEVADDR_V << USB_OTGFS_HCCHAR2_DEVADDR_S)
#define USB_OTGFS_HCCHAR2_DEVADDR_V  0x0000007FU
#define USB_OTGFS_HCCHAR2_DEVADDR_S  22
/** USB_OTGFS_HCCHAR2_ODDFRM : R/W; bitpos: [29]; default: 0;
 *  Odd Frame (OddFrm)
 *
 *  This field is set (reset) by the application to indicate that the OTG host must
 *  perform
 *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
 *  (isochronous and interrupt) transactions.
 *  - 1'b0: Even (micro)Frame
 *  - 1'b1: Odd (micro)Frame
 */
#define USB_OTGFS_HCCHAR2_ODDFRM    (BIT(29))
#define USB_OTGFS_HCCHAR2_ODDFRM_M  (USB_OTGFS_HCCHAR2_ODDFRM_V << USB_OTGFS_HCCHAR2_ODDFRM_S)
#define USB_OTGFS_HCCHAR2_ODDFRM_V  0x00000001U
#define USB_OTGFS_HCCHAR2_ODDFRM_S  29
/** USB_OTGFS_HCCHAR2_CHDIS : R/W; bitpos: [30]; default: 0;
 *  Channel Disable (ChDis)
 *
 *  The application sets this bit to stop transmitting/receiving data
 *  on a channel, even before the transfer for that channel is
 *  complete. The application must wait for the Channel Disabled
 *  interrupt before treating the channel as disabled.
 */
#define USB_OTGFS_HCCHAR2_CHDIS    (BIT(30))
#define USB_OTGFS_HCCHAR2_CHDIS_M  (USB_OTGFS_HCCHAR2_CHDIS_V << USB_OTGFS_HCCHAR2_CHDIS_S)
#define USB_OTGFS_HCCHAR2_CHDIS_V  0x00000001U
#define USB_OTGFS_HCCHAR2_CHDIS_S  30
/** USB_OTGFS_HCCHAR2_CHENA : R/W; bitpos: [31]; default: 0;
 *  Channel Enable (ChEna)
 *
 *  When Scatter/Gather mode is enabled
 *  - 1'b0: Indicates that the descriptor structure is not yet ready.
 *  - 1'b1:  Indicates  that  the  descriptor  structure  and  data  buffer  with data
 *  is setup and this channel can access the descriptor.
 *  When Scatter/Gather mode is disabled
 *
 *  This field is set by the application and cleared by the OTG host.
 *  - 1'b0: Channel disabled
 *  - 1'b1: Channel enabled
 */
#define USB_OTGFS_HCCHAR2_CHENA    (BIT(31))
#define USB_OTGFS_HCCHAR2_CHENA_M  (USB_OTGFS_HCCHAR2_CHENA_V << USB_OTGFS_HCCHAR2_CHENA_S)
#define USB_OTGFS_HCCHAR2_CHENA_V  0x00000001U
#define USB_OTGFS_HCCHAR2_CHENA_S  31

/** USB_OTGFS_HCINT2_REG register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
#define USB_OTGFS_HCINT2_REG(i) (DR_REG_USB_BASE(i) + 0x548)
/** USB_OTGFS_HCINT2_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed (XferCompl)
 *
 *  Transfer completed normally without any errors.This bit can be set only by the core
 *  and the application should write 1 to clear it.
 *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
 *  completed with IOC bit set in its descriptor.
 *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
 *  without any errors.
 */
#define USB_OTGFS_HCINT2_XFERCOMPL    (BIT(0))
#define USB_OTGFS_HCINT2_XFERCOMPL_M  (USB_OTGFS_HCINT2_XFERCOMPL_V << USB_OTGFS_HCINT2_XFERCOMPL_S)
#define USB_OTGFS_HCINT2_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_HCINT2_XFERCOMPL_S  0
/** USB_OTGFS_HCINT2_CHHLTD : R/W; bitpos: [1]; default: 0;
 *  Channel Halted (ChHltd)
 *
 *  In non Scatter/Gather DMA mode, it indicates the transfer completed abnormally
 *  either because of any USB transaction error or in response to disable request by
 *  the application or because of a completed transfer.
 *
 *  In Scatter/gather DMA mode, this indicates that transfer completed due to any of
 *  the following
 *  - EOL being set in descriptor
 *  - AHB error
 *  - Excessive transaction errors
 *  - Babble
 *  - Stall
 */
#define USB_OTGFS_HCINT2_CHHLTD    (BIT(1))
#define USB_OTGFS_HCINT2_CHHLTD_M  (USB_OTGFS_HCINT2_CHHLTD_V << USB_OTGFS_HCINT2_CHHLTD_S)
#define USB_OTGFS_HCINT2_CHHLTD_V  0x00000001U
#define USB_OTGFS_HCINT2_CHHLTD_S  1
/** USB_OTGFS_HCINT2_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  This is generated only in Internal DMA mode when there is an AHB error during AHB
 *  read/write. The application can read the corresponding channel's DMA address
 *  register to get the error address.
 */
#define USB_OTGFS_HCINT2_AHBERR    (BIT(2))
#define USB_OTGFS_HCINT2_AHBERR_M  (USB_OTGFS_HCINT2_AHBERR_V << USB_OTGFS_HCINT2_AHBERR_S)
#define USB_OTGFS_HCINT2_AHBERR_V  0x00000001U
#define USB_OTGFS_HCINT2_AHBERR_S  2
/** USB_OTGFS_HCINT2_STALL : R/W; bitpos: [3]; default: 0;
 *  STALL Response Received Interrupt (STALL)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT2_STALL    (BIT(3))
#define USB_OTGFS_HCINT2_STALL_M  (USB_OTGFS_HCINT2_STALL_V << USB_OTGFS_HCINT2_STALL_S)
#define USB_OTGFS_HCINT2_STALL_V  0x00000001U
#define USB_OTGFS_HCINT2_STALL_S  3
/** USB_OTGFS_HCINT2_NAK : R/W; bitpos: [4]; default: 0;
 *  NAK Response Received Interrupt (NAK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT2_NAK    (BIT(4))
#define USB_OTGFS_HCINT2_NAK_M  (USB_OTGFS_HCINT2_NAK_V << USB_OTGFS_HCINT2_NAK_S)
#define USB_OTGFS_HCINT2_NAK_V  0x00000001U
#define USB_OTGFS_HCINT2_NAK_S  4
/** USB_OTGFS_HCINT2_ACK : R/W; bitpos: [5]; default: 0;
 *  ACK Response Received/Transmitted Interrupt (ACK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT2_ACK    (BIT(5))
#define USB_OTGFS_HCINT2_ACK_M  (USB_OTGFS_HCINT2_ACK_V << USB_OTGFS_HCINT2_ACK_S)
#define USB_OTGFS_HCINT2_ACK_V  0x00000001U
#define USB_OTGFS_HCINT2_ACK_S  5
/** USB_OTGFS_HCINT2_NYET : R/W; bitpos: [6]; default: 0;
 *  NYET Response Received Interrupt (NYET)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT2_NYET    (BIT(6))
#define USB_OTGFS_HCINT2_NYET_M  (USB_OTGFS_HCINT2_NYET_V << USB_OTGFS_HCINT2_NYET_S)
#define USB_OTGFS_HCINT2_NYET_V  0x00000001U
#define USB_OTGFS_HCINT2_NYET_S  6
/** USB_OTGFS_HCINT2_XACTERR : R/W; bitpos: [7]; default: 0;
 *  Transaction Error (XactErr)
 *
 *  Indicates one of the following errors occurred on the USB.
 *  - CRC check failure
 *  - Timeout
 *  - Bit stuff error
 *  - False EOP
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT2_XACTERR    (BIT(7))
#define USB_OTGFS_HCINT2_XACTERR_M  (USB_OTGFS_HCINT2_XACTERR_V << USB_OTGFS_HCINT2_XACTERR_S)
#define USB_OTGFS_HCINT2_XACTERR_V  0x00000001U
#define USB_OTGFS_HCINT2_XACTERR_S  7
/** USB_OTGFS_HCINT2_BBLERR : R/W; bitpos: [8]; default: 0;
 *  Babble Error (BblErr)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
 *  This bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_HCINT2_BBLERR    (BIT(8))
#define USB_OTGFS_HCINT2_BBLERR_M  (USB_OTGFS_HCINT2_BBLERR_V << USB_OTGFS_HCINT2_BBLERR_S)
#define USB_OTGFS_HCINT2_BBLERR_V  0x00000001U
#define USB_OTGFS_HCINT2_BBLERR_S  8
/** USB_OTGFS_HCINT2_FRMOVRUN : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun (FrmOvrun).
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core. This bit can be set only by the core and the application should write
 *  1 to clear
 *  it.
 */
#define USB_OTGFS_HCINT2_FRMOVRUN    (BIT(9))
#define USB_OTGFS_HCINT2_FRMOVRUN_M  (USB_OTGFS_HCINT2_FRMOVRUN_V << USB_OTGFS_HCINT2_FRMOVRUN_S)
#define USB_OTGFS_HCINT2_FRMOVRUN_V  0x00000001U
#define USB_OTGFS_HCINT2_FRMOVRUN_S  9
/** USB_OTGFS_HCINT2_DATATGLERR : R/W; bitpos: [10]; default: 0;
 *
 *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
 *  application should write 1 to clear
 *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core.
 */
#define USB_OTGFS_HCINT2_DATATGLERR    (BIT(10))
#define USB_OTGFS_HCINT2_DATATGLERR_M  (USB_OTGFS_HCINT2_DATATGLERR_V << USB_OTGFS_HCINT2_DATATGLERR_S)
#define USB_OTGFS_HCINT2_DATATGLERR_V  0x00000001U
#define USB_OTGFS_HCINT2_DATATGLERR_S  10
/** USB_OTGFS_HCINT2_BNAINTR : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process. BNA will not be generated
 *  for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT2_BNAINTR    (BIT(11))
#define USB_OTGFS_HCINT2_BNAINTR_M  (USB_OTGFS_HCINT2_BNAINTR_V << USB_OTGFS_HCINT2_BNAINTR_S)
#define USB_OTGFS_HCINT2_BNAINTR_V  0x00000001U
#define USB_OTGFS_HCINT2_BNAINTR_S  11
/** USB_OTGFS_HCINT2_XCS_XACT_ERR : R/W; bitpos: [12]; default: 0;
 *  Excessive Transaction Error (XCS_XACT_ERR)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
 *  not be generated for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT2_XCS_XACT_ERR    (BIT(12))
#define USB_OTGFS_HCINT2_XCS_XACT_ERR_M  (USB_OTGFS_HCINT2_XCS_XACT_ERR_V << USB_OTGFS_HCINT2_XCS_XACT_ERR_S)
#define USB_OTGFS_HCINT2_XCS_XACT_ERR_V  0x00000001U
#define USB_OTGFS_HCINT2_XCS_XACT_ERR_S  12
/** USB_OTGFS_HCINT2_DESC_LST_ROLLINTR : R/W; bitpos: [13]; default: 0;
 *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when the corresponding channel's descriptor list rolls over.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT2_DESC_LST_ROLLINTR    (BIT(13))
#define USB_OTGFS_HCINT2_DESC_LST_ROLLINTR_M  (USB_OTGFS_HCINT2_DESC_LST_ROLLINTR_V << USB_OTGFS_HCINT2_DESC_LST_ROLLINTR_S)
#define USB_OTGFS_HCINT2_DESC_LST_ROLLINTR_V  0x00000001U
#define USB_OTGFS_HCINT2_DESC_LST_ROLLINTR_S  13

/** USB_OTGFS_HCINTMSK2_REG register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
#define USB_OTGFS_HCINTMSK2_REG(i) (DR_REG_USB_BASE(i) + 0x54c)
/** USB_OTGFS_HCINTMSK2_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *
 *  Transfer Completed Mask (XferComplMsk)
 */
#define USB_OTGFS_HCINTMSK2_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_HCINTMSK2_XFERCOMPLMSK_M  (USB_OTGFS_HCINTMSK2_XFERCOMPLMSK_V << USB_OTGFS_HCINTMSK2_XFERCOMPLMSK_S)
#define USB_OTGFS_HCINTMSK2_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK2_XFERCOMPLMSK_S  0
/** USB_OTGFS_HCINTMSK2_CHHLTDMSK : R/W; bitpos: [1]; default: 0;
 *
 *  Channel Halted Mask (ChHltdMsk)
 */
#define USB_OTGFS_HCINTMSK2_CHHLTDMSK    (BIT(1))
#define USB_OTGFS_HCINTMSK2_CHHLTDMSK_M  (USB_OTGFS_HCINTMSK2_CHHLTDMSK_V << USB_OTGFS_HCINTMSK2_CHHLTDMSK_S)
#define USB_OTGFS_HCINTMSK2_CHHLTDMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK2_CHHLTDMSK_S  1
/** USB_OTGFS_HCINTMSK2_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *
 *  AHB Error Mask (AHBErrMsk)
 *  In  scatter/gather  DMA  mode  for  host,
 *  interrupts will not be generated due to the corresponding bits set in
 *  HCINTn.
 */
#define USB_OTGFS_HCINTMSK2_AHBERRMSK    (BIT(2))
#define USB_OTGFS_HCINTMSK2_AHBERRMSK_M  (USB_OTGFS_HCINTMSK2_AHBERRMSK_V << USB_OTGFS_HCINTMSK2_AHBERRMSK_S)
#define USB_OTGFS_HCINTMSK2_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK2_AHBERRMSK_S  2
/** USB_OTGFS_HCINTMSK2_BNAINTRMSK : R/W; bitpos: [11]; default: 0;
 *
 *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK2_BNAINTRMSK    (BIT(11))
#define USB_OTGFS_HCINTMSK2_BNAINTRMSK_M  (USB_OTGFS_HCINTMSK2_BNAINTRMSK_V << USB_OTGFS_HCINTMSK2_BNAINTRMSK_S)
#define USB_OTGFS_HCINTMSK2_BNAINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK2_BNAINTRMSK_S  11
/** USB_OTGFS_HCINTMSK2_DESC_LST_ROLLINTRMSK : R/W; bitpos: [13]; default: 0;
 *
 *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK2_DESC_LST_ROLLINTRMSK    (BIT(13))
#define USB_OTGFS_HCINTMSK2_DESC_LST_ROLLINTRMSK_M  (USB_OTGFS_HCINTMSK2_DESC_LST_ROLLINTRMSK_V << USB_OTGFS_HCINTMSK2_DESC_LST_ROLLINTRMSK_S)
#define USB_OTGFS_HCINTMSK2_DESC_LST_ROLLINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK2_DESC_LST_ROLLINTRMSK_S  13

/** USB_OTGFS_HCTSIZ2_REG register
 *  Host Channel 2 Transfer Size Register
 */
#define USB_OTGFS_HCTSIZ2_REG(i) (DR_REG_USB_BASE(i) + 0x550)
/** USB_OTGFS_HCTSIZ2_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  For an OUT, this field is the number of data bytes the host sends during the
 *  transfer.
 *
 *  For an IN, this field is the buffer size that the application has Reserved for the
 *  transfer. The application is expected to program this field as an integer multiple
 *  of the maximum packet size for IN transactions (periodic and non-periodic).
 *
 *  The width of this counter is specified as Width of Transfer Size Counters during
 *  coreConsultant configuration (parameter OTG_TRANS_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ2_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_HCTSIZ2_XFERSIZE_M  (USB_OTGFS_HCTSIZ2_XFERSIZE_V << USB_OTGFS_HCTSIZ2_XFERSIZE_S)
#define USB_OTGFS_HCTSIZ2_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_HCTSIZ2_XFERSIZE_S  0
/** USB_OTGFS_HCTSIZ2_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  This field is programmed by the application with the expected number of packets to
 *  be transmitted (OUT) or received (IN).
 *
 *  The host decrements this count on every successful transmission or reception of an
 *  OUT/IN packet. Once this count reaches zero, the application is interrupted to
 *  indicate normal completion.
 *
 *  The width of this counter is specified as Width of Packet Counters during
 *  coreConsultant configuration (parameter OTG_PACKET_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ2_PKTCNT    0x0000007FU
#define USB_OTGFS_HCTSIZ2_PKTCNT_M  (USB_OTGFS_HCTSIZ2_PKTCNT_V << USB_OTGFS_HCTSIZ2_PKTCNT_S)
#define USB_OTGFS_HCTSIZ2_PKTCNT_V  0x0000007FU
#define USB_OTGFS_HCTSIZ2_PKTCNT_S  19
/** USB_OTGFS_HCTSIZ2_PID : R/W; bitpos: [30:29]; default: 0;
 *  PID (Pid)
 *
 *  The application programs this field with the type of PID to use for the initial
 *  transaction. The host maintains this field for the rest of the transfer.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_OTGFS_HCTSIZ2_PID    0x00000003U
#define USB_OTGFS_HCTSIZ2_PID_M  (USB_OTGFS_HCTSIZ2_PID_V << USB_OTGFS_HCTSIZ2_PID_S)
#define USB_OTGFS_HCTSIZ2_PID_V  0x00000003U
#define USB_OTGFS_HCTSIZ2_PID_S  29
/** USB_OTGFS_HCTSIZ2_DOPNG : R/W; bitpos: [31]; default: 0;
 *  Do Ping (DoPng)
 *
 *  This bit is used only for OUT transfers.
 *  Setting this field to 1 directs the host to do PING protocol.
 *
 *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
 *  it disables the channel.
 */
#define USB_OTGFS_HCTSIZ2_DOPNG    (BIT(31))
#define USB_OTGFS_HCTSIZ2_DOPNG_M  (USB_OTGFS_HCTSIZ2_DOPNG_V << USB_OTGFS_HCTSIZ2_DOPNG_S)
#define USB_OTGFS_HCTSIZ2_DOPNG_V  0x00000001U
#define USB_OTGFS_HCTSIZ2_DOPNG_S  31

/** USB_OTGFS_HCDMA2_REG register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
#define USB_OTGFS_HCDMA2_REG(i) (DR_REG_USB_BASE(i) + 0x554)
/** USB_OTGFS_HCDMA2_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  In Buffer DMA Mode:
 *
 *  [31:0]: DMA Address (DMAAddr)
 *
 *  This field holds the start address in the external memory from which the data for
 *  the endpoint must be fetched or to which it must be stored. This register is
 *  incremented on every AHB transaction.
 *
 *  Reset: X if not programmed as the register is in SPRAM.
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *
 *  [31:9]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the start address of the 512 bytes page. The first descriptor in
 *  the list should be located in this address. The first descriptor may be or may not
 *  be ready. The core starts processing the list from the CTD value.
 *
 *  [8:3]: Current Transfer Desc(CTD)
 *
 *  This value is in terms of number of descriptors. The values can be from 0 to 63.
 *  - 0 -  1 descriptor.
 *  - 63 - 64 descriptors.
 *  This field indicates the current descriptor processed in the list. This field is
 *  updated both by application and the core. For example, if the application enables
 *  the channel after programming CTD=5, then the core will start processing the sixth
 *  descriptor. The address is obtained by adding a value of (8bytes*5=) 40(decimal) to
 *  DMAAddr.
 *
 *  Reset: 6'h0
 *
 *  [2:0]: Reserved
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Isochronous:
 *
 *  [31:N]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the address of the 2*(nTD+1) bytes of locations in which the
 *  isochronous descriptors are present where N is based on nTD as follows:
 *  - [31:N]: Base Address
 *  - [N-1:3]: Offset
 *  - [2:0]: 000
 *  For HS ISOC, if nTD is,
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  - 127, N=10
 *  - 255, N=11
 *  For FS ISOC, if nTD is,
 *  - 1, N=4
 *  - 3, N=5
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  [N-1:3]: Current Transfer Desc(CTD)
 *
 *  CTD for isochronous is based on the current frame/(micro)frame value. Need to be
 *  set to zero by application.
 *
 *  Reset: (N+1:3)'h0
 *
 *  [2:0]: Reserved
 */
#define USB_OTGFS_HCDMA2_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_HCDMA2_DMAADDR_M  (USB_OTGFS_HCDMA2_DMAADDR_V << USB_OTGFS_HCDMA2_DMAADDR_S)
#define USB_OTGFS_HCDMA2_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMA2_DMAADDR_S  0

/** USB_OTGFS_HCDMAB2_REG register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
#define USB_OTGFS_HCDMAB2_REG(i) (DR_REG_USB_BASE(i) + 0x55c)
/** USB_OTGFS_HCDMAB2_HCDMAB : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.
 *  This register is updated as and when the data transfer for the corresponding end
 *  point
 *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
 *  this
 *  field is reserved.
 */
#define USB_OTGFS_HCDMAB2_HCDMAB    0xFFFFFFFFU
#define USB_OTGFS_HCDMAB2_HCDMAB_M  (USB_OTGFS_HCDMAB2_HCDMAB_V << USB_OTGFS_HCDMAB2_HCDMAB_S)
#define USB_OTGFS_HCDMAB2_HCDMAB_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMAB2_HCDMAB_S  0

/** USB_OTGFS_HCCHAR3_REG register
 *  Host Channel 3 Characteristics Register
 */
#define USB_OTGFS_HCCHAR3_REG(i) (DR_REG_USB_BASE(i) + 0x560)
/** USB_OTGFS_HCCHAR3_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  Indicates the maximum packet size of the associated endpoint.
 */
#define USB_OTGFS_HCCHAR3_MPS    0x000007FFU
#define USB_OTGFS_HCCHAR3_MPS_M  (USB_OTGFS_HCCHAR3_MPS_V << USB_OTGFS_HCCHAR3_MPS_S)
#define USB_OTGFS_HCCHAR3_MPS_V  0x000007FFU
#define USB_OTGFS_HCCHAR3_MPS_S  0
/** USB_OTGFS_HCCHAR3_EPNUM : R/W; bitpos: [14:11]; default: 0;
 *  Endpoint Number (EPNum)
 *
 *  Indicates the endpoint number on the device serving as the data source or sink.
 */
#define USB_OTGFS_HCCHAR3_EPNUM    0x0000000FU
#define USB_OTGFS_HCCHAR3_EPNUM_M  (USB_OTGFS_HCCHAR3_EPNUM_V << USB_OTGFS_HCCHAR3_EPNUM_S)
#define USB_OTGFS_HCCHAR3_EPNUM_V  0x0000000FU
#define USB_OTGFS_HCCHAR3_EPNUM_S  11
/** USB_OTGFS_HCCHAR3_EPDIR : R/W; bitpos: [15]; default: 0;
 *  Endpoint Direction (EPDir)
 *
 *  Indicates whether the transaction is IN or OUT.
 *  - 1'b0: OUT
 *  - 1'b1: IN
 */
#define USB_OTGFS_HCCHAR3_EPDIR    (BIT(15))
#define USB_OTGFS_HCCHAR3_EPDIR_M  (USB_OTGFS_HCCHAR3_EPDIR_V << USB_OTGFS_HCCHAR3_EPDIR_S)
#define USB_OTGFS_HCCHAR3_EPDIR_V  0x00000001U
#define USB_OTGFS_HCCHAR3_EPDIR_S  15
/** USB_OTGFS_HCCHAR3_LSPDDEV : R/W; bitpos: [17]; default: 0;
 *  Low-Speed Device (LSpdDev)
 *
 *  This field is Set by the application to indicate that this channel is communicating
 *  to a low-speed device.
 *
 *  The application must program this bit when a low speed device is connected to the
 *  host through an FS HUB. The DWC_otg Host core uses this field to drive the
 *  XCVR_SELECT signal to 2'b11 while communicating to the LS Device through the FS hub.
 *
 *  Note: In a peer to peer setup, the DWC_otg Host core ignores this bit even if it is
 *  set by the application software.
 */
#define USB_OTGFS_HCCHAR3_LSPDDEV    (BIT(17))
#define USB_OTGFS_HCCHAR3_LSPDDEV_M  (USB_OTGFS_HCCHAR3_LSPDDEV_V << USB_OTGFS_HCCHAR3_LSPDDEV_S)
#define USB_OTGFS_HCCHAR3_LSPDDEV_V  0x00000001U
#define USB_OTGFS_HCCHAR3_LSPDDEV_S  17
/** USB_OTGFS_HCCHAR3_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Indicates the transfer type selected.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_HCCHAR3_EPTYPE    0x00000003U
#define USB_OTGFS_HCCHAR3_EPTYPE_M  (USB_OTGFS_HCCHAR3_EPTYPE_V << USB_OTGFS_HCCHAR3_EPTYPE_S)
#define USB_OTGFS_HCCHAR3_EPTYPE_V  0x00000003U
#define USB_OTGFS_HCCHAR3_EPTYPE_S  18
/** USB_OTGFS_HCCHAR3_EC : R/W; bitpos: [21:20]; default: 0;
 *  Multi Count (MC) / Error Count (EC)
 *
 *  When the Split Enable bit of the Host Channel-n Split Control
 *  register (HCSPLTn.SpltEna) is reset (1'b0), this field indicates to
 *  the host the number of transactions that must be executed per
 *  microframe for this periodic endpoint. For non periodic transfers,
 *  this field is used only in DMA mode, and specifies the number
 *  packets to be fetched for this channel before the internal DMA
 *  engine changes arbitration.
 *  - 2'b00: Reserved This field yields undefined results.
 *  - 2'b01: 1 transaction
 *  - 2'b10: 2 transactions to be issued for this endpoint per microframe
 *  - 2'b11: 3 transactions to be issued for this endpoint per microframe
 *  When HCSPLTn.SpltEna is Set (1'b1), this field indicates the
 *  number of immediate retries to be performed for a periodic split
 *  transactions on transaction errors. This field must be Set to at
 *  least 2'b01.
 */
#define USB_OTGFS_HCCHAR3_EC    0x00000003U
#define USB_OTGFS_HCCHAR3_EC_M  (USB_OTGFS_HCCHAR3_EC_V << USB_OTGFS_HCCHAR3_EC_S)
#define USB_OTGFS_HCCHAR3_EC_V  0x00000003U
#define USB_OTGFS_HCCHAR3_EC_S  20
/** USB_OTGFS_HCCHAR3_DEVADDR : R/W; bitpos: [28:22]; default: 0;
 *  Device Address (DevAddr)
 *
 *  This field selects the specific device serving as the data source
 *  or sink.
 */
#define USB_OTGFS_HCCHAR3_DEVADDR    0x0000007FU
#define USB_OTGFS_HCCHAR3_DEVADDR_M  (USB_OTGFS_HCCHAR3_DEVADDR_V << USB_OTGFS_HCCHAR3_DEVADDR_S)
#define USB_OTGFS_HCCHAR3_DEVADDR_V  0x0000007FU
#define USB_OTGFS_HCCHAR3_DEVADDR_S  22
/** USB_OTGFS_HCCHAR3_ODDFRM : R/W; bitpos: [29]; default: 0;
 *  Odd Frame (OddFrm)
 *
 *  This field is set (reset) by the application to indicate that the OTG host must
 *  perform
 *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
 *  (isochronous and interrupt) transactions.
 *  - 1'b0: Even (micro)Frame
 *  - 1'b1: Odd (micro)Frame
 */
#define USB_OTGFS_HCCHAR3_ODDFRM    (BIT(29))
#define USB_OTGFS_HCCHAR3_ODDFRM_M  (USB_OTGFS_HCCHAR3_ODDFRM_V << USB_OTGFS_HCCHAR3_ODDFRM_S)
#define USB_OTGFS_HCCHAR3_ODDFRM_V  0x00000001U
#define USB_OTGFS_HCCHAR3_ODDFRM_S  29
/** USB_OTGFS_HCCHAR3_CHDIS : R/W; bitpos: [30]; default: 0;
 *  Channel Disable (ChDis)
 *
 *  The application sets this bit to stop transmitting/receiving data
 *  on a channel, even before the transfer for that channel is
 *  complete. The application must wait for the Channel Disabled
 *  interrupt before treating the channel as disabled.
 */
#define USB_OTGFS_HCCHAR3_CHDIS    (BIT(30))
#define USB_OTGFS_HCCHAR3_CHDIS_M  (USB_OTGFS_HCCHAR3_CHDIS_V << USB_OTGFS_HCCHAR3_CHDIS_S)
#define USB_OTGFS_HCCHAR3_CHDIS_V  0x00000001U
#define USB_OTGFS_HCCHAR3_CHDIS_S  30
/** USB_OTGFS_HCCHAR3_CHENA : R/W; bitpos: [31]; default: 0;
 *  Channel Enable (ChEna)
 *
 *  When Scatter/Gather mode is enabled
 *  - 1'b0: Indicates that the descriptor structure is not yet ready.
 *  - 1'b1:  Indicates  that  the  descriptor  structure  and  data  buffer  with data
 *  is setup and this channel can access the descriptor.
 *  When Scatter/Gather mode is disabled
 *
 *  This field is set by the application and cleared by the OTG host.
 *  - 1'b0: Channel disabled
 *  - 1'b1: Channel enabled
 */
#define USB_OTGFS_HCCHAR3_CHENA    (BIT(31))
#define USB_OTGFS_HCCHAR3_CHENA_M  (USB_OTGFS_HCCHAR3_CHENA_V << USB_OTGFS_HCCHAR3_CHENA_S)
#define USB_OTGFS_HCCHAR3_CHENA_V  0x00000001U
#define USB_OTGFS_HCCHAR3_CHENA_S  31

/** USB_OTGFS_HCINT3_REG register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
#define USB_OTGFS_HCINT3_REG(i) (DR_REG_USB_BASE(i) + 0x568)
/** USB_OTGFS_HCINT3_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed (XferCompl)
 *
 *  Transfer completed normally without any errors.This bit can be set only by the core
 *  and the application should write 1 to clear it.
 *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
 *  completed with IOC bit set in its descriptor.
 *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
 *  without any errors.
 */
#define USB_OTGFS_HCINT3_XFERCOMPL    (BIT(0))
#define USB_OTGFS_HCINT3_XFERCOMPL_M  (USB_OTGFS_HCINT3_XFERCOMPL_V << USB_OTGFS_HCINT3_XFERCOMPL_S)
#define USB_OTGFS_HCINT3_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_HCINT3_XFERCOMPL_S  0
/** USB_OTGFS_HCINT3_CHHLTD : R/W; bitpos: [1]; default: 0;
 *  Channel Halted (ChHltd)
 *
 *  In non Scatter/Gather DMA mode, it indicates the transfer completed abnormally
 *  either because of any USB transaction error or in response to disable request by
 *  the application or because of a completed transfer.
 *
 *  In Scatter/gather DMA mode, this indicates that transfer completed due to any of
 *  the following
 *  - EOL being set in descriptor
 *  - AHB error
 *  - Excessive transaction errors
 *  - Babble
 *  - Stall
 */
#define USB_OTGFS_HCINT3_CHHLTD    (BIT(1))
#define USB_OTGFS_HCINT3_CHHLTD_M  (USB_OTGFS_HCINT3_CHHLTD_V << USB_OTGFS_HCINT3_CHHLTD_S)
#define USB_OTGFS_HCINT3_CHHLTD_V  0x00000001U
#define USB_OTGFS_HCINT3_CHHLTD_S  1
/** USB_OTGFS_HCINT3_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  This is generated only in Internal DMA mode when there is an AHB error during AHB
 *  read/write. The application can read the corresponding channel's DMA address
 *  register to get the error address.
 */
#define USB_OTGFS_HCINT3_AHBERR    (BIT(2))
#define USB_OTGFS_HCINT3_AHBERR_M  (USB_OTGFS_HCINT3_AHBERR_V << USB_OTGFS_HCINT3_AHBERR_S)
#define USB_OTGFS_HCINT3_AHBERR_V  0x00000001U
#define USB_OTGFS_HCINT3_AHBERR_S  2
/** USB_OTGFS_HCINT3_STALL : R/W; bitpos: [3]; default: 0;
 *  STALL Response Received Interrupt (STALL)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT3_STALL    (BIT(3))
#define USB_OTGFS_HCINT3_STALL_M  (USB_OTGFS_HCINT3_STALL_V << USB_OTGFS_HCINT3_STALL_S)
#define USB_OTGFS_HCINT3_STALL_V  0x00000001U
#define USB_OTGFS_HCINT3_STALL_S  3
/** USB_OTGFS_HCINT3_NAK : R/W; bitpos: [4]; default: 0;
 *  NAK Response Received Interrupt (NAK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT3_NAK    (BIT(4))
#define USB_OTGFS_HCINT3_NAK_M  (USB_OTGFS_HCINT3_NAK_V << USB_OTGFS_HCINT3_NAK_S)
#define USB_OTGFS_HCINT3_NAK_V  0x00000001U
#define USB_OTGFS_HCINT3_NAK_S  4
/** USB_OTGFS_HCINT3_ACK : R/W; bitpos: [5]; default: 0;
 *  ACK Response Received/Transmitted Interrupt (ACK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT3_ACK    (BIT(5))
#define USB_OTGFS_HCINT3_ACK_M  (USB_OTGFS_HCINT3_ACK_V << USB_OTGFS_HCINT3_ACK_S)
#define USB_OTGFS_HCINT3_ACK_V  0x00000001U
#define USB_OTGFS_HCINT3_ACK_S  5
/** USB_OTGFS_HCINT3_NYET : R/W; bitpos: [6]; default: 0;
 *  NYET Response Received Interrupt (NYET)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT3_NYET    (BIT(6))
#define USB_OTGFS_HCINT3_NYET_M  (USB_OTGFS_HCINT3_NYET_V << USB_OTGFS_HCINT3_NYET_S)
#define USB_OTGFS_HCINT3_NYET_V  0x00000001U
#define USB_OTGFS_HCINT3_NYET_S  6
/** USB_OTGFS_HCINT3_XACTERR : R/W; bitpos: [7]; default: 0;
 *  Transaction Error (XactErr)
 *
 *  Indicates one of the following errors occurred on the USB.
 *  - CRC check failure
 *  - Timeout
 *  - Bit stuff error
 *  - False EOP
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT3_XACTERR    (BIT(7))
#define USB_OTGFS_HCINT3_XACTERR_M  (USB_OTGFS_HCINT3_XACTERR_V << USB_OTGFS_HCINT3_XACTERR_S)
#define USB_OTGFS_HCINT3_XACTERR_V  0x00000001U
#define USB_OTGFS_HCINT3_XACTERR_S  7
/** USB_OTGFS_HCINT3_BBLERR : R/W; bitpos: [8]; default: 0;
 *  Babble Error (BblErr)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
 *  This bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_HCINT3_BBLERR    (BIT(8))
#define USB_OTGFS_HCINT3_BBLERR_M  (USB_OTGFS_HCINT3_BBLERR_V << USB_OTGFS_HCINT3_BBLERR_S)
#define USB_OTGFS_HCINT3_BBLERR_V  0x00000001U
#define USB_OTGFS_HCINT3_BBLERR_S  8
/** USB_OTGFS_HCINT3_FRMOVRUN : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun (FrmOvrun).
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core. This bit can be set only by the core and the application should write
 *  1 to clear
 *  it.
 */
#define USB_OTGFS_HCINT3_FRMOVRUN    (BIT(9))
#define USB_OTGFS_HCINT3_FRMOVRUN_M  (USB_OTGFS_HCINT3_FRMOVRUN_V << USB_OTGFS_HCINT3_FRMOVRUN_S)
#define USB_OTGFS_HCINT3_FRMOVRUN_V  0x00000001U
#define USB_OTGFS_HCINT3_FRMOVRUN_S  9
/** USB_OTGFS_HCINT3_DATATGLERR : R/W; bitpos: [10]; default: 0;
 *
 *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
 *  application should write 1 to clear
 *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core.
 */
#define USB_OTGFS_HCINT3_DATATGLERR    (BIT(10))
#define USB_OTGFS_HCINT3_DATATGLERR_M  (USB_OTGFS_HCINT3_DATATGLERR_V << USB_OTGFS_HCINT3_DATATGLERR_S)
#define USB_OTGFS_HCINT3_DATATGLERR_V  0x00000001U
#define USB_OTGFS_HCINT3_DATATGLERR_S  10
/** USB_OTGFS_HCINT3_BNAINTR : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process. BNA will not be generated
 *  for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT3_BNAINTR    (BIT(11))
#define USB_OTGFS_HCINT3_BNAINTR_M  (USB_OTGFS_HCINT3_BNAINTR_V << USB_OTGFS_HCINT3_BNAINTR_S)
#define USB_OTGFS_HCINT3_BNAINTR_V  0x00000001U
#define USB_OTGFS_HCINT3_BNAINTR_S  11
/** USB_OTGFS_HCINT3_XCS_XACT_ERR : R/W; bitpos: [12]; default: 0;
 *  Excessive Transaction Error (XCS_XACT_ERR)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
 *  not be generated for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT3_XCS_XACT_ERR    (BIT(12))
#define USB_OTGFS_HCINT3_XCS_XACT_ERR_M  (USB_OTGFS_HCINT3_XCS_XACT_ERR_V << USB_OTGFS_HCINT3_XCS_XACT_ERR_S)
#define USB_OTGFS_HCINT3_XCS_XACT_ERR_V  0x00000001U
#define USB_OTGFS_HCINT3_XCS_XACT_ERR_S  12
/** USB_OTGFS_HCINT3_DESC_LST_ROLLINTR : R/W; bitpos: [13]; default: 0;
 *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when the corresponding channel's descriptor list rolls over.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT3_DESC_LST_ROLLINTR    (BIT(13))
#define USB_OTGFS_HCINT3_DESC_LST_ROLLINTR_M  (USB_OTGFS_HCINT3_DESC_LST_ROLLINTR_V << USB_OTGFS_HCINT3_DESC_LST_ROLLINTR_S)
#define USB_OTGFS_HCINT3_DESC_LST_ROLLINTR_V  0x00000001U
#define USB_OTGFS_HCINT3_DESC_LST_ROLLINTR_S  13

/** USB_OTGFS_HCINTMSK3_REG register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
#define USB_OTGFS_HCINTMSK3_REG(i) (DR_REG_USB_BASE(i) + 0x56c)
/** USB_OTGFS_HCINTMSK3_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *
 *  Transfer Completed Mask (XferComplMsk)
 */
#define USB_OTGFS_HCINTMSK3_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_HCINTMSK3_XFERCOMPLMSK_M  (USB_OTGFS_HCINTMSK3_XFERCOMPLMSK_V << USB_OTGFS_HCINTMSK3_XFERCOMPLMSK_S)
#define USB_OTGFS_HCINTMSK3_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK3_XFERCOMPLMSK_S  0
/** USB_OTGFS_HCINTMSK3_CHHLTDMSK : R/W; bitpos: [1]; default: 0;
 *
 *  Channel Halted Mask (ChHltdMsk)
 */
#define USB_OTGFS_HCINTMSK3_CHHLTDMSK    (BIT(1))
#define USB_OTGFS_HCINTMSK3_CHHLTDMSK_M  (USB_OTGFS_HCINTMSK3_CHHLTDMSK_V << USB_OTGFS_HCINTMSK3_CHHLTDMSK_S)
#define USB_OTGFS_HCINTMSK3_CHHLTDMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK3_CHHLTDMSK_S  1
/** USB_OTGFS_HCINTMSK3_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *
 *  AHB Error Mask (AHBErrMsk)
 *  In  scatter/gather  DMA  mode  for  host,
 *  interrupts will not be generated due to the corresponding bits set in
 *  HCINTn.
 */
#define USB_OTGFS_HCINTMSK3_AHBERRMSK    (BIT(2))
#define USB_OTGFS_HCINTMSK3_AHBERRMSK_M  (USB_OTGFS_HCINTMSK3_AHBERRMSK_V << USB_OTGFS_HCINTMSK3_AHBERRMSK_S)
#define USB_OTGFS_HCINTMSK3_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK3_AHBERRMSK_S  2
/** USB_OTGFS_HCINTMSK3_BNAINTRMSK : R/W; bitpos: [11]; default: 0;
 *
 *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK3_BNAINTRMSK    (BIT(11))
#define USB_OTGFS_HCINTMSK3_BNAINTRMSK_M  (USB_OTGFS_HCINTMSK3_BNAINTRMSK_V << USB_OTGFS_HCINTMSK3_BNAINTRMSK_S)
#define USB_OTGFS_HCINTMSK3_BNAINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK3_BNAINTRMSK_S  11
/** USB_OTGFS_HCINTMSK3_DESC_LST_ROLLINTRMSK : R/W; bitpos: [13]; default: 0;
 *
 *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK3_DESC_LST_ROLLINTRMSK    (BIT(13))
#define USB_OTGFS_HCINTMSK3_DESC_LST_ROLLINTRMSK_M  (USB_OTGFS_HCINTMSK3_DESC_LST_ROLLINTRMSK_V << USB_OTGFS_HCINTMSK3_DESC_LST_ROLLINTRMSK_S)
#define USB_OTGFS_HCINTMSK3_DESC_LST_ROLLINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK3_DESC_LST_ROLLINTRMSK_S  13

/** USB_OTGFS_HCTSIZ3_REG register
 *  Host Channel 3 Transfer Size Register
 */
#define USB_OTGFS_HCTSIZ3_REG(i) (DR_REG_USB_BASE(i) + 0x570)
/** USB_OTGFS_HCTSIZ3_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  For an OUT, this field is the number of data bytes the host sends during the
 *  transfer.
 *
 *  For an IN, this field is the buffer size that the application has Reserved for the
 *  transfer. The application is expected to program this field as an integer multiple
 *  of the maximum packet size for IN transactions (periodic and non-periodic).
 *
 *  The width of this counter is specified as Width of Transfer Size Counters during
 *  coreConsultant configuration (parameter OTG_TRANS_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ3_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_HCTSIZ3_XFERSIZE_M  (USB_OTGFS_HCTSIZ3_XFERSIZE_V << USB_OTGFS_HCTSIZ3_XFERSIZE_S)
#define USB_OTGFS_HCTSIZ3_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_HCTSIZ3_XFERSIZE_S  0
/** USB_OTGFS_HCTSIZ3_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  This field is programmed by the application with the expected number of packets to
 *  be transmitted (OUT) or received (IN).
 *
 *  The host decrements this count on every successful transmission or reception of an
 *  OUT/IN packet. Once this count reaches zero, the application is interrupted to
 *  indicate normal completion.
 *
 *  The width of this counter is specified as Width of Packet Counters during
 *  coreConsultant configuration (parameter OTG_PACKET_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ3_PKTCNT    0x0000007FU
#define USB_OTGFS_HCTSIZ3_PKTCNT_M  (USB_OTGFS_HCTSIZ3_PKTCNT_V << USB_OTGFS_HCTSIZ3_PKTCNT_S)
#define USB_OTGFS_HCTSIZ3_PKTCNT_V  0x0000007FU
#define USB_OTGFS_HCTSIZ3_PKTCNT_S  19
/** USB_OTGFS_HCTSIZ3_PID : R/W; bitpos: [30:29]; default: 0;
 *  PID (Pid)
 *
 *  The application programs this field with the type of PID to use for the initial
 *  transaction. The host maintains this field for the rest of the transfer.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_OTGFS_HCTSIZ3_PID    0x00000003U
#define USB_OTGFS_HCTSIZ3_PID_M  (USB_OTGFS_HCTSIZ3_PID_V << USB_OTGFS_HCTSIZ3_PID_S)
#define USB_OTGFS_HCTSIZ3_PID_V  0x00000003U
#define USB_OTGFS_HCTSIZ3_PID_S  29
/** USB_OTGFS_HCTSIZ3_DOPNG : R/W; bitpos: [31]; default: 0;
 *  Do Ping (DoPng)
 *
 *  This bit is used only for OUT transfers.
 *  Setting this field to 1 directs the host to do PING protocol.
 *
 *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
 *  it disables the channel.
 */
#define USB_OTGFS_HCTSIZ3_DOPNG    (BIT(31))
#define USB_OTGFS_HCTSIZ3_DOPNG_M  (USB_OTGFS_HCTSIZ3_DOPNG_V << USB_OTGFS_HCTSIZ3_DOPNG_S)
#define USB_OTGFS_HCTSIZ3_DOPNG_V  0x00000001U
#define USB_OTGFS_HCTSIZ3_DOPNG_S  31

/** USB_OTGFS_HCDMA3_REG register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
#define USB_OTGFS_HCDMA3_REG(i) (DR_REG_USB_BASE(i) + 0x574)
/** USB_OTGFS_HCDMA3_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  In Buffer DMA Mode:
 *
 *  [31:0]: DMA Address (DMAAddr)
 *
 *  This field holds the start address in the external memory from which the data for
 *  the endpoint must be fetched or to which it must be stored. This register is
 *  incremented on every AHB transaction.
 *
 *  Reset: X if not programmed as the register is in SPRAM.
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *
 *  [31:9]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the start address of the 512 bytes page. The first descriptor in
 *  the list should be located in this address. The first descriptor may be or may not
 *  be ready. The core starts processing the list from the CTD value.
 *
 *  [8:3]: Current Transfer Desc(CTD)
 *
 *  This value is in terms of number of descriptors. The values can be from 0 to 63.
 *  - 0 -  1 descriptor.
 *  - 63 - 64 descriptors.
 *  This field indicates the current descriptor processed in the list. This field is
 *  updated both by application and the core. For example, if the application enables
 *  the channel after programming CTD=5, then the core will start processing the sixth
 *  descriptor. The address is obtained by adding a value of (8bytes*5=) 40(decimal) to
 *  DMAAddr.
 *
 *  Reset: 6'h0
 *
 *  [2:0]: Reserved
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Isochronous:
 *
 *  [31:N]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the address of the 2*(nTD+1) bytes of locations in which the
 *  isochronous descriptors are present where N is based on nTD as follows:
 *  - [31:N]: Base Address
 *  - [N-1:3]: Offset
 *  - [2:0]: 000
 *  For HS ISOC, if nTD is,
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  - 127, N=10
 *  - 255, N=11
 *  For FS ISOC, if nTD is,
 *  - 1, N=4
 *  - 3, N=5
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  [N-1:3]: Current Transfer Desc(CTD)
 *
 *  CTD for isochronous is based on the current frame/(micro)frame value. Need to be
 *  set to zero by application.
 *
 *  Reset: (N+1:3)'h0
 *
 *  [2:0]: Reserved
 */
#define USB_OTGFS_HCDMA3_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_HCDMA3_DMAADDR_M  (USB_OTGFS_HCDMA3_DMAADDR_V << USB_OTGFS_HCDMA3_DMAADDR_S)
#define USB_OTGFS_HCDMA3_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMA3_DMAADDR_S  0

/** USB_OTGFS_HCDMAB3_REG register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
#define USB_OTGFS_HCDMAB3_REG(i) (DR_REG_USB_BASE(i) + 0x57c)
/** USB_OTGFS_HCDMAB3_HCDMAB : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.
 *  This register is updated as and when the data transfer for the corresponding end
 *  point
 *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
 *  this
 *  field is reserved.
 */
#define USB_OTGFS_HCDMAB3_HCDMAB    0xFFFFFFFFU
#define USB_OTGFS_HCDMAB3_HCDMAB_M  (USB_OTGFS_HCDMAB3_HCDMAB_V << USB_OTGFS_HCDMAB3_HCDMAB_S)
#define USB_OTGFS_HCDMAB3_HCDMAB_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMAB3_HCDMAB_S  0

/** USB_OTGFS_HCCHAR4_REG register
 *  Host Channel 4 Characteristics Register
 */
#define USB_OTGFS_HCCHAR4_REG(i) (DR_REG_USB_BASE(i) + 0x580)
/** USB_OTGFS_HCCHAR4_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  Indicates the maximum packet size of the associated endpoint.
 */
#define USB_OTGFS_HCCHAR4_MPS    0x000007FFU
#define USB_OTGFS_HCCHAR4_MPS_M  (USB_OTGFS_HCCHAR4_MPS_V << USB_OTGFS_HCCHAR4_MPS_S)
#define USB_OTGFS_HCCHAR4_MPS_V  0x000007FFU
#define USB_OTGFS_HCCHAR4_MPS_S  0
/** USB_OTGFS_HCCHAR4_EPNUM : R/W; bitpos: [14:11]; default: 0;
 *  Endpoint Number (EPNum)
 *
 *  Indicates the endpoint number on the device serving as the data source or sink.
 */
#define USB_OTGFS_HCCHAR4_EPNUM    0x0000000FU
#define USB_OTGFS_HCCHAR4_EPNUM_M  (USB_OTGFS_HCCHAR4_EPNUM_V << USB_OTGFS_HCCHAR4_EPNUM_S)
#define USB_OTGFS_HCCHAR4_EPNUM_V  0x0000000FU
#define USB_OTGFS_HCCHAR4_EPNUM_S  11
/** USB_OTGFS_HCCHAR4_EPDIR : R/W; bitpos: [15]; default: 0;
 *  Endpoint Direction (EPDir)
 *
 *  Indicates whether the transaction is IN or OUT.
 *  - 1'b0: OUT
 *  - 1'b1: IN
 */
#define USB_OTGFS_HCCHAR4_EPDIR    (BIT(15))
#define USB_OTGFS_HCCHAR4_EPDIR_M  (USB_OTGFS_HCCHAR4_EPDIR_V << USB_OTGFS_HCCHAR4_EPDIR_S)
#define USB_OTGFS_HCCHAR4_EPDIR_V  0x00000001U
#define USB_OTGFS_HCCHAR4_EPDIR_S  15
/** USB_OTGFS_HCCHAR4_LSPDDEV : R/W; bitpos: [17]; default: 0;
 *  Low-Speed Device (LSpdDev)
 *
 *  This field is Set by the application to indicate that this channel is communicating
 *  to a low-speed device.
 *
 *  The application must program this bit when a low speed device is connected to the
 *  host through an FS HUB. The DWC_otg Host core uses this field to drive the
 *  XCVR_SELECT signal to 2'b11 while communicating to the LS Device through the FS hub.
 *
 *  Note: In a peer to peer setup, the DWC_otg Host core ignores this bit even if it is
 *  set by the application software.
 */
#define USB_OTGFS_HCCHAR4_LSPDDEV    (BIT(17))
#define USB_OTGFS_HCCHAR4_LSPDDEV_M  (USB_OTGFS_HCCHAR4_LSPDDEV_V << USB_OTGFS_HCCHAR4_LSPDDEV_S)
#define USB_OTGFS_HCCHAR4_LSPDDEV_V  0x00000001U
#define USB_OTGFS_HCCHAR4_LSPDDEV_S  17
/** USB_OTGFS_HCCHAR4_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Indicates the transfer type selected.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_HCCHAR4_EPTYPE    0x00000003U
#define USB_OTGFS_HCCHAR4_EPTYPE_M  (USB_OTGFS_HCCHAR4_EPTYPE_V << USB_OTGFS_HCCHAR4_EPTYPE_S)
#define USB_OTGFS_HCCHAR4_EPTYPE_V  0x00000003U
#define USB_OTGFS_HCCHAR4_EPTYPE_S  18
/** USB_OTGFS_HCCHAR4_EC : R/W; bitpos: [21:20]; default: 0;
 *  Multi Count (MC) / Error Count (EC)
 *
 *  When the Split Enable bit of the Host Channel-n Split Control
 *  register (HCSPLTn.SpltEna) is reset (1'b0), this field indicates to
 *  the host the number of transactions that must be executed per
 *  microframe for this periodic endpoint. For non periodic transfers,
 *  this field is used only in DMA mode, and specifies the number
 *  packets to be fetched for this channel before the internal DMA
 *  engine changes arbitration.
 *  - 2'b00: Reserved This field yields undefined results.
 *  - 2'b01: 1 transaction
 *  - 2'b10: 2 transactions to be issued for this endpoint per microframe
 *  - 2'b11: 3 transactions to be issued for this endpoint per microframe
 *  When HCSPLTn.SpltEna is Set (1'b1), this field indicates the
 *  number of immediate retries to be performed for a periodic split
 *  transactions on transaction errors. This field must be Set to at
 *  least 2'b01.
 */
#define USB_OTGFS_HCCHAR4_EC    0x00000003U
#define USB_OTGFS_HCCHAR4_EC_M  (USB_OTGFS_HCCHAR4_EC_V << USB_OTGFS_HCCHAR4_EC_S)
#define USB_OTGFS_HCCHAR4_EC_V  0x00000003U
#define USB_OTGFS_HCCHAR4_EC_S  20
/** USB_OTGFS_HCCHAR4_DEVADDR : R/W; bitpos: [28:22]; default: 0;
 *  Device Address (DevAddr)
 *
 *  This field selects the specific device serving as the data source
 *  or sink.
 */
#define USB_OTGFS_HCCHAR4_DEVADDR    0x0000007FU
#define USB_OTGFS_HCCHAR4_DEVADDR_M  (USB_OTGFS_HCCHAR4_DEVADDR_V << USB_OTGFS_HCCHAR4_DEVADDR_S)
#define USB_OTGFS_HCCHAR4_DEVADDR_V  0x0000007FU
#define USB_OTGFS_HCCHAR4_DEVADDR_S  22
/** USB_OTGFS_HCCHAR4_ODDFRM : R/W; bitpos: [29]; default: 0;
 *  Odd Frame (OddFrm)
 *
 *  This field is set (reset) by the application to indicate that the OTG host must
 *  perform
 *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
 *  (isochronous and interrupt) transactions.
 *  - 1'b0: Even (micro)Frame
 *  - 1'b1: Odd (micro)Frame
 */
#define USB_OTGFS_HCCHAR4_ODDFRM    (BIT(29))
#define USB_OTGFS_HCCHAR4_ODDFRM_M  (USB_OTGFS_HCCHAR4_ODDFRM_V << USB_OTGFS_HCCHAR4_ODDFRM_S)
#define USB_OTGFS_HCCHAR4_ODDFRM_V  0x00000001U
#define USB_OTGFS_HCCHAR4_ODDFRM_S  29
/** USB_OTGFS_HCCHAR4_CHDIS : R/W; bitpos: [30]; default: 0;
 *  Channel Disable (ChDis)
 *
 *  The application sets this bit to stop transmitting/receiving data
 *  on a channel, even before the transfer for that channel is
 *  complete. The application must wait for the Channel Disabled
 *  interrupt before treating the channel as disabled.
 */
#define USB_OTGFS_HCCHAR4_CHDIS    (BIT(30))
#define USB_OTGFS_HCCHAR4_CHDIS_M  (USB_OTGFS_HCCHAR4_CHDIS_V << USB_OTGFS_HCCHAR4_CHDIS_S)
#define USB_OTGFS_HCCHAR4_CHDIS_V  0x00000001U
#define USB_OTGFS_HCCHAR4_CHDIS_S  30
/** USB_OTGFS_HCCHAR4_CHENA : R/W; bitpos: [31]; default: 0;
 *  Channel Enable (ChEna)
 *
 *  When Scatter/Gather mode is enabled
 *  - 1'b0: Indicates that the descriptor structure is not yet ready.
 *  - 1'b1:  Indicates  that  the  descriptor  structure  and  data  buffer  with data
 *  is setup and this channel can access the descriptor.
 *  When Scatter/Gather mode is disabled
 *
 *  This field is set by the application and cleared by the OTG host.
 *  - 1'b0: Channel disabled
 *  - 1'b1: Channel enabled
 */
#define USB_OTGFS_HCCHAR4_CHENA    (BIT(31))
#define USB_OTGFS_HCCHAR4_CHENA_M  (USB_OTGFS_HCCHAR4_CHENA_V << USB_OTGFS_HCCHAR4_CHENA_S)
#define USB_OTGFS_HCCHAR4_CHENA_V  0x00000001U
#define USB_OTGFS_HCCHAR4_CHENA_S  31

/** USB_OTGFS_HCINT4_REG register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
#define USB_OTGFS_HCINT4_REG(i) (DR_REG_USB_BASE(i) + 0x588)
/** USB_OTGFS_HCINT4_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed (XferCompl)
 *
 *  Transfer completed normally without any errors.This bit can be set only by the core
 *  and the application should write 1 to clear it.
 *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
 *  completed with IOC bit set in its descriptor.
 *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
 *  without any errors.
 */
#define USB_OTGFS_HCINT4_XFERCOMPL    (BIT(0))
#define USB_OTGFS_HCINT4_XFERCOMPL_M  (USB_OTGFS_HCINT4_XFERCOMPL_V << USB_OTGFS_HCINT4_XFERCOMPL_S)
#define USB_OTGFS_HCINT4_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_HCINT4_XFERCOMPL_S  0
/** USB_OTGFS_HCINT4_CHHLTD : R/W; bitpos: [1]; default: 0;
 *  Channel Halted (ChHltd)
 *
 *  In non Scatter/Gather DMA mode, it indicates the transfer completed abnormally
 *  either because of any USB transaction error or in response to disable request by
 *  the application or because of a completed transfer.
 *
 *  In Scatter/gather DMA mode, this indicates that transfer completed due to any of
 *  the following
 *  - EOL being set in descriptor
 *  - AHB error
 *  - Excessive transaction errors
 *  - Babble
 *  - Stall
 */
#define USB_OTGFS_HCINT4_CHHLTD    (BIT(1))
#define USB_OTGFS_HCINT4_CHHLTD_M  (USB_OTGFS_HCINT4_CHHLTD_V << USB_OTGFS_HCINT4_CHHLTD_S)
#define USB_OTGFS_HCINT4_CHHLTD_V  0x00000001U
#define USB_OTGFS_HCINT4_CHHLTD_S  1
/** USB_OTGFS_HCINT4_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  This is generated only in Internal DMA mode when there is an AHB error during AHB
 *  read/write. The application can read the corresponding channel's DMA address
 *  register to get the error address.
 */
#define USB_OTGFS_HCINT4_AHBERR    (BIT(2))
#define USB_OTGFS_HCINT4_AHBERR_M  (USB_OTGFS_HCINT4_AHBERR_V << USB_OTGFS_HCINT4_AHBERR_S)
#define USB_OTGFS_HCINT4_AHBERR_V  0x00000001U
#define USB_OTGFS_HCINT4_AHBERR_S  2
/** USB_OTGFS_HCINT4_STALL : R/W; bitpos: [3]; default: 0;
 *  STALL Response Received Interrupt (STALL)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT4_STALL    (BIT(3))
#define USB_OTGFS_HCINT4_STALL_M  (USB_OTGFS_HCINT4_STALL_V << USB_OTGFS_HCINT4_STALL_S)
#define USB_OTGFS_HCINT4_STALL_V  0x00000001U
#define USB_OTGFS_HCINT4_STALL_S  3
/** USB_OTGFS_HCINT4_NAK : R/W; bitpos: [4]; default: 0;
 *  NAK Response Received Interrupt (NAK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT4_NAK    (BIT(4))
#define USB_OTGFS_HCINT4_NAK_M  (USB_OTGFS_HCINT4_NAK_V << USB_OTGFS_HCINT4_NAK_S)
#define USB_OTGFS_HCINT4_NAK_V  0x00000001U
#define USB_OTGFS_HCINT4_NAK_S  4
/** USB_OTGFS_HCINT4_ACK : R/W; bitpos: [5]; default: 0;
 *  ACK Response Received/Transmitted Interrupt (ACK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT4_ACK    (BIT(5))
#define USB_OTGFS_HCINT4_ACK_M  (USB_OTGFS_HCINT4_ACK_V << USB_OTGFS_HCINT4_ACK_S)
#define USB_OTGFS_HCINT4_ACK_V  0x00000001U
#define USB_OTGFS_HCINT4_ACK_S  5
/** USB_OTGFS_HCINT4_NYET : R/W; bitpos: [6]; default: 0;
 *  NYET Response Received Interrupt (NYET)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT4_NYET    (BIT(6))
#define USB_OTGFS_HCINT4_NYET_M  (USB_OTGFS_HCINT4_NYET_V << USB_OTGFS_HCINT4_NYET_S)
#define USB_OTGFS_HCINT4_NYET_V  0x00000001U
#define USB_OTGFS_HCINT4_NYET_S  6
/** USB_OTGFS_HCINT4_XACTERR : R/W; bitpos: [7]; default: 0;
 *  Transaction Error (XactErr)
 *
 *  Indicates one of the following errors occurred on the USB.
 *  - CRC check failure
 *  - Timeout
 *  - Bit stuff error
 *  - False EOP
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT4_XACTERR    (BIT(7))
#define USB_OTGFS_HCINT4_XACTERR_M  (USB_OTGFS_HCINT4_XACTERR_V << USB_OTGFS_HCINT4_XACTERR_S)
#define USB_OTGFS_HCINT4_XACTERR_V  0x00000001U
#define USB_OTGFS_HCINT4_XACTERR_S  7
/** USB_OTGFS_HCINT4_BBLERR : R/W; bitpos: [8]; default: 0;
 *  Babble Error (BblErr)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
 *  This bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_HCINT4_BBLERR    (BIT(8))
#define USB_OTGFS_HCINT4_BBLERR_M  (USB_OTGFS_HCINT4_BBLERR_V << USB_OTGFS_HCINT4_BBLERR_S)
#define USB_OTGFS_HCINT4_BBLERR_V  0x00000001U
#define USB_OTGFS_HCINT4_BBLERR_S  8
/** USB_OTGFS_HCINT4_FRMOVRUN : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun (FrmOvrun).
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core. This bit can be set only by the core and the application should write
 *  1 to clear
 *  it.
 */
#define USB_OTGFS_HCINT4_FRMOVRUN    (BIT(9))
#define USB_OTGFS_HCINT4_FRMOVRUN_M  (USB_OTGFS_HCINT4_FRMOVRUN_V << USB_OTGFS_HCINT4_FRMOVRUN_S)
#define USB_OTGFS_HCINT4_FRMOVRUN_V  0x00000001U
#define USB_OTGFS_HCINT4_FRMOVRUN_S  9
/** USB_OTGFS_HCINT4_DATATGLERR : R/W; bitpos: [10]; default: 0;
 *
 *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
 *  application should write 1 to clear
 *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core.
 */
#define USB_OTGFS_HCINT4_DATATGLERR    (BIT(10))
#define USB_OTGFS_HCINT4_DATATGLERR_M  (USB_OTGFS_HCINT4_DATATGLERR_V << USB_OTGFS_HCINT4_DATATGLERR_S)
#define USB_OTGFS_HCINT4_DATATGLERR_V  0x00000001U
#define USB_OTGFS_HCINT4_DATATGLERR_S  10
/** USB_OTGFS_HCINT4_BNAINTR : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process. BNA will not be generated
 *  for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT4_BNAINTR    (BIT(11))
#define USB_OTGFS_HCINT4_BNAINTR_M  (USB_OTGFS_HCINT4_BNAINTR_V << USB_OTGFS_HCINT4_BNAINTR_S)
#define USB_OTGFS_HCINT4_BNAINTR_V  0x00000001U
#define USB_OTGFS_HCINT4_BNAINTR_S  11
/** USB_OTGFS_HCINT4_XCS_XACT_ERR : R/W; bitpos: [12]; default: 0;
 *  Excessive Transaction Error (XCS_XACT_ERR)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
 *  not be generated for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT4_XCS_XACT_ERR    (BIT(12))
#define USB_OTGFS_HCINT4_XCS_XACT_ERR_M  (USB_OTGFS_HCINT4_XCS_XACT_ERR_V << USB_OTGFS_HCINT4_XCS_XACT_ERR_S)
#define USB_OTGFS_HCINT4_XCS_XACT_ERR_V  0x00000001U
#define USB_OTGFS_HCINT4_XCS_XACT_ERR_S  12
/** USB_OTGFS_HCINT4_DESC_LST_ROLLINTR : R/W; bitpos: [13]; default: 0;
 *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when the corresponding channel's descriptor list rolls over.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT4_DESC_LST_ROLLINTR    (BIT(13))
#define USB_OTGFS_HCINT4_DESC_LST_ROLLINTR_M  (USB_OTGFS_HCINT4_DESC_LST_ROLLINTR_V << USB_OTGFS_HCINT4_DESC_LST_ROLLINTR_S)
#define USB_OTGFS_HCINT4_DESC_LST_ROLLINTR_V  0x00000001U
#define USB_OTGFS_HCINT4_DESC_LST_ROLLINTR_S  13

/** USB_OTGFS_HCINTMSK4_REG register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
#define USB_OTGFS_HCINTMSK4_REG(i) (DR_REG_USB_BASE(i) + 0x58c)
/** USB_OTGFS_HCINTMSK4_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *
 *  Transfer Completed Mask (XferComplMsk)
 */
#define USB_OTGFS_HCINTMSK4_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_HCINTMSK4_XFERCOMPLMSK_M  (USB_OTGFS_HCINTMSK4_XFERCOMPLMSK_V << USB_OTGFS_HCINTMSK4_XFERCOMPLMSK_S)
#define USB_OTGFS_HCINTMSK4_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK4_XFERCOMPLMSK_S  0
/** USB_OTGFS_HCINTMSK4_CHHLTDMSK : R/W; bitpos: [1]; default: 0;
 *
 *  Channel Halted Mask (ChHltdMsk)
 */
#define USB_OTGFS_HCINTMSK4_CHHLTDMSK    (BIT(1))
#define USB_OTGFS_HCINTMSK4_CHHLTDMSK_M  (USB_OTGFS_HCINTMSK4_CHHLTDMSK_V << USB_OTGFS_HCINTMSK4_CHHLTDMSK_S)
#define USB_OTGFS_HCINTMSK4_CHHLTDMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK4_CHHLTDMSK_S  1
/** USB_OTGFS_HCINTMSK4_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *
 *  AHB Error Mask (AHBErrMsk)
 *  In  scatter/gather  DMA  mode  for  host,
 *  interrupts will not be generated due to the corresponding bits set in
 *  HCINTn.
 */
#define USB_OTGFS_HCINTMSK4_AHBERRMSK    (BIT(2))
#define USB_OTGFS_HCINTMSK4_AHBERRMSK_M  (USB_OTGFS_HCINTMSK4_AHBERRMSK_V << USB_OTGFS_HCINTMSK4_AHBERRMSK_S)
#define USB_OTGFS_HCINTMSK4_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK4_AHBERRMSK_S  2
/** USB_OTGFS_HCINTMSK4_BNAINTRMSK : R/W; bitpos: [11]; default: 0;
 *
 *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK4_BNAINTRMSK    (BIT(11))
#define USB_OTGFS_HCINTMSK4_BNAINTRMSK_M  (USB_OTGFS_HCINTMSK4_BNAINTRMSK_V << USB_OTGFS_HCINTMSK4_BNAINTRMSK_S)
#define USB_OTGFS_HCINTMSK4_BNAINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK4_BNAINTRMSK_S  11
/** USB_OTGFS_HCINTMSK4_DESC_LST_ROLLINTRMSK : R/W; bitpos: [13]; default: 0;
 *
 *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK4_DESC_LST_ROLLINTRMSK    (BIT(13))
#define USB_OTGFS_HCINTMSK4_DESC_LST_ROLLINTRMSK_M  (USB_OTGFS_HCINTMSK4_DESC_LST_ROLLINTRMSK_V << USB_OTGFS_HCINTMSK4_DESC_LST_ROLLINTRMSK_S)
#define USB_OTGFS_HCINTMSK4_DESC_LST_ROLLINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK4_DESC_LST_ROLLINTRMSK_S  13

/** USB_OTGFS_HCTSIZ4_REG register
 *  Host Channel 4 Transfer Size Register
 */
#define USB_OTGFS_HCTSIZ4_REG(i) (DR_REG_USB_BASE(i) + 0x590)
/** USB_OTGFS_HCTSIZ4_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  For an OUT, this field is the number of data bytes the host sends during the
 *  transfer.
 *
 *  For an IN, this field is the buffer size that the application has Reserved for the
 *  transfer. The application is expected to program this field as an integer multiple
 *  of the maximum packet size for IN transactions (periodic and non-periodic).
 *
 *  The width of this counter is specified as Width of Transfer Size Counters during
 *  coreConsultant configuration (parameter OTG_TRANS_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ4_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_HCTSIZ4_XFERSIZE_M  (USB_OTGFS_HCTSIZ4_XFERSIZE_V << USB_OTGFS_HCTSIZ4_XFERSIZE_S)
#define USB_OTGFS_HCTSIZ4_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_HCTSIZ4_XFERSIZE_S  0
/** USB_OTGFS_HCTSIZ4_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  This field is programmed by the application with the expected number of packets to
 *  be transmitted (OUT) or received (IN).
 *
 *  The host decrements this count on every successful transmission or reception of an
 *  OUT/IN packet. Once this count reaches zero, the application is interrupted to
 *  indicate normal completion.
 *
 *  The width of this counter is specified as Width of Packet Counters during
 *  coreConsultant configuration (parameter OTG_PACKET_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ4_PKTCNT    0x0000007FU
#define USB_OTGFS_HCTSIZ4_PKTCNT_M  (USB_OTGFS_HCTSIZ4_PKTCNT_V << USB_OTGFS_HCTSIZ4_PKTCNT_S)
#define USB_OTGFS_HCTSIZ4_PKTCNT_V  0x0000007FU
#define USB_OTGFS_HCTSIZ4_PKTCNT_S  19
/** USB_OTGFS_HCTSIZ4_PID : R/W; bitpos: [30:29]; default: 0;
 *  PID (Pid)
 *
 *  The application programs this field with the type of PID to use for the initial
 *  transaction. The host maintains this field for the rest of the transfer.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_OTGFS_HCTSIZ4_PID    0x00000003U
#define USB_OTGFS_HCTSIZ4_PID_M  (USB_OTGFS_HCTSIZ4_PID_V << USB_OTGFS_HCTSIZ4_PID_S)
#define USB_OTGFS_HCTSIZ4_PID_V  0x00000003U
#define USB_OTGFS_HCTSIZ4_PID_S  29
/** USB_OTGFS_HCTSIZ4_DOPNG : R/W; bitpos: [31]; default: 0;
 *  Do Ping (DoPng)
 *
 *  This bit is used only for OUT transfers.
 *  Setting this field to 1 directs the host to do PING protocol.
 *
 *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
 *  it disables the channel.
 */
#define USB_OTGFS_HCTSIZ4_DOPNG    (BIT(31))
#define USB_OTGFS_HCTSIZ4_DOPNG_M  (USB_OTGFS_HCTSIZ4_DOPNG_V << USB_OTGFS_HCTSIZ4_DOPNG_S)
#define USB_OTGFS_HCTSIZ4_DOPNG_V  0x00000001U
#define USB_OTGFS_HCTSIZ4_DOPNG_S  31

/** USB_OTGFS_HCDMA4_REG register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
#define USB_OTGFS_HCDMA4_REG(i) (DR_REG_USB_BASE(i) + 0x594)
/** USB_OTGFS_HCDMA4_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  In Buffer DMA Mode:
 *
 *  [31:0]: DMA Address (DMAAddr)
 *
 *  This field holds the start address in the external memory from which the data for
 *  the endpoint must be fetched or to which it must be stored. This register is
 *  incremented on every AHB transaction.
 *
 *  Reset: X if not programmed as the register is in SPRAM.
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *
 *  [31:9]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the start address of the 512 bytes page. The first descriptor in
 *  the list should be located in this address. The first descriptor may be or may not
 *  be ready. The core starts processing the list from the CTD value.
 *
 *  [8:3]: Current Transfer Desc(CTD)
 *
 *  This value is in terms of number of descriptors. The values can be from 0 to 63.
 *  - 0 -  1 descriptor.
 *  - 63 - 64 descriptors.
 *  This field indicates the current descriptor processed in the list. This field is
 *  updated both by application and the core. For example, if the application enables
 *  the channel after programming CTD=5, then the core will start processing the sixth
 *  descriptor. The address is obtained by adding a value of (8bytes*5=) 40(decimal) to
 *  DMAAddr.
 *
 *  Reset: 6'h0
 *
 *  [2:0]: Reserved
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Isochronous:
 *
 *  [31:N]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the address of the 2*(nTD+1) bytes of locations in which the
 *  isochronous descriptors are present where N is based on nTD as follows:
 *  - [31:N]: Base Address
 *  - [N-1:3]: Offset
 *  - [2:0]: 000
 *  For HS ISOC, if nTD is,
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  - 127, N=10
 *  - 255, N=11
 *  For FS ISOC, if nTD is,
 *  - 1, N=4
 *  - 3, N=5
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  [N-1:3]: Current Transfer Desc(CTD)
 *
 *  CTD for isochronous is based on the current frame/(micro)frame value. Need to be
 *  set to zero by application.
 *
 *  Reset: (N+1:3)'h0
 *
 *  [2:0]: Reserved
 */
#define USB_OTGFS_HCDMA4_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_HCDMA4_DMAADDR_M  (USB_OTGFS_HCDMA4_DMAADDR_V << USB_OTGFS_HCDMA4_DMAADDR_S)
#define USB_OTGFS_HCDMA4_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMA4_DMAADDR_S  0

/** USB_OTGFS_HCDMAB4_REG register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
#define USB_OTGFS_HCDMAB4_REG(i) (DR_REG_USB_BASE(i) + 0x59c)
/** USB_OTGFS_HCDMAB4_HCDMAB : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.
 *  This register is updated as and when the data transfer for the corresponding end
 *  point
 *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
 *  this
 *  field is reserved.
 */
#define USB_OTGFS_HCDMAB4_HCDMAB    0xFFFFFFFFU
#define USB_OTGFS_HCDMAB4_HCDMAB_M  (USB_OTGFS_HCDMAB4_HCDMAB_V << USB_OTGFS_HCDMAB4_HCDMAB_S)
#define USB_OTGFS_HCDMAB4_HCDMAB_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMAB4_HCDMAB_S  0

/** USB_OTGFS_HCCHAR5_REG register
 *  Host Channel 5 Characteristics Register
 */
#define USB_OTGFS_HCCHAR5_REG(i) (DR_REG_USB_BASE(i) + 0x5a0)
/** USB_OTGFS_HCCHAR5_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  Indicates the maximum packet size of the associated endpoint.
 */
#define USB_OTGFS_HCCHAR5_MPS    0x000007FFU
#define USB_OTGFS_HCCHAR5_MPS_M  (USB_OTGFS_HCCHAR5_MPS_V << USB_OTGFS_HCCHAR5_MPS_S)
#define USB_OTGFS_HCCHAR5_MPS_V  0x000007FFU
#define USB_OTGFS_HCCHAR5_MPS_S  0
/** USB_OTGFS_HCCHAR5_EPNUM : R/W; bitpos: [14:11]; default: 0;
 *  Endpoint Number (EPNum)
 *
 *  Indicates the endpoint number on the device serving as the data source or sink.
 */
#define USB_OTGFS_HCCHAR5_EPNUM    0x0000000FU
#define USB_OTGFS_HCCHAR5_EPNUM_M  (USB_OTGFS_HCCHAR5_EPNUM_V << USB_OTGFS_HCCHAR5_EPNUM_S)
#define USB_OTGFS_HCCHAR5_EPNUM_V  0x0000000FU
#define USB_OTGFS_HCCHAR5_EPNUM_S  11
/** USB_OTGFS_HCCHAR5_EPDIR : R/W; bitpos: [15]; default: 0;
 *  Endpoint Direction (EPDir)
 *
 *  Indicates whether the transaction is IN or OUT.
 *  - 1'b0: OUT
 *  - 1'b1: IN
 */
#define USB_OTGFS_HCCHAR5_EPDIR    (BIT(15))
#define USB_OTGFS_HCCHAR5_EPDIR_M  (USB_OTGFS_HCCHAR5_EPDIR_V << USB_OTGFS_HCCHAR5_EPDIR_S)
#define USB_OTGFS_HCCHAR5_EPDIR_V  0x00000001U
#define USB_OTGFS_HCCHAR5_EPDIR_S  15
/** USB_OTGFS_HCCHAR5_LSPDDEV : R/W; bitpos: [17]; default: 0;
 *  Low-Speed Device (LSpdDev)
 *
 *  This field is Set by the application to indicate that this channel is communicating
 *  to a low-speed device.
 *
 *  The application must program this bit when a low speed device is connected to the
 *  host through an FS HUB. The DWC_otg Host core uses this field to drive the
 *  XCVR_SELECT signal to 2'b11 while communicating to the LS Device through the FS hub.
 *
 *  Note: In a peer to peer setup, the DWC_otg Host core ignores this bit even if it is
 *  set by the application software.
 */
#define USB_OTGFS_HCCHAR5_LSPDDEV    (BIT(17))
#define USB_OTGFS_HCCHAR5_LSPDDEV_M  (USB_OTGFS_HCCHAR5_LSPDDEV_V << USB_OTGFS_HCCHAR5_LSPDDEV_S)
#define USB_OTGFS_HCCHAR5_LSPDDEV_V  0x00000001U
#define USB_OTGFS_HCCHAR5_LSPDDEV_S  17
/** USB_OTGFS_HCCHAR5_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Indicates the transfer type selected.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_HCCHAR5_EPTYPE    0x00000003U
#define USB_OTGFS_HCCHAR5_EPTYPE_M  (USB_OTGFS_HCCHAR5_EPTYPE_V << USB_OTGFS_HCCHAR5_EPTYPE_S)
#define USB_OTGFS_HCCHAR5_EPTYPE_V  0x00000003U
#define USB_OTGFS_HCCHAR5_EPTYPE_S  18
/** USB_OTGFS_HCCHAR5_EC : R/W; bitpos: [21:20]; default: 0;
 *  Multi Count (MC) / Error Count (EC)
 *
 *  When the Split Enable bit of the Host Channel-n Split Control
 *  register (HCSPLTn.SpltEna) is reset (1'b0), this field indicates to
 *  the host the number of transactions that must be executed per
 *  microframe for this periodic endpoint. For non periodic transfers,
 *  this field is used only in DMA mode, and specifies the number
 *  packets to be fetched for this channel before the internal DMA
 *  engine changes arbitration.
 *  - 2'b00: Reserved This field yields undefined results.
 *  - 2'b01: 1 transaction
 *  - 2'b10: 2 transactions to be issued for this endpoint per microframe
 *  - 2'b11: 3 transactions to be issued for this endpoint per microframe
 *  When HCSPLTn.SpltEna is Set (1'b1), this field indicates the
 *  number of immediate retries to be performed for a periodic split
 *  transactions on transaction errors. This field must be Set to at
 *  least 2'b01.
 */
#define USB_OTGFS_HCCHAR5_EC    0x00000003U
#define USB_OTGFS_HCCHAR5_EC_M  (USB_OTGFS_HCCHAR5_EC_V << USB_OTGFS_HCCHAR5_EC_S)
#define USB_OTGFS_HCCHAR5_EC_V  0x00000003U
#define USB_OTGFS_HCCHAR5_EC_S  20
/** USB_OTGFS_HCCHAR5_DEVADDR : R/W; bitpos: [28:22]; default: 0;
 *  Device Address (DevAddr)
 *
 *  This field selects the specific device serving as the data source
 *  or sink.
 */
#define USB_OTGFS_HCCHAR5_DEVADDR    0x0000007FU
#define USB_OTGFS_HCCHAR5_DEVADDR_M  (USB_OTGFS_HCCHAR5_DEVADDR_V << USB_OTGFS_HCCHAR5_DEVADDR_S)
#define USB_OTGFS_HCCHAR5_DEVADDR_V  0x0000007FU
#define USB_OTGFS_HCCHAR5_DEVADDR_S  22
/** USB_OTGFS_HCCHAR5_ODDFRM : R/W; bitpos: [29]; default: 0;
 *  Odd Frame (OddFrm)
 *
 *  This field is set (reset) by the application to indicate that the OTG host must
 *  perform
 *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
 *  (isochronous and interrupt) transactions.
 *  - 1'b0: Even (micro)Frame
 *  - 1'b1: Odd (micro)Frame
 */
#define USB_OTGFS_HCCHAR5_ODDFRM    (BIT(29))
#define USB_OTGFS_HCCHAR5_ODDFRM_M  (USB_OTGFS_HCCHAR5_ODDFRM_V << USB_OTGFS_HCCHAR5_ODDFRM_S)
#define USB_OTGFS_HCCHAR5_ODDFRM_V  0x00000001U
#define USB_OTGFS_HCCHAR5_ODDFRM_S  29
/** USB_OTGFS_HCCHAR5_CHDIS : R/W; bitpos: [30]; default: 0;
 *  Channel Disable (ChDis)
 *
 *  The application sets this bit to stop transmitting/receiving data
 *  on a channel, even before the transfer for that channel is
 *  complete. The application must wait for the Channel Disabled
 *  interrupt before treating the channel as disabled.
 */
#define USB_OTGFS_HCCHAR5_CHDIS    (BIT(30))
#define USB_OTGFS_HCCHAR5_CHDIS_M  (USB_OTGFS_HCCHAR5_CHDIS_V << USB_OTGFS_HCCHAR5_CHDIS_S)
#define USB_OTGFS_HCCHAR5_CHDIS_V  0x00000001U
#define USB_OTGFS_HCCHAR5_CHDIS_S  30
/** USB_OTGFS_HCCHAR5_CHENA : R/W; bitpos: [31]; default: 0;
 *  Channel Enable (ChEna)
 *
 *  When Scatter/Gather mode is enabled
 *  - 1'b0: Indicates that the descriptor structure is not yet ready.
 *  - 1'b1:  Indicates  that  the  descriptor  structure  and  data  buffer  with data
 *  is setup and this channel can access the descriptor.
 *  When Scatter/Gather mode is disabled
 *
 *  This field is set by the application and cleared by the OTG host.
 *  - 1'b0: Channel disabled
 *  - 1'b1: Channel enabled
 */
#define USB_OTGFS_HCCHAR5_CHENA    (BIT(31))
#define USB_OTGFS_HCCHAR5_CHENA_M  (USB_OTGFS_HCCHAR5_CHENA_V << USB_OTGFS_HCCHAR5_CHENA_S)
#define USB_OTGFS_HCCHAR5_CHENA_V  0x00000001U
#define USB_OTGFS_HCCHAR5_CHENA_S  31

/** USB_OTGFS_HCINT5_REG register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
#define USB_OTGFS_HCINT5_REG(i) (DR_REG_USB_BASE(i) + 0x5a8)
/** USB_OTGFS_HCINT5_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed (XferCompl)
 *
 *  Transfer completed normally without any errors.This bit can be set only by the core
 *  and the application should write 1 to clear it.
 *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
 *  completed with IOC bit set in its descriptor.
 *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
 *  without any errors.
 */
#define USB_OTGFS_HCINT5_XFERCOMPL    (BIT(0))
#define USB_OTGFS_HCINT5_XFERCOMPL_M  (USB_OTGFS_HCINT5_XFERCOMPL_V << USB_OTGFS_HCINT5_XFERCOMPL_S)
#define USB_OTGFS_HCINT5_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_HCINT5_XFERCOMPL_S  0
/** USB_OTGFS_HCINT5_CHHLTD : R/W; bitpos: [1]; default: 0;
 *  Channel Halted (ChHltd)
 *
 *  In non Scatter/Gather DMA mode, it indicates the transfer completed abnormally
 *  either because of any USB transaction error or in response to disable request by
 *  the application or because of a completed transfer.
 *
 *  In Scatter/gather DMA mode, this indicates that transfer completed due to any of
 *  the following
 *  - EOL being set in descriptor
 *  - AHB error
 *  - Excessive transaction errors
 *  - Babble
 *  - Stall
 */
#define USB_OTGFS_HCINT5_CHHLTD    (BIT(1))
#define USB_OTGFS_HCINT5_CHHLTD_M  (USB_OTGFS_HCINT5_CHHLTD_V << USB_OTGFS_HCINT5_CHHLTD_S)
#define USB_OTGFS_HCINT5_CHHLTD_V  0x00000001U
#define USB_OTGFS_HCINT5_CHHLTD_S  1
/** USB_OTGFS_HCINT5_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  This is generated only in Internal DMA mode when there is an AHB error during AHB
 *  read/write. The application can read the corresponding channel's DMA address
 *  register to get the error address.
 */
#define USB_OTGFS_HCINT5_AHBERR    (BIT(2))
#define USB_OTGFS_HCINT5_AHBERR_M  (USB_OTGFS_HCINT5_AHBERR_V << USB_OTGFS_HCINT5_AHBERR_S)
#define USB_OTGFS_HCINT5_AHBERR_V  0x00000001U
#define USB_OTGFS_HCINT5_AHBERR_S  2
/** USB_OTGFS_HCINT5_STALL : R/W; bitpos: [3]; default: 0;
 *  STALL Response Received Interrupt (STALL)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT5_STALL    (BIT(3))
#define USB_OTGFS_HCINT5_STALL_M  (USB_OTGFS_HCINT5_STALL_V << USB_OTGFS_HCINT5_STALL_S)
#define USB_OTGFS_HCINT5_STALL_V  0x00000001U
#define USB_OTGFS_HCINT5_STALL_S  3
/** USB_OTGFS_HCINT5_NAK : R/W; bitpos: [4]; default: 0;
 *  NAK Response Received Interrupt (NAK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT5_NAK    (BIT(4))
#define USB_OTGFS_HCINT5_NAK_M  (USB_OTGFS_HCINT5_NAK_V << USB_OTGFS_HCINT5_NAK_S)
#define USB_OTGFS_HCINT5_NAK_V  0x00000001U
#define USB_OTGFS_HCINT5_NAK_S  4
/** USB_OTGFS_HCINT5_ACK : R/W; bitpos: [5]; default: 0;
 *  ACK Response Received/Transmitted Interrupt (ACK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT5_ACK    (BIT(5))
#define USB_OTGFS_HCINT5_ACK_M  (USB_OTGFS_HCINT5_ACK_V << USB_OTGFS_HCINT5_ACK_S)
#define USB_OTGFS_HCINT5_ACK_V  0x00000001U
#define USB_OTGFS_HCINT5_ACK_S  5
/** USB_OTGFS_HCINT5_NYET : R/W; bitpos: [6]; default: 0;
 *  NYET Response Received Interrupt (NYET)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT5_NYET    (BIT(6))
#define USB_OTGFS_HCINT5_NYET_M  (USB_OTGFS_HCINT5_NYET_V << USB_OTGFS_HCINT5_NYET_S)
#define USB_OTGFS_HCINT5_NYET_V  0x00000001U
#define USB_OTGFS_HCINT5_NYET_S  6
/** USB_OTGFS_HCINT5_XACTERR : R/W; bitpos: [7]; default: 0;
 *  Transaction Error (XactErr)
 *
 *  Indicates one of the following errors occurred on the USB.
 *  - CRC check failure
 *  - Timeout
 *  - Bit stuff error
 *  - False EOP
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT5_XACTERR    (BIT(7))
#define USB_OTGFS_HCINT5_XACTERR_M  (USB_OTGFS_HCINT5_XACTERR_V << USB_OTGFS_HCINT5_XACTERR_S)
#define USB_OTGFS_HCINT5_XACTERR_V  0x00000001U
#define USB_OTGFS_HCINT5_XACTERR_S  7
/** USB_OTGFS_HCINT5_BBLERR : R/W; bitpos: [8]; default: 0;
 *  Babble Error (BblErr)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
 *  This bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_HCINT5_BBLERR    (BIT(8))
#define USB_OTGFS_HCINT5_BBLERR_M  (USB_OTGFS_HCINT5_BBLERR_V << USB_OTGFS_HCINT5_BBLERR_S)
#define USB_OTGFS_HCINT5_BBLERR_V  0x00000001U
#define USB_OTGFS_HCINT5_BBLERR_S  8
/** USB_OTGFS_HCINT5_FRMOVRUN : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun (FrmOvrun).
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core. This bit can be set only by the core and the application should write
 *  1 to clear
 *  it.
 */
#define USB_OTGFS_HCINT5_FRMOVRUN    (BIT(9))
#define USB_OTGFS_HCINT5_FRMOVRUN_M  (USB_OTGFS_HCINT5_FRMOVRUN_V << USB_OTGFS_HCINT5_FRMOVRUN_S)
#define USB_OTGFS_HCINT5_FRMOVRUN_V  0x00000001U
#define USB_OTGFS_HCINT5_FRMOVRUN_S  9
/** USB_OTGFS_HCINT5_DATATGLERR : R/W; bitpos: [10]; default: 0;
 *
 *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
 *  application should write 1 to clear
 *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core.
 */
#define USB_OTGFS_HCINT5_DATATGLERR    (BIT(10))
#define USB_OTGFS_HCINT5_DATATGLERR_M  (USB_OTGFS_HCINT5_DATATGLERR_V << USB_OTGFS_HCINT5_DATATGLERR_S)
#define USB_OTGFS_HCINT5_DATATGLERR_V  0x00000001U
#define USB_OTGFS_HCINT5_DATATGLERR_S  10
/** USB_OTGFS_HCINT5_BNAINTR : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process. BNA will not be generated
 *  for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT5_BNAINTR    (BIT(11))
#define USB_OTGFS_HCINT5_BNAINTR_M  (USB_OTGFS_HCINT5_BNAINTR_V << USB_OTGFS_HCINT5_BNAINTR_S)
#define USB_OTGFS_HCINT5_BNAINTR_V  0x00000001U
#define USB_OTGFS_HCINT5_BNAINTR_S  11
/** USB_OTGFS_HCINT5_XCS_XACT_ERR : R/W; bitpos: [12]; default: 0;
 *  Excessive Transaction Error (XCS_XACT_ERR)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
 *  not be generated for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT5_XCS_XACT_ERR    (BIT(12))
#define USB_OTGFS_HCINT5_XCS_XACT_ERR_M  (USB_OTGFS_HCINT5_XCS_XACT_ERR_V << USB_OTGFS_HCINT5_XCS_XACT_ERR_S)
#define USB_OTGFS_HCINT5_XCS_XACT_ERR_V  0x00000001U
#define USB_OTGFS_HCINT5_XCS_XACT_ERR_S  12
/** USB_OTGFS_HCINT5_DESC_LST_ROLLINTR : R/W; bitpos: [13]; default: 0;
 *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when the corresponding channel's descriptor list rolls over.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT5_DESC_LST_ROLLINTR    (BIT(13))
#define USB_OTGFS_HCINT5_DESC_LST_ROLLINTR_M  (USB_OTGFS_HCINT5_DESC_LST_ROLLINTR_V << USB_OTGFS_HCINT5_DESC_LST_ROLLINTR_S)
#define USB_OTGFS_HCINT5_DESC_LST_ROLLINTR_V  0x00000001U
#define USB_OTGFS_HCINT5_DESC_LST_ROLLINTR_S  13

/** USB_OTGFS_HCINTMSK5_REG register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
#define USB_OTGFS_HCINTMSK5_REG(i) (DR_REG_USB_BASE(i) + 0x5ac)
/** USB_OTGFS_HCINTMSK5_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *
 *  Transfer Completed Mask (XferComplMsk)
 */
#define USB_OTGFS_HCINTMSK5_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_HCINTMSK5_XFERCOMPLMSK_M  (USB_OTGFS_HCINTMSK5_XFERCOMPLMSK_V << USB_OTGFS_HCINTMSK5_XFERCOMPLMSK_S)
#define USB_OTGFS_HCINTMSK5_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK5_XFERCOMPLMSK_S  0
/** USB_OTGFS_HCINTMSK5_CHHLTDMSK : R/W; bitpos: [1]; default: 0;
 *
 *  Channel Halted Mask (ChHltdMsk)
 */
#define USB_OTGFS_HCINTMSK5_CHHLTDMSK    (BIT(1))
#define USB_OTGFS_HCINTMSK5_CHHLTDMSK_M  (USB_OTGFS_HCINTMSK5_CHHLTDMSK_V << USB_OTGFS_HCINTMSK5_CHHLTDMSK_S)
#define USB_OTGFS_HCINTMSK5_CHHLTDMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK5_CHHLTDMSK_S  1
/** USB_OTGFS_HCINTMSK5_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *
 *  AHB Error Mask (AHBErrMsk)
 *  In  scatter/gather  DMA  mode  for  host,
 *  interrupts will not be generated due to the corresponding bits set in
 *  HCINTn.
 */
#define USB_OTGFS_HCINTMSK5_AHBERRMSK    (BIT(2))
#define USB_OTGFS_HCINTMSK5_AHBERRMSK_M  (USB_OTGFS_HCINTMSK5_AHBERRMSK_V << USB_OTGFS_HCINTMSK5_AHBERRMSK_S)
#define USB_OTGFS_HCINTMSK5_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK5_AHBERRMSK_S  2
/** USB_OTGFS_HCINTMSK5_BNAINTRMSK : R/W; bitpos: [11]; default: 0;
 *
 *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK5_BNAINTRMSK    (BIT(11))
#define USB_OTGFS_HCINTMSK5_BNAINTRMSK_M  (USB_OTGFS_HCINTMSK5_BNAINTRMSK_V << USB_OTGFS_HCINTMSK5_BNAINTRMSK_S)
#define USB_OTGFS_HCINTMSK5_BNAINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK5_BNAINTRMSK_S  11
/** USB_OTGFS_HCINTMSK5_DESC_LST_ROLLINTRMSK : R/W; bitpos: [13]; default: 0;
 *
 *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK5_DESC_LST_ROLLINTRMSK    (BIT(13))
#define USB_OTGFS_HCINTMSK5_DESC_LST_ROLLINTRMSK_M  (USB_OTGFS_HCINTMSK5_DESC_LST_ROLLINTRMSK_V << USB_OTGFS_HCINTMSK5_DESC_LST_ROLLINTRMSK_S)
#define USB_OTGFS_HCINTMSK5_DESC_LST_ROLLINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK5_DESC_LST_ROLLINTRMSK_S  13

/** USB_OTGFS_HCTSIZ5_REG register
 *  Host Channel 5 Transfer Size Register
 */
#define USB_OTGFS_HCTSIZ5_REG(i) (DR_REG_USB_BASE(i) + 0x5b0)
/** USB_OTGFS_HCTSIZ5_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  For an OUT, this field is the number of data bytes the host sends during the
 *  transfer.
 *
 *  For an IN, this field is the buffer size that the application has Reserved for the
 *  transfer. The application is expected to program this field as an integer multiple
 *  of the maximum packet size for IN transactions (periodic and non-periodic).
 *
 *  The width of this counter is specified as Width of Transfer Size Counters during
 *  coreConsultant configuration (parameter OTG_TRANS_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ5_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_HCTSIZ5_XFERSIZE_M  (USB_OTGFS_HCTSIZ5_XFERSIZE_V << USB_OTGFS_HCTSIZ5_XFERSIZE_S)
#define USB_OTGFS_HCTSIZ5_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_HCTSIZ5_XFERSIZE_S  0
/** USB_OTGFS_HCTSIZ5_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  This field is programmed by the application with the expected number of packets to
 *  be transmitted (OUT) or received (IN).
 *
 *  The host decrements this count on every successful transmission or reception of an
 *  OUT/IN packet. Once this count reaches zero, the application is interrupted to
 *  indicate normal completion.
 *
 *  The width of this counter is specified as Width of Packet Counters during
 *  coreConsultant configuration (parameter OTG_PACKET_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ5_PKTCNT    0x0000007FU
#define USB_OTGFS_HCTSIZ5_PKTCNT_M  (USB_OTGFS_HCTSIZ5_PKTCNT_V << USB_OTGFS_HCTSIZ5_PKTCNT_S)
#define USB_OTGFS_HCTSIZ5_PKTCNT_V  0x0000007FU
#define USB_OTGFS_HCTSIZ5_PKTCNT_S  19
/** USB_OTGFS_HCTSIZ5_PID : R/W; bitpos: [30:29]; default: 0;
 *  PID (Pid)
 *
 *  The application programs this field with the type of PID to use for the initial
 *  transaction. The host maintains this field for the rest of the transfer.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_OTGFS_HCTSIZ5_PID    0x00000003U
#define USB_OTGFS_HCTSIZ5_PID_M  (USB_OTGFS_HCTSIZ5_PID_V << USB_OTGFS_HCTSIZ5_PID_S)
#define USB_OTGFS_HCTSIZ5_PID_V  0x00000003U
#define USB_OTGFS_HCTSIZ5_PID_S  29
/** USB_OTGFS_HCTSIZ5_DOPNG : R/W; bitpos: [31]; default: 0;
 *  Do Ping (DoPng)
 *
 *  This bit is used only for OUT transfers.
 *  Setting this field to 1 directs the host to do PING protocol.
 *
 *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
 *  it disables the channel.
 */
#define USB_OTGFS_HCTSIZ5_DOPNG    (BIT(31))
#define USB_OTGFS_HCTSIZ5_DOPNG_M  (USB_OTGFS_HCTSIZ5_DOPNG_V << USB_OTGFS_HCTSIZ5_DOPNG_S)
#define USB_OTGFS_HCTSIZ5_DOPNG_V  0x00000001U
#define USB_OTGFS_HCTSIZ5_DOPNG_S  31

/** USB_OTGFS_HCDMA5_REG register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
#define USB_OTGFS_HCDMA5_REG(i) (DR_REG_USB_BASE(i) + 0x5b4)
/** USB_OTGFS_HCDMA5_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  In Buffer DMA Mode:
 *
 *  [31:0]: DMA Address (DMAAddr)
 *
 *  This field holds the start address in the external memory from which the data for
 *  the endpoint must be fetched or to which it must be stored. This register is
 *  incremented on every AHB transaction.
 *
 *  Reset: X if not programmed as the register is in SPRAM.
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *
 *  [31:9]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the start address of the 512 bytes page. The first descriptor in
 *  the list should be located in this address. The first descriptor may be or may not
 *  be ready. The core starts processing the list from the CTD value.
 *
 *  [8:3]: Current Transfer Desc(CTD)
 *
 *  This value is in terms of number of descriptors. The values can be from 0 to 63.
 *  - 0 -  1 descriptor.
 *  - 63 - 64 descriptors.
 *  This field indicates the current descriptor processed in the list. This field is
 *  updated both by application and the core. For example, if the application enables
 *  the channel after programming CTD=5, then the core will start processing the sixth
 *  descriptor. The address is obtained by adding a value of (8bytes*5=) 40(decimal) to
 *  DMAAddr.
 *
 *  Reset: 6'h0
 *
 *  [2:0]: Reserved
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Isochronous:
 *
 *  [31:N]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the address of the 2*(nTD+1) bytes of locations in which the
 *  isochronous descriptors are present where N is based on nTD as follows:
 *  - [31:N]: Base Address
 *  - [N-1:3]: Offset
 *  - [2:0]: 000
 *  For HS ISOC, if nTD is,
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  - 127, N=10
 *  - 255, N=11
 *  For FS ISOC, if nTD is,
 *  - 1, N=4
 *  - 3, N=5
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  [N-1:3]: Current Transfer Desc(CTD)
 *
 *  CTD for isochronous is based on the current frame/(micro)frame value. Need to be
 *  set to zero by application.
 *
 *  Reset: (N+1:3)'h0
 *
 *  [2:0]: Reserved
 */
#define USB_OTGFS_HCDMA5_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_HCDMA5_DMAADDR_M  (USB_OTGFS_HCDMA5_DMAADDR_V << USB_OTGFS_HCDMA5_DMAADDR_S)
#define USB_OTGFS_HCDMA5_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMA5_DMAADDR_S  0

/** USB_OTGFS_HCDMAB5_REG register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
#define USB_OTGFS_HCDMAB5_REG(i) (DR_REG_USB_BASE(i) + 0x5bc)
/** USB_OTGFS_HCDMAB5_HCDMAB : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.
 *  This register is updated as and when the data transfer for the corresponding end
 *  point
 *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
 *  this
 *  field is reserved.
 */
#define USB_OTGFS_HCDMAB5_HCDMAB    0xFFFFFFFFU
#define USB_OTGFS_HCDMAB5_HCDMAB_M  (USB_OTGFS_HCDMAB5_HCDMAB_V << USB_OTGFS_HCDMAB5_HCDMAB_S)
#define USB_OTGFS_HCDMAB5_HCDMAB_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMAB5_HCDMAB_S  0

/** USB_OTGFS_HCCHAR6_REG register
 *  Host Channel 6 Characteristics Register
 */
#define USB_OTGFS_HCCHAR6_REG(i) (DR_REG_USB_BASE(i) + 0x5c0)
/** USB_OTGFS_HCCHAR6_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  Indicates the maximum packet size of the associated endpoint.
 */
#define USB_OTGFS_HCCHAR6_MPS    0x000007FFU
#define USB_OTGFS_HCCHAR6_MPS_M  (USB_OTGFS_HCCHAR6_MPS_V << USB_OTGFS_HCCHAR6_MPS_S)
#define USB_OTGFS_HCCHAR6_MPS_V  0x000007FFU
#define USB_OTGFS_HCCHAR6_MPS_S  0
/** USB_OTGFS_HCCHAR6_EPNUM : R/W; bitpos: [14:11]; default: 0;
 *  Endpoint Number (EPNum)
 *
 *  Indicates the endpoint number on the device serving as the data source or sink.
 */
#define USB_OTGFS_HCCHAR6_EPNUM    0x0000000FU
#define USB_OTGFS_HCCHAR6_EPNUM_M  (USB_OTGFS_HCCHAR6_EPNUM_V << USB_OTGFS_HCCHAR6_EPNUM_S)
#define USB_OTGFS_HCCHAR6_EPNUM_V  0x0000000FU
#define USB_OTGFS_HCCHAR6_EPNUM_S  11
/** USB_OTGFS_HCCHAR6_EPDIR : R/W; bitpos: [15]; default: 0;
 *  Endpoint Direction (EPDir)
 *
 *  Indicates whether the transaction is IN or OUT.
 *  - 1'b0: OUT
 *  - 1'b1: IN
 */
#define USB_OTGFS_HCCHAR6_EPDIR    (BIT(15))
#define USB_OTGFS_HCCHAR6_EPDIR_M  (USB_OTGFS_HCCHAR6_EPDIR_V << USB_OTGFS_HCCHAR6_EPDIR_S)
#define USB_OTGFS_HCCHAR6_EPDIR_V  0x00000001U
#define USB_OTGFS_HCCHAR6_EPDIR_S  15
/** USB_OTGFS_HCCHAR6_LSPDDEV : R/W; bitpos: [17]; default: 0;
 *  Low-Speed Device (LSpdDev)
 *
 *  This field is Set by the application to indicate that this channel is communicating
 *  to a low-speed device.
 *
 *  The application must program this bit when a low speed device is connected to the
 *  host through an FS HUB. The DWC_otg Host core uses this field to drive the
 *  XCVR_SELECT signal to 2'b11 while communicating to the LS Device through the FS hub.
 *
 *  Note: In a peer to peer setup, the DWC_otg Host core ignores this bit even if it is
 *  set by the application software.
 */
#define USB_OTGFS_HCCHAR6_LSPDDEV    (BIT(17))
#define USB_OTGFS_HCCHAR6_LSPDDEV_M  (USB_OTGFS_HCCHAR6_LSPDDEV_V << USB_OTGFS_HCCHAR6_LSPDDEV_S)
#define USB_OTGFS_HCCHAR6_LSPDDEV_V  0x00000001U
#define USB_OTGFS_HCCHAR6_LSPDDEV_S  17
/** USB_OTGFS_HCCHAR6_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Indicates the transfer type selected.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_HCCHAR6_EPTYPE    0x00000003U
#define USB_OTGFS_HCCHAR6_EPTYPE_M  (USB_OTGFS_HCCHAR6_EPTYPE_V << USB_OTGFS_HCCHAR6_EPTYPE_S)
#define USB_OTGFS_HCCHAR6_EPTYPE_V  0x00000003U
#define USB_OTGFS_HCCHAR6_EPTYPE_S  18
/** USB_OTGFS_HCCHAR6_EC : R/W; bitpos: [21:20]; default: 0;
 *  Multi Count (MC) / Error Count (EC)
 *
 *  When the Split Enable bit of the Host Channel-n Split Control
 *  register (HCSPLTn.SpltEna) is reset (1'b0), this field indicates to
 *  the host the number of transactions that must be executed per
 *  microframe for this periodic endpoint. For non periodic transfers,
 *  this field is used only in DMA mode, and specifies the number
 *  packets to be fetched for this channel before the internal DMA
 *  engine changes arbitration.
 *  - 2'b00: Reserved This field yields undefined results.
 *  - 2'b01: 1 transaction
 *  - 2'b10: 2 transactions to be issued for this endpoint per microframe
 *  - 2'b11: 3 transactions to be issued for this endpoint per microframe
 *  When HCSPLTn.SpltEna is Set (1'b1), this field indicates the
 *  number of immediate retries to be performed for a periodic split
 *  transactions on transaction errors. This field must be Set to at
 *  least 2'b01.
 */
#define USB_OTGFS_HCCHAR6_EC    0x00000003U
#define USB_OTGFS_HCCHAR6_EC_M  (USB_OTGFS_HCCHAR6_EC_V << USB_OTGFS_HCCHAR6_EC_S)
#define USB_OTGFS_HCCHAR6_EC_V  0x00000003U
#define USB_OTGFS_HCCHAR6_EC_S  20
/** USB_OTGFS_HCCHAR6_DEVADDR : R/W; bitpos: [28:22]; default: 0;
 *  Device Address (DevAddr)
 *
 *  This field selects the specific device serving as the data source
 *  or sink.
 */
#define USB_OTGFS_HCCHAR6_DEVADDR    0x0000007FU
#define USB_OTGFS_HCCHAR6_DEVADDR_M  (USB_OTGFS_HCCHAR6_DEVADDR_V << USB_OTGFS_HCCHAR6_DEVADDR_S)
#define USB_OTGFS_HCCHAR6_DEVADDR_V  0x0000007FU
#define USB_OTGFS_HCCHAR6_DEVADDR_S  22
/** USB_OTGFS_HCCHAR6_ODDFRM : R/W; bitpos: [29]; default: 0;
 *  Odd Frame (OddFrm)
 *
 *  This field is set (reset) by the application to indicate that the OTG host must
 *  perform
 *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
 *  (isochronous and interrupt) transactions.
 *  - 1'b0: Even (micro)Frame
 *  - 1'b1: Odd (micro)Frame
 */
#define USB_OTGFS_HCCHAR6_ODDFRM    (BIT(29))
#define USB_OTGFS_HCCHAR6_ODDFRM_M  (USB_OTGFS_HCCHAR6_ODDFRM_V << USB_OTGFS_HCCHAR6_ODDFRM_S)
#define USB_OTGFS_HCCHAR6_ODDFRM_V  0x00000001U
#define USB_OTGFS_HCCHAR6_ODDFRM_S  29
/** USB_OTGFS_HCCHAR6_CHDIS : R/W; bitpos: [30]; default: 0;
 *  Channel Disable (ChDis)
 *
 *  The application sets this bit to stop transmitting/receiving data
 *  on a channel, even before the transfer for that channel is
 *  complete. The application must wait for the Channel Disabled
 *  interrupt before treating the channel as disabled.
 */
#define USB_OTGFS_HCCHAR6_CHDIS    (BIT(30))
#define USB_OTGFS_HCCHAR6_CHDIS_M  (USB_OTGFS_HCCHAR6_CHDIS_V << USB_OTGFS_HCCHAR6_CHDIS_S)
#define USB_OTGFS_HCCHAR6_CHDIS_V  0x00000001U
#define USB_OTGFS_HCCHAR6_CHDIS_S  30
/** USB_OTGFS_HCCHAR6_CHENA : R/W; bitpos: [31]; default: 0;
 *  Channel Enable (ChEna)
 *
 *  When Scatter/Gather mode is enabled
 *  - 1'b0: Indicates that the descriptor structure is not yet ready.
 *  - 1'b1:  Indicates  that  the  descriptor  structure  and  data  buffer  with data
 *  is setup and this channel can access the descriptor.
 *  When Scatter/Gather mode is disabled
 *
 *  This field is set by the application and cleared by the OTG host.
 *  - 1'b0: Channel disabled
 *  - 1'b1: Channel enabled
 */
#define USB_OTGFS_HCCHAR6_CHENA    (BIT(31))
#define USB_OTGFS_HCCHAR6_CHENA_M  (USB_OTGFS_HCCHAR6_CHENA_V << USB_OTGFS_HCCHAR6_CHENA_S)
#define USB_OTGFS_HCCHAR6_CHENA_V  0x00000001U
#define USB_OTGFS_HCCHAR6_CHENA_S  31

/** USB_OTGFS_HCINT6_REG register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
#define USB_OTGFS_HCINT6_REG(i) (DR_REG_USB_BASE(i) + 0x5c8)
/** USB_OTGFS_HCINT6_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed (XferCompl)
 *
 *  Transfer completed normally without any errors.This bit can be set only by the core
 *  and the application should write 1 to clear it.
 *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
 *  completed with IOC bit set in its descriptor.
 *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
 *  without any errors.
 */
#define USB_OTGFS_HCINT6_XFERCOMPL    (BIT(0))
#define USB_OTGFS_HCINT6_XFERCOMPL_M  (USB_OTGFS_HCINT6_XFERCOMPL_V << USB_OTGFS_HCINT6_XFERCOMPL_S)
#define USB_OTGFS_HCINT6_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_HCINT6_XFERCOMPL_S  0
/** USB_OTGFS_HCINT6_CHHLTD : R/W; bitpos: [1]; default: 0;
 *  Channel Halted (ChHltd)
 *
 *  In non Scatter/Gather DMA mode, it indicates the transfer completed abnormally
 *  either because of any USB transaction error or in response to disable request by
 *  the application or because of a completed transfer.
 *
 *  In Scatter/gather DMA mode, this indicates that transfer completed due to any of
 *  the following
 *  - EOL being set in descriptor
 *  - AHB error
 *  - Excessive transaction errors
 *  - Babble
 *  - Stall
 */
#define USB_OTGFS_HCINT6_CHHLTD    (BIT(1))
#define USB_OTGFS_HCINT6_CHHLTD_M  (USB_OTGFS_HCINT6_CHHLTD_V << USB_OTGFS_HCINT6_CHHLTD_S)
#define USB_OTGFS_HCINT6_CHHLTD_V  0x00000001U
#define USB_OTGFS_HCINT6_CHHLTD_S  1
/** USB_OTGFS_HCINT6_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  This is generated only in Internal DMA mode when there is an AHB error during AHB
 *  read/write. The application can read the corresponding channel's DMA address
 *  register to get the error address.
 */
#define USB_OTGFS_HCINT6_AHBERR    (BIT(2))
#define USB_OTGFS_HCINT6_AHBERR_M  (USB_OTGFS_HCINT6_AHBERR_V << USB_OTGFS_HCINT6_AHBERR_S)
#define USB_OTGFS_HCINT6_AHBERR_V  0x00000001U
#define USB_OTGFS_HCINT6_AHBERR_S  2
/** USB_OTGFS_HCINT6_STALL : R/W; bitpos: [3]; default: 0;
 *  STALL Response Received Interrupt (STALL)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT6_STALL    (BIT(3))
#define USB_OTGFS_HCINT6_STALL_M  (USB_OTGFS_HCINT6_STALL_V << USB_OTGFS_HCINT6_STALL_S)
#define USB_OTGFS_HCINT6_STALL_V  0x00000001U
#define USB_OTGFS_HCINT6_STALL_S  3
/** USB_OTGFS_HCINT6_NAK : R/W; bitpos: [4]; default: 0;
 *  NAK Response Received Interrupt (NAK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT6_NAK    (BIT(4))
#define USB_OTGFS_HCINT6_NAK_M  (USB_OTGFS_HCINT6_NAK_V << USB_OTGFS_HCINT6_NAK_S)
#define USB_OTGFS_HCINT6_NAK_V  0x00000001U
#define USB_OTGFS_HCINT6_NAK_S  4
/** USB_OTGFS_HCINT6_ACK : R/W; bitpos: [5]; default: 0;
 *  ACK Response Received/Transmitted Interrupt (ACK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT6_ACK    (BIT(5))
#define USB_OTGFS_HCINT6_ACK_M  (USB_OTGFS_HCINT6_ACK_V << USB_OTGFS_HCINT6_ACK_S)
#define USB_OTGFS_HCINT6_ACK_V  0x00000001U
#define USB_OTGFS_HCINT6_ACK_S  5
/** USB_OTGFS_HCINT6_NYET : R/W; bitpos: [6]; default: 0;
 *  NYET Response Received Interrupt (NYET)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT6_NYET    (BIT(6))
#define USB_OTGFS_HCINT6_NYET_M  (USB_OTGFS_HCINT6_NYET_V << USB_OTGFS_HCINT6_NYET_S)
#define USB_OTGFS_HCINT6_NYET_V  0x00000001U
#define USB_OTGFS_HCINT6_NYET_S  6
/** USB_OTGFS_HCINT6_XACTERR : R/W; bitpos: [7]; default: 0;
 *  Transaction Error (XactErr)
 *
 *  Indicates one of the following errors occurred on the USB.
 *  - CRC check failure
 *  - Timeout
 *  - Bit stuff error
 *  - False EOP
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT6_XACTERR    (BIT(7))
#define USB_OTGFS_HCINT6_XACTERR_M  (USB_OTGFS_HCINT6_XACTERR_V << USB_OTGFS_HCINT6_XACTERR_S)
#define USB_OTGFS_HCINT6_XACTERR_V  0x00000001U
#define USB_OTGFS_HCINT6_XACTERR_S  7
/** USB_OTGFS_HCINT6_BBLERR : R/W; bitpos: [8]; default: 0;
 *  Babble Error (BblErr)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
 *  This bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_HCINT6_BBLERR    (BIT(8))
#define USB_OTGFS_HCINT6_BBLERR_M  (USB_OTGFS_HCINT6_BBLERR_V << USB_OTGFS_HCINT6_BBLERR_S)
#define USB_OTGFS_HCINT6_BBLERR_V  0x00000001U
#define USB_OTGFS_HCINT6_BBLERR_S  8
/** USB_OTGFS_HCINT6_FRMOVRUN : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun (FrmOvrun).
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core. This bit can be set only by the core and the application should write
 *  1 to clear
 *  it.
 */
#define USB_OTGFS_HCINT6_FRMOVRUN    (BIT(9))
#define USB_OTGFS_HCINT6_FRMOVRUN_M  (USB_OTGFS_HCINT6_FRMOVRUN_V << USB_OTGFS_HCINT6_FRMOVRUN_S)
#define USB_OTGFS_HCINT6_FRMOVRUN_V  0x00000001U
#define USB_OTGFS_HCINT6_FRMOVRUN_S  9
/** USB_OTGFS_HCINT6_DATATGLERR : R/W; bitpos: [10]; default: 0;
 *
 *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
 *  application should write 1 to clear
 *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core.
 */
#define USB_OTGFS_HCINT6_DATATGLERR    (BIT(10))
#define USB_OTGFS_HCINT6_DATATGLERR_M  (USB_OTGFS_HCINT6_DATATGLERR_V << USB_OTGFS_HCINT6_DATATGLERR_S)
#define USB_OTGFS_HCINT6_DATATGLERR_V  0x00000001U
#define USB_OTGFS_HCINT6_DATATGLERR_S  10
/** USB_OTGFS_HCINT6_BNAINTR : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process. BNA will not be generated
 *  for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT6_BNAINTR    (BIT(11))
#define USB_OTGFS_HCINT6_BNAINTR_M  (USB_OTGFS_HCINT6_BNAINTR_V << USB_OTGFS_HCINT6_BNAINTR_S)
#define USB_OTGFS_HCINT6_BNAINTR_V  0x00000001U
#define USB_OTGFS_HCINT6_BNAINTR_S  11
/** USB_OTGFS_HCINT6_XCS_XACT_ERR : R/W; bitpos: [12]; default: 0;
 *  Excessive Transaction Error (XCS_XACT_ERR)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
 *  not be generated for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT6_XCS_XACT_ERR    (BIT(12))
#define USB_OTGFS_HCINT6_XCS_XACT_ERR_M  (USB_OTGFS_HCINT6_XCS_XACT_ERR_V << USB_OTGFS_HCINT6_XCS_XACT_ERR_S)
#define USB_OTGFS_HCINT6_XCS_XACT_ERR_V  0x00000001U
#define USB_OTGFS_HCINT6_XCS_XACT_ERR_S  12
/** USB_OTGFS_HCINT6_DESC_LST_ROLLINTR : R/W; bitpos: [13]; default: 0;
 *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when the corresponding channel's descriptor list rolls over.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT6_DESC_LST_ROLLINTR    (BIT(13))
#define USB_OTGFS_HCINT6_DESC_LST_ROLLINTR_M  (USB_OTGFS_HCINT6_DESC_LST_ROLLINTR_V << USB_OTGFS_HCINT6_DESC_LST_ROLLINTR_S)
#define USB_OTGFS_HCINT6_DESC_LST_ROLLINTR_V  0x00000001U
#define USB_OTGFS_HCINT6_DESC_LST_ROLLINTR_S  13

/** USB_OTGFS_HCINTMSK6_REG register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
#define USB_OTGFS_HCINTMSK6_REG(i) (DR_REG_USB_BASE(i) + 0x5cc)
/** USB_OTGFS_HCINTMSK6_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *
 *  Transfer Completed Mask (XferComplMsk)
 */
#define USB_OTGFS_HCINTMSK6_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_HCINTMSK6_XFERCOMPLMSK_M  (USB_OTGFS_HCINTMSK6_XFERCOMPLMSK_V << USB_OTGFS_HCINTMSK6_XFERCOMPLMSK_S)
#define USB_OTGFS_HCINTMSK6_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK6_XFERCOMPLMSK_S  0
/** USB_OTGFS_HCINTMSK6_CHHLTDMSK : R/W; bitpos: [1]; default: 0;
 *
 *  Channel Halted Mask (ChHltdMsk)
 */
#define USB_OTGFS_HCINTMSK6_CHHLTDMSK    (BIT(1))
#define USB_OTGFS_HCINTMSK6_CHHLTDMSK_M  (USB_OTGFS_HCINTMSK6_CHHLTDMSK_V << USB_OTGFS_HCINTMSK6_CHHLTDMSK_S)
#define USB_OTGFS_HCINTMSK6_CHHLTDMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK6_CHHLTDMSK_S  1
/** USB_OTGFS_HCINTMSK6_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *
 *  AHB Error Mask (AHBErrMsk)
 *  In  scatter/gather  DMA  mode  for  host,
 *  interrupts will not be generated due to the corresponding bits set in
 *  HCINTn.
 */
#define USB_OTGFS_HCINTMSK6_AHBERRMSK    (BIT(2))
#define USB_OTGFS_HCINTMSK6_AHBERRMSK_M  (USB_OTGFS_HCINTMSK6_AHBERRMSK_V << USB_OTGFS_HCINTMSK6_AHBERRMSK_S)
#define USB_OTGFS_HCINTMSK6_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK6_AHBERRMSK_S  2
/** USB_OTGFS_HCINTMSK6_BNAINTRMSK : R/W; bitpos: [11]; default: 0;
 *
 *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK6_BNAINTRMSK    (BIT(11))
#define USB_OTGFS_HCINTMSK6_BNAINTRMSK_M  (USB_OTGFS_HCINTMSK6_BNAINTRMSK_V << USB_OTGFS_HCINTMSK6_BNAINTRMSK_S)
#define USB_OTGFS_HCINTMSK6_BNAINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK6_BNAINTRMSK_S  11
/** USB_OTGFS_HCINTMSK6_DESC_LST_ROLLINTRMSK : R/W; bitpos: [13]; default: 0;
 *
 *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK6_DESC_LST_ROLLINTRMSK    (BIT(13))
#define USB_OTGFS_HCINTMSK6_DESC_LST_ROLLINTRMSK_M  (USB_OTGFS_HCINTMSK6_DESC_LST_ROLLINTRMSK_V << USB_OTGFS_HCINTMSK6_DESC_LST_ROLLINTRMSK_S)
#define USB_OTGFS_HCINTMSK6_DESC_LST_ROLLINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK6_DESC_LST_ROLLINTRMSK_S  13

/** USB_OTGFS_HCTSIZ6_REG register
 *  Host Channel 6 Transfer Size Register
 */
#define USB_OTGFS_HCTSIZ6_REG(i) (DR_REG_USB_BASE(i) + 0x5d0)
/** USB_OTGFS_HCTSIZ6_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  For an OUT, this field is the number of data bytes the host sends during the
 *  transfer.
 *
 *  For an IN, this field is the buffer size that the application has Reserved for the
 *  transfer. The application is expected to program this field as an integer multiple
 *  of the maximum packet size for IN transactions (periodic and non-periodic).
 *
 *  The width of this counter is specified as Width of Transfer Size Counters during
 *  coreConsultant configuration (parameter OTG_TRANS_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ6_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_HCTSIZ6_XFERSIZE_M  (USB_OTGFS_HCTSIZ6_XFERSIZE_V << USB_OTGFS_HCTSIZ6_XFERSIZE_S)
#define USB_OTGFS_HCTSIZ6_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_HCTSIZ6_XFERSIZE_S  0
/** USB_OTGFS_HCTSIZ6_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  This field is programmed by the application with the expected number of packets to
 *  be transmitted (OUT) or received (IN).
 *
 *  The host decrements this count on every successful transmission or reception of an
 *  OUT/IN packet. Once this count reaches zero, the application is interrupted to
 *  indicate normal completion.
 *
 *  The width of this counter is specified as Width of Packet Counters during
 *  coreConsultant configuration (parameter OTG_PACKET_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ6_PKTCNT    0x0000007FU
#define USB_OTGFS_HCTSIZ6_PKTCNT_M  (USB_OTGFS_HCTSIZ6_PKTCNT_V << USB_OTGFS_HCTSIZ6_PKTCNT_S)
#define USB_OTGFS_HCTSIZ6_PKTCNT_V  0x0000007FU
#define USB_OTGFS_HCTSIZ6_PKTCNT_S  19
/** USB_OTGFS_HCTSIZ6_PID : R/W; bitpos: [30:29]; default: 0;
 *  PID (Pid)
 *
 *  The application programs this field with the type of PID to use for the initial
 *  transaction. The host maintains this field for the rest of the transfer.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_OTGFS_HCTSIZ6_PID    0x00000003U
#define USB_OTGFS_HCTSIZ6_PID_M  (USB_OTGFS_HCTSIZ6_PID_V << USB_OTGFS_HCTSIZ6_PID_S)
#define USB_OTGFS_HCTSIZ6_PID_V  0x00000003U
#define USB_OTGFS_HCTSIZ6_PID_S  29
/** USB_OTGFS_HCTSIZ6_DOPNG : R/W; bitpos: [31]; default: 0;
 *  Do Ping (DoPng)
 *
 *  This bit is used only for OUT transfers.
 *  Setting this field to 1 directs the host to do PING protocol.
 *
 *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
 *  it disables the channel.
 */
#define USB_OTGFS_HCTSIZ6_DOPNG    (BIT(31))
#define USB_OTGFS_HCTSIZ6_DOPNG_M  (USB_OTGFS_HCTSIZ6_DOPNG_V << USB_OTGFS_HCTSIZ6_DOPNG_S)
#define USB_OTGFS_HCTSIZ6_DOPNG_V  0x00000001U
#define USB_OTGFS_HCTSIZ6_DOPNG_S  31

/** USB_OTGFS_HCDMA6_REG register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
#define USB_OTGFS_HCDMA6_REG(i) (DR_REG_USB_BASE(i) + 0x5d4)
/** USB_OTGFS_HCDMA6_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  In Buffer DMA Mode:
 *
 *  [31:0]: DMA Address (DMAAddr)
 *
 *  This field holds the start address in the external memory from which the data for
 *  the endpoint must be fetched or to which it must be stored. This register is
 *  incremented on every AHB transaction.
 *
 *  Reset: X if not programmed as the register is in SPRAM.
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *
 *  [31:9]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the start address of the 512 bytes page. The first descriptor in
 *  the list should be located in this address. The first descriptor may be or may not
 *  be ready. The core starts processing the list from the CTD value.
 *
 *  [8:3]: Current Transfer Desc(CTD)
 *
 *  This value is in terms of number of descriptors. The values can be from 0 to 63.
 *  - 0 -  1 descriptor.
 *  - 63 - 64 descriptors.
 *  This field indicates the current descriptor processed in the list. This field is
 *  updated both by application and the core. For example, if the application enables
 *  the channel after programming CTD=5, then the core will start processing the sixth
 *  descriptor. The address is obtained by adding a value of (8bytes*5=) 40(decimal) to
 *  DMAAddr.
 *
 *  Reset: 6'h0
 *
 *  [2:0]: Reserved
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Isochronous:
 *
 *  [31:N]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the address of the 2*(nTD+1) bytes of locations in which the
 *  isochronous descriptors are present where N is based on nTD as follows:
 *  - [31:N]: Base Address
 *  - [N-1:3]: Offset
 *  - [2:0]: 000
 *  For HS ISOC, if nTD is,
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  - 127, N=10
 *  - 255, N=11
 *  For FS ISOC, if nTD is,
 *  - 1, N=4
 *  - 3, N=5
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  [N-1:3]: Current Transfer Desc(CTD)
 *
 *  CTD for isochronous is based on the current frame/(micro)frame value. Need to be
 *  set to zero by application.
 *
 *  Reset: (N+1:3)'h0
 *
 *  [2:0]: Reserved
 */
#define USB_OTGFS_HCDMA6_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_HCDMA6_DMAADDR_M  (USB_OTGFS_HCDMA6_DMAADDR_V << USB_OTGFS_HCDMA6_DMAADDR_S)
#define USB_OTGFS_HCDMA6_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMA6_DMAADDR_S  0

/** USB_OTGFS_HCDMAB6_REG register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
#define USB_OTGFS_HCDMAB6_REG(i) (DR_REG_USB_BASE(i) + 0x5dc)
/** USB_OTGFS_HCDMAB6_HCDMAB : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.
 *  This register is updated as and when the data transfer for the corresponding end
 *  point
 *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
 *  this
 *  field is reserved.
 */
#define USB_OTGFS_HCDMAB6_HCDMAB    0xFFFFFFFFU
#define USB_OTGFS_HCDMAB6_HCDMAB_M  (USB_OTGFS_HCDMAB6_HCDMAB_V << USB_OTGFS_HCDMAB6_HCDMAB_S)
#define USB_OTGFS_HCDMAB6_HCDMAB_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMAB6_HCDMAB_S  0

/** USB_OTGFS_HCCHAR7_REG register
 *  Host Channel 7 Characteristics Register
 */
#define USB_OTGFS_HCCHAR7_REG(i) (DR_REG_USB_BASE(i) + 0x5e0)
/** USB_OTGFS_HCCHAR7_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  Indicates the maximum packet size of the associated endpoint.
 */
#define USB_OTGFS_HCCHAR7_MPS    0x000007FFU
#define USB_OTGFS_HCCHAR7_MPS_M  (USB_OTGFS_HCCHAR7_MPS_V << USB_OTGFS_HCCHAR7_MPS_S)
#define USB_OTGFS_HCCHAR7_MPS_V  0x000007FFU
#define USB_OTGFS_HCCHAR7_MPS_S  0
/** USB_OTGFS_HCCHAR7_EPNUM : R/W; bitpos: [14:11]; default: 0;
 *  Endpoint Number (EPNum)
 *
 *  Indicates the endpoint number on the device serving as the data source or sink.
 */
#define USB_OTGFS_HCCHAR7_EPNUM    0x0000000FU
#define USB_OTGFS_HCCHAR7_EPNUM_M  (USB_OTGFS_HCCHAR7_EPNUM_V << USB_OTGFS_HCCHAR7_EPNUM_S)
#define USB_OTGFS_HCCHAR7_EPNUM_V  0x0000000FU
#define USB_OTGFS_HCCHAR7_EPNUM_S  11
/** USB_OTGFS_HCCHAR7_EPDIR : R/W; bitpos: [15]; default: 0;
 *  Endpoint Direction (EPDir)
 *
 *  Indicates whether the transaction is IN or OUT.
 *  - 1'b0: OUT
 *  - 1'b1: IN
 */
#define USB_OTGFS_HCCHAR7_EPDIR    (BIT(15))
#define USB_OTGFS_HCCHAR7_EPDIR_M  (USB_OTGFS_HCCHAR7_EPDIR_V << USB_OTGFS_HCCHAR7_EPDIR_S)
#define USB_OTGFS_HCCHAR7_EPDIR_V  0x00000001U
#define USB_OTGFS_HCCHAR7_EPDIR_S  15
/** USB_OTGFS_HCCHAR7_LSPDDEV : R/W; bitpos: [17]; default: 0;
 *  Low-Speed Device (LSpdDev)
 *
 *  This field is Set by the application to indicate that this channel is communicating
 *  to a low-speed device.
 *
 *  The application must program this bit when a low speed device is connected to the
 *  host through an FS HUB. The DWC_otg Host core uses this field to drive the
 *  XCVR_SELECT signal to 2'b11 while communicating to the LS Device through the FS hub.
 *
 *  Note: In a peer to peer setup, the DWC_otg Host core ignores this bit even if it is
 *  set by the application software.
 */
#define USB_OTGFS_HCCHAR7_LSPDDEV    (BIT(17))
#define USB_OTGFS_HCCHAR7_LSPDDEV_M  (USB_OTGFS_HCCHAR7_LSPDDEV_V << USB_OTGFS_HCCHAR7_LSPDDEV_S)
#define USB_OTGFS_HCCHAR7_LSPDDEV_V  0x00000001U
#define USB_OTGFS_HCCHAR7_LSPDDEV_S  17
/** USB_OTGFS_HCCHAR7_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Indicates the transfer type selected.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_HCCHAR7_EPTYPE    0x00000003U
#define USB_OTGFS_HCCHAR7_EPTYPE_M  (USB_OTGFS_HCCHAR7_EPTYPE_V << USB_OTGFS_HCCHAR7_EPTYPE_S)
#define USB_OTGFS_HCCHAR7_EPTYPE_V  0x00000003U
#define USB_OTGFS_HCCHAR7_EPTYPE_S  18
/** USB_OTGFS_HCCHAR7_EC : R/W; bitpos: [21:20]; default: 0;
 *  Multi Count (MC) / Error Count (EC)
 *
 *  When the Split Enable bit of the Host Channel-n Split Control
 *  register (HCSPLTn.SpltEna) is reset (1'b0), this field indicates to
 *  the host the number of transactions that must be executed per
 *  microframe for this periodic endpoint. For non periodic transfers,
 *  this field is used only in DMA mode, and specifies the number
 *  packets to be fetched for this channel before the internal DMA
 *  engine changes arbitration.
 *  - 2'b00: Reserved This field yields undefined results.
 *  - 2'b01: 1 transaction
 *  - 2'b10: 2 transactions to be issued for this endpoint per microframe
 *  - 2'b11: 3 transactions to be issued for this endpoint per microframe
 *  When HCSPLTn.SpltEna is Set (1'b1), this field indicates the
 *  number of immediate retries to be performed for a periodic split
 *  transactions on transaction errors. This field must be Set to at
 *  least 2'b01.
 */
#define USB_OTGFS_HCCHAR7_EC    0x00000003U
#define USB_OTGFS_HCCHAR7_EC_M  (USB_OTGFS_HCCHAR7_EC_V << USB_OTGFS_HCCHAR7_EC_S)
#define USB_OTGFS_HCCHAR7_EC_V  0x00000003U
#define USB_OTGFS_HCCHAR7_EC_S  20
/** USB_OTGFS_HCCHAR7_DEVADDR : R/W; bitpos: [28:22]; default: 0;
 *  Device Address (DevAddr)
 *
 *  This field selects the specific device serving as the data source
 *  or sink.
 */
#define USB_OTGFS_HCCHAR7_DEVADDR    0x0000007FU
#define USB_OTGFS_HCCHAR7_DEVADDR_M  (USB_OTGFS_HCCHAR7_DEVADDR_V << USB_OTGFS_HCCHAR7_DEVADDR_S)
#define USB_OTGFS_HCCHAR7_DEVADDR_V  0x0000007FU
#define USB_OTGFS_HCCHAR7_DEVADDR_S  22
/** USB_OTGFS_HCCHAR7_ODDFRM : R/W; bitpos: [29]; default: 0;
 *  Odd Frame (OddFrm)
 *
 *  This field is set (reset) by the application to indicate that the OTG host must
 *  perform
 *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
 *  (isochronous and interrupt) transactions.
 *  - 1'b0: Even (micro)Frame
 *  - 1'b1: Odd (micro)Frame
 */
#define USB_OTGFS_HCCHAR7_ODDFRM    (BIT(29))
#define USB_OTGFS_HCCHAR7_ODDFRM_M  (USB_OTGFS_HCCHAR7_ODDFRM_V << USB_OTGFS_HCCHAR7_ODDFRM_S)
#define USB_OTGFS_HCCHAR7_ODDFRM_V  0x00000001U
#define USB_OTGFS_HCCHAR7_ODDFRM_S  29
/** USB_OTGFS_HCCHAR7_CHDIS : R/W; bitpos: [30]; default: 0;
 *  Channel Disable (ChDis)
 *
 *  The application sets this bit to stop transmitting/receiving data
 *  on a channel, even before the transfer for that channel is
 *  complete. The application must wait for the Channel Disabled
 *  interrupt before treating the channel as disabled.
 */
#define USB_OTGFS_HCCHAR7_CHDIS    (BIT(30))
#define USB_OTGFS_HCCHAR7_CHDIS_M  (USB_OTGFS_HCCHAR7_CHDIS_V << USB_OTGFS_HCCHAR7_CHDIS_S)
#define USB_OTGFS_HCCHAR7_CHDIS_V  0x00000001U
#define USB_OTGFS_HCCHAR7_CHDIS_S  30
/** USB_OTGFS_HCCHAR7_CHENA : R/W; bitpos: [31]; default: 0;
 *  Channel Enable (ChEna)
 *
 *  When Scatter/Gather mode is enabled
 *  - 1'b0: Indicates that the descriptor structure is not yet ready.
 *  - 1'b1:  Indicates  that  the  descriptor  structure  and  data  buffer  with data
 *  is setup and this channel can access the descriptor.
 *  When Scatter/Gather mode is disabled
 *
 *  This field is set by the application and cleared by the OTG host.
 *  - 1'b0: Channel disabled
 *  - 1'b1: Channel enabled
 */
#define USB_OTGFS_HCCHAR7_CHENA    (BIT(31))
#define USB_OTGFS_HCCHAR7_CHENA_M  (USB_OTGFS_HCCHAR7_CHENA_V << USB_OTGFS_HCCHAR7_CHENA_S)
#define USB_OTGFS_HCCHAR7_CHENA_V  0x00000001U
#define USB_OTGFS_HCCHAR7_CHENA_S  31

/** USB_OTGFS_HCINT7_REG register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
#define USB_OTGFS_HCINT7_REG(i) (DR_REG_USB_BASE(i) + 0x5e8)
/** USB_OTGFS_HCINT7_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed (XferCompl)
 *
 *  Transfer completed normally without any errors.This bit can be set only by the core
 *  and the application should write 1 to clear it.
 *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
 *  completed with IOC bit set in its descriptor.
 *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
 *  without any errors.
 */
#define USB_OTGFS_HCINT7_XFERCOMPL    (BIT(0))
#define USB_OTGFS_HCINT7_XFERCOMPL_M  (USB_OTGFS_HCINT7_XFERCOMPL_V << USB_OTGFS_HCINT7_XFERCOMPL_S)
#define USB_OTGFS_HCINT7_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_HCINT7_XFERCOMPL_S  0
/** USB_OTGFS_HCINT7_CHHLTD : R/W; bitpos: [1]; default: 0;
 *  Channel Halted (ChHltd)
 *
 *  In non Scatter/Gather DMA mode, it indicates the transfer completed abnormally
 *  either because of any USB transaction error or in response to disable request by
 *  the application or because of a completed transfer.
 *
 *  In Scatter/gather DMA mode, this indicates that transfer completed due to any of
 *  the following
 *  - EOL being set in descriptor
 *  - AHB error
 *  - Excessive transaction errors
 *  - Babble
 *  - Stall
 */
#define USB_OTGFS_HCINT7_CHHLTD    (BIT(1))
#define USB_OTGFS_HCINT7_CHHLTD_M  (USB_OTGFS_HCINT7_CHHLTD_V << USB_OTGFS_HCINT7_CHHLTD_S)
#define USB_OTGFS_HCINT7_CHHLTD_V  0x00000001U
#define USB_OTGFS_HCINT7_CHHLTD_S  1
/** USB_OTGFS_HCINT7_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  This is generated only in Internal DMA mode when there is an AHB error during AHB
 *  read/write. The application can read the corresponding channel's DMA address
 *  register to get the error address.
 */
#define USB_OTGFS_HCINT7_AHBERR    (BIT(2))
#define USB_OTGFS_HCINT7_AHBERR_M  (USB_OTGFS_HCINT7_AHBERR_V << USB_OTGFS_HCINT7_AHBERR_S)
#define USB_OTGFS_HCINT7_AHBERR_V  0x00000001U
#define USB_OTGFS_HCINT7_AHBERR_S  2
/** USB_OTGFS_HCINT7_STALL : R/W; bitpos: [3]; default: 0;
 *  STALL Response Received Interrupt (STALL)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT7_STALL    (BIT(3))
#define USB_OTGFS_HCINT7_STALL_M  (USB_OTGFS_HCINT7_STALL_V << USB_OTGFS_HCINT7_STALL_S)
#define USB_OTGFS_HCINT7_STALL_V  0x00000001U
#define USB_OTGFS_HCINT7_STALL_S  3
/** USB_OTGFS_HCINT7_NAK : R/W; bitpos: [4]; default: 0;
 *  NAK Response Received Interrupt (NAK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT7_NAK    (BIT(4))
#define USB_OTGFS_HCINT7_NAK_M  (USB_OTGFS_HCINT7_NAK_V << USB_OTGFS_HCINT7_NAK_S)
#define USB_OTGFS_HCINT7_NAK_V  0x00000001U
#define USB_OTGFS_HCINT7_NAK_S  4
/** USB_OTGFS_HCINT7_ACK : R/W; bitpos: [5]; default: 0;
 *  ACK Response Received/Transmitted Interrupt (ACK)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT7_ACK    (BIT(5))
#define USB_OTGFS_HCINT7_ACK_M  (USB_OTGFS_HCINT7_ACK_V << USB_OTGFS_HCINT7_ACK_S)
#define USB_OTGFS_HCINT7_ACK_V  0x00000001U
#define USB_OTGFS_HCINT7_ACK_S  5
/** USB_OTGFS_HCINT7_NYET : R/W; bitpos: [6]; default: 0;
 *  NYET Response Received Interrupt (NYET)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT7_NYET    (BIT(6))
#define USB_OTGFS_HCINT7_NYET_M  (USB_OTGFS_HCINT7_NYET_V << USB_OTGFS_HCINT7_NYET_S)
#define USB_OTGFS_HCINT7_NYET_V  0x00000001U
#define USB_OTGFS_HCINT7_NYET_S  6
/** USB_OTGFS_HCINT7_XACTERR : R/W; bitpos: [7]; default: 0;
 *  Transaction Error (XactErr)
 *
 *  Indicates one of the following errors occurred on the USB.
 *  - CRC check failure
 *  - Timeout
 *  - Bit stuff error
 *  - False EOP
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
 *  core.This bit can be set only by the core and the application should write 1 to
 *  clear it.
 */
#define USB_OTGFS_HCINT7_XACTERR    (BIT(7))
#define USB_OTGFS_HCINT7_XACTERR_M  (USB_OTGFS_HCINT7_XACTERR_V << USB_OTGFS_HCINT7_XACTERR_S)
#define USB_OTGFS_HCINT7_XACTERR_V  0x00000001U
#define USB_OTGFS_HCINT7_XACTERR_S  7
/** USB_OTGFS_HCINT7_BBLERR : R/W; bitpos: [8]; default: 0;
 *  Babble Error (BblErr)
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
 *  This bit can be set only by the core and the application should write 1 to clear it.
 */
#define USB_OTGFS_HCINT7_BBLERR    (BIT(8))
#define USB_OTGFS_HCINT7_BBLERR_M  (USB_OTGFS_HCINT7_BBLERR_V << USB_OTGFS_HCINT7_BBLERR_S)
#define USB_OTGFS_HCINT7_BBLERR_V  0x00000001U
#define USB_OTGFS_HCINT7_BBLERR_S  8
/** USB_OTGFS_HCINT7_FRMOVRUN : R/W; bitpos: [9]; default: 0;
 *  Frame Overrun (FrmOvrun).
 *
 *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core. This bit can be set only by the core and the application should write
 *  1 to clear
 *  it.
 */
#define USB_OTGFS_HCINT7_FRMOVRUN    (BIT(9))
#define USB_OTGFS_HCINT7_FRMOVRUN_M  (USB_OTGFS_HCINT7_FRMOVRUN_V << USB_OTGFS_HCINT7_FRMOVRUN_S)
#define USB_OTGFS_HCINT7_FRMOVRUN_V  0x00000001U
#define USB_OTGFS_HCINT7_FRMOVRUN_S  9
/** USB_OTGFS_HCINT7_DATATGLERR : R/W; bitpos: [10]; default: 0;
 *
 *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
 *  application should write 1 to clear
 *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
 *  in the core.
 */
#define USB_OTGFS_HCINT7_DATATGLERR    (BIT(10))
#define USB_OTGFS_HCINT7_DATATGLERR_M  (USB_OTGFS_HCINT7_DATATGLERR_V << USB_OTGFS_HCINT7_DATATGLERR_S)
#define USB_OTGFS_HCINT7_DATATGLERR_V  0x00000001U
#define USB_OTGFS_HCINT7_DATATGLERR_S  10
/** USB_OTGFS_HCINT7_BNAINTR : R/W; bitpos: [11]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process. BNA will not be generated
 *  for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT7_BNAINTR    (BIT(11))
#define USB_OTGFS_HCINT7_BNAINTR_M  (USB_OTGFS_HCINT7_BNAINTR_V << USB_OTGFS_HCINT7_BNAINTR_S)
#define USB_OTGFS_HCINT7_BNAINTR_V  0x00000001U
#define USB_OTGFS_HCINT7_BNAINTR_S  11
/** USB_OTGFS_HCINT7_XCS_XACT_ERR : R/W; bitpos: [12]; default: 0;
 *  Excessive Transaction Error (XCS_XACT_ERR)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
 *  not be generated for Isochronous channels.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT7_XCS_XACT_ERR    (BIT(12))
#define USB_OTGFS_HCINT7_XCS_XACT_ERR_M  (USB_OTGFS_HCINT7_XCS_XACT_ERR_V << USB_OTGFS_HCINT7_XCS_XACT_ERR_S)
#define USB_OTGFS_HCINT7_XCS_XACT_ERR_V  0x00000001U
#define USB_OTGFS_HCINT7_XCS_XACT_ERR_S  12
/** USB_OTGFS_HCINT7_DESC_LST_ROLLINTR : R/W; bitpos: [13]; default: 0;
 *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
 *  bit
 *  when the corresponding channel's descriptor list rolls over.
 *  For non Scatter/Gather DMA mode, this bit is reserved.
 */
#define USB_OTGFS_HCINT7_DESC_LST_ROLLINTR    (BIT(13))
#define USB_OTGFS_HCINT7_DESC_LST_ROLLINTR_M  (USB_OTGFS_HCINT7_DESC_LST_ROLLINTR_V << USB_OTGFS_HCINT7_DESC_LST_ROLLINTR_S)
#define USB_OTGFS_HCINT7_DESC_LST_ROLLINTR_V  0x00000001U
#define USB_OTGFS_HCINT7_DESC_LST_ROLLINTR_S  13

/** USB_OTGFS_HCINTMSK7_REG register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
#define USB_OTGFS_HCINTMSK7_REG(i) (DR_REG_USB_BASE(i) + 0x5ec)
/** USB_OTGFS_HCINTMSK7_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *
 *  Transfer Completed Mask (XferComplMsk)
 */
#define USB_OTGFS_HCINTMSK7_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_HCINTMSK7_XFERCOMPLMSK_M  (USB_OTGFS_HCINTMSK7_XFERCOMPLMSK_V << USB_OTGFS_HCINTMSK7_XFERCOMPLMSK_S)
#define USB_OTGFS_HCINTMSK7_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK7_XFERCOMPLMSK_S  0
/** USB_OTGFS_HCINTMSK7_CHHLTDMSK : R/W; bitpos: [1]; default: 0;
 *
 *  Channel Halted Mask (ChHltdMsk)
 */
#define USB_OTGFS_HCINTMSK7_CHHLTDMSK    (BIT(1))
#define USB_OTGFS_HCINTMSK7_CHHLTDMSK_M  (USB_OTGFS_HCINTMSK7_CHHLTDMSK_V << USB_OTGFS_HCINTMSK7_CHHLTDMSK_S)
#define USB_OTGFS_HCINTMSK7_CHHLTDMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK7_CHHLTDMSK_S  1
/** USB_OTGFS_HCINTMSK7_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *
 *  AHB Error Mask (AHBErrMsk)
 *  In  scatter/gather  DMA  mode  for  host,
 *  interrupts will not be generated due to the corresponding bits set in
 *  HCINTn.
 */
#define USB_OTGFS_HCINTMSK7_AHBERRMSK    (BIT(2))
#define USB_OTGFS_HCINTMSK7_AHBERRMSK_M  (USB_OTGFS_HCINTMSK7_AHBERRMSK_V << USB_OTGFS_HCINTMSK7_AHBERRMSK_S)
#define USB_OTGFS_HCINTMSK7_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK7_AHBERRMSK_S  2
/** USB_OTGFS_HCINTMSK7_BNAINTRMSK : R/W; bitpos: [11]; default: 0;
 *
 *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK7_BNAINTRMSK    (BIT(11))
#define USB_OTGFS_HCINTMSK7_BNAINTRMSK_M  (USB_OTGFS_HCINTMSK7_BNAINTRMSK_V << USB_OTGFS_HCINTMSK7_BNAINTRMSK_S)
#define USB_OTGFS_HCINTMSK7_BNAINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK7_BNAINTRMSK_S  11
/** USB_OTGFS_HCINTMSK7_DESC_LST_ROLLINTRMSK : R/W; bitpos: [13]; default: 0;
 *
 *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 */
#define USB_OTGFS_HCINTMSK7_DESC_LST_ROLLINTRMSK    (BIT(13))
#define USB_OTGFS_HCINTMSK7_DESC_LST_ROLLINTRMSK_M  (USB_OTGFS_HCINTMSK7_DESC_LST_ROLLINTRMSK_V << USB_OTGFS_HCINTMSK7_DESC_LST_ROLLINTRMSK_S)
#define USB_OTGFS_HCINTMSK7_DESC_LST_ROLLINTRMSK_V  0x00000001U
#define USB_OTGFS_HCINTMSK7_DESC_LST_ROLLINTRMSK_S  13

/** USB_OTGFS_HCTSIZ7_REG register
 *  Host Channel 7 Transfer Size Register
 */
#define USB_OTGFS_HCTSIZ7_REG(i) (DR_REG_USB_BASE(i) + 0x5f0)
/** USB_OTGFS_HCTSIZ7_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  For an OUT, this field is the number of data bytes the host sends during the
 *  transfer.
 *
 *  For an IN, this field is the buffer size that the application has Reserved for the
 *  transfer. The application is expected to program this field as an integer multiple
 *  of the maximum packet size for IN transactions (periodic and non-periodic).
 *
 *  The width of this counter is specified as Width of Transfer Size Counters during
 *  coreConsultant configuration (parameter OTG_TRANS_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ7_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_HCTSIZ7_XFERSIZE_M  (USB_OTGFS_HCTSIZ7_XFERSIZE_V << USB_OTGFS_HCTSIZ7_XFERSIZE_S)
#define USB_OTGFS_HCTSIZ7_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_HCTSIZ7_XFERSIZE_S  0
/** USB_OTGFS_HCTSIZ7_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  This field is programmed by the application with the expected number of packets to
 *  be transmitted (OUT) or received (IN).
 *
 *  The host decrements this count on every successful transmission or reception of an
 *  OUT/IN packet. Once this count reaches zero, the application is interrupted to
 *  indicate normal completion.
 *
 *  The width of this counter is specified as Width of Packet Counters during
 *  coreConsultant configuration (parameter OTG_PACKET_COUNT_WIDTH).
 */
#define USB_OTGFS_HCTSIZ7_PKTCNT    0x0000007FU
#define USB_OTGFS_HCTSIZ7_PKTCNT_M  (USB_OTGFS_HCTSIZ7_PKTCNT_V << USB_OTGFS_HCTSIZ7_PKTCNT_S)
#define USB_OTGFS_HCTSIZ7_PKTCNT_V  0x0000007FU
#define USB_OTGFS_HCTSIZ7_PKTCNT_S  19
/** USB_OTGFS_HCTSIZ7_PID : R/W; bitpos: [30:29]; default: 0;
 *  PID (Pid)
 *
 *  The application programs this field with the type of PID to use for the initial
 *  transaction. The host maintains this field for the rest of the transfer.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA (non-control)/SETUP (control)
 */
#define USB_OTGFS_HCTSIZ7_PID    0x00000003U
#define USB_OTGFS_HCTSIZ7_PID_M  (USB_OTGFS_HCTSIZ7_PID_V << USB_OTGFS_HCTSIZ7_PID_S)
#define USB_OTGFS_HCTSIZ7_PID_V  0x00000003U
#define USB_OTGFS_HCTSIZ7_PID_S  29
/** USB_OTGFS_HCTSIZ7_DOPNG : R/W; bitpos: [31]; default: 0;
 *  Do Ping (DoPng)
 *
 *  This bit is used only for OUT transfers.
 *  Setting this field to 1 directs the host to do PING protocol.
 *
 *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
 *  it disables the channel.
 */
#define USB_OTGFS_HCTSIZ7_DOPNG    (BIT(31))
#define USB_OTGFS_HCTSIZ7_DOPNG_M  (USB_OTGFS_HCTSIZ7_DOPNG_V << USB_OTGFS_HCTSIZ7_DOPNG_S)
#define USB_OTGFS_HCTSIZ7_DOPNG_V  0x00000001U
#define USB_OTGFS_HCTSIZ7_DOPNG_S  31

/** USB_OTGFS_HCDMA7_REG register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
#define USB_OTGFS_HCDMA7_REG(i) (DR_REG_USB_BASE(i) + 0x5f4)
/** USB_OTGFS_HCDMA7_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  In Buffer DMA Mode:
 *
 *  [31:0]: DMA Address (DMAAddr)
 *
 *  This field holds the start address in the external memory from which the data for
 *  the endpoint must be fetched or to which it must be stored. This register is
 *  incremented on every AHB transaction.
 *
 *  Reset: X if not programmed as the register is in SPRAM.
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Non-Isochronous:
 *
 *  [31:9]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the start address of the 512 bytes page. The first descriptor in
 *  the list should be located in this address. The first descriptor may be or may not
 *  be ready. The core starts processing the list from the CTD value.
 *
 *  [8:3]: Current Transfer Desc(CTD)
 *
 *  This value is in terms of number of descriptors. The values can be from 0 to 63.
 *  - 0 -  1 descriptor.
 *  - 63 - 64 descriptors.
 *  This field indicates the current descriptor processed in the list. This field is
 *  updated both by application and the core. For example, if the application enables
 *  the channel after programming CTD=5, then the core will start processing the sixth
 *  descriptor. The address is obtained by adding a value of (8bytes*5=) 40(decimal) to
 *  DMAAddr.
 *
 *  Reset: 6'h0
 *
 *  [2:0]: Reserved
 *
 *  In Scatter-Gather DMA (DescDMA) Mode for Isochronous:
 *
 *  [31:N]: DMA Address (DMAAddr)
 *
 *  The start address must be 512-bytes aligned.
 *
 *  This field holds the address of the 2*(nTD+1) bytes of locations in which the
 *  isochronous descriptors are present where N is based on nTD as follows:
 *  - [31:N]: Base Address
 *  - [N-1:3]: Offset
 *  - [2:0]: 000
 *  For HS ISOC, if nTD is,
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  - 127, N=10
 *  - 255, N=11
 *  For FS ISOC, if nTD is,
 *  - 1, N=4
 *  - 3, N=5
 *  - 7, N=6
 *  - 15, N=7
 *  - 31, N=8
 *  - 63, N=9
 *  [N-1:3]: Current Transfer Desc(CTD)
 *
 *  CTD for isochronous is based on the current frame/(micro)frame value. Need to be
 *  set to zero by application.
 *
 *  Reset: (N+1:3)'h0
 *
 *  [2:0]: Reserved
 */
#define USB_OTGFS_HCDMA7_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_HCDMA7_DMAADDR_M  (USB_OTGFS_HCDMA7_DMAADDR_V << USB_OTGFS_HCDMA7_DMAADDR_S)
#define USB_OTGFS_HCDMA7_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMA7_DMAADDR_S  0

/** USB_OTGFS_HCDMAB7_REG register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
#define USB_OTGFS_HCDMAB7_REG(i) (DR_REG_USB_BASE(i) + 0x5fc)
/** USB_OTGFS_HCDMAB7_HCDMAB : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.
 *  This register is updated as and when the data transfer for the corresponding end
 *  point
 *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
 *  this
 *  field is reserved.
 */
#define USB_OTGFS_HCDMAB7_HCDMAB    0xFFFFFFFFU
#define USB_OTGFS_HCDMAB7_HCDMAB_M  (USB_OTGFS_HCDMAB7_HCDMAB_V << USB_OTGFS_HCDMAB7_HCDMAB_S)
#define USB_OTGFS_HCDMAB7_HCDMAB_V  0xFFFFFFFFU
#define USB_OTGFS_HCDMAB7_HCDMAB_S  0

/** USB_OTGFS_DCFG_REG register
 *  Device Configuration Register  This register configures the core in Device mode
 *  after power-on or after certain control commands or enumeration. Do not make
 *  changes to this register after initial programming.
 */
#define USB_OTGFS_DCFG_REG(i) (DR_REG_USB_BASE(i) + 0x800)
/** USB_OTGFS_DCFG_DEVSPD : R/W; bitpos: [1:0]; default: 0;
 *  Device Speed (DevSpd)
 *
 *  Indicates the speed at which the application requires the core to
 *  enumerate, or the maximum speed the application can support.
 *  However, the actual bus speed is determined only after the connect
 *  sequence is completed, and is based on the speed of the USB
 *  host to which the core is connected.
 */
#define USB_OTGFS_DCFG_DEVSPD    0x00000003U
#define USB_OTGFS_DCFG_DEVSPD_M  (USB_OTGFS_DCFG_DEVSPD_V << USB_OTGFS_DCFG_DEVSPD_S)
#define USB_OTGFS_DCFG_DEVSPD_V  0x00000003U
#define USB_OTGFS_DCFG_DEVSPD_S  0
/** USB_OTGFS_DCFG_NZSTSOUTHSHK : R/W; bitpos: [2]; default: 0;
 *  Non-Zero-Length Status OUT Handshake (NZStsOUTHShk)
 *
 *  The application can use this field to select the handshake the core sends on
 *  receiving a nonzero-length data packet during the OUT transaction of a control
 *  transfer's Status stage.
 *  - 1'b1: Send a STALL handshake on a nonzero-length status OUT transaction and do
 *  not send the received OUT packet to the application.
 *  - 1'b0: Send the received OUT packet to the application (zerolength or
 *  nonzero-length) and send a handshake based on the NAK and STALL bits for the
 *  endpoint in the Device Endpoint Control register.
 */
#define USB_OTGFS_DCFG_NZSTSOUTHSHK    (BIT(2))
#define USB_OTGFS_DCFG_NZSTSOUTHSHK_M  (USB_OTGFS_DCFG_NZSTSOUTHSHK_V << USB_OTGFS_DCFG_NZSTSOUTHSHK_S)
#define USB_OTGFS_DCFG_NZSTSOUTHSHK_V  0x00000001U
#define USB_OTGFS_DCFG_NZSTSOUTHSHK_S  2
/** USB_OTGFS_DCFG_ENA32KHZSUSP : R/W; bitpos: [3]; default: 0;
 *  Enable 32 KHz Suspend mode (Ena32KHzSusp)
 *
 *  This bit can be set only if FS PHY interface is selected. Otherwise, this bit needs
 *  to be set to zero. If FS PHY interface is chosen and this bit is set, the PHY clock
 *  during Suspend must be switched from 48 MHz to 32 KHz.
 */
#define USB_OTGFS_DCFG_ENA32KHZSUSP    (BIT(3))
#define USB_OTGFS_DCFG_ENA32KHZSUSP_M  (USB_OTGFS_DCFG_ENA32KHZSUSP_V << USB_OTGFS_DCFG_ENA32KHZSUSP_S)
#define USB_OTGFS_DCFG_ENA32KHZSUSP_V  0x00000001U
#define USB_OTGFS_DCFG_ENA32KHZSUSP_S  3
/** USB_OTGFS_DCFG_DEVADDR : R/W; bitpos: [10:4]; default: 0;
 *  Device Address (DevAddr)
 *
 *  The application must program this field after every SetAddress control command.
 */
#define USB_OTGFS_DCFG_DEVADDR    0x0000007FU
#define USB_OTGFS_DCFG_DEVADDR_M  (USB_OTGFS_DCFG_DEVADDR_V << USB_OTGFS_DCFG_DEVADDR_S)
#define USB_OTGFS_DCFG_DEVADDR_V  0x0000007FU
#define USB_OTGFS_DCFG_DEVADDR_S  4
/** USB_OTGFS_DCFG_PERFRINT : R/W; bitpos: [12:11]; default: 0;
 *  Periodic Frame Interval (PerFrInt)
 *
 *  Indicates the time within a (micro)Frame at which the application
 *  must be notified using the End Of Periodic Frame Interrupt. This
 *  can be used to determine If all the isochronous traffic for that
 *  (micro)Frame is complete.
 *  - 2'b00: 80% of the (micro)Frame interval
 *  - 2'b01: 85% of the (micro)Frame interval
 *  - 2'b10: 90% of the (micro)Frame interval
 *  - 2'b11: 95% of the (micro)Frame interval
 */
#define USB_OTGFS_DCFG_PERFRINT    0x00000003U
#define USB_OTGFS_DCFG_PERFRINT_M  (USB_OTGFS_DCFG_PERFRINT_V << USB_OTGFS_DCFG_PERFRINT_S)
#define USB_OTGFS_DCFG_PERFRINT_V  0x00000003U
#define USB_OTGFS_DCFG_PERFRINT_S  11
/** USB_OTGFS_DCFG_ENDEVOUTNAK : R/W; bitpos: [13]; default: 0;
 *  Enable Device OUT NAK (EnDevOutNak)
 *
 *  This bit enables setting NAK for Bulk OUT endpoints after the transfer is completed
 *  for Device mode Descriptor DMA
 *  - 1'b0 : The core does not set NAK after Bulk OUT transfer complete
 *  - 1'b1  :  The core sets NAK after Bulk OUT transfer complete
 *  It bit is one time programmable after reset like any other DCFG register bits.
 */
#define USB_OTGFS_DCFG_ENDEVOUTNAK    (BIT(13))
#define USB_OTGFS_DCFG_ENDEVOUTNAK_M  (USB_OTGFS_DCFG_ENDEVOUTNAK_V << USB_OTGFS_DCFG_ENDEVOUTNAK_S)
#define USB_OTGFS_DCFG_ENDEVOUTNAK_V  0x00000001U
#define USB_OTGFS_DCFG_ENDEVOUTNAK_S  13
/** USB_OTGFS_DCFG_XCVRDLY : R/W; bitpos: [14]; default: 0;
 *  XCVRDLY
 *
 *  Enables or disables delay between xcvr_sel and txvalid during device chirp
 */
#define USB_OTGFS_DCFG_XCVRDLY    (BIT(14))
#define USB_OTGFS_DCFG_XCVRDLY_M  (USB_OTGFS_DCFG_XCVRDLY_V << USB_OTGFS_DCFG_XCVRDLY_S)
#define USB_OTGFS_DCFG_XCVRDLY_V  0x00000001U
#define USB_OTGFS_DCFG_XCVRDLY_S  14
/** USB_OTGFS_DCFG_ERRATICINTMSK : R/W; bitpos: [15]; default: 0;
 *  Erratic Error Interrupt Mask
 */
#define USB_OTGFS_DCFG_ERRATICINTMSK    (BIT(15))
#define USB_OTGFS_DCFG_ERRATICINTMSK_M  (USB_OTGFS_DCFG_ERRATICINTMSK_V << USB_OTGFS_DCFG_ERRATICINTMSK_S)
#define USB_OTGFS_DCFG_ERRATICINTMSK_V  0x00000001U
#define USB_OTGFS_DCFG_ERRATICINTMSK_S  15
/** USB_OTGFS_DCFG_DESCDMA : R/W; bitpos: [23]; default: 0;
 *  Enable Scatter/gather DMA in device mode (DescDMA).
 *
 *  When the Scatter/Gather DMA option selected during configuration of the RTL, the
 *  application can Set this bit during initialization to enable the Scatter/Gather DMA
 *  operation.
 *
 *  Note: This bit must be modified only once after a reset. The following combinations
 *  are available for programming:
 *  - GAHBCFG.DMAEn=0,DCFG.DescDMA=0 => Slave mode
 *  - GAHBCFG.DMAEn=0,DCFG.DescDMA=1 => Invalid
 *  - GAHBCFG.DMAEn=1,DCFG.DescDMA=0 => Buffered DMA mode
 *  - GAHBCFG.DMAEn=1,DCFG.DescDMA=1 => Scatter/Gather DMA mode
 */
#define USB_OTGFS_DCFG_DESCDMA    (BIT(23))
#define USB_OTGFS_DCFG_DESCDMA_M  (USB_OTGFS_DCFG_DESCDMA_V << USB_OTGFS_DCFG_DESCDMA_S)
#define USB_OTGFS_DCFG_DESCDMA_V  0x00000001U
#define USB_OTGFS_DCFG_DESCDMA_S  23
/** USB_OTGFS_DCFG_PERSCHINTVL : R/W; bitpos: [25:24]; default: 0;
 *  Periodic Scheduling Interval (PerSchIntvl)
 *
 *  PerSchIntvl must be programmed for Scatter/Gather DMA mode.
 *
 *  This field specifies the amount of time the Internal
 *  DMA engine must allocate for fetching periodic IN endpoint data.
 *  Based on the number of periodic endpoints, this value must be
 *  specified as 25,50 or 75% of (micro)Frame.
 *  - When any periodic endpoints are active, the internal DMA engine allocates the
 *  specified amount of time in fetching periodic IN endpoint data .
 *  - When no periodic endpoints are active, Then the internal DMA engine services
 *  non-periodic endpoints, ignoring this field.
 *  - After the specified time within a (micro)Frame, the DMA switches to fetching for
 *  non-periodic endpoints.
 *  -- 2'b00: 25% of (micro)Frame.
 *  -- 2'b01: 50% of (micro)Frame.
 *  -- 2'b10: 75% of (micro)Frame.
 *  -- 2'b11: Reserved.
 *  Reset: 2'b00
 */
#define USB_OTGFS_DCFG_PERSCHINTVL    0x00000003U
#define USB_OTGFS_DCFG_PERSCHINTVL_M  (USB_OTGFS_DCFG_PERSCHINTVL_V << USB_OTGFS_DCFG_PERSCHINTVL_S)
#define USB_OTGFS_DCFG_PERSCHINTVL_V  0x00000003U
#define USB_OTGFS_DCFG_PERSCHINTVL_S  24
/** USB_OTGFS_DCFG_RESVALID : R/W; bitpos: [31:26]; default: 2;
 *  Resume Validation Period (ResValid)
 *
 *  This field is effective only when DCFG.Ena32KHzSusp is set.
 *  It controls the resume period when the core resumes from
 *  suspend. The core counts for ResValid number of clock cycles
 *  to detect a valid resume when this bit is set
 */
#define USB_OTGFS_DCFG_RESVALID    0x0000003FU
#define USB_OTGFS_DCFG_RESVALID_M  (USB_OTGFS_DCFG_RESVALID_V << USB_OTGFS_DCFG_RESVALID_S)
#define USB_OTGFS_DCFG_RESVALID_V  0x0000003FU
#define USB_OTGFS_DCFG_RESVALID_S  26

/** USB_OTGFS_DCTL_REG register
 *  Device Control Register
 */
#define USB_OTGFS_DCTL_REG(i) (DR_REG_USB_BASE(i) + 0x804)
/** USB_OTGFS_DCTL_RMTWKUPSIG : R/W; bitpos: [0]; default: 0;
 *  Remote Wakeup Signaling (RmtWkUpSig)
 *
 *  When the application sets this bit, the core initiates remote
 *  signaling to wake up the USB host. The application must Set this
 *  bit to instruct the core to exit the Suspend state. As specified in
 *  the USB 2.0 specification, the application must clear this bit
 *  1-15 ms after setting it.
 *
 *
 *  If LPM is enabled and the core is in the L1 (Sleep) state, when the application
 *  sets this bit, the core initiates L1 remote signaling to wake up the USB host. The
 *  application must set this bit to instruct the core to exit the Sleep state. As
 *  specified in the LPM specification, the hardware automatically clears this bit 50
 *  microseconds (TL1DevDrvResume) after being set by the application. The application
 *  must not set this bit when GLPMCFG bRemoteWake from the previous LPM transaction is
 *  zero.
 */
#define USB_OTGFS_DCTL_RMTWKUPSIG    (BIT(0))
#define USB_OTGFS_DCTL_RMTWKUPSIG_M  (USB_OTGFS_DCTL_RMTWKUPSIG_V << USB_OTGFS_DCTL_RMTWKUPSIG_S)
#define USB_OTGFS_DCTL_RMTWKUPSIG_V  0x00000001U
#define USB_OTGFS_DCTL_RMTWKUPSIG_S  0
/** USB_OTGFS_DCTL_SFTDISCON : R/W; bitpos: [1]; default: 1;
 *  Soft Disconnect (SftDiscon)
 *
 *  The application uses this bit to signal the controller to do a soft disconnect. As
 *  long as this bit is Set, the host does not see that the device is connected, and
 *  the device does not receive
 *  signals on the USB. The core stays in the disconnected state until the application
 *  clears this bit.
 *  - 1'b0: Normal operation. When this bit is cleared after a soft disconnect, the
 *  core drives the phy_opmode_o signal on the
 *  UTMI+ to 2'b00, which generates a device connect event to the USB host. When the
 *  device is reconnected, the USB host restarts device enumeration.
 *  - 1'b1: The core drives the phy_opmode_o signal on the UTMI+ to 2'b01, which
 *  generates a device disconnect event to the USB host.
 *  The following is the minimum duration under various conditions for which this bit
 *  must be set for the USB host to detect a device disconnect. To accommodate clock
 *  jitter, it is
 *  recommended that the application adds some extra delay to the specified minimum
 *  duration.
 *
 *  For high speed, if the device state is,
 *  - Suspended, the minimum duration is 1ms + 2.5us
 *  - Idle, the minimum duration is 3ms + 2.5us
 *  - Not Idle or Suspended (performing transactions), the minimum duration 125 us
 *  For full speed/low speed, if the device state is,
 *  - Suspended, the minimum duration is 1ms + 2.5us
 *  - Idle, the minimum duration is 2.5us
 *  - Not Idle or Suspended (performing transactions), the minimum duration 125 us
 *  Note:
 *  - This bit can be also used for ULPI/FS Serial interfaces.
 *  - This bit is not impacted by a soft reset.
 */
#define USB_OTGFS_DCTL_SFTDISCON    (BIT(1))
#define USB_OTGFS_DCTL_SFTDISCON_M  (USB_OTGFS_DCTL_SFTDISCON_V << USB_OTGFS_DCTL_SFTDISCON_S)
#define USB_OTGFS_DCTL_SFTDISCON_V  0x00000001U
#define USB_OTGFS_DCTL_SFTDISCON_S  1
/** USB_OTGFS_DCTL_GNPINNAKSTS : RO; bitpos: [2]; default: 0;
 *  Global Non-periodic IN NAK Status (GNPINNakSts)
 *  - 1'b0: A handshake is sent out based on the data availability in the transmit FIFO.
 *  - 1'b1: A NAK handshake is sent out on all non-periodic IN endpoints, irrespective
 *  of the data availability in the transmit FIFO.
 */
#define USB_OTGFS_DCTL_GNPINNAKSTS    (BIT(2))
#define USB_OTGFS_DCTL_GNPINNAKSTS_M  (USB_OTGFS_DCTL_GNPINNAKSTS_V << USB_OTGFS_DCTL_GNPINNAKSTS_S)
#define USB_OTGFS_DCTL_GNPINNAKSTS_V  0x00000001U
#define USB_OTGFS_DCTL_GNPINNAKSTS_S  2
/** USB_OTGFS_DCTL_GOUTNAKSTS : RO; bitpos: [3]; default: 0;
 *  Global OUT NAK Status (GOUTNakSts)
 *  - 1'b0: A handshake is sent based on the FIFO Status and the NAK and STALL bit
 *  settings.
 *  - 1'b1: No data is written to the RxFIFO, irrespective of space availability. Sends
 *  a NAK handshake on all packets, except on SETUP transactions. All isochronous OUT
 *  packets are dropped.
 */
#define USB_OTGFS_DCTL_GOUTNAKSTS    (BIT(3))
#define USB_OTGFS_DCTL_GOUTNAKSTS_M  (USB_OTGFS_DCTL_GOUTNAKSTS_V << USB_OTGFS_DCTL_GOUTNAKSTS_S)
#define USB_OTGFS_DCTL_GOUTNAKSTS_V  0x00000001U
#define USB_OTGFS_DCTL_GOUTNAKSTS_S  3
/** USB_OTGFS_DCTL_TSTCTL : R/W; bitpos: [6:4]; default: 0;
 *  Test Control (TstCtl)
 *  - 3'b000: Test mode disabled
 *  - 3'b001: Test_J mode
 *  - 3'b010: Test_K mode
 *  - 3'b011: Test_SE0_NAK mode
 *  - 3'b100: Test_Packet mode
 *  - 3'b101: Test_Force_Enable
 *  - Others: Reserved
 */
#define USB_OTGFS_DCTL_TSTCTL    0x00000007U
#define USB_OTGFS_DCTL_TSTCTL_M  (USB_OTGFS_DCTL_TSTCTL_V << USB_OTGFS_DCTL_TSTCTL_S)
#define USB_OTGFS_DCTL_TSTCTL_V  0x00000007U
#define USB_OTGFS_DCTL_TSTCTL_S  4
/** USB_OTGFS_DCTL_SGNPINNAK : R/W; bitpos: [7]; default: 0;
 *  Set Global Non-periodic IN NAK (SGNPInNak)
 *
 *  A write to this field sets the Global Non-periodic IN NAK.The application uses this
 *  bit to send a NAK handshake on all non-periodic IN endpoints.
 *  The core can also Set this bit when a timeout condition is detected on a
 *  non-periodic endpoint in shared FIFO operation.
 *  The application must Set this bit only after making sure that the Global IN NAK
 *  Effective bit in the Core Interrupt Register (GINTSTS.GINNakEff) is cleared
 */
#define USB_OTGFS_DCTL_SGNPINNAK    (BIT(7))
#define USB_OTGFS_DCTL_SGNPINNAK_M  (USB_OTGFS_DCTL_SGNPINNAK_V << USB_OTGFS_DCTL_SGNPINNAK_S)
#define USB_OTGFS_DCTL_SGNPINNAK_V  0x00000001U
#define USB_OTGFS_DCTL_SGNPINNAK_S  7
/** USB_OTGFS_DCTL_CGNPINNAK : R/W; bitpos: [8]; default: 0;
 *  Clear Global Non-periodic IN NAK (CGNPInNak)
 *
 *  A write to this field clears the Global Non-periodic IN NAK.
 */
#define USB_OTGFS_DCTL_CGNPINNAK    (BIT(8))
#define USB_OTGFS_DCTL_CGNPINNAK_M  (USB_OTGFS_DCTL_CGNPINNAK_V << USB_OTGFS_DCTL_CGNPINNAK_S)
#define USB_OTGFS_DCTL_CGNPINNAK_V  0x00000001U
#define USB_OTGFS_DCTL_CGNPINNAK_S  8
/** USB_OTGFS_DCTL_SGOUTNAK : R/W; bitpos: [9]; default: 0;
 *  Set Global OUT NAK (SGOUTNak)
 *
 *  A write to this field sets the Global OUT NAK. The application uses this bit to
 *  send a NAK handshake on all OUT endpoints.
 *  The application must set the this bit only after making sure that the Global OUT
 *  NAK Effective bit in the Core Interrupt Register (GINTSTS.GOUTNakEff) is cleared.
 */
#define USB_OTGFS_DCTL_SGOUTNAK    (BIT(9))
#define USB_OTGFS_DCTL_SGOUTNAK_M  (USB_OTGFS_DCTL_SGOUTNAK_V << USB_OTGFS_DCTL_SGOUTNAK_S)
#define USB_OTGFS_DCTL_SGOUTNAK_V  0x00000001U
#define USB_OTGFS_DCTL_SGOUTNAK_S  9
/** USB_OTGFS_DCTL_CGOUTNAK : R/W; bitpos: [10]; default: 0;
 *  Clear Global OUT NAK (CGOUTNak)
 *
 *  A write to this field clears the Global OUT NAK.
 */
#define USB_OTGFS_DCTL_CGOUTNAK    (BIT(10))
#define USB_OTGFS_DCTL_CGOUTNAK_M  (USB_OTGFS_DCTL_CGOUTNAK_V << USB_OTGFS_DCTL_CGOUTNAK_S)
#define USB_OTGFS_DCTL_CGOUTNAK_V  0x00000001U
#define USB_OTGFS_DCTL_CGOUTNAK_S  10
/** USB_OTGFS_DCTL_PWRONPRGDONE : R/W; bitpos: [11]; default: 0;
 *  Power-On Programming Done (PWROnPrgDone)
 *
 *  The application uses this bit to indicate that register programming is completed
 *  after a wake-up from Power Down mode.
 */
#define USB_OTGFS_DCTL_PWRONPRGDONE    (BIT(11))
#define USB_OTGFS_DCTL_PWRONPRGDONE_M  (USB_OTGFS_DCTL_PWRONPRGDONE_V << USB_OTGFS_DCTL_PWRONPRGDONE_S)
#define USB_OTGFS_DCTL_PWRONPRGDONE_V  0x00000001U
#define USB_OTGFS_DCTL_PWRONPRGDONE_S  11
/** USB_OTGFS_DCTL_GMC : R/W; bitpos: [14:13]; default: 0;
 *  Global Multi Count (GMC)
 *
 *  GMC must be programmed only once after initialization.
 *  Applicable only for Scatter/Gather DMA mode. This indicates the number of packets
 *  to be serviced for that end point before moving to the next end point. It is only
 *  for non-periodic endpoints.
 *  - 2'b00: Invalid.
 *  - 2'b01: 1 packet.
 *  - 2'b10: 2 packets.
 *  - 2'b11: 3 packets.
 *  The value of this field automatically changes to 2'h1 when DCFG.DescDMA is set to
 *  1. When Scatter/Gather DMA mode is disabled, this field is reserved. and reads
 *  2'b00.
 */
#define USB_OTGFS_DCTL_GMC    0x00000003U
#define USB_OTGFS_DCTL_GMC_M  (USB_OTGFS_DCTL_GMC_V << USB_OTGFS_DCTL_GMC_S)
#define USB_OTGFS_DCTL_GMC_V  0x00000003U
#define USB_OTGFS_DCTL_GMC_S  13
/** USB_OTGFS_DCTL_IGNRFRMNUM : R/W; bitpos: [15]; default: 0;
 *  Ignore Frame Number Feature for Isochronous Endpoints (IgnrFrmNum)
 *
 *  This field is also used to control the Periodic Transfer Interrupt (PTI) feature.
 *
 *  Note: Do not program IgnrFrmNum bit to 1'b1 when the core is operating in threshold
 *  mode.
 *
 *  Slave Mode (GAHBCFG.DMAEn=0):
 *
 *  This bit is not valid in Slave mode and should not be programmed to 1.
 *
 *  Scatter/Gather DMA Mode (GAHBCFG.DMAEn=1,DCFG.DescDMA=1):
 *
 *  Note: When Scatter/Gather DMA mode is enabled this feature is not applicable to
 *  High Speed, High bandwidth transfers.
 *
 *  When this bit is enabled, there must be only one packet per descriptor.
 *  - 0: The core transmits the packets only in the frame number in which they are
 *  intended to be transmitted.
 *  - 1: The core ignores the frame number, sending packets immediately as the packets
 *  are ready.
 *  In Scatter/Gather DMA mode, if this bit is enabled, the packets are not flushed
 *  when a ISOC IN token is received for an elapsed frame.
 *
 *  Non-Scatter/Gather DMA Mode, that is, Buffer DMA Mode
 *  (GAHBCFG.DMAEn=1,DCFG.DescDMA=0):
 *
 *  When Scatter/Gather DMA mode is disabled, this field is used by the application to
 *  enable Periodic Transfer Interrupt (PTI) Mode.
 *
 *  The application can program Periodic Endpoint transfers for multiple (micro)Frames.
 *  - 0: Periodic Transfer Interrupt feature is disabled, application needs to program
 *  transfers for periodic endpoints every (micro)Frame
 *  - 1: Periodic Transfer Interrupt feature is enabled, application can program
 *  transfers for multiple (micro)Frames for periodic endpoints.
 *  In the PTI mode, the application will receive Transfer Complete Interrupt after
 *  transfers for multiple (micro)Frames are completed.
 */
#define USB_OTGFS_DCTL_IGNRFRMNUM    (BIT(15))
#define USB_OTGFS_DCTL_IGNRFRMNUM_M  (USB_OTGFS_DCTL_IGNRFRMNUM_V << USB_OTGFS_DCTL_IGNRFRMNUM_S)
#define USB_OTGFS_DCTL_IGNRFRMNUM_V  0x00000001U
#define USB_OTGFS_DCTL_IGNRFRMNUM_S  15
/** USB_OTGFS_DCTL_NAKONBBLE : R/W; bitpos: [16]; default: 0;
 *  NAK on Babble Error (NakOnBble)
 *
 *  Set NAK automatically on babble (NakOnBble). The core sets NAK automatically for
 *  the endpoint on which babble is received.
 */
#define USB_OTGFS_DCTL_NAKONBBLE    (BIT(16))
#define USB_OTGFS_DCTL_NAKONBBLE_M  (USB_OTGFS_DCTL_NAKONBBLE_V << USB_OTGFS_DCTL_NAKONBBLE_S)
#define USB_OTGFS_DCTL_NAKONBBLE_V  0x00000001U
#define USB_OTGFS_DCTL_NAKONBBLE_S  16
/** USB_OTGFS_DCTL_ENCONTONBNA : R/W; bitpos: [17]; default: 0;
 *  Enable Continue on BNA (EnContOnBNA)
 *
 *  This bit enables the core to continue on BNA for Bulk OUT endpoints.
 *  With this feature enabled, when a Bulk OUT or INTR OUT endpoint receives a BNA
 *  interrupt
 *  the core starts processing the descriptor that caused the BNA interrupt after
 *  the endpoint re-enables the endpoint.
 *  - 1'b0: After receiving BNA interrupt,the core disables the endpoint. When the
 *  endpoint is re-enabled by the application,the core starts processing from the
 *  DOEPDMA descriptor.
 *  - 1'b1: After receiving BNA interrupt, the core disables the endpoint. When the
 *  endpoint is re-enabled by the application, the core starts processing from the
 *  descriptor that received the BNA interrupt.
 *
 *  This bit is valid only when OTG_EN_DESC_DMA == 1'b1. It is a one-time programmable
 *  after reset bit like any other DCTL register bits.
 */
#define USB_OTGFS_DCTL_ENCONTONBNA    (BIT(17))
#define USB_OTGFS_DCTL_ENCONTONBNA_M  (USB_OTGFS_DCTL_ENCONTONBNA_V << USB_OTGFS_DCTL_ENCONTONBNA_S)
#define USB_OTGFS_DCTL_ENCONTONBNA_V  0x00000001U
#define USB_OTGFS_DCTL_ENCONTONBNA_S  17

/** USB_OTGFS_DSTS_REG register
 *  Device Status Register  This register indicates the status of the core with respect
 *  to USB-related events. It must be read on interrupts from Device All Interrupts
 *  (DAINT) register.
 */
#define USB_OTGFS_DSTS_REG(i) (DR_REG_USB_BASE(i) + 0x808)
/** USB_OTGFS_DSTS_SUSPSTS : RO; bitpos: [0]; default: 0;
 *  Suspend Status (SuspSts)
 *
 *  In Device mode, this bit is set as long as a Suspend condition is
 *  detected on the USB. The core enters the Suspend state
 *  when there is no activity on the phy_line_state_i signal for an
 *  extended period of time. The core comes out of the suspend under the following
 *  conditions :
 *  - If there is any activity on the phy_line_state_i signal, or
 *  - If the application writes to the Remote Wakeup Signaling bit in the Device
 *  Control register (DCTL.RmtWkUpSig).
 *  When the core comes out of the suspend, this bit is set to 1'b0.
 */
#define USB_OTGFS_DSTS_SUSPSTS    (BIT(0))
#define USB_OTGFS_DSTS_SUSPSTS_M  (USB_OTGFS_DSTS_SUSPSTS_V << USB_OTGFS_DSTS_SUSPSTS_S)
#define USB_OTGFS_DSTS_SUSPSTS_V  0x00000001U
#define USB_OTGFS_DSTS_SUSPSTS_S  0
/** USB_OTGFS_DSTS_ENUMSPD : RO; bitpos: [2:1]; default: 1;
 *  Enumerated Speed (EnumSpd)
 *
 *  Indicates the speed at which the controller has come up
 *  after speed detection through a connect or reset sequence.
 *  - 2'b00: High speed (PHY clock is running at 30 or 60 MHz)
 *  - 2'b01: Full speed (PHY clock is running at 30 or 60 MHz)
 *  - 2'b10: Low speed (PHY clock is running at 6 MHz)
 *  - 2'b11: Full speed (PHY clock is running at 48 MHz)
 *  Low speed is not supported for devices using a UTMI+ PHY.
 */
#define USB_OTGFS_DSTS_ENUMSPD    0x00000003U
#define USB_OTGFS_DSTS_ENUMSPD_M  (USB_OTGFS_DSTS_ENUMSPD_V << USB_OTGFS_DSTS_ENUMSPD_S)
#define USB_OTGFS_DSTS_ENUMSPD_V  0x00000003U
#define USB_OTGFS_DSTS_ENUMSPD_S  1
/** USB_OTGFS_DSTS_ERRTICERR : RO; bitpos: [3]; default: 0;
 *  Erratic Error (ErrticErr)
 *
 *  The core sets this bit to report any erratic errors
 *  (phy_rxvalid_i/phy_rxvldh_i or phy_rxactive_i is asserted for at
 *  least 2 ms, due to PHY error) seen on the UTMI+.
 *  Due to erratic errors, the DWC_otg core goes into Suspended
 *  state and an interrupt is generated to the application with Early
 *  Suspend bit of the Core Interrupt register (GINTSTS.ErlySusp).
 *  If the early suspend is asserted due to an erratic error, the
 *  application can only perform a soft disconnect recover.
 */
#define USB_OTGFS_DSTS_ERRTICERR    (BIT(3))
#define USB_OTGFS_DSTS_ERRTICERR_M  (USB_OTGFS_DSTS_ERRTICERR_V << USB_OTGFS_DSTS_ERRTICERR_S)
#define USB_OTGFS_DSTS_ERRTICERR_V  0x00000001U
#define USB_OTGFS_DSTS_ERRTICERR_S  3
/** USB_OTGFS_DSTS_SOFFN : RO; bitpos: [21:8]; default: 0;
 *  Frame or Microframe Number of the Received SOF  (SOFFN)
 *
 *  When the core is operating at high speed, this field contains a microframe number.
 *  When the core is operating at full or low speed, this field contains a Frame number.
 *
 *  Note: This register may return a non-zero value if read immediately after power-on
 *  reset.
 *  In case the register bit reads non-zero immediately after power-on reset, it does
 *  not
 *  indicate that SOF has been received from the host. The read value of this interrupt
 *  is
 *  valid only after a valid connection between host and device is established.
 */
#define USB_OTGFS_DSTS_SOFFN    0x00003FFFU
#define USB_OTGFS_DSTS_SOFFN_M  (USB_OTGFS_DSTS_SOFFN_V << USB_OTGFS_DSTS_SOFFN_S)
#define USB_OTGFS_DSTS_SOFFN_V  0x00003FFFU
#define USB_OTGFS_DSTS_SOFFN_S  8
/** USB_OTGFS_DSTS_DEVLNSTS : RO; bitpos: [23:22]; default: 0;
 *  Device Line Status (DevLnSts)
 *
 *  Indicates the current logic level USB data lines
 *  - DevLnSts[1]: Logic level of D+
 *  - DevLnSts[0]: Logic level of D-
 */
#define USB_OTGFS_DSTS_DEVLNSTS    0x00000003U
#define USB_OTGFS_DSTS_DEVLNSTS_M  (USB_OTGFS_DSTS_DEVLNSTS_V << USB_OTGFS_DSTS_DEVLNSTS_S)
#define USB_OTGFS_DSTS_DEVLNSTS_V  0x00000003U
#define USB_OTGFS_DSTS_DEVLNSTS_S  22

/** USB_OTGFS_DIEPMSK_REG register
 *  Device IN Endpoint Common Interrupt Mask Register  This register works with each of
 *  the Device IN Endpoint Interrupt (DIEPINTn) registers for all endpoints to generate
 *  an interrupt per IN endpoint. The IN endpoint interrupt for a specific status in
 *  the DIEPINTn register can be masked by writing to the corresponding bit in this
 *  register. Status bits are masked by default.
 */
#define USB_OTGFS_DIEPMSK_REG(i) (DR_REG_USB_BASE(i) + 0x810)
/** USB_OTGFS_DIEPMSK_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt Mask (XferComplMsk)
 */
#define USB_OTGFS_DIEPMSK_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_DIEPMSK_XFERCOMPLMSK_M  (USB_OTGFS_DIEPMSK_XFERCOMPLMSK_V << USB_OTGFS_DIEPMSK_XFERCOMPLMSK_S)
#define USB_OTGFS_DIEPMSK_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_XFERCOMPLMSK_S  0
/** USB_OTGFS_DIEPMSK_EPDISBLDMSK : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt Mask (EPDisbldMsk)
 */
#define USB_OTGFS_DIEPMSK_EPDISBLDMSK    (BIT(1))
#define USB_OTGFS_DIEPMSK_EPDISBLDMSK_M  (USB_OTGFS_DIEPMSK_EPDISBLDMSK_V << USB_OTGFS_DIEPMSK_EPDISBLDMSK_S)
#define USB_OTGFS_DIEPMSK_EPDISBLDMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_EPDISBLDMSK_S  1
/** USB_OTGFS_DIEPMSK_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *  AHB Error Mask (AHBErrMsk)
 */
#define USB_OTGFS_DIEPMSK_AHBERRMSK    (BIT(2))
#define USB_OTGFS_DIEPMSK_AHBERRMSK_M  (USB_OTGFS_DIEPMSK_AHBERRMSK_V << USB_OTGFS_DIEPMSK_AHBERRMSK_S)
#define USB_OTGFS_DIEPMSK_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_AHBERRMSK_S  2
/** USB_OTGFS_DIEPMSK_TIMEOUTMSK : R/W; bitpos: [3]; default: 0;
 *  Timeout Condition Mask (TimeOUTMsk) (Non-isochronous endpoints)
 */
#define USB_OTGFS_DIEPMSK_TIMEOUTMSK    (BIT(3))
#define USB_OTGFS_DIEPMSK_TIMEOUTMSK_M  (USB_OTGFS_DIEPMSK_TIMEOUTMSK_V << USB_OTGFS_DIEPMSK_TIMEOUTMSK_S)
#define USB_OTGFS_DIEPMSK_TIMEOUTMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_TIMEOUTMSK_S  3
/** USB_OTGFS_DIEPMSK_INTKNTXFEMPMSK : R/W; bitpos: [4]; default: 0;
 *  IN Token Received When TxFIFO Empty Mask (INTknTXFEmpMsk)
 */
#define USB_OTGFS_DIEPMSK_INTKNTXFEMPMSK    (BIT(4))
#define USB_OTGFS_DIEPMSK_INTKNTXFEMPMSK_M  (USB_OTGFS_DIEPMSK_INTKNTXFEMPMSK_V << USB_OTGFS_DIEPMSK_INTKNTXFEMPMSK_S)
#define USB_OTGFS_DIEPMSK_INTKNTXFEMPMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_INTKNTXFEMPMSK_S  4
/** USB_OTGFS_DIEPMSK_INTKNEPMISMSK : R/W; bitpos: [5]; default: 0;
 *  IN Token received with EP Mismatch Mask (INTknEPMisMsk)
 */
#define USB_OTGFS_DIEPMSK_INTKNEPMISMSK    (BIT(5))
#define USB_OTGFS_DIEPMSK_INTKNEPMISMSK_M  (USB_OTGFS_DIEPMSK_INTKNEPMISMSK_V << USB_OTGFS_DIEPMSK_INTKNEPMISMSK_S)
#define USB_OTGFS_DIEPMSK_INTKNEPMISMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_INTKNEPMISMSK_S  5
/** USB_OTGFS_DIEPMSK_INEPNAKEFFMSK : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective Mask (INEPNakEffMsk)
 */
#define USB_OTGFS_DIEPMSK_INEPNAKEFFMSK    (BIT(6))
#define USB_OTGFS_DIEPMSK_INEPNAKEFFMSK_M  (USB_OTGFS_DIEPMSK_INEPNAKEFFMSK_V << USB_OTGFS_DIEPMSK_INEPNAKEFFMSK_S)
#define USB_OTGFS_DIEPMSK_INEPNAKEFFMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_INEPNAKEFFMSK_S  6
/** USB_OTGFS_DIEPMSK_TXFIFOUNDRNMSK : R/W; bitpos: [8]; default: 0;
 *  Fifo Underrun Mask (TxfifoUndrnMsk)
 */
#define USB_OTGFS_DIEPMSK_TXFIFOUNDRNMSK    (BIT(8))
#define USB_OTGFS_DIEPMSK_TXFIFOUNDRNMSK_M  (USB_OTGFS_DIEPMSK_TXFIFOUNDRNMSK_V << USB_OTGFS_DIEPMSK_TXFIFOUNDRNMSK_S)
#define USB_OTGFS_DIEPMSK_TXFIFOUNDRNMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_TXFIFOUNDRNMSK_S  8
/** USB_OTGFS_DIEPMSK_BNAININTRMSK : R/W; bitpos: [9]; default: 0;
 *  BNA interrupt Mask (BNAInIntrMsk)
 */
#define USB_OTGFS_DIEPMSK_BNAININTRMSK    (BIT(9))
#define USB_OTGFS_DIEPMSK_BNAININTRMSK_M  (USB_OTGFS_DIEPMSK_BNAININTRMSK_V << USB_OTGFS_DIEPMSK_BNAININTRMSK_S)
#define USB_OTGFS_DIEPMSK_BNAININTRMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_BNAININTRMSK_S  9
/** USB_OTGFS_DIEPMSK_NAKMSK : R/W; bitpos: [13]; default: 0;
 *  NAK interrupt Mask (NAKMsk)
 */
#define USB_OTGFS_DIEPMSK_NAKMSK    (BIT(13))
#define USB_OTGFS_DIEPMSK_NAKMSK_M  (USB_OTGFS_DIEPMSK_NAKMSK_V << USB_OTGFS_DIEPMSK_NAKMSK_S)
#define USB_OTGFS_DIEPMSK_NAKMSK_V  0x00000001U
#define USB_OTGFS_DIEPMSK_NAKMSK_S  13

/** USB_OTGFS_DOEPMSK_REG register
 *  Device OUT Endpoint Common Interrupt Mask Register  This register works with each
 *  of the Device OUT Endpoint Interrupt (DOEPINTn) registers for all endpoints to
 *  generate an interrupt per OUT endpoint. The OUT endpoint interrupt for a specific
 *  status in the DOEPINTn register can be masked by writing into the corresponding bit
 *  in this register. Status bits are masked by default.
 */
#define USB_OTGFS_DOEPMSK_REG(i) (DR_REG_USB_BASE(i) + 0x814)
/** USB_OTGFS_DOEPMSK_XFERCOMPLMSK : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt Mask (XferComplMsk)
 */
#define USB_OTGFS_DOEPMSK_XFERCOMPLMSK    (BIT(0))
#define USB_OTGFS_DOEPMSK_XFERCOMPLMSK_M  (USB_OTGFS_DOEPMSK_XFERCOMPLMSK_V << USB_OTGFS_DOEPMSK_XFERCOMPLMSK_S)
#define USB_OTGFS_DOEPMSK_XFERCOMPLMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_XFERCOMPLMSK_S  0
/** USB_OTGFS_DOEPMSK_EPDISBLDMSK : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt Mask (EPDisbldMsk)
 */
#define USB_OTGFS_DOEPMSK_EPDISBLDMSK    (BIT(1))
#define USB_OTGFS_DOEPMSK_EPDISBLDMSK_M  (USB_OTGFS_DOEPMSK_EPDISBLDMSK_V << USB_OTGFS_DOEPMSK_EPDISBLDMSK_S)
#define USB_OTGFS_DOEPMSK_EPDISBLDMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_EPDISBLDMSK_S  1
/** USB_OTGFS_DOEPMSK_AHBERRMSK : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErrMsk)
 */
#define USB_OTGFS_DOEPMSK_AHBERRMSK    (BIT(2))
#define USB_OTGFS_DOEPMSK_AHBERRMSK_M  (USB_OTGFS_DOEPMSK_AHBERRMSK_V << USB_OTGFS_DOEPMSK_AHBERRMSK_S)
#define USB_OTGFS_DOEPMSK_AHBERRMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_AHBERRMSK_S  2
/** USB_OTGFS_DOEPMSK_SETUPMSK : R/W; bitpos: [3]; default: 0;
 *  SETUP Phase Done Mask (SetUPMsk)
 *
 *  Applies to control endpoints only.
 */
#define USB_OTGFS_DOEPMSK_SETUPMSK    (BIT(3))
#define USB_OTGFS_DOEPMSK_SETUPMSK_M  (USB_OTGFS_DOEPMSK_SETUPMSK_V << USB_OTGFS_DOEPMSK_SETUPMSK_S)
#define USB_OTGFS_DOEPMSK_SETUPMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_SETUPMSK_S  3
/** USB_OTGFS_DOEPMSK_OUTTKNEPDISMSK : R/W; bitpos: [4]; default: 0;
 *  OUT Token Received when Endpoint Disabled Mask (OUTTknEPdisMsk)
 *
 *  Applies to control OUT endpoints only.
 */
#define USB_OTGFS_DOEPMSK_OUTTKNEPDISMSK    (BIT(4))
#define USB_OTGFS_DOEPMSK_OUTTKNEPDISMSK_M  (USB_OTGFS_DOEPMSK_OUTTKNEPDISMSK_V << USB_OTGFS_DOEPMSK_OUTTKNEPDISMSK_S)
#define USB_OTGFS_DOEPMSK_OUTTKNEPDISMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_OUTTKNEPDISMSK_S  4
/** USB_OTGFS_DOEPMSK_STSPHSERCVDMSK : R/W; bitpos: [5]; default: 0;
 *  Status Phase Received Mask (StsPhseRcvdMsk)
 *
 *  Applies to control OUT endpoints only.
 */
#define USB_OTGFS_DOEPMSK_STSPHSERCVDMSK    (BIT(5))
#define USB_OTGFS_DOEPMSK_STSPHSERCVDMSK_M  (USB_OTGFS_DOEPMSK_STSPHSERCVDMSK_V << USB_OTGFS_DOEPMSK_STSPHSERCVDMSK_S)
#define USB_OTGFS_DOEPMSK_STSPHSERCVDMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_STSPHSERCVDMSK_S  5
/** USB_OTGFS_DOEPMSK_BACK2BACKSETUP : R/W; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received Mask (Back2BackSETup)
 *
 *  Applies to control OUT endpoints only.
 */
#define USB_OTGFS_DOEPMSK_BACK2BACKSETUP    (BIT(6))
#define USB_OTGFS_DOEPMSK_BACK2BACKSETUP_M  (USB_OTGFS_DOEPMSK_BACK2BACKSETUP_V << USB_OTGFS_DOEPMSK_BACK2BACKSETUP_S)
#define USB_OTGFS_DOEPMSK_BACK2BACKSETUP_V  0x00000001U
#define USB_OTGFS_DOEPMSK_BACK2BACKSETUP_S  6
/** USB_OTGFS_DOEPMSK_OUTPKTERRMSK : R/W; bitpos: [8]; default: 0;
 *  OUT Packet Error Mask (OutPktErrMsk)
 */
#define USB_OTGFS_DOEPMSK_OUTPKTERRMSK    (BIT(8))
#define USB_OTGFS_DOEPMSK_OUTPKTERRMSK_M  (USB_OTGFS_DOEPMSK_OUTPKTERRMSK_V << USB_OTGFS_DOEPMSK_OUTPKTERRMSK_S)
#define USB_OTGFS_DOEPMSK_OUTPKTERRMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_OUTPKTERRMSK_S  8
/** USB_OTGFS_DOEPMSK_BNAOUTINTRMSK : R/W; bitpos: [9]; default: 0;
 *  BNA interrupt Mask (BnaOutIntrMsk)
 */
#define USB_OTGFS_DOEPMSK_BNAOUTINTRMSK    (BIT(9))
#define USB_OTGFS_DOEPMSK_BNAOUTINTRMSK_M  (USB_OTGFS_DOEPMSK_BNAOUTINTRMSK_V << USB_OTGFS_DOEPMSK_BNAOUTINTRMSK_S)
#define USB_OTGFS_DOEPMSK_BNAOUTINTRMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_BNAOUTINTRMSK_S  9
/** USB_OTGFS_DOEPMSK_BBLEERRMSK : R/W; bitpos: [12]; default: 0;
 *  Babble Error interrupt Mask (BbleErrMsk)
 */
#define USB_OTGFS_DOEPMSK_BBLEERRMSK    (BIT(12))
#define USB_OTGFS_DOEPMSK_BBLEERRMSK_M  (USB_OTGFS_DOEPMSK_BBLEERRMSK_V << USB_OTGFS_DOEPMSK_BBLEERRMSK_S)
#define USB_OTGFS_DOEPMSK_BBLEERRMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_BBLEERRMSK_S  12
/** USB_OTGFS_DOEPMSK_NAKMSK : R/W; bitpos: [13]; default: 0;
 *  NAK interrupt Mask (NAKMsk)
 */
#define USB_OTGFS_DOEPMSK_NAKMSK    (BIT(13))
#define USB_OTGFS_DOEPMSK_NAKMSK_M  (USB_OTGFS_DOEPMSK_NAKMSK_V << USB_OTGFS_DOEPMSK_NAKMSK_S)
#define USB_OTGFS_DOEPMSK_NAKMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_NAKMSK_S  13
/** USB_OTGFS_DOEPMSK_NYETMSK : R/W; bitpos: [14]; default: 0;
 *  NYET interrupt Mask (NYETMsk)
 */
#define USB_OTGFS_DOEPMSK_NYETMSK    (BIT(14))
#define USB_OTGFS_DOEPMSK_NYETMSK_M  (USB_OTGFS_DOEPMSK_NYETMSK_V << USB_OTGFS_DOEPMSK_NYETMSK_S)
#define USB_OTGFS_DOEPMSK_NYETMSK_V  0x00000001U
#define USB_OTGFS_DOEPMSK_NYETMSK_S  14

/** USB_OTGFS_DAINT_REG register
 *  Device All Endpoints Interrupt Register  When a significant event occurs on an
 *  endpoint, a Device All Endpoints Interrupt register interrupts the application
 *  using the Device OUT Endpoints Interrupt bit or Device IN Endpoints Interrupt bit
 *  of the Core Interrupt register (GINTSTS.OEPInt or GINTSTS.IEPInt, respectively).
 *  This is shown in Figure 5-2. There is one interrupt bit per endpoint, up to a
 *  maximum of 16 bits for OUT endpoints and 16 bits for IN endpoints. For a
 *  bidirectional endpoint, the corresponding IN and OUT interrupt bits are used. Bits
 *  in this register are set and cleared when the application sets and clears bits in
 *  the corresponding Device Endpoint-n Interrupt register (DIEPINTn/DOEPINTn).
 */
#define USB_OTGFS_DAINT_REG(i) (DR_REG_USB_BASE(i) + 0x818)
/** USB_OTGFS_DAINT_INEPINT0 : RO; bitpos: [0]; default: 0;
 *  IN Endpoint 0 Interrupt Bit
 */
#define USB_OTGFS_DAINT_INEPINT0    (BIT(0))
#define USB_OTGFS_DAINT_INEPINT0_M  (USB_OTGFS_DAINT_INEPINT0_V << USB_OTGFS_DAINT_INEPINT0_S)
#define USB_OTGFS_DAINT_INEPINT0_V  0x00000001U
#define USB_OTGFS_DAINT_INEPINT0_S  0
/** USB_OTGFS_DAINT_INEPINT1 : RO; bitpos: [1]; default: 0;
 *  IN Endpoint 1 Interrupt Bit
 */
#define USB_OTGFS_DAINT_INEPINT1    (BIT(1))
#define USB_OTGFS_DAINT_INEPINT1_M  (USB_OTGFS_DAINT_INEPINT1_V << USB_OTGFS_DAINT_INEPINT1_S)
#define USB_OTGFS_DAINT_INEPINT1_V  0x00000001U
#define USB_OTGFS_DAINT_INEPINT1_S  1
/** USB_OTGFS_DAINT_INEPINT2 : RO; bitpos: [2]; default: 0;
 *  IN Endpoint 2 Interrupt Bit
 */
#define USB_OTGFS_DAINT_INEPINT2    (BIT(2))
#define USB_OTGFS_DAINT_INEPINT2_M  (USB_OTGFS_DAINT_INEPINT2_V << USB_OTGFS_DAINT_INEPINT2_S)
#define USB_OTGFS_DAINT_INEPINT2_V  0x00000001U
#define USB_OTGFS_DAINT_INEPINT2_S  2
/** USB_OTGFS_DAINT_INEPINT3 : RO; bitpos: [3]; default: 0;
 *  IN Endpoint 3 Interrupt Bit
 */
#define USB_OTGFS_DAINT_INEPINT3    (BIT(3))
#define USB_OTGFS_DAINT_INEPINT3_M  (USB_OTGFS_DAINT_INEPINT3_V << USB_OTGFS_DAINT_INEPINT3_S)
#define USB_OTGFS_DAINT_INEPINT3_V  0x00000001U
#define USB_OTGFS_DAINT_INEPINT3_S  3
/** USB_OTGFS_DAINT_INEPINT4 : RO; bitpos: [4]; default: 0;
 *  IN Endpoint 4 Interrupt Bit
 */
#define USB_OTGFS_DAINT_INEPINT4    (BIT(4))
#define USB_OTGFS_DAINT_INEPINT4_M  (USB_OTGFS_DAINT_INEPINT4_V << USB_OTGFS_DAINT_INEPINT4_S)
#define USB_OTGFS_DAINT_INEPINT4_V  0x00000001U
#define USB_OTGFS_DAINT_INEPINT4_S  4
/** USB_OTGFS_DAINT_INEPINT5 : RO; bitpos: [5]; default: 0;
 *  IN Endpoint 5 Interrupt Bit
 */
#define USB_OTGFS_DAINT_INEPINT5    (BIT(5))
#define USB_OTGFS_DAINT_INEPINT5_M  (USB_OTGFS_DAINT_INEPINT5_V << USB_OTGFS_DAINT_INEPINT5_S)
#define USB_OTGFS_DAINT_INEPINT5_V  0x00000001U
#define USB_OTGFS_DAINT_INEPINT5_S  5
/** USB_OTGFS_DAINT_INEPINT6 : RO; bitpos: [6]; default: 0;
 *  IN Endpoint 6 Interrupt Bit
 */
#define USB_OTGFS_DAINT_INEPINT6    (BIT(6))
#define USB_OTGFS_DAINT_INEPINT6_M  (USB_OTGFS_DAINT_INEPINT6_V << USB_OTGFS_DAINT_INEPINT6_S)
#define USB_OTGFS_DAINT_INEPINT6_V  0x00000001U
#define USB_OTGFS_DAINT_INEPINT6_S  6
/** USB_OTGFS_DAINT_OUTEPINT0 : RO; bitpos: [16]; default: 0;
 *  OUT Endpoint 0 Interrupt Bit
 */
#define USB_OTGFS_DAINT_OUTEPINT0    (BIT(16))
#define USB_OTGFS_DAINT_OUTEPINT0_M  (USB_OTGFS_DAINT_OUTEPINT0_V << USB_OTGFS_DAINT_OUTEPINT0_S)
#define USB_OTGFS_DAINT_OUTEPINT0_V  0x00000001U
#define USB_OTGFS_DAINT_OUTEPINT0_S  16
/** USB_OTGFS_DAINT_OUTEPINT1 : RO; bitpos: [17]; default: 0;
 *  OUT Endpoint 1 Interrupt Bit
 */
#define USB_OTGFS_DAINT_OUTEPINT1    (BIT(17))
#define USB_OTGFS_DAINT_OUTEPINT1_M  (USB_OTGFS_DAINT_OUTEPINT1_V << USB_OTGFS_DAINT_OUTEPINT1_S)
#define USB_OTGFS_DAINT_OUTEPINT1_V  0x00000001U
#define USB_OTGFS_DAINT_OUTEPINT1_S  17
/** USB_OTGFS_DAINT_OUTEPINT2 : RO; bitpos: [18]; default: 0;
 *  OUT Endpoint 2 Interrupt Bit
 */
#define USB_OTGFS_DAINT_OUTEPINT2    (BIT(18))
#define USB_OTGFS_DAINT_OUTEPINT2_M  (USB_OTGFS_DAINT_OUTEPINT2_V << USB_OTGFS_DAINT_OUTEPINT2_S)
#define USB_OTGFS_DAINT_OUTEPINT2_V  0x00000001U
#define USB_OTGFS_DAINT_OUTEPINT2_S  18
/** USB_OTGFS_DAINT_OUTEPINT3 : RO; bitpos: [19]; default: 0;
 *  OUT Endpoint 3 Interrupt Bit
 */
#define USB_OTGFS_DAINT_OUTEPINT3    (BIT(19))
#define USB_OTGFS_DAINT_OUTEPINT3_M  (USB_OTGFS_DAINT_OUTEPINT3_V << USB_OTGFS_DAINT_OUTEPINT3_S)
#define USB_OTGFS_DAINT_OUTEPINT3_V  0x00000001U
#define USB_OTGFS_DAINT_OUTEPINT3_S  19
/** USB_OTGFS_DAINT_OUTEPINT4 : RO; bitpos: [20]; default: 0;
 *  OUT Endpoint 4 Interrupt Bit
 */
#define USB_OTGFS_DAINT_OUTEPINT4    (BIT(20))
#define USB_OTGFS_DAINT_OUTEPINT4_M  (USB_OTGFS_DAINT_OUTEPINT4_V << USB_OTGFS_DAINT_OUTEPINT4_S)
#define USB_OTGFS_DAINT_OUTEPINT4_V  0x00000001U
#define USB_OTGFS_DAINT_OUTEPINT4_S  20
/** USB_OTGFS_DAINT_OUTEPINT5 : RO; bitpos: [21]; default: 0;
 *  OUT Endpoint 5 Interrupt Bit
 */
#define USB_OTGFS_DAINT_OUTEPINT5    (BIT(21))
#define USB_OTGFS_DAINT_OUTEPINT5_M  (USB_OTGFS_DAINT_OUTEPINT5_V << USB_OTGFS_DAINT_OUTEPINT5_S)
#define USB_OTGFS_DAINT_OUTEPINT5_V  0x00000001U
#define USB_OTGFS_DAINT_OUTEPINT5_S  21
/** USB_OTGFS_DAINT_OUTEPINT6 : RO; bitpos: [22]; default: 0;
 *  OUT Endpoint 6 Interrupt Bit
 */
#define USB_OTGFS_DAINT_OUTEPINT6    (BIT(22))
#define USB_OTGFS_DAINT_OUTEPINT6_M  (USB_OTGFS_DAINT_OUTEPINT6_V << USB_OTGFS_DAINT_OUTEPINT6_S)
#define USB_OTGFS_DAINT_OUTEPINT6_V  0x00000001U
#define USB_OTGFS_DAINT_OUTEPINT6_S  22

/** USB_OTGFS_DAINTMSK_REG register
 *  Device All Endpoints Interrupt Mask Register    The Device Endpoint Interrupt Mask
 *  register works with the Device Endpoint Interrupt register to interrupt the
 *  application when an event occurs on a device endpoint. However, the Device All
 *  Endpoints Interrupt (DAINT) register bit corresponding to that interrupt is still
 *  set.
 */
#define USB_OTGFS_DAINTMSK_REG(i) (DR_REG_USB_BASE(i) + 0x81c)
/** USB_OTGFS_DAINTMSK_INEPMSK0 : R/W; bitpos: [0]; default: 0;
 *  IN Endpoint 0 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_INEPMSK0    (BIT(0))
#define USB_OTGFS_DAINTMSK_INEPMSK0_M  (USB_OTGFS_DAINTMSK_INEPMSK0_V << USB_OTGFS_DAINTMSK_INEPMSK0_S)
#define USB_OTGFS_DAINTMSK_INEPMSK0_V  0x00000001U
#define USB_OTGFS_DAINTMSK_INEPMSK0_S  0
/** USB_OTGFS_DAINTMSK_INEPMSK1 : R/W; bitpos: [1]; default: 0;
 *  IN Endpoint 1 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_INEPMSK1    (BIT(1))
#define USB_OTGFS_DAINTMSK_INEPMSK1_M  (USB_OTGFS_DAINTMSK_INEPMSK1_V << USB_OTGFS_DAINTMSK_INEPMSK1_S)
#define USB_OTGFS_DAINTMSK_INEPMSK1_V  0x00000001U
#define USB_OTGFS_DAINTMSK_INEPMSK1_S  1
/** USB_OTGFS_DAINTMSK_INEPMSK2 : R/W; bitpos: [2]; default: 0;
 *  IN Endpoint 2 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_INEPMSK2    (BIT(2))
#define USB_OTGFS_DAINTMSK_INEPMSK2_M  (USB_OTGFS_DAINTMSK_INEPMSK2_V << USB_OTGFS_DAINTMSK_INEPMSK2_S)
#define USB_OTGFS_DAINTMSK_INEPMSK2_V  0x00000001U
#define USB_OTGFS_DAINTMSK_INEPMSK2_S  2
/** USB_OTGFS_DAINTMSK_INEPMSK3 : R/W; bitpos: [3]; default: 0;
 *  IN Endpoint 3 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_INEPMSK3    (BIT(3))
#define USB_OTGFS_DAINTMSK_INEPMSK3_M  (USB_OTGFS_DAINTMSK_INEPMSK3_V << USB_OTGFS_DAINTMSK_INEPMSK3_S)
#define USB_OTGFS_DAINTMSK_INEPMSK3_V  0x00000001U
#define USB_OTGFS_DAINTMSK_INEPMSK3_S  3
/** USB_OTGFS_DAINTMSK_INEPMSK4 : R/W; bitpos: [4]; default: 0;
 *  IN Endpoint 4 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_INEPMSK4    (BIT(4))
#define USB_OTGFS_DAINTMSK_INEPMSK4_M  (USB_OTGFS_DAINTMSK_INEPMSK4_V << USB_OTGFS_DAINTMSK_INEPMSK4_S)
#define USB_OTGFS_DAINTMSK_INEPMSK4_V  0x00000001U
#define USB_OTGFS_DAINTMSK_INEPMSK4_S  4
/** USB_OTGFS_DAINTMSK_INEPMSK5 : R/W; bitpos: [5]; default: 0;
 *  IN Endpoint 5 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_INEPMSK5    (BIT(5))
#define USB_OTGFS_DAINTMSK_INEPMSK5_M  (USB_OTGFS_DAINTMSK_INEPMSK5_V << USB_OTGFS_DAINTMSK_INEPMSK5_S)
#define USB_OTGFS_DAINTMSK_INEPMSK5_V  0x00000001U
#define USB_OTGFS_DAINTMSK_INEPMSK5_S  5
/** USB_OTGFS_DAINTMSK_INEPMSK6 : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint 6 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_INEPMSK6    (BIT(6))
#define USB_OTGFS_DAINTMSK_INEPMSK6_M  (USB_OTGFS_DAINTMSK_INEPMSK6_V << USB_OTGFS_DAINTMSK_INEPMSK6_S)
#define USB_OTGFS_DAINTMSK_INEPMSK6_V  0x00000001U
#define USB_OTGFS_DAINTMSK_INEPMSK6_S  6
/** USB_OTGFS_DAINTMSK_OUTEPMSK0 : R/W; bitpos: [16]; default: 0;
 *  OUT Endpoint 0 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_OUTEPMSK0    (BIT(16))
#define USB_OTGFS_DAINTMSK_OUTEPMSK0_M  (USB_OTGFS_DAINTMSK_OUTEPMSK0_V << USB_OTGFS_DAINTMSK_OUTEPMSK0_S)
#define USB_OTGFS_DAINTMSK_OUTEPMSK0_V  0x00000001U
#define USB_OTGFS_DAINTMSK_OUTEPMSK0_S  16
/** USB_OTGFS_DAINTMSK_OUTEPMSK1 : R/W; bitpos: [17]; default: 0;
 *  OUT Endpoint 1 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_OUTEPMSK1    (BIT(17))
#define USB_OTGFS_DAINTMSK_OUTEPMSK1_M  (USB_OTGFS_DAINTMSK_OUTEPMSK1_V << USB_OTGFS_DAINTMSK_OUTEPMSK1_S)
#define USB_OTGFS_DAINTMSK_OUTEPMSK1_V  0x00000001U
#define USB_OTGFS_DAINTMSK_OUTEPMSK1_S  17
/** USB_OTGFS_DAINTMSK_OUTEPMSK2 : R/W; bitpos: [18]; default: 0;
 *  OUT Endpoint 2 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_OUTEPMSK2    (BIT(18))
#define USB_OTGFS_DAINTMSK_OUTEPMSK2_M  (USB_OTGFS_DAINTMSK_OUTEPMSK2_V << USB_OTGFS_DAINTMSK_OUTEPMSK2_S)
#define USB_OTGFS_DAINTMSK_OUTEPMSK2_V  0x00000001U
#define USB_OTGFS_DAINTMSK_OUTEPMSK2_S  18
/** USB_OTGFS_DAINTMSK_OUTEPMSK3 : R/W; bitpos: [19]; default: 0;
 *  OUT Endpoint 3 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_OUTEPMSK3    (BIT(19))
#define USB_OTGFS_DAINTMSK_OUTEPMSK3_M  (USB_OTGFS_DAINTMSK_OUTEPMSK3_V << USB_OTGFS_DAINTMSK_OUTEPMSK3_S)
#define USB_OTGFS_DAINTMSK_OUTEPMSK3_V  0x00000001U
#define USB_OTGFS_DAINTMSK_OUTEPMSK3_S  19
/** USB_OTGFS_DAINTMSK_OUTEPMSK4 : R/W; bitpos: [20]; default: 0;
 *  OUT Endpoint 4 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_OUTEPMSK4    (BIT(20))
#define USB_OTGFS_DAINTMSK_OUTEPMSK4_M  (USB_OTGFS_DAINTMSK_OUTEPMSK4_V << USB_OTGFS_DAINTMSK_OUTEPMSK4_S)
#define USB_OTGFS_DAINTMSK_OUTEPMSK4_V  0x00000001U
#define USB_OTGFS_DAINTMSK_OUTEPMSK4_S  20
/** USB_OTGFS_DAINTMSK_OUTEPMSK5 : R/W; bitpos: [21]; default: 0;
 *  OUT Endpoint 5 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_OUTEPMSK5    (BIT(21))
#define USB_OTGFS_DAINTMSK_OUTEPMSK5_M  (USB_OTGFS_DAINTMSK_OUTEPMSK5_V << USB_OTGFS_DAINTMSK_OUTEPMSK5_S)
#define USB_OTGFS_DAINTMSK_OUTEPMSK5_V  0x00000001U
#define USB_OTGFS_DAINTMSK_OUTEPMSK5_S  21
/** USB_OTGFS_DAINTMSK_OUTEPMSK6 : R/W; bitpos: [22]; default: 0;
 *  OUT Endpoint 6 Interrupt mask Bit
 */
#define USB_OTGFS_DAINTMSK_OUTEPMSK6    (BIT(22))
#define USB_OTGFS_DAINTMSK_OUTEPMSK6_M  (USB_OTGFS_DAINTMSK_OUTEPMSK6_V << USB_OTGFS_DAINTMSK_OUTEPMSK6_S)
#define USB_OTGFS_DAINTMSK_OUTEPMSK6_V  0x00000001U
#define USB_OTGFS_DAINTMSK_OUTEPMSK6_S  22

/** USB_OTGFS_DVBUSDIS_REG register
 *  Device VBUS Discharge Time Register   This register specifies the VBUS discharge
 *  time after VBUS pulsing during SRP.
 */
#define USB_OTGFS_DVBUSDIS_REG(i) (DR_REG_USB_BASE(i) + 0x828)
/** USB_OTGFS_DVBUSDIS_DVBUSDIS : R/W; bitpos: [15:0]; default: 6103;
 *  Device VBUS Discharge Time (DVBUSDis)
 *
 *  Specifies the VBUS discharge time after VBUS pulsing during SRP. This value equals
 *  (VBUS discharge time in PHY clocks) / 1, 024.
 *
 *  The value you use depends whether the PHY is operating at 30MHz (16-bit data width)
 *  or 60 MHz (8-bit data width).
 *
 *  Depending on your VBUS load, this value can need adjustment.
 */
#define USB_OTGFS_DVBUSDIS_DVBUSDIS    0x0000FFFFU
#define USB_OTGFS_DVBUSDIS_DVBUSDIS_M  (USB_OTGFS_DVBUSDIS_DVBUSDIS_V << USB_OTGFS_DVBUSDIS_DVBUSDIS_S)
#define USB_OTGFS_DVBUSDIS_DVBUSDIS_V  0x0000FFFFU
#define USB_OTGFS_DVBUSDIS_DVBUSDIS_S  0

/** USB_OTGFS_DVBUSPULSE_REG register
 *  Device VBUS Pulsing Time Register
 */
#define USB_OTGFS_DVBUSPULSE_REG(i) (DR_REG_USB_BASE(i) + 0x82c)
/** USB_OTGFS_DVBUSPULSE_DVBUSPULSE : R/W; bitpos: [11:0]; default: 1464;
 *  Device VBUS Pulsing Time (DVBUSPulse)
 *
 *  Specifies the VBUS pulsing time during SRP. This value equals (VBUS pulsing time in
 *  PHY clocks) / 1, 024
 *
 *  The value you use depends whether the PHY is operating at 30MHz (16-bit data width)
 *  or 60 MHz (8-bit data width).
 */
#define USB_OTGFS_DVBUSPULSE_DVBUSPULSE    0x00000FFFU
#define USB_OTGFS_DVBUSPULSE_DVBUSPULSE_M  (USB_OTGFS_DVBUSPULSE_DVBUSPULSE_V << USB_OTGFS_DVBUSPULSE_DVBUSPULSE_S)
#define USB_OTGFS_DVBUSPULSE_DVBUSPULSE_V  0x00000FFFU
#define USB_OTGFS_DVBUSPULSE_DVBUSPULSE_S  0

/** USB_OTGFS_DTHRCTL_REG register
 *  Device Threshold Control Register
 */
#define USB_OTGFS_DTHRCTL_REG(i) (DR_REG_USB_BASE(i) + 0x830)
/** USB_OTGFS_DTHRCTL_NONISOTHREN : R/W; bitpos: [0]; default: 0;
 *  Non-ISO IN Endpoints Threshold Enable. (NonISOThrEn)
 *
 *  When this bit is Set, the core enables thresholding for Non Isochronous IN
 *  endpoints.
 */
#define USB_OTGFS_DTHRCTL_NONISOTHREN    (BIT(0))
#define USB_OTGFS_DTHRCTL_NONISOTHREN_M  (USB_OTGFS_DTHRCTL_NONISOTHREN_V << USB_OTGFS_DTHRCTL_NONISOTHREN_S)
#define USB_OTGFS_DTHRCTL_NONISOTHREN_V  0x00000001U
#define USB_OTGFS_DTHRCTL_NONISOTHREN_S  0
/** USB_OTGFS_DTHRCTL_ISOTHREN : R/W; bitpos: [1]; default: 0;
 *
 *  ISO IN Endpoints Threshold Enable. (ISOThrEn)
 *
 *  When this bit is Set, the core enables thresholding for isochronous IN
 *  endpoints.
 */
#define USB_OTGFS_DTHRCTL_ISOTHREN    (BIT(1))
#define USB_OTGFS_DTHRCTL_ISOTHREN_M  (USB_OTGFS_DTHRCTL_ISOTHREN_V << USB_OTGFS_DTHRCTL_ISOTHREN_S)
#define USB_OTGFS_DTHRCTL_ISOTHREN_V  0x00000001U
#define USB_OTGFS_DTHRCTL_ISOTHREN_S  1
/** USB_OTGFS_DTHRCTL_TXTHRLEN : R/W; bitpos: [10:2]; default: 8;
 *  Transmit Threshold Length (TxThrLen)
 *
 *  This field specifies Transmit thresholding size in DWORDS. This also forms
 *  the MAC threshold and specifies the amount of data in bytes to be in the
 *  corresponding endpoint transmit FIFO, before the core can start transmit
 *  on the USB. The threshold length has to be at least eight DWORDS when the
 *  value of AHBThrRatio is 2'h00. In case the AHBThrRatio is non zero the
 *  application needs to ensure that the AHB Threshold value does not go below
 *  the recommended eight DWORD.  This field controls both isochronous and
 *  non-isochronous IN endpoint thresholds. The recommended value for ThrLen
 *  is to be the same as the programmed AHB Burst Length (GAHBCFG.HBstLen).
 *
 *  Note:
 *  - When OTG_ARCHITECTURE=0, the reset value of this register field is 0.
 *  - When OTG_ARCHITECTURE=2, the reset value of this register field is 8.
 */
#define USB_OTGFS_DTHRCTL_TXTHRLEN    0x000001FFU
#define USB_OTGFS_DTHRCTL_TXTHRLEN_M  (USB_OTGFS_DTHRCTL_TXTHRLEN_V << USB_OTGFS_DTHRCTL_TXTHRLEN_S)
#define USB_OTGFS_DTHRCTL_TXTHRLEN_V  0x000001FFU
#define USB_OTGFS_DTHRCTL_TXTHRLEN_S  2
/** USB_OTGFS_DTHRCTL_AHBTHRRATIO : R/W; bitpos: [12:11]; default: 0;
 *  AHB Threshold Ratio (AHBThrRatio)
 *
 *  These bits define the ratio between the AHB threshold and the MAC threshold for the
 *  transmit path only. The AHB threshold always remains less than or equal to the USB
 *  threshold, because this does not increase overhead. Both the AHB and the MAC
 *  threshold must be DWORD-aligned. The application needs to program TxThrLen and the
 *  AHBThrRatio to make the AHB Threshold value DWORD aligned. If the AHB threshold
 *  value is not DWORD aligned, the core might not behave correctly. When programming
 *  the TxThrLen and AHBThrRatio, the application must ensure that the minimum AHB
 *  threshold value does not go below 8 DWORDS to meet the USB turnaround time
 *  requirements.
 *  - 2'b00: AHB threshold = MAC threshold
 *  - 2'b01: AHB threshold = MAC threshold / 2
 *  - 2'b10: AHB threshold = MAC threshold / 4
 *  - 2'b11: AHB threshold = MAC threshold / 8
 */
#define USB_OTGFS_DTHRCTL_AHBTHRRATIO    0x00000003U
#define USB_OTGFS_DTHRCTL_AHBTHRRATIO_M  (USB_OTGFS_DTHRCTL_AHBTHRRATIO_V << USB_OTGFS_DTHRCTL_AHBTHRRATIO_S)
#define USB_OTGFS_DTHRCTL_AHBTHRRATIO_V  0x00000003U
#define USB_OTGFS_DTHRCTL_AHBTHRRATIO_S  11
/** USB_OTGFS_DTHRCTL_RXTHREN : R/W; bitpos: [16]; default: 0;
 *  Receive Threshold Enable (RxThrEn)
 *
 *  When this bit is set, the core enables thresholding in the receive direction.
 *
 *  Note: We recommends that you do not enable RxThrEn, because it may cause issues in
 *  the RxFIFO especially during error conditions such as RxError and Babble.
 */
#define USB_OTGFS_DTHRCTL_RXTHREN    (BIT(16))
#define USB_OTGFS_DTHRCTL_RXTHREN_M  (USB_OTGFS_DTHRCTL_RXTHREN_V << USB_OTGFS_DTHRCTL_RXTHREN_S)
#define USB_OTGFS_DTHRCTL_RXTHREN_V  0x00000001U
#define USB_OTGFS_DTHRCTL_RXTHREN_S  16
/** USB_OTGFS_DTHRCTL_RXTHRLEN : R/W; bitpos: [25:17]; default: 8;
 *  Receive Threshold Length (RxThrLen)
 *
 *  This field specifies Receive thresholding size in DWORDS.
 *  This field also specifies the amount of data received on the USB before the core
 *  can start transmitting on the AHB.
 *  The threshold length has to be at least eight DWORDS.
 *  The recommended value for ThrLen is to be the same as the programmed
 *  AHB Burst Length (GAHBCFG.HBstLen).
 */
#define USB_OTGFS_DTHRCTL_RXTHRLEN    0x000001FFU
#define USB_OTGFS_DTHRCTL_RXTHRLEN_M  (USB_OTGFS_DTHRCTL_RXTHRLEN_V << USB_OTGFS_DTHRCTL_RXTHRLEN_S)
#define USB_OTGFS_DTHRCTL_RXTHRLEN_V  0x000001FFU
#define USB_OTGFS_DTHRCTL_RXTHRLEN_S  17
/** USB_OTGFS_DTHRCTL_ARBPRKEN : R/W; bitpos: [27]; default: 1;
 *  Arbiter Parking Enable (ArbPrkEn)
 *
 *  This bit controls internal DMA arbiter parking for IN endpoints. If thresholding is
 *  enabled and this bit is set to one, then the arbiter parks on the IN endpoint for
 *  which there is a token received on the USB. This is done to avoid getting into
 *  underrun conditions. By default, arbiter parking is enabled.
 */
#define USB_OTGFS_DTHRCTL_ARBPRKEN    (BIT(27))
#define USB_OTGFS_DTHRCTL_ARBPRKEN_M  (USB_OTGFS_DTHRCTL_ARBPRKEN_V << USB_OTGFS_DTHRCTL_ARBPRKEN_S)
#define USB_OTGFS_DTHRCTL_ARBPRKEN_V  0x00000001U
#define USB_OTGFS_DTHRCTL_ARBPRKEN_S  27

/** USB_OTGFS_DIEPEMPMSK_REG register
 *  Device IN Endpoint FIFO Empty Interrupt Mask Register This register is valid only
 *  in Dedicated FIFO operation (OTG_EN_DED_TX_FIFO = 1). This register is used to
 *  control the IN endpoint FIFO empty interrupt generation (DIEPINTn.TxfEmp).
 */
#define USB_OTGFS_DIEPEMPMSK_REG(i) (DR_REG_USB_BASE(i) + 0x834)
/** USB_OTGFS_DIEPEMPMSK_INEPTXFEMPMSK : R/W; bitpos: [15:0]; default: 0;
 *  IN EP Tx FIFO Empty Interrupt Mask Bits (InEpTxfEmpMsk)
 *
 *  These bits acts as mask bits for DIEPINTn.TxFEmp interrupt, one bit per IN Endpoint:
 *
 *  Bit 0 for IN EP 0, bit 15 for IN EP 15
 */
#define USB_OTGFS_DIEPEMPMSK_INEPTXFEMPMSK    0x0000FFFFU
#define USB_OTGFS_DIEPEMPMSK_INEPTXFEMPMSK_M  (USB_OTGFS_DIEPEMPMSK_INEPTXFEMPMSK_V << USB_OTGFS_DIEPEMPMSK_INEPTXFEMPMSK_S)
#define USB_OTGFS_DIEPEMPMSK_INEPTXFEMPMSK_V  0x0000FFFFU
#define USB_OTGFS_DIEPEMPMSK_INEPTXFEMPMSK_S  0

/** USB_OTGFS_DIEPCTL0_REG register
 *  Device Control IN Endpoint 0 Control Register
 */
#define USB_OTGFS_DIEPCTL0_REG(i) (DR_REG_USB_BASE(i) + 0x900)
/** USB_OTGFS_DIEPCTL0_MPS : R/W; bitpos: [1:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application must program this field with the maximum packet size for the
 *  current logical endpoint.
 *  - 2'b00: 64 bytes
 *  - 2'b01: 32 bytes
 *  - 2'b10: 16 bytes
 *  - 2'b11: 8 bytes
 */
#define USB_OTGFS_DIEPCTL0_MPS    0x00000003U
#define USB_OTGFS_DIEPCTL0_MPS_M  (USB_OTGFS_DIEPCTL0_MPS_V << USB_OTGFS_DIEPCTL0_MPS_S)
#define USB_OTGFS_DIEPCTL0_MPS_V  0x00000003U
#define USB_OTGFS_DIEPCTL0_MPS_S  0
/** USB_OTGFS_DIEPCTL0_USBACTEP : RO; bitpos: [15]; default: 1;
 *  USB Active Endpoint (USBActEP)
 *
 *  This bit is always SET to 1, indicating that control endpoint 0 is always active in
 *  all configurations and interfaces.
 */
#define USB_OTGFS_DIEPCTL0_USBACTEP    (BIT(15))
#define USB_OTGFS_DIEPCTL0_USBACTEP_M  (USB_OTGFS_DIEPCTL0_USBACTEP_V << USB_OTGFS_DIEPCTL0_USBACTEP_S)
#define USB_OTGFS_DIEPCTL0_USBACTEP_V  0x00000001U
#define USB_OTGFS_DIEPCTL0_USBACTEP_S  15
/** USB_OTGFS_DIEPCTL0_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When this bit is set, either by the application or core, the core stops
 *  transmitting data, even If there is data available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data
 *  packets with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL0_NAKSTS    (BIT(17))
#define USB_OTGFS_DIEPCTL0_NAKSTS_M  (USB_OTGFS_DIEPCTL0_NAKSTS_V << USB_OTGFS_DIEPCTL0_NAKSTS_S)
#define USB_OTGFS_DIEPCTL0_NAKSTS_V  0x00000001U
#define USB_OTGFS_DIEPCTL0_NAKSTS_S  17
/** USB_OTGFS_DIEPCTL0_EPTYPE : RO; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Hardcoded to 00 for control.
 */
#define USB_OTGFS_DIEPCTL0_EPTYPE    0x00000003U
#define USB_OTGFS_DIEPCTL0_EPTYPE_M  (USB_OTGFS_DIEPCTL0_EPTYPE_V << USB_OTGFS_DIEPCTL0_EPTYPE_S)
#define USB_OTGFS_DIEPCTL0_EPTYPE_V  0x00000003U
#define USB_OTGFS_DIEPCTL0_EPTYPE_S  18
/** USB_OTGFS_DIEPCTL0_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  The application can only set this bit, and the core clears it, when a
 *  SETUP token is received for this endpoint. If a NAK bit, Global Nonperiodic
 *  IN NAK, or Global OUT NAK is set along with this bit, the STALL
 *  bit takes priority.
 */
#define USB_OTGFS_DIEPCTL0_STALL    (BIT(21))
#define USB_OTGFS_DIEPCTL0_STALL_M  (USB_OTGFS_DIEPCTL0_STALL_V << USB_OTGFS_DIEPCTL0_STALL_S)
#define USB_OTGFS_DIEPCTL0_STALL_V  0x00000001U
#define USB_OTGFS_DIEPCTL0_STALL_S  21
/** USB_OTGFS_DIEPCTL0_TXFNUM : R/W; bitpos: [25:22]; default: 0;
 *  TxFIFO Number (TxFNum)
 *  - For Shared FIFO operation, this value is always set to 0, indicating that control
 *  IN endpoint 0 data is always written in the Non-Periodic Transmit FIFO.
 *  - For Dedicated FIFO operation, this value is set to the FIFO number that is
 *  assigned to IN Endpoint.
 */
#define USB_OTGFS_DIEPCTL0_TXFNUM    0x0000000FU
#define USB_OTGFS_DIEPCTL0_TXFNUM_M  (USB_OTGFS_DIEPCTL0_TXFNUM_V << USB_OTGFS_DIEPCTL0_TXFNUM_S)
#define USB_OTGFS_DIEPCTL0_TXFNUM_V  0x0000000FU
#define USB_OTGFS_DIEPCTL0_TXFNUM_S  22
/** USB_OTGFS_DIEPCTL0_CNAK : R/W; bitpos: [26]; default: 0;
 *
 *  Clear NAK (CNAK)
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DIEPCTL0_CNAK    (BIT(26))
#define USB_OTGFS_DIEPCTL0_CNAK_M  (USB_OTGFS_DIEPCTL0_CNAK_V << USB_OTGFS_DIEPCTL0_CNAK_S)
#define USB_OTGFS_DIEPCTL0_CNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL0_CNAK_S  26
/** USB_OTGFS_DIEPCTL0_SNAK : R/W; bitpos: [27]; default: 0;
 *
 *  Set NAK (SNAK)
 *  A write to this bit sets the NAK bit for the endpoint.
 *  Using this bit, the application can control the transmission of NAK
 *  handshakes on an endpoint. The core can also set this bit for an
 *  endpoint after a SETUP packet is received on that endpoint.
 */
#define USB_OTGFS_DIEPCTL0_SNAK    (BIT(27))
#define USB_OTGFS_DIEPCTL0_SNAK_M  (USB_OTGFS_DIEPCTL0_SNAK_V << USB_OTGFS_DIEPCTL0_SNAK_S)
#define USB_OTGFS_DIEPCTL0_SNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL0_SNAK_S  27
/** USB_OTGFS_DIEPCTL0_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  The application sets this bit to stop transmitting data on an endpoint,
 *  even before the transfer for that endpoint is complete. The application
 *  must wait for the Endpoint Disabled interrupt before treating the endpoint
 *  as disabled. The core clears this bit before setting the Endpoint Disabled
 *  Interrupt. The application must Set this bit only if Endpoint Enable is
 *  already set for this endpoint.
 */
#define USB_OTGFS_DIEPCTL0_EPDIS    (BIT(30))
#define USB_OTGFS_DIEPCTL0_EPDIS_M  (USB_OTGFS_DIEPCTL0_EPDIS_V << USB_OTGFS_DIEPCTL0_EPDIS_S)
#define USB_OTGFS_DIEPCTL0_EPDIS_V  0x00000001U
#define USB_OTGFS_DIEPCTL0_EPDIS_S  30
/** USB_OTGFS_DIEPCTL0_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  When Scatter/Gather DMA mode is enabled for IN endpoints, this bit indicates that
 *  the descriptor structure and data buffer with data ready to transmit is setup.
 *
 *  When Scatter/Gather DMA mode is disabled (such as in buffer pointer based DMA mode)
 *  this bit indicates that data is ready to be transmitted on the endpoint.
 *  The core clears this bit before setting the following interrupts on this endpoint:
 *  - Endpoint Disabled
 *  - Transfer Completed
 */
#define USB_OTGFS_DIEPCTL0_EPENA    (BIT(31))
#define USB_OTGFS_DIEPCTL0_EPENA_M  (USB_OTGFS_DIEPCTL0_EPENA_V << USB_OTGFS_DIEPCTL0_EPENA_S)
#define USB_OTGFS_DIEPCTL0_EPENA_V  0x00000001U
#define USB_OTGFS_DIEPCTL0_EPENA_S  31

/** USB_OTGFS_DIEPINT0_REG register
 *  Device IN Endpoint 0 Interrupt Register   This register indicates the status of an
 *  endpoint with respect to USB- and AHB-related events. It is shown in the Interrupt
 *  Hierarchy figure in the databook. The application must read this register when the
 *  OUT Endpoints Interrupt bit or IN Endpoints Interrupt bit of the Core Interrupt
 *  register (GINTSTS.OEPInt or GINTSTS.IEPInt, respectively) is set. Before the
 *  application can read this register, it must first read the Device All Endpoints
 *  Interrupt (DAINT) register to get the exact endpoint number for the Device
 *  Endpoint-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the DAINT and GINTSTS registers
 */
#define USB_OTGFS_DIEPINT0_REG(i) (DR_REG_USB_BASE(i) + 0x908)
/** USB_OTGFS_DIEPINT0_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DIEPINT0_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DIEPINT0_XFERCOMPL_M  (USB_OTGFS_DIEPINT0_XFERCOMPL_V << USB_OTGFS_DIEPINT0_XFERCOMPL_S)
#define USB_OTGFS_DIEPINT0_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DIEPINT0_XFERCOMPL_S  0
/** USB_OTGFS_DIEPINT0_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DIEPINT0_EPDISBLD    (BIT(1))
#define USB_OTGFS_DIEPINT0_EPDISBLD_M  (USB_OTGFS_DIEPINT0_EPDISBLD_V << USB_OTGFS_DIEPINT0_EPDISBLD_S)
#define USB_OTGFS_DIEPINT0_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DIEPINT0_EPDISBLD_S  1
/** USB_OTGFS_DIEPINT0_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an AHB error during an
 *  AHB read/write. The application can read the corresponding endpoint DMA address
 *  register to get the error address.
 */
#define USB_OTGFS_DIEPINT0_AHBERR    (BIT(2))
#define USB_OTGFS_DIEPINT0_AHBERR_M  (USB_OTGFS_DIEPINT0_AHBERR_V << USB_OTGFS_DIEPINT0_AHBERR_S)
#define USB_OTGFS_DIEPINT0_AHBERR_V  0x00000001U
#define USB_OTGFS_DIEPINT0_AHBERR_S  2
/** USB_OTGFS_DIEPINT0_TIMEOUT : R/W; bitpos: [3]; default: 0;
 *  Timeout Condition (TimeOUT)
 *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
 *  - In dedicated FIFO mode, applies only to Control IN endpoints.
 *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
 *  Indicates that the core has detected a timeout condition on the USB for the last IN
 *  token on this endpoint.
 */
#define USB_OTGFS_DIEPINT0_TIMEOUT    (BIT(3))
#define USB_OTGFS_DIEPINT0_TIMEOUT_M  (USB_OTGFS_DIEPINT0_TIMEOUT_V << USB_OTGFS_DIEPINT0_TIMEOUT_S)
#define USB_OTGFS_DIEPINT0_TIMEOUT_V  0x00000001U
#define USB_OTGFS_DIEPINT0_TIMEOUT_S  3
/** USB_OTGFS_DIEPINT0_INTKNTXFEMP : R/W; bitpos: [4]; default: 0;
 *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that an IN token was received when the associated TxFIFO
 *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
 *  which the IN token was received.
 */
#define USB_OTGFS_DIEPINT0_INTKNTXFEMP    (BIT(4))
#define USB_OTGFS_DIEPINT0_INTKNTXFEMP_M  (USB_OTGFS_DIEPINT0_INTKNTXFEMP_V << USB_OTGFS_DIEPINT0_INTKNTXFEMP_S)
#define USB_OTGFS_DIEPINT0_INTKNTXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT0_INTKNTXFEMP_S  4
/** USB_OTGFS_DIEPINT0_INTKNEPMIS : R/W; bitpos: [5]; default: 0;
 *  IN Token Received with EP Mismatch (INTknEPMis)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
 *  endpoint other than the one for which the IN token was received. This interrupt is
 *  asserted on the endpoint for which the IN token was received.
 */
#define USB_OTGFS_DIEPINT0_INTKNEPMIS    (BIT(5))
#define USB_OTGFS_DIEPINT0_INTKNEPMIS_M  (USB_OTGFS_DIEPINT0_INTKNEPMIS_V << USB_OTGFS_DIEPINT0_INTKNEPMIS_S)
#define USB_OTGFS_DIEPINT0_INTKNEPMIS_V  0x00000001U
#define USB_OTGFS_DIEPINT0_INTKNEPMIS_S  5
/** USB_OTGFS_DIEPINT0_INEPNAKEFF : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective (INEPNakEff)
 *
 *  Applies to periodic IN endpoints only.
 *
 *  This bit can be cleared when the application clears the IN endpoint NAK by writing
 *  to DIEPCTLn.CNAK.
 *
 *  This interrupt indicates that the core has sampled the NAK bit
 *
 *  Set (either by the application or by the core).
 *
 *  The interrupt indicates that the IN endpoint NAK bit Set by the application has
 *  taken effect in the core.
 *
 *  This interrupt does not guarantee that a NAK handshake is sent on the USB. A STALL
 *  bit takes priority over a NAK bit.
 */
#define USB_OTGFS_DIEPINT0_INEPNAKEFF    (BIT(6))
#define USB_OTGFS_DIEPINT0_INEPNAKEFF_M  (USB_OTGFS_DIEPINT0_INEPNAKEFF_V << USB_OTGFS_DIEPINT0_INEPNAKEFF_S)
#define USB_OTGFS_DIEPINT0_INEPNAKEFF_V  0x00000001U
#define USB_OTGFS_DIEPINT0_INEPNAKEFF_S  6
/** USB_OTGFS_DIEPINT0_TXFEMP : RO; bitpos: [7]; default: 1;
 *  Transmit FIFO Empty (TxFEmp)
 *
 *  This bit is valid only for IN Endpoints
 *
 *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
 *  completely empty. The half or completely empty status is determined by the TxFIFO
 *  Empty Level bit in the Core AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
 */
#define USB_OTGFS_DIEPINT0_TXFEMP    (BIT(7))
#define USB_OTGFS_DIEPINT0_TXFEMP_M  (USB_OTGFS_DIEPINT0_TXFEMP_V << USB_OTGFS_DIEPINT0_TXFEMP_S)
#define USB_OTGFS_DIEPINT0_TXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT0_TXFEMP_S  7
/** USB_OTGFS_DIEPINT0_TXFIFOUNDRN : R/W; bitpos: [8]; default: 0;
 *  Fifo Underrun (TxfifoUndrn)
 *
 *  Applies to IN endpoints only.
 *
 *  The core generates this interrupt when it detects a transmit FIFO underrun
 *  condition in threshold mode for this endpoint.
 */
#define USB_OTGFS_DIEPINT0_TXFIFOUNDRN    (BIT(8))
#define USB_OTGFS_DIEPINT0_TXFIFOUNDRN_M  (USB_OTGFS_DIEPINT0_TXFIFOUNDRN_V << USB_OTGFS_DIEPINT0_TXFIFOUNDRN_S)
#define USB_OTGFS_DIEPINT0_TXFIFOUNDRN_V  0x00000001U
#define USB_OTGFS_DIEPINT0_TXFIFOUNDRN_S  8
/** USB_OTGFS_DIEPINT0_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
 *  this interrupt when the descriptor accessed is not ready for the Core to process,
 *  such as Host busy or DMA done.
 */
#define USB_OTGFS_DIEPINT0_BNAINTR    (BIT(9))
#define USB_OTGFS_DIEPINT0_BNAINTR_M  (USB_OTGFS_DIEPINT0_BNAINTR_V << USB_OTGFS_DIEPINT0_BNAINTR_S)
#define USB_OTGFS_DIEPINT0_BNAINTR_V  0x00000001U
#define USB_OTGFS_DIEPINT0_BNAINTR_S  9
/** USB_OTGFS_DIEPINT0_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped.
 *  This bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DIEPINT0_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DIEPINT0_PKTDRPSTS_M  (USB_OTGFS_DIEPINT0_PKTDRPSTS_V << USB_OTGFS_DIEPINT0_PKTDRPSTS_S)
#define USB_OTGFS_DIEPINT0_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DIEPINT0_PKTDRPSTS_S  11
/** USB_OTGFS_DIEPINT0_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DIEPINT0_BBLEERR    (BIT(12))
#define USB_OTGFS_DIEPINT0_BBLEERR_M  (USB_OTGFS_DIEPINT0_BBLEERR_V << USB_OTGFS_DIEPINT0_BBLEERR_S)
#define USB_OTGFS_DIEPINT0_BBLEERR_V  0x00000001U
#define USB_OTGFS_DIEPINT0_BBLEERR_S  12
/** USB_OTGFS_DIEPINT0_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *  <bring case of isochronous IN endpoints the interrupt gets generated when a zero
 *  length packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DIEPINT0_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DIEPINT0_NAKINTRPT_M  (USB_OTGFS_DIEPINT0_NAKINTRPT_V << USB_OTGFS_DIEPINT0_NAKINTRPT_S)
#define USB_OTGFS_DIEPINT0_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT0_NAKINTRPT_S  13
/** USB_OTGFS_DIEPINT0_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DIEPINT0_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DIEPINT0_NYETINTRPT_M  (USB_OTGFS_DIEPINT0_NYETINTRPT_V << USB_OTGFS_DIEPINT0_NYETINTRPT_S)
#define USB_OTGFS_DIEPINT0_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT0_NYETINTRPT_S  14

/** USB_OTGFS_DIEPTSIZ0_REG register
 *  Device IN Endpoint 0 Transfer Size Register   The application must modify this
 *  register before enabling endpoint 0. Once endpoint 0 is enabled using Endpoint
 *  Enable bit of the Device Control Endpoint 0 Control registers
 *  (DIEPCTL0.EPEna/DOEPCTL0.EPEna), the core modifies this register. The application
 *  can only read this register once the core has cleared the Endpoint Enable bit.
 *  Nonzero endpoints use the registers for endpoints 115. When Scatter/Gather DMA mode
 *  is enabled, this register must not be programmed by the application. If the
 *  application reads this register when Scatter/Gather DMA mode is enabled, the core
 *  returns all zeros.
 */
#define USB_OTGFS_DIEPTSIZ0_REG(i) (DR_REG_USB_BASE(i) + 0x910)
/** USB_OTGFS_DIEPTSIZ0_XFERSIZE : R/W; bitpos: [6:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  This field contains the transfer size in bytes for the current endpoint. The
 *  transfer size (XferSize) = Sum of buffer sizes across all descriptors in the list
 *  for the endpoint.
 *  In Buffer DMA, the core only interrupts the application after it has exhausted the
 *  transfer size amount of data. The transfer size can be set to the maximum packet
 *  size of the endpoint, to be interrupted at the end of each packet.
 *  - IN Endpoints: The core decrements this field every time a packet from the
 *  external memory is written to the TxFIFO.
 *  - OUT Endpoints: The core decrements this field every time a packet is read from
 *  the RxFIFO and written to the external memory.
 */
#define USB_OTGFS_DIEPTSIZ0_XFERSIZE    0x0000007FU
#define USB_OTGFS_DIEPTSIZ0_XFERSIZE_M  (USB_OTGFS_DIEPTSIZ0_XFERSIZE_V << USB_OTGFS_DIEPTSIZ0_XFERSIZE_S)
#define USB_OTGFS_DIEPTSIZ0_XFERSIZE_V  0x0000007FU
#define USB_OTGFS_DIEPTSIZ0_XFERSIZE_S  0
/** USB_OTGFS_DIEPTSIZ0_PKTCNT : R/W; bitpos: [20:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  Indicates the total number of USB packets that constitute the
 *
 *  Transfer Size amount of data for endpoint 0.
 *  - IN Endpoints : This field is decremented every time a packet (maximum size or
 *  short packet) is read from the TxFIFO.
 *  - OUT Endpoints: This field is decremented every time a packet (maximum size or
 *  short packet) is written to the RxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ0_PKTCNT    0x00000003U
#define USB_OTGFS_DIEPTSIZ0_PKTCNT_M  (USB_OTGFS_DIEPTSIZ0_PKTCNT_V << USB_OTGFS_DIEPTSIZ0_PKTCNT_S)
#define USB_OTGFS_DIEPTSIZ0_PKTCNT_V  0x00000003U
#define USB_OTGFS_DIEPTSIZ0_PKTCNT_S  19

/** USB_OTGFS_DIEPDMA0_REG register
 *  Device IN Endpoint 0 DMA Address Register
 */
#define USB_OTGFS_DIEPDMA0_REG(i) (DR_REG_USB_BASE(i) + 0x914)
/** USB_OTGFS_DIEPDMA0_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  DMAAddr
 *
 *  This field holds the start address of the external memory for storing or fetching
 *  endpoint data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *
 *  When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *
 *  When Scatter/Gather DMA mode is enabled, this field indicates the base pointer for
 *  the descriptor list.
 */
#define USB_OTGFS_DIEPDMA0_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA0_DMAADDR_M  (USB_OTGFS_DIEPDMA0_DMAADDR_V << USB_OTGFS_DIEPDMA0_DMAADDR_S)
#define USB_OTGFS_DIEPDMA0_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA0_DMAADDR_S  0

/** USB_OTGFS_DTXFSTS0_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 0
 */
#define USB_OTGFS_DTXFSTS0_REG(i) (DR_REG_USB_BASE(i) + 0x918)
/** USB_OTGFS_DTXFSTS0_INEPTXFSPCAVAIL : RO; bitpos: [15:0]; default: 256;
 *  IN Endpoint TxFIFO Space Avail (INEPTxFSpcAvail)
 *
 *  Indicates the amount of free space available in the Endpoint TxFIFO.
 *
 *  Values are in terms of 32-bit words.
 *  - 16'h0: Endpoint TxFIFO is full
 *  - 16'h1: 1 word available
 *  - 16'h2: 2 words available
 *  - 16'hn: n words available (where 0  n  32,768)
 *  - 16'h8000: 32,768 words available
 *  - Others: Reserved
 */
#define USB_OTGFS_DTXFSTS0_INEPTXFSPCAVAIL    0x0000FFFFU
#define USB_OTGFS_DTXFSTS0_INEPTXFSPCAVAIL_M  (USB_OTGFS_DTXFSTS0_INEPTXFSPCAVAIL_V << USB_OTGFS_DTXFSTS0_INEPTXFSPCAVAIL_S)
#define USB_OTGFS_DTXFSTS0_INEPTXFSPCAVAIL_V  0x0000FFFFU
#define USB_OTGFS_DTXFSTS0_INEPTXFSPCAVAIL_S  0

/** USB_OTGFS_DIEPDMAB0_REG register
 *  Device IN Endpoint 16 Buffer Address Register
 */
#define USB_OTGFS_DIEPDMAB0_REG(i) (DR_REG_USB_BASE(i) + 0x91c)
/** USB_OTGFS_DIEPDMAB0_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DIEPDMAB0_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB0_DMABUFFERADDR_M  (USB_OTGFS_DIEPDMAB0_DMABUFFERADDR_V << USB_OTGFS_DIEPDMAB0_DMABUFFERADDR_S)
#define USB_OTGFS_DIEPDMAB0_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB0_DMABUFFERADDR_S  0

/** USB_OTGFS_DIEPCTL1_REG register
 *  Device Control IN Endpoint 1 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPCTL1_REG(i) (DR_REG_USB_BASE(i) + 0x920)
/** USB_OTGFS_DIEPCTL1_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the
 *  current logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DIEPCTL1_MPS    0x000007FFU
#define USB_OTGFS_DIEPCTL1_MPS_M  (USB_OTGFS_DIEPCTL1_MPS_V << USB_OTGFS_DIEPCTL1_MPS_S)
#define USB_OTGFS_DIEPCTL1_MPS_V  0x000007FFU
#define USB_OTGFS_DIEPCTL1_MPS_S  0
/** USB_OTGFS_DIEPCTL1_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DIEPCTL1_USBACTEP    (BIT(15))
#define USB_OTGFS_DIEPCTL1_USBACTEP_M  (USB_OTGFS_DIEPCTL1_USBACTEP_V << USB_OTGFS_DIEPCTL1_USBACTEP_S)
#define USB_OTGFS_DIEPCTL1_USBACTEP_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_USBACTEP_S  15
/** USB_OTGFS_DIEPCTL1_DPID : RO; bitpos: [16]; default: 0;
 *
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *
 *  Applies to isochronous IN and OUT endpoints only.
 *
 *  Indicates the (micro)frame number in which the core transmits/receives isochronous
 *  data for this endpoint. The application must program the even/odd (micro)frame
 *  number in which it intends to transmit/receive isochronous data for this endpoint
 *  using
 *  the SetEvnFr and SetOddFr fields in this register.
 *  - 1'b0: Even (micro)frame
 *  - 1'b1: Odd (micro)frame
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame in
 *  which data is received is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL1_DPID    (BIT(16))
#define USB_OTGFS_DIEPCTL1_DPID_M  (USB_OTGFS_DIEPCTL1_DPID_V << USB_OTGFS_DIEPCTL1_DPID_S)
#define USB_OTGFS_DIEPCTL1_DPID_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_DPID_S  16
/** USB_OTGFS_DIEPCTL1_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL1_NAKSTS    (BIT(17))
#define USB_OTGFS_DIEPCTL1_NAKSTS_M  (USB_OTGFS_DIEPCTL1_NAKSTS_V << USB_OTGFS_DIEPCTL1_NAKSTS_S)
#define USB_OTGFS_DIEPCTL1_NAKSTS_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_NAKSTS_S  17
/** USB_OTGFS_DIEPCTL1_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DIEPCTL1_EPTYPE    0x00000003U
#define USB_OTGFS_DIEPCTL1_EPTYPE_M  (USB_OTGFS_DIEPCTL1_EPTYPE_V << USB_OTGFS_DIEPCTL1_EPTYPE_S)
#define USB_OTGFS_DIEPCTL1_EPTYPE_V  0x00000003U
#define USB_OTGFS_DIEPCTL1_EPTYPE_S  18
/** USB_OTGFS_DIEPCTL1_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL1_STALL    (BIT(21))
#define USB_OTGFS_DIEPCTL1_STALL_M  (USB_OTGFS_DIEPCTL1_STALL_V << USB_OTGFS_DIEPCTL1_STALL_S)
#define USB_OTGFS_DIEPCTL1_STALL_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_STALL_S  21
/** USB_OTGFS_DIEPCTL1_TXFNUM : R/W; bitpos: [25:22]; default: 0;
 *  TxFIFO Number (TxFNum)
 *
 *  Shared FIFO Operation non-periodic endpoints must set this bit to zero. Periodic
 *  endpoints must map this to the corresponding Periodic TxFIFO number.
 *  - 4'h0: Non-Periodic TxFIFO
 *  - Others: Specified Periodic TxFIFO.number
 *  Note: An interrupt IN endpoint can be configured as a non-periodic endpoint for
 *  applications such as mass storage. The core treats an IN endpoint as a non-periodic
 *  endpoint if the TxFNum field is set to 0. Otherwise, a separate periodic FIFO must
 *  be
 *  allocated for an interrupt IN endpoint, and the number of this
 *  FIFO must be programmed into the TxFNum field. Configuring an interrupt IN
 *  endpoint as a non-periodic endpoint saves the extra periodic FIFO area.
 *
 *  Dedicated FIFO Operation: These bits specify the FIFO number associated with this
 *  endpoint. Each active IN endpoint must be programmed to a separate FIFO number.
 *  This field is valid only for IN endpoints.
 */
#define USB_OTGFS_DIEPCTL1_TXFNUM    0x0000000FU
#define USB_OTGFS_DIEPCTL1_TXFNUM_M  (USB_OTGFS_DIEPCTL1_TXFNUM_V << USB_OTGFS_DIEPCTL1_TXFNUM_S)
#define USB_OTGFS_DIEPCTL1_TXFNUM_V  0x0000000FU
#define USB_OTGFS_DIEPCTL1_TXFNUM_S  22
/** USB_OTGFS_DIEPCTL1_CNAK : R/W; bitpos: [26]; default: 0;
 *  Clear NAK (CNAK)
 *
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DIEPCTL1_CNAK    (BIT(26))
#define USB_OTGFS_DIEPCTL1_CNAK_M  (USB_OTGFS_DIEPCTL1_CNAK_V << USB_OTGFS_DIEPCTL1_CNAK_S)
#define USB_OTGFS_DIEPCTL1_CNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_CNAK_S  26
/** USB_OTGFS_DIEPCTL1_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK handshakes on
 *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
 *  received on that endpoint.
 */
#define USB_OTGFS_DIEPCTL1_SNAK    (BIT(27))
#define USB_OTGFS_DIEPCTL1_SNAK_M  (USB_OTGFS_DIEPCTL1_SNAK_V << USB_OTGFS_DIEPCTL1_SNAK_S)
#define USB_OTGFS_DIEPCTL1_SNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_SNAK_S  27
/** USB_OTGFS_DIEPCTL1_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  SetD0PID
 *  - Set DATA0 PID (SetD0PID)
 *  -- Applies to interrupt/bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA0.
 *  -- This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  In non-Scatter/Gather DMA mode: Set Even (micro)Frame (SetEvenFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the Even/Odd (micro)Frame (EO_FrNum) field to even
 *  (micro)Frame.
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which to
 *  receive data is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL1_SETD0PID    (BIT(28))
#define USB_OTGFS_DIEPCTL1_SETD0PID_M  (USB_OTGFS_DIEPCTL1_SETD0PID_V << USB_OTGFS_DIEPCTL1_SETD0PID_S)
#define USB_OTGFS_DIEPCTL1_SETD0PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_SETD0PID_S  28
/** USB_OTGFS_DIEPCTL1_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  SetD1PID
 *  - Set DATA1 PID (SetD1PID)
 *  -- Applies to interrupt and bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA1.
 *  -- This field is applicable both for Scatter-Gather DMA mode and non Scatter-Gather
 *  DMA mode.
 *  - Set odd (micro)Frame (SetOddFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the even and odd (micro)Frame (EO_FrNum) field to odd
 *  (micro)Frame.
 *  -- This field is not applicable for Scatter-Gather DMA mode.
 */
#define USB_OTGFS_DIEPCTL1_SETD1PID    (BIT(29))
#define USB_OTGFS_DIEPCTL1_SETD1PID_M  (USB_OTGFS_DIEPCTL1_SETD1PID_V << USB_OTGFS_DIEPCTL1_SETD1PID_S)
#define USB_OTGFS_DIEPCTL1_SETD1PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_SETD1PID_S  29
/** USB_OTGFS_DIEPCTL1_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DIEPCTL1_EPDIS    (BIT(30))
#define USB_OTGFS_DIEPCTL1_EPDIS_M  (USB_OTGFS_DIEPCTL1_EPDIS_V << USB_OTGFS_DIEPCTL1_EPDIS_S)
#define USB_OTGFS_DIEPCTL1_EPDIS_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_EPDIS_S  30
/** USB_OTGFS_DIEPCTL1_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled,
 *  -- For IN endpoints this bit indicates that the descriptor structure and data
 *  buffer with data ready to transmit is setup.
 *  -- For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  - When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  -- For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  -- For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  - The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  -- SETUP Phase Done
 *  -- Endpoint Disabled
 *  -- Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DIEPCTL1_EPENA    (BIT(31))
#define USB_OTGFS_DIEPCTL1_EPENA_M  (USB_OTGFS_DIEPCTL1_EPENA_V << USB_OTGFS_DIEPCTL1_EPENA_S)
#define USB_OTGFS_DIEPCTL1_EPENA_V  0x00000001U
#define USB_OTGFS_DIEPCTL1_EPENA_S  31

/** USB_OTGFS_DIEPINT1_REG register
 *  Device IN Endpoint 1 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPINT1_REG(i) (DR_REG_USB_BASE(i) + 0x928)
/** USB_OTGFS_DIEPINT1_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DIEPINT1_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DIEPINT1_XFERCOMPL_M  (USB_OTGFS_DIEPINT1_XFERCOMPL_V << USB_OTGFS_DIEPINT1_XFERCOMPL_S)
#define USB_OTGFS_DIEPINT1_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DIEPINT1_XFERCOMPL_S  0
/** USB_OTGFS_DIEPINT1_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DIEPINT1_EPDISBLD    (BIT(1))
#define USB_OTGFS_DIEPINT1_EPDISBLD_M  (USB_OTGFS_DIEPINT1_EPDISBLD_V << USB_OTGFS_DIEPINT1_EPDISBLD_S)
#define USB_OTGFS_DIEPINT1_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DIEPINT1_EPDISBLD_S  1
/** USB_OTGFS_DIEPINT1_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DIEPINT1_AHBERR    (BIT(2))
#define USB_OTGFS_DIEPINT1_AHBERR_M  (USB_OTGFS_DIEPINT1_AHBERR_V << USB_OTGFS_DIEPINT1_AHBERR_S)
#define USB_OTGFS_DIEPINT1_AHBERR_V  0x00000001U
#define USB_OTGFS_DIEPINT1_AHBERR_S  2
/** USB_OTGFS_DIEPINT1_TIMEOUT : R/W; bitpos: [3]; default: 0;
 *  Timeout Condition (TimeOUT)
 *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
 *  - In dedicated FIFO mode, applies only to Control IN endpoints.
 *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
 *  Indicates that the core has detected a timeout condition on the USB for the last IN
 *  token on this endpoint.
 */
#define USB_OTGFS_DIEPINT1_TIMEOUT    (BIT(3))
#define USB_OTGFS_DIEPINT1_TIMEOUT_M  (USB_OTGFS_DIEPINT1_TIMEOUT_V << USB_OTGFS_DIEPINT1_TIMEOUT_S)
#define USB_OTGFS_DIEPINT1_TIMEOUT_V  0x00000001U
#define USB_OTGFS_DIEPINT1_TIMEOUT_S  3
/** USB_OTGFS_DIEPINT1_INTKNTXFEMP : R/W; bitpos: [4]; default: 0;
 *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that an IN token was received when the associated TxFIFO
 *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
 *  which the IN token was received.
 */
#define USB_OTGFS_DIEPINT1_INTKNTXFEMP    (BIT(4))
#define USB_OTGFS_DIEPINT1_INTKNTXFEMP_M  (USB_OTGFS_DIEPINT1_INTKNTXFEMP_V << USB_OTGFS_DIEPINT1_INTKNTXFEMP_S)
#define USB_OTGFS_DIEPINT1_INTKNTXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT1_INTKNTXFEMP_S  4
/** USB_OTGFS_DIEPINT1_INTKNEPMIS : R/W; bitpos: [5]; default: 0;
 *  IN Token Received with EP Mismatch (INTknEPMis)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
 *  endpoint other than the one for which the IN token was received. This interrupt is
 *  asserted on the endpoint for which the IN token was received.
 */
#define USB_OTGFS_DIEPINT1_INTKNEPMIS    (BIT(5))
#define USB_OTGFS_DIEPINT1_INTKNEPMIS_M  (USB_OTGFS_DIEPINT1_INTKNEPMIS_V << USB_OTGFS_DIEPINT1_INTKNEPMIS_S)
#define USB_OTGFS_DIEPINT1_INTKNEPMIS_V  0x00000001U
#define USB_OTGFS_DIEPINT1_INTKNEPMIS_S  5
/** USB_OTGFS_DIEPINT1_INEPNAKEFF : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective (INEPNakEff)
 *
 *  Applies to periodic IN endpoints only.
 *
 *  This bit can be cleared when the application clears the IN endpoint NAK by writing
 *  to DIEPCTLn.CNAK.
 *
 *  This interrupt indicates that the core has sampled the NAK bit
 *
 *  Set (either by the application or by the core). The interrupt indicates that the IN
 *  endpoint NAK bit Set by the application has taken effect in the core.
 *
 *  This interrupt does not guarantee that a NAK handshake is sent on the USB. A STALL
 *  bit takes priority over a NAK bit.
 */
#define USB_OTGFS_DIEPINT1_INEPNAKEFF    (BIT(6))
#define USB_OTGFS_DIEPINT1_INEPNAKEFF_M  (USB_OTGFS_DIEPINT1_INEPNAKEFF_V << USB_OTGFS_DIEPINT1_INEPNAKEFF_S)
#define USB_OTGFS_DIEPINT1_INEPNAKEFF_V  0x00000001U
#define USB_OTGFS_DIEPINT1_INEPNAKEFF_S  6
/** USB_OTGFS_DIEPINT1_TXFEMP : RO; bitpos: [7]; default: 1;
 *  Transmit FIFO Empty (TxFEmp)
 *
 *  This bit is valid only for IN endpoints
 *
 *  This interrupt is asserted when the TxFIFO for this endpoint is
 *  either half or completely empty. The half or completely empty
 *  status is determined by the TxFIFO Empty Level bit in the Core
 *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
 */
#define USB_OTGFS_DIEPINT1_TXFEMP    (BIT(7))
#define USB_OTGFS_DIEPINT1_TXFEMP_M  (USB_OTGFS_DIEPINT1_TXFEMP_V << USB_OTGFS_DIEPINT1_TXFEMP_S)
#define USB_OTGFS_DIEPINT1_TXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT1_TXFEMP_S  7
/** USB_OTGFS_DIEPINT1_TXFIFOUNDRN : R/W; bitpos: [8]; default: 0;
 *  Fifo Underrun (TxfifoUndrn)
 *
 *  Applies to IN endpoints Only
 *
 *  This bit is valid only If thresholding is enabled. The core generates this
 *  interrupt when
 *  it detects a transmit FIFO underrun condition for this endpoint.
 */
#define USB_OTGFS_DIEPINT1_TXFIFOUNDRN    (BIT(8))
#define USB_OTGFS_DIEPINT1_TXFIFOUNDRN_M  (USB_OTGFS_DIEPINT1_TXFIFOUNDRN_V << USB_OTGFS_DIEPINT1_TXFIFOUNDRN_S)
#define USB_OTGFS_DIEPINT1_TXFIFOUNDRN_V  0x00000001U
#define USB_OTGFS_DIEPINT1_TXFIFOUNDRN_S  8
/** USB_OTGFS_DIEPINT1_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed is not ready for the
 *  Core to process, such as Host busy or DMA done.
 */
#define USB_OTGFS_DIEPINT1_BNAINTR    (BIT(9))
#define USB_OTGFS_DIEPINT1_BNAINTR_M  (USB_OTGFS_DIEPINT1_BNAINTR_V << USB_OTGFS_DIEPINT1_BNAINTR_S)
#define USB_OTGFS_DIEPINT1_BNAINTR_V  0x00000001U
#define USB_OTGFS_DIEPINT1_BNAINTR_S  9
/** USB_OTGFS_DIEPINT1_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DIEPINT1_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DIEPINT1_PKTDRPSTS_M  (USB_OTGFS_DIEPINT1_PKTDRPSTS_V << USB_OTGFS_DIEPINT1_PKTDRPSTS_S)
#define USB_OTGFS_DIEPINT1_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DIEPINT1_PKTDRPSTS_S  11
/** USB_OTGFS_DIEPINT1_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DIEPINT1_BBLEERR    (BIT(12))
#define USB_OTGFS_DIEPINT1_BBLEERR_M  (USB_OTGFS_DIEPINT1_BBLEERR_V << USB_OTGFS_DIEPINT1_BBLEERR_S)
#define USB_OTGFS_DIEPINT1_BBLEERR_V  0x00000001U
#define USB_OTGFS_DIEPINT1_BBLEERR_S  12
/** USB_OTGFS_DIEPINT1_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DIEPINT1_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DIEPINT1_NAKINTRPT_M  (USB_OTGFS_DIEPINT1_NAKINTRPT_V << USB_OTGFS_DIEPINT1_NAKINTRPT_S)
#define USB_OTGFS_DIEPINT1_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT1_NAKINTRPT_S  13
/** USB_OTGFS_DIEPINT1_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DIEPINT1_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DIEPINT1_NYETINTRPT_M  (USB_OTGFS_DIEPINT1_NYETINTRPT_V << USB_OTGFS_DIEPINT1_NYETINTRPT_S)
#define USB_OTGFS_DIEPINT1_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT1_NYETINTRPT_S  14

/** USB_OTGFS_DIEPTSIZ1_REG register
 *  Device IN Endpoint 1 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPTSIZ1_REG(i) (DR_REG_USB_BASE(i) + 0x930)
/** USB_OTGFS_DIEPTSIZ1_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet from the
 *  external memory is written to the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ1_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ1_XFERSIZE_M  (USB_OTGFS_DIEPTSIZ1_XFERSIZE_V << USB_OTGFS_DIEPTSIZ1_XFERSIZE_S)
#define USB_OTGFS_DIEPTSIZ1_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ1_XFERSIZE_S  0
/** USB_OTGFS_DIEPTSIZ1_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  Indicates the total number of USB packets that constitute the Transfer Size amount
 *  of data for endpoint 0.
 *
 *  This field is decremented every time a packet (maximum size or short packet) is
 *  read from the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ1_PKTCNT    0x0000007FU
#define USB_OTGFS_DIEPTSIZ1_PKTCNT_M  (USB_OTGFS_DIEPTSIZ1_PKTCNT_V << USB_OTGFS_DIEPTSIZ1_PKTCNT_S)
#define USB_OTGFS_DIEPTSIZ1_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DIEPTSIZ1_PKTCNT_S  19
/** USB_OTGFS_DIEPTSIZ1_MC : R/W; bitpos: [30:29]; default: 0;
 *  MC
 *
 *  Applies to IN endpoints only.
 *
 *  For periodic IN endpoints, this field indicates the number of packets that must be
 *  transmitted per microframe on the USB. The core uses this field to calculate the
 *  data PID for isochronous IN endpoints.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 *  For non-periodic IN endpoints, this field is valid only in Internal DMA mode. It
 *  specifies the number of packets the core must fetchfor an IN endpoint before it
 *  switches to the endpoint pointed to by the Next Endpoint field of the Device
 *  Endpoint-n Control register (DIEPCTLn.NextEp)
 */
#define USB_OTGFS_DIEPTSIZ1_MC    0x00000003U
#define USB_OTGFS_DIEPTSIZ1_MC_M  (USB_OTGFS_DIEPTSIZ1_MC_V << USB_OTGFS_DIEPTSIZ1_MC_S)
#define USB_OTGFS_DIEPTSIZ1_MC_V  0x00000003U
#define USB_OTGFS_DIEPTSIZ1_MC_S  29

/** USB_OTGFS_DIEPDMA1_REG register
 *  Device IN Endpoint 1 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMA1_REG(i) (DR_REG_USB_BASE(i) + 0x934)
/** USB_OTGFS_DIEPDMA1_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DIEPDMA1_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA1_DMAADDR_M  (USB_OTGFS_DIEPDMA1_DMAADDR_V << USB_OTGFS_DIEPDMA1_DMAADDR_S)
#define USB_OTGFS_DIEPDMA1_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA1_DMAADDR_S  0

/** USB_OTGFS_DTXFSTS1_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 1  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DTXFSTS1_REG(i) (DR_REG_USB_BASE(i) + 0x938)
/** USB_OTGFS_DTXFSTS1_INEPTXFSPCAVAIL : RO; bitpos: [15:0]; default: 256;
 *  IN Endpoint TxFIFO Space Avail (INEPTxFSpcAvail)
 *
 *  Indicates the amount of free space available in the Endpoint TxFIFO.
 *
 *  Values are in terms of 32-bit words.
 *  - 16'h0: Endpoint TxFIFO is full
 *  - 16'h1: 1 word available
 *  - 16'h2: 2 words available
 *  - 16'hn: n words available (where 0  n  32,768)
 *  - 16'h8000: 32,768 words available
 *  - Others: Reserved
 */
#define USB_OTGFS_DTXFSTS1_INEPTXFSPCAVAIL    0x0000FFFFU
#define USB_OTGFS_DTXFSTS1_INEPTXFSPCAVAIL_M  (USB_OTGFS_DTXFSTS1_INEPTXFSPCAVAIL_V << USB_OTGFS_DTXFSTS1_INEPTXFSPCAVAIL_S)
#define USB_OTGFS_DTXFSTS1_INEPTXFSPCAVAIL_V  0x0000FFFFU
#define USB_OTGFS_DTXFSTS1_INEPTXFSPCAVAIL_S  0

/** USB_OTGFS_DIEPDMAB1_REG register
 *  Device IN Endpoint 1 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMAB1_REG(i) (DR_REG_USB_BASE(i) + 0x93c)
/** USB_OTGFS_DIEPDMAB1_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DIEPDMAB1_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB1_DMABUFFERADDR_M  (USB_OTGFS_DIEPDMAB1_DMABUFFERADDR_V << USB_OTGFS_DIEPDMAB1_DMABUFFERADDR_S)
#define USB_OTGFS_DIEPDMAB1_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB1_DMABUFFERADDR_S  0

/** USB_OTGFS_DIEPCTL2_REG register
 *  Device Control IN Endpoint 2 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPCTL2_REG(i) (DR_REG_USB_BASE(i) + 0x940)
/** USB_OTGFS_DIEPCTL2_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the
 *  current logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DIEPCTL2_MPS    0x000007FFU
#define USB_OTGFS_DIEPCTL2_MPS_M  (USB_OTGFS_DIEPCTL2_MPS_V << USB_OTGFS_DIEPCTL2_MPS_S)
#define USB_OTGFS_DIEPCTL2_MPS_V  0x000007FFU
#define USB_OTGFS_DIEPCTL2_MPS_S  0
/** USB_OTGFS_DIEPCTL2_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DIEPCTL2_USBACTEP    (BIT(15))
#define USB_OTGFS_DIEPCTL2_USBACTEP_M  (USB_OTGFS_DIEPCTL2_USBACTEP_V << USB_OTGFS_DIEPCTL2_USBACTEP_S)
#define USB_OTGFS_DIEPCTL2_USBACTEP_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_USBACTEP_S  15
/** USB_OTGFS_DIEPCTL2_DPID : RO; bitpos: [16]; default: 0;
 *
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *
 *  Applies to isochronous IN and OUT endpoints only.
 *
 *  Indicates the (micro)frame number in which the core transmits/receives isochronous
 *  data for this endpoint. The application must program the even/odd (micro)frame
 *  number in which it intends to transmit/receive isochronous data for this endpoint
 *  using
 *  the SetEvnFr and SetOddFr fields in this register.
 *  - 1'b0: Even (micro)frame
 *  - 1'b1: Odd (micro)frame
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame in
 *  which data is received is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL2_DPID    (BIT(16))
#define USB_OTGFS_DIEPCTL2_DPID_M  (USB_OTGFS_DIEPCTL2_DPID_V << USB_OTGFS_DIEPCTL2_DPID_S)
#define USB_OTGFS_DIEPCTL2_DPID_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_DPID_S  16
/** USB_OTGFS_DIEPCTL2_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL2_NAKSTS    (BIT(17))
#define USB_OTGFS_DIEPCTL2_NAKSTS_M  (USB_OTGFS_DIEPCTL2_NAKSTS_V << USB_OTGFS_DIEPCTL2_NAKSTS_S)
#define USB_OTGFS_DIEPCTL2_NAKSTS_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_NAKSTS_S  17
/** USB_OTGFS_DIEPCTL2_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DIEPCTL2_EPTYPE    0x00000003U
#define USB_OTGFS_DIEPCTL2_EPTYPE_M  (USB_OTGFS_DIEPCTL2_EPTYPE_V << USB_OTGFS_DIEPCTL2_EPTYPE_S)
#define USB_OTGFS_DIEPCTL2_EPTYPE_V  0x00000003U
#define USB_OTGFS_DIEPCTL2_EPTYPE_S  18
/** USB_OTGFS_DIEPCTL2_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL2_STALL    (BIT(21))
#define USB_OTGFS_DIEPCTL2_STALL_M  (USB_OTGFS_DIEPCTL2_STALL_V << USB_OTGFS_DIEPCTL2_STALL_S)
#define USB_OTGFS_DIEPCTL2_STALL_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_STALL_S  21
/** USB_OTGFS_DIEPCTL2_TXFNUM : R/W; bitpos: [25:22]; default: 0;
 *  TxFIFO Number (TxFNum)
 *
 *  Shared FIFO Operation non-periodic endpoints must set this bit to zero. Periodic
 *  endpoints must map this to the corresponding Periodic TxFIFO number.
 *  - 4'h0: Non-Periodic TxFIFO
 *  - Others: Specified Periodic TxFIFO.number
 *  Note: An interrupt IN endpoint can be configured as a non-periodic endpoint for
 *  applications such as mass storage. The core treats an IN endpoint as a non-periodic
 *  endpoint if the TxFNum field is set to 0. Otherwise, a separate periodic FIFO must
 *  be
 *  allocated for an interrupt IN endpoint, and the number of this
 *  FIFO must be programmed into the TxFNum field. Configuring an interrupt IN
 *  endpoint as a non-periodic endpoint saves the extra periodic FIFO area.
 *
 *  Dedicated FIFO Operation: These bits specify the FIFO number associated with this
 *  endpoint. Each active IN endpoint must be programmed to a separate FIFO number.
 *  This field is valid only for IN endpoints.
 */
#define USB_OTGFS_DIEPCTL2_TXFNUM    0x0000000FU
#define USB_OTGFS_DIEPCTL2_TXFNUM_M  (USB_OTGFS_DIEPCTL2_TXFNUM_V << USB_OTGFS_DIEPCTL2_TXFNUM_S)
#define USB_OTGFS_DIEPCTL2_TXFNUM_V  0x0000000FU
#define USB_OTGFS_DIEPCTL2_TXFNUM_S  22
/** USB_OTGFS_DIEPCTL2_CNAK : R/W; bitpos: [26]; default: 0;
 *  Clear NAK (CNAK)
 *
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DIEPCTL2_CNAK    (BIT(26))
#define USB_OTGFS_DIEPCTL2_CNAK_M  (USB_OTGFS_DIEPCTL2_CNAK_V << USB_OTGFS_DIEPCTL2_CNAK_S)
#define USB_OTGFS_DIEPCTL2_CNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_CNAK_S  26
/** USB_OTGFS_DIEPCTL2_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK handshakes on
 *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
 *  received on that endpoint.
 */
#define USB_OTGFS_DIEPCTL2_SNAK    (BIT(27))
#define USB_OTGFS_DIEPCTL2_SNAK_M  (USB_OTGFS_DIEPCTL2_SNAK_V << USB_OTGFS_DIEPCTL2_SNAK_S)
#define USB_OTGFS_DIEPCTL2_SNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_SNAK_S  27
/** USB_OTGFS_DIEPCTL2_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  SetD0PID
 *  - Set DATA0 PID (SetD0PID)
 *  -- Applies to interrupt/bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA0.
 *  -- This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  In non-Scatter/Gather DMA mode: Set Even (micro)Frame (SetEvenFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the Even/Odd (micro)Frame (EO_FrNum) field to even
 *  (micro)Frame.
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which to
 *  receive data is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL2_SETD0PID    (BIT(28))
#define USB_OTGFS_DIEPCTL2_SETD0PID_M  (USB_OTGFS_DIEPCTL2_SETD0PID_V << USB_OTGFS_DIEPCTL2_SETD0PID_S)
#define USB_OTGFS_DIEPCTL2_SETD0PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_SETD0PID_S  28
/** USB_OTGFS_DIEPCTL2_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  SetD1PID
 *  - Set DATA1 PID (SetD1PID)
 *  -- Applies to interrupt and bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA1.
 *  -- This field is applicable both for Scatter-Gather DMA mode and non Scatter-Gather
 *  DMA mode.
 *  - Set odd (micro)Frame (SetOddFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the even and odd (micro)Frame (EO_FrNum) field to odd
 *  (micro)Frame.
 *  -- This field is not applicable for Scatter-Gather DMA mode.
 */
#define USB_OTGFS_DIEPCTL2_SETD1PID    (BIT(29))
#define USB_OTGFS_DIEPCTL2_SETD1PID_M  (USB_OTGFS_DIEPCTL2_SETD1PID_V << USB_OTGFS_DIEPCTL2_SETD1PID_S)
#define USB_OTGFS_DIEPCTL2_SETD1PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_SETD1PID_S  29
/** USB_OTGFS_DIEPCTL2_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DIEPCTL2_EPDIS    (BIT(30))
#define USB_OTGFS_DIEPCTL2_EPDIS_M  (USB_OTGFS_DIEPCTL2_EPDIS_V << USB_OTGFS_DIEPCTL2_EPDIS_S)
#define USB_OTGFS_DIEPCTL2_EPDIS_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_EPDIS_S  30
/** USB_OTGFS_DIEPCTL2_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled,
 *  -- For IN endpoints this bit indicates that the descriptor structure and data
 *  buffer with data ready to transmit is setup.
 *  -- For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  - When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  -- For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  -- For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  - The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  -- SETUP Phase Done
 *  -- Endpoint Disabled
 *  -- Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DIEPCTL2_EPENA    (BIT(31))
#define USB_OTGFS_DIEPCTL2_EPENA_M  (USB_OTGFS_DIEPCTL2_EPENA_V << USB_OTGFS_DIEPCTL2_EPENA_S)
#define USB_OTGFS_DIEPCTL2_EPENA_V  0x00000001U
#define USB_OTGFS_DIEPCTL2_EPENA_S  31

/** USB_OTGFS_DIEPINT2_REG register
 *  Device IN Endpoint 2 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPINT2_REG(i) (DR_REG_USB_BASE(i) + 0x948)
/** USB_OTGFS_DIEPINT2_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DIEPINT2_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DIEPINT2_XFERCOMPL_M  (USB_OTGFS_DIEPINT2_XFERCOMPL_V << USB_OTGFS_DIEPINT2_XFERCOMPL_S)
#define USB_OTGFS_DIEPINT2_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DIEPINT2_XFERCOMPL_S  0
/** USB_OTGFS_DIEPINT2_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DIEPINT2_EPDISBLD    (BIT(1))
#define USB_OTGFS_DIEPINT2_EPDISBLD_M  (USB_OTGFS_DIEPINT2_EPDISBLD_V << USB_OTGFS_DIEPINT2_EPDISBLD_S)
#define USB_OTGFS_DIEPINT2_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DIEPINT2_EPDISBLD_S  1
/** USB_OTGFS_DIEPINT2_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DIEPINT2_AHBERR    (BIT(2))
#define USB_OTGFS_DIEPINT2_AHBERR_M  (USB_OTGFS_DIEPINT2_AHBERR_V << USB_OTGFS_DIEPINT2_AHBERR_S)
#define USB_OTGFS_DIEPINT2_AHBERR_V  0x00000001U
#define USB_OTGFS_DIEPINT2_AHBERR_S  2
/** USB_OTGFS_DIEPINT2_TIMEOUT : R/W; bitpos: [3]; default: 0;
 *  Timeout Condition (TimeOUT)
 *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
 *  - In dedicated FIFO mode, applies only to Control IN endpoints.
 *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
 *  Indicates that the core has detected a timeout condition on the USB for the last IN
 *  token on this endpoint.
 */
#define USB_OTGFS_DIEPINT2_TIMEOUT    (BIT(3))
#define USB_OTGFS_DIEPINT2_TIMEOUT_M  (USB_OTGFS_DIEPINT2_TIMEOUT_V << USB_OTGFS_DIEPINT2_TIMEOUT_S)
#define USB_OTGFS_DIEPINT2_TIMEOUT_V  0x00000001U
#define USB_OTGFS_DIEPINT2_TIMEOUT_S  3
/** USB_OTGFS_DIEPINT2_INTKNTXFEMP : R/W; bitpos: [4]; default: 0;
 *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that an IN token was received when the associated TxFIFO
 *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
 *  which the IN token was received.
 */
#define USB_OTGFS_DIEPINT2_INTKNTXFEMP    (BIT(4))
#define USB_OTGFS_DIEPINT2_INTKNTXFEMP_M  (USB_OTGFS_DIEPINT2_INTKNTXFEMP_V << USB_OTGFS_DIEPINT2_INTKNTXFEMP_S)
#define USB_OTGFS_DIEPINT2_INTKNTXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT2_INTKNTXFEMP_S  4
/** USB_OTGFS_DIEPINT2_INTKNEPMIS : R/W; bitpos: [5]; default: 0;
 *  IN Token Received with EP Mismatch (INTknEPMis)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
 *  endpoint other than the one for which the IN token was received. This interrupt is
 *  asserted on the endpoint for which the IN token was received.
 */
#define USB_OTGFS_DIEPINT2_INTKNEPMIS    (BIT(5))
#define USB_OTGFS_DIEPINT2_INTKNEPMIS_M  (USB_OTGFS_DIEPINT2_INTKNEPMIS_V << USB_OTGFS_DIEPINT2_INTKNEPMIS_S)
#define USB_OTGFS_DIEPINT2_INTKNEPMIS_V  0x00000001U
#define USB_OTGFS_DIEPINT2_INTKNEPMIS_S  5
/** USB_OTGFS_DIEPINT2_INEPNAKEFF : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective (INEPNakEff)
 *
 *  Applies to periodic IN endpoints only.
 *
 *  This bit can be cleared when the application clears the IN endpoint NAK by writing
 *  to DIEPCTLn.CNAK.
 *
 *  This interrupt indicates that the core has sampled the NAK bit
 *
 *  Set (either by the application or by the core). The interrupt indicates that the IN
 *  endpoint NAK bit Set by the application has taken effect in the core.
 *
 *  This interrupt does not guarantee that a NAK handshake is sent on the USB. A STALL
 *  bit takes priority over a NAK bit.
 */
#define USB_OTGFS_DIEPINT2_INEPNAKEFF    (BIT(6))
#define USB_OTGFS_DIEPINT2_INEPNAKEFF_M  (USB_OTGFS_DIEPINT2_INEPNAKEFF_V << USB_OTGFS_DIEPINT2_INEPNAKEFF_S)
#define USB_OTGFS_DIEPINT2_INEPNAKEFF_V  0x00000001U
#define USB_OTGFS_DIEPINT2_INEPNAKEFF_S  6
/** USB_OTGFS_DIEPINT2_TXFEMP : RO; bitpos: [7]; default: 1;
 *  Transmit FIFO Empty (TxFEmp)
 *
 *  This bit is valid only for IN endpoints
 *
 *  This interrupt is asserted when the TxFIFO for this endpoint is
 *  either half or completely empty. The half or completely empty
 *  status is determined by the TxFIFO Empty Level bit in the Core
 *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
 */
#define USB_OTGFS_DIEPINT2_TXFEMP    (BIT(7))
#define USB_OTGFS_DIEPINT2_TXFEMP_M  (USB_OTGFS_DIEPINT2_TXFEMP_V << USB_OTGFS_DIEPINT2_TXFEMP_S)
#define USB_OTGFS_DIEPINT2_TXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT2_TXFEMP_S  7
/** USB_OTGFS_DIEPINT2_TXFIFOUNDRN : R/W; bitpos: [8]; default: 0;
 *  Fifo Underrun (TxfifoUndrn)
 *
 *  Applies to IN endpoints Only
 *
 *  This bit is valid only If thresholding is enabled. The core generates this
 *  interrupt when
 *  it detects a transmit FIFO underrun condition for this endpoint.
 */
#define USB_OTGFS_DIEPINT2_TXFIFOUNDRN    (BIT(8))
#define USB_OTGFS_DIEPINT2_TXFIFOUNDRN_M  (USB_OTGFS_DIEPINT2_TXFIFOUNDRN_V << USB_OTGFS_DIEPINT2_TXFIFOUNDRN_S)
#define USB_OTGFS_DIEPINT2_TXFIFOUNDRN_V  0x00000001U
#define USB_OTGFS_DIEPINT2_TXFIFOUNDRN_S  8
/** USB_OTGFS_DIEPINT2_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed is not ready for the
 *  Core to process, such as Host busy or DMA done.
 */
#define USB_OTGFS_DIEPINT2_BNAINTR    (BIT(9))
#define USB_OTGFS_DIEPINT2_BNAINTR_M  (USB_OTGFS_DIEPINT2_BNAINTR_V << USB_OTGFS_DIEPINT2_BNAINTR_S)
#define USB_OTGFS_DIEPINT2_BNAINTR_V  0x00000001U
#define USB_OTGFS_DIEPINT2_BNAINTR_S  9
/** USB_OTGFS_DIEPINT2_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DIEPINT2_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DIEPINT2_PKTDRPSTS_M  (USB_OTGFS_DIEPINT2_PKTDRPSTS_V << USB_OTGFS_DIEPINT2_PKTDRPSTS_S)
#define USB_OTGFS_DIEPINT2_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DIEPINT2_PKTDRPSTS_S  11
/** USB_OTGFS_DIEPINT2_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DIEPINT2_BBLEERR    (BIT(12))
#define USB_OTGFS_DIEPINT2_BBLEERR_M  (USB_OTGFS_DIEPINT2_BBLEERR_V << USB_OTGFS_DIEPINT2_BBLEERR_S)
#define USB_OTGFS_DIEPINT2_BBLEERR_V  0x00000001U
#define USB_OTGFS_DIEPINT2_BBLEERR_S  12
/** USB_OTGFS_DIEPINT2_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DIEPINT2_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DIEPINT2_NAKINTRPT_M  (USB_OTGFS_DIEPINT2_NAKINTRPT_V << USB_OTGFS_DIEPINT2_NAKINTRPT_S)
#define USB_OTGFS_DIEPINT2_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT2_NAKINTRPT_S  13
/** USB_OTGFS_DIEPINT2_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DIEPINT2_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DIEPINT2_NYETINTRPT_M  (USB_OTGFS_DIEPINT2_NYETINTRPT_V << USB_OTGFS_DIEPINT2_NYETINTRPT_S)
#define USB_OTGFS_DIEPINT2_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT2_NYETINTRPT_S  14

/** USB_OTGFS_DIEPTSIZ2_REG register
 *  Device IN Endpoint 2 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPTSIZ2_REG(i) (DR_REG_USB_BASE(i) + 0x950)
/** USB_OTGFS_DIEPTSIZ2_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet from the
 *  external memory is written to the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ2_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ2_XFERSIZE_M  (USB_OTGFS_DIEPTSIZ2_XFERSIZE_V << USB_OTGFS_DIEPTSIZ2_XFERSIZE_S)
#define USB_OTGFS_DIEPTSIZ2_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ2_XFERSIZE_S  0
/** USB_OTGFS_DIEPTSIZ2_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  Indicates the total number of USB packets that constitute the Transfer Size amount
 *  of data for endpoint 0.
 *
 *  This field is decremented every time a packet (maximum size or short packet) is
 *  read from the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ2_PKTCNT    0x0000007FU
#define USB_OTGFS_DIEPTSIZ2_PKTCNT_M  (USB_OTGFS_DIEPTSIZ2_PKTCNT_V << USB_OTGFS_DIEPTSIZ2_PKTCNT_S)
#define USB_OTGFS_DIEPTSIZ2_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DIEPTSIZ2_PKTCNT_S  19
/** USB_OTGFS_DIEPTSIZ2_MC : R/W; bitpos: [30:29]; default: 0;
 *  MC
 *
 *  Applies to IN endpoints only.
 *
 *  For periodic IN endpoints, this field indicates the number of packets that must be
 *  transmitted per microframe on the USB. The core uses this field to calculate the
 *  data PID for isochronous IN endpoints.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 *  For non-periodic IN endpoints, this field is valid only in Internal DMA mode. It
 *  specifies the number of packets the core must fetchfor an IN endpoint before it
 *  switches to the endpoint pointed to by the Next Endpoint field of the Device
 *  Endpoint-n Control register (DIEPCTLn.NextEp)
 */
#define USB_OTGFS_DIEPTSIZ2_MC    0x00000003U
#define USB_OTGFS_DIEPTSIZ2_MC_M  (USB_OTGFS_DIEPTSIZ2_MC_V << USB_OTGFS_DIEPTSIZ2_MC_S)
#define USB_OTGFS_DIEPTSIZ2_MC_V  0x00000003U
#define USB_OTGFS_DIEPTSIZ2_MC_S  29

/** USB_OTGFS_DIEPDMA2_REG register
 *  Device IN Endpoint 2 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMA2_REG(i) (DR_REG_USB_BASE(i) + 0x954)
/** USB_OTGFS_DIEPDMA2_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DIEPDMA2_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA2_DMAADDR_M  (USB_OTGFS_DIEPDMA2_DMAADDR_V << USB_OTGFS_DIEPDMA2_DMAADDR_S)
#define USB_OTGFS_DIEPDMA2_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA2_DMAADDR_S  0

/** USB_OTGFS_DTXFSTS2_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 2  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DTXFSTS2_REG(i) (DR_REG_USB_BASE(i) + 0x958)
/** USB_OTGFS_DTXFSTS2_INEPTXFSPCAVAIL : RO; bitpos: [15:0]; default: 256;
 *  IN Endpoint TxFIFO Space Avail (INEPTxFSpcAvail)
 *
 *  Indicates the amount of free space available in the Endpoint TxFIFO.
 *
 *  Values are in terms of 32-bit words.
 *  - 16'h0: Endpoint TxFIFO is full
 *  - 16'h1: 1 word available
 *  - 16'h2: 2 words available
 *  - 16'hn: n words available (where 0  n  32,768)
 *  - 16'h8000: 32,768 words available
 *  - Others: Reserved
 */
#define USB_OTGFS_DTXFSTS2_INEPTXFSPCAVAIL    0x0000FFFFU
#define USB_OTGFS_DTXFSTS2_INEPTXFSPCAVAIL_M  (USB_OTGFS_DTXFSTS2_INEPTXFSPCAVAIL_V << USB_OTGFS_DTXFSTS2_INEPTXFSPCAVAIL_S)
#define USB_OTGFS_DTXFSTS2_INEPTXFSPCAVAIL_V  0x0000FFFFU
#define USB_OTGFS_DTXFSTS2_INEPTXFSPCAVAIL_S  0

/** USB_OTGFS_DIEPDMAB2_REG register
 *  Device IN Endpoint 2 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMAB2_REG(i) (DR_REG_USB_BASE(i) + 0x95c)
/** USB_OTGFS_DIEPDMAB2_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DIEPDMAB2_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB2_DMABUFFERADDR_M  (USB_OTGFS_DIEPDMAB2_DMABUFFERADDR_V << USB_OTGFS_DIEPDMAB2_DMABUFFERADDR_S)
#define USB_OTGFS_DIEPDMAB2_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB2_DMABUFFERADDR_S  0

/** USB_OTGFS_DIEPCTL3_REG register
 *  Device Control IN Endpoint 3 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPCTL3_REG(i) (DR_REG_USB_BASE(i) + 0x960)
/** USB_OTGFS_DIEPCTL3_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the
 *  current logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DIEPCTL3_MPS    0x000007FFU
#define USB_OTGFS_DIEPCTL3_MPS_M  (USB_OTGFS_DIEPCTL3_MPS_V << USB_OTGFS_DIEPCTL3_MPS_S)
#define USB_OTGFS_DIEPCTL3_MPS_V  0x000007FFU
#define USB_OTGFS_DIEPCTL3_MPS_S  0
/** USB_OTGFS_DIEPCTL3_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DIEPCTL3_USBACTEP    (BIT(15))
#define USB_OTGFS_DIEPCTL3_USBACTEP_M  (USB_OTGFS_DIEPCTL3_USBACTEP_V << USB_OTGFS_DIEPCTL3_USBACTEP_S)
#define USB_OTGFS_DIEPCTL3_USBACTEP_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_USBACTEP_S  15
/** USB_OTGFS_DIEPCTL3_DPID : RO; bitpos: [16]; default: 0;
 *
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *
 *  Applies to isochronous IN and OUT endpoints only.
 *
 *  Indicates the (micro)frame number in which the core transmits/receives isochronous
 *  data for this endpoint. The application must program the even/odd (micro)frame
 *  number in which it intends to transmit/receive isochronous data for this endpoint
 *  using
 *  the SetEvnFr and SetOddFr fields in this register.
 *  - 1'b0: Even (micro)frame
 *  - 1'b1: Odd (micro)frame
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame in
 *  which data is received is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL3_DPID    (BIT(16))
#define USB_OTGFS_DIEPCTL3_DPID_M  (USB_OTGFS_DIEPCTL3_DPID_V << USB_OTGFS_DIEPCTL3_DPID_S)
#define USB_OTGFS_DIEPCTL3_DPID_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_DPID_S  16
/** USB_OTGFS_DIEPCTL3_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL3_NAKSTS    (BIT(17))
#define USB_OTGFS_DIEPCTL3_NAKSTS_M  (USB_OTGFS_DIEPCTL3_NAKSTS_V << USB_OTGFS_DIEPCTL3_NAKSTS_S)
#define USB_OTGFS_DIEPCTL3_NAKSTS_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_NAKSTS_S  17
/** USB_OTGFS_DIEPCTL3_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DIEPCTL3_EPTYPE    0x00000003U
#define USB_OTGFS_DIEPCTL3_EPTYPE_M  (USB_OTGFS_DIEPCTL3_EPTYPE_V << USB_OTGFS_DIEPCTL3_EPTYPE_S)
#define USB_OTGFS_DIEPCTL3_EPTYPE_V  0x00000003U
#define USB_OTGFS_DIEPCTL3_EPTYPE_S  18
/** USB_OTGFS_DIEPCTL3_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL3_STALL    (BIT(21))
#define USB_OTGFS_DIEPCTL3_STALL_M  (USB_OTGFS_DIEPCTL3_STALL_V << USB_OTGFS_DIEPCTL3_STALL_S)
#define USB_OTGFS_DIEPCTL3_STALL_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_STALL_S  21
/** USB_OTGFS_DIEPCTL3_TXFNUM : R/W; bitpos: [25:22]; default: 0;
 *  TxFIFO Number (TxFNum)
 *
 *  Shared FIFO Operation non-periodic endpoints must set this bit to zero. Periodic
 *  endpoints must map this to the corresponding Periodic TxFIFO number.
 *  - 4'h0: Non-Periodic TxFIFO
 *  - Others: Specified Periodic TxFIFO.number
 *  Note: An interrupt IN endpoint can be configured as a non-periodic endpoint for
 *  applications such as mass storage. The core treats an IN endpoint as a non-periodic
 *  endpoint if the TxFNum field is set to 0. Otherwise, a separate periodic FIFO must
 *  be
 *  allocated for an interrupt IN endpoint, and the number of this
 *  FIFO must be programmed into the TxFNum field. Configuring an interrupt IN
 *  endpoint as a non-periodic endpoint saves the extra periodic FIFO area.
 *
 *  Dedicated FIFO Operation: These bits specify the FIFO number associated with this
 *  endpoint. Each active IN endpoint must be programmed to a separate FIFO number.
 *  This field is valid only for IN endpoints.
 */
#define USB_OTGFS_DIEPCTL3_TXFNUM    0x0000000FU
#define USB_OTGFS_DIEPCTL3_TXFNUM_M  (USB_OTGFS_DIEPCTL3_TXFNUM_V << USB_OTGFS_DIEPCTL3_TXFNUM_S)
#define USB_OTGFS_DIEPCTL3_TXFNUM_V  0x0000000FU
#define USB_OTGFS_DIEPCTL3_TXFNUM_S  22
/** USB_OTGFS_DIEPCTL3_CNAK : R/W; bitpos: [26]; default: 0;
 *  Clear NAK (CNAK)
 *
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DIEPCTL3_CNAK    (BIT(26))
#define USB_OTGFS_DIEPCTL3_CNAK_M  (USB_OTGFS_DIEPCTL3_CNAK_V << USB_OTGFS_DIEPCTL3_CNAK_S)
#define USB_OTGFS_DIEPCTL3_CNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_CNAK_S  26
/** USB_OTGFS_DIEPCTL3_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK handshakes on
 *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
 *  received on that endpoint.
 */
#define USB_OTGFS_DIEPCTL3_SNAK    (BIT(27))
#define USB_OTGFS_DIEPCTL3_SNAK_M  (USB_OTGFS_DIEPCTL3_SNAK_V << USB_OTGFS_DIEPCTL3_SNAK_S)
#define USB_OTGFS_DIEPCTL3_SNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_SNAK_S  27
/** USB_OTGFS_DIEPCTL3_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  SetD0PID
 *  - Set DATA0 PID (SetD0PID)
 *  -- Applies to interrupt/bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA0.
 *  -- This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  In non-Scatter/Gather DMA mode: Set Even (micro)Frame (SetEvenFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the Even/Odd (micro)Frame (EO_FrNum) field to even
 *  (micro)Frame.
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which to
 *  receive data is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL3_SETD0PID    (BIT(28))
#define USB_OTGFS_DIEPCTL3_SETD0PID_M  (USB_OTGFS_DIEPCTL3_SETD0PID_V << USB_OTGFS_DIEPCTL3_SETD0PID_S)
#define USB_OTGFS_DIEPCTL3_SETD0PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_SETD0PID_S  28
/** USB_OTGFS_DIEPCTL3_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  SetD1PID
 *  - Set DATA1 PID (SetD1PID)
 *  -- Applies to interrupt and bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA1.
 *  -- This field is applicable both for Scatter-Gather DMA mode and non Scatter-Gather
 *  DMA mode.
 *  - Set odd (micro)Frame (SetOddFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the even and odd (micro)Frame (EO_FrNum) field to odd
 *  (micro)Frame.
 *  -- This field is not applicable for Scatter-Gather DMA mode.
 */
#define USB_OTGFS_DIEPCTL3_SETD1PID    (BIT(29))
#define USB_OTGFS_DIEPCTL3_SETD1PID_M  (USB_OTGFS_DIEPCTL3_SETD1PID_V << USB_OTGFS_DIEPCTL3_SETD1PID_S)
#define USB_OTGFS_DIEPCTL3_SETD1PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_SETD1PID_S  29
/** USB_OTGFS_DIEPCTL3_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DIEPCTL3_EPDIS    (BIT(30))
#define USB_OTGFS_DIEPCTL3_EPDIS_M  (USB_OTGFS_DIEPCTL3_EPDIS_V << USB_OTGFS_DIEPCTL3_EPDIS_S)
#define USB_OTGFS_DIEPCTL3_EPDIS_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_EPDIS_S  30
/** USB_OTGFS_DIEPCTL3_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled,
 *  -- For IN endpoints this bit indicates that the descriptor structure and data
 *  buffer with data ready to transmit is setup.
 *  -- For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  - When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  -- For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  -- For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  - The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  -- SETUP Phase Done
 *  -- Endpoint Disabled
 *  -- Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DIEPCTL3_EPENA    (BIT(31))
#define USB_OTGFS_DIEPCTL3_EPENA_M  (USB_OTGFS_DIEPCTL3_EPENA_V << USB_OTGFS_DIEPCTL3_EPENA_S)
#define USB_OTGFS_DIEPCTL3_EPENA_V  0x00000001U
#define USB_OTGFS_DIEPCTL3_EPENA_S  31

/** USB_OTGFS_DIEPINT3_REG register
 *  Device IN Endpoint 3 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPINT3_REG(i) (DR_REG_USB_BASE(i) + 0x968)
/** USB_OTGFS_DIEPINT3_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DIEPINT3_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DIEPINT3_XFERCOMPL_M  (USB_OTGFS_DIEPINT3_XFERCOMPL_V << USB_OTGFS_DIEPINT3_XFERCOMPL_S)
#define USB_OTGFS_DIEPINT3_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DIEPINT3_XFERCOMPL_S  0
/** USB_OTGFS_DIEPINT3_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DIEPINT3_EPDISBLD    (BIT(1))
#define USB_OTGFS_DIEPINT3_EPDISBLD_M  (USB_OTGFS_DIEPINT3_EPDISBLD_V << USB_OTGFS_DIEPINT3_EPDISBLD_S)
#define USB_OTGFS_DIEPINT3_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DIEPINT3_EPDISBLD_S  1
/** USB_OTGFS_DIEPINT3_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DIEPINT3_AHBERR    (BIT(2))
#define USB_OTGFS_DIEPINT3_AHBERR_M  (USB_OTGFS_DIEPINT3_AHBERR_V << USB_OTGFS_DIEPINT3_AHBERR_S)
#define USB_OTGFS_DIEPINT3_AHBERR_V  0x00000001U
#define USB_OTGFS_DIEPINT3_AHBERR_S  2
/** USB_OTGFS_DIEPINT3_TIMEOUT : R/W; bitpos: [3]; default: 0;
 *  Timeout Condition (TimeOUT)
 *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
 *  - In dedicated FIFO mode, applies only to Control IN endpoints.
 *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
 *  Indicates that the core has detected a timeout condition on the USB for the last IN
 *  token on this endpoint.
 */
#define USB_OTGFS_DIEPINT3_TIMEOUT    (BIT(3))
#define USB_OTGFS_DIEPINT3_TIMEOUT_M  (USB_OTGFS_DIEPINT3_TIMEOUT_V << USB_OTGFS_DIEPINT3_TIMEOUT_S)
#define USB_OTGFS_DIEPINT3_TIMEOUT_V  0x00000001U
#define USB_OTGFS_DIEPINT3_TIMEOUT_S  3
/** USB_OTGFS_DIEPINT3_INTKNTXFEMP : R/W; bitpos: [4]; default: 0;
 *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that an IN token was received when the associated TxFIFO
 *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
 *  which the IN token was received.
 */
#define USB_OTGFS_DIEPINT3_INTKNTXFEMP    (BIT(4))
#define USB_OTGFS_DIEPINT3_INTKNTXFEMP_M  (USB_OTGFS_DIEPINT3_INTKNTXFEMP_V << USB_OTGFS_DIEPINT3_INTKNTXFEMP_S)
#define USB_OTGFS_DIEPINT3_INTKNTXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT3_INTKNTXFEMP_S  4
/** USB_OTGFS_DIEPINT3_INTKNEPMIS : R/W; bitpos: [5]; default: 0;
 *  IN Token Received with EP Mismatch (INTknEPMis)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
 *  endpoint other than the one for which the IN token was received. This interrupt is
 *  asserted on the endpoint for which the IN token was received.
 */
#define USB_OTGFS_DIEPINT3_INTKNEPMIS    (BIT(5))
#define USB_OTGFS_DIEPINT3_INTKNEPMIS_M  (USB_OTGFS_DIEPINT3_INTKNEPMIS_V << USB_OTGFS_DIEPINT3_INTKNEPMIS_S)
#define USB_OTGFS_DIEPINT3_INTKNEPMIS_V  0x00000001U
#define USB_OTGFS_DIEPINT3_INTKNEPMIS_S  5
/** USB_OTGFS_DIEPINT3_INEPNAKEFF : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective (INEPNakEff)
 *
 *  Applies to periodic IN endpoints only.
 *
 *  This bit can be cleared when the application clears the IN endpoint NAK by writing
 *  to DIEPCTLn.CNAK.
 *
 *  This interrupt indicates that the core has sampled the NAK bit
 *
 *  Set (either by the application or by the core). The interrupt indicates that the IN
 *  endpoint NAK bit Set by the application has taken effect in the core.
 *
 *  This interrupt does not guarantee that a NAK handshake is sent on the USB. A STALL
 *  bit takes priority over a NAK bit.
 */
#define USB_OTGFS_DIEPINT3_INEPNAKEFF    (BIT(6))
#define USB_OTGFS_DIEPINT3_INEPNAKEFF_M  (USB_OTGFS_DIEPINT3_INEPNAKEFF_V << USB_OTGFS_DIEPINT3_INEPNAKEFF_S)
#define USB_OTGFS_DIEPINT3_INEPNAKEFF_V  0x00000001U
#define USB_OTGFS_DIEPINT3_INEPNAKEFF_S  6
/** USB_OTGFS_DIEPINT3_TXFEMP : RO; bitpos: [7]; default: 1;
 *  Transmit FIFO Empty (TxFEmp)
 *
 *  This bit is valid only for IN endpoints
 *
 *  This interrupt is asserted when the TxFIFO for this endpoint is
 *  either half or completely empty. The half or completely empty
 *  status is determined by the TxFIFO Empty Level bit in the Core
 *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
 */
#define USB_OTGFS_DIEPINT3_TXFEMP    (BIT(7))
#define USB_OTGFS_DIEPINT3_TXFEMP_M  (USB_OTGFS_DIEPINT3_TXFEMP_V << USB_OTGFS_DIEPINT3_TXFEMP_S)
#define USB_OTGFS_DIEPINT3_TXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT3_TXFEMP_S  7
/** USB_OTGFS_DIEPINT3_TXFIFOUNDRN : R/W; bitpos: [8]; default: 0;
 *  Fifo Underrun (TxfifoUndrn)
 *
 *  Applies to IN endpoints Only
 *
 *  This bit is valid only If thresholding is enabled. The core generates this
 *  interrupt when
 *  it detects a transmit FIFO underrun condition for this endpoint.
 */
#define USB_OTGFS_DIEPINT3_TXFIFOUNDRN    (BIT(8))
#define USB_OTGFS_DIEPINT3_TXFIFOUNDRN_M  (USB_OTGFS_DIEPINT3_TXFIFOUNDRN_V << USB_OTGFS_DIEPINT3_TXFIFOUNDRN_S)
#define USB_OTGFS_DIEPINT3_TXFIFOUNDRN_V  0x00000001U
#define USB_OTGFS_DIEPINT3_TXFIFOUNDRN_S  8
/** USB_OTGFS_DIEPINT3_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed is not ready for the
 *  Core to process, such as Host busy or DMA done.
 */
#define USB_OTGFS_DIEPINT3_BNAINTR    (BIT(9))
#define USB_OTGFS_DIEPINT3_BNAINTR_M  (USB_OTGFS_DIEPINT3_BNAINTR_V << USB_OTGFS_DIEPINT3_BNAINTR_S)
#define USB_OTGFS_DIEPINT3_BNAINTR_V  0x00000001U
#define USB_OTGFS_DIEPINT3_BNAINTR_S  9
/** USB_OTGFS_DIEPINT3_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DIEPINT3_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DIEPINT3_PKTDRPSTS_M  (USB_OTGFS_DIEPINT3_PKTDRPSTS_V << USB_OTGFS_DIEPINT3_PKTDRPSTS_S)
#define USB_OTGFS_DIEPINT3_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DIEPINT3_PKTDRPSTS_S  11
/** USB_OTGFS_DIEPINT3_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DIEPINT3_BBLEERR    (BIT(12))
#define USB_OTGFS_DIEPINT3_BBLEERR_M  (USB_OTGFS_DIEPINT3_BBLEERR_V << USB_OTGFS_DIEPINT3_BBLEERR_S)
#define USB_OTGFS_DIEPINT3_BBLEERR_V  0x00000001U
#define USB_OTGFS_DIEPINT3_BBLEERR_S  12
/** USB_OTGFS_DIEPINT3_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DIEPINT3_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DIEPINT3_NAKINTRPT_M  (USB_OTGFS_DIEPINT3_NAKINTRPT_V << USB_OTGFS_DIEPINT3_NAKINTRPT_S)
#define USB_OTGFS_DIEPINT3_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT3_NAKINTRPT_S  13
/** USB_OTGFS_DIEPINT3_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DIEPINT3_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DIEPINT3_NYETINTRPT_M  (USB_OTGFS_DIEPINT3_NYETINTRPT_V << USB_OTGFS_DIEPINT3_NYETINTRPT_S)
#define USB_OTGFS_DIEPINT3_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT3_NYETINTRPT_S  14

/** USB_OTGFS_DIEPTSIZ3_REG register
 *  Device IN Endpoint 3 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPTSIZ3_REG(i) (DR_REG_USB_BASE(i) + 0x970)
/** USB_OTGFS_DIEPTSIZ3_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet from the
 *  external memory is written to the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ3_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ3_XFERSIZE_M  (USB_OTGFS_DIEPTSIZ3_XFERSIZE_V << USB_OTGFS_DIEPTSIZ3_XFERSIZE_S)
#define USB_OTGFS_DIEPTSIZ3_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ3_XFERSIZE_S  0
/** USB_OTGFS_DIEPTSIZ3_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  Indicates the total number of USB packets that constitute the Transfer Size amount
 *  of data for endpoint 0.
 *
 *  This field is decremented every time a packet (maximum size or short packet) is
 *  read from the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ3_PKTCNT    0x0000007FU
#define USB_OTGFS_DIEPTSIZ3_PKTCNT_M  (USB_OTGFS_DIEPTSIZ3_PKTCNT_V << USB_OTGFS_DIEPTSIZ3_PKTCNT_S)
#define USB_OTGFS_DIEPTSIZ3_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DIEPTSIZ3_PKTCNT_S  19
/** USB_OTGFS_DIEPTSIZ3_MC : R/W; bitpos: [30:29]; default: 0;
 *  MC
 *
 *  Applies to IN endpoints only.
 *
 *  For periodic IN endpoints, this field indicates the number of packets that must be
 *  transmitted per microframe on the USB. The core uses this field to calculate the
 *  data PID for isochronous IN endpoints.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 *  For non-periodic IN endpoints, this field is valid only in Internal DMA mode. It
 *  specifies the number of packets the core must fetchfor an IN endpoint before it
 *  switches to the endpoint pointed to by the Next Endpoint field of the Device
 *  Endpoint-n Control register (DIEPCTLn.NextEp)
 */
#define USB_OTGFS_DIEPTSIZ3_MC    0x00000003U
#define USB_OTGFS_DIEPTSIZ3_MC_M  (USB_OTGFS_DIEPTSIZ3_MC_V << USB_OTGFS_DIEPTSIZ3_MC_S)
#define USB_OTGFS_DIEPTSIZ3_MC_V  0x00000003U
#define USB_OTGFS_DIEPTSIZ3_MC_S  29

/** USB_OTGFS_DIEPDMA3_REG register
 *  Device IN Endpoint 3 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMA3_REG(i) (DR_REG_USB_BASE(i) + 0x974)
/** USB_OTGFS_DIEPDMA3_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DIEPDMA3_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA3_DMAADDR_M  (USB_OTGFS_DIEPDMA3_DMAADDR_V << USB_OTGFS_DIEPDMA3_DMAADDR_S)
#define USB_OTGFS_DIEPDMA3_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA3_DMAADDR_S  0

/** USB_OTGFS_DTXFSTS3_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 3  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DTXFSTS3_REG(i) (DR_REG_USB_BASE(i) + 0x978)
/** USB_OTGFS_DTXFSTS3_INEPTXFSPCAVAIL : RO; bitpos: [15:0]; default: 256;
 *  IN Endpoint TxFIFO Space Avail (INEPTxFSpcAvail)
 *
 *  Indicates the amount of free space available in the Endpoint TxFIFO.
 *
 *  Values are in terms of 32-bit words.
 *  - 16'h0: Endpoint TxFIFO is full
 *  - 16'h1: 1 word available
 *  - 16'h2: 2 words available
 *  - 16'hn: n words available (where 0  n  32,768)
 *  - 16'h8000: 32,768 words available
 *  - Others: Reserved
 */
#define USB_OTGFS_DTXFSTS3_INEPTXFSPCAVAIL    0x0000FFFFU
#define USB_OTGFS_DTXFSTS3_INEPTXFSPCAVAIL_M  (USB_OTGFS_DTXFSTS3_INEPTXFSPCAVAIL_V << USB_OTGFS_DTXFSTS3_INEPTXFSPCAVAIL_S)
#define USB_OTGFS_DTXFSTS3_INEPTXFSPCAVAIL_V  0x0000FFFFU
#define USB_OTGFS_DTXFSTS3_INEPTXFSPCAVAIL_S  0

/** USB_OTGFS_DIEPDMAB3_REG register
 *  Device IN Endpoint 3 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMAB3_REG(i) (DR_REG_USB_BASE(i) + 0x97c)
/** USB_OTGFS_DIEPDMAB3_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DIEPDMAB3_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB3_DMABUFFERADDR_M  (USB_OTGFS_DIEPDMAB3_DMABUFFERADDR_V << USB_OTGFS_DIEPDMAB3_DMABUFFERADDR_S)
#define USB_OTGFS_DIEPDMAB3_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB3_DMABUFFERADDR_S  0

/** USB_OTGFS_DIEPCTL4_REG register
 *  Device Control IN Endpoint 4 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPCTL4_REG(i) (DR_REG_USB_BASE(i) + 0x980)
/** USB_OTGFS_DIEPCTL4_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the
 *  current logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DIEPCTL4_MPS    0x000007FFU
#define USB_OTGFS_DIEPCTL4_MPS_M  (USB_OTGFS_DIEPCTL4_MPS_V << USB_OTGFS_DIEPCTL4_MPS_S)
#define USB_OTGFS_DIEPCTL4_MPS_V  0x000007FFU
#define USB_OTGFS_DIEPCTL4_MPS_S  0
/** USB_OTGFS_DIEPCTL4_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DIEPCTL4_USBACTEP    (BIT(15))
#define USB_OTGFS_DIEPCTL4_USBACTEP_M  (USB_OTGFS_DIEPCTL4_USBACTEP_V << USB_OTGFS_DIEPCTL4_USBACTEP_S)
#define USB_OTGFS_DIEPCTL4_USBACTEP_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_USBACTEP_S  15
/** USB_OTGFS_DIEPCTL4_DPID : RO; bitpos: [16]; default: 0;
 *
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *
 *  Applies to isochronous IN and OUT endpoints only.
 *
 *  Indicates the (micro)frame number in which the core transmits/receives isochronous
 *  data for this endpoint. The application must program the even/odd (micro)frame
 *  number in which it intends to transmit/receive isochronous data for this endpoint
 *  using
 *  the SetEvnFr and SetOddFr fields in this register.
 *  - 1'b0: Even (micro)frame
 *  - 1'b1: Odd (micro)frame
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame in
 *  which data is received is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL4_DPID    (BIT(16))
#define USB_OTGFS_DIEPCTL4_DPID_M  (USB_OTGFS_DIEPCTL4_DPID_V << USB_OTGFS_DIEPCTL4_DPID_S)
#define USB_OTGFS_DIEPCTL4_DPID_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_DPID_S  16
/** USB_OTGFS_DIEPCTL4_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL4_NAKSTS    (BIT(17))
#define USB_OTGFS_DIEPCTL4_NAKSTS_M  (USB_OTGFS_DIEPCTL4_NAKSTS_V << USB_OTGFS_DIEPCTL4_NAKSTS_S)
#define USB_OTGFS_DIEPCTL4_NAKSTS_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_NAKSTS_S  17
/** USB_OTGFS_DIEPCTL4_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DIEPCTL4_EPTYPE    0x00000003U
#define USB_OTGFS_DIEPCTL4_EPTYPE_M  (USB_OTGFS_DIEPCTL4_EPTYPE_V << USB_OTGFS_DIEPCTL4_EPTYPE_S)
#define USB_OTGFS_DIEPCTL4_EPTYPE_V  0x00000003U
#define USB_OTGFS_DIEPCTL4_EPTYPE_S  18
/** USB_OTGFS_DIEPCTL4_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL4_STALL    (BIT(21))
#define USB_OTGFS_DIEPCTL4_STALL_M  (USB_OTGFS_DIEPCTL4_STALL_V << USB_OTGFS_DIEPCTL4_STALL_S)
#define USB_OTGFS_DIEPCTL4_STALL_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_STALL_S  21
/** USB_OTGFS_DIEPCTL4_TXFNUM : R/W; bitpos: [25:22]; default: 0;
 *  TxFIFO Number (TxFNum)
 *
 *  Shared FIFO Operation non-periodic endpoints must set this bit to zero. Periodic
 *  endpoints must map this to the corresponding Periodic TxFIFO number.
 *  - 4'h0: Non-Periodic TxFIFO
 *  - Others: Specified Periodic TxFIFO.number
 *  Note: An interrupt IN endpoint can be configured as a non-periodic endpoint for
 *  applications such as mass storage. The core treats an IN endpoint as a non-periodic
 *  endpoint if the TxFNum field is set to 0. Otherwise, a separate periodic FIFO must
 *  be
 *  allocated for an interrupt IN endpoint, and the number of this
 *  FIFO must be programmed into the TxFNum field. Configuring an interrupt IN
 *  endpoint as a non-periodic endpoint saves the extra periodic FIFO area.
 *
 *  Dedicated FIFO Operation: These bits specify the FIFO number associated with this
 *  endpoint. Each active IN endpoint must be programmed to a separate FIFO number.
 *  This field is valid only for IN endpoints.
 */
#define USB_OTGFS_DIEPCTL4_TXFNUM    0x0000000FU
#define USB_OTGFS_DIEPCTL4_TXFNUM_M  (USB_OTGFS_DIEPCTL4_TXFNUM_V << USB_OTGFS_DIEPCTL4_TXFNUM_S)
#define USB_OTGFS_DIEPCTL4_TXFNUM_V  0x0000000FU
#define USB_OTGFS_DIEPCTL4_TXFNUM_S  22
/** USB_OTGFS_DIEPCTL4_CNAK : R/W; bitpos: [26]; default: 0;
 *  Clear NAK (CNAK)
 *
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DIEPCTL4_CNAK    (BIT(26))
#define USB_OTGFS_DIEPCTL4_CNAK_M  (USB_OTGFS_DIEPCTL4_CNAK_V << USB_OTGFS_DIEPCTL4_CNAK_S)
#define USB_OTGFS_DIEPCTL4_CNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_CNAK_S  26
/** USB_OTGFS_DIEPCTL4_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK handshakes on
 *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
 *  received on that endpoint.
 */
#define USB_OTGFS_DIEPCTL4_SNAK    (BIT(27))
#define USB_OTGFS_DIEPCTL4_SNAK_M  (USB_OTGFS_DIEPCTL4_SNAK_V << USB_OTGFS_DIEPCTL4_SNAK_S)
#define USB_OTGFS_DIEPCTL4_SNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_SNAK_S  27
/** USB_OTGFS_DIEPCTL4_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  SetD0PID
 *  - Set DATA0 PID (SetD0PID)
 *  -- Applies to interrupt/bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA0.
 *  -- This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  In non-Scatter/Gather DMA mode: Set Even (micro)Frame (SetEvenFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the Even/Odd (micro)Frame (EO_FrNum) field to even
 *  (micro)Frame.
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which to
 *  receive data is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL4_SETD0PID    (BIT(28))
#define USB_OTGFS_DIEPCTL4_SETD0PID_M  (USB_OTGFS_DIEPCTL4_SETD0PID_V << USB_OTGFS_DIEPCTL4_SETD0PID_S)
#define USB_OTGFS_DIEPCTL4_SETD0PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_SETD0PID_S  28
/** USB_OTGFS_DIEPCTL4_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  SetD1PID
 *  - Set DATA1 PID (SetD1PID)
 *  -- Applies to interrupt and bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA1.
 *  -- This field is applicable both for Scatter-Gather DMA mode and non Scatter-Gather
 *  DMA mode.
 *  - Set odd (micro)Frame (SetOddFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the even and odd (micro)Frame (EO_FrNum) field to odd
 *  (micro)Frame.
 *  -- This field is not applicable for Scatter-Gather DMA mode.
 */
#define USB_OTGFS_DIEPCTL4_SETD1PID    (BIT(29))
#define USB_OTGFS_DIEPCTL4_SETD1PID_M  (USB_OTGFS_DIEPCTL4_SETD1PID_V << USB_OTGFS_DIEPCTL4_SETD1PID_S)
#define USB_OTGFS_DIEPCTL4_SETD1PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_SETD1PID_S  29
/** USB_OTGFS_DIEPCTL4_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DIEPCTL4_EPDIS    (BIT(30))
#define USB_OTGFS_DIEPCTL4_EPDIS_M  (USB_OTGFS_DIEPCTL4_EPDIS_V << USB_OTGFS_DIEPCTL4_EPDIS_S)
#define USB_OTGFS_DIEPCTL4_EPDIS_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_EPDIS_S  30
/** USB_OTGFS_DIEPCTL4_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled,
 *  -- For IN endpoints this bit indicates that the descriptor structure and data
 *  buffer with data ready to transmit is setup.
 *  -- For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  - When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  -- For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  -- For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  - The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  -- SETUP Phase Done
 *  -- Endpoint Disabled
 *  -- Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DIEPCTL4_EPENA    (BIT(31))
#define USB_OTGFS_DIEPCTL4_EPENA_M  (USB_OTGFS_DIEPCTL4_EPENA_V << USB_OTGFS_DIEPCTL4_EPENA_S)
#define USB_OTGFS_DIEPCTL4_EPENA_V  0x00000001U
#define USB_OTGFS_DIEPCTL4_EPENA_S  31

/** USB_OTGFS_DIEPINT4_REG register
 *  Device IN Endpoint 4 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPINT4_REG(i) (DR_REG_USB_BASE(i) + 0x988)
/** USB_OTGFS_DIEPINT4_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DIEPINT4_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DIEPINT4_XFERCOMPL_M  (USB_OTGFS_DIEPINT4_XFERCOMPL_V << USB_OTGFS_DIEPINT4_XFERCOMPL_S)
#define USB_OTGFS_DIEPINT4_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DIEPINT4_XFERCOMPL_S  0
/** USB_OTGFS_DIEPINT4_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DIEPINT4_EPDISBLD    (BIT(1))
#define USB_OTGFS_DIEPINT4_EPDISBLD_M  (USB_OTGFS_DIEPINT4_EPDISBLD_V << USB_OTGFS_DIEPINT4_EPDISBLD_S)
#define USB_OTGFS_DIEPINT4_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DIEPINT4_EPDISBLD_S  1
/** USB_OTGFS_DIEPINT4_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DIEPINT4_AHBERR    (BIT(2))
#define USB_OTGFS_DIEPINT4_AHBERR_M  (USB_OTGFS_DIEPINT4_AHBERR_V << USB_OTGFS_DIEPINT4_AHBERR_S)
#define USB_OTGFS_DIEPINT4_AHBERR_V  0x00000001U
#define USB_OTGFS_DIEPINT4_AHBERR_S  2
/** USB_OTGFS_DIEPINT4_TIMEOUT : R/W; bitpos: [3]; default: 0;
 *  Timeout Condition (TimeOUT)
 *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
 *  - In dedicated FIFO mode, applies only to Control IN endpoints.
 *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
 *  Indicates that the core has detected a timeout condition on the USB for the last IN
 *  token on this endpoint.
 */
#define USB_OTGFS_DIEPINT4_TIMEOUT    (BIT(3))
#define USB_OTGFS_DIEPINT4_TIMEOUT_M  (USB_OTGFS_DIEPINT4_TIMEOUT_V << USB_OTGFS_DIEPINT4_TIMEOUT_S)
#define USB_OTGFS_DIEPINT4_TIMEOUT_V  0x00000001U
#define USB_OTGFS_DIEPINT4_TIMEOUT_S  3
/** USB_OTGFS_DIEPINT4_INTKNTXFEMP : R/W; bitpos: [4]; default: 0;
 *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that an IN token was received when the associated TxFIFO
 *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
 *  which the IN token was received.
 */
#define USB_OTGFS_DIEPINT4_INTKNTXFEMP    (BIT(4))
#define USB_OTGFS_DIEPINT4_INTKNTXFEMP_M  (USB_OTGFS_DIEPINT4_INTKNTXFEMP_V << USB_OTGFS_DIEPINT4_INTKNTXFEMP_S)
#define USB_OTGFS_DIEPINT4_INTKNTXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT4_INTKNTXFEMP_S  4
/** USB_OTGFS_DIEPINT4_INTKNEPMIS : R/W; bitpos: [5]; default: 0;
 *  IN Token Received with EP Mismatch (INTknEPMis)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
 *  endpoint other than the one for which the IN token was received. This interrupt is
 *  asserted on the endpoint for which the IN token was received.
 */
#define USB_OTGFS_DIEPINT4_INTKNEPMIS    (BIT(5))
#define USB_OTGFS_DIEPINT4_INTKNEPMIS_M  (USB_OTGFS_DIEPINT4_INTKNEPMIS_V << USB_OTGFS_DIEPINT4_INTKNEPMIS_S)
#define USB_OTGFS_DIEPINT4_INTKNEPMIS_V  0x00000001U
#define USB_OTGFS_DIEPINT4_INTKNEPMIS_S  5
/** USB_OTGFS_DIEPINT4_INEPNAKEFF : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective (INEPNakEff)
 *
 *  Applies to periodic IN endpoints only.
 *
 *  This bit can be cleared when the application clears the IN endpoint NAK by writing
 *  to DIEPCTLn.CNAK.
 *
 *  This interrupt indicates that the core has sampled the NAK bit
 *
 *  Set (either by the application or by the core). The interrupt indicates that the IN
 *  endpoint NAK bit Set by the application has taken effect in the core.
 *
 *  This interrupt does not guarantee that a NAK handshake is sent on the USB. A STALL
 *  bit takes priority over a NAK bit.
 */
#define USB_OTGFS_DIEPINT4_INEPNAKEFF    (BIT(6))
#define USB_OTGFS_DIEPINT4_INEPNAKEFF_M  (USB_OTGFS_DIEPINT4_INEPNAKEFF_V << USB_OTGFS_DIEPINT4_INEPNAKEFF_S)
#define USB_OTGFS_DIEPINT4_INEPNAKEFF_V  0x00000001U
#define USB_OTGFS_DIEPINT4_INEPNAKEFF_S  6
/** USB_OTGFS_DIEPINT4_TXFEMP : RO; bitpos: [7]; default: 1;
 *  Transmit FIFO Empty (TxFEmp)
 *
 *  This bit is valid only for IN endpoints
 *
 *  This interrupt is asserted when the TxFIFO for this endpoint is
 *  either half or completely empty. The half or completely empty
 *  status is determined by the TxFIFO Empty Level bit in the Core
 *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
 */
#define USB_OTGFS_DIEPINT4_TXFEMP    (BIT(7))
#define USB_OTGFS_DIEPINT4_TXFEMP_M  (USB_OTGFS_DIEPINT4_TXFEMP_V << USB_OTGFS_DIEPINT4_TXFEMP_S)
#define USB_OTGFS_DIEPINT4_TXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT4_TXFEMP_S  7
/** USB_OTGFS_DIEPINT4_TXFIFOUNDRN : R/W; bitpos: [8]; default: 0;
 *  Fifo Underrun (TxfifoUndrn)
 *
 *  Applies to IN endpoints Only
 *
 *  This bit is valid only If thresholding is enabled. The core generates this
 *  interrupt when
 *  it detects a transmit FIFO underrun condition for this endpoint.
 */
#define USB_OTGFS_DIEPINT4_TXFIFOUNDRN    (BIT(8))
#define USB_OTGFS_DIEPINT4_TXFIFOUNDRN_M  (USB_OTGFS_DIEPINT4_TXFIFOUNDRN_V << USB_OTGFS_DIEPINT4_TXFIFOUNDRN_S)
#define USB_OTGFS_DIEPINT4_TXFIFOUNDRN_V  0x00000001U
#define USB_OTGFS_DIEPINT4_TXFIFOUNDRN_S  8
/** USB_OTGFS_DIEPINT4_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed is not ready for the
 *  Core to process, such as Host busy or DMA done.
 */
#define USB_OTGFS_DIEPINT4_BNAINTR    (BIT(9))
#define USB_OTGFS_DIEPINT4_BNAINTR_M  (USB_OTGFS_DIEPINT4_BNAINTR_V << USB_OTGFS_DIEPINT4_BNAINTR_S)
#define USB_OTGFS_DIEPINT4_BNAINTR_V  0x00000001U
#define USB_OTGFS_DIEPINT4_BNAINTR_S  9
/** USB_OTGFS_DIEPINT4_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DIEPINT4_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DIEPINT4_PKTDRPSTS_M  (USB_OTGFS_DIEPINT4_PKTDRPSTS_V << USB_OTGFS_DIEPINT4_PKTDRPSTS_S)
#define USB_OTGFS_DIEPINT4_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DIEPINT4_PKTDRPSTS_S  11
/** USB_OTGFS_DIEPINT4_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DIEPINT4_BBLEERR    (BIT(12))
#define USB_OTGFS_DIEPINT4_BBLEERR_M  (USB_OTGFS_DIEPINT4_BBLEERR_V << USB_OTGFS_DIEPINT4_BBLEERR_S)
#define USB_OTGFS_DIEPINT4_BBLEERR_V  0x00000001U
#define USB_OTGFS_DIEPINT4_BBLEERR_S  12
/** USB_OTGFS_DIEPINT4_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DIEPINT4_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DIEPINT4_NAKINTRPT_M  (USB_OTGFS_DIEPINT4_NAKINTRPT_V << USB_OTGFS_DIEPINT4_NAKINTRPT_S)
#define USB_OTGFS_DIEPINT4_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT4_NAKINTRPT_S  13
/** USB_OTGFS_DIEPINT4_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DIEPINT4_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DIEPINT4_NYETINTRPT_M  (USB_OTGFS_DIEPINT4_NYETINTRPT_V << USB_OTGFS_DIEPINT4_NYETINTRPT_S)
#define USB_OTGFS_DIEPINT4_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT4_NYETINTRPT_S  14

/** USB_OTGFS_DIEPTSIZ4_REG register
 *  Device IN Endpoint 4 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPTSIZ4_REG(i) (DR_REG_USB_BASE(i) + 0x990)
/** USB_OTGFS_DIEPTSIZ4_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet from the
 *  external memory is written to the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ4_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ4_XFERSIZE_M  (USB_OTGFS_DIEPTSIZ4_XFERSIZE_V << USB_OTGFS_DIEPTSIZ4_XFERSIZE_S)
#define USB_OTGFS_DIEPTSIZ4_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ4_XFERSIZE_S  0
/** USB_OTGFS_DIEPTSIZ4_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  Indicates the total number of USB packets that constitute the Transfer Size amount
 *  of data for endpoint 0.
 *
 *  This field is decremented every time a packet (maximum size or short packet) is
 *  read from the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ4_PKTCNT    0x0000007FU
#define USB_OTGFS_DIEPTSIZ4_PKTCNT_M  (USB_OTGFS_DIEPTSIZ4_PKTCNT_V << USB_OTGFS_DIEPTSIZ4_PKTCNT_S)
#define USB_OTGFS_DIEPTSIZ4_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DIEPTSIZ4_PKTCNT_S  19
/** USB_OTGFS_DIEPTSIZ4_MC : R/W; bitpos: [30:29]; default: 0;
 *  MC
 *
 *  Applies to IN endpoints only.
 *
 *  For periodic IN endpoints, this field indicates the number of packets that must be
 *  transmitted per microframe on the USB. The core uses this field to calculate the
 *  data PID for isochronous IN endpoints.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 *  For non-periodic IN endpoints, this field is valid only in Internal DMA mode. It
 *  specifies the number of packets the core must fetchfor an IN endpoint before it
 *  switches to the endpoint pointed to by the Next Endpoint field of the Device
 *  Endpoint-n Control register (DIEPCTLn.NextEp)
 */
#define USB_OTGFS_DIEPTSIZ4_MC    0x00000003U
#define USB_OTGFS_DIEPTSIZ4_MC_M  (USB_OTGFS_DIEPTSIZ4_MC_V << USB_OTGFS_DIEPTSIZ4_MC_S)
#define USB_OTGFS_DIEPTSIZ4_MC_V  0x00000003U
#define USB_OTGFS_DIEPTSIZ4_MC_S  29

/** USB_OTGFS_DIEPDMA4_REG register
 *  Device IN Endpoint 4 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMA4_REG(i) (DR_REG_USB_BASE(i) + 0x994)
/** USB_OTGFS_DIEPDMA4_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DIEPDMA4_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA4_DMAADDR_M  (USB_OTGFS_DIEPDMA4_DMAADDR_V << USB_OTGFS_DIEPDMA4_DMAADDR_S)
#define USB_OTGFS_DIEPDMA4_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA4_DMAADDR_S  0

/** USB_OTGFS_DTXFSTS4_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 4  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DTXFSTS4_REG(i) (DR_REG_USB_BASE(i) + 0x998)
/** USB_OTGFS_DTXFSTS4_INEPTXFSPCAVAIL : RO; bitpos: [15:0]; default: 256;
 *  IN Endpoint TxFIFO Space Avail (INEPTxFSpcAvail)
 *
 *  Indicates the amount of free space available in the Endpoint TxFIFO.
 *
 *  Values are in terms of 32-bit words.
 *  - 16'h0: Endpoint TxFIFO is full
 *  - 16'h1: 1 word available
 *  - 16'h2: 2 words available
 *  - 16'hn: n words available (where 0  n  32,768)
 *  - 16'h8000: 32,768 words available
 *  - Others: Reserved
 */
#define USB_OTGFS_DTXFSTS4_INEPTXFSPCAVAIL    0x0000FFFFU
#define USB_OTGFS_DTXFSTS4_INEPTXFSPCAVAIL_M  (USB_OTGFS_DTXFSTS4_INEPTXFSPCAVAIL_V << USB_OTGFS_DTXFSTS4_INEPTXFSPCAVAIL_S)
#define USB_OTGFS_DTXFSTS4_INEPTXFSPCAVAIL_V  0x0000FFFFU
#define USB_OTGFS_DTXFSTS4_INEPTXFSPCAVAIL_S  0

/** USB_OTGFS_DIEPDMAB4_REG register
 *  Device IN Endpoint 4 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMAB4_REG(i) (DR_REG_USB_BASE(i) + 0x99c)
/** USB_OTGFS_DIEPDMAB4_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DIEPDMAB4_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB4_DMABUFFERADDR_M  (USB_OTGFS_DIEPDMAB4_DMABUFFERADDR_V << USB_OTGFS_DIEPDMAB4_DMABUFFERADDR_S)
#define USB_OTGFS_DIEPDMAB4_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB4_DMABUFFERADDR_S  0

/** USB_OTGFS_DIEPCTL5_REG register
 *  Device Control IN Endpoint 5 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPCTL5_REG(i) (DR_REG_USB_BASE(i) + 0x9a0)
/** USB_OTGFS_DIEPCTL5_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the
 *  current logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DIEPCTL5_MPS    0x000007FFU
#define USB_OTGFS_DIEPCTL5_MPS_M  (USB_OTGFS_DIEPCTL5_MPS_V << USB_OTGFS_DIEPCTL5_MPS_S)
#define USB_OTGFS_DIEPCTL5_MPS_V  0x000007FFU
#define USB_OTGFS_DIEPCTL5_MPS_S  0
/** USB_OTGFS_DIEPCTL5_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DIEPCTL5_USBACTEP    (BIT(15))
#define USB_OTGFS_DIEPCTL5_USBACTEP_M  (USB_OTGFS_DIEPCTL5_USBACTEP_V << USB_OTGFS_DIEPCTL5_USBACTEP_S)
#define USB_OTGFS_DIEPCTL5_USBACTEP_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_USBACTEP_S  15
/** USB_OTGFS_DIEPCTL5_DPID : RO; bitpos: [16]; default: 0;
 *
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *
 *  Applies to isochronous IN and OUT endpoints only.
 *
 *  Indicates the (micro)frame number in which the core transmits/receives isochronous
 *  data for this endpoint. The application must program the even/odd (micro)frame
 *  number in which it intends to transmit/receive isochronous data for this endpoint
 *  using
 *  the SetEvnFr and SetOddFr fields in this register.
 *  - 1'b0: Even (micro)frame
 *  - 1'b1: Odd (micro)frame
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame in
 *  which data is received is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL5_DPID    (BIT(16))
#define USB_OTGFS_DIEPCTL5_DPID_M  (USB_OTGFS_DIEPCTL5_DPID_V << USB_OTGFS_DIEPCTL5_DPID_S)
#define USB_OTGFS_DIEPCTL5_DPID_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_DPID_S  16
/** USB_OTGFS_DIEPCTL5_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL5_NAKSTS    (BIT(17))
#define USB_OTGFS_DIEPCTL5_NAKSTS_M  (USB_OTGFS_DIEPCTL5_NAKSTS_V << USB_OTGFS_DIEPCTL5_NAKSTS_S)
#define USB_OTGFS_DIEPCTL5_NAKSTS_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_NAKSTS_S  17
/** USB_OTGFS_DIEPCTL5_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DIEPCTL5_EPTYPE    0x00000003U
#define USB_OTGFS_DIEPCTL5_EPTYPE_M  (USB_OTGFS_DIEPCTL5_EPTYPE_V << USB_OTGFS_DIEPCTL5_EPTYPE_S)
#define USB_OTGFS_DIEPCTL5_EPTYPE_V  0x00000003U
#define USB_OTGFS_DIEPCTL5_EPTYPE_S  18
/** USB_OTGFS_DIEPCTL5_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL5_STALL    (BIT(21))
#define USB_OTGFS_DIEPCTL5_STALL_M  (USB_OTGFS_DIEPCTL5_STALL_V << USB_OTGFS_DIEPCTL5_STALL_S)
#define USB_OTGFS_DIEPCTL5_STALL_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_STALL_S  21
/** USB_OTGFS_DIEPCTL5_TXFNUM : R/W; bitpos: [25:22]; default: 0;
 *  TxFIFO Number (TxFNum)
 *
 *  Shared FIFO Operation non-periodic endpoints must set this bit to zero. Periodic
 *  endpoints must map this to the corresponding Periodic TxFIFO number.
 *  - 4'h0: Non-Periodic TxFIFO
 *  - Others: Specified Periodic TxFIFO.number
 *  Note: An interrupt IN endpoint can be configured as a non-periodic endpoint for
 *  applications such as mass storage. The core treats an IN endpoint as a non-periodic
 *  endpoint if the TxFNum field is set to 0. Otherwise, a separate periodic FIFO must
 *  be
 *  allocated for an interrupt IN endpoint, and the number of this
 *  FIFO must be programmed into the TxFNum field. Configuring an interrupt IN
 *  endpoint as a non-periodic endpoint saves the extra periodic FIFO area.
 *
 *  Dedicated FIFO Operation: These bits specify the FIFO number associated with this
 *  endpoint. Each active IN endpoint must be programmed to a separate FIFO number.
 *  This field is valid only for IN endpoints.
 */
#define USB_OTGFS_DIEPCTL5_TXFNUM    0x0000000FU
#define USB_OTGFS_DIEPCTL5_TXFNUM_M  (USB_OTGFS_DIEPCTL5_TXFNUM_V << USB_OTGFS_DIEPCTL5_TXFNUM_S)
#define USB_OTGFS_DIEPCTL5_TXFNUM_V  0x0000000FU
#define USB_OTGFS_DIEPCTL5_TXFNUM_S  22
/** USB_OTGFS_DIEPCTL5_CNAK : R/W; bitpos: [26]; default: 0;
 *  Clear NAK (CNAK)
 *
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DIEPCTL5_CNAK    (BIT(26))
#define USB_OTGFS_DIEPCTL5_CNAK_M  (USB_OTGFS_DIEPCTL5_CNAK_V << USB_OTGFS_DIEPCTL5_CNAK_S)
#define USB_OTGFS_DIEPCTL5_CNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_CNAK_S  26
/** USB_OTGFS_DIEPCTL5_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK handshakes on
 *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
 *  received on that endpoint.
 */
#define USB_OTGFS_DIEPCTL5_SNAK    (BIT(27))
#define USB_OTGFS_DIEPCTL5_SNAK_M  (USB_OTGFS_DIEPCTL5_SNAK_V << USB_OTGFS_DIEPCTL5_SNAK_S)
#define USB_OTGFS_DIEPCTL5_SNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_SNAK_S  27
/** USB_OTGFS_DIEPCTL5_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  SetD0PID
 *  - Set DATA0 PID (SetD0PID)
 *  -- Applies to interrupt/bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA0.
 *  -- This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  In non-Scatter/Gather DMA mode: Set Even (micro)Frame (SetEvenFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the Even/Odd (micro)Frame (EO_FrNum) field to even
 *  (micro)Frame.
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which to
 *  receive data is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL5_SETD0PID    (BIT(28))
#define USB_OTGFS_DIEPCTL5_SETD0PID_M  (USB_OTGFS_DIEPCTL5_SETD0PID_V << USB_OTGFS_DIEPCTL5_SETD0PID_S)
#define USB_OTGFS_DIEPCTL5_SETD0PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_SETD0PID_S  28
/** USB_OTGFS_DIEPCTL5_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  SetD1PID
 *  - Set DATA1 PID (SetD1PID)
 *  -- Applies to interrupt and bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA1.
 *  -- This field is applicable both for Scatter-Gather DMA mode and non Scatter-Gather
 *  DMA mode.
 *  - Set odd (micro)Frame (SetOddFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the even and odd (micro)Frame (EO_FrNum) field to odd
 *  (micro)Frame.
 *  -- This field is not applicable for Scatter-Gather DMA mode.
 */
#define USB_OTGFS_DIEPCTL5_SETD1PID    (BIT(29))
#define USB_OTGFS_DIEPCTL5_SETD1PID_M  (USB_OTGFS_DIEPCTL5_SETD1PID_V << USB_OTGFS_DIEPCTL5_SETD1PID_S)
#define USB_OTGFS_DIEPCTL5_SETD1PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_SETD1PID_S  29
/** USB_OTGFS_DIEPCTL5_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DIEPCTL5_EPDIS    (BIT(30))
#define USB_OTGFS_DIEPCTL5_EPDIS_M  (USB_OTGFS_DIEPCTL5_EPDIS_V << USB_OTGFS_DIEPCTL5_EPDIS_S)
#define USB_OTGFS_DIEPCTL5_EPDIS_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_EPDIS_S  30
/** USB_OTGFS_DIEPCTL5_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled,
 *  -- For IN endpoints this bit indicates that the descriptor structure and data
 *  buffer with data ready to transmit is setup.
 *  -- For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  - When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  -- For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  -- For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  - The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  -- SETUP Phase Done
 *  -- Endpoint Disabled
 *  -- Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DIEPCTL5_EPENA    (BIT(31))
#define USB_OTGFS_DIEPCTL5_EPENA_M  (USB_OTGFS_DIEPCTL5_EPENA_V << USB_OTGFS_DIEPCTL5_EPENA_S)
#define USB_OTGFS_DIEPCTL5_EPENA_V  0x00000001U
#define USB_OTGFS_DIEPCTL5_EPENA_S  31

/** USB_OTGFS_DIEPINT5_REG register
 *  Device IN Endpoint 5 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPINT5_REG(i) (DR_REG_USB_BASE(i) + 0x9a8)
/** USB_OTGFS_DIEPINT5_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DIEPINT5_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DIEPINT5_XFERCOMPL_M  (USB_OTGFS_DIEPINT5_XFERCOMPL_V << USB_OTGFS_DIEPINT5_XFERCOMPL_S)
#define USB_OTGFS_DIEPINT5_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DIEPINT5_XFERCOMPL_S  0
/** USB_OTGFS_DIEPINT5_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DIEPINT5_EPDISBLD    (BIT(1))
#define USB_OTGFS_DIEPINT5_EPDISBLD_M  (USB_OTGFS_DIEPINT5_EPDISBLD_V << USB_OTGFS_DIEPINT5_EPDISBLD_S)
#define USB_OTGFS_DIEPINT5_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DIEPINT5_EPDISBLD_S  1
/** USB_OTGFS_DIEPINT5_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DIEPINT5_AHBERR    (BIT(2))
#define USB_OTGFS_DIEPINT5_AHBERR_M  (USB_OTGFS_DIEPINT5_AHBERR_V << USB_OTGFS_DIEPINT5_AHBERR_S)
#define USB_OTGFS_DIEPINT5_AHBERR_V  0x00000001U
#define USB_OTGFS_DIEPINT5_AHBERR_S  2
/** USB_OTGFS_DIEPINT5_TIMEOUT : R/W; bitpos: [3]; default: 0;
 *  Timeout Condition (TimeOUT)
 *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
 *  - In dedicated FIFO mode, applies only to Control IN endpoints.
 *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
 *  Indicates that the core has detected a timeout condition on the USB for the last IN
 *  token on this endpoint.
 */
#define USB_OTGFS_DIEPINT5_TIMEOUT    (BIT(3))
#define USB_OTGFS_DIEPINT5_TIMEOUT_M  (USB_OTGFS_DIEPINT5_TIMEOUT_V << USB_OTGFS_DIEPINT5_TIMEOUT_S)
#define USB_OTGFS_DIEPINT5_TIMEOUT_V  0x00000001U
#define USB_OTGFS_DIEPINT5_TIMEOUT_S  3
/** USB_OTGFS_DIEPINT5_INTKNTXFEMP : R/W; bitpos: [4]; default: 0;
 *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that an IN token was received when the associated TxFIFO
 *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
 *  which the IN token was received.
 */
#define USB_OTGFS_DIEPINT5_INTKNTXFEMP    (BIT(4))
#define USB_OTGFS_DIEPINT5_INTKNTXFEMP_M  (USB_OTGFS_DIEPINT5_INTKNTXFEMP_V << USB_OTGFS_DIEPINT5_INTKNTXFEMP_S)
#define USB_OTGFS_DIEPINT5_INTKNTXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT5_INTKNTXFEMP_S  4
/** USB_OTGFS_DIEPINT5_INTKNEPMIS : R/W; bitpos: [5]; default: 0;
 *  IN Token Received with EP Mismatch (INTknEPMis)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
 *  endpoint other than the one for which the IN token was received. This interrupt is
 *  asserted on the endpoint for which the IN token was received.
 */
#define USB_OTGFS_DIEPINT5_INTKNEPMIS    (BIT(5))
#define USB_OTGFS_DIEPINT5_INTKNEPMIS_M  (USB_OTGFS_DIEPINT5_INTKNEPMIS_V << USB_OTGFS_DIEPINT5_INTKNEPMIS_S)
#define USB_OTGFS_DIEPINT5_INTKNEPMIS_V  0x00000001U
#define USB_OTGFS_DIEPINT5_INTKNEPMIS_S  5
/** USB_OTGFS_DIEPINT5_INEPNAKEFF : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective (INEPNakEff)
 *
 *  Applies to periodic IN endpoints only.
 *
 *  This bit can be cleared when the application clears the IN endpoint NAK by writing
 *  to DIEPCTLn.CNAK.
 *
 *  This interrupt indicates that the core has sampled the NAK bit
 *
 *  Set (either by the application or by the core). The interrupt indicates that the IN
 *  endpoint NAK bit Set by the application has taken effect in the core.
 *
 *  This interrupt does not guarantee that a NAK handshake is sent on the USB. A STALL
 *  bit takes priority over a NAK bit.
 */
#define USB_OTGFS_DIEPINT5_INEPNAKEFF    (BIT(6))
#define USB_OTGFS_DIEPINT5_INEPNAKEFF_M  (USB_OTGFS_DIEPINT5_INEPNAKEFF_V << USB_OTGFS_DIEPINT5_INEPNAKEFF_S)
#define USB_OTGFS_DIEPINT5_INEPNAKEFF_V  0x00000001U
#define USB_OTGFS_DIEPINT5_INEPNAKEFF_S  6
/** USB_OTGFS_DIEPINT5_TXFEMP : RO; bitpos: [7]; default: 1;
 *  Transmit FIFO Empty (TxFEmp)
 *
 *  This bit is valid only for IN endpoints
 *
 *  This interrupt is asserted when the TxFIFO for this endpoint is
 *  either half or completely empty. The half or completely empty
 *  status is determined by the TxFIFO Empty Level bit in the Core
 *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
 */
#define USB_OTGFS_DIEPINT5_TXFEMP    (BIT(7))
#define USB_OTGFS_DIEPINT5_TXFEMP_M  (USB_OTGFS_DIEPINT5_TXFEMP_V << USB_OTGFS_DIEPINT5_TXFEMP_S)
#define USB_OTGFS_DIEPINT5_TXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT5_TXFEMP_S  7
/** USB_OTGFS_DIEPINT5_TXFIFOUNDRN : R/W; bitpos: [8]; default: 0;
 *  Fifo Underrun (TxfifoUndrn)
 *
 *  Applies to IN endpoints Only
 *
 *  This bit is valid only If thresholding is enabled. The core generates this
 *  interrupt when
 *  it detects a transmit FIFO underrun condition for this endpoint.
 */
#define USB_OTGFS_DIEPINT5_TXFIFOUNDRN    (BIT(8))
#define USB_OTGFS_DIEPINT5_TXFIFOUNDRN_M  (USB_OTGFS_DIEPINT5_TXFIFOUNDRN_V << USB_OTGFS_DIEPINT5_TXFIFOUNDRN_S)
#define USB_OTGFS_DIEPINT5_TXFIFOUNDRN_V  0x00000001U
#define USB_OTGFS_DIEPINT5_TXFIFOUNDRN_S  8
/** USB_OTGFS_DIEPINT5_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed is not ready for the
 *  Core to process, such as Host busy or DMA done.
 */
#define USB_OTGFS_DIEPINT5_BNAINTR    (BIT(9))
#define USB_OTGFS_DIEPINT5_BNAINTR_M  (USB_OTGFS_DIEPINT5_BNAINTR_V << USB_OTGFS_DIEPINT5_BNAINTR_S)
#define USB_OTGFS_DIEPINT5_BNAINTR_V  0x00000001U
#define USB_OTGFS_DIEPINT5_BNAINTR_S  9
/** USB_OTGFS_DIEPINT5_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DIEPINT5_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DIEPINT5_PKTDRPSTS_M  (USB_OTGFS_DIEPINT5_PKTDRPSTS_V << USB_OTGFS_DIEPINT5_PKTDRPSTS_S)
#define USB_OTGFS_DIEPINT5_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DIEPINT5_PKTDRPSTS_S  11
/** USB_OTGFS_DIEPINT5_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DIEPINT5_BBLEERR    (BIT(12))
#define USB_OTGFS_DIEPINT5_BBLEERR_M  (USB_OTGFS_DIEPINT5_BBLEERR_V << USB_OTGFS_DIEPINT5_BBLEERR_S)
#define USB_OTGFS_DIEPINT5_BBLEERR_V  0x00000001U
#define USB_OTGFS_DIEPINT5_BBLEERR_S  12
/** USB_OTGFS_DIEPINT5_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DIEPINT5_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DIEPINT5_NAKINTRPT_M  (USB_OTGFS_DIEPINT5_NAKINTRPT_V << USB_OTGFS_DIEPINT5_NAKINTRPT_S)
#define USB_OTGFS_DIEPINT5_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT5_NAKINTRPT_S  13
/** USB_OTGFS_DIEPINT5_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DIEPINT5_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DIEPINT5_NYETINTRPT_M  (USB_OTGFS_DIEPINT5_NYETINTRPT_V << USB_OTGFS_DIEPINT5_NYETINTRPT_S)
#define USB_OTGFS_DIEPINT5_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT5_NYETINTRPT_S  14

/** USB_OTGFS_DIEPTSIZ5_REG register
 *  Device IN Endpoint 5 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPTSIZ5_REG(i) (DR_REG_USB_BASE(i) + 0x9b0)
/** USB_OTGFS_DIEPTSIZ5_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet from the
 *  external memory is written to the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ5_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ5_XFERSIZE_M  (USB_OTGFS_DIEPTSIZ5_XFERSIZE_V << USB_OTGFS_DIEPTSIZ5_XFERSIZE_S)
#define USB_OTGFS_DIEPTSIZ5_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ5_XFERSIZE_S  0
/** USB_OTGFS_DIEPTSIZ5_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  Indicates the total number of USB packets that constitute the Transfer Size amount
 *  of data for endpoint 0.
 *
 *  This field is decremented every time a packet (maximum size or short packet) is
 *  read from the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ5_PKTCNT    0x0000007FU
#define USB_OTGFS_DIEPTSIZ5_PKTCNT_M  (USB_OTGFS_DIEPTSIZ5_PKTCNT_V << USB_OTGFS_DIEPTSIZ5_PKTCNT_S)
#define USB_OTGFS_DIEPTSIZ5_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DIEPTSIZ5_PKTCNT_S  19
/** USB_OTGFS_DIEPTSIZ5_MC : R/W; bitpos: [30:29]; default: 0;
 *  MC
 *
 *  Applies to IN endpoints only.
 *
 *  For periodic IN endpoints, this field indicates the number of packets that must be
 *  transmitted per microframe on the USB. The core uses this field to calculate the
 *  data PID for isochronous IN endpoints.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 *  For non-periodic IN endpoints, this field is valid only in Internal DMA mode. It
 *  specifies the number of packets the core must fetchfor an IN endpoint before it
 *  switches to the endpoint pointed to by the Next Endpoint field of the Device
 *  Endpoint-n Control register (DIEPCTLn.NextEp)
 */
#define USB_OTGFS_DIEPTSIZ5_MC    0x00000003U
#define USB_OTGFS_DIEPTSIZ5_MC_M  (USB_OTGFS_DIEPTSIZ5_MC_V << USB_OTGFS_DIEPTSIZ5_MC_S)
#define USB_OTGFS_DIEPTSIZ5_MC_V  0x00000003U
#define USB_OTGFS_DIEPTSIZ5_MC_S  29

/** USB_OTGFS_DIEPDMA5_REG register
 *  Device IN Endpoint 5 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMA5_REG(i) (DR_REG_USB_BASE(i) + 0x9b4)
/** USB_OTGFS_DIEPDMA5_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DIEPDMA5_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA5_DMAADDR_M  (USB_OTGFS_DIEPDMA5_DMAADDR_V << USB_OTGFS_DIEPDMA5_DMAADDR_S)
#define USB_OTGFS_DIEPDMA5_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA5_DMAADDR_S  0

/** USB_OTGFS_DTXFSTS5_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 5  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DTXFSTS5_REG(i) (DR_REG_USB_BASE(i) + 0x9b8)
/** USB_OTGFS_DTXFSTS5_INEPTXFSPCAVAIL : RO; bitpos: [15:0]; default: 256;
 *  IN Endpoint TxFIFO Space Avail (INEPTxFSpcAvail)
 *
 *  Indicates the amount of free space available in the Endpoint TxFIFO.
 *
 *  Values are in terms of 32-bit words.
 *  - 16'h0: Endpoint TxFIFO is full
 *  - 16'h1: 1 word available
 *  - 16'h2: 2 words available
 *  - 16'hn: n words available (where 0  n  32,768)
 *  - 16'h8000: 32,768 words available
 *  - Others: Reserved
 */
#define USB_OTGFS_DTXFSTS5_INEPTXFSPCAVAIL    0x0000FFFFU
#define USB_OTGFS_DTXFSTS5_INEPTXFSPCAVAIL_M  (USB_OTGFS_DTXFSTS5_INEPTXFSPCAVAIL_V << USB_OTGFS_DTXFSTS5_INEPTXFSPCAVAIL_S)
#define USB_OTGFS_DTXFSTS5_INEPTXFSPCAVAIL_V  0x0000FFFFU
#define USB_OTGFS_DTXFSTS5_INEPTXFSPCAVAIL_S  0

/** USB_OTGFS_DIEPDMAB5_REG register
 *  Device IN Endpoint 5 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMAB5_REG(i) (DR_REG_USB_BASE(i) + 0x9bc)
/** USB_OTGFS_DIEPDMAB5_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DIEPDMAB5_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB5_DMABUFFERADDR_M  (USB_OTGFS_DIEPDMAB5_DMABUFFERADDR_V << USB_OTGFS_DIEPDMAB5_DMABUFFERADDR_S)
#define USB_OTGFS_DIEPDMAB5_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB5_DMABUFFERADDR_S  0

/** USB_OTGFS_DIEPCTL6_REG register
 *  Device Control IN Endpoint 6 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPCTL6_REG(i) (DR_REG_USB_BASE(i) + 0x9c0)
/** USB_OTGFS_DIEPCTL6_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the
 *  current logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DIEPCTL6_MPS    0x000007FFU
#define USB_OTGFS_DIEPCTL6_MPS_M  (USB_OTGFS_DIEPCTL6_MPS_V << USB_OTGFS_DIEPCTL6_MPS_S)
#define USB_OTGFS_DIEPCTL6_MPS_V  0x000007FFU
#define USB_OTGFS_DIEPCTL6_MPS_S  0
/** USB_OTGFS_DIEPCTL6_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DIEPCTL6_USBACTEP    (BIT(15))
#define USB_OTGFS_DIEPCTL6_USBACTEP_M  (USB_OTGFS_DIEPCTL6_USBACTEP_V << USB_OTGFS_DIEPCTL6_USBACTEP_S)
#define USB_OTGFS_DIEPCTL6_USBACTEP_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_USBACTEP_S  15
/** USB_OTGFS_DIEPCTL6_DPID : RO; bitpos: [16]; default: 0;
 *
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *
 *  Applies to isochronous IN and OUT endpoints only.
 *
 *  Indicates the (micro)frame number in which the core transmits/receives isochronous
 *  data for this endpoint. The application must program the even/odd (micro)frame
 *  number in which it intends to transmit/receive isochronous data for this endpoint
 *  using
 *  the SetEvnFr and SetOddFr fields in this register.
 *  - 1'b0: Even (micro)frame
 *  - 1'b1: Odd (micro)frame
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame in
 *  which data is received is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL6_DPID    (BIT(16))
#define USB_OTGFS_DIEPCTL6_DPID_M  (USB_OTGFS_DIEPCTL6_DPID_V << USB_OTGFS_DIEPCTL6_DPID_S)
#define USB_OTGFS_DIEPCTL6_DPID_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_DPID_S  16
/** USB_OTGFS_DIEPCTL6_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL6_NAKSTS    (BIT(17))
#define USB_OTGFS_DIEPCTL6_NAKSTS_M  (USB_OTGFS_DIEPCTL6_NAKSTS_V << USB_OTGFS_DIEPCTL6_NAKSTS_S)
#define USB_OTGFS_DIEPCTL6_NAKSTS_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_NAKSTS_S  17
/** USB_OTGFS_DIEPCTL6_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DIEPCTL6_EPTYPE    0x00000003U
#define USB_OTGFS_DIEPCTL6_EPTYPE_M  (USB_OTGFS_DIEPCTL6_EPTYPE_V << USB_OTGFS_DIEPCTL6_EPTYPE_S)
#define USB_OTGFS_DIEPCTL6_EPTYPE_V  0x00000003U
#define USB_OTGFS_DIEPCTL6_EPTYPE_S  18
/** USB_OTGFS_DIEPCTL6_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DIEPCTL6_STALL    (BIT(21))
#define USB_OTGFS_DIEPCTL6_STALL_M  (USB_OTGFS_DIEPCTL6_STALL_V << USB_OTGFS_DIEPCTL6_STALL_S)
#define USB_OTGFS_DIEPCTL6_STALL_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_STALL_S  21
/** USB_OTGFS_DIEPCTL6_TXFNUM : R/W; bitpos: [25:22]; default: 0;
 *  TxFIFO Number (TxFNum)
 *
 *  Shared FIFO Operation non-periodic endpoints must set this bit to zero. Periodic
 *  endpoints must map this to the corresponding Periodic TxFIFO number.
 *  - 4'h0: Non-Periodic TxFIFO
 *  - Others: Specified Periodic TxFIFO.number
 *  Note: An interrupt IN endpoint can be configured as a non-periodic endpoint for
 *  applications such as mass storage. The core treats an IN endpoint as a non-periodic
 *  endpoint if the TxFNum field is set to 0. Otherwise, a separate periodic FIFO must
 *  be
 *  allocated for an interrupt IN endpoint, and the number of this
 *  FIFO must be programmed into the TxFNum field. Configuring an interrupt IN
 *  endpoint as a non-periodic endpoint saves the extra periodic FIFO area.
 *
 *  Dedicated FIFO Operation: These bits specify the FIFO number associated with this
 *  endpoint. Each active IN endpoint must be programmed to a separate FIFO number.
 *  This field is valid only for IN endpoints.
 */
#define USB_OTGFS_DIEPCTL6_TXFNUM    0x0000000FU
#define USB_OTGFS_DIEPCTL6_TXFNUM_M  (USB_OTGFS_DIEPCTL6_TXFNUM_V << USB_OTGFS_DIEPCTL6_TXFNUM_S)
#define USB_OTGFS_DIEPCTL6_TXFNUM_V  0x0000000FU
#define USB_OTGFS_DIEPCTL6_TXFNUM_S  22
/** USB_OTGFS_DIEPCTL6_CNAK : R/W; bitpos: [26]; default: 0;
 *  Clear NAK (CNAK)
 *
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DIEPCTL6_CNAK    (BIT(26))
#define USB_OTGFS_DIEPCTL6_CNAK_M  (USB_OTGFS_DIEPCTL6_CNAK_V << USB_OTGFS_DIEPCTL6_CNAK_S)
#define USB_OTGFS_DIEPCTL6_CNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_CNAK_S  26
/** USB_OTGFS_DIEPCTL6_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK handshakes on
 *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
 *  received on that endpoint.
 */
#define USB_OTGFS_DIEPCTL6_SNAK    (BIT(27))
#define USB_OTGFS_DIEPCTL6_SNAK_M  (USB_OTGFS_DIEPCTL6_SNAK_V << USB_OTGFS_DIEPCTL6_SNAK_S)
#define USB_OTGFS_DIEPCTL6_SNAK_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_SNAK_S  27
/** USB_OTGFS_DIEPCTL6_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  SetD0PID
 *  - Set DATA0 PID (SetD0PID)
 *  -- Applies to interrupt/bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA0.
 *  -- This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  In non-Scatter/Gather DMA mode: Set Even (micro)Frame (SetEvenFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the Even/Odd (micro)Frame (EO_FrNum) field to even
 *  (micro)Frame.
 *  When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which to
 *  receive data is updated in receive descriptor structure.
 */
#define USB_OTGFS_DIEPCTL6_SETD0PID    (BIT(28))
#define USB_OTGFS_DIEPCTL6_SETD0PID_M  (USB_OTGFS_DIEPCTL6_SETD0PID_V << USB_OTGFS_DIEPCTL6_SETD0PID_S)
#define USB_OTGFS_DIEPCTL6_SETD0PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_SETD0PID_S  28
/** USB_OTGFS_DIEPCTL6_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  SetD1PID
 *  - Set DATA1 PID (SetD1PID)
 *  -- Applies to interrupt and bulk IN and OUT endpoints only.
 *  -- Writing to this field sets the Endpoint Data PID (DPID) field in this register
 *  to DATA1.
 *  -- This field is applicable both for Scatter-Gather DMA mode and non Scatter-Gather
 *  DMA mode.
 *  - Set odd (micro)Frame (SetOddFr)
 *  -- Applies to isochronous IN and OUT endpoints only.
 *  -- Writing to this field sets the even and odd (micro)Frame (EO_FrNum) field to odd
 *  (micro)Frame.
 *  -- This field is not applicable for Scatter-Gather DMA mode.
 */
#define USB_OTGFS_DIEPCTL6_SETD1PID    (BIT(29))
#define USB_OTGFS_DIEPCTL6_SETD1PID_M  (USB_OTGFS_DIEPCTL6_SETD1PID_V << USB_OTGFS_DIEPCTL6_SETD1PID_S)
#define USB_OTGFS_DIEPCTL6_SETD1PID_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_SETD1PID_S  29
/** USB_OTGFS_DIEPCTL6_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DIEPCTL6_EPDIS    (BIT(30))
#define USB_OTGFS_DIEPCTL6_EPDIS_M  (USB_OTGFS_DIEPCTL6_EPDIS_V << USB_OTGFS_DIEPCTL6_EPDIS_S)
#define USB_OTGFS_DIEPCTL6_EPDIS_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_EPDIS_S  30
/** USB_OTGFS_DIEPCTL6_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled,
 *  -- For IN endpoints this bit indicates that the descriptor structure and data
 *  buffer with data ready to transmit is setup.
 *  -- For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  - When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  -- For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  -- For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  - The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  -- SETUP Phase Done
 *  -- Endpoint Disabled
 *  -- Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DIEPCTL6_EPENA    (BIT(31))
#define USB_OTGFS_DIEPCTL6_EPENA_M  (USB_OTGFS_DIEPCTL6_EPENA_V << USB_OTGFS_DIEPCTL6_EPENA_S)
#define USB_OTGFS_DIEPCTL6_EPENA_V  0x00000001U
#define USB_OTGFS_DIEPCTL6_EPENA_S  31

/** USB_OTGFS_DIEPINT6_REG register
 *  Device IN Endpoint 6 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPINT6_REG(i) (DR_REG_USB_BASE(i) + 0x9c8)
/** USB_OTGFS_DIEPINT6_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DIEPINT6_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DIEPINT6_XFERCOMPL_M  (USB_OTGFS_DIEPINT6_XFERCOMPL_V << USB_OTGFS_DIEPINT6_XFERCOMPL_S)
#define USB_OTGFS_DIEPINT6_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DIEPINT6_XFERCOMPL_S  0
/** USB_OTGFS_DIEPINT6_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DIEPINT6_EPDISBLD    (BIT(1))
#define USB_OTGFS_DIEPINT6_EPDISBLD_M  (USB_OTGFS_DIEPINT6_EPDISBLD_V << USB_OTGFS_DIEPINT6_EPDISBLD_S)
#define USB_OTGFS_DIEPINT6_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DIEPINT6_EPDISBLD_S  1
/** USB_OTGFS_DIEPINT6_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DIEPINT6_AHBERR    (BIT(2))
#define USB_OTGFS_DIEPINT6_AHBERR_M  (USB_OTGFS_DIEPINT6_AHBERR_V << USB_OTGFS_DIEPINT6_AHBERR_S)
#define USB_OTGFS_DIEPINT6_AHBERR_V  0x00000001U
#define USB_OTGFS_DIEPINT6_AHBERR_S  2
/** USB_OTGFS_DIEPINT6_TIMEOUT : R/W; bitpos: [3]; default: 0;
 *  Timeout Condition (TimeOUT)
 *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
 *  - In dedicated FIFO mode, applies only to Control IN endpoints.
 *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
 *  Indicates that the core has detected a timeout condition on the USB for the last IN
 *  token on this endpoint.
 */
#define USB_OTGFS_DIEPINT6_TIMEOUT    (BIT(3))
#define USB_OTGFS_DIEPINT6_TIMEOUT_M  (USB_OTGFS_DIEPINT6_TIMEOUT_V << USB_OTGFS_DIEPINT6_TIMEOUT_S)
#define USB_OTGFS_DIEPINT6_TIMEOUT_V  0x00000001U
#define USB_OTGFS_DIEPINT6_TIMEOUT_S  3
/** USB_OTGFS_DIEPINT6_INTKNTXFEMP : R/W; bitpos: [4]; default: 0;
 *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that an IN token was received when the associated TxFIFO
 *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
 *  which the IN token was received.
 */
#define USB_OTGFS_DIEPINT6_INTKNTXFEMP    (BIT(4))
#define USB_OTGFS_DIEPINT6_INTKNTXFEMP_M  (USB_OTGFS_DIEPINT6_INTKNTXFEMP_V << USB_OTGFS_DIEPINT6_INTKNTXFEMP_S)
#define USB_OTGFS_DIEPINT6_INTKNTXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT6_INTKNTXFEMP_S  4
/** USB_OTGFS_DIEPINT6_INTKNEPMIS : R/W; bitpos: [5]; default: 0;
 *  IN Token Received with EP Mismatch (INTknEPMis)
 *
 *  Applies to non-periodic IN endpoints only.
 *
 *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
 *  endpoint other than the one for which the IN token was received. This interrupt is
 *  asserted on the endpoint for which the IN token was received.
 */
#define USB_OTGFS_DIEPINT6_INTKNEPMIS    (BIT(5))
#define USB_OTGFS_DIEPINT6_INTKNEPMIS_M  (USB_OTGFS_DIEPINT6_INTKNEPMIS_V << USB_OTGFS_DIEPINT6_INTKNEPMIS_S)
#define USB_OTGFS_DIEPINT6_INTKNEPMIS_V  0x00000001U
#define USB_OTGFS_DIEPINT6_INTKNEPMIS_S  5
/** USB_OTGFS_DIEPINT6_INEPNAKEFF : R/W; bitpos: [6]; default: 0;
 *  IN Endpoint NAK Effective (INEPNakEff)
 *
 *  Applies to periodic IN endpoints only.
 *
 *  This bit can be cleared when the application clears the IN endpoint NAK by writing
 *  to DIEPCTLn.CNAK.
 *
 *  This interrupt indicates that the core has sampled the NAK bit
 *
 *  Set (either by the application or by the core). The interrupt indicates that the IN
 *  endpoint NAK bit Set by the application has taken effect in the core.
 *
 *  This interrupt does not guarantee that a NAK handshake is sent on the USB. A STALL
 *  bit takes priority over a NAK bit.
 */
#define USB_OTGFS_DIEPINT6_INEPNAKEFF    (BIT(6))
#define USB_OTGFS_DIEPINT6_INEPNAKEFF_M  (USB_OTGFS_DIEPINT6_INEPNAKEFF_V << USB_OTGFS_DIEPINT6_INEPNAKEFF_S)
#define USB_OTGFS_DIEPINT6_INEPNAKEFF_V  0x00000001U
#define USB_OTGFS_DIEPINT6_INEPNAKEFF_S  6
/** USB_OTGFS_DIEPINT6_TXFEMP : RO; bitpos: [7]; default: 1;
 *  Transmit FIFO Empty (TxFEmp)
 *
 *  This bit is valid only for IN endpoints
 *
 *  This interrupt is asserted when the TxFIFO for this endpoint is
 *  either half or completely empty. The half or completely empty
 *  status is determined by the TxFIFO Empty Level bit in the Core
 *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
 */
#define USB_OTGFS_DIEPINT6_TXFEMP    (BIT(7))
#define USB_OTGFS_DIEPINT6_TXFEMP_M  (USB_OTGFS_DIEPINT6_TXFEMP_V << USB_OTGFS_DIEPINT6_TXFEMP_S)
#define USB_OTGFS_DIEPINT6_TXFEMP_V  0x00000001U
#define USB_OTGFS_DIEPINT6_TXFEMP_S  7
/** USB_OTGFS_DIEPINT6_TXFIFOUNDRN : R/W; bitpos: [8]; default: 0;
 *  Fifo Underrun (TxfifoUndrn)
 *
 *  Applies to IN endpoints Only
 *
 *  This bit is valid only If thresholding is enabled. The core generates this
 *  interrupt when
 *  it detects a transmit FIFO underrun condition for this endpoint.
 */
#define USB_OTGFS_DIEPINT6_TXFIFOUNDRN    (BIT(8))
#define USB_OTGFS_DIEPINT6_TXFIFOUNDRN_M  (USB_OTGFS_DIEPINT6_TXFIFOUNDRN_V << USB_OTGFS_DIEPINT6_TXFIFOUNDRN_S)
#define USB_OTGFS_DIEPINT6_TXFIFOUNDRN_V  0x00000001U
#define USB_OTGFS_DIEPINT6_TXFIFOUNDRN_S  8
/** USB_OTGFS_DIEPINT6_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed is not ready for the
 *  Core to process, such as Host busy or DMA done.
 */
#define USB_OTGFS_DIEPINT6_BNAINTR    (BIT(9))
#define USB_OTGFS_DIEPINT6_BNAINTR_M  (USB_OTGFS_DIEPINT6_BNAINTR_V << USB_OTGFS_DIEPINT6_BNAINTR_S)
#define USB_OTGFS_DIEPINT6_BNAINTR_V  0x00000001U
#define USB_OTGFS_DIEPINT6_BNAINTR_S  9
/** USB_OTGFS_DIEPINT6_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DIEPINT6_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DIEPINT6_PKTDRPSTS_M  (USB_OTGFS_DIEPINT6_PKTDRPSTS_V << USB_OTGFS_DIEPINT6_PKTDRPSTS_S)
#define USB_OTGFS_DIEPINT6_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DIEPINT6_PKTDRPSTS_S  11
/** USB_OTGFS_DIEPINT6_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DIEPINT6_BBLEERR    (BIT(12))
#define USB_OTGFS_DIEPINT6_BBLEERR_M  (USB_OTGFS_DIEPINT6_BBLEERR_V << USB_OTGFS_DIEPINT6_BBLEERR_S)
#define USB_OTGFS_DIEPINT6_BBLEERR_V  0x00000001U
#define USB_OTGFS_DIEPINT6_BBLEERR_S  12
/** USB_OTGFS_DIEPINT6_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DIEPINT6_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DIEPINT6_NAKINTRPT_M  (USB_OTGFS_DIEPINT6_NAKINTRPT_V << USB_OTGFS_DIEPINT6_NAKINTRPT_S)
#define USB_OTGFS_DIEPINT6_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT6_NAKINTRPT_S  13
/** USB_OTGFS_DIEPINT6_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DIEPINT6_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DIEPINT6_NYETINTRPT_M  (USB_OTGFS_DIEPINT6_NYETINTRPT_V << USB_OTGFS_DIEPINT6_NYETINTRPT_S)
#define USB_OTGFS_DIEPINT6_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DIEPINT6_NYETINTRPT_S  14

/** USB_OTGFS_DIEPTSIZ6_REG register
 *  Device IN Endpoint 6 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPTSIZ6_REG(i) (DR_REG_USB_BASE(i) + 0x9d0)
/** USB_OTGFS_DIEPTSIZ6_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet from the
 *  external memory is written to the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ6_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ6_XFERSIZE_M  (USB_OTGFS_DIEPTSIZ6_XFERSIZE_V << USB_OTGFS_DIEPTSIZ6_XFERSIZE_S)
#define USB_OTGFS_DIEPTSIZ6_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DIEPTSIZ6_XFERSIZE_S  0
/** USB_OTGFS_DIEPTSIZ6_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  Indicates the total number of USB packets that constitute the Transfer Size amount
 *  of data for endpoint 0.
 *
 *  This field is decremented every time a packet (maximum size or short packet) is
 *  read from the TxFIFO.
 */
#define USB_OTGFS_DIEPTSIZ6_PKTCNT    0x0000007FU
#define USB_OTGFS_DIEPTSIZ6_PKTCNT_M  (USB_OTGFS_DIEPTSIZ6_PKTCNT_V << USB_OTGFS_DIEPTSIZ6_PKTCNT_S)
#define USB_OTGFS_DIEPTSIZ6_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DIEPTSIZ6_PKTCNT_S  19
/** USB_OTGFS_DIEPTSIZ6_MC : R/W; bitpos: [30:29]; default: 0;
 *  MC
 *
 *  Applies to IN endpoints only.
 *
 *  For periodic IN endpoints, this field indicates the number of packets that must be
 *  transmitted per microframe on the USB. The core uses this field to calculate the
 *  data PID for isochronous IN endpoints.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 *  For non-periodic IN endpoints, this field is valid only in Internal DMA mode. It
 *  specifies the number of packets the core must fetchfor an IN endpoint before it
 *  switches to the endpoint pointed to by the Next Endpoint field of the Device
 *  Endpoint-n Control register (DIEPCTLn.NextEp)
 */
#define USB_OTGFS_DIEPTSIZ6_MC    0x00000003U
#define USB_OTGFS_DIEPTSIZ6_MC_M  (USB_OTGFS_DIEPTSIZ6_MC_V << USB_OTGFS_DIEPTSIZ6_MC_S)
#define USB_OTGFS_DIEPTSIZ6_MC_V  0x00000003U
#define USB_OTGFS_DIEPTSIZ6_MC_S  29

/** USB_OTGFS_DIEPDMA6_REG register
 *  Device IN Endpoint 6 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMA6_REG(i) (DR_REG_USB_BASE(i) + 0x9d4)
/** USB_OTGFS_DIEPDMA6_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DIEPDMA6_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA6_DMAADDR_M  (USB_OTGFS_DIEPDMA6_DMAADDR_V << USB_OTGFS_DIEPDMA6_DMAADDR_S)
#define USB_OTGFS_DIEPDMA6_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMA6_DMAADDR_S  0

/** USB_OTGFS_DTXFSTS6_REG register
 *  Device IN Endpoint Transmit FIFO Status Register 6  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DTXFSTS6_REG(i) (DR_REG_USB_BASE(i) + 0x9d8)
/** USB_OTGFS_DTXFSTS6_INEPTXFSPCAVAIL : RO; bitpos: [15:0]; default: 256;
 *  IN Endpoint TxFIFO Space Avail (INEPTxFSpcAvail)
 *
 *  Indicates the amount of free space available in the Endpoint TxFIFO.
 *
 *  Values are in terms of 32-bit words.
 *  - 16'h0: Endpoint TxFIFO is full
 *  - 16'h1: 1 word available
 *  - 16'h2: 2 words available
 *  - 16'hn: n words available (where 0  n  32,768)
 *  - 16'h8000: 32,768 words available
 *  - Others: Reserved
 */
#define USB_OTGFS_DTXFSTS6_INEPTXFSPCAVAIL    0x0000FFFFU
#define USB_OTGFS_DTXFSTS6_INEPTXFSPCAVAIL_M  (USB_OTGFS_DTXFSTS6_INEPTXFSPCAVAIL_V << USB_OTGFS_DTXFSTS6_INEPTXFSPCAVAIL_S)
#define USB_OTGFS_DTXFSTS6_INEPTXFSPCAVAIL_V  0x0000FFFFU
#define USB_OTGFS_DTXFSTS6_INEPTXFSPCAVAIL_S  0

/** USB_OTGFS_DIEPDMAB6_REG register
 *  Device IN Endpoint 6 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
#define USB_OTGFS_DIEPDMAB6_REG(i) (DR_REG_USB_BASE(i) + 0x9dc)
/** USB_OTGFS_DIEPDMAB6_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DIEPDMAB6_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB6_DMABUFFERADDR_M  (USB_OTGFS_DIEPDMAB6_DMABUFFERADDR_V << USB_OTGFS_DIEPDMAB6_DMABUFFERADDR_S)
#define USB_OTGFS_DIEPDMAB6_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DIEPDMAB6_DMABUFFERADDR_S  0

/** USB_OTGFS_DOEPCTL0_REG register
 *  Device Control OUT Endpoint 0 Control Register
 */
#define USB_OTGFS_DOEPCTL0_REG(i) (DR_REG_USB_BASE(i) + 0xb00)
/** USB_OTGFS_DOEPCTL0_MPS : RO; bitpos: [1:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The maximum packet size for control OUT endpoint 0 is the same as what is
 *  programmed in control IN Endpoint 0.
 *  - 2'b00: 64 bytes
 *  - 2'b01: 32 bytes
 *  - 2'b10: 16 bytes
 *  - 2'b11: 8 bytes
 */
#define USB_OTGFS_DOEPCTL0_MPS    0x00000003U
#define USB_OTGFS_DOEPCTL0_MPS_M  (USB_OTGFS_DOEPCTL0_MPS_V << USB_OTGFS_DOEPCTL0_MPS_S)
#define USB_OTGFS_DOEPCTL0_MPS_V  0x00000003U
#define USB_OTGFS_DOEPCTL0_MPS_S  0
/** USB_OTGFS_DOEPCTL0_USBACTEP : RO; bitpos: [15]; default: 1;
 *  USB Active Endpoint (USBActEP)
 *
 *  This bit is always set to 1, indicating that a control endpoint 0 is always active
 *  in all configurations and interfaces.
 */
#define USB_OTGFS_DOEPCTL0_USBACTEP    (BIT(15))
#define USB_OTGFS_DOEPCTL0_USBACTEP_M  (USB_OTGFS_DOEPCTL0_USBACTEP_V << USB_OTGFS_DOEPCTL0_USBACTEP_S)
#define USB_OTGFS_DOEPCTL0_USBACTEP_V  0x00000001U
#define USB_OTGFS_DOEPCTL0_USBACTEP_S  15
/** USB_OTGFS_DOEPCTL0_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit, the core
 *  stops receiving data, even If there is space in the RxFIFO to
 *  accommodate the incoming packet. Irrespective of this bit's
 *  setting, the core always responds to SETUP data packets with
 *  an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL0_NAKSTS    (BIT(17))
#define USB_OTGFS_DOEPCTL0_NAKSTS_M  (USB_OTGFS_DOEPCTL0_NAKSTS_V << USB_OTGFS_DOEPCTL0_NAKSTS_S)
#define USB_OTGFS_DOEPCTL0_NAKSTS_V  0x00000001U
#define USB_OTGFS_DOEPCTL0_NAKSTS_S  17
/** USB_OTGFS_DOEPCTL0_EPTYPE : RO; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  Hardcoded to 2'b00 for control.
 */
#define USB_OTGFS_DOEPCTL0_EPTYPE    0x00000003U
#define USB_OTGFS_DOEPCTL0_EPTYPE_M  (USB_OTGFS_DOEPCTL0_EPTYPE_V << USB_OTGFS_DOEPCTL0_EPTYPE_S)
#define USB_OTGFS_DOEPCTL0_EPTYPE_V  0x00000003U
#define USB_OTGFS_DOEPCTL0_EPTYPE_S  18
/** USB_OTGFS_DOEPCTL0_SNP : R/W; bitpos: [20]; default: 0;
 *  RESERVED
 */
#define USB_OTGFS_DOEPCTL0_SNP    (BIT(20))
#define USB_OTGFS_DOEPCTL0_SNP_M  (USB_OTGFS_DOEPCTL0_SNP_V << USB_OTGFS_DOEPCTL0_SNP_S)
#define USB_OTGFS_DOEPCTL0_SNP_V  0x00000001U
#define USB_OTGFS_DOEPCTL0_SNP_S  20
/** USB_OTGFS_DOEPCTL0_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  The application can only set this bit, and the core clears it, when
 *  a SETUP token is received for this endpoint. If a NAK bit or
 *  Global OUT NAK is Set along with this bit, the STALL bit takes
 *  priority. Irrespective of this bit's setting, the core always
 *  responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL0_STALL    (BIT(21))
#define USB_OTGFS_DOEPCTL0_STALL_M  (USB_OTGFS_DOEPCTL0_STALL_V << USB_OTGFS_DOEPCTL0_STALL_S)
#define USB_OTGFS_DOEPCTL0_STALL_V  0x00000001U
#define USB_OTGFS_DOEPCTL0_STALL_S  21
/** USB_OTGFS_DOEPCTL0_CNAK : R/W; bitpos: [26]; default: 0;
 *  Clear NAK (CNAK)
 *
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DOEPCTL0_CNAK    (BIT(26))
#define USB_OTGFS_DOEPCTL0_CNAK_M  (USB_OTGFS_DOEPCTL0_CNAK_V << USB_OTGFS_DOEPCTL0_CNAK_S)
#define USB_OTGFS_DOEPCTL0_CNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL0_CNAK_S  26
/** USB_OTGFS_DOEPCTL0_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *  Using this bit, the application can control the transmission of NAK handshakes on
 *  an endpoint.
 *  The core can also set bit on a Transfer Completed interrupt, or after a SETUP is
 *  received on the endpoint.
 */
#define USB_OTGFS_DOEPCTL0_SNAK    (BIT(27))
#define USB_OTGFS_DOEPCTL0_SNAK_M  (USB_OTGFS_DOEPCTL0_SNAK_V << USB_OTGFS_DOEPCTL0_SNAK_S)
#define USB_OTGFS_DOEPCTL0_SNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL0_SNAK_S  27
/** USB_OTGFS_DOEPCTL0_EPDIS : RO; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  The application cannot disable control OUT endpoint 0.
 */
#define USB_OTGFS_DOEPCTL0_EPDIS    (BIT(30))
#define USB_OTGFS_DOEPCTL0_EPDIS_M  (USB_OTGFS_DOEPCTL0_EPDIS_V << USB_OTGFS_DOEPCTL0_EPDIS_S)
#define USB_OTGFS_DOEPCTL0_EPDIS_V  0x00000001U
#define USB_OTGFS_DOEPCTL0_EPDIS_S  30
/** USB_OTGFS_DOEPCTL0_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *  - When Scatter/Gather DMA mode is enabled, for OUT endpoints this bit indicates
 *  that the descriptor structure and data buffer to receive data is setup.
 *  - When Scatter/Gather DMA mode is disabled (such as for buffer-pointer based DMA
 *  mode)this bit indicates that the application has allocated the memory to start
 *  receiving data from the USB.
 *  - The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  -- SETUP Phase Done
 *  -- Endpoint Disabled
 *  -- Transfer Completed
 *  Note: In DMA mode, this bit must be set for the core to transfer SETUP data packets
 *  into memory.
 */
#define USB_OTGFS_DOEPCTL0_EPENA    (BIT(31))
#define USB_OTGFS_DOEPCTL0_EPENA_M  (USB_OTGFS_DOEPCTL0_EPENA_V << USB_OTGFS_DOEPCTL0_EPENA_S)
#define USB_OTGFS_DOEPCTL0_EPENA_V  0x00000001U
#define USB_OTGFS_DOEPCTL0_EPENA_S  31

/** USB_OTGFS_DOEPINT0_REG register
 *  Device OUT Endpoint 0 Interrupt Register
 */
#define USB_OTGFS_DOEPINT0_REG(i) (DR_REG_USB_BASE(i) + 0xb08)
/** USB_OTGFS_DOEPINT0_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  When Scatter/Gather DMA mode is enabled
 *  - For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  - For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is Set.
 *  Note: In DMA mode, this bit must be set for the core to transfer SETUP data packets
 *  into memory. When Scatter/Gather DMA mode is disabled, this field indicates that
 *  the programmed transfer is complete on the AHB as well as on the USB, for this
 *  endpoint.
 */
#define USB_OTGFS_DOEPINT0_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DOEPINT0_XFERCOMPL_M  (USB_OTGFS_DOEPINT0_XFERCOMPL_V << USB_OTGFS_DOEPINT0_XFERCOMPL_S)
#define USB_OTGFS_DOEPINT0_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DOEPINT0_XFERCOMPL_S  0
/** USB_OTGFS_DOEPINT0_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DOEPINT0_EPDISBLD    (BIT(1))
#define USB_OTGFS_DOEPINT0_EPDISBLD_M  (USB_OTGFS_DOEPINT0_EPDISBLD_V << USB_OTGFS_DOEPINT0_EPDISBLD_S)
#define USB_OTGFS_DOEPINT0_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DOEPINT0_EPDISBLD_S  1
/** USB_OTGFS_DOEPINT0_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DOEPINT0_AHBERR    (BIT(2))
#define USB_OTGFS_DOEPINT0_AHBERR_M  (USB_OTGFS_DOEPINT0_AHBERR_V << USB_OTGFS_DOEPINT0_AHBERR_S)
#define USB_OTGFS_DOEPINT0_AHBERR_V  0x00000001U
#define USB_OTGFS_DOEPINT0_AHBERR_S  2
/** USB_OTGFS_DOEPINT0_SETUP : R/W; bitpos: [3]; default: 0;
 *  SETUP Phase Done (SetUp)
 *
 *  Applies to control OUT endpoints only.
 *
 *  Indicates that the SETUP phase for the control endpoint is
 *  complete and no more back-to-back SETUP packets were
 *  received for the current control transfer. On this interrupt, the
 *  application can decode the received SETUP data packet.
 */
#define USB_OTGFS_DOEPINT0_SETUP    (BIT(3))
#define USB_OTGFS_DOEPINT0_SETUP_M  (USB_OTGFS_DOEPINT0_SETUP_V << USB_OTGFS_DOEPINT0_SETUP_S)
#define USB_OTGFS_DOEPINT0_SETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT0_SETUP_S  3
/** USB_OTGFS_DOEPINT0_OUTTKNEPDIS : R/W; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
 *
 *  Applies only to control OUT endpoints.
 *
 *  Indicates that an OUT token was received when the endpoint
 *  was not yet enabled. This interrupt is asserted on the endpoint for which the OUT
 *  token was received.
 */
#define USB_OTGFS_DOEPINT0_OUTTKNEPDIS    (BIT(4))
#define USB_OTGFS_DOEPINT0_OUTTKNEPDIS_M  (USB_OTGFS_DOEPINT0_OUTTKNEPDIS_V << USB_OTGFS_DOEPINT0_OUTTKNEPDIS_S)
#define USB_OTGFS_DOEPINT0_OUTTKNEPDIS_V  0x00000001U
#define USB_OTGFS_DOEPINT0_OUTTKNEPDIS_S  4
/** USB_OTGFS_DOEPINT0_STSPHSERCVD : R/W; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write (StsPhseRcvd)
 *
 *  This interrupt is valid only for Control OUT endpoints and only in
 *  Scatter Gather DMA mode.
 *
 *  This interrupt is generated only after the core has transferred all
 *  the data that the host has sent during the data phase of a control
 *  write transfer, to the system memory buffer.
 *
 *  The interrupt indicates to the application that the host has
 *  switched from data phase to the status phase of a Control Write
 *  transfer. The application can use this interrupt to ACK or STALL
 *  the Status phase, after it has decoded the data phase. This is
 *  applicable only in case of Scatter Gather DMA mode.
 */
#define USB_OTGFS_DOEPINT0_STSPHSERCVD    (BIT(5))
#define USB_OTGFS_DOEPINT0_STSPHSERCVD_M  (USB_OTGFS_DOEPINT0_STSPHSERCVD_V << USB_OTGFS_DOEPINT0_STSPHSERCVD_S)
#define USB_OTGFS_DOEPINT0_STSPHSERCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT0_STSPHSERCVD_S  5
/** USB_OTGFS_DOEPINT0_BACK2BACKSETUP : R/W; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received (Back2BackSETup)
 *
 *  Applies to Control OUT endpoints only.
 *
 *  This bit indicates that the core has received more than three
 *  back-to-back SETUP packets for this particular endpoint. For
 *  information about handling this interrupt,
 */
#define USB_OTGFS_DOEPINT0_BACK2BACKSETUP    (BIT(6))
#define USB_OTGFS_DOEPINT0_BACK2BACKSETUP_M  (USB_OTGFS_DOEPINT0_BACK2BACKSETUP_V << USB_OTGFS_DOEPINT0_BACK2BACKSETUP_S)
#define USB_OTGFS_DOEPINT0_BACK2BACKSETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT0_BACK2BACKSETUP_S  6
/** USB_OTGFS_DOEPINT0_OUTPKTERR : R/W; bitpos: [8]; default: 0;
 *  OUT Packet Error (OutPktErr)
 *
 *  Applies to OUT endpoints Only
 *
 *  This interrupt is valid only when thresholding is enabled.
 *
 *  This interrupt is asserted when the core detects an overflow or a CRC error for
 *  non-Isochronous OUT packet.
 */
#define USB_OTGFS_DOEPINT0_OUTPKTERR    (BIT(8))
#define USB_OTGFS_DOEPINT0_OUTPKTERR_M  (USB_OTGFS_DOEPINT0_OUTPKTERR_V << USB_OTGFS_DOEPINT0_OUTPKTERR_S)
#define USB_OTGFS_DOEPINT0_OUTPKTERR_V  0x00000001U
#define USB_OTGFS_DOEPINT0_OUTPKTERR_S  8
/** USB_OTGFS_DOEPINT0_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the core to process, such as Host busy or DMA
 *  done.
 */
#define USB_OTGFS_DOEPINT0_BNAINTR    (BIT(9))
#define USB_OTGFS_DOEPINT0_BNAINTR_M  (USB_OTGFS_DOEPINT0_BNAINTR_V << USB_OTGFS_DOEPINT0_BNAINTR_S)
#define USB_OTGFS_DOEPINT0_BNAINTR_V  0x00000001U
#define USB_OTGFS_DOEPINT0_BNAINTR_S  9
/** USB_OTGFS_DOEPINT0_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DOEPINT0_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DOEPINT0_PKTDRPSTS_M  (USB_OTGFS_DOEPINT0_PKTDRPSTS_V << USB_OTGFS_DOEPINT0_PKTDRPSTS_S)
#define USB_OTGFS_DOEPINT0_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DOEPINT0_PKTDRPSTS_S  11
/** USB_OTGFS_DOEPINT0_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DOEPINT0_BBLEERR    (BIT(12))
#define USB_OTGFS_DOEPINT0_BBLEERR_M  (USB_OTGFS_DOEPINT0_BBLEERR_V << USB_OTGFS_DOEPINT0_BBLEERR_S)
#define USB_OTGFS_DOEPINT0_BBLEERR_V  0x00000001U
#define USB_OTGFS_DOEPINT0_BBLEERR_S  12
/** USB_OTGFS_DOEPINT0_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DOEPINT0_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DOEPINT0_NAKINTRPT_M  (USB_OTGFS_DOEPINT0_NAKINTRPT_V << USB_OTGFS_DOEPINT0_NAKINTRPT_S)
#define USB_OTGFS_DOEPINT0_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT0_NAKINTRPT_S  13
/** USB_OTGFS_DOEPINT0_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DOEPINT0_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DOEPINT0_NYETINTRPT_M  (USB_OTGFS_DOEPINT0_NYETINTRPT_V << USB_OTGFS_DOEPINT0_NYETINTRPT_S)
#define USB_OTGFS_DOEPINT0_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT0_NYETINTRPT_S  14
/** USB_OTGFS_DOEPINT0_STUPPKTRCVD : R/W; bitpos: [15]; default: 0;
 *  Setup Packet Received
 *
 *  Applicable for Control OUT Endpoints in only in the Buffer DMA Mode
 *
 *  Set by the controller, this bit indicates that this buffer holds 8 bytes of
 *  setup data. There is only one Setup packet per buffer. On receiving a
 *  Setup packet, the controller closes the buffer and disables the
 *  corresponding endpoint. The application has to re-enable the endpoint to
 *  receive any OUT data for the Control Transfer and reprogram the buffer
 *  start address.
 *
 *  Note: Because of the above behavior, the controller can receive any
 *  number of back to back setup packets and one buffer for every setup
 *  packet is used.
 *  - 1'b0: No Setup packet received
 *  - 1'b1: Setup packet received
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPINT0_STUPPKTRCVD    (BIT(15))
#define USB_OTGFS_DOEPINT0_STUPPKTRCVD_M  (USB_OTGFS_DOEPINT0_STUPPKTRCVD_V << USB_OTGFS_DOEPINT0_STUPPKTRCVD_S)
#define USB_OTGFS_DOEPINT0_STUPPKTRCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT0_STUPPKTRCVD_S  15

/** USB_OTGFS_DOEPTSIZ0_REG register
 *  Device OUT Endpoint 0 Transfer Size Register
 */
#define USB_OTGFS_DOEPTSIZ0_REG(i) (DR_REG_USB_BASE(i) + 0xb10)
/** USB_OTGFS_DOEPTSIZ0_XFERSIZE : R/W; bitpos: [6:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet is read from
 *  the RxFIFO and written to the external memory.
 */
#define USB_OTGFS_DOEPTSIZ0_XFERSIZE    0x0000007FU
#define USB_OTGFS_DOEPTSIZ0_XFERSIZE_M  (USB_OTGFS_DOEPTSIZ0_XFERSIZE_V << USB_OTGFS_DOEPTSIZ0_XFERSIZE_S)
#define USB_OTGFS_DOEPTSIZ0_XFERSIZE_V  0x0000007FU
#define USB_OTGFS_DOEPTSIZ0_XFERSIZE_S  0
/** USB_OTGFS_DOEPTSIZ0_PKTCNT : R/W; bitpos: [19]; default: 0;
 *  Packet Count (PktCnt)
 *
 *  This field is decremented to zero after a packet is written into the RxFIFO.
 */
#define USB_OTGFS_DOEPTSIZ0_PKTCNT    (BIT(19))
#define USB_OTGFS_DOEPTSIZ0_PKTCNT_M  (USB_OTGFS_DOEPTSIZ0_PKTCNT_V << USB_OTGFS_DOEPTSIZ0_PKTCNT_S)
#define USB_OTGFS_DOEPTSIZ0_PKTCNT_V  0x00000001U
#define USB_OTGFS_DOEPTSIZ0_PKTCNT_S  19
/** USB_OTGFS_DOEPTSIZ0_SUPCNT : R/W; bitpos: [30:29]; default: 0;
 *  SETUP Packet Count (SUPCnt)
 *
 *  This field specifies the number of back-to-back SETUP data packets the endpoint can
 *  receive.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 */
#define USB_OTGFS_DOEPTSIZ0_SUPCNT    0x00000003U
#define USB_OTGFS_DOEPTSIZ0_SUPCNT_M  (USB_OTGFS_DOEPTSIZ0_SUPCNT_V << USB_OTGFS_DOEPTSIZ0_SUPCNT_S)
#define USB_OTGFS_DOEPTSIZ0_SUPCNT_V  0x00000003U
#define USB_OTGFS_DOEPTSIZ0_SUPCNT_S  29

/** USB_OTGFS_DOEPDMA0_REG register
 *  Device OUT Endpoint 0 DMA Address Register
 */
#define USB_OTGFS_DOEPDMA0_REG(i) (DR_REG_USB_BASE(i) + 0xb14)
/** USB_OTGFS_DOEPDMA0_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DOEPDMA0_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA0_DMAADDR_M  (USB_OTGFS_DOEPDMA0_DMAADDR_V << USB_OTGFS_DOEPDMA0_DMAADDR_S)
#define USB_OTGFS_DOEPDMA0_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA0_DMAADDR_S  0

/** USB_OTGFS_DOEPDMAB0_REG register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
#define USB_OTGFS_DOEPDMAB0_REG(i) (DR_REG_USB_BASE(i) + 0xb1c)
/** USB_OTGFS_DOEPDMAB0_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DOEPDMAB0_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB0_DMABUFFERADDR_M  (USB_OTGFS_DOEPDMAB0_DMABUFFERADDR_V << USB_OTGFS_DOEPDMAB0_DMABUFFERADDR_S)
#define USB_OTGFS_DOEPDMAB0_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB0_DMABUFFERADDR_S  0

/** USB_OTGFS_DOEPCTL1_REG register
 *  Device Control OUT Endpoint 1 Control Register
 */
#define USB_OTGFS_DOEPCTL1_REG(i) (DR_REG_USB_BASE(i) + 0xb20)
/** USB_OTGFS_DOEPCTL1_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the current
 *  logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DOEPCTL1_MPS    0x000007FFU
#define USB_OTGFS_DOEPCTL1_MPS_M  (USB_OTGFS_DOEPCTL1_MPS_V << USB_OTGFS_DOEPCTL1_MPS_S)
#define USB_OTGFS_DOEPCTL1_MPS_V  0x000007FFU
#define USB_OTGFS_DOEPCTL1_MPS_S  0
/** USB_OTGFS_DOEPCTL1_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DOEPCTL1_USBACTEP    (BIT(15))
#define USB_OTGFS_DOEPCTL1_USBACTEP_M  (USB_OTGFS_DOEPCTL1_USBACTEP_V << USB_OTGFS_DOEPCTL1_USBACTEP_S)
#define USB_OTGFS_DOEPCTL1_USBACTEP_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_USBACTEP_S  15
/** USB_OTGFS_DOEPCTL1_DPID : RO; bitpos: [16]; default: 0;
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable for both Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Reset: 1'b0
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Indicates the (micro)frame number in which the core transmits/receives
 *  isochronous data for this endpoint. The application must program the even/odd
 *  (micro)frame number in which it intends to transmit/receive isochronous data for
 *  this endpoint using the SetEvnFr and SetOddFr fields in this register.
 *  -- 1'b0: Even (micro)frame
 *  -- 1'b1: Odd (micro)frame
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame
 *  in which data is received is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL1_DPID    (BIT(16))
#define USB_OTGFS_DOEPCTL1_DPID_M  (USB_OTGFS_DOEPCTL1_DPID_V << USB_OTGFS_DOEPCTL1_DPID_S)
#define USB_OTGFS_DOEPCTL1_DPID_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_DPID_S  16
/** USB_OTGFS_DOEPCTL1_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL1_NAKSTS    (BIT(17))
#define USB_OTGFS_DOEPCTL1_NAKSTS_M  (USB_OTGFS_DOEPCTL1_NAKSTS_V << USB_OTGFS_DOEPCTL1_NAKSTS_S)
#define USB_OTGFS_DOEPCTL1_NAKSTS_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_NAKSTS_S  17
/** USB_OTGFS_DOEPCTL1_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DOEPCTL1_EPTYPE    0x00000003U
#define USB_OTGFS_DOEPCTL1_EPTYPE_M  (USB_OTGFS_DOEPCTL1_EPTYPE_V << USB_OTGFS_DOEPCTL1_EPTYPE_S)
#define USB_OTGFS_DOEPCTL1_EPTYPE_V  0x00000003U
#define USB_OTGFS_DOEPCTL1_EPTYPE_S  18
/** USB_OTGFS_DOEPCTL1_SNP : R/W; bitpos: [20]; default: 0;
 *  RESERVED
 */
#define USB_OTGFS_DOEPCTL1_SNP    (BIT(20))
#define USB_OTGFS_DOEPCTL1_SNP_M  (USB_OTGFS_DOEPCTL1_SNP_V << USB_OTGFS_DOEPCTL1_SNP_S)
#define USB_OTGFS_DOEPCTL1_SNP_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_SNP_S  20
/** USB_OTGFS_DOEPCTL1_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL1_STALL    (BIT(21))
#define USB_OTGFS_DOEPCTL1_STALL_M  (USB_OTGFS_DOEPCTL1_STALL_V << USB_OTGFS_DOEPCTL1_STALL_S)
#define USB_OTGFS_DOEPCTL1_STALL_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_STALL_S  21
/** USB_OTGFS_DOEPCTL1_CNAK : R/W; bitpos: [26]; default: 0;
 *
 *  Clear NAK (CNAK)
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DOEPCTL1_CNAK    (BIT(26))
#define USB_OTGFS_DOEPCTL1_CNAK_M  (USB_OTGFS_DOEPCTL1_CNAK_V << USB_OTGFS_DOEPCTL1_CNAK_S)
#define USB_OTGFS_DOEPCTL1_CNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_CNAK_S  26
/** USB_OTGFS_DOEPCTL1_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK
 *  handshakes on an endpoint. The core can also set this bit for an
 *  endpoint after a SETUP packet is received on that endpoint.
 */
#define USB_OTGFS_DOEPCTL1_SNAK    (BIT(27))
#define USB_OTGFS_DOEPCTL1_SNAK_M  (USB_OTGFS_DOEPCTL1_SNAK_V << USB_OTGFS_DOEPCTL1_SNAK_S)
#define USB_OTGFS_DOEPCTL1_SNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_SNAK_S  27
/** USB_OTGFS_DOEPCTL1_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  Set DATA0 PID (SetD0PID)
 *  - Applies to interrupt/bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA0.
 *  - This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  In non-Scatter/Gather DMA mode: Set Even (micro)frame (SetEvenFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the Even/Odd (micro)frame (EO_FrNum) field to even
 *  (micro)frame.
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which
 *  to receive data is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL1_SETD0PID    (BIT(28))
#define USB_OTGFS_DOEPCTL1_SETD0PID_M  (USB_OTGFS_DOEPCTL1_SETD0PID_V << USB_OTGFS_DOEPCTL1_SETD0PID_S)
#define USB_OTGFS_DOEPCTL1_SETD0PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_SETD0PID_S  28
/** USB_OTGFS_DOEPCTL1_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  Set DATA1 PID (SetD1PID)
 *  - Applies to interrupt and bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA1.
 *  - This field is applicable both for scatter-gather DMA mode and non scatter-gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  Set Odd (micro)frame (SetOddFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the even and odd (micro)frame (EO_FrNum) field to odd
 *  (micro)frame.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL1_SETD1PID    (BIT(29))
#define USB_OTGFS_DOEPCTL1_SETD1PID_M  (USB_OTGFS_DOEPCTL1_SETD1PID_V << USB_OTGFS_DOEPCTL1_SETD1PID_S)
#define USB_OTGFS_DOEPCTL1_SETD1PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_SETD1PID_S  29
/** USB_OTGFS_DOEPCTL1_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DOEPCTL1_EPDIS    (BIT(30))
#define USB_OTGFS_DOEPCTL1_EPDIS_M  (USB_OTGFS_DOEPCTL1_EPDIS_V << USB_OTGFS_DOEPCTL1_EPDIS_S)
#define USB_OTGFS_DOEPCTL1_EPDIS_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_EPDIS_S  30
/** USB_OTGFS_DOEPCTL1_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  When Scatter/Gather DMA mode is enabled,
 *  - For IN endpoints this bit indicates that the descriptor structure and data buffer
 *  with data ready to transmit is setup.
 *  - For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  - For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  - For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  - SETUP Phase Done
 *  - Endpoint Disabled
 *  - Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DOEPCTL1_EPENA    (BIT(31))
#define USB_OTGFS_DOEPCTL1_EPENA_M  (USB_OTGFS_DOEPCTL1_EPENA_V << USB_OTGFS_DOEPCTL1_EPENA_S)
#define USB_OTGFS_DOEPCTL1_EPENA_V  0x00000001U
#define USB_OTGFS_DOEPCTL1_EPENA_S  31

/** USB_OTGFS_DOEPINT1_REG register
 *  Device OUT Endpoint 1 Interrupt Register
 */
#define USB_OTGFS_DOEPINT1_REG(i) (DR_REG_USB_BASE(i) + 0xb28)
/** USB_OTGFS_DOEPINT1_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is Set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DOEPINT1_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DOEPINT1_XFERCOMPL_M  (USB_OTGFS_DOEPINT1_XFERCOMPL_V << USB_OTGFS_DOEPINT1_XFERCOMPL_S)
#define USB_OTGFS_DOEPINT1_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DOEPINT1_XFERCOMPL_S  0
/** USB_OTGFS_DOEPINT1_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DOEPINT1_EPDISBLD    (BIT(1))
#define USB_OTGFS_DOEPINT1_EPDISBLD_M  (USB_OTGFS_DOEPINT1_EPDISBLD_V << USB_OTGFS_DOEPINT1_EPDISBLD_S)
#define USB_OTGFS_DOEPINT1_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DOEPINT1_EPDISBLD_S  1
/** USB_OTGFS_DOEPINT1_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DOEPINT1_AHBERR    (BIT(2))
#define USB_OTGFS_DOEPINT1_AHBERR_M  (USB_OTGFS_DOEPINT1_AHBERR_V << USB_OTGFS_DOEPINT1_AHBERR_S)
#define USB_OTGFS_DOEPINT1_AHBERR_V  0x00000001U
#define USB_OTGFS_DOEPINT1_AHBERR_S  2
/** USB_OTGFS_DOEPINT1_SETUP : R/W; bitpos: [3]; default: 0;
 *  SETUP Phase Done (SetUp)
 *
 *  Applies to control OUT endpoints only.
 *
 *  Indicates that the SETUP phase for the control endpoint is
 *  complete and no more back-to-back SETUP packets were
 *  received for the current control transfer. On this interrupt, the
 *  application can decode the received SETUP data packet.
 */
#define USB_OTGFS_DOEPINT1_SETUP    (BIT(3))
#define USB_OTGFS_DOEPINT1_SETUP_M  (USB_OTGFS_DOEPINT1_SETUP_V << USB_OTGFS_DOEPINT1_SETUP_S)
#define USB_OTGFS_DOEPINT1_SETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT1_SETUP_S  3
/** USB_OTGFS_DOEPINT1_OUTTKNEPDIS : R/W; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
 *
 *  Applies only to control OUT endpoints.
 *
 *  Indicates that an OUT token was received when the endpoint was not yet enabled.
 *  This interrupt is asserted on the endpoint for which the OUT token was received.
 */
#define USB_OTGFS_DOEPINT1_OUTTKNEPDIS    (BIT(4))
#define USB_OTGFS_DOEPINT1_OUTTKNEPDIS_M  (USB_OTGFS_DOEPINT1_OUTTKNEPDIS_V << USB_OTGFS_DOEPINT1_OUTTKNEPDIS_S)
#define USB_OTGFS_DOEPINT1_OUTTKNEPDIS_V  0x00000001U
#define USB_OTGFS_DOEPINT1_OUTTKNEPDIS_S  4
/** USB_OTGFS_DOEPINT1_STSPHSERCVD : R/W; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write (StsPhseRcvd)
 *
 *  This interrupt is valid only for Control OUT endpoints and only in
 *  Scatter Gather DMA mode.
 *
 *  This interrupt is generated only after the core has transferred all
 *  the data that the host has sent during the data phase of a control
 *  write transfer, to the system memory buffer.
 *
 *  The interrupt indicates to the application that the host has
 *  switched from data phase to the status phase of a Control Write
 *  transfer. The application can use this interrupt to ACK or STALL
 *  the Status phase, after it has decoded the data phase. This is
 *  applicable only in Case of Scatter Gather DMA mode.
 */
#define USB_OTGFS_DOEPINT1_STSPHSERCVD    (BIT(5))
#define USB_OTGFS_DOEPINT1_STSPHSERCVD_M  (USB_OTGFS_DOEPINT1_STSPHSERCVD_V << USB_OTGFS_DOEPINT1_STSPHSERCVD_S)
#define USB_OTGFS_DOEPINT1_STSPHSERCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT1_STSPHSERCVD_S  5
/** USB_OTGFS_DOEPINT1_BACK2BACKSETUP : R/W; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received (Back2BackSETup)
 *
 *  Applies to Control OUT endpoints only.
 *
 *  This bit indicates that the core has received more than three
 *  back-to-back SETUP packets for this particular endpoint. For
 *  information about handling this interrupt,
 */
#define USB_OTGFS_DOEPINT1_BACK2BACKSETUP    (BIT(6))
#define USB_OTGFS_DOEPINT1_BACK2BACKSETUP_M  (USB_OTGFS_DOEPINT1_BACK2BACKSETUP_V << USB_OTGFS_DOEPINT1_BACK2BACKSETUP_S)
#define USB_OTGFS_DOEPINT1_BACK2BACKSETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT1_BACK2BACKSETUP_S  6
/** USB_OTGFS_DOEPINT1_OUTPKTERR : R/W; bitpos: [8]; default: 0;
 *  OUT Packet Error (OutPktErr)
 *
 *  Applies to OUT endpoints Only
 *
 *  This interrupt is valid only when thresholding is enabled. This interrupt is
 *  asserted when the
 *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
 */
#define USB_OTGFS_DOEPINT1_OUTPKTERR    (BIT(8))
#define USB_OTGFS_DOEPINT1_OUTPKTERR_M  (USB_OTGFS_DOEPINT1_OUTPKTERR_V << USB_OTGFS_DOEPINT1_OUTPKTERR_S)
#define USB_OTGFS_DOEPINT1_OUTPKTERR_V  0x00000001U
#define USB_OTGFS_DOEPINT1_OUTPKTERR_S  8
/** USB_OTGFS_DOEPINT1_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process, such as Host busy or DMA
 *  done
 */
#define USB_OTGFS_DOEPINT1_BNAINTR    (BIT(9))
#define USB_OTGFS_DOEPINT1_BNAINTR_M  (USB_OTGFS_DOEPINT1_BNAINTR_V << USB_OTGFS_DOEPINT1_BNAINTR_S)
#define USB_OTGFS_DOEPINT1_BNAINTR_V  0x00000001U
#define USB_OTGFS_DOEPINT1_BNAINTR_S  9
/** USB_OTGFS_DOEPINT1_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DOEPINT1_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DOEPINT1_PKTDRPSTS_M  (USB_OTGFS_DOEPINT1_PKTDRPSTS_V << USB_OTGFS_DOEPINT1_PKTDRPSTS_S)
#define USB_OTGFS_DOEPINT1_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DOEPINT1_PKTDRPSTS_S  11
/** USB_OTGFS_DOEPINT1_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DOEPINT1_BBLEERR    (BIT(12))
#define USB_OTGFS_DOEPINT1_BBLEERR_M  (USB_OTGFS_DOEPINT1_BBLEERR_V << USB_OTGFS_DOEPINT1_BBLEERR_S)
#define USB_OTGFS_DOEPINT1_BBLEERR_V  0x00000001U
#define USB_OTGFS_DOEPINT1_BBLEERR_S  12
/** USB_OTGFS_DOEPINT1_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DOEPINT1_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DOEPINT1_NAKINTRPT_M  (USB_OTGFS_DOEPINT1_NAKINTRPT_V << USB_OTGFS_DOEPINT1_NAKINTRPT_S)
#define USB_OTGFS_DOEPINT1_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT1_NAKINTRPT_S  13
/** USB_OTGFS_DOEPINT1_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DOEPINT1_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DOEPINT1_NYETINTRPT_M  (USB_OTGFS_DOEPINT1_NYETINTRPT_V << USB_OTGFS_DOEPINT1_NYETINTRPT_S)
#define USB_OTGFS_DOEPINT1_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT1_NYETINTRPT_S  14
/** USB_OTGFS_DOEPINT1_STUPPKTRCVD : R/W; bitpos: [15]; default: 0;
 *  Setup Packet Received
 *
 *  Applicable for Control OUT Endpoints in only in the Buffer DMA Mode
 *
 *  Set by the controller, this bit indicates that this buffer holds 8 bytes of
 *  setup data. There is only one Setup packet per buffer. On receiving a
 *  Setup packet, the controller closes the buffer and disables the
 *  corresponding endpoint. The application has to re-enable the endpoint to
 *  receive any OUT data for the Control Transfer and reprogram the buffer
 *  start address.
 *
 *  Note: Because of the above behavior, the controller can receive any
 *  number of back to back setup packets and one buffer for every setup
 *  packet is used.
 *  - 1'b0: No Setup packet received
 *  - 1'b1: Setup packet received
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPINT1_STUPPKTRCVD    (BIT(15))
#define USB_OTGFS_DOEPINT1_STUPPKTRCVD_M  (USB_OTGFS_DOEPINT1_STUPPKTRCVD_V << USB_OTGFS_DOEPINT1_STUPPKTRCVD_S)
#define USB_OTGFS_DOEPINT1_STUPPKTRCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT1_STUPPKTRCVD_S  15

/** USB_OTGFS_DOEPTSIZ1_REG register
 *  Device OUT Endpoint 1 Transfer Size Register
 */
#define USB_OTGFS_DOEPTSIZ1_REG(i) (DR_REG_USB_BASE(i) + 0xb30)
/** USB_OTGFS_DOEPTSIZ1_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet is read from
 *  the RxFIFO and written to the external memory.
 */
#define USB_OTGFS_DOEPTSIZ1_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ1_XFERSIZE_M  (USB_OTGFS_DOEPTSIZ1_XFERSIZE_V << USB_OTGFS_DOEPTSIZ1_XFERSIZE_S)
#define USB_OTGFS_DOEPTSIZ1_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ1_XFERSIZE_S  0
/** USB_OTGFS_DOEPTSIZ1_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *  This field is decremented to zero after a packet is written into the RxFIFO.
 */
#define USB_OTGFS_DOEPTSIZ1_PKTCNT    0x0000007FU
#define USB_OTGFS_DOEPTSIZ1_PKTCNT_M  (USB_OTGFS_DOEPTSIZ1_PKTCNT_V << USB_OTGFS_DOEPTSIZ1_PKTCNT_S)
#define USB_OTGFS_DOEPTSIZ1_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DOEPTSIZ1_PKTCNT_S  19
/** USB_OTGFS_DOEPTSIZ1_RXDPID : RO; bitpos: [30:29]; default: 0;
 *  RxDPID
 *
 *  Applies to isochronous OUT endpoints only.
 *
 *  This is the data PID received in the last packet for this endpoint.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA
 *  SETUP Packet Count (SUPCnt)
 *
 *  Applies to control OUT Endpoints only.
 *
 *  This field specifies the number of back-to-back SETUP data
 *  packets the endpoint can receive.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 */
#define USB_OTGFS_DOEPTSIZ1_RXDPID    0x00000003U
#define USB_OTGFS_DOEPTSIZ1_RXDPID_M  (USB_OTGFS_DOEPTSIZ1_RXDPID_V << USB_OTGFS_DOEPTSIZ1_RXDPID_S)
#define USB_OTGFS_DOEPTSIZ1_RXDPID_V  0x00000003U
#define USB_OTGFS_DOEPTSIZ1_RXDPID_S  29

/** USB_OTGFS_DOEPDMA1_REG register
 *  Device OUT Endpoint 1 DMA Address Register
 */
#define USB_OTGFS_DOEPDMA1_REG(i) (DR_REG_USB_BASE(i) + 0xb34)
/** USB_OTGFS_DOEPDMA1_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DOEPDMA1_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA1_DMAADDR_M  (USB_OTGFS_DOEPDMA1_DMAADDR_V << USB_OTGFS_DOEPDMA1_DMAADDR_S)
#define USB_OTGFS_DOEPDMA1_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA1_DMAADDR_S  0

/** USB_OTGFS_DOEPDMAB1_REG register
 *  Device OUT Endpoint 1 Buffer Address Register
 */
#define USB_OTGFS_DOEPDMAB1_REG(i) (DR_REG_USB_BASE(i) + 0xb3c)
/** USB_OTGFS_DOEPDMAB1_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DOEPDMAB1_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB1_DMABUFFERADDR_M  (USB_OTGFS_DOEPDMAB1_DMABUFFERADDR_V << USB_OTGFS_DOEPDMAB1_DMABUFFERADDR_S)
#define USB_OTGFS_DOEPDMAB1_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB1_DMABUFFERADDR_S  0

/** USB_OTGFS_DOEPCTL2_REG register
 *  Device Control OUT Endpoint 2 Control Register
 */
#define USB_OTGFS_DOEPCTL2_REG(i) (DR_REG_USB_BASE(i) + 0xb40)
/** USB_OTGFS_DOEPCTL2_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the current
 *  logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DOEPCTL2_MPS    0x000007FFU
#define USB_OTGFS_DOEPCTL2_MPS_M  (USB_OTGFS_DOEPCTL2_MPS_V << USB_OTGFS_DOEPCTL2_MPS_S)
#define USB_OTGFS_DOEPCTL2_MPS_V  0x000007FFU
#define USB_OTGFS_DOEPCTL2_MPS_S  0
/** USB_OTGFS_DOEPCTL2_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DOEPCTL2_USBACTEP    (BIT(15))
#define USB_OTGFS_DOEPCTL2_USBACTEP_M  (USB_OTGFS_DOEPCTL2_USBACTEP_V << USB_OTGFS_DOEPCTL2_USBACTEP_S)
#define USB_OTGFS_DOEPCTL2_USBACTEP_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_USBACTEP_S  15
/** USB_OTGFS_DOEPCTL2_DPID : RO; bitpos: [16]; default: 0;
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable for both Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Reset: 1'b0
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Indicates the (micro)frame number in which the core transmits/receives
 *  isochronous data for this endpoint. The application must program the even/odd
 *  (micro)frame number in which it intends to transmit/receive isochronous data for
 *  this endpoint using the SetEvnFr and SetOddFr fields in this register.
 *  -- 1'b0: Even (micro)frame
 *  -- 1'b1: Odd (micro)frame
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame
 *  in which data is received is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL2_DPID    (BIT(16))
#define USB_OTGFS_DOEPCTL2_DPID_M  (USB_OTGFS_DOEPCTL2_DPID_V << USB_OTGFS_DOEPCTL2_DPID_S)
#define USB_OTGFS_DOEPCTL2_DPID_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_DPID_S  16
/** USB_OTGFS_DOEPCTL2_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL2_NAKSTS    (BIT(17))
#define USB_OTGFS_DOEPCTL2_NAKSTS_M  (USB_OTGFS_DOEPCTL2_NAKSTS_V << USB_OTGFS_DOEPCTL2_NAKSTS_S)
#define USB_OTGFS_DOEPCTL2_NAKSTS_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_NAKSTS_S  17
/** USB_OTGFS_DOEPCTL2_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DOEPCTL2_EPTYPE    0x00000003U
#define USB_OTGFS_DOEPCTL2_EPTYPE_M  (USB_OTGFS_DOEPCTL2_EPTYPE_V << USB_OTGFS_DOEPCTL2_EPTYPE_S)
#define USB_OTGFS_DOEPCTL2_EPTYPE_V  0x00000003U
#define USB_OTGFS_DOEPCTL2_EPTYPE_S  18
/** USB_OTGFS_DOEPCTL2_SNP : R/W; bitpos: [20]; default: 0;
 *  RESERVED
 */
#define USB_OTGFS_DOEPCTL2_SNP    (BIT(20))
#define USB_OTGFS_DOEPCTL2_SNP_M  (USB_OTGFS_DOEPCTL2_SNP_V << USB_OTGFS_DOEPCTL2_SNP_S)
#define USB_OTGFS_DOEPCTL2_SNP_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_SNP_S  20
/** USB_OTGFS_DOEPCTL2_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL2_STALL    (BIT(21))
#define USB_OTGFS_DOEPCTL2_STALL_M  (USB_OTGFS_DOEPCTL2_STALL_V << USB_OTGFS_DOEPCTL2_STALL_S)
#define USB_OTGFS_DOEPCTL2_STALL_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_STALL_S  21
/** USB_OTGFS_DOEPCTL2_CNAK : R/W; bitpos: [26]; default: 0;
 *
 *  Clear NAK (CNAK)
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DOEPCTL2_CNAK    (BIT(26))
#define USB_OTGFS_DOEPCTL2_CNAK_M  (USB_OTGFS_DOEPCTL2_CNAK_V << USB_OTGFS_DOEPCTL2_CNAK_S)
#define USB_OTGFS_DOEPCTL2_CNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_CNAK_S  26
/** USB_OTGFS_DOEPCTL2_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK
 *  handshakes on an endpoint. The core can also set this bit for an
 *  endpoint after a SETUP packet is received on that endpoint.
 */
#define USB_OTGFS_DOEPCTL2_SNAK    (BIT(27))
#define USB_OTGFS_DOEPCTL2_SNAK_M  (USB_OTGFS_DOEPCTL2_SNAK_V << USB_OTGFS_DOEPCTL2_SNAK_S)
#define USB_OTGFS_DOEPCTL2_SNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_SNAK_S  27
/** USB_OTGFS_DOEPCTL2_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  Set DATA0 PID (SetD0PID)
 *  - Applies to interrupt/bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA0.
 *  - This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  In non-Scatter/Gather DMA mode: Set Even (micro)frame (SetEvenFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the Even/Odd (micro)frame (EO_FrNum) field to even
 *  (micro)frame.
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which
 *  to receive data is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL2_SETD0PID    (BIT(28))
#define USB_OTGFS_DOEPCTL2_SETD0PID_M  (USB_OTGFS_DOEPCTL2_SETD0PID_V << USB_OTGFS_DOEPCTL2_SETD0PID_S)
#define USB_OTGFS_DOEPCTL2_SETD0PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_SETD0PID_S  28
/** USB_OTGFS_DOEPCTL2_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  Set DATA1 PID (SetD1PID)
 *  - Applies to interrupt and bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA1.
 *  - This field is applicable both for scatter-gather DMA mode and non scatter-gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  Set Odd (micro)frame (SetOddFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the even and odd (micro)frame (EO_FrNum) field to odd
 *  (micro)frame.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL2_SETD1PID    (BIT(29))
#define USB_OTGFS_DOEPCTL2_SETD1PID_M  (USB_OTGFS_DOEPCTL2_SETD1PID_V << USB_OTGFS_DOEPCTL2_SETD1PID_S)
#define USB_OTGFS_DOEPCTL2_SETD1PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_SETD1PID_S  29
/** USB_OTGFS_DOEPCTL2_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DOEPCTL2_EPDIS    (BIT(30))
#define USB_OTGFS_DOEPCTL2_EPDIS_M  (USB_OTGFS_DOEPCTL2_EPDIS_V << USB_OTGFS_DOEPCTL2_EPDIS_S)
#define USB_OTGFS_DOEPCTL2_EPDIS_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_EPDIS_S  30
/** USB_OTGFS_DOEPCTL2_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  When Scatter/Gather DMA mode is enabled,
 *  - For IN endpoints this bit indicates that the descriptor structure and data buffer
 *  with data ready to transmit is setup.
 *  - For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  - For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  - For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  - SETUP Phase Done
 *  - Endpoint Disabled
 *  - Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DOEPCTL2_EPENA    (BIT(31))
#define USB_OTGFS_DOEPCTL2_EPENA_M  (USB_OTGFS_DOEPCTL2_EPENA_V << USB_OTGFS_DOEPCTL2_EPENA_S)
#define USB_OTGFS_DOEPCTL2_EPENA_V  0x00000001U
#define USB_OTGFS_DOEPCTL2_EPENA_S  31

/** USB_OTGFS_DOEPINT2_REG register
 *  Device OUT Endpoint 2 Interrupt Register
 */
#define USB_OTGFS_DOEPINT2_REG(i) (DR_REG_USB_BASE(i) + 0xb48)
/** USB_OTGFS_DOEPINT2_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is Set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DOEPINT2_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DOEPINT2_XFERCOMPL_M  (USB_OTGFS_DOEPINT2_XFERCOMPL_V << USB_OTGFS_DOEPINT2_XFERCOMPL_S)
#define USB_OTGFS_DOEPINT2_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DOEPINT2_XFERCOMPL_S  0
/** USB_OTGFS_DOEPINT2_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DOEPINT2_EPDISBLD    (BIT(1))
#define USB_OTGFS_DOEPINT2_EPDISBLD_M  (USB_OTGFS_DOEPINT2_EPDISBLD_V << USB_OTGFS_DOEPINT2_EPDISBLD_S)
#define USB_OTGFS_DOEPINT2_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DOEPINT2_EPDISBLD_S  1
/** USB_OTGFS_DOEPINT2_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DOEPINT2_AHBERR    (BIT(2))
#define USB_OTGFS_DOEPINT2_AHBERR_M  (USB_OTGFS_DOEPINT2_AHBERR_V << USB_OTGFS_DOEPINT2_AHBERR_S)
#define USB_OTGFS_DOEPINT2_AHBERR_V  0x00000001U
#define USB_OTGFS_DOEPINT2_AHBERR_S  2
/** USB_OTGFS_DOEPINT2_SETUP : R/W; bitpos: [3]; default: 0;
 *  SETUP Phase Done (SetUp)
 *
 *  Applies to control OUT endpoints only.
 *
 *  Indicates that the SETUP phase for the control endpoint is
 *  complete and no more back-to-back SETUP packets were
 *  received for the current control transfer. On this interrupt, the
 *  application can decode the received SETUP data packet.
 */
#define USB_OTGFS_DOEPINT2_SETUP    (BIT(3))
#define USB_OTGFS_DOEPINT2_SETUP_M  (USB_OTGFS_DOEPINT2_SETUP_V << USB_OTGFS_DOEPINT2_SETUP_S)
#define USB_OTGFS_DOEPINT2_SETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT2_SETUP_S  3
/** USB_OTGFS_DOEPINT2_OUTTKNEPDIS : R/W; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
 *
 *  Applies only to control OUT endpoints.
 *
 *  Indicates that an OUT token was received when the endpoint was not yet enabled.
 *  This interrupt is asserted on the endpoint for which the OUT token was received.
 */
#define USB_OTGFS_DOEPINT2_OUTTKNEPDIS    (BIT(4))
#define USB_OTGFS_DOEPINT2_OUTTKNEPDIS_M  (USB_OTGFS_DOEPINT2_OUTTKNEPDIS_V << USB_OTGFS_DOEPINT2_OUTTKNEPDIS_S)
#define USB_OTGFS_DOEPINT2_OUTTKNEPDIS_V  0x00000001U
#define USB_OTGFS_DOEPINT2_OUTTKNEPDIS_S  4
/** USB_OTGFS_DOEPINT2_STSPHSERCVD : R/W; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write (StsPhseRcvd)
 *
 *  This interrupt is valid only for Control OUT endpoints and only in
 *  Scatter Gather DMA mode.
 *
 *  This interrupt is generated only after the core has transferred all
 *  the data that the host has sent during the data phase of a control
 *  write transfer, to the system memory buffer.
 *
 *  The interrupt indicates to the application that the host has
 *  switched from data phase to the status phase of a Control Write
 *  transfer. The application can use this interrupt to ACK or STALL
 *  the Status phase, after it has decoded the data phase. This is
 *  applicable only in Case of Scatter Gather DMA mode.
 */
#define USB_OTGFS_DOEPINT2_STSPHSERCVD    (BIT(5))
#define USB_OTGFS_DOEPINT2_STSPHSERCVD_M  (USB_OTGFS_DOEPINT2_STSPHSERCVD_V << USB_OTGFS_DOEPINT2_STSPHSERCVD_S)
#define USB_OTGFS_DOEPINT2_STSPHSERCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT2_STSPHSERCVD_S  5
/** USB_OTGFS_DOEPINT2_BACK2BACKSETUP : R/W; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received (Back2BackSETup)
 *
 *  Applies to Control OUT endpoints only.
 *
 *  This bit indicates that the core has received more than three
 *  back-to-back SETUP packets for this particular endpoint. For
 *  information about handling this interrupt,
 */
#define USB_OTGFS_DOEPINT2_BACK2BACKSETUP    (BIT(6))
#define USB_OTGFS_DOEPINT2_BACK2BACKSETUP_M  (USB_OTGFS_DOEPINT2_BACK2BACKSETUP_V << USB_OTGFS_DOEPINT2_BACK2BACKSETUP_S)
#define USB_OTGFS_DOEPINT2_BACK2BACKSETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT2_BACK2BACKSETUP_S  6
/** USB_OTGFS_DOEPINT2_OUTPKTERR : R/W; bitpos: [8]; default: 0;
 *  OUT Packet Error (OutPktErr)
 *
 *  Applies to OUT endpoints Only
 *
 *  This interrupt is valid only when thresholding is enabled. This interrupt is
 *  asserted when the
 *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
 */
#define USB_OTGFS_DOEPINT2_OUTPKTERR    (BIT(8))
#define USB_OTGFS_DOEPINT2_OUTPKTERR_M  (USB_OTGFS_DOEPINT2_OUTPKTERR_V << USB_OTGFS_DOEPINT2_OUTPKTERR_S)
#define USB_OTGFS_DOEPINT2_OUTPKTERR_V  0x00000001U
#define USB_OTGFS_DOEPINT2_OUTPKTERR_S  8
/** USB_OTGFS_DOEPINT2_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process, such as Host busy or DMA
 *  done
 */
#define USB_OTGFS_DOEPINT2_BNAINTR    (BIT(9))
#define USB_OTGFS_DOEPINT2_BNAINTR_M  (USB_OTGFS_DOEPINT2_BNAINTR_V << USB_OTGFS_DOEPINT2_BNAINTR_S)
#define USB_OTGFS_DOEPINT2_BNAINTR_V  0x00000001U
#define USB_OTGFS_DOEPINT2_BNAINTR_S  9
/** USB_OTGFS_DOEPINT2_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DOEPINT2_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DOEPINT2_PKTDRPSTS_M  (USB_OTGFS_DOEPINT2_PKTDRPSTS_V << USB_OTGFS_DOEPINT2_PKTDRPSTS_S)
#define USB_OTGFS_DOEPINT2_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DOEPINT2_PKTDRPSTS_S  11
/** USB_OTGFS_DOEPINT2_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DOEPINT2_BBLEERR    (BIT(12))
#define USB_OTGFS_DOEPINT2_BBLEERR_M  (USB_OTGFS_DOEPINT2_BBLEERR_V << USB_OTGFS_DOEPINT2_BBLEERR_S)
#define USB_OTGFS_DOEPINT2_BBLEERR_V  0x00000001U
#define USB_OTGFS_DOEPINT2_BBLEERR_S  12
/** USB_OTGFS_DOEPINT2_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DOEPINT2_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DOEPINT2_NAKINTRPT_M  (USB_OTGFS_DOEPINT2_NAKINTRPT_V << USB_OTGFS_DOEPINT2_NAKINTRPT_S)
#define USB_OTGFS_DOEPINT2_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT2_NAKINTRPT_S  13
/** USB_OTGFS_DOEPINT2_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DOEPINT2_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DOEPINT2_NYETINTRPT_M  (USB_OTGFS_DOEPINT2_NYETINTRPT_V << USB_OTGFS_DOEPINT2_NYETINTRPT_S)
#define USB_OTGFS_DOEPINT2_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT2_NYETINTRPT_S  14
/** USB_OTGFS_DOEPINT2_STUPPKTRCVD : R/W; bitpos: [15]; default: 0;
 *  Setup Packet Received
 *
 *  Applicable for Control OUT Endpoints in only in the Buffer DMA Mode
 *
 *  Set by the controller, this bit indicates that this buffer holds 8 bytes of
 *  setup data. There is only one Setup packet per buffer. On receiving a
 *  Setup packet, the controller closes the buffer and disables the
 *  corresponding endpoint. The application has to re-enable the endpoint to
 *  receive any OUT data for the Control Transfer and reprogram the buffer
 *  start address.
 *
 *  Note: Because of the above behavior, the controller can receive any
 *  number of back to back setup packets and one buffer for every setup
 *  packet is used.
 *  - 1'b0: No Setup packet received
 *  - 1'b1: Setup packet received
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPINT2_STUPPKTRCVD    (BIT(15))
#define USB_OTGFS_DOEPINT2_STUPPKTRCVD_M  (USB_OTGFS_DOEPINT2_STUPPKTRCVD_V << USB_OTGFS_DOEPINT2_STUPPKTRCVD_S)
#define USB_OTGFS_DOEPINT2_STUPPKTRCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT2_STUPPKTRCVD_S  15

/** USB_OTGFS_DOEPTSIZ2_REG register
 *  Device OUT Endpoint 2 Transfer Size Register
 */
#define USB_OTGFS_DOEPTSIZ2_REG(i) (DR_REG_USB_BASE(i) + 0xb50)
/** USB_OTGFS_DOEPTSIZ2_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet is read from
 *  the RxFIFO and written to the external memory.
 */
#define USB_OTGFS_DOEPTSIZ2_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ2_XFERSIZE_M  (USB_OTGFS_DOEPTSIZ2_XFERSIZE_V << USB_OTGFS_DOEPTSIZ2_XFERSIZE_S)
#define USB_OTGFS_DOEPTSIZ2_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ2_XFERSIZE_S  0
/** USB_OTGFS_DOEPTSIZ2_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *  This field is decremented to zero after a packet is written into the RxFIFO.
 */
#define USB_OTGFS_DOEPTSIZ2_PKTCNT    0x0000007FU
#define USB_OTGFS_DOEPTSIZ2_PKTCNT_M  (USB_OTGFS_DOEPTSIZ2_PKTCNT_V << USB_OTGFS_DOEPTSIZ2_PKTCNT_S)
#define USB_OTGFS_DOEPTSIZ2_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DOEPTSIZ2_PKTCNT_S  19
/** USB_OTGFS_DOEPTSIZ2_RXDPID : RO; bitpos: [30:29]; default: 0;
 *  RxDPID
 *
 *  Applies to isochronous OUT endpoints only.
 *
 *  This is the data PID received in the last packet for this endpoint.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA
 *  SETUP Packet Count (SUPCnt)
 *
 *  Applies to control OUT Endpoints only.
 *
 *  This field specifies the number of back-to-back SETUP data
 *  packets the endpoint can receive.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 */
#define USB_OTGFS_DOEPTSIZ2_RXDPID    0x00000003U
#define USB_OTGFS_DOEPTSIZ2_RXDPID_M  (USB_OTGFS_DOEPTSIZ2_RXDPID_V << USB_OTGFS_DOEPTSIZ2_RXDPID_S)
#define USB_OTGFS_DOEPTSIZ2_RXDPID_V  0x00000003U
#define USB_OTGFS_DOEPTSIZ2_RXDPID_S  29

/** USB_OTGFS_DOEPDMA2_REG register
 *  Device OUT Endpoint 2 DMA Address Register
 */
#define USB_OTGFS_DOEPDMA2_REG(i) (DR_REG_USB_BASE(i) + 0xb54)
/** USB_OTGFS_DOEPDMA2_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DOEPDMA2_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA2_DMAADDR_M  (USB_OTGFS_DOEPDMA2_DMAADDR_V << USB_OTGFS_DOEPDMA2_DMAADDR_S)
#define USB_OTGFS_DOEPDMA2_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA2_DMAADDR_S  0

/** USB_OTGFS_DOEPDMAB2_REG register
 *  Device OUT Endpoint 2 Buffer Address Register
 */
#define USB_OTGFS_DOEPDMAB2_REG(i) (DR_REG_USB_BASE(i) + 0xb5c)
/** USB_OTGFS_DOEPDMAB2_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DOEPDMAB2_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB2_DMABUFFERADDR_M  (USB_OTGFS_DOEPDMAB2_DMABUFFERADDR_V << USB_OTGFS_DOEPDMAB2_DMABUFFERADDR_S)
#define USB_OTGFS_DOEPDMAB2_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB2_DMABUFFERADDR_S  0

/** USB_OTGFS_DOEPCTL3_REG register
 *  Device Control OUT Endpoint 3 Control Register
 */
#define USB_OTGFS_DOEPCTL3_REG(i) (DR_REG_USB_BASE(i) + 0xb60)
/** USB_OTGFS_DOEPCTL3_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the current
 *  logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DOEPCTL3_MPS    0x000007FFU
#define USB_OTGFS_DOEPCTL3_MPS_M  (USB_OTGFS_DOEPCTL3_MPS_V << USB_OTGFS_DOEPCTL3_MPS_S)
#define USB_OTGFS_DOEPCTL3_MPS_V  0x000007FFU
#define USB_OTGFS_DOEPCTL3_MPS_S  0
/** USB_OTGFS_DOEPCTL3_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DOEPCTL3_USBACTEP    (BIT(15))
#define USB_OTGFS_DOEPCTL3_USBACTEP_M  (USB_OTGFS_DOEPCTL3_USBACTEP_V << USB_OTGFS_DOEPCTL3_USBACTEP_S)
#define USB_OTGFS_DOEPCTL3_USBACTEP_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_USBACTEP_S  15
/** USB_OTGFS_DOEPCTL3_DPID : RO; bitpos: [16]; default: 0;
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable for both Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Reset: 1'b0
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Indicates the (micro)frame number in which the core transmits/receives
 *  isochronous data for this endpoint. The application must program the even/odd
 *  (micro)frame number in which it intends to transmit/receive isochronous data for
 *  this endpoint using the SetEvnFr and SetOddFr fields in this register.
 *  -- 1'b0: Even (micro)frame
 *  -- 1'b1: Odd (micro)frame
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame
 *  in which data is received is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL3_DPID    (BIT(16))
#define USB_OTGFS_DOEPCTL3_DPID_M  (USB_OTGFS_DOEPCTL3_DPID_V << USB_OTGFS_DOEPCTL3_DPID_S)
#define USB_OTGFS_DOEPCTL3_DPID_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_DPID_S  16
/** USB_OTGFS_DOEPCTL3_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL3_NAKSTS    (BIT(17))
#define USB_OTGFS_DOEPCTL3_NAKSTS_M  (USB_OTGFS_DOEPCTL3_NAKSTS_V << USB_OTGFS_DOEPCTL3_NAKSTS_S)
#define USB_OTGFS_DOEPCTL3_NAKSTS_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_NAKSTS_S  17
/** USB_OTGFS_DOEPCTL3_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DOEPCTL3_EPTYPE    0x00000003U
#define USB_OTGFS_DOEPCTL3_EPTYPE_M  (USB_OTGFS_DOEPCTL3_EPTYPE_V << USB_OTGFS_DOEPCTL3_EPTYPE_S)
#define USB_OTGFS_DOEPCTL3_EPTYPE_V  0x00000003U
#define USB_OTGFS_DOEPCTL3_EPTYPE_S  18
/** USB_OTGFS_DOEPCTL3_SNP : R/W; bitpos: [20]; default: 0;
 *  RESERVED
 */
#define USB_OTGFS_DOEPCTL3_SNP    (BIT(20))
#define USB_OTGFS_DOEPCTL3_SNP_M  (USB_OTGFS_DOEPCTL3_SNP_V << USB_OTGFS_DOEPCTL3_SNP_S)
#define USB_OTGFS_DOEPCTL3_SNP_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_SNP_S  20
/** USB_OTGFS_DOEPCTL3_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL3_STALL    (BIT(21))
#define USB_OTGFS_DOEPCTL3_STALL_M  (USB_OTGFS_DOEPCTL3_STALL_V << USB_OTGFS_DOEPCTL3_STALL_S)
#define USB_OTGFS_DOEPCTL3_STALL_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_STALL_S  21
/** USB_OTGFS_DOEPCTL3_CNAK : R/W; bitpos: [26]; default: 0;
 *
 *  Clear NAK (CNAK)
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DOEPCTL3_CNAK    (BIT(26))
#define USB_OTGFS_DOEPCTL3_CNAK_M  (USB_OTGFS_DOEPCTL3_CNAK_V << USB_OTGFS_DOEPCTL3_CNAK_S)
#define USB_OTGFS_DOEPCTL3_CNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_CNAK_S  26
/** USB_OTGFS_DOEPCTL3_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK
 *  handshakes on an endpoint. The core can also set this bit for an
 *  endpoint after a SETUP packet is received on that endpoint.
 */
#define USB_OTGFS_DOEPCTL3_SNAK    (BIT(27))
#define USB_OTGFS_DOEPCTL3_SNAK_M  (USB_OTGFS_DOEPCTL3_SNAK_V << USB_OTGFS_DOEPCTL3_SNAK_S)
#define USB_OTGFS_DOEPCTL3_SNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_SNAK_S  27
/** USB_OTGFS_DOEPCTL3_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  Set DATA0 PID (SetD0PID)
 *  - Applies to interrupt/bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA0.
 *  - This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  In non-Scatter/Gather DMA mode: Set Even (micro)frame (SetEvenFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the Even/Odd (micro)frame (EO_FrNum) field to even
 *  (micro)frame.
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which
 *  to receive data is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL3_SETD0PID    (BIT(28))
#define USB_OTGFS_DOEPCTL3_SETD0PID_M  (USB_OTGFS_DOEPCTL3_SETD0PID_V << USB_OTGFS_DOEPCTL3_SETD0PID_S)
#define USB_OTGFS_DOEPCTL3_SETD0PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_SETD0PID_S  28
/** USB_OTGFS_DOEPCTL3_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  Set DATA1 PID (SetD1PID)
 *  - Applies to interrupt and bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA1.
 *  - This field is applicable both for scatter-gather DMA mode and non scatter-gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  Set Odd (micro)frame (SetOddFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the even and odd (micro)frame (EO_FrNum) field to odd
 *  (micro)frame.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL3_SETD1PID    (BIT(29))
#define USB_OTGFS_DOEPCTL3_SETD1PID_M  (USB_OTGFS_DOEPCTL3_SETD1PID_V << USB_OTGFS_DOEPCTL3_SETD1PID_S)
#define USB_OTGFS_DOEPCTL3_SETD1PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_SETD1PID_S  29
/** USB_OTGFS_DOEPCTL3_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DOEPCTL3_EPDIS    (BIT(30))
#define USB_OTGFS_DOEPCTL3_EPDIS_M  (USB_OTGFS_DOEPCTL3_EPDIS_V << USB_OTGFS_DOEPCTL3_EPDIS_S)
#define USB_OTGFS_DOEPCTL3_EPDIS_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_EPDIS_S  30
/** USB_OTGFS_DOEPCTL3_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  When Scatter/Gather DMA mode is enabled,
 *  - For IN endpoints this bit indicates that the descriptor structure and data buffer
 *  with data ready to transmit is setup.
 *  - For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  - For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  - For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  - SETUP Phase Done
 *  - Endpoint Disabled
 *  - Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DOEPCTL3_EPENA    (BIT(31))
#define USB_OTGFS_DOEPCTL3_EPENA_M  (USB_OTGFS_DOEPCTL3_EPENA_V << USB_OTGFS_DOEPCTL3_EPENA_S)
#define USB_OTGFS_DOEPCTL3_EPENA_V  0x00000001U
#define USB_OTGFS_DOEPCTL3_EPENA_S  31

/** USB_OTGFS_DOEPINT3_REG register
 *  Device OUT Endpoint 3 Interrupt Register
 */
#define USB_OTGFS_DOEPINT3_REG(i) (DR_REG_USB_BASE(i) + 0xb68)
/** USB_OTGFS_DOEPINT3_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is Set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DOEPINT3_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DOEPINT3_XFERCOMPL_M  (USB_OTGFS_DOEPINT3_XFERCOMPL_V << USB_OTGFS_DOEPINT3_XFERCOMPL_S)
#define USB_OTGFS_DOEPINT3_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DOEPINT3_XFERCOMPL_S  0
/** USB_OTGFS_DOEPINT3_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DOEPINT3_EPDISBLD    (BIT(1))
#define USB_OTGFS_DOEPINT3_EPDISBLD_M  (USB_OTGFS_DOEPINT3_EPDISBLD_V << USB_OTGFS_DOEPINT3_EPDISBLD_S)
#define USB_OTGFS_DOEPINT3_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DOEPINT3_EPDISBLD_S  1
/** USB_OTGFS_DOEPINT3_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DOEPINT3_AHBERR    (BIT(2))
#define USB_OTGFS_DOEPINT3_AHBERR_M  (USB_OTGFS_DOEPINT3_AHBERR_V << USB_OTGFS_DOEPINT3_AHBERR_S)
#define USB_OTGFS_DOEPINT3_AHBERR_V  0x00000001U
#define USB_OTGFS_DOEPINT3_AHBERR_S  2
/** USB_OTGFS_DOEPINT3_SETUP : R/W; bitpos: [3]; default: 0;
 *  SETUP Phase Done (SetUp)
 *
 *  Applies to control OUT endpoints only.
 *
 *  Indicates that the SETUP phase for the control endpoint is
 *  complete and no more back-to-back SETUP packets were
 *  received for the current control transfer. On this interrupt, the
 *  application can decode the received SETUP data packet.
 */
#define USB_OTGFS_DOEPINT3_SETUP    (BIT(3))
#define USB_OTGFS_DOEPINT3_SETUP_M  (USB_OTGFS_DOEPINT3_SETUP_V << USB_OTGFS_DOEPINT3_SETUP_S)
#define USB_OTGFS_DOEPINT3_SETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT3_SETUP_S  3
/** USB_OTGFS_DOEPINT3_OUTTKNEPDIS : R/W; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
 *
 *  Applies only to control OUT endpoints.
 *
 *  Indicates that an OUT token was received when the endpoint was not yet enabled.
 *  This interrupt is asserted on the endpoint for which the OUT token was received.
 */
#define USB_OTGFS_DOEPINT3_OUTTKNEPDIS    (BIT(4))
#define USB_OTGFS_DOEPINT3_OUTTKNEPDIS_M  (USB_OTGFS_DOEPINT3_OUTTKNEPDIS_V << USB_OTGFS_DOEPINT3_OUTTKNEPDIS_S)
#define USB_OTGFS_DOEPINT3_OUTTKNEPDIS_V  0x00000001U
#define USB_OTGFS_DOEPINT3_OUTTKNEPDIS_S  4
/** USB_OTGFS_DOEPINT3_STSPHSERCVD : R/W; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write (StsPhseRcvd)
 *
 *  This interrupt is valid only for Control OUT endpoints and only in
 *  Scatter Gather DMA mode.
 *
 *  This interrupt is generated only after the core has transferred all
 *  the data that the host has sent during the data phase of a control
 *  write transfer, to the system memory buffer.
 *
 *  The interrupt indicates to the application that the host has
 *  switched from data phase to the status phase of a Control Write
 *  transfer. The application can use this interrupt to ACK or STALL
 *  the Status phase, after it has decoded the data phase. This is
 *  applicable only in Case of Scatter Gather DMA mode.
 */
#define USB_OTGFS_DOEPINT3_STSPHSERCVD    (BIT(5))
#define USB_OTGFS_DOEPINT3_STSPHSERCVD_M  (USB_OTGFS_DOEPINT3_STSPHSERCVD_V << USB_OTGFS_DOEPINT3_STSPHSERCVD_S)
#define USB_OTGFS_DOEPINT3_STSPHSERCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT3_STSPHSERCVD_S  5
/** USB_OTGFS_DOEPINT3_BACK2BACKSETUP : R/W; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received (Back2BackSETup)
 *
 *  Applies to Control OUT endpoints only.
 *
 *  This bit indicates that the core has received more than three
 *  back-to-back SETUP packets for this particular endpoint. For
 *  information about handling this interrupt,
 */
#define USB_OTGFS_DOEPINT3_BACK2BACKSETUP    (BIT(6))
#define USB_OTGFS_DOEPINT3_BACK2BACKSETUP_M  (USB_OTGFS_DOEPINT3_BACK2BACKSETUP_V << USB_OTGFS_DOEPINT3_BACK2BACKSETUP_S)
#define USB_OTGFS_DOEPINT3_BACK2BACKSETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT3_BACK2BACKSETUP_S  6
/** USB_OTGFS_DOEPINT3_OUTPKTERR : R/W; bitpos: [8]; default: 0;
 *  OUT Packet Error (OutPktErr)
 *
 *  Applies to OUT endpoints Only
 *
 *  This interrupt is valid only when thresholding is enabled. This interrupt is
 *  asserted when the
 *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
 */
#define USB_OTGFS_DOEPINT3_OUTPKTERR    (BIT(8))
#define USB_OTGFS_DOEPINT3_OUTPKTERR_M  (USB_OTGFS_DOEPINT3_OUTPKTERR_V << USB_OTGFS_DOEPINT3_OUTPKTERR_S)
#define USB_OTGFS_DOEPINT3_OUTPKTERR_V  0x00000001U
#define USB_OTGFS_DOEPINT3_OUTPKTERR_S  8
/** USB_OTGFS_DOEPINT3_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process, such as Host busy or DMA
 *  done
 */
#define USB_OTGFS_DOEPINT3_BNAINTR    (BIT(9))
#define USB_OTGFS_DOEPINT3_BNAINTR_M  (USB_OTGFS_DOEPINT3_BNAINTR_V << USB_OTGFS_DOEPINT3_BNAINTR_S)
#define USB_OTGFS_DOEPINT3_BNAINTR_V  0x00000001U
#define USB_OTGFS_DOEPINT3_BNAINTR_S  9
/** USB_OTGFS_DOEPINT3_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DOEPINT3_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DOEPINT3_PKTDRPSTS_M  (USB_OTGFS_DOEPINT3_PKTDRPSTS_V << USB_OTGFS_DOEPINT3_PKTDRPSTS_S)
#define USB_OTGFS_DOEPINT3_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DOEPINT3_PKTDRPSTS_S  11
/** USB_OTGFS_DOEPINT3_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DOEPINT3_BBLEERR    (BIT(12))
#define USB_OTGFS_DOEPINT3_BBLEERR_M  (USB_OTGFS_DOEPINT3_BBLEERR_V << USB_OTGFS_DOEPINT3_BBLEERR_S)
#define USB_OTGFS_DOEPINT3_BBLEERR_V  0x00000001U
#define USB_OTGFS_DOEPINT3_BBLEERR_S  12
/** USB_OTGFS_DOEPINT3_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DOEPINT3_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DOEPINT3_NAKINTRPT_M  (USB_OTGFS_DOEPINT3_NAKINTRPT_V << USB_OTGFS_DOEPINT3_NAKINTRPT_S)
#define USB_OTGFS_DOEPINT3_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT3_NAKINTRPT_S  13
/** USB_OTGFS_DOEPINT3_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DOEPINT3_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DOEPINT3_NYETINTRPT_M  (USB_OTGFS_DOEPINT3_NYETINTRPT_V << USB_OTGFS_DOEPINT3_NYETINTRPT_S)
#define USB_OTGFS_DOEPINT3_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT3_NYETINTRPT_S  14
/** USB_OTGFS_DOEPINT3_STUPPKTRCVD : R/W; bitpos: [15]; default: 0;
 *  Setup Packet Received
 *
 *  Applicable for Control OUT Endpoints in only in the Buffer DMA Mode
 *
 *  Set by the controller, this bit indicates that this buffer holds 8 bytes of
 *  setup data. There is only one Setup packet per buffer. On receiving a
 *  Setup packet, the controller closes the buffer and disables the
 *  corresponding endpoint. The application has to re-enable the endpoint to
 *  receive any OUT data for the Control Transfer and reprogram the buffer
 *  start address.
 *
 *  Note: Because of the above behavior, the controller can receive any
 *  number of back to back setup packets and one buffer for every setup
 *  packet is used.
 *  - 1'b0: No Setup packet received
 *  - 1'b1: Setup packet received
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPINT3_STUPPKTRCVD    (BIT(15))
#define USB_OTGFS_DOEPINT3_STUPPKTRCVD_M  (USB_OTGFS_DOEPINT3_STUPPKTRCVD_V << USB_OTGFS_DOEPINT3_STUPPKTRCVD_S)
#define USB_OTGFS_DOEPINT3_STUPPKTRCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT3_STUPPKTRCVD_S  15

/** USB_OTGFS_DOEPTSIZ3_REG register
 *  Device OUT Endpoint 3 Transfer Size Register
 */
#define USB_OTGFS_DOEPTSIZ3_REG(i) (DR_REG_USB_BASE(i) + 0xb70)
/** USB_OTGFS_DOEPTSIZ3_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet is read from
 *  the RxFIFO and written to the external memory.
 */
#define USB_OTGFS_DOEPTSIZ3_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ3_XFERSIZE_M  (USB_OTGFS_DOEPTSIZ3_XFERSIZE_V << USB_OTGFS_DOEPTSIZ3_XFERSIZE_S)
#define USB_OTGFS_DOEPTSIZ3_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ3_XFERSIZE_S  0
/** USB_OTGFS_DOEPTSIZ3_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *  This field is decremented to zero after a packet is written into the RxFIFO.
 */
#define USB_OTGFS_DOEPTSIZ3_PKTCNT    0x0000007FU
#define USB_OTGFS_DOEPTSIZ3_PKTCNT_M  (USB_OTGFS_DOEPTSIZ3_PKTCNT_V << USB_OTGFS_DOEPTSIZ3_PKTCNT_S)
#define USB_OTGFS_DOEPTSIZ3_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DOEPTSIZ3_PKTCNT_S  19
/** USB_OTGFS_DOEPTSIZ3_RXDPID : RO; bitpos: [30:29]; default: 0;
 *  RxDPID
 *
 *  Applies to isochronous OUT endpoints only.
 *
 *  This is the data PID received in the last packet for this endpoint.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA
 *  SETUP Packet Count (SUPCnt)
 *
 *  Applies to control OUT Endpoints only.
 *
 *  This field specifies the number of back-to-back SETUP data
 *  packets the endpoint can receive.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 */
#define USB_OTGFS_DOEPTSIZ3_RXDPID    0x00000003U
#define USB_OTGFS_DOEPTSIZ3_RXDPID_M  (USB_OTGFS_DOEPTSIZ3_RXDPID_V << USB_OTGFS_DOEPTSIZ3_RXDPID_S)
#define USB_OTGFS_DOEPTSIZ3_RXDPID_V  0x00000003U
#define USB_OTGFS_DOEPTSIZ3_RXDPID_S  29

/** USB_OTGFS_DOEPDMA3_REG register
 *  Device OUT Endpoint 3 DMA Address Register
 */
#define USB_OTGFS_DOEPDMA3_REG(i) (DR_REG_USB_BASE(i) + 0xb74)
/** USB_OTGFS_DOEPDMA3_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DOEPDMA3_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA3_DMAADDR_M  (USB_OTGFS_DOEPDMA3_DMAADDR_V << USB_OTGFS_DOEPDMA3_DMAADDR_S)
#define USB_OTGFS_DOEPDMA3_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA3_DMAADDR_S  0

/** USB_OTGFS_DOEPDMAB3_REG register
 *  Device OUT Endpoint 3 Buffer Address Register
 */
#define USB_OTGFS_DOEPDMAB3_REG(i) (DR_REG_USB_BASE(i) + 0xb7c)
/** USB_OTGFS_DOEPDMAB3_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DOEPDMAB3_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB3_DMABUFFERADDR_M  (USB_OTGFS_DOEPDMAB3_DMABUFFERADDR_V << USB_OTGFS_DOEPDMAB3_DMABUFFERADDR_S)
#define USB_OTGFS_DOEPDMAB3_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB3_DMABUFFERADDR_S  0

/** USB_OTGFS_DOEPCTL4_REG register
 *  Device Control OUT Endpoint 4 Control Register
 */
#define USB_OTGFS_DOEPCTL4_REG(i) (DR_REG_USB_BASE(i) + 0xb80)
/** USB_OTGFS_DOEPCTL4_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the current
 *  logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DOEPCTL4_MPS    0x000007FFU
#define USB_OTGFS_DOEPCTL4_MPS_M  (USB_OTGFS_DOEPCTL4_MPS_V << USB_OTGFS_DOEPCTL4_MPS_S)
#define USB_OTGFS_DOEPCTL4_MPS_V  0x000007FFU
#define USB_OTGFS_DOEPCTL4_MPS_S  0
/** USB_OTGFS_DOEPCTL4_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DOEPCTL4_USBACTEP    (BIT(15))
#define USB_OTGFS_DOEPCTL4_USBACTEP_M  (USB_OTGFS_DOEPCTL4_USBACTEP_V << USB_OTGFS_DOEPCTL4_USBACTEP_S)
#define USB_OTGFS_DOEPCTL4_USBACTEP_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_USBACTEP_S  15
/** USB_OTGFS_DOEPCTL4_DPID : RO; bitpos: [16]; default: 0;
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable for both Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Reset: 1'b0
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Indicates the (micro)frame number in which the core transmits/receives
 *  isochronous data for this endpoint. The application must program the even/odd
 *  (micro)frame number in which it intends to transmit/receive isochronous data for
 *  this endpoint using the SetEvnFr and SetOddFr fields in this register.
 *  -- 1'b0: Even (micro)frame
 *  -- 1'b1: Odd (micro)frame
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame
 *  in which data is received is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL4_DPID    (BIT(16))
#define USB_OTGFS_DOEPCTL4_DPID_M  (USB_OTGFS_DOEPCTL4_DPID_V << USB_OTGFS_DOEPCTL4_DPID_S)
#define USB_OTGFS_DOEPCTL4_DPID_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_DPID_S  16
/** USB_OTGFS_DOEPCTL4_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL4_NAKSTS    (BIT(17))
#define USB_OTGFS_DOEPCTL4_NAKSTS_M  (USB_OTGFS_DOEPCTL4_NAKSTS_V << USB_OTGFS_DOEPCTL4_NAKSTS_S)
#define USB_OTGFS_DOEPCTL4_NAKSTS_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_NAKSTS_S  17
/** USB_OTGFS_DOEPCTL4_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DOEPCTL4_EPTYPE    0x00000003U
#define USB_OTGFS_DOEPCTL4_EPTYPE_M  (USB_OTGFS_DOEPCTL4_EPTYPE_V << USB_OTGFS_DOEPCTL4_EPTYPE_S)
#define USB_OTGFS_DOEPCTL4_EPTYPE_V  0x00000003U
#define USB_OTGFS_DOEPCTL4_EPTYPE_S  18
/** USB_OTGFS_DOEPCTL4_SNP : R/W; bitpos: [20]; default: 0;
 *  RESERVED
 */
#define USB_OTGFS_DOEPCTL4_SNP    (BIT(20))
#define USB_OTGFS_DOEPCTL4_SNP_M  (USB_OTGFS_DOEPCTL4_SNP_V << USB_OTGFS_DOEPCTL4_SNP_S)
#define USB_OTGFS_DOEPCTL4_SNP_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_SNP_S  20
/** USB_OTGFS_DOEPCTL4_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL4_STALL    (BIT(21))
#define USB_OTGFS_DOEPCTL4_STALL_M  (USB_OTGFS_DOEPCTL4_STALL_V << USB_OTGFS_DOEPCTL4_STALL_S)
#define USB_OTGFS_DOEPCTL4_STALL_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_STALL_S  21
/** USB_OTGFS_DOEPCTL4_CNAK : R/W; bitpos: [26]; default: 0;
 *
 *  Clear NAK (CNAK)
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DOEPCTL4_CNAK    (BIT(26))
#define USB_OTGFS_DOEPCTL4_CNAK_M  (USB_OTGFS_DOEPCTL4_CNAK_V << USB_OTGFS_DOEPCTL4_CNAK_S)
#define USB_OTGFS_DOEPCTL4_CNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_CNAK_S  26
/** USB_OTGFS_DOEPCTL4_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK
 *  handshakes on an endpoint. The core can also set this bit for an
 *  endpoint after a SETUP packet is received on that endpoint.
 */
#define USB_OTGFS_DOEPCTL4_SNAK    (BIT(27))
#define USB_OTGFS_DOEPCTL4_SNAK_M  (USB_OTGFS_DOEPCTL4_SNAK_V << USB_OTGFS_DOEPCTL4_SNAK_S)
#define USB_OTGFS_DOEPCTL4_SNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_SNAK_S  27
/** USB_OTGFS_DOEPCTL4_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  Set DATA0 PID (SetD0PID)
 *  - Applies to interrupt/bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA0.
 *  - This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  In non-Scatter/Gather DMA mode: Set Even (micro)frame (SetEvenFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the Even/Odd (micro)frame (EO_FrNum) field to even
 *  (micro)frame.
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which
 *  to receive data is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL4_SETD0PID    (BIT(28))
#define USB_OTGFS_DOEPCTL4_SETD0PID_M  (USB_OTGFS_DOEPCTL4_SETD0PID_V << USB_OTGFS_DOEPCTL4_SETD0PID_S)
#define USB_OTGFS_DOEPCTL4_SETD0PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_SETD0PID_S  28
/** USB_OTGFS_DOEPCTL4_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  Set DATA1 PID (SetD1PID)
 *  - Applies to interrupt and bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA1.
 *  - This field is applicable both for scatter-gather DMA mode and non scatter-gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  Set Odd (micro)frame (SetOddFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the even and odd (micro)frame (EO_FrNum) field to odd
 *  (micro)frame.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL4_SETD1PID    (BIT(29))
#define USB_OTGFS_DOEPCTL4_SETD1PID_M  (USB_OTGFS_DOEPCTL4_SETD1PID_V << USB_OTGFS_DOEPCTL4_SETD1PID_S)
#define USB_OTGFS_DOEPCTL4_SETD1PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_SETD1PID_S  29
/** USB_OTGFS_DOEPCTL4_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DOEPCTL4_EPDIS    (BIT(30))
#define USB_OTGFS_DOEPCTL4_EPDIS_M  (USB_OTGFS_DOEPCTL4_EPDIS_V << USB_OTGFS_DOEPCTL4_EPDIS_S)
#define USB_OTGFS_DOEPCTL4_EPDIS_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_EPDIS_S  30
/** USB_OTGFS_DOEPCTL4_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  When Scatter/Gather DMA mode is enabled,
 *  - For IN endpoints this bit indicates that the descriptor structure and data buffer
 *  with data ready to transmit is setup.
 *  - For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  - For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  - For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  - SETUP Phase Done
 *  - Endpoint Disabled
 *  - Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DOEPCTL4_EPENA    (BIT(31))
#define USB_OTGFS_DOEPCTL4_EPENA_M  (USB_OTGFS_DOEPCTL4_EPENA_V << USB_OTGFS_DOEPCTL4_EPENA_S)
#define USB_OTGFS_DOEPCTL4_EPENA_V  0x00000001U
#define USB_OTGFS_DOEPCTL4_EPENA_S  31

/** USB_OTGFS_DOEPINT4_REG register
 *  Device OUT Endpoint 4 Interrupt Register
 */
#define USB_OTGFS_DOEPINT4_REG(i) (DR_REG_USB_BASE(i) + 0xb88)
/** USB_OTGFS_DOEPINT4_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is Set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DOEPINT4_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DOEPINT4_XFERCOMPL_M  (USB_OTGFS_DOEPINT4_XFERCOMPL_V << USB_OTGFS_DOEPINT4_XFERCOMPL_S)
#define USB_OTGFS_DOEPINT4_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DOEPINT4_XFERCOMPL_S  0
/** USB_OTGFS_DOEPINT4_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DOEPINT4_EPDISBLD    (BIT(1))
#define USB_OTGFS_DOEPINT4_EPDISBLD_M  (USB_OTGFS_DOEPINT4_EPDISBLD_V << USB_OTGFS_DOEPINT4_EPDISBLD_S)
#define USB_OTGFS_DOEPINT4_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DOEPINT4_EPDISBLD_S  1
/** USB_OTGFS_DOEPINT4_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DOEPINT4_AHBERR    (BIT(2))
#define USB_OTGFS_DOEPINT4_AHBERR_M  (USB_OTGFS_DOEPINT4_AHBERR_V << USB_OTGFS_DOEPINT4_AHBERR_S)
#define USB_OTGFS_DOEPINT4_AHBERR_V  0x00000001U
#define USB_OTGFS_DOEPINT4_AHBERR_S  2
/** USB_OTGFS_DOEPINT4_SETUP : R/W; bitpos: [3]; default: 0;
 *  SETUP Phase Done (SetUp)
 *
 *  Applies to control OUT endpoints only.
 *
 *  Indicates that the SETUP phase for the control endpoint is
 *  complete and no more back-to-back SETUP packets were
 *  received for the current control transfer. On this interrupt, the
 *  application can decode the received SETUP data packet.
 */
#define USB_OTGFS_DOEPINT4_SETUP    (BIT(3))
#define USB_OTGFS_DOEPINT4_SETUP_M  (USB_OTGFS_DOEPINT4_SETUP_V << USB_OTGFS_DOEPINT4_SETUP_S)
#define USB_OTGFS_DOEPINT4_SETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT4_SETUP_S  3
/** USB_OTGFS_DOEPINT4_OUTTKNEPDIS : R/W; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
 *
 *  Applies only to control OUT endpoints.
 *
 *  Indicates that an OUT token was received when the endpoint was not yet enabled.
 *  This interrupt is asserted on the endpoint for which the OUT token was received.
 */
#define USB_OTGFS_DOEPINT4_OUTTKNEPDIS    (BIT(4))
#define USB_OTGFS_DOEPINT4_OUTTKNEPDIS_M  (USB_OTGFS_DOEPINT4_OUTTKNEPDIS_V << USB_OTGFS_DOEPINT4_OUTTKNEPDIS_S)
#define USB_OTGFS_DOEPINT4_OUTTKNEPDIS_V  0x00000001U
#define USB_OTGFS_DOEPINT4_OUTTKNEPDIS_S  4
/** USB_OTGFS_DOEPINT4_STSPHSERCVD : R/W; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write (StsPhseRcvd)
 *
 *  This interrupt is valid only for Control OUT endpoints and only in
 *  Scatter Gather DMA mode.
 *
 *  This interrupt is generated only after the core has transferred all
 *  the data that the host has sent during the data phase of a control
 *  write transfer, to the system memory buffer.
 *
 *  The interrupt indicates to the application that the host has
 *  switched from data phase to the status phase of a Control Write
 *  transfer. The application can use this interrupt to ACK or STALL
 *  the Status phase, after it has decoded the data phase. This is
 *  applicable only in Case of Scatter Gather DMA mode.
 */
#define USB_OTGFS_DOEPINT4_STSPHSERCVD    (BIT(5))
#define USB_OTGFS_DOEPINT4_STSPHSERCVD_M  (USB_OTGFS_DOEPINT4_STSPHSERCVD_V << USB_OTGFS_DOEPINT4_STSPHSERCVD_S)
#define USB_OTGFS_DOEPINT4_STSPHSERCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT4_STSPHSERCVD_S  5
/** USB_OTGFS_DOEPINT4_BACK2BACKSETUP : R/W; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received (Back2BackSETup)
 *
 *  Applies to Control OUT endpoints only.
 *
 *  This bit indicates that the core has received more than three
 *  back-to-back SETUP packets for this particular endpoint. For
 *  information about handling this interrupt,
 */
#define USB_OTGFS_DOEPINT4_BACK2BACKSETUP    (BIT(6))
#define USB_OTGFS_DOEPINT4_BACK2BACKSETUP_M  (USB_OTGFS_DOEPINT4_BACK2BACKSETUP_V << USB_OTGFS_DOEPINT4_BACK2BACKSETUP_S)
#define USB_OTGFS_DOEPINT4_BACK2BACKSETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT4_BACK2BACKSETUP_S  6
/** USB_OTGFS_DOEPINT4_OUTPKTERR : R/W; bitpos: [8]; default: 0;
 *  OUT Packet Error (OutPktErr)
 *
 *  Applies to OUT endpoints Only
 *
 *  This interrupt is valid only when thresholding is enabled. This interrupt is
 *  asserted when the
 *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
 */
#define USB_OTGFS_DOEPINT4_OUTPKTERR    (BIT(8))
#define USB_OTGFS_DOEPINT4_OUTPKTERR_M  (USB_OTGFS_DOEPINT4_OUTPKTERR_V << USB_OTGFS_DOEPINT4_OUTPKTERR_S)
#define USB_OTGFS_DOEPINT4_OUTPKTERR_V  0x00000001U
#define USB_OTGFS_DOEPINT4_OUTPKTERR_S  8
/** USB_OTGFS_DOEPINT4_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process, such as Host busy or DMA
 *  done
 */
#define USB_OTGFS_DOEPINT4_BNAINTR    (BIT(9))
#define USB_OTGFS_DOEPINT4_BNAINTR_M  (USB_OTGFS_DOEPINT4_BNAINTR_V << USB_OTGFS_DOEPINT4_BNAINTR_S)
#define USB_OTGFS_DOEPINT4_BNAINTR_V  0x00000001U
#define USB_OTGFS_DOEPINT4_BNAINTR_S  9
/** USB_OTGFS_DOEPINT4_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DOEPINT4_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DOEPINT4_PKTDRPSTS_M  (USB_OTGFS_DOEPINT4_PKTDRPSTS_V << USB_OTGFS_DOEPINT4_PKTDRPSTS_S)
#define USB_OTGFS_DOEPINT4_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DOEPINT4_PKTDRPSTS_S  11
/** USB_OTGFS_DOEPINT4_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DOEPINT4_BBLEERR    (BIT(12))
#define USB_OTGFS_DOEPINT4_BBLEERR_M  (USB_OTGFS_DOEPINT4_BBLEERR_V << USB_OTGFS_DOEPINT4_BBLEERR_S)
#define USB_OTGFS_DOEPINT4_BBLEERR_V  0x00000001U
#define USB_OTGFS_DOEPINT4_BBLEERR_S  12
/** USB_OTGFS_DOEPINT4_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DOEPINT4_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DOEPINT4_NAKINTRPT_M  (USB_OTGFS_DOEPINT4_NAKINTRPT_V << USB_OTGFS_DOEPINT4_NAKINTRPT_S)
#define USB_OTGFS_DOEPINT4_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT4_NAKINTRPT_S  13
/** USB_OTGFS_DOEPINT4_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DOEPINT4_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DOEPINT4_NYETINTRPT_M  (USB_OTGFS_DOEPINT4_NYETINTRPT_V << USB_OTGFS_DOEPINT4_NYETINTRPT_S)
#define USB_OTGFS_DOEPINT4_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT4_NYETINTRPT_S  14
/** USB_OTGFS_DOEPINT4_STUPPKTRCVD : R/W; bitpos: [15]; default: 0;
 *  Setup Packet Received
 *
 *  Applicable for Control OUT Endpoints in only in the Buffer DMA Mode
 *
 *  Set by the controller, this bit indicates that this buffer holds 8 bytes of
 *  setup data. There is only one Setup packet per buffer. On receiving a
 *  Setup packet, the controller closes the buffer and disables the
 *  corresponding endpoint. The application has to re-enable the endpoint to
 *  receive any OUT data for the Control Transfer and reprogram the buffer
 *  start address.
 *
 *  Note: Because of the above behavior, the controller can receive any
 *  number of back to back setup packets and one buffer for every setup
 *  packet is used.
 *  - 1'b0: No Setup packet received
 *  - 1'b1: Setup packet received
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPINT4_STUPPKTRCVD    (BIT(15))
#define USB_OTGFS_DOEPINT4_STUPPKTRCVD_M  (USB_OTGFS_DOEPINT4_STUPPKTRCVD_V << USB_OTGFS_DOEPINT4_STUPPKTRCVD_S)
#define USB_OTGFS_DOEPINT4_STUPPKTRCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT4_STUPPKTRCVD_S  15

/** USB_OTGFS_DOEPTSIZ4_REG register
 *  Device OUT Endpoint 4 Transfer Size Register
 */
#define USB_OTGFS_DOEPTSIZ4_REG(i) (DR_REG_USB_BASE(i) + 0xb90)
/** USB_OTGFS_DOEPTSIZ4_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet is read from
 *  the RxFIFO and written to the external memory.
 */
#define USB_OTGFS_DOEPTSIZ4_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ4_XFERSIZE_M  (USB_OTGFS_DOEPTSIZ4_XFERSIZE_V << USB_OTGFS_DOEPTSIZ4_XFERSIZE_S)
#define USB_OTGFS_DOEPTSIZ4_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ4_XFERSIZE_S  0
/** USB_OTGFS_DOEPTSIZ4_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *  This field is decremented to zero after a packet is written into the RxFIFO.
 */
#define USB_OTGFS_DOEPTSIZ4_PKTCNT    0x0000007FU
#define USB_OTGFS_DOEPTSIZ4_PKTCNT_M  (USB_OTGFS_DOEPTSIZ4_PKTCNT_V << USB_OTGFS_DOEPTSIZ4_PKTCNT_S)
#define USB_OTGFS_DOEPTSIZ4_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DOEPTSIZ4_PKTCNT_S  19
/** USB_OTGFS_DOEPTSIZ4_RXDPID : RO; bitpos: [30:29]; default: 0;
 *  RxDPID
 *
 *  Applies to isochronous OUT endpoints only.
 *
 *  This is the data PID received in the last packet for this endpoint.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA
 *  SETUP Packet Count (SUPCnt)
 *
 *  Applies to control OUT Endpoints only.
 *
 *  This field specifies the number of back-to-back SETUP data
 *  packets the endpoint can receive.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 */
#define USB_OTGFS_DOEPTSIZ4_RXDPID    0x00000003U
#define USB_OTGFS_DOEPTSIZ4_RXDPID_M  (USB_OTGFS_DOEPTSIZ4_RXDPID_V << USB_OTGFS_DOEPTSIZ4_RXDPID_S)
#define USB_OTGFS_DOEPTSIZ4_RXDPID_V  0x00000003U
#define USB_OTGFS_DOEPTSIZ4_RXDPID_S  29

/** USB_OTGFS_DOEPDMA4_REG register
 *  Device OUT Endpoint 4 DMA Address Register
 */
#define USB_OTGFS_DOEPDMA4_REG(i) (DR_REG_USB_BASE(i) + 0xb94)
/** USB_OTGFS_DOEPDMA4_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DOEPDMA4_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA4_DMAADDR_M  (USB_OTGFS_DOEPDMA4_DMAADDR_V << USB_OTGFS_DOEPDMA4_DMAADDR_S)
#define USB_OTGFS_DOEPDMA4_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA4_DMAADDR_S  0

/** USB_OTGFS_DOEPDMAB4_REG register
 *  Device OUT Endpoint 4 Buffer Address Register
 */
#define USB_OTGFS_DOEPDMAB4_REG(i) (DR_REG_USB_BASE(i) + 0xb9c)
/** USB_OTGFS_DOEPDMAB4_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DOEPDMAB4_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB4_DMABUFFERADDR_M  (USB_OTGFS_DOEPDMAB4_DMABUFFERADDR_V << USB_OTGFS_DOEPDMAB4_DMABUFFERADDR_S)
#define USB_OTGFS_DOEPDMAB4_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB4_DMABUFFERADDR_S  0

/** USB_OTGFS_DOEPCTL5_REG register
 *  Device Control OUT Endpoint 5 Control Register
 */
#define USB_OTGFS_DOEPCTL5_REG(i) (DR_REG_USB_BASE(i) + 0xba0)
/** USB_OTGFS_DOEPCTL5_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the current
 *  logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DOEPCTL5_MPS    0x000007FFU
#define USB_OTGFS_DOEPCTL5_MPS_M  (USB_OTGFS_DOEPCTL5_MPS_V << USB_OTGFS_DOEPCTL5_MPS_S)
#define USB_OTGFS_DOEPCTL5_MPS_V  0x000007FFU
#define USB_OTGFS_DOEPCTL5_MPS_S  0
/** USB_OTGFS_DOEPCTL5_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DOEPCTL5_USBACTEP    (BIT(15))
#define USB_OTGFS_DOEPCTL5_USBACTEP_M  (USB_OTGFS_DOEPCTL5_USBACTEP_V << USB_OTGFS_DOEPCTL5_USBACTEP_S)
#define USB_OTGFS_DOEPCTL5_USBACTEP_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_USBACTEP_S  15
/** USB_OTGFS_DOEPCTL5_DPID : RO; bitpos: [16]; default: 0;
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable for both Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Reset: 1'b0
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Indicates the (micro)frame number in which the core transmits/receives
 *  isochronous data for this endpoint. The application must program the even/odd
 *  (micro)frame number in which it intends to transmit/receive isochronous data for
 *  this endpoint using the SetEvnFr and SetOddFr fields in this register.
 *  -- 1'b0: Even (micro)frame
 *  -- 1'b1: Odd (micro)frame
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame
 *  in which data is received is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL5_DPID    (BIT(16))
#define USB_OTGFS_DOEPCTL5_DPID_M  (USB_OTGFS_DOEPCTL5_DPID_V << USB_OTGFS_DOEPCTL5_DPID_S)
#define USB_OTGFS_DOEPCTL5_DPID_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_DPID_S  16
/** USB_OTGFS_DOEPCTL5_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL5_NAKSTS    (BIT(17))
#define USB_OTGFS_DOEPCTL5_NAKSTS_M  (USB_OTGFS_DOEPCTL5_NAKSTS_V << USB_OTGFS_DOEPCTL5_NAKSTS_S)
#define USB_OTGFS_DOEPCTL5_NAKSTS_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_NAKSTS_S  17
/** USB_OTGFS_DOEPCTL5_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DOEPCTL5_EPTYPE    0x00000003U
#define USB_OTGFS_DOEPCTL5_EPTYPE_M  (USB_OTGFS_DOEPCTL5_EPTYPE_V << USB_OTGFS_DOEPCTL5_EPTYPE_S)
#define USB_OTGFS_DOEPCTL5_EPTYPE_V  0x00000003U
#define USB_OTGFS_DOEPCTL5_EPTYPE_S  18
/** USB_OTGFS_DOEPCTL5_SNP : R/W; bitpos: [20]; default: 0;
 *  RESERVED
 */
#define USB_OTGFS_DOEPCTL5_SNP    (BIT(20))
#define USB_OTGFS_DOEPCTL5_SNP_M  (USB_OTGFS_DOEPCTL5_SNP_V << USB_OTGFS_DOEPCTL5_SNP_S)
#define USB_OTGFS_DOEPCTL5_SNP_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_SNP_S  20
/** USB_OTGFS_DOEPCTL5_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL5_STALL    (BIT(21))
#define USB_OTGFS_DOEPCTL5_STALL_M  (USB_OTGFS_DOEPCTL5_STALL_V << USB_OTGFS_DOEPCTL5_STALL_S)
#define USB_OTGFS_DOEPCTL5_STALL_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_STALL_S  21
/** USB_OTGFS_DOEPCTL5_CNAK : R/W; bitpos: [26]; default: 0;
 *
 *  Clear NAK (CNAK)
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DOEPCTL5_CNAK    (BIT(26))
#define USB_OTGFS_DOEPCTL5_CNAK_M  (USB_OTGFS_DOEPCTL5_CNAK_V << USB_OTGFS_DOEPCTL5_CNAK_S)
#define USB_OTGFS_DOEPCTL5_CNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_CNAK_S  26
/** USB_OTGFS_DOEPCTL5_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK
 *  handshakes on an endpoint. The core can also set this bit for an
 *  endpoint after a SETUP packet is received on that endpoint.
 */
#define USB_OTGFS_DOEPCTL5_SNAK    (BIT(27))
#define USB_OTGFS_DOEPCTL5_SNAK_M  (USB_OTGFS_DOEPCTL5_SNAK_V << USB_OTGFS_DOEPCTL5_SNAK_S)
#define USB_OTGFS_DOEPCTL5_SNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_SNAK_S  27
/** USB_OTGFS_DOEPCTL5_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  Set DATA0 PID (SetD0PID)
 *  - Applies to interrupt/bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA0.
 *  - This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  In non-Scatter/Gather DMA mode: Set Even (micro)frame (SetEvenFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the Even/Odd (micro)frame (EO_FrNum) field to even
 *  (micro)frame.
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which
 *  to receive data is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL5_SETD0PID    (BIT(28))
#define USB_OTGFS_DOEPCTL5_SETD0PID_M  (USB_OTGFS_DOEPCTL5_SETD0PID_V << USB_OTGFS_DOEPCTL5_SETD0PID_S)
#define USB_OTGFS_DOEPCTL5_SETD0PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_SETD0PID_S  28
/** USB_OTGFS_DOEPCTL5_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  Set DATA1 PID (SetD1PID)
 *  - Applies to interrupt and bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA1.
 *  - This field is applicable both for scatter-gather DMA mode and non scatter-gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  Set Odd (micro)frame (SetOddFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the even and odd (micro)frame (EO_FrNum) field to odd
 *  (micro)frame.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL5_SETD1PID    (BIT(29))
#define USB_OTGFS_DOEPCTL5_SETD1PID_M  (USB_OTGFS_DOEPCTL5_SETD1PID_V << USB_OTGFS_DOEPCTL5_SETD1PID_S)
#define USB_OTGFS_DOEPCTL5_SETD1PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_SETD1PID_S  29
/** USB_OTGFS_DOEPCTL5_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DOEPCTL5_EPDIS    (BIT(30))
#define USB_OTGFS_DOEPCTL5_EPDIS_M  (USB_OTGFS_DOEPCTL5_EPDIS_V << USB_OTGFS_DOEPCTL5_EPDIS_S)
#define USB_OTGFS_DOEPCTL5_EPDIS_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_EPDIS_S  30
/** USB_OTGFS_DOEPCTL5_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  When Scatter/Gather DMA mode is enabled,
 *  - For IN endpoints this bit indicates that the descriptor structure and data buffer
 *  with data ready to transmit is setup.
 *  - For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  - For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  - For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  - SETUP Phase Done
 *  - Endpoint Disabled
 *  - Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DOEPCTL5_EPENA    (BIT(31))
#define USB_OTGFS_DOEPCTL5_EPENA_M  (USB_OTGFS_DOEPCTL5_EPENA_V << USB_OTGFS_DOEPCTL5_EPENA_S)
#define USB_OTGFS_DOEPCTL5_EPENA_V  0x00000001U
#define USB_OTGFS_DOEPCTL5_EPENA_S  31

/** USB_OTGFS_DOEPINT5_REG register
 *  Device OUT Endpoint 5 Interrupt Register
 */
#define USB_OTGFS_DOEPINT5_REG(i) (DR_REG_USB_BASE(i) + 0xba8)
/** USB_OTGFS_DOEPINT5_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is Set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DOEPINT5_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DOEPINT5_XFERCOMPL_M  (USB_OTGFS_DOEPINT5_XFERCOMPL_V << USB_OTGFS_DOEPINT5_XFERCOMPL_S)
#define USB_OTGFS_DOEPINT5_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DOEPINT5_XFERCOMPL_S  0
/** USB_OTGFS_DOEPINT5_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DOEPINT5_EPDISBLD    (BIT(1))
#define USB_OTGFS_DOEPINT5_EPDISBLD_M  (USB_OTGFS_DOEPINT5_EPDISBLD_V << USB_OTGFS_DOEPINT5_EPDISBLD_S)
#define USB_OTGFS_DOEPINT5_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DOEPINT5_EPDISBLD_S  1
/** USB_OTGFS_DOEPINT5_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DOEPINT5_AHBERR    (BIT(2))
#define USB_OTGFS_DOEPINT5_AHBERR_M  (USB_OTGFS_DOEPINT5_AHBERR_V << USB_OTGFS_DOEPINT5_AHBERR_S)
#define USB_OTGFS_DOEPINT5_AHBERR_V  0x00000001U
#define USB_OTGFS_DOEPINT5_AHBERR_S  2
/** USB_OTGFS_DOEPINT5_SETUP : R/W; bitpos: [3]; default: 0;
 *  SETUP Phase Done (SetUp)
 *
 *  Applies to control OUT endpoints only.
 *
 *  Indicates that the SETUP phase for the control endpoint is
 *  complete and no more back-to-back SETUP packets were
 *  received for the current control transfer. On this interrupt, the
 *  application can decode the received SETUP data packet.
 */
#define USB_OTGFS_DOEPINT5_SETUP    (BIT(3))
#define USB_OTGFS_DOEPINT5_SETUP_M  (USB_OTGFS_DOEPINT5_SETUP_V << USB_OTGFS_DOEPINT5_SETUP_S)
#define USB_OTGFS_DOEPINT5_SETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT5_SETUP_S  3
/** USB_OTGFS_DOEPINT5_OUTTKNEPDIS : R/W; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
 *
 *  Applies only to control OUT endpoints.
 *
 *  Indicates that an OUT token was received when the endpoint was not yet enabled.
 *  This interrupt is asserted on the endpoint for which the OUT token was received.
 */
#define USB_OTGFS_DOEPINT5_OUTTKNEPDIS    (BIT(4))
#define USB_OTGFS_DOEPINT5_OUTTKNEPDIS_M  (USB_OTGFS_DOEPINT5_OUTTKNEPDIS_V << USB_OTGFS_DOEPINT5_OUTTKNEPDIS_S)
#define USB_OTGFS_DOEPINT5_OUTTKNEPDIS_V  0x00000001U
#define USB_OTGFS_DOEPINT5_OUTTKNEPDIS_S  4
/** USB_OTGFS_DOEPINT5_STSPHSERCVD : R/W; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write (StsPhseRcvd)
 *
 *  This interrupt is valid only for Control OUT endpoints and only in
 *  Scatter Gather DMA mode.
 *
 *  This interrupt is generated only after the core has transferred all
 *  the data that the host has sent during the data phase of a control
 *  write transfer, to the system memory buffer.
 *
 *  The interrupt indicates to the application that the host has
 *  switched from data phase to the status phase of a Control Write
 *  transfer. The application can use this interrupt to ACK or STALL
 *  the Status phase, after it has decoded the data phase. This is
 *  applicable only in Case of Scatter Gather DMA mode.
 */
#define USB_OTGFS_DOEPINT5_STSPHSERCVD    (BIT(5))
#define USB_OTGFS_DOEPINT5_STSPHSERCVD_M  (USB_OTGFS_DOEPINT5_STSPHSERCVD_V << USB_OTGFS_DOEPINT5_STSPHSERCVD_S)
#define USB_OTGFS_DOEPINT5_STSPHSERCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT5_STSPHSERCVD_S  5
/** USB_OTGFS_DOEPINT5_BACK2BACKSETUP : R/W; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received (Back2BackSETup)
 *
 *  Applies to Control OUT endpoints only.
 *
 *  This bit indicates that the core has received more than three
 *  back-to-back SETUP packets for this particular endpoint. For
 *  information about handling this interrupt,
 */
#define USB_OTGFS_DOEPINT5_BACK2BACKSETUP    (BIT(6))
#define USB_OTGFS_DOEPINT5_BACK2BACKSETUP_M  (USB_OTGFS_DOEPINT5_BACK2BACKSETUP_V << USB_OTGFS_DOEPINT5_BACK2BACKSETUP_S)
#define USB_OTGFS_DOEPINT5_BACK2BACKSETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT5_BACK2BACKSETUP_S  6
/** USB_OTGFS_DOEPINT5_OUTPKTERR : R/W; bitpos: [8]; default: 0;
 *  OUT Packet Error (OutPktErr)
 *
 *  Applies to OUT endpoints Only
 *
 *  This interrupt is valid only when thresholding is enabled. This interrupt is
 *  asserted when the
 *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
 */
#define USB_OTGFS_DOEPINT5_OUTPKTERR    (BIT(8))
#define USB_OTGFS_DOEPINT5_OUTPKTERR_M  (USB_OTGFS_DOEPINT5_OUTPKTERR_V << USB_OTGFS_DOEPINT5_OUTPKTERR_S)
#define USB_OTGFS_DOEPINT5_OUTPKTERR_V  0x00000001U
#define USB_OTGFS_DOEPINT5_OUTPKTERR_S  8
/** USB_OTGFS_DOEPINT5_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process, such as Host busy or DMA
 *  done
 */
#define USB_OTGFS_DOEPINT5_BNAINTR    (BIT(9))
#define USB_OTGFS_DOEPINT5_BNAINTR_M  (USB_OTGFS_DOEPINT5_BNAINTR_V << USB_OTGFS_DOEPINT5_BNAINTR_S)
#define USB_OTGFS_DOEPINT5_BNAINTR_V  0x00000001U
#define USB_OTGFS_DOEPINT5_BNAINTR_S  9
/** USB_OTGFS_DOEPINT5_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DOEPINT5_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DOEPINT5_PKTDRPSTS_M  (USB_OTGFS_DOEPINT5_PKTDRPSTS_V << USB_OTGFS_DOEPINT5_PKTDRPSTS_S)
#define USB_OTGFS_DOEPINT5_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DOEPINT5_PKTDRPSTS_S  11
/** USB_OTGFS_DOEPINT5_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DOEPINT5_BBLEERR    (BIT(12))
#define USB_OTGFS_DOEPINT5_BBLEERR_M  (USB_OTGFS_DOEPINT5_BBLEERR_V << USB_OTGFS_DOEPINT5_BBLEERR_S)
#define USB_OTGFS_DOEPINT5_BBLEERR_V  0x00000001U
#define USB_OTGFS_DOEPINT5_BBLEERR_S  12
/** USB_OTGFS_DOEPINT5_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DOEPINT5_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DOEPINT5_NAKINTRPT_M  (USB_OTGFS_DOEPINT5_NAKINTRPT_V << USB_OTGFS_DOEPINT5_NAKINTRPT_S)
#define USB_OTGFS_DOEPINT5_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT5_NAKINTRPT_S  13
/** USB_OTGFS_DOEPINT5_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DOEPINT5_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DOEPINT5_NYETINTRPT_M  (USB_OTGFS_DOEPINT5_NYETINTRPT_V << USB_OTGFS_DOEPINT5_NYETINTRPT_S)
#define USB_OTGFS_DOEPINT5_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT5_NYETINTRPT_S  14
/** USB_OTGFS_DOEPINT5_STUPPKTRCVD : R/W; bitpos: [15]; default: 0;
 *  Setup Packet Received
 *
 *  Applicable for Control OUT Endpoints in only in the Buffer DMA Mode
 *
 *  Set by the controller, this bit indicates that this buffer holds 8 bytes of
 *  setup data. There is only one Setup packet per buffer. On receiving a
 *  Setup packet, the controller closes the buffer and disables the
 *  corresponding endpoint. The application has to re-enable the endpoint to
 *  receive any OUT data for the Control Transfer and reprogram the buffer
 *  start address.
 *
 *  Note: Because of the above behavior, the controller can receive any
 *  number of back to back setup packets and one buffer for every setup
 *  packet is used.
 *  - 1'b0: No Setup packet received
 *  - 1'b1: Setup packet received
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPINT5_STUPPKTRCVD    (BIT(15))
#define USB_OTGFS_DOEPINT5_STUPPKTRCVD_M  (USB_OTGFS_DOEPINT5_STUPPKTRCVD_V << USB_OTGFS_DOEPINT5_STUPPKTRCVD_S)
#define USB_OTGFS_DOEPINT5_STUPPKTRCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT5_STUPPKTRCVD_S  15

/** USB_OTGFS_DOEPTSIZ5_REG register
 *  Device OUT Endpoint 5 Transfer Size Register
 */
#define USB_OTGFS_DOEPTSIZ5_REG(i) (DR_REG_USB_BASE(i) + 0xbb0)
/** USB_OTGFS_DOEPTSIZ5_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet is read from
 *  the RxFIFO and written to the external memory.
 */
#define USB_OTGFS_DOEPTSIZ5_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ5_XFERSIZE_M  (USB_OTGFS_DOEPTSIZ5_XFERSIZE_V << USB_OTGFS_DOEPTSIZ5_XFERSIZE_S)
#define USB_OTGFS_DOEPTSIZ5_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ5_XFERSIZE_S  0
/** USB_OTGFS_DOEPTSIZ5_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *  This field is decremented to zero after a packet is written into the RxFIFO.
 */
#define USB_OTGFS_DOEPTSIZ5_PKTCNT    0x0000007FU
#define USB_OTGFS_DOEPTSIZ5_PKTCNT_M  (USB_OTGFS_DOEPTSIZ5_PKTCNT_V << USB_OTGFS_DOEPTSIZ5_PKTCNT_S)
#define USB_OTGFS_DOEPTSIZ5_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DOEPTSIZ5_PKTCNT_S  19
/** USB_OTGFS_DOEPTSIZ5_RXDPID : RO; bitpos: [30:29]; default: 0;
 *  RxDPID
 *
 *  Applies to isochronous OUT endpoints only.
 *
 *  This is the data PID received in the last packet for this endpoint.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA
 *  SETUP Packet Count (SUPCnt)
 *
 *  Applies to control OUT Endpoints only.
 *
 *  This field specifies the number of back-to-back SETUP data
 *  packets the endpoint can receive.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 */
#define USB_OTGFS_DOEPTSIZ5_RXDPID    0x00000003U
#define USB_OTGFS_DOEPTSIZ5_RXDPID_M  (USB_OTGFS_DOEPTSIZ5_RXDPID_V << USB_OTGFS_DOEPTSIZ5_RXDPID_S)
#define USB_OTGFS_DOEPTSIZ5_RXDPID_V  0x00000003U
#define USB_OTGFS_DOEPTSIZ5_RXDPID_S  29

/** USB_OTGFS_DOEPDMA5_REG register
 *  Device OUT Endpoint 5 DMA Address Register
 */
#define USB_OTGFS_DOEPDMA5_REG(i) (DR_REG_USB_BASE(i) + 0xbb4)
/** USB_OTGFS_DOEPDMA5_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DOEPDMA5_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA5_DMAADDR_M  (USB_OTGFS_DOEPDMA5_DMAADDR_V << USB_OTGFS_DOEPDMA5_DMAADDR_S)
#define USB_OTGFS_DOEPDMA5_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA5_DMAADDR_S  0

/** USB_OTGFS_DOEPDMAB5_REG register
 *  Device OUT Endpoint 5 Buffer Address Register
 */
#define USB_OTGFS_DOEPDMAB5_REG(i) (DR_REG_USB_BASE(i) + 0xbbc)
/** USB_OTGFS_DOEPDMAB5_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DOEPDMAB5_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB5_DMABUFFERADDR_M  (USB_OTGFS_DOEPDMAB5_DMABUFFERADDR_V << USB_OTGFS_DOEPDMAB5_DMABUFFERADDR_S)
#define USB_OTGFS_DOEPDMAB5_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB5_DMABUFFERADDR_S  0

/** USB_OTGFS_DOEPCTL6_REG register
 *  Device Control OUT Endpoint 6 Control Register
 */
#define USB_OTGFS_DOEPCTL6_REG(i) (DR_REG_USB_BASE(i) + 0xbc0)
/** USB_OTGFS_DOEPCTL6_MPS : R/W; bitpos: [10:0]; default: 0;
 *  Maximum Packet Size (MPS)
 *
 *  The application must program this field with the maximum packet size for the current
 *  logical endpoint. This value is in bytes.
 */
#define USB_OTGFS_DOEPCTL6_MPS    0x000007FFU
#define USB_OTGFS_DOEPCTL6_MPS_M  (USB_OTGFS_DOEPCTL6_MPS_V << USB_OTGFS_DOEPCTL6_MPS_S)
#define USB_OTGFS_DOEPCTL6_MPS_V  0x000007FFU
#define USB_OTGFS_DOEPCTL6_MPS_S  0
/** USB_OTGFS_DOEPCTL6_USBACTEP : R/W; bitpos: [15]; default: 0;
 *  USB Active Endpoint (USBActEP)
 *
 *  Indicates whether this endpoint is active in the current configuration and
 *  interface. The
 *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
 *  reset. After
 *  receiving the SetConfiguration and SetInterface commands, the application must
 *  program endpoint registers accordingly and set this bit.
 */
#define USB_OTGFS_DOEPCTL6_USBACTEP    (BIT(15))
#define USB_OTGFS_DOEPCTL6_USBACTEP_M  (USB_OTGFS_DOEPCTL6_USBACTEP_V << USB_OTGFS_DOEPCTL6_USBACTEP_S)
#define USB_OTGFS_DOEPCTL6_USBACTEP_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_USBACTEP_S  15
/** USB_OTGFS_DOEPCTL6_DPID : RO; bitpos: [16]; default: 0;
 *  Endpoint Data PID (DPID)
 *
 *  Applies to interrupt/bulk IN and OUT endpoints only.
 *
 *  Contains the PID of the packet to be received or transmitted on this endpoint. The
 *  application must program the PID of the first packet to be received or transmitted
 *  on
 *  this endpoint, after the endpoint is activated. The applications use the SetD1PID
 *  and
 *  SetD0PID fields of this register to program either DATA0 or DATA1 PID.
 *  - 1'b0: DATA0
 *  - 1'b1: DATA1
 *  This field is applicable for both Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *
 *  Reset: 1'b0
 *
 *  Even/Odd (Micro)Frame (EO_FrNum)
 *
 *  In non-Scatter/Gather DMA mode:
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Indicates the (micro)frame number in which the core transmits/receives
 *  isochronous data for this endpoint. The application must program the even/odd
 *  (micro)frame number in which it intends to transmit/receive isochronous data for
 *  this endpoint using the SetEvnFr and SetOddFr fields in this register.
 *  -- 1'b0: Even (micro)frame
 *  -- 1'b1: Odd (micro)frame
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is provided in the transmit descriptor structure. The frame
 *  in which data is received is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL6_DPID    (BIT(16))
#define USB_OTGFS_DOEPCTL6_DPID_M  (USB_OTGFS_DOEPCTL6_DPID_V << USB_OTGFS_DOEPCTL6_DPID_S)
#define USB_OTGFS_DOEPCTL6_DPID_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_DPID_S  16
/** USB_OTGFS_DOEPCTL6_NAKSTS : RO; bitpos: [17]; default: 0;
 *  NAK Status (NAKSts)
 *
 *  Indicates the following:
 *  - 1'b0: The core is transmitting non-NAK handshakes based on the FIFO status.
 *  - 1'b1: The core is transmitting NAK handshakes on this endpoint.
 *  When either the application or the core sets this bit:
 *  - The core stops receiving any data on an OUT endpoint, even if there is space in
 *  the RxFIFO to accommodate the incoming packet.
 *  - For non-isochronous IN endpoints: The core stops transmitting any data on an IN
 *  endpoint, even if there data is available in the TxFIFO.
 *  - For isochronous IN endpoints: The core sends out a zero-length data packet, even
 *  if there data is available in the TxFIFO.
 *  Irrespective of this bit's setting, the core always responds to SETUP data packets
 *  with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL6_NAKSTS    (BIT(17))
#define USB_OTGFS_DOEPCTL6_NAKSTS_M  (USB_OTGFS_DOEPCTL6_NAKSTS_V << USB_OTGFS_DOEPCTL6_NAKSTS_S)
#define USB_OTGFS_DOEPCTL6_NAKSTS_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_NAKSTS_S  17
/** USB_OTGFS_DOEPCTL6_EPTYPE : R/W; bitpos: [19:18]; default: 0;
 *  Endpoint Type (EPType)
 *
 *  This is the transfer type supported by this logical endpoint.
 *  - 2'b00: Control
 *  - 2'b01: Isochronous
 *  - 2'b10: Bulk
 *  - 2'b11: Interrupt
 */
#define USB_OTGFS_DOEPCTL6_EPTYPE    0x00000003U
#define USB_OTGFS_DOEPCTL6_EPTYPE_M  (USB_OTGFS_DOEPCTL6_EPTYPE_V << USB_OTGFS_DOEPCTL6_EPTYPE_S)
#define USB_OTGFS_DOEPCTL6_EPTYPE_V  0x00000003U
#define USB_OTGFS_DOEPCTL6_EPTYPE_S  18
/** USB_OTGFS_DOEPCTL6_SNP : R/W; bitpos: [20]; default: 0;
 *  RESERVED
 */
#define USB_OTGFS_DOEPCTL6_SNP    (BIT(20))
#define USB_OTGFS_DOEPCTL6_SNP_M  (USB_OTGFS_DOEPCTL6_SNP_V << USB_OTGFS_DOEPCTL6_SNP_S)
#define USB_OTGFS_DOEPCTL6_SNP_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_SNP_S  20
/** USB_OTGFS_DOEPCTL6_STALL : R/W; bitpos: [21]; default: 0;
 *  STALL Handshake (Stall)
 *
 *  Applies to non-control, non-isochronous IN and OUT endpoints only.
 *
 *  The application sets this bit to stall all tokens from the USB host to this
 *  endpoint. If a
 *  NAK bit, Global Non-periodic IN NAK, or Global OUT NAK is set along with this bit,
 *  the
 *  STALL bit takes priority. Only the application can clear this bit, never the core.
 *
 *  Applies to control endpoints only.
 *
 *  The application can only set this bit, and the core clears it, when a SETUP token is
 *  received for this endpoint. If a NAK bit, Global Non-periodic IN NAK, or Global OUT
 *  NAK is set along with this bit, the STALL bit takes priority. Irrespective of this
 *  bit's
 *  setting, the core always responds to SETUP data packets with an ACK handshake.
 */
#define USB_OTGFS_DOEPCTL6_STALL    (BIT(21))
#define USB_OTGFS_DOEPCTL6_STALL_M  (USB_OTGFS_DOEPCTL6_STALL_V << USB_OTGFS_DOEPCTL6_STALL_S)
#define USB_OTGFS_DOEPCTL6_STALL_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_STALL_S  21
/** USB_OTGFS_DOEPCTL6_CNAK : R/W; bitpos: [26]; default: 0;
 *
 *  Clear NAK (CNAK)
 *  A write to this bit clears the NAK bit for the endpoint.
 */
#define USB_OTGFS_DOEPCTL6_CNAK    (BIT(26))
#define USB_OTGFS_DOEPCTL6_CNAK_M  (USB_OTGFS_DOEPCTL6_CNAK_V << USB_OTGFS_DOEPCTL6_CNAK_S)
#define USB_OTGFS_DOEPCTL6_CNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_CNAK_S  26
/** USB_OTGFS_DOEPCTL6_SNAK : R/W; bitpos: [27]; default: 0;
 *  Set NAK (SNAK)
 *
 *  A write to this bit sets the NAK bit for the endpoint.
 *
 *  Using this bit, the application can control the transmission of NAK
 *  handshakes on an endpoint. The core can also set this bit for an
 *  endpoint after a SETUP packet is received on that endpoint.
 */
#define USB_OTGFS_DOEPCTL6_SNAK    (BIT(27))
#define USB_OTGFS_DOEPCTL6_SNAK_M  (USB_OTGFS_DOEPCTL6_SNAK_V << USB_OTGFS_DOEPCTL6_SNAK_S)
#define USB_OTGFS_DOEPCTL6_SNAK_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_SNAK_S  27
/** USB_OTGFS_DOEPCTL6_SETD0PID : R/W; bitpos: [28]; default: 0;
 *  Set DATA0 PID (SetD0PID)
 *  - Applies to interrupt/bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA0.
 *  - This field is applicable both for Scatter/Gather DMA mode and non-Scatter/Gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  In non-Scatter/Gather DMA mode: Set Even (micro)frame (SetEvenFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the Even/Odd (micro)frame (EO_FrNum) field to even
 *  (micro)frame.
 *  - When Scatter/Gather DMA mode is enabled, this field is reserved. The frame number
 *  in which to send data is in the transmit descriptor structure. The frame in which
 *  to receive data is updated in receive descriptor structure.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL6_SETD0PID    (BIT(28))
#define USB_OTGFS_DOEPCTL6_SETD0PID_M  (USB_OTGFS_DOEPCTL6_SETD0PID_V << USB_OTGFS_DOEPCTL6_SETD0PID_S)
#define USB_OTGFS_DOEPCTL6_SETD0PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_SETD0PID_S  28
/** USB_OTGFS_DOEPCTL6_SETD1PID : R/W; bitpos: [29]; default: 0;
 *  Set DATA1 PID (SetD1PID)
 *  - Applies to interrupt and bulk IN and OUT endpoints only.
 *  - Writing to this field sets the Endpoint Data PID (DPID) field in this register to
 *  DATA1.
 *  - This field is applicable both for scatter-gather DMA mode and non scatter-gather
 *  DMA mode.
 *  Reset: 1'b0
 *
 *  Set Odd (micro)frame (SetOddFr)
 *  - Applies to isochronous IN and OUT endpoints only.
 *  - Writing to this field sets the even and odd (micro)frame (EO_FrNum) field to odd
 *  (micro)frame.
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPCTL6_SETD1PID    (BIT(29))
#define USB_OTGFS_DOEPCTL6_SETD1PID_M  (USB_OTGFS_DOEPCTL6_SETD1PID_V << USB_OTGFS_DOEPCTL6_SETD1PID_S)
#define USB_OTGFS_DOEPCTL6_SETD1PID_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_SETD1PID_S  29
/** USB_OTGFS_DOEPCTL6_EPDIS : R/W; bitpos: [30]; default: 0;
 *  Endpoint Disable (EPDis)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  The application sets this bit to stop transmitting/receiving data on an endpoint,
 *  even
 *  before the transfer for that endpoint is complete. The application must wait for the
 *  Endpoint Disabled interrupt before treating the endpoint as disabled. The core
 *  clears
 *  this bit before setting the Endpoint Disabled interrupt. The application must set
 *  this bit
 *  only if Endpoint Enable is already set for this endpoint.
 */
#define USB_OTGFS_DOEPCTL6_EPDIS    (BIT(30))
#define USB_OTGFS_DOEPCTL6_EPDIS_M  (USB_OTGFS_DOEPCTL6_EPDIS_V << USB_OTGFS_DOEPCTL6_EPDIS_S)
#define USB_OTGFS_DOEPCTL6_EPDIS_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_EPDIS_S  30
/** USB_OTGFS_DOEPCTL6_EPENA : R/W; bitpos: [31]; default: 0;
 *  Endpoint Enable (EPEna)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  When Scatter/Gather DMA mode is enabled,
 *  - For IN endpoints this bit indicates that the descriptor structure and data buffer
 *  with data ready to transmit is setup.
 *  - For OUT endpoint it indicates that the descriptor structure and data buffer to
 *  receive data is setup.
 *  When Scatter/Gather DMA mode is enabled such as for buffer-pointer based DMA mode:
 *  - For IN endpoints, this bit indicates that data is ready to be transmitted on the
 *  endpoint.
 *  - For OUT endpoints, this bit indicates that the application has allocated the
 *  memory to start receiving data from the USB.
 *  The core clears this bit before setting any of the following interrupts on this
 *  endpoint:
 *  - SETUP Phase Done
 *  - Endpoint Disabled
 *  - Transfer Completed
 *  Note: For control endpoints in DMA mode, this bit must be set to be able to
 *  transfer SETUP data packets in memory.
 */
#define USB_OTGFS_DOEPCTL6_EPENA    (BIT(31))
#define USB_OTGFS_DOEPCTL6_EPENA_M  (USB_OTGFS_DOEPCTL6_EPENA_V << USB_OTGFS_DOEPCTL6_EPENA_S)
#define USB_OTGFS_DOEPCTL6_EPENA_V  0x00000001U
#define USB_OTGFS_DOEPCTL6_EPENA_S  31

/** USB_OTGFS_DOEPINT6_REG register
 *  Device OUT Endpoint 6 Interrupt Register
 */
#define USB_OTGFS_DOEPINT6_REG(i) (DR_REG_USB_BASE(i) + 0xbc8)
/** USB_OTGFS_DOEPINT6_XFERCOMPL : R/W; bitpos: [0]; default: 0;
 *  Transfer Completed Interrupt (XferCompl)
 *
 *  Applies to IN and OUT endpoints.
 *  - When Scatter/Gather DMA mode is enabled
 *  -- For IN endpoint this field indicates that the requested data from the descriptor
 *  is moved from external system memory to internal FIFO.
 *  -- For OUT endpoint this field indicates that the requested data from the internal
 *  FIFO is moved to external system memory. This interrupt is generated only when the
 *  corresponding endpoint descriptor is closed, and the IOC bit for the corresponding
 *  descriptor is Set.
 *  - When Scatter/Gather DMA mode is disabled, this field indicates that the
 *  programmed transfer is complete on the AHB as well as on the USB, for this endpoint.
 */
#define USB_OTGFS_DOEPINT6_XFERCOMPL    (BIT(0))
#define USB_OTGFS_DOEPINT6_XFERCOMPL_M  (USB_OTGFS_DOEPINT6_XFERCOMPL_V << USB_OTGFS_DOEPINT6_XFERCOMPL_S)
#define USB_OTGFS_DOEPINT6_XFERCOMPL_V  0x00000001U
#define USB_OTGFS_DOEPINT6_XFERCOMPL_S  0
/** USB_OTGFS_DOEPINT6_EPDISBLD : R/W; bitpos: [1]; default: 0;
 *  Endpoint Disabled Interrupt (EPDisbld)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This bit indicates that the endpoint is disabled per the application's request.
 */
#define USB_OTGFS_DOEPINT6_EPDISBLD    (BIT(1))
#define USB_OTGFS_DOEPINT6_EPDISBLD_M  (USB_OTGFS_DOEPINT6_EPDISBLD_V << USB_OTGFS_DOEPINT6_EPDISBLD_S)
#define USB_OTGFS_DOEPINT6_EPDISBLD_V  0x00000001U
#define USB_OTGFS_DOEPINT6_EPDISBLD_S  1
/** USB_OTGFS_DOEPINT6_AHBERR : R/W; bitpos: [2]; default: 0;
 *  AHB Error (AHBErr)
 *
 *  Applies to IN and OUT endpoints.
 *
 *  This is generated only in Internal DMA mode when there is an
 *  AHB error during an AHB read/write. The application can read
 *  the corresponding endpoint DMA address register to get the
 *  error address.
 */
#define USB_OTGFS_DOEPINT6_AHBERR    (BIT(2))
#define USB_OTGFS_DOEPINT6_AHBERR_M  (USB_OTGFS_DOEPINT6_AHBERR_V << USB_OTGFS_DOEPINT6_AHBERR_S)
#define USB_OTGFS_DOEPINT6_AHBERR_V  0x00000001U
#define USB_OTGFS_DOEPINT6_AHBERR_S  2
/** USB_OTGFS_DOEPINT6_SETUP : R/W; bitpos: [3]; default: 0;
 *  SETUP Phase Done (SetUp)
 *
 *  Applies to control OUT endpoints only.
 *
 *  Indicates that the SETUP phase for the control endpoint is
 *  complete and no more back-to-back SETUP packets were
 *  received for the current control transfer. On this interrupt, the
 *  application can decode the received SETUP data packet.
 */
#define USB_OTGFS_DOEPINT6_SETUP    (BIT(3))
#define USB_OTGFS_DOEPINT6_SETUP_M  (USB_OTGFS_DOEPINT6_SETUP_V << USB_OTGFS_DOEPINT6_SETUP_S)
#define USB_OTGFS_DOEPINT6_SETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT6_SETUP_S  3
/** USB_OTGFS_DOEPINT6_OUTTKNEPDIS : R/W; bitpos: [4]; default: 0;
 *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
 *
 *  Applies only to control OUT endpoints.
 *
 *  Indicates that an OUT token was received when the endpoint was not yet enabled.
 *  This interrupt is asserted on the endpoint for which the OUT token was received.
 */
#define USB_OTGFS_DOEPINT6_OUTTKNEPDIS    (BIT(4))
#define USB_OTGFS_DOEPINT6_OUTTKNEPDIS_M  (USB_OTGFS_DOEPINT6_OUTTKNEPDIS_V << USB_OTGFS_DOEPINT6_OUTTKNEPDIS_S)
#define USB_OTGFS_DOEPINT6_OUTTKNEPDIS_V  0x00000001U
#define USB_OTGFS_DOEPINT6_OUTTKNEPDIS_S  4
/** USB_OTGFS_DOEPINT6_STSPHSERCVD : R/W; bitpos: [5]; default: 0;
 *  Status Phase Received for Control Write (StsPhseRcvd)
 *
 *  This interrupt is valid only for Control OUT endpoints and only in
 *  Scatter Gather DMA mode.
 *
 *  This interrupt is generated only after the core has transferred all
 *  the data that the host has sent during the data phase of a control
 *  write transfer, to the system memory buffer.
 *
 *  The interrupt indicates to the application that the host has
 *  switched from data phase to the status phase of a Control Write
 *  transfer. The application can use this interrupt to ACK or STALL
 *  the Status phase, after it has decoded the data phase. This is
 *  applicable only in Case of Scatter Gather DMA mode.
 */
#define USB_OTGFS_DOEPINT6_STSPHSERCVD    (BIT(5))
#define USB_OTGFS_DOEPINT6_STSPHSERCVD_M  (USB_OTGFS_DOEPINT6_STSPHSERCVD_V << USB_OTGFS_DOEPINT6_STSPHSERCVD_S)
#define USB_OTGFS_DOEPINT6_STSPHSERCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT6_STSPHSERCVD_S  5
/** USB_OTGFS_DOEPINT6_BACK2BACKSETUP : R/W; bitpos: [6]; default: 0;
 *  Back-to-Back SETUP Packets Received (Back2BackSETup)
 *
 *  Applies to Control OUT endpoints only.
 *
 *  This bit indicates that the core has received more than three
 *  back-to-back SETUP packets for this particular endpoint. For
 *  information about handling this interrupt,
 */
#define USB_OTGFS_DOEPINT6_BACK2BACKSETUP    (BIT(6))
#define USB_OTGFS_DOEPINT6_BACK2BACKSETUP_M  (USB_OTGFS_DOEPINT6_BACK2BACKSETUP_V << USB_OTGFS_DOEPINT6_BACK2BACKSETUP_S)
#define USB_OTGFS_DOEPINT6_BACK2BACKSETUP_V  0x00000001U
#define USB_OTGFS_DOEPINT6_BACK2BACKSETUP_S  6
/** USB_OTGFS_DOEPINT6_OUTPKTERR : R/W; bitpos: [8]; default: 0;
 *  OUT Packet Error (OutPktErr)
 *
 *  Applies to OUT endpoints Only
 *
 *  This interrupt is valid only when thresholding is enabled. This interrupt is
 *  asserted when the
 *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
 */
#define USB_OTGFS_DOEPINT6_OUTPKTERR    (BIT(8))
#define USB_OTGFS_DOEPINT6_OUTPKTERR_M  (USB_OTGFS_DOEPINT6_OUTPKTERR_V << USB_OTGFS_DOEPINT6_OUTPKTERR_S)
#define USB_OTGFS_DOEPINT6_OUTPKTERR_V  0x00000001U
#define USB_OTGFS_DOEPINT6_OUTPKTERR_S  8
/** USB_OTGFS_DOEPINT6_BNAINTR : R/W; bitpos: [9]; default: 0;
 *  BNA (Buffer Not Available) Interrupt (BNAIntr)
 *
 *  This bit is valid only when Scatter/Gather DMA mode is enabled.
 *
 *  The core generates this interrupt when the descriptor accessed
 *  is not ready for the Core to process, such as Host busy or DMA
 *  done
 */
#define USB_OTGFS_DOEPINT6_BNAINTR    (BIT(9))
#define USB_OTGFS_DOEPINT6_BNAINTR_M  (USB_OTGFS_DOEPINT6_BNAINTR_V << USB_OTGFS_DOEPINT6_BNAINTR_S)
#define USB_OTGFS_DOEPINT6_BNAINTR_V  0x00000001U
#define USB_OTGFS_DOEPINT6_BNAINTR_S  9
/** USB_OTGFS_DOEPINT6_PKTDRPSTS : R/W; bitpos: [11]; default: 0;
 *  Packet Drop Status (PktDrpSts)
 *
 *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
 *  bit does not have an associated mask bit and does not generate an interrupt.
 *
 *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
 *  interrupt feature is selected.
 */
#define USB_OTGFS_DOEPINT6_PKTDRPSTS    (BIT(11))
#define USB_OTGFS_DOEPINT6_PKTDRPSTS_M  (USB_OTGFS_DOEPINT6_PKTDRPSTS_V << USB_OTGFS_DOEPINT6_PKTDRPSTS_S)
#define USB_OTGFS_DOEPINT6_PKTDRPSTS_V  0x00000001U
#define USB_OTGFS_DOEPINT6_PKTDRPSTS_S  11
/** USB_OTGFS_DOEPINT6_BBLEERR : R/W; bitpos: [12]; default: 0;
 *  NAK Interrupt (BbleErr)
 *
 *  The core generates this interrupt when babble is received for the endpoint.
 */
#define USB_OTGFS_DOEPINT6_BBLEERR    (BIT(12))
#define USB_OTGFS_DOEPINT6_BBLEERR_M  (USB_OTGFS_DOEPINT6_BBLEERR_V << USB_OTGFS_DOEPINT6_BBLEERR_S)
#define USB_OTGFS_DOEPINT6_BBLEERR_V  0x00000001U
#define USB_OTGFS_DOEPINT6_BBLEERR_S  12
/** USB_OTGFS_DOEPINT6_NAKINTRPT : R/W; bitpos: [13]; default: 0;
 *  NAK Interrupt (NAKInterrupt)
 *
 *  The core generates this interrupt when a NAK is transmitted or received by the
 *  device.
 *
 *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
 *  packet is transmitted due to un-availability of data in the TXFifo.
 */
#define USB_OTGFS_DOEPINT6_NAKINTRPT    (BIT(13))
#define USB_OTGFS_DOEPINT6_NAKINTRPT_M  (USB_OTGFS_DOEPINT6_NAKINTRPT_V << USB_OTGFS_DOEPINT6_NAKINTRPT_S)
#define USB_OTGFS_DOEPINT6_NAKINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT6_NAKINTRPT_S  13
/** USB_OTGFS_DOEPINT6_NYETINTRPT : R/W; bitpos: [14]; default: 0;
 *  NYET Interrupt (NYETIntrpt)
 *
 *  The core generates this interrupt when a NYET response is transmitted for a non
 *  isochronous OUT endpoint.
 */
#define USB_OTGFS_DOEPINT6_NYETINTRPT    (BIT(14))
#define USB_OTGFS_DOEPINT6_NYETINTRPT_M  (USB_OTGFS_DOEPINT6_NYETINTRPT_V << USB_OTGFS_DOEPINT6_NYETINTRPT_S)
#define USB_OTGFS_DOEPINT6_NYETINTRPT_V  0x00000001U
#define USB_OTGFS_DOEPINT6_NYETINTRPT_S  14
/** USB_OTGFS_DOEPINT6_STUPPKTRCVD : R/W; bitpos: [15]; default: 0;
 *  Setup Packet Received
 *
 *  Applicable for Control OUT Endpoints in only in the Buffer DMA Mode
 *
 *  Set by the controller, this bit indicates that this buffer holds 8 bytes of
 *  setup data. There is only one Setup packet per buffer. On receiving a
 *  Setup packet, the controller closes the buffer and disables the
 *  corresponding endpoint. The application has to re-enable the endpoint to
 *  receive any OUT data for the Control Transfer and reprogram the buffer
 *  start address.
 *
 *  Note: Because of the above behavior, the controller can receive any
 *  number of back to back setup packets and one buffer for every setup
 *  packet is used.
 *  - 1'b0: No Setup packet received
 *  - 1'b1: Setup packet received
 *  Reset: 1'b0
 */
#define USB_OTGFS_DOEPINT6_STUPPKTRCVD    (BIT(15))
#define USB_OTGFS_DOEPINT6_STUPPKTRCVD_M  (USB_OTGFS_DOEPINT6_STUPPKTRCVD_V << USB_OTGFS_DOEPINT6_STUPPKTRCVD_S)
#define USB_OTGFS_DOEPINT6_STUPPKTRCVD_V  0x00000001U
#define USB_OTGFS_DOEPINT6_STUPPKTRCVD_S  15

/** USB_OTGFS_DOEPTSIZ6_REG register
 *  Device OUT Endpoint 6 Transfer Size Register
 */
#define USB_OTGFS_DOEPTSIZ6_REG(i) (DR_REG_USB_BASE(i) + 0xbd0)
/** USB_OTGFS_DOEPTSIZ6_XFERSIZE : R/W; bitpos: [15:0]; default: 0;
 *  Transfer Size (XferSize)
 *
 *  Indicates the transfer size in bytes for endpoint 0. The core
 *  interrupts the application only after it has exhausted the transfer
 *  size amount of data. The transfer size can be Set to the
 *  maximum packet size of the endpoint, to be interrupted at the
 *  end of each packet.
 *
 *  The core decrements this field every time a packet is read from
 *  the RxFIFO and written to the external memory.
 */
#define USB_OTGFS_DOEPTSIZ6_XFERSIZE    0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ6_XFERSIZE_M  (USB_OTGFS_DOEPTSIZ6_XFERSIZE_V << USB_OTGFS_DOEPTSIZ6_XFERSIZE_S)
#define USB_OTGFS_DOEPTSIZ6_XFERSIZE_V  0x0000FFFFU
#define USB_OTGFS_DOEPTSIZ6_XFERSIZE_S  0
/** USB_OTGFS_DOEPTSIZ6_PKTCNT : R/W; bitpos: [25:19]; default: 0;
 *  Packet Count (PktCnt)
 *  This field is decremented to zero after a packet is written into the RxFIFO.
 */
#define USB_OTGFS_DOEPTSIZ6_PKTCNT    0x0000007FU
#define USB_OTGFS_DOEPTSIZ6_PKTCNT_M  (USB_OTGFS_DOEPTSIZ6_PKTCNT_V << USB_OTGFS_DOEPTSIZ6_PKTCNT_S)
#define USB_OTGFS_DOEPTSIZ6_PKTCNT_V  0x0000007FU
#define USB_OTGFS_DOEPTSIZ6_PKTCNT_S  19
/** USB_OTGFS_DOEPTSIZ6_RXDPID : RO; bitpos: [30:29]; default: 0;
 *  RxDPID
 *
 *  Applies to isochronous OUT endpoints only.
 *
 *  This is the data PID received in the last packet for this endpoint.
 *  - 2'b00: DATA0
 *  - 2'b01: DATA2
 *  - 2'b10: DATA1
 *  - 2'b11: MDATA
 *  SETUP Packet Count (SUPCnt)
 *
 *  Applies to control OUT Endpoints only.
 *
 *  This field specifies the number of back-to-back SETUP data
 *  packets the endpoint can receive.
 *  - 2'b01: 1 packet
 *  - 2'b10: 2 packets
 *  - 2'b11: 3 packets
 */
#define USB_OTGFS_DOEPTSIZ6_RXDPID    0x00000003U
#define USB_OTGFS_DOEPTSIZ6_RXDPID_M  (USB_OTGFS_DOEPTSIZ6_RXDPID_V << USB_OTGFS_DOEPTSIZ6_RXDPID_S)
#define USB_OTGFS_DOEPTSIZ6_RXDPID_V  0x00000003U
#define USB_OTGFS_DOEPTSIZ6_RXDPID_S  29

/** USB_OTGFS_DOEPDMA6_REG register
 *  Device OUT Endpoint 6 DMA Address Register
 */
#define USB_OTGFS_DOEPDMA6_REG(i) (DR_REG_USB_BASE(i) + 0xbd4)
/** USB_OTGFS_DOEPDMA6_DMAADDR : R/W; bitpos: [31:0]; default: 0;
 *  Holds the start address of the external memory for storing or fetching endpoint
 *  data.
 *
 *  Note: For control endpoints, this field stores control OUT data packets as well as
 *  SETUP transaction data packets. When more than three SETUP packets are
 *  received back-to-back, the SETUP data packet in the memory is overwritten.
 *
 *  This register is incremented on every AHB transaction. The application can give
 *  only a DWORD-aligned address.
 *  - When Scatter/Gather DMA mode is not enabled, the application programs the start
 *  address value in this field.
 *  - When Scatter/Gather DMA mode is enabled, this field indicates the base pointer
 *  for the descriptor list.
 */
#define USB_OTGFS_DOEPDMA6_DMAADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA6_DMAADDR_M  (USB_OTGFS_DOEPDMA6_DMAADDR_V << USB_OTGFS_DOEPDMA6_DMAADDR_S)
#define USB_OTGFS_DOEPDMA6_DMAADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMA6_DMAADDR_S  0

/** USB_OTGFS_DOEPDMAB6_REG register
 *  Device OUT Endpoint 6 Buffer Address Register
 */
#define USB_OTGFS_DOEPDMAB6_REG(i) (DR_REG_USB_BASE(i) + 0xbdc)
/** USB_OTGFS_DOEPDMAB6_DMABUFFERADDR : RO; bitpos: [31:0]; default: 0;
 *  Holds the current buffer address.This register is updated as and when the data
 *  transfer for the corresponding end point is in progress.
 *
 *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
 *  reserved.
 */
#define USB_OTGFS_DOEPDMAB6_DMABUFFERADDR    0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB6_DMABUFFERADDR_M  (USB_OTGFS_DOEPDMAB6_DMABUFFERADDR_V << USB_OTGFS_DOEPDMAB6_DMABUFFERADDR_S)
#define USB_OTGFS_DOEPDMAB6_DMABUFFERADDR_V  0xFFFFFFFFU
#define USB_OTGFS_DOEPDMAB6_DMABUFFERADDR_S  0

/** USB_OTGFS_PCGCCTL_REG register
 *  Power and Clock Gating Control Register
 */
#define USB_OTGFS_PCGCCTL_REG(i) (DR_REG_USB_BASE(i) + 0xe00)
/** USB_OTGFS_PCGCCTL_STOPPCLK : R/W; bitpos: [0]; default: 0;
 *  Stop Pclk (StopPclk)
 *  - The application sets this bit to stop the PHY clock (phy_clk) when the USB is
 *  suspended, the session is not valid, or the device is disconnected.
 *  - The application clears this bit when the USB is resumed or a new session starts.
 */
#define USB_OTGFS_PCGCCTL_STOPPCLK    (BIT(0))
#define USB_OTGFS_PCGCCTL_STOPPCLK_M  (USB_OTGFS_PCGCCTL_STOPPCLK_V << USB_OTGFS_PCGCCTL_STOPPCLK_S)
#define USB_OTGFS_PCGCCTL_STOPPCLK_V  0x00000001U
#define USB_OTGFS_PCGCCTL_STOPPCLK_S  0
/** USB_OTGFS_PCGCCTL_GATEHCLK : R/W; bitpos: [1]; default: 0;
 *  Gate Hclk (GateHclk)
 *  - The application sets this bit to gate hclk to modules other than the AHB Slave
 *  and Master and wakeup logic when the USB is suspended or the session is not valid.
 *  - The application clears this bit when the USB is resumed or a new session starts.
 */
#define USB_OTGFS_PCGCCTL_GATEHCLK    (BIT(1))
#define USB_OTGFS_PCGCCTL_GATEHCLK_M  (USB_OTGFS_PCGCCTL_GATEHCLK_V << USB_OTGFS_PCGCCTL_GATEHCLK_S)
#define USB_OTGFS_PCGCCTL_GATEHCLK_V  0x00000001U
#define USB_OTGFS_PCGCCTL_GATEHCLK_S  1
/** USB_OTGFS_PCGCCTL_PWRCLMP : R/W; bitpos: [2]; default: 0;
 *  Power Clamp (PwrClmp)
 *
 *  This bit is valid only in Partial Power-Down mode
 *  - The application sets this bit before the power is turned off to clamp the signals
 *  between the power-on modules and the power-off modules.
 *  - The application clears the bit to disable the clamping before the power is turned
 *  on.
 */
#define USB_OTGFS_PCGCCTL_PWRCLMP    (BIT(2))
#define USB_OTGFS_PCGCCTL_PWRCLMP_M  (USB_OTGFS_PCGCCTL_PWRCLMP_V << USB_OTGFS_PCGCCTL_PWRCLMP_S)
#define USB_OTGFS_PCGCCTL_PWRCLMP_V  0x00000001U
#define USB_OTGFS_PCGCCTL_PWRCLMP_S  2
/** USB_OTGFS_PCGCCTL_RSTPDWNMODULE : R/W; bitpos: [3]; default: 0;
 *  Reset Power-Down Modules (RstPdwnModule)
 *
 *  This bit is valid only in Partial Power-Down mode.
 *  - The application sets this bit when the power is turned off.
 *  - The application clears this bit after the power is turned on and the PHY clock is
 *  up.
 *
 *  Note: The R/W of all core registers are possible only when this bit is set to 1b0.
 */
#define USB_OTGFS_PCGCCTL_RSTPDWNMODULE    (BIT(3))
#define USB_OTGFS_PCGCCTL_RSTPDWNMODULE_M  (USB_OTGFS_PCGCCTL_RSTPDWNMODULE_V << USB_OTGFS_PCGCCTL_RSTPDWNMODULE_S)
#define USB_OTGFS_PCGCCTL_RSTPDWNMODULE_V  0x00000001U
#define USB_OTGFS_PCGCCTL_RSTPDWNMODULE_S  3
/** USB_OTGFS_PCGCCTL_PHYSLEEP : RO; bitpos: [6]; default: 0;
 *  PHY In Sleep
 *
 *  Indicates that the PHY is in Sleep State.
 */
#define USB_OTGFS_PCGCCTL_PHYSLEEP    (BIT(6))
#define USB_OTGFS_PCGCCTL_PHYSLEEP_M  (USB_OTGFS_PCGCCTL_PHYSLEEP_V << USB_OTGFS_PCGCCTL_PHYSLEEP_S)
#define USB_OTGFS_PCGCCTL_PHYSLEEP_V  0x00000001U
#define USB_OTGFS_PCGCCTL_PHYSLEEP_S  6
/** USB_OTGFS_PCGCCTL_L1SUSPENDED : RO; bitpos: [7]; default: 0;
 *  L1 Deep Sleep
 *
 *  Indicates that the PHY is in deep sleep when in L1 state.
 */
#define USB_OTGFS_PCGCCTL_L1SUSPENDED    (BIT(7))
#define USB_OTGFS_PCGCCTL_L1SUSPENDED_M  (USB_OTGFS_PCGCCTL_L1SUSPENDED_V << USB_OTGFS_PCGCCTL_L1SUSPENDED_S)
#define USB_OTGFS_PCGCCTL_L1SUSPENDED_V  0x00000001U
#define USB_OTGFS_PCGCCTL_L1SUSPENDED_S  7
/** USB_OTGFS_PCGCCTL_RESETAFTERSUSP : R/W; bitpos: [8]; default: 0;
 *  Reset after suspend
 *
 *  Applicable in Partial power-down mode
 *
 *  In partial power-down mode of operation, this bit needs to be set in host mode
 *  before
 *  clamp is removed if the host needs to issue reset after suspend. If this bit is not
 *  set,
 *  then the host issues resume after suspend.
 *
 *  This bit is not applicable in device mode and non-partial power-down mode.
 *  In Hibernation mode, this bit needs to be set at RESTORE_POINT before
 *  PCGCCTL.EssRegRestored is set. In this case, PCGCCTL.restore_mode needs to
 *  be set to wait_restore.
 */
#define USB_OTGFS_PCGCCTL_RESETAFTERSUSP    (BIT(8))
#define USB_OTGFS_PCGCCTL_RESETAFTERSUSP_M  (USB_OTGFS_PCGCCTL_RESETAFTERSUSP_V << USB_OTGFS_PCGCCTL_RESETAFTERSUSP_S)
#define USB_OTGFS_PCGCCTL_RESETAFTERSUSP_V  0x00000001U
#define USB_OTGFS_PCGCCTL_RESETAFTERSUSP_S  8

#ifdef __cplusplus
}
#endif
