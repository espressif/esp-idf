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

/** Group: Status and state control register */
/** Type of cmd register
 *  SPI0 FSM status register
 */
typedef union {
    struct {
        /** mst_st : RO; bitpos: [3:0]; default: 0;
         *  The current status of SPI0 master FSM: spi0_mst_st. 0: idle state, 1:SPI0_GRANT ,
         *  2: program/erase suspend state, 3: SPI0 read data state, 4: wait cache/EDMA sent
         *  data is stored in SPI0 TX FIFO, 5: SPI0 write data state.
         */
        uint32_t mst_st:4;
        /** slv_st : RO; bitpos: [7:4]; default: 0;
         *  The current status of SPI0 slave FSM: mspi_st. 0: idle state, 1: preparation state,
         *  2: send command state, 3: send address state, 4: wait state, 5: read data state,
         *  6:write data state, 7: done state, 8: read data end state.
         */
        uint32_t slv_st:4;
        uint32_t reserved_8:10;
        /** usr : HRO; bitpos: [18]; default: 0;
         *  SPI0 USR_CMD start bit, only used when spi_mem_c_C_AXI_REQ_EN is cleared.  An operation
         *  will be triggered when the bit is set. The bit will be cleared once the operation
         *  done.1: enable 0: disable.
         */
        uint32_t usr:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} spi_mem_c_cmd_reg_t;

/** Type of axi_err_addr register
 *  SPI0 AXI request error address.
 */
typedef union {
    struct {
        /** axi_err_addr : R/SS/WTC; bitpos: [26:0]; default: 0;
         *  This bits show the first AXI write/read invalid error or AXI write flash error
         *  address. It is cleared by when spi_mem_c_C_AXI_WADDR_ERR_INT_CLR,
         *  spi_mem_c_C_AXI_WR_FLASH_ERR_IN_CLR or spi_mem_c_C_AXI_RADDR_ERR_IN_CLR bit is set.
         */
        uint32_t axi_err_addr:27;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_mem_c_axi_err_addr_reg_t;


/** Group: Flash Control and configuration registers */
/** Type of ctrl register
 *  SPI0 control register.
 */
typedef union {
    struct {
        /** wdummy_dqs_always_out : HRO; bitpos: [0]; default: 0;
         *  In the dummy phase of an MSPI write data transfer when accesses to flash, the level
         *  of SPI_DQS is output by the MSPI controller.
         */
        uint32_t wdummy_dqs_always_out:1;
        /** wdummy_always_out : R/W; bitpos: [1]; default: 0;
         *  In the dummy phase of an MSPI write data transfer when accesses to flash, the level
         *  of SPI_IO[7:0] is output by the MSPI controller.
         */
        uint32_t wdummy_always_out:1;
        /** fdummy_rin : R/W; bitpos: [2]; default: 1;
         *  In an MSPI read data transfer when accesses to flash, the level of SPI_IO[7:0] is
         *  output by the MSPI controller in the first half part of dummy phase. It is used to
         *  mask invalid SPI_DQS in the half part of dummy phase.
         */
        uint32_t fdummy_rin:1;
        /** fdummy_wout : R/W; bitpos: [3]; default: 1;
         *  In an MSPI write data transfer when accesses to flash, the level of SPI_IO[7:0] is
         *  output by the MSPI controller in the second half part of dummy phase. It is used to
         *  pre-drive flash.
         */
        uint32_t fdummy_wout:1;
        /** fdout_oct : HRO; bitpos: [4]; default: 0;
         *  Apply 8 signals during write-data phase 1:enable 0: disable
         */
        uint32_t fdout_oct:1;
        /** fdin_oct : HRO; bitpos: [5]; default: 0;
         *  Apply 8 signals during read-data phase 1:enable 0: disable
         */
        uint32_t fdin_oct:1;
        /** faddr_oct : HRO; bitpos: [6]; default: 0;
         *  Apply 8 signals during address phase 1:enable 0: disable
         */
        uint32_t faddr_oct:1;
        uint32_t reserved_7:1;
        /** fcmd_quad : R/W; bitpos: [8]; default: 0;
         *  Apply 4 signals during command phase 1:enable 0: disable
         */
        uint32_t fcmd_quad:1;
        /** fcmd_oct : HRO; bitpos: [9]; default: 0;
         *  Apply 8 signals during command phase 1:enable 0: disable
         */
        uint32_t fcmd_oct:1;
        uint32_t reserved_10:3;
        /** fastrd_mode : R/W; bitpos: [13]; default: 1;
         *  This bit enable the bits: spi_mem_c_C_FREAD_QIO, spi_mem_c_C_FREAD_DIO, spi_mem_c_C_FREAD_QOUT
         *  and spi_mem_c_C_FREAD_DOUT. 1: enable 0: disable.
         */
        uint32_t fastrd_mode:1;
        /** fread_dual : R/W; bitpos: [14]; default: 0;
         *  In the read operations, read-data phase apply 2 signals. 1: enable 0: disable.
         */
        uint32_t fread_dual:1;
        uint32_t reserved_15:3;
        /** q_pol : R/W; bitpos: [18]; default: 1;
         *  The bit is used to set MISO line polarity, 1: high 0, low
         */
        uint32_t q_pol:1;
        /** d_pol : R/W; bitpos: [19]; default: 1;
         *  The bit is used to set MOSI line polarity, 1: high 0, low
         */
        uint32_t d_pol:1;
        /** fread_quad : R/W; bitpos: [20]; default: 0;
         *  In the read operations read-data phase apply 4 signals. 1: enable 0: disable.
         */
        uint32_t fread_quad:1;
        /** wp_reg : R/W; bitpos: [21]; default: 1;
         *  Write protect signal output when SPI is idle.  1: output high, 0: output low.
         */
        uint32_t wp_reg:1;
        uint32_t reserved_22:1;
        /** fread_dio : R/W; bitpos: [23]; default: 0;
         *  In the read operations address phase and read-data phase apply 2 signals. 1: enable
         *  0: disable.
         */
        uint32_t fread_dio:1;
        /** fread_qio : R/W; bitpos: [24]; default: 0;
         *  In the read operations address phase and read-data phase apply 4 signals. 1: enable
         *  0: disable.
         */
        uint32_t fread_qio:1;
        uint32_t reserved_25:5;
        /** dqs_ie_always_on : HRO; bitpos: [30]; default: 0;
         *  When accesses to flash, 1: the IE signals of pads connected to SPI_DQS are always
         *  1. 0: Others.
         */
        uint32_t dqs_ie_always_on:1;
        /** data_ie_always_on : R/W; bitpos: [31]; default: 1;
         *  When accesses to flash, 1: the IE signals of pads connected to SPI_IO[7:0] are
         *  always 1. 0: Others.
         */
        uint32_t data_ie_always_on:1;
    };
    uint32_t val;
} spi_mem_c_ctrl_reg_t;

/** Type of ctrl1 register
 *  SPI0 control1 register.
 */
typedef union {
    struct {
        /** clk_mode : R/W; bitpos: [1:0]; default: 0;
         *  SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed
         *  one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3:
         *  SPI clock is alwasy on.
         */
        uint32_t clk_mode:2;
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
        /** axi_rdata_back_fast : HRO; bitpos: [23]; default: 1;
         *  1: Reply AXI read data to AXI bus when one AXI read beat data is available. 0:
         *  Reply AXI read data to AXI bus when all the read data is available.
         */
        uint32_t axi_rdata_back_fast:1;
        /** rresp_ecc_err_en : R/W; bitpos: [24]; default: 0;
         *  1: RRESP is SLV_ERR when there is a ECC error in AXI read data. 0: RRESP is OKAY
         *  when there is a ECC error in AXI read data. The ECC error information is recorded
         *  in spi_mem_c_C_ECC_ERR_ADDR_REG.
         */
        uint32_t rresp_ecc_err_en:1;
        /** ar_splice_en : HRO; bitpos: [25]; default: 0;
         *  Set this bit to enable AXI Read Splice-transfer.
         */
        uint32_t ar_splice_en:1;
        /** aw_splice_en : HRO; bitpos: [26]; default: 0;
         *  Set this bit to enable AXI Write Splice-transfer.
         */
        uint32_t aw_splice_en:1;
        /** ram0_en : HRO; bitpos: [27]; default: 1;
         *  When spi_mem_c_C_DUAL_RAM_EN is 0 and spi_mem_c_C_RAM0_EN is 1, only EXT_RAM0 will be
         *  accessed. When spi_mem_c_C_DUAL_RAM_EN is 0 and spi_mem_c_C_RAM0_EN is 0, only EXT_RAM1
         *  will be accessed. When spi_mem_c_C_DUAL_RAM_EN is 1,  EXT_RAM0 and EXT_RAM1 will be
         *  accessed at the same time.
         */
        uint32_t ram0_en:1;
        /** dual_ram_en : HRO; bitpos: [28]; default: 0;
         *  Set this bit to enable DUAL-RAM mode, EXT_RAM0 and EXT_RAM1 will be accessed at the
         *  same time.
         */
        uint32_t dual_ram_en:1;
        /** fast_write_en : R/W; bitpos: [29]; default: 1;
         *  Set this bit to write data faster, do not wait write data has been stored in
         *  tx_bus_fifo_l2. It will wait 4*T_clk_ctrl to insure the write data has been stored
         *  in  tx_bus_fifo_l2.
         */
        uint32_t fast_write_en:1;
        /** rxfifo_rst : WT; bitpos: [30]; default: 0;
         *  The synchronous reset signal for SPI0 RX AFIFO and all the AES_MSPI SYNC FIFO to
         *  receive signals from AXI.  Set this bit to reset these FIFO.
         */
        uint32_t rxfifo_rst:1;
        /** txfifo_rst : WT; bitpos: [31]; default: 0;
         *  The synchronous reset signal for SPI0 TX AFIFO and all the AES_MSPI SYNC FIFO to
         *  send signals to AXI. Set this bit to reset these FIFO.
         */
        uint32_t txfifo_rst:1;
    };
    uint32_t val;
} spi_mem_c_ctrl1_reg_t;

/** Type of ctrl2 register
 *  SPI0 control2 register.
 */
typedef union {
    struct {
        /** cs_setup_time : R/W; bitpos: [4:0]; default: 1;
         *  (cycles-1) of prepare phase by SPI Bus clock, this bits are combined with
         *  spi_mem_c_C_CS_SETUP bit.
         */
        uint32_t cs_setup_time:5;
        /** cs_hold_time : R/W; bitpos: [9:5]; default: 1;
         *  SPI CS signal is delayed to inactive by SPI bus clock, this bits are combined with
         *  spi_mem_c_C_CS_HOLD bit.
         */
        uint32_t cs_hold_time:5;
        /** ecc_cs_hold_time : HRO; bitpos: [12:10]; default: 3;
         *  spi_mem_c_C_CS_HOLD_TIME + spi_mem_c_C_ECC_CS_HOLD_TIME is the SPI0 CS hold cycle in ECC
         *  mode when accessed flash.
         */
        uint32_t ecc_cs_hold_time:3;
        /** ecc_skip_page_corner : HRO; bitpos: [13]; default: 1;
         *  1: SPI0 and SPI1 skip page corner when accesses flash. 0: Not skip page corner when
         *  accesses flash.
         */
        uint32_t ecc_skip_page_corner:1;
        /** ecc_16to18_byte_en : HRO; bitpos: [14]; default: 0;
         *  Set this bit to enable SPI0 and SPI1 ECC 16 bytes data with 2 ECC bytes mode when
         *  accesses flash.
         */
        uint32_t ecc_16to18_byte_en:1;
        uint32_t reserved_15:9;
        /** split_trans_en : R/W; bitpos: [24]; default: 1;
         *  Set this bit to enable SPI0 split one AXI read flash transfer into two SPI
         *  transfers when one transfer will cross flash or EXT_RAM page corner, valid no
         *  matter whether there is an ECC region or not.
         */
        uint32_t split_trans_en:1;
        /** cs_hold_delay : R/W; bitpos: [30:25]; default: 0;
         *  These bits are used to set the minimum CS high time tSHSL between SPI burst
         *  transfer when accesses to flash. tSHSL is (spi_mem_c_C_CS_HOLD_DELAY[5:0] + 1) MSPI
         *  core clock cycles.
         */
        uint32_t cs_hold_delay:6;
        /** sync_reset : WT; bitpos: [31]; default: 0;
         *  The spi0_mst_st and spi0_slv_st will be reset.
         */
        uint32_t sync_reset:1;
    };
    uint32_t val;
} spi_mem_c_ctrl2_reg_t;

/** Type of misc register
 *  SPI0 misc register
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** fsub_pin : HRO; bitpos: [7]; default: 0;
         *  For SPI0,  flash is connected to SUBPINs.
         */
        uint32_t fsub_pin:1;
        /** ssub_pin : HRO; bitpos: [8]; default: 0;
         *  For SPI0,  sram is connected to SUBPINs.
         */
        uint32_t ssub_pin:1;
        /** ck_idle_edge : R/W; bitpos: [9]; default: 0;
         *  1: SPI_CLK line is high when idle     0: spi clk line is low when idle
         */
        uint32_t ck_idle_edge:1;
        /** cs_keep_active : R/W; bitpos: [10]; default: 0;
         *  SPI_CS line keep low when the bit is set.
         */
        uint32_t cs_keep_active:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} spi_mem_c_misc_reg_t;

/** Type of cache_fctrl register
 *  SPI0 bit mode control register.
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** same_aw_ar_addr_chk_en : HRO; bitpos: [30]; default: 1;
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

/** Type of ddr register
 *  SPI0 flash DDR mode control register
 */
typedef union {
    struct {
        /** fmem_ddr_en : HRO; bitpos: [0]; default: 0;
         *  1: in DDR mode,  0 in SDR mode
         */
        uint32_t fmem_ddr_en:1;
        /** fmem_var_dummy : HRO; bitpos: [1]; default: 0;
         *  Set the bit to enable variable dummy cycle in spi DDR mode.
         */
        uint32_t fmem_var_dummy:1;
        /** fmem_ddr_rdat_swp : HRO; bitpos: [2]; default: 0;
         *  Set the bit to reorder rx data of the word in spi DDR mode.
         */
        uint32_t fmem_ddr_rdat_swp:1;
        /** fmem_ddr_wdat_swp : HRO; bitpos: [3]; default: 0;
         *  Set the bit to reorder tx data of the word in spi DDR mode.
         */
        uint32_t fmem_ddr_wdat_swp:1;
        /** fmem_ddr_cmd_dis : HRO; bitpos: [4]; default: 0;
         *  the bit is used to disable dual edge in command phase when DDR mode.
         */
        uint32_t fmem_ddr_cmd_dis:1;
        /** fmem_outminbytelen : HRO; bitpos: [11:5]; default: 1;
         *  It is the minimum output data length in the panda device.
         */
        uint32_t fmem_outminbytelen:7;
        /** fmem_tx_ddr_msk_en : HRO; bitpos: [12]; default: 1;
         *  Set this bit to mask the first or the last byte in SPI0 ECC DDR write mode, when
         *  accesses to flash.
         */
        uint32_t fmem_tx_ddr_msk_en:1;
        /** fmem_rx_ddr_msk_en : HRO; bitpos: [13]; default: 1;
         *  Set this bit to mask the first or the last byte in SPI0 ECC DDR read mode, when
         *  accesses to flash.
         */
        uint32_t fmem_rx_ddr_msk_en:1;
        /** fmem_usr_ddr_dqs_thd : HRO; bitpos: [20:14]; default: 0;
         *  The delay number of data strobe which from memory based on SPI clock.
         */
        uint32_t fmem_usr_ddr_dqs_thd:7;
        /** fmem_ddr_dqs_loop : HRO; bitpos: [21]; default: 0;
         *  1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when
         *  spi0_slv_st is in spi_mem_c_C_DIN state. It is used when there is no SPI_DQS signal or
         *  SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and
         *  negative edge of SPI_DQS.
         */
        uint32_t fmem_ddr_dqs_loop:1;
        uint32_t reserved_22:2;
        /** fmem_clk_diff_en : HRO; bitpos: [24]; default: 0;
         *  Set this bit to enable the differential SPI_CLK#.
         */
        uint32_t fmem_clk_diff_en:1;
        uint32_t reserved_25:1;
        /** fmem_dqs_ca_in : HRO; bitpos: [26]; default: 0;
         *  Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.
         */
        uint32_t fmem_dqs_ca_in:1;
        /** fmem_hyperbus_dummy_2x : HRO; bitpos: [27]; default: 0;
         *  Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0
         *  accesses flash or SPI1 accesses flash or sram.
         */
        uint32_t fmem_hyperbus_dummy_2x:1;
        /** fmem_clk_diff_inv : HRO; bitpos: [28]; default: 0;
         *  Set this bit to invert SPI_DIFF when accesses to flash. .
         */
        uint32_t fmem_clk_diff_inv:1;
        /** fmem_octa_ram_addr : HRO; bitpos: [29]; default: 0;
         *  Set this bit to enable octa_ram address out when accesses to flash, which means
         *  ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.
         */
        uint32_t fmem_octa_ram_addr:1;
        /** fmem_hyperbus_ca : HRO; bitpos: [30]; default: 0;
         *  Set this bit to enable HyperRAM address out when accesses to flash, which means
         *  ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.
         */
        uint32_t fmem_hyperbus_ca:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} spi_mem_c_ddr_reg_t;


/** Group: Clock control and configuration registers */
/** Type of clock register
 *  SPI clock division control register.
 */
typedef union {
    struct {
        /** clkcnt_l : R/W; bitpos: [7:0]; default: 3;
         *  In the master mode it must be equal to spi_mem_c_clkcnt_N.
         */
        uint32_t clkcnt_l:8;
        /** clkcnt_h : R/W; bitpos: [15:8]; default: 1;
         *  In the master mode it must be floor((spi_mem_c_clkcnt_N+1)/2-1).
         */
        uint32_t clkcnt_h:8;
        /** clkcnt_n : R/W; bitpos: [23:16]; default: 3;
         *  In the master mode it is the divider of spi_mem_c_clk. So spi_mem_c_clk frequency is
         *  system/(spi_mem_c_clkcnt_N+1)
         */
        uint32_t clkcnt_n:8;
        uint32_t reserved_24:7;
        /** clk_equ_sysclk : R/W; bitpos: [31]; default: 0;
         *  1: 1-division mode, the frequency of SPI bus clock equals to that of MSPI module
         *  clock.
         */
        uint32_t clk_equ_sysclk:1;
    };
    uint32_t val;
} spi_mem_c_clock_reg_t;

/** Type of clock_gate register
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
/** Type of user register
 *  SPI0 user register.
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** cs_hold : R/W; bitpos: [6]; default: 0;
         *  spi cs keep low when spi is in  done  phase. 1: enable 0: disable.
         */
        uint32_t cs_hold:1;
        /** cs_setup : R/W; bitpos: [7]; default: 0;
         *  spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable.
         */
        uint32_t cs_setup:1;
        uint32_t reserved_8:1;
        /** ck_out_edge : R/W; bitpos: [9]; default: 0;
         *  The bit combined with spi_mem_c_C_CK_IDLE_EDGE bit to control SPI clock mode 0~3.
         */
        uint32_t ck_out_edge:1;
        uint32_t reserved_10:16;
        /** usr_dummy_idle : R/W; bitpos: [26]; default: 0;
         *  spi clock is disable in dummy phase when the bit is enable.
         */
        uint32_t usr_dummy_idle:1;
        uint32_t reserved_27:2;
        /** usr_dummy : R/W; bitpos: [29]; default: 0;
         *  This bit enable the dummy phase of an operation.
         */
        uint32_t usr_dummy:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} spi_mem_c_user_reg_t;

/** Type of user1 register
 *  SPI0 user1 register.
 */
typedef union {
    struct {
        /** usr_dummy_cyclelen : R/W; bitpos: [5:0]; default: 7;
         *  The length in spi_mem_c_clk cycles of dummy phase. The register value shall be
         *  (cycle_num-1).
         */
        uint32_t usr_dummy_cyclelen:6;
        /** usr_dbytelen : HRO; bitpos: [8:6]; default: 1;
         *  SPI0 USR_CMD read or write data byte length -1
         */
        uint32_t usr_dbytelen:3;
        uint32_t reserved_9:17;
        /** usr_addr_bitlen : R/W; bitpos: [31:26]; default: 23;
         *  The length in bits of address phase. The register value shall be (bit_num-1).
         */
        uint32_t usr_addr_bitlen:6;
    };
    uint32_t val;
} spi_mem_c_user1_reg_t;

/** Type of user2 register
 *  SPI0 user2 register.
 */
typedef union {
    struct {
        /** usr_command_value : R/W; bitpos: [15:0]; default: 0;
         *  The value of  command.
         */
        uint32_t usr_command_value:16;
        uint32_t reserved_16:12;
        /** usr_command_bitlen : R/W; bitpos: [31:28]; default: 7;
         *  The length in bits of command phase. The register value shall be (bit_num-1)
         */
        uint32_t usr_command_bitlen:4;
    };
    uint32_t val;
} spi_mem_c_user2_reg_t;


/** Group: External RAM Control and configuration registers */
/** Type of sram_cmd register
 *  SPI0 external RAM mode control register
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** smem_wdummy_dqs_always_out : HRO; bitpos: [24]; default: 0;
         *  In the dummy phase of an MSPI write data transfer when accesses to external RAM,
         *  the level of SPI_DQS is output by the MSPI controller.
         */
        uint32_t smem_wdummy_dqs_always_out:1;
        /** smem_wdummy_always_out : HRO; bitpos: [25]; default: 0;
         *  In the dummy phase of an MSPI write data transfer when accesses to external RAM,
         *  the level of SPI_IO[7:0] is output by the MSPI controller.
         */
        uint32_t smem_wdummy_always_out:1;
        uint32_t reserved_26:4;
        /** smem_dqs_ie_always_on : HRO; bitpos: [30]; default: 1;
         *  When accesses to external RAM, 1: the IE signals of pads connected to SPI_DQS are
         *  always 1. 0: Others.
         */
        uint32_t smem_dqs_ie_always_on:1;
        /** smem_data_ie_always_on : HRO; bitpos: [31]; default: 1;
         *  When accesses to external RAM, 1: the IE signals of pads connected to SPI_IO[7:0]
         *  are always 1. 0: Others.
         */
        uint32_t smem_data_ie_always_on:1;
    };
    uint32_t val;
} spi_mem_c_sram_cmd_reg_t;

