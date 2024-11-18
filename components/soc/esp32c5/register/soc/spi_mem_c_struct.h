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

/** Group: Status and state control register */
/** Type of mem_cmd register
 *  SPI0 FSM status register
 */
typedef union {
    struct {
        /** mem_mst_st : RO; bitpos: [3:0]; default: 0;
         *  The current status of SPI0 master FSM: spi0_mst_st. 0: idle state, 1:SPI0_GRANT ,
         *  2: program/erase suspend state, 3: SPI0 read data state, 4: wait cache/EDMA sent
         *  data is stored in SPI0 TX FIFO, 5: SPI0 write data state.
         */
        uint32_t mem_mst_st:4;
        /** mem_slv_st : RO; bitpos: [7:4]; default: 0;
         *  The current status of SPI0 slave FSM: mspi_st. 0: idle state, 1: preparation state,
         *  2: send command state, 3: send address state, 4: wait state, 5: read data state,
         *  6:write data state, 7: done state, 8: read data end state.
         */
        uint32_t mem_slv_st:4;
        uint32_t reserved_8:10;
        /** mem_usr : HRO; bitpos: [18]; default: 0;
         *  SPI0 USR_CMD start bit, only used when spi_mem_c_AXI_REQ_EN is cleared.  An operation
         *  will be triggered when the bit is set. The bit will be cleared once the operation
         *  done.1: enable 0: disable.
         */
        uint32_t mem_usr:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} spi_mem_c_cmd_reg_t;

/** Type of mem_axi_err_addr register
 *  SPI0 AXI request error address.
 */
typedef union {
    struct {
        /** mem_axi_err_addr : R/SS/WTC; bitpos: [28:0]; default: 0;
         *  This bits show the first AXI write/read invalid error or AXI write flash error
         *  address. It is cleared by when spi_mem_c_AXI_WADDR_ERR_INT_CLR,
         *  spi_mem_c_AXI_WR_FLASH_ERR_IN_CLR or spi_mem_c_AXI_RADDR_ERR_IN_CLR bit is set.
         */
        uint32_t mem_axi_err_addr:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} spi_mem_c_axi_err_addr_reg_t;


/** Group: Flash Control and configuration registers */
/** Type of mem_ctrl register
 *  SPI0 control register.
 */
typedef union {
    struct {
        /** mem_wdummy_dqs_always_out : R/W; bitpos: [0]; default: 0;
         *  In the dummy phase of an MSPI write data transfer when accesses to flash, the level
         *  of SPI_DQS is output by the MSPI controller.
         */
        uint32_t mem_wdummy_dqs_always_out:1;
        /** mem_wdummy_always_out : R/W; bitpos: [1]; default: 0;
         *  In the dummy phase of an MSPI write data transfer when accesses to flash, the level
         *  of SPI_IO[7:0] is output by the MSPI controller.
         */
        uint32_t mem_wdummy_always_out:1;
        /** mem_fdummy_rin : R/W; bitpos: [2]; default: 1;
         *  In an MSPI read data transfer when accesses to flash, the level of SPI_IO[7:0] is
         *  output by the MSPI controller in the first half part of dummy phase. It is used to
         *  mask invalid SPI_DQS in the half part of dummy phase.
         */
        uint32_t mem_fdummy_rin:1;
        /** mem_fdummy_wout : R/W; bitpos: [3]; default: 1;
         *  In an MSPI write data transfer when accesses to flash, the level of SPI_IO[7:0] is
         *  output by the MSPI controller in the second half part of dummy phase. It is used to
         *  pre-drive flash.
         */
        uint32_t mem_fdummy_wout:1;
        /** mem_fdout_oct : R/W; bitpos: [4]; default: 0;
         *  Apply 8 signals during write-data phase 1:enable 0: disable
         */
        uint32_t mem_fdout_oct:1;
        /** mem_fdin_oct : R/W; bitpos: [5]; default: 0;
         *  Apply 8 signals during read-data phase 1:enable 0: disable
         */
        uint32_t mem_fdin_oct:1;
        /** mem_faddr_oct : R/W; bitpos: [6]; default: 0;
         *  Apply 8 signals during address phase 1:enable 0: disable
         */
        uint32_t mem_faddr_oct:1;
        uint32_t reserved_7:1;
        /** mem_fcmd_quad : R/W; bitpos: [8]; default: 0;
         *  Apply 4 signals during command phase 1:enable 0: disable
         */
        uint32_t mem_fcmd_quad:1;
        /** mem_fcmd_oct : R/W; bitpos: [9]; default: 0;
         *  Apply 8 signals during command phase 1:enable 0: disable
         */
        uint32_t mem_fcmd_oct:1;
        uint32_t reserved_10:3;
        /** mem_fastrd_mode : R/W; bitpos: [13]; default: 1;
         *  This bit enable the bits: spi_mem_c_FREAD_QIO, spi_mem_c_FREAD_DIO, spi_mem_c_FREAD_QOUT
         *  and spi_mem_c_FREAD_DOUT. 1: enable 0: disable.
         */
        uint32_t mem_fastrd_mode:1;
        /** mem_fread_dual : R/W; bitpos: [14]; default: 0;
         *  In the read operations, read-data phase apply 2 signals. 1: enable 0: disable.
         */
        uint32_t mem_fread_dual:1;
        uint32_t reserved_15:3;
        /** mem_q_pol : R/W; bitpos: [18]; default: 1;
         *  The bit is used to set MISO line polarity, 1: high 0, low
         */
        uint32_t mem_q_pol:1;
        /** mem_d_pol : R/W; bitpos: [19]; default: 1;
         *  The bit is used to set MOSI line polarity, 1: high 0, low
         */
        uint32_t mem_d_pol:1;
        /** mem_fread_quad : R/W; bitpos: [20]; default: 0;
         *  In the read operations read-data phase apply 4 signals. 1: enable 0: disable.
         */
        uint32_t mem_fread_quad:1;
        /** mem_wp_reg : R/W; bitpos: [21]; default: 1;
         *  Write protect signal output when SPI is idle.  1: output high, 0: output low.
         */
        uint32_t mem_wp_reg:1;
        uint32_t reserved_22:1;
        /** mem_fread_dio : R/W; bitpos: [23]; default: 0;
         *  In the read operations address phase and read-data phase apply 2 signals. 1: enable
         *  0: disable.
         */
        uint32_t mem_fread_dio:1;
        /** mem_fread_qio : R/W; bitpos: [24]; default: 0;
         *  In the read operations address phase and read-data phase apply 4 signals. 1: enable
         *  0: disable.
         */
        uint32_t mem_fread_qio:1;
        uint32_t reserved_25:5;
        /** mem_dqs_ie_always_on : R/W; bitpos: [30]; default: 0;
         *  When accesses to flash, 1: the IE signals of pads connected to SPI_DQS are always
         *  1. 0: Others.
         */
        uint32_t mem_dqs_ie_always_on:1;
        /** mem_data_ie_always_on : R/W; bitpos: [31]; default: 1;
         *  When accesses to flash, 1: the IE signals of pads connected to SPI_IO[7:0] are
         *  always 1. 0: Others.
         */
        uint32_t mem_data_ie_always_on:1;
    };
    uint32_t val;
} spi_mem_c_ctrl_reg_t;

/** Type of mem_ctrl1 register
 *  SPI0 control1 register.
 */
typedef union {
    struct {
        /** mem_clk_mode : R/W; bitpos: [1:0]; default: 0;
         *  SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed
         *  one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3:
         *  SPI clock is always on.
         */
        uint32_t mem_clk_mode:2;
        uint32_t reserved_2:19;
        /** ar_size0_1_support_en : R/W; bitpos: [21]; default: 1;
         *  1: MSPI supports ARSIZE 0~3. When ARSIZE =0~2, MSPI read address is 4*n and reply
         *  the real AXI read data back. 0: When ARSIZE 0~1, MSPI reply SLV_ERR.
         */
        uint32_t ar_size0_1_support_en:1;
        /** aw_size0_1_support_en : R/W; bitpos: [22]; default: 1;
         *  1: MSPI supports AWSIZE 0~3. 0: When AWSIZE 0~1, MSPI reply SLV_ERR.
         */
        uint32_t aw_size0_1_support_en:1;
        /** axi_rdata_back_fast : R/W; bitpos: [23]; default: 1;
         *  1: Reply AXI read data to AXI bus when one AXI read beat data is available. 0:
         *  Reply AXI read data to AXI bus when all the read data is available.
         */
        uint32_t axi_rdata_back_fast:1;
        /** mem_rresp_ecc_err_en : R/W; bitpos: [24]; default: 0;
         *  1: RRESP is SLV_ERR when there is a ECC error in AXI read data. 0: RRESP is OKAY
         *  when there is a ECC error in AXI read data. The ECC error information is recorded
         *  in spi_mem_c_ECC_ERR_ADDR_REG.
         */
        uint32_t mem_rresp_ecc_err_en:1;
        /** mem_ar_splice_en : R/W; bitpos: [25]; default: 0;
         *  Set this bit to enable AXI Read Splice-transfer.
         */
        uint32_t mem_ar_splice_en:1;
        /** mem_aw_splice_en : R/W; bitpos: [26]; default: 0;
         *  Set this bit to enable AXI Write Splice-transfer.
         */
        uint32_t mem_aw_splice_en:1;
        /** mem_ram0_en : HRO; bitpos: [27]; default: 1;
         *  When spi_mem_c_DUAL_RAM_EN is 0 and spi_mem_c_RAM0_EN is 1, only EXT_RAM0 will be
         *  accessed. When spi_mem_c_DUAL_RAM_EN is 0 and spi_mem_c_RAM0_EN is 0, only EXT_RAM1
         *  will be accessed. When spi_mem_c_DUAL_RAM_EN is 1,  EXT_RAM0 and EXT_RAM1 will be
         *  accessed at the same time.
         */
        uint32_t mem_ram0_en:1;
        /** mem_dual_ram_en : HRO; bitpos: [28]; default: 0;
         *  Set this bit to enable DUAL-RAM mode, EXT_RAM0 and EXT_RAM1 will be accessed at the
         *  same time.
         */
        uint32_t mem_dual_ram_en:1;
        /** mem_fast_write_en : R/W; bitpos: [29]; default: 1;
         *  Set this bit to write data faster, do not wait write data has been stored in
         *  tx_bus_fifo_l2. It will wait 4*T_clk_ctrl to insure the write data has been stored
         *  in  tx_bus_fifo_l2.
         */
        uint32_t mem_fast_write_en:1;
        /** mem_rxfifo_rst : WT; bitpos: [30]; default: 0;
         *  The synchronous reset signal for SPI0 RX AFIFO and all the AES_MSPI SYNC FIFO to
         *  receive signals from AXI.  Set this bit to reset these FIFO.
         */
        uint32_t mem_rxfifo_rst:1;
        /** mem_txfifo_rst : WT; bitpos: [31]; default: 0;
         *  The synchronous reset signal for SPI0 TX AFIFO and all the AES_MSPI SYNC FIFO to
         *  send signals to AXI. Set this bit to reset these FIFO.
         */
        uint32_t mem_txfifo_rst:1;
    };
    uint32_t val;
} spi_mem_c_ctrl1_reg_t;

/** Type of mem_ctrl2 register
 *  SPI0 control2 register.
 */
typedef union {
    struct {
        /** mem_cs_setup_time : R/W; bitpos: [4:0]; default: 1;
         *  (cycles-1) of prepare phase by SPI Bus clock, this bits are combined with
         *  spi_mem_c_CS_SETUP bit.
         */
        uint32_t mem_cs_setup_time:5;
        /** mem_cs_hold_time : R/W; bitpos: [9:5]; default: 1;
         *  SPI CS signal is delayed to inactive by SPI bus clock, this bits are combined with
         *  spi_mem_c_CS_HOLD bit.
         */
        uint32_t mem_cs_hold_time:5;
        /** mem_ecc_cs_hold_time : R/W; bitpos: [12:10]; default: 3;
         *  spi_mem_c_CS_HOLD_TIME + spi_mem_c_ECC_CS_HOLD_TIME is the SPI0 CS hold cycle in ECC
         *  mode when accessed flash.
         */
        uint32_t mem_ecc_cs_hold_time:3;
        /** mem_ecc_skip_page_corner : R/W; bitpos: [13]; default: 1;
         *  1: SPI0 and SPI1 skip page corner when accesses flash. 0: Not skip page corner when
         *  accesses flash.
         */
        uint32_t mem_ecc_skip_page_corner:1;
        /** mem_ecc_16to18_byte_en : R/W; bitpos: [14]; default: 0;
         *  Set this bit to enable SPI0 and SPI1 ECC 16 bytes data with 2 ECC bytes mode when
         *  accesses flash.
         */
        uint32_t mem_ecc_16to18_byte_en:1;
        uint32_t reserved_15:9;
        /** mem_split_trans_en : R/W; bitpos: [24]; default: 0;
         *  Set this bit to enable SPI0 split one AXI read flash transfer into two SPI
         *  transfers when one transfer will cross flash or EXT_RAM page corner, valid no
         *  matter whether there is an ECC region or not.
         */
        uint32_t mem_split_trans_en:1;
        /** mem_cs_hold_delay : R/W; bitpos: [30:25]; default: 0;
         *  These bits are used to set the minimum CS high time tSHSL between SPI burst
         *  transfer when accesses to flash. tSHSL is (spi_mem_c_CS_HOLD_DELAY[5:0] + 1) MSPI
         *  core clock cycles.
         */
        uint32_t mem_cs_hold_delay:6;
        /** mem_sync_reset : WT; bitpos: [31]; default: 0;
         *  The spi0_mst_st and spi0_slv_st will be reset.
         */
        uint32_t sync_reset:1;
    };
    uint32_t val;
} spi_mem_c_ctrl2_reg_t;

/** Type of mem_misc register
 *  SPI0 misc register
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** mem_fsub_pin : HRO; bitpos: [7]; default: 0;
         *  For SPI0,  flash is connected to SUBPINs.
         */
        uint32_t mem_fsub_pin:1;
        /** mem_ssub_pin : HRO; bitpos: [8]; default: 0;
         *  For SPI0,  sram is connected to SUBPINs.
         */
        uint32_t mem_ssub_pin:1;
        /** mem_ck_idle_edge : R/W; bitpos: [9]; default: 0;
         *  1: SPI_CLK line is high when idle     0: spi clk line is low when idle
         */
        uint32_t mem_ck_idle_edge:1;
        /** mem_cs_keep_active : R/W; bitpos: [10]; default: 0;
         *  SPI_CS line keep low when the bit is set.
         */
        uint32_t mem_cs_keep_active:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} spi_mem_c_misc_reg_t;

/** Type of mem_cache_fctrl register
 *  SPI0 bit mode control register.
 */
typedef union {
    struct {
        /** mem_axi_req_en : R/W; bitpos: [0]; default: 0;
         *  For SPI0, AXI master access enable, 1: enable, 0:disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_axi_req_en:1;
        /** mem_cache_usr_addr_4byte : R/W; bitpos: [1]; default: 0;
         *  For SPI0,  cache  read flash with 4 bytes address, 1: enable, 0:disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_cache_usr_addr_4byte:1;
        /** mem_cache_flash_usr_cmd : R/W; bitpos: [2]; default: 0;
         *  For SPI0,  cache  read flash for user define command, 1: enable, 0:disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_cache_flash_usr_cmd:1;
        /** mem_fdin_dual : R/W; bitpos: [3]; default: 0;
         *  For SPI0 flash, din phase apply 2 signals. 1: enable 0: disable. The bit is the
         *  same with spi_mem_c_fread_dio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_fdin_dual:1;
        /** mem_fdout_dual : R/W; bitpos: [4]; default: 0;
         *  For SPI0 flash, dout phase apply 2 signals. 1: enable 0: disable. The bit is the
         *  same with spi_mem_c_fread_dio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_fdout_dual:1;
        /** mem_faddr_dual : R/W; bitpos: [5]; default: 0;
         *  For SPI0 flash, address phase apply 2 signals. 1: enable 0: disable.  The bit is
         *  the same with spi_mem_c_fread_dio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_faddr_dual:1;
        /** mem_fdin_quad : R/W; bitpos: [6]; default: 0;
         *  For SPI0 flash, din phase apply 4 signals. 1: enable 0: disable.  The bit is the
         *  same with spi_mem_c_fread_qio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_fdin_quad:1;
        /** mem_fdout_quad : R/W; bitpos: [7]; default: 0;
         *  For SPI0 flash, dout phase apply 4 signals. 1: enable 0: disable.  The bit is the
         *  same with spi_mem_c_fread_qio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_fdout_quad:1;
        /** mem_faddr_quad : R/W; bitpos: [8]; default: 0;
         *  For SPI0 flash, address phase apply 4 signals. 1: enable 0: disable.  The bit is
         *  the same with spi_mem_c_fread_qio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_faddr_quad:1;
        uint32_t reserved_9:21;
        /** same_aw_ar_addr_chk_en : R/W; bitpos: [30]; default: 1;
         *  Set this bit to check AXI read/write the same address region.
         */
        uint32_t same_aw_ar_addr_chk_en:1;
        /** close_axi_inf_en : R/W; bitpos: [31]; default: 1;
         *  Set this bit to close AXI read/write transfer to MSPI, which means that only
         *  SLV_ERR will be replied to BRESP/RRESP.
         */
        uint32_t close_axi_inf_en:1;
    };
    uint32_t val;
} spi_mem_c_cache_fctrl_reg_t;

/** Type of mem_ddr register
 *  SPI0 flash DDR mode control register
 */
typedef union {
    struct {
        /** fmem_ddr_en : R/W; bitpos: [0]; default: 0;
         *  1: in DDR mode,  0 in SDR mode
         */
        uint32_t fmem_ddr_en:1;
        /** fmem_var_dummy : R/W; bitpos: [1]; default: 0;
         *  Set the bit to enable variable dummy cycle in spi DDR mode.
         */
        uint32_t fmem_var_dummy:1;
        /** fmem_ddr_rdat_swp : R/W; bitpos: [2]; default: 0;
         *  Set the bit to reorder rx data of the word in spi DDR mode.
         */
        uint32_t fmem_ddr_rdat_swp:1;
        /** fmem_ddr_wdat_swp : R/W; bitpos: [3]; default: 0;
         *  Set the bit to reorder tx data of the word in spi DDR mode.
         */
        uint32_t fmem_ddr_wdat_swp:1;
        /** fmem_ddr_cmd_dis : R/W; bitpos: [4]; default: 0;
         *  the bit is used to disable dual edge in command phase when DDR mode.
         */
        uint32_t fmem_ddr_cmd_dis:1;
        /** fmem_outminbytelen : R/W; bitpos: [11:5]; default: 1;
         *  It is the minimum output data length in the panda device.
         */
        uint32_t fmem_outminbytelen:7;
        /** fmem_tx_ddr_msk_en : R/W; bitpos: [12]; default: 1;
         *  Set this bit to mask the first or the last byte in SPI0 ECC DDR write mode, when
         *  accesses to flash.
         */
        uint32_t fmem_tx_ddr_msk_en:1;
        /** fmem_rx_ddr_msk_en : R/W; bitpos: [13]; default: 1;
         *  Set this bit to mask the first or the last byte in SPI0 ECC DDR read mode, when
         *  accesses to flash.
         */
        uint32_t fmem_rx_ddr_msk_en:1;
        /** fmem_usr_ddr_dqs_thd : R/W; bitpos: [20:14]; default: 0;
         *  The delay number of data strobe which from memory based on SPI clock.
         */
        uint32_t fmem_usr_ddr_dqs_thd:7;
        /** fmem_ddr_dqs_loop : R/W; bitpos: [21]; default: 0;
         *  1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when
         *  spi0_slv_st is in spi_mem_c_DIN state. It is used when there is no SPI_DQS signal or
         *  SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and
         *  negative edge of SPI_DQS.
         */
        uint32_t fmem_ddr_dqs_loop:1;
        uint32_t reserved_22:2;
        /** fmem_clk_diff_en : R/W; bitpos: [24]; default: 0;
         *  Set this bit to enable the differential SPI_CLK#.
         */
        uint32_t fmem_clk_diff_en:1;
        uint32_t reserved_25:1;
        /** fmem_dqs_ca_in : R/W; bitpos: [26]; default: 0;
         *  Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.
         */
        uint32_t fmem_dqs_ca_in:1;
        /** fmem_hyperbus_dummy_2x : R/W; bitpos: [27]; default: 0;
         *  Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0
         *  accesses flash or SPI1 accesses flash or sram.
         */
        uint32_t fmem_hyperbus_dummy_2x:1;
        /** fmem_clk_diff_inv : R/W; bitpos: [28]; default: 0;
         *  Set this bit to invert SPI_DIFF when accesses to flash. .
         */
        uint32_t fmem_clk_diff_inv:1;
        /** fmem_octa_ram_addr : R/W; bitpos: [29]; default: 0;
         *  Set this bit to enable octa_ram address out when accesses to flash, which means
         *  ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.
         */
        uint32_t fmem_octa_ram_addr:1;
        /** fmem_hyperbus_ca : R/W; bitpos: [30]; default: 0;
         *  Set this bit to enable HyperRAM address out when accesses to flash, which means
         *  ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.
         */
        uint32_t fmem_hyperbus_ca:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} spi_mem_c_ddr_reg_t;


/** Group: Clock control and configuration registers */
/** Type of mem_clock register
 *  SPI clock division control register.
 */
typedef union {
    struct {
        /** mem_clkcnt_l : R/W; bitpos: [7:0]; default: 3;
         *  In the master mode it must be equal to spi_mem_c_clkcnt_N.
         */
        uint32_t mem_clkcnt_l:8;
        /** mem_clkcnt_h : R/W; bitpos: [15:8]; default: 1;
         *  In the master mode it must be floor((spi_mem_c_clkcnt_N+1)/2-1).
         */
        uint32_t mem_clkcnt_h:8;
        /** mem_clkcnt_n : R/W; bitpos: [23:16]; default: 3;
         *  In the master mode it is the divider of spi_mem_c_clk. So spi_mem_c_clk frequency is
         *  system/(spi_mem_c_clkcnt_N+1)
         */
        uint32_t mem_clkcnt_n:8;
        uint32_t reserved_24:7;
        /** mem_clk_equ_sysclk : R/W; bitpos: [31]; default: 0;
         *  1: 1-division mode, the frequency of SPI bus clock equals to that of MSPI module
         *  clock.
         */
        uint32_t mem_clk_equ_sysclk:1;
    };
    uint32_t val;
} spi_mem_c_clock_reg_t;

/** Type of mem_sram_clk register
 *  SPI0 external RAM clock control register
 */
typedef union {
    struct {
        /** mem_sclkcnt_l : R/W; bitpos: [7:0]; default: 3;
         *  For SPI0 external RAM  interface, it must be equal to spi_mem_c_clkcnt_N.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sclkcnt_l:8;
        /** mem_sclkcnt_h : R/W; bitpos: [15:8]; default: 1;
         *  For SPI0 external RAM  interface, it must be floor((spi_mem_c_clkcnt_N+1)/2-1).
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sclkcnt_h:8;
        /** mem_sclkcnt_n : R/W; bitpos: [23:16]; default: 3;
         *  For SPI0 external RAM  interface, it is the divider of spi_mem_c_clk. So spi_mem_c_clk
         *  frequency is system/(spi_mem_c_clkcnt_N+1)
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sclkcnt_n:8;
        uint32_t reserved_24:7;
        /** mem_sclk_equ_sysclk : R/W; bitpos: [31]; default: 0;
         *  For SPI0 external RAM  interface, 1: spi_mem_c_clk is equal to system 0: spi_mem_c_clk
         *  is divided from system clock.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sclk_equ_sysclk:1;
    };
    uint32_t val;
} spi_mem_c_sram_clk_reg_t;

/** Type of mem_clock_gate register
 *  SPI0 clock gate register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Register clock gate enable signal. 1: Enable. 0: Disable.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} spi_mem_c_clock_gate_reg_t;


/** Group: Flash User-defined control registers */
/** Type of mem_user register
 *  SPI0 user register.
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** mem_cs_hold : R/W; bitpos: [6]; default: 0;
         *  spi cs keep low when spi is in  done  phase. 1: enable 0: disable.
         */
        uint32_t mem_cs_hold:1;
        /** mem_cs_setup : R/W; bitpos: [7]; default: 0;
         *  spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable.
         */
        uint32_t mem_cs_setup:1;
        uint32_t reserved_8:1;
        /** mem_ck_out_edge : R/W; bitpos: [9]; default: 0;
         *  The bit combined with spi_mem_c_CK_IDLE_EDGE bit to control SPI clock mode 0~3.
         */
        uint32_t mem_ck_out_edge:1;
        uint32_t reserved_10:16;
        /** mem_usr_dummy_idle : R/W; bitpos: [26]; default: 0;
         *  spi clock is disable in dummy phase when the bit is enable.
         */
        uint32_t mem_usr_dummy_idle:1;
        uint32_t reserved_27:2;
        /** mem_usr_dummy : R/W; bitpos: [29]; default: 0;
         *  This bit enable the dummy phase of an operation.
         */
        uint32_t mem_usr_dummy:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} spi_mem_c_user_reg_t;

/** Type of mem_user1 register
 *  SPI0 user1 register.
 */
typedef union {
    struct {
        /** mem_usr_dummy_cyclelen : R/W; bitpos: [5:0]; default: 7;
         *  The length in spi_mem_c_clk cycles of dummy phase. The register value shall be
         *  (cycle_num-1).
         */
        uint32_t mem_usr_dummy_cyclelen:6;
        /** mem_usr_dbytelen : HRO; bitpos: [11:6]; default: 1;
         *  SPI0 USR_CMD read or write data byte length -1
         */
        uint32_t mem_usr_dbytelen:6;
        uint32_t reserved_12:14;
        /** mem_usr_addr_bitlen : R/W; bitpos: [31:26]; default: 23;
         *  The length in bits of address phase. The register value shall be (bit_num-1).
         */
        uint32_t mem_usr_addr_bitlen:6;
    };
    uint32_t val;
} spi_mem_c_user1_reg_t;

/** Type of mem_user2 register
 *  SPI0 user2 register.
 */
typedef union {
    struct {
        /** mem_usr_command_value : R/W; bitpos: [15:0]; default: 0;
         *  The value of  command.
         */
        uint32_t mem_usr_command_value:16;
        uint32_t reserved_16:12;
        /** mem_usr_command_bitlen : R/W; bitpos: [31:28]; default: 7;
         *  The length in bits of command phase. The register value shall be (bit_num-1)
         */
        uint32_t mem_usr_command_bitlen:4;
    };
    uint32_t val;
} spi_mem_c_user2_reg_t;

/** Type of mem_rd_status register
 *  SPI0 read control register.
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** mem_wb_mode : R/W; bitpos: [23:16]; default: 0;
         *  Mode bits in the flash fast read mode  it is combined with spi_mem_c_fastrd_mode bit.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_wb_mode:8;
        /** mem_wb_mode_bitlen : R/W; bitpos: [26:24]; default: 0;
         *  Mode bits length for flash fast read mode.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_wb_mode_bitlen:3;
        /** mem_wb_mode_en : R/W; bitpos: [27]; default: 0;
         *  Mode bits is valid while this bit is enable. 1: enable 0: disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_wb_mode_en:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} spi_mem_c_rd_status_reg_t;


/** Group: External RAM Control and configuration registers */
/** Type of mem_cache_sctrl register
 *  SPI0 external RAM control register
 */
typedef union {
    struct {
        /** mem_cache_usr_saddr_4byte : R/W; bitpos: [0]; default: 0;
         *  For SPI0, In the external RAM mode, cache read flash with 4 bytes command, 1:
         *  enable, 0:disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_cache_usr_saddr_4byte:1;
        /** mem_usr_sram_dio : R/W; bitpos: [1]; default: 0;
         *  For SPI0, In the external RAM mode, spi dual I/O mode enable, 1: enable, 0:disable
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_usr_sram_dio:1;
        /** mem_usr_sram_qio : R/W; bitpos: [2]; default: 0;
         *  For SPI0, In the external RAM mode, spi quad I/O mode enable, 1: enable, 0:disable
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_usr_sram_qio:1;
        /** mem_usr_wr_sram_dummy : R/W; bitpos: [3]; default: 0;
         *  For SPI0, In the external RAM mode, it is the enable bit of dummy phase for write
         *  operations.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_usr_wr_sram_dummy:1;
        /** mem_usr_rd_sram_dummy : R/W; bitpos: [4]; default: 1;
         *  For SPI0, In the external RAM mode, it is the enable bit of dummy phase for read
         *  operations.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_usr_rd_sram_dummy:1;
        /** mem_cache_sram_usr_rcmd : R/W; bitpos: [5]; default: 1;
         *  For SPI0, In the external RAM mode cache read external RAM for user define command.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_cache_sram_usr_rcmd:1;
        /** mem_sram_rdummy_cyclelen : R/W; bitpos: [11:6]; default: 1;
         *  For SPI0, In the external RAM mode, it is the length in bits of read dummy phase.
         *  The register value shall be (bit_num-1).
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sram_rdummy_cyclelen:6;
        uint32_t reserved_12:2;
        /** mem_sram_addr_bitlen : R/W; bitpos: [19:14]; default: 23;
         *  For SPI0, In the external RAM mode, it is the length in bits of address phase. The
         *  register value shall be (bit_num-1).
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sram_addr_bitlen:6;
        /** mem_cache_sram_usr_wcmd : R/W; bitpos: [20]; default: 1;
         *  For SPI0, In the external RAM mode cache write sram for user define command
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_cache_sram_usr_wcmd:1;
        /** mem_sram_oct : R/W; bitpos: [21]; default: 0;
         *  reserved
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sram_oct:1;
        /** mem_sram_wdummy_cyclelen : R/W; bitpos: [27:22]; default: 1;
         *  For SPI0, In the external RAM mode, it is the length in bits of write dummy phase.
         *  The register value shall be (bit_num-1).
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sram_wdummy_cyclelen:6;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} spi_mem_c_cache_sctrl_reg_t;

/** Type of mem_sram_cmd register
 *  SPI0 external RAM mode control register
 */
typedef union {
    struct {
        /** mem_sclk_mode : R/W; bitpos: [1:0]; default: 0;
         *  SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed
         *  one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3:
         *  SPI clock is always on.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sclk_mode:2;
        /** mem_swb_mode : R/W; bitpos: [9:2]; default: 0;
         *  Mode bits in the external RAM fast read mode  it is combined with
         *  spi_mem_c_fastrd_mode bit.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_swb_mode:8;
        /** mem_sdin_dual : R/W; bitpos: [10]; default: 0;
         *  For SPI0 external RAM , din phase apply 2 signals. 1: enable 0: disable. The bit is
         *  the same with spi_mem_c_usr_sram_dio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdin_dual:1;
        /** mem_sdout_dual : R/W; bitpos: [11]; default: 0;
         *  For SPI0 external RAM , dout phase apply 2 signals. 1: enable 0: disable. The bit
         *  is the same with spi_mem_c_usr_sram_dio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdout_dual:1;
        /** mem_saddr_dual : R/W; bitpos: [12]; default: 0;
         *  For SPI0 external RAM , address phase apply 2 signals. 1: enable 0: disable. The
         *  bit is the same with spi_mem_c_usr_sram_dio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_saddr_dual:1;
        uint32_t reserved_13:1;
        /** mem_sdin_quad : R/W; bitpos: [14]; default: 0;
         *  For SPI0 external RAM , din phase apply 4 signals. 1: enable 0: disable. The bit is
         *  the same with spi_mem_c_usr_sram_qio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdin_quad:1;
        /** mem_sdout_quad : R/W; bitpos: [15]; default: 0;
         *  For SPI0 external RAM , dout phase apply 4 signals. 1: enable 0: disable. The bit
         *  is the same with spi_mem_c_usr_sram_qio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdout_quad:1;
        /** mem_saddr_quad : R/W; bitpos: [16]; default: 0;
         *  For SPI0 external RAM , address phase apply 4 signals. 1: enable 0: disable. The
         *  bit is the same with spi_mem_c_usr_sram_qio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_saddr_quad:1;
        /** mem_scmd_quad : R/W; bitpos: [17]; default: 0;
         *  For SPI0 external RAM , cmd phase apply 4 signals. 1: enable 0: disable. The bit is
         *  the same with spi_mem_c_usr_sram_qio.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_scmd_quad:1;
        /** mem_sdin_oct : R/W; bitpos: [18]; default: 0;
         *  For SPI0 external RAM , din phase apply 8 signals. 1: enable 0: disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdin_oct:1;
        /** mem_sdout_oct : R/W; bitpos: [19]; default: 0;
         *  For SPI0 external RAM , dout phase apply 8 signals. 1: enable 0: disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdout_oct:1;
        /** mem_saddr_oct : R/W; bitpos: [20]; default: 0;
         *  For SPI0 external RAM , address phase apply 4 signals. 1: enable 0: disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_saddr_oct:1;
        /** mem_scmd_oct : R/W; bitpos: [21]; default: 0;
         *  For SPI0 external RAM , cmd phase apply 8 signals. 1: enable 0: disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_scmd_oct:1;
        /** mem_sdummy_rin : R/W; bitpos: [22]; default: 1;
         *  In the dummy phase of a MSPI read data transfer when accesses to external RAM, the
         *  signal level of SPI bus is output by the MSPI controller.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdummy_rin:1;
        /** mem_sdummy_wout : R/W; bitpos: [23]; default: 1;
         *  In the dummy phase of a MSPI write data transfer when accesses to external RAM, the
         *  signal level of SPI bus is output by the MSPI controller.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdummy_wout:1;
        /** smem_wdummy_dqs_always_out : R/W; bitpos: [24]; default: 0;
         *  In the dummy phase of an MSPI write data transfer when accesses to external RAM,
         *  the level of SPI_DQS is output by the MSPI controller.
         */
        uint32_t smem_wdummy_dqs_always_out:1;
        /** smem_wdummy_always_out : R/W; bitpos: [25]; default: 0;
         *  In the dummy phase of an MSPI write data transfer when accesses to external RAM,
         *  the level of SPI_IO[7:0] is output by the MSPI controller.
         */
        uint32_t smem_wdummy_always_out:1;
        /** mem_sdin_hex : HRO; bitpos: [26]; default: 0;
         *  For SPI0 external RAM , din phase apply 16 signals. 1: enable 0: disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdin_hex:1;
        /** mem_sdout_hex : HRO; bitpos: [27]; default: 0;
         *  For SPI0 external RAM , dout phase apply 16 signals. 1: enable 0: disable.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_sdout_hex:1;
        uint32_t reserved_28:2;
        /** smem_dqs_ie_always_on : R/W; bitpos: [30]; default: 0;
         *  When accesses to external RAM, 1: the IE signals of pads connected to SPI_DQS are
         *  always 1. 0: Others.
         */
        uint32_t smem_dqs_ie_always_on:1;
        /** smem_data_ie_always_on : R/W; bitpos: [31]; default: 1;
         *  When accesses to external RAM, 1: the IE signals of pads connected to SPI_IO[7:0]
         *  are always 1. 0: Others.
         */
        uint32_t smem_data_ie_always_on:1;
    };
    uint32_t val;
} spi_mem_c_sram_cmd_reg_t;

/** Type of mem_sram_drd_cmd register
 *  SPI0 external RAM DDR read command control register
 */
typedef union {
    struct {
        /** mem_cache_sram_usr_rd_cmd_value : R/W; bitpos: [15:0]; default: 0;
         *  For SPI0,When cache mode is enable it is the read command value of command phase
         *  for sram.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_cache_sram_usr_rd_cmd_value:16;
        uint32_t reserved_16:12;
        /** mem_cache_sram_usr_rd_cmd_bitlen : R/W; bitpos: [31:28]; default: 0;
         *  For SPI0,When cache mode is enable it is the length in bits of command phase for
         *  sram. The register value shall be (bit_num-1).
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_cache_sram_usr_rd_cmd_bitlen:4;
    };
    uint32_t val;
} spi_mem_c_sram_drd_cmd_reg_t;

/** Type of mem_sram_dwr_cmd register
 *  SPI0 external RAM DDR write command control register
 */
typedef union {
    struct {
        /** mem_cache_sram_usr_wr_cmd_value : R/W; bitpos: [15:0]; default: 0;
         *  For SPI0,When cache mode is enable it is the write command value of command phase
         *  for sram.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_cache_sram_usr_wr_cmd_value:16;
        uint32_t reserved_16:12;
        /** mem_cache_sram_usr_wr_cmd_bitlen : R/W; bitpos: [31:28]; default: 0;
         *  For SPI0,When cache mode is enable it is the in bits of command phase  for sram.
         *  The register value shall be (bit_num-1).
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_cache_sram_usr_wr_cmd_bitlen:4;
    };
    uint32_t val;
} spi_mem_c_sram_dwr_cmd_reg_t;

/** Type of smem_ddr register
 *  SPI0 external RAM DDR mode control register
 */
typedef union {
    struct {
        /** smem_ddr_en : R/W; bitpos: [0]; default: 0;
         *  1: in DDR mode,  0 in SDR mode
         */
        uint32_t smem_ddr_en:1;
        /** smem_var_dummy : R/W; bitpos: [1]; default: 0;
         *  Set the bit to enable variable dummy cycle in spi DDR mode.
         */
        uint32_t smem_var_dummy:1;
        /** smem_ddr_rdat_swp : R/W; bitpos: [2]; default: 0;
         *  Set the bit to reorder rx data of the word in spi DDR mode.
         */
        uint32_t smem_ddr_rdat_swp:1;
        /** smem_ddr_wdat_swp : R/W; bitpos: [3]; default: 0;
         *  Set the bit to reorder tx data of the word in spi DDR mode.
         */
        uint32_t smem_ddr_wdat_swp:1;
        /** smem_ddr_cmd_dis : R/W; bitpos: [4]; default: 0;
         *  the bit is used to disable dual edge in command phase when DDR mode.
         */
        uint32_t smem_ddr_cmd_dis:1;
        /** smem_outminbytelen : R/W; bitpos: [11:5]; default: 1;
         *  It is the minimum output data length in the DDR psram.
         */
        uint32_t smem_outminbytelen:7;
        /** smem_tx_ddr_msk_en : R/W; bitpos: [12]; default: 1;
         *  Set this bit to mask the first or the last byte in SPI0 ECC DDR write mode, when
         *  accesses to external RAM.
         */
        uint32_t smem_tx_ddr_msk_en:1;
        /** smem_rx_ddr_msk_en : R/W; bitpos: [13]; default: 1;
         *  Set this bit to mask the first or the last byte in SPI0 ECC DDR read mode, when
         *  accesses to external RAM.
         */
        uint32_t smem_rx_ddr_msk_en:1;
        /** smem_usr_ddr_dqs_thd : R/W; bitpos: [20:14]; default: 0;
         *  The delay number of data strobe which from memory based on SPI clock.
         */
        uint32_t smem_usr_ddr_dqs_thd:7;
        /** smem_ddr_dqs_loop : R/W; bitpos: [21]; default: 0;
         *  1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when
         *  spi0_slv_st is in spi_mem_c_DIN state. It is used when there is no SPI_DQS signal or
         *  SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and
         *  negative edge of SPI_DQS.
         */
        uint32_t smem_ddr_dqs_loop:1;
        uint32_t reserved_22:2;
        /** smem_clk_diff_en : R/W; bitpos: [24]; default: 0;
         *  Set this bit to enable the differential SPI_CLK#.
         */
        uint32_t smem_clk_diff_en:1;
        uint32_t reserved_25:1;
        /** smem_dqs_ca_in : R/W; bitpos: [26]; default: 0;
         *  Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.
         */
        uint32_t smem_dqs_ca_in:1;
        /** smem_hyperbus_dummy_2x : R/W; bitpos: [27]; default: 0;
         *  Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0
         *  accesses flash or SPI1 accesses flash or sram.
         */
        uint32_t smem_hyperbus_dummy_2x:1;
        /** smem_clk_diff_inv : R/W; bitpos: [28]; default: 0;
         *  Set this bit to invert SPI_DIFF when accesses to external RAM. .
         */
        uint32_t smem_clk_diff_inv:1;
        /** smem_octa_ram_addr : R/W; bitpos: [29]; default: 0;
         *  Set this bit to enable octa_ram address out when accesses to external RAM, which
         *  means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1],
         *  1'b0}.
         */
        uint32_t smem_octa_ram_addr:1;
        /** smem_hyperbus_ca : R/W; bitpos: [30]; default: 0;
         *  Set this bit to enable HyperRAM address out when accesses to external RAM, which
         *  means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.
         */
        uint32_t smem_hyperbus_ca:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} spi_smem_c_ddr_reg_t;

/** Type of smem_ac register
 *  MSPI external RAM ECC and SPI CS timing control register
 */
typedef union {
    struct {
        /** smem_cs_setup : R/W; bitpos: [0]; default: 0;
         *  For SPI0 and SPI1, spi cs is enable when spi is in prepare phase. 1: enable 0:
         *  disable.
         */
        uint32_t smem_cs_setup:1;
        /** smem_cs_hold : R/W; bitpos: [1]; default: 0;
         *  For SPI0 and SPI1, spi cs keep low when spi is in done phase. 1: enable 0: disable.
         */
        uint32_t smem_cs_hold:1;
        /** smem_cs_setup_time : R/W; bitpos: [6:2]; default: 1;
         *  For spi0, (cycles-1) of prepare phase by spi clock this bits are combined with
         *  spi_mem_c_cs_setup bit.
         */
        uint32_t smem_cs_setup_time:5;
        /** smem_cs_hold_time : R/W; bitpos: [11:7]; default: 1;
         *  For SPI0 and SPI1, spi cs signal is delayed to inactive by spi clock this bits are
         *  combined with spi_mem_c_cs_hold bit.
         */
        uint32_t smem_cs_hold_time:5;
        /** smem_ecc_cs_hold_time : R/W; bitpos: [14:12]; default: 3;
         *  spi_smem_c_CS_HOLD_TIME + spi_smem_c_ECC_CS_HOLD_TIME is the SPI0 and SPI1 CS hold
         *  cycles in ECC mode when accessed external RAM.
         */
        uint32_t smem_ecc_cs_hold_time:3;
        /** smem_ecc_skip_page_corner : R/W; bitpos: [15]; default: 1;
         *  1: SPI0 skips page corner when accesses external RAM. 0: Not skip page corner when
         *  accesses external RAM.
         */
        uint32_t smem_ecc_skip_page_corner:1;
        /** smem_ecc_16to18_byte_en : R/W; bitpos: [16]; default: 0;
         *  Set this bit to enable SPI0 and SPI1 ECC 16 bytes data with 2 ECC bytes mode when
         *  accesses external RAM.
         */
        uint32_t smem_ecc_16to18_byte_en:1;
        uint32_t reserved_17:8;
        /** smem_cs_hold_delay : R/W; bitpos: [30:25]; default: 0;
         *  These bits are used to set the minimum CS high time tSHSL between SPI burst
         *  transfer when accesses to external RAM. tSHSL is (spi_smem_c_CS_HOLD_DELAY[5:0] + 1)
         *  MSPI core clock cycles.
         */
        uint32_t smem_cs_hold_delay:6;
        /** smem_split_trans_en : R/W; bitpos: [31]; default: 0;
         *  Set this bit to enable SPI0 split one AXI accesses EXT_RAM transfer into two SPI
         *  transfers when one transfer will cross flash/EXT_RAM page corner, valid no matter
         *  whether there is an ECC region or not.
         */
        uint32_t smem_split_trans_en:1;
    };
    uint32_t val;
} spi_smem_c_ac_reg_t;


/** Group: State control register */
/** Type of mem_fsm register
 *  SPI0 FSM status register
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** mem_lock_delay_time : R/W; bitpos: [11:7]; default: 4;
         *  The lock delay time of SPI0/1 arbiter by spi0_slv_st, after PER is sent by SPI1.
         */
        uint32_t mem_lock_delay_time:5;
        /** mem_flash_lock_en : R/W; bitpos: [12]; default: 0;
         *  The lock enable for FLASH to lock spi0 trans req.1: Enable. 0: Disable.
         */
        uint32_t mem_flash_lock_en:1;
        /** mem_sram_lock_en : R/W; bitpos: [13]; default: 0;
         *  The lock enable for external RAM to lock spi0 trans req.1: Enable. 0: Disable.
         */
        uint32_t mem_sram_lock_en:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} spi_mem_c_fsm_reg_t;


/** Group: Interrupt registers */
/** Type of mem_int_ena register
 *  SPI0 interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** mem_slv_st_end_int_ena : R/W; bitpos: [3]; default: 0;
         *  The enable bit for spi_mem_c_SLV_ST_END_INT interrupt.
         */
        uint32_t mem_slv_st_end_int_ena:1;
        /** mem_mst_st_end_int_ena : R/W; bitpos: [4]; default: 0;
         *  The enable bit for spi_mem_c_MST_ST_END_INT interrupt.
         */
        uint32_t mem_mst_st_end_int_ena:1;
        /** mem_ecc_err_int_ena : R/W; bitpos: [5]; default: 0;
         *  The enable bit for spi_mem_c_ECC_ERR_INT interrupt.
         */
        uint32_t mem_ecc_err_int_ena:1;
        /** mem_pms_reject_int_ena : R/W; bitpos: [6]; default: 0;
         *  The enable bit for spi_mem_c_PMS_REJECT_INT interrupt.
         */
        uint32_t mem_pms_reject_int_ena:1;
        /** mem_axi_raddr_err_int_ena : R/W; bitpos: [7]; default: 0;
         *  The enable bit for spi_mem_c_AXI_RADDR_ERR_INT interrupt.
         */
        uint32_t mem_axi_raddr_err_int_ena:1;
        /** mem_axi_wr_flash_err_int_ena : R/W; bitpos: [8]; default: 0;
         *  The enable bit for spi_mem_c_AXI_WR_FALSH_ERR_INT interrupt.
         */
        uint32_t mem_axi_wr_flash_err_int_ena:1;
        /** mem_axi_waddr_err_int__ena : R/W; bitpos: [9]; default: 0;
         *  The enable bit for spi_mem_c_AXI_WADDR_ERR_INT interrupt.
         */
        uint32_t mem_axi_waddr_err_int__ena:1;
        uint32_t reserved_10:18;
        /** mem_dqs0_afifo_ovf_int_ena : R/W; bitpos: [28]; default: 0;
         *  The enable bit for spi_mem_c_DQS0_AFIFO_OVF_INT interrupt.
         */
        uint32_t mem_dqs0_afifo_ovf_int_ena:1;
        /** mem_dqs1_afifo_ovf_int_ena : R/W; bitpos: [29]; default: 0;
         *  The enable bit for spi_mem_c_DQS1_AFIFO_OVF_INT interrupt.
         */
        uint32_t mem_dqs1_afifo_ovf_int_ena:1;
        /** mem_bus_fifo1_udf_int_ena : R/W; bitpos: [30]; default: 0;
         *  The enable bit for spi_mem_c_BUS_FIFO1_UDF_INT interrupt.
         */
        uint32_t mem_bus_fifo1_udf_int_ena:1;
        /** mem_bus_fifo0_udf_int_ena : R/W; bitpos: [31]; default: 0;
         *  The enable bit for spi_mem_c_BUS_FIFO0_UDF_INT interrupt.
         */
        uint32_t mem_bus_fifo0_udf_int_ena:1;
    };
    uint32_t val;
} spi_mem_c_int_ena_reg_t;

/** Type of mem_int_clr register
 *  SPI0 interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** mem_slv_st_end_int_clr : WT; bitpos: [3]; default: 0;
         *  The clear bit for spi_mem_c_SLV_ST_END_INT interrupt.
         */
        uint32_t mem_slv_st_end_int_clr:1;
        /** mem_mst_st_end_int_clr : WT; bitpos: [4]; default: 0;
         *  The clear bit for spi_mem_c_MST_ST_END_INT interrupt.
         */
        uint32_t mem_mst_st_end_int_clr:1;
        /** mem_ecc_err_int_clr : WT; bitpos: [5]; default: 0;
         *  The clear bit for spi_mem_c_ECC_ERR_INT interrupt.
         */
        uint32_t mem_ecc_err_int_clr:1;
        /** mem_pms_reject_int_clr : WT; bitpos: [6]; default: 0;
         *  The clear bit for spi_mem_c_PMS_REJECT_INT interrupt.
         */
        uint32_t mem_pms_reject_int_clr:1;
        /** mem_axi_raddr_err_int_clr : WT; bitpos: [7]; default: 0;
         *  The clear bit for spi_mem_c_AXI_RADDR_ERR_INT interrupt.
         */
        uint32_t mem_axi_raddr_err_int_clr:1;
        /** mem_axi_wr_flash_err_int_clr : WT; bitpos: [8]; default: 0;
         *  The clear bit for spi_mem_c_AXI_WR_FALSH_ERR_INT interrupt.
         */
        uint32_t mem_axi_wr_flash_err_int_clr:1;
        /** mem_axi_waddr_err_int_clr : WT; bitpos: [9]; default: 0;
         *  The clear bit for spi_mem_c_AXI_WADDR_ERR_INT interrupt.
         */
        uint32_t mem_axi_waddr_err_int_clr:1;
        uint32_t reserved_10:18;
        /** mem_dqs0_afifo_ovf_int_clr : WT; bitpos: [28]; default: 0;
         *  The clear bit for spi_mem_c_DQS0_AFIFO_OVF_INT interrupt.
         */
        uint32_t mem_dqs0_afifo_ovf_int_clr:1;
        /** mem_dqs1_afifo_ovf_int_clr : WT; bitpos: [29]; default: 0;
         *  The clear bit for spi_mem_c_DQS1_AFIFO_OVF_INT interrupt.
         */
        uint32_t mem_dqs1_afifo_ovf_int_clr:1;
        /** mem_bus_fifo1_udf_int_clr : WT; bitpos: [30]; default: 0;
         *  The clear bit for spi_mem_c_BUS_FIFO1_UDF_INT interrupt.
         */
        uint32_t mem_bus_fifo1_udf_int_clr:1;
        /** mem_bus_fifo0_udf_int_clr : WT; bitpos: [31]; default: 0;
         *  The clear bit for spi_mem_c_BUS_FIFO0_UDF_INT interrupt.
         */
        uint32_t mem_bus_fifo0_udf_int_clr:1;
    };
    uint32_t val;
} spi_mem_c_int_clr_reg_t;

/** Type of mem_int_raw register
 *  SPI0 interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** mem_slv_st_end_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw bit for spi_mem_c_SLV_ST_END_INT interrupt. 1: Triggered when spi0_slv_st is
         *  changed from non idle state to idle state. It means that SPI_CS raises high. 0:
         *  Others
         */
        uint32_t mem_slv_st_end_int_raw:1;
        /** mem_mst_st_end_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit for spi_mem_c_MST_ST_END_INT interrupt. 1: Triggered when spi0_mst_st is
         *  changed from non idle state to idle state. 0: Others.
         */
        uint32_t mem_mst_st_end_int_raw:1;
        /** mem_ecc_err_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw bit for spi_mem_c_ECC_ERR_INT interrupt. When spi_fmem_c_ECC_ERR_INT_EN is set
         *  and  spi_smem_c_ECC_ERR_INT_EN is cleared, this bit is triggered when the error times
         *  of SPI0/1 ECC read flash are equal or bigger than spi_mem_c_ECC_ERR_INT_NUM. When
         *  spi_fmem_c_ECC_ERR_INT_EN is cleared and  spi_smem_c_ECC_ERR_INT_EN is set, this bit is
         *  triggered when the error times of SPI0/1 ECC read external RAM are equal or bigger
         *  than spi_mem_c_ECC_ERR_INT_NUM. When spi_fmem_c_ECC_ERR_INT_EN and
         *  spi_smem_c_ECC_ERR_INT_EN are set, this bit is triggered when the total error times
         *  of SPI0/1 ECC read external RAM and flash are equal or bigger than
         *  spi_mem_c_ECC_ERR_INT_NUM. When spi_fmem_c_ECC_ERR_INT_EN and  spi_smem_c_ECC_ERR_INT_EN
         *  are cleared, this bit will not be triggered.
         */
        uint32_t mem_ecc_err_int_raw:1;
        /** mem_pms_reject_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw bit for spi_mem_c_PMS_REJECT_INT interrupt. 1: Triggered when SPI1 access is
         *  rejected. 0: Others.
         */
        uint32_t mem_pms_reject_int_raw:1;
        /** mem_axi_raddr_err_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw bit for spi_mem_c_AXI_RADDR_ERR_INT interrupt. 1: Triggered when AXI read
         *  address is invalid by compared to MMU configuration. 0: Others.
         */
        uint32_t mem_axi_raddr_err_int_raw:1;
        /** mem_axi_wr_flash_err_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw bit for spi_mem_c_AXI_WR_FALSH_ERR_INT interrupt. 1: Triggered when AXI write
         *  flash request is received. 0: Others.
         */
        uint32_t mem_axi_wr_flash_err_int_raw:1;
        /** mem_axi_waddr_err_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw bit for spi_mem_c_AXI_WADDR_ERR_INT interrupt. 1: Triggered when AXI write
         *  address is invalid by compared to MMU configuration. 0: Others.
         */
        uint32_t mem_axi_waddr_err_int_raw:1;
        uint32_t reserved_10:18;
        /** mem_dqs0_afifo_ovf_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  The raw bit for spi_mem_c_DQS0_AFIFO_OVF_INT interrupt. 1: Triggered when the AFIFO
         *  connected to SPI_DQS1 is overflow.
         */
        uint32_t mem_dqs0_afifo_ovf_int_raw:1;
        /** mem_dqs1_afifo_ovf_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  The raw bit for spi_mem_c_DQS1_AFIFO_OVF_INT interrupt. 1: Triggered when the AFIFO
         *  connected to SPI_DQS is overflow.
         */
        uint32_t mem_dqs1_afifo_ovf_int_raw:1;
        /** mem_bus_fifo1_udf_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  The raw bit for spi_mem_c_BUS_FIFO1_UDF_INT interrupt. 1: Triggered when BUS1 FIFO is
         *  underflow.
         */
        uint32_t mem_bus_fifo1_udf_int_raw:1;
        /** mem_bus_fifo0_udf_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  The raw bit for spi_mem_c_BUS_FIFO0_UDF_INT interrupt. 1: Triggered when BUS0 FIFO is
         *  underflow.
         */
        uint32_t mem_bus_fifo0_udf_int_raw:1;
    };
    uint32_t val;
} spi_mem_c_int_raw_reg_t;

/** Type of mem_int_st register
 *  SPI0 interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** mem_slv_st_end_int_st : RO; bitpos: [3]; default: 0;
         *  The status bit for spi_mem_c_SLV_ST_END_INT interrupt.
         */
        uint32_t mem_slv_st_end_int_st:1;
        /** mem_mst_st_end_int_st : RO; bitpos: [4]; default: 0;
         *  The status bit for spi_mem_c_MST_ST_END_INT interrupt.
         */
        uint32_t mem_mst_st_end_int_st:1;
        /** mem_ecc_err_int_st : RO; bitpos: [5]; default: 0;
         *  The status bit for spi_mem_c_ECC_ERR_INT interrupt.
         */
        uint32_t mem_ecc_err_int_st:1;
        /** mem_pms_reject_int_st : RO; bitpos: [6]; default: 0;
         *  The status bit for spi_mem_c_PMS_REJECT_INT interrupt.
         */
        uint32_t mem_pms_reject_int_st:1;
        /** mem_axi_raddr_err_int_st : RO; bitpos: [7]; default: 0;
         *  The enable bit for spi_mem_c_AXI_RADDR_ERR_INT interrupt.
         */
        uint32_t mem_axi_raddr_err_int_st:1;
        /** mem_axi_wr_flash_err_int_st : RO; bitpos: [8]; default: 0;
         *  The enable bit for spi_mem_c_AXI_WR_FALSH_ERR_INT interrupt.
         */
        uint32_t mem_axi_wr_flash_err_int_st:1;
        /** mem_axi_waddr_err_int_st : RO; bitpos: [9]; default: 0;
         *  The enable bit for spi_mem_c_AXI_WADDR_ERR_INT interrupt.
         */
        uint32_t mem_axi_waddr_err_int_st:1;
        uint32_t reserved_10:18;
        /** mem_dqs0_afifo_ovf_int_st : RO; bitpos: [28]; default: 0;
         *  The status bit for spi_mem_c_DQS0_AFIFO_OVF_INT interrupt.
         */
        uint32_t mem_dqs0_afifo_ovf_int_st:1;
        /** mem_dqs1_afifo_ovf_int_st : RO; bitpos: [29]; default: 0;
         *  The status bit for spi_mem_c_DQS1_AFIFO_OVF_INT interrupt.
         */
        uint32_t mem_dqs1_afifo_ovf_int_st:1;
        /** mem_bus_fifo1_udf_int_st : RO; bitpos: [30]; default: 0;
         *  The status bit for spi_mem_c_BUS_FIFO1_UDF_INT interrupt.
         */
        uint32_t mem_bus_fifo1_udf_int_st:1;
        /** mem_bus_fifo0_udf_int_st : RO; bitpos: [31]; default: 0;
         *  The status bit for spi_mem_c_BUS_FIFO0_UDF_INT interrupt.
         */
        uint32_t mem_bus_fifo0_udf_int_st:1;
    };
    uint32_t val;
} spi_mem_c_int_st_reg_t;


/** Group: PMS control and configuration registers */
/** Type of fmem_pmsn_attr register
 *  SPI1 flash PMS section n attribute register
 */
typedef union {
    struct {
        /** fmem_pmsn_rd_attr : R/W; bitpos: [0]; default: 1;
         *  1: SPI1 flash PMS section n read accessible. 0: Not allowed.
         */
        uint32_t fmem_pmsn_rd_attr:1;
        /** fmem_pmsn_wr_attr : R/W; bitpos: [1]; default: 1;
         *  1: SPI1 flash PMS section n write accessible. 0: Not allowed.
         */
        uint32_t fmem_pmsn_wr_attr:1;
        /** fmem_pmsn_ecc : R/W; bitpos: [2]; default: 0;
         *  SPI1 flash PMS section n ECC mode, 1: enable ECC mode. 0: Disable it. The flash PMS
         *  section n is configured by registers spi_fmem_c_PMSn_ADDR_REG and
         *  spi_fmem_c_PMSn_SIZE_REG.
         */
        uint32_t fmem_pmsn_ecc:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} spi_fmem_c_pmsn_attr_reg_t;

/** Type of fmem_pmsn_addr register
 *  SPI1 flash PMS section n start address register
 */
typedef union {
    struct {
        /** fmem_pmsn_addr_s : R/W; bitpos: [28:0]; default: 0;
         *  SPI1 flash PMS section n start address value
         */
        uint32_t fmem_pmsn_addr_s:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} spi_fmem_c_pmsn_addr_reg_t;

/** Type of fmem_pmsn_size register
 *  SPI1 flash PMS section n start address register
 */
typedef union {
    struct {
        /** fmem_pmsn_size : R/W; bitpos: [16:0]; default: 4096;
         *  SPI1 flash PMS section n address region is (spi_fmem_c_PMSn_ADDR_S,
         *  spi_fmem_c_PMSn_ADDR_S + spi_fmem_c_PMSn_SIZE)
         */
        uint32_t fmem_pmsn_size:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} spi_fmem_c_pmsn_size_reg_t;

/** Type of smem_pmsn_attr register
 *  SPI1 external RAM PMS section n attribute register
 */
typedef union {
    struct {
        /** smem_pmsn_rd_attr : R/W; bitpos: [0]; default: 1;
         *  1: SPI1 external RAM PMS section n read accessible. 0: Not allowed.
         */
        uint32_t smem_pmsn_rd_attr:1;
        /** smem_pmsn_wr_attr : R/W; bitpos: [1]; default: 1;
         *  1: SPI1 external RAM PMS section n write accessible. 0: Not allowed.
         */
        uint32_t smem_pmsn_wr_attr:1;
        /** smem_pmsn_ecc : R/W; bitpos: [2]; default: 0;
         *  SPI1 external RAM PMS section n ECC mode, 1: enable ECC mode. 0: Disable it. The
         *  external RAM PMS section n is configured by registers spi_smem_c_PMSn_ADDR_REG and
         *  spi_smem_c_PMSn_SIZE_REG.
         */
        uint32_t smem_pmsn_ecc:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} spi_smem_c_pmsn_attr_reg_t;

/** Type of smem_pmsn_addr register
 *  SPI1 external RAM PMS section n start address register
 */
typedef union {
    struct {
        /** smem_pmsn_addr_s : R/W; bitpos: [28:0]; default: 0;
         *  SPI1 external RAM PMS section n start address value
         */
        uint32_t smem_pmsn_addr_s:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} spi_smem_c_pmsn_addr_reg_t;

/** Type of smem_pmsn_size register
 *  SPI1 external RAM PMS section n start address register
 */
typedef union {
    struct {
        /** smem_pmsn_size : R/W; bitpos: [16:0]; default: 4096;
         *  SPI1 external RAM PMS section n address region is (spi_smem_c_PMSn_ADDR_S,
         *  spi_smem_c_PMSn_ADDR_S + spi_smem_c_PMSn_SIZE)
         */
        uint32_t smem_pmsn_size:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} spi_smem_c_pmsn_size_reg_t;

/** Type of mem_pms_reject register
 *  SPI1 access reject register
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** mem_pm_en : R/W; bitpos: [27]; default: 0;
         *  Set this bit to enable SPI0/1 transfer permission control function.
         */
        uint32_t mem_pm_en:1;
        /** mem_pms_ld : R/SS/WTC; bitpos: [28]; default: 0;
         *  1: SPI1 write access error. 0: No write access error. It is cleared by when
         *  spi_mem_c_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t mem_pms_ld:1;
        /** mem_pms_st : R/SS/WTC; bitpos: [29]; default: 0;
         *  1: SPI1 read access error. 0: No read access error. It is cleared by when
         *  spi_mem_c_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t mem_pms_st:1;
        /** mem_pms_multi_hit : R/SS/WTC; bitpos: [30]; default: 0;
         *  1: SPI1 access is rejected because of address miss. 0: No address miss error. It is
         *  cleared by when  spi_mem_c_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t mem_pms_multi_hit:1;
        /** mem_pms_ivd : R/SS/WTC; bitpos: [31]; default: 0;
         *  1: SPI1 access is rejected because of address multi-hit. 0: No address multi-hit
         *  error. It is cleared by when  spi_mem_c_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t mem_pms_ivd:1;
    };
    uint32_t val;
} spi_mem_c_pms_reject_reg_t;

/** Type of mem_pms_reject_addr register
 *  SPI1 access reject addr register
 */
typedef union {
    struct {
        /** mem_reject_addr : R/SS/WTC; bitpos: [28:0]; default: 0;
         *  This bits show the first SPI1 access error address. It is cleared by when
         *  spi_mem_c_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t mem_reject_addr:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} spi_mem_c_pms_reject_addr_reg_t;


/** Group: MSPI ECC registers */
/** Type of mem_ecc_ctrl register
 *  MSPI ECC control register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** mem_ecc_err_cnt : R/SS/WTC; bitpos: [10:5]; default: 0;
         *  This bits show the error times of MSPI ECC read. It is cleared by when
         *  spi_mem_c_ECC_ERR_INT_CLR bit is set.
         */
        uint32_t mem_ecc_err_cnt:6;
        /** fmem_ecc_err_int_num : R/W; bitpos: [16:11]; default: 10;
         *  Set the error times of MSPI ECC read to generate MSPI spi_mem_c_ECC_ERR_INT interrupt.
         */
        uint32_t fmem_ecc_err_int_num:6;
        /** fmem_ecc_err_int_en : R/W; bitpos: [17]; default: 0;
         *  Set this bit to calculate the error times of MSPI ECC read when accesses to flash.
         */
        uint32_t fmem_ecc_err_int_en:1;
        /** fmem_page_size : R/W; bitpos: [19:18]; default: 0;
         *  Set the page size of the flash accessed by MSPI. 0: 256 bytes. 1: 512 bytes. 2:
         *  1024 bytes. 3: 2048 bytes.
         */
        uint32_t fmem_page_size:2;
        uint32_t reserved_20:1;
        /** fmem_ecc_addr_en : R/W; bitpos: [21]; default: 0;
         *  Set this bit to enable MSPI ECC address conversion, no matter MSPI accesses to the
         *  ECC region or non-ECC region of flash. If there is no ECC region in flash, this bit
         *  should be 0. Otherwise, this bit should be 1.
         */
        uint32_t fmem_ecc_addr_en:1;
        /** mem_usr_ecc_addr_en : R/W; bitpos: [22]; default: 0;
         *  Set this bit to enable ECC address convert in SPI0/1 USR_CMD transfer.
         */
        uint32_t mem_usr_ecc_addr_en:1;
        uint32_t reserved_23:1;
        /** mem_ecc_continue_record_err_en : R/W; bitpos: [24]; default: 1;
         *  1: The error information in spi_mem_c_ECC_ERR_BITS and spi_mem_c_ECC_ERR_ADDR is
         *  updated when there is an ECC error. 0: spi_mem_c_ECC_ERR_BITS and
         *  spi_mem_c_ECC_ERR_ADDR record the first ECC error information.
         */
        uint32_t mem_ecc_continue_record_err_en:1;
        /** mem_ecc_err_bits : R/SS/WTC; bitpos: [31:25]; default: 0;
         *  Records the first ECC error bit number in the 16 bytes(From 0~127, corresponding to
         *  byte 0 bit 0 to byte 15 bit 7)
         */
        uint32_t mem_ecc_err_bits:7;
    };
    uint32_t val;
} spi_mem_c_ecc_ctrl_reg_t;

/** Type of mem_ecc_err_addr register
 *  MSPI ECC error address register
 */
typedef union {
    struct {
        /** mem_ecc_err_addr : R/SS/WTC; bitpos: [28:0]; default: 0;
         *  This bits show the first MSPI ECC error address. It is cleared by when
         *  spi_mem_c_ECC_ERR_INT_CLR bit is set.
         */
        uint32_t mem_ecc_err_addr:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} spi_mem_c_ecc_err_addr_reg_t;

/** Type of smem_ecc_ctrl register
 *  MSPI ECC control register
 */
typedef union {
    struct {
        uint32_t reserved_0:17;
        /** smem_ecc_err_int_en : R/W; bitpos: [17]; default: 0;
         *  Set this bit to calculate the error times of MSPI ECC read when accesses to
         *  external RAM.
         */
        uint32_t smem_ecc_err_int_en:1;
        /** smem_page_size : R/W; bitpos: [19:18]; default: 2;
         *  Set the page size of the external RAM accessed by MSPI. 0: 256 bytes. 1: 512 bytes.
         *  2: 1024 bytes. 3: 2048 bytes.
         */
        uint32_t smem_page_size:2;
        /** smem_ecc_addr_en : R/W; bitpos: [20]; default: 0;
         *  Set this bit to enable MSPI ECC address conversion, no matter MSPI accesses to the
         *  ECC region or non-ECC region of external RAM. If there is no ECC region in external
         *  RAM, this bit should be 0. Otherwise, this bit should be 1.
         */
        uint32_t smem_ecc_addr_en:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} spi_smem_c_ecc_ctrl_reg_t;


/** Group: Status and state control registers */
/** Type of smem_axi_addr_ctrl register
 *  SPI0 AXI address control register
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** mem_all_fifo_empty : RO; bitpos: [26]; default: 1;
         *  The empty status of all AFIFO and SYNC_FIFO in MSPI module. 1: All AXI transfers
         *  and SPI0 transfers are done. 0: Others.
         */
        uint32_t mem_all_fifo_empty:1;
        /** rdata_afifo_rempty : RO; bitpos: [27]; default: 1;
         *  1: RDATA_AFIFO is empty. 0: At least one AXI read transfer is pending.
         */
        uint32_t rdata_afifo_rempty:1;
        /** raddr_afifo_rempty : RO; bitpos: [28]; default: 1;
         *  1: AXI_RADDR_CTL_AFIFO is empty. 0: At least one AXI read transfer is pending.
         */
        uint32_t raddr_afifo_rempty:1;
        /** wdata_afifo_rempty : RO; bitpos: [29]; default: 1;
         *  1: WDATA_AFIFO is empty. 0: At least one AXI write transfer is pending.
         */
        uint32_t wdata_afifo_rempty:1;
        /** wblen_afifo_rempty : RO; bitpos: [30]; default: 1;
         *  1: WBLEN_AFIFO is empty. 0: At least one AXI write transfer is pending.
         */
        uint32_t wblen_afifo_rempty:1;
        /** all_axi_trans_afifo_empty : RO; bitpos: [31]; default: 1;
         *  This bit is set when WADDR_AFIFO, WBLEN_AFIFO, WDATA_AFIFO, AXI_RADDR_CTL_AFIFO and
         *  RDATA_AFIFO are empty and spi0_mst_st is IDLE.
         */
        uint32_t all_axi_trans_afifo_empty:1;
    };
    uint32_t val;
} spi_smem_c_axi_addr_ctrl_reg_t;

/** Type of mem_axi_err_resp_en register
 *  SPI0 AXI error response enable register
 */
typedef union {
    struct {
        /** mem_aw_resp_en_mmu_vld : R/W; bitpos: [0]; default: 0;
         *  Set this bit  to enable AXI response function for mmu valid err in axi write trans.
         */
        uint32_t mem_aw_resp_en_mmu_vld:1;
        /** mem_aw_resp_en_mmu_gid : R/W; bitpos: [1]; default: 0;
         *  Set this bit  to enable AXI response function for mmu gid err in axi write trans.
         */
        uint32_t mem_aw_resp_en_mmu_gid:1;
        /** mem_aw_resp_en_axi_size : R/W; bitpos: [2]; default: 0;
         *  Set this bit  to enable AXI response function for axi size err in axi write trans.
         */
        uint32_t mem_aw_resp_en_axi_size:1;
        /** mem_aw_resp_en_axi_flash : R/W; bitpos: [3]; default: 0;
         *  Set this bit  to enable AXI response function for axi flash err in axi write trans.
         */
        uint32_t mem_aw_resp_en_axi_flash:1;
        /** mem_aw_resp_en_mmu_ecc : R/W; bitpos: [4]; default: 0;
         *  Set this bit  to enable AXI response function for mmu ecc err in axi write trans.
         */
        uint32_t mem_aw_resp_en_mmu_ecc:1;
        /** mem_aw_resp_en_mmu_sens : R/W; bitpos: [5]; default: 0;
         *  Set this bit  to enable AXI response function for mmu sens in err axi write trans.
         */
        uint32_t mem_aw_resp_en_mmu_sens:1;
        /** mem_aw_resp_en_axi_wstrb : R/W; bitpos: [6]; default: 0;
         *  Set this bit  to enable AXI response function for axi wstrb err in axi write trans.
         */
        uint32_t mem_aw_resp_en_axi_wstrb:1;
        /** mem_ar_resp_en_mmu_vld : R/W; bitpos: [7]; default: 0;
         *  Set this bit  to enable AXI response function for mmu valid err in axi read trans.
         */
        uint32_t mem_ar_resp_en_mmu_vld:1;
        /** mem_ar_resp_en_mmu_gid : R/W; bitpos: [8]; default: 0;
         *  Set this bit  to enable AXI response function for mmu gid err in axi read trans.
         */
        uint32_t mem_ar_resp_en_mmu_gid:1;
        /** mem_ar_resp_en_mmu_ecc : R/W; bitpos: [9]; default: 0;
         *  Set this bit  to enable AXI response function for mmu ecc err in axi read trans.
         */
        uint32_t mem_ar_resp_en_mmu_ecc:1;
        /** mem_ar_resp_en_mmu_sens : R/W; bitpos: [10]; default: 0;
         *  Set this bit  to enable AXI response function for mmu sensitive err in axi read
         *  trans.
         */
        uint32_t mem_ar_resp_en_mmu_sens:1;
        /** mem_ar_resp_en_axi_size : R/W; bitpos: [11]; default: 0;
         *  Set this bit  to enable AXI response function for axi size err in axi read trans.
         */
        uint32_t mem_ar_resp_en_axi_size:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} spi_mem_c_axi_err_resp_en_reg_t;


/** Group: Flash timing registers */
/** Type of mem_timing_cali register
 *  SPI0 flash timing calibration register
 */
typedef union {
    struct {
        /** mem_timing_clk_ena : R/W; bitpos: [0]; default: 1;
         *  The bit is used to enable timing adjust clock for all reading operations.
         */
        uint32_t mem_timing_clk_ena:1;
        /** mem_timing_cali : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable timing auto-calibration for all reading operations.
         */
        uint32_t mem_timing_cali:1;
        /** mem_extra_dummy_cyclelen : R/W; bitpos: [4:2]; default: 0;
         *  add extra dummy spi clock cycle length for spi clock calibration.
         */
        uint32_t mem_extra_dummy_cyclelen:3;
        /** mem_dll_timing_cali : R/W; bitpos: [5]; default: 0;
         *  Set this bit to enable DLL for timing calibration in DDR mode when accessed to
         *  flash.
         */
        uint32_t mem_dll_timing_cali:1;
        /** mem_timing_cali_update : WT; bitpos: [6]; default: 0;
         *  Set this bit to update delay mode, delay num and extra dummy in MSPI.
         */
        uint32_t mem_timing_cali_update:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} spi_mem_c_timing_cali_reg_t;

/** Type of mem_din_mode register
 *  MSPI flash input timing delay mode control register
 */
typedef union {
    struct {
        /** mem_din0_mode : R/W; bitpos: [2:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t mem_din0_mode:3;
        /** mem_din1_mode : R/W; bitpos: [5:3]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t mem_din1_mode:3;
        /** mem_din2_mode : R/W; bitpos: [8:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t mem_din2_mode:3;
        /** mem_din3_mode : R/W; bitpos: [11:9]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t mem_din3_mode:3;
        /** mem_din4_mode : R/W; bitpos: [14:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t mem_din4_mode:3;
        /** mem_din5_mode : R/W; bitpos: [17:15]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t mem_din5_mode:3;
        /** mem_din6_mode : R/W; bitpos: [20:18]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t mem_din6_mode:3;
        /** mem_din7_mode : R/W; bitpos: [23:21]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t mem_din7_mode:3;
        /** mem_dins_mode : R/W; bitpos: [26:24]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t mem_dins_mode:3;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_mem_c_din_mode_reg_t;

/** Type of mem_din_num register
 *  MSPI flash input timing delay number control register
 */
typedef union {
    struct {
        /** mem_din0_num : R/W; bitpos: [1:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t mem_din0_num:2;
        /** mem_din1_num : R/W; bitpos: [3:2]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t mem_din1_num:2;
        /** mem_din2_num : R/W; bitpos: [5:4]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t mem_din2_num:2;
        /** mem_din3_num : R/W; bitpos: [7:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t mem_din3_num:2;
        /** mem_din4_num : R/W; bitpos: [9:8]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t mem_din4_num:2;
        /** mem_din5_num : R/W; bitpos: [11:10]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t mem_din5_num:2;
        /** mem_din6_num : R/W; bitpos: [13:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t mem_din6_num:2;
        /** mem_din7_num : R/W; bitpos: [15:14]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t mem_din7_num:2;
        /** mem_dins_num : R/W; bitpos: [17:16]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t mem_dins_num:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} spi_mem_c_din_num_reg_t;

/** Type of mem_dout_mode register
 *  MSPI flash output timing adjustment control register
 */
typedef union {
    struct {
        /** mem_dout0_mode : R/W; bitpos: [0]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t mem_dout0_mode:1;
        /** mem_dout1_mode : R/W; bitpos: [1]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t mem_dout1_mode:1;
        /** mem_dout2_mode : R/W; bitpos: [2]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t mem_dout2_mode:1;
        /** mem_dout3_mode : R/W; bitpos: [3]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t mem_dout3_mode:1;
        /** mem_dout4_mode : R/W; bitpos: [4]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t mem_dout4_mode:1;
        /** mem_dout5_mode : R/W; bitpos: [5]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t mem_dout5_mode:1;
        /** mem_dout6_mode : R/W; bitpos: [6]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t mem_dout6_mode:1;
        /** mem_dout7_mode : R/W; bitpos: [7]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t mem_dout7_mode:1;
        /** mem_douts_mode : R/W; bitpos: [8]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t mem_douts_mode:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} spi_mem_c_dout_mode_reg_t;


/** Group: External RAM timing registers */
/** Type of smem_timing_cali register
 *  MSPI external RAM timing calibration register
 */
typedef union {
    struct {
        /** smem_timing_clk_ena : R/W; bitpos: [0]; default: 1;
         *  For sram, the bit is used to enable timing adjust clock for all reading operations.
         */
        uint32_t smem_timing_clk_ena:1;
        /** smem_timing_cali : R/W; bitpos: [1]; default: 0;
         *  For sram, the bit is used to enable timing auto-calibration for all reading
         *  operations.
         */
        uint32_t smem_timing_cali:1;
        /** smem_extra_dummy_cyclelen : R/W; bitpos: [4:2]; default: 0;
         *  For sram, add extra dummy spi clock cycle length for spi clock calibration.
         */
        uint32_t smem_extra_dummy_cyclelen:3;
        /** smem_dll_timing_cali : R/W; bitpos: [5]; default: 0;
         *  Set this bit to enable DLL for timing calibration in DDR mode when accessed to
         *  EXT_RAM.
         */
        uint32_t smem_dll_timing_cali:1;
        uint32_t reserved_6:1;
        /** smem_dqs0_270_sel : R/W; bitpos: [8:7]; default: 1;
         *  Set these bits to delay dqs signal & invert delayed signal for DLL timing adjust.
         *  2'd0: 0.5ns, 2'd1: 1.0ns, 2'd2: 1.5ns 2'd3: 2.0ns.
         */
        uint32_t smem_dqs0_270_sel:2;
        /** smem_dqs0_90_sel : R/W; bitpos: [10:9]; default: 1;
         *  Set these bits to delay dqs signal for DLL timing adjust. 2'd0: 0.5ns, 2'd1: 1.0ns,
         *  2'd2: 1.5ns 2'd3: 2.0ns.
         */
        uint32_t smem_dqs0_90_sel:2;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} spi_smem_c_timing_cali_reg_t;

/** Type of smem_din_mode register
 *  MSPI external RAM input timing delay mode control register
 */
typedef union {
    struct {
        /** smem_din0_mode : R/W; bitpos: [2:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din0_mode:3;
        /** smem_din1_mode : R/W; bitpos: [5:3]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din1_mode:3;
        /** smem_din2_mode : R/W; bitpos: [8:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din2_mode:3;
        /** smem_din3_mode : R/W; bitpos: [11:9]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din3_mode:3;
        /** smem_din4_mode : R/W; bitpos: [14:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din4_mode:3;
        /** smem_din5_mode : R/W; bitpos: [17:15]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din5_mode:3;
        /** smem_din6_mode : R/W; bitpos: [20:18]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din6_mode:3;
        /** smem_din7_mode : R/W; bitpos: [23:21]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din7_mode:3;
        /** smem_dins_mode : R/W; bitpos: [26:24]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_dins_mode:3;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_smem_c_din_mode_reg_t;

/** Type of smem_din_num register
 *  MSPI external RAM input timing delay number control register
 */
typedef union {
    struct {
        /** smem_din0_num : R/W; bitpos: [1:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din0_num:2;
        /** smem_din1_num : R/W; bitpos: [3:2]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din1_num:2;
        /** smem_din2_num : R/W; bitpos: [5:4]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din2_num:2;
        /** smem_din3_num : R/W; bitpos: [7:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din3_num:2;
        /** smem_din4_num : R/W; bitpos: [9:8]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din4_num:2;
        /** smem_din5_num : R/W; bitpos: [11:10]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din5_num:2;
        /** smem_din6_num : R/W; bitpos: [13:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din6_num:2;
        /** smem_din7_num : R/W; bitpos: [15:14]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din7_num:2;
        /** smem_dins_num : R/W; bitpos: [17:16]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_dins_num:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} spi_smem_c_din_num_reg_t;

/** Type of smem_dout_mode register
 *  MSPI external RAM output timing adjustment control register
 */
typedef union {
    struct {
        /** smem_dout0_mode : R/W; bitpos: [0]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout0_mode:1;
        /** smem_dout1_mode : R/W; bitpos: [1]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout1_mode:1;
        /** smem_dout2_mode : R/W; bitpos: [2]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout2_mode:1;
        /** smem_dout3_mode : R/W; bitpos: [3]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout3_mode:1;
        /** smem_dout4_mode : R/W; bitpos: [4]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout4_mode:1;
        /** smem_dout5_mode : R/W; bitpos: [5]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout5_mode:1;
        /** smem_dout6_mode : R/W; bitpos: [6]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout6_mode:1;
        /** smem_dout7_mode : R/W; bitpos: [7]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout7_mode:1;
        /** smem_douts_mode : R/W; bitpos: [8]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_douts_mode:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} spi_smem_c_dout_mode_reg_t;

/** Type of smem_din_hex_mode register
 *  MSPI 16x external RAM input timing delay mode control register
 */
typedef union {
    struct {
        /** smem_din08_mode : HRO; bitpos: [2:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din08_mode:3;
        /** smem_din09_mode : HRO; bitpos: [5:3]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din09_mode:3;
        /** smem_din10_mode : HRO; bitpos: [8:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din10_mode:3;
        /** smem_din11_mode : HRO; bitpos: [11:9]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din11_mode:3;
        /** smem_din12_mode : HRO; bitpos: [14:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din12_mode:3;
        /** smem_din13_mode : HRO; bitpos: [17:15]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din13_mode:3;
        /** smem_din14_mode : HRO; bitpos: [20:18]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din14_mode:3;
        /** smem_din15_mode : HRO; bitpos: [23:21]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din15_mode:3;
        /** smem_dins_hex_mode : HRO; bitpos: [26:24]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_dins_hex_mode:3;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_smem_c_din_hex_mode_reg_t;

/** Type of smem_din_hex_num register
 *  MSPI 16x external RAM input timing delay number control register
 */
typedef union {
    struct {
        /** smem_din08_num : HRO; bitpos: [1:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din08_num:2;
        /** smem_din09_num : HRO; bitpos: [3:2]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din09_num:2;
        /** smem_din10_num : HRO; bitpos: [5:4]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din10_num:2;
        /** smem_din11_num : HRO; bitpos: [7:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din11_num:2;
        /** smem_din12_num : HRO; bitpos: [9:8]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din12_num:2;
        /** smem_din13_num : HRO; bitpos: [11:10]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din13_num:2;
        /** smem_din14_num : HRO; bitpos: [13:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din14_num:2;
        /** smem_din15_num : HRO; bitpos: [15:14]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din15_num:2;
        /** smem_dins_hex_num : HRO; bitpos: [17:16]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_dins_hex_num:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} spi_smem_c_din_hex_num_reg_t;

/** Type of smem_dout_hex_mode register
 *  MSPI 16x external RAM output timing adjustment control register
 */
typedef union {
    struct {
        /** smem_dout08_mode : HRO; bitpos: [0]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout08_mode:1;
        /** smem_dout09_mode : HRO; bitpos: [1]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout09_mode:1;
        /** smem_dout10_mode : HRO; bitpos: [2]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout10_mode:1;
        /** smem_dout11_mode : HRO; bitpos: [3]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout11_mode:1;
        /** smem_dout12_mode : HRO; bitpos: [4]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout12_mode:1;
        /** smem_dout13_mode : HRO; bitpos: [5]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout13_mode:1;
        /** smem_dout14_mode : HRO; bitpos: [6]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout14_mode:1;
        /** smem_dout15_mode : HRO; bitpos: [7]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout15_mode:1;
        /** smem_douts_hex_mode : HRO; bitpos: [8]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_douts_hex_mode:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} spi_smem_c_dout_hex_mode_reg_t;


/** Group: NAND FLASH control and status registers */
/** Type of mem_nand_flash_en register
 *  NAND FLASH control register
 */
typedef union {
    struct {
        /** mem_nand_flash_en : HRO; bitpos: [0]; default: 0;
         *  NAND FLASH function enable signal. 1: Enable NAND FLASH, Disable NOR FLASH. 0:
         *  Disable NAND FLASH, Enable NOR FLASH.
         */
        uint32_t mem_nand_flash_en:1;
        /** mem_nand_flash_seq_hd_index : HRO; bitpos: [15:1]; default: 32767;
         *  NAND FLASH spi seq head index configure register. Every 5 bits represent  the 1st
         *  index of a SPI CMD sequence.[14:10]:usr; [9:5]:axi_rd; [4:0]:axi_wr.
         */
        uint32_t mem_nand_flash_seq_hd_index:15;
        /** mem_nand_flash_seq_usr_trig : HRO; bitpos: [16]; default: 0;
         *  NAND FLASH spi seq user trigger configure register. spi_mem_c_NAND_FLASH_SEQ_USR_TRIG
         *  is corresponds to spi_mem_c_NAND_FLASH_SEQ_HD_INDEX[14:10].1: enable 0: disable.
         */
        uint32_t mem_nand_flash_seq_usr_trig:1;
        /** mem_nand_flash_lut_en : HRO; bitpos: [17]; default: 0;
         *  NAND FLASH spi seq & cmd lut cfg en. 1: enable 0: disable.
         */
        uint32_t mem_nand_flash_lut_en:1;
        /** mem_nand_flash_seq_usr_wend : HRO; bitpos: [18]; default: 0;
         *  Used with spi_mem_c_NAND_FLASH_SEQ_USR_TRIG to indicate the last page program ,and to
         *  execute page execute. 1: write end 0: write in a page size.
         */
        uint32_t mem_nand_flash_seq_usr_wend:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} spi_mem_c_nand_flash_en_reg_t;

/** Type of mem_nand_flash_sr_addr0 register
 *  NAND FLASH SPI SEQ control register
 */
typedef union {
    struct {
        /** mem_nand_flash_sr_addr0 : HRO; bitpos: [7:0]; default: 0;
         *  configure state register address for SPI SEQ need. If OIP is in address C0H , user
         *  could configure C0H into this register
         */
        uint32_t mem_nand_flash_sr_addr0:8;
        /** mem_nand_flash_sr_addr1 : HRO; bitpos: [15:8]; default: 0;
         *  configure state register address for SPI SEQ need. If OIP is in address C0H , user
         *  could configure C0H into this register
         */
        uint32_t mem_nand_flash_sr_addr1:8;
        /** mem_nand_flash_sr_addr2 : HRO; bitpos: [23:16]; default: 0;
         *  configure state register address for SPI SEQ need. If OIP is in address C0H , user
         *  could configure C0H into this register
         */
        uint32_t mem_nand_flash_sr_addr2:8;
        /** mem_nand_flash_sr_addr3 : HRO; bitpos: [31:24]; default: 0;
         *  configure state register address for SPI SEQ need. If OIP is in address C0H , user
         *  could configure C0H into this register
         */
        uint32_t mem_nand_flash_sr_addr3:8;
    };
    uint32_t val;
} spi_mem_c_nand_flash_sr_addr0_reg_t;

/** Type of mem_nand_flash_sr_din0 register
 *  NAND FLASH SPI SEQ control register
 */
typedef union {
    struct {
        /** mem_nand_flash_sr_din0 : RO; bitpos: [7:0]; default: 0;
         *  spi read state register data to this register for SPI SEQ need.
         *  spi_mem_c_NAND_FLASH_SR_DIN0_REG corresponds to spi_mem_c_NAND_FLASH_SR_ADDR0_REG.
         */
        uint32_t mem_nand_flash_sr_din0:8;
        /** mem_nand_flash_sr_din1 : RO; bitpos: [15:8]; default: 0;
         *  spi read state register data to this register for SPI SEQ need.
         *  spi_mem_c_NAND_FLASH_SR_DIN0_REG corresponds to spi_mem_c_NAND_FLASH_SR_ADDR0_REG.
         */
        uint32_t mem_nand_flash_sr_din1:8;
        /** mem_nand_flash_sr_din2 : RO; bitpos: [23:16]; default: 0;
         *  spi read state register data to this register for SPI SEQ need.
         *  spi_mem_c_NAND_FLASH_SR_DIN0_REG corresponds to spi_mem_c_NAND_FLASH_SR_ADDR0_REG.
         */
        uint32_t mem_nand_flash_sr_din2:8;
        /** mem_nand_flash_sr_din3 : RO; bitpos: [31:24]; default: 0;
         *  spi read state register data to this register for SPI SEQ need.
         *  spi_mem_c_NAND_FLASH_SR_DIN0_REG corresponds to spi_mem_c_NAND_FLASH_SR_ADDR0_REG.
         */
        uint32_t mem_nand_flash_sr_din3:8;
    };
    uint32_t val;
} spi_mem_c_nand_flash_sr_din0_reg_t;

/** Type of mem_nand_flash_cfg_data0 register
 *  NAND FLASH SPI SEQ control register
 */
typedef union {
    struct {
        /** mem_nand_flash_cfg_data0 : HRO; bitpos: [15:0]; default: 0;
         *  configure data for SPI SEQ din/dout need. The data could be use to configure NAND
         *  FLASH or compare read data
         */
        uint32_t mem_nand_flash_cfg_data0:16;
        /** mem_nand_flash_cfg_data1 : HRO; bitpos: [31:16]; default: 0;
         *  configure data for SPI SEQ din/dout need. The data could be use to configure NAND
         *  FLASH or compare read data
         */
        uint32_t mem_nand_flash_cfg_data1:16;
    };
    uint32_t val;
} spi_mem_c_nand_flash_cfg_data0_reg_t;

/** Type of mem_nand_flash_cfg_data1 register
 *  NAND FLASH SPI SEQ control register
 */
typedef union {
    struct {
        /** mem_nand_flash_cfg_data2 : HRO; bitpos: [15:0]; default: 0;
         *  configure data for SPI SEQ din/dout need. The data could be use to configure NAND
         *  FLASH or compare read data
         */
        uint32_t mem_nand_flash_cfg_data2:16;
        /** mem_nand_flash_cfg_data3 : HRO; bitpos: [31:16]; default: 0;
         *  configure data for SPI SEQ din/dout need. The data could be use to configure NAND
         *  FLASH or compare read data
         */
        uint32_t mem_nand_flash_cfg_data3:16;
    };
    uint32_t val;
} spi_mem_c_nand_flash_cfg_data1_reg_t;

/** Type of mem_nand_flash_cfg_data2 register
 *  NAND FLASH SPI SEQ control register
 */
typedef union {
    struct {
        /** mem_nand_flash_cfg_data4 : HRO; bitpos: [15:0]; default: 0;
         *  configure data for SPI SEQ din/dout need. The data could be use to configure NAND
         *  FLASH or compare read data
         */
        uint32_t mem_nand_flash_cfg_data4:16;
        /** mem_nand_flash_cfg_data5 : HRO; bitpos: [31:16]; default: 0;
         *  configure data for SPI SEQ din/dout need. The data could be use to configure NAND
         *  FLASH or compare read data
         */
        uint32_t mem_nand_flash_cfg_data5:16;
    };
    uint32_t val;
} spi_mem_c_nand_flash_cfg_data2_reg_t;

/** Type of mem_nand_flash_cmd_lut0 register
 *  MSPI NAND FLASH CMD LUT control register
 */
typedef union {
    struct {
        /** mem_nand_flash_lut_cmd_value0 : HRO; bitpos: [15:0]; default: 0;
         *  MSPI NAND FLASH config cmd value at cmd lut address 0.
         */
        uint32_t mem_nand_flash_lut_cmd_value0:16;
        /** mem_nand_flash_lut_sfsm_st_en0 : HRO; bitpos: [19:16]; default: 0;
         *  MSPI NAND FLASH config sfsm_st_en at cmd lut address 0.[3]-ADDR period enable;
         *  [2]-DUMMY period enable; [1]-DIN period; [0]-DOUT period.
         */
        uint32_t mem_nand_flash_lut_sfsm_st_en0:4;
        /** mem_nand_flash_lut_cmd_len0 : HRO; bitpos: [23:20]; default: 0;
         *  MSPI NAND FLASH config cmd length at cmd lut address 0.
         */
        uint32_t mem_nand_flash_lut_cmd_len0:4;
        /** mem_nand_flash_lut_addr_len0 : HRO; bitpos: [27:24]; default: 0;
         *  MSPI NAND FLASH config address length at cmd lut address 0.
         */
        uint32_t mem_nand_flash_lut_addr_len0:4;
        /** mem_nand_flash_lut_data_len0 : HRO; bitpos: [29:28]; default: 0;
         *  MSPI NAND FLASH config data length at cmd lut address 0.
         */
        uint32_t mem_nand_flash_lut_data_len0:2;
        /** mem_nand_flash_lut_bus_en0 : HRO; bitpos: [30]; default: 0;
         *  MSPI NAND FLASH config spi_bus_en at cmd lut address 0,SPI could use DUAL/QUAD mode
         *  while enable, SPI could use SINGLE mode while disable.1:Enable. 0:Disable.(Note
         *  these registers are described to indicate the spi_mem_c_NAND_FLASH_CMD_LUT0_REG's
         *  field. The number of CMD LUT entries can be defined by the user, but cannot exceed
         *  16 )
         */
        uint32_t mem_nand_flash_lut_bus_en0:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} spi_mem_c_nand_flash_cmd_lut0_reg_t;

/** Type of mem_nand_flash_spi_seq0 register
 *  NAND FLASH SPI SEQ control register
 */
typedef union {
    struct {
        /** mem_nand_flash_seq_tail_flg0 : HRO; bitpos: [0]; default: 0;
         *  MSPI NAND FLASH config seq_tail_flg at spi seq index 0.1: The last index for
         *  sequence. 0: Not the last index.
         */
        uint32_t mem_nand_flash_seq_tail_flg0:1;
        /** mem_nand_flash_sr_chk_en0 : HRO; bitpos: [1]; default: 0;
         *  MSPI NAND FLASH config sr_chk_en at spi seq index 0. 1: enable 0: disable.
         */
        uint32_t mem_nand_flash_sr_chk_en0:1;
        /** mem_nand_flash_din_index0 : HRO; bitpos: [5:2]; default: 0;
         *  MSPI NAND FLASH config din_index at spi seq index 0.  Use with
         *  spi_mem_c_NAND_FLASH_CFG_DATA
         */
        uint32_t mem_nand_flash_din_index0:4;
        /** mem_nand_flash_addr_index0 : HRO; bitpos: [9:6]; default: 0;
         *  MSPI NAND FLASH config addr_index at spi seq index 0.  Use with
         *  spi_mem_c_NAND_FLASH_SR_ADDR
         */
        uint32_t mem_nand_flash_addr_index0:4;
        /** mem_nand_flash_req_or_cfg0 : HRO; bitpos: [10]; default: 0;
         *  MSPI NAND FLASH config reg_or_cfg at spi seq index 0. 1: AXI/APB request  0: SPI
         *  SEQ configuration.
         */
        uint32_t mem_nand_flash_req_or_cfg0:1;
        /** mem_nand_flash_cmd_index0 : HRO; bitpos: [14:11]; default: 0;
         *  MSPI NAND FLASH config spi_cmd_index at spi seq index 0. Use to find SPI command in
         *  CMD LUT.(Note these registers are described to indicate the
         *  spi_mem_c_NAND_FLASH_SPI_SEQ_REG' fieldd The number of CMD LUT entries can be defined
         *  by the user, but cannot exceed 16 )
         */
        uint32_t mem_nand_flash_cmd_index0:4;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} spi_mem_c_nand_flash_spi_seq0_reg_t;


/** Group: Manual Encryption plaintext Memory */
/** Type of mem_xts_plain_base register
 *  The base address of the memory that stores plaintext in Manual Encryption
 */
typedef union {
    struct {
        /** xts_plain : R/W; bitpos: [31:0]; default: 0;
         *  This field is only used to generate include file in c case. This field is useless.
         *  Please do not use this field.
         */
        uint32_t xts_plain:32;
    };
    uint32_t val;
} spi_mem_c_xts_plain_base_reg_t;


/** Group: Manual Encryption configuration registers */
/** Type of mem_xts_linesize register
 *  Manual Encryption Line-Size register
 */
typedef union {
    struct {
        /** xts_linesize : R/W; bitpos: [1:0]; default: 0;
         *  This bits stores the line-size parameter which will be used in manual encryption
         *  calculation. It decides how many bytes will be encrypted one time. 0: 16-bytes, 1:
         *  32-bytes, 2: 64-bytes, 3:reserved.
         */
        uint32_t xts_linesize:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} spi_mem_c_xts_linesize_reg_t;

/** Type of mem_xts_destination register
 *  Manual Encryption destination register
 */
typedef union {
    struct {
        /** xts_destination : R/W; bitpos: [0]; default: 0;
         *  This bit stores the destination parameter which will be used in manual encryption
         *  calculation. 0: flash(default), 1: psram(reserved). Only default value can be used.
         */
        uint32_t xts_destination:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} spi_mem_c_xts_destination_reg_t;

/** Type of mem_xts_physical_address register
 *  Manual Encryption physical address register
 */
typedef union {
    struct {
        /** xts_physical_address : R/W; bitpos: [29:0]; default: 0;
         *  This bits stores the physical-address parameter which will be used in manual
         *  encryption calculation. This value should aligned with byte number decided by
         *  line-size parameter.
         */
        uint32_t xts_physical_address:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} spi_mem_c_xts_physical_address_reg_t;


/** Group: Manual Encryption control and status registers */
/** Type of mem_xts_trigger register
 *  Manual Encryption physical address register
 */
typedef union {
    struct {
        /** xts_trigger : WT; bitpos: [0]; default: 0;
         *  Set this bit to trigger the process of manual encryption calculation. This action
         *  should only be asserted when manual encryption status is 0. After this action,
         *  manual encryption status becomes 1. After calculation is done, manual encryption
         *  status becomes 2.
         */
        uint32_t xts_trigger:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} spi_mem_c_xts_trigger_reg_t;

/** Type of mem_xts_release register
 *  Manual Encryption physical address register
 */
typedef union {
    struct {
        /** xts_release : WT; bitpos: [0]; default: 0;
         *  Set this bit to release encrypted result to mspi. This action should only be
         *  asserted when manual encryption status is 2. After this action, manual encryption
         *  status will become 3.
         */
        uint32_t xts_release:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} spi_mem_c_xts_release_reg_t;

/** Type of mem_xts_destroy register
 *  Manual Encryption physical address register
 */
typedef union {
    struct {
        /** xts_destroy : WT; bitpos: [0]; default: 0;
         *  Set this bit to destroy encrypted result. This action should be asserted only when
         *  manual encryption status is 3. After this action, manual encryption status will
         *  become 0.
         */
        uint32_t xts_destroy:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} spi_mem_c_xts_destroy_reg_t;

/** Type of mem_xts_state register
 *  Manual Encryption physical address register
 */
typedef union {
    struct {
        /** xts_state : RO; bitpos: [1:0]; default: 0;
         *  This bits stores the status of manual encryption. 0: idle, 1: busy of encryption
         *  calculation, 2: encryption calculation is done but the encrypted result is
         *  invisible to mspi, 3: the encrypted result is visible to mspi.
         */
        uint32_t xts_state:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} spi_mem_c_xts_state_reg_t;


/** Group: Manual Encryption version control register */
/** Type of mem_xts_date register
 *  Manual Encryption version register
 */
typedef union {
    struct {
        /** xts_date : R/W; bitpos: [29:0]; default: 539035911;
         *  This bits stores the last modified-time of manual encryption feature.
         */
        uint32_t xts_date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} spi_mem_c_xts_date_reg_t;


/** Group: MMU access registers */
/** Type of mem_mmu_item_content register
 *  MSPI-MMU item content register
 */
typedef union {
    struct {
        /** mmu_item_content : R/W; bitpos: [31:0]; default: 892;
         *  MSPI-MMU item content
         */
        uint32_t mmu_item_content:32;
    };
    uint32_t val;
} spi_mem_c_mmu_item_content_reg_t;

/** Type of mem_mmu_item_index register
 *  MSPI-MMU item index register
 */
typedef union {
    struct {
        /** mmu_item_index : R/W; bitpos: [31:0]; default: 0;
         *  MSPI-MMU item index
         */
        uint32_t mmu_item_index:32;
    };
    uint32_t val;
} spi_mem_c_mmu_item_index_reg_t;


/** Group: MMU power control and configuration registers */
/** Type of mem_mmu_power_ctrl register
 *  MSPI MMU power control register
 */
typedef union {
    struct {
        /** mmu_mem_force_on : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable mmu-memory clock force on
         */
        uint32_t mmu_mem_force_on:1;
        /** mmu_mem_force_pd : R/W; bitpos: [1]; default: 1;
         *  Set this bit to force mmu-memory powerdown
         */
        uint32_t mmu_mem_force_pd:1;
        /** mmu_mem_force_pu : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force mmu-memory powerup, in this case, the power should also be
         *  controlled by rtc.
         */
        uint32_t mmu_mem_force_pu:1;
        /** mmu_page_size : HRO; bitpos: [4:3]; default: 0;
         *  0: Max page size , 1: Max page size/2 , 2: Max page size/4, 3: Max page size/8
         */
        uint32_t mmu_page_size:2;
        uint32_t reserved_5:11;
        /** mem_aux_ctrl : R/W; bitpos: [29:16]; default: 4896;
         *  MMU PSRAM aux control register
         */
        uint32_t mem_aux_ctrl:14;
        /** mem_rdn_ena : R/W; bitpos: [30]; default: 0;
         *  ECO register enable bit
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_rdn_ena:1;
        /** mem_rdn_result : RO; bitpos: [31]; default: 0;
         *  MSPI module clock domain and AXI clock domain ECO register result register
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_rdn_result:1;
    };
    uint32_t val;
} spi_mem_c_mmu_power_ctrl_reg_t;


/** Group: External mem cryption DPA registers */
/** Type of mem_dpa_ctrl register
 *  SPI memory cryption DPA register
 */
typedef union {
    struct {
        /** crypt_security_level : R/W; bitpos: [2:0]; default: 7;
         *  Set the security level of spi mem cryption. 0: Shut off cryption DPA function. 1-7:
         *  The bigger the number is, the more secure the cryption is. (Note that the
         *  performance of cryption will decrease together with this number increasing)
         */
        uint32_t crypt_security_level:3;
        /** crypt_calc_d_dpa_en : R/W; bitpos: [3]; default: 1;
         *  Only available when SPI_CRYPT_SECURITY_LEVEL is not 0. 1: Enable DPA in the
         *  calculation that using key 1 or key 2. 0: Enable DPA only in the calculation that
         *  using key 1.
         */
        uint32_t crypt_calc_d_dpa_en:1;
        /** crypt_dpa_select_register : R/W; bitpos: [4]; default: 0;
         *  1: MSPI XTS DPA clock gate is controlled by SPI_CRYPT_CALC_D_DPA_EN and
         *  SPI_CRYPT_SECURITY_LEVEL. 0: Controlled by efuse bits.
         */
        uint32_t crypt_dpa_select_register:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} spi_mem_c_dpa_ctrl_reg_t;


/** Group: External mem cryption PSEUDO registers */
/** Type of mem_xts_pseudo_round_conf register
 *  SPI memory cryption PSEUDO register
 */
typedef union {
    struct {
        /** mem_mode_pseudo : R/W; bitpos: [1:0]; default: 0;
         *  Set the mode of pseudo. 2'b00: crypto without pseudo. 2'b01: state T with pseudo
         *  and state D without pseudo. 2'b10: state T with pseudo and state D with few pseudo.
         *  2'b11: crypto with pseudo.
         */
        uint32_t mem_mode_pseudo:2;
        /** mem_pseudo_rng_cnt : R/W; bitpos: [4:2]; default: 7;
         *  xts aes peseudo function base round that must be performed.
         */
        uint32_t mem_pseudo_rng_cnt:3;
        /** mem_pseudo_base : R/W; bitpos: [8:5]; default: 2;
         *  xts aes peseudo function base round that must be performed.
         */
        uint32_t mem_pseudo_base:4;
        /** mem_pseudo_inc : R/W; bitpos: [10:9]; default: 2;
         *  xts aes peseudo function increment round that will be performed randomly between 0 &
         *  2**(inc+1).
         */
        uint32_t mem_pseudo_inc:2;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} spi_mem_c_xts_pseudo_round_conf_reg_t;


/** Group: ECO registers */
/** Type of mem_registerrnd_eco_high register
 *  MSPI ECO high register
 */
typedef union {
    struct {
        /** mem_registerrnd_eco_high : R/W; bitpos: [31:0]; default: 892;
         *  ECO high register
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_registerrnd_eco_high:32;
    };
    uint32_t val;
} spi_mem_c_registerrnd_eco_high_reg_t;

/** Type of mem_registerrnd_eco_low register
 *  MSPI ECO low register
 */
typedef union {
    struct {
        /** mem_registerrnd_eco_low : R/W; bitpos: [31:0]; default: 892;
         *  ECO low register
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t mem_registerrnd_eco_low:32;
    };
    uint32_t val;
} spi_mem_c_registerrnd_eco_low_reg_t;


/** Group: Version control register */
/** Type of mem_date register
 *  SPI0 version control register
 */
typedef union {
    struct {
        /** mem_date : R/W; bitpos: [27:0]; default: 36774400;
         *  SPI0 register version.
         */
        uint32_t mem_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} spi_mem_c_date_reg_t;


typedef struct {
    volatile spi_mem_c_cmd_reg_t cmd;
    uint32_t reserved_004;
    volatile spi_mem_c_ctrl_reg_t ctrl;
    volatile spi_mem_c_ctrl1_reg_t ctrl1;
    volatile spi_mem_c_ctrl2_reg_t ctrl2;
    volatile spi_mem_c_clock_reg_t clock;
    volatile spi_mem_c_user_reg_t user;
    volatile spi_mem_c_user1_reg_t user1;
    volatile spi_mem_c_user2_reg_t user2;
    uint32_t reserved_024[2];
    volatile spi_mem_c_rd_status_reg_t mem_rd_status;
    uint32_t reserved_030;
    volatile spi_mem_c_misc_reg_t mem_misc;
    uint32_t reserved_038;
    volatile spi_mem_c_cache_fctrl_reg_t mem_cache_fctrl;
    volatile spi_mem_c_cache_sctrl_reg_t mem_cache_sctrl;
    volatile spi_mem_c_sram_cmd_reg_t mem_sram_cmd;
    volatile spi_mem_c_sram_drd_cmd_reg_t mem_sram_drd_cmd;
    volatile spi_mem_c_sram_dwr_cmd_reg_t mem_sram_dwr_cmd;
    volatile spi_mem_c_sram_clk_reg_t mem_sram_clk;
    volatile spi_mem_c_fsm_reg_t mem_fsm;
    uint32_t reserved_058[26];
    volatile spi_mem_c_int_ena_reg_t mem_int_ena;
    volatile spi_mem_c_int_clr_reg_t mem_int_clr;
    volatile spi_mem_c_int_raw_reg_t mem_int_raw;
    volatile spi_mem_c_int_st_reg_t mem_int_st;
    uint32_t reserved_0d0;
    volatile spi_mem_c_ddr_reg_t mem_ddr;
    volatile spi_smem_c_ddr_reg_t smem_ddr;
    uint32_t reserved_0dc[9];
    volatile spi_fmem_c_pmsn_attr_reg_t fmem_pmsn_attr[4];
    volatile spi_fmem_c_pmsn_addr_reg_t fmem_pmsn_addr[4];
    volatile spi_fmem_c_pmsn_size_reg_t fmem_pmsn_size[4];
    volatile spi_smem_c_pmsn_attr_reg_t smem_pmsn_attr[4];
    volatile spi_smem_c_pmsn_addr_reg_t smem_pmsn_addr[4];
    volatile spi_smem_c_pmsn_size_reg_t smem_pmsn_size[4];
    volatile spi_mem_c_pms_reject_reg_t mem_pms_reject;
    volatile spi_mem_c_pms_reject_addr_reg_t mem_pms_reject_addr;
    volatile spi_mem_c_ecc_ctrl_reg_t mem_ecc_ctrl;
    volatile spi_mem_c_ecc_err_addr_reg_t mem_ecc_err_addr;
    volatile spi_mem_c_axi_err_addr_reg_t mem_axi_err_addr;
    volatile spi_smem_c_ecc_ctrl_reg_t smem_ecc_ctrl;
    volatile spi_smem_c_axi_addr_ctrl_reg_t smem_axi_addr_ctrl;
    volatile spi_mem_c_axi_err_resp_en_reg_t mem_axi_err_resp_en;
    volatile spi_mem_c_timing_cali_reg_t mem_timing_cali;
    volatile spi_mem_c_din_mode_reg_t mem_din_mode;
    volatile spi_mem_c_din_num_reg_t mem_din_num;
    volatile spi_mem_c_dout_mode_reg_t mem_dout_mode;
    volatile spi_smem_c_timing_cali_reg_t smem_timing_cali;
    volatile spi_smem_c_din_mode_reg_t smem_din_mode;
    volatile spi_smem_c_din_num_reg_t smem_din_num;
    volatile spi_smem_c_dout_mode_reg_t smem_dout_mode;
    volatile spi_smem_c_ac_reg_t smem_ac;
    volatile spi_smem_c_din_hex_mode_reg_t smem_din_hex_mode;
    volatile spi_smem_c_din_hex_num_reg_t smem_din_hex_num;
    volatile spi_smem_c_dout_hex_mode_reg_t smem_dout_hex_mode;
    uint32_t reserved_1b0[20];
    volatile spi_mem_c_clock_gate_reg_t mem_clock_gate;
    volatile spi_mem_c_nand_flash_en_reg_t mem_nand_flash_en;
    volatile spi_mem_c_nand_flash_sr_addr0_reg_t mem_nand_flash_sr_addr0;
    volatile spi_mem_c_nand_flash_sr_din0_reg_t mem_nand_flash_sr_din0;
    volatile spi_mem_c_nand_flash_cfg_data0_reg_t mem_nand_flash_cfg_data0;
    volatile spi_mem_c_nand_flash_cfg_data1_reg_t mem_nand_flash_cfg_data1;
    volatile spi_mem_c_nand_flash_cfg_data2_reg_t mem_nand_flash_cfg_data2;
    uint32_t reserved_21c[9];
    volatile spi_mem_c_nand_flash_cmd_lut0_reg_t mem_nand_flash_cmd_lut0;
    uint32_t reserved_244[15];
    volatile spi_mem_c_nand_flash_spi_seq0_reg_t mem_nand_flash_spi_seq0;
    uint32_t reserved_284[31];
    volatile spi_mem_c_xts_plain_base_reg_t mem_xts_plain_base;
    uint32_t reserved_304[15];
    volatile spi_mem_c_xts_linesize_reg_t mem_xts_linesize;
    volatile spi_mem_c_xts_destination_reg_t mem_xts_destination;
    volatile spi_mem_c_xts_physical_address_reg_t mem_xts_physical_address;
    volatile spi_mem_c_xts_trigger_reg_t mem_xts_trigger;
    volatile spi_mem_c_xts_release_reg_t mem_xts_release;
    volatile spi_mem_c_xts_destroy_reg_t mem_xts_destroy;
    volatile spi_mem_c_xts_state_reg_t mem_xts_state;
    volatile spi_mem_c_xts_date_reg_t mem_xts_date;
    uint32_t reserved_360[7];
    volatile spi_mem_c_mmu_item_content_reg_t mem_mmu_item_content;
    volatile spi_mem_c_mmu_item_index_reg_t mem_mmu_item_index;
    volatile spi_mem_c_mmu_power_ctrl_reg_t mem_mmu_power_ctrl;
    volatile spi_mem_c_dpa_ctrl_reg_t mem_dpa_ctrl;
    volatile spi_mem_c_xts_pseudo_round_conf_reg_t mem_xts_pseudo_round_conf;
    uint32_t reserved_390[24];
    volatile spi_mem_c_registerrnd_eco_high_reg_t mem_registerrnd_eco_high;
    volatile spi_mem_c_registerrnd_eco_low_reg_t mem_registerrnd_eco_low;
    uint32_t reserved_3f8;
    volatile spi_mem_c_date_reg_t mem_date;
} spi_mem_c_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(spi_mem_c_dev_t) == 0x400, "Invalid size of spi0_mem_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
