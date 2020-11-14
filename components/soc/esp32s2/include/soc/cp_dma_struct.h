/** Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Interrupt Registers */
/** Type of dma_int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** dma_in_done_int_raw : RO; bitpos: [0]; default: 0;
         *  This is the interrupt raw bit. Triggered when the last data of frame is received or
         *  the receive buffer is full indicated by inlink descriptor.
         */
        uint32_t dma_in_done_int_raw: 1;
        /** dma_in_suc_eof_int_raw : RO; bitpos: [1]; default: 0;
         *  This is the interrupt raw bit. Triggered when the last data of one frame is
         *  received.
         */
        uint32_t dma_in_suc_eof_int_raw: 1;
        /** dma_out_done_int_raw : RO; bitpos: [2]; default: 0;
         *  This is the interrupt raw bit. Triggered when all data indicated by one outlink
         *  descriptor has been pushed into Tx FIFO.
         */
        uint32_t dma_out_done_int_raw: 1;
        /** dma_out_eof_int_raw : RO; bitpos: [3]; default: 0;
         *  This is the interrupt raw bit. Triggered when the last data with EOF flag has been
         *  pushed into Tx FIFO.
         */
        uint32_t dma_out_eof_int_raw: 1;
        /** dma_in_dscr_err_int_raw : RO; bitpos: [4]; default: 0;
         *  This is the interrupt raw bit. Triggered when detecting inlink descriptor error,
         *  including owner error, the second and third word error of inlink descriptor.
         */
        uint32_t dma_in_dscr_err_int_raw: 1;
        /** dma_out_dscr_err_int_raw : RO; bitpos: [5]; default: 0;
         *  This is the interrupt raw bit. Triggered when detecting outlink descriptor error,
         *  including owner error, the second and third word error of outlink descriptor.
         */
        uint32_t dma_out_dscr_err_int_raw: 1;
        /** dma_in_dscr_empty_int_raw : RO; bitpos: [6]; default: 0;
         *  This is the interrupt raw bit. Triggered when receiving data is completed and no
         *  more inlink descriptor.
         */
        uint32_t dma_in_dscr_empty_int_raw: 1;
        /** dma_out_total_eof_int_raw : RO; bitpos: [7]; default: 0;
         *  This is the interrupt raw bit. Triggered when data corresponding to all outlink
         *  descriptor and the last descriptor with valid EOF is transmitted out.
         */
        uint32_t dma_out_total_eof_int_raw: 1;
        /** dma_crc_done_int_raw : RO; bitpos: [8]; default: 0;
         *  This is the interrupt raw bit. Triggered when crc calculation is done.
         */
        uint32_t dma_crc_done_int_raw: 1;
    };
    uint32_t val;
} cp_dma_int_raw_reg_t;

