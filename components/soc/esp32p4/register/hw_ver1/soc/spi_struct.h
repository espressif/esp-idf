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

/** Group: User-defined control registers */
/** Type of cmd register
 *  Command control register
 */
typedef union {
    struct {
        /** conf_bitlen : R/W; bitpos: [17:0]; default: 0;
         *  Define the APB cycles of  SPI_CONF state. Can be configured in CONF state.
         */
        uint32_t conf_bitlen:18;  //this field is only for GPSPI2
        uint32_t reserved_18:5;
        /** update : WT; bitpos: [23]; default: 0;
         *  Set this bit to synchronize SPI registers from APB clock domain into SPI module
         *  clock domain, which is only used in SPI master mode.
         */
        uint32_t update:1;
        /** usr : R/W/SC; bitpos: [24]; default: 0;
         *  User define command enable.  An operation will be triggered when the bit is set.
         *  The bit will be cleared once the operation done.1: enable 0: disable. Can not be
         *  changed by CONF_buf.
         */
        uint32_t usr:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} spi_cmd_reg_t;

/** Type of addr register
 *  Address value register
 */
typedef union {
    struct {
        /** usr_addr_value : R/W; bitpos: [31:0]; default: 0;
         *  Address to slave. Can be configured in CONF state.
         */
        uint32_t usr_addr_value:32;
    };
    uint32_t val;
} spi_addr_reg_t;

/** Type of user register
 *  SPI USER control register
 */
typedef union {
    struct {
        /** doutdin : R/W; bitpos: [0]; default: 0;
         *  Set the bit to enable full duplex communication. 1: enable 0: disable. Can be
         *  configured in CONF state.
         */
        uint32_t doutdin:1;
        uint32_t reserved_1:2;
        /** qpi_mode : R/W/SS/SC; bitpos: [3]; default: 0;
         *  Both for master mode and slave mode. 1: spi controller is in QPI mode. 0: others.
         *  Can be configured in CONF state.
         */
        uint32_t qpi_mode:1;
        /** opi_mode : R/W; bitpos: [4]; default: 0;
         *  Just for master mode. 1: spi controller is in OPI mode (all in 8-b-m). 0: others.
         *  Can be configured in CONF state.
         */
        uint32_t opi_mode:1;  //this field is only for GPSPI2
        /** tsck_i_edge : R/W; bitpos: [5]; default: 0;
         *  In the slave mode, this bit can be used to change the polarity of tsck. 0: tsck =
         *  spi_ck_i. 1:tsck = !spi_ck_i.
         */
        uint32_t tsck_i_edge:1;
        /** cs_hold : R/W; bitpos: [6]; default: 1;
         *  spi cs keep low when spi is in  done  phase. 1: enable 0: disable. Can be
         *  configured in CONF state.
         */
        uint32_t cs_hold:1;
        /** cs_setup : R/W; bitpos: [7]; default: 1;
         *  spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable. Can be
         *  configured in CONF state.
         */
        uint32_t cs_setup:1;
        /** rsck_i_edge : R/W; bitpos: [8]; default: 0;
         *  In the slave mode, this bit can be used to change the polarity of rsck. 0: rsck =
         *  !spi_ck_i. 1:rsck = spi_ck_i.
         */
        uint32_t rsck_i_edge:1;
        /** ck_out_edge : R/W; bitpos: [9]; default: 0;
         *  the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode. Can
         *  be configured in CONF state.
         */
        uint32_t ck_out_edge:1;
        uint32_t reserved_10:2;
        /** fwrite_dual : R/W; bitpos: [12]; default: 0;
         *  In the write operations read-data phase apply 2 signals. Can be configured in CONF
         *  state.
         */
        uint32_t fwrite_dual:1;
        /** fwrite_quad : R/W; bitpos: [13]; default: 0;
         *  In the write operations read-data phase apply 4 signals. Can be configured in CONF
         *  state.
         */
        uint32_t fwrite_quad:1;
        /** fwrite_oct : R/W; bitpos: [14]; default: 0;
         *  In the write operations read-data phase apply 8 signals. Can be configured in CONF
         *  state.
         */
        uint32_t fwrite_oct:1;  //this field is only for GPSPI2
        /** usr_conf_nxt : R/W; bitpos: [15]; default: 0;
         *  1: Enable the DMA CONF phase of next seg-trans operation, which means seg-trans
         *  will continue. 0: The seg-trans will end after the current SPI seg-trans or this is
         *  not seg-trans mode. Can be configured in CONF state.
         */
        uint32_t usr_conf_nxt:1;  //this field is only for GPSPI2
        uint32_t reserved_16:1;
        /** sio : R/W; bitpos: [17]; default: 0;
         *  Set the bit to enable 3-line half duplex communication mosi and miso signals share
         *  the same pin. 1: enable 0: disable. Can be configured in CONF state.
         */
        uint32_t sio:1;
        uint32_t reserved_18:6;
        /** usr_miso_highpart : R/W; bitpos: [24]; default: 0;
         *  read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0:
         *  disable. Can be configured in CONF state.
         */
        uint32_t usr_miso_highpart:1;
        /** usr_mosi_highpart : R/W; bitpos: [25]; default: 0;
         *  write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable
         *  0: disable.  Can be configured in CONF state.
         */
        uint32_t usr_mosi_highpart:1;
        /** usr_dummy_idle : R/W; bitpos: [26]; default: 0;
         *  spi clock is disable in dummy phase when the bit is enable. Can be configured in
         *  CONF state.
         */
        uint32_t usr_dummy_idle:1;
        /** usr_mosi : R/W; bitpos: [27]; default: 0;
         *  This bit enable the write-data phase of an operation. Can be configured in CONF
         *  state.
         */
        uint32_t usr_mosi:1;
        /** usr_miso : R/W; bitpos: [28]; default: 0;
         *  This bit enable the read-data phase of an operation. Can be configured in CONF
         *  state.
         */
        uint32_t usr_miso:1;
        /** usr_dummy : R/W; bitpos: [29]; default: 0;
         *  This bit enable the dummy phase of an operation. Can be configured in CONF state.
         */
        uint32_t usr_dummy:1;
        /** usr_addr : R/W; bitpos: [30]; default: 0;
         *  This bit enable the address phase of an operation. Can be configured in CONF state.
         */
        uint32_t usr_addr:1;
        /** usr_command : R/W; bitpos: [31]; default: 1;
         *  This bit enable the command phase of an operation. Can be configured in CONF state.
         */
        uint32_t usr_command:1;
    };
    uint32_t val;
} spi_user_reg_t;

/** Type of user1 register
 *  SPI USER control register 1
 */
typedef union {
    struct {
        /** usr_dummy_cyclelen : R/W; bitpos: [7:0]; default: 7;
         *  The length in spi_clk cycles of dummy phase. The register value shall be
         *  (cycle_num-1). Can be configured in CONF state.
         */
        uint32_t usr_dummy_cyclelen:8;
        uint32_t reserved_8:8;
        /** mst_wfull_err_end_en : R/W; bitpos: [16]; default: 1;
         *  1: SPI transfer is ended when SPI RX AFIFO wfull error is valid in GP-SPI master
         *  FD/HD-mode. 0: SPI transfer is not ended when SPI RX AFIFO wfull error is valid in
         *  GP-SPI master FD/HD-mode.
         */
        uint32_t mst_wfull_err_end_en:1;
        /** cs_setup_time : R/W; bitpos: [21:17]; default: 0;
         *  (cycles+1) of prepare phase by spi clock this bits are combined with spi_cs_setup
         *  bit. Can be configured in CONF state.
         */
        uint32_t cs_setup_time:5;
        /** cs_hold_time : R/W; bitpos: [26:22]; default: 1;
         *  delay cycles of cs pin by spi clock this bits are combined with spi_cs_hold bit.
         *  Can be configured in CONF state.
         */
        uint32_t cs_hold_time:5;
        /** usr_addr_bitlen : R/W; bitpos: [31:27]; default: 23;
         *  The length in bits of address phase. The register value shall be (bit_num-1). Can
         *  be configured in CONF state.
         */
        uint32_t usr_addr_bitlen:5;
    };
    uint32_t val;
} spi_user1_reg_t;

/** Type of user2 register
 *  SPI USER control register 2
 */
typedef union {
    struct {
        /** usr_command_value : R/W; bitpos: [15:0]; default: 0;
         *  The value of  command. Can be configured in CONF state.
         */
        uint32_t usr_command_value:16;
        uint32_t reserved_16:11;
        /** mst_rempty_err_end_en : R/W; bitpos: [27]; default: 1;
         *  1: SPI transfer is ended when SPI TX AFIFO read empty error is valid in GP-SPI
         *  master FD/HD-mode. 0: SPI transfer is not ended when SPI TX AFIFO read empty error
         *  is valid in GP-SPI master FD/HD-mode.
         */
        uint32_t mst_rempty_err_end_en:1;
        /** usr_command_bitlen : R/W; bitpos: [31:28]; default: 7;
         *  The length in bits of command phase. The register value shall be (bit_num-1). Can
         *  be configured in CONF state.
         */
        uint32_t usr_command_bitlen:4;
    };
    uint32_t val;
} spi_user2_reg_t;


/** Group: Control and configuration registers */
/** Type of ctrl register
 *  SPI control register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** dummy_out : R/W; bitpos: [3]; default: 0;
         *  0: In the dummy phase, the FSPI bus signals are not output. 1: In the dummy phase,
         *  the FSPI bus signals are output. Can be configured in CONF state.
         */
        uint32_t dummy_out:1;
        uint32_t reserved_4:1;
        /** faddr_dual : R/W; bitpos: [5]; default: 0;
         *  Apply 2 signals during addr phase 1:enable 0: disable. Can be configured in CONF
         *  state.
         */
        uint32_t faddr_dual:1;
        /** faddr_quad : R/W; bitpos: [6]; default: 0;
         *  Apply 4 signals during addr phase 1:enable 0: disable. Can be configured in CONF
         *  state.
         */
        uint32_t faddr_quad:1;
        /** faddr_oct : R/W; bitpos: [7]; default: 0;
         *  Apply 8 signals during addr phase 1:enable 0: disable. Can be configured in CONF
         *  state.
         */
        uint32_t faddr_oct:1;  //this field is only for GPSPI2
        /** fcmd_dual : R/W; bitpos: [8]; default: 0;
         *  Apply 2 signals during command phase 1:enable 0: disable. Can be configured in CONF
         *  state.
         */
        uint32_t fcmd_dual:1;
        /** fcmd_quad : R/W; bitpos: [9]; default: 0;
         *  Apply 4 signals during command phase 1:enable 0: disable. Can be configured in CONF
         *  state.
         */
        uint32_t fcmd_quad:1;
        /** fcmd_oct : R/W; bitpos: [10]; default: 0;
         *  Apply 8 signals during command phase 1:enable 0: disable. Can be configured in CONF
         *  state.
         */
        uint32_t fcmd_oct:1;  //this field is only for GPSPI2
        uint32_t reserved_11:3;
        /** fread_dual : R/W; bitpos: [14]; default: 0;
         *  In the read operations, read-data phase apply 2 signals. 1: enable 0: disable. Can
         *  be configured in CONF state.
         */
        uint32_t fread_dual:1;
        /** fread_quad : R/W; bitpos: [15]; default: 0;
         *  In the read operations read-data phase apply 4 signals. 1: enable 0: disable.  Can
         *  be configured in CONF state.
         */
        uint32_t fread_quad:1;
        /** fread_oct : R/W; bitpos: [16]; default: 0;
         *  In the read operations read-data phase apply 8 signals. 1: enable 0: disable.  Can
         *  be configured in CONF state.
         */
        uint32_t fread_oct:1;  //this field is only for GPSPI2
        uint32_t reserved_17:1;
        /** q_pol : R/W; bitpos: [18]; default: 1;
         *  The bit is used to set MISO line polarity, 1: high 0, low. Can be configured in
         *  CONF state.
         */
        uint32_t q_pol:1;
        /** d_pol : R/W; bitpos: [19]; default: 1;
         *  The bit is used to set MOSI line polarity, 1: high 0, low. Can be configured in
         *  CONF state.
         */
        uint32_t d_pol:1;
        /** hold_pol : R/W; bitpos: [20]; default: 1;
         *  SPI_HOLD output value when SPI is idle. 1: output high, 0: output low. Can be
         *  configured in CONF state.
         */
        uint32_t hold_pol:1;
        /** wp_pol : R/W; bitpos: [21]; default: 1;
         *  Write protect signal output when SPI is idle.  1: output high, 0: output low.  Can
         *  be configured in CONF state.
         */
        uint32_t wp_pol:1;
        uint32_t reserved_22:1;
        /** rd_bit_order : R/W; bitpos: [24:23]; default: 0;
         *  In read-data (MISO) phase 1: LSB first 0: MSB first. Can be configured in CONF
         *  state.
         */
        uint32_t rd_bit_order:2;
        /** wr_bit_order : R/W; bitpos: [26:25]; default: 0;
         *  In command address write-data (MOSI) phases 1: LSB firs 0: MSB first. Can be
         *  configured in CONF state.
         */
        uint32_t wr_bit_order:2;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_ctrl_reg_t;

/** Type of ms_dlen register
 *  SPI data bit length control register
 */
typedef union {
    struct {
        /** ms_data_bitlen : R/W; bitpos: [17:0]; default: 0;
         *  The value of these bits is the configured SPI transmission data bit length in
         *  master mode DMA controlled transfer or CPU controlled transfer. The value is also
         *  the configured bit length in slave mode DMA RX controlled transfer. The register
         *  value shall be (bit_num-1). Can be configured in CONF state.
         */
        uint32_t ms_data_bitlen:18;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} spi_ms_dlen_reg_t;

/** Type of misc register
 *  SPI misc register
 */
typedef union {
    struct {
        /** cs0_dis : R/W; bitpos: [0]; default: 0;
         *  SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can
         *  be configured in CONF state.
         */
        uint32_t cs0_dis:1;
        /** cs1_dis : R/W; bitpos: [1]; default: 1;
         *  SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can
         *  be configured in CONF state.
         */
        uint32_t cs1_dis:1;
        /** cs2_dis : R/W; bitpos: [2]; default: 1;
         *  SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can
         *  be configured in CONF state.
         */
        uint32_t cs2_dis:1;
        /** cs3_dis : R/W; bitpos: [3]; default: 1;
         *  SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can
         *  be configured in CONF state.
         */
        uint32_t cs3_dis:1;  //this field is only for GPSPI2
        /** cs4_dis : R/W; bitpos: [4]; default: 1;
         *  SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can
         *  be configured in CONF state.
         */
        uint32_t cs4_dis:1;  //this field is only for GPSPI2
        /** cs5_dis : R/W; bitpos: [5]; default: 1;
         *  SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can
         *  be configured in CONF state.
         */
        uint32_t cs5_dis:1;  //this field is only for GPSPI2
        /** ck_dis : R/W; bitpos: [6]; default: 0;
         *  1: spi clk out disable,  0: spi clk out enable. Can be configured in CONF state.
         */
        uint32_t ck_dis:1;
        /** master_cs_pol : R/W; bitpos: [12:7]; default: 0;
         *  In the master mode the bits are the polarity of spi cs line, the value is
         *  equivalent to spi_cs ^ spi_master_cs_pol. Can be configured in CONF state.
         */
        uint32_t master_cs_pol:6;  //This field for GPSPI3 is only 3-bit-width
        uint32_t reserved_13:3;
        /** clk_data_dtr_en : R/W; bitpos: [16]; default: 0;
         *  1: SPI master DTR mode is applied to SPI clk, data and spi_dqs.  0: SPI master DTR
         *  mode is  only applied to spi_dqs. This bit should be used with bit 17/18/19.
         */
        uint32_t clk_data_dtr_en:1;  //this field is only for GPSPI2
        /** data_dtr_en : R/W; bitpos: [17]; default: 0;
         *  1: SPI clk and data of SPI_DOUT and SPI_DIN state are in DTR mode, including master
         *  1/2/4/8-bm.  0:  SPI clk and data of SPI_DOUT and SPI_DIN state are in STR mode.
         *  Can be configured in CONF state.
         */
        uint32_t data_dtr_en:1;  //this field is only for GPSPI2
        /** addr_dtr_en : R/W; bitpos: [18]; default: 0;
         *  1: SPI clk and data of SPI_SEND_ADDR state are in DTR mode, including master
         *  1/2/4/8-bm.  0:  SPI clk and data of SPI_SEND_ADDR state are in STR mode. Can be
         *  configured in CONF state.
         */
        uint32_t addr_dtr_en:1;  //this field is only for GPSPI2
        /** cmd_dtr_en : R/W; bitpos: [19]; default: 0;
         *  1: SPI clk and data of SPI_SEND_CMD state are in DTR mode, including master
         *  1/2/4/8-bm. 0:  SPI clk and data of SPI_SEND_CMD state are in STR mode. Can be
         *  configured in CONF state.
         */
        uint32_t cmd_dtr_en:1;  //this field is only for GPSPI2
        uint32_t reserved_20:3;
        /** slave_cs_pol : R/W; bitpos: [23]; default: 0;
         *  spi slave input cs polarity select. 1: inv  0: not change. Can be configured in
         *  CONF state.
         */
        uint32_t slave_cs_pol:1;
        /** dqs_idle_edge : R/W; bitpos: [24]; default: 0;
         *  The default value of spi_dqs. Can be configured in CONF state.
         */
        uint32_t dqs_idle_edge:1;  //this field is only for GPSPI2
        uint32_t reserved_25:4;
        /** ck_idle_edge : R/W; bitpos: [29]; default: 0;
         *  1: spi clk line is high when idle     0: spi clk line is low when idle. Can be
         *  configured in CONF state.
         */
        uint32_t ck_idle_edge:1;
        /** cs_keep_active : R/W; bitpos: [30]; default: 0;
         *  spi cs line keep low when the bit is set. Can be configured in CONF state.
         */
        uint32_t cs_keep_active:1;
        /** quad_din_pin_swap : R/W; bitpos: [31]; default: 0;
         *  1: SPI quad input swap enable, swap FSPID with FSPIQ, swap FSPIWP with FSPIHD. 0:
         *  spi quad input swap disable. Can be configured in CONF state.
         */
        uint32_t quad_din_pin_swap:1;
    };
    uint32_t val;
} spi_misc_reg_t;

/** Type of dma_conf register
 *  SPI DMA control register
 */
typedef union {
    struct {
        /** dma_outfifo_empty : RO; bitpos: [0]; default: 1;
         *  Records the status of DMA TX FIFO. 1: DMA TX FIFO is not ready for sending data. 0:
         *  DMA TX FIFO is ready for sending data.
         */
        uint32_t dma_outfifo_empty:1;
        /** dma_infifo_full : RO; bitpos: [1]; default: 1;
         *  Records the status of DMA RX FIFO. 1: DMA RX FIFO is not ready for receiving data.
         *  0: DMA RX FIFO is ready for receiving data.
         */
        uint32_t dma_infifo_full:1;
        uint32_t reserved_2:16;
        /** dma_slv_seg_trans_en : R/W; bitpos: [18]; default: 0;
         *  Enable dma segment transfer in spi dma half slave mode. 1: enable. 0: disable.
         */
        uint32_t dma_slv_seg_trans_en:1;
        /** slv_rx_seg_trans_clr_en : R/W; bitpos: [19]; default: 0;
         *  1: spi_dma_infifo_full_vld is cleared by spi slave cmd 5. 0:
         *  spi_dma_infifo_full_vld is cleared by spi_trans_done.
         */
        uint32_t slv_rx_seg_trans_clr_en:1;
        /** slv_tx_seg_trans_clr_en : R/W; bitpos: [20]; default: 0;
         *  1: spi_dma_outfifo_empty_vld is cleared by spi slave cmd 6. 0:
         *  spi_dma_outfifo_empty_vld is cleared by spi_trans_done.
         */
        uint32_t slv_tx_seg_trans_clr_en:1;
        /** rx_eof_en : R/W; bitpos: [21]; default: 0;
         *  1: spi_dma_inlink_eof is set when the number of dma pushed data bytes is equal to
         *  the value of spi_slv/mst_dma_rd_bytelen[19:0] in spi dma transition.  0:
         *  spi_dma_inlink_eof is set by spi_trans_done in non-seg-trans or
         *  spi_dma_seg_trans_done in seg-trans.
         */
        uint32_t rx_eof_en:1;
        uint32_t reserved_22:5;
        /** dma_rx_ena : R/W; bitpos: [27]; default: 0;
         *  Set this bit to enable SPI DMA controlled receive data mode.
         */
        uint32_t dma_rx_ena:1;
        /** dma_tx_ena : R/W; bitpos: [28]; default: 0;
         *  Set this bit to enable SPI DMA controlled send data mode.
         */
        uint32_t dma_tx_ena:1;
        /** rx_afifo_rst : WT; bitpos: [29]; default: 0;
         *  Set this bit to reset RX AFIFO, which is used to receive data in SPI master and
         *  slave mode transfer.
         */
        uint32_t rx_afifo_rst:1;
        /** buf_afifo_rst : WT; bitpos: [30]; default: 0;
         *  Set this bit to reset BUF TX AFIFO, which is used send data out in SPI slave CPU
         *  controlled mode transfer and master mode transfer.
         */
        uint32_t buf_afifo_rst:1;
        /** dma_afifo_rst : WT; bitpos: [31]; default: 0;
         *  Set this bit to reset DMA TX AFIFO, which is used to send data out in SPI slave DMA
         *  controlled mode transfer.
         */
        uint32_t dma_afifo_rst:1;
    };
    uint32_t val;
} spi_dma_conf_reg_t;

/** Type of slave register
 *  SPI slave control register
 */
typedef union {
    struct {
        /** clk_mode : R/W; bitpos: [1:0]; default: 0;
         *  SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed
         *  one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3:
         *  SPI clock is always on. Can be configured in CONF state.
         */
        uint32_t clk_mode:2;
        /** clk_mode_13 : R/W; bitpos: [2]; default: 0;
         *  {CPOL, CPHA},1: support spi clk mode 1 and 3, first edge output data B[0]/B[7].  0:
         *  support spi clk mode 0 and 2, first edge output data B[1]/B[6].
         */
        uint32_t clk_mode_13:1;
        /** rsck_data_out : R/W; bitpos: [3]; default: 0;
         *  It saves half a cycle when tsck is the same as rsck. 1: output data at rsck posedge
         *  0: output data at tsck posedge
         */
        uint32_t rsck_data_out:1;
        uint32_t reserved_4:4;
        /** slv_rddma_bitlen_en : R/W; bitpos: [8]; default: 0;
         *  1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length in
         *  DMA controlled mode(Rd_DMA). 0: others
         */
        uint32_t slv_rddma_bitlen_en:1;
        /** slv_wrdma_bitlen_en : R/W; bitpos: [9]; default: 0;
         *  1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data length
         *  in DMA controlled mode(Wr_DMA). 0: others
         */
        uint32_t slv_wrdma_bitlen_en:1;
        /** slv_rdbuf_bitlen_en : R/W; bitpos: [10]; default: 0;
         *  1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length in
         *  CPU controlled mode(Rd_BUF). 0: others
         */
        uint32_t slv_rdbuf_bitlen_en:1;
        /** slv_wrbuf_bitlen_en : R/W; bitpos: [11]; default: 0;
         *  1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data length
         *  in CPU controlled mode(Wr_BUF). 0: others
         */
        uint32_t slv_wrbuf_bitlen_en:1;
        /** slv_last_byte_strb : R/SS; bitpos: [19:12]; default: 0;
         *  Represents the effective bit of the last received data byte in SPI slave FD and HD
         *  mode.
         */
        uint32_t slv_last_byte_strb:8;
        uint32_t reserved_20:2;
        /** dma_seg_magic_value : R/W; bitpos: [25:22]; default: 10;
         *  The magic value of BM table in master DMA seg-trans.
         */
        uint32_t dma_seg_magic_value:4;  //this field is only for GPSPI2
        /** slave_mode : R/W; bitpos: [26]; default: 0;
         *  Set SPI work mode. 1: slave mode 0: master mode.
         */
        uint32_t slave_mode:1;
        /** soft_reset : WT; bitpos: [27]; default: 0;
         *  Software reset enable, reset the spi clock line cs line and data lines. Can be
         *  configured in CONF state.
         */
        uint32_t soft_reset:1;
        /** usr_conf : R/W; bitpos: [28]; default: 0;
         *  1: Enable the DMA CONF phase of current seg-trans operation, which means seg-trans
         *  will start. 0: This is not seg-trans mode.
         */
        uint32_t usr_conf:1;  //this field is only for GPSPI2
        /** mst_fd_wait_dma_tx_data : R/W; bitpos: [29]; default: 0;
         *  In master full-duplex mode, 1: GP-SPI will wait DMA TX data is ready before
         *  starting SPI transfer. 0: GP-SPI does not wait DMA TX data before starting SPI
         *  transfer.
         */
        uint32_t mst_fd_wait_dma_tx_data:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} spi_slave_reg_t;

/** Type of slave1 register
 *  SPI slave control register 1
 */
typedef union {
    struct {
        /** slv_data_bitlen : R/W/SS; bitpos: [17:0]; default: 0;
         *  The transferred data bit length in SPI slave FD and HD mode.
         */
        uint32_t slv_data_bitlen:18;
        /** slv_last_command : R/W/SS; bitpos: [25:18]; default: 0;
         *  In the slave mode it is the value of command.
         */
        uint32_t slv_last_command:8;
        /** slv_last_addr : R/W/SS; bitpos: [31:26]; default: 0;
         *  In the slave mode it is the value of address.
         */
        uint32_t slv_last_addr:6;
    };
    uint32_t val;
} spi_slave1_reg_t;


/** Group: Clock control registers */
/** Type of clock register
 *  SPI clock control register
 */
typedef union {
    struct {
        /** clkcnt_l : R/W; bitpos: [5:0]; default: 3;
         *  In the master mode it must be equal to spi_clkcnt_N. In the slave mode it must be
         *  0. Can be configured in CONF state.
         */
        uint32_t clkcnt_l:6;
        /** clkcnt_h : R/W; bitpos: [11:6]; default: 1;
         *  In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In the slave mode it
         *  must be 0. Can be configured in CONF state.
         */
        uint32_t clkcnt_h:6;
        /** clkcnt_n : R/W; bitpos: [17:12]; default: 3;
         *  In the master mode it is the divider of spi_clk. So spi_clk frequency is
         *  system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1). Can be configured in CONF state.
         */
        uint32_t clkcnt_n:6;
        /** clkdiv_pre : R/W; bitpos: [21:18]; default: 0;
         *  In the master mode it is pre-divider of spi_clk.  Can be configured in CONF state.
         */
        uint32_t clkdiv_pre:4;
        uint32_t reserved_22:9;
        /** clk_equ_sysclk : R/W; bitpos: [31]; default: 1;
         *  In the master mode 1: spi_clk is equal to system 0: spi_clk is divided from system
         *  clock. Can be configured in CONF state.
         */
        uint32_t clk_equ_sysclk:1;
    };
    uint32_t val;
} spi_clock_reg_t;

/** Type of clk_gate register
 *  SPI module clock and register clock control
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable clk gate
         */
        uint32_t clk_en:1;
        /** mst_clk_active : R/W; bitpos: [1]; default: 0;
         *  Set this bit to power on the SPI module clock.
         */
        uint32_t mst_clk_active:1;
        /** mst_clk_sel : R/W; bitpos: [2]; default: 0;
         *  This bit is used to select SPI module clock source in master mode. 1: PLL_CLK_80M.
         *  0: XTAL CLK.
         */
        uint32_t mst_clk_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} spi_clk_gate_reg_t;


/** Group: Timing registers */
/** Type of din_mode register
 *  SPI input delay mode configuration
 */
typedef union {
    struct {
        /** din0_mode : R/W; bitpos: [1:0]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din0_mode:2;
        /** din1_mode : R/W; bitpos: [3:2]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din1_mode:2;
        /** din2_mode : R/W; bitpos: [5:4]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din2_mode:2;
        /** din3_mode : R/W; bitpos: [7:6]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din3_mode:2;
        /** din4_mode : R/W; bitpos: [9:8]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din4_mode:2;  //this field is only for GPSPI2
        /** din5_mode : R/W; bitpos: [11:10]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din5_mode:2;  //this field is only for GPSPI2
        /** din6_mode : R/W; bitpos: [13:12]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din6_mode:2;  //this field is only for GPSPI2
        /** din7_mode : R/W; bitpos: [15:14]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din7_mode:2;  //this field is only for GPSPI2
        /** timing_hclk_active : R/W; bitpos: [16]; default: 0;
         *  1:enable hclk in SPI input timing module.  0: disable it. Can be configured in CONF
         *  state.
         */
        uint32_t timing_hclk_active:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} spi_din_mode_reg_t;

/** Type of din_num register
 *  SPI input delay number configuration
 */
typedef union {
    struct {
        /** din0_num : R/W; bitpos: [1:0]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din0_num:2;
        /** din1_num : R/W; bitpos: [3:2]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din1_num:2;
        /** din2_num : R/W; bitpos: [5:4]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din2_num:2;
        /** din3_num : R/W; bitpos: [7:6]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din3_num:2;
        /** din4_num : R/W; bitpos: [9:8]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din4_num:2;  //this field is only for GPSPI2
        /** din5_num : R/W; bitpos: [11:10]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din5_num:2;  //this field is only for GPSPI2
        /** din6_num : R/W; bitpos: [13:12]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din6_num:2;  //this field is only for GPSPI2
        /** din7_num : R/W; bitpos: [15:14]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din7_num:2;  //this field is only for GPSPI2
        uint32_t reserved_16:16;
    };
    uint32_t val;
} spi_din_num_reg_t;

/** Type of dout_mode register
 *  SPI output delay mode configuration
 */
typedef union {
    struct {
        /** dout0_mode : R/W; bitpos: [0]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout0_mode:1;
        /** dout1_mode : R/W; bitpos: [1]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout1_mode:1;
        /** dout2_mode : R/W; bitpos: [2]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout2_mode:1;
        /** dout3_mode : R/W; bitpos: [3]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout3_mode:1;
        /** dout4_mode : R/W; bitpos: [4]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout4_mode:1;  //this field is only for GPSPI2
        /** dout5_mode : R/W; bitpos: [5]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout5_mode:1;  //this field is only for GPSPI2
        /** dout6_mode : R/W; bitpos: [6]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout6_mode:1;  //this field is only for GPSPI2
        /** dout7_mode : R/W; bitpos: [7]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout7_mode:1;  //this field is only for GPSPI2
        /** d_dqs_mode : R/W; bitpos: [8]; default: 0;
         *  The output signal SPI_DQS is delayed by the SPI module clock, 0: output without
         *  delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t d_dqs_mode:1;  //this field is only for GPSPI2
        uint32_t reserved_9:23;
    };
    uint32_t val;
} spi_dout_mode_reg_t;

/** Type of dma_int register
 *  SPI interrupt raw/ena/clr/sta/set register
 */
typedef union {
    struct {
        /** dma_infifo_full_err_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  1: The current data rate of DMA Rx is smaller than that of SPI, which will lose the
         *  receive data.  0: Others.
         */
        uint32_t dma_infifo_full_err_int:1;
        /** dma_outfifo_empty_err_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  1: The current data rate of DMA TX is smaller than that of SPI. SPI will stop in
         *  master mode and send out all 0 in slave mode.  0: Others.
         */
        uint32_t dma_outfifo_empty_err_int:1;
        /** slv_ex_qpi_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw bit for SPI slave Ex_QPI interrupt. 1: SPI slave mode Ex_QPI transmission
         *  is ended. 0: Others.
         */
        uint32_t slv_ex_qpi_int:1;
        /** slv_en_qpi_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw bit for SPI slave En_QPI interrupt. 1: SPI slave mode En_QPI transmission
         *  is ended. 0: Others.
         */
        uint32_t slv_en_qpi_int:1;
        /** slv_cmd7_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit for SPI slave CMD7 interrupt. 1: SPI slave mode CMD7 transmission is
         *  ended. 0: Others.
         */
        uint32_t slv_cmd7_int:1;
        /** slv_cmd8_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw bit for SPI slave CMD8 interrupt. 1: SPI slave mode CMD8 transmission is
         *  ended. 0: Others.
         */
        uint32_t slv_cmd8_int:1;
        /** slv_cmd9_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw bit for SPI slave CMD9 interrupt. 1: SPI slave mode CMD9 transmission is
         *  ended. 0: Others.
         */
        uint32_t slv_cmd9_int:1;
        /** slv_cmda_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw bit for SPI slave CMDA interrupt. 1: SPI slave mode CMDA transmission is
         *  ended. 0: Others.
         */
        uint32_t slv_cmda_int:1;
        /** slv_rd_dma_done_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw bit for SPI_SLV_RD_DMA_DONE_INT interrupt. 1: SPI slave mode Rd_DMA
         *  transmission is ended. 0: Others.
         */
        uint32_t slv_rd_dma_done_int:1;
        /** slv_wr_dma_done_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw bit for SPI_SLV_WR_DMA_DONE_INT interrupt. 1: SPI slave mode Wr_DMA
         *  transmission is ended. 0: Others.
         */
        uint32_t slv_wr_dma_done_int:1;
        /** slv_rd_buf_done_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw bit for SPI_SLV_RD_BUF_DONE_INT interrupt. 1: SPI slave mode Rd_BUF
         *  transmission is ended. 0: Others.
         */
        uint32_t slv_rd_buf_done_int:1;
        /** slv_wr_buf_done_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw bit for SPI_SLV_WR_BUF_DONE_INT interrupt. 1: SPI slave mode Wr_BUF
         *  transmission is ended. 0: Others.
         */
        uint32_t slv_wr_buf_done_int:1;
        /** trans_done_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw bit for SPI_TRANS_DONE_INT interrupt. 1: SPI master mode transmission is
         *  ended. 0: others.
         */
        uint32_t trans_done_int:1;
        /** dma_seg_trans_done_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt. 1:  spi master DMA
         *  full-duplex/half-duplex seg-conf-trans ends or slave half-duplex seg-trans ends.
         *  And data has been pushed to corresponding memory.  0:  seg-conf-trans or seg-trans
         *  is not ended or not occurred.
         */
        uint32_t dma_seg_trans_done_int:1;
        /** seg_magic_err_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The raw bit for SPI_SEG_MAGIC_ERR_INT interrupt. 1: The magic value in CONF buffer
         *  is error in the DMA seg-conf-trans. 0: others.
         */
        uint32_t seg_magic_err_int_raw:1;  //this field is only forPI2
        /** slv_buf_addr_err_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  The raw bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt. 1: The accessing data address
         *  of the current SPI slave mode CPU controlled FD, Wr_BUF or Rd_BUF transmission is
         *  bigger than 63. 0: Others.
         */
        uint32_t slv_buf_addr_err_int:1;
        /** slv_cmd_err_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  The raw bit for SPI_SLV_CMD_ERR_INT interrupt. 1: The slave command value in the
         *  current SPI slave HD mode transmission is not supported. 0: Others.
         */
        uint32_t slv_cmd_err_int:1;
        /** mst_rx_afifo_wfull_err_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  The raw bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt. 1: There is a RX AFIFO
         *  write-full error when SPI inputs data in master mode. 0: Others.
         */
        uint32_t mst_rx_afifo_wfull_err_int:1;
        /** mst_tx_afifo_rempty_err_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  The raw bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt. 1: There is a TX BUF
         *  AFIFO read-empty error when SPI outputs data in master mode. 0: Others.
         */
        uint32_t mst_tx_afifo_rempty_err_int:1;
        /** app2_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  The raw bit for SPI_APP2_INT interrupt. The value is only controlled by software.
         */
        uint32_t app2_int:1;
        /** app1_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  The raw bit for SPI_APP1_INT interrupt. The value is only controlled by software.
         */
        uint32_t app1_int:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} spi_dma_int_reg_t;

/** Type of wn register
 *  SPI CPU-controlled buffer
 */
typedef union {
    struct {
        /** buf15 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf:32;
    };
    uint32_t val;
} spi_wn_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35680770;
         *  SPI register version.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} spi_date_reg_t;


typedef struct {
    volatile spi_cmd_reg_t cmd;
    volatile spi_addr_reg_t addr;
    volatile spi_ctrl_reg_t ctrl;
    volatile spi_clock_reg_t clock;
    volatile spi_user_reg_t user;
    volatile spi_user1_reg_t user1;
    volatile spi_user2_reg_t user2;
    volatile spi_ms_dlen_reg_t ms_dlen;
    volatile spi_misc_reg_t misc;
    volatile spi_din_mode_reg_t din_mode;
    volatile spi_din_num_reg_t din_num;
    volatile spi_dout_mode_reg_t dout_mode;
    volatile spi_dma_conf_reg_t dma_conf;
    volatile spi_dma_int_reg_t dma_int_ena;
    volatile spi_dma_int_reg_t dma_int_clr;
    volatile spi_dma_int_reg_t dma_int_raw;
    volatile spi_dma_int_reg_t dma_int_sta;
    volatile spi_dma_int_reg_t dma_int_set;
    uint32_t reserved_048[20];
    volatile spi_wn_reg_t data_buf[16];
    uint32_t reserved_0d8[2];
    volatile spi_slave_reg_t slave;
    volatile spi_slave1_reg_t slave1;
    volatile spi_clk_gate_reg_t clk_gate;
    uint32_t reserved_0ec;
    volatile spi_date_reg_t date;
} spi_dev_t;

extern spi_dev_t GPSPI2;
extern spi_dev_t GPSPI3;

#ifndef __cplusplus
_Static_assert(sizeof(spi_dev_t) == 0xf4, "Invalid size of spi_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
