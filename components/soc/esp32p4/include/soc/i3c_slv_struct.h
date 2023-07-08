/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: I3C_SLV CONFIG REG */
/** Type of config register
 *  NA
 */
typedef union {
    struct {
        /** slvena : R/W; bitpos: [0]; default: 1;
         *  1: allow the slave to operate on i2c or i3c bus. 0: the slave will ignore the bus.
         *  This should be not set until registers such as PARTNO, IDEXT and the like are set
         *  1st  -if used- since they impact data to the master
         */
        uint32_t slvena:1;
        /** nack : R/W; bitpos: [1]; default: 0;
         *  1:the slave will NACK all requests to it except CCC broadcast. This should be used
         *  with caution as the Master may determine the slave is missing if overused.
         */
        uint32_t nack:1;
        /** matchss : R/W; bitpos: [2]; default: 0;
         *  1: the START and STOP sticky STATUS bits will only be set if MATCHED is set..This
         *  allows START and STOP to be used to detect end of a message to /from this slave.
         */
        uint32_t matchss:1;
        /** s0ignore : R/W; bitpos: [3]; default: 0;
         *  If 1, the Slave will not detect S0 or S1 errors and so not lock up waiting on an
         *  Exit Pattern. This should only be used when the bus will not use HDR.
         */
        uint32_t s0ignore:1;
        /** ddrok : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t ddrok:1;
        uint32_t reserved_5:3;
        /** idrand : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t idrand:1;
        /** offline : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t offline:1;
        uint32_t reserved_10:6;
        /** bamatch : R/W; bitpos: [23:16]; default: 47;
         *  Bus Available condition match value for current ???Slow clock???. This provides the
         *  count of the slow clock to count out 1us (or more) to allow an IBI to drive SDA Low
         *  when the Master is not doing so. The max width , and so max value, is controlled by
         *  the block. Only if enabled for events such IBI or MR or HJ, and if enabled to
         *  provide this as a register. With is limited to CLK_SLOW_BITS
         */
        uint32_t bamatch:8;
        uint32_t reserved_24:1;
        /** saddr : R/W; bitpos: [31:25]; default: 0;
         *  If allowed by the block:sets i2c 7 bits static address,else should be 0. If enabled
         *  to use one and to be provided by SW. Block may provide in HW as well.
         */
        uint32_t saddr:7;
    };
    uint32_t val;
} i3c_slv_config_reg_t;


/** Group: I3C_SLV STATUS REG */
/** Type of status register
 *  NA
 */
