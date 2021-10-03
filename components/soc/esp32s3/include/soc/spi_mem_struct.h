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
#ifndef _SOC_SPI_MEM_STRUCT_H_
#define _SOC_SPI_MEM_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct spi_mem_dev_s {
    union {
        struct {
            uint32_t reserved0                     :    17;  /*reserved*/
            uint32_t flash_pe                      :    1;  /*In user mode, it is set to indicate that program/erase operation will be triggered. The bit is combined with SPI_MEM_USR bit. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t usr                           :    1;  /*User define command enable.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_hpm                     :    1;  /*Drive Flash into high performance mode.  The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_res                     :    1;  /*This bit combined with SPI_MEM_RESANDRES bit releases Flash from the power-down state or high performance mode and obtains the devices ID. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_dp                      :    1;  /*Drive Flash into power down.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_ce                      :    1;  /*Chip erase enable. Chip erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_be                      :    1;  /*Block erase enable(32KB) .  Block erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_se                      :    1;  /*Sector erase enable(4KB). Sector erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_pp                      :    1;  /*Page program enable(1 byte ~64 bytes data to be programmed). Page program operation  will be triggered when the bit is set. The bit will be cleared once the operation done .1: enable 0: disable. */
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
            uint32_t fdummy_out                    :    1;  /*In the DUMMY phase the signal level of SPI bus is output by the SPI0 controller.*/
            uint32_t fdout_oct                     :    1;  /*Set this bit to enable 8-bit-mode(8-bm) in DOUT phase.*/
            uint32_t fdin_oct                      :    1;  /*Set this bit to enable 8-bit-mode(8-bm) in DIN phase.*/
            uint32_t faddr_oct                     :    1;  /*Set this bit to enable 8-bit-mode(8-bm) in ADDR phase.*/
            uint32_t fcmd_dual                     :    1;  /*Set this bit to enable 2-bit-mode(2-bm) in CMD phase.*/
            uint32_t fcmd_quad                     :    1;  /*Set this bit to enable 4-bit-mode(4-bm) in CMD phase.*/
            uint32_t fcmd_oct                      :    1;  /*Set this bit to enable 8-bit-mode(8-bm) in CMD phase.*/
            uint32_t fcs_crc_en                    :    1;  /*For SPI1,  initialize crc32 module before writing encrypted data to flash. Active low.*/
            uint32_t tx_crc_en                     :    1;  /*For SPI1,  enable crc32 when writing encrypted data to flash. 1: enable 0:disable*/
            uint32_t reserved12                    :    1;  /*reserved*/
            uint32_t fastrd_mode                   :    1;  /*This bit should be set when SPI_MEM_FREAD_QIO, SPI_MEM_FREAD_DIO, SPI_MEM_FREAD_QUAD or SPI_MEM_FREAD_DUAL is set.*/
            uint32_t fread_dual                    :    1;  /*In hardware 0x3B read operation, DIN phase apply 2 signals. 1: enable 0: disable. */
            uint32_t resandres                     :    1;  /*The Device ID is read out to SPI_MEM_RD_STATUS register,  this bit combine with spi_mem_flash_res bit. 1: enable 0: disable. */
            uint32_t reserved16                    :    2;  /*reserved*/
            uint32_t q_pol                         :    1;  /*The bit is used to set MISO line polarity, 1: high 0, low*/
            uint32_t d_pol                         :    1;  /*The bit is used to set MOSI line polarity, 1: high 0, low*/
            uint32_t fread_quad                    :    1;  /*In hardware 0x6B read operation, DIN phase apply 4 signals(4-bit-mode). 1: enable 0: disable. */
            uint32_t wp                            :    1;  /*Write protect signal output when SPI is idle.  1: output high, 0: output low. */
            uint32_t wrsr_2b                       :    1;  /*Two bytes data will be written to status register when it is set. 1: enable 0: disable. */
            uint32_t fread_dio                     :    1;  /*In hardware 0xBB read operation, ADDR phase and DIN phase apply 2 signals(2-bit-mode). 1: enable 0: disable. */
            uint32_t fread_qio                     :    1;  /*In hardware 0xEB read operation, ADDR phase and DIN phase apply 4 signals(4-bit-mode). 1: enable 0: disable. */
            uint32_t reserved25                    :    7;  /*reserved*/
        };
        uint32_t val;
    } ctrl;
    union {
        struct {
            uint32_t clk_mode                      :    2;  /*SPI Bus clock (SPI_CLK) mode bits. 0: SPI Bus clock (SPI_CLK) is off when CS inactive 1: SPI_CLK is delayed one cycle after SPI_CS inactive 2: SPI_CLK is delayed two cycles after SPI_CS inactive 3: SPI_CLK is always on.*/
            uint32_t cs_hold_dly_res               :    10;  /*After RES/DP/HPM/PES/PER command is sent, SPI1 may waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4 or * 256) SPI_CLK cycles.*/
            uint32_t reserved2                     :    18;  /*reserved*/
            uint32_t rxfifo_rst                    :    1;  /*SPI0 RX FIFO reset signal. Set this bit and clear it before SPI0 transfer starts.*/
            uint32_t reserved31                    :    1;  /*reserved*/
        };
        uint32_t val;
    } ctrl1;
    union {
        struct {
            uint32_t cs_setup_time                 :    5;  /*(cycles-1) of PREP phase by SPI_CLK, which is the SPI_CS setup time. These bits are combined with SPI_MEM_CS_SETUP bit.*/
            uint32_t cs_hold_time                  :    5;  /*SPI Bus CS (SPI_CS) signal is delayed to inactive by SPI Bus clock (SPI_CLK), which is the SPI_CS hold time in non-ECC mode. These bits are combined with SPI_MEM_CS_HOLD bit.*/
            uint32_t ecc_cs_hold_time              :    3;  /*SPI_MEM_CS_HOLD_TIME + SPI_MEM_ECC_CS_HOLD_TIME is the SPI_CS hold cycle in ECC mode when accessed flash.*/
            uint32_t ecc_skip_page_corner          :    1;  /*1: MSPI skips page corner when accesses flash. 0: Not skip page corner when accesses flash.*/
            uint32_t ecc_16to18_byte_en            :    1;  /*Set this bit to enable MSPI ECC 16 bytes data with 2 ECC bytes mode when accesses flash.*/
            uint32_t reserved15                    :    10;  /*reserved*/
            uint32_t cs_hold_delay                 :    6;  /*These bits are used to set the minimum CS high time tSHSL between SPI burst transfer when accesses to flash. tSHSL is (SPI_MEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.*/
            uint32_t sync_reset                    :    1;  /*The FSM will be reset.*/
        };
        uint32_t val;
    } ctrl2;
    union {
        struct {
            uint32_t clkcnt_l                      :    8;  /*It must equal to the value of SPI_MEM_CLKCNT_N. */
            uint32_t clkcnt_h                      :    8;  /*It must be a floor value of ((SPI_MEM_CLKCNT_N+1)/2-1).*/
            uint32_t clkcnt_n                      :    8;  /*When SPI0 accesses flash, f_SPI_CLK = f_MSPI_CORE_CLK/(SPI_MEM_CLKCNT_N+1)*/
            uint32_t reserved24                    :    7;  /*reserved*/
            uint32_t clk_equ_sysclk                :    1;  /*When SPI0 accesses flash, set this bit in 1-division mode, f_SPI_CLK = f_MSPI_CORE_CLK.*/
        };
        uint32_t val;
    } clock;
    union {
        struct {
            uint32_t reserved0                     :    6;  /*reserved*/
            uint32_t cs_hold                       :    1;  /*Set this bit to keep SPI_CS low when MSPI is in DONE state.*/
            uint32_t cs_setup                      :    1;  /*Set this bit to keep SPI_CS low when MSPI is in PREP state.*/
            uint32_t reserved8                     :    1;  /*reserved*/
            uint32_t ck_out_edge                   :    1;  /*This bit, combined with SPI_MEM_CK_IDLE_EDGE bit, is used to change the clock mode 0~3 of SPI_CLK. */
            uint32_t reserved10                    :    2;  /*reserved*/
            uint32_t fwrite_dual                   :    1;  /*Set this bit to enable 2-bm in DOUT phase in SPI1 write operation.*/
            uint32_t fwrite_quad                   :    1;  /*Set this bit to enable 4-bm in DOUT phase in SPI1 write operation.*/
            uint32_t fwrite_dio                    :    1;  /*Set this bit to enable 2-bm in ADDR and DOUT phase in SPI1 write operation.*/
            uint32_t fwrite_qio                    :    1;  /*Set this bit to enable 4-bit-mode(4-bm) in ADDR and DOUT phase in SPI1 write operation.*/
            uint32_t reserved16                    :    8;  /*reserved*/
            uint32_t usr_miso_highpart             :    1;  /*DIN phase only access to high-part of the buffer SPI_MEM_W8_REG~SPI_MEM_W15_REG. 1: enable 0: disable. */
            uint32_t usr_mosi_highpart             :    1;  /*DOUT phase only access to high-part of the buffer SPI_MEM_W8_REG~SPI_MEM_W15_REG. 1: enable 0: disable. */
            uint32_t usr_dummy_idle                :    1;  /*SPI_CLK is disabled(No clock edges) in DUMMY phase when the bit is enable.*/
            uint32_t usr_mosi                      :    1;  /*Set this bit to enable the DOUT phase of an write-data operation.*/
            uint32_t usr_miso                      :    1;  /*Set this bit to enable enable the DIN phase of a read-data operation.*/
            uint32_t usr_dummy                     :    1;  /*Set this bit to enable enable the DUMMY phase of an operation.*/
            uint32_t usr_addr                      :    1;  /*Set this bit to enable enable the ADDR phase of an operation.*/
            uint32_t usr_command                   :    1;  /*Set this bit to enable enable the CMD phase of an operation.*/
        };
        uint32_t val;
    } user;
    union {
        struct {
            uint32_t usr_dummy_cyclelen            :    6;  /*The SPI_CLK cycle length minus 1 of DUMMY phase.*/
            uint32_t reserved6                     :    20;  /*reserved*/
            uint32_t usr_addr_bitlen               :    6;  /*The length in bits of ADDR phase. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } user1;
    union {
        struct {
            uint32_t usr_command_value             :    16;  /*The value of user defined(USR) command.*/
            uint32_t reserved16                    :    12;  /*reserved*/
            uint32_t usr_command_bitlen            :    4;  /*The length in bits of CMD phase. The register value shall be (bit_num-1)*/
        };
        uint32_t val;
    } user2;
    union {
        struct {
            uint32_t usr_mosi_bit_len              :    10;  /*The length in bits of DOUT phase. The register value shall be (bit_num-1).*/
            uint32_t reserved10                    :    22;  /*reserved*/
        };
        uint32_t val;
    } mosi_dlen;
    union {
        struct {
            uint32_t usr_miso_bit_len              :    10;  /*The length in bits of DIN phase. The register value shall be (bit_num-1).*/
            uint32_t reserved10                    :    22;  /*reserved*/
        };
        uint32_t val;
    } miso_dlen;
    union {
        struct {
            uint32_t status                        :    16;  /*The value is stored when set SPI_MEM_FLASH_RDSR bit and SPI_MEM_FLASH_RES bit.*/
            uint32_t wb_mode                       :    8;  /*Mode bits in the flash fast read mode  it is combined with SPI_MEM_FASTRD_MODE bit.*/
            uint32_t reserved24                    :    8;  /*reserved*/
        };
        uint32_t val;
    } rd_status;
    uint32_t ext_addr;
    union {
        struct {
            uint32_t cs0_dis                       :    1;  /*Set this bit to raise high SPI_CS pin, which means that the SPI device(flash) connected to SPI_CS is in low level when SPI1 transfer starts.*/
            uint32_t cs1_dis                       :    1;  /*Set this bit to raise high SPI_CS1 pin, which means that the SPI device(Ext_RAM) connected to SPI_CS1 is in low level when SPI1 transfer starts.*/
            uint32_t reserved0                     :    5;  /*reserved*/
            uint32_t fsub_pin                      :    1;  /*Flash is connected to SPI SUBPIN bus.*/
            uint32_t ssub_pin                      :    1;  /*Ext_RAM is connected to SPI SUBPIN bus.*/
            uint32_t ck_idle_edge                  :    1;  /*1: SPI_CLK line is high when idle. 0: SPI_CLK line is low when idle */
            uint32_t cs_keep_active                :    1;  /*SPI_CS line keep low when the bit is set.*/
            uint32_t auto_per                      :    1;  /*Set this bit to enable auto PER function. Hardware will sent out PER command if PES command is sent.*/
            uint32_t reserved12                    :    20;  /*reserved*/
        };
        uint32_t val;
    } misc;
    uint32_t tx_crc;
    union {
        struct {
            uint32_t req_en                        :    1;  /*Set this bit to enable Cache's access and SPI0's transfer.*/
            uint32_t usr_cmd_4byte                 :    1;  /*Set this bit to enable SPI0 read flash with 32 bits address. The value of SPI_MEM_USR_ADDR_BITLEN should be 31.*/
            uint32_t flash_usr_cmd                 :    1;  /*1: The command value of SPI0 reads flash is SPI_MEM_USR_COMMAND_VALUE. 0: Hardware read command value, controlled by SPI_MEM_FREAD_QIO, SPI_MEM_FREAD_DIO, SPI_MEM_FREAD_QUAD, SPI_MEM_FREAD_DUAL and SPI_MEM_FASTRD_MODE bits.*/
            uint32_t fdin_dual                     :    1;  /*When SPI0 accesses to flash, set this bit to enable 2-bm in DIN phase.*/
            uint32_t fdout_dual                    :    1;  /*When SPI0 accesses to flash, set this bit to enable 2-bm in DOUT phase.*/
            uint32_t faddr_dual                    :    1;  /*When SPI0 accesses to flash, set this bit to enable 2-bm in ADDR phase.*/
            uint32_t fdin_quad                     :    1;  /*When SPI0 accesses to flash, set this bit to enable 4-bm in DIN phase.*/
            uint32_t fdout_quad                    :    1;  /*When SPI0 accesses to flash, set this bit to enable 4-bm in DOUT phase.*/
            uint32_t faddr_quad                    :    1;  /*When SPI0 accesses to flash, set this bit to enable 4-bm in ADDR phase.*/
            uint32_t reserved9                     :    23;  /*reserved*/
        };
        uint32_t val;
    } cache_fctrl;
    union {
        struct {
            uint32_t usr_scmd_4byte                :    1;  /*Set this bit to enable SPI0 read Ext_RAM with 32 bits address. The value of SPI_MEM_SRAM_ADDR_BITLEN should be 31.*/
            uint32_t usr_sram_dio                  :    1;  /*Set the bit to enable 2-bm in all the phases of SPI0 Ext_RAM transfer.*/
            uint32_t usr_sram_qio                  :    1;  /*Set the bit to enable QPI mode in all SPI0 Ext_RAM transfer.*/
            uint32_t usr_wr_sram_dummy             :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable DUMMY phase in write operations.*/
            uint32_t usr_rd_sram_dummy             :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable DUMMY phase in read operations.*/
            uint32_t usr_rcmd                      :    1;  /*1: The command value of SPI0 read Ext_RAM is SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE. 0: The value is 0x2.*/
            uint32_t sram_rdummy_cyclelen          :    6;  /*When SPI0 accesses to Ext_RAM, it is the SPI_CLK cycles minus 1 of DUMMY phase in read data transfer.*/
            uint32_t reserved12                    :    2;  /*reserved*/
            uint32_t sram_addr_bitlen              :    6;  /*When SPI0 accesses to Ext_RAM, it is the length in bits of ADDR phase. The register value shall be (bit_num-1).*/
            uint32_t usr_wcmd                      :    1;  /*1: The command value of SPI0 write Ext_RAM is SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE. 0: The value is 0x3.*/
            uint32_t sram_oct                      :    1;  /*Set the bit to enable OPI mode in all SPI0 Ext_RAM transfer.*/
            uint32_t sram_wdummy_cyclelen          :    6;  /*When SPI0 accesses to Ext_RAM, it is the SPI_CLK cycles minus 1 of DUMMY phase in write data transfer.*/
            uint32_t reserved28                    :    4;  /*reserved*/
        };
        uint32_t val;
    } cache_sctrl;
    union {
        struct {
            uint32_t sclk_mode                     :    2;  /*SPI_CLK mode bits  when SPI0 accesses to Ext_RAM. 0: SPI_CLK is off when CS inactive 1: SPI_CLK is delayed one cycle after CS inactive 2: SPI_CLK is delayed two cycles after CS inactive 3: SPI_CLK is always on.*/
            uint32_t swb_mode                      :    8;  /*Mode bits when SPI0 accesses to Ext_RAM.*/
            uint32_t sdin_dual                     :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 2-bm in DIN phase.*/
            uint32_t sdout_dual                    :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 2-bm in DOUT phase.*/
            uint32_t saddr_dual                    :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 2-bm in ADDR phase.*/
            uint32_t scmd_dual                     :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 2-bm in CMD phase.*/
            uint32_t sdin_quad                     :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 4-bm in DIN phase.*/
            uint32_t sdout_quad                    :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 4-bm in DOUT phase.*/
            uint32_t saddr_quad                    :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 4-bm in ADDR phase.*/
            uint32_t scmd_quad                     :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 4-bm in CMD phase.*/
            uint32_t sdin_oct                      :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 8-bm in DIN phase.*/
            uint32_t sdout_oct                     :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 8-bm in DOUT phase.*/
            uint32_t saddr_oct                     :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 8-bm in ADDR phase.*/
            uint32_t scmd_oct                      :    1;  /*When SPI0 accesses to Ext_RAM, set this bit to enable 8-bm in CMD phase.*/
            uint32_t sdummy_out                    :    1;  /*When SPI0 accesses to Ext_RAM, in the DUMMY phase the signal level of SPI bus is output by the SPI0 controller.*/
            uint32_t reserved23                    :    9;  /*reserved*/
        };
        uint32_t val;
    } sram_cmd;
    union {
        struct {
            uint32_t usr_rd_cmd_value              :    16;  /*When SPI0 reads Ext_RAM, it is the command value of CMD phase.*/
            uint32_t reserved16                    :    12;  /*reserved*/
            uint32_t usr_rd_cmd_bitlen             :    4;  /*When SPI0 reads Ext_RAM, it is the length in bits of CMD phase. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } sram_drd_cmd;
    union {
        struct {
            uint32_t usr_wr_cmd_value              :    16;  /*When SPI0 writes Ext_RAM, it is the command value of CMD phase.*/
            uint32_t reserved16                    :    12;  /*reserved*/
            uint32_t usr_wr_cmd_bitlen             :    4;  /*When SPI0 writes Ext_RAM, it is the length in bits of CMD phase. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } sram_dwr_cmd;
    union {
        struct {
            uint32_t cnt_l                         :    8;  /*It must equal to the value of SPI_MEM_SCLKCNT_N. */
            uint32_t cnt_h                         :    8;  /*It must be a floor value of ((SPI_MEM_SCLKCNT_N+1)/2-1).*/
            uint32_t cnt_n                         :    8;  /*When SPI0 accesses to Ext_RAM, f_SPI_CLK = f_MSPI_CORE_CLK/(SPI_MEM_SCLKCNT_N+1)*/
            uint32_t reserved24                    :    7;  /*reserved*/
            uint32_t equ_sysclk                    :    1;  /*When SPI0 accesses to Ext_RAM, set this bit in 1-division mode, f_SPI_CLK = f_MSPI_CORE_CLK.*/
        };
        uint32_t val;
    } sram_clk;
    union {
        struct {
            uint32_t st                            :    3;  /*The status of SPI0 state machine. 0: idle state(IDLE), 1: preparation state(PREP), 2: send command state(CMD), 3: send address state(ADDR), 4: red data state(DIN), 5:write data state(DOUT), 6: wait state(DUMMY), 7: done state(DONE).*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } fsm;
    uint32_t data_buf[16];                                                /*data buffer*/
    union {
        struct {
            uint32_t waiti_en                      :    1;  /*Set this bit to enable auto-waiting flash idle operation when PP/SE/BE/CE/WRSR/PES command is sent.*/
            uint32_t waiti_dummy                   :    1;  /*Set this bit to enable DUMMY phase in auto wait flash idle transfer(RDSR).*/
            uint32_t waiti_cmd                     :    8;  /*The command value of auto wait flash idle transfer(RDSR).*/
            uint32_t waiti_dummy_cyclelen          :    6;  /*The dummy cycle length when wait flash idle(RDSR).*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } flash_waiti_ctrl;
    union {
        struct {
            uint32_t flash_per                     :    1;  /*program erase resume bit, program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_pes                     :    1;  /*program erase suspend bit, program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. */
            uint32_t flash_per_wait_en             :    1;  /*Set this bit to add delay time after program erase resume(PER) is sent.*/
            uint32_t flash_pes_wait_en             :    1;  /*Set this bit to add delay time after program erase suspend(PES) command is sent.*/
            uint32_t pes_per_en                    :    1;  /*Set this bit to enable PES transfer trigger PES transfer option.*/
            uint32_t pesr_idle_en                  :    1;  /*1: Separate PER flash wait idle and PES flash wait idle. 0: Not separate.*/
            uint32_t reserved6                     :    26;  /*reserved*/
        };
        uint32_t val;
    } flash_sus_cmd;
    union {
        struct {
            uint32_t flash_pes_en                  :    1;  /*Set this bit to enable auto-suspend function.*/
            uint32_t flash_per_command             :    8;  /*Program/Erase resume command value.*/
            uint32_t flash_pes_command             :    8;  /*Program/Erase suspend command value.*/
            uint32_t reserved17                    :    15;  /*reserved*/
        };
        uint32_t val;
    } flash_sus_ctrl;
    union {
        struct {
            uint32_t flash_sus                     :    1;  /*The status of flash suspend. This bit is set when PES command is sent, and cleared when PER is sent. Only used in SPI1.*/
            uint32_t reserved1                     :    1;  /*reserved*/
            uint32_t flash_hpm_dly_256             :    1;  /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after HPM command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after HPM command is sent.*/
            uint32_t flash_res_dly_256             :    1;  /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after RES command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after RES command is sent.*/
            uint32_t flash_dp_dly_256              :    1;  /*1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after DP command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after DP command is sent.*/
            uint32_t flash_per_dly_256             :    1;  /*Valid when SPI_MEM_FLASH_PER_WAIT_EN is 1. 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after PER command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PER command is sent.*/
            uint32_t flash_pes_dly_256             :    1;  /*Valid when SPI_MEM_FLASH_PES_WAIT_EN is 1. 1: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 256) SPI_CLK cycles after PES command is sent. 0: SPI1 waits (SPI_MEM_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PES command is sent.*/
            uint32_t reserved7                     :    25;  /*reserved*/
        };
        uint32_t val;
    } sus_status;
    union {
        struct {
            uint32_t timing_clk_ena                :    1;  /*Set this bit to power on HCLK. When PLL is powered on, the frequency of HCLK equals to that of PLL. Otherwise, the frequency equals to that of XTAL.*/
            uint32_t timing_cali                   :    1;  /*Set this bit to add extra SPI_CLK cycles in DUMMY phase for all reading operations.*/
            uint32_t extra_dummy_cyclelen          :    3;  /*Extra SPI_CLK cycles added in DUMMY phase for timing compensation, when SPI0 accesses to flash. Active when SPI_MEM_TIMING_CALI bit is set.*/
            uint32_t reserved5                     :    27;  /*reserved*/
        };
        uint32_t val;
    } timing_cali;
    union {
        struct {
            uint32_t din0_mode                     :    3;  /*SPI_D input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din1_mode                     :    3;  /*SPI_Q input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din2_mode                     :    3;  /*SPI_WP input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din3_mode                     :    3;  /*SPI_HD input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din4_mode                     :    3;  /*SPI_IO4 input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din5_mode                     :    3;  /*SPI_IO5 input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din6_mode                     :    3;  /*SPI_IO6 input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t din7_mode                     :    3;  /*SPI_IO7 input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dins_mode                     :    3;  /*SPI_DQS input delay mode. 0: No delay. 1: Delay for (SPI_MEM_DINS_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_MEM_DINS_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_MEM_DINS_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_MEM_DINS_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_MEM_DINS_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t reserved27                    :    5;  /*reserved*/
        };
        uint32_t val;
    } din_mode;
    union {
        struct {
            uint32_t din0_num                      :    2;  /*SPI_D input delay number.*/
            uint32_t din1_num                      :    2;  /*SPI_Q input delay number.*/
            uint32_t din2_num                      :    2;  /*SPI_WP input delay number.*/
            uint32_t din3_num                      :    2;  /*SPI_HD input delay number.*/
            uint32_t din4_num                      :    2;  /*SPI_IO4 input delay number.*/
            uint32_t din5_num                      :    2;  /*SPI_IO5 input delay number.*/
            uint32_t din6_num                      :    2;  /*SPI_IO6 input delay number.*/
            uint32_t din7_num                      :    2;  /*SPI_IO7 input delay number.*/
            uint32_t dins_num                      :    2;  /*SPI_DQS input delay number.*/
            uint32_t reserved18                    :    14;  /*reserved*/
        };
        uint32_t val;
    } din_num;
    union {
        struct {
            uint32_t dout0_mode                    :    1;  /*SPI_D output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout1_mode                    :    1;  /*SPI_Q output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout2_mode                    :    1;  /*SPI_WP output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout3_mode                    :    1;  /*SPI_HD output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout4_mode                    :    1;  /*SPI_IO4 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout5_mode                    :    1;  /*SPI_IO5 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout6_mode                    :    1;  /*SPI_IO6 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t dout7_mode                    :    1;  /*SPI_IO7 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t douts_mode                    :    1;  /*SPI_DQS output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t reserved9                     :    23;  /*reserved*/
        };
        uint32_t val;
    } dout_mode;
    uint32_t reserved_b8;
    union {
        struct {
            uint32_t smem_timing_clk_ena           :    1;  /*Set this bit to power on HCLK. When PLL is powered on, the frequency of HCLK equals to that of PLL. Otherwise, the frequency equals to that of XTAL.*/
            uint32_t smem_timing_cali              :    1;  /*Set this bit to add extra SPI_CLK cycles in DUMMY phase for all reading operations.*/
            uint32_t smem_extra_dummy_cyclelen     :    3;  /*Extra SPI_CLK cycles added in DUMMY phase for timing compensation, when SPI0 accesses to Ext_RAM. Active when SPI_SMEM_TIMING_CALI bit is set.*/
            uint32_t reserved5                     :    27;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_timing_cali;
    union {
        struct {
            uint32_t smem_din0_mode                :    3;  /*SPI_D input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din1_mode                :    3;  /*SPI_Q input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din2_mode                :    3;  /*SPI_WP input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din3_mode                :    3;  /*SPI_HD input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din4_mode                :    3;  /*SPI_IO4 input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din5_mode                :    3;  /*SPI_IO5 input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din6_mode                :    3;  /*SPI_IO6 input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_din7_mode                :    3;  /*SPI_IO7 input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DIN$n_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dins_mode                :    3;  /*SPI_DQS input delay mode. 0: No delay. 1: Delay for (SPI_SMEM_DINS_NUM+1) cycles at MSPI_CORE_CLK negative edge. 2: Delay for (SPI_SMEM_DINS_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK positive edge. 3: Delay for (SPI_SMEM_DINS_NUM+1) cycles at HCLK positive edge and one cycle at MSPI_CORE_CLK negative edge. 4: Delay for (SPI_SMEM_DINS_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK positive edge. 5: Delay for (SPI_SMEM_DINS_NUM+1) cycles at HCLK negative edge and one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t reserved27                    :    5;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_din_mode;
    union {
        struct {
            uint32_t smem_din0_num                 :    2;  /*SPI_D input delay number.*/
            uint32_t smem_din1_num                 :    2;  /*SPI_Q input delay number.*/
            uint32_t smem_din2_num                 :    2;  /*SPI_WP input delay number.*/
            uint32_t smem_din3_num                 :    2;  /*SPI_HD input delay number.*/
            uint32_t smem_din4_num                 :    2;  /*SPI_IO4 input delay number.*/
            uint32_t smem_din5_num                 :    2;  /*SPI_IO5 input delay number.*/
            uint32_t smem_din6_num                 :    2;  /*SPI_IO6 input delay number.*/
            uint32_t smem_din7_num                 :    2;  /*SPI_IO7 input delay number.*/
            uint32_t smem_dins_num                 :    2;  /*SPI_DQS input delay number.*/
            uint32_t reserved18                    :    14;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_din_num;
    union {
        struct {
            uint32_t smem_dout0_mode               :    1;  /*SPI_D output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout1_mode               :    1;  /*SPI_Q output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout2_mode               :    1;  /*SPI_WP output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout3_mode               :    1;  /*SPI_HD output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout4_mode               :    1;  /*SPI_IO4 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout5_mode               :    1;  /*SPI_IO5 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout6_mode               :    1;  /*SPI_IO6 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_dout7_mode               :    1;  /*SPI_IO7 output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t smem_douts_mode               :    1;  /*SPI_DQS output delay mode. 0: No delay. 1: Delay one cycle at MSPI_CORE_CLK negative edge.*/
            uint32_t reserved9                     :    23;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_dout_mode;
    union {
        struct {
            uint32_t ecc_err_int_num               :    8;  /*Set the error times of MSPI ECC read to generate MSPI SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t fmem_ecc_err_int_en           :    1;  /*Set this bit to calculate the error times of MSPI ECC read when accesses to flash.*/
            uint32_t reserved9                     :    23;  /*reserved*/
        };
        uint32_t val;
    } ecc_ctrl;
    uint32_t ecc_err_addr;
    union {
        struct {
            uint32_t reserved0                     :    6;  /*reserved*/
            uint32_t ecc_data_err_bit              :    7;  /*It records the first ECC data error bit number when SPI_FMEM_ECC_ERR_INT_EN/SPI_SMEM_ECC_ERR_INT_EN is set and accessed to flash/Ext_RAM. The value ranges from 0~127, corresponding to the bit number in 16 data bytes. It is cleared by SPI_MEM_ECC_ERR_INT_CLR bit.*/
            uint32_t ecc_chk_err_bit               :    3;  /*When SPI_MEM_ECC_BYTE_ERR is set, these bits show the error bit number of ECC byte.*/
            uint32_t ecc_byte_err                  :    1;  /*It records the first ECC byte error when SPI_FMEM_ECC_ERR_INT_EN/SPI_SMEM_ECC_ERR_INT_EN is set and accessed to flash/Ext_RAM. It is cleared by SPI_MEM_ECC_ERR_INT_CLR bit.*/
            uint32_t ecc_err_cnt                   :    8;  /*This bits show the error times of MSPI ECC read, including ECC byte error and data byte error. It is cleared by when  SPI_MEM_ECC_ERR_INT_CLR bit is set. */
            uint32_t reserved25                    :    7;  /*reserved*/
        };
        uint32_t val;
    } ecc_err_bit;
    uint32_t reserved_d8;
    union {
        struct {
            uint32_t smem_cs_setup                 :    1;  /*Set this bit to keep SPI_CS low when MSPI is in PREP state.*/
            uint32_t smem_cs_hold                  :    1;  /*Set this bit to keep SPI_CS low when MSPI is in DONE state.*/
            uint32_t smem_cs_setup_time            :    5;  /*(cycles-1) of PREP phase by SPI_CLK, which is the SPI_CS setup time. These bits are combined with SPI_MEM_CS_SETUP bit.*/
            uint32_t smem_cs_hold_time             :    5;  /*SPI Bus CS (SPI_CS) signal is delayed to inactive by SPI Bus clock (SPI_CLK), which is the SPI_CS hold time in non-ECC mode. These bits are combined with SPI_MEM_CS_HOLD bit.*/
            uint32_t smem_ecc_cs_hold_time         :    3;  /*SPI_SMEM_CS_HOLD_TIME + SPI_SMEM_ECC_CS_HOLD_TIME is the MSPI CS hold cycles in ECC mode when accesses to external RAM.*/
            uint32_t smem_ecc_skip_page_corner     :    1;  /*1: MSPI skips page corner when accesses to external RAM. 0: Not skip page corner when accesses to external RAM.*/
            uint32_t smem_ecc_16to18_byte_en       :    1;  /*Set this bit to enable MSPI ECC 16 bytes data with 2 ECC bytes mode when accesses to external RAM.*/
            uint32_t reserved17                    :    7;  /*reserved*/
            uint32_t smem_ecc_err_int_en           :    1;  /*Set this bit to calculate the error times of MSPI ECC read when accesses to external RAM.*/
            uint32_t smem_cs_hold_delay            :    6;  /*These bits are used to set the minimum CS high time tSHSL between SPI burst transfer when accesses to external RAM. tSHSL is (SPI_SMEM_CS_HOLD_DELAY[5:0] + 1) MSPI core clock cycles.*/
            uint32_t reserved31                    :    1;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_ac;
    union {
        struct {
            uint32_t fmem_ddr_en                   :    1;  /*1: in ddr mode,  0 in sdr mode*/
            uint32_t fmem_var_dummy                :    1;  /*Set the bit to enable variable dummy cycle in DDR mode.*/
            uint32_t fmem_ddr_rdat_swp             :    1;  /*Set the bit to reorder RX data of the word in DDR mode.*/
            uint32_t fmem_ddr_wdat_swp             :    1;  /*Set the bit to swap TX data of a word in DDR mode.*/
            uint32_t fmem_ddr_cmd_dis              :    1;  /*the bit is used to disable dual edge in CMD phase when ddr mode.*/
            uint32_t fmem_outminbytelen            :    7;  /*It is the minimum output data length in the panda device.*/
            uint32_t fmem_tx_ddr_msk_en            :    1;  /*Set this bit to mask the first or the last byte in MSPI ECC DDR write mode, when accesses to flash.*/
            uint32_t fmem_rx_ddr_msk_en            :    1;  /*Set this bit to mask the first or the last byte in MSPI ECC DDR read mode, when accesses to flash.*/
            uint32_t fmem_usr_ddr_dqs_thd          :    7;  /*The delay number of data strobe which from memory based on SPI_CLK.*/
            uint32_t fmem_ddr_dqs_loop             :    1;  /*1: Use internal signal  as data strobe, the strobe can not be delayed by input timing module. 0: Use input SPI_DQS signal from PAD as data strobe, the strobe can be delayed by input timing module*/
            uint32_t fmem_ddr_dqs_loop_mode        :    1;  /*When SPI_FMEM_DDR_DQS_LOOP and SPI_FMEM_DDR_EN are set, 1: Use internal SPI_CLK  as data strobe. 0: Use internal ~SPI_CLK  as data strobe. Otherwise this bit is not active.*/
            uint32_t reserved23                    :    1;  /*reserved*/
            uint32_t fmem_clk_diff_en              :    1;  /*Set this bit to enable the differential SPI_CLK#.*/
            uint32_t fmem_hyperbus_mode            :    1;  /*Set this bit to enable the SPI HyperBus mode.*/
            uint32_t fmem_dqs_ca_in                :    1;  /*Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.*/
            uint32_t fmem_hyperbus_dummy_2x        :    1;  /*Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0 accesses to flash or SPI1 accesses flash or sram.*/
            uint32_t fmem_clk_diff_inv             :    1;  /*Set this bit to invert SPI_DIFF when accesses to flash. .*/
            uint32_t fmem_octa_ram_addr            :    1;  /*Set this bit to enable octa_ram address out when accesses to flash, which means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.*/
            uint32_t fmem_hyperbus_ca              :    1;  /*Set this bit to enable HyperRAM address out when accesses to flash, which means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.*/
            uint32_t reserved31                    :    1;  /*reserved*/
        };
        uint32_t val;
    } ddr;
    union {
        struct {
            uint32_t smem_ddr_en                   :    1;  /*1: in ddr mode,  0 in sdr mode*/
            uint32_t smem_var_dummy                :    1;  /*Set the bit to enable variable dummy cycle in spi ddr mode.*/
            uint32_t smem_ddr_rdat_swp             :    1;  /*Set the bit to reorder rx data of the word in spi ddr mode.*/
            uint32_t smem_ddr_wdat_swp             :    1;  /*Set the bit to reorder tx data of the word in spi ddr mode.*/
            uint32_t smem_ddr_cmd_dis              :    1;  /*the bit is used to disable dual edge in CMD phase when ddr mode.*/
            uint32_t smem_outminbytelen            :    7;  /*It is the minimum output data length in the ddr psram.*/
            uint32_t smem_tx_ddr_msk_en            :    1;  /*Set this bit to mask the first or the last byte in MSPI ECC DDR write mode, when accesses to external RAM.*/
            uint32_t smem_rx_ddr_msk_en            :    1;  /*Set this bit to mask the first or the last byte in MSPI ECC DDR read mode, when accesses to external RAM.*/
            uint32_t smem_usr_ddr_dqs_thd          :    7;  /*The delay number of data strobe which from memory based on SPI_CLK.*/
            uint32_t smem_ddr_dqs_loop             :    1;  /*1: Use internal signal  as data strobe, the strobe can not be delayed by input timing module. 0: Use input SPI_DQS signal from PAD as data strobe, the strobe can be delayed by input timing module*/
            uint32_t smem_ddr_dqs_loop_mode        :    1;  /*When SPI_SMEM_DDR_DQS_LOOP and SPI_SMEM_DDR_EN are set, 1: Use internal SPI_CLK  as data strobe. 0: Use internal ~SPI_CLK  as data strobe. Otherwise this bit is not active.*/
            uint32_t reserved23                    :    1;  /*reserved*/
            uint32_t smem_clk_diff_en              :    1;  /*Set this bit to enable the differential SPI_CLK#.*/
            uint32_t smem_hyperbus_mode            :    1;  /*Set this bit to enable the SPI HyperBus mode.*/
            uint32_t smem_dqs_ca_in                :    1;  /*Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.*/
            uint32_t smem_hyperbus_dummy_2x        :    1;  /*Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0 accesses to flash or SPI1 accesses flash or sram.*/
            uint32_t smem_clk_diff_inv             :    1;  /*Set this bit to invert SPI_DIFF when accesses to external RAM. .*/
            uint32_t smem_octa_ram_addr            :    1;  /*Set this bit to enable octa_ram address out when accesses to external RAM, which means ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.*/
            uint32_t smem_hyperbus_ca              :    1;  /*Set this bit to enable HyperRAM address out when accesses to external RAM, which means ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.*/
            uint32_t reserved31                    :    1;  /*reserved*/
        };
        uint32_t val;
    } spi_smem_ddr;
    union {
        struct {
            uint32_t clk_en                        :    1;  /*Register clock gate enable signal. 1: Enable. 0: Disable.*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } clock_gate;
    union {
        struct {
            uint32_t core_clk_sel                  :    2;  /*When the digital system clock selects PLL clock and the frequency of PLL clock is 480MHz, the value of SPI_MEM_CORE_CLK_SEL:  0: SPI0/1 module clock (MSPI_CORE_CLK) is 80MHz. 1: MSPI_CORE_CLK is 120MHz.  2: MSPI_CORE_CLK is 160MHz. 3: MSPI_CORE_CLK is 240MHz. When the digital system clock selects PLL clock and the frequency of PLL clock is 320MHz, the value of SPI_MEM_CORE_CLK_SEL:  0: MSPI_CORE_CLK is 80MHz. 1: MSPI_CORE_CLK is 80MHz.  2: MSPI_CORE_CLK 160MHz. 3: Not used. */
            uint32_t reserved2                     :    30;  /*reserved*/
        };
        uint32_t val;
    } core_clk_sel;
    union {
        struct {
            uint32_t per_end_en                    :    1;  /*The enable bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end_en                    :    1;  /*The enable bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t total_trans_end_en            :    1;  /*The enable bit for SPI_MEM_TOTAL_TRANS_END_INT interrupt.*/
            uint32_t brown_out_en                  :    1;  /*The enable bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t ecc_err_en                    :    1;  /*The enable bit for SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t reserved5                     :    27;  /*reserved*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t per_end_int_clr               :    1;  /*The clear bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end_int_clr               :    1;  /*The clear bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t total_trans_end_int_clr       :    1;  /*The clear bit for SPI_MEM_TOTAL_TRANS_END_INT interrupt.*/
            uint32_t brown_out_int_clr             :    1;  /*The status bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t ecc_err_int_clr               :    1;  /*The clear bit for SPI_MEM_ECC_ERR_INT interrupt. SPI_MEM_ECC_ERR_ADDR and SPI_MEM_ECC_ERR_CNT will be cleared by the pulse of this bit.*/
            uint32_t reserved5                     :    27;  /*reserved*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t per_end_int_raw               :    1;  /*The raw bit for SPI_MEM_PER_END_INT interrupt. 1: Triggered when Auto Resume command (0x7A) is sent and flash is resumed successfully. 0: Others.*/
            uint32_t pes_end_int_raw               :    1;  /*The raw bit for SPI_MEM_PES_END_INT interrupt.1: Triggered when Auto Suspend command (0x75) is sent and flash is suspended successfully. 0: Others.*/
            uint32_t total_trans_end_int_raw       :    1;  /*The raw bit for SPI_MEM_TOTAL_TRANS_END_INT interrupt. 1: Triggered when SPI1 transfer is done and flash is already idle. When WRSR/PP/SE/BE/CE is sent and PES/PER command is sent, this bit is set when WRSR/PP/SE/BE/CE is success.  0: Others.*/
            uint32_t brown_out_int_raw             :    1;  /*The raw bit for SPI_MEM_BROWN_OUT_INT interrupt. 1: Triggered condition is that chip is loosing power and RTC module sends out brown out close flash request to SPI1. After SPI1 sends out suspend command to flash, this interrupt is triggered and MSPI returns to idle state. 0: Others.*/
            uint32_t ecc_err_int_raw               :    1;  /*The raw bit for SPI_MEM_ECC_ERR_INT interrupt. When APB_CTRL_FECC_ERR_INT_EN is set and  APB_CTRL_SECC_ERR_INT_EN is cleared, this bit is triggered when the error times of SPI0/1 ECC read flash are equal or bigger than APB_CTRL_ECC_ERR_INT_NUM. When APB_CTRL_FECC_ERR_INT_EN is cleared and  APB_CTRL_SECC_ERR_INT_EN is set, this bit is triggered when the error times of SPI0/1 ECC read external RAM are equal or bigger than APB_CTRL_ECC_ERR_INT_NUM. When APB_CTRL_FECC_ERR_INT_EN and  APB_CTRL_SECC_ERR_INT_EN are set, this bit is triggered when the total error times of SPI0/1 ECC read external RAM and flash are equal or bigger than APB_CTRL_ECC_ERR_INT_NUM. When APB_CTRL_FECC_ERR_INT_EN and  APB_CTRL_SECC_ERR_INT_EN are cleared, this bit will not be triggered.*/
            uint32_t reserved5                     :    27;  /*reserved*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t per_end_int_st                :    1;  /*The status bit for SPI_MEM_PER_END_INT interrupt.*/
            uint32_t pes_end_int_st                :    1;  /*The status bit for SPI_MEM_PES_END_INT interrupt.*/
            uint32_t total_trans_end_int_st        :    1;  /*The status bit for SPI_MEM_TOTAL_TRANS_END_INT interrupt.*/
            uint32_t brown_out_int_st              :    1;  /*The status bit for SPI_MEM_BROWN_OUT_INT interrupt.*/
            uint32_t ecc_err_int_st                :    1;  /*The status bit for SPI_MEM_ECC_ERR_INT interrupt.*/
            uint32_t reserved5                     :    27;  /*reserved*/
        };
        uint32_t val;
    } int_st;
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
            uint32_t reg_smem_spiclk_fun_drv       :    2;  /*The driver of SPI_CLK PAD  is controlled by the bits SPI_SMEM_SPICLK_FUN_DRV[1:0] when the bit SPI_SPICLK_PAD_DRV_CTL_EN is set and MSPI accesses to external RAM.*/
            uint32_t fmem_spiclk_fun_drv           :    2;  /*The driver of SPI_CLK PAD  is controlled by the bits SPI_FMEM_SPICLK_FUN_DRV[1:0] when the bit SPI_SPICLK_PAD_DRV_CTL_EN is set and MSPI accesses to flash.*/
            uint32_t reg_spiclk_pad_drv_ctl_en     :    1;  /*SPI_CLK PAD driver control signal. 1: The driver of SPI_CLK PAD  is controlled by the bits SPI_FMEM_SPICLK_FUN_DRV[1:0] and SPI_SMEM_SPICLK_FUN_DRV[1:0]. 0: The driver of SPI_CLK PAD  is controlled by the bits IO_MUX_FUNC_DRV[1:0] of SPICLK PAD.*/
            uint32_t date                          :    23;  /*SPI register version.*/
            uint32_t reserved28                    :    4;  /*reserved*/
        };
        uint32_t val;
    } date;
} spi_mem_dev_t;
extern spi_mem_dev_t SPIMEM0;
extern spi_mem_dev_t SPIMEM1;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_SPI_MEM_STRUCT_H_ */
