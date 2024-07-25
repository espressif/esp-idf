/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: LP SPI CMD REG */
/** Type of spi_cmd register
 *  Command control register
 */
typedef union {
    struct {
        uint32_t reserved_0:23;
        /** reg_update : WT; bitpos: [23]; default: 0;
         *  Set this bit to synchronize SPI registers from APB clock domain into SPI module
         *  clock domain, which is only used in SPI master mode.
         */
        uint32_t reg_update:1;
        /** reg_usr : R/W/SC; bitpos: [24]; default: 0;
         *  User define command enable.  An operation will be triggered when the bit is set.
         *  The bit will be cleared once the operation done.1: enable 0: disable. Can not be
         *  changed by CONF_buf.
         */
        uint32_t reg_usr:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} lp_spi_cmd_reg_t;


/** Group: LP SPI ADDR REG */
/** Type of spi_addr register
 *  Address value register
 */
typedef union {
    struct {
        /** reg_usr_addr_value : R/W; bitpos: [31:0]; default: 0;
         *  Address to slave. Can be configured in CONF state.
         */
        uint32_t reg_usr_addr_value:32;
    };
    uint32_t val;
} lp_spi_addr_reg_t;


/** Group: LP SPI CTRL REG */
/** Type of spi_ctrl register
 *  SPI control register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** reg_dummy_out : R/W; bitpos: [3]; default: 0;
         *  In the dummy phase the signal level of spi is output by the spi controller. Can be
         *  configured in CONF state.
         */
        uint32_t reg_dummy_out:1;
        uint32_t reserved_4:14;
        /** reg_q_pol : R/W; bitpos: [18]; default: 1;
         *  The bit is used to set MISO line polarity, 1: high 0, low. Can be configured in
         *  CONF state.
         */
        uint32_t reg_q_pol:1;
        /** reg_d_pol : R/W; bitpos: [19]; default: 1;
         *  The bit is used to set MOSI line polarity, 1: high 0, low. Can be configured in
         *  CONF state.
         */
        uint32_t reg_d_pol:1;
        uint32_t reserved_20:5;
        /** reg_rd_bit_order : R/W; bitpos: [25]; default: 0;
         *  In read-data (MISO) phase 1: LSB first 0: MSB first. Can be configured in CONF
         *  state.
         */
        uint32_t reg_rd_bit_order:1;
        /** reg_wr_bit_order : R/W; bitpos: [26]; default: 0;
         *  In command address write-data (MOSI) phases 1: LSB firs 0: MSB first. Can be
         *  configured in CONF state.
         */
        uint32_t reg_wr_bit_order:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} lp_spi_ctrl_reg_t;


/** Group: LP SPI CLOCK REG */
/** Type of spi_clock register
 *  SPI clock control register
 */
typedef union {
    struct {
        /** reg_clkcnt_l : R/W; bitpos: [5:0]; default: 3;
         *  In the master mode it must be equal to spi_clkcnt_N. In the slave mode it must be
         *  0. Can be configured in CONF state.
         */
        uint32_t reg_clkcnt_l:6;
        /** reg_clkcnt_h : R/W; bitpos: [11:6]; default: 1;
         *  In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In the slave mode it
         *  must be 0. Can be configured in CONF state.
         */
        uint32_t reg_clkcnt_h:6;
        /** reg_clkcnt_n : R/W; bitpos: [17:12]; default: 3;
         *  In the master mode it is the divider of spi_clk. So spi_clk frequency is
         *  system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1). Can be configured in CONF state.
         */
        uint32_t reg_clkcnt_n:6;
        /** reg_clkdiv_pre : R/W; bitpos: [21:18]; default: 0;
         *  In the master mode it is pre-divider of spi_clk.  Can be configured in CONF state.
         */
        uint32_t reg_clkdiv_pre:4;
        uint32_t reserved_22:9;
        /** reg_clk_equ_sysclk : R/W; bitpos: [31]; default: 1;
         *  In the master mode 1: spi_clk is equal to system 0: spi_clk is divided from system
         *  clock. Can be configured in CONF state.
         */
        uint32_t reg_clk_equ_sysclk:1;
    };
    uint32_t val;
} lp_spi_clock_reg_t;


/** Group: LP SPI USER REG */
/** Type of spi_user register
 *  SPI USER control register
 */