typedef union {
    struct {
        /** stnotstop : RO; bitpos: [0]; default: 0;
         *  Is 1 if bus is busy(activity) and 0 when in a STOP condition. Other bits may also
         *  set when busy. Note that this can also be true from an S0 or S1 error, which waits
         *  for an Exit Pattern.
         */
        uint32_t stnotstop:1;
        /** stmsg : RO; bitpos: [1]; default: 0;
         *  Is 1 if this bus Slave is listening to the bus traffic or repsonding, If
         *  STNOSTOP=1, then this will be 0 when a non-matching address seen until next
         *  respeated START it STOP.
         */
        uint32_t stmsg:1;
        /** stccch : RO; bitpos: [2]; default: 0;
         *  Is 1 if a CCC message is being handled automatically.
         */
        uint32_t stccch:1;
        /** streqrd : RO; bitpos: [3]; default: 0;
         *  1 if the req in process is an sdr read from this slave or an IBI is being pushed
         *  out,
         */
        uint32_t streqrd:1;
        /** streqwr : RO; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t streqwr:1;
        /** stdaa : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t stdaa:1;
        /** sthdr : RO; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t sthdr:1;
        uint32_t reserved_7:1;
        /** start : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t start:1;
        /** matched : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t matched:1;
        /** stop : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t stop:1;
        /** rxpend : RO; bitpos: [11]; default: 0;
         *  Receiving a message from master,which is not being handled by block(not a CCC
         *  internally processed). For all but External FIFO, this uses DATACTRL RXTRIG, which
         *  defaults to not-empty. If DMA is enabled for RX, DMA will be signaled as well. Will
         *  self-clear if data is read(FIFO and non-FIFO)
         */
        uint32_t rxpend:1;
        /** txnotfull : RO; bitpos: [12]; default: 0;
         *  Is 1 when the To-bus buffer/FIFO can accept more data to go out. Defau:1. For all
         *  but External FIFO, this uses DATACTRL TXTRIG,which defaults to not-full. If DMA is
         *  enabled for TX, it will also be signaled to provide more.
         */
        uint32_t txnotfull:1;
        /** dachg : R/W; bitpos: [13]; default: 0;
         *  The Slv Dynamic Address has been assigned, reassigned, or reset(lost) and is now in
         *  that state of being valid or none. Actual DA can be seen in the DYNADDR register.
         *  Note that this will also be used when MAP Auto feature is configured. This will be
         *  changing one or more MAP items. See DYNADDR and/or MAPCTRLn. DYNAADDR for the main
         *  DA(0) will indicate if last change was due to Auto MAP.
         */
        uint32_t dachg:1;
        /** ccc : R/W; bitpos: [14]; default: 0;
         *  A common -command-code(CCC), not handled by block, has been received. This acts
         *  differently between: *Broadcasted ones, which will then also correspond with RXPEND
         *  and the 1st byte will be the CCC(command) . *Direct ones, which may never be
         *  directed to this device.  If it is, then the TXSEND or RXPEND will be triggered
         *  with this end the RXPEND will contain the command.
         */
        uint32_t ccc:1;
        /** errwarn : RO; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t errwarn:1;
        /** hdrmatch : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t hdrmatch:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} i3c_slv_status_reg_t;


/** Group: I3C_SLV CTRL REG */
/** Type of ctrl register
 *  NA
 */
typedef union {
    struct {
        /** slv_event : R/W; bitpos: [1:0]; default: 0;
         *  If set to non-0, will request an event. Once requested, STATUS.EVENT and EVDET will
         *  show the status as it progresses. Once completed, the field will automatically
         *  return to 0. Once non-0, only 0 can be written(to cancel) until done. 0: Normal
         *  mode. If set to 0 after was a non-0 value, will cancel if not already in flight. 1:
         *  start an IBI. This will try to push through an IBI on the bus. If data associate
         *  with the IBI, it will be drawn from the IBIDATA field. Note that if Time control is
         *  enabled, this will include anytime control related bytes  further, the IBIDATA byte
         *  will have bit7 set to 1.
         */
        uint32_t slv_event:2;
        uint32_t reserved_2:1;
        /** extdata : R/W; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t extdata:1;
        /** mapidx : R/W; bitpos: [7:4]; default: 0;
         *  Index of Dynamic Address that IBI is for. This is 0 for the main or base Dynamic
         *  Address, or can be any valid index.
         */
        uint32_t mapidx:4;
        /** ibidata : R/W; bitpos: [15:8]; default: 0;
         *  Data byte to go with an IBI, if enabled for it. If enabled (was in BCR), then it is
         *  required.
         */
        uint32_t ibidata:8;
        /** pendint : R/W; bitpos: [19:16]; default: 0;
         *  Should be set to the pending interrupt that GETSTATUS CCC will return. This should
         *  be maintained by the application if used and configured, as the Master will read
         *  this. If not configured, the GETSTATUS field will return 1 if an IBI is pending,
         *  and 0 otherwise.
         */
        uint32_t pendint:4;
        /** actstate : R/W; bitpos: [21:20]; default: 0;
         *  NA
         */
        uint32_t actstate:2;
        uint32_t reserved_22:2;
        /** vendinfo : R/W; bitpos: [31:24]; default: 0;
         *  NA
         */
        uint32_t vendinfo:8;
    };
    uint32_t val;
} i3c_slv_ctrl_reg_t;


/** Group: I3C_SLV INTSET REG */
/** Type of intset register
 *  INSET allows setting enables for interrupts(connecting the corresponding STATUS
 *  source to causing an IRQ to the processor)
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** stop_ena : R/W; bitpos: [10]; default: 0;
         *  Interrupt on STOP state on the bus. See Start as the preferred interrupt when
         *  needed. This interrupt may not trigger for quick STOP/START combination, as it
         *  relates to the state of being stopped.
         */
        uint32_t stop_ena:1;
        /** rxpend_ena : R/W; bitpos: [11]; default: 0;
         *  Interrupt when receiving a message from Master, which is not being handled by the
         *  block (excludes CCCs being handled automatically). If FIFO, then RX fullness
         *  trigger. If DMA, then message end.
         */
        uint32_t rxpend_ena:1;
        /** txsend_ena : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t txsend_ena:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} i3c_slv_intset_reg_t;


/** Group: I3C_SLV INTCLR REG */
/** Type of intclr register
 *  NA
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** stop_clr : WO; bitpos: [10]; default: 0;
         *  Interrupt on STOP state on the bus. See Start as the preferred interrupt when
         *  needed. This interrupt may not trigger for quick STOP/START combination, as it
         *  relates to the state of being stopped.
         */
        uint32_t stop_clr:1;
        /** rxpend_clr : WO; bitpos: [11]; default: 0;
         *  Interrupt when receiving a message from Master, which is not being handled by the
         *  block (excludes CCCs being handled automatically). If FIFO, then RX fullness
         *  trigger. If DMA, then message end.
         */
        uint32_t rxpend_clr:1;
        /** txsend_clr : WO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t txsend_clr:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} i3c_slv_intclr_reg_t;


/** Group: I3C_SLV INTMASKED REG */
/** Type of intmasked register
 *  NA
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** stop_mask : RO; bitpos: [10]; default: 0;
         *  Interrupt on STOP state on the bus. See Start as the preferred interrupt when
         *  needed. This interrupt may not trigger for quick STOP/START combination, as it
         *  relates to the state of being stopped.
         */
        uint32_t stop_mask:1;
        /** rxpend_mask : RO; bitpos: [11]; default: 0;
         *  Interrupt when receiving a message from Master, which is not being handled by the
         *  block (excludes CCCs being handled automatically). If FIFO, then RX fullness
         *  trigger. If DMA, then message end.
         */
        uint32_t rxpend_mask:1;
        /** txsend_mask : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t txsend_mask:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} i3c_slv_intmasked_reg_t;


/** Group: I3C_SLV DATACTRL REG */
/** Type of datactrl register
 *  NA
 */
typedef union {
    struct {
        /** flushtb : WO; bitpos: [0]; default: 0;
         *  Flushes the from-bus buffer/FIFO. Not normally used
         */
        uint32_t flushtb:1;
        /** flushfb : WO; bitpos: [1]; default: 0;
         *  Flushes the to-bus buffer/FIFO. Used when Master terminates a to-bus (read) message
         *  prematurely
         */
        uint32_t flushfb:1;
        uint32_t reserved_2:1;
        /** unlock : WO; bitpos: [3]; default: 0;
         *  If this bit is not written 1, the register bits from 7 to 4 are not changed on
         *  write.
         */
        uint32_t unlock:1;
        /** txtrig : R/W; bitpos: [5:4]; default: 3;
         *  Trigger level for tx emptiness when FIFOed, Affects interrupt and DMA(if enabled).
         *  The defaults is 3
         */
        uint32_t txtrig:2;
        /** rxtrig : R/W; bitpos: [7:6]; default: 2;
         *  Trigger level for rx fulless when FIFOed, Affects interrupt and DMA(if enabled).
         *  The defaults is 3
         */
        uint32_t rxtrig:2;
        uint32_t reserved_8:8;
        /** txcount : RO; bitpos: [20:16]; default: 0;
         *  NA
         */
        uint32_t txcount:5;
        uint32_t reserved_21:3;
        /** rxcount : RO; bitpos: [28:24]; default: 0;
         *  NA
         */
        uint32_t rxcount:5;
        uint32_t reserved_29:1;
        /** txfull : RO; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t txfull:1;
        /** rxempty : RO; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t rxempty:1;
    };
    uint32_t val;
} i3c_slv_datactrl_reg_t;


/** Group: I3C_SLV WDATAB REG */
/** Type of wdatab register
 *  NA
 */
typedef union {
    struct {
        /** wdatab : WO; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t wdatab:8;
        /** wdata_end : WO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t wdata_end:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i3c_slv_wdatab_reg_t;


/** Group: I3C_SLV WDATABE REG */
/** Type of wdatabe register
 *  NA
 */
typedef union {
    struct {
        /** wdatabe : WO; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t wdatabe:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} i3c_slv_wdatabe_reg_t;


/** Group: I3C_SLV RDARAB REG */
/** Type of rdarab register
 *  Read Byte Data (from-bus) register
 */
typedef union {
    struct {
        /** data0 : RO; bitpos: [7:0]; default: 0;
         *  This register allows reading a byte from the bus unless external FIFO is used. A
         *  byte should not be read unless there is data waiting, as indicated by the RXPEND
         *  bit being set in the STATUS register
         */
        uint32_t data0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} i3c_slv_rdarab_reg_t;


/** Group: I3C_SLV RDATAH REG */
/** Type of rdatah register
 *  Read Half-word Data (from-bus) register
 */
typedef union {
    struct {
        /** data_lsb : RO; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t data_lsb:8;
        /** data_msb : RO; bitpos: [15:8]; default: 0;
         *  This register allows reading a Half-word (byte pair)  from the bus unless external
         *  FIFO is used. A Half-word should not be read unless there is at least 2 bytes of
         *  data waiting, as indicated by the RX FIFO level trigger or RXCOUNT available space
         *  in the DATACTRL register
         */
        uint32_t data_msb:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} i3c_slv_rdatah_reg_t;


/** Group: I3C_SLV CAPABILITIES2 REG */
/** Type of capabilities2 register
 *  NA
 */
typedef union {
    struct {
        /** capablities2 : RO; bitpos: [31:0]; default: 256;
         *  NA
         */
        uint32_t capablities2:32;
    };
    uint32_t val;
} i3c_slv_capabilities2_reg_t;


/** Group: I3C_SLV CAPABILITIES REG */
/** Type of capabilities register
 *  NA
 */
typedef union {
    struct {
        /** capablities : RO; bitpos: [31:0]; default: 2081684508;
         *  NA
         */
        uint32_t capablities:32;
    };
    uint32_t val;
} i3c_slv_capabilities_reg_t;


/** Group: I3C_SLV IDPARTNO REG */
/** Type of idpartno register
 *  NA
 */
typedef union {
    struct {
        /** partno : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t partno:32;
    };
    uint32_t val;
} i3c_slv_idpartno_reg_t;


/** Group: I3C_SLV IDEXT REG */
/** Type of idext register
 *  NA
 */
typedef union {
    struct {
        /** idext : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t idext:32;
    };
    uint32_t val;
} i3c_slv_idext_reg_t;


/** Group: I3C_SLV VENDORID REG */
/** Type of vendorid register
 *  NA
 */
typedef union {
    struct {
        /** vid : R/W; bitpos: [14:0]; default: 21840;
         *  NA
         */
        uint32_t vid:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} i3c_slv_vendorid_reg_t;


typedef struct {
    uint32_t reserved_000;
    volatile i3c_slv_config_reg_t config;
    volatile i3c_slv_status_reg_t status;
    volatile i3c_slv_ctrl_reg_t ctrl;
    volatile i3c_slv_intset_reg_t intset;
    volatile i3c_slv_intclr_reg_t intclr;
    volatile i3c_slv_intmasked_reg_t intmasked;
    uint32_t reserved_01c[4];
    volatile i3c_slv_datactrl_reg_t datactrl;
    volatile i3c_slv_wdatab_reg_t wdatab;
    volatile i3c_slv_wdatabe_reg_t wdatabe;
    uint32_t reserved_038[2];
    volatile i3c_slv_rdarab_reg_t rdarab;
    uint32_t reserved_044;
    volatile i3c_slv_rdatah_reg_t rdatah;
    uint32_t reserved_04c[4];
    volatile i3c_slv_capabilities2_reg_t capabilities2;
    volatile i3c_slv_capabilities_reg_t capabilities;
    uint32_t reserved_064[2];
    volatile i3c_slv_idpartno_reg_t idpartno;
    volatile i3c_slv_idext_reg_t idext;
    volatile i3c_slv_vendorid_reg_t vendorid;
} i3c_slv_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(i3c_slv_dev_t) == 0x78, "Invalid size of i3c_slv_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
