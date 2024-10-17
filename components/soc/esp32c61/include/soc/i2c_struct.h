/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
         *  Configures the low level width of the SCL Clock in master mode. \\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_low_period:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i2c_scl_low_period_reg_t;

/** Type of sda_hold register
 *  Configures the hold time after a negative SCL edge
 */
typedef union {
    struct {
        /** sda_hold_time : R/W; bitpos: [8:0]; default: 0;
         *  Configures the time to hold the data after the falling edge of SCL.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t sda_hold_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i2c_sda_hold_reg_t;

/** Type of sda_sample register
 *  Configures the sample time after a positive SCL edge
 */
typedef union {
    struct {
        /** sda_sample_time : R/W; bitpos: [8:0]; default: 0;
         *  Configures the time for sampling SDA.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t sda_sample_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i2c_sda_sample_reg_t;

/** Type of scl_high_period register
 *  Configures the high level width of SCL
 */
typedef union {
    struct {
        /** scl_high_period : R/W; bitpos: [8:0]; default: 0;
         *  Configures for how long SCL remains high in master mode.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_high_period:9;
        /** scl_wait_high_period : R/W; bitpos: [15:9]; default: 0;
         *  Configures the SCL_FSM's waiting period for SCL high level in master mode.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_wait_high_period:7;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} i2c_scl_high_period_reg_t;

/** Type of scl_start_hold register
 *  Configures the delay between the SDA and SCL negative edge for a start condition
 */
typedef union {
    struct {
        /** scl_start_hold_time : R/W; bitpos: [8:0]; default: 8;
         *  Configures the time between the falling edge of SDA and the falling edge of SCL for
         *  a START condition.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_start_hold_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i2c_scl_start_hold_reg_t;

/** Type of scl_rstart_setup register
 *  Configures the delay between the positive
 *  edge of SCL and the negative edge of SDA
 */
typedef union {
    struct {
        /** scl_rstart_setup_time : R/W; bitpos: [8:0]; default: 8;
         *  Configures the time between the positive edge of SCL and the negative edge of SDA
         *  for a RESTART condition.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_rstart_setup_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i2c_scl_rstart_setup_reg_t;

/** Type of scl_stop_hold register
 *  Configures the delay after the SCL clock
 *  edge for a stop condition
 */
typedef union {
    struct {
        /** scl_stop_hold_time : R/W; bitpos: [8:0]; default: 8;
         *  Configures the delay after the STOP condition.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_stop_hold_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i2c_scl_stop_hold_reg_t;

/** Type of scl_stop_setup register
 *  Configures the delay between the SDA and
 *  SCL positive edge for a stop condition
 */
typedef union {
    struct {
        /** scl_stop_setup_time : R/W; bitpos: [8:0]; default: 8;
         *  This register is used to configure the time between the positive edgeof SCL and the
         *  positive edge of SDA, in I2C module clock cycles.
         */
        uint32_t scl_stop_setup_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i2c_scl_stop_setup_reg_t;

/** Type of scl_st_time_out register
 *  SCL status time out register
 */
typedef union {
    struct {
        /** scl_st_to_i2c : R/W; bitpos: [4:0]; default: 16;
         *  Configures the threshold value of SCL_FSM state unchanged period. It should be no
         *  more than 23.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_st_to_i2c:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} i2c_scl_st_time_out_reg_t;

/** Type of scl_main_st_time_out register
 *  SCL main status time out register
 */
typedef union {
    struct {
        /** scl_main_st_to_i2c : R/W; bitpos: [4:0]; default: 16;
         *  Configures the threshold value of SCL_MAIN_FSM state unchanged period. It should be
         *  no more than 23.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_main_st_to_i2c:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} i2c_scl_main_st_time_out_reg_t;


/** Group: Configuration registers */
/** Type of ctr register
 *  Transmission setting
 */
typedef union {
    struct {
        /** sda_force_out : R/W; bitpos: [0]; default: 0;
         *  Configures the SDA output mode.\\
         *  0: Open drain output \\
         *  1: Direct output \\
         */
        uint32_t sda_force_out:1;
        /** scl_force_out : R/W; bitpos: [1]; default: 0;
         *  Configures the SCL output mode.\\
         *  0: Open drain output \\
         *  1: Direct output \\
         */
        uint32_t scl_force_out:1;
        /** sample_scl_level : R/W; bitpos: [2]; default: 0;
         *  Configures the sample mode for SDA.\\
         *  0: Sample SDA data on the SCL high level \\
         *  1: Sample SDA data on the SCL low level \\
         */
        uint32_t sample_scl_level:1;
        /** rx_full_ack_level : R/W; bitpos: [3]; default: 1;
         *  Configures the ACK value that needs to be sent by master when the rx_fifo_cnt has
         *  reached the threshold.\\
         */
        uint32_t rx_full_ack_level:1;
        /** ms_mode : R/W; bitpos: [4]; default: 0;
         *  Configures the module as an I2C Master or Slave.\\
         *  0: Slave \\
         *  1: Master \\
         */
        uint32_t ms_mode:1;
        /** trans_start : WT; bitpos: [5]; default: 0;
         *  Configures whether the slave starts sending the data in txfifo.\\
         *  0: No effect \\
         *  1: Start
         */
        uint32_t trans_start:1;
        /** tx_lsb_first : R/W; bitpos: [6]; default: 0;
         *  Configures to control the sending order for data needing to be sent.\\
         *  0: send data from the most significant bit \\
         *  1: send data from the least significant bit \\
         */
        uint32_t tx_lsb_first:1;
        /** rx_lsb_first : R/W; bitpos: [7]; default: 0;
         *  Configures to control the storage order for received data.\\
         *  0: receive data from the most significant bit \\
         *  1: receive data from the least significant bit \\
         */
        uint32_t rx_lsb_first:1;
        /** clk_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether to gate clock signal for registers.\\
         *  0: Support clock only when registers are read or written to by software \\
         *  1: Force clock on for registers \\
         */
        uint32_t clk_en:1;
        /** arbitration_en : R/W; bitpos: [9]; default: 1;
         *  Configures to enable I2C bus arbitration detection.\\
         *  0: No effect \\
         *  1: Enable \\
         */
        uint32_t arbitration_en:1;
        /** fsm_rst : WT; bitpos: [10]; default: 0;
         *  Configures to reset the SCL_FSM.\\
         *  0: No effect \\
         *  1: Reset
         */
        uint32_t fsm_rst:1;
        /** conf_upgate : WT; bitpos: [11]; default: 0;
         *  Configures this bit for synchronization.\\
         *  0: No effect \\
         *  1: Synchronize
         */
        uint32_t conf_upgate:1;
        /** slv_tx_auto_start_en : R/W; bitpos: [12]; default: 0;
         *  Configures to enable slave to send data automatically\\
         *  0: Disable\\
         *  1: Enable \\
         */
        uint32_t slv_tx_auto_start_en:1;
        /** addr_10bit_rw_check_en : R/W; bitpos: [13]; default: 0;
         *  Configures to check if the r/w bit of 10bit addressing consists with I2C protocol.\\
         *  0: Not check\\
         *  1: Check
         */
        uint32_t addr_10bit_rw_check_en:1;
        /** addr_broadcasting_en : R/W; bitpos: [14]; default: 0;
         *  Configures to support the 7 bit general call function.\\
         *  0: Not support \\
         *  1: Support \\
         */
        uint32_t addr_broadcasting_en:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} i2c_ctr_reg_t;

/** Type of to register
 *  Setting time out control for receiving data
 */
typedef union {
    struct {
        /** time_out_value : R/W; bitpos: [4:0]; default: 16;
         *  Configures the timeout threshold period for SCL stucking at high or low level. The
         *  actual period is 2\^{}(reg_time_out_value).\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t time_out_value:5;
        /** time_out_en : R/W; bitpos: [5]; default: 0;
         *  Configures to enable time out control.\\
         *  0: No effect \\
         *  1: Enable \\
         */
        uint32_t time_out_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} i2c_to_reg_t;

/** Type of slave_addr register
 *  Local slave address setting
 */
typedef union {
    struct {
        /** slave_addr : R/W; bitpos: [14:0]; default: 0;
         *  Configure the slave address of I2C Slave.\\
         */
        uint32_t slave_addr:15;
        uint32_t reserved_15:16;
        /** addr_10bit_en : R/W; bitpos: [31]; default: 0;
         *  Configures to enable the slave 10-bit addressing mode in master mode.\\
         *  0: No effect \\
         *  1: Enable \\
         */
        uint32_t addr_10bit_en:1;
    };
    uint32_t val;
} i2c_slave_addr_reg_t;

/** Type of fifo_conf register
 *  FIFO configuration register
 */
typedef union {
    struct {
        /** rxfifo_wm_thrhd : R/W; bitpos: [4:0]; default: 11;
         *  Configures the water mark threshold of RXFIFO in nonfifo access mode. When
         *  I2C_FIFO_PRT_EN is 1 and RX FIFO counter is bigger than I2C_RXFIFO_WM_THRHD[4:0],
         *  I2C_RXFIFO_WM_INT_RAW bit will be valid.
         *  \tododone{For CJ, please check this description. I habe doubt about
         *  reg_reg_fifo_prt_en.CJ: modified}
         */
        uint32_t rxfifo_wm_thrhd:5;
        /** txfifo_wm_thrhd : R/W; bitpos: [9:5]; default: 4;
         *  Configures the water mark threshold of TXFIFO in nonfifo access mode.  When
         *  I2C_FIFO_PRT_EN is 1 and TC FIFO counter is bigger than I2C_TXFIFO_WM_THRHD[4:0],
         *  I2C_TXFIFO_WM_INT_RAW bit will be valid.
         */
        uint32_t txfifo_wm_thrhd:5;
        /** nonfifo_en : R/W; bitpos: [10]; default: 0;
         *  Configures to enable APB nonfifo access.
         */
        uint32_t nonfifo_en:1;
        /** fifo_addr_cfg_en : R/W; bitpos: [11]; default: 0;
         *  Configures the slave to enable dual address mode. When this mode is enabled, the
         *  byte received after the I2C address byte represents the offset address in the I2C
         *  Slave RAM. \\
         *  0: Disable\\
         *  1: Enable \\
         */
        uint32_t fifo_addr_cfg_en:1;
        /** rx_fifo_rst : R/W; bitpos: [12]; default: 0;
         *  Configures to reset RXFIFO.\\
         *  0: No effect \\
         *  1: Reset
         */
        uint32_t rx_fifo_rst:1;
        /** tx_fifo_rst : R/W; bitpos: [13]; default: 0;
         *  Configures to reset TXFIFO.\\
         *  0: No effect \\
         *  1: Reset
         */
        uint32_t tx_fifo_rst:1;
        /** fifo_prt_en : R/W; bitpos: [14]; default: 1;
         *  Configures to enable FIFO pointer in non-fifo access mode. This bit controls the
         *  valid bits and the TX/RX FIFO overflow, underflow, full and empty interrupts.\\
         *  0: No effect \\
         *  1: Enable \\
         */
        uint32_t fifo_prt_en:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} i2c_fifo_conf_reg_t;

/** Type of filter_cfg register
 *  SCL and SDA filter configuration register
 */
typedef union {
    struct {
        /** scl_filter_thres : R/W; bitpos: [3:0]; default: 0;
         *  Configures the threshold pulse width to be filtered on SCL. When a pulse on the SCL
         *  input has smaller width than this register value, the I2C controller will ignore
         *  that pulse.
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_filter_thres:4;
        /** sda_filter_thres : R/W; bitpos: [7:4]; default: 0;
         *  Configures the threshold pulse width to be filtered on SDA. When a pulse on the SDA
         *  input has smaller width than this register value, the I2C controller will ignore
         *  that pulse.
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t sda_filter_thres:4;
        /** scl_filter_en : R/W; bitpos: [8]; default: 1;
         *  Configures to enable the filter function for SCL. \\
         *  0: No effect \\
         *  1: Enable \\
         */
        uint32_t scl_filter_en:1;
        /** sda_filter_en : R/W; bitpos: [9]; default: 1;
         *  Configures to enable the filter function for SDA. \\
         *  0: No effect \\
         *  1: Enable \\
         */
        uint32_t sda_filter_en:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} i2c_filter_cfg_reg_t;

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
} i2c_clk_conf_reg_t;

/** Type of scl_sp_conf register
 *  Power configuration register
 */
typedef union {
    struct {
        /** scl_rst_slv_en : R/W/SC; bitpos: [0]; default: 0;
         *  Configures to send out SCL pulses when I2C master is IDLE. The number of pulses
         *  equals to I2C_SCL_RST_SLV_NUM[4:0].
         */
        uint32_t scl_rst_slv_en:1;
        /** scl_rst_slv_num : R/W; bitpos: [5:1]; default: 0;
         *  Configure the pulses of SCL generated in I2C master mode. \\
         *  Valid when I2C_SCL_RST_SLV_EN is 1.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t scl_rst_slv_num:5;
        /** scl_pd_en : R/W; bitpos: [6]; default: 0;
         *  Configures to power down the I2C output SCL line. \\
         *  0: Not power down.\\
         *  1: Not work and power down.\\
         *  Valid only when I2C_SCL_FORCE_OUT is 1.
         */
        uint32_t scl_pd_en:1;
        /** sda_pd_en : R/W; bitpos: [7]; default: 0;
         *  Configures to power down the I2C output SDA line. \\
         *  0: Not power down.\\
         *  1: Not work and power down.\\
         *  Valid only when I2C_SDA_FORCE_OUT is 1.
         */
        uint32_t sda_pd_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} i2c_scl_sp_conf_reg_t;

/** Type of scl_stretch_conf register
 *  Set SCL stretch of I2C slave
 */
typedef union {
    struct {
        /** stretch_protect_num : R/W; bitpos: [9:0]; default: 0;
         *  Configures the time period to release the SCL line from stretching to avoid timing
         *  violation. Usually it should be larger than the SDA setup time.\\
         *  Measurement unit: i2c_sclk \\
         */
        uint32_t stretch_protect_num:10;
        /** slave_scl_stretch_en : R/W; bitpos: [10]; default: 0;
         *  Configures to enable slave SCL stretch function. The SCL output line will be
         *  stretched low when I2C_SLAVE_SCL_STRETCH_EN is 1 and stretch event happens. The
         *  stretch cause can be seen in I2C_STRETCH_CAUSE.\\
         *  0: Disable \\
         *  1: Enable \\
         */
        uint32_t slave_scl_stretch_en:1;
        /** slave_scl_stretch_clr : WT; bitpos: [11]; default: 0;
         *  Configures to clear the I2C slave SCL stretch function.\\
         *  0: No effect \\
         *  1: Clear\\
         */
        uint32_t slave_scl_stretch_clr:1;
        /** slave_byte_ack_ctl_en : R/W; bitpos: [12]; default: 0;
         *  Configures to enable the function for slave to control ACK level.\\
         *  0: Disable \\
         *  1: Enable \\
         */
        uint32_t slave_byte_ack_ctl_en:1;
        /** slave_byte_ack_lvl : R/W; bitpos: [13]; default: 0;
         *  Set the ACK level when slave controlling ACK level function enables.\\
         *  0: Low level\\
         *  1: High level \\
         */
        uint32_t slave_byte_ack_lvl:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} i2c_scl_stretch_conf_reg_t;


/** Group: Status registers */
/** Type of sr register
 *  Describe I2C work status
 */
typedef union {
    struct {
        /** resp_rec : RO; bitpos: [0]; default: 0;
         *  Represents the received ACK value in master mode or slave mode.\\
         *  0: ACK\\
         *  1: NACK.
         */
        uint32_t resp_rec:1;
        /** slave_rw : RO; bitpos: [1]; default: 0;
         *  Represents the transfer direction in slave mode.\\
         *  1: Master reads from slave\\
         *  0: Master writes to slave.
         */
        uint32_t slave_rw:1;
        uint32_t reserved_2:1;
        /** arb_lost : RO; bitpos: [3]; default: 0;
         *  Represents whether the I2C controller loses control of SCL line.\\
         *  0: No arbitration lost\\
         *  1: Arbitration lost\\
         */
        uint32_t arb_lost:1;
        /** bus_busy : RO; bitpos: [4]; default: 0;
         *  Represents the I2C bus state.\\
         *  1: The I2C bus is busy transferring data\\
         *  0: The I2C bus is in idle state.\\
         */
        uint32_t bus_busy:1;
        /** slave_addressed : RO; bitpos: [5]; default: 0;
         *  Represents whether the address sent by the master is equal to the address of the
         *  slave.\\
         *  Valid only when the module is configured as an I2C Slave.\\
         *  0: Not equal\\
         *  1: Equal\\
         */
        uint32_t slave_addressed:1;
        uint32_t reserved_6:2;
        /** rxfifo_cnt : RO; bitpos: [13:8]; default: 0;
         *  Represents the number of data bytes received in RAM.
         */
        uint32_t rxfifo_cnt:6;
        /** stretch_cause : RO; bitpos: [15:14]; default: 3;
         *  Represents the cause of SCL clocking stretching in slave mode.\\
         *  0: Stretching SCL low when the master starts to read data.\\
         *  1: Stretching SCL low when I2C TX FIFO is empty in slave mode.\\
         *  2: Stretching SCL low when I2C RX FIFO is full in slave mode.
         */
        uint32_t stretch_cause:2;
        uint32_t reserved_16:2;
        /** txfifo_cnt : RO; bitpos: [23:18]; default: 0;
         *  Represents the number of data bytes to be sent.
         */
        uint32_t txfifo_cnt:6;
        /** scl_main_state_last : RO; bitpos: [26:24]; default: 0;
         *  Represents the states of the I2C module state machine.\\
         *  0: Idle\\
         *  1: Address shift\\
         *  2: ACK address\\
         *  3: Rx data\\
         *  4: Tx data\\
         *  5: Send ACK\\
         *  6: Wait ACK
         */
        uint32_t scl_main_state_last:3;
        uint32_t reserved_27:1;
        /** scl_state_last : RO; bitpos: [30:28]; default: 0;
         *  Represents the states of the state machine used to produce SCL.\\
         *  0: Idle\\
         *  1: Start\\
         *  2: Negative edge\\
         *  3: Low\\
         *  4: Positive edge\\
         *  5: High\\
         *  6: Stop
         */
        uint32_t scl_state_last:3;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} i2c_sr_reg_t;

/** Type of fifo_st register
 *  FIFO status register
 */
typedef union {
    struct {
        /** rxfifo_raddr : RO; bitpos: [4:0]; default: 0;
         *  Represents the offset address of the APB reading from RXFIFO.
         */
        uint32_t rxfifo_raddr:5;
        /** rxfifo_waddr : RO; bitpos: [9:5]; default: 0;
         *  Represents the offset address of i2c module receiving data and writing to RXFIFO.
         */
        uint32_t rxfifo_waddr:5;
        /** txfifo_raddr : RO; bitpos: [14:10]; default: 0;
         *  Represents the offset address of i2c module reading from TXFIFO.
         */
        uint32_t txfifo_raddr:5;
        /** txfifo_waddr : RO; bitpos: [19:15]; default: 0;
         *  Represents the offset address of APB bus writing to TXFIFO.
         */
        uint32_t txfifo_waddr:5;
        uint32_t reserved_20:2;
        /** slave_rw_point : RO; bitpos: [29:22]; default: 0;
         *  Represents the offset address in the I2C Slave RAM addressed by I2C Master when in
         *  I2C slave mode.
         */
        uint32_t slave_rw_point:8;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} i2c_fifo_st_reg_t;

/** Type of data register
 *  Rx FIFO read data
 */
typedef union {
    struct {
        /** fifo_rdata : HRO; bitpos: [7:0]; default: 0;
         *  Represents the value of RXFIFO read data.
         */
        uint32_t fifo_rdata:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} i2c_data_reg_t;


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
        /** slave_stretch_int_raw : R/SS/WTC; bitpos: [16]; default: 0;
         *  The raw interrupt status of I2C_SLAVE_STRETCH_INT interrupt.
         */
        uint32_t slave_stretch_int_raw:1;
        /** general_call_int_raw : R/SS/WTC; bitpos: [17]; default: 0;
         *  The raw interrupt status of I2C_GENARAL_CALL_INT interrupt.
         */
        uint32_t general_call_int_raw:1;
        /** slave_addr_unmatch_int_raw : R/SS/WTC; bitpos: [18]; default: 0;
         *  The raw interrupt status of I2C_SLAVE_ADDR_UNMATCH_INT_RAW interrupt.
         */
        uint32_t slave_addr_unmatch_int_raw:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} i2c_int_raw_reg_t;

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
        /** slave_stretch_int_clr : WT; bitpos: [16]; default: 0;
         *  Write 1 to clear I2C_SLAVE_STRETCH_INT interrupt.
         */
        uint32_t slave_stretch_int_clr:1;
        /** general_call_int_clr : WT; bitpos: [17]; default: 0;
         *  Write 1 to clear I2C_GENARAL_CALL_INT interrupt.
         */
        uint32_t general_call_int_clr:1;
        /** slave_addr_unmatch_int_clr : WT; bitpos: [18]; default: 0;
         *  Write 1 to clear I2C_SLAVE_ADDR_UNMATCH_INT_RAW interrupt.
         */
        uint32_t slave_addr_unmatch_int_clr:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} i2c_int_clr_reg_t;

/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** rxfifo_wm_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable I2C_RXFIFO_WM_INT interrupt.
         */
        uint32_t rxfifo_wm_int_ena:1;
        /** txfifo_wm_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable I2C_TXFIFO_WM_INT interrupt.
         */
        uint32_t txfifo_wm_int_ena:1;
        /** rxfifo_ovf_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable I2C_RXFIFO_OVF_INT interrupt.
         */
        uint32_t rxfifo_ovf_int_ena:1;
        /** end_detect_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable the I2C_END_DETECT_INT interrupt.
         */
        uint32_t end_detect_int_ena:1;
        /** byte_trans_done_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable the I2C_END_DETECT_INT interrupt.
         */
        uint32_t byte_trans_done_int_ena:1;
        /** arbitration_lost_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable the I2C_ARBITRATION_LOST_INT interrupt.
         */
        uint32_t arbitration_lost_int_ena:1;
        /** mst_txfifo_udf_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t mst_txfifo_udf_int_ena:1;
        /** trans_complete_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to enable the I2C_TRANS_COMPLETE_INT interrupt.
         */
        uint32_t trans_complete_int_ena:1;
        /** time_out_int_ena : R/W; bitpos: [8]; default: 0;
         *  Write 1 to enable the I2C_TIME_OUT_INT interrupt.
         */
        uint32_t time_out_int_ena:1;
        /** trans_start_int_ena : R/W; bitpos: [9]; default: 0;
         *  Write 1 to enable the I2C_TRANS_START_INT interrupt.
         */
        uint32_t trans_start_int_ena:1;
        /** nack_int_ena : R/W; bitpos: [10]; default: 0;
         *  Write 1 to enable I2C_SLAVE_STRETCH_INT interrupt.
         */
        uint32_t nack_int_ena:1;
        /** txfifo_ovf_int_ena : R/W; bitpos: [11]; default: 0;
         *  Write 1 to enable I2C_TXFIFO_OVF_INT interrupt.
         */
        uint32_t txfifo_ovf_int_ena:1;
        /** rxfifo_udf_int_ena : R/W; bitpos: [12]; default: 0;
         *  Write 1 to enable I2C_RXFIFO_UDF_INT  interrupt.
         */
        uint32_t rxfifo_udf_int_ena:1;
        /** scl_st_to_int_ena : R/W; bitpos: [13]; default: 0;
         *  Write 1 to enable I2C_SCL_ST_TO_INT interrupt.
         */
        uint32_t scl_st_to_int_ena:1;
        /** scl_main_st_to_int_ena : R/W; bitpos: [14]; default: 0;
         *  Write 1 to enable I2C_SCL_MAIN_ST_TO_INT interrupt.
         */
        uint32_t scl_main_st_to_int_ena:1;
        /** det_start_int_ena : R/W; bitpos: [15]; default: 0;
         *  Write 1 to enable I2C_DET_START_INT interrupt.
         */
        uint32_t det_start_int_ena:1;
        /** slave_stretch_int_ena : R/W; bitpos: [16]; default: 0;
         *  Write 1 to enable I2C_SLAVE_STRETCH_INT interrupt.
         */
        uint32_t slave_stretch_int_ena:1;
        /** general_call_int_ena : R/W; bitpos: [17]; default: 0;
         *  Write 1 to enable I2C_GENARAL_CALL_INT interrupt.
         */
        uint32_t general_call_int_ena:1;
        /** slave_addr_unmatch_int_ena : R/W; bitpos: [18]; default: 0;
         *  Write 1 to enable I2C_SLAVE_ADDR_UNMATCH_INT interrupt.
         */
        uint32_t slave_addr_unmatch_int_ena:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} i2c_int_ena_reg_t;

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
        /** slave_stretch_int_st : RO; bitpos: [16]; default: 0;
         *  The masked interrupt status status of I2C_SLAVE_STRETCH_INT interrupt.
         */
        uint32_t slave_stretch_int_st:1;
        /** general_call_int_st : RO; bitpos: [17]; default: 0;
         *  The masked interrupt status status of I2C_GENARAL_CALL_INT interrupt.
         */
        uint32_t general_call_int_st:1;
        /** slave_addr_unmatch_int_st : RO; bitpos: [18]; default: 0;
         *  The masked interrupt status status of I2C_SLAVE_ADDR_UNMATCH_INT interrupt.
         */
        uint32_t slave_addr_unmatch_int_st:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} i2c_int_status_reg_t;


/** Group: Command registers */
/** Type of comd register
 *  I2C command register 0
 */
typedef union {
    struct {
        /** command : R/W; bitpos: [13:0]; default: 0;
         *  Configures command 0. \\
         *  It consists of three parts:\\
         *  op_code is the command\\
         *  1: WRITE\\
         *  2: STOP\\
         *  3: READ\\
         *  4: END\\
         *  6: RSTART\\
         *  Byte_num represents the number of bytes that need to be sent or received.\\
         *  ack_check_en, ack_exp and ack are used to control the ACK bit. See I2C cmd
         *  structure <a href=fig:i2c-cmd-structure">link</a> for more information.
         *  \\\tododone{for CJ, please add a hyperlink for I2C CMD structure.CJ: done.}"
         */
        uint32_t command:14;
        uint32_t reserved_14:17;
        /** command0_done : R/W/SS; bitpos: [31]; default: 0;
         *  Represents whether command 0 is done in I2C Master mode.\\
         *  0: Not done \\
         *  1: Done \\
         */
        uint32_t command_done:1;
    };
    uint32_t val;
} i2c_comd_reg_t;

/** Group: Version register */
/** Type of date register
 *  Version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 37752896;
         *  Version control register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} i2c_date_reg_t;


typedef struct {
    volatile i2c_scl_low_period_reg_t scl_low_period;
    volatile i2c_ctr_reg_t ctr;
    volatile i2c_sr_reg_t sr;
    volatile i2c_to_reg_t to;
    volatile i2c_slave_addr_reg_t slave_addr;
    volatile i2c_fifo_st_reg_t fifo_st;
    volatile i2c_fifo_conf_reg_t fifo_conf;
    volatile i2c_data_reg_t data;
    volatile i2c_int_raw_reg_t int_raw;
    volatile i2c_int_clr_reg_t int_clr;
    volatile i2c_int_ena_reg_t int_ena;
    volatile i2c_int_status_reg_t int_status;
    volatile i2c_sda_hold_reg_t sda_hold;
    volatile i2c_sda_sample_reg_t sda_sample;
    volatile i2c_scl_high_period_reg_t scl_high_period;
    uint32_t reserved_03c;
    volatile i2c_scl_start_hold_reg_t scl_start_hold;
    volatile i2c_scl_rstart_setup_reg_t scl_rstart_setup;
    volatile i2c_scl_stop_hold_reg_t scl_stop_hold;
    volatile i2c_scl_stop_setup_reg_t scl_stop_setup;
    volatile i2c_filter_cfg_reg_t filter_cfg;
    volatile i2c_clk_conf_reg_t clk_conf;
    volatile i2c_comd_reg_t command[8];
    volatile i2c_scl_st_time_out_reg_t scl_st_time_out;
    volatile i2c_scl_main_st_time_out_reg_t scl_main_st_time_out;
    volatile i2c_scl_sp_conf_reg_t scl_sp_conf;
    volatile i2c_scl_stretch_conf_reg_t scl_stretch_conf;
    uint32_t reserved_088[28];
    volatile i2c_date_reg_t date;
    uint32_t reserved_0fc;
    volatile uint32_t txfifo_mem[32];
    volatile uint32_t rxfifo_mem[32];
} i2c_dev_t;

extern i2c_dev_t I2C0;

#ifndef __cplusplus
_Static_assert(sizeof(i2c_dev_t) == 0x200, "Invalid size of i2c_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