/** Type of smem_ddr register
 *  SPI0 external RAM DDR mode control register
 */
typedef union {
    struct {
        /** smem_ddr_en : HRO; bitpos: [0]; default: 0;
         *  1: in DDR mode,  0 in SDR mode
         */
        uint32_t smem_ddr_en:1;
        /** smem_var_dummy : HRO; bitpos: [1]; default: 0;
         *  Set the bit to enable variable dummy cycle in spi DDR mode.
         */
        uint32_t smem_var_dummy:1;
        /** smem_ddr_rdat_swp : HRO; bitpos: [2]; default: 0;
         *  Set the bit to reorder rx data of the word in spi DDR mode.
         */
        uint32_t smem_ddr_rdat_swp:1;
        /** smem_ddr_wdat_swp : HRO; bitpos: [3]; default: 0;
         *  Set the bit to reorder tx data of the word in spi DDR mode.
         */
        uint32_t smem_ddr_wdat_swp:1;
        /** smem_ddr_cmd_dis : HRO; bitpos: [4]; default: 0;
         *  the bit is used to disable dual edge in command phase when DDR mode.
         */
        uint32_t smem_ddr_cmd_dis:1;
        /** smem_outminbytelen : HRO; bitpos: [11:5]; default: 1;
         *  It is the minimum output data length in the DDR psram.
         */
        uint32_t smem_outminbytelen:7;
        /** smem_tx_ddr_msk_en : HRO; bitpos: [12]; default: 1;
         *  Set this bit to mask the first or the last byte in SPI0 ECC DDR write mode, when
         *  accesses to external RAM.
         */
        uint32_t smem_tx_ddr_msk_en:1;
        /** smem_rx_ddr_msk_en : HRO; bitpos: [13]; default: 1;
         *  Set this bit to mask the first or the last byte in SPI0 ECC DDR read mode, when
         *  accesses to external RAM.
         */
        uint32_t smem_rx_ddr_msk_en:1;
        /** smem_usr_ddr_dqs_thd : HRO; bitpos: [20:14]; default: 0;
         *  The delay number of data strobe which from memory based on SPI clock.
         */
        uint32_t smem_usr_ddr_dqs_thd:7;
        /** smem_ddr_dqs_loop : HRO; bitpos: [21]; default: 0;
         *  1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when
         *  spi0_slv_st is in spi_mem_c_C_DIN state. It is used when there is no SPI_DQS signal or
         *  SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and
         *  negative edge of SPI_DQS.
         */
        uint32_t smem_ddr_dqs_loop:1;
        uint32_t reserved_22:2;
        /** smem_clk_diff_en : HRO; bitpos: [24]; default: 0;
         *  Set this bit to enable the differential SPI_CLK#.
         */
        uint32_t smem_clk_diff_en:1;
        uint32_t reserved_25:1;
        /** smem_dqs_ca_in : HRO; bitpos: [26]; default: 0;
         *  Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.
         */
        uint32_t smem_dqs_ca_in:1;
        /** smem_hyperbus_dummy_2x : HRO; bitpos: [27]; default: 0;
         *  Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0
         *  accesses flash or SPI1 accesses flash or sram.
         */
        uint32_t smem_hyperbus_dummy_2x:1;
        /** smem_clk_diff_inv : HRO; bitpos: [28]; default: 0;
         *  Set this bit to invert SPI_DIFF when accesses to external RAM. .
         */
        uint32_t smem_clk_diff_inv:1;
        /** smem_octa_ram_addr : HRO; bitpos: [29]; default: 0;
         *  Set this bit to enable octa_ram address out when accesses to external RAM, which
         *  means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1],
         *  1'b0}.
         */
        uint32_t smem_octa_ram_addr:1;
        /** smem_hyperbus_ca : HRO; bitpos: [30]; default: 0;
         *  Set this bit to enable HyperRAM address out when accesses to external RAM, which
         *  means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.
         */
        uint32_t smem_hyperbus_ca:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} spi_mem_c_smem_ddr_reg_t;

/** Type of smem_ac register
 *  MSPI external RAM ECC and SPI CS timing control register
 */
typedef union {
    struct {
        /** smem_cs_setup : HRO; bitpos: [0]; default: 0;
         *  For SPI0 and SPI1, spi cs is enable when spi is in prepare phase. 1: enable 0:
         *  disable.
         */
        uint32_t smem_cs_setup:1;
        /** smem_cs_hold : HRO; bitpos: [1]; default: 0;
         *  For SPI0 and SPI1, spi cs keep low when spi is in done phase. 1: enable 0: disable.
         */
        uint32_t smem_cs_hold:1;
        /** smem_cs_setup_time : HRO; bitpos: [6:2]; default: 1;
         *  For spi0, (cycles-1) of prepare phase by spi clock this bits are combined with
         *  spi_mem_c_cs_setup bit.
         */
        uint32_t smem_cs_setup_time:5;
        /** smem_cs_hold_time : HRO; bitpos: [11:7]; default: 1;
         *  For SPI0 and SPI1, spi cs signal is delayed to inactive by spi clock this bits are
         *  combined with spi_mem_c_cs_hold bit.
         */
        uint32_t smem_cs_hold_time:5;
        /** smem_ecc_cs_hold_time : HRO; bitpos: [14:12]; default: 3;
         *  SPI_MEM_C_SMEM_CS_HOLD_TIME + SPI_MEM_C_SMEM_ECC_CS_HOLD_TIME is the SPI0 and SPI1 CS hold
         *  cycles in ECC mode when accessed external RAM.
         */
        uint32_t smem_ecc_cs_hold_time:3;
        /** smem_ecc_skip_page_corner : HRO; bitpos: [15]; default: 1;
         *  1: SPI0 skips page corner when accesses external RAM. 0: Not skip page corner when
         *  accesses external RAM.
         */
        uint32_t smem_ecc_skip_page_corner:1;
        /** smem_ecc_16to18_byte_en : HRO; bitpos: [16]; default: 0;
         *  Set this bit to enable SPI0 and SPI1 ECC 16 bytes data with 2 ECC bytes mode when
         *  accesses external RAM.
         */
        uint32_t smem_ecc_16to18_byte_en:1;
        uint32_t reserved_17:8;
        /** smem_cs_hold_delay : HRO; bitpos: [30:25]; default: 0;
         *  These bits are used to set the minimum CS high time tSHSL between SPI burst
         *  transfer when accesses to external RAM. tSHSL is (SPI_MEM_C_SMEM_CS_HOLD_DELAY[5:0] + 1)
         *  MSPI core clock cycles.
         */
        uint32_t smem_cs_hold_delay:6;
        /** smem_split_trans_en : HRO; bitpos: [31]; default: 1;
         *  Set this bit to enable SPI0 split one AXI accesses EXT_RAM transfer into two SPI
         *  transfers when one transfer will cross flash/EXT_RAM page corner, valid no matter
         *  whether there is an ECC region or not.
         */
        uint32_t smem_split_trans_en:1;
    };
    uint32_t val;
} spi_mem_c_smem_ac_reg_t;


/** Group: State control register */
/** Type of fsm register
 *  SPI0 FSM status register
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** lock_delay_time : R/W; bitpos: [11:7]; default: 4;
         *  The lock delay time of SPI0/1 arbiter by spi0_slv_st, after PER is sent by SPI1.
         */
        uint32_t lock_delay_time:5;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} spi_mem_c_fsm_reg_t;


