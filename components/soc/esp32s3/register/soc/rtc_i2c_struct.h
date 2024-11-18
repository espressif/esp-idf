/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configure Registers */
/** Type of i2c_scl_low register
 *  configure low scl period
 */
typedef union {
    struct {
        /** i2c_scl_low_period_reg : R/W; bitpos: [19:0]; default: 256;
         *  time period that scl =0
         */
        uint32_t i2c_scl_low_period_reg:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} rtc_i2c_scl_low_reg_t;

/** Type of i2c_ctrl register
 *  configure i2c ctrl
 */
typedef union {
    struct {
        /** i2c_sda_force_out : R/W; bitpos: [0]; default: 0;
         *  1=push pull,0=open drain
         */
        uint32_t i2c_sda_force_out:1;
        /** i2c_scl_force_out : R/W; bitpos: [1]; default: 0;
         *  1=push pull,0=open drain
         */
        uint32_t i2c_scl_force_out:1;
        /** i2c_ms_mode : R/W; bitpos: [2]; default: 0;
         *  1=master,0=slave
         */
        uint32_t i2c_ms_mode:1;
        /** i2c_trans_start : R/W; bitpos: [3]; default: 0;
         *  force start
         */
        uint32_t i2c_trans_start:1;
        /** i2c_tx_lsb_first : R/W; bitpos: [4]; default: 0;
         *  transit lsb first
         */
        uint32_t i2c_tx_lsb_first:1;
        /** i2c_rx_lsb_first : R/W; bitpos: [5]; default: 0;
         *  receive lsb first
         */
        uint32_t i2c_rx_lsb_first:1;
        uint32_t reserved_6:23;
        /** i2c_i2c_ctrl_clk_gate_en : R/W; bitpos: [29]; default: 0;
         *  configure i2c ctrl clk enable
         */
        uint32_t i2c_i2c_ctrl_clk_gate_en:1;
        /** i2c_i2c_reset : R/W; bitpos: [30]; default: 0;
         *  rtc i2c sw reset
         */
        uint32_t i2c_i2c_reset:1;
        /** i2c_i2cclk_en : R/W; bitpos: [31]; default: 0;
         *  rtc i2c reg clk gating
         */
        uint32_t i2c_i2cclk_en:1;
    };
    uint32_t val;
} rtc_i2c_ctrl_reg_t;

/** Type of i2c_to register
 *  configure time out
 */
typedef union {
    struct {
        /** i2c_time_out_reg : R/W; bitpos: [19:0]; default: 65536;
         *  time out threshold
         */
        uint32_t i2c_time_out_reg:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} rtc_i2c_to_reg_t;

/** Type of i2c_slave_addr register
 *  configure slave id
 */
typedef union {
    struct {
        /** i2c_slave_addr : R/W; bitpos: [14:0]; default: 0;
         *  slave address
         */
        uint32_t i2c_slave_addr:15;
        uint32_t reserved_15:16;
        /** i2c_addr_10bit_en : R/W; bitpos: [31]; default: 0;
         *  i2c 10bit mode enable
         */
        uint32_t i2c_addr_10bit_en:1;
    };
    uint32_t val;
} rtc_i2c_slave_addr_reg_t;

/** Type of i2c_scl_high register
 *  configure high scl period
 */
typedef union {
    struct {
        /** i2c_scl_high_period_reg : R/W; bitpos: [19:0]; default: 256;
         *  time period that scl = 1
         */
        uint32_t i2c_scl_high_period_reg:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} rtc_i2c_scl_high_reg_t;

/** Type of i2c_sda_duty register
 *  configure sda duty
 */
typedef union {
    struct {
        /** i2c_sda_duty_num : R/W; bitpos: [19:0]; default: 16;
         *  time period for SDA to toggle after SCL goes low
         */
        uint32_t i2c_sda_duty_num:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} rtc_i2c_sda_duty_reg_t;

/** Type of i2c_scl_start_period register
 *  configure scl start period
 */
typedef union {
    struct {
        /** i2c_scl_start_period : R/W; bitpos: [19:0]; default: 8;
         *  time period for SCL to toggle after I2C start is triggered
         */
        uint32_t i2c_scl_start_period:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} rtc_i2c_scl_start_period_reg_t;

/** Type of i2c_scl_stop_period register
 *  configure scl stop period
 */
typedef union {
    struct {
        /** i2c_scl_stop_period : R/W; bitpos: [19:0]; default: 8;
         *  time period for SCL to stop after I2C end is triggered
         */
        uint32_t i2c_scl_stop_period:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} rtc_i2c_scl_stop_period_reg_t;

/** Type of i2c_data register
 *  get i2c data status
 */
typedef union {
    struct {
        /** i2c_i2c_rdata : RO; bitpos: [7:0]; default: 0;
         *  data received
         */
        uint32_t i2c_i2c_rdata:8;
        /** i2c_slave_tx_data : R/W; bitpos: [15:8]; default: 0;
         *  data sent by slave
         */
        uint32_t i2c_slave_tx_data:8;
        uint32_t reserved_16:15;
        /** i2c_i2c_done : RO; bitpos: [31]; default: 0;
         *  i2c done
         */
        uint32_t i2c_i2c_done:1;
    };
    uint32_t val;
} rtc_i2c_data_reg_t;

/** Type of i2c_cmd register
 *  i2c command register
 */
typedef union {
    struct {
        /** i2c_command : R/W; bitpos: [13:0]; default: 2307;
         *  command
         */
        uint32_t i2c_byte_num:8;
        uint32_t i2c_ack_en:1;
        uint32_t i2c_ack_exp:1;
        uint32_t i2c_ack_val:1;
        uint32_t i2c_op_code:3;
        uint32_t reserved14:17;
        /** i2c_command_done : RO; bitpos: [31]; default: 0;
         *  command0_done
         */
        uint32_t i2c_command_done:1;
    };
    uint32_t val;
} rtc_i2c_cmd_reg_t;

/** Group: status register */
/** Type of i2c_status register
 *  get i2c status
 */
typedef union {
    struct {
        /** i2c_ack_rec : RO; bitpos: [0]; default: 0;
         *  ack response
         */
        uint32_t i2c_ack_rec:1;
        /** i2c_slave_rw : RO; bitpos: [1]; default: 0;
         *  slave read or write
         */
        uint32_t i2c_slave_rw:1;
        /** i2c_arb_lost : RO; bitpos: [2]; default: 0;
         *  arbitration is lost
         */
        uint32_t i2c_arb_lost:1;
        /** i2c_bus_busy : RO; bitpos: [3]; default: 0;
         *  bus is busy
         */
        uint32_t i2c_bus_busy:1;
        /** i2c_slave_addressed : RO; bitpos: [4]; default: 0;
         *  slave reg sub address
         */
        uint32_t i2c_slave_addressed:1;
        /** i2c_byte_trans : RO; bitpos: [5]; default: 0;
         *  One byte transit done
         */
        uint32_t i2c_byte_trans:1;
        /** i2c_op_cnt : RO; bitpos: [7:6]; default: 0;
         *  which operation is working
         */
        uint32_t i2c_op_cnt:2;
        uint32_t reserved_8:8;
        /** i2c_shift_reg : RO; bitpos: [23:16]; default: 0;
         *  shifter content
         */
        uint32_t i2c_shift_reg:8;
        /** i2c_scl_main_state_last : RO; bitpos: [26:24]; default: 0;
         *  i2c last main status
         */
        uint32_t i2c_scl_main_state_last:3;
        uint32_t reserved_27:1;
        /** i2c_scl_state_last : RO; bitpos: [30:28]; default: 0;
         *  scl last status
         */
        uint32_t i2c_scl_state_last:3;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} rtc_i2c_status_reg_t;


/** Group: interrupt Register */
/** Type of i2c_int_clr register
 *  interrupt clear register
 */
typedef union {
    struct {
        /** i2c_slave_tran_comp_int_clr : WO; bitpos: [0]; default: 0;
         *  clear slave transit complete interrupt
         */
        uint32_t i2c_slave_tran_comp_int_clr:1;
        /** i2c_arbitration_lost_int_clr : WO; bitpos: [1]; default: 0;
         *  clear arbitration lost interrupt
         */
        uint32_t i2c_arbitration_lost_int_clr:1;
        /** i2c_master_tran_comp_int_clr : WO; bitpos: [2]; default: 0;
         *  clear master transit complete interrupt
         */
        uint32_t i2c_master_tran_comp_int_clr:1;
        /** i2c_trans_complete_int_clr : WO; bitpos: [3]; default: 0;
         *  clear transit complete interrupt
         */
        uint32_t i2c_trans_complete_int_clr:1;
        /** i2c_time_out_int_clr : WO; bitpos: [4]; default: 0;
         *  clear time out interrupt
         */
        uint32_t i2c_time_out_int_clr:1;
        /** i2c_ack_err_int_clr : WO; bitpos: [5]; default: 0;
         *  clear ack error interrupt
         */
        uint32_t i2c_ack_err_int_clr:1;
        /** i2c_rx_data_int_clr : WO; bitpos: [6]; default: 0;
         *  clear receive data interrupt
         */
        uint32_t i2c_rx_data_int_clr:1;
        /** i2c_tx_data_int_clr : WO; bitpos: [7]; default: 0;
         *  clear transit load data complete interrupt
         */
        uint32_t i2c_tx_data_int_clr:1;
        /** i2c_detect_start_int_clr : WO; bitpos: [8]; default: 0;
         *  clear detect start interrupt
         */
        uint32_t i2c_detect_start_int_clr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} rtc_i2c_int_clr_reg_t;

/** Type of i2c_int_raw register
 *  interrupt raw register
 */
typedef union {
    struct {
        /** i2c_slave_tran_comp_int_raw : RO; bitpos: [0]; default: 0;
         *  slave transit complete interrupt raw
         */
        uint32_t i2c_slave_tran_comp_int_raw:1;
        /** i2c_arbitration_lost_int_raw : RO; bitpos: [1]; default: 0;
         *  arbitration lost interrupt raw
         */
        uint32_t i2c_arbitration_lost_int_raw:1;
        /** i2c_master_tran_comp_int_raw : RO; bitpos: [2]; default: 0;
         *  master transit complete interrupt raw
         */
        uint32_t i2c_master_tran_comp_int_raw:1;
        /** i2c_trans_complete_int_raw : RO; bitpos: [3]; default: 0;
         *  transit complete interrupt raw
         */
        uint32_t i2c_trans_complete_int_raw:1;
        /** i2c_time_out_int_raw : RO; bitpos: [4]; default: 0;
         *  time out interrupt raw
         */
        uint32_t i2c_time_out_int_raw:1;
        /** i2c_ack_err_int_raw : RO; bitpos: [5]; default: 0;
         *  ack error interrupt raw
         */
        uint32_t i2c_ack_err_int_raw:1;
        /** i2c_rx_data_int_raw : RO; bitpos: [6]; default: 0;
         *  receive data interrupt raw
         */
        uint32_t i2c_rx_data_int_raw:1;
        /** i2c_tx_data_int_raw : RO; bitpos: [7]; default: 0;
         *  transit data interrupt raw
         */
        uint32_t i2c_tx_data_int_raw:1;
        /** i2c_detect_start_int_raw : RO; bitpos: [8]; default: 0;
         *  detect start interrupt raw
         */
        uint32_t i2c_detect_start_int_raw:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} rtc_i2c_int_raw_reg_t;

/** Type of i2c_int_st register
 *  interrupt state register
 */
typedef union {
    struct {
        /** i2c_slave_tran_comp_int_st : RO; bitpos: [0]; default: 0;
         *  slave transit complete interrupt state
         */
        uint32_t i2c_slave_tran_comp_int_st:1;
        /** i2c_arbitration_lost_int_st : RO; bitpos: [1]; default: 0;
         *  arbitration lost interrupt state
         */
        uint32_t i2c_arbitration_lost_int_st:1;
        /** i2c_master_tran_comp_int_st : RO; bitpos: [2]; default: 0;
         *  master transit complete interrupt state
         */
        uint32_t i2c_master_tran_comp_int_st:1;
        /** i2c_trans_complete_int_st : RO; bitpos: [3]; default: 0;
         *  transit complete interrupt state
         */
        uint32_t i2c_trans_complete_int_st:1;
        /** i2c_time_out_int_st : RO; bitpos: [4]; default: 0;
         *  time out interrupt state
         */
        uint32_t i2c_time_out_int_st:1;
        /** i2c_ack_err_int_st : RO; bitpos: [5]; default: 0;
         *  ack error interrupt state
         */
        uint32_t i2c_ack_err_int_st:1;
        /** i2c_rx_data_int_st : RO; bitpos: [6]; default: 0;
         *  receive data interrupt state
         */
        uint32_t i2c_rx_data_int_st:1;
        /** i2c_tx_data_int_st : RO; bitpos: [7]; default: 0;
         *  transit data interrupt state
         */
        uint32_t i2c_tx_data_int_st:1;
        /** i2c_detect_start_int_st : RO; bitpos: [8]; default: 0;
         *  detect start interrupt state
         */
        uint32_t i2c_detect_start_int_st:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} rtc_i2c_int_st_reg_t;

/** Type of i2c_int_ena register
 *  interrupt enable register
 */
typedef union {
    struct {
        /** i2c_slave_tran_comp_int_ena : R/W; bitpos: [0]; default: 0;
         *  enable slave transit complete interrupt
         */
        uint32_t i2c_slave_tran_comp_int_ena:1;
        /** i2c_arbitration_lost_int_ena : R/W; bitpos: [1]; default: 0;
         *  enable arbitration lost interrupt
         */
        uint32_t i2c_arbitration_lost_int_ena:1;
        /** i2c_master_tran_comp_int_ena : R/W; bitpos: [2]; default: 0;
         *  enable master transit complete interrupt
         */
        uint32_t i2c_master_tran_comp_int_ena:1;
        /** i2c_trans_complete_int_ena : R/W; bitpos: [3]; default: 0;
         *  enable transit complete interrupt
         */
        uint32_t i2c_trans_complete_int_ena:1;
        /** i2c_time_out_int_ena : R/W; bitpos: [4]; default: 0;
         *  enable time out interrupt
         */
        uint32_t i2c_time_out_int_ena:1;
        /** i2c_ack_err_int_ena : R/W; bitpos: [5]; default: 0;
         *  enable eack error interrupt
         */
        uint32_t i2c_ack_err_int_ena:1;
        /** i2c_rx_data_int_ena : R/W; bitpos: [6]; default: 0;
         *  enable receive data interrupt
         */
        uint32_t i2c_rx_data_int_ena:1;
        /** i2c_tx_data_int_ena : R/W; bitpos: [7]; default: 0;
         *  enable transit data interrupt
         */
        uint32_t i2c_tx_data_int_ena:1;
        /** i2c_detect_start_int_ena : R/W; bitpos: [8]; default: 0;
         *  enable detect start interrupt
         */
        uint32_t i2c_detect_start_int_ena:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} rtc_i2c_int_ena_reg_t;


/** Group: version Registers */
/** Type of i2c_date register
 *  version register
 */
typedef union {
    struct {
        /** i2c_i2c_date : R/W; bitpos: [27:0]; default: 26235664;
         *  version
         */
        uint32_t i2c_i2c_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} rtc_i2c_date_reg_t;


typedef struct {
    volatile rtc_i2c_scl_low_reg_t i2c_scl_low;
    volatile rtc_i2c_ctrl_reg_t i2c_ctrl;
    volatile rtc_i2c_status_reg_t i2c_status;
    volatile rtc_i2c_to_reg_t i2c_to;
    volatile rtc_i2c_slave_addr_reg_t i2c_slave_addr;
    volatile rtc_i2c_scl_high_reg_t i2c_scl_high;
    volatile rtc_i2c_sda_duty_reg_t i2c_sda_duty;
    volatile rtc_i2c_scl_start_period_reg_t i2c_scl_start_period;
    volatile rtc_i2c_scl_stop_period_reg_t i2c_scl_stop_period;
    volatile rtc_i2c_int_clr_reg_t i2c_int_clr;
    volatile rtc_i2c_int_raw_reg_t i2c_int_raw;
    volatile rtc_i2c_int_st_reg_t i2c_int_st;
    volatile rtc_i2c_int_ena_reg_t i2c_int_ena;
    volatile rtc_i2c_data_reg_t i2c_data;
    volatile rtc_i2c_cmd_reg_t i2c_cmd[16];
    uint32_t reserved_078[33];
    volatile rtc_i2c_date_reg_t i2c_date;
} rtc_i2c_dev_t;
extern rtc_i2c_dev_t RTC_I2C;


#ifndef __cplusplus
_Static_assert(sizeof(rtc_i2c_dev_t) == 0x100, "Invalid size of rtc_i2c_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
