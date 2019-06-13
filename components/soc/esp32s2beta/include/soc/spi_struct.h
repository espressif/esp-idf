// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_SPI_STRUCT_H_
#define _SOC_SPI_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t reserved0: 18;                       /*reserved*/
            uint32_t usr:        1;                       /*User define command enable.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t reserved19:13;
        };
        uint32_t val;
    } cmd;
    uint32_t addr;                                        /*[31:8]:address to slave  [7:0]:Reserved.*/
    union {
        struct {
            uint32_t reserved0:       2;                  /*reserved*/
            uint32_t ext_hold_en:     1;                  /*Set the bit to hold spi. The bit is combined with spi_usr_prep_hold spi_usr_cmd_hold spi_usr_addr_hold spi_usr_dummy_hold spi_usr_din_hold spi_usr_dout_hold and spi_usr_hold_pol.*/
            uint32_t dummy_out:       1;                  /*In the dummy phase the signal level of spi is output by the spi controller.*/
            uint32_t reserved4:       1;                  /*reserved*/
            uint32_t faddr_dual:      1;                  /*Apply 2 signals during addr phase 1:enable 0: disable*/
            uint32_t faddr_quad:      1;                  /*Apply 4 signals during addr phase 1:enable 0: disable*/
            uint32_t fcmd_dual:       1;                  /*Apply 2 signals during command phase 1:enable 0: disable*/
            uint32_t fcmd_quad:       1;                  /*Apply 4 signals during command phase 1:enable 0: disable*/
            uint32_t reserved9:       4;
            uint32_t fast_rd_mode:    1;                  /*Enable 2/4 lines addr command  including read and write command.*/
            uint32_t fread_dual:      1;                  /*In the read operations  read-data phase apply 2 signals. 1: enable 0: disable.*/
            uint32_t reserved15:      3;
            uint32_t q_pol:           1;                  /*The bit is used to set MISO line polarity  1: high 0  low*/
            uint32_t d_pol:           1;                  /*The bit is used to set MOSI line polarity  1: high 0  low*/
            uint32_t fread_quad:      1;                  /*In the read operations read-data phase apply 4 signals. 1: enable 0: disable.*/
            uint32_t wp:              1;                  /*Write protect signal output when SPI is idle.  1: output high  0: output low.*/
            uint32_t reserved22:      3;                  /*reserved*/
            uint32_t rd_bit_order:    1;                  /*In read-data (MISO) phase 1: LSB first 0: MSB first*/
            uint32_t wr_bit_order:    1;                  /*In command address write-data (MOSI) phases 1: LSB firs 0: MSB first*/
            uint32_t define_addr:     1;                  /*set spi_usr_addr and spi_usr_addr_value in spi defined slave mode. 1: disable 0:enable.*/
            uint32_t reserved28:      4;                  /*reserved*/
        };
        uint32_t val;
    } ctrl;
    union {
        struct {
            uint32_t clk_mode:      2;                    /*SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.*/
            uint32_t clk_mode_13:   1;                    /*{CPOL  CPHA} 1: support spi clk mode 1 and 3  first edge output data B[0]/B[7]*/
            uint32_t rsck_data_out: 1;                    /*It saves half a cycle when tsck is the same as rsck. 1: output data at rsck posedge   0: output data at tsck posedge*/
            uint32_t w16_17_wr_ena: 1;                    /*1:reg_buf[16] [17] can be written   0:reg_buf[16] [17] can not  be written.*/
            uint32_t reserved5:     9;                    /*reserved*/
            uint32_t cs_hold_delay: 6;                    /*SPI cs signal is delayed by spi clock cycles.*/
            uint32_t reserved20:   12;
        };
        uint32_t val;
    } ctrl1;
    union {
        struct {
            uint32_t cs_setup_time:13;                    /*(cycles-1) of prepare phase by spi clock this bits are combined with spi_cs_setup bit.*/
            uint32_t cs_hold_time: 13;                    /*delay cycles of cs pin by spi clock this bits are combined with spi_cs_hold bit.*/
            uint32_t cs_delay_mode: 2;                    /*spi_cs signal is delayed by spi_clk . 0: zero 1: if spi_ck_out_edge or spi_ck_i_edge is set 1 delayed by half cycle  else delayed by one cycle 2: if spi_ck_out_edge or spi_ck_i_edge is set 1 delayed by one cycle  else delayed by half cycle 3: delayed one cycle*/
            uint32_t cs_delay_num:  2;                    /*spi_cs signal is delayed by system clock cycles*/
            uint32_t reserved30:    1;                    /*reserved*/
            uint32_t cs_delay_edge: 1;
        };
        uint32_t val;
    } ctrl2;
    union {
        struct {
            uint32_t clkcnt_l:       6;                   /*In the master mode it must be equal to spi_clkcnt_N. In the slave mode it must be 0.*/
            uint32_t clkcnt_h:       6;                   /*In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In the slave mode it must be 0.*/
            uint32_t clkcnt_n:       6;                   /*In the master mode it is the divider of spi_clk. So spi_clk frequency is system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1)*/
            uint32_t clkdiv_pre:    13;                   /*In the master mode it is pre-divider of spi_clk.*/
            uint32_t clk_equ_sysclk: 1;                   /*In the master mode 1: spi_clk is eqaul to system 0: spi_clk is divided from system clock.*/
        };
        uint32_t val;
    } clock;
    union {
        struct {
            uint32_t doutdin:           1;                /*Set the bit to enable full duplex communication. 1: enable 0: disable.*/
            uint32_t tx_start_bit:      3;                /*It determines the start time of tx output data. It can be used for timing adjustment in MISO slave mode.*/
            uint32_t reserved4:         1;                /*reserved*/
            uint32_t tsck_i_edge:       1;                /*In the slave mode  this bit can be used to change the polarity of tsck. 0: tsck = spi_ck_i. 1:tsck = !spi_ck_i.*/
            uint32_t cs_hold:           1;                /*spi cs keep low when spi is in  done  phase. 1: enable 0: disable.*/
            uint32_t cs_setup:          1;                /*spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable.*/
            uint32_t rsck_i_edge:       1;                /*In the slave mode  this bit can be used to change the polarity of rsck. 0: rsck = !spi_ck_i. 1:rsck = spi_ck_i.*/
            uint32_t ck_out_edge:       1;                /*the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode.*/
            uint32_t rd_byte_order:     1;                /*In read-data (MISO) phase 1: big-endian 0: little_endian*/
            uint32_t wr_byte_order:     1;                /*In command address write-data (MOSI) phases 1: big-endian 0: litte_endian*/
            uint32_t fwrite_dual:       1;                /*In the write operations read-data phase apply 2 signals*/
            uint32_t fwrite_quad:       1;                /*In the write operations read-data phase apply 4 signals*/
            uint32_t reserved14:        1;                /*reserved*/
            uint32_t reserved15:        1;                /*reserved*/
            uint32_t sio:               1;                /*Set the bit to enable 3-line half duplex communication mosi and miso signals share the same pin. 1: enable 0: disable.*/
            uint32_t usr_hold_pol:      1;                /*It is combined with hold bits to set the polarity of spi hold line 1: spi will be held when spi hold line is high 0: spi will be held when spi hold line is low*/
            uint32_t usr_dout_hold:     1;                /*spi is hold at data out state the bit are combined with spi_usr_hold_pol bit.*/
            uint32_t usr_din_hold:      1;                /*spi is hold at data in state the bit are combined with spi_usr_hold_pol bit.*/
            uint32_t usr_dummy_hold:    1;                /*spi is hold at dummy state the bit are combined with spi_usr_hold_pol bit.*/
            uint32_t usr_addr_hold:     1;                /*spi is hold at address state the bit are combined with spi_usr_hold_pol bit.*/
            uint32_t usr_cmd_hold:      1;                /*spi is hold at command state the bit are combined with spi_usr_hold_pol bit.*/
            uint32_t usr_prep_hold:     1;                /*spi is hold at prepare state the bit are combined with spi_usr_hold_pol bit.*/
            uint32_t usr_miso_highpart: 1;                /*read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable.*/
            uint32_t usr_mosi_highpart: 1;                /*write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable.*/
            uint32_t usr_dummy_idle:    1;                /*spi clock is disable in dummy phase when the bit is enable.*/
            uint32_t usr_mosi:          1;                /*This bit enable the write-data phase of an operation.*/
            uint32_t usr_miso:          1;                /*This bit enable the read-data phase of an operation.*/
            uint32_t usr_dummy:         1;                /*This bit enable the dummy phase of an operation.*/
            uint32_t usr_addr:          1;                /*This bit enable the address phase of an operation.*/
            uint32_t usr_command:       1;                /*This bit enable the command phase of an operation.*/
        };
        uint32_t val;
    } user;
    union {
        struct {
            uint32_t usr_dummy_cyclelen: 8;               /*The length in spi_clk cycles of dummy phase. The register value shall be (cycle_num-1).*/
            uint32_t reserved8:         18;               /*reserved*/
            uint32_t usr_addr_bitlen:    6;               /*The length in bits of address phase. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } user1;
    union {
        struct {
            uint32_t usr_command_value: 16;               /*The value of  command.*/
            uint32_t reserved16:        12;               /*reserved*/
            uint32_t usr_command_bitlen: 4;               /*The length in bits of command phase. The register value shall be (bit_num-1)*/
        };
        uint32_t val;
    } user2;
    union {
        struct {
            uint32_t usr_mosi_bit_len:24;                 /*The length in bits of write-data. The register value shall be (bit_num-1).*/
            uint32_t reserved24:       8;                 /*reserved*/
        };
        uint32_t val;
    } mosi_dlen;
    union {
        struct {
            uint32_t usr_miso_bit_len:24;                 /*The length in bits of  read-data. The register value shall be (bit_num-1).*/
            uint32_t reserved24:       8;                 /*reserved*/
        };
        uint32_t val;
    } miso_dlen;
    uint32_t slv_wr_status;                               /*In the master mode this register are the higher 32bits in the 64 bits address condition.*/
    union {
        struct {
            uint32_t cs0_dis:           1;                /*SPI CS0 pin enable  1: disable CS0  0: spi_cs0 signal is from/to CS0 pin*/
            uint32_t cs1_dis:           1;                /*SPI CS1 pin enable  1: disable CS1  0: spi_cs1 signal is from/to CS1 pin*/
            uint32_t cs2_dis:           1;                /*SPI CS2 pin enable  1: disable CS2  0: spi_cs2 signal is from/to CS2 pin*/
            uint32_t reserved3:         2;                /*reserved*/
            uint32_t ck_dis:            1;                /*1: spi clk out disable   0: spi clk out enable*/
            uint32_t master_cs_pol:     3;                /*In the master mode the bits are the polarity of spi cs line  the value is equivalent to spi_cs ^ spi_master_cs_pol.*/
            uint32_t reserved9:        14;                /*reserved*/
            uint32_t slave_cs_pol:      1;                /*spi slave input cs polarity select. 1: inv  0: not change*/
            uint32_t reserved24:        3;                /*reserved*/
            uint32_t q_idle_out:        1;                /*The idle output value of spi_q.*/
            uint32_t reserved28:        1;                /*reserved*/
            uint32_t ck_idle_edge:      1;                /*1: spi clk line is high when idle     0: spi clk line is low when idle*/
            uint32_t cs_keep_active:    1;                /*spi cs line keep low when the bit is set.*/
            uint32_t quad_din_pin_swap: 1;                /*1:  spi quad input swap enable  0:  spi quad input swap disable*/
        };
        uint32_t val;
    } misc;
    union {
        struct {
            uint32_t reserved0:          4;               /*reserved*/
            uint32_t trans_done:         1;               /*The interrupt raw bit for the completion of any operation in both the master mode and the slave mode.*/
            uint32_t int_rd_buf_done_en: 1;               /*spi_slv_rd_buf Interrupt enable. 1: enable 0: disable*/
            uint32_t int_wr_buf_done_en: 1;               /*spi_slv_wr_buf Interrupt enable. 1: enable 0: disable*/
            uint32_t int_rd_dma_done_en: 1;               /*spi_slv_rd_dma Interrupt enable. 1: enable 0: disable*/
            uint32_t int_wr_dma_done_en: 1;               /*spi_slv_wr_dma Interrupt enable. 1: enable 0: disable*/
            uint32_t int_trans_done_en:  1;               /*spi_trans_done Interrupt enable. 1: enable 0: disable*/
            uint32_t reserved10:        13;               /*reserved*/
            uint32_t trans_cnt:          4;               /*The operations counter in both the master mode and the slave mode.*/
            uint32_t reserved27:         1;               /*reserved*/
            uint32_t reserved28:         1;               /*reserved*/
            uint32_t reserved29:         1;               /*reserved*/
            uint32_t slave_mode:         1;               /*Set SPI work mode. 1: slave mode 0: master mode.*/
            uint32_t sync_reset:         1;               /*Software reset enable  reset the spi clock line cs line and data lines.*/
        };
        uint32_t val;
    } slave;
    union {
        struct {
            uint32_t rd_addr_bytelen:     4;              /*In the slave mode it is the address length in bytes for read-buffer operation. The register value shall be byte_num.*/
            uint32_t wr_addr_bytelen:     4;              /*In the slave mode it is the address length in bytes for write-buffer operation. The register value shall be byte_num.*/
            uint32_t reserved8:           3;              /*reserved*/
            uint32_t wr_dma_done:         1;              /*The interrupt raw bit for the completion of dma write operation in the slave mode.*/
            uint32_t last_command:        3;              /*In the slave mode it is the value of command.*/
            uint32_t last_addr:           7;              /*In the slave mode it is the value of address.*/
            uint32_t reserved22:         10;              /*reserved*/
        };
        uint32_t val;
    } slave1;
    union {
        struct {
            uint32_t rd_dummy_bytelen:     4;             /*In the slave mode it is the length in spi_clk byte cycles of dummy phase for read operations.*/
            uint32_t wr_dummy_bytelen:     4;             /*In the slave mode it is the length in spi_clk byte cycles of dummy phase for write operations.*/
            uint32_t rd_dma_done:          1;             /*The interrupt raw bit for the completion of dma read operation in the slave mode.*/
            uint32_t reserved9:           23;             /*reserved*/
        };
        uint32_t val;
    } slave2;
    union {
        struct {
            uint32_t bit_len:          24;                /*In the slave mode it is the length in bits for write-buffer operations. The register value shall be (bit_num-1).*/
            uint32_t wr_buf_done:       1;                /*The interrupt raw bit for the completion of write-buffer operation in the slave mode.*/
            uint32_t reserved25:        7;                /*reserved*/
        };
        uint32_t val;
    } slv_wrbuf_dlen;
    union {
        struct {
            uint32_t bit_len:          24;                /*In the slave mode it is the length in bits for read-buffer operations. The register value shall be (bit_num-1).*/
            uint32_t rd_buf_done:       1;                /*The interrupt raw bit for the completion of read-buffer operation in the slave mode.*/
            uint32_t reserved25:        7;                /*reserved*/
        };
        uint32_t val;
    } slv_rdbuf_dlen;
    union {
        struct {
            uint32_t slv_rdata_bit:24;                    /*In the slave mode it is the byte number of read data.*/
            uint32_t reserved24:    8;                    /*reserved*/
        };
        uint32_t val;
    } slv_rd_byte;
    uint32_t reserved_4c;
    union {
        struct {
            uint32_t st:         3;                       /*The status of spi state machine. 0: idle state  1: preparation state  2: send command state  3: send data state  4: red data state  5:write data state  6: wait state  7: done state.*/
            uint32_t reserved3: 29;                       /*reserved*/
        };
        uint32_t val;
    } fsm;
    union {
        struct {
            uint32_t int_hold_ena:  2;                    /*This register is for two SPI masters to share the same cs clock and data signals. The bits of one SPI are set  if the other SPI is busy  the SPI will be hold. 1(3): hold at  idle  phase 2: hold at  prepare  phase.*/
            uint32_t hold_val:      1;                    /*spi hold output value  which should be used with spi_hold_out_en.*/
            uint32_t hold_out_en:   1;                    /*Enable set spi output hold value to spi_hold_reg. It can be used to hold spi state machine with spi_ext_hold_en and other usr hold signals.*/
            uint32_t hold_out_time: 3;                    /*set the hold cycles of output spi_hold signal when spi_hold_out_en is enable.*/
            uint32_t reserved7:    25;                    /*reserved*/
        };
        uint32_t val;
    } hold;
    union {
        struct {
            uint32_t reserved0:             2;            /*reserved*/
            uint32_t in_rst:                1;            /*The bit is used to reset in dma fsm and in data fifo pointer.*/
            uint32_t out_rst:               1;            /*The bit is used to reset out dma fsm and out data fifo pointer.*/
            uint32_t ahbm_fifo_rst:         1;            /*Reset spi dma ahb master fifo pointer.*/
            uint32_t ahbm_rst:              1;            /*Reset spi dma ahb master.*/
            uint32_t in_loop_test:          1;            /*Set bit to test in link.*/
            uint32_t out_loop_test:         1;            /*Set bit to test out link.*/
            uint32_t out_auto_wrback:       1;            /*when the bit is set  DMA continue to use the next inlink node when the length of inlink is 0.*/
            uint32_t out_eof_mode:          1;            /*out eof flag generation mode . 1: when dma pop all data from fifo  0:when ahb push all data to fifo.*/
            uint32_t outdscr_burst_en:      1;            /*read descriptor use burst mode when read data for memory.*/
            uint32_t indscr_burst_en:       1;            /*read descriptor use burst mode when write data to memory.*/
            uint32_t out_data_burst_en:     1;            /*spi dma read data from memory in burst mode.*/
            uint32_t mem_trans_en:          1;
            uint32_t dma_rx_stop:           1;            /*spi dma read data stop  when in continue tx/rx mode.*/
            uint32_t dma_tx_stop:           1;            /*spi dma write data stop when in continue tx/rx mode.*/
            uint32_t dma_continue:          1;            /*spi dma continue tx/rx data.*/
            uint32_t continue_pop_data_clr: 1;            /*Disable spi slave dma to pop data continuously in next transmission in dma half duplex slave mode. 1: disable continue transmit.   0: enable continue transmit.*/
            uint32_t slv_rx_seg_trans_en:   1;           /*enable DMA segment transfer in slave mode*/
            uint32_t reserved19:           3;            /*reserved*/
            uint32_t infifo_full_clr:        1;
            uint32_t outfifo_empty_clr:      1;
            uint32_t reserved24:          8;
        };
        uint32_t val;
    } dma_conf;
    union {
        struct {
            uint32_t addr:           20;                  /*The address of the first outlink descriptor.*/
            uint32_t reserved20:      8;                  /*reserved*/
            uint32_t stop:            1;                  /*Set the bit to stop to use outlink descriptor.*/
            uint32_t start:           1;                  /*Set the bit to start to use outlink descriptor.*/
            uint32_t restart:         1;                  /*Set the bit to mount on new outlink descriptors.*/
            uint32_t dma_tx_ena:      1;                  /*spi dma write data status bit.*/
        };
        uint32_t val;
    } dma_out_link;
    union {
        struct {
            uint32_t addr:           20;                  /*The address of the first inlink descriptor.*/
            uint32_t auto_ret:        1;                  /*when the bit is set  the inlink descriptor returns to the first link node when a packet is error.*/
            uint32_t reserved21:      7;                  /*reserved*/
            uint32_t stop:            1;                  /*Set the bit to stop to use inlink descriptor.*/
            uint32_t start:           1;                  /*Set the bit to start to use inlink descriptor.*/
            uint32_t restart:         1;                  /*Set the bit to mount on new inlink descriptors.*/
            uint32_t dma_rx_ena:      1;                  /*spi dma read data status bit.*/
        };
        uint32_t val;
    } dma_in_link;
    union {
        struct {
            uint32_t inlink_dscr_empty:          1;       /*The enable bit for lack of enough inlink descriptors.*/
            uint32_t outlink_dscr_error:         1;       /*The enable bit for outlink descriptor error.*/
            uint32_t inlink_dscr_error:          1;       /*The enable bit for inlink descriptor error.*/
            uint32_t in_done:                    1;       /*The enable bit for completing usage of a inlink descriptor.*/
            uint32_t in_err_eof:                 1;       /*The enable bit for receiving error.*/
            uint32_t in_suc_eof:                 1;       /*The enable bit for completing receiving all the packets from host.*/
            uint32_t out_done:                   1;       /*The enable bit for completing usage of a outlink descriptor .*/
            uint32_t out_eof:                    1;       /*The enable bit for sending a packet to host done.*/
            uint32_t out_total_eof:              1;       /*The enable bit for sending all the packets to host done.*/
            uint32_t reserved9:                 23;       /*reserved*/
        };
        uint32_t val;
    } dma_int_ena;
    union {
        struct {
            uint32_t inlink_dscr_empty:          1;       /*The raw bit for lack of enough inlink descriptors.*/
            uint32_t outlink_dscr_error:         1;       /*The raw bit for outlink descriptor error.*/
            uint32_t inlink_dscr_error:          1;       /*The raw bit for inlink descriptor error.*/
            uint32_t in_done:                    1;       /*The raw bit for completing usage of a inlink descriptor.*/
            uint32_t in_err_eof:                 1;       /*The raw bit for receiving error.*/
            uint32_t in_suc_eof:                 1;       /*The raw bit for completing receiving all the packets from host.*/
            uint32_t out_done:                   1;       /*The raw bit for completing usage of a outlink descriptor.*/
            uint32_t out_eof:                    1;       /*The raw bit for sending a packet to host done.*/
            uint32_t out_total_eof:              1;       /*The raw bit for sending all the packets to host done.*/
            uint32_t reserved9:                 23;       /*reserved*/
        };
        uint32_t val;
    } dma_int_raw;
    union {
        struct {
            uint32_t inlink_dscr_empty:         1;        /*The status bit for lack of enough inlink descriptors.*/
            uint32_t outlink_dscr_error:        1;        /*The status bit for outlink descriptor error.*/
            uint32_t inlink_dscr_error:         1;        /*The status bit for inlink descriptor error.*/
            uint32_t in_done:                   1;        /*The status bit for completing usage of a inlink descriptor.*/
            uint32_t in_err_eof:                1;        /*The status bit for receiving error.*/
            uint32_t in_suc_eof:                1;        /*The status bit for completing receiving all the packets from host.*/
            uint32_t out_done:                  1;        /*The status bit for completing usage of a outlink descriptor.*/
            uint32_t out_eof:                   1;        /*The status bit for sending a packet to host done.*/
            uint32_t out_total_eof:             1;        /*The status bit for sending all the packets to host done.*/
            uint32_t reserved9:                23;        /*reserved*/
        };
        uint32_t val;
    } dma_int_st;
    union {
        struct {
            uint32_t inlink_dscr_empty:          1;       /*The clear bit for lack of enough inlink descriptors.*/
            uint32_t outlink_dscr_error:         1;       /*The clear bit for outlink descriptor error.*/
            uint32_t inlink_dscr_error:          1;       /*The clear bit for inlink descriptor error.*/
            uint32_t in_done:                    1;       /*The clear bit for completing usage of a inlink descriptor.*/
            uint32_t in_err_eof:                 1;       /*The clear bit for receiving error.*/
            uint32_t in_suc_eof:                 1;       /*The clear bit for completing receiving all the packets from host.*/
            uint32_t out_done:                   1;       /*The clear bit for completing usage of a outlink descriptor.*/
            uint32_t out_eof:                    1;       /*The clear bit for sending a packet to host done.*/
            uint32_t out_total_eof:              1;       /*The clear bit for sending all the packets to host done.*/
            uint32_t reserved9:                 23;       /*reserved*/
        };
        uint32_t val;
    } dma_int_clr;
    uint32_t dma_in_err_eof_des_addr;                         /*The inlink descriptor address when spi dma produce receiving error.*/
    uint32_t dma_in_suc_eof_des_addr;                         /*The last inlink descriptor address when spi dma produce from_suc_eof.*/
    uint32_t dma_inlink_dscr;                                 /*The content of current in descriptor pointer.*/
    uint32_t dma_inlink_dscr_bf0;                             /*The content of next in descriptor pointer.*/
    uint32_t dma_inlink_dscr_bf1;                             /*The content of current in descriptor data buffer pointer.*/
    uint32_t dma_out_eof_bfr_des_addr;                        /*The address of buffer relative to the outlink descriptor that produce eof.*/
    uint32_t dma_out_eof_des_addr;                            /*The last outlink descriptor address when spi dma produce to_eof.*/
    uint32_t dma_outlink_dscr;                                /*The content of current out descriptor pointer.*/
    uint32_t dma_outlink_dscr_bf0;                            /*The content of next out descriptor pointer.*/
    uint32_t dma_outlink_dscr_bf1;                            /*The content of current out descriptor data buffer pointer.*/
    union {
        struct {
            uint32_t out_dscr_addr:    18;                /*SPI dma out descriptor address.*/
            uint32_t out_dscr_state:    2;                /*SPI dma out descriptor state.*/
            uint32_t out_state:         3;                /*SPI dma out data state.*/
            uint32_t out_fifo_cnt:      7;                /*The remains of SPI dma outfifo data.*/
            uint32_t out_fifo_full:     1;                /*SPI dma outfifo is full.*/
            uint32_t out_fifo_empty:    1;                /*SPI dma outfifo is empty.*/
        };
        uint32_t val;
    } dma_out_status;
    union {
        struct {
            uint32_t in_dscr_addr:    18;                 /*SPI dma in descriptor address.*/
            uint32_t in_dscr_state:    2;                 /*SPI dma in descriptor state.*/
            uint32_t in_state:         3;                 /*SPI dma in data state.*/
            uint32_t in_fifo_cnt:      7;                 /*The remains of SPI dma infifo data.*/
            uint32_t in_fifo_full:     1;                 /*SPI dma infifo is full.*/
            uint32_t in_fifo_empty:    1;                 /*SPI dma infifo is empty.*/
        };
        uint32_t val;
    } dma_in_status;
    uint32_t data_buf[18];                                          /*data buffer*/
    union {
        struct {
            uint32_t din0_mode:     2;                    /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
            uint32_t din1_mode:     2;                    /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
            uint32_t din2_mode:     2;                    /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
            uint32_t din3_mode:     2;                    /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
            uint32_t reserved8:    10;
            uint32_t din0_dly_edge: 1;                    /*The bit is used to select the spi clock edge to modify input line timing.*/
            uint32_t din1_dly_edge: 1;                    /*The bit is used to select the spi clock edge to modify input line timing.*/
            uint32_t din2_dly_edge: 1;                    /*The bit is used to select the spi clock edge to modify input line timing.*/
            uint32_t din3_dly_edge: 1;                    /*The bit is used to select the spi clock edge to modify input line timing.*/
            uint32_t reserved22:   10;                    /*reserved*/
        };
        uint32_t val;
    } din_mode;
    union {
        struct {
            uint32_t din0_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din1_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din2_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din3_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t reserved8: 24;                       /*reserved*/
        };
        uint32_t val;
    } din_num;
    union {
        struct {
            uint32_t dout0_mode:     2;                   /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t dout1_mode:     2;                   /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t dout2_mode:     2;                   /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t dout3_mode:     2;                   /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t dout4_mode:     2;                   /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t reserved10:     8;                   /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t dout0_dly_edge: 1;                   /*The bit is used to select the spi clock edge to modify output line timing.*/
            uint32_t dout1_dly_edge: 1;                   /*The bit is used to select the spi clock edge to modify output line timing.*/
            uint32_t dout2_dly_edge: 1;                   /*The bit is used to select the spi clock edge to modify output line timing.*/
            uint32_t dout3_dly_edge: 1;                   /*The bit is used to select the spi clock edge to modify output line timing.*/
            uint32_t dout4_dly_edge: 1;                   /*The bit is used to select the spi clock edge to modify output line timing.*/
            uint32_t reserved23:     9;                   /*reserved*/
        };
        uint32_t val;
    } dout_mode;
    union {
        struct {
            uint32_t dout0_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout1_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout2_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout3_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout4_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t reserved10:22;                       /*reserved*/
        };
        uint32_t val;
    } dout_num;
    union {
        struct {
            uint32_t sop_en:       1;
            uint32_t sop_mode:     4;
            uint32_t sop_rcmd:     8;
            uint32_t sop_wcmd:     8;
            uint32_t sop_dqs_edge: 1;
            uint32_t reserved22:  10;
        };
        uint32_t val;
    } spr_cfg0;
    union {
        struct {
            uint32_t sop_itl:    12;
            uint32_t sop_len:     6;
            uint32_t sop_dlen:    6;
            uint32_t sop_dc_mode: 1;
            uint32_t reserved25:  7;
        };
        uint32_t val;
    } spr_cfg1;
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
    uint32_t reserved_180;
    uint32_t reserved_184;
    uint32_t reserved_188;
    uint32_t reserved_18c;
    uint32_t reserved_190;
    uint32_t reserved_194;
    uint32_t reserved_198;
    uint32_t reserved_19c;
    uint32_t reserved_1a0;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    uint32_t reserved_1c0;
    uint32_t reserved_1c4;
    uint32_t reserved_1c8;
    uint32_t reserved_1cc;
    uint32_t reserved_1d0;
    uint32_t reserved_1d4;
    uint32_t reserved_1d8;
    uint32_t reserved_1dc;
    uint32_t reserved_1e0;
    uint32_t reserved_1e4;
    uint32_t reserved_1e8;
    uint32_t reserved_1ec;
    uint32_t reserved_1f0;
    uint32_t reserved_1f4;
    uint32_t reserved_1f8;
    uint32_t reserved_1fc;
    uint32_t reserved_200;
    uint32_t reserved_204;
    uint32_t reserved_208;
    uint32_t reserved_20c;
    uint32_t reserved_210;
    uint32_t reserved_214;
    uint32_t reserved_218;
    uint32_t reserved_21c;
    uint32_t reserved_220;
    uint32_t reserved_224;
    uint32_t reserved_228;
    uint32_t reserved_22c;
    uint32_t reserved_230;
    uint32_t reserved_234;
    uint32_t reserved_238;
    uint32_t reserved_23c;
    uint32_t reserved_240;
    uint32_t reserved_244;
    uint32_t reserved_248;
    uint32_t reserved_24c;
    uint32_t reserved_250;
    uint32_t reserved_254;
    uint32_t reserved_258;
    uint32_t reserved_25c;
    uint32_t reserved_260;
    uint32_t reserved_264;
    uint32_t reserved_268;
    uint32_t reserved_26c;
    uint32_t reserved_270;
    uint32_t reserved_274;
    uint32_t reserved_278;
    uint32_t reserved_27c;
    uint32_t reserved_280;
    uint32_t reserved_284;
    uint32_t reserved_288;
    uint32_t reserved_28c;
    uint32_t reserved_290;
    uint32_t reserved_294;
    uint32_t reserved_298;
    uint32_t reserved_29c;
    uint32_t reserved_2a0;
    uint32_t reserved_2a4;
    uint32_t reserved_2a8;
    uint32_t reserved_2ac;
    uint32_t reserved_2b0;
    uint32_t reserved_2b4;
    uint32_t reserved_2b8;
    uint32_t reserved_2bc;
    uint32_t reserved_2c0;
    uint32_t reserved_2c4;
    uint32_t reserved_2c8;
    uint32_t reserved_2cc;
    uint32_t reserved_2d0;
    uint32_t reserved_2d4;
    uint32_t reserved_2d8;
    uint32_t reserved_2dc;
    uint32_t reserved_2e0;
    uint32_t reserved_2e4;
    uint32_t reserved_2e8;
    uint32_t reserved_2ec;
    uint32_t reserved_2f0;
    uint32_t reserved_2f4;
    uint32_t reserved_2f8;
    uint32_t reserved_2fc;
    uint32_t reserved_300;
    uint32_t reserved_304;
    uint32_t reserved_308;
    uint32_t reserved_30c;
    uint32_t reserved_310;
    uint32_t reserved_314;
    uint32_t reserved_318;
    uint32_t reserved_31c;
    uint32_t reserved_320;
    uint32_t reserved_324;
    uint32_t reserved_328;
    uint32_t reserved_32c;
    uint32_t reserved_330;
    uint32_t reserved_334;
    uint32_t reserved_338;
    uint32_t reserved_33c;
    uint32_t reserved_340;
    uint32_t reserved_344;
    uint32_t reserved_348;
    uint32_t reserved_34c;
    uint32_t reserved_350;
    uint32_t reserved_354;
    uint32_t reserved_358;
    uint32_t reserved_35c;
    uint32_t reserved_360;
    uint32_t reserved_364;
    uint32_t reserved_368;
    uint32_t reserved_36c;
    uint32_t reserved_370;
    uint32_t reserved_374;
    uint32_t reserved_378;
    uint32_t reserved_37c;
    uint32_t reserved_380;
    uint32_t reserved_384;
    uint32_t reserved_388;
    uint32_t reserved_38c;
    uint32_t reserved_390;
    uint32_t reserved_394;
    uint32_t reserved_398;
    uint32_t reserved_39c;
    uint32_t reserved_3a0;
    uint32_t reserved_3a4;
    uint32_t reserved_3a8;
    uint32_t reserved_3ac;
    uint32_t reserved_3b0;
    uint32_t reserved_3b4;
    uint32_t reserved_3b8;
    uint32_t reserved_3bc;
    uint32_t reserved_3c0;
    uint32_t reserved_3c4;
    uint32_t reserved_3c8;
    uint32_t reserved_3cc;
    uint32_t reserved_3d0;
    uint32_t reserved_3d4;
    uint32_t reserved_3d8;
    uint32_t reserved_3dc;
    uint32_t reserved_3e0;
    uint32_t reserved_3e4;
    uint32_t reserved_3e8;
    uint32_t reserved_3ec;
    uint32_t reserved_3f0;
    uint32_t reserved_3f4;
    uint32_t reserved_3f8;
    union {
        struct {
            uint32_t date:      28;                       /*SPI register version.*/
            uint32_t reserved28: 4;                       /*reserved*/
        };
        uint32_t val;
    } date;
} spi_dev_t;
extern spi_dev_t GPSPI2;   //FSPI
extern spi_dev_t GPSPI3;   //HSPI
extern spi_dev_t GPSPI4;   //VSPI


_Static_assert(sizeof(spi_dev_t)==0x400, "***invalid spi");


#ifdef __cplusplus
}
#endif

#endif  /* _SOC_SPI_STRUCT_H_ */
