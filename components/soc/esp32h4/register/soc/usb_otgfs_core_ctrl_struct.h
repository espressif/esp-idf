/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: OTG Control and Status Register */
/** Type of otgfs_gotgctl register
 *  OTG Control and Status Register The OTG Control and Status register controls the
 *  behavior and reflects the status of the OTG function of the controller.
 */
typedef union {
    struct {
        /** otgfs_gotgctl_sesreqscs : RO; bitpos: [0]; default: 0;
         *  Mode: Device only
         *  Session Request Success (SesReqScs)
         *  The core sets this bit when a session request initiation is successful.
         *  - 1'b0: Session request failure
         *  - 1'b1: Session request success
         */
        uint32_t otgfs_gotgctl_sesreqscs:1;
        /** otgfs_gotgctl_sesreq : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_gotgctl_sesreq:1;
        /** otgfs_gotgctl_vbvalidoven : R/W; bitpos: [2]; default: 0;
         *  Mode: Host only
         *  VBUS Valid Override Enable (VbvalidOvEn)
         *  This bit is used to enable/disable the software to override the Bvalid signal using
         *  the GOTGCTL.VbvalidOvVal.
         *  - 1'b1 : Internally Bvalid received from the PHY is overridden with
         *  GOTGCTL.VbvalidOvVal.
         *  - 1'b0 : Override is disabled and bvalid signal from the respective PHY selected is
         *  used internally by the controller.
         */
        uint32_t otgfs_gotgctl_vbvalidoven:1;
        /** otgfs_gotgctl_vbvalidovval : R/W; bitpos: [3]; default: 0;
         *  Mode: Host only
         *  VBUS Valid OverrideValue (VbvalidOvVal)
         *  This bit is used to set Override value for vbusvalid signal when
         *  GOTGCTL.VbvalidOvEn is set.
         *  - 1'b0 : vbusvalid value is 1'b0 when GOTGCTL.VbvalidOvEn =1
         *  - 1'b1 : vbusvalid value is 1'b1 when GOTGCTL.VbvalidOvEn =1
         */
        uint32_t otgfs_gotgctl_vbvalidovval:1;
        /** otgfs_gotgctl_avalidoven : R/W; bitpos: [4]; default: 0;
         *  Mode: Host only
         *  A-Peripheral Session Valid Override Enable (AvalidOvEn)
         *  This bit is used to enable/disable the software to override the Avalid signal using
         *  the GOTGCTL.AvalidOvVal.
         *  - 1'b1: Internally Avalid received from the PHY is overridden with
         *  GOTGCTL.AvalidOvVal.
         *  - 1'b0: Override is disabled and avalid signal from the respective PHY selected is
         *  used internally by the core
         */
        uint32_t otgfs_gotgctl_avalidoven:1;
        /** otgfs_gotgctl_avalidovval : R/W; bitpos: [5]; default: 0;
         *  Mode: Host only
         *  A-Peripheral Session Valid OverrideValue (AvalidOvVal)
         *  This bit is used to set Override value for Avalid signal when GOTGCTL.AvalidOvEn is
         *  set.
         *  - 1'b0 : Avalid value is 1'b0 when GOTGCTL.AvalidOvEn =1
         *  - 1'b1 : Avalid value is 1'b1 when GOTGCTL.AvalidOvEn =1
         */
        uint32_t otgfs_gotgctl_avalidovval:1;
        /** otgfs_gotgctl_bvalidoven : R/W; bitpos: [6]; default: 0;
         *  Mode: Device only
         *  B-Peripheral Session Valid Override Value (BvalidOvEn)
         *  This bit is used to enable/disable the software to override the Bvalid signal using
         *  the GOTGCTL.BvalidOvVal.
         *  - 1'b1 : Internally Bvalid received from the PHY is overridden with
         *  GOTGCTL.BvalidOvVal.
         *  - 1'b0 : Override is disabled and bvalid signal from the respective PHY selected is
         *  used internally by the force
         */
        uint32_t otgfs_gotgctl_bvalidoven:1;
        /** otgfs_gotgctl_bvalidovval : R/W; bitpos: [7]; default: 0;
         *  Mode: Device only
         *  B-Peripheral Session Valid OverrideValue (BvalidOvVal)
         *  This bit is used to set Override value for Bvalid signal when GOTGCTL.BvalidOvEn is
         *  set.
         *  - 1'b0 : Bvalid value is 1'b0 when GOTGCTL.BvalidOvEn =1
         *  - 1'b1 : Bvalid value is 1'b1 when GOTGCTL.BvalidOvEn =1
         */
        uint32_t otgfs_gotgctl_bvalidovval:1;
        /** otgfs_gotgctl_hstnegscs : RO; bitpos: [8]; default: 0;
         *  Mode: HNP-capable Device
         *  Host Negotiation Success (HstNegScs)
         *  The controller sets this bit when host negotiation is successful. The controller
         *  clears this bit when the HNP Request (HNPReq) bit in this register is set.
         *  - 1'b0: Host negotiation failure
         *  - 1'b1: Host negotiation success
         */
        uint32_t otgfs_gotgctl_hstnegscs:1;
        /** otgfs_gotgctl_hnpreq : R/W; bitpos: [9]; default: 0;
         *  Mode: HNP Capable OTG Device
         *  HNP Request (HNPReq)
         *  The application sets this bit to initiate an HNP request to the connected USB host.
         *  The application can clear this bit by writing a 0 when the Host Negotiation Success
         *  Status Change bit in the OTG Interrupt register (GOTGINT.HstNegSucStsChng) is SET.
         *  The controller clears this bit when the HstNegSucStsChng bit is cleared.
         *  - 1'b0: No HNP request
         *  - 1'b1: HNP request
         */
        uint32_t otgfs_gotgctl_hnpreq:1;
        /** otgfs_gotgctl_hstsethnpen : R/W; bitpos: [10]; default: 0;
         *  Mode: HNP Capable OTG Host
         *  Host Set HNP Enable (HstSetHNPEn)
         *  The application sets this bit when it has successfully enabled HNP (using the
         *  SetFeature.SetHNPEnable command) on the connected device.
         *  - 1'b0: Host Set HNP is not enabled
         *  - 1'b1: Host Set HNP is enabled
         */
        uint32_t otgfs_gotgctl_hstsethnpen:1;
        /** otgfs_gotgctl_devhnpen : R/W; bitpos: [11]; default: 0;
         *  Mode: HNP Capable OTG Device
         *  Device HNP Enabled (DevHNPEn)
         *  The application sets this bit when it successfully receives a
         *  SetFeature.SetHNPEnable command from the connected USB host.
         *  - 1'b0: HNP is not enabled in the application
         *  - 1'b1: HNP is enabled in the application
         */
        uint32_t otgfs_gotgctl_devhnpen:1;
        /** otgfs_gotgctl_ehen : R/W; bitpos: [12]; default: 0;
         *  Mode: SRP Capable Host
         *  Embedded Host Enable
         *  It is used to select between OTG A Device state Machine and Embedded Host state
         *  machine.
         *  - 1'b0: OTG A Device state machine is selected
         *  - 1'b1: Embedded Host State Machine is selected
         *  Note:
         *  This field is valid only in SRP-Capable OTG Mode (OTG_MODE=0,1).
         */
        uint32_t otgfs_gotgctl_ehen:1;
        uint32_t reserved_13:2;
        /** otgfs_gotgctl_dbncefltrbypass : R/W; bitpos: [15]; default: 0;
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
        uint32_t otgfs_gotgctl_dbncefltrbypass:1;
        /** otgfs_gotgctl_conidsts : RO; bitpos: [16]; default: 1;
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
        uint32_t otgfs_gotgctl_conidsts:1;
        /** otgfs_gotgctl_dbnctime : RO; bitpos: [17]; default: 0;
         *  Mode: Host only
         *  Long/Short Debounce Time (DbncTime)
         *  Indicates the debounce time of a detected connection.
         *  - 1'b0: Long debounce time, used for physical connections (100 ms + 2.5 micro-sec)
         *  - 1'b1: Short debounce time, used for soft connections (2.5 micro-sec)
         */
        uint32_t otgfs_gotgctl_dbnctime:1;
        /** otgfs_gotgctl_asesvld : RO; bitpos: [18]; default: 0;
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
        uint32_t otgfs_gotgctl_asesvld:1;
        /** otgfs_gotgctl_bsesvld : RO; bitpos: [19]; default: 0;
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
        uint32_t otgfs_gotgctl_bsesvld:1;
        /** otgfs_gotgctl_otgver : R/W; bitpos: [20]; default: 0;
         *  OTG Version (OTGVer)
         *  Indicates the OTG revision.
         *  - 1'b0: OTG Version 1.3. In this version the core supports Data line pulsing and
         *  VBus pulsing for SRP.
         *  - 1'b1: OTG Version 2.0. In this version the core supports only Data line pulsing
         *  for SRP.
         */
        uint32_t otgfs_gotgctl_otgver:1;
        /** otgfs_gotgctl_curmod : RO; bitpos: [21]; default: 0;
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
        uint32_t otgfs_gotgctl_curmod:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} usb_otgfs_gotgctl_reg_t;


/** Group: OTG Interrupt Register */
/** Type of otgfs_gotgint register
 *  OTG Interrupt Register The application reads this register whenever there is an OTG
 *  interrupt and clears the bits in this register to clear the OTG interrupt.
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** otgfs_gotgint_sesenddet : R/W; bitpos: [2]; default: 0;
         *  Mode: Host and Device
         *  Session End Detected (SesEndDet)
         *  The controller sets this bit when the utmiotg_bvalid signal is deasserted. This bit
         *  can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_gotgint_sesenddet:1;
        uint32_t reserved_3:5;
        /** otgfs_gotgint_sesreqsucstschng : R/W; bitpos: [8]; default: 0;
         *  Mode: Host and Device
         *  Session Request Success Status Change (SesReqSucStsChng)
         *  The core sets this bit on the success or failure of a session request. The
         *  application must read the Session Request Success bit in the OTG Control and Status
         *  register (GOTGCTL.SesReqScs) to check for success or failure. This bit can be set
         *  only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_gotgint_sesreqsucstschng:1;
        /** otgfs_gotgint_hstnegsucstschng : R/W; bitpos: [9]; default: 0;
         *  Mode: Host and Device
         *  Host Negotiation Success Status Change (HstNegSucStsChng)
         *  The core sets this bit on the success or failure of a USB host negotiation request.
         *  The application must read the Host Negotiation Success bit of the OTG Control and
         *  Status register (GOTGCTL.HstNegScs) to check for success or failure. This bit can
         *  be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_gotgint_hstnegsucstschng:1;
        uint32_t reserved_10:7;
        /** otgfs_gotgint_hstnegdet : R/W; bitpos: [17]; default: 0;
         *  Mode:Host and Device
         *  Host Negotiation Detected (HstNegDet)
         *  The core sets this bit when it detects a host negotiation request on the USB. This
         *  bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_gotgint_hstnegdet:1;
        /** otgfs_gotgint_adevtoutchg : R/W; bitpos: [18]; default: 0;
         *  Mode: Host and Device
         *  A-Device Timeout Change (ADevTOUTChg)
         *  The core sets this bit to indicate that the A-device has timed out while waiting
         *  for the B-device to connect.This bit can be set only by the core and the
         *  application should write 1 to clear it.
         */
        uint32_t otgfs_gotgint_adevtoutchg:1;
        /** otgfs_gotgint_dbncedone : R/W; bitpos: [19]; default: 0;
         *  Mode: Host only
         *  Debounce Done (DbnceDone)
         *  The core sets this bit when the debounce is completed after the device connect. The
         *  application can start driving USB reset after seeing this interrupt. This bit is
         *  only valid when the HNP Capable or SRP Capable bit is SET in the Core USB
         *  Configuration register (GUSBCFG.HNPCap or GUSBCFG.SRPCap, respectively). This bit
         *  can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_gotgint_dbncedone:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} usb_otgfs_gotgint_reg_t;


/** Group: AHB Configuration Register */
/** Type of otgfs_gahbcfg register
 *  AHB Configuration Register This register can be used to configure the core after
 *  power-on or a change in mode. This register mainly contains AHB system-related
 *  configuration parameters. Do not change this register after the initial
 *  programming. The application must program this register before starting any
 *  transactions on either the AHB or the USB.
 */
typedef union {
    struct {
        /** otgfs_gahbcfg_glblintrmsk : R/W; bitpos: [0]; default: 0;
         *  Mode: Host and device
         *  Global Interrupt Mask (GlblIntrMsk)
         *  The application uses this bit to mask or unmask the interrupt line assertion to
         *  itself. Irrespective of this bit's setting, the interrupt status registers are
         *  updated by the controller.
         *  - 1'b0: Mask the interrupt assertion to the application.
         *  - 1'b1: Unmask the interrupt assertion to the application.
         */
        uint32_t otgfs_gahbcfg_glblintrmsk:1;
        /** otgfs_gahbcfg_hbstlen : R/W; bitpos: [4:1]; default: 0;
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
        uint32_t otgfs_gahbcfg_hbstlen:4;
        /** otgfs_gahbcfg_dmaen : R/W; bitpos: [5]; default: 0;
         *  Mode: Host and device
         *  DMA Enable (DMAEn)
         *
         *  This bit is always 0 when Slave-Only mode has been selected.
         *
         *  Reset: 1'b0
         */
        uint32_t otgfs_gahbcfg_dmaen:1;
        uint32_t reserved_6:1;
        /** otgfs_gahbcfg_nptxfemplvl : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_gahbcfg_nptxfemplvl:1;
        /** otgfs_gahbcfg_ptxfemplvl : R/W; bitpos: [8]; default: 0;
         *  Mode: Host only
         *  Periodic TxFIFO Empty Level (PTxFEmpLvl)
         *  Indicates when the Periodic TxFIFO Empty Interrupt bit in the Core Interrupt
         *  register (GINTSTS.PTxFEmp) is triggered. This bit is used only in Slave mode.
         *  - 1'b0: GINTSTS.PTxFEmp interrupt indicates that the Periodic TxFIFO is half empty
         *  - 1'b1: GINTSTS.PTxFEmp interrupt indicates that the Periodic TxFIFO is completely
         *  empty
         */
        uint32_t otgfs_gahbcfg_ptxfemplvl:1;
        uint32_t reserved_9:12;
        /** otgfs_gahbcfg_remmemsupp : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_gahbcfg_remmemsupp:1;
        /** otgfs_gahbcfg_notialldmawrit : R/W; bitpos: [22]; default: 0;
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
        uint32_t otgfs_gahbcfg_notialldmawrit:1;
        /** otgfs_gahbcfg_ahbsingle : R/W; bitpos: [23]; default: 0;
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
        uint32_t otgfs_gahbcfg_ahbsingle:1;
        /** otgfs_gahbcfg_invdescendianess : R/W; bitpos: [24]; default: 0;
         *  Mode: Host and Device
         *  Invert Descriptor Endianness (InvDescEndianess)
         *  - 1'b0: Descriptor Endianness is same as AHB Master Endianness.
         *  - 1'b1:
         *  -- If the AHB Master endianness is Big Endian, the Descriptor Endianness is Little
         *  Endian.
         *  -- If the AHB Master endianness is Little Endian, the Descriptor Endianness is Big
         *  Endian.
         */
        uint32_t otgfs_gahbcfg_invdescendianess:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} usb_otgfs_gahbcfg_reg_t;


/** Group: USB Configuration Register */
/** Type of otgfs_gusbcfg register
 *  USB Configuration Register This register can be used to configure the core after
 *  power-on or a changing to Host mode or Device mode. It contains USB and USB-PHY
 *  related configuration parameters. The application must program this register before
 *  starting any transactions on either the AHB or the USB. Do not make changes to this
 *  register after the initial programming.
 */
typedef union {
    struct {
        /** otgfs_gusbcfg_toutcal : R/W; bitpos: [2:0]; default: 0;
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
        uint32_t otgfs_gusbcfg_toutcal:3;
        /** otgfs_gusbcfg_phyif : R/W; bitpos: [3]; default: 0;
         *  Mode: Host and Device
         *  PHY Interface (PHYIf)
         *  The application uses this bit to configure the core to support a UTMI+ PHY with an
         *  8- or 16-bit interface. When a ULPI PHY is chosen, this must be Set to 8-bit mode.
         *  - 1'b0: 8 bits
         *  - 1'b1: 16 bits
         *  This bit is writable only If UTMI+ and ULPI were selected. Otherwise, this bit
         *  returns the value for the power-on interface selected during configuration.
         */
        uint32_t otgfs_gusbcfg_phyif:1;
        uint32_t reserved_4:1;
        /** otgfs_gusbcfg_fsintf : R/W; bitpos: [5]; default: 0;
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
        uint32_t otgfs_gusbcfg_fsintf:1;
        /** otgfs_gusbcfg_physel : RO; bitpos: [6]; default: 1;
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
        uint32_t otgfs_gusbcfg_physel:1;
        uint32_t reserved_7:1;
        /** otgfs_gusbcfg_srpcap : R/W; bitpos: [8]; default: 0;
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
        uint32_t otgfs_gusbcfg_srpcap:1;
        /** otgfs_gusbcfg_hnpcap : R/W; bitpos: [9]; default: 0;
         *  Mode: Host and Device
         *  HNP-Capable (HNPCap)
         *  The application uses this bit to control the controller's HNP capabilities.
         *  - 1'b0: HNP capability is not enabled.
         *  - 1'b1: HNP capability is enabled.
         *  If HNP functionality is disabled by the software, the OTG signals on the PHY domain
         *  must be tied to the appropriate values.
         */
        uint32_t otgfs_gusbcfg_hnpcap:1;
        /** otgfs_gusbcfg_usbtrdtim : R/W; bitpos: [13:10]; default: 5;
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
        uint32_t otgfs_gusbcfg_usbtrdtim:4;
        uint32_t reserved_14:8;
        /** otgfs_gusbcfg_termseldlpulse : RO; bitpos: [22]; default: 0;
         *  Mode: Device only
         *  TermSel DLine Pulsing Selection (TermSelDLPulse)
         *  This bit selects utmi_termselect to drive data line pulse during SRP.
         *  - 1'b0: Data line pulsing using utmi_txvalid (Default).
         *  - 1'b1: Data line pulsing using utmi_termsel.
         */
        uint32_t otgfs_gusbcfg_termseldlpulse:1;
        uint32_t reserved_23:3;
        /** otgfs_gusbcfg_ic_usbcap : RO; bitpos: [26]; default: 0;
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
        uint32_t otgfs_gusbcfg_ic_usbcap:1;
        uint32_t reserved_27:1;
        /** otgfs_gusbcfg_txenddelay : R/W; bitpos: [28]; default: 0;
         *  Mode: Device only
         *  Tx End Delay (TxEndDelay)
         *  Writing 1'b1 to this bit enables the controller to follow the TxEndDelay timings as
         *  per UTMI+ specification 1.05 section 4.1.5 for opmode signal during remote wakeup.
         *  - 1'b0 : Normal Mode.
         *  - 1'b1 : Tx End delay.
         */
        uint32_t otgfs_gusbcfg_txenddelay:1;
        /** otgfs_gusbcfg_forcehstmode : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_gusbcfg_forcehstmode:1;
        /** otgfs_gusbcfg_forcedevmode : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_gusbcfg_forcedevmode:1;
        /** otgfs_gusbcfg_corrupttxpkt : R/W; bitpos: [31]; default: 0;
         *  Mode: Host and device
         *  Corrupt Tx packet (CorruptTxPkt)
         *  This bit is for debug purposes only. Never Set this bit to 1. The application
         *  should always write 1'b0 to this bit.
         */
        uint32_t otgfs_gusbcfg_corrupttxpkt:1;
    };
    uint32_t val;
} usb_otgfs_gusbcfg_reg_t;


/** Group: Reset Register */
/** Type of otgfs_grstctl register
 *  Reset Register The application uses this register to reset various hardware
 *  features inside the controller.
 */
typedef union {
    struct {
        /** otgfs_grstctl_csftrst : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_grstctl_csftrst:1;
        /** otgfs_grstctl_piufssftrst : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_grstctl_piufssftrst:1;
        /** otgfs_grstctl_frmcntrrst : R/W; bitpos: [2]; default: 0;
         *  Mode: Host only
         *  Host Frame Counter Reset (FrmCntrRst)
         *  The application writes this bit to reset the (micro)Frame number counter inside the
         *  core. When the (micro)Frame counter is reset, the subsequent SOF sent out by the
         *  core has a (micro)Frame number of 0.
         *  When application writes 1 to the bit, it might not be able to read back the value
         *  as it will get cleared by the core in a few clock cycles.
         */
        uint32_t otgfs_grstctl_frmcntrrst:1;
        uint32_t reserved_3:1;
        /** otgfs_grstctl_rxfflsh : R/W; bitpos: [4]; default: 0;
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
        uint32_t otgfs_grstctl_rxfflsh:1;
        /** otgfs_grstctl_txfflsh : R/W; bitpos: [5]; default: 0;
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
        uint32_t otgfs_grstctl_txfflsh:1;
        /** otgfs_grstctl_txfnum : R/W; bitpos: [10:6]; default: 0;
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
        uint32_t otgfs_grstctl_txfnum:5;
        uint32_t reserved_11:19;
        /** otgfs_grstctl_dmareq : RO; bitpos: [30]; default: 0;
         *  Mode: Host and Device
         *  DMA Request Signal (DMAReq)
         *  Indicates that the DMA request is in progress. Used for debug.
         */
        uint32_t otgfs_grstctl_dmareq:1;
        /** otgfs_grstctl_ahbidle : RO; bitpos: [31]; default: 1;
         *  Mode: Host and Device
         *  AHB Master Idle (AHBIdle)
         *  Indicates that the AHB Master State Machine is in the IDLE condition.
         */
        uint32_t otgfs_grstctl_ahbidle:1;
    };
    uint32_t val;
} usb_otgfs_grstctl_reg_t;


/** Group: Interrupt Register */
/** Type of otgfs_gintsts register
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
typedef union {
    struct {
        /** otgfs_gintsts_curmod : RO; bitpos: [0]; default: 0;
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
        uint32_t otgfs_gintsts_curmod:1;
        /** otgfs_gintsts_modemis : R/W; bitpos: [1]; default: 0;
         *  Mode: Host and Device
         *  Mode Mismatch Interrupt (ModeMis)
         *  The core sets this bit when the application is trying to access:
         *  - A Host mode register, when the controller is operating in Device mode
         *  - A Device mode register, when the controller is operating in Host mode
         *  The register access is completed on the AHB with an OKAY response, but is ignored
         *  by the controller internally and does not affect the operation of the controller.
         *  This bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_gintsts_modemis:1;
        /** otgfs_gintsts_otgint : RO; bitpos: [2]; default: 0;
         *  Mode: Host and Device
         *  OTG Interrupt (OTGInt)
         *  The controller sets this bit to indicate an OTG protocol event. The application
         *  must read the OTG Interrupt Status (GOTGINT) register to determine the exact event
         *  that caused this interrupt. The application must clear the appropriate status bit
         *  in the GOTGINT register to clear this bit.
         */
        uint32_t otgfs_gintsts_otgint:1;
        /** otgfs_gintsts_sof : R/W; bitpos: [3]; default: 0;
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
        uint32_t otgfs_gintsts_sof:1;
        /** otgfs_gintsts_rxflvl : RO; bitpos: [4]; default: 0;
         *  Mode: Host and Device
         *  RxFIFO Non-Empty (RxFLvl)
         *
         *  Indicates that there is at least one packet pending to be read from the RxFIFO.
         */
        uint32_t otgfs_gintsts_rxflvl:1;
        /** otgfs_gintsts_nptxfemp : RO; bitpos: [5]; default: 1;
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
        uint32_t otgfs_gintsts_nptxfemp:1;
        /** otgfs_gintsts_ginnakeff : RO; bitpos: [6]; default: 0;
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
        uint32_t otgfs_gintsts_ginnakeff:1;
        /** otgfs_gintsts_goutnakeff : RO; bitpos: [7]; default: 0;
         *  Mode: Device only
         *  Global OUT NAK Effective (GOUTNakEff)
         *  Indicates that the Set Global OUT NAK bit in the Device Control register
         *  (DCTL.SGOUTNak), Set by the application, has taken effect in the core. This bit can
         *  be cleared by writing the Clear Global OUT NAK bit in the Device Control register
         *  (DCTL.CGOUTNak).
         */
        uint32_t otgfs_gintsts_goutnakeff:1;
        uint32_t reserved_8:2;
        /** otgfs_gintsts_erlysusp : R/W; bitpos: [10]; default: 0;
         *  Mode: Device only
         *  Early Suspend (ErlySusp)
         *  The controller sets this bit to indicate that an Idle state has been detected on
         *  the USB for 3 ms.
         */
        uint32_t otgfs_gintsts_erlysusp:1;
        /** otgfs_gintsts_usbsusp : R/W; bitpos: [11]; default: 0;
         *  Mode: Device only
         *  USB Suspend (USBSusp)
         *  The controller sets this bit to indicate that a suspend was detected on the USB.
         *  The controller enters the Suspended state when there is no activity on the
         *  linestate signal for an extended period of time.
         */
        uint32_t otgfs_gintsts_usbsusp:1;
        /** otgfs_gintsts_usbrst : R/W; bitpos: [12]; default: 0;
         *  Mode: Device only
         *  USB Reset (USBRst)
         *  The controller sets this bit to indicate that a reset is detected on the USB.
         */
        uint32_t otgfs_gintsts_usbrst:1;
        /** otgfs_gintsts_enumdone : R/W; bitpos: [13]; default: 0;
         *  Mode: Device only
         *  Enumeration Done (EnumDone)
         *  The core sets this bit to indicate that speed enumeration is complete. The
         *  application must read the Device Status (DSTS) register to obtain the enumerated
         *  speed.
         */
        uint32_t otgfs_gintsts_enumdone:1;
        /** otgfs_gintsts_isooutdrop : R/W; bitpos: [14]; default: 0;
         *  Mode: Device only
         *  Isochronous OUT Packet Dropped Interrupt (ISOOutDrop)
         *  The controller sets this bit when it fails to write an isochronous OUT packet into
         *  the RxFIFO because the RxFIFO does not have enough space to accommodate a maximum
         *  packet size packet for the isochronous OUT endpoint.
         */
        uint32_t otgfs_gintsts_isooutdrop:1;
        /** otgfs_gintsts_eopf : R/W; bitpos: [15]; default: 0;
         *  Mode: Device only
         *  End of Periodic Frame Interrupt (EOPF)
         *  Indicates that the period specified in the Periodic Frame Interval field of the
         *  Device Configuration register (DCFG.PerFrInt) has been reached in the current
         *  microframe.
         */
        uint32_t otgfs_gintsts_eopf:1;
        uint32_t reserved_16:1;
        /** otgfs_gintsts_epmis : R/W; bitpos: [17]; default: 0;
         *  Mode: Device only
         *  Endpoint Mismatch Interrupt (EPMis)
         *  Note: This interrupt is valid only in shared FIFO operation.
         *  Indicates that an IN token has been received for a non-periodic endpoint, but the
         *  data for another endpoint is present in the top of the Non-periodic Transmit FIFO
         *  and the IN endpoint mismatch count programmed by the application has expired.
         */
        uint32_t otgfs_gintsts_epmis:1;
        /** otgfs_gintsts_iepint : RO; bitpos: [18]; default: 0;
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
        uint32_t otgfs_gintsts_iepint:1;
        /** otgfs_gintsts_oepint : RO; bitpos: [19]; default: 0;
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
        uint32_t otgfs_gintsts_oepint:1;
        /** otgfs_gintsts_incompisoin : R/W; bitpos: [20]; default: 0;
         *  Mode: Device only
         *  Incomplete Isochronous IN Transfer (incompISOIN)
         *  The core sets this interrupt to indicate that there is at least one isochronous IN
         *  endpoint on which the transfer is not completed in the current microframe. This
         *  interrupt is asserted along with the End of Periodic Frame Interrupt (EOPF) bit in
         *  this register.
         *  Note: This interrupt is not asserted in Scatter/Gather DMA mode.
         */
        uint32_t otgfs_gintsts_incompisoin:1;
        /** otgfs_gintsts_incomplp : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_gintsts_incomplp:1;
        /** otgfs_gintsts_fetsusp : R/W; bitpos: [22]; default: 0;
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
        uint32_t otgfs_gintsts_fetsusp:1;
        /** otgfs_gintsts_resetdet : R/W; bitpos: [23]; default: 0;
         *  Mode: Device only
         *  Reset detected Interrupt (ResetDet)
         *  In Device mode, this interrupt is asserted when a reset is detected on the USB in
         *  partial power-down mode when the device is in Suspend.
         *
         *  In Host mode, this interrupt is not asserted.
         */
        uint32_t otgfs_gintsts_resetdet:1;
        /** otgfs_gintsts_prtint : RO; bitpos: [24]; default: 0;
         *  Mode: Host only
         *  Host Port Interrupt (PrtInt)
         *  The core sets this bit to indicate a change in port status of one of the controller
         *  ports in Host mode. The application must read the Host Port Control and Status
         *  (HPRT) register to determine the exact event that caused this interrupt. The
         *  application must clear the appropriate status bit in the Host Port
         *  Control and Status register to clear this bit.
         */
        uint32_t otgfs_gintsts_prtint:1;
        /** otgfs_gintsts_hchint : RO; bitpos: [25]; default: 0;
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
        uint32_t otgfs_gintsts_hchint:1;
        /** otgfs_gintsts_ptxfemp : RO; bitpos: [26]; default: 1;
         *  Mode: Host only
         *  Periodic TxFIFO Empty (PTxFEmp)
         *  This interrupt is asserted when the Periodic Transmit FIFO is either half or
         *  completely empty and there is space for at least one entry to be written in the
         *  Periodic Request Queue. The half or completely empty status is determined by the
         *  Periodic TxFIFO Empty Level bit in the Core AHB Configuration register
         *  (GAHBCFG.PTxFEmpLvl).
         */
        uint32_t otgfs_gintsts_ptxfemp:1;
        uint32_t reserved_27:1;
        /** otgfs_gintsts_conidstschng : R/W; bitpos: [28]; default: 0;
         *  Mode: Host and Device
         *  Connector ID Status Change (ConIDStsChng)
         *  The core sets this bit when there is a change in connector ID status.
         */
        uint32_t otgfs_gintsts_conidstschng:1;
        /** otgfs_gintsts_disconnint : R/W; bitpos: [29]; default: 0;
         *  Mode: Host only
         *  Disconnect Detected Interrupt (DisconnInt)
         *  Asserted when a device disconnect is detected.
         */
        uint32_t otgfs_gintsts_disconnint:1;
        /** otgfs_gintsts_sessreqint : R/W; bitpos: [30]; default: 0;
         *  Mode: Host and Device
         *  Session Request/New Session Detected Interrupt (SessReqInt)
         *  In Host mode, this interrupt is asserted when a session request is detected from
         *  the device. In Host mode, this interrupt is asserted when a session request is
         *  detected from the device.
         *  In Device mode, this interrupt is asserted when the utmisrp_bvalid signal goes high.
         *  For more information on how to use this interrupt, see 'Partial Power-Down and
         *  Clock Gating Programming Model' in the Programming Guide.
         */
        uint32_t otgfs_gintsts_sessreqint:1;
        /** otgfs_gintsts_wkupint : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_gintsts_wkupint:1;
    };
    uint32_t val;
} usb_otgfs_gintsts_reg_t;


/** Group: Interrupt Mask Register */
/** Type of otgfs_gintmsk register
 *  Interrupt Mask Register This register works with the Interrupt Register (GINTSTS)
 *  to interrupt the application. When an interrupt bit is masked, the interrupt
 *  associated with that bit is not generated. However, the GINTSTS register bit
 *  corresponding to that interrupt is still set.  Note: The fields of this register
 *  change depending on host or device mode.
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** otgfs_gintmsk_modemismsk : R/W; bitpos: [1]; default: 0;
         *  Mode: Host and Device
         *  Mode Mismatch Interrupt Mask (ModeMisMsk)
         */
        uint32_t otgfs_gintmsk_modemismsk:1;
        /** otgfs_gintmsk_otgintmsk : R/W; bitpos: [2]; default: 0;
         *  Mode: Host and Device
         *  OTG Interrupt Mask (OTGIntMsk)
         */
        uint32_t otgfs_gintmsk_otgintmsk:1;
        /** otgfs_gintmsk_sofmsk : R/W; bitpos: [3]; default: 0;
         *  Mode: Host and Device
         *  Start of (micro)Frame Mask (SofMsk)
         */
        uint32_t otgfs_gintmsk_sofmsk:1;
        /** otgfs_gintmsk_rxflvlmsk : R/W; bitpos: [4]; default: 0;
         *  Mode: Host and Device
         *  Receive FIFO Non-Empty Mask (RxFLvlMsk)
         */
        uint32_t otgfs_gintmsk_rxflvlmsk:1;
        /** otgfs_gintmsk_nptxfempmsk : R/W; bitpos: [5]; default: 0;
         *  Mode: Host and Device
         *  Non-periodic TxFIFO Empty Mask (NPTxFEmpMsk)
         */
        uint32_t otgfs_gintmsk_nptxfempmsk:1;
        /** otgfs_gintmsk_ginnakeffmsk : R/W; bitpos: [6]; default: 0;
         *  Mode: Device only,
         *  Global Non-periodic IN NAK Effective Mask (GINNakEffMsk)
         */
        uint32_t otgfs_gintmsk_ginnakeffmsk:1;
        /** otgfs_gintmsk_goutnakeffmsk : R/W; bitpos: [7]; default: 0;
         *  Mode: Device only
         *  Global OUT NAK Effective Mask (GOUTNakEffMsk)
         */
        uint32_t otgfs_gintmsk_goutnakeffmsk:1;
        uint32_t reserved_8:2;
        /** otgfs_gintmsk_erlysuspmsk : R/W; bitpos: [10]; default: 0;
         *  Mode: Device only
         *  Early Suspend Mask (ErlySuspMsk)
         */
        uint32_t otgfs_gintmsk_erlysuspmsk:1;
        /** otgfs_gintmsk_usbsuspmsk : R/W; bitpos: [11]; default: 0;
         *  Mode: Device only
         *  USB Suspend Mask (USBSuspMsk)
         */
        uint32_t otgfs_gintmsk_usbsuspmsk:1;
        /** otgfs_gintmsk_usbrstmsk : R/W; bitpos: [12]; default: 0;
         *  Mode: Device only
         *  USB Reset Mask (USBRstMsk)
         */
        uint32_t otgfs_gintmsk_usbrstmsk:1;
        /** otgfs_gintmsk_enumdonemsk : R/W; bitpos: [13]; default: 0;
         *  Mode: Device only
         *  Enumeration Done Mask (EnumDoneMsk)
         */
        uint32_t otgfs_gintmsk_enumdonemsk:1;
        /** otgfs_gintmsk_isooutdropmsk : R/W; bitpos: [14]; default: 0;
         *  Mode: Device only
         *  Isochronous OUT Packet Dropped Interrupt Mask (ISOOutDropMsk)
         */
        uint32_t otgfs_gintmsk_isooutdropmsk:1;
        /** otgfs_gintmsk_eopfmsk : R/W; bitpos: [15]; default: 0;
         *  Mode: Device only
         *  End of Periodic Frame Interrupt Mask (EOPFMsk)
         */
        uint32_t otgfs_gintmsk_eopfmsk:1;
        uint32_t reserved_16:1;
        /** otgfs_gintmsk_epmismsk : R/W; bitpos: [17]; default: 0;
         *  Mode: Device only
         *  Endpoint Mismatch Interrupt Mask (EPMisMsk)
         */
        uint32_t otgfs_gintmsk_epmismsk:1;
        /** otgfs_gintmsk_iepintmsk : R/W; bitpos: [18]; default: 0;
         *  Mode: Device only
         *  IN Endpoints Interrupt Mask (IEPIntMsk)
         */
        uint32_t otgfs_gintmsk_iepintmsk:1;
        /** otgfs_gintmsk_oepintmsk : R/W; bitpos: [19]; default: 0;
         *  Mode: Device only
         *  OUT Endpoints Interrupt Mask (OEPIntMsk)
         */
        uint32_t otgfs_gintmsk_oepintmsk:1;
        uint32_t reserved_20:1;
        /** otgfs_gintmsk_incomplpmsk : R/W; bitpos: [21]; default: 0;
         *  Incomplete Periodic Transfer Mask (incomplPMsk)
         *  Mode: Host only
         *  Incomplete Isochronous OUT Transfer Interrupt Mask (incompISOOUTMsk)
         *  Mode: Device only
         */
        uint32_t otgfs_gintmsk_incomplpmsk:1;
        /** otgfs_gintmsk_fetsuspmsk : R/W; bitpos: [22]; default: 0;
         *  Mode: Device only
         *  Data Fetch Suspended Mask (FetSuspMsk)
         */
        uint32_t otgfs_gintmsk_fetsuspmsk:1;
        /** otgfs_gintmsk_resetdetmsk : R/W; bitpos: [23]; default: 0;
         *  Mode: Device only
         *  Reset detected Interrupt Mask (ResetDetMsk)
         */
        uint32_t otgfs_gintmsk_resetdetmsk:1;
        /** otgfs_gintmsk_prtintmsk : R/W; bitpos: [24]; default: 0;
         *  Mode: Host only
         *  Host Port Interrupt Mask (PrtIntMsk)
         */
        uint32_t otgfs_gintmsk_prtintmsk:1;
        /** otgfs_gintmsk_hchintmsk : R/W; bitpos: [25]; default: 0;
         *  Mode: Host only
         *  Host Channels Interrupt Mask (HChIntMsk)
         */
        uint32_t otgfs_gintmsk_hchintmsk:1;
        /** otgfs_gintmsk_ptxfempmsk : R/W; bitpos: [26]; default: 0;
         *  Mode: Host only
         *  Periodic TxFIFO Empty Mask (PTxFEmpMsk)
         */
        uint32_t otgfs_gintmsk_ptxfempmsk:1;
        uint32_t reserved_27:1;
        /** otgfs_gintmsk_conidstschngmsk : R/W; bitpos: [28]; default: 0;
         *  Mode: Host and Device
         *  Connector ID Status Change Mask (ConIDStsChngMsk)
         */
        uint32_t otgfs_gintmsk_conidstschngmsk:1;
        /** otgfs_gintmsk_disconnintmsk : R/W; bitpos: [29]; default: 0;
         *  Mode: Host and Device
         *  Disconnect Detected Interrupt Mask (DisconnIntMsk)
         */
        uint32_t otgfs_gintmsk_disconnintmsk:1;
        /** otgfs_gintmsk_sessreqintmsk : R/W; bitpos: [30]; default: 0;
         *  Mode: Host and Device
         *  Session Request/New Session Detected Interrupt Mask (SessReqIntMsk)
         */
        uint32_t otgfs_gintmsk_sessreqintmsk:1;
        /** otgfs_gintmsk_wkupintmsk : R/W; bitpos: [31]; default: 0;
         *  Mode: Host and Device
         *  Resume/Remote Wakeup Detected Interrupt Mask (WkUpIntMsk)
         *  The WakeUp bit is used for LPM state wake up in a way similar to that of wake up in
         *  suspend state.
         */
        uint32_t otgfs_gintmsk_wkupintmsk:1;
    };
    uint32_t val;
} usb_otgfs_gintmsk_reg_t;


/** Group: Receive Status Debug Read Register */
/** Type of otgfs_grxstsr register
 *  Receive Status Debug Read Register A read to the Receive Status Debug Read register
 *  returns the contents of the top of the Receive FIFO.  The receive status contents
 *  must be interpreted differently in Host and Device modes. The core ignores the
 *  receive status read when the receive FIFO is empty and returns a value of
 *  32'h0000_0000.  Note:  - Use of these fields vary based on whether the core is
 *  functioning as a host or a device.  - Do not read this register's reset value
 *  before configuring the core because the read value is 'X' in the simulation.
 */
typedef union {
    struct {
        /** otgfs_grxstsr_chnum : RO; bitpos: [3:0]; default: 0;
         *  Channel Number (ChNum)
         *  Mode: Host only
         *  Indicates the channel number to which the current received packet belongs.
         *  Endpoint Number (EPNum)
         *  Mode: Device only
         *  Indicates the endpoint number to which the current received packet belongs.
         */
        uint32_t otgfs_grxstsr_chnum:4;
        /** otgfs_grxstsr_bcnt : RO; bitpos: [14:4]; default: 0;
         *  Byte Count (BCnt)
         *
         *  In host mode, indicates the byte count of the received IN data packet.
         *
         *  In device mode, indicates the byte count of the received data packet.
         */
        uint32_t otgfs_grxstsr_bcnt:11;
        /** otgfs_grxstsr_dpid : RO; bitpos: [16:15]; default: 0;
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
        uint32_t otgfs_grxstsr_dpid:2;
        /** otgfs_grxstsr_pktsts : RO; bitpos: [20:17]; default: 0;
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
        uint32_t otgfs_grxstsr_pktsts:4;
        /** otgfs_grxstsr_fn : RO; bitpos: [24:21]; default: 0;
         *  Mode: Device only
         *  Frame Number (FN)
         *  This is the least significant 4 bits of the (micro)Frame number in which the packet
         *  is received on the USB. This field is supported only when isochronous OUT endpoints
         *  are supported.
         */
        uint32_t otgfs_grxstsr_fn:4;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} usb_otgfs_grxstsr_reg_t;


/** Group: Receive Status Read/Pop Register */
/** Type of otgfs_grxstsp register
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
typedef union {
    struct {
        /** otgfs_grxstsp_chnum : RO; bitpos: [3:0]; default: 0;
         *  Channel Number (ChNum)
         *  Mode: Host only
         *  Indicates the channel number to which the current received packet belongs.
         *  Endpoint Number (EPNum)
         *  Mode: Device only
         *  Indicates the endpoint number to which the current received packet belongs.
         */
        uint32_t otgfs_grxstsp_chnum:4;
        /** otgfs_grxstsp_bcnt : RO; bitpos: [14:4]; default: 0;
         *  Byte Count (BCnt)
         *
         *  In host mode, indicates the byte count of the received IN data packet.
         *
         *  In device mode, indicates the byte count of the received data packet.
         */
        uint32_t otgfs_grxstsp_bcnt:11;
        /** otgfs_grxstsp_dpid : RO; bitpos: [16:15]; default: 0;
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
        uint32_t otgfs_grxstsp_dpid:2;
        /** otgfs_grxstsp_pktsts : RO; bitpos: [20:17]; default: 0;
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
        uint32_t otgfs_grxstsp_pktsts:4;
        /** otgfs_grxstsp_fn : RO; bitpos: [24:21]; default: 0;
         *  Mode: Device only
         *  Frame Number (FN)
         *  This is the least significant 4 bits of the (micro)Frame number in which the packet
         *  is received on the USB. This field is supported only when isochronous OUT endpoints
         *  are supported.
         */
        uint32_t otgfs_grxstsp_fn:4;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} usb_otgfs_grxstsp_reg_t;


/** Group: Receive FIFO Size Register */
/** Type of otgfs_grxfsiz register
 *  Receive FIFO Size Register The application can program the RAM size that must be
 *  allocated to the RxFIFO.
 */
typedef union {
    struct {
        /** otgfs_grxfsiz_rxfdep : R/W; bitpos: [8:0]; default: 256;
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
        uint32_t otgfs_grxfsiz_rxfdep:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} usb_otgfs_grxfsiz_reg_t;


/** Group: Non-periodic Transmit FIFO Size Register */
/** Type of otgfs_gnptxfsiz register
 *  Non-periodic Transmit FIFO Size Register The application can program the RAM size
 *  and the memory start address for the Non-periodic TxFIFO  Note: The fields of this
 *  register change depending on host or device mode.
 */
typedef union {
    struct {
        /** otgfs_gnptxfsiz_nptxfstaddr : R/W; bitpos: [8:0]; default: 256;
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
        uint32_t otgfs_gnptxfsiz_nptxfstaddr:9;
        uint32_t reserved_9:7;
        /** otgfs_gnptxfsiz_nptxfdep : R/W; bitpos: [24:16]; default: 256;
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
        uint32_t otgfs_gnptxfsiz_nptxfdep:9;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} usb_otgfs_gnptxfsiz_reg_t;


/** Group: Non-periodic Transmit FIFO/Queue Status Register */
/** Type of otgfs_gnptxsts register
 *  Non-periodic Transmit FIFO/Queue Status Register In Device mode, this register is
 *  valid only in Shared FIFO operation. This read-only register contains the free
 *  space information for the Non-periodic TxFIFO and the Non-periodic Transmit Request
 *  Queue.
 */
typedef union {
    struct {
        /** otgfs_gnptxsts_nptxfspcavail : RO; bitpos: [15:0]; default: 256;
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
        uint32_t otgfs_gnptxsts_nptxfspcavail:16;
        /** otgfs_gnptxsts_nptxqspcavail : RO; bitpos: [23:16]; default: 4;
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
        uint32_t otgfs_gnptxsts_nptxqspcavail:8;
        /** otgfs_gnptxsts_nptxqtop : RO; bitpos: [30:24]; default: 0;
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
        uint32_t otgfs_gnptxsts_nptxqtop:7;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_gnptxsts_reg_t;


/** Group: Synopsis ID Register */
/** Type of otgfs_gsnpsid register
 *  Synopsis ID Register  This read-only register contains the release number of the
 *  core being used.
 */
typedef union {
    struct {
        /** otgfs_gsnpsid_synopsysid : RO; bitpos: [31:0]; default: 1330921482;
         *  Release number of the controller being used currently.
         */
        uint32_t otgfs_gsnpsid_synopsysid:32;
    };
    uint32_t val;
} usb_otgfs_gsnpsid_reg_t;


/** Group: User Hardware Configuration 1 Register */
/** Type of otgfs_ghwcfg1 register
 *  User Hardware Configuration 1 Register  This register contains the logical endpoint
 *  direction(s) selected using coreConsultant.
 */
typedef union {
    struct {
        /** otgfs_ghwcfg1_epdir : RO; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_ghwcfg1_epdir:32;
    };
    uint32_t val;
} usb_otgfs_ghwcfg1_reg_t;


/** Group: User Hardware Configuration 2 Register */
/** Type of otgfs_ghwcfg2 register
 *  User Hardware Configuration 2 Register  This register contains configuration
 *  options selected using coreConsultant.
 */
typedef union {
    struct {
        /** otgfs_ghwcfg2_otgmode : RO; bitpos: [2:0]; default: 0;
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
        uint32_t otgfs_ghwcfg2_otgmode:3;
        /** otgfs_ghwcfg2_otgarch : RO; bitpos: [4:3]; default: 2;
         *  Architecture (OtgArch)
         *  - 2'b00: Slave-Only
         *  - 2'b01: External DMA
         *  - 2'b10: Internal DMA
         *  - Others: Reserved
         *  Note: This field is configured using the OTG_ARCHITECTURE parameter.
         */
        uint32_t otgfs_ghwcfg2_otgarch:2;
        /** otgfs_ghwcfg2_singpnt : RO; bitpos: [5]; default: 1;
         *  Point-to-Point (SingPnt)
         *  - 1'b0: Multi-point application  (hub and split support)
         *  - 1'b1: Single-point application (no hub and split support)
         *  Note: This field is configured using the OTG_SINGLE_POINT parameter.
         */
        uint32_t otgfs_ghwcfg2_singpnt:1;
        /** otgfs_ghwcfg2_hsphytype : RO; bitpos: [7:6]; default: 0;
         *  High-Speed PHY Interface Type (HSPhyType)
         *  - 2'b00: High-Speed interface not supported
         *  - 2'b01: UTMI+
         *  - 2'b10: ULPI
         *  - 2'b11: UTMI+ and ULPI
         *  Note: This field is configured using the OTG_HSPHY_INTERFACE parameter.
         */
        uint32_t otgfs_ghwcfg2_hsphytype:2;
        /** otgfs_ghwcfg2_fsphytype : RO; bitpos: [9:8]; default: 1;
         *  Full-Speed PHY Interface Type (FSPhyType)
         *  - 2'b00: Full-speed interface not supported
         *  - 2'b01: Dedicated full-speed interface
         *  - 2'b10: FS pins shared with UTMI+ pins
         *  - 2'b11: FS pins shared with ULPI pins
         *  Note: This field is configured using the OTG_FSPHY_INTERFACE parameter.
         */
        uint32_t otgfs_ghwcfg2_fsphytype:2;
        /** otgfs_ghwcfg2_numdeveps : RO; bitpos: [13:10]; default: 6;
         *  Number of Device Endpoints (NumDevEps)
         *
         *  Indicates the number of device endpoints supported by the core in Device mode.
         *
         *  The range of this field is 0-15.
         *
         *  Note: This field is configured using the OTG_NUM_EPS parameter.
         */
        uint32_t otgfs_ghwcfg2_numdeveps:4;
        /** otgfs_ghwcfg2_numhstchnl : RO; bitpos: [17:14]; default: 7;
         *  Number of Host Channels (NumHstChnl)
         *  Indicates the number of host channels supported by the core in Host mode. The range
         *  of this field is 0-15: 0 specifies 1 channel, 15 specifies 16 channels.
         *
         *  Note: This field is configured using the OTG_NUM_HOST_CHAN parameter.
         */
        uint32_t otgfs_ghwcfg2_numhstchnl:4;
        /** otgfs_ghwcfg2_periosupport : RO; bitpos: [18]; default: 1;
         *  Periodic OUT Channels Supported in Host Mode (PerioSupport)
         *  - 1'b0: No
         *  - 1'b1: Yes
         *  Note: This field is configured using the OTG_EN_PERIO_HOST parameter.
         */
        uint32_t otgfs_ghwcfg2_periosupport:1;
        /** otgfs_ghwcfg2_dynfifosizing : RO; bitpos: [19]; default: 1;
         *  Dynamic FIFO Sizing Enabled (DynFifoSizing)
         *  - 1'b0: No
         *  - 1'b1: Yes
         *  Note: This field is configured using the OTG_DFIFO_DYNAMIC parameter.
         */
        uint32_t otgfs_ghwcfg2_dynfifosizing:1;
        /** otgfs_ghwcfg2_multiprocintrpt : RO; bitpos: [20]; default: 0;
         *  Multi Processor Interrupt Enabled (MultiProcIntrpt)
         *  - 1'b0: No
         *  - 1'b1: Yes
         *  Note: This field is configured using the OTG_MULTI_PROC_INTRPT parameter.
         */
        uint32_t otgfs_ghwcfg2_multiprocintrpt:1;
        uint32_t reserved_21:1;
        /** otgfs_ghwcfg2_nptxqdepth : RO; bitpos: [23:22]; default: 1;
         *  Non-periodic Request Queue Depth (NPTxQDepth)
         *  - 2'b00: 2
         *  - 2'b01: 4
         *  - 2'b10: 8
         *  - Others: Reserved
         *  Note: This field is configured using the OTG_NPERIO_TX_QUEUE_DEPTH parameter.
         */
        uint32_t otgfs_ghwcfg2_nptxqdepth:2;
        /** otgfs_ghwcfg2_ptxqdepth : RO; bitpos: [25:24]; default: 2;
         *  Host Mode Periodic Request Queue Depth (PTxQDepth)
         *  - 2'b00: 2
         *  - 2'b01: 4
         *  - 2'b10: 8
         *  - 2'b11:16
         *  Note: This field is configured using the OTG_PERIO_TX_QUEUE_DEPTH parameter.
         */
        uint32_t otgfs_ghwcfg2_ptxqdepth:2;
        /** otgfs_ghwcfg2_tknqdepth : RO; bitpos: [30:26]; default: 8;
         *  Device Mode IN Token Sequence Learning Queue Depth (TknQDepth)
         *  Range: 0-30
         *
         *  Note: This field is configured using the OTG_TOKEN_QUEUE_DEPTH parameter.
         */
        uint32_t otgfs_ghwcfg2_tknqdepth:5;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_ghwcfg2_reg_t;


/** Group: User Hardware Configuration 3 Register */
/** Type of otgfs_ghwcfg3 register
 *  User Hardware Configuration 3 Register
 */
typedef union {
    struct {
        /** otgfs_ghwcfg3_xfersizewidth : RO; bitpos: [3:0]; default: 5;
         *  Width of Transfer Size Counters (XferSizeWidth)
         *  - 4'b0000: 11 bits
         *  - 4'b0001: 12 bits
         *  ...
         *  - 4'b1000: 19 bits
         *  - Others: Reserved
         *  Note: This field is configured using the OTG_PACKET_COUNT_WIDTH parameter.
         */
        uint32_t otgfs_ghwcfg3_xfersizewidth:4;
        /** otgfs_ghwcfg3_pktsizewidth : RO; bitpos: [6:4]; default: 3;
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
        uint32_t otgfs_ghwcfg3_pktsizewidth:3;
        /** otgfs_ghwcfg3_otgen : RO; bitpos: [7]; default: 1;
         *  OTG Function Enabled (OtgEn)
         *
         *  The application uses this bit to indicate the OTG capabilities of the controller .
         *  - 1'b0: Not OTG capable
         *  - 1'b1: OTG Capable
         *  Note: This field is configured using the OTG_MODE parameter.
         */
        uint32_t otgfs_ghwcfg3_otgen:1;
        /** otgfs_ghwcfg3_i2cintsel : RO; bitpos: [8]; default: 0;
         *  I2C Selection (I2CIntSel)
         *  - 1'b0: I2C Interface is not available on the controller.
         *  - 1'b1: I2C Interface is available on the controller.
         *  Note: This field is configured using the OTG_I2C_INTERFACE parameter.
         */
        uint32_t otgfs_ghwcfg3_i2cintsel:1;
        /** otgfs_ghwcfg3_vndctlsupt : RO; bitpos: [9]; default: 0;
         *  Vendor Control Interface Support (VndctlSupt)
         *
         *  - 1'b0: Vendor Control Interface is not available on the core.
         *  - 1'b1: Vendor Control Interface is available.
         *  Note: This field is configured using the OTG_VENDOR_CTL_INTERFACE parameter.
         */
        uint32_t otgfs_ghwcfg3_vndctlsupt:1;
        /** otgfs_ghwcfg3_optfeature : RO; bitpos: [10]; default: 1;
         *  Optional Features Removed (OptFeature)
         *  Indicates whether the User ID register, GPIO interface ports, and SOF toggle and
         *  counter ports were removed for gate count optimization by enabling Remove Optional
         *  Features.
         *  - 1'b0: No
         *  - 1'b1: Yes
         *  Note: This field is configured using the OTG_RM_OPT_FEATURES parameter.
         */
        uint32_t otgfs_ghwcfg3_optfeature:1;
        /** otgfs_ghwcfg3_rsttype : RO; bitpos: [11]; default: 0;
         *  Reset Style for Clocked always Blocks in RTL (RstType)
         *  - 1'b0: Asynchronous reset is used in the controller
         *  - 1'b1: Synchronous reset is used in the controller
         *  Note: This field is configured using the OTG_SYNC_RESET_TYPE parameter.
         */
        uint32_t otgfs_ghwcfg3_rsttype:1;
        /** otgfs_ghwcfg3_adpsupport : RO; bitpos: [12]; default: 0;
         *  This bit indicates whether ADP logic is present within or external to the controller
         *  - 0: No ADP logic present with the controller
         *  - 1: ADP logic is present along with the controller.
         */
        uint32_t otgfs_ghwcfg3_adpsupport:1;
        /** otgfs_ghwcfg3_hsicmode : RO; bitpos: [13]; default: 0;
         *  HSIC mode specified for Mode of Operation
         *  Value Range: 0 - 1
         *  - 1: HSIC-capable with shared UTMI PHY interface
         *  - 0: Non-HSIC-capable
         */
        uint32_t otgfs_ghwcfg3_hsicmode:1;
        /** otgfs_ghwcfg3_bcsupport : RO; bitpos: [14]; default: 0;
         *  This bit indicates the controller support for Battery Charger.
         *  - 0 - No Battery Charger Support
         *  - 1 - Battery Charger support present
         */
        uint32_t otgfs_ghwcfg3_bcsupport:1;
        /** otgfs_ghwcfg3_lpmmode : RO; bitpos: [15]; default: 0;
         *  LPM mode specified for Mode of Operation.
         */
        uint32_t otgfs_ghwcfg3_lpmmode:1;
        /** otgfs_ghwcfg3_dfifodepth : RO; bitpos: [31:16]; default: 200;
         *  DFIFO Depth (DfifoDepth - EP_LOC_CNT)
         *
         *  This value is in terms of 32-bit words.
         *  - Minimum value is 32
         *  - Maximum value is 32,768
         *  Note: This field is configured using the OTG_DFIFO_DEPTH parameter. For more
         *  information on EP_LOC_CNT, see the Endpoint Information Controller (EPINFO_CTL)
         *  section.
         */
        uint32_t otgfs_ghwcfg3_dfifodepth:16;
    };
    uint32_t val;
} usb_otgfs_ghwcfg3_reg_t;


/** Group: User Hardware Configuration 4 Register */
/** Type of otgfs_ghwcfg4 register
 *  User Hardware Configuration 4 Register  Note Bit [31] is available only when
 *  Scatter/Gather DMA mode is enabled. When Scatter/Gather DMA mode is disabled, this
 *  field is reserved.
 */
typedef union {
    struct {
        /** otgfs_ghwcfg4_numdevperioeps : RO; bitpos: [3:0]; default: 0;
         *  Number of Device Mode Periodic IN Endpoints (NumDevPerioEps)
         *
         *  Range: 0-15
         */
        uint32_t otgfs_ghwcfg4_numdevperioeps:4;
        /** otgfs_ghwcfg4_partialpwrdn : RO; bitpos: [4]; default: 1;
         *  Enable Partial Power Down (PartialPwrDn)
         *  - 1'b0: Partial Power Down Not Enabled
         *  - 1'b1: Partial Power Down Enabled
         */
        uint32_t otgfs_ghwcfg4_partialpwrdn:1;
        /** otgfs_ghwcfg4_ahbfreq : RO; bitpos: [5]; default: 1;
         *  Minimum AHB Frequency Less Than 60 MHz (AhbFreq)
         *  - 1'b0: No
         *  - 1'b1: Yes
         */
        uint32_t otgfs_ghwcfg4_ahbfreq:1;
        /** otgfs_ghwcfg4_hibernation : RO; bitpos: [6]; default: 0;
         *  Enable Hibernation (Hibernation)
         *  - 1'b0: Hibernation feature not enabled
         *  - 1'b1: Hibernation feature enabled
         */
        uint32_t otgfs_ghwcfg4_hibernation:1;
        /** otgfs_ghwcfg4_extendedhibernation : RO; bitpos: [7]; default: 0;
         *  Enable Hibernation
         *  - 1'b0: Extended Hibernation feature not enabled
         *  - 1'b1: Extended Hibernation feature enabled
         */
        uint32_t otgfs_ghwcfg4_extendedhibernation:1;
        uint32_t reserved_8:4;
        /** otgfs_ghwcfg4_acgsupt : RO; bitpos: [12]; default: 0;
         *  Active Clock Gating Support
         *
         *  This bit indicates that the controller supports the Dynamic (Switching) Power
         *  Reduction during periods
         *  when there is no USB and AHB Traffic.
         *  - 1'b0:  Active Clock Gating is not enabled.
         *  - 1'b1:  Active Clock Gating Enabled.
         */
        uint32_t otgfs_ghwcfg4_acgsupt:1;
        /** otgfs_ghwcfg4_enhancedlpmsupt : RO; bitpos: [13]; default: 1;
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
        uint32_t otgfs_ghwcfg4_enhancedlpmsupt:1;
        /** otgfs_ghwcfg4_phydatawidth : RO; bitpos: [15:14]; default: 2;
         *  UTMI+ PHY/ULPI-to-Internal UTMI+ Wrapper Data Width
         *  (PhyDataWidth)<vr>When a ULPI PHY is used, an internal wrapper converts ULPI to
         *  UTMI+.
         *  - 2'b00: 8 bits
         *  - 2'b01: 16 bits
         *  - 2'b10: 8/16 bits, software selectable
         *  - Others: Reserved
         */
        uint32_t otgfs_ghwcfg4_phydatawidth:2;
        /** otgfs_ghwcfg4_numctleps : RO; bitpos: [19:16]; default: 0;
         *  Number of Device Mode Control Endpoints in Addition to
         *  Endpoint 0 (NumCtlEps)
         *  Range: 0-15
         */
        uint32_t otgfs_ghwcfg4_numctleps:4;
        /** otgfs_ghwcfg4_iddgfltr : RO; bitpos: [20]; default: 1;
         *  IDDIG Filter Enable (IddgFltr)
         *  - 1'b0: No filter
         *  - 1'b1: Filter
         */
        uint32_t otgfs_ghwcfg4_iddgfltr:1;
        /** otgfs_ghwcfg4_vbusvalidfltr : RO; bitpos: [21]; default: 1;
         *  VBUS Valid Filter Enabled (VBusValidFltr)
         *  - 1'b0: No filter
         *  - 1'b1: Filter
         */
        uint32_t otgfs_ghwcfg4_vbusvalidfltr:1;
        /** otgfs_ghwcfg4_avalidfltr : RO; bitpos: [22]; default: 1;
         *  a_valid Filter Enabled (AValidFltr)
         *  - 1'b0: No filter
         *  - 1'b1: Filter
         */
        uint32_t otgfs_ghwcfg4_avalidfltr:1;
        /** otgfs_ghwcfg4_bvalidfltr : RO; bitpos: [23]; default: 1;
         *  b_valid Filter Enabled (BValidFltr)
         *  - 1'b0: No filter
         *  - 1'b1: Filter
         */
        uint32_t otgfs_ghwcfg4_bvalidfltr:1;
        /** otgfs_ghwcfg4_sessendfltr : RO; bitpos: [24]; default: 1;
         *  session_end Filter Enabled (SessEndFltr)
         *  - 1'b0: No filter
         *  - 1'b1: Filter
         */
        uint32_t otgfs_ghwcfg4_sessendfltr:1;
        /** otgfs_ghwcfg4_dedfifomode : RO; bitpos: [25]; default: 1;
         *  Enable Dedicated Transmit FIFO for device IN Endpoints
         *  (DedFifoMode)
         *  - 1'b0 : Dedicated Transmit FIFO Operation not enabled.
         *  - 1'b1 : Dedicated Transmit FIFO Operation enabled.
         */
        uint32_t otgfs_ghwcfg4_dedfifomode:1;
        /** otgfs_ghwcfg4_ineps : RO; bitpos: [29:26]; default: 4;
         *  Number of Device Mode IN Endpoints Including Control Endpoints (INEps)
         *  - 0: 1 IN Endpoint
         *  - 1: 2 IN Endpoints
         *  ....
         *  - 15: 16 IN Endpoints
         */
        uint32_t otgfs_ghwcfg4_ineps:4;
        /** otgfs_ghwcfg4_descdmaenabled : RO; bitpos: [30]; default: 1;
         *  Scatter/Gather DMA configuration
         *  - 1'b0: Non-Scatter/Gather DMA configuration
         *  - 1'b1: Scatter/Gather DMA configuration
         */
        uint32_t otgfs_ghwcfg4_descdmaenabled:1;
        /** otgfs_ghwcfg4_descdma : RO; bitpos: [31]; default: 1;
         *  Scatter/Gather DMA configuration
         *  - 1'b0: Non Dynamic configuration
         *  - 1'b1: Dynamic configuration
         *  Note: This field is configured using the OTG_EN_DESC_DMA parameter.
         */
        uint32_t otgfs_ghwcfg4_descdma:1;
    };
    uint32_t val;
} usb_otgfs_ghwcfg4_reg_t;


/** Group: Global DFIFO Configuration Register */
/** Type of otgfs_gdfifocfg register
 *  Global DFIFO Configuration Register
 */
typedef union {
    struct {
        /** otgfs_gdfifocfg_gdfifocfg : R/W; bitpos: [15:0]; default: 256;
         *  GDFIFOCfg
         *
         *  This field is for dynamic programming of the DFIFO Size. This value takes effect
         *  only when the application programs a non zero value to this register. The value
         *  programmed must conform to the guidelines described in 'FIFO RAM Allocation'. The
         *  core does not have any corrective logic if the FIFO sizes are programmed
         *  incorrectly.
         */
        uint32_t otgfs_gdfifocfg_gdfifocfg:16;
        /** otgfs_gdfifocfg_epinfobaseaddr : R/W; bitpos: [31:16]; default: 200;
         *  EPInfoBaseAddr
         *
         *  This field provides the start address of the EP info controller.
         */
        uint32_t otgfs_gdfifocfg_epinfobaseaddr:16;
    };
    uint32_t val;
} usb_otgfs_gdfifocfg_reg_t;


/** Group: Host Periodic Transmit FIFO Size Register */
/** Type of otgfs_hptxfsiz register
 *  Host Periodic Transmit FIFO Size Register  This register holds the size and the
 *  memory start address of the Periodic TxFIFO.  Note: Read the reset value of this
 *  register only after the following conditions:  - If IDDIG_FILTER is disabled, read
 *  only after PHY clock is stable.  - If IDDIG_FILTER is enabled, read only after the
 *  filter timer expires.
 */
typedef union {
    struct {
        /** otgfs_hptxfsiz_ptxfstaddr : R/W; bitpos: [9:0]; default: 512;
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
        uint32_t otgfs_hptxfsiz_ptxfstaddr:10;
        uint32_t reserved_10:6;
        /** otgfs_hptxfsiz_ptxfsize : R/W; bitpos: [24:16]; default: 256;
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
        uint32_t otgfs_hptxfsiz_ptxfsize:9;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} usb_otgfs_hptxfsiz_reg_t;


/** Group: Device IN Endpoint Transmit FIFO Size Register $i */
/** Type of otgfs_dieptxf1 register
 *  Device IN Endpoint Transmit FIFO Size Register $i This register is valid only in
 *  dedicated FIFO mode (OTG_EN_DED_TX_FIFO=1). It holds the size and memory start
 *  address of IN endpoint TxFIFOs implemented in Device mode. Each FIFO holds the data
 *  for one IN endpoint. This register is repeated for instantiated IN endpoint FIFOs 1
 *  to 15. For IN endpoint FIFO 0, use GNPTXFSIZ register for programming the size and
 *  memory start address.
 */
typedef union {
    struct {
        /** otgfs_dieptxf1_inepntxfstaddr : R/W; bitpos: [9:0]; default: 512;
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
        uint32_t otgfs_dieptxf1_inepntxfstaddr:10;
        uint32_t reserved_10:6;
        /** otgfs_dieptxf1_inepntxfdep : R/W; bitpos: [24:16]; default: 256;
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
        uint32_t otgfs_dieptxf1_inepntxfdep:9;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} usb_otgfs_dieptxf1_reg_t;

/** Type of otgfs_dieptxf2 register
 *  Device IN Endpoint Transmit FIFO Size Register $i This register is valid only in
 *  dedicated FIFO mode (OTG_EN_DED_TX_FIFO=1). It holds the size and memory start
 *  address of IN endpoint TxFIFOs implemented in Device mode. Each FIFO holds the data
 *  for one IN endpoint. This register is repeated for instantiated IN endpoint FIFOs 1
 *  to 15. For IN endpoint FIFO 0, use GNPTXFSIZ register for programming the size and
 *  memory start address.
 */
typedef union {
    struct {
        /** otgfs_dieptxf2_inepntxfstaddr : R/W; bitpos: [9:0]; default: 768;
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
        uint32_t otgfs_dieptxf2_inepntxfstaddr:10;
        uint32_t reserved_10:6;
        /** otgfs_dieptxf2_inepntxfdep : R/W; bitpos: [24:16]; default: 256;
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
        uint32_t otgfs_dieptxf2_inepntxfdep:9;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} usb_otgfs_dieptxf2_reg_t;

/** Type of otgfs_dieptxf3 register
 *  Device IN Endpoint Transmit FIFO Size Register $i This register is valid only in
 *  dedicated FIFO mode (OTG_EN_DED_TX_FIFO=1). It holds the size and memory start
 *  address of IN endpoint TxFIFOs implemented in Device mode. Each FIFO holds the data
 *  for one IN endpoint. This register is repeated for instantiated IN endpoint FIFOs 1
 *  to 15. For IN endpoint FIFO 0, use GNPTXFSIZ register for programming the size and
 *  memory start address.
 */
typedef union {
    struct {
        /** otgfs_dieptxf3_inepntxfstaddr : R/W; bitpos: [10:0]; default: 1024;
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
        uint32_t otgfs_dieptxf3_inepntxfstaddr:11;
        uint32_t reserved_11:5;
        /** otgfs_dieptxf3_inepntxfdep : R/W; bitpos: [24:16]; default: 256;
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
        uint32_t otgfs_dieptxf3_inepntxfdep:9;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} usb_otgfs_dieptxf3_reg_t;

/** Type of otgfs_dieptxf4 register
 *  Device IN Endpoint Transmit FIFO Size Register $i This register is valid only in
 *  dedicated FIFO mode (OTG_EN_DED_TX_FIFO=1). It holds the size and memory start
 *  address of IN endpoint TxFIFOs implemented in Device mode. Each FIFO holds the data
 *  for one IN endpoint. This register is repeated for instantiated IN endpoint FIFOs 1
 *  to 15. For IN endpoint FIFO 0, use GNPTXFSIZ register for programming the size and
 *  memory start address.
 */
typedef union {
    struct {
        /** otgfs_dieptxf4_inepntxfstaddr : R/W; bitpos: [10:0]; default: 1280;
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
        uint32_t otgfs_dieptxf4_inepntxfstaddr:11;
        uint32_t reserved_11:5;
        /** otgfs_dieptxf4_inepntxfdep : R/W; bitpos: [24:16]; default: 256;
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
        uint32_t otgfs_dieptxf4_inepntxfdep:9;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} usb_otgfs_dieptxf4_reg_t;


/** Group: Host Configuration Register */
/** Type of otgfs_hcfg register
 *  Host Configuration Register
 */
typedef union {
    struct {
        /** otgfs_hcfg_fslspclksel : R/W; bitpos: [1:0]; default: 0;
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
        uint32_t otgfs_hcfg_fslspclksel:2;
        /** otgfs_hcfg_fslssupp : R/W; bitpos: [2]; default: 0;
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
        uint32_t otgfs_hcfg_fslssupp:1;
        uint32_t reserved_3:4;
        /** otgfs_hcfg_ena32khzs : R/W; bitpos: [7]; default: 0;
         *  Enable 32 KHz Suspend mode (Ena32KHzS)
         *
         *  This bit can be set only in FS PHY interface is selected.
         *  Else, this bit needs to be set to zero.
         *  When FS PHY interface is chosen and this bit is set,
         *  the core expects that the PHY clock during Suspend is switched
         *  from 48 MHz to 32 KHz.
         */
        uint32_t otgfs_hcfg_ena32khzs:1;
        /** otgfs_hcfg_resvalid : R/W; bitpos: [15:8]; default: 2;
         *  Resume Validation Period  (ResValid)
         *
         *  This field is effective only when HCFG.Ena32KHzS is set.
         *  It will control the resume period when the core resumes from suspend.
         *  The core counts for 'ResValid' number of clock cycles to detect a
         *  valid resume when this is set.
         */
        uint32_t otgfs_hcfg_resvalid:8;
        uint32_t reserved_16:7;
        /** otgfs_hcfg_descdma : R/W; bitpos: [23]; default: 0;
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
        uint32_t otgfs_hcfg_descdma:1;
        /** otgfs_hcfg_frlisten : R/W; bitpos: [25:24]; default: 0;
         *  Frame List Entries(FrListEn)
         *
         *  The value in the register specifies the number of entries in the Frame list.
         *  This field is valid only in Scatter/Gather DMA mode.
         *  - 2'b00: 8 Entries
         *  - 2'b01: 16 Entries
         *  - 2'b10: 32 Entries
         *  - 2'b11: 64 Entries
         */
        uint32_t otgfs_hcfg_frlisten:2;
        /** otgfs_hcfg_perschedena : R/W; bitpos: [26]; default: 0;
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
        uint32_t otgfs_hcfg_perschedena:1;
        uint32_t reserved_27:4;
        /** otgfs_hcfg_modechtimen : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_hcfg_modechtimen:1;
    };
    uint32_t val;
} usb_otgfs_hcfg_reg_t;


/** Group: Host Frame Interval Register */
/** Type of otgfs_hfir register
 *  Host Frame Interval Register
 */
typedef union {
    struct {
        /** otgfs_hfir_frint : R/W; bitpos: [15:0]; default: 60000;
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
        uint32_t otgfs_hfir_frint:16;
        /** otgfs_hfir_hfirrldctrl : R/W; bitpos: [16]; default: 0;
         *  Reload Control (HFIRRldCtrl)
         *
         *  This bit allows dynamic reloading of the HFIR register during run time.
         *  - 1'b0 : The HFIR cannot be reloaded dynamically
         *  - 1'b1: the HFIR can be dynamically reloaded during runtime.
         *  This bit needs to be programmed during initial configuration and its value should
         *  not be changed during runtime.
         */
        uint32_t otgfs_hfir_hfirrldctrl:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} usb_otgfs_hfir_reg_t;


/** Group: Host Frame Number/Frame Time Remaining Register */
/** Type of otgfs_hfnum register
 *  Host Frame Number/Frame Time Remaining Register  This register indicates the
 *  current frame number. It also indicates the time remaining (in terms of the number
 *  of PHY clocks) in the current (micro)frame.  Note: Read the reset value of this
 *  register only after the following conditions:  - If IDDIG_FILTER is disabled, read
 *  only when the PHY clock is stable.  - If IDDIG_FILTER is enabled, read only after
 *  the filter timer expires.
 */
typedef union {
    struct {
        /** otgfs_hfnum_frnum : RO; bitpos: [15:0]; default: 16383;
         *  Frame Number (FrNum)
         *
         *  This field increments when a new SOF is transmitted on the
         *  USB, and is reset to 0 when it reaches 16'h3FFF.
         */
        uint32_t otgfs_hfnum_frnum:16;
        /** otgfs_hfnum_frrem : RO; bitpos: [31:16]; default: 0;
         *  Frame Time Remaining (FrRem)
         *
         *  Indicates the amount of time remaining in the current
         *  microframe (HS) or Frame (FS/LS), in terms of PHY clocks. This
         *  field decrements on each PHY clock. When it reaches zero, this
         *  field is reloaded with the value in the Frame Interval register and
         *  a new SOF is transmitted on the USB.
         */
        uint32_t otgfs_hfnum_frrem:16;
    };
    uint32_t val;
} usb_otgfs_hfnum_reg_t;


/** Group: Host Periodic Transmit FIFO/Queue Status Register */
/** Type of otgfs_hptxsts register
 *  Host Periodic Transmit FIFO/Queue Status Register
 */
typedef union {
    struct {
        /** otgfs_hptxsts_ptxfspcavail : RO; bitpos: [15:0]; default: 256;
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
        uint32_t otgfs_hptxsts_ptxfspcavail:16;
        /** otgfs_hptxsts_ptxqspcavail : RO; bitpos: [23:16]; default: 8;
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
        uint32_t otgfs_hptxsts_ptxqspcavail:8;
        /** otgfs_hptxsts_ptxqtop : RO; bitpos: [31:24]; default: 0;
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
        uint32_t otgfs_hptxsts_ptxqtop:8;
    };
    uint32_t val;
} usb_otgfs_hptxsts_reg_t;


/** Group: Host All Channels Interrupt Register */
/** Type of otgfs_haint register
 *  Host All Channels Interrupt Register  When a significant event occurs on a channel,
 *  the Host All Channels Interrupt register interrupts the application using the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt). This is
 *  shown in the Interrupt Hierarchy figure in the databook. There is one interrupt bit
 *  per channel, up to a maximum of 16 bits. Bits in this register are set and cleared
 *  when the application sets and clears bits in the corresponding Host Channel-n
 *  Interrupt register.
 */
typedef union {
    struct {
        /** otgfs_haint_haint : RO; bitpos: [7:0]; default: 0;
         *
         *  Channel Interrupt for channel no.
         */
        uint32_t otgfs_haint_haint:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} usb_otgfs_haint_reg_t;


/** Group: Host All Channels Interrupt Mask Register */
/** Type of otgfs_haintmsk register
 *  Host All Channels Interrupt Mask Register  The Host All Channel Interrupt Mask
 *  register works with the Host All Channel Interrupt register to interrupt the
 *  application when an event occurs on a channel. There is one interrupt mask bit per
 *  channel, up to a maximum of 16 bits.
 */
typedef union {
    struct {
        /** otgfs_haintmsk_haintmsk : R/W; bitpos: [7:0]; default: 0;
         *  Channel Interrupt Mask (HAINTMsk)
         *  One bit per channel: Bit 0 for channel 0, bit 15 for channel 15
         */
        uint32_t otgfs_haintmsk_haintmsk:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} usb_otgfs_haintmsk_reg_t;


/** Group: Host Frame List Base Address Register */
/** Type of otgfs_hflbaddr register
 *  Host Frame List Base Address Register  This register is present only in case of
 *  Scatter/Gather DMA. It is implemented as flops. This register holds the starting
 *  address of the Frame list information.
 */
typedef union {
    struct {
        /** otgfs_hflbaddr_hflbaddr : R/W; bitpos: [31:0]; default: 0;
         *  The starting address of the Frame list.
         *  This register is used only for Isochronous and Interrupt Channels.
         */
        uint32_t otgfs_hflbaddr_hflbaddr:32;
    };
    uint32_t val;
} usb_otgfs_hflbaddr_reg_t;


/** Group: Host Port Control and Status Register */
/** Type of otgfs_hprt register
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
typedef union {
    struct {
        /** otgfs_hprt_prtconnsts : RO; bitpos: [0]; default: 0;
         *  Port Connect Status (PrtConnSts)
         *  - 0: No device is attached to the port.
         *  - 1: A device is attached to the port.
         */
        uint32_t otgfs_hprt_prtconnsts:1;
        /** otgfs_hprt_prtconndet : R/W; bitpos: [1]; default: 0;
         *  Port Connect Detected (PrtConnDet)
         *
         *  The core sets this bit when a device connection is detected
         *  to trigger an interrupt to the application using the Host Port
         *  Interrupt bit of the Core Interrupt register (GINTSTS.PrtInt).This bit can be set
         *  only by the core and the application should write 1 to clear it.The application
         *  must write a 1 to this bit to clear the
         *  interrupt.
         */
        uint32_t otgfs_hprt_prtconndet:1;
        /** otgfs_hprt_prtena : R/W; bitpos: [2]; default: 0;
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
        uint32_t otgfs_hprt_prtena:1;
        /** otgfs_hprt_prtenchng : R/W; bitpos: [3]; default: 0;
         *  Port Enable/Disable Change (PrtEnChng)
         *
         *  The core sets this bit when the status of the Port Enable bit [2] of this register
         *  changes.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hprt_prtenchng:1;
        /** otgfs_hprt_prtovrcurract : RO; bitpos: [4]; default: 0;
         *  Port Overcurrent Active (PrtOvrCurrAct)
         *
         *  Indicates the overcurrent condition of the port.
         *  - 1'b0: No overcurrent condition
         *  - 1'b1: Overcurrent condition
         */
        uint32_t otgfs_hprt_prtovrcurract:1;
        /** otgfs_hprt_prtovrcurrchng : R/W; bitpos: [5]; default: 0;
         *  Port Overcurrent Change (PrtOvrCurrChng)
         *
         *  The core sets this bit when the status of the Port Overcurrent Active bit (bit 4)
         *  in this register changes.This bit can be set only by the core and the application
         *  should write 1 to clear it
         */
        uint32_t otgfs_hprt_prtovrcurrchng:1;
        /** otgfs_hprt_prtres : R/W; bitpos: [6]; default: 0;
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
        uint32_t otgfs_hprt_prtres:1;
        /** otgfs_hprt_prtsusp : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_hprt_prtsusp:1;
        /** otgfs_hprt_prtrst : R/W; bitpos: [8]; default: 0;
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
        uint32_t otgfs_hprt_prtrst:1;
        uint32_t reserved_9:1;
        /** otgfs_hprt_prtlnsts : RO; bitpos: [11:10]; default: 0;
         *  Port Line Status (PrtLnSts)
         *
         *  Indicates the current logic level USB data lines
         *  - Bit [10]: Logic level of D+
         *  - Bit [11]: Logic level of D-
         */
        uint32_t otgfs_hprt_prtlnsts:2;
        /** otgfs_hprt_prtpwr : R/W; bitpos: [12]; default: 0;
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
        uint32_t otgfs_hprt_prtpwr:1;
        /** otgfs_hprt_prttstctl : R/W; bitpos: [16:13]; default: 0;
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
        uint32_t otgfs_hprt_prttstctl:4;
        /** otgfs_hprt_prtspd : RO; bitpos: [18:17]; default: 0;
         *  Port Speed (PrtSpd)
         *
         *  Indicates the speed of the device attached to this port.
         *  - 2'b00: High speed
         *  - 2'b01: Full speed
         *  - 2'b10: Low speed
         *  - 2'b11: Reserved
         */
        uint32_t otgfs_hprt_prtspd:2;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} usb_otgfs_hprt_reg_t;


/** Group: Host Channel 0 Characteristics Register */
/** Type of otgfs_hcchar0 register
 *  Host Channel 0 Characteristics Register
 */
typedef union {
    struct {
        /** otgfs_hcchar0_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  Indicates the maximum packet size of the associated endpoint.
         */
        uint32_t otgfs_hcchar0_mps:11;
        /** otgfs_hcchar0_epnum : R/W; bitpos: [14:11]; default: 0;
         *  Endpoint Number (EPNum)
         *
         *  Indicates the endpoint number on the device serving as the data source or sink.
         */
        uint32_t otgfs_hcchar0_epnum:4;
        /** otgfs_hcchar0_epdir : R/W; bitpos: [15]; default: 0;
         *  Endpoint Direction (EPDir)
         *
         *  Indicates whether the transaction is IN or OUT.
         *  - 1'b0: OUT
         *  - 1'b1: IN
         */
        uint32_t otgfs_hcchar0_epdir:1;
        uint32_t reserved_16:1;
        /** otgfs_hcchar0_lspddev : R/W; bitpos: [17]; default: 0;
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
        uint32_t otgfs_hcchar0_lspddev:1;
        /** otgfs_hcchar0_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Indicates the transfer type selected.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_hcchar0_eptype:2;
        /** otgfs_hcchar0_ec : R/W; bitpos: [21:20]; default: 0;
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
        uint32_t otgfs_hcchar0_ec:2;
        /** otgfs_hcchar0_devaddr : R/W; bitpos: [28:22]; default: 0;
         *  Device Address (DevAddr)
         *
         *  This field selects the specific device serving as the data source
         *  or sink.
         */
        uint32_t otgfs_hcchar0_devaddr:7;
        /** otgfs_hcchar0_oddfrm : R/W; bitpos: [29]; default: 0;
         *  Odd Frame (OddFrm)
         *
         *  This field is set (reset) by the application to indicate that the OTG host must
         *  perform
         *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
         *  (isochronous and interrupt) transactions.
         *  - 1'b0: Even (micro)Frame
         *  - 1'b1: Odd (micro)Frame
         */
        uint32_t otgfs_hcchar0_oddfrm:1;
        /** otgfs_hcchar0_chdis : R/W; bitpos: [30]; default: 0;
         *  Channel Disable (ChDis)
         *
         *  The application sets this bit to stop transmitting/receiving data
         *  on a channel, even before the transfer for that channel is
         *  complete. The application must wait for the Channel Disabled
         *  interrupt before treating the channel as disabled.
         */
        uint32_t otgfs_hcchar0_chdis:1;
        /** otgfs_hcchar0_chena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_hcchar0_chena:1;
    };
    uint32_t val;
} usb_otgfs_hcchar0_reg_t;


/** Group: "Host Channel $i Interrupt Register" */
/** Type of otgfs_hcint0 register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
typedef union {
    struct {
        /** otgfs_hcint0_xfercompl : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed (XferCompl)
         *
         *  Transfer completed normally without any errors.This bit can be set only by the core
         *  and the application should write 1 to clear it.
         *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
         *  completed with IOC bit set in its descriptor.
         *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
         *  without any errors.
         */
        uint32_t otgfs_hcint0_xfercompl:1;
        /** otgfs_hcint0_chhltd : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_hcint0_chhltd:1;
        /** otgfs_hcint0_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  This is generated only in Internal DMA mode when there is an AHB error during AHB
         *  read/write. The application can read the corresponding channel's DMA address
         *  register to get the error address.
         */
        uint32_t otgfs_hcint0_ahberr:1;
        /** otgfs_hcint0_stall : R/W; bitpos: [3]; default: 0;
         *  STALL Response Received Interrupt (STALL)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint0_stall:1;
        /** otgfs_hcint0_nak : R/W; bitpos: [4]; default: 0;
         *  NAK Response Received Interrupt (NAK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint0_nak:1;
        /** otgfs_hcint0_ack : R/W; bitpos: [5]; default: 0;
         *  ACK Response Received/Transmitted Interrupt (ACK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint0_ack:1;
        /** otgfs_hcint0_nyet : R/W; bitpos: [6]; default: 0;
         *  NYET Response Received Interrupt (NYET)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint0_nyet:1;
        /** otgfs_hcint0_xacterr : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_hcint0_xacterr:1;
        /** otgfs_hcint0_bblerr : R/W; bitpos: [8]; default: 0;
         *  Babble Error (BblErr)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
         *  This bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_hcint0_bblerr:1;
        /** otgfs_hcint0_frmovrun : R/W; bitpos: [9]; default: 0;
         *  Frame Overrun (FrmOvrun).
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core. This bit can be set only by the core and the application should write
         *  1 to clear
         *  it.
         */
        uint32_t otgfs_hcint0_frmovrun:1;
        /** otgfs_hcint0_datatglerr : R/W; bitpos: [10]; default: 0;
         *
         *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
         *  application should write 1 to clear
         *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core.
         */
        uint32_t otgfs_hcint0_datatglerr:1;
        /** otgfs_hcint0_bnaintr : R/W; bitpos: [11]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process. BNA will not be generated
         *  for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint0_bnaintr:1;
        /** otgfs_hcint0_xcs_xact_err : R/W; bitpos: [12]; default: 0;
         *  Excessive Transaction Error (XCS_XACT_ERR)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
         *  not be generated for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint0_xcs_xact_err:1;
        /** otgfs_hcint0_desc_lst_rollintr : R/W; bitpos: [13]; default: 0;
         *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when the corresponding channel's descriptor list rolls over.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint0_desc_lst_rollintr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcint0_reg_t;

/** Type of otgfs_hcint1 register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
typedef union {
    struct {
        /** otgfs_hcint1_xfercompl : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed (XferCompl)
         *
         *  Transfer completed normally without any errors.This bit can be set only by the core
         *  and the application should write 1 to clear it.
         *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
         *  completed with IOC bit set in its descriptor.
         *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
         *  without any errors.
         */
        uint32_t otgfs_hcint1_xfercompl:1;
        /** otgfs_hcint1_chhltd : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_hcint1_chhltd:1;
        /** otgfs_hcint1_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  This is generated only in Internal DMA mode when there is an AHB error during AHB
         *  read/write. The application can read the corresponding channel's DMA address
         *  register to get the error address.
         */
        uint32_t otgfs_hcint1_ahberr:1;
        /** otgfs_hcint1_stall : R/W; bitpos: [3]; default: 0;
         *  STALL Response Received Interrupt (STALL)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint1_stall:1;
        /** otgfs_hcint1_nak : R/W; bitpos: [4]; default: 0;
         *  NAK Response Received Interrupt (NAK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint1_nak:1;
        /** otgfs_hcint1_ack : R/W; bitpos: [5]; default: 0;
         *  ACK Response Received/Transmitted Interrupt (ACK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint1_ack:1;
        /** otgfs_hcint1_nyet : R/W; bitpos: [6]; default: 0;
         *  NYET Response Received Interrupt (NYET)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint1_nyet:1;
        /** otgfs_hcint1_xacterr : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_hcint1_xacterr:1;
        /** otgfs_hcint1_bblerr : R/W; bitpos: [8]; default: 0;
         *  Babble Error (BblErr)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
         *  This bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_hcint1_bblerr:1;
        /** otgfs_hcint1_frmovrun : R/W; bitpos: [9]; default: 0;
         *  Frame Overrun (FrmOvrun).
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core. This bit can be set only by the core and the application should write
         *  1 to clear
         *  it.
         */
        uint32_t otgfs_hcint1_frmovrun:1;
        /** otgfs_hcint1_datatglerr : R/W; bitpos: [10]; default: 0;
         *
         *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
         *  application should write 1 to clear
         *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core.
         */
        uint32_t otgfs_hcint1_datatglerr:1;
        /** otgfs_hcint1_bnaintr : R/W; bitpos: [11]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process. BNA will not be generated
         *  for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint1_bnaintr:1;
        /** otgfs_hcint1_xcs_xact_err : R/W; bitpos: [12]; default: 0;
         *  Excessive Transaction Error (XCS_XACT_ERR)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
         *  not be generated for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint1_xcs_xact_err:1;
        /** otgfs_hcint1_desc_lst_rollintr : R/W; bitpos: [13]; default: 0;
         *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when the corresponding channel's descriptor list rolls over.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint1_desc_lst_rollintr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcint1_reg_t;

/** Type of otgfs_hcint2 register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
typedef union {
    struct {
        /** otgfs_hcint2_xfercompl : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed (XferCompl)
         *
         *  Transfer completed normally without any errors.This bit can be set only by the core
         *  and the application should write 1 to clear it.
         *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
         *  completed with IOC bit set in its descriptor.
         *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
         *  without any errors.
         */
        uint32_t otgfs_hcint2_xfercompl:1;
        /** otgfs_hcint2_chhltd : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_hcint2_chhltd:1;
        /** otgfs_hcint2_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  This is generated only in Internal DMA mode when there is an AHB error during AHB
         *  read/write. The application can read the corresponding channel's DMA address
         *  register to get the error address.
         */
        uint32_t otgfs_hcint2_ahberr:1;
        /** otgfs_hcint2_stall : R/W; bitpos: [3]; default: 0;
         *  STALL Response Received Interrupt (STALL)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint2_stall:1;
        /** otgfs_hcint2_nak : R/W; bitpos: [4]; default: 0;
         *  NAK Response Received Interrupt (NAK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint2_nak:1;
        /** otgfs_hcint2_ack : R/W; bitpos: [5]; default: 0;
         *  ACK Response Received/Transmitted Interrupt (ACK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint2_ack:1;
        /** otgfs_hcint2_nyet : R/W; bitpos: [6]; default: 0;
         *  NYET Response Received Interrupt (NYET)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint2_nyet:1;
        /** otgfs_hcint2_xacterr : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_hcint2_xacterr:1;
        /** otgfs_hcint2_bblerr : R/W; bitpos: [8]; default: 0;
         *  Babble Error (BblErr)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
         *  This bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_hcint2_bblerr:1;
        /** otgfs_hcint2_frmovrun : R/W; bitpos: [9]; default: 0;
         *  Frame Overrun (FrmOvrun).
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core. This bit can be set only by the core and the application should write
         *  1 to clear
         *  it.
         */
        uint32_t otgfs_hcint2_frmovrun:1;
        /** otgfs_hcint2_datatglerr : R/W; bitpos: [10]; default: 0;
         *
         *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
         *  application should write 1 to clear
         *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core.
         */
        uint32_t otgfs_hcint2_datatglerr:1;
        /** otgfs_hcint2_bnaintr : R/W; bitpos: [11]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process. BNA will not be generated
         *  for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint2_bnaintr:1;
        /** otgfs_hcint2_xcs_xact_err : R/W; bitpos: [12]; default: 0;
         *  Excessive Transaction Error (XCS_XACT_ERR)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
         *  not be generated for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint2_xcs_xact_err:1;
        /** otgfs_hcint2_desc_lst_rollintr : R/W; bitpos: [13]; default: 0;
         *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when the corresponding channel's descriptor list rolls over.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint2_desc_lst_rollintr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcint2_reg_t;

/** Type of otgfs_hcint3 register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
typedef union {
    struct {
        /** otgfs_hcint3_xfercompl : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed (XferCompl)
         *
         *  Transfer completed normally without any errors.This bit can be set only by the core
         *  and the application should write 1 to clear it.
         *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
         *  completed with IOC bit set in its descriptor.
         *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
         *  without any errors.
         */
        uint32_t otgfs_hcint3_xfercompl:1;
        /** otgfs_hcint3_chhltd : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_hcint3_chhltd:1;
        /** otgfs_hcint3_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  This is generated only in Internal DMA mode when there is an AHB error during AHB
         *  read/write. The application can read the corresponding channel's DMA address
         *  register to get the error address.
         */
        uint32_t otgfs_hcint3_ahberr:1;
        /** otgfs_hcint3_stall : R/W; bitpos: [3]; default: 0;
         *  STALL Response Received Interrupt (STALL)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint3_stall:1;
        /** otgfs_hcint3_nak : R/W; bitpos: [4]; default: 0;
         *  NAK Response Received Interrupt (NAK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint3_nak:1;
        /** otgfs_hcint3_ack : R/W; bitpos: [5]; default: 0;
         *  ACK Response Received/Transmitted Interrupt (ACK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint3_ack:1;
        /** otgfs_hcint3_nyet : R/W; bitpos: [6]; default: 0;
         *  NYET Response Received Interrupt (NYET)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint3_nyet:1;
        /** otgfs_hcint3_xacterr : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_hcint3_xacterr:1;
        /** otgfs_hcint3_bblerr : R/W; bitpos: [8]; default: 0;
         *  Babble Error (BblErr)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
         *  This bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_hcint3_bblerr:1;
        /** otgfs_hcint3_frmovrun : R/W; bitpos: [9]; default: 0;
         *  Frame Overrun (FrmOvrun).
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core. This bit can be set only by the core and the application should write
         *  1 to clear
         *  it.
         */
        uint32_t otgfs_hcint3_frmovrun:1;
        /** otgfs_hcint3_datatglerr : R/W; bitpos: [10]; default: 0;
         *
         *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
         *  application should write 1 to clear
         *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core.
         */
        uint32_t otgfs_hcint3_datatglerr:1;
        /** otgfs_hcint3_bnaintr : R/W; bitpos: [11]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process. BNA will not be generated
         *  for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint3_bnaintr:1;
        /** otgfs_hcint3_xcs_xact_err : R/W; bitpos: [12]; default: 0;
         *  Excessive Transaction Error (XCS_XACT_ERR)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
         *  not be generated for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint3_xcs_xact_err:1;
        /** otgfs_hcint3_desc_lst_rollintr : R/W; bitpos: [13]; default: 0;
         *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when the corresponding channel's descriptor list rolls over.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint3_desc_lst_rollintr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcint3_reg_t;

/** Type of otgfs_hcint4 register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
typedef union {
    struct {
        /** otgfs_hcint4_xfercompl : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed (XferCompl)
         *
         *  Transfer completed normally without any errors.This bit can be set only by the core
         *  and the application should write 1 to clear it.
         *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
         *  completed with IOC bit set in its descriptor.
         *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
         *  without any errors.
         */
        uint32_t otgfs_hcint4_xfercompl:1;
        /** otgfs_hcint4_chhltd : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_hcint4_chhltd:1;
        /** otgfs_hcint4_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  This is generated only in Internal DMA mode when there is an AHB error during AHB
         *  read/write. The application can read the corresponding channel's DMA address
         *  register to get the error address.
         */
        uint32_t otgfs_hcint4_ahberr:1;
        /** otgfs_hcint4_stall : R/W; bitpos: [3]; default: 0;
         *  STALL Response Received Interrupt (STALL)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint4_stall:1;
        /** otgfs_hcint4_nak : R/W; bitpos: [4]; default: 0;
         *  NAK Response Received Interrupt (NAK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint4_nak:1;
        /** otgfs_hcint4_ack : R/W; bitpos: [5]; default: 0;
         *  ACK Response Received/Transmitted Interrupt (ACK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint4_ack:1;
        /** otgfs_hcint4_nyet : R/W; bitpos: [6]; default: 0;
         *  NYET Response Received Interrupt (NYET)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint4_nyet:1;
        /** otgfs_hcint4_xacterr : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_hcint4_xacterr:1;
        /** otgfs_hcint4_bblerr : R/W; bitpos: [8]; default: 0;
         *  Babble Error (BblErr)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
         *  This bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_hcint4_bblerr:1;
        /** otgfs_hcint4_frmovrun : R/W; bitpos: [9]; default: 0;
         *  Frame Overrun (FrmOvrun).
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core. This bit can be set only by the core and the application should write
         *  1 to clear
         *  it.
         */
        uint32_t otgfs_hcint4_frmovrun:1;
        /** otgfs_hcint4_datatglerr : R/W; bitpos: [10]; default: 0;
         *
         *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
         *  application should write 1 to clear
         *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core.
         */
        uint32_t otgfs_hcint4_datatglerr:1;
        /** otgfs_hcint4_bnaintr : R/W; bitpos: [11]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process. BNA will not be generated
         *  for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint4_bnaintr:1;
        /** otgfs_hcint4_xcs_xact_err : R/W; bitpos: [12]; default: 0;
         *  Excessive Transaction Error (XCS_XACT_ERR)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
         *  not be generated for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint4_xcs_xact_err:1;
        /** otgfs_hcint4_desc_lst_rollintr : R/W; bitpos: [13]; default: 0;
         *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when the corresponding channel's descriptor list rolls over.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint4_desc_lst_rollintr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcint4_reg_t;

/** Type of otgfs_hcint5 register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
typedef union {
    struct {
        /** otgfs_hcint5_xfercompl : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed (XferCompl)
         *
         *  Transfer completed normally without any errors.This bit can be set only by the core
         *  and the application should write 1 to clear it.
         *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
         *  completed with IOC bit set in its descriptor.
         *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
         *  without any errors.
         */
        uint32_t otgfs_hcint5_xfercompl:1;
        /** otgfs_hcint5_chhltd : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_hcint5_chhltd:1;
        /** otgfs_hcint5_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  This is generated only in Internal DMA mode when there is an AHB error during AHB
         *  read/write. The application can read the corresponding channel's DMA address
         *  register to get the error address.
         */
        uint32_t otgfs_hcint5_ahberr:1;
        /** otgfs_hcint5_stall : R/W; bitpos: [3]; default: 0;
         *  STALL Response Received Interrupt (STALL)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint5_stall:1;
        /** otgfs_hcint5_nak : R/W; bitpos: [4]; default: 0;
         *  NAK Response Received Interrupt (NAK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint5_nak:1;
        /** otgfs_hcint5_ack : R/W; bitpos: [5]; default: 0;
         *  ACK Response Received/Transmitted Interrupt (ACK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint5_ack:1;
        /** otgfs_hcint5_nyet : R/W; bitpos: [6]; default: 0;
         *  NYET Response Received Interrupt (NYET)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint5_nyet:1;
        /** otgfs_hcint5_xacterr : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_hcint5_xacterr:1;
        /** otgfs_hcint5_bblerr : R/W; bitpos: [8]; default: 0;
         *  Babble Error (BblErr)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
         *  This bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_hcint5_bblerr:1;
        /** otgfs_hcint5_frmovrun : R/W; bitpos: [9]; default: 0;
         *  Frame Overrun (FrmOvrun).
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core. This bit can be set only by the core and the application should write
         *  1 to clear
         *  it.
         */
        uint32_t otgfs_hcint5_frmovrun:1;
        /** otgfs_hcint5_datatglerr : R/W; bitpos: [10]; default: 0;
         *
         *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
         *  application should write 1 to clear
         *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core.
         */
        uint32_t otgfs_hcint5_datatglerr:1;
        /** otgfs_hcint5_bnaintr : R/W; bitpos: [11]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process. BNA will not be generated
         *  for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint5_bnaintr:1;
        /** otgfs_hcint5_xcs_xact_err : R/W; bitpos: [12]; default: 0;
         *  Excessive Transaction Error (XCS_XACT_ERR)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
         *  not be generated for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint5_xcs_xact_err:1;
        /** otgfs_hcint5_desc_lst_rollintr : R/W; bitpos: [13]; default: 0;
         *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when the corresponding channel's descriptor list rolls over.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint5_desc_lst_rollintr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcint5_reg_t;

/** Type of otgfs_hcint6 register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
typedef union {
    struct {
        /** otgfs_hcint6_xfercompl : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed (XferCompl)
         *
         *  Transfer completed normally without any errors.This bit can be set only by the core
         *  and the application should write 1 to clear it.
         *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
         *  completed with IOC bit set in its descriptor.
         *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
         *  without any errors.
         */
        uint32_t otgfs_hcint6_xfercompl:1;
        /** otgfs_hcint6_chhltd : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_hcint6_chhltd:1;
        /** otgfs_hcint6_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  This is generated only in Internal DMA mode when there is an AHB error during AHB
         *  read/write. The application can read the corresponding channel's DMA address
         *  register to get the error address.
         */
        uint32_t otgfs_hcint6_ahberr:1;
        /** otgfs_hcint6_stall : R/W; bitpos: [3]; default: 0;
         *  STALL Response Received Interrupt (STALL)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint6_stall:1;
        /** otgfs_hcint6_nak : R/W; bitpos: [4]; default: 0;
         *  NAK Response Received Interrupt (NAK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint6_nak:1;
        /** otgfs_hcint6_ack : R/W; bitpos: [5]; default: 0;
         *  ACK Response Received/Transmitted Interrupt (ACK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint6_ack:1;
        /** otgfs_hcint6_nyet : R/W; bitpos: [6]; default: 0;
         *  NYET Response Received Interrupt (NYET)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint6_nyet:1;
        /** otgfs_hcint6_xacterr : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_hcint6_xacterr:1;
        /** otgfs_hcint6_bblerr : R/W; bitpos: [8]; default: 0;
         *  Babble Error (BblErr)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
         *  This bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_hcint6_bblerr:1;
        /** otgfs_hcint6_frmovrun : R/W; bitpos: [9]; default: 0;
         *  Frame Overrun (FrmOvrun).
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core. This bit can be set only by the core and the application should write
         *  1 to clear
         *  it.
         */
        uint32_t otgfs_hcint6_frmovrun:1;
        /** otgfs_hcint6_datatglerr : R/W; bitpos: [10]; default: 0;
         *
         *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
         *  application should write 1 to clear
         *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core.
         */
        uint32_t otgfs_hcint6_datatglerr:1;
        /** otgfs_hcint6_bnaintr : R/W; bitpos: [11]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process. BNA will not be generated
         *  for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint6_bnaintr:1;
        /** otgfs_hcint6_xcs_xact_err : R/W; bitpos: [12]; default: 0;
         *  Excessive Transaction Error (XCS_XACT_ERR)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
         *  not be generated for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint6_xcs_xact_err:1;
        /** otgfs_hcint6_desc_lst_rollintr : R/W; bitpos: [13]; default: 0;
         *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when the corresponding channel's descriptor list rolls over.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint6_desc_lst_rollintr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcint6_reg_t;

/** Type of otgfs_hcint7 register
 *  Host Channel $i Interrupt Register  This register indicates the status of a channel
 *  with respect to USB- and AHB-related events. It is shown in the Interrupt Hierarchy
 *  figure in the databook. The application must read this register when the Host
 *  Channels Interrupt bit of the Core Interrupt register (GINTSTS.HChInt) is set.
 *  Before the application can read this register, it must first read the Host All
 *  Channels Interrupt (HAINT) register to get the exact channel number for the Host
 *  Channel-n Interrupt register. The application must clear the appropriate bit in
 *  this register to clear the corresponding bits in the HAINT and GINTSTS registers.
 */
typedef union {
    struct {
        /** otgfs_hcint7_xfercompl : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed (XferCompl)
         *
         *  Transfer completed normally without any errors.This bit can be set only by the core
         *  and the application should write 1 to clear it.
         *  - For Scatter/Gather DMA mode, it indicates that current descriptor processing got
         *  completed with IOC bit set in its descriptor.
         *  - In non Scatter/Gather DMA mode, it indicates that Transfer completed normally
         *  without any errors.
         */
        uint32_t otgfs_hcint7_xfercompl:1;
        /** otgfs_hcint7_chhltd : R/W; bitpos: [1]; default: 0;
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
        uint32_t otgfs_hcint7_chhltd:1;
        /** otgfs_hcint7_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  This is generated only in Internal DMA mode when there is an AHB error during AHB
         *  read/write. The application can read the corresponding channel's DMA address
         *  register to get the error address.
         */
        uint32_t otgfs_hcint7_ahberr:1;
        /** otgfs_hcint7_stall : R/W; bitpos: [3]; default: 0;
         *  STALL Response Received Interrupt (STALL)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint7_stall:1;
        /** otgfs_hcint7_nak : R/W; bitpos: [4]; default: 0;
         *  NAK Response Received Interrupt (NAK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint7_nak:1;
        /** otgfs_hcint7_ack : R/W; bitpos: [5]; default: 0;
         *  ACK Response Received/Transmitted Interrupt (ACK)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint7_ack:1;
        /** otgfs_hcint7_nyet : R/W; bitpos: [6]; default: 0;
         *  NYET Response Received Interrupt (NYET)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the
         *  core.This bit can be set only by the core and the application should write 1 to
         *  clear it.
         */
        uint32_t otgfs_hcint7_nyet:1;
        /** otgfs_hcint7_xacterr : R/W; bitpos: [7]; default: 0;
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
        uint32_t otgfs_hcint7_xacterr:1;
        /** otgfs_hcint7_bblerr : R/W; bitpos: [8]; default: 0;
         *  Babble Error (BblErr)
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked in the core.
         *  This bit can be set only by the core and the application should write 1 to clear it.
         */
        uint32_t otgfs_hcint7_bblerr:1;
        /** otgfs_hcint7_frmovrun : R/W; bitpos: [9]; default: 0;
         *  Frame Overrun (FrmOvrun).
         *
         *  In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core. This bit can be set only by the core and the application should write
         *  1 to clear
         *  it.
         */
        uint32_t otgfs_hcint7_frmovrun:1;
        /** otgfs_hcint7_datatglerr : R/W; bitpos: [10]; default: 0;
         *
         *  Data Toggle Error (DataTglErr).This bit can be set only by the core and the
         *  application should write 1 to clear
         *  it.In Scatter/Gather DMA mode, the interrupt due to this bit is masked
         *  in the core.
         */
        uint32_t otgfs_hcint7_datatglerr:1;
        /** otgfs_hcint7_bnaintr : R/W; bitpos: [11]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process. BNA will not be generated
         *  for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint7_bnaintr:1;
        /** otgfs_hcint7_xcs_xact_err : R/W; bitpos: [12]; default: 0;
         *  Excessive Transaction Error (XCS_XACT_ERR)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when 3 consecutive transaction errors occurred on the USB bus. XCS_XACT_ERR will
         *  not be generated for Isochronous channels.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint7_xcs_xact_err:1;
        /** otgfs_hcint7_desc_lst_rollintr : R/W; bitpos: [13]; default: 0;
         *  Descriptor rollover interrupt (DESC_LST_ROLLIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core sets this
         *  bit
         *  when the corresponding channel's descriptor list rolls over.
         *  For non Scatter/Gather DMA mode, this bit is reserved.
         */
        uint32_t otgfs_hcint7_desc_lst_rollintr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcint7_reg_t;


/** Group: "Host Channel $i Interrupt Mask Register" */
/** Type of otgfs_hcintmsk0 register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
typedef union {
    struct {
        /** otgfs_hcintmsk0_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *
         *  Transfer Completed Mask (XferComplMsk)
         */
        uint32_t otgfs_hcintmsk0_xfercomplmsk:1;
        /** otgfs_hcintmsk0_chhltdmsk : R/W; bitpos: [1]; default: 0;
         *
         *  Channel Halted Mask (ChHltdMsk)
         */
        uint32_t otgfs_hcintmsk0_chhltdmsk:1;
        /** otgfs_hcintmsk0_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *
         *  AHB Error Mask (AHBErrMsk)
         *  In  scatter/gather  DMA  mode  for  host,
         *  interrupts will not be generated due to the corresponding bits set in
         *  HCINTn.
         */
        uint32_t otgfs_hcintmsk0_ahberrmsk:1;
        uint32_t reserved_3:8;
        /** otgfs_hcintmsk0_bnaintrmsk : R/W; bitpos: [11]; default: 0;
         *
         *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk0_bnaintrmsk:1;
        uint32_t reserved_12:1;
        /** otgfs_hcintmsk0_desc_lst_rollintrmsk : R/W; bitpos: [13]; default: 0;
         *
         *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk0_desc_lst_rollintrmsk:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcintmsk0_reg_t;

/** Type of otgfs_hcintmsk1 register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
typedef union {
    struct {
        /** otgfs_hcintmsk1_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *
         *  Transfer Completed Mask (XferComplMsk)
         */
        uint32_t otgfs_hcintmsk1_xfercomplmsk:1;
        /** otgfs_hcintmsk1_chhltdmsk : R/W; bitpos: [1]; default: 0;
         *
         *  Channel Halted Mask (ChHltdMsk)
         */
        uint32_t otgfs_hcintmsk1_chhltdmsk:1;
        /** otgfs_hcintmsk1_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *
         *  AHB Error Mask (AHBErrMsk)
         *  In  scatter/gather  DMA  mode  for  host,
         *  interrupts will not be generated due to the corresponding bits set in
         *  HCINTn.
         */
        uint32_t otgfs_hcintmsk1_ahberrmsk:1;
        uint32_t reserved_3:8;
        /** otgfs_hcintmsk1_bnaintrmsk : R/W; bitpos: [11]; default: 0;
         *
         *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk1_bnaintrmsk:1;
        uint32_t reserved_12:1;
        /** otgfs_hcintmsk1_desc_lst_rollintrmsk : R/W; bitpos: [13]; default: 0;
         *
         *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk1_desc_lst_rollintrmsk:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcintmsk1_reg_t;

/** Type of otgfs_hcintmsk2 register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
typedef union {
    struct {
        /** otgfs_hcintmsk2_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *
         *  Transfer Completed Mask (XferComplMsk)
         */
        uint32_t otgfs_hcintmsk2_xfercomplmsk:1;
        /** otgfs_hcintmsk2_chhltdmsk : R/W; bitpos: [1]; default: 0;
         *
         *  Channel Halted Mask (ChHltdMsk)
         */
        uint32_t otgfs_hcintmsk2_chhltdmsk:1;
        /** otgfs_hcintmsk2_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *
         *  AHB Error Mask (AHBErrMsk)
         *  In  scatter/gather  DMA  mode  for  host,
         *  interrupts will not be generated due to the corresponding bits set in
         *  HCINTn.
         */
        uint32_t otgfs_hcintmsk2_ahberrmsk:1;
        uint32_t reserved_3:8;
        /** otgfs_hcintmsk2_bnaintrmsk : R/W; bitpos: [11]; default: 0;
         *
         *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk2_bnaintrmsk:1;
        uint32_t reserved_12:1;
        /** otgfs_hcintmsk2_desc_lst_rollintrmsk : R/W; bitpos: [13]; default: 0;
         *
         *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk2_desc_lst_rollintrmsk:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcintmsk2_reg_t;

/** Type of otgfs_hcintmsk3 register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
typedef union {
    struct {
        /** otgfs_hcintmsk3_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *
         *  Transfer Completed Mask (XferComplMsk)
         */
        uint32_t otgfs_hcintmsk3_xfercomplmsk:1;
        /** otgfs_hcintmsk3_chhltdmsk : R/W; bitpos: [1]; default: 0;
         *
         *  Channel Halted Mask (ChHltdMsk)
         */
        uint32_t otgfs_hcintmsk3_chhltdmsk:1;
        /** otgfs_hcintmsk3_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *
         *  AHB Error Mask (AHBErrMsk)
         *  In  scatter/gather  DMA  mode  for  host,
         *  interrupts will not be generated due to the corresponding bits set in
         *  HCINTn.
         */
        uint32_t otgfs_hcintmsk3_ahberrmsk:1;
        uint32_t reserved_3:8;
        /** otgfs_hcintmsk3_bnaintrmsk : R/W; bitpos: [11]; default: 0;
         *
         *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk3_bnaintrmsk:1;
        uint32_t reserved_12:1;
        /** otgfs_hcintmsk3_desc_lst_rollintrmsk : R/W; bitpos: [13]; default: 0;
         *
         *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk3_desc_lst_rollintrmsk:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcintmsk3_reg_t;

/** Type of otgfs_hcintmsk4 register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
typedef union {
    struct {
        /** otgfs_hcintmsk4_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *
         *  Transfer Completed Mask (XferComplMsk)
         */
        uint32_t otgfs_hcintmsk4_xfercomplmsk:1;
        /** otgfs_hcintmsk4_chhltdmsk : R/W; bitpos: [1]; default: 0;
         *
         *  Channel Halted Mask (ChHltdMsk)
         */
        uint32_t otgfs_hcintmsk4_chhltdmsk:1;
        /** otgfs_hcintmsk4_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *
         *  AHB Error Mask (AHBErrMsk)
         *  In  scatter/gather  DMA  mode  for  host,
         *  interrupts will not be generated due to the corresponding bits set in
         *  HCINTn.
         */
        uint32_t otgfs_hcintmsk4_ahberrmsk:1;
        uint32_t reserved_3:8;
        /** otgfs_hcintmsk4_bnaintrmsk : R/W; bitpos: [11]; default: 0;
         *
         *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk4_bnaintrmsk:1;
        uint32_t reserved_12:1;
        /** otgfs_hcintmsk4_desc_lst_rollintrmsk : R/W; bitpos: [13]; default: 0;
         *
         *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk4_desc_lst_rollintrmsk:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcintmsk4_reg_t;

/** Type of otgfs_hcintmsk5 register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
typedef union {
    struct {
        /** otgfs_hcintmsk5_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *
         *  Transfer Completed Mask (XferComplMsk)
         */
        uint32_t otgfs_hcintmsk5_xfercomplmsk:1;
        /** otgfs_hcintmsk5_chhltdmsk : R/W; bitpos: [1]; default: 0;
         *
         *  Channel Halted Mask (ChHltdMsk)
         */
        uint32_t otgfs_hcintmsk5_chhltdmsk:1;
        /** otgfs_hcintmsk5_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *
         *  AHB Error Mask (AHBErrMsk)
         *  In  scatter/gather  DMA  mode  for  host,
         *  interrupts will not be generated due to the corresponding bits set in
         *  HCINTn.
         */
        uint32_t otgfs_hcintmsk5_ahberrmsk:1;
        uint32_t reserved_3:8;
        /** otgfs_hcintmsk5_bnaintrmsk : R/W; bitpos: [11]; default: 0;
         *
         *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk5_bnaintrmsk:1;
        uint32_t reserved_12:1;
        /** otgfs_hcintmsk5_desc_lst_rollintrmsk : R/W; bitpos: [13]; default: 0;
         *
         *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk5_desc_lst_rollintrmsk:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcintmsk5_reg_t;

/** Type of otgfs_hcintmsk6 register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
typedef union {
    struct {
        /** otgfs_hcintmsk6_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *
         *  Transfer Completed Mask (XferComplMsk)
         */
        uint32_t otgfs_hcintmsk6_xfercomplmsk:1;
        /** otgfs_hcintmsk6_chhltdmsk : R/W; bitpos: [1]; default: 0;
         *
         *  Channel Halted Mask (ChHltdMsk)
         */
        uint32_t otgfs_hcintmsk6_chhltdmsk:1;
        /** otgfs_hcintmsk6_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *
         *  AHB Error Mask (AHBErrMsk)
         *  In  scatter/gather  DMA  mode  for  host,
         *  interrupts will not be generated due to the corresponding bits set in
         *  HCINTn.
         */
        uint32_t otgfs_hcintmsk6_ahberrmsk:1;
        uint32_t reserved_3:8;
        /** otgfs_hcintmsk6_bnaintrmsk : R/W; bitpos: [11]; default: 0;
         *
         *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk6_bnaintrmsk:1;
        uint32_t reserved_12:1;
        /** otgfs_hcintmsk6_desc_lst_rollintrmsk : R/W; bitpos: [13]; default: 0;
         *
         *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk6_desc_lst_rollintrmsk:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcintmsk6_reg_t;

/** Type of otgfs_hcintmsk7 register
 *  Host Channel $i Interrupt Mask Register  This register reflects the mask for each
 *  channel status described in the previous section.
 */
typedef union {
    struct {
        /** otgfs_hcintmsk7_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *
         *  Transfer Completed Mask (XferComplMsk)
         */
        uint32_t otgfs_hcintmsk7_xfercomplmsk:1;
        /** otgfs_hcintmsk7_chhltdmsk : R/W; bitpos: [1]; default: 0;
         *
         *  Channel Halted Mask (ChHltdMsk)
         */
        uint32_t otgfs_hcintmsk7_chhltdmsk:1;
        /** otgfs_hcintmsk7_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *
         *  AHB Error Mask (AHBErrMsk)
         *  In  scatter/gather  DMA  mode  for  host,
         *  interrupts will not be generated due to the corresponding bits set in
         *  HCINTn.
         */
        uint32_t otgfs_hcintmsk7_ahberrmsk:1;
        uint32_t reserved_3:8;
        /** otgfs_hcintmsk7_bnaintrmsk : R/W; bitpos: [11]; default: 0;
         *
         *  BNA  (Buffer  Not  Available)  Interrupt  mask  register  (BNAIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk7_bnaintrmsk:1;
        uint32_t reserved_12:1;
        /** otgfs_hcintmsk7_desc_lst_rollintrmsk : R/W; bitpos: [13]; default: 0;
         *
         *  Descriptor List rollover interrupt Mask register(DESC_LST_ROLLIntrMsk)
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         */
        uint32_t otgfs_hcintmsk7_desc_lst_rollintrmsk:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_hcintmsk7_reg_t;


/** Group: Host Channel 0 Transfer Size Register */
/** Type of otgfs_hctsiz0 register
 *  Host Channel 0 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_hctsiz0_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_hctsiz0_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_hctsiz0_pktcnt : R/W; bitpos: [25:19]; default: 0;
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
        uint32_t otgfs_hctsiz0_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_hctsiz0_pid : R/W; bitpos: [30:29]; default: 0;
         *  PID (Pid)
         *
         *  The application programs this field with the type of PID to use for the initial
         *  transaction. The host maintains this field for the rest of the transfer.
         *  - 2'b00: DATA0
         *  - 2'b01: DATA2
         *  - 2'b10: DATA1
         *  - 2'b11: MDATA (non-control)/SETUP (control)
         */
        uint32_t otgfs_hctsiz0_pid:2;
        /** otgfs_hctsiz0_dopng : R/W; bitpos: [31]; default: 0;
         *  Do Ping (DoPng)
         *
         *  This bit is used only for OUT transfers.
         *  Setting this field to 1 directs the host to do PING protocol.
         *
         *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
         *  it disables the channel.
         */
        uint32_t otgfs_hctsiz0_dopng:1;
    };
    uint32_t val;
} usb_otgfs_hctsiz0_reg_t;


/** Group: "Host Channel $i DMA Address Register" */
/** Type of otgfs_hcdma0 register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
typedef union {
    struct {
        /** otgfs_hcdma0_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_hcdma0_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_hcdma0_reg_t;

/** Type of otgfs_hcdma1 register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
typedef union {
    struct {
        /** otgfs_hcdma1_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_hcdma1_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_hcdma1_reg_t;

/** Type of otgfs_hcdma2 register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
typedef union {
    struct {
        /** otgfs_hcdma2_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_hcdma2_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_hcdma2_reg_t;

/** Type of otgfs_hcdma3 register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
typedef union {
    struct {
        /** otgfs_hcdma3_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_hcdma3_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_hcdma3_reg_t;

/** Type of otgfs_hcdma4 register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
typedef union {
    struct {
        /** otgfs_hcdma4_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_hcdma4_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_hcdma4_reg_t;

/** Type of otgfs_hcdma5 register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
typedef union {
    struct {
        /** otgfs_hcdma5_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_hcdma5_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_hcdma5_reg_t;

/** Type of otgfs_hcdma6 register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
typedef union {
    struct {
        /** otgfs_hcdma6_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_hcdma6_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_hcdma6_reg_t;

/** Type of otgfs_hcdma7 register
 *  Host Channel $i DMA Address Register  This register is used by the OTG host in the
 *  internal DMA mode to maintain the current buffer pointer for IN/OUT transactions.
 *  The starting DMA address must be DWORD-aligned.
 */
typedef union {
    struct {
        /** otgfs_hcdma7_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_hcdma7_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_hcdma7_reg_t;


/** Group: "Host Channel $i DMA Buffer Address Register" */
/** Type of otgfs_hcdmab0 register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
typedef union {
    struct {
        /** otgfs_hcdmab0_hcdmab : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.
         *  This register is updated as and when the data transfer for the corresponding end
         *  point
         *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
         *  this
         *  field is reserved.
         */
        uint32_t otgfs_hcdmab0_hcdmab:32;
    };
    uint32_t val;
} usb_otgfs_hcdmab0_reg_t;

/** Type of otgfs_hcdmab1 register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
typedef union {
    struct {
        /** otgfs_hcdmab1_hcdmab : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.
         *  This register is updated as and when the data transfer for the corresponding end
         *  point
         *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
         *  this
         *  field is reserved.
         */
        uint32_t otgfs_hcdmab1_hcdmab:32;
    };
    uint32_t val;
} usb_otgfs_hcdmab1_reg_t;

/** Type of otgfs_hcdmab2 register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
typedef union {
    struct {
        /** otgfs_hcdmab2_hcdmab : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.
         *  This register is updated as and when the data transfer for the corresponding end
         *  point
         *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
         *  this
         *  field is reserved.
         */
        uint32_t otgfs_hcdmab2_hcdmab:32;
    };
    uint32_t val;
} usb_otgfs_hcdmab2_reg_t;

/** Type of otgfs_hcdmab3 register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
typedef union {
    struct {
        /** otgfs_hcdmab3_hcdmab : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.
         *  This register is updated as and when the data transfer for the corresponding end
         *  point
         *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
         *  this
         *  field is reserved.
         */
        uint32_t otgfs_hcdmab3_hcdmab:32;
    };
    uint32_t val;
} usb_otgfs_hcdmab3_reg_t;

/** Type of otgfs_hcdmab4 register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
typedef union {
    struct {
        /** otgfs_hcdmab4_hcdmab : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.
         *  This register is updated as and when the data transfer for the corresponding end
         *  point
         *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
         *  this
         *  field is reserved.
         */
        uint32_t otgfs_hcdmab4_hcdmab:32;
    };
    uint32_t val;
} usb_otgfs_hcdmab4_reg_t;

/** Type of otgfs_hcdmab5 register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
typedef union {
    struct {
        /** otgfs_hcdmab5_hcdmab : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.
         *  This register is updated as and when the data transfer for the corresponding end
         *  point
         *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
         *  this
         *  field is reserved.
         */
        uint32_t otgfs_hcdmab5_hcdmab:32;
    };
    uint32_t val;
} usb_otgfs_hcdmab5_reg_t;

/** Type of otgfs_hcdmab6 register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
typedef union {
    struct {
        /** otgfs_hcdmab6_hcdmab : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.
         *  This register is updated as and when the data transfer for the corresponding end
         *  point
         *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
         *  this
         *  field is reserved.
         */
        uint32_t otgfs_hcdmab6_hcdmab:32;
    };
    uint32_t val;
} usb_otgfs_hcdmab6_reg_t;

/** Type of otgfs_hcdmab7 register
 *  Host Channel $i DMA Buffer Address Register  This register is present only in case
 *  of Scatter/Gather DMA. It is implemented in RAM instead of flop-based
 *  implementation. This register holds the current buffer address.
 */
typedef union {
    struct {
        /** otgfs_hcdmab7_hcdmab : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.
         *  This register is updated as and when the data transfer for the corresponding end
         *  point
         *  is in progress. This register is present only in Scatter/Gather DMA mode. Otherwise
         *  this
         *  field is reserved.
         */
        uint32_t otgfs_hcdmab7_hcdmab:32;
    };
    uint32_t val;
} usb_otgfs_hcdmab7_reg_t;


/** Group: Host Channel 1 Characteristics Register */
/** Type of otgfs_hcchar1 register
 *  Host Channel 1 Characteristics Register
 */
typedef union {
    struct {
        /** otgfs_hcchar1_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  Indicates the maximum packet size of the associated endpoint.
         */
        uint32_t otgfs_hcchar1_mps:11;
        /** otgfs_hcchar1_epnum : R/W; bitpos: [14:11]; default: 0;
         *  Endpoint Number (EPNum)
         *
         *  Indicates the endpoint number on the device serving as the data source or sink.
         */
        uint32_t otgfs_hcchar1_epnum:4;
        /** otgfs_hcchar1_epdir : R/W; bitpos: [15]; default: 0;
         *  Endpoint Direction (EPDir)
         *
         *  Indicates whether the transaction is IN or OUT.
         *  - 1'b0: OUT
         *  - 1'b1: IN
         */
        uint32_t otgfs_hcchar1_epdir:1;
        uint32_t reserved_16:1;
        /** otgfs_hcchar1_lspddev : R/W; bitpos: [17]; default: 0;
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
        uint32_t otgfs_hcchar1_lspddev:1;
        /** otgfs_hcchar1_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Indicates the transfer type selected.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_hcchar1_eptype:2;
        /** otgfs_hcchar1_ec : R/W; bitpos: [21:20]; default: 0;
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
        uint32_t otgfs_hcchar1_ec:2;
        /** otgfs_hcchar1_devaddr : R/W; bitpos: [28:22]; default: 0;
         *  Device Address (DevAddr)
         *
         *  This field selects the specific device serving as the data source
         *  or sink.
         */
        uint32_t otgfs_hcchar1_devaddr:7;
        /** otgfs_hcchar1_oddfrm : R/W; bitpos: [29]; default: 0;
         *  Odd Frame (OddFrm)
         *
         *  This field is set (reset) by the application to indicate that the OTG host must
         *  perform
         *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
         *  (isochronous and interrupt) transactions.
         *  - 1'b0: Even (micro)Frame
         *  - 1'b1: Odd (micro)Frame
         */
        uint32_t otgfs_hcchar1_oddfrm:1;
        /** otgfs_hcchar1_chdis : R/W; bitpos: [30]; default: 0;
         *  Channel Disable (ChDis)
         *
         *  The application sets this bit to stop transmitting/receiving data
         *  on a channel, even before the transfer for that channel is
         *  complete. The application must wait for the Channel Disabled
         *  interrupt before treating the channel as disabled.
         */
        uint32_t otgfs_hcchar1_chdis:1;
        /** otgfs_hcchar1_chena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_hcchar1_chena:1;
    };
    uint32_t val;
} usb_otgfs_hcchar1_reg_t;


/** Group: Host Channel 1 Transfer Size Register */
/** Type of otgfs_hctsiz1 register
 *  Host Channel 1 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_hctsiz1_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_hctsiz1_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_hctsiz1_pktcnt : R/W; bitpos: [25:19]; default: 0;
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
        uint32_t otgfs_hctsiz1_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_hctsiz1_pid : R/W; bitpos: [30:29]; default: 0;
         *  PID (Pid)
         *
         *  The application programs this field with the type of PID to use for the initial
         *  transaction. The host maintains this field for the rest of the transfer.
         *  - 2'b00: DATA0
         *  - 2'b01: DATA2
         *  - 2'b10: DATA1
         *  - 2'b11: MDATA (non-control)/SETUP (control)
         */
        uint32_t otgfs_hctsiz1_pid:2;
        /** otgfs_hctsiz1_dopng : R/W; bitpos: [31]; default: 0;
         *  Do Ping (DoPng)
         *
         *  This bit is used only for OUT transfers.
         *  Setting this field to 1 directs the host to do PING protocol.
         *
         *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
         *  it disables the channel.
         */
        uint32_t otgfs_hctsiz1_dopng:1;
    };
    uint32_t val;
} usb_otgfs_hctsiz1_reg_t;


/** Group: Host Channel 2 Characteristics Register */
/** Type of otgfs_hcchar2 register
 *  Host Channel 2 Characteristics Register
 */
typedef union {
    struct {
        /** otgfs_hcchar2_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  Indicates the maximum packet size of the associated endpoint.
         */
        uint32_t otgfs_hcchar2_mps:11;
        /** otgfs_hcchar2_epnum : R/W; bitpos: [14:11]; default: 0;
         *  Endpoint Number (EPNum)
         *
         *  Indicates the endpoint number on the device serving as the data source or sink.
         */
        uint32_t otgfs_hcchar2_epnum:4;
        /** otgfs_hcchar2_epdir : R/W; bitpos: [15]; default: 0;
         *  Endpoint Direction (EPDir)
         *
         *  Indicates whether the transaction is IN or OUT.
         *  - 1'b0: OUT
         *  - 1'b1: IN
         */
        uint32_t otgfs_hcchar2_epdir:1;
        uint32_t reserved_16:1;
        /** otgfs_hcchar2_lspddev : R/W; bitpos: [17]; default: 0;
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
        uint32_t otgfs_hcchar2_lspddev:1;
        /** otgfs_hcchar2_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Indicates the transfer type selected.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_hcchar2_eptype:2;
        /** otgfs_hcchar2_ec : R/W; bitpos: [21:20]; default: 0;
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
        uint32_t otgfs_hcchar2_ec:2;
        /** otgfs_hcchar2_devaddr : R/W; bitpos: [28:22]; default: 0;
         *  Device Address (DevAddr)
         *
         *  This field selects the specific device serving as the data source
         *  or sink.
         */
        uint32_t otgfs_hcchar2_devaddr:7;
        /** otgfs_hcchar2_oddfrm : R/W; bitpos: [29]; default: 0;
         *  Odd Frame (OddFrm)
         *
         *  This field is set (reset) by the application to indicate that the OTG host must
         *  perform
         *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
         *  (isochronous and interrupt) transactions.
         *  - 1'b0: Even (micro)Frame
         *  - 1'b1: Odd (micro)Frame
         */
        uint32_t otgfs_hcchar2_oddfrm:1;
        /** otgfs_hcchar2_chdis : R/W; bitpos: [30]; default: 0;
         *  Channel Disable (ChDis)
         *
         *  The application sets this bit to stop transmitting/receiving data
         *  on a channel, even before the transfer for that channel is
         *  complete. The application must wait for the Channel Disabled
         *  interrupt before treating the channel as disabled.
         */
        uint32_t otgfs_hcchar2_chdis:1;
        /** otgfs_hcchar2_chena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_hcchar2_chena:1;
    };
    uint32_t val;
} usb_otgfs_hcchar2_reg_t;


/** Group: Host Channel 2 Transfer Size Register */
/** Type of otgfs_hctsiz2 register
 *  Host Channel 2 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_hctsiz2_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_hctsiz2_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_hctsiz2_pktcnt : R/W; bitpos: [25:19]; default: 0;
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
        uint32_t otgfs_hctsiz2_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_hctsiz2_pid : R/W; bitpos: [30:29]; default: 0;
         *  PID (Pid)
         *
         *  The application programs this field with the type of PID to use for the initial
         *  transaction. The host maintains this field for the rest of the transfer.
         *  - 2'b00: DATA0
         *  - 2'b01: DATA2
         *  - 2'b10: DATA1
         *  - 2'b11: MDATA (non-control)/SETUP (control)
         */
        uint32_t otgfs_hctsiz2_pid:2;
        /** otgfs_hctsiz2_dopng : R/W; bitpos: [31]; default: 0;
         *  Do Ping (DoPng)
         *
         *  This bit is used only for OUT transfers.
         *  Setting this field to 1 directs the host to do PING protocol.
         *
         *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
         *  it disables the channel.
         */
        uint32_t otgfs_hctsiz2_dopng:1;
    };
    uint32_t val;
} usb_otgfs_hctsiz2_reg_t;


/** Group: Host Channel 3 Characteristics Register */
/** Type of otgfs_hcchar3 register
 *  Host Channel 3 Characteristics Register
 */
typedef union {
    struct {
        /** otgfs_hcchar3_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  Indicates the maximum packet size of the associated endpoint.
         */
        uint32_t otgfs_hcchar3_mps:11;
        /** otgfs_hcchar3_epnum : R/W; bitpos: [14:11]; default: 0;
         *  Endpoint Number (EPNum)
         *
         *  Indicates the endpoint number on the device serving as the data source or sink.
         */
        uint32_t otgfs_hcchar3_epnum:4;
        /** otgfs_hcchar3_epdir : R/W; bitpos: [15]; default: 0;
         *  Endpoint Direction (EPDir)
         *
         *  Indicates whether the transaction is IN or OUT.
         *  - 1'b0: OUT
         *  - 1'b1: IN
         */
        uint32_t otgfs_hcchar3_epdir:1;
        uint32_t reserved_16:1;
        /** otgfs_hcchar3_lspddev : R/W; bitpos: [17]; default: 0;
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
        uint32_t otgfs_hcchar3_lspddev:1;
        /** otgfs_hcchar3_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Indicates the transfer type selected.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_hcchar3_eptype:2;
        /** otgfs_hcchar3_ec : R/W; bitpos: [21:20]; default: 0;
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
        uint32_t otgfs_hcchar3_ec:2;
        /** otgfs_hcchar3_devaddr : R/W; bitpos: [28:22]; default: 0;
         *  Device Address (DevAddr)
         *
         *  This field selects the specific device serving as the data source
         *  or sink.
         */
        uint32_t otgfs_hcchar3_devaddr:7;
        /** otgfs_hcchar3_oddfrm : R/W; bitpos: [29]; default: 0;
         *  Odd Frame (OddFrm)
         *
         *  This field is set (reset) by the application to indicate that the OTG host must
         *  perform
         *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
         *  (isochronous and interrupt) transactions.
         *  - 1'b0: Even (micro)Frame
         *  - 1'b1: Odd (micro)Frame
         */
        uint32_t otgfs_hcchar3_oddfrm:1;
        /** otgfs_hcchar3_chdis : R/W; bitpos: [30]; default: 0;
         *  Channel Disable (ChDis)
         *
         *  The application sets this bit to stop transmitting/receiving data
         *  on a channel, even before the transfer for that channel is
         *  complete. The application must wait for the Channel Disabled
         *  interrupt before treating the channel as disabled.
         */
        uint32_t otgfs_hcchar3_chdis:1;
        /** otgfs_hcchar3_chena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_hcchar3_chena:1;
    };
    uint32_t val;
} usb_otgfs_hcchar3_reg_t;


/** Group: Host Channel 3 Transfer Size Register */
/** Type of otgfs_hctsiz3 register
 *  Host Channel 3 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_hctsiz3_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_hctsiz3_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_hctsiz3_pktcnt : R/W; bitpos: [25:19]; default: 0;
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
        uint32_t otgfs_hctsiz3_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_hctsiz3_pid : R/W; bitpos: [30:29]; default: 0;
         *  PID (Pid)
         *
         *  The application programs this field with the type of PID to use for the initial
         *  transaction. The host maintains this field for the rest of the transfer.
         *  - 2'b00: DATA0
         *  - 2'b01: DATA2
         *  - 2'b10: DATA1
         *  - 2'b11: MDATA (non-control)/SETUP (control)
         */
        uint32_t otgfs_hctsiz3_pid:2;
        /** otgfs_hctsiz3_dopng : R/W; bitpos: [31]; default: 0;
         *  Do Ping (DoPng)
         *
         *  This bit is used only for OUT transfers.
         *  Setting this field to 1 directs the host to do PING protocol.
         *
         *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
         *  it disables the channel.
         */
        uint32_t otgfs_hctsiz3_dopng:1;
    };
    uint32_t val;
} usb_otgfs_hctsiz3_reg_t;


/** Group: Host Channel 4 Characteristics Register */
/** Type of otgfs_hcchar4 register
 *  Host Channel 4 Characteristics Register
 */
typedef union {
    struct {
        /** otgfs_hcchar4_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  Indicates the maximum packet size of the associated endpoint.
         */
        uint32_t otgfs_hcchar4_mps:11;
        /** otgfs_hcchar4_epnum : R/W; bitpos: [14:11]; default: 0;
         *  Endpoint Number (EPNum)
         *
         *  Indicates the endpoint number on the device serving as the data source or sink.
         */
        uint32_t otgfs_hcchar4_epnum:4;
        /** otgfs_hcchar4_epdir : R/W; bitpos: [15]; default: 0;
         *  Endpoint Direction (EPDir)
         *
         *  Indicates whether the transaction is IN or OUT.
         *  - 1'b0: OUT
         *  - 1'b1: IN
         */
        uint32_t otgfs_hcchar4_epdir:1;
        uint32_t reserved_16:1;
        /** otgfs_hcchar4_lspddev : R/W; bitpos: [17]; default: 0;
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
        uint32_t otgfs_hcchar4_lspddev:1;
        /** otgfs_hcchar4_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Indicates the transfer type selected.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_hcchar4_eptype:2;
        /** otgfs_hcchar4_ec : R/W; bitpos: [21:20]; default: 0;
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
        uint32_t otgfs_hcchar4_ec:2;
        /** otgfs_hcchar4_devaddr : R/W; bitpos: [28:22]; default: 0;
         *  Device Address (DevAddr)
         *
         *  This field selects the specific device serving as the data source
         *  or sink.
         */
        uint32_t otgfs_hcchar4_devaddr:7;
        /** otgfs_hcchar4_oddfrm : R/W; bitpos: [29]; default: 0;
         *  Odd Frame (OddFrm)
         *
         *  This field is set (reset) by the application to indicate that the OTG host must
         *  perform
         *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
         *  (isochronous and interrupt) transactions.
         *  - 1'b0: Even (micro)Frame
         *  - 1'b1: Odd (micro)Frame
         */
        uint32_t otgfs_hcchar4_oddfrm:1;
        /** otgfs_hcchar4_chdis : R/W; bitpos: [30]; default: 0;
         *  Channel Disable (ChDis)
         *
         *  The application sets this bit to stop transmitting/receiving data
         *  on a channel, even before the transfer for that channel is
         *  complete. The application must wait for the Channel Disabled
         *  interrupt before treating the channel as disabled.
         */
        uint32_t otgfs_hcchar4_chdis:1;
        /** otgfs_hcchar4_chena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_hcchar4_chena:1;
    };
    uint32_t val;
} usb_otgfs_hcchar4_reg_t;


/** Group: Host Channel 4 Transfer Size Register */
/** Type of otgfs_hctsiz4 register
 *  Host Channel 4 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_hctsiz4_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_hctsiz4_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_hctsiz4_pktcnt : R/W; bitpos: [25:19]; default: 0;
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
        uint32_t otgfs_hctsiz4_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_hctsiz4_pid : R/W; bitpos: [30:29]; default: 0;
         *  PID (Pid)
         *
         *  The application programs this field with the type of PID to use for the initial
         *  transaction. The host maintains this field for the rest of the transfer.
         *  - 2'b00: DATA0
         *  - 2'b01: DATA2
         *  - 2'b10: DATA1
         *  - 2'b11: MDATA (non-control)/SETUP (control)
         */
        uint32_t otgfs_hctsiz4_pid:2;
        /** otgfs_hctsiz4_dopng : R/W; bitpos: [31]; default: 0;
         *  Do Ping (DoPng)
         *
         *  This bit is used only for OUT transfers.
         *  Setting this field to 1 directs the host to do PING protocol.
         *
         *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
         *  it disables the channel.
         */
        uint32_t otgfs_hctsiz4_dopng:1;
    };
    uint32_t val;
} usb_otgfs_hctsiz4_reg_t;


/** Group: Host Channel 5 Characteristics Register */
/** Type of otgfs_hcchar5 register
 *  Host Channel 5 Characteristics Register
 */
typedef union {
    struct {
        /** otgfs_hcchar5_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  Indicates the maximum packet size of the associated endpoint.
         */
        uint32_t otgfs_hcchar5_mps:11;
        /** otgfs_hcchar5_epnum : R/W; bitpos: [14:11]; default: 0;
         *  Endpoint Number (EPNum)
         *
         *  Indicates the endpoint number on the device serving as the data source or sink.
         */
        uint32_t otgfs_hcchar5_epnum:4;
        /** otgfs_hcchar5_epdir : R/W; bitpos: [15]; default: 0;
         *  Endpoint Direction (EPDir)
         *
         *  Indicates whether the transaction is IN or OUT.
         *  - 1'b0: OUT
         *  - 1'b1: IN
         */
        uint32_t otgfs_hcchar5_epdir:1;
        uint32_t reserved_16:1;
        /** otgfs_hcchar5_lspddev : R/W; bitpos: [17]; default: 0;
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
        uint32_t otgfs_hcchar5_lspddev:1;
        /** otgfs_hcchar5_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Indicates the transfer type selected.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_hcchar5_eptype:2;
        /** otgfs_hcchar5_ec : R/W; bitpos: [21:20]; default: 0;
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
        uint32_t otgfs_hcchar5_ec:2;
        /** otgfs_hcchar5_devaddr : R/W; bitpos: [28:22]; default: 0;
         *  Device Address (DevAddr)
         *
         *  This field selects the specific device serving as the data source
         *  or sink.
         */
        uint32_t otgfs_hcchar5_devaddr:7;
        /** otgfs_hcchar5_oddfrm : R/W; bitpos: [29]; default: 0;
         *  Odd Frame (OddFrm)
         *
         *  This field is set (reset) by the application to indicate that the OTG host must
         *  perform
         *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
         *  (isochronous and interrupt) transactions.
         *  - 1'b0: Even (micro)Frame
         *  - 1'b1: Odd (micro)Frame
         */
        uint32_t otgfs_hcchar5_oddfrm:1;
        /** otgfs_hcchar5_chdis : R/W; bitpos: [30]; default: 0;
         *  Channel Disable (ChDis)
         *
         *  The application sets this bit to stop transmitting/receiving data
         *  on a channel, even before the transfer for that channel is
         *  complete. The application must wait for the Channel Disabled
         *  interrupt before treating the channel as disabled.
         */
        uint32_t otgfs_hcchar5_chdis:1;
        /** otgfs_hcchar5_chena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_hcchar5_chena:1;
    };
    uint32_t val;
} usb_otgfs_hcchar5_reg_t;


/** Group: Host Channel 5 Transfer Size Register */
/** Type of otgfs_hctsiz5 register
 *  Host Channel 5 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_hctsiz5_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_hctsiz5_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_hctsiz5_pktcnt : R/W; bitpos: [25:19]; default: 0;
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
        uint32_t otgfs_hctsiz5_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_hctsiz5_pid : R/W; bitpos: [30:29]; default: 0;
         *  PID (Pid)
         *
         *  The application programs this field with the type of PID to use for the initial
         *  transaction. The host maintains this field for the rest of the transfer.
         *  - 2'b00: DATA0
         *  - 2'b01: DATA2
         *  - 2'b10: DATA1
         *  - 2'b11: MDATA (non-control)/SETUP (control)
         */
        uint32_t otgfs_hctsiz5_pid:2;
        /** otgfs_hctsiz5_dopng : R/W; bitpos: [31]; default: 0;
         *  Do Ping (DoPng)
         *
         *  This bit is used only for OUT transfers.
         *  Setting this field to 1 directs the host to do PING protocol.
         *
         *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
         *  it disables the channel.
         */
        uint32_t otgfs_hctsiz5_dopng:1;
    };
    uint32_t val;
} usb_otgfs_hctsiz5_reg_t;


/** Group: Host Channel 6 Characteristics Register */
/** Type of otgfs_hcchar6 register
 *  Host Channel 6 Characteristics Register
 */
typedef union {
    struct {
        /** otgfs_hcchar6_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  Indicates the maximum packet size of the associated endpoint.
         */
        uint32_t otgfs_hcchar6_mps:11;
        /** otgfs_hcchar6_epnum : R/W; bitpos: [14:11]; default: 0;
         *  Endpoint Number (EPNum)
         *
         *  Indicates the endpoint number on the device serving as the data source or sink.
         */
        uint32_t otgfs_hcchar6_epnum:4;
        /** otgfs_hcchar6_epdir : R/W; bitpos: [15]; default: 0;
         *  Endpoint Direction (EPDir)
         *
         *  Indicates whether the transaction is IN or OUT.
         *  - 1'b0: OUT
         *  - 1'b1: IN
         */
        uint32_t otgfs_hcchar6_epdir:1;
        uint32_t reserved_16:1;
        /** otgfs_hcchar6_lspddev : R/W; bitpos: [17]; default: 0;
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
        uint32_t otgfs_hcchar6_lspddev:1;
        /** otgfs_hcchar6_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Indicates the transfer type selected.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_hcchar6_eptype:2;
        /** otgfs_hcchar6_ec : R/W; bitpos: [21:20]; default: 0;
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
        uint32_t otgfs_hcchar6_ec:2;
        /** otgfs_hcchar6_devaddr : R/W; bitpos: [28:22]; default: 0;
         *  Device Address (DevAddr)
         *
         *  This field selects the specific device serving as the data source
         *  or sink.
         */
        uint32_t otgfs_hcchar6_devaddr:7;
        /** otgfs_hcchar6_oddfrm : R/W; bitpos: [29]; default: 0;
         *  Odd Frame (OddFrm)
         *
         *  This field is set (reset) by the application to indicate that the OTG host must
         *  perform
         *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
         *  (isochronous and interrupt) transactions.
         *  - 1'b0: Even (micro)Frame
         *  - 1'b1: Odd (micro)Frame
         */
        uint32_t otgfs_hcchar6_oddfrm:1;
        /** otgfs_hcchar6_chdis : R/W; bitpos: [30]; default: 0;
         *  Channel Disable (ChDis)
         *
         *  The application sets this bit to stop transmitting/receiving data
         *  on a channel, even before the transfer for that channel is
         *  complete. The application must wait for the Channel Disabled
         *  interrupt before treating the channel as disabled.
         */
        uint32_t otgfs_hcchar6_chdis:1;
        /** otgfs_hcchar6_chena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_hcchar6_chena:1;
    };
    uint32_t val;
} usb_otgfs_hcchar6_reg_t;


/** Group: Host Channel 6 Transfer Size Register */
/** Type of otgfs_hctsiz6 register
 *  Host Channel 6 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_hctsiz6_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_hctsiz6_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_hctsiz6_pktcnt : R/W; bitpos: [25:19]; default: 0;
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
        uint32_t otgfs_hctsiz6_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_hctsiz6_pid : R/W; bitpos: [30:29]; default: 0;
         *  PID (Pid)
         *
         *  The application programs this field with the type of PID to use for the initial
         *  transaction. The host maintains this field for the rest of the transfer.
         *  - 2'b00: DATA0
         *  - 2'b01: DATA2
         *  - 2'b10: DATA1
         *  - 2'b11: MDATA (non-control)/SETUP (control)
         */
        uint32_t otgfs_hctsiz6_pid:2;
        /** otgfs_hctsiz6_dopng : R/W; bitpos: [31]; default: 0;
         *  Do Ping (DoPng)
         *
         *  This bit is used only for OUT transfers.
         *  Setting this field to 1 directs the host to do PING protocol.
         *
         *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
         *  it disables the channel.
         */
        uint32_t otgfs_hctsiz6_dopng:1;
    };
    uint32_t val;
} usb_otgfs_hctsiz6_reg_t;


/** Group: Host Channel 7 Characteristics Register */
/** Type of otgfs_hcchar7 register
 *  Host Channel 7 Characteristics Register
 */
typedef union {
    struct {
        /** otgfs_hcchar7_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  Indicates the maximum packet size of the associated endpoint.
         */
        uint32_t otgfs_hcchar7_mps:11;
        /** otgfs_hcchar7_epnum : R/W; bitpos: [14:11]; default: 0;
         *  Endpoint Number (EPNum)
         *
         *  Indicates the endpoint number on the device serving as the data source or sink.
         */
        uint32_t otgfs_hcchar7_epnum:4;
        /** otgfs_hcchar7_epdir : R/W; bitpos: [15]; default: 0;
         *  Endpoint Direction (EPDir)
         *
         *  Indicates whether the transaction is IN or OUT.
         *  - 1'b0: OUT
         *  - 1'b1: IN
         */
        uint32_t otgfs_hcchar7_epdir:1;
        uint32_t reserved_16:1;
        /** otgfs_hcchar7_lspddev : R/W; bitpos: [17]; default: 0;
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
        uint32_t otgfs_hcchar7_lspddev:1;
        /** otgfs_hcchar7_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Indicates the transfer type selected.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_hcchar7_eptype:2;
        /** otgfs_hcchar7_ec : R/W; bitpos: [21:20]; default: 0;
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
        uint32_t otgfs_hcchar7_ec:2;
        /** otgfs_hcchar7_devaddr : R/W; bitpos: [28:22]; default: 0;
         *  Device Address (DevAddr)
         *
         *  This field selects the specific device serving as the data source
         *  or sink.
         */
        uint32_t otgfs_hcchar7_devaddr:7;
        /** otgfs_hcchar7_oddfrm : R/W; bitpos: [29]; default: 0;
         *  Odd Frame (OddFrm)
         *
         *  This field is set (reset) by the application to indicate that the OTG host must
         *  perform
         *  a transfer in an odd (micro)Frame. This field is applicable for only periodic
         *  (isochronous and interrupt) transactions.
         *  - 1'b0: Even (micro)Frame
         *  - 1'b1: Odd (micro)Frame
         */
        uint32_t otgfs_hcchar7_oddfrm:1;
        /** otgfs_hcchar7_chdis : R/W; bitpos: [30]; default: 0;
         *  Channel Disable (ChDis)
         *
         *  The application sets this bit to stop transmitting/receiving data
         *  on a channel, even before the transfer for that channel is
         *  complete. The application must wait for the Channel Disabled
         *  interrupt before treating the channel as disabled.
         */
        uint32_t otgfs_hcchar7_chdis:1;
        /** otgfs_hcchar7_chena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_hcchar7_chena:1;
    };
    uint32_t val;
} usb_otgfs_hcchar7_reg_t;


/** Group: Host Channel 7 Transfer Size Register */
/** Type of otgfs_hctsiz7 register
 *  Host Channel 7 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_hctsiz7_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_hctsiz7_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_hctsiz7_pktcnt : R/W; bitpos: [25:19]; default: 0;
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
        uint32_t otgfs_hctsiz7_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_hctsiz7_pid : R/W; bitpos: [30:29]; default: 0;
         *  PID (Pid)
         *
         *  The application programs this field with the type of PID to use for the initial
         *  transaction. The host maintains this field for the rest of the transfer.
         *  - 2'b00: DATA0
         *  - 2'b01: DATA2
         *  - 2'b10: DATA1
         *  - 2'b11: MDATA (non-control)/SETUP (control)
         */
        uint32_t otgfs_hctsiz7_pid:2;
        /** otgfs_hctsiz7_dopng : R/W; bitpos: [31]; default: 0;
         *  Do Ping (DoPng)
         *
         *  This bit is used only for OUT transfers.
         *  Setting this field to 1 directs the host to do PING protocol.
         *
         *  Note: Do not set this bit for IN transfers. If this bit is set for for IN transfers
         *  it disables the channel.
         */
        uint32_t otgfs_hctsiz7_dopng:1;
    };
    uint32_t val;
} usb_otgfs_hctsiz7_reg_t;


/** Group: Device Configuration Register */
/** Type of otgfs_dcfg register
 *  Device Configuration Register  This register configures the core in Device mode
 *  after power-on or after certain control commands or enumeration. Do not make
 *  changes to this register after initial programming.
 */
typedef union {
    struct {
        /** otgfs_dcfg_devspd : R/W; bitpos: [1:0]; default: 0;
         *  Device Speed (DevSpd)
         *
         *  Indicates the speed at which the application requires the core to
         *  enumerate, or the maximum speed the application can support.
         *  However, the actual bus speed is determined only after the connect
         *  sequence is completed, and is based on the speed of the USB
         *  host to which the core is connected.
         */
        uint32_t otgfs_dcfg_devspd:2;
        /** otgfs_dcfg_nzstsouthshk : R/W; bitpos: [2]; default: 0;
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
        uint32_t otgfs_dcfg_nzstsouthshk:1;
        /** otgfs_dcfg_ena32khzsusp : R/W; bitpos: [3]; default: 0;
         *  Enable 32 KHz Suspend mode (Ena32KHzSusp)
         *
         *  This bit can be set only if FS PHY interface is selected. Otherwise, this bit needs
         *  to be set to zero. If FS PHY interface is chosen and this bit is set, the PHY clock
         *  during Suspend must be switched from 48 MHz to 32 KHz.
         */
        uint32_t otgfs_dcfg_ena32khzsusp:1;
        /** otgfs_dcfg_devaddr : R/W; bitpos: [10:4]; default: 0;
         *  Device Address (DevAddr)
         *
         *  The application must program this field after every SetAddress control command.
         */
        uint32_t otgfs_dcfg_devaddr:7;
        /** otgfs_dcfg_perfrint : R/W; bitpos: [12:11]; default: 0;
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
        uint32_t otgfs_dcfg_perfrint:2;
        /** otgfs_dcfg_endevoutnak : R/W; bitpos: [13]; default: 0;
         *  Enable Device OUT NAK (EnDevOutNak)
         *
         *  This bit enables setting NAK for Bulk OUT endpoints after the transfer is completed
         *  for Device mode Descriptor DMA
         *  - 1'b0 : The core does not set NAK after Bulk OUT transfer complete
         *  - 1'b1  :  The core sets NAK after Bulk OUT transfer complete
         *  It bit is one time programmable after reset like any other DCFG register bits.
         */
        uint32_t otgfs_dcfg_endevoutnak:1;
        /** otgfs_dcfg_xcvrdly : R/W; bitpos: [14]; default: 0;
         *  XCVRDLY
         *
         *  Enables or disables delay between xcvr_sel and txvalid during device chirp
         */
        uint32_t otgfs_dcfg_xcvrdly:1;
        /** otgfs_dcfg_erraticintmsk : R/W; bitpos: [15]; default: 0;
         *  Erratic Error Interrupt Mask
         */
        uint32_t otgfs_dcfg_erraticintmsk:1;
        uint32_t reserved_16:7;
        /** otgfs_dcfg_descdma : R/W; bitpos: [23]; default: 0;
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
        uint32_t otgfs_dcfg_descdma:1;
        /** otgfs_dcfg_perschintvl : R/W; bitpos: [25:24]; default: 0;
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
        uint32_t otgfs_dcfg_perschintvl:2;
        /** otgfs_dcfg_resvalid : R/W; bitpos: [31:26]; default: 2;
         *  Resume Validation Period (ResValid)
         *
         *  This field is effective only when DCFG.Ena32KHzSusp is set.
         *  It controls the resume period when the core resumes from
         *  suspend. The core counts for ResValid number of clock cycles
         *  to detect a valid resume when this bit is set
         */
        uint32_t otgfs_dcfg_resvalid:6;
    };
    uint32_t val;
} usb_otgfs_dcfg_reg_t;


/** Group: Device Control Register */
/** Type of otgfs_dctl register
 *  Device Control Register
 */
typedef union {
    struct {
        /** otgfs_dctl_rmtwkupsig : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_dctl_rmtwkupsig:1;
        /** otgfs_dctl_sftdiscon : R/W; bitpos: [1]; default: 1;
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
        uint32_t otgfs_dctl_sftdiscon:1;
        /** otgfs_dctl_gnpinnaksts : RO; bitpos: [2]; default: 0;
         *  Global Non-periodic IN NAK Status (GNPINNakSts)
         *  - 1'b0: A handshake is sent out based on the data availability in the transmit FIFO.
         *  - 1'b1: A NAK handshake is sent out on all non-periodic IN endpoints, irrespective
         *  of the data availability in the transmit FIFO.
         */
        uint32_t otgfs_dctl_gnpinnaksts:1;
        /** otgfs_dctl_goutnaksts : RO; bitpos: [3]; default: 0;
         *  Global OUT NAK Status (GOUTNakSts)
         *  - 1'b0: A handshake is sent based on the FIFO Status and the NAK and STALL bit
         *  settings.
         *  - 1'b1: No data is written to the RxFIFO, irrespective of space availability. Sends
         *  a NAK handshake on all packets, except on SETUP transactions. All isochronous OUT
         *  packets are dropped.
         */
        uint32_t otgfs_dctl_goutnaksts:1;
        /** otgfs_dctl_tstctl : R/W; bitpos: [6:4]; default: 0;
         *  Test Control (TstCtl)
         *  - 3'b000: Test mode disabled
         *  - 3'b001: Test_J mode
         *  - 3'b010: Test_K mode
         *  - 3'b011: Test_SE0_NAK mode
         *  - 3'b100: Test_Packet mode
         *  - 3'b101: Test_Force_Enable
         *  - Others: Reserved
         */
        uint32_t otgfs_dctl_tstctl:3;
        /** otgfs_dctl_sgnpinnak : R/W; bitpos: [7]; default: 0;
         *  Set Global Non-periodic IN NAK (SGNPInNak)
         *
         *  A write to this field sets the Global Non-periodic IN NAK.The application uses this
         *  bit to send a NAK handshake on all non-periodic IN endpoints.
         *  The core can also Set this bit when a timeout condition is detected on a
         *  non-periodic endpoint in shared FIFO operation.
         *  The application must Set this bit only after making sure that the Global IN NAK
         *  Effective bit in the Core Interrupt Register (GINTSTS.GINNakEff) is cleared
         */
        uint32_t otgfs_dctl_sgnpinnak:1;
        /** otgfs_dctl_cgnpinnak : R/W; bitpos: [8]; default: 0;
         *  Clear Global Non-periodic IN NAK (CGNPInNak)
         *
         *  A write to this field clears the Global Non-periodic IN NAK.
         */
        uint32_t otgfs_dctl_cgnpinnak:1;
        /** otgfs_dctl_sgoutnak : R/W; bitpos: [9]; default: 0;
         *  Set Global OUT NAK (SGOUTNak)
         *
         *  A write to this field sets the Global OUT NAK. The application uses this bit to
         *  send a NAK handshake on all OUT endpoints.
         *  The application must set the this bit only after making sure that the Global OUT
         *  NAK Effective bit in the Core Interrupt Register (GINTSTS.GOUTNakEff) is cleared.
         */
        uint32_t otgfs_dctl_sgoutnak:1;
        /** otgfs_dctl_cgoutnak : R/W; bitpos: [10]; default: 0;
         *  Clear Global OUT NAK (CGOUTNak)
         *
         *  A write to this field clears the Global OUT NAK.
         */
        uint32_t otgfs_dctl_cgoutnak:1;
        /** otgfs_dctl_pwronprgdone : R/W; bitpos: [11]; default: 0;
         *  Power-On Programming Done (PWROnPrgDone)
         *
         *  The application uses this bit to indicate that register programming is completed
         *  after a wake-up from Power Down mode.
         */
        uint32_t otgfs_dctl_pwronprgdone:1;
        uint32_t reserved_12:1;
        /** otgfs_dctl_gmc : R/W; bitpos: [14:13]; default: 0;
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
        uint32_t otgfs_dctl_gmc:2;
        /** otgfs_dctl_ignrfrmnum : R/W; bitpos: [15]; default: 0;
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
        uint32_t otgfs_dctl_ignrfrmnum:1;
        /** otgfs_dctl_nakonbble : R/W; bitpos: [16]; default: 0;
         *  NAK on Babble Error (NakOnBble)
         *
         *  Set NAK automatically on babble (NakOnBble). The core sets NAK automatically for
         *  the endpoint on which babble is received.
         */
        uint32_t otgfs_dctl_nakonbble:1;
        /** otgfs_dctl_encontonbna : R/W; bitpos: [17]; default: 0;
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
        uint32_t otgfs_dctl_encontonbna:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} usb_otgfs_dctl_reg_t;


/** Group: Device Status Register */
/** Type of otgfs_dsts register
 *  Device Status Register  This register indicates the status of the core with respect
 *  to USB-related events. It must be read on interrupts from Device All Interrupts
 *  (DAINT) register.
 */
typedef union {
    struct {
        /** otgfs_dsts_suspsts : RO; bitpos: [0]; default: 0;
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
        uint32_t otgfs_dsts_suspsts:1;
        /** otgfs_dsts_enumspd : RO; bitpos: [2:1]; default: 1;
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
        uint32_t otgfs_dsts_enumspd:2;
        /** otgfs_dsts_errticerr : RO; bitpos: [3]; default: 0;
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
        uint32_t otgfs_dsts_errticerr:1;
        uint32_t reserved_4:4;
        /** otgfs_dsts_soffn : RO; bitpos: [21:8]; default: 0;
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
        uint32_t otgfs_dsts_soffn:14;
        /** otgfs_dsts_devlnsts : RO; bitpos: [23:22]; default: 0;
         *  Device Line Status (DevLnSts)
         *
         *  Indicates the current logic level USB data lines
         *  - DevLnSts[1]: Logic level of D+
         *  - DevLnSts[0]: Logic level of D-
         */
        uint32_t otgfs_dsts_devlnsts:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} usb_otgfs_dsts_reg_t;


/** Group: Device IN Endpoint Common Interrupt Mask Register */
/** Type of otgfs_diepmsk register
 *  Device IN Endpoint Common Interrupt Mask Register  This register works with each of
 *  the Device IN Endpoint Interrupt (DIEPINTn) registers for all endpoints to generate
 *  an interrupt per IN endpoint. The IN endpoint interrupt for a specific status in
 *  the DIEPINTn register can be masked by writing to the corresponding bit in this
 *  register. Status bits are masked by default.
 */
typedef union {
    struct {
        /** otgfs_diepmsk_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed Interrupt Mask (XferComplMsk)
         */
        uint32_t otgfs_diepmsk_xfercomplmsk:1;
        /** otgfs_diepmsk_epdisbldmsk : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt Mask (EPDisbldMsk)
         */
        uint32_t otgfs_diepmsk_epdisbldmsk:1;
        /** otgfs_diepmsk_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *  AHB Error Mask (AHBErrMsk)
         */
        uint32_t otgfs_diepmsk_ahberrmsk:1;
        /** otgfs_diepmsk_timeoutmsk : R/W; bitpos: [3]; default: 0;
         *  Timeout Condition Mask (TimeOUTMsk) (Non-isochronous endpoints)
         */
        uint32_t otgfs_diepmsk_timeoutmsk:1;
        /** otgfs_diepmsk_intkntxfempmsk : R/W; bitpos: [4]; default: 0;
         *  IN Token Received When TxFIFO Empty Mask (INTknTXFEmpMsk)
         */
        uint32_t otgfs_diepmsk_intkntxfempmsk:1;
        /** otgfs_diepmsk_intknepmismsk : R/W; bitpos: [5]; default: 0;
         *  IN Token received with EP Mismatch Mask (INTknEPMisMsk)
         */
        uint32_t otgfs_diepmsk_intknepmismsk:1;
        /** otgfs_diepmsk_inepnakeffmsk : R/W; bitpos: [6]; default: 0;
         *  IN Endpoint NAK Effective Mask (INEPNakEffMsk)
         */
        uint32_t otgfs_diepmsk_inepnakeffmsk:1;
        uint32_t reserved_7:1;
        /** otgfs_diepmsk_txfifoundrnmsk : R/W; bitpos: [8]; default: 0;
         *  Fifo Underrun Mask (TxfifoUndrnMsk)
         */
        uint32_t otgfs_diepmsk_txfifoundrnmsk:1;
        /** otgfs_diepmsk_bnainintrmsk : R/W; bitpos: [9]; default: 0;
         *  BNA interrupt Mask (BNAInIntrMsk)
         */
        uint32_t otgfs_diepmsk_bnainintrmsk:1;
        uint32_t reserved_10:3;
        /** otgfs_diepmsk_nakmsk : R/W; bitpos: [13]; default: 0;
         *  NAK interrupt Mask (NAKMsk)
         */
        uint32_t otgfs_diepmsk_nakmsk:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} usb_otgfs_diepmsk_reg_t;


/** Group: Device OUT Endpoint Common Interrupt Mask Register */
/** Type of otgfs_doepmsk register
 *  Device OUT Endpoint Common Interrupt Mask Register  This register works with each
 *  of the Device OUT Endpoint Interrupt (DOEPINTn) registers for all endpoints to
 *  generate an interrupt per OUT endpoint. The OUT endpoint interrupt for a specific
 *  status in the DOEPINTn register can be masked by writing into the corresponding bit
 *  in this register. Status bits are masked by default.
 */
typedef union {
    struct {
        /** otgfs_doepmsk_xfercomplmsk : R/W; bitpos: [0]; default: 0;
         *  Transfer Completed Interrupt Mask (XferComplMsk)
         */
        uint32_t otgfs_doepmsk_xfercomplmsk:1;
        /** otgfs_doepmsk_epdisbldmsk : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt Mask (EPDisbldMsk)
         */
        uint32_t otgfs_doepmsk_epdisbldmsk:1;
        /** otgfs_doepmsk_ahberrmsk : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErrMsk)
         */
        uint32_t otgfs_doepmsk_ahberrmsk:1;
        /** otgfs_doepmsk_setupmsk : R/W; bitpos: [3]; default: 0;
         *  SETUP Phase Done Mask (SetUPMsk)
         *
         *  Applies to control endpoints only.
         */
        uint32_t otgfs_doepmsk_setupmsk:1;
        /** otgfs_doepmsk_outtknepdismsk : R/W; bitpos: [4]; default: 0;
         *  OUT Token Received when Endpoint Disabled Mask (OUTTknEPdisMsk)
         *
         *  Applies to control OUT endpoints only.
         */
        uint32_t otgfs_doepmsk_outtknepdismsk:1;
        /** otgfs_doepmsk_stsphsercvdmsk : R/W; bitpos: [5]; default: 0;
         *  Status Phase Received Mask (StsPhseRcvdMsk)
         *
         *  Applies to control OUT endpoints only.
         */
        uint32_t otgfs_doepmsk_stsphsercvdmsk:1;
        /** otgfs_doepmsk_back2backsetup : R/W; bitpos: [6]; default: 0;
         *  Back-to-Back SETUP Packets Received Mask (Back2BackSETup)
         *
         *  Applies to control OUT endpoints only.
         */
        uint32_t otgfs_doepmsk_back2backsetup:1;
        uint32_t reserved_7:1;
        /** otgfs_doepmsk_outpkterrmsk : R/W; bitpos: [8]; default: 0;
         *  OUT Packet Error Mask (OutPktErrMsk)
         */
        uint32_t otgfs_doepmsk_outpkterrmsk:1;
        /** otgfs_doepmsk_bnaoutintrmsk : R/W; bitpos: [9]; default: 0;
         *  BNA interrupt Mask (BnaOutIntrMsk)
         */
        uint32_t otgfs_doepmsk_bnaoutintrmsk:1;
        uint32_t reserved_10:2;
        /** otgfs_doepmsk_bbleerrmsk : R/W; bitpos: [12]; default: 0;
         *  Babble Error interrupt Mask (BbleErrMsk)
         */
        uint32_t otgfs_doepmsk_bbleerrmsk:1;
        /** otgfs_doepmsk_nakmsk : R/W; bitpos: [13]; default: 0;
         *  NAK interrupt Mask (NAKMsk)
         */
        uint32_t otgfs_doepmsk_nakmsk:1;
        /** otgfs_doepmsk_nyetmsk : R/W; bitpos: [14]; default: 0;
         *  NYET interrupt Mask (NYETMsk)
         */
        uint32_t otgfs_doepmsk_nyetmsk:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} usb_otgfs_doepmsk_reg_t;


/** Group: Device All Endpoints Interrupt Register */
/** Type of otgfs_daint register
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
typedef union {
    struct {
        /** otgfs_daint_inepint0 : RO; bitpos: [0]; default: 0;
         *  IN Endpoint 0 Interrupt Bit
         */
        uint32_t otgfs_daint_inepint0:1;
        /** otgfs_daint_inepint1 : RO; bitpos: [1]; default: 0;
         *  IN Endpoint 1 Interrupt Bit
         */
        uint32_t otgfs_daint_inepint1:1;
        /** otgfs_daint_inepint2 : RO; bitpos: [2]; default: 0;
         *  IN Endpoint 2 Interrupt Bit
         */
        uint32_t otgfs_daint_inepint2:1;
        /** otgfs_daint_inepint3 : RO; bitpos: [3]; default: 0;
         *  IN Endpoint 3 Interrupt Bit
         */
        uint32_t otgfs_daint_inepint3:1;
        /** otgfs_daint_inepint4 : RO; bitpos: [4]; default: 0;
         *  IN Endpoint 4 Interrupt Bit
         */
        uint32_t otgfs_daint_inepint4:1;
        /** otgfs_daint_inepint5 : RO; bitpos: [5]; default: 0;
         *  IN Endpoint 5 Interrupt Bit
         */
        uint32_t otgfs_daint_inepint5:1;
        /** otgfs_daint_inepint6 : RO; bitpos: [6]; default: 0;
         *  IN Endpoint 6 Interrupt Bit
         */
        uint32_t otgfs_daint_inepint6:1;
        uint32_t reserved_7:9;
        /** otgfs_daint_outepint0 : RO; bitpos: [16]; default: 0;
         *  OUT Endpoint 0 Interrupt Bit
         */
        uint32_t otgfs_daint_outepint0:1;
        /** otgfs_daint_outepint1 : RO; bitpos: [17]; default: 0;
         *  OUT Endpoint 1 Interrupt Bit
         */
        uint32_t otgfs_daint_outepint1:1;
        /** otgfs_daint_outepint2 : RO; bitpos: [18]; default: 0;
         *  OUT Endpoint 2 Interrupt Bit
         */
        uint32_t otgfs_daint_outepint2:1;
        /** otgfs_daint_outepint3 : RO; bitpos: [19]; default: 0;
         *  OUT Endpoint 3 Interrupt Bit
         */
        uint32_t otgfs_daint_outepint3:1;
        /** otgfs_daint_outepint4 : RO; bitpos: [20]; default: 0;
         *  OUT Endpoint 4 Interrupt Bit
         */
        uint32_t otgfs_daint_outepint4:1;
        /** otgfs_daint_outepint5 : RO; bitpos: [21]; default: 0;
         *  OUT Endpoint 5 Interrupt Bit
         */
        uint32_t otgfs_daint_outepint5:1;
        /** otgfs_daint_outepint6 : RO; bitpos: [22]; default: 0;
         *  OUT Endpoint 6 Interrupt Bit
         */
        uint32_t otgfs_daint_outepint6:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} usb_otgfs_daint_reg_t;


/** Group: Device All Endpoints Interrupt Mask Register */
/** Type of otgfs_daintmsk register
 *  Device All Endpoints Interrupt Mask Register    The Device Endpoint Interrupt Mask
 *  register works with the Device Endpoint Interrupt register to interrupt the
 *  application when an event occurs on a device endpoint. However, the Device All
 *  Endpoints Interrupt (DAINT) register bit corresponding to that interrupt is still
 *  set.
 */
typedef union {
    struct {
        /** otgfs_daintmsk_inepmsk0 : R/W; bitpos: [0]; default: 0;
         *  IN Endpoint 0 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_inepmsk0:1;
        /** otgfs_daintmsk_inepmsk1 : R/W; bitpos: [1]; default: 0;
         *  IN Endpoint 1 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_inepmsk1:1;
        /** otgfs_daintmsk_inepmsk2 : R/W; bitpos: [2]; default: 0;
         *  IN Endpoint 2 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_inepmsk2:1;
        /** otgfs_daintmsk_inepmsk3 : R/W; bitpos: [3]; default: 0;
         *  IN Endpoint 3 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_inepmsk3:1;
        /** otgfs_daintmsk_inepmsk4 : R/W; bitpos: [4]; default: 0;
         *  IN Endpoint 4 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_inepmsk4:1;
        /** otgfs_daintmsk_inepmsk5 : R/W; bitpos: [5]; default: 0;
         *  IN Endpoint 5 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_inepmsk5:1;
        /** otgfs_daintmsk_inepmsk6 : R/W; bitpos: [6]; default: 0;
         *  IN Endpoint 6 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_inepmsk6:1;
        uint32_t reserved_7:9;
        /** otgfs_daintmsk_outepmsk0 : R/W; bitpos: [16]; default: 0;
         *  OUT Endpoint 0 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_outepmsk0:1;
        /** otgfs_daintmsk_outepmsk1 : R/W; bitpos: [17]; default: 0;
         *  OUT Endpoint 1 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_outepmsk1:1;
        /** otgfs_daintmsk_outepmsk2 : R/W; bitpos: [18]; default: 0;
         *  OUT Endpoint 2 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_outepmsk2:1;
        /** otgfs_daintmsk_outepmsk3 : R/W; bitpos: [19]; default: 0;
         *  OUT Endpoint 3 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_outepmsk3:1;
        /** otgfs_daintmsk_outepmsk4 : R/W; bitpos: [20]; default: 0;
         *  OUT Endpoint 4 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_outepmsk4:1;
        /** otgfs_daintmsk_outepmsk5 : R/W; bitpos: [21]; default: 0;
         *  OUT Endpoint 5 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_outepmsk5:1;
        /** otgfs_daintmsk_outepmsk6 : R/W; bitpos: [22]; default: 0;
         *  OUT Endpoint 6 Interrupt mask Bit
         */
        uint32_t otgfs_daintmsk_outepmsk6:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} usb_otgfs_daintmsk_reg_t;


/** Group: Device VBUS Discharge Time Register */
/** Type of otgfs_dvbusdis register
 *  Device VBUS Discharge Time Register   This register specifies the VBUS discharge
 *  time after VBUS pulsing during SRP.
 */
typedef union {
    struct {
        /** otgfs_dvbusdis_dvbusdis : R/W; bitpos: [15:0]; default: 6103;
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
        uint32_t otgfs_dvbusdis_dvbusdis:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_dvbusdis_reg_t;


/** Group: Device VBUS Pulsing Time Register */
/** Type of otgfs_dvbuspulse register
 *  Device VBUS Pulsing Time Register
 */
typedef union {
    struct {
        /** otgfs_dvbuspulse_dvbuspulse : R/W; bitpos: [11:0]; default: 1464;
         *  Device VBUS Pulsing Time (DVBUSPulse)
         *
         *  Specifies the VBUS pulsing time during SRP. This value equals (VBUS pulsing time in
         *  PHY clocks) / 1, 024
         *
         *  The value you use depends whether the PHY is operating at 30MHz (16-bit data width)
         *  or 60 MHz (8-bit data width).
         */
        uint32_t otgfs_dvbuspulse_dvbuspulse:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} usb_otgfs_dvbuspulse_reg_t;


/** Group: Device Threshold Control Register */
/** Type of otgfs_dthrctl register
 *  Device Threshold Control Register
 */
typedef union {
    struct {
        /** otgfs_dthrctl_nonisothren : R/W; bitpos: [0]; default: 0;
         *  Non-ISO IN Endpoints Threshold Enable. (NonISOThrEn)
         *
         *  When this bit is Set, the core enables thresholding for Non Isochronous IN
         *  endpoints.
         */
        uint32_t otgfs_dthrctl_nonisothren:1;
        /** otgfs_dthrctl_isothren : R/W; bitpos: [1]; default: 0;
         *
         *  ISO IN Endpoints Threshold Enable. (ISOThrEn)
         *
         *  When this bit is Set, the core enables thresholding for isochronous IN
         *  endpoints.
         */
        uint32_t otgfs_dthrctl_isothren:1;
        /** otgfs_dthrctl_txthrlen : R/W; bitpos: [10:2]; default: 8;
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
        uint32_t otgfs_dthrctl_txthrlen:9;
        /** otgfs_dthrctl_ahbthrratio : R/W; bitpos: [12:11]; default: 0;
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
        uint32_t otgfs_dthrctl_ahbthrratio:2;
        uint32_t reserved_13:3;
        /** otgfs_dthrctl_rxthren : R/W; bitpos: [16]; default: 0;
         *  Receive Threshold Enable (RxThrEn)
         *
         *  When this bit is set, the core enables thresholding in the receive direction.
         *
         *  Note: We recommends that you do not enable RxThrEn, because it may cause issues in
         *  the RxFIFO especially during error conditions such as RxError and Babble.
         */
        uint32_t otgfs_dthrctl_rxthren:1;
        /** otgfs_dthrctl_rxthrlen : R/W; bitpos: [25:17]; default: 8;
         *  Receive Threshold Length (RxThrLen)
         *
         *  This field specifies Receive thresholding size in DWORDS.
         *  This field also specifies the amount of data received on the USB before the core
         *  can start transmitting on the AHB.
         *  The threshold length has to be at least eight DWORDS.
         *  The recommended value for ThrLen is to be the same as the programmed
         *  AHB Burst Length (GAHBCFG.HBstLen).
         */
        uint32_t otgfs_dthrctl_rxthrlen:9;
        uint32_t reserved_26:1;
        /** otgfs_dthrctl_arbprken : R/W; bitpos: [27]; default: 1;
         *  Arbiter Parking Enable (ArbPrkEn)
         *
         *  This bit controls internal DMA arbiter parking for IN endpoints. If thresholding is
         *  enabled and this bit is set to one, then the arbiter parks on the IN endpoint for
         *  which there is a token received on the USB. This is done to avoid getting into
         *  underrun conditions. By default, arbiter parking is enabled.
         */
        uint32_t otgfs_dthrctl_arbprken:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} usb_otgfs_dthrctl_reg_t;


/** Group: Device IN Endpoint FIFO Empty Interrupt Mask Register */
/** Type of otgfs_diepempmsk register
 *  Device IN Endpoint FIFO Empty Interrupt Mask Register This register is valid only
 *  in Dedicated FIFO operation (OTG_EN_DED_TX_FIFO = 1). This register is used to
 *  control the IN endpoint FIFO empty interrupt generation (DIEPINTn.TxfEmp).
 */
typedef union {
    struct {
        /** otgfs_diepempmsk_ineptxfempmsk : R/W; bitpos: [15:0]; default: 0;
         *  IN EP Tx FIFO Empty Interrupt Mask Bits (InEpTxfEmpMsk)
         *
         *  These bits acts as mask bits for DIEPINTn.TxFEmp interrupt, one bit per IN Endpoint:
         *
         *  Bit 0 for IN EP 0, bit 15 for IN EP 15
         */
        uint32_t otgfs_diepempmsk_ineptxfempmsk:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_diepempmsk_reg_t;


/** Group: Device Control IN Endpoint 0 Control Register */
/** Type of otgfs_diepctl0 register
 *  Device Control IN Endpoint 0 Control Register
 */
typedef union {
    struct {
        /** otgfs_diepctl0_mps : R/W; bitpos: [1:0]; default: 0;
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
        uint32_t otgfs_diepctl0_mps:2;
        uint32_t reserved_2:13;
        /** otgfs_diepctl0_usbactep : RO; bitpos: [15]; default: 1;
         *  USB Active Endpoint (USBActEP)
         *
         *  This bit is always SET to 1, indicating that control endpoint 0 is always active in
         *  all configurations and interfaces.
         */
        uint32_t otgfs_diepctl0_usbactep:1;
        uint32_t reserved_16:1;
        /** otgfs_diepctl0_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_diepctl0_naksts:1;
        /** otgfs_diepctl0_eptype : RO; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Hardcoded to 00 for control.
         */
        uint32_t otgfs_diepctl0_eptype:2;
        uint32_t reserved_20:1;
        /** otgfs_diepctl0_stall : R/W; bitpos: [21]; default: 0;
         *  STALL Handshake (Stall)
         *
         *  The application can only set this bit, and the core clears it, when a
         *  SETUP token is received for this endpoint. If a NAK bit, Global Nonperiodic
         *  IN NAK, or Global OUT NAK is set along with this bit, the STALL
         *  bit takes priority.
         */
        uint32_t otgfs_diepctl0_stall:1;
        /** otgfs_diepctl0_txfnum : R/W; bitpos: [25:22]; default: 0;
         *  TxFIFO Number (TxFNum)
         *  - For Shared FIFO operation, this value is always set to 0, indicating that control
         *  IN endpoint 0 data is always written in the Non-Periodic Transmit FIFO.
         *  - For Dedicated FIFO operation, this value is set to the FIFO number that is
         *  assigned to IN Endpoint.
         */
        uint32_t otgfs_diepctl0_txfnum:4;
        /** otgfs_diepctl0_cnak : R/W; bitpos: [26]; default: 0;
         *
         *  Clear NAK (CNAK)
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_diepctl0_cnak:1;
        /** otgfs_diepctl0_snak : R/W; bitpos: [27]; default: 0;
         *
         *  Set NAK (SNAK)
         *  A write to this bit sets the NAK bit for the endpoint.
         *  Using this bit, the application can control the transmission of NAK
         *  handshakes on an endpoint. The core can also set this bit for an
         *  endpoint after a SETUP packet is received on that endpoint.
         */
        uint32_t otgfs_diepctl0_snak:1;
        uint32_t reserved_28:2;
        /** otgfs_diepctl0_epdis : R/W; bitpos: [30]; default: 0;
         *  Endpoint Disable (EPDis)
         *
         *  The application sets this bit to stop transmitting data on an endpoint,
         *  even before the transfer for that endpoint is complete. The application
         *  must wait for the Endpoint Disabled interrupt before treating the endpoint
         *  as disabled. The core clears this bit before setting the Endpoint Disabled
         *  Interrupt. The application must Set this bit only if Endpoint Enable is
         *  already set for this endpoint.
         */
        uint32_t otgfs_diepctl0_epdis:1;
        /** otgfs_diepctl0_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_diepctl0_epena:1;
    };
    uint32_t val;
} usb_otgfs_diepctl0_reg_t;


/** Group: Device IN Endpoint 0 Interrupt Register */
/** Type of otgfs_diepint0 register
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
typedef union {
    struct {
        /** otgfs_diepint0_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_diepint0_xfercompl:1;
        /** otgfs_diepint0_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_diepint0_epdisbld:1;
        /** otgfs_diepint0_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an AHB error during an
         *  AHB read/write. The application can read the corresponding endpoint DMA address
         *  register to get the error address.
         */
        uint32_t otgfs_diepint0_ahberr:1;
        /** otgfs_diepint0_timeout : R/W; bitpos: [3]; default: 0;
         *  Timeout Condition (TimeOUT)
         *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
         *  - In dedicated FIFO mode, applies only to Control IN endpoints.
         *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
         *  Indicates that the core has detected a timeout condition on the USB for the last IN
         *  token on this endpoint.
         */
        uint32_t otgfs_diepint0_timeout:1;
        /** otgfs_diepint0_intkntxfemp : R/W; bitpos: [4]; default: 0;
         *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that an IN token was received when the associated TxFIFO
         *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
         *  which the IN token was received.
         */
        uint32_t otgfs_diepint0_intkntxfemp:1;
        /** otgfs_diepint0_intknepmis : R/W; bitpos: [5]; default: 0;
         *  IN Token Received with EP Mismatch (INTknEPMis)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
         *  endpoint other than the one for which the IN token was received. This interrupt is
         *  asserted on the endpoint for which the IN token was received.
         */
        uint32_t otgfs_diepint0_intknepmis:1;
        /** otgfs_diepint0_inepnakeff : R/W; bitpos: [6]; default: 0;
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
        uint32_t otgfs_diepint0_inepnakeff:1;
        /** otgfs_diepint0_txfemp : RO; bitpos: [7]; default: 1;
         *  Transmit FIFO Empty (TxFEmp)
         *
         *  This bit is valid only for IN Endpoints
         *
         *  This interrupt is asserted when the TxFIFO for this endpoint is either half or
         *  completely empty. The half or completely empty status is determined by the TxFIFO
         *  Empty Level bit in the Core AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
         */
        uint32_t otgfs_diepint0_txfemp:1;
        /** otgfs_diepint0_txfifoundrn : R/W; bitpos: [8]; default: 0;
         *  Fifo Underrun (TxfifoUndrn)
         *
         *  Applies to IN endpoints only.
         *
         *  The core generates this interrupt when it detects a transmit FIFO underrun
         *  condition in threshold mode for this endpoint.
         */
        uint32_t otgfs_diepint0_txfifoundrn:1;
        /** otgfs_diepint0_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled. The core generates
         *  this interrupt when the descriptor accessed is not ready for the Core to process,
         *  such as Host busy or DMA done.
         */
        uint32_t otgfs_diepint0_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_diepint0_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped.
         *  This bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_diepint0_pktdrpsts:1;
        /** otgfs_diepint0_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_diepint0_bbleerr:1;
        /** otgfs_diepint0_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *  <bring case of isochronous IN endpoints the interrupt gets generated when a zero
         *  length packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_diepint0_nakintrpt:1;
        /** otgfs_diepint0_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_diepint0_nyetintrpt:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} usb_otgfs_diepint0_reg_t;


/** Group: Device IN Endpoint 0 Transfer Size Register */
/** Type of otgfs_dieptsiz0 register
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
typedef union {
    struct {
        /** otgfs_dieptsiz0_xfersize : R/W; bitpos: [6:0]; default: 0;
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
        uint32_t otgfs_dieptsiz0_xfersize:7;
        uint32_t reserved_7:12;
        /** otgfs_dieptsiz0_pktcnt : R/W; bitpos: [20:19]; default: 0;
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
        uint32_t otgfs_dieptsiz0_pktcnt:2;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} usb_otgfs_dieptsiz0_reg_t;


/** Group: Device IN Endpoint 0 DMA Address Register */
/** Type of otgfs_diepdma0 register
 *  Device IN Endpoint 0 DMA Address Register
 */
typedef union {
    struct {
        /** otgfs_diepdma0_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_diepdma0_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdma0_reg_t;


/** Group: Device IN Endpoint Transmit FIFO Status Register 0 */
/** Type of otgfs_dtxfsts0 register
 *  Device IN Endpoint Transmit FIFO Status Register 0
 */
typedef union {
    struct {
        /** otgfs_dtxfsts0_ineptxfspcavail : RO; bitpos: [15:0]; default: 256;
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
        uint32_t otgfs_dtxfsts0_ineptxfspcavail:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_dtxfsts0_reg_t;


/** Group: Device IN Endpoint 16 Buffer Address Register */
/** Type of otgfs_diepdmab0 register
 *  Device IN Endpoint 16 Buffer Address Register
 */
typedef union {
    struct {
        /** otgfs_diepdmab0_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_diepdmab0_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdmab0_reg_t;


/** Group: Device Control IN Endpoint 1 Control Register */
/** Type of otgfs_diepctl1 register
 *  Device Control IN Endpoint 1 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepctl1_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the
         *  current logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_diepctl1_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_diepctl1_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_diepctl1_usbactep:1;
        /** otgfs_diepctl1_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_diepctl1_dpid:1;
        /** otgfs_diepctl1_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_diepctl1_naksts:1;
        /** otgfs_diepctl1_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_diepctl1_eptype:2;
        uint32_t reserved_20:1;
        /** otgfs_diepctl1_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_diepctl1_stall:1;
        /** otgfs_diepctl1_txfnum : R/W; bitpos: [25:22]; default: 0;
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
        uint32_t otgfs_diepctl1_txfnum:4;
        /** otgfs_diepctl1_cnak : R/W; bitpos: [26]; default: 0;
         *  Clear NAK (CNAK)
         *
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_diepctl1_cnak:1;
        /** otgfs_diepctl1_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK handshakes on
         *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
         *  received on that endpoint.
         */
        uint32_t otgfs_diepctl1_snak:1;
        /** otgfs_diepctl1_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_diepctl1_setd0pid:1;
        /** otgfs_diepctl1_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_diepctl1_setd1pid:1;
        /** otgfs_diepctl1_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_diepctl1_epdis:1;
        /** otgfs_diepctl1_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_diepctl1_epena:1;
    };
    uint32_t val;
} usb_otgfs_diepctl1_reg_t;


/** Group: Device IN Endpoint 1 Interrupt Register */
/** Type of otgfs_diepint1 register
 *  Device IN Endpoint 1 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepint1_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_diepint1_xfercompl:1;
        /** otgfs_diepint1_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_diepint1_epdisbld:1;
        /** otgfs_diepint1_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_diepint1_ahberr:1;
        /** otgfs_diepint1_timeout : R/W; bitpos: [3]; default: 0;
         *  Timeout Condition (TimeOUT)
         *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
         *  - In dedicated FIFO mode, applies only to Control IN endpoints.
         *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
         *  Indicates that the core has detected a timeout condition on the USB for the last IN
         *  token on this endpoint.
         */
        uint32_t otgfs_diepint1_timeout:1;
        /** otgfs_diepint1_intkntxfemp : R/W; bitpos: [4]; default: 0;
         *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that an IN token was received when the associated TxFIFO
         *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
         *  which the IN token was received.
         */
        uint32_t otgfs_diepint1_intkntxfemp:1;
        /** otgfs_diepint1_intknepmis : R/W; bitpos: [5]; default: 0;
         *  IN Token Received with EP Mismatch (INTknEPMis)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
         *  endpoint other than the one for which the IN token was received. This interrupt is
         *  asserted on the endpoint for which the IN token was received.
         */
        uint32_t otgfs_diepint1_intknepmis:1;
        /** otgfs_diepint1_inepnakeff : R/W; bitpos: [6]; default: 0;
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
        uint32_t otgfs_diepint1_inepnakeff:1;
        /** otgfs_diepint1_txfemp : RO; bitpos: [7]; default: 1;
         *  Transmit FIFO Empty (TxFEmp)
         *
         *  This bit is valid only for IN endpoints
         *
         *  This interrupt is asserted when the TxFIFO for this endpoint is
         *  either half or completely empty. The half or completely empty
         *  status is determined by the TxFIFO Empty Level bit in the Core
         *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
         */
        uint32_t otgfs_diepint1_txfemp:1;
        /** otgfs_diepint1_txfifoundrn : R/W; bitpos: [8]; default: 0;
         *  Fifo Underrun (TxfifoUndrn)
         *
         *  Applies to IN endpoints Only
         *
         *  This bit is valid only If thresholding is enabled. The core generates this
         *  interrupt when
         *  it detects a transmit FIFO underrun condition for this endpoint.
         */
        uint32_t otgfs_diepint1_txfifoundrn:1;
        /** otgfs_diepint1_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed is not ready for the
         *  Core to process, such as Host busy or DMA done.
         */
        uint32_t otgfs_diepint1_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_diepint1_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_diepint1_pktdrpsts:1;
        /** otgfs_diepint1_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_diepint1_bbleerr:1;
        /** otgfs_diepint1_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_diepint1_nakintrpt:1;
        /** otgfs_diepint1_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_diepint1_nyetintrpt:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} usb_otgfs_diepint1_reg_t;


/** Group: Device IN Endpoint 1 Transfer Size Register */
/** Type of otgfs_dieptsiz1 register
 *  Device IN Endpoint 1 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dieptsiz1_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_dieptsiz1_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_dieptsiz1_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *
         *  Indicates the total number of USB packets that constitute the Transfer Size amount
         *  of data for endpoint 0.
         *
         *  This field is decremented every time a packet (maximum size or short packet) is
         *  read from the TxFIFO.
         */
        uint32_t otgfs_dieptsiz1_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_dieptsiz1_mc : R/W; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_dieptsiz1_mc:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_dieptsiz1_reg_t;


/** Group: Device IN Endpoint 1 DMA Address Register */
/** Type of otgfs_diepdma1 register
 *  Device IN Endpoint 1 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdma1_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_diepdma1_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdma1_reg_t;


/** Group: Device IN Endpoint Transmit FIFO Status Register 1 */
/** Type of otgfs_dtxfsts1 register
 *  Device IN Endpoint Transmit FIFO Status Register 1  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dtxfsts1_ineptxfspcavail : RO; bitpos: [15:0]; default: 256;
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
        uint32_t otgfs_dtxfsts1_ineptxfspcavail:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_dtxfsts1_reg_t;


/** Group: Device IN Endpoint 1 Buffer Address Register */
/** Type of otgfs_diepdmab1 register
 *  Device IN Endpoint 1 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdmab1_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_diepdmab1_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdmab1_reg_t;


/** Group: Device Control IN Endpoint 2 Control Register */
/** Type of otgfs_diepctl2 register
 *  Device Control IN Endpoint 2 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepctl2_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the
         *  current logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_diepctl2_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_diepctl2_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_diepctl2_usbactep:1;
        /** otgfs_diepctl2_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_diepctl2_dpid:1;
        /** otgfs_diepctl2_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_diepctl2_naksts:1;
        /** otgfs_diepctl2_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_diepctl2_eptype:2;
        uint32_t reserved_20:1;
        /** otgfs_diepctl2_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_diepctl2_stall:1;
        /** otgfs_diepctl2_txfnum : R/W; bitpos: [25:22]; default: 0;
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
        uint32_t otgfs_diepctl2_txfnum:4;
        /** otgfs_diepctl2_cnak : R/W; bitpos: [26]; default: 0;
         *  Clear NAK (CNAK)
         *
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_diepctl2_cnak:1;
        /** otgfs_diepctl2_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK handshakes on
         *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
         *  received on that endpoint.
         */
        uint32_t otgfs_diepctl2_snak:1;
        /** otgfs_diepctl2_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_diepctl2_setd0pid:1;
        /** otgfs_diepctl2_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_diepctl2_setd1pid:1;
        /** otgfs_diepctl2_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_diepctl2_epdis:1;
        /** otgfs_diepctl2_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_diepctl2_epena:1;
    };
    uint32_t val;
} usb_otgfs_diepctl2_reg_t;


/** Group: Device IN Endpoint 2 Interrupt Register */
/** Type of otgfs_diepint2 register
 *  Device IN Endpoint 2 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepint2_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_diepint2_xfercompl:1;
        /** otgfs_diepint2_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_diepint2_epdisbld:1;
        /** otgfs_diepint2_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_diepint2_ahberr:1;
        /** otgfs_diepint2_timeout : R/W; bitpos: [3]; default: 0;
         *  Timeout Condition (TimeOUT)
         *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
         *  - In dedicated FIFO mode, applies only to Control IN endpoints.
         *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
         *  Indicates that the core has detected a timeout condition on the USB for the last IN
         *  token on this endpoint.
         */
        uint32_t otgfs_diepint2_timeout:1;
        /** otgfs_diepint2_intkntxfemp : R/W; bitpos: [4]; default: 0;
         *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that an IN token was received when the associated TxFIFO
         *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
         *  which the IN token was received.
         */
        uint32_t otgfs_diepint2_intkntxfemp:1;
        /** otgfs_diepint2_intknepmis : R/W; bitpos: [5]; default: 0;
         *  IN Token Received with EP Mismatch (INTknEPMis)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
         *  endpoint other than the one for which the IN token was received. This interrupt is
         *  asserted on the endpoint for which the IN token was received.
         */
        uint32_t otgfs_diepint2_intknepmis:1;
        /** otgfs_diepint2_inepnakeff : R/W; bitpos: [6]; default: 0;
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
        uint32_t otgfs_diepint2_inepnakeff:1;
        /** otgfs_diepint2_txfemp : RO; bitpos: [7]; default: 1;
         *  Transmit FIFO Empty (TxFEmp)
         *
         *  This bit is valid only for IN endpoints
         *
         *  This interrupt is asserted when the TxFIFO for this endpoint is
         *  either half or completely empty. The half or completely empty
         *  status is determined by the TxFIFO Empty Level bit in the Core
         *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
         */
        uint32_t otgfs_diepint2_txfemp:1;
        /** otgfs_diepint2_txfifoundrn : R/W; bitpos: [8]; default: 0;
         *  Fifo Underrun (TxfifoUndrn)
         *
         *  Applies to IN endpoints Only
         *
         *  This bit is valid only If thresholding is enabled. The core generates this
         *  interrupt when
         *  it detects a transmit FIFO underrun condition for this endpoint.
         */
        uint32_t otgfs_diepint2_txfifoundrn:1;
        /** otgfs_diepint2_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed is not ready for the
         *  Core to process, such as Host busy or DMA done.
         */
        uint32_t otgfs_diepint2_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_diepint2_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_diepint2_pktdrpsts:1;
        /** otgfs_diepint2_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_diepint2_bbleerr:1;
        /** otgfs_diepint2_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_diepint2_nakintrpt:1;
        /** otgfs_diepint2_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_diepint2_nyetintrpt:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} usb_otgfs_diepint2_reg_t;


/** Group: Device IN Endpoint 2 Transfer Size Register */
/** Type of otgfs_dieptsiz2 register
 *  Device IN Endpoint 2 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dieptsiz2_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_dieptsiz2_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_dieptsiz2_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *
         *  Indicates the total number of USB packets that constitute the Transfer Size amount
         *  of data for endpoint 0.
         *
         *  This field is decremented every time a packet (maximum size or short packet) is
         *  read from the TxFIFO.
         */
        uint32_t otgfs_dieptsiz2_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_dieptsiz2_mc : R/W; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_dieptsiz2_mc:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_dieptsiz2_reg_t;


/** Group: Device IN Endpoint 2 DMA Address Register */
/** Type of otgfs_diepdma2 register
 *  Device IN Endpoint 2 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdma2_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_diepdma2_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdma2_reg_t;


/** Group: Device IN Endpoint Transmit FIFO Status Register 2 */
/** Type of otgfs_dtxfsts2 register
 *  Device IN Endpoint Transmit FIFO Status Register 2  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dtxfsts2_ineptxfspcavail : RO; bitpos: [15:0]; default: 256;
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
        uint32_t otgfs_dtxfsts2_ineptxfspcavail:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_dtxfsts2_reg_t;


/** Group: Device IN Endpoint 2 Buffer Address Register */
/** Type of otgfs_diepdmab2 register
 *  Device IN Endpoint 2 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdmab2_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_diepdmab2_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdmab2_reg_t;


/** Group: Device Control IN Endpoint 3 Control Register */
/** Type of otgfs_diepctl3 register
 *  Device Control IN Endpoint 3 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepctl3_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the
         *  current logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_diepctl3_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_diepctl3_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_diepctl3_usbactep:1;
        /** otgfs_diepctl3_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_diepctl3_dpid:1;
        /** otgfs_diepctl3_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_diepctl3_naksts:1;
        /** otgfs_diepctl3_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_diepctl3_eptype:2;
        uint32_t reserved_20:1;
        /** otgfs_diepctl3_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_diepctl3_stall:1;
        /** otgfs_diepctl3_txfnum : R/W; bitpos: [25:22]; default: 0;
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
        uint32_t otgfs_diepctl3_txfnum:4;
        /** otgfs_diepctl3_cnak : R/W; bitpos: [26]; default: 0;
         *  Clear NAK (CNAK)
         *
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_diepctl3_cnak:1;
        /** otgfs_diepctl3_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK handshakes on
         *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
         *  received on that endpoint.
         */
        uint32_t otgfs_diepctl3_snak:1;
        /** otgfs_diepctl3_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_diepctl3_setd0pid:1;
        /** otgfs_diepctl3_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_diepctl3_setd1pid:1;
        /** otgfs_diepctl3_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_diepctl3_epdis:1;
        /** otgfs_diepctl3_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_diepctl3_epena:1;
    };
    uint32_t val;
} usb_otgfs_diepctl3_reg_t;


/** Group: Device IN Endpoint 3 Interrupt Register */
/** Type of otgfs_diepint3 register
 *  Device IN Endpoint 3 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepint3_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_diepint3_xfercompl:1;
        /** otgfs_diepint3_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_diepint3_epdisbld:1;
        /** otgfs_diepint3_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_diepint3_ahberr:1;
        /** otgfs_diepint3_timeout : R/W; bitpos: [3]; default: 0;
         *  Timeout Condition (TimeOUT)
         *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
         *  - In dedicated FIFO mode, applies only to Control IN endpoints.
         *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
         *  Indicates that the core has detected a timeout condition on the USB for the last IN
         *  token on this endpoint.
         */
        uint32_t otgfs_diepint3_timeout:1;
        /** otgfs_diepint3_intkntxfemp : R/W; bitpos: [4]; default: 0;
         *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that an IN token was received when the associated TxFIFO
         *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
         *  which the IN token was received.
         */
        uint32_t otgfs_diepint3_intkntxfemp:1;
        /** otgfs_diepint3_intknepmis : R/W; bitpos: [5]; default: 0;
         *  IN Token Received with EP Mismatch (INTknEPMis)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
         *  endpoint other than the one for which the IN token was received. This interrupt is
         *  asserted on the endpoint for which the IN token was received.
         */
        uint32_t otgfs_diepint3_intknepmis:1;
        /** otgfs_diepint3_inepnakeff : R/W; bitpos: [6]; default: 0;
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
        uint32_t otgfs_diepint3_inepnakeff:1;
        /** otgfs_diepint3_txfemp : RO; bitpos: [7]; default: 1;
         *  Transmit FIFO Empty (TxFEmp)
         *
         *  This bit is valid only for IN endpoints
         *
         *  This interrupt is asserted when the TxFIFO for this endpoint is
         *  either half or completely empty. The half or completely empty
         *  status is determined by the TxFIFO Empty Level bit in the Core
         *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
         */
        uint32_t otgfs_diepint3_txfemp:1;
        /** otgfs_diepint3_txfifoundrn : R/W; bitpos: [8]; default: 0;
         *  Fifo Underrun (TxfifoUndrn)
         *
         *  Applies to IN endpoints Only
         *
         *  This bit is valid only If thresholding is enabled. The core generates this
         *  interrupt when
         *  it detects a transmit FIFO underrun condition for this endpoint.
         */
        uint32_t otgfs_diepint3_txfifoundrn:1;
        /** otgfs_diepint3_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed is not ready for the
         *  Core to process, such as Host busy or DMA done.
         */
        uint32_t otgfs_diepint3_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_diepint3_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_diepint3_pktdrpsts:1;
        /** otgfs_diepint3_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_diepint3_bbleerr:1;
        /** otgfs_diepint3_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_diepint3_nakintrpt:1;
        /** otgfs_diepint3_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_diepint3_nyetintrpt:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} usb_otgfs_diepint3_reg_t;


/** Group: Device IN Endpoint 3 Transfer Size Register */
/** Type of otgfs_dieptsiz3 register
 *  Device IN Endpoint 3 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dieptsiz3_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_dieptsiz3_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_dieptsiz3_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *
         *  Indicates the total number of USB packets that constitute the Transfer Size amount
         *  of data for endpoint 0.
         *
         *  This field is decremented every time a packet (maximum size or short packet) is
         *  read from the TxFIFO.
         */
        uint32_t otgfs_dieptsiz3_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_dieptsiz3_mc : R/W; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_dieptsiz3_mc:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_dieptsiz3_reg_t;


/** Group: Device IN Endpoint 3 DMA Address Register */
/** Type of otgfs_diepdma3 register
 *  Device IN Endpoint 3 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdma3_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_diepdma3_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdma3_reg_t;


/** Group: Device IN Endpoint Transmit FIFO Status Register 3 */
/** Type of otgfs_dtxfsts3 register
 *  Device IN Endpoint Transmit FIFO Status Register 3  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dtxfsts3_ineptxfspcavail : RO; bitpos: [15:0]; default: 256;
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
        uint32_t otgfs_dtxfsts3_ineptxfspcavail:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_dtxfsts3_reg_t;


/** Group: Device IN Endpoint 3 Buffer Address Register */
/** Type of otgfs_diepdmab3 register
 *  Device IN Endpoint 3 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdmab3_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_diepdmab3_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdmab3_reg_t;


/** Group: Device Control IN Endpoint 4 Control Register */
/** Type of otgfs_diepctl4 register
 *  Device Control IN Endpoint 4 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepctl4_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the
         *  current logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_diepctl4_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_diepctl4_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_diepctl4_usbactep:1;
        /** otgfs_diepctl4_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_diepctl4_dpid:1;
        /** otgfs_diepctl4_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_diepctl4_naksts:1;
        /** otgfs_diepctl4_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_diepctl4_eptype:2;
        uint32_t reserved_20:1;
        /** otgfs_diepctl4_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_diepctl4_stall:1;
        /** otgfs_diepctl4_txfnum : R/W; bitpos: [25:22]; default: 0;
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
        uint32_t otgfs_diepctl4_txfnum:4;
        /** otgfs_diepctl4_cnak : R/W; bitpos: [26]; default: 0;
         *  Clear NAK (CNAK)
         *
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_diepctl4_cnak:1;
        /** otgfs_diepctl4_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK handshakes on
         *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
         *  received on that endpoint.
         */
        uint32_t otgfs_diepctl4_snak:1;
        /** otgfs_diepctl4_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_diepctl4_setd0pid:1;
        /** otgfs_diepctl4_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_diepctl4_setd1pid:1;
        /** otgfs_diepctl4_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_diepctl4_epdis:1;
        /** otgfs_diepctl4_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_diepctl4_epena:1;
    };
    uint32_t val;
} usb_otgfs_diepctl4_reg_t;


/** Group: Device IN Endpoint 4 Interrupt Register */
/** Type of otgfs_diepint4 register
 *  Device IN Endpoint 4 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepint4_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_diepint4_xfercompl:1;
        /** otgfs_diepint4_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_diepint4_epdisbld:1;
        /** otgfs_diepint4_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_diepint4_ahberr:1;
        /** otgfs_diepint4_timeout : R/W; bitpos: [3]; default: 0;
         *  Timeout Condition (TimeOUT)
         *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
         *  - In dedicated FIFO mode, applies only to Control IN endpoints.
         *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
         *  Indicates that the core has detected a timeout condition on the USB for the last IN
         *  token on this endpoint.
         */
        uint32_t otgfs_diepint4_timeout:1;
        /** otgfs_diepint4_intkntxfemp : R/W; bitpos: [4]; default: 0;
         *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that an IN token was received when the associated TxFIFO
         *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
         *  which the IN token was received.
         */
        uint32_t otgfs_diepint4_intkntxfemp:1;
        /** otgfs_diepint4_intknepmis : R/W; bitpos: [5]; default: 0;
         *  IN Token Received with EP Mismatch (INTknEPMis)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
         *  endpoint other than the one for which the IN token was received. This interrupt is
         *  asserted on the endpoint for which the IN token was received.
         */
        uint32_t otgfs_diepint4_intknepmis:1;
        /** otgfs_diepint4_inepnakeff : R/W; bitpos: [6]; default: 0;
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
        uint32_t otgfs_diepint4_inepnakeff:1;
        /** otgfs_diepint4_txfemp : RO; bitpos: [7]; default: 1;
         *  Transmit FIFO Empty (TxFEmp)
         *
         *  This bit is valid only for IN endpoints
         *
         *  This interrupt is asserted when the TxFIFO for this endpoint is
         *  either half or completely empty. The half or completely empty
         *  status is determined by the TxFIFO Empty Level bit in the Core
         *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
         */
        uint32_t otgfs_diepint4_txfemp:1;
        /** otgfs_diepint4_txfifoundrn : R/W; bitpos: [8]; default: 0;
         *  Fifo Underrun (TxfifoUndrn)
         *
         *  Applies to IN endpoints Only
         *
         *  This bit is valid only If thresholding is enabled. The core generates this
         *  interrupt when
         *  it detects a transmit FIFO underrun condition for this endpoint.
         */
        uint32_t otgfs_diepint4_txfifoundrn:1;
        /** otgfs_diepint4_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed is not ready for the
         *  Core to process, such as Host busy or DMA done.
         */
        uint32_t otgfs_diepint4_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_diepint4_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_diepint4_pktdrpsts:1;
        /** otgfs_diepint4_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_diepint4_bbleerr:1;
        /** otgfs_diepint4_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_diepint4_nakintrpt:1;
        /** otgfs_diepint4_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_diepint4_nyetintrpt:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} usb_otgfs_diepint4_reg_t;


/** Group: Device IN Endpoint 4 Transfer Size Register */
/** Type of otgfs_dieptsiz4 register
 *  Device IN Endpoint 4 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dieptsiz4_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_dieptsiz4_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_dieptsiz4_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *
         *  Indicates the total number of USB packets that constitute the Transfer Size amount
         *  of data for endpoint 0.
         *
         *  This field is decremented every time a packet (maximum size or short packet) is
         *  read from the TxFIFO.
         */
        uint32_t otgfs_dieptsiz4_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_dieptsiz4_mc : R/W; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_dieptsiz4_mc:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_dieptsiz4_reg_t;


/** Group: Device IN Endpoint 4 DMA Address Register */
/** Type of otgfs_diepdma4 register
 *  Device IN Endpoint 4 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdma4_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_diepdma4_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdma4_reg_t;


/** Group: Device IN Endpoint Transmit FIFO Status Register 4 */
/** Type of otgfs_dtxfsts4 register
 *  Device IN Endpoint Transmit FIFO Status Register 4  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dtxfsts4_ineptxfspcavail : RO; bitpos: [15:0]; default: 256;
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
        uint32_t otgfs_dtxfsts4_ineptxfspcavail:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_dtxfsts4_reg_t;


/** Group: Device IN Endpoint 4 Buffer Address Register */
/** Type of otgfs_diepdmab4 register
 *  Device IN Endpoint 4 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdmab4_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_diepdmab4_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdmab4_reg_t;


/** Group: Device Control IN Endpoint 5 Control Register */
/** Type of otgfs_diepctl5 register
 *  Device Control IN Endpoint 5 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepctl5_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the
         *  current logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_diepctl5_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_diepctl5_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_diepctl5_usbactep:1;
        /** otgfs_diepctl5_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_diepctl5_dpid:1;
        /** otgfs_diepctl5_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_diepctl5_naksts:1;
        /** otgfs_diepctl5_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_diepctl5_eptype:2;
        uint32_t reserved_20:1;
        /** otgfs_diepctl5_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_diepctl5_stall:1;
        /** otgfs_diepctl5_txfnum : R/W; bitpos: [25:22]; default: 0;
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
        uint32_t otgfs_diepctl5_txfnum:4;
        /** otgfs_diepctl5_cnak : R/W; bitpos: [26]; default: 0;
         *  Clear NAK (CNAK)
         *
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_diepctl5_cnak:1;
        /** otgfs_diepctl5_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK handshakes on
         *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
         *  received on that endpoint.
         */
        uint32_t otgfs_diepctl5_snak:1;
        /** otgfs_diepctl5_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_diepctl5_setd0pid:1;
        /** otgfs_diepctl5_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_diepctl5_setd1pid:1;
        /** otgfs_diepctl5_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_diepctl5_epdis:1;
        /** otgfs_diepctl5_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_diepctl5_epena:1;
    };
    uint32_t val;
} usb_otgfs_diepctl5_reg_t;


/** Group: Device IN Endpoint 5 Interrupt Register */
/** Type of otgfs_diepint5 register
 *  Device IN Endpoint 5 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepint5_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_diepint5_xfercompl:1;
        /** otgfs_diepint5_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_diepint5_epdisbld:1;
        /** otgfs_diepint5_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_diepint5_ahberr:1;
        /** otgfs_diepint5_timeout : R/W; bitpos: [3]; default: 0;
         *  Timeout Condition (TimeOUT)
         *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
         *  - In dedicated FIFO mode, applies only to Control IN endpoints.
         *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
         *  Indicates that the core has detected a timeout condition on the USB for the last IN
         *  token on this endpoint.
         */
        uint32_t otgfs_diepint5_timeout:1;
        /** otgfs_diepint5_intkntxfemp : R/W; bitpos: [4]; default: 0;
         *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that an IN token was received when the associated TxFIFO
         *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
         *  which the IN token was received.
         */
        uint32_t otgfs_diepint5_intkntxfemp:1;
        /** otgfs_diepint5_intknepmis : R/W; bitpos: [5]; default: 0;
         *  IN Token Received with EP Mismatch (INTknEPMis)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
         *  endpoint other than the one for which the IN token was received. This interrupt is
         *  asserted on the endpoint for which the IN token was received.
         */
        uint32_t otgfs_diepint5_intknepmis:1;
        /** otgfs_diepint5_inepnakeff : R/W; bitpos: [6]; default: 0;
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
        uint32_t otgfs_diepint5_inepnakeff:1;
        /** otgfs_diepint5_txfemp : RO; bitpos: [7]; default: 1;
         *  Transmit FIFO Empty (TxFEmp)
         *
         *  This bit is valid only for IN endpoints
         *
         *  This interrupt is asserted when the TxFIFO for this endpoint is
         *  either half or completely empty. The half or completely empty
         *  status is determined by the TxFIFO Empty Level bit in the Core
         *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
         */
        uint32_t otgfs_diepint5_txfemp:1;
        /** otgfs_diepint5_txfifoundrn : R/W; bitpos: [8]; default: 0;
         *  Fifo Underrun (TxfifoUndrn)
         *
         *  Applies to IN endpoints Only
         *
         *  This bit is valid only If thresholding is enabled. The core generates this
         *  interrupt when
         *  it detects a transmit FIFO underrun condition for this endpoint.
         */
        uint32_t otgfs_diepint5_txfifoundrn:1;
        /** otgfs_diepint5_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed is not ready for the
         *  Core to process, such as Host busy or DMA done.
         */
        uint32_t otgfs_diepint5_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_diepint5_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_diepint5_pktdrpsts:1;
        /** otgfs_diepint5_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_diepint5_bbleerr:1;
        /** otgfs_diepint5_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_diepint5_nakintrpt:1;
        /** otgfs_diepint5_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_diepint5_nyetintrpt:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} usb_otgfs_diepint5_reg_t;


/** Group: Device IN Endpoint 5 Transfer Size Register */
/** Type of otgfs_dieptsiz5 register
 *  Device IN Endpoint 5 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dieptsiz5_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_dieptsiz5_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_dieptsiz5_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *
         *  Indicates the total number of USB packets that constitute the Transfer Size amount
         *  of data for endpoint 0.
         *
         *  This field is decremented every time a packet (maximum size or short packet) is
         *  read from the TxFIFO.
         */
        uint32_t otgfs_dieptsiz5_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_dieptsiz5_mc : R/W; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_dieptsiz5_mc:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_dieptsiz5_reg_t;


/** Group: Device IN Endpoint 5 DMA Address Register */
/** Type of otgfs_diepdma5 register
 *  Device IN Endpoint 5 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdma5_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_diepdma5_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdma5_reg_t;


/** Group: Device IN Endpoint Transmit FIFO Status Register 5 */
/** Type of otgfs_dtxfsts5 register
 *  Device IN Endpoint Transmit FIFO Status Register 5  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dtxfsts5_ineptxfspcavail : RO; bitpos: [15:0]; default: 256;
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
        uint32_t otgfs_dtxfsts5_ineptxfspcavail:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_dtxfsts5_reg_t;


/** Group: Device IN Endpoint 5 Buffer Address Register */
/** Type of otgfs_diepdmab5 register
 *  Device IN Endpoint 5 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdmab5_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_diepdmab5_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdmab5_reg_t;


/** Group: Device Control IN Endpoint 6 Control Register */
/** Type of otgfs_diepctl6 register
 *  Device Control IN Endpoint 6 Control Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepctl6_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the
         *  current logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_diepctl6_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_diepctl6_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_diepctl6_usbactep:1;
        /** otgfs_diepctl6_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_diepctl6_dpid:1;
        /** otgfs_diepctl6_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_diepctl6_naksts:1;
        /** otgfs_diepctl6_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_diepctl6_eptype:2;
        uint32_t reserved_20:1;
        /** otgfs_diepctl6_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_diepctl6_stall:1;
        /** otgfs_diepctl6_txfnum : R/W; bitpos: [25:22]; default: 0;
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
        uint32_t otgfs_diepctl6_txfnum:4;
        /** otgfs_diepctl6_cnak : R/W; bitpos: [26]; default: 0;
         *  Clear NAK (CNAK)
         *
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_diepctl6_cnak:1;
        /** otgfs_diepctl6_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK handshakes on
         *  an endpoint. The core can also Set this bit for an endpoint after a SETUP packet is
         *  received on that endpoint.
         */
        uint32_t otgfs_diepctl6_snak:1;
        /** otgfs_diepctl6_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_diepctl6_setd0pid:1;
        /** otgfs_diepctl6_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_diepctl6_setd1pid:1;
        /** otgfs_diepctl6_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_diepctl6_epdis:1;
        /** otgfs_diepctl6_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_diepctl6_epena:1;
    };
    uint32_t val;
} usb_otgfs_diepctl6_reg_t;


/** Group: Device IN Endpoint 6 Interrupt Register */
/** Type of otgfs_diepint6 register
 *  Device IN Endpoint 6 Interrupt Register  Note: This register exists for an endpoint
 *  i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepint6_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_diepint6_xfercompl:1;
        /** otgfs_diepint6_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_diepint6_epdisbld:1;
        /** otgfs_diepint6_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_diepint6_ahberr:1;
        /** otgfs_diepint6_timeout : R/W; bitpos: [3]; default: 0;
         *  Timeout Condition (TimeOUT)
         *  - In shared TX FIFO mode, applies to non-isochronous IN endpoints only.
         *  - In dedicated FIFO mode, applies only to Control IN endpoints.
         *  - In Scatter/Gather DMA mode, the TimeOUT interrupt is not asserted.
         *  Indicates that the core has detected a timeout condition on the USB for the last IN
         *  token on this endpoint.
         */
        uint32_t otgfs_diepint6_timeout:1;
        /** otgfs_diepint6_intkntxfemp : R/W; bitpos: [4]; default: 0;
         *  IN Token Received When TxFIFO is Empty (INTknTXFEmp)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that an IN token was received when the associated TxFIFO
         *  (periodic/non-periodic) was empty. This interrupt is asserted on the endpoint for
         *  which the IN token was received.
         */
        uint32_t otgfs_diepint6_intkntxfemp:1;
        /** otgfs_diepint6_intknepmis : R/W; bitpos: [5]; default: 0;
         *  IN Token Received with EP Mismatch (INTknEPMis)
         *
         *  Applies to non-periodic IN endpoints only.
         *
         *  Indicates that the data in the top of the non-periodic TxFIFO belongs to an
         *  endpoint other than the one for which the IN token was received. This interrupt is
         *  asserted on the endpoint for which the IN token was received.
         */
        uint32_t otgfs_diepint6_intknepmis:1;
        /** otgfs_diepint6_inepnakeff : R/W; bitpos: [6]; default: 0;
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
        uint32_t otgfs_diepint6_inepnakeff:1;
        /** otgfs_diepint6_txfemp : RO; bitpos: [7]; default: 1;
         *  Transmit FIFO Empty (TxFEmp)
         *
         *  This bit is valid only for IN endpoints
         *
         *  This interrupt is asserted when the TxFIFO for this endpoint is
         *  either half or completely empty. The half or completely empty
         *  status is determined by the TxFIFO Empty Level bit in the Core
         *  AHB Configuration register (GAHBCFG.NPTxFEmpLvl)).
         */
        uint32_t otgfs_diepint6_txfemp:1;
        /** otgfs_diepint6_txfifoundrn : R/W; bitpos: [8]; default: 0;
         *  Fifo Underrun (TxfifoUndrn)
         *
         *  Applies to IN endpoints Only
         *
         *  This bit is valid only If thresholding is enabled. The core generates this
         *  interrupt when
         *  it detects a transmit FIFO underrun condition for this endpoint.
         */
        uint32_t otgfs_diepint6_txfifoundrn:1;
        /** otgfs_diepint6_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed is not ready for the
         *  Core to process, such as Host busy or DMA done.
         */
        uint32_t otgfs_diepint6_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_diepint6_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_diepint6_pktdrpsts:1;
        /** otgfs_diepint6_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_diepint6_bbleerr:1;
        /** otgfs_diepint6_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_diepint6_nakintrpt:1;
        /** otgfs_diepint6_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_diepint6_nyetintrpt:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} usb_otgfs_diepint6_reg_t;


/** Group: Device IN Endpoint 6 Transfer Size Register */
/** Type of otgfs_dieptsiz6 register
 *  Device IN Endpoint 6 Transfer Size Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dieptsiz6_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_dieptsiz6_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_dieptsiz6_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *
         *  Indicates the total number of USB packets that constitute the Transfer Size amount
         *  of data for endpoint 0.
         *
         *  This field is decremented every time a packet (maximum size or short packet) is
         *  read from the TxFIFO.
         */
        uint32_t otgfs_dieptsiz6_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_dieptsiz6_mc : R/W; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_dieptsiz6_mc:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_dieptsiz6_reg_t;


/** Group: Device IN Endpoint 6 DMA Address Register */
/** Type of otgfs_diepdma6 register
 *  Device IN Endpoint 6 DMA Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdma6_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_diepdma6_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdma6_reg_t;


/** Group: Device IN Endpoint Transmit FIFO Status Register 6 */
/** Type of otgfs_dtxfsts6 register
 *  Device IN Endpoint Transmit FIFO Status Register 6  Note: This register exists for
 *  an endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_dtxfsts6_ineptxfspcavail : RO; bitpos: [15:0]; default: 256;
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
        uint32_t otgfs_dtxfsts6_ineptxfspcavail:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_dtxfsts6_reg_t;


/** Group: Device IN Endpoint 6 Buffer Address Register */
/** Type of otgfs_diepdmab6 register
 *  Device IN Endpoint 6 Buffer Address Register  Note: This register exists for an
 *  endpoint i if the OTG_EP_DIR_i parameter is 0 or 1 for that endpoint.
 */
typedef union {
    struct {
        /** otgfs_diepdmab6_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_diepdmab6_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_diepdmab6_reg_t;


/** Group: Device Control OUT Endpoint 0 Control Register */
/** Type of otgfs_doepctl0 register
 *  Device Control OUT Endpoint 0 Control Register
 */
typedef union {
    struct {
        /** otgfs_doepctl0_mps : RO; bitpos: [1:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The maximum packet size for control OUT endpoint 0 is the same as what is
         *  programmed in control IN Endpoint 0.
         *  - 2'b00: 64 bytes
         *  - 2'b01: 32 bytes
         *  - 2'b10: 16 bytes
         *  - 2'b11: 8 bytes
         */
        uint32_t otgfs_doepctl0_mps:2;
        uint32_t reserved_2:13;
        /** otgfs_doepctl0_usbactep : RO; bitpos: [15]; default: 1;
         *  USB Active Endpoint (USBActEP)
         *
         *  This bit is always set to 1, indicating that a control endpoint 0 is always active
         *  in all configurations and interfaces.
         */
        uint32_t otgfs_doepctl0_usbactep:1;
        uint32_t reserved_16:1;
        /** otgfs_doepctl0_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_doepctl0_naksts:1;
        /** otgfs_doepctl0_eptype : RO; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  Hardcoded to 2'b00 for control.
         */
        uint32_t otgfs_doepctl0_eptype:2;
        /** otgfs_doepctl0_snp : R/W; bitpos: [20]; default: 0;
         *  RESERVED
         */
        uint32_t otgfs_doepctl0_snp:1;
        /** otgfs_doepctl0_stall : R/W; bitpos: [21]; default: 0;
         *  STALL Handshake (Stall)
         *
         *  The application can only set this bit, and the core clears it, when
         *  a SETUP token is received for this endpoint. If a NAK bit or
         *  Global OUT NAK is Set along with this bit, the STALL bit takes
         *  priority. Irrespective of this bit's setting, the core always
         *  responds to SETUP data packets with an ACK handshake.
         */
        uint32_t otgfs_doepctl0_stall:1;
        uint32_t reserved_22:4;
        /** otgfs_doepctl0_cnak : R/W; bitpos: [26]; default: 0;
         *  Clear NAK (CNAK)
         *
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_doepctl0_cnak:1;
        /** otgfs_doepctl0_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *  Using this bit, the application can control the transmission of NAK handshakes on
         *  an endpoint.
         *  The core can also set bit on a Transfer Completed interrupt, or after a SETUP is
         *  received on the endpoint.
         */
        uint32_t otgfs_doepctl0_snak:1;
        uint32_t reserved_28:2;
        /** otgfs_doepctl0_epdis : RO; bitpos: [30]; default: 0;
         *  Endpoint Disable (EPDis)
         *
         *  The application cannot disable control OUT endpoint 0.
         */
        uint32_t otgfs_doepctl0_epdis:1;
        /** otgfs_doepctl0_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_doepctl0_epena:1;
    };
    uint32_t val;
} usb_otgfs_doepctl0_reg_t;


/** Group: Device OUT Endpoint 0 Interrupt Register */
/** Type of otgfs_doepint0 register
 *  Device OUT Endpoint 0 Interrupt Register
 */
typedef union {
    struct {
        /** otgfs_doepint0_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_doepint0_xfercompl:1;
        /** otgfs_doepint0_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_doepint0_epdisbld:1;
        /** otgfs_doepint0_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_doepint0_ahberr:1;
        /** otgfs_doepint0_setup : R/W; bitpos: [3]; default: 0;
         *  SETUP Phase Done (SetUp)
         *
         *  Applies to control OUT endpoints only.
         *
         *  Indicates that the SETUP phase for the control endpoint is
         *  complete and no more back-to-back SETUP packets were
         *  received for the current control transfer. On this interrupt, the
         *  application can decode the received SETUP data packet.
         */
        uint32_t otgfs_doepint0_setup:1;
        /** otgfs_doepint0_outtknepdis : R/W; bitpos: [4]; default: 0;
         *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
         *
         *  Applies only to control OUT endpoints.
         *
         *  Indicates that an OUT token was received when the endpoint
         *  was not yet enabled. This interrupt is asserted on the endpoint for which the OUT
         *  token was received.
         */
        uint32_t otgfs_doepint0_outtknepdis:1;
        /** otgfs_doepint0_stsphsercvd : R/W; bitpos: [5]; default: 0;
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
        uint32_t otgfs_doepint0_stsphsercvd:1;
        /** otgfs_doepint0_back2backsetup : R/W; bitpos: [6]; default: 0;
         *  Back-to-Back SETUP Packets Received (Back2BackSETup)
         *
         *  Applies to Control OUT endpoints only.
         *
         *  This bit indicates that the core has received more than three
         *  back-to-back SETUP packets for this particular endpoint. For
         *  information about handling this interrupt,
         */
        uint32_t otgfs_doepint0_back2backsetup:1;
        uint32_t reserved_7:1;
        /** otgfs_doepint0_outpkterr : R/W; bitpos: [8]; default: 0;
         *  OUT Packet Error (OutPktErr)
         *
         *  Applies to OUT endpoints Only
         *
         *  This interrupt is valid only when thresholding is enabled.
         *
         *  This interrupt is asserted when the core detects an overflow or a CRC error for
         *  non-Isochronous OUT packet.
         */
        uint32_t otgfs_doepint0_outpkterr:1;
        /** otgfs_doepint0_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the core to process, such as Host busy or DMA
         *  done.
         */
        uint32_t otgfs_doepint0_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_doepint0_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_doepint0_pktdrpsts:1;
        /** otgfs_doepint0_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_doepint0_bbleerr:1;
        /** otgfs_doepint0_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_doepint0_nakintrpt:1;
        /** otgfs_doepint0_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_doepint0_nyetintrpt:1;
        /** otgfs_doepint0_stuppktrcvd : R/W; bitpos: [15]; default: 0;
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
        uint32_t otgfs_doepint0_stuppktrcvd:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_doepint0_reg_t;


/** Group: Device OUT Endpoint 0 Transfer Size Register */
/** Type of otgfs_doeptsiz0 register
 *  Device OUT Endpoint 0 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_doeptsiz0_xfersize : R/W; bitpos: [6:0]; default: 0;
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
        uint32_t otgfs_doeptsiz0_xfersize:7;
        uint32_t reserved_7:12;
        /** otgfs_doeptsiz0_pktcnt : R/W; bitpos: [19]; default: 0;
         *  Packet Count (PktCnt)
         *
         *  This field is decremented to zero after a packet is written into the RxFIFO.
         */
        uint32_t otgfs_doeptsiz0_pktcnt:1;
        uint32_t reserved_20:9;
        /** otgfs_doeptsiz0_supcnt : R/W; bitpos: [30:29]; default: 0;
         *  SETUP Packet Count (SUPCnt)
         *
         *  This field specifies the number of back-to-back SETUP data packets the endpoint can
         *  receive.
         *  - 2'b01: 1 packet
         *  - 2'b10: 2 packets
         *  - 2'b11: 3 packets
         */
        uint32_t otgfs_doeptsiz0_supcnt:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_doeptsiz0_reg_t;


/** Group: Device OUT Endpoint 0 DMA Address Register */
/** Type of otgfs_doepdma0 register
 *  Device OUT Endpoint 0 DMA Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdma0_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_doepdma0_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdma0_reg_t;


/** Group: Device OUT Endpoint 16 Buffer Address Register */
/** Type of otgfs_doepdmab0 register
 *  Device OUT Endpoint 16 Buffer Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdmab0_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_doepdmab0_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdmab0_reg_t;


/** Group: Device Control OUT Endpoint 1 Control Register */
/** Type of otgfs_doepctl1 register
 *  Device Control OUT Endpoint 1 Control Register
 */
typedef union {
    struct {
        /** otgfs_doepctl1_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the current
         *  logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_doepctl1_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_doepctl1_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_doepctl1_usbactep:1;
        /** otgfs_doepctl1_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_doepctl1_dpid:1;
        /** otgfs_doepctl1_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_doepctl1_naksts:1;
        /** otgfs_doepctl1_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_doepctl1_eptype:2;
        /** otgfs_doepctl1_snp : R/W; bitpos: [20]; default: 0;
         *  RESERVED
         */
        uint32_t otgfs_doepctl1_snp:1;
        /** otgfs_doepctl1_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_doepctl1_stall:1;
        uint32_t reserved_22:4;
        /** otgfs_doepctl1_cnak : R/W; bitpos: [26]; default: 0;
         *
         *  Clear NAK (CNAK)
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_doepctl1_cnak:1;
        /** otgfs_doepctl1_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK
         *  handshakes on an endpoint. The core can also set this bit for an
         *  endpoint after a SETUP packet is received on that endpoint.
         */
        uint32_t otgfs_doepctl1_snak:1;
        /** otgfs_doepctl1_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_doepctl1_setd0pid:1;
        /** otgfs_doepctl1_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_doepctl1_setd1pid:1;
        /** otgfs_doepctl1_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_doepctl1_epdis:1;
        /** otgfs_doepctl1_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_doepctl1_epena:1;
    };
    uint32_t val;
} usb_otgfs_doepctl1_reg_t;


/** Group: Device OUT Endpoint 1 Interrupt Register */
/** Type of otgfs_doepint1 register
 *  Device OUT Endpoint 1 Interrupt Register
 */
typedef union {
    struct {
        /** otgfs_doepint1_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_doepint1_xfercompl:1;
        /** otgfs_doepint1_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_doepint1_epdisbld:1;
        /** otgfs_doepint1_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_doepint1_ahberr:1;
        /** otgfs_doepint1_setup : R/W; bitpos: [3]; default: 0;
         *  SETUP Phase Done (SetUp)
         *
         *  Applies to control OUT endpoints only.
         *
         *  Indicates that the SETUP phase for the control endpoint is
         *  complete and no more back-to-back SETUP packets were
         *  received for the current control transfer. On this interrupt, the
         *  application can decode the received SETUP data packet.
         */
        uint32_t otgfs_doepint1_setup:1;
        /** otgfs_doepint1_outtknepdis : R/W; bitpos: [4]; default: 0;
         *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
         *
         *  Applies only to control OUT endpoints.
         *
         *  Indicates that an OUT token was received when the endpoint was not yet enabled.
         *  This interrupt is asserted on the endpoint for which the OUT token was received.
         */
        uint32_t otgfs_doepint1_outtknepdis:1;
        /** otgfs_doepint1_stsphsercvd : R/W; bitpos: [5]; default: 0;
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
        uint32_t otgfs_doepint1_stsphsercvd:1;
        /** otgfs_doepint1_back2backsetup : R/W; bitpos: [6]; default: 0;
         *  Back-to-Back SETUP Packets Received (Back2BackSETup)
         *
         *  Applies to Control OUT endpoints only.
         *
         *  This bit indicates that the core has received more than three
         *  back-to-back SETUP packets for this particular endpoint. For
         *  information about handling this interrupt,
         */
        uint32_t otgfs_doepint1_back2backsetup:1;
        uint32_t reserved_7:1;
        /** otgfs_doepint1_outpkterr : R/W; bitpos: [8]; default: 0;
         *  OUT Packet Error (OutPktErr)
         *
         *  Applies to OUT endpoints Only
         *
         *  This interrupt is valid only when thresholding is enabled. This interrupt is
         *  asserted when the
         *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
         */
        uint32_t otgfs_doepint1_outpkterr:1;
        /** otgfs_doepint1_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process, such as Host busy or DMA
         *  done
         */
        uint32_t otgfs_doepint1_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_doepint1_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_doepint1_pktdrpsts:1;
        /** otgfs_doepint1_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_doepint1_bbleerr:1;
        /** otgfs_doepint1_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_doepint1_nakintrpt:1;
        /** otgfs_doepint1_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_doepint1_nyetintrpt:1;
        /** otgfs_doepint1_stuppktrcvd : R/W; bitpos: [15]; default: 0;
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
        uint32_t otgfs_doepint1_stuppktrcvd:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_doepint1_reg_t;


/** Group: Device OUT Endpoint 1 Transfer Size Register */
/** Type of otgfs_doeptsiz1 register
 *  Device OUT Endpoint 1 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_doeptsiz1_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_doeptsiz1_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_doeptsiz1_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *  This field is decremented to zero after a packet is written into the RxFIFO.
         */
        uint32_t otgfs_doeptsiz1_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_doeptsiz1_rxdpid : RO; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_doeptsiz1_rxdpid:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_doeptsiz1_reg_t;


/** Group: Device OUT Endpoint 1 DMA Address Register */
/** Type of otgfs_doepdma1 register
 *  Device OUT Endpoint 1 DMA Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdma1_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_doepdma1_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdma1_reg_t;


/** Group: Device OUT Endpoint 1 Buffer Address Register */
/** Type of otgfs_doepdmab1 register
 *  Device OUT Endpoint 1 Buffer Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdmab1_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_doepdmab1_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdmab1_reg_t;


/** Group: Device Control OUT Endpoint 2 Control Register */
/** Type of otgfs_doepctl2 register
 *  Device Control OUT Endpoint 2 Control Register
 */
typedef union {
    struct {
        /** otgfs_doepctl2_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the current
         *  logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_doepctl2_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_doepctl2_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_doepctl2_usbactep:1;
        /** otgfs_doepctl2_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_doepctl2_dpid:1;
        /** otgfs_doepctl2_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_doepctl2_naksts:1;
        /** otgfs_doepctl2_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_doepctl2_eptype:2;
        /** otgfs_doepctl2_snp : R/W; bitpos: [20]; default: 0;
         *  RESERVED
         */
        uint32_t otgfs_doepctl2_snp:1;
        /** otgfs_doepctl2_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_doepctl2_stall:1;
        uint32_t reserved_22:4;
        /** otgfs_doepctl2_cnak : R/W; bitpos: [26]; default: 0;
         *
         *  Clear NAK (CNAK)
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_doepctl2_cnak:1;
        /** otgfs_doepctl2_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK
         *  handshakes on an endpoint. The core can also set this bit for an
         *  endpoint after a SETUP packet is received on that endpoint.
         */
        uint32_t otgfs_doepctl2_snak:1;
        /** otgfs_doepctl2_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_doepctl2_setd0pid:1;
        /** otgfs_doepctl2_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_doepctl2_setd1pid:1;
        /** otgfs_doepctl2_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_doepctl2_epdis:1;
        /** otgfs_doepctl2_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_doepctl2_epena:1;
    };
    uint32_t val;
} usb_otgfs_doepctl2_reg_t;


/** Group: Device OUT Endpoint 2 Interrupt Register */
/** Type of otgfs_doepint2 register
 *  Device OUT Endpoint 2 Interrupt Register
 */
typedef union {
    struct {
        /** otgfs_doepint2_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_doepint2_xfercompl:1;
        /** otgfs_doepint2_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_doepint2_epdisbld:1;
        /** otgfs_doepint2_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_doepint2_ahberr:1;
        /** otgfs_doepint2_setup : R/W; bitpos: [3]; default: 0;
         *  SETUP Phase Done (SetUp)
         *
         *  Applies to control OUT endpoints only.
         *
         *  Indicates that the SETUP phase for the control endpoint is
         *  complete and no more back-to-back SETUP packets were
         *  received for the current control transfer. On this interrupt, the
         *  application can decode the received SETUP data packet.
         */
        uint32_t otgfs_doepint2_setup:1;
        /** otgfs_doepint2_outtknepdis : R/W; bitpos: [4]; default: 0;
         *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
         *
         *  Applies only to control OUT endpoints.
         *
         *  Indicates that an OUT token was received when the endpoint was not yet enabled.
         *  This interrupt is asserted on the endpoint for which the OUT token was received.
         */
        uint32_t otgfs_doepint2_outtknepdis:1;
        /** otgfs_doepint2_stsphsercvd : R/W; bitpos: [5]; default: 0;
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
        uint32_t otgfs_doepint2_stsphsercvd:1;
        /** otgfs_doepint2_back2backsetup : R/W; bitpos: [6]; default: 0;
         *  Back-to-Back SETUP Packets Received (Back2BackSETup)
         *
         *  Applies to Control OUT endpoints only.
         *
         *  This bit indicates that the core has received more than three
         *  back-to-back SETUP packets for this particular endpoint. For
         *  information about handling this interrupt,
         */
        uint32_t otgfs_doepint2_back2backsetup:1;
        uint32_t reserved_7:1;
        /** otgfs_doepint2_outpkterr : R/W; bitpos: [8]; default: 0;
         *  OUT Packet Error (OutPktErr)
         *
         *  Applies to OUT endpoints Only
         *
         *  This interrupt is valid only when thresholding is enabled. This interrupt is
         *  asserted when the
         *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
         */
        uint32_t otgfs_doepint2_outpkterr:1;
        /** otgfs_doepint2_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process, such as Host busy or DMA
         *  done
         */
        uint32_t otgfs_doepint2_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_doepint2_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_doepint2_pktdrpsts:1;
        /** otgfs_doepint2_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_doepint2_bbleerr:1;
        /** otgfs_doepint2_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_doepint2_nakintrpt:1;
        /** otgfs_doepint2_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_doepint2_nyetintrpt:1;
        /** otgfs_doepint2_stuppktrcvd : R/W; bitpos: [15]; default: 0;
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
        uint32_t otgfs_doepint2_stuppktrcvd:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_doepint2_reg_t;


/** Group: Device OUT Endpoint 2 Transfer Size Register */
/** Type of otgfs_doeptsiz2 register
 *  Device OUT Endpoint 2 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_doeptsiz2_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_doeptsiz2_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_doeptsiz2_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *  This field is decremented to zero after a packet is written into the RxFIFO.
         */
        uint32_t otgfs_doeptsiz2_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_doeptsiz2_rxdpid : RO; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_doeptsiz2_rxdpid:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_doeptsiz2_reg_t;


/** Group: Device OUT Endpoint 2 DMA Address Register */
/** Type of otgfs_doepdma2 register
 *  Device OUT Endpoint 2 DMA Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdma2_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_doepdma2_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdma2_reg_t;


/** Group: Device OUT Endpoint 2 Buffer Address Register */
/** Type of otgfs_doepdmab2 register
 *  Device OUT Endpoint 2 Buffer Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdmab2_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_doepdmab2_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdmab2_reg_t;


/** Group: Device Control OUT Endpoint 3 Control Register */
/** Type of otgfs_doepctl3 register
 *  Device Control OUT Endpoint 3 Control Register
 */
typedef union {
    struct {
        /** otgfs_doepctl3_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the current
         *  logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_doepctl3_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_doepctl3_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_doepctl3_usbactep:1;
        /** otgfs_doepctl3_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_doepctl3_dpid:1;
        /** otgfs_doepctl3_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_doepctl3_naksts:1;
        /** otgfs_doepctl3_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_doepctl3_eptype:2;
        /** otgfs_doepctl3_snp : R/W; bitpos: [20]; default: 0;
         *  RESERVED
         */
        uint32_t otgfs_doepctl3_snp:1;
        /** otgfs_doepctl3_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_doepctl3_stall:1;
        uint32_t reserved_22:4;
        /** otgfs_doepctl3_cnak : R/W; bitpos: [26]; default: 0;
         *
         *  Clear NAK (CNAK)
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_doepctl3_cnak:1;
        /** otgfs_doepctl3_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK
         *  handshakes on an endpoint. The core can also set this bit for an
         *  endpoint after a SETUP packet is received on that endpoint.
         */
        uint32_t otgfs_doepctl3_snak:1;
        /** otgfs_doepctl3_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_doepctl3_setd0pid:1;
        /** otgfs_doepctl3_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_doepctl3_setd1pid:1;
        /** otgfs_doepctl3_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_doepctl3_epdis:1;
        /** otgfs_doepctl3_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_doepctl3_epena:1;
    };
    uint32_t val;
} usb_otgfs_doepctl3_reg_t;


/** Group: Device OUT Endpoint 3 Interrupt Register */
/** Type of otgfs_doepint3 register
 *  Device OUT Endpoint 3 Interrupt Register
 */
typedef union {
    struct {
        /** otgfs_doepint3_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_doepint3_xfercompl:1;
        /** otgfs_doepint3_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_doepint3_epdisbld:1;
        /** otgfs_doepint3_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_doepint3_ahberr:1;
        /** otgfs_doepint3_setup : R/W; bitpos: [3]; default: 0;
         *  SETUP Phase Done (SetUp)
         *
         *  Applies to control OUT endpoints only.
         *
         *  Indicates that the SETUP phase for the control endpoint is
         *  complete and no more back-to-back SETUP packets were
         *  received for the current control transfer. On this interrupt, the
         *  application can decode the received SETUP data packet.
         */
        uint32_t otgfs_doepint3_setup:1;
        /** otgfs_doepint3_outtknepdis : R/W; bitpos: [4]; default: 0;
         *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
         *
         *  Applies only to control OUT endpoints.
         *
         *  Indicates that an OUT token was received when the endpoint was not yet enabled.
         *  This interrupt is asserted on the endpoint for which the OUT token was received.
         */
        uint32_t otgfs_doepint3_outtknepdis:1;
        /** otgfs_doepint3_stsphsercvd : R/W; bitpos: [5]; default: 0;
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
        uint32_t otgfs_doepint3_stsphsercvd:1;
        /** otgfs_doepint3_back2backsetup : R/W; bitpos: [6]; default: 0;
         *  Back-to-Back SETUP Packets Received (Back2BackSETup)
         *
         *  Applies to Control OUT endpoints only.
         *
         *  This bit indicates that the core has received more than three
         *  back-to-back SETUP packets for this particular endpoint. For
         *  information about handling this interrupt,
         */
        uint32_t otgfs_doepint3_back2backsetup:1;
        uint32_t reserved_7:1;
        /** otgfs_doepint3_outpkterr : R/W; bitpos: [8]; default: 0;
         *  OUT Packet Error (OutPktErr)
         *
         *  Applies to OUT endpoints Only
         *
         *  This interrupt is valid only when thresholding is enabled. This interrupt is
         *  asserted when the
         *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
         */
        uint32_t otgfs_doepint3_outpkterr:1;
        /** otgfs_doepint3_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process, such as Host busy or DMA
         *  done
         */
        uint32_t otgfs_doepint3_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_doepint3_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_doepint3_pktdrpsts:1;
        /** otgfs_doepint3_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_doepint3_bbleerr:1;
        /** otgfs_doepint3_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_doepint3_nakintrpt:1;
        /** otgfs_doepint3_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_doepint3_nyetintrpt:1;
        /** otgfs_doepint3_stuppktrcvd : R/W; bitpos: [15]; default: 0;
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
        uint32_t otgfs_doepint3_stuppktrcvd:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_doepint3_reg_t;


/** Group: Device OUT Endpoint 3 Transfer Size Register */
/** Type of otgfs_doeptsiz3 register
 *  Device OUT Endpoint 3 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_doeptsiz3_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_doeptsiz3_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_doeptsiz3_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *  This field is decremented to zero after a packet is written into the RxFIFO.
         */
        uint32_t otgfs_doeptsiz3_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_doeptsiz3_rxdpid : RO; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_doeptsiz3_rxdpid:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_doeptsiz3_reg_t;


/** Group: Device OUT Endpoint 3 DMA Address Register */
/** Type of otgfs_doepdma3 register
 *  Device OUT Endpoint 3 DMA Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdma3_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_doepdma3_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdma3_reg_t;


/** Group: Device OUT Endpoint 3 Buffer Address Register */
/** Type of otgfs_doepdmab3 register
 *  Device OUT Endpoint 3 Buffer Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdmab3_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_doepdmab3_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdmab3_reg_t;


/** Group: Device Control OUT Endpoint 4 Control Register */
/** Type of otgfs_doepctl4 register
 *  Device Control OUT Endpoint 4 Control Register
 */
typedef union {
    struct {
        /** otgfs_doepctl4_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the current
         *  logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_doepctl4_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_doepctl4_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_doepctl4_usbactep:1;
        /** otgfs_doepctl4_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_doepctl4_dpid:1;
        /** otgfs_doepctl4_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_doepctl4_naksts:1;
        /** otgfs_doepctl4_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_doepctl4_eptype:2;
        /** otgfs_doepctl4_snp : R/W; bitpos: [20]; default: 0;
         *  RESERVED
         */
        uint32_t otgfs_doepctl4_snp:1;
        /** otgfs_doepctl4_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_doepctl4_stall:1;
        uint32_t reserved_22:4;
        /** otgfs_doepctl4_cnak : R/W; bitpos: [26]; default: 0;
         *
         *  Clear NAK (CNAK)
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_doepctl4_cnak:1;
        /** otgfs_doepctl4_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK
         *  handshakes on an endpoint. The core can also set this bit for an
         *  endpoint after a SETUP packet is received on that endpoint.
         */
        uint32_t otgfs_doepctl4_snak:1;
        /** otgfs_doepctl4_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_doepctl4_setd0pid:1;
        /** otgfs_doepctl4_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_doepctl4_setd1pid:1;
        /** otgfs_doepctl4_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_doepctl4_epdis:1;
        /** otgfs_doepctl4_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_doepctl4_epena:1;
    };
    uint32_t val;
} usb_otgfs_doepctl4_reg_t;


/** Group: Device OUT Endpoint 4 Interrupt Register */
/** Type of otgfs_doepint4 register
 *  Device OUT Endpoint 4 Interrupt Register
 */
typedef union {
    struct {
        /** otgfs_doepint4_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_doepint4_xfercompl:1;
        /** otgfs_doepint4_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_doepint4_epdisbld:1;
        /** otgfs_doepint4_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_doepint4_ahberr:1;
        /** otgfs_doepint4_setup : R/W; bitpos: [3]; default: 0;
         *  SETUP Phase Done (SetUp)
         *
         *  Applies to control OUT endpoints only.
         *
         *  Indicates that the SETUP phase for the control endpoint is
         *  complete and no more back-to-back SETUP packets were
         *  received for the current control transfer. On this interrupt, the
         *  application can decode the received SETUP data packet.
         */
        uint32_t otgfs_doepint4_setup:1;
        /** otgfs_doepint4_outtknepdis : R/W; bitpos: [4]; default: 0;
         *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
         *
         *  Applies only to control OUT endpoints.
         *
         *  Indicates that an OUT token was received when the endpoint was not yet enabled.
         *  This interrupt is asserted on the endpoint for which the OUT token was received.
         */
        uint32_t otgfs_doepint4_outtknepdis:1;
        /** otgfs_doepint4_stsphsercvd : R/W; bitpos: [5]; default: 0;
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
        uint32_t otgfs_doepint4_stsphsercvd:1;
        /** otgfs_doepint4_back2backsetup : R/W; bitpos: [6]; default: 0;
         *  Back-to-Back SETUP Packets Received (Back2BackSETup)
         *
         *  Applies to Control OUT endpoints only.
         *
         *  This bit indicates that the core has received more than three
         *  back-to-back SETUP packets for this particular endpoint. For
         *  information about handling this interrupt,
         */
        uint32_t otgfs_doepint4_back2backsetup:1;
        uint32_t reserved_7:1;
        /** otgfs_doepint4_outpkterr : R/W; bitpos: [8]; default: 0;
         *  OUT Packet Error (OutPktErr)
         *
         *  Applies to OUT endpoints Only
         *
         *  This interrupt is valid only when thresholding is enabled. This interrupt is
         *  asserted when the
         *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
         */
        uint32_t otgfs_doepint4_outpkterr:1;
        /** otgfs_doepint4_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process, such as Host busy or DMA
         *  done
         */
        uint32_t otgfs_doepint4_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_doepint4_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_doepint4_pktdrpsts:1;
        /** otgfs_doepint4_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_doepint4_bbleerr:1;
        /** otgfs_doepint4_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_doepint4_nakintrpt:1;
        /** otgfs_doepint4_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_doepint4_nyetintrpt:1;
        /** otgfs_doepint4_stuppktrcvd : R/W; bitpos: [15]; default: 0;
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
        uint32_t otgfs_doepint4_stuppktrcvd:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_doepint4_reg_t;


/** Group: Device OUT Endpoint 4 Transfer Size Register */
/** Type of otgfs_doeptsiz4 register
 *  Device OUT Endpoint 4 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_doeptsiz4_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_doeptsiz4_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_doeptsiz4_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *  This field is decremented to zero after a packet is written into the RxFIFO.
         */
        uint32_t otgfs_doeptsiz4_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_doeptsiz4_rxdpid : RO; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_doeptsiz4_rxdpid:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_doeptsiz4_reg_t;


/** Group: Device OUT Endpoint 4 DMA Address Register */
/** Type of otgfs_doepdma4 register
 *  Device OUT Endpoint 4 DMA Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdma4_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_doepdma4_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdma4_reg_t;


/** Group: Device OUT Endpoint 4 Buffer Address Register */
/** Type of otgfs_doepdmab4 register
 *  Device OUT Endpoint 4 Buffer Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdmab4_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_doepdmab4_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdmab4_reg_t;


/** Group: Device Control OUT Endpoint 5 Control Register */
/** Type of otgfs_doepctl5 register
 *  Device Control OUT Endpoint 5 Control Register
 */
typedef union {
    struct {
        /** otgfs_doepctl5_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the current
         *  logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_doepctl5_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_doepctl5_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_doepctl5_usbactep:1;
        /** otgfs_doepctl5_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_doepctl5_dpid:1;
        /** otgfs_doepctl5_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_doepctl5_naksts:1;
        /** otgfs_doepctl5_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_doepctl5_eptype:2;
        /** otgfs_doepctl5_snp : R/W; bitpos: [20]; default: 0;
         *  RESERVED
         */
        uint32_t otgfs_doepctl5_snp:1;
        /** otgfs_doepctl5_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_doepctl5_stall:1;
        uint32_t reserved_22:4;
        /** otgfs_doepctl5_cnak : R/W; bitpos: [26]; default: 0;
         *
         *  Clear NAK (CNAK)
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_doepctl5_cnak:1;
        /** otgfs_doepctl5_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK
         *  handshakes on an endpoint. The core can also set this bit for an
         *  endpoint after a SETUP packet is received on that endpoint.
         */
        uint32_t otgfs_doepctl5_snak:1;
        /** otgfs_doepctl5_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_doepctl5_setd0pid:1;
        /** otgfs_doepctl5_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_doepctl5_setd1pid:1;
        /** otgfs_doepctl5_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_doepctl5_epdis:1;
        /** otgfs_doepctl5_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_doepctl5_epena:1;
    };
    uint32_t val;
} usb_otgfs_doepctl5_reg_t;


/** Group: Device OUT Endpoint 5 Interrupt Register */
/** Type of otgfs_doepint5 register
 *  Device OUT Endpoint 5 Interrupt Register
 */
typedef union {
    struct {
        /** otgfs_doepint5_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_doepint5_xfercompl:1;
        /** otgfs_doepint5_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_doepint5_epdisbld:1;
        /** otgfs_doepint5_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_doepint5_ahberr:1;
        /** otgfs_doepint5_setup : R/W; bitpos: [3]; default: 0;
         *  SETUP Phase Done (SetUp)
         *
         *  Applies to control OUT endpoints only.
         *
         *  Indicates that the SETUP phase for the control endpoint is
         *  complete and no more back-to-back SETUP packets were
         *  received for the current control transfer. On this interrupt, the
         *  application can decode the received SETUP data packet.
         */
        uint32_t otgfs_doepint5_setup:1;
        /** otgfs_doepint5_outtknepdis : R/W; bitpos: [4]; default: 0;
         *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
         *
         *  Applies only to control OUT endpoints.
         *
         *  Indicates that an OUT token was received when the endpoint was not yet enabled.
         *  This interrupt is asserted on the endpoint for which the OUT token was received.
         */
        uint32_t otgfs_doepint5_outtknepdis:1;
        /** otgfs_doepint5_stsphsercvd : R/W; bitpos: [5]; default: 0;
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
        uint32_t otgfs_doepint5_stsphsercvd:1;
        /** otgfs_doepint5_back2backsetup : R/W; bitpos: [6]; default: 0;
         *  Back-to-Back SETUP Packets Received (Back2BackSETup)
         *
         *  Applies to Control OUT endpoints only.
         *
         *  This bit indicates that the core has received more than three
         *  back-to-back SETUP packets for this particular endpoint. For
         *  information about handling this interrupt,
         */
        uint32_t otgfs_doepint5_back2backsetup:1;
        uint32_t reserved_7:1;
        /** otgfs_doepint5_outpkterr : R/W; bitpos: [8]; default: 0;
         *  OUT Packet Error (OutPktErr)
         *
         *  Applies to OUT endpoints Only
         *
         *  This interrupt is valid only when thresholding is enabled. This interrupt is
         *  asserted when the
         *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
         */
        uint32_t otgfs_doepint5_outpkterr:1;
        /** otgfs_doepint5_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process, such as Host busy or DMA
         *  done
         */
        uint32_t otgfs_doepint5_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_doepint5_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_doepint5_pktdrpsts:1;
        /** otgfs_doepint5_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_doepint5_bbleerr:1;
        /** otgfs_doepint5_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_doepint5_nakintrpt:1;
        /** otgfs_doepint5_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_doepint5_nyetintrpt:1;
        /** otgfs_doepint5_stuppktrcvd : R/W; bitpos: [15]; default: 0;
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
        uint32_t otgfs_doepint5_stuppktrcvd:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_doepint5_reg_t;


/** Group: Device OUT Endpoint 5 Transfer Size Register */
/** Type of otgfs_doeptsiz5 register
 *  Device OUT Endpoint 5 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_doeptsiz5_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_doeptsiz5_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_doeptsiz5_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *  This field is decremented to zero after a packet is written into the RxFIFO.
         */
        uint32_t otgfs_doeptsiz5_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_doeptsiz5_rxdpid : RO; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_doeptsiz5_rxdpid:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_doeptsiz5_reg_t;


/** Group: Device OUT Endpoint 5 DMA Address Register */
/** Type of otgfs_doepdma5 register
 *  Device OUT Endpoint 5 DMA Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdma5_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_doepdma5_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdma5_reg_t;


/** Group: Device OUT Endpoint 5 Buffer Address Register */
/** Type of otgfs_doepdmab5 register
 *  Device OUT Endpoint 5 Buffer Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdmab5_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_doepdmab5_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdmab5_reg_t;


/** Group: Device Control OUT Endpoint 6 Control Register */
/** Type of otgfs_doepctl6 register
 *  Device Control OUT Endpoint 6 Control Register
 */
typedef union {
    struct {
        /** otgfs_doepctl6_mps : R/W; bitpos: [10:0]; default: 0;
         *  Maximum Packet Size (MPS)
         *
         *  The application must program this field with the maximum packet size for the current
         *  logical endpoint. This value is in bytes.
         */
        uint32_t otgfs_doepctl6_mps:11;
        uint32_t reserved_11:4;
        /** otgfs_doepctl6_usbactep : R/W; bitpos: [15]; default: 0;
         *  USB Active Endpoint (USBActEP)
         *
         *  Indicates whether this endpoint is active in the current configuration and
         *  interface. The
         *  core clears this bit for all endpoints (other than EP 0) after detecting a USB
         *  reset. After
         *  receiving the SetConfiguration and SetInterface commands, the application must
         *  program endpoint registers accordingly and set this bit.
         */
        uint32_t otgfs_doepctl6_usbactep:1;
        /** otgfs_doepctl6_dpid : RO; bitpos: [16]; default: 0;
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
        uint32_t otgfs_doepctl6_dpid:1;
        /** otgfs_doepctl6_naksts : RO; bitpos: [17]; default: 0;
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
        uint32_t otgfs_doepctl6_naksts:1;
        /** otgfs_doepctl6_eptype : R/W; bitpos: [19:18]; default: 0;
         *  Endpoint Type (EPType)
         *
         *  This is the transfer type supported by this logical endpoint.
         *  - 2'b00: Control
         *  - 2'b01: Isochronous
         *  - 2'b10: Bulk
         *  - 2'b11: Interrupt
         */
        uint32_t otgfs_doepctl6_eptype:2;
        /** otgfs_doepctl6_snp : R/W; bitpos: [20]; default: 0;
         *  RESERVED
         */
        uint32_t otgfs_doepctl6_snp:1;
        /** otgfs_doepctl6_stall : R/W; bitpos: [21]; default: 0;
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
        uint32_t otgfs_doepctl6_stall:1;
        uint32_t reserved_22:4;
        /** otgfs_doepctl6_cnak : R/W; bitpos: [26]; default: 0;
         *
         *  Clear NAK (CNAK)
         *  A write to this bit clears the NAK bit for the endpoint.
         */
        uint32_t otgfs_doepctl6_cnak:1;
        /** otgfs_doepctl6_snak : R/W; bitpos: [27]; default: 0;
         *  Set NAK (SNAK)
         *
         *  A write to this bit sets the NAK bit for the endpoint.
         *
         *  Using this bit, the application can control the transmission of NAK
         *  handshakes on an endpoint. The core can also set this bit for an
         *  endpoint after a SETUP packet is received on that endpoint.
         */
        uint32_t otgfs_doepctl6_snak:1;
        /** otgfs_doepctl6_setd0pid : R/W; bitpos: [28]; default: 0;
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
        uint32_t otgfs_doepctl6_setd0pid:1;
        /** otgfs_doepctl6_setd1pid : R/W; bitpos: [29]; default: 0;
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
        uint32_t otgfs_doepctl6_setd1pid:1;
        /** otgfs_doepctl6_epdis : R/W; bitpos: [30]; default: 0;
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
        uint32_t otgfs_doepctl6_epdis:1;
        /** otgfs_doepctl6_epena : R/W; bitpos: [31]; default: 0;
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
        uint32_t otgfs_doepctl6_epena:1;
    };
    uint32_t val;
} usb_otgfs_doepctl6_reg_t;


/** Group: Device OUT Endpoint 6 Interrupt Register */
/** Type of otgfs_doepint6 register
 *  Device OUT Endpoint 6 Interrupt Register
 */
typedef union {
    struct {
        /** otgfs_doepint6_xfercompl : R/W; bitpos: [0]; default: 0;
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
        uint32_t otgfs_doepint6_xfercompl:1;
        /** otgfs_doepint6_epdisbld : R/W; bitpos: [1]; default: 0;
         *  Endpoint Disabled Interrupt (EPDisbld)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This bit indicates that the endpoint is disabled per the application's request.
         */
        uint32_t otgfs_doepint6_epdisbld:1;
        /** otgfs_doepint6_ahberr : R/W; bitpos: [2]; default: 0;
         *  AHB Error (AHBErr)
         *
         *  Applies to IN and OUT endpoints.
         *
         *  This is generated only in Internal DMA mode when there is an
         *  AHB error during an AHB read/write. The application can read
         *  the corresponding endpoint DMA address register to get the
         *  error address.
         */
        uint32_t otgfs_doepint6_ahberr:1;
        /** otgfs_doepint6_setup : R/W; bitpos: [3]; default: 0;
         *  SETUP Phase Done (SetUp)
         *
         *  Applies to control OUT endpoints only.
         *
         *  Indicates that the SETUP phase for the control endpoint is
         *  complete and no more back-to-back SETUP packets were
         *  received for the current control transfer. On this interrupt, the
         *  application can decode the received SETUP data packet.
         */
        uint32_t otgfs_doepint6_setup:1;
        /** otgfs_doepint6_outtknepdis : R/W; bitpos: [4]; default: 0;
         *  OUT Token Received When Endpoint Disabled (OUTTknEPdis)
         *
         *  Applies only to control OUT endpoints.
         *
         *  Indicates that an OUT token was received when the endpoint was not yet enabled.
         *  This interrupt is asserted on the endpoint for which the OUT token was received.
         */
        uint32_t otgfs_doepint6_outtknepdis:1;
        /** otgfs_doepint6_stsphsercvd : R/W; bitpos: [5]; default: 0;
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
        uint32_t otgfs_doepint6_stsphsercvd:1;
        /** otgfs_doepint6_back2backsetup : R/W; bitpos: [6]; default: 0;
         *  Back-to-Back SETUP Packets Received (Back2BackSETup)
         *
         *  Applies to Control OUT endpoints only.
         *
         *  This bit indicates that the core has received more than three
         *  back-to-back SETUP packets for this particular endpoint. For
         *  information about handling this interrupt,
         */
        uint32_t otgfs_doepint6_back2backsetup:1;
        uint32_t reserved_7:1;
        /** otgfs_doepint6_outpkterr : R/W; bitpos: [8]; default: 0;
         *  OUT Packet Error (OutPktErr)
         *
         *  Applies to OUT endpoints Only
         *
         *  This interrupt is valid only when thresholding is enabled. This interrupt is
         *  asserted when the
         *  core detects an overflow or a CRC error for non-Isochronous OUT packet.
         */
        uint32_t otgfs_doepint6_outpkterr:1;
        /** otgfs_doepint6_bnaintr : R/W; bitpos: [9]; default: 0;
         *  BNA (Buffer Not Available) Interrupt (BNAIntr)
         *
         *  This bit is valid only when Scatter/Gather DMA mode is enabled.
         *
         *  The core generates this interrupt when the descriptor accessed
         *  is not ready for the Core to process, such as Host busy or DMA
         *  done
         */
        uint32_t otgfs_doepint6_bnaintr:1;
        uint32_t reserved_10:1;
        /** otgfs_doepint6_pktdrpsts : R/W; bitpos: [11]; default: 0;
         *  Packet Drop Status (PktDrpSts)
         *
         *  This bit indicates to the application that an ISOC OUT packet has been dropped. This
         *  bit does not have an associated mask bit and does not generate an interrupt.
         *
         *  Dependency: This bit is valid in non Scatter/Gather DMA mode when periodic transfer
         *  interrupt feature is selected.
         */
        uint32_t otgfs_doepint6_pktdrpsts:1;
        /** otgfs_doepint6_bbleerr : R/W; bitpos: [12]; default: 0;
         *  NAK Interrupt (BbleErr)
         *
         *  The core generates this interrupt when babble is received for the endpoint.
         */
        uint32_t otgfs_doepint6_bbleerr:1;
        /** otgfs_doepint6_nakintrpt : R/W; bitpos: [13]; default: 0;
         *  NAK Interrupt (NAKInterrupt)
         *
         *  The core generates this interrupt when a NAK is transmitted or received by the
         *  device.
         *
         *  In case of isochronous IN endpoints the interrupt gets generated when a zero length
         *  packet is transmitted due to un-availability of data in the TXFifo.
         */
        uint32_t otgfs_doepint6_nakintrpt:1;
        /** otgfs_doepint6_nyetintrpt : R/W; bitpos: [14]; default: 0;
         *  NYET Interrupt (NYETIntrpt)
         *
         *  The core generates this interrupt when a NYET response is transmitted for a non
         *  isochronous OUT endpoint.
         */
        uint32_t otgfs_doepint6_nyetintrpt:1;
        /** otgfs_doepint6_stuppktrcvd : R/W; bitpos: [15]; default: 0;
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
        uint32_t otgfs_doepint6_stuppktrcvd:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_otgfs_doepint6_reg_t;


/** Group: Device OUT Endpoint 6 Transfer Size Register */
/** Type of otgfs_doeptsiz6 register
 *  Device OUT Endpoint 6 Transfer Size Register
 */
typedef union {
    struct {
        /** otgfs_doeptsiz6_xfersize : R/W; bitpos: [15:0]; default: 0;
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
        uint32_t otgfs_doeptsiz6_xfersize:16;
        uint32_t reserved_16:3;
        /** otgfs_doeptsiz6_pktcnt : R/W; bitpos: [25:19]; default: 0;
         *  Packet Count (PktCnt)
         *  This field is decremented to zero after a packet is written into the RxFIFO.
         */
        uint32_t otgfs_doeptsiz6_pktcnt:7;
        uint32_t reserved_26:3;
        /** otgfs_doeptsiz6_rxdpid : RO; bitpos: [30:29]; default: 0;
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
        uint32_t otgfs_doeptsiz6_rxdpid:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} usb_otgfs_doeptsiz6_reg_t;


/** Group: Device OUT Endpoint 6 DMA Address Register */
/** Type of otgfs_doepdma6 register
 *  Device OUT Endpoint 6 DMA Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdma6_dmaaddr : R/W; bitpos: [31:0]; default: 0;
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
        uint32_t otgfs_doepdma6_dmaaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdma6_reg_t;


/** Group: Device OUT Endpoint 6 Buffer Address Register */
/** Type of otgfs_doepdmab6 register
 *  Device OUT Endpoint 6 Buffer Address Register
 */
typedef union {
    struct {
        /** otgfs_doepdmab6_dmabufferaddr : RO; bitpos: [31:0]; default: 0;
         *  Holds the current buffer address.This register is updated as and when the data
         *  transfer for the corresponding end point is in progress.
         *
         *  This register is present only in Scatter/Gather DMA mode. Otherwise this field is
         *  reserved.
         */
        uint32_t otgfs_doepdmab6_dmabufferaddr:32;
    };
    uint32_t val;
} usb_otgfs_doepdmab6_reg_t;


/** Group: Power and Clock Gating Control Register */
/** Type of otgfs_pcgcctl register
 *  Power and Clock Gating Control Register
 */
typedef union {
    struct {
        /** otgfs_pcgcctl_stoppclk : R/W; bitpos: [0]; default: 0;
         *  Stop Pclk (StopPclk)
         *  - The application sets this bit to stop the PHY clock (phy_clk) when the USB is
         *  suspended, the session is not valid, or the device is disconnected.
         *  - The application clears this bit when the USB is resumed or a new session starts.
         */
        uint32_t otgfs_pcgcctl_stoppclk:1;
        /** otgfs_pcgcctl_gatehclk : R/W; bitpos: [1]; default: 0;
         *  Gate Hclk (GateHclk)
         *  - The application sets this bit to gate hclk to modules other than the AHB Slave
         *  and Master and wakeup logic when the USB is suspended or the session is not valid.
         *  - The application clears this bit when the USB is resumed or a new session starts.
         */
        uint32_t otgfs_pcgcctl_gatehclk:1;
        /** otgfs_pcgcctl_pwrclmp : R/W; bitpos: [2]; default: 0;
         *  Power Clamp (PwrClmp)
         *
         *  This bit is valid only in Partial Power-Down mode
         *  - The application sets this bit before the power is turned off to clamp the signals
         *  between the power-on modules and the power-off modules.
         *  - The application clears the bit to disable the clamping before the power is turned
         *  on.
         */
        uint32_t otgfs_pcgcctl_pwrclmp:1;
        /** otgfs_pcgcctl_rstpdwnmodule : R/W; bitpos: [3]; default: 0;
         *  Reset Power-Down Modules (RstPdwnModule)
         *
         *  This bit is valid only in Partial Power-Down mode.
         *  - The application sets this bit when the power is turned off.
         *  - The application clears this bit after the power is turned on and the PHY clock is
         *  up.
         *
         *  Note: The R/W of all core registers are possible only when this bit is set to 1b0.
         */
        uint32_t otgfs_pcgcctl_rstpdwnmodule:1;
        uint32_t reserved_4:2;
        /** otgfs_pcgcctl_physleep : RO; bitpos: [6]; default: 0;
         *  PHY In Sleep
         *
         *  Indicates that the PHY is in Sleep State.
         */
        uint32_t otgfs_pcgcctl_physleep:1;
        /** otgfs_pcgcctl_l1suspended : RO; bitpos: [7]; default: 0;
         *  L1 Deep Sleep
         *
         *  Indicates that the PHY is in deep sleep when in L1 state.
         */
        uint32_t otgfs_pcgcctl_l1suspended:1;
        /** otgfs_pcgcctl_resetaftersusp : R/W; bitpos: [8]; default: 0;
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
        uint32_t otgfs_pcgcctl_resetaftersusp:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} usb_otgfs_pcgcctl_reg_t;


typedef struct {
    volatile usb_otgfs_gotgctl_reg_t otgfs_gotgctl;
    volatile usb_otgfs_gotgint_reg_t otgfs_gotgint;
    volatile usb_otgfs_gahbcfg_reg_t otgfs_gahbcfg;
    volatile usb_otgfs_gusbcfg_reg_t otgfs_gusbcfg;
    volatile usb_otgfs_grstctl_reg_t otgfs_grstctl;
    volatile usb_otgfs_gintsts_reg_t otgfs_gintsts;
    volatile usb_otgfs_gintmsk_reg_t otgfs_gintmsk;
    volatile usb_otgfs_grxstsr_reg_t otgfs_grxstsr;
    volatile usb_otgfs_grxstsp_reg_t otgfs_grxstsp;
    volatile usb_otgfs_grxfsiz_reg_t otgfs_grxfsiz;
    volatile usb_otgfs_gnptxfsiz_reg_t otgfs_gnptxfsiz;
    volatile usb_otgfs_gnptxsts_reg_t otgfs_gnptxsts;
    uint32_t reserved_030[4];
    volatile usb_otgfs_gsnpsid_reg_t otgfs_gsnpsid;
    volatile usb_otgfs_ghwcfg1_reg_t otgfs_ghwcfg1;
    volatile usb_otgfs_ghwcfg2_reg_t otgfs_ghwcfg2;
    volatile usb_otgfs_ghwcfg3_reg_t otgfs_ghwcfg3;
    volatile usb_otgfs_ghwcfg4_reg_t otgfs_ghwcfg4;
    uint32_t reserved_054[2];
    volatile usb_otgfs_gdfifocfg_reg_t otgfs_gdfifocfg;
    uint32_t reserved_060[40];
    volatile usb_otgfs_hptxfsiz_reg_t otgfs_hptxfsiz;
    volatile usb_otgfs_dieptxf1_reg_t otgfs_dieptxf1;
    volatile usb_otgfs_dieptxf2_reg_t otgfs_dieptxf2;
    volatile usb_otgfs_dieptxf3_reg_t otgfs_dieptxf3;
    volatile usb_otgfs_dieptxf4_reg_t otgfs_dieptxf4;
    uint32_t reserved_114[187];
    volatile usb_otgfs_hcfg_reg_t otgfs_hcfg;
    volatile usb_otgfs_hfir_reg_t otgfs_hfir;
    volatile usb_otgfs_hfnum_reg_t otgfs_hfnum;
    uint32_t reserved_40c;
    volatile usb_otgfs_hptxsts_reg_t otgfs_hptxsts;
    volatile usb_otgfs_haint_reg_t otgfs_haint;
    volatile usb_otgfs_haintmsk_reg_t otgfs_haintmsk;
    volatile usb_otgfs_hflbaddr_reg_t otgfs_hflbaddr;
    uint32_t reserved_420[8];
    volatile usb_otgfs_hprt_reg_t otgfs_hprt;
    uint32_t reserved_444[47];
    volatile usb_otgfs_hcchar0_reg_t otgfs_hcchar0;
    uint32_t reserved_504;
    volatile usb_otgfs_hcint0_reg_t otgfs_hcint0;
    volatile usb_otgfs_hcintmsk0_reg_t otgfs_hcintmsk0;
    volatile usb_otgfs_hctsiz0_reg_t otgfs_hctsiz0;
    volatile usb_otgfs_hcdma0_reg_t otgfs_hcdma0;
    uint32_t reserved_518;
    volatile usb_otgfs_hcdmab0_reg_t otgfs_hcdmab0;
    volatile usb_otgfs_hcchar1_reg_t otgfs_hcchar1;
    uint32_t reserved_524;
    volatile usb_otgfs_hcint1_reg_t otgfs_hcint1;
    volatile usb_otgfs_hcintmsk1_reg_t otgfs_hcintmsk1;
    volatile usb_otgfs_hctsiz1_reg_t otgfs_hctsiz1;
    volatile usb_otgfs_hcdma1_reg_t otgfs_hcdma1;
    uint32_t reserved_538;
    volatile usb_otgfs_hcdmab1_reg_t otgfs_hcdmab1;
    volatile usb_otgfs_hcchar2_reg_t otgfs_hcchar2;
    uint32_t reserved_544;
    volatile usb_otgfs_hcint2_reg_t otgfs_hcint2;
    volatile usb_otgfs_hcintmsk2_reg_t otgfs_hcintmsk2;
    volatile usb_otgfs_hctsiz2_reg_t otgfs_hctsiz2;
    volatile usb_otgfs_hcdma2_reg_t otgfs_hcdma2;
    uint32_t reserved_558;
    volatile usb_otgfs_hcdmab2_reg_t otgfs_hcdmab2;
    volatile usb_otgfs_hcchar3_reg_t otgfs_hcchar3;
    uint32_t reserved_564;
    volatile usb_otgfs_hcint3_reg_t otgfs_hcint3;
    volatile usb_otgfs_hcintmsk3_reg_t otgfs_hcintmsk3;
    volatile usb_otgfs_hctsiz3_reg_t otgfs_hctsiz3;
    volatile usb_otgfs_hcdma3_reg_t otgfs_hcdma3;
    uint32_t reserved_578;
    volatile usb_otgfs_hcdmab3_reg_t otgfs_hcdmab3;
    volatile usb_otgfs_hcchar4_reg_t otgfs_hcchar4;
    uint32_t reserved_584;
    volatile usb_otgfs_hcint4_reg_t otgfs_hcint4;
    volatile usb_otgfs_hcintmsk4_reg_t otgfs_hcintmsk4;
    volatile usb_otgfs_hctsiz4_reg_t otgfs_hctsiz4;
    volatile usb_otgfs_hcdma4_reg_t otgfs_hcdma4;
    uint32_t reserved_598;
    volatile usb_otgfs_hcdmab4_reg_t otgfs_hcdmab4;
    volatile usb_otgfs_hcchar5_reg_t otgfs_hcchar5;
    uint32_t reserved_5a4;
    volatile usb_otgfs_hcint5_reg_t otgfs_hcint5;
    volatile usb_otgfs_hcintmsk5_reg_t otgfs_hcintmsk5;
    volatile usb_otgfs_hctsiz5_reg_t otgfs_hctsiz5;
    volatile usb_otgfs_hcdma5_reg_t otgfs_hcdma5;
    uint32_t reserved_5b8;
    volatile usb_otgfs_hcdmab5_reg_t otgfs_hcdmab5;
    volatile usb_otgfs_hcchar6_reg_t otgfs_hcchar6;
    uint32_t reserved_5c4;
    volatile usb_otgfs_hcint6_reg_t otgfs_hcint6;
    volatile usb_otgfs_hcintmsk6_reg_t otgfs_hcintmsk6;
    volatile usb_otgfs_hctsiz6_reg_t otgfs_hctsiz6;
    volatile usb_otgfs_hcdma6_reg_t otgfs_hcdma6;
    uint32_t reserved_5d8;
    volatile usb_otgfs_hcdmab6_reg_t otgfs_hcdmab6;
    volatile usb_otgfs_hcchar7_reg_t otgfs_hcchar7;
    uint32_t reserved_5e4;
    volatile usb_otgfs_hcint7_reg_t otgfs_hcint7;
    volatile usb_otgfs_hcintmsk7_reg_t otgfs_hcintmsk7;
    volatile usb_otgfs_hctsiz7_reg_t otgfs_hctsiz7;
    volatile usb_otgfs_hcdma7_reg_t otgfs_hcdma7;
    uint32_t reserved_5f8;
    volatile usb_otgfs_hcdmab7_reg_t otgfs_hcdmab7;
    uint32_t reserved_600[128];
    volatile usb_otgfs_dcfg_reg_t otgfs_dcfg;
    volatile usb_otgfs_dctl_reg_t otgfs_dctl;
    volatile usb_otgfs_dsts_reg_t otgfs_dsts;
    uint32_t reserved_80c;
    volatile usb_otgfs_diepmsk_reg_t otgfs_diepmsk;
    volatile usb_otgfs_doepmsk_reg_t otgfs_doepmsk;
    volatile usb_otgfs_daint_reg_t otgfs_daint;
    volatile usb_otgfs_daintmsk_reg_t otgfs_daintmsk;
    uint32_t reserved_820[2];
    volatile usb_otgfs_dvbusdis_reg_t otgfs_dvbusdis;
    volatile usb_otgfs_dvbuspulse_reg_t otgfs_dvbuspulse;
    volatile usb_otgfs_dthrctl_reg_t otgfs_dthrctl;
    volatile usb_otgfs_diepempmsk_reg_t otgfs_diepempmsk;
    uint32_t reserved_838[50];
    volatile usb_otgfs_diepctl0_reg_t otgfs_diepctl0;
    uint32_t reserved_904;
    volatile usb_otgfs_diepint0_reg_t otgfs_diepint0;
    uint32_t reserved_90c;
    volatile usb_otgfs_dieptsiz0_reg_t otgfs_dieptsiz0;
    volatile usb_otgfs_diepdma0_reg_t otgfs_diepdma0;
    volatile usb_otgfs_dtxfsts0_reg_t otgfs_dtxfsts0;
    volatile usb_otgfs_diepdmab0_reg_t otgfs_diepdmab0;
    volatile usb_otgfs_diepctl1_reg_t otgfs_diepctl1;
    uint32_t reserved_924;
    volatile usb_otgfs_diepint1_reg_t otgfs_diepint1;
    uint32_t reserved_92c;
    volatile usb_otgfs_dieptsiz1_reg_t otgfs_dieptsiz1;
    volatile usb_otgfs_diepdma1_reg_t otgfs_diepdma1;
    volatile usb_otgfs_dtxfsts1_reg_t otgfs_dtxfsts1;
    volatile usb_otgfs_diepdmab1_reg_t otgfs_diepdmab1;
    volatile usb_otgfs_diepctl2_reg_t otgfs_diepctl2;
    uint32_t reserved_944;
    volatile usb_otgfs_diepint2_reg_t otgfs_diepint2;
    uint32_t reserved_94c;
    volatile usb_otgfs_dieptsiz2_reg_t otgfs_dieptsiz2;
    volatile usb_otgfs_diepdma2_reg_t otgfs_diepdma2;
    volatile usb_otgfs_dtxfsts2_reg_t otgfs_dtxfsts2;
    volatile usb_otgfs_diepdmab2_reg_t otgfs_diepdmab2;
    volatile usb_otgfs_diepctl3_reg_t otgfs_diepctl3;
    uint32_t reserved_964;
    volatile usb_otgfs_diepint3_reg_t otgfs_diepint3;
    uint32_t reserved_96c;
    volatile usb_otgfs_dieptsiz3_reg_t otgfs_dieptsiz3;
    volatile usb_otgfs_diepdma3_reg_t otgfs_diepdma3;
    volatile usb_otgfs_dtxfsts3_reg_t otgfs_dtxfsts3;
    volatile usb_otgfs_diepdmab3_reg_t otgfs_diepdmab3;
    volatile usb_otgfs_diepctl4_reg_t otgfs_diepctl4;
    uint32_t reserved_984;
    volatile usb_otgfs_diepint4_reg_t otgfs_diepint4;
    uint32_t reserved_98c;
    volatile usb_otgfs_dieptsiz4_reg_t otgfs_dieptsiz4;
    volatile usb_otgfs_diepdma4_reg_t otgfs_diepdma4;
    volatile usb_otgfs_dtxfsts4_reg_t otgfs_dtxfsts4;
    volatile usb_otgfs_diepdmab4_reg_t otgfs_diepdmab4;
    volatile usb_otgfs_diepctl5_reg_t otgfs_diepctl5;
    uint32_t reserved_9a4;
    volatile usb_otgfs_diepint5_reg_t otgfs_diepint5;
    uint32_t reserved_9ac;
    volatile usb_otgfs_dieptsiz5_reg_t otgfs_dieptsiz5;
    volatile usb_otgfs_diepdma5_reg_t otgfs_diepdma5;
    volatile usb_otgfs_dtxfsts5_reg_t otgfs_dtxfsts5;
    volatile usb_otgfs_diepdmab5_reg_t otgfs_diepdmab5;
    volatile usb_otgfs_diepctl6_reg_t otgfs_diepctl6;
    uint32_t reserved_9c4;
    volatile usb_otgfs_diepint6_reg_t otgfs_diepint6;
    uint32_t reserved_9cc;
    volatile usb_otgfs_dieptsiz6_reg_t otgfs_dieptsiz6;
    volatile usb_otgfs_diepdma6_reg_t otgfs_diepdma6;
    volatile usb_otgfs_dtxfsts6_reg_t otgfs_dtxfsts6;
    volatile usb_otgfs_diepdmab6_reg_t otgfs_diepdmab6;
    uint32_t reserved_9e0[72];
    volatile usb_otgfs_doepctl0_reg_t otgfs_doepctl0;
    uint32_t reserved_b04;
    volatile usb_otgfs_doepint0_reg_t otgfs_doepint0;
    uint32_t reserved_b0c;
    volatile usb_otgfs_doeptsiz0_reg_t otgfs_doeptsiz0;
    volatile usb_otgfs_doepdma0_reg_t otgfs_doepdma0;
    uint32_t reserved_b18;
    volatile usb_otgfs_doepdmab0_reg_t otgfs_doepdmab0;
    volatile usb_otgfs_doepctl1_reg_t otgfs_doepctl1;
    uint32_t reserved_b24;
    volatile usb_otgfs_doepint1_reg_t otgfs_doepint1;
    uint32_t reserved_b2c;
    volatile usb_otgfs_doeptsiz1_reg_t otgfs_doeptsiz1;
    volatile usb_otgfs_doepdma1_reg_t otgfs_doepdma1;
    uint32_t reserved_b38;
    volatile usb_otgfs_doepdmab1_reg_t otgfs_doepdmab1;
    volatile usb_otgfs_doepctl2_reg_t otgfs_doepctl2;
    uint32_t reserved_b44;
    volatile usb_otgfs_doepint2_reg_t otgfs_doepint2;
    uint32_t reserved_b4c;
    volatile usb_otgfs_doeptsiz2_reg_t otgfs_doeptsiz2;
    volatile usb_otgfs_doepdma2_reg_t otgfs_doepdma2;
    uint32_t reserved_b58;
    volatile usb_otgfs_doepdmab2_reg_t otgfs_doepdmab2;
    volatile usb_otgfs_doepctl3_reg_t otgfs_doepctl3;
    uint32_t reserved_b64;
    volatile usb_otgfs_doepint3_reg_t otgfs_doepint3;
    uint32_t reserved_b6c;
    volatile usb_otgfs_doeptsiz3_reg_t otgfs_doeptsiz3;
    volatile usb_otgfs_doepdma3_reg_t otgfs_doepdma3;
    uint32_t reserved_b78;
    volatile usb_otgfs_doepdmab3_reg_t otgfs_doepdmab3;
    volatile usb_otgfs_doepctl4_reg_t otgfs_doepctl4;
    uint32_t reserved_b84;
    volatile usb_otgfs_doepint4_reg_t otgfs_doepint4;
    uint32_t reserved_b8c;
    volatile usb_otgfs_doeptsiz4_reg_t otgfs_doeptsiz4;
    volatile usb_otgfs_doepdma4_reg_t otgfs_doepdma4;
    uint32_t reserved_b98;
    volatile usb_otgfs_doepdmab4_reg_t otgfs_doepdmab4;
    volatile usb_otgfs_doepctl5_reg_t otgfs_doepctl5;
    uint32_t reserved_ba4;
    volatile usb_otgfs_doepint5_reg_t otgfs_doepint5;
    uint32_t reserved_bac;
    volatile usb_otgfs_doeptsiz5_reg_t otgfs_doeptsiz5;
    volatile usb_otgfs_doepdma5_reg_t otgfs_doepdma5;
    uint32_t reserved_bb8;
    volatile usb_otgfs_doepdmab5_reg_t otgfs_doepdmab5;
    volatile usb_otgfs_doepctl6_reg_t otgfs_doepctl6;
    uint32_t reserved_bc4;
    volatile usb_otgfs_doepint6_reg_t otgfs_doepint6;
    uint32_t reserved_bcc;
    volatile usb_otgfs_doeptsiz6_reg_t otgfs_doeptsiz6;
    volatile usb_otgfs_doepdma6_reg_t otgfs_doepdma6;
    uint32_t reserved_bd8;
    volatile usb_otgfs_doepdmab6_reg_t otgfs_doepdmab6;
    uint32_t reserved_be0[136];
    volatile usb_otgfs_pcgcctl_reg_t otgfs_pcgcctl;
} usb_dev_t;

extern usb_dev_t USB_OTG_FS_CORE0;
extern usb_dev_t USB_OTG_FS_CORE1;

#ifndef __cplusplus
_Static_assert(sizeof(usb_dev_t) == 0xe04, "Invalid size of usb_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