/** Type of dma_int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** dma_in_done_int_st : RO; bitpos: [0]; default: 0;
         *  This is the masked interrupt bit for cp_in_done_int interrupt when
         *  cp_in_done_int_ena is set to 1.
         */
        uint32_t dma_in_done_int_st: 1;
        /** dma_in_suc_eof_int_st : RO; bitpos: [1]; default: 0;
         *  This is the masked interrupt bit for cp_in_suc_eof_int interrupt when
         *  cp_in_suc_eof_int_ena is set to 1.
         */
        uint32_t dma_in_suc_eof_int_st: 1;
        /** dma_out_done_int_st : RO; bitpos: [2]; default: 0;
         *  This is the masked interrupt bit for cp_out_done_int interrupt when
         *  cp_out_done_int_ena is set to 1.
         */
        uint32_t dma_out_done_int_st: 1;
        /** dma_out_eof_int_st : RO; bitpos: [3]; default: 0;
         *  This is the masked interrupt bit for cp_out_eof_int interrupt when
         *  cp_out_eof_int_ena is set to 1.
         */
        uint32_t dma_out_eof_int_st: 1;
        /** dma_in_dscr_err_int_st : RO; bitpos: [4]; default: 0;
         *  This is the masked interrupt bit for cp_in_dscr_err_int interrupt when
         *  cp_in_dscr_err_int_ena is set to 1.
         */
        uint32_t dma_in_dscr_err_int_st: 1;
        /** dma_out_dscr_err_int_st : RO; bitpos: [5]; default: 0;
         *  This is the masked interrupt bit for cp_out_dscr_err_int interrupt when
         *  cp_out_dscr_err_int_ena is set to 1.
         */
        uint32_t dma_out_dscr_err_int_st: 1;
        /** dma_in_dscr_empty_int_st : RO; bitpos: [6]; default: 0;
         *  This is the masked interrupt bit for cp_in_dscr_empty_int interrupt when
         *  cp_in_dscr_empty_int_ena is set to 1.
         */
        uint32_t dma_in_dscr_empty_int_st: 1;
        /** dma_out_total_eof_int_st : RO; bitpos: [7]; default: 0;
         *  This is the masked interrupt bit for cp_out_total_eof_int interrupt when
         *  cp_out_total_eof_int_ena is set to 1.
         */
        uint32_t dma_out_total_eof_int_st: 1;
        /** dma_crc_done_int_st : RO; bitpos: [8]; default: 0;
         *  This is the masked interrupt bit for cp_crc_done_int interrupt when
         *  cp_crc_done_int_ena is set to 1.
         */
        uint32_t dma_crc_done_int_st: 1;
    };
    uint32_t val;
} cp_dma_int_st_reg_t;

/** Type of dma_int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** dma_in_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  This is the interrupt enable bit for cp_in_done_int interrupt.
         */
        uint32_t dma_in_done_int_ena: 1;
        /** dma_in_suc_eof_int_ena : R/W; bitpos: [1]; default: 0;
         *  This is the interrupt enable bit for cp_in_suc_eof_int interrupt.
         */
        uint32_t dma_in_suc_eof_int_ena: 1;
        /** dma_out_done_int_ena : R/W; bitpos: [2]; default: 0;
         *  This is the interrupt enable bit for cp_out_done_int interrupt.
         */
        uint32_t dma_out_done_int_ena: 1;
        /** dma_out_eof_int_ena : R/W; bitpos: [3]; default: 0;
         *  This is the interrupt enable bit for cp_out_eof_int interrupt.
         */
        uint32_t dma_out_eof_int_ena: 1;
        /** dma_in_dscr_err_int_ena : R/W; bitpos: [4]; default: 0;
         *  This is the interrupt enable bit for cp_in_dscr_err_int interrupt.
         */
        uint32_t dma_in_dscr_err_int_ena: 1;
        /** dma_out_dscr_err_int_ena : R/W; bitpos: [5]; default: 0;
         *  This is the interrupt enable bit for cp_out_dscr_err_int interrupt.
         */
        uint32_t dma_out_dscr_err_int_ena: 1;
        /** dma_in_dscr_empty_int_ena : R/W; bitpos: [6]; default: 0;
         *  This is the interrupt enable bit for cp_in_dscr_empty_int interrupt.
         */
        uint32_t dma_in_dscr_empty_int_ena: 1;
        /** dma_out_total_eof_int_ena : R/W; bitpos: [7]; default: 0;
         *  This is the interrupt enable bit for cp_out_total_eof_int interrupt.
         */
        uint32_t dma_out_total_eof_int_ena: 1;
        /** dma_crc_done_int_ena : R/W; bitpos: [8]; default: 0;
         *  This is the interrupt enable bit for cp_crc_done_int interrupt.
         */
        uint32_t dma_crc_done_int_ena: 1;
    };
    uint32_t val;
} cp_dma_int_ena_reg_t;

