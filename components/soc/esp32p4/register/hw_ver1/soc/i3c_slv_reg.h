/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** I3C_SLV_CONFIG_REG register
 *  NA
 */
#define I3C_SLV_CONFIG_REG (DR_REG_I3C_SLV_BASE + 0x4)
/** I3C_SLV_SLVENA : R/W; bitpos: [0]; default: 1;
 *  1: allow the slave to operate on i2c or i3c bus. 0: the slave will ignore the bus.
 *  This should be not set until registers such as PARTNO, IDEXT and the like are set
 *  1st  -if used- since they impact data to the master
 */
#define I3C_SLV_SLVENA    (BIT(0))
#define I3C_SLV_SLVENA_M  (I3C_SLV_SLVENA_V << I3C_SLV_SLVENA_S)
#define I3C_SLV_SLVENA_V  0x00000001U
#define I3C_SLV_SLVENA_S  0
/** I3C_SLV_NACK : R/W; bitpos: [1]; default: 0;
 *  1:the slave will NACK all requests to it except CCC broadcast. This should be used
 *  with caution as the Master may determine the slave is missing if overused.
 */
#define I3C_SLV_NACK    (BIT(1))
#define I3C_SLV_NACK_M  (I3C_SLV_NACK_V << I3C_SLV_NACK_S)
#define I3C_SLV_NACK_V  0x00000001U
#define I3C_SLV_NACK_S  1
/** I3C_SLV_MATCHSS : R/W; bitpos: [2]; default: 0;
 *  1: the START and STOP sticky STATUS bits will only be set if MATCHED is set..This
 *  allows START and STOP to be used to detect end of a message to /from this slave.
 */
#define I3C_SLV_MATCHSS    (BIT(2))
#define I3C_SLV_MATCHSS_M  (I3C_SLV_MATCHSS_V << I3C_SLV_MATCHSS_S)
#define I3C_SLV_MATCHSS_V  0x00000001U
#define I3C_SLV_MATCHSS_S  2
/** I3C_SLV_S0IGNORE : R/W; bitpos: [3]; default: 0;
 *  If 1, the Slave will not detect S0 or S1 errors and so not lock up waiting on an
 *  Exit Pattern. This should only be used when the bus will not use HDR.
 */
#define I3C_SLV_S0IGNORE    (BIT(3))
#define I3C_SLV_S0IGNORE_M  (I3C_SLV_S0IGNORE_V << I3C_SLV_S0IGNORE_S)
#define I3C_SLV_S0IGNORE_V  0x00000001U
#define I3C_SLV_S0IGNORE_S  3
/** I3C_SLV_DDROK : R/W; bitpos: [4]; default: 0;
 *  NA
 */
#define I3C_SLV_DDROK    (BIT(4))
#define I3C_SLV_DDROK_M  (I3C_SLV_DDROK_V << I3C_SLV_DDROK_S)
#define I3C_SLV_DDROK_V  0x00000001U
#define I3C_SLV_DDROK_S  4
/** I3C_SLV_IDRAND : R/W; bitpos: [8]; default: 0;
 *  NA
 */
#define I3C_SLV_IDRAND    (BIT(8))
#define I3C_SLV_IDRAND_M  (I3C_SLV_IDRAND_V << I3C_SLV_IDRAND_S)
#define I3C_SLV_IDRAND_V  0x00000001U
#define I3C_SLV_IDRAND_S  8
/** I3C_SLV_OFFLINE : R/W; bitpos: [9]; default: 0;
 *  NA
 */
#define I3C_SLV_OFFLINE    (BIT(9))
#define I3C_SLV_OFFLINE_M  (I3C_SLV_OFFLINE_V << I3C_SLV_OFFLINE_S)
#define I3C_SLV_OFFLINE_V  0x00000001U
#define I3C_SLV_OFFLINE_S  9
/** I3C_SLV_BAMATCH : R/W; bitpos: [23:16]; default: 47;
 *  Bus Available condition match value for current ???Slow clock???. This provides the
 *  count of the slow clock to count out 1us (or more) to allow an IBI to drive SDA Low
 *  when the Master is not doing so. The max width , and so max value, is controlled by
 *  the block. Only if enabled for events such IBI or MR or HJ, and if enabled to
 *  provide this as a register. With is limited to CLK_SLOW_BITS
 */