typedef union {
    struct {
        /** reg_doutdin : R/W; bitpos: [0]; default: 0;
         *  Set the bit to enable full duplex communication. 1: enable 0: disable. Can be
         *  configured in CONF state.
         */
        uint32_t reg_doutdin:1;
        uint32_t reserved_1:4;
        /** reg_tsck_i_edge : R/W; bitpos: [5]; default: 0;
         *  In the slave mode, this bit can be used to change the polarity of tsck. 0: tsck =
         *  spi_ck_i. 1:tsck = !spi_ck_i.
         */
        uint32_t reg_tsck_i_edge:1;
        /** reg_cs_hold : R/W; bitpos: [6]; default: 1;
         *  spi cs keep low when spi is in  done  phase. 1: enable 0: disable. Can be
         *  configured in CONF state.
         */
        uint32_t reg_cs_hold:1;
        /** reg_cs_setup : R/W; bitpos: [7]; default: 1;
         *  spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable. Can be
         *  configured in CONF state.
         */
        uint32_t reg_cs_setup:1;
        /** reg_rsck_i_edge : R/W; bitpos: [8]; default: 0;
         *  In the slave mode, this bit can be used to change the polarity of rsck. 0: rsck =
         *  !spi_ck_i. 1:rsck = spi_ck_i.
         */
        uint32_t reg_rsck_i_edge:1;
        /** reg_ck_out_edge : R/W; bitpos: [9]; default: 0;
         *  the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode. Can
         *  be configured in CONF state.
         */
        uint32_t reg_ck_out_edge:1;
        uint32_t reserved_10:7;
        /** reg_sio : R/W; bitpos: [17]; default: 0;
         *  Set the bit to enable 3-line half duplex communication mosi and miso signals share
         *  the same pin. 1: enable 0: disable. Can be configured in CONF state.
         */
        uint32_t reg_sio:1;
        uint32_t reserved_18:6;
        /** reg_usr_miso_highpart : R/W; bitpos: [24]; default: 0;
         *  read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0:
         *  disable. Can be configured in CONF state.
         */
        uint32_t reg_usr_miso_highpart:1;
        /** reg_usr_mosi_highpart : R/W; bitpos: [25]; default: 0;
         *  write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable
         *  0: disable.  Can be configured in CONF state.
         */
        uint32_t reg_usr_mosi_highpart:1;
        /** reg_usr_dummy_idle : R/W; bitpos: [26]; default: 0;
         *  spi clock is disable in dummy phase when the bit is enable. Can be configured in
         *  CONF state.
         */
        uint32_t reg_usr_dummy_idle:1;
        /** reg_usr_mosi : R/W; bitpos: [27]; default: 0;
         *  This bit enable the write-data phase of an operation. Can be configured in CONF
         *  state.
         */
        uint32_t reg_usr_mosi:1;
        /** reg_usr_miso : R/W; bitpos: [28]; default: 0;
         *  This bit enable the read-data phase of an operation. Can be configured in CONF
         *  state.
         */
        uint32_t reg_usr_miso:1;
        /** reg_usr_dummy : R/W; bitpos: [29]; default: 0;
         *  This bit enable the dummy phase of an operation. Can be configured in CONF state.
         */
        uint32_t reg_usr_dummy:1;
        /** reg_usr_addr : R/W; bitpos: [30]; default: 0;
         *  This bit enable the address phase of an operation. Can be configured in CONF state.
         */
        uint32_t reg_usr_addr:1;
        /** reg_usr_command : R/W; bitpos: [31]; default: 1;
         *  This bit enable the command phase of an operation. Can be configured in CONF state.
         */
        uint32_t reg_usr_command:1;
    };
    uint32_t val;
} lp_spi_user_reg_t;


/** Group: LP SPI USER1 REG */
/** Type of spi_user1 register
 *  SPI USER control register 1
 */
typedef union {
    struct {
        /** reg_usr_dummy_cyclelen : R/W; bitpos: [7:0]; default: 7;
         *  The length in spi_clk cycles of dummy phase. The register value shall be
         *  (cycle_num-1). Can be configured in CONF state.
         */
        uint32_t reg_usr_dummy_cyclelen:8;
        uint32_t reserved_8:8;
        /** reg_mst_wfull_err_end_en : R/W; bitpos: [16]; default: 1;
         *  1: SPI transfer is ended when SPI RX AFIFO wfull error is valid in GP-SPI master
         *  FD/HD-mode. 0: SPI transfer is not ended when SPI RX AFIFO wfull error is valid in
         *  GP-SPI master FD/HD-mode.
         */
        uint32_t reg_mst_wfull_err_end_en:1;
        /** reg_cs_setup_time : R/W; bitpos: [21:17]; default: 0;
         *  (cycles+1) of prepare phase by spi clock this bits are combined with spi_cs_setup
         *  bit. Can be configured in CONF state.
         */
        uint32_t reg_cs_setup_time:5;
        /** reg_cs_hold_time : R/W; bitpos: [26:22]; default: 1;
         *  delay cycles of cs pin by spi clock this bits are combined with spi_cs_hold bit.
         *  Can be configured in CONF state.
         */
        uint32_t reg_cs_hold_time:5;
        /** reg_usr_addr_bitlen : R/W; bitpos: [31:27]; default: 23;
         *  The length in bits of address phase. The register value shall be (bit_num-1). Can
         *  be configured in CONF state.
         */
        uint32_t reg_usr_addr_bitlen:5;
    };
    uint32_t val;
} lp_spi_user1_reg_t;


