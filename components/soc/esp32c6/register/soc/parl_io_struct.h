/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: PARL_IO RX Configuration0 */
/** Type of rx_cfg0 register
 *  Parallel RX module configuration register0.
 */
typedef union {
    struct {
        /** rx_eof_gen_sel : R/W; bitpos: [0]; default: 0;
         *  Write 0 to select eof generated manchnism by configured data byte length. Write 1
         *  to select eof generated manchnism by external enable signal.
         */
        uint32_t rx_eof_gen_sel:1;
        /** rx_start : R/W; bitpos: [1]; default: 0;
         *  Write 1 to start rx global data sampling.
         */
        uint32_t rx_start:1;
        /** rx_data_bytelen : R/W; bitpos: [17:2]; default: 0;
         *  Configures rx received data byte length.
         */
        uint32_t rx_data_bytelen:16;
        /** rx_sw_en : R/W; bitpos: [18]; default: 0;
         *  Write 1 to enable software data sampling.
         */
        uint32_t rx_sw_en:1;
        /** rx_pulse_submode_sel : R/W; bitpos: [22:19]; default: 0;
         *  Pulse submode selection.
         *  0000: positive pulse start(data bit included) &&  positive pulse end(data bit
         *  included)
         *  0001: positive pulse start(data bit included) && positive pulse end (data bit
         *  excluded)
         *  0010: positive pulse start(data bit excluded) && positive pulse end (data bit
         *  included)
         *  0011: positive pulse start(data bit excluded) && positive pulse end (data bit
         *  excluded)
         *  0100: positive pulse start(data bit included) && length end
         *  0101: positive pulse start(data bit excluded) && length end
         *  0110: negative pulse start(data bit included) &&  negative pulse end(data bit
         *  included)
         *  0111: negative pulse start(data bit included) && negative pulse end (data bit
         *  excluded)
         *  1000: negative pulse start(data bit excluded) && negative pulse end (data bit
         *  included)
         *  1001: negative pulse start(data bit excluded) && negative pulse end (data bit
         *  excluded)
         *  1010: negative pulse start(data bit included) && length end
         *  1011: negative pulse start(data bit excluded) && length end
         */
        uint32_t rx_pulse_submode_sel:4;
        /** rx_level_submode_sel : R/W; bitpos: [23]; default: 0;
         *  Write 0 to sample data at high level of external enable signal. Write 1 to sample
         *  data at low level of external enable signal.
         */
        uint32_t rx_level_submode_sel:1;
        /** rx_smp_mode_sel : R/W; bitpos: [25:24]; default: 0;
         *  Rx data sampling mode selection.
         *  000: external level enable mode
         *  001: external pulse enable mode
         *  010: internal software enable mode
         */
        uint32_t rx_smp_mode_sel:2;
        /** rx_clk_edge_sel : R/W; bitpos: [26]; default: 0;
         *  Write 0 to enable sampling data on the rising edge of rx clock. Write 0 to enable
         *  sampling data on the falling edge of rx clock.
         */
        uint32_t rx_clk_edge_sel:1;
        /** rx_bit_pack_order : R/W; bitpos: [27]; default: 0;
         *  Write 0 to pack bits into 1byte from MSB when data bus width is 4/2/1 bits. Write 0
         *  to pack bits into 1byte from LSB when data bus width is 4/2/1 bits.
         */
        uint32_t rx_bit_pack_order:1;
        /** rx_bus_wid_sel : R/W; bitpos: [30:28]; default: 0;
         *  Rx data bus width selection.
         *  100: bus width is 1 bit
         *  011: bus width is 2 bits
         *  010: bus width is 4 bits
         *  001: bus width is 8 bits
         *  000: bus width is 16 bits
         */
        uint32_t rx_bus_wid_sel:3;
        /** rx_fifo_srst : R/W; bitpos: [31]; default: 0;
         *  Write 1 to enable soft reset of async fifo in rx module.
         */
        uint32_t rx_fifo_srst:1;
    };
    uint32_t val;
} parl_io_rx_cfg0_reg_t;