#define I3C_SLV_BAMATCH    0x000000FFU
#define I3C_SLV_BAMATCH_M  (I3C_SLV_BAMATCH_V << I3C_SLV_BAMATCH_S)
#define I3C_SLV_BAMATCH_V  0x000000FFU
#define I3C_SLV_BAMATCH_S  16
/** I3C_SLV_SADDR : R/W; bitpos: [31:25]; default: 0;
 *  If allowed by the block:sets i2c 7 bits static address,else should be 0. If enabled
 *  to use one and to be provided by SW. Block may provide in HW as well.
 */
#define I3C_SLV_SADDR    0x0000007FU
#define I3C_SLV_SADDR_M  (I3C_SLV_SADDR_V << I3C_SLV_SADDR_S)
#define I3C_SLV_SADDR_V  0x0000007FU
#define I3C_SLV_SADDR_S  25

/** I3C_SLV_STATUS_REG register
 *  NA
 */
#define I3C_SLV_STATUS_REG (DR_REG_I3C_SLV_BASE + 0x8)
/** I3C_SLV_STNOTSTOP : RO; bitpos: [0]; default: 0;
 *  Is 1 if bus is busy(activity) and 0 when in a STOP condition. Other bits may also
 *  set when busy. Note that this can also be true from an S0 or S1 error, which waits
 *  for an Exit Pattern.
 */
#define I3C_SLV_STNOTSTOP    (BIT(0))
#define I3C_SLV_STNOTSTOP_M  (I3C_SLV_STNOTSTOP_V << I3C_SLV_STNOTSTOP_S)
#define I3C_SLV_STNOTSTOP_V  0x00000001U
#define I3C_SLV_STNOTSTOP_S  0
/** I3C_SLV_STMSG : RO; bitpos: [1]; default: 0;
 *  Is 1 if this bus Slave is listening to the bus traffic or responding, If
 *  STNOSTOP=1, then this will be 0 when a non-matching address seen until next
 *  respeated START it STOP.
 */
#define I3C_SLV_STMSG    (BIT(1))
#define I3C_SLV_STMSG_M  (I3C_SLV_STMSG_V << I3C_SLV_STMSG_S)
#define I3C_SLV_STMSG_V  0x00000001U
#define I3C_SLV_STMSG_S  1
/** I3C_SLV_STCCCH : RO; bitpos: [2]; default: 0;
 *  Is 1 if a CCC message is being handled automatically.
 */
#define I3C_SLV_STCCCH    (BIT(2))
#define I3C_SLV_STCCCH_M  (I3C_SLV_STCCCH_V << I3C_SLV_STCCCH_S)
#define I3C_SLV_STCCCH_V  0x00000001U
#define I3C_SLV_STCCCH_S  2
/** I3C_SLV_STREQRD : RO; bitpos: [3]; default: 0;
 *  1 if the req in process is an sdr read from this slave or an IBI is being pushed
 *  out,
 */
#define I3C_SLV_STREQRD    (BIT(3))
#define I3C_SLV_STREQRD_M  (I3C_SLV_STREQRD_V << I3C_SLV_STREQRD_S)
#define I3C_SLV_STREQRD_V  0x00000001U
#define I3C_SLV_STREQRD_S  3
/** I3C_SLV_STREQWR : RO; bitpos: [4]; default: 0;
 *  NA
 */
#define I3C_SLV_STREQWR    (BIT(4))
#define I3C_SLV_STREQWR_M  (I3C_SLV_STREQWR_V << I3C_SLV_STREQWR_S)
#define I3C_SLV_STREQWR_V  0x00000001U
#define I3C_SLV_STREQWR_S  4
/** I3C_SLV_STDAA : RO; bitpos: [5]; default: 0;
 *  NA
 */
#define I3C_SLV_STDAA    (BIT(5))
#define I3C_SLV_STDAA_M  (I3C_SLV_STDAA_V << I3C_SLV_STDAA_S)
#define I3C_SLV_STDAA_V  0x00000001U
#define I3C_SLV_STDAA_S  5
/** I3C_SLV_STHDR : RO; bitpos: [6]; default: 0;
 *  NA
 */
#define I3C_SLV_STHDR    (BIT(6))
#define I3C_SLV_STHDR_M  (I3C_SLV_STHDR_V << I3C_SLV_STHDR_S)
#define I3C_SLV_STHDR_V  0x00000001U
#define I3C_SLV_STHDR_S  6
/** I3C_SLV_START : R/W; bitpos: [8]; default: 0;
 *  NA
 */