/** Type of dma_int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** dma_in_done_int_clr : WO; bitpos: [0]; default: 0;
         *  Set this bit to clear cp_in_done_int interrupt.
         */
        uint32_t dma_in_done_int_clr: 1;
        /** dma_in_suc_eof_int_clr : WO; bitpos: [1]; default: 0;
         *  Set this bit to clear cp_in_suc_eof_int interrupt.
         */
        uint32_t dma_in_suc_eof_int_clr: 1;
        /** dma_out_done_int_clr : WO; bitpos: [2]; default: 0;
         *  Set this bit to clear cp_out_done_int interrupt.
         */
        uint32_t dma_out_done_int_clr: 1;
        /** dma_out_eof_int_clr : WO; bitpos: [3]; default: 0;
         *  Set this bit to clear cp_out_eof_int interrupt.
         */
        uint32_t dma_out_eof_int_clr: 1;
        /** dma_in_dscr_err_int_clr : WO; bitpos: [4]; default: 0;
         *  Set this bit to clear cp_in_dscr_err_int interrupt.
         */
        uint32_t dma_in_dscr_err_int_clr: 1;
        /** dma_out_dscr_err_int_clr : WO; bitpos: [5]; default: 0;
         *  Set this bit to clear cp_out_dscr_err_int interrupt.
         */
        uint32_t dma_out_dscr_err_int_clr: 1;
        /** dma_in_dscr_empty_int_clr : WO; bitpos: [6]; default: 0;
         *  Set this bit to clear cp_in_dscr_empty_int interrupt.
         */
        uint32_t dma_in_dscr_empty_int_clr: 1;
        /** dma_out_total_eof_int_clr : WO; bitpos: [7]; default: 0;
         *  Set this bit to clear cp_out_total_eof_int interrupt.
         */
        uint32_t dma_out_total_eof_int_clr: 1;
        /** dma_crc_done_int_clr : WO; bitpos: [8]; default: 0;
         *  Set this bit to clear cp_crc_done_int interrupt.
         */
        uint32_t dma_crc_done_int_clr: 1;
    };
    uint32_t val;
} cp_dma_int_clr_reg_t;


/** Configuration Registers */
/** Type of dma_out_link register
 *  Link descriptor address and control
 */
typedef union {
    struct {
        /** dma_outlink_addr : R/W; bitpos: [19:0]; default: 0;
         *  This register is used to specify the least significant 20 bits of the first outlink
         *  descriptor's address.
         */
        uint32_t dma_outlink_addr: 20;
        uint32_t reserved_20: 8;
        /** dma_outlink_stop : R/W; bitpos: [28]; default: 0;
         *  Set this bit to stop dealing with the outlink descriptor.
         */
        uint32_t dma_outlink_stop: 1;
        /** dma_outlink_start : R/W; bitpos: [29]; default: 0;
         *  Set this bit to start a new outlink descriptor.
         */
        uint32_t dma_outlink_start: 1;
        /** dma_outlink_restart : R/W; bitpos: [30]; default: 0;
         *  Set this bit to restart the outlink descriptpr from the last address.
         */
        uint32_t dma_outlink_restart: 1;
        /** dma_outlink_park : RO; bitpos: [31]; default: 0;
         *  1: the outlink descriptor's FSM is in idle state.
         *  0: the outlink descriptor's FSM is working.
         */
        uint32_t dma_outlink_park: 1;
    };
    uint32_t val;
} cp_dma_out_link_reg_t;

/** Type of dma_in_link register
 *  Link descriptor address and control
 */
typedef union {
    struct {
        /** dma_inlink_addr : R/W; bitpos: [19:0]; default: 0;
         *  This register is used to specify the least significant 20 bits of the first inlink
         *  descriptor's address.
         */
        uint32_t dma_inlink_addr: 20;
        uint32_t reserved_20: 8;
        /** dma_inlink_stop : R/W; bitpos: [28]; default: 0;
         *  Set this bit to stop dealing with the inlink descriptors.
         */
        uint32_t dma_inlink_stop: 1;
        /** dma_inlink_start : R/W; bitpos: [29]; default: 0;
         *  Set this bit to start dealing with the inlink descriptors.
         */
        uint32_t dma_inlink_start: 1;
        /** dma_inlink_restart : R/W; bitpos: [30]; default: 0;
         *  Set this bit to restart new inlink descriptors.
         */
        uint32_t dma_inlink_restart: 1;
        /** dma_inlink_park : RO; bitpos: [31]; default: 0;
         *  1: the inlink descriptor's FSM is in idle state.
         *  0: the inlink descriptor's FSM is working.
         */
        uint32_t dma_inlink_park: 1;
    };
    uint32_t val;
} cp_dma_in_link_reg_t;

