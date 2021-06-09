// Copyright 2017-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_I2C_STRUCT_H_
#define _SOC_I2C_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t period                        :    9;  /*This register is used to configure for how long SCL remains low in master mode, in I2C module clock cycles. */
            uint32_t reserved9                     :    23;  /*Reserved*/
        };
        uint32_t val;
    } scl_low_period;
    union {
        struct {
            uint32_t sda_force_out                 :    1;  /*0: direct output; 1: open drain output.*/
            uint32_t scl_force_out                 :    1;  /*0: direct output; 1: open drain output.*/
            uint32_t sample_scl_level              :    1;  /*This register is used to select the sample mode.; 1: sample SDA data on the SCL low level.; 0: sample SDA data on the SCL high level.*/
            uint32_t rx_full_ack_level             :    1;  /*This register is used to configure the ACK value that need to sent by master when the rx_fifo_cnt has reached the threshold.*/
            uint32_t ms_mode                       :    1;  /*Set this bit to configure the module as an I2C Master. Clear this bit to configure the; module as an I2C Slave. */
            uint32_t trans_start                   :    1;  /*Set this bit to start sending the data in txfifo. */
            uint32_t tx_lsb_first                  :    1;  /*This bit is used to control the sending mode for data needing to be sent. ; 1: send data from the least significant bit;; 0: send data from the most significant bit.*/
            uint32_t rx_lsb_first                  :    1;  /*This bit is used to control the storage mode for received data.; 1: receive data from the least significant bit;; 0: receive data from the most significant bit.*/
            uint32_t clk_en                        :    1;  /*Reserved*/
            uint32_t arbitration_en                :    1;  /*This is the enable bit for arbitration_lost.*/
            uint32_t fsm_rst                       :    1;  /*This register is used to reset the scl FMS.*/
            uint32_t conf_upgate                   :    1;  /*synchronization bit*/
            uint32_t slv_tx_auto_start_en          :    1;  /*This is the enable bit for slave to send data automatically*/
            uint32_t addr_10bit_rw_check_en        :    1;  /*This is the enable bit to check if the r/w bit of 10bit addressing consists with I2C protocol*/
            uint32_t addr_broadcasting_en          :    1;  /*This is the enable bit to support the 7bit general call function. */
            uint32_t reserved15                    :    17;  /*Reserved*/
        };
        uint32_t val;
    } ctr;
    union {
        struct {
            uint32_t resp_rec                      :    1;  /*The received ACK value in master mode or slave mode. 0: ACK, 1: NACK.*/
            uint32_t slave_rw                      :    1;  /*When in slave mode, 1: master reads from slave; 0: master writes to slave.*/
            uint32_t reserved2                     :    1;  /*Reserved*/
            uint32_t arb_lost                      :    1;  /*When the I2C controller loses control of SCL line, this register changes to 1.*/
            uint32_t bus_busy                      :    1;  /*1: the I2C bus is busy transferring data; 0: the I2C bus is in idle state. */
            uint32_t slave_addressed               :    1;  /*When configured as an I2C Slave, and the address sent by the master is; equal to the address of the slave, then this bit will be of high level.*/
            uint32_t reserved6                     :    1;  /*Reserved*/
            uint32_t reserved7                     :    1;  /*Reserved*/
            uint32_t rx_fifo_cnt                   :    6;  /*This field represents the amount of data needed to be sent. */
            uint32_t stretch_cause                 :    2;  /*The cause of stretching SCL low in slave mode. 0:  stretching SCL low at the beginning of I2C read data state. 1: stretching SCL low when I2C Tx FIFO is empty in slave mode. 2: stretching SCL low when I2C Rx FIFO is full in slave mode.*/
            uint32_t reserved16                    :    2;  /*Reserved*/
            uint32_t tx_fifo_cnt                   :    6;  /*This field stores the amount of received data in RAM. */
            uint32_t scl_main_state_last           :    3;  /*This field indicates the states of the I2C module state machine. ; 0: Idle; 1: Address shift; 2: ACK address; 3: Rx data; 4: Tx data; 5: Send ACK; 6: Wait ACK*/
            uint32_t reserved27                    :    1;  /*Reserved*/
            uint32_t scl_state_last                :    3;  /*This field indicates the states of the state machine used to produce SCL.; 0: Idle; 1: Start; 2: Negative edge; 3: Low; 4: Positive edge; 5: High; 6: Stop*/
            uint32_t reserved31                    :    1;  /*Reserved*/
        };
        uint32_t val;
    } sr;
    union {
        struct {
            uint32_t tout                          :    5;  /*This register is used to configure the timeout for receiving a data bit in APB; clock cycles. */
            uint32_t time_out_en                   :    1;  /*This is the enable bit for time out control.*/
            uint32_t reserved6                     :    26;  /*Reserved*/
        };
        uint32_t val;
    } timeout;
    union {
        struct {
            uint32_t addr                          :    15;  /*When configured as an I2C Slave, this field is used to configure the slave address.*/
            uint32_t reserved15                    :    16;  /*Reserved*/
            uint32_t en_10bit                      :    1;  /*This field is used to enable the slave 10-bit addressing mode in master mode. */
        };
        uint32_t val;
    } slave_addr;
    union {
        struct {
            uint32_t rx_fifo_raddr                 :    5;  /*This is the offset address of the APB reading from rxfifo*/
            uint32_t rx_fifo_waddr                 :    5;  /*This is the offset address of i2c module receiving data and writing to rxfifo.*/
            uint32_t tx_fifo_raddr                 :    5;  /*This is the offset address of i2c module reading from txfifo.*/
            uint32_t tx_fifo_waddr                 :    5;  /*This is the offset address of APB bus writing to txfifo.*/
            uint32_t reserved20                    :    1;  /*Reserved*/
            uint32_t reserved21                    :    1;  /*Reserved*/
            uint32_t slave_rw_point                :    8;  /*The received data in I2C slave mode.*/
            uint32_t reserved30                    :    2;  /*Reserved*/
        };
        uint32_t val;
    } fifo_st;
    union {
        struct {
            uint32_t rx_fifo_wm_thrhd              :    5;  /*The water mark threshold of rx FIFO in nonfifo access mode. When reg_reg_fifo_prt_en is 1 and rx FIFO counter is bigger than reg_rxfifo_wm_thrhd[4:0], reg_rxfifo_wm_int_raw bit will be valid. */
            uint32_t tx_fifo_wm_thrhd              :    5;  /*The water mark threshold of tx FIFO in nonfifo access mode. When reg_reg_fifo_prt_en is 1 and tx FIFO counter is smaller than reg_txfifo_wm_thrhd[4:0], reg_txfifo_wm_int_raw bit will be valid. */
            uint32_t nonfifo_en                    :    1;  /*Set this bit to enable APB nonfifo access. */
            uint32_t fifo_addr_cfg_en              :    1;  /*When this bit is set to 1, the byte received after the I2C address byte represents the offset address in the I2C Slave RAM. */
            uint32_t rx_fifo_rst                   :    1;  /*Set this bit to reset rx-fifo.*/
            uint32_t tx_fifo_rst                   :    1;  /*Set this bit to reset tx-fifo.*/
            uint32_t fifo_prt_en                   :    1;  /*The control enable bit of FIFO pointer in non-fifo access mode. This bit controls the valid bits and the interrupts of tx/rx_fifo overflow, underflow, full and empty.*/
            uint32_t reserved15                    :    5;  /*Reserved*/
            uint32_t reserved20                    :    6;  /*Reserved*/
            uint32_t reserved26                    :    1;  /*Reserved*/
            uint32_t reserved27                    :    5;  /*Reserved*/
        };
        uint32_t val;
    } fifo_conf;
    union {
        struct {
            uint32_t data                          :    8;  /*The value of rx FIFO read data.*/
            uint32_t reserved8                     :    24;  /*Reserved*/
        };
        uint32_t val;
    } fifo_data;
    union {
        struct {
            uint32_t rx_fifo_wm                    :    1;  /*The raw interrupt bit for I2C_RXFIFO_WM_INT interrupt.*/
            uint32_t tx_fifo_wm                    :    1;  /*The raw interrupt bit for I2C_TXFIFO_WM_INT interrupt.*/
            uint32_t rx_fifo_ovf                   :    1;  /*The raw interrupt bit for I2C_RXFIFO_OVF_INT interrupt.*/
            uint32_t end_detect                    :    1;  /*The raw interrupt bit for the I2C_END_DETECT_INT interrupt. */
            uint32_t byte_trans_done               :    1;  /*The raw interrupt bit for the I2C_END_DETECT_INT interrupt. */
            uint32_t arbitration_lost              :    1;  /*The raw interrupt bit for the I2C_ARBITRATION_LOST_INT interrupt. */
            uint32_t mst_tx_fifo_udf               :    1;  /*The raw interrupt bit for I2C_TRANS_COMPLETE_INT interrupt.*/
            uint32_t trans_complete                :    1;  /*The raw interrupt bit for the I2C_TRANS_COMPLETE_INT interrupt.*/
            uint32_t time_out                      :    1;  /*The raw interrupt bit for the I2C_TIME_OUT_INT interrupt. */
            uint32_t trans_start                   :    1;  /*The raw interrupt bit for the I2C_TRANS_START_INT interrupt.*/
            uint32_t nack                          :    1;  /*The raw interrupt bit for I2C_SLAVE_STRETCH_INT interrupt.*/
            uint32_t tx_fifo_ovf                   :    1;  /*The raw interrupt bit for I2C_TXFIFO_OVF_INT interrupt.*/
            uint32_t rx_fifo_udf                   :    1;  /*The raw interrupt bit for I2C_RXFIFO_UDF_INT  interrupt.*/
            uint32_t scl_st_to                     :    1;  /*The raw interrupt bit for I2C_SCL_ST_TO_INT interrupt.*/
            uint32_t scl_main_st_to                :    1;  /*The raw interrupt bit for I2C_SCL_MAIN_ST_TO_INT interrupt.*/
            uint32_t det_start                     :    1;  /*The raw interrupt bit for I2C_DET_START_INT interrupt.*/
            uint32_t slave_stretch                 :    1;  /*The raw interrupt bit for I2C_SLAVE_STRETCH_INT interrupt.*/
            uint32_t general_call                  :    1;  /*The raw interrupt bit for I2C_GENARAL_CALL_INT interrupt.*/
            uint32_t reserved18                    :    14;  /*Reserved*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_fifo_wm                    :    1;  /*Set this bit to clear I2C_RXFIFO_WM_INT interrupt.*/
            uint32_t tx_fifo_wm                    :    1;  /*Set this bit to clear I2C_TXFIFO_WM_INT interrupt.*/
            uint32_t rx_fifo_ovf                   :    1;  /*Set this bit to clear I2C_RXFIFO_OVF_INT interrupt.*/
            uint32_t end_detect                    :    1;  /*Set this bit to clear the I2C_END_DETECT_INT interrupt. */
            uint32_t byte_trans_done               :    1;  /*Set this bit to clear the I2C_END_DETECT_INT interrupt. */
            uint32_t arbitration_lost              :    1;  /*Set this bit to clear the I2C_ARBITRATION_LOST_INT interrupt. */
            uint32_t mst_tx_fifo_udf               :    1;  /*Set this bit to clear I2C_TRANS_COMPLETE_INT interrupt.*/
            uint32_t trans_complete                :    1;  /*Set this bit to clear the I2C_TRANS_COMPLETE_INT interrupt.*/
            uint32_t time_out                      :    1;  /*Set this bit to clear the I2C_TIME_OUT_INT interrupt. */
            uint32_t trans_start                   :    1;  /*Set this bit to clear the I2C_TRANS_START_INT interrupt.*/
            uint32_t nack                          :    1;  /*Set this bit to clear I2C_SLAVE_STRETCH_INT interrupt.*/
            uint32_t tx_fifo_ovf                   :    1;  /*Set this bit to clear I2C_TXFIFO_OVF_INT interrupt.*/
            uint32_t rx_fifo_udf                   :    1;  /*Set this bit to clear I2C_RXFIFO_UDF_INT  interrupt.*/
            uint32_t scl_st_to                     :    1;  /*Set this bit to clear I2C_SCL_ST_TO_INT interrupt.*/
            uint32_t scl_main_st_to                :    1;  /*Set this bit to clear I2C_SCL_MAIN_ST_TO_INT interrupt.*/
            uint32_t det_start                     :    1;  /*Set this bit to clear I2C_DET_START_INT interrupt.*/
            uint32_t slave_stretch                 :    1;  /*Set this bit to clear I2C_SLAVE_STRETCH_INT interrupt.*/
            uint32_t general_call                  :    1;  /*Set this bit for I2C_GENARAL_CALL_INT interrupt.*/
            uint32_t reserved18                    :    14;  /*Reserved*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t rx_fifo_wm                    :    1;  /*The interrupt enable bit for I2C_RXFIFO_WM_INT interrupt.*/
            uint32_t tx_fifo_wm                    :    1;  /*The interrupt enable bit for I2C_TXFIFO_WM_INT interrupt.*/
            uint32_t rx_fifo_ovf                   :    1;  /*The interrupt enable bit for I2C_RXFIFO_OVF_INT interrupt.*/
            uint32_t end_detect                    :    1;  /*The interrupt enable bit for the I2C_END_DETECT_INT interrupt. */
            uint32_t byte_trans_done               :    1;  /*The interrupt enable bit for the I2C_END_DETECT_INT interrupt. */
            uint32_t arbitration_lost              :    1;  /*The interrupt enable bit for the I2C_ARBITRATION_LOST_INT interrupt. */
            uint32_t mst_tx_fifo_udf               :    1;  /*The interrupt enable bit for I2C_TRANS_COMPLETE_INT interrupt.*/
            uint32_t trans_complete                :    1;  /*The interrupt enable bit for the I2C_TRANS_COMPLETE_INT interrupt.*/
            uint32_t time_out                      :    1;  /*The interrupt enable bit for the I2C_TIME_OUT_INT interrupt. */
            uint32_t trans_start                   :    1;  /*The interrupt enable bit for the I2C_TRANS_START_INT interrupt.*/
            uint32_t nack                          :    1;  /*The interrupt enable bit for I2C_SLAVE_STRETCH_INT interrupt.*/
            uint32_t tx_fifo_ovf                   :    1;  /*The interrupt enable bit for I2C_TXFIFO_OVF_INT interrupt.*/
            uint32_t rx_fifo_udf                   :    1;  /*The interrupt enable bit for I2C_RXFIFO_UDF_INT  interrupt.*/
            uint32_t scl_st_to                     :    1;  /*The interrupt enable bit for I2C_SCL_ST_TO_INT interrupt.*/
            uint32_t scl_main_st_to                :    1;  /*The interrupt enable bit for I2C_SCL_MAIN_ST_TO_INT interrupt.*/
            uint32_t det_start                     :    1;  /*The interrupt enable bit for I2C_DET_START_INT interrupt.*/
            uint32_t slave_stretch                 :    1;  /*The interrupt enable bit for I2C_SLAVE_STRETCH_INT interrupt.*/
            uint32_t general_call                  :    1;  /*The interrupt enable bit for I2C_GENARAL_CALL_INT interrupt.*/
            uint32_t reserved18                    :    14;  /*Reserved*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rx_fifo_wm                    :    1;  /*The masked interrupt status bit for I2C_RXFIFO_WM_INT interrupt.*/
            uint32_t tx_fifo_wm                    :    1;  /*The masked interrupt status bit for I2C_TXFIFO_WM_INT interrupt.*/
            uint32_t rx_fifo_ovf                   :    1;  /*The masked interrupt status bit for I2C_RXFIFO_OVF_INT interrupt.*/
            uint32_t end_detect                    :    1;  /*The masked interrupt status bit for the I2C_END_DETECT_INT interrupt. */
            uint32_t byte_trans_done               :    1;  /*The masked interrupt status bit for the I2C_END_DETECT_INT interrupt. */
            uint32_t arbitration_lost              :    1;  /*The masked interrupt status bit for the I2C_ARBITRATION_LOST_INT interrupt. */
            uint32_t mst_tx_fifo_udf               :    1;  /*The masked interrupt status bit for I2C_TRANS_COMPLETE_INT interrupt.*/
            uint32_t trans_complete                :    1;  /*The masked interrupt status bit for the I2C_TRANS_COMPLETE_INT interrupt.*/
            uint32_t time_out                      :    1;  /*The masked interrupt status bit for the I2C_TIME_OUT_INT interrupt. */
            uint32_t trans_start                   :    1;  /*The masked interrupt status bit for the I2C_TRANS_START_INT interrupt.*/
            uint32_t nack                          :    1;  /*The masked interrupt status bit for I2C_SLAVE_STRETCH_INT interrupt.*/
            uint32_t tx_fifo_ovf                   :    1;  /*The masked interrupt status bit for I2C_TXFIFO_OVF_INT interrupt.*/
            uint32_t rx_fifo_udf                   :    1;  /*The masked interrupt status bit for I2C_RXFIFO_UDF_INT  interrupt.*/
            uint32_t scl_st_to                     :    1;  /*The masked interrupt status bit for I2C_SCL_ST_TO_INT interrupt.*/
            uint32_t scl_main_st_to                :    1;  /*The masked interrupt status bit for I2C_SCL_MAIN_ST_TO_INT interrupt.*/
            uint32_t det_start                     :    1;  /*The masked interrupt status bit for I2C_DET_START_INT interrupt.*/
            uint32_t slave_stretch                 :    1;  /*The masked interrupt status bit for I2C_SLAVE_STRETCH_INT interrupt.*/
            uint32_t general_call                  :    1;  /*The masked interrupt status bit for I2C_GENARAL_CALL_INT interrupt.*/
            uint32_t reserved18                    :    14;  /*Reserved*/
        };
        uint32_t val;
    } int_status;
    union {
        struct {
            uint32_t time                          :    9;  /*This register is used to configure the time to hold the data after the negative; edge of SCL, in I2C module clock cycles. */
            uint32_t reserved9                     :    23;  /*Reserved*/
        };
        uint32_t val;
    } sda_hold;
    union {
        struct {
            uint32_t time                          :    9;  /*This register is used to configure for how long SDA is sampled, in I2C module clock cycles. */
            uint32_t reserved9                     :    23;  /*Reserved*/
        };
        uint32_t val;
    } sda_sample;
    union {
        struct {
            uint32_t period                        :    9;  /*This register is used to configure for how long SCL remains high in master mode, in I2C module clock cycles. */
            uint32_t scl_wait_high_period          :    7;  /*This register is used to configure for the SCL_FSM's waiting period for SCL high level in master mode, in I2C module clock cycles. */
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } scl_high_period;
    uint32_t reserved_3c;
    union {
        struct {
            uint32_t time                          :    9;  /*This register is used to configure the time between the negative edge; of SDA and the negative edge of SCL for a START condition, in I2C module clock cycles. */
            uint32_t reserved9                     :    23;  /*Reserved*/
        };
        uint32_t val;
    } scl_start_hold;
    union {
        struct {
            uint32_t time                          :    9;  /*This register is used to configure the time between the positive; edge of SCL and the negative edge of SDA for a RESTART condition, in I2C module clock cycles. */
            uint32_t reserved9                     :    23;  /*Reserved*/
        };
        uint32_t val;
    } scl_rstart_setup;
    union {
        struct {
            uint32_t time                          :    9;  /*This register is used to configure the delay after the STOP condition,; in I2C module clock cycles. */
            uint32_t reserved9                     :    23;  /*Reserved*/
        };
        uint32_t val;
    } scl_stop_hold;
    union {
        struct {
            uint32_t time                          :    9;  /*This register is used to configure the time between the positive edge; of SCL and the positive edge of SDA, in I2C module clock cycles. */
            uint32_t reserved9                     :    23;  /*Reserved*/
        };
        uint32_t val;
    } scl_stop_setup;
    union {
        struct {
            uint32_t scl_thres                     :    4;  /*When a pulse on the SCL input has smaller width than this register value; in I2C module clock cycles, the I2C controller will ignore that pulse. */
            uint32_t sda_thres                     :    4;  /*When a pulse on the SDA input has smaller width than this register value; in I2C module clock cycles, the I2C controller will ignore that pulse. */
            uint32_t scl_en                        :    1;  /*This is the filter enable bit for SCL. */
            uint32_t sda_en                        :    1;  /*This is the filter enable bit for SDA. */
            uint32_t reserved10                    :    22;  /*Reserved*/
        };
        uint32_t val;
    } filter_cfg;
    union {
        struct {
            uint32_t sclk_div_num                  :    8;  /*the integral part of the fractional divisor for i2c module*/
            uint32_t sclk_div_a                    :    6;  /*the numerator of the fractional part of the fractional divisor for i2c module*/
            uint32_t sclk_div_b                    :    6;  /*the denominator of the fractional part of the fractional divisor for i2c module*/
            uint32_t sclk_sel                      :    1;  /*The clock selection for i2c module:0-XTAL;1-CLK_8MHz.*/
            uint32_t sclk_active                   :    1;  /*The clock switch for i2c module*/
            uint32_t reserved22                    :    10;  /*Reserved*/
        };
        uint32_t val;
    } clk_conf;
    union {
        struct {
            uint32_t byte_num:      8;              /*Byte_num represent the number of data need to be send or data need to be received.*/
            uint32_t ack_en:        1;              /*ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
            uint32_t ack_exp:       1;              /*ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
            uint32_t ack_val:       1;              /*ack_check_en  ack_exp and ack value are used to control  the ack bit.*/
            uint32_t op_code:       3;              /*op_code is the command  0：RSTART   1：WRITE  2：READ  3：STOP . 4:END.*/
            uint32_t reserved14:   17;
            uint32_t done:          1;
        };
        uint32_t val;
    } command[8];
    union {
        struct {
            uint32_t scl_st_to                     :    5;  /*The threshold value of SCL_FSM state unchanged period. It should be o more than 23*/
            uint32_t reserved5                     :    27;  /*Reserved*/
        };
        uint32_t val;
    } scl_st_time_out;
    union {
        struct {
            uint32_t scl_main_st_to                :    5;  /*The threshold value of SCL_MAIN_FSM state unchanged period.nIt should be o more than 23*/
            uint32_t reserved5                     :    27;  /*Reserved*/
        };
        uint32_t val;
    } scl_main_st_time_out;
    union {
        struct {
            uint32_t scl_rst_slv_en                :    1;  /*When I2C master is IDLE, set this bit to send out SCL pulses. The number of pulses equals to reg_scl_rst_slv_num[4:0].*/
            uint32_t scl_rst_slv_num               :    5;  /*Configure the pulses of SCL generated in I2C master mode. Valid when reg_scl_rst_slv_en is 1.*/
            uint32_t scl_pd_en                     :    1;  /*The power down enable bit for the I2C output SCL line. 1: Power down. 0: Not power down. Set reg_scl_force_out and reg_scl_pd_en to 1 to stretch SCL low.*/
            uint32_t sda_pd_en                     :    1;  /*The power down enable bit for the I2C output SDA line. 1: Power down. 0: Not power down. Set reg_sda_force_out and reg_sda_pd_en to 1 to stretch SDA low.*/
            uint32_t reserved8                     :    24;  /*Reserved*/
        };
        uint32_t val;
    } scl_sp_conf;
    union {
        struct {
            uint32_t stretch_protect_num           :    10;  /*Configure the period of I2C slave stretching SCL line.*/
            uint32_t slave_scl_stretch_en          :    1;  /*The enable bit for slave SCL stretch function. 1: Enable. 0: Disable. The SCL output line will be stretched low when reg_slave_scl_stretch_en is 1 and stretch event happens. The stretch cause can be seen in reg_stretch_cause.*/
            uint32_t slave_scl_stretch_clr         :    1;  /*Set this bit to clear the I2C slave SCL stretch function.*/
            uint32_t slave_byte_ack_ctl_en         :    1;  /*The enable bit for slave to control ACK level function.*/
            uint32_t slave_byte_ack_level          :    1;  /*Set the ACK level when slave controlling ACK level function enables.*/
            uint32_t reserved14                    :    18;  /*Reserved*/
        };
        uint32_t val;
    } scl_stretch_conf;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t reserved_98;
    uint32_t reserved_9c;
    uint32_t reserved_a0;
    uint32_t reserved_a4;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t date;
    uint32_t reserved_fc;
    uint32_t txfifo_start_addr;
    uint32_t reserved_104;
    uint32_t reserved_108;
    uint32_t reserved_10c;
    uint32_t reserved_110;
    uint32_t reserved_114;
    uint32_t reserved_118;
    uint32_t reserved_11c;
    uint32_t reserved_120;
    uint32_t reserved_124;
    uint32_t reserved_128;
    uint32_t reserved_12c;
    uint32_t reserved_130;
    uint32_t reserved_134;
    uint32_t reserved_138;
    uint32_t reserved_13c;
    uint32_t reserved_140;
    uint32_t reserved_144;
    uint32_t reserved_148;
    uint32_t reserved_14c;
    uint32_t reserved_150;
    uint32_t reserved_154;
    uint32_t reserved_158;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t rxfifo_start_addr;
} i2c_dev_t;
extern i2c_dev_t I2C0;
extern i2c_dev_t I2C1;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_I2C_STRUCT_H_ */
