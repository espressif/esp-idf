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
#ifndef _SOC_RMT_STRUCT_H_
#define _SOC_RMT_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    uint32_t data_ch[8];
    union {
        struct {
            uint32_t tx_start                      :    1;  /*Set this bit to start sending data on CHANNEL$n.*/
            uint32_t mem_rd_rst                    :    1;  /*Set this bit to reset read ram address for CHANNEL$n by accessing transmitter.*/
            uint32_t mem_rst                       :    1;  /*Set this bit to reset W/R ram address for CHANNEL$n by accessing apb fifo.*/
            uint32_t tx_conti_mode                 :    1;  /*Set this bit to restart transmission  from the first data to the last data in CHANNEL$n.*/
            uint32_t mem_tx_wrap_en                :    1;  /*This is the channel $n enable bit for wraparound mode: it will resume sending at the start when the data to be sent is more than its memory size.*/
            uint32_t idle_out_lv                   :    1;  /*This bit configures the level of output signal in CHANNEL$n when the latter is in IDLE state.*/
            uint32_t idle_out_en                   :    1;  /*This is the output enable-control bit for CHANNEL$n in IDLE state.*/
            uint32_t tx_stop                       :    1;  /*Set this bit to stop the transmitter of CHANNEL$n sending data out.*/
            uint32_t div_cnt                       :    8;  /*This register is used to configure the divider for clock of CHANNEL$n.*/
            uint32_t mem_size                      :    4;  /*This register is used to configure the maximum size of memory allocated to CHANNEL$n.*/
            uint32_t carrier_eff_en                :    1;  /*1: Add carrier modulation on the output signal only at the send data state for CHANNEL$n. 0: Add carrier modulation on the output signal at all state for CHANNEL$n. Only valid when RMT_CARRIER_EN_CH$n is 1.*/
            uint32_t carrier_en                    :    1;  /*This is the carrier modulation enable-control bit for CHANNEL$n. 1: Add carrier modulation in the output signal. 0: No carrier modulation in sig_out.*/
            uint32_t carrier_out_lv                :    1;  /*This bit is used to configure the position of carrier wave for CHANNEL$n.; ; 1'h0: add carrier wave on low level.; ; 1'h1: add carrier wave on high level.*/
            uint32_t afifo_rst                     :    1;  /*Reserved*/
            uint32_t conf_update                   :    1;  /*synchronization bit for CHANNEL$n*/
            uint32_t reserved25                    :    7;  /*Reserved*/
        };
        uint32_t val;
    } tx_conf[4];
    struct {
        union {
            struct {
                uint32_t div_cnt                       :    8;  /*This register is used to configure the divider for clock of CHANNEL$m.*/
                uint32_t idle_thres                    :    15;  /*When no edge is detected on the input signal and continuous clock cycles is longer than this register value, received process is finished.*/
                uint32_t reserved23                    :    1;  /*Reserved*/
                uint32_t mem_size                      :    4;  /*This register is used to configure the maximum size of memory allocated to CHANNEL$m.*/
                uint32_t carrier_en                    :    1;  /*This is the carrier modulation enable-control bit for CHANNEL$m. 1: Add carrier modulation in the output signal. 0: No carrier modulation in sig_out.*/
                uint32_t carrier_out_lv                :    1;  /*This bit is used to configure the position of carrier wave for CHANNEL$m.; ; 1'h0: add carrier wave on low level.; ; 1'h1: add carrier wave on high level.*/
                uint32_t reserved30                    :    2;  /*Reserved*/
            };
            uint32_t val;
        } conf0;
        union {
            struct {
                uint32_t rx_en                         :    1;  /*Set this bit to enable receiver to receive data on CHANNEL$m.*/
                uint32_t mem_wr_rst                    :    1;  /*Set this bit to reset write ram address for CHANNEL$m by accessing receiver.*/
                uint32_t mem_rst                       :    1;  /*Set this bit to reset W/R ram address for CHANNEL$m by accessing apb fifo.*/
                uint32_t mem_owner                     :    1;  /*This register marks the ownership of CHANNEL$m's ram block.; ; 1'h1: Receiver is using the ram. ; ; 1'h0: APB bus is using the ram.*/
                uint32_t rx_filter_en                  :    1;  /*This is the receive filter's enable bit for CHANNEL$m.*/
                uint32_t rx_filter_thres               :    8;  /*Ignores the input pulse when its width is smaller than this register value in APB clock periods (in receive mode).*/
                uint32_t mem_rx_wrap_en                :    1;  /*This is the channel $m enable bit for wraparound mode: it will resume receiving at the start when the data to be received is more than its memory size.*/
                uint32_t afifo_rst                     :    1;  /*Reserved*/
                uint32_t conf_update                   :    1;  /*synchronization bit for CHANNEL$m*/
                uint32_t reserved16                    :    16;  /*Reserved*/
            };
            uint32_t val;
        } conf1;
    } rx_conf[4];
    union {
        struct {
            uint32_t mem_raddr_ex                  :    10;  /*This register records the memory address offset when transmitter of CHANNEL$n is using the RAM.*/
            uint32_t reserved10                    :    1;  /*Reserved*/
            uint32_t mem_waddr                     :    10;  /*This register records the memory address offset when writes RAM over APB bus.*/
            uint32_t reserved21                    :    1;  /*Reserved*/
            uint32_t state                         :    3;  /*This register records the FSM status of CHANNEL$n.*/
            uint32_t mem_empty                     :    1;  /*This status bit will be set when the data to be set is more than memory size and the wraparound mode is disabled.*/
            uint32_t mem_wr_err                    :    1;  /*This status bit will be set if the offset address out of memory size when writes via APB bus.*/
            uint32_t reserved27                    :    5;  /*Reserved*/
        };
        uint32_t val;
    } tx_status[4];
    union {
        struct {
            uint32_t mem_waddr_ex                  :    10;  /*This register records the memory address offset when receiver of CHANNEL$m is using the RAM.*/
            uint32_t reserved10                    :    1;  /*Reserved*/
            uint32_t mem_raddr                     :    10;  /*This register records the memory address offset when reads RAM over APB bus.*/
            uint32_t reserved21                    :    1;  /*Reserved*/
            uint32_t state                         :    3;  /*This register records the FSM status of CHANNEL$m.*/
            uint32_t mem_owner_err                 :    1;  /*This status bit will be set when the ownership of memory block is wrong.*/
            uint32_t mem_full                      :    1;  /*This status bit will be set if the receiver receives more data than the memory size.*/
            uint32_t mem_rd_err                    :    1;  /*This status bit will be set if the offset address out of memory size when reads via APB bus.*/
            uint32_t reserved28                    :    4;  /*Reserved*/
        };
        uint32_t val;
    } rx_status[4];
    union {
        struct {
            uint32_t ch0_tx_end                    :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when transmission done.*/
            uint32_t ch1_tx_end                    :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when transmission done.*/
            uint32_t ch2_tx_end                    :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when transmission done.*/
            uint32_t ch3_tx_end                    :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when transmission done.*/
            uint32_t ch0_err                       :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when error occurs.*/
            uint32_t ch1_err                       :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when error occurs.*/
            uint32_t ch2_err                       :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when error occurs.*/
            uint32_t ch3_err                       :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when error occurs.*/
            uint32_t ch0_tx_thr_event              :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when transmitter sent more data than configured value.*/
            uint32_t ch1_tx_thr_event              :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when transmitter sent more data than configured value.*/
            uint32_t ch2_tx_thr_event              :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when transmitter sent more data than configured value.*/
            uint32_t ch3_tx_thr_event              :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when transmitter sent more data than configured value.*/
            uint32_t ch0_tx_loop                   :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when the loop count reaches the configured threshold value.*/
            uint32_t ch1_tx_loop                   :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when the loop count reaches the configured threshold value.*/
            uint32_t ch2_tx_loop                   :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when the loop count reaches the configured threshold value.*/
            uint32_t ch3_tx_loop                   :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when the loop count reaches the configured threshold value.*/
            uint32_t ch4_rx_end                    :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when reception done.*/
            uint32_t ch5_rx_end                    :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when reception done.*/
            uint32_t ch6_rx_end                    :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when reception done.*/
            uint32_t ch7_rx_end                    :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when reception done.*/
            uint32_t ch4_err                       :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when error occurs.*/
            uint32_t ch5_err                       :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when error occurs.*/
            uint32_t ch6_err                       :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when error occurs.*/
            uint32_t ch7_err                       :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when error occurs.*/
            uint32_t ch4_rx_thr_event              :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when receiver receive more data than configured value.*/
            uint32_t ch5_rx_thr_event              :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when receiver receive more data than configured value.*/
            uint32_t ch6_rx_thr_event              :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when receiver receive more data than configured value.*/
            uint32_t ch7_rx_thr_event              :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when receiver receive more data than configured value.*/
            uint32_t ch3_dma_access_fail           :    1;  /*The interrupt raw bit for CHANNEL$n. Triggered when dma accessing CHANNEL$n fails.*/
            uint32_t ch7_dma_access_fail           :    1;  /*The interrupt raw bit for CHANNEL$m. Triggered when dma accessing CHANNEL$m fails.*/
            uint32_t reserved30                    :    2;  /*Reserved*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t ch0_tx_end                    :    1;  /*The masked interrupt status bit for CH$n_TX_END_INT.*/
            uint32_t ch1_tx_end                    :    1;  /*The masked interrupt status bit for CH$n_TX_END_INT.*/
            uint32_t ch2_tx_end                    :    1;  /*The masked interrupt status bit for CH$n_TX_END_INT.*/
            uint32_t ch3_tx_end                    :    1;  /*The masked interrupt status bit for CH$n_TX_END_INT.*/
            uint32_t ch0_err                       :    1;  /*The masked interrupt status bit for CH$n_ERR_INT.*/
            uint32_t ch1_err                       :    1;  /*The masked interrupt status bit for CH$n_ERR_INT.*/
            uint32_t ch2_err                       :    1;  /*The masked interrupt status bit for CH$n_ERR_INT.*/
            uint32_t ch3_err                       :    1;  /*The masked interrupt status bit for CH$n_ERR_INT.*/
            uint32_t ch0_tx_thr_event              :    1;  /*The masked interrupt status bit for CH$n_TX_THR_EVENT_INT.*/
            uint32_t ch1_tx_thr_event              :    1;  /*The masked interrupt status bit for CH$n_TX_THR_EVENT_INT.*/
            uint32_t ch2_tx_thr_event              :    1;  /*The masked interrupt status bit for CH$n_TX_THR_EVENT_INT.*/
            uint32_t ch3_tx_thr_event              :    1;  /*The masked interrupt status bit for CH$n_TX_THR_EVENT_INT.*/
            uint32_t ch0_tx_loop                   :    1;  /*The masked interrupt status bit for CH$n_TX_LOOP_INT.*/
            uint32_t ch1_tx_loop                   :    1;  /*The masked interrupt status bit for CH$n_TX_LOOP_INT.*/
            uint32_t ch2_tx_loop                   :    1;  /*The masked interrupt status bit for CH$n_TX_LOOP_INT.*/
            uint32_t ch3_tx_loop                   :    1;  /*The masked interrupt status bit for CH$n_TX_LOOP_INT.*/
            uint32_t ch4_rx_end                    :    1;  /*The masked interrupt status bit for CH$m_RX_END_INT.*/
            uint32_t ch5_rx_end                    :    1;  /*The masked interrupt status bit for CH$m_RX_END_INT.*/
            uint32_t ch6_rx_end                    :    1;  /*The masked interrupt status bit for CH$m_RX_END_INT.*/
            uint32_t ch7_rx_end                    :    1;  /*The masked interrupt status bit for CH$m_RX_END_INT.*/
            uint32_t ch4_err                       :    1;  /*The masked interrupt status bit for CH$m_ERR_INT.*/
            uint32_t ch5_err                       :    1;  /*The masked interrupt status bit for CH$m_ERR_INT.*/
            uint32_t ch6_err                       :    1;  /*The masked interrupt status bit for CH$m_ERR_INT.*/
            uint32_t ch7_err                       :    1;  /*The masked interrupt status bit for CH$m_ERR_INT.*/
            uint32_t ch4_rx_thr_event              :    1;  /*The masked interrupt status bit for CH$m_RX_THR_EVENT_INT.*/
            uint32_t ch5_rx_thr_event              :    1;  /*The masked interrupt status bit for CH$m_RX_THR_EVENT_INT.*/
            uint32_t ch6_rx_thr_event              :    1;  /*The masked interrupt status bit for CH$m_RX_THR_EVENT_INT.*/
            uint32_t ch7_rx_thr_event              :    1;  /*The masked interrupt status bit for CH$m_RX_THR_EVENT_INT.*/
            uint32_t ch3_dma_access_fail           :    1;  /*The masked interrupt status bit for  CH$n_DMA_ACCESS_FAIL_INT.*/
            uint32_t ch7_dma_access_fail           :    1;  /*The masked interrupt status bit for  CH$m_DMA_ACCESS_FAIL_INT.*/
            uint32_t reserved30                    :    2;  /*Reserved*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t ch0_tx_end                    :    1;  /*The interrupt enable bit for CH$n_TX_END_INT.*/
            uint32_t ch1_tx_end                    :    1;  /*The interrupt enable bit for CH$n_TX_END_INT.*/
            uint32_t ch2_tx_end                    :    1;  /*The interrupt enable bit for CH$n_TX_END_INT.*/
            uint32_t ch3_tx_end                    :    1;  /*The interrupt enable bit for CH$n_TX_END_INT.*/
            uint32_t ch0_err                       :    1;  /*The interrupt enable bit for CH$n_ERR_INT.*/
            uint32_t ch1_err                       :    1;  /*The interrupt enable bit for CH$n_ERR_INT.*/
            uint32_t ch2_err                       :    1;  /*The interrupt enable bit for CH$n_ERR_INT.*/
            uint32_t ch3_err                       :    1;  /*The interrupt enable bit for CH$n_ERR_INT.*/
            uint32_t ch0_tx_thr_event              :    1;  /*The interrupt enable bit for CH$n_TX_THR_EVENT_INT.*/
            uint32_t ch1_tx_thr_event              :    1;  /*The interrupt enable bit for CH$n_TX_THR_EVENT_INT.*/
            uint32_t ch2_tx_thr_event              :    1;  /*The interrupt enable bit for CH$n_TX_THR_EVENT_INT.*/
            uint32_t ch3_tx_thr_event              :    1;  /*The interrupt enable bit for CH$n_TX_THR_EVENT_INT.*/
            uint32_t ch0_tx_loop                   :    1;  /*The interrupt enable bit for CH$n_TX_LOOP_INT.*/
            uint32_t ch1_tx_loop                   :    1;  /*The interrupt enable bit for CH$n_TX_LOOP_INT.*/
            uint32_t ch2_tx_loop                   :    1;  /*The interrupt enable bit for CH$n_TX_LOOP_INT.*/
            uint32_t ch3_tx_loop                   :    1;  /*The interrupt enable bit for CH$n_TX_LOOP_INT.*/
            uint32_t ch4_rx_end                    :    1;  /*The interrupt enable bit for CH$m_RX_END_INT.*/
            uint32_t ch5_rx_end                    :    1;  /*The interrupt enable bit for CH$m_RX_END_INT.*/
            uint32_t ch6_rx_end                    :    1;  /*The interrupt enable bit for CH$m_RX_END_INT.*/
            uint32_t ch7_rx_end                    :    1;  /*The interrupt enable bit for CH$m_RX_END_INT.*/
            uint32_t ch4_err                       :    1;  /*The interrupt enable bit for CH$m_ERR_INT.*/
            uint32_t ch5_err                       :    1;  /*The interrupt enable bit for CH$m_ERR_INT.*/
            uint32_t ch6_err                       :    1;  /*The interrupt enable bit for CH$m_ERR_INT.*/
            uint32_t ch7_err                       :    1;  /*The interrupt enable bit for CH$m_ERR_INT.*/
            uint32_t ch4_rx_thr_event              :    1;  /*The interrupt enable bit for CH$m_RX_THR_EVENT_INT.*/
            uint32_t ch5_rx_thr_event              :    1;  /*The interrupt enable bit for CH$m_RX_THR_EVENT_INT.*/
            uint32_t ch6_rx_thr_event              :    1;  /*The interrupt enable bit for CH$m_RX_THR_EVENT_INT.*/
            uint32_t ch7_rx_thr_event              :    1;  /*The interrupt enable bit for CH$m_RX_THR_EVENT_INT.*/
            uint32_t ch3_dma_access_fail           :    1;  /*The interrupt enable bit for CH$n_DMA_ACCESS_FAIL_INT.*/
            uint32_t ch7_dma_access_fail           :    1;  /*The interrupt enable bit for CH$m_DMA_ACCESS_FAIL_INT.*/
            uint32_t reserved30                    :    2;  /*Reserved*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t ch0_tx_end                    :    1;  /*Set this bit to clear theCH$n_TX_END_INT interrupt.*/
            uint32_t ch1_tx_end                    :    1;  /*Set this bit to clear theCH$n_TX_END_INT interrupt.*/
            uint32_t ch2_tx_end                    :    1;  /*Set this bit to clear theCH$n_TX_END_INT interrupt.*/
            uint32_t ch3_tx_end                    :    1;  /*Set this bit to clear theCH$n_TX_END_INT interrupt.*/
            uint32_t ch0_err                       :    1;  /*Set this bit to clear theCH$n_ERR_INT interrupt.*/
            uint32_t ch1_err                       :    1;  /*Set this bit to clear theCH$n_ERR_INT interrupt.*/
            uint32_t ch2_err                       :    1;  /*Set this bit to clear theCH$n_ERR_INT interrupt.*/
            uint32_t ch3_err                       :    1;  /*Set this bit to clear theCH$n_ERR_INT interrupt.*/
            uint32_t ch0_tx_thr_event              :    1;  /*Set this bit to clear theCH$n_TX_THR_EVENT_INT interrupt.*/
            uint32_t ch1_tx_thr_event              :    1;  /*Set this bit to clear theCH$n_TX_THR_EVENT_INT interrupt.*/
            uint32_t ch2_tx_thr_event              :    1;  /*Set this bit to clear theCH$n_TX_THR_EVENT_INT interrupt.*/
            uint32_t ch3_tx_thr_event              :    1;  /*Set this bit to clear theCH$n_TX_THR_EVENT_INT interrupt.*/
            uint32_t ch0_tx_loop                   :    1;  /*Set this bit to clear theCH$n_TX_LOOP_INT interrupt.*/
            uint32_t ch1_tx_loop                   :    1;  /*Set this bit to clear theCH$n_TX_LOOP_INT interrupt.*/
            uint32_t ch2_tx_loop                   :    1;  /*Set this bit to clear theCH$n_TX_LOOP_INT interrupt.*/
            uint32_t ch3_tx_loop                   :    1;  /*Set this bit to clear theCH$n_TX_LOOP_INT interrupt.*/
            uint32_t ch4_rx_end                    :    1;  /*Set this bit to clear theCH$m_RX_END_INT interrupt.*/
            uint32_t ch5_rx_end                    :    1;  /*Set this bit to clear theCH$m_RX_END_INT interrupt.*/
            uint32_t ch6_rx_end                    :    1;  /*Set this bit to clear theCH$m_RX_END_INT interrupt.*/
            uint32_t ch7_rx_end                    :    1;  /*Set this bit to clear theCH$m_RX_END_INT interrupt.*/
            uint32_t ch4_err                       :    1;  /*Set this bit to clear theCH$m_ERR_INT interrupt.*/
            uint32_t ch5_err                       :    1;  /*Set this bit to clear theCH$m_ERR_INT interrupt.*/
            uint32_t ch6_err                       :    1;  /*Set this bit to clear theCH$m_ERR_INT interrupt.*/
            uint32_t ch7_err                       :    1;  /*Set this bit to clear theCH$m_ERR_INT interrupt.*/
            uint32_t ch4_rx_thr_event              :    1;  /*Set this bit to clear theCH$m_RX_THR_EVENT_INT interrupt.*/
            uint32_t ch5_rx_thr_event              :    1;  /*Set this bit to clear theCH$m_RX_THR_EVENT_INT interrupt.*/
            uint32_t ch6_rx_thr_event              :    1;  /*Set this bit to clear theCH$m_RX_THR_EVENT_INT interrupt.*/
            uint32_t ch7_rx_thr_event              :    1;  /*Set this bit to clear theCH$m_RX_THR_EVENT_INT interrupt.*/
            uint32_t ch3_dma_access_fail           :    1;  /*Set this bit to clear the CH$n_DMA_ACCESS_FAIL_INT interrupt.*/
            uint32_t ch7_dma_access_fail           :    1;  /*Set this bit to clear the CH$m_DMA_ACCESS_FAIL_INT interrupt.*/
            uint32_t reserved30                    :    2;  /*Reserved*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t low                           :    16;  /*This register is used to configure carrier wave 's low level clock period for CHANNEL$n.*/
            uint32_t high                          :    16;  /*This register is used to configure carrier wave 's high level clock period for CHANNEL$n.*/
        };
        uint32_t val;
    } tx_carrier[4];
    union {
        struct {
            uint32_t low_thres                     :    16;  /*The low level period in a carrier modulation mode is (REG_RMT_REG_CARRIER_LOW_THRES_CH$m + 1) for channel $m.*/
            uint32_t high_thres                    :    16;  /*The high level period in a carrier modulation mode is (REG_RMT_REG_CARRIER_HIGH_THRES_CH$m + 1) for channel $m.*/
        };
        uint32_t val;
    } rx_carrier[4];
    union {
        struct {
            uint32_t limit                         :    9;  /*This register is used to configure the maximum entries that CHANNEL$n can send out.*/
            uint32_t tx_loop_num                   :    10;  /*This register is used to configure the maximum loop count when tx_conti_mode is valid.*/
            uint32_t tx_loop_cnt_en                :    1;  /*This register is the enabled bit for loop count.*/
            uint32_t loop_count_reset              :    1;  /*This register is used to reset the loop count when tx_conti_mode is valid.*/
            uint32_t loop_stop_en                  :    1;  /*This bit is used to enable the loop send stop function after the loop counter counts to  loop number for CHANNEL$n.*/
            uint32_t reserved22                    :    10;  /*Reserved*/
        };
        uint32_t val;
    } tx_lim[4];
    union {
        struct {
            uint32_t rx_lim                        :    9;  /*This register is used to configure the maximum entries that CHANNEL$m can receive.*/
            uint32_t reserved9                     :    23;  /*Reserved*/
        };
        uint32_t val;
    } rx_lim[4];
    union {
        struct {
            uint32_t fifo_mask                     :    1;  /*1'h1: access memory directly.   1'h0: access memory by FIFO.*/
            uint32_t mem_clk_force_on              :    1;  /*Set this bit to enable the clock for RMT memory.*/
            uint32_t mem_force_pd                  :    1;  /*Set this bit to power down RMT memory.*/
            uint32_t mem_force_pu                  :    1;  /*1: Disable RMT memory light sleep power down function. 0: Power down RMT memory when RMT is in light sleep mode.*/
            uint32_t sclk_div_num                  :    8;  /*the integral part of the fractional divisor*/
            uint32_t sclk_div_a                    :    6;  /*the numerator of the fractional part of the fractional divisor*/
            uint32_t sclk_div_b                    :    6;  /*the denominator of the fractional part of the fractional divisor*/
            uint32_t sclk_sel                      :    2;  /*choose the clock source of rmt_sclk. 1:CLK_80Mhz;2:CLK_8MHz; 2:XTAL */
            uint32_t sclk_active                   :    1;  /*rmt_sclk switch*/
            uint32_t reserved27                    :    4;  /*Reserved*/
            uint32_t clk_en                        :    1;  /*RMT register clock gate enable signal. 1: Power up the drive clock of registers. 0: Power down the drive clock of registers*/
        };
        uint32_t val;
    } sys_conf;
    union {
        struct {
            uint32_t ch0                           :    1;  /*Set this bit to enable CHANNEL$n to start sending data synchronously with other enabled channels.*/
            uint32_t ch1                           :    1;  /*Set this bit to enable CHANNEL$n to start sending data synchronously with other enabled channels.*/
            uint32_t ch2                           :    1;  /*Set this bit to enable CHANNEL$n to start sending data synchronously with other enabled channels.*/
            uint32_t ch3                           :    1;  /*Set this bit to enable CHANNEL$n to start sending data synchronously with other enabled channels.*/
            uint32_t en                            :    1;  /*This register is used to enable multiple of channels to start sending data synchronously.*/
            uint32_t reserved5                     :    27;  /*Reserved*/
        };
        uint32_t val;
    } tx_sim;
    union {
        struct {
            uint32_t ch0                           :    1;  /*This register is used to reset the clock divider of CHANNEL$n.*/
            uint32_t ch1                           :    1;  /*This register is used to reset the clock divider of CHANNEL$n.*/
            uint32_t ch2                           :    1;  /*This register is used to reset the clock divider of CHANNEL$n.*/
            uint32_t ch3                           :    1;  /*This register is used to reset the clock divider of CHANNEL$n.*/
            uint32_t ch4                           :    1;  /*This register is used to reset the clock divider of CHANNEL$m.*/
            uint32_t ch5                           :    1;  /*This register is used to reset the clock divider of CHANNEL$m.*/
            uint32_t ch6                           :    1;  /*This register is used to reset the clock divider of CHANNEL$m.*/
            uint32_t ch7                           :    1;  /*This register is used to reset the clock divider of CHANNEL$m.*/
            uint32_t reserved8                     :    24;  /*Reserved*/
        };
        uint32_t val;
    } ref_cnt_rst;
    union {
        struct {
            uint32_t date                          :    28;  /*This is the version register.*/
            uint32_t reserved28                    :    4;  /*Reserved*/
        };
        uint32_t val;
    } date;
} rmt_dev_t;

typedef struct {
    union {
        struct {
            uint32_t duration0 : 15;
            uint32_t level0 : 1;
            uint32_t duration1 : 15;
            uint32_t level1 : 1;
        };
        uint32_t val;
    };
} rmt_item32_t;

extern rmt_dev_t RMT;

typedef volatile struct {
    struct {
        union {
            rmt_item32_t data32[48];
        };
    } chan[8];
} rmt_mem_t;

extern rmt_mem_t RMTMEM;

#ifdef __cplusplus
}
#endif



#endif /*_SOC_RMT_STRUCT_H_ */