/** Type of dma_conf register
 *  Copy DMA configuration register
 */
typedef union {
    struct {
        /** dma_in_rst : R/W; bitpos: [0]; default: 0;
         *  Set this bit to reset in_inf state machine.
         */
        uint32_t dma_in_rst: 1;
        /** dma_out_rst : R/W; bitpos: [1]; default: 0;
         *  Set this bit to reset out_inf state machine.
         */
        uint32_t dma_out_rst: 1;
        /** dma_cmdfifo_rst : R/W; bitpos: [2]; default: 0;
         *  Set this bit to reset in_cmd fifo and out_cmd fifo.
         */
        uint32_t dma_cmdfifo_rst: 1;
        /** dma_fifo_rst : R/W; bitpos: [3]; default: 0;
         *  Set this bit to reset data in receive FIFO.
         */
        uint32_t dma_fifo_rst: 1;
        /** dma_out_owner : R/W; bitpos: [4]; default: 0;
         *  This is used to configure the owner bit in OUT descriptor. This is effective only
         *  when you set reg_out_auto_wrback.
         */
        uint32_t dma_out_owner: 1;
        /** dma_in_owner : R/W; bitpos: [5]; default: 0;
         *  This is used to configure the owner bit in IN descriptor.
         */
        uint32_t dma_in_owner: 1;
        /** dma_out_auto_wrback : R/W; bitpos: [6]; default: 0;
         *  This bit is used to write back out descriptor when hardware has already used this
         *  descriptor.
         */
        uint32_t dma_out_auto_wrback: 1;
        /** dma_check_owner : R/W; bitpos: [7]; default: 0;
         *  Set this bit to enable owner bit check in descriptor.
         */
        uint32_t dma_check_owner: 1;
        /** dma_crc_cal_reset : R/W; bitpos: [8]; default: 0;
         *  Set this bit to reset crc calculation.
         */
        uint32_t dma_crc_cal_reset: 1;
        /** dma_crc_cal_en : R/W; bitpos: [9]; default: 0;
         *  Set this bit enable crc calculation function.
         */
        uint32_t dma_crc_cal_en: 1;
        /** dma_crc_big_endian_en : R/W; bitpos: [10]; default: 0;
         *  Set this bit to reorder the bit of data which will be send to excute crc.
         */
        uint32_t dma_crc_big_endian_en: 1;
        /** dma_crc_out_reverse_en : R/W; bitpos: [11]; default: 0;
         *  Set this bit to reverse the crc calculation result.
         */
        uint32_t dma_crc_out_reverse_en: 1;
        uint32_t reserved_12: 19;
        /** dma_clk_en : R/W; bitpos: [31]; default: 0;
         *  1'b1: Force clock on for register. 1'b0: Support clock only when application writes
         *  registers.
         */
        uint32_t dma_clk_en: 1;
    };
    uint32_t val;
} cp_dma_conf_reg_t;


/** Status Registers */
/** Type of dma_out_eof_des_addr register
 *  Outlink descriptor address when EOF occurs
 */
