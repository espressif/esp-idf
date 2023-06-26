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

/** Group: Control and configuration registers */
/** Type of tx_inst_cfg0 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** tx_inst_idx : R/W; bitpos: [2:0]; default: 0;
         *  write this bits to specify the one of 8 instruction
         */
        uint32_t tx_inst_idx:3;
        /** tx_inst_pos : R/W; bitpos: [6:3]; default: 0;
         *  write this bits to specify the bit position of 257 bit instruction which in units
         *  of 32 bits
         */
        uint32_t tx_inst_pos:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} bitscrambler_tx_inst_cfg0_reg_t;

/** Type of tx_inst_cfg1 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** tx_inst : R/W; bitpos: [31:0]; default: 4;
         *  write this bits to update instruction which specified by
         *  BITSCRAMBLER_TX_INST_CFG0_REG, Read this bits to get instruction which specified by
         *  BITSCRAMBLER_TX_INST_CFG0_REG
         */
        uint32_t tx_inst:32;
    };
    uint32_t val;
} bitscrambler_tx_inst_cfg1_reg_t;

/** Type of rx_inst_cfg0 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** rx_inst_idx : R/W; bitpos: [2:0]; default: 0;
         *  write this bits to specify the one of 8 instruction
         */
        uint32_t rx_inst_idx:3;
        /** rx_inst_pos : R/W; bitpos: [6:3]; default: 0;
         *  write this bits to specify the bit position of 257 bit instruction which in units
         *  of 32 bits
         */
        uint32_t rx_inst_pos:4;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} bitscrambler_rx_inst_cfg0_reg_t;

/** Type of rx_inst_cfg1 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** rx_inst : R/W; bitpos: [31:0]; default: 12;
         *  write this bits to update instruction which specified by
         *  BITSCRAMBLER_RX_INST_CFG0_REG, Read this bits to get instruction which specified by
         *  BITSCRAMBLER_RX_INST_CFG0_REG
         */
        uint32_t rx_inst:32;
    };
    uint32_t val;
} bitscrambler_rx_inst_cfg1_reg_t;

/** Type of tx_lut_cfg0 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** tx_lut_idx : R/W; bitpos: [10:0]; default: 0;
         *  write this bits to specify the bytes position of LUT RAM based on
         *  reg_bitscrambler_tx_lut_mode
         */
        uint32_t tx_lut_idx:11;
        /** tx_lut_mode : R/W; bitpos: [12:11]; default: 0;
         *  write this bits to specify the bytes mode of LUT RAM, 0: 1 byte,1: 2bytes, 2: 4
         *  bytes
         */
        uint32_t tx_lut_mode:2;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} bitscrambler_tx_lut_cfg0_reg_t;

/** Type of tx_lut_cfg1 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** tx_lut : R/W; bitpos: [31:0]; default: 20;
         *  write this bits to update LUT which specified by BITSCRAMBLER_TX_LUT_CFG0_REG, Read
         *  this bits to get LUT which specified by BITSCRAMBLER_TX_LUT_CFG0_REG
         */
        uint32_t tx_lut:32;
    };
    uint32_t val;
} bitscrambler_tx_lut_cfg1_reg_t;

/** Type of rx_lut_cfg0 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** rx_lut_idx : R/W; bitpos: [10:0]; default: 0;
         *  write this bits to specify the bytes position of LUT RAM based on
         *  reg_bitscrambler_rx_lut_mode
         */
        uint32_t rx_lut_idx:11;
        /** rx_lut_mode : R/W; bitpos: [12:11]; default: 0;
         *  write this bits to specify the bytes mode of LUT RAM, 0: 1 byte,1: 2bytes, 2: 4
         *  bytes
         */
        uint32_t rx_lut_mode:2;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} bitscrambler_rx_lut_cfg0_reg_t;

/** Type of rx_lut_cfg1 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** rx_lut : R/W; bitpos: [31:0]; default: 28;
         *  write this bits to update LUT which specified by BITSCRAMBLER_RX_LUT_CFG0_REG, Read
         *  this bits to get LUT which specified by BITSCRAMBLER_RX_LUT_CFG0_REG
         */
        uint32_t rx_lut:32;
    };
    uint32_t val;
} bitscrambler_rx_lut_cfg1_reg_t;


/** Group: Configuration registers */
/** Type of tx_tailing_bits register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** tx_tailing_bits : R/W; bitpos: [15:0]; default: 0;
         *  write this bits to specify the extra data bit length after getting EOF
         */
        uint32_t tx_tailing_bits:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} bitscrambler_tx_tailing_bits_reg_t;

/** Type of rx_tailing_bits register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** rx_tailing_bits : R/W; bitpos: [15:0]; default: 0;
         *  write this bits to specify the extra data bit length after getting EOF
         */
        uint32_t rx_tailing_bits:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} bitscrambler_rx_tailing_bits_reg_t;