#define I3C_SLV_START    (BIT(8))
#define I3C_SLV_START_M  (I3C_SLV_START_V << I3C_SLV_START_S)
#define I3C_SLV_START_V  0x00000001U
#define I3C_SLV_START_S  8
/** I3C_SLV_MATCHED : R/W; bitpos: [9]; default: 0;
 *  NA
 */
#define I3C_SLV_MATCHED    (BIT(9))
#define I3C_SLV_MATCHED_M  (I3C_SLV_MATCHED_V << I3C_SLV_MATCHED_S)
#define I3C_SLV_MATCHED_V  0x00000001U
#define I3C_SLV_MATCHED_S  9
/** I3C_SLV_STOP : R/W; bitpos: [10]; default: 0;
 *  NA
 */
#define I3C_SLV_STOP    (BIT(10))
#define I3C_SLV_STOP_M  (I3C_SLV_STOP_V << I3C_SLV_STOP_S)
#define I3C_SLV_STOP_V  0x00000001U
#define I3C_SLV_STOP_S  10
/** I3C_SLV_RXPEND : RO; bitpos: [11]; default: 0;
 *  Receiving a message from master,which is not being handled by block(not a CCC
 *  internally processed). For all but External FIFO, this uses DATACTRL RXTRIG, which
 *  defaults to not-empty. If DMA is enabled for RX, DMA will be signaled as well. Will
 *  self-clear if data is read(FIFO and non-FIFO)
 */
#define I3C_SLV_RXPEND    (BIT(11))
#define I3C_SLV_RXPEND_M  (I3C_SLV_RXPEND_V << I3C_SLV_RXPEND_S)
#define I3C_SLV_RXPEND_V  0x00000001U
#define I3C_SLV_RXPEND_S  11
/** I3C_SLV_TXNOTFULL : RO; bitpos: [12]; default: 0;
 *  Is 1 when the To-bus buffer/FIFO can accept more data to go out. Defau:1. For all
 *  but External FIFO, this uses DATACTRL TXTRIG,which defaults to not-full. If DMA is
 *  enabled for TX, it will also be signaled to provide more.
 */
#define I3C_SLV_TXNOTFULL    (BIT(12))
#define I3C_SLV_TXNOTFULL_M  (I3C_SLV_TXNOTFULL_V << I3C_SLV_TXNOTFULL_S)
#define I3C_SLV_TXNOTFULL_V  0x00000001U
#define I3C_SLV_TXNOTFULL_S  12
/** I3C_SLV_DACHG : R/W; bitpos: [13]; default: 0;
 *  The Slv Dynamic Address has been assigned, reassigned, or reset(lost) and is now in
 *  that state of being valid or none. Actual DA can be seen in the DYNADDR register.
 *  Note that this will also be used when MAP Auto feature is configured. This will be
 *  changing one or more MAP items. See DYNADDR and/or MAPCTRLn. DYNAADDR for the main
 *  DA(0) will indicate if last change was due to Auto MAP.
 */
#define I3C_SLV_DACHG    (BIT(13))
#define I3C_SLV_DACHG_M  (I3C_SLV_DACHG_V << I3C_SLV_DACHG_S)
#define I3C_SLV_DACHG_V  0x00000001U
#define I3C_SLV_DACHG_S  13
/** I3C_SLV_CCC : R/W; bitpos: [14]; default: 0;
 *  A common -command-code(CCC), not handled by block, has been received. This acts
 *  differently between: *Broadcasted ones, which will then also correspond with RXPEND
 *  and the 1st byte will be the CCC(command) . *Direct ones, which may never be
 *  directed to this device.  If it is, then the TXSEND or RXPEND will be triggered
 *  with this end the RXPEND will contain the command.
 */
#define I3C_SLV_CCC    (BIT(14))
#define I3C_SLV_CCC_M  (I3C_SLV_CCC_V << I3C_SLV_CCC_S)
#define I3C_SLV_CCC_V  0x00000001U
#define I3C_SLV_CCC_S  14
/** I3C_SLV_ERRWARN : RO; bitpos: [15]; default: 0;
 *  NA
 */
#define I3C_SLV_ERRWARN    (BIT(15))
#define I3C_SLV_ERRWARN_M  (I3C_SLV_ERRWARN_V << I3C_SLV_ERRWARN_S)
#define I3C_SLV_ERRWARN_V  0x00000001U
#define I3C_SLV_ERRWARN_S  15
/** I3C_SLV_HDRMATCH : R/W; bitpos: [16]; default: 0;
 *  NA
 */