typedef union {
    struct {
        /** dma_out_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the outlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t dma_out_eof_des_addr: 32;
    };
    uint32_t val;
} cp_dma_out_eof_des_addr_reg_t;

/** Type of dma_in_eof_des_addr register
 *  Inlink descriptor address when EOF occurs
 */
typedef union {
    struct {
        /** dma_in_suc_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when received successful
         *  EOF.
         */
        uint32_t dma_in_suc_eof_des_addr: 32;
    };
    uint32_t val;
} cp_dma_in_eof_des_addr_reg_t;

/** Type of dma_out_eof_bfr_des_addr register
 *  Outlink descriptor address before the last outlink descriptor
 */
typedef union {
    struct {
        /** dma_out_eof_bfr_des_addr : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the outlink descriptor before the last outlink
         *  descriptor.
         */
        uint32_t dma_out_eof_bfr_des_addr: 32;
    };
    uint32_t val;
} cp_dma_out_eof_bfr_des_addr_reg_t;

/** Type of dma_inlink_dscr register
 *  Address of current inlink descriptor
 */
typedef union {
    struct {
        /** dma_inlink_dscr : RO; bitpos: [31:0]; default: 0;
         *  The address of the current inlink descriptor x.
         */
        uint32_t dma_inlink_dscr: 32;
    };
    uint32_t val;
} cp_dma_inlink_dscr_reg_t;

/** Type of dma_inlink_dscr_bf0 register
 *  Address of last inlink descriptor
 */
typedef union {
    struct {
        /** dma_inlink_dscr_bf0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last inlink descriptor x-1.
         */
        uint32_t dma_inlink_dscr_bf0: 32;
    };
    uint32_t val;
} cp_dma_inlink_dscr_bf0_reg_t;

/** Type of dma_inlink_dscr_bf1 register
 *  Address of the second-to-last inlink descriptor
 */
typedef union {
    struct {
        /** dma_inlink_dscr_bf1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor x-2.
         */
        uint32_t dma_inlink_dscr_bf1: 32;
    };
    uint32_t val;
} cp_dma_inlink_dscr_bf1_reg_t;

/** Type of dma_outlink_dscr register
 *  Address of current outlink descriptor
 */
typedef union {
    struct {
        /** dma_outlink_dscr : RO; bitpos: [31:0]; default: 0;
         *  The address of the current outlink descriptor y.
         */
        uint32_t dma_outlink_dscr: 32;
    };
    uint32_t val;
} cp_dma_outlink_dscr_reg_t;

/** Type of dma_outlink_dscr_bf0 register
 *  Address of last outlink descriptor
 */
typedef union {
    struct {
        /** dma_outlink_dscr_bf0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last outlink descriptor y-1.
         */
        uint32_t dma_outlink_dscr_bf0: 32;
    };
    uint32_t val;
} cp_dma_outlink_dscr_bf0_reg_t;

/** Type of dma_outlink_dscr_bf1 register
 *  Address of the second-to-last outlink descriptor
 */
typedef union {
    struct {
        /** dma_outlink_dscr_bf1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last outlink descriptor y-2.
         */
        uint32_t dma_outlink_dscr_bf1: 32;
    };
    uint32_t val;
} cp_dma_outlink_dscr_bf1_reg_t;

/** Type of dma_in_st register
 *  Status register of receiving data
 */
typedef union {
    struct {
        /** dma_inlink_dscr_addr : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current inlink descriptor's address.
         */
        uint32_t dma_inlink_dscr_addr: 18;
        /** dma_in_dscr_state : RO; bitpos: [19:18]; default: 0;
         *  Reserved.
         */
        uint32_t dma_in_dscr_state: 2;
        /** dma_in_state : RO; bitpos: [22:20]; default: 0;
         *  Reserved.
         */
        uint32_t dma_in_state: 3;
        /** dma_fifo_empty : RO; bitpos: [23]; default: 0;
         *  Copy DMA FIFO empty signal.
         */
        uint32_t dma_fifo_empty: 1;
    };
    uint32_t val;
} cp_dma_in_st_reg_t;

/** Type of dma_out_st register
 *  Status register of transmitting data
 */
typedef union {
    struct {
        /** dma_outlink_dscr_addr : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current outlink descriptor's address.
         */
        uint32_t dma_outlink_dscr_addr: 18;
        /** dma_out_dscr_state : RO; bitpos: [19:18]; default: 0;
         *  Reserved.
         */
        uint32_t dma_out_dscr_state: 2;
        /** dma_out_state : RO; bitpos: [22:20]; default: 0;
         *  Reserved.
         */
        uint32_t dma_out_state: 3;
        /** dma_fifo_full : RO; bitpos: [23]; default: 0;
         *  Copy DMA FIFO full signal.
         */
        uint32_t dma_fifo_full: 1;
    };
    uint32_t val;
} cp_dma_out_st_reg_t;

/** Type of dma_crc_out register
 *  CRC result register
 */
typedef union {
    struct {
        /** dma_crc_result : RO; bitpos: [31:0]; default: 0;
         *  This register stores the result of CRC.
         */
        uint32_t dma_crc_result: 32;
    };
    uint32_t val;
} cp_dma_crc_out_reg_t;

/** Type of dma_date register
 *  Copy DMA version register
 */
typedef union {
    struct {
        /** dma_dma_date : R/W; bitpos: [31:0]; default: 403185664;
         *  This is the version register.
         */
        uint32_t dma_dma_date: 32;
    };
    uint32_t val;
} cp_dma_date_reg_t;


typedef struct {
    volatile cp_dma_int_raw_reg_t dma_int_raw;
    volatile cp_dma_int_st_reg_t dma_int_st;
    volatile cp_dma_int_ena_reg_t dma_int_ena;
    volatile cp_dma_int_clr_reg_t dma_int_clr;
    volatile cp_dma_out_link_reg_t dma_out_link;
    volatile cp_dma_in_link_reg_t dma_in_link;
    volatile cp_dma_out_eof_des_addr_reg_t dma_out_eof_des_addr;
    volatile cp_dma_in_eof_des_addr_reg_t dma_in_eof_des_addr;
    volatile cp_dma_out_eof_bfr_des_addr_reg_t dma_out_eof_bfr_des_addr;
    volatile cp_dma_inlink_dscr_reg_t dma_inlink_dscr;
    volatile cp_dma_inlink_dscr_bf0_reg_t dma_inlink_dscr_bf0;
    volatile cp_dma_inlink_dscr_bf1_reg_t dma_inlink_dscr_bf1;
    volatile cp_dma_outlink_dscr_reg_t dma_outlink_dscr;
    volatile cp_dma_outlink_dscr_bf0_reg_t dma_outlink_dscr_bf0;
    volatile cp_dma_outlink_dscr_bf1_reg_t dma_outlink_dscr_bf1;
    volatile cp_dma_conf_reg_t dma_conf;
    volatile cp_dma_in_st_reg_t dma_in_st;
    volatile cp_dma_out_st_reg_t dma_out_st;
    volatile cp_dma_crc_out_reg_t dma_crc_out;
    uint32_t reserved_04c;
    uint32_t reserved_050;
    uint32_t reserved_054;
    uint32_t reserved_058;
    uint32_t reserved_05c;
    uint32_t reserved_060;
    uint32_t reserved_064;
    uint32_t reserved_068;
    uint32_t reserved_06c;
    uint32_t reserved_070;
    uint32_t reserved_074;
    uint32_t reserved_078;
    uint32_t reserved_07c;
    uint32_t reserved_080;
    uint32_t reserved_084;
    uint32_t reserved_088;
    uint32_t reserved_08c;
    uint32_t reserved_090;
    uint32_t reserved_094;
    uint32_t reserved_098;
    uint32_t reserved_09c;
    uint32_t reserved_0a0;
    uint32_t reserved_0a4;
    uint32_t reserved_0a8;
    uint32_t reserved_0ac;
    uint32_t reserved_0b0;
    uint32_t reserved_0b4;
    uint32_t reserved_0b8;
    uint32_t reserved_0bc;
    uint32_t reserved_0c0;
    uint32_t reserved_0c4;
    uint32_t reserved_0c8;
    uint32_t reserved_0cc;
    uint32_t reserved_0d0;
    uint32_t reserved_0d4;
    uint32_t reserved_0d8;
    uint32_t reserved_0dc;
    uint32_t reserved_0e0;
    uint32_t reserved_0e4;
    uint32_t reserved_0e8;
    uint32_t reserved_0ec;
    uint32_t reserved_0f0;
    uint32_t reserved_0f4;
    uint32_t reserved_0f8;
    volatile cp_dma_date_reg_t dma_date;
} cp_dma_dev_t;

_Static_assert(sizeof(cp_dma_dev_t) == 0x100, "cp_dma_dev_t should occupy 0x100 bytes in memory");

extern cp_dma_dev_t CP_DMA;

#ifdef __cplusplus
}
#endif
