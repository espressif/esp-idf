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

typedef volatile struct spi_mem_dev_s {
    union {
        struct {
            uint32_t reserved0: 17;                             /*reserved*/
            uint32_t flash_pe:   1;                             /*In user mode  it is set to indicate that program/erase operation will be triggered. The bit is combined with spi_mem_usr bit. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t usr:        1;                             /*User define command enable.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_hpm:  1;                             /*Drive Flash into high performance mode.  The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_res:  1;                             /*This bit combined with reg_resandres bit releases Flash from the power-down state or high performance mode and obtains the devices ID. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_dp:   1;                             /*Drive Flash into power down.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_ce:   1;                             /*Chip erase enable. Chip erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_be:   1;                             /*Block erase enable(32KB) .  Block erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_se:   1;                             /*Sector erase enable(4KB). Sector erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_pp:   1;                             /*Page program enable(1 byte ~256 bytes data to be programmed). Page program operation  will be triggered when the bit is set. The bit will be cleared once the operation done .1: enable 0: disable.*/
            uint32_t flash_wrsr: 1;                             /*Write status register enable.   Write status operation  will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_rdsr: 1;                             /*Read status register-1.  Read status operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_rdid: 1;                             /*Read JEDEC ID . Read ID command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
            uint32_t flash_wrdi: 1;                             /*Write flash disable. Write disable command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
            uint32_t flash_wren: 1;                             /*Write flash enable.  Write enable command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
            uint32_t flash_read: 1;                             /*Read flash enable. Read flash operation will be triggered when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
        };
        uint32_t val;
    } cmd;
    uint32_t addr;                                              /*In user mode  it is the memory address. other then the bit0-bit23 is the memory address  the bit24-bit31 are the byte length of a transfer.*/
    union {
        struct {
            uint32_t reserved0:   3;                            /*reserved*/
            uint32_t fdummy_out:  1;                            /*In the dummy phase the signal level of spi is output by the spi controller.*/
            uint32_t fdout_oct:   1;                            /*Apply 8 signals during write-data phase 1:enable 0: disable*/
            uint32_t fdin_oct:    1;                            /*Apply 8 signals during read-data phase 1:enable 0: disable*/
            uint32_t faddr_oct:   1;                            /*Apply 8 signals during address phase 1:enable 0: disable*/
            uint32_t fcmd_dual:   1;                            /*Apply 2 signals during command phase 1:enable 0: disable*/
            uint32_t fcmd_quad:   1;                            /*Apply 4 signals during command phase 1:enable 0: disable*/
            uint32_t fcmd_oct:    1;                            /*Apply 8 signals during command phase 1:enable 0: disable*/
            uint32_t fcs_crc_en:  1;                            /*For SPI1   initialize crc32 module before writing encrypted data to flash. Active low.*/
            uint32_t tx_crc_en:   1;                            /*For SPI1   enable crc32 when writing encrypted data to flash. 1: enable 0:disable*/
            uint32_t reserved12:  1;                            /*reserved*/
            uint32_t fastrd_mode: 1;                            /*This bit enable the bits: spi_mem_fread_qio  spi_mem_fread_dio  spi_mem_fread_qout and spi_mem_fread_dout. 1: enable 0: disable.*/
            uint32_t fread_dual:  1;                            /*In the read operations  read-data phase apply 2 signals. 1: enable 0: disable.*/
            uint32_t resandres:   1;                            /*The Device ID is read out to SPI_MEM_RD_STATUS register   this bit combine with spi_mem_flash_res bit. 1: enable 0: disable.*/
            uint32_t reserved16:  2;                            /*reserved*/
            uint32_t q_pol:       1;                            /*The bit is used to set MISO line polarity  1: high 0  low*/
            uint32_t d_pol:       1;                            /*The bit is used to set MOSI line polarity  1: high 0  low*/
            uint32_t fread_quad:  1;                            /*In the read operations read-data phase apply 4 signals. 1: enable 0: disable.*/
            uint32_t wp:          1;                            /*Write protect signal output when SPI is idle.  1: output high  0: output low.*/
            uint32_t wrsr_2b:     1;                            /*two bytes data will be written to status register when it is set. 1: enable 0: disable.*/
            uint32_t fread_dio:   1;                            /*In the read operations address phase and read-data phase apply 2 signals. 1: enable 0: disable.*/
            uint32_t fread_qio:   1;                            /*In the read operations address phase and read-data phase apply 4 signals. 1: enable 0: disable.*/
            uint32_t reserved25:  7;                            /*reserved*/
        };
        uint32_t val;
    } ctrl;
    union {
        struct {
            uint32_t clk_mode:        2;                        /*SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.*/
            uint32_t cs_hold_dly_res:12;                        /*Delay cycles of resume Flash when resume Flash from standby mode is enable by spi clock.*/
            uint32_t cs_hold_dly:    12;                        /*SPI fsm is delayed to idle by spi clock cycles.*/
            uint32_t cs_dly_num:      2;                        /*spi_mem_cs signal is delayed by system clock cycles*/
            uint32_t cs_dly_mode:     2;                        /*The cs signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk*/
            uint32_t reserved30:      1;
            uint32_t cs_dly_edge:     1;                        /*The bit is used to select the spi clock edge to modify CS line timing.*/
        };
        uint32_t val;
    } ctrl1;
    union {
        struct {
            uint32_t cs_setup_time:13;                          /*(cycles-1) of prepare phase by spi clock this bits are combined with spi_mem_cs_setup bit.*/
            uint32_t cs_hold_time: 13;                          /*Spi cs signal is delayed to inactive by spi clock this bits are combined with spi_mem_cs_hold bit.*/
            uint32_t reserved26:    5;                          /*reserved*/
            uint32_t sync_reset:    1;                          /*The FSM will be reset.*/
        };
        uint32_t val;
    } ctrl2;
    union {
        struct {
            uint32_t clkcnt_l:       8;                         /*In the master mode it must be equal to spi_mem_clkcnt_N.*/
            uint32_t clkcnt_h:       8;                         /*In the master mode it must be floor((spi_mem_clkcnt_N+1)/2-1).*/
            uint32_t clkcnt_n:       8;                         /*In the master mode it is the divider of spi_mem_clk. So spi_mem_clk frequency is system/(spi_mem_clkcnt_N+1)*/
            uint32_t reserved24:     7;                         /*In the master mode it is pre-divider of spi_mem_clk.*/
            uint32_t clk_equ_sysclk: 1;                         /*reserved*/
        };
        uint32_t val;
    } clock;
    union {
        struct {
            uint32_t reserved0:         6;                      /*reserved*/
            uint32_t cs_hold:           1;                      /*spi cs keep low when spi is in  done  phase. 1: enable 0: disable.*/
            uint32_t cs_setup:          1;                      /*spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable.*/
            uint32_t reserved8:         1;                      /*reserved*/
            uint32_t ck_out_edge:       1;                      /*the bit combined with spi_mem_mosi_delay_mode bits to set mosi signal delay mode.*/
            uint32_t reserved10:        2;                      /*reserved*/
            uint32_t fwrite_dual:       1;                      /*In the write operations read-data phase apply 2 signals*/
            uint32_t fwrite_quad:       1;                      /*In the write operations read-data phase apply 4 signals*/
            uint32_t fwrite_dio:        1;                      /*In the write operations address phase and read-data phase apply 2 signals.*/
            uint32_t fwrite_qio:        1;                      /*In the write operations address phase and read-data phase apply 4 signals.*/
            uint32_t reserved16:        8;                      /*reserved*/
            uint32_t usr_miso_highpart: 1;                      /*read-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable.*/
            uint32_t usr_mosi_highpart: 1;                      /*write-data phase only access to high-part of the buffer spi_mem_w8~spi_mem_w15. 1: enable 0: disable.*/
            uint32_t usr_dummy_idle:    1;                      /*spi clock is disable in dummy phase when the bit is enable.*/
            uint32_t usr_mosi:          1;                      /*This bit enable the write-data phase of an operation.*/
            uint32_t usr_miso:          1;                      /*This bit enable the read-data phase of an operation.*/
            uint32_t usr_dummy:         1;                      /*This bit enable the dummy phase of an operation.*/
            uint32_t usr_addr:          1;                      /*This bit enable the address phase of an operation.*/
            uint32_t usr_command:       1;                      /*This bit enable the command phase of an operation.*/
        };
        uint32_t val;
    } user;
    union {
        struct {
            uint32_t usr_dummy_cyclelen: 8;                     /*The length in spi_mem_clk cycles of dummy phase. The register value shall be (cycle_num-1).*/
            uint32_t reserved8:         18;                     /*reserved*/
            uint32_t usr_addr_bitlen:    6;                     /*The length in bits of address phase. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } user1;
    union {
        struct {
            uint32_t usr_command_value: 16;                     /*The value of  command.*/
            uint32_t reserved16:        12;                     /*reserved*/
            uint32_t usr_command_bitlen: 4;                     /*The length in bits of command phase. The register value shall be (bit_num-1)*/
        };
        uint32_t val;
    } user2;
    union {
        struct {
            uint32_t usr_mosi_bit_len:11;                       /*The length in bits of write-data. The register value shall be (bit_num-1).*/
            uint32_t reserved11:      21;                       /*reserved*/
        };
        uint32_t val;
    } mosi_dlen;
    union {
        struct {
            uint32_t usr_miso_bit_len:11;                       /*The length in bits of  read-data. The register value shall be (bit_num-1).*/
            uint32_t reserved11:      21;                       /*reserved*/
        };
        uint32_t val;
    } miso_dlen;
    union {
        struct {
            uint32_t status:    16;                             /*The value is stored when set spi_mem_flash_rdsr bit and spi_mem_flash_res bit.*/
            uint32_t wb_mode:    8;                             /*Mode bits in the flash fast read mode  it is combined with spi_mem_fastrd_mode bit.*/
            uint32_t reserved24: 8;                             /*reserved*/
        };
        uint32_t val;
    } rd_status;
    uint32_t ext_addr;                                          /*The register are the higher 32bits in the 64 bits address mode.*/
    union {
        struct {
            uint32_t cs0_dis:           1;                      /*SPI CS0 pin enable  1: disable CS0  0: spi_mem_cs0 signal is from/to CS0 pin*/
            uint32_t cs1_dis:           1;                      /*SPI CS1 pin enable  1: disable CS1  0: spi_mem_cs1 signal is from/to CS1 pin*/
            uint32_t reserved2:         1;                      /*reserved*/
            uint32_t trans_end:         1;                      /*The bit is used to indicate the transimitting is done.*/
            uint32_t trans_end_en:      1;                      /*The bit is used to enable the intterrupt of SPI transmitting done.*/
            uint32_t cs_pol:            2;                      /*In the master mode the bits are the polarity of spi cs line  the value is equivalent to spi_mem_cs ^ spi_mem_master_cs_pol.*/
            uint32_t fsub_pin:          1;                      /*For SPI0   flash is connected to SUBPINs.*/
            uint32_t ssub_pin:          1;                      /*For SPI0   sram is connected to SUBPINs.*/
            uint32_t ck_idle_edge:      1;                      /*1: spi clk line is high when idle     0: spi clk line is low when idle*/
            uint32_t cs_keep_active:    1;                      /*spi cs line keep low when the bit is set.*/
            uint32_t auto_per:          1;                      /*reserved*/
            uint32_t reserved12:       20;                      /*reserved*/
        };
        uint32_t val;
    } misc;
    uint32_t tx_crc;                                            /*For SPI1  the value of crc32.*/
    union {
        struct {
            uint32_t req_en:              1;                    /*For SPI0  Cache access enable  1: enable  0:disable.*/
            uint32_t usr_cmd_4byte:       1;                    /*For SPI0   cache  read flash with 4 bytes command  1: enable  0:disable.*/
            uint32_t flash_usr_cmd:       1;                    /*For SPI0   cache  read flash for user define command  1: enable  0:disable.*/
            uint32_t fdin_dual:           1;                    /*For SPI0 flash  din phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_fread_dio.*/
            uint32_t fdout_dual:          1;                    /*For SPI0 flash  dout phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_fread_dio.*/
            uint32_t faddr_dual:          1;                    /*For SPI0 flash  address phase apply 2 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_dio.*/
            uint32_t fdin_quad:           1;                    /*For SPI0 flash  din phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.*/
            uint32_t fdout_quad:          1;                    /*For SPI0 flash  dout phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.*/
            uint32_t faddr_quad:          1;                    /*For SPI0 flash  address phase apply 4 signals. 1: enable 0: disable.  The bit is the same with spi_mem_fread_qio.*/
            uint32_t reserved9:          23;                    /*reserved*/
        };
        uint32_t val;
    } cache_fctrl;
    union {
        struct {
            uint32_t usr_scmd_4byte:       1;                   /*For SPI0  In the spi sram mode  cache read flash with 4 bytes command  1: enable  0:disable.*/
            uint32_t usr_sram_dio:         1;                   /*For SPI0  In the spi sram mode  spi dual I/O mode enable  1: enable  0:disable*/
            uint32_t usr_sram_qio:         1;                   /*For SPI0  In the spi sram mode  spi quad I/O mode enable  1: enable  0:disable*/
            uint32_t usr_wr_sram_dummy:    1;                   /*For SPI0  In the spi sram mode  it is the enable bit of dummy phase for write operations.*/
            uint32_t usr_rd_sram_dummy:    1;                   /*For SPI0  In the spi sram mode  it is the enable bit of dummy phase for read operations.*/
            uint32_t cache_sram_usr_rcmd:  1;                   /*For SPI0  In the spi sram mode cache read sram for user define command.*/
            uint32_t sram_rdummy_cyclelen: 8;                   /*For SPI0  In the sram mode  it is the length in bits of read dummy phase. The register value shall be (bit_num-1).*/
            uint32_t sram_addr_bitlen:     6;                   /*For SPI0  In the sram mode  it is the length in bits of address phase. The register value shall be (bit_num-1).*/
            uint32_t cache_sram_usr_wcmd:  1;                   /*For SPI0  In the spi sram mode cache write sram for user define command*/
            uint32_t sram_oct:             1;                   /*reserved*/
            uint32_t sram_wdummy_cyclelen: 8;                   /*For SPI0  In the sram mode  it is the length in bits of write dummy phase. The register value shall be (bit_num-1).*/
            uint32_t reserved30:           2;                   /*reserved*/
        };
        uint32_t val;
    } cache_sctrl;
    union {
        struct {
            uint32_t sclk_mode:  2;                             /*SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on.*/
            uint32_t swb_mode:   8;                             /*Mode bits in the psram fast read mode  it is combined with spi_mem_fastrd_mode bit.*/
            uint32_t sdin_dual:  1;                             /*For SPI0 sram  din phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_dio.*/
            uint32_t sdout_dual: 1;                             /*For SPI0 sram  dout phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_dio.*/
            uint32_t saddr_dual: 1;                             /*For SPI0 sram  address phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_dio.*/
            uint32_t scmd_dual:  1;                             /*For SPI0 sram  cmd phase apply 2 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_dio.*/
            uint32_t sdin_quad:  1;                             /*For SPI0 sram  din phase apply 4 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_qio.*/
            uint32_t sdout_quad: 1;                             /*For SPI0 sram  dout phase apply 4 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_qio.*/
            uint32_t saddr_quad: 1;                             /*For SPI0 sram  address phase apply 4 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_qio.*/
            uint32_t scmd_quad:  1;                             /*For SPI0 sram  cmd phase apply 4 signals. 1: enable 0: disable. The bit is the same with spi_mem_usr_sram_qio.*/
            uint32_t sdin_oct:   1;                             /*For SPI0 sram  din phase apply 8 signals. 1: enable 0: disable.*/
            uint32_t sdout_oct:  1;                             /*For SPI0 sram  dout phase apply 8 signals. 1: enable 0: disable.*/
            uint32_t saddr_oct:  1;                             /*For SPI0 sram  address phase apply 4 signals. 1: enable 0: disable.*/
            uint32_t scmd_oct:   1;                             /*For SPI0 sram  cmd phase apply 8 signals. 1: enable 0: disable.*/
            uint32_t sdummy_out: 1;                             /*In the dummy phase the signal level of spi is output by the spi controller.*/
            uint32_t reserved23: 9;                             /*reserved*/
        };
        uint32_t val;
    } sram_cmd;
    union {
        struct {
            uint32_t usr_rd_cmd_value:            16;           /*For SPI0 When cache mode is enable it is the read command value of command phase for sram.*/
            uint32_t reserved16:                  12;           /*reserved*/
            uint32_t usr_rd_cmd_bitlen:            4;           /*For SPI0 When cache mode is enable it is the length in bits of command phase for sram. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } sram_drd_cmd;
    union {
        struct {
            uint32_t usr_wr_cmd_value:            16;           /*For SPI0 When cache mode is enable it is the write command value of command phase for sram.*/
            uint32_t reserved16:                  12;           /*reserved*/
            uint32_t usr_wr_cmd_bitlen:            4;           /*For SPI0 When cache mode is enable it is the in bits of command phase  for sram. The register value shall be (bit_num-1).*/
        };
        uint32_t val;
    } sram_dwr_cmd;
    union {
        struct {
            uint32_t cnt_l:           8;                        /*For SPI0 sram interface  it must be equal to spi_mem_clkcnt_N.*/
            uint32_t cnt_h:           8;                        /*For SPI0 sram interface  it must be floor((spi_mem_clkcnt_N+1)/2-1).*/
            uint32_t cnt_n:           8;                        /*For SPI0 sram interface  it is the divider of spi_mem_clk. So spi_mem_clk frequency is system/(spi_mem_clkcnt_N+1)*/
            uint32_t reserved24:      7;                        /*reserved*/
            uint32_t equ_sysclk:      1;                        /*For SPI0 sram interface  1: spi_mem_clk is eqaul to system 0: spi_mem_clk is divided from system clock.*/
        };
        uint32_t val;
    } sram_clk;
    union {
        struct {
            uint32_t st:         3;                             /*The status of spi state machine. 0: idle state  1: preparation state  2: send command state  3: send data state  4: red data state  5:write data state  6: wait state  7: done state.*/
            uint32_t reserved3: 29;                             /*reserved*/
        };
        uint32_t val;
    } fsm;
    uint32_t data_buf[16];                                                /*data buffer*/
    union {
        struct {
            uint32_t waiti_en:             1;                   /*auto-waiting flash idle operation when program flash or erase flash. 1: enable 0: disable.*/
            uint32_t waiti_dummy:          1;                   /*The dummy phase enable when auto wait flash idle*/
            uint32_t waiti_cmd:            8;                   /*The command to auto wait idle*/
            uint32_t waiti_dummy_cyclelen: 8;                   /*The dummy cycle length when auto wait flash idle*/
            uint32_t reserved18:          14;                   /*reserved*/
        };
        uint32_t val;
    } flash_waiti_ctrl;
    union {
        struct {
            uint32_t flash_per:  1;                             /*program erase resume bit  program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t flash_pes:  1;                             /*program erase suspend bit  program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            uint32_t reserved2: 30;
        };
        uint32_t val;
    } flash_sus_cmd;
    union {
        struct {
            uint32_t flash_pes_en:      1;                      /*Auto-suspending enable*/
            uint32_t flash_per_command: 8;                      /*Program/Erase resume command.*/
            uint32_t flash_pes_command: 8;                      /*Program/Erase suspend command.*/
            uint32_t reserved17:       15;
        };
        uint32_t val;
    } flash_sus_ctrl;
    union {
        struct {
            uint32_t flash_sus:  1;                             /*The status of flash suspend  only used in SPI1.*/
            uint32_t reserved1: 31;
        };
        uint32_t val;
    } sus_status;
    union {
        struct {
            uint32_t timing_clk_ena:       1;                   /*The bit is used to enable timing adjust clock for all reading operations.*/
            uint32_t timing_cali:          1;                   /*The bit is used to enable timing auto-calibration for all reading operations.*/
            uint32_t extra_dummy_cyclelen: 3;                   /*add extra dummy spi clock cycle length for spi clock calibration.*/
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } timing_cali;
    union {
        struct {
            uint32_t din0_mode:  2;                             /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t din1_mode:  2;                             /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t din2_mode:  2;                             /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t din3_mode:  2;                             /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t din4_mode:  2;                             /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
            uint32_t din5_mode:  2;                             /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
            uint32_t din6_mode:  2;                             /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
            uint32_t din7_mode:  2;                             /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
            uint32_t dins_mode:  2;                             /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb  3: input with the spi_clk*/
            uint32_t reserved18:14;                             /*reserved*/
        };
        uint32_t val;
    } din_mode;
    union {
        struct {
            uint32_t din0_num:   3;                             /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din1_num:   3;                             /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din2_num:   3;                             /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din3_num:   3;                             /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din4_num:   3;                             /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din5_num:   3;                             /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din6_num:   3;                             /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t din7_num:   3;                             /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dins_num:   3;                             /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t reserved27: 5;                             /*reserved*/
        };
        uint32_t val;
    } din_num;
    union {
        struct {
            uint32_t dout0_mode: 2;                             /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t dout1_mode: 2;                             /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t dout2_mode: 2;                             /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t dout3_mode: 2;                             /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t dout4_mode: 2;                             /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t dout5_mode: 2;                             /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t dout6_mode: 2;                             /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t dout7_mode: 2;                             /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t douts_mode: 2;                             /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the spi_clk*/
            uint32_t reserved18:14;                             /*reserved*/
        };
        uint32_t val;
    } dout_mode;
    union {
        struct {
            uint32_t dout0_num:  2;                             /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout1_num:  2;                             /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout2_num:  2;                             /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout3_num:  2;                             /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout4_num:  2;                             /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout5_num:  2;                             /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout6_num:  2;                             /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t dout7_num:  2;                             /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t douts_num:  2;                             /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t reserved18:14;                             /*reserved*/
        };
        uint32_t val;
    } dout_num;
    union {
        struct {
            uint32_t spi_smem_timing_clk_ena:       1;          /*For sram  the bit is used to enable timing adjust clock for all reading operations.*/
            uint32_t spi_smem_timing_cali:          1;          /*For sram  the bit is used to enable timing auto-calibration for all reading operations.*/
            uint32_t spi_smem_extra_dummy_cyclelen: 3;          /*For sram  add extra dummy spi clock cycle length for spi clock calibration.*/
            uint32_t reserved5:                    27;
        };
        uint32_t val;
    } spi_smem_timing_cali;
    union {
        struct {
            uint32_t spi_smem_din0_mode: 2;                     /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t spi_smem_din1_mode: 2;                     /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t spi_smem_din2_mode: 2;                     /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t spi_smem_din3_mode: 2;                     /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t spi_smem_din4_mode: 2;                     /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t spi_smem_din5_mode: 2;                     /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t spi_smem_din6_mode: 2;                     /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t spi_smem_din7_mode: 2;                     /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t spi_smem_dins_mode: 2;                     /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t reserved18:        14;                     /*reserved*/
        };
        uint32_t val;
    } spi_smem_din_mode;
    union {
        struct {
            uint32_t spi_smem_din0_num: 3;                      /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_din1_num: 3;                      /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_din2_num: 3;                      /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_din3_num: 3;                      /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_din4_num: 3;                      /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_din5_num: 3;                      /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_din6_num: 3;                      /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_din7_num: 3;                      /*the input signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_dins_num: 3;                      /*the input signals are delayed by system clock cycles  0: input without delayed  1: input with the posedge of clk_apb 2 input with the negedge of clk_apb   3: input with the posedge of clk_160  4 input with the negedge of clk_160  5: input with the spi_clk high edge   6: input with the spi_clk low edge*/
            uint32_t reserved27:        5;                      /*reserved*/
        };
        uint32_t val;
    } spi_smem_din_num;
    union {
        struct {
            uint32_t spi_smem_dout0_mode: 2;                    /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout1_mode: 2;                    /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout2_mode: 2;                    /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout3_mode: 2;                    /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout4_mode: 2;                    /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout5_mode: 2;                    /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout6_mode: 2;                    /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout7_mode: 2;                    /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_douts_mode: 2;                    /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t reserved18:         14;                    /*reserved*/
        };
        uint32_t val;
    } spi_smem_dout_mode;
    union {
        struct {
            uint32_t spi_smem_dout0_num: 2;                     /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_dout1_num: 2;                     /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_dout2_num: 2;                     /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_dout3_num: 2;                     /*the output signals are delayed by system clock cycles  0: delayed by 1 cycle  1: delayed by 2 cycles ...*/
            uint32_t spi_smem_dout4_num: 2;                     /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout5_num: 2;                     /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout6_num: 2;                     /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_dout7_num: 2;                     /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t spi_smem_douts_num: 2;                     /*the output signals are delayed by system clock cycles  0: output without delayed  1: output with the posedge of clk_apb 2 output with the negedge of clk_apb  3: output with the posedge of clk_160 4 output with the negedge of clk_160 5: output with the spi_clk high edge  6: output with the spi_clk low edge*/
            uint32_t reserved18:        14;                     /*reserved*/
        };
        uint32_t val;
    } spi_smem_dout_num;
    union {
        struct {
            uint32_t spi_smem_cs_setup:      1;                 /*For spi0  spi cs is enable when spi is in prepare phase. 1: enable 0: disable.*/
            uint32_t spi_smem_cs_hold:       1;                 /*For spi0  spi cs keep low when spi is in done phase. 1: enable 0: disable.*/
            uint32_t spi_smem_cs_setup_time:13;                 /*For spi0  (cycles-1) of prepare phase by spi clock this bits are combined with spi_mem_cs_setup bit.*/
            uint32_t spi_smem_cs_hold_time: 13;                 /*For spi0  spi cs signal is delayed to inactive by spi clock this bits are combined with spi_mem_cs_hold bit.*/
            uint32_t reserved28:             4;
        };
        uint32_t val;
    } spi_smem_ac;
    union {
        struct {
            uint32_t spi_fmem_ddr_en:            1;             /*1: in ddr mode   0 in sdr mode*/
            uint32_t spi_fmem_var_dummy:         1;             /*Set the bit to enable variable dummy cycle in spi ddr mode.*/
            uint32_t spi_fmem_ddr_rdat_swp:      1;             /*Set the bit to reorder rx data of the word in spi ddr mode.*/
            uint32_t spi_fmem_ddr_wdat_swp:      1;             /*Set the bit to reorder tx data of the word in spi ddr mode.*/
            uint32_t spi_fmem_ddr_cmd_dis:       1;             /*the bit is used to disable dual edge in command phase when ddr mode.*/
            uint32_t spi_fmem_outminbytelen:     8;             /*It is the minimum output data length in the panda device.*/
            uint32_t spi_fmem_usr_ddr_dqs_thd:   8;             /*The delay number of data strobe which from memory based on SPI clock.*/
            uint32_t spi_fmem_ddr_dqs_loop:      1;             /*the data strobe is generated by SPI.*/
            uint32_t spi_fmem_ddr_dqs_loop_mode: 2;             /*the bits are combined with the bit spi_fmem_ddr_fdqs_loop which used to select data strobe generating mode in ddr mode.*/
            uint32_t reserved24:                 8;             /*reserved*/
        };
        uint32_t val;
    } ddr;
    union {
        struct {
            uint32_t spi_smem_ddr_en:            1;             /*1: in ddr mode   0 in sdr mode*/
            uint32_t spi_smem_var_dummy:         1;             /*Set the bit to enable variable dummy cycle in spi ddr mode.*/
            uint32_t spi_smem_ddr_rdat_swp:      1;             /*Set the bit to reorder rx data of the word in spi ddr mode.*/
            uint32_t spi_smem_ddr_wdat_swp:      1;             /*Set the bit to reorder tx data of the word in spi ddr mode.*/
            uint32_t spi_smem_ddr_cmd_dis:       1;             /*the bit is used to disable dual edge in command phase when ddr mode.*/
            uint32_t spi_smem_outminbytelen:     8;             /*It is the minimum output data length in the ddr psram.*/
            uint32_t spi_smem_usr_ddr_dqs_thd:   8;             /*The delay number of data strobe which from memory based on SPI clock.*/
            uint32_t spi_smem_ddr_dqs_loop:      1;             /*the data strobe is generated by SPI.*/
            uint32_t spi_smem_ddr_dqs_loop_mode: 2;             /*the bits are combined with the bit spi_smem_ddr_fdqs_loop which used to select data strobe generating mode in ddr mode.*/
            uint32_t reserved24:                 8;             /*reserved*/
        };
        uint32_t val;
    } spi_smem_ddr;
    union {
        struct {
            uint32_t clk_en:     1;                             /*Register clock gate enable signal. 1: Enable. 0: Disable.*/
            uint32_t reserved1: 31;                             /*reserved*/
        };
        uint32_t val;
    } clock_gate;
    union {
        struct {
            uint32_t spi01_clk_sel: 2;                          /*When the digital system clock selects PLL clock and the frequency of PLL clock is 480MHz  the value of reg_spi01_clk_sel:  0: SPI0/1 module clock (clk) is 80MHz. 1: SPI0/1 module clock (clk) is 120MHz.  2: SPI0/1 module clock (clk) 160MHz. 3: Not used. When the digital system clock selects PLL clock and the frequency of PLL clock is 320MHz  the value of reg_spi01_clk_sel:  0: SPI0/1 module clock (clk) is 80MHz. 1: SPI0/1 module clock (clk) is 80MHz.  2: SPI0/1 module clock (clk) 160MHz. 3: Not used.*/
            uint32_t reserved2:    30;                          /*reserved*/
        };
        uint32_t val;
    } spi_core_clk_sel;
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
            uint32_t date:      28;                             /*SPI register version.*/
            uint32_t reserved28: 4;                             /*reserved*/
        };
        uint32_t val;
    } date;
} spi_mem_dev_t;

#ifndef __cplusplus
_Static_assert(sizeof(spi_mem_dev_t) == 0x400, "invalid spi_mem_dev_t size");
#endif

extern spi_mem_dev_t SPIMEM0;
extern spi_mem_dev_t SPIMEM1;
#ifdef __cplusplus
}
#endif
