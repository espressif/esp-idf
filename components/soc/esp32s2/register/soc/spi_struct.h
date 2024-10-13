/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct spi_dev_s {
    union {
        struct {
            uint32_t conf_bitlen:23;                      /*Define the spi_clk cycles of  SPI_CONF state. Can be configured in CONF state.*/
            uint32_t reserved23:  1;                      /*reserved*/
            uint32_t usr:         1;                      /*User define command enable.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. Can not be changed by CONF_buf.*/
            uint32_t reserved25:  7;                      /*reserved*/
        };
        uint32_t val;
    } cmd;
    uint32_t addr;                                        /*[31:8]:address to slave  [7:0]:Reserved. Can be configured in CONF state.*/
    union {
        struct {
            uint32_t reserved0:    2;                     /*reserved*/
            uint32_t ext_hold_en:  1;                     /*Set the bit to hold spi. The bit is combined with spi_usr_prep_hold spi_usr_cmd_hold spi_usr_addr_hold spi_usr_dummy_hold spi_usr_din_hold spi_usr_dout_hold and spi_usr_hold_pol. Can be configured in CONF state.*/
            uint32_t dummy_out:    1;                     /*In the dummy phase the signal level of spi is output by the spi controller. Can be configured in CONF state.*/
            uint32_t reserved4:    1;                     /*reserved*/
            uint32_t faddr_dual:   1;                     /*Apply 2 signals during addr phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t faddr_quad:   1;                     /*Apply 4 signals during addr phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t faddr_oct:    1;                     /*Apply 8 signals during addr phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t fcmd_dual:    1;                     /*Apply 2 signals during command phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t fcmd_quad:    1;                     /*Apply 4 signals during command phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t fcmd_oct:     1;                     /*Apply 8 signals during command phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t reserved11:   3;                     /*reserved*/
            uint32_t fread_dual:   1;                     /*In the read operations  read-data phase apply 2 signals. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t fread_quad:   1;                     /*In the read operations read-data phase apply 4 signals. 1: enable 0: disable.  Can be configured in CONF state.*/
            uint32_t fread_oct:    1;                     /*In the read operations read-data phase apply 8 signals. 1: enable 0: disable.  Can be configured in CONF state.*/
            uint32_t reserved17:   1;                     /*reserved*/
            uint32_t q_pol:        1;                     /*The bit is used to set MISO line polarity  1: high 0  low. Can be configured in CONF state.*/
            uint32_t d_pol:        1;                     /*The bit is used to set MOSI line polarity  1: high 0  low. Can be configured in CONF state.*/
            uint32_t reserved20:   1;                     /*reserved*/
            uint32_t wp:           1;                     /*Write protect signal output when SPI is idle.  1: output high  0: output low.  Can be configured in CONF state.*/
            uint32_t reserved22:   3;                     /*reserved*/
            uint32_t rd_bit_order: 1;                     /*In read-data (MISO) phase 1: LSB first 0: MSB first. Can be configured in CONF state.*/
            uint32_t wr_bit_order: 1;                     /*In command address write-data (MOSI) phases 1: LSB firs 0: MSB first. Can be configured in CONF state.*/
            uint32_t reserved27:   5;                     /*reserved*/
        };
        uint32_t val;
    } ctrl;
    union {
        struct {
            uint32_t clk_mode:      2;                    /*SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is always on. Can be configured in CONF state.*/
            uint32_t clk_mode_13:   1;                    /*{CPOL  CPHA} 1: support spi clk mode 1 and 3  first edge output data B[0]/B[7].  0: support spi clk mode 0 and 2  first edge output data B[1]/B[6].*/
            uint32_t rsck_data_out: 1;                    /*It saves half a cycle when tsck is the same as rsck. 1: output data at rsck posedge   0: output data at tsck posedge*/
            uint32_t w16_17_wr_ena: 1;                    /*1:reg_buf[16] [17] can be written   0:reg_buf[16] [17] can not  be written. Can be configured in CONF state.*/
            uint32_t reserved5:     9;                    /*reserved*/
            uint32_t cs_hold_delay: 6;                    /*SPI cs signal is delayed by spi clock cycles. Can be configured in CONF state.*/
            uint32_t reserved20:   12;                    /*reserved*/
        };
        uint32_t val;
    } ctrl1;
    union {
        struct {
            uint32_t cs_setup_time:13;                    /*(cycles+1) of prepare phase by spi clock this bits are combined with spi_cs_setup bit. Can be configured in CONF state.*/
            uint32_t cs_hold_time: 13;                    /*delay cycles of cs pin by spi clock this bits are combined with spi_cs_hold bit. Can be configured in CONF state.*/
            uint32_t cs_delay_mode: 3;                    /*spi_cs signal is delayed by spi_clk . 0: zero 1: if spi_ck_out_edge or spi_ck_i_edge is set 1 delayed by half cycle  else delayed by one cycle 2: if spi_ck_out_edge or spi_ck_i_edge is set 1 delayed by one cycle  else delayed by half cycle 3: delayed one cycle. Can be configured in CONF state.*/
            uint32_t cs_delay_num:  2;                    /*spi_cs signal is delayed by system clock cycles. Can be configured in CONF state.*/
            uint32_t reserved31:    1;                    /*reserved*/
        };
        uint32_t val;
    } ctrl2;
    union {
        struct {
            uint32_t clkcnt_l:       6;                   /*In the master mode it must be equal to spi_clkcnt_N. In the slave mode it must be 0. Can be configured in CONF state.*/
            uint32_t clkcnt_h:       6;                   /*In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In the slave mode it must be 0. Can be configured in CONF state.*/
            uint32_t clkcnt_n:       6;                   /*In the master mode it is the divider of spi_clk. So spi_clk frequency is system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1). Can be configured in CONF state.*/
            uint32_t clkdiv_pre:    13;                   /*In the master mode it is pre-divider of spi_clk.  Can be configured in CONF state.*/
            uint32_t clk_equ_sysclk: 1;                   /*In the master mode 1: spi_clk is equal to system 0: spi_clk is divided from system clock. Can be configured in CONF state.*/
        };
        uint32_t val;
    } clock;
    union {
        struct {
            uint32_t doutdin:           1;                /*Set the bit to enable full duplex communication. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t reserved1:         2;                /*reserved*/
            uint32_t qpi_mode:          1;                /*Both for master mode and slave mode. 1: spi controller is in QPI mode. 0: others. Can be configured in CONF state.*/
            uint32_t opi_mode:          1;                /*Just for master mode. 1: spi controller is in OPI mode (all in 8-b-m). 0: others. Can be configured in CONF state.*/
            uint32_t tsck_i_edge:       1;                /*In the slave mode  this bit can be used to change the polarity of tsck. 0: tsck = spi_ck_i. 1:tsck = !spi_ck_i.*/
            uint32_t cs_hold:           1;                /*spi cs keep low when spi is in  done  phase. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t cs_setup:          1;                /*spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t rsck_i_edge:       1;                /*In the slave mode  this bit can be used to change the polarity of rsck. 0: rsck = !spi_ck_i. 1:rsck = spi_ck_i.*/
            uint32_t ck_out_edge:       1;                /*the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode. Can be configured in CONF state.*/
            uint32_t rd_byte_order:     1;                /*In read-data (MISO) phase 1: big-endian 0: little_endian. Can be configured in CONF state.*/
            uint32_t wr_byte_order:     1;                /*In command address write-data (MOSI) phases 1: big-endian 0: litte_endian. Can be configured in CONF state.*/
            uint32_t fwrite_dual:       1;                /*In the write operations read-data phase apply 2 signals. Can be configured in CONF state.*/
            uint32_t fwrite_quad:       1;                /*In the write operations read-data phase apply 4 signals. Can be configured in CONF state.*/
            uint32_t fwrite_oct:        1;                /*In the write operations read-data phase apply 8 signals. Can be configured in CONF state.*/
            uint32_t usr_conf_nxt:      1;                /*1: Enable the DMA CONF phase of next seg-trans operation  which means seg-trans will continue. 0: The seg-trans will end after the current SPI seg-trans or this is not seg-trans mode. Can be configured in CONF state.*/
            uint32_t sio:               1;                /*Set the bit to enable 3-line half duplex communication mosi and miso signals share the same pin. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t usr_hold_pol:      1;                /*It is combined with hold bits to set the polarity of spi hold line 1: spi will be held when spi hold line is high 0: spi will be held when spi hold line is low. Can be configured in CONF state.*/
            uint32_t usr_dout_hold:     1;                /*spi is hold at data out state the bit are combined with spi_usr_hold_pol bit. Can be configured in CONF state.*/
            uint32_t usr_din_hold:      1;                /*spi is hold at data in state the bit are combined with spi_usr_hold_pol bit. Can be configured in CONF state.*/
            uint32_t usr_dummy_hold:    1;                /*spi is hold at dummy state the bit are combined with spi_usr_hold_pol bit. Can be configured in CONF state.*/
            uint32_t usr_addr_hold:     1;                /*spi is hold at address state the bit are combined with spi_usr_hold_pol bit. Can be configured in CONF state.*/
            uint32_t usr_cmd_hold:      1;                /*spi is hold at command state the bit are combined with spi_usr_hold_pol bit. Can be configured in CONF state.*/
            uint32_t usr_prep_hold:     1;                /*spi is hold at prepare state the bit are combined with spi_usr_hold_pol bit. Can be configured in CONF state.*/
            uint32_t usr_miso_highpart: 1;                /*read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t usr_mosi_highpart: 1;                /*write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable.  Can be configured in CONF state.*/
            uint32_t usr_dummy_idle:    1;                /*spi clock is disable in dummy phase when the bit is enable. Can be configured in CONF state.*/
            uint32_t usr_mosi:          1;                /*This bit enable the write-data phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_miso:          1;                /*This bit enable the read-data phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_dummy:         1;                /*This bit enable the dummy phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_addr:          1;                /*This bit enable the address phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_command:       1;                /*This bit enable the command phase of an operation. Can be configured in CONF state.*/
        };
        uint32_t val;
    } user;
    union {
        struct {
            uint32_t usr_dummy_cyclelen: 8;               /*The length in spi_clk cycles of dummy phase. The register value shall be (cycle_num-1). Can be configured in CONF state.*/
            uint32_t reserved8:         19;               /*reserved*/
            uint32_t usr_addr_bitlen:    5;               /*The length in bits of address phase. The register value shall be (bit_num-1). Can be configured in CONF state.*/
        };
        uint32_t val;
    } user1;
    union {
        struct {
            uint32_t usr_command_value: 16;               /*The value of  command. Can be configured in CONF state.*/
            uint32_t reserved16:        12;               /*reserved*/
            uint32_t usr_command_bitlen: 4;               /*The length in bits of command phase. The register value shall be (bit_num-1). Can be configured in CONF state.*/
        };
        uint32_t val;
    } user2;
    union {
        struct {
            uint32_t usr_mosi_bit_len:23;                 /*The length in bits of write-data. The register value shall be (bit_num-1). Can be configured in CONF state.*/
            uint32_t reserved23:       9;                 /*reserved*/
        };
        uint32_t val;
    } mosi_dlen;
    union {
        struct {
            uint32_t usr_miso_bit_len:23;                 /*The length in bits of  read-data. The register value shall be (bit_num-1). Can be configured in CONF state.*/
            uint32_t reserved23:       9;                 /*reserved*/
        };
        uint32_t val;
    } miso_dlen;
    union {
        struct {
            uint32_t cs0_dis:           1;                /*SPI CS0 pin enable  1: disable CS0  0: spi_cs0 signal is from/to CS0 pin. Can be configured in CONF state.*/
            uint32_t cs1_dis:           1;                /*SPI CS1 pin enable  1: disable CS1  0: spi_cs1 signal is from/to CS1 pin. Can be configured in CONF state.*/
            uint32_t cs2_dis:           1;                /*SPI CS2 pin enable  1: disable CS2  0: spi_cs2 signal is from/to CS2 pin. Can be configured in CONF state.*/
            uint32_t cs3_dis:           1;                /*reserved*/
            uint32_t cs4_dis:           1;                /*SPI CS4 pin enable  1: disable CS4  0: spi_cs4 signal is from/to CS4 pin. Can be configured in CONF state.*/
            uint32_t cs5_dis:           1;                /*SPI CS5 pin enable  1: disable CS5  0: spi_cs5 signal is from/to CS5 pin. Can be configured in CONF state.*/
            uint32_t ck_dis:            1;                /*1: spi clk out disable   0: spi clk out enable. Can be configured in CONF state.*/
            uint32_t master_cs_pol:     6;                /*In the master mode the bits are the polarity of spi cs line  the value is equivalent to spi_cs ^ spi_master_cs_pol. Can be configured in CONF state.*/
            uint32_t reserved13:        3;                /*reserved*/
            uint32_t clk_data_dtr_en:   1;                /*1: SPI master DTR mode is applied to SPI clk  data and spi_dqs*/
            uint32_t data_dtr_en:       1;                /*1: SPI clk and data of SPI_DOUT and SPI_DIN state are in DTR mode  including master 1/2/4/8-bm. Can be configured in CONF state.*/
            uint32_t addr_dtr_en:       1;                /*1: SPI clk and data of SPI_SEND_ADDR state are in DTR mode  including master 1/2/4/8-bm. Can be configured in CONF state.*/
            uint32_t cmd_dtr_en:        1;                /*1: SPI clk and data of SPI_SEND_CMD state are in DTR mode  including master 1/2/4/8-bm. Can be configured in CONF state.*/
            uint32_t cd_data_set:       1;                /*1: spi_cd = !spi_cd_idle_edge when spi_st[3:0] is in SPI_DOUT or SPI_DIN state.  0: spi_cd = spi_cd_idle_edge. Can be configured in CONF state.*/
            uint32_t cd_dummy_set:      1;                /*1: spi_cd = !spi_cd_idle_edge when spi_st[3:0] is in SPI_DUMMY state.  0: spi_cd = spi_cd_idle_edge. Can be configured in CONF state.*/
            uint32_t cd_addr_set:       1;                /*1: spi_cd = !spi_cd_idle_edge when spi_st[3:0] is in SPI_SEND_ADDR state.  0: spi_cd = spi_cd_idle_edge. Can be configured in CONF state.*/
            uint32_t slave_cs_pol:      1;                /*spi slave input cs polarity select. 1: inv  0: not change. Can be configured in CONF state.*/
            uint32_t dqs_idle_edge:     1;                /*The default value of spi_dqs. Can be configured in CONF state.*/
            uint32_t cd_cmd_set:        1;                /*1: spi_cd = !spi_cd_idle_edge when spi_st[3:0] is in SPI_SEND_CMD state.  0: spi_cd = spi_cd_idle_edge. Can be configured in CONF state.*/
            uint32_t cd_idle_edge:      1;                /*The default value of spi_cd. Can be configured in CONF state.*/
            uint32_t reserved27:        2;                /*reserved*/
            uint32_t ck_idle_edge:      1;                /*1: spi clk line is high when idle     0: spi clk line is low when idle. Can be configured in CONF state.*/
            uint32_t cs_keep_active:    1;                /*spi cs line keep low when the bit is set. Can be configured in CONF state.*/
            uint32_t quad_din_pin_swap: 1;                /*1:  spi quad input swap enable  0:  spi quad input swap disable. Can be configured in CONF state.*/
        };
        uint32_t val;
    } misc;
    union {
        struct {
            uint32_t reserved0:              4;           /*reserved*/
            uint32_t trans_done:             1;           /*The interrupt raw bit for the completion of any operation in both the master mode and the slave mode.  Can not be changed by CONF_buf.*/
            uint32_t int_rd_buf_done_en:     1;           /*spi_slv_rd_buf Interrupt enable. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t int_wr_buf_done_en:     1;           /*spi_slv_wr_buf Interrupt enable. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t int_rd_dma_done_en:     1;           /*spi_slv_rd_dma Interrupt enable. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t int_wr_dma_done_en:     1;           /*spi_slv_wr_dma Interrupt enable. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t int_trans_done_en:      1;           /*spi_trans_done Interrupt enable. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t int_dma_seg_trans_en:   1;           /*spi_dma_seg_trans_done Interrupt enable. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t seg_magic_err_int_en:   1;           /*1: Enable seg magic value error interrupt. 0: Others. Can be configured in CONF state.*/
            uint32_t reserved12:            11;           /*reserved*/
            uint32_t trans_cnt:              4;           /*The operations counter in both the master mode and the slave mode.*/
            uint32_t reserved27:             1;           /*reserved*/
            uint32_t reserved28:             1;           /*reserved*/
            uint32_t trans_done_auto_clr_en: 1;           /*spi_trans_done auto clear enable  clear it 3 apb cycles after the pos edge of spi_trans_done.  0:disable. 1: enable. Can be configured in CONF state.*/
            uint32_t slave_mode:             1;           /*Set SPI work mode. 1: slave mode 0: master mode.*/
            uint32_t soft_reset:             1;           /*Software reset enable  reset the spi clock line cs line and data lines. Can be configured in CONF state.*/
        };
        uint32_t val;
    } slave;
    union {
        struct {
            uint32_t reserved0:       10;                 /*reserved*/
            uint32_t addr_err_clr:     1;                 /*1: Clear spi_slv_addr_err. 0: not valid. Can be changed by CONF_buf.*/
            uint32_t cmd_err_clr:      1;                 /*1: Clear spi_slv_cmd_err. 0: not valid.  Can be changed by CONF_buf.*/
            uint32_t no_qpi_en:        1;                 /*1: spi slave QPI mode is not supported. 0: spi slave QPI mode is supported.*/
            uint32_t addr_err:         1;                 /*1: The address value of the last SPI transfer is not supported by SPI slave. 0: The address value is supported or no address value is received.*/
            uint32_t cmd_err:          1;                 /*1: The command value of the last SPI transfer is not supported by SPI slave. 0: The command value is supported or no command value is received.*/
            uint32_t wr_dma_done:      1;                 /*The interrupt raw bit for the completion of dma write operation in the slave mode.  Can not be changed by CONF_buf.*/
            uint32_t last_command:     8;                 /*In the slave mode it is the value of command.*/
            uint32_t last_addr:        8;                 /*In the slave mode it is the value of address.*/
        };
        uint32_t val;
    } slave1;
    union {
        struct {
            uint32_t reserved0:       24;                 /*reserved*/
            uint32_t wr_buf_done:      1;                 /*The interrupt raw bit for the completion of write-buffer operation in the slave mode.  Can not be changed by CONF_buf.*/
            uint32_t conf_base_bitlen: 7;                 /*The basic spi_clk cycles of CONF state. The real cycle length of CONF state  if spi_usr_conf is enabled  is spi_conf_base_bitlen[6:0] + spi_conf_bitlen[23:0].*/
        };
        uint32_t val;
    } slv_wrbuf_dlen;
    union {
        struct {
            uint32_t dma_rd_bytelen:    20;               /*In the slave mode it is the length in bytes for read operations. The register value shall be byte_num.*/
            uint32_t reserved20:         4;               /*reserved*/
            uint32_t rd_buf_done:        1;               /*The interrupt raw bit for the completion of read-buffer operation in the slave mode.  Can not be changed by CONF_buf.*/
            uint32_t seg_magic_err:      1;               /*1: The recent magic value in CONF buffer is not right in master DMA seg-trans mode. 0: others.*/
            uint32_t reserved26:         6;               /*reserved*/
        };
        uint32_t val;
    } slv_rdbuf_dlen;
    union {
        struct {
            uint32_t data_bytelen:        20;             /*The full-duplex or half-duplex data byte length of the last SPI transfer in slave mode. In half-duplex mode  this value is controlled by bits [23:20].*/
            uint32_t rddma_bytelen_en:     1;             /*1: spi_slv_data_bytelen stores data byte length of master-read-slave data length in DMA controlled mode(Rd_DMA). 0: others*/
            uint32_t wrdma_bytelen_en:     1;             /*1: spi_slv_data_bytelen stores data byte length of master-write-to-slave data length in DMA controlled mode(Wr_DMA). 0: others*/
            uint32_t rdbuf_bytelen_en:     1;             /*1: spi_slv_data_bytelen stores data byte length of master-read-slave data length in CPU controlled mode(Rd_BUF). 0: others*/
            uint32_t wrbuf_bytelen_en:     1;             /*1: spi_slv_data_bytelen stores data byte length of master-write-to-slave data length in CPU controlled mode(Wr_BUF). 0: others*/
            uint32_t dma_seg_magic_value:  4;             /*The magic value of BM table in master DMA seg-trans.*/
            uint32_t reserved28:           2;             /*reserved*/
            uint32_t rd_dma_done:          1;             /*The interrupt raw bit for the completion of Rd-DMA operation in the slave mode.  Can not be changed by CONF_buf.*/
            uint32_t usr_conf:             1;             /*1: Enable the DMA CONF phase of current seg-trans operation  which means seg-trans will start. 0: This is not seg-trans mode.*/
        };
        uint32_t val;
    } slv_rd_byte;
    union {
        struct {
            uint32_t st:                 4;               /*The status of spi state machine. 0: idle state  1: preparation state  2: send command state  3: send data state  4: red data state  5:write data state  6: wait state  7: done state.*/
            uint32_t reserved4:          8;               /*reserved*/
            uint32_t mst_dma_rd_bytelen:20;               /*Define the master DMA read byte length in non seg-trans or seg-trans mode. Invalid when spi_rx_eof_en is 0. Can be configured in CONF state..*/
        };
        uint32_t val;
    } fsm;
    union {
        struct {
            uint32_t int_hold_ena:       2;               /*This register is for two SPI masters to share the same cs clock and data signals. The bits of one SPI are set  if the other SPI is busy  the SPI will be hold. 1(3): hold at  idle  phase 2: hold at  prepare  phase. Can be configured in CONF state.*/
            uint32_t hold_val:           1;               /*spi hold output value  which should be used with spi_hold_out_en. Can be configured in CONF state.*/
            uint32_t hold_out_en:        1;               /*Enable set spi output hold value to spi_hold_reg. It can be used to hold spi state machine with spi_ext_hold_en and other usr hold signals. Can be configured in CONF state.*/
            uint32_t hold_out_time:      3;               /*set the hold cycles of output spi_hold signal when spi_hold_out_en is enable. Can be configured in CONF state.*/
            uint32_t dma_seg_trans_done: 1;               /*1:  spi master DMA full-duplex/half-duplex seg-trans ends or slave half-duplex seg-trans ends. And data has been pushed to corresponding memory.  0:  seg-trans is not ended or not occurred.  Can not be changed by CONF_buf.*/
            uint32_t reserved8:         24;               /*reserved*/
        };
        uint32_t val;
    } hold;
    union {
        struct {
            uint32_t reserved0:               2;          /*reserved*/
            uint32_t in_rst:                  1;          /*The bit is used to reset in dma fsm and in data fifo pointer.*/
            uint32_t out_rst:                 1;          /*The bit is used to reset out dma fsm and out data fifo pointer.*/
            uint32_t ahbm_fifo_rst:           1;          /*Reset spi dma ahb master fifo pointer.*/
            uint32_t ahbm_rst:                1;          /*Reset spi dma ahb master.*/
            uint32_t in_loop_test:            1;          /*Set bit to test in link.*/
            uint32_t out_loop_test:           1;          /*Set bit to test out link.*/
            uint32_t out_auto_wrback:         1;          /*when the bit is set  DMA continue to use the next inlink node when the length of inlink is 0.*/
            uint32_t out_eof_mode:            1;          /*out eof flag generation mode . 1: when dma pop all data from fifo  0:when ahb push all data to fifo.*/
            uint32_t outdscr_burst_en:        1;          /*read descriptor use burst mode when read data for memory.*/
            uint32_t indscr_burst_en:         1;          /*read descriptor use burst mode when write data to memory.*/
            uint32_t out_data_burst_en:       1;          /*spi dma read data from memory in burst mode.*/
            uint32_t mem_trans_en:            1;
            uint32_t dma_rx_stop:             1;          /*spi dma read data stop  when in continue tx/rx mode.*/
            uint32_t dma_tx_stop:             1;          /*spi dma write data stop when in continue tx/rx mode.*/
            uint32_t dma_continue:            1;          /*spi dma continue tx/rx data.*/
            uint32_t last_seg_pop_clr:        1;          /*1: Clear spi_slv_seg_frt_pop_mask. 0 : others*/
            uint32_t dma_seg_trans_en:        1;          /*Enable dma segment transfer in spi dma half slave mode. 1: enable. 0: disable.*/
            uint32_t rx_seg_trans_clr_en:     1;          /*1: spi_dma_infifo_full_vld is cleared by spi slave cmd 5. 0: spi_dma_infifo_full_vld is cleared by spi_trans_done.*/
            uint32_t tx_seg_trans_clr_en:     1;          /*1: spi_dma_outfifo_empty_vld is cleared by spi slave cmd 6. 0: spi_dma_outfifo_empty_vld is cleared by spi_trans_done.*/
            uint32_t rx_eof_en:               1;          /*1: spi_dma_inlink_eof is set when the number of dma pushed data bytes is equal to the value of spi_slv/mst_dma_rd_bytelen[19:0] in spi dma transition.  0: spi_dma_inlink_eof is set by spi_trans_done in non-seg-trans or spi_dma_seg_trans_done in seg-trans.*/
            uint32_t infifo_full_clr:         1;          /*1:Clear spi_dma_infifo_full_vld. 0: Do not control it.*/
            uint32_t outfifo_empty_clr:       1;          /*1:Clear spi_dma_outfifo_empty_vld. 0: Do not control it.*/
            uint32_t reserved24:              2;          /*reserved*/
            uint32_t ext_mem_bk_size:         2;          /*Select the external memory block size.*/
            uint32_t dma_seg_trans_clr:       1;          /*1: End slave seg-trans  which acts as 0x05 command. 2 or more end seg-trans signals will induce error in DMA RX.*/
            uint32_t reserved29:              3;          /*reserved*/
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
            uint32_t inlink_dscr_empty:          1;       /*The enable bit for lack of enough inlink descriptors. Can be configured in CONF state.*/
            uint32_t outlink_dscr_error:         1;       /*The enable bit for outlink descriptor error. Can be configured in CONF state.*/
            uint32_t inlink_dscr_error:          1;       /*The enable bit for inlink descriptor error. Can be configured in CONF state.*/
            uint32_t in_done:                    1;       /*The enable bit for completing usage of a inlink descriptor. Can be configured in CONF state.*/
            uint32_t in_err_eof:                 1;       /*The enable bit for receiving error. Can be configured in CONF state.*/
            uint32_t in_suc_eof:                 1;       /*The enable bit for completing receiving all the packets from host. Can be configured in CONF state.*/
            uint32_t out_done:                   1;       /*The enable bit for completing usage of a outlink descriptor . Can be configured in CONF state.*/
            uint32_t out_eof:                    1;       /*The enable bit for sending a packet to host done. Can be configured in CONF state.*/
            uint32_t out_total_eof:              1;       /*The enable bit for sending all the packets to host done. Can be configured in CONF state.*/
            uint32_t infifo_full_err:            1;       /*The enable bit for infifo full error interrupt.*/
            uint32_t outfifo_empty_err:          1;       /*The enable bit for outfifo empty error interrupt.*/
            uint32_t cmd6:                       1;       /*The enable bit for SPI slave CMD6 interrupt.*/
            uint32_t cmd7:                       1;       /*The enable bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8:                       1;       /*The enable bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9:                       1;       /*The enable bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda:                       1;       /*The enable bit for SPI slave CMDA interrupt.*/
            uint32_t reserved16:                16;       /*reserved*/
        };
        uint32_t val;
    } dma_int_ena;
    union {
        struct {
            uint32_t inlink_dscr_empty:          1;       /*The raw bit for lack of enough inlink descriptors. Can be configured in CONF state.*/
            uint32_t outlink_dscr_error:         1;       /*The raw bit for outlink descriptor error. Can be configured in CONF state.*/
            uint32_t inlink_dscr_error:          1;       /*The raw bit for inlink descriptor error. Can be configured in CONF state.*/
            uint32_t in_done:                    1;       /*The raw bit for completing usage of a inlink descriptor. Can be configured in CONF state.*/
            uint32_t in_err_eof:                 1;       /*The raw bit for receiving error. Can be configured in CONF state.*/
            uint32_t in_suc_eof:                 1;       /*The raw bit for completing receiving all the packets from host. Can be configured in CONF state.*/
            uint32_t out_done:                   1;       /*The raw bit for completing usage of a outlink descriptor. Can be configured in CONF state.*/
            uint32_t out_eof:                    1;       /*The raw bit for sending a packet to host done. Can be configured in CONF state.*/
            uint32_t out_total_eof:              1;       /*The raw bit for sending all the packets to host done. Can be configured in CONF state.*/
            uint32_t infifo_full_err:            1;       /*1:spi_dma_infifo_full and spi_push_data_prep are valid  which means that DMA Rx buffer is full but push is valid.  0: Others.  Can not be changed by CONF_buf.*/
            uint32_t outfifo_empty_err:          1;       /*1:spi_dma_outfifo_empty and spi_pop_data_prep are valid  which means that there is no data to pop but pop is valid.  0: Others.  Can not be changed by CONF_buf.*/
            uint32_t cmd6:                       1;       /*The raw bit for SPI slave CMD6 interrupt.*/
            uint32_t cmd7:                       1;       /*The raw bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8:                       1;       /*The raw bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9:                       1;       /*The raw bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda:                       1;       /*The raw bit for SPI slave CMDA interrupt.*/
            uint32_t reserved16:                16;       /*reserved*/
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
            uint32_t infifo_full_err:           1;        /*The status bit for infifo full error.*/
            uint32_t outfifo_empty_err:         1;        /*The status bit for outfifo empty error.*/
            uint32_t cmd6:                      1;        /*The status bit for SPI slave CMD6 interrupt.*/
            uint32_t cmd7:                      1;        /*The status bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8:                      1;        /*The status bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9:                      1;        /*The status bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda:                      1;        /*The status bit for SPI slave CMDA interrupt.*/
            uint32_t reserved16:               16;        /*reserved*/
        };
        uint32_t val;
    } dma_int_st;
    union {
        struct {
            uint32_t inlink_dscr_empty:          1;       /*The clear bit for lack of enough inlink descriptors. Can be configured in CONF state.*/
            uint32_t outlink_dscr_error:         1;       /*The clear bit for outlink descriptor error. Can be configured in CONF state.*/
            uint32_t inlink_dscr_error:          1;       /*The clear bit for inlink descriptor error. Can be configured in CONF state.*/
            uint32_t in_done:                    1;       /*The clear bit for completing usage of a inlink descriptor. Can be configured in CONF state.*/
            uint32_t in_err_eof:                 1;       /*The clear bit for receiving error. Can be configured in CONF state.*/
            uint32_t in_suc_eof:                 1;       /*The clear bit for completing receiving all the packets from host. Can be configured in CONF state.*/
            uint32_t out_done:                   1;       /*The clear bit for completing usage of a outlink descriptor. Can be configured in CONF state.*/
            uint32_t out_eof:                    1;       /*The clear bit for sending a packet to host done. Can be configured in CONF state.*/
            uint32_t out_total_eof:              1;       /*The clear bit for sending all the packets to host done. Can be configured in CONF state.*/
            uint32_t infifo_full_err:            1;       /*1: Clear spi_dma_infifo_full_err. 0: not valid. Can be changed by CONF_buf.*/
            uint32_t outfifo_empty_err:          1;       /*1: Clear spi_dma_outfifo_empty_err signal. 0: not valid. Can be changed by CONF_buf.*/
            uint32_t cmd6:                       1;       /*The clear bit for SPI slave CMD6 interrupt.*/
            uint32_t cmd7:                       1;       /*The clear bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8:                       1;       /*The clear bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9:                       1;       /*The clear bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda:                       1;       /*The clear bit for SPI slave CMDA interrupt.*/
            uint32_t reserved16:                16;       /*reserved*/
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
            uint32_t din0_mode:      3;                   /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din1_mode:      3;                   /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din2_mode:      3;                   /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din3_mode:      3;                   /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din4_mode:      3;                   /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din5_mode:      3;                   /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din6_mode:      3;                   /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din7_mode:      3;                   /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t timing_clk_ena: 1;                   /*1:enable hclk in spi_timing.v.  0: disable it. Can be configured in CONF state.*/
            uint32_t reserved25:     7;                   /*reserved*/
        };
        uint32_t val;
    } din_mode;
    union {
        struct {
            uint32_t din0_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...  Can be configured in CONF state.*/
            uint32_t din1_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...  Can be configured in CONF state.*/
            uint32_t din2_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...  Can be configured in CONF state.*/
            uint32_t din3_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...  Can be configured in CONF state.*/
            uint32_t din4_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...  Can be configured in CONF state.*/
            uint32_t din5_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...  Can be configured in CONF state.*/
            uint32_t din6_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...  Can be configured in CONF state.*/
            uint32_t din7_num:   2;                       /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...  Can be configured in CONF state.*/
            uint32_t reserved16:16;                       /*reserved*/
        };
        uint32_t val;
    } din_num;
    union {
        struct {
            uint32_t dout0_mode: 3;                       /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t dout1_mode: 3;                       /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t dout2_mode: 3;                       /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t dout3_mode: 3;                       /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t dout4_mode: 3;                       /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t dout5_mode: 3;                       /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t dout6_mode: 3;                       /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t dout7_mode: 3;                       /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t reserved24: 8;                       /*reserved*/
        };
        uint32_t val;
    } dout_mode;
    union {
        struct {
            uint32_t dout0_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t dout1_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t dout2_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t dout3_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t dout4_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t dout5_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t dout6_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t dout7_num:  2;                       /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t reserved16:16;                       /*reserved*/
        };
        uint32_t val;
    } dout_num;
    union {
        struct {
            uint32_t lcd_hb_front: 11;                    /*It is the horizontal blank front porch of a frame. Can be configured in CONF state.*/
            uint32_t lcd_va_height:10;                    /*It is the vertical active height of a frame. Can be configured in CONF state.*/
            uint32_t lcd_vt_height:10;                    /*It is the vertical total height of a frame. Can be configured in CONF state.*/
            uint32_t lcd_mode_en:   1;                    /*1: Enable LCD mode output vsync  hsync  de. 0: Disable. Can be configured in CONF state.*/
        };
        uint32_t val;
    } lcd_ctrl;
    union {
        struct {
            uint32_t lcd_vb_front: 8;                     /*It is the vertical blank front porch of a frame. Can be configured in CONF state.*/
            uint32_t lcd_ha_width:12;                     /*It is the horizontal active width of a frame. Can be configured in CONF state.*/
            uint32_t lcd_ht_width:12;                     /*It is the horizontal total width of a frame. Can be configured in CONF state.*/
        };
        uint32_t val;
    } lcd_ctrl1;
    union {
        struct {
            uint32_t lcd_vsync_width:    7;               /*It is the position of spi_vsync active pulse in a line. Can be configured in CONF state.*/
            uint32_t vsync_idle_pol:     1;               /*It is the idle value of spi_vsync. Can be configured in CONF state.*/
            uint32_t reserved8:          8;               /*reserved*/
            uint32_t lcd_hsync_width:    7;               /*It is the position of spi_hsync active pulse in a line. Can be configured in CONF state.*/
            uint32_t hsync_idle_pol:     1;               /*It is the idle value of spi_hsync. Can be configured in CONF state.*/
            uint32_t lcd_hsync_position: 8;               /*It is the position of spi_hsync active pulse in a line. Can be configured in CONF state.*/
        };
        uint32_t val;
    } lcd_ctrl2;
    union {
        struct {
            uint32_t d_dqs_mode:   3;                     /*the output spi_dqs is delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t d_cd_mode:    3;                     /*the output spi_cd is delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t d_de_mode:    3;                     /*the output spi_de is delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t d_hsync_mode: 3;                     /*the output spi_hsync is delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t d_vsync_mode: 3;                     /*the output spi_vsync is delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk. Can be configured in CONF state.*/
            uint32_t de_idle_pol:  1;                     /*It is the idle value of spi_de.*/
            uint32_t hs_blank_en:  1;                     /*1: The pulse of spi_hsync is out in vertical blanking lines in seg-trans or one trans. 0: spi_hsync pulse is valid only in active region lines in seg-trans.*/
            uint32_t reserved17:  15;                     /*reserved*/
        };
        uint32_t val;
    } lcd_d_mode;
    union {
        struct {
            uint32_t d_dqs_num:   2;                      /*the output spi_dqs is delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t d_cd_num:    2;                      /*the output spi_cd is delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t d_de_num:    2;                      /*the output spi_de is delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t d_hsync_num: 2;                      /*the output spi_hsync is delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t d_vsync_num: 2;                      /*the output spi_vsync is delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ... Can be configured in CONF state.*/
            uint32_t reserved10: 22;                      /*reserved*/
        };
        uint32_t val;
    } lcd_d_num;
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

#ifndef __cplusplus
_Static_assert(sizeof(spi_dev_t)==0x400, "***invalid spi");
#endif

#ifdef __cplusplus
}
#endif