/** Group: Interrupt registers */
/** Type of int_ena register
 *  SPI0 interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** slv_st_end_int_ena : R/W; bitpos: [3]; default: 0;
         *  The enable bit for spi_mem_c_C_SLV_ST_END_INT interrupt.
         */
        uint32_t slv_st_end_int_ena:1;
        /** mst_st_end_int_ena : R/W; bitpos: [4]; default: 0;
         *  The enable bit for spi_mem_c_C_MST_ST_END_INT interrupt.
         */
        uint32_t mst_st_end_int_ena:1;
        /** ecc_err_int_ena : HRO; bitpos: [5]; default: 0;
         *  The enable bit for spi_mem_c_C_ECC_ERR_INT interrupt.
         */
        uint32_t ecc_err_int_ena:1;
        /** pms_reject_int_ena : R/W; bitpos: [6]; default: 0;
         *  The enable bit for spi_mem_c_C_PMS_REJECT_INT interrupt.
         */
        uint32_t pms_reject_int_ena:1;
        /** axi_raddr_err_int_ena : R/W; bitpos: [7]; default: 0;
         *  The enable bit for spi_mem_c_C_AXI_RADDR_ERR_INT interrupt.
         */
        uint32_t axi_raddr_err_int_ena:1;
        /** axi_wr_flash_err_int_ena : HRO; bitpos: [8]; default: 0;
         *  The enable bit for spi_mem_c_C_AXI_WR_FALSH_ERR_INT interrupt.
         */
        uint32_t axi_wr_flash_err_int_ena:1;
        /** axi_waddr_err_int__ena : HRO; bitpos: [9]; default: 0;
         *  The enable bit for spi_mem_c_C_AXI_WADDR_ERR_INT interrupt.
         */
        uint32_t axi_waddr_err_int__ena:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} spi_mem_c_int_ena_reg_t;