/** Group: LP SPI USER2 REG */
/** Type of spi_user2 register
 *  SPI USER control register 2
 */
typedef union {
    struct {
        /** reg_usr_command_value : R/W; bitpos: [15:0]; default: 0;
         *  The value of  command. Can be configured in CONF state.
         */
        uint32_t reg_usr_command_value:16;
        uint32_t reserved_16:11;
        /** reg_mst_rempty_err_end_en : R/W; bitpos: [27]; default: 1;
         *  1: SPI transfer is ended when SPI TX AFIFO read empty error is valid in GP-SPI
         *  master FD/HD-mode. 0: SPI transfer is not ended when SPI TX AFIFO read empty error
         *  is valid in GP-SPI master FD/HD-mode.
         */
        uint32_t reg_mst_rempty_err_end_en:1;
        /** reg_usr_command_bitlen : R/W; bitpos: [31:28]; default: 7;
         *  The length in bits of command phase. The register value shall be (bit_num-1). Can
         *  be configured in CONF state.
         */
        uint32_t reg_usr_command_bitlen:4;
    };
    uint32_t val;
} lp_spi_user2_reg_t;


/** Group: LP SPI MS DLEN REG */
/** Type of spi_ms_dlen register
 *  SPI data bit length control register
 */
typedef union {
    struct {
        /** reg_ms_data_bitlen : R/W; bitpos: [17:0]; default: 0;
         *  The value of these bits is the configured SPI transmission data bit length in
         *  master mode DMA controlled transfer or CPU controlled transfer. The value is also
         *  the configured bit length in slave mode DMA RX controlled transfer. The register
         *  value shall be (bit_num-1). Can be configured in CONF state.
         */
        uint32_t reg_ms_data_bitlen:18;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} lp_spi_ms_dlen_reg_t;


/** Group: LP SPI MISC REG */
/** Type of spi_misc register
 *  SPI misc register
 */
typedef union {
    struct {
        /** reg_cs0_dis : R/W; bitpos: [0]; default: 0;
         *  SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can
         *  be configured in CONF state.
         */
        uint32_t reg_cs0_dis:1;
        uint32_t reserved_1:5;
        /** reg_ck_dis : R/W; bitpos: [6]; default: 0;
         *  1: spi clk out disable,  0: spi clk out enable. Can be configured in CONF state.
         */
        uint32_t reg_ck_dis:1;
        /** reg_master_cs_pol : R/W; bitpos: [9:7]; default: 0;
         *  In the master mode the bits are the polarity of spi cs line, the value is
         *  equivalent to spi_cs ^ spi_master_cs_pol. Can be configured in CONF state.
         */
        uint32_t reg_master_cs_pol:3;
        uint32_t reserved_10:13;
        /** reg_slave_cs_pol : R/W; bitpos: [23]; default: 0;
         *  spi slave input cs polarity select. 1: inv  0: not change. Can be configured in
         *  CONF state.
         */
        uint32_t reg_slave_cs_pol:1;
        uint32_t reserved_24:5;
        /** reg_ck_idle_edge : R/W; bitpos: [29]; default: 0;
         *  1: spi clk line is high when idle     0: spi clk line is low when idle. Can be
         *  configured in CONF state.
         */
        uint32_t reg_ck_idle_edge:1;
        /** reg_cs_keep_active : R/W; bitpos: [30]; default: 0;
         *  spi cs line keep low when the bit is set. Can be configured in CONF state.
         */
        uint32_t reg_cs_keep_active:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_spi_misc_reg_t;


/** Group: LP SPI DIN MODE REG */
/** Type of spi_din_mode register
 *  SPI input delay mode configuration
 */
typedef union {
    struct {
        /** reg_din0_mode : R/W; bitpos: [1:0]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t reg_din0_mode:2;
        /** reg_din1_mode : R/W; bitpos: [3:2]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t reg_din1_mode:2;
        /** reg_din2_mode : R/W; bitpos: [5:4]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t reg_din2_mode:2;
        /** reg_din3_mode : R/W; bitpos: [7:6]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: input without delayed,
         *  1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk. Can be configured in CONF state.
         */
        uint32_t reg_din3_mode:2;
        uint32_t reserved_8:8;
        /** reg_timing_hclk_active : R/W; bitpos: [16]; default: 0;
         *  1:enable hclk in SPI input timing module.  0: disable it. Can be configured in CONF
         *  state.
         */
        uint32_t reg_timing_hclk_active:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lp_spi_din_mode_reg_t;


/** Group: LP SPI DIN NUM REG */
/** Type of spi_din_num register
 *  SPI input delay number configuration
 */
typedef union {
    struct {
        /** reg_din0_num : R/W; bitpos: [1:0]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t reg_din0_num:2;
        /** reg_din1_num : R/W; bitpos: [3:2]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t reg_din1_num:2;
        /** reg_din2_num : R/W; bitpos: [5:4]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t reg_din2_num:2;
        /** reg_din3_num : R/W; bitpos: [7:6]; default: 0;
         *  the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...  Can be configured in CONF state.
         */
        uint32_t reg_din3_num:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_spi_din_num_reg_t;


/** Group: LP SPI DOUT MODE REG */
/** Type of spi_dout_mode register
 *  SPI output delay mode configuration
 */
typedef union {
    struct {
        /** reg_dout0_mode : R/W; bitpos: [0]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t reg_dout0_mode:1;
        /** reg_dout1_mode : R/W; bitpos: [1]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t reg_dout1_mode:1;
        /** reg_dout2_mode : R/W; bitpos: [2]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t reg_dout2_mode:1;
        /** reg_dout3_mode : R/W; bitpos: [3]; default: 0;
         *  The output signal $n is delayed by the SPI module clock, 0: output without delayed,
         *  1: output delay for a SPI module clock cycle at its negative edge. Can be
         *  configured in CONF state.
         */
        uint32_t reg_dout3_mode:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_spi_dout_mode_reg_t;


/** Group: LP SPI DMA CONF REG */
/** Type of spi_dma_conf register
 *  SPI DMA control register
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** reg_rx_afifo_rst : WT; bitpos: [29]; default: 0;
         *  Set this bit to reset RX AFIFO, which is used to receive data in SPI master and
         *  slave mode transfer.
         */
        uint32_t reg_rx_afifo_rst:1;
        /** reg_buf_afifo_rst : WT; bitpos: [30]; default: 0;
         *  Set this bit to reset BUF TX AFIFO, which is used send data out in SPI slave CPU
         *  controlled mode transfer and master mode transfer.
         */
        uint32_t reg_buf_afifo_rst:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} lp_spi_dma_conf_reg_t;


/** Group: LP SPI DMA INT ENA REG */
/** Type of spi_dma_int_ena register
 *  SPI DMA interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** reg_slv_rd_buf_done_int_ena : R/W; bitpos: [10]; default: 0;
         *  The enable bit for SPI_SLV_RD_BUF_DONE_INT interrupt.
         */
        uint32_t reg_slv_rd_buf_done_int_ena:1;
        /** reg_slv_wr_buf_done_int_ena : R/W; bitpos: [11]; default: 0;
         *  The enable bit for SPI_SLV_WR_BUF_DONE_INT interrupt.
         */
        uint32_t reg_slv_wr_buf_done_int_ena:1;
        /** reg_trans_done_int_ena : R/W; bitpos: [12]; default: 0;
         *  The enable bit for SPI_TRANS_DONE_INT interrupt.
         */
        uint32_t reg_trans_done_int_ena:1;
        uint32_t reserved_13:1;
        /** reg_spi_wakeup_int_ena : R/W; bitpos: [14]; default: 0;
         *  The enable bit for SPI_WAKEUP_INT interrupt
         */
        uint32_t reg_spi_wakeup_int_ena:1;
        /** reg_slv_buf_addr_err_int_ena : R/W; bitpos: [15]; default: 0;
         *  The enable bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
         */
        uint32_t reg_slv_buf_addr_err_int_ena:1;
        /** reg_slv_cmd_err_int_ena : R/W; bitpos: [16]; default: 0;
         *  The enable bit for SPI_SLV_CMD_ERR_INT interrupt.
         */
        uint32_t reg_slv_cmd_err_int_ena:1;
        /** reg_mst_rx_afifo_wfull_err_int_ena : R/W; bitpos: [17]; default: 0;
         *  The enable bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
         */
        uint32_t reg_mst_rx_afifo_wfull_err_int_ena:1;
        /** reg_mst_tx_afifo_rempty_err_int_ena : R/W; bitpos: [18]; default: 0;
         *  The enable bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
         */
        uint32_t reg_mst_tx_afifo_rempty_err_int_ena:1;
        /** reg_app2_int_ena : R/W; bitpos: [19]; default: 0;
         *  The enable bit for SPI_APP2_INT interrupt.
         */
        uint32_t reg_app2_int_ena:1;
        /** reg_app1_int_ena : R/W; bitpos: [20]; default: 0;
         *  The enable bit for SPI_APP1_INT interrupt.
         */
        uint32_t reg_app1_int_ena:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} lp_spi_dma_int_ena_reg_t;


/** Group: LP SPI DMA INT CLR REG */
/** Type of spi_dma_int_clr register
 *  SPI DMA interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** reg_slv_rd_buf_done_int_clr : WT; bitpos: [10]; default: 0;
         *  The clear bit for SPI_SLV_RD_BUF_DONE_INT interrupt.
         */
        uint32_t reg_slv_rd_buf_done_int_clr:1;
        /** reg_slv_wr_buf_done_int_clr : WT; bitpos: [11]; default: 0;
         *  The clear bit for SPI_SLV_WR_BUF_DONE_INT interrupt.
         */
        uint32_t reg_slv_wr_buf_done_int_clr:1;
        /** reg_trans_done_int_clr : WT; bitpos: [12]; default: 0;
         *  The clear bit for SPI_TRANS_DONE_INT interrupt.
         */
        uint32_t reg_trans_done_int_clr:1;
        uint32_t reserved_13:1;
        /** reg_spi_wakeup_int_clr : WT; bitpos: [14]; default: 0;
         *  The clear bit for SPI_WAKEUP_INT interrupt
         */
        uint32_t reg_spi_wakeup_int_clr:1;
        /** reg_slv_buf_addr_err_int_clr : WT; bitpos: [15]; default: 0;
         *  The clear bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
         */
        uint32_t reg_slv_buf_addr_err_int_clr:1;
        /** reg_slv_cmd_err_int_clr : WT; bitpos: [16]; default: 0;
         *  The clear bit for SPI_SLV_CMD_ERR_INT interrupt.
         */
        uint32_t reg_slv_cmd_err_int_clr:1;
        /** reg_mst_rx_afifo_wfull_err_int_clr : WT; bitpos: [17]; default: 0;
         *  The clear bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
         */
        uint32_t reg_mst_rx_afifo_wfull_err_int_clr:1;
        /** reg_mst_tx_afifo_rempty_err_int_clr : WT; bitpos: [18]; default: 0;
         *  The clear bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
         */
        uint32_t reg_mst_tx_afifo_rempty_err_int_clr:1;
        /** reg_app2_int_clr : WT; bitpos: [19]; default: 0;
         *  The clear bit for SPI_APP2_INT interrupt.
         */
        uint32_t reg_app2_int_clr:1;
        /** reg_app1_int_clr : WT; bitpos: [20]; default: 0;
         *  The clear bit for SPI_APP1_INT interrupt.
         */
        uint32_t reg_app1_int_clr:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} lp_spi_dma_int_clr_reg_t;


/** Group: LP SPI DMA INT RAW REG */
/** Type of spi_dma_int_raw register
 *  SPI DMA interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** reg_slv_rd_buf_done_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw bit for SPI_SLV_RD_BUF_DONE_INT interrupt. 1: SPI slave mode Rd_BUF
         *  transmission is ended. 0: Others.
         */
        uint32_t reg_slv_rd_buf_done_int_raw:1;
        /** reg_slv_wr_buf_done_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw bit for SPI_SLV_WR_BUF_DONE_INT interrupt. 1: SPI slave mode Wr_BUF
         *  transmission is ended. 0: Others.
         */
        uint32_t reg_slv_wr_buf_done_int_raw:1;
        /** reg_trans_done_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw bit for SPI_TRANS_DONE_INT interrupt. 1: SPI master mode transmission is
         *  ended. 0: others.
         */
        uint32_t reg_trans_done_int_raw:1;
        uint32_t reserved_13:1;
        /** reg_spi_wakeup_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The raw bit for SPI_SPI_WAKEUP_INT interrupt. 1: There is a  wake up signal when
         *  low power mode. 0: Others.
         */
        uint32_t reg_spi_wakeup_int_raw:1;
        /** reg_slv_buf_addr_err_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  The raw bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt. 1: The accessing data address
         *  of the current SPI slave mode CPU controlled FD, Wr_BUF or Rd_BUF transmission is
         *  bigger than 63. 0: Others.
         */
        uint32_t reg_slv_buf_addr_err_int_raw:1;
        /** reg_slv_cmd_err_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  The raw bit for SPI_SLV_CMD_ERR_INT interrupt. 1: The slave command value in the
         *  current SPI slave HD mode transmission is not supported. 0: Others.
         */
        uint32_t reg_slv_cmd_err_int_raw:1;
        /** reg_mst_rx_afifo_wfull_err_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  The raw bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt. 1: There is a RX AFIFO
         *  write-full error when SPI inputs data in master mode. 0: Others.
         */
        uint32_t reg_mst_rx_afifo_wfull_err_int_raw:1;
        /** reg_mst_tx_afifo_rempty_err_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  The raw bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt. 1: There is a TX BUF
         *  AFIFO read-empty error when SPI outputs data in master mode. 0: Others.
         */
        uint32_t reg_mst_tx_afifo_rempty_err_int_raw:1;
        /** reg_app2_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  The raw bit for SPI_APP2_INT interrupt. The value is only controlled by application.
         */
        uint32_t reg_app2_int_raw:1;
        /** reg_app1_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  The raw bit for SPI_APP1_INT interrupt. The value is only controlled by application.
         */
        uint32_t reg_app1_int_raw:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} lp_spi_dma_int_raw_reg_t;


/** Group: LP SPI DMA INT ST REG */
/** Type of spi_dma_int_st register
 *  SPI DMA interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** reg_slv_rd_buf_done_int_st : RO; bitpos: [10]; default: 0;
         *  The status bit for SPI_SLV_RD_BUF_DONE_INT interrupt.
         */
        uint32_t reg_slv_rd_buf_done_int_st:1;
        /** reg_slv_wr_buf_done_int_st : RO; bitpos: [11]; default: 0;
         *  The status bit for SPI_SLV_WR_BUF_DONE_INT interrupt.
         */
        uint32_t reg_slv_wr_buf_done_int_st:1;
        /** reg_trans_done_int_st : RO; bitpos: [12]; default: 0;
         *  The status bit for SPI_TRANS_DONE_INT interrupt.
         */
        uint32_t reg_trans_done_int_st:1;
        uint32_t reserved_13:1;
        /** reg_spi_wakeup_int_st : RO; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t reg_spi_wakeup_int_st:1;
        /** reg_slv_buf_addr_err_int_st : RO; bitpos: [15]; default: 0;
         *  The status bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
         */
        uint32_t reg_slv_buf_addr_err_int_st:1;
        /** reg_slv_cmd_err_int_st : RO; bitpos: [16]; default: 0;
         *  The status bit for SPI_SLV_CMD_ERR_INT interrupt.
         */
        uint32_t reg_slv_cmd_err_int_st:1;
        /** reg_mst_rx_afifo_wfull_err_int_st : RO; bitpos: [17]; default: 0;
         *  The status bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
         */
        uint32_t reg_mst_rx_afifo_wfull_err_int_st:1;
        /** reg_mst_tx_afifo_rempty_err_int_st : RO; bitpos: [18]; default: 0;
         *  The status bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
         */
        uint32_t reg_mst_tx_afifo_rempty_err_int_st:1;
        /** reg_app2_int_st : RO; bitpos: [19]; default: 0;
         *  The status bit for SPI_APP2_INT interrupt.
         */
        uint32_t reg_app2_int_st:1;
        /** reg_app1_int_st : RO; bitpos: [20]; default: 0;
         *  The status bit for SPI_APP1_INT interrupt.
         */
        uint32_t reg_app1_int_st:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} lp_spi_dma_int_st_reg_t;


/** Group: Interrupt registers */
/** Type of spi_dma_int_set register
 *  SPI interrupt software set register
 */
typedef union {
    struct {
        uint32_t reserved_0:10;
        /** spi_slv_rd_buf_done_int_set : WT; bitpos: [10]; default: 0;
         *  The software set bit for SPI_SLV_RD_BUF_DONE_INT interrupt.
         */
        uint32_t spi_slv_rd_buf_done_int_set:1;
        /** spi_slv_wr_buf_done_int_set : WT; bitpos: [11]; default: 0;
         *  The software set bit for SPI_SLV_WR_BUF_DONE_INT interrupt.
         */
        uint32_t spi_slv_wr_buf_done_int_set:1;
        /** spi_trans_done_int_set : WT; bitpos: [12]; default: 0;
         *  The software set bit for SPI_TRANS_DONE_INT interrupt.
         */
        uint32_t spi_trans_done_int_set:1;
        uint32_t reserved_13:2;
        /** spi_slv_buf_addr_err_int_set : WT; bitpos: [15]; default: 0;
         *  The software set bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.
         */
        uint32_t spi_slv_buf_addr_err_int_set:1;
        /** spi_slv_cmd_err_int_set : WT; bitpos: [16]; default: 0;
         *  The software set bit for SPI_SLV_CMD_ERR_INT interrupt.
         */
        uint32_t spi_slv_cmd_err_int_set:1;
        /** spi_mst_rx_afifo_wfull_err_int_set : WT; bitpos: [17]; default: 0;
         *  The software set bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.
         */
        uint32_t spi_mst_rx_afifo_wfull_err_int_set:1;
        /** spi_mst_tx_afifo_rempty_err_int_set : WT; bitpos: [18]; default: 0;
         *  The software set bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.
         */
        uint32_t spi_mst_tx_afifo_rempty_err_int_set:1;
        /** spi_app2_int_set : WT; bitpos: [19]; default: 0;
         *  The software set bit for SPI_APP2_INT interrupt.
         */
        uint32_t spi_app2_int_set:1;
        /** spi_app1_int_set : WT; bitpos: [20]; default: 0;
         *  The software set bit for SPI_APP1_INT interrupt.
         */
        uint32_t spi_app1_int_set:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} lp_spi_dma_int_set_reg_t;


/** Group: LP SPI SLEEP CONF0 REG */
/** Type of spi_sleep_conf0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_slv_wk_char0 : R/W; bitpos: [7:0]; default: 10;
         *  NA
         */
        uint32_t reg_slv_wk_char0:8;
        /** reg_slv_wk_char_num : R/W; bitpos: [10:8]; default: 0;
         *  NA
         */
        uint32_t reg_slv_wk_char_num:3;
        /** reg_slv_wk_char_mask : R/W; bitpos: [15:11]; default: 0;
         *  NA
         */
        uint32_t reg_slv_wk_char_mask:5;
        /** reg_slv_wk_mode_sel : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t reg_slv_wk_mode_sel:1;
        /** reg_sleep_en : R/W; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t reg_sleep_en:1;
        /** reg_sleep_dis_rxfifo_wr_en : R/W; bitpos: [18]; default: 0;
         *  NA
         */
        uint32_t reg_sleep_dis_rxfifo_wr_en:1;
        /** reg_sleep_wk_data_sel : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t reg_sleep_wk_data_sel:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} lp_spi_sleep_conf0_reg_t;


/** Group: LP SPI SLEEP CONF1 REG */
/** Type of spi_sleep_conf1 register
 *  NA
 */
typedef union {
    struct {
        /** reg_slv_wk_char1 : R/W; bitpos: [7:0]; default: 11;
         *  NA
         */
        uint32_t reg_slv_wk_char1:8;
        /** reg_slv_wk_char2 : R/W; bitpos: [15:8]; default: 12;
         *  NA
         */
        uint32_t reg_slv_wk_char2:8;
        /** reg_slv_wk_char3 : R/W; bitpos: [23:16]; default: 13;
         *  NA
         */
        uint32_t reg_slv_wk_char3:8;
        /** reg_slv_wk_char4 : R/W; bitpos: [31:24]; default: 14;
         *  NA
         */
        uint32_t reg_slv_wk_char4:8;
    };
    uint32_t val;
} lp_spi_sleep_conf1_reg_t;


/** Group: LP SPI Wn REG */
/** Type of spi_wn register
 *  SPI CPU-controlled buffer
 */
typedef union {
    struct {
        /** reg_buf : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t reg_buf:32;
    };
    uint32_t val;
} lp_spi_wn_reg_t;


/** Group: LP SPI SLAVE REG */
/** Type of spi_slave register
 *  SPI slave control register
 */
typedef union {
    struct {
        /** reg_clk_mode : R/W; bitpos: [1:0]; default: 0;
         *  SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed
         *  one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3:
         *  SPI clock is always on. Can be configured in CONF state.
         */
        uint32_t reg_clk_mode:2;
        /** reg_clk_mode_13 : R/W; bitpos: [2]; default: 0;
         *  {CPOL, CPHA},1: support spi clk mode 1 and 3, first edge output data B[0]/B[7].  0:
         *  support spi clk mode 0 and 2, first edge output data B[1]/B[6].
         */
        uint32_t reg_clk_mode_13:1;
        /** reg_rsck_data_out : R/W; bitpos: [3]; default: 0;
         *  It saves half a cycle when tsck is the same as rsck. 1: output data at rsck posedge
         *  0: output data at tsck posedge
         */
        uint32_t reg_rsck_data_out:1;
        uint32_t reserved_4:6;
        /** reg_slv_rdbuf_bitlen_en : R/W; bitpos: [10]; default: 0;
         *  1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length in
         *  CPU controlled mode(Rd_BUF). 0: others
         */
        uint32_t reg_slv_rdbuf_bitlen_en:1;
        /** reg_slv_wrbuf_bitlen_en : R/W; bitpos: [11]; default: 0;
         *  1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data length
         *  in CPU controlled mode(Wr_BUF). 0: others
         */
        uint32_t reg_slv_wrbuf_bitlen_en:1;
        uint32_t reserved_12:14;
        /** reg_slave_mode : R/W; bitpos: [26]; default: 0;
         *  Set SPI work mode. 1: slave mode 0: master mode.
         */
        uint32_t reg_slave_mode:1;
        /** reg_soft_reset : WT; bitpos: [27]; default: 0;
         *  Software reset enable, reset the spi clock line cs line and data lines. Can be
         *  configured in CONF state.
         */
        uint32_t reg_soft_reset:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_spi_slave_reg_t;


/** Group: LP SPI SLAVE1 REG */
/** Type of spi_slave1 register
 *  SPI slave control register 1
 */
typedef union {
    struct {
        /** reg_slv_data_bitlen : R/W/SS; bitpos: [17:0]; default: 0;
         *  The transferred data bit length in SPI slave FD and HD mode.
         */
        uint32_t reg_slv_data_bitlen:18;
        /** reg_slv_last_command : R/W/SS; bitpos: [25:18]; default: 0;
         *  In the slave mode it is the value of command.
         */
        uint32_t reg_slv_last_command:8;
        /** reg_slv_last_addr : R/W/SS; bitpos: [31:26]; default: 0;
         *  In the slave mode it is the value of address.
         */
        uint32_t reg_slv_last_addr:6;
    };
    uint32_t val;
} lp_spi_slave1_reg_t;


/** Group: LP SPI CLK GATE REG */
/** Type of spi_clk_gate register
 *  SPI module clock and register clock control
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable clk gate
         */
        uint32_t reg_clk_en:1;
        /** reg_mst_clk_active : R/W; bitpos: [1]; default: 0;
         *  Set this bit to power on the SPI module clock.
         */
        uint32_t reg_mst_clk_active:1;
        /** reg_mst_clk_sel : R/W; bitpos: [2]; default: 0;
         *  This bit is used to select SPI module clock source in master mode. 1: PLL_CLK_80M.
         *  0: XTAL CLK.
         */
        uint32_t reg_mst_clk_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_spi_clk_gate_reg_t;


/** Group: LP SPI DATE REG */
/** Type of spi_date register
 *  Version control
 */
typedef union {
    struct {
        /** reg_date : R/W; bitpos: [27:0]; default: 33591360;
         *  SPI register version.
         */
        uint32_t reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_spi_date_reg_t;


/** Group: LP RND ECO CS REG */
/** Type of rnd_eco_cs register
 *  NA
 */
typedef union {
    struct {
        /** reg_rnd_eco_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_rnd_eco_en:1;
        /** rnd_eco_result : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t rnd_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_rnd_eco_cs_reg_t;


/** Group: LP RND ECO LOW REG */
/** Type of rnd_eco_low register
 *  NA
 */
typedef union {
    struct {
        /** reg_rnd_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t reg_rnd_eco_low:32;
    };
    uint32_t val;
} lp_rnd_eco_low_reg_t;


/** Group: LP RND ECO HIGH REG */
/** Type of rnd_eco_high register
 *  NA
 */
typedef union {
    struct {
        /** reg_rnd_eco_high : R/W; bitpos: [31:0]; default: 65535;
         *  NA
         */
        uint32_t reg_rnd_eco_high:32;
    };
    uint32_t val;
} lp_rnd_eco_high_reg_t;


typedef struct {
    volatile lp_spi_cmd_reg_t spi_cmd;
    volatile lp_spi_addr_reg_t spi_addr;
    volatile lp_spi_ctrl_reg_t spi_ctrl;
    volatile lp_spi_clock_reg_t spi_clock;
    volatile lp_spi_user_reg_t spi_user;
    volatile lp_spi_user1_reg_t spi_user1;
    volatile lp_spi_user2_reg_t spi_user2;
    volatile lp_spi_ms_dlen_reg_t spi_ms_dlen;
    volatile lp_spi_misc_reg_t spi_misc;
    volatile lp_spi_din_mode_reg_t spi_din_mode;
    volatile lp_spi_din_num_reg_t spi_din_num;
    volatile lp_spi_dout_mode_reg_t spi_dout_mode;
    volatile lp_spi_dma_conf_reg_t spi_dma_conf;
    volatile lp_spi_dma_int_ena_reg_t spi_dma_int_ena;
    volatile lp_spi_dma_int_clr_reg_t spi_dma_int_clr;
    volatile lp_spi_dma_int_raw_reg_t spi_dma_int_raw;
    volatile lp_spi_dma_int_st_reg_t spi_dma_int_st;
    volatile lp_spi_sleep_conf0_reg_t spi_sleep_conf0;
    volatile lp_spi_sleep_conf1_reg_t spi_sleep_conf1;
    volatile lp_spi_dma_int_set_reg_t spi_dma_int_set;
    uint32_t reserved_050[18];
    volatile lp_spi_wn_reg_t data_buf[16];
    uint32_t reserved_0d8[2];
    volatile lp_spi_slave_reg_t spi_slave;
    volatile lp_spi_slave1_reg_t spi_slave1;
    volatile lp_spi_clk_gate_reg_t spi_clk_gate;
    uint32_t reserved_0ec;
    volatile lp_spi_date_reg_t spi_date;
    volatile lp_rnd_eco_cs_reg_t rnd_eco_cs;
    volatile lp_rnd_eco_low_reg_t rnd_eco_low;
    volatile lp_rnd_eco_high_reg_t rnd_eco_high;
} lp_spi_dev_t;

extern lp_spi_dev_t LP_SPI;

#ifndef __cplusplus
_Static_assert(sizeof(lp_spi_dev_t) == 0x100, "Invalid size of lp_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