#define I3C_SLV_HDRMATCH    (BIT(16))
#define I3C_SLV_HDRMATCH_M  (I3C_SLV_HDRMATCH_V << I3C_SLV_HDRMATCH_S)
#define I3C_SLV_HDRMATCH_V  0x00000001U
#define I3C_SLV_HDRMATCH_S  16

/** I3C_SLV_CTRL_REG register
 *  NA
 */
#define I3C_SLV_CTRL_REG (DR_REG_I3C_SLV_BASE + 0xc)
/** I3C_SLV_SLV_EVENT : R/W; bitpos: [1:0]; default: 0;
 *  If set to non-0, will request an event. Once requested, STATUS.EVENT and EVDET will
 *  show the status as it progresses. Once completed, the field will automatically
 *  return to 0. Once non-0, only 0 can be written(to cancel) until done. 0: Normal
 *  mode. If set to 0 after was a non-0 value, will cancel if not already in flight. 1:
 *  start an IBI. This will try to push through an IBI on the bus. If data associate
 *  with the IBI, it will be drawn from the IBIDATA field. Note that if Time control is
 *  enabled, this will include anytime control related bytes  further, the IBIDATA byte
 *  will have bit7 set to 1.
 */
#define I3C_SLV_SLV_EVENT    0x00000003U
#define I3C_SLV_SLV_EVENT_M  (I3C_SLV_SLV_EVENT_V << I3C_SLV_SLV_EVENT_S)
#define I3C_SLV_SLV_EVENT_V  0x00000003U
#define I3C_SLV_SLV_EVENT_S  0
/** I3C_SLV_EXTDATA : R/W; bitpos: [3]; default: 0;
 *  reserved
 */
#define I3C_SLV_EXTDATA    (BIT(3))
#define I3C_SLV_EXTDATA_M  (I3C_SLV_EXTDATA_V << I3C_SLV_EXTDATA_S)
#define I3C_SLV_EXTDATA_V  0x00000001U
#define I3C_SLV_EXTDATA_S  3
/** I3C_SLV_MAPIDX : R/W; bitpos: [7:4]; default: 0;
 *  Index of Dynamic Address that IBI is for. This is 0 for the main or base Dynamic
 *  Address, or can be any valid index.
 */
#define I3C_SLV_MAPIDX    0x0000000FU
#define I3C_SLV_MAPIDX_M  (I3C_SLV_MAPIDX_V << I3C_SLV_MAPIDX_S)
#define I3C_SLV_MAPIDX_V  0x0000000FU
#define I3C_SLV_MAPIDX_S  4
/** I3C_SLV_IBIDATA : R/W; bitpos: [15:8]; default: 0;
 *  Data byte to go with an IBI, if enabled for it. If enabled (was in BCR), then it is
 *  required.
 */
#define I3C_SLV_IBIDATA    0x000000FFU
#define I3C_SLV_IBIDATA_M  (I3C_SLV_IBIDATA_V << I3C_SLV_IBIDATA_S)
#define I3C_SLV_IBIDATA_V  0x000000FFU
#define I3C_SLV_IBIDATA_S  8
/** I3C_SLV_PENDINT : R/W; bitpos: [19:16]; default: 0;
 *  Should be set to the pending interrupt that GETSTATUS CCC will return. This should
 *  be maintained by the application if used and configured, as the Master will read
 *  this. If not configured, the GETSTATUS field will return 1 if an IBI is pending,
 *  and 0 otherwise.
 */
#define I3C_SLV_PENDINT    0x0000000FU
#define I3C_SLV_PENDINT_M  (I3C_SLV_PENDINT_V << I3C_SLV_PENDINT_S)
#define I3C_SLV_PENDINT_V  0x0000000FU
#define I3C_SLV_PENDINT_S  16
/** I3C_SLV_ACTSTATE : R/W; bitpos: [21:20]; default: 0;
 *  NA
 */
#define I3C_SLV_ACTSTATE    0x00000003U
#define I3C_SLV_ACTSTATE_M  (I3C_SLV_ACTSTATE_V << I3C_SLV_ACTSTATE_S)
#define I3C_SLV_ACTSTATE_V  0x00000003U
#define I3C_SLV_ACTSTATE_S  20
/** I3C_SLV_VENDINFO : R/W; bitpos: [31:24]; default: 0;
 *  NA
 */