/** Type of tx_ctrl register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** tx_ena : R/W; bitpos: [0]; default: 0;
         *  write this bit to enable the bitscrambler tx
         */
        uint32_t tx_ena:1;
        /** tx_pause : R/W; bitpos: [1]; default: 0;
         *  write this bit to pause the bitscrambler tx core
         */
        uint32_t tx_pause:1;
        /** tx_halt : R/W; bitpos: [2]; default: 1;
         *  write this bit to halt the bitscrambler tx core
         */
        uint32_t tx_halt:1;
        /** tx_eof_mode : R/W; bitpos: [3]; default: 0;
         *  write this bit to ser the bitscrambler tx core EOF signal generating mode which is
         *  combined with reg_bitscrambler_tx_tailing_bits, 0: counter by read dma fifo, 0
         *  counter by write peripheral buffer
         */
        uint32_t tx_eof_mode:1;
        /** tx_cond_mode : R/W; bitpos: [4]; default: 0;
         *  write this bit to specify the LOOP instruction condition mode of bitscrambler tx
         *  core, 0: use the little than operator to get the condition, 1: use not equal
         *  operator to get the condition
         */
        uint32_t tx_cond_mode:1;
        /** tx_fetch_mode : R/W; bitpos: [5]; default: 0;
         *  write this bit to set the bitscrambler tx core fetch instruction mode, 0: prefetch
         *  by reset, 1: fetch by instrutions
         */
        uint32_t tx_fetch_mode:1;
        /** tx_halt_mode : R/W; bitpos: [6]; default: 0;
         *  write this bit to set the bitscrambler tx core halt mode when tx_halt is set, 0:
         *  wait write data back done, , 1: ignore write data back
         */
        uint32_t tx_halt_mode:1;
        /** tx_rd_dummy : R/W; bitpos: [7]; default: 0;
         *  write this bit to set the bitscrambler tx core read data mode when EOF received.0:
         *  wait read data, 1: ignore read data
         */
        uint32_t tx_rd_dummy:1;
        /** tx_fifo_rst : WT; bitpos: [8]; default: 0;
         *  write this bit to reset the bitscrambler tx fifo
         */
        uint32_t tx_fifo_rst:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} bitscrambler_tx_ctrl_reg_t;

/** Type of rx_ctrl register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** rx_ena : R/W; bitpos: [0]; default: 0;
         *  write this bit to enable the bitscrambler rx
         */
        uint32_t rx_ena:1;
        /** rx_pause : R/W; bitpos: [1]; default: 0;
         *  write this bit to pause the bitscrambler rx core
         */
        uint32_t rx_pause:1;
        /** rx_halt : R/W; bitpos: [2]; default: 1;
         *  write this bit to halt the bitscrambler rx core
         */
        uint32_t rx_halt:1;
        /** rx_eof_mode : R/W; bitpos: [3]; default: 0;
         *  write this bit to ser the bitscrambler rx core EOF signal generating mode which is
         *  combined with reg_bitscrambler_rx_tailing_bits, 0: counter by read peripheral
         *  buffer, 0 counter by write dma fifo
         */
        uint32_t rx_eof_mode:1;
        /** rx_cond_mode : R/W; bitpos: [4]; default: 0;
         *  write this bit to specify the LOOP instruction condition mode of bitscrambler rx
         *  core, 0: use the little than operator to get the condition, 1: use not equal
         *  operator to get the condition
         */
        uint32_t rx_cond_mode:1;
        /** rx_fetch_mode : R/W; bitpos: [5]; default: 0;
         *  write this bit to set the bitscrambler rx core fetch instruction mode, 0: prefetch
         *  by reset, 1: fetch by instrutions
         */
        uint32_t rx_fetch_mode:1;
        /** rx_halt_mode : R/W; bitpos: [6]; default: 0;
         *  write this bit to set the bitscrambler rx core halt mode when rx_halt is set, 0:
         *  wait write data back done, , 1: ignore write data back
         */
        uint32_t rx_halt_mode:1;
        /** rx_rd_dummy : R/W; bitpos: [7]; default: 0;
         *  write this bit to set the bitscrambler rx core read data mode when EOF received.0:
         *  wait read data, 1: ignore read data
         */
        uint32_t rx_rd_dummy:1;
        /** rx_fifo_rst : WT; bitpos: [8]; default: 0;
         *  write this bit to reset the bitscrambler rx fifo
         */
        uint32_t rx_fifo_rst:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} bitscrambler_rx_ctrl_reg_t;

/** Type of sys register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** loop_mode : R/W; bitpos: [0]; default: 0;
         *  write this bit to set the bitscrambler tx loop back to DMA rx
         */
        uint32_t loop_mode:1;
        uint32_t reserved_1:30;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} bitscrambler_sys_reg_t;


/** Group: Status registers */
/** Type of tx_state register
 *  Status registers
 */