/** Type of int_clr register
 *  SPI0 interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** slv_st_end_int_clr : WT; bitpos: [3]; default: 0;
         *  The clear bit for spi_mem_c_C_SLV_ST_END_INT interrupt.
         */
        uint32_t slv_st_end_int_clr:1;
        /** mst_st_end_int_clr : WT; bitpos: [4]; default: 0;
         *  The clear bit for spi_mem_c_C_MST_ST_END_INT interrupt.
         */
        uint32_t mst_st_end_int_clr:1;
        /** ecc_err_int_clr : HRO; bitpos: [5]; default: 0;
         *  The clear bit for spi_mem_c_C_ECC_ERR_INT interrupt.
         */
        uint32_t ecc_err_int_clr:1;
        /** pms_reject_int_clr : WT; bitpos: [6]; default: 0;
         *  The clear bit for spi_mem_c_C_PMS_REJECT_INT interrupt.
         */
        uint32_t pms_reject_int_clr:1;
        /** axi_raddr_err_int_clr : WT; bitpos: [7]; default: 0;
         *  The clear bit for spi_mem_c_C_AXI_RADDR_ERR_INT interrupt.
         */
        uint32_t axi_raddr_err_int_clr:1;
        /** axi_wr_flash_err_int_clr : HRO; bitpos: [8]; default: 0;
         *  The clear bit for spi_mem_c_C_AXI_WR_FALSH_ERR_INT interrupt.
         */
        uint32_t axi_wr_flash_err_int_clr:1;
        /** axi_waddr_err_int_clr : HRO; bitpos: [9]; default: 0;
         *  The clear bit for spi_mem_c_C_AXI_WADDR_ERR_INT interrupt.
         */
        uint32_t axi_waddr_err_int_clr:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} spi_mem_c_int_clr_reg_t;

