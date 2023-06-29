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

/** Group: Timing registers */
/** Type of scl_low_period register
 *  Configures the low level width of the SCL
 *  Clock
 */
typedef union {
    struct {
        /** scl_low_period : R/W; bitpos: [8:0]; default: 0;
         *  Configures the low level width of the SCL Clock.
         *  Measurement unit: i2c_sclk.
         */
        uint32_t scl_low_period:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_i2c_scl_low_period_reg_t;

/** Type of sda_hold register
 *  Configures the hold time after a negative SCL edge.
 */
typedef union {
    struct {
        /** sda_hold_time : R/W; bitpos: [8:0]; default: 0;
         *  Configures the time to hold the data after the falling edge of SCL.
         *  Measurement unit: i2c_sclk
         */
        uint32_t sda_hold_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_i2c_sda_hold_reg_t;

/** Type of sda_sample register
 *  Configures the sample time after a positive SCL edge.
 */
typedef union {
    struct {
        /** sda_sample_time : R/W; bitpos: [8:0]; default: 0;
         *  Configures the sample time after a positive SCL edge.
         *  Measurement unit: i2c_sclk
         */
        uint32_t sda_sample_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_i2c_sda_sample_reg_t;

/** Type of scl_high_period register
 *  Configures the high level width of SCL
 */
typedef union {
    struct {
        /** scl_high_period : R/W; bitpos: [8:0]; default: 0;
         *  Configures for how long SCL remains high in master mode.
         *  Measurement unit: i2c_sclk
         */
        uint32_t scl_high_period:9;
        /** scl_wait_high_period : R/W; bitpos: [15:9]; default: 0;
         *  Configures the SCL_FSM's waiting period for SCL high level in master mode.
         *  Measurement unit: i2c_sclk
         */
        uint32_t scl_wait_high_period:7;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_i2c_scl_high_period_reg_t;

/** Type of scl_start_hold register
 *  Configures the delay between the SDA and SCL negative edge for a start condition
 */
typedef union {
    struct {
        /** scl_start_hold_time : R/W; bitpos: [8:0]; default: 8;
         *  Configures the time between the falling edge of SDA and the falling edge of SCL for
         *  a START condition.
         *  Measurement unit: i2c_sclk.
         */
        uint32_t scl_start_hold_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_i2c_scl_start_hold_reg_t;

/** Type of scl_rstart_setup register
 *  Configures the delay between the positive
 *  edge of SCL and the negative edge of SDA
 */
typedef union {
    struct {
        /** scl_rstart_setup_time : R/W; bitpos: [8:0]; default: 8;
         *  Configures the time between the positive edge of SCL and the negative edge of SDA
         *  for a RESTART condition.
         *  Measurement unit: i2c_sclk
         */
        uint32_t scl_rstart_setup_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_i2c_scl_rstart_setup_reg_t;

/** Type of scl_stop_hold register
 *  Configures the delay after the SCL clock
 *  edge for a stop condition
 */
typedef union {
    struct {
        /** scl_stop_hold_time : R/W; bitpos: [8:0]; default: 8;
         *  Configures the delay after the STOP condition.
         *  Measurement unit: i2c_sclk
         */
        uint32_t scl_stop_hold_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_i2c_scl_stop_hold_reg_t;

/** Type of scl_stop_setup register
 *  Configures the delay between the SDA and
 *  SCL positive edge for a stop condition
 */
typedef union {
    struct {
        /** scl_stop_setup_time : R/W; bitpos: [8:0]; default: 8;
         *  Configures the time between the rising edge of SCL and the rising edge of SDA.
         *  Measurement unit: i2c_sclk
         */
        uint32_t scl_stop_setup_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_i2c_scl_stop_setup_reg_t;

/** Type of scl_st_time_out register
 *  SCL status time out register
 */
typedef union {
    struct {
        /** scl_st_to_i2c : R/W; bitpos: [4:0]; default: 16;
         *  Configures the threshold value of SCL_FSM state unchanged period. It should be no
         *  more than 23.
         *  Measurement unit: i2c_sclk
         */
        uint32_t scl_st_to_i2c:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} lp_i2c_scl_st_time_out_reg_t;

/** Type of scl_main_st_time_out register
 *  SCL main status time out register
 */
typedef union {
    struct {
        /** scl_main_st_to_i2c : R/W; bitpos: [4:0]; default: 16;
         *  Configures the threshold value of SCL_MAIN_FSM state unchanged period.nIt should be
         *  no more than 23.
         *  Measurement unit: i2c_sclk
         */
        uint32_t scl_main_st_to_i2c:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} lp_i2c_scl_main_st_time_out_reg_t;


/** Group: Configuration registers */
/** Type of ctr register
 *  Transmission setting
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** sample_scl_level : R/W; bitpos: [2]; default: 0;
         *  Configures the sample mode for SDA.
         *  1: Sample SDA data on the SCL low level.
         *
         *  0: Sample SDA data on the SCL high level.
         */
        uint32_t sample_scl_level:1;
        /** rx_full_ack_level : R/W; bitpos: [3]; default: 1;
         *  Configures the ACK value that needs to be sent by master when the rx_fifo_cnt has
         *  reached the threshold.
         */
        uint32_t rx_full_ack_level:1;
        uint32_t reserved_4:1;
        /** trans_start : WT; bitpos: [5]; default: 0;
         *  Configures to start sending the data in txfifo for slave.
         *  0: No effect
         *
         *  1: Start
         */
        uint32_t trans_start:1;
        /** tx_lsb_first : R/W; bitpos: [6]; default: 0;
         *  Configures to control the sending order for data needing to be sent.
         *  1: send data from the least significant bit,
         *
         *  0: send data from the most significant bit.
         */
        uint32_t tx_lsb_first:1;
        /** rx_lsb_first : R/W; bitpos: [7]; default: 0;
         *  Configures to control the storage order for received data.
         *  1: receive data from the least significant bit
         *
         *  0: receive data from the most significant bit.
         */
        uint32_t rx_lsb_first:1;
        /** clk_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether to gate clock signal for registers.
         *
         *  0: Force clock on for registers
         *
         *  1: Support clock only when registers are read or written to by software.
         */
        uint32_t clk_en:1;
        /** arbitration_en : R/W; bitpos: [9]; default: 1;
         *  Configures to enable I2C bus arbitration detection.
         *  0: No effect
         *
         *  1: Enable
         */
        uint32_t arbitration_en:1;
        /** fsm_rst : WT; bitpos: [10]; default: 0;
         *  Configures to reset the SCL_FSM.
         *  0: No effect
         *
         *  1: Reset
         */
        uint32_t fsm_rst:1;
        /** conf_upgate : WT; bitpos: [11]; default: 0;
         *  Configures this bit for synchronization
         *  0: No effect
         *
         *  1: Synchronize
         */
        uint32_t conf_upgate:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_i2c_ctr_reg_t;

/** Type of to register
 *  Setting time out control for receiving data.
 */
typedef union {
    struct {
        /** time_out_value : R/W; bitpos: [4:0]; default: 16;
         *  Configures the timeout threshold period for SCL stucking at high or low level. The
         *  actual period is 2^(reg_time_out_value).
         *  Measurement unit: i2c_sclk.
         */
        uint32_t time_out_value:5;
        /** time_out_en : R/W; bitpos: [5]; default: 0;
         *  Configures to enable time out control.
         *  0: No effect
         *
         *  1: Enable
         */
        uint32_t time_out_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_i2c_to_reg_t;

/** Type of fifo_conf register
 *  FIFO configuration register.
 */
typedef union {
    struct {
        /** rxfifo_wm_thrhd : R/W; bitpos: [3:0]; default: 6;
         *  Configures the water mark threshold of RXFIFO in nonfifo access mode. When
         *  reg_reg_fifo_prt_en is 1 and rx FIFO counter is bigger than
         *  reg_rxfifo_wm_thrhd[3:0], reg_rxfifo_wm_int_raw bit will be valid.
         */
        uint32_t rxfifo_wm_thrhd:4;
        uint32_t reserved_4:1;
        /** txfifo_wm_thrhd : R/W; bitpos: [8:5]; default: 2;
         *  Configures the water mark threshold of TXFIFO in nonfifo access mode. When
         *  reg_reg_fifo_prt_en is 1 and tx FIFO counter is smaller than
         *  reg_txfifo_wm_thrhd[3:0], reg_txfifo_wm_int_raw bit will be valid.
         */
        uint32_t txfifo_wm_thrhd:4;
        uint32_t reserved_9:1;
        /** nonfifo_en : R/W; bitpos: [10]; default: 0;
         *  Configures to enable APB nonfifo access.
         */
        uint32_t nonfifo_en:1;
        uint32_t reserved_11:1;
        /** rx_fifo_rst : R/W; bitpos: [12]; default: 0;
         *  Configures to reset RXFIFO.
         *  0: No effect
         *
         *  1: Reset
         */
        uint32_t rx_fifo_rst:1;
        /** tx_fifo_rst : R/W; bitpos: [13]; default: 0;
         *  Configures to reset TXFIFO.
         *  0: No effect
         *
         *  1: Reset
         */
        uint32_t tx_fifo_rst:1;
        /** fifo_prt_en : R/W; bitpos: [14]; default: 1;
         *  Configures to enable FIFO pointer in non-fifo access mode. This bit controls the
         *  valid bits and the TX/RX FIFO overflow, underflow, full and empty interrupts.
         *  0: No effect
         *
         *  1: Enable
         */
        uint32_t fifo_prt_en:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_i2c_fifo_conf_reg_t;

/** Type of filter_cfg register
 *  SCL and SDA filter configuration register
 */
typedef union {
    struct {
        /** scl_filter_thres : R/W; bitpos: [3:0]; default: 0;
         *  Configures the threshold pulse width to be filtered on SCL. When a pulse on the SCL
         *  input has smaller width than this register value, the I2C controller will ignore
         *  that pulse.
         *  Measurement unit: i2c_sclk
         */
        uint32_t scl_filter_thres:4;
        /** sda_filter_thres : R/W; bitpos: [7:4]; default: 0;
         *  Configures the threshold pulse width to be filtered on SDA. When a pulse on the SDA
         *  input has smaller width than this register value, the I2C controller will ignore
         *  that pulse.
         *  Measurement unit: i2c_sclk
         */
        uint32_t sda_filter_thres:4;
        /** scl_filter_en : R/W; bitpos: [8]; default: 1;
         *  Configures to enable the filter function for SCL.
         */
        uint32_t scl_filter_en:1;
        /** sda_filter_en : R/W; bitpos: [9]; default: 1;
         *  Configures to enable the filter function for SDA.
         */
        uint32_t sda_filter_en:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} lp_i2c_filter_cfg_reg_t;

/** Type of clk_conf register
 *  I2C CLK configuration register
 */
typedef union {
    struct {
        /** sclk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  the integral part of the fractional divisor for i2c module
         */
        uint32_t sclk_div_num:8;
        /** sclk_div_a : R/W; bitpos: [13:8]; default: 0;
         *  the numerator of the fractional part of the fractional divisor for i2c module
         */
        uint32_t sclk_div_a:6;
        /** sclk_div_b : R/W; bitpos: [19:14]; default: 0;
         *  the denominator of the fractional part of the fractional divisor for i2c module
         */
        uint32_t sclk_div_b:6;
        /** sclk_sel : R/W; bitpos: [20]; default: 0;
         *  The clock selection for i2c module:0-XTAL,1-CLK_8MHz.
         */
        uint32_t sclk_sel:1;
        /** sclk_active : R/W; bitpos: [21]; default: 1;
         *  The clock switch for i2c module
         */
        uint32_t sclk_active:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} lp_i2c_clk_conf_reg_t;

/** Type of scl_sp_conf register
 *  Power configuration register
 */
typedef union {
    struct {
        /** scl_rst_slv_en : R/W/SC; bitpos: [0]; default: 0;
         *  When I2C master is IDLE, set this bit to send out SCL pulses. The number of pulses
         *  equals to reg_scl_rst_slv_num[4:0].
         */
        uint32_t scl_rst_slv_en:1;
        /** scl_rst_slv_num : R/W; bitpos: [5:1]; default: 0;
         *  Configures to send out SCL pulses when I2C master is IDLE. The number of pulses
         *  equals to reg_scl_rst_slv_num[4:0].
         */
        uint32_t scl_rst_slv_num:5;
        /** scl_pd_en : R/W; bitpos: [6]; default: 0;
         *  Configure the pulses of SCL generated in I2C master mode.
         *  Valid when reg_scl_rst_slv_en is 1.
         *  Measurement unit: i2c_sclk
         */
        uint32_t scl_pd_en:1;
        /** sda_pd_en : R/W; bitpos: [7]; default: 0;
         *  Configures to power down the I2C output SCL line.
         *  0: Not power down.
         *
         *  1: Power down.
         *  Valid only when reg_scl_force_out is 1.
         */
        uint32_t sda_pd_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_i2c_scl_sp_conf_reg_t;


/** Group: Status registers */
/** Type of sr register
 *  Describe I2C work status.
 */
typedef union {
    struct {
        /** resp_rec : RO; bitpos: [0]; default: 0;
         *  Represents the received ACK value in master mode or slave mode.
         *  0: ACK,
         *
         *  1: NACK.
         */
        uint32_t resp_rec:1;
        uint32_t reserved_1:2;
        /** arb_lost : RO; bitpos: [3]; default: 0;
         *  Represents whether the I2C controller loses control of SCL line.
         *  0: No arbitration lost
         *
         *  1: Arbitration lost
         */
        uint32_t arb_lost:1;
        /** bus_busy : RO; bitpos: [4]; default: 0;
         *  Represents the I2C bus state.
         *  1: The I2C bus is busy transferring data,
         *
         *  0: The I2C bus is in idle state.
         */
        uint32_t bus_busy:1;
        uint32_t reserved_5:3;
        /** rxfifo_cnt : RO; bitpos: [12:8]; default: 0;
         *  Represents the number of data bytes to be sent.
         */
        uint32_t rxfifo_cnt:5;
        uint32_t reserved_13:5;
        /** txfifo_cnt : RO; bitpos: [22:18]; default: 0;
         *  Represents the number of data bytes received in RAM.
         */
        uint32_t txfifo_cnt:5;
        uint32_t reserved_23:1;
        /** scl_main_state_last : RO; bitpos: [26:24]; default: 0;
         *  Represents the states of the I2C module state machine.
         *  0: Idle,
         *
         *  1: Address shift,
         *
         *  2: ACK address,
         *
         *  3: Rx data,
         *
         *  4: Tx data,
         *
         *  5: Send ACK,
         *
         *  6: Wait ACK
         */
        uint32_t scl_main_state_last:3;
        uint32_t reserved_27:1;
        /** scl_state_last : RO; bitpos: [30:28]; default: 0;
         *  Represents the states of the state machine used to produce SCL.
         *  0: Idle,
         *
         *  1: Start,
         *
         *  2: Negative edge,
         *
         *  3: Low,
         *
         *  4: Positive edge,
         *
         *  5: High,
         *
         *  6: Stop
         */
        uint32_t scl_state_last:3;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_i2c_sr_reg_t;

/** Type of fifo_st register
 *  FIFO status register.
 */
typedef union {
    struct {
        /** rxfifo_raddr : RO; bitpos: [3:0]; default: 0;
         *  Represents the offset address of the APB reading from RXFIFO
         */
        uint32_t rxfifo_raddr:4;
        uint32_t reserved_4:1;
        /** rxfifo_waddr : RO; bitpos: [8:5]; default: 0;
         *  Represents the offset address of i2c module receiving data and writing to RXFIFO.
         */
        uint32_t rxfifo_waddr:4;
        uint32_t reserved_9:1;
        /** txfifo_raddr : RO; bitpos: [13:10]; default: 0;
         *  Represents the offset address of i2c module reading from TXFIFO.
         */
        uint32_t txfifo_raddr:4;
        uint32_t reserved_14:1;
        /** txfifo_waddr : RO; bitpos: [18:15]; default: 0;
         *  Represents the offset address of APB bus writing to TXFIFO.
         */
        uint32_t txfifo_waddr:4;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} lp_i2c_fifo_st_reg_t;

/** Type of data register
 *  Rx FIFO read data.
 */
typedef union {
    struct {
        /** fifo_rdata : RO; bitpos: [7:0]; default: 0;
         *  Represents the value of RXFIFO read data.
         */
        uint32_t fifo_rdata:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_i2c_data_reg_t;


/** Group: Interrupt registers */
/** Type of int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** rxfifo_wm_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  The raw interrupt status of I2C_RXFIFO_WM_INT interrupt.
         */
        uint32_t rxfifo_wm_int_raw:1;
        /** txfifo_wm_int_raw : R/SS/WTC; bitpos: [1]; default: 1;
         *  The raw interrupt status of I2C_TXFIFO_WM_INT interrupt.
         */
        uint32_t txfifo_wm_int_raw:1;
        /** rxfifo_ovf_int_raw : R/SS/WTC; bitpos: [2]; default: 0;
         *  The raw interrupt status of I2C_RXFIFO_OVF_INT interrupt.
         */
        uint32_t rxfifo_ovf_int_raw:1;
        /** end_detect_int_raw : R/SS/WTC; bitpos: [3]; default: 0;
         *  The raw interrupt status of the I2C_END_DETECT_INT interrupt.
         */
        uint32_t end_detect_int_raw:1;
        /** byte_trans_done_int_raw : R/SS/WTC; bitpos: [4]; default: 0;
         *  The raw interrupt status of the I2C_END_DETECT_INT interrupt.
         */
        uint32_t byte_trans_done_int_raw:1;
        /** arbitration_lost_int_raw : R/SS/WTC; bitpos: [5]; default: 0;
         *  The raw interrupt status of the I2C_ARBITRATION_LOST_INT interrupt.
         */
        uint32_t arbitration_lost_int_raw:1;
        /** mst_txfifo_udf_int_raw : R/SS/WTC; bitpos: [6]; default: 0;
         *  The raw interrupt status of I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t mst_txfifo_udf_int_raw:1;
        /** trans_complete_int_raw : R/SS/WTC; bitpos: [7]; default: 0;
         *  The raw interrupt status of the I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t trans_complete_int_raw:1;
        /** time_out_int_raw : R/SS/WTC; bitpos: [8]; default: 0;
         *  The raw interrupt status of the I2C_TIME_OUT_INT interrupt.
         */
        uint32_t time_out_int_raw:1;
        /** trans_start_int_raw : R/SS/WTC; bitpos: [9]; default: 0;
         *  The raw interrupt status of the I2C_TRANS_START_INT interrupt.
         */
        uint32_t trans_start_int_raw:1;
        /** nack_int_raw : R/SS/WTC; bitpos: [10]; default: 0;
         *  The raw interrupt status of I2C_SLAVE_STRETCH_INT interrupt.
         */
        uint32_t nack_int_raw:1;
        /** txfifo_ovf_int_raw : R/SS/WTC; bitpos: [11]; default: 0;
         *  The raw interrupt status of I2C_TXFIFO_OVF_INT interrupt.
         */
        uint32_t txfifo_ovf_int_raw:1;
        /** rxfifo_udf_int_raw : R/SS/WTC; bitpos: [12]; default: 0;
         *  The raw interrupt status of I2C_RXFIFO_UDF_INT  interrupt.
         */
        uint32_t rxfifo_udf_int_raw:1;
        /** scl_st_to_int_raw : R/SS/WTC; bitpos: [13]; default: 0;
         *  The raw interrupt status of I2C_SCL_ST_TO_INT interrupt.
         */
        uint32_t scl_st_to_int_raw:1;
        /** scl_main_st_to_int_raw : R/SS/WTC; bitpos: [14]; default: 0;
         *  The raw interrupt status of I2C_SCL_MAIN_ST_TO_INT interrupt.
         */
        uint32_t scl_main_st_to_int_raw:1;
        /** det_start_int_raw : R/SS/WTC; bitpos: [15]; default: 0;
         *  The raw interrupt status of I2C_DET_START_INT interrupt.
         */
        uint32_t det_start_int_raw:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_i2c_int_raw_reg_t;

/** Type of int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** rxfifo_wm_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear I2C_RXFIFO_WM_INT interrupt.
         */
        uint32_t rxfifo_wm_int_clr:1;
        /** txfifo_wm_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear I2C_TXFIFO_WM_INT interrupt.
         */
        uint32_t txfifo_wm_int_clr:1;
        /** rxfifo_ovf_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear I2C_RXFIFO_OVF_INT interrupt.
         */
        uint32_t rxfifo_ovf_int_clr:1;
        /** end_detect_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear the I2C_END_DETECT_INT interrupt.
         */
        uint32_t end_detect_int_clr:1;
        /** byte_trans_done_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear the I2C_END_DETECT_INT interrupt.
         */
        uint32_t byte_trans_done_int_clr:1;
        /** arbitration_lost_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear the I2C_ARBITRATION_LOST_INT interrupt.
         */
        uint32_t arbitration_lost_int_clr:1;
        /** mst_txfifo_udf_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t mst_txfifo_udf_int_clr:1;
        /** trans_complete_int_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear the I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t trans_complete_int_clr:1;
        /** time_out_int_clr : WT; bitpos: [8]; default: 0;
         *  Write 1 to clear the I2C_TIME_OUT_INT interrupt.
         */
        uint32_t time_out_int_clr:1;
        /** trans_start_int_clr : WT; bitpos: [9]; default: 0;
         *  Write 1 to clear the I2C_TRANS_START_INT interrupt.
         */
        uint32_t trans_start_int_clr:1;
        /** nack_int_clr : WT; bitpos: [10]; default: 0;
         *  Write 1 to clear I2C_SLAVE_STRETCH_INT interrupt.
         */
        uint32_t nack_int_clr:1;
        /** txfifo_ovf_int_clr : WT; bitpos: [11]; default: 0;
         *  Write 1 to clear I2C_TXFIFO_OVF_INT interrupt.
         */
        uint32_t txfifo_ovf_int_clr:1;
        /** rxfifo_udf_int_clr : WT; bitpos: [12]; default: 0;
         *  Write 1 to clear I2C_RXFIFO_UDF_INT  interrupt.
         */
        uint32_t rxfifo_udf_int_clr:1;
        /** scl_st_to_int_clr : WT; bitpos: [13]; default: 0;
         *  Write 1 to clear I2C_SCL_ST_TO_INT interrupt.
         */
        uint32_t scl_st_to_int_clr:1;
        /** scl_main_st_to_int_clr : WT; bitpos: [14]; default: 0;
         *  Write 1 to clear I2C_SCL_MAIN_ST_TO_INT interrupt.
         */
        uint32_t scl_main_st_to_int_clr:1;
        /** det_start_int_clr : WT; bitpos: [15]; default: 0;
         *  Write 1 to clear I2C_DET_START_INT interrupt.
         */
        uint32_t det_start_int_clr:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_i2c_int_clr_reg_t;

/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** rxfifo_wm_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to anable I2C_RXFIFO_WM_INT interrupt.
         */
        uint32_t rxfifo_wm_int_ena:1;
        /** txfifo_wm_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to anable I2C_TXFIFO_WM_INT interrupt.
         */
        uint32_t txfifo_wm_int_ena:1;
        /** rxfifo_ovf_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to anable I2C_RXFIFO_OVF_INT interrupt.
         */
        uint32_t rxfifo_ovf_int_ena:1;
        /** end_detect_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to anable the I2C_END_DETECT_INT interrupt.
         */
        uint32_t end_detect_int_ena:1;
        /** byte_trans_done_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to anable the I2C_END_DETECT_INT interrupt.
         */
        uint32_t byte_trans_done_int_ena:1;
        /** arbitration_lost_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to anable the I2C_ARBITRATION_LOST_INT interrupt.
         */
        uint32_t arbitration_lost_int_ena:1;
        /** mst_txfifo_udf_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to anable I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t mst_txfifo_udf_int_ena:1;
        /** trans_complete_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to anable the I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t trans_complete_int_ena:1;
        /** time_out_int_ena : R/W; bitpos: [8]; default: 0;
         *  Write 1 to anable the I2C_TIME_OUT_INT interrupt.
         */
        uint32_t time_out_int_ena:1;
        /** trans_start_int_ena : R/W; bitpos: [9]; default: 0;
         *  Write 1 to anable the I2C_TRANS_START_INT interrupt.
         */
        uint32_t trans_start_int_ena:1;
        /** nack_int_ena : R/W; bitpos: [10]; default: 0;
         *  Write 1 to anable I2C_SLAVE_STRETCH_INT interrupt.
         */
        uint32_t nack_int_ena:1;
        /** txfifo_ovf_int_ena : R/W; bitpos: [11]; default: 0;
         *  Write 1 to anable I2C_TXFIFO_OVF_INT interrupt.
         */
        uint32_t txfifo_ovf_int_ena:1;
        /** rxfifo_udf_int_ena : R/W; bitpos: [12]; default: 0;
         *  Write 1 to anable I2C_RXFIFO_UDF_INT  interrupt.
         */
        uint32_t rxfifo_udf_int_ena:1;
        /** scl_st_to_int_ena : R/W; bitpos: [13]; default: 0;
         *  Write 1 to anable I2C_SCL_ST_TO_INT interrupt.
         */
        uint32_t scl_st_to_int_ena:1;
        /** scl_main_st_to_int_ena : R/W; bitpos: [14]; default: 0;
         *  Write 1 to anable I2C_SCL_MAIN_ST_TO_INT interrupt.
         */
        uint32_t scl_main_st_to_int_ena:1;
        /** det_start_int_ena : R/W; bitpos: [15]; default: 0;
         *  Write 1 to anable I2C_DET_START_INT interrupt.
         */
        uint32_t det_start_int_ena:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_i2c_int_ena_reg_t;

/** Type of int_status register
 *  Status of captured I2C communication events
 */
typedef union {
    struct {
        /** rxfifo_wm_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status status of I2C_RXFIFO_WM_INT interrupt.
         */
        uint32_t rxfifo_wm_int_st:1;
        /** txfifo_wm_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status status of I2C_TXFIFO_WM_INT interrupt.
         */
        uint32_t txfifo_wm_int_st:1;
        /** rxfifo_ovf_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status status of I2C_RXFIFO_OVF_INT interrupt.
         */
        uint32_t rxfifo_ovf_int_st:1;
        /** end_detect_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status status of the I2C_END_DETECT_INT interrupt.
         */
        uint32_t end_detect_int_st:1;
        /** byte_trans_done_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status status of the I2C_END_DETECT_INT interrupt.
         */
        uint32_t byte_trans_done_int_st:1;
        /** arbitration_lost_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status status of the I2C_ARBITRATION_LOST_INT interrupt.
         */
        uint32_t arbitration_lost_int_st:1;
        /** mst_txfifo_udf_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status status of I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t mst_txfifo_udf_int_st:1;
        /** trans_complete_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status status of the I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t trans_complete_int_st:1;
        /** time_out_int_st : RO; bitpos: [8]; default: 0;
         *  The masked interrupt status status of the I2C_TIME_OUT_INT interrupt.
         */
        uint32_t time_out_int_st:1;
        /** trans_start_int_st : RO; bitpos: [9]; default: 0;
         *  The masked interrupt status status of the I2C_TRANS_START_INT interrupt.
         */
        uint32_t trans_start_int_st:1;
        /** nack_int_st : RO; bitpos: [10]; default: 0;
         *  The masked interrupt status status of I2C_SLAVE_STRETCH_INT interrupt.
         */
        uint32_t nack_int_st:1;
        /** txfifo_ovf_int_st : RO; bitpos: [11]; default: 0;
         *  The masked interrupt status status of I2C_TXFIFO_OVF_INT interrupt.
         */
        uint32_t txfifo_ovf_int_st:1;
        /** rxfifo_udf_int_st : RO; bitpos: [12]; default: 0;
         *  The masked interrupt status status of I2C_RXFIFO_UDF_INT  interrupt.
         */
        uint32_t rxfifo_udf_int_st:1;
        /** scl_st_to_int_st : RO; bitpos: [13]; default: 0;
         *  The masked interrupt status status of I2C_SCL_ST_TO_INT interrupt.
         */
        uint32_t scl_st_to_int_st:1;
        /** scl_main_st_to_int_st : RO; bitpos: [14]; default: 0;
         *  The masked interrupt status status of I2C_SCL_MAIN_ST_TO_INT interrupt.
         */
        uint32_t scl_main_st_to_int_st:1;
        /** det_start_int_st : RO; bitpos: [15]; default: 0;
         *  The masked interrupt status status of I2C_DET_START_INT interrupt.
         */
        uint32_t det_start_int_st:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_i2c_int_status_reg_t;


/** Group: Command registers */
/** Type of comd0 register
 *  I2C command register 0
 */
typedef union {
    struct {
        /** command0 : R/W; bitpos: [13:0]; default: 0;
         *  Configures command 0. It consists of three parts:
         *  op_code is the command,
         *  0: RSTART,
         *  1: WRITE,
         *  2: READ,
         *  3: STOP,
         *  4: END.
         *
         *  Byte_num represents the number of bytes that need to be sent or received.
         *  ack_check_en, ack_exp and ack are used to control the ACK bit. See I2C cmd
         *  structure for more information.
         */
        uint32_t command0:14;
        uint32_t reserved_14:17;
        /** command0_done : R/W/SS; bitpos: [31]; default: 0;
         *  Represents whether command 0 is done in I2C Master mode.
         *  0: Not done
         *
         *  1: Done
         */
        uint32_t command0_done:1;
    };
    uint32_t val;
} lp_i2c_comd0_reg_t;

/** Type of comd1 register
 *  I2C command register 1
 */
typedef union {
    struct {
        /** command1 : R/W; bitpos: [13:0]; default: 0;
         *  Configures command 1. See details in I2C_CMD0_REG[13:0].
         */
        uint32_t command1:14;
        uint32_t reserved_14:17;
        /** command1_done : R/W/SS; bitpos: [31]; default: 0;
         *  Represents whether command 1 is done in I2C Master mode.
         *  0: Not done
         *
         *  1: Done
         */
        uint32_t command1_done:1;
    };
    uint32_t val;
} lp_i2c_comd1_reg_t;

/** Type of comd2 register
 *  I2C command register 2
 */
typedef union {
    struct {
        /** command2 : R/W; bitpos: [13:0]; default: 0;
         *  Configures command 2. See details in I2C_CMD0_REG[13:0].
         */
        uint32_t command2:14;
        uint32_t reserved_14:17;
        /** command2_done : R/W/SS; bitpos: [31]; default: 0;
         *  Represents whether command 2 is done in I2C Master mode.
         *  0: Not done
         *
         *  1: Done
         */
        uint32_t command2_done:1;
    };
    uint32_t val;
} lp_i2c_comd2_reg_t;

/** Type of comd3 register
 *  I2C command register 3
 */
typedef union {
    struct {
        /** command3 : R/W; bitpos: [13:0]; default: 0;
         *  Configures command 3. See details in I2C_CMD0_REG[13:0].
         */
        uint32_t command3:14;
        uint32_t reserved_14:17;
        /** command3_done : R/W/SS; bitpos: [31]; default: 0;
         *  Represents whether command 3 is done in I2C Master mode.
         *  0: Not done
         *
         *  1: Done
         */
        uint32_t command3_done:1;
    };
    uint32_t val;
} lp_i2c_comd3_reg_t;

/** Type of comd4 register
 *  I2C command register 4
 */
typedef union {
    struct {
        /** command4 : R/W; bitpos: [13:0]; default: 0;
         *  Configures command 4. See details in I2C_CMD0_REG[13:0].
         */
        uint32_t command4:14;
        uint32_t reserved_14:17;
        /** command4_done : R/W/SS; bitpos: [31]; default: 0;
         *  Represents whether command 4 is done in I2C Master mode.
         *  0: Not done
         *
         *  1: Done
         */
        uint32_t command4_done:1;
    };
    uint32_t val;
} lp_i2c_comd4_reg_t;

/** Type of comd5 register
 *  I2C command register 5
 */
typedef union {
    struct {
        /** command5 : R/W; bitpos: [13:0]; default: 0;
         *  Configures command 5. See details in I2C_CMD0_REG[13:0].
         */
        uint32_t command5:14;
        uint32_t reserved_14:17;
        /** command5_done : R/W/SS; bitpos: [31]; default: 0;
         *  Represents whether command 5 is done in I2C Master mode.
         *  0: Not done
         *
         *  1: Done
         */
        uint32_t command5_done:1;
    };
    uint32_t val;
} lp_i2c_comd5_reg_t;

/** Type of comd6 register
 *  I2C command register 6
 */
typedef union {
    struct {
        /** command6 : R/W; bitpos: [13:0]; default: 0;
         *  Configures command 6. See details in I2C_CMD0_REG[13:0].
         */
        uint32_t command6:14;
        uint32_t reserved_14:17;
        /** command6_done : R/W/SS; bitpos: [31]; default: 0;
         *  Represents whether command 6 is done in I2C Master mode.
         *  0: Not done
         *
         *  1: Done
         */
        uint32_t command6_done:1;
    };
    uint32_t val;
} lp_i2c_comd6_reg_t;

/** Type of comd7 register
 *  I2C command register 7
 */
typedef union {
    struct {
        /** command7 : R/W; bitpos: [13:0]; default: 0;
         *  Configures command 7. See details in I2C_CMD0_REG[13:0].
         */
        uint32_t command7:14;
        uint32_t reserved_14:17;
        /** command7_done : R/W/SS; bitpos: [31]; default: 0;
         *  Represents whether command 7 is done in I2C Master mode.
         *  0: Not done
         *
         *  1: Done
         */
        uint32_t command7_done:1;
    };
    uint32_t val;
} lp_i2c_comd7_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 35656003;
         *  Version control register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} lp_i2c_date_reg_t;


/** Group: Address register */
/** Type of txfifo_start_addr register
 *  I2C TXFIFO base address register
 */
typedef union {
    struct {
        /** txfifo_start_addr : HRO; bitpos: [31:0]; default: 0;
         *  Represents the I2C txfifo first address.
         */
        uint32_t txfifo_start_addr:32;
    };
    uint32_t val;
} lp_i2c_txfifo_start_addr_reg_t;

/** Type of rxfifo_start_addr register
 *  I2C RXFIFO base address register
 */
typedef union {
    struct {
        /** rxfifo_start_addr : HRO; bitpos: [31:0]; default: 0;
         *  Represents the I2C rxfifo first address.
         */
        uint32_t rxfifo_start_addr:32;
    };
    uint32_t val;
} lp_i2c_rxfifo_start_addr_reg_t;


typedef struct {
    volatile lp_i2c_scl_low_period_reg_t scl_low_period;
    volatile lp_i2c_ctr_reg_t ctr;
    volatile lp_i2c_sr_reg_t sr;
    volatile lp_i2c_to_reg_t to;
    uint32_t reserved_010;
    volatile lp_i2c_fifo_st_reg_t fifo_st;
    volatile lp_i2c_fifo_conf_reg_t fifo_conf;
    volatile lp_i2c_data_reg_t data;
    volatile lp_i2c_int_raw_reg_t int_raw;
    volatile lp_i2c_int_clr_reg_t int_clr;
    volatile lp_i2c_int_ena_reg_t int_ena;
    volatile lp_i2c_int_status_reg_t int_status;
    volatile lp_i2c_sda_hold_reg_t sda_hold;
    volatile lp_i2c_sda_sample_reg_t sda_sample;
    volatile lp_i2c_scl_high_period_reg_t scl_high_period;
    uint32_t reserved_03c;
    volatile lp_i2c_scl_start_hold_reg_t scl_start_hold;
    volatile lp_i2c_scl_rstart_setup_reg_t scl_rstart_setup;
    volatile lp_i2c_scl_stop_hold_reg_t scl_stop_hold;
    volatile lp_i2c_scl_stop_setup_reg_t scl_stop_setup;
    volatile lp_i2c_filter_cfg_reg_t filter_cfg;
    volatile lp_i2c_clk_conf_reg_t clk_conf;
    volatile lp_i2c_comd0_reg_t comd0;
    volatile lp_i2c_comd1_reg_t comd1;
    volatile lp_i2c_comd2_reg_t comd2;
    volatile lp_i2c_comd3_reg_t comd3;
    volatile lp_i2c_comd4_reg_t comd4;
    volatile lp_i2c_comd5_reg_t comd5;
    volatile lp_i2c_comd6_reg_t comd6;
    volatile lp_i2c_comd7_reg_t comd7;
    volatile lp_i2c_scl_st_time_out_reg_t scl_st_time_out;
    volatile lp_i2c_scl_main_st_time_out_reg_t scl_main_st_time_out;
    volatile lp_i2c_scl_sp_conf_reg_t scl_sp_conf;
    uint32_t reserved_084[29];
    volatile lp_i2c_date_reg_t date;
    uint32_t reserved_0fc;
    volatile lp_i2c_txfifo_start_addr_reg_t txfifo_start_addr;
    uint32_t reserved_104[31];
    volatile lp_i2c_rxfifo_start_addr_reg_t rxfifo_start_addr;
} lp_i2c_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_i2c_dev_t) == 0x184, "Invalid size of lp_i2c_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