#define I3C_SLV_VENDINFO    0x000000FFU
#define I3C_SLV_VENDINFO_M  (I3C_SLV_VENDINFO_V << I3C_SLV_VENDINFO_S)
#define I3C_SLV_VENDINFO_V  0x000000FFU
#define I3C_SLV_VENDINFO_S  24

/** I3C_SLV_INTSET_REG register
 *  INSET allows setting enables for interrupts(connecting the corresponding STATUS
 *  source to causing an IRQ to the processor)
 */
#define I3C_SLV_INTSET_REG (DR_REG_I3C_SLV_BASE + 0x10)
/** I3C_SLV_STOP_ENA : R/W; bitpos: [10]; default: 0;
 *  Interrupt on STOP state on the bus. See Start as the preferred interrupt when
 *  needed. This interrupt may not trigger for quick STOP/START combination, as it
 *  relates to the state of being stopped.
 */
#define I3C_SLV_STOP_ENA    (BIT(10))
#define I3C_SLV_STOP_ENA_M  (I3C_SLV_STOP_ENA_V << I3C_SLV_STOP_ENA_S)
#define I3C_SLV_STOP_ENA_V  0x00000001U
#define I3C_SLV_STOP_ENA_S  10
/** I3C_SLV_RXPEND_ENA : R/W; bitpos: [11]; default: 0;
 *  Interrupt when receiving a message from Master, which is not being handled by the
 *  block (excludes CCCs being handled automatically). If FIFO, then RX fullness
 *  trigger. If DMA, then message end.
 */
#define I3C_SLV_RXPEND_ENA    (BIT(11))
#define I3C_SLV_RXPEND_ENA_M  (I3C_SLV_RXPEND_ENA_V << I3C_SLV_RXPEND_ENA_S)
#define I3C_SLV_RXPEND_ENA_V  0x00000001U
#define I3C_SLV_RXPEND_ENA_S  11
/** I3C_SLV_TXSEND_ENA : R/W; bitpos: [12]; default: 0;
 *  NA
 */
#define I3C_SLV_TXSEND_ENA    (BIT(12))
#define I3C_SLV_TXSEND_ENA_M  (I3C_SLV_TXSEND_ENA_V << I3C_SLV_TXSEND_ENA_S)
#define I3C_SLV_TXSEND_ENA_V  0x00000001U
#define I3C_SLV_TXSEND_ENA_S  12

/** I3C_SLV_INTCLR_REG register
 *  NA
 */
#define I3C_SLV_INTCLR_REG (DR_REG_I3C_SLV_BASE + 0x14)
/** I3C_SLV_STOP_CLR : WO; bitpos: [10]; default: 0;
 *  Interrupt on STOP state on the bus. See Start as the preferred interrupt when
 *  needed. This interrupt may not trigger for quick STOP/START combination, as it
 *  relates to the state of being stopped.
 */
#define I3C_SLV_STOP_CLR    (BIT(10))
#define I3C_SLV_STOP_CLR_M  (I3C_SLV_STOP_CLR_V << I3C_SLV_STOP_CLR_S)
#define I3C_SLV_STOP_CLR_V  0x00000001U
#define I3C_SLV_STOP_CLR_S  10
/** I3C_SLV_RXPEND_CLR : WO; bitpos: [11]; default: 0;
 *  Interrupt when receiving a message from Master, which is not being handled by the
 *  block (excludes CCCs being handled automatically). If FIFO, then RX fullness
 *  trigger. If DMA, then message end.
 */
#define I3C_SLV_RXPEND_CLR    (BIT(11))
#define I3C_SLV_RXPEND_CLR_M  (I3C_SLV_RXPEND_CLR_V << I3C_SLV_RXPEND_CLR_S)
#define I3C_SLV_RXPEND_CLR_V  0x00000001U
#define I3C_SLV_RXPEND_CLR_S  11
/** I3C_SLV_TXSEND_CLR : WO; bitpos: [12]; default: 0;
 *  NA
 */
#define I3C_SLV_TXSEND_CLR    (BIT(12))
#define I3C_SLV_TXSEND_CLR_M  (I3C_SLV_TXSEND_CLR_V << I3C_SLV_TXSEND_CLR_S)
#define I3C_SLV_TXSEND_CLR_V  0x00000001U
#define I3C_SLV_TXSEND_CLR_S  12

/** I3C_SLV_INTMASKED_REG register
 *  NA
 */