/** Group: PARL_IO RX Configuration1 */
/** Type of rx_cfg1 register
 *  Parallel RX module configuration register1.
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** rx_reg_update : WT; bitpos: [2]; default: 0;
         *  Write 1 to update rx register configuration signals.
         */
        uint32_t rx_reg_update:1;
        /** rx_timeout_en : R/W; bitpos: [3]; default: 1;
         *  Write 1 to enable timeout count to generate error eof.
         */
        uint32_t rx_timeout_en:1;
        uint32_t reserved_4:8;
        /** rx_ext_en_sel : R/W; bitpos: [15:12]; default: 15;
         *  Configures rx external enable signal selection from 16 data lines.
         */
        uint32_t rx_ext_en_sel:4;
        /** rx_timeout_threshold : R/W; bitpos: [31:16]; default: 4095;
         *  Configures rx threshold of timeout counter.
         */
        uint32_t rx_timeout_threshold:16;
    };
    uint32_t val;
} parl_io_rx_cfg1_reg_t;


/** Group: PARL_IO TX Configuration0 */
/** Type of tx_cfg0 register
 *  Parallel TX module configuration register0.
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** tx_bytelen : R/W; bitpos: [17:2]; default: 0;
         *  Configures tx sending data byte length.
         */
        uint32_t tx_bytelen:16;
        /** tx_gating_en : R/W; bitpos: [18]; default: 0;
         *  Write 1 to enable output tx clock gating.
         */
        uint32_t tx_gating_en:1;
        /** tx_start : R/W; bitpos: [19]; default: 0;
         *  Write 1 to start tx global data output.
         */
        uint32_t tx_start:1;
        /** tx_hw_valid_en : R/W; bitpos: [20]; default: 0;
         *  Write 1 to enable tx hardware data valid signal.
         */
        uint32_t tx_hw_valid_en:1;
        uint32_t reserved_21:4;
        /** tx_smp_edge_sel : R/W; bitpos: [25]; default: 0;
         *  Write 0 to enable sampling data on the rising edge of tx clock. Write 0 to enable
         *  sampling data on the falling edge of tx clock.
         */
        uint32_t tx_smp_edge_sel:1;
        /** tx_bit_unpack_order : R/W; bitpos: [26]; default: 0;
         *  Write 0 to unpack bits from 1byte from MSB when data bus width is 4/2/1 bits. Write
         *  0 to unpack bits from 1byte from LSB when data bus width is 4/2/1 bits.
         */
        uint32_t tx_bit_unpack_order:1;
        /** tx_bus_wid_sel : R/W; bitpos: [29:27]; default: 0;
         *  Tx data bus width selection.
         *  100: bus width is 1 bit
         *  011: bus width is 2 bits
         *  010: bus width is 4 bits
         *  001: bus width is 8 bits
         *  000: bus width is 16 bits
         */
        uint32_t tx_bus_wid_sel:3;
        /** tx_fifo_srst : R/W; bitpos: [30]; default: 0;
         *  Write 1 to enable soft reset of async fifo in tx module.
         */
        uint32_t tx_fifo_srst:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} parl_io_tx_cfg0_reg_t;


/** Group: PARL_IO TX Configuration1 */
/** Type of tx_cfg1 register
 *  Parallel TX module configuration register1.
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** tx_idle_value : R/W; bitpos: [31:16]; default: 0;
         *  Configures data value on tx bus when IDLE state.
         */
        uint32_t tx_idle_value:16;
    };
    uint32_t val;
} parl_io_tx_cfg1_reg_t;


/** Group: PARL_IO TX Status0 */
/** Type of st register
 *  Parallel IO module status register0.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** tx_ready : RO; bitpos: [31]; default: 0;
         *  Represents the status that tx is ready.
         */
        uint32_t tx_ready:1;
    };
    uint32_t val;
} parl_io_st_reg_t;


