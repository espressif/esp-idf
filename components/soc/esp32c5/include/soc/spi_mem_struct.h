/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct spi_mem_dev_s {
    union {
        struct {
            uint32_t mst_st                        :    4;  /*The current status of SPI0 master FSM: spi0_mst_st. 0: idle state, 1:SPI0_GRANT , 2: program/erase suspend state, 3: SPI0 read data state, 4: wait cache/EDMA sent data is stored in SPI0 TX FIFO, 5: SPI0 write data state.*/
            uint32_t slv_st                        :    4;  /*The current status of SPI0 slave FSM: mspi_st. 0: idle state, 1: preparation state, 2: send command state, 3: send address state, 4: wait state, 5: read data state, 6:write data state, 7: done state, 8: read data end state.*/
            uint32_t reserved8                     :    9;  /*reserved*/
            uint32_t flash_pe                      :    1;  /*In user mode, it is set to indicate that program/erase operation will be triggered. The bit is combined with spi_mem_usr bit. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t usr                           :    1;  /*SPI0 USR_CMD start bit, only used when SPI_MEM_AXI_REQ_EN is cleared.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_hpm                     :    1;  /*Drive Flash into high performance mode.  The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_res                     :    1;  /*This bit combined with reg_resandres bit releases Flash from the power-down state or high performance mode and obtains the devices ID. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_dp                      :    1;  /*Drive Flash into power down.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_ce                      :    1;  /*Chip erase enable. Chip erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_be                      :    1;  /*Block erase enable(32KB) .  Block erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_se                      :    1;  /*Sector erase enable(4KB). Sector erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_pp                      :    1;  /*Page program enable(1 byte ~256 bytes data to be programmed). Page program operation  will be triggered when the bit is set. The bit will be cleared once the operation done .1: enable 0: disable. */
            uint32_t flash_wrsr                    :    1;  /*Write status register enable.   Write status operation  will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_rdsr                    :    1;  /*Read status register-1.  Read status operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_rdid                    :    1;  /*Read JEDEC ID . Read ID command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable. */
            uint32_t flash_wrdi                    :    1;  /*Write flash disable. Write disable command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable. */
            uint32_t flash_wren                    :    1;  /*Write flash enable.  Write enable command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable. */
            uint32_t flash_read                    :    1;  /*Read flash enable. Read flash operation will be triggered when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable. */
        };
        uint32_t val;
    } cmd;
    uint32_t addr;
    union {
        struct {
            uint32_t wdummy_dqs_always_out         :    1;  /*In the dummy phase of an MSPI write data transfer when accesses to flash, the level of SPI_DQS is output by the MSPI controller.*/
            uint32_t wdummy_always_out             :    1;  /*In the dummy phase of an MSPI write data transfer when accesses to flash, the level of SPI_IO[7:0] is output by the MSPI controller.*/
            uint32_t fdummy_rin                    :    1;  /*In an MSPI read data transfer when accesses to flash, the level of SPI_IO[7:0] is output by the MSPI controller in the first half part of dummy phase. It is used to mask invalid SPI_DQS in the half part of dummy phase.*/
            uint32_t fdummy_wout                   :    1;  /*In an MSPI write data transfer when accesses to flash, the level of SPI_IO[7:0] is output by the MSPI controller in the second half part of dummy phase. It is used to pre-drive flash.*/
            uint32_t fdout_oct                     :    1;  /*Apply 8 signals during write-data phase 1:enable 0: disable*/
            uint32_t fdin_oct                      :    1;  /*Apply 8 signals during read-data phase 1:enable 0: disable*/
            uint32_t faddr_oct                     :    1;  /*Apply 8 signals during address phase 1:enable 0: disable*/
            uint32_t reserved7                     :    1;  /*reserved*/
            uint32_t fcmd_quad                     :    1;  /*Apply 4 signals during command phase 1:enable 0: disable*/
            uint32_t fcmd_oct                      :    1;  /*Apply 8 signals during command phase 1:enable 0: disable*/
            uint32_t fcs_crc_en                    :    1;  /*For SPI1,  initialize crc32 module before writing encrypted data to flash. Active low.*/
            uint32_t tx_crc_en                     :    1;  /*For SPI1,  enable crc32 when writing encrypted data to flash. 1: enable 0:disable*/
            uint32_t reserved12                    :    1;  /*reserved*/
            uint32_t fastrd_mode                   :    1;  /*This bit enable the bits: SPI_MEM_FREAD_QIO, SPI_MEM_FREAD_DIO, SPI_MEM_FREAD_QOUT and SPI_MEM_FREAD_DOUT. 1: enable 0: disable. */
            uint32_t fread_dual                    :    1;  /*In the read operations, read-data phase apply 2 signals. 1: enable 0: disable. */
            uint32_t resandres                     :    1;  /*The Device ID is read out to SPI_MEM_RD_STATUS register,  this bit combine with spi_mem_flash_res bit. 1: enable 0: disable. */
            uint32_t reserved16                    :    2;  /*reserved*/
            uint32_t q_pol                         :    1;  /*The bit is used to set MISO line polarity, 1: high 0, low*/
            uint32_t d_pol                         :    1;  /*The bit is used to set MOSI line polarity, 1: high 0, low*/
            uint32_t fread_quad                    :    1;  /*In the read operations read-data phase apply 4 signals. 1: enable 0: disable. */
            uint32_t wp                            :    1;  /*Write protect signal output when SPI is idle.  1: output high, 0: output low. */
            uint32_t wrsr_2b                       :    1;  /*two bytes data will be written to status register when it is set. 1: enable 0: disable. */
            uint32_t fread_dio                     :    1;  /*In the read operations address phase and read-data phase apply 2 signals. 1: enable 0: disable. */
            uint32_t fread_qio                     :    1;  /*In the read operations address phase and read-data phase apply 4 signals. 1: enable 0: disable. */
            uint32_t reserved25                    :    5;  /*reserved*/
            uint32_t dqs_ie_always_on              :    1;  /*When accesses to flash, 1: the IE signals of pads connected to SPI_DQS are always 1. 0: Others.*/
            uint32_t data_ie_always_on             :    1;  /*When accesses to flash, 1: the IE signals of pads connected to SPI_IO[7:0] are always 1. 0: Others.*/
        };
        uint32_t val;
    } ctrl;
    union {
        struct {
            uint32_t clk_mode                      :    2;  /*SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.*/
            uint32_t cs_hold_dly_res               :    10;  /*After RES/DP/HPM command is sent, SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 512) SPI_CLK cycles.*/
            uint32_t reserved2                     :    9;  /*reserved*/
            uint32_t reg_ar_size0_1_support_en     :    1;  /*1: MSPI supports ARSIZE 0~3. When ARSIZE =0~2, MSPI read address is 4*n and reply the real AXI read data back. 0: When ARSIZE 0~1, MSPI reply SLV_ERR.*/
            uint32_t reg_aw_size0_1_support_en     :    1;  /*1: MSPI supports AWSIZE 0~3. 0: When AWSIZE 0~1, MSPI reply SLV_ERR.*/
            uint32_t reg_axi_rdata_back_fast       :    1;  /*1: Reply AXI read data to AXI bus when one AXI read beat data is available. 0: Reply AXI read data to AXI bus when all the read data is available.*/
            uint32_t rresp_ecc_err_en              :    1;  /*1: RRESP is SLV_ERR when there is a ECC error in AXI read data. 0: RRESP is OKAY when there is a ECC error in AXI read data. The ECC error information is recorded in SPI_MEM_ECC_ERR_ADDR_REG.*/
            uint32_t ar_splice_en                  :    1;  /*Set this bit to enable AXI Read Splice-transfer.*/
            uint32_t aw_splice_en                  :    1;  /*Set this bit to enable AXI Write Splice-transfer.*/
            uint32_t ram0_en                       :    1;  /*When SPI_MEM_DUAL_RAM_EN is 0 and SPI_MEM_RAM0_EN is 1, only EXT_RAM0 will be accessed. When SPI_MEM_DUAL_RAM_EN is 0 and SPI_MEM_RAM0_EN is 0, only EXT_RAM1 will be accessed. When SPI_MEM_DUAL_RAM_EN is 1,  EXT_RAM0 and EXT_RAM1 will be accessed at the same time.*/
            uint32_t dual_ram_en                   :    1;  /*Set this bit to enable DUAL-RAM mode, EXT_RAM0 and EXT_RAM1 will be accessed at the same time.*/
            uint32_t fast_write_en                 :    1;  /*Set this bit to write data faster, do not wait write data has been stored in tx_bus_fifo_l2. It will wait 4*T_clk_ctrl to insure the write data has been stored in  tx_bus_fifo_l2.*/
            uint32_t rxfifo_rst                    :    1;  /*The synchronous reset signal for SPI0 RX AFIFO and all the AES_MSPI SYNC FIFO to receive signals from AXI.  Set this bit to reset these FIFO.*/
            uint32_t txfifo_rst                    :    1;  /*The synchronous reset signal for SPI0 TX AFIFO and all the AES_MSPI SYNC FIFO to send signals to AXI. Set this bit to reset these FIFO.*/
        };
        uint32_t val;
    } ctrl1;
    union {
        struct {
            uint32_t cs_setup_time                 :    5;  /*(cycles-1) of prepare phase by SPI Bus clock, this bits are combined with SPI_MEM_CS_SETUP bit.*/
            uint32_t cs_hold_time                  :    5;  /*SPI CS signal is delayed to inactive by SPI bus clock, this bits are combined with SPI_MEM_CS_HOLD bit.*/
            uint32_t ecc_cs_hold_time              :    3;  /*SPI_MEM_CS_HOLD_TIME + SPI_MEM_ECC_CS_HOLD_TIME is the SPI0 CS hold cycle in ECC mode when accessed flash.*/
            uint32_t ecc_skip_page_corner          :    1;  /*1: SPI0 and SPI1 skip page corner when accesses flash. 0: Not skip page corner when accesses flash.*/
            uint32_t ecc_16to18_byte_en            :    1;  /*Set this bit to enable SPI0 and SPI1 ECC 16 bytes data with 2 ECC bytes mode when accesses flash.*/
            uint32_t reserved15                    :    9;  /*reserved*/
            uint32_t split_trans_en                :    1;  /*Set this bit to enable SPI0 split one AXI read flash transfer into two SPI transfers when one transfer will cross flash or EXT_RAM page corner, valid no matter whether there is an ECC region or not.*/
            uint32_t cs_hold_delay                 :    6;  /*These bits are used to set the minimum CS high time tSHSL between SPI burst transfer when accesses to flash. tSHSL is (SPI_MEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.*/
            uint32_t sync_reset                    :    1;  /*The spi0_mst_st and spi0_slv_st will be reset.*/
        };
        uint32_t val;
    } ctrl2;
    union {
        struct {
            uint32_t clkcnt_l                      :    8;  /*In the master mode it must be equal to spi_mem_clkcnt_N. */
            uint32_t clkcnt_h                      :    8;  /*In the master mode it must be floor((spi_mem_clkcnt_N+1)/2-1).*/
            uint32_t clkcnt_n                      :    8;  /*In the master mode it is the divider of spi_mem_clk. So spi_mem_clk frequency is system/(spi_mem_clkcnt_N+1)*/
            uint32_t reserved24                    :    7;  /*In the master mode it is pre-divider of spi_mem_clk. */
            uint32_t clk_equ_sysclk                :    1;  /*1: 1-division mode, the frequency of SPI bus clock equals to that of MSPI module clock.*/
        };
        uint32_t val;
    } clock;
    union {
        struct {
            uint32_t reserved0                     :    6;  /*reserved*/
            uint32_t cs_hold                       :    1;  /*spi cs keep low when spi is in  done  phase. 1: enable 0: disable. */
            uint32_t cs_setup                      :    1;  /*spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable. */
            uint32_t reserved8                     :    1;  /*reserved*/
            uint32_t ck_out_edge                   :    1;  /*The bit combined with SPI_MEM_CK_IDLE_EDGE bit to control SPI clock mode 0~3.*/
            uint32_t reserved10                    :    2;  /*reserved*/
            uint32_t fwrite_dual                   :    1;  /*In the write operations read-data phase apply 2 signals*/
            uint32_t fwrite_quad                   :    1;  /*In the write operations read-data phase apply 4 signals*/
            uint32_t fwrite_dio                    :    1;  /*In the write operations address phase and read-data phase apply 2 signals.*/
            uint32_t fwrite_qio                    :    1;  /*In the write operations address phase and read-data phase apply 4 signals.*/
            uint32_t reserved16                    :    8;  /*reserved*/
            uint32_t usr_miso_highpart             :    1;  /*read-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable. */
            uint32_t usr_mosi_highpart             :    1;  /*write-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable. */
            uint32_t usr_dummy_idle                :    1;  /*spi clock is disable in dummy phase when the bit is enable.*/
            uint32_t usr_mosi                      :    1;  /*This bit enable the write-data phase of an operation.*/
            uint32_t usr_miso                      :    1;  /*This bit enable the read-data phase of an operation.*/
            uint32_t usr_dummy                     :    1;  /*This bit enable the dummy phase of an operation.*/
            uint32_t usr_addr                      :    1;  /*This bit enable the address phase of an operation.*/
            uint32_t usr_command                   :    1;  /*This bit enable the command phase of an operation.*/
        };
        uint32_t val;
    } user;
    union {
        struct {
            uint32_t usr_dummy_cyclelen            :    6;  /*The length in spi_mem_clk cycles of dummy phase. The register value shall be (cycle_num-1).*/
            uint32_t usr_dbytelen                  :    3;  /*SPI0 USR_CMD read or write data byte length -1*/
            uint32_t reserved9                     :    17;  /*reserved*/
            uint32_t usr_addr_bitlen               :    6;  /*The length in bits of address phase. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } user1;
    union {
        struct {
            uint32_t usr_command_value             :    16;  /*The value of  command.*/
            uint32_t reserved16                    :    12;  /*reserved*/
            uint32_t usr_command_bitlen            :    4;  /*The length in bits of command phase. The register value shall be (bit_num-1)*/
        };
        uint32_t val;
    } user2;
    union {
        struct {
            uint32_t usr_mosi_bit_len              :    10;  /*The length in bits of write-data. The register value shall be (bit_num-1).*/
            uint32_t reserved10                    :    22;  /*reserved*/
        };
        uint32_t val;
    } mosi_dlen;
    union {
        struct {
            uint32_t usr_miso_bit_len              :    10;  /*The length in bits of  read-data. The register value shall be (bit_num-1).*/
            uint32_t reserved10                    :    22;  /*reserved*/
        };
        uint32_t val;
    } miso_dlen;
    union {
        struct {
            uint32_t status                        :    16;  /*The value is stored when set spi_mem_flash_rdsr bit and spi_mem_flash_res bit.*/
            uint32_t wb_mode                       :    8;  /*Mode bits in the flash fast read mode  it is combined with spi_mem_fastrd_mode bit.*/
            uint32_t reserved24                    :    8;  /*reserved*/
        };
        uint32_t val;
    } rd_status;
    uint32_t reserved_30;
    union {
        struct {
            uint32_t cs0_dis                       :    1;  /*SPI_CS0 pin enable, 1: disable SPI_CS0, 0: SPI_CS0 pin is active to select SPI device, such as flash, external RAM and so on.*/
            uint32_t cs1_dis                       :    1;  /*SPI_CS1 pin enable, 1: disable SPI_CS1, 0: SPI_CS1 pin is active to select SPI device, such as flash, external RAM and so on.*/
            uint32_t reserved0                     :    5;  /*reserved*/
            uint32_t fsub_pin                      :    1;  /*For SPI0,  flash is connected to SUBPINs.*/
            uint32_t ssub_pin                      :    1;  /*For SPI0,  sram is connected to SUBPINs.*/
            uint32_t ck_idle_edge                  :    1;  /*1: SPI_CLK line is high when idle     0: spi clk line is low when idle */
            uint32_t cs_keep_active                :    1;  /*SPI_CS line keep low when the bit is set.*/
            uint32_t reserved11                    :    21;  /*reserved*/
        };
        uint32_t val;
    } misc;
    uint32_t tx_crc;
    union {
        struct {
            uint32_t axi_req_en                    :    1;  /*For SPI0, AXI master access enable, 1: enable, 0:disable.*/
            uint32_t usr_addr_4byte                :    1;  /*For SPI0,  cache  read flash with 4 bytes address, 1: enable, 0:disable.*/
            uint32_t flash_usr_cmd                 :    1;  /*For SPI0,  cache  read flash for user define command, 1: enable, 0:disable.*/
            uint32_t fdin_dual                     :    1;  /*For SPI0 flash, din phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_fread_dio.*/
            uint32_t fdout_dual                    :    1;  /*For SPI0 flash, dout phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_fread_dio.*/
            uint32_t faddr_dual                    :    1;  /*For SPI0 flash, address phase apply 2 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_dio.*/
            uint32_t fdin_quad                     :    1;  /*For SPI0 flash, din phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.*/
            uint32_t fdout_quad                    :    1;  /*For SPI0 flash, dout phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.*/
            uint32_t faddr_quad                    :    1;  /*For SPI0 flash, address phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.*/
            uint32_t reserved9                     :    21;  /*reserved*/
            uint32_t reg_same_aw_ar_addr_chk_en    :    1;  /*Set this bit to check AXI read/write the same address region.*/
            uint32_t reg_close_axi_inf_en          :    1;  /*Set this bit to close AXI read/write transfer to MSPI, which means that only SLV_ERR will be replied to BRESP/RRESP.*/
        };
        uint32_t val;
    } cache_fctrl;
    union {
        struct {
            uint32_t usr_saddr_4byte               :    1;  /*For SPI0, In the external RAM mode, cache read flash with 4 bytes command, 1: enable, 0:disable.*/
            uint32_t usr_sram_dio                  :    1;  /*For SPI0, In the external RAM mode, spi dual I/O mode enable, 1: enable, 0:disable*/
            uint32_t usr_sram_qio                  :    1;  /*For SPI0, In the external RAM mode, spi quad I/O mode enable, 1: enable, 0:disable*/
            uint32_t usr_wr_sram_dummy             :    1;  /*For SPI0, In the external RAM mode, it is the enable bit of dummy phase for write operations.*/
            uint32_t usr_rd_sram_dummy             :    1;  /*For SPI0, In the external RAM mode, it is the enable bit of dummy phase for read operations.*/
            uint32_t sram_usr_rcmd                 :    1;  /*For SPI0, In the external RAM mode cache read external RAM for user define command.*/
            uint32_t sram_rdummy_cyclelen          :    6;  /*For SPI0, In the external RAM mode, it is the length in bits of read dummy phase. The register value shall be (bit_num-1).*/
            uint32_t reserved12                    :    2;  /*reserved*/
            uint32_t sram_addr_bitlen              :    6;  /*For SPI0, In the external RAM mode, it is the length in bits of address phase. The register value shall be (bit_num-1).*/
            uint32_t sram_usr_wcmd                 :    1;  /*For SPI0, In the external RAM mode cache write sram for user define command*/
            uint32_t sram_oct                      :    1;  /*reserved*/
            uint32_t sram_wdummy_cyclelen          :    6;  /*For SPI0, In the external RAM mode, it is the length in bits of write dummy phase. The register value shall be (bit_num-1).*/
            uint32_t reserved28                    :    4;  /*reserved*/
        };
        uint32_t val;
    } cache_sctrl;
    union {
        struct {
            uint32_t sclk_mode                     :    2;  /*SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is always on.*/
            uint32_t swb_mode                      :    8;  /*Mode bits in the external RAM fast read mode  it is combined with spi_mem_fastrd_mode bit.*/
            uint32_t sdin_dual                     :    1;  /*For SPI0 external RAM , din phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_dio.*/
            uint32_t sdout_dual                    :    1;  /*For SPI0 external RAM , dout phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_dio.*/
            uint32_t saddr_dual                    :    1;  /*For SPI0 external RAM , address phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_dio.*/
            uint32_t reserved13                    :    1;  /*reserved*/
            uint32_t sdin_quad                     :    1;  /*For SPI0 external RAM , din phase apply 4 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_qio.*/
            uint32_t sdout_quad                    :    1;  /*For SPI0 external RAM , dout phase apply 4 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_qio.*/
            uint32_t saddr_quad                    :    1;  /*For SPI0 external RAM , address phase apply 4 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_qio.*/
            uint32_t scmd_quad                     :    1;  /*For SPI0 external RAM , cmd phase apply 4 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_qio.*/
            uint32_t sdin_oct                      :    1;  /*For SPI0 external RAM , din phase apply 8 signals. 1: enable 0: disable. */
            uint32_t sdout_oct                     :    1;  /*For SPI0 external RAM , dout phase apply 8 signals. 1: enable 0: disable. */
            uint32_t saddr_oct                     :    1;  /*For SPI0 external RAM , address phase apply 4 signals. 1: enable 0: disable. */
            uint32_t scmd_oct                      :    1;  /*For SPI0 external RAM , cmd phase apply 8 signals. 1: enable 0: disable. */
            uint32_t sdummy_rin                    :    1;  /*In the dummy phase of a MSPI read data transfer when accesses to external RAM, the signal level of SPI bus is output by the MSPI controller.*/
            uint32_t sdummy_wout                   :    1;  /*In the dummy phase of a MSPI write data transfer when accesses to external RAM, the signal level of SPI bus is output by the MSPI controller.*/
            uint32_t reg_smem_wdummy_dqs_always_out:    1;  /*In the dummy phase of an MSPI write data transfer when accesses to external RAM, the level of SPI_DQS is output by the MSPI controller.*/
            uint32_t reg_smem_wdummy_always_out    :    1;  /*In the dummy phase of an MSPI write data transfer when accesses to external RAM, the level of SPI_IO[7:0] is output by the MSPI controller.*/
            uint32_t reserved26                    :    4;  /*reserved*/
            uint32_t reg_smem_dqs_ie_always_on     :    1;  /*When accesses to external RAM, 1: the IE signals of pads connected to SPI_DQS are always 1. 0: Others.*/
            uint32_t reg_smem_data_ie_always_on    :    1;  /*When accesses to external RAM, 1: the IE signals of pads connected to SPI_IO[7:0] are always 1. 0: Others.*/
        };
        uint32_t val;
    } sram_cmd;
    union {
        struct {
            uint32_t sram_usr_rd_cmd_value         :    16;  /*For SPI0,When cache mode is enable it is the read command value of command phase for sram.*/
            uint32_t reserved16                    :    12;  /*reserved*/
            uint32_t sram_usr_rd_cmd_bitlen        :    4;  /*For SPI0,When cache mode is enable it is the length in bits of command phase for sram. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } sram_drd_cmd;
    union {
        struct {
            uint32_t sram_usr_wr_cmd_value         :    16;  /*For SPI0,When cache mode is enable it is the write command value of command phase for sram.*/
            uint32_t reserved16                    :    12;  /*reserved*/
            uint32_t sram_usr_wr_cmd_bitlen        :    4;  /*For SPI0,When cache mode is enable it is the in bits of command phase  for sram. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } sram_dwr_cmd;
    union {
        struct {
            uint32_t sclkcnt_l                     :    8;  /*For SPI0 external RAM  interface, it must be equal to spi_mem_clkcnt_N.*/
            uint32_t sclkcnt_h                     :    8;  /*For SPI0 external RAM  interface, it must be floor((spi_mem_clkcnt_N+1)/2-1).*/
            uint32_t sclkcnt_n                     :    8;  /*For SPI0 external RAM  interface, it is the divider of spi_mem_clk. So spi_mem_clk frequency is system/(spi_mem_clkcnt_N+1)*/
            uint32_t reserved24                    :    7;  /*reserved*/
            uint32_t sclk_equ_sysclk               :    1;  /*For SPI0 external RAM  interface, 1: spi_mem_clk is eqaul to system 0: spi_mem_clk is divided from system clock.*/
        };
        uint32_t val;
    } sram_clk;
    union {
        struct {
            uint32_t reserved0                     :    7;  /*reserved*/
            uint32_t lock_delay_time               :    5;  /*The lock delay time of SPI0/1 arbiter by spi0_slv_st, after PER is sent by SPI1.*/
            uint32_t reserved12                    :    20;  /*reserved*/
        };
        uint32_t val;
    } fsm;
    uint32_t data_buf[16];
    union {
        struct {
            uint32_t waiti_en                      :    1;  /*1: The hardware will wait idle after SE/PP/WRSR automatically, and hardware auto Suspend/Resume can be enabled. 0: The functions of hardware wait idle and auto Suspend/Resume are not supported.*/
            uint32_t waiti_dummy                   :    1;  /*The dummy phase enable when wait flash idle (RDSR)*/
            uint32_t waiti_addr_en                 :    1;  /*1: Output  address 0 in RDSR or read SUS command transfer. 0: Do not send out address in RDSR or read SUS command transfer.*/
            uint32_t waiti_addr_cyclelen           :    2;  /*When SPI_MEM_WAITI_ADDR_EN is set, the  cycle length of sent out address is (SPI_MEM_WAITI_ADDR_CYCLELEN[1:0] + 1) SPI  bus clock cycles. It is not active when SPI_MEM_WAITI_ADDR_EN is cleared.*/
            uint32_t reserved5                     :    4;  /*reserved*/
            uint32_t waiti_cmd_2b                  :    1;  /*1:The wait idle command bit length is 16. 0: The wait idle command bit length is 8.*/
            uint32_t waiti_dummy_cyclelen          :    6;  /*The dummy cycle length when wait flash idle(RDSR).*/
            uint32_t waiti_cmd                     :    16;  /*The command value to wait flash idle(RDSR).*/
        };
        uint32_t val;
    } flash_waiti_ctrl;
    union {
        struct {
            uint32_t flash_per                     :    1;  /*program erase resume bit, program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_pes                     :    1;  /*program erase suspend bit, program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_per_wait_en             :    1;  /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4 or *128) SPI_CLK cycles after program erase resume command is sent. 0: SPI1 does not wait after program erase resume command is sent. */
            uint32_t flash_pes_wait_en             :    1;  /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4 or *128) SPI_CLK cycles after program erase suspend command is sent. 0: SPI1 does not wait after program erase suspend command is sent. */
            uint32_t pes_per_en                    :    1;  /*Set this bit to enable PES end triggers PER transfer option. If this bit is 0, application should send PER after PES is done.*/
            uint32_t flash_pes_en                  :    1;  /*Set this bit to enable Auto-suspending function.*/
            uint32_t pesr_end_msk                  :    16;  /*The mask value when check SUS/SUS1/SUS2 status bit. If the read status value is status_in[15:0](only status_in[7:0] is valid when only one byte of data is read out, status_in[15:0] is valid when two bytes of data are read out), SUS/SUS1/SUS2 = status_in[15:0]^ SPI_MEM_PESR_END_MSK[15:0].*/
            uint32_t frd_sus_2b                    :    1;  /*1: Read two bytes when check flash SUS/SUS1/SUS2 status bit. 0:  Read one byte when check flash SUS/SUS1/SUS2 status bit*/
            uint32_t per_end_en                    :    1;  /*1: Both WIP and SUS/SUS1/SUS2 bits should be checked to insure the resume status of flash. 0: Only need to check WIP is 0.*/
            uint32_t pes_end_en                    :    1;  /*1: Both WIP and SUS/SUS1/SUS2 bits should be checked to insure the suspend status of flash. 0: Only need to check WIP is 0.*/
            uint32_t sus_timeout_cnt               :    7;  /*When SPI1 checks SUS/SUS1/SUS2 bits fail for SPI_MEM_SUS_TIMEOUT_CNT[6:0] times, it will be treated as check pass.*/
        };
        uint32_t val;
    } flash_sus_ctrl;
    union {
        struct {
            uint32_t flash_pes_command             :    16;  /*Program/Erase suspend command.*/
            uint32_t wait_pesr_command             :    16;  /*Flash SUS/SUS1/SUS2 status bit read command. The command should be sent when SUS/SUS1/SUS2 bit should be checked to insure the suspend or resume status of flash.*/
        };
        uint32_t val;
    } flash_sus_cmd;
    union {
        struct {
            uint32_t flash_sus                     :    1;  /*The status of flash suspend, only used in SPI1.*/
            uint32_t wait_pesr_cmd_2b              :    1;  /*1: SPI1 sends out SPI_MEM_WAIT_PESR_COMMAND[15:0] to check SUS/SUS1/SUS2 bit. 0: SPI1 sends out SPI_MEM_WAIT_PESR_COMMAND[7:0] to check SUS/SUS1/SUS2 bit.*/
            uint32_t flash_hpm_dly_128             :    1;  /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after HPM command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after HPM command is sent.*/
            uint32_t flash_res_dly_128             :    1;  /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after RES command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after RES command is sent.*/
            uint32_t flash_dp_dly_128              :    1;  /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after DP command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after DP command is sent.*/
            uint32_t flash_per_dly_128             :    1;  /*Valid when SPI_MEM_FLASH_PER_WAIT_EN is 1. 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after PER command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PER command is sent.*/
            uint32_t flash_pes_dly_128             :    1;  /*Valid when SPI_MEM_FLASH_PES_WAIT_EN is 1. 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after PES command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PES command is sent.*/
            uint32_t spi0_lock_en                  :    1;  /*1: Enable SPI0 lock SPI0/1 arbiter option. 0: Disable it.*/
            uint32_t reserved8                     :    7;  /*reserved*/
            uint32_t flash_pesr_cmd_2b             :    1;  /*1: The bit length of Program/Erase Suspend/Resume command is 16. 0: The bit length of Program/Erase Suspend/Resume command is 8.*/
            uint32_t flash_per_command             :    16;  /*Program/Erase resume command.*/
        };
        uint32_t val;
    } sus_status;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    union {
        struct {
            uint32_t per_end_en                    :    1;  /*The enable bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end_en                    :    1;  /*The enable bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t wpe_end_en                    :    1;  /*The enable bit for SPI_MEM_WPE_END_INT interrupt.*/
            uint32_t slv_st_end_en                 :    1;  /*The enable bit for SPI_MEM_SLV_ST_END_INT interrupt.*/
            uint32_t mst_st_end_en                 :    1;  /*The enable bit for SPI_MEM_MST_ST_END_INT interrupt.*/
            uint32_t ecc_err_en                    :    1;  /*The enable bit for SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t pms_reject_en                 :    1;  /*The enable bit for SPI_MEM_PMS_REJECT_INT interrupt.*/
            uint32_t axi_raddr_err_en              :    1;  /*The enable bit for SPI_MEM_AXI_RADDR_ERR_INT interrupt.*/
            uint32_t axi_wr_flash_err_en           :    1;  /*The enable bit for SPI_MEM_AXI_WR_FALSH_ERR_INT interrupt.*/
            uint32_t axi_waddr_err_en              :    1;  /*The enable bit for SPI_MEM_AXI_WADDR_ERR_INT interrupt.*/
            uint32_t brown_out_en                  :    1;  /*The enable bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t reserved11                    :    21;  /*reserved*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t per_end                       :    1;  /*The clear bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end                       :    1;  /*The clear bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t wpe_end                       :    1;  /*The clear bit for SPI_MEM_WPE_END_INT interrupt.*/
            uint32_t slv_st_end                    :    1;  /*The clear bit for SPI_MEM_SLV_ST_END_INT interrupt.*/
            uint32_t mst_st_end                    :    1;  /*The clear bit for SPI_MEM_MST_ST_END_INT interrupt.*/
            uint32_t ecc_err                       :    1;  /*The clear bit for SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t pms_reject                    :    1;  /*The clear bit for SPI_MEM_PMS_REJECT_INT interrupt.*/
            uint32_t axi_raddr_err                 :    1;  /*The clear bit for SPI_MEM_AXI_RADDR_ERR_INT interrupt.*/
            uint32_t axi_wr_flash_err              :    1;  /*The clear bit for SPI_MEM_AXI_WR_FALSH_ERR_INT interrupt.*/
            uint32_t axi_waddr_err                 :    1;  /*The clear bit for SPI_MEM_AXI_WADDR_ERR_INT interrupt.*/
            uint32_t brown_out                     :    1;  /*The status bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t reserved11                    :    21;  /*reserved*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t per_end                       :    1;  /*The raw bit for SPI_MEM_PER_END_INT interrupt. 1: Triggered when Auto Resume command (0x7A) is sent and flash is resumed successfully. 0: Others.*/
            uint32_t pes_end                       :    1;  /*The raw bit for SPI_MEM_PES_END_INT interrupt.1: Triggered when Auto Suspend command (0x75) is sent and flash is suspended successfully. 0: Others.*/
            uint32_t wpe_end                       :    1;  /*The raw bit for SPI_MEM_WPE_END_INT interrupt. 1: Triggered when WRSR/PP/SE/BE/CE is sent and flash is already idle. 0: Others.*/
            uint32_t slv_st_end                    :    1;  /*The raw bit for SPI_MEM_SLV_ST_END_INT interrupt. 1: Triggered when spi0_slv_st is changed from non idle state to idle state. It means that SPI_CS raises high. 0: Others*/
            uint32_t mst_st_end                    :    1;  /*The raw bit for SPI_MEM_MST_ST_END_INT interrupt. 1: Triggered when spi0_mst_st is changed from non idle state to idle state. 0: Others.*/
            uint32_t ecc_err                       :    1;  /*The raw bit for SPI_MEM_ECC_ERR_INT interrupt. When SPI_FMEM_ECC_ERR_INT_EN is set and  SPI_SMEM_ECC_ERR_INT_EN is cleared, this bit is triggered when the error times of SPI0/1 ECC read flash are equal or bigger than SPI_MEM_ECC_ERR_INT_NUM. When SPI_FMEM_ECC_ERR_INT_EN is cleared and  SPI_SMEM_ECC_ERR_INT_EN is set, this bit is triggered when the error times of SPI0/1 ECC read external RAM are equal or bigger than SPI_MEM_ECC_ERR_INT_NUM. When SPI_FMEM_ECC_ERR_INT_EN and  SPI_SMEM_ECC_ERR_INT_EN are set, this bit is triggered when the total error times of SPI0/1 ECC read external RAM and flash are equal or bigger than SPI_MEM_ECC_ERR_INT_NUM. When SPI_FMEM_ECC_ERR_INT_EN and  SPI_SMEM_ECC_ERR_INT_EN are cleared, this bit will not be triggered.*/
            uint32_t pms_reject                    :    1;  /*The raw bit for SPI_MEM_PMS_REJECT_INT interrupt. 1: Triggered when SPI1 access is rejected. 0: Others.*/
            uint32_t axi_raddr_err                 :    1;  /*The raw bit for SPI_MEM_AXI_RADDR_ERR_INT interrupt. 1: Triggered when AXI read address is invalid by compared to MMU configuration. 0: Others.*/
            uint32_t axi_wr_flash_err              :    1;  /*The raw bit for SPI_MEM_AXI_WR_FALSH_ERR_INT interrupt. 1: Triggered when AXI write flash request is received. 0: Others.*/
            uint32_t axi_waddr_err                 :    1;  /*The raw bit for SPI_MEM_AXI_WADDR_ERR_INT interrupt. 1: Triggered when AXI write address is invalid by compared to MMU configuration. 0: Others.*/
            uint32_t brown_out                     :    1;  /*The raw bit for SPI_MEM_BROWN_OUT_INT interrupt. 1: Triggered condition is that chip is loosing power and RTC module sends out brown out close flash request to SPI1. After SPI1 sends out suspend command to flash, this interrupt is triggered and MSPI returns to idle state. 0: Others.*/
            uint32_t reserved11                    :    21;  /*reserved*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t per_end                       :    1;  /*The status bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end                       :    1;  /*The status bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t wpe_end                       :    1;  /*The status bit for SPI_MEM_WPE_END_INT interrupt.*/
            uint32_t slv_st_end                    :    1;  /*The status bit for SPI_MEM_SLV_ST_END_INT interrupt.*/
            uint32_t mst_st_end                    :    1;  /*The status bit for SPI_MEM_MST_ST_END_INT interrupt.*/
            uint32_t ecc_err                       :    1;  /*The status bit for SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t pms_reject                    :    1;  /*The status bit for SPI_MEM_PMS_REJECT_INT interrupt.*/
            uint32_t axi_raddr_err                 :    1;  /*The enable bit for SPI_MEM_AXI_RADDR_ERR_INT interrupt.*/
            uint32_t axi_wr_flash_err              :    1;  /*The enable bit for SPI_MEM_AXI_WR_FALSH_ERR_INT interrupt.*/
            uint32_t axi_waddr_err                 :    1;  /*The enable bit for SPI_MEM_AXI_WADDR_ERR_INT interrupt.*/
            uint32_t brown_out                     :    1;  /*The status bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t reserved11                    :    21;  /*reserved*/
        };
        uint32_t val;
    } int_st;
    uint32_t reserved_d0;
    union {
        struct {
            uint32_t reg_fmem_ddr_en               :    1;  /*1: in DDR mode,  0 in SDR mode*/
            uint32_t reg_fmem_var_dummy            :    1;  /*Set the bit to enable variable dummy cycle in spi DDR mode.*/
            uint32_t reg_fmem_ddr_rdat_swp         :    1;  /*Set the bit to reorder rx data of the word in spi DDR mode.*/
            uint32_t reg_fmem_ddr_wdat_swp         :    1;  /*Set the bit to reorder tx data of the word in spi DDR mode.*/
            uint32_t reg_fmem_ddr_cmd_dis          :    1;  /*the bit is used to disable dual edge in command phase when DDR mode.*/
            uint32_t reg_fmem_outminbytelen        :    7;  /*It is the minimum output data length in the panda device.*/
            uint32_t reg_fmem_tx_ddr_msk_en        :    1;  /*Set this bit to mask the first or the last byte in SPI0 ECC DDR write mode, when accesses to flash.*/
            uint32_t reg_fmem_rx_ddr_msk_en        :    1;  /*Set this bit to mask the first or the last byte in SPI0 ECC DDR read mode, when accesses to flash.*/
            uint32_t reg_fmem_usr_ddr_dqs_thd      :    7;  /*The delay number of data strobe which from memory based on SPI clock.*/
            uint32_t reg_fmem_ddr_dqs_loop         :    1;  /*1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when spi0_slv_st is in SPI_MEM_DIN state. It is used when there is no SPI_DQS signal or SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and negative edge of SPI_DQS.*/
            uint32_t reserved22                    :    2;  /*reserved*/
            uint32_t reg_fmem_clk_diff_en          :    1;  /*Set this bit to enable the differential SPI_CLK#.*/
            uint32_t reserved25                    :    1;  /*reserved*/
            uint32_t reg_fmem_dqs_ca_in            :    1;  /*Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.*/
            uint32_t reg_fmem_hyperbus_dummy_2x    :    1;  /*Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0 accesses flash or SPI1 accesses flash or sram.*/
            uint32_t reg_fmem_clk_diff_inv         :    1;  /*Set this bit to invert SPI_DIFF when accesses to flash. .*/
            uint32_t reg_fmem_octa_ram_addr        :    1;  /*Set this bit to enable octa_ram address out when accesses to flash, which means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.*/
            uint32_t reg_fmem_hyperbus_ca          :    1;  /*Set this bit to enable HyperRAM address out when accesses to flash, which means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.*/
            uint32_t reserved31                    :    1;  /*reserved*/
        };
        uint32_t val;
    } ddr;
    union {
        struct {
            uint32_t reg_smem_ddr_en               :    1;  /*1: in DDR mode,  0 in SDR mode*/
            uint32_t reg_smem_var_dummy            :    1;  /*Set the bit to enable variable dummy cycle in spi DDR mode.*/
            uint32_t reg_smem_ddr_rdat_swp         :    1;  /*Set the bit to reorder rx data of the word in spi DDR mode.*/
            uint32_t reg_smem_ddr_wdat_swp         :    1;  /*Set the bit to reorder tx data of the word in spi DDR mode.*/
            uint32_t reg_smem_ddr_cmd_dis          :    1;  /*the bit is used to disable dual edge in command phase when DDR mode.*/
            uint32_t reg_smem_outminbytelen        :    7;  /*It is the minimum output data length in the DDR psram.*/
            uint32_t reg_smem_tx_ddr_msk_en        :    1;  /*Set this bit to mask the first or the last byte in SPI0 ECC DDR write mode, when accesses to external RAM.*/
            uint32_t reg_smem_rx_ddr_msk_en        :    1;  /*Set this bit to mask the first or the last byte in SPI0 ECC DDR read mode, when accesses to external RAM.*/
            uint32_t reg_smem_usr_ddr_dqs_thd      :    7;  /*The delay number of data strobe which from memory based on SPI clock.*/
            uint32_t reg_smem_ddr_dqs_loop         :    1;  /*1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when spi0_slv_st is in SPI_MEM_DIN state. It is used when there is no SPI_DQS signal or SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and negative edge of SPI_DQS.*/
            uint32_t reserved22                    :    2;  /*reserved*/
            uint32_t reg_smem_clk_diff_en          :    1;  /*Set this bit to enable the differential SPI_CLK#.*/
            uint32_t reserved25                    :    1;  /*reserved*/
            uint32_t reg_smem_dqs_ca_in            :    1;  /*Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.*/
            uint32_t reg_smem_hyperbus_dummy_2x    :    1;  /*Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0 accesses flash or SPI1 accesses flash or sram.*/
            uint32_t reg_smem_clk_diff_inv         :    1;  /*Set this bit to invert SPI_DIFF when accesses to external RAM. .*/
            uint32_t reg_smem_octa_ram_addr        :    1;  /*Set this bit to enable octa_ram address out when accesses to external RAM, which means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.*/
            uint32_t reg_smem_hyperbus_ca          :    1;  /*Set this bit to enable HyperRAM address out when accesses to external RAM, which means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.*/
            uint32_t reserved31                    :    1;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_ddr;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t reserved_fc;
    union {
        struct {
            uint32_t reg_fmem_pms0_rd_attr         :    1;  /*1: SPI1 flash ACE section $n read accessible. 0: Not allowed.*/
            uint32_t reg_fmem_pms0_wr_attr         :    1;  /*1: SPI1 flash ACE section $n write accessible. 0: Not allowed.*/
            uint32_t reg_fmem_pms0_ecc             :    1;  /*SPI1 flash ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The flash ACE section $n is configured by registers SPI_FMEM_PMS$n_ADDR_REG and SPI_FMEM_PMS$n_SIZE_REG.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms0_attr;
    union {
        struct {
            uint32_t reg_fmem_pms1_rd_attr         :    1;  /*1: SPI1 flash ACE section $n read accessible. 0: Not allowed.*/
            uint32_t reg_fmem_pms1_wr_attr         :    1;  /*1: SPI1 flash ACE section $n write accessible. 0: Not allowed.*/
            uint32_t reg_fmem_pms1_ecc             :    1;  /*SPI1 flash ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The flash ACE section $n is configured by registers SPI_FMEM_PMS$n_ADDR_REG and SPI_FMEM_PMS$n_SIZE_REG.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms1_attr;
    union {
        struct {
            uint32_t reg_fmem_pms2_rd_attr         :    1;  /*1: SPI1 flash ACE section $n read accessible. 0: Not allowed.*/
            uint32_t reg_fmem_pms2_wr_attr         :    1;  /*1: SPI1 flash ACE section $n write accessible. 0: Not allowed.*/
            uint32_t reg_fmem_pms2_ecc             :    1;  /*SPI1 flash ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The flash ACE section $n is configured by registers SPI_FMEM_PMS$n_ADDR_REG and SPI_FMEM_PMS$n_SIZE_REG.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms2_attr;
    union {
        struct {
            uint32_t reg_fmem_pms3_rd_attr         :    1;  /*1: SPI1 flash ACE section $n read accessible. 0: Not allowed.*/
            uint32_t reg_fmem_pms3_wr_attr         :    1;  /*1: SPI1 flash ACE section $n write accessible. 0: Not allowed.*/
            uint32_t reg_fmem_pms3_ecc             :    1;  /*SPI1 flash ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The flash ACE section $n is configured by registers SPI_FMEM_PMS$n_ADDR_REG and SPI_FMEM_PMS$n_SIZE_REG.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms3_attr;
    union {
        struct {
            uint32_t reg_fmem_pms0_addr_s          :    26;  /*SPI1 flash ACE section $n start address value*/
            uint32_t reserved26                    :    6;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms0_addr;
    union {
        struct {
            uint32_t reg_fmem_pms1_addr_s          :    26;  /*SPI1 flash ACE section $n start address value*/
            uint32_t reserved26                    :    6;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms1_addr;
    union {
        struct {
            uint32_t reg_fmem_pms2_addr_s          :    26;  /*SPI1 flash ACE section $n start address value*/
            uint32_t reserved26                    :    6;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms2_addr;
    union {
        struct {
            uint32_t reg_fmem_pms3_addr_s          :    26;  /*SPI1 flash ACE section $n start address value*/
            uint32_t reserved26                    :    6;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms3_addr;
    union {
        struct {
            uint32_t reg_fmem_pms0_size            :    14;  /*SPI1 flash ACE section $n address region is (SPI_FMEM_PMS$n_ADDR_S, SPI_FMEM_PMS$n_ADDR_S + SPI_FMEM_PMS$n_SIZE)*/
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms0_size;
    union {
        struct {
            uint32_t reg_fmem_pms1_size            :    14;  /*SPI1 flash ACE section $n address region is (SPI_FMEM_PMS$n_ADDR_S, SPI_FMEM_PMS$n_ADDR_S + SPI_FMEM_PMS$n_SIZE)*/
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms1_size;
    union {
        struct {
            uint32_t reg_fmem_pms2_size            :    14;  /*SPI1 flash ACE section $n address region is (SPI_FMEM_PMS$n_ADDR_S, SPI_FMEM_PMS$n_ADDR_S + SPI_FMEM_PMS$n_SIZE)*/
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms2_size;
    union {
        struct {
            uint32_t reg_fmem_pms3_size            :    14;  /*SPI1 flash ACE section $n address region is (SPI_FMEM_PMS$n_ADDR_S, SPI_FMEM_PMS$n_ADDR_S + SPI_FMEM_PMS$n_SIZE)*/
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } spi_fmem_pms3_size;
    union {
        struct {
            uint32_t reg_smem_pms0_rd_attr         :    1;  /*1: SPI1 external RAM ACE section $n read accessible. 0: Not allowed.*/
            uint32_t reg_smem_pms0_wr_attr         :    1;  /*1: SPI1 external RAM ACE section $n write accessible. 0: Not allowed.*/
            uint32_t reg_smem_pms0_ecc             :    1;  /*SPI1 external RAM ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The external RAM ACE section $n is configured by registers SPI_SMEM_PMS$n_ADDR_REG and SPI_SMEM_PMS$n_SIZE_REG.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms0_attr;
    union {
        struct {
            uint32_t reg_smem_pms1_rd_attr         :    1;  /*1: SPI1 external RAM ACE section $n read accessible. 0: Not allowed.*/
            uint32_t reg_smem_pms1_wr_attr         :    1;  /*1: SPI1 external RAM ACE section $n write accessible. 0: Not allowed.*/
            uint32_t reg_smem_pms1_ecc             :    1;  /*SPI1 external RAM ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The external RAM ACE section $n is configured by registers SPI_SMEM_PMS$n_ADDR_REG and SPI_SMEM_PMS$n_SIZE_REG.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms1_attr;
    union {
        struct {
            uint32_t reg_smem_pms2_rd_attr         :    1;  /*1: SPI1 external RAM ACE section $n read accessible. 0: Not allowed.*/
            uint32_t reg_smem_pms2_wr_attr         :    1;  /*1: SPI1 external RAM ACE section $n write accessible. 0: Not allowed.*/
            uint32_t reg_smem_pms2_ecc             :    1;  /*SPI1 external RAM ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The external RAM ACE section $n is configured by registers SPI_SMEM_PMS$n_ADDR_REG and SPI_SMEM_PMS$n_SIZE_REG.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms2_attr;
    union {
        struct {
            uint32_t reg_smem_pms3_rd_attr         :    1;  /*1: SPI1 external RAM ACE section $n read accessible. 0: Not allowed.*/
            uint32_t reg_smem_pms3_wr_attr         :    1;  /*1: SPI1 external RAM ACE section $n write accessible. 0: Not allowed.*/
            uint32_t reg_smem_pms3_ecc             :    1;  /*SPI1 external RAM ACE section $n ECC mode, 1: enable ECC mode. 0: Disable it. The external RAM ACE section $n is configured by registers SPI_SMEM_PMS$n_ADDR_REG and SPI_SMEM_PMS$n_SIZE_REG.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms3_attr;
    union {
        struct {
            uint32_t reg_smem_pms0_addr_s          :    26;  /*SPI1 external RAM ACE section $n start address value*/
            uint32_t reserved26                    :    6;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms0_addr;
    union {
        struct {
            uint32_t reg_smem_pms1_addr_s          :    26;  /*SPI1 external RAM ACE section $n start address value*/
            uint32_t reserved26                    :    6;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms1_addr;
    union {
        struct {
            uint32_t reg_smem_pms2_addr_s          :    26;  /*SPI1 external RAM ACE section $n start address value*/
            uint32_t reserved26                    :    6;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms2_addr;
    union {
        struct {
            uint32_t reg_smem_pms3_addr_s          :    26;  /*SPI1 external RAM ACE section $n start address value*/
            uint32_t reserved26                    :    6;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms3_addr;
    union {
        struct {
            uint32_t reg_smem_pms0_size            :    14;  /*SPI1 external RAM ACE section $n address region is (SPI_SMEM_PMS$n_ADDR_S, SPI_SMEM_PMS$n_ADDR_S + SPI_SMEM_PMS$n_SIZE)*/
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms0_size;
    union {
        struct {
            uint32_t reg_smem_pms1_size            :    14;  /*SPI1 external RAM ACE section $n address region is (SPI_SMEM_PMS$n_ADDR_S, SPI_SMEM_PMS$n_ADDR_S + SPI_SMEM_PMS$n_SIZE)*/
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms1_size;
    union {
        struct {
            uint32_t reg_smem_pms2_size            :    14;  /*SPI1 external RAM ACE section $n address region is (SPI_SMEM_PMS$n_ADDR_S, SPI_SMEM_PMS$n_ADDR_S + SPI_SMEM_PMS$n_SIZE)*/
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms2_size;
    union {
        struct {
            uint32_t reg_smem_pms3_size            :    14;  /*SPI1 external RAM ACE section $n address region is (SPI_SMEM_PMS$n_ADDR_S, SPI_SMEM_PMS$n_ADDR_S + SPI_SMEM_PMS$n_SIZE)*/
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_pms3_size;
    uint32_t reserved_160;
    union {
        struct {
            uint32_t reject_addr                   :    26;  /*This bits show the first SPI1 access error address. It is cleared by when  SPI_MEM_PMS_REJECT_INT_CLR bit is set. */
            uint32_t pm_en                         :    1;  /*Set this bit to enable SPI0/1 transfer permission control function.*/
            uint32_t reserved27                    :    1;  /*reserved*/
            uint32_t pms_ld                        :    1;  /*1: SPI1 write access error. 0: No write access error. It is cleared by when  SPI_MEM_PMS_REJECT_INT_CLR bit is set. */
            uint32_t pms_st                        :    1;  /*1: SPI1 read access error. 0: No read access error. It is cleared by when  SPI_MEM_PMS_REJECT_INT_CLR bit is set. */
            uint32_t pms_multi_hit                 :    1;  /*1: SPI1 access is rejected because of address miss. 0: No address miss error. It is cleared by when  SPI_MEM_PMS_REJECT_INT_CLR bit is set. */
            uint32_t pms_ivd                       :    1;  /*1: SPI1 access is rejected because of address multi-hit. 0: No address multi-hit error. It is cleared by when  SPI_MEM_PMS_REJECT_INT_CLR bit is set. */
        };
        uint32_t val;
    } pms_reject;
    union {
        struct {
            uint32_t reserved0                     :    11;  /*reserved*/
            uint32_t ecc_err_int_num               :    6;  /*Set the error times of MSPI ECC read to generate MSPI SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t reg_fmem_ecc_err_int_en       :    1;  /*Set this bit to calculate the error times of MSPI ECC read when accesses to flash.*/
            uint32_t reg_fmem_page_size            :    2;  /*Set the page size of the flash accessed by MSPI. 0: 256 bytes. 1: 512 bytes. 2: 1024 bytes. 3: 2048 bytes.*/
            uint32_t reg_fmem_ecc_addr_en          :    1;  /*Set this bit to enable MSPI ECC address conversion, no matter MSPI accesses to the ECC region or non-ECC region of flash. If there is no ECC region in flash, this bit should be 0. Otherwise, this bit should be 1.*/
            uint32_t usr_ecc_addr_en               :    1;  /*Set this bit to enable ECC address convert in SPI0/1 USR_CMD transfer.*/
            uint32_t reserved22                    :    2;  /*reserved*/
            uint32_t ecc_continue_record_err_en    :    1;  /*1: The error information in SPI_MEM_ECC_ERR_BITS and SPI_MEM_ECC_ERR_ADDR is updated when there is an ECC error. 0: SPI_MEM_ECC_ERR_BITS and SPI_MEM_ECC_ERR_ADDR record the first ECC error information.*/
            uint32_t ecc_err_bits                  :    7;  /*Records the first ECC error bit number in the 16 bytes(From 0~127, corresponding to byte 0 bit 0 to byte 15 bit 7)*/
        };
        uint32_t val;
    } ecc_ctrl;
    union {
        struct {
            uint32_t ecc_err_addr                  :    26;  /*This bits show the first MSPI ECC error address. It is cleared by when  SPI_MEM_ECC_ERR_INT_CLR bit is set. */
            uint32_t ecc_err_cnt                   :    6;  /*This bits show the error times of MSPI ECC read. It is cleared by when  SPI_MEM_ECC_ERR_INT_CLR bit is set. */
        };
        uint32_t val;
    } ecc_err_addr;
    union {
        struct {
            uint32_t axi_err_addr                  :    26;  /*This bits show the first AXI write/read invalid error or AXI write flash error address. It is cleared by when SPI_MEM_AXI_WADDR_ERR_INT_CLR, SPI_MEM_AXI_WR_FLASH_ERR_IN_CLR or SPI_MEM_AXI_RADDR_ERR_IN_CLR bit is set. */
            uint32_t all_fifo_empty                :    1;  /*The empty status of all AFIFO and SYNC_FIFO in MSPI module. 1: All AXI transfers and SPI0 transfers are done. 0: Others.*/
            uint32_t reg_rdata_afifo_rempty        :    1;  /*1: RDATA_AFIFO is empty. 0: At least one AXI read transfer is pending.*/
            uint32_t reg_raddr_afifo_rempty        :    1;  /*1: AXI_RADDR_CTL_AFIFO is empty. 0: At least one AXI read transfer is pending.*/
            uint32_t reg_wdata_afifo_rempty        :    1;  /*1: WDATA_AFIFO is empty. 0: At least one AXI write transfer is pending.*/
            uint32_t reg_wblen_afifo_rempty        :    1;  /*1: WBLEN_AFIFO is empty. 0: At least one AXI write transfer is pending.*/
            uint32_t reg_all_axi_trans_afifo_empty :    1;  /*This bit is set when WADDR_AFIFO, WBLEN_AFIFO, WDATA_AFIFO, AXI_RADDR_CTL_AFIFO and RDATA_AFIFO are empty and spi0_mst_st is IDLE.*/
        };
        uint32_t val;
    } axi_err_addr;
    union {
        struct {
            uint32_t reserved0                     :    17;  /*reserved*/
            uint32_t reg_smem_ecc_err_int_en       :    1;  /*Set this bit to calculate the error times of MSPI ECC read when accesses to external RAM.*/
            uint32_t reg_smem_page_size            :    2;  /*Set the page size of the external RAM accessed by MSPI. 0: 256 bytes. 1: 512 bytes. 2: 1024 bytes. 3: 2048 bytes.*/
            uint32_t reg_smem_ecc_addr_en          :    1;  /*Set this bit to enable MSPI ECC address conversion, no matter MSPI accesses to the ECC region or non-ECC region of external RAM. If there is no ECC region in external RAM, this bit should be 0. Otherwise, this bit should be 1.*/
            uint32_t reserved21                    :    11;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_ecc_ctrl;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    union {
        struct {
            uint32_t timing_clk_ena                :    1;  /*The bit is used to enable timing adjust clock for all reading operations.*/
            uint32_t timing_cali                   :    1;  /*The bit is used to enable timing auto-calibration for all reading operations.*/
            uint32_t extra_dummy_cyclelen          :    3;  /*add extra dummy spi clock cycle length for spi clock calibration.*/
            uint32_t dll_timing_cali               :    1;  /*Set this bit to enable DLL for timing calibration in DDR mode when accessed to flash.*/
            uint32_t timing_cali_update            :    1;  /*Set this bit to update delay mode, delay num and extra dummy in MSPI.*/
            uint32_t reserved7                     :    25;  /*reserved*/
        };
        uint32_t val;
    } timing_cali;
    union {
        struct {
            uint32_t din0_mode                     :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t din1_mode                     :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t din2_mode                     :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t din3_mode                     :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t din4_mode                     :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk*/
            uint32_t din5_mode                     :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk*/
            uint32_t din6_mode                     :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk*/
            uint32_t din7_mode                     :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk*/
            uint32_t dins_mode                     :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk*/
            uint32_t reserved27                    :    5;  /*reserved*/
        };
        uint32_t val;
    } din_mode;
    union {
        struct {
            uint32_t din0_num                      :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din1_num                      :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din2_num                      :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din3_num                      :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din4_num                      :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din5_num                      :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din6_num                      :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din7_num                      :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t dins_num                      :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reserved18                    :    14;  /*reserved*/
        };
        uint32_t val;
    } din_num;
    union {
        struct {
            uint32_t dout0_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t dout1_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t dout2_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t dout3_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t dout4_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the spi_clk*/
            uint32_t dout5_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the spi_clk*/
            uint32_t dout6_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the spi_clk*/
            uint32_t dout7_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the spi_clk*/
            uint32_t douts_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the spi_clk*/
            uint32_t reserved9                     :    23;  /*reserved*/
        };
        uint32_t val;
    } dout_mode;
    union {
        struct {
            uint32_t reg_smem_timing_clk_ena       :    1;  /*For sram, the bit is used to enable timing adjust clock for all reading operations.*/
            uint32_t reg_smem_timing_cali          :    1;  /*For sram, the bit is used to enable timing auto-calibration for all reading operations.*/
            uint32_t reg_smem_extra_dummy_cyclelen :    3;  /*For sram, add extra dummy spi clock cycle length for spi clock calibration.*/
            uint32_t reg_smem_dll_timing_cali      :    1;  /*Set this bit to enable DLL for timing calibration in DDR mode when accessed to EXT_RAM.*/
            uint32_t reserved6                     :    26;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_timing_cali;
    union {
        struct {
            uint32_t reg_smem_din0_mode            :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reg_smem_din1_mode            :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reg_smem_din2_mode            :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reg_smem_din3_mode            :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reg_smem_din4_mode            :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reg_smem_din5_mode            :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reg_smem_din6_mode            :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reg_smem_din7_mode            :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reg_smem_dins_mode            :    3;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reserved27                    :    5;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_din_mode;
    union {
        struct {
            uint32_t reg_smem_din0_num             :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reg_smem_din1_num             :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reg_smem_din2_num             :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reg_smem_din3_num             :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reg_smem_din4_num             :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reg_smem_din5_num             :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reg_smem_din6_num             :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reg_smem_din7_num             :    2;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reg_smem_dins_num             :    2;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reserved18                    :    14;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_din_num;
    union {
        struct {
            uint32_t reg_smem_dout0_mode           :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reg_smem_dout1_mode           :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reg_smem_dout2_mode           :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reg_smem_dout3_mode           :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reg_smem_dout4_mode           :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reg_smem_dout5_mode           :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reg_smem_dout6_mode           :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reg_smem_dout7_mode           :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reg_smem_douts_mode           :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reserved9                     :    23;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_dout_mode;
    union {
        struct {
            uint32_t reg_smem_cs_setup             :    1;  /*For SPI0 and SPI1, spi cs is enable when spi is in prepare phase. 1: enable 0: disable. */
            uint32_t reg_smem_cs_hold              :    1;  /*For SPI0 and SPI1, spi cs keep low when spi is in done phase. 1: enable 0: disable. */
            uint32_t reg_smem_cs_setup_time        :    5;  /*For spi0, (cycles-1) of prepare phase by spi clock this bits are combined with spi_mem_cs_setup bit.*/
            uint32_t reg_smem_cs_hold_time         :    5;  /*For SPI0 and SPI1, spi cs signal is delayed to inactive by spi clock this bits are combined with spi_mem_cs_hold bit.*/
            uint32_t reg_smem_ecc_cs_hold_time     :    3;  /*SPI_SMEM_CS_HOLD_TIME + SPI_SMEM_ECC_CS_HOLD_TIME is the SPI0 and SPI1 CS hold cycles in ECC mode when accessed external RAM.*/
            uint32_t reg_smem_ecc_skip_page_corner :    1;  /*1: SPI0 skips page corner when accesses external RAM. 0: Not skip page corner when accesses external RAM.*/
            uint32_t reg_smem_ecc_16to18_byte_en   :    1;  /*Set this bit to enable SPI0 and SPI1 ECC 16 bytes data with 2 ECC bytes mode when accesses external RAM.*/
            uint32_t reserved17                    :    8;  /*reserved*/
            uint32_t reg_smem_cs_hold_delay        :    6;  /*These bits are used to set the minimum CS high time tSHSL between SPI burst transfer when accesses to external RAM. tSHSL is (SPI_SMEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.*/
            uint32_t reg_smem_split_trans_en       :    1;  /*Set this bit to enable SPI0 split one AXI accesses EXT_RAM transfer into two SPI transfers when one transfer will cross flash/EXT_RAM page corner, valid no matter whether there is an ECC region or not.*/
        };
        uint32_t val;
    } spi_smem_ac;
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
    union {
        struct {
            uint32_t reg_clk_en                    :    1;  /*Register clock gate enable signal. 1: Enable. 0: Disable.*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } clock_gate;
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
    uint32_t xts_plain_base;
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
    union {
        struct {
            uint32_t reg_xts_linesize              :    2;  /*This bits stores the line-size parameter which will be used in manual encryption calculation. It decides how many bytes will be encrypted one time. 0: 16-bytes, 1: 32-bytes, 2: 64-bytes, 3:reserved.*/
            uint32_t reserved2                     :    30;  /*reserved*/
        };
        uint32_t val;
    } xts_linesize;
    union {
        struct {
            uint32_t reg_xts_destination           :    1;  /*This bit stores the destination parameter which will be used in manual encryption calculation. 0: flash(default), 1: psram(reserved). Only default value can be used.*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } xts_destination;
    union {
        struct {
            uint32_t reg_xts_physical_address      :    26;  /*This bits stores the physical-address parameter which will be used in manual encryption calculation. This value should aligned with byte number decided by line-size parameter.*/
            uint32_t reserved26                    :    6;  /*reserved*/
        };
        uint32_t val;
    } xts_physical_address;
    union {
        struct {
            uint32_t reg_xts_trigger               :    1;  /*Set this bit to trigger the process of manual encryption calculation. This action should only be asserted when manual encryption status is 0. After this action, manual encryption status becomes 1. After calculation is done, manual encryption status becomes 2.*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } xts_trigger;
    union {
        struct {
            uint32_t reg_xts_release               :    1;  /*Set this bit to release encrypted result to mspi. This action should only be asserted when manual encryption status is 2. After this action, manual encryption status will become 3.*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } xts_release;
    union {
        struct {
            uint32_t reg_xts_destroy               :    1;  /*Set this bit to destroy encrypted result. This action should be asserted only when manual encryption status is 3. After this action, manual encryption status will become 0.*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } xts_destroy;
    union {
        struct {
            uint32_t reg_xts_state                 :    2;  /*This bits stores the status of manual encryption. 0: idle, 1: busy of encryption calculation, 2: encryption calculation is done but the encrypted result is invisible to mspi, 3: the encrypted result is visible to mspi.*/
            uint32_t reserved2                     :    30;  /*reserved*/
        };
        uint32_t val;
    } xts_state;
    union {
        struct {
            uint32_t reg_xts_date                  :    30;  /*This bits stores the last modified-time of manual encryption feature.*/
            uint32_t reserved30                    :    2;  /*reserved*/
        };
        uint32_t val;
    } xts_date;
    uint32_t reserved_360;
    uint32_t reserved_364;
    uint32_t reserved_368;
    uint32_t reserved_36c;
    uint32_t reserved_370;
    uint32_t reserved_374;
    uint32_t reserved_378;
    uint32_t mmu_item_content;
    uint32_t mmu_item_index;
    union {
        struct {
            uint32_t reg_mmu_mem_force_on          :    1;  /*Set this bit to enable mmu-memory clock force on*/
            uint32_t reg_mmu_mem_force_pd          :    1;  /*Set this bit to force mmu-memory powerdown*/
            uint32_t reg_mmu_mem_force_pu          :    1;  /*Set this bit to force mmu-memory powerup, in this case, the power should also be controlled by rtc.*/
            uint32_t reg_mmu_page_size             :    2;  /*0: Max page size , 1: Max page size/2 , 2: Max page size/4, 3: Max page size/8*/
            uint32_t reserved5                     :    11;  /*reserved*/
            uint32_t aux_ctrl                      :    14;  /*MMU PSRAM aux control register*/
            uint32_t rdn_ena                       :    1;  /*ECO register enable bit*/
            uint32_t rdn_result                    :    1;  /*MSPI module clock domain and AXI clock domain ECO register result register*/
        };
        uint32_t val;
    } mmu_power_ctrl;
    union {
        struct {
            uint32_t reg_crypt_security_level      :    3;  /*Set the security level of spi mem cryption. 0: Shut off cryption DPA funtion. 1-7: The bigger the number is, the more secure the cryption is. (Note that the performance of cryption will decrease together with this number increasing)*/
            uint32_t reg_crypt_calc_d_dpa_en       :    1;  /*Only available when SPI_CRYPT_SECURITY_LEVEL is not 0. 1: Enable DPA in the calculation that using key 1 or key 2. 0: Enable DPA only in the calculation that using key 1.*/
            uint32_t reg_crypt_dpa_selectister     :    1;  /*1: MSPI XTS DPA clock gate is controlled by SPI_CRYPT_CALC_D_DPA_EN and SPI_CRYPT_SECURITY_LEVEL. 0: Controlled by efuse bits.*/
            uint32_t reserved5                     :    27;  /*reserved*/
        };
        uint32_t val;
    } dpa_ctrl;
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
    uint32_t spi_memisterrnd_eco_high;
    uint32_t spi_memisterrnd_eco_low;
    uint32_t reserved_3f8;
    union {
        struct {
            uint32_t date                          :    28;  /*SPI0/1 register version.*/
            uint32_t reserved28                    :    4;  /*reserved*/
        };
        uint32_t val;
    } date;
} spi_mem_dev_t;

extern spi_mem_dev_t SPIMEM0;
extern spi_mem_dev_t SPIMEM1;

#ifndef __cplusplus
_Static_assert(sizeof(spi_mem_dev_t) == 0x400, "Invalid size of spi_mem_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