#define I3C_SLV_INTMASKED_REG (DR_REG_I3C_SLV_BASE + 0x18)
/** I3C_SLV_STOP_MASK : RO; bitpos: [10]; default: 0;
 *  Interrupt on STOP state on the bus. See Start as the preferred interrupt when
 *  needed. This interrupt may not trigger for quick STOP/START combination, as it
 *  relates to the state of being stopped.
 */
#define I3C_SLV_STOP_MASK    (BIT(10))
#define I3C_SLV_STOP_MASK_M  (I3C_SLV_STOP_MASK_V << I3C_SLV_STOP_MASK_S)
#define I3C_SLV_STOP_MASK_V  0x00000001U
#define I3C_SLV_STOP_MASK_S  10
/** I3C_SLV_RXPEND_MASK : RO; bitpos: [11]; default: 0;
 *  Interrupt when receiving a message from Master, which is not being handled by the
 *  block (excludes CCCs being handled automatically). If FIFO, then RX fullness
 *  trigger. If DMA, then message end.
 */
#define I3C_SLV_RXPEND_MASK    (BIT(11))
#define I3C_SLV_RXPEND_MASK_M  (I3C_SLV_RXPEND_MASK_V << I3C_SLV_RXPEND_MASK_S)
#define I3C_SLV_RXPEND_MASK_V  0x00000001U
#define I3C_SLV_RXPEND_MASK_S  11
/** I3C_SLV_TXSEND_MASK : RO; bitpos: [12]; default: 0;
 *  NA
 */
#define I3C_SLV_TXSEND_MASK    (BIT(12))
#define I3C_SLV_TXSEND_MASK_M  (I3C_SLV_TXSEND_MASK_V << I3C_SLV_TXSEND_MASK_S)
#define I3C_SLV_TXSEND_MASK_V  0x00000001U
#define I3C_SLV_TXSEND_MASK_S  12

/** I3C_SLV_DATACTRL_REG register
 *  NA
 */
#define I3C_SLV_DATACTRL_REG (DR_REG_I3C_SLV_BASE + 0x2c)
/** I3C_SLV_FLUSHTB : WO; bitpos: [0]; default: 0;
 *  Flushes the from-bus buffer/FIFO. Not normally used
 */
#define I3C_SLV_FLUSHTB    (BIT(0))
#define I3C_SLV_FLUSHTB_M  (I3C_SLV_FLUSHTB_V << I3C_SLV_FLUSHTB_S)
#define I3C_SLV_FLUSHTB_V  0x00000001U
#define I3C_SLV_FLUSHTB_S  0
/** I3C_SLV_FLUSHFB : WO; bitpos: [1]; default: 0;
 *  Flushes the to-bus buffer/FIFO. Used when Master terminates a to-bus (read) message
 *  prematurely
 */
#define I3C_SLV_FLUSHFB    (BIT(1))
#define I3C_SLV_FLUSHFB_M  (I3C_SLV_FLUSHFB_V << I3C_SLV_FLUSHFB_S)
#define I3C_SLV_FLUSHFB_V  0x00000001U
#define I3C_SLV_FLUSHFB_S  1
/** I3C_SLV_UNLOCK : WO; bitpos: [3]; default: 0;
 *  If this bit is not written 1, the register bits from 7 to 4 are not changed on
 *  write.
 */
#define I3C_SLV_UNLOCK    (BIT(3))
#define I3C_SLV_UNLOCK_M  (I3C_SLV_UNLOCK_V << I3C_SLV_UNLOCK_S)
#define I3C_SLV_UNLOCK_V  0x00000001U
#define I3C_SLV_UNLOCK_S  3
/** I3C_SLV_TXTRIG : R/W; bitpos: [5:4]; default: 3;
 *  Trigger level for tx emptiness when FIFOed, Affects interrupt and DMA(if enabled).
 *  The defaults is 3
 */
#define I3C_SLV_TXTRIG    0x00000003U
#define I3C_SLV_TXTRIG_M  (I3C_SLV_TXTRIG_V << I3C_SLV_TXTRIG_S)
#define I3C_SLV_TXTRIG_V  0x00000003U
#define I3C_SLV_TXTRIG_S  4
/** I3C_SLV_RXTRIG : R/W; bitpos: [7:6]; default: 2;
 *  Trigger level for rx fulless when FIFOed, Affects interrupt and DMA(if enabled).
 *  The defaults is 3
 */