/** Group: PARL_IO Interrupt Configuration and Status */
/** Type of int_ena register
 *  Parallel IO interrupt enable signal configuration register.
 */
typedef union {
    struct {
        /** tx_fifo_rempty_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable TX_FIFO_REMPTY_INTR.
         */
        uint32_t tx_fifo_rempty_int_ena:1;
        /** rx_fifo_wfull_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable RX_FIFO_WFULL_INTR.
         */
        uint32_t rx_fifo_wfull_int_ena:1;
        /** tx_eof_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable TX_EOF_INTR.
         */
        uint32_t tx_eof_int_ena:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} parl_io_int_ena_reg_t;

/** Type of int_raw register
 *  Parallel IO interrupt raw signal status register.
 */
typedef union {
    struct {
        /** tx_fifo_rempty_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of TX_FIFO_REMPTY_INTR.
         */
        uint32_t tx_fifo_rempty_int_raw:1;
        /** rx_fifo_wfull_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of RX_FIFO_WFULL_INTR.
         */
        uint32_t rx_fifo_wfull_int_raw:1;
        /** tx_eof_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of TX_EOF_INTR.
         */
        uint32_t tx_eof_int_raw:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} parl_io_int_raw_reg_t;

/** Type of int_st register
 *  Parallel IO interrupt signal status register.
 */
typedef union {
    struct {
        /** tx_fifo_rempty_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of TX_FIFO_REMPTY_INTR.
         */
        uint32_t tx_fifo_rempty_int_st:1;
        /** rx_fifo_wfull_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of RX_FIFO_WFULL_INTR.
         */
        uint32_t rx_fifo_wfull_int_st:1;
        /** tx_eof_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of TX_EOF_INTR.
         */
        uint32_t tx_eof_int_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} parl_io_int_st_reg_t;

/** Type of int_clr register
 *  Parallel IO interrupt  clear signal configuration register.
 */
typedef union {
    struct {
        /** tx_fifo_rempty_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear TX_FIFO_REMPTY_INTR.
         */
        uint32_t tx_fifo_rempty_int_clr:1;
        /** rx_fifo_wfull_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear RX_FIFO_WFULL_INTR.
         */
        uint32_t rx_fifo_wfull_int_clr:1;
        /** tx_eof_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear TX_EOF_INTR.
         */
        uint32_t tx_eof_int_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} parl_io_int_clr_reg_t;


/** Group: PARL_IO Clock Gating Configuration */
/** Type of clk register
 *  Parallel IO clk configuration register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Force clock on for this register file
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} parl_io_clk_reg_t;


/** Group: PARL_IO Version Register */
/** Type of version register
 *  Version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35660352;
         *  Version of this register file
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} parl_io_version_reg_t;


typedef struct parl_io_dev_t {
    volatile parl_io_rx_cfg0_reg_t rx_cfg0;
    volatile parl_io_rx_cfg1_reg_t rx_cfg1;
    volatile parl_io_tx_cfg0_reg_t tx_cfg0;
    volatile parl_io_tx_cfg1_reg_t tx_cfg1;
    volatile parl_io_st_reg_t st;
    volatile parl_io_int_ena_reg_t int_ena;
    volatile parl_io_int_raw_reg_t int_raw;
    volatile parl_io_int_st_reg_t int_st;
    volatile parl_io_int_clr_reg_t int_clr;
    uint32_t reserved_024[63];
    volatile parl_io_clk_reg_t clk;
    uint32_t reserved_124[182];
    volatile parl_io_version_reg_t version;
} parl_io_dev_t;

extern parl_io_dev_t PARL_IO;

#ifndef __cplusplus
_Static_assert(sizeof(parl_io_dev_t) == 0x400, "Invalid size of parl_io_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