typedef union {
    struct {
        /** tx_in_idle : RO; bitpos: [0]; default: 1;
         *  represents the bitscrambler tx core in halt mode
         */
        uint32_t tx_in_idle:1;
        /** tx_in_run : RO; bitpos: [1]; default: 0;
         *  represents the bitscrambler tx core in run mode
         */
        uint32_t tx_in_run:1;
        /** tx_in_wait : RO; bitpos: [2]; default: 0;
         *  represents the bitscrambler tx core in wait mode to wait write back done
         */
        uint32_t tx_in_wait:1;
        /** tx_in_pause : RO; bitpos: [3]; default: 0;
         *  represents the bitscrambler tx core in pause mode
         */
        uint32_t tx_in_pause:1;
        /** tx_fifo_empty : RO; bitpos: [4]; default: 0;
         *  represents the bitscrambler tx fifo in empty state
         */
        uint32_t tx_fifo_empty:1;
        uint32_t reserved_5:11;
        /** tx_eof_get_cnt : RO; bitpos: [29:16]; default: 0;
         *  represents the bytes numbers of bitscrambler tx core when get EOF
         */
        uint32_t tx_eof_get_cnt:14;
        /** tx_eof_overload : RO; bitpos: [30]; default: 0;
         *  represents the some EOFs will be lost for bitscrambler tx core
         */
        uint32_t tx_eof_overload:1;
        /** tx_eof_trace_clr : WT; bitpos: [31]; default: 0;
         *  write this bit to clear reg_bitscrambler_tx_eof_overload and
         *  reg_bitscrambler_tx_eof_get_cnt registers
         */
        uint32_t tx_eof_trace_clr:1;
    };
    uint32_t val;
} bitscrambler_tx_state_reg_t;

/** Type of rx_state register
 *  Status registers
 */
typedef union {
    struct {
        /** rx_in_idle : RO; bitpos: [0]; default: 1;
         *  represents the bitscrambler rx core in halt mode
         */
        uint32_t rx_in_idle:1;
        /** rx_in_run : RO; bitpos: [1]; default: 0;
         *  represents the bitscrambler rx core in run mode
         */
        uint32_t rx_in_run:1;
        /** rx_in_wait : RO; bitpos: [2]; default: 0;
         *  represents the bitscrambler rx core in wait mode to wait write back done
         */
        uint32_t rx_in_wait:1;
        /** rx_in_pause : RO; bitpos: [3]; default: 0;
         *  represents the bitscrambler rx core in pause mode
         */
        uint32_t rx_in_pause:1;
        /** rx_fifo_full : RO; bitpos: [4]; default: 0;
         *  represents the bitscrambler rx fifo in full state
         */
        uint32_t rx_fifo_full:1;
        uint32_t reserved_5:11;
        /** rx_eof_get_cnt : RO; bitpos: [29:16]; default: 0;
         *  represents the bytes numbers of bitscrambler rx core when get EOF
         */
        uint32_t rx_eof_get_cnt:14;
        /** rx_eof_overload : RO; bitpos: [30]; default: 0;
         *  represents the some EOFs will be lost for bitscrambler rx core
         */
        uint32_t rx_eof_overload:1;
        /** rx_eof_trace_clr : WT; bitpos: [31]; default: 0;
         *  write this bit to clear reg_bitscrambler_rx_eof_overload and
         *  reg_bitscrambler_rx_eof_get_cnt registers
         */
        uint32_t rx_eof_trace_clr:1;
    };
    uint32_t val;
} bitscrambler_rx_state_reg_t;


/** Group: Version register */
/** Type of version register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** bitscrambler_ver : R/W; bitpos: [27:0]; default: 36713024;
         *  Reserved
         */
        uint32_t bitscrambler_ver:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} bitscrambler_version_reg_t;


typedef struct {
    volatile bitscrambler_tx_inst_cfg0_reg_t tx_inst_cfg0;
    volatile bitscrambler_tx_inst_cfg1_reg_t tx_inst_cfg1;
    volatile bitscrambler_rx_inst_cfg0_reg_t rx_inst_cfg0;
    volatile bitscrambler_rx_inst_cfg1_reg_t rx_inst_cfg1;
    volatile bitscrambler_tx_lut_cfg0_reg_t tx_lut_cfg0;
    volatile bitscrambler_tx_lut_cfg1_reg_t tx_lut_cfg1;
    volatile bitscrambler_rx_lut_cfg0_reg_t rx_lut_cfg0;
    volatile bitscrambler_rx_lut_cfg1_reg_t rx_lut_cfg1;
    volatile bitscrambler_tx_tailing_bits_reg_t tx_tailing_bits;
    volatile bitscrambler_rx_tailing_bits_reg_t rx_tailing_bits;
    volatile bitscrambler_tx_ctrl_reg_t tx_ctrl;
    volatile bitscrambler_rx_ctrl_reg_t rx_ctrl;
    volatile bitscrambler_tx_state_reg_t tx_state;
    volatile bitscrambler_rx_state_reg_t rx_state;
    uint32_t reserved_038[48];
    volatile bitscrambler_sys_reg_t sys;
    volatile bitscrambler_version_reg_t version;
} bitscrambler_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(bitscrambler_dev_t) == 0x100, "Invalid size of bitscrambler_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