#define I3C_SLV_RXTRIG    0x00000003U
#define I3C_SLV_RXTRIG_M  (I3C_SLV_RXTRIG_V << I3C_SLV_RXTRIG_S)
#define I3C_SLV_RXTRIG_V  0x00000003U
#define I3C_SLV_RXTRIG_S  6
/** I3C_SLV_TXCOUNT : RO; bitpos: [20:16]; default: 0;
 *  NA
 */
#define I3C_SLV_TXCOUNT    0x0000001FU
#define I3C_SLV_TXCOUNT_M  (I3C_SLV_TXCOUNT_V << I3C_SLV_TXCOUNT_S)
#define I3C_SLV_TXCOUNT_V  0x0000001FU
#define I3C_SLV_TXCOUNT_S  16
/** I3C_SLV_RXCOUNT : RO; bitpos: [28:24]; default: 0;
 *  NA
 */
#define I3C_SLV_RXCOUNT    0x0000001FU
#define I3C_SLV_RXCOUNT_M  (I3C_SLV_RXCOUNT_V << I3C_SLV_RXCOUNT_S)
#define I3C_SLV_RXCOUNT_V  0x0000001FU
#define I3C_SLV_RXCOUNT_S  24
/** I3C_SLV_TXFULL : RO; bitpos: [30]; default: 0;
 *  NA
 */
#define I3C_SLV_TXFULL    (BIT(30))
#define I3C_SLV_TXFULL_M  (I3C_SLV_TXFULL_V << I3C_SLV_TXFULL_S)
#define I3C_SLV_TXFULL_V  0x00000001U
#define I3C_SLV_TXFULL_S  30
/** I3C_SLV_RXEMPTY : RO; bitpos: [31]; default: 0;
 *  NA
 */
#define I3C_SLV_RXEMPTY    (BIT(31))
#define I3C_SLV_RXEMPTY_M  (I3C_SLV_RXEMPTY_V << I3C_SLV_RXEMPTY_S)
#define I3C_SLV_RXEMPTY_V  0x00000001U
#define I3C_SLV_RXEMPTY_S  31

/** I3C_SLV_WDATAB_REG register
 *  NA
 */
#define I3C_SLV_WDATAB_REG (DR_REG_I3C_SLV_BASE + 0x30)
/** I3C_SLV_WDATAB : WO; bitpos: [7:0]; default: 0;
 *  NA
 */
#define I3C_SLV_WDATAB    0x000000FFU
#define I3C_SLV_WDATAB_M  (I3C_SLV_WDATAB_V << I3C_SLV_WDATAB_S)
#define I3C_SLV_WDATAB_V  0x000000FFU
#define I3C_SLV_WDATAB_S  0
/** I3C_SLV_WDATA_END : WO; bitpos: [8]; default: 0;
 *  NA
 */
#define I3C_SLV_WDATA_END    (BIT(8))
#define I3C_SLV_WDATA_END_M  (I3C_SLV_WDATA_END_V << I3C_SLV_WDATA_END_S)
#define I3C_SLV_WDATA_END_V  0x00000001U
#define I3C_SLV_WDATA_END_S  8

/** I3C_SLV_WDATABE_REG register
 *  NA
 */
#define I3C_SLV_WDATABE_REG (DR_REG_I3C_SLV_BASE + 0x34)
/** I3C_SLV_WDATABE : WO; bitpos: [7:0]; default: 0;
 *  NA
 */
#define I3C_SLV_WDATABE    0x000000FFU
#define I3C_SLV_WDATABE_M  (I3C_SLV_WDATABE_V << I3C_SLV_WDATABE_S)
#define I3C_SLV_WDATABE_V  0x000000FFU
#define I3C_SLV_WDATABE_S  0

/** I3C_SLV_RDARAB_REG register
 *  Read Byte Data (from-bus) register
 */
#define I3C_SLV_RDARAB_REG (DR_REG_I3C_SLV_BASE + 0x40)
/** I3C_SLV_DATA0 : RO; bitpos: [7:0]; default: 0;
 *  This register allows reading a byte from the bus unless external FIFO is used. A
 *  byte should not be read unless there is data waiting, as indicated by the RXPEND
 *  bit being set in the STATUS register
 */
#define I3C_SLV_DATA0    0x000000FFU
#define I3C_SLV_DATA0_M  (I3C_SLV_DATA0_V << I3C_SLV_DATA0_S)
#define I3C_SLV_DATA0_V  0x000000FFU
#define I3C_SLV_DATA0_S  0

/** I3C_SLV_RDATAH_REG register
 *  Read Half-word Data (from-bus) register
 */
