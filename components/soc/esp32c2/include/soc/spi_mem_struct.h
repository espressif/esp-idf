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

typedef volatile struct spi_mem_dev_s{
    union {
        struct {
            uint32_t mst_st                        :    4;  /*The current status of SPI1 master FSM.*/
            uint32_t slv_st                        :    4;  /*The current status of SPI1 slave FSM: mspi_st. 0: idle state, 1: preparation state, 2: send command state, 3: send address state, 4: wait state, 5: read data state, 6:write data state, 7: done state, 8: read data end state.*/
            uint32_t reserved8                     :    9;  /*reserved*/
            uint32_t flash_pe                      :    1;  /*In user mode, it is set to indicate that program/erase operation will be triggered. The bit is combined with spi_mem_usr bit. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t usr                           :    1;  /*User define command enable.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
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
            uint32_t reserved0                     :    3;  /*reserved*/
            uint32_t fdummy_out                    :    1;  /*In the dummy phase the signal level of spi is output by the spi controller.*/
            uint32_t reserved4                     :    3;  /*reserved*/
            uint32_t fcmd_dual                     :    1;  /*Apply 2 signals during command phase 1:enable 0: disable*/
            uint32_t fcmd_quad                     :    1;  /*Apply 4 signals during command phase 1:enable 0: disable*/
            uint32_t reserved9                     :    1;  /*reserved*/
            uint32_t fcs_crc_en                    :    1;  /*For SPI1,  initialize crc32 module before writing encrypted data to flash. Active low.*/
            uint32_t tx_crc_en                     :    1;  /*For SPI1,  enable crc32 when writing encrypted data to flash. 1: enable 0:disable*/
            uint32_t reserved12                    :    1;  /*reserved*/
            uint32_t fastrd_mode                   :    1;  /*This bit enable the bits: SPI_MEM_FREAD_QIO, SPI_MEM_FREAD_DIO, SPI_MEM_FREAD_QOUT AND SPI_MEM_FREAD_DOUT. 1: enable 0: disable. */
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
            uint32_t reserved25                    :    7;  /*reserved*/
        };
        uint32_t val;
    } ctrl;
    union {
        struct {
            uint32_t clk_mode                      :    2;  /*SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.*/
            uint32_t cs_hold_dly_res               :    10;  /*After RES/DP/HPM command is sent, SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 512) SPI_CLK cycles.*/
            uint32_t reserved2                     :    18;  /*reserved*/
            uint32_t rxfifo_rst                    :    1;  /*SPI0 RX FIFO reset signal.*/
            uint32_t reserved31                    :    1;  /*reserved*/
        };
        uint32_t val;
    } ctrl1;
    union {
        struct {
            uint32_t cs_setup_time                 :    5;  /*(cycles-1) of prepare phase by spi clock this bits are combined with spi_mem_cs_setup bit.*/
            uint32_t cs_hold_time                  :    5;  /*Spi cs signal is delayed to inactive by spi clock this bits are combined with spi_mem_cs_hold bit.*/
            uint32_t reserved10                    :    15;  /*reserved*/
            uint32_t cs_hold_delay                 :    6;  /*These bits are used to set the minimum CS high time tSHSL between SPI burst transfer when accesses to flash. tSHSL is (SPI_MEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.*/
            uint32_t sync_reset                    :    1;  /*The FSM will be reset.*/
        };
        uint32_t val;
    } ctrl2;
    union {
        struct {
            uint32_t clkcnt_l                      :    8;  /*In the master mode it must be equal to spi_mem_clkcnt_N. */
            uint32_t clkcnt_h                      :    8;  /*In the master mode it must be floor((spi_mem_clkcnt_N+1)/2-1).*/
            uint32_t clkcnt_n                      :    8;  /*In the master mode it is the divider of spi_mem_clk. So spi_mem_clk frequency is system/(spi_mem_clkcnt_N+1)*/
            uint32_t reserved24                    :    7;  /*In the master mode it is pre-divider of spi_mem_clk. */
            uint32_t clk_equ_sysclk                :    1;  /*Set this bit in 1-division mode.*/
        };
        uint32_t val;
    } clock;
    union {
        struct {
            uint32_t reserved0                     :    6;  /*reserved*/
            uint32_t cs_hold                       :    1;  /*spi cs keep low when spi is in  done  phase. 1: enable 0: disable. */
            uint32_t cs_setup                      :    1;  /*spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable. */
            uint32_t reserved8                     :    1;  /*reserved*/
            uint32_t ck_out_edge                   :    1;  /*the bit combined with spi_mem_mosi_delay_mode bits to set mosi signal delay mode. */
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
            uint32_t reserved6                     :    20;  /*reserved*/
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
            uint32_t reserved0                     :    1;  /*reserved*/
            uint32_t mst_st_trans_end              :    1;  /*The bit is used to indicate the  spi0_mst_st controlled transmitting is done.*/
            uint32_t mst_st_trans_end_en           :    1;  /*The bit is used to enable the interrupt of  spi0_mst_st controlled transmitting is done.*/
            uint32_t slv_st_trans_end              :    1;  /*The bit is used to indicate the  spi0_slv_st controlled transmitting is done.*/
            uint32_t slv_st_trans_end_en           :    1;  /*The bit is used to enable the interrupt of spi0_slv_st controlled transmitting is done.*/
            uint32_t reserved7                     :    2;  /*reserved*/
            uint32_t ck_idle_edge                  :    1;  /*1: spi clk line is high when idle     0: spi clk line is low when idle */
            uint32_t cs_keep_active                :    1;  /*spi cs line keep low when the bit is set.*/
            uint32_t reserved11                    :    21;  /*reserved*/
        };
        uint32_t val;
    } misc;
    uint32_t tx_crc;
    union {
        struct {
            uint32_t req_en                        :    1;  /*For SPI0, Cache access enable, 1: enable, 0:disable.*/
            uint32_t usr_addr_4byte                :    1;  /*For SPI0,  cache  read flash with 4 bytes address, 1: enable, 0:disable.*/
            uint32_t flash_usr_cmd                 :    1;  /*For SPI0,  cache  read flash for user define command, 1: enable, 0:disable.*/
            uint32_t fdin_dual                     :    1;  /*For SPI0 flash, din phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_fread_dio.*/
            uint32_t fdout_dual                    :    1;  /*For SPI0 flash, dout phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_fread_dio.*/
            uint32_t faddr_dual                    :    1;  /*For SPI0 flash, address phase apply 2 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_dio.*/
            uint32_t fdin_quad                     :    1;  /*For SPI0 flash, din phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.*/
            uint32_t fdout_quad                    :    1;  /*For SPI0 flash, dout phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.*/
            uint32_t faddr_quad                    :    1;  /*For SPI0 flash, address phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.*/
            uint32_t reserved9                     :    23;  /*reserved*/
        };
        uint32_t val;
    } cache_fctrl;
    uint32_t reserved_40;
    uint32_t reserved_44;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    union {
        struct {
            uint32_t spi0_slv_st                   :    4;  /*The current status of SPI0 slave FSM: spi0_slv_st. 0: idle state, 1: preparation state, 2: send command state, 3: send address state, 4: wait state, 5: read data state, 6:write data state, 7: done state, 8: read data end state.*/
            uint32_t spi0_mst_st                   :    3;  /*The current status of SPI0 master FSM: spi0_mst_st. 0: idle state, 1:EM_CACHE_GRANT , 2: program/erase suspend state, 3: SPI0 read data state, 4: wait cache/EDMA sent data is stored in SPI0 TX FIFO, 5: SPI0 write data state.*/
            uint32_t cspi_lock_delay_time          :    5;  /*The lock delay time of SPI0/1 arbiter by spi0_slv_st, after PER is sent by SPI1.*/
            uint32_t reserved12                    :    20;  /*reserved*/
        };
        uint32_t val;
    } fsm;
    uint32_t data_buf[16];
    union {
        struct {
            uint32_t reserved0                     :    1;  /*reserved*/
            uint32_t waiti_dummy                   :    1;  /*The dummy phase enable when wait flash idle (RDSR)*/
            uint32_t waiti_cmd                     :    8;  /*The command to wait flash idle(RDSR).*/
            uint32_t waiti_dummy_cyclelen          :    6;  /*The dummy cycle length when wait flash idle(RDSR).*/
            uint32_t reserved16                    :    16;  /*reserved*/
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
            uint32_t flash_per_command             :    8;  /*Program/Erase resume command.*/
            uint32_t flash_pes_command             :    8;  /*Program/Erase suspend command.*/
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
            uint32_t reserved8                     :    24;  /*reserved*/
        };
        uint32_t val;
    } sus_status;
    union {
        struct {
            uint32_t timing_clk_ena                :    1;  /*The bit is used to enable timing adjust clock for all reading operations.*/
            uint32_t timing_cali                   :    1;  /*The bit is used to enable timing auto-calibration for all reading operations.*/
            uint32_t extra_dummy_cyclelen          :    3;  /*add extra dummy spi clock cycle length for spi clock calibration.*/
            uint32_t reserved5                     :    27;  /*reserved*/
        };
        uint32_t val;
    } timing_cali;
    union {
        struct {
            uint32_t din0_mode                     :    2;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t din1_mode                     :    2;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t din2_mode                     :    2;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t din3_mode                     :    2;  /*the input signals are delayed by system clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb,  3: input with the posedge of clk_160, 4 input with the negedge of clk_160, 5: input with the spi_clk high edge,  6: input with the spi_clk low edge*/
            uint32_t reserved8                     :    24;  /*reserved*/
        };
        uint32_t val;
    } din_mode;
    union {
        struct {
            uint32_t din0_num                      :    1;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din1_num                      :    1;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din2_num                      :    1;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t din3_num                      :    1;  /*the input signals are delayed by system clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...*/
            uint32_t reserved4                     :    28;  /*reserved*/
        };
        uint32_t val;
    } din_num;
    union {
        struct {
            uint32_t dout0_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t dout1_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t dout2_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t dout3_mode                    :    1;  /*the output signals are delayed by system clock cycles, 0: output without delayed, 1: output with the posedge of clk_apb,2 output with the negedge of clk_apb, 3: output with the posedge of clk_160,4 output with the negedge of clk_160,5: output with the spi_clk high edge ,6: output with the spi_clk low edge*/
            uint32_t reserved4                     :    28;  /*reserved*/
        };
        uint32_t val;
    } dout_mode;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    union {
        struct {
            uint32_t per_end_en                    :    1;  /*The enable bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end_en                    :    1;  /*The enable bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t wpe_end_en                    :    1;  /*The enable bit for SPI_MEM_WPE_END_INT interrupt.*/
            uint32_t slv_st_end_en                 :    1;  /*The enable bit for SPI_MEM_SLV_ST_END_INT interrupt.*/
            uint32_t mst_st_end_en                 :    1;  /*The enable bit for SPI_MEM_MST_ST_END_INT interrupt.*/
            uint32_t brown_out_en                  :    1;  /*The enable bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t reserved6                     :    26;  /*reserved*/
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
            uint32_t brown_out                     :    1;  /*The status bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t reserved6                     :    26;  /*reserved*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t per_end                       :    1;  /*The raw bit for SPI_MEM_PER_END_INT interrupt. 1: Triggered when Auto Resume command (0x7A) is sent and flash is resumed. 0: Others.*/
            uint32_t pes_end                       :    1;  /*The raw bit for SPI_MEM_PES_END_INT interrupt.1: Triggered when Auto Suspend command (0x75) is sent and flash is suspended. 0: Others.*/
            uint32_t wpe_end                       :    1;  /*The raw bit for SPI_MEM_WPE_END_INT interrupt. 1: Triggered when WRSR/PP/SE/BE/CE is sent and flash is already idle. 0: Others.*/
            uint32_t slv_st_end                    :    1;  /*The raw bit for SPI_MEM_SLV_ST_END_INT interrupt. 1: Triggered when spi1_slv_st is changed from non idle state to idle state. It means that SPI_CS raises high. 0: Others*/
            uint32_t mst_st_end                    :    1;  /*The raw bit for SPI_MEM_MST_ST_END_INT interrupt. 1: Triggered when spi1_mst_st is changed from non idle state to idle state. 0: Others.*/
            uint32_t brown_out                     :    1;  /*The raw bit for SPI_MEM_BROWN_OUT_INT interrupt. 1: Triggered condition is that chip is loosing power and RTC module sends out brown out close flash request to SPI1. After SPI1 sends out suspend command to flash, this interrupt is triggered and MSPI returns to idle state. 0: Others.*/
            uint32_t reserved6                     :    26;  /*reserved*/
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
            uint32_t brown_out                     :    1;  /*The status bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t reserved6                     :    26;  /*reserved*/
        };
        uint32_t val;
    } int_st;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    union {
        struct {
            uint32_t clk_en                        :    1;  /*Register clock gate enable signal. 1: Enable. 0: Disable.*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } clock_gate;
    union {
        struct {
            uint32_t spi01_clk_sel                 :    2;  /*When the digital system clock selects PLL clock and the frequency of PLL clock is 480MHz, the value of reg_spi01_clk_sel:  0: SPI0/1 module clock (clk) is 80MHz. 1: SPI0/1 module clock (clk) is 120MHz.  2: SPI0/1 module clock (clk) 160MHz. 3: Not used. When the digital system clock selects PLL clock and the frequency of PLL clock is 320MHz, the value of reg_spi01_clk_sel:  0: SPI0/1 module clock (clk) is 80MHz. 1: SPI0/1 module clock (clk) is 80MHz.  2: SPI0/1 module clock (clk) 160MHz. 3: Not used. */
            uint32_t reserved2                     :    30;  /*reserved*/
        };
        uint32_t val;
    } core_clk_sel;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t reserved_fc;
    uint32_t reserved_100;
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
            uint32_t date                          :    28;  /*SPI register version.*/
            uint32_t reserved28                    :    4;  /*reserved*/
        };
        uint32_t val;
    } date;
} spi_mem_dev_t;
extern spi_mem_dev_t SPIMEM0;
extern spi_mem_dev_t SPIMEM1;

#ifndef __cplusplus
_Static_assert(sizeof(spi_mem_dev_t) == 0x400, "spi_mem_dev_t size error!");
#endif

#ifdef __cplusplus
}
#endif
