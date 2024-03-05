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

/** Group: User-defined control registers */
/** Type of cmd register
 *  Command control register
 */
typedef union {
    struct {
        /** conf_bitlen : R/W; bitpos: [17:0]; default: 0;
         *  Configures the SPI_CLK cycles of SPI CONF state.
         *  Measurement unit: SPI_CLK clock cycle.\\
         *  Can be configured in CONF state.
         */
        uint32_t conf_bitlen:18;
        uint32_t reserved_18:5;
        /** update : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to synchronize SPI registers from APB clock domain into
         *  SPI module clock domain. \\
         *  0: Not synchronize \\
         *  1: Synchronize \\
         *  This bit is only used in SPI master transfer.
         */
        uint32_t update:1;
        /** usr : R/W/SC; bitpos: [24]; default: 0;
         *  Configures whether or not to enable user-defined command. \\
         *  0: Not enable \\
         *  1: Enable \\
         *  An SPI operation will be triggered when the bit is set. This bit will be cleared
         *  once the operation is done. Can not be changed by CONF_buf.
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
         *  Configures the address to slave.
         *  Can be configured in CONF state.
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
         *  Configures whether or not to enable full-duplex communication. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t doutdin:1;
        uint32_t reserved_1:2;
        /** qpi_mode : R/W/SS/SC; bitpos: [3]; default: 0;
         *  Configures whether or not to enable QPI mode. \\
         *  0: Disable \\
         *  1: Enable \\
         *  This configuration is applicable when the SPI controller works as master or slave.
         *  Can be configured in CONF state.
         */
        uint32_t qpi_mode:1;
        /** opi_mode : HRO; bitpos: [4]; default: 0;
         *  Just for master mode. 1: spi controller is in OPI mode (all in 8-b-m). 0: others.
         *  Can be configured in CONF state.
         */
        uint32_t opi_mode:1;
        /** tsck_i_edge : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to change the polarity of TSCK in slave transfer. \\
         *  0: TSCK = SPI_CK_I \\
         *  1: TSCK = !SPI_CK_I \\
         */
        uint32_t tsck_i_edge:1;
        /** cs_hold : R/W; bitpos: [6]; default: 1;
         *  Configures whether or not to keep SPI CS low when SPI is in DONE state. \\
         *  0: Not keep low \\
         *  1: Keep low \\
         *  Can be configured in CONF state.
         */
        uint32_t cs_hold:1;
        /** cs_setup : R/W; bitpos: [7]; default: 1;
         *  Configures whether or not to enable SPI CS when SPI is in prepare (PREP) state. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t cs_setup:1;
        /** rsck_i_edge : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to change the polarity of RSCK in slave transfer. \\
         *  0: RSCK = !SPI_CK_I \\
         *  1: RSCK = SPI_CK_I \\
         */
        uint32_t rsck_i_edge:1;
        /** ck_out_edge : R/W; bitpos: [9]; default: 0;
         *  Configures SPI clock mode together with SPI_CK_IDLE_EDGE.
         *  Can be configured in CONF state. For more information, see Section <a href="GP-SPI2
         *  master clock control">link</a>.
         */
        uint32_t ck_out_edge:1;
        uint32_t reserved_10:2;
        /** fwrite_dual : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable the 2-bit mode of read-data phase in write
         *  operations.\\
         *  0: Not enable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t fwrite_dual:1;
        /** fwrite_quad : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable the 4-bit mode of read-data phase in write
         *  operations. \\
         *  0: Not enable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t fwrite_quad:1;
        /** fwrite_oct : HRO; bitpos: [14]; default: 0;
         *  In the write operations read-data phase apply 8 signals. Can be configured in CONF
         *  state.
         */
        uint32_t fwrite_oct:1;
        /** usr_conf_nxt : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable the CONF state for the next transaction
         *  (segment) in a configurable segmented transfer. \\
         *  0: this transfer will end after the current transaction (segment) is finished. Or
         *  this is not a configurable segmented transfer. \\
         *  1: this configurable segmented transfer will continue its next transaction
         *  (segment). \\
         *  Can be configured in CONF state.
         */
        uint32_t usr_conf_nxt:1;
        uint32_t reserved_16:1;
        /** sio : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable 3-line half-duplex communication, where MOSI
         *  and MISO signals share the same pin.\\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t sio:1;
        uint32_t reserved_18:6;
        /** usr_miso_highpart : R/W; bitpos: [24]; default: 0;
         *  Configures whether or not to enable high part mode, i.e., only access to high part
         *  of the buffers: SPI_W8_REG ~ SPI_W15_REG in read-data phase. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t usr_miso_highpart:1;
        /** usr_mosi_highpart : R/W; bitpos: [25]; default: 0;
         *  Configures whether or not to enable high part mode, i.e., only access to high part
         *  of the buffers: SPI_W8_REG ~ SPI_W15_REG in write-data phase. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t usr_mosi_highpart:1;
        /** usr_dummy_idle : R/W; bitpos: [26]; default: 0;
         *  Configures whether or not to disable SPI clock in DUMMY state. \\
         *  0: Not disable \\
         *  1: Disable \\
         *  Can be configured in CONF state.
         */
        uint32_t usr_dummy_idle:1;
        /** usr_mosi : R/W; bitpos: [27]; default: 0;
         *  Configures whether or not to enable the write-data (DOUT) state of an operation. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t usr_mosi:1;
        /** usr_miso : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to enable the read-data (DIN) state of an operation. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t usr_miso:1;
        /** usr_dummy : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable the DUMMY state of an operation. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t usr_dummy:1;
        /** usr_addr : R/W; bitpos: [30]; default: 0;
         *  Configures whether or not to enable the address (ADDR) state of an operation. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t usr_addr:1;
        /** usr_command : R/W; bitpos: [31]; default: 1;
         *  Configures whether or not to enable the command (CMD) state of an operation. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
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
         *  Configures the length of DUMMY state.
         *  Measurement unit: SPI_CLK clock cycles.\\
         *  This value is (the expected cycle number - 1). Can be configured in CONF state.
         */
        uint32_t usr_dummy_cyclelen:8;
        uint32_t reserved_8:8;
        /** mst_wfull_err_end_en : R/W; bitpos: [16]; default: 1;
         *  Configures whether or not to end the SPI transfer when SPI RX AFIFO wfull error
         *  occurs in master full-/half-duplex transfers. \\
         *  0: Not end \\
         *  1: End \\
         */
        uint32_t mst_wfull_err_end_en:1;
        /** cs_setup_time : R/W; bitpos: [21:17]; default: 0;
         *  Configures the length of prepare (PREP) state.
         *  Measurement unit: SPI_CLK clock cycles.\\
         *  This value is equal to the expected cycles - 1. This field is used together with
         *  SPI_CS_SETUP. Can be configured in CONF state.
         */
        uint32_t cs_setup_time:5;
        /** cs_hold_time : R/W; bitpos: [26:22]; default: 1;
         *  Configures the delay cycles of CS pin.
         *  Measurement unit: SPI_CLK clock cycles. \\
         *  This field is used together with SPI_CS_HOLD. Can be configured in CONF state.
         */
        uint32_t cs_hold_time:5;
        /** usr_addr_bitlen : R/W; bitpos: [31:27]; default: 23;
         *  Configures the bit length in address state.
         *  This value is (expected bit number - 1). Can be configured in CONF state.
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
         *  Configures the command value.
         *  Can be configured in CONF state.
         */
        uint32_t usr_command_value:16;
        uint32_t reserved_16:11;
        /** mst_rempty_err_end_en : R/W; bitpos: [27]; default: 1;
         *  Configures whether or not to end the SPI transfer when SPI TX AFIFO read empty
         *  error occurs in master full-/half-duplex transfers. \\
         *  0: Not end \\
         *  1: End \\
         */
        uint32_t mst_rempty_err_end_en:1;
        /** usr_command_bitlen : R/W; bitpos: [31:28]; default: 7;
         *  Configures the bit length of command state.
         *  This value is (expected bit number - 1). Can be configured in CONF state.
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
         *  Configures whether or not to output the FSPI bus signals in DUMMY state. \\
         *  0: Not output \\
         *  1: Output \\
         *  Can be configured in CONF state.
         */
        uint32_t dummy_out:1;
        uint32_t reserved_4:1;
        /** faddr_dual : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable 2-bit mode during address (ADDR) state.\\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t faddr_dual:1;
        /** faddr_quad : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable 4-bit mode during address (ADDR) state. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t faddr_quad:1;
        /** faddr_oct : HRO; bitpos: [7]; default: 0;
         *  Configures whether or not to enable 8-bit mode during address (ADDR) state. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t faddr_oct:1;
        /** fcmd_dual : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable 2-bit mode during command (CMD) state. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t fcmd_dual:1;
        /** fcmd_quad : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable 4-bit mode during command (CMD) state. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t fcmd_quad:1;
        /** fcmd_oct : HRO; bitpos: [10]; default: 0;
         *  Configures whether or not to enable 8-bit mode during command (CMD) state. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t fcmd_oct:1;
        uint32_t reserved_11:3;
        /** fread_dual : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to enable the 2-bit mode of read-data (DIN) state in read
         *  operations. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t fread_dual:1;
        /** fread_quad : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable the 4-bit mode of read-data (DIN) state in read
         *  operations. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t fread_quad:1;
        /** fread_oct : HRO; bitpos: [16]; default: 0;
         *  Configures whether or not to enable the 8-bit mode of read-data (DIN) state in read
         *  operations. \\
         *  0: Disable \\
         *  1: Enable \\
         *  Can be configured in CONF state.
         */
        uint32_t fread_oct:1;
        uint32_t reserved_17:1;
        /** q_pol : R/W; bitpos: [18]; default: 1;
         *  Configures MISO line polarity. \\
         *  0: Low \\
         *  1: High \\
         *  Can be configured in CONF state.
         */
        uint32_t q_pol:1;
        /** d_pol : R/W; bitpos: [19]; default: 1;
         *  Configures MOSI line polarity. \\
         *  0: Low \\
         *  1: High \\
         *  Can be configured in CONF state.
         */
        uint32_t d_pol:1;
        /** hold_pol : R/W; bitpos: [20]; default: 1;
         *  Configures SPI_HOLD output value when SPI is in idle. \\
         *  0: Output low \\
         *  1: Output high \\
         *  Can be configured in CONF state.
         */
        uint32_t hold_pol:1;
        /** wp_pol : R/W; bitpos: [21]; default: 1;
         *  Configures the output value of write-protect signal when SPI is in idle. \\
         *  0: Output low \\
         *  1: Output high \\
         *  Can be configured in CONF state.
         */
        uint32_t wp_pol:1;
        uint32_t reserved_22:1;
        /** rd_bit_order : R/W; bitpos: [24:23]; default: 0;
         *  Configures the bit order in read-data (MISO) state. \\
         *  0: MSB first \\
         *  1: LSB first \\
         *  Can be configured in CONF state.
         */
        uint32_t rd_bit_order:2;
        /** wr_bit_order : R/W; bitpos: [26:25]; default: 0;
         *  Configures the bit order in command (CMD), address (ADDR), and write-data (MOSI)
         *  states. \\
         *  0: MSB first \\
         *  1: LSB first \\
         *  Can be configured in CONF state.
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
         *  Configures the data bit length of SPI transfer in DMA-controlled master transfer or
         *  in CPU-controlled master transfer. Or configures the bit length of SPI RX transfer
         *  in DMA-controlled slave transfer.
         *  This value shall be (expected bit_num - 1). Can be configured in CONF state.
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
         *  Configures whether or not to disable SPI_CS$n pin.\\
         *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
         *  1: Disable SPI_CS$n pin.\\
         *  Can be configured in CONF state.
         */
        uint32_t cs0_dis:1;
        /** cs1_dis : R/W; bitpos: [1]; default: 1;
         *  Configures whether or not to disable SPI_CS$n pin.\\
         *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
         *  1: Disable SPI_CS$n pin.\\
         *  Can be configured in CONF state.
         */
        uint32_t cs1_dis:1;
        /** cs2_dis : R/W; bitpos: [2]; default: 1;
         *  Configures whether or not to disable SPI_CS$n pin.\\
         *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
         *  1: Disable SPI_CS$n pin.\\
         *  Can be configured in CONF state.
         */
        uint32_t cs2_dis:1;
        /** cs3_dis : R/W; bitpos: [3]; default: 1;
         *  Configures whether or not to disable SPI_CS$n pin.\\
         *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
         *  1: Disable SPI_CS$n pin.\\
         *  Can be configured in CONF state.
         */
        uint32_t cs3_dis:1;
        /** cs4_dis : R/W; bitpos: [4]; default: 1;
         *  Configures whether or not to disable SPI_CS$n pin.\\
         *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
         *  1: Disable SPI_CS$n pin.\\
         *  Can be configured in CONF state.
         */
        uint32_t cs4_dis:1;
        /** cs5_dis : R/W; bitpos: [5]; default: 1;
         *  Configures whether or not to disable SPI_CS$n pin.\\
         *  0: SPI_CS$n signal is from/to SPI_CS$n pin.\\
         *  1: Disable SPI_CS$n pin.\\
         *  Can be configured in CONF state.
         */
        uint32_t cs5_dis:1;
        /** ck_dis : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to disable SPI_CLK output.\\
         *  0: Enable\\
         *  1: Disable\\
         *  Can be configured in CONF state.
         */
        uint32_t ck_dis:1;
        /** master_cs_pol : R/W; bitpos: [12:7]; default: 0;
         *  Configures the polarity of SPI_CS$n ($n = 0-5) line in master transfer.\\
         *  0: SPI_CS$n is low active.\\
         *  1: SPI_CS$n is high active.\\
         *  Can be configured in CONF state.
         */
        uint32_t master_cs_pol:6;
        uint32_t reserved_13:3;
        /** clk_data_dtr_en : HRO; bitpos: [16]; default: 0;
         *  1: SPI master DTR mode is applied to SPI clk, data and spi_dqs.  0: SPI master DTR
         *  mode is  only applied to spi_dqs. This bit should be used with bit 17/18/19.
         */
        uint32_t clk_data_dtr_en:1;
        /** data_dtr_en : HRO; bitpos: [17]; default: 0;
         *  1: SPI clk and data of SPI_DOUT and SPI_DIN state are in DTR mode, including master
         *  1/2/4/8-bm.  0:  SPI clk and data of SPI_DOUT and SPI_DIN state are in STR mode.
         *  Can be configured in CONF state.
         */
        uint32_t data_dtr_en:1;
        /** addr_dtr_en : HRO; bitpos: [18]; default: 0;
         *  1: SPI clk and data of SPI_SEND_ADDR state are in DTR mode, including master
         *  1/2/4/8-bm.  0:  SPI clk and data of SPI_SEND_ADDR state are in STR mode. Can be
         *  configured in CONF state.
         */
        uint32_t addr_dtr_en:1;
        /** cmd_dtr_en : HRO; bitpos: [19]; default: 0;
         *  1: SPI clk and data of SPI_SEND_CMD state are in DTR mode, including master
         *  1/2/4/8-bm. 0:  SPI clk and data of SPI_SEND_CMD state are in STR mode. Can be
         *  configured in CONF state.
         */
        uint32_t cmd_dtr_en:1;
        uint32_t reserved_20:3;
        /** slave_cs_pol : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not invert SPI slave input CS polarity.\\
         *  0: Not change\\
         *  1: Invert\\
         *  Can be configured in CONF state.
         */
        uint32_t slave_cs_pol:1;
        /** dqs_idle_edge : HRO; bitpos: [24]; default: 0;
         *  The default value of spi_dqs. Can be configured in CONF state.
         */
        uint32_t dqs_idle_edge:1;
        uint32_t reserved_25:4;
        /** ck_idle_edge : R/W; bitpos: [29]; default: 0;
         *  Configures the level of SPI_CLK line when GP-SPI2 is in idle.\\
         *  0: Low\\
         *  1: High\\
         *  Can be configured in CONF state.
         */
        uint32_t ck_idle_edge:1;
        /** cs_keep_active : R/W; bitpos: [30]; default: 0;
         *  Configures whether or not to keep the SPI_CS line low.\\
         *  0: Not keep low\\
         *  1: Keep low\\
         *  Can be configured in CONF state.
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
         *  Represents whether or not the DMA TX FIFO is ready for sending data.\\
         *  0: Ready\\
         *  1: Not ready\\
         */
        uint32_t dma_outfifo_empty:1;
        /** dma_infifo_full : RO; bitpos: [1]; default: 1;
         *  Represents whether or not the DMA RX FIFO is ready for receiving data.\\
         *  0: Ready\\
         *  1: Not ready\\
         */
        uint32_t dma_infifo_full:1;
        uint32_t reserved_2:16;
        /** dma_slv_seg_trans_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to enable DMA-controlled segmented transfer in slave
         *  half-duplex communication.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t dma_slv_seg_trans_en:1;
        /** slv_rx_seg_trans_clr_en : R/W; bitpos: [19]; default: 0;
         *  In slave segmented transfer, if the size of the DMA RX buffer is smaller than the
         *  size of the received data, \\1: the data in all the following Wr_DMA transactions
         *  will not be received\\ 0: the data in this Wr_DMA transaction will not be received,
         *  but in the following transactions,\\
         *
         *  - if the size of DMA RX buffer is not 0, the data in following Wr_DMA transactions
         *  will be received.
         *  - if the size of DMA RX buffer is 0, the data in following Wr_DMA transactions will
         *  not be received.
         */
        uint32_t slv_rx_seg_trans_clr_en:1;
        /** slv_tx_seg_trans_clr_en : R/W; bitpos: [20]; default: 0;
         *  In slave segmented transfer, if the size of the DMA TX buffer is smaller than the
         *  size of the transmitted data,\\
         *  1: the data in the following transactions will not be updated, i.e. the old data is
         *  transmitted repeatedly.\\
         *  0: the data in this transaction will not be updated. But in the following
         *  transactions,\\
         *
         *  - if new data is filled in DMA TX FIFO, new data will be transmitted.
         *  - if no new data is filled in DMA TX FIFO, no new data will be transmitted.
         */
        uint32_t slv_tx_seg_trans_clr_en:1;
        /** rx_eof_en : R/W; bitpos: [21]; default: 0;
         *  1: In a DAM-controlled transfer, if the bit number of transferred data is equal to
         *  (SPI_MS_DATA_BITLEN + 1), then GDMA_IN_SUC_EOF_CH\it{n}_INT_RAW will be set by
         *  hardware. 0: GDMA_IN_SUC_EOF_CH\it{n}_INT_RAW is set by SPI_TRANS_DONE_INT event in
         *  a single transfer, or by an SPI_DMA_SEG_TRANS_DONE_INT event in a segmented
         *  transfer.
         */
        uint32_t rx_eof_en:1;
        uint32_t reserved_22:5;
        /** dma_rx_ena : R/W; bitpos: [27]; default: 0;
         *  Configures whether or not to enable DMA-controlled receive data transfer.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t dma_rx_ena:1;
        /** dma_tx_ena : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to enable DMA-controlled send data transfer.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t dma_tx_ena:1;
        /** rx_afifo_rst : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to reset spi_rx_afifo as shown in Figure <a
         *  href="fig:spi-master-data-flow-control">link</a> and in Figure  <a
         *  href="fig:spi-slave-data-flow-control">link</a>.\\
         *  0: Not reset\\
         *  1: Reset\\
         *  spi_rx_afifo is used to receive data in SPI master and slave transfer.
         */
        uint32_t rx_afifo_rst:1;
        /** buf_afifo_rst : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to reset buf_tx_afifo as shown in Figure <a
         *  href="fig:spi-master-data-flow-control">link</a> and in Figure  <a
         *  href="fig:spi-slave-data-flow-control">link</a>.\\
         *  0: Not reset\\
         *  1: Reset\\
         *  buf_tx_afifo is used to send data out in CPU-controlled master and slave transfer.
         */
        uint32_t buf_afifo_rst:1;
        /** dma_afifo_rst : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to reset dma_tx_afifo as shown in Figure <a
         *  href="fig:spi-master-data-flow-control">link</a> and in Figure  <a
         *  href="fig:spi-slave-data-flow-control">link</a>.\\
         *  0: Not reset\\
         *  1: Reset\\
         *  dma_tx_afifo is used to send data out in DMA-controlled slave transfer.
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
         *  Configures SPI clock mode.\\
         *  0: SPI clock is off when CS becomes inactive.\\
         *  1: SPI clock is delayed one cycle after CS becomes inactive.\\
         *  2: SPI clock is delayed two cycles after CS becomes inactive.\\
         *  3: SPI clock is always on.\\
         *  Can be configured in CONF state.
         */
        uint32_t clk_mode:2;
        /** clk_mode_13 : R/W; bitpos: [2]; default: 0;
         *  Configure clock mode.\\
         *  0: Support SPI clock mode 0 or 2. See Table <a
         *  href="table:clock_pol_slave">link</a>.\\
         *  1: Support SPI clock mode 1 or 3. See Table <a
         *  href="table:clock_pol_slave">link</a>.\\
         */
        uint32_t clk_mode_13:1;
        /** rsck_data_out : R/W; bitpos: [3]; default: 0;
         *  Configures the edge of output data.\\
         *  0: Output data at TSCK rising edge.\\
         *  1: Output data at RSCK rising edge.\\
         */
        uint32_t rsck_data_out:1;
        uint32_t reserved_4:4;
        /** slv_rddma_bitlen_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to use SPI_SLV_DATA_BITLEN to store the data bit length
         *  of Rd_DMA transfer.\\
         *  0: Not use\\
         *  1: Use\\
         */
        uint32_t slv_rddma_bitlen_en:1;
        /** slv_wrdma_bitlen_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to use SPI_SLV_DATA_BITLEN to store the data bit length
         *  of Wr_DMA transfer.\\
         *  0: Not use\\
         *  1: Use\\
         */
        uint32_t slv_wrdma_bitlen_en:1;
        /** slv_rdbuf_bitlen_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to use SPI_SLV_DATA_BITLEN to store the data bit length
         *  of Rd_BUF transfer.\\
         *  0: Not use\\
         *  1: Use\\
         */
        uint32_t slv_rdbuf_bitlen_en:1;
        /** slv_wrbuf_bitlen_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to use SPI_SLV_DATA_BITLEN to store the data bit length
         *  of Wr_BUF transfer.\\
         *  0: Not use\\
         *  1: Use\\
         */
        uint32_t slv_wrbuf_bitlen_en:1;
        /** slv_last_byte_strb : R/SS; bitpos: [19:12]; default: 0;
         *  Represents the effective bit of the last received data byte in SPI slave FD and HD
         *  mode.
         */
        uint32_t slv_last_byte_strb:8;
        uint32_t reserved_20:2;
        /** dma_seg_magic_value : R/W; bitpos: [25:22]; default: 10;
         *  Configures the magic value of BM table in DMA-controlled configurable segmented
         *  transfer.
         */
        uint32_t dma_seg_magic_value:4;
        /** slave_mode : R/W; bitpos: [26]; default: 0;
         *  Configures SPI work mode.\\
         *  0: Master\\
         *  1: Slave\\
         */
        uint32_t slave_mode:1;
        /** soft_reset : WT; bitpos: [27]; default: 0;
         *  Configures whether to reset the SPI clock line, CS line, and data line via
         *  software.\\
         *  0: Not reset\\
         *  1: Reset\\
         *  Can be configured in CONF state.
         */
        uint32_t soft_reset:1;
        /** usr_conf : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to enable the CONF state of current DMA-controlled
         *  configurable segmented transfer.\\
         *  0: No effect, which means the current transfer is not a configurable segmented
         *  transfer.\\
         *  1: Enable, which means a configurable segmented transfer is started.\\
         */
        uint32_t usr_conf:1;
        /** mst_fd_wait_dma_tx_data : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to wait DMA TX data gets ready before starting SPI
         *  transfer in master full-duplex transfer.\\
         *  0: Not wait\\
         *  1: Wait\\
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
         *  Configures the transferred data bit length in SPI slave full-/half-duplex modes.
         */
        uint32_t slv_data_bitlen:18;
        /** slv_last_command : R/W/SS; bitpos: [25:18]; default: 0;
         *  Configures the command value in slave mode.
         */
        uint32_t slv_last_command:8;
        /** slv_last_addr : R/W/SS; bitpos: [31:26]; default: 0;
         *  Configures the address value in slave mode.
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
         *  In master transfer, this field must be equal to SPI_CLKCNT_N. In slave mode, it
         *  must be 0. Can be configured in CONF state.
         */
        uint32_t clkcnt_l:6;
        /** clkcnt_h : R/W; bitpos: [11:6]; default: 1;
         *  Configures the duty cycle of SPI_CLK (high level) in master transfer.
         *  It's recommended to configure this value to floor((SPI_CLKCNT_N + 1)/2 - 1).
         *  floor() here is to round a number down, e.g., floor(2.2) = 2. In slave mode, it
         *  must be 0. \\
         *  Can be configured in CONF state.
         */
        uint32_t clkcnt_h:6;
        /** clkcnt_n : R/W; bitpos: [17:12]; default: 3;
         *  Configures the divider of SPI_CLK in master transfer.
         *  SPI_CLK frequency is $f_{\textrm{apb_clk}}$/(SPI_CLKDIV_PRE + 1)/(SPI_CLKCNT_N +
         *  1). \\
         *  Can be configured in CONF state.
         */
        uint32_t clkcnt_n:6;
        /** clkdiv_pre : R/W; bitpos: [21:18]; default: 0;
         *  Configures the pre-divider of SPI_CLK in master transfer.
         *  Can be configured in CONF state.
         */
        uint32_t clkdiv_pre:4;
        uint32_t reserved_22:9;
        /** clk_equ_sysclk : R/W; bitpos: [31]; default: 1;
         *  Configures whether or not the SPI_CLK is equal to APB_CLK in master transfer.\\
         *  0: SPI_CLK is divided from APB_CLK.\\
         *  1: SPI_CLK is eqaul to APB_CLK.\\
         *  Can be configured in CONF state.
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
         *  Configures whether or not to enable clock gate.\\
         *  0: Disable\\
         *  1: Enable\\
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
         *  Configures the input mode for FSPID signal.\\
         *  0: Input without delay\\
         *  1: Input at the (SPI_DIN0_NUM + 1)th falling edge of clk_spi_mst\\
         *  2: Input at the (SPI_DIN0_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
         *  rising edge cycle\\
         *  3: Input at the (SPI_DIN0_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
         *  falling edge cycle\\
         *  Can be configured in CONF state.
         */
        uint32_t din0_mode:2;
        /** din1_mode : R/W; bitpos: [3:2]; default: 0;
         *  Configures the input mode for FSPIQ signal.\\
         *  0: Input without delay\\
         *  1: Input at the (SPI_DIN1_NUM+1)th falling edge of clk_spi_mst\\
         *  2: Input at the (SPI_DIN1_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
         *  rising edge cycle\\
         *  3: Input at the (SPI_DIN1_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
         *  falling edge cycle\\
         *  Can be configured in CONF state.
         */
        uint32_t din1_mode:2;
        /** din2_mode : R/W; bitpos: [5:4]; default: 0;
         *  Configures the input mode for FSPIWP signal.\\
         *  0: Input without delay\\
         *  1: Input at the (SPI_DIN2_NUM + 1)th falling edge of clk_spi_mst\\
         *  2: Input at the (SPI_DIN2_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
         *  rising edge cycle\\
         *  3: Input at the (SPI_DIN2_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
         *  falling edge cycle\\
         *  Can be configured in CONF state.
         */
        uint32_t din2_mode:2;
        /** din3_mode : R/W; bitpos: [7:6]; default: 0;
         *  Configures the input mode for FSPIHD signal.\\
         *  0: Input without delay\\
         *  1: Input at the (SPI_DIN3_NUM + 1)th falling edge of clk_spi_mst\\
         *  2: Input at the (SPI_DIN3_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
         *  rising edge cycle\\
         *  3: Input at the (SPI_DIN3_NUM + 1)th rising edge of clk_hclk plus one clk_spi_mst
         *  falling edge cycle\\
         *  Can be configured in CONF state.
         *
         */
        uint32_t din3_mode:2;
        /** din4_mode : HRO; bitpos: [9:8]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din4_mode:2;
        /** din5_mode : HRO; bitpos: [11:10]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din5_mode:2;
        /** din6_mode : HRO; bitpos: [13:12]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din6_mode:2;
        /** din7_mode : HRO; bitpos: [15:14]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t din7_mode:2;
        /** timing_hclk_active : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable HCLK (high-frequency clock) in SPI input timing
         *  module.\\
         *  0: Disable\\
         *  1: Enable\\
         *  Can be configured in CONF state.
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
         *  Configures the delays to input signal FSPID based on the setting of SPI_DIN0_MODE.\\
         *  0: Delayed by 1 clock cycle\\
         *  1: Delayed by 2 clock cycles\\
         *  2: Delayed by 3 clock cycles\\
         *  3: Delayed by 4 clock cycles\\
         *  Can be configured in CONF state.
         */
        uint32_t din0_num:2;
        /** din1_num : R/W; bitpos: [3:2]; default: 0;
         *  Configures the delays to input signal FSPIQ based on the setting of SPI_DIN1_MODE.\\
         *  0: Delayed by 1 clock cycle\\
         *  1: Delayed by 2 clock cycles\\
         *  2: Delayed by 3 clock cycles\\
         *  3: Delayed by 4 clock cycles\\
         *  Can be configured in CONF state.
         */
        uint32_t din1_num:2;
        /** din2_num : R/W; bitpos: [5:4]; default: 0;
         *  Configures the delays to input signal FSPIWP based on the setting of
         *  SPI_DIN2_MODE.\\
         *  0: Delayed by 1 clock cycle\\
         *  1: Delayed by 2 clock cycles\\
         *  2: Delayed by 3 clock cycles\\
         *  3: Delayed by 4 clock cycles\\
         *  Can be configured in CONF state.
         */
        uint32_t din2_num:2;
        /** din3_num : R/W; bitpos: [7:6]; default: 0;
         *  Configures the delays to input signal FSPIHD based on the setting of
         *  SPI_DIN3_MODE.\\
         *  0: Delayed by 1 clock cycle\\
         *  1: Delayed by 2 clock cycles\\
         *  2: Delayed by 3 clock cycles\\
         *  3: Delayed by 4 clock cycles\\
         *  Can be configured in CONF state.
         */
        uint32_t din3_num:2;
        /** din4_num : HRO; bitpos: [9:8]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din4_num:2;
        /** din5_num : HRO; bitpos: [11:10]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din5_num:2;
        /** din6_num : HRO; bitpos: [13:12]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din6_num:2;
        /** din7_num : HRO; bitpos: [15:14]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t din7_num:2;
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
         *  Configures the output mode for FSPID signal.\\
         *  0: Output without delay\\
         *  1: Output with a delay of a SPI module clock cycle at its falling edge\\
         *  Can be configured in CONF state.
         */
        uint32_t dout0_mode:1;
        /** dout1_mode : R/W; bitpos: [1]; default: 0;
         *  Configures the output mode for FSPIQ signal.\\
         *  0: Output without delay\\
         *  1: Output with a delay of a SPI module clock cycle at its falling edge\\
         *  Can be configured in CONF state.
         */
        uint32_t dout1_mode:1;
        /** dout2_mode : R/W; bitpos: [2]; default: 0;
         *  Configures the output mode for FSPIWP signal.\\
         *  0: Output without delay\\
         *  1: Output with a delay of a SPI module clock cycle at its falling edge\\
         *  Can be configured in CONF state.
         */
        uint32_t dout2_mode:1;
        /** dout3_mode : R/W; bitpos: [3]; default: 0;
         *  Configures the output mode for FSPIHD signal.\\
         *  0: Output without delay\\
         *  1: Output with a delay of a SPI module clock cycle at its falling edge\\
         *  Can be configured in CONF state.
         */
        uint32_t dout3_mode:1;
        /** dout4_mode : HRO; bitpos: [4]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout4_mode:1;
        /** dout5_mode : HRO; bitpos: [5]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout5_mode:1;
        /** dout6_mode : HRO; bitpos: [6]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout6_mode:1;
        /** dout7_mode : HRO; bitpos: [7]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t dout7_mode:1;
        /** d_dqs_mode : HRO; bitpos: [8]; default: 0;
         *  The output signal SPI_DQS is delayed by the SPI module clock, 0: output without
         *  delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t d_dqs_mode:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} spi_dout_mode_reg_t;


/** Group: Interrupt registers */
/** Type of dma_int_ena register
 *  SPI interrupt enable register
 */
typedef union {
    struct {
        /** dma_infifo_full_err_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
         */
        uint32_t dma_infifo_full_err_int_ena:1;
        /** dma_outfifo_empty_err_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
         */
        uint32_t dma_outfifo_empty_err_int_ena:1;
        /** slv_ex_qpi_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable SPI_SLV_EX_QPI_INT interrupt.
         */
        uint32_t slv_ex_qpi_int_ena:1;
        /** slv_en_qpi_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable SPI_SLV_EN_QPI_INT interrupt.
         */
        uint32_t slv_en_qpi_int_ena:1;
        /** slv_cmd7_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable SPI_SLV_CMD7_INT interrupt.
         */
        uint32_t slv_cmd7_int_ena:1;
        /** slv_cmd8_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable SPI_SLV_CMD8_INT interrupt.
         */
        uint32_t slv_cmd8_int_ena:1;
        /** slv_cmd9_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable SPI_SLV_CMD9_INT interrupt.
         */
        uint32_t slv_cmd9_int_ena:1;
        /** slv_cmda_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to enable SPI_SLV_CMDA_INT interrupt.
         */
        uint32_t slv_cmda_int_ena:1;
        /** slv_rd_dma_done_int_ena : R/W; bitpos: [8]; default: 0;
         *  Write 1 to enable SPI_SLV_RD_DMA_DONE_INT interrupt.
         */
        uint32_t slv_rd_dma_done_int_ena:1;
        /** slv_wr_dma_done_int_ena : R/W; bitpos: [9]; default: 0;
         *  Write 1 to enable SPI_SLV_WR_DMA_DONE_INT interrupt.
         */
        uint32_t slv_wr_dma_done_int_ena:1;
        /** slv_rd_buf_done_int_ena : R/W; bitpos: [10]; default: 0;
         *  Write 1 to enable SPI_SLV_RD_BUF_DONE_INT interrupt.
         */
        uint32_t slv_rd_buf_done_int_ena:1;
        /** slv_wr_buf_done_int_ena : R/W; bitpos: [11]; default: 0;
         *  Write 1 to enable SPI_SLV_WR_BUF_DONE_INT interrupt.
         */
        uint32_t slv_wr_buf_done_int_ena:1;
        /** trans_done_int_ena : R/W; bitpos: [12]; default: 0;
         *  Write 1 to enable SPI_TRANS_DONE_INT interrupt.
         */
        uint32_t trans_done_int_ena:1;
        /** dma_seg_trans_done_int_ena : R/W; bitpos: [13]; default: 0;
         *  Write 1 to enable SPI_DMA_SEG_TRANS_DONE_INT interrupt.
         */
        uint32_t dma_seg_trans_done_int_ena:1;
        /** seg_magic_err_int_ena : R/W; bitpos: [14]; default: 0;
         *  Write 1 to enable SPI_SEG_MAGIC_ERR_INT interrupt.
         */
        uint32_t seg_magic_err_int_ena:1;
        /** slv_buf_addr_err_int_ena : R/W; bitpos: [15]; default: 0;
         *  The enable bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
         */
        uint32_t slv_buf_addr_err_int_ena:1;
        /** slv_cmd_err_int_ena : R/W; bitpos: [16]; default: 0;
         *  Write 1 to enable SPI_SLV_CMD_ERR_INT interrupt.
         */
        uint32_t slv_cmd_err_int_ena:1;
        /** mst_rx_afifo_wfull_err_int_ena : R/W; bitpos: [17]; default: 0;
         *  Write 1 to enable SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
         */
        uint32_t mst_rx_afifo_wfull_err_int_ena:1;
        /** mst_tx_afifo_rempty_err_int_ena : R/W; bitpos: [18]; default: 0;
         *  Write 1 to enable SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
         */
        uint32_t mst_tx_afifo_rempty_err_int_ena:1;
        /** app2_int_ena : R/W; bitpos: [19]; default: 0;
         *  Write 1 to enable SPI_APP2_INT interrupt.
         */
        uint32_t app2_int_ena:1;
        /** app1_int_ena : R/W; bitpos: [20]; default: 0;
         *  Write 1 to enable SPI_APP1_INT interrupt.
         */
        uint32_t app1_int_ena:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} spi_dma_int_ena_reg_t;

/** Type of dma_int_clr register
 *  SPI interrupt clear register
 */
typedef union {
    struct {
        /** dma_infifo_full_err_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
         */
        uint32_t dma_infifo_full_err_int_clr:1;
        /** dma_outfifo_empty_err_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
         */
        uint32_t dma_outfifo_empty_err_int_clr:1;
        /** slv_ex_qpi_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear SPI_SLV_EX_QPI_INT interrupt.
         */
        uint32_t slv_ex_qpi_int_clr:1;
        /** slv_en_qpi_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear SPI_SLV_EN_QPI_INT interrupt.
         */
        uint32_t slv_en_qpi_int_clr:1;
        /** slv_cmd7_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear SPI_SLV_CMD7_INT interrupt.
         */
        uint32_t slv_cmd7_int_clr:1;
        /** slv_cmd8_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear SPI_SLV_CMD8_INT interrupt.
         */
        uint32_t slv_cmd8_int_clr:1;
        /** slv_cmd9_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear SPI_SLV_CMD9_INT interrupt.
         */
        uint32_t slv_cmd9_int_clr:1;
        /** slv_cmda_int_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear SPI_SLV_CMDA_INT interrupt.
         */
        uint32_t slv_cmda_int_clr:1;
        /** slv_rd_dma_done_int_clr : WT; bitpos: [8]; default: 0;
         *  Write 1 to clear SPI_SLV_RD_DMA_DONE_INT interrupt.
         */
        uint32_t slv_rd_dma_done_int_clr:1;
        /** slv_wr_dma_done_int_clr : WT; bitpos: [9]; default: 0;
         *  Write 1 to clear SPI_SLV_WR_DMA_DONE_INT interrupt.
         */
        uint32_t slv_wr_dma_done_int_clr:1;
        /** slv_rd_buf_done_int_clr : WT; bitpos: [10]; default: 0;
         *  Write 1 to clear SPI_SLV_RD_BUF_DONE_INT interrupt.
         */
        uint32_t slv_rd_buf_done_int_clr:1;
        /** slv_wr_buf_done_int_clr : WT; bitpos: [11]; default: 0;
         *  Write 1 to clear SPI_SLV_WR_BUF_DONE_INT interrupt.
         */
        uint32_t slv_wr_buf_done_int_clr:1;
        /** trans_done_int_clr : WT; bitpos: [12]; default: 0;
         *  Write 1 to clear SPI_TRANS_DONE_INT interrupt.
         */
        uint32_t trans_done_int_clr:1;
        /** dma_seg_trans_done_int_clr : WT; bitpos: [13]; default: 0;
         *  Write 1 to clear SPI_DMA_SEG_TRANS_DONE_INT interrupt.
         */
        uint32_t dma_seg_trans_done_int_clr:1;
        /** seg_magic_err_int_clr : WT; bitpos: [14]; default: 0;
         *  Write 1 to clear SPI_SEG_MAGIC_ERR_INT interrupt.
         */
        uint32_t seg_magic_err_int_clr:1;
        /** slv_buf_addr_err_int_clr : WT; bitpos: [15]; default: 0;
         *  The clear bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
         */
        uint32_t slv_buf_addr_err_int_clr:1;
        /** slv_cmd_err_int_clr : WT; bitpos: [16]; default: 0;
         *  Write 1 to clear SPI_SLV_CMD_ERR_INT interrupt.
         */
        uint32_t slv_cmd_err_int_clr:1;
        /** mst_rx_afifo_wfull_err_int_clr : WT; bitpos: [17]; default: 0;
         *  Write 1 to clear SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
         */
        uint32_t mst_rx_afifo_wfull_err_int_clr:1;
        /** mst_tx_afifo_rempty_err_int_clr : WT; bitpos: [18]; default: 0;
         *  Write 1 to clear SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
         */
        uint32_t mst_tx_afifo_rempty_err_int_clr:1;
        /** app2_int_clr : WT; bitpos: [19]; default: 0;
         *  Write 1 to clear SPI_APP2_INT interrupt.
         */
        uint32_t app2_int_clr:1;
        /** app1_int_clr : WT; bitpos: [20]; default: 0;
         *  Write 1 to clear SPI_APP1_INT interrupt.
         */
        uint32_t app1_int_clr:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} spi_dma_int_clr_reg_t;

/** Type of dma_int_raw register
 *  SPI interrupt raw register
 */
typedef union {
    struct {
        /** dma_infifo_full_err_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
         */
        uint32_t dma_infifo_full_err_int_raw:1;
        /** dma_outfifo_empty_err_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
         */
        uint32_t dma_outfifo_empty_err_int_raw:1;
        /** slv_ex_qpi_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of SPI_SLV_EX_QPI_INT interrupt.
         */
        uint32_t slv_ex_qpi_int_raw:1;
        /** slv_en_qpi_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of SPI_SLV_EN_QPI_INT interrupt.
         */
        uint32_t slv_en_qpi_int_raw:1;
        /** slv_cmd7_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of SPI_SLV_CMD7_INT interrupt.
         */
        uint32_t slv_cmd7_int_raw:1;
        /** slv_cmd8_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of SPI_SLV_CMD8_INT interrupt.
         */
        uint32_t slv_cmd8_int_raw:1;
        /** slv_cmd9_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of SPI_SLV_CMD9_INT interrupt.
         */
        uint32_t slv_cmd9_int_raw:1;
        /** slv_cmda_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt status of SPI_SLV_CMDA_INT interrupt.
         */
        uint32_t slv_cmda_int_raw:1;
        /** slv_rd_dma_done_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt status of SPI_SLV_RD_DMA_DONE_INT interrupt.
         */
        uint32_t slv_rd_dma_done_int_raw:1;
        /** slv_wr_dma_done_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt status of SPI_SLV_WR_DMA_DONE_INT interrupt.
         */
        uint32_t slv_wr_dma_done_int_raw:1;
        /** slv_rd_buf_done_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw interrupt status of SPI_SLV_RD_BUF_DONE_INT interrupt.
         */
        uint32_t slv_rd_buf_done_int_raw:1;
        /** slv_wr_buf_done_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw interrupt status of SPI_SLV_WR_BUF_DONE_INT interrupt.
         */
        uint32_t slv_wr_buf_done_int_raw:1;
        /** trans_done_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw interrupt status of SPI_TRANS_DONE_INT interrupt.
         */
        uint32_t trans_done_int_raw:1;
        /** dma_seg_trans_done_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw interrupt status of SPI_DMA_SEG_TRANS_DONE_INT interrupt.
         */
        uint32_t dma_seg_trans_done_int_raw:1;
        /** seg_magic_err_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The raw interrupt status of SPI_SEG_MAGIC_ERR_INT interrupt.
         */
        uint32_t seg_magic_err_int_raw:1;
        /** slv_buf_addr_err_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  The raw bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt. 1: The accessing data address
         *  of the current SPI slave mode CPU controlled FD, Wr_BUF or Rd_BUF transmission is
         *  bigger than 63. 0: Others.
         */
        uint32_t slv_buf_addr_err_int_raw:1;
        /** slv_cmd_err_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  The raw interrupt status of SPI_SLV_CMD_ERR_INT interrupt.
         */
        uint32_t slv_cmd_err_int_raw:1;
        /** mst_rx_afifo_wfull_err_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  The raw interrupt status of SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
         */
        uint32_t mst_rx_afifo_wfull_err_int_raw:1;
        /** mst_tx_afifo_rempty_err_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  The raw interrupt status of SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
         */
        uint32_t mst_tx_afifo_rempty_err_int_raw:1;
        /** app2_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  The raw interrupt status of SPI_APP2_INT interrupt. The value is only controlled by
         *  the application.
         */
        uint32_t app2_int_raw:1;
        /** app1_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  The raw interrupt status of SPI_APP1_INT interrupt. The value is only controlled by
         *  the application.
         */
        uint32_t app1_int_raw:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} spi_dma_int_raw_reg_t;

/** Type of dma_int_st register
 *  SPI interrupt status register
 */
typedef union {
    struct {
        /** dma_infifo_full_err_int_st : RO; bitpos: [0]; default: 0;
         *  The interrupt status of SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
         */
        uint32_t dma_infifo_full_err_int_st:1;
        /** dma_outfifo_empty_err_int_st : RO; bitpos: [1]; default: 0;
         *  The interrupt status of SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
         */
        uint32_t dma_outfifo_empty_err_int_st:1;
        /** slv_ex_qpi_int_st : RO; bitpos: [2]; default: 0;
         *  The interrupt status of SPI_SLV_EX_QPI_INT interrupt.
         */
        uint32_t slv_ex_qpi_int_st:1;
        /** slv_en_qpi_int_st : RO; bitpos: [3]; default: 0;
         *  The interrupt status of SPI_SLV_EN_QPI_INT interrupt.
         */
        uint32_t slv_en_qpi_int_st:1;
        /** slv_cmd7_int_st : RO; bitpos: [4]; default: 0;
         *  The interrupt status of SPI_SLV_CMD7_INT interrupt.
         */
        uint32_t slv_cmd7_int_st:1;
        /** slv_cmd8_int_st : RO; bitpos: [5]; default: 0;
         *  The interrupt status of SPI_SLV_CMD8_INT interrupt.
         */
        uint32_t slv_cmd8_int_st:1;
        /** slv_cmd9_int_st : RO; bitpos: [6]; default: 0;
         *  The interrupt status of SPI_SLV_CMD9_INT interrupt.
         */
        uint32_t slv_cmd9_int_st:1;
        /** slv_cmda_int_st : RO; bitpos: [7]; default: 0;
         *  The interrupt status of SPI_SLV_CMDA_INT interrupt.
         */
        uint32_t slv_cmda_int_st:1;
        /** slv_rd_dma_done_int_st : RO; bitpos: [8]; default: 0;
         *  The interrupt status of SPI_SLV_RD_DMA_DONE_INT interrupt.
         */
        uint32_t slv_rd_dma_done_int_st:1;
        /** slv_wr_dma_done_int_st : RO; bitpos: [9]; default: 0;
         *  The interrupt status of SPI_SLV_WR_DMA_DONE_INT interrupt.
         */
        uint32_t slv_wr_dma_done_int_st:1;
        /** slv_rd_buf_done_int_st : RO; bitpos: [10]; default: 0;
         *  The interrupt status of SPI_SLV_RD_BUF_DONE_INT interrupt.
         */
        uint32_t slv_rd_buf_done_int_st:1;
        /** slv_wr_buf_done_int_st : RO; bitpos: [11]; default: 0;
         *  The interrupt status of SPI_SLV_WR_BUF_DONE_INT interrupt.
         */
        uint32_t slv_wr_buf_done_int_st:1;
        /** trans_done_int_st : RO; bitpos: [12]; default: 0;
         *  The interrupt status of SPI_TRANS_DONE_INT interrupt.
         */
        uint32_t trans_done_int_st:1;
        /** dma_seg_trans_done_int_st : RO; bitpos: [13]; default: 0;
         *  The interrupt status of SPI_DMA_SEG_TRANS_DONE_INT interrupt.
         */
        uint32_t dma_seg_trans_done_int_st:1;
        /** seg_magic_err_int_st : RO; bitpos: [14]; default: 0;
         *  The interrupt status of SPI_SEG_MAGIC_ERR_INT interrupt.
         */
        uint32_t seg_magic_err_int_st:1;
        /** slv_buf_addr_err_int_st : RO; bitpos: [15]; default: 0;
         *  The status bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
         */
        uint32_t slv_buf_addr_err_int_st:1;
        /** slv_cmd_err_int_st : RO; bitpos: [16]; default: 0;
         *  The interrupt status of SPI_SLV_CMD_ERR_INT interrupt.
         */
        uint32_t slv_cmd_err_int_st:1;
        /** mst_rx_afifo_wfull_err_int_st : RO; bitpos: [17]; default: 0;
         *  The interrupt status of SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
         */
        uint32_t mst_rx_afifo_wfull_err_int_st:1;
        /** mst_tx_afifo_rempty_err_int_st : RO; bitpos: [18]; default: 0;
         *  The interrupt status of SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
         */
        uint32_t mst_tx_afifo_rempty_err_int_st:1;
        /** app2_int_st : RO; bitpos: [19]; default: 0;
         *  The interrupt status of SPI_APP2_INT interrupt.
         */
        uint32_t app2_int_st:1;
        /** app1_int_st : RO; bitpos: [20]; default: 0;
         *  The interrupt status of SPI_APP1_INT interrupt.
         */
        uint32_t app1_int_st:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} spi_dma_int_st_reg_t;

/** Type of dma_int_set register
 *  SPI interrupt software set register
 */
typedef union {
    struct {
        /** dma_infifo_full_err_int_set : WT; bitpos: [0]; default: 0;
         *  Write 1 to set SPI_DMA_INFIFO_FULL_ERR_INT interrupt.
         */
        uint32_t dma_infifo_full_err_int_set:1;
        /** dma_outfifo_empty_err_int_set : WT; bitpos: [1]; default: 0;
         *  Write 1 to set SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.
         */
        uint32_t dma_outfifo_empty_err_int_set:1;
        /** slv_ex_qpi_int_set : WT; bitpos: [2]; default: 0;
         *  Write 1 to set SPI_SLV_EX_QPI_INT interrupt.
         */
        uint32_t slv_ex_qpi_int_set:1;
        /** slv_en_qpi_int_set : WT; bitpos: [3]; default: 0;
         *  Write 1 to set SPI_SLV_EN_QPI_INT interrupt.
         */
        uint32_t slv_en_qpi_int_set:1;
        /** slv_cmd7_int_set : WT; bitpos: [4]; default: 0;
         *  Write 1 to set SPI_SLV_CMD7_INT interrupt.
         */
        uint32_t slv_cmd7_int_set:1;
        /** slv_cmd8_int_set : WT; bitpos: [5]; default: 0;
         *  Write 1 to set SPI_SLV_CMD8_INT interrupt.
         */
        uint32_t slv_cmd8_int_set:1;
        /** slv_cmd9_int_set : WT; bitpos: [6]; default: 0;
         *  Write 1 to set SPI_SLV_CMD9_INT interrupt.
         */
        uint32_t slv_cmd9_int_set:1;
        /** slv_cmda_int_set : WT; bitpos: [7]; default: 0;
         *  Write 1 to set SPI_SLV_CMDA_INT interrupt.
         */
        uint32_t slv_cmda_int_set:1;
        /** slv_rd_dma_done_int_set : WT; bitpos: [8]; default: 0;
         *  Write 1 to set SPI_SLV_RD_DMA_DONE_INT interrupt.
         */
        uint32_t slv_rd_dma_done_int_set:1;
        /** slv_wr_dma_done_int_set : WT; bitpos: [9]; default: 0;
         *  Write 1 to set SPI_SLV_WR_DMA_DONE_INT interrupt.
         */
        uint32_t slv_wr_dma_done_int_set:1;
        /** slv_rd_buf_done_int_set : WT; bitpos: [10]; default: 0;
         *  Write 1 to set SPI_SLV_RD_BUF_DONE_INT interrupt.
         */
        uint32_t slv_rd_buf_done_int_set:1;
        /** slv_wr_buf_done_int_set : WT; bitpos: [11]; default: 0;
         *  Write 1 to set SPI_SLV_WR_BUF_DONE_INT interrupt.
         */
        uint32_t slv_wr_buf_done_int_set:1;
        /** trans_done_int_set : WT; bitpos: [12]; default: 0;
         *  Write 1 to set SPI_TRANS_DONE_INT interrupt.
         */
        uint32_t trans_done_int_set:1;
        /** dma_seg_trans_done_int_set : WT; bitpos: [13]; default: 0;
         *  Write 1 to set SPI_DMA_SEG_TRANS_DONE_INT interrupt.
         */
        uint32_t dma_seg_trans_done_int_set:1;
        /** seg_magic_err_int_set : WT; bitpos: [14]; default: 0;
         *  Write 1 to set SPI_SEG_MAGIC_ERR_INT interrupt.
         */
        uint32_t seg_magic_err_int_set:1;
        /** slv_buf_addr_err_int_set : WT; bitpos: [15]; default: 0;
         *  The software set bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
         */
        uint32_t slv_buf_addr_err_int_set:1;
        /** slv_cmd_err_int_set : WT; bitpos: [16]; default: 0;
         *  Write 1 to set SPI_SLV_CMD_ERR_INT interrupt.
         */
        uint32_t slv_cmd_err_int_set:1;
        /** mst_rx_afifo_wfull_err_int_set : WT; bitpos: [17]; default: 0;
         *  Write 1 to set SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
         */
        uint32_t mst_rx_afifo_wfull_err_int_set:1;
        /** mst_tx_afifo_rempty_err_int_set : WT; bitpos: [18]; default: 0;
         *  Write 1 to set SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
         */
        uint32_t mst_tx_afifo_rempty_err_int_set:1;
        /** app2_int_set : WT; bitpos: [19]; default: 0;
         *  Write 1 to set SPI_APP2_INT interrupt.
         */
        uint32_t app2_int_set:1;
        /** app1_int_set : WT; bitpos: [20]; default: 0;
         *  Write 1 to set SPI_APP1_INT interrupt.
         */
        uint32_t app1_int_set:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} spi_dma_int_set_reg_t;


/** Group: CPU-controlled data buffer */
/** Type of w0 register
 *  SPI CPU-controlled buffer0
 */
typedef union {
    struct {
        /** buf0 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf0:32;
    };
    uint32_t val;
} spi_w0_reg_t;

/** Type of w1 register
 *  SPI CPU-controlled buffer1
 */
typedef union {
    struct {
        /** buf1 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf1:32;
    };
    uint32_t val;
} spi_w1_reg_t;

/** Type of w2 register
 *  SPI CPU-controlled buffer2
 */
typedef union {
    struct {
        /** buf2 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf2:32;
    };
    uint32_t val;
} spi_w2_reg_t;

/** Type of w3 register
 *  SPI CPU-controlled buffer3
 */
typedef union {
    struct {
        /** buf3 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf3:32;
    };
    uint32_t val;
} spi_w3_reg_t;

/** Type of w4 register
 *  SPI CPU-controlled buffer4
 */
typedef union {
    struct {
        /** buf4 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf4:32;
    };
    uint32_t val;
} spi_w4_reg_t;

/** Type of w5 register
 *  SPI CPU-controlled buffer5
 */
typedef union {
    struct {
        /** buf5 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf5:32;
    };
    uint32_t val;
} spi_w5_reg_t;

/** Type of w6 register
 *  SPI CPU-controlled buffer6
 */
typedef union {
    struct {
        /** buf6 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf6:32;
    };
    uint32_t val;
} spi_w6_reg_t;

/** Type of w7 register
 *  SPI CPU-controlled buffer7
 */
typedef union {
    struct {
        /** buf7 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf7:32;
    };
    uint32_t val;
} spi_w7_reg_t;

/** Type of w8 register
 *  SPI CPU-controlled buffer8
 */
typedef union {
    struct {
        /** buf8 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf8:32;
    };
    uint32_t val;
} spi_w8_reg_t;

/** Type of w9 register
 *  SPI CPU-controlled buffer9
 */
typedef union {
    struct {
        /** buf9 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf9:32;
    };
    uint32_t val;
} spi_w9_reg_t;

/** Type of w10 register
 *  SPI CPU-controlled buffer10
 */
typedef union {
    struct {
        /** buf10 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf10:32;
    };
    uint32_t val;
} spi_w10_reg_t;

/** Type of w11 register
 *  SPI CPU-controlled buffer11
 */
typedef union {
    struct {
        /** buf11 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf11:32;
    };
    uint32_t val;
} spi_w11_reg_t;

/** Type of w12 register
 *  SPI CPU-controlled buffer12
 */
typedef union {
    struct {
        /** buf12 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf12:32;
    };
    uint32_t val;
} spi_w12_reg_t;

/** Type of w13 register
 *  SPI CPU-controlled buffer13
 */
typedef union {
    struct {
        /** buf13 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf13:32;
    };
    uint32_t val;
} spi_w13_reg_t;

/** Type of w14 register
 *  SPI CPU-controlled buffer14
 */
typedef union {
    struct {
        /** buf14 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf14:32;
    };
    uint32_t val;
} spi_w14_reg_t;

/** Type of w15 register
 *  SPI CPU-controlled buffer15
 */
typedef union {
    struct {
        /** buf15 : R/W/SS; bitpos: [31:0]; default: 0;
         *  32-bit data buffer $n.
         */
        uint32_t buf15:32;
    };
    uint32_t val;
} spi_w15_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36716931;
         *  Version control register.
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
    volatile spi_dma_int_ena_reg_t dma_int_ena;
    volatile spi_dma_int_clr_reg_t dma_int_clr;
    volatile spi_dma_int_raw_reg_t dma_int_raw;
    volatile spi_dma_int_st_reg_t dma_int_st;
    volatile spi_dma_int_set_reg_t dma_int_set;
    uint32_t reserved_048[20];
    volatile spi_w0_reg_t w0;
    volatile spi_w1_reg_t w1;
    volatile spi_w2_reg_t w2;
    volatile spi_w3_reg_t w3;
    volatile spi_w4_reg_t w4;
    volatile spi_w5_reg_t w5;
    volatile spi_w6_reg_t w6;
    volatile spi_w7_reg_t w7;
    volatile spi_w8_reg_t w8;
    volatile spi_w9_reg_t w9;
    volatile spi_w10_reg_t w10;
    volatile spi_w11_reg_t w11;
    volatile spi_w12_reg_t w12;
    volatile spi_w13_reg_t w13;
    volatile spi_w14_reg_t w14;
    volatile spi_w15_reg_t w15;
    uint32_t reserved_0d8[2];
    volatile spi_slave_reg_t slave;
    volatile spi_slave1_reg_t slave1;
    volatile spi_clk_gate_reg_t clk_gate;
    uint32_t reserved_0ec;
    volatile spi_date_reg_t date;
} spi_dev_t;

extern spi_dev_t GPSPI2;

#ifndef __cplusplus
_Static_assert(sizeof(spi_dev_t) == 0xf4, "Invalid size of spi_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