#define I3C_SLV_RDATAH_REG (DR_REG_I3C_SLV_BASE + 0x48)
/** I3C_SLV_DATA_LSB : RO; bitpos: [7:0]; default: 0;
 *  NA
 */
#define I3C_SLV_DATA_LSB    0x000000FFU
#define I3C_SLV_DATA_LSB_M  (I3C_SLV_DATA_LSB_V << I3C_SLV_DATA_LSB_S)
#define I3C_SLV_DATA_LSB_V  0x000000FFU
#define I3C_SLV_DATA_LSB_S  0
/** I3C_SLV_DATA_MSB : RO; bitpos: [15:8]; default: 0;
 *  This register allows reading a Half-word (byte pair)  from the bus unless external
 *  FIFO is used. A Half-word should not be read unless there is at least 2 bytes of
 *  data waiting, as indicated by the RX FIFO level trigger or RXCOUNT available space
 *  in the DATACTRL register
 */
#define I3C_SLV_DATA_MSB    0x000000FFU
#define I3C_SLV_DATA_MSB_M  (I3C_SLV_DATA_MSB_V << I3C_SLV_DATA_MSB_S)
#define I3C_SLV_DATA_MSB_V  0x000000FFU
#define I3C_SLV_DATA_MSB_S  8

/** I3C_SLV_CAPABILITIES2_REG register
 *  NA
 */
#define I3C_SLV_CAPABILITIES2_REG (DR_REG_I3C_SLV_BASE + 0x5c)
/** I3C_SLV_CAPABLITIES2 : RO; bitpos: [31:0]; default: 256;
 *  NA
 */
#define I3C_SLV_CAPABLITIES2    0xFFFFFFFFU
#define I3C_SLV_CAPABLITIES2_M  (I3C_SLV_CAPABLITIES2_V << I3C_SLV_CAPABLITIES2_S)
#define I3C_SLV_CAPABLITIES2_V  0xFFFFFFFFU
#define I3C_SLV_CAPABLITIES2_S  0

/** I3C_SLV_CAPABILITIES_REG register
 *  NA
 */
#define I3C_SLV_CAPABILITIES_REG (DR_REG_I3C_SLV_BASE + 0x60)
/** I3C_SLV_CAPABLITIES : RO; bitpos: [31:0]; default: 2081684508;
 *  NA
 */
#define I3C_SLV_CAPABLITIES    0xFFFFFFFFU
#define I3C_SLV_CAPABLITIES_M  (I3C_SLV_CAPABLITIES_V << I3C_SLV_CAPABLITIES_S)
#define I3C_SLV_CAPABLITIES_V  0xFFFFFFFFU
#define I3C_SLV_CAPABLITIES_S  0

/** I3C_SLV_IDPARTNO_REG register
 *  NA
 */
#define I3C_SLV_IDPARTNO_REG (DR_REG_I3C_SLV_BASE + 0x6c)
/** I3C_SLV_PARTNO : R/W; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_SLV_PARTNO    0xFFFFFFFFU
#define I3C_SLV_PARTNO_M  (I3C_SLV_PARTNO_V << I3C_SLV_PARTNO_S)
#define I3C_SLV_PARTNO_V  0xFFFFFFFFU
#define I3C_SLV_PARTNO_S  0

/** I3C_SLV_IDEXT_REG register
 *  NA
 */
#define I3C_SLV_IDEXT_REG (DR_REG_I3C_SLV_BASE + 0x70)
/** I3C_SLV_IDEXT : R/W; bitpos: [31:0]; default: 0;
 *  NA
 */
#define I3C_SLV_IDEXT    0xFFFFFFFFU
#define I3C_SLV_IDEXT_M  (I3C_SLV_IDEXT_V << I3C_SLV_IDEXT_S)
#define I3C_SLV_IDEXT_V  0xFFFFFFFFU
#define I3C_SLV_IDEXT_S  0

/** I3C_SLV_VENDORID_REG register
 *  NA
 */
#define I3C_SLV_VENDORID_REG (DR_REG_I3C_SLV_BASE + 0x74)
/** I3C_SLV_VID : R/W; bitpos: [14:0]; default: 21840;
 *  NA
 */
#define I3C_SLV_VID    0x00007FFFU
#define I3C_SLV_VID_M  (I3C_SLV_VID_V << I3C_SLV_VID_S)
#define I3C_SLV_VID_V  0x00007FFFU
#define I3C_SLV_VID_S  0

#ifdef __cplusplus
}
#endif