/** Type of int_raw register
 *  SPI0 interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** slv_st_end_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw bit for spi_mem_c_C_SLV_ST_END_INT interrupt. 1: Triggered when spi0_slv_st is
         *  changed from non idle state to idle state. It means that SPI_CS raises high. 0:
         *  Others
         */
        uint32_t slv_st_end_int_raw:1;
        /** mst_st_end_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit for spi_mem_c_C_MST_ST_END_INT interrupt. 1: Triggered when spi0_mst_st is
         *  changed from non idle state to idle state. 0: Others.
         */
        uint32_t mst_st_end_int_raw:1;
        /** ecc_err_int_raw : HRO; bitpos: [5]; default: 0;
         *  The raw bit for spi_mem_c_C_ECC_ERR_INT interrupt. When SPI_MEM_C_FMEM__ECC_ERR_INT_EN is set
         *  and  SPI_MEM_C_SMEM_ECC_ERR_INT_EN is cleared, this bit is triggered when the error times
         *  of SPI0/1 ECC read flash are equal or bigger than spi_mem_c_C_ECC_ERR_INT_NUM. When
         *  SPI_MEM_C_FMEM__ECC_ERR_INT_EN is cleared and  SPI_MEM_C_SMEM_ECC_ERR_INT_EN is set, this bit is
         *  triggered when the error times of SPI0/1 ECC read external RAM are equal or bigger
         *  than spi_mem_c_C_ECC_ERR_INT_NUM. When SPI_MEM_C_FMEM__ECC_ERR_INT_EN and
         *  SPI_MEM_C_SMEM_ECC_ERR_INT_EN are set, this bit is triggered when the total error times
         *  of SPI0/1 ECC read external RAM and flash are equal or bigger than
         *  spi_mem_c_C_ECC_ERR_INT_NUM. When SPI_MEM_C_FMEM__ECC_ERR_INT_EN and  SPI_MEM_C_SMEM_ECC_ERR_INT_EN
         *  are cleared, this bit will not be triggered.
         */
        uint32_t ecc_err_int_raw:1;
        /** pms_reject_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw bit for spi_mem_c_C_PMS_REJECT_INT interrupt. 1: Triggered when SPI1 access is
         *  rejected. 0: Others.
         */
        uint32_t pms_reject_int_raw:1;
        /** axi_raddr_err_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw bit for spi_mem_c_C_AXI_RADDR_ERR_INT interrupt. 1: Triggered when AXI read
         *  address is invalid by compared to MMU configuration. 0: Others.
         */
        uint32_t axi_raddr_err_int_raw:1;
        /** axi_wr_flash_err_int_raw : HRO; bitpos: [8]; default: 0;
         *  The raw bit for spi_mem_c_C_AXI_WR_FALSH_ERR_INT interrupt. 1: Triggered when AXI write
         *  flash request is received. 0: Others.
         */
        uint32_t axi_wr_flash_err_int_raw:1;
        /** axi_waddr_err_int_raw : HRO; bitpos: [9]; default: 0;
         *  The raw bit for spi_mem_c_C_AXI_WADDR_ERR_INT interrupt. 1: Triggered when AXI write
         *  address is invalid by compared to MMU configuration. 0: Others.
         */
        uint32_t axi_waddr_err_int_raw:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} spi_mem_c_int_raw_reg_t;

/** Type of int_st register
 *  SPI0 interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** slv_st_end_int_st : RO; bitpos: [3]; default: 0;
         *  The status bit for spi_mem_c_C_SLV_ST_END_INT interrupt.
         */
        uint32_t slv_st_end_int_st:1;
        /** mst_st_end_int_st : RO; bitpos: [4]; default: 0;
         *  The status bit for spi_mem_c_C_MST_ST_END_INT interrupt.
         */
        uint32_t mst_st_end_int_st:1;
        /** ecc_err_int_st : HRO; bitpos: [5]; default: 0;
         *  The status bit for spi_mem_c_C_ECC_ERR_INT interrupt.
         */
        uint32_t ecc_err_int_st:1;
        /** pms_reject_int_st : RO; bitpos: [6]; default: 0;
         *  The status bit for spi_mem_c_C_PMS_REJECT_INT interrupt.
         */
        uint32_t pms_reject_int_st:1;
        /** axi_raddr_err_int_st : RO; bitpos: [7]; default: 0;
         *  The enable bit for spi_mem_c_C_AXI_RADDR_ERR_INT interrupt.
         */
        uint32_t axi_raddr_err_int_st:1;
        /** axi_wr_flash_err_int_st : HRO; bitpos: [8]; default: 0;
         *  The enable bit for spi_mem_c_C_AXI_WR_FALSH_ERR_INT interrupt.
         */
        uint32_t axi_wr_flash_err_int_st:1;
        /** axi_waddr_err_int_st : HRO; bitpos: [9]; default: 0;
         *  The enable bit for spi_mem_c_C_AXI_WADDR_ERR_INT interrupt.
         */
        uint32_t axi_waddr_err_int_st:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} spi_mem_c_int_st_reg_t;


/** Group: PMS control and configuration registers */
/** Type of fmem_pmsn_attr register
 *  MSPI flash PMS section n attribute register
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
         *  section n is configured by registers SPI_MEM_C_FMEM__PMSn_ADDR_REG and
         *  SPI_MEM_C_FMEM__PMSn_SIZE_REG.
         */
        uint32_t fmem_pmsn_ecc:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} spi_mem_c_fmem_pmsn_attr_reg_t;

/** Type of fmem_pmsn_addr register
 *  SPI1 flash PMS section n start address register
 */
typedef union {
    struct {
        /** fmem_pmsn_addr_s : R/W; bitpos: [26:0]; default: 0;
         *  SPI1 flash PMS section n start address value
         */
        uint32_t fmem_pmsn_addr_s:27;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_mem_c_fmem_pmsn_addr_reg_t;

/** Type of fmem_pmsn_size register
 *  SPI1 flash PMS section n start address register
 */
typedef union {
    struct {
        /** fmem_pmsn_size : R/W; bitpos: [14:0]; default: 4096;
         *  SPI1 flash PMS section n address region is (SPI_MEM_C_FMEM__PMSn_ADDR_S,
         *  SPI_MEM_C_FMEM__PMSn_ADDR_S + SPI_MEM_C_FMEM__PMSn_SIZE)
         */
        uint32_t fmem_pmsn_size:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} spi_mem_c_fmem_pmsn_size_reg_t;

/** Type of smem_pmsn_attr register
 *  SPI1 flash PMS section n start address register
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
         *  external RAM PMS section n is configured by registers SPI_MEM_C_SMEM_PMSn_ADDR_REG and
         *  SPI_MEM_C_SMEM_PMSn_SIZE_REG.
         */
        uint32_t smem_pmsn_ecc:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} spi_mem_c_smem_pmsn_attr_reg_t;

/** Type of smem_pmsn_addr register
 *  SPI1 external RAM PMS section n start address register
 */
typedef union {
    struct {
        /** smem_pmsn_addr_s : R/W; bitpos: [26:0]; default: 0;
         *  SPI1 external RAM PMS section n start address value
         */
        uint32_t smem_pmsn_addr_s:27;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_mem_c_smem_pmsn_addr_reg_t;

/** Type of smem_pmsn_size register
 *  SPI1 external RAM PMS section n start address register
 */
typedef union {
    struct {
        /** smem_pmsn_size : R/W; bitpos: [14:0]; default: 4096;
         *  SPI1 external RAM PMS section n address region is (SPI_MEM_C_SMEM_PMSn_ADDR_S,
         *  SPI_MEM_C_SMEM_PMSn_ADDR_S + SPI_MEM_C_SMEM_PMSn_SIZE)
         */
        uint32_t smem_pmsn_size:15;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} spi_mem_c_smem_pmsn_size_reg_t;

/** Type of pms_reject register
 *  SPI1 access reject register
 */
typedef union {
    struct {
        /** reject_addr : R/SS/WTC; bitpos: [26:0]; default: 0;
         *  This bits show the first SPI1 access error address. It is cleared by when
         *  spi_mem_c_C_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t reject_addr:27;
        /** pm_en : R/W; bitpos: [27]; default: 0;
         *  Set this bit to enable SPI0/1 transfer permission control function.
         */
        uint32_t pm_en:1;
        /** pms_ld : R/SS/WTC; bitpos: [28]; default: 0;
         *  1: SPI1 write access error. 0: No write access error. It is cleared by when
         *  spi_mem_c_C_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t pms_ld:1;
        /** pms_st : R/SS/WTC; bitpos: [29]; default: 0;
         *  1: SPI1 read access error. 0: No read access error. It is cleared by when
         *  spi_mem_c_C_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t pms_st:1;
        /** pms_multi_hit : R/SS/WTC; bitpos: [30]; default: 0;
         *  1: SPI1 access is rejected because of address miss. 0: No address miss error. It is
         *  cleared by when  spi_mem_c_C_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t pms_multi_hit:1;
        /** pms_ivd : R/SS/WTC; bitpos: [31]; default: 0;
         *  1: SPI1 access is rejected because of address multi-hit. 0: No address multi-hit
         *  error. It is cleared by when  spi_mem_c_C_PMS_REJECT_INT_CLR bit is set.
         */
        uint32_t pms_ivd:1;
    };
    uint32_t val;
} spi_mem_c_pms_reject_reg_t;


/** Group: MSPI ECC registers */
/** Type of ecc_ctrl register
 *  MSPI ECC control register
 */
typedef union {
    struct {
        uint32_t reserved_0:5;
        /** ecc_err_cnt : HRO; bitpos: [10:5]; default: 0;
         *  This bits show the error times of MSPI ECC read. It is cleared by when
         *  spi_mem_c_C_ECC_ERR_INT_CLR bit is set.
         */
        uint32_t ecc_err_cnt:6;
        /** fmem_ecc_err_int_num : HRO; bitpos: [16:11]; default: 10;
         *  Set the error times of MSPI ECC read to generate MSPI spi_mem_c_C_ECC_ERR_INT interrupt.
         */
        uint32_t fmem_ecc_err_int_num:6;
        /** fmem_ecc_err_int_en : HRO; bitpos: [17]; default: 0;
         *  Set this bit to calculate the error times of MSPI ECC read when accesses to flash.
         */
        uint32_t fmem_ecc_err_int_en:1;
        /** fmem_page_size : R/W; bitpos: [19:18]; default: 0;
         *  Set the page size of the flash accessed by MSPI. 0: 256 bytes. 1: 512 bytes. 2:
         *  1024 bytes. 3: 2048 bytes.
         */
        uint32_t fmem_page_size:2;
        /** fmem_ecc_addr_en : HRO; bitpos: [20]; default: 0;
         *  Set this bit to enable MSPI ECC address conversion, no matter MSPI accesses to the
         *  ECC region or non-ECC region of flash. If there is no ECC region in flash, this bit
         *  should be 0. Otherwise, this bit should be 1.
         */
        uint32_t fmem_ecc_addr_en:1;
        /** usr_ecc_addr_en : HRO; bitpos: [21]; default: 0;
         *  Set this bit to enable ECC address convert in SPI0/1 USR_CMD transfer.
         */
        uint32_t usr_ecc_addr_en:1;
        uint32_t reserved_22:2;
        /** ecc_continue_record_err_en : HRO; bitpos: [24]; default: 1;
         *  1: The error information in spi_mem_c_C_ECC_ERR_BITS and spi_mem_c_C_ECC_ERR_ADDR is
         *  updated when there is an ECC error. 0: spi_mem_c_C_ECC_ERR_BITS and
         *  spi_mem_c_C_ECC_ERR_ADDR record the first ECC error information.
         */
        uint32_t ecc_continue_record_err_en:1;
        /** ecc_err_bits : HRO; bitpos: [31:25]; default: 0;
         *  Records the first ECC error bit number in the 16 bytes(From 0~127, corresponding to
         *  byte 0 bit 0 to byte 15 bit 7)
         */
        uint32_t ecc_err_bits:7;
    };
    uint32_t val;
} spi_mem_c_ecc_ctrl_reg_t;

/** Type of ecc_err_addr register
 *  MSPI ECC error address register
 */
typedef union {
    struct {
        /** ecc_err_addr : HRO; bitpos: [26:0]; default: 0;
         *  This bits show the first MSPI ECC error address. It is cleared by when
         *  spi_mem_c_C_ECC_ERR_INT_CLR bit is set.
         */
        uint32_t ecc_err_addr:27;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_mem_c_ecc_err_addr_reg_t;

/** Type of smem_ecc_ctrl register
 *  MSPI ECC control register
 */
typedef union {
    struct {
        uint32_t reserved_0:17;
        /** smem_ecc_err_int_en : HRO; bitpos: [17]; default: 0;
         *  Set this bit to calculate the error times of MSPI ECC read when accesses to
         *  external RAM.
         */
        uint32_t smem_ecc_err_int_en:1;
        /** smem_page_size : HRO; bitpos: [19:18]; default: 2;
         *  Set the page size of the external RAM accessed by MSPI. 0: 256 bytes. 1: 512 bytes.
         *  2: 1024 bytes. 3: 2048 bytes.
         */
        uint32_t smem_page_size:2;
        /** smem_ecc_addr_en : HRO; bitpos: [20]; default: 0;
         *  Set this bit to enable MSPI ECC address conversion, no matter MSPI accesses to the
         *  ECC region or non-ECC region of external RAM. If there is no ECC region in external
         *  RAM, this bit should be 0. Otherwise, this bit should be 1.
         */
        uint32_t smem_ecc_addr_en:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} spi_mem_c_smem_ecc_ctrl_reg_t;


/** Group: Status and state control registers */
/** Type of smem_axi_addr_ctrl register
 *  SPI0 AXI address control register
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** all_fifo_empty : RO; bitpos: [26]; default: 1;
         *  The empty status of all AFIFO and SYNC_FIFO in MSPI module. 1: All AXI transfers
         *  and SPI0 transfers are done. 0: Others.
         */
        uint32_t all_fifo_empty:1;
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
} spi_mem_c_smem_axi_addr_ctrl_reg_t;

/** Type of axi_err_resp_en register
 *  SPI0 AXI error response enable register
 */
typedef union {
    struct {
        /** aw_resp_en_mmu_vld : HRO; bitpos: [0]; default: 0;
         *  Set this bit  to enable AXI response function for mmu valid err in axi write trans.
         */
        uint32_t aw_resp_en_mmu_vld:1;
        /** aw_resp_en_mmu_gid : HRO; bitpos: [1]; default: 0;
         *  Set this bit  to enable AXI response function for mmu gid err in axi write trans.
         */
        uint32_t aw_resp_en_mmu_gid:1;
        /** aw_resp_en_axi_size : HRO; bitpos: [2]; default: 0;
         *  Set this bit  to enable AXI response function for axi size err in axi write trans.
         */
        uint32_t aw_resp_en_axi_size:1;
        /** aw_resp_en_axi_flash : HRO; bitpos: [3]; default: 0;
         *  Set this bit  to enable AXI response function for axi flash err in axi write trans.
         */
        uint32_t aw_resp_en_axi_flash:1;
        /** aw_resp_en_mmu_ecc : HRO; bitpos: [4]; default: 0;
         *  Set this bit  to enable AXI response function for mmu ecc err in axi write trans.
         */
        uint32_t aw_resp_en_mmu_ecc:1;
        /** aw_resp_en_mmu_sens : HRO; bitpos: [5]; default: 0;
         *  Set this bit  to enable AXI response function for mmu sens in err axi write trans.
         */
        uint32_t aw_resp_en_mmu_sens:1;
        /** aw_resp_en_axi_wstrb : HRO; bitpos: [6]; default: 0;
         *  Set this bit  to enable AXI response function for axi wstrb err in axi write trans.
         */
        uint32_t aw_resp_en_axi_wstrb:1;
        /** ar_resp_en_mmu_vld : R/W; bitpos: [7]; default: 0;
         *  Set this bit  to enable AXI response function for mmu valid err in axi read trans.
         */
        uint32_t ar_resp_en_mmu_vld:1;
        /** ar_resp_en_mmu_gid : R/W; bitpos: [8]; default: 0;
         *  Set this bit  to enable AXI response function for mmu gid err in axi read trans.
         */
        uint32_t ar_resp_en_mmu_gid:1;
        /** ar_resp_en_mmu_ecc : R/W; bitpos: [9]; default: 0;
         *  Set this bit  to enable AXI response function for mmu ecc err in axi read trans.
         */
        uint32_t ar_resp_en_mmu_ecc:1;
        /** ar_resp_en_mmu_sens : R/W; bitpos: [10]; default: 0;
         *  Set this bit  to enable AXI response function for mmu sensitive err in axi read
         *  trans.
         */
        uint32_t ar_resp_en_mmu_sens:1;
        /** ar_resp_en_axi_size : R/W; bitpos: [11]; default: 0;
         *  Set this bit  to enable AXI response function for axi size err in axi read trans.
         */
        uint32_t ar_resp_en_axi_size:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} spi_mem_c_axi_err_resp_en_reg_t;


/** Group: Flash timing registers */
/** Type of timing_cali register
 *  SPI0 flash timing calibration register
 */
typedef union {
    struct {
        /** timing_clk_ena : R/W; bitpos: [0]; default: 1;
         *  The bit is used to enable timing adjust clock for all reading operations.
         */
        uint32_t timing_clk_ena:1;
        /** timing_cali : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable timing auto-calibration for all reading operations.
         */
        uint32_t timing_cali:1;
        /** extra_dummy_cyclelen : R/W; bitpos: [4:2]; default: 0;
         *  add extra dummy spi clock cycle length for spi clock calibration.
         */
        uint32_t extra_dummy_cyclelen:3;
        /** dll_timing_cali : HRO; bitpos: [5]; default: 0;
         *  Set this bit to enable DLL for timing calibration in DDR mode when accessed to
         *  flash.
         */
        uint32_t dll_timing_cali:1;
        /** timing_cali_update : WT; bitpos: [6]; default: 0;
         *  Set this bit to update delay mode, delay num and extra dummy in MSPI.
         */
        uint32_t timing_cali_update:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} spi_mem_c_timing_cali_reg_t;

/** Type of din_mode register
 *  MSPI flash input timing delay mode control register
 */
typedef union {
    struct {
        /** din0_mode : R/W; bitpos: [2:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t din0_mode:3;
        /** din1_mode : R/W; bitpos: [5:3]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t din1_mode:3;
        /** din2_mode : R/W; bitpos: [8:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t din2_mode:3;
        /** din3_mode : R/W; bitpos: [11:9]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t din3_mode:3;
        /** din4_mode : R/W; bitpos: [14:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t din4_mode:3;
        /** din5_mode : R/W; bitpos: [17:15]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t din5_mode:3;
        /** din6_mode : R/W; bitpos: [20:18]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t din6_mode:3;
        /** din7_mode : R/W; bitpos: [23:21]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t din7_mode:3;
        /** dins_mode : R/W; bitpos: [26:24]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input
         *  with the spi_clk
         */
        uint32_t dins_mode:3;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_mem_c_din_mode_reg_t;

/** Type of din_num register
 *  MSPI flash input timing delay number control register
 */
typedef union {
    struct {
        /** din0_num : R/W; bitpos: [1:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t din0_num:2;
        /** din1_num : R/W; bitpos: [3:2]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t din1_num:2;
        /** din2_num : R/W; bitpos: [5:4]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t din2_num:2;
        /** din3_num : R/W; bitpos: [7:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t din3_num:2;
        /** din4_num : R/W; bitpos: [9:8]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t din4_num:2;
        /** din5_num : R/W; bitpos: [11:10]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t din5_num:2;
        /** din6_num : R/W; bitpos: [13:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t din6_num:2;
        /** din7_num : R/W; bitpos: [15:14]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t din7_num:2;
        /** dins_num : R/W; bitpos: [17:16]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t dins_num:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} spi_mem_c_din_num_reg_t;

/** Type of dout_mode register
 *  MSPI flash output timing adjustment control register
 */
typedef union {
    struct {
        /** dout0_mode : R/W; bitpos: [0]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t dout0_mode:1;
        /** dout1_mode : R/W; bitpos: [1]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t dout1_mode:1;
        /** dout2_mode : R/W; bitpos: [2]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t dout2_mode:1;
        /** dout3_mode : R/W; bitpos: [3]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t dout3_mode:1;
        /** dout4_mode : R/W; bitpos: [4]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t dout4_mode:1;
        /** dout5_mode : R/W; bitpos: [5]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t dout5_mode:1;
        /** dout6_mode : R/W; bitpos: [6]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t dout6_mode:1;
        /** dout7_mode : R/W; bitpos: [7]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t dout7_mode:1;
        /** douts_mode : R/W; bitpos: [8]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the spi_clk
         */
        uint32_t douts_mode:1;
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
        /** smem_timing_clk_ena : HRO; bitpos: [0]; default: 1;
         *  For sram, the bit is used to enable timing adjust clock for all reading operations.
         */
        uint32_t smem_timing_clk_ena:1;
        /** smem_timing_cali : HRO; bitpos: [1]; default: 0;
         *  For sram, the bit is used to enable timing auto-calibration for all reading
         *  operations.
         */
        uint32_t smem_timing_cali:1;
        /** smem_extra_dummy_cyclelen : HRO; bitpos: [4:2]; default: 0;
         *  For sram, add extra dummy spi clock cycle length for spi clock calibration.
         */
        uint32_t smem_extra_dummy_cyclelen:3;
        /** smem_dll_timing_cali : HRO; bitpos: [5]; default: 0;
         *  Set this bit to enable DLL for timing calibration in DDR mode when accessed to
         *  EXT_RAM.
         */
        uint32_t smem_dll_timing_cali:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} spi_mem_c_smem_timing_cali_reg_t;

/** Type of smem_din_mode register
 *  MSPI external RAM input timing delay mode control register
 */
typedef union {
    struct {
        /** smem_din0_mode : HRO; bitpos: [2:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din0_mode:3;
        /** smem_din1_mode : HRO; bitpos: [5:3]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din1_mode:3;
        /** smem_din2_mode : HRO; bitpos: [8:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din2_mode:3;
        /** smem_din3_mode : HRO; bitpos: [11:9]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din3_mode:3;
        /** smem_din4_mode : HRO; bitpos: [14:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din4_mode:3;
        /** smem_din5_mode : HRO; bitpos: [17:15]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din5_mode:3;
        /** smem_din6_mode : HRO; bitpos: [20:18]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din6_mode:3;
        /** smem_din7_mode : HRO; bitpos: [23:21]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_din7_mode:3;
        /** smem_dins_mode : HRO; bitpos: [26:24]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: input without delayed, 1:
         *  input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input
         *  with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the
         *  spi_clk high edge,  6: input with the spi_clk low edge
         */
        uint32_t smem_dins_mode:3;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} spi_mem_c_smem_din_mode_reg_t;

/** Type of smem_din_num register
 *  MSPI external RAM input timing delay number control register
 */
typedef union {
    struct {
        /** smem_din0_num : HRO; bitpos: [1:0]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din0_num:2;
        /** smem_din1_num : HRO; bitpos: [3:2]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din1_num:2;
        /** smem_din2_num : HRO; bitpos: [5:4]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din2_num:2;
        /** smem_din3_num : HRO; bitpos: [7:6]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din3_num:2;
        /** smem_din4_num : HRO; bitpos: [9:8]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din4_num:2;
        /** smem_din5_num : HRO; bitpos: [11:10]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din5_num:2;
        /** smem_din6_num : HRO; bitpos: [13:12]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din6_num:2;
        /** smem_din7_num : HRO; bitpos: [15:14]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_din7_num:2;
        /** smem_dins_num : HRO; bitpos: [17:16]; default: 0;
         *  the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1:
         *  delayed by 2 cycles,...
         */
        uint32_t smem_dins_num:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} spi_mem_c_smem_din_num_reg_t;

/** Type of smem_dout_mode register
 *  MSPI external RAM output timing adjustment control register
 */
typedef union {
    struct {
        /** smem_dout0_mode : HRO; bitpos: [0]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout0_mode:1;
        /** smem_dout1_mode : HRO; bitpos: [1]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout1_mode:1;
        /** smem_dout2_mode : HRO; bitpos: [2]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout2_mode:1;
        /** smem_dout3_mode : HRO; bitpos: [3]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout3_mode:1;
        /** smem_dout4_mode : HRO; bitpos: [4]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout4_mode:1;
        /** smem_dout5_mode : HRO; bitpos: [5]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout5_mode:1;
        /** smem_dout6_mode : HRO; bitpos: [6]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout6_mode:1;
        /** smem_dout7_mode : HRO; bitpos: [7]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_dout7_mode:1;
        /** smem_douts_mode : HRO; bitpos: [8]; default: 0;
         *  the output signals are delayed by system clock cycles, 0: output without delayed,
         *  1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3:
         *  output with the posedge of clk_160,4 output with the negedge of clk_160,5: output
         *  with the spi_clk high edge ,6: output with the spi_clk low edge
         */
        uint32_t smem_douts_mode:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} spi_mem_c_smem_dout_mode_reg_t;


/** Group: Manual Encryption plaintext Memory */
/** Type of xts_plain_base register
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
/** Type of xts_linesize register
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

/** Type of xts_destination register
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

/** Type of xts_physical_address register
 *  Manual Encryption physical address register
 */
typedef union {
    struct {
        /** xts_physical_address : R/W; bitpos: [25:0]; default: 0;
         *  This bits stores the physical-address parameter which will be used in manual
         *  encryption calculation. This value should aligned with byte number decided by
         *  line-size parameter.
         */
        uint32_t xts_physical_address:26;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} spi_mem_c_xts_physical_address_reg_t;


/** Group: Manual Encryption control and status registers */
/** Type of xts_trigger register
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

/** Type of xts_release register
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

/** Type of xts_destroy register
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

/** Type of xts_state register
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
/** Type of xts_date register
 *  Manual Encryption version register
 */
typedef union {
    struct {
        /** xts_date : R/W; bitpos: [29:0]; default: 538972176;
         *  This bits stores the last modified-time of manual encryption feature.
         */
        uint32_t xts_date:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} spi_mem_c_xts_date_reg_t;


/** Group: MMU access registers */
/** Type of mmu_item_content register
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

/** Type of mmu_item_index register
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
/** Type of mmu_power_ctrl register
 *  MSPI MMU power control register
 */
typedef union {
    struct {
        /** mmu_mem_force_on : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable mmu-memory clock force on
         */
        uint32_t mmu_mem_force_on:1;
        /** mmu_mem_force_pd : R/W; bitpos: [1]; default: 0;
         *  Set this bit to force mmu-memory powerdown
         */
        uint32_t mmu_mem_force_pd:1;
        /** mmu_mem_force_pu : R/W; bitpos: [2]; default: 1;
         *  Set this bit to force mmu-memory powerup, in this case, the power should also be
         *  controlled by rtc.
         */
        uint32_t mmu_mem_force_pu:1;
        /** mmu_page_size : R/W; bitpos: [4:3]; default: 0;
         *  0: Max page size , 1: Max page size/2 , 2: Max page size/4, 3: Max page size/8
         */
        uint32_t mmu_page_size:2;
        uint32_t reserved_5:11;
        /** aux_ctrl : HRO; bitpos: [29:16]; default: 4896;
         *  MMU PSRAM aux control register
         */
        uint32_t aux_ctrl:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} spi_mem_c_mmu_power_ctrl_reg_t;


/** Group: External mem cryption DPA registers */
/** Type of dpa_ctrl register
 *  SPI memory cryption DPA register
 */
typedef union {
    struct {
        /** crypt_security_level : R/W; bitpos: [2:0]; default: 7;
         *  Set the security level of spi mem cryption. 0: Shut off cryption DPA funtion. 1-7:
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


/** Group: Version control register */
/** Type of date register
 *  SPI0 version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36712560;
         *  SPI0 register version.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} spi_mem_c_date_reg_t;


typedef struct spi_mem_c_dev_s {
    volatile spi_mem_c_cmd_reg_t cmd;
    uint32_t reserved_004;
    volatile spi_mem_c_ctrl_reg_t ctrl;
    volatile spi_mem_c_ctrl1_reg_t ctrl1;
    volatile spi_mem_c_ctrl2_reg_t ctrl2;
    volatile spi_mem_c_clock_reg_t clock;
    volatile spi_mem_c_user_reg_t user;
    volatile spi_mem_c_user1_reg_t user1;
    volatile spi_mem_c_user2_reg_t user2;
    uint32_t reserved_024[4];
    volatile spi_mem_c_misc_reg_t misc;
    uint32_t reserved_038;
    volatile spi_mem_c_cache_fctrl_reg_t cache_fctrl;
    uint32_t reserved_040;
    volatile spi_mem_c_sram_cmd_reg_t sram_cmd;
    uint32_t reserved_048[3];
    volatile spi_mem_c_fsm_reg_t fsm;
    uint32_t reserved_058[26];
    volatile spi_mem_c_int_ena_reg_t int_ena;
    volatile spi_mem_c_int_clr_reg_t int_clr;
    volatile spi_mem_c_int_raw_reg_t int_raw;
    volatile spi_mem_c_int_st_reg_t int_st;
    uint32_t reserved_0d0;
    volatile spi_mem_c_ddr_reg_t ddr;
    volatile spi_mem_c_smem_ddr_reg_t smem_ddr;
    uint32_t reserved_0dc[9];
    volatile spi_mem_c_fmem_pmsn_attr_reg_t fmem_pmsn_attr[4];
    volatile spi_mem_c_fmem_pmsn_addr_reg_t fmem_pmsn_addr[4];
    volatile spi_mem_c_fmem_pmsn_size_reg_t fmem_pmsn_size[4];
    volatile spi_mem_c_smem_pmsn_attr_reg_t smem_pmsn_attr[4];
    volatile spi_mem_c_smem_pmsn_addr_reg_t smem_pmsn_addr[4];
    volatile spi_mem_c_smem_pmsn_size_reg_t smem_pmsn_size[4];
    uint32_t reserved_160;
    volatile spi_mem_c_pms_reject_reg_t pms_reject;
    volatile spi_mem_c_ecc_ctrl_reg_t ecc_ctrl;
    volatile spi_mem_c_ecc_err_addr_reg_t ecc_err_addr;
    volatile spi_mem_c_axi_err_addr_reg_t axi_err_addr;
    volatile spi_mem_c_smem_ecc_ctrl_reg_t smem_ecc_ctrl;
    volatile spi_mem_c_smem_axi_addr_ctrl_reg_t smem_axi_addr_ctrl;
    volatile spi_mem_c_axi_err_resp_en_reg_t axi_err_resp_en;
    volatile spi_mem_c_timing_cali_reg_t timing_cali;
    volatile spi_mem_c_din_mode_reg_t din_mode;
    volatile spi_mem_c_din_num_reg_t din_num;
    volatile spi_mem_c_dout_mode_reg_t dout_mode;
    volatile spi_mem_c_smem_timing_cali_reg_t smem_timing_cali;
    volatile spi_mem_c_smem_din_mode_reg_t smem_din_mode;
    volatile spi_mem_c_smem_din_num_reg_t smem_din_num;
    volatile spi_mem_c_smem_dout_mode_reg_t smem_dout_mode;
    volatile spi_mem_c_smem_ac_reg_t smem_ac;
    uint32_t reserved_1a4[23];
    volatile spi_mem_c_clock_gate_reg_t clock_gate;
    uint32_t reserved_204[63];
    volatile spi_mem_c_xts_plain_base_reg_t xts_plain_base;
    uint32_t reserved_304[15];
    volatile spi_mem_c_xts_linesize_reg_t xts_linesize;
    volatile spi_mem_c_xts_destination_reg_t xts_destination;
    volatile spi_mem_c_xts_physical_address_reg_t xts_physical_address;
    volatile spi_mem_c_xts_trigger_reg_t xts_trigger;
    volatile spi_mem_c_xts_release_reg_t xts_release;
    volatile spi_mem_c_xts_destroy_reg_t xts_destroy;
    volatile spi_mem_c_xts_state_reg_t xts_state;
    volatile spi_mem_c_xts_date_reg_t xts_date;
    uint32_t reserved_360[7];
    volatile spi_mem_c_mmu_item_content_reg_t mmu_item_content;
    volatile spi_mem_c_mmu_item_index_reg_t mmu_item_index;
    volatile spi_mem_c_mmu_power_ctrl_reg_t mmu_power_ctrl;
    volatile spi_mem_c_dpa_ctrl_reg_t dpa_ctrl;
    uint32_t reserved_38c[28];
    volatile spi_mem_c_date_reg_t date;
} spi_mem_c_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(spi_mem_c_dev_t) == 0x400, "Invalid size of spi_mem_c_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
